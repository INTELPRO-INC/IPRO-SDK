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
 * Transport-agnostic OTA staging core (see ota_common.h).
 */

/* The OTA package header, read without any HAL: host tests build this file alone. */
#include <string.h>

#include "../include/ipro_ota.h"

#define OTA_MAGIC           "IPRO_OTA_Ver1.0"
#define OTA_TYPE_OFFSET     16
#define OTA_PAYLEN_OFFSET   20
#define OTA_HWVER_OFFSET    32
#define OTA_FWVER_OFFSET    48
#define OTA_SHA256_OFFSET   64
#define OTA_SIG_OFFSET      96

static void text16(char out[17], const uint8_t *field)
{
    memcpy(out, field, 16);
    out[16] = '\0';
}

int ota_pkg_parse(const uint8_t hdr[OTA_HEADER_SIZE], ota_pkg_info_t *out)
{
    if (memcmp(hdr, OTA_MAGIC, sizeof(OTA_MAGIC) - 1) != 0) {
        return OTA_PKG_BAD_MAGIC;
    }
    if (memcmp(&hdr[OTA_TYPE_OFFSET], "XZ  ", 4) == 0) {
        out->type = OTA_PKG_TYPE_XZ;
    } else if (memcmp(&hdr[OTA_TYPE_OFFSET], "RAW ", 4) == 0) {
        out->type = OTA_PKG_TYPE_RAW;
    } else {
        return OTA_PKG_BAD_TYPE;
    }
    out->payload_len = (uint32_t)hdr[OTA_PAYLEN_OFFSET] | ((uint32_t)hdr[OTA_PAYLEN_OFFSET + 1] << 8) |
                       ((uint32_t)hdr[OTA_PAYLEN_OFFSET + 2] << 16) | ((uint32_t)hdr[OTA_PAYLEN_OFFSET + 3] << 24);
    if (out->payload_len == 0) {
        return OTA_PKG_BAD_LENGTH;
    }
    text16(out->hw_version, &hdr[OTA_HWVER_OFFSET]);
    text16(out->fw_version, &hdr[OTA_FWVER_OFFSET]);
    memcpy(out->sha256, &hdr[OTA_SHA256_OFFSET], sizeof(out->sha256));
    out->has_signature = memcmp(&hdr[OTA_SIG_OFFSET], "GSG1", 4) == 0;
    return OTA_PKG_OK;
}
