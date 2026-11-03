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
#include "hal_sec_hash_v2.h"
#include "hal_sec_v2_priv.h"

#define SHA_V2_CFG      96u
#define SHA_V2_EN       (1u << 5)
#define SHA_V2_HASH_SEL (1u << 6)
#define SHA_V2_CFG_INT  ((1u << 9) | (1u << 10))

struct sha_v2_info {
    uint8_t mode;
    uint8_t block;
    uint8_t digest;
};

static const struct sha_v2_info sha_v2_info[HAL_SEC_HASH_V2_TYPES] = {
    [HAL_SEC_HASH_V2_SHA1] = { 2, 64, 20 },
    [HAL_SEC_HASH_V2_SHA224] = { 1, 64, 28 },
    [HAL_SEC_HASH_V2_SHA256] = { 0, 64, 32 },
    [HAL_SEC_HASH_V2_SHA384] = { 5, 128, 48 },
    [HAL_SEC_HASH_V2_SHA512] = { 4, 128, 64 },
};

/* link cfg {ctrl, src, result[16]} then data, whole cache lines */
static uint32_t sha_v2_scratch[(SHA_V2_CFG + SEC_V2_CHUNK) / 4]
    __attribute__((aligned(32)));

size_t hal_sec_hash_v2_digest_len(unsigned int type)
{
    return type < HAL_SEC_HASH_V2_TYPES ? sha_v2_info[type].digest : 0;
}

static const struct sha_v2_info *sha_v2_get(const hal_sec_hash_v2_ctx_t *ctx)
{
    if (!ctx || ctx->type >= HAL_SEC_HASH_V2_TYPES) {
        return NULL;
    }
    return &sha_v2_info[ctx->type];
}

/*
 * nblk whole blocks. The engine reads in place when sec_v2_direct()
 * (nblk * block <= SEC_V2_DIRECT_MAX), else from a copy in the scratch
 * (nblk * block <= SEC_V2_CHUNK).
 */
static int sha_v2_run(hal_sec_hash_v2_ctx_t *ctx,
                      const struct sha_v2_info *in_info, const uint8_t *in,
                      size_t nblk)
{
    uint8_t *s = (uint8_t *)sha_v2_scratch;
    uint32_t *cfg = sha_v2_scratch;
    size_t n = nblk * in_info->block;
    size_t used = SHA_V2_CFG;
    int ret;

    if (sec_v2_direct(in, n)) {
        sec_v2_flush(in, n);
    } else {
        memcpy(s + SHA_V2_CFG, in, n);
        in = s + SHA_V2_CFG;
        used += n;
    }
    cfg[0] = ((uint32_t)in_info->mode << 2) | SHA_V2_CFG_INT |
             ((uint32_t)nblk << 16) | (ctx->started ? SHA_V2_HASH_SEL : 0);
    cfg[1] = sec_v2_dma(in);
    memcpy(&cfg[2], ctx->state, sizeof(ctx->state));
    sec_v2_flush(s, used);
    ret = sec_v2_job(SEC_V2_SHA_CTRL, SEC_V2_SHA_LINK,
                     SHA_V2_EN | SEC_V2_LINK_MODE, SHA_V2_HASH_SEL,
                     sec_v2_dma(cfg));
    sec_v2_flush(s, SHA_V2_CFG);
    if (ret == HAL_SEC_V2_OK) {
        memcpy(ctx->state, &cfg[2], sizeof(ctx->state));
        ctx->started = 1;
    }
    hal_sec_v2_zeroize(s, used);
    sec_v2_flush(s, used);
    return ret;
}

int hal_sec_hash_v2_setup(hal_sec_hash_v2_ctx_t *ctx, unsigned int type)
{
    if (!ctx || type >= HAL_SEC_HASH_V2_TYPES) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    memset(ctx, 0, sizeof(*ctx));
    ctx->type = (uint8_t)type;
    return HAL_SEC_V2_OK;
}

static int sha_v2_update(hal_sec_hash_v2_ctx_t *ctx,
                         const struct sha_v2_info *info, const uint8_t *in,
                         size_t len)
{
    size_t bs = info->block;
    int ret = HAL_SEC_V2_OK;

    if (ctx->blen) {
        size_t take = bs - ctx->blen;

        if (take > len) {
            take = len;
        }
        memcpy(ctx->buf + ctx->blen, in, take);
        ctx->blen += (uint8_t)take;
        in += take;
        len -= take;
        if (ctx->blen == bs) {
            ret = sha_v2_run(ctx, info, ctx->buf, 1);
            ctx->blen = 0;
        }
    }
    while (ret == HAL_SEC_V2_OK && len >= bs) {
        size_t nblk = len / bs;
        size_t max = sec_v2_direct(in, nblk * bs) ? SEC_V2_DIRECT_MAX :
                                                    SEC_V2_CHUNK;

        if (nblk > max / bs) {
            nblk = max / bs;
        }
        ret = sha_v2_run(ctx, info, in, nblk);
        in += nblk * bs;
        len -= nblk * bs;
    }
    if (ret == HAL_SEC_V2_OK && len) {
        memcpy(ctx->buf, in, len);
        ctx->blen = (uint8_t)len;
    }
    return ret;
}

int hal_sec_hash_v2_update(hal_sec_hash_v2_ctx_t *ctx, const uint8_t *in,
                           size_t len)
{
    const struct sha_v2_info *info = sha_v2_get(ctx);
    int ret;

    if (!info || (!in && len)) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    if (!len) {
        return HAL_SEC_V2_OK;
    }
    if (sec_v2_lock(HAL_SEC_V2_SHA) != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_TIMEOUT;
    }
    ret = sha_v2_update(ctx, info, in, len);
    sec_v2_unlock(HAL_SEC_V2_SHA);
    if (ret == HAL_SEC_V2_OK) {
        ctx->total += len;
    }
    return ret;
}

static int sha_v2_pad(hal_sec_hash_v2_ctx_t *ctx,
                      const struct sha_v2_info *info)
{
    size_t bs = info->block;
    size_t lenf = bs == 128 ? 16 : 8;
    uint64_t bits = ctx->total << 3;
    size_t b = ctx->blen;
    int ret = HAL_SEC_V2_OK;

    ctx->buf[b++] = 0x80;
    if (b > bs - lenf) {
        memset(ctx->buf + b, 0, bs - b);
        ret = sha_v2_run(ctx, info, ctx->buf, 1);
        b = 0;
    }
    memset(ctx->buf + b, 0, bs - b);
    for (size_t i = 0; i < 8; i++) {
        ctx->buf[bs - 1 - i] = (uint8_t)(bits >> (8 * i));
    }
    if (lenf == 16) {
        ctx->buf[bs - 9] = (uint8_t)(ctx->total >> 61);
    }
    if (ret == HAL_SEC_V2_OK) {
        ret = sha_v2_run(ctx, info, ctx->buf, 1);
    }
    return ret;
}

int hal_sec_hash_v2_finish(hal_sec_hash_v2_ctx_t *ctx, uint8_t *out,
                           size_t out_size, size_t *out_len)
{
    const struct sha_v2_info *info = sha_v2_get(ctx);
    int ret;

    if (!info || !out || out_size < info->digest) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    if (sec_v2_lock(HAL_SEC_V2_SHA) != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_TIMEOUT;
    }
    ret = sha_v2_pad(ctx, info);
    sec_v2_unlock(HAL_SEC_V2_SHA);
    if (ret == HAL_SEC_V2_OK) {
        memcpy(out, ctx->state, info->digest);
        if (out_len) {
            *out_len = info->digest;
        }
    }
    hal_sec_v2_zeroize(ctx, sizeof(*ctx));
    return ret;
}

void hal_sec_hash_v2_abort(hal_sec_hash_v2_ctx_t *ctx)
{
    if (ctx) {
        hal_sec_v2_zeroize(ctx, sizeof(*ctx));
    }
}
