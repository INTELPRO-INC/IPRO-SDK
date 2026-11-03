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

#include <string.h>
#include "hal_sec_aes_v2.h"
#include "hal_sec_v2_priv.h"

#define AES_V2_CFG     64u
#define AES_V2_EN      (1u << 2)
#define AES_V2_BE      0x1Fu
#define AES_V2_HW_KEY  (1u << 7)
#define AES_V2_INT_CLR (1u << 9)

/* link cfg {ctrl, src, dst, iv[4], key[8]} then data, whole cache lines */
static uint32_t aes_v2_scratch[(AES_V2_CFG + SEC_V2_CHUNK) / 4]
    __attribute__((aligned(32)));

static int aes_v2_keymode(size_t keylen, uint32_t *mode)
{
    switch (keylen) {
    case 16:
        *mode = 0;
        return HAL_SEC_V2_OK;
    case 32:
        *mode = 1;
        return HAL_SEC_V2_OK;
    case 24:
        *mode = 2;
        return HAL_SEC_V2_OK;
    default:
        return HAL_SEC_V2_ERR_PARAM;
    }
}

static int aes_v2_init(hal_sec_aes_v2_ctx_t *ctx, unsigned int mode,
                       unsigned int dir)
{
    if (!ctx || mode > HAL_SEC_AES_V2_CTR32 ||
        dir > HAL_SEC_AES_V2_DECRYPT) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    hal_sec_v2_zeroize(ctx, sizeof(*ctx));
    ctx->mode = (uint8_t)mode;
    ctx->dir = (uint8_t)dir;
    return HAL_SEC_V2_OK;
}

int hal_sec_aes_v2_setkey(hal_sec_aes_v2_ctx_t *ctx, unsigned int mode,
                          unsigned int dir, const uint8_t *key,
                          size_t keylen)
{
    uint32_t km = 0;

    if (!key || aes_v2_keymode(keylen, &km) != HAL_SEC_V2_OK ||
        aes_v2_init(ctx, mode, dir) != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    memcpy(ctx->key, key, keylen);
    ctx->keylen = (uint8_t)keylen;
    return HAL_SEC_V2_OK;
}

int hal_sec_aes_v2_setkey_hw(hal_sec_aes_v2_ctx_t *ctx, unsigned int mode,
                             unsigned int dir, unsigned int sel,
                             size_t keylen)
{
    if (sel > 3 || (keylen != 16 && keylen != 32) ||
        aes_v2_init(ctx, mode, dir) != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    ctx->hw_sel = (uint8_t)sel;
    ctx->hw_keylen = (uint8_t)keylen;
    return HAL_SEC_V2_OK;
}

int hal_sec_aes_v2_set_hw_src(hal_sec_aes_v2_ctx_t *ctx, unsigned int src)
{
    if (!ctx || ctx->keylen || !ctx->hw_keylen || src > 1) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    ctx->hw_src = (uint8_t)src;
    return HAL_SEC_V2_OK;
}

int hal_sec_aes_v2_set_iv(hal_sec_aes_v2_ctx_t *ctx, const uint8_t iv[16])
{
    if (!ctx || !iv) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    memcpy(ctx->iv, iv, 16);
    return HAL_SEC_V2_OK;
}

static uint32_t aes_v2_hw_mode(unsigned int mode)
{
    return mode == HAL_SEC_AES_V2_CTR32 ? HAL_SEC_AES_V2_CTR : mode;
}

/* blocks left before the low 32 bits of the counter wrap */
static size_t aes_v2_ctr_room(const uint8_t ctr[16])
{
    uint32_t lo = ((uint32_t)ctr[12] << 24) | ((uint32_t)ctr[13] << 16) |
                  ((uint32_t)ctr[14] << 8) | ctr[15];
    uint64_t room = 0x100000000ull - lo;

    return room > SIZE_MAX ? SIZE_MAX : (size_t)room;
}

static void aes_v2_ctr_add(uint8_t ctr[16], size_t nblk, int last)
{
    uint64_t carry = nblk;

    for (int i = 15; i >= last && carry; i--) {
        carry += ctr[i];
        ctr[i] = (uint8_t)carry;
        carry >>= 8;
    }
}

/*
 * n bytes, n % 16 == 0. direct: the engine reads in and writes out in
 * place, n <= SEC_V2_DIRECT_MAX; else it reads in place when
 * sec_v2_direct(in, n), else from the scratch, and writes the scratch,
 * n <= SEC_V2_CHUNK.
 */
static int aes_v2_run(hal_sec_aes_v2_ctx_t *ctx, const uint8_t *in,
                      uint8_t *out, size_t n, int direct)
{
    uint8_t *s = (uint8_t *)aes_v2_scratch;
    uint32_t *cfg = aes_v2_scratch;
    uint8_t *data = direct ? out : s + AES_V2_CFG;
    size_t used = direct ? AES_V2_CFG : AES_V2_CFG + n;
    size_t nblk = n / 16;
    uint8_t next_iv[16];
    uint32_t km = 0;
    int ret;

    aes_v2_keymode(ctx->keylen ? ctx->keylen : ctx->hw_keylen, &km);
    if (direct || sec_v2_direct(in, n)) {
        sec_v2_flush(in, n);
        cfg[1] = sec_v2_dma(in);
    } else {
        memcpy(data, in, n);
        cfg[1] = sec_v2_dma(data);
    }
    if (ctx->mode == HAL_SEC_AES_V2_CBC &&
        ctx->dir == HAL_SEC_AES_V2_DECRYPT) {
        memcpy(next_iv, in + n - 16, 16);
    }
    cfg[0] = (km << 3) | ((uint32_t)ctx->dir << 5) |
             (ctx->keylen ? 0 : AES_V2_HW_KEY) | AES_V2_INT_CLR |
             (aes_v2_hw_mode(ctx->mode) << 12) | ((uint32_t)nblk << 16);
    cfg[2] = sec_v2_dma(data);
    memcpy(&cfg[3], ctx->iv, 16);
    memcpy(&cfg[7], ctx->key, 32);
    if (!ctx->keylen) {
        sec_v2_wr(SEC_V2_AES_KEY_SEL0, ctx->hw_sel);
        sec_v2_wr(SEC_V2_AES_KEY_SEL1, ctx->hw_sel);
        sec_v2_wr(SEC_V2_AES_SBOOT,
                  (sec_v2_rd(SEC_V2_AES_SBOOT) & ~1u) | ctx->hw_src);
    }
    sec_v2_wr(SEC_V2_AES_ENDIAN, AES_V2_BE);
    sec_v2_flush(s, used);
    if (direct) {
        sec_v2_flush(out, n);
    }
    ret = sec_v2_job(SEC_V2_AES_CTRL, SEC_V2_AES_LINK,
                     AES_V2_EN | SEC_V2_LINK_MODE, 0, sec_v2_dma(cfg));
    sec_v2_flush(s, used);
    if (direct) {
        sec_v2_flush(out, n);
    }
    if (ret == HAL_SEC_V2_OK) {
        if (ctx->mode == HAL_SEC_AES_V2_CBC) {
            if (ctx->dir == HAL_SEC_AES_V2_ENCRYPT) {
                memcpy(next_iv, data + n - 16, 16);
            }
            memcpy(ctx->iv, next_iv, 16);
        } else if (ctx->mode == HAL_SEC_AES_V2_CTR) {
            aes_v2_ctr_add(ctx->iv, nblk, 0);
        } else if (ctx->mode == HAL_SEC_AES_V2_CTR32) {
            aes_v2_ctr_add(ctx->iv, nblk, 12);
        }
        if (!direct) {
            memcpy(out, data, n);
        }
    }
    hal_sec_v2_zeroize(next_iv, sizeof(next_iv));
    hal_sec_v2_zeroize(s, used);
    sec_v2_flush(s, used);
    return ret;
}

int hal_sec_aes_v2_crypt(hal_sec_aes_v2_ctx_t *ctx, const uint8_t *in,
                         uint8_t *out, size_t len)
{
    int ret = HAL_SEC_V2_OK;

    if (!ctx || (!ctx->keylen && !ctx->hw_keylen) || len % 16 ||
        ((!in || !out) && len)) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    if (sec_v2_lock(HAL_SEC_V2_AES) != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_TIMEOUT;
    }
    while (len && ret == HAL_SEC_V2_OK) {
        size_t n = len < SEC_V2_DIRECT_MAX ? len : SEC_V2_DIRECT_MAX;
        int direct;

        n = sec_v2_direct(in, n) ? sec_v2_direct_out(out, n) : 0;
        if (!n) {
            n = len < SEC_V2_CHUNK ? len : SEC_V2_CHUNK;
        }
        if ((ctx->mode == HAL_SEC_AES_V2_CTR ||
             ctx->mode == HAL_SEC_AES_V2_CTR32) &&
            n / 16 > aes_v2_ctr_room(ctx->iv)) {
            n = aes_v2_ctr_room(ctx->iv) * 16;
        }
        direct = sec_v2_direct(in, n) && sec_v2_direct_out(out, n) == n;
        if (!direct && n > SEC_V2_CHUNK) {
            n = SEC_V2_CHUNK;
        }
        ret = aes_v2_run(ctx, in, out, n, direct);
        in += n;
        out += n;
        len -= n;
    }
    sec_v2_unlock(HAL_SEC_V2_AES);
    return ret;
}

void hal_sec_aes_v2_abort(hal_sec_aes_v2_ctx_t *ctx)
{
    if (ctx) {
        hal_sec_v2_zeroize(ctx, sizeof(*ctx));
    }
}
