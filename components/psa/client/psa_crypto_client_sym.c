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
 * NSPE PSA Crypto client: hash, MAC and unauthenticated cipher, single
 * part and multipart. Multipart operations live in SPE op slots; the
 * client operation keeps the slot handle in id.
 */
#include "psa_crypto_client_priv.h"

psa_status_t psa_hash_compute(psa_algorithm_t alg, const uint8_t *input,
                              size_t input_length, uint8_t *hash,
                              size_t hash_size, size_t *hash_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, alg, 0);
    psa_invec d = { input, input_length };

    return psa_client_crypto_call(CRYPTO_FN_HASH_COMPUTE, &r, &d, 1, hash,
                                  hash_size, hash_length, NULL);
}

psa_status_t psa_hash_compare(psa_algorithm_t alg, const uint8_t *input,
                              size_t input_length, const uint8_t *hash,
                              size_t hash_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, alg, 0);
    psa_invec d[2] = { { input, input_length }, { hash, hash_length } };

    return psa_client_crypto_call(CRYPTO_FN_HASH_COMPARE, &r, d, 2, NULL, 0,
                                  NULL, NULL);
}

psa_status_t psa_hash_setup(psa_hash_operation_t *operation,
                            psa_algorithm_t alg)
{
    return psa_client_crypto_setup(CRYPTO_FN_HASH_SETUP, 0, alg,
                                   &PSA_CLIENT_H(operation));
}

psa_status_t psa_hash_update(psa_hash_operation_t *operation,
                             const uint8_t *input, size_t input_length)
{
    return psa_client_crypto_op_in(CRYPTO_FN_HASH_UPDATE, PSA_CLIENT_H(operation), input,
                                   input_length);
}

psa_status_t psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash, size_t hash_size,
                             size_t *hash_length)
{
    return psa_client_crypto_end(CRYPTO_FN_HASH_FINISH, &PSA_CLIENT_H(operation), NULL,
                                 0, hash, hash_size, hash_length);
}

psa_status_t psa_hash_verify(psa_hash_operation_t *operation,
                             const uint8_t *hash, size_t hash_length)
{
    psa_invec d = { hash, hash_length };

    return psa_client_crypto_end(CRYPTO_FN_HASH_VERIFY, &PSA_CLIENT_H(operation), &d, 1,
                                 NULL, 0, NULL);
}

psa_status_t psa_hash_abort(psa_hash_operation_t *operation)
{
    return psa_client_crypto_end(CRYPTO_FN_HASH_ABORT, &PSA_CLIENT_H(operation), NULL, 0,
                                 NULL, 0, NULL);
}

/* A new SPE slot copied from the source; active target: BAD_STATE */
psa_status_t psa_hash_clone(const psa_hash_operation_t *source_operation,
                            psa_hash_operation_t *target_operation)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0,
                                                    PSA_CLIENT_H(source_operation));
    struct psa_crypto_rsp rsp;
    psa_status_t st;

    if (PSA_CLIENT_H(target_operation) != 0) {
        return PSA_ERROR_BAD_STATE;
    }
    st = psa_client_crypto_call(CRYPTO_FN_HASH_CLONE, &r, NULL, 0, NULL, 0,
                                NULL, &rsp);
    if (st == PSA_SUCCESS) {
        PSA_CLIENT_H(target_operation) = rsp.op_handle;
    }
    return st;
}

psa_status_t psa_mac_compute(mbedtls_svc_key_id_t key, psa_algorithm_t alg,
                             const uint8_t *input, size_t input_length,
                             uint8_t *mac, size_t mac_size,
                             size_t *mac_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(key, alg, 0);
    psa_invec d = { input, input_length };

    return psa_client_crypto_call(CRYPTO_FN_MAC_COMPUTE, &r, &d, 1, mac,
                                  mac_size, mac_length, NULL);
}

psa_status_t psa_mac_verify(mbedtls_svc_key_id_t key, psa_algorithm_t alg,
                            const uint8_t *input, size_t input_length,
                            const uint8_t *mac, size_t mac_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(key, alg, 0);
    psa_invec d[2] = { { input, input_length }, { mac, mac_length } };

    return psa_client_crypto_call(CRYPTO_FN_MAC_VERIFY, &r, d, 2, NULL, 0,
                                  NULL, NULL);
}

psa_status_t psa_mac_sign_setup(psa_mac_operation_t *operation,
                                mbedtls_svc_key_id_t key,
                                psa_algorithm_t alg)
{
    return psa_client_crypto_setup(CRYPTO_FN_MAC_SIGN_SETUP, key, alg,
                                   &PSA_CLIENT_H(operation));
}

psa_status_t psa_mac_verify_setup(psa_mac_operation_t *operation,
                                  mbedtls_svc_key_id_t key,
                                  psa_algorithm_t alg)
{
    return psa_client_crypto_setup(CRYPTO_FN_MAC_VERIFY_SETUP, key, alg,
                                   &PSA_CLIENT_H(operation));
}

psa_status_t psa_mac_update(psa_mac_operation_t *operation,
                            const uint8_t *input, size_t input_length)
{
    return psa_client_crypto_op_in(CRYPTO_FN_MAC_UPDATE, PSA_CLIENT_H(operation), input,
                                   input_length);
}

psa_status_t psa_mac_sign_finish(psa_mac_operation_t *operation,
                                 uint8_t *mac, size_t mac_size,
                                 size_t *mac_length)
{
    return psa_client_crypto_end(CRYPTO_FN_MAC_SIGN_FINISH, &PSA_CLIENT_H(operation),
                                 NULL, 0, mac, mac_size, mac_length);
}

psa_status_t psa_mac_verify_finish(psa_mac_operation_t *operation,
                                   const uint8_t *mac, size_t mac_length)
{
    psa_invec d = { mac, mac_length };

    return psa_client_crypto_end(CRYPTO_FN_MAC_VERIFY_FINISH, &PSA_CLIENT_H(operation),
                                 &d, 1, NULL, 0, NULL);
}

psa_status_t psa_mac_abort(psa_mac_operation_t *operation)
{
    return psa_client_crypto_end(CRYPTO_FN_MAC_ABORT, &PSA_CLIENT_H(operation), NULL, 0,
                                 NULL, 0, NULL);
}

static psa_status_t cipher_one(int32_t fn, mbedtls_svc_key_id_t key,
                               psa_algorithm_t alg, const uint8_t *input,
                               size_t input_length, uint8_t *output,
                               size_t output_size, size_t *output_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(key, alg, 0);
    psa_invec d = { input, input_length };

    return psa_client_crypto_call(fn, &r, &d, 1, output, output_size,
                                  output_length, NULL);
}

psa_status_t psa_cipher_encrypt(mbedtls_svc_key_id_t key,
                                psa_algorithm_t alg, const uint8_t *input,
                                size_t input_length, uint8_t *output,
                                size_t output_size, size_t *output_length)
{
    return cipher_one(CRYPTO_FN_CIPHER_ENCRYPT, key, alg, input,
                      input_length, output, output_size, output_length);
}

psa_status_t psa_cipher_decrypt(mbedtls_svc_key_id_t key,
                                psa_algorithm_t alg, const uint8_t *input,
                                size_t input_length, uint8_t *output,
                                size_t output_size, size_t *output_length)
{
    return cipher_one(CRYPTO_FN_CIPHER_DECRYPT, key, alg, input,
                      input_length, output, output_size, output_length);
}

psa_status_t psa_cipher_encrypt_setup(psa_cipher_operation_t *operation,
                                      mbedtls_svc_key_id_t key,
                                      psa_algorithm_t alg)
{
    return psa_client_crypto_setup(CRYPTO_FN_CIPHER_ENCRYPT_SETUP, key, alg,
                                   &PSA_CLIENT_H(operation));
}

psa_status_t psa_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                      mbedtls_svc_key_id_t key,
                                      psa_algorithm_t alg)
{
    return psa_client_crypto_setup(CRYPTO_FN_CIPHER_DECRYPT_SETUP, key, alg,
                                   &PSA_CLIENT_H(operation));
}

psa_status_t psa_cipher_generate_iv(psa_cipher_operation_t *operation,
                                    uint8_t *iv, size_t iv_size,
                                    size_t *iv_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0, PSA_CLIENT_H(operation));

    return psa_client_crypto_call(CRYPTO_FN_CIPHER_GENERATE_IV, &r, NULL, 0,
                                  iv, iv_size, iv_length, NULL);
}

psa_status_t psa_cipher_set_iv(psa_cipher_operation_t *operation,
                               const uint8_t *iv, size_t iv_length)
{
    return psa_client_crypto_op_in(CRYPTO_FN_CIPHER_SET_IV, PSA_CLIENT_H(operation), iv,
                                   iv_length);
}

psa_status_t psa_cipher_update(psa_cipher_operation_t *operation,
                               const uint8_t *input, size_t input_length,
                               uint8_t *output, size_t output_size,
                               size_t *output_length)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0, PSA_CLIENT_H(operation));
    psa_invec d = { input, input_length };

    return psa_client_crypto_call(CRYPTO_FN_CIPHER_UPDATE, &r, &d, 1, output,
                                  output_size, output_length, NULL);
}

psa_status_t psa_cipher_finish(psa_cipher_operation_t *operation,
                               uint8_t *output, size_t output_size,
                               size_t *output_length)
{
    return psa_client_crypto_end(CRYPTO_FN_CIPHER_FINISH, &PSA_CLIENT_H(operation), NULL,
                                 0, output, output_size, output_length);
}

psa_status_t psa_cipher_abort(psa_cipher_operation_t *operation)
{
    return psa_client_crypto_end(CRYPTO_FN_CIPHER_ABORT, &PSA_CLIENT_H(operation), NULL,
                                 0, NULL, 0, NULL);
}
