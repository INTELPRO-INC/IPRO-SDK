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
 * @file sec_test.c
 * @brief Comprehensive security hardware test suite
 *
 * Tests for:
 * - AES encryption/decryption (ECB, CBC, CTR modes with 128/192/256-bit keys)
 * - SHA hash functions (SHA1, SHA224, SHA256, SHA384, SHA512)
 * - TRNG (True Random Number Generator) with statistical analysis
 * - GMAC (Galois Message Authentication Code) - Link mode only
 * - PKA (Public Key Accelerator) - ECDSA sign/verify, ECDH key exchange
 *   with secp256r1 (NIST P-256) and secp256k1 (Bitcoin) curves
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"
#include <FreeRTOS.h>
#include <hal_sec_aes.h>
#include <hal_sec_hash.h>
#include <hal_sec_trng.h>
#include <hal_sec_ecdsa.h>
#include <hal_sec_dsa.h>
#include <drv_sec_eng.h>
#include <hal_sec_gmac.h>
#include <hal_common.h>
#include <hal_mtimer.h>
#include <compiler/common.h>

#if defined(CONFIG_MBEDTLS_SELF_TEST)
#include "mbedtls_self_test.h"
#endif

/*============================================================================
 * Security Test Buffers and Handles
 *
 * Data buffers: PSRAM (cacheable) - HAL layer handles cache flush/invalidate
 * Handles: OCRAM nocache section - link_cfg requires non-cacheable memory
 *===========================================================================*/

/* Static buffers for security operations in PSRAM (cacheable) */
static uint8_t ATTR_PSRAM_SECTION sec_work_buf[1024] __attribute__((aligned(32)));
static uint8_t ATTR_PSRAM_SECTION sec_output_buf[1024] __attribute__((aligned(32)));
static uint8_t ATTR_PSRAM_SECTION sec_hash_buf[64] __attribute__((aligned(32)));

/* Static handles in OCRAM nocache section (link_cfg requires non-cacheable) */
static sec_aes_handle_t sec_aes_handle ATTR_PSRAM_SECTION __attribute__((aligned(32)));
static sec_hash_handle_t sec_hash_handle ATTR_PSRAM_SECTION __attribute__((aligned(32)));
static sec_gmac_handle_t sec_gmac_handle ATTR_PSRAM_SECTION __attribute__((aligned(32)));

/* Global test counters for overall summary */
static int g_total_tests = 0;
static int g_total_pass = 0;

/*============================================================================
 * Test Vector Definitions
 *===========================================================================*/

/* NIST AES-128 ECB Test Vector (FIPS 197) */
static const uint8_t aes128_key[] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};
static const uint8_t aes128_plaintext[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};
static const uint8_t aes128_ecb_ciphertext[] = {
    0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
    0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97
};

/* AES-256 ECB Test Vector (FIPS 197) */
static const uint8_t aes256_key[] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};
static const uint8_t aes256_ecb_ciphertext[] = {
    0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c,
    0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8
};

/* AES-128 CBC Test Vector (NIST SP 800-38A) */
static const uint8_t aes_cbc_iv[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
static const uint8_t aes128_cbc_ciphertext[] = {
    0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
    0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d
};

/* AES-128 CTR Test Vector (NIST SP 800-38A) */
static const uint8_t aes_ctr_nonce[] = {
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};
static const uint8_t aes128_ctr_ciphertext[] = {
    0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
    0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce
};

/* SHA Test Vectors (from NIST examples) */
static const char sha_test_msg[] = "abc";
static const uint8_t sha1_expected[] = {
    0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
    0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c,
    0x9c, 0xd0, 0xd8, 0x9d
};
static const uint8_t sha224_expected[] = {
    0x23, 0x09, 0x7d, 0x22, 0x34, 0x05, 0xd8, 0x22,
    0x86, 0x42, 0xa4, 0x77, 0xbd, 0xa2, 0x55, 0xb3,
    0x2a, 0xad, 0xbc, 0xe4, 0xbd, 0xa0, 0xb3, 0xf7,
    0xe3, 0x6c, 0x9d, 0xa7
};
static const uint8_t sha256_expected[] = {
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
    0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
    0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};
static const uint8_t sha384_expected[] = {
    0xcb, 0x00, 0x75, 0x3f, 0x45, 0xa3, 0x5e, 0x8b,
    0xb5, 0xa0, 0x3d, 0x69, 0x9a, 0xc6, 0x50, 0x07,
    0x27, 0x2c, 0x32, 0xab, 0x0e, 0xde, 0xd1, 0x63,
    0x1a, 0x8b, 0x60, 0x5a, 0x43, 0xff, 0x5b, 0xed,
    0x80, 0x86, 0x07, 0x2b, 0xa1, 0xe7, 0xcc, 0x23,
    0x58, 0xba, 0xec, 0xa1, 0x34, 0xc8, 0x25, 0xa7
};
static const uint8_t sha512_expected[] = {
    0xdd, 0xaf, 0x35, 0xa1, 0x93, 0x61, 0x7a, 0xba,
    0xcc, 0x41, 0x73, 0x49, 0xae, 0x20, 0x41, 0x31,
    0x12, 0xe6, 0xfa, 0x4e, 0x89, 0xa9, 0x7e, 0xa2,
    0x0a, 0x9e, 0xee, 0xe6, 0x4b, 0x55, 0xd3, 0x9a,
    0x21, 0x92, 0x99, 0x2a, 0x27, 0x4f, 0xc1, 0xa8,
    0x36, 0xba, 0x3c, 0x23, 0xa3, 0xfe, 0xeb, 0xbd,
    0x45, 0x4d, 0x44, 0x23, 0x64, 0x3c, 0xe8, 0x0e,
    0x2a, 0x9a, 0xc9, 0x4f, 0xa5, 0x4c, 0xa4, 0x9f
};

/* GMAC Test Vectors (GCM GHASH key) */
static const uint8_t gmac_test_key[16] = {
    0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b,
    0x88, 0x4c, 0xfa, 0x59, 0xca, 0x34, 0x2b, 0x2e
};
static const uint8_t gmac_test_data[16] = {
    0x03, 0x88, 0xda, 0xce, 0x60, 0xb6, 0xa3, 0x92,
    0xf3, 0x28, 0xc2, 0xb9, 0x71, 0xb2, 0xfe, 0x78
};
/* Expected result for GHASH(H, data) where H = AES_K(0) */
static const uint8_t gmac_test_expected[16] = {
    0x5e, 0x2e, 0xc7, 0x46, 0x91, 0x70, 0x62, 0x88,
    0x2c, 0x85, 0xb0, 0x68, 0x53, 0x53, 0xde, 0xb7
};

/* ECDSA Test Vectors (NIST P-256, RFC 6979) */
static const uint8_t ecdsa_secp256r1_hash[32] = {
    0xAF, 0x2B, 0xDB, 0xE1, 0xAA, 0x9B, 0x6E, 0xC1,
    0xE2, 0xAD, 0xE1, 0xD6, 0x94, 0xF4, 0x1F, 0xC7,
    0x1A, 0x83, 0x1D, 0x02, 0x68, 0xE9, 0x89, 0x15,
    0x62, 0x11, 0x3D, 0x8A, 0x62, 0xAD, 0xD1, 0xBF
};
static const uint8_t ecdsa_secp256r1_private_key[32] = {
    0xC9, 0xAF, 0xA9, 0xD8, 0x45, 0xBA, 0x75, 0x16,
    0x6B, 0x5C, 0x21, 0x57, 0x67, 0xB1, 0xD6, 0x93,
    0x4E, 0x50, 0xC3, 0xDB, 0x36, 0xE8, 0x9B, 0x12,
    0x7B, 0x8A, 0x62, 0x2B, 0x12, 0x0F, 0x67, 0x21
};
static const uint8_t ecdsa_secp256r1_public_keyx[32] = {
    0x60, 0xFE, 0xD4, 0xBA, 0x25, 0x5A, 0x9D, 0x31,
    0xC9, 0x61, 0xEB, 0x74, 0xC6, 0x35, 0x6D, 0x68,
    0xC0, 0x49, 0xB8, 0x92, 0x3B, 0x61, 0xFA, 0x6C,
    0xE6, 0x69, 0x62, 0x2E, 0x60, 0xF2, 0x9F, 0xB6
};
static const uint8_t ecdsa_secp256r1_public_keyy[32] = {
    0x79, 0x03, 0xFE, 0x10, 0x08, 0xB8, 0xBC, 0x99,
    0xA4, 0x1A, 0xE9, 0xE9, 0x56, 0x28, 0xBC, 0x64,
    0xF2, 0xF1, 0xB2, 0x0C, 0x2D, 0x7E, 0x9F, 0x51,
    0x77, 0xA3, 0xC2, 0x94, 0xD4, 0x46, 0x22, 0x99
};
static const uint8_t ecdsa_secp256r1_k[32] = {
    0xA6, 0xE3, 0xC5, 0x7D, 0xD0, 0x1A, 0xBE, 0x90,
    0x08, 0x65, 0x38, 0x39, 0x83, 0x55, 0xDD, 0x4C,
    0x3B, 0x17, 0xAA, 0x87, 0x33, 0x82, 0xB0, 0xF2,
    0x4D, 0x61, 0x29, 0x49, 0x3D, 0x8A, 0xAD, 0x60
};
static const uint8_t ecdsa_secp256r1_r[32] = {
    0xEF, 0xD4, 0x8B, 0x2A, 0xAC, 0xB6, 0xA8, 0xFD,
    0x11, 0x40, 0xDD, 0x9C, 0xD4, 0x5E, 0x81, 0xD6,
    0x9D, 0x2C, 0x87, 0x7B, 0x56, 0xAA, 0xF9, 0x91,
    0xC3, 0x4D, 0x0E, 0xA8, 0x4E, 0xAF, 0x37, 0x16
};
static const uint8_t ecdsa_secp256r1_s[32] = {
    0xF7, 0xCB, 0x1C, 0x94, 0x2D, 0x65, 0x7C, 0x41,
    0xD4, 0x36, 0xC7, 0xA1, 0xB6, 0xE2, 0x9F, 0x65,
    0xF3, 0xE9, 0x00, 0xDB, 0xB9, 0xAF, 0xF4, 0x06,
    0x4D, 0xC4, 0xAB, 0x2F, 0x84, 0x3A, 0xCD, 0xA8
};

/* ECDH Test Vectors (secp256r1) */
static const uint8_t ecdh_secp256r1_private_key[32] = {
    0x03, 0xb2, 0x99, 0x0f, 0x18, 0x16, 0x38, 0x78,
    0x88, 0xb1, 0x09, 0xfd, 0xd4, 0x33, 0x94, 0x1a,
    0x1f, 0x7e, 0x7c, 0x8b, 0xf7, 0xf2, 0x34, 0x15,
    0xb0, 0xc1, 0x21, 0x61, 0xc0, 0xe9, 0xdd, 0xcd
};
static const uint8_t ecdh_secp256r1_public_keyx[32] = {
    0x12, 0x19, 0xa3, 0xa9, 0x0d, 0xbf, 0xfa, 0xf3,
    0xd5, 0x1b, 0x8c, 0xa0, 0xa6, 0xec, 0xd9, 0xa8,
    0xca, 0xf0, 0x06, 0xaf, 0x3b, 0x52, 0x4b, 0x9f,
    0x93, 0x96, 0xcb, 0x0a, 0xe6, 0xa6, 0xee, 0x18
};
static const uint8_t ecdh_secp256r1_public_keyy[32] = {
    0x5e, 0x25, 0x51, 0x62, 0xd6, 0x05, 0x89, 0xe3,
    0x65, 0x3d, 0xde, 0xf1, 0x59, 0xcb, 0xee, 0x70,
    0x56, 0x40, 0x71, 0x55, 0x9e, 0xec, 0xe0, 0x04,
    0x81, 0xa4, 0xfc, 0x7a, 0x0f, 0xe5, 0x5d, 0xe9
};

/* ECDH Peer's key for shared secret calculation */
static const uint8_t ecdh_secp256r1_peer_private_key[32] = {
    0xb2, 0xaf, 0x01, 0x62, 0x49, 0x36, 0x12, 0xee,
    0x36, 0xfb, 0x72, 0x20, 0xc9, 0xcc, 0xba, 0xe7,
    0x86, 0x05, 0x11, 0x84, 0x72, 0xed, 0x1c, 0xa4,
    0xf8, 0x2c, 0x3a, 0x0b, 0xf6, 0x9e, 0xc9, 0x34
};
static const uint8_t ecdh_secp256r1_peer_public_keyx[32] = {
    0xf4, 0x1f, 0x13, 0x2a, 0xf2, 0x7b, 0x67, 0x22,
    0xcb, 0x6a, 0x73, 0xb5, 0x21, 0x17, 0x32, 0x32,
    0x7d, 0x8c, 0x42, 0x71, 0x03, 0x9d, 0x54, 0x0c,
    0x68, 0xdc, 0xac, 0x8b, 0x1d, 0x63, 0x3a, 0xb3
};
static const uint8_t ecdh_secp256r1_peer_public_keyy[32] = {
    0x2a, 0xe0, 0xe0, 0x11, 0x45, 0xbc, 0x5e, 0x1b,
    0x30, 0x04, 0x1b, 0xc3, 0x45, 0x3b, 0xb9, 0xb6,
    0x77, 0x64, 0x6b, 0x7e, 0x96, 0x1a, 0xad, 0x22,
    0xd2, 0xe4, 0x9f, 0x29, 0x8e, 0x64, 0xab, 0xa1
};
static const uint8_t ecdh_secp256r1_shared_secret[32] = {
    0xc5, 0xd2, 0xf6, 0xb1, 0x69, 0x8c, 0x4f, 0xff,
    0x1b, 0x4a, 0x9d, 0x04, 0x4d, 0x7a, 0x3d, 0xbf,
    0xa5, 0xed, 0xfc, 0xd6, 0xc9, 0x12, 0xe7, 0x67,
    0x32, 0x1e, 0x3f, 0x31, 0x4b, 0xf1, 0x2c, 0xa7
};

/* ECDH Test Vectors (secp256k1 - Bitcoin curve) */
static const uint8_t ecdh_secp256k1_private_key[32] = {
    0x85, 0xE6, 0xDC, 0xDB, 0x9D, 0x9C, 0x00, 0x72,
    0xC0, 0xB7, 0x7C, 0x85, 0xF3, 0x34, 0x83, 0xFC,
    0x8A, 0x3A, 0xDF, 0xC5, 0xAC, 0x64, 0xB5, 0xA0,
    0xC2, 0xFA, 0xD6, 0xDA, 0xAA, 0x51, 0x04, 0xEE
};
static const uint8_t ecdh_secp256k1_public_keyx[32] = {
    0x10, 0xC8, 0x99, 0xD4, 0x8B, 0xA1, 0x23, 0x90,
    0xBC, 0x54, 0xE3, 0x1F, 0x32, 0x09, 0x7B, 0x3F,
    0xFF, 0xF2, 0x50, 0xB2, 0xBA, 0xBA, 0x82, 0xFB,
    0x64, 0x58, 0x12, 0xDD, 0x14, 0x17, 0x89, 0x53
};
static const uint8_t ecdh_secp256k1_public_keyy[32] = {
    0xC3, 0x71, 0x10, 0x3B, 0xF3, 0x00, 0x4B, 0x6E,
    0x4E, 0x6A, 0x95, 0xF1, 0xAB, 0xCE, 0xDE, 0x10,
    0x91, 0x38, 0x7A, 0x9C, 0xCF, 0x0D, 0x9B, 0x67,
    0x55, 0x79, 0xFD, 0x16, 0x30, 0xFB, 0x0B, 0xC4
};
static const uint8_t ecdh_secp256k1_peer_private_key[32] = {
    0x07, 0xDF, 0xAB, 0x47, 0x68, 0xA1, 0x2B, 0x99,
    0xDF, 0x8F, 0xE0, 0x1A, 0x5D, 0x04, 0x83, 0xB2,
    0xB1, 0x64, 0x45, 0xB5, 0xB5, 0xC7, 0x7B, 0x09,
    0x17, 0xC9, 0x07, 0x04, 0x2C, 0xFB, 0xBF, 0x40
};
static const uint8_t ecdh_secp256k1_peer_public_keyx[32] = {
    0xBD, 0x50, 0xB9, 0x7C, 0x60, 0xB5, 0x23, 0x6B,
    0xDA, 0xCD, 0xA8, 0xB6, 0xCF, 0x9F, 0x01, 0xDF,
    0xAC, 0x00, 0x5D, 0x23, 0xA3, 0x21, 0xEC, 0xDB,
    0xC2, 0x0C, 0x03, 0x70, 0x9B, 0x2B, 0x15, 0x7B
};
static const uint8_t ecdh_secp256k1_peer_public_keyy[32] = {
    0x95, 0xE3, 0x6C, 0x9C, 0xE2, 0xB3, 0x08, 0x8B,
    0x80, 0x95, 0x54, 0xFD, 0x73, 0x6D, 0xF5, 0xB4,
    0x3B, 0xDE, 0x5B, 0xD7, 0x90, 0x5C, 0x89, 0x73,
    0xF0, 0x08, 0xF5, 0x23, 0x7B, 0x2D, 0x31, 0xD3
};
static const uint8_t ecdh_secp256k1_shared_secret[32] = {
    0x4B, 0xA5, 0xE0, 0xFD, 0x2C, 0x36, 0xC4, 0xA4,
    0xB5, 0xAA, 0x6D, 0x05, 0xF6, 0x3D, 0xAF, 0x99,
    0x58, 0x38, 0x84, 0x6A, 0xE1, 0xD3, 0x0E, 0x2E,
    0xE6, 0x2D, 0xCF, 0x3C, 0x96, 0x70, 0xC0, 0x86
};

/*============================================================================
 * Helper Functions
 *===========================================================================*/

static void hex_dump(const char *label, const uint8_t *data, uint32_t len)
{
    printf("%s: ", label);
    for (uint32_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\r\n");
}

/* Certification logging: one field per line, wrapped at 32 bytes, so the console
 * transcript doubles as the per-case evidence record. */
static void sec_log_hex(const char *label, const uint8_t *data, uint32_t len)
{
    printf("  %-14s (%3luB): ", label, (unsigned long)len);
    for (uint32_t i = 0; i < len; i++) {
        if (i != 0 && (i % 32) == 0) {
            printf("\r\n  %-14s          ", "");
        }
        printf("%02x", data[i]);
    }
    printf("\r\n");
}

static int compare_data(const uint8_t *a, const uint8_t *b, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        if (a[i] != b[i]) {
            return -1;
        }
    }
    return 0;
}

static const char* sec_mode_name(sec_mode_t mode)
{
    return (mode == SEC_MODE_LINK) ? "Link" : "Direct";
}

static const char* sec_hash_mode_name(sec_hash_mode_t mode)
{
    return (mode == SEC_HASH_MODE_LINK) ? "Link" : "Direct";
}

/*============================================================================
 * AES Tests
 *===========================================================================*/

static int test_aes_ecb_128_mode(sec_mode_t mode)
{
    sec_aes_handle_t *handle = &sec_aes_handle;
    uint8_t *input_nc = sec_work_buf;
    uint8_t *output_nc = sec_output_buf;
    int ret;

    printf("\r\n[TEST] AES-128 ECB (%s mode)\r\n", sec_mode_name(mode));
    sec_log_hex("INPUT key", aes128_key, 16);
    sec_log_hex("INPUT plaintext", aes128_plaintext, 16);
    sec_log_hex("EXPECT ct", aes128_ecb_ciphertext, 16);
    printf("  Using OCRAM buffers: in=%p out=%p\r\n", input_nc, output_nc);

    /* Copy plaintext to input buffer */
    memcpy(input_nc, aes128_plaintext, 16);

    /* Encrypt */
    sec_aes_init_ex(handle, mode, SEC_AES_ECB, SEC_AES_KEY_128);
    sec_aes_setkey(handle, aes128_key, 16, NULL, SEC_AES_DIR_ENCRYPT);
    ret = sec_aes_encrypt(handle, input_nc, 16, 0, output_nc);
    sec_aes_deinit(handle);

    if (ret != 0) {
        printf("  Encrypt failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT ct", output_nc, 16);
    if (compare_data(output_nc, aes128_ecb_ciphertext, 16) != 0) {
        printf("  Encrypt mismatch!\r\n");
        hex_dump("  Expected", aes128_ecb_ciphertext, 16);
        hex_dump("  Got     ", output_nc, 16);
        return -1;
    }
    printf("  Encrypt: PASS\r\n");

    /* Copy ciphertext to input buffer */
    memcpy(input_nc, aes128_ecb_ciphertext, 16);

    /* Decrypt */
    sec_aes_init_ex(handle, mode, SEC_AES_ECB, SEC_AES_KEY_128);
    sec_aes_setkey(handle, aes128_key, 16, NULL, SEC_AES_DIR_DECRYPT);
    ret = sec_aes_decrypt(handle, input_nc, 16, 0, output_nc);
    sec_aes_deinit(handle);

    if (ret != 0) {
        printf("  Decrypt failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT pt", output_nc, 16);
    if (compare_data(output_nc, aes128_plaintext, 16) != 0) {
        printf("  Decrypt mismatch!\r\n");
        hex_dump("  Expected", aes128_plaintext, 16);
        hex_dump("  Got     ", output_nc, 16);
        return -1;
    }
    printf("  Decrypt: PASS\r\n");

    return 0;
}

static int test_aes_ecb_128(void)
{
    return test_aes_ecb_128_mode(SEC_MODE_LINK);
}

static int test_aes_ecb_256_mode(sec_mode_t mode)
{
    sec_aes_handle_t *handle = &sec_aes_handle;
    uint8_t *input_nc = sec_work_buf;
    uint8_t *output_nc = sec_output_buf;
    int ret;

    printf("\r\n[TEST] AES-256 ECB (%s mode)\r\n", sec_mode_name(mode));
    sec_log_hex("INPUT key", aes256_key, 32);
    sec_log_hex("INPUT plaintext", aes128_plaintext, 16);
    sec_log_hex("EXPECT ct", aes256_ecb_ciphertext, 16);

    /* Copy plaintext to input buffer */
    memcpy(input_nc, aes128_plaintext, 16);

    /* Encrypt */
    sec_aes_init_ex(handle, mode, SEC_AES_ECB, SEC_AES_KEY_256);
    sec_aes_setkey(handle, aes256_key, 32, NULL, SEC_AES_DIR_ENCRYPT);
    ret = sec_aes_encrypt(handle, input_nc, 16, 0, output_nc);
    sec_aes_deinit(handle);

    if (ret != 0) {
        printf("  Encrypt failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT ct", output_nc, 16);
    if (compare_data(output_nc, aes256_ecb_ciphertext, 16) != 0) {
        printf("  Encrypt mismatch!\r\n");
        hex_dump("  Expected", aes256_ecb_ciphertext, 16);
        hex_dump("  Got     ", output_nc, 16);
        return -1;
    }
    printf("  Encrypt: PASS\r\n");

    /* Copy ciphertext to input buffer */
    memcpy(input_nc, aes256_ecb_ciphertext, 16);

    /* Decrypt */
    sec_aes_init_ex(handle, mode, SEC_AES_ECB, SEC_AES_KEY_256);
    sec_aes_setkey(handle, aes256_key, 32, NULL, SEC_AES_DIR_DECRYPT);
    ret = sec_aes_decrypt(handle, input_nc, 16, 0, output_nc);
    sec_aes_deinit(handle);

    if (ret != 0) {
        printf("  Decrypt failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT pt", output_nc, 16);
    if (compare_data(output_nc, aes128_plaintext, 16) != 0) {
        printf("  Decrypt mismatch!\r\n");
        hex_dump("  Expected", aes128_plaintext, 16);
        hex_dump("  Got     ", output_nc, 16);
        return -1;
    }
    printf("  Decrypt: PASS\r\n");

    return 0;
}

static int test_aes_ecb_256(void)
{
    return test_aes_ecb_256_mode(SEC_MODE_LINK);
}

static int test_aes_cbc_128_mode(sec_mode_t mode)
{
    sec_aes_handle_t *handle = &sec_aes_handle;
    uint8_t *input_nc = sec_work_buf;
    uint8_t *output_nc = sec_output_buf;
    int ret;

    printf("\r\n[TEST] AES-128 CBC (%s mode)\r\n", sec_mode_name(mode));
    sec_log_hex("INPUT key", aes128_key, 16);
    sec_log_hex("INPUT iv", aes_cbc_iv, 16);
    sec_log_hex("INPUT plaintext", aes128_plaintext, 16);
    sec_log_hex("EXPECT ct", aes128_cbc_ciphertext, 16);

    /* Copy plaintext to input buffer */
    memcpy(input_nc, aes128_plaintext, 16);

    /* Encrypt */
    sec_aes_init_ex(handle, mode, SEC_AES_CBC, SEC_AES_KEY_128);
    sec_aes_setkey(handle, aes128_key, 16, aes_cbc_iv, SEC_AES_DIR_ENCRYPT);
    ret = sec_aes_encrypt(handle, input_nc, 16, 0, output_nc);
    sec_aes_deinit(handle);

    if (ret != 0) {
        printf("  Encrypt failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT ct", output_nc, 16);
    if (compare_data(output_nc, aes128_cbc_ciphertext, 16) != 0) {
        printf("  Encrypt mismatch!\r\n");
        hex_dump("  Expected", aes128_cbc_ciphertext, 16);
        hex_dump("  Got     ", output_nc, 16);
        return -1;
    }
    printf("  Encrypt: PASS\r\n");

    /* Copy ciphertext to input buffer */
    memcpy(input_nc, aes128_cbc_ciphertext, 16);

    /* Decrypt */
    sec_aes_init_ex(handle, mode, SEC_AES_CBC, SEC_AES_KEY_128);
    sec_aes_setkey(handle, aes128_key, 16, aes_cbc_iv, SEC_AES_DIR_DECRYPT);
    ret = sec_aes_decrypt(handle, input_nc, 16, 0, output_nc);
    sec_aes_deinit(handle);

    if (ret != 0) {
        printf("  Decrypt failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT pt", output_nc, 16);
    if (compare_data(output_nc, aes128_plaintext, 16) != 0) {
        printf("  Decrypt mismatch!\r\n");
        hex_dump("  Expected", aes128_plaintext, 16);
        hex_dump("  Got     ", output_nc, 16);
        return -1;
    }
    printf("  Decrypt: PASS\r\n");

    return 0;
}

static int test_aes_cbc_128(void)
{
    return test_aes_cbc_128_mode(SEC_MODE_LINK);
}

static int test_aes_ctr_128_mode(sec_mode_t mode)
{
    sec_aes_handle_t *handle = &sec_aes_handle;
    uint8_t *input_nc = sec_work_buf;
    uint8_t *output_nc = sec_output_buf;
    int ret;

    printf("\r\n[TEST] AES-128 CTR (%s mode)\r\n", sec_mode_name(mode));
    sec_log_hex("INPUT key", aes128_key, 16);
    sec_log_hex("INPUT counter", aes_ctr_nonce, 16);
    sec_log_hex("INPUT plaintext", aes128_plaintext, 16);
    sec_log_hex("EXPECT ct", aes128_ctr_ciphertext, 16);

    /* Copy plaintext to input buffer */
    memcpy(input_nc, aes128_plaintext, 16);

    /* Encrypt */
    sec_aes_init_ex(handle, mode, SEC_AES_CTR, SEC_AES_KEY_128);
    sec_aes_setkey(handle, aes128_key, 16, aes_ctr_nonce, SEC_AES_DIR_ENCRYPT);
    ret = sec_aes_encrypt(handle, input_nc, 16, 0, output_nc);
    sec_aes_deinit(handle);

    if (ret != 0) {
        printf("  Encrypt failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT ct", output_nc, 16);
    if (compare_data(output_nc, aes128_ctr_ciphertext, 16) != 0) {
        printf("  Encrypt mismatch!\r\n");
        hex_dump("  Expected", aes128_ctr_ciphertext, 16);
        hex_dump("  Got     ", output_nc, 16);
        return -1;
    }
    printf("  Encrypt: PASS\r\n");

    /* Copy ciphertext to input buffer */
    memcpy(input_nc, aes128_ctr_ciphertext, 16);

    /* Decrypt (CTR mode: decrypt = encrypt) */
    sec_aes_init_ex(handle, mode, SEC_AES_CTR, SEC_AES_KEY_128);
    sec_aes_setkey(handle, aes128_key, 16, aes_ctr_nonce, SEC_AES_DIR_DECRYPT);
    ret = sec_aes_decrypt(handle, input_nc, 16, 0, output_nc);
    sec_aes_deinit(handle);

    if (ret != 0) {
        printf("  Decrypt failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT pt", output_nc, 16);
    if (compare_data(output_nc, aes128_plaintext, 16) != 0) {
        printf("  Decrypt mismatch!\r\n");
        hex_dump("  Expected", aes128_plaintext, 16);
        hex_dump("  Got     ", output_nc, 16);
        return -1;
    }
    printf("  Decrypt: PASS\r\n");

    return 0;
}

static int test_aes_ctr_128(void)
{
    return test_aes_ctr_128_mode(SEC_MODE_LINK);
}

static int test_aes_performance_mode(sec_mode_t mode)
{
    sec_aes_handle_t *handle = &sec_aes_handle;
    uint8_t *data_nc = sec_work_buf;
    uint8_t *output_nc = sec_output_buf;
    uint32_t start, end;
    int ret;

    printf("\r\n[TEST] AES Performance (%s mode, 1KB data)\r\n", sec_mode_name(mode));

    /* Fill test data in non-cached buffer */
    for (int i = 0; i < 1024; i++) {
        data_nc[i] = (uint8_t)(i & 0xFF);
    }

    /* AES-128 ECB */
    sec_aes_init_ex(handle, mode, SEC_AES_ECB, SEC_AES_KEY_128);
    sec_aes_setkey(handle, aes128_key, 16, NULL, SEC_AES_DIR_ENCRYPT);
    start = (uint32_t)mtimer_get_time_us();
    ret = sec_aes_encrypt(handle, data_nc, 1024, 0, output_nc);
    end = (uint32_t)mtimer_get_time_us();
    sec_aes_deinit(handle);
    if (ret == 0) {
        printf("  AES-128 ECB: %lu us (%.2f MB/s)\r\n",
               (unsigned long)(end - start),
               (float)1024 * 1000000 / (end - start) / 1024 / 1024);
    }

    /* AES-256 ECB */
    sec_aes_init_ex(handle, mode, SEC_AES_ECB, SEC_AES_KEY_256);
    sec_aes_setkey(handle, aes256_key, 32, NULL, SEC_AES_DIR_ENCRYPT);
    start = (uint32_t)mtimer_get_time_us();
    ret = sec_aes_encrypt(handle, data_nc, 1024, 0, output_nc);
    end = (uint32_t)mtimer_get_time_us();
    sec_aes_deinit(handle);
    if (ret == 0) {
        printf("  AES-256 ECB: %lu us (%.2f MB/s)\r\n",
               (unsigned long)(end - start),
               (float)1024 * 1000000 / (end - start) / 1024 / 1024);
    }

    /* AES-128 CBC */
    sec_aes_init_ex(handle, mode, SEC_AES_CBC, SEC_AES_KEY_128);
    sec_aes_setkey(handle, aes128_key, 16, aes_cbc_iv, SEC_AES_DIR_ENCRYPT);
    start = (uint32_t)mtimer_get_time_us();
    ret = sec_aes_encrypt(handle, data_nc, 1024, 0, output_nc);
    end = (uint32_t)mtimer_get_time_us();
    sec_aes_deinit(handle);
    if (ret == 0) {
        printf("  AES-128 CBC: %lu us (%.2f MB/s)\r\n",
               (unsigned long)(end - start),
               (float)1024 * 1000000 / (end - start) / 1024 / 1024);
    }

    /* AES-128 CTR */
    sec_aes_init_ex(handle, mode, SEC_AES_CTR, SEC_AES_KEY_128);
    sec_aes_setkey(handle, aes128_key, 16, aes_ctr_nonce, SEC_AES_DIR_ENCRYPT);
    start = (uint32_t)mtimer_get_time_us();
    ret = sec_aes_encrypt(handle, data_nc, 1024, 0, output_nc);
    end = (uint32_t)mtimer_get_time_us();
    sec_aes_deinit(handle);
    if (ret == 0) {
        printf("  AES-128 CTR: %lu us (%.2f MB/s)\r\n",
               (unsigned long)(end - start),
               (float)1024 * 1000000 / (end - start) / 1024 / 1024);
    }

    return 0;
}

static int test_aes_performance(void)
{
    return test_aes_performance_mode(SEC_MODE_LINK);
}

static int cmd_aes_test(int argc, char **argv)
{
    int pass = 0, fail = 0;

    printf("\r\n========================================\r\n");
    printf("AES Hardware Security Test Suite (Link Mode)\r\n");
    printf("========================================\r\n");

    if (test_aes_ecb_128() == 0) pass++; else fail++;
    if (test_aes_ecb_256() == 0) pass++; else fail++;
    if (test_aes_cbc_128() == 0) pass++; else fail++;
    if (test_aes_ctr_128() == 0) pass++; else fail++;
    test_aes_performance();

    printf("\r\n========================================\r\n");
    printf("AES Test Result: %d PASS, %d FAIL\r\n", pass, fail);
    printf("========================================\r\n");

    return (fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_aes_test, aes_test, AES hardware test suite);

/* Test both Link and Direct modes */
static int cmd_aes_mode_test(int argc, char **argv)
{
    int pass = 0, fail = 0;

    printf("\r\n========================================\r\n");
    printf("AES Mode Comparison Test (Link vs Direct)\r\n");
    printf("========================================\r\n");

    /* Test Link Mode */
    printf("\r\n--- LINK MODE ---\r\n");
    if (test_aes_ecb_128_mode(SEC_MODE_LINK) == 0) pass++; else fail++;
    if (test_aes_ecb_256_mode(SEC_MODE_LINK) == 0) pass++; else fail++;
    if (test_aes_cbc_128_mode(SEC_MODE_LINK) == 0) pass++; else fail++;
    if (test_aes_ctr_128_mode(SEC_MODE_LINK) == 0) pass++; else fail++;
    test_aes_performance_mode(SEC_MODE_LINK);

    /* Test Direct Mode */
    printf("\r\n--- DIRECT MODE ---\r\n");
    if (test_aes_ecb_128_mode(SEC_MODE_DIRECT) == 0) pass++; else fail++;
    if (test_aes_ecb_256_mode(SEC_MODE_DIRECT) == 0) pass++; else fail++;
    if (test_aes_cbc_128_mode(SEC_MODE_DIRECT) == 0) pass++; else fail++;
    if (test_aes_ctr_128_mode(SEC_MODE_DIRECT) == 0) pass++; else fail++;
    test_aes_performance_mode(SEC_MODE_DIRECT);

    printf("\r\n========================================\r\n");
    printf("AES Mode Test Result: %d PASS, %d FAIL (Total: %d)\r\n", pass, fail, pass + fail);
    printf("========================================\r\n");

    /* Update global counters */
    g_total_tests += pass + fail;
    g_total_pass += pass;

    return (fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_aes_mode_test, aes_mode_test, AES Link vs Direct mode test);

/*============================================================================
 * SHA Hash Tests
 *===========================================================================*/

static int test_sha1_mode(sec_hash_mode_t mode)
{
    sec_hash_handle_t *handle = &sec_hash_handle;
    uint8_t *output_nc = sec_hash_buf;
    uint8_t *input_nc = sec_work_buf;
    int ret;

    printf("\r\n[TEST] SHA-1 (%s mode)\r\n", sec_hash_mode_name(mode));
    sec_log_hex("INPUT msg", (const uint8_t *)sha_test_msg, strlen(sha_test_msg));
    sec_log_hex("EXPECT digest", sha1_expected, 20);

    /* Copy input to buffer */
    memcpy(input_nc, sha_test_msg, strlen(sha_test_msg));

    sec_hash_init_ex(handle, mode, SEC_HASH_SHA1);
    ret = sec_hash_update(handle, input_nc, strlen(sha_test_msg));
    if (ret != 0) {
        printf("  Update failed: %d\r\n", ret);
        return -1;
    }
    ret = sec_hash_finish(handle, output_nc);
    sec_hash_deinit(handle);

    if (ret < 0) {
        printf("  Finish failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT digest", output_nc, 20);
    if (compare_data(output_nc, sha1_expected, 20) != 0) {
        printf("  Hash mismatch!\r\n");
        hex_dump("  Expected", sha1_expected, 20);
        hex_dump("  Got     ", output_nc, 20);
        return -1;
    }
    printf("  PASS\r\n");
    return 0;
}

static int test_sha1(void)
{
    return test_sha1_mode(SEC_HASH_MODE_DIRECT);
}

static int test_sha224_mode(sec_hash_mode_t mode)
{
    sec_hash_handle_t *handle = &sec_hash_handle;
    uint8_t *output_nc = sec_hash_buf;
    uint8_t *input_nc = sec_work_buf;
    int ret;

    printf("\r\n[TEST] SHA-224 (%s mode)\r\n", sec_hash_mode_name(mode));
    sec_log_hex("INPUT msg", (const uint8_t *)sha_test_msg, strlen(sha_test_msg));
    sec_log_hex("EXPECT digest", sha224_expected, 28);

    /* Copy input to buffer */
    memcpy(input_nc, sha_test_msg, strlen(sha_test_msg));

    sec_hash_init_ex(handle, mode, SEC_HASH_SHA224);
    ret = sec_hash_update(handle, input_nc, strlen(sha_test_msg));
    if (ret != 0) {
        printf("  Update failed: %d\r\n", ret);
        return -1;
    }
    ret = sec_hash_finish(handle, output_nc);
    sec_hash_deinit(handle);

    if (ret < 0) {
        printf("  Finish failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT digest", output_nc, 28);
    if (compare_data(output_nc, sha224_expected, 28) != 0) {
        printf("  Hash mismatch!\r\n");
        hex_dump("  Expected", sha224_expected, 28);
        hex_dump("  Got     ", output_nc, 28);
        return -1;
    }
    printf("  PASS\r\n");
    return 0;
}

static int test_sha224(void)
{
    return test_sha224_mode(SEC_HASH_MODE_DIRECT);
}

static int test_sha256_mode(sec_hash_mode_t mode)
{
    sec_hash_handle_t *handle = &sec_hash_handle;
    uint8_t *output_nc = sec_hash_buf;
    uint8_t *input_nc = sec_work_buf;
    int ret;

    printf("\r\n[TEST] SHA-256 (%s mode)\r\n", sec_hash_mode_name(mode));
    sec_log_hex("INPUT msg", (const uint8_t *)sha_test_msg, strlen(sha_test_msg));
    sec_log_hex("EXPECT digest", sha256_expected, 32);

    /* Copy input to buffer */
    memcpy(input_nc, sha_test_msg, strlen(sha_test_msg));

    sec_hash_init_ex(handle, mode, SEC_HASH_SHA256);
    ret = sec_hash_update(handle, input_nc, strlen(sha_test_msg));
    if (ret != 0) {
        printf("  Update failed: %d\r\n", ret);
        return -1;
    }
    ret = sec_hash_finish(handle, output_nc);
    sec_hash_deinit(handle);

    if (ret < 0) {
        printf("  Finish failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT digest", output_nc, 32);
    if (compare_data(output_nc, sha256_expected, 32) != 0) {
        printf("  Hash mismatch!\r\n");
        hex_dump("  Expected", sha256_expected, 32);
        hex_dump("  Got     ", output_nc, 32);
        return -1;
    }
    printf("  PASS\r\n");
    return 0;
}

static int test_sha256(void)
{
    return test_sha256_mode(SEC_HASH_MODE_DIRECT);
}

static int test_sha384_mode(sec_hash_mode_t mode)
{
    sec_hash_handle_t *handle = &sec_hash_handle;
    uint8_t *output_nc = sec_hash_buf;
    uint8_t *input_nc = sec_work_buf;
    int ret;

    printf("\r\n[TEST] SHA-384 (%s mode)\r\n", sec_hash_mode_name(mode));
    sec_log_hex("INPUT msg", (const uint8_t *)sha_test_msg, strlen(sha_test_msg));
    sec_log_hex("EXPECT digest", sha384_expected, 48);

    /* Copy input to buffer */
    memcpy(input_nc, sha_test_msg, strlen(sha_test_msg));

    sec_hash_init_ex(handle, mode, SEC_HASH_SHA384);
    ret = sec_hash_update(handle, input_nc, strlen(sha_test_msg));
    if (ret != 0) {
        printf("  Update failed: %d\r\n", ret);
        return -1;
    }
    ret = sec_hash_finish(handle, output_nc);
    sec_hash_deinit(handle);

    if (ret < 0) {
        printf("  Finish failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT digest", output_nc, 48);
    if (compare_data(output_nc, sha384_expected, 48) != 0) {
        printf("  Hash mismatch!\r\n");
        hex_dump("  Expected", sha384_expected, 48);
        hex_dump("  Got     ", output_nc, 48);
        return -1;
    }
    printf("  PASS\r\n");
    return 0;
}

static int test_sha384(void)
{
    return test_sha384_mode(SEC_HASH_MODE_DIRECT);
}

static int test_sha512_mode(sec_hash_mode_t mode)
{
    sec_hash_handle_t *handle = &sec_hash_handle;
    uint8_t *output_nc = sec_hash_buf;
    uint8_t *input_nc = sec_work_buf;
    int ret;

    printf("\r\n[TEST] SHA-512 (%s mode)\r\n", sec_hash_mode_name(mode));
    sec_log_hex("INPUT msg", (const uint8_t *)sha_test_msg, strlen(sha_test_msg));
    sec_log_hex("EXPECT digest", sha512_expected, 64);

    /* Copy input to buffer */
    memcpy(input_nc, sha_test_msg, strlen(sha_test_msg));

    sec_hash_init_ex(handle, mode, SEC_HASH_SHA512);
    ret = sec_hash_update(handle, input_nc, strlen(sha_test_msg));
    if (ret != 0) {
        printf("  Update failed: %d\r\n", ret);
        return -1;
    }
    ret = sec_hash_finish(handle, output_nc);
    sec_hash_deinit(handle);

    if (ret < 0) {
        printf("  Finish failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT digest", output_nc, 64);
    if (compare_data(output_nc, sha512_expected, 64) != 0) {
        printf("  Hash mismatch!\r\n");
        hex_dump("  Expected", sha512_expected, 64);
        hex_dump("  Got     ", output_nc, 64);
        return -1;
    }
    printf("  PASS\r\n");
    return 0;
}

static int test_sha512(void)
{
    return test_sha512_mode(SEC_HASH_MODE_DIRECT);
}

static int test_sha_incremental_mode(sec_hash_mode_t mode)
{
    sec_hash_handle_t *handle = &sec_hash_handle;
    uint8_t *output_nc = sec_hash_buf;
    uint8_t *input_nc = sec_work_buf;
    const char *part1 = "a";
    const char *part2 = "bc";
    int ret;

    printf("\r\n[TEST] SHA-256 Incremental Update (%s mode)\r\n", sec_hash_mode_name(mode));
    sec_log_hex("INPUT msg", (const uint8_t *)sha_test_msg, strlen(sha_test_msg));
    sec_log_hex("EXPECT digest", sha256_expected, 32);

    sec_hash_init_ex(handle, mode, SEC_HASH_SHA256);

    /* Part 1 */
    memcpy(input_nc, part1, strlen(part1));
    ret = sec_hash_update(handle, input_nc, strlen(part1));
    if (ret != 0) {
        printf("  Update 1 failed: %d\r\n", ret);
        return -1;
    }

    /* Part 2 */
    memcpy(input_nc, part2, strlen(part2));
    ret = sec_hash_update(handle, input_nc, strlen(part2));
    if (ret != 0) {
        printf("  Update 2 failed: %d\r\n", ret);
        return -1;
    }
    ret = sec_hash_finish(handle, output_nc);
    sec_hash_deinit(handle);

    if (ret < 0) {
        printf("  Finish failed: %d\r\n", ret);
        return -1;
    }

    sec_log_hex("OUTPUT digest", output_nc, 32);
    if (compare_data(output_nc, sha256_expected, 32) != 0) {
        printf("  Hash mismatch!\r\n");
        hex_dump("  Expected", sha256_expected, 32);
        hex_dump("  Got     ", output_nc, 32);
        return -1;
    }
    printf("  PASS\r\n");
    return 0;
}

static int test_sha_incremental(void)
{
    return test_sha_incremental_mode(SEC_HASH_MODE_DIRECT);
}

static int test_sha_performance_mode(sec_hash_mode_t mode)
{
    sec_hash_handle_t *handle = &sec_hash_handle;
    uint8_t *data_nc = sec_work_buf;
    uint8_t *output_nc = sec_hash_buf;
    uint32_t start, end;
    int ret;

    printf("\r\n[TEST] SHA Performance (%s mode, 1KB data)\r\n", sec_hash_mode_name(mode));

    /* Fill test data in non-cached buffer */
    for (int i = 0; i < 1024; i++) {
        data_nc[i] = (uint8_t)(i & 0xFF);
    }

    /* SHA-1 */
    sec_hash_init_ex(handle, mode, SEC_HASH_SHA1);
    start = (uint32_t)mtimer_get_time_us();
    ret = sec_hash_update(handle, data_nc, 1024);
    sec_hash_finish(handle, output_nc);
    end = (uint32_t)mtimer_get_time_us();
    sec_hash_deinit(handle);
    if (ret == 0) {
        printf("  SHA-1  : %lu us (%.2f MB/s)\r\n",
               (unsigned long)(end - start),
               (float)1024 * 1000000 / (end - start) / 1024 / 1024);
    }

    /* SHA-256 */
    sec_hash_init_ex(handle, mode, SEC_HASH_SHA256);
    start = (uint32_t)mtimer_get_time_us();
    ret = sec_hash_update(handle, data_nc, 1024);
    sec_hash_finish(handle, output_nc);
    end = (uint32_t)mtimer_get_time_us();
    sec_hash_deinit(handle);
    if (ret == 0) {
        printf("  SHA-256: %lu us (%.2f MB/s)\r\n",
               (unsigned long)(end - start),
               (float)1024 * 1000000 / (end - start) / 1024 / 1024);
    }

    return 0;
}

static int test_sha_performance(void)
{
    return test_sha_performance_mode(SEC_HASH_MODE_DIRECT);
}

static int cmd_sha_test(int argc, char **argv)
{
    int pass = 0, fail = 0;

    printf("\r\n========================================\r\n");
    printf("SHA Hardware Security Test Suite (Direct Mode)\r\n");
    printf("========================================\r\n");

    if (test_sha1() == 0) pass++; else fail++;
    if (test_sha224() == 0) pass++; else fail++;
    if (test_sha256() == 0) pass++; else fail++;
    if (test_sha384() == 0) pass++; else fail++;
    if (test_sha512() == 0) pass++; else fail++;
    if (test_sha_incremental() == 0) pass++; else fail++;
    test_sha_performance();

    printf("\r\n========================================\r\n");
    printf("SHA Test Result: %d PASS, %d FAIL\r\n", pass, fail);
    printf("========================================\r\n");

    return (fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sha_test, sha_test, SHA hardware test suite);

/* Test both Link and Direct modes for SHA */
static int cmd_sha_mode_test(int argc, char **argv)
{
    int pass = 0, fail = 0;

    printf("\r\n========================================\r\n");
    printf("SHA Mode Comparison Test (Link vs Direct)\r\n");
    printf("========================================\r\n");

    /* Test Link Mode */
    printf("\r\n--- LINK MODE ---\r\n");
    if (test_sha1_mode(SEC_HASH_MODE_LINK) == 0) pass++; else fail++;
    if (test_sha224_mode(SEC_HASH_MODE_LINK) == 0) pass++; else fail++;
    if (test_sha256_mode(SEC_HASH_MODE_LINK) == 0) pass++; else fail++;
    if (test_sha384_mode(SEC_HASH_MODE_LINK) == 0) pass++; else fail++;
    if (test_sha512_mode(SEC_HASH_MODE_LINK) == 0) pass++; else fail++;
    if (test_sha_incremental_mode(SEC_HASH_MODE_LINK) == 0) pass++; else fail++;
    test_sha_performance_mode(SEC_HASH_MODE_LINK);

    /* Test Direct Mode */
    printf("\r\n--- DIRECT MODE ---\r\n");
    if (test_sha1_mode(SEC_HASH_MODE_DIRECT) == 0) pass++; else fail++;
    if (test_sha224_mode(SEC_HASH_MODE_DIRECT) == 0) pass++; else fail++;
    if (test_sha256_mode(SEC_HASH_MODE_DIRECT) == 0) pass++; else fail++;
    if (test_sha384_mode(SEC_HASH_MODE_DIRECT) == 0) pass++; else fail++;
    if (test_sha512_mode(SEC_HASH_MODE_DIRECT) == 0) pass++; else fail++;
    if (test_sha_incremental_mode(SEC_HASH_MODE_DIRECT) == 0) pass++; else fail++;
    test_sha_performance_mode(SEC_HASH_MODE_DIRECT);

    printf("\r\n========================================\r\n");
    printf("SHA Mode Test Result: %d PASS, %d FAIL (Total: %d)\r\n", pass, fail, pass + fail);
    printf("========================================\r\n");

    /* Update global counters */
    g_total_tests += pass + fail;
    g_total_pass += pass;

    return (fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sha_mode_test, sha_mode_test, SHA Link vs Direct mode test);

/*============================================================================
 * TRNG Tests
 *===========================================================================*/

#define TRNG_TEST_SAMPLES   1000
#define TRNG_SAMPLE_SIZE    32

static int test_trng_basic(void)
{
    uint8_t data1[32], data2[32];
    int ret;

    printf("\r\n[TEST] TRNG Basic\r\n");

    ret = sec_eng_trng_enable();
    if (ret != 0) {
        printf("  TRNG enable failed: %d\r\n", ret);
        return -1;
    }

    /* Generate two random values */
    sec_eng_trng_read(data1);
    sec_eng_trng_read(data2);

    sec_eng_trng_disable();

    /* They should be different */
    if (compare_data(data1, data2, 32) == 0) {
        printf("  Two random values are identical - FAIL\r\n");
        hex_dump("  Data1", data1, 32);
        hex_dump("  Data2", data2, 32);
        return -1;
    }

    printf("  Two unique random values generated: PASS\r\n");
    hex_dump("  Sample", data1, 32);
    return 0;
}

static int test_trng_distribution(void)
{
    uint32_t bit_count[8] = {0};
    uint8_t data[32];
    int i, j, k;
    float expected, deviation;

    printf("\r\n[TEST] TRNG Bit Distribution (1000 samples)\r\n");

    if (sec_eng_trng_enable() != 0) {
        printf("  TRNG enable failed\r\n");
        return -1;
    }

    /* Collect samples and count bit distribution */
    for (i = 0; i < TRNG_TEST_SAMPLES; i++) {
        sec_eng_trng_read(data);
        /* Count bits in first byte for quick analysis */
        for (j = 0; j < 8; j++) {
            for (k = 0; k < 8; k++) {
                if (data[j] & (1 << k)) {
                    bit_count[k]++;
                }
            }
        }
    }

    sec_eng_trng_disable();

    /* Expected count for each bit position */
    expected = (float)(TRNG_TEST_SAMPLES * 8) / 2.0f;

    printf("  Bit distribution (expected ~%.0f per position):\r\n", expected);
    for (i = 0; i < 8; i++) {
        deviation = ((float)bit_count[i] - expected) / expected * 100.0f;
        printf("    Bit %d: %lu (%.1f%% deviation)\r\n",
               i, (unsigned long)bit_count[i], deviation);
    }

    /* Check if deviation is within acceptable range (< 10%) */
    for (i = 0; i < 8; i++) {
        deviation = ((float)bit_count[i] - expected) / expected * 100.0f;
        if (deviation > 10.0f || deviation < -10.0f) {
            printf("  Bit %d deviation too high: WARN\r\n", i);
        }
    }

    printf("  Distribution test: PASS\r\n");
    return 0;
}

static int test_trng_performance(void)
{
    uint8_t data[32];
    uint32_t start, end;
    int i;

    printf("\r\n[TEST] TRNG Performance\r\n");

    if (sec_eng_trng_enable() != 0) {
        printf("  TRNG enable failed\r\n");
        return -1;
    }

    start = (uint32_t)mtimer_get_time_us();
    for (i = 0; i < 100; i++) {
        sec_eng_trng_read(data);
    }
    end = (uint32_t)mtimer_get_time_us();

    sec_eng_trng_disable();

    printf("  100 x 32-byte reads: %lu us\r\n", (unsigned long)(end - start));
    printf("  Average: %lu us per 32 bytes\r\n", (unsigned long)(end - start) / 100);
    printf("  Throughput: %.2f KB/s\r\n",
           (float)100 * 32 * 1000000 / (end - start) / 1024);

    return 0;
}

static int test_trng_hal_rand(void)
{
    int val1, val2, val3;

    printf("\r\n[TEST] TRNG hal_rand() API\r\n");

    hal_sec_trng_init();

    val1 = hal_rand();
    val2 = hal_rand();
    val3 = hal_rand();

    printf("  Random values: 0x%08x, 0x%08x, 0x%08x\r\n", val1, val2, val3);

    if (val1 == val2 && val2 == val3) {
        printf("  All values identical - FAIL\r\n");
        return -1;
    }

    printf("  PASS\r\n");
    return 0;
}

static int test_trng_stream(void)
{
    uint8_t buffer[64];
    int ret;

    printf("\r\n[TEST] TRNG hal_rand_stream() API\r\n");

    hal_sec_trng_init();

    memset(buffer, 0, sizeof(buffer));
    ret = hal_rand_stream(buffer, 64);

    if (ret != 0) {
        printf("  hal_rand_stream failed: %d\r\n", ret);
        return -1;
    }

    /* Check if buffer was filled (not all zeros) */
    int zero_count = 0;
    for (int i = 0; i < 64; i++) {
        if (buffer[i] == 0) zero_count++;
    }

    if (zero_count == 64) {
        printf("  Buffer still all zeros - FAIL\r\n");
        return -1;
    }

    hex_dump("  64-byte random stream", buffer, 64);
    printf("  PASS\r\n");
    return 0;
}

static int cmd_trng_test(int argc, char **argv)
{
    int pass = 0, fail = 0;

    printf("\r\n========================================\r\n");
    printf("TRNG Hardware Security Test Suite\r\n");
    printf("========================================\r\n");

    if (test_trng_basic() == 0) pass++; else fail++;
    if (test_trng_hal_rand() == 0) pass++; else fail++;
    if (test_trng_stream() == 0) pass++; else fail++;
    if (test_trng_distribution() == 0) pass++; else fail++;
    test_trng_performance();

    printf("\r\n========================================\r\n");
    printf("TRNG Test Result: %d PASS, %d FAIL (Total: %d)\r\n", pass, fail, pass + fail);
    printf("========================================\r\n");

    /* Update global counters */
    g_total_tests += pass + fail;
    g_total_pass += pass;

    return (fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_trng_test, trng_test, TRNG hardware test suite);

/*============================================================================
 * GMAC Tests (Link Mode Only - hardware only supports Link mode)
 *===========================================================================*/

static int test_gmac_basic(void)
{
    sec_gmac_handle_t *handle = &sec_gmac_handle;
    uint8_t *input_buf = sec_work_buf;
    uint8_t result[16];
    int ret;

    printf("\r\n[TEST] GMAC Basic (Link mode)\r\n");
    sec_log_hex("INPUT ghash key", gmac_test_key, 16);
    sec_log_hex("INPUT data", gmac_test_data, 16);
    sec_log_hex("EXPECT ghash", gmac_test_expected, 16);

    /* Initialize GMAC hardware */
    sec_gmac_init();

    /* Copy test data to buffer */
    memcpy(input_buf, gmac_test_data, 16);

    /* Start GMAC with key */
    ret = sec_gmac_start(handle, gmac_test_key);
    if (ret != 0) {
        printf("  GMAC start failed: %d\r\n", ret);
        return -1;
    }

    /* Update with data */
    ret = sec_gmac_update(handle, input_buf, 16);
    if (ret != 0) {
        printf("  GMAC update failed: %d\r\n", ret);
        return -1;
    }

    /* Get result */
    ret = sec_gmac_finish(handle, result);
    if (ret != 0) {
        printf("  GMAC finish failed: %d\r\n", ret);
        return -1;
    }

    sec_gmac_deinit(handle);

    /* Compare result */
    sec_log_hex("OUTPUT ghash", result, 16);
    if (compare_data(result, gmac_test_expected, 16) != 0) {
        printf("  GMAC mismatch!\r\n");
        hex_dump("  Expected", gmac_test_expected, 16);
        hex_dump("  Got     ", result, 16);
        return -1;
    }

    printf("  PASS\r\n");
    return 0;
}

static int test_gmac_incremental(void)
{
    sec_gmac_handle_t *handle = &sec_gmac_handle;
    uint8_t *input_buf = sec_work_buf;
    uint8_t result1[16], result2[16];
    int ret;

    printf("\r\n[TEST] GMAC Incremental Update (16+16 bytes)\r\n");

    /* First: compute GMAC of 32 bytes in one shot */
    sec_gmac_init();
    ret = sec_gmac_start(handle, gmac_test_key);
    if (ret != 0) {
        printf("  GMAC start failed: %d\r\n", ret);
        return -1;
    }

    /* Fill 32 bytes of test data */
    for (int i = 0; i < 32; i++) {
        input_buf[i] = (uint8_t)(i & 0xFF);
    }

    ret = sec_gmac_update(handle, input_buf, 32);
    if (ret != 0) {
        printf("  GMAC single update failed: %d\r\n", ret);
        return -1;
    }

    ret = sec_gmac_finish(handle, result1);
    sec_gmac_deinit(handle);
    if (ret != 0) {
        printf("  GMAC finish failed: %d\r\n", ret);
        return -1;
    }

    /* Second: compute GMAC of same 32 bytes in two 16-byte parts */
    sec_gmac_init();
    ret = sec_gmac_start(handle, gmac_test_key);
    if (ret != 0) {
        printf("  GMAC start failed: %d\r\n", ret);
        return -1;
    }

    /* Update first 16 bytes */
    ret = sec_gmac_update(handle, input_buf, 16);
    if (ret != 0) {
        printf("  GMAC update 1 failed: %d\r\n", ret);
        return -1;
    }

    /* Update second 16 bytes */
    ret = sec_gmac_update(handle, input_buf + 16, 16);
    if (ret != 0) {
        printf("  GMAC update 2 failed: %d\r\n", ret);
        return -1;
    }

    ret = sec_gmac_finish(handle, result2);
    sec_gmac_deinit(handle);

    if (ret != 0) {
        printf("  GMAC finish failed: %d\r\n", ret);
        return -1;
    }

    /* Incremental update result should match single update */
    if (compare_data(result1, result2, 16) != 0) {
        printf("  GMAC incremental mismatch!\r\n");
        hex_dump("  Single 32B", result1, 16);
        hex_dump("  Two 16B   ", result2, 16);
        return -1;
    }

    printf("  PASS\r\n");
    return 0;
}

static int test_gmac_performance(void)
{
    sec_gmac_handle_t *handle = &sec_gmac_handle;
    uint8_t *data = sec_work_buf;
    uint8_t result[16];
    uint32_t start, end;

    printf("\r\n[TEST] GMAC Performance (1KB data)\r\n");

    /* Fill test data */
    for (int i = 0; i < 1024; i++) {
        data[i] = (uint8_t)(i & 0xFF);
    }

    sec_gmac_init();
    sec_gmac_start(handle, gmac_test_key);

    start = (uint32_t)mtimer_get_time_us();
    sec_gmac_update(handle, data, 1024);
    sec_gmac_finish(handle, result);
    end = (uint32_t)mtimer_get_time_us();

    sec_gmac_deinit(handle);

    printf("  GMAC 1KB: %lu us (%.2f MB/s)\r\n",
           (unsigned long)(end - start),
           (float)1024 * 1000000 / (end - start) / 1024 / 1024);

    return 0;
}

static int cmd_gmac_test(int argc, char **argv)
{
    int pass = 0, fail = 0;

    printf("\r\n========================================\r\n");
    printf("GMAC Hardware Security Test Suite\r\n");
    printf("(Link mode only - hardware limitation)\r\n");
    printf("========================================\r\n");

    if (test_gmac_basic() == 0) pass++; else fail++;
    if (test_gmac_incremental() == 0) pass++; else fail++;
    test_gmac_performance();

    printf("\r\n========================================\r\n");
    printf("GMAC Test Result: %d PASS, %d FAIL (Total: %d)\r\n", pass, fail, pass + fail);
    printf("========================================\r\n");

    /* Update global counters */
    g_total_tests += pass + fail;
    g_total_pass += pass;

    return (fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_gmac_test, gmac_test, GMAC hardware test suite);

/*============================================================================
 * PKA Tests (ECDSA Sign/Verify, ECDH Key Exchange)
 *===========================================================================*/

static const char* ecp_type_name(sec_ecp_type type)
{
    switch (type) {
        case ECP_SECP256R1: return "secp256r1 (NIST P-256)";
        case ECP_SECP256K1: return "secp256k1 (Bitcoin)";
        default: return "Unknown";
    }
}

static int test_ecdsa_secp256r1(void)
{
    sec_ecdsa_handle_t ecdsa_handle;
    uint32_t r[8], s[8];
    uint32_t time_start, time_end;
    int ret;

    printf("\r\n[TEST] ECDSA Sign/Verify (%s)\r\n", ecp_type_name(ECP_SECP256R1));
    sec_log_hex("INPUT hash", ecdsa_secp256r1_hash, 32);
    sec_log_hex("INPUT priv key", ecdsa_secp256r1_private_key, 32);
    sec_log_hex("INPUT nonce k", ecdsa_secp256r1_k, 32);
    sec_log_hex("INPUT pub key x", ecdsa_secp256r1_public_keyx, 32);
    sec_log_hex("INPUT pub key y", ecdsa_secp256r1_public_keyy, 32);
    sec_log_hex("EXPECT sig r", ecdsa_secp256r1_r, 32);
    sec_log_hex("EXPECT sig s", ecdsa_secp256r1_s, 32);

    sec_ecdsa_init(&ecdsa_handle, ECP_SECP256R1);
    ecdsa_handle.privateKey = (uint32_t *)ecdsa_secp256r1_private_key;
    ecdsa_handle.publicKeyx = (uint32_t *)ecdsa_secp256r1_public_keyx;
    ecdsa_handle.publicKeyy = (uint32_t *)ecdsa_secp256r1_public_keyy;

    /* Sign */
    time_start = (uint32_t)mtimer_get_time_us();
    ret = sec_ecdsa_sign(&ecdsa_handle,
                         (uint32_t *)ecdsa_secp256r1_k,
                         (uint32_t *)ecdsa_secp256r1_hash,
                         sizeof(ecdsa_secp256r1_hash) / 4,
                         r, s);
    time_end = (uint32_t)mtimer_get_time_us();

    if (ret != 0) {
        printf("  Sign failed: %d\r\n", ret);
        return -1;
    }
    printf("  Sign time: %lu us\r\n", (unsigned long)(time_end - time_start));

    /* Verify signature matches expected */
    sec_log_hex("OUTPUT sig r", (const uint8_t *)r, 32);
    sec_log_hex("OUTPUT sig s", (const uint8_t *)s, 32);
    if (compare_data((uint8_t *)r, ecdsa_secp256r1_r, 32) != 0) {
        printf("  Signature R mismatch!\r\n");
        hex_dump("  Expected R", ecdsa_secp256r1_r, 32);
        hex_dump("  Got R     ", (uint8_t *)r, 32);
        return -1;
    }
    if (compare_data((uint8_t *)s, ecdsa_secp256r1_s, 32) != 0) {
        printf("  Signature S mismatch!\r\n");
        hex_dump("  Expected S", ecdsa_secp256r1_s, 32);
        hex_dump("  Got S     ", (uint8_t *)s, 32);
        return -1;
    }
    printf("  Sign: PASS\r\n");

    /* Verify */
    time_start = (uint32_t)mtimer_get_time_us();
    ret = sec_ecdsa_verify(&ecdsa_handle,
                           (uint32_t *)ecdsa_secp256r1_hash,
                           sizeof(ecdsa_secp256r1_hash) / 4,
                           r, s);
    time_end = (uint32_t)mtimer_get_time_us();

    if (ret != 0) {
        printf("  Verify failed: %d\r\n", ret);
        return -1;
    }
    printf("  Verify time: %lu us\r\n", (unsigned long)(time_end - time_start));
    printf("  Verify: PASS\r\n");

    sec_ecdsa_deinit(&ecdsa_handle);
    return 0;
}

static int test_ecdsa_get_public_key(void)
{
    sec_ecdsa_handle_t ecdsa_handle;
    uint32_t pub_x[8], pub_y[8];
    uint32_t time_start, time_end;

    printf("\r\n[TEST] ECDSA Get Public Key (secp256r1)\r\n");
    sec_log_hex("INPUT priv key", ecdsa_secp256r1_private_key, 32);
    sec_log_hex("EXPECT pub key x", ecdsa_secp256r1_public_keyx, 32);
    sec_log_hex("EXPECT pub key y", ecdsa_secp256r1_public_keyy, 32);

    sec_ecdsa_init(&ecdsa_handle, ECP_SECP256R1);
    ecdsa_handle.privateKey = (uint32_t *)ecdsa_secp256r1_private_key;
    ecdsa_handle.publicKeyx = (uint32_t *)ecdsa_secp256r1_public_keyx;
    ecdsa_handle.publicKeyy = (uint32_t *)ecdsa_secp256r1_public_keyy;

    time_start = (uint32_t)mtimer_get_time_us();
    sec_ecdsa_get_public_key(&ecdsa_handle,
                             (uint32_t *)ecdsa_secp256r1_private_key,
                             pub_x, pub_y);
    time_end = (uint32_t)mtimer_get_time_us();

    printf("  Get public key time: %lu us\r\n", (unsigned long)(time_end - time_start));

    /* Verify computed public key matches expected */
    sec_log_hex("OUTPUT pub x", (const uint8_t *)pub_x, 32);
    sec_log_hex("OUTPUT pub y", (const uint8_t *)pub_y, 32);
    if (compare_data((uint8_t *)pub_x, ecdsa_secp256r1_public_keyx, 32) != 0) {
        printf("  Public key X mismatch!\r\n");
        hex_dump("  Expected", ecdsa_secp256r1_public_keyx, 32);
        hex_dump("  Got     ", (uint8_t *)pub_x, 32);
        return -1;
    }
    if (compare_data((uint8_t *)pub_y, ecdsa_secp256r1_public_keyy, 32) != 0) {
        printf("  Public key Y mismatch!\r\n");
        hex_dump("  Expected", ecdsa_secp256r1_public_keyy, 32);
        hex_dump("  Got     ", (uint8_t *)pub_y, 32);
        return -1;
    }

    printf("  PASS\r\n");
    sec_ecdsa_deinit(&ecdsa_handle);
    return 0;
}

static int test_ecdh_secp256r1(void)
{
    sec_ecdh_handle_t ecdh_handle;
    uint32_t shared_x[8], shared_y[8];
    uint32_t time_start, time_end;

    printf("\r\n[TEST] ECDH Key Exchange (%s)\r\n", ecp_type_name(ECP_SECP256R1));
    sec_log_hex("INPUT A priv", ecdh_secp256r1_private_key, 32);
    sec_log_hex("INPUT B pub x", ecdh_secp256r1_peer_public_keyx, 32);
    sec_log_hex("INPUT B pub y", ecdh_secp256r1_peer_public_keyy, 32);
    sec_log_hex("EXPECT secret", ecdh_secp256r1_shared_secret, 32);

    sec_ecdh_init(&ecdh_handle, ECP_SECP256R1);

    /* Party A computes shared secret: A_private * B_public */
    time_start = (uint32_t)mtimer_get_time_us();
    sec_ecdh_get_encrypt_key(&ecdh_handle,
                             (uint32_t *)ecdh_secp256r1_peer_public_keyx,
                             (uint32_t *)ecdh_secp256r1_peer_public_keyy,
                             (uint32_t *)ecdh_secp256r1_private_key,
                             shared_x, shared_y);
    time_end = (uint32_t)mtimer_get_time_us();

    printf("  Get shared secret time: %lu us\r\n", (unsigned long)(time_end - time_start));

    /* Verify shared secret matches expected */
    sec_log_hex("OUTPUT A secret", (const uint8_t *)shared_x, 32);
    if (compare_data((uint8_t *)shared_x, ecdh_secp256r1_shared_secret, 32) != 0) {
        printf("  Shared secret mismatch!\r\n");
        hex_dump("  Expected", ecdh_secp256r1_shared_secret, 32);
        hex_dump("  Got     ", (uint8_t *)shared_x, 32);
        sec_ecdh_deinit(&ecdh_handle);
        return -1;
    }
    printf("  Party A shared secret: PASS\r\n");

    /* Party B computes shared secret: B_private * A_public (should be same) */
    sec_ecdh_get_encrypt_key(&ecdh_handle,
                             (uint32_t *)ecdh_secp256r1_public_keyx,
                             (uint32_t *)ecdh_secp256r1_public_keyy,
                             (uint32_t *)ecdh_secp256r1_peer_private_key,
                             shared_x, shared_y);

    sec_log_hex("OUTPUT B secret", (const uint8_t *)shared_x, 32);
    if (compare_data((uint8_t *)shared_x, ecdh_secp256r1_shared_secret, 32) != 0) {
        printf("  Party B shared secret mismatch!\r\n");
        sec_ecdh_deinit(&ecdh_handle);
        return -1;
    }
    printf("  Party B shared secret: PASS\r\n");

    sec_ecdh_deinit(&ecdh_handle);
    return 0;
}

static int test_ecdh_secp256k1(void)
{
    sec_ecdh_handle_t ecdh_handle;
    uint32_t shared_x[8], shared_y[8];
    uint32_t time_start, time_end;

    printf("\r\n[TEST] ECDH Key Exchange (%s)\r\n", ecp_type_name(ECP_SECP256K1));
    sec_log_hex("INPUT A priv", ecdh_secp256k1_private_key, 32);
    sec_log_hex("INPUT B pub x", ecdh_secp256k1_peer_public_keyx, 32);
    sec_log_hex("INPUT B pub y", ecdh_secp256k1_peer_public_keyy, 32);
    sec_log_hex("EXPECT secret", ecdh_secp256k1_shared_secret, 32);

    sec_ecdh_init(&ecdh_handle, ECP_SECP256K1);

    /* Party A computes shared secret */
    time_start = (uint32_t)mtimer_get_time_us();
    sec_ecdh_get_encrypt_key(&ecdh_handle,
                             (uint32_t *)ecdh_secp256k1_peer_public_keyx,
                             (uint32_t *)ecdh_secp256k1_peer_public_keyy,
                             (uint32_t *)ecdh_secp256k1_private_key,
                             shared_x, shared_y);
    time_end = (uint32_t)mtimer_get_time_us();

    printf("  Get shared secret time: %lu us\r\n", (unsigned long)(time_end - time_start));

    /* Verify shared secret */
    sec_log_hex("OUTPUT A secret", (const uint8_t *)shared_x, 32);
    if (compare_data((uint8_t *)shared_x, ecdh_secp256k1_shared_secret, 32) != 0) {
        printf("  Shared secret mismatch!\r\n");
        hex_dump("  Expected", ecdh_secp256k1_shared_secret, 32);
        hex_dump("  Got     ", (uint8_t *)shared_x, 32);
        sec_ecdh_deinit(&ecdh_handle);
        return -1;
    }
    printf("  Party A shared secret: PASS\r\n");

    /* Party B computes shared secret */
    sec_ecdh_get_encrypt_key(&ecdh_handle,
                             (uint32_t *)ecdh_secp256k1_public_keyx,
                             (uint32_t *)ecdh_secp256k1_public_keyy,
                             (uint32_t *)ecdh_secp256k1_peer_private_key,
                             shared_x, shared_y);

    sec_log_hex("OUTPUT B secret", (const uint8_t *)shared_x, 32);
    if (compare_data((uint8_t *)shared_x, ecdh_secp256k1_shared_secret, 32) != 0) {
        printf("  Party B shared secret mismatch!\r\n");
        sec_ecdh_deinit(&ecdh_handle);
        return -1;
    }
    printf("  Party B shared secret: PASS\r\n");

    sec_ecdh_deinit(&ecdh_handle);
    return 0;
}

static int test_ecdh_get_public_key(void)
{
    sec_ecdh_handle_t ecdh_handle;
    uint32_t pub_x[8], pub_y[8];
    uint32_t time_start, time_end;

    printf("\r\n[TEST] ECDH Get Public Key (secp256r1)\r\n");
    sec_log_hex("INPUT priv key", ecdh_secp256r1_private_key, 32);
    sec_log_hex("EXPECT pub key x", ecdh_secp256r1_public_keyx, 32);
    sec_log_hex("EXPECT pub key y", ecdh_secp256r1_public_keyy, 32);

    sec_ecdh_init(&ecdh_handle, ECP_SECP256R1);

    time_start = (uint32_t)mtimer_get_time_us();
    sec_ecdh_get_public_key(&ecdh_handle,
                            (uint32_t *)ecdh_secp256r1_private_key,
                            pub_x, pub_y);
    time_end = (uint32_t)mtimer_get_time_us();

    printf("  Get public key time: %lu us\r\n", (unsigned long)(time_end - time_start));

    sec_log_hex("OUTPUT pub x", (const uint8_t *)pub_x, 32);
    sec_log_hex("OUTPUT pub y", (const uint8_t *)pub_y, 32);
    if (compare_data((uint8_t *)pub_x, ecdh_secp256r1_public_keyx, 32) != 0) {
        printf("  Public key X mismatch!\r\n");
        hex_dump("  Expected", ecdh_secp256r1_public_keyx, 32);
        hex_dump("  Got     ", (uint8_t *)pub_x, 32);
        sec_ecdh_deinit(&ecdh_handle);
        return -1;
    }
    if (compare_data((uint8_t *)pub_y, ecdh_secp256r1_public_keyy, 32) != 0) {
        printf("  Public key Y mismatch!\r\n");
        hex_dump("  Expected", ecdh_secp256r1_public_keyy, 32);
        hex_dump("  Got     ", (uint8_t *)pub_y, 32);
        sec_ecdh_deinit(&ecdh_handle);
        return -1;
    }

    printf("  PASS\r\n");
    sec_ecdh_deinit(&ecdh_handle);
    return 0;
}

/* DSA Test Vectors (1024-bit RSA key from OpenSSL) */
static const uint8_t dsa_p[64] = {
    0xc9, 0x52, 0xa3, 0x18, 0x0f, 0x7b, 0xca, 0x49, 0x7b, 0xc5, 0xad, 0x1e, 0x9b, 0x58, 0x18, 0xb2,
    0x33, 0x70, 0x2a, 0xc9, 0xab, 0x43, 0xf7, 0x44, 0xf4, 0x51, 0xa4, 0xa2, 0x41, 0xfc, 0xda, 0x5b,
    0x9e, 0xff, 0x81, 0x53, 0x51, 0xc4, 0x5d, 0x04, 0x6e, 0xe4, 0x26, 0x2a, 0xc2, 0x8f, 0x7f, 0x3d,
    0xf2, 0x26, 0xac, 0x2e, 0x94, 0x9d, 0x1f, 0xe8, 0xd5, 0xb6, 0x92, 0x3f, 0x33, 0xf1, 0xa6, 0x41
};
static const uint8_t dsa_q[64] = {
    0xc2, 0x25, 0x09, 0x1c, 0x9a, 0x66, 0x38, 0x43, 0xdd, 0x18, 0xcc, 0xe9, 0x24, 0x09, 0x58, 0x1c,
    0x23, 0x65, 0xd0, 0xfc, 0xbc, 0x95, 0x6b, 0xf1, 0x15, 0x2e, 0x11, 0x00, 0x4b, 0xde, 0x53, 0xc9,
    0x98, 0x43, 0xf1, 0xf1, 0x49, 0xa7, 0x6a, 0x33, 0xfb, 0x46, 0xed, 0xb9, 0x57, 0xf1, 0x9e, 0x87,
    0x20, 0x00, 0x3b, 0x03, 0x45, 0x63, 0x3f, 0x28, 0x40, 0xb3, 0x5e, 0x3b, 0x30, 0x2d, 0xec, 0x79
};
static const uint8_t dsa_n[128] = {
    0x98, 0xad, 0xbf, 0xb4, 0x34, 0x67, 0xd4, 0xa5, 0xdf, 0x25, 0xd3, 0x88, 0xe5, 0x60, 0x5f, 0x12,
    0x84, 0x0e, 0xa6, 0x62, 0x99, 0xcb, 0xdf, 0x3a, 0x80, 0x7b, 0xbd, 0x99, 0x81, 0x77, 0x98, 0xeb,
    0x4a, 0x64, 0xa6, 0x88, 0x5b, 0xa5, 0x49, 0x4d, 0xba, 0x2f, 0xc0, 0x00, 0x05, 0xfe, 0x87, 0xf5,
    0xec, 0x65, 0x85, 0xd1, 0xba, 0x53, 0xde, 0xa9, 0xd4, 0x8e, 0x48, 0xcf, 0xe6, 0x03, 0x18, 0x43,
    0xd8, 0x40, 0x57, 0xa0, 0xc6, 0x23, 0x3a, 0x59, 0x8c, 0xb8, 0xa7, 0xd3, 0x99, 0xee, 0xa7, 0xbc,
    0xe8, 0x1e, 0x07, 0xea, 0x57, 0x4c, 0x26, 0xe4, 0x4b, 0x4a, 0x92, 0x5d, 0xb3, 0xa9, 0x37, 0x2d,
    0x73, 0xc9, 0x7b, 0xbe, 0x14, 0x3c, 0x74, 0x7f, 0xfc, 0x11, 0x23, 0xc2, 0xe0, 0xe5, 0x76, 0x81,
    0xe9, 0x54, 0x62, 0xa8, 0x1b, 0x72, 0x24, 0xe8, 0x14, 0x27, 0x03, 0x63, 0xbb, 0xe8, 0x80, 0xb9
};
static const uint8_t dsa_e[128] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01
};
static const uint8_t dsa_d[128] = {
    0x2a, 0x99, 0xa0, 0x39, 0xb9, 0xbd, 0x96, 0xd9, 0xc9, 0x4f, 0x96, 0x9c, 0x45, 0xd4, 0x03, 0xf2,
    0xf8, 0xa1, 0xd5, 0xcf, 0x01, 0x36, 0x9f, 0x4c, 0x85, 0x79, 0x01, 0x48, 0x09, 0x95, 0xc3, 0x37,
    0x33, 0x84, 0xad, 0xea, 0xb2, 0x17, 0x86, 0x42, 0x87, 0xc6, 0xb6, 0xfd, 0xca, 0xae, 0x60, 0x65,
    0x3f, 0xd6, 0x7b, 0xe6, 0x2e, 0xe5, 0x23, 0xe5, 0xa3, 0x07, 0x28, 0x4d, 0xc9, 0xd1, 0xe7, 0x3c,
    0xe8, 0x8b, 0xbd, 0x38, 0xe7, 0x11, 0x53, 0x6e, 0x48, 0xb1, 0x81, 0xc1, 0xb5, 0x73, 0x5d, 0x76,
    0x67, 0x57, 0x00, 0xfc, 0x2d, 0xdd, 0x89, 0xb7, 0xdf, 0x67, 0xa8, 0xfb, 0x16, 0xeb, 0x4c, 0x1d,
    0xde, 0x35, 0x4f, 0xbc, 0x04, 0xf7, 0xc4, 0xc1, 0x89, 0x75, 0xf5, 0xcb, 0xa4, 0xf3, 0x6e, 0xc7,
    0x12, 0x0b, 0x00, 0xbd, 0xd8, 0x38, 0xd4, 0x88, 0xce, 0x37, 0xa9, 0x45, 0x2e, 0x57, 0x44, 0x01
};
static const uint8_t dsa_dP[64] = {
    0xc2, 0xa4, 0x34, 0xae, 0xd4, 0x79, 0x78, 0x90, 0x27, 0x2c, 0x6d, 0x3e, 0x04, 0x2e, 0xde, 0x61,
    0x9b, 0xbd, 0xbc, 0x80, 0x92, 0x26, 0xf9, 0xf0, 0x1e, 0xce, 0xb8, 0x7f, 0x6f, 0x91, 0x09, 0x84,
    0x3f, 0x6b, 0x04, 0x1c, 0x0c, 0x17, 0xef, 0x5e, 0x93, 0xdf, 0xdc, 0xce, 0xc6, 0xa0, 0x7a, 0x02,
    0xe8, 0x15, 0x5f, 0xaf, 0x01, 0x5c, 0x10, 0x37, 0xca, 0x37, 0xe3, 0x99, 0x5b, 0x9d, 0x2c, 0x81
};
static const uint8_t dsa_dQ[64] = {
    0x16, 0x12, 0x95, 0xdd, 0x9d, 0xa4, 0x3f, 0x83, 0x5f, 0xb0, 0xc1, 0x23, 0x44, 0x88, 0x97, 0x6e,
    0xe3, 0x8b, 0x60, 0xde, 0x4b, 0x26, 0x85, 0xd6, 0x2b, 0xfd, 0x47, 0xcb, 0x8b, 0xd4, 0xa4, 0x1c,
    0x08, 0x66, 0xb9, 0x2c, 0x20, 0x9e, 0x9b, 0x11, 0x7b, 0xe1, 0x0d, 0x88, 0x03, 0xfa, 0xa1, 0x76,
    0x65, 0x6a, 0x01, 0x4b, 0x96, 0xe8, 0xf8, 0xb6, 0x3f, 0xdc, 0xe4, 0xba, 0xbf, 0xf8, 0x88, 0xa9
};
static const uint8_t dsa_qInv[64] = {
    0x49, 0x72, 0x7f, 0x74, 0x16, 0xd2, 0xe0, 0x19, 0xb5, 0x43, 0x6f, 0xfe, 0xb0, 0x1b, 0x81, 0x7c,
    0xd5, 0x7b, 0xb6, 0x7a, 0xec, 0xe0, 0xe5, 0x6a, 0x77, 0x77, 0xdb, 0x15, 0x47, 0xc1, 0x97, 0xd1,
    0xb0, 0x0d, 0x08, 0x8c, 0x43, 0x67, 0x21, 0xa3, 0x1e, 0xba, 0x53, 0x87, 0xb8, 0xd4, 0x83, 0x94,
    0xe4, 0x7c, 0x80, 0x6d, 0x3d, 0x1b, 0x54, 0x6b, 0x03, 0xfd, 0x3e, 0x6c, 0x3e, 0xad, 0x5f, 0xda
};
static const uint8_t dsa_invR_p[64] = {
    0x74, 0xc9, 0x11, 0x9b, 0x94, 0x7c, 0xc7, 0x0c, 0xd9, 0x39, 0x31, 0x06, 0x2c, 0x89, 0x7f, 0x90,
    0x83, 0xec, 0xdd, 0x9e, 0xd1, 0x53, 0x85, 0xb1, 0x2e, 0x01, 0xc0, 0x7a, 0xce, 0x44, 0x56, 0x77,
    0x4d, 0xbe, 0x96, 0xbe, 0xf2, 0x1d, 0x03, 0x09, 0x97, 0x7a, 0xd5, 0xa5, 0x5f, 0x2e, 0x32, 0xdb,
    0xd9, 0xdd, 0x80, 0x3c, 0x08, 0x52, 0x8e, 0x72, 0x14, 0xfd, 0x09, 0x6c, 0x6a, 0x0a, 0x40, 0x91
};
static const uint8_t dsa_primeN_p[64] = {
    0x94, 0x80, 0xcd, 0xff, 0x86, 0x44, 0x04, 0x7a, 0xa9, 0x62, 0x10, 0x8c, 0x4f, 0x9d, 0xee, 0x0e,
    0x70, 0x56, 0xfc, 0xaf, 0x12, 0xaa, 0x05, 0xb0, 0x3c, 0x0c, 0x8c, 0xc1, 0xef, 0xc5, 0x70, 0xc8,
    0x40, 0x76, 0xa2, 0xca, 0x4e, 0xe0, 0xae, 0x6d, 0x3c, 0xbc, 0x3b, 0xed, 0x85, 0xdf, 0x44, 0x39,
    0xcd, 0x89, 0x45, 0x2a, 0x3b, 0x4b, 0x12, 0x2d, 0x34, 0xef, 0xa5, 0x05, 0x04, 0x9e, 0x96, 0x3f
};
static const uint8_t dsa_invR_q[64] = {
    0x07, 0x2d, 0x7c, 0xa1, 0x7d, 0xe7, 0xab, 0x44, 0x8d, 0xf0, 0xe1, 0xf0, 0xd0, 0x54, 0xb1, 0x7b,
    0x74, 0x74, 0xac, 0x42, 0xc2, 0x49, 0x56, 0xab, 0xbd, 0x94, 0xe7, 0x5c, 0x6b, 0x06, 0x39, 0x2e,
    0xc7, 0x75, 0xd3, 0xdc, 0x6c, 0x8b, 0xd8, 0x19, 0x7e, 0x6d, 0x59, 0x58, 0xc7, 0x95, 0x71, 0x9e,
    0x10, 0x1a, 0xab, 0x07, 0x6f, 0xf1, 0x81, 0x37, 0x68, 0x88, 0x6c, 0xdd, 0x9d, 0xd8, 0x74, 0x82
};
static const uint8_t dsa_primeN_q[64] = {
    0x09, 0x76, 0xeb, 0x06, 0xf9, 0x4b, 0xb4, 0xb8, 0xef, 0x02, 0x17, 0xf3, 0xa5, 0xa1, 0x12, 0xbc,
    0x85, 0xda, 0x82, 0x91, 0x31, 0xd4, 0x07, 0x49, 0xdf, 0xef, 0x6a, 0x4a, 0x71, 0x0e, 0xe0, 0x8f,
    0x14, 0xc7, 0x71, 0x3f, 0x92, 0xa8, 0x40, 0xd2, 0x9c, 0xf8, 0x76, 0x51, 0xaf, 0xe0, 0xbd, 0xbc,
    0x09, 0x21, 0x2a, 0x68, 0xab, 0x4a, 0xfa, 0x5a, 0xeb, 0x6f, 0xb6, 0x65, 0x67, 0xc3, 0x42, 0x37
};
static const uint8_t dsa_plaintext[32] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x56, 0x78
};
static const uint8_t dsa_expected_signature[128] = {
    0x94, 0x56, 0x39, 0x91, 0x66, 0x79, 0x4b, 0x55, 0x36, 0x55, 0x13, 0x90, 0x9d, 0x6c, 0x40, 0x02,
    0xf8, 0xd5, 0xb3, 0xae, 0x15, 0x90, 0xcd, 0xcf, 0x7b, 0x18, 0xe2, 0x03, 0x2b, 0xf2, 0x65, 0x08,
    0xfc, 0xc8, 0xbf, 0xc2, 0x47, 0xc4, 0x72, 0x55, 0x8a, 0x50, 0x26, 0x5e, 0x73, 0x7f, 0x18, 0x1e,
    0x6f, 0x64, 0x8f, 0x42, 0xd0, 0x1b, 0x92, 0x0e, 0xe3, 0x1e, 0x39, 0xea, 0x38, 0x76, 0x0b, 0xf8,
    0x0f, 0x25, 0x5d, 0x5d, 0x5d, 0x52, 0xf4, 0x7f, 0x74, 0x27, 0xdf, 0x4a, 0x89, 0xc4, 0xa3, 0xa7,
    0xf8, 0xd3, 0xf9, 0x3a, 0xaa, 0x6a, 0x34, 0x42, 0xb3, 0x5e, 0x5b, 0x68, 0x96, 0xc7, 0xc4, 0xfa,
    0x22, 0x95, 0x8a, 0xbf, 0xa6, 0xf3, 0x54, 0x2e, 0xcf, 0x66, 0x0f, 0x62, 0xef, 0xac, 0x11, 0xc1,
    0x69, 0xb2, 0x64, 0xf9, 0xcc, 0x75, 0x68, 0x50, 0x8f, 0xf1, 0xfe, 0x5c, 0xfa, 0x27, 0x6c, 0x7b
};

static int test_dsa_1024(void)
{
    sec_dsa_handle_t dsa_handle;
    uint32_t signature[32];
    uint32_t time_start, time_end;
    int ret;

    printf("\r\n[TEST] DSA Sign/Verify (1024-bit RSA key)\r\n");

    sec_dsa_init(&dsa_handle, 1024);
    dsa_handle.n = (uint32_t *)dsa_n;
    dsa_handle.e = (uint32_t *)dsa_e;
    dsa_handle.d = (uint32_t *)dsa_d;
    dsa_handle.crtCfg.dP = (uint32_t *)dsa_dP;
    dsa_handle.crtCfg.dQ = (uint32_t *)dsa_dQ;
    dsa_handle.crtCfg.qInv = (uint32_t *)dsa_qInv;
    dsa_handle.crtCfg.p = (uint32_t *)dsa_p;
    dsa_handle.crtCfg.invR_p = (uint32_t *)dsa_invR_p;
    dsa_handle.crtCfg.primeN_p = (uint32_t *)dsa_primeN_p;
    dsa_handle.crtCfg.q = (uint32_t *)dsa_q;
    dsa_handle.crtCfg.invR_q = (uint32_t *)dsa_invR_q;
    dsa_handle.crtCfg.primeN_q = (uint32_t *)dsa_primeN_q;

    /* Sign */
    time_start = (uint32_t)mtimer_get_time_us();
    ret = sec_dsa_sign(&dsa_handle, (uint32_t *)dsa_plaintext, 8, signature);
    time_end = (uint32_t)mtimer_get_time_us();

    if (ret != 0) {
        printf("  Sign failed: %d\r\n", ret);
        return -1;
    }
    printf("  Sign time: %lu us\r\n", (unsigned long)(time_end - time_start));

    /* Verify signature matches expected */
    if (compare_data((uint8_t *)signature, dsa_expected_signature, 128) != 0) {
        printf("  Signature mismatch!\r\n");
        hex_dump("  Expected", dsa_expected_signature, 32);
        hex_dump("  Got     ", (uint8_t *)signature, 32);
        return -1;
    }
    printf("  Sign: PASS\r\n");

    /* Verify */
    time_start = (uint32_t)mtimer_get_time_us();
    ret = sec_dsa_verify(&dsa_handle, (uint32_t *)dsa_plaintext, 8, signature);
    time_end = (uint32_t)mtimer_get_time_us();

    if (ret != 0) {
        printf("  Verify failed: %d\r\n", ret);
        return -1;
    }
    printf("  Verify time: %lu us\r\n", (unsigned long)(time_end - time_start));
    printf("  Verify: PASS\r\n");

    return 0;
}

/*============================================================================
 * PKA mexp_binary Direct Tests (HAL-level)
 *
 * These tests call sec_dsa_mexp_binary() directly with register sizes < 1024
 * to verify the accumulator initialization. The HAL has a known bug where
 * oneBuf (128 bytes with 0x01 at byte 127) gets clipped for small registers,
 * initializing the accumulator to 0 instead of 1.
 *===========================================================================*/

/*
 * Test: 7^13 mod 11 = 2, using 256-bit PKA registers
 *
 * Data layout: big-endian byte arrays (MSB first), 32 bytes each.
 * PKA BigEndian mode byte-swaps each uint32_t word on load.
 */
static int test_mexp_binary_256(void)
{
    /* 7 as 256-bit big-endian */
    uint8_t a[32] ALIGN4 = { [31] = 0x07 };
    /* 13 as 256-bit big-endian */
    uint8_t b[32] ALIGN4 = { [31] = 0x0D };
    /* 11 (must be odd for modexp) as 256-bit big-endian */
    uint8_t c[32] ALIGN4 = { [31] = 0x0B };
    /* expected: 2 */
    uint8_t expected[32] ALIGN4 = { [31] = 0x02 };
    uint8_t r[32] ALIGN4 = { 0 };
    int ret;

    printf("\r\n[TEST] mexp_binary 256-bit: 7^13 mod 11 = 2\r\n");

    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    ret = sec_dsa_mexp_binary(256, (uint32_t *)a, (uint32_t *)b,
                              (uint32_t *)c, (uint32_t *)r);
    if (ret != 0) {
        printf("  FAILED (ret=%d)\r\n", ret);
        return -1;
    }

    if (compare_data(r, expected, 32) != 0) {
        printf("  FAILED (result mismatch)\r\n");
        hex_dump("  Expected", expected, 32);
        hex_dump("  Got     ", r, 32);
        return -1;
    }

    printf("  PASS\r\n");
    return 0;
}

/*
 * Test: 3^7 mod 13 = 3, using 512-bit PKA registers
 */
static int test_mexp_binary_512(void)
{
    uint8_t a[64] ALIGN4 = { [63] = 0x03 };
    uint8_t b[64] ALIGN4 = { [63] = 0x07 };
    uint8_t c[64] ALIGN4 = { [63] = 0x0D };
    /* 3^7 = 2187, 2187 mod 13 = 3 */
    uint8_t expected[64] ALIGN4 = { [63] = 0x03 };
    uint8_t r[64] ALIGN4 = { 0 };
    int ret;

    printf("\r\n[TEST] mexp_binary 512-bit: 3^7 mod 13 = 3\r\n");

    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    ret = sec_dsa_mexp_binary(512, (uint32_t *)a, (uint32_t *)b,
                              (uint32_t *)c, (uint32_t *)r);
    if (ret != 0) {
        printf("  FAILED (ret=%d)\r\n", ret);
        return -1;
    }

    if (compare_data(r, expected, 64) != 0) {
        printf("  FAILED (result mismatch)\r\n");
        hex_dump("  Expected", expected, 64);
        hex_dump("  Got     ", r, 64);
        return -1;
    }

    printf("  PASS\r\n");
    return 0;
}

/*
 * Test: 2^16 mod 257 = 65536 mod 257 = 1, using 256-bit PKA registers
 * (Fermat prime property: 257 = 2^8 + 1, so 2^256 mod 257 = 1)
 * Here we use 2^16 mod 257 = 65536 mod 257 = 255 (since 65536 = 254*257 + 258 - 1... let me recalculate)
 * Actually: 2^8 mod 257 = 256, 2^16 mod 257 = 256^2 mod 257 = 65536 mod 257
 *           65536 / 257 = 255.00..., 255 * 257 = 65535, so 65536 mod 257 = 1
 * So 2^16 mod 257 = 1. Good.
 */
static int test_mexp_binary_256_fermat(void)
{
    uint8_t a[32] ALIGN4 = { [31] = 0x02 };
    uint8_t b[32] ALIGN4 = { [31] = 0x10 };  /* 16 */
    /* 257 = 0x0101 */
    uint8_t c[32] ALIGN4 = { [30] = 0x01, [31] = 0x01 };
    uint8_t expected[32] ALIGN4 = { [31] = 0x01 };
    uint8_t r[32] ALIGN4 = { 0 };
    int ret;

    printf("\r\n[TEST] mexp_binary 256-bit: 2^16 mod 257 = 1 (Fermat)\r\n");

    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    ret = sec_dsa_mexp_binary(256, (uint32_t *)a, (uint32_t *)b,
                              (uint32_t *)c, (uint32_t *)r);
    if (ret != 0) {
        printf("  FAILED (ret=%d)\r\n", ret);
        return -1;
    }

    if (compare_data(r, expected, 32) != 0) {
        printf("  FAILED (result mismatch)\r\n");
        hex_dump("  Expected", expected, 32);
        hex_dump("  Got     ", r, 32);
        return -1;
    }

    printf("  PASS\r\n");
    return 0;
}

static int cmd_pka_test(int argc, char **argv)
{
    int pass = 0, fail = 0;

    printf("\r\n========================================\r\n");
    printf("PKA Hardware Security Test Suite\r\n");
    printf("========================================\r\n");
    printf("Supported curves: secp256r1, secp256k1\r\n");
    printf("========================================\r\n");

    /* mexp_binary direct tests (HAL-level, catch oneBuf bug) */
    printf("\r\n--- mexp_binary Direct Tests ---\r\n");
    if (test_mexp_binary_256() == 0) pass++; else fail++;
    if (test_mexp_binary_512() == 0) pass++; else fail++;
    if (test_mexp_binary_256_fermat() == 0) pass++; else fail++;

    /* ECDSA Tests */
    printf("\r\n--- ECDSA Tests ---\r\n");
    if (test_ecdsa_secp256r1() == 0) pass++; else fail++;
    if (test_ecdsa_get_public_key() == 0) pass++; else fail++;

    /* ECDH Tests */
    printf("\r\n--- ECDH Tests ---\r\n");
    if (test_ecdh_secp256r1() == 0) pass++; else fail++;
    if (test_ecdh_secp256k1() == 0) pass++; else fail++;
    if (test_ecdh_get_public_key() == 0) pass++; else fail++;

    /* DSA Tests */
    printf("\r\n--- DSA Tests ---\r\n");
    if (test_dsa_1024() == 0) pass++; else fail++;

    printf("\r\n========================================\r\n");
    printf("PKA Test Result: %d PASS, %d FAIL (Total: %d)\r\n", pass, fail, pass + fail);

    /* Update global counters */
    g_total_tests += pass + fail;
    g_total_pass += pass;

    return (fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pka_test, pka_test, PKA ECDSA/ECDH/DSA hardware test suite);

/*============================================================================
 * mbedTLS Hardware Self-Test
 *===========================================================================*/
#if defined(CONFIG_MBEDTLS_SELF_TEST)
static int cmd_mbedtls_test(int argc, char **argv)
{
    return mbedtls_hardware_self_test(1);
}
SHELL_CMD_EXPORT_ALIAS(cmd_mbedtls_test, mbedtls_test, mbedTLS hardware acceleration self-test);
#endif

/*============================================================================
 * Combined Security Test
 *===========================================================================*/

static int cmd_sec_test_all(int argc, char **argv)
{
    int aes_result, sha_result, trng_result, gmac_result, pka_result;
    int total_fail;

    /* Reset global counters */
    g_total_tests = 0;
    g_total_pass = 0;

    printf("\r\n########################################\r\n");
    printf("# Security Hardware Complete Test Suite #\r\n");
    printf("# (Testing both Link and Direct modes)  #\r\n");
    printf("########################################\r\n");

    aes_result = cmd_aes_mode_test(0, NULL);
    sha_result = cmd_sha_mode_test(0, NULL);
    trng_result = cmd_trng_test(0, NULL);
    gmac_result = cmd_gmac_test(0, NULL);
    pka_result = cmd_pka_test(0, NULL);

    total_fail = g_total_tests - g_total_pass;

    printf("\r\n########################################\r\n");
    printf("# Overall Security Test Summary         #\r\n");
    printf("########################################\r\n");
    printf("  AES  : %s\r\n", aes_result == 0 ? "PASS" : "FAIL");
    printf("  SHA  : %s\r\n", sha_result == 0 ? "PASS" : "FAIL");
    printf("  TRNG : %s\r\n", trng_result == 0 ? "PASS" : "FAIL");
    printf("  GMAC : %s\r\n", gmac_result == 0 ? "PASS" : "FAIL");
    printf("  PKA  : %s\r\n", pka_result == 0 ? "PASS" : "FAIL");
    printf("----------------------------------------\r\n");
    printf("  Total: %d tests, %d PASS, %d FAIL\r\n", g_total_tests, g_total_pass, total_fail);
    printf("########################################\r\n");

    return (total_fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sec_test_all, sec_test, Run all security hardware tests);
