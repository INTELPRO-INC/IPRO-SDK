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
 * @file voice_link.c
 * @brief CDC-ACM line protocol for the Teach page. See voice_link.h.
 *
 * The OUT endpoint is always armed for one 64-byte packet, so every packet
 * completes its transfer (a longer request would wait for a short packet the
 * host may never send - apps/test/ipro_ai_link/src/cdc_link.c found that).
 * Packets go into a stream buffer from the interrupt; when it has no room for
 * another packet the endpoint is left unarmed, the host is NAKed, and the
 * task re-arms it once it has drained some.
 *
 * Bytes a previous host left behind (a line cut off by a close) are not
 * flushed: any flush would race the new host's first command. Hosts send a
 * bare "\n" first instead, which ends such a line; it is answered with
 * ERR unknown at worst.
 */

#include "voice_link.h"

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
#include "hal_sys.h"             /* hal_reboot */
#include "utils_crc.h"
#include "ipro_vendor_ids.h"
#include "ipro_log.h"
#include "ipro_ai_devinfo.h"
#include "ipro_ai_event.h"
#include "ipro_ai_store.h"

#include "spk_model.h"
#include "voice.h"

#define LOG_TAG       "LINK"

#define CDC_IN_EP     0x81
#define CDC_OUT_EP    0x02
#define CDC_INT_EP    0x83
#define CDC_MPS       64u
#define USBD_PID      0x0720          /* ipro_reef_diarize */
#define USBD_MAX_POWER 100
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

#define RX_STREAM     4096u
#define LINE_MAX      160u
#define TX_BYTES      2368u
#define TX_WAIT_MS    20u             /* LEVEL: a host that stopped reading costs this */
#define TX_WAIT_KEY_MS 500u           /* every other line: the page waits for them */
/* FEAT runs the log-mel front end on this task, and fbank keeps its frame
 * buffers on the stack (about 6.7 KB): 1536 words overflowed on the first
 * FEAT, 2026-11-01; with FEAT, RUN and BENCH the task peaked at 1920 words. */
#define LINK_STACK    3072u
#define LINK_PRIO     (configMAX_PRIORITIES - 5)

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
    0x20, USB_DESCRIPTOR_TYPE_STRING,
    'I', 0, 'P', 0, 'R', 0, 'O', 0, ' ', 0, 'R', 0, 'e', 0, 'e', 0, 'f', 0, ' ', 0, 'V', 0,
    'o', 0, 'i', 0, 'c', 0, 'e', 0,
    /* string 3: serial */
    0x0A, USB_DESCRIPTOR_TYPE_STRING,
    'r', 0, 'v', 0, '0', 0, '1', 0,
    0x00
};

static struct usbd_interface s_intf0, s_intf1;
static uint8_t s_rx_buf[CDC_MPS] ATTR_EALIGN(32);
static uint8_t s_tx_buf[TX_BYTES] ATTR_EALIGN(32);

static StreamBufferHandle_t s_rx;
static SemaphoreHandle_t    s_tx_free, s_tx_lock;
static volatile bool        s_configured, s_dtr, s_rx_paused;
static volatile bool        s_tx_reseed;      /* DTR came up: a cut-off write is void */
static volatile uint32_t    s_rx_bytes, s_tx_lines, s_tx_dropped;
static int16_t             *s_pcm;

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

/* The only USB user in this app: no event multiplexer needed. */
#ifdef REEF_TEACH_COMBINED
/* apps/ai/ipro_reef_teach links three USB apps into one image and calls
 * these through its own handlers (src/reef_usb.c), by mode. */
#define usbd_event_handler   voice_usbd_event_handler
void usbd_event_handler(uint8_t event);
#define usbd_cdc_acm_set_dtr voice_usbd_cdc_acm_set_dtr
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

/* Web Serial and pyserial raise DTR on open and drop it on close; lines are
 * only sent while it is up, so a closed page never stalls the voice task. */
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

void voice_link_send(const char *line)
{
    size_t n = strlen(line);

    if (!s_configured || !s_dtr || n == 0) {
        return;
    }
    if (n > TX_BYTES - 1u) {
        n = TX_BYTES - 1u;
    }
    xSemaphoreTake(s_tx_lock, portMAX_DELAY);
    if (s_tx_reseed) {
        s_tx_reseed = false;
        xSemaphoreTake(s_tx_free, 0);
        xSemaphoreGive(s_tx_free);
    }
    /* LEVEL and VEC are a stream: a host that stopped reading must not hold the
     * voice task (64 VEC lines leave enroll_finish back to back) */
    const bool stream = !strncmp(line, "LEVEL ", 6) || !strncmp(line, "VEC ", 4);
    const uint32_t wait = stream ? TX_WAIT_MS : TX_WAIT_KEY_MS;
    if (xSemaphoreTake(s_tx_free, pdMS_TO_TICKS(wait)) != pdTRUE) {
        s_tx_dropped++;
        xSemaphoreGive(s_tx_lock);
        return;
    }
    memcpy(s_tx_buf, line, n);
    /* A transfer that is a whole number of packets needs a zero-length packet
     * to end it; one extra '\r' before the '\n' is simpler. */
    if (n % CDC_MPS == 0 && n >= 1 && s_tx_buf[n - 1] == '\n') {
        s_tx_buf[n - 1] = '\r';
        s_tx_buf[n++] = '\n';
    }
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)s_tx_buf, n);
    CPU_Interrupt_Disable(USB_IRQn);
    usbd_ep_start_write(CDC_IN_EP, s_tx_buf, (uint32_t)n);
    CPU_Interrupt_Enable(USB_IRQn);
    s_tx_lines++;
    xSemaphoreGive(s_tx_lock);
}

/* A header line, then exactly n bytes, then "\n" ("\n\n" when the last
 * transfer would end on a whole packet): the text tools' binary replies
 * (docs/edge_ai/06 §5), in TX_BYTES transfers under one hold of the TX lock. */
static void send_bin(const char *header, const uint8_t *data, uint32_t bytes)
{
    const size_t hn = strlen(header);
    const uint32_t room = TX_BYTES - 2u;
    bool first = true;

    if (!s_configured || !s_dtr || hn >= room) {
        return;
    }
    xSemaphoreTake(s_tx_lock, portMAX_DELAY);
    for (;;) {
        if (xSemaphoreTake(s_tx_free, pdMS_TO_TICKS(first ? TX_WAIT_KEY_MS : 4u * TX_WAIT_KEY_MS)) !=
            pdTRUE) {
            s_tx_dropped++;
            break;
        }
        uint32_t n = 0;
        if (first) {
            memcpy(s_tx_buf, header, hn);
            n = (uint32_t)hn;
        }
        const uint32_t take = bytes < room - n ? bytes : room - n;
        memcpy(s_tx_buf + n, data, take);
        n += take;
        data += take;
        bytes -= take;
        if (bytes == 0) {
            s_tx_buf[n++] = '\n';
            if (n % CDC_MPS == 0) {
                s_tx_buf[n++] = '\n';
            }
        }
        L1C_DCACHE_CLEAN_RANGE((uintptr_t)s_tx_buf, n);
        CPU_Interrupt_Disable(USB_IRQn);
        usbd_ep_start_write(CDC_IN_EP, s_tx_buf, n);
        CPU_Interrupt_Enable(USB_IRQn);
        first = false;
        if (bytes == 0) {
            s_tx_lines++;
            break;
        }
    }
    xSemaphoreGive(s_tx_lock);
}

/* ---------------------------------------------------------------- commands */

static void reply(const char *fmt, const char *a, const char *b)
{
    char line[LINE_MAX + 48];

    snprintf(line, sizeof(line), fmt, a, b);
    voice_link_send(line);
}

/* Read exactly n bytes of payload; 0 on success. */
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

/* FEED <bytes>: 16 kHz PCM for a run started with SRC HOST. The payload is
 * always consumed, so a refused feed cannot leave PCM to be read as commands. */
static void feed(uint32_t bytes)
{
    static int16_t chunk[1024];
    bool ok = (bytes % 2u) == 0;

    while (bytes) {
        const uint32_t n = bytes < sizeof(chunk) ? bytes : sizeof(chunk);
        if (read_payload((uint8_t *)chunk, n) != 0) {
            ok = false;
            break;
        }
        if (ok && voice_feed(chunk, n / 2u) != 0) {
            ok = false;
        }
        bytes -= n;
    }
    if (!ok) {
        reply("FEED ERR %s%s\n", "refused", "");
    }
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

/* TDEP <bytes> <crc32 hex> + an SPKB (pack_spkb.py): a new voiceprint model
 * over USB, as the sensor protocol deploys its model. It is checked as the
 * loader would check it, installed through the store (committed last: a cut
 * leaves no model rather than half of one) and the board restarts; the saved
 * speakers belong to the old model and are dropped at the next start. */
static void tdep(const char *arg)
{
    char *end = NULL;
    const unsigned long size = strtoul(arg, &end, 10);
    const unsigned long want = end ? strtoul(end, NULL, 16) : 0;
    const ipro_ai_slot_t *slot = spk_model_slot();
    const uint32_t room = ipro_ai_store_room(slot);
    char a[16], b[16];

    if (!room) {
        drain(size);
        reply("TDEP ERR %s%s\n", "no_mfg", "");
        return;
    }
    if (size < 128u || size > room) {
        drain(size);
        snprintf(a, sizeof(a), "%lu", size);
        snprintf(b, sizeof(b), "%lu", (unsigned long)room);
        reply("TDEP ERR size %s of %s\n", a, b);
        return;
    }
    uint8_t *stage = pvPortMalloc(size);
    if (!stage) {
        drain(size);
        reply("TDEP ERR %s%s\n", "nomem", "");
        return;
    }
    if (read_payload(stage, size) != 0) {
        vPortFree(stage);
        reply("TDEP ERR %s%s\n", "short", "");
        return;
    }
    const uint32_t crc = utils_crc32(stage, size);
    const char *why = NULL;
    if (crc != want) {
        why = "crc";
    } else if (!spk_model_payload_ok(stage, size)) {
        why = "not_a_voice_model";
    }
    if (why) {
        vPortFree(stage);
        reply("TDEP ERR %s%s\n", why, "");
        return;
    }
    voice_stop();
    const int rc = ipro_ai_store_install(slot, stage, size, 1u, &why);
    vPortFree(stage);
    if (rc != 0) {
        reply("TDEP ERR %s%s\n", why, "");
    } else {
        snprintf(a, sizeof(a), "%lu", size);
        snprintf(b, sizeof(b), "%08lx", (unsigned long)crc);
        reply("TDEP OK %s %s\n", a, b);
    }
    vTaskDelay(pdMS_TO_TICKS(300));
    /* Pull-up off long enough for the host to see a detach, then a power-on
     * reset (as the image and sensor deploys do). */
    usbd_deinitialize();
    vTaskDelay(pdMS_TO_TICKS(500));
    hal_reboot();
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
        reply("APP ERR %s%s\n", "unsupported", "");
    } else if (!*arg) {
        reply("APP %s %s\n", now, reef_teach_modes());
    } else {
        const int rc = reef_teach_switch(arg);
        reply(rc == 0 ? "APP OK %s%s\n" : rc == -2 ? "APP ERR bad %s%s\n" : "APP ERR failed %s%s\n",
              arg, "");
    }
}

#ifdef REEF_TEACH_COMBINED
#define FW_NAME "ipro_reef_teach"
#else
#define FW_NAME "ipro_reef_diarize"
#endif
#define RUN_OUT_MAX 8192u

/* one key=value line built by components/ai/app/ipro_ai_devinfo.h */
static void reply_line(char *line, int n, uint32_t size)
{
    if (n <= 0) {
        return;
    }
    if ((uint32_t)n > size - 2u) {
        n = (int)(size - 2u);                            /* cut, but always a whole line */
    }
    line[n] = '\n';
    line[n + 1] = 0;
    voice_link_send(line);
}

/* RUN <bytes>: the trunk on a raw input tensor; FEAT <bytes>: the front end on PCM */
static void run_or_feat(bool feat, const char *arg)
{
    const uint32_t n = (uint32_t)strtoul(arg, NULL, 10);
    const char *word = feat ? "FEAT" : "RUN";
    const uint32_t want = feat ? VOICE_PCM_TEST_BYTES : SPK_MODEL_FRAMES * 80u;
    char head[48];

    if (!spk_model_ready() || n != want) {
        drain(n);
        snprintf(head, sizeof(head), "%s ERR %s\n", word, spk_model_ready() ? "size" : "no_model");
        voice_link_send(head);
        return;
    }
    uint8_t *in = feat ? (uint8_t *)s_pcm : pvPortMalloc(n);
    uint8_t *out = pvPortMalloc(feat ? SPK_MODEL_FRAMES * 80u : RUN_OUT_MAX);
    uint32_t out_bytes = SPK_MODEL_FRAMES * 80u, us = 0;
    int rc = 0;
    if (!in || !out) {
        drain(n);                                        /* the payload is on its way regardless */
        rc = -3;
    } else if (read_payload(in, n) != 0) {
        rc = -5;
    }
    if (rc == 0) {
        rc = feat ? spk_model_feat((const int16_t *)in, n / 2u, out)
                  : spk_model_run_raw(in, n, out, RUN_OUT_MAX, &out_bytes, &us);
    }
    if (rc == 0) {
        if (feat) {
            snprintf(head, sizeof(head), "FEAT %lu\n", (unsigned long)out_bytes);
        } else {
            snprintf(head, sizeof(head), "RUN %lu %lu\n", (unsigned long)out_bytes, (unsigned long)us);
        }
        send_bin(head, out, out_bytes);
    } else {
        snprintf(head, sizeof(head), "%s ERR %s\n", word,
                 rc == -3 ? "nomem" : rc == -5 ? "short" : rc == -2 ? "size" : "failed");
        voice_link_send(head);
    }
    if (!feat) {
        vPortFree(in);
    }
    vPortFree(out);
}

/* EVT ON|OFF: results from the event queue as EVT lines */
static int s_evt_sub = -1;

static void evt_line(const ipro_ai_event_t *e, void *ctx)
{
    char line[LINE_MAX];
    (void)ctx;
    reply_line(line, ipro_ai_event_format(e, line, sizeof(line) - 2u), sizeof(line));
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
    reply(on && s_evt_sub < 0 ? "EVT ERR %s%s\n" : "EVT %s%s\n", on ? "ON" : "OFF", "");
}

/* DEL: the voiceprint model's slot emptied, then a restart */
static void del(void)
{
    const char *why = NULL;
    voice_stop();
    const int rc = ipro_ai_store_remove(spk_model_slot(), &why);
    reply(rc == 0 ? "DEL OK%s%s\n" : "DEL ERR %s%s\n", rc == 0 ? "" : why, "");
    vTaskDelay(pdMS_TO_TICKS(300));
    usbd_deinitialize();
    vTaskDelay(pdMS_TO_TICKS(500));
    hal_reboot();
}

static void query(const char *cmd, const char *arg)
{
    char line[LINE_MAX + 48];
    int n;

    if (!strcmp(cmd, "VER")) {
        n = ipro_ai_devinfo_ver(line, sizeof(line) - 2u, "VER", "voice", FW_NAME);
    } else if (!strcmp(cmd, "CAPS")) {
        n = ipro_ai_devinfo_caps(line, sizeof(line) - 2u, "CAPS", "voice",
                                 ipro_ai_store_room(spk_model_slot()), spk_model_slot()->banks,
                                 reef_teach_modes(), "run,bench,model,del,feat,evt");
    } else if (!strcmp(cmd, "MODEL")) {
        n = spk_model_describe(line, sizeof(line) - 2u);
    } else {
        n = spk_model_bench(line, sizeof(line) - 2u, (uint32_t)strtoul(arg, NULL, 10));
    }
    reply_line(line, n, sizeof(line));
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
        voice_report_hello();
    } else if (!strcmp(line, "LIST")) {
        voice_report_list();
    } else if (!strcmp(line, "BANDS")) {
        voice_report_bands();
    } else if (!strcmp(line, "MODE")) {
        if (strcmp(arg, "STRANGER") && strcmp(arg, "NORMAL")) {
            reply("ERR bad MODE %s%s\n", arg, "");
        } else if (voice_set_mode(!strcmp(arg, "STRANGER")) != 0) {
            reply("ERR busy %s%s\n", "MODE", "");
        }
    } else if (!strcmp(line, "VECS")) {
        voice_set_vectors(!strcmp(arg, "ON"));
        reply("VECS %s%s\n", !strcmp(arg, "ON") ? "ON" : "OFF", "");
    } else if (!strcmp(line, "ENROLL")) {
        char *name = strchr(arg, ' ');
        const unsigned long sec = strtoul(arg, NULL, 10);
        if (!name || voice_enroll(name + 1, (uint32_t)sec) != 0) {
            reply("ENROLL ERR refused %s%s\n", name ? name + 1 : "-", "");
        }
    } else if (!strcmp(line, "LIVE")) {
        if (voice_live(!strcmp(arg, "ON")) != 0) {
            reply("ERR busy %s%s\n", "LIVE", "");
        }
    } else if (!strcmp(line, "STOP")) {
        voice_stop();
    } else if (!strcmp(line, "DELETE")) {
        if (voice_delete(arg) != 0) {
            reply("DELETE ERR refused %s%s\n", arg, "");
        }
    } else if (!strcmp(line, "NAME")) {
        char *name = strchr(arg, ' ');
        if (!name || voice_name_anon(atoi(arg), name + 1) != 0) {
            reply("NAME ERR refused %s%s\n", arg, "");
        }
    } else if (!strcmp(line, "THRESH")) {
        char *b = strchr(arg, ' ');
        if (!b || voice_set_thresholds(strtof(arg, NULL), strtof(b + 1, NULL)) != 0) {
            reply("ERR bad THRESH %s%s\n", arg, "");
        }
    } else if (!strcmp(line, "SRC")) {
        if (voice_set_source(!strcmp(arg, "HOST")) != 0) {
            reply("ERR busy %s%s\n", "SRC", "");
        }
    } else if (!strcmp(line, "FEED")) {
        feed(strtoul(arg, NULL, 10));
    } else if (!strcmp(line, "TDEP")) {
        tdep(arg);
    } else if (!strcmp(line, "EMBED")) {
        const uint32_t n = (uint32_t)strtoul(arg, NULL, 10);
        if (n != VOICE_PCM_TEST_BYTES || read_payload((uint8_t *)s_pcm, n) != 0 ||
            voice_embed_pcm(s_pcm, n / 2u) != 0) {
            reply("EMB ERR %s %s\n", arg, "refused");
        }
    } else if (!strcmp(line, "VER") || !strcmp(line, "CAPS") || !strcmp(line, "MODEL") ||
               !strcmp(line, "BENCH")) {
        query(line, arg);
    } else if (!strcmp(line, "RUN") || !strcmp(line, "FEAT")) {
        run_or_feat(line[0] == 'F', arg);
    } else if (!strcmp(line, "EVT")) {
        evt(arg);
    } else if (!strcmp(line, "DEL")) {
        del();
    } else if (line[0]) {
        reply("ERR unknown %s%s\n", line, "");
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
            reply("ERR line too long %s%s\n", "", "");
        } else {
            dispatch(line);
        }
        len = 0;
        overlong = false;
    }
}

/* ---------------------------------------------------------------- start */

void voice_link_print(void)
{
    printf("link    : %s, DTR %s, %lu B in, %lu lines out, %lu dropped%s\r\n",
           s_configured ? "configured" : "not configured", s_dtr ? "up" : "down",
           (unsigned long)s_rx_bytes, (unsigned long)s_tx_lines, (unsigned long)s_tx_dropped,
           s_rx_paused ? ", RX paused" : "");
}

int voice_link_start(void)
{
    s_rx = xStreamBufferCreate(RX_STREAM, 1);
    s_tx_free = xSemaphoreCreateBinary();
    s_tx_lock = xSemaphoreCreateMutex();
    s_pcm = pvPortMalloc(VOICE_PCM_TEST_BYTES);
    if (!s_rx || !s_tx_free || !s_tx_lock || !s_pcm) {
        IPRO_LOGE(LOG_TAG, "out of memory");
        return -1;
    }
    xSemaphoreGive(s_tx_free);
    if (xTaskCreate(link_task, "voice_link", LINK_STACK, NULL, LINK_PRIO, NULL) != pdPASS) {
        return -1;
    }
    usbd_desc_register(s_desc);
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf1));
    usbd_add_endpoint(&s_out_ep);
    usbd_add_endpoint(&s_in_ep);
    usbd_initialize();
    voice_set_sink(voice_link_send);
    IPRO_LOGI(LOG_TAG, "CDC-ACM %04x:%04x up", IPRO_USB_VID, USBD_PID);
    return 0;
}
