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
 * The SHA-256 legacy module under the 2.28 names: mbedtls 3.6 and
 * TF-PSA-Crypto name the *_ret functions without the suffix (int
 * return as well); TF-PSA-Crypto declares them under mbedtls/private.
 */
#pragma once

#include "psa_ipro/crypto_lib.h"

#if PSA_IPRO_LEGACY_PRIVATE
#ifndef MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS
#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS
#endif
#include "mbedtls/private/sha256.h"
#else
#include "mbedtls/sha256.h"
#endif

#if !PSA_IPRO_SHA256_RET
#define mbedtls_sha256_ret        mbedtls_sha256
#define mbedtls_sha256_starts_ret mbedtls_sha256_starts
#define mbedtls_sha256_update_ret mbedtls_sha256_update
#define mbedtls_sha256_finish_ret mbedtls_sha256_finish
#endif
