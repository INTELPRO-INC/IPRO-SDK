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
 * SEC_ENG PSA transparent accelerator driver (ipro_sec), called by the
 * driver wrappers of mbedtls 3.6 and TF-PSA-Crypto 1.2 when
 * CONFIG_PSA_SEC_ENG_ACCEL is set. PSA_ERROR_NOT_SUPPORTED hands the
 * call to the built-in software driver.
 */
#ifndef IPRO_SEC_DRIVER_H
#define IPRO_SEC_DRIVER_H

#include "psa/crypto_driver_common.h"
#include "ipro_sec_driver_contexts.h"

psa_status_t ipro_sec_init(void);

psa_status_t ipro_sec_hash_compute(psa_algorithm_t alg,
                                   const uint8_t *input, size_t input_length,
                                   uint8_t *hash, size_t hash_size,
                                   size_t *hash_length);
psa_status_t ipro_sec_hash_setup(ipro_sec_hash_operation_t *operation,
                                 psa_algorithm_t alg);
psa_status_t ipro_sec_hash_clone(const ipro_sec_hash_operation_t *source,
                                 ipro_sec_hash_operation_t *target);
psa_status_t ipro_sec_hash_update(ipro_sec_hash_operation_t *operation,
                                  const uint8_t *input, size_t input_length);
psa_status_t ipro_sec_hash_finish(ipro_sec_hash_operation_t *operation,
                                  uint8_t *hash, size_t hash_size,
                                  size_t *hash_length);
psa_status_t ipro_sec_hash_abort(ipro_sec_hash_operation_t *operation);

psa_status_t ipro_sec_cipher_encrypt(const psa_key_attributes_t *attributes,
                                     const uint8_t *key_buffer,
                                     size_t key_buffer_size,
                                     psa_algorithm_t alg,
                                     const uint8_t *iv, size_t iv_length,
                                     const uint8_t *input,
                                     size_t input_length,
                                     uint8_t *output, size_t output_size,
                                     size_t *output_length);
psa_status_t ipro_sec_cipher_decrypt(const psa_key_attributes_t *attributes,
                                     const uint8_t *key_buffer,
                                     size_t key_buffer_size,
                                     psa_algorithm_t alg,
                                     const uint8_t *input,
                                     size_t input_length,
                                     uint8_t *output, size_t output_size,
                                     size_t *output_length);
psa_status_t ipro_sec_cipher_encrypt_setup(
    ipro_sec_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg);
psa_status_t ipro_sec_cipher_decrypt_setup(
    ipro_sec_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg);
psa_status_t ipro_sec_cipher_set_iv(ipro_sec_cipher_operation_t *operation,
                                    const uint8_t *iv, size_t iv_length);
psa_status_t ipro_sec_cipher_update(ipro_sec_cipher_operation_t *operation,
                                    const uint8_t *input,
                                    size_t input_length, uint8_t *output,
                                    size_t output_size,
                                    size_t *output_length);
psa_status_t ipro_sec_cipher_finish(ipro_sec_cipher_operation_t *operation,
                                    uint8_t *output, size_t output_size,
                                    size_t *output_length);
psa_status_t ipro_sec_cipher_abort(ipro_sec_cipher_operation_t *operation);

psa_status_t ipro_sec_aead_encrypt(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *plaintext,
    size_t plaintext_length, uint8_t *ciphertext, size_t ciphertext_size,
    size_t *ciphertext_length);
psa_status_t ipro_sec_aead_decrypt(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *ciphertext,
    size_t ciphertext_length, uint8_t *plaintext, size_t plaintext_size,
    size_t *plaintext_length);
psa_status_t ipro_sec_aead_encrypt_setup(
    ipro_sec_aead_operation_t *operation,
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg);
psa_status_t ipro_sec_aead_decrypt_setup(
    ipro_sec_aead_operation_t *operation,
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg);
psa_status_t ipro_sec_aead_set_nonce(ipro_sec_aead_operation_t *operation,
                                     const uint8_t *nonce,
                                     size_t nonce_length);
psa_status_t ipro_sec_aead_set_lengths(ipro_sec_aead_operation_t *operation,
                                       size_t ad_length,
                                       size_t plaintext_length);
psa_status_t ipro_sec_aead_update_ad(ipro_sec_aead_operation_t *operation,
                                     const uint8_t *input,
                                     size_t input_length);
psa_status_t ipro_sec_aead_update(ipro_sec_aead_operation_t *operation,
                                  const uint8_t *input, size_t input_length,
                                  uint8_t *output, size_t output_size,
                                  size_t *output_length);
psa_status_t ipro_sec_aead_finish(ipro_sec_aead_operation_t *operation,
                                  uint8_t *ciphertext,
                                  size_t ciphertext_size,
                                  size_t *ciphertext_length, uint8_t *tag,
                                  size_t tag_size, size_t *tag_length);
psa_status_t ipro_sec_aead_verify(ipro_sec_aead_operation_t *operation,
                                  uint8_t *plaintext, size_t plaintext_size,
                                  size_t *plaintext_length,
                                  const uint8_t *tag, size_t tag_length);
psa_status_t ipro_sec_aead_abort(ipro_sec_aead_operation_t *operation);


psa_status_t ipro_sec_sign_hash(const psa_key_attributes_t *attributes,
                                const uint8_t *key_buffer,
                                size_t key_buffer_size, psa_algorithm_t alg,
                                const uint8_t *hash, size_t hash_length,
                                uint8_t *signature, size_t signature_size,
                                size_t *signature_length);
psa_status_t ipro_sec_verify_hash(const psa_key_attributes_t *attributes,
                                  const uint8_t *key_buffer,
                                  size_t key_buffer_size,
                                  psa_algorithm_t alg, const uint8_t *hash,
                                  size_t hash_length,
                                  const uint8_t *signature,
                                  size_t signature_length);
psa_status_t ipro_sec_key_agreement(const psa_key_attributes_t *attributes,
                                    const uint8_t *key_buffer,
                                    size_t key_buffer_size,
                                    psa_algorithm_t alg,
                                    const uint8_t *peer_key,
                                    size_t peer_key_length,
                                    uint8_t *shared_secret,
                                    size_t shared_secret_size,
                                    size_t *shared_secret_length);
psa_status_t ipro_sec_generate_key(const psa_key_attributes_t *attributes,
                                   uint8_t *key_buffer,
                                   size_t key_buffer_size,
                                   size_t *key_buffer_length);
psa_status_t ipro_sec_export_public_key(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, uint8_t *data, size_t data_size,
    size_t *data_length);

#endif
