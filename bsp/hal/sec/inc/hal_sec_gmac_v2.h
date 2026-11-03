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

/* SEC_ENG HAL v2: GHASH (the GMAC block), for AES-GCM. */
#ifndef HAL_SEC_GMAC_V2_H
#define HAL_SEC_GMAC_V2_H

#include <stddef.h>
#include <stdint.h>
#include "hal_sec_common_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Plain data. */
typedef struct {
    uint8_t h[16];
    uint8_t x[16];
    uint8_t buf[16];
    uint8_t blen;
    uint8_t reserved[3];
} hal_sec_ghash_v2_ctx_t;

int hal_sec_ghash_v2_setup(hal_sec_ghash_v2_ctx_t *ctx, const uint8_t h[16]);
/* Partial blocks are kept until 16 bytes or hal_sec_ghash_v2_pad(). */
int hal_sec_ghash_v2_update(hal_sec_ghash_v2_ctx_t *ctx, const uint8_t *in,
                            size_t len);
/* Zero-pads and absorbs a pending partial block. */
int hal_sec_ghash_v2_pad(hal_sec_ghash_v2_ctx_t *ctx);
/* Pads, writes X and wipes ctx. */
int hal_sec_ghash_v2_finish(hal_sec_ghash_v2_ctx_t *ctx, uint8_t out[16]);
void hal_sec_ghash_v2_abort(hal_sec_ghash_v2_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif
