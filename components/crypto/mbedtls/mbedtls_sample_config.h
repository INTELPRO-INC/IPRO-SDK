/* App-level override: if the app provides its own mbedtls_app_config.h,
 * use it instead of this entire default configuration. */
#if __has_include("mbedtls_app_config.h")
#include "mbedtls_app_config.h"
#else

/**
 * \file mbedtls_sample_config.h
 *
 * \brief mbedTLS configuration for IPRO SDK with OpenThread support
 * 
 * This file defines additional mbedTLS features required for OpenThread.
 * It will be included BEFORE the default config.h when MBEDTLS_CONFIG_FILE is set.
 */

#ifndef MBEDTLS_SAMPLE_CONFIG_H
#define MBEDTLS_SAMPLE_CONFIG_H

/* ========================================================================
 * Hardware Crypto Acceleration (when enabled in proj.conf)
 * ======================================================================== */

#if defined(CONFIG_MBEDTLS_AES_USE_HW) && CONFIG_MBEDTLS_AES_USE_HW
#define MBEDTLS_AES_ALT
#endif

/* AES lookup tables live in flash .rodata instead of runtime-generated
 * ram_tcm .bss (8KB: FT0-3/RT0-3). SW AES is only on cold paths here
 * (SAE/EAPOL handshakes; the data path uses MAC HW CCMP), so XIP-cached
 * table reads are an acceptable trade for 8KB of heap headroom. */
#define MBEDTLS_AES_ROM_TABLES

#if defined(CONFIG_MBEDTLS_SHA1_USE_HW) && CONFIG_MBEDTLS_SHA1_USE_HW
#define MBEDTLS_SHA1_ALT
#endif

#if defined(CONFIG_MBEDTLS_SHA256_USE_HW) && CONFIG_MBEDTLS_SHA256_USE_HW
#define MBEDTLS_SHA256_ALT
#endif

#if defined(CONFIG_MBEDTLS_SHA512_USE_HW) && CONFIG_MBEDTLS_SHA512_USE_HW
#define MBEDTLS_SHA512_ALT
#endif

#if defined(CONFIG_MBEDTLS_ECC_USE_HW) && CONFIG_MBEDTLS_ECC_USE_HW
#define MBEDTLS_ECP_ALT
#endif

#if defined(CONFIG_MBEDTLS_GCM_USE_HW) && CONFIG_MBEDTLS_GCM_USE_HW
#define MBEDTLS_GCM_ALT
#endif

/* ========================================================================
 * OpenThread Required Features
 * ======================================================================== */

/* Enable EC-JPAKE key exchange for Thread commissioning */
#define MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
#define MBEDTLS_ECJPAKE_C

/* Enable ECDSA for Thread security */
#define MBEDTLS_ECDSA_C
#define MBEDTLS_ECDSA_DETERMINISTIC

/* Enable required ECC curves */
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED

/* Enable CCM cipher mode for Thread */
#define MBEDTLS_CCM_C

/* Enable CMAC for Thread crypto */
#define MBEDTLS_CMAC_C

/* Enable DTLS for Thread */
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY

/* Enable SSL export keys for Thread */
#define MBEDTLS_SSL_EXPORT_KEYS

/* Route mbedTLS heap allocations to the FreeRTOS heap.
 *
 * Without this, mbedtls_calloc/free resolve to libc calloc()/free() (the SDK
 * port's "TODO: change to ZSP" placeholder). libc calloc uses the unconfigured
 * newlib _sbrk heap, which on this target overlaps live RAM — so every mbedTLS
 * allocation (EC-JPAKE / bignum / DTLS during a Thread MeshCoP join) hands back
 * memory that corrupts the FreeRTOS heap/task state. The damage is latent and
 * only manifests when the scheduler next touches the corrupted region — which
 * is the whole-system freeze seen during OOB Joiner commissioning.
 *
 * pvPortCalloc (heap_5) is overflow-checked and zeroing — a correct calloc on
 * the one real heap. Must be set BEFORE including mbedtls/config.h. */
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void *pvPortCalloc(size_t xNum, size_t xSize);
void  vPortFree(void *pv);
#ifdef __cplusplus
}
#endif
#ifndef MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_C
#endif
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_CALLOC_MACRO pvPortCalloc
#define MBEDTLS_PLATFORM_FREE_MACRO   vPortFree

/* Bound the DTLS/EC-JPAKE memory footprint to match the PROVEN Rafael OT
 * mbedtls-config.h. Without these, mbedTLS defaults SSL_IN/OUT_CONTENT_LEN to
 * 16384 -> a ~32KB twin calloc inside mbedtls_ssl_setup at the START of the
 * MeshCoP Joiner handshake; together with the fixed-point ECP comb table that
 * exhausts the FreeRTOS heap mid-handshake. The failed/over-large allocation
 * leads to a NULL-deref fault whose MCAUSE dump is swallowed by the buffered
 * USB console (the trap runs IRQs-off, the ring never flushes) -> a SILENT
 * whole-system freeze right after "Joiner discover network", before the
 * "Joiner connecting" log can flush. Thread MeshCoP records are small, so the
 * Rafael values (≤900B buffers, no comb table, 256-bit ECP) make the handshake
 * fit. Gated to OT builds so non-OT TLS users (large certs) are unaffected. */
#ifdef CONFIG_OPENTHREAD_ENABLE
#define MBEDTLS_SSL_MAX_CONTENT_LEN 900
#define MBEDTLS_SSL_IN_CONTENT_LEN  900
#define MBEDTLS_SSL_OUT_CONTENT_LEN 900
/* WARNING: this 900B cap bounds EVERY TLS user in an OT build, not just MeshCoP.
 * If you enable gateway MQTT-TLS (CONFIG_GATEWAY_MQTT_TLS), the broker's server
 * certificate CHAIN must fit in 900B plaintext (TLS cannot fragment a handshake
 * message), or the handshake hard-fails — keep the chain < ~850B DER, or raise
 * these (each +N costs ~2N extra heap during the handshake). */
#define MBEDTLS_ECP_WINDOW_SIZE     2
#define MBEDTLS_ECP_FIXED_POINT_OPTIM 0
/* NOTE: ECP_MAX_BITS / MPI_MAX_SIZE are NOT pinned to 256/32 here (the Rafael
 * oracle can, because it enables ONLY secp256r1; this config still enables
 * larger curves, so a 256-bit cap fails the build / breaks them at runtime). */
#endif

/* Disable filesystem I/O — not available on embedded target.
 * This must be undefined AFTER including the base config. */

/* Now include the base mbedTLS configuration */
#include "mbedtls/config.h"

#undef MBEDTLS_FS_IO
#undef MBEDTLS_PSA_ITS_FILE_C

/* Disable mbedTLS wall-clock time for OpenThread builds. The base config.h
 * defines MBEDTLS_HAVE_TIME, which makes ssl_generate_random() call
 * mbedtls_time(NULL) -> newlib time()/_gettimeofday — and this target has no
 * working time syscall, so that call NEVER RETURNS, hanging the DTLS Joiner
 * handshake inside ssl_write_client_hello BEFORE the first f_rng (confirmed by
 * UART0 trace: stops right after handshake state CLIENT_HELLO, no rng/ecp).
 * The proven Rafael OT mbedtls-config.h does not enable HAVE_TIME either; with
 * it off, ssl_generate_random uses f_rng for the ClientHello time field. OT
 * MeshCoP (EC-JPAKE, no X.509 cert validity) does not need real time. */
#ifdef CONFIG_OPENTHREAD_ENABLE
#undef MBEDTLS_HAVE_TIME
#undef MBEDTLS_HAVE_TIME_DATE
/* WARNING: with HAVE_TIME_DATE off, mbedTLS stubs X.509 validity-period checks to
 * always pass. Harmless for MeshCoP (EC-JPAKE, no certs), BUT if you add an X.509
 * TLS user to an OT build (e.g. MQTT-TLS), it will NOT verify certificate expiry.
 * There is no clean fix without a working wall-clock time source on this target —
 * supply one and drop these undefs before relying on X.509 cert validity here. */
#endif

/* ========================================================================
 * Flash size optimization — disable unused features for miniclaw
 * Miniclaw only needs TLS 1.2 client with ECDHE/RSA to reach
 * api.telegram.org and api.anthropic.com.
 * ======================================================================== */

/* TLS server not needed for miniclaw (client-only). BUT the OpenThread
 * MeshCoP COMMISSIONER is a DTLS SERVER — it sends HelloVerifyRequest +
 * ServerHello to the joiner and REQUIRES MBEDTLS_SSL_SRV_C (+ SSL_COOKIE_C,
 * which stays on). Without it, ssl_srv.c is not compiled, the server handshake
 * step returns FEATURE_UNAVAILABLE, and the commissioner instantly drops every
 * joiner (SecTransport Connecting->Disconnecting in ~1ms, joiner never gets a
 * valid HelloVerifyRequest). So strip the TLS server ONLY for non-OT builds. */
#ifndef CONFIG_OPENTHREAD_ENABLE
#undef MBEDTLS_SSL_SRV_C
#endif

/* Obsolete/unused ciphers */
#undef MBEDTLS_DES_C
#undef MBEDTLS_BLOWFISH_C
#undef MBEDTLS_CAMELLIA_C
#undef MBEDTLS_ARC4_C

/* Obsolete hashes — keep MD5 (needed by ssl_internal.h for TLS <1.2 compat) */
#undef MBEDTLS_RIPEMD160_C

/* DHE key exchange not needed — use ECDHE */
#undef MBEDTLS_DHM_C
#undef MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
#undef MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED

/* Debug output not needed in production */
#undef MBEDTLS_DEBUG_C

/* Keep MBEDTLS_SELF_TEST — port/mbedtls_self_test.c depends on it */

/* DTLS and EC-JPAKE only needed for OpenThread (not used in miniclaw).
 * Gate on CONFIG_OPENTHREAD_ENABLE — the build only ever defines that token
 * (bare CONFIG_OPENTHREAD is never emitted by Kconfig/autoconf.h), so the old
 * `#ifndef CONFIG_OPENTHREAD` fired unconditionally and stripped DTLS/EC-JPAKE
 * from EVERY build, including OpenThread apps. That silently broke MeshCoP
 * commissioning (Joiner/Commissioner link but the EC-JPAKE handshake can't run).
 * The MeshCoP ciphers are defined above; keep them for OT apps only. */
#ifndef CONFIG_OPENTHREAD_ENABLE
#undef MBEDTLS_SSL_PROTO_DTLS
#undef MBEDTLS_SSL_DTLS_ANTI_REPLAY
#undef MBEDTLS_SSL_DTLS_HELLO_VERIFY
#undef MBEDTLS_ECJPAKE_C
#undef MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
#undef MBEDTLS_SSL_EXPORT_KEYS
#endif

#endif /* MBEDTLS_SAMPLE_CONFIG_H */

#endif /* __has_include("mbedtls_app_config.h") */
