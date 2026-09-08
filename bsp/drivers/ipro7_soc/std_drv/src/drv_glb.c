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
#include "drv_common.h"
#include "drv_hbn.h"
#include "drv_ef_cfg.h"
#include "glb_reg.h"

#define GLB_CLK_SET_DUMMY_WAIT \
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
#define GLB_REG_BCLK_DIS_TRUE
#define GLB_REG_BCLK_DIS_FALSE

GLB_ROOT_CLK_Type ATTR_CLOCK_SECTION GLB_Get_Root_CLK_Sel(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG0_OFFSET);

    switch (reg_get_bits(tmpVal, GLB_HBN_ROOT_CLK_SEL)) {
        case 0:
            return GLB_ROOT_CLK_RC32M;
        case 1:
            return GLB_ROOT_CLK_XTAL;
        case 2:
        case 3:
            return GLB_ROOT_CLK_DLL;
        default:
            return GLB_ROOT_CLK_RC32M;
    }
}

int ATTR_CLOCK_SECTION GLB_Set_System_CLK_Div(uint8_t hclkDiv, uint8_t bclkDiv)
{
    /***********************************************************************************/
    /*                                 NOTE                                            */
    /* "GLB_REG_BCLK_DIS_TRUE + GLB_REG_BCLK_DIS_FALSE" will stop bclk a little while. */
    /* OCRAM use bclk as source clock. Pay attention to risks when using this API.     */
    /***********************************************************************************/
    uint32_t tmpVal;
    uint32_t timeout = 1024;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG1_OFFSET);
    if ((hclkDiv == reg_get_bits(tmpVal, GLB_REG_HCLK_DIV)) && (bclkDiv == reg_get_bits(tmpVal, GLB_REG_BCLK_DIV)))
        return SUCCESS;

    tmpVal = reg_set_bits(tmpVal, GLB_REG_HCLK_DIV, hclkDiv);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV, bclkDiv);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG1_OFFSET);

     /* bclk act pulse */
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV_ACT_PULSE, 1);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG0_OFFSET);

    timeout = 1024;
    do {
        tmpVal = readl(GLB_BASE + GLB_CLK_CFG0_OFFSET);
        tmpVal = reg_get_bits(tmpVal, GLB_STS_BCLK_PROT_DONE);
    } while ((--timeout) && (!tmpVal));
    if (!timeout) {
        return ERROR;
    }

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

uint8_t ATTR_CLOCK_SECTION GLB_Get_BCLK_Div(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG1_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_BCLK_DIV);
}

uint8_t ATTR_CLOCK_SECTION GLB_Get_HCLK_Div(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG1_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_HCLK_DIV);
}

int ATTR_CLOCK_SECTION GLB_Switch_System_CLK(GLB_DLL_XTAL_Type xtalType,GLB_SYS_CLK_Type clkFreq, uint8_t hclkDiv,uint8_t bclkDiv)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;
    uint32_t timeout;

    /* make sure root clk is from XCLK */
    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal,HBN_ROOT_CLK_SEL);
    tmpVal2 &= 0x1;
    tmpVal = reg_set_bits(tmpVal,HBN_ROOT_CLK_SEL,tmpVal2);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    GLB_CLK_SET_DUMMY_WAIT;
    
    /* Before config XTAL and DLL ,make sure root clk is from RC32M */
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG1_OFFSET);
    tmpVal=reg_set_bits(tmpVal,GLB_REG_HCLK_DIV,hclkDiv);
    tmpVal=reg_set_bits(tmpVal,GLB_REG_BCLK_DIV,bclkDiv);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG1_OFFSET);

     /* bclk act pulse */
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV_ACT_PULSE, 1);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG0_OFFSET);

    timeout = 1024;
    do {
        tmpVal = readl(GLB_BASE + GLB_CLK_CFG0_OFFSET);
        tmpVal = reg_get_bits(tmpVal, GLB_STS_BCLK_PROT_DONE);
    } while ((--timeout) && (!tmpVal));
    if (!timeout) {
        return ERROR;
    }


    GLB_CLK_SET_DUMMY_WAIT;
    
    /* select pll output clock before select root clock */
    if(clkFreq>=GLB_SYS_CLK_DLL25P6M){
        tmpVal = readl(GLB_BASE + GLB_CLK_CFG0_OFFSET);
        tmpVal = reg_set_bits(tmpVal,GLB_REG_PLL_SEL,clkFreq-GLB_SYS_CLK_DLL25P6M);
        writel(tmpVal, GLB_BASE + GLB_CLK_CFG0_OFFSET);
    }

    /* select root clock */
    switch(clkFreq){
        case GLB_SYS_CLK_RC32M:
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_RC32M);
            break;
        case GLB_SYS_CLK_XTAL:
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_XTAL);
            break;
        case GLB_SYS_CLK_PLL160M:
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_DLL);
            break;
        case GLB_SYS_CLK_PLL192M:
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_DLL);
            break;
        case GLB_SYS_CLK_DLL25P6M:
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_DLL);
            break;
        case GLB_SYS_CLK_DLL128M:
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_DLL);
            break;
        default :
            break;
    }
    
    GLB_CLK_SET_DUMMY_WAIT;
    
    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Set_System_CLK_Ex(GLB_DLL_XTAL_Type xtalType, GLB_SYS_CLK_Type clkFreq,
                                             uint8_t pllOn, uint8_t dllOn)
{
    uint32_t tmpVal;

    if (xtalType == GLB_DLL_XTAL_32M)
        HBN_Set_ROOT_CLK(HBN_ROOT_CLK_XTAL);
    else
        HBN_Set_ROOT_CLK(HBN_ROOT_CLK_RC32M);

    GLB_Set_System_CLK_Div(0, 0);
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_XCLK, 15); // FCLK = 32M by default, change rtc to 2M (4T=1us)

    if (xtalType == GLB_DLL_XTAL_NONE) {
        if (clkFreq == GLB_SYS_CLK_RC32M) {
            return SUCCESS;
        } else {
            return ERROR;
        }
    }

    /* make PLL Setting out of RF, so following setting can be removed*/
    //AON_Power_On_MBG();
    //AON_Power_On_LDO15_RF();

    /* PLL power: caller decides.  On → power up and enable clock outputs.
     * Off → power down.  Caller must ensure clkFreq does not depend on PLL
     * when pllOn == 0. */
    if (pllOn) {
        GLB_Power_On_PLL((GLB_PLL_XTAL_Type)xtalType);
        //arch_delay_us(55);
        GLB_Enable_PLL_All_Clks();
    } else {
        GLB_Power_Off_PLL();
    }

    /* DLL power: caller decides.  On → reset and re-enable clock outputs.
     * Off → power down.  Caller must ensure clkFreq does not depend on DLL
     * when dllOn == 0. */
    if (dllOn) {
        GLB_Power_Off_DLL();
        GLB_Power_On_DLL(xtalType);
        GLB_Enable_DLL_All_Clks();
    } else {
        GLB_Power_Off_DLL();
    }

    /* reg_pll_en = 1, cannot be zero */
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_PLL_EN);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG0_OFFSET);

    /* select pll output clock before select root clock */
    if (clkFreq >= GLB_SYS_CLK_DLL25P6M) {
        tmpVal = readl(GLB_BASE + GLB_CLK_CFG0_OFFSET);
        tmpVal = reg_set_bits(tmpVal, GLB_REG_PLL_SEL, clkFreq - GLB_SYS_CLK_DLL25P6M);
        writel(tmpVal, GLB_BASE + GLB_CLK_CFG0_OFFSET);
    }

    /* select root clock */
    switch (clkFreq) {
        case GLB_SYS_CLK_RC32M:
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_RC32M);
            break;
        case GLB_SYS_CLK_XTAL:
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_XTAL);
            break;
        case GLB_SYS_CLK_DLL25P6M:
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_DLL);
            break;
        case GLB_SYS_CLK_PLL160M:
            GLB_Set_System_CLK_Div(0, 1);
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_DLL);
            break;
        case GLB_SYS_CLK_PLL192M:
            GLB_Set_System_CLK_Div(0, 1);
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_DLL);
            break;
        case GLB_SYS_CLK_DLL128M:
            GLB_Set_System_CLK_Div(0, 1);
            HBN_Set_ROOT_CLK(HBN_ROOT_CLK_DLL);
            break;
        default:
            break;
    }

    GLB_CLK_SET_DUMMY_WAIT;


    return SUCCESS;
}

/* Back-compat wrapper: legacy behavior always powered both PLL and DLL on. */
int ATTR_CLOCK_SECTION GLB_Set_System_CLK(GLB_DLL_XTAL_Type xtalType, GLB_SYS_CLK_Type clkFreq)
{
    return GLB_Set_System_CLK_Ex(xtalType, clkFreq, 1, 1);
}

int ATTR_CLOCK_SECTION System_Core_Clock_Update_From_RC32M(void)
{
    SF_Ctrl_Cfg_Type sfCtrlCfg = {
        .owner = SF_CTRL_OWNER_IAHB,
        .clkDelay = 1,
        .clkInvert = 1,
        .rxClkInvert = 1,
        .doDelay = 0,
        .diDelay = 0,
        .oeDelay = 0,
    };
    /* Use RC32M as DLL ref source to set up DLL to 144M */
    GLB_Set_System_CLK(GLB_DLL_XTAL_RC32M, GLB_SYS_CLK_DLL128M);
    /* Flash controller also need changes since system (bus) clock changed */
    SF_Ctrl_Enable(&sfCtrlCfg);
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Enable_DLL_All_Clks(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_DLL2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV1_RF, 1);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV63, 1);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV21, 1);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV5, 1);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV3, 1);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV2, 1);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV1, 1);
    writel(tmpVal, CCI_BASE + CCI_DLL2_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Enable_DLL_Clk(GLB_DLL_CLK_Type dllClk)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_DLL2_OFFSET);
    switch (dllClk) {
        case GLB_DLL_CLK_RF:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV1_RF, 1);
            break;
        case GLB_DLL_CLK_2P032M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV63, 1);
            break;
        case GLB_DLL_CLK_6P095M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV21, 1);
            break;
        case GLB_DLL_CLK_25P6M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV5, 1);
            break;
        case GLB_DLL_CLK_42P67M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV3, 1);
            break;
        case GLB_DLL_CLK_64M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV2, 1);
            break;
        case GLB_DLL_CLK_128M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV1, 1);
            break;
        default:
            break;
    }
    writel(tmpVal, CCI_BASE + CCI_DLL2_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Disable_DLL_All_Clks(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_DLL2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV1_RF, 0);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV63, 0);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV21, 0);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV5, 0);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV3, 0);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV2, 0);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV1, 0);
    writel(tmpVal, CCI_BASE + CCI_DLL2_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Disable_DLL_Clk(GLB_DLL_CLK_Type dllClk)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_DLL2_OFFSET);
    switch (dllClk) {
        case GLB_DLL_CLK_RF:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV1_RF, 0);
            break;
        case GLB_DLL_CLK_2P032M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV63, 0);
            break;
        case GLB_DLL_CLK_6P095M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV21, 0);
            break;
        case GLB_DLL_CLK_25P6M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV5, 0);
            break;
        case GLB_DLL_CLK_42P67M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV3, 0);
            break;
        case GLB_DLL_CLK_64M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV2, 0);
            break;
        case GLB_DLL_CLK_128M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_EN_DIV1, 0);
            break;

        default:
            break;
    }
    writel(tmpVal, CCI_BASE + CCI_DLL2_OFFSET);

    return SUCCESS;
}

int GLB_Set_CAM_CLK(uint8_t enable, GLB_CAM_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_CAM_REF_CLK_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_CAM_REF_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG5_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_CAM_REF_CLK_DIV_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_CAM_REF_CLK_DIV_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG5_OFFSET);

    return SUCCESS;
}

int GLB_Set_SDH_CLK(uint8_t enable, GLB_SDH_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_SDH_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_SDH_CLK_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_SDH_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_SDH_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_SDH_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_SDH_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_SDH_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_SDH_CFG0_OFFSET);

    return SUCCESS;
}


int GLB_Set_MAC154_ZIGBEE_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_M154_ZBEN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_M154_ZBEN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_BLE_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_BLE_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_BLE_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_I2S_CLK(uint8_t enable, GLB_I2S_OUT_REF_CLK_Type outRef)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_I2S_0_REF_CLK_OE, outRef);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG2_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG6_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_I2S_REF_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_I2S_REF_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG6_OFFSET);
    return SUCCESS;
}

int GLB_Set_I2S_CLK_Sel(GLB_I2S_REF_CLK_SRC_Type clkSel, uint8_t div)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG6_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_I2S_REF_CLK_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_I2S_REF_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG6_OFFSET);
    return SUCCESS;
}

int GLB_Set_USB_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_PLL_48M_DIV_EN, 1);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG2_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_USB_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_USB_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_QDEC_CLK(GLB_QDEC_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CLK_1M_CLK_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_CLK_1M_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_DMA_CLK(uint8_t enable, GLB_DMA_CLK_ID_Type clk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, GLB_DMA_CLK_EN);
    if (enable) {
        tmpVal2 |= (1 << clk);
    } else {
        tmpVal2 &= (~(1 << clk));
    }
    tmpVal = reg_set_bits(tmpVal, GLB_DMA_CLK_EN, tmpVal2);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG3_OFFSET);

    return SUCCESS;
}

int GLB_Set_IR_CLK(uint8_t enable, GLB_IR_CLK_SRC_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_IR_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG3_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_IR_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_IR_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG3_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Set_SF_CLK(uint8_t enable, GLB_SFLASH_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    /* disable SFLASH clock first */
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_SF_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG3_OFFSET);

    /* clock divider */
    /* Select flash clock, all Flash CLKs are divied by DLL_288M */
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_DIV, div);
    switch (clkSel) {
        case GLB_SFLASH_CLK_XCLK:
            tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_SEL, 0x0);
            break;
        case GLB_SFLASH_CLK_64M:
            tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_SEL, 0x1);
            break;
        case GLB_SFLASH_CLK_BCLK:
            tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_SEL, 0x2);
            break;
        case GLB_SFLASH_CLK_80M:
            tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_SEL, 0x3);
            break;
        default:
            break;
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG3_OFFSET);

    /* enable or disable flash clock */
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_SF_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_SF_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG3_OFFSET);

    return SUCCESS;
}

int GLB_Set_UART_CLK(uint8_t enable, HBN_UART_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    /* disable UART clock first */
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_UART_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG3_OFFSET);

    /* Set div */
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_UART_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG3_OFFSET);

    /* Select clock source for uart */
    HBN_Set_UART_CLK_Sel(clkSel);

    /* Set enable or disable */
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_UART_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_UART_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG3_OFFSET);

    return SUCCESS;
}

int GLB_Set_Chip_Out_0_CLK_Sel(GLB_CHIP_CLK_OUT_Type clkSel)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_0_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG4_OFFSET);

    return SUCCESS;
}

int GLB_Set_Chip_Out_1_CLK_Sel(GLB_CHIP_CLK_OUT_Type clkSel)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_1_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG4_OFFSET);

    return SUCCESS;
}

int GLB_Set_Chip_Out_2_CLK_Sel(GLB_CHIP_CLK_OUT_Type clkSel)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_2_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG4_OFFSET);

    return SUCCESS;
}

int GLB_Set_I2C_CLK(uint8_t enable, GLB_I2C_CLK_SRC_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_I2C_CLK_DIV, div);
    tmpVal = reg_set_bits(tmpVal, GLB_I2C_CLK_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG4_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG4_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_I2C_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_I2C_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG4_OFFSET);

    return SUCCESS;
}

int GLB_Invert_ETH_RX_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_CFG_INV_ETH_RX_CLK);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_CFG_INV_ETH_RX_CLK);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG5_OFFSET);

    return SUCCESS;
}

int GLB_Invert_RF_TEST_O_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_CFG_INV_RF_TEST_CLK_O);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_CFG_INV_RF_TEST_CLK_O);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG3_OFFSET);
    return SUCCESS;
}

int GLB_Set_SPI_CLK(uint8_t enable, GLB_SPI_CLK_Type sel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_SPI_CLK_SEL, sel);
    tmpVal = reg_set_bits(tmpVal, GLB_SPI_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG4_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG4_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_SPI_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_SPI_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG4_OFFSET);

    return SUCCESS;
}

/* SPI1 input clock has its OWN divider in CLK_CFG5.spi2_clk_* (the
 * register field is named "spi2" in the SVD but it controls SPI1).
 * Default reset value is div=3 (HW=4), sel=BCLK -- so SPI1 input runs
 * at BCLK/4 unless someone writes this register. drv_spi.c's
 * SPI_SetClock used to only write CFG4 (SPI0 only), leaving SPI1 at
 * the /4 default and producing 6 MHz SCLK instead of the 24 MHz
 * implied by a "40 MHz" caller request on a 96 MHz BCLK system. */
int GLB_Set_SPI_2_CLK(uint8_t enable, GLB_SPI_CLK_Type sel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_SPI2_CLK_SEL, sel);
    tmpVal = reg_set_bits(tmpVal, GLB_SPI2_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG5_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_SPI2_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_SPI2_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG5_OFFSET);

    return SUCCESS;
}

int GLB_Invert_ETH_TX_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_CFG_INV_ETH_TX_CLK);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_CFG_INV_ETH_TX_CLK);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG5_OFFSET);
    return SUCCESS;
}

int GLB_Invert_ETH_REF_O_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_CFG_INV_ETH_REF_CLK_O);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_CFG_INV_ETH_REF_CLK_O);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG5_OFFSET);

    return SUCCESS;
}

int GLB_Set_ETH_REF_O_CLK_Sel(GLB_ETH_REF_CLK_OUT_Type clkSel)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CFG_SEL_ETH_REF_CLK_O, clkSel);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG5_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_Type clkSel)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_PKA_CLK_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_SW_System_Reset(void)
{
    /***********************************************************************************/
    /*                                 NOTE                                            */
    /* "GLB_REG_BCLK_DIS_TRUE + GLB_REG_BCLK_DIS_FALSE" will stop bclk a little while. */
    /* OCRAM use bclk as source clock. Pay attention to risks when using this API.     */
    /***********************************************************************************/
    uint32_t tmpVal;

    /* Swicth clock to 32M as default */
    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_ROOT_CLK_SEL, 0);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    GLB_CLK_SET_DUMMY_WAIT;

    /* HCLK is RC32M , so BCLK/HCLK no need divider */
    GLB_Set_System_CLK_Div(0, 0);

    /* Do reset */
    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_CPU_RESET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_PWRON_RST);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_CTRL_CPU_RESET);
    //tmpVal=reg_clr_bit(tmpVal,GLB_REG_CTRL_PWRON_RST);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    /* waiting for reset */
    while (1) {
        arch_delay_us(10);
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_SW_CPU_Reset(void)
{
    /***********************************************************************************/
    /*                                 NOTE                                            */
    /* "GLB_REG_BCLK_DIS_TRUE + GLB_REG_BCLK_DIS_FALSE" will stop bclk a little while. */
    /* OCRAM use bclk as source clock. Pay attention to risks when using this API.     */
    /***********************************************************************************/
    uint32_t tmpVal;

    /* Swicth clock to 32M as default */
    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_ROOT_CLK_SEL, 0);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    GLB_CLK_SET_DUMMY_WAIT;

    /* HCLK is RC32M , so BCLK/HCLK no need divider */
    GLB_Set_System_CLK_Div(0, 0);

    /* Do reset */
    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_CPU_RESET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_PWRON_RST);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    //tmpVal=reg_clr_bit(tmpVal,GLB_REG_CTRL_SYS_RESET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_CTRL_CPU_RESET);
    //tmpVal=reg_clr_bit(tmpVal,GLB_REG_CTRL_PWRON_RST);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    /* waiting for reset */
    while (1) {
        arch_delay_us(10);
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_SW_POR_Reset(void)
{
    /***********************************************************************************/
    /*                                 NOTE                                            */
    /* "GLB_REG_BCLK_DIS_TRUE + GLB_REG_BCLK_DIS_FALSE" will stop bclk a little while. */
    /* OCRAM use bclk as source clock. Pay attention to risks when using this API.     */
    /***********************************************************************************/
    uint32_t tmpVal;

    /* Swicth clock to 32M as default */
    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_ROOT_CLK_SEL, 0);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    GLB_CLK_SET_DUMMY_WAIT;

    /* HCLK is RC32M , so BCLK/HCLK no need divider */
    GLB_Set_System_CLK_Div(0, 0);

    /* Do reset */
    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_CPU_RESET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_PWRON_RST);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_CTRL_CPU_RESET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_CTRL_PWRON_RST);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    /* waiting for reset */
    while (1) {
        arch_delay_us(10);
    }

    return SUCCESS;
}

int GLB_AHB_MCU_Software_Reset(DRV_AHB_MCU_SW_Type swrst)
{
    uint32_t tmpVal = 0;
    uint32_t bit = 0;
    uint32_t regAddr = 0;

    if (swrst == GLB_AHB_MCU_SW_PWRON_RST) {
        return GLB_SW_POR_Reset();
    } else if (swrst == GLB_AHB_MCU_SW_CPU_RESET) {
        return GLB_SW_CPU_Reset();
    } else if (swrst == GLB_AHB_MCU_SW_SYS_RESET) {
        return GLB_SW_System_Reset();
    }

    if (swrst < 32) {
        bit = swrst;
        regAddr = GLB_BASE + GLB_SWRST_CFG0_OFFSET;
    } else if (swrst < 64) {
        bit = swrst - 32;
        regAddr = GLB_BASE + GLB_SWRST_CFG1_OFFSET;
    } else if (swrst < 96) {
        bit = swrst - 64;
        regAddr = GLB_BASE + GLB_SWRST_CFG2_OFFSET;
    }

    tmpVal = readl(regAddr);
    tmpVal &= ~(1 << bit);
    writel(tmpVal, regAddr);
    __NOP();
    __NOP();
    __NOP();
    tmpVal = readl(regAddr);
    tmpVal |= (1 << bit);
    writel(tmpVal, regAddr);
    __NOP();
    __NOP();
    __NOP();
    tmpVal = readl(regAddr);
    tmpVal &= ~(1 << bit);
    writel(tmpVal, regAddr);

    return SUCCESS;
}

int GLB_PER_Clock_Gate(GLB_AHB_CLOCK_Type module)
{
    uint32_t tmpVal = 0;
    uint32_t bit = 0;
    uint32_t regAddr = 0;

    if (module == GLB_AHB_CLOCK_SEC) {
        tmpVal = readl(regAddr);
        tmpVal &= ~(1 << GLB_AHB_CLOCK_SEC);
        tmpVal &= ~(1 << GLB_AHB_CLOCK_SEC_DBG);
        tmpVal &= ~(1 << GLB_AHB_CLOCK_SEC_ENG);
        writel(tmpVal, regAddr);        
    } else {
        if (module < 32) {
            bit = module;
            regAddr = GLB_BASE + GLB_CGEN_CFG0_OFFSET;
        } else if (module < 64) {
            bit = module - 32;
            regAddr = GLB_BASE + GLB_CGEN_CFG1_OFFSET;
        } else if (module < 96) {
            bit = module - 64;
            regAddr = GLB_BASE + GLB_CGEN_CFG2_OFFSET;
        }
        
        tmpVal = readl(regAddr);
        tmpVal &= ~(1 << bit);
        writel(tmpVal, regAddr);
    }
    return SUCCESS;
}

int GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_Type module)
{
    uint32_t tmpVal = 0;
    uint32_t bit = 0;
    uint32_t regAddr = 0;

    if (module == GLB_AHB_CLOCK_SEC) {
        tmpVal = readl(regAddr);
        tmpVal |= (1 << GLB_AHB_CLOCK_SEC);
        tmpVal |= (1 << GLB_AHB_CLOCK_SEC_DBG);
        tmpVal |= (1 << GLB_AHB_CLOCK_SEC_ENG);
        writel(tmpVal, regAddr);        
    } else {
        if (module < 32) {
            bit = module;
            regAddr = GLB_BASE + GLB_CGEN_CFG0_OFFSET;
        } else if (module < 64) {
            bit = module - 32;
            regAddr = GLB_BASE + GLB_CGEN_CFG1_OFFSET;
        } else if (module < 96) {
            bit = module - 64;
            regAddr = GLB_BASE + GLB_CGEN_CFG2_OFFSET;
        }
        
        tmpVal = readl(regAddr);
        tmpVal |= (1 << bit);
        writel(tmpVal, regAddr);
    }
    return SUCCESS;
}

int GLB_Set_EM_Sel(GLB_EM_Type emType)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_SRAM_MISC_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CR_EM_SEL, emType);
    writel(tmpVal, GLB_BASE + GLB_SRAM_MISC_OFFSET);

    return SUCCESS;
}

int GLB_Set_IPROAISram(int en)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_SRAM_MISC_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CR_IPROAI_EN, en);
    writel(tmpVal, GLB_BASE + GLB_SRAM_MISC_OFFSET);

    return SUCCESS;
}

int GLB_Set_Kys_Drv_Col(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_PARM_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_KYS_DRV_VAL);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_KYS_DRV_VAL);
    }
    writel(tmpVal, GLB_BASE + GLB_PARM_OFFSET);

    return SUCCESS;
}

int GLB_JTAG_Sig_Swap_Set(uint8_t swapSel)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_PARM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_JTAG_SWAP_SET, swapSel);
    writel(tmpVal, GLB_BASE + GLB_PARM_OFFSET);

    return SUCCESS;
}

int GLB_CCI_Use_IO_0_1_2_7(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_PARM_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_P3_CCI_USE_IO_0_2_7);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_P3_CCI_USE_IO_0_2_7);
    }
    writel(tmpVal, GLB_BASE + GLB_PARM_OFFSET);

    return SUCCESS;
}

int GLB_CCI_Use_Jtag_Pin(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_PARM_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_CCI_USE_JTAG_PIN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_CCI_USE_JTAG_PIN);
    }
    writel(tmpVal, GLB_BASE + GLB_PARM_OFFSET);

    return SUCCESS;
}

int GLB_Swap_SPI_0_MOSI_With_MISO(int newState)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_PARM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_SPI_0_SWAP, newState);
    writel(tmpVal, GLB_BASE + GLB_PARM_OFFSET);

    return SUCCESS;
}

int GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_Type mod)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_PARM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_SPI_0_MASTER_MODE, mod);
    writel(tmpVal, GLB_BASE + GLB_PARM_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_Set_Embedded_FLash_IO_PARM(uint8_t reverse, uint8_t swapIo3Io0, uint8_t swapIo2Cs)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_PARM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CFG_SFLASH_REVERSE, reverse);
    tmpVal = reg_set_bits(tmpVal, GLB_CFG_SFLASH_SWAP_IO0_IO3, swapIo3Io0);
    tmpVal = reg_set_bits(tmpVal, GLB_CFG_SFLASH_SWAP_CS_IO2, swapIo2Cs);
    writel(tmpVal, GLB_BASE + GLB_PARM_OFFSET);
    return SUCCESS;
}


int ATTR_CLOCK_SECTION GLB_Set_MTimer_CLK(uint8_t enable, GLB_MTIMER_CLK_Type clkSel, uint32_t div)
{
    uint32_t tmpVal;

    /* disable MTimer clock first */
    tmpVal = readl(GLB_BASE + GLB_CPU_CLK_CFG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_CPU_RTC_EN);
    writel(tmpVal, GLB_BASE + GLB_CPU_CLK_CFG_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CPU_CLK_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CPU_RTC_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_CPU_RTC_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_CPU_CLK_CFG_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CPU_CLK_CFG_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_CPU_RTC_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_CPU_RTC_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CPU_CLK_CFG_OFFSET);

    return SUCCESS;
}

int GLB_Set_ADC_CLK(uint8_t enable, GLB_ADC_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal;

    /* disable ADC clock first */
    tmpVal = readl(GLB_BASE + GLB_GPADC_32M_SRC_CTRL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_GPADC_32M_DIV_EN);
    writel(tmpVal, GLB_BASE + GLB_GPADC_32M_SRC_CTRL_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_GPADC_32M_SRC_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_GPADC_32M_CLK_DIV, div);
    tmpVal = reg_set_bits(tmpVal, GLB_GPADC_32M_CLK_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_GPADC_32M_SRC_CTRL_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_GPADC_32M_SRC_CTRL_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_GPADC_32M_DIV_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_GPADC_32M_DIV_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_GPADC_32M_SRC_CTRL_OFFSET);

    return SUCCESS;
}

int GLB_Set_AUDIO_CLK(uint8_t clkDivEn, uint8_t autoDivEn, GLB_AUDIO_CLK_SRC_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_AUDIO_CLK_DIV, div);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_AUDIO_CLK_SRC_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG2_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    if (clkDivEn) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_AUDIO_CLK_DIV_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_CLK_DIV_EN);
    }
    if (autoDivEn) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_GPDAC_CLK_Sel(GLB_DAC_CLK_Type clkSel, int div, int en)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_DIG_512K_EN);
    writel(tmpVal, GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_DIG_CLK_SRC_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_DIG_512K_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);
    if (en) {
        tmpVal = reg_set_bit(tmpVal, GLB_DIG_512K_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_DIG_512K_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);

    return SUCCESS;
}

int GLB_Set_DIG_CLK_Sel(GLB_DIG_CLK_Type clkSel)
{
    uint32_t tmpVal;
    uint32_t dig32kEn;

    /* disable DIG512K and DIG32K clock first */
    tmpVal = readl(GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);
    dig32kEn = reg_get_bits(tmpVal, GLB_DIG_32K_EN);
    tmpVal = reg_clr_bit(tmpVal, GLB_DIG_32K_EN);
    writel(tmpVal, GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_DIG_CLK_SRC_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);

    /* repristinate DIG512K and DIG32K clock */
    tmpVal = readl(GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_DIG_32K_EN, dig32kEn);
    writel(tmpVal, GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);

    return SUCCESS;
}

int GLB_Set_DIG_32K_CLK(uint8_t enable, uint8_t compensation, uint16_t div)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);
    if (compensation) {
        tmpVal = reg_set_bit(tmpVal, GLB_DIG_32K_COMP);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_DIG_32K_COMP);
    }
    tmpVal = reg_set_bits(tmpVal, GLB_DIG_32K_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_DIG_32K_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_DIG_32K_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_Set_PSRAMB_CLK_Sel(uint8_t enable, GLB_PSRAMB_PLL_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_PSRAM_CLK_DIV_EN);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG5_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_PSRAM_CLK_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_PSRAM_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG5_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_PSRAM_CLK_DIV_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_PSRAM_CLK_DIV_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CLK_CFG5_OFFSET);

    return SUCCESS;
}

int GLB_UART_Fun_Sel(GLB_UART_SIG_Type sig, GLB_UART_SIG_FUN_Type fun)
{
    /*
     * Swap semantics with uniqueness invariant:
     *   after this call, exactly ONE sig carries `fun`, and that sig is
     *   the one the caller asked for. Any other sig currently holding
     *   `fun` is restored to its identity value (slot N := N) so that
     *   stale duplicates left over from prior configurations (the
     *   SIG_SEL registers survive CPU reset on IPRO7) don't silently
     *   keep routing the function to a second pin.
     *
     *   The classic "swap with first match" behaviour is preserved for
     *   the first duplicate — it gets the caller's old function so
     *   well-formed single-swap callers see no observable change.
     */
    uint32_t regVal0 = readl(GLB_BASE + GLB_UART_SIG_SEL_0_OFFSET);
    uint32_t regVal1 = readl(GLB_BASE + GLB_UART_SIG_SEL_1_OFFSET);
    uint32_t oldFun;
    int firstOwner = -1;

    /* Snapshot the target sig's current function so we can gift it to
     * the first duplicate owner during the swap. */
    if (sig < GLB_UART_SIG_8) {
        oldFun = (regVal0 >> (sig * 4)) & 0xf;
    } else {
        oldFun = (regVal1 >> ((sig - GLB_UART_SIG_8) * 4)) & 0xf;
    }

    /* Walk every other sig; clear any that currently equal `fun`.
     * The first one seen takes `oldFun` (traditional swap).
     * Subsequent ones are restored to their identity value. */
    for (int i = 0; i < GLB_UART_SIG_8; i++) {
        if (i == sig) continue;
        if (((regVal0 >> (i * 4)) & 0xf) == fun) {
            uint32_t newVal = (firstOwner < 0) ? oldFun : (uint32_t)i;
            regVal0 = (regVal0 & ~(0xfu << (i * 4))) | (newVal << (i * 4));
            if (firstOwner < 0) firstOwner = i;
        }
    }
    for (int i = 0; i < 4; i++) {
        int s = GLB_UART_SIG_8 + i;
        if (s == sig) continue;
        if (((regVal1 >> (i * 4)) & 0xf) == fun) {
            uint32_t newVal = (firstOwner < 0) ? oldFun : (uint32_t)s;
            regVal1 = (regVal1 & ~(0xfu << (i * 4))) | (newVal << (i * 4));
            if (firstOwner < 0) firstOwner = s;
        }
    }

    /* Assign fun to the requested sig. */
    if (sig < GLB_UART_SIG_8) {
        regVal0 = (regVal0 & ~(0xfu << (sig * 4))) | ((uint32_t)fun << (sig * 4));
    } else {
        int i = sig - GLB_UART_SIG_8;
        regVal1 = (regVal1 & ~(0xfu << (i * 4))) | ((uint32_t)fun << (i * 4));
    }

    writel(regVal0, GLB_BASE + GLB_UART_SIG_SEL_0_OFFSET);
    writel(regVal1, GLB_BASE + GLB_UART_SIG_SEL_1_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Power_Off_DLL(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_DLL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_PPU_DLL, 0);
    tmpVal = reg_set_bits(tmpVal, CCI_PU_DLL, 0);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_RESET, 1);
    writel(tmpVal, CCI_BASE + CCI_DLL_OFFSET);

    return SUCCESS;
}

/* External declaration for mtimer_get_time_ms */
extern uint32_t mtimer_get_time_ms(void);

int ATTR_CLOCK_SECTION GLB_Power_On_DLL(GLB_DLL_XTAL_Type xtalType)
{
    uint32_t tmpVal = 0;

    /* GLB->dll.BF.dll_refclk_sel = XXX; */
    tmpVal = readl(CCI_BASE + CCI_DLL_OFFSET);
    switch (xtalType) {
        case GLB_DLL_XTAL_NONE:
            return ERROR;
        case GLB_DLL_XTAL_32M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_REFCLK_SEL, 0);
            break;
        case GLB_DLL_XTAL_RC32M:
            tmpVal = reg_set_bits(tmpVal, CCI_DLL_REFCLK_SEL, 1);
            break;
        default:
            break;
    }
    writel(tmpVal, CCI_BASE + CCI_DLL_OFFSET);

    /* GLB->dll.BF.dll_prechg_sel = 1;   */
    tmpVal = readl(CCI_BASE + CCI_DLL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_PRECHG_SEL, 1);
    writel(tmpVal, CCI_BASE + CCI_DLL_OFFSET);

    /* GLB->dll.BF.ppu_dll = 1;   */
    tmpVal = readl(CCI_BASE + CCI_DLL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_PPU_DLL, 1);
    writel(tmpVal, CCI_BASE + CCI_DLL_OFFSET);

    arch_delay_us(2);

    /* GLB->dll.BF.pu_dll = 1; */
    tmpVal = readl(CCI_BASE + CCI_DLL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_PU_DLL, 1);
    writel(tmpVal, CCI_BASE + CCI_DLL_OFFSET);

    arch_delay_us(2);

    /* GLB->dll.BF.dll_reset = 0; */
    tmpVal = readl(CCI_BASE + CCI_DLL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_DLL_RESET, 0);
    writel(tmpVal, CCI_BASE + CCI_DLL_OFFSET);

    /* delay for settling */
    arch_delay_us(5);

    return SUCCESS;
}


int ATTR_CLOCK_SECTION GLB_Select_RC32M_As_PLL_Ref(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_TOP_CTRL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CCI_CLKPLL_XTAL_RC32M_SEL);
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_TOP_CTRL_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Select_XTAL_As_PLL_Ref(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_TOP_CTRL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, CCI_CLKPLL_XTAL_RC32M_SEL);
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_TOP_CTRL_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Power_On_PLL_With_Freq(GLB_PLL_XTAL_Type xtalType, GLB_PLL_FREQ_Type freq)
{
    uint32_t tmpVal = 0;
    uint32_t sdmin;
    uint32_t postdiv;

    switch (freq) {
    case GLB_PLL_FREQ_400M:
        sdmin = 0x640000;
        postdiv = 0x10;
        break;
    case GLB_PLL_FREQ_480M:
    default:
        sdmin = 0x780000;
        postdiv = 0x14;
        break;
    }

    /* select PLL XTAL source */
    if ((xtalType == GLB_PLL_XTAL_RC32M) || (xtalType == GLB_PLL_XTAL_NONE)) {
        GLB_Trim_RC32M();
        GLB_Select_RC32M_As_PLL_Ref();
    } else {
        GLB_Select_XTAL_As_PLL_Ref();
    }

    /* PLL power down first, not indispensable */
    GLB_Power_Off_PLL();

    /* PLL param config */
    tmpVal = readl(CCI_BASE + CCI_CLKPLL_CP_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_ICP_1U, 0);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_ICP_5U, 2);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_INT_FRAC_SW, 0);
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_CP_OFFSET);

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_RZ_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_C3, 3);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_CZ, 1);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_RZ, 1);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_R4_SHORT, 1);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_R4, 2);
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_RZ_OFFSET);

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_TOP_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_POSTDIV, postdiv);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_REFDIV_RATIO, 4);
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_TOP_CTRL_OFFSET);

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_SDM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_SDMIN, sdmin);
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_SDM_OFFSET);

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_FBDV_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_SEL_FB_CLK, 1);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_SEL_SAMPLE_CLK, 1);
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_FBDV_OFFSET);

    /* PLL power up sequence */
    tmpVal = readl(CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CCI_PU_CLKPLL_SFREG);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);

    arch_delay_us(5);

    tmpVal = readl(CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CCI_PU_CLKPLL);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);

    tmpVal = readl(CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CCI_CLKPLL_PU_CP);
    tmpVal = reg_set_bit(tmpVal, CCI_CLKPLL_PU_PFD);
    tmpVal = reg_set_bit(tmpVal, CCI_CLKPLL_PU_FBDV);
    tmpVal = reg_set_bit(tmpVal, CCI_CLKPLL_PU_POSTDIV);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);

    arch_delay_us(5);

    tmpVal = readl(CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CCI_CLKPLL_SDM_RESET);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    arch_delay_us(1);
    tmpVal = readl(CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CCI_CLKPLL_RESET_FBDV);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    arch_delay_us(2);
    tmpVal = readl(CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, CCI_CLKPLL_RESET_FBDV);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    arch_delay_us(1);
    tmpVal = readl(CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, CCI_CLKPLL_SDM_RESET);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Power_On_PLL(GLB_PLL_XTAL_Type xtalType)
{
    return GLB_Power_On_PLL_With_Freq(xtalType, GLB_PLL_FREQ_480M);
}

int ATTR_CLOCK_SECTION GLB_Enable_PLL_All_Clks(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_OUTPUT_EN_OFFSET);
    tmpVal |= 0x1FF;
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_OUTPUT_EN_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Disable_PLL_All_Clks(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_OUTPUT_EN_OFFSET);
    tmpVal &= (~0x1FF);
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_OUTPUT_EN_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Enable_PLL_Clk(GLB_PLL_CLK_Type pllClk)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_OUTPUT_EN_OFFSET);
    tmpVal |= (1 << pllClk);
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_OUTPUT_EN_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Disable_PLL_Clk(GLB_PLL_CLK_Type pllClk)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_OUTPUT_EN_OFFSET);
    tmpVal &= (~(1 << pllClk));
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_OUTPUT_EN_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Power_Off_PLL(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, CCI_PU_CLKPLL_SFREG);
    tmpVal = reg_clr_bit(tmpVal, CCI_PU_CLKPLL);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);

    tmpVal = readl(CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, CCI_CLKPLL_PU_CP);
    tmpVal = reg_clr_bit(tmpVal, CCI_CLKPLL_PU_PFD);
    tmpVal = reg_clr_bit(tmpVal, CCI_CLKPLL_PU_FBDV);
    tmpVal = reg_clr_bit(tmpVal, CCI_CLKPLL_PU_POSTDIV);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Set_Audio_PLL_Freq(GLB_AUDIO_PLL_Type audioPLLFreq)
{
    uint32_t sdmin_table[] = { 0x374BC6, 0x32CCED, 0x32CCED, 0x6E978D, 0x6C0000, 0x3E8000};
    uint32_t tmpVal = 0;

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_TOP_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_REFDIV_RATIO, 0x2);
    writel(tmpVal, CCI_BASE + CCI_CLKPLL_TOP_CTRL_OFFSET);

    tmpVal = readl(CCI_BASE + CCI_CLKPLL_SDM_OFFSET);
    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_SDMIN, (uint32_t)sdmin_table[audioPLLFreq % (sizeof(sdmin_table) / sizeof(sdmin_table[0]))]);

    writel(tmpVal, CCI_BASE + CCI_CLKPLL_SDM_OFFSET);

    tmpVal = readl(CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);

    tmpVal = reg_set_bits(tmpVal, CCI_PU_CLKPLL_SFREG, 1);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);

    arch_delay_ms(10);

    tmpVal = reg_set_bits(tmpVal, CCI_PU_CLKPLL, 1);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);

    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_RESET_FBDV, 1);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);

    arch_delay_ms(10);

    tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_RESET_FBDV, 0);
    writel(tmpVal, CCI_BASE + CCI_PU_RST_CLKPLL_OFFSET);

    /*set div for audio pll */
    tmpVal = readl(CCI_BASE + CCI_CLKPLL_TOP_CTRL_OFFSET);

    if (audioPLLFreq != AUDIO_PLL_5644800_HZ) {
        tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_POSTDIV, 36);
    } else {
        tmpVal = reg_set_bits(tmpVal, CCI_CLKPLL_POSTDIV, 72);
    }

    writel(tmpVal, CCI_BASE + CCI_CLKPLL_TOP_CTRL_OFFSET);

    return SUCCESS;
}

int GLB_Set_Flash_Id_Value(uint32_t idValue)
{
    writel(idValue|FLASH_ID_VALID_FLAG, GLB_BASE + GLB_RSV2_OFFSET);

    return SUCCESS;
}

uint32_t GLB_Get_Flash_Id_Value(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_RSV2_OFFSET);
    if ((tmpVal&FLASH_ID_VALID_FLAG) != 0) {
        return (tmpVal&FLASH_ID_VALID_MASK);
    }

    return 0x00000000;
}

int GLB_SetUnLockReq(GLB_LOCK_REQ_Type target)
{
    uint32_t tmpVal = 0;


    if (target == GLB_LOCK_REQ_SF) {
        tmpVal = readl(MCU_MISC_BASE + MCU_MISC_MCU_BUS_CFG1_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, MCU_MISC_ST_PROT_RDY_SF);
        writel(tmpVal, MCU_MISC_BASE + MCU_MISC_MCU_BUS_CFG1_OFFSET);
//        while (reg_is_bit_set(readl(MCU_MISC_BASE + MCU_MISC_MCU_BUS_CFG1_OFFSET), MCU_MISC_ST_PROT_RDY_SF));
    } else if (target == GLB_LOCK_REQ_OPI) {
        tmpVal = readl(GLB_BASE + GLB_RSV0_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_PROT_REQ_OPI);
        writel(tmpVal, GLB_BASE + GLB_RSV0_OFFSET);
//        while (reg_is_bit_set(readl(GLB_BASE + GLB_RSV0_OFFSET), GLB_STS_PROT_RDY_OPI));
    } else {
        return 1;
    }

    return 0;
}

int ATTR_CLOCK_SECTION GLB_Trim_RC32M(void)
{
    Efuse_Common_Trim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Common_Trim("rc32m", &trim, 1);
    if (trim.en) {
        if (trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
            tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
            tmpVal = reg_set_bit(tmpVal, GLB_RC32M_EXT_CODE_EN);
            writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
            arch_delay_us(2);
            tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL1_OFFSET);
            tmpVal = reg_set_bits(tmpVal, GLB_RC32M_CODE_FR_EXT2, trim.value);
            writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL1_OFFSET);
            tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL1_OFFSET);
            tmpVal = reg_set_bit(tmpVal, GLB_RC32M_EXT_CODE_SEL);
            writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL1_OFFSET);
            /* hw_5T + sw_5T  */
            arch_delay_us(1);
            return 0;
        }
    }
    return 1;
}

int ATTR_CLOCK_SECTION GLB_RC32K_Cal(void)
{
    uint32_t tmpVal = 0;
    int retry;
    int res = 8;

    /* Mirror HW vendor's IPRO7_CCI_RC32K_Cal_hbn_f32k_sel_RC32K_*.py flow.
     *
     * Step 0.2: pre-FSM RC32M setup. Lock RC32M to ext_code path so the
     * shared cal-clock domain runs at the trimmed RC32M frequency. Without
     * EXT_CODE_EN=1 the FSM may use an unstable / un-calibrated RC32M as
     * its time reference, producing the wrong cap_sel (observed: cap=5,
     * +115 kppm vs SW cal's cap=6, -30 kppm on a 4-bit trim).
     *
     *   cal_cgen   = 1   open the shared cal-clock gate
     *   ext_code_en= 1   use trimmed RC32M code, not auto-cal
     *   cal_en     = 0   RC32M FSM stays idle
     *   allow_cal  = 0   RC32M FSM stays idle
     *   cal_div    = 3   default
     */
    tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
    tmpVal = reg_set_bit(tmpVal,  GLB_RC32M_RC32K_CAL_CGEN);
    tmpVal = reg_set_bit(tmpVal,  GLB_RC32M_EXT_CODE_EN);
    tmpVal = reg_clr_bit(tmpVal,  GLB_RC32M_CAL_EN);
    tmpVal = reg_clr_bit(tmpVal,  GLB_RC32M_ALLOW_CAL);
    tmpVal = reg_set_bits(tmpVal, GLB_RC32M_CAL_DIV, 3);
    writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
    arch_delay_us(2);

    /* [260430] Reset RC32K fine code to default 0x1800 before FSM. The
     * 13-bit code_fr_cal field is a separate fine-tuning knob beyond the
     * 4-bit cap_sel; if it's left at a previous arbitrary value, the FSM
     * starts from a non-canonical baseline. */
    tmpVal = 0
        | (0xBU   << 28)   /* aon_rc32k_resv */
        | (0x1U   << 20)   /* sdm_bypass */
        | (0x1U   << 16)   /* sdm_rstb */
        | (0x1800U << 0);  /* code_fr_cal default */
    writel(tmpVal, HBN_BASE + HBN_RC32K_CTRL1_OFFSET);

    /* Initial cap_sel = 8 (midpoint), cal_en = 0. */
    HBN_Set_RC32K_CapSel(8);
    HBN_Set_RC32K_CalEnable(0);

    /* GLB_RC32K_CTRL2: allow_cal=0 → 1 with vref_dly=0, cal_div=3. */
    tmpVal = readl(GLB_BASE + GLB_RC32K_CTRL2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal,  GLB_RC32K_ALLOW_CAL);
    tmpVal = reg_set_bits(tmpVal, GLB_RC32K_DIG_VREF_DLY, 0);
    tmpVal = reg_set_bits(tmpVal, GLB_RC32K_DIG_CAL_DIV,  3);
    writel(tmpVal, GLB_BASE + GLB_RC32K_CTRL2_OFFSET);

    tmpVal = reg_set_bit(tmpVal, GLB_RC32K_ALLOW_CAL);
    writel(tmpVal, GLB_BASE + GLB_RC32K_CTRL2_OFFSET);

    /* Trigger cal: cal_en = 1 (with cap_sel still 8). */
    HBN_Set_RC32K_CalEnable(1);

    /* Wait for cal_done.
     * Script uses time.sleep(0.1); we poll the DIG_CAL_DONE flag with a
     * 200 ms ceiling. INPROGRESS proved unreliable on this silicon (often
     * never asserts); CAL_DONE is the signal to trust.
     *
     * If CAL_DONE never asserts (XTAL32K absent → FSM has no reference
     * clock), return -1 so caller can fall back to SW cal. Cleanup still
     * runs below to leave registers in a clean state. */
    int timed_out = 1;
    retry = 400;
    while (retry-- > 0) {
        if (readl(GLB_BASE + GLB_RC32K_CTRL2_OFFSET) & GLB_RC32K_DIG_CAL_DONE_MSK) {
            timed_out = 0;
            break;
        }
        arch_delay_us(500);
    }
    arch_delay_us(2000);    /* settle */

    res = reg_get_bits(readl(GLB_BASE + GLB_RC32K_CTRL2_OFFSET),
                       GLB_RC32K_DIG_CAP_CODE_FR_CAL);
    if (timed_out) {
        res = -1;
    }

    /* Cleanup: disable cal_en, allow_cal, cgen so caller can apply
     * cap_sel = res manually via HBN_Set_RC32K_CapSel(). */
    HBN_Set_RC32K_CalEnable(0);

    tmpVal = readl(GLB_BASE + GLB_RC32K_CTRL2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_RC32K_ALLOW_CAL);
    writel(tmpVal, GLB_BASE + GLB_RC32K_CTRL2_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_RC32M_RC32K_CAL_CGEN);
    writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL0_OFFSET);

    return res;
}

int ATTR_CLOCK_SECTION GLB_RC32M_Cal(void)
{
    uint32_t tmpVal;
    int retry;

    /* enable shared cal clock gate */
    tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_RC32M_RC32K_CAL_CGEN);
    writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
    arch_delay_us(1);

    /* clear cal state: ext_code_en=0, cal_en=0, allow_cal=0, cal_div=3 */
    tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_RC32M_EXT_CODE_EN);
    tmpVal = reg_clr_bit(tmpVal, GLB_RC32M_CAL_EN);
    tmpVal = reg_clr_bit(tmpVal, GLB_RC32M_ALLOW_CAL);
    tmpVal = reg_set_bits(tmpVal, GLB_RC32M_CAL_DIV, 3);
    writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL0_OFFSET);

    /* start calibration */
    tmpVal = reg_set_bit(tmpVal, GLB_RC32M_ALLOW_CAL);
    writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_RC32M_CAL_EN);
    writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL0_OFFSET);

    /* wait for CAL_INPROGRESS HIGH (cal started) */
    retry = 1000;
    while (!reg_is_bit_set(readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET), GLB_RC32M_CAL_INPROGRESS) && (retry-- > 0))
        arch_delay_us(10);

    /* wait for CAL_INPROGRESS LOW (cal done) */
    retry = 1000;
    while (reg_is_bit_set(readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET), GLB_RC32M_CAL_INPROGRESS) && (retry-- > 0))
        arch_delay_us(100);

    if (retry <= 0) {
        /* timeout — reset cal and return error */
        tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, GLB_RC32M_CAL_EN);
        tmpVal = reg_clr_bit(tmpVal, GLB_RC32M_ALLOW_CAL);
        tmpVal = reg_clr_bit(tmpVal, GLB_RC32M_RC32K_CAL_CGEN);
        writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
        return -1;
    }

    /* read 8-bit cal code [13:6] */
    int code = reg_get_bits(readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET), GLB_RC32M_DIG_CODE_FR_CAL);

    /* reset cal */
    tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_RC32M_CAL_EN);
    tmpVal = reg_clr_bit(tmpVal, GLB_RC32M_ALLOW_CAL);
    writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL0_OFFSET);

    /* apply result via ext_code path (same as GLB_Trim_RC32M) */
    tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_RC32M_EXT_CODE_EN);
    writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
    arch_delay_us(2);

    tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_RC32M_CODE_FR_EXT2, code);
    writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL1_OFFSET);
    tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_RC32M_EXT_CODE_SEL);
    writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL1_OFFSET);
    arch_delay_us(1);

    /* disable cal clock gate */
    tmpVal = readl(GLB_BASE + GLB_RC32M_CTRL0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_RC32M_RC32K_CAL_CGEN);
    writel(tmpVal, GLB_BASE + GLB_RC32M_CTRL0_OFFSET);

    return code;
}
