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

/* SEC_ENG HAL v2: AES-128/192/256 ECB, CBC, CTR; XTS-AES-128/256. */
#ifndef HAL_SEC_AES_V2_H
#define HAL_SEC_AES_V2_H

#include <stddef.h>
#include <stdint.h>
#include "hal_sec_common_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CTR32: only the low 32 counter bits advance (GCM inc32). */
enum hal_sec_aes_v2_mode {
    HAL_SEC_AES_V2_ECB,
    HAL_SEC_AES_V2_CTR,
    HAL_SEC_AES_V2_CBC,
    HAL_SEC_AES_V2_CTR32
};

enum hal_sec_aes_v2_dir {
    HAL_SEC_AES_V2_ENCRYPT,
    HAL_SEC_AES_V2_DECRYPT
};

/*
 * Plain data. keylen 0 = hardware key hw_sel (eFuse KEY_SEL) with
 * SE_AES_SBOOT key source hw_src.
 */
typedef struct {
    uint8_t key[32];
    uint8_t iv[16];
    uint8_t keylen;
    uint8_t hw_sel;
    uint8_t hw_keylen;
    uint8_t mode;
    uint8_t dir;
    uint8_t hw_src;
    uint8_t reserved[2];
} hal_sec_aes_v2_ctx_t;

/* keylen 16, 24 or 32 bytes. */
int hal_sec_aes_v2_setkey(hal_sec_aes_v2_ctx_t *ctx, unsigned int mode,
                          unsigned int dir, const uint8_t *key,
                          size_t keylen);
/* Hardware key: sel 0-3, keylen 16 or 32, key source 0. */
int hal_sec_aes_v2_setkey_hw(hal_sec_aes_v2_ctx_t *ctx, unsigned int mode,
                             unsigned int dir, unsigned int sel,
                             size_t keylen);
/* SE_AES_SBOOT key source (0 or 1) of the hardware key in ctx. */
int hal_sec_aes_v2_set_hw_src(hal_sec_aes_v2_ctx_t *ctx, unsigned int src);
/* CBC: IV; CTR, CTR32: big-endian counter block. */
int hal_sec_aes_v2_set_iv(hal_sec_aes_v2_ctx_t *ctx, const uint8_t iv[16]);
/*
 * len % 16 == 0; in and out may be the same buffer. The IV / counter in
 * ctx advances, so consecutive calls continue one stream.
 */
int hal_sec_aes_v2_crypt(hal_sec_aes_v2_ctx_t *ctx, const uint8_t *in,
                         uint8_t *out, size_t len);
void hal_sec_aes_v2_abort(hal_sec_aes_v2_ctx_t *ctx);

/*
 * XTS-AES (IEEE P1619), plain keys: key = key1 (data) || key2 (tweak).
 * unit: data unit sequence number, 16 bytes little-endian as in P1619.
 */
typedef struct {
    uint8_t key[64];
    uint8_t unit[16];
    uint8_t tweak[16];
    uint32_t unit_len;
    uint32_t blk;
    uint8_t half;
    uint8_t dir;
    uint8_t tweak_ok;
    uint8_t reserved;
} hal_sec_xts_v2_ctx_t;

/* keylen 32 (XTS-AES-128) or 64 (XTS-AES-256). */
int hal_sec_xts_v2_setkey(hal_sec_xts_v2_ctx_t *ctx, unsigned int dir,
                          const uint8_t *key, size_t keylen);
/* Starts at data unit unit; unit_len % 16 == 0, 16 to 1048560 bytes. */
int hal_sec_xts_v2_set_unit(hal_sec_xts_v2_ctx_t *ctx,
                            const uint8_t unit[16], size_t unit_len);
/*
 * len % 16 == 0 (no ciphertext stealing); in and out may be the same
 * buffer. Consecutive calls continue the stream; unit + 1 per unit_len.
 */
int hal_sec_xts_v2_crypt(hal_sec_xts_v2_ctx_t *ctx, const uint8_t *in,
                         uint8_t *out, size_t len);
void hal_sec_xts_v2_abort(hal_sec_xts_v2_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif
