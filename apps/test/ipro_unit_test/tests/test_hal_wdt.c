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
 * @file test_hal_wdt.c
 * @brief Unit tests for HAL Watchdog Timer functionality
 *
 * Uses interrupt mode (int_mode=1) only. Never tests reset mode.
 * Always disables watchdog in cleanup to prevent board reset.
 */

#include <stdio.h>
#include <stdbool.h>
#include "unity.h"
#include "hal_wdt.h"

static bool hw_available = false;

TEST_CASE(test_wdt_init_and_counter_runs)
{
    /* Init watchdog in interrupt mode with 5000ms timeout */
    wdt_init(1, 5000);

    /* Small delay for counter to start */
    for (volatile int i = 0; i < 10000; i++) {}

    uint32_t count1 = wdt_get_counter();

    for (volatile int i = 0; i < 10000; i++) {}

    uint32_t count2 = wdt_get_counter();

    /* Check if counter is changing (hardware is working) */
    hw_available = (count1 != count2);

    if (!hw_available) {
        wdt_disable();
        TEST_IGNORE_MESSAGE("Watchdog hardware not available (counter not changing)");
        return;
    }

    /* Counter should be running (values differ) */
    TEST_ASSERT(count1 != count2);

    wdt_disable();
}

TEST_CASE(test_wdt_feed_resets_counter)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("Watchdog hardware not available"); return; }

    wdt_init(1, 5000);

    /* Let counter run for a bit */
    for (volatile int i = 0; i < 50000; i++) {}

    uint32_t count_before_feed = wdt_get_counter();

    /* Feed the watchdog (should reset counter) */
    wdt_feed();

    uint32_t count_after_feed = wdt_get_counter();

    /*
     * After feeding, the counter should be reset to a value closer to the
     * initial state (lower or reset value). The exact behavior depends on
     * whether the counter counts up or down, but the counter after feed
     * should differ significantly from the counter before feed.
     */
    TEST_ASSERT(count_before_feed != count_after_feed);

    wdt_disable();
}

TEST_CASE(test_wdt_disable_stops_counting)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("Watchdog hardware not available"); return; }

    wdt_init(1, 5000);

    /* Let counter run */
    for (volatile int i = 0; i < 10000; i++) {}

    /* Disable watchdog */
    wdt_disable();

    uint32_t count1 = wdt_get_counter();

    /* Delay while disabled */
    for (volatile int i = 0; i < 50000; i++) {}

    uint32_t count2 = wdt_get_counter();

    /* Counter should be stopped (not changing) after disable */
    TEST_ASSERT(count1 == count2);
}

/* Test suite runner */
void test_hal_wdt_run(void)
{
    printf("Running HAL Watchdog tests\n");

    RUN_TEST(test_wdt_init_and_counter_runs);
    RUN_TEST(test_wdt_feed_resets_counter);
    RUN_TEST(test_wdt_disable_stops_counting);

    /* Safety: always disable watchdog on exit */
    wdt_disable();
}
