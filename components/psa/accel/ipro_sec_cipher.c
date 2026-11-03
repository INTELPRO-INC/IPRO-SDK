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

/* ipro_sec driver: AES ECB and CBC without padding, CTR. */
#include <string.h>

#include "psa/crypto.h"
#include "hal_sec_common_v2.h"
#include "ipro_sec_driver.h"
#include "ipro_sec_priv.h"

#define IPRO_SEC_BLOCK 16u

psa_status_t ipro_sec_aes_key(const psa_key_attributes_t *attributes,
                              size_t key_buffer_size)
{
    size_t bits = psa_get_key_bits(attributes);

#if defined(PSA_WANT_KEY_TYPE_AES)
    if (psa_get_key_type(attributes) == PSA_KEY_TYPE_AES &&
        (bits == 128 || bits == 192 || bits == 256) &&
        key_buffer_size == bits / 8) {
        return PSA_SUCCESS;
    }
#else
    (void)attributes;
    (void)key_buffer_size;
    (void)bits;
#endif
    return PSA_ERROR_NOT_SUPPORTED;
}

static psa_status_t cipher_setup(ipro_sec_cipher_operation_t *operation,
                                 const psa_key_attributes_t *attributes,
                                 const uint8_t *key_buffer,
                                 size_t key_buffer_size, psa_algorithm_t alg,
                                 unsigned int dir)
{
    unsigned int mode;
    uint8_t iv_length = IPRO_SEC_BLOCK;
    psa_status_t st = ipro_sec_aes_key(attributes, key_buffer_size);

    if (st != PSA_SUCCESS) {
        return st;
    }
    switch (alg) {
#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    case PSA_ALG_ECB_NO_PADDING:
        mode = HAL_SEC_AES_V2_ECB;
        iv_length = 0;
        break;
#endif
#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
    case PSA_ALG_CBC_NO_PADDING:
        mode = HAL_SEC_AES_V2_CBC;
        break;
#endif
#if defined(PSA_WANT_ALG_CTR)
    case PSA_ALG_CTR:
        mode = HAL_SEC_AES_V2_CTR;
        dir = HAL_SEC_AES_V2_ENCRYPT;
        break;
#endif
    default:
        (void)key_buffer;
        (void)dir;
        (void)iv_length;
        return PSA_ERROR_NOT_SUPPORTED;
    }
    memset(operation, 0, sizeof(*operation));
    operation->mode = (uint8_t)mode;
    operation->iv_length = iv_length;
    operation->blen = mode == HAL_SEC_AES_V2_CTR ? IPRO_SEC_BLOCK : 0;
    return ipro_sec_status(hal_sec_aes_v2_setkey(&operation->aes, mode, dir,
                                                 key_buffer,
                                                 key_buffer_size));
}

psa_status_t ipro_sec_cipher_encrypt_setup(
    ipro_sec_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg)
{
    return cipher_setup(operation, attributes, key_buffer, key_buffer_size,
                        alg, HAL_SEC_AES_V2_ENCRYPT);
}

psa_status_t ipro_sec_cipher_decrypt_setup(
    ipro_sec_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg)
{
    return cipher_setup(operation, attributes, key_buffer, key_buffer_size,
                        alg, HAL_SEC_AES_V2_DECRYPT);
}

psa_status_t ipro_sec_cipher_set_iv(ipro_sec_cipher_operation_t *operation,
                                    const uint8_t *iv, size_t iv_length)
{
    if (iv_length != operation->iv_length || iv_length == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return ipro_sec_status(hal_sec_aes_v2_set_iv(&operation->aes, iv));
}

/* CTR: keystream left in buf first, then whole blocks, then one more */
static psa_status_t cipher_ctr(ipro_sec_cipher_operation_t *operation,
                               const uint8_t *input, size_t input_length,
                               uint8_t *output, size_t *output_length)
{
    size_t n;
    int ret = HAL_SEC_V2_OK;

    while (input_length && operation->blen < IPRO_SEC_BLOCK) {
        *output++ = *input++ ^ operation->buf[operation->blen++];
        input_length--;
        (*output_length)++;
    }
    n = input_length & ~(size_t)(IPRO_SEC_BLOCK - 1);
    if (n) {
        ret = hal_sec_aes_v2_crypt(&operation->aes, input, output, n);
        input += n;
        output += n;
        input_length -= n;
        *output_length += n;
    }
    if (ret == HAL_SEC_V2_OK && input_length) {
        memset(operation->buf, 0, sizeof(operation->buf));
        ret = hal_sec_aes_v2_crypt(&operation->aes, operation->buf,
                                   operation->buf, IPRO_SEC_BLOCK);
        operation->blen = 0;
        while (ret == HAL_SEC_V2_OK && input_length) {
            *output++ = *input++ ^ operation->buf[operation->blen++];
            input_length--;
            (*output_length)++;
        }
    }
    return ipro_sec_status(ret);
}

/* ECB, CBC: whole blocks out, a partial block kept in buf */
static psa_status_t cipher_blocks(ipro_sec_cipher_operation_t *operation,
                                  const uint8_t *input, size_t input_length,
                                  uint8_t *output, size_t *output_length)
{
    size_t n;
    int ret = HAL_SEC_V2_OK;

    if (operation->blen) {
        size_t take = IPRO_SEC_BLOCK - operation->blen;

        if (take > input_length) {
            take = input_length;
        }
        memcpy(operation->buf + operation->blen, input, take);
        operation->blen += (uint8_t)take;
        input += take;
        input_length -= take;
        if (operation->blen == IPRO_SEC_BLOCK) {
            ret = hal_sec_aes_v2_crypt(&operation->aes, operation->buf,
                                       output, IPRO_SEC_BLOCK);
            output += IPRO_SEC_BLOCK;
            *output_length += IPRO_SEC_BLOCK;
            operation->blen = 0;
        }
    }
    n = input_length & ~(size_t)(IPRO_SEC_BLOCK - 1);
    if (ret == HAL_SEC_V2_OK && n) {
        ret = hal_sec_aes_v2_crypt(&operation->aes, input, output, n);
        input += n;
        input_length -= n;
        *output_length += n;
    }
    if (ret == HAL_SEC_V2_OK && input_length) {
        memcpy(operation->buf, input, input_length);
        operation->blen = (uint8_t)input_length;
    }
    return ipro_sec_status(ret);
}

psa_status_t ipro_sec_cipher_update(ipro_sec_cipher_operation_t *operation,
                                    const uint8_t *input,
                                    size_t input_length, uint8_t *output,
                                    size_t output_size,
                                    size_t *output_length)
{
    int ctr = operation->mode == HAL_SEC_AES_V2_CTR;
    size_t expected = ctr ? input_length :
                            (operation->blen + input_length) /
                                IPRO_SEC_BLOCK * IPRO_SEC_BLOCK;

    *output_length = 0;
    if (output_size < expected) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    if (input_length == 0) {
        return PSA_SUCCESS;
    }
    /* a buffered partial block shifts output against input */
    if (!ctr && operation->blen && expected &&
        output < input + input_length && input < output + expected) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return ctr ? cipher_ctr(operation, input, input_length, output,
                            output_length) :
                 cipher_blocks(operation, input, input_length, output,
                               output_length);
}

psa_status_t ipro_sec_cipher_finish(ipro_sec_cipher_operation_t *operation,
                                    uint8_t *output, size_t output_size,
                                    size_t *output_length)
{
    (void)output;
    (void)output_size;
    *output_length = 0;
    if (operation->mode != HAL_SEC_AES_V2_CTR && operation->blen != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return PSA_SUCCESS;
}

psa_status_t ipro_sec_cipher_abort(ipro_sec_cipher_operation_t *operation)
{
    hal_sec_v2_zeroize(operation, sizeof(*operation));
    return PSA_SUCCESS;
}

/* setup done; iv set when iv_length != 0; update + finish into output */
static psa_status_t cipher_oneshot(ipro_sec_cipher_operation_t *op,
                                   const uint8_t *iv, size_t iv_length,
                                   const uint8_t *input,
                                   size_t input_length, uint8_t *output,
                                   size_t output_size, size_t *output_length)
{
    size_t n = 0;
    size_t m = 0;
    psa_status_t st = PSA_SUCCESS;

    if (iv_length > 0) {
        st = ipro_sec_cipher_set_iv(op, iv, iv_length);
    }
    if (st == PSA_SUCCESS) {
        st = ipro_sec_cipher_update(op, input, input_length, output,
                                    output_size, &n);
    }
    if (st == PSA_SUCCESS) {
        st = ipro_sec_cipher_finish(op, output + n, output_size - n, &m);
    }
    if (st == PSA_SUCCESS) {
        *output_length = n + m;
    }
    ipro_sec_cipher_abort(op);
    return st;
}

psa_status_t ipro_sec_cipher_encrypt(const psa_key_attributes_t *attributes,
                                     const uint8_t *key_buffer,
                                     size_t key_buffer_size,
                                     psa_algorithm_t alg,
                                     const uint8_t *iv, size_t iv_length,
                                     const uint8_t *input,
                                     size_t input_length,
                                     uint8_t *output, size_t output_size,
                                     size_t *output_length)
{
    ipro_sec_cipher_operation_t op;
    psa_status_t st = ipro_sec_cipher_encrypt_setup(&op, attributes,
                                                    key_buffer,
                                                    key_buffer_size, alg);

    if (st != PSA_SUCCESS) {
        return st;
    }
    return cipher_oneshot(&op, iv, iv_length, input, input_length, output,
                          output_size, output_length);
}

psa_status_t ipro_sec_cipher_decrypt(const psa_key_attributes_t *attributes,
                                     const uint8_t *key_buffer,
                                     size_t key_buffer_size,
                                     psa_algorithm_t alg,
                                     const uint8_t *input,
                                     size_t input_length,
                                     uint8_t *output, size_t output_size,
                                     size_t *output_length)
{
    ipro_sec_cipher_operation_t op;
    size_t iv_length;
    psa_status_t st = ipro_sec_cipher_decrypt_setup(&op, attributes,
                                                    key_buffer,
                                                    key_buffer_size, alg);

    if (st != PSA_SUCCESS) {
        return st;
    }
    iv_length = op.iv_length;
    if (input_length < iv_length) {
        ipro_sec_cipher_abort(&op);
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return cipher_oneshot(&op, input, iv_length, input + iv_length,
                          input_length - iv_length, output, output_size,
                          output_length);
}
