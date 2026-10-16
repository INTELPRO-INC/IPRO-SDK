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

/**
 * \file mbedtls_agora_config.h
 *
 * \brief Minimal mbedTLS config for this app
 *
 * Optimized for minimum flash footprint on IPRO7 (~80-100KB).
 * The app opens no TLS session: Sentino MQTT runs on plain TCP and the Agora
 * SDK brings its own transport. mbedTLS is used for the Sentino login
 * signature (HMAC-SHA1). The TLS 1.2 client options below are kept for an
 * HTTPS client; one that talks to a real server must turn certificate
 * verification on (MBEDTLS_X509_CRT_PARSE_C, a root CA, and
 * MBEDTLS_SSL_VERIFY_REQUIRED) - this config does not.
 */

#ifndef MBEDTLS_AGORA_CONFIG_H
#define MBEDTLS_AGORA_CONFIG_H

/* ========================================================================
 * System support
 * ======================================================================== */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_NO_UDBL_DIVISION

/* Platform — use stdlib calloc/free (mapped to FreeRTOS heap via newlib) */
#define MBEDTLS_PLATFORM_C

/* No filesystem, no time, no POSIX signals */
/* MBEDTLS_FS_IO — explicitly NOT defined */
/* MBEDTLS_HAVE_TIME — not defined (no time_t needed) */

/* ========================================================================
 * Entropy & DRBG
 * ======================================================================== */
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_CTR_DRBG_USE_128_BIT_KEY

/* ========================================================================
 * Symmetric ciphers — AES + GCM only
 * ======================================================================== */
#define MBEDTLS_AES_C
#define MBEDTLS_GCM_C
#define MBEDTLS_CIPHER_C

/* Enable AES-CBC for wider server compatibility (some servers use
 * TLS_RSA_WITH_AES_128_CBC_SHA256 or similar) */
#define MBEDTLS_CIPHER_MODE_CBC

/* ========================================================================
 * Hashes — SHA-256 (TLS 1.2 mandatory) + SHA-1 (HMAC-SHA1 for Sentino auth)
 * ======================================================================== */
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_MD_C

/* ========================================================================
 * Public key — RSA + ECDHE (most HTTPS servers)
 * ======================================================================== */
#define MBEDTLS_RSA_C
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C

/* ECDH for ECDHE key exchange */
#define MBEDTLS_ECDH_C
#define MBEDTLS_ECP_C
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED

/* ========================================================================
 * ASN.1 — needed by RSA/ECC key parsing
 * ======================================================================== */
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C

/* ========================================================================
 * X.509 — minimal (parse only, no write, no CRL)
 * We still need basic x509 for the SSL handshake even with VERIFY_NONE
 * ======================================================================== */
#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_BASE64_C

/* ========================================================================
 * TLS 1.2 Client
 * ======================================================================== */
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_PROTO_TLS1_2

/* Cipher suites — only modern ones */
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED

/* ECDSA for ECDHE_ECDSA key exchange */
#define MBEDTLS_ECDSA_C

/* ========================================================================
 * Network I/O (lwIP sockets)
 * MBEDTLS_NET_C is defined by the component CMakeLists.txt when CONFIG_LWIP=y
 * ======================================================================== */

/* ========================================================================
 * Platform utilities
 * ======================================================================== */
#define MBEDTLS_PLATFORM_UTIL_C

/* ========================================================================
 * SSL tuning — reduce RAM usage
 * ======================================================================== */
#define MBEDTLS_SSL_MAX_CONTENT_LEN     4096
#define MBEDTLS_SSL_IN_CONTENT_LEN      4096
#define MBEDTLS_SSL_OUT_CONTENT_LEN     2048

/* MPI (bignum) window size — smaller = less RAM, slightly slower */
#define MBEDTLS_MPI_WINDOW_SIZE         2
#define MBEDTLS_MPI_MAX_SIZE            512

/* ECP window size — reduce RAM for ECDHE */
#define MBEDTLS_ECP_WINDOW_SIZE         2
#define MBEDTLS_ECP_FIXED_POINT_OPTIM   0

/* ========================================================================
 * Explicitly DISABLED features (flash savings)
 * ======================================================================== */

/* No TLS server */
/* MBEDTLS_SSL_SRV_C — not defined */

/* No DTLS */
/* MBEDTLS_SSL_PROTO_DTLS — not defined */

/* No older TLS versions */
/* MBEDTLS_SSL_PROTO_TLS1 — not defined */
/* MBEDTLS_SSL_PROTO_TLS1_1 — not defined */

/* No debug output (save ~5KB) */
/* MBEDTLS_DEBUG_C — not defined */

/* No error strings (save ~5KB) */
/* MBEDTLS_ERROR_C — not defined */

/* No PSA crypto API */
/* MBEDTLS_PSA_CRYPTO_C — not defined */

/* No self-test */
/* MBEDTLS_SELF_TEST — not defined */

/* No version info */
/* MBEDTLS_VERSION_C — not defined */
/* MBEDTLS_VERSION_FEATURES — not defined */

/* No unused ciphers */
/* MBEDTLS_DES_C — not defined */
/* MBEDTLS_ARC4_C — not defined */
/* MBEDTLS_BLOWFISH_C — not defined */
/* MBEDTLS_CAMELLIA_C — not defined */
/* MBEDTLS_ARIA_C — not defined */
/* MBEDTLS_CHACHA20_C — not defined */
/* MBEDTLS_CHACHAPOLY_C — not defined */
/* MBEDTLS_POLY1305_C — not defined */

/* No unused hashes */
/* MBEDTLS_MD5_C — not defined */
/* MBEDTLS_SHA1_C — not defined */
/* MBEDTLS_SHA512_C — not defined */
/* MBEDTLS_RIPEMD160_C — not defined */

/* No DHM (save bignum RAM) */
/* MBEDTLS_DHM_C — not defined */

/* No PKCS */
/* MBEDTLS_PKCS5_C — not defined */
/* MBEDTLS_PKCS12_C — not defined */
/* MBEDTLS_PKCS11_C — not defined */

/* No unused features */
/* MBEDTLS_HKDF_C — not defined */
/* MBEDTLS_HMAC_DRBG_C — not defined */
/* MBEDTLS_CCM_C — not defined */
/* MBEDTLS_CMAC_C — not defined */
/* MBEDTLS_ECJPAKE_C — not defined */
/* MBEDTLS_NIST_KW_C — not defined */
/* MBEDTLS_XTEA_C — not defined */

/* No X.509 write/CRL/CSR */
/* MBEDTLS_X509_CRL_PARSE_C — not defined */
/* MBEDTLS_X509_CSR_PARSE_C — not defined */
/* MBEDTLS_X509_CREATE_C — not defined */
/* MBEDTLS_X509_CRT_WRITE_C — not defined */
/* MBEDTLS_X509_CSR_WRITE_C — not defined */
/* MBEDTLS_PK_WRITE_C — not defined */
/* MBEDTLS_PEM_WRITE_C — not defined */

/* No SSL extras */
/* MBEDTLS_SSL_CACHE_C — not defined */
/* MBEDTLS_SSL_COOKIE_C — not defined */
/* MBEDTLS_SSL_TICKET_C — not defined */
/* MBEDTLS_SSL_EXPORT_KEYS — not defined */

/* No memory buffer allocator (we use FreeRTOS heap) */
/* MBEDTLS_MEMORY_BUFFER_ALLOC_C — not defined */

/* No threading (single-threaded HTTPS calls) */
/* MBEDTLS_THREADING_C — not defined */

/* No HAVEGE */
/* MBEDTLS_HAVEGE_C — not defined */

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_AGORA_CONFIG_H */
