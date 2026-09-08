/*
 *  SHA-512/384 hardware acceleration for IPRO7 — per-context implementation
 *
 *  Each context owns its own sec_hash_handle_t for concurrent safety.
 */

#include "common.h"

#if defined(MBEDTLS_SHA512_C)

#include "mbedtls/sha512.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include <string.h>

#if defined(MBEDTLS_SHA512_ALT)

#include "hal_sec_hash.h"
#include "hal_common.h"
#include "system_soc.h"

#define SHA512_VALIDATE_RET(cond) \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_SHA512_BAD_INPUT_DATA)
#define SHA512_VALIDATE(cond) \
    MBEDTLS_INTERNAL_VALIDATE(cond)

/* SHA-512 uses a different Link_Ctx with uint64_t* pointers */
static void handle_fixup_sha512(sec_hash_handle_t *h)
{
    h->link.sha512_link_ctx.shaBuf =
        (uint64_t *)hal_get_noncacheable_addr(h->shaBuf);
    h->link.sha512_link_ctx.shaPadding =
        (uint64_t *)hal_get_noncacheable_addr(h->shaPadding);
    h->link.sha512_link_ctx.linkAddr =
        (uint32_t)hal_get_noncacheable_addr(&h->link.link_cfg);
}

void mbedtls_sha512_init(mbedtls_sha512_context *ctx)
{
    SHA512_VALIDATE(ctx != NULL);
    memset(ctx, 0, sizeof(mbedtls_sha512_context));
}

void mbedtls_sha512_free(mbedtls_sha512_context *ctx)
{
    if (ctx == NULL)
        return;
    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_sha512_context));
}

void mbedtls_sha512_clone(mbedtls_sha512_context *dst,
                           const mbedtls_sha512_context *src)
{
    SHA512_VALIDATE(dst != NULL);
    SHA512_VALIDATE(src != NULL);

    *dst = *src;

    /* Flush and discard dirty cache lines for dst's HW-visible buffers
     * before NC memcpy (see sha256_alt.c for detailed explanation) */
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

    handle_fixup_sha512(&dst->handle);
}

int mbedtls_sha512_starts_ret(mbedtls_sha512_context *ctx, int is384)
{
    SHA512_VALIDATE_RET(ctx != NULL);
    SHA512_VALIDATE_RET(is384 == 0 || is384 == 1);

    ctx->is384 = is384;
    uint8_t type = is384 ? SEC_HASH_SHA384 : SEC_HASH_SHA512;
    int ret = sec_hash_init_ex(&ctx->handle, SEC_HASH_MODE_LINK, type);
    return (ret == 0) ? 0 : MBEDTLS_ERR_SHA512_HW_ACCEL_FAILED;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512_starts(mbedtls_sha512_context *ctx, int is384)
{
    mbedtls_sha512_starts_ret(ctx, is384);
}
#endif

int mbedtls_sha512_update_ret(mbedtls_sha512_context *ctx,
                               const unsigned char *input,
                               size_t ilen)
{
    SHA512_VALIDATE_RET(ctx != NULL);
    SHA512_VALIDATE_RET(ilen == 0 || input != NULL);

    if (ilen == 0)
        return 0;

    int ret = sec_hash_update(&ctx->handle, input, ilen);
    return (ret == 0) ? 0 : MBEDTLS_ERR_SHA512_HW_ACCEL_FAILED;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512_update(mbedtls_sha512_context *ctx,
                            const unsigned char *input,
                            size_t ilen)
{
    mbedtls_sha512_update_ret(ctx, input, ilen);
}
#endif

int mbedtls_sha512_finish_ret(mbedtls_sha512_context *ctx,
                                unsigned char output[64])
{
    SHA512_VALIDATE_RET(ctx != NULL);
    SHA512_VALIDATE_RET((unsigned char *)output != NULL);

    int ret = sec_hash_finish(&ctx->handle, output);
    return (ret > 0) ? 0 : MBEDTLS_ERR_SHA512_HW_ACCEL_FAILED;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512_finish(mbedtls_sha512_context *ctx,
                            unsigned char output[64])
{
    mbedtls_sha512_finish_ret(ctx, output);
}
#endif

int mbedtls_sha512_ret(const unsigned char *input,
                        size_t ilen,
                        unsigned char output[64],
                        int is384)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_sha512_context ctx;

    SHA512_VALIDATE_RET(ilen == 0 || input != NULL);
    SHA512_VALIDATE_RET((unsigned char *)output != NULL);

    mbedtls_sha512_init(&ctx);

    if ((ret = mbedtls_sha512_starts_ret(&ctx, is384)) != 0)
        goto exit;
    if ((ret = mbedtls_sha512_update_ret(&ctx, input, ilen)) != 0)
        goto exit;
    if ((ret = mbedtls_sha512_finish_ret(&ctx, output)) != 0)
        goto exit;

exit:
    mbedtls_sha512_free(&ctx);
    return ret;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512(const unsigned char *input,
                     size_t ilen,
                     unsigned char output[64],
                     int is384)
{
    mbedtls_sha512_ret(input, ilen, output, is384);
}
#endif

/*
 * SHA-512/384 Hardware Self-Test (NIST vectors, matches mbedtls_sha512_self_test)
 *
 * Test #1: "abc" (3 bytes)
 * Test #2: "abcdefgh...nopqrstu" (112 bytes)
 * Test #3: "a" x 1,000,000 (multi-block, 1000 x 1000-byte updates)
 *
 * Runs all 3 tests for both SHA-384 (i=0..2) and SHA-512 (i=3..5).
 */
static const unsigned char sha512_test_buf[3][113] = {
    { "abc" },
    { "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
      "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu" },
    { "" }
};
static const size_t sha512_test_buflen[3] = { 3, 112, 1000 };
static const unsigned char sha512_test_sum[6][64] = {
    /* SHA-384 */
    { 0xCB, 0x00, 0x75, 0x3F, 0x45, 0xA3, 0x5E, 0x8B,
      0xB5, 0xA0, 0x3D, 0x69, 0x9A, 0xC6, 0x50, 0x07,
      0x27, 0x2C, 0x32, 0xAB, 0x0E, 0xDE, 0xD1, 0x63,
      0x1A, 0x8B, 0x60, 0x5A, 0x43, 0xFF, 0x5B, 0xED,
      0x80, 0x86, 0x07, 0x2B, 0xA1, 0xE7, 0xCC, 0x23,
      0x58, 0xBA, 0xEC, 0xA1, 0x34, 0xC8, 0x25, 0xA7 },
    { 0x09, 0x33, 0x0C, 0x33, 0xF7, 0x11, 0x47, 0xE8,
      0x3D, 0x19, 0x2F, 0xC7, 0x82, 0xCD, 0x1B, 0x47,
      0x53, 0x11, 0x1B, 0x17, 0x3B, 0x3B, 0x05, 0xD2,
      0x2F, 0xA0, 0x80, 0x86, 0xE3, 0xB0, 0xF7, 0x12,
      0xFC, 0xC7, 0xC7, 0x1A, 0x55, 0x7E, 0x2D, 0xB9,
      0x66, 0xC3, 0xE9, 0xFA, 0x91, 0x74, 0x60, 0x39 },
    { 0x9D, 0x0E, 0x18, 0x09, 0x71, 0x64, 0x74, 0xCB,
      0x08, 0x6E, 0x83, 0x4E, 0x31, 0x0A, 0x4A, 0x1C,
      0xED, 0x14, 0x9E, 0x9C, 0x00, 0xF2, 0x48, 0x52,
      0x79, 0x72, 0xCE, 0xC5, 0x70, 0x4C, 0x2A, 0x5B,
      0x07, 0xB8, 0xB3, 0xDC, 0x38, 0xEC, 0xC4, 0xEB,
      0xAE, 0x97, 0xDD, 0xD8, 0x7F, 0x3D, 0x89, 0x85 },
    /* SHA-512 */
    { 0xDD, 0xAF, 0x35, 0xA1, 0x93, 0x61, 0x7A, 0xBA,
      0xCC, 0x41, 0x73, 0x49, 0xAE, 0x20, 0x41, 0x31,
      0x12, 0xE6, 0xFA, 0x4E, 0x89, 0xA9, 0x7E, 0xA2,
      0x0A, 0x9E, 0xEE, 0xE6, 0x4B, 0x55, 0xD3, 0x9A,
      0x21, 0x92, 0x99, 0x2A, 0x27, 0x4F, 0xC1, 0xA8,
      0x36, 0xBA, 0x3C, 0x23, 0xA3, 0xFE, 0xEB, 0xBD,
      0x45, 0x4D, 0x44, 0x23, 0x64, 0x3C, 0xE8, 0x0E,
      0x2A, 0x9A, 0xC9, 0x4F, 0xA5, 0x4C, 0xA4, 0x9F },
    { 0x8E, 0x95, 0x9B, 0x75, 0xDA, 0xE3, 0x13, 0xDA,
      0x8C, 0xF4, 0xF7, 0x28, 0x14, 0xFC, 0x14, 0x3F,
      0x8F, 0x77, 0x79, 0xC6, 0xEB, 0x9F, 0x7F, 0xA1,
      0x72, 0x99, 0xAE, 0xAD, 0xB6, 0x88, 0x90, 0x18,
      0x50, 0x1D, 0x28, 0x9E, 0x49, 0x00, 0xF7, 0xE4,
      0x33, 0x1B, 0x99, 0xDE, 0xC4, 0xB5, 0x43, 0x3A,
      0xC7, 0xD3, 0x29, 0xEE, 0xB6, 0xDD, 0x26, 0x54,
      0x5E, 0x96, 0xE5, 0x5B, 0x87, 0x4B, 0xE9, 0x09 },
    { 0xE7, 0x18, 0x48, 0x3D, 0x0C, 0xE7, 0x69, 0x64,
      0x4E, 0x2E, 0x42, 0xC7, 0xBC, 0x15, 0xB4, 0x63,
      0x8E, 0x1F, 0x98, 0xB1, 0x3B, 0x20, 0x44, 0x28,
      0x56, 0x32, 0xA8, 0x03, 0xAF, 0xA9, 0x73, 0xEB,
      0xDE, 0x0F, 0xF2, 0x44, 0x87, 0x7E, 0xA6, 0x0A,
      0x4C, 0xB0, 0x43, 0x2C, 0xE5, 0x77, 0xC3, 0x1B,
      0xEB, 0x00, 0x9C, 0x5C, 0x2C, 0x49, 0xAA, 0x2E,
      0x4E, 0xAD, 0xB2, 0x17, 0xAD, 0x8C, 0xC0, 0x9B }
};

int mbedtls_sha512_self_test_hw(int verbose)
{
    int i, j, k, ret = 0;
    unsigned char sha512sum[64];
    unsigned char buf[1024];
    mbedtls_sha512_context ctx;

    mbedtls_sha512_init(&ctx);

    for (i = 0; i < 6; i++) {
        j = i % 3;
        k = i < 3;

        if (verbose)
            printf("  SHA-%d test #%d: ", 512 - k * 128, j + 1);

        if ((ret = mbedtls_sha512_starts_ret(&ctx, k)) != 0)
            goto fail;

        if (j == 2) {
            memset(buf, 'a', 1000);
            for (j = 0; j < 1000; j++) {
                ret = mbedtls_sha512_update_ret(&ctx, buf, 1000);
                if (ret != 0)
                    goto fail;
            }
        } else {
            ret = mbedtls_sha512_update_ret(&ctx, sha512_test_buf[j],
                                            sha512_test_buflen[j]);
            if (ret != 0)
                goto fail;
        }

        if ((ret = mbedtls_sha512_finish_ret(&ctx, sha512sum)) != 0)
            goto fail;

        if (memcmp(sha512sum, sha512_test_sum[i], 64 - k * 16) != 0) {
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
    mbedtls_sha512_free(&ctx);
    return ret;
}

#endif /* MBEDTLS_SHA512_ALT */
#endif /* MBEDTLS_SHA512_C */
