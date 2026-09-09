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

static uint32_t ATTR_CLOCK_SECTION Clock_Get_WIFI_PLL_Output(uint32_t pllOut)
{
    return pllOut;
}

static uint32_t ATTR_CLOCK_SECTION Clock_Xtal_Output(void)
{
    /* ipro6 only ever uses a 40 MHz crystal, so report it directly instead of
     * decoding the type from HBN_RSV3. This frees HBN_RSV3 to be used as a
     * scratch register (e.g. the boot2 USB-download flag). */
    return 40 * 1000 * 1000;
}

static uint32_t ATTR_CLOCK_SECTION Clock_XClk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* rc32m */
        return (32 * 1000 * 1000);
    } else if (sel == 1) {
        /* xtal */
        return Clock_Xtal_Output();
    } else {
        return 0;
    }
}

static uint32_t ATTR_CLOCK_SECTION Clock_F32k_Mux_Output(uint8_t sel)
{
    uint32_t tmpVal;
    uint32_t div = 0;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);
    div = reg_get_bits(tmpVal, GLB_DIG_32K_DIV);

    if (sel == 0) {
        /* src32k */
        return (32 * 1000);
    } else if (sel == 1) {
        /* xtal 32K */
        return (32 * 1000);
    } else {
        return Clock_Xtal_Output() / (div + 1);
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_F32k_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);

    return reg_get_bits(tmpVal, HBN_F32K_SEL);
}

static uint32_t ATTR_CLOCK_SECTION Clock_160M_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 160m */
        return Clock_Get_WIFI_PLL_Output(160 * 1000 * 1000);
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_Muxpll_160M_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_TOP_MUXPLL_160M_SEL);
}

static uint32_t ATTR_CLOCK_SECTION Clock_80M_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 80m */
        return Clock_Get_WIFI_PLL_Output(80 * 1000 * 1000);
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_Muxpll_80M_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_TOP_MUXPLL_80M_SEL);
}

static uint32_t ATTR_CLOCK_SECTION Clock_MCU_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 2) {
        /* wifi pll 240m */
        return Clock_Get_WIFI_PLL_Output(240 * 1000 * 1000);
    } else if (sel == 3) {
        /* wifi pll 320m */
        return Clock_Get_WIFI_PLL_Output(320 * 1000 * 1000);
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_MCU_XClk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_get_bits(tmpVal, HBN_ROOT_CLK_SEL);

    return (tmpVal & 0x1);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_MCU_Root_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_get_bits(tmpVal, HBN_ROOT_CLK_SEL);

    return ((tmpVal >> 1) & 0x1);
}

static uint32_t ATTR_CLOCK_SECTION Clock_MCU_Root_Clk_Mux_Output(uint8_t sel)
{
    uint32_t tmpVal;

    if (sel == 0) {
        /* xclk */
        return Clock_XClk_Mux_Output(Clock_Get_MCU_XClk_Sel_Val());
    } else if (sel == 1) {
        /* pll */
        tmpVal = readl(PDS_BASE + PDS_CPU_CORE_CFG1_OFFSET);
        sel = reg_get_bits(tmpVal, PDS_REG_PLL_SEL);
        return Clock_MCU_Clk_Mux_Output(sel);
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_MCU_HClk_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_HCLK_DIV);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_Peri_BClk_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_SYS_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_BCLK_DIV);
}

uint32_t ATTR_CLOCK_SECTION Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_Type type)
{
    uint32_t clock = 0;
    uint32_t div = 0;

    switch (type) {
        case DRV_SYSTEM_CLOCK_MCU_ROOT_CLK:
            /*!< mcu root clock */
            return Clock_MCU_Root_Clk_Mux_Output(Clock_Get_MCU_Root_Clk_Sel_Val());

        case DRV_SYSTEM_CLOCK_MCU_CLK:
            /*!< mcu fast clock/cpu clock */
            clock = Clock_MCU_Root_Clk_Mux_Output(Clock_Get_MCU_Root_Clk_Sel_Val());
            div = Clock_Get_MCU_HClk_Div_Val();
            return clock / (div + 1);

        case DRV_SYSTEM_CLOCK_MCU_PBCLK:
            /*!< mcu peri bus clock */
            clock = Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK);
            div = Clock_Get_Peri_BClk_Div_Val();
            return clock / (div + 1);

        case DRV_SYSTEM_CLOCK_F32K:
            /*!< f32k clock */
            return Clock_F32k_Mux_Output(Clock_Get_F32k_Sel_Val());

        case DRV_SYSTEM_CLOCK_XCLK:
            /*!< xclk:rc32M or xtal */
            return Clock_XClk_Mux_Output(Clock_Get_MCU_XClk_Sel_Val());

        case DRV_SYSTEM_CLOCK_XTAL:
            /*!< xtal clock */
            return Clock_Xtal_Output();

        default:
            return 0;
    }
}

static uint32_t ATTR_CLOCK_SECTION Clock_PARAMB_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 320m */
        return Clock_Get_WIFI_PLL_Output(320 * 1000 * 1000);
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_PSRAMB_Clk_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_PSRAM_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_PSRAMB_CLK_DIV);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_PSRAMB_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_PSRAM_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_PSRAMB_CLK_SEL);
}

uint32_t ATTR_CLOCK_SECTION Clock_Get_PSRAMB_Clk(void)
{
    uint32_t div = 0;

    div = Clock_Get_PSRAMB_Clk_Div_Val();

    return Clock_PARAMB_Clk_Mux_Output(Clock_Get_PSRAMB_Clk_Sel_Val()) / (div + 1);
}

static uint32_t ATTR_CLOCK_SECTION Clock_ISP_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 80m */
        return Clock_Get_WIFI_PLL_Output(80 * 1000 * 1000);
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_ISP_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG1_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_ISP_MUXPLL_80M_SEL);
}

uint32_t ATTR_CLOCK_SECTION Clock_Get_ISP_Clk(void)
{
    return Clock_ISP_Clk_Mux_Output(Clock_Get_ISP_Clk_Sel_Val());
}

static uint32_t ATTR_CLOCK_SECTION Clock_UART_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* mcu bclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK);
    } else if (sel == 1) {
        /* mux 160Mkz */
        return Clock_160M_Clk_Mux_Output(Clock_Get_Muxpll_160M_Sel_Val());
    } else {
        /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_UART_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_UART_CFG0_OFFSET);

    return ((reg_get_bits(tmpVal, GLB_HBN_UART_CLK_SEL2) << 1) | reg_get_bits(tmpVal, GLB_HBN_UART_CLK_SEL));
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_UART_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_UART_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_UART_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_PKA_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* mcu pbclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK);
    } else if (sel == 1) {
        /* mux 160m */
        return Clock_160M_Clk_Mux_Output(Clock_Get_Muxpll_160M_Sel_Val());
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_PKA_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_SWRST_CFG2_OFFSET);

    return reg_get_bits(tmpVal, GLB_PKA_CLK_SEL);
}

static uint32_t ATTR_CLOCK_SECTION Clock_IR_Clk_Mux_Output(void)
{
    /* IR not supported on IPRO6 */
    return 0;
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_IR_Div_Val(void)
{
    /* IR not supported on IPRO6 */
    return 0;
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_SF_Clk_Sel2_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_SF_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_SF_CLK_SEL2);
}

static uint32_t ATTR_CLOCK_SECTION Clock_SF_SEL2_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 120m */
        return Clock_Get_WIFI_PLL_Output(120 * 1000 * 1000);
    } else if (sel == 1) {
        /* xtal */
        return Clock_Xtal_Output();
    } else if (sel == 2) {
        /* wifi pll 120m */
        return Clock_Get_WIFI_PLL_Output(120 * 1000 * 1000);
    } else {
        return 0;
    }
}

static uint32_t ATTR_CLOCK_SECTION Clock_SF_SEL_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* sf sel2 */
        return Clock_SF_SEL2_Clk_Mux_Output(Clock_Get_SF_Clk_Sel2_Val());
    } else if (sel == 1) {
        /* mux 80m */
        return Clock_80M_Clk_Mux_Output(Clock_Get_Muxpll_80M_Sel_Val());
    } else if (sel == 2) {
        /* mcu pbclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK);
    } else {
        /* wifi 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_SF_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_SF_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_SF_CLK_SEL);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_SF_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_SF_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_SF_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_SPI_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* mux 160Mkz */
        return Clock_160M_Clk_Mux_Output(Clock_Get_Muxpll_160M_Sel_Val());
    } else {
        /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_SPI_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_SPI_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_SPI_CLK_SEL);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_SPI_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_SPI_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_SPI_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_DBI_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* mux 160Mkz */
        return Clock_160M_Clk_Mux_Output(Clock_Get_Muxpll_160M_Sel_Val());
    } else {
        /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_DBI_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DBI_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_DBI_CLK_SEL);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_DBI_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DBI_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_DBI_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_I2C_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* mcu pblck */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK);
    } else if (sel == 1) {
        /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_I2C_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_I2C_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_I2C_CLK_SEL);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_I2C_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_I2C_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_I2C_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_GPADC_Clk_Output(uint8_t sel)
{
    if (sel == 1) {
        /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_GPADC_32M_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_ADC_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_GPADC_32M_CLK_SEL);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_GPADC_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_ADC_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_GPADC_32M_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_GPADC_Clk_Mux_Output(uint8_t sel)
{
    uint32_t div = 0;

    if (sel == 1) {
        /* f32k bclk */
        return Clock_F32k_Mux_Output(Clock_Get_F32k_Sel_Val());
    } else {
        div = Clock_Get_GPADC_Div_Val();
        return Clock_GPADC_Clk_Output(Clock_Get_GPADC_32M_Clk_Sel_Val()) / (div + 1);
    }
}

static uint32_t ATTR_CLOCK_SECTION Clock_I2S_Clk_Mux_Output(void)
{
    uint32_t postdiv;

    /* IPRO6S derives I2S_REF_CLK from the 960 MHz Wi-Fi PLL VCO and its
     * post-divider.  With postdiv=20 this is 48 MHz.  Returning zero here
     * makes I2S_SetBclkPeriod() underflow and stalls the TX FIFO/DMA path. */
    postdiv = reg_get_bits(readl(CCI_BASE + CCI_WIFI_PLL_CFG1_OFFSET),
                           CCI_WIFIPLL_POSTDIV);
    if (postdiv == 0u) {
        return 0u;
    }

    return Clock_Get_WIFI_PLL_Output(960u * 1000u * 1000u) / postdiv;
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_I2S_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_I2S_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_I2S_REF_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_ADC_Clk_Mux_Output(void)
{
    return 0;
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_ADC_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_AUDIO_CFG1_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_AUDIO_SOLO_CLK_DIV);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_GPADC_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_PIR_CFG_OFFSET);

    return reg_get_bits(tmpVal, HBN_GPADC_CS);
}

static uint32_t ATTR_CLOCK_SECTION Clock_GPDAC_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* wifi pll 32m */
        return Clock_Get_WIFI_PLL_Output(32 * 1000 * 1000);
    } else if (sel == 1) {
        /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_GPDAC_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_DIG_512K_DIV);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_GPDAC_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG_CLK_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_DIG_CLK_SRC_SEL);
}


static uint32_t ATTR_CLOCK_SECTION Clock_CAM_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    } else if (sel == 1) {
        /* wifi pull 96m */
        return Clock_Get_WIFI_PLL_Output(96 * 1000 * 1000);
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_CAM_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CAM_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_CAM_REF_CLK_SRC_SEL);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_CAM_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CAM_CFG0_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_CAM_REF_CLK_DIV);
}

uint32_t ATTR_CLOCK_SECTION Clock_Audio_ADC_Clock_Get(void)
{
    uint32_t clock = 0;
    uint32_t div = 0;

    clock = Clock_ADC_Clk_Mux_Output();
    div = Clock_Get_ADC_Div_Val();

    return clock / (div + 1);
}

uint32_t ATTR_CLOCK_SECTION Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_Type type)
{
    uint32_t clock = 0;
    uint32_t div = 0;

#ifdef CONFIG_FPGA
    return 32*1000*1000;
#endif

    switch (type) {
        /*!< uart0 clock */
        case DRV_PERIPHERAL_CLOCK_UART0:
            ATTR_FALLTHROUGH();

        /*!< uart1 clock */
        case DRV_PERIPHERAL_CLOCK_UART1:
            ATTR_FALLTHROUGH();

        /*!< uart2 clock */
        case DRV_PERIPHERAL_CLOCK_UART2:
            clock = Clock_UART_Clk_Mux_Output(Clock_Get_UART_Clk_Sel_Val());
            div = Clock_Get_UART_Div_Val();
            return clock / (div + 1);


        /*!< spi clock */
        case DRV_PERIPHERAL_CLOCK_SPI:
            clock = Clock_SPI_Clk_Mux_Output(Clock_Get_SPI_Clk_Sel_Val());
            div = Clock_Get_SPI_Div_Val();
            return clock / (div + 1);

        /*!< dbi clock */
        case DRV_PERIPHERAL_CLOCK_DBI:
            clock = Clock_DBI_Clk_Mux_Output(Clock_Get_DBI_Clk_Sel_Val());
            div = Clock_Get_DBI_Div_Val();
            return clock / (div + 1);

        /*!< i2c0 clock */
        case DRV_PERIPHERAL_CLOCK_I2C0:
            ATTR_FALLTHROUGH();

        /*!< i2c1 clock */
        case DRV_PERIPHERAL_CLOCK_I2C1:
            clock = Clock_I2C_Clk_Mux_Output(Clock_Get_I2C_Clk_Sel_Val());
            div = Clock_Get_I2C_Div_Val();
            return clock / (div + 1);

        /*!< PSRAMB clock */
        case DRV_PERIPHERAL_CLOCK_PSRAMB:
            return Clock_Get_PSRAMB_Clk();

        /*!< emi clock */
        case DRV_PERIPHERAL_CLOCK_EMI:
           /* mcu pblck */
            return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK);

        /*!< isp clock */
        case DRV_PERIPHERAL_CLOCK_ISP:
            return Clock_Get_ISP_Clk();

        /*!< FLASH clock */
        case DRV_PERIPHERAL_CLOCK_FLASH:
            clock = Clock_SF_SEL_Clk_Mux_Output(Clock_Get_SF_Clk_Sel_Val());
            div = Clock_Get_SF_Div_Val();
            return clock / (div + 1);

        /*!< I2S clock */
        case DRV_PERIPHERAL_CLOCK_I2S:
            clock = Clock_I2S_Clk_Mux_Output();
            div = Clock_Get_I2S_Div_Val();
            return clock / (div + 1);

        /*!< IR clock */
        case DRV_PERIPHERAL_CLOCK_IR:
            clock = Clock_IR_Clk_Mux_Output();
            div = Clock_Get_IR_Div_Val();
            return clock / (div + 1);

        /*!< ADC clock */
        case DRV_PERIPHERAL_CLOCK_ADC:
            return Clock_Audio_ADC_Clock_Get();

        /*!< GPADC clock */
        case DRV_PERIPHERAL_CLOCK_GPADC:
            return Clock_GPADC_Clk_Mux_Output(Clock_Get_GPADC_Clk_Sel_Val());

        /*!< GPDAC clock */
        case DRV_PERIPHERAL_CLOCK_GPDAC:
            clock = Clock_GPDAC_Clk_Mux_Output(Clock_Get_GPDAC_Clk_Sel_Val());
            div = Clock_Get_GPDAC_Div_Val();
            return clock / (div + 1);

        /*!< CAM clock */
        case DRV_PERIPHERAL_CLOCK_CAM:
            clock = Clock_CAM_Clk_Mux_Output(Clock_Get_CAM_Clk_Sel_Val());
            div = Clock_Get_CAM_Div_Val();
            return clock / (div + 1);


        /*!< PKA clock */
        case DRV_PERIPHERAL_CLOCK_PKA:
            return Clock_PKA_Clk_Mux_Output(Clock_Get_PKA_Clk_Sel_Val());

        default:
            return 0;
    }
}

uint32_t ATTR_CLOCK_SECTION Clock_Peripheral_Clock_Get_Uart(void)
{
    uint32_t clock = 0;
    uint32_t div = 0;
#ifdef CONFIG_FPGA
    return 32*1000*1000;
#endif
    clock = Clock_UART_Clk_Mux_Output(Clock_Get_UART_Clk_Sel_Val());
    div = Clock_Get_UART_Div_Val();
    return clock / (div + 1);
}

uint32_t ATTR_CLOCK_SECTION Clock_Peripheral_Clock_Get_Flash(void)
{
    uint32_t clock = 0;
    uint32_t div = 0;

    clock = Clock_SF_SEL_Clk_Mux_Output(Clock_Get_SF_Clk_Sel_Val());
    div = Clock_Get_SF_Div_Val();
    return clock / (div + 1);
}

uint32_t ATTR_CLOCK_SECTION SystemCoreClockGet(void)
{
    uint32_t clockVal = 0;

    switch (GLB_Get_Core_Type()) {
        case GLB_CORE_ID_M0:
            clockVal = Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK);
            break;
        default:
            clockVal = 0;
            break;
    }
    clockVal = clockVal ? clockVal : (32 * 1000 * 1000);

    return clockVal;
}

void ATTR_CLOCK_SECTION CPU_Set_MTimer_Toggle_Cnt(uint16_t cnt)
{
    uint32_t tmpVal;
    uint32_t address = MCU_MISC_BASE + MCU_MISC_MAIN_MCU_CFG2_OFFSET;

    tmpVal = readl(address);
    tmpVal = reg_set_bits(tmpVal, MCU_MISC_CR_MTIME_TOGGLE_CNT, cnt);
    writel(tmpVal, address);
}

uint16_t ATTR_CLOCK_SECTION CPU_Get_MTimer_Toggle_Cnt(void)
{
    uint32_t address = MCU_MISC_BASE + MCU_MISC_MAIN_MCU_CFG2_OFFSET;

    return (uint16_t)reg_get_bits(readl(address), MCU_MISC_CR_MTIME_TOGGLE_CNT);
}

uint32_t ATTR_CLOCK_SECTION CPU_Get_MTimer_Clock(void)
{
    uint32_t xclk = Clock_XClk_Mux_Output(Clock_Get_MCU_XClk_Sel_Val());
    uint16_t toggle_cnt = CPU_Get_MTimer_Toggle_Cnt();

    if (toggle_cnt == 0) {
        return xclk;
    }

    return xclk / toggle_cnt;
}

uint64_t ATTR_CLOCK_SECTION CPU_Get_MTimer_Counter(void)
{
    return SysTimer_GetLoadValue();
}

uint64_t ATTR_CLOCK_SECTION CPU_Get_CPU_Cycle(void)
{
#if (__riscv_xlen == 64)
    return (uint64_t)__get_MCYCLE();
#else
    volatile uint64_t tmpValLow, tmpValHigh, tmpValLow1, tmpValHigh1;
    uint8_t cnt = 0;

    do {
        tmpValHigh = (uint64_t)__get_MCYCLEH();
        tmpValLow = (uint64_t)__get_MCYCLE();
        tmpValLow1 = (uint64_t)__get_MCYCLE();
        tmpValHigh1 = (uint64_t)__get_MCYCLEH();
        cnt++;
        if (cnt > 4) {
            break;
        }
    } while (tmpValLow > tmpValLow1 || tmpValHigh != tmpValHigh1);

    return (uint64_t)((tmpValHigh1 << 32) + tmpValLow1);
#endif
}

uint64_t ATTR_CLOCK_SECTION CPU_Get_MTimer_US(void)
{
    uint32_t clk = CPU_Get_MTimer_Clock();
    uint64_t mtimer_counter = CPU_Get_MTimer_Counter();
    if (mtimer_counter == 0) {
        mtimer_counter = CPU_Get_CPU_Cycle();
        clk = SystemCoreClockGet();
    }

    if (clk < 1000 * 1000) {
        return mtimer_counter * 1000 / (clk / 1000);
    } else {
        return mtimer_counter / (clk / 1000 / 1000);
    }
}

uint64_t ATTR_CLOCK_SECTION CPU_Get_MTimer_MS(void)
{
    uint32_t clk = CPU_Get_MTimer_Clock();
    uint64_t mtimer_counter = CPU_Get_MTimer_Counter();
    if (mtimer_counter == 0) {
        mtimer_counter = CPU_Get_CPU_Cycle();
        clk = SystemCoreClockGet();
    }

    return mtimer_counter / (clk / 1000);
}

int ATTR_CLOCK_SECTION CPU_MTimer_Delay_US(uint32_t cnt)
{
    volatile uint64_t us1 = 0;
    volatile uint64_t us2 = 0;

    us1 = CPU_Get_MTimer_US();
    do {
        us2 = CPU_Get_MTimer_US();
    } while (((us2 - us1) < (uint64_t)(cnt)));

    return SUCCESS;
}

int ATTR_CLOCK_SECTION CPU_MTimer_Delay_MS(uint32_t cnt)
{
    volatile uint64_t ms1 = 0;
    volatile uint64_t ms2 = 0;

    ms1 = CPU_Get_MTimer_MS();
    do {
        ms2 = CPU_Get_MTimer_MS();
    } while (((ms2 - ms1) < (uint64_t)(cnt)));

    return SUCCESS;
}
