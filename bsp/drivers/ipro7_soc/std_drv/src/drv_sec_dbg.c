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
#include "drv_sec_dbg.h"

/****************************************************************************/ /**
 * @brief  Sec Dbg read chip ID
 *
 * @param  id[8]: chip ID buffer
 *
 * @return None
 *
*******************************************************************************/
void Sec_Dbg_Read_Chip_ID(uint8_t id[8])
{
    uint32_t idLow, idHigh;

    idLow = readl(SEC_DBG_BASE + SEC_DBG_SD_CHIP_ID_LOW_OFFSET);
    DRV_WORD_TO_BYTES(id, idLow);

    idHigh = readl(SEC_DBG_BASE + SEC_DBG_SD_CHIP_ID_HIGH_OFFSET);
    DRV_WORD_TO_BYTES((id + 4), idHigh);
}

/***************************************************************************** /
 * @brief  Sec Dbg set PWD
 *
 * @param  pwd: PWD buffer
 *
 * @return None
 *
*******************************************************************************/
void Sec_Dbg_Set_Dbg_Pwd(const uint32_t pwd[4])
{
    writel(pwd[0], SEC_DBG_BASE + SEC_DBG_SD_DBG_PWD_LOW_OFFSET);
    writel(pwd[1], SEC_DBG_BASE + SEC_DBG_SD_DBG_PWD_HIGH_OFFSET);
    writel(pwd[2], SEC_DBG_BASE + SEC_DBG_SD_DBG_PWD2_LOW_OFFSET);
    writel(pwd[3], SEC_DBG_BASE + SEC_DBG_SD_DBG_PWD2_HIGH_OFFSET);
}

/***************************************************************************** /
 * @brief  get Sec Dbg state
 *
 * @param  None
 *
 * @return Sec Dbg state
 *
*******************************************************************************/
SEC_DBG_STATE_Type Sec_Dbg_Read_Dbg_State(void)
{
    SEC_DBG_STATE_Type ret;
    uint32_t mode = 0;
    uint32_t ena = 0;

    mode = reg_get_bits(readl(SEC_DBG_BASE + SEC_DBG_SD_STATUS_OFFSET), SEC_DBG_SD_DBG_MODE);
    ena = reg_get_bits(readl(SEC_DBG_BASE + SEC_DBG_SD_STATUS_OFFSET), SEC_DBG_SD_DBG_ENA);

    if(0x0 == mode){
        ret = SEC_DBG_DEBUG_OPEN_MODE;
    }else if((0x1 == mode)||(0x2 == mode)||(0x3 == mode)){
        if(0xF == ena){
            ret = SEC_DBG_DEBUG_OPEN_MODE;
        }else{
            ret = SEC_DBG_DEBUG_PASSWORD_MODE;
        }
    }else{
        ret = SEC_DBG_DEBUG_CLOSE_MODE;
    }

    return ret;
}

/****************************************************************************/ /**
 * @brief  Sec Dbg read debug mode
 *
 * @param  None
 *
 * @return debug mode status
 *
*******************************************************************************/
uint32_t Sec_Dbg_Read_Dbg_Mode(void)
{
    return reg_get_bits(readl(SEC_DBG_BASE + SEC_DBG_SD_STATUS_OFFSET), SEC_DBG_SD_DBG_MODE);
}

/****************************************************************************/ /**
 * @brief  Sec Dbg read debug enable status
 *
 * @param  None
 *
 * @return enable status
 *
*******************************************************************************/
uint32_t Sec_Dbg_Read_Dbg_Enable(void)
{
    return reg_get_bits(readl(SEC_DBG_BASE + SEC_DBG_SD_STATUS_OFFSET), SEC_DBG_SD_DBG_ENA);
}

/****************************************************************************/ /**
 * @brief  Sec Dbg Set debug trigger status
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void Sec_Dbg_Set_Dbg_Trigger(void)
{
    uint32_t tmpVal=0;
    tmpVal=readl(SEC_DBG_BASE + SEC_DBG_SD_STATUS_OFFSET);
    tmpVal=reg_set_bit(tmpVal,SEC_DBG_SD_DBG_PWD_TRIG);
    writel(tmpVal, SEC_DBG_BASE + SEC_DBG_SD_STATUS_OFFSET);
}

/****************************************************************************/ /**
 * @brief  Sec Dbg wait ready
 *
 * @param  None
 *
 * @return busy status
 *
*******************************************************************************/
uint32_t Sec_Dbg_Wait_Ready(void)
{
    uint32_t to = 1024;
    uint32_t busy;

    do{
        busy = reg_get_bits(readl(SEC_DBG_BASE + SEC_DBG_SD_STATUS_OFFSET), SEC_DBG_SD_DBG_PWD_BUSY);
    }while((busy) && (to--));

    return busy;
}

