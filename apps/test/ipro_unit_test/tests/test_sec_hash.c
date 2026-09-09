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
 * @file test_sec_hash.c
 * @brief Unit tests for HW SHA hash engine using DIRECT mode
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "hal_sec_hash.h"

/* Static aligned handle and buffers (32-byte aligned for HW engine) */
static sec_hash_handle_t hash_handle __attribute__((aligned(32)));
static uint8_t hash_out_buf[64] __attribute__((aligned(32)));
static uint8_t hash_in_buf[64] __attribute__((aligned(32)));

static bool hw_hash_available = false;

/* Known SHA-256 digests */
static const uint8_t sha256_abc[32] = {
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
    0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
    0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};

static const uint8_t sha1_abc[20] = {
    0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
    0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c,
    0x9c, 0xd0, 0xd8, 0x9d
};

TEST_CASE(test_sha256_known_vector_hw)
{
    if (!hw_hash_available) { TEST_IGNORE_MESSAGE("HW hash not available"); }

    int ret;

    memcpy(hash_in_buf, "abc", 3);

    ret = sec_hash_init_ex(&hash_handle, SEC_HASH_MODE_DIRECT, SEC_HASH_SHA256);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_hash_update(&hash_handle, hash_in_buf, 3);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_hash_finish(&hash_handle, hash_out_buf);
    TEST_ASSERT_EQUAL_INT(32, ret);

    sec_hash_deinit(&hash_handle);

    TEST_ASSERT_EQUAL_MEMORY(sha256_abc, hash_out_buf, 32);
}

TEST_CASE(test_hw_sha256_abc)
{
    if (!hw_hash_available) { TEST_IGNORE_MESSAGE("HW hash not available"); }

    int ret;

    memcpy(hash_in_buf, "abc", 3);

    ret = sec_hash_init_ex(&hash_handle, SEC_HASH_MODE_DIRECT, SEC_HASH_SHA256);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_hash_update(&hash_handle, hash_in_buf, 3);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_hash_finish(&hash_handle, hash_out_buf);
    TEST_ASSERT_EQUAL_INT(32, ret);

    sec_hash_deinit(&hash_handle);

    TEST_ASSERT_EQUAL_MEMORY(sha256_abc, hash_out_buf, 32);
}

TEST_CASE(test_sha1_abc_hw)
{
    if (!hw_hash_available) { TEST_IGNORE_MESSAGE("HW hash not available"); }

    int ret;

    memcpy(hash_in_buf, "abc", 3);

    ret = sec_hash_init_ex(&hash_handle, SEC_HASH_MODE_DIRECT, SEC_HASH_SHA1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_hash_update(&hash_handle, hash_in_buf, 3);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = sec_hash_finish(&hash_handle, hash_out_buf);
    TEST_ASSERT_EQUAL_INT(20, ret);

    sec_hash_deinit(&hash_handle);

    TEST_ASSERT_EQUAL_MEMORY(sha1_abc, hash_out_buf, 20);
}

TEST_CASE(test_sha256_incremental_update_hw)
{
    if (!hw_hash_available) { TEST_IGNORE_MESSAGE("HW hash not available"); }

    uint8_t output_single[32] __attribute__((aligned(32)));
    int ret;

    /* Compute SHA-256 of "abc" in one shot */
    memcpy(hash_in_buf, "abc", 3);
    ret = sec_hash_init_ex(&hash_handle, SEC_HASH_MODE_DIRECT, SEC_HASH_SHA256);
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = sec_hash_update(&hash_handle, hash_in_buf, 3);
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = sec_hash_finish(&hash_handle, output_single);
    TEST_ASSERT_EQUAL_INT(32, ret);
    sec_hash_deinit(&hash_handle);

    /* Compute SHA-256 of "abc" incrementally: "a" + "bc" */
    memcpy(hash_in_buf, "a", 1);
    ret = sec_hash_init_ex(&hash_handle, SEC_HASH_MODE_DIRECT, SEC_HASH_SHA256);
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = sec_hash_update(&hash_handle, hash_in_buf, 1);
    TEST_ASSERT_EQUAL_INT(0, ret);
    memcpy(hash_in_buf, "bc", 2);
    ret = sec_hash_update(&hash_handle, hash_in_buf, 2);
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = sec_hash_finish(&hash_handle, hash_out_buf);
    TEST_ASSERT_EQUAL_INT(32, ret);
    sec_hash_deinit(&hash_handle);

    TEST_ASSERT_EQUAL_MEMORY(output_single, hash_out_buf, 32);
}

/* Test suite runner */
void test_sec_hash_run(void)
{
    printf("Running HW SHA hash tests (DIRECT mode)\n");

    /* Probe HW availability using DIRECT mode */
    int ret = sec_hash_init_ex(&hash_handle, SEC_HASH_MODE_DIRECT, SEC_HASH_SHA256);
    if (ret == 0) {
        hw_hash_available = true;
        sec_hash_deinit(&hash_handle);
    }

    RUN_TEST(test_sha256_known_vector_hw);
    RUN_TEST(test_hw_sha256_abc);
    RUN_TEST(test_sha1_abc_hw);
    RUN_TEST(test_sha256_incremental_update_hw);
}
