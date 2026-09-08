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

#ifndef __DRV_XIP_SFLASH_EXT_H__
#define __DRV_XIP_SFLASH_EXT_H__

#include "drv_common.h"
#include "drv_sflash.h"
#include "drv_xip_sflash.h"
#include "drv_sflash.h"
#include "drv_sflash_ext.h"
#include "drv_sf_cfg.h"
#include "drv_sf_cfg_ext.h"

int XIP_SFlash_Clear_Status_Register_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t group, uint8_t bank);

#endif /* __DRV_XIP_SFLASH_EXT_H__ */
