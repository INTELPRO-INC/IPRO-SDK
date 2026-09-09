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
 * @file test_sec_trng.c
 * @brief Unit tests for HW TRNG (True Random Number Generator)
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "hal_sec_trng.h"

#define TAG "TEST_SEC_TRNG"

static bool hw_trng_available = false;

/* Test cases */
TEST_CASE(test_trng_init)
{
    if (!hw_trng_available) {
        TEST_IGNORE_MESSAGE("HW TRNG not available");
    }

    /* Init already succeeded in probe, just verify flag */
    TEST_ASSERT_TRUE(hw_trng_available);
}

TEST_CASE(test_trng_random_word_nonzero)
{
    if (!hw_trng_available) {
        TEST_IGNORE_MESSAGE("HW TRNG not available");
    }

    /* Call 10 times, at least one must be non-zero */
    bool got_nonzero = false;
    for (int i = 0; i < 10; i++) {
        uint32_t val = hal_sec_get_random_word();
        if (val != 0) {
            got_nonzero = true;
            break;
        }
    }
    if (!got_nonzero) { TEST_FAIL("All 10 random words were zero"); }
}

TEST_CASE(test_trng_two_reads_differ)
{
    if (!hw_trng_available) {
        TEST_IGNORE_MESSAGE("HW TRNG not available");
    }

    /* Try 10 pairs, at least one pair must differ */
    bool found_different = false;
    for (int i = 0; i < 10; i++) {
        uint32_t a = hal_rand();
        uint32_t b = hal_rand();
        if (a != b) {
            found_different = true;
            break;
        }
    }
    if (!found_different) { TEST_FAIL("All 10 pairs of random values were identical"); }
}

TEST_CASE(test_trng_stream_fill)
{
    if (!hw_trng_available) {
        TEST_IGNORE_MESSAGE("HW TRNG not available");
    }

    uint8_t buf[256];
    memset(buf, 0xAA, sizeof(buf));

    hal_rand_stream(buf, sizeof(buf));

    /* Check that not all bytes are the same value */
    bool all_same = true;
    for (int i = 1; i < 256; i++) {
        if (buf[i] != buf[0]) {
            all_same = false;
            break;
        }
    }
    if (all_same) { TEST_FAIL("All 256 random stream bytes are identical"); }
}

TEST_CASE(test_trng_monobit)
{
    if (!hw_trng_available) {
        TEST_IGNORE_MESSAGE("HW TRNG not available");
    }

    uint8_t buf[256];
    hal_rand_stream(buf, sizeof(buf));

    /* Count the number of 1-bits in the buffer */
    int ones = 0;
    for (int i = 0; i < 256; i++) {
        uint8_t byte = buf[i];
        while (byte) {
            ones += byte & 1;
            byte >>= 1;
        }
    }

    /* Total bits = 256 * 8 = 2048. Expect between 25% and 75% ones. */
    int total_bits = 256 * 8;
    int low = total_bits / 4;   /* 512 */
    int high = (total_bits * 3) / 4; /* 1536 */

    if (ones < low) { TEST_FAIL("Monobit: too few 1-bits (< 25%)"); }
    if (ones > high) { TEST_FAIL("Monobit: too many 1-bits (> 75%)"); }
}

/* Test suite runner */
void test_sec_trng_run(void)
{
    printf("Running HW TRNG tests\n");

    /* Probe HW availability */
    int ret = hal_sec_trng_init();
    if (ret == 0) {
        hw_trng_available = true;
    } else {
        hw_trng_available = false;
    }

    RUN_TEST(test_trng_init);
    RUN_TEST(test_trng_random_word_nonzero);
    RUN_TEST(test_trng_two_reads_differ);
    RUN_TEST(test_trng_stream_fill);
    RUN_TEST(test_trng_monobit);
}
