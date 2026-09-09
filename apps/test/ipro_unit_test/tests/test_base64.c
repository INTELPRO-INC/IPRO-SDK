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
 * @file test_base64.c
 * @brief Unit tests for utils_base64 encode/decode
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "utils_base64.h"

TEST_CASE(test_base64_encode_hello)
{
    const uint8_t input[] = "Hello";
    uint8_t output[32] = {0};
    uint32_t out_len = 0;

    iotx_err_t ret = utils_base64encode(input, 5, sizeof(output), output, &out_len);
    TEST_ASSERT_EQUAL_INT(SUCCESS_RETURN, ret);
    TEST_ASSERT_EQUAL_STRING("SGVsbG8=", (char *)output);
    TEST_ASSERT_EQUAL_UINT(8, out_len);
}

TEST_CASE(test_base64_encode_empty)
{
    uint8_t output[32] = {0};
    uint32_t out_len = 0;

    iotx_err_t ret = utils_base64encode((const uint8_t *)"", 0, sizeof(output), output, &out_len);
    TEST_ASSERT_EQUAL_INT(SUCCESS_RETURN, ret);
    TEST_ASSERT_EQUAL_UINT(0, out_len);
}

TEST_CASE(test_base64_decode_hello)
{
    const uint8_t encoded[] = "SGVsbG8=";
    uint8_t decoded[32] = {0};
    uint32_t out_len = 0;

    iotx_err_t ret = utils_base64decode(encoded, 8, sizeof(decoded), decoded, &out_len);
    TEST_ASSERT_EQUAL_INT(SUCCESS_RETURN, ret);
    TEST_ASSERT_EQUAL_UINT(5, out_len);
    TEST_ASSERT_EQUAL_MEMORY("Hello", decoded, 5);
}

TEST_CASE(test_base64_roundtrip)
{
    const uint8_t original[] = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0xFD, 0x80, 0x7F};
    uint8_t encoded[32] = {0};
    uint8_t decoded[32] = {0};
    uint32_t enc_len = 0, dec_len = 0;

    iotx_err_t ret = utils_base64encode(original, sizeof(original), sizeof(encoded), encoded, &enc_len);
    TEST_ASSERT_EQUAL_INT(SUCCESS_RETURN, ret);

    ret = utils_base64decode(encoded, enc_len, sizeof(decoded), decoded, &dec_len);
    TEST_ASSERT_EQUAL_INT(SUCCESS_RETURN, ret);
    TEST_ASSERT_EQUAL_UINT(sizeof(original), dec_len);
    TEST_ASSERT_EQUAL_MEMORY(original, decoded, sizeof(original));
}

TEST_CASE(test_base64_encode_padding)
{
    /* 1 byte -> 4 chars with == padding */
    uint8_t out1[16] = {0};
    uint32_t len1 = 0;
    utils_base64encode((const uint8_t *)"A", 1, sizeof(out1), out1, &len1);
    TEST_ASSERT_EQUAL_STRING("QQ==", (char *)out1);

    /* 2 bytes -> 4 chars with = padding */
    uint8_t out2[16] = {0};
    uint32_t len2 = 0;
    utils_base64encode((const uint8_t *)"AB", 2, sizeof(out2), out2, &len2);
    TEST_ASSERT_EQUAL_STRING("QUI=", (char *)out2);

    /* 3 bytes -> 4 chars, no padding */
    uint8_t out3[16] = {0};
    uint32_t len3 = 0;
    utils_base64encode((const uint8_t *)"ABC", 3, sizeof(out3), out3, &len3);
    TEST_ASSERT_EQUAL_STRING("QUJD", (char *)out3);
}

TEST_CASE(test_base64_known_vectors)
{
    /* RFC 4648 test vectors */
    struct {
        const char *plain;
        const char *encoded;
    } vectors[] = {
        {"f",      "Zg=="},
        {"fo",     "Zm8="},
        {"foo",    "Zm9v"},
        {"foob",   "Zm9vYg=="},
        {"fooba",  "Zm9vYmE="},
        {"foobar", "Zm9vYmFy"},
    };

    for (size_t i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++) {
        uint8_t output[32] = {0};
        uint32_t out_len = 0;
        iotx_err_t ret = utils_base64encode(
            (const uint8_t *)vectors[i].plain,
            strlen(vectors[i].plain),
            sizeof(output), output, &out_len);
        TEST_ASSERT_EQUAL_INT(SUCCESS_RETURN, ret);
        TEST_ASSERT_EQUAL_STRING(vectors[i].encoded, (char *)output);
    }
}

void test_base64_run(void)
{
    printf("Running base64 encode/decode tests\n");

    RUN_TEST(test_base64_encode_hello);
    RUN_TEST(test_base64_encode_empty);
    RUN_TEST(test_base64_decode_hello);
    RUN_TEST(test_base64_roundtrip);
    RUN_TEST(test_base64_encode_padding);
    RUN_TEST(test_base64_known_vectors);
}
