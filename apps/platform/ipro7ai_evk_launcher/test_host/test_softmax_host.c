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

/*
 * Host test for the integer softmax against double-precision softmax.
 *
 *   sh apps/platform/ipro7ai_evk_launcher/test_host/run.sh
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "softmax_q.h"

static int g_fail, g_run;

#define CHECK(cond, ...) do {                                      \
    g_run++;                                                       \
    if (!(cond)) { g_fail++;                                       \
        printf("FAIL %s:%d  ", __func__, __LINE__);                \
        printf(__VA_ARGS__); printf("\n"); }                       \
} while (0)

static int ref_softmax(const uint8_t *q, int n, double scale, double *p)
{
    double m = -1e30, sum = 0;
    int best = 0;
    for (int i = 0; i < n; i++) {
        double v = q[i] * scale;
        if (v > m) { m = v; best = i; }
    }
    for (int i = 0; i < n; i++) { p[i] = exp(q[i] * scale - m); sum += p[i]; }
    for (int i = 0; i < n; i++) p[i] = 100.0 * p[i] / sum;
    return best;
}

static void test_cases(void)
{
    uint8_t pct[4];
    double  ref[4];

    /* All equal: 25% each. */
    uint8_t eq[4] = { 128, 128, 128, 128 };
    CHECK(softmax_q_pct(eq, 4, softmax_q_scale(0.1f), pct) == 0, "argmax of equal = first");
    CHECK(pct[0] == 25 && pct[1] == 25 && pct[2] == 25 && pct[3] == 25,
          "equal -> 25/25/25/25 (%u %u %u %u)", pct[0], pct[1], pct[2], pct[3]);

    /* One clear winner. */
    uint8_t win[4] = { 30, 200, 40, 10 };
    int b = softmax_q_pct(win, 4, softmax_q_scale(0.1f), pct);
    CHECK(b == 1 && pct[1] == 100, "clear winner: idx %d %u%%", b, pct[1]);

    /* Realistic RPS output: zp 128, scale ~0.12 (from the v3 model header). */
    uint8_t rps[4] = { 150, 128, 120, 135 };
    b = softmax_q_pct(rps, 4, softmax_q_scale(0.12f), pct);
    ref_softmax(rps, 4, 0.12, ref);
    CHECK(b == 0, "rps argmax %d", b);
    for (int i = 0; i < 4; i++) {
        CHECK(fabs(pct[i] - ref[i]) <= 2.0, "rps p[%d] = %u exp %.1f", i, pct[i], ref[i]);
    }
}

static void test_random_against_reference(void)
{
    uint8_t q[4], pct[4];
    double  ref[4];
    const float scales[] = { 0.03f, 0.08f, 0.12f, 0.2f, 0.35f };

    srand(12345);
    for (int s = 0; s < 5; s++) {
        uint32_t sq = softmax_q_scale(scales[s]);
        for (int t = 0; t < 4000; t++) {
            for (int i = 0; i < 4; i++) q[i] = (uint8_t)(rand() & 0xFF);
            int b  = softmax_q_pct(q, 4, sq, pct);
            int rb = ref_softmax(q, 4, scales[s], ref);
            int sum = pct[0] + pct[1] + pct[2] + pct[3];
            /* The argmax may legitimately differ only on an exact tie. */
            CHECK(b == rb || q[b] == q[rb], "argmax %d vs %d", b, rb);
            CHECK(sum >= 97 && sum <= 103, "sum %d", sum);
            for (int i = 0; i < 4; i++) {
                CHECK(fabs(pct[i] - ref[i]) <= 2.0, "s=%.2f q=%u,%u,%u,%u p[%d]=%u exp %.1f",
                      (double)scales[s], q[0], q[1], q[2], q[3], i, pct[i], ref[i]);
            }
        }
    }
}

int main(void)
{
    test_cases();
    test_random_against_reference();
    printf("softmax_q: %d checks, %d failed\n", g_run, g_fail);
    return g_fail ? 1 : 0;
}
