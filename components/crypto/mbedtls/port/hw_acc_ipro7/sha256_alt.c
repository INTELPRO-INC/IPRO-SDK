/*
 *  SHA-256/224 hardware acceleration for IPRO7 — per-context implementation
 *
 *  Each mbedtls_sha256_context owns its own sec_hash_handle_t, allowing
 *  multiple concurrent SHA operations (required by TLS handshake).
 *
 *  Copyright (C) 2025, INTELPRO Inc.
 *  SPDX-License-Identifier: Apache-2.0
 */

#include "common.h"

#if defined(MBEDTLS_SHA256_C)

#include "mbedtls/sha256.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include <string.h>

#if defined(MBEDTLS_SHA256_ALT)

#include "hal_sec_hash.h"
#include "hal_common.h"
#include "system_soc.h"

#define SHA256_VALIDATE_RET(cond) \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_SHA256_BAD_INPUT_DATA)
#define SHA256_VALIDATE(cond) \
    MBEDTLS_INTERNAL_VALIDATE(cond)

/*
 * Lightweight SHA256 operation trace — records the exact sequence of
 * starts/updates/finishes across all contexts.  Used to capture the
 * real TLS handshake interleaving pattern for standalone replay.
 *
 * Overhead: ~12 bytes written per operation, no printf, no branching
 * beyond the bounds check.  Minimal timing impact.
 */
#define SHA_TRACE_MAX 2048

struct sha_trace_entry {
    uint8_t  op;       /* 0=start, 1=update, 2=finish */
    uint8_t  ctx_idx;  /* context index (0-based, assigned on first start) */
    uint16_t size;     /* update size (truncated to 16 bits) */
};

static struct sha_trace_entry sha_trace_buf[SHA_TRACE_MAX];
static int sha_trace_idx;
static int sha_trace_on;

/* Map context pointers to small indices */
#define SHA_CTX_MAP_MAX 16
static mbedtls_sha256_context *sha_ctx_map[SHA_CTX_MAP_MAX];
static int sha_ctx_map_count;

static int sha_trace_ctx_id(mbedtls_sha256_context *ctx)
{
    for (int i = 0; i < sha_ctx_map_count; i++)
        if (sha_ctx_map[i] == ctx) return i;
    if (sha_ctx_map_count < SHA_CTX_MAP_MAX) {
        sha_ctx_map[sha_ctx_map_count] = ctx;
        return sha_ctx_map_count++;
    }
    return 0xFF;
}

static void sha_trace_record(uint8_t op, mbedtls_sha256_context *ctx,
                              uint16_t size)
{
    if (!sha_trace_on || sha_trace_idx >= SHA_TRACE_MAX) return;
    sha_trace_buf[sha_trace_idx].op = op;
    sha_trace_buf[sha_trace_idx].ctx_idx = sha_trace_ctx_id(ctx);
    sha_trace_buf[sha_trace_idx].size = size;
    sha_trace_idx++;
}

/* Called from shell to enable/disable tracing and dump results */
void sha256_trace_start(void)
{
    sha_trace_idx = 0;
    sha_ctx_map_count = 0;
    memset(sha_ctx_map, 0, sizeof(sha_ctx_map));
    sha_trace_on = 1;
}

void sha256_trace_stop(void)
{
    sha_trace_on = 0;
}

void sha256_trace_dump(void)
{
    printf("[SHA_TRACE] %d entries, %d contexts\r\n",
           sha_trace_idx, sha_ctx_map_count);
    for (int i = 0; i < sha_trace_idx; i++) {
        struct sha_trace_entry *e = &sha_trace_buf[i];
        const char *ops[] = {"S", "U", "F"};
        printf("  %s c%d %u\r\n",
               e->op <= 2 ? ops[e->op] : "?",
               e->ctx_idx, e->size);
    }
}

/*
 * After memcpy of sec_hash_handle_t, the Link_Ctx internal pointers
 * (shaBuf, shaPadding, linkAddr) still reference the SOURCE handle's
 * buffers. Repoint them to the destination's own embedded buffers.
 */
static void handle_fixup_sha256(sec_hash_handle_t *h)
{
    h->link.sha256_link_ctx.shaBuf =
        (uint32_t *)hal_get_noncacheable_addr(h->shaBuf);
    h->link.sha256_link_ctx.shaPadding =
        (uint32_t *)hal_get_noncacheable_addr(h->shaPadding);
    h->link.sha256_link_ctx.linkAddr =
        (uint32_t)hal_get_noncacheable_addr(&h->link.link_cfg);
}

void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
    SHA256_VALIDATE(ctx != NULL);
    memset(ctx, 0, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
    if (ctx == NULL)
        return;
    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src)
{
    SHA256_VALIDATE(dst != NULL);
    SHA256_VALIDATE(src != NULL);

    *dst = *src;

    /*
     * Flush and discard dirty cache lines for dst's HW-visible buffers.
     * *dst = *src created dirty cache lines with src's cached (possibly
     * stale) data. Must flush+invalidate BEFORE NC memcpy so that later
     * cache eviction cannot overwrite the NC-written correct values.
     */
    L1C_DCACHE_CLEAN_INVALID_RANGE(dst->handle.shaBuf,
                                   sizeof(dst->handle.shaBuf));
    L1C_DCACHE_CLEAN_INVALID_RANGE(dst->handle.shaPadding,
                                   sizeof(dst->handle.shaPadding));
    L1C_DCACHE_CLEAN_INVALID_RANGE(&dst->handle.link.link_cfg,
                                   sizeof(SEC_Eng_SHA_Link_Config_Type));

    /* Re-copy HW-accessed data through noncacheable aliases */
    memcpy(hal_get_noncacheable_addr(dst->handle.shaBuf),
           hal_get_noncacheable_addr((void *)src->handle.shaBuf),
           sizeof(dst->handle.shaBuf));
    memcpy(hal_get_noncacheable_addr(dst->handle.shaPadding),
           hal_get_noncacheable_addr((void *)src->handle.shaPadding),
           sizeof(dst->handle.shaPadding));
    memcpy(hal_get_noncacheable_addr(&dst->handle.link.link_cfg),
           hal_get_noncacheable_addr((void *)&src->handle.link.link_cfg),
           sizeof(SEC_Eng_SHA_Link_Config_Type));

    handle_fixup_sha256(&dst->handle);
}

int mbedtls_sha256_starts_ret(mbedtls_sha256_context *ctx, int is224)
{
    SHA256_VALIDATE_RET(ctx != NULL);
    SHA256_VALIDATE_RET(is224 == 0 || is224 == 1);

    ctx->is224 = is224;
    uint8_t type = is224 ? SEC_HASH_SHA224 : SEC_HASH_SHA256;
    int ret = sec_hash_init_ex(&ctx->handle, SEC_HASH_MODE_LINK, type);
    if (sha_trace_on) sha_trace_record(0, ctx, is224);
    return (ret == 0) ? 0 : MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224)
{
    mbedtls_sha256_starts_ret(ctx, is224);
}
#endif

int mbedtls_sha256_update_ret(mbedtls_sha256_context *ctx,
                                const unsigned char *input,
                                size_t ilen)
{
    SHA256_VALIDATE_RET(ctx != NULL);
    SHA256_VALIDATE_RET(ilen == 0 || input != NULL);

    if (ilen == 0)
        return 0;

    if (sha_trace_on) sha_trace_record(1, ctx, (uint16_t)ilen);
    int ret = sec_hash_update(&ctx->handle, input, ilen);
    return (ret == 0) ? 0 : MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha256_update(mbedtls_sha256_context *ctx,
                            const unsigned char *input,
                            size_t ilen)
{
    mbedtls_sha256_update_ret(ctx, input, ilen);
}
#endif

int mbedtls_sha256_finish_ret(mbedtls_sha256_context *ctx,
                                unsigned char output[32])
{
    SHA256_VALIDATE_RET(ctx != NULL);
    SHA256_VALIDATE_RET((unsigned char *)output != NULL);

    if (sha_trace_on) sha_trace_record(2, ctx, 0);
    int ret = sec_hash_finish(&ctx->handle, output);
    return (ret > 0) ? 0 : MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha256_finish(mbedtls_sha256_context *ctx,
                            unsigned char output[32])
{
    mbedtls_sha256_finish_ret(ctx, output);
}
#endif

int mbedtls_sha256_ret(const unsigned char *input,
                        size_t ilen,
                        unsigned char output[32],
                        int is224)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_sha256_context ctx;

    SHA256_VALIDATE_RET(is224 == 0 || is224 == 1);
    SHA256_VALIDATE_RET(ilen == 0 || input != NULL);
    SHA256_VALIDATE_RET((unsigned char *)output != NULL);

    mbedtls_sha256_init(&ctx);

    if ((ret = mbedtls_sha256_starts_ret(&ctx, is224)) != 0)
        goto exit;
    if ((ret = mbedtls_sha256_update_ret(&ctx, input, ilen)) != 0)
        goto exit;
    if ((ret = mbedtls_sha256_finish_ret(&ctx, output)) != 0)
        goto exit;

exit:
    mbedtls_sha256_free(&ctx);
    return ret;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha256(const unsigned char *input,
                     size_t ilen,
                     unsigned char output[32],
                     int is224)
{
    mbedtls_sha256_ret(input, ilen, output, is224);
}
#endif

/*
 * SHA-256/224 Hardware Self-Test (NIST vectors, matches mbedtls_sha256_self_test)
 *
 * Test #1: "abc" (3 bytes)
 * Test #2: "abcdbcde...nopq" (56 bytes, near block boundary)
 * Test #3: "a" x 1,000,000 (multi-block, 1000 x 1000-byte updates)
 *
 * Runs all 3 tests for both SHA-224 (i=0..2) and SHA-256 (i=3..5).
 */
static const unsigned char sha256_test_buf[3][57] = {
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};
static const size_t sha256_test_buflen[3] = { 3, 56, 1000 };
static const unsigned char sha256_test_sum[6][32] = {
    /* SHA-224 */
    { 0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8, 0x22,
      0x86, 0x42, 0xA4, 0x77, 0xBD, 0xA2, 0x55, 0xB3,
      0x2A, 0xAD, 0xBC, 0xE4, 0xBD, 0xA0, 0xB3, 0xF7,
      0xE3, 0x6C, 0x9D, 0xA7 },
    { 0x75, 0x38, 0x8B, 0x16, 0x51, 0x27, 0x76, 0xCC,
      0x5D, 0xBA, 0x5D, 0xA1, 0xFD, 0x89, 0x01, 0x50,
      0xB0, 0xC6, 0x45, 0x5C, 0xB4, 0xF5, 0x8B, 0x19,
      0x52, 0x52, 0x25, 0x25 },
    { 0x20, 0x79, 0x46, 0x55, 0x98, 0x0C, 0x91, 0xD8,
      0xBB, 0xB4, 0xC1, 0xEA, 0x97, 0x61, 0x8A, 0x4B,
      0xF0, 0x3F, 0x42, 0x58, 0x19, 0x48, 0xB2, 0xEE,
      0x4E, 0xE7, 0xAD, 0x67 },
    /* SHA-256 */
    { 0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
      0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
      0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
      0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD },
    { 0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8,
      0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
      0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67,
      0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1 },
    { 0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92,
      0x81, 0xA1, 0xC7, 0xE2, 0x84, 0xD7, 0x3E, 0x67,
      0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97, 0x20, 0x0E,
      0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0 }
};

int mbedtls_sha256_self_test_hw(int verbose)
{
    int i, j, k, ret = 0;
    unsigned char sha256sum[32];
    unsigned char buf[1024];
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init(&ctx);

    for (i = 0; i < 6; i++) {
        j = i % 3;
        k = i < 3;

        if (verbose)
            printf("  SHA-%d test #%d: ", 256 - k * 32, j + 1);

        if ((ret = mbedtls_sha256_starts_ret(&ctx, k)) != 0)
            goto fail;

        if (j == 2) {
            memset(buf, 'a', 1000);
            for (j = 0; j < 1000; j++) {
                ret = mbedtls_sha256_update_ret(&ctx, buf, 1000);
                if (ret != 0)
                    goto fail;
            }
        } else {
            ret = mbedtls_sha256_update_ret(&ctx, sha256_test_buf[j],
                                            sha256_test_buflen[j]);
            if (ret != 0)
                goto fail;
        }

        if ((ret = mbedtls_sha256_finish_ret(&ctx, sha256sum)) != 0)
            goto fail;

        if (memcmp(sha256sum, sha256_test_sum[i], 32 - k * 4) != 0) {
            ret = 1;
            goto fail;
        }

        if (verbose)
            printf("PASSED\n");
    }

    if (verbose)
        printf("\n");

    goto exit;

fail:
    if (verbose)
        printf("FAILED\n");

exit:
    mbedtls_sha256_free(&ctx);
    return ret;
}

#endif /* MBEDTLS_SHA256_ALT */
#endif /* MBEDTLS_SHA256_C */
