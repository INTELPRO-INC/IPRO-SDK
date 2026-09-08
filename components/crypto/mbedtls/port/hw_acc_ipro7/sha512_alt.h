/**
 * @file sha512_alt.h
 * @brief Alternative SHA-512/384 implementation using IPRO7 hardware acceleration
 */

#ifndef MBEDTLS_SHA512_ALT_H
#define MBEDTLS_SHA512_ALT_H

#include "hal_sec_hash.h"

typedef struct mbedtls_sha512_context
{
    sec_hash_handle_t handle;   /**< Per-context HW hash handle (link mode) */
    int is384;                  /**< 0 = SHA-512, 1 = SHA-384 */
}
mbedtls_sha512_context;

#endif /* MBEDTLS_SHA512_ALT_H */
