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
 * @file spk_model.c
 * @brief Model B voiceprint on the NPU + CPU head. See spk_model.h.
 *
 * The trunk runs through the IPRO Edge AI runtime (components/ai/runtime,
 * ipro_ai.h), which owns the NPU lock and the input buffer; the log-mel front
 * end is components/ai/preproc's, the pooling and projection head runs as the
 * embedding adapter's head (components/ai/app). s_lock lets the USB link's
 * RUN, FEAT and BENCH use the model beside the voice task.
 */

#include "spk_model.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include "hal_mtimer.h"
#include "ipro_ai.h"
#include "ipro_ai_app.h"
#include "ipro_ai_devinfo.h"
#include "utils_crc.h"
#include "ipro_log.h"

#include "audio/fbank.h"
#include "ipro_ai_preproc.h"

#define LOG_TAG       "SPKM"
#define MFG_PARTITION "mfg"
#define SPKB_HDR      128u
#define SPKB_VERSION  1u
#define N_MELS        80u

typedef struct {
    char     magic[4];
    uint32_t version, hdr_bytes, frames, mels, out_t, out_f, out_c, dim;
    float    in_scale;
    int32_t  in_zero;
    float    out_scale;
    int32_t  out_zero;
    uint32_t inm_off, inm_bytes, fcw_off, fcb_off, total, crc;
    char     name[32];
} spkb_hdr_t;

static ipro_ai_model_t   *s_model;
static spkb_hdr_t         s_h;
static FBankContext      *s_fbank;
static float            (*s_feat)[FBANK_N_MELS];   /* [FBANK_SEQ_LEN][80] */
static uint32_t           s_pitch;                 /* trunk output: bytes per position */
static float             *s_fc_w;                  /* [dim][2*F*C] */
static float             *s_fc_b;                  /* [dim] */
static float             *s_stats;                 /* [2*F*C] */
static uint32_t           s_last_us, s_pre_us = UINT32_MAX;
static ipro_ai_blob_t     s_found;                 /* what the store reported (MODEL) */
static SemaphoreHandle_t  s_lock;
static ipro_ai_adapter_t  s_embed;
static ipro_ai_embed_cfg_t s_embed_cfg;

static int head(void *ctx, const uint8_t *out, const ipro_ai_tensor_info_t *ti, float *emb, uint32_t dim);

/* IEEE half -> float. The head is stored as f16 to halve the blob. */
static float half_to_float(uint16_t h)
{
    const uint32_t sign = (uint32_t)(h & 0x8000u) << 16;
    uint32_t exp = (h >> 10) & 0x1fu;
    uint32_t man = h & 0x3ffu;
    uint32_t bits;

    if (exp == 0) {
        if (man == 0) {
            bits = sign;
        } else {                                   /* subnormal: normalise */
            exp = 127 - 15 + 1;
            while (!(man & 0x400u)) {
                man <<= 1;
                exp--;
            }
            bits = sign | (exp << 23) | ((man & 0x3ffu) << 13);
        }
    } else if (exp == 0x1f) {
        bits = sign | 0x7f800000u | (man << 13);
    } else {
        bits = sign | ((exp + 127 - 15) << 23) | (man << 13);
    }
    float f;
    memcpy(&f, &bits, sizeof(f));
    return f;
}

static int header_ok(const spkb_hdr_t *h, uint32_t room)
{
    if (memcmp(h->magic, "SPKB", 4) != 0) {
        IPRO_LOGE(LOG_TAG, "no SPKB model in mfg - flash model_b.spkb with --mfg");
        return 0;
    }
    /* 64-bit sums: the header can come over USB (TDEP), and a crafted one
     * must not wrap past the checks. */
    if (h->version != SPKB_VERSION || h->hdr_bytes != SPKB_HDR || h->total > room ||
        h->total < SPKB_HDR ||
        h->frames != SPK_MODEL_FRAMES || h->mels != N_MELS || h->dim != EMBEDDING_DIM ||
        h->out_t == 0 || h->out_f == 0 || h->out_c == 0 || h->out_f > 1024u || h->out_c > 1024u ||
        (uint64_t)h->inm_off + h->inm_bytes > h->total ||
        (uint64_t)h->fcw_off + (uint64_t)h->dim * 2u * h->out_f * h->out_c * 2u > h->total ||
        (uint64_t)h->fcb_off + (uint64_t)h->dim * 4u > h->total) {
        IPRO_LOGE(LOG_TAG, "SPKB v%lu does not fit this firmware (frames %lu, dim %lu of %u, "
                  "total %lu of %lu)", (unsigned long)h->version, (unsigned long)h->frames,
                  (unsigned long)h->dim, (unsigned)EMBEDDING_DIM, (unsigned long)h->total,
                  (unsigned long)room);
        return 0;
    }
    return 1;
}

/* A V1.1 board's payload (flashed with --mfg, no store record): its own header
 * says how long it is, and spk_model_init() checks its CRC. */
static uint32_t legacy_size(const uint8_t *blob, uint32_t room)
{
    spkb_hdr_t h;

    memcpy(&h, blob, sizeof(h));
    return !memcmp(h.magic, "SPKB", 4) && h.total >= SPKB_HDR && h.total <= room ? h.total : 0;
}

static const ipro_ai_slot_t s_slot = {
    .partition = MFG_PARTITION, .offset = VOICE_MFG_OFFSET, .size = VOICE_MFG_SIZE,
    .banks = 1, .kind = IPRO_AI_KIND_VOICE, .legacy_size = legacy_size,
};

const ipro_ai_slot_t *spk_model_slot(void)
{
    return &s_slot;
}

int spk_model_payload_ok(const uint8_t *blob, uint32_t bytes)
{
    spkb_hdr_t h;

    if (!blob || bytes < SPKB_HDR) {
        return 0;
    }
    memcpy(&h, blob, sizeof(h));
    ipro_ai_inm_header_t inm;
    return header_ok(&h, bytes) && h.total == bytes &&
           utils_crc32(blob + SPKB_HDR, h.total - SPKB_HDR) == h.crc &&
           ipro_ai_inm_peek(blob + h.inm_off, h.inm_bytes, &inm) == IPRO_AI_OK &&
           inm.bytes == h.inm_bytes && inm.ext_input;
}

/* what spk_model_init() allocated, after a failure */
static void drop(void)
{
    vPortFree(s_fc_w);
    vPortFree(s_fc_b);
    vPortFree(s_stats);
    vPortFree(s_fbank);
    vPortFree(s_feat);
    s_fc_w = s_fc_b = s_stats = NULL;
    s_fbank = NULL;
    s_feat = NULL;
}

int spk_model_init(void)
{
    ipro_ai_blob_t found;

    if (s_model) {
        return 0;
    }
    if (!s_lock) {
        s_lock = xSemaphoreCreateMutex();
    }
    const int found_rc = ipro_ai_store_open(&s_slot, &found);
    s_found = found;
    if (!s_lock || found_rc != 0 || found.state == IPRO_AI_SLOT_NONE) {
        IPRO_LOGE(LOG_TAG, "no voiceprint model: %s - deploy one, or flash model_b.spkb with --mfg",
                  found.why);
        return -1;
    }
    const uint8_t *blob = found.data;
    const uint32_t room = found.bytes;
    if (found.state == IPRO_AI_SLOT_COMMITTED) {
        IPRO_LOGI(LOG_TAG, "store: install #%lu, CRC %08lx checked", (unsigned long)found.seq,
                  (unsigned long)found.crc);
    }
    memcpy(&s_h, blob, sizeof(s_h));
    s_h.name[sizeof(s_h.name) - 1] = 0;
    if (!header_ok(&s_h, room)) {
        return -1;
    }
    const uint32_t crc = utils_crc32(blob + SPKB_HDR, s_h.total - SPKB_HDR);
    if (crc != s_h.crc) {
        IPRO_LOGE(LOG_TAG, "SPKB crc %08lx, header says %08lx - reflash the model",
                  (unsigned long)crc, (unsigned long)s_h.crc);
        return -1;
    }

    const uint32_t n_in = 2u * s_h.out_f * s_h.out_c;
    s_fc_w  = pvPortMalloc(s_h.dim * n_in * sizeof(float));
    s_fc_b  = pvPortMalloc(s_h.dim * sizeof(float));
    s_stats = pvPortMalloc(n_in * sizeof(float));
    s_fbank = pvPortMalloc(sizeof(FBankContext));
    s_feat  = pvPortMalloc(sizeof(float) * FBANK_SEQ_LEN * FBANK_N_MELS);
    if (!s_fc_w || !s_fc_b || !s_stats || !s_fbank || !s_feat || ipro_ai_init() != IPRO_AI_OK) {
        IPRO_LOGE(LOG_TAG, "out of memory");
        drop();
        return -1;
    }
    const uint16_t *w16 = (const uint16_t *)(blob + s_h.fcw_off);
    for (uint32_t i = 0; i < s_h.dim * n_in; i++) {
        s_fc_w[i] = half_to_float(w16[i]);
    }
    memcpy(s_fc_b, blob + s_h.fcb_off, s_h.dim * sizeof(float));
    fbank_init(s_fbank);

    /* Straight from XIP: the runtime copies every section to the heap. The
     * input is one channel, read packed (frames x 80 bytes). */
    const ipro_ai_model_cfg_t mc = {
        .inm = blob + s_h.inm_off, .max_bytes = s_h.inm_bytes,
        .input_bytes = SPK_MODEL_FRAMES * N_MELS, .name = s_h.name,
    };
    const int rc = ipro_ai_model_open(&mc, &s_model);
    if (rc != IPRO_AI_OK) {
        IPRO_LOGE(LOG_TAG, "NPU model load failed: %s", ipro_ai_strerror(rc));
        drop();
        return -1;
    }

    ipro_ai_model_info_t info;
    ipro_ai_tensor_info_t out;
    ipro_ai_model_get_info(s_model, &info);
    ipro_ai_tensor_get_info(s_model, IPRO_AI_OUTPUT, 0, &out);
    if (out.h != s_h.out_t || out.w != s_h.out_f || out.c != s_h.out_c) {
        IPRO_LOGE(LOG_TAG, "trunk output %ux%ux%u, header says %lux%lux%lu", out.h, out.w, out.c,
                  (unsigned long)s_h.out_t, (unsigned long)s_h.out_f, (unsigned long)s_h.out_c);
        ipro_ai_model_close(s_model);
        s_model = NULL;
        drop();
        return -1;
    }
    s_pitch = out.c_pitch;
    s_embed_cfg = (ipro_ai_embed_cfg_t){ .dim = s_h.dim, .head = head };
    ipro_ai_adapter_embed(&s_embed, &s_embed_cfg);
    IPRO_LOGI(LOG_TAG, "model '%s': %lu B, trunk %u/%u layers on the NPU, out %ux%ux%u, "
              "dim %lu (heap free %u)", s_h.name, (unsigned long)s_h.total, info.npu_layers,
              info.layers, out.h, out.w, out.c, (unsigned long)s_h.dim,
              (unsigned)xPortGetFreeHeapSize());
    return 0;
}

int spk_model_ready(void)
{
    return s_model != NULL;
}

/* The CPU head: mean and deviation over time of every (frequency, channel),
 * a dim x 2FC projection, unit length. The embedding adapter's head. */
static int head(void *ctx, const uint8_t *out, const ipro_ai_tensor_info_t *ti, float *emb, uint32_t dim)
{
    (void)ctx;
    (void)ti;
    /* Trunk output: [T][F][pitch] int8 offset-binary, channels padded to 4. */
    const uint32_t t_n = s_h.out_t, f_n = s_h.out_f, c_n = s_h.out_c;
    const uint32_t pitch = s_pitch;

    const uint32_t fc = f_n * c_n;
    float *mean = s_stats, *std = s_stats + fc;
    memset(s_stats, 0, 2u * fc * sizeof(float));
    for (uint32_t t = 0; t < t_n; t++) {
        for (uint32_t j = 0; j < fc; j++) {
            const uint8_t b = out[(t * f_n + j / c_n) * pitch + j % c_n];
            const float v = (float)((int8_t)(b ^ 0x80) - s_h.out_zero) * s_h.out_scale;
            mean[j] += v;
            std[j] += v * v;
        }
    }
    for (uint32_t j = 0; j < fc; j++) {
        mean[j] /= (float)t_n;
        const float var = std[j] / (float)t_n - mean[j] * mean[j];
        std[j] = sqrtf((var > 0.0f ? var : 0.0f) + 1e-5f);
    }

    float nrm = 0.0f;
    for (uint32_t d = 0; d < dim; d++) {
        const float *w = s_fc_w + d * 2u * fc;
        float acc = s_fc_b[d];
        for (uint32_t j = 0; j < 2u * fc; j++) {
            acc += w[j] * s_stats[j];
        }
        emb[d] = acc;
        nrm += acc * acc;
    }
    nrm = sqrtf(nrm);
    for (uint32_t d = 0; d < dim; d++) {
        emb[d] = (nrm > 1e-8f) ? emb[d] / nrm : 0.0f;
    }
    return IPRO_AI_OK;
}

int spk_model_embed(const int16_t *pcm, float emb[EMBEDDING_DIM])
{
    if (!s_model) {
        return -1;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    const uint64_t t0 = mtimer_get_time_us();
    /* log-mel, int8 offset-binary (q ^ 0x80), single channel packed */
    int rc = ipro_ai_voice_front(s_fbank, pcm, SPK_MODEL_SAMPLES, SPK_MODEL_FRAMES, s_h.in_scale,
                                 s_h.in_zero, (float *)s_feat, ipro_ai_input_buffer(s_model));
    const uint64_t t1 = mtimer_get_time_us();
    const uint8_t *out = NULL;
    rc = rc ? rc : ipro_ai_invoke(s_model, NULL, &out);
    if (rc == IPRO_AI_OK) {
        ipro_ai_tensor_info_t ti;
        ipro_ai_result_t res;
        ipro_ai_tensor_get_info(s_model, IPRO_AI_OUTPUT, 0, &ti);
        s_embed_cfg.vec = emb;
        rc = s_embed.post(&s_embed, out, &ti, &res);
    }
    s_pre_us = (uint32_t)(t1 - t0);
    s_last_us = (uint32_t)(mtimer_get_time_us() - t0);
    xSemaphoreGive(s_lock);
    return rc == IPRO_AI_OK ? 0 : -1;
}

int spk_model_run_raw(const uint8_t *in, uint32_t bytes, uint8_t *out, uint32_t cap,
                      uint32_t *out_bytes, uint32_t *us)
{
    ipro_ai_tensor_info_t ti, to;

    if (!s_model) {
        return -1;
    }
    ipro_ai_tensor_get_info(s_model, IPRO_AI_INPUT, 0, &ti);
    ipro_ai_tensor_get_info(s_model, IPRO_AI_OUTPUT, 0, &to);
    if (bytes != ti.bytes || cap < to.bytes) {
        return -2;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    memcpy(ipro_ai_input_buffer(s_model), in, bytes);
    const uint8_t *o = NULL;
    const uint64_t t0 = mtimer_get_time_us();
    const int rc = ipro_ai_invoke(s_model, NULL, &o);
    *us = (uint32_t)(mtimer_get_time_us() - t0);
    if (rc == IPRO_AI_OK) {
        memcpy(out, o, to.bytes);
        *out_bytes = to.bytes;
    }
    xSemaphoreGive(s_lock);
    return rc == IPRO_AI_OK ? 0 : -4;
}

int spk_model_feat(const int16_t *pcm, uint32_t samples, uint8_t *out)
{
    if (!s_model || samples != SPK_MODEL_SAMPLES) {
        return -1;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    const int rc = ipro_ai_voice_front(s_fbank, pcm, SPK_MODEL_SAMPLES, SPK_MODEL_FRAMES, s_h.in_scale,
                                       s_h.in_zero, (float *)s_feat, out);
    xSemaphoreGive(s_lock);
    return rc == IPRO_AI_OK ? 0 : -1;
}

int spk_model_bench(char *line, uint32_t size, uint32_t n)
{
    if (!s_model) {
        return ipro_ai_devinfo_bench(line, size, "BENCH", NULL, n, NULL, 0);
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    /* the input buffer as the last voiceprint left it; pre_us is that voiceprint's front end */
    const int k = ipro_ai_devinfo_bench(line, size, "BENCH", s_model, n, NULL, s_pre_us);
    xSemaphoreGive(s_lock);
    return k;
}

int spk_model_describe(char *line, uint32_t size)
{
    return ipro_ai_devinfo_model(line, size, "MODEL", &s_found, s_model, s_model ? s_h.name : NULL,
                                 s_model ? s_h.in_scale : 0.0f);
}

void spk_model_get_info(spk_model_info_t *info)
{
    memset(info, 0, sizeof(*info));
    if (!s_model) {
        return;
    }
    memcpy(info->name, s_h.name, sizeof(info->name));
    info->blob_bytes = s_h.total;
    info->inm_bytes  = s_h.inm_bytes;
    info->dim        = s_h.dim;
    info->crc        = s_h.crc;
    info->last_us    = s_last_us;
}
