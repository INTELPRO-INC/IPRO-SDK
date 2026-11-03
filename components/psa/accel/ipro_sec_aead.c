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

/* ipro_sec driver: AES-GCM (CTR32 + GHASH on the SEC_ENG). */
#include <string.h>

#include "psa/crypto.h"
#include "hal_sec_common_v2.h"
#include "ipro_sec_driver.h"
#include "ipro_sec_priv.h"

#define GCM_BLOCK 16u

static void put_be64(uint8_t *p, uint64_t v)
{
    for (int i = 7; i >= 0; i--) {
        p[i] = (uint8_t)v;
        v >>= 8;
    }
}

/* out = E(K, in) for one block under the key of ctr */
static int gcm_ecb(const hal_sec_aes_v2_ctx_t *ctr, const uint8_t in[16],
                   uint8_t out[16])
{
    hal_sec_aes_v2_ctx_t ecb;
    int ret = hal_sec_aes_v2_setkey(&ecb, HAL_SEC_AES_V2_ECB,
                                    HAL_SEC_AES_V2_ENCRYPT, ctr->key,
                                    ctr->keylen);

    if (ret == HAL_SEC_V2_OK) {
        ret = hal_sec_aes_v2_crypt(&ecb, in, out, GCM_BLOCK);
    }
    hal_sec_aes_v2_abort(&ecb);
    return ret;
}

static psa_status_t gcm_setup(ipro_sec_aead_operation_t *operation,
                              const psa_key_attributes_t *attributes,
                              const uint8_t *key_buffer,
                              size_t key_buffer_size, psa_algorithm_t alg,
                              uint8_t is_encrypt)
{
    size_t tag_length = PSA_ALG_AEAD_GET_TAG_LENGTH(alg);
    uint8_t h[GCM_BLOCK] = { 0 };
    psa_status_t st = ipro_sec_aes_key(attributes, key_buffer_size);
    int ret;

#if defined(PSA_WANT_ALG_GCM)
    if (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0) !=
        PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0)) {
        st = PSA_ERROR_NOT_SUPPORTED;
    }
#else
    st = PSA_ERROR_NOT_SUPPORTED;
#endif
    /* other tag lengths: the built-in driver reports the error */
    if (tag_length != 4 && tag_length != 8 &&
        (tag_length < 12 || tag_length > 16)) {
        st = PSA_ERROR_NOT_SUPPORTED;
    }
    if (st != PSA_SUCCESS) {
        return st;
    }
    memset(operation, 0, sizeof(*operation));
    ret = hal_sec_aes_v2_setkey(&operation->ctr, HAL_SEC_AES_V2_CTR32,
                                HAL_SEC_AES_V2_ENCRYPT, key_buffer,
                                key_buffer_size);
    if (ret == HAL_SEC_V2_OK) {
        ret = gcm_ecb(&operation->ctr, h, h);
    }
    if (ret == HAL_SEC_V2_OK) {
        ret = hal_sec_ghash_v2_setup(&operation->ghash, h);
    }
    hal_sec_v2_zeroize(h, sizeof(h));
    operation->tag_length = (uint8_t)tag_length;
    operation->is_encrypt = is_encrypt;
    operation->ks_used = GCM_BLOCK;
    if (ret != HAL_SEC_V2_OK) {
        hal_sec_v2_zeroize(operation, sizeof(*operation));
    }
    return ipro_sec_status(ret);
}

psa_status_t ipro_sec_aead_encrypt_setup(
    ipro_sec_aead_operation_t *operation,
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg)
{
    return gcm_setup(operation, attributes, key_buffer, key_buffer_size, alg,
                     1);
}

psa_status_t ipro_sec_aead_decrypt_setup(
    ipro_sec_aead_operation_t *operation,
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg)
{
    return gcm_setup(operation, attributes, key_buffer, key_buffer_size, alg,
                     0);
}

/* J0 of SP 800-38D 7.1; E(K, J0) and CTR32 at inc32(J0) */
psa_status_t ipro_sec_aead_set_nonce(ipro_sec_aead_operation_t *operation,
                                     const uint8_t *nonce,
                                     size_t nonce_length)
{
    uint8_t j0[GCM_BLOCK] = { 0 };
    int ret = HAL_SEC_V2_OK;

    if (nonce_length == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (nonce_length == 12) {
        memcpy(j0, nonce, 12);
        j0[15] = 1;
    } else {
        hal_sec_ghash_v2_ctx_t g;
        uint8_t len[GCM_BLOCK] = { 0 };

        put_be64(len + 8, (uint64_t)nonce_length * 8u);
        ret = hal_sec_ghash_v2_setup(&g, operation->ghash.h);
        if (ret == HAL_SEC_V2_OK) {
            ret = hal_sec_ghash_v2_update(&g, nonce, nonce_length);
        }
        if (ret == HAL_SEC_V2_OK) {
            ret = hal_sec_ghash_v2_pad(&g);
        }
        if (ret == HAL_SEC_V2_OK) {
            ret = hal_sec_ghash_v2_update(&g, len, sizeof(len));
        }
        if (ret == HAL_SEC_V2_OK) {
            ret = hal_sec_ghash_v2_finish(&g, j0);
        }
        hal_sec_ghash_v2_abort(&g);
    }
    if (ret == HAL_SEC_V2_OK) {
        ret = gcm_ecb(&operation->ctr, j0, operation->ek_j0);
    }
    if (ret == HAL_SEC_V2_OK) {
        for (int i = 15; i >= 12 && ++j0[i] == 0; i--) {
        }
        ret = hal_sec_aes_v2_set_iv(&operation->ctr, j0);
    }
    hal_sec_v2_zeroize(j0, sizeof(j0));
    return ipro_sec_status(ret);
}

psa_status_t ipro_sec_aead_set_lengths(ipro_sec_aead_operation_t *operation,
                                       size_t ad_length,
                                       size_t plaintext_length)
{
    (void)operation;
    (void)ad_length;
    (void)plaintext_length;
    return PSA_SUCCESS;
}

psa_status_t ipro_sec_aead_update_ad(ipro_sec_aead_operation_t *operation,
                                     const uint8_t *input,
                                     size_t input_length)
{
    if (operation->body_started) {
        return PSA_ERROR_BAD_STATE;
    }
    operation->ad_len += input_length;
    return ipro_sec_status(
        hal_sec_ghash_v2_update(&operation->ghash, input, input_length));
}

/* CTR32 keystream: left in ks first, then whole blocks, then one more */
static int gcm_ctr(ipro_sec_aead_operation_t *op, const uint8_t *in,
                   uint8_t *out, size_t len)
{
    size_t n;
    int ret = HAL_SEC_V2_OK;

    while (len && op->ks_used < GCM_BLOCK) {
        *out++ = *in++ ^ op->ks[op->ks_used++];
        len--;
    }
    n = len & ~(size_t)(GCM_BLOCK - 1);
    if (n) {
        ret = hal_sec_aes_v2_crypt(&op->ctr, in, out, n);
        in += n;
        out += n;
        len -= n;
    }
    if (ret == HAL_SEC_V2_OK && len) {
        memset(op->ks, 0, sizeof(op->ks));
        ret = hal_sec_aes_v2_crypt(&op->ctr, op->ks, op->ks, GCM_BLOCK);
        op->ks_used = 0;
        while (ret == HAL_SEC_V2_OK && len) {
            *out++ = *in++ ^ op->ks[op->ks_used++];
            len--;
        }
    }
    return ret;
}

psa_status_t ipro_sec_aead_update(ipro_sec_aead_operation_t *operation,
                                  const uint8_t *input, size_t input_length,
                                  uint8_t *output, size_t output_size,
                                  size_t *output_length)
{
    int ret = HAL_SEC_V2_OK;

    *output_length = 0;
    if (output_size < input_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    if (!operation->body_started) {
        ret = hal_sec_ghash_v2_pad(&operation->ghash);
        operation->body_started = 1;
    }
    if (ret == HAL_SEC_V2_OK && input_length) {
        if (!operation->is_encrypt) {
            ret = hal_sec_ghash_v2_update(&operation->ghash, input,
                                          input_length);
        }
        if (ret == HAL_SEC_V2_OK) {
            ret = gcm_ctr(operation, input, output, input_length);
        }
        if (ret == HAL_SEC_V2_OK && operation->is_encrypt) {
            ret = hal_sec_ghash_v2_update(&operation->ghash, output,
                                          input_length);
        }
    }
    if (ret == HAL_SEC_V2_OK) {
        operation->text_len += input_length;
        *output_length = input_length;
    }
    return ipro_sec_status(ret);
}

/* tag = E(K, J0) ^ GHASH(A || C || len(A) || len(C)), 16 bytes */
static int gcm_tag(ipro_sec_aead_operation_t *operation, uint8_t tag[16])
{
    uint8_t len[GCM_BLOCK];
    int ret = hal_sec_ghash_v2_pad(&operation->ghash);

    put_be64(len, operation->ad_len * 8u);
    put_be64(len + 8, operation->text_len * 8u);
    if (ret == HAL_SEC_V2_OK) {
        ret = hal_sec_ghash_v2_update(&operation->ghash, len, sizeof(len));
    }
    if (ret == HAL_SEC_V2_OK) {
        ret = hal_sec_ghash_v2_finish(&operation->ghash, tag);
    }
    for (size_t i = 0; i < GCM_BLOCK; i++) {
        tag[i] ^= operation->ek_j0[i];
    }
    return ret;
}

psa_status_t ipro_sec_aead_finish(ipro_sec_aead_operation_t *operation,
                                  uint8_t *ciphertext,
                                  size_t ciphertext_size,
                                  size_t *ciphertext_length, uint8_t *tag,
                                  size_t tag_size, size_t *tag_length)
{
    uint8_t t[GCM_BLOCK];
    int ret;

    (void)ciphertext;
    (void)ciphertext_size;
    if (tag_size < operation->tag_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    ret = gcm_tag(operation, t);
    if (ret == HAL_SEC_V2_OK) {
        memcpy(tag, t, operation->tag_length);
        *ciphertext_length = 0;
        *tag_length = operation->tag_length;
    }
    hal_sec_v2_zeroize(t, sizeof(t));
    return ipro_sec_status(ret);
}

psa_status_t ipro_sec_aead_verify(ipro_sec_aead_operation_t *operation,
                                  uint8_t *plaintext, size_t plaintext_size,
                                  size_t *plaintext_length,
                                  const uint8_t *tag, size_t tag_length)
{
    uint8_t t[GCM_BLOCK];
    uint8_t diff = 0;
    int ret;

    (void)plaintext;
    (void)plaintext_size;
    ret = gcm_tag(operation, t);
    if (ret == HAL_SEC_V2_OK) {
        for (size_t i = 0; i < operation->tag_length && i < tag_length; i++) {
            diff |= t[i] ^ tag[i];
        }
        if (tag_length != operation->tag_length || diff != 0) {
            ret = HAL_SEC_V2_ERR_VERIFY;
        }
    }
    hal_sec_v2_zeroize(t, sizeof(t));
    if (ret == HAL_SEC_V2_OK) {
        *plaintext_length = 0;
    }
    return ipro_sec_status(ret);
}

psa_status_t ipro_sec_aead_abort(ipro_sec_aead_operation_t *operation)
{
    hal_sec_v2_zeroize(operation, sizeof(*operation));
    return PSA_SUCCESS;
}

psa_status_t ipro_sec_aead_encrypt(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *plaintext,
    size_t plaintext_length, uint8_t *ciphertext, size_t ciphertext_size,
    size_t *ciphertext_length)
{
    ipro_sec_aead_operation_t op;
    size_t n = 0;
    size_t m = 0;
    size_t t = 0;
    psa_status_t st = ipro_sec_aead_encrypt_setup(&op, attributes,
                                                  key_buffer,
                                                  key_buffer_size, alg);

    if (st != PSA_SUCCESS) {
        return st;
    }
    if (ciphertext_size < plaintext_length + op.tag_length) {
        st = PSA_ERROR_BUFFER_TOO_SMALL;
    }
    if (st == PSA_SUCCESS) {
        st = ipro_sec_aead_set_nonce(&op, nonce, nonce_length);
    }
    if (st == PSA_SUCCESS) {
        st = ipro_sec_aead_update_ad(&op, additional_data,
                                     additional_data_length);
    }
    if (st == PSA_SUCCESS) {
        st = ipro_sec_aead_update(&op, plaintext, plaintext_length,
                                  ciphertext, ciphertext_size, &n);
    }
    if (st == PSA_SUCCESS) {
        st = ipro_sec_aead_finish(&op, NULL, 0, &m, ciphertext + n,
                                  ciphertext_size - n, &t);
    }
    if (st == PSA_SUCCESS) {
        *ciphertext_length = n + m + t;
    }
    ipro_sec_aead_abort(&op);
    return st;
}

psa_status_t ipro_sec_aead_decrypt(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *ciphertext,
    size_t ciphertext_length, uint8_t *plaintext, size_t plaintext_size,
    size_t *plaintext_length)
{
    ipro_sec_aead_operation_t op;
    size_t payload = 0;
    size_t n = 0;
    size_t m = 0;
    psa_status_t st = ipro_sec_aead_decrypt_setup(&op, attributes,
                                                  key_buffer,
                                                  key_buffer_size, alg);

    if (st != PSA_SUCCESS) {
        return st;
    }
    if (op.tag_length > ciphertext_length) {
        st = PSA_ERROR_INVALID_ARGUMENT;
    } else {
        payload = ciphertext_length - op.tag_length;
        if (payload > plaintext_size) {
            st = PSA_ERROR_BUFFER_TOO_SMALL;
        }
    }
    if (st == PSA_SUCCESS) {
        st = ipro_sec_aead_set_nonce(&op, nonce, nonce_length);
    }
    if (st == PSA_SUCCESS) {
        st = ipro_sec_aead_update_ad(&op, additional_data,
                                     additional_data_length);
    }
    if (st == PSA_SUCCESS) {
        st = ipro_sec_aead_update(&op, ciphertext, payload, plaintext,
                                  plaintext_size, &n);
    }
    if (st == PSA_SUCCESS) {
        st = ipro_sec_aead_verify(&op, NULL, 0, &m, ciphertext + payload,
                                  op.tag_length);
    }
    if (st == PSA_SUCCESS) {
        *plaintext_length = n + m;
    } else if (plaintext_size != 0 && st != PSA_ERROR_BUFFER_TOO_SMALL) {
        hal_sec_v2_zeroize(plaintext, payload);
    }
    ipro_sec_aead_abort(&op);
    return st;
}
