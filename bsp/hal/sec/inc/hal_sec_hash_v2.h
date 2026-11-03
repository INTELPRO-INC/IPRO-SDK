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

/* SEC_ENG HAL v2: SHA-1/224/256/384/512. */
#ifndef HAL_SEC_HASH_V2_H
#define HAL_SEC_HASH_V2_H

#include <stddef.h>
#include <stdint.h>
#include "hal_sec_common_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

enum hal_sec_hash_v2_type {
    HAL_SEC_HASH_V2_SHA1,
    HAL_SEC_HASH_V2_SHA224,
    HAL_SEC_HASH_V2_SHA256,
    HAL_SEC_HASH_V2_SHA384,
    HAL_SEC_HASH_V2_SHA512,
    HAL_SEC_HASH_V2_TYPES
};

/* Plain data: copy it to clone an operation. */
typedef struct {
    uint64_t total;
    uint8_t state[64];
    uint8_t buf[128];
    uint8_t type;
    uint8_t blen;
    uint8_t started;
    uint8_t reserved;
} hal_sec_hash_v2_ctx_t;

/* Digest length in bytes, 0 for an unknown type. */
size_t hal_sec_hash_v2_digest_len(unsigned int type);

int hal_sec_hash_v2_setup(hal_sec_hash_v2_ctx_t *ctx, unsigned int type);
int hal_sec_hash_v2_update(hal_sec_hash_v2_ctx_t *ctx, const uint8_t *in,
                           size_t len);
/* Writes the digest and wipes ctx; *out_len = digest length. */
int hal_sec_hash_v2_finish(hal_sec_hash_v2_ctx_t *ctx, uint8_t *out,
                           size_t out_size, size_t *out_len);
void hal_sec_hash_v2_abort(hal_sec_hash_v2_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif
