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
 * The PSA Crypto library of the build: mbedtls 2.28, mbedtls 3.6 or
 * TF-PSA-Crypto 1.x (TF_PSA_CRYPTO_CONFIG_FILE set), and what of the
 * RPC each one serves. The services and the client test these only.
 */
#pragma once

#if defined(TF_PSA_CRYPTO_CONFIG_FILE)
#include "tf-psa-crypto/build_info.h"
#define PSA_IPRO_CRYPTO_LIB 4
#else
#include "mbedtls/version.h"
#define PSA_IPRO_CRYPTO_LIB MBEDTLS_VERSION_MAJOR
#endif

/* client-mode op structs hold a handle field (2.28: id, KD alg) */
#define PSA_IPRO_CLIENT_HANDLE      (PSA_IPRO_CRYPTO_LIB >= 3)
/* psa_{set,get}_key_domain_parameters exist */
#define PSA_IPRO_DOMAIN_PARAMS      (PSA_IPRO_CRYPTO_LIB < 3)
/* AEAD_ENCRYPT_SETUP..AEAD_ABORT */
#define PSA_IPRO_AEAD_MULTI         (PSA_IPRO_CRYPTO_LIB >= 3)
/* KD_INPUT_INTEGER */
#define PSA_IPRO_KD_INTEGER         (PSA_IPRO_CRYPTO_LIB >= 3)
/* KEY_AGREEMENT, PAKE_SETUP..PAKE_ABORT */
#define PSA_IPRO_API_1_2            (PSA_IPRO_CRYPTO_LIB >= 4)
/* SHA-256 legacy calls named mbedtls_sha256_*_ret */
#define PSA_IPRO_SHA256_RET         (PSA_IPRO_CRYPTO_LIB < 3)
/* legacy modules declared under mbedtls/private/ */
#define PSA_IPRO_LEGACY_PRIVATE     (PSA_IPRO_CRYPTO_LIB >= 4)
