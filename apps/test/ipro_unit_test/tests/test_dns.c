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
 * @file test_dns.c
 * @brief Unit tests for utils_dns domain label decoding
 *
 * Note: utils_dns_domain_get outputs a trailing dot after the last label.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "utils_dns.h"

TEST_CASE(test_dns_decode_www_google_com)
{
    /*
     * DNS label encoding for "www.google.com":
     * 3 'w' 'w' 'w' 6 'g' 'o' 'o' 'g' 'l' 'e' 3 'c' 'o' 'm' 0
     */
    uint8_t records[] = {
        3, 'w', 'w', 'w',
        6, 'g', 'o', 'o', 'g', 'l', 'e',
        3, 'c', 'o', 'm',
        0
    };
    uint8_t buf[64];
    int len = sizeof(buf);

    memset(buf, 0, sizeof(buf));
    int ret = utils_dns_domain_get(records, buf, &len);
    TEST_ASSERT_EQUAL_INT(0, ret);
    /* SDK outputs trailing dot: "www.google.com." */
    TEST_ASSERT_EQUAL_INT(15, len);
    TEST_ASSERT_EQUAL_MEMORY("www.google.com.", buf, 15);
}

TEST_CASE(test_dns_decode_simple_label)
{
    uint8_t records[] = {
        4, 't', 'e', 's', 't',
        5, 'l', 'o', 'c', 'a', 'l',
        0
    };
    uint8_t buf[64];
    int len = sizeof(buf);

    memset(buf, 0, sizeof(buf));
    int ret = utils_dns_domain_get(records, buf, &len);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(11, len);
    TEST_ASSERT_EQUAL_MEMORY("test.local.", buf, 11);
}

TEST_CASE(test_dns_decode_single_label)
{
    uint8_t records[] = {
        9, 'l', 'o', 'c', 'a', 'l', 'h', 'o', 's', 't',
        0
    };
    uint8_t buf[64];
    int len = sizeof(buf);

    memset(buf, 0, sizeof(buf));
    int ret = utils_dns_domain_get(records, buf, &len);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(10, len);
    TEST_ASSERT_EQUAL_MEMORY("localhost.", buf, 10);
}

TEST_CASE(test_dns_output_length)
{
    /* Decode "a.b" */
    uint8_t records[] = {
        1, 'a',
        1, 'b',
        0
    };
    uint8_t buf[64];
    int len = sizeof(buf);

    memset(buf, 0, sizeof(buf));
    int ret = utils_dns_domain_get(records, buf, &len);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(4, len);
    TEST_ASSERT_EQUAL_MEMORY("a.b.", buf, 4);
}

void test_dns_run(void)
{
    printf("Running DNS utility tests\n");

    RUN_TEST(test_dns_decode_www_google_com);
    RUN_TEST(test_dns_decode_simple_label);
    RUN_TEST(test_dns_decode_single_label);
    RUN_TEST(test_dns_output_length);
}
