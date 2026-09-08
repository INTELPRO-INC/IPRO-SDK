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

#ifndef __DRV_SFLAH_EXT_H__
#define __DRV_SFLAH_EXT_H__

#include "drv_sflash.h"
#include "drv_common.h"

typedef enum {
    SFLASH_KH25V40_PROTECT_NONE,          /*!< SFlash no protect KH25V40 */
    SFLASH_KH25V40_PROTECT_448KTO512K,    /*!< SFlash protect KH25V40 448K to 512K */
    SFLASH_KH25V40_PROTECT_384KTO512K,    /*!< SFlash protect KH25V40 384K to 512K */
    SFLASH_KH25V40_PROTECT_256KTO512K,    /*!< SFlash protect KH25V40 256K to 512K */
    SFLASH_KH25V40_PROTECT_ALL,           /*!< SFlash protect KH25V40 0K to 512K */
} SFlash_Protect_Kh25v40_Type;

int SFlash_KH25V40_Write_Protect(SPI_Flash_Cfg_Type *flashCfg, SFlash_Protect_Kh25v40_Type protect);
int SFlash_Clear_Status_Register(SPI_Flash_Cfg_Type *pFlashCfg);

#endif /* __DRV_SFLAH_EXT_H__ */
