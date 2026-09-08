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
 * @file app_header.h
 * @brief IPRO u-boot application image envelope header
 *
 * 256-byte envelope prepended to an existing IPNP-bootheader app binary so
 * that boot2 (u-boot) can:
 *   1. Detect whether the partition holds an envelope-wrapped image
 *   2. Verify payload integrity via CRC32 before jumping
 *   3. Carry optional metadata (version, name, encryption hook) without
 *      touching the inner IPNP bootheader structure
 *
 * Layout on flash (user_app partition):
 *
 *   [0x000 .. 0x0FF]   app_header_t (this struct, 256 B)
 *   [0x100 ..      ]   payload = original build output (IPNP bootheader + app)
 *
 * Legacy apps (no envelope) keep IPNP magic 0x504E5049 at offset 0 and are
 * still bootable — boot2 falls back to the pre-existing IPNP flow when the
 * envelope magic is absent.
 */
#ifndef APP_HEADER_H
#define APP_HEADER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Envelope magic: 'I','O','B','U' little-endian == 0x55424F49 */
#define APP_HEADER_MAGIC            0x55424F49u

/** Legacy inner bootheader magic: 'I','P','N','P' little-endian */
#define APP_HEADER_LEGACY_IPNP_MAGIC 0x504E5049u

#define APP_HEADER_VERSION          1u
#define APP_HEADER_SIZE             256u
#define APP_HEADER_VERSION_STR_LEN  24u
#define APP_HEADER_NAME_LEN         32u
#define APP_HEADER_IV_LEN           16u
#define APP_HEADER_RESERVED_LEN     148u

/* Flag bits */
#define APP_HEADER_FLAG_ENCRYPTED   (1u << 0)
#define APP_HEADER_FLAG_COMPRESSED  (1u << 1)  /* reserved, not used in P1 */

/**
 * Envelope header. All fields little-endian. Total size must equal
 * APP_HEADER_SIZE (256 B). Reserved bytes MUST be zero-filled.
 */
typedef struct __attribute__((packed)) {
    uint32_t magic;                                 /* 0x00 APP_HEADER_MAGIC */
    uint16_t header_version;                        /* 0x04 =APP_HEADER_VERSION */
    uint16_t header_length;                         /* 0x06 =APP_HEADER_SIZE (256) */

    uint32_t payload_offset;                        /* 0x08 bytes from header start to payload (=256) */
    uint32_t payload_length;                        /* 0x0C payload size in bytes (wrapped .bin size) */
    uint32_t payload_crc32;                         /* 0x10 CRC-32/IEEE over payload */
    uint32_t build_timestamp;                       /* 0x14 Unix epoch seconds */

    uint8_t  app_version[APP_HEADER_VERSION_STR_LEN]; /* 0x18 semver string, null-padded */
    uint8_t  app_name[APP_HEADER_NAME_LEN];           /* 0x30 identifier, null-padded */

    uint32_t flags;                                 /* 0x50 APP_HEADER_FLAG_* */
    uint32_t encryption_key_id;                     /* 0x54 AES key slot ID, 0=plaintext */
    uint8_t  encryption_iv[APP_HEADER_IV_LEN];      /* 0x58 AES IV, zero when plaintext */

    uint8_t  reserved[APP_HEADER_RESERVED_LEN];     /* 0x68 zero-fill for future use */

    uint32_t header_crc32;                          /* 0xFC CRC-32/IEEE over bytes [0x00..0xFB] */
} app_header_t;

/* Compile-time layout guarantees — build fails if struct drifts */
#ifdef __cplusplus
static_assert(sizeof(app_header_t) == APP_HEADER_SIZE, "app_header_t must be 256 bytes");
#else
_Static_assert(sizeof(app_header_t) == APP_HEADER_SIZE, "app_header_t must be 256 bytes");
_Static_assert(__builtin_offsetof(app_header_t, payload_offset)    == 0x08, "payload_offset offset");
_Static_assert(__builtin_offsetof(app_header_t, payload_length)    == 0x0C, "payload_length offset");
_Static_assert(__builtin_offsetof(app_header_t, payload_crc32)     == 0x10, "payload_crc32 offset");
_Static_assert(__builtin_offsetof(app_header_t, build_timestamp)   == 0x14, "build_timestamp offset");
_Static_assert(__builtin_offsetof(app_header_t, app_version)       == 0x18, "app_version offset");
_Static_assert(__builtin_offsetof(app_header_t, app_name)          == 0x30, "app_name offset");
_Static_assert(__builtin_offsetof(app_header_t, flags)             == 0x50, "flags offset");
_Static_assert(__builtin_offsetof(app_header_t, encryption_key_id) == 0x54, "encryption_key_id offset");
_Static_assert(__builtin_offsetof(app_header_t, encryption_iv)     == 0x58, "encryption_iv offset");
_Static_assert(__builtin_offsetof(app_header_t, reserved)          == 0x68, "reserved offset");
_Static_assert(__builtin_offsetof(app_header_t, header_crc32)      == 0xFC, "header_crc32 offset");
#endif

/** Verification result codes. */
typedef enum {
    APP_HDR_OK                  = 0,  /* envelope present and valid */
    APP_HDR_LEGACY_IPNP         = 1,  /* no envelope, legacy IPNP image at offset 0 */
    APP_HDR_ERR_MAGIC           = -1, /* unknown / blank flash (likely 0xFF) */
    APP_HDR_ERR_VERSION         = -2, /* header_version not supported */
    APP_HDR_ERR_LENGTH          = -3, /* header_length != APP_HEADER_SIZE or payload overflow */
    APP_HDR_ERR_HEADER_CRC      = -4, /* envelope header CRC mismatch */
    APP_HDR_ERR_PAYLOAD_CRC     = -5, /* payload CRC mismatch (plaintext only) */
    APP_HDR_ERR_ENCRYPTION      = -6, /* encrypted image but AES not supported in this boot2 */
} app_hdr_status_t;

/**
 * Verify envelope integrity in RAM.
 *
 * @param hdr           Pointer to 256-byte header candidate in RAM.
 * @param payload       Pointer to payload bytes (may be NULL to skip payload CRC).
 * @param max_payload   Upper bound on payload size (for overflow guard).
 * @param aes_supported true if this bootloader build can decrypt encrypted images.
 * @return              APP_HDR_OK on success, APP_HDR_LEGACY_IPNP when legacy
 *                      magic is seen, or a negative error code.
 *
 * On APP_HDR_LEGACY_IPNP the caller should treat @p hdr as the IPNP bootheader
 * base (i.e. run the pre-u-boot flow unchanged).
 *
 * Payload CRC is skipped for encrypted images — decryption must happen first.
 */
app_hdr_status_t app_header_verify(const app_header_t *hdr,
                                   const void *payload,
                                   uint32_t max_payload,
                                   bool aes_supported);

/**
 * CRC-32/IEEE (polynomial 0xEDB88320, init/xor 0xFFFFFFFF). Matches
 * components/softcrc Utils_Soft_CRC32() and Python zlib.crc32().
 *
 * Provided as a weak symbol so the linker can resolve it to softcrc's
 * implementation when COMPONENTS_SOFTCRC_ENABLE=y, or the verify unit can
 * pull in its own copy for host-side testing.
 */
uint32_t app_header_crc32(const void *data, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* APP_HEADER_H */
