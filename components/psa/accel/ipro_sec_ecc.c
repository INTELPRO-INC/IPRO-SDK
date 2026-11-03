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
 * ipro_sec driver: P-256 randomized ECDSA sign, ECDSA verify, ECDH,
 * key generation and public key export on the SEC_ENG PKA. Other
 * curves, deterministic ECDSA and compressed points fall back.
 */
#include <string.h>

#include "psa/crypto.h"
#include "hal_sec_common_v2.h"
#include "hal_sec_ecc_v2.h"
#include "ipro_sec_driver.h"
#include "ipro_sec_priv.h"

#define P256_BYTES 32u
#define P256_POINT (2u * P256_BYTES)
#define P256_PUB   (1u + P256_POINT)

#if defined(PSA_WANT_ECC_SECP_R1_256)
#define P256_PAIR   PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)
#define P256_PUBLIC PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1)

/* 1 when attributes and buffer are a P-256 key pair (d only) */
static int p256_pair(const psa_key_attributes_t *attributes,
                     size_t key_buffer_size)
{
    return psa_get_key_type(attributes) == P256_PAIR &&
           psa_get_key_bits(attributes) == 256 &&
           key_buffer_size == P256_BYTES;
}

/* X || Y of an uncompressed point 0x04 || X || Y, or NULL */
static const uint8_t *p256_point(const uint8_t *data, size_t length)
{
    return length == P256_PUB && data[0] == 0x04 ? data + 1 : NULL;
}
#endif

psa_status_t ipro_sec_sign_hash(const psa_key_attributes_t *attributes,
                                const uint8_t *key_buffer,
                                size_t key_buffer_size, psa_algorithm_t alg,
                                const uint8_t *hash, size_t hash_length,
                                uint8_t *signature, size_t signature_size,
                                size_t *signature_length)
{
#if defined(PSA_WANT_ECC_SECP_R1_256) && defined(PSA_WANT_ALG_ECDSA)
    int ret;

    if (!PSA_ALG_IS_RANDOMIZED_ECDSA(alg) || hash_length == 0 ||
        !p256_pair(attributes, key_buffer_size)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    *signature_length = 0;
    if (signature_size < P256_POINT) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    ret = hal_sec_p256_v2_sign(key_buffer, hash, hash_length, signature);
    if (ret != HAL_SEC_V2_OK) {
        hal_sec_v2_zeroize(signature, P256_POINT);
        return ipro_sec_status(ret);
    }
    *signature_length = P256_POINT;
    return PSA_SUCCESS;
#else
    (void)attributes;
    (void)key_buffer;
    (void)key_buffer_size;
    (void)alg;
    (void)hash;
    (void)hash_length;
    (void)signature;
    (void)signature_size;
    (void)signature_length;
    return PSA_ERROR_NOT_SUPPORTED;
#endif
}

psa_status_t ipro_sec_verify_hash(const psa_key_attributes_t *attributes,
                                  const uint8_t *key_buffer,
                                  size_t key_buffer_size,
                                  psa_algorithm_t alg, const uint8_t *hash,
                                  size_t hash_length,
                                  const uint8_t *signature,
                                  size_t signature_length)
{
#if defined(PSA_WANT_ECC_SECP_R1_256) && \
    (defined(PSA_WANT_ALG_ECDSA) || defined(PSA_WANT_ALG_DETERMINISTIC_ECDSA))
    uint8_t q[P256_POINT];
    const uint8_t *pub;
    int ret;

    if (!PSA_ALG_IS_ECDSA(alg) || hash_length == 0 ||
        psa_get_key_bits(attributes) != 256) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (psa_get_key_type(attributes) == P256_PUBLIC) {
        pub = p256_point(key_buffer, key_buffer_size);
        if (!pub) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
    } else if (p256_pair(attributes, key_buffer_size)) {
        ret = hal_sec_p256_v2_pubkey(key_buffer, q);
        if (ret != HAL_SEC_V2_OK) {
            return ipro_sec_status(ret);
        }
        pub = q;
    } else {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (signature_length != P256_POINT) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }
    ret = hal_sec_p256_v2_verify(pub, hash, hash_length, signature);
    return ret == HAL_SEC_V2_ERR_PARAM ? PSA_ERROR_INVALID_SIGNATURE :
                                         ipro_sec_status(ret);
#else
    (void)attributes;
    (void)key_buffer;
    (void)key_buffer_size;
    (void)alg;
    (void)hash;
    (void)hash_length;
    (void)signature;
    (void)signature_length;
    return PSA_ERROR_NOT_SUPPORTED;
#endif
}

psa_status_t ipro_sec_key_agreement(const psa_key_attributes_t *attributes,
                                    const uint8_t *key_buffer,
                                    size_t key_buffer_size,
                                    psa_algorithm_t alg,
                                    const uint8_t *peer_key,
                                    size_t peer_key_length,
                                    uint8_t *shared_secret,
                                    size_t shared_secret_size,
                                    size_t *shared_secret_length)
{
#if defined(PSA_WANT_ECC_SECP_R1_256) && defined(PSA_WANT_ALG_ECDH)
    const uint8_t *peer = p256_point(peer_key, peer_key_length);
    int ret;

    if (alg != PSA_ALG_ECDH || !peer ||
        !p256_pair(attributes, key_buffer_size)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    *shared_secret_length = 0;
    if (shared_secret_size < P256_BYTES) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    ret = hal_sec_p256_v2_ecdh(key_buffer, peer, shared_secret);
    if (ret != HAL_SEC_V2_OK) {
        hal_sec_v2_zeroize(shared_secret, shared_secret_size);
        return ipro_sec_status(ret);
    }
    *shared_secret_length = P256_BYTES;
    return PSA_SUCCESS;
#else
    (void)attributes;
    (void)key_buffer;
    (void)key_buffer_size;
    (void)alg;
    (void)peer_key;
    (void)peer_key_length;
    (void)shared_secret;
    (void)shared_secret_size;
    (void)shared_secret_length;
    return PSA_ERROR_NOT_SUPPORTED;
#endif
}

psa_status_t ipro_sec_generate_key(const psa_key_attributes_t *attributes,
                                   uint8_t *key_buffer,
                                   size_t key_buffer_size,
                                   size_t *key_buffer_length)
{
#if defined(PSA_WANT_ECC_SECP_R1_256) && \
    defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE)
    uint8_t q[P256_POINT];
    int ret;

    if (psa_get_key_type(attributes) != P256_PAIR ||
        psa_get_key_bits(attributes) != 256) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    *key_buffer_length = 0;
    if (key_buffer_size < P256_BYTES) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    ret = hal_sec_p256_v2_keygen(key_buffer, q);
    hal_sec_v2_zeroize(q, sizeof(q));
    if (ret != HAL_SEC_V2_OK) {
        hal_sec_v2_zeroize(key_buffer, key_buffer_size);
        return ipro_sec_status(ret);
    }
    *key_buffer_length = P256_BYTES;
    return PSA_SUCCESS;
#else
    (void)attributes;
    (void)key_buffer;
    (void)key_buffer_size;
    (void)key_buffer_length;
    return PSA_ERROR_NOT_SUPPORTED;
#endif
}

psa_status_t ipro_sec_export_public_key(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, uint8_t *data, size_t data_size,
    size_t *data_length)
{
#if defined(PSA_WANT_ECC_SECP_R1_256)
    int ret;

    if (!p256_pair(attributes, key_buffer_size)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    *data_length = 0;
    if (data_size < P256_PUB) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    ret = hal_sec_p256_v2_pubkey(key_buffer, data + 1);
    if (ret != HAL_SEC_V2_OK) {
        return ipro_sec_status(ret);
    }
    data[0] = 0x04;
    *data_length = P256_PUB;
    return PSA_SUCCESS;
#else
    (void)attributes;
    (void)key_buffer;
    (void)key_buffer_size;
    (void)data;
    (void)data_size;
    (void)data_length;
    return PSA_ERROR_NOT_SUPPORTED;
#endif
}
