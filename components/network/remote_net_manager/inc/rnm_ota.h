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

#pragma once

#include <rnm_server.h>

#include <utils_sha256.h>
#include <hal_boot2.h>
#include <bl_mtd.h>

typedef struct
{
    iot_sha256_context sha_ctx;
    uint8_t sha256_this[32];
    uint8_t sha256_that[32];     // ota header advertised checksum
    bl_mtd_handle_t mtd_hdl;
    uint32_t offset_written;
    uint32_t body_length;        // ota header advertised body size
    uint32_t tgt_part_size;      // target partition size
    uint8_t *write_buf;
    HALPartition_Entry_Config ptEntry;
} rnm_ota_t;

void rnms_handle_ota(rnms_t *rnm, rnm_msg_t *cmd);
