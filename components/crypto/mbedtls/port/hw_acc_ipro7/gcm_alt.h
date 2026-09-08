/*
 *  GCM hardware acceleration context for IPRO7
 *
 *  AES-GCM = AES-CTR (encryption) + GHASH (authentication)
 *  IPRO7 provides separate AES and GMAC hardware engines.
 *
 *  Copyright (C) 2025, INTELPRO Inc.
 *  SPDX-License-Identifier: Apache-2.0
 */

#ifndef MBEDTLS_GCM_ALT_H
#define MBEDTLS_GCM_ALT_H

#include "hal_sec_aes.h"
#include "hal_sec_gmac.h"

typedef struct mbedtls_gcm_context {
    sec_gmac_handle_t gmac_handle;  /* GMAC handle for GHASH */
    uint8_t h_key[16];             /* GHASH subkey H = AES_K(0^128) */
    uint8_t aes_key[32];           /* AES key storage (up to 256 bits) */
    unsigned int keybits;          /* Key size in bits (128/192/256) */
    uint64_t len;                  /* Total ciphertext length in bytes */
    uint64_t add_len;              /* Total AAD length in bytes */
    unsigned char base_ectr[16];   /* AES_K(J0) for final tag XOR */
    unsigned char y[16];           /* Counter block J (incremented for CTR) */
    unsigned char buf[16];         /* Partial keystream block buffer */
    int mode;                      /* MBEDTLS_GCM_ENCRYPT or DECRYPT */
} mbedtls_gcm_context;

#endif /* MBEDTLS_GCM_ALT_H */
