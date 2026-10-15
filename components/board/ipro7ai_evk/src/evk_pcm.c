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
 * @file evk_pcm.c
 * @brief Integer-only PCM helpers. See evk_pcm.h.
 */

#include "evk_pcm.h"

#define SINE_LUT_BITS   8
#define SINE_LUT_SIZE   (1 << SINE_LUT_BITS)

/* One quarter wave would do, but a full period keeps the lookup branch-free.
 * sin(2*pi*i/256) * 32767, generated on the host. */
static const int16_t s_sine[SINE_LUT_SIZE] = {
        0,   804,  1608,  2410,  3212,  4011,  4808,  5602,  6393,  7179,  7962,  8739,  9512, 10278, 11039, 11793,
    12539, 13279, 14010, 14732, 15446, 16151, 16846, 17530, 18204, 18868, 19519, 20159, 20787, 21403, 22005, 22594,
    23170, 23731, 24279, 24811, 25329, 25832, 26319, 26790, 27245, 27683, 28105, 28510, 28898, 29268, 29621, 29956,
    30273, 30571, 30852, 31113, 31356, 31580, 31785, 31971, 32137, 32285, 32412, 32521, 32609, 32678, 32728, 32757,
    32767, 32757, 32728, 32678, 32609, 32521, 32412, 32285, 32137, 31971, 31785, 31580, 31356, 31113, 30852, 30571,
    30273, 29956, 29621, 29268, 28898, 28510, 28105, 27683, 27245, 26790, 26319, 25832, 25329, 24811, 24279, 23731,
    23170, 22594, 22005, 21403, 20787, 20159, 19519, 18868, 18204, 17530, 16846, 16151, 15446, 14732, 14010, 13279,
    12539, 11793, 11039, 10278,  9512,  8739,  7962,  7179,  6393,  5602,  4808,  4011,  3212,  2410,  1608,   804,
        0,  -804, -1608, -2410, -3212, -4011, -4808, -5602, -6393, -7179, -7962, -8739, -9512,-10278,-11039,-11793,
   -12539,-13279,-14010,-14732,-15446,-16151,-16846,-17530,-18204,-18868,-19519,-20159,-20787,-21403,-22005,-22594,
   -23170,-23731,-24279,-24811,-25329,-25832,-26319,-26790,-27245,-27683,-28105,-28510,-28898,-29268,-29621,-29956,
   -30273,-30571,-30852,-31113,-31356,-31580,-31785,-31971,-32137,-32285,-32412,-32521,-32609,-32678,-32728,-32757,
   -32767,-32757,-32728,-32678,-32609,-32521,-32412,-32285,-32137,-31971,-31785,-31580,-31356,-31113,-30852,-30571,
   -30273,-29956,-29621,-29268,-28898,-28510,-28105,-27683,-27245,-26790,-26319,-25832,-25329,-24811,-24279,-23731,
   -23170,-22594,-22005,-21403,-20787,-20159,-19519,-18868,-18204,-17530,-16846,-16151,-15446,-14732,-14010,-13279,
   -12539,-11793,-11039,-10278, -9512, -8739, -7962, -7179, -6393, -5602, -4808, -4011, -3212, -2410, -1608,  -804,
};

/* round(4096 * 10^(dB/20)) for -60..+48 dB, generated on the host. */
#define GAIN_DB_MIN  (-60)
#define GAIN_DB_MAX  48
static const int32_t s_gain_q12[GAIN_DB_MAX - GAIN_DB_MIN + 1] = {
    4, 5, 5, 6, 6, 7,
    8, 9, 10, 12, 13, 15,
    16, 18, 21, 23, 26, 29,
    33, 37, 41, 46, 52, 58,
    65, 73, 82, 92, 103, 115,
    130, 145, 163, 183, 205, 230,
    258, 290, 325, 365, 410, 460,
    516, 579, 649, 728, 817, 917,
    1029, 1154, 1295, 1453, 1631, 1830,
    2053, 2303, 2584, 2900, 3254, 3651,
    4096, 4596, 5157, 5786, 6492, 7284,
    8173, 9170, 10289, 11544, 12953, 14533,
    16306, 18296, 20529, 23034, 25844, 28997,
    32536, 36506, 40960, 45958, 51566, 57858,
    64917, 72838, 81726, 91698, 102887, 115441,
    129527, 145332, 163065, 182962, 205286, 230335,
    258440, 289975, 325357, 365056, 409600, 459579,
    515656, 578575, 649172, 728383, 817259, 916980,
    1028869,
};

uint32_t evk_pcm_isqrt(uint32_t v)
{
    uint32_t r = 0, bit = 1u << 30;

    while (bit > v) {
        bit >>= 2;
    }
    while (bit) {
        if (v >= r + bit) {
            v -= r + bit;
            r = (r >> 1) + bit;
        } else {
            r >>= 1;
        }
        bit >>= 2;
    }
    return r;
}

uint32_t evk_pcm_ilog2_q8(uint32_t v)
{
    uint32_t ip = 0, fp = 0;
    uint64_t m;

    if (v == 0) {
        return 0;
    }
    while (v >> (ip + 1)) {
        ip++;
    }
    /* Fraction: normalise v to [1,2) in Q31, square 8 times, collect bits. */
    m = ((uint64_t)v << 31) >> ip;
    for (int i = 0; i < 8; i++) {
        m = (m * m) >> 31;
        fp <<= 1;
        if (m >= (2ull << 31)) {
            m >>= 1;
            fp |= 1u;
        }
    }
    return (ip << 8) | fp;
}

int evk_pcm_dbfs_x10(uint32_t v)
{
    if (v == 0) {
        return -1200;
    }
    /* 20*log10(v) = 6.0206 * log2(v); in tenths of a dB with log2 in Q8:
     * 60.206 * q8 / 256. 20*log10(32768) = 90.309 dB -> 903 tenths. */
    int32_t db = (int32_t)(((int64_t)evk_pcm_ilog2_q8(v) * 60206 + 128000) / 256000);
    return db - 903;
}

void evk_pcm_level(const int16_t *pcm, uint32_t n, uint16_t *peak, uint16_t *rms)
{
    uint32_t pk = 0;
    uint64_t sumsq = 0;

    for (uint32_t i = 0; i < n; i++) {
        int32_t  v = pcm[i];
        uint32_t a = (uint32_t)(v < 0 ? -v : v);
        if (a > pk) {
            pk = a;
        }
        sumsq += (uint64_t)((int64_t)v * v);
    }
    if (peak) {
        *peak = (uint16_t)pk;
    }
    if (rms) {
        uint32_t r = n ? evk_pcm_isqrt((uint32_t)(sumsq / n)) : 0;
        *rms = (uint16_t)(r > 32767u ? 32767u : r);
    }
}

int32_t evk_pcm_gain_q12(int db)
{
    if (db < GAIN_DB_MIN) db = GAIN_DB_MIN;
    if (db > GAIN_DB_MAX) db = GAIN_DB_MAX;
    return s_gain_q12[db - GAIN_DB_MIN];
}

uint32_t evk_pcm_apply_gain(const int16_t *src, int16_t *dst, uint32_t n,
                            int32_t gain_q12)
{
    uint32_t clipped = 0;

    for (uint32_t i = 0; i < n; i++) {
        /* 64-bit: +48 dB is x251, and 32767 * 1028869 overflows 32 bits. */
        int32_t v = (int32_t)(((int64_t)src[i] * gain_q12) >> 12);
        if (v > 32767) {
            v = 32767;
            clipped++;
        } else if (v < -32768) {
            v = -32768;
            clipped++;
        }
        dst[i] = (int16_t)v;
    }
    return clipped;
}

uint32_t evk_pcm_sine_step(uint32_t freq_hz, uint32_t rate_hz)
{
    return rate_hz ? (uint32_t)(((uint64_t)freq_hz << 32) / rate_hz) : 0;
}

void evk_pcm_sine_fill(int16_t *dst, uint32_t n, uint32_t *phase, uint32_t step,
                       int32_t amp_q15)
{
    uint32_t ph = *phase;

    for (uint32_t i = 0; i < n; i++) {
        /* Linear interpolation between LUT entries: the raw 256-entry table
         * alone leaves spurs ~42 dB down, interpolated they sit near -70. */
        uint32_t idx  = ph >> (32 - SINE_LUT_BITS);
        int32_t  frac = (int32_t)((ph >> (32 - SINE_LUT_BITS - 16)) & 0xFFFFu);
        int32_t  a    = s_sine[idx];
        int32_t  b    = s_sine[(idx + 1u) & (SINE_LUT_SIZE - 1u)];
        int32_t  s    = a + (((b - a) * frac) >> 16);
        dst[i] = (int16_t)((s * amp_q15) >> 15);
        ph += step;
    }
    *phase = ph;
}
