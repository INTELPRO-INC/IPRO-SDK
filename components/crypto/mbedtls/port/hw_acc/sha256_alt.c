/*
 *  FIPS-180-2 compliant SHA-256 implementation
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#include "common.h"

#if defined(MBEDTLS_SHA256_C)

#include "mbedtls/sha256.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#include <string.h>

#include "mbedtls/platform.h"
#include "sec_mutex.h"

#define SHA256_VALIDATE_RET(cond)                           \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_SHA256_BAD_INPUT_DATA )
#define SHA256_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE( cond )

static ATTR_NOCACHE_NOINIT_RAM_SECTION struct ipro_sha256_link_ctx_s link_ctx_temp;

void mbedtls_sha256_init( mbedtls_sha256_context *ctx )
{
    SHA256_VALIDATE( ctx != NULL );

    struct ipro_device_s *sha;

    sha = ipro_device_get_by_name("sha");

    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_sha256_context ) );
    ctx->sha = sha;

    ipro_group0_request_sha_access(sha);
    ipro_sha_link_init(sha);
}

void mbedtls_sha256_free( mbedtls_sha256_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_clone( mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src )
{
    SHA256_VALIDATE( dst != NULL );
    SHA256_VALIDATE( src != NULL );

    *dst = *src;
}

/*
 * SHA-256 context setup
 */
int mbedtls_sha256_starts_ret( mbedtls_sha256_context *ctx, int is224 )
{
    SHA256_VALIDATE_RET( ctx != NULL );
    SHA256_VALIDATE_RET( is224 == 0 || is224 == 1 );

    if(ctx->sha == NULL)
    {
        ctx->sha = ipro_device_get_by_name("sha");
    }

    ipro_sec_sha_mutex_take();
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct ipro_sha256_link_ctx_s));
    ipro_sha256_link_start(ctx->sha, &link_ctx_temp, is224);
    memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct ipro_sha256_link_ctx_s));
    ipro_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha256_starts( mbedtls_sha256_context *ctx,
                            int is224 )
{
    mbedtls_sha256_starts_ret( ctx, is224 );
}
#endif

/*
 * SHA-256 process buffer
 */
int mbedtls_sha256_update_ret( mbedtls_sha256_context *ctx,
                               const unsigned char *input,
                               size_t ilen )
{
    //int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    SHA256_VALIDATE_RET( ctx != NULL );
    SHA256_VALIDATE_RET( ilen == 0 || input != NULL );

    if( ilen == 0 )
        return( 0 );

    ipro_l1c_dcache_clean_range((void *)input, ilen);
    ipro_sec_sha_mutex_take();
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct ipro_sha256_link_ctx_s));
    ipro_sha256_link_update(ctx->sha, &link_ctx_temp, input, ilen);
    memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct ipro_sha256_link_ctx_s));
    ipro_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha256_update( mbedtls_sha256_context *ctx,
                            const unsigned char *input,
                            size_t ilen )
{
    mbedtls_sha256_update_ret( ctx, input, ilen );
}
#endif

/*
 * SHA-256 final digest
 */
int mbedtls_sha256_finish_ret( mbedtls_sha256_context *ctx,
                               unsigned char output[32] )
{
    //int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    SHA256_VALIDATE_RET( ctx != NULL );
    SHA256_VALIDATE_RET( (unsigned char *)output != NULL );

    ipro_sec_sha_mutex_take();
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct ipro_sha256_link_ctx_s));
    ipro_sha256_link_finish(ctx->sha, &link_ctx_temp, output);
    ipro_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha256_finish( mbedtls_sha256_context *ctx,
                            unsigned char output[32] )
{
    mbedtls_sha256_finish_ret( ctx, output );
}
#endif

#endif /* MBEDTLS_SHA256_C */
