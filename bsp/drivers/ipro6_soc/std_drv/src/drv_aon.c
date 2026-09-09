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
    tmpVal = reg_set_bit(tmpVal, AON_PU_XTAL_BUF_AON);
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

    tmpVal = readl(AON_BASE + AON_XTAL_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AON_XTAL_CAPCODE_IN_AON, capIn);
    tmpVal = reg_set_bits(tmpVal, AON_XTAL_CAPCODE_OUT_AON, capOut);
    writel(tmpVal, AON_BASE + AON_XTAL_CFG_OFFSET);

    arch_delay_us(100);

    return SUCCESS;
}

uint8_t ATTR_CLOCK_SECTION AON_Get_Xtal_CapCode(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_XTAL_CFG_OFFSET);

    return reg_get_bits(tmpVal, AON_XTAL_CAPCODE_IN_AON);
}

int ATTR_CLOCK_SECTION AON_Power_Off_XTAL(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_XTAL_AON);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_XTAL_BUF_AON);
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

int ATTR_TCM_SECTION AON_Power_On_LDO15_RF(void)
{
    uint32_t tmpVal = 0;

    /* ldo15rf power on */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_PU_LDO15RF_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    arch_delay_us(90);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Power_Off_LDO15_RF(void)
{
    uint32_t tmpVal = 0;

    /* ldo15rf power off */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_LDO15RF_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

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

int ATTR_TCM_SECTION AON_Power_On_MicBias(void)
{
    uint32_t tmpVal = 0;

    /* power on sfreg */
    tmpVal = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_GPADC_MICBIAS_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CMD_OFFSET);

    arch_delay_us(10);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Power_Off_MicBias(void)
{
    uint32_t tmpVal = 0;

    /* power off sfreg */
    tmpVal = readl(AON_BASE + AON_GPADC_REG_CMD_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_GPADC_MICBIAS_EN);
    writel(tmpVal, AON_BASE + AON_GPADC_REG_CMD_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_LowPower_Enter_PDS0(void)
{
    uint32_t tmpVal = 0;

    /* power off sfreg */
    tmpVal = readl(AON_BASE + AON_MISC_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_SW_WB_EN_AON);
    writel(tmpVal, AON_BASE + AON_MISC_OFFSET);

    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_SFREG_AON);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_LDO15RF_AON);
    tmpVal = reg_clr_bit(tmpVal, AON_PU_MBG_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    /* gating Clock */
    tmpVal = readl(GLB_BASE + GLB_CGEN_CFG0_OFFSET);
    tmpVal = tmpVal & (~(1 << 6));
    tmpVal = tmpVal & (~(1 << 7));
    writel(tmpVal, GLB_BASE + GLB_CGEN_CFG0_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_LowPower_Exit_PDS0(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);

    tmpVal = reg_set_bit(tmpVal, AON_PU_MBG_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    arch_delay_us(20);

    tmpVal = reg_set_bit(tmpVal, AON_PU_LDO15RF_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    arch_delay_us(60);

    tmpVal = reg_set_bit(tmpVal, AON_PU_SFREG_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    arch_delay_us(20);

    /* power on wb */
    tmpVal = readl(AON_BASE + AON_MISC_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_SW_WB_EN_AON);
    writel(tmpVal, AON_BASE + AON_MISC_OFFSET);

    /* ungating Clock */
    tmpVal = readl(GLB_BASE + GLB_CGEN_CFG0_OFFSET);
    tmpVal = tmpVal | ((1 << 6));
    tmpVal = tmpVal | ((1 << 7));
    writel(tmpVal, GLB_BASE + GLB_CGEN_CFG0_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Trim_DcdcDis(void)
{
    Efuse_Ana_DcdcDis_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_DcdcDis_Trim(&trim);
    if (trim.dcdcDisAonEn) {
        if (trim.dcdcDisAonParity == EF_Ctrl_Get_Trim_Parity(trim.dcdcDisAon, 1)) {
            tmpVal = readl(AON_BASE + AON_DCDC_TOP_0_OFFSET);
            tmpVal = reg_set_bits(tmpVal, AON_DCDC_DIS_AON, trim.dcdcDisAon);
            writel(tmpVal, AON_BASE + AON_DCDC_TOP_0_OFFSET);
            return SUCCESS;
        }
    }

    return ERROR;
}

int ATTR_TCM_SECTION AON_Trim_DcdcVoutSel(void)
{
    Efuse_Ana_DcdcVoutSel_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_DcdcVoutSel_Trim(&trim);
    if (trim.dcdcVoutSelAonEn) {
        if (trim.dcdcVoutSelAonParity == EF_Ctrl_Get_Trim_Parity(trim.dcdcVoutSelAon, 5)) {
            tmpVal = readl(AON_BASE + AON_DCDC_TOP_0_OFFSET);
            tmpVal = reg_set_bits(tmpVal, AON_DCDC_VOUT_SEL_AON, trim.dcdcVoutSelAon);
            writel(tmpVal, AON_BASE + AON_DCDC_TOP_0_OFFSET);
            return SUCCESS;
        }
    }

    return ERROR;
}

int ATTR_TCM_SECTION AON_Trim_DcdcVoutTrim(void)
{
    Efuse_Ana_DcdcVoutTrim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_DcdcVoutTrim_Trim(&trim);
    if (trim.dcdcVoutTrimAonEn) {
        if (trim.dcdcVoutTrimAonParity == EF_Ctrl_Get_Trim_Parity(trim.dcdcVoutTrimAon, 4)) {
            tmpVal = readl(AON_BASE + AON_DCDC_TOP_2_OFFSET);
            tmpVal = reg_set_bits(tmpVal, AON_DCDC_VOUT_TRIM_AON, trim.dcdcVoutTrimAon);
            writel(tmpVal, AON_BASE + AON_DCDC_TOP_2_OFFSET);
            return SUCCESS;
        }
    }

    return ERROR;
}

int ATTR_TCM_SECTION AON_Trim_Ldo11socVoutTrim(void)
{
    Efuse_Ana_Ldo11socVoutTrim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Ldo11socVoutTrim_Trim(&trim);
    if (trim.ldo11socVoutTrimAonEn) {
        if (trim.ldo11socVoutTrimAonTrim == EF_Ctrl_Get_Trim_Parity(trim.ldo11socVoutTrimAon, 4)) {
            tmpVal = readl(AON_BASE + AON_LDO11SOC_AND_DCTEST_OFFSET);
            tmpVal = reg_set_bits(tmpVal, AON_LDO11SOC_VOUT_TRIM_AON, trim.ldo11socVoutTrimAon);
            writel(tmpVal, AON_BASE + AON_LDO11SOC_AND_DCTEST_OFFSET);
            return SUCCESS;
        }
    }

    return ERROR;
}

int ATTR_TCM_SECTION AON_Trim_Usb20RcalCode(void)
{
    Efuse_Ana_Usb20RcalCode_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Usb20RcalCode_Trim(&trim);
    if (trim.usb20RcalCodeAonEn) {
        if (trim.usb20RcalCodeAonParity == EF_Ctrl_Get_Trim_Parity(trim.usb20RcalCodeAon, 6)) {
            tmpVal = readl(AON_BASE + AON_PSW_MISC_OFFSET);
            tmpVal = reg_set_bits(tmpVal, AON_USB20_RCAL_CODE_AON, trim.usb20RcalCodeAon);
            writel(tmpVal, AON_BASE + AON_PSW_MISC_OFFSET);
            return SUCCESS;
        }
    }

    return ERROR;
}

int ATTR_TCM_SECTION AON_Output_Float_LDO15_RF(void)
{
    uint32_t tmpVal=0;

    /* ldo15rf pulldown select by aon */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_LDO15RF_PULLDOWN_SEL_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    /* ldo15rf float output */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_LDO15RF_PULLDOWN_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Output_Pulldown_LDO15_RF(void)
{
    uint32_t tmpVal=0;

    /* ldo15rf pulldown select by aon */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_LDO15RF_PULLDOWN_SEL_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    /* ldo15rf pulldown output to ground */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_LDO15RF_PULLDOWN_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Output_LDO15_RF_Ctrl_by_Pd(void)
{
    uint32_t tmpVal=0;

    /* output ldo15rf pulldown select by pd signal */
    tmpVal = readl(AON_BASE + AON_RF_TOP_AON_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_LDO15RF_PULLDOWN_SEL_AON);
    writel(tmpVal, AON_BASE + AON_RF_TOP_AON_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Output_Pulldown_DCDC18(void)
{
    uint32_t tmpVal=0;

    tmpVal = readl(AON_BASE + AON_DCDC_TOP_1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, AON_DCDC_PULLDOWN_AON);
    writel(tmpVal, AON_BASE + AON_DCDC_TOP_1_OFFSET);
    return SUCCESS;
}

int ATTR_TCM_SECTION AON_Output_Float_DCDC18(void)
{
    uint32_t tmpVal=0;

    tmpVal = readl(AON_BASE + AON_DCDC_TOP_1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_DCDC_PULLDOWN_AON);
    writel(tmpVal, AON_BASE + AON_DCDC_TOP_1_OFFSET);

    return SUCCESS;
}

