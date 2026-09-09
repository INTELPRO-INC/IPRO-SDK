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
 * @file test_md5.c
 * @brief Unit tests for utils_md5 hashing
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "utils_md5.h"

/* Known MD5 test vectors */

/* MD5("") = d41d8cd98f00b204e9800998ecf8427e */
static const uint8_t md5_empty[] = {
    0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
    0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e
};

/* MD5("abc") = 900150983cd24fb0d6963f7d28e17f72 */
static const uint8_t md5_abc[] = {
    0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
    0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72
};

TEST_CASE(test_md5_empty_string)
{
    uint8_t hash[16];
    utils_md5((const unsigned char *)"", 0, hash);
    TEST_ASSERT_EQUAL_MEMORY(md5_empty, hash, 16);
}

TEST_CASE(test_md5_abc)
{
    uint8_t hash[16];
    utils_md5((const unsigned char *)"abc", 3, hash);
    TEST_ASSERT_EQUAL_MEMORY(md5_abc, hash, 16);
}

TEST_CASE(test_md5_incremental)
{
    /* Hash "abc" using incremental API: "a" + "bc" */
    iot_md5_context ctx;
    uint8_t hash_inc[16];
    uint8_t hash_one[16];

    utils_md5_init(&ctx);
    utils_md5_starts(&ctx);
    utils_md5_update(&ctx, (const unsigned char *)"a", 1);
    utils_md5_update(&ctx, (const unsigned char *)"bc", 2);
    utils_md5_finish(&ctx, hash_inc);
    utils_md5_free(&ctx);

    utils_md5((const unsigned char *)"abc", 3, hash_one);

    TEST_ASSERT_EQUAL_MEMORY(hash_one, hash_inc, 16);
}

TEST_CASE(test_md5_consistency)
{
    /* Same input should always produce same hash */
    const char *msg = "IPRO SDK unit test consistency check";
    uint8_t hash1[16], hash2[16];

    utils_md5((const unsigned char *)msg, strlen(msg), hash1);
    utils_md5((const unsigned char *)msg, strlen(msg), hash2);

    TEST_ASSERT_EQUAL_MEMORY(hash1, hash2, 16);
}

TEST_CASE(test_md5_different_inputs)
{
    uint8_t hash1[16], hash2[16];

    utils_md5((const unsigned char *)"hello", 5, hash1);
    utils_md5((const unsigned char *)"world", 5, hash2);

    /* Different inputs must produce different hashes */
    TEST_ASSERT_FALSE(memcmp(hash1, hash2, 16) == 0);
}

void test_md5_run(void)
{
    printf("Running MD5 hash tests\n");

    RUN_TEST(test_md5_empty_string);
    RUN_TEST(test_md5_abc);
    RUN_TEST(test_md5_incremental);
    RUN_TEST(test_md5_consistency);
    RUN_TEST(test_md5_different_inputs);
}
