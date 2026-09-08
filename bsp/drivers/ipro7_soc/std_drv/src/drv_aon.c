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

#include "drv_aon.h"
#include "drv_ef_cfg.h"
#include "drv_ef_ctrl.h"

#define AON_CLK_SET_DUMMY_WAIT \
    {                          \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
    }

int ATTR_CLOCK_SECTION AON_Power_On_MBG(void)
{
    uint32_t tmpVal = 0;

    /* Power up RF for PLL to work */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_PU_MBG_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    arch_delay_us(55);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION AON_Power_Off_MBG(void)
{
    uint32_t tmpVal = 0;

    /* Power OFF */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_MBG_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION AON_Power_On_XTAL(void)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_PU_XTAL_AON);
    tmpVal = reg_set_bit(tmpVal, AON_PU_XTAL_HF_RC32M_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    /* Polling for ready */
    do {
        arch_delay_us(10);
        timeOut++;
        tmpVal = readl(AON_BASE + AON_TSEN_OFFSET);
    } while (!reg_is_bit_set(tmpVal, AON_XTAL_RDY) && timeOut < 120);

    if (timeOut >= 120) {
        return TIMEOUT;
    }

    return SUCCESS;
}

int ATTR_CLOCK_SECTION AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_XTAL_CFG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AON_XTAL_HF_CAPCODE_IN_AON, capIn);
    tmpVal = reg_set_bits(tmpVal, AON_XTAL_HF_CAPCODE_OUT_AON, capOut);
    writel(tmpVal, AON_BASE + AON_XTAL_CFG2_OFFSET);

    arch_delay_us(100);

    return SUCCESS;
}

uint8_t ATTR_CLOCK_SECTION AON_Get_Xtal_CapCode(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_XTAL_CFG2_OFFSET);

    return reg_get_bits(tmpVal, AON_XTAL_HF_CAPCODE_IN_AON);
}

int ATTR_CLOCK_SECTION AON_Set_Xtal_CapCode_Extra(uint8_t extra)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_XTAL_CFG_OFFSET);
    if (extra) {
        tmpVal = reg_set_bit(tmpVal, AON_XTAL_CAPCODE_EXTRA_AON);
    } else {
        tmpVal = reg_clr_bit(tmpVal, AON_XTAL_CAPCODE_EXTRA_AON);
    }
    writel(tmpVal, AON_BASE + AON_XTAL_CFG_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION AON_Power_Off_XTAL(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_XTAL_AON);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_XTAL_HF_RC32M_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Power_On_BG(void)
{
    uint32_t tmpVal = 0;

    /* power up RF for PLL to work */
    tmpVal = readl(AON_BASE + AON_BG_SYS_TOP_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_PU_BG_SYS_AON);
    writel(tmpVal, AON_BASE + AON_BG_SYS_TOP_OFFSET);

    arch_delay_us(55);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Power_Off_BG(void)
{
    uint32_t tmpVal = 0;

    /* power up RF for PLL to work */
    tmpVal = readl(AON_BASE + AON_BG_SYS_TOP_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_BG_SYS_AON);
    writel(tmpVal, AON_BASE + AON_BG_SYS_TOP_OFFSET);

    arch_delay_us(55);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Power_On_LDO11_SOC(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_LDO11SOC_TOP_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_PU_LDO11SOC_AON);
    writel(tmpVal, AON_BASE + AON_LDO11SOC_TOP_OFFSET);

    arch_delay_us(55);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Power_Off_LDO11_SOC(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_LDO11SOC_TOP_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_LDO11SOC_AON);
    writel(tmpVal, AON_BASE + AON_LDO11SOC_TOP_OFFSET);

    arch_delay_us(55);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Power_On_SFReg(void)
{
    uint32_t tmpVal = 0;

    /* power on sfreg */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_PU_SFREG_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    arch_delay_us(10);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Power_Off_SFReg(void)
{
    uint32_t tmpVal = 0;

    /* power off sfreg */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_SFREG_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_LowPower_Enter_PDS0(void)
{
    uint32_t tmpVal = 0;

    /* power off bz */
    tmpVal = readl(AON_BASE + AON_MISC_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_SW_BZ_EN_AON);
    writel(tmpVal, AON_BASE + AON_MISC_OFFSET);

    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_SFREG_AON);
#if 0
    tmpVal = reg_clr_bit(tmpVal, AON_PU_LDO15RF_AON);
#endif
    tmpVal = reg_clr_bit(tmpVal, AON_PU_MBG_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_LowPower_Exit_PDS0(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);

    tmpVal = reg_set_bit(tmpVal, AON_PU_MBG_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    arch_delay_us(20);
#if 0
    tmpVal = reg_set_bit(tmpVal, AON_PU_LDO15RF_AON);
#endif
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    arch_delay_us(60);

    tmpVal = reg_set_bit(tmpVal, AON_PU_SFREG_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    arch_delay_us(20);

    /* power on bz */
    tmpVal = readl(AON_BASE + AON_MISC_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_SW_BZ_EN_AON);
    writel(tmpVal, AON_BASE + AON_MISC_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Set_LDO11_SOC_Sstart_Delay(uint8_t delay)
{
    uint32_t tmpVal = 0;

    /* config ldo11soc_sstart_delay_aon */
    tmpVal = readl(AON_BASE + AON_LDO11SOC_TOP_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AON_LDO11SOC_SSTART_DELAY_AON, delay);
    writel(tmpVal, AON_BASE + AON_LDO11SOC_TOP_OFFSET);

    return SUCCESS;
}

int AON_Set_DCDC14_Top_0(uint8_t voutSel, uint8_t vpfm)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_DCDC18_TOP_0_OFFSET);
    //dcdc18_vout_sel_aon, 1.425V*1.05=1.5V
    tmpVal = reg_set_bits(tmpVal, AON_DCDC_VOUT_SEL_AON, voutSel);
    //dcdc18_vpfm_aon
    tmpVal = reg_set_bits(tmpVal, AON_DCDC_VPFM_AON, vpfm);
    writel(tmpVal, AON_BASE + AON_DCDC18_TOP_0_OFFSET);

    return SUCCESS;
}

/**
 * @brief Program LDO14 output voltage select (ldo14_vout_sel_aon, 3-bit).
 */
int AON_Set_LDO14_Vout_Sel(uint8_t sel)
{
    uint32_t tmpVal;

    tmpVal = readl(AON_BASE + AON_LDO14_TOP_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AON_LDO14_VOUT_SEL_AON, sel);
    writel(tmpVal, AON_BASE + AON_LDO14_TOP_OFFSET);

    return SUCCESS;
}

/**
 * @brief Poll DCDC ready flag (dcdc_rdy_aon, bit 31 of DCDC18_TOP_0).
 *
 * @param max_polls Iteration budget; 0 means default (1,000,000).
 * @return SUCCESS when ready, TIMEOUT on budget exhausted.
 */
int AON_Wait_DCDC_Ready(uint32_t max_polls)
{
    uint32_t i;

    if (max_polls == 0) {
        max_polls = 1000000U;
    }

    for (i = 0; i < max_polls; i++) {
        if (readl(AON_BASE + AON_DCDC18_TOP_0_OFFSET) & AON_DCDC_RDY_AON_MSK) {
            return SUCCESS;
        }
    }

    return TIMEOUT;
}

int ATTR_TCM_SECTION AON_Trim_Ldo11socVoutTrim(void)
{
    Efuse_Common_Trim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Common_Trim("ldo11_trim", &trim, 1);
    if (trim.en) {
        if (trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
            tmpVal = readl(AON_BASE + AON_LDO11SOC_TOP_OFFSET);
            tmpVal = reg_set_bits(tmpVal, AON_LDO11SOC_VOUT_TRIM_AON, trim.value);
            writel(tmpVal, AON_BASE + AON_LDO11SOC_TOP_OFFSET);
            return SUCCESS;
        }
    }

    return ERROR;
}

int ATTR_TCM_SECTION AON_Trim_Ldo14VoutTrim(void)
{
    Efuse_Common_Trim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Common_Trim("ldo14_trim", &trim, 1);
    if (trim.en) {
        if (trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
            tmpVal = readl(AON_BASE + AON_LDO14_TOP_OFFSET);
            tmpVal = reg_set_bits(tmpVal, AON_LDO14_VOUT_TRIM_AON, trim.value);
            writel(tmpVal, AON_BASE + AON_LDO14_TOP_OFFSET);
            return SUCCESS;
        }
    }

    return ERROR;
}

int ATTR_TCM_SECTION AON_Trim_Dcdc14VoutTrim(void)
{
    Efuse_Common_Trim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Common_Trim("ldo14_trim", &trim, 1);
    if (trim.en) {
        if (trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
            tmpVal = readl(AON_BASE + AON_DCDC_TOP_2_OFFSET);
            tmpVal = reg_set_bits(tmpVal, AON_DCDC_VOUT_TRIM_AON, trim.value);
            writel(tmpVal, AON_BASE + AON_DCDC_TOP_2_OFFSET);
            return SUCCESS;
        }
    }

    return ERROR;
}