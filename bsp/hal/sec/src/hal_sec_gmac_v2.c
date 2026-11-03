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
#include "hal_sec_gmac_v2.h"
#include "hal_sec_v2_priv.h"

#define GMAC_V2_CFG     64u
#define GMAC_V2_EN      (1u << 2)
#define GMAC_V2_BE      0x7000u
#define GMAC_V2_INT_CLR (1u << 9)

/* link cfg {ctrl, src, key[4], result[4]} then data, whole cache lines */
static uint32_t gmac_v2_scratch[(GMAC_V2_CFG + SEC_V2_CHUNK) / 4]
    __attribute__((aligned(32)));

/*
 * n bytes, n % 16 == 0; n <= SEC_V2_DIRECT_MAX when sec_v2_direct(),
 * else n <= SEC_V2_CHUNK
 */
static int gmac_v2_run(hal_sec_ghash_v2_ctx_t *ctx, const uint8_t *in,
                       size_t n)
{
    uint8_t *s = (uint8_t *)gmac_v2_scratch;
    uint32_t *cfg = gmac_v2_scratch;
    size_t used = GMAC_V2_CFG;
    int ret;

    if (sec_v2_direct(in, n)) {
        sec_v2_flush(in, n);
    } else {
        memcpy(s + GMAC_V2_CFG, in, n);
        in = s + GMAC_V2_CFG;
        used += n;
    }
    cfg[0] = GMAC_V2_INT_CLR | ((uint32_t)(n / 16) << 16);
    cfg[1] = sec_v2_dma(in);
    memcpy(&cfg[2], ctx->h, 16);
    memcpy(&cfg[6], ctx->x, 16);
    sec_v2_flush(s, used);
    ret = sec_v2_job(SEC_V2_GMAC_CTRL, SEC_V2_GMAC_LINK,
                     GMAC_V2_EN | GMAC_V2_BE, 0, sec_v2_dma(cfg));
    sec_v2_flush(s, GMAC_V2_CFG);
    if (ret == HAL_SEC_V2_OK) {
        memcpy(ctx->x, &cfg[6], 16);
    }
    hal_sec_v2_zeroize(s, used);
    sec_v2_flush(s, used);
    return ret;
}

int hal_sec_ghash_v2_setup(hal_sec_ghash_v2_ctx_t *ctx, const uint8_t h[16])
{
    if (!ctx || !h) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    memset(ctx, 0, sizeof(*ctx));
    memcpy(ctx->h, h, 16);
    return HAL_SEC_V2_OK;
}

static int gmac_v2_update(hal_sec_ghash_v2_ctx_t *ctx, const uint8_t *in,
                          size_t len)
{
    int ret = HAL_SEC_V2_OK;

    if (ctx->blen) {
        size_t take = 16u - ctx->blen;

        if (take > len) {
            take = len;
        }
        memcpy(ctx->buf + ctx->blen, in, take);
        ctx->blen += (uint8_t)take;
        in += take;
        len -= take;
        if (ctx->blen == 16) {
            ret = gmac_v2_run(ctx, ctx->buf, 16);
            ctx->blen = 0;
        }
    }
    while (ret == HAL_SEC_V2_OK && len >= 16) {
        size_t n = len & ~(size_t)15;
        size_t max = sec_v2_direct(in, n) ? SEC_V2_DIRECT_MAX : SEC_V2_CHUNK;

        if (n > max) {
            n = max;
        }
        ret = gmac_v2_run(ctx, in, n);
        in += n;
        len -= n;
    }
    if (ret == HAL_SEC_V2_OK && len) {
        memcpy(ctx->buf, in, len);
        ctx->blen = (uint8_t)len;
    }
    return ret;
}

int hal_sec_ghash_v2_update(hal_sec_ghash_v2_ctx_t *ctx, const uint8_t *in,
                            size_t len)
{
    int ret;

    if (!ctx || ctx->blen >= 16 || (!in && len)) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    if (!len) {
        return HAL_SEC_V2_OK;
    }
    if (sec_v2_lock(HAL_SEC_V2_GMAC) != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_TIMEOUT;
    }
    ret = gmac_v2_update(ctx, in, len);
    sec_v2_unlock(HAL_SEC_V2_GMAC);
    return ret;
}

int hal_sec_ghash_v2_pad(hal_sec_ghash_v2_ctx_t *ctx)
{
    int ret;

    if (!ctx || ctx->blen >= 16) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    if (!ctx->blen) {
        return HAL_SEC_V2_OK;
    }
    if (sec_v2_lock(HAL_SEC_V2_GMAC) != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_TIMEOUT;
    }
    memset(ctx->buf + ctx->blen, 0, 16u - ctx->blen);
    ret = gmac_v2_run(ctx, ctx->buf, 16);
    ctx->blen = 0;
    sec_v2_unlock(HAL_SEC_V2_GMAC);
    return ret;
}

int hal_sec_ghash_v2_finish(hal_sec_ghash_v2_ctx_t *ctx, uint8_t out[16])
{
    int ret;

    if (!ctx || !out) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    ret = hal_sec_ghash_v2_pad(ctx);
    if (ret == HAL_SEC_V2_OK) {
        memcpy(out, ctx->x, 16);
    }
    hal_sec_v2_zeroize(ctx, sizeof(*ctx));
    return ret;
}

void hal_sec_ghash_v2_abort(hal_sec_ghash_v2_ctx_t *ctx)
{
    if (ctx) {
        hal_sec_v2_zeroize(ctx, sizeof(*ctx));
    }
}
