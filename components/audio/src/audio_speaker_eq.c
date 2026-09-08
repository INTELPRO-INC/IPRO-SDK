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
 * @file audio_speaker_eq.c
 * @brief Fixed-point speaker correction. See audio_speaker_eq.h.
 */

#include "audio_speaker_eq.h"

/*
 * Two peaking cuts in cascade, fitted to the measured acoustic response of the
 * IPRO7AI EVK speaker at 32 kHz. Both sections only attenuate, so the cascade
 * needs no headroom and cannot clip (peak |H| = -0.01 dB).
 *
 *   1. fc 2850 Hz, Q 2.65, -13.0 dB - the main cone resonance, +14 dB raw and
 *      sitting where the ear is most sensitive.
 *   2. fc 7225 Hz, Q 4.90,  -9.0 dB - a narrower peak, +7.7 dB raw. Corrected
 *      despite being narrow because it reproduced across two independent
 *      measurement sessions (7171 Hz and 7225 Hz, 0.75% apart), which is what
 *      separates a speaker property from a microphone-position artefact.
 *
 * Flatness over 1.2-10 kHz: 4.80 -> 2.57 dB rms, peak-to-trough 16.8 -> 11.4.
 *
 * The cascade costs about 4 dB of broadband level - unavoidable, since a
 * cuts-only EQ flattens by removing peaks rather than filling dips. Make it up
 * with the existing volume control if needed; do not bake a boost in here,
 * where it would silently cost headroom on peaks.
 *
 * Coefficients are Q28, normalised so a0 == 1. b1 == a1 in each section is not
 * a slip: for a peaking EQ both are -2*cos(w0).
 */
#define EQ_Q        28
#define EQ_SECTIONS 2

typedef struct {
    int32_t b0, b1, b2, a1, a2;
} eq_coef_t;

static const eq_coef_t s_coef[EQ_SECTIONS] = {
    /* 2850 Hz, Q 2.65, -13.0 dB */
    { 232037724, -375494923, 211040057, -375494923, 174642325 },
    /* 7225 Hz, Q 4.90,  -9.0 dB */
    { 243356157,  -69596615, 215771979,  -69596615, 190692679 },
};

/*
 * Direct Form I per section. State is kept in sample units, costing about
 * 1 LSB of quantisation noise - some 6 bits below the DAC's own LSB, since the
 * output stage keeps only 10 bits.
 */
typedef struct {
    int32_t x1, x2, y1, y2;
} eq_state_t;

static eq_state_t s_state[EQ_SECTIONS];
static bool       s_on = true;

int32_t audio_speaker_eq_sample(int32_t x)
{
    if (!s_on) {
        return x;
    }

    for (unsigned i = 0; i < EQ_SECTIONS; i++) {
        const eq_coef_t *c = &s_coef[i];
        eq_state_t      *st = &s_state[i];

        int64_t acc = (int64_t)c->b0 * x
                    + (int64_t)c->b1 * st->x1
                    + (int64_t)c->b2 * st->x2
                    - (int64_t)c->a1 * st->y1
                    - (int64_t)c->a2 * st->y2;

        int32_t y = (int32_t)((acc + (1 << (EQ_Q - 1))) >> EQ_Q);

        if (y >  32767) y =  32767;
        if (y < -32768) y = -32768;

        st->x2 = st->x1; st->x1 = x;
        st->y2 = st->y1; st->y1 = y;
        x = y;
    }
    return x;
}

void audio_speaker_eq_reset(void)
{
    for (unsigned i = 0; i < EQ_SECTIONS; i++) {
        s_state[i].x1 = s_state[i].x2 = s_state[i].y1 = s_state[i].y2 = 0;
    }
}

void audio_speaker_eq_enable(bool on)
{
    if (on != s_on) {
        audio_speaker_eq_reset();   /* stale state would click on re-entry */
        s_on = on;
    }
}

bool audio_speaker_eq_enabled(void)
{
    return s_on;
}
