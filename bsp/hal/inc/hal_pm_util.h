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

#ifndef __HAL_PM_UTIL_H__
#define __HAL_PM_UTIL_H__

#include "hal_common.h"

#define ATTR_PDS_RAM_SECTION       __attribute__((section(".pds_ram_code")))
#define ATTR_PDS_RAM_CONST_SECTION __attribute__((section(".pds_ram_data")))

uint32_t hal_pds_enter_with_time_compensation(uint32_t pdsLevel, uint32_t pdsSleepCycles);
void pm_set_hardware_recovery_callback(void (*hardware_recovery_cb)(void));
#endif