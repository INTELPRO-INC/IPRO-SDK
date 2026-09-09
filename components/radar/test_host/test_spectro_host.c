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
 * Host tests for the magnitude spectrogram.
 *
 * The risk here is arithmetic, not transforms: the decimation rate, the bin
 * mapping and the amount of history the image needs all have to agree with
 * each other, and a wrong one still produces a plausible-looking picture. So
 * a synthetic breath at a KNOWN rate is pushed through, and the peak has to
 * land where the header says it will.
 */
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "kw307_history.h"
#include "kw307_spectro.h"

static int g_fail, g_run;

#define CHECK(cond, ...) do {                                   \
    g_run++;                                                    \
    if (!(cond)) { g_fail++;                                    \
        printf("FAIL %s:%d  ", __func__, __LINE__);             \
        printf(__VA_ARGS__); printf("\n"); }                    \
} while (0)

static void push_still(uint16_t index, uint16_t dist_mm, uint16_t mag)
{
    kw307_frame_t f;
    memset(&f, 0, sizeof(f));
    f.index = index;
    f.human_flag = 1;
    f.stationary[0].distance_raw = (uint16_t)(dist_mm * KW307_DIST_DIVISOR_DEFAULT);
    f.stationary[0].magnitude = mag;
    f.stationary[0].state = 1;
    f.stationary[0].counter = 30;
    kw307_history_push(&f, false);
}

static void test_bin_arithmetic(void)
{
    /* fs after decimation is 5 Hz over a 128-point transform: 39.06 mHz a bin. */
    CHECK(KW307_SPECTRO_RATE_HZ == 5, "decimated rate is 5 Hz, got %d",
          KW307_SPECTRO_RATE_HZ);
    CHECK(kw307_spectro_bin_mhz(1) == 39, "bin 1 = 39 mHz, got %u",
          kw307_spectro_bin_mhz(1));

    /* The header claims breathing (0.2..0.5 Hz) lands in bins 5..13. */
    CHECK(kw307_spectro_bin_mhz(5) <= 200 && kw307_spectro_bin_mhz(6) > 200,
          "0.2 Hz sits at bin 5, not %u", kw307_spectro_bin_mhz(5));
    CHECK(kw307_spectro_bin_mhz(12) <= 500 && kw307_spectro_bin_mhz(13) > 500,
          "0.5 Hz sits at bin 12/13");

    /* The image keeps bins 1..32, i.e. 2.3..75 cycles per minute. */
    const uint32_t top_cpm = kw307_spectro_bin_mhz(KW307_SPECTRO_BINS) * 60 / 1000;
    CHECK(top_cpm == 75, "top row is 75/min, got %u", top_cpm);

    /* And the history it needs must match what the sliding window consumes. */
    const uint32_t expect =
        (KW307_SPECTRO_FFT_N + (KW307_SPECTRO_STEPS - 1) * KW307_SPECTRO_HOP)
        * KW307_SPECTRO_DECIMATE;
    CHECK(KW307_SPECTRO_NEED_SAMPLES == expect, "need-samples matches the window");
    CHECK(KW307_SPECTRO_NEED_SAMPLES / KW307_HISTORY_RATE_HZ == 126,
          "which is 126 s, got %u",
          KW307_SPECTRO_NEED_SAMPLES / KW307_HISTORY_RATE_HZ);
}

static void test_refuses_short_history(void)
{
    CHECK(kw307_history_init(300) == 0, "init");
    for (uint32_t i = 0; i < 100; i++) {
        push_still((uint16_t)i, 1000, 1500);
    }
    static uint8_t img[KW307_SPECTRO_CELLS];
    kw307_spectro_info_t info;
    CHECK(kw307_spectro_init() == 0, "spectro init");
    CHECK(!kw307_spectro_build(img, &info), "refuses a window it cannot fill");
}

static void test_refuses_a_mostly_empty_window(void)
{
    CHECK(kw307_history_init(300) == 0, "init");
    kw307_frame_t empty;
    memset(&empty, 0, sizeof(empty));

    for (uint32_t i = 0; i < KW307_SPECTRO_NEED_SAMPLES; i++) {
        if (i % 4) {
            empty.index = (uint16_t)i;
            kw307_history_push(&empty, false);   /* no Valid slot */
        } else {
            push_still((uint16_t)i, 1000, 1500);
        }
    }
    static uint8_t img[KW307_SPECTRO_CELLS];
    kw307_spectro_info_t info;
    /* Gaps read as zero and a run of zeros looks like a slow oscillation, so
     * a sparse window must be refused rather than turned into a breath rate. */
    CHECK(!kw307_spectro_build(img, &info), "refuses a mostly-empty window");
}

/* The real one: a synthetic breath at a known rate must peak on the right row. */
static void test_synthetic_breath_lands_on_the_right_row(void)
{
    const double breath_hz = 0.25;              /* 15 breaths per minute */

    CHECK(kw307_history_init(300) == 0, "init");
    CHECK(kw307_spectro_init() == 0, "spectro init");

    for (uint32_t i = 0; i < KW307_SPECTRO_NEED_SAMPLES; i++) {
        const double t = (double)i / KW307_HISTORY_RATE_HZ;
        /* A 1500-count carrier modulated +/-200 by breathing, on a slow drift
         * the detrender has to remove. */
        const double v = 1500.0 + 200.0 * sin(2.0 * M_PI * breath_hz * t)
                                + 0.4 * (double)i;
        push_still((uint16_t)i, 1000, (uint16_t)v);
    }

    static uint8_t img[KW307_SPECTRO_CELLS];
    kw307_spectro_info_t info;
    CHECK(kw307_spectro_build(img, &info), "builds");

    const uint32_t expect_bin = (uint32_t)(breath_hz * 1000.0
                                / (double)kw307_spectro_bin_mhz(1) + 0.5) - 1;

    /* Peak row of the newest column. */
    uint32_t best = 0, best_bin = 0;
    const uint8_t *col = &img[(KW307_SPECTRO_STEPS - 1) * KW307_SPECTRO_BINS];
    for (uint32_t b = 0; b < KW307_SPECTRO_BINS; b++) {
        if (col[b] > best) { best = col[b]; best_bin = b; }
    }

    CHECK(best_bin + 1 >= expect_bin && best_bin <= expect_bin + 1,
          "0.25 Hz breath peaks at row %u, expected %u", best_bin, expect_bin);
    CHECK(info.peak_mhz >= 220 && info.peak_mhz <= 280,
          "reported peak %u mHz should be near 250", info.peak_mhz);

    /* And the drift must not dominate: row 0 is the slowest surviving bin, and
     * a working detrender leaves it below the breathing row. */
    CHECK(col[0] < col[best_bin], "linear drift removed (row0 %u < peak %u)",
          col[0], col[best_bin]);
}

int main(void)
{
    test_bin_arithmetic();
    test_refuses_short_history();
    test_refuses_a_mostly_empty_window();
    test_synthetic_breath_lands_on_the_right_row();
    kw307_spectro_free();
    kw307_history_free();

    printf("%s: %d checks, %d failed\n", g_fail ? "FAIL" : "PASS", g_run, g_fail);
    return g_fail ? 1 : 0;
}
