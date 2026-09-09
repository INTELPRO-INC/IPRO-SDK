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



#include "ipro6.h"
#include "drv_pds.h"

#ifndef IPRO_USE_HAL_DRIVER
static intCallback_Type *pdsIntCbfArra[PDS_INT_MAX] = { NULL };
#endif

int ATTR_TCM_SECTION PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(uint8_t grp, uint8_t pu, uint8_t pd, uint8_t ie)
{
    uint32_t tmpVal;
    uint32_t tmpValPu;
    uint32_t tmpValPd;
    uint32_t tmpValIe;

    /* pu/pd/ie config */
    tmpVal = readl(PDS_BASE + PDS_GPIO_I_SET_OFFSET);
    tmpValPu = reg_get_bits(tmpVal, PDS_CR_PDS_GPIO_PU_SET);
    if (pu) {
        if (GLB_PACKAGE_TYPE_QFN56 != GLB_Get_Package_Type()) {
            if (SET == GLB_Get_PAD_Bonging_to_GND_Sts()) {
                /* IF it is 40 PACKAGE TYPE and PAD Bonging to GND, Pull-UP is forbidden */
                tmpValPu &= ~(1 << grp);
            } else {
                tmpValPu |= (1 << grp);
            }
        } else {
            tmpValPu |= (1 << grp);
        }
    } else {
        tmpValPu &= ~(1 << grp);
    }
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_GPIO_PU_SET, tmpValPu);
    tmpValPd = reg_get_bits(tmpVal, PDS_CR_PDS_GPIO_PD_SET);
    if (pd) {
        tmpValPd |= (1 << grp);
    } else {
        tmpValPd &= ~(1 << grp);
    }
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_GPIO_PD_SET, tmpValPd);
    tmpValIe = reg_get_bits(tmpVal, PDS_CR_PDS_GPIO_IE_SET);
    if (ie) {
        tmpValIe |= (1 << grp);
    } else {
        tmpValIe &= ~(1 << grp);
    }
    tmpVal = reg_set_bits(tmpVal, PDS_CR_PDS_GPIO_IE_SET, tmpValIe);
    writel(tmpVal, PDS_BASE + PDS_GPIO_I_SET_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMask(uint8_t pad, int intMask)
{
    uint32_t tmpVal = 0;
    uint32_t pos = 0;

    /* [15:0] : GPIO0~15   */
    /* [30:16] : GPIO20~34 */
    if (pad <= GLB_GPIO_PIN_15) {
        pos = pad;
    } else if ((pad >= GLB_GPIO_PIN_20) && (pad <= GLB_GPIO_PIN_34)) {
        pos = pad - 4;
    } else {
        return ERROR;
    }

    tmpVal = readl(PDS_BASE + PDS_GPIO_PD_SET_OFFSET);
    if (UNMASK != intMask) {
        /* MASK */
        tmpVal |= (1 << pos);
    } else {
        /* UNMASK */
        tmpVal &= ~(1 << pos);
    }
    writel(tmpVal, PDS_BASE + PDS_GPIO_PD_SET_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMode(uint8_t set, uint8_t trig)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_GPIO_INT_OFFSET);
    switch (set) {
        case PDS_GPIO_INT_SET_1_GPIO0_GPIO7:
            tmpVal = reg_set_bits(tmpVal, PDS_GPIO_SET1_INT_MODE, trig);
            break;
        case PDS_GPIO_INT_SET_2_GPIO8_GPIO15:
            tmpVal = reg_set_bits(tmpVal, PDS_GPIO_SET2_INT_MODE, trig);
            break;
        case PDS_GPIO_INT_SET_3_GPIO20_GPIO27:
            tmpVal = reg_set_bits(tmpVal, PDS_GPIO_SET3_INT_MODE, trig);
            break;
        case PDS_GPIO_INT_SET_4_GPIO28_GPIO34:
            tmpVal = reg_set_bits(tmpVal, PDS_GPIO_SET4_INT_MODE, trig);
            break;
        default:
            break;
    }
    writel(tmpVal, PDS_BASE + PDS_GPIO_INT_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntClr(uint8_t set)
{
    uint32_t tmpVal = 0;

    /* pds_gpio_setx_int_clr = 0 */
    tmpVal = readl(PDS_BASE + PDS_GPIO_INT_OFFSET);
    switch (set) {
        case PDS_GPIO_INT_SET_1_GPIO0_GPIO7:
            tmpVal = reg_clr_bit(tmpVal, PDS_GPIO_SET1_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_2_GPIO8_GPIO15:
            tmpVal = reg_clr_bit(tmpVal, PDS_GPIO_SET2_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_3_GPIO20_GPIO27:
            tmpVal = reg_clr_bit(tmpVal, PDS_GPIO_SET3_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_4_GPIO28_GPIO34:
            tmpVal = reg_clr_bit(tmpVal, PDS_GPIO_SET4_INT_CLR);
            break;
        default:
            break;
    }
    writel(tmpVal, PDS_BASE + PDS_GPIO_INT_OFFSET);

    /* pds_gpio_setx_int_clr = 1 */
    tmpVal = readl(PDS_BASE + PDS_GPIO_INT_OFFSET);
    switch (set) {
        case PDS_GPIO_INT_SET_1_GPIO0_GPIO7:
            tmpVal = reg_set_bit(tmpVal, PDS_GPIO_SET1_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_2_GPIO8_GPIO15:
            tmpVal = reg_set_bit(tmpVal, PDS_GPIO_SET2_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_3_GPIO20_GPIO27:
            tmpVal = reg_set_bit(tmpVal, PDS_GPIO_SET3_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_4_GPIO28_GPIO34:
            tmpVal = reg_set_bit(tmpVal, PDS_GPIO_SET4_INT_CLR);
            break;
        default:
            break;
    }
    writel(tmpVal, PDS_BASE + PDS_GPIO_INT_OFFSET);

    /* pds_gpio_setx_int_clr = 0 */
    tmpVal = readl(PDS_BASE + PDS_GPIO_INT_OFFSET);
    switch (set) {
        case PDS_GPIO_INT_SET_1_GPIO0_GPIO7:
            tmpVal = reg_clr_bit(tmpVal, PDS_GPIO_SET1_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_2_GPIO8_GPIO15:
            tmpVal = reg_clr_bit(tmpVal, PDS_GPIO_SET2_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_3_GPIO20_GPIO27:
            tmpVal = reg_clr_bit(tmpVal, PDS_GPIO_SET3_INT_CLR);
            break;
        case PDS_GPIO_INT_SET_4_GPIO28_GPIO34:
            tmpVal = reg_clr_bit(tmpVal, PDS_GPIO_SET4_INT_CLR);
            break;
        default:
            break;
    }
    writel(tmpVal, PDS_BASE + PDS_GPIO_INT_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Get_GPIO_Pad_IntStatus(uint8_t pad)
{
    uint32_t pos = 0;

    /* [15:0]  : GPIO0~15  */
    /* [30:16] : GPIO20~34 */
    if (pad <= GLB_GPIO_PIN_15) {
        pos = pad;
    } else if ((pad >= GLB_GPIO_PIN_20) && (pad <= GLB_GPIO_PIN_34)) {
        pos = pad - 4;
    } else {
        return RESET;
    }

    return (readl(PDS_BASE + PDS_GPIO_STAT_OFFSET) & (1 << pos)) ? SET : RESET;
}

int ATTR_TCM_SECTION PDS_Set_Flash_Pad_Pull_None(uint8_t pinCfg)
{

    if (pinCfg >= SF_IO_EXT_SF2_SWAP_IO3IO0) {
        PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(PDS_GPIO_GROUP_SET_GPIO0_GPIO15, 0, 0, 0);
    } else {
        return INVALID;
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Set_Flash_Pad_Pull_None_Fast(uint8_t pinCfg)
{
    uint32_t tmpVal;

    if (pinCfg >= SF_IO_EXT_SF2_SWAP_IO3IO0) {
        /* pu/pd/ie config */
        tmpVal = readl(PDS_BASE + PDS_GPIO_I_SET_OFFSET);
        /* GPIO4~9 [6][3][0] = 0 */
        tmpVal &= ~(0x49);
        writel(tmpVal, PDS_BASE + PDS_GPIO_I_SET_OFFSET);
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
    /* don't entry PDS */
    tmpVal = reg_clr_bit(tmpVal, PDS_START_PS);
    writel(tmpVal, PDS_BASE + PDS_CTL_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_CTL5_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_GPIO_KEEP_EN);
    writel(tmpVal, PDS_BASE + PDS_CTL5_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION PDS_Set_MCU0_Clock_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_REG_MCU1_CLK_EN);
    writel(tmpVal, PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION PDS_Set_MCU0_Clock_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_REG_MCU1_CLK_EN);
    writel(tmpVal, PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION PDS_Set_MCU0_Reset_Address(uint32_t addr)
{
    writel(addr, PDS_BASE + PDS_CPU_CORE_CFG14_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION PDS_Pu_PLL_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_CR_PDS_PU_CLKPLL_SFREG);
    tmpVal = reg_set_bit(tmpVal, PDS_CR_PDS_PU_CLKPLL);
    writel(tmpVal, PDS_BASE + PDS_PU_RST_CLKPLL_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION PDS_Pu_PLL_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_PU_CLKPLL_SFREG);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_PU_CLKPLL);
    writel(tmpVal, PDS_BASE + PDS_PU_RST_CLKPLL_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt)
{
    uint32_t tmpVal = 0;
    /* sleepForever set 1, chip will sleep forever */
    /* PDS sleep time 1~PDS_WARMUP_LATENCY_CNT <=> error */
    /* PDS sleep time >PDS_WARMUP_LATENCY_CNT <=> correct */
    if ((pdsSleepCnt) && (pdsSleepCnt <= PDS_WARMUP_LATENCY_CNT)) {
        return ERROR;
    } else if (pdsSleepCnt > PDS_WARMUP_LATENCY_CNT) {
        writel(pdsSleepCnt - PDS_WARMUP_LATENCY_CNT, PDS_BASE + PDS_TIME1_OFFSET);
    } else {
    }

    /* PDS_CTL4 config */
    writel(*(uint32_t *)cfg4, PDS_BASE + PDS_CTL4_OFFSET);

    /* PDS_CTL config */
    if (cfg->pdsStart) {
        /* clear pds int */
        tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
        tmpVal = reg_set_bit(tmpVal, PDS_CR_PDS_INT_CLR);
        writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

        tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_INT_CLR);
        writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

        writel((*(uint32_t *)cfg & ~(1 << 0)), PDS_BASE + PDS_CTL_OFFSET);
        writel((*(uint32_t *)cfg | (1 << 0)), PDS_BASE + PDS_CTL_OFFSET);
    } else {
        writel(*(uint32_t *)cfg, PDS_BASE + PDS_CTL_OFFSET);
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3, PDS_CTL5_Type *cfg5)
{
    /* PDS_CTL2 config */
    writel(*(uint32_t *)cfg2, PDS_BASE + PDS_CTL2_OFFSET);

    /* PDS_CTL3 config */
    writel(*(uint32_t *)cfg3, PDS_BASE + PDS_CTL3_OFFSET);

    /* PDS_CTL5 config */
    writel(*(uint32_t *)cfg5, PDS_BASE + PDS_CTL5_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_RAM_Config(PDS_CTRL_RAM1_Type *ram1Cfg, PDS_CTRL_RAM2_Type *ram2Cfg,
                                            PDS_CTRL_RAM3_Type *ram3Cfg, PDS_CTRL_RAM4_Type *ram4Cfg)
{
    if ((NULL == ram1Cfg) || (NULL == ram2Cfg) || (NULL == ram3Cfg) || (NULL == ram4Cfg)) {
        return ERROR;
    }

    writel(*(uint32_t *)ram1Cfg, PDS_BASE + PDS_RAM1_OFFSET);
    writel(*(uint32_t *)ram2Cfg, PDS_BASE + PDS_RAM2_OFFSET);
    writel(*(uint32_t *)ram3Cfg, PDS_BASE + PDS_RAM3_OFFSET);
    writel(*(uint32_t *)ram4Cfg, PDS_BASE + PDS_RAM4_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt)
{
    PDS_Force_Config((PDS_CTL2_Type *)&(defaultLvCfg->pdsCtl2), (PDS_CTL3_Type *)&(defaultLvCfg->pdsCtl3), (PDS_CTL5_Type *)&(defaultLvCfg->pdsCtl5));
    PDS_Enable((PDS_CTL_Type *)&(defaultLvCfg->pdsCtl), (PDS_CTL4_Type *)&(defaultLvCfg->pdsCtl4), pdsSleepCnt);

    return SUCCESS;
}

int PDS_IntMask(uint8_t intType, int intMask)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    if (intMask != UNMASK) {
        tmpVal = tmpVal | (1 << (intType + PDS_INT_MASK_BIT_OFFSET));
    } else {
        tmpVal = tmpVal & ~(1 << (intType + PDS_INT_MASK_BIT_OFFSET));
    }
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    return SUCCESS;
}

int PDS_Get_IntStatus(uint8_t intType)
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

/* Enable/disable a single PDS wakeup trigger source.
 * cr_pds_wakeup_src_en occupies PDS_INT[19:10]; src is the bit index within
 * that field (0 = pds_sleep_cnt, 1 = hbn_irq_out, 2 = gpio_irq,
 * 3 = pds_gpio_int, ...). */
int ATTR_TCM_SECTION PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_Type src, int enable)
{
    uint32_t tmpVal;

    if ((uint32_t)src >= PDS_CR_PDS_WAKEUP_SRC_EN_LEN) {
        return ERROR;
    }

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    if (enable) {
        tmpVal |= (1U << (PDS_CR_PDS_WAKEUP_SRC_EN_POS + (uint32_t)src));
    } else {
        tmpVal &= ~(1U << (PDS_CR_PDS_WAKEUP_SRC_EN_POS + (uint32_t)src));
    }
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    return SUCCESS;
}

/* Disable every PDS wakeup trigger source (clear the whole src_en field). */
int ATTR_TCM_SECTION PDS_Wakeup_Src_En_DisableAll(void)
{
    uint32_t tmpVal;

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    tmpVal &= PDS_CR_PDS_WAKEUP_SRC_EN_UMSK;
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    return SUCCESS;
}

/* Clear pending interrupt status on all four PDS GPIO interrupt sets. */
int ATTR_TCM_SECTION PDS_Set_All_GPIO_IntClear(void)
{
    PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_1_GPIO0_GPIO7);
    PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_2_GPIO8_GPIO15);
    PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_3_GPIO20_GPIO27);
    PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_4_GPIO28_GPIO34);

    return SUCCESS;
}

#ifndef IPRO_USE_HAL_DRIVER
int PDS_Int_Callback_Install(uint8_t intType, intCallback_Type *cbFun)
{

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(PDS_WAKEUP_IRQn, PDS_WAKEUP_IRQHandler);
#endif

    pdsIntCbfArra[intType] = cbFun;

    return SUCCESS;
}
#endif

int ATTR_CLOCK_SECTION PDS_Trim_RC32M(void)
{
    Efuse_Ana_RC32M_Trim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_RC32M_Trim(&trim);
    if (trim.rc32mCodeFrExt2En) {
        if (trim.rc32mCodeFrExt2Parity == EF_Ctrl_Get_Trim_Parity(trim.rc32mCodeFrExt2, 8)) {
            tmpVal = readl(PDS_BASE + PDS_RC32M_CTRL0_OFFSET);
            tmpVal = reg_set_bit(tmpVal, PDS_RC32M_EXT_CODE_EN);
            writel(tmpVal, PDS_BASE + PDS_RC32M_CTRL0_OFFSET);
            arch_delay_us(2);
            tmpVal = readl(PDS_BASE + PDS_RC32M_CTRL2_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PDS_RC32M_CODE_FR_EXT2, trim.rc32mCodeFrExt2);
            writel(tmpVal, PDS_BASE + PDS_RC32M_CTRL2_OFFSET);
            tmpVal = readl(PDS_BASE + PDS_RC32M_CTRL2_OFFSET);
            tmpVal = reg_set_bit(tmpVal, PDS_RC32M_EXT_CODE_SEL);
            writel(tmpVal, PDS_BASE + PDS_RC32M_CTRL2_OFFSET);
            /* hw_5T + sw_5T  */
            arch_delay_us(1);
            return SUCCESS;
        }
    }

    return ERROR;
}

int PDS_Turn_On_USB(uint8_t waitReady)
{
    uint32_t tmpVal = 0;

    /* USB_PHY_CTRL[3:2] reg_usb_phy_xtlsel=0                             */
    /* 2000e504 = 0x40; #100; USB_PHY_CTRL[6] reg_pu_usb20_psw=1 (VCC33A) */
    /* 2000e504 = 0x41; #500; USB_PHY_CTRL[0] reg_usb_phy_ponrst=1        */
    /* 2000e500 = 0x20; #100; USB_CTL[0] reg_usb_sw_rst_n=0               */
    /* 2000e500 = 0x22; #500; USB_CTL[1] reg_usb_ext_susp_n=1             */
    /* 2000e500 = 0x23; #100; USB_CTL[0] reg_usb_sw_rst_n=1               */
    /* #1.2ms; wait UCLK                                                  */
    /* wait(soc616_b0.usb_uclk);                                          */

    tmpVal = readl(PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PDS_REG_USB_PHY_XTLSEL, 0);
    writel(tmpVal, PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_REG_PU_USB20_PSW);
    writel(tmpVal, PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_REG_USB_PHY_PONRST);
    writel(tmpVal, PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    /* greater than 5T */
    arch_delay_us(1);

    tmpVal = readl(PDS_BASE + PDS_USB_CTL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_REG_USB_SW_RST_N);
    writel(tmpVal, PDS_BASE + PDS_USB_CTL_OFFSET);

    /* greater than 5T */
    arch_delay_us(1);

    tmpVal = readl(PDS_BASE + PDS_USB_CTL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_REG_USB_EXT_SUSP_N);
    writel(tmpVal, PDS_BASE + PDS_USB_CTL_OFFSET);

    /* wait UCLK 1.2ms */
    arch_delay_ms(3);

    tmpVal = readl(PDS_BASE + PDS_USB_CTL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_REG_USB_SW_RST_N);
    writel(tmpVal, PDS_BASE + PDS_USB_CTL_OFFSET);

    if (waitReady) {
        arch_delay_ms(2);
    }

    return SUCCESS;
}

int PDS_Turn_Off_USB(void)
{
    uint32_t tmpVal = 0;

    /* USB_PHY_CTRL[3:2] reg_usb_phy_xtlsel=0                             */
    /* 2000e504 = 0x40; #100; USB_PHY_CTRL[6] reg_pu_usb20_psw=1 (VCC33A) */
    /* 2000e504 = 0x41; #500; USB_PHY_CTRL[0] reg_usb_phy_ponrst=1        */
    /* 2000e500 = 0x20; #100; USB_CTL[0] reg_usb_sw_rst_n=0               */
    /* 2000e500 = 0x22; #500; USB_CTL[1] reg_usb_ext_susp_n=1             */
    /* 2000e500 = 0x23; #100; USB_CTL[0] reg_usb_sw_rst_n=1               */
    /* #1.2ms; wait UCLK                                                  */
    /* wait(soc616_b0.usb_uclk);                                          */

    tmpVal = readl(PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PDS_REG_USB_PHY_XTLSEL, 0);
    writel(tmpVal, PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_REG_PU_USB20_PSW);
    writel(tmpVal, PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_REG_USB_PHY_PONRST);
    writel(tmpVal, PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    return SUCCESS;
}

int PDS_Set_USB_Suspend(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_USB_CTL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_REG_USB_EXT_SUSP_N);
    writel(tmpVal, PDS_BASE + PDS_USB_CTL_OFFSET);

    return SUCCESS;
}

int PDS_Set_USB_Resume(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_USB_CTL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_REG_USB_EXT_SUSP_N);
    writel(tmpVal, PDS_BASE + PDS_USB_CTL_OFFSET);

    return SUCCESS;
}

#ifndef IPRO_USE_HAL_DRIVER
void PDS_WAKEUP_IRQHandler(void)
{
    for (uint8_t intType = PDS_INT_WAKEUP; intType < PDS_INT_MAX; intType++) {
        if (PDS_Get_IntStatus(intType) && (pdsIntCbfArra[intType] != NULL)) {
            pdsIntCbfArra[intType]();
        }
    }

    PDS_IntClear();
}
#endif

int PDS_Power_Off_WB(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PDS_CR_PDS_FORCE_WB_ISO_EN);
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    tmpVal = reg_set_bit(tmpVal, PDS_CR_PDS_FORCE_WB_PWR_OFF);
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    return SUCCESS;
}

int PDS_Power_On_WB(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(PDS_BASE + PDS_CTL2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_FORCE_WB_PWR_OFF);
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_FORCE_WB_ISO_EN);
    writel(tmpVal, PDS_BASE + PDS_CTL2_OFFSET);

    return SUCCESS;
}

int PDS_Reset_Low_Power_USB_Ctrl(void)
{
    uint32_t tmpVal = 0;

    /* 2000E000[19]    = 0 */
    /* 2000E00C[18:17] = 0 */
    /* 2000E014[29]    = 0 */
    /* 2000E018[21:20] = 0 */
    /* 2000E024[8]     = 0 */

    tmpVal = readl(PDS_BASE + PDS_CTL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_CTRL_USBPLL_PD);
    writel(tmpVal, PDS_BASE + PDS_CTL_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_INT_OFFSET);
    tmpVal &= ~(0x3<<17);
    writel(tmpVal, PDS_BASE + PDS_INT_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_CTL3_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_USB_ISO_EN);
    writel(tmpVal, PDS_BASE + PDS_CTL3_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_CTL4_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_USB_PWR_OFF);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_USB_RESET);
    writel(tmpVal, PDS_BASE + PDS_CTL4_OFFSET);

    tmpVal = readl(PDS_BASE + PDS_CTL5_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PDS_CR_PDS_CTRL_USB33);
    writel(tmpVal, PDS_BASE + PDS_CTL5_OFFSET);

    return SUCCESS;
}

