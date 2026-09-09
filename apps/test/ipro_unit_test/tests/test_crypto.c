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
 * @file test_crypto.c
 * @brief Unit tests for cryptography components
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"

#ifdef CONFIG_MBEDTLS
#include "mbedtls/md.h"
#include "mbedtls/sha256.h"
#include "mbedtls/aes.h"
#endif

#define TAG "TEST_CRYPTO"

#ifdef CONFIG_MBEDTLS

/* Test cases */
TEST_CASE(test_sha256)
{
    unsigned char output[32];
    const char *input = "Hello, World!";
    
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    
    int ret = mbedtls_sha256_starts(&ctx, 0); /* 0 = SHA-256 */
    TEST_ASSERT_EQUAL_INT(0, ret);
    
    ret = mbedtls_sha256_update(&ctx, (unsigned char *)input, strlen(input));
    TEST_ASSERT_EQUAL_INT(0, ret);
    
    ret = mbedtls_sha256_finish(&ctx, output);
    TEST_ASSERT_EQUAL_INT(0, ret);
    
    mbedtls_sha256_free(&ctx);
    
    /* Verify non-zero output */
    bool non_zero = false;
    for (int i = 0; i < 32; i++) {
        if (output[i] != 0) {
            non_zero = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(non_zero);
}

TEST_CASE(test_aes_encrypt_decrypt)
{
    mbedtls_aes_context aes;
    unsigned char key[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                             0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    unsigned char input[16] = "Test plaintext.";
    unsigned char encrypted[16];
    unsigned char decrypted[16];
    
    mbedtls_aes_init(&aes);
    
    /* Set encryption key */
    int ret = mbedtls_aes_setkey_enc(&aes, key, 128);
    TEST_ASSERT_EQUAL_INT(0, ret);
    
    /* Encrypt */
    ret = mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, input, encrypted);
    TEST_ASSERT_EQUAL_INT(0, ret);
    
    /* Set decryption key */
    ret = mbedtls_aes_setkey_dec(&aes, key, 128);
    TEST_ASSERT_EQUAL_INT(0, ret);
    
    /* Decrypt */
    ret = mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, encrypted, decrypted);
    TEST_ASSERT_EQUAL_INT(0, ret);
    
    /* Verify */
    TEST_ASSERT_EQUAL_MEMORY(input, decrypted, 16);
    
    mbedtls_aes_free(&aes);
}

#endif /* CONFIG_MBEDTLS */

/* Test suite runner */
void test_crypto_run(void)
{
    printf("Running cryptography tests\n");
    
#ifdef CONFIG_MBEDTLS
    RUN_TEST(test_sha256);
    RUN_TEST(test_aes_encrypt_decrypt);
#else
    printf("WARNING: Crypto tests skipped (CONFIG_MBEDTLS not enabled)\n");
    TEST_IGNORE_MESSAGE("CONFIG_MBEDTLS not enabled");
#endif
}
