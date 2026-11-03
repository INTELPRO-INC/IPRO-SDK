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
 * PSA_WANT_* the SPE serves on mbedtls 3.6 or TF-PSA-Crypto; the SPE
 * derives its drivers from it, the NSPE client its size macros.
 */
#ifndef PSA_IPRO_CRYPTO_FEATURES3_H
#define PSA_IPRO_CRYPTO_FEATURES3_H

#define PSA_WANT_ALG_CBC_NO_PADDING 1
#define PSA_WANT_ALG_CBC_PKCS7 1
#define PSA_WANT_ALG_CCM 1
#define PSA_WANT_ALG_CCM_STAR_NO_TAG 1
#define PSA_WANT_ALG_CMAC 1
#define PSA_WANT_ALG_CTR 1
#define PSA_WANT_ALG_DETERMINISTIC_ECDSA 1
#define PSA_WANT_ALG_ECB_NO_PADDING 1
#define PSA_WANT_ALG_ECDH 1
#define PSA_WANT_ALG_ECDSA 1
#define PSA_WANT_ALG_GCM 1
#define PSA_WANT_ALG_HKDF 1
#define PSA_WANT_ALG_HKDF_EXPAND 1
#define PSA_WANT_ALG_HKDF_EXTRACT 1
#define PSA_WANT_ALG_HMAC 1
#define PSA_WANT_ALG_PBKDF2_HMAC 1
#define PSA_WANT_ALG_RSA_OAEP 1
#define PSA_WANT_ALG_RSA_PKCS1V15_CRYPT 1
#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN 1
#define PSA_WANT_ALG_RSA_PSS 1
#define PSA_WANT_ALG_SHA_1 1
#define PSA_WANT_ALG_SHA_224 1
#define PSA_WANT_ALG_SHA_256 1
#define PSA_WANT_ALG_SHA_384 1
#define PSA_WANT_ALG_SHA_512 1
#define PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS 1
#define PSA_WANT_ALG_TLS12_PRF 1
#define PSA_WANT_ALG_TLS12_PSK_TO_MS 1
#define PSA_WANT_ECC_MONTGOMERY_255 1
#define PSA_WANT_ECC_SECP_R1_256 1
#define PSA_WANT_ECC_SECP_R1_384 1
#define PSA_WANT_ECC_SECP_R1_521 1
#define PSA_WANT_KEY_TYPE_AES 1
#define PSA_WANT_KEY_TYPE_DERIVE 1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE 1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT 1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT 1
#define PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY 1
#define PSA_WANT_KEY_TYPE_HMAC 1
#define PSA_WANT_KEY_TYPE_PASSWORD 1
#define PSA_WANT_KEY_TYPE_PASSWORD_HASH 1
#define PSA_WANT_KEY_TYPE_RAW_DATA 1
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT 1
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE 1
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT 1
#define PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY 1

/* PSA Crypto API 1.2 PAKE: TF-PSA-Crypto only */
#if defined(TF_PSA_CRYPTO_CONFIG_VERSION)
#define PSA_WANT_ALG_JPAKE 1
#endif

/* Synonyms of ECDSA and RSA_PKCS1V15_SIGN; TF-PSA-Crypto derives them */
#if !defined(TF_PSA_CRYPTO_CONFIG_VERSION)
#define PSA_WANT_ALG_ECDSA_ANY 1
#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN_RAW 1
#endif

#endif /* PSA_IPRO_CRYPTO_FEATURES3_H */
