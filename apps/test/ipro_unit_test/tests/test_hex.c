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
 * @file test_hex.c
 * @brief Unit tests for utils_hex (bin2hex, hex2bin)
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "utils_hex.h"

TEST_CASE(test_bin2hex_basic)
{
    const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    char hex[16] = {0};

    char *result = utils_bin2hex(hex, data, 4);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("DEADBEEF", hex);
}

TEST_CASE(test_bin2hex_single_byte)
{
    const uint8_t data[] = {0x0A};
    char hex[8] = {0};

    utils_bin2hex(hex, data, 1);
    TEST_ASSERT_EQUAL_STRING("0A", hex);
}

TEST_CASE(test_bin2hex_zeros)
{
    const uint8_t data[] = {0x00, 0x00, 0x00};
    char hex[16] = {0};

    utils_bin2hex(hex, data, 3);
    TEST_ASSERT_EQUAL_STRING("000000", hex);  /* zeros are same in upper/lower */
}

TEST_CASE(test_hex2bin_basic)
{
    const char *hex = "deadbeef";
    uint8_t buf[8] = {0};

    size_t len = utils_hex2bin(hex, 8, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_UINT(4, len);
    TEST_ASSERT_EQUAL_INT(0xDE, buf[0]);
    TEST_ASSERT_EQUAL_INT(0xAD, buf[1]);
    TEST_ASSERT_EQUAL_INT(0xBE, buf[2]);
    TEST_ASSERT_EQUAL_INT(0xEF, buf[3]);
}

TEST_CASE(test_hex2bin_uppercase)
{
    const char *hex = "AABB";
    uint8_t buf[4] = {0};

    size_t len = utils_hex2bin(hex, 4, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_UINT(2, len);
    TEST_ASSERT_EQUAL_INT(0xAA, buf[0]);
    TEST_ASSERT_EQUAL_INT(0xBB, buf[1]);
}

TEST_CASE(test_hex_roundtrip)
{
    const uint8_t original[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    char hex[32] = {0};
    uint8_t recovered[8] = {0};

    utils_bin2hex(hex, original, sizeof(original));
    size_t len = utils_hex2bin(hex, strlen(hex), recovered, sizeof(recovered));

    TEST_ASSERT_EQUAL_UINT(sizeof(original), len);
    TEST_ASSERT_EQUAL_MEMORY(original, recovered, sizeof(original));
}

void test_hex_run(void)
{
    printf("Running hex encode/decode tests\n");

    RUN_TEST(test_bin2hex_basic);
    RUN_TEST(test_bin2hex_single_byte);
    RUN_TEST(test_bin2hex_zeros);
    RUN_TEST(test_hex2bin_basic);
    RUN_TEST(test_hex2bin_uppercase);
    RUN_TEST(test_hex_roundtrip);
}
