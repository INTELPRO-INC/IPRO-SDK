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
 * @file sensor_model.c
 * @brief .inm + SNS1 from mfg onto the NPU. See sensor_model.h.
 *
 * The model runs through the IPRO Edge AI runtime (components/ai/runtime,
 * ipro_ai.h), which owns the NPU lock and the input buffer.
 *
 * Input: 64x64x8 bytes, HWC, through the external input address, always
 * offset-binary (int8 ^ 0x80) as ipro_reef_teach/voice/spk_model.c feeds it:
 * this model's unsign_input is 0, and feeding plain int8 left the NPU 60 steps
 * off LiteRT; offset-binary matches it within one (2026-10-31). Output: 4x4
 * positions x pitch bytes in the runtime's uint8 domain, less tf_output_offset,
 * times output_scale (as ipro_reef_teach/image/teach_model.c reads it).
 */
#include "sensor_model.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include "hal_mtimer.h"
#include "ipro_ai.h"
#include "ipro_ai_app.h"
#include "ipro_ai_devinfo.h"
#include "ipro_ai_store.h"
#include "ipro_log.h"

#define LOG_TAG        "SENM"
#define SNS1_V1_BYTES  256u
#define SNS1_V2_BYTES  320u

static ipro_ai_model_t   *s_model;
static sensor_cfg_t       s_cfg;
static const char        *s_why = "not loaded";
static SemaphoreHandle_t  s_lock;         /* the sensor task and the link's INFER share the model */
static uint32_t           s_pitch, s_out_zp;
static float              s_out_scale;
static uint32_t           s_last_us;
static ipro_ai_blob_t     s_found;        /* what the store reported (MODEL) */
/* live classification through the app framework (components/ai/app): the
 * classification adapter, and for an anomaly model ("normal", "anomaly") the
 * anomaly adapter beside it, with events for both */
static ipro_ai_app_t         *s_app;
static ipro_ai_adapter_t      s_cls, s_anom;
static ipro_ai_classify_cfg_t s_cls_cfg;
static ipro_ai_anomaly_cfg_t  s_anom_cfg;
static bool                   s_is_anomaly;
static ipro_ai_tensor_info_t  s_out;

static int run_locked(const int8_t *img, float logits[SM_MAX_CLASSES], int8_t *raw);

static uint32_t rd32(const uint8_t *p)
{
    uint32_t v;
    memcpy(&v, p, 4);
    return v;
}

static float rdf(const uint8_t *p)
{
    float v;
    memcpy(&v, p, 4);
    return v;
}

/* A V1.1 board's payload, written before the store kept records: the .inm,
 * then a SNS1 block whose own size ends it. */
static uint32_t legacy_size(const uint8_t *b, uint32_t room)
{
    ipro_ai_inm_header_t h;

    if (ipro_ai_inm_peek(b, room, &h) != IPRO_AI_OK || room - h.bytes < SNS1_V1_BYTES ||
        memcmp(b + h.bytes, "SNS1", 4) != 0) {
        return 0;
    }
    const uint32_t sns = rd32(b + h.bytes + 8);
    return (sns == SNS1_V1_BYTES || sns == SNS1_V2_BYTES) && sns <= room - h.bytes ? h.bytes + sns : 0;
}

static const ipro_ai_slot_t s_slot = {
    .partition = "mfg", .offset = SENSOR_MFG_OFFSET, .size = SENSOR_MFG_SIZE,
    .banks = SENSOR_MFG_BANKS, .kind = IPRO_AI_KIND_SENSOR, .legacy_size = legacy_size,
};

const ipro_ai_slot_t *sensor_model_slot(void)
{
    return &s_slot;
}

static int parse_sns1(const uint8_t *b, uint32_t room)
{
    const uint32_t ver = rd32(b + 4), bytes = rd32(b + 8);
    if (memcmp(b, "SNS1", 4) || !((ver == 1u && bytes == SNS1_V1_BYTES) ||
                                  (ver == 2u && bytes == SNS1_V2_BYTES)) || bytes > room) {
        s_why = "no SNS1 block after the model";
        return -1;
    }
    sensor_cfg_t c;
    memset(&c, 0, sizeof(c));
    c.n = rd32(b + 12);
    for (uint32_t i = 0; i < SM_MAX_CLASSES; i++) {
        memcpy(c.labels[i], b + 16 + 16 * i, SM_LABEL_LEN - 1);
    }
    c.rate = rdf(b + 144);
    c.front.channels = rd32(b + 148);
    c.front.window = rd32(b + 152);
    c.front.mode = rd32(b + 156);
    for (uint32_t i = 0; i < SF_MAX_CH; i++) {
        c.front.mean[i] = rdf(b + 160 + 4 * i);
        c.front.std[i] = rdf(b + 192 + 4 * i);
    }
    c.front.in_scale = rdf(b + 224);
    c.front.in_zp = (int32_t)rd32(b + 228);
    c.out_scale = rdf(b + 232);
    memcpy(c.source, b + 240, sizeof(c.source) - 1);
    c.sns_bytes = bytes;
    if (ver == 2u) {
        for (uint32_t i = 0; i < SM_MAX_CLASSES; i++) {
            c.floor[i] = rdf(b + 256 + 4 * i);
        }
        c.unknown = rd32(b + 288) & 1u;
        memcpy(c.size, b + 292, sizeof(c.size) - 1);
    }
    if (c.n < 1 || c.n > SM_MAX_CLASSES || c.front.channels < 1 || c.front.channels > SF_MAX_CH ||
        c.front.mode > SF_WAVE || !(c.front.in_scale > 0.0f) || !(c.rate > 0.0f) ||
        (c.front.mode == SF_SPECTROGRAM ? c.front.window < SF_NFFT : c.front.window < 2u)) {
        s_why = "SNS1 block out of range";
        return -1;
    }
    for (uint32_t i = 0; i < c.front.channels; i++) {
        if (!(c.front.std[i] > 0.0f)) {
            s_why = "SNS1 deviation not positive";
            return -1;
        }
    }
    s_cfg = c;
    return 0;
}

int sensor_model_init(void)
{
    ipro_ai_blob_t found;
    ipro_ai_inm_header_t inm;

    if (s_model) {
        return 0;
    }
    const int found_rc = ipro_ai_store_open(&s_slot, &found);
    s_found = found;
    if (found_rc != 0 || found.state == IPRO_AI_SLOT_NONE) {
        /* "empty" keeps the V1.1 wording the host shows */
        s_why = !strcmp(found.why, "empty") ? "no model in mfg" : found.why;
        IPRO_LOGW(LOG_TAG, "no model: %s", found.why);
        return -1;
    }
    const uint8_t *blob = found.data;
    const uint32_t room = found.bytes;
    if (ipro_ai_inm_peek(blob, room, &inm) != IPRO_AI_OK || inm.bytes + SNS1_V1_BYTES > room) {
        s_why = "no model in mfg";
        return -1;
    }
    if (parse_sns1(blob + inm.bytes, room - inm.bytes) != 0) {
        return -1;
    }
    if (found.state == IPRO_AI_SLOT_COMMITTED) {
        IPRO_LOGI(LOG_TAG, "store: bank %c, install #%lu, CRC %08lx checked", 'A' + found.bank,
                  (unsigned long)found.seq, (unsigned long)found.crc);
    } else {
        IPRO_LOGI(LOG_TAG, "store: V1.1 payload, no record (not CRC-checked)");
    }
    s_cfg.inm_bytes = inm.bytes;

    if (!s_lock) {
        s_lock = xSemaphoreCreateMutex();
    }
    if (!s_lock || ipro_ai_init() != IPRO_AI_OK) {
        s_why = "out of memory";
        return -1;
    }
    /* Straight from XIP: the runtime copies every section to the heap. */
    const ipro_ai_model_cfg_t mc = {
        .inm = blob, .max_bytes = inm.bytes, .input_bytes = SF_IMG_BYTES, .name = "sensor",
    };
    const int rc = ipro_ai_model_open(&mc, &s_model);
    if (rc != IPRO_AI_OK) {
        IPRO_LOGE(LOG_TAG, "NPU load failed: %s", ipro_ai_strerror(rc));
        s_why = rc == IPRO_AI_E_NOMEM ? "out of memory" : "NPU load failed";
        return -1;
    }

    ipro_ai_model_info_t info;
    ipro_ai_tensor_info_t out;
    ipro_ai_model_get_info(s_model, &info);
    ipro_ai_tensor_get_info(s_model, IPRO_AI_OUTPUT, 0, &out);
    if ((uint32_t)out.w * out.h != SM_OUT_POS || out.c < s_cfg.n) {
        IPRO_LOGE(LOG_TAG, "output %ux%ux%u, expected 4x4x>=%lu", out.h, out.w, out.c,
                  (unsigned long)s_cfg.n);
        s_why = "model output shape";
        ipro_ai_model_close(s_model);
        s_model = NULL;
        return -1;
    }
    s_pitch = out.c_pitch;
    s_out_zp = (uint32_t)out.zero_point;
    s_out_scale = out.scale;
    s_out = out;
    s_cls_cfg = (ipro_ai_classify_cfg_t){ .n = s_cfg.n, .labels = s_cfg.labels,
                                          .floor = s_cfg.unknown ? s_cfg.floor : NULL };
    ipro_ai_adapter_classify(&s_cls, &s_cls_cfg);
    /* Studio's anomaly mode: the taught class against a made-up "anomaly"
     * class whose bias sets the false-alarm rate, so the alarm is the anomaly
     * logit winning - held two windows before it starts or ends */
    s_is_anomaly = s_cfg.n == 2 && !strcmp(s_cfg.labels[1], "anomaly");
    s_anom_cfg = (ipro_ai_anomaly_cfg_t){ .mode = IPRO_AI_ANOM_LOGIT_MARGIN, .normal_index = 0,
                                          .anomaly_index = 1, .threshold = 0.0f, .hold = 2,
                                          .release = 2 };
    ipro_ai_adapter_anomaly(&s_anom, &s_anom_cfg);
    const ipro_ai_app_cfg_t ac = { .name = s_cfg.source, .model_id = 3, .publish = true };
    ipro_ai_event_init(16, true);
    if (ipro_ai_app_create(&ac, &s_app) != IPRO_AI_OK ||
        ipro_ai_app_set_model(s_app, s_model, &s_cls) != IPRO_AI_OK) {
        ipro_ai_app_destroy(s_app);
        s_app = NULL;
        s_why = "out of memory";
        ipro_ai_model_close(s_model);
        s_model = NULL;
        return -1;
    }
    s_why = "";
    IPRO_LOGI(LOG_TAG, "model %lu B, %u/%u layers on the NPU, %lu classes, %s %lu Hz x%lu, "
              "window %lu, %s", (unsigned long)inm.bytes, info.npu_layers, info.layers,
              (unsigned long)s_cfg.n, s_cfg.source, (unsigned long)lrintf(s_cfg.rate),
              (unsigned long)s_cfg.front.channels, (unsigned long)s_cfg.front.window,
              s_cfg.front.mode == SF_SPECTROGRAM ? "spectrogram" : "wave");
    return 0;
}

const sensor_cfg_t *sensor_model_cfg(void)
{
    return s_model ? &s_cfg : NULL;
}

const char *sensor_model_why(void)
{
    return s_why;
}

int sensor_model_run(const int8_t *img, float logits[SM_MAX_CLASSES], int8_t *raw, uint32_t *us)
{
    if (!s_model) {
        return -1;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    const int rc = run_locked(img, logits, raw);
    if (us) {
        *us = s_last_us;     /* before the link's INFER can run and overwrite it */
    }
    xSemaphoreGive(s_lock);
    return rc;
}

static int run_locked(const int8_t *img, float logits[SM_MAX_CLASSES], int8_t *raw)
{
    const uint64_t t0 = mtimer_get_time_us();
    uint8_t *q = ipro_ai_input_buffer(s_model);
    for (uint32_t i = 0; i < SF_IMG_BYTES; i++) {
        q[i] = (uint8_t)img[i] ^ 0x80u;
    }
    const uint8_t *out = NULL;
    if (ipro_ai_invoke(s_model, NULL, &out) != IPRO_AI_OK) {
        return -1;
    }
    for (uint32_t k = 0; k < SM_MAX_CLASSES; k++) {
        int32_t acc = 0;
        for (uint32_t p = 0; p < SM_OUT_POS; p++) {
            const uint8_t b = k < s_pitch ? out[p * s_pitch + k] : (uint8_t)s_out_zp;
            acc += (int32_t)b - (int32_t)s_out_zp;
            if (raw) {
                raw[p * SM_MAX_CLASSES + k] = (int8_t)(b ^ 0x80u);
            }
        }
        logits[k] = (float)acc / (float)SM_OUT_POS * s_out_scale;
    }
    s_last_us = (uint32_t)(mtimer_get_time_us() - t0);
    return 0;
}

/* ---------------------------------------------------------------- framework */

int sensor_model_classify(const int8_t *img, uint64_t t_us, ipro_ai_result_t *res, uint32_t *us)
{
    if (!s_model) {
        return -1;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    uint8_t *q = ipro_ai_input_buffer(s_model);
    for (uint32_t i = 0; i < SF_IMG_BYTES; i++) {
        q[i] = (uint8_t)img[i] ^ 0x80u;
    }
    int rc = ipro_ai_app_process(s_app, NULL, 0, 0, t_us, res);
    if (rc == IPRO_AI_OK && s_is_anomaly) {
        ipro_ai_result_t a = *res;
        if (s_anom.post(&s_anom, res->raw, &s_out, &a) == IPRO_AI_OK) {
            ipro_ai_event_t e = { .model_id = 3, .t_us = t_us };
            strncpy(e.model, s_cfg.source, IPRO_AI_EVENT_NAME_MAX);
            s_anom.events(&s_anom, &a, &e);
        }
    }
    if (us) {
        *us = res->npu_us;
        s_last_us = res->npu_us;
    }
    xSemaphoreGive(s_lock);
    return rc == IPRO_AI_OK ? 0 : -1;
}

int sensor_model_metrics(ipro_ai_app_metrics_t *m)
{
    return s_app ? ipro_ai_app_get_metrics(s_app, m) : -1;
}

int sensor_model_run_raw(const uint8_t *in, uint32_t bytes, uint8_t *out, uint32_t cap,
                         uint32_t *out_bytes, uint32_t *us)
{
    if (!s_model) {
        return -1;
    }
    if (bytes != SF_IMG_BYTES || cap < s_out.bytes) {
        return -2;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    memcpy(ipro_ai_input_buffer(s_model), in, bytes);
    const uint8_t *o = NULL;
    const uint64_t t0 = mtimer_get_time_us();
    const int rc = ipro_ai_invoke(s_model, NULL, &o);
    *us = (uint32_t)(mtimer_get_time_us() - t0);
    if (rc == IPRO_AI_OK) {
        memcpy(out, o, s_out.bytes);
        *out_bytes = s_out.bytes;
    }
    xSemaphoreGive(s_lock);
    return rc == IPRO_AI_OK ? 0 : -4;
}

static float zero_sample(void *ctx, uint32_t ch, uint32_t i)
{
    (void)ctx;
    return (float)((int32_t)(ch * 977u + i * 131u) % 2001 - 1000);   /* not flat: a real FFT */
}

int sensor_model_bench(char *line, uint32_t size, uint32_t n)
{
    if (!s_model) {
        return ipro_ai_devinfo_bench(line, size, "BENCH", NULL, n, NULL, 0);
    }
    /* the front end once on a synthetic window: its time is pre_us */
    int8_t *img = pvPortMalloc(SF_IMG_BYTES);
    uint32_t pre_us = UINT32_MAX;
    if (img) {
        const uint64_t t0 = mtimer_get_time_us();
        if (sensor_front(&s_cfg.front, zero_sample, NULL, img) == 0) {
            pre_us = (uint32_t)(mtimer_get_time_us() - t0);
        }
        vPortFree(img);
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    const int k = ipro_ai_devinfo_bench(line, size, "BENCH", s_model, n, NULL, pre_us);
    xSemaphoreGive(s_lock);
    return k;
}

int sensor_model_describe(char *line, uint32_t size)
{
    char name[32];
    snprintf(name, sizeof(name), "%s-%s", s_cfg.source, s_cfg.size[0] ? s_cfg.size : "model");
    return ipro_ai_devinfo_model(line, size, "MODEL", &s_found, s_model, s_model ? name : NULL,
                                 s_model ? s_cfg.front.in_scale : 0.0f);
}
