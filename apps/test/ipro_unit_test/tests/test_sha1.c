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
 * @file test_sha1.c
 * @brief Unit tests for utils_sha1 hashing
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "utils_sha1.h"

/* Known SHA-1 test vectors */

/* SHA1("") = da39a3ee5e6b4b0d3255bfef95601890afd80709 */
static const uint8_t sha1_empty[] = {
    0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d,
    0x32, 0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90,
    0xaf, 0xd8, 0x07, 0x09
};

/* SHA1("abc") = a9993e364706816aba3e25717850c26c9cd0d89d */
static const uint8_t sha1_abc[] = {
    0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
    0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c,
    0x9c, 0xd0, 0xd8, 0x9d
};

TEST_CASE(test_sha1_empty_string)
{
    uint8_t hash[20];
    utils_sha1((const unsigned char *)"", 0, hash);
    TEST_ASSERT_EQUAL_MEMORY(sha1_empty, hash, 20);
}

TEST_CASE(test_sha1_abc)
{
    uint8_t hash[20];
    utils_sha1((const unsigned char *)"abc", 3, hash);
    TEST_ASSERT_EQUAL_MEMORY(sha1_abc, hash, 20);
}

TEST_CASE(test_sha1_incremental)
{
    /* Hash "abc" using incremental API: "a" + "bc" */
    iot_sha1_context ctx;
    uint8_t hash_inc[20];
    uint8_t hash_one[20];

    utils_sha1_init(&ctx);
    utils_sha1_starts(&ctx);
    utils_sha1_update(&ctx, (const unsigned char *)"a", 1);
    utils_sha1_update(&ctx, (const unsigned char *)"bc", 2);
    utils_sha1_finish(&ctx, hash_inc);
    utils_sha1_free(&ctx);

    utils_sha1((const unsigned char *)"abc", 3, hash_one);

    TEST_ASSERT_EQUAL_MEMORY(hash_one, hash_inc, 20);
}

TEST_CASE(test_sha1_consistency)
{
    /* Same input should always produce same hash */
    const char *msg = "IPRO SDK unit test consistency check";
    uint8_t hash1[20], hash2[20];

    utils_sha1((const unsigned char *)msg, strlen(msg), hash1);
    utils_sha1((const unsigned char *)msg, strlen(msg), hash2);

    TEST_ASSERT_EQUAL_MEMORY(hash1, hash2, 20);
}

TEST_CASE(test_sha1_different_inputs)
{
    uint8_t hash1[20], hash2[20];

    utils_sha1((const unsigned char *)"hello", 5, hash1);
    utils_sha1((const unsigned char *)"world", 5, hash2);

    /* Different inputs must produce different hashes */
    TEST_ASSERT_FALSE(memcmp(hash1, hash2, 20) == 0);
}

void test_sha1_run(void)
{
    printf("Running SHA-1 hash tests\n");

    RUN_TEST(test_sha1_empty_string);
    RUN_TEST(test_sha1_abc);
    RUN_TEST(test_sha1_incremental);
    RUN_TEST(test_sha1_consistency);
    RUN_TEST(test_sha1_different_inputs);
}
