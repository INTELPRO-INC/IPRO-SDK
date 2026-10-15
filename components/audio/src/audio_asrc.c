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
 * @file audio_asrc.c
 * @brief Integer streaming resampler + FIFO level controller (see audio_asrc.h)
 *
 * Output k sits at input position pos_k = pos_0 + k * step (Q32.32). With
 * n0 = floor(pos) and mu = pos - n0 it is
 *     y = sum_j x[n0 - 7 + j] * h_mu[j],   j = 0..15
 * where h_mu is interpolated between the two nearest of 128 tabulated phases.
 * buf[] keeps the 7 samples before the next output's n0 across calls.
 */
#include "audio_asrc.h"

#include <string.h>

#define HALF       (AUDIO_ASRC_TAPS / 2)
#define ONE_Q32    ((uint64_t)1 << 32)
#define BUF_LEN    (AUDIO_ASRC_TAPS + AUDIO_ASRC_MAX_IN)
#define PHASE_BITS 7                       /* 128 phases */
#define SUB_BITS   15                      /* interpolation between phases, Q15 */

int audio_asrc_init(audio_asrc_t *s, uint32_t in_mhz, uint32_t out_mhz)
{
    if (s == NULL || in_mhz == 0 || out_mhz == 0) {
        return -1;
    }
    uint64_t step = ((uint64_t)in_mhz << 32) / out_mhz;
    if (step > 4 * ONE_Q32 || step < ONE_Q32 / 4) {
        return -1;
    }
    memset(s, 0, sizeof(*s));
    s->step_nom = step;
    s->step = step;
    audio_asrc_reset(s);
    return 0;
}

void audio_asrc_reset(audio_asrc_t *s)
{
    /* HALF - 1 samples of silence in front of the first input sample, which
     * is where the first output lands. */
    memset(s->buf, 0, sizeof(s->buf));
    s->count = HALF - 1;
    s->pos = (uint64_t)(HALF - 1) << 32;
}

void audio_asrc_trim_ppm_x100(audio_asrc_t *s, int32_t ppm_x100)
{
    int64_t delta = ((int64_t)s->step_nom * ppm_x100) / 100000000LL;
    s->step = (uint64_t)((int64_t)s->step_nom + delta);
}

static int16_t convolve(const int16_t *x, uint32_t frac)
{
    uint32_t ph  = frac >> (32 - PHASE_BITS);
    int32_t  sub = (int32_t)((frac >> (32 - PHASE_BITS - SUB_BITS)) & ((1u << SUB_BITS) - 1u));
    const int16_t *c0 = g_audio_asrc_coef[ph];
    const int16_t *c1 = g_audio_asrc_coef[ph + 1];
    int32_t c[AUDIO_ASRC_TAPS];
    int32_t sum = 0;
    int32_t acc = 0;

    /* Interpolated taps no longer sum to exactly 32768; the tap at n0
     * (index HALF - 1, the largest for mu < 1/2) takes the rounding so DC
     * still passes at unity. */
    for (int j = 0; j < AUDIO_ASRC_TAPS; j++) {
        c[j] = c0[j] + (((int32_t)(c1[j] - c0[j]) * sub) >> SUB_BITS);
        sum += c[j];
    }
    c[HALF - 1] += 32768 - sum;

    /* Each Q15 product is pre-shifted by 4 so sixteen of them fit in 32 bits. */
    for (int j = 0; j < AUDIO_ASRC_TAPS; j++) {
        acc += ((int32_t)x[j] * c[j]) >> 4;
    }
    acc = (acc + (1 << 10)) >> 11;
    if (acc > 32767) {
        acc = 32767;
    } else if (acc < -32768) {
        acc = -32768;
    }
    return (int16_t)acc;
}

uint32_t audio_asrc_process(audio_asrc_t *s, const int16_t *in, uint32_t n_in,
                          int16_t *out, uint32_t out_cap, uint32_t *in_used)
{
    uint32_t take = n_in;
    if (take > AUDIO_ASRC_MAX_IN) {
        take = AUDIO_ASRC_MAX_IN;
    }
    if (take > BUF_LEN - s->count) {
        take = BUF_LEN - s->count;
    }
    memcpy(&s->buf[s->count], in, take * sizeof(int16_t));
    s->count += take;
    if (in_used != NULL) {
        *in_used = take;
    }

    uint32_t produced = 0;
    while (produced < out_cap) {
        uint32_t n0 = (uint32_t)(s->pos >> 32);
        if (n0 + HALF >= s->count) {
            break;                                  /* needs x[n0 + 8] */
        }
        out[produced++] = convolve(&s->buf[n0 - (HALF - 1)], (uint32_t)s->pos);
        s->pos += s->step;
    }

    /* Keep the HALF - 1 samples in front of the next output's n0. */
    uint32_t n0 = (uint32_t)(s->pos >> 32);
    uint32_t drop = (n0 > HALF - 1) ? n0 - (HALF - 1) : 0;
    if (drop > s->count) {
        drop = s->count;
    }
    if (drop != 0) {
        memmove(s->buf, &s->buf[drop], (s->count - drop) * sizeof(int16_t));
        s->count -= drop;
        s->pos -= (uint64_t)drop << 32;
    }
    return produced;
}

uint32_t audio_asrc_pending(const audio_asrc_t *s)
{
    uint32_t n0 = (uint32_t)(s->pos >> 32);
    return (s->count > n0) ? s->count - n0 : 0;
}

/* ---------------------------------------------------------- level control */

/* 0.5 ppm per sample of error: at 16 kHz a FIFO settles in ~2 min and a
 * 50 ppm crystal offset costs ~100 samples of level. The level IIR spans
 * ~128 readings (5 s at 40 ms) so a producer whose blocks beat against the
 * consumer's (320-sample mic frames against 640-sample radio bursts) does
 * not drive the trim. Fast gains followed that beat by +/-1600 ppm on the
 * board (2026-09-26). */
#define CTL_GAIN_X100_PER_SAMPLE 50
#define CTL_SMOOTH_SHIFT         7

void audio_asrc_ctl_init(audio_asrc_ctl_t *c, int32_t target, int32_t max_ppm)
{
    c->target = target;
    c->max_x100 = max_ppm * 100;
    c->level_q8 = target << 8;
    c->primed = false;
}

int32_t audio_asrc_ctl_update(audio_asrc_ctl_t *c, int32_t level)
{
    if (!c->primed) {
        c->level_q8 = level << 8;
        c->primed = true;
    } else {
        c->level_q8 += ((level << 8) - c->level_q8) >> CTL_SMOOTH_SHIFT;
    }
    int32_t err_q8 = c->level_q8 - (c->target << 8);
    int32_t trim = (int32_t)(((int64_t)err_q8 * CTL_GAIN_X100_PER_SAMPLE) / 256);
    if (trim > c->max_x100) {
        trim = c->max_x100;
    } else if (trim < -c->max_x100) {
        trim = -c->max_x100;
    }
    return trim;
}
