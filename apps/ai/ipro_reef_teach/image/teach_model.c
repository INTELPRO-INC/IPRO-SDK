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
 * @file teach_model.c
 * @brief User-trained N-class classifier on the NPU. See teach_model.h.
 *
 * The model runs through the IPRO Edge AI runtime (components/ai/runtime,
 * ipro_ai.h), which owns the NPU lock. Load path (mfg partition -> XIP ->
 * ipro_ai_model_open), feed and one-frame-in-flight task are rps_model.c's (apps/ai/ipro7ai_evk_rps), which
 * the Reef game build proved. What differs: the class count and names come
 * from the label block after the model, and the output is a 4x4xN map whose
 * positions are averaged here (the head is a 1x1 conv, see teach_model.h).
 */

#include "teach_model.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include "drv_sf_ctrl.h"
#include "hal_mtimer.h"
#include "hal_boot2.h"
#include "hal_flash.h"
#include <system_soc.h>

#include "ipro_ai.h"
#include "ipro_ai_app.h"
#include "ipro_ai_devinfo.h"
#include "ipro_ai_store.h"

#include "ipro_log.h"

#define LOG_TAG "TEACH"

#define TEACH_PARTITION        "mfg"

/* ext_input: 4 bytes per pixel (pad_c = align(3,4)). */
#define TEACH_FRAME_BYTES      ((uint32_t)TEACH_IN_W * TEACH_IN_H * 4u)
#define TEACH_FRAME_ALIGN      32u

#define TEACH_TASK_STACK       4096
#define TEACH_TASK_PRIO        (configMAX_PRIORITIES - 3)

typedef struct {
    char    magic[4];
    uint8_t version;
    uint8_t nclass;
    uint16_t reserved;
    char    labels[TEACH_MAX_CLASSES][TEACH_LABEL_LEN];
} teach_trailer_t;

static ipro_ai_model_t   *s_model;
static char               s_labels[TEACH_MAX_CLASSES][TEACH_LABEL_LEN];
static uint8_t           *s_frame_raw;
static uint32_t           s_frame;
static QueueHandle_t      s_q;
static SemaphoreHandle_t  s_mtx;
static volatile bool      s_ready, s_busy, s_print, s_paused;
static const char        *s_fail = "";
static teach_result_t     s_result;
static teach_stats_t      s_stats;
static teach_done_cb_t    s_done_cb;
static void              *s_done_arg;
static uint8_t           *s_cap_buf;
static volatile bool      s_cap_req, s_cap_ready;
static volatile uint32_t  s_cap_seq;
static teach_result_t     s_cap_result;    /* result of the captured frame */
static ipro_ai_blob_t     s_found;         /* what the store reported at boot (TMOD) */
/* the result's decode, shared with Sensor Teach (components/ai/app) */
static ipro_ai_adapter_t      s_cls;
static ipro_ai_classify_cfg_t s_cls_cfg;
static ipro_ai_tensor_info_t  s_out;

/* ---------------------------------------------------------------------------
 * Model load
 * -------------------------------------------------------------------------*/

/* A V1.1 board's payload, written before the store kept records: the .inm,
 * then the label block when the host sent one. */
static uint32_t legacy_size(const uint8_t *blob, uint32_t room)
{
    ipro_ai_inm_header_t inm;

    if (ipro_ai_inm_peek(blob, room, &inm) != IPRO_AI_OK) {
        return 0;
    }
    if (room - inm.bytes >= sizeof(teach_trailer_t) &&
        !memcmp(blob + inm.bytes, TEACH_TRAILER_MAGIC, 4)) {
        return inm.bytes + (uint32_t)sizeof(teach_trailer_t);
    }
    return inm.bytes;
}

static const ipro_ai_slot_t s_slot = {
    .partition = TEACH_PARTITION, .offset = TEACH_MFG_OFFSET, .size = TEACH_MFG_SIZE,
    .banks = 1, .kind = IPRO_AI_KIND_IMAGE, .legacy_size = legacy_size,
};

const ipro_ai_slot_t *teach_model_slot(void)
{
    return &s_slot;
}

/* What the store found in the slot (components/ai/store): the payload is the
 * .inm and, after it, the label block; *room_after = bytes after the .inm. */
static const uint8_t *locate_model(uint32_t *room_after)
{
    ipro_ai_blob_t found;
    ipro_ai_inm_header_t inm;

    const int rc = ipro_ai_store_open(&s_slot, &found);
    s_found = found;
    if (rc != 0 || found.state == IPRO_AI_SLOT_NONE) {
        IPRO_LOGE(LOG_TAG, "no model in '%s': %s", TEACH_PARTITION, found.why);
        s_fail = !strcmp(found.why, "install interrupted") ? "DEPLOY INTERRUPTED"
               : !strcmp(found.why, "model damaged (CRC)") ? "MODEL DAMAGED"
               : !strcmp(found.why, "no slot") ? "NO MFG PARTITION" : "NO MODEL IN MFG";
        return NULL;
    }
    if (ipro_ai_inm_peek(found.data, found.bytes, &inm) != IPRO_AI_OK) {
        s_fail = "NO MODEL IN MFG";
        return NULL;
    }
    s_stats.model_bytes = inm.bytes;
    *room_after = found.bytes - inm.bytes;
    if (found.state == IPRO_AI_SLOT_COMMITTED) {
        IPRO_LOGI(LOG_TAG, "model: %u B, install #%lu, CRC %08lx checked", (unsigned)inm.bytes,
                  (unsigned long)found.seq, (unsigned long)found.crc);
    } else {
        IPRO_LOGI(LOG_TAG, "model: %u B, V1.1 payload (not CRC-checked)", (unsigned)inm.bytes);
    }
    return found.data;
}

/* Class names from the label block right after the model; CLASS n without it. */
static void read_labels(const uint8_t *trailer, uint32_t room, unsigned out_c)
{
    teach_trailer_t t;
    unsigned n = out_c < TEACH_MAX_CLASSES ? out_c : TEACH_MAX_CLASSES;

    s_stats.labelled = false;
    if (room >= sizeof(t)) {
        memcpy(&t, trailer, sizeof(t));
        if (!memcmp(t.magic, TEACH_TRAILER_MAGIC, 4) && t.version == TEACH_TRAILER_VERSION &&
            t.nclass >= 1 && t.nclass <= n) {
            n = t.nclass;
            for (unsigned i = 0; i < n; i++) {
                memcpy(s_labels[i], t.labels[i], TEACH_LABEL_LEN);
                s_labels[i][TEACH_LABEL_LEN - 1] = '\0';
            }
            s_stats.labelled = true;
        }
    }
    if (!s_stats.labelled) {
        for (unsigned i = 0; i < n; i++) {
            snprintf(s_labels[i], TEACH_LABEL_LEN, "CLASS %u", i);
        }
    }
    s_stats.nclass = (uint8_t)n;
}

static int load_model(void)
{
    uint32_t room = 0;

    const uint8_t *xip = locate_model(&room);
    if (!xip) {
        return -1;
    }
    if (ipro_ai_init() != IPRO_AI_OK) {
        s_fail = "NPU CREATE FAILED";
        return -1;
    }
    /* The ISP DMAs every frame into s_frame, the model's external input. */
    const ipro_ai_model_cfg_t mc = {
        .inm = xip, .max_bytes = s_stats.model_bytes, .input = (void *)(uintptr_t)s_frame,
        .name = "image",
    };
    const int rc = ipro_ai_model_open(&mc, &s_model);
    if (rc != IPRO_AI_OK) {
        IPRO_LOGE(LOG_TAG, "NPU load failed: %s", ipro_ai_strerror(rc));
        s_fail = rc == IPRO_AI_E_NOMEM ? "NPU CREATE FAILED" : "NPU LOAD FAILED";
        return -1;
    }
    ipro_ai_model_info_t info;
    ipro_ai_tensor_info_t in, out;
    ipro_ai_model_get_info(s_model, &info);
    ipro_ai_tensor_get_info(s_model, IPRO_AI_INPUT, 0, &in);
    ipro_ai_tensor_get_info(s_model, IPRO_AI_OUTPUT, 0, &out);
    s_stats.load_ms = info.load_us / 1000u;
    if (in.w != TEACH_IN_W || in.h != TEACH_IN_H || info.outputs != 1) {
        IPRO_LOGE(LOG_TAG, "model %ux%u with %u outputs; this app feeds %dx%d, one output",
                  in.w, in.h, info.outputs, TEACH_IN_W, TEACH_IN_H);
        s_fail = "WRONG MODEL";
        ipro_ai_model_close(s_model);
        s_model = NULL;
        return -1;
    }
    /* The output is the LAST layer's (what iproai_getOutputBuffer() returns),
     * channels padded to 4 when the NPU ran it. */
    s_stats.out_w     = out.w;
    s_stats.out_h     = out.h;
    s_stats.out_c     = out.c;
    s_stats.out_pitch = out.c_pitch;
    s_stats.out_scale = out.scale;
    s_stats.out_zp    = out.zero_point;
    read_labels(xip + s_stats.model_bytes, room, (unsigned)out.c);
    s_out = out;
    s_cls_cfg = (ipro_ai_classify_cfg_t){ .n = s_stats.nclass, .labels = s_labels };
    ipro_ai_adapter_classify(&s_cls, &s_cls_cfg);

    IPRO_LOGI(LOG_TAG, "loaded in %u ms: %u layers (%u NPU), out %ux%ux%u pitch %u, "
                       "scale %.5f zp %d, %u classes%s",
              (unsigned)s_stats.load_ms, info.layers, info.npu_layers,
              s_stats.out_w, s_stats.out_h, s_stats.out_c, s_stats.out_pitch,
              (double)s_stats.out_scale, s_stats.out_zp, s_stats.nclass,
              s_stats.labelled ? "" : " (no label block)");
    if (!info.ext_input) {
        s_fail = "MODEL NOT EXT_INPUT";
        ipro_ai_model_close(s_model);
        s_model = NULL;
        return -1;
    }
    return 0;
}

/* ---------------------------------------------------------------------------
 * Result
 * -------------------------------------------------------------------------*/

static void publish(const uint8_t *out, uint32_t npu_ms)
{
    const unsigned n = s_stats.nclass;
    teach_result_t r;
    ipro_ai_result_t res;

    memset(&r, 0, sizeof(r));
    /* Each class averaged over the positions in the uint8 domain the runtime
     * presents (zero point already shifted, as rps_model.c reads it): the
     * classification adapter (components/ai/app), which also publishes the
     * class whenever it changes. */
    if (s_cls.post(&s_cls, out, &s_out, &res) != IPRO_AI_OK) {
        s_stats.errors++;
        return;
    }
    const unsigned best = (unsigned)res.cls.best;
    for (unsigned k = 0; k < n; k++) {
        r.logit_x100[k] = (int16_t)lrintf(fmaxf(fminf(res.cls.logit[k] * 100.0f, 32767.0f), -32768.0f));
        r.prob[k] = (uint8_t)(100.0f * res.cls.exp[k] / res.cls.exp_sum + 0.5f);
    }
    ipro_ai_event_t e = { .model_id = 1, .t_us = mtimer_get_time_us() };
    strcpy(e.model, "image");
    s_cls.events(&s_cls, &res, &e);
    r.cls    = (uint8_t)best;
    r.conf   = r.prob[best];
    r.npu_ms = npu_ms;

    xSemaphoreTake(s_mtx, portMAX_DELAY);
    r.seq = s_result.seq + 1u;
    s_result = r;
    if (r.seq == s_cap_seq) {
        s_cap_result = r;
    }
    xSemaphoreGive(s_mtx);

    if (s_print) {
        printf("[TEACH] #%lu %s %u%% | logits x100:", (unsigned long)r.seq, s_labels[best],
               r.conf);
        for (unsigned k = 0; k < n; k++) {
            printf(" %d", r.logit_x100[k]);
        }
        printf(" | %lu ms\r\n", (unsigned long)npu_ms);
    }
}

static void teach_task(void *arg)
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
            L1C_DCACHE_INVALID_RANGE(addr, TEACH_FRAME_BYTES);
            memcpy(s_cap_buf, (const void *)(uintptr_t)addr, TEACH_FRAME_BYTES);
            s_cap_seq   = s_result.seq + 1u;   /* only this task publishes */
            s_cap_req   = false;
            s_cap_ready = true;
        }
        /* ext_input reads the ISP's ARGB frame directly: uint8 pixels are the
         * NPU's unsigned domain, i.e. int8 = pixel - 128 - what Studio feeds
         * the TFLite model when it trains. A DMA target, so no cache clean. */
        const uint8_t *out = NULL;
        const int rc = ipro_ai_invoke(s_model, (const void *)(uintptr_t)addr, &out);
        TickType_t t1 = xTaskGetTickCount();

        if (rc != IPRO_AI_OK) {
            s_stats.errors++;
            IPRO_LOGE(LOG_TAG, "NPU compute failed: %s", ipro_ai_strerror(rc));
        } else {
            publish(out, (uint32_t)((t1 - t0) * portTICK_PERIOD_MS));
            s_stats.frames++;
            if (last) {
                uint32_t period = (uint32_t)((t1 - last) * portTICK_PERIOD_MS);
                if (period) {
                    uint32_t inst = 10000u / period;
                    s_stats.fps_x10 = s_stats.fps_x10 ? (s_stats.fps_x10 * 3u + inst) / 4u : inst;
                }
            }
            last = t1;
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

int teach_model_init(void)
{
    if (s_ready) {
        return 0;
    }
    if (!s_frame_raw) {
        s_frame_raw = pvPortMalloc(TEACH_FRAME_BYTES + TEACH_FRAME_ALIGN + 4u);
        if (!s_frame_raw) {
            s_fail = "NO PSRAM FOR INPUT";
            return -1;
        }
        uintptr_t p = ((uintptr_t)s_frame_raw + 4u + TEACH_FRAME_ALIGN - 1u)
                      & ~(uintptr_t)(TEACH_FRAME_ALIGN - 1u);
        s_frame   = (uint32_t)p;
        memset((void *)p, 0, TEACH_FRAME_BYTES);
    }
    if (!s_cap_buf) {
        s_cap_buf = pvPortMalloc(TEACH_FRAME_BYTES);   /* 64 KB, PSRAM */
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
    ipro_ai_event_init(16, true);                 /* class changes as events (ipro_ai_event.h) */
    if (load_model() != 0) {
        return -1;
    }
    s_stats.free_heap = (uint32_t)xPortGetFreeHeapSize();
    if (xTaskCreate(teach_task, "teach_npu", TEACH_TASK_STACK, NULL, TEACH_TASK_PRIO,
                    NULL) != pdPASS) {
        s_fail = "TASK CREATE FAILED";
        return -1;
    }
    s_ready = true;
    s_fail  = "";
    return 0;
}

bool        teach_model_ready(void)       { return s_ready; }
const char *teach_model_fail_reason(void) { return s_fail; }
uint32_t    teach_model_input_addr(void)  { return s_frame; }
void        teach_model_set_print(bool on){ s_print = on; }

const char *teach_model_label(unsigned cls)
{
    return cls < s_stats.nclass ? s_labels[cls] : "?";
}

int teach_model_push(uint32_t addr)
{
    BaseType_t woken = pdFALSE;

    if (!s_ready || s_busy || s_paused) {
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

int teach_model_pause(bool pause, uint32_t timeout_ms)
{
    s_paused = pause;
    if (!pause) {
        return 0;
    }
    for (uint32_t t = 0; s_busy; t += 10) {
        if (t >= timeout_ms) {
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return 0;
}

void teach_model_set_done_cb(teach_done_cb_t cb, void *arg)
{
    s_done_cb  = cb;
    s_done_arg = arg;
}

int teach_model_capture(const uint8_t **out, uint32_t *seq, uint32_t timeout_ms)
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
    if (seq) {
        *seq = s_cap_seq;
    }
    return 0;
}

int teach_model_capture_result(teach_result_t *out)
{
    int ret = -1;

    if (!out || !s_mtx) {
        return -1;
    }
    xSemaphoreTake(s_mtx, portMAX_DELAY);
    if (s_cap_seq && s_cap_result.seq == s_cap_seq) {
        *out = s_cap_result;
        ret = 0;
    }
    xSemaphoreGive(s_mtx);
    return ret;
}

void teach_model_get_result(teach_result_t *out)
{
    if (!out) {
        return;
    }
    if (!s_mtx) {
        memset(out, 0, sizeof(*out));
        return;
    }
    xSemaphoreTake(s_mtx, portMAX_DELAY);
    *out = s_result;
    xSemaphoreGive(s_mtx);
}

void teach_model_get_stats(teach_stats_t *out)
{
    if (out) {
        *out = s_stats;
    }
}

/* ---------------------------------------------------------------------------
 * The image protocol's V2.0 queries (uvc_teach_link.c): the camera's frames
 * are held off with teach_model_pause() so the link has the model alone.
 * -------------------------------------------------------------------------*/

#define RAW_PAUSE_MS 500u

int teach_model_run_raw(const uint8_t *in, uint32_t bytes, uint8_t *out, uint32_t out_cap,
                        uint32_t *out_bytes, uint32_t *us)
{
    ipro_ai_tensor_info_t ti;

    if (!s_ready || !s_model || ipro_ai_tensor_get_info(s_model, IPRO_AI_INPUT, 0, &ti) != IPRO_AI_OK) {
        return -1;
    }
    if (bytes != ti.bytes || s_out.bytes > out_cap) {
        return -2;
    }
    /* our own copy, aligned, with the header word the runtime keeps before an input */
    uint8_t *raw = pvPortMalloc(bytes + TEACH_FRAME_ALIGN + IPRO_AI_INPUT_HEADER);
    if (!raw) {
        return -3;
    }
    uint8_t *buf = (uint8_t *)(((uintptr_t)raw + IPRO_AI_INPUT_HEADER + TEACH_FRAME_ALIGN - 1u) &
                               ~(uintptr_t)(TEACH_FRAME_ALIGN - 1u));
    memcpy(buf, in, bytes);
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)buf, bytes);  /* the caller's input: ours to clean */
    int rc = -5;                                    /* the camera's frame did not finish in time */
    if (teach_model_pause(true, RAW_PAUSE_MS) == 0) {
        const uint8_t *o = NULL;
        const uint64_t t0 = mtimer_get_time_us();
        rc = ipro_ai_invoke(s_model, buf, &o) == IPRO_AI_OK ? 0 : -4;
        *us = (uint32_t)(mtimer_get_time_us() - t0);
        if (rc == 0) {
            *out_bytes = s_out.bytes;
            memcpy(out, o, s_out.bytes);
        }
    }
    teach_model_pause(false, 0);
    vPortFree(raw);
    return rc;
}

int teach_model_bench(char *line, uint32_t size, uint32_t n)
{
    if (!s_ready || !s_model) {
        return snprintf(line, size, "TBEN ERR no_model");
    }
    if (teach_model_pause(true, RAW_PAUSE_MS) != 0) {
        teach_model_pause(false, 0);
        return snprintf(line, size, "TBEN ERR busy");
    }
    /* the frame buffer as it is: the last frame the camera wrote, or zeros */
    const int k = ipro_ai_devinfo_bench(line, size, "TBEN", s_model, n,
                                        (const void *)(uintptr_t)s_frame, UINT32_MAX);
    teach_model_pause(false, 0);
    return k;
}

int teach_model_describe(char *line, uint32_t size, const char *word)
{
    /* The input is the ISP's ARGB frame: its scale is the pixel itself ("u8 - 128"). */
    return ipro_ai_devinfo_model(line, size, word, &s_found, s_ready ? s_model : NULL,
                                 s_stats.labelled ? "teach" : "template", 0.0f);
}
