/*
 *  SHA-1 hardware acceleration for IPRO7 — per-context implementation
 *
 *  IPRO7 reuses the SHA-256 engine for SHA-1 operations.
 *  Each context owns its own sec_hash_handle_t for concurrent safety.
 *
 *  @warning SHA-1 is weak — use only for legacy compatibility.
 */

#include "common.h"

#if defined(MBEDTLS_SHA1_C)

#include "mbedtls/sha1.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include <string.h>

#if defined(MBEDTLS_SHA1_ALT)

#include "hal_sec_hash.h"
#include "hal_common.h"
#include "system_soc.h"

#define SHA1_VALIDATE_RET(cond) \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_SHA1_BAD_INPUT_DATA)
#define SHA1_VALIDATE(cond) \
    MBEDTLS_INTERNAL_VALIDATE(cond)

/* SHA-1 uses the SHA-256 engine — same pointer layout as SHA-256 */
static void handle_fixup_sha1(sec_hash_handle_t *h)
{
    h->link.sha256_link_ctx.shaBuf =
        (uint32_t *)hal_get_noncacheable_addr(h->shaBuf);
    h->link.sha256_link_ctx.shaPadding =
        (uint32_t *)hal_get_noncacheable_addr(h->shaPadding);
    h->link.sha256_link_ctx.linkAddr =
        (uint32_t)hal_get_noncacheable_addr(&h->link.link_cfg);
}

void mbedtls_sha1_init(mbedtls_sha1_context *ctx)
{
    SHA1_VALIDATE(ctx != NULL);
    memset(ctx, 0, sizeof(mbedtls_sha1_context));
}

void mbedtls_sha1_free(mbedtls_sha1_context *ctx)
{
    if (ctx == NULL)
        return;
    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_sha1_context));
}

void mbedtls_sha1_clone(mbedtls_sha1_context *dst,
                         const mbedtls_sha1_context *src)
{
    SHA1_VALIDATE(dst != NULL);
    SHA1_VALIDATE(src != NULL);

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

    handle_fixup_sha1(&dst->handle);
}

int mbedtls_sha1_starts_ret(mbedtls_sha1_context *ctx)
{
    SHA1_VALIDATE_RET(ctx != NULL);

    int ret = sec_hash_init_ex(&ctx->handle, SEC_HASH_MODE_LINK, SEC_HASH_SHA1);
    return (ret == 0) ? 0 : MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha1_starts(mbedtls_sha1_context *ctx)
{
    mbedtls_sha1_starts_ret(ctx);
}
#endif

int mbedtls_sha1_update_ret(mbedtls_sha1_context *ctx,
                              const unsigned char *input,
                              size_t ilen)
{
    SHA1_VALIDATE_RET(ctx != NULL);
    SHA1_VALIDATE_RET(ilen == 0 || input != NULL);

    if (ilen == 0)
        return 0;

    int ret = sec_hash_update(&ctx->handle, input, ilen);
    return (ret == 0) ? 0 : MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha1_update(mbedtls_sha1_context *ctx,
                          const unsigned char *input,
                          size_t ilen)
{
    mbedtls_sha1_update_ret(ctx, input, ilen);
}
#endif

int mbedtls_sha1_finish_ret(mbedtls_sha1_context *ctx,
                              unsigned char output[20])
{
    SHA1_VALIDATE_RET(ctx != NULL);
    SHA1_VALIDATE_RET(output != NULL);

    int ret = sec_hash_finish(&ctx->handle, output);
    return (ret > 0) ? 0 : MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha1_finish(mbedtls_sha1_context *ctx,
                          unsigned char output[20])
{
    mbedtls_sha1_finish_ret(ctx, output);
}
#endif

int mbedtls_sha1_ret(const unsigned char *input,
                      size_t ilen,
                      unsigned char output[20])
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_sha1_context ctx;

    SHA1_VALIDATE_RET(ilen == 0 || input != NULL);
    SHA1_VALIDATE_RET((unsigned char *)output != NULL);

    mbedtls_sha1_init(&ctx);

    if ((ret = mbedtls_sha1_starts_ret(&ctx)) != 0)
        goto exit;
    if ((ret = mbedtls_sha1_update_ret(&ctx, input, ilen)) != 0)
        goto exit;
    if ((ret = mbedtls_sha1_finish_ret(&ctx, output)) != 0)
        goto exit;

exit:
    mbedtls_sha1_free(&ctx);
    return ret;
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha1(const unsigned char *input,
                   size_t ilen,
                   unsigned char output[20])
{
    mbedtls_sha1_ret(input, ilen, output);
}
#endif

/*
 * SHA-1 Hardware Self-Test (NIST vectors, matches mbedtls_sha1_self_test)
 *
 * Test #1: "abc" (3 bytes)
 * Test #2: "abcdbcde...nopq" (56 bytes, near block boundary)
 * Test #3: "a" x 1,000,000 (multi-block, 1000 x 1000-byte updates)
 */
static const unsigned char sha1_test_buf[3][57] = {
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};
static const size_t sha1_test_buflen[3] = { 3, 56, 1000 };
static const unsigned char sha1_test_sum[3][20] = {
    { 0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
      0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D },
    { 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
      0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1 },
    { 0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
      0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F }
};

int mbedtls_sha1_self_test_hw(int verbose)
{
    int i, j, ret = 0;
    unsigned char sha1sum[20];
    unsigned char buf[1024];
    mbedtls_sha1_context ctx;

    mbedtls_sha1_init(&ctx);

    for (i = 0; i < 3; i++) {
        if (verbose)
            printf("  SHA-1 test #%d: ", i + 1);

        if ((ret = mbedtls_sha1_starts_ret(&ctx)) != 0)
            goto fail;

        if (i == 2) {
            memset(buf, 'a', 1000);
            for (j = 0; j < 1000; j++) {
                ret = mbedtls_sha1_update_ret(&ctx, buf, 1000);
                if (ret != 0)
                    goto fail;
            }
        } else {
            ret = mbedtls_sha1_update_ret(&ctx, sha1_test_buf[i],
                                          sha1_test_buflen[i]);
            if (ret != 0)
                goto fail;
        }

        if ((ret = mbedtls_sha1_finish_ret(&ctx, sha1sum)) != 0)
            goto fail;

        if (memcmp(sha1sum, sha1_test_sum[i], 20) != 0) {
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
    mbedtls_sha1_free(&ctx);
    return ret;
}

#endif /* MBEDTLS_SHA1_ALT */
#endif /* MBEDTLS_SHA1_C */
