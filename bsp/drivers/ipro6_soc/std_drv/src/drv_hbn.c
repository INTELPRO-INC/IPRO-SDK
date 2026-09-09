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



#include "drv_hbn.h"
#include "drv_acomp.h"
#include "drv_glb.h"
#include "drv_xip_sflash.h"

#define HBN_CLK_SET_DUMMY_WAIT \
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

#ifndef IPRO_USE_HAL_DRIVER
static intCallback_Type *hbnInt0CbfArra[HBN_OUT0_INT_MAX] = { NULL };
static intCallback_Type *hbnInt1CbfArra[HBN_OUT1_INT_MAX] = { NULL };
#endif

void ATTR_TCM_SECTION HBN_Mode_Enter(HBN_APP_CFG_Type *cfg)
{
#ifndef BOOTROM
    uint32_t valLow = 0, valHigh = 0;
    uint64_t val;

    if (cfg->useXtal32k) {
        HBN_32K_Sel(HBN_32K_XTAL);
    } else {
        HBN_32K_Sel(HBN_32K_RC);
        HBN_Power_Off_Xtal_32K();
    }

    /* always disable HBN pin pull up/down to reduce PDS/HBN current, 0x4000F014[16]=0 */
    HBN_Hw_Pu_Pd_Cfg(DISABLE);

    HBN_Pin_WakeUp_Mask(~(cfg->gpioWakeupSrc));
    if (cfg->gpioWakeupSrc != 0) {
        HBN_GPIO_INT_Enable(cfg->gpioTrigType);
    }

    /* HBN RTC config and enable */
    HBN_Clear_RTC_Counter();
    if (cfg->sleepTime != 0) {
        HBN_Get_RTC_Timer_Val(&valLow, &valHigh);
        val = valLow + ((uint64_t)valHigh << 32);
        val += cfg->sleepTime;
        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, val & 0xffffffff, val >> 32, HBN_RTC_COMP_BIT0_39);
        HBN_Enable_RTC_Counter();
    }

    HBN_Power_Down_Flash(cfg->flashCfg);

    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_RC32M);

    HBN_Enable(cfg->gpioWakeupSrc, cfg->ldoLevel, cfg->hbnLevel, cfg->dcdcPuSeq);
#endif
}

void ATTR_TCM_SECTION HBN_Power_Down_Flash(SPI_Flash_Cfg_Type *flashCfg)
{
    SPI_Flash_Cfg_Type bhFlashCfg;

    if (flashCfg == NULL) {
        L1C_DCache_Invalid_By_Addr(IPRO6_FLASH_XIP_BASE + 8 + 4, sizeof(SPI_Flash_Cfg_Type));
        XIP_SFlash_Read_Via_Cache_Need_Lock(IPRO6_FLASH_XIP_BASE + 8 + 4, (uint8_t *)(&bhFlashCfg), sizeof(SPI_Flash_Cfg_Type));
        L1C_DCache_Invalid_By_Addr(IPRO6_FLASH_XIP_BASE + 8 + 4, sizeof(SPI_Flash_Cfg_Type));

        SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
        SFlash_Reset_Continue_Read(&bhFlashCfg);
    } else {
        SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
        SFlash_Reset_Continue_Read(flashCfg);
    }

    SFlash_Powerdown();
}

void ATTR_TCM_SECTION HBN_Enable(uint32_t aGPIOIeCfg, uint8_t ldoLevel, HBN_LEVEL_Type hbnLevel, uint8_t dcdcPuSeq)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    /* Setting from guide */
    /* HBN_SRAM Retion */
    tmpVal = readl(HBN_BASE + HBN_SRAM_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_RETRAM_RET);
    tmpVal = reg_clr_bit(tmpVal, HBN_RETRAM_SLP);
    writel(tmpVal, HBN_BASE + HBN_SRAM_OFFSET);

    /* AON GPIO IE */
    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_REG_EN_HW_PU_PD);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    /* HBN mode LDO level */
    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_LDO11_AON_VOUT_SEL, ldoLevel);
    tmpVal = reg_set_bits(tmpVal, HBN_LDO11_RT_VOUT_SEL, ldoLevel);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    /* power on dcdc18 sequence  */
    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    if (dcdcPuSeq) {
        tmpVal = reg_set_bit(tmpVal, HBN_PU_DCDC18_AON);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_PU_DCDC18_AON);
    }
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    /* Select RC32M */
    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_ROOT_CLK_SEL, 0);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    /* Set HBN flag */
    writel(HBN_STATUS_ENTER_FLAG, HBN_BASE + HBN_RSV0_OFFSET);

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Set HBN level, (HBN_PWRDN_HBN_RAM not use) */
    switch (hbnLevel) {
        case HBN_LEVEL_0:
            tmpVal = reg_clr_bit(tmpVal, HBN_PWRDN_HBN_CORE);
            break;

        case HBN_LEVEL_1:
            tmpVal = reg_set_bit(tmpVal, HBN_PWRDN_HBN_CORE);
            break;

        default:
            break;
    }
    /* Set power on option:0 for por reset twice for robust 1 for reset only once*/
    tmpVal = reg_clr_bit(tmpVal, HBN_PWR_ON_OPTION);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    /* Enable HBN mode */
    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_MODE);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    while (1) {
        arch_delay_ms(1000);
    }
#endif
}

int ATTR_TCM_SECTION HBN_Reset(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Reset HBN mode */
    tmpVal = reg_clr_bit(tmpVal, HBN_SW_RST);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    tmpVal = reg_set_bit(tmpVal, HBN_SW_RST);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, HBN_SW_RST);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Set_GPADC_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_GPADC_CS, clkSel);
    writel(tmpVal, HBN_BASE + HBN_PIR_CFG_OFFSET);

    return SUCCESS;
}

int HBN_PIR_Enable(void)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_PIR_EN);
    writel(tmpVal, HBN_BASE + HBN_PIR_CFG_OFFSET);
#endif
    return SUCCESS;
}

int HBN_PIR_Disable(void)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_PIR_EN);
    writel(tmpVal, HBN_BASE + HBN_PIR_CFG_OFFSET);
#endif
    return SUCCESS;
}

int HBN_PIR_INT_Config(HBN_PIR_INT_CFG_Type *pirIntCfg)
{
#ifndef BOOTROM
    uint32_t tmpVal;
    uint32_t bit4 = 0;
    uint32_t bit5 = 0;
    uint32_t bitVal = 0;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);

    /* low trigger interrupt */
    if (pirIntCfg->lowIntEn == ENABLE) {
        bit5 = 0;
    } else {
        bit5 = 1;
    }

    /* high trigger interrupt */
    if (pirIntCfg->highIntEn == ENABLE) {
        bit4 = 0;
    } else {
        bit4 = 1;
    }

    bitVal = bit4 | (bit5 << 1);
    tmpVal = reg_set_bits(tmpVal, HBN_PIR_DIS, bitVal);
    writel(tmpVal, HBN_BASE + HBN_PIR_CFG_OFFSET);
#endif
    return SUCCESS;
}

int HBN_PIR_LPF_Sel(HBN_PIR_LPF_Type lpf)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIR_LPF_SEL, lpf);
    writel(tmpVal, HBN_BASE + HBN_PIR_CFG_OFFSET);
#endif
    return SUCCESS;
}

int HBN_PIR_HPF_Sel(HBN_PIR_HPF_Type hpf)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIR_HPF_SEL, hpf);
    writel(tmpVal, HBN_BASE + HBN_PIR_CFG_OFFSET);
#endif
    return SUCCESS;
}

int HBN_Set_PIR_Threshold(uint16_t threshold)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_VTH_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIR_VTH, threshold);
    writel(tmpVal, HBN_BASE + HBN_PIR_VTH_OFFSET);
#endif
    return SUCCESS;
}

uint16_t HBN_Get_PIR_Threshold(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_VTH_OFFSET);

    return reg_get_bits(tmpVal, HBN_PIR_VTH);
}

int HBN_Set_PIR_Interval(uint16_t interval)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_INTERVAL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIR_INTERVAL, interval);
    writel(tmpVal, HBN_BASE + HBN_PIR_INTERVAL_OFFSET);
#endif
    return SUCCESS;
}

uint16_t HBN_Get_PIR_Interval(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_INTERVAL_OFFSET);

    return reg_get_bits(tmpVal, HBN_PIR_INTERVAL);
}

int HBN_Get_BOD_OUT_State(void)
{
    return reg_get_bits(readl(HBN_BASE + HBN_BOR_CFG_OFFSET), HBN_R_BOD_OUT) ? SET : RESET;
}

int HBN_Set_BOD_Config(uint8_t enable, HBN_BOD_THRES_Type threshold, HBN_BOD_MODE_Type mode)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_BOR_CFG_OFFSET);
    if (enable) {
        tmpVal = reg_set_bits(tmpVal, HBN_PU_BOD, 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, HBN_PU_BOD, 0);
    }
    tmpVal = reg_set_bits(tmpVal, HBN_BOD_VTH, threshold);
    tmpVal = reg_set_bits(tmpVal, HBN_BOD_SEL, mode);
    writel(tmpVal, HBN_BASE + HBN_BOR_CFG_OFFSET);
#endif
    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_Ldo11_Aon_Vout(uint8_t ldoLevel)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_SW_LDO11_AON_VOUT_SEL, ldoLevel);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_Ldo11_Rt_Vout(uint8_t ldoLevel)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_SW_LDO11_RT_VOUT_SEL, ldoLevel);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_Ldo11_Soc_Vout(uint8_t ldoLevel)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_SW_LDO11SOC_VOUT_SEL_AON, ldoLevel);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_Ldo11_All_Vout(uint8_t ldoLevel)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_SW_LDO11_AON_VOUT_SEL, ldoLevel);
    tmpVal = reg_set_bits(tmpVal, HBN_SW_LDO11_RT_VOUT_SEL, ldoLevel);
    tmpVal = reg_set_bits(tmpVal, HBN_SW_LDO11SOC_VOUT_SEL_AON, ldoLevel);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_32K_Sel(uint8_t clkType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_F32K_SEL, clkType);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int HBN_Set_UART_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    if ((HBN_UART_CLK_MCU_BCLK == clkSel) || (HBN_UART_CLK_MUXPLL_160M == clkSel)) {
        tmpVal = reg_set_bits(tmpVal, HBN_UART_CLK_SEL2, 0);
        tmpVal = reg_set_bits(tmpVal, HBN_UART_CLK_SEL, clkSel);
    } else {
        /* mcu_bclk as default */
        tmpVal = reg_set_bits(tmpVal, HBN_UART_CLK_SEL2, 1);
        tmpVal = reg_set_bits(tmpVal, HBN_UART_CLK_SEL, 0);
    }

    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

uint8_t ATTR_CLOCK_SECTION HBN_Get_MCU_XCLK_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_get_bits(tmpVal, HBN_ROOT_CLK_SEL);

    return (uint8_t)(tmpVal & 0x1);
}

int ATTR_CLOCK_SECTION HBN_Set_MCU_XCLK_Sel(uint8_t xclk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, HBN_ROOT_CLK_SEL);
    switch (xclk) {
        case HBN_MCU_XCLK_RC32M:
            tmpVal2 &= (~(1 << 0));
            break;
        case HBN_MCU_XCLK_XTAL:
            tmpVal2 |= (1 << 0);
            break;
        default:
            break;
    }
    tmpVal = reg_set_bits(tmpVal, HBN_ROOT_CLK_SEL, tmpVal2);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    HBN_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

uint8_t ATTR_CLOCK_SECTION HBN_Get_MCU_Root_CLK_Sel(void)
{
    uint32_t tmpVal = 0;
    uint32_t rootClkSel = 0;

    /* root_clock_select = hbn_root_clk_sel[1] */
    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    rootClkSel = reg_get_bits(tmpVal, HBN_ROOT_CLK_SEL) >> 1;
    if (0 == rootClkSel) {
        return HBN_MCU_ROOT_CLK_XCLK;
    } else {
        return HBN_MCU_ROOT_CLK_PLL;
    }
}

int ATTR_CLOCK_SECTION HBN_Set_MCU_Root_CLK_Sel(uint8_t rootClk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    /* root_clock_select = hbn_root_clk_sel[1] */
    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, HBN_ROOT_CLK_SEL);
    switch (rootClk) {
        case HBN_MCU_ROOT_CLK_XCLK:
            tmpVal2 &= 0x1; //[1]=0
            break;
        case HBN_MCU_ROOT_CLK_PLL:
            tmpVal2 |= 0x2; //[1]=1
            break;
        default:
            break;
    }
    tmpVal = reg_set_bits(tmpVal, HBN_ROOT_CLK_SEL, tmpVal2);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    HBN_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_HRAM_slp(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_SRAM_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_RETRAM_SLP);
    tmpVal = reg_clr_bit(tmpVal, HBN_RETRAM_RET);
    writel(tmpVal, HBN_BASE + HBN_SRAM_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_HRAM_Ret(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_SRAM_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_RETRAM_SLP);
    tmpVal = reg_set_bit(tmpVal, HBN_RETRAM_RET);
    writel(tmpVal, HBN_BASE + HBN_SRAM_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Power_On_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_XTAL32K_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, HBN_XTAL32K_HIZ_EN);
    tmpVal = reg_set_bits(tmpVal, HBN_XTAL32K_INV_STRE, 3);
    tmpVal = reg_set_bit(tmpVal, HBN_PU_XTAL32K);
    tmpVal = reg_set_bit(tmpVal, HBN_PU_XTAL32K_BUF);
    writel(tmpVal, HBN_BASE + HBN_XTAL32K_OFFSET);

    /* Delay >1s */
    arch_delay_us(1100);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Power_Off_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_XTAL32K_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_XTAL32K_HIZ_EN);
    tmpVal = reg_clr_bit(tmpVal, HBN_PU_XTAL32K);
    tmpVal = reg_clr_bit(tmpVal, HBN_PU_XTAL32K_BUF);
    writel(tmpVal, HBN_BASE + HBN_XTAL32K_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Keep_On_RC32K(void)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_PU_RC32K);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
#endif
    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Power_Off_RC32K(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_PU_RC32K);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION HBN_Trim_Ldo33VoutTrim(void)
{
    Efuse_Ana_Ldo33VoutTrim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Ldo33VoutTrim_Trim(&trim);
    if (trim.ldo33VoutTrimAonEn) {
        if (trim.ldo33VoutTrimAonParity == EF_Ctrl_Get_Trim_Parity(trim.ldo33VoutTrimAon, 4)) {
            tmpVal = readl(HBN_BASE + HBN_VBAT_LDO_OFFSET);
            tmpVal = reg_set_bits(tmpVal, HBN_LDO33_VOUT_TRIM_AON, trim.ldo33VoutTrimAon);
            writel(tmpVal, HBN_BASE + HBN_VBAT_LDO_OFFSET);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

int ATTR_CLOCK_SECTION HBN_Trim_RC32K(void)
{
    Efuse_Ana_RC32K_Trim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_RC32K_Trim(&trim);
    if (trim.rc32kCodeFrExtEn) {
        if (trim.rc32kCodeFrExtParity == EF_Ctrl_Get_Trim_Parity(trim.rc32kCodeFrExt, 10)) {
            tmpVal = readl(HBN_BASE + HBN_RC32K_CTRL0_OFFSET);
            tmpVal = reg_set_bits(tmpVal, HBN_RC32K_CODE_FR_EXT, trim.rc32kCodeFrExt);
            tmpVal = reg_set_bit(tmpVal, HBN_RC32K_EXT_CODE_EN);
            writel(tmpVal, HBN_BASE + HBN_RC32K_CTRL0_OFFSET);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

uint32_t HBN_Get_Status_Flag(void)
{
    return readl(HBN_BASE + HBN_RSV0_OFFSET);
}

int HBN_Set_Status_Flag(uint32_t flag)
{
    writel(flag, HBN_BASE + HBN_RSV0_OFFSET);

    return SUCCESS;
}

uint32_t HBN_Get_Wakeup_Addr(void)
{
    return readl(HBN_BASE + HBN_RSV1_OFFSET);
}

int HBN_Set_Wakeup_Addr(uint32_t addr)
{
    writel(addr, HBN_BASE + HBN_RSV1_OFFSET);

    return SUCCESS;
}

uint8_t ATTR_TCM_SECTION HBN_Get_Core_Unhalt_Config(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV2_OFFSET);
    if (HBN_RELEASE_CORE_FLAG == reg_get_bits(tmpVal, HBN_RELEASE_CORE)) {
        return reg_get_bits(tmpVal, HBN_CORE_UNHALT);
    }

    return 0x00;
}

int ATTR_TCM_SECTION HBN_Set_Core_Reboot_Config(uint8_t core, uint8_t hcfg)
{
    uint32_t tmpVal = 0;
    uint8_t unhalt = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_RELEASE_CORE, HBN_RELEASE_CORE_FLAG);
    unhalt = reg_get_bits(tmpVal, HBN_CORE_UNHALT);
    if (hcfg) {
        unhalt |= (1 << core);
    } else {
        unhalt &= ~(1 << core);
    }
    tmpVal = reg_set_bits(tmpVal, HBN_CORE_UNHALT, unhalt);
    writel(tmpVal, HBN_BASE + HBN_RSV2_OFFSET);

    return SUCCESS;
}

uint8_t HBN_Get_User_Boot_Config(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV2_OFFSET);
    if (HBN_RELEASE_CORE_FLAG == reg_get_bits(tmpVal, HBN_RELEASE_CORE)) {
        return reg_get_bits(tmpVal, HBN_USER_BOOT_SEL);
    }

    return 0x00;
}

int HBN_Set_User_Boot_Config(uint8_t ubCfg)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_RELEASE_CORE, HBN_RELEASE_CORE_FLAG);
    tmpVal = reg_set_bits(tmpVal, HBN_USER_BOOT_SEL, ubCfg);
    writel(tmpVal, HBN_BASE + HBN_RSV2_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Get_Ldo18io_Power_Delay_Config(uint16_t *pwrOffDly, uint16_t *pwrOnDly)
{
    uint32_t tmpVal = 0;

    if ((NULL == pwrOffDly) || (NULL == pwrOnDly)) {
        return ERROR;
    }

    tmpVal = readl(HBN_BASE + HBN_RSV2_OFFSET);
    if (HBN_LDO18IO_POWER_DLY_FLAG == reg_get_bits(tmpVal, HBN_LDO18IO_POWER_DLY_STS)) {
        *pwrOffDly = reg_get_bits(tmpVal, HBN_LDO18IO_POWER_OFF_DLY);
        *pwrOnDly = reg_get_bits(tmpVal, HBN_LDO18IO_POWER_ON_DLY);
        return SUCCESS;
    }

    return ERROR;
}

int ATTR_TCM_SECTION HBN_Set_Ldo18io_Power_Delay_Config(uint16_t pwrOffDly, uint16_t pwrOnDly)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_LDO18IO_POWER_DLY_STS, HBN_LDO18IO_POWER_DLY_FLAG);
    tmpVal = reg_set_bits(tmpVal, HBN_LDO18IO_POWER_OFF_DLY, pwrOffDly);
    tmpVal = reg_set_bits(tmpVal, HBN_LDO18IO_POWER_ON_DLY, pwrOnDly);
    writel(tmpVal, HBN_BASE + HBN_RSV2_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_Xtal_Type(uint8_t xtalType)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_XTAL_STS, HBN_XTAL_FLAG_VALUE);
    tmpVal = reg_set_bits(tmpVal, HBN_XTAL_TYPE, xtalType);
    writel(tmpVal, HBN_BASE + HBN_RSV3_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Get_Xtal_Type(uint8_t *xtalType)
{
    uint32_t tmpVal = 0;

    if (NULL == xtalType) {
        return ERROR;
    }

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    if (HBN_XTAL_FLAG_VALUE == reg_get_bits(tmpVal, HBN_XTAL_STS)) {
        *xtalType = reg_get_bits(tmpVal, HBN_XTAL_TYPE);
        return SUCCESS;
    }

    return ERROR;
}

int ATTR_TCM_SECTION HBN_Get_Xtal_Value(uint32_t *xtalVal)
{
    uint32_t tmpVal = 0;
    uint8_t xtalType = 0;

    if (NULL == xtalVal) {
        return ERROR;
    }

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    if (HBN_XTAL_FLAG_VALUE == reg_get_bits(tmpVal, HBN_XTAL_STS)) {
        xtalType = reg_get_bits(tmpVal, HBN_XTAL_TYPE);
        switch (xtalType) {
            case GLB_XTAL_NONE:
                *xtalVal = 0;
                break;
            case GLB_XTAL_24M:
                *xtalVal = 24000000;
                break;
            case GLB_XTAL_32M:
                *xtalVal = 32000000;
                break;
            case GLB_XTAL_38P4M:
                *xtalVal = 38400000;
                break;
            case GLB_XTAL_40M:
                *xtalVal = 40000000;
                break;
            case GLB_XTAL_26M:
                *xtalVal = 26000000;
                break;
            case GLB_XTAL_RC32M:
                *xtalVal = 32000000;
                break;
            default:
                *xtalVal = 0;
                break;
        }
        return SUCCESS;
    }

    *xtalVal = 0;
    return ERROR;
}

#if 0

int ATTR_TCM_SECTION HBN_Set_Flash_Power_Delay(uint8_t flashPwrDly)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_FLASH_POWER_STS, HBN_FLASH_POWER_DLY_FLAG);
    tmpVal = reg_set_bits(tmpVal, HBN_FLASH_POWER_DLY, flashPwrDly);
    writel(tmpVal, HBN_BASE + HBN_RSV3_OFFSET);

    return SUCCESS;
}
#endif

int ATTR_TCM_SECTION HBN_Get_Flash_Power_Delay(uint8_t *flashPwrDly)
{
    uint32_t tmpVal = 0;

    if (NULL == flashPwrDly) {
        return ERROR;
    }

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    if (HBN_FLASH_POWER_DLY_FLAG == reg_get_bits(tmpVal, HBN_FLASH_POWER_STS)) {
        *flashPwrDly = reg_get_bits(tmpVal, HBN_FLASH_POWER_DLY);
        return SUCCESS;
    }

    return ERROR;
}

int ATTR_TCM_SECTION HBN_Set_Reset_Reason(uint16_t rstReason)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_FLASH_POWER_STS, HBN_RESET_REASON_FLAG);
    tmpVal = reg_set_bits(tmpVal, HBN_FLASH_POWER_DLY, rstReason);
    writel(tmpVal, HBN_BASE + HBN_RSV3_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Get_Reset_Reason(uint16_t *rstReason)
{
    uint32_t tmpVal = 0;

    if (NULL == rstReason) {
        return ERROR;
    }

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    if (HBN_RESET_REASON_FLAG == reg_get_bits(tmpVal, HBN_FLASH_POWER_STS)) {
        *rstReason = reg_get_bits(tmpVal, HBN_FLASH_POWER_DLY);
        return SUCCESS;
    }

    return ERROR;

}

int ATTR_TCM_SECTION HBN_Set_Gpio_Keep(uint8_t gpioKeep)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_GPIO_KEEP_STS, HBN_GPIO_KEEP_FLAG);
    tmpVal = reg_set_bits(tmpVal, HBN_GPIO_KEEP_PIN, gpioKeep);
    writel(tmpVal, HBN_BASE + HBN_RSV3_OFFSET);
#endif
    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Get_Gpio_Keep(uint8_t *gpioKeep)
{
    uint32_t tmpVal = 0;

    if (NULL == gpioKeep) {
        return ERROR;
    }

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    if (HBN_GPIO_KEEP_FLAG == reg_get_bits(tmpVal, HBN_GPIO_KEEP_STS)) {
        *gpioKeep = reg_get_bits(tmpVal, HBN_GPIO_KEEP_PIN);
        return SUCCESS;
    }

    return ERROR;
}

int ATTR_TCM_SECTION HBN_Clear_Gpio_Keep(uint8_t gpioKeep)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_CR_GPIO_KEEP_EN, (gpioKeep & 0x7));
    tmpVal = reg_set_bits(tmpVal, HBN_REG_AON_GPIO_ISO_MODE, ((gpioKeep >> 3) & 0x1));
    writel(tmpVal, HBN_BASE + HBN_PAD_CTRL_0_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_PDS_Gpio_Keep(uint8_t gpioKeep)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PDS_GPIO_KEEP_STS, PDS_GPIO_KEEP_FLAG);
    tmpVal = reg_set_bits(tmpVal, PDS_GPIO_KEEP_PIN, gpioKeep);
    writel(tmpVal, HBN_BASE + HBN_RSV3_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Get_PDS_Gpio_Keep(uint8_t *gpioKeep)
{
    uint32_t tmpVal = 0;

    if (NULL == gpioKeep) {
        return ERROR;
    }

    tmpVal = readl(HBN_BASE + HBN_RSV3_OFFSET);
    if (PDS_GPIO_KEEP_FLAG == reg_get_bits(tmpVal, PDS_GPIO_KEEP_STS)) {
        *gpioKeep = reg_get_bits(tmpVal, PDS_GPIO_KEEP_PIN);
        return SUCCESS;
    }

    return ERROR;
}

int ATTR_TCM_SECTION HBN_Clear_PDS_Gpio_Keep(uint8_t gpioKeep)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_CTL5_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_GPIO_KEEP_EN, (gpioKeep & 0x7));
    writel(tmpVal, PDS_BASE + PDS_CTL5_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_CTL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_GPIO_ISO_MODE, ((gpioKeep >> 3) & 0x1));
    /* don't entry PDS */
    tmpVal = reg_clr_bit(tmpVal, PDS_START_PS);
    writel(tmpVal, PDS_BASE + PDS_CTL_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Clear_RTC_Counter(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Clear RTC control bit0 */
    writel(tmpVal & 0xfffffffe, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Enable_RTC_Counter(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Set RTC control bit0 */
    writel(tmpVal | 0x01, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Set_RTC_Timer(uint8_t delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode)
{
    uint32_t tmpVal;

    /* Check the parameters */

    writel(compValLow, HBN_BASE + HBN_TIME_L_OFFSET);
    writel(compValHigh & 0xff, HBN_BASE + HBN_TIME_H_OFFSET);

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Set interrupt delay option */
    tmpVal = reg_set_bits(tmpVal, HBN_RTC_DLY_OPTION, delay);
    /* Set RTC compare mode */
    tmpVal |= (compMode << 1);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Get_RTC_Timer_Val(uint32_t *valLow, uint32_t *valHigh)
{
    uint32_t tmpVal;

    /* Tigger RTC val read */
    tmpVal = readl(HBN_BASE + HBN_RTC_TIME_H_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_RTC_TIME_LATCH);
    writel(tmpVal, HBN_BASE + HBN_RTC_TIME_H_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_RTC_TIME_LATCH);
    writel(tmpVal, HBN_BASE + HBN_RTC_TIME_H_OFFSET);

    /* Read RTC val */
    *valLow = readl(HBN_BASE + HBN_RTC_TIME_L_OFFSET);
    *valHigh = (readl(HBN_BASE + HBN_RTC_TIME_H_OFFSET) & 0xff);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Recal_RC32K(int32_t expected_counter, int32_t actual_counter)
{
    int32_t tmpVal = 0;
    int32_t current = 0;
    int32_t delta = 0;

    delta = actual_counter - expected_counter;

    /* normalize to 1s count */
    delta = (delta * 32768) / expected_counter;

    if ((delta < 32) && (delta > -32)) {
        return -1;
    }
    if (delta < -320) {
        delta = -320;
    } else if (delta > 320) {
        delta = 320;
    }

    if (delta >= 64 || delta <= -64) {
        delta = delta / 64;
    } else {
        delta = delta / 32;
    }

    tmpVal = readl(HBN_BASE + HBN_RC32K_CTRL0_OFFSET);
    current = reg_get_bits(tmpVal, HBN_RC32K_CODE_FR_EXT);
    current += delta;
    current &= ((1U << HBN_RC32K_CODE_FR_EXT_LEN) - 1);
    tmpVal = reg_set_bits(tmpVal, HBN_RC32K_CODE_FR_EXT, current);
    writel(tmpVal, HBN_BASE + HBN_RC32K_CTRL0_OFFSET);

    return SUCCESS;
}

int HBN_Clear_RTC_INT(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    /* Clear RTC commpare:bit1-3 for clearing Delayed RTC IRQ */
    writel(tmpVal & 0xfffffff1, HBN_BASE + HBN_CTL_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_GPIO_INT_Enable(uint8_t gpioIntTrigType)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MODE, gpioIntTrigType);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
#endif
    return SUCCESS;
}

int HBN_GPIO_INT_Disable(void)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MASK, 0);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
#endif
    return SUCCESS;
}

int HBN_Get_INT_State(uint8_t irqType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(HBN_BASE + HBN_IRQ_STAT_OFFSET);

    if (tmpVal & (1 << irqType)) {
        return SET;
    } else {
        return RESET;
    }
}

uint8_t HBN_Get_Pin_Wakeup_Mode(void)
{
    return reg_get_bits(readl(HBN_BASE + HBN_IRQ_MODE_OFFSET), HBN_PIN_WAKEUP_MODE);
}

int HBN_Clear_IRQ(uint8_t irqType)
{
    uint32_t tmpVal;

    /* set clear bit */
    tmpVal = readl(HBN_BASE + HBN_IRQ_CLR_OFFSET);
    tmpVal |= (1 << irqType);
    writel(tmpVal, HBN_BASE + HBN_IRQ_CLR_OFFSET);

    /* unset clear bit */
    tmpVal = readl(HBN_BASE + HBN_IRQ_CLR_OFFSET);
    tmpVal &= (~(1 << irqType));
    writel(tmpVal, HBN_BASE + HBN_IRQ_CLR_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Hw_Pu_Pd_Cfg(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, HBN_REG_EN_HW_PU_PD);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_REG_EN_HW_PU_PD);
    }
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Pin_WakeUp_Mask(uint8_t maskVal)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MASK, maskVal);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Aon_Pad_Ctrl(uint32_t aonPadCtl1, uint32_t aonPadCtl2)
{
#ifndef BOOTROM
    writel(aonPadCtl1, HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    writel(aonPadCtl2, HBN_BASE + HBN_PAD_CTRL_1_OFFSET);
#endif
    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Aon_Pad_Cfg(uint8_t aonPadHwCtrlEn, uint8_t aonGpio, HBN_AON_PAD_CFG_Type *aonPadCfg)
{
    uint32_t tmpVal0;
    uint32_t tmpVal1;
    uint32_t enAonCtrlGpio;
    uint32_t aonPadIeSmt;
    uint32_t aonPadPu;
    uint32_t aonPadPd;
    uint32_t aonPadOe;

    if (GLB_PACKAGE_TYPE_QFN56 != GLB_Get_Package_Type()) {
        if ((aonGpio == HBN_AON_PAD_GPIO18) || (aonGpio == HBN_AON_PAD_GPIO19)) {
            return ERROR;
        }
    }

    if (NULL == aonPadCfg) {
        return ERROR;
    }

    /* hbn_pad_ctrl_0 */
    tmpVal0 = readl(HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    enAonCtrlGpio = reg_get_bits(tmpVal0, HBN_REG_EN_AON_CTRL_GPIO);
    if (aonPadCfg->ctrlEn) {
        enAonCtrlGpio |= (1 << aonGpio);
    } else {
        enAonCtrlGpio &= ~(1 << aonGpio);
    }
    tmpVal0 = reg_set_bits(tmpVal0, HBN_REG_EN_AON_CTRL_GPIO, enAonCtrlGpio);
    aonPadIeSmt = reg_get_bits(tmpVal0, HBN_REG_AON_PAD_IE_SMT);
    if (aonPadCfg->ie) {
        aonPadIeSmt |= (1 << aonGpio);
    } else {
        aonPadIeSmt &= ~(1 << aonGpio);
    }
    tmpVal0 = reg_set_bits(tmpVal0, HBN_REG_AON_PAD_IE_SMT, aonPadIeSmt);
    writel(tmpVal0, HBN_BASE + HBN_PAD_CTRL_0_OFFSET);

    /* hbn_pad_ctrl_1 */
    tmpVal1 = readl(HBN_BASE + HBN_PAD_CTRL_1_OFFSET);
    aonPadPu = reg_get_bits(tmpVal1, HBN_REG_AON_PAD_PU);
    if (aonPadCfg->pullUp) {
        aonPadPu |= (1 << aonGpio);
    } else {
        aonPadPu &= ~(1 << aonGpio);
    }
    tmpVal1 = reg_set_bits(tmpVal1, HBN_REG_AON_PAD_PU, aonPadPu);
    aonPadPd = reg_get_bits(tmpVal1, HBN_REG_AON_PAD_PD);
    if (aonPadCfg->pullDown) {
        aonPadPd |= (1 << aonGpio);
    } else {
        aonPadPd &= ~(1 << aonGpio);
    }
    tmpVal1 = reg_set_bits(tmpVal1, HBN_REG_AON_PAD_PD, aonPadPd);
    aonPadOe = reg_get_bits(tmpVal1, HBN_REG_AON_PAD_OE);
    if (aonPadCfg->oe) {
        aonPadOe |= (1 << aonGpio);
    } else {
        aonPadOe &= ~(1 << aonGpio);
    }
    tmpVal1 = reg_set_bits(tmpVal1, HBN_REG_AON_PAD_OE, aonPadOe);
    writel(tmpVal1, HBN_BASE + HBN_PAD_CTRL_1_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION HBN_Aon_Pad_Cfg_Set(uint8_t aonPadHwCtrlEn, uint8_t aonGpio)
{
#ifndef BOOTROM
    uint32_t tmpVal;
    uint32_t enAonCtrlGpio;

    /* hbn_pad_ctrl_0 */
    tmpVal = readl(HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    enAonCtrlGpio = reg_get_bits(tmpVal, HBN_REG_EN_AON_CTRL_GPIO);

    if (aonPadHwCtrlEn) {
        enAonCtrlGpio |= (1 << aonGpio);
    } else {
        enAonCtrlGpio &= ~(1 << aonGpio);
    }
    tmpVal = reg_set_bits(tmpVal, HBN_REG_EN_AON_CTRL_GPIO, enAonCtrlGpio);

    writel(tmpVal, HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
#endif
    return SUCCESS;
}

int HBN_Enable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    if (acompId == AON_ACOMP0_ID) {
        tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
        tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP0_EN);
        tmpVal2 = tmpVal2 | edge;
        tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
        writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
    } else if (acompId == AON_ACOMP1_ID) {
        tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
        tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP1_EN);
        tmpVal2 = tmpVal2 | edge;
        tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
        writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
    }

    return SUCCESS;
}

int HBN_Disable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    if (acompId == (uint8_t)AON_ACOMP0_ID) {
        tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
        tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP0_EN);
        tmpVal2 = tmpVal2 & (~edge);
        tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
        writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
    } else if (acompId == (uint8_t)AON_ACOMP1_ID) {
        tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
        tmpVal2 = reg_get_bits(tmpVal, HBN_IRQ_ACOMP1_EN);
        tmpVal2 = tmpVal2 & (~edge);
        tmpVal = reg_set_bits(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
        writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
    }

    return SUCCESS;
}

int HBN_Enable_BOD_IRQ(void)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_IRQ_BOR_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
#endif
    return SUCCESS;
}

int HBN_Disable_BOD_IRQ(void)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_IRQ_BOR_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
#endif
    return SUCCESS;
}

#ifndef IPRO_USE_HAL_DRIVER
int HBN_Out0_Callback_Install(HBN_OUT0_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(HBN_OUT0_IRQn, HBN_OUT0_IRQHandler);
#endif

    hbnInt0CbfArra[intType] = cbFun;

    return SUCCESS;
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
int HBN_Out1_Callback_Install(HBN_OUT1_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(HBN_OUT1_IRQn, HBN_OUT1_IRQHandler);
#endif

    hbnInt1CbfArra[intType] = cbFun;

    return SUCCESS;
}
#endif

int HBN_Aon_Pad_WakeUpCfg(int puPdEn, uint8_t trigMode, uint32_t maskVal, int dlyEn, uint8_t dlySec)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_EN, dlyEn);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_SEL, dlySec);
    tmpVal = reg_set_bits(tmpVal, HBN_REG_EN_HW_PU_PD, puPdEn);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MASK, maskVal);
    tmpVal = reg_set_bits(tmpVal, HBN_PIN_WAKEUP_MODE, trigMode);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
#endif
    return SUCCESS;
}

int HBN_Set_BOD_Cfg(HBN_BOD_CFG_Type *cfg)
{
#ifndef BOOTROM
    uint32_t tmpVal = 0;

    if (cfg->enableBodInt) {
        HBN_Enable_BOD_IRQ();
    } else {
        HBN_Disable_BOD_IRQ();
    }

    tmpVal = readl(HBN_BASE + HBN_BOR_CFG_OFFSET);

    tmpVal = reg_set_bits(tmpVal, HBN_BOD_VTH, cfg->bodThreshold);
    if (cfg->enablePorInBod) {
        tmpVal = reg_set_bit(tmpVal, HBN_BOD_SEL);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_BOD_SEL);
    }

    if (cfg->enableBod) {
        tmpVal = reg_set_bit(tmpVal, HBN_PU_BOD);
    } else {
        tmpVal = reg_clr_bit(tmpVal, HBN_PU_BOD);
    }

    writel(tmpVal, HBN_BASE + HBN_BOR_CFG_OFFSET);
#endif
    return SUCCESS;
}

#ifndef IPRO_USE_HAL_DRIVER
void HBN_OUT0_IRQHandler(void)
{
    HBN_OUT0_INT_Type out0Int = HBN_OUT0_INT_GPIO16;

    /* GPIO and RTC */
    for (uint8_t gpioInt = HBN_INT_GPIO16; gpioInt <= HBN_INT_GPIO19; gpioInt++) {
        if (SET == HBN_Get_INT_State(gpioInt)) {
            HBN_Clear_IRQ(gpioInt);
            switch (gpioInt) {
                case HBN_INT_GPIO16:
                    out0Int = HBN_OUT0_INT_GPIO16;
                    break;
                case HBN_INT_GPIO17:
                    out0Int = HBN_OUT0_INT_GPIO17;
                    break;
                case HBN_INT_GPIO18:
                    out0Int = HBN_OUT0_INT_GPIO18;
                    break;
                case HBN_INT_GPIO19:
                    out0Int = HBN_OUT0_INT_GPIO19;
                    break;
                default:
                    break;
            }
            if (hbnInt0CbfArra[out0Int] != NULL) {
                hbnInt0CbfArra[out0Int]();
            }
        }
    }
    if (SET == HBN_Get_INT_State(HBN_INT_RTC)) {
        HBN_Clear_IRQ(HBN_INT_RTC);
        if (hbnInt0CbfArra[HBN_OUT0_INT_RTC] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_RTC]();
        }
    }
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
void HBN_OUT1_IRQHandler(void)
{
    /* PIR */
    if (SET == HBN_Get_INT_State(HBN_INT_PIR)) {
        HBN_Clear_IRQ(HBN_INT_PIR);
        if (hbnInt1CbfArra[HBN_OUT1_INT_PIR] != NULL) {
            hbnInt1CbfArra[HBN_OUT1_INT_PIR]();
        }
    }
    /* BOD */
    if (SET == HBN_Get_INT_State(HBN_INT_BOD)) {
        HBN_Clear_IRQ(HBN_INT_BOD);
        if (hbnInt1CbfArra[HBN_OUT1_INT_BOD] != NULL) {
            hbnInt1CbfArra[HBN_OUT1_INT_BOD]();
        }
    }
    /* ACOMP0 */
    if (SET == HBN_Get_INT_State(HBN_INT_ACOMP0)) {
        HBN_Clear_IRQ(HBN_INT_ACOMP0);
        if (hbnInt1CbfArra[HBN_OUT1_INT_ACOMP0] != NULL) {
            hbnInt1CbfArra[HBN_OUT1_INT_ACOMP0]();
        }
    }
    /* ACOMP1 */
    if (SET == HBN_Get_INT_State(HBN_INT_ACOMP1)) {
        HBN_Clear_IRQ(HBN_INT_ACOMP1);
        if (hbnInt1CbfArra[HBN_OUT1_INT_ACOMP1] != NULL) {
            hbnInt1CbfArra[HBN_OUT1_INT_ACOMP1]();
        }
    }
}
#endif

void HBN_Get_Reset_Event(uint8_t *event)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);

    *event = reg_get_bits(tmpVal, HBN_RESET_EVENT);
}

void HBN_Clr_Reset_Event(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);

    tmpVal |= (1 << 13);

    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
}

int HBN_Disable_AComp0_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_IRQ_ACOMP0_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Disable_AComp1_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_IRQ_ACOMP1_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Enable_AComp0_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_IRQ_ACOMP0_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

int HBN_Enable_AComp1_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_IRQ_ACOMP1_EN);
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);

    return SUCCESS;
}

