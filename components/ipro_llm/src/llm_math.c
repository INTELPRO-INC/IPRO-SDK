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
 * @file llm_math.c
 * @brief Scalar helpers: fp16, Walsh-Hadamard, ZC-RMSNorm, gates, CRC / hash.
 */
#include <math.h>
#include <string.h>

#include "llm_internal.h"

uint16_t llm_rd16(const uint8_t *p)
{
    return (uint16_t)(p[0] | (uint16_t)p[1] << 8);
}

uint32_t llm_rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

uint32_t llm_crc32(const uint8_t *p, size_t n)
{
    uint32_t c = 0xFFFFFFFFu;
    for (size_t i = 0; i < n; i++) {
        c ^= p[i];
        for (int b = 0; b < 8; b++) {
            c = (c >> 1) ^ (0xEDB88320u & (0u - (c & 1u)));
        }
    }
    return ~c;
}

uint32_t llm_fnv1a(const char *s)
{
    uint32_t h = 0x811C9DC5u;
    while (*s) {
        h = (h ^ (uint8_t)*s++) * 0x01000193u;
    }
    return h;
}

float llm_f16(uint16_t h)
{
    const uint32_t sign = (uint32_t)(h & 0x8000u) << 16;
    uint32_t exp = (h >> 10) & 0x1Fu;
    uint32_t man = h & 0x3FFu;
    uint32_t bits;
    if (exp == 0x1Fu) {                 /* inf / nan */
        bits = sign | 0x7F800000u | man << 13;
    } else if (exp != 0) {              /* normal */
        bits = sign | (exp + 112u) << 23 | man << 13;
    } else if (man == 0) {              /* zero */
        bits = sign;
    } else {                            /* subnormal: normalise */
        exp = 113u;
        while (!(man & 0x400u)) {
            man <<= 1;
            exp--;
        }
        bits = sign | exp << 23 | (man & 0x3FFu) << 13;
    }
    float f;
    memcpy(&f, &bits, sizeof f);
    return f;
}

/* In-place unnormalised Walsh-Hadamard transform, natural (Sylvester) order:
 * x <- H x with H the +-1 matrix. walsh(n) in the trainer is H / sqrt(n).
 *
 * Three butterfly stages at a time on eight values in registers (then two, then
 * one), so x is read and written once per three stages instead of every stage.
 * Each value is still the sum / difference of the same two operands as in the
 * stage-by-stage loop: the result is bit-identical. */
void llm_fwht(float *x, int n)
{
    int len = 1;
    for (; len * 8 <= n; len <<= 3) {
        for (int i = 0; i < n; i += len << 3) {
            for (int j = i; j < i + len; j++) {
                float *p = x + j;
                const float a0 = p[0], a1 = p[len], a2 = p[2 * len], a3 = p[3 * len];
                const float a4 = p[4 * len], a5 = p[5 * len], a6 = p[6 * len], a7 = p[7 * len];
                const float b0 = a0 + a1, b1 = a0 - a1, b2 = a2 + a3, b3 = a2 - a3;     /* stage len */
                const float b4 = a4 + a5, b5 = a4 - a5, b6 = a6 + a7, b7 = a6 - a7;
                const float c0 = b0 + b2, c2 = b0 - b2, c1 = b1 + b3, c3 = b1 - b3;     /* stage 2 len */
                const float c4 = b4 + b6, c6 = b4 - b6, c5 = b5 + b7, c7 = b5 - b7;
                p[0] = c0 + c4, p[4 * len] = c0 - c4;                                   /* stage 4 len */
                p[len] = c1 + c5, p[5 * len] = c1 - c5;
                p[2 * len] = c2 + c6, p[6 * len] = c2 - c6;
                p[3 * len] = c3 + c7, p[7 * len] = c3 - c7;
            }
        }
    }
    if (len * 4 <= n) {
        for (int i = 0; i < n; i += len << 2) {
            for (int j = i; j < i + len; j++) {
                float *p = x + j;
                const float a0 = p[0], a1 = p[len], a2 = p[2 * len], a3 = p[3 * len];
                const float b0 = a0 + a1, b1 = a0 - a1, b2 = a2 + a3, b3 = a2 - a3;
                p[0] = b0 + b2, p[2 * len] = b0 - b2;
                p[len] = b1 + b3, p[3 * len] = b1 - b3;
            }
        }
        len <<= 2;
    }
    for (; len < n; len <<= 1) {
        for (int i = 0; i < n; i += len << 1) {
            for (int j = i; j < i + len; j++) {
                const float a = x[j], b = x[j + len];
                x[j] = a + b;
                x[j + len] = a - b;
            }
        }
    }
}

/* ZC-RMSNorm: gain * x / rms(x) with gain = 1 + scale (folded at open);
 * gain == NULL gives the unit vector x / rms(x) the engram gate uses. out may
 * alias x. */
void llm_rmsnorm(float *out, const float *x, const float *gain, int n, float eps)
{
    float ss = 0.0f;
    for (int i = 0; i < n; i++) {
        ss += x[i] * x[i];
    }
    const float r = 1.0f / sqrtf(ss / (float)n + eps);
    if (!gain) {
        for (int i = 0; i < n; i++) {
            out[i] = 1.0f * x[i] * r;
        }
        return;
    }
    for (int i = 0; i < n; i++) {
        out[i] = gain[i] * x[i] * r;
    }
}

/* ------------------------------------------------- int8-kernel tables */

#if defined(__riscv_xxldsp)
#define LUT_RAM __attribute__((section(".ocram_bss")))
#else
#define LUT_RAM
#endif
LUT_RAM float llm_sig_lut[LLM_SIG_N + 1][2];
LUT_RAM float llm_exp_lut[LLM_EXP_N + 1][2];

void llm_lut_init(void)
{
    for (int i = 0; i < LLM_SIG_N; i++) {
        llm_sig_lut[i][0] = llm_sigmoid((float)i / LLM_SIG_STEPS - LLM_SIG_RANGE);
    }
    for (int i = 0; i < LLM_EXP_N; i++) {
        llm_exp_lut[i][0] = llm_expf((float)i / LLM_EXP_STEPS - LLM_EXP_RANGE);
    }
    /* the knot past the end (t at the range's top) has slope 0 */
    llm_sig_lut[LLM_SIG_N][0] = llm_sig_lut[LLM_SIG_N - 1][0];
    llm_exp_lut[LLM_EXP_N][0] = llm_exp_lut[LLM_EXP_N - 1][0];
    for (int i = 0; i < LLM_SIG_N; i++) {
        llm_sig_lut[i][1] = llm_sig_lut[i + 1][0] - llm_sig_lut[i][0];
    }
    for (int i = 0; i < LLM_EXP_N; i++) {
        llm_exp_lut[i][1] = llm_exp_lut[i + 1][0] - llm_exp_lut[i][0];
    }
    llm_sig_lut[LLM_SIG_N][1] = 0.0f;
    llm_exp_lut[LLM_EXP_N][1] = 0.0f;
}
