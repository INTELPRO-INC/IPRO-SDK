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


#ifndef __DRV_XIP_SFLASH_H__
#define __DRV_XIP_SFLASH_H__

#include "drv_common.h"
#include "drv_sflash.h"

int XIP_SFlash_State_Save(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t *offset,
                                  uint8_t group, uint8_t bank);
int XIP_SFlash_State_Restore(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t offset,
                                     uint8_t group, uint8_t bank);
int XIP_SFlash_Erase_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t startaddr,
                                       int len, uint8_t group, uint8_t bank);
int XIP_SFlash_Write_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr,
                                       uint8_t *data, uint32_t len, uint8_t group, uint8_t bank);
int XIP_SFlash_Read_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr,
                                      uint8_t *data, uint32_t len, uint8_t group, uint8_t bank);
int XIP_SFlash_GetJedecId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data,
                                            uint8_t group, uint8_t bank);
int XIP_SFlash_GetDeviceId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, int is32BitsAddr,
                                             uint8_t *data, uint8_t group, uint8_t bank);
int XIP_SFlash_GetUniqueId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data,
                                             uint8_t idLen, uint8_t group, uint8_t bank);
int XIP_SFlash_Read_Via_Cache_Need_Lock(uint32_t addr, uint8_t *data, uint32_t len);
void XIP_SFlash_Opt_Enter(uint8_t *aesEnable);
void XIP_SFlash_Opt_Exit(uint8_t aesEnable);

int XIP_SFlash_Clear_Status_Register_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg,
                                                       uint8_t group, uint8_t bank);

#endif /* __DRV_XIP_SFLASH_H__ */
