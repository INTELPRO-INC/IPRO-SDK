/**
 * @file sha256_alt.h
 * @brief Alternative SHA-256/224 implementation using IPRO7 hardware acceleration
 *
 * Each context embeds a full sec_hash_handle_t so multiple concurrent
 * SHA operations (e.g. TLS handshake hash + certificate verify hash)
 * do not clobber each other.
 */

#ifndef MBEDTLS_SHA256_ALT_H
#define MBEDTLS_SHA256_ALT_H

#include "hal_sec_hash.h"

typedef struct mbedtls_sha256_context
{
    sec_hash_handle_t handle;   /**< Per-context HW hash handle (link mode) */
    int is224;                  /**< 0 = SHA-256, 1 = SHA-224 */
}
mbedtls_sha256_context;

#endif /* MBEDTLS_SHA256_ALT_H */
