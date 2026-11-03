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

#define XTS_V2_CFG      96u
#define XTS_V2_EN       (1u << 2)
#define XTS_V2_BE       0x1Fu
#define XTS_V2_INT_CLR  (1u << 9)
#define XTS_V2_MODE     (3u << 12)
#define XTS_V2_RAW_T    (1u << 15)
#define XTS_V2_UNIT_MAX (0xFFFFu * 16u)

/*
 * link cfg {ctrl, src, dst, tweak[4], key1[8], unit, key2[8]} then
 * data, whole cache lines
 */
static uint32_t xts_v2_scratch[(XTS_V2_CFG + SEC_V2_CHUNK) / 4]
    __attribute__((aligned(32)));

int hal_sec_xts_v2_setkey(hal_sec_xts_v2_ctx_t *ctx, unsigned int dir,
                          const uint8_t *key, size_t keylen)
{
    size_t half = keylen / 2;

    if (!ctx || !key || (keylen != 32 && keylen != 64) ||
        dir > HAL_SEC_AES_V2_DECRYPT) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    hal_sec_v2_zeroize(ctx, sizeof(*ctx));
    memcpy(ctx->key, key, keylen);
    ctx->half = (uint8_t)half;
    ctx->dir = (uint8_t)dir;
    return HAL_SEC_V2_OK;
}

int hal_sec_xts_v2_set_unit(hal_sec_xts_v2_ctx_t *ctx,
                            const uint8_t unit[16], size_t unit_len)
{
    if (!ctx || !ctx->half || !unit || !unit_len || unit_len % 16 ||
        unit_len > XTS_V2_UNIT_MAX) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    memcpy(ctx->unit, unit, 16);
    ctx->unit_len = (uint32_t)unit_len;
    ctx->blk = 0;
    ctx->tweak_ok = 0;
    return HAL_SEC_V2_OK;
}

/* t * x in GF(2^128), P1619 byte order (byte 0 least significant) */
static void xts_v2_mul_x(uint8_t t[16])
{
    uint8_t carry = 0;

    for (unsigned int i = 0; i < 16; i++) {
        uint8_t next = t[i] >> 7;

        t[i] = (uint8_t)((t[i] << 1) | carry);
        carry = next;
    }
    if (carry) {
        t[0] ^= 0x87;
    }
}

static void xts_v2_unit_add(uint8_t unit[16], size_t n)
{
    uint64_t carry = n;

    for (unsigned int i = 0; i < 16 && carry; i++) {
        carry += unit[i];
        unit[i] = (uint8_t)carry;
        carry >>= 8;
    }
}

/* ctx->tweak = AES-ECB(key2, unit) */
static int xts_v2_tweak(hal_sec_xts_v2_ctx_t *ctx)
{
    uint8_t *s = (uint8_t *)xts_v2_scratch;
    uint32_t *cfg = xts_v2_scratch;
    uint8_t *data = s + XTS_V2_CFG;
    size_t used = XTS_V2_CFG + 16;
    int ret;

    memset(cfg, 0, XTS_V2_CFG);
    cfg[0] = ((ctx->half == 32 ? 1u : 0u) << 3) | XTS_V2_INT_CLR |
             (1u << 16);
    cfg[1] = sec_v2_dma(data);
    cfg[2] = sec_v2_dma(data);
    memcpy(&cfg[7], ctx->key + ctx->half, ctx->half);
    memcpy(data, ctx->unit, 16);
    sec_v2_wr(SEC_V2_AES_ENDIAN, XTS_V2_BE);
    sec_v2_flush(s, used);
    ret = sec_v2_job(SEC_V2_AES_CTRL, SEC_V2_AES_LINK,
                     XTS_V2_EN | SEC_V2_LINK_MODE, 0, sec_v2_dma(cfg));
    sec_v2_flush(s, used);
    if (ret == HAL_SEC_V2_OK) {
        memcpy(ctx->tweak, data, 16);
        ctx->tweak_ok = 1;
    }
    hal_sec_v2_zeroize(s, used);
    sec_v2_flush(s, used);
    return ret;
}

/*
 * n bytes. raw: tweak words = ctx->tweak and no unit boundary in the
 * job; else they hold the unit number and the engine encrypts it and
 * steps it at each unit_len. direct: in place, else via the scratch.
 */
static int xts_v2_run(hal_sec_xts_v2_ctx_t *ctx, const uint8_t *in,
                      uint8_t *out, size_t n, int direct, int raw)
{
    uint8_t *s = (uint8_t *)xts_v2_scratch;
    uint32_t *cfg = xts_v2_scratch;
    uint8_t *data = direct ? out : s + XTS_V2_CFG;
    size_t used = direct ? XTS_V2_CFG : XTS_V2_CFG + n;
    uint32_t unit = raw ? (uint32_t)(n / 16) : ctx->unit_len / 16;
    uint8_t *tw = (uint8_t *)&cfg[3];
    int ret;

    memset(cfg, 0, XTS_V2_CFG);
    if (direct) {
        sec_v2_flush(in, n);
        cfg[1] = sec_v2_dma(in);
    } else {
        memcpy(data, in, n);
        cfg[1] = sec_v2_dma(data);
    }
    cfg[0] = ((ctx->half == 32 ? 1u : 0u) << 3) | ((uint32_t)ctx->dir << 5) |
             XTS_V2_INT_CLR | XTS_V2_MODE | (raw ? XTS_V2_RAW_T : 0) |
             ((uint32_t)(n / 16) << 16);
    cfg[2] = sec_v2_dma(data);
    for (unsigned int i = 0; i < 16; i++) {
        tw[i] = raw ? ctx->tweak[i] : ctx->unit[15 - i];
    }
    memcpy(&cfg[7], ctx->key, ctx->half);
    cfg[15] = unit << 16;
    memcpy(&cfg[16], ctx->key + ctx->half, ctx->half);
    sec_v2_wr(SEC_V2_AES_ENDIAN, XTS_V2_BE);
    sec_v2_flush(s, used);
    if (direct) {
        sec_v2_flush(out, n);
    }
    ret = sec_v2_job(SEC_V2_AES_CTRL, SEC_V2_AES_LINK,
                     XTS_V2_EN | SEC_V2_LINK_MODE, 0, sec_v2_dma(cfg));
    sec_v2_flush(s, used);
    if (direct) {
        sec_v2_flush(out, n);
    }
    if (ret == HAL_SEC_V2_OK && !direct) {
        memcpy(out, data, n);
    }
    hal_sec_v2_zeroize(s, used);
    sec_v2_flush(s, used);
    return ret;
}

/* job size from max bytes; *raw set when it cannot start a unit */
static size_t xts_v2_size(const hal_sec_xts_v2_ctx_t *ctx, size_t max,
                          int *raw)
{
    size_t left = ctx->unit_len - (size_t)ctx->blk * 16;

    *raw = ctx->blk != 0 || max < ctx->unit_len;
    return *raw ? (max < left ? max : left) : max - max % ctx->unit_len;
}

static void xts_v2_advance(hal_sec_xts_v2_ctx_t *ctx, size_t n, int raw)
{
    if (!raw) {
        xts_v2_unit_add(ctx->unit, n / ctx->unit_len);
        ctx->tweak_ok = 0;
        return;
    }
    for (size_t i = 0; i < n / 16; i++) {
        xts_v2_mul_x(ctx->tweak);
    }
    ctx->blk += (uint32_t)(n / 16);
    if ((size_t)ctx->blk * 16 == ctx->unit_len) {
        xts_v2_unit_add(ctx->unit, 1);
        ctx->blk = 0;
        ctx->tweak_ok = 0;
    }
}

int hal_sec_xts_v2_crypt(hal_sec_xts_v2_ctx_t *ctx, const uint8_t *in,
                         uint8_t *out, size_t len)
{
    int ret = HAL_SEC_V2_OK;

    if (!ctx || !ctx->half || !ctx->unit_len || len % 16 ||
        ((!in || !out) && len)) {
        return HAL_SEC_V2_ERR_PARAM;
    }
    if (sec_v2_lock(HAL_SEC_V2_AES) != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_TIMEOUT;
    }
    while (len && ret == HAL_SEC_V2_OK) {
        int raw;
        size_t n = xts_v2_size(ctx, len < SEC_V2_DIRECT_MAX ?
                                    len : SEC_V2_DIRECT_MAX, &raw);
        int direct = sec_v2_direct(in, n) && sec_v2_direct_out(out, n) == n;

        if (!direct && n > SEC_V2_CHUNK) {
            n = xts_v2_size(ctx, SEC_V2_CHUNK, &raw);
        }
        if (raw && !ctx->tweak_ok) {
            ret = xts_v2_tweak(ctx);
        }
        if (ret == HAL_SEC_V2_OK) {
            ret = xts_v2_run(ctx, in, out, n, direct, raw);
        }
        if (ret == HAL_SEC_V2_OK) {
            xts_v2_advance(ctx, n, raw);
        }
        in += n;
        out += n;
        len -= n;
    }
    sec_v2_unlock(HAL_SEC_V2_AES);
    return ret;
}

void hal_sec_xts_v2_abort(hal_sec_xts_v2_ctx_t *ctx)
{
    if (ctx) {
        hal_sec_v2_zeroize(ctx, sizeof(*ctx));
    }
}
