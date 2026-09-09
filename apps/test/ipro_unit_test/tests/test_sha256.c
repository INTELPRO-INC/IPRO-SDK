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
 * @file test_sha256.c
 * @brief Unit tests for utils_sha256 hashing
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "utils_sha256.h"

/* Known SHA-256 test vectors (NIST) */
static const uint8_t sha256_empty[] = {
    0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
    0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
    0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
    0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55
};

/* SHA-256("abc") */
static const uint8_t sha256_abc[] = {
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
    0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
    0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};

TEST_CASE(test_sha256_empty_string)
{
    uint8_t hash[32];
    utils_sha256((const uint8_t *)"", 0, hash);
    TEST_ASSERT_EQUAL_MEMORY(sha256_empty, hash, 32);
}

TEST_CASE(test_sha256_abc)
{
    uint8_t hash[32];
    utils_sha256((const uint8_t *)"abc", 3, hash);
    TEST_ASSERT_EQUAL_MEMORY(sha256_abc, hash, 32);
}

TEST_CASE(test_sha256_incremental)
{
    /* Hash "abc" using incremental API: "a" + "bc" */
    iot_sha256_context ctx;
    uint8_t hash_inc[32];
    uint8_t hash_one[32];

    utils_sha256_init(&ctx);
    utils_sha256_starts(&ctx);
    utils_sha256_update(&ctx, (const uint8_t *)"a", 1);
    utils_sha256_update(&ctx, (const uint8_t *)"bc", 2);
    utils_sha256_finish(&ctx, hash_inc);
    utils_sha256_free(&ctx);

    utils_sha256((const uint8_t *)"abc", 3, hash_one);

    TEST_ASSERT_EQUAL_MEMORY(hash_one, hash_inc, 32);
}

TEST_CASE(test_sha256_consistency)
{
    /* Same input should always produce same hash */
    const char *msg = "IPRO SDK unit test consistency check";
    uint8_t hash1[32], hash2[32];

    utils_sha256((const uint8_t *)msg, strlen(msg), hash1);
    utils_sha256((const uint8_t *)msg, strlen(msg), hash2);

    TEST_ASSERT_EQUAL_MEMORY(hash1, hash2, 32);
}

TEST_CASE(test_sha256_different_inputs)
{
    uint8_t hash1[32], hash2[32];

    utils_sha256((const uint8_t *)"hello", 5, hash1);
    utils_sha256((const uint8_t *)"world", 5, hash2);

    /* Different inputs must produce different hashes */
    TEST_ASSERT_FALSE(memcmp(hash1, hash2, 32) == 0);
}

void test_sha256_run(void)
{
    printf("Running SHA-256 hash tests\n");

    RUN_TEST(test_sha256_empty_string);
    RUN_TEST(test_sha256_abc);
    RUN_TEST(test_sha256_incremental);
    RUN_TEST(test_sha256_consistency);
    RUN_TEST(test_sha256_different_inputs);
}
