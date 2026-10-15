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
 * @file rps_model.c
 * @brief Rock / paper / scissors / none classifier on the NPU. See rps_model.h.
 *
 * Model path (mfg partition -> XIP -> iproai_load_model_from_buffer) is the
 * one apps/ai/ipro7ai_evk_person_detect proved on this board;
 * CONFIG_RPS_EMBED_MODEL swaps only where the XIP pointer comes from (the
 * blob linked in by src/model_blob.S). The output
 * quantisation is read from the model header at load time rather than
 * copied from the training log, so a retrained model needs no firmware edit.
 */

#include "rps_model.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

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

#include "ipro_log.h"

#define LOG_TAG "RPS_MODEL"

#define RPS_PARTITION          "mfg"
#define PARTITION_HEADER_SIZE  0x1000u

/* ext_input: 4 bytes per pixel (pad_c = align(3,4)). */
#define RPS_FRAME_BYTES        ((uint32_t)RPS_IN_W * RPS_IN_H * 4u)
#define RPS_FRAME_ALIGN        32u

/* Above the display task (polling SPI) for the same reason as the detector
 * in the person app: the NPU layer dispatch must not queue behind a blit. */
#define RPS_TASK_STACK         4096
#define RPS_TASK_PRIO          (configMAX_PRIORITIES - 3)

/* components/ai programs the NPU's external-input address from imgBuffer+4
 * at load; hal_npu.c only has a weak NULL. */
char *imgBuffer = NULL;

static const char *s_names[RPS_NCLASS] = { "ROCK", "PAPER", "SCISSORS", "NONE" };

static iproai_model_hdl_t   s_hdl;
static struct iproai_net_info_t *s_net;
static uint8_t             *s_frame_raw;
static uint32_t             s_frame;
static QueueHandle_t        s_q;
static SemaphoreHandle_t    s_mtx;
static volatile bool        s_ready, s_busy, s_print;
static const char          *s_fail = "";
static rps_feed_t           s_feed = RPS_FEED_RAW;
static rps_result_t         s_result;
static rps_model_stats_t    s_stats;
static rps_done_cb_t        s_done_cb;
static void                *s_done_arg;
static uint8_t             *s_cap_buf;         /* rps_model_capture() copy */
static volatile bool        s_cap_req, s_cap_ready;

const char *rps_class_name(rps_class_t c)
{
    return ((unsigned)c < RPS_NCLASS) ? s_names[c] : "?";
}

/* ---------------------------------------------------------------------------
 * Model load
 * -------------------------------------------------------------------------*/

/* Total size the .inm's section table declares: 8 section lengths + the
 * 32-byte table itself. */
static uint32_t inm_declared_size(const uint8_t *blob)
{
    uint32_t header[8], size = sizeof(header);

    memcpy(header, blob, sizeof(header));
    for (int i = 0; i < 8; i++) {
        size += header[i];
    }
    return size;
}

#ifdef CONFIG_RPS_EMBED_MODEL
/* src/model_blob.S */
extern const uint8_t g_rps_model[];
extern const uint8_t g_rps_model_end[];

/* The model is linked into the image (.rodata, read over XIP). */
static const uint8_t *locate_model(void)
{
    uint32_t linked = (uint32_t)(g_rps_model_end - g_rps_model);
    uint32_t model_size = inm_declared_size(g_rps_model);

    if (model_size != linked) {
        IPRO_LOGE(LOG_TAG, "embedded model declares %u B but %u B are linked",
                  (unsigned)model_size, (unsigned)linked);
        s_fail = "BAD EMBEDDED MODEL";
        return NULL;
    }
    s_stats.model_bytes = model_size;
    s_stats.source = "firmware";
    IPRO_LOGI(LOG_TAG, "model: %u B linked into the firmware at %p",
              (unsigned)model_size, (const void *)g_rps_model);
    return g_rps_model;
}
#else
/* The model sits in the mfg partition at +0x1000, read over XIP. */
static const uint8_t *locate_model(void)
{
    uint32_t part_addr = 0, part_size = 0;

    if (hal_boot2_partition_addr_active(RPS_PARTITION, &part_addr, &part_size) != 0) {
        IPRO_LOGE(LOG_TAG, "partition '%s' not in the flashed table - "
                           "./flash.sh --full", RPS_PARTITION);
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

    uint32_t model_size = inm_declared_size(xip);
    if (model_size <= 32u || model_size + PARTITION_HEADER_SIZE > part_size) {
        IPRO_LOGE(LOG_TAG, "no valid model in '%s' (size word 0x%08X) - "
                           "./flash.sh --model", RPS_PARTITION, (unsigned)model_size);
        s_fail = "NO MODEL IN MFG";
        return NULL;
    }
    s_stats.model_bytes = model_size;
    s_stats.source = "mfg";
    IPRO_LOGI(LOG_TAG, "model: %u B at flash 0x%06X", (unsigned)model_size,
              (unsigned)blob_phys);
    return xip;
}
#endif

static int load_model(void)
{
    IPROAI_Status_e st;
    TickType_t t0;

    const uint8_t *xip = locate_model();
    if (!xip) {
        return -1;
    }

    s_hdl = iproai_create();
    if (!s_hdl) {
        s_fail = "NPU CREATE FAILED";
        return -1;
    }
    t0 = xTaskGetTickCount();
    st = iproai_load_model_from_buffer(s_hdl, xip);
    s_stats.load_ms = (uint32_t)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS);
    if (st != IPROAI_STATUS_NO_ERROR) {
        IPRO_LOGE(LOG_TAG, "NPU load failed (status %d)", (int)st);
        s_fail = "NPU LOAD FAILED";
        return -1;
    }
    s_net = iproai_getNetInfo(s_hdl);
    if (!s_net) {
        s_fail = "NO NET INFO";
        return -1;
    }

    s_stats.in_w = (uint32_t)s_net->w;
    s_stats.in_h = (uint32_t)s_net->h;
    s_stats.ext_input = s_net->ext_input;
    s_stats.unsigned_input = (uint8_t)s_net->unsign_input;
    if (s_net->output_num >= 1) {
        const struct cpu_inst_layer_t *ol = &s_net->layers[s_net->output_layer[0]];
        s_stats.out_scale = ol->output_scale;
        s_stats.out_zp    = ol->tf_output_offset;
    }
    IPRO_LOGI(LOG_TAG, "loaded in %u ms: input %dx%dx%d, %d outputs, ext_input %d, "
                       "unsigned %d, %d layers (%d NPU), out scale %.5f zp %d",
              (unsigned)s_stats.load_ms, s_net->w, s_net->h, s_net->c,
              s_net->output_num, s_net->ext_input, s_net->unsign_input,
              s_net->layer_cnt, s_net->npu_layer_cnt,
              (double)s_stats.out_scale, s_stats.out_zp);

    if (s_net->w != RPS_IN_W || s_net->h > RPS_IN_H || s_net->output_num != 1) {
        IPRO_LOGE(LOG_TAG, "model %dx%d with %d outputs does not fit the %dx%d "
                           "single-output frame this app feeds",
                  s_net->w, s_net->h, s_net->output_num, RPS_IN_W, RPS_IN_H);
        s_fail = "WRONG MODEL";
        return -1;
    }
    /* Default feed from the header; `rps feed` overrides on the bench. */
    s_feed = s_net->ext_input ? RPS_FEED_RAW : RPS_FEED_COPY;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Feed + result
 * -------------------------------------------------------------------------*/

static void feed(uint32_t addr)
{
    switch (s_feed) {
    case RPS_FEED_RAW:
        NPU_Img_Ext_Addr_Cfg(addr);
        break;

    case RPS_FEED_XOR: {
        /* int8 with zero point -128: q = u - 128 == u ^ 0x80. In place - the
         * ISP is gated off this buffer until we are done. */
        uint32_t *p = (uint32_t *)(uintptr_t)addr;
        uint32_t  n = RPS_FRAME_BYTES / 4u;
        L1C_DCACHE_INVALID_RANGE(addr, RPS_FRAME_BYTES);
        for (uint32_t i = 0; i < n; i++) {
            p[i] ^= 0x80808080u;
        }
        L1C_DCACHE_CLEAN_INVALID_RANGE(addr, RPS_FRAME_BYTES);
        NPU_Img_Ext_Addr_Cfg(addr);
        break;
    }

    case RPS_FEED_COPY:
    default: {
        /* Model reads its input from its own buffer (DM0): copy the frame in,
         * 4 bytes per pixel (pad_c), int8 = u - 128. */
        const uint8_t *src = (const uint8_t *)(uintptr_t)addr;
        int8_t *dst = (int8_t *)iproai_getInputBuffer(s_hdl);
        if (!dst) {
            return;
        }
        L1C_DCACHE_INVALID_RANGE(addr, RPS_FRAME_BYTES);
        for (uint32_t i = 0; i < RPS_FRAME_BYTES; i++) {
            dst[i] = (int8_t)((int)src[i] - 128);
        }
        L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)dst, RPS_FRAME_BYTES);
        break;
    }
    }
}

static void publish(const uint8_t *out, uint32_t npu_ms)
{
    rps_result_t r;
    float f[RPS_NCLASS], m = -1e30f, sum = 0.0f;
    int best = 0;

    memset(&r, 0, sizeof(r));
    /* The runtime keeps NPU outputs in the uint8 domain with the header's
     * zero point already shifted to match (person_pet: int8 zp 55 -> 183). */
    for (int i = 0; i < RPS_NCLASS; i++) {
        r.logit[i] = (int8_t)out[i];
        f[i] = ((float)out[i] - (float)s_stats.out_zp) * s_stats.out_scale;
        if (f[i] > m) {
            m = f[i];
            best = i;
        }
    }
    for (int i = 0; i < RPS_NCLASS; i++) {
        f[i] = expf(f[i] - m);
        sum += f[i];
    }
    for (int i = 0; i < RPS_NCLASS; i++) {
        r.prob[i] = (uint8_t)(100.0f * f[i] / sum + 0.5f);
    }
    r.cls    = (rps_class_t)best;
    r.conf   = r.prob[best];
    r.npu_ms = npu_ms;

    xSemaphoreTake(s_mtx, portMAX_DELAY);
    r.seq = s_result.seq + 1u;
    s_result = r;
    xSemaphoreGive(s_mtx);

    if (s_print) {
        printf("[RPS] #%lu %-8s %3u%%  p=%3u/%3u/%3u/%3u  raw=%4d %4d %4d %4d  %lu ms\r\n",
               (unsigned long)r.seq, s_names[best], r.conf,
               r.prob[0], r.prob[1], r.prob[2], r.prob[3],
               out[0], out[1], out[2], out[3], (unsigned long)npu_ms);
    }
}

static void rps_task(void *arg)
{
    TickType_t last = 0;
    uint32_t addr;

    (void)arg;
    for (;;) {
        if (xQueueReceive(s_q, &addr, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        TickType_t t0 = xTaskGetTickCount();
        if (s_cap_req && s_cap_buf) {
            /* Snapshot BEFORE feed() - the XOR path rewrites the frame. */
            L1C_DCACHE_INVALID_RANGE(addr, RPS_FRAME_BYTES);
            memcpy(s_cap_buf, (const void *)(uintptr_t)addr, RPS_FRAME_BYTES);
            s_cap_req   = false;
            s_cap_ready = true;
        }
        feed(addr);
        IPROAI_Status_e st = iproai_startCompute(s_hdl);
        TickType_t t1 = xTaskGetTickCount();

        if (st != IPROAI_STATUS_NO_ERROR) {
            s_stats.errors++;
            IPRO_LOGE(LOG_TAG, "NPU compute failed (status %d)", (int)st);
        } else {
            uint32_t osz = 0;
            const uint8_t *out = iproai_getOutputBuffer(s_hdl, &osz);
            if (!out || osz < RPS_NCLASS) {
                s_stats.errors++;
            } else {
                publish(out, (uint32_t)((t1 - t0) * portTICK_PERIOD_MS));
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

int rps_model_init(void)
{
    if (s_ready) {
        return 0;
    }
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

    if (!s_frame_raw) {
        s_frame_raw = pvPortMalloc(RPS_FRAME_BYTES + RPS_FRAME_ALIGN + 4u);
        if (!s_frame_raw) {
            s_fail = "NO PSRAM FOR INPUT";
            return -1;
        }
        uintptr_t p = ((uintptr_t)s_frame_raw + 4u + RPS_FRAME_ALIGN - 1u)
                      & ~(uintptr_t)(RPS_FRAME_ALIGN - 1u);
        s_frame   = (uint32_t)p;
        imgBuffer = (char *)(p - 4u);
        memset((void *)p, 0, RPS_FRAME_BYTES);
    }
    if (!s_cap_buf) {
        s_cap_buf = pvPortMalloc(RPS_FRAME_BYTES);   /* 64 KB, PSRAM */
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
    IPRO_LOGI(LOG_TAG, "PSRAM heap left after the model: %u KB",
              (unsigned)(s_stats.free_heap / 1024u));

    if (xTaskCreate(rps_task, "rps_npu", RPS_TASK_STACK, NULL, RPS_TASK_PRIO,
                    NULL) != pdPASS) {
        s_fail = "TASK CREATE FAILED";
        return -1;
    }
    s_ready = true;
    s_fail  = "";
    return 0;
}

bool        rps_model_ready(void)       { return s_ready; }
const char *rps_model_fail_reason(void) { return s_fail; }
uint32_t    rps_model_input_addr(void)  { return s_frame; }
bool        rps_model_busy(void)        { return s_busy; }
rps_feed_t  rps_model_get_feed(void)    { return s_feed; }
void        rps_model_set_print(bool on){ s_print = on; }
bool        rps_model_get_print(void)   { return s_print; }

int rps_model_set_feed(rps_feed_t f)
{
    if (f > RPS_FEED_COPY) {
        return -1;
    }
    s_feed = f;
    return 0;
}

int rps_model_push(uint32_t addr)
{
    BaseType_t woken = pdFALSE;

    if (!s_ready || s_busy) {
        return -1;
    }
    s_busy = true;
    if (xQueueSendFromISR(s_q, &addr, &woken) != pdTRUE) {
        s_busy = false;
        return -1;
    }
    portYIELD_FROM_ISR(woken);
    return 0;
}

void rps_model_set_done_cb(rps_done_cb_t cb, void *arg)
{
    s_done_cb  = cb;
    s_done_arg = arg;
}

int rps_model_capture(const uint8_t **out, uint32_t timeout_ms)
{
    if (!s_ready || !s_cap_buf || !out) {
        return -1;
    }
    s_cap_ready = false;
    s_cap_req   = true;
    for (uint32_t t = 0; t < timeout_ms && !s_cap_ready; t += 10) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (!s_cap_ready) {
        s_cap_req = false;
        return -2;
    }
    *out = s_cap_buf;
    return 0;
}

void rps_model_get_result(rps_result_t *out)
{
    if (!out) {
        return;
    }
    if (!s_mtx) {
        memset(out, 0, sizeof(*out));
        out->cls = RPS_NONE;
        return;
    }
    xSemaphoreTake(s_mtx, portMAX_DELAY);
    *out = s_result;
    xSemaphoreGive(s_mtx);
}

void rps_model_get_stats(rps_model_stats_t *out)
{
    if (out) {
        *out = s_stats;
    }
}
