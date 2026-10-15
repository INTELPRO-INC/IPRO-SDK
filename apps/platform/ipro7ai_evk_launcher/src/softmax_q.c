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
 * @file softmax_q.c
 * @brief Integer softmax. See softmax_q.h.
 */

#include "softmax_q.h"

#define EXP_STEP_SHIFT   4                  /* table step = 1/16 */
#define EXP_TABLE_LEN    129                /* x = 0 .. 8 inclusive */
#define SOFTMAX_MAX_N    16

/* round(32768 * exp(-k/16)), k = 0..128. Generated on the host. */
static const uint16_t s_exp_q15[EXP_TABLE_LEN] = {
    32768, 30783, 28918, 27166, 25520, 23974, 22521, 21157,
    19875, 18671, 17539, 16477, 15479, 14541, 13660, 12832,
    12055, 11324, 10638,  9994,  9388,  8819,  8285,  7783,
     7312,  6869,  6452,  6061,  5694,  5349,  5025,  4721,
     4435,  4166,  3914,  3676,  3454,  3244,  3048,  2863,
     2690,  2527,  2374,  2230,  2095,  1968,  1849,  1737,
     1631,  1533,  1440,  1352,  1271,  1194,  1121,  1053,
      990,   930,   873,   820,   771,   724,   680,   639,
      600,   564,   530,   498,   467,   439,   412,   387,
      364,   342,   321,   302,   283,   266,   250,   235,
      221,   207,   195,   183,   172,   162,   152,   143,
      134,   126,   118,   111,   104,    98,    92,    86,
       81,    76,    72,    67,    63,    59,    56,    52,
       49,    46,    43,    41,    38,    36,    34,    32,
       30,    28,    26,    25,    23,    22,    21,    19,
       18,    17,    16,    15,    14,    13,    12,    12,
       11,
};

uint32_t softmax_q_scale(float out_scale)
{
    if (out_scale <= 0.0f) {
        return 0;
    }
    return (uint32_t)(out_scale * 65536.0f + 0.5f);
}

/* exp(-x) in Q15 for x in Q16, linearly interpolated; 0 beyond the table. */
static uint32_t exp_neg_q15(uint32_t x_q16)
{
    uint32_t idx  = x_q16 >> (16 - EXP_STEP_SHIFT);          /* x * 16 */
    uint32_t frac = x_q16 & ((1u << (16 - EXP_STEP_SHIFT)) - 1u); /* Q12 */

    if (idx >= EXP_TABLE_LEN - 1) {
        return 0;
    }
    uint32_t a = s_exp_q15[idx], b = s_exp_q15[idx + 1];
    return a - (((a - b) * frac) >> (16 - EXP_STEP_SHIFT));
}

int softmax_q_pct(const uint8_t *q, int n, uint32_t scale_q16, uint8_t *pct_out)
{
    uint32_t e[SOFTMAX_MAX_N];
    uint32_t sum = 0;
    int best = 0;

    if (n <= 0 || n > SOFTMAX_MAX_N) {
        return 0;
    }
    for (int i = 1; i < n; i++) {
        if (q[i] > q[best]) {
            best = i;
        }
    }
    for (int i = 0; i < n; i++) {
        uint32_t d = (uint32_t)(q[best] - q[i]);            /* 0..255 */
        uint64_t x = (uint64_t)d * scale_q16;               /* Q16 */
        e[i] = x > 0xFFFFFFFFull ? 0u : exp_neg_q15((uint32_t)x);
        sum += e[i];
    }
    if (sum == 0) {
        sum = 1;
    }
    for (int i = 0; i < n; i++) {
        pct_out[i] = (uint8_t)((e[i] * 100u + sum / 2u) / sum);
    }
    return best;
}
