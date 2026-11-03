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
 * @file uvc_teach_link.c
 * @brief Teach model deploy over CDC-ACM, beside the UVC stream. See
 *        uvc_teach_link.h for the wire format.
 *
 * Receive path: the bulk-OUT callback (USB ISR) copies each completed read
 * straight into a PSRAM staging buffer sized for the mfg partition, then arms
 * the next read for exactly what is left (at most RX_BUF_BYTES). Asking for
 * the remainder matters: the controller only completes a read on a short
 * packet or when the requested length is reached, so a tail that is a
 * multiple of 64 but shorter than the request would never complete
 * (apps/test/ipro_ai_link/src/cdc_link.c found that out). Copying in the ISR
 * means there is no stream buffer to overflow and no flow control to get
 * wrong: the next read is armed only after the previous one is copied.
 *
 * Every endpoint call from the ISR (arming reads) is safe on its own; the
 * one task-context call - the reply write - masks USB_IRQn, because
 * cherryusb's endpoint calls take no lock and IPRO7 runs every endpoint
 * through one VDMA (memory: cherryusb-ep-multicontext-race).
 */

#include "uvc_teach_link.h"
#include "teach_model.h"                /* TEACH_MFG_OFFSET / _SIZE */

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "usbd_core.h"
#include "usbd_cdc.h"
#include "hal_boot2.h"
#include "hal_flash.h"
#include "hal_sys.h"
#include "system_soc.h"     /* CPU_Interrupt_Disable/Enable, USB_IRQn */
#include "utils_crc.h"
#include "ipro_ai.h"
#include "ipro_ai_devinfo.h"
#include "ipro_ai_store.h"
#include "compiler/common.h"

#include "teach_model.h"

#define CDC_OUT_EP   UVC_TEACH_LINK_OUT_EP
#define CDC_IN_EP    UVC_TEACH_LINK_IN_EP

#define RX_BUF_BYTES  2048u
#define HDR_BYTES     16u
#define MFG_HEADER    TEACH_MFG_OFFSET  /* the model starts at mfg + this (teach_model.h) */

#define RX_IDLE_MS      2000u     /* a deploy that stops arriving is abandoned */
#define LINK_TASK_STACK 2048
#define LINK_TASK_PRIO  (configMAX_PRIORITIES - 5)   /* below the NPU task */

typedef enum {
    CMD_NONE = 0, CMD_DEPLOY, CMD_INFO, CMD_APP,
    CMD_VER, CMD_CAPS, CMD_MODEL, CMD_BENCH, CMD_RUN, CMD_DEL,   /* V2.0, docs/edge_ai/06 §5 */
} link_cmd_t;

static struct usbd_interface s_intf_ctrl, s_intf_data;

/* DMA targets: OCRAM, aligned (usb_console.c keeps its TX out of PSRAM). */
static uint8_t s_rx_buf[RX_BUF_BYTES] ATTR_EALIGN(32);
static uint8_t s_tx_buf[256] ATTR_EALIGN(32);   /* a TMOD line, or TRUN's header and output */

static uint8_t          *s_stage;          /* PSRAM, mfg room bytes */
static uint32_t          s_room;           /* the slot's room (ipro_ai_store_room) */
static SemaphoreHandle_t s_done;
static SemaphoreHandle_t s_tx_free;        /* previous reply has left */

/* ISR-owned receive state; handed to the task through s_done. */
static volatile link_cmd_t s_cmd;
static uint32_t          s_app;            /* TAPP: mode index, 0xFFFFFFFF = which one; TBEN: runs */
static volatile uint32_t   s_want, s_got, s_crc;
static volatile bool       s_busy;         /* task working on a command: drop input */
static volatile bool       s_configured;
static volatile uint32_t   s_dropped;
static volatile uint32_t   s_reads, s_last_n;   /* completed reads, size of the last */

/* ---------------------------------------------------------------------------
 * USB side (ISR)
 * -------------------------------------------------------------------------*/

static uint32_t next_read_len(void)
{
    if ((s_cmd == CMD_DEPLOY || s_cmd == CMD_RUN) && s_got < s_want) {
        uint32_t left = s_want - s_got;
        return left < RX_BUF_BYTES ? left : RX_BUF_BYTES;
    }
    return RX_BUF_BYTES;
}

static void copy_payload(const uint8_t *p, uint32_t n)
{
    uint32_t room = s_want - s_got;
    if (n > room) {
        s_dropped += n - room;
        n = room;
    }
    memcpy(s_stage + s_got, p, n);
    s_got += n;
}

static void finish(BaseType_t *woken)
{
    s_busy = true;
    xSemaphoreGiveFromISR(s_done, woken);
}

static void bulk_out_cb(uint8_t ep, uint32_t nbytes)
{
    BaseType_t woken = pdFALSE;

    (void)ep;
    s_reads++;
    s_last_n = nbytes;
    L1C_DCACHE_INVALID_RANGE((uintptr_t)s_rx_buf, RX_BUF_BYTES);
    if (s_busy || !s_stage) {
        s_dropped += nbytes;                       /* one command at a time */
    } else if (s_cmd == CMD_DEPLOY || s_cmd == CMD_RUN) {
        copy_payload(s_rx_buf, nbytes);
        if (s_got >= s_want) {
            finish(&woken);
        }
    } else if (nbytes >= HDR_BYTES && (!memcmp(s_rx_buf, "TDEP", 4) || !memcmp(s_rx_buf, "TRUN", 4))) {
        uint32_t size, crc;
        memcpy(&size, s_rx_buf + 4, 4);
        memcpy(&crc, s_rx_buf + 8, 4);
        s_want = size;
        s_got  = 0;
        s_crc  = crc;
        s_cmd  = s_rx_buf[1] == 'D' ? CMD_DEPLOY : CMD_RUN;
        if (size == 0 || size > s_room) {
            finish(&woken);                        /* the task reports the size */
        } else {
            copy_payload(s_rx_buf + HDR_BYTES, nbytes - HDR_BYTES);
            if (s_got >= s_want) {
                finish(&woken);
            }
        }
    } else if (nbytes >= HDR_BYTES && !memcmp(s_rx_buf, "TINF", 4)) {
        s_cmd = CMD_INFO;
        finish(&woken);
    } else if (nbytes >= HDR_BYTES && !memcmp(s_rx_buf, "TAPP", 4)) {
        memcpy(&s_app, s_rx_buf + 4, 4);
        s_cmd = CMD_APP;
        finish(&woken);
    } else if (nbytes >= HDR_BYTES && s_rx_buf[0] == 'T' &&
               (!memcmp(s_rx_buf, "TVER", 4) || !memcmp(s_rx_buf, "TCAP", 4) ||
                !memcmp(s_rx_buf, "TMOD", 4) || !memcmp(s_rx_buf, "TBEN", 4) ||
                !memcmp(s_rx_buf, "TDEL", 4))) {
        memcpy(&s_app, s_rx_buf + 4, 4);
        s_cmd = s_rx_buf[1] == 'V' ? CMD_VER : s_rx_buf[1] == 'C' ? CMD_CAPS
              : s_rx_buf[1] == 'M' ? CMD_MODEL : s_rx_buf[1] == 'B' ? CMD_BENCH : CMD_DEL;
        finish(&woken);
    } else {
        s_dropped += nbytes;                       /* not a header: ignore */
    }
    usbd_ep_start_read(CDC_OUT_EP, s_rx_buf, next_read_len());
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

void uvc_teach_link_event(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_CONFIGURED:
        s_configured = true;
        if (s_tx_free) {
            xSemaphoreGiveFromISR(s_tx_free, NULL);   /* a reply cut off by a reset never completes */
        }
        usbd_ep_start_read(CDC_OUT_EP, s_rx_buf, next_read_len());
        break;
    case USBD_EVENT_RESET:
    case USBD_EVENT_DISCONNECTED:
        s_configured = false;
        if (!s_busy) {
            s_cmd = CMD_NONE;                      /* a half transfer is void */
        }
        break;
    default:
        break;
    }
}

void uvc_teach_link_add(void)
{
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf_ctrl));
    usbd_add_interface(usbd_cdc_acm_init_intf(&s_intf_data));
    usbd_add_endpoint(&s_out_ep);
    usbd_add_endpoint(&s_in_ep);
}

/* ---------------------------------------------------------------------------
 * Task side
 * -------------------------------------------------------------------------*/

/* The command is finished: take the next header. Called BEFORE replying -
 * the host sends its next command as soon as it reads the reply, and a
 * header that arrives while s_busy is still set is dropped, with all the
 * payload behind it (a deploy right after TINF was lost that way). */
static void release(void)
{
    s_cmd  = CMD_NONE;
    s_busy = false;
}

/* One text line to the host (and the console). The only task-context
 * endpoint call: mask the USB ISR around it. */
static void reply(const char *line)
{
    size_t n = strlen(line);

    if (n > sizeof(s_tx_buf) - 1u) {
        n = sizeof(s_tx_buf) - 1u;
    }
    printf("[TEACH] %s", line);
    if (!s_configured) {
        return;
    }
    /* s_tx_buf is the DMA source of the previous reply until it has left:
     * a host that has not read that one for 200 ms does not get this one. */
    if (xSemaphoreTake(s_tx_free, pdMS_TO_TICKS(200)) != pdTRUE) {
        printf("[TEACH] host not reading: reply dropped\r\n");
        return;
    }
    memcpy(s_tx_buf, line, n);
    /* cherryusb sends no zero-length packet on bulk IN: a transfer of whole
     * packets would not end, so "\n" becomes "\r\n" (as the text tools do) */
    if (n % UVC_TEACH_LINK_MPS == 0 && n >= 1 && s_tx_buf[n - 1] == '\n') {
        s_tx_buf[n - 1] = '\r';
        s_tx_buf[n++] = '\n';
    }
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)s_tx_buf, sizeof(s_tx_buf));
    CPU_Interrupt_Disable(USB_IRQn);
    usbd_ep_start_write(CDC_IN_EP, s_tx_buf, (uint32_t)n);
    CPU_Interrupt_Enable(USB_IRQn);
}

/* A header line, then exactly n bytes, then "\n" ("\n\n" when the transfer
 * would end on a whole packet): the binary reply framing of the text tools
 * (docs/edge_ai/06 §5), so one host reader serves all three. */
static void reply_bin(const char *head, const uint8_t *data, uint32_t n)
{
    const size_t h = strlen(head);
    size_t len = h + n + 1u;

    if (len % UVC_TEACH_LINK_MPS == 0) {
        len++;
    }
    if (!s_configured || len > sizeof(s_tx_buf)) {
        printf("[TEACH] reply of %u B does not fit\r\n", (unsigned)len);
        return;
    }
    if (xSemaphoreTake(s_tx_free, pdMS_TO_TICKS(200)) != pdTRUE) {
        printf("[TEACH] host not reading: reply dropped\r\n");
        return;
    }
    memcpy(s_tx_buf, head, h);
    memcpy(s_tx_buf + h, data, n);
    memset(s_tx_buf + h + n, '\n', len - h - n);
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)s_tx_buf, sizeof(s_tx_buf));
    CPU_Interrupt_Disable(USB_IRQn);
    usbd_ep_start_write(CDC_IN_EP, s_tx_buf, (uint32_t)len);
    CPU_Interrupt_Enable(USB_IRQn);
    printf("[TEACH] %s", head);
}

/* teach_model_init() has not finished yet: the link starts first so that a
 * board without a model can still be reached. */
static bool model_loading(void)
{
    return !teach_model_ready() && teach_model_fail_reason()[0] == '\0';
}

/* The combined Teach firmware (apps/ai/ipro_reef_teach) runs one Teach at a
 * time and switches between them with a reboot; it defines these. On its own
 * this app is the only one, and TAPP answers "unsupported". */
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

/* TAPP <index>: switch to mode 0 image, 1 voice, 2 sensor; 0xFFFFFFFF: which one */
static void reply_app(void)
{
    static const char *const modes[] = { "image", "voice", "sensor" };
    const char *now = reef_teach_mode();
    const uint32_t want = s_app;
    char line[96];

    release();
    if (!now) {
        reply("TAPP ERR unsupported\n");
    } else if (want == 0xFFFFFFFFu) {
        snprintf(line, sizeof(line), "TAPP %s %s\n", now, reef_teach_modes());
        reply(line);
    } else if (want >= sizeof(modes) / sizeof(modes[0])) {
        snprintf(line, sizeof(line), "TAPP ERR bad %lu\n", (unsigned long)want);
        reply(line);
    } else {
        const int rc = reef_teach_switch(modes[want]);
        snprintf(line, sizeof(line), rc == 0 ? "TAPP OK %s\n" : "TAPP ERR failed %s\n",
                 modes[want]);
        reply(line);
    }
}

static void reply_info(void)
{
    teach_stats_t st;
    char line[sizeof(s_tx_buf)];
    int used;

    teach_model_get_stats(&st);
    release();
    if (model_loading()) {
        reply("TINF BUSY\n");
        return;
    }
    if (!teach_model_ready()) {
        reply("TINF 0 0\n");
        return;
    }
    used = snprintf(line, sizeof(line), "TINF %lu %u ", (unsigned long)st.model_bytes,
                    st.nclass);
    /* The line is short; labels may be cut, which the host tolerates. */
    for (unsigned k = 0; k < st.nclass && used > 0 && used < (int)sizeof(line) - 2; k++) {
        int n = snprintf(line + used, sizeof(line) - 1u - (size_t)used, "%s%s", k ? "," : "",
                         teach_model_label(k));
        if (n < 0) {
            break;
        }
        used += n;
        if (used > (int)sizeof(line) - 2) {
            used = (int)sizeof(line) - 2;
        }
    }
    line[used] = '\n';
    line[used + 1] = '\0';
    reply(line);
}

static void deploy(void)
{
    const uint32_t size = s_want;
    char line[sizeof(s_tx_buf)];

    if (size == 0 || size > s_room) {
        snprintf(line, sizeof(line), "TDEP ERR size %lu > %lu\n", (unsigned long)size,
                 (unsigned long)s_room);
        release();
        reply(line);
        return;
    }
    if (s_got != size) {
        snprintf(line, sizeof(line), "TDEP ERR short %lu of %lu\n", (unsigned long)s_got,
                 (unsigned long)size);
        release();
        reply(line);
        return;
    }
    uint32_t crc = utils_crc32(s_stage, size);
    if (crc != s_crc) {
        snprintf(line, sizeof(line), "TDEP ERR crc %08lx != %08lx\n", (unsigned long)crc,
                 (unsigned long)s_crc);
        release();
        reply(line);
        return;
    }
    /* Refused before anything is erased: a payload that is not an .inm
     * (the label block after it is optional). */
    ipro_ai_inm_header_t inm;
    if (ipro_ai_inm_peek(s_stage, size, &inm) != IPRO_AI_OK) {
        release();
        reply("TDEP ERR not_a_model\n");
        return;
    }
    /* The loader reads the model over XIP; erasing under it is not an option. */
    if (model_loading()) {
        release();
        reply("TDEP ERR loading\n");
        return;
    }
    if (teach_model_pause(true, 1000) != 0) {
        teach_model_pause(false, 0);
        release();
        reply("TDEP ERR npu busy\n");
        return;
    }
    /* Committed last (components/ai/store): a cut leaves the old model or
     * none - the overlay then says so - never half of one. */
    const char *why = NULL;
    if (ipro_ai_store_install(teach_model_slot(), s_stage, size, TEACH_TRAILER_VERSION, &why) != 0) {
        snprintf(line, sizeof(line), "TDEP ERR %s\n", why);
    } else {
        snprintf(line, sizeof(line), "TDEP OK %lu %08lx\n", (unsigned long)size,
                 (unsigned long)crc);
    }
    reply(line);
    vTaskDelay(pdMS_TO_TICKS(300));
    /* A warm reset keeps the cable attached, and the host never re-enumerated
     * the board after one (its CDC port stayed, "not configured" on the
     * device). Pull-up off long enough for the host to see a detach first. */
    usbd_deinitialize();
    vTaskDelay(pdMS_TO_TICKS(500));
    /* Power-on reset, as the shell's `reboot`: a plain system reset
     * (hal_system_reset) with the ISP, NPU and PSRAM busy left the board
     * silent - no boot log - on one deploy in three. */
    hal_reboot();
}

/* Detach, then a power-on reset: what a deploy ends with (see deploy()). */
static void restart(void)
{
    vTaskDelay(pdMS_TO_TICKS(300));
    usbd_deinitialize();
    vTaskDelay(pdMS_TO_TICKS(500));
    hal_reboot();
}

#ifdef REEF_TEACH_COMBINED
#define FW_NAME "ipro_reef_teach"
#else
#define FW_NAME "ipro7_uvc_cam"
#endif

/* TVER, TCAP, TMOD, TBEN, TDEL: the V2.0 queries (docs/edge_ai/06 §5) */
static void reply_query(link_cmd_t cmd)
{
    char line[sizeof(s_tx_buf) - 2];
    const uint32_t arg = s_app;
    int n;

    release();
    if (cmd == CMD_VER) {
        n = ipro_ai_devinfo_ver(line, sizeof(line), "TVER", "image", FW_NAME);
    } else if (cmd == CMD_CAPS) {
        n = ipro_ai_devinfo_caps(line, sizeof(line), "TCAP", "image", s_room, teach_model_slot()->banks,
                                 reef_teach_modes(), "run,bench,model,del");
    } else if (cmd == CMD_MODEL) {
        n = model_loading() ? snprintf(line, sizeof(line), "TMOD ERR busy")
                            : teach_model_describe(line, sizeof(line), "TMOD");
    } else if (cmd == CMD_BENCH) {
        n = model_loading() ? snprintf(line, sizeof(line), "TBEN ERR busy")
                            : teach_model_bench(line, sizeof(line), arg);
    } else {
        /* TDEL: the camera off the model, the slot emptied, then a restart */
        if (model_loading() || teach_model_pause(true, 1000) != 0) {
            teach_model_pause(false, 0);
            reply("TDEL ERR busy\n");
            return;
        }
        const char *why = NULL;
        n = ipro_ai_store_remove(teach_model_slot(), &why) == 0
                ? snprintf(line, sizeof(line), "TDEL OK")
                : snprintf(line, sizeof(line), "TDEL ERR %s", why);
        strcat(line, "\n");
        reply(line);
        restart();
        return;
    }
    if (n < 0) {
        n = 0;
    }
    if ((size_t)n > sizeof(line) - 2u) {
        n = (int)(sizeof(line) - 2u);                  /* cut, but always a whole line */
    }
    line[n] = '\n';
    line[n + 1] = 0;
    reply(line);
}

/* TRUN: one inference on the staged input tensor; the output in the reply */
static void run_raw(void)
{
    char line[48];
    uint8_t out[160];
    uint32_t out_bytes = 0, us = 0;
    const uint32_t size = s_want;

    if (size == 0 || size > s_room) {
        release();
        reply("TRUN ERR size\n");
        return;
    }
    if (s_got != size) {
        snprintf(line, sizeof(line), "TRUN ERR short %lu of %lu\n", (unsigned long)s_got,
                 (unsigned long)size);
        release();
        reply(line);
        return;
    }
    if (utils_crc32(s_stage, size) != s_crc) {
        release();
        reply("TRUN ERR crc\n");
        return;
    }
    const int rc = model_loading() ? -1 : teach_model_run_raw(s_stage, size, out, sizeof(out), &out_bytes, &us);
    release();
    if (rc != 0) {
        reply(rc == -2 ? "TRUN ERR size\n" : rc == -1 ? "TRUN ERR no_model\n"
              : rc == -5 ? "TRUN ERR busy\n" : rc == -3 ? "TRUN ERR nomem\n" : "TRUN ERR failed\n");
        return;
    }
    snprintf(line, sizeof(line), "TRUN OK %lu %lu\n", (unsigned long)out_bytes, (unsigned long)us);
    reply_bin(line, out, out_bytes);
}

/* A deploy whose data stopped arriving: void it, so the next header is read
 * as a header and not as more payload. */
static void abandon_if_stalled(uint32_t *seen, uint32_t *idle_ms)
{
    char line[sizeof(s_tx_buf)];

    if ((s_cmd != CMD_DEPLOY && s_cmd != CMD_RUN) || s_busy) {
        *seen = 0;
        *idle_ms = 0;
        return;
    }
    if (s_got != *seen) {
        *seen = s_got;
        *idle_ms = 0;
        return;
    }
    *idle_ms += 500u;
    if (*idle_ms < RX_IDLE_MS) {
        return;
    }
    CPU_Interrupt_Disable(USB_IRQn);
    s_busy = true;                                 /* the ISR drops from here */
    uint32_t got = s_got, want = s_want;
    const bool run = s_cmd == CMD_RUN;
    s_cmd = CMD_NONE;
    CPU_Interrupt_Enable(USB_IRQn);
    snprintf(line, sizeof(line), "%s ERR timeout %lu of %lu\n", run ? "TRUN" : "TDEP",
             (unsigned long)got, (unsigned long)want);
    release();
    reply(line);
    *seen = 0;
    *idle_ms = 0;
}

static void link_task(void *arg)
{
    uint32_t seen = 0, idle_ms = 0;

    (void)arg;
    for (;;) {
        if (xSemaphoreTake(s_done, pdMS_TO_TICKS(500)) != pdTRUE) {
            abandon_if_stalled(&seen, &idle_ms);
            continue;
        }
        seen = 0;
        idle_ms = 0;
        if (s_cmd == CMD_DEPLOY) {
            deploy();                              /* releases, or reboots */
        } else if (s_cmd == CMD_INFO) {
            reply_info();
        } else if (s_cmd == CMD_APP) {
            reply_app();
        } else if (s_cmd == CMD_RUN) {
            run_raw();
        } else if (s_cmd >= CMD_VER) {
            reply_query(s_cmd);
        }
        release();
    }
}

void uvc_teach_link_print(void)
{
    static const char *const cmds[] = { "idle", "deploy", "info", "app", "ver", "caps", "model",
                                        "bench", "run", "del" };

    printf("link    : %s, cmd %s, got %lu of %lu, %lu reads (last %lu B), %lu B dropped, "
           "room %lu\r\n", s_configured ? "configured" : "not configured",
           cmds[s_cmd], (unsigned long)s_got, (unsigned long)s_want, (unsigned long)s_reads,
           (unsigned long)s_last_n, (unsigned long)s_dropped, (unsigned long)s_room);
}

int uvc_teach_link_start(void)
{
    s_room = ipro_ai_store_room(teach_model_slot());
    if (!s_room) {
        printf("[TEACH] no mfg partition: deploy over USB disabled\r\n");
        return -1;
    }
    s_stage = pvPortMalloc(s_room);
    s_done  = xSemaphoreCreateBinary();
    s_tx_free = xSemaphoreCreateBinary();
    if (!s_stage || !s_done || !s_tx_free) {
        printf("[TEACH] no PSRAM for a %lu B deploy buffer\r\n", (unsigned long)s_room);
        s_stage = NULL;
        return -1;
    }
    xSemaphoreGive(s_tx_free);
    if (xTaskCreate(link_task, "teach_link", LINK_TASK_STACK, NULL, LINK_TASK_PRIO,
                    NULL) != pdPASS) {
        return -1;
    }
    printf("[TEACH] deploy over USB ready: up to %lu B into mfg+0x%lx\r\n",
           (unsigned long)s_room, (unsigned long)MFG_HEADER);
    return 0;
}
