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

#include "string.h"
#include "drv_xip_sflash_ext.h"

int ATTR_TCM_SECTION XIP_SFlash_KH25V40_Write_Protect_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SFlash_Protect_Kh25v40_Type protect)
{
    int stat;
    uint32_t offset;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode&0xf;

    stat = XIP_SFlash_State_Save(pFlashCfg, &offset, 0, 0);
    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, 0);
    } else {
        stat = SFlash_KH25V40_Write_Protect(pFlashCfg, protect);
        XIP_SFlash_State_Restore(pFlashCfg, offset, 0, 0);
    }

    return stat;
}

int ATTR_TCM_SECTION XIP_SFlash_Clear_Status_Register_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t group, uint8_t bank)
{
    int stat;
    uint32_t offset;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode&0xf;

    stat=XIP_SFlash_State_Save(pFlashCfg, &offset, group, bank);
    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32, 0);
    } else {
        stat=SFlash_Clear_Status_Register(pFlashCfg);
        XIP_SFlash_State_Restore(pFlashCfg, offset, group, bank);
    }

    return stat;
}

