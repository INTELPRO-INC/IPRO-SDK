/**
 * @file sha1_alt.h
 * @brief Alternative SHA-1 implementation using IPRO7 hardware acceleration
 *
 * @warning SHA-1 is considered weak and should only be used for legacy compatibility
 */

#ifndef MBEDTLS_SHA1_ALT_H
#define MBEDTLS_SHA1_ALT_H

#include "hal_sec_hash.h"

typedef struct mbedtls_sha1_context
{
    sec_hash_handle_t handle;   /**< Per-context HW hash handle (link mode) */
}
mbedtls_sha1_context;

#endif /* MBEDTLS_SHA1_ALT_H */
