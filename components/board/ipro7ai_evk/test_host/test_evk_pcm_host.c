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
 * Host tests for the integer PCM helpers - runs on the dev machine, no board.
 *
 *   sh components/board/ipro7ai_evk/test_host/run.sh
 *
 * Expected values come from double-precision math on the host, so a pass
 * means the integer approximations are within the tolerance stated at each
 * check, not merely self-consistent.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "evk_pcm.h"

static int g_fail;
static int g_run;

#define CHECK(cond, ...) do {                                      \
    g_run++;                                                       \
    if (!(cond)) { g_fail++;                                       \
        printf("FAIL %s:%d  ", __func__, __LINE__);                \
        printf(__VA_ARGS__); printf("\n"); }                       \
} while (0)

static void test_isqrt(void)
{
    CHECK(evk_pcm_isqrt(0) == 0, "isqrt(0)");
    CHECK(evk_pcm_isqrt(1) == 1, "isqrt(1)");
    CHECK(evk_pcm_isqrt(15) == 3, "isqrt(15) = %u", evk_pcm_isqrt(15));
    CHECK(evk_pcm_isqrt(16) == 4, "isqrt(16)");
    CHECK(evk_pcm_isqrt(0xFFFFFFFFu) == 65535, "isqrt(max) = %u", evk_pcm_isqrt(0xFFFFFFFFu));
    for (uint32_t v = 1; v < 100000; v += 7) {
        uint32_t r = evk_pcm_isqrt(v);
        CHECK(r * r <= v && (r + 1) * (r + 1) > v, "isqrt(%u) = %u", v, r);
    }
}

static void test_ilog2(void)
{
    CHECK(evk_pcm_ilog2_q8(1) == 0, "log2(1)");
    CHECK(evk_pcm_ilog2_q8(2) == 256, "log2(2) = %u", evk_pcm_ilog2_q8(2));
    CHECK(evk_pcm_ilog2_q8(4) == 512, "log2(4)");
    CHECK(abs((int)evk_pcm_ilog2_q8(3) - 406) <= 1, "log2(3) = %u", evk_pcm_ilog2_q8(3));
    CHECK(abs((int)evk_pcm_ilog2_q8(1000) - 2551) <= 1, "log2(1000) = %u", evk_pcm_ilog2_q8(1000));
    CHECK(evk_pcm_ilog2_q8(32768) == 3840, "log2(32768) = %u", evk_pcm_ilog2_q8(32768));
    for (uint32_t v = 1; v < 70000; v += 13) {
        double exp = log2((double)v) * 256.0;
        CHECK(fabs((double)evk_pcm_ilog2_q8(v) - exp) <= 1.5, "log2(%u) = %u exp %.1f",
              v, evk_pcm_ilog2_q8(v), exp);
    }
}

static void test_dbfs(void)
{
    CHECK(evk_pcm_dbfs_x10(0) == -1200, "dbfs(0) = %d", evk_pcm_dbfs_x10(0));
    CHECK(evk_pcm_dbfs_x10(32768) == 0, "dbfs(32768) = %d", evk_pcm_dbfs_x10(32768));
    CHECK(abs(evk_pcm_dbfs_x10(16384) + 60) <= 1, "dbfs(16384) = %d", evk_pcm_dbfs_x10(16384));
    CHECK(abs(evk_pcm_dbfs_x10(3277) + 200) <= 2, "dbfs(3277) = %d", evk_pcm_dbfs_x10(3277));
    CHECK(abs(evk_pcm_dbfs_x10(1) + 903) <= 2, "dbfs(1) = %d", evk_pcm_dbfs_x10(1));
    for (uint32_t v = 1; v <= 32768; v += 97) {
        int exp = (int)lround(20.0 * log10((double)v / 32768.0) * 10.0);
        CHECK(abs(evk_pcm_dbfs_x10(v) - exp) <= 2, "dbfs(%u) = %d exp %d",
              v, evk_pcm_dbfs_x10(v), exp);
    }
}

static void test_level(void)
{
    static int16_t buf[640];
    uint16_t peak, rms;

    memset(buf, 0, sizeof(buf));
    evk_pcm_level(buf, 640, &peak, &rms);
    CHECK(peak == 0 && rms == 0, "silence: peak %u rms %u", peak, rms);

    for (int i = 0; i < 640; i++) buf[i] = (i & 1) ? 32767 : -32767;
    evk_pcm_level(buf, 640, &peak, &rms);
    CHECK(peak == 32767 && rms == 32767, "square: peak %u rms %u", peak, rms);

    buf[3] = -32768;
    evk_pcm_level(buf, 640, &peak, &rms);
    CHECK(peak == 32768, "negative rail: peak %u", peak);

    uint32_t ph = 0, step = evk_pcm_sine_step(1000, 32000);
    evk_pcm_sine_fill(buf, 640, &ph, step, 16384);
    evk_pcm_level(buf, 640, &peak, &rms);
    CHECK(peak >= 16300 && peak <= 16384, "sine: peak %u", peak);
    CHECK(abs((int)rms - 11585) <= 120, "sine: rms %u (exp 11585)", rms);

    evk_pcm_level(buf, 0, &peak, &rms);
    CHECK(peak == 0 && rms == 0, "n = 0");
}

static void test_gain(void)
{
    CHECK(evk_pcm_gain_q12(0) == 4096, "0 dB = %d", evk_pcm_gain_q12(0));
    CHECK(evk_pcm_gain_q12(6) == 8173, "+6 dB = %d", evk_pcm_gain_q12(6));
    CHECK(evk_pcm_gain_q12(-6) == 2053, "-6 dB = %d", evk_pcm_gain_q12(-6));
    CHECK(evk_pcm_gain_q12(-60) == 4, "-60 dB = %d", evk_pcm_gain_q12(-60));
    CHECK(evk_pcm_gain_q12(48) == 1028869, "+48 dB = %d", evk_pcm_gain_q12(48));
    CHECK(evk_pcm_gain_q12(-99) == evk_pcm_gain_q12(-60), "clamps low");
    CHECK(evk_pcm_gain_q12(99) == evk_pcm_gain_q12(48), "clamps high");
    for (int db = -60; db <= 48; db++) {
        int32_t exp = (int32_t)lround(4096.0 * pow(10.0, db / 20.0));
        CHECK(evk_pcm_gain_q12(db) == exp, "%d dB = %d exp %d", db, evk_pcm_gain_q12(db), exp);
    }

    int16_t in[4] = { 1000, -1000, 30000, -30000 }, out[4];
    uint32_t clipped = evk_pcm_apply_gain(in, out, 4, evk_pcm_gain_q12(6));
    /* >> 12 floors, so the negative side lands one code lower. */
    CHECK(out[0] == 1995 && out[1] == -1996, "+6 dB: %d %d", out[0], out[1]);
    CHECK(out[2] == 32767 && out[3] == -32768 && clipped == 2, "clip: %d %d n=%u",
          out[2], out[3], clipped);
    clipped = evk_pcm_apply_gain(in, in, 4, 4096);      /* in place, unity */
    CHECK(in[0] == 1000 && in[3] == -30000 && clipped == 0, "unity in place");
}

static void test_sine(void)
{
    static int16_t buf[320];
    uint32_t ph = 0, step = evk_pcm_sine_step(1000, 32000);
    long sum = 0;

    CHECK(step == 134217728u, "step = %u", step);
    evk_pcm_sine_fill(buf, 320, &ph, step, 32767);     /* exactly 10 periods */
    for (int i = 0; i < 320; i++) sum += buf[i];
    CHECK(labs(sum) < 320 * 4, "one period sums to ~0: %ld", sum);
    CHECK(buf[0] == 0, "starts at 0: %d", buf[0]);
    CHECK(buf[8] > 32000, "quarter period near +full: %d", buf[8]);
    CHECK(buf[24] < -32000, "three-quarter near -full: %d", buf[24]);
    CHECK(ph == 0, "phase wraps back to 0: %u", ph);

    evk_pcm_sine_fill(buf, 320, &ph, step, 0);
    int nonzero = 0;
    for (int i = 0; i < 320; i++) nonzero += buf[i] != 0;
    CHECK(nonzero == 0, "zero amplitude is silence: %d nonzero", nonzero);
}

int main(void)
{
    test_isqrt();
    test_ilog2();
    test_dbfs();
    test_level();
    test_gain();
    test_sine();
    printf("evk_pcm: %d checks, %d failed\n", g_run, g_fail);
    return g_fail ? 1 : 0;
}
