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
#include "drv_glb.h"
#include "drv_glb_gpio.h"
#include "drv_sflash.h"
#include "drv_sf_ctrl.h"

/* Flash option sf2 */
/* Flash CLK */
#define IPRO_EXTFLASH_CLK0_GPIO   GLB_GPIO_PIN_8
/* FLASH CS */
#define IPRO_EXTFLASH_CS0_GPIO    GLB_GPIO_PIN_4
/* FLASH DATA */
#define IPRO_EXTFLASH_DATA00_GPIO GLB_GPIO_PIN_7
#define IPRO_EXTFLASH_DATA10_GPIO GLB_GPIO_PIN_5
#define IPRO_EXTFLASH_DATA20_GPIO GLB_GPIO_PIN_6
#define IPRO_EXTFLASH_DATA30_GPIO GLB_GPIO_PIN_9
/* Flash option sf3 */
/* Flash CLK */
#define IPRO_EXTFLASH_CLK1_GPIO   GLB_GPIO_PIN_11
/* FLASH CS */
#define IPRO_EXTFLASH_CS1_GPIO    GLB_GPIO_PIN_15
/* FLASH DATA */
#define IPRO_EXTFLASH_DATA01_GPIO GLB_GPIO_PIN_12
#define IPRO_EXTFLASH_DATA11_GPIO GLB_GPIO_PIN_14
#define IPRO_EXTFLASH_DATA21_GPIO GLB_GPIO_PIN_13
#define IPRO_EXTFLASH_DATA31_GPIO GLB_GPIO_PIN_10
#define IPRO_FLASH_ID_VALID_FLAG  0x80000000
#define IPRO_FLASH_ID_VALID_MASK  0x7FFFFFFF

int SF_Cfg_Get_Flash_Cfg_Need_Lock(uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg);
int SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg);
int SF_Cfg_Init_Flash_Gpio(uint8_t flashPinCfg, uint8_t restoreDefault);
int SF_Cfg_Init_Flash2_Gpio(uint8_t swap);
int SF_Cfg_Init_Ext_Flash_Gpio(uint8_t extFlashPin);
int SF_Cfg_Deinit_Ext_Flash_Gpio(uint8_t extFlashPin);
uint32_t SF_Cfg_Flash_Identify(uint8_t callFromFlash, uint8_t flashPinCfg, uint8_t restoreDefault,
                               SPI_Flash_Cfg_Type *pFlashCfg, uint8_t group, uint8_t bank);
uint32_t SF_Cfg_Flash_Identify_Ext(uint8_t callFromFlash, uint8_t flashPinCfg, uint8_t restoreDefault,
                                   SPI_Flash_Cfg_Type *pFlashCfg, uint8_t group, uint8_t bank);
int SF_Cfg_Flash_Init(uint8_t sel, const SF_Ctrl_Cfg_Type *pSfCtrlCfg, const SF_Ctrl_Bank2_Cfg *pBank2Cfg);
int SF_Cfg_Sbus2_Flash_Init(uint8_t sel, const SF_Ctrl_Bank2_Cfg *pBank2Cfg);

#endif /* __DRV_SF_CFG_H__ */
