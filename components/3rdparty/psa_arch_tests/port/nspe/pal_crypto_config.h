/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

/* Crypto suite features the IPRO7 SPE serves (its mbedtls config is
 * apps/security/ipro7_psa_spe/mbedtls_app_config.h); README.md lists
 * every ARCH_TEST_* flag left undefined and why.
 */

#ifndef _PAL_CRYPTO_CONFIG_H_
#define _PAL_CRYPTO_CONFIG_H_

/* Key types */
#define ARCH_TEST_AES
#define ARCH_TEST_AES_128
#define ARCH_TEST_AES_192
#define ARCH_TEST_AES_256
#define ARCH_TEST_RAW

/* Ciphers: AES-CTR, AES-CBC without padding and with PKCS#7 */
#define ARCH_TEST_CIPHER
#define ARCH_TEST_CIPHER_MODE_CTR
#define ARCH_TEST_CIPHER_MODE_CBC
#define ARCH_TEST_CTR_AES
#define ARCH_TEST_CBC_AES
#define ARCH_TEST_CBC_AES_NO_PADDING
#define ARCH_TEST_CBC_NO_PADDING
#define ARCH_TEST_CBC_PKCS7

/* Hashes and HMAC */
#define ARCH_TEST_HASH
#define ARCH_TEST_HMAC
#define ARCH_TEST_SHA1
#define ARCH_TEST_SHA224
#define ARCH_TEST_SHA256
#define ARCH_TEST_SHA384
#define ARCH_TEST_SHA512

/* Key derivation: HKDF and the TLS 1.2 PRF */
#define ARCH_TEST_HKDF
#define ARCH_TEST_TLS12_PRF

/* MACs and one-shot AEADs (no multipart AEAD in mbedtls 2.28) */
#define ARCH_TEST_CMAC
#define ARCH_TEST_GMAC
#define ARCH_TEST_CCM
#define ARCH_TEST_GCM
#define ARCH_TEST_TRUNCATED_MAC
#ifdef PSA_ARCH_MBEDTLS_V3
#define ARCH_TEST_PBKDF2
#define ARCH_TEST_RSA
#define ARCH_TEST_PKCS1V15
#define ARCH_TEST_RSA_1024
#define ARCH_TEST_RSA_2048
#define ARCH_TEST_RSA_PKCS1V15_CRYPT
#define ARCH_TEST_RSA_PKCS1V15_SIGN
#define ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#define ARCH_TEST_RSA_OAEP
#else
#define ARCH_TEST_AEAD_MULTISTAGE_OFF
#endif
#ifdef PSA_ARCH_API_1_2
#define ARCH_TEST_JPAKE
#endif

/* ECC P-256 and P-384: ECDH, ECDSA, deterministic ECDSA */
#define ARCH_TEST_ECC
#define ARCH_TEST_ECC_CURVE_SECP256R1
#define ARCH_TEST_ECC_CURVE_SECP384R1
#define ARCH_TEST_ECDH
#define ARCH_TEST_ECDSA
#define ARCH_TEST_DETERMINISTIC_ECDSA
#define ARCH_TEST_ECC_ASYMMETRIC_API_SUPPORT

#include "pal_crypto_config_check.h"

#endif /* _PAL_CRYPTO_CONFIG_H_ */
