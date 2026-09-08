/**
 * @file aes_alt.h
 * @brief Alternative AES implementation for IPRO7 using HAL API
 */

#ifndef MBEDTLS_AES_ALT_H
#define MBEDTLS_AES_ALT_H

#include "hal_sec_aes.h"

/**
 * @brief AES context structure for IPRO7 hardware acceleration using HAL
 * 
 * Stores only the key and key size. For each encrypt/decrypt operation,
 * a temporary sec_aes_handle_t is created on stack using DIRECT mode.
 */
typedef struct mbedtls_aes_context
{
    uint8_t key[32];                      /**< Key storage (up to 256-bit) */
    unsigned int keybits;                 /**< Key size in bits (128/192/256) */
}
mbedtls_aes_context;

#if defined(MBEDTLS_CIPHER_MODE_XTS)
/**
 * @brief AES XTS context structure
 */
typedef struct mbedtls_aes_xts_context
{
    mbedtls_aes_context crypt; /**< AES context for block encryption/decryption */
    mbedtls_aes_context tweak; /**< AES context for tweak computation */
} mbedtls_aes_xts_context;
#endif /* MBEDTLS_CIPHER_MODE_XTS */

#endif /* MBEDTLS_AES_ALT_H */
