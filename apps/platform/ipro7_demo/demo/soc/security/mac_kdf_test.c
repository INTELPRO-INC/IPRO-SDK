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
 * @file mac_kdf_test.c
 * @brief MAC and KDF certification self-tests (HMAC / CMAC / GMAC / HKDF)
 *
 * The IPRO7 security engine provides SHA, AES, GHASH, TRNG and PKA hardware
 * blocks. HMAC, CMAC and HKDF are *modes* layered on those primitives, not
 * separate engines, so they are built here directly on the sec_eng HAL:
 *
 *   HMAC  : two SHA-2 passes over the ipad/opad blocks   (HW SHA engine)
 *   CMAC  : subkey generation + CBC-MAC over AES-ECB     (HW AES engine)
 *   GMAC  : H = AES_K(0), GHASH(A || len), XOR AES_K(J0) (HW AES + HW GHASH)
 *   HKDF  : HMAC-extract then HMAC-expand                (HW SHA engine)
 *
 * Every case prints INPUT / EXPECT / OUTPUT / RESULT so the console transcript
 * is directly usable as a self-test report.
 *
 * Test vectors:
 *   HMAC  - RFC 4231
 *   CMAC  - RFC 4493 (AES-128) and NIST SP 800-38B (AES-192/256)
 *   GMAC  - NIST GCM spec test case 1; TC4/TC16 key/IV/AAD with Plen = 0
 *   HKDF  - RFC 5869
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"
#include <FreeRTOS.h>
#include <hal_sec_aes.h>
#include <hal_sec_hash.h>
#include <hal_sec_gmac.h>
#include <hal_common.h>
#include <hal_mtimer.h>
#include <compiler/common.h>

/*============================================================================
 * DMA-visible working buffers
 *
 * The SHA/AES/GHASH engines fetch operands by address, so every buffer handed
 * to the HAL lives in the PSRAM section with 32-byte alignment, matching the
 * convention used by sec_test.c.
 *===========================================================================*/

#define MK_MSG_MAX   256   /* longest RFC 4231 message is 152 bytes */
#define MK_BLK_MAX   128   /* SHA-512 block size */
#define MK_DIG_MAX   64    /* SHA-512 digest size */
#define MK_OKM_MAX   96    /* longest RFC 5869 OKM is 82 bytes */

static uint8_t ATTR_PSRAM_SECTION mk_msg_buf[MK_MSG_MAX] __attribute__((aligned(32)));
static uint8_t ATTR_PSRAM_SECTION mk_pad_buf[MK_BLK_MAX] __attribute__((aligned(32)));
static uint8_t ATTR_PSRAM_SECTION mk_dig_buf[MK_DIG_MAX] __attribute__((aligned(32)));
static uint8_t ATTR_PSRAM_SECTION mk_aes_in[MK_BLK_MAX] __attribute__((aligned(32)));
static uint8_t ATTR_PSRAM_SECTION mk_aes_out[MK_BLK_MAX] __attribute__((aligned(32)));

static sec_hash_handle_t mk_hash_handle ATTR_PSRAM_SECTION __attribute__((aligned(32)));
static sec_aes_handle_t mk_aes_handle ATTR_PSRAM_SECTION __attribute__((aligned(32)));
static sec_gmac_handle_t mk_gmac_handle ATTR_PSRAM_SECTION __attribute__((aligned(32)));

/*============================================================================
 * Logging helpers — one line per field, wrapped at 32 bytes
 *===========================================================================*/

static void mk_log_hex(const char *label, const uint8_t *data, uint32_t len)
{
    uint32_t i;

    printf("  %-14s (%3luB): ", label, (unsigned long)len);
    if (len == 0) {
        printf("(empty)\r\n");
        return;
    }
    for (i = 0; i < len; i++) {
        if (i != 0 && (i % 32) == 0) {
            printf("\r\n  %-14s          ", "");
        }
        printf("%02x", data[i]);
    }
    printf("\r\n");
}

static int mk_cmp(const uint8_t *a, const uint8_t *b, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++) {
        if (a[i] != b[i]) {
            return -1;
        }
    }
    return 0;
}

/*============================================================================
 * SHA-2 helpers on the hardware hash engine
 *===========================================================================*/

static uint32_t mk_hash_block_size(uint8_t type)
{
    return (type == SEC_HASH_SHA384 || type == SEC_HASH_SHA512) ? 128 : 64;
}

static uint32_t mk_hash_digest_size(uint8_t type)
{
    switch (type) {
        case SEC_HASH_SHA224: return 28;
        case SEC_HASH_SHA256: return 32;
        case SEC_HASH_SHA384: return 48;
        case SEC_HASH_SHA512: return 64;
        default:              return 0;
    }
}

static const char *mk_hash_name(uint8_t type)
{
    switch (type) {
        case SEC_HASH_SHA224: return "SHA-224";
        case SEC_HASH_SHA256: return "SHA-256";
        case SEC_HASH_SHA384: return "SHA-384";
        case SEC_HASH_SHA512: return "SHA-512";
        default:              return "SHA-?";
    }
}

/* Single-shot hash of a buffer that is already in mk_msg_buf */
static int mk_hash_buf(uint8_t type, uint32_t len, uint8_t *out)
{
    int ret;

    sec_hash_init_ex(&mk_hash_handle, SEC_HASH_MODE_DIRECT, type);
    ret = sec_hash_update(&mk_hash_handle, mk_msg_buf, len);
    if (ret != 0) {
        sec_hash_deinit(&mk_hash_handle);
        return -1;
    }
    ret = sec_hash_finish(&mk_hash_handle, mk_dig_buf);
    sec_hash_deinit(&mk_hash_handle);
    if (ret < 0) {
        return -1;
    }
    memcpy(out, mk_dig_buf, mk_hash_digest_size(type));
    return 0;
}

/*
 * HMAC (FIPS 198-1) on the hardware SHA engine.
 *
 * K0 = H(key) when key is longer than the block, otherwise the key itself,
 * both zero-padded to the block size. The inner and outer hashes each feed
 * the pad block first, then the message, so no oversized staging buffer is
 * needed for the concatenation.
 */
static int mk_hmac(uint8_t type, const uint8_t *key, uint32_t keylen,
                   const uint8_t *msg, uint32_t msglen, uint8_t *out)
{
    uint32_t blk = mk_hash_block_size(type);
    uint32_t dig = mk_hash_digest_size(type);
    uint8_t k0[MK_BLK_MAX];
    uint32_t i;
    int ret;

    if (dig == 0 || msglen > MK_MSG_MAX) {
        return -1;
    }

    memset(k0, 0, blk);
    if (keylen > blk) {
        if (keylen > MK_MSG_MAX) {
            return -1;
        }
        memcpy(mk_msg_buf, key, keylen);
        if (mk_hash_buf(type, keylen, k0) != 0) {
            return -1;
        }
    } else {
        memcpy(k0, key, keylen);
    }

    /* Inner hash: H((K0 ^ ipad) || msg) */
    for (i = 0; i < blk; i++) {
        mk_pad_buf[i] = k0[i] ^ 0x36;
    }
    if (msglen != 0) {
        memcpy(mk_msg_buf, msg, msglen);
    }

    sec_hash_init_ex(&mk_hash_handle, SEC_HASH_MODE_DIRECT, type);
    ret = sec_hash_update(&mk_hash_handle, mk_pad_buf, blk);
    if (ret == 0 && msglen != 0) {
        ret = sec_hash_update(&mk_hash_handle, mk_msg_buf, msglen);
    }
    if (ret == 0) {
        ret = sec_hash_finish(&mk_hash_handle, mk_dig_buf);
        ret = (ret < 0) ? -1 : 0;
    }
    sec_hash_deinit(&mk_hash_handle);
    if (ret != 0) {
        return -1;
    }

    /* Outer hash: H((K0 ^ opad) || inner) — inner digest is already in
     * mk_dig_buf, so stage it in mk_msg_buf before reusing mk_dig_buf. */
    memcpy(mk_msg_buf, mk_dig_buf, dig);
    for (i = 0; i < blk; i++) {
        mk_pad_buf[i] = k0[i] ^ 0x5c;
    }

    sec_hash_init_ex(&mk_hash_handle, SEC_HASH_MODE_DIRECT, type);
    ret = sec_hash_update(&mk_hash_handle, mk_pad_buf, blk);
    if (ret == 0) {
        ret = sec_hash_update(&mk_hash_handle, mk_msg_buf, dig);
    }
    if (ret == 0) {
        ret = sec_hash_finish(&mk_hash_handle, mk_dig_buf);
        ret = (ret < 0) ? -1 : 0;
    }
    sec_hash_deinit(&mk_hash_handle);
    if (ret != 0) {
        return -1;
    }

    memcpy(out, mk_dig_buf, dig);
    return 0;
}

/*============================================================================
 * AES-ECB single block on the hardware AES engine
 *===========================================================================*/

static sec_aes_key_type mk_aes_key_type(uint32_t keylen)
{
    if (keylen == 24) {
        return SEC_AES_KEY_192;
    }
    if (keylen == 32) {
        return SEC_AES_KEY_256;
    }
    return SEC_AES_KEY_128;
}

static int mk_aes_ecb_block(const uint8_t *key, uint32_t keylen,
                            const uint8_t *in, uint8_t *out)
{
    int ret;

    memcpy(mk_aes_in, in, 16);

    sec_aes_init_ex(&mk_aes_handle, SEC_MODE_LINK, SEC_AES_ECB,
                    mk_aes_key_type(keylen));
    sec_aes_setkey(&mk_aes_handle, key, (uint8_t)keylen, NULL,
                   SEC_AES_DIR_ENCRYPT);
    ret = sec_aes_encrypt(&mk_aes_handle, mk_aes_in, 16, 0, mk_aes_out);
    sec_aes_deinit(&mk_aes_handle);

    if (ret != 0) {
        return -1;
    }
    memcpy(out, mk_aes_out, 16);
    return 0;
}

/*============================================================================
 * CMAC (NIST SP 800-38B) on the hardware AES engine
 *===========================================================================*/

/* Left shift by one bit over a 128-bit big-endian block, with the Rb = 0x87
 * reduction when the shifted-out MSB is set. */
static void mk_cmac_dbl(const uint8_t in[16], uint8_t out[16])
{
    uint8_t carry = (uint8_t)(in[0] >> 7);
    int i;

    for (i = 0; i < 15; i++) {
        out[i] = (uint8_t)((in[i] << 1) | (in[i + 1] >> 7));
    }
    out[15] = (uint8_t)(in[15] << 1);
    if (carry) {
        out[15] ^= 0x87;
    }
}

static int mk_cmac(const uint8_t *key, uint32_t keylen,
                   const uint8_t *msg, uint32_t msglen, uint8_t tag[16])
{
    uint8_t l[16], k1[16], k2[16];
    uint8_t x[16], block[16];
    uint32_t n, i, j;
    uint32_t full;

    memset(x, 0, 16);
    memset(block, 0, 16);

    /* Subkey generation: L = AES_K(0), K1 = dbl(L), K2 = dbl(K1) */
    if (mk_aes_ecb_block(key, keylen, x, l) != 0) {
        return -1;
    }
    mk_cmac_dbl(l, k1);
    mk_cmac_dbl(k1, k2);

    n = (msglen + 15) / 16;
    full = (msglen != 0 && (msglen % 16) == 0);
    if (n == 0) {
        n = 1;
    }

    /* CBC-MAC over the first n-1 blocks */
    for (i = 0; i + 1 < n; i++) {
        for (j = 0; j < 16; j++) {
            block[j] = x[j] ^ msg[i * 16 + j];
        }
        if (mk_aes_ecb_block(key, keylen, block, x) != 0) {
            return -1;
        }
    }

    /* Last block: complete blocks take K1, padded blocks take K2 */
    if (full) {
        for (j = 0; j < 16; j++) {
            block[j] = msg[(n - 1) * 16 + j] ^ k1[j];
        }
    } else {
        uint32_t rem = msglen - (n - 1) * 16;
        memset(block, 0, 16);
        for (j = 0; j < rem; j++) {
            block[j] = msg[(n - 1) * 16 + j];
        }
        block[rem] = 0x80;
        for (j = 0; j < 16; j++) {
            block[j] ^= k2[j];
        }
    }
    for (j = 0; j < 16; j++) {
        block[j] ^= x[j];
    }
    return mk_aes_ecb_block(key, keylen, block, tag);
}

/*============================================================================
 * GMAC (NIST SP 800-38D) on the hardware AES + GHASH engines
 *
 * GMAC is AES-GCM with an empty plaintext: the tag authenticates the AAD only.
 * With a 96-bit IV, J0 = IV || 0^31 || 1.
 *===========================================================================*/

static int mk_gmac(const uint8_t *key, uint32_t keylen,
                   const uint8_t *iv12, const uint8_t *aad, uint32_t aadlen,
                   uint8_t *h_out, uint8_t *ej0_out,
                   uint8_t *ghash_out, uint8_t tag[16])
{
    uint8_t zero[16];
    uint8_t j0[16];
    uint8_t h[16], ej0[16], ghash[16];
    uint8_t lenblk[16];
    uint64_t aadbits = (uint64_t)aadlen * 8;
    uint32_t i;

    if (aadlen > MK_MSG_MAX) {
        return -1;
    }

    memset(zero, 0, 16);

    /* H = AES_K(0^128) */
    if (mk_aes_ecb_block(key, keylen, zero, h) != 0) {
        return -1;
    }

    /* E(K, J0) with J0 = IV || 0^31 || 1 */
    memcpy(j0, iv12, 12);
    j0[12] = 0x00;
    j0[13] = 0x00;
    j0[14] = 0x00;
    j0[15] = 0x01;
    if (mk_aes_ecb_block(key, keylen, j0, ej0) != 0) {
        return -1;
    }

    /* GHASH over the zero-padded AAD followed by the length block.
     * sec_gmac_update() zero-pads each partial call, which is exactly the
     * padding GHASH applies to the AAD. */
    memset(lenblk, 0, 16);
    for (i = 0; i < 8; i++) {
        lenblk[7 - i] = (uint8_t)(aadbits >> (8 * i));
    }

    sec_gmac_init();
    if (sec_gmac_start(&mk_gmac_handle, h) != 0) {
        return -1;
    }
    if (aadlen != 0) {
        memcpy(mk_msg_buf, aad, aadlen);
        if (sec_gmac_update(&mk_gmac_handle, mk_msg_buf, aadlen) != 0) {
            sec_gmac_deinit(&mk_gmac_handle);
            return -1;
        }
    }
    memcpy(mk_msg_buf, lenblk, 16);
    if (sec_gmac_update(&mk_gmac_handle, mk_msg_buf, 16) != 0) {
        sec_gmac_deinit(&mk_gmac_handle);
        return -1;
    }
    if (sec_gmac_finish(&mk_gmac_handle, ghash) != 0) {
        sec_gmac_deinit(&mk_gmac_handle);
        return -1;
    }
    sec_gmac_deinit(&mk_gmac_handle);

    for (i = 0; i < 16; i++) {
        tag[i] = (uint8_t)(ghash[i] ^ ej0[i]);
    }

    memcpy(h_out, h, 16);
    memcpy(ej0_out, ej0, 16);
    memcpy(ghash_out, ghash, 16);
    return 0;
}

/*============================================================================
 * HKDF (RFC 5869) on the hardware-backed HMAC
 *===========================================================================*/

static int mk_hkdf_extract(uint8_t type, const uint8_t *salt, uint32_t saltlen,
                           const uint8_t *ikm, uint32_t ikmlen, uint8_t *prk)
{
    uint8_t zero_salt[MK_DIG_MAX];
    uint32_t dig = mk_hash_digest_size(type);

    if (saltlen == 0) {
        memset(zero_salt, 0, dig);
        return mk_hmac(type, zero_salt, dig, ikm, ikmlen, prk);
    }
    return mk_hmac(type, salt, saltlen, ikm, ikmlen, prk);
}

static int mk_hkdf_expand(uint8_t type, const uint8_t *prk, uint32_t prklen,
                          const uint8_t *info, uint32_t infolen,
                          uint8_t *okm, uint32_t okmlen)
{
    uint32_t dig = mk_hash_digest_size(type);
    uint8_t t[MK_DIG_MAX];
    uint8_t input[MK_DIG_MAX + MK_MSG_MAX + 1];
    uint32_t tlen = 0;
    uint32_t done = 0;
    uint8_t counter = 1;

    if (infolen + dig + 1 > sizeof(input)) {
        return -1;
    }

    while (done < okmlen) {
        uint32_t n = 0;
        uint32_t take;

        if (tlen != 0) {
            memcpy(input, t, tlen);
            n = tlen;
        }
        if (infolen != 0) {
            memcpy(input + n, info, infolen);
            n += infolen;
        }
        input[n++] = counter;

        if (mk_hmac(type, prk, prklen, input, n, t) != 0) {
            return -1;
        }
        tlen = dig;

        take = okmlen - done;
        if (take > dig) {
            take = dig;
        }
        memcpy(okm + done, t, take);
        done += take;
        counter++;
    }
    return 0;
}

/*============================================================================
 * HMAC test vectors — RFC 4231
 *===========================================================================*/

typedef struct {
    const char *name;
    uint8_t type;
    const uint8_t *key;
    uint32_t keylen;
    const uint8_t *msg;
    uint32_t msglen;
    const uint8_t *expect;
} mk_hmac_case_t;

static const uint8_t hmac_key_tc1[20] = {
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
};
static const uint8_t hmac_msg_tc1[8] = {
    0x48, 0x69, 0x20, 0x54, 0x68, 0x65, 0x72, 0x65  /* "Hi There" */
};
static const uint8_t hmac_key_tc2[4] = { 0x4a, 0x65, 0x66, 0x65 };  /* "Jefe" */
static const uint8_t hmac_msg_tc2[28] = {
    /* "what do ya want for nothing?" */
    0x77, 0x68, 0x61, 0x74, 0x20, 0x64, 0x6f, 0x20, 0x79, 0x61,
    0x20, 0x77, 0x61, 0x6e, 0x74, 0x20, 0x66, 0x6f, 0x72, 0x20,
    0x6e, 0x6f, 0x74, 0x68, 0x69, 0x6e, 0x67, 0x3f
};
static const uint8_t hmac_key_tc3[20] = {
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
};
static const uint8_t hmac_msg_tc3[50] = {
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
    0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd
};
static const uint8_t hmac_key_tc4[25] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
    0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14,
    0x15, 0x16, 0x17, 0x18, 0x19
};
static const uint8_t hmac_msg_tc4[50] = {
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
    0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd
};
/* TC6/TC7 use a 131-byte key to exercise the "key longer than block" path */
static const uint8_t hmac_key_tc6[131] = {
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa
};
static const uint8_t hmac_msg_tc6[54] = {
    /* "Test Using Larger Than Block-Size Key - Hash Key First" */
    0x54, 0x65, 0x73, 0x74, 0x20, 0x55, 0x73, 0x69, 0x6e, 0x67,
    0x20, 0x4c, 0x61, 0x72, 0x67, 0x65, 0x72, 0x20, 0x54, 0x68,
    0x61, 0x6e, 0x20, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x2d, 0x53,
    0x69, 0x7a, 0x65, 0x20, 0x4b, 0x65, 0x79, 0x20, 0x2d, 0x20,
    0x48, 0x61, 0x73, 0x68, 0x20, 0x4b, 0x65, 0x79, 0x20, 0x46,
    0x69, 0x72, 0x73, 0x74
};
static const uint8_t hmac_msg_tc7[152] = {
    /* "This is a test using a larger than block-size key and a larger than
     *  block-size data. The key needs to be hashed before being used by the
     *  HMAC algorithm." */
    0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20,
    0x74, 0x65, 0x73, 0x74, 0x20, 0x75, 0x73, 0x69, 0x6e, 0x67,
    0x20, 0x61, 0x20, 0x6c, 0x61, 0x72, 0x67, 0x65, 0x72, 0x20,
    0x74, 0x68, 0x61, 0x6e, 0x20, 0x62, 0x6c, 0x6f, 0x63, 0x6b,
    0x2d, 0x73, 0x69, 0x7a, 0x65, 0x20, 0x6b, 0x65, 0x79, 0x20,
    0x61, 0x6e, 0x64, 0x20, 0x61, 0x20, 0x6c, 0x61, 0x72, 0x67,
    0x65, 0x72, 0x20, 0x74, 0x68, 0x61, 0x6e, 0x20, 0x62, 0x6c,
    0x6f, 0x63, 0x6b, 0x2d, 0x73, 0x69, 0x7a, 0x65, 0x20, 0x64,
    0x61, 0x74, 0x61, 0x2e, 0x20, 0x54, 0x68, 0x65, 0x20, 0x6b,
    0x65, 0x79, 0x20, 0x6e, 0x65, 0x65, 0x64, 0x73, 0x20, 0x74,
    0x6f, 0x20, 0x62, 0x65, 0x20, 0x68, 0x61, 0x73, 0x68, 0x65,
    0x64, 0x20, 0x62, 0x65, 0x66, 0x6f, 0x72, 0x65, 0x20, 0x62,
    0x65, 0x69, 0x6e, 0x67, 0x20, 0x75, 0x73, 0x65, 0x64, 0x20,
    0x62, 0x79, 0x20, 0x74, 0x68, 0x65, 0x20, 0x48, 0x4d, 0x41,
    0x43, 0x20, 0x61, 0x6c, 0x67, 0x6f, 0x72, 0x69, 0x74, 0x68,
    0x6d, 0x2e
};

static const uint8_t hmac_sha256_tc1[32] = {
    0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53, 0x5c, 0xa8,
    0xaf, 0xce, 0xaf, 0x0b, 0xf1, 0x2b, 0x88, 0x1d, 0xc2, 0x00,
    0xc9, 0x83, 0x3d, 0xa7, 0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32,
    0xcf, 0xf7
};
static const uint8_t hmac_sha256_tc2[32] = {
    0x5b, 0xdc, 0xc1, 0x46, 0xbf, 0x60, 0x75, 0x4e, 0x6a, 0x04,
    0x24, 0x26, 0x08, 0x95, 0x75, 0xc7, 0x5a, 0x00, 0x3f, 0x08,
    0x9d, 0x27, 0x39, 0x83, 0x9d, 0xec, 0x58, 0xb9, 0x64, 0xec,
    0x38, 0x43
};
static const uint8_t hmac_sha256_tc3[32] = {
    0x77, 0x3e, 0xa9, 0x1e, 0x36, 0x80, 0x0e, 0x46, 0x85, 0x4d,
    0xb8, 0xeb, 0xd0, 0x91, 0x81, 0xa7, 0x29, 0x59, 0x09, 0x8b,
    0x3e, 0xf8, 0xc1, 0x22, 0xd9, 0x63, 0x55, 0x14, 0xce, 0xd5,
    0x65, 0xfe
};
static const uint8_t hmac_sha256_tc4[32] = {
    0x82, 0x55, 0x8a, 0x38, 0x9a, 0x44, 0x3c, 0x0e, 0xa4, 0xcc,
    0x81, 0x98, 0x99, 0xf2, 0x08, 0x3a, 0x85, 0xf0, 0xfa, 0xa3,
    0xe5, 0x78, 0xf8, 0x07, 0x7a, 0x2e, 0x3f, 0xf4, 0x67, 0x29,
    0x66, 0x5b
};
static const uint8_t hmac_sha256_tc6[32] = {
    0x60, 0xe4, 0x31, 0x59, 0x1e, 0xe0, 0xb6, 0x7f, 0x0d, 0x8a,
    0x26, 0xaa, 0xcb, 0xf5, 0xb7, 0x7f, 0x8e, 0x0b, 0xc6, 0x21,
    0x37, 0x28, 0xc5, 0x14, 0x05, 0x46, 0x04, 0x0f, 0x0e, 0xe3,
    0x7f, 0x54
};
static const uint8_t hmac_sha256_tc7[32] = {
    0x9b, 0x09, 0xff, 0xa7, 0x1b, 0x94, 0x2f, 0xcb, 0x27, 0x63,
    0x5f, 0xbc, 0xd5, 0xb0, 0xe9, 0x44, 0xbf, 0xdc, 0x63, 0x64,
    0x4f, 0x07, 0x13, 0x93, 0x8a, 0x7f, 0x51, 0x53, 0x5c, 0x3a,
    0x35, 0xe2
};
static const uint8_t hmac_sha224_tc1[28] = {
    0x89, 0x6f, 0xb1, 0x12, 0x8a, 0xbb, 0xdf, 0x19, 0x68, 0x32,
    0x10, 0x7c, 0xd4, 0x9d, 0xf3, 0x3f, 0x47, 0xb4, 0xb1, 0x16,
    0x99, 0x12, 0xba, 0x4f, 0x53, 0x68, 0x4b, 0x22
};
static const uint8_t hmac_sha384_tc1[48] = {
    0xaf, 0xd0, 0x39, 0x44, 0xd8, 0x48, 0x95, 0x62, 0x6b, 0x08,
    0x25, 0xf4, 0xab, 0x46, 0x90, 0x7f, 0x15, 0xf9, 0xda, 0xdb,
    0xe4, 0x10, 0x1e, 0xc6, 0x82, 0xaa, 0x03, 0x4c, 0x7c, 0xeb,
    0xc5, 0x9c, 0xfa, 0xea, 0x9e, 0xa9, 0x07, 0x6e, 0xde, 0x7f,
    0x4a, 0xf1, 0x52, 0xe8, 0xb2, 0xfa, 0x9c, 0xb6
};
static const uint8_t hmac_sha512_tc1[64] = {
    0x87, 0xaa, 0x7c, 0xde, 0xa5, 0xef, 0x61, 0x9d, 0x4f, 0xf0,
    0xb4, 0x24, 0x1a, 0x1d, 0x6c, 0xb0, 0x23, 0x79, 0xf4, 0xe2,
    0xce, 0x4e, 0xc2, 0x78, 0x7a, 0xd0, 0xb3, 0x05, 0x45, 0xe1,
    0x7c, 0xde, 0xda, 0xa8, 0x33, 0xb7, 0xd6, 0xb8, 0xa7, 0x02,
    0x03, 0x8b, 0x27, 0x4e, 0xae, 0xa3, 0xf4, 0xe4, 0xbe, 0x9d,
    0x91, 0x4e, 0xeb, 0x61, 0xf1, 0x70, 0x2e, 0x69, 0x6c, 0x20,
    0x3a, 0x12, 0x68, 0x54
};

static const mk_hmac_case_t hmac_cases[] = {
    { "RFC4231 TC1 HMAC-SHA-256", SEC_HASH_SHA256, hmac_key_tc1, 20, hmac_msg_tc1, 8,   hmac_sha256_tc1 },
    { "RFC4231 TC2 HMAC-SHA-256", SEC_HASH_SHA256, hmac_key_tc2, 4,  hmac_msg_tc2, 28,  hmac_sha256_tc2 },
    { "RFC4231 TC3 HMAC-SHA-256", SEC_HASH_SHA256, hmac_key_tc3, 20, hmac_msg_tc3, 50,  hmac_sha256_tc3 },
    { "RFC4231 TC4 HMAC-SHA-256", SEC_HASH_SHA256, hmac_key_tc4, 25, hmac_msg_tc4, 50,  hmac_sha256_tc4 },
    { "RFC4231 TC6 HMAC-SHA-256", SEC_HASH_SHA256, hmac_key_tc6, 131, hmac_msg_tc6, 54, hmac_sha256_tc6 },
    { "RFC4231 TC7 HMAC-SHA-256", SEC_HASH_SHA256, hmac_key_tc6, 131, hmac_msg_tc7, 152, hmac_sha256_tc7 },
    { "RFC4231 TC1 HMAC-SHA-224", SEC_HASH_SHA224, hmac_key_tc1, 20, hmac_msg_tc1, 8,   hmac_sha224_tc1 },
    { "RFC4231 TC1 HMAC-SHA-384", SEC_HASH_SHA384, hmac_key_tc1, 20, hmac_msg_tc1, 8,   hmac_sha384_tc1 },
    { "RFC4231 TC1 HMAC-SHA-512", SEC_HASH_SHA512, hmac_key_tc1, 20, hmac_msg_tc1, 8,   hmac_sha512_tc1 },
};

static int cmd_hmac_test(int argc, char **argv)
{
    uint8_t out[MK_DIG_MAX];
    uint32_t i;
    int pass = 0, fail = 0;

    (void)argc;
    (void)argv;

    printf("\r\n========================================\r\n");
    printf("HMAC Self-Test (SW mode over HW SHA engine)\r\n");
    printf("Vectors: RFC 4231\r\n");
    printf("========================================\r\n");

    for (i = 0; i < sizeof(hmac_cases) / sizeof(hmac_cases[0]); i++) {
        const mk_hmac_case_t *c = &hmac_cases[i];
        uint32_t dig = mk_hash_digest_size(c->type);
        uint32_t t0, t1;
        int ret;

        printf("\r\n[CASE] %s\r\n", c->name);
        printf("  %-14s        : %s (hardware)\r\n", "ALGORITHM", mk_hash_name(c->type));
        mk_log_hex("INPUT key", c->key, c->keylen);
        mk_log_hex("INPUT msg", c->msg, c->msglen);
        mk_log_hex("EXPECT mac", c->expect, dig);

        t0 = (uint32_t)mtimer_get_time_us();
        ret = mk_hmac(c->type, c->key, c->keylen, c->msg, c->msglen, out);
        t1 = (uint32_t)mtimer_get_time_us();

        if (ret != 0) {
            printf("  %-14s        : engine error %d\r\n", "OUTPUT mac", ret);
            printf("  RESULT: FAIL\r\n");
            fail++;
            continue;
        }
        mk_log_hex("OUTPUT mac", out, dig);
        printf("  %-14s        : %lu us\r\n", "TIME", (unsigned long)(t1 - t0));

        if (mk_cmp(out, c->expect, dig) != 0) {
            printf("  RESULT: FAIL (mac mismatch)\r\n");
            fail++;
        } else {
            printf("  RESULT: PASS\r\n");
            pass++;
        }
    }

    printf("\r\n----------------------------------------\r\n");
    printf("HMAC Test Result: %d PASS, %d FAIL (Total: %d)\r\n", pass, fail, pass + fail);
    printf("========================================\r\n");
    return (fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_hmac_test, hmac_test, HMAC-SHA2 known-answer test);

/*============================================================================
 * CMAC test vectors — RFC 4493 (AES-128), NIST SP 800-38B (AES-192/256)
 *===========================================================================*/

static const uint8_t cmac_key128[16] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};
static const uint8_t cmac_key192[24] = {
    0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
    0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
    0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
};
static const uint8_t cmac_key256[32] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};
/* SP 800-38B example message; cases use the first 0/16/40/64 bytes */
static const uint8_t cmac_msg[64] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
    0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};

typedef struct {
    const char *name;
    const uint8_t *key;
    uint32_t keylen;
    uint32_t msglen;
    const uint8_t expect[16];
} mk_cmac_case_t;

static const mk_cmac_case_t cmac_cases[] = {
    { "RFC4493 Example 1  AES-128 CMAC, Mlen=0",  cmac_key128, 16, 0,
      { 0xbb, 0x1d, 0x69, 0x29, 0xe9, 0x59, 0x37, 0x28,
        0x7f, 0xa3, 0x7d, 0x12, 0x9b, 0x75, 0x67, 0x46 } },
    { "RFC4493 Example 2  AES-128 CMAC, Mlen=16", cmac_key128, 16, 16,
      { 0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44,
        0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c } },
    { "RFC4493 Example 3  AES-128 CMAC, Mlen=40", cmac_key128, 16, 40,
      { 0xdf, 0xa6, 0x67, 0x47, 0xde, 0x9a, 0xe6, 0x30,
        0x30, 0xca, 0x32, 0x61, 0x14, 0x97, 0xc8, 0x27 } },
    { "RFC4493 Example 4  AES-128 CMAC, Mlen=64", cmac_key128, 16, 64,
      { 0x51, 0xf0, 0xbe, 0xbf, 0x7e, 0x3b, 0x9d, 0x92,
        0xfc, 0x49, 0x74, 0x17, 0x79, 0x36, 0x3c, 0xfe } },
    { "SP800-38B D.2      AES-192 CMAC, Mlen=0",  cmac_key192, 24, 0,
      { 0xd1, 0x7d, 0xdf, 0x46, 0xad, 0xaa, 0xcd, 0xe5,
        0x31, 0xca, 0xc4, 0x83, 0xde, 0x7a, 0x93, 0x67 } },
    { "SP800-38B D.2      AES-192 CMAC, Mlen=16", cmac_key192, 24, 16,
      { 0x9e, 0x99, 0xa7, 0xbf, 0x31, 0xe7, 0x10, 0x90,
        0x06, 0x62, 0xf6, 0x5e, 0x61, 0x7c, 0x51, 0x84 } },
    { "SP800-38B D.2      AES-192 CMAC, Mlen=40", cmac_key192, 24, 40,
      { 0x8a, 0x1d, 0xe5, 0xbe, 0x2e, 0xb3, 0x1a, 0xad,
        0x08, 0x9a, 0x82, 0xe6, 0xee, 0x90, 0x8b, 0x0e } },
    { "SP800-38B D.2      AES-192 CMAC, Mlen=64", cmac_key192, 24, 64,
      { 0xa1, 0xd5, 0xdf, 0x0e, 0xed, 0x79, 0x0f, 0x79,
        0x4d, 0x77, 0x58, 0x96, 0x59, 0xf3, 0x9a, 0x11 } },
    { "SP800-38B D.3      AES-256 CMAC, Mlen=0",  cmac_key256, 32, 0,
      { 0x02, 0x89, 0x62, 0xf6, 0x1b, 0x7b, 0xf8, 0x9e,
        0xfc, 0x6b, 0x55, 0x1f, 0x46, 0x67, 0xd9, 0x83 } },
    { "SP800-38B D.3      AES-256 CMAC, Mlen=16", cmac_key256, 32, 16,
      { 0x28, 0xa7, 0x02, 0x3f, 0x45, 0x2e, 0x8f, 0x82,
        0xbd, 0x4b, 0xf2, 0x8d, 0x8c, 0x37, 0xc3, 0x5c } },
    { "SP800-38B D.3      AES-256 CMAC, Mlen=40", cmac_key256, 32, 40,
      { 0xaa, 0xf3, 0xd8, 0xf1, 0xde, 0x56, 0x40, 0xc2,
        0x32, 0xf5, 0xb1, 0x69, 0xb9, 0xc9, 0x11, 0xe6 } },
    { "SP800-38B D.3      AES-256 CMAC, Mlen=64", cmac_key256, 32, 64,
      { 0xe1, 0x99, 0x21, 0x90, 0x54, 0x9f, 0x6e, 0xd5,
        0x69, 0x6a, 0x2c, 0x05, 0x6c, 0x31, 0x54, 0x10 } },
};

static int cmd_cmac_test(int argc, char **argv)
{
    uint8_t tag[16];
    uint32_t i;
    int pass = 0, fail = 0;

    (void)argc;
    (void)argv;

    printf("\r\n========================================\r\n");
    printf("CMAC Self-Test (SW mode over HW AES engine)\r\n");
    printf("Vectors: RFC 4493 / NIST SP 800-38B\r\n");
    printf("========================================\r\n");

    for (i = 0; i < sizeof(cmac_cases) / sizeof(cmac_cases[0]); i++) {
        const mk_cmac_case_t *c = &cmac_cases[i];
        uint32_t t0, t1;
        int ret;

        printf("\r\n[CASE] %s\r\n", c->name);
        printf("  %-14s        : AES-%lu ECB (hardware)\r\n", "ALGORITHM",
               (unsigned long)(c->keylen * 8));
        mk_log_hex("INPUT key", c->key, c->keylen);
        mk_log_hex("INPUT msg", cmac_msg, c->msglen);
        mk_log_hex("EXPECT tag", c->expect, 16);

        t0 = (uint32_t)mtimer_get_time_us();
        ret = mk_cmac(c->key, c->keylen, cmac_msg, c->msglen, tag);
        t1 = (uint32_t)mtimer_get_time_us();

        if (ret != 0) {
            printf("  %-14s        : engine error %d\r\n", "OUTPUT tag", ret);
            printf("  RESULT: FAIL\r\n");
            fail++;
            continue;
        }
        mk_log_hex("OUTPUT tag", tag, 16);
        printf("  %-14s        : %lu us\r\n", "TIME", (unsigned long)(t1 - t0));

        if (mk_cmp(tag, c->expect, 16) != 0) {
            printf("  RESULT: FAIL (tag mismatch)\r\n");
            fail++;
        } else {
            printf("  RESULT: PASS\r\n");
            pass++;
        }
    }

    printf("\r\n----------------------------------------\r\n");
    printf("CMAC Test Result: %d PASS, %d FAIL (Total: %d)\r\n", pass, fail, pass + fail);
    printf("========================================\r\n");
    return (fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cmac_test, cmac_test, AES-CMAC known-answer test);

/*============================================================================
 * GMAC test vectors — NIST SP 800-38D / GCM specification
 *===========================================================================*/

static const uint8_t gmac_key128_zero[16] = { 0 };
static const uint8_t gmac_iv_zero[12] = { 0 };
static const uint8_t gmac_key128_tc4[16] = {
    0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
    0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08
};
static const uint8_t gmac_key256_tc16[32] = {
    0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
    0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
    0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
    0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08
};
static const uint8_t gmac_iv_cafe[12] = {
    0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad, 0xde, 0xca, 0xf8, 0x88
};
static const uint8_t gmac_aad_tc4[20] = {
    0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
    0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
    0xab, 0xad, 0xda, 0xd2
};

typedef struct {
    const char *name;
    const uint8_t *key;
    uint32_t keylen;
    const uint8_t *iv;
    const uint8_t *aad;
    uint32_t aadlen;
    const uint8_t expect[16];
} mk_gmac_case_t;

static const mk_gmac_case_t gmac_cases[] = {
    { "NIST GCM Test Case 1  AES-128 GMAC, Alen=0", gmac_key128_zero, 16,
      gmac_iv_zero, NULL, 0,
      { 0x58, 0xe2, 0xfc, 0xce, 0xfa, 0x7e, 0x30, 0x61,
        0x36, 0x7f, 0x1d, 0x57, 0xa4, 0xe7, 0x45, 0x5a } },
    { "NIST GCM TC4 K/IV/A   AES-128 GMAC, Plen=0", gmac_key128_tc4, 16,
      gmac_iv_cafe, gmac_aad_tc4, 20,
      { 0x34, 0x64, 0x34, 0xfd, 0x51, 0xd5, 0xcd, 0x0c,
        0x58, 0x87, 0xec, 0x63, 0xe3, 0x9b, 0x90, 0x7a } },
    { "NIST GCM TC16 K/IV/A  AES-256 GMAC, Plen=0", gmac_key256_tc16, 32,
      gmac_iv_cafe, gmac_aad_tc4, 20,
      { 0x9f, 0x6b, 0xe0, 0x76, 0x03, 0xc0, 0xb0, 0xbd,
        0x12, 0x72, 0x85, 0x40, 0x63, 0xe9, 0xc9, 0xba } },
};

static int cmd_gmac_kat(int argc, char **argv)
{
    uint8_t tag[16], h[16], ej0[16], ghash[16];
    uint32_t i;
    int pass = 0, fail = 0;

    (void)argc;
    (void)argv;

    printf("\r\n========================================\r\n");
    printf("GMAC Self-Test (HW AES + HW GHASH engines)\r\n");
    printf("Vectors: NIST SP 800-38D / GCM specification\r\n");
    printf("========================================\r\n");

    for (i = 0; i < sizeof(gmac_cases) / sizeof(gmac_cases[0]); i++) {
        const mk_gmac_case_t *c = &gmac_cases[i];
        uint32_t t0, t1;
        int ret;

        printf("\r\n[CASE] %s\r\n", c->name);
        printf("  %-14s        : AES-%lu GCM/GMAC (hardware)\r\n", "ALGORITHM",
               (unsigned long)(c->keylen * 8));
        mk_log_hex("INPUT key", c->key, c->keylen);
        mk_log_hex("INPUT iv", c->iv, 12);
        mk_log_hex("INPUT aad", c->aad ? c->aad : (const uint8_t *)"", c->aadlen);
        printf("  %-14s        : 0 (GMAC authenticates AAD only)\r\n", "INPUT plaintext");
        mk_log_hex("EXPECT tag", c->expect, 16);

        t0 = (uint32_t)mtimer_get_time_us();
        ret = mk_gmac(c->key, c->keylen, c->iv, c->aad, c->aadlen,
                      h, ej0, ghash, tag);
        t1 = (uint32_t)mtimer_get_time_us();

        if (ret != 0) {
            printf("  %-14s        : engine error %d\r\n", "OUTPUT tag", ret);
            printf("  RESULT: FAIL\r\n");
            fail++;
            continue;
        }
        mk_log_hex("TRACE H", h, 16);
        mk_log_hex("TRACE E(K,J0)", ej0, 16);
        mk_log_hex("TRACE GHASH", ghash, 16);
        mk_log_hex("OUTPUT tag", tag, 16);
        printf("  %-14s        : %lu us\r\n", "TIME", (unsigned long)(t1 - t0));

        if (mk_cmp(tag, c->expect, 16) != 0) {
            printf("  RESULT: FAIL (tag mismatch)\r\n");
            fail++;
        } else {
            printf("  RESULT: PASS\r\n");
            pass++;
        }
    }

    printf("\r\n----------------------------------------\r\n");
    printf("GMAC Test Result: %d PASS, %d FAIL (Total: %d)\r\n", pass, fail, pass + fail);
    printf("========================================\r\n");
    return (fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_gmac_kat, gmac_kat, AES-GMAC known-answer test);

/*============================================================================
 * HKDF test vectors — RFC 5869 (SHA-256)
 *===========================================================================*/

static const uint8_t hkdf_ikm_tc1[22] = {
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
};
static const uint8_t hkdf_salt_tc1[13] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c
};
static const uint8_t hkdf_info_tc1[10] = {
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9
};
static const uint8_t hkdf_prk_tc1[32] = {
    0x07, 0x77, 0x09, 0x36, 0x2c, 0x2e, 0x32, 0xdf, 0x0d, 0xdc,
    0x3f, 0x0d, 0xc4, 0x7b, 0xba, 0x63, 0x90, 0xb6, 0xc7, 0x3b,
    0xb5, 0x0f, 0x9c, 0x31, 0x22, 0xec, 0x84, 0x4a, 0xd7, 0xc2,
    0xb3, 0xe5
};
static const uint8_t hkdf_okm_tc1[42] = {
    0x3c, 0xb2, 0x5f, 0x25, 0xfa, 0xac, 0xd5, 0x7a, 0x90, 0x43,
    0x4f, 0x64, 0xd0, 0x36, 0x2f, 0x2a, 0x2d, 0x2d, 0x0a, 0x90,
    0xcf, 0x1a, 0x5a, 0x4c, 0x5d, 0xb0, 0x2d, 0x56, 0xec, 0xc4,
    0xc5, 0xbf, 0x34, 0x00, 0x72, 0x08, 0xd5, 0xb8, 0x87, 0x18,
    0x58, 0x65
};

/* TC2 uses 80-byte IKM, 80-byte salt, 80-byte info and an 82-byte OKM */
static const uint8_t hkdf_ikm_tc2[80] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
    0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31,
    0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
    0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
    0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
};
static const uint8_t hkdf_salt_tc2[80] = {
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73,
    0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d,
    0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91,
    0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b,
    0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf
};
static const uint8_t hkdf_info_tc2[80] = {
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9,
    0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd,
    0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb,
    0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5,
    0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};
static const uint8_t hkdf_prk_tc2[32] = {
    0x06, 0xa6, 0xb8, 0x8c, 0x58, 0x53, 0x36, 0x1a, 0x06, 0x10,
    0x4c, 0x9c, 0xeb, 0x35, 0xb4, 0x5c, 0xef, 0x76, 0x00, 0x14,
    0x90, 0x46, 0x71, 0x01, 0x4a, 0x19, 0x3f, 0x40, 0xc1, 0x5f,
    0xc2, 0x44
};
static const uint8_t hkdf_okm_tc2[82] = {
    0xb1, 0x1e, 0x39, 0x8d, 0xc8, 0x03, 0x27, 0xa1, 0xc8, 0xe7,
    0xf7, 0x8c, 0x59, 0x6a, 0x49, 0x34, 0x4f, 0x01, 0x2e, 0xda,
    0x2d, 0x4e, 0xfa, 0xd8, 0xa0, 0x50, 0xcc, 0x4c, 0x19, 0xaf,
    0xa9, 0x7c, 0x59, 0x04, 0x5a, 0x99, 0xca, 0xc7, 0x82, 0x72,
    0x71, 0xcb, 0x41, 0xc6, 0x5e, 0x59, 0x0e, 0x09, 0xda, 0x32,
    0x75, 0x60, 0x0c, 0x2f, 0x09, 0xb8, 0x36, 0x77, 0x93, 0xa9,
    0xac, 0xa3, 0xdb, 0x71, 0xcc, 0x30, 0xc5, 0x81, 0x79, 0xec,
    0x3e, 0x87, 0xc1, 0x4c, 0x01, 0xd5, 0xc1, 0xf3, 0x43, 0x4f,
    0x1d, 0x87
};

/* TC3: zero-length salt and info */
static const uint8_t hkdf_prk_tc3[32] = {
    0x19, 0xef, 0x24, 0xa3, 0x2c, 0x71, 0x7b, 0x16, 0x7f, 0x33,
    0xa9, 0x1d, 0x6f, 0x64, 0x8b, 0xdf, 0x96, 0x59, 0x67, 0x76,
    0xaf, 0xdb, 0x63, 0x77, 0xac, 0x43, 0x4c, 0x1c, 0x29, 0x3c,
    0xcb, 0x04
};
static const uint8_t hkdf_okm_tc3[42] = {
    0x8d, 0xa4, 0xe7, 0x75, 0xa5, 0x63, 0xc1, 0x8f, 0x71, 0x5f,
    0x80, 0x2a, 0x06, 0x3c, 0x5a, 0x31, 0xb8, 0xa1, 0x1f, 0x5c,
    0x5e, 0xe1, 0x87, 0x9e, 0xc3, 0x45, 0x4e, 0x5f, 0x3c, 0x73,
    0x8d, 0x2d, 0x9d, 0x20, 0x13, 0x95, 0xfa, 0xa4, 0xb6, 0x1a,
    0x96, 0xc8
};

typedef struct {
    const char *name;
    const uint8_t *ikm;
    uint32_t ikmlen;
    const uint8_t *salt;
    uint32_t saltlen;
    const uint8_t *info;
    uint32_t infolen;
    const uint8_t *prk;
    const uint8_t *okm;
    uint32_t okmlen;
} mk_hkdf_case_t;

static const mk_hkdf_case_t hkdf_cases[] = {
    { "RFC5869 TC1 HKDF-SHA-256 basic",
      hkdf_ikm_tc1, 22, hkdf_salt_tc1, 13, hkdf_info_tc1, 10,
      hkdf_prk_tc1, hkdf_okm_tc1, 42 },
    { "RFC5869 TC2 HKDF-SHA-256 long inputs",
      hkdf_ikm_tc2, 80, hkdf_salt_tc2, 80, hkdf_info_tc2, 80,
      hkdf_prk_tc2, hkdf_okm_tc2, 82 },
    { "RFC5869 TC3 HKDF-SHA-256 zero salt/info",
      hkdf_ikm_tc1, 22, NULL, 0, NULL, 0,
      hkdf_prk_tc3, hkdf_okm_tc3, 42 },
};

static int cmd_hkdf_test(int argc, char **argv)
{
    uint8_t prk[MK_DIG_MAX];
    uint8_t okm[MK_OKM_MAX];
    uint32_t i;
    int pass = 0, fail = 0;

    (void)argc;
    (void)argv;

    printf("\r\n========================================\r\n");
    printf("HKDF Self-Test (SW mode over HW SHA engine)\r\n");
    printf("Vectors: RFC 5869\r\n");
    printf("========================================\r\n");

    for (i = 0; i < sizeof(hkdf_cases) / sizeof(hkdf_cases[0]); i++) {
        const mk_hkdf_case_t *c = &hkdf_cases[i];
        uint32_t t0, t1;
        int step_fail = 0;
        int ret;

        printf("\r\n[CASE] %s\r\n", c->name);
        printf("  %-14s        : HKDF-Extract + HKDF-Expand, SHA-256 (hardware)\r\n",
               "ALGORITHM");
        mk_log_hex("INPUT ikm", c->ikm, c->ikmlen);
        mk_log_hex("INPUT salt", c->salt ? c->salt : (const uint8_t *)"", c->saltlen);
        mk_log_hex("INPUT info", c->info ? c->info : (const uint8_t *)"", c->infolen);
        printf("  %-14s        : %lu\r\n", "INPUT L", (unsigned long)c->okmlen);
        mk_log_hex("EXPECT prk", c->prk, 32);
        mk_log_hex("EXPECT okm", c->okm, c->okmlen);

        t0 = (uint32_t)mtimer_get_time_us();
        ret = mk_hkdf_extract(SEC_HASH_SHA256, c->salt, c->saltlen,
                              c->ikm, c->ikmlen, prk);
        if (ret == 0) {
            ret = mk_hkdf_expand(SEC_HASH_SHA256, prk, 32,
                                 c->info, c->infolen, okm, c->okmlen);
        }
        t1 = (uint32_t)mtimer_get_time_us();

        if (ret != 0) {
            printf("  %-14s        : engine error %d\r\n", "OUTPUT", ret);
            printf("  RESULT: FAIL\r\n");
            fail++;
            continue;
        }
        mk_log_hex("OUTPUT prk", prk, 32);
        mk_log_hex("OUTPUT okm", okm, c->okmlen);
        printf("  %-14s        : %lu us\r\n", "TIME", (unsigned long)(t1 - t0));

        if (mk_cmp(prk, c->prk, 32) != 0) {
            printf("  RESULT: FAIL (prk mismatch)\r\n");
            step_fail = 1;
        } else if (mk_cmp(okm, c->okm, c->okmlen) != 0) {
            printf("  RESULT: FAIL (okm mismatch)\r\n");
            step_fail = 1;
        }
        if (step_fail) {
            fail++;
        } else {
            printf("  RESULT: PASS\r\n");
            pass++;
        }
    }

    printf("\r\n----------------------------------------\r\n");
    printf("HKDF Test Result: %d PASS, %d FAIL (Total: %d)\r\n", pass, fail, pass + fail);
    printf("========================================\r\n");
    return (fail == 0) ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_hkdf_test, hkdf_test, HKDF-SHA256 known-answer test);

/*============================================================================
 * Combined MAC/KDF suite
 *===========================================================================*/

static int cmd_mac_kdf_test(int argc, char **argv)
{
    int hmac_ret, cmac_ret, gmac_ret, hkdf_ret;

    (void)argc;
    (void)argv;

    printf("\r\n########################################\r\n");
    printf("# MAC / KDF Certification Self-Test    #\r\n");
    printf("########################################\r\n");

    hmac_ret = cmd_hmac_test(0, NULL);
    cmac_ret = cmd_cmac_test(0, NULL);
    gmac_ret = cmd_gmac_kat(0, NULL);
    hkdf_ret = cmd_hkdf_test(0, NULL);

    printf("\r\n########################################\r\n");
    printf("# MAC / KDF Summary                     #\r\n");
    printf("########################################\r\n");
    printf("  HMAC : %s\r\n", hmac_ret == 0 ? "PASS" : "FAIL");
    printf("  CMAC : %s\r\n", cmac_ret == 0 ? "PASS" : "FAIL");
    printf("  GMAC : %s\r\n", gmac_ret == 0 ? "PASS" : "FAIL");
    printf("  HKDF : %s\r\n", hkdf_ret == 0 ? "PASS" : "FAIL");
    printf("########################################\r\n");

    return (hmac_ret | cmac_ret | gmac_ret | hkdf_ret) == 0 ? 0 : -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mac_kdf_test, mac_kdf_test, Run HMAC CMAC GMAC HKDF tests);
