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
 * @file app_header_verify.c
 * @brief IPRO u-boot envelope header verification logic
 *
 * Shared by boot2 and host-side unit tests. All operations are RAM-only —
 * flash reads and caching are the caller's responsibility.
 */

#include "app_header.h"

#include <stddef.h>
#include <string.h>

/* Pull in CRC32 from softcrc when compiled for target; host-side test
 * pulls in its own standalone copy. */
#if defined(APP_HEADER_USE_SOFTCRC)
#include "softcrc.h"
uint32_t app_header_crc32(const void *data, uint32_t length)
{
    return Utils_Soft_CRC32((void *)data, length);
}
#elif defined(APP_HEADER_STANDALONE_CRC)
/* Host-side / boot2 self-contained CRC-32/IEEE (no table — size over speed). */
uint32_t app_header_crc32(const void *data, uint32_t length)
{
    const uint8_t *p = (const uint8_t *)data;
    uint32_t crc = 0xFFFFFFFFu;
    while (length--) {
        crc ^= *p++;
        for (int i = 0; i < 8; i++) {
            crc = (crc >> 1) ^ (0xEDB88320u & -(crc & 1u));
        }
    }
    return ~crc;
}
#else
#error "Define APP_HEADER_USE_SOFTCRC or APP_HEADER_STANDALONE_CRC"
#endif

app_hdr_status_t app_header_verify(const app_header_t *hdr,
                                   const void *payload,
                                   uint32_t max_payload,
                                   bool aes_supported)
{
    if (hdr == NULL) {
        return APP_HDR_ERR_MAGIC;
    }

    /* 1. Magic check — distinguish envelope / legacy / blank. */
    const uint32_t magic = hdr->magic;
    if (magic == APP_HEADER_LEGACY_IPNP_MAGIC) {
        return APP_HDR_LEGACY_IPNP;
    }
    if (magic != APP_HEADER_MAGIC) {
        return APP_HDR_ERR_MAGIC;
    }

    /* 2. Header self-consistency. */
    if (hdr->header_version != APP_HEADER_VERSION) {
        return APP_HDR_ERR_VERSION;
    }
    if (hdr->header_length != APP_HEADER_SIZE ||
        hdr->payload_offset != APP_HEADER_SIZE) {
        return APP_HDR_ERR_LENGTH;
    }
    if (hdr->payload_length == 0 || hdr->payload_length > max_payload) {
        return APP_HDR_ERR_LENGTH;
    }

    /* 3. Header CRC — covers bytes [0 .. APP_HEADER_SIZE - 4). */
    const uint32_t hdr_crc_expected = hdr->header_crc32;
    const uint32_t hdr_crc_actual   = app_header_crc32(hdr, APP_HEADER_SIZE - 4);
    if (hdr_crc_actual != hdr_crc_expected) {
        return APP_HDR_ERR_HEADER_CRC;
    }

    /* 4. Encryption support gate — plaintext payload CRC happens below; for
     *    encrypted images the CRC covers ciphertext but we only validate it
     *    after decryption (caller's responsibility). */
    const bool encrypted = (hdr->flags & APP_HEADER_FLAG_ENCRYPTED) != 0;
    if (encrypted && !aes_supported) {
        return APP_HDR_ERR_ENCRYPTION;
    }

    /* 5. Payload CRC for plaintext images. Caller may pass payload=NULL to
     *    skip (e.g. boot2 could defer until post-XIP-map for speed). */
    if (!encrypted && payload != NULL) {
        const uint32_t payload_crc = app_header_crc32(payload, hdr->payload_length);
        if (payload_crc != hdr->payload_crc32) {
            return APP_HDR_ERR_PAYLOAD_CRC;
        }
    }

    return APP_HDR_OK;
}
