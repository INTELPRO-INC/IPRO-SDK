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

#ifndef __DRV_SF_CFG_EXT_H__
#define __DRV_SF_CFG_EXT_H__

#include "string.h"
#include "drv_sflash.h"
#include "drv_sf_ctrl.h"

void SF_Cfg_Init_Internal_Flash_Gpio(void);
int SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg);
uint32_t SF_Cfg_Flash_Identify_Ext(uint8_t callFromFlash, uint32_t autoScan, uint32_t flashPinCfg,
                                   uint8_t restoreDefault, SPI_Flash_Cfg_Type *pFlashCfg);

#endif /* __DRV_SF_CFG_EXT_H__ */
