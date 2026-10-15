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

void rnms_handle_ota(rnms_t *rnm, rnm_msg_t *cmd);
/* Keeps the BROM flash config; call before the heap is set up. */
void rnm_ota_capture_flash_cfg(void);
/* "brom", "rom" or "none": where the flash config came from. */
const char *rnm_ota_flash_cfg_source(void);
/* Takes boot2's result of an OTA on this boot (and clears it); call once at
 * boot. Returns the boot2 reason code, 0 if none or installed. */
uint8_t rnm_ota_capture_boot2_result(void);
