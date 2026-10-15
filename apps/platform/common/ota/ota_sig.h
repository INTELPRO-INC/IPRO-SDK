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

/*
 * OTA package signature block (review C5) — shared, pure logic + mbedTLS.
 *
 * Lives in the 512 B OTA header's reserved area, which boot2 and older
 * applications ignore, so signed packages stay installable everywhere:
 *
 *   [96..99]    "GSG1" signature-block magic
 *   [100..103]  product id   (LE u32, OTA_PRODUCT_*)
 *   [104..107]  hardware id  (LE u32, 0 = any)
 *   [108..111]  fw version   (LE u32, monotonic; anti-rollback)
 *   [112..115]  unpacked image length (LE u32; must fit the execute slot)
 *   [116..119]  key id       (LE u32)
 *   [120..183]  ECDSA-P256 signature, r||s (2 x 32 B big-endian)
 *
 * The signature covers SHA-256(header[0..120)), which already contains the
 * payload's SHA-256 at [64..96) — so it authenticates the whole package.
 * Produced by tools/create_ota.py --sign-key.
 */
#ifndef OTA_SIG_H
#define OTA_SIG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define OTA_SIG_MAGIC        "GSG1"
#define OTA_SIG_OFFSET       96
#define OTA_SIG_SIGNED_LEN   120     /* header bytes covered by the signature */
#define OTA_SIG_RS_OFFSET    120
#define OTA_SIG_RS_LEN       64

#define OTA_PRODUCT_GATEWAY      0x57474754u   /* gateway product ID, "TGGW" little-endian */

typedef struct {
    uint32_t product_id;
    uint32_t hw_id;
    uint32_t fw_version;
    uint32_t unpacked_len;
    uint32_t key_id;
} ota_sig_info_t;

typedef struct {
    uint32_t key_id;
    uint8_t  pub[65];            /* uncompressed SEC1 point: 0x04 || X || Y */
} ota_pubkey_t;

enum {
    OTA_SIG_OK          = 0,
    OTA_SIG_ABSENT      = -1,    /* no signature block (unsigned package) */
    OTA_SIG_BAD_PRODUCT = -2,
    OTA_SIG_BAD_HW      = -3,
    OTA_SIG_ROLLBACK    = -4,
    OTA_SIG_TOO_BIG     = -5,
    OTA_SIG_UNKNOWN_KEY = -6,
    OTA_SIG_BAD_SIG     = -7,
};

/** Read the signature block. OTA_SIG_OK or OTA_SIG_ABSENT. */
int ota_sig_parse(const uint8_t hdr[512], ota_sig_info_t *out);

/**
 * Policy checks that need only the header (do them on the first chunk,
 * before anything is erased): product, hardware, version >= @p cur_version,
 * unpacked length <= @p exec_slot_len, key id known.
 */
int ota_sig_check_policy(const uint8_t hdr[512], uint32_t product_id,
                         uint32_t hw_id, uint32_t cur_version,
                         uint32_t exec_slot_len,
                         const ota_pubkey_t *keys, size_t nkeys);

/** Verify the ECDSA signature against the key named by the block. */
int ota_sig_verify(const uint8_t hdr[512], const ota_pubkey_t *keys, size_t nkeys);

#endif /* OTA_SIG_H */
