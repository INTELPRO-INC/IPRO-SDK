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
 * @file test_hmac.c
 * @brief Unit tests for utils_hmac (HMAC-SHA1, HMAC-SHA256)
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "utils_hmac.h"

/*
 * HMAC-SHA1("key", "The quick brown fox jumps over the lazy dog")
 * = de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9
 */
static const uint8_t hmac_sha1_known[] = {
    0xde, 0x7c, 0x9b, 0x85, 0xb8, 0xb7, 0x8a, 0xa6,
    0xbc, 0x8a, 0x7a, 0x36, 0xf7, 0x0a, 0x90, 0x70,
    0x1c, 0x9d, 0xb4, 0xd9
};

/*
 * HMAC-SHA256("key", "The quick brown fox jumps over the lazy dog")
 * = f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8
 */
static const uint8_t hmac_sha256_known[] = {
    0xf7, 0xbc, 0x83, 0xf4, 0x30, 0x53, 0x84, 0x24,
    0xb1, 0x32, 0x98, 0xe6, 0xaa, 0x6f, 0xb1, 0x43,
    0xef, 0x4d, 0x59, 0xa1, 0x49, 0x46, 0x17, 0x59,
    0x97, 0x47, 0x9d, 0xbc, 0x2d, 0x1a, 0x3c, 0xd8
};

#ifdef UTILS_SHA1
TEST_CASE(test_hmac_sha1_known_vector)
{
    const char *key = "key";
    const char *msg = "The quick brown fox jumps over the lazy dog";
    char digest[SHA1_DIGEST_SIZE];

    utils_hmac_sha1(msg, (int)strlen(msg), digest, key, (int)strlen(key));
    TEST_ASSERT_EQUAL_MEMORY(hmac_sha1_known, digest, SHA1_DIGEST_SIZE);
}

TEST_CASE(test_hmac_sha1_empty_message)
{
    const char *key = "key";
    char digest1[SHA1_DIGEST_SIZE];
    char digest2[SHA1_DIGEST_SIZE];

    /* Empty message should still produce a valid HMAC */
    utils_hmac_sha1("", 0, digest1, key, (int)strlen(key));

    /* Running twice should give the same result */
    utils_hmac_sha1("", 0, digest2, key, (int)strlen(key));
    TEST_ASSERT_EQUAL_MEMORY(digest1, digest2, SHA1_DIGEST_SIZE);
}
#endif

#ifdef UTILS_SHA256
TEST_CASE(test_hmac_sha256_known_vector)
{
    const char *key = "key";
    const char *msg = "The quick brown fox jumps over the lazy dog";
    char digest[SHA256_DIGEST_SIZE];

    utils_hmac_sha256(msg, (int)strlen(msg), digest, key, (int)strlen(key));
    TEST_ASSERT_EQUAL_MEMORY(hmac_sha256_known, digest, SHA256_DIGEST_SIZE);
}

TEST_CASE(test_hmac_sha256_empty_message)
{
    const char *key = "key";
    char digest1[SHA256_DIGEST_SIZE];
    char digest2[SHA256_DIGEST_SIZE];

    /* Empty message should still produce a valid HMAC */
    utils_hmac_sha256("", 0, digest1, key, (int)strlen(key));

    /* Running twice should give the same result */
    utils_hmac_sha256("", 0, digest2, key, (int)strlen(key));
    TEST_ASSERT_EQUAL_MEMORY(digest1, digest2, SHA256_DIGEST_SIZE);
}

TEST_CASE(test_hmac_sha256_different_keys)
{
    const char *msg = "test message";
    char digest1[SHA256_DIGEST_SIZE];
    char digest2[SHA256_DIGEST_SIZE];

    utils_hmac_sha256(msg, (int)strlen(msg), digest1, "key1", 4);
    utils_hmac_sha256(msg, (int)strlen(msg), digest2, "key2", 4);

    /* Different keys must produce different HMACs */
    TEST_ASSERT_FALSE(memcmp(digest1, digest2, SHA256_DIGEST_SIZE) == 0);
}
#endif

void test_hmac_run(void)
{
    printf("Running HMAC tests\n");

#ifdef UTILS_SHA1
    RUN_TEST(test_hmac_sha1_known_vector);
    RUN_TEST(test_hmac_sha1_empty_message);
#endif
#ifdef UTILS_SHA256
    RUN_TEST(test_hmac_sha256_known_vector);
    RUN_TEST(test_hmac_sha256_empty_message);
    RUN_TEST(test_hmac_sha256_different_keys);
#endif
}
