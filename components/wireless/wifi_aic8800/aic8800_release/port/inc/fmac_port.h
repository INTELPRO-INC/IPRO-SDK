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
 * Shim header replacing platform/fmac/fmac_port.h
 * Firmware loading function declarations.
 */
#ifndef _AIC_LOADFW_H_
#define _AIC_LOADFW_H_

#include "aic_porting_types.h"

int aicwf_rwnx_load_firmware(uint16_t chipid, int mode, uint32_t **fw_buf);
void aicwf_rwnx_restore_firmware(uint32_t **fw_buf);
int aicwf_rwnx_load_calib(uint16_t chipid, uint32_t **fw_buf);
int aicwf_rwnx_load_patch_tbl(uint16_t chipid, uint32_t **fw_buf);
uint32_t aicwf_get_wifi_fw_type(void);

#endif /* _AIC_LOADFW_H_ */
