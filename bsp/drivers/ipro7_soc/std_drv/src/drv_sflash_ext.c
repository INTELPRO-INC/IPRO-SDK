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

#include "drv_sflash_ext.h"
#include "drv_sf_ctrl.h"
#include "l1c_reg.h"

int ATTR_TCM_SECTION SFlash_KH25V40_Write_Protect(SPI_Flash_Cfg_Type *flashCfg, SFlash_Protect_Kh25v40_Type protect)
{
    uint32_t stat = 0, ret;

    SFlash_Read_Reg(flashCfg, 0, (uint8_t *)&stat, 1);
    if (((stat >> 2) & 0xf) == protect) {
        return SUCCESS;
    }

    stat |= ((protect << 2) & 0xff);

    ret = SFlash_Write_Enable(flashCfg);
    if (SUCCESS != ret) {
        return ERROR;
    }

    SFlash_Write_Reg(flashCfg, 0, (uint8_t *)&stat, 1);
    SFlash_Read_Reg(flashCfg, 0, (uint8_t *)&stat, 1);
    if (((stat >> 2) & 0xf) == protect) {
        return SUCCESS;
    }

    return ERROR;
}

int ATTR_TCM_SECTION SFlash_Clear_Status_Register(SPI_Flash_Cfg_Type *pFlashCfg)
{
    uint32_t ret = 0;
    uint32_t qeValue = 0;
    uint32_t regValue = 0;
    uint32_t readValue = 0;
    uint8_t readRegValue0 = 0;
    uint8_t readRegValue1 = 0;

    if((pFlashCfg->ioMode&0xf)==SF_CTRL_QO_MODE || (pFlashCfg->ioMode&0xf)==SF_CTRL_QIO_MODE){
        qeValue = 1;
    }

    SFlash_Read_Reg(pFlashCfg, 0, (uint8_t *)&readRegValue0, 1);
    SFlash_Read_Reg(pFlashCfg, 1, (uint8_t *)&readRegValue1, 1);
    readValue = (readRegValue0|(readRegValue1<<8));
    if ((readValue & (~((1<<(pFlashCfg->qeIndex*8+pFlashCfg->qeBit)) |
                        (1<<(pFlashCfg->busyIndex*8+pFlashCfg->busyBit)) |
                        (1<<(pFlashCfg->wrEnableIndex*8+pFlashCfg->wrEnableBit))))) == 0){
        return SUCCESS;
    }

    ret = SFlash_Write_Enable(pFlashCfg);
    if (SUCCESS != ret) {
        return ERROR;
    }
    if (pFlashCfg->qeWriteRegLen == 2) {
        regValue = (qeValue<<(pFlashCfg->qeIndex*8+pFlashCfg->qeBit));
        SFlash_Write_Reg(pFlashCfg, 0, (uint8_t *)&regValue, 2);
    } else {
        if (pFlashCfg->qeIndex == 0) {
            regValue = (qeValue<<pFlashCfg->qeBit);
        } else {
            regValue = 0;
        }
        SFlash_Write_Reg(pFlashCfg, 0, (uint8_t *)&regValue, 1);
        ret = SFlash_Write_Enable(pFlashCfg);
        if (SUCCESS != ret) {
            return ERROR;
        }
        if (pFlashCfg->qeIndex == 1) {
            regValue = (qeValue<<pFlashCfg->qeBit);
        } else {
            regValue = 0;
        }
        SFlash_Write_Reg(pFlashCfg, 1, (uint8_t *)&regValue, 1);
    }
    return SUCCESS;
}

