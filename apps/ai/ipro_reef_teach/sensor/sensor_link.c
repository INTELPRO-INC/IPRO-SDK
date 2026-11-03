/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

/**
 * @file sensor_link.c
 * @brief CDC-ACM line protocol for Sensor Teach. See sensor_link.h.
 *
 * The USB side is voice/voice_link.c's: the OUT endpoint is armed
 * one 64-byte packet at a time into a stream buffer and left unarmed (host
 * NAKed) while the buffer is full; lines go out only while DTR is up; a
 * transfer that would end on a whole packet gets one more byte instead of a
 * zero-length packet. Deploy is ipro7_uvc_cam's uvc_teach_link.c: stage in
 * PSRAM, check the CRC, stop the sensor task, erase/write/verify mfg one
 * sector at a time, answer, drop off the bus and power-on reset.
 */
#include "sensor_link.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <stream_buffer.h>
#include <task.h>

#include "usbd_core.h"
#include "usbd_cdc.h"
#include <compiler/common.h>
#include "system_soc.h"          /* CPU_Interrupt_Disable/Enable, USB_IRQn, L1C_DCACHE_* */
#include "hal_boot2.h"
#include "hal_flash.h"
#include "hal_sys.h"
#include "utils_crc.h"
#include "ipro_vendor_ids.h"
#include "ipro_log.h"
#include "ipro_ai.h"             /* ipro_ai_inm_peek */
#include "ipro_ai_devinfo.h"
#include "ipro_ai_event.h"
#include "ipro_ai_store.h"

#include "sensor_app.h"
#include "sensor_front.h"
#include "sensor_model.h"

#define LOG_TAG        "LINK"

#define CDC_IN_EP      0x81
#define CDC_OUT_EP     0x02
#define CDC_INT_EP     0x83
#define CDC_MPS        64u
#define USBD_PID       0x0721          /* ipro_sensor_teach */
#define USBD_MAX_POWER 100
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

#define RX_STREAM      4096u
#define LINE_MAX       160u
#define TX_BYTES       4160u           /* 65 packets: a DATA line and its payload in one */
#define TX_WAIT_MS     50u             /* DATA / RES: a host that stopped reading costs this */
#define TX_WAIT_KEY_MS 500u
#define LINK_STACK     4096u           /* words: INFER runs the NPU here, which wants 16 KB */
#define LINK_PRIO      (configMAX_PRIORITIES - 5)

#define SNS1_MIN       256u            /* version 1; version 2 is 320 */

static const uint8_t s_desc[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, IPRO_USB_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MPS, 0x02),
    USB_LANGID_INIT(1033),
    /* string 1: manufacturer, IPRO_USB_MANUFACTURER_STR */
    0x1C, USB_DESCRIPTOR_TYPE_STRING,
    'I', 0, 'N', 0, 'T', 0, 'E', 0, 'L', 0, 'P', 0, 'R', 0, 'O', 0, ' ', 0, 'I', 0, 'N', 0,
    'C', 0, '.', 0,
    /* string 2: product */
    0x22, USB_DESCRIPTOR_TYPE_STRING,
    'I', 0, 'P', 0, 'R', 0, 'O', 0, ' ', 0, 'R', 0, 'e', 0, 'e', 0, 'f', 0, ' ', 0, 'S', 0,
    'e', 0, 'n', 0, 's', 0, 'o', 0, 'r', 0,
    /* string 3: serial */
    0x0A, USB_DESCRIPTOR_TYPE_STRING,
    's', 0, 't', 0, '0', 0, '1', 0,
    0x00
};

static struct usbd_interface s_intf0, s_intf1;
static uint8_t s_rx_buf[CDC_MPS] ATTR_EALIGN(32);
static uint8_t s_tx_buf[TX_BYTES] ATTR_EALIGN(32);

static StreamBufferHandle_t s_rx;
static SemaphoreHandle_t    s_tx_free, s_tx_lock;
static volatile bool        s_configured, s_dtr, s_rx_paused, s_tx_reseed;
static volatile uint32_t    s_rx_bytes, s_tx_lines, s_tx_dropped;

/* ---------------------------------------------------------------- USB side */

static void arm_read(void)
{
    usbd_ep_start_read(CDC_OUT_EP, s_rx_buf, CDC_MPS);
}

static void bulk_out_cb(uint8_t ep, uint32_t nbytes)
{
    BaseType_t woken = pdFALSE;

    (void)ep;
    L1C_DCACHE_INVALID_RANGE((uintptr_t)s_rx_buf, sizeof(s_rx_buf));
    if (nbytes) {
        xStreamBufferSendFromISR(s_rx, s_rx_buf, nbytes, &woken);
        s_rx_bytes += nbytes;
    }
    if (xStreamBufferSpacesAvailable(s_rx) >= CDC_MPS) {
        arm_read();
    } else {
        s_rx_paused = true;
    }
    portYIELD_FROM_ISR(woken);
}

static void bulk_in_cb(uint8_t ep, uint32_t nbytes)
{
    BaseType_t woken = pdFALSE;

    (void)ep;
    (void)nbytes;
    xSemaphoreGiveFromISR(s_tx_free, &woken);
    portYIELD_FROM_ISR(woken);
}

static struct usbd_endpoint s_out_ep = { .ep_addr = CDC_OUT_EP, .ep_cb = bulk_out_cb };
static struct usbd_endpoint s_in_ep  = { .ep_addr = CDC_IN_EP,  .ep_cb = bulk_in_cb };

#ifdef REEF_TEACH_COMBINED
/* apps/ai/ipro_reef_teach links three USB apps into one image and calls
 * these through its own handlers (src/reef_usb.c), by mode. */
#define usbd_event_handler   sensor_usbd_event_handler
void usbd_event_handler(uint8_t event);
#define usbd_cdc_acm_set_dtr sensor_usbd_cdc_acm_set_dtr
void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr);
#endif

void usbd_event_handler(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_CONFIGURED:
        if (!s_configured) {                  /* a repeated event must not arm twice */
            s_configured = true;
            s_rx_paused = false;
            arm_read();
        }
        break;
    case USBD_EVENT_RESET:
    case USBD_EVENT_DISCONNECTED:
        s_configured = false;
        s_dtr = false;
        break;
    default:
        break;
    }
}

void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    (void)intf;
    /* Called from the USB interrupt (EP0 class request): flags only. */
    if (dtr && !s_dtr) {
        s_tx_reseed = true;
    }
    s_dtr = dtr;
}

static void rx_resume(void)
{
    bool resume = false;

    taskENTER_CRITICAL();
    if (s_rx_paused && xStreamBufferSpacesAvailable(s_rx) >= CDC_MPS) {
        s_rx_paused = false;
        resume = true;
    }
    taskEXIT_CRITICAL();
    if (resume) {
        CPU_Interrupt_Disable(USB_IRQn);
        arm_read();
        CPU_Interrupt_Enable(USB_IRQn);
    }
}

/* The IN buffer is free once the previous transfer completed: wait for that
 * before writing into it. The caller holds s_tx_lock. */
static int tx_acquire(uint32_t wait_ms)
{
    if (s_tx_reseed) {
        s_tx_reseed = false;
        xSemaphoreTake(s_tx_free, 0);
        xSemaphoreGive(s_tx_free);
    }
    if (!s_configured || !s_dtr) {
        return -1;
    }
    return xSemaphoreTake(s_tx_free, pdMS_TO_TICKS(wait_ms)) == pdTRUE ? 0 : -1;
}

static void tx_go(uint32_t n)
{
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)s_tx_buf, n);
    CPU_Interrupt_Disable(USB_IRQn);
    usbd_ep_start_write(CDC_IN_EP, s_tx_buf, n);
    CPU_Interrupt_Enable(USB_IRQn);
}

static void send_line(const char *line)
{
    size_t n = strlen(line);

    if (!s_configured || !s_dtr || n == 0) {
        return;
    }
    if (n > TX_BYTES - 1u) {
        n = TX_BYTES - 1u;
    }
    xSemaphoreTake(s_tx_lock, portMAX_DELAY);
    /* RES lines are a stream: drop them rather than stall for a host that left */
    if (tx_acquire(strncmp(line, "RES ", 4) ? TX_WAIT_KEY_MS : TX_WAIT_MS) != 0) {
        s_tx_dropped++;
        xSemaphoreGive(s_tx_lock);
        return;
    }
    memcpy(s_tx_buf, line, n);
    if (n % CDC_MPS == 0 && s_tx_buf[n - 1] == '\n') {
        s_tx_buf[n - 1] = '\r';
        s_tx_buf[n++] = '\n';
    }
    tx_go((uint32_t)n);
    s_tx_lines++;
    xSemaphoreGive(s_tx_lock);
}

/* header line + payload + "\n" (+ "\n"), in TX_BYTES transfers. Only the
 * first transfer may be dropped (DATA, short wait): once the header is out
 * the rest waits, so the host never sees half a payload. */
static void send_bin(const char *header, const void *data, uint32_t bytes)
{
    const size_t hn = strlen(header);
    const uint8_t *d = data;
    const uint32_t room = TX_BYTES - 2u;
    uint32_t wait = strncmp(header, "DATA ", 5) ? TX_WAIT_KEY_MS : TX_WAIT_MS;
    bool first = true;

    if (!s_configured || !s_dtr || hn >= room) {
        return;
    }
    xSemaphoreTake(s_tx_lock, portMAX_DELAY);
    for (;;) {
        if (tx_acquire(wait) != 0) {
            s_tx_dropped++;
            if (!first) {
                IPRO_LOGW(LOG_TAG, "host stopped reading in the middle of %s", header);
            }
            break;
        }
        uint32_t n = 0;
        if (first) {
            memcpy(s_tx_buf, header, hn);
            n = (uint32_t)hn;
        }
        const uint32_t take = bytes < room - n ? bytes : room - n;
        memcpy(s_tx_buf + n, d, take);
        n += take;
        d += take;
        bytes -= take;
        if (bytes == 0) {
            s_tx_buf[n++] = '\n';
            if (n % CDC_MPS == 0) {
                s_tx_buf[n++] = '\n';
            }
        }
        tx_go(n);
        first = false;
        wait = TX_WAIT_KEY_MS * 4u;
        if (bytes == 0) {
            s_tx_lines++;
            break;
        }
    }
    xSemaphoreGive(s_tx_lock);
}

/* ---------------------------------------------------------------- commands */

static void reply(const char *fmt, ...)
{
    char line[LINE_MAX + 96];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    send_line(line);
}

/* Read exactly n payload bytes; 0 on success. A 2 s pause in the stream ends it. */
static int read_payload(uint8_t *dst, uint32_t n)
{
    uint32_t got = 0;

    while (got < n) {
        size_t k = xStreamBufferReceive(s_rx, dst + got, n - got, pdMS_TO_TICKS(2000));
        rx_resume();
        if (k == 0) {
            return -1;
        }
        got += (uint32_t)k;
    }
    return 0;
}

/* A refused payload is still read, so it cannot be taken for commands. */
static void drain(uint32_t n)
{
    uint8_t sink[64];

    while (n) {
        const uint32_t k = n < sizeof(sink) ? n : sizeof(sink);
        if (read_payload(sink, k) != 0) {
            return;
        }
        n -= k;
    }
}

static uint8_t *take_payload(uint32_t n)
{
    uint8_t *p = pvPortMalloc(n ? n : 1u);

    if (!p) {
        drain(n);
        return NULL;
    }
    if (read_payload(p, n) != 0) {
        vPortFree(p);
        return NULL;
    }
    return p;
}

static void tinf(void)
{
    const sensor_cfg_t *m = sensor_model_cfg();
    char labels[SM_MAX_CLASSES * SM_LABEL_LEN + 2] = "";

    if (!m) {
        reply("TINF NONE %s\n", sensor_model_why());
        return;
    }
    for (uint32_t k = 0; k < m->n; k++) {
        strcat(labels, m->labels[k]);
        if (k + 1 < m->n) {
            strcat(labels, ",");
        }
    }
    reply("TINF %lu %lu %s %s %lu %lu %lu %s\n", (unsigned long)m->inm_bytes, (unsigned long)m->n,
          labels, m->source, (unsigned long)lrintf(m->rate), (unsigned long)m->front.channels,
          (unsigned long)m->front.window, m->front.mode == SF_SPECTROGRAM ? "spectrogram" : "wave");
}

/* .inm, then a SNS1 block whose own size takes the payload to its end: its version, else 0 */
static uint32_t sns1_version(const uint8_t *b, uint32_t size)
{
    ipro_ai_inm_header_t h;
    uint32_t bytes, version;

    if (ipro_ai_inm_peek(b, size, &h) != IPRO_AI_OK) {
        return 0;
    }
    const uint32_t inm = h.bytes;
    if (inm >= size || size - inm < SNS1_MIN || memcmp(b + inm, "SNS1", 4)) {
        return 0;
    }
    memcpy(&version, b + inm + 4, 4);
    memcpy(&bytes, b + inm + 8, 4);
    return bytes == size - inm ? version : 0;
}

static void tdep(const char *arg)
{
    char *end = NULL;
    const unsigned long size = strtoul(arg, &end, 10);
    const unsigned long want = end ? strtoul(end, NULL, 16) : 0;
    const ipro_ai_slot_t *slot = sensor_model_slot();
    const uint32_t room = ipro_ai_store_room(slot);

    if (!room) {
        drain(size);
        reply("TDEP ERR no_mfg\n");
        return;
    }
    if (size <= SNS1_MIN || size > room) {
        drain(size);
        reply("TDEP ERR size %lu of %lu\n", size, (unsigned long)room);
        return;
    }
    uint8_t *stage = take_payload(size);
    if (!stage) {
        reply("TDEP ERR short\n");
        return;
    }
    const uint32_t crc = utils_crc32(stage, size);
    const uint32_t sns1 = sns1_version(stage, size);
    const char *why = NULL;
    if (crc != want) {
        why = "crc";
    } else if (!sns1) {
        why = "not_a_sensor_model";
    } else if (sensor_app_pause() != 0) {
        why = "busy";
    }
    if (why) {
        vPortFree(stage);
        reply("TDEP ERR %s\n", why);
        return;
    }
    /* Committed last (components/ai/store): a cut leaves the previous model
     * (bank A/B) or none, never half of one. */
    const int rc = ipro_ai_store_install(slot, stage, size, sns1, &why);
    vPortFree(stage);
    if (rc != 0) {
        reply("TDEP ERR %s\n", why);
    } else {
        reply("TDEP OK %lu %08lx\n", size, (unsigned long)crc);
    }
    vTaskDelay(pdMS_TO_TICKS(300));
    /* Pull-up off long enough for the host to see a detach, then a power-on
     * reset (ipro7_uvc_cam: a plain system reset left the board silent once
     * in three deploys). */
    usbd_deinitialize();
    vTaskDelay(pdMS_TO_TICKS(500));
    hal_reboot();
}

typedef struct { const int16_t *x; uint32_t ch; } win_t;

static float win_sample(void *ctx, uint32_t ch, uint32_t i)
{
    const win_t *w = ctx;
    return (float)w->x[i * w->ch + ch];
}

static void front(uint32_t bytes)
{
    const sensor_cfg_t *m = sensor_model_cfg();

    if (!m || bytes != m->front.window * m->front.channels * 2u) {
        drain(bytes);
        reply("FRONT ERR %s\n", m ? "size" : "no_model");
        return;
    }
    uint8_t *x = take_payload(bytes);
    int8_t *img = pvPortMalloc(SF_IMG_BYTES);
    win_t w = { (const int16_t *)x, m->front.channels };
    if (!x || !img || sensor_front(&m->front, win_sample, &w, img) != 0) {
        reply("FRONT ERR failed\n");
    } else {
        send_bin("IMG 32768\n", img, SF_IMG_BYTES);
    }
    vPortFree(x);
    vPortFree(img);
}

static void infer(uint32_t bytes)
{
    if (bytes != SF_IMG_BYTES || !sensor_model_cfg()) {
        drain(bytes);
        reply("INFER ERR %s\n", sensor_model_cfg() ? "size" : "no_model");
        return;
    }
    uint8_t *img = take_payload(bytes);
    int8_t raw[SM_OUT_POS * SM_MAX_CLASSES];
    float z[SM_MAX_CLASSES];
    uint32_t us = 0;
    if (!img || sensor_model_run((const int8_t *)img, z, raw, &us) != 0) {
        reply("INFER ERR failed\n");
    } else {
        char hdr[32];
        snprintf(hdr, sizeof(hdr), "OUT %u %lu\n", (unsigned)sizeof(raw), (unsigned long)us);
        send_bin(hdr, raw, sizeof(raw));
    }
    vPortFree(img);
}

/* SIMCFG [rate <Hz>] [ch <n>] [noise <%>] [drift <%>] [severity <%>] [ramp <s>]:
 * only the fields given change */
static void simcfg(char *arg)
{
    sa_simcfg_t c = { 0 };
    char *save = NULL;

    for (char *key = strtok_r(arg, " ", &save); key; key = strtok_r(NULL, " ", &save)) {
        char *val = strtok_r(NULL, " ", &save);
        if (!val) {
            break;
        }
        const long v = strtol(val, NULL, 10);
        if (v < 0) {
            continue;
        }
        if (!strcmp(key, "rate")) {
            c.rate = (uint32_t)v;
            c.set |= SA_SET_RATE;
        } else if (!strcmp(key, "ch")) {
            c.channels = (uint32_t)v;
            c.set |= SA_SET_CH;
        } else if (!strcmp(key, "noise")) {
            c.noise_pct = (uint32_t)v;
            c.set |= SA_SET_NOISE;
        } else if (!strcmp(key, "drift")) {
            c.drift_pct = (uint32_t)v;
            c.set |= SA_SET_DRIFT;
        } else if (!strcmp(key, "severity")) {
            c.severity_pct = (uint32_t)v;
            c.set |= SA_SET_SEVERITY;
        } else if (!strcmp(key, "ramp")) {
            c.ramp_s = (uint32_t)v;
            c.set |= SA_SET_RAMP;
        }
    }
    if (sensor_app_simcfg(&c) != 0) {
        reply("SIMCFG ERR busy\n");
    }
}

/* The combined Teach firmware (apps/ai/ipro_reef_teach) runs one Teach at a
 * time and switches between them with a reboot; it defines these. On its own
 * this app is the only one, and APP answers "unsupported". */
__attribute__((weak)) const char *reef_teach_mode(void)
{
    return NULL;
}

__attribute__((weak)) const char *reef_teach_modes(void)
{
    return NULL;
}

/** 0: switching (the board restarts in a moment), -2: no such mode, other: failed. */
__attribute__((weak)) int reef_teach_switch(const char *mode)
{
    (void)mode;
    return -1;
}

/* APP: which Teach the board runs; APP <mode>: switch to another */
static void app(const char *arg)
{
    const char *now = reef_teach_mode();

    if (!now) {
        reply("APP ERR unsupported\n");
    } else if (!*arg) {
        reply("APP %s %s\n", now, reef_teach_modes());
    } else {
        const int rc = reef_teach_switch(arg);
        reply(rc == 0 ? "APP OK %s\n" : rc == -2 ? "APP ERR bad %s\n" : "APP ERR failed %s\n", arg);
    }
}

/* ---------------------------------------------------------------- V2.0 */

#ifdef REEF_TEACH_COMBINED
#define FW_NAME "ipro_reef_teach"
#else
#define FW_NAME "ipro_sensor_teach"
#endif

/* one key=value line built by components/ai/app/ipro_ai_devinfo.h */
static void reply_kv(char *line, int n, uint32_t size)
{
    if (n <= 0) {
        return;
    }
    if ((uint32_t)n > size - 2u) {
        n = (int)(size - 2u);                            /* cut, but always a whole line */
    }
    line[n] = '\n';
    line[n + 1] = 0;
    send_line(line);
}

static void query(const char *cmd, const char *arg)
{
    char line[LINE_MAX + 64];
    const ipro_ai_slot_t *slot = sensor_model_slot();
    int n;

    if (!strcmp(cmd, "VER")) {
        n = ipro_ai_devinfo_ver(line, sizeof(line) - 2u, "VER", "sensor", FW_NAME);
    } else if (!strcmp(cmd, "CAPS")) {
        n = ipro_ai_devinfo_caps(line, sizeof(line) - 2u, "CAPS", "sensor", ipro_ai_store_room(slot),
                                 slot->banks, reef_teach_modes(),
                                 slot->banks == 2 ? "run,bench,model,del,rollback,evt"
                                                  : "run,bench,model,del,evt");
    } else if (!strcmp(cmd, "MODEL")) {
        n = sensor_model_describe(line, sizeof(line) - 2u);
    } else {
        n = sensor_model_bench(line, sizeof(line) - 2u, (uint32_t)strtoul(arg, NULL, 10));
    }
    reply_kv(line, n, sizeof(line));
}

/* RUN <bytes>: the model on a raw input tensor (offset-binary bytes) */
static void run_raw(uint32_t bytes)
{
    uint8_t out[SM_OUT_POS * 16u];
    uint32_t out_bytes = 0, us = 0;
    char head[40];

    if (!sensor_model_cfg() || bytes != SF_IMG_BYTES) {
        drain(bytes);
        reply("RUN ERR %s\n", sensor_model_cfg() ? "size" : "no_model");
        return;
    }
    uint8_t *in = take_payload(bytes);
    const int rc = in ? sensor_model_run_raw(in, bytes, out, sizeof(out), &out_bytes, &us) : -3;
    vPortFree(in);
    if (rc != 0) {
        reply("RUN ERR %s\n", rc == -3 ? "short" : rc == -2 ? "size" : "failed");
        return;
    }
    snprintf(head, sizeof(head), "RUN %lu %lu\n", (unsigned long)out_bytes, (unsigned long)us);
    send_bin(head, out, out_bytes);
}

/* DEL and ROLLBACK change the slot and restart, as a deploy does */
static void restart(void)
{
    vTaskDelay(pdMS_TO_TICKS(300));
    usbd_deinitialize();
    vTaskDelay(pdMS_TO_TICKS(500));
    hal_reboot();
}

static void del_or_rollback(bool rollback)
{
    const char *why = NULL;
    uint8_t bank = 0;
    uint32_t seq = 0;

    if (rollback && sensor_model_slot()->banks != 2) {
        reply("ROLLBACK ERR %s\n", "single_bank");      /* nothing written, no restart */
        return;
    }
    if (sensor_app_pause() != 0) {
        reply("%s ERR busy\n", rollback ? "ROLLBACK" : "DEL");
        return;
    }
    if (rollback) {
        if (ipro_ai_store_rollback(sensor_model_slot(), &bank, &seq, &why) != 0) {
            reply("ROLLBACK ERR %s\n", why);
            if (strcmp(why, "erase") != 0) {
                sensor_app_resume();                     /* refused: nothing written, go on */
                return;
            }
        } else {
            reply("ROLLBACK OK bank=%c seq=%lu\n", 'A' + bank, (unsigned long)seq);
        }
    } else if (ipro_ai_store_remove(sensor_model_slot(), &why) != 0) {
        reply("DEL ERR %s\n", why);
    } else {
        reply("DEL OK\n");
    }
    restart();
}

/* EVT ON|OFF: results from the event queue as EVT lines */
static int s_evt_sub = -1;

static void evt_line(const ipro_ai_event_t *e, void *ctx)
{
    char line[LINE_MAX];
    (void)ctx;
    reply_kv(line, ipro_ai_event_format(e, line, sizeof(line) - 2u), sizeof(line));
}

static void evt(const char *arg)
{
    const bool on = !strcmp(arg, "ON");
    if (on && s_evt_sub < 0) {
        s_evt_sub = ipro_ai_event_subscribe(IPRO_AI_EVT_ALL, evt_line, NULL);
    } else if (!on && s_evt_sub >= 0) {
        ipro_ai_event_unsubscribe(s_evt_sub);
        s_evt_sub = -1;
    }
    reply(on && s_evt_sub < 0 ? "EVT ERR %s\n" : "EVT %s\n", on ? "ON" : "OFF");
}

static void dispatch(char *line)
{
    char *arg = strchr(line, ' ');

    if (arg) {
        *arg++ = 0;
    } else {
        arg = line + strlen(line);
    }
    if (!strcmp(line, "APP")) {
        app(arg);
    } else if (!strcmp(line, "HELLO")) {
        sensor_app_hello();
    } else if (!strcmp(line, "SRC")) {
        if (sensor_app_src(arg) != 0) {
            reply("SRC ERR %s\n", arg);
        }
    } else if (!strcmp(line, "SIM")) {
        if (sensor_app_sim(arg) != 0) {
            reply("SIM ERR %s\n", arg);
        }
    } else if (!strcmp(line, "SRCS")) {
        sensor_app_srcs();
    } else if (!strcmp(line, "SIMS")) {
        sensor_app_sims();
    } else if (!strcmp(line, "SIMCFG")) {
        simcfg(arg);
    } else if (!strcmp(line, "STREAM")) {
        sensor_app_stream(!strcmp(arg, "ON"));
    } else if (!strcmp(line, "LIVE")) {
        sensor_app_live(!strcmp(arg, "ON"));
    } else if (!strcmp(line, "TINF")) {
        tinf();
    } else if (!strcmp(line, "TDEP")) {
        tdep(arg);
    } else if (!strcmp(line, "FRONT")) {
        front((uint32_t)strtoul(arg, NULL, 10));
    } else if (!strcmp(line, "INFER")) {
        infer((uint32_t)strtoul(arg, NULL, 10));
    } else if (!strcmp(line, "VER") || !strcmp(line, "CAPS") || !strcmp(line, "MODEL") ||
               !strcmp(line, "BENCH")) {
        query(line, arg);
    } else if (!strcmp(line, "RUN")) {
        run_raw((uint32_t)strtoul(arg, NULL, 10));
    } else if (!strcmp(line, "DEL") || !strcmp(line, "ROLLBACK")) {
        del_or_rollback(line[0] == 'R');
    } else if (!strcmp(line, "EVT")) {
        evt(arg);
    } else if (line[0]) {
        reply("ERR unknown %s\n", line);
    }
}

static void link_task(void *arg)
{
    static char line[LINE_MAX + 1];
    size_t len = 0;
    bool overlong = false;

    (void)arg;
    for (;;) {
        uint8_t c;
        if (xStreamBufferReceive(s_rx, &c, 1, pdMS_TO_TICKS(200)) == 0) {
            rx_resume();
            continue;
        }
        rx_resume();
        if (c == '\r') {
            continue;
        }
        if (c != '\n') {
            if (len < LINE_MAX) {
                line[len++] = (char)c;
            } else {
                overlong = true;
            }
            continue;
        }
        line[len] = 0;
        if (overlong) {
            reply("ERR line too long\n");
        } else {
            dispatch(line);
        }
        len = 0;
        overlong = false;
    }
}

/* ---------------------------------------------------------------- start */

void sensor_link_print(void)
{
    printf("link    : %s, DTR %s, %lu B in, %lu sent, %lu dropped%s\r\n",
           s_configured ? "configured" : "not configured", s_dtr ? "up" : "down",
           (unsigned long)s_rx_bytes, (unsigned long)s_tx_lines, (unsigned long)s_tx_dropped,
           s_rx_paused ? ", RX paused" : "");
}

int sensor_link_start(void)
{
    s_rx = xStreamBufferCreate(RX_STREAM, 1);
    s_tx_free = xSemaphoreCreateBinary();
    s_tx_lock = xSemaphoreCreateMutex();
    if (!s_rx || !s_tx_free || !s_tx_lock) {
        IPRO_LOGE(LOG_TAG, "out of memory");
        return -1;
    }
    xSemaphoreGive(s_tx_free);
    if (xTaskCreate(link_task, "sensor_link", LINK_STACK, NULL, LINK_PRIO, NULL) != pdPASS) {
        return -1;
    }
    usbd_desc_register(s_desc);
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf1));
    usbd_add_endpoint(&s_out_ep);
    usbd_add_endpoint(&s_in_ep);
    usbd_initialize();
    sensor_app_set_sink(send_line, send_bin);
    IPRO_LOGI(LOG_TAG, "CDC-ACM %04x:%04x up", IPRO_USB_VID, USBD_PID);
    return 0;
}
