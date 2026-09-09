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
 * @file test_sec_aes.c
 * @brief Unit tests for HW AES engine
 *
 * Follows the same pattern as ipro7_demo/sec_test.c:
 * - Handle and I/O buffers are static, aligned, in PSRAM section
 * - Input data is memcpy'd into the work buffer before each operation
 * - Full init_ex/setkey/crypt/deinit cycle for each operation
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "hal_sec_aes.h"

/* Static aligned handle and I/O buffers (32-byte aligned for HW engine) */
static sec_aes_handle_t aes_handle __attribute__((aligned(32)));
static uint8_t aes_in_buf[64] __attribute__((aligned(32)));
static uint8_t aes_out_buf[64] __attribute__((aligned(32)));

static bool hw_aes_available = false;

/* NIST FIPS 197 Appendix B - AES-128-ECB test vector */
static const uint8_t nist_key_128[16] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static const uint8_t nist_plaintext[16] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

static const uint8_t nist_ciphertext[16] = {
    0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
    0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
};

TEST_CASE(test_aes_ecb128_encrypt)
{
    if (!hw_aes_available) { TEST_IGNORE_MESSAGE("HW AES not available"); }

    int ret;

    /* Copy plaintext into aligned work buffer */
    memcpy(aes_in_buf, nist_plaintext, 16);
    memset(aes_out_buf, 0, 16);

    sec_aes_init_ex(&aes_handle, SEC_MODE_DIRECT, SEC_AES_ECB, SEC_AES_KEY_128);
    sec_aes_setkey(&aes_handle, nist_key_128, sizeof(nist_key_128), NULL, SEC_AES_DIR_ENCRYPT);
    ret = sec_aes_encrypt(&aes_handle, aes_in_buf, 16, 0, aes_out_buf);
    sec_aes_deinit(&aes_handle);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_MEMORY(nist_ciphertext, aes_out_buf, 16);
}

TEST_CASE(test_aes_ecb128_decrypt)
{
    if (!hw_aes_available) { TEST_IGNORE_MESSAGE("HW AES not available"); }

    int ret;

    /* Copy ciphertext into aligned work buffer */
    memcpy(aes_in_buf, nist_ciphertext, 16);
    memset(aes_out_buf, 0, 16);

    sec_aes_init_ex(&aes_handle, SEC_MODE_DIRECT, SEC_AES_ECB, SEC_AES_KEY_128);
    sec_aes_setkey(&aes_handle, nist_key_128, sizeof(nist_key_128), NULL, SEC_AES_DIR_DECRYPT);
    ret = sec_aes_decrypt(&aes_handle, aes_in_buf, 16, 0, aes_out_buf);
    sec_aes_deinit(&aes_handle);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_MEMORY(nist_plaintext, aes_out_buf, 16);
}

TEST_CASE(test_aes_ecb128_roundtrip)
{
    if (!hw_aes_available) { TEST_IGNORE_MESSAGE("HW AES not available"); }

    uint8_t original[16] = "Hello AES Test!";
    int ret;

    /* Encrypt */
    memcpy(aes_in_buf, original, 16);
    memset(aes_out_buf, 0, 16);

    sec_aes_init_ex(&aes_handle, SEC_MODE_DIRECT, SEC_AES_ECB, SEC_AES_KEY_128);
    sec_aes_setkey(&aes_handle, nist_key_128, sizeof(nist_key_128), NULL, SEC_AES_DIR_ENCRYPT);
    ret = sec_aes_encrypt(&aes_handle, aes_in_buf, 16, 0, aes_out_buf);
    sec_aes_deinit(&aes_handle);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Encrypted must differ from original */
    TEST_ASSERT_FALSE(memcmp(original, aes_out_buf, 16) == 0);

    /* Decrypt: copy encrypted output into input buffer */
    memcpy(aes_in_buf, aes_out_buf, 16);
    memset(aes_out_buf, 0, 16);

    sec_aes_init_ex(&aes_handle, SEC_MODE_DIRECT, SEC_AES_ECB, SEC_AES_KEY_128);
    sec_aes_setkey(&aes_handle, nist_key_128, sizeof(nist_key_128), NULL, SEC_AES_DIR_DECRYPT);
    ret = sec_aes_decrypt(&aes_handle, aes_in_buf, 16, 0, aes_out_buf);
    sec_aes_deinit(&aes_handle);
    TEST_ASSERT_EQUAL_INT(0, ret);

    TEST_ASSERT_EQUAL_MEMORY(original, aes_out_buf, 16);
}

/* Test suite runner */
void test_sec_aes_run(void)
{
    printf("Running HW AES tests (DIRECT mode)\n");

    /* Probe HW availability */
    sec_aes_init_ex(&aes_handle, SEC_MODE_DIRECT, SEC_AES_ECB, SEC_AES_KEY_128);
    int ret = sec_aes_setkey(&aes_handle, nist_key_128, sizeof(nist_key_128), NULL, SEC_AES_DIR_ENCRYPT);
    if (ret == 0) {
        hw_aes_available = true;
    }
    sec_aes_deinit(&aes_handle);

    RUN_TEST(test_aes_ecb128_encrypt);
    RUN_TEST(test_aes_ecb128_decrypt);
    RUN_TEST(test_aes_ecb128_roundtrip);
}
