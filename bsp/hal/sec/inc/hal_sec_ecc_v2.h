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
 * SEC_ENG HAL v2: P-256 (secp256r1) on the PKA. Integers are 32-byte
 * big-endian; points are 64 bytes X || Y (affine, uncompressed).
 */
#ifndef HAL_SEC_ECC_V2_H
#define HAL_SEC_ECC_V2_H

#include <stddef.h>
#include <stdint.h>
#include "hal_sec_common_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* OK when x, y < p and y^2 = x^3 - 3x + b; else ERR_PARAM. */
int hal_sec_p256_v2_check_point(const uint8_t q[64]);

/* OK when 1 <= d <= n - 1; else ERR_PARAM. */
int hal_sec_p256_v2_check_scalar(const uint8_t d[32]);

/* Q = dG. Fixed PKA sequence for every d. */
int hal_sec_p256_v2_pubkey(const uint8_t d[32], uint8_t q[64]);

/* Random d in [1, n - 1] from the TRNG, and Q = dG. */
int hal_sec_p256_v2_keygen(uint8_t d[32], uint8_t q[64]);

/*
 * Randomized ECDSA. hash is truncated to its leftmost 256 bits or
 * left-padded, then reduced mod n. sig = r || s.
 */
int hal_sec_p256_v2_sign(const uint8_t d[32], const uint8_t *hash,
                         size_t hash_len, uint8_t sig[64]);

/* OK for a valid signature, ERR_VERIFY otherwise. Variable time. */
int hal_sec_p256_v2_verify(const uint8_t q[64], const uint8_t *hash,
                           size_t hash_len, const uint8_t sig[64]);

/* x = x(d * peer). The peer point is validated first. */
int hal_sec_p256_v2_ecdh(const uint8_t d[32], const uint8_t peer[64],
                         uint8_t x[32]);

#ifdef __cplusplus
}
#endif

#endif
