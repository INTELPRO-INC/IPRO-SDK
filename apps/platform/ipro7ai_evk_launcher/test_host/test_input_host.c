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
 * Host tests for the gesture -> event debounce - runs on the dev machine.
 *
 *   sh apps/platform/ipro7ai_evk_launcher/test_host/run.sh
 *
 * The classifier is simulated at 15 results/s (67 ms apart), which is what
 * the NPU delivers on the board for the 128x128 model.
 */
#include <stdio.h>
#include <string.h>

#include "input.h"

static int g_fail, g_run;

#define CHECK(cond, ...) do {                                      \
    g_run++;                                                       \
    if (!(cond)) { g_fail++;                                       \
        printf("FAIL %s:%d  ", __func__, __LINE__);                \
        printf(__VA_ARGS__); printf("\n"); }                       \
} while (0)

#define HOLD_MS     500
#define RELEASE_MS  300
#define MIN_CONF    60
#define PERIOD_MS   67

static uint32_t g_now, g_seq;

static input_event_t feed(input_t *in, int cls, int conf)
{
    g_now += PERIOD_MS;
    return input_update(in, g_now, cls, conf, ++g_seq);
}

/* Feed the same class for `ms`, return the first non-NONE event seen and how
 * many events fired in total. */
static input_event_t feed_for(input_t *in, int cls, int conf, uint32_t ms, int *count)
{
    input_event_t first = INPUT_EV_NONE;
    int n = 0;
    for (uint32_t t = 0; t < ms; t += PERIOD_MS) {
        input_event_t ev = feed(in, cls, conf);
        if (ev != INPUT_EV_NONE) {
            if (first == INPUT_EV_NONE) first = ev;
            n++;
        }
    }
    if (count) *count = n;
    return first;
}

static void reset(input_t *in)
{
    g_now = 1000;
    g_seq = 0;
    input_init(in, HOLD_MS, RELEASE_MS, MIN_CONF);
}

static void test_mapping(void)
{
    CHECK(input_event_for_class(INPUT_CLS_PAPER) == INPUT_EV_NEXT, "paper -> next");
    CHECK(input_event_for_class(INPUT_CLS_ROCK) == INPUT_EV_SELECT, "rock -> select");
    CHECK(input_event_for_class(INPUT_CLS_SCISSORS) == INPUT_EV_ALT, "scissors -> alt");
    CHECK(input_event_for_class(INPUT_CLS_NONE) == INPUT_EV_NONE, "none -> none");
    CHECK(input_event_for_class(42) == INPUT_EV_NONE, "garbage -> none");
}

static void test_hold_fires_once(void)
{
    input_t in;
    int n;

    reset(&in);
    /* Empty box first, so the input is armed. */
    CHECK(feed_for(&in, INPUT_CLS_NONE, 90, 400, &n) == INPUT_EV_NONE && n == 0, "none fires nothing");

    /* Under the hold time: nothing. */
    CHECK(feed_for(&in, INPUT_CLS_PAPER, 90, 400, &n) == INPUT_EV_NONE && n == 0,
          "400 ms of paper does not fire (%d)", n);
    int pct;
    CHECK(input_holding(&in, &pct) == INPUT_CLS_PAPER && pct >= 60 && pct < 100,
          "holding paper at %d%%", pct);

    /* Crossing the hold time: exactly one event, then silence while held. */
    CHECK(feed_for(&in, INPUT_CLS_PAPER, 90, 2000, &n) == INPUT_EV_NEXT && n == 1,
          "paper held: NEXT once (%d)", n);
    CHECK(in.fired == 1, "fired count %u", (unsigned)in.fired);
}

static void test_release_rearms(void)
{
    input_t in;
    int n;

    reset(&in);
    feed_for(&in, INPUT_CLS_NONE, 90, 400, NULL);
    CHECK(feed_for(&in, INPUT_CLS_ROCK, 90, 800, &n) == INPUT_EV_SELECT && n == 1, "rock fires");

    /* Straight to another gesture without emptying the box: nothing. */
    CHECK(feed_for(&in, INPUT_CLS_PAPER, 90, 1500, &n) == INPUT_EV_NONE && n == 0,
          "rock -> paper without release does not fire (%d)", n);

    /* A short gap (under release_ms) does not re-arm. */
    feed_for(&in, INPUT_CLS_NONE, 90, 150, NULL);
    CHECK(feed_for(&in, INPUT_CLS_PAPER, 90, 1500, &n) == INPUT_EV_NONE && n == 0,
          "150 ms gap does not re-arm (%d)", n);

    /* A real gap does. */
    feed_for(&in, INPUT_CLS_NONE, 90, 400, NULL);
    CHECK(feed_for(&in, INPUT_CLS_PAPER, 90, 800, &n) == INPUT_EV_NEXT && n == 1,
          "400 ms gap re-arms (%d)", n);
}

static void test_low_confidence_counts_as_none(void)
{
    input_t in;
    int n;

    reset(&in);
    feed_for(&in, INPUT_CLS_NONE, 90, 400, NULL);
    CHECK(feed_for(&in, INPUT_CLS_ROCK, 40, 2000, &n) == INPUT_EV_NONE && n == 0,
          "40%% rock never fires (%d)", n);
    CHECK(input_holding(&in, NULL) == INPUT_CLS_NONE, "low confidence is not a hold");

    /* Confidence flickering across the floor restarts the hold. */
    feed_for(&in, INPUT_CLS_ROCK, 90, 300, NULL);
    feed(&in, INPUT_CLS_ROCK, 30);
    CHECK(feed_for(&in, INPUT_CLS_ROCK, 90, 300, &n) == INPUT_EV_NONE && n == 0,
          "dip below the floor restarts the hold (%d)", n);
    CHECK(feed_for(&in, INPUT_CLS_ROCK, 90, 300, &n) == INPUT_EV_SELECT && n == 1,
          "and it fires once the hold completes (%d)", n);
}

static void test_class_change_restarts_hold(void)
{
    input_t in;
    int n;

    reset(&in);
    feed_for(&in, INPUT_CLS_NONE, 90, 400, NULL);
    feed_for(&in, INPUT_CLS_PAPER, 90, 400, NULL);     /* almost there */
    feed_for(&in, INPUT_CLS_ROCK, 90, 400, NULL);      /* switched: restart */
    CHECK(in.fired == 0, "no event after 400+400 ms of two gestures");
    CHECK(feed_for(&in, INPUT_CLS_ROCK, 90, 200, &n) == INPUT_EV_SELECT && n == 1,
          "rock completes its own hold (%d)", n);
}

static void test_duplicate_seq_ignored(void)
{
    input_t in;

    reset(&in);
    feed_for(&in, INPUT_CLS_NONE, 90, 400, NULL);
    /* Same seq fed many times with the clock advancing: must not count as
     * held time (the classifier produced nothing new). */
    g_now += PERIOD_MS;
    input_update(&in, g_now, INPUT_CLS_PAPER, 90, ++g_seq);
    for (int i = 0; i < 30; i++) {
        g_now += PERIOD_MS;
        CHECK(input_update(&in, g_now, INPUT_CLS_PAPER, 90, g_seq) == INPUT_EV_NONE,
              "stale seq fires nothing");
    }
    CHECK(in.fired == 0, "stale results never fire");
}

static void test_starts_armed_after_none(void)
{
    input_t in;
    int n;

    reset(&in);
    /* Cold start with a hand already in the box: must not fire until the
     * box has been empty once (a hand left over from the previous page). */
    CHECK(feed_for(&in, INPUT_CLS_PAPER, 90, 1500, &n) == INPUT_EV_NONE && n == 0,
          "hand present at boot does not fire (%d)", n);
    feed_for(&in, INPUT_CLS_NONE, 90, 400, NULL);
    CHECK(feed_for(&in, INPUT_CLS_PAPER, 90, 800, &n) == INPUT_EV_NEXT && n == 1,
          "fires after the first empty box (%d)", n);
}

int main(void)
{
    test_mapping();
    test_hold_fires_once();
    test_release_rearms();
    test_low_confidence_counts_as_none();
    test_class_change_restarts_hold();
    test_duplicate_seq_ignored();
    test_starts_armed_after_none();
    printf("input: %d checks, %d failed\n", g_run, g_fail);
    return g_fail ? 1 : 0;
}
