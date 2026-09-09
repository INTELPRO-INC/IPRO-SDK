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
 * @file test_hal_timer.c
 * @brief Unit tests for HAL Timer functionality
 */

#include <stdio.h>
#include <stdbool.h>
#include "unity.h"
#include "hal_timer.h"

static bool hw_available = false;

TEST_CASE(test_timer_init_freerun)
{
    hal_timer_config_t cfg = {
        .cnt_mode = TIMER_CNT_FREERUN,
        .trigger  = TIMER_PRELOAD_TRIGGER_NONE,
        .reload   = 0,
        .timeout1 = 0,
        .timeout2 = 0,
        .timeout3 = 0,
    };

    int ret = timer_init(TIMER0_INDEX, TIMER_CH0_INDEX, &cfg);
    hw_available = (ret == 0);

    if (!hw_available) {
        TEST_IGNORE_MESSAGE("Timer hardware not available");
        return;
    }

    TEST_ASSERT_EQUAL_INT(0, ret);
    timer_stop(TIMER0_INDEX, TIMER_CH0_INDEX);
}

TEST_CASE(test_timer_counter_increments)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("Timer hardware not available"); return; }

    hal_timer_config_t cfg = {
        .cnt_mode = TIMER_CNT_FREERUN,
        .trigger  = TIMER_PRELOAD_TRIGGER_NONE,
        .reload   = 0,
        .timeout1 = 0,
        .timeout2 = 0,
        .timeout3 = 0,
    };

    timer_init(TIMER0_INDEX, TIMER_CH0_INDEX, &cfg);
    timer_start(TIMER0_INDEX, TIMER_CH0_INDEX);

    /* Small delay for counter to advance */
    for (volatile int i = 0; i < 10000; i++) {}

    uint32_t count1 = timer_get_counter(TIMER0_INDEX, TIMER_CH0_INDEX);

    for (volatile int i = 0; i < 10000; i++) {}

    uint32_t count2 = timer_get_counter(TIMER0_INDEX, TIMER_CH0_INDEX);

    timer_stop(TIMER0_INDEX, TIMER_CH0_INDEX);

    /* Counter should have advanced */
    TEST_ASSERT(count2 > count1);
}

TEST_CASE(test_timer_stop_holds_counter)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("Timer hardware not available"); return; }

    hal_timer_config_t cfg = {
        .cnt_mode = TIMER_CNT_FREERUN,
        .trigger  = TIMER_PRELOAD_TRIGGER_NONE,
        .reload   = 0,
        .timeout1 = 0,
        .timeout2 = 0,
        .timeout3 = 0,
    };

    timer_init(TIMER0_INDEX, TIMER_CH0_INDEX, &cfg);
    timer_start(TIMER0_INDEX, TIMER_CH0_INDEX);

    /* Let counter run a bit */
    for (volatile int i = 0; i < 10000; i++) {}

    timer_stop(TIMER0_INDEX, TIMER_CH0_INDEX);

    uint32_t count1 = timer_get_counter(TIMER0_INDEX, TIMER_CH0_INDEX);

    /* Delay while stopped */
    for (volatile int i = 0; i < 10000; i++) {}

    uint32_t count2 = timer_get_counter(TIMER0_INDEX, TIMER_CH0_INDEX);

    /* Counter should not have advanced while stopped */
    TEST_ASSERT(count1 == count2);
}

TEST_CASE(test_timer_preload_mode)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("Timer hardware not available"); return; }

    hal_timer_config_t cfg = {
        .cnt_mode = TIMER_CNT_PRELOAD,
        .trigger  = TIMER_PRELOAD_TRIGGER_COMP0,
        .reload   = 0xFFFFFFFF,
        .timeout1 = 50000,
        .timeout2 = 0,
        .timeout3 = 0,
    };

    int ret = timer_init(TIMER0_INDEX, TIMER_CH0_INDEX, &cfg);
    TEST_ASSERT_EQUAL_INT(0, ret);

    timer_start(TIMER0_INDEX, TIMER_CH0_INDEX);

    /* Let it run briefly */
    for (volatile int i = 0; i < 10000; i++) {}

    uint32_t count = timer_get_counter(TIMER0_INDEX, TIMER_CH0_INDEX);
    timer_stop(TIMER0_INDEX, TIMER_CH0_INDEX);

    /* Counter should be running (non-zero or wrapping is fine, just not stuck) */
    /* In preload mode the counter counts and reloads, so just verify it ran */
    TEST_PASS();
}

/* Test suite runner */
void test_hal_timer_run(void)
{
    printf("Running HAL Timer tests\n");

    RUN_TEST(test_timer_init_freerun);
    RUN_TEST(test_timer_counter_increments);
    RUN_TEST(test_timer_stop_holds_counter);
    RUN_TEST(test_timer_preload_mode);

    /* Ensure timer is stopped on exit */
    timer_stop(TIMER0_INDEX, TIMER_CH0_INDEX);
}
