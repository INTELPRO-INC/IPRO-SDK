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
 * Host tests for the history ring and the range-time map.
 *
 * The map's age-to-column arithmetic is the kind of thing that is silently
 * wrong - a track drawn backwards in time still looks like a plausible
 * picture - so it is pinned here against a synthetic track whose position at
 * every instant is known.
 *
 *   cc -std=c11 -Wall -Wextra -Werror -I include -I test_host \
 *      -o /tmp/kw307_map_test test_host/test_map_host.c \
 *      src/kw307_history.c src/kw307_map.c src/kw307_proto.c && /tmp/kw307_map_test
 */
#include <stdio.h>
#include <string.h>

#include "kw307_history.h"
#include "kw307_map.h"

static int g_fail, g_run;

#define CHECK(cond, ...) do {                                   \
    g_run++;                                                    \
    if (!(cond)) { g_fail++;                                    \
        printf("FAIL %s:%d  ", __func__, __LINE__);             \
        printf(__VA_ARGS__); printf("\n"); }                    \
} while (0)

/* Push one frame whose stationary slot sits at `dist_mm`. */
static void push_still(uint16_t index, uint16_t dist_mm, uint16_t mag)
{
    kw307_frame_t f;
    memset(&f, 0, sizeof(f));
    f.index = index;
    f.human_flag = 1;
    /* distance_raw is the wire value; the ring stores the corrected range, so
     * feed twice what we expect to read back. */
    f.stationary[0].distance_raw = (uint16_t)(dist_mm * KW307_DIST_DIVISOR_DEFAULT);
    f.stationary[0].magnitude = mag;
    f.stationary[0].state = 1;          /* Valid in the simple form */
    f.stationary[0].counter = 20;
    kw307_history_push(&f, false);
}

static void push_empty(uint16_t index)
{
    kw307_frame_t f;
    memset(&f, 0, sizeof(f));
    f.index = index;
    kw307_history_push(&f, false);
}

static void test_history_basics(void)
{
    CHECK(kw307_history_init(10) == 0, "init 10 s");
    CHECK(kw307_history_capacity() == 10 * KW307_HISTORY_RATE_HZ, "capacity");
    CHECK(kw307_history_count() == 0, "starts empty");

    push_still(1, 2000, 500);
    CHECK(kw307_history_count() == 1, "one sample");

    kw307_sample_t s;
    CHECK(kw307_history_at(0, &s), "newest readable");
    CHECK(s.s_dist_mm == 2000, "stores the CORRECTED range, got %u", s.s_dist_mm);
    CHECK(s.valid_mask == KW307_VALID_STILL, "valid mask");
    CHECK(!kw307_history_at(1, &s), "nothing older yet");
}

static void test_history_wraps_and_counts_loss(void)
{
    CHECK(kw307_history_init(1) == 0, "init 1 s");
    const uint32_t cap = kw307_history_capacity();

    for (uint32_t i = 0; i < cap + 5; i++) {
        push_still((uint16_t)i, (uint16_t)(1000 + i), 500);
    }
    CHECK(kw307_history_count() == cap, "count saturates at capacity");
    CHECK(kw307_history_dropped() == 5, "counted 5 overwritten, got %u",
          kw307_history_dropped());

    kw307_sample_t s;
    CHECK(kw307_history_at(0, &s) && s.s_dist_mm == 1000 + cap + 4,
          "newest survived the wrap");
    CHECK(kw307_history_at(cap - 1, &s) && s.s_dist_mm == 1000 + 5,
          "oldest is the 6th pushed, got %u", s.s_dist_mm);
}

static void test_series_runs_forwards(void)
{
    CHECK(kw307_history_init(10) == 0, "init");
    for (uint16_t i = 0; i < 5; i++) {
        push_still(i, 2000, (uint16_t)(100 + i));   /* magnitude 100..104 */
    }

    int16_t buf[5];
    CHECK(kw307_history_series(KW307_SERIES_STILL_MAG, 5, buf) == 5, "got 5");
    /* Oldest first: an FFT and a difference filter both need time forwards. */
    CHECK(buf[0] == 100 && buf[4] == 104,
          "oldest first, got %d..%d", buf[0], buf[4]);
}

static void test_series_valid_count(void)
{
    CHECK(kw307_history_init(10) == 0, "init");
    for (int i = 0; i < 10; i++) {
        if (i % 2) push_still((uint16_t)i, 2000, 500);
        else       push_empty((uint16_t)i);
    }
    CHECK(kw307_history_series_valid(KW307_SERIES_STILL_MAG, 10) == 5,
          "half the window carried the series");
}

/* The map: a target walking outwards must appear as a track rising down the
 * rows from left to right, never mirrored. */
static void test_map_time_direction(void)
{
    const uint32_t window_s = KW307_MAP_TIME_STEPS / KW307_HISTORY_RATE_HZ * 2; /* 6 s */
    CHECK(kw307_history_init(60) == 0, "init");

    const uint32_t n = window_s * KW307_HISTORY_RATE_HZ;
    /* Walk from 0.5 m out to 0.5 m + n*10 mm, one sample at a time. */
    for (uint32_t i = 0; i < n; i++) {
        push_still((uint16_t)i, (uint16_t)(500 + i * 10), 2048);
    }

    static uint8_t map[KW307_MAP_CELLS];
    kw307_map_info_t info;
    CHECK(kw307_map_build(window_s, map, &info), "map builds");
    CHECK(info.samples_per_step == n / KW307_MAP_TIME_STEPS,
          "decimation %u", info.samples_per_step);

    /* Column 0 is the oldest: the target was near. The last column is newest:
     * it must be further out. */
    int first_row = -1, last_row = -1;
    for (uint32_t r = 0; r < KW307_MAP_RANGE_BINS; r++) {
        if (map[0 * KW307_MAP_RANGE_BINS + r]) { first_row = (int)r; break; }
    }
    for (uint32_t r = 0; r < KW307_MAP_RANGE_BINS; r++) {
        if (map[(KW307_MAP_TIME_STEPS - 1) * KW307_MAP_RANGE_BINS + r]) {
            last_row = (int)r;
            break;
        }
    }
    CHECK(first_row >= 0 && last_row >= 0, "track present in both edge columns");
    CHECK(last_row > first_row,
          "time runs oldest->newest: first col row %d, last col row %d",
          first_row, last_row);

    /* 500 mm lands in bin 2 with 250 mm bins. */
    CHECK(first_row == 500 / KW307_MAP_BIN_MM,
          "oldest column at the right range bin, got %d", first_row);
}

static void test_map_needs_enough_history(void)
{
    CHECK(kw307_history_init(60) == 0, "init");
    for (int i = 0; i < 10; i++) {
        push_still((uint16_t)i, 1000, 2048);
    }
    static uint8_t map[KW307_MAP_CELLS];
    kw307_map_info_t info;
    CHECK(!kw307_map_build(60, map, &info), "refuses a window it cannot fill");
}

static void test_map_saturates_and_zeroes(void)
{
    const uint32_t window_s = 6;
    CHECK(kw307_history_init(60) == 0, "init");
    const uint32_t n = window_s * KW307_HISTORY_RATE_HZ;
    for (uint32_t i = 0; i < n; i++) {
        push_still((uint16_t)i, 1000, 60000);      /* way over full scale */
    }
    static uint8_t map[KW307_MAP_CELLS];
    kw307_map_info_t info;
    CHECK(kw307_map_build(window_s, map, &info), "builds");
    CHECK(info.peak == 255, "saturates rather than wrapping, got %u", info.peak);

    /* Only one range bin is occupied, so occupancy is about 1/32 = 3%. */
    CHECK(info.occupancy_pct <= 5, "sparse map stays sparse, got %u%%",
          info.occupancy_pct);
    CHECK(map[0 * KW307_MAP_RANGE_BINS + 0] == 0, "bin 0 empty");
    CHECK(map[0 * KW307_MAP_RANGE_BINS + 4] == 255, "1000mm lands in bin 4");
}

int main(void)
{
    test_history_basics();
    test_history_wraps_and_counts_loss();
    test_series_runs_forwards();
    test_series_valid_count();
    test_map_time_direction();
    test_map_needs_enough_history();
    test_map_saturates_and_zeroes();
    kw307_history_free();

    printf("%s: %d checks, %d failed\n", g_fail ? "FAIL" : "PASS", g_run, g_fail);
    return g_fail ? 1 : 0;
}
