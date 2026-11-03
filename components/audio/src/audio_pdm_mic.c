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
 * @file audio_pdm_mic.c
 * @brief PDM microphone -> 16 kHz, high-passed, +20 dB. See audio_pdm_mic.h.
 */
#include "audio_pdm_mic.h"

#include <math.h>
#include <stddef.h>

#include "audio_asrc.h"
#include "audio_capture.h"

#define PI_F        3.14159265f
#define OUT_MHZ     (AUDIO_PDM_MIC_RATE * 1000u)
#define DEFAULT_BUF (AUDIO_PDM_MIC_RATE * 2u * 2u)   /* 2 s */

static int          s_consumer = -1;
static audio_asrc_t s_asrc;
static struct { float b0, b1, b2, a1, a2, x1, x2, y1, y2; } s_hpf;
static int16_t      s_in[AUDIO_PDM_MIC_FRAME];
static uint32_t     s_in_pos, s_in_n;

/* RBJ cookbook high-pass, Q = 1/sqrt(2), history cleared */
static void hpf_reset(void)
{
    const float w0 = 2.0f * PI_F * AUDIO_PDM_MIC_HPF_HZ / (float)AUDIO_PDM_MIC_RATE;
    const float c = cosf(w0);
    const float alpha = sinf(w0) / (2.0f * 0.70710678f);
    const float a0 = 1.0f + alpha;

    s_hpf.b0 = (1.0f + c) / 2.0f / a0;
    s_hpf.b1 = -(1.0f + c) / a0;
    s_hpf.b2 = s_hpf.b0;
    s_hpf.a1 = -2.0f * c / a0;
    s_hpf.a2 = (1.0f - alpha) / a0;
    s_hpf.x1 = s_hpf.x2 = s_hpf.y1 = s_hpf.y2 = 0.0f;
}

int audio_pdm_mic_open(const audio_pdm_mic_cfg_t *cfg)
{
    audio_capture_cfg_t cap = {
        .type          = AUDIO_SRC_DMIC,
        .sample_rate   = AUDIO_PDM_MIC_RATE,
        .frame_samples = AUDIO_PDM_MIC_FRAME,
    };

    if (!cfg || s_consumer >= 0) {
        return -1;
    }
    cap.dmic.clk_pin         = cfg->clk_pin;
    cap.dmic.data_pin        = cfg->data_pin;
    cap.dmic.channel         = cfg->channel;
    cap.dmic.digital_gain_db = AUDIO_PDM_MIC_GAIN_DB;
    if (audio_capture_init(&cap) != 0) {
        return -1;
    }
    s_consumer = audio_capture_add_consumer(cfg->buf_bytes ? cfg->buf_bytes : DEFAULT_BUF);
    if (s_consumer < 0 || audio_capture_start() != 0) {
        audio_capture_deinit();
        s_consumer = -1;
        return -1;
    }
    audio_asrc_init(&s_asrc, AUDIO_PDM_MIC_IN_MHZ, OUT_MHZ);
    hpf_reset();
    s_in_pos = s_in_n = 0;
    return 0;
}

void audio_pdm_mic_close(void)
{
    if (s_consumer < 0) {
        return;
    }
    audio_capture_stop();
    audio_capture_deinit();
    s_consumer = -1;
    s_in_pos = s_in_n = 0;
}

bool audio_pdm_mic_is_open(void)
{
    return s_consumer >= 0;
}

bool audio_pdm_mic_pending(void)
{
    return s_in_pos < s_in_n;
}

uint32_t audio_pdm_mic_read(int16_t *dst, uint32_t max, uint32_t timeout_ms)
{
    uint32_t out = 0;

    if (s_consumer < 0 || !dst || max == 0) {
        return 0;
    }
    if (s_in_pos >= s_in_n) {
        const size_t got = audio_capture_read(s_consumer, s_in, sizeof(s_in), timeout_ms);
        s_in_pos = 0;
        s_in_n = (uint32_t)(got / sizeof(int16_t));
    }
    while (s_in_pos < s_in_n && out < max) {
        uint32_t used = 0;
        const uint32_t n = audio_asrc_process(&s_asrc, &s_in[s_in_pos], s_in_n - s_in_pos,
                                              &dst[out], max - out, &used);
        for (uint32_t i = out; i < out + n; i++) {
            const float x = (float)dst[i];
            const float y = s_hpf.b0 * x + s_hpf.b1 * s_hpf.x1 + s_hpf.b2 * s_hpf.x2 -
                            s_hpf.a1 * s_hpf.y1 - s_hpf.a2 * s_hpf.y2;
            s_hpf.x2 = s_hpf.x1;
            s_hpf.x1 = x;
            s_hpf.y2 = s_hpf.y1;
            s_hpf.y1 = y;
            const long v = lrintf(y * AUDIO_PDM_MIC_SW_GAIN);
            dst[i] = (int16_t)(v > 32767 ? 32767 : v < -32768 ? -32768 : v);
        }
        out += n;
        s_in_pos += used;
        if (used == 0 && n == 0) {
            break;
        }
    }
    return out;
}
