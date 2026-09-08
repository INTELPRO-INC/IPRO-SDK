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
 * @file audio_bass_boost.c
 * @brief Fixed-point psychoacoustic bass enhancement. See the header.
 */

#include "audio_bass_boost.h"

/*
 * Coefficients are Q28, normalised so a0 == 1, designed at 32 kHz.
 *
 * State carries 12 fractional bits (BB_SB). That is not conservatism: with
 * plain integer state a 30 Hz section measured 0.0 dB SNR - completely buried
 * in its own quantisation noise - and the 55 Hz high-pass only managed 31 dB.
 * These sections have pole radii of 0.99+, where a 1 LSB state error
 * recirculates for thousands of samples. The speaker EQ gets away with integer
 * state because its sections sit at 2850 and 7225 Hz, radius 0.87-0.93.
 *
 * There is deliberately no DC-blocking filter after the rectifier. A
 * second-order high-pass has exactly zero gain at DC - sum(b) == 0 by
 * construction - so the 550 Hz sections already remove it; adding a 30 Hz
 * blocker changed the output by -46 dB and would have been the worst
 * conditioned filter in the chain.
 */
#define BB_Q    28
#define BB_SB   12
#define BB_LIM  ((1 << 27) - 1)

enum { BQ_HP380, BQ_HP55, BQ_LP380, BQ_HP550, BQ_LP1600, BQ_COUNT };

typedef struct { int32_t b0, b1, b2, a1, a2; } bb_coef_t;
typedef struct { int32_t x1, x2, y1, y2; } bb_state_t;

static const bb_coef_t s_coef[BQ_COUNT] = {
    [BQ_HP380] = {  254637960,  -509275920,  254637960,  -508566467,  241549916 },
    [BQ_HP55] = {  266393129,  -532786259,  266393129,  -532770725,  264366337 },
    [BQ_LP380] = {     354726,      709452,     354726,  -508566467,  241549916 },
    [BQ_HP550] = {  248696616,  -497393233,  248696616,  -495940219,  230410790 },
    [BQ_LP1600] = {    5390941,    10781883,    5390941,  -419021250,  172149560 },
};

/* keep: hp380 x2   bass: hp55 + lp380 x2   harm: hp550 x2 + lp1600 */
enum { S_K1, S_K2, S_B0, S_B1, S_B2, S_H1, S_H2, S_H3, S_COUNT };
static bb_state_t s_st[S_COUNT];

/*
 * Envelope followers, Q16 smoothing. Fast attack keeps transients intact,
 * slow release stops the synthetic part pumping between bass notes.
 */
#define BB_ATTACK_Q16   65128   /* 5 ms   */
#define BB_RELEASE_Q16  65519   /* 120 ms */

static int32_t  s_env_bass, s_env_harm;
static uint32_t s_gain_q8 = 128;      /* half the full envelope match */
static bool     s_on;                 /* off by default */

/*
 * always_inline is load-bearing, not decoration. The SDK builds at -Os, where
 * GCC declines to inline a function used eight times; bb_step stayed a real
 * call and the chain cost ~630 instructions plus eight prologue/epilogue pairs
 * per sample, which starved the DAC ring and dropped ISO packets.
 */
__attribute__((always_inline))
static inline int32_t bb_step(unsigned idx, unsigned st, int32_t x)
{
    const bb_coef_t *c = &s_coef[idx];
    bb_state_t      *s = &s_st[st];

    int64_t acc = (int64_t)c->b0 * x
                + (int64_t)c->b1 * s->x1
                + (int64_t)c->b2 * s->x2
                - (int64_t)c->a1 * s->y1
                - (int64_t)c->a2 * s->y2;

    int32_t y = (int32_t)((acc + (1 << (BB_Q - 1))) >> BB_Q);
    if (y >  BB_LIM) y =  BB_LIM;
    if (y < -BB_LIM) y = -BB_LIM;

    s->x2 = s->x1; s->x1 = x;
    s->y2 = s->y1; s->y1 = y;
    return y;
}

__attribute__((always_inline))
static inline int32_t bb_env(int32_t prev, int32_t v)
{
    int32_t k = (v > prev) ? BB_ATTACK_Q16 : BB_RELEASE_Q16;
    return (int32_t)(((int64_t)prev * k + (int64_t)v * (65536 - k)) >> 16);
}

int32_t audio_bass_boost_sample(int32_t x)
{
    if (!s_on) {
        return x;
    }

    int32_t v = x << BB_SB;

    /* What the speaker can still deliver, with the dead band taken out. */
    int32_t keep = bb_step(BQ_HP380, S_K2, bb_step(BQ_HP380, S_K1, v));

    /* The band it cannot. */
    /*
     * No sub-sonic high-pass ahead of the rectifier. It was measured to cost
     * 0.5 dB of dead-band rejection, i.e. nothing, because anything DC-ish it
     * would have removed is killed downstream anyway: a second-order high-pass
     * has exactly zero gain at DC, and there are two of them at 550 Hz.
     */
    int32_t bass = bb_step(BQ_LP380, S_B2, bb_step(BQ_LP380, S_B1, v));

    /*
     * Half-wave rectification, not full-wave: it yields a dense series
     * (2f, 3f, 4f ...) where a full-wave rectifier gives only even harmonics.
     * What the ear needs is the f spacing, not any individual harmonic.
     */
    int32_t harm = bb_step(BQ_LP1600, S_H3,
                   bb_step(BQ_HP550,  S_H2,
                   bb_step(BQ_HP550,  S_H1, bass > 0 ? bass : 0)));

    s_env_bass = bb_env(s_env_bass, bass > 0 ? bass : -bass);
    s_env_harm = bb_env(s_env_harm, harm > 0 ? harm : -harm);

    /*
     * Match the harmonic envelope to the original bass envelope.
     *
     * Rectification is homogeneous, so this is not just a constant: the
     * harmonic yield depends on the bass pitch. A 200 Hz note puts its third
     * harmonic inside the usable band while a 60 Hz note has to reach its
     * tenth, and the measured ratio swings 7-9 dB across the clips. Without
     * this, deep bass would come out weak and upper bass overblown.
     *
     * Both operands are shifted down first so a 32-bit divide suffices - a
     * 64-bit division here would be a libgcc call in the per-sample path.
     */
    uint32_t num = (uint32_t)(s_env_bass >> 4);
    uint32_t den = (uint32_t)(s_env_harm >> 4) + 1u;
    uint32_t g   = (num << 8) / den;                 /* Q8 */
    if (g < (1u << 8))  g = 1u << 8;
    if (g > (64u << 8)) g = 64u << 8;

    int64_t hp = (((int64_t)harm * (int32_t)g) >> 8);
    hp = (hp * (int32_t)s_gain_q8) >> 8;
    if (hp >  BB_LIM) hp =  BB_LIM;
    if (hp < -BB_LIM) hp = -BB_LIM;

    int32_t y = (int32_t)((keep + (int32_t)hp + (1 << (BB_SB - 1))) >> BB_SB);
    if (y >  32767) y =  32767;
    if (y < -32768) y = -32768;
    return y;
}

void audio_bass_boost_reset(void)
{
    for (unsigned i = 0; i < S_COUNT; i++) {
        s_st[i].x1 = s_st[i].x2 = s_st[i].y1 = s_st[i].y2 = 0;
    }
    s_env_bass = s_env_harm = 0;
}

void audio_bass_boost_enable(bool on)
{
    if (on != s_on) {
        audio_bass_boost_reset();
        s_on = on;
    }
}

bool audio_bass_boost_enabled(void) { return s_on; }

void audio_bass_boost_set_gain_q8(uint32_t g_q8)
{
    s_gain_q8 = (g_q8 > 1024u) ? 1024u : g_q8;   /* cap at 4x envelope match */
}

uint32_t audio_bass_boost_get_gain_q8(void) { return s_gain_q8; }
