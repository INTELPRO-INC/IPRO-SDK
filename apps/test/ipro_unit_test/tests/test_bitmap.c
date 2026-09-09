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
 * @file test_bitmap.c
 * @brief Unit tests for utils_bitmap_window (sliding window bitmap)
 *
 * The bitmap is a sliding window for tracking received sequence numbers.
 * When contiguous bits from the bottom are set, the window auto-advances
 * (shifts right and increments SSN).
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "utils_bitmap_window.h"

TEST_CASE(test_bitmap_init)
{
    struct utils_bitmap_ctx ctx;
    int ret = utils_bitmap_window_init(&ctx, 0, 64);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* After init, map should be zero */
    uint64_t map = utils_bitmap_window_map_get(&ctx);
    TEST_ASSERT(map == 0);

    /* SSN should be 0 */
    int ssn = utils_bitmap_window_ssn_get(&ctx);
    TEST_ASSERT_EQUAL_INT(0, ssn);
}

TEST_CASE(test_bitmap_set_advances_ssn)
{
    struct utils_bitmap_ctx ctx;
    utils_bitmap_window_init(&ctx, 0, 64);

    /* Setting bit 0 should advance SSN from 0 to 1 (window slides) */
    utils_bitmap_window_bit_set(&ctx, 0);

    int ssn = utils_bitmap_window_ssn_get(&ctx);
    TEST_ASSERT_EQUAL_INT(1, ssn);

    /* Map should be 0 after sliding past the single set bit */
    uint64_t map = utils_bitmap_window_map_get(&ctx);
    TEST_ASSERT(map == 0);
}

TEST_CASE(test_bitmap_gap_holds_window)
{
    struct utils_bitmap_ctx ctx;
    utils_bitmap_window_init(&ctx, 0, 64);

    /* Set bit 1 (skip bit 0) — window can't advance past the gap */
    utils_bitmap_window_bit_set(&ctx, 1);

    int ssn = utils_bitmap_window_ssn_get(&ctx);
    TEST_ASSERT_EQUAL_INT(0, ssn);

    /* Map should have bit 1 set (waiting for bit 0 to arrive) */
    uint64_t map = utils_bitmap_window_map_get(&ctx);
    TEST_ASSERT_TRUE(map != 0);
}

TEST_CASE(test_bitmap_ssn_get)
{
    struct utils_bitmap_ctx ctx;
    int ssn_start = 10;
    utils_bitmap_window_init(&ctx, ssn_start, 64);

    int ssn = utils_bitmap_window_ssn_get(&ctx);
    TEST_ASSERT_EQUAL_INT(ssn_start, ssn);
}

TEST_CASE(test_bitmap_init_different_ssn)
{
    struct utils_bitmap_ctx ctx;

    /* Init with non-zero SSN */
    utils_bitmap_window_init(&ctx, 32, 64);

    int ssn = utils_bitmap_window_ssn_get(&ctx);
    TEST_ASSERT_EQUAL_INT(32, ssn);

    uint64_t map = utils_bitmap_window_map_get(&ctx);
    TEST_ASSERT(map == 0);
}

void test_bitmap_run(void)
{
    printf("Running bitmap window tests\n");

    RUN_TEST(test_bitmap_init);
    RUN_TEST(test_bitmap_set_advances_ssn);
    RUN_TEST(test_bitmap_gap_holds_window);
    RUN_TEST(test_bitmap_ssn_get);
    RUN_TEST(test_bitmap_init_different_ssn);
}
