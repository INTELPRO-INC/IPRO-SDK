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


#include "drv_glb.h"
#include "drv_psram.h"
#include "psram_reg.h"

#define PSRAM_X8_CTRL_WAIT_TIMEOUT 1000

void PSram_Ctrl_Init(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_Cfg_Type *psramCtrlCfg)
{
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    uint32_t tmpVal = 0;

    //PSRAM initial sequence
    arch_delay_us(150);
    /* set psram dqs delay 0xfff0 */
    tmpVal = readl(psram_base + PSRAM_ROUGH_DELAY_CTRL5_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_ROUGH_SEL_I_DQS0, psramCtrlCfg->dqs_delay);
    writel(tmpVal, psram_base + PSRAM_ROUGH_DELAY_CTRL5_OFFSET);

    /* Set vendor and mode for psram controller */
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_PCK_S_DIV, 0x1);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_VENDOR_SEL, psramCtrlCfg->vendor);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_X16_MODE, psramCtrlCfg->ioMode);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    tmpVal = readl(psram_base + PSRAM_MANUAL_CONTROL2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_ADDR_MASK, psramCtrlCfg->size);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_DQS_REL_VAL, 0x1f);
    writel(tmpVal, psram_base + PSRAM_MANUAL_CONTROL2_OFFSET);

    if (psramCtrlCfg->vendor == PSRAM_CTRL_VENDOR_WINBOND && psramCtrlCfg->ioMode == PSRAM_CTRL_X16_MODE && psramCtrlCfg->size == PSRAM_SIZE_16MB) {
        //If using W957D6NKR, must set reg_wb_hyper3 bit to 1
        tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
        tmpVal = reg_set_bit(tmpVal, PSRAM_REG_WB_HYPER3);
        writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);
    }
}

static void PSram_Ctrl_Request(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    uint32_t time_out = 0;

    //start configure request
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_CONFIG_REQ, 1);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    //Waiting for the authorization
    do {
        tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
        if (time_out++ > PSRAM_X8_CTRL_WAIT_TIMEOUT) {
            break;
        }
    } while (!reg_is_bit_set(tmpVal, PSRAM_REG_CONFIG_GNT));
}

static void PSram_Ctrl_Release(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);

    //clear start configure request
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_CONFIG_REQ, 0);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);
}

int PSram_Ctrl_Winbond_Read_Reg(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_Winbond_Cfg_Reg_Type reg_addr, uint16_t *regVal)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    uint32_t time_out = 0;

    PSram_Ctrl_Request(PSRAM_ID);

    //configure pSRAM register,select reg_addr CR0 or CR1
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_REG_SEL, reg_addr);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    //start psram configure
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_CONFIG_R_PUSLE, 1);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    //waiting confiure complete
    do {
        tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
        if (time_out++ > PSRAM_X8_CTRL_WAIT_TIMEOUT) {
            return TIMEOUT;
        }
    } while (!reg_is_bit_set(tmpVal, PSRAM_STS_CONFIG_R_DONE));

    //read reg data form sts_config_read
    tmpVal = readl(psram_base + PSRAM_MANUAL_CONTROL_OFFSET);
    *regVal = (uint16_t)(tmpVal >> 16);

    PSram_Ctrl_Release(PSRAM_ID);

    return SUCCESS;
}

int PSram_Ctrl_Winbond_Eneter_Hybrid_Sleep(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_Winbond_Cfg_Reg_Type reg_addr, uint8_t psar)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    uint32_t time_out = 0;
    PSRAM_Ctrl_Size_Type psramDensity;

    PSram_Ctrl_Request(PSRAM_ID);
    tmpVal = readl(psram_base + PSRAM_WINBOND_PSRAM_CONFIGURE_OFFSET);
    psramDensity = reg_get_bits(readl(psram_base + PSRAM_MANUAL_CONTROL2_OFFSET), PSRAM_REG_ADDR_MASK);

    if (psramDensity == PSRAM_SIZE_4MB) {
        tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_PASR, psar);
    } else {
        tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_PASR, psar << 2);
    }
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_HYBRID_SLP, 1);
    writel(tmpVal, psram_base + PSRAM_WINBOND_PSRAM_CONFIGURE_OFFSET);

    //configure pSRAM register,select reg_addr CR0 or CR1
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_REG_SEL, reg_addr);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    //start psram configure
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_CONFIG_W_PUSLE, 1);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    //waiting confiure complete
    do {
        tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
        if (time_out++ > PSRAM_X8_CTRL_WAIT_TIMEOUT) {
            return TIMEOUT;
        }
    } while (!reg_is_bit_set(tmpVal, PSRAM_STS_CONFIG_W_DONE));

    PSram_Ctrl_Release(PSRAM_ID);

    return SUCCESS;
}

int PSram_Ctrl_Winbond_Eneter_Deep_Sleep(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    uint32_t time_out = 0;

    PSram_Ctrl_Request(PSRAM_ID);

    /* CR0 bit: DPD_DIS=0 tells Winbond to enter Deep Power Down. */
    tmpVal = readl(psram_base + PSRAM_WINBOND_PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_DPD_DIS, 0);
    writel(tmpVal, psram_base + PSRAM_WINBOND_PSRAM_CONFIGURE_OFFSET);

    /* Select CR0 for the write pulse. */
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_REG_SEL, PSRAM_WINBOND_REG_CR0);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    /* Issue write pulse. */
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_CONFIG_W_PUSLE, 1);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    do {
        tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
        if (time_out++ > PSRAM_X8_CTRL_WAIT_TIMEOUT) {
            return TIMEOUT;
        }
    } while (!reg_is_bit_set(tmpVal, PSRAM_STS_CONFIG_W_DONE));

    PSram_Ctrl_Release(PSRAM_ID);

    return SUCCESS;
}

int PSram_Ctrl_Winbond_Exit_Hybrid_Sleep(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);

    PSram_Ctrl_Request(PSRAM_ID);

    tmpVal = readl(psram_base + PSRAM_MANUAL_CONTROL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_FORCE_CEB_LOW, 1);
    writel(tmpVal, psram_base + PSRAM_MANUAL_CONTROL_OFFSET);
    arch_delay_us(1);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_FORCE_CEB_LOW, 0);
    writel(tmpVal, psram_base + PSRAM_MANUAL_CONTROL_OFFSET);

    PSram_Ctrl_Release(PSRAM_ID);

    return SUCCESS;
}

int PSram_Ctrl_Winbond_Write_Reg(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_Winbond_Cfg_Reg_Type reg_addr, PSRAM_Winbond_Cfg_Type *reg_cfg)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    PSRAM_Ctrl_Size_Type psramDensity;
    uint32_t time_out = 0;

    PSram_Ctrl_Request(PSRAM_ID);

    //configure Winbond register
    tmpVal = readl(psram_base + PSRAM_WINBOND_PSRAM_CONFIGURE_OFFSET);

    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_LATENCY, reg_cfg->latency);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_DRIVE_ST, reg_cfg->driveStrength);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_HYBRID_EN, reg_cfg->brustType);

    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_BURST_LENGTH, reg_cfg->brustLen);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_FIX_LATENCY, reg_cfg->fixedLatency);

    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_DPD_DIS, reg_cfg->disDeepPowerDownMode);

    psramDensity = reg_get_bits(readl(psram_base + PSRAM_MANUAL_CONTROL2_OFFSET), PSRAM_REG_ADDR_MASK);

    if (psramDensity == PSRAM_SIZE_4MB) {
        tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_PASR, reg_cfg->PASR);
    } else {
        tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_PASR, reg_cfg->PASR << 2);
    }

    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_HYBRID_SLP, reg_cfg->hybridSleepMode);

    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_IPD, reg_cfg->inputPowerDownMode);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_MCLK_TYPE, reg_cfg->clockType);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_LINEAR_DIS, reg_cfg->linear_dis);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_SW_RST, reg_cfg->rst);

    writel(tmpVal, psram_base + PSRAM_WINBOND_PSRAM_CONFIGURE_OFFSET);

    //configure pSRAM register,select reg_addr CR0 or CR1
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_REG_SEL, reg_addr);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    //start psram configure
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_CONFIG_W_PUSLE, 1);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    //waiting confiure complete
    do {
        tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
        if (time_out++ > PSRAM_X8_CTRL_WAIT_TIMEOUT) {
            return TIMEOUT;
        }
    } while (!reg_is_bit_set(tmpVal, PSRAM_STS_CONFIG_W_DONE));

    PSram_Ctrl_Release(PSRAM_ID);

    return SUCCESS;
}

int PSram_Ctrl_ApMem_Read_Reg(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_ApMem_Cfg_Reg_Type reg_addr, uint16_t *regVal)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    uint32_t time_out = 0;

    PSram_Ctrl_Request(PSRAM_ID);

    //configure pSRAM register,select reg_addr CR0 or CR1
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_REG_SEL, reg_addr);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    //start psram configure
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_CONFIG_R_PUSLE, 1);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    //waiting confiure complete
    do {
        tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
        if (time_out++ > PSRAM_X8_CTRL_WAIT_TIMEOUT) {
            return TIMEOUT;
        }
    } while (!reg_is_bit_set(tmpVal, PSRAM_STS_CONFIG_R_DONE));

    //read reg data form sts_config_read
    tmpVal = readl(psram_base + PSRAM_MANUAL_CONTROL_OFFSET);
    *regVal = (uint16_t)(tmpVal >> 16);

    PSram_Ctrl_Release(PSRAM_ID);

    return SUCCESS;
}

int PSram_Ctrl_ApMem_Write_Reg(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_ApMem_Cfg_Reg_Type reg_addr, PSRAM_APMemory_Cfg_Type *reg_cfg)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    uint32_t time_out = 0;

    PSram_Ctrl_Request(PSRAM_ID);

    //configure Winbond register
    tmpVal = readl(psram_base + PSRAM_APMEMORY_PSRAM_CONFIGURE_OFFSET);

    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_AP_R_LATENCY_TYPE, reg_cfg->fixedLatency);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_AP_R_LATENCY_CODE, reg_cfg->readLatency);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_AP_RF, reg_cfg->refreshFreq);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_AP_DRIVE_ST, reg_cfg->driveStrength);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_AP_W_LATENCY_CODE, reg_cfg->writeLatency);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_AP_PASR, reg_cfg->PASR);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_AP_SLEEP, reg_cfg->halfSleepModeEnable);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_AP_DPD, reg_cfg->deepPowerDownModeEnable);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_AP_RBX, reg_cfg->crossBoundaryEnable);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_AP_BURST_TYPE, reg_cfg->brustType);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_AP_BURST_LENGTH, reg_cfg->brustLen);

    writel(tmpVal, psram_base + PSRAM_APMEMORY_PSRAM_CONFIGURE_OFFSET);

    //configure pSRAM register,select reg_addr CR0 or CR1
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_WB_REG_SEL, reg_addr);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    //start psram configure
    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_CONFIG_W_PUSLE, 1);
    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);

    //waiting confiure complete
    do {
        tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);
        if (time_out++ > PSRAM_X8_CTRL_WAIT_TIMEOUT) {
            return TIMEOUT;
        }
    } while (!reg_is_bit_set(tmpVal, PSRAM_STS_CONFIG_W_DONE));

    PSram_Ctrl_Release(PSRAM_ID);

    return SUCCESS;
}

void PSram_Ctrl_ApMem_Reset(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);

    tmpVal = readl(psram_base + PSRAM_APMEMORY_PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PSRAM_REG_GLB_RESET_PULSE);
    writel(tmpVal, psram_base + PSRAM_APMEMORY_PSRAM_CONFIGURE_OFFSET);
}

void PSram_Ctrl_Winbond_Reset(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);

    tmpVal = readl(psram_base + PSRAM_WINBOND_PSRAM_CONFIGURE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PSRAM_REG_WB_SW_RST);
    writel(tmpVal, psram_base + PSRAM_WINBOND_PSRAM_CONFIGURE_OFFSET);
}

void PSram_Ctrl_CK_Sel(PSRAM_ID_Type PSRAM_ID, PSRAM_Clock_Type clkSel)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);

    tmpVal = readl(psram_base + PSRAM_CONFIGURE_OFFSET);

    if (clkSel == PSRAM_CLOCK_DIFF) {
        tmpVal = reg_set_bit(tmpVal, PSRAM_REG_CLKN_FREE);
    } else {
        tmpVal = reg_clr_bit(tmpVal, PSRAM_REG_CLKN_FREE);
    }

    writel(tmpVal, psram_base + PSRAM_CONFIGURE_OFFSET);
}

uint8_t PSram_Ctrl_Get_Timeout_Flag(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);

    tmpVal = readl(psram_base + PSRAM_TIMEOUT_REG_OFFSET);

    return reg_is_bit_set(tmpVal, PSRAM_STS_TIMEOUT);
}

void PSram_Ctrl_Clear_Timout_Flag(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);

    tmpVal = readl(psram_base + PSRAM_TIMEOUT_REG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PSRAM_REG_TIMEOUT_CLR);
    writel(tmpVal, psram_base + PSRAM_TIMEOUT_REG_OFFSET);

    tmpVal = readl(psram_base + PSRAM_TIMEOUT_REG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PSRAM_REG_TIMEOUT_CLR);
    writel(tmpVal, psram_base + PSRAM_TIMEOUT_REG_OFFSET);
}

void PSram_Ctrl_Debug_Timout(PSRAM_ID_Type PSRAM_ID, uint8_t enable, uint32_t timeoutThr)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);

    tmpVal = readl(psram_base + PSRAM_TIMEOUT_REG_OFFSET);

    if (enable) {
        tmpVal = reg_set_bit(tmpVal, PSRAM_REG_TIMEOUT_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, PSRAM_REG_TIMEOUT_EN);
    }

    tmpVal = reg_set_bits(tmpVal, PSRAM_REG_TIMEOUT_CNT, timeoutThr);

    writel(tmpVal, psram_base + PSRAM_TIMEOUT_REG_OFFSET);
}

