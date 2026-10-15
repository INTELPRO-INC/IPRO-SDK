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
 * @file gesture.c
 * @brief Hand-gesture classifier on the NPU. See gesture.h.
 *
 * Model source, in order:
 *   1. sd:/models/gesture.inm on the micro-SD (iproai_load_model_from_file,
 *      the whole file read into PSRAM) - so a model swap is a file copy;
 *   2. the "mfg" flash partition over XIP, the path the RPS app proved.
 *
 * Input: a 128x128 window of the ISP channel-0 preview (YUYV 320x180, the
 * sensor at 1/4), converted on the CPU by gesture_push_yuyv() - ISP channel
 * 1 belongs to the person detector. Everything after load is integer:
 * softmax_q for the probabilities, and no printf("%f") on the inference task.
 * The NPU is shared with the detector through npu_share.h.
 */

#include "gesture.h"
#include "softmax_q.h"
#include "npu_share.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include "hal_flash.h"
#include "drv_sf_ctrl.h"
#include "hal_boot2.h"
#include "drv_glb.h"
#include <system_soc.h>

#include <iproai_core.h>
#include <iproai_def.h>
#include "hal_npu.h"

#include "evk.h"
#include "ipro_log.h"

#define LOG_TAG "GESTURE"

#define GESTURE_SD_PATH        "sd:/models/gesture.inm"
#define GESTURE_PARTITION      "mfg"
#define PARTITION_HEADER_SIZE  0x1000u

/* ext_input: 4 bytes per pixel (pad_c = align(3,4)). */
#define GESTURE_FRAME_BYTES    ((uint32_t)GESTURE_IN_W * GESTURE_IN_H * 4u)
#define GESTURE_FRAME_ALIGN    32u

/* Above the display task (polling SPI): the NPU layer dispatch must not
 * queue behind a blit. */
#define GESTURE_TASK_STACK     4096
#define GESTURE_TASK_PRIO      (configMAX_PRIORITIES - 3)

/* components/ai programs the NPU's external-input address from imgBuffer+4
 * at load; hal_npu.c only has a weak NULL. */
char *imgBuffer = NULL;

static const char *s_names[GESTURE_NCLASS] = { "ROCK", "PAPER", "SCISSORS", "NONE" };

static iproai_model_hdl_t         s_hdl;
static struct iproai_net_info_t  *s_net;
static uint8_t                   *s_frame_raw;
static uint32_t                   s_frame;
static QueueHandle_t              s_q;
static SemaphoreHandle_t          s_mtx;
static volatile bool              s_ready, s_busy, s_print;
static const char                *s_fail = "";
static const char                *s_source = "none";
static gesture_feed_t             s_feed = GESTURE_FEED_RAW;
static gesture_result_t           s_result;
static gesture_stats_t            s_stats;
static gesture_done_cb_t          s_done_cb;
static void                      *s_done_arg;

const char *gesture_name(gesture_class_t c)
{
    return ((unsigned)c < GESTURE_NCLASS) ? s_names[c] : "?";
}

const char *gesture_feed_name(gesture_feed_t f)
{
    return f == GESTURE_FEED_RAW ? "raw" : f == GESTURE_FEED_XOR ? "xor" : "copy";
}

/* ---------------------------------------------------------------------------
 * Model load
 * -------------------------------------------------------------------------*/

static const uint8_t *mfg_model(uint32_t *size_out)
{
    uint32_t part_addr = 0, part_size = 0, header[8], model_size = 0;

    if (hal_boot2_partition_addr_active(GESTURE_PARTITION, &part_addr, &part_size) != 0) {
        IPRO_LOGE(LOG_TAG, "partition '%s' not in the flashed table - ./flash.sh --full",
                  GESTURE_PARTITION);
        s_fail = "NO MFG PARTITION";
        return NULL;
    }
    uint32_t blob_phys = part_addr + PARTITION_HEADER_SIZE;
    uint32_t sf_off    = SF_Ctrl_Get_Flash_Image_Offset(0, 0);
    if (blob_phys < sf_off) {
        s_fail = "MODEL BELOW XIP";
        return NULL;
    }
    const uint8_t *xip = (const uint8_t *)(uintptr_t)(FLASH_XIP_BASE + (blob_phys - sf_off));

    memcpy(header, xip, sizeof(header));
    for (int i = 0; i < 8; i++) {
        model_size += header[i];
    }
    model_size += sizeof(header);
    if (model_size <= sizeof(header) || model_size + PARTITION_HEADER_SIZE > part_size) {
        IPRO_LOGE(LOG_TAG, "no valid model in '%s' (size word 0x%08X) - ./flash.sh --model",
                  GESTURE_PARTITION, (unsigned)model_size);
        s_fail = "NO MODEL IN MFG";
        return NULL;
    }
    *size_out = model_size;
    IPRO_LOGI(LOG_TAG, "model: %u B in flash '%s' at 0x%06X", (unsigned)model_size,
              GESTURE_PARTITION, (unsigned)blob_phys);
    return xip;
}

static int load_model(void)
{
    IPROAI_Status_e st = IPROAI_STATUS_INVALID_INPUT;
    TickType_t t0;

    s_hdl = iproai_create();
    if (!s_hdl) {
        s_fail = "NPU CREATE FAILED";
        return -1;
    }

    t0 = xTaskGetTickCount();
#if EVK_HAS_SDCARD
    /* Card first: a model on the card is a model somebody just put there. */
    if (evk_sd_present() && evk_sd_mount() == EVK_SD_OK && evk_sd_exists(GESTURE_SD_PATH)) {
        st = iproai_load_model_from_file(s_hdl, GESTURE_SD_PATH);
        if (st == IPROAI_STATUS_NO_ERROR) {
            s_source = "sd card";
            IPRO_LOGI(LOG_TAG, "model: " GESTURE_SD_PATH);
        } else {
            IPRO_LOGW(LOG_TAG, GESTURE_SD_PATH " failed to load (status %d) - trying flash",
                      (int)st);
            /* The handle may hold partial buffers; start over. */
            iproai_free(s_hdl);
            s_hdl = iproai_create();
            if (!s_hdl) {
                s_fail = "NPU CREATE FAILED";
                return -1;
            }
        }
    }
#endif
    if (st != IPROAI_STATUS_NO_ERROR) {
        uint32_t size = 0;
        const uint8_t *xip = mfg_model(&size);
        if (!xip) {
            goto fail;                       /* s_fail set by mfg_model() */
        }
        s_stats.model_bytes = size;
        st = iproai_load_model_from_buffer(s_hdl, xip);
        if (st != IPROAI_STATUS_NO_ERROR) {
            IPRO_LOGE(LOG_TAG, "NPU load failed (status %d)", (int)st);
            s_fail = "NPU LOAD FAILED";
            goto fail;
        }
        s_source = "flash mfg";
    }
    s_stats.load_ms = (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);

    s_net = iproai_getNetInfo(s_hdl);
    if (!s_net) {
        s_fail = "NO NET INFO";
        goto fail;
    }
    s_stats.in_w      = (uint32_t)s_net->w;
    s_stats.in_h      = (uint32_t)s_net->h;
    s_stats.ext_input = s_net->ext_input;
    if (s_net->output_num >= 1) {
        const struct cpu_inst_layer_t *ol = &s_net->layers[s_net->output_layer[0]];
        /* The only float touched after boot: folded into Q16 here, on the app
         * task, before the audio task exists. */
        s_stats.scale_q16 = softmax_q_scale(ol->output_scale);
        s_stats.out_zp    = ol->tf_output_offset;
    }
    IPRO_LOGI(LOG_TAG, "loaded from %s in %u ms: input %dx%dx%d, %d outputs, ext_input %d, "
                       "%d layers (%d NPU), out scale q16 %u zp %d",
              s_source, (unsigned)s_stats.load_ms, s_net->w, s_net->h, s_net->c,
              s_net->output_num, s_net->ext_input, s_net->layer_cnt, s_net->npu_layer_cnt,
              (unsigned)s_stats.scale_q16, s_stats.out_zp);

    if (s_net->w != GESTURE_IN_W || s_net->h > GESTURE_IN_H || s_net->output_num != 1) {
        IPRO_LOGE(LOG_TAG, "model %dx%d with %d outputs does not fit the %dx%d "
                           "single-output frame this app feeds",
                  s_net->w, s_net->h, s_net->output_num, GESTURE_IN_W, GESTURE_IN_H);
        s_fail = "WRONG MODEL";
        goto fail;
    }
    s_feed = s_net->ext_input ? GESTURE_FEED_RAW : GESTURE_FEED_COPY;
    return 0;

fail:
    /* A failed load must not keep the handle (and, after a partial load, the
     * model's PSRAM buffers) for the life of the boot. */
    iproai_free(s_hdl);
    s_hdl    = NULL;
    s_net    = NULL;
    s_source = "none";
    return -1;
}

/* ---------------------------------------------------------------------------
 * Feed + result
 * -------------------------------------------------------------------------*/

/* Prepare the frame for the NPU and return the external-input address to
 * program (0 when the model reads its own input buffer). */
static uint32_t feed(uint32_t addr)
{
    switch (s_feed) {
    case GESTURE_FEED_RAW:
        return addr;

    case GESTURE_FEED_XOR: {
        /* int8 with zero point -128: q = u - 128 == u ^ 0x80. In place. */
        uint32_t *p = (uint32_t *)(uintptr_t)addr;
        uint32_t  n = GESTURE_FRAME_BYTES / 4u;
        L1C_DCACHE_INVALID_RANGE(addr, GESTURE_FRAME_BYTES);
        for (uint32_t i = 0; i < n; i++) {
            p[i] ^= 0x80808080u;
        }
        L1C_DCACHE_CLEAN_INVALID_RANGE(addr, GESTURE_FRAME_BYTES);
        return addr;
    }

    case GESTURE_FEED_COPY:
    default: {
        const uint8_t *src = (const uint8_t *)(uintptr_t)addr;
        int8_t *dst = (int8_t *)iproai_getInputBuffer(s_hdl);
        if (!dst) {
            return 0;
        }
        L1C_DCACHE_INVALID_RANGE(addr, GESTURE_FRAME_BYTES);
        for (uint32_t i = 0; i < GESTURE_FRAME_BYTES; i++) {
            dst[i] = (int8_t)((int)src[i] - 128);
        }
        L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)dst, GESTURE_FRAME_BYTES);
        return 0;
    }
    }
}

static void publish(const uint8_t *out, uint32_t npu_ms)
{
    gesture_result_t r;

    memset(&r, 0, sizeof(r));
    memcpy(r.raw, out, GESTURE_NCLASS);
    /* The runtime keeps NPU outputs in the uint8 domain with the zero point
     * shifted to match; softmax only needs the differences. */
    r.cls    = (gesture_class_t)softmax_q_pct(out, GESTURE_NCLASS, s_stats.scale_q16, r.prob);
    r.conf   = r.prob[r.cls];
    r.npu_ms = npu_ms;

    xSemaphoreTake(s_mtx, portMAX_DELAY);
    r.seq = s_result.seq + 1u;
    s_result = r;
    xSemaphoreGive(s_mtx);

    if (s_print) {
        printf("[GESTURE] #%lu %-8s %3u%%  p=%3u/%3u/%3u/%3u  raw=%3u %3u %3u %3u  %lu ms\r\n",
               (unsigned long)r.seq, s_names[r.cls], r.conf,
               r.prob[0], r.prob[1], r.prob[2], r.prob[3],
               out[0], out[1], out[2], out[3], (unsigned long)npu_ms);
    }
}

static void gesture_task(void *arg)
{
    TickType_t last = 0;
    uint32_t addr;

    (void)arg;
    for (;;) {
        if (xQueueReceive(s_q, &addr, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        TickType_t t0 = xTaskGetTickCount();
        uint32_t ext = feed(addr);
        npu_share_begin(s_net, ext);
        TickType_t tw = xTaskGetTickCount();          /* how long the detector held it */
        IPROAI_Status_e st = iproai_startCompute(s_hdl);
        npu_share_end();
        TickType_t t1 = xTaskGetTickCount();
        s_stats.wait_ms = (uint32_t)((tw - t0) * portTICK_PERIOD_MS);

        if (st != IPROAI_STATUS_NO_ERROR) {
            s_stats.errors++;
            IPRO_LOGE(LOG_TAG, "NPU compute failed (status %d)", (int)st);
        } else {
            uint32_t osz = 0;
            const uint8_t *out = iproai_getOutputBuffer(s_hdl, &osz);
            if (!out || osz < GESTURE_NCLASS) {
                s_stats.errors++;
            } else {
                publish(out, (uint32_t)((t1 - tw) * portTICK_PERIOD_MS));
                s_stats.frames++;
                if (last) {
                    uint32_t period = (uint32_t)((t1 - last) * portTICK_PERIOD_MS);
                    if (period) {
                        uint32_t inst = 10000u / period;
                        s_stats.fps_x10 = s_stats.fps_x10
                                        ? (s_stats.fps_x10 * 3u + inst) / 4u : inst;
                    }
                }
                last = t1;
            }
        }
        s_busy = false;
        if (s_done_cb) {
            s_done_cb(s_done_arg);
        }
    }
}

/* ---------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

int gesture_init(void)
{
    if (s_ready) {
        return 0;
    }
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

    if (!s_frame_raw) {
        s_frame_raw = pvPortMalloc(GESTURE_FRAME_BYTES + GESTURE_FRAME_ALIGN + 4u);
        if (!s_frame_raw) {
            s_fail = "NO PSRAM FOR INPUT";
            return -1;
        }
        uintptr_t p = ((uintptr_t)s_frame_raw + 4u + GESTURE_FRAME_ALIGN - 1u)
                      & ~(uintptr_t)(GESTURE_FRAME_ALIGN - 1u);
        s_frame   = (uint32_t)p;
        imgBuffer = (char *)(p - 4u);
        memset((void *)p, 0, GESTURE_FRAME_BYTES);
    }
    if (!s_mtx) {
        s_mtx = xSemaphoreCreateMutex();
    }
    if (!s_q) {
        s_q = xQueueCreate(1, sizeof(uint32_t));
    }
    if (!s_mtx || !s_q) {
        s_fail = "NO RAM FOR QUEUES";
        return -1;
    }
    if (load_model() != 0) {
        return -1;
    }
    s_stats.free_heap = (uint32_t)xPortGetFreeHeapSize();
    IPRO_LOGI(LOG_TAG, "heap left after the model: %u KB", (unsigned)(s_stats.free_heap / 1024u));

    if (xTaskCreate(gesture_task, "gesture", GESTURE_TASK_STACK, NULL, GESTURE_TASK_PRIO,
                    NULL) != pdPASS) {
        s_fail = "TASK CREATE FAILED";
        return -1;
    }
    s_result.cls = GESTURE_NONE;
    s_ready = true;
    s_fail  = "";
    return 0;
}

bool           gesture_ready(void)        { return s_ready; }
const char    *gesture_fail_reason(void)  { return s_fail; }
const char    *gesture_source(void)       { return s_source; }
uint32_t       gesture_input_addr(void)   { return s_frame; }
bool           gesture_busy(void)         { return s_busy; }
gesture_feed_t gesture_get_feed(void)     { return s_feed; }
void           gesture_set_print(bool on) { s_print = on; }
bool           gesture_get_print(void)    { return s_print; }

int gesture_set_feed(gesture_feed_t f)
{
    if (f > GESTURE_FEED_COPY) {
        return -1;
    }
    s_feed = f;
    return 0;
}

static inline uint8_t clamp_u8(int v)
{
    return (uint8_t)(v < 0 ? 0 : (v > 255 ? 255 : v));
}

int gesture_push_yuyv(const uint32_t *yuyv, int stride_px, int x0, int y0)
{
    uint8_t *dst = (uint8_t *)(uintptr_t)s_frame;

    if (!s_ready || s_busy || !yuyv || (x0 & 1) || stride_px < x0 + GESTURE_IN_W) {
        return -1;
    }
    s_busy = true;

    /* YUYV [Y0 U Y1 V] -> R,G,B,pad per pixel (byte 0 = R, what the ISP's
     * channel-1 ARGB path gave the model in training). BT.601, integer. */
    for (int y = 0; y < GESTURE_IN_H; y++) {
        const uint32_t *src = yuyv + ((uint32_t)(y0 + y) * (uint32_t)stride_px + (uint32_t)x0) / 2u;
        for (int x = 0; x < GESTURE_IN_W; x += 2) {
            uint32_t w  = *src++;
            int yy0 = (int)(w & 0xFFu), yy1 = (int)((w >> 16) & 0xFFu);
            int cb  = (int)((w >> 8) & 0xFFu) - 128;
            int cr  = (int)((w >> 24) & 0xFFu) - 128;
            int dr  = (359 * cr) >> 8, dg = (88 * cb + 183 * cr) >> 8, db = (454 * cb) >> 8;
            dst[0] = clamp_u8(yy0 + dr); dst[1] = clamp_u8(yy0 - dg); dst[2] = clamp_u8(yy0 + db); dst[3] = 0;
            dst[4] = clamp_u8(yy1 + dr); dst[5] = clamp_u8(yy1 - dg); dst[6] = clamp_u8(yy1 + db); dst[7] = 0;
            dst += 8;
        }
    }
    /* Written through the cache; the NPU reads PSRAM directly. */
    L1C_DCACHE_CLEAN_INVALID_RANGE(s_frame, GESTURE_FRAME_BYTES);

    /* Cheap fingerprint of what went in (the first 64 rows' words summed),
     * so a constant classifier output can be told apart from a constant
     * input. */
    {
        const uint32_t *w = (const uint32_t *)(uintptr_t)s_frame;
        uint32_t sum = 0;
        for (uint32_t i = 0; i < GESTURE_FRAME_BYTES / 8u; i += 16u) {
            sum += w[i];
        }
        s_stats.in_sum = sum;
    }

    uint32_t addr = s_frame;
    if (xQueueSend(s_q, &addr, 0) != pdTRUE) {
        s_busy = false;
        return -1;
    }
    return 0;
}

void gesture_set_done_cb(gesture_done_cb_t cb, void *arg)
{
    s_done_cb  = cb;
    s_done_arg = arg;
}

void gesture_get_result(gesture_result_t *out)
{
    if (!out) {
        return;
    }
    if (!s_mtx) {
        memset(out, 0, sizeof(*out));
        out->cls = GESTURE_NONE;
        return;
    }
    xSemaphoreTake(s_mtx, portMAX_DELAY);
    *out = s_result;
    xSemaphoreGive(s_mtx);
}

void gesture_get_stats(gesture_stats_t *out)
{
    if (out) {
        *out = s_stats;
    }
}
