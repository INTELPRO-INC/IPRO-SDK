/*
 *  AES-GCM hardware acceleration for IPRO7
 *
 *  Combines AES-ECB HW (for CTR keystream + H computation) with
 *  GMAC HW (for GHASH authentication) to implement NIST SP 800-38D.
 *
 *  CTR mode is done manually via AES-ECB to guarantee correct 32-bit
 *  counter increment semantics required by GCM (the HW CTR engine
 *  uses 128-bit counter increment which is incompatible).
 *
 *  Copyright (C) 2025, INTELPRO Inc.
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"

#if defined(MBEDTLS_GCM_C)

#include "mbedtls/gcm.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include <string.h>

#if defined(MBEDTLS_GCM_ALT)

#include "hal_sec_aes.h"
#include "hal_sec_gmac.h"

/* Global AES handle — avoid large stack allocation on embedded targets */
static sec_aes_handle_t g_gcm_aes_handle __attribute__((aligned(64)));

#define GCM_VALIDATE_RET(cond) \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_GCM_BAD_INPUT)
#define GCM_VALIDATE(cond) \
    MBEDTLS_INTERNAL_VALIDATE(cond)

/*
 * Map keybits to HAL key type enum.
 */
static sec_aes_key_type gcm_key_type(unsigned int keybits)
{
    switch (keybits) {
    case 128: return SEC_AES_KEY_128;
    case 192: return SEC_AES_KEY_192;
    case 256: return SEC_AES_KEY_256;
    default:  return SEC_AES_KEY_128;
    }
}

/*
 * AES-ECB encrypt a single 16-byte block using HW.
 * Caller must ensure in/out are 16 bytes.
 */
static int gcm_aes_ecb(const uint8_t *key, unsigned int keybits,
                        const uint8_t in[16], uint8_t out[16])
{
    int ret;

    ret = sec_aes_init_ex(&g_gcm_aes_handle, SEC_MODE_LINK,
                          SEC_AES_ECB, gcm_key_type(keybits));
    if (ret != 0)
        return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;

    ret = sec_aes_setkey(&g_gcm_aes_handle, key, keybits / 8,
                         NULL, SEC_AES_DIR_ENCRYPT);
    if (ret != 0) {
        sec_aes_deinit(&g_gcm_aes_handle);
        return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;
    }

    ret = sec_aes_encrypt(&g_gcm_aes_handle, in, 16, 0, out);
    sec_aes_deinit(&g_gcm_aes_handle);

    return (ret == 0) ? 0 : MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;
}

/*
 * Increment the 32-bit counter in the last 4 bytes of a 16-byte block
 * (big-endian, as specified by GCM / NIST SP 800-38D).
 */
static void gcm_incr32(unsigned char y[16])
{
    uint32_t val = ((uint32_t)y[12] << 24) |
                   ((uint32_t)y[13] << 16) |
                   ((uint32_t)y[14] << 8)  |
                   ((uint32_t)y[15]);
    val++;
    y[12] = (unsigned char)(val >> 24);
    y[13] = (unsigned char)(val >> 16);
    y[14] = (unsigned char)(val >> 8);
    y[15] = (unsigned char)(val);
}

/*
 * XOR n bytes: dst ^= src
 */
static void gcm_xor(unsigned char *dst, const unsigned char *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
        dst[i] ^= src[i];
}

/*
 * Store a 64-bit value as big-endian into buf[0..7].
 */
static void gcm_put_be64(unsigned char buf[8], uint64_t val)
{
    buf[0] = (unsigned char)(val >> 56);
    buf[1] = (unsigned char)(val >> 48);
    buf[2] = (unsigned char)(val >> 40);
    buf[3] = (unsigned char)(val >> 32);
    buf[4] = (unsigned char)(val >> 24);
    buf[5] = (unsigned char)(val >> 16);
    buf[6] = (unsigned char)(val >> 8);
    buf[7] = (unsigned char)(val);
}

void mbedtls_gcm_init(mbedtls_gcm_context *ctx)
{
    GCM_VALIDATE(ctx != NULL);
    memset(ctx, 0, sizeof(mbedtls_gcm_context));
}

void mbedtls_gcm_free(mbedtls_gcm_context *ctx)
{
    if (ctx == NULL)
        return;
    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_gcm_context));
}

int mbedtls_gcm_setkey(mbedtls_gcm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        const unsigned char *key,
                        unsigned int keybits)
{
    unsigned char zero_block[16] = { 0 };
    int ret;

    GCM_VALIDATE_RET(ctx != NULL);
    GCM_VALIDATE_RET(key != NULL);

    if (cipher != MBEDTLS_CIPHER_ID_AES)
        return MBEDTLS_ERR_GCM_BAD_INPUT;

    if (keybits != 128 && keybits != 192 && keybits != 256)
        return MBEDTLS_ERR_GCM_BAD_INPUT;

    ctx->keybits = keybits;
    memcpy(ctx->aes_key, key, keybits / 8);

    /* Compute GHASH subkey: H = AES_K(0^128) */
    ret = gcm_aes_ecb(ctx->aes_key, ctx->keybits, zero_block, ctx->h_key);
    if (ret != 0)
        return ret;

    /* Initialize GMAC hardware (idempotent) */
    sec_gmac_init();

    return 0;
}

int mbedtls_gcm_starts(mbedtls_gcm_context *ctx,
                        int mode,
                        const unsigned char *iv,
                        size_t iv_len,
                        const unsigned char *add,
                        size_t add_len)
{
    int ret;
    unsigned char work_buf[16];

    GCM_VALIDATE_RET(ctx != NULL);
    GCM_VALIDATE_RET(iv != NULL);
    GCM_VALIDATE_RET(add_len == 0 || add != NULL);

    if (iv_len == 0)
        return MBEDTLS_ERR_GCM_BAD_INPUT;

    ctx->mode = mode;
    ctx->len = 0;
    ctx->add_len = 0;

    /* Compute J0 (initial counter block) */
    if (iv_len == 12) {
        /* Common case: J0 = IV || 0^31 || 1 */
        memcpy(ctx->y, iv, 12);
        ctx->y[12] = 0;
        ctx->y[13] = 0;
        ctx->y[14] = 0;
        ctx->y[15] = 1;
    } else {
        /*
         * General case: J0 = GHASH_H(IV || pad || 0^64 || [len(IV)]^64)
         * where pad brings (IV || pad) to a multiple of 16 bytes.
         */
        sec_gmac_handle_t iv_gmac;
        memset(&iv_gmac, 0, sizeof(iv_gmac));

        ret = sec_gmac_start(&iv_gmac, ctx->h_key);
        if (ret != 0)
            return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;

        ret = sec_gmac_clear(&iv_gmac);
        if (ret != 0) {
            sec_gmac_deinit(&iv_gmac);
            return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;
        }

        /* Feed IV (GMAC pads partial blocks with zeros) */
        ret = sec_gmac_update(&iv_gmac, iv, iv_len);
        if (ret != 0) {
            sec_gmac_deinit(&iv_gmac);
            return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;
        }

        /*
         * If IV was not a multiple of 16, GMAC already zero-padded it.
         * But we still need explicit zero-padding to reach the format:
         * IV || 0-pad-to-16-boundary (already done) || 0^64 || [len(IV)*8]^64
         *
         * GMAC auto-pads partial blocks, so after the IV feed the GHASH
         * state already includes the padded IV. Now feed the 16-byte
         * length block: [0^64 || len(IV)*8 as 64-bit BE].
         */
        memset(work_buf, 0, 16);
        gcm_put_be64(work_buf + 8, (uint64_t)iv_len * 8);

        ret = sec_gmac_update(&iv_gmac, work_buf, 16);
        if (ret != 0) {
            sec_gmac_deinit(&iv_gmac);
            return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;
        }

        ret = sec_gmac_finish(&iv_gmac, ctx->y);
        sec_gmac_deinit(&iv_gmac);
        if (ret != 0)
            return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;
    }

    /* Compute base_ectr = AES_K(J0) for final tag XOR */
    ret = gcm_aes_ecb(ctx->aes_key, ctx->keybits, ctx->y, ctx->base_ectr);
    if (ret != 0)
        return ret;

    /* Increment counter for CTR data encryption (J0+1) */
    gcm_incr32(ctx->y);

    /* Initialize GMAC for authentication with GHASH subkey H */
    ret = sec_gmac_start(&ctx->gmac_handle, ctx->h_key);
    if (ret != 0)
        return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;

    ret = sec_gmac_clear(&ctx->gmac_handle);
    if (ret != 0)
        return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;

    /* GHASH the AAD */
    if (add_len > 0) {
        ret = sec_gmac_update(&ctx->gmac_handle, add, add_len);
        if (ret != 0)
            return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;
        ctx->add_len = add_len;
    }

    return 0;
}

int mbedtls_gcm_update(mbedtls_gcm_context *ctx,
                        size_t length,
                        const unsigned char *input,
                        unsigned char *output)
{
    int ret;
    unsigned char ectr[16];
    size_t offset = 0;

    GCM_VALIDATE_RET(ctx != NULL);
    GCM_VALIDATE_RET(length == 0 || input != NULL);
    GCM_VALIDATE_RET(length == 0 || output != NULL);

    if (length == 0)
        return 0;

    /* Process full 16-byte blocks */
    while (offset + 16 <= length) {
        /* Generate keystream: ectr = AES_K(counter) */
        ret = gcm_aes_ecb(ctx->aes_key, ctx->keybits, ctx->y, ectr);
        if (ret != 0)
            return ret;
        gcm_incr32(ctx->y);

        if (ctx->mode == MBEDTLS_GCM_ENCRYPT) {
            /* Encrypt: output = input XOR keystream */
            memcpy(output + offset, input + offset, 16);
            gcm_xor(output + offset, ectr, 16);
            /* GHASH ciphertext */
            ret = sec_gmac_update(&ctx->gmac_handle, output + offset, 16);
        } else {
            /* GHASH ciphertext (= input for decrypt) */
            ret = sec_gmac_update(&ctx->gmac_handle, input + offset, 16);
            if (ret != 0)
                return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;
            /* Decrypt: output = input XOR keystream */
            memcpy(output + offset, input + offset, 16);
            gcm_xor(output + offset, ectr, 16);
            ret = 0;
        }
        if (ret != 0)
            return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;

        offset += 16;
    }

    /* Process partial final block (< 16 bytes) */
    if (offset < length) {
        size_t tail = length - offset;

        ret = gcm_aes_ecb(ctx->aes_key, ctx->keybits, ctx->y, ectr);
        if (ret != 0)
            return ret;
        gcm_incr32(ctx->y);

        if (ctx->mode == MBEDTLS_GCM_ENCRYPT) {
            for (size_t i = 0; i < tail; i++)
                output[offset + i] = input[offset + i] ^ ectr[i];
            /* GHASH partial ciphertext (GMAC zero-pads internally) */
            ret = sec_gmac_update(&ctx->gmac_handle, output + offset, tail);
        } else {
            /* GHASH partial ciphertext (= input) */
            ret = sec_gmac_update(&ctx->gmac_handle, input + offset, tail);
            if (ret != 0)
                return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;
            for (size_t i = 0; i < tail; i++)
                output[offset + i] = input[offset + i] ^ ectr[i];
            ret = 0;
        }
        if (ret != 0)
            return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;
    }

    ctx->len += length;

    mbedtls_platform_zeroize(ectr, sizeof(ectr));
    return 0;
}

int mbedtls_gcm_finish(mbedtls_gcm_context *ctx,
                        unsigned char *tag,
                        size_t tag_len)
{
    int ret;
    unsigned char len_block[16];
    unsigned char ghash_out[16];

    GCM_VALIDATE_RET(ctx != NULL);
    GCM_VALIDATE_RET(tag != NULL);

    if (tag_len > 16 || tag_len < 4)
        return MBEDTLS_ERR_GCM_BAD_INPUT;

    /*
     * Final GHASH block: [len(A)*8]_64 || [len(C)*8]_64
     * Both lengths are in bits, stored as big-endian 64-bit.
     */
    gcm_put_be64(len_block, ctx->add_len * 8);
    gcm_put_be64(len_block + 8, ctx->len * 8);

    ret = sec_gmac_update(&ctx->gmac_handle, len_block, 16);
    if (ret != 0)
        return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;

    ret = sec_gmac_finish(&ctx->gmac_handle, ghash_out);
    if (ret != 0)
        return MBEDTLS_ERR_GCM_HW_ACCEL_FAILED;

    sec_gmac_deinit(&ctx->gmac_handle);

    /* Tag = GHASH_result XOR base_ectr */
    gcm_xor(ghash_out, ctx->base_ectr, 16);
    memcpy(tag, ghash_out, tag_len);

    mbedtls_platform_zeroize(ghash_out, sizeof(ghash_out));
    mbedtls_platform_zeroize(len_block, sizeof(len_block));
    return 0;
}

int mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context *ctx,
                               int mode,
                               size_t length,
                               const unsigned char *iv,
                               size_t iv_len,
                               const unsigned char *add,
                               size_t add_len,
                               const unsigned char *input,
                               unsigned char *output,
                               size_t tag_len,
                               unsigned char *tag)
{
    int ret;

    GCM_VALIDATE_RET(ctx != NULL);
    GCM_VALIDATE_RET(iv != NULL);
    GCM_VALIDATE_RET(add_len == 0 || add != NULL);
    GCM_VALIDATE_RET(length == 0 || input != NULL);
    GCM_VALIDATE_RET(length == 0 || output != NULL);
    GCM_VALIDATE_RET(tag != NULL);

    ret = mbedtls_gcm_starts(ctx, mode, iv, iv_len, add, add_len);
    if (ret != 0)
        return ret;

    ret = mbedtls_gcm_update(ctx, length, input, output);
    if (ret != 0)
        return ret;

    ret = mbedtls_gcm_finish(ctx, tag, tag_len);
    return ret;
}

int mbedtls_gcm_auth_decrypt(mbedtls_gcm_context *ctx,
                              size_t length,
                              const unsigned char *iv,
                              size_t iv_len,
                              const unsigned char *add,
                              size_t add_len,
                              const unsigned char *tag,
                              size_t tag_len,
                              const unsigned char *input,
                              unsigned char *output)
{
    int ret;
    unsigned char check_tag[16];
    unsigned char diff;

    GCM_VALIDATE_RET(ctx != NULL);
    GCM_VALIDATE_RET(iv != NULL);
    GCM_VALIDATE_RET(add_len == 0 || add != NULL);
    GCM_VALIDATE_RET(tag != NULL);
    GCM_VALIDATE_RET(length == 0 || input != NULL);
    GCM_VALIDATE_RET(length == 0 || output != NULL);

    if (tag_len > 16 || tag_len < 4)
        return MBEDTLS_ERR_GCM_BAD_INPUT;

    ret = mbedtls_gcm_crypt_and_tag(ctx, MBEDTLS_GCM_DECRYPT, length,
                                     iv, iv_len, add, add_len,
                                     input, output, tag_len, check_tag);
    if (ret != 0)
        return ret;

    /* Constant-time tag comparison */
    diff = 0;
    for (size_t i = 0; i < tag_len; i++)
        diff |= tag[i] ^ check_tag[i];

    mbedtls_platform_zeroize(check_tag, sizeof(check_tag));

    if (diff != 0) {
        mbedtls_platform_zeroize(output, length);
        return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    return 0;
}

/*
 * AES-128-GCM test vectors from NIST SP 800-38D (Test Case 3 & 4)
 */
static const unsigned char gcm_test_key[16] = {
    0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
    0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08
};

static const unsigned char gcm_test_iv[12] = {
    0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad,
    0xde, 0xca, 0xf8, 0x88
};

static const unsigned char gcm_test_pt[64] = {
    0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
    0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
    0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
    0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
    0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
    0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
    0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
    0xba, 0x63, 0x7b, 0x39, 0x1a, 0xaf, 0xd2, 0x55
};

/* Test Case 3: 64 bytes plaintext, no AAD */
static const unsigned char gcm_tc3_ct[64] = {
    0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24,
    0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c,
    0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0,
    0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e,
    0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c,
    0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05,
    0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97,
    0x3d, 0x58, 0xe0, 0x91, 0x47, 0x3f, 0x59, 0x85
};

static const unsigned char gcm_tc3_tag[16] = {
    0x4d, 0x5c, 0x2a, 0xf3, 0x27, 0xcd, 0x64, 0xa6,
    0x2c, 0xf3, 0x5a, 0xbd, 0x2b, 0xa6, 0xfa, 0xb4
};

/* Test Case 4: 60 bytes plaintext + 20 bytes AAD */
static const unsigned char gcm_tc4_aad[20] = {
    0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
    0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
    0xab, 0xad, 0xda, 0xd2
};

static const unsigned char gcm_tc4_ct[60] = {
    0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24,
    0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c,
    0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0,
    0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e,
    0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c,
    0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05,
    0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97,
    0x3d, 0x58, 0xe0, 0x91
};

static const unsigned char gcm_tc4_tag[16] = {
    0x5b, 0xc9, 0x4f, 0xbc, 0x32, 0x21, 0xa5, 0xdb,
    0x94, 0xfa, 0xe9, 0x5a, 0xe7, 0x12, 0x1a, 0x47
};

int mbedtls_gcm_self_test_hw(int verbose)
{
    mbedtls_gcm_context ctx;
    unsigned char buf[64];
    unsigned char tag[16];
    int ret;

    mbedtls_gcm_init(&ctx);

    /* Test Case 3: encrypt */
    if (verbose)
        printf("  AES-GCM-128 test case 3 (enc): ");

    ret = mbedtls_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, gcm_test_key, 128);
    if (ret != 0)
        goto fail;

    ret = mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT, 64,
                                     gcm_test_iv, 12, NULL, 0,
                                     gcm_test_pt, buf, 16, tag);
    if (ret != 0)
        goto fail;

    if (memcmp(buf, gcm_tc3_ct, 64) != 0 ||
        memcmp(tag, gcm_tc3_tag, 16) != 0) {
        ret = 1;
        goto fail;
    }

    if (verbose)
        printf("PASSED\n");

    /* Test Case 3: decrypt + auth */
    if (verbose)
        printf("  AES-GCM-128 test case 3 (dec): ");

    ret = mbedtls_gcm_auth_decrypt(&ctx, 64,
                                    gcm_test_iv, 12, NULL, 0,
                                    gcm_tc3_tag, 16,
                                    gcm_tc3_ct, buf);
    if (ret != 0)
        goto fail;

    if (memcmp(buf, gcm_test_pt, 64) != 0) {
        ret = 1;
        goto fail;
    }

    if (verbose)
        printf("PASSED\n");

    /* Test Case 4: encrypt with AAD */
    if (verbose)
        printf("  AES-GCM-128 test case 4 (enc): ");

    ret = mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT, 60,
                                     gcm_test_iv, 12,
                                     gcm_tc4_aad, 20,
                                     gcm_test_pt, buf, 16, tag);
    if (ret != 0)
        goto fail;

    if (memcmp(buf, gcm_tc4_ct, 60) != 0 ||
        memcmp(tag, gcm_tc4_tag, 16) != 0) {
        ret = 1;
        goto fail;
    }

    if (verbose)
        printf("PASSED\n");

    /* Test Case 4: decrypt + auth with AAD */
    if (verbose)
        printf("  AES-GCM-128 test case 4 (dec): ");

    ret = mbedtls_gcm_auth_decrypt(&ctx, 60,
                                    gcm_test_iv, 12,
                                    gcm_tc4_aad, 20,
                                    gcm_tc4_tag, 16,
                                    gcm_tc4_ct, buf);
    if (ret != 0)
        goto fail;

    if (memcmp(buf, gcm_test_pt, 60) != 0) {
        ret = 1;
        goto fail;
    }

    if (verbose)
        printf("PASSED\n");

    ret = 0;
    goto exit;

fail:
    if (verbose)
        printf("FAILED\n");

exit:
    mbedtls_gcm_free(&ctx);
    return ret;
}

#endif /* MBEDTLS_GCM_ALT */
#endif /* MBEDTLS_GCM_C */
