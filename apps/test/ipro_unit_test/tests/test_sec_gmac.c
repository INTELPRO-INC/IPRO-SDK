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
 * @file test_sec_gmac.c
 * @brief Unit tests for HW GMAC engine
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "hal_sec_gmac.h"

#define TAG "TEST_SEC_GMAC"

static sec_gmac_handle_t gmac_handle;
static bool hw_gmac_available = false;

static const uint8_t test_key[16] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
};

static const uint8_t test_data[] = "Hello, GMAC test data for verification!";

/* Test cases */
TEST_CASE(test_gmac_init)
{
    if (!hw_gmac_available) {
        TEST_IGNORE_MESSAGE("HW GMAC not available");
    }

    TEST_ASSERT_TRUE(hw_gmac_available);
}

TEST_CASE(test_gmac_compute)
{
    if (!hw_gmac_available) {
        TEST_IGNORE_MESSAGE("HW GMAC not available");
    }

    uint8_t result[16];
    int ret;

    ret = sec_gmac_start(&gmac_handle, test_key);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_update(&gmac_handle, test_data, sizeof(test_data) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_finish(&gmac_handle, result);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Result should not be all zeros */
    bool non_zero = false;
    for (int i = 0; i < 16; i++) {
        if (result[i] != 0) {
            non_zero = true;
            break;
        }
    }
    if (!non_zero) { TEST_FAIL("GMAC result is all zeros"); }

    sec_gmac_clear(&gmac_handle);
}

TEST_CASE(test_gmac_deterministic)
{
    if (!hw_gmac_available) {
        TEST_IGNORE_MESSAGE("HW GMAC not available");
    }

    uint8_t result1[16];
    uint8_t result2[16];
    int ret;

    /* First computation */
    ret = sec_gmac_start(&gmac_handle, test_key);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_update(&gmac_handle, test_data, sizeof(test_data) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_finish(&gmac_handle, result1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    sec_gmac_clear(&gmac_handle);

    /* Second computation with same input */
    ret = sec_gmac_start(&gmac_handle, test_key);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_update(&gmac_handle, test_data, sizeof(test_data) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_finish(&gmac_handle, result2);
    TEST_ASSERT_EQUAL_INT(0, ret);

    sec_gmac_clear(&gmac_handle);

    /* Same input must produce same result */
    TEST_ASSERT_EQUAL_MEMORY(result1, result2, 16);
}

TEST_CASE(test_gmac_clear_and_recompute)
{
    if (!hw_gmac_available) {
        TEST_IGNORE_MESSAGE("HW GMAC not available");
    }

    uint8_t result1[16];
    uint8_t result2[16];
    int ret;

    /* First computation */
    ret = sec_gmac_start(&gmac_handle, test_key);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_update(&gmac_handle, test_data, sizeof(test_data) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_finish(&gmac_handle, result1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Clear and recompute */
    sec_gmac_clear(&gmac_handle);

    ret = sec_gmac_start(&gmac_handle, test_key);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_update(&gmac_handle, test_data, sizeof(test_data) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_finish(&gmac_handle, result2);
    TEST_ASSERT_EQUAL_INT(0, ret);

    sec_gmac_clear(&gmac_handle);

    /* Must produce same result after clear */
    TEST_ASSERT_EQUAL_MEMORY(result1, result2, 16);
}

TEST_CASE(test_gmac_different_inputs)
{
    if (!hw_gmac_available) {
        TEST_IGNORE_MESSAGE("HW GMAC not available");
    }

    static const uint8_t data_a[] = "Input data A";
    static const uint8_t data_b[] = "Input data B";
    uint8_t result_a[16];
    uint8_t result_b[16];
    int ret;

    /* Compute GMAC of data_a */
    ret = sec_gmac_start(&gmac_handle, test_key);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_update(&gmac_handle, data_a, sizeof(data_a) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_finish(&gmac_handle, result_a);
    TEST_ASSERT_EQUAL_INT(0, ret);

    sec_gmac_clear(&gmac_handle);

    /* Compute GMAC of data_b */
    ret = sec_gmac_start(&gmac_handle, test_key);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_update(&gmac_handle, data_b, sizeof(data_b) - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_gmac_finish(&gmac_handle, result_b);
    TEST_ASSERT_EQUAL_INT(0, ret);

    sec_gmac_clear(&gmac_handle);

    /* Different inputs should produce different results */
    if (memcmp(result_a, result_b, 16) == 0) { TEST_FAIL("Different inputs produced same GMAC"); }
}

/* Test suite runner */
void test_sec_gmac_run(void)
{
    printf("Running HW GMAC tests\n");

    /* Probe HW availability */
    int ret = sec_gmac_init();
    if (ret == 0) {
        hw_gmac_available = true;
    } else {
        hw_gmac_available = false;
    }

    RUN_TEST(test_gmac_init);
    RUN_TEST(test_gmac_compute);
    RUN_TEST(test_gmac_deterministic);
    RUN_TEST(test_gmac_clear_and_recompute);
    RUN_TEST(test_gmac_different_inputs);

    if (hw_gmac_available) {
        sec_gmac_deinit(&gmac_handle);
    }
}
