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



#include "drv_clock.h"
#include "drv_glb.h"
#include "drv_aon.h"
#include "drv_hbn.h"
#include "drv_pds.h"
#include "drv_ef_ctrl.h"

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
#define GLB_CLK_SET_DUMMY_TIMEOUT (0xFF)
#define GLB_FREQ_RC32M            (32000000)

#ifndef IPRO_USE_HAL_DRIVER
static intCallback_Type *glbBmxToIntCbfArra[BMX_TO_INT_ALL] = { NULL };
#endif

static int GLB_Get_And_Clr_First_Set_From_U64(uint64_t *val, uint32_t *bit);

/* WiFi PLL Config*/
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifiPllBasicCfg_32M_38P4M_40M = {
    .clkpllRefdivRatio = 2,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 0,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 0,        /*!< pll_icp_1u */
    .clkpllIcp5u = 2,        /*!< pll_icp_5u */
    .clkpllRz = 3,           /*!< pll_rz */
    .clkpllCz = 1,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 1,      /*!< pll_r4_short */
    .clkpllC4En = 0,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 5,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 1,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 1,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifiPllBasicCfg_24M = {
    .clkpllRefdivRatio = 1,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 0,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 0,        /*!< pll_icp_1u */
    .clkpllIcp5u = 2,        /*!< pll_icp_5u */
    .clkpllRz = 3,           /*!< pll_rz */
    .clkpllCz = 1,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 1,      /*!< pll_r4_short */
    .clkpllC4En = 0,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 5,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 1,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 1,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifiPllBasicCfg_26M = {
    .clkpllRefdivRatio = 1,  /*!< pll_refdiv_ratio */
    .clkpllIntFracSw = 1,    /*!< pll_int_frac_sw */
    .clkpllIcp1u = 1,        /*!< pll_icp_1u */
    .clkpllIcp5u = 0,        /*!< pll_icp_5u */
    .clkpllRz = 5,           /*!< pll_rz */
    .clkpllCz = 2,           /*!< pll_cz */
    .clkpllC3 = 2,           /*!< pll_c3 */
    .clkpllR4Short = 0,      /*!< pll_r4_short */
    .clkpllC4En = 1,         /*!< pll_r4_en */
    .clkpllSelSampleClk = 1, /*!< pll_sel_sample_clk */
    .clkpllVcoSpeed = 5,     /*!< pll_vco_speed */
    .clkpllSdmCtrlHw = 0,    /*!< pll_sdm_ctrl_hw */
    .clkpllSdmBypass = 0,    /*!< pll_sdm_bypass */
};
const GLB_WA_PLL_Cfg_Type ATTR_CLOCK_CONST_SECTION wifiPllCfg_960M[GLB_XTAL_MAX] = {
    { NULL, 0x0, 0 },                                 /*!< XTAL is None */
    { &wifiPllBasicCfg_24M, 0x1400000, 0 },           /*!< XTAL is 24M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1E00000, 0 }, /*!< XTAL is 32M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1900000, 0 }, /*!< XTAL is 38.4M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1800000, 0 }, /*!< XTAL is 40M */
    { &wifiPllBasicCfg_26M, 0x1276276, 0 },           /*!< XTAL is 26M */
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1E00000, 0 }, /*!< XTAL is RC32M */
};

const GLB_WA_PLL_Cfg_Type ATTR_CLOCK_CONST_SECTION wifiPllCfg_960M_Fast[1] = {
    { &wifiPllBasicCfg_32M_38P4M_40M, 0x1800000, 0 }, /*!< XTAL is 40M */
};
const uint32_t ATTR_CLOCK_CONST_SECTION usbPllSdmin_12M = 0x28000;
const uint32_t ATTR_CLOCK_CONST_SECTION sscDivSdmin_24M = 0x28000;

const GLB_SLAVE_GRP_0_TBL_Type ATTR_CLOCK_CONST_SECTION glb_slave_grp_0_table[GLB_SLAVE_GRP_0_MAX] = {
    { GLB_ADC_CFG0_OFFSET, GLB_GPADC_32M_DIV_EN_POS, GLB_GPADC_32M_CLK_SEL_POS, GLB_GPADC_32M_CLK_DIV_POS, GLB_GPADC_32M_DIV_EN_LEN, GLB_GPADC_32M_CLK_SEL_LEN, GLB_GPADC_32M_CLK_DIV_LEN },
    { GLB_I2C_CFG0_OFFSET, GLB_I2C_CLK_EN_POS, GLB_I2C_CLK_SEL_POS, GLB_I2C_CLK_DIV_POS, GLB_I2C_CLK_EN_LEN, GLB_I2C_CLK_SEL_LEN, GLB_I2C_CLK_DIV_LEN },
    { GLB_SPI_CFG0_OFFSET, GLB_SPI_CLK_EN_POS, GLB_SPI_CLK_SEL_POS, GLB_SPI_CLK_DIV_POS, GLB_SPI_CLK_EN_LEN, GLB_SPI_CLK_SEL_LEN, GLB_SPI_CLK_DIV_LEN },
    { GLB_DBI_CFG0_OFFSET, GLB_DBI_CLK_EN_POS, GLB_DBI_CLK_SEL_POS, GLB_DBI_CLK_DIV_POS, GLB_DBI_CLK_EN_LEN, GLB_DBI_CLK_SEL_LEN, GLB_DBI_CLK_DIV_LEN },
    { GLB_AUDIO_CFG0_OFFSET, GLB_REG_AUDIO_AUTO_DIV_EN_POS, 0, 0, GLB_REG_AUDIO_AUTO_DIV_EN_LEN, 0, 0 },
    { GLB_AUDIO_CFG0_OFFSET, GLB_REG_AUDIO_ADC_CLK_EN_POS, 0, GLB_REG_AUDIO_ADC_CLK_DIV_POS, GLB_REG_AUDIO_ADC_CLK_EN_LEN, 0, GLB_REG_AUDIO_ADC_CLK_DIV_LEN },
    { GLB_AUDIO_CFG1_OFFSET, GLB_REG_AUDIO_SOLO_CLK_EN_POS, 0, GLB_REG_AUDIO_SOLO_CLK_DIV_POS, GLB_REG_AUDIO_SOLO_CLK_EN_LEN, 0, GLB_REG_AUDIO_SOLO_CLK_DIV_LEN },
    { GLB_CAM_CFG0_OFFSET, GLB_REG_CAM_REF_CLK_EN_POS, GLB_REG_CAM_REF_CLK_SRC_SEL_POS, GLB_REG_CAM_REF_CLK_DIV_POS, GLB_REG_CAM_REF_CLK_EN_LEN, GLB_REG_CAM_REF_CLK_SRC_SEL_LEN, GLB_REG_CAM_REF_CLK_DIV_LEN },
    { GLB_PSRAM_CFG0_OFFSET, GLB_REG_PSRAMB_CLK_EN_POS, GLB_REG_PSRAMB_CLK_SEL_POS, GLB_REG_PSRAMB_CLK_DIV_POS, GLB_REG_PSRAMB_CLK_EN_LEN, GLB_REG_PSRAMB_CLK_SEL_LEN, GLB_REG_PSRAMB_CLK_DIV_LEN },
};

int ATTR_CLOCK_SECTION GLB_Set_MCU_Muxpll_160M_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_TOP_MUXPLL_160M_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);

    return SUCCESS;
}

uint8_t ATTR_CLOCK_SECTION GLB_Get_MCU_Muxpll_160M_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);
    return (uint8_t)(reg_get_bits(tmpVal, GLB_REG_TOP_MUXPLL_160M_SEL));
}

int ATTR_CLOCK_SECTION GLB_Set_MCU_Muxpll_80M_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_TOP_MUXPLL_80M_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);

    return SUCCESS;
}

uint8_t ATTR_CLOCK_SECTION GLB_Get_MCU_Muxpll_80M_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);
    return (uint8_t)(reg_get_bits(tmpVal, GLB_REG_TOP_MUXPLL_80M_SEL));
}

int ATTR_CLOCK_SECTION GLB_Set_ISP_Muxpll_80M_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_ISP_MUXPLL_80M_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);

    return SUCCESS;
}

uint8_t ATTR_CLOCK_SECTION GLB_Get_ISP_Muxpll_80M_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);
    return (uint8_t)(reg_get_bits(tmpVal, GLB_REG_ISP_MUXPLL_80M_SEL));
}

int ATTR_CLOCK_SECTION GLB_Power_On_XTAL_And_PLL_CLK(uint8_t xtalType, uint8_t pllType)
{
    uint32_t tmpVal;
    uint8_t mcuXclkSel;
    uint8_t mcuRootClkSel;
    volatile uint8_t refClk;

    if (xtalType == GLB_XTAL_NONE) {
        return ERROR;
    }

    if (xtalType != GLB_XTAL_RC32M) {
        /* power on xtal first */
        AON_Power_On_XTAL();
    }

#if defined(DUAL_CORE)
    uint8_t coreType;
    coreType = GLB_Get_Core_Type();
    if ((GLB_CORE_ID_M0 == coreType)) {
#endif
        /* M0 */
        /* Before config XTAL and PLL ,make sure root clk is from RC32M */
        mcuXclkSel = HBN_Get_MCU_XCLK_Sel();
        mcuRootClkSel = HBN_Get_MCU_Root_CLK_Sel();
        if ((mcuXclkSel != HBN_MCU_XCLK_RC32M) || (mcuRootClkSel != HBN_MCU_ROOT_CLK_XCLK)) {
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
        }
        GLB_Set_MCU_System_CLK_Div(0, 0);
#if defined(DUAL_CORE)
    }
#endif

    HBN_Set_Xtal_Type(xtalType);

    if (GLB_PLL_NONE == pllType) {
        GLB_CLK_SET_DUMMY_WAIT;
        return SUCCESS;
    }

    if (xtalType == GLB_XTAL_RC32M) {
        refClk = GLB_PLL_REFCLK_RC32M;
    } else {
        refClk = GLB_PLL_REFCLK_XTAL;
    }

    /* power on wifipll */
    if (pllType & GLB_PLL_WIFIPLL) {
        GLB_Power_Off_WIFIPLL();
        GLB_WIFIPLL_Ref_Clk_Sel(refClk);
        GLB_Power_On_WIFIPLL(&wifiPllCfg_960M[xtalType], 0);
    }

    arch_delay_us(75);

    if (xtalType != GLB_XTAL_RC32M) {
        /* if power on xtal, always set xclk from xtal */
        HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
    }

    /* enable all PLL clock output */
    /* GLB reg_pll_en = 1, cannot be zero */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_PLL_EN);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Power_Off_WIFIPLL(void)
{
    uint32_t REG_PLL_BASE_ADDRESS = 0;
    uint32_t tmpVal = 0;

    REG_PLL_BASE_ADDRESS = CCI_BASE + CCI_WIFI_PLL_CFG0_OFFSET;

    /* cfg0 : pu_wifipll=0 */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_PU_WIFIPLL, 0);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* cfg0 : pu_wifipll_sfreg=0 */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_PU_WIFIPLL_SFREG, 0);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_WIFIPLL_Ref_Clk_Sel(uint8_t refClk)
{
    uint32_t REG_PLL_BASE_ADDRESS = 0;
    uint32_t tmpVal = 0;

    REG_PLL_BASE_ADDRESS = CCI_BASE + CCI_WIFI_PLL_CFG0_OFFSET;

    /* xxxpll_refclk_sel */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 1);
    if (GLB_PLL_REFCLK_XTAL == refClk) {
        /* wifipll_refclk_sel different from other's pll, 1 means xtal */
        tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_REFCLK_SEL, 1);
    } else {
        tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_REFCLK_SEL, refClk);
    }
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 1);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Power_On_WIFIPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable)
{
    uint32_t REG_PLL_BASE_ADDRESS = 0;
    uint32_t tmpVal = 0;

    REG_PLL_BASE_ADDRESS = CCI_BASE + CCI_WIFI_PLL_CFG0_OFFSET;

    /* Step1:config parameter */
    /* cfg1:Set wifipll_refclk_sel and wifipll_refdiv_ratio */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 1);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_REFDIV_RATIO, cfg->basicCfg->clkpllRefdivRatio);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 1);

    /* cfg2:Set wifipll_int_frac_sw,wifipll_icp_1u,wifipll_icp_5u */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 2);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_INT_FRAC_SW, cfg->basicCfg->clkpllIntFracSw);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_ICP_1U, cfg->basicCfg->clkpllIcp1u);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_ICP_5U, cfg->basicCfg->clkpllIcp5u);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 2);

    /* cfg3:Set wifipll_rz,wifipll_cz,wifipll_c3,wifipll_r4_short,wifipll_r4_en */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 3);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_RZ, cfg->basicCfg->clkpllRz);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_CZ, cfg->basicCfg->clkpllCz);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_C3, cfg->basicCfg->clkpllC3);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_R4_SHORT, cfg->basicCfg->clkpllR4Short);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_C4_EN, cfg->basicCfg->clkpllC4En);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 3);

    /* cfg4:Set wifipll_sel_sample_clk */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 4);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SEL_SAMPLE_CLK, cfg->basicCfg->clkpllSelSampleClk);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 4);

    /* cfg5:Set wifipll_vco_speed */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 5);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_VCO_SPEED, cfg->basicCfg->clkpllVcoSpeed);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 5);

    /* cfg6:Set wifipll_sdm_bypass,wifipll_sdmin */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 6);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_CTRL_HW, cfg->basicCfg->clkpllSdmCtrlHw);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_BYPASS, cfg->basicCfg->clkpllSdmBypass);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDMIN, cfg->clkpllSdmin);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 6);

    /* cfg10:always set usbpll_sdmin */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 10);
    tmpVal = reg_set_bits(tmpVal, CCI_USBPLL_SDMIN, usbPllSdmin_12M);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 10);

    /* cfg12:always set sscdiv_sdmin */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 12);
    tmpVal = reg_set_bits(tmpVal, CCI_SSCDIV_SDMIN, sscDivSdmin_24M);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 12);

    /* Step2:config pu */
    /* cfg0 : pu_wifipll_sfreg=1 */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_PU_WIFIPLL_SFREG, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* delay > 2us */
    arch_delay_us(3);

    /* cfg0 : pu_wifipll=1 */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_PU_WIFIPLL, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* delay > 2us */
    arch_delay_us(3);

    /* toggle sdm_reset (pulse 0 > 1us) */
    /* cfg0 : wifipll_sdm_reset */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    arch_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_RSTB, 0);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    arch_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* Step3:reset pll */
    /* cfg0 : toggle wifipll_reset_fbdv, pulse 0 > 1us */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_FBDV_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    arch_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_FBDV_RSTB, 0);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    arch_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_FBDV_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* Step4:enable output clock */
    /* cfg8 : wifipll clock enable */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 8);
    tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV3);
    tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV4);
    tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV5);
    tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV6);
    tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV8);
    tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV10);
    tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV12);
    tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV20);
    tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV30);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 8);

    if (waitStable) {
        /* Wait 1.5*30us    */
        arch_delay_us(45);
    }

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Set_USB_CLK_From_WIFIPLL(uint8_t enable)
{
    uint32_t tmpVal = 0;

    /* pu_usbpll_mmdiv */
    tmpVal = readl(CCI_BASE + CCI_WIFI_PLL_CFG10_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, CCI_PU_USBPLL_MMDIV);
    } else {
        tmpVal = reg_clr_bit(tmpVal, CCI_PU_USBPLL_MMDIV);
    }
    writel(tmpVal, CCI_BASE + CCI_WIFI_PLL_CFG10_OFFSET);

    /* delay > 2us */
    arch_delay_us(3);

    /* toggle usbpll_rstb */
    tmpVal = readl(CCI_BASE + CCI_WIFI_PLL_CFG10_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CCI_USBPLL_RSTB);
    writel(tmpVal, CCI_BASE + CCI_WIFI_PLL_CFG10_OFFSET);
    arch_delay_us(2);
    tmpVal = readl(CCI_BASE + CCI_WIFI_PLL_CFG10_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, CCI_USBPLL_RSTB);
    writel(tmpVal, CCI_BASE + CCI_WIFI_PLL_CFG10_OFFSET);
    arch_delay_us(2);
    tmpVal = readl(CCI_BASE + CCI_WIFI_PLL_CFG10_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CCI_USBPLL_RSTB);
    writel(tmpVal, CCI_BASE + CCI_WIFI_PLL_CFG10_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Set_SSC_CLK_From_WIFIPLL(uint8_t enable)
{
    uint32_t tmpVal = 0;

    /* pu_usbpll_mmdiv */
    tmpVal = readl(CCI_BASE + CCI_WIFI_PLL_CFG12_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, CCI_PU_SSCDIV_MMDIV);
    } else {
        tmpVal = reg_clr_bit(tmpVal, CCI_PU_SSCDIV_MMDIV);
    }
    writel(tmpVal, CCI_BASE + CCI_WIFI_PLL_CFG12_OFFSET);

    /* delay > 2us */
    arch_delay_us(3);

    /* toggle sscdiv_rstb */
    tmpVal = readl(CCI_BASE + CCI_WIFI_PLL_CFG12_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CCI_SSCDIV_RSTB);
    writel(tmpVal, CCI_BASE + CCI_WIFI_PLL_CFG12_OFFSET);
    arch_delay_us(2);
    tmpVal = readl(CCI_BASE + CCI_WIFI_PLL_CFG12_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, CCI_SSCDIV_RSTB);
    writel(tmpVal, CCI_BASE + CCI_WIFI_PLL_CFG12_OFFSET);
    arch_delay_us(2);
    tmpVal = readl(CCI_BASE + CCI_WIFI_PLL_CFG12_OFFSET);
    tmpVal = reg_set_bit(tmpVal, CCI_SSCDIV_RSTB);
    writel(tmpVal, CCI_BASE + CCI_WIFI_PLL_CFG12_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv)
{
    uint32_t tmpVal;
    uint32_t timeout = 1024;
    uint8_t xclkSel;
    uint8_t rootClkSel;

    /* get root clock */
    xclkSel = HBN_Get_MCU_XCLK_Sel();
    rootClkSel = HBN_Get_MCU_Root_CLK_Sel();

    if ((xclkSel != HBN_MCU_XCLK_RC32M) || (rootClkSel != HBN_MCU_ROOT_CLK_XCLK)) {
        HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
        HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    }

    /* config hclk_div=mcuClkDiv */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_HCLK_DIV, mcuClkDiv);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);

    /* config bclk_div=mcuPBclkDiv */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV, mcuPBclkDiv);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);
    /* bclk act pulse */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV_ACT_PULSE, 1);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG1_OFFSET);

    timeout = 1024;
    do {
        tmpVal = readl(GLB_BASE + GLB_SYS_CFG1_OFFSET);
        tmpVal = reg_get_bits(tmpVal, GLB_STS_BCLK_PROT_DONE);
    } while ((--timeout) && (!tmpVal));
    if (!timeout) {
        return ERROR;
    }

    /* recover root clock */
    HBN_Set_MCU_XCLK_Sel(xclkSel);
    HBN_Set_MCU_Root_CLK_Sel(rootClkSel);

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Get_MCU_System_CLK_Div(uint8_t *mcuClkDiv, uint8_t *mcuPBclkDiv)
{
    if ((NULL == mcuClkDiv) || (NULL == mcuPBclkDiv)) {
        return ERROR;
    }

    *mcuClkDiv = reg_get_bits(readl(GLB_BASE + GLB_SYS_CFG0_OFFSET), GLB_REG_HCLK_DIV);
    *mcuPBclkDiv = reg_get_bits(readl(GLB_BASE + GLB_SYS_CFG0_OFFSET), GLB_REG_BCLK_DIV);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Set_MCU_System_CLK(uint8_t clkFreq)
{
    uint32_t tmpVal;
    uint8_t mcuXclkSel;

    /* get xclk&&rootclk clock */
    mcuXclkSel = HBN_Get_MCU_XCLK_Sel();

    /* change root clock to rc32m */
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    GLB_Set_MCU_System_CLK_Div(0, 0);

    /* select pll output clock before select root clock */
    tmpVal = readl(PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_TOP_AUPLL_DIV2:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 0);
            break;
        case GLB_MCU_SYS_CLK_TOP_AUPLL_DIV1:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 1);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 2);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 3);
            break;
        default:
            break;
    }
    writel(tmpVal, PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);

    /* select root clock */
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_RC32M:
            GLB_Set_MCU_System_CLK_Div(0, 0);
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
            break;
        case GLB_MCU_SYS_CLK_XTAL:
            GLB_Set_MCU_System_CLK_Div(0, 0);
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
            break;
        case GLB_MCU_SYS_CLK_TOP_AUPLL_DIV2:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 2);
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_AUPLL_DIV2);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        case GLB_MCU_SYS_CLK_TOP_AUPLL_DIV1:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 3);
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_AUPLL_DIV1);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 2);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_240M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 3);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_320M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        default:
            break;
    }

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

uint8_t ATTR_CLOCK_SECTION GLB_Get_Core_Type(void)
{
    return GLB_CORE_ID_M0;
}

int ATTR_CLOCK_SECTION GLB_Set_CPU_Reset_Address(uint8_t coreID, uint32_t addr)
{

    switch (coreID) {
        case GLB_CORE_ID_M0:
            writel(addr, PDS_BASE + PDS_CPU_CORE_CFG14_OFFSET);
            break;
        default:
            return ERROR;
    }

    return SUCCESS;
}

int GLB_Set_ADC_CLK(uint8_t enable, GLB_ADC_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal;

    /* disable ADC clock first */
    tmpVal = readl(GLB_BASE + GLB_ADC_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_GPADC_32M_DIV_EN);
    writel(tmpVal, GLB_BASE + GLB_ADC_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_ADC_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_GPADC_32M_CLK_DIV, div);
    tmpVal = reg_set_bits(tmpVal, GLB_GPADC_32M_CLK_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_ADC_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_ADC_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_GPADC_32M_DIV_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_GPADC_32M_DIV_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_ADC_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_DMA_CLK(uint8_t enable, uint8_t clk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    /* DMA0 */
    tmpVal = readl(GLB_BASE + GLB_DMA_CFG0_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, GLB_DMA_CLK_EN);
    if (enable) {
        tmpVal2 |= (1 << clk);
    } else {
        tmpVal2 &= (~(1 << clk));
    }
    tmpVal = reg_set_bits(tmpVal, GLB_DMA_CLK_EN, tmpVal2);
    writel(tmpVal, GLB_BASE + GLB_DMA_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_Peripheral_DMA_CN(uint8_t peri, uint8_t cn)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_DMA_CFG2_OFFSET);
    switch (cn) {
        case GLB_PERI_DMA_CN_SEL_DMA0:
            tmpVal &= ~(1 << peri);
            break;
        default:
            break;
    }
    writel(tmpVal, GLB_BASE + GLB_DMA_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_IR_RX_GPIO_Sel(uint8_t gpio)
{
    uint32_t tmpVal = 0;

    /* Select gpio between gpio9 and gpio23 */
    if (gpio > 8 && gpio < 24) {
        tmpVal = readl(GLB_BASE + GLB_IR_CFG1_OFFSET);
        tmpVal = reg_set_bits(tmpVal, GLB_IR_RX_GPIO_SEL, gpio - 8);
        writel(tmpVal, GLB_BASE + GLB_IR_CFG1_OFFSET);
    }

    /* Close ir rx */
    if (gpio == 0) {
        tmpVal = readl(GLB_BASE + GLB_IR_CFG1_OFFSET);
        tmpVal = reg_set_bits(tmpVal, GLB_IR_RX_GPIO_SEL, 0);
        writel(tmpVal, GLB_BASE + GLB_IR_CFG1_OFFSET);
    } else {
        return ERROR;
    }

    return SUCCESS;
}

int GLB_Set_UART_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    /* disable UART clock first */
    tmpVal = readl(GLB_BASE + GLB_UART_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_UART_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_UART_CFG0_OFFSET);

    /* Set div */
    tmpVal = readl(GLB_BASE + GLB_UART_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_UART_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_UART_CFG0_OFFSET);

    /* Select clock source for uart */
    HBN_Set_UART_CLK_Sel(clkSel);

    /* Set enable or disable */
    tmpVal = readl(GLB_BASE + GLB_UART_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_UART_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_UART_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_UART_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_UART_Fun_Sel(uint8_t sig, uint8_t fun)
{
    uint32_t sig_pos = 0;
    uint32_t tmpVal = 0;

    if (sig < GLB_UART_SIG_8) {
        tmpVal = readl(GLB_BASE + GLB_UART_CFG1_OFFSET);
        sig_pos = (sig * 4);
        /* Clear original val */
        tmpVal &= (~(0xf << sig_pos));
        /* Set new value */
        tmpVal |= (fun << sig_pos);
        writel(tmpVal, GLB_BASE + GLB_UART_CFG1_OFFSET);
    } else {
        tmpVal = readl(GLB_BASE + GLB_UART_CFG2_OFFSET);
        sig_pos = ((sig - 8) * 4);
        /* Clear original val */
        tmpVal &= (~(0xf << sig_pos));
        /* Set new value */
        tmpVal |= (fun << sig_pos);
        writel(tmpVal, GLB_BASE + GLB_UART_CFG2_OFFSET);
    }

    return SUCCESS;
}

uint8_t GLB_UART_Get_Fun(uint8_t sig)
{
    uint32_t sig_pos = 0;
    uint32_t tmpVal = 0;

    if (sig < GLB_UART_SIG_8) {
        tmpVal = readl(GLB_BASE + GLB_UART_CFG1_OFFSET);
        sig_pos = (sig * 4);
    } else {
        tmpVal = readl(GLB_BASE + GLB_UART_CFG2_OFFSET);
        sig_pos = ((sig - 8) * 4);
    }

    return (uint8_t)((tmpVal >> sig_pos) & 0xf);
}

uint8_t GLB_UART_Find_Sig_By_Fun(uint8_t fun)
{
    uint32_t tmpVal1 = 0;
    uint32_t tmpVal2 = 0;
    uint8_t i;

    tmpVal1 = readl(GLB_BASE + GLB_UART_CFG1_OFFSET);
    tmpVal2 = readl(GLB_BASE + GLB_UART_CFG2_OFFSET);

    /* Search in sig 0-7 */
    for (i = 0; i < 8; i++) {
        if (((tmpVal1 >> (i * 4)) & 0xf) == fun) {
            return i;
        }
    }

    /* Search in sig 8-11 */
    for (i = 0; i < 4; i++) {
        if (((tmpVal2 >> (i * 4)) & 0xf) == fun) {
            return (uint8_t)(i + 8);
        }
    }

    /* Not found, return invalid sig */
    return 0xff;
}

int GLB_UART_Fun_Sel_Swap(uint8_t sig, uint8_t fun)
{
    uint8_t old_fun;
    uint8_t old_sig;

    /* 1. Read current function of target sig */
    old_fun = GLB_UART_Get_Fun(sig);

    /* 2. Find which sig currently has the target fun */
    old_sig = GLB_UART_Find_Sig_By_Fun(fun);

    /* 3. Set target sig to new fun */
    GLB_UART_Fun_Sel(sig, fun);

    /* 4. Swap: set old_sig to old_fun */
    if (old_sig != sig && old_sig != 0xff) {
        GLB_UART_Fun_Sel(old_sig, old_fun);
    }

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Set_SF_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    /* disable SFLASH clock first */
    tmpVal = readl(GLB_BASE + GLB_SF_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_SF_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_SF_CFG0_OFFSET);

    /* clock divider */
    /* Select flash clock, all Flash CLKs are divied by PLL_480M */
    tmpVal = readl(GLB_BASE + GLB_SF_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_DIV, div);
    switch (clkSel) {
        case GLB_SFLASH_CLK_WIFIPLL_120M:
            tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_SEL, 0x0);
            tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_SEL2, 0x0);
            break;
        case GLB_SFLASH_CLK_XTAL:
            tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_SEL, 0x0);
            tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_SEL2, 0x1);
            break;
        case GLB_SFLASH_CLK_MUXPLL_80M:
            tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_SEL, 0x1);
            break;
        case GLB_SFLASH_CLK_BCLK:
            tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_SEL, 0x2);
            break;
        case GLB_SFLASH_CLK_WIFIPLL_96M:
            tmpVal = reg_set_bits(tmpVal, GLB_SF_CLK_SEL, 0x3);
            break;
        default:
            break;
    }
    writel(tmpVal, GLB_BASE + GLB_SF_CFG0_OFFSET);

    /* enable or disable flash clock */
    tmpVal = readl(GLB_BASE + GLB_SF_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_SF_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_SF_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_SF_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_I2C_CLK(uint8_t enable, GLB_I2C_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_I2C_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_I2C_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_I2C_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_I2C_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_I2C_CLK_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_I2C_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_I2C_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_I2C_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_I2C_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_I2C_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_I2C_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_I2S_CLK(uint8_t refClkEn, uint8_t refClkDiv, uint8_t inRef, uint8_t outRef)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_I2S_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_I2S_DO_REF_CLK_SEL, outRef);
    if (refClkEn) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_I2S_REF_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_I2S_REF_CLK_EN);
    }
    tmpVal = reg_set_bits(tmpVal, GLB_REG_I2S_DI_REF_CLK_SEL, inRef);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_I2S_REF_CLK_DIV, refClkDiv);
    writel(tmpVal, GLB_BASE + GLB_I2S_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_SPI_CLK(uint8_t enable, GLB_SPI_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_SPI_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_SPI_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_SPI_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_SPI_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_SPI_CLK_DIV, div);
    tmpVal = reg_set_bits(tmpVal, GLB_SPI_CLK_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_SPI_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_SPI_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_SPI_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_SPI_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_SPI_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_SPI_Sig_Swap_Set(uint8_t group, uint8_t swap)
{
    uint32_t tmpVal = 0;
    uint32_t tmpVal2 = 0;

    tmpVal = readl(GLB_BASE + GLB_SPI_CFG0_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, GLB_SPI_SWAP_SET);
    if (swap) {
        tmpVal2 |= (1 << group);
    } else {
        tmpVal2 &= ~(1 << group);
    }
    tmpVal = reg_set_bits(tmpVal, GLB_SPI_SWAP_SET, tmpVal2);
    writel(tmpVal, GLB_BASE + GLB_SPI_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_PWM1_IO_Sel(uint8_t ioSel)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_PWM_CFG0_OFFSET);
    tmpVal >>= 1;
    tmpVal = reg_set_bits(tmpVal, GLB_REG_PWM1_IO_SEL, ioSel);
    writel(tmpVal, GLB_BASE + GLB_PWM_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_PDM_IO_Sel(uint8_t ioSel)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_PDM_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_PDM_IO_SEL, ioSel);
    writel(tmpVal, GLB_BASE + GLB_PDM_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_DBI_CLK(uint8_t enable, GLB_DBI_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_DBI_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_DBI_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_DBI_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_DBI_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_DBI_CLK_DIV, div);
    tmpVal = reg_set_bits(tmpVal, GLB_DBI_CLK_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_DBI_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_DBI_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_DBI_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_DBI_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_DBI_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_DIG_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;
    uint32_t dig512kEn;
    uint32_t dig32kEn;

    /* disable DIG512K and DIG32K clock first */
    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);
    dig512kEn = reg_get_bits(tmpVal, GLB_DIG_512K_EN);
    dig32kEn = reg_get_bits(tmpVal, GLB_DIG_32K_EN);
    tmpVal = reg_clr_bit(tmpVal, GLB_DIG_512K_EN);
    tmpVal = reg_clr_bit(tmpVal, GLB_DIG_32K_EN);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_DIG_CLK_SRC_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);

    /* repristinate DIG512K and DIG32K clock */
    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_DIG_512K_EN, dig512kEn);
    tmpVal = reg_set_bits(tmpVal, GLB_DIG_32K_EN, dig32kEn);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_DIG_512K_CLK(uint8_t enable, uint8_t compensationEn, uint8_t div)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);
    if (compensationEn) {
        tmpVal = reg_set_bit(tmpVal, GLB_DIG_512K_COMP);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_DIG_512K_COMP);
    }
    tmpVal = reg_set_bits(tmpVal, GLB_DIG_512K_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_DIG_512K_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_DIG_512K_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_DIG_32K_CLK(uint8_t enable, uint8_t compensationEn, uint16_t div)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);
    if (compensationEn) {
        tmpVal = reg_set_bit(tmpVal, GLB_DIG_32K_COMP);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_DIG_32K_COMP);
    }
    tmpVal = reg_set_bits(tmpVal, GLB_DIG_32K_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_DIG_32K_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_DIG_32K_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Platform_Wakeup_PDS_Enable(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_EN_PLATFORM_WAKEUP);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_EN_PLATFORM_WAKEUP);
    }
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Sel_MCU_TMR_GPIO_Clock(uint8_t gpioPin)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);
    switch (gpioPin & 0x3) {
        case 0: /* inout_sig_0 */
            tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_0_EN, 0);
            tmpVal = reg_set_bits(tmpVal, GLB_GPIO_TMR_CLK_SEL, 0);
            break;
        case 1: /* inout_sig_1 */
            tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_1_EN, 0);
            tmpVal = reg_set_bits(tmpVal, GLB_GPIO_TMR_CLK_SEL, 1);
            break;
        case 2: /* inout_sig_2 */
            tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_2_EN, 0);
            tmpVal = reg_set_bits(tmpVal, GLB_GPIO_TMR_CLK_SEL, 2);
            break;
        case 3: /* inout_sig_3 */
            tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_3_EN, 0);
            tmpVal = reg_set_bits(tmpVal, GLB_GPIO_TMR_CLK_SEL, 3);
            break;
        default:
            break;
    }
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_Chip_Clock_Out0_Sel(GLB_CHIP_CLK_OUT_0_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);
    /* set clk out0 output enable */
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_0_EN, 1);
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_0_SEL, clkOutType);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);

    return SUCCESS;

}

int GLB_Set_Chip_Clock_Out1_Sel(GLB_CHIP_CLK_OUT_1_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);
    /* set clk out1 output enable */
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_1_EN, 1);
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_1_SEL, clkOutType);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_Chip_Clock_Out2_Sel(GLB_CHIP_CLK_OUT_2_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);
    /* set clk out2 output enable */
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_2_EN, 1);
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_2_SEL, clkOutType);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_Set_Chip_Clock_Out3_Sel(GLB_CHIP_CLK_OUT_3_Type clkOutType)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);
    /* set clk out3 output enable */
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_3_EN, 1);
    tmpVal = reg_set_bits(tmpVal, GLB_CHIP_CLK_OUT_3_SEL, clkOutType);
    writel(tmpVal, GLB_BASE + GLB_DIG_CLK_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_BMX_TO_Init(BMX_TO_Cfg_Type *BmxCfg)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_BMX_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BMX_TIMEOUT_EN, BmxCfg->timeoutEn);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BMX_ARB_MODE, BmxCfg->arbMod);
    writel(tmpVal, GLB_BASE + GLB_BMX_CFG0_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(BMX_MCU_TO_IRQn, BMX_TO_IRQHandler);
#endif

    return SUCCESS;
}

uint8_t GLB_Get_BMX_TO_Status(void)
{
    return reg_get_bits(readl(GLB_BASE + GLB_BMX_CFG0_OFFSET), GLB_STS_BMX_TIMEOUT_STS);
}

int GLB_Clr_BMX_TO_Status(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_BMX_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_BMX_TIMEOUT_CLR);
    writel(tmpVal, GLB_BASE + GLB_BMX_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_BMX_CFG0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_BMX_TIMEOUT_CLR);
    writel(tmpVal, GLB_BASE + GLB_BMX_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_BMX_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_BMX_TIMEOUT_CLR);
    writel(tmpVal, GLB_BASE + GLB_BMX_CFG0_OFFSET);

    return SUCCESS;
}

#ifndef IPRO_USE_HAL_DRIVER
int BMX_TIMEOUT_INT_Callback_Install(BMX_TO_INT_Type intType, intCallback_Type *cbFun)
{

    glbBmxToIntCbfArra[intType] = cbFun;

    return SUCCESS;
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
void BMX_TO_IRQHandler(void)
{
    BMX_TO_INT_Type intType;

    for (intType = 0; intType < BMX_TO_INT_ALL; intType++) {
        if (glbBmxToIntCbfArra[intType] != NULL) {
            glbBmxToIntCbfArra[intType]();
        }
    }
    GLB_Clr_BMX_TO_Status();
}
#endif

int GLB_Set_Audio_AUTO_CLK(uint8_t divEn)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG0_OFFSET);
    if (divEn) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_Audio_ADC_CLK(uint8_t enable, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_AUDIO_ADC_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_Audio_SOLO_CLK(uint8_t enable, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG1_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_AUDIO_SOLO_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG1_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG1_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_AUDIO_CFG1_OFFSET);

    return SUCCESS;
}

int GLB_Invert_ETH_RX_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_ETH_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_CFG_INV_ETH_RX_CLK);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_CFG_INV_ETH_RX_CLK);
    }
    writel(tmpVal, GLB_BASE + GLB_ETH_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Invert_ETH_TX_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_ETH_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_CFG_INV_ETH_TX_CLK);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_CFG_INV_ETH_TX_CLK);
    }
    writel(tmpVal, GLB_BASE + GLB_ETH_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Invert_ETH_REF_O_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_ETH_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_CFG_INV_ETH_REF_CLK_O);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_CFG_INV_ETH_REF_CLK_O);
    }
    writel(tmpVal, GLB_BASE + GLB_ETH_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_ETH_REF_O_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_ETH_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_CFG_SEL_ETH_REF_CLK_O, clkSel);
    writel(tmpVal, GLB_BASE + GLB_ETH_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_CAM_CLK(uint8_t enable, GLB_CAM_CLK_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CAM_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_CAM_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CAM_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_CAM_REF_CLK_SRC_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_CAM_REF_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_CAM_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_CAM_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_CAM_CFG0_OFFSET);

    return SUCCESS;
}


int GLB_Config_SDIO_Host_Reset_System(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_SDIO_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_SYS_RST_SD_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_SYS_RST_SD_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_SDIO_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Config_SDIO_Host_Interrupt_CPU(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_SDIO_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_SDIO_INT_SYS_DIS);
    } else {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_SDIO_INT_SYS_DIS);
    }
    writel(tmpVal, GLB_BASE + GLB_SDIO_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_UART_Sig_Swap_Set(uint8_t group, uint8_t swap)
{
    uint32_t tmpVal = 0;
    uint32_t tmpVal2 = 0;

    tmpVal = readl(GLB_BASE + GLB_PARM_CFG0_OFFSET);
    tmpVal2 = reg_get_bits(tmpVal, GLB_UART_SWAP_SET);
    if (swap) {
        tmpVal2 |= (1 << group);
    } else {
        tmpVal2 &= ~(1 << group);
    }
    tmpVal = reg_set_bits(tmpVal, GLB_UART_SWAP_SET, tmpVal2);
    writel(tmpVal, GLB_BASE + GLB_PARM_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Swap_SPI_0_MOSI_With_MISO(int newState)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_PARM_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_SPI_0_SWAP, newState);
    writel(tmpVal, GLB_BASE + GLB_PARM_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Set_SPI_0_ACT_MOD_Sel(uint8_t mod)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_PARM_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_SPI_0_MASTER_MODE, mod);
    writel(tmpVal, GLB_BASE + GLB_PARM_CFG0_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_Set_SFlash_IO_PARM(uint8_t selEmbedded, uint8_t swapIo3Io0, uint8_t swapIo2Cs)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_PARM_CFG0_OFFSET);
    if (selEmbedded) {
        tmpVal = reg_set_bit(tmpVal, GLB_SEL_EMBEDDED_SFLASH);
        if (swapIo3Io0) {
            tmpVal = reg_set_bit(tmpVal, GLB_SWAP_SFLASH_IO_3_IO_0);
        } else {
            tmpVal = reg_clr_bit(tmpVal, GLB_SWAP_SFLASH_IO_3_IO_0);
        }
        if (swapIo2Cs) {
            tmpVal = reg_set_bit(tmpVal, GLB_SWAP_SFLASH_IO_2_CS);
        } else {
            tmpVal = reg_clr_bit(tmpVal, GLB_SWAP_SFLASH_IO_2_CS);
        }
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_SEL_EMBEDDED_SFLASH);
        tmpVal = reg_set_bit(tmpVal, GLB_SWAP_SFLASH_IO_3_IO_0);
        tmpVal = reg_set_bit(tmpVal, GLB_SWAP_SFLASH_IO_2_CS);
    }
    writel(tmpVal, GLB_BASE + GLB_PARM_CFG0_OFFSET);

    if (selEmbedded) {
        GLB_Embedded_Flash_Pad_Enable(swapIo2Cs);
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_Set_SFlash2_IO_PARM(uint8_t swapIo3Io0)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_PARM_CFG0_OFFSET);
    if (swapIo3Io0) {
        tmpVal = reg_set_bit(tmpVal, GLB_SWAP_SFLASH2_IO_3_IO_0);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_SWAP_SFLASH2_IO_3_IO_0);
    }
    writel(tmpVal, GLB_BASE + GLB_PARM_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Clr_Reset_Reason(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_RESET_STS0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_CLR_TOP_RESET_RECORDER);
    writel(tmpVal, GLB_BASE + GLB_RESET_STS0_OFFSET);
    tmpVal = readl(GLB_BASE + GLB_RESET_STS0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_CLR_TOP_RESET_RECORDER);
    writel(tmpVal, GLB_BASE + GLB_RESET_STS0_OFFSET);
    tmpVal = readl(GLB_BASE + GLB_RESET_STS0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_CLR_TOP_RESET_RECORDER);
    writel(tmpVal, GLB_BASE + GLB_RESET_STS0_OFFSET);

    return SUCCESS;
}

int GLB_Set_SPI2DVP_State(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_PARM_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_SPI2DVP_EN, enable);
    writel(tmpVal, GLB_BASE + GLB_PARM_CFG0_OFFSET);

    return SUCCESS;
}

int GLB_Get_Reset_Reason(GLB_RESET_RECORD_Type *reason)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_RESET_STS0_OFFSET);
    reason->reset_recorder_ana_powb = (tmpVal >> 0) & 0x1;
    reason->reset_recorder_ext_rst_n = (tmpVal >> 1) & 0x1;
    reason->reset_recorder_pds_reset = (tmpVal >> 2) & 0x1;
    reason->reset_recorder_wdt_rst_n = (tmpVal >> 3) & 0x1;
    reason->reset_recorder_cpu_porst_n = (tmpVal >> 4) & 0x1;
    reason->reset_recorder_sys_reset_n = (tmpVal >> 5) & 0x1;
    reason->reset_recorder_cpu_sys_rstreq_n = (tmpVal >> 6) & 0x1;

    return SUCCESS;
}

int GLB_AHB_MCU_Software_Reset(uint8_t swrst)
{
    uint32_t tmpVal = 0;
    uint32_t bit = 0;
    uint32_t regAddr = 0;

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
    DRV_DUMMY_ACT;
    tmpVal = readl(regAddr);
    tmpVal |= (1 << bit);
    writel(tmpVal, regAddr);
    DRV_DUMMY_ACT;
    tmpVal = readl(regAddr);
    tmpVal &= ~(1 << bit);
    writel(tmpVal, regAddr);

    return SUCCESS;
}

int GLB_Disrst_Set(uint8_t enable, GLB_DISRST_Type disrst)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG3_OFFSET);
    if (enable) {
        tmpVal |= (1 << disrst);
    } else {
        tmpVal &= ~(1 << disrst);
    }
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG3_OFFSET);

    return SUCCESS;
}

static int GLB_Get_And_Clr_First_Set_From_U64(uint64_t *val, uint32_t *bit)
{
    if (!*val) {
        return ERROR;
    }

    for (uint8_t i = 0; i < 64; i++) {
        if ((*val) & ((uint64_t)1 << i)) {
            *bit = i;
            (*val) &= ~((uint64_t)1 << i);
            break;
        }
    }

    return SUCCESS;
}

int GLB_PER_Clock_Gate(uint64_t ips)
{
    /* api request from cjy */

    uint32_t tmpValCfg0 = 0;
    uint32_t tmpValCfg1 = 0;
    uint32_t tmpValCfg2 = 0;
    uint32_t bitfield = 0;

    tmpValCfg0 = readl(GLB_BASE + GLB_CGEN_CFG0_OFFSET);
    tmpValCfg1 = readl(GLB_BASE + GLB_CGEN_CFG1_OFFSET);
    tmpValCfg2 = readl(GLB_BASE + GLB_CGEN_CFG2_OFFSET);
    while (ips) {
        if (SUCCESS == GLB_Get_And_Clr_First_Set_From_U64(&ips, &bitfield)) {
            switch (bitfield) {
                case GLB_AHB_CLOCK_IP_CPU:
                    tmpValCfg0 &= ~(1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_RSVD:
                    tmpValCfg0 &= ~(1 << 1);
                    break;
                case GLB_AHB_CLOCK_IP_SEC:
                    tmpValCfg0 &= ~(1 << 2);
                    tmpValCfg1 &= ~(1 << 3);
                    tmpValCfg1 &= ~(1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_0:
                    tmpValCfg0 &= ~(1 << 3);
                    tmpValCfg1 &= ~(1 << 12);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_1:
                    break;
                case GLB_AHB_CLOCK_IP_DMA_2:
                    break;
                case GLB_AHB_CLOCK_IP_CCI:
                    tmpValCfg0 &= ~(1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_RF_TOP:
                    break;
                case GLB_AHB_CLOCK_IP_GPIP:
                    tmpValCfg1 &= ~(1 << 2);
                    break;
                case GLB_AHB_CLOCK_IP_TZC:
                    tmpValCfg1 &= ~(1 << 5);
                    break;
                case GLB_AHB_CLOCK_IP_EF_CTRL:
                    tmpValCfg1 &= ~(1 << 7);
                    break;
                case GLB_AHB_CLOCK_IP_SF_CTRL:
                    tmpValCfg1 &= ~(1 << 11);
                    break;
                case GLB_AHB_CLOCK_IP_EMAC:
                    tmpValCfg2 &= ~(1 << 23);
                    break;
                case GLB_AHB_CLOCK_IP_UART0:
                    tmpValCfg1 &= ~(1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_UART1:
                    tmpValCfg1 &= ~(1 << 17);
                    break;
                case GLB_AHB_CLOCK_IP_UART2:
                    break;
                case GLB_AHB_CLOCK_IP_UART3:
                    break;
                case GLB_AHB_CLOCK_IP_UART4:
                    break;
                case GLB_AHB_CLOCK_IP_SPI:
                    tmpValCfg1 &= ~(1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_I2C:
                    tmpValCfg1 &= ~(1 << 19);
                    break;
                case GLB_AHB_CLOCK_IP_PWM:
                    tmpValCfg1 &= ~(1 << 20);
                    break;
                case GLB_AHB_CLOCK_IP_TIMER:
                    tmpValCfg1 &= ~(1 << 21);
                    break;
                case GLB_AHB_CLOCK_IP_IR:
                    tmpValCfg1 &= ~(1 << 22);
                    break;
                case GLB_AHB_CLOCK_IP_CHECKSUM:
                    tmpValCfg1 &= ~(1 << 23);
                    break;
                case GLB_AHB_CLOCK_IP_QDEC:
                    break;
                case GLB_AHB_CLOCK_IP_KYS:
                    break;
                case GLB_AHB_CLOCK_IP_I2S:
                    tmpValCfg1 &= ~(1 << 27);
                    break;
                case GLB_AHB_CLOCK_IP_USB11:
                    break;
                case GLB_AHB_CLOCK_IP_CAM:
                    break;
                case GLB_AHB_CLOCK_IP_MJPEG:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_LP:
                    break;
                case GLB_AHB_CLOCK_IP_ZB_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_ZB_LP:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_NORMAL:
                    tmpValCfg2 &= ~(1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_LP:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_2_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_2_LP:
                    break;
                case GLB_AHB_CLOCK_IP_EMI_MISC:
                    break;
                case GLB_AHB_CLOCK_IP_PSRAM0_CTRL:
                    break;
                case GLB_AHB_CLOCK_IP_PSRAM1_CTRL:
                    tmpValCfg2 &= ~(1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_USB20_SDU:
                    tmpValCfg1 &= ~(1 << 13);
                    break;
                case GLB_AHB_CLOCK_IP_MIX2:
                    break;
                case GLB_AHB_CLOCK_IP_AUDIO:
                    tmpValCfg2 &= ~(1 << 21);
                    break;
                case GLB_AHB_CLOCK_IP_SDH:
                    tmpValCfg2 &= ~(1 << 22);
                    break;
                case GLB_AHB_CLOCK_IP_ZB2_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_ZB2_LP:
                    break;
                case GLB_AHB_CLOCK_IP_I2C1:
                    tmpValCfg1 &= ~(1 << 25);
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_MAC_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PLATFORM:
                    break;
                case GLB_AHB_CLOCK_IP_LZ4:
                    break;
                case GLB_AHB_CLOCK_IP_AUPDM:
                    break;
                case GLB_AHB_CLOCK_IP_GAUGE:
                    break;
                case GLB_AHB_CLOCK_IP_DBI:
                    tmpValCfg1 &= ~(1 << 24);
                    break;
                case GLB_AHB_CLOCK_IP_PEC:
                    tmpValCfg2 &= ~(1 << 25);
                    break;
                case GLB_AHB_CLOCK_IP_ISO11898:
                    tmpValCfg1 &= ~(1 << 26);
                    break;
                case GLB_AHB_CLOCK_IP_AUSOLO_TOP:
                    tmpValCfg1 &= ~(1 << 28);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_GPIO:
                    tmpValCfg1 &= ~(1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_MM_MISC:
                    tmpValCfg2 &= ~(1 << 16);
                    break;
                default:
                    break;
            }
        }
    }
    writel(tmpValCfg0, GLB_BASE + GLB_CGEN_CFG0_OFFSET);
    writel(tmpValCfg1, GLB_BASE + GLB_CGEN_CFG1_OFFSET);
    writel(tmpValCfg2, GLB_BASE + GLB_CGEN_CFG2_OFFSET);

    return SUCCESS;
}

int GLB_PER_Clock_UnGate(uint64_t ips)
{
    /* api request from cjy */

    uint32_t tmpValCfg0 = 0;
    uint32_t tmpValCfg1 = 0;
    uint32_t tmpValCfg2 = 0;
    uint32_t bitfield = 0;

    tmpValCfg0 = readl(GLB_BASE + GLB_CGEN_CFG0_OFFSET);
    tmpValCfg1 = readl(GLB_BASE + GLB_CGEN_CFG1_OFFSET);
    tmpValCfg2 = readl(GLB_BASE + GLB_CGEN_CFG2_OFFSET);
    while (ips) {
        if (SUCCESS == GLB_Get_And_Clr_First_Set_From_U64(&ips, &bitfield)) {
            switch (bitfield) {
                case GLB_AHB_CLOCK_IP_CPU:
                    tmpValCfg0 |= (1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_RSVD:
                    tmpValCfg0 |= (1 << 1);
                    break;
                case GLB_AHB_CLOCK_IP_SEC:
                    tmpValCfg0 |= (1 << 2);
                    tmpValCfg1 |= (1 << 3);
                    tmpValCfg1 |= (1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_0:
                    tmpValCfg0 |= (1 << 3);
                    tmpValCfg1 |= (1 << 12);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_1:
                    break;
                case GLB_AHB_CLOCK_IP_DMA_2:
                    break;
                case GLB_AHB_CLOCK_IP_CCI:
                    tmpValCfg0 |= (1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_RF_TOP:
                    break;
                case GLB_AHB_CLOCK_IP_GPIP:
                    tmpValCfg1 |= (1 << 2);
                    break;
                case GLB_AHB_CLOCK_IP_TZC:
                    tmpValCfg1 |= (1 << 5);
                    break;
                case GLB_AHB_CLOCK_IP_EF_CTRL:
                    tmpValCfg1 |= (1 << 7);
                    break;
                case GLB_AHB_CLOCK_IP_SF_CTRL:
                    tmpValCfg1 |= (1 << 11);
                    break;
                case GLB_AHB_CLOCK_IP_EMAC:
                    tmpValCfg2 |= (1 << 23);
                    break;
                case GLB_AHB_CLOCK_IP_UART0:
                    tmpValCfg1 |= (1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_UART1:
                    tmpValCfg1 |= (1 << 17);
                    break;
                case GLB_AHB_CLOCK_IP_UART2:
                    break;
                case GLB_AHB_CLOCK_IP_UART3:
                    break;
                case GLB_AHB_CLOCK_IP_UART4:
                    break;
                case GLB_AHB_CLOCK_IP_SPI:
                    tmpValCfg1 |= (1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_I2C:
                    tmpValCfg1 |= (1 << 19);
                    break;
                case GLB_AHB_CLOCK_IP_PWM:
                    tmpValCfg1 |= (1 << 20);
                    break;
                case GLB_AHB_CLOCK_IP_TIMER:
                    tmpValCfg1 |= (1 << 21);
                    break;
                case GLB_AHB_CLOCK_IP_IR:
                    tmpValCfg1 |= (1 << 22);
                    break;
                case GLB_AHB_CLOCK_IP_CHECKSUM:
                    tmpValCfg1 |= (1 << 23);
                    break;
                case GLB_AHB_CLOCK_IP_QDEC:
                    break;
                case GLB_AHB_CLOCK_IP_KYS:
                    break;
                case GLB_AHB_CLOCK_IP_I2S:
                    tmpValCfg1 |= (1 << 27);
                    break;
                case GLB_AHB_CLOCK_IP_USB11:
                    break;
                case GLB_AHB_CLOCK_IP_CAM:
                    break;
                case GLB_AHB_CLOCK_IP_MJPEG:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_LP:
                    break;
                case GLB_AHB_CLOCK_IP_ZB_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_ZB_LP:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_NORMAL:
                    tmpValCfg2 |= (1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_LP:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_2_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_2_LP:
                    break;
                case GLB_AHB_CLOCK_IP_EMI_MISC:
                    break;
                case GLB_AHB_CLOCK_IP_PSRAM0_CTRL:
                    break;
                case GLB_AHB_CLOCK_IP_PSRAM1_CTRL:
                    tmpValCfg2 |= (1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_USB20_SDU:
                    tmpValCfg1 |= (1 << 13);
                    break;
                case GLB_AHB_CLOCK_IP_MIX2:
                    break;
                case GLB_AHB_CLOCK_IP_AUDIO:
                    tmpValCfg2 |= (1 << 21);
                    break;
                case GLB_AHB_CLOCK_IP_SDH:
                    tmpValCfg2 |= (1 << 22);
                    break;
                case GLB_AHB_CLOCK_IP_ZB2_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_ZB2_LP:
                    break;
                case GLB_AHB_CLOCK_IP_I2C1:
                    tmpValCfg1 |= (1 << 25);
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_MAC_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PLATFORM:
                    break;
                case GLB_AHB_CLOCK_IP_LZ4:
                    break;
                case GLB_AHB_CLOCK_IP_AUPDM:
                    break;
                case GLB_AHB_CLOCK_IP_GAUGE:
                    break;
                case GLB_AHB_CLOCK_IP_DBI:
                    tmpValCfg1 |= (1 << 24);
                    break;
                case GLB_AHB_CLOCK_IP_PEC:
                    tmpValCfg2 |= (1 << 25);
                    break;
                case GLB_AHB_CLOCK_IP_ISO11898:
                    tmpValCfg1 |= (1 << 26);
                    break;
                case GLB_AHB_CLOCK_IP_AUSOLO_TOP:
                    tmpValCfg1 |= (1 << 28);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_GPIO:
                    tmpValCfg1 |= (1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_MM_MISC:
                    tmpValCfg2 |= (1 << 16);
                    break;
                default:
                    break;
            }
        }
    }
    writel(tmpValCfg0, GLB_BASE + GLB_CGEN_CFG0_OFFSET);
    writel(tmpValCfg1, GLB_BASE + GLB_CGEN_CFG1_OFFSET);
    writel(tmpValCfg2, GLB_BASE + GLB_CGEN_CFG2_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_PLL_CGEN_Clock_Gate(uint8_t clk)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CGEN_CFG3_OFFSET);
    tmpVal &= (~(1 << clk));
    writel(tmpVal, GLB_BASE + GLB_CGEN_CFG3_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_PLL_CGEN_Clock_UnGate(uint8_t clk)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CGEN_CFG3_OFFSET);
    tmpVal |= (1 << clk);
    writel(tmpVal, GLB_BASE + GLB_CGEN_CFG3_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Set_PKA_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_PKA_CLK_SEL, clkSel);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_MCU_SW_System_Reset(uint8_t sysPart)
{
    uint32_t tmpVal = 0;

    /* reset=0 */
    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal &= ~(1 << sysPart);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    DRV_DUMMY_ACT;
    /* reset=1 */
    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal |= (1 << sysPart);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    DRV_DUMMY_ACT;
    /* reset=1 */
    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal &= ~(1 << sysPart);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_SW_System_Reset(void)
{
    uint32_t tmpVal;

    /* Swicth MCU clock to RC32M, no divider */
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    GLB_Set_MCU_System_CLK_Div(0, 0);

    /* reset M0/SYS(bus) */
    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_CPU_RESET);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_CTRL_CPU_RESET);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    /* waiting for reset */
    while (1) {
        arch_delay_us(10);
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_SW_CPU_Reset(void)
{
    uint32_t tmpVal;

#if defined(DUAL_CORE)
    uint8_t core = GLB_CORE_ID_INVALID;
    /* Do reset */
    core = GLB_Get_Core_Type();
    switch (core) {
        case GLB_CORE_ID_M0:
#endif
            /* Swicth MCU clock to RC32M, no divider */
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
            GLB_Set_MCU_System_CLK_Div(0, 0);
            tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_CPU_RESET);
            writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);
            tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
            tmpVal = reg_set_bit(tmpVal, GLB_REG_CTRL_CPU_RESET);
            writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);
            tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
            tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_CPU_RESET);
            writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);
#if defined(DUAL_CORE)
            break;
        default:
            break;
    }
#endif

    /* waiting for reset */
    while (1) {
        arch_delay_us(10);
    }

    return SUCCESS;
}

int ATTR_TCM_SECTION GLB_SW_POR_Reset(void)
{
    uint32_t tmpVal;

    /* Swicth MCU clock to RC32M, no divider */
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    GLB_Set_MCU_System_CLK_Div(0, 0);

    /* Do reset */
    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_PWRON_RST);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_CTRL_PWRON_RST);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_CTRL_PWRON_RST);
    writel(tmpVal, GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    /* waiting for reset */
    while (1) {
        arch_delay_us(10);
    }

    return SUCCESS;
}

int GLB_Set_Auto_Calc_Xtal_Type(uint8_t calcXtalType)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_HW_RSV0_OFFSET);
    tmpVal = tmpVal & 0xffff0000;
    tmpVal |= GLB_AUTO_CALC_XTAL_FLAG_VALUE;
    tmpVal |= calcXtalType;
    writel(tmpVal, GLB_BASE + GLB_HW_RSV0_OFFSET);

    return SUCCESS;
}

int GLB_Get_Auto_Calc_Xtal_Type(uint8_t *calcXtalType)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_HW_RSV0_OFFSET);
    if ((tmpVal & GLB_AUTO_CALC_XTAL_FLAG_MASK) == GLB_AUTO_CALC_XTAL_FLAG_VALUE) {
        *calcXtalType = (tmpVal & 0xff);
        return SUCCESS;
    } else {
        *calcXtalType = 0;
        return ERROR;
    }
}

int GLB_Set_Flash_Id_Value(uint32_t idValue)
{
    writel(((idValue&0xFFFFFF)|0x5A000000), GLB_BASE + GLB_HW_RSV1_OFFSET);

    return SUCCESS;
}

uint32_t GLB_Get_Flash_Id_Value(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_HW_RSV1_OFFSET);
    if ((tmpVal&0x7F000000) == 0x5A000000) {
        return (tmpVal&0x00FFFFFF);
    }

    return 0x00000000;
}

int ATTR_TCM_SECTION GLB_Trim_Ldo18ioVoutSel(void)
{
    Efuse_Ana_Ldo18ioVoutSel_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Ldo18ioVoutSel_Trim(&trim);
    if (trim.ldo18ioVoutSelEn) {
        if (trim.ldo18ioVoutSelParity == EF_Ctrl_Get_Trim_Parity(trim.ldo18ioVoutSelAon, 4)) {
            tmpVal = readl(GLB_BASE + GLB_LDO18IO_OFFSET);
            tmpVal = reg_set_bits(tmpVal, GLB_LDO18IO_VOUT_SEL, trim.ldo18ioVoutSelAon);
            writel(tmpVal, GLB_BASE + GLB_LDO18IO_OFFSET);
            return SUCCESS;
        }
    }

    return ERROR;
}

int ATTR_TCM_SECTION GLB_Trim_Ldo18ioBypass(void)
{
    Efuse_Ana_Ldo18ioBypass_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Ldo18ioBypass_Trim(&trim);
    if (trim.ldo18ioBypassEn) {
        if (trim.ldo18ioBypassParity == EF_Ctrl_Get_Trim_Parity(trim.ldo18ioBypassAon, 1)) {
            tmpVal = readl(GLB_BASE + GLB_LDO18IO_OFFSET);
            tmpVal = reg_set_bits(tmpVal, GLB_LDO18IO_BYPASS, trim.ldo18ioBypassAon);
            writel(tmpVal, GLB_BASE + GLB_LDO18IO_OFFSET);
            return SUCCESS;
        }
    }

    return ERROR;
}

int ATTR_TCM_SECTION GLB_Trim_Ldo18ioVoutTrim(void)
{
    Efuse_Ana_Ldo18ioVoutTrim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Ldo18ioVoutTrim_Trim(&trim);
    if (trim.ldo18ioVoutTrimEn) {
        if (trim.ldo18ioVoutTrimParity == EF_Ctrl_Get_Trim_Parity(trim.ldo18ioVoutTrimAon, 4)) {
            tmpVal = readl(GLB_BASE + GLB_LDO18IO_OFFSET);
            tmpVal = reg_set_bits(tmpVal, GLB_LDO18IO_VOUT_TRIM, trim.ldo18ioVoutTrimAon);
            writel(tmpVal, GLB_BASE + GLB_LDO18IO_OFFSET);
            return SUCCESS;
        }
    }

    return ERROR;
}

void ATTR_TCM_SECTION GLB_Power_Down_Ldo18ioVout(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_LDO18IO_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_PU_LDO18IO);
    writel(tmpVal, GLB_BASE + GLB_LDO18IO_OFFSET);
}

int GLB_Set_SRAM_RET(uint32_t value)
{
    writel(value, GLB_BASE + GLB_SRAM_CFG0_OFFSET);

    return SUCCESS;
}

uint32_t GLB_Get_SRAM_RET(void)
{
    return readl(GLB_BASE + GLB_SRAM_CFG0_OFFSET);
}

int GLB_Set_SRAM_SLP(uint32_t value)
{
    writel(value, GLB_BASE + GLB_SRAM_CFG1_OFFSET);

    return SUCCESS;
}

uint32_t GLB_Get_SRAM_SLP(void)
{
    return readl(GLB_BASE + GLB_SRAM_CFG1_OFFSET);
}

int GLB_Set_SRAM_PARM(uint32_t value)
{
    writel(value, GLB_BASE + GLB_SRAM_CFG2_OFFSET);

    return SUCCESS;
}

uint32_t GLB_Get_SRAM_PARM(void)
{
    return readl(GLB_BASE + GLB_SRAM_CFG2_OFFSET);
}

int GLB_Set_EM_Sel(uint8_t emType)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_SRAM_CFG3_OFFSET);
    switch (emType) {
        case GLB_WRAM160KB_EM0KB:
            tmpVal = reg_set_bits(tmpVal, GLB_EM_SEL, 0x00);
            break;
        case GLB_WRAM128KB_EM32KB:
            tmpVal = reg_set_bits(tmpVal, GLB_EM_SEL, 0x03);
            break;
        case GLB_WRAM96KB_EM64KB:
            tmpVal = reg_set_bits(tmpVal, GLB_EM_SEL, 0x0F);
            break;
        default:
            tmpVal = reg_set_bits(tmpVal, GLB_EM_SEL, 0x03);
            break;
    }
    writel(tmpVal, GLB_BASE + GLB_SRAM_CFG3_OFFSET);

    return SUCCESS;
}

int GLB_Set_PSRAMB_CLK_Sel(uint8_t enable, GLB_PSRAMB_PLL_Type clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_PSRAM_CFG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    writel(tmpVal, GLB_BASE + GLB_PSRAM_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_PSRAM_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_PSRAMB_CLK_SEL, clkSel);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_PSRAMB_CLK_DIV, div);
    writel(tmpVal, GLB_BASE + GLB_PSRAM_CFG0_OFFSET);

    tmpVal = readl(GLB_BASE + GLB_PSRAM_CFG0_OFFSET);
    if (enable) {
        tmpVal = reg_set_bit(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    }
    writel(tmpVal, GLB_BASE + GLB_PSRAM_CFG0_OFFSET);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Set_Slave_Grp_0_CLK(GLB_SLAVE_GRP_0_Type slave, uint8_t enable, GLB_SLAVE_GRP_0_CLK_SEL_Type clkSel, uint32_t div)
{
    uint32_t tmpVal;
    uint32_t regAddr = GLB_BASE + glb_slave_grp_0_table[slave].clkOffSetAddr;
    uint8_t enPos = glb_slave_grp_0_table[slave].clkEnPos;
    uint8_t selPos = glb_slave_grp_0_table[slave].clkSelPos;
    uint8_t divPos = glb_slave_grp_0_table[slave].clkDivPos;
    uint8_t enLen = glb_slave_grp_0_table[slave].clkEnLen;
    uint8_t selLen = glb_slave_grp_0_table[slave].clkSelLen;
    uint8_t divLen = glb_slave_grp_0_table[slave].clkDivLen;

    /* disable clock first */
    if (enLen) {
        tmpVal = readl(regAddr);
        tmpVal &= ~(((1ULL << enLen) - 1) << enPos);
        writel(tmpVal, regAddr);
    }

    /* set clock sel or div */
    tmpVal = readl(regAddr);
    if (selLen) {
        tmpVal &= ~(((1ULL << selLen) - 1) << selPos);
        tmpVal |= (((clkSel.clkSel) & ((1 << selLen) - 1)) << selPos);
    }
    if (divLen) {
        tmpVal &= ~(((1ULL << divLen) - 1) << divPos);
        tmpVal |= ((div & ((1 << divLen) - 1)) << divPos);
    }
    writel(tmpVal, regAddr);

    /* enable clock */
    if (enLen && enable) {
        tmpVal = readl(regAddr);
        tmpVal |= (((1ULL << enLen) - 1) << enPos);
        writel(tmpVal, regAddr);
    }

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Config_WIFI_PLL(uint8_t xtalType, const GLB_WA_PLL_Cfg_Type * pllCfgList)
{
    uint8_t refClk;

    if (xtalType == GLB_XTAL_RC32M) {
        refClk = GLB_PLL_REFCLK_RC32M;
    } else {
        refClk = GLB_PLL_REFCLK_XTAL;
    }

    GLB_Power_Off_WIFIPLL();
    GLB_WIFIPLL_Ref_Clk_Sel(refClk);
    GLB_Power_On_WIFIPLL(&(pllCfgList[xtalType]), 1);

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Simple_Set_MCU_System_CLK(uint8_t clkFreq, uint8_t mcuClkDiv, uint8_t mcuPBclkDiv)
{
    uint32_t tmpVal;
    uint32_t timeout;

    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);

    /* select pll output clock before select root clock */
    tmpVal = readl(PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_TOP_AUPLL_DIV2:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 0);
            break;
        case GLB_MCU_SYS_CLK_TOP_AUPLL_DIV1:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 1);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 2);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 3);
            break;
        default:
            break;
    }
    writel(tmpVal, PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);

    /* clock div */
    /* config hclk_div=mcuClkDiv */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_HCLK_DIV, mcuClkDiv);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);
    /* config bclk_div=mcuPBclkDiv */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV, mcuPBclkDiv);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);
    /* bclk act pulse */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV_ACT_PULSE, 1);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG1_OFFSET);
    timeout = 1024;
    do {
        tmpVal = readl(GLB_BASE + GLB_SYS_CFG1_OFFSET);
        tmpVal = reg_get_bits(tmpVal, GLB_STS_BCLK_PROT_DONE);
    } while ((--timeout) && (!tmpVal));
    if (!timeout) {
        return ERROR;
    }

    /* select root clock */
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_RC32M:
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
            break;
        case GLB_MCU_SYS_CLK_XTAL:
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
            break;
        case GLB_MCU_SYS_CLK_TOP_AUPLL_DIV2:
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_AUPLL_DIV2);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        case GLB_MCU_SYS_CLK_TOP_AUPLL_DIV1:
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_AUPLL_DIV1);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_240M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_320M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        default:
            break;
    }

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

void glb_40M_delay_us(uint32_t us)
{
    for (uint32_t i = 0; i < us; i++) {
        GLB_CLK_SET_DUMMY_WAIT;
        GLB_CLK_SET_DUMMY_WAIT;
        GLB_CLK_SET_DUMMY_WAIT;
    }
}

int ATTR_CLOCK_SECTION GLB_Fast_Power_On_WIFIPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable)
{
    uint32_t REG_PLL_BASE_ADDRESS = 0;
    uint32_t tmpVal = 0;

    REG_PLL_BASE_ADDRESS = CCI_BASE + CCI_WIFI_PLL_CFG0_OFFSET;

    /* Step1:config parameter */
    /* cfg1:Set wifipll_refclk_sel and wifipll_refdiv_ratio */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 1);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_REFDIV_RATIO, cfg->basicCfg->clkpllRefdivRatio);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 1);

    /* cfg2:Set wifipll_int_frac_sw,wifipll_icp_1u,wifipll_icp_5u */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 2);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_INT_FRAC_SW, cfg->basicCfg->clkpllIntFracSw);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_ICP_1U, cfg->basicCfg->clkpllIcp1u);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_ICP_5U, cfg->basicCfg->clkpllIcp5u);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 2);

    /* cfg3:Set wifipll_rz,wifipll_cz,wifipll_c3,wifipll_r4_short,wifipll_r4_en */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 3);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_RZ, cfg->basicCfg->clkpllRz);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_CZ, cfg->basicCfg->clkpllCz);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_C3, cfg->basicCfg->clkpllC3);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_R4_SHORT, cfg->basicCfg->clkpllR4Short);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_C4_EN, cfg->basicCfg->clkpllC4En);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 3);

    /* cfg4:Set wifipll_sel_sample_clk */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 4);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SEL_SAMPLE_CLK, cfg->basicCfg->clkpllSelSampleClk);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 4);

    /* cfg5:Set wifipll_vco_speed */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 5);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_VCO_SPEED, cfg->basicCfg->clkpllVcoSpeed);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 5);

    /* cfg6:Set wifipll_sdm_bypass,wifipll_sdmin */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 6);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_CTRL_HW, cfg->basicCfg->clkpllSdmCtrlHw);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_BYPASS, cfg->basicCfg->clkpllSdmBypass);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDMIN, cfg->clkpllSdmin);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 6);

    /* cfg10:always set usbpll_sdmin */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 10);
    tmpVal = reg_set_bits(tmpVal, CCI_USBPLL_SDMIN, usbPllSdmin_12M);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 10);

    /* cfg12:always set sscdiv_sdmin */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 12);
    tmpVal = reg_set_bits(tmpVal, CCI_SSCDIV_SDMIN, sscDivSdmin_24M);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 12);

    /* Step2:config pu */
    /* cfg0 : pu_wifipll_sfreg=1 */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_PU_WIFIPLL_SFREG, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* delay > 2us */
    glb_40M_delay_us(3);

    /* cfg0 : pu_wifipll=1 */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_PU_WIFIPLL, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* delay > 2us */
    glb_40M_delay_us(3);

    /* toggle sdm_reset (pulse 0 > 1us) */
    /* cfg0 : wifipll_sdm_reset */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    glb_40M_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_RSTB, 0);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    glb_40M_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_SDM_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* Step3:reset pll */
    /* cfg0 : toggle wifipll_reset_fbdv, pulse 0 > 1us */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_FBDV_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    glb_40M_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_FBDV_RSTB, 0);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);
    glb_40M_delay_us(2);
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 0);
    tmpVal = reg_set_bits(tmpVal, CCI_WIFIPLL_FBDV_RSTB, 1);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 0);

    /* Step4:enable output clock */
    /* cfg8 : wifipll clock enable */
    tmpVal = readl(REG_PLL_BASE_ADDRESS + 4 * 8);
    tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV3);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV4);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV5);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV6);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV8);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV10);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV12);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV20);
    // tmpVal = reg_set_bit(tmpVal, CCI_WIFIPLL_EN_DIV30);
    writel(tmpVal, REG_PLL_BASE_ADDRESS + 4 * 8);

    if (waitStable) {
        /* Wait 1.5*30us    */
        glb_40M_delay_us(45);
    }

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Fast_Power_On_XTAL_40M_And_WIFIPLL(void)
{
    uint32_t tmpVal;
    volatile uint8_t refClk;

    refClk = GLB_PLL_REFCLK_XTAL;

    /* power on xtal first */
    tmpVal = readl(AON_BASE + AON_TSEN_OFFSET);
    if (!reg_is_bit_set(tmpVal, AON_XTAL_RDY) ){
        AON_Power_On_XTAL();
    }

    // HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    //HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);

    //HBN_Set_Xtal_Type(GLB_XTAL_40M);

    /* power on wifipll */
    GLB_Power_Off_WIFIPLL();
    GLB_WIFIPLL_Ref_Clk_Sel(refClk);
    GLB_Fast_Power_On_WIFIPLL(&wifiPllCfg_960M_Fast[0], 0);

    glb_40M_delay_us(30);

    /* if power on xtal, always set xclk from xtal */
    // HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);

    /* enable all PLL clock output */
    /* GLB reg_pll_en = 1, cannot be zero */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GLB_REG_PLL_EN);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Fast_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv)
{
    uint32_t tmpVal;
    uint32_t timeout = 1024;

    /* config hclk_div=mcuClkDiv */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_HCLK_DIV, mcuClkDiv);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);

    /* config bclk_div=mcuPBclkDiv */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV, mcuPBclkDiv);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG0_OFFSET);
    /* bclk act pulse */
    tmpVal = readl(GLB_BASE + GLB_SYS_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_REG_BCLK_DIV_ACT_PULSE, 1);
    writel(tmpVal, GLB_BASE + GLB_SYS_CFG1_OFFSET);

    timeout = 1024;
    do {
        tmpVal = readl(GLB_BASE + GLB_SYS_CFG1_OFFSET);
        tmpVal = reg_get_bits(tmpVal, GLB_STS_BCLK_PROT_DONE);
    } while ((--timeout) && (!tmpVal));
    if (!timeout) {
        return ERROR;
    }

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

int ATTR_CLOCK_SECTION GLB_Fast_Set_MCU_System_CLK(uint8_t clkFreq)
{
    uint32_t tmpVal;

    //
    /* get xclk&&rootclk clock */
    // mcuXclkSel = HBN_Get_MCU_XCLK_Sel();

    // /* change root clock to rc32m */
    // HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    // HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    // GLB_Set_MCU_System_CLK_Div(0, 0);

    /* select pll output clock before select root clock */
    tmpVal = readl(PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);
#if 0
    switch (clkFreq) {
        // case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
        //     tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 2);
        //     break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 3);
            break;
        default:
            break;
    }
#else
    tmpVal = reg_set_bits(tmpVal, PDS_REG_PLL_SEL, 3);
#endif
    writel(tmpVal, PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);

#if 0
    /* select root clock */
    switch (clkFreq) {
        // case GLB_MCU_SYS_CLK_RC32M:
        //     GLB_Set_MCU_System_CLK_Div(0, 0);
        //     HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
        //     HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
        //     break;
        // case GLB_MCU_SYS_CLK_XTAL:
        //     GLB_Set_MCU_System_CLK_Div(0, 0);
        //     HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
        //     HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
        //     break;
        // case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
        //     /* For high speed, set DIV first */
        //     GLB_Set_MCU_System_CLK_Div(0, 2);
        //     /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
        //     /* MCU_MISC_IROM_2T_Access_Set(0); */
        //     /* unuseful for mcu, useful for dsp, just for safe */
        //     GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_240M);
        //     HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
        //     /* recover xclk */
        //     HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
        //     break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(3, 0);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_320M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            // HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        default:
            break;
    }
#else
    /* For high speed, set DIV first */
    GLB_Fast_Set_MCU_System_CLK_Div(3, 0);
    /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
    /* MCU_MISC_IROM_2T_Access_Set(0); */
    //GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_320M);
    writel(readl(0x2000058C)|(1<<14), 0x2000058C);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
    writel(readl(0x2000f030)|(1<<1), 0x2000f030);

#endif

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

uint8_t ATTR_TCM_SECTION GLB_Get_Package_Type(void)
{
    uint32_t tmpVal = 0;
    uint8_t package_type = 0;

    /* get device_info[1:0] from efuse */
    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_WIFI_MAC_HIGH_OFFSET);

    package_type = (uint8_t)((tmpVal >> 22) & 0x3);

    return package_type;
}

int ATTR_TCM_SECTION GLB_Get_PAD_Bonging_to_GND_Sts(void)
{
    uint32_t tmpVal = 0;
    uint8_t package_cfg = 0;

    /* get package_cfg[2:0] from efuse */
    tmpVal = readl(EF_DATA_BASE + EF_DATA_EF_KEY_SLOT_10_W0_OFFSET);

    package_cfg = (uint8_t)((tmpVal >> 28) & 0x7);

    if (0 == package_cfg) {
        return RESET;
    } else {
        return SET;
    }
}

