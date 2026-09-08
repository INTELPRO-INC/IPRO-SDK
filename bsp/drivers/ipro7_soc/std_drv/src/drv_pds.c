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

#include "hbn_reg.h"
#include "ipro7.h"
#include "drv_pds.h"
#include "drv_hbn.h"
#include "drv_ef_cfg.h"
#include "uart_reg.h"

int ATTR_TCM_SECTION PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_Type pin, PDS_GPIO_Cfg_Type *cfg)
{
    uint32_t tmpVal;


    /* pu/pd/ie config */
    tmpVal = readl(PDS_BASE + PDS_GPIO_PU_SET_OFFSET);
    if (cfg->pu_en) {
        tmpVal |= (1 << pin);
    } else {
        tmpVal &= ~(1 << pin);
    }
    writel(tmpVal, PDS_BASE + PDS_GPIO_PU_SET_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_GPIO_PD_SET_OFFSET);
    if (cfg->pd_en) {
        tmpVal |= (1 << pin);
    } else {
        tmpVal &= ~(1 << pin);
    }
    writel(tmpVal, PDS_BASE + PDS_GPIO_PD_SET_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_GPIO_IE_SET_OFFSET);
    if (cfg->ie_en) {
        tmpVal |= (1 << pin);
    } else {
        tmpVal &= ~(1 << pin);
    }
    writel(tmpVal, PDS_BASE + PDS_GPIO_IE_SET_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_GPIO_OE_SET_OFFSET);
    if (cfg->oe_en) {
        tmpVal |= (1 << pin);
    } else {
        tmpVal &= ~(1 << pin);
    }
    writel(tmpVal, PDS_BASE + PDS_GPIO_OE_SET_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_GPIO_Write(PDS_GPIO_GROUP_SET_Type grp, uint32_t val)
{
    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMask(int pin, int intMask)
{
    uint32_t tmpVal;
    uint32_t tmpValMask;

    tmpVal = readl(PDS_BASE + PDS_GPIO_INT_MASK_OFFSET);
    if (intMask) {
        if (pin >= 0 && pin <= 9)
            tmpValMask |= (1 << pin);
        else if (pin >= 20)
            tmpValMask |= (1 << (pin-10));
    } else {
        if (pin >= 0 && pin <= 9)
            tmpValMask &= ~(1 << pin);
        else if (pin >= 20)
            tmpValMask &= ~(1 << (pin-10));
    }
    writel(tmpVal, PDS_BASE + PDS_GPIO_INT_MASK_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_GROUP_SET_Type grp, PDS_GPIO_INT_TRIG_Type trig)
{
    uint32_t tmpVal = 0;


    tmpVal = readl(PDS_BASE + PDS_GPIO_INT_SET_OFFSET);
    tmpVal &= ~(0xF << (grp << 2));
    tmpVal |= (trig << (grp << 2));
    writel(tmpVal, PDS_BASE + PDS_GPIO_INT_SET_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_GROUP_SET_Type grp)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_GPIO_INT_CLR_OFFSET);
    tmpVal |= (1 << grp);
    writel(tmpVal, PDS_BASE + PDS_GPIO_INT_CLR_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Set_All_GPIO_IntClear(void)
{
    writel(0x1FF, PDS_BASE + PDS_GPIO_INT_CLR_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Get_GPIO_Pad_IntStatus(PDS_GPIO_Type pin)
{

    return (readl(PDS_BASE + PDS_GPIO_STAT_OFFSET) & (1 << pin)) ? SET : RESET;
}

int ATTR_TCM_SECTION PDS_Set_Flash_Pad_Pull_None(SF_Ctrl_Pin_Select pinCfg)
{
    PDS_GPIO_Cfg_Type pds_io_cfg={
        .pd_en=0,
        .pu_en=0,
        .ie_en=0,
        .oe_en=0,
        };

    if (pinCfg == SF_CTRL_SEL_EXTERNAL_FLASH_SF2) {
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_22,&pds_io_cfg);
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_23,&pds_io_cfg);
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_24,&pds_io_cfg);
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_25,&pds_io_cfg);
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_26,&pds_io_cfg);
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_27,&pds_io_cfg);
    } else {
        return INVALID;
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Set_Flash_Pad_Pull_None_Fast(SF_Ctrl_Pin_Select pinCfg)
{
    uint32_t tmpVal;

    if (pinCfg == SF_CTRL_SEL_EXTERNAL_FLASH_SF2) {
        /* pd config */
        tmpVal = readl(PDS_BASE + PDS_GPIO_PD_SET_OFFSET);
        /* GPIO23~28 [23]~[28] = 0 */
        tmpVal &= ~(0x1F800000);
        writel(tmpVal, PDS_BASE + PDS_GPIO_PD_SET_OFFSET);

        /* pu config */
        tmpVal = readl(PDS_BASE + PDS_GPIO_PU_SET_OFFSET);
        /* GPIO23~28 [23]~[28] = 0 */
        tmpVal &= ~(0x1F800000);
        writel(tmpVal, PDS_BASE + PDS_GPIO_PU_SET_OFFSET);

        /* ie config */
        tmpVal = readl(PDS_BASE + PDS_GPIO_IE_SET_OFFSET);
        /* GPIO23~28 [23]~[28] = 0 */
        tmpVal &= ~(0x1F800000);
        writel(tmpVal, PDS_BASE + PDS_GPIO_IE_SET_OFFSET);

        /* oe config */
        tmpVal = readl(PDS_BASE + PDS_GPIO_OE_SET_OFFSET);
        /* GPIO23~28 [23]~[28] = 0 */
        tmpVal &= ~(0x1F800000);
        writel(tmpVal, PDS_BASE + PDS_GPIO_OE_SET_OFFSET);
    } else {
        return INVALID;
    }
    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Disable_GPIO_Keep(void)
{
    uint32_t tmpVal = 0;

    /* PDS_IO keep disable */
    tmpVal = readl(PDS_BASE + PDS_CTL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_GPIO_ISO_MODE);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_GPIO_KEEP_EN);
    /* don't entry PDS */
    tmpVal = reg_clr_bit(tmpVal, PDS_START_PS);
    writel(tmpVal, PDS_BASE + PDS_CTL_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt)
{
    extern volatile uint32_t pm_pds_hbn_arm_diag[40];

    /* PDS sleep time 1~PDS_WARMUP_LATENCY_CNT <=> error */
    /* PDS sleep time >PDS_WARMUP_LATENCY_CNT <=> correct */
    if ((pdsSleepCnt) && (pdsSleepCnt <= PDS_WARMUP_LATENCY_CNT)) {
        return ERROR;
    }
    /* PDS_CTL4 config */
    writel(*(uint32_t *)cfg4, PDS_BASE + PDS_CTL4_OFFSET);
    pm_pds_hbn_arm_diag[32] = readl(PDS_BASE + PDS_CTL4_OFFSET);

    /* PDS_CTL config */
    if (cfg->pdsStart) {
        writel((*(uint32_t *)cfg & ~(1 << 0)) ,PDS_BASE + PDS_CTL_OFFSET);
        pm_pds_hbn_arm_diag[33] = readl(PDS_BASE + PDS_CTL_OFFSET);
        writel((*(uint32_t *)cfg | (1 << 0)), PDS_BASE + PDS_CTL_OFFSET);
        pm_pds_hbn_arm_diag[34] = readl(PDS_BASE + PDS_CTL_OFFSET);
    } else {
        writel(*(uint32_t *)cfg, PDS_BASE + PDS_CTL_OFFSET);
        pm_pds_hbn_arm_diag[33] = readl(PDS_BASE + PDS_CTL_OFFSET);
        pm_pds_hbn_arm_diag[34] = pm_pds_hbn_arm_diag[33];
    }
    pm_pds_hbn_arm_diag[35] = readl(PDS_BASE + PDS_CTL4_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3)
{
    /* PDS_CTL2 config */
    writel(*(uint32_t *)cfg2, PDS_BASE + PDS_CTL2_OFFSET);

    /* PDS_CTL3 config */
    writel(*(uint32_t *)cfg3, PDS_BASE + PDS_CTL3_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_RAM_Config(PDS_RAM_CFG_Type *ramCfg)
{
    if (NULL == ramCfg) {
        return ERROR;
    }

    /* PDS_RAM1 config */
    writel(*(uint32_t *)ramCfg, PDS_BASE + PDS_RAM1_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt)
{
    PDS_Force_Config((PDS_CTL2_Type *)&(defaultLvCfg->pdsCtl2), (PDS_CTL3_Type *)&(defaultLvCfg->pdsCtl3));
    PDS_Enable((PDS_CTL_Type *)&(defaultLvCfg->pdsCtl), (PDS_CTL4_Type *)&(defaultLvCfg->pdsCtl4), pdsSleepCnt);

    return SUCCESS;
}

int PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_Type intType, int enable)
{
    uint32_t offset = 0;
    uint32_t tmpVal = 0;

    if (intType > PDS_WAKEUP_SRC_WDG_TIMEOUT) {
        return ERROR;
    }

    offset = intType + PDS_CR_PDS_WAKEUP_SRC_EN_POS;

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);

    if (enable) {
        tmpVal = tmpVal | (1 << offset);
    } else {
        tmpVal = tmpVal & ~(1 << offset);
    }

    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    return SUCCESS;
}

int PDS_Wakeup_Src_En_DisableAll(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    tmpVal &= ~(0xFF << 16);
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    return SUCCESS;
}

int PDS_Get_Wakeup_Src(PDS_WAKEUP_SRC_Type intType)
{
    uint32_t offset = 0;

    if (intType > PDS_WAKEUP_SRC_WDG_TIMEOUT) {
        return RESET;
    }

    offset = intType + PDS_RO_PDS_WAKEUP_EVENT_POS;

    return (readl(PDS_BASE + PDS_INT_OFFSET) & (1 << offset)) ? SET : RESET;
}

int PDS_IntMask(PDS_INT_Type intType, int intMask)
{
    uint32_t tmpVal = 0;


    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);

    if (intMask != UNMASK) {
        tmpVal = tmpVal | (1 << (intType + PDS_CR_PDS_WAKE_INT_MASK_POS));
    } else {
        tmpVal = tmpVal & ~(1 << (intType + PDS_CR_PDS_WAKE_INT_MASK_POS));
    }

    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    return SUCCESS;
}

int PDS_Get_IntStatus(PDS_INT_Type intType)
{

    return (readl(PDS_BASE + PDS_INT_OFFSET) & (1 << intType)) ? SET : RESET;
}

int ATTR_TCM_SECTION PDS_IntClear(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_INT_CLR);
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_CR_PDS_INT_CLR);
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_INT_CLR);
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    return SUCCESS;
}

int PDS_Get_PdsRfStstus(void)
{
    return (PDS_RF_STS_Type)reg_get_bits(readl(PDS_BASE + PDS_STAT_OFFSET), PDS_RO_PDS_RF_STATE);
}

int PDS_Get_PdsStstus(void)
{
    return (PDS_STS_Type)reg_get_bits(readl(PDS_BASE + PDS_STAT_OFFSET), PDS_RO_PDS_STATE);
}

int PDS_Clear_Reset_Event(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CLR_RESET_EVENT);
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_CLR_RESET_EVENT);
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CLR_RESET_EVENT);
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    return SUCCESS;
}

int PDS_Get_Reset_Event(PDS_RST_EVENT_Type event)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    tmpVal = reg_get_bits(tmpVal, PDS_RESET_EVENT);

    return (tmpVal & (1 << event)) ? SET : RESET;
}

void ATTR_TCM_SECTION PDS_Auto_Enable(PDS_AUTO_POWER_DOWN_CFG_Type *powerCfg, PDS_AUTO_NORMAL_CFG_Type *normalCfg, int enable)
{
    uint32_t pdsCtl = 0;


    /* power config */
    pdsCtl |= (powerCfg->mbgPower << 31) |
              (powerCfg->ldo18rfPower << 30) |
              (powerCfg->sfregPower << 29) |
              (powerCfg->pllPower << 28) |
              (powerCfg->cpu0Power << 19) |
              (powerCfg->rc32mPower << 17) |
              (powerCfg->xtalPower << 14) |
              (powerCfg->allPower << 13) |
              (powerCfg->isoPower << 11) |
              (powerCfg->bzPower << 10) |
              (powerCfg->sramDisStanby << 9) |
              (powerCfg->cgPower << 8) |
              (powerCfg->cpu1Power << 7) |
              (powerCfg->usbPower << 3);
    // pdsCtl = reg_set_bits(pdsCtl, PDS_CR_PDS_LDO_VOL, normalCfg->vddcoreVol);
    pdsCtl |= (normalCfg->vddcoreVolEn << 18) |
              (normalCfg->cpu0NotNeedWFI << 21) |
              (normalCfg->cpu1NotNeedWFI << 20) |
              (normalCfg->busReset << 16) |
              (normalCfg->disIrqWakeUp << 15) |
              (normalCfg->powerOffXtalForever << 2) |
              (normalCfg->sleepForever << 1);
    writel(pdsCtl, PDS_BASE + PDS_CTL_OFFSET);

    pdsCtl = readl(PDS_BASE + PDS_CTL_OFFSET);

    if (enable) {
        pdsCtl |= (1 << 0);
    } else {
        pdsCtl &= ~(1 << 0);
    }

    writel(pdsCtl, PDS_BASE + PDS_CTL_OFFSET);
}

void ATTR_TCM_SECTION PDS_Manual_Force_Turn_Off(PDS_FORCE_Type domain)
{
    uint32_t tmpVal = 0;

    /* memory sleep */
    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal |= 1 << (domain + PDS_FORCE_MEM_STBY_OFFSET);
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    /* gate clock */
    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal |= 1 << (domain + PDS_FORCE_GATE_CLK_OFFSET);
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    /* pds reset */
    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal |= 1 << (domain + PDS_FORCE_PDS_RST_OFFSET);
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    /* isolation on */
    // tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    // tmpVal |= 1 << (domain + PDS_FORCE_ISO_EN_OFFSET);
    // writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    /* power off */
    // tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    // tmpVal |= 1 << (domain + PDS_FORCE_PWR_OFF_OFFSET);
    // writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);
}

void ATTR_TCM_SECTION PDS_Manual_Force_Turn_On(PDS_FORCE_Type domain)
{
    uint32_t tmpVal = 0;

    /* power on */
    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal &= ~(1 << (domain + PDS_FORCE_PWR_OFF_OFFSET));
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    /* isolation off */
    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal &= ~(1 << (domain + PDS_FORCE_ISO_EN_OFFSET));
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    /* pds de_reset */
    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal &= ~(1 << (domain + PDS_FORCE_PDS_RST_OFFSET));
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    /* memory active */
    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal &= ~(1 << (domain + PDS_FORCE_MEM_STBY_OFFSET));
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    /* clock on */
    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal &= ~(1 << (domain + PDS_FORCE_GATE_CLK_OFFSET));
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);
}

int PDS_Set_KYD_Matrix_Size(uint8_t col_size, uint8_t row_size)
{
    uint32_t tmpVal = 0;

    /* set col size */
    tmpVal = readl(PDS_BASE + PDS_KYD_CTL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_KYD_COL_MATRIX_SIZE, col_size);
    writel(tmpVal, PDS_BASE + PDS_KYD_CTL_OFFSET);

    /* set row size */
    tmpVal = readl(PDS_BASE + PDS_KYD_CTL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_KYD_ROW_MATRIX_SIZE, row_size);
    writel(tmpVal, PDS_BASE + PDS_KYD_CTL_OFFSET);

    return SUCCESS;
}

int PDS_Set_KYD_Col_Value(uint8_t val)
{
    uint32_t tmpVal = 0;

    /* set col output value */
    tmpVal = readl(PDS_BASE + PDS_KYD_CTL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_KYD_COL_O_VAL, val);
    writel(tmpVal, PDS_BASE + PDS_KYD_CTL_OFFSET);

    return SUCCESS;
}

int PDS_Set_KYD_Row_Pull(uint8_t en)
{
    uint32_t tmpVal = 0;

    /* disable/enable pullup or pulldown */
    tmpVal = readl(PDS_BASE + PDS_KYD_CTL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_KYD_ROW_I_PULL_EN, en);
    writel(tmpVal, PDS_BASE + PDS_KYD_CTL_OFFSET);

    return SUCCESS;
}

int PDS_Set_KYD_Wakeup_En(uint8_t en)
{
    uint32_t tmpVal = 0;

    /* disable/enable cr_pds_kyd_en  */
    tmpVal = readl(PDS_BASE + PDS_KYD_CTL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_KYD_EN, en);
    writel(tmpVal, PDS_BASE + PDS_KYD_CTL_OFFSET);

    PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_KYD_WAKEUP, en);

    return SUCCESS;
}

int PDS_Clear_PSRAM_Pad_Cfg(void)
{
    /* Clear PDS PSRAM pad settings to 0 */
    writel(0, PDS_BASE + PDS_PSRAM_PDPU_OFFSET);
    writel(0, PDS_BASE + PDS_PSRAM_IEOE_OFFSET);
    writel(0, PDS_BASE + PDS_PSRAM_O_OFFSET);

    return SUCCESS;
}
