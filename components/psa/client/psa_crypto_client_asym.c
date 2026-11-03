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

/*
 * NSPE PSA Crypto client: AEAD, signatures, asymmetric encryption, key
 * derivation and key agreement. Multipart AEAD runs in the SPE on
 * mbedtls 3.6; on 2.28 those functions answer here without a call.
 */
#include <string.h>

#include "psa_crypto_client_priv.h"

static psa_status_t aead_one(int32_t fn, mbedtls_svc_key_id_t key,
                             psa_algorithm_t alg, const uint8_t *nonce,
                             size_t nonce_length, const uint8_t *ad,
                             size_t ad_length, const uint8_t *input,
                             size_t input_length, uint8_t *output,
                             size_t output_size, size_t *output_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(key, alg, 0);
    psa_invec d[3] = {
        { nonce, nonce_length }, { ad, ad_length }, { input, input_length },
    };

    return psa_client_crypto_call(fn, &r, d, 3, output, output_size,
                                  output_length, NULL);
}

psa_status_t psa_aead_encrypt(mbedtls_svc_key_id_t key, psa_algorithm_t alg,
                              const uint8_t *nonce, size_t nonce_length,
                              const uint8_t *additional_data,
                              size_t additional_data_length,
                              const uint8_t *plaintext,
                              size_t plaintext_length, uint8_t *ciphertext,
                              size_t ciphertext_size,
                              size_t *ciphertext_length)
{
    return aead_one(CRYPTO_FN_AEAD_ENCRYPT, key, alg, nonce, nonce_length,
                    additional_data, additional_data_length, plaintext,
                    plaintext_length, ciphertext, ciphertext_size,
                    ciphertext_length);
}

psa_status_t psa_aead_decrypt(mbedtls_svc_key_id_t key, psa_algorithm_t alg,
                              const uint8_t *nonce, size_t nonce_length,
                              const uint8_t *additional_data,
                              size_t additional_data_length,
                              const uint8_t *ciphertext,
                              size_t ciphertext_length, uint8_t *plaintext,
                              size_t plaintext_size,
                              size_t *plaintext_length)
{
    return aead_one(CRYPTO_FN_AEAD_DECRYPT, key, alg, nonce, nonce_length,
                    additional_data, additional_data_length, ciphertext,
                    ciphertext_length, plaintext, plaintext_size,
                    plaintext_length);
}

#if PSA_IPRO_AEAD_MULTI
psa_status_t psa_aead_encrypt_setup(psa_aead_operation_t *operation,
                                    mbedtls_svc_key_id_t key,
                                    psa_algorithm_t alg)
{
    return psa_client_crypto_setup(CRYPTO_FN_AEAD_ENCRYPT_SETUP, key, alg,
                                   &PSA_CLIENT_H(operation));
}

psa_status_t psa_aead_decrypt_setup(psa_aead_operation_t *operation,
                                    mbedtls_svc_key_id_t key,
                                    psa_algorithm_t alg)
{
    return psa_client_crypto_setup(CRYPTO_FN_AEAD_DECRYPT_SETUP, key, alg,
                                   &PSA_CLIENT_H(operation));
}

psa_status_t psa_aead_generate_nonce(psa_aead_operation_t *operation,
                                     uint8_t *nonce, size_t nonce_size,
                                     size_t *nonce_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0,
                                                    PSA_CLIENT_H(operation));

    return psa_client_crypto_call(CRYPTO_FN_AEAD_GENERATE_NONCE, &r, NULL, 0,
                                  nonce, nonce_size, nonce_length, NULL);
}

psa_status_t psa_aead_set_nonce(psa_aead_operation_t *operation,
                                const uint8_t *nonce, size_t nonce_length)
{
    return psa_client_crypto_op_in(CRYPTO_FN_AEAD_SET_NONCE,
                                   PSA_CLIENT_H(operation), nonce,
                                   nonce_length);
}

/* Lengths above UINT32_MAX cannot occur on RV32 */
psa_status_t psa_aead_set_lengths(psa_aead_operation_t *operation,
                                  size_t ad_length, size_t plaintext_length)
{
    struct psa_crypto_aead_lengths l = { (uint32_t)ad_length,
                                         (uint32_t)plaintext_length };

    if (ad_length > UINT32_MAX || plaintext_length > UINT32_MAX) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return psa_client_crypto_op_in(CRYPTO_FN_AEAD_SET_LENGTHS,
                                   PSA_CLIENT_H(operation), &l, sizeof(l));
}

psa_status_t psa_aead_update_ad(psa_aead_operation_t *operation,
                                const uint8_t *input, size_t input_length)
{
    return psa_client_crypto_op_in(CRYPTO_FN_AEAD_UPDATE_AD,
                                   PSA_CLIENT_H(operation), input,
                                   input_length);
}

psa_status_t psa_aead_update(psa_aead_operation_t *operation,
                             const uint8_t *input, size_t input_length,
                             uint8_t *output, size_t output_size,
                             size_t *output_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0,
                                                    PSA_CLIENT_H(operation));
    psa_invec d = { input, input_length };

    return psa_client_crypto_call(CRYPTO_FN_AEAD_UPDATE, &r, &d, 1, output,
                                  output_size, output_length, NULL);
}

/*
 * One bounce of PSA_AEAD_FINISH_OUTPUT_MAX_SIZE ciphertext bytes and
 * PSA_AEAD_TAG_MAX_SIZE tag bytes; larger sizes are clamped to these
 */
psa_status_t psa_aead_finish(psa_aead_operation_t *operation,
                             uint8_t *ciphertext, size_t ciphertext_size,
                             size_t *ciphertext_length, uint8_t *tag,
                             size_t tag_size, size_t *tag_length)
{
    uint8_t b[PSA_AEAD_FINISH_OUTPUT_MAX_SIZE + PSA_AEAD_TAG_MAX_SIZE];
    size_t ct_cap = ciphertext_size < PSA_AEAD_FINISH_OUTPUT_MAX_SIZE
                        ? ciphertext_size : PSA_AEAD_FINISH_OUTPUT_MAX_SIZE;
    size_t tag_cap = tag_size < PSA_AEAD_TAG_MAX_SIZE
                         ? tag_size : PSA_AEAD_TAG_MAX_SIZE;
    struct psa_crypto_req r = psa_client_crypto_req(0, 0,
                                                    PSA_CLIENT_H(operation));
    struct psa_crypto_rsp rsp;
    size_t n = 0;
    psa_status_t st;

    r.aux = (uint32_t)ct_cap;
    st = psa_client_crypto_end_req(CRYPTO_FN_AEAD_FINISH,
                                   &PSA_CLIENT_H(operation), &r, NULL, 0, b,
                                   ct_cap + tag_cap, &n, &rsp);
    *ciphertext_length = 0;
    *tag_length = 0;
    if (st == PSA_SUCCESS && (n < ct_cap || rsp.length2 > ct_cap)) {
        st = PSA_ERROR_CORRUPTION_DETECTED;
    }
    if (st == PSA_SUCCESS) {
        memcpy(ciphertext, b, rsp.length2);
        memcpy(tag, b + ct_cap, n - ct_cap);
        *ciphertext_length = rsp.length2;
        *tag_length = n - ct_cap;
    }
    return st;
}

psa_status_t psa_aead_verify(psa_aead_operation_t *operation,
                             uint8_t *plaintext, size_t plaintext_size,
                             size_t *plaintext_length, const uint8_t *tag,
                             size_t tag_length)
{
    psa_invec d = { tag, tag_length };

    return psa_client_crypto_end(CRYPTO_FN_AEAD_VERIFY,
                                 &PSA_CLIENT_H(operation), &d, 1, plaintext,
                                 plaintext_size, plaintext_length);
}

psa_status_t psa_aead_abort(psa_aead_operation_t *operation)
{
    return psa_client_crypto_end(CRYPTO_FN_AEAD_ABORT,
                                 &PSA_CLIENT_H(operation), NULL, 0, NULL, 0,
                                 NULL);
}
#else
psa_status_t psa_aead_encrypt_setup(psa_aead_operation_t *operation,
                                    mbedtls_svc_key_id_t key,
                                    psa_algorithm_t alg)
{
    (void)operation;
    (void)key;
    (void)alg;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_decrypt_setup(psa_aead_operation_t *operation,
                                    mbedtls_svc_key_id_t key,
                                    psa_algorithm_t alg)
{
    (void)operation;
    (void)key;
    (void)alg;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_generate_nonce(psa_aead_operation_t *operation,
                                     uint8_t *nonce, size_t nonce_size,
                                     size_t *nonce_length)
{
    (void)operation;
    (void)nonce;
    (void)nonce_size;
    *nonce_length = 0;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_set_nonce(psa_aead_operation_t *operation,
                                const uint8_t *nonce, size_t nonce_length)
{
    (void)operation;
    (void)nonce;
    (void)nonce_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_set_lengths(psa_aead_operation_t *operation,
                                  size_t ad_length, size_t plaintext_length)
{
    (void)operation;
    (void)ad_length;
    (void)plaintext_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_update_ad(psa_aead_operation_t *operation,
                                const uint8_t *input, size_t input_length)
{
    (void)operation;
    (void)input;
    (void)input_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_update(psa_aead_operation_t *operation,
                             const uint8_t *input, size_t input_length,
                             uint8_t *output, size_t output_size,
                             size_t *output_length)
{
    (void)operation;
    (void)input;
    (void)input_length;
    (void)output;
    (void)output_size;
    *output_length = 0;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_finish(psa_aead_operation_t *operation,
                             uint8_t *ciphertext, size_t ciphertext_size,
                             size_t *ciphertext_length, uint8_t *tag,
                             size_t tag_size, size_t *tag_length)
{
    (void)operation;
    (void)ciphertext;
    (void)ciphertext_size;
    (void)tag;
    (void)tag_size;
    *ciphertext_length = 0;
    *tag_length = 0;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_verify(psa_aead_operation_t *operation,
                             uint8_t *plaintext, size_t plaintext_size,
                             size_t *plaintext_length, const uint8_t *tag,
                             size_t tag_length)
{
    (void)operation;
    (void)plaintext;
    (void)plaintext_size;
    (void)tag;
    (void)tag_length;
    *plaintext_length = 0;
    return PSA_ERROR_NOT_SUPPORTED;
}

/* PSA_SUCCESS; no AEAD operation is ever set up here */
psa_status_t psa_aead_abort(psa_aead_operation_t *operation)
{
    (void)operation;
    return PSA_SUCCESS;
}
#endif

/* An id of key, alg, n data vectors and an output (o may be NULL) */
static psa_status_t key_alg_call(int32_t fn, mbedtls_svc_key_id_t key,
                                 psa_algorithm_t alg, const psa_invec *d,
                                 size_t n, uint8_t *o, size_t o_size,
                                 size_t *o_len)
{
    struct psa_crypto_req r = psa_client_crypto_req(key, alg, 0);

    return psa_client_crypto_call(fn, &r, d, n, o, o_size, o_len, NULL);
}

psa_status_t psa_sign_hash(mbedtls_svc_key_id_t key, psa_algorithm_t alg,
                           const uint8_t *hash, size_t hash_length,
                           uint8_t *signature, size_t signature_size,
                           size_t *signature_length)
{
    psa_invec d = { hash, hash_length };

    return key_alg_call(CRYPTO_FN_SIGN_HASH, key, alg, &d, 1, signature,
                        signature_size, signature_length);
}

psa_status_t psa_verify_hash(mbedtls_svc_key_id_t key, psa_algorithm_t alg,
                             const uint8_t *hash, size_t hash_length,
                             const uint8_t *signature,
                             size_t signature_length)
{
    psa_invec d[2] = { { hash, hash_length },
                       { signature, signature_length } };

    return key_alg_call(CRYPTO_FN_VERIFY_HASH, key, alg, d, 2, NULL, 0,
                        NULL);
}

psa_status_t psa_sign_message(mbedtls_svc_key_id_t key, psa_algorithm_t alg,
                              const uint8_t *input, size_t input_length,
                              uint8_t *signature, size_t signature_size,
                              size_t *signature_length)
{
    psa_invec d = { input, input_length };

    return key_alg_call(CRYPTO_FN_SIGN_MESSAGE, key, alg, &d, 1, signature,
                        signature_size, signature_length);
}

psa_status_t psa_verify_message(mbedtls_svc_key_id_t key,
                                psa_algorithm_t alg, const uint8_t *input,
                                size_t input_length,
                                const uint8_t *signature,
                                size_t signature_length)
{
    psa_invec d[2] = { { input, input_length },
                       { signature, signature_length } };

    return key_alg_call(CRYPTO_FN_VERIFY_MESSAGE, key, alg, d, 2, NULL, 0,
                        NULL);
}

psa_status_t psa_asymmetric_encrypt(mbedtls_svc_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t *input,
                                    size_t input_length,
                                    const uint8_t *salt, size_t salt_length,
                                    uint8_t *output, size_t output_size,
                                    size_t *output_length)
{
    psa_invec d[2] = { { input, input_length }, { salt, salt_length } };

    return key_alg_call(CRYPTO_FN_ASYM_ENCRYPT, key, alg, d, 2, output,
                        output_size, output_length);
}

psa_status_t psa_asymmetric_decrypt(mbedtls_svc_key_id_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t *input,
                                    size_t input_length,
                                    const uint8_t *salt, size_t salt_length,
                                    uint8_t *output, size_t output_size,
                                    size_t *output_length)
{
    psa_invec d[2] = { { input, input_length }, { salt, salt_length } };

    return key_alg_call(CRYPTO_FN_ASYM_DECRYPT, key, alg, d, 2, output,
                        output_size, output_length);
}

psa_status_t psa_raw_key_agreement(psa_algorithm_t alg,
                                   mbedtls_svc_key_id_t private_key,
                                   const uint8_t *peer_key,
                                   size_t peer_key_length, uint8_t *output,
                                   size_t output_size, size_t *output_length)
{
    psa_invec d = { peer_key, peer_key_length };

    return key_alg_call(CRYPTO_FN_RAW_KEY_AGREEMENT, private_key, alg, &d, 1,
                        output, output_size, output_length);
}

psa_status_t psa_key_derivation_setup(
    psa_key_derivation_operation_t *operation, psa_algorithm_t alg)
{
    psa_client_handle_t h = PSA_CLIENT_KD_H(operation);
    psa_status_t st = psa_client_crypto_setup(CRYPTO_FN_KD_SETUP, 0, alg, &h);

    PSA_CLIENT_KD_H(operation) = h;
    return st;
}

psa_status_t psa_key_derivation_get_capacity(
    const psa_key_derivation_operation_t *operation, size_t *capacity)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0, PSA_CLIENT_KD_H(operation));
    struct psa_crypto_rsp rsp;
    psa_status_t st = psa_client_crypto_call(CRYPTO_FN_KD_GET_CAPACITY, &r,
                                             NULL, 0, NULL, 0, NULL, &rsp);

    *capacity = rsp.length;
    return st;
}

psa_status_t psa_key_derivation_set_capacity(
    psa_key_derivation_operation_t *operation, size_t capacity)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0, PSA_CLIENT_KD_H(operation));

    r.aux = (uint32_t)capacity;
    return psa_client_crypto_call(CRYPTO_FN_KD_SET_CAPACITY, &r, NULL, 0, NULL,
                                  0, NULL, NULL);
}

/* An id of step (and key) on the KD handle, n data vectors */
static psa_status_t kd_step_call(int32_t fn,
                                 const psa_key_derivation_operation_t *op,
                                 psa_key_derivation_step_t step,
                                 mbedtls_svc_key_id_t key,
                                 const psa_invec *d, size_t n)
{
    struct psa_crypto_req r = psa_client_crypto_req(key, 0, PSA_CLIENT_KD_H(op));

    r.step = step;
    return psa_client_crypto_call(fn, &r, d, n, NULL, 0, NULL, NULL);
}

psa_status_t psa_key_derivation_input_bytes(
    psa_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step, const uint8_t *data, size_t data_length)
{
    psa_invec d = { data, data_length };

    return kd_step_call(CRYPTO_FN_KD_INPUT_BYTES, operation, step, 0, &d, 1);
}

#if PSA_IPRO_KD_INTEGER
psa_status_t psa_key_derivation_input_integer(
    psa_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step, uint64_t value)
{
    psa_invec d = { &value, sizeof(value) };

    return kd_step_call(CRYPTO_FN_KD_INPUT_INTEGER, operation, step, 0, &d,
                        1);
}
#endif

#if PSA_IPRO_API_1_2
psa_status_t psa_key_agreement(mbedtls_svc_key_id_t private_key,
                               const uint8_t *peer_key,
                               size_t peer_key_length, psa_algorithm_t alg,
                               const psa_key_attributes_t *attributes,
                               mbedtls_svc_key_id_t *key)
{
    struct psa_crypto_req r = psa_client_crypto_req(private_key, alg, 0);
    psa_invec d = { peer_key, peer_key_length };
    struct psa_crypto_rsp rsp;
    psa_status_t st;

    psa_crypto_attr_pack(&r.attr, attributes);
    st = psa_client_crypto_call(CRYPTO_FN_KEY_AGREEMENT, &r, &d, 1, NULL, 0,
                                NULL, &rsp);
    *key = rsp.key_id;
    return st;
}
#endif

psa_status_t psa_key_derivation_input_key(
    psa_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step, mbedtls_svc_key_id_t key)
{
    return kd_step_call(CRYPTO_FN_KD_INPUT_KEY, operation, step, key, NULL,
                        0);
}

psa_status_t psa_key_derivation_key_agreement(
    psa_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step, mbedtls_svc_key_id_t private_key,
    const uint8_t *peer_key, size_t peer_key_length)
{
    psa_invec d = { peer_key, peer_key_length };

    return kd_step_call(CRYPTO_FN_KD_KEY_AGREEMENT, operation, step,
                        private_key, &d, 1);
}

/* One call: output_length above the bounce limit is refused */
psa_status_t psa_key_derivation_output_bytes(
    psa_key_derivation_operation_t *operation, uint8_t *output,
    size_t output_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0, PSA_CLIENT_KD_H(operation));

    return psa_client_crypto_call(CRYPTO_FN_KD_OUTPUT_BYTES, &r, NULL, 0,
                                  output, output_length, NULL, NULL);
}

psa_status_t psa_key_derivation_output_key(
    const psa_key_attributes_t *attributes,
    psa_key_derivation_operation_t *operation, mbedtls_svc_key_id_t *key)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0, PSA_CLIENT_KD_H(operation));
    struct psa_crypto_rsp rsp;
    psa_status_t st;

    psa_crypto_attr_pack(&r.attr, attributes);
    st = psa_client_crypto_call(CRYPTO_FN_KD_OUTPUT_KEY, &r, NULL, 0, NULL, 0,
                                NULL, &rsp);
    *key = rsp.key_id;
    return st;
}

psa_status_t psa_key_derivation_abort(
    psa_key_derivation_operation_t *operation)
{
    psa_client_handle_t h = PSA_CLIENT_KD_H(operation);
    psa_status_t st = psa_client_crypto_end(CRYPTO_FN_KD_ABORT, &h, NULL, 0,
                                            NULL, 0, NULL);

    PSA_CLIENT_KD_H(operation) = h;
    return st;
}
