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

#ifndef __HAL_FLASH__H__
#define __HAL_FLASH__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "drv_sflash.h"
#include "drv_sflash_ext.h"

#define FLASH_NOT_DETECT  0x10
#if defined(IPRO7)
#define FLASH_XIP_BASE DRV_FLASH_XIP_BASE
#elif defined(IPRO6)
#define FLASH_XIP_BASE IPRO6_FLASH_XIP_BASE
#else
#define FLASH_XIP_BASE DRV_FLASH_XIP_BASE
#endif
#define FLASH_MAGIC_CODE  0x47464346

uint32_t flash_get_jedecid(void);
uint32_t flash_get_size(void);
int flash_init(void);
int flash_read_jedec_id(uint8_t *data);
int flash_read_via_xip(uint32_t addr, uint8_t *data, uint32_t len);
int flash_read(uint32_t addr, uint8_t *data, uint32_t len);
int flash_write(uint32_t addr, uint8_t *data, uint32_t len);
int flash_erase(uint32_t startaddr, uint32_t len);
int flash_set_cache(uint8_t cont_read, uint8_t cache_enable, uint8_t cache_way_disable, uint32_t flash_offset);
int flash_get_cfg(uint8_t **cfg_addr, uint32_t *len);
uint32_t flash_readReg(uint8_t readRegCmd, uint8_t len);
void flash_writeReg(uint8_t writeRegCmd, uint32_t writeVal, uint8_t len);
int hal_flash_config_update(uint32_t flashCfgAddr);

#ifdef __cplusplus
}
#endif
#endif
