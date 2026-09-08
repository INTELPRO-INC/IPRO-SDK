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

#ifndef __DRV_SF_CFG_H__
#define __DRV_SF_CFG_H__

#include "string.h"
#include "drv_sflash.h"
#include "drv_sf_ctrl.h"

/* Flash option 0 */
/* Flash CLK */
#define EXTFLASH_CLK0_GPIO GLB_GPIO_PIN_26
/* FLASH CS */
#define EXTFLASH_CS0_GPIO GLB_GPIO_PIN_25
/* FLASH DATA */
#define EXTFLASH_DATA00_GPIO GLB_GPIO_PIN_22
#define EXTFLASH_DATA10_GPIO GLB_GPIO_PIN_24
#define EXTFLASH_DATA20_GPIO GLB_GPIO_PIN_23
#define EXTFLASH_DATA30_GPIO GLB_GPIO_PIN_27
/* Flash option 1 */
/* Flash CLK */
#define EXTFLASH_CLK1_GPIO GLB_GPIO_PIN_32
/* FLASH CS */
#define EXTFLASH_CS1_GPIO GLB_GPIO_PIN_31
/* FLASH DATA */
#define EXTFLASH_DATA01_GPIO GLB_GPIO_PIN_28
#define EXTFLASH_DATA11_GPIO GLB_GPIO_PIN_30
#define EXTFLASH_DATA21_GPIO GLB_GPIO_PIN_29
#define EXTFLASH_DATA31_GPIO GLB_GPIO_PIN_33

#define FLASH_CFG_SF1_EMB_1M        0
#define FLASH_CFG_SF2_EXT_22_27     1
#define FLASH_CFG_SF3_EXT_28_33     2

#define FLASH_SF1_SWAP_NONE         0
#define FLASH_SF1_SWAP_CS_IO2       1
#define FLASH_SF1_SWAP_IO0_IO3      2
#define FLASH_SF1_SWAP_BOTH         3
#define FLASH_ID_VALID_FLAG     0x80000000
#define FLASH_ID_VALID_MASK     0x7FFFFFFF

int SF_Cfg_Get_Flash_Cfg_Need_Lock(uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg);
void SF_Cfg_Init_Flash_Gpio(uint8_t flashPinCfg, uint8_t restoreDefault);
uint32_t SF_Cfg_Flash_Identify(uint8_t callFromFlash, uint32_t autoScan, uint32_t flashPinCfg, uint8_t restoreDefault, SPI_Flash_Cfg_Type *pFlashCfg, uint8_t group, uint8_t bank);

void SF_Cfg_Init_Ext_Flash_Gpio(uint8_t extFlashPin);
void SF_Cfg_Deinit_Ext_Flash_Gpio(uint8_t extFlashPin);

#endif /* __DRV_SF_CFG_H__ */
