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
#include "drv_pwm.h"
#include "drv_timer.h"

static uint32_t ATTR_CLOCK_SECTION Clock_Xtal_Output(void)
{
    /* 32m */
    return (32 * 1000 * 1000);
}

static uint32_t ATTR_CLOCK_SECTION Clock_XClk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* rc32m */
        return (32 * 1000 * 1000);
    } else {
        /* xtal */
        return Clock_Xtal_Output();
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_MCU_XClk_Sel_Val(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG0_OFFSET);
    tmpVal = reg_get_bits(tmpVal, GLB_HBN_ROOT_CLK_SEL);

    return (tmpVal & 0x1);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_MCU_Root_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG0_OFFSET);
    tmpVal = reg_get_bits(tmpVal, GLB_HBN_ROOT_CLK_SEL);

    return ((tmpVal >> 1) & 0x1);
}

static uint32_t ATTR_CLOCK_SECTION Clock_MCU_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* pll 25.6m */
        return (256 * 100 * 1000);
    } else if (sel == 1) {
        /* pll 160m */
        return (160 * 1000 * 1000);
    } else if (sel == 2) {
        /* pll 192m */
        return (192 * 1000 * 1000);
    } else if (sel == 3) {
        /* pll 128m */
        return (128 * 1000 * 1000);
    } else {
        return 0;
    }
}

static uint32_t ATTR_CLOCK_SECTION Clock_MCU_Root_Clk_Mux_Output(uint8_t sel)
{
    uint32_t tmpVal;

    if (sel == 0) {
        /* xclk */
        return Clock_XClk_Mux_Output(Clock_Get_MCU_XClk_Sel_Val());
    } else if (sel == 1) {
        /* pll */
        tmpVal = readl(GLB_BASE + GLB_CLK_CFG0_OFFSET);
        tmpVal = reg_get_bits(tmpVal, GLB_REG_PLL_SEL);
        return Clock_MCU_Clk_Mux_Output(tmpVal);
    } else {
        return 0;
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_MCU_HClk_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG1_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_HCLK_DIV);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_Peri_BClk_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG1_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_BCLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_F32k_Mux_Output(uint8_t sel)
{
    uint32_t tmpVal;
    uint32_t div = 0;

    tmpVal = readl(GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);
    div = reg_get_bits(tmpVal, GLB_DIG_32K_DIV);

    if (sel == 0) {
        /* rc32K */
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

static uint32_t ATTR_CLOCK_SECTION Clock_Get_CLKPLL_PostDivOutput(void)
{
    uint32_t sdmin = 0;
    uint32_t vco_speed = 0;
    uint32_t postdiv = 0;

    sdmin = reg_get_bits(readl(CCI_BASE + CCI_CLKPLL_SDM_OFFSET), CCI_CLKPLL_SDMIN);
    postdiv = reg_get_bits(readl(CCI_BASE + CCI_CLKPLL_TOP_CTRL_OFFSET), CCI_CLKPLL_POSTDIV);

    if (sdmin == 0x780000)
        vco_speed = 480000000;
    else if (sdmin == 0x6c0000)
        vco_speed = 432000000;
    else if (sdmin == 0x600000)
        vco_speed = 384000000;
    else if (sdmin == 0x5A0000)
        vco_speed = 360000000;
    else
        vco_speed = 480000000;
    return (vco_speed >> postdiv);
}

static uint32_t ATTR_CLOCK_SECTION Clock_Get_Audio_Clk_Mux_Output(int sel)
{
    if (sel == 0) // 2.032M
        return (2032*1000);
    else if (sel == 1) // 6.095M
        return (6095*1000);
    else if (sel == 2) // 6.095M
        return (48*1000*1000);
    else
        return Clock_Get_CLKPLL_PostDivOutput();
}

static uint32_t ATTR_CLOCK_SECTION Clock_Get_Audio_Clk_Mux_Sel(void)
{
    uint32_t tmpVal = 0;
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    return reg_get_bits(tmpVal, GLB_REG_AUDIO_CLK_SRC_SEL);
}

static uint32_t __attribute__((unused)) ATTR_CLOCK_SECTION Clock_Get_Audio_CLK(void)
{
    uint32_t tmpVal = 0;
    uint32_t div = 0;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    div = reg_get_bits(tmpVal, GLB_REG_AUDIO_CLK_DIV);

    return Clock_Get_Audio_Clk_Mux_Output(Clock_Get_Audio_Clk_Mux_Sel()) / (div + 1);
}

static uint32_t ATTR_CLOCK_SECTION Clock_I2S_Clk_Mux_Output(void)
{
    uint32_t tmpVal;
    uint32_t sel, div;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG6_OFFSET);
    sel = reg_get_bits(tmpVal, GLB_I2S_REF_CLK_SEL);
    div = reg_get_bits(tmpVal, GLB_I2S_REF_CLK_DIV);

    return Clock_Get_Audio_Clk_Mux_Output(sel) / (div + 1);
}

uint32_t ATTR_CLOCK_SECTION Clock_System_Clock_Get(Drv_System_Clock_Type type)
{
    uint32_t clock = 0;
    uint32_t div = 0;

    switch (type) {
        case DRV_SYSTEM_CLOCK_XCLK:
            /*!< mcu xclk clock */
            return Clock_XClk_Mux_Output(Clock_Get_MCU_XClk_Sel_Val());

        case DRV_SYSTEM_CLOCK_ROOT:
            /*!< mcu root clock */
            return Clock_MCU_Root_Clk_Mux_Output(Clock_Get_MCU_Root_Clk_Sel_Val());

        case DRV_SYSTEM_CLOCK_FCLK:
            /*!< mcu fast clock/cpu clock */
            clock = Clock_MCU_Root_Clk_Mux_Output(Clock_Get_MCU_Root_Clk_Sel_Val());
            div = Clock_Get_MCU_HClk_Div_Val();
            return clock / (div + 1);

        case DRV_SYSTEM_CLOCK_BCLK:
            /*!< mcu peri bus clock */
            clock = Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_FCLK);
            div = Clock_Get_Peri_BClk_Div_Val();
            return clock / (div + 1);

        case DRV_SYSTEM_CLOCK_XTAL:
            /*!< xtal clock */
            return Clock_Xtal_Output();

        case DRV_SYSTEM_CLOCK_F32K:
            /*!< f32k clock */
            return Clock_F32k_Mux_Output(Clock_Get_F32k_Sel_Val());

        default:
            return 0;
    }
}

uint32_t ATTR_CLOCK_SECTION SystemCoreClockGet(void)
{
    uint32_t clockVal = 0;

    clockVal = Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_FCLK);

    clockVal = clockVal ? clockVal : (32 * 1000 * 1000);

    return clockVal;
}

static uint32_t ATTR_CLOCK_SECTION Clock_UART_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* fclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_FCLK);
    } else if (sel == 1) {
        /* 64m */
        return (64 * 1000 * 1000);
    } else {
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_UART_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);

    return (reg_get_bits(tmpVal, HBN_UART_CLK_SEL2) << 1) | reg_get_bits(tmpVal, HBN_UART_CLK_SEL);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_UART_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);

    return reg_get_bits(tmpVal, GLB_UART_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_SPI_Clk_Mux_Output(void)
{
    /* pbclk */
    return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_BCLK);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_SPI_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG4_OFFSET);

    return reg_get_bits(tmpVal, GLB_SPI_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_I2C_Clk_Mux_Output()
{
    /* pbclk */
    return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_BCLK);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_I2C_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG4_OFFSET);

    return reg_get_bits(tmpVal, GLB_I2C_CLK_DIV);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_GPADC_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_GPADC_32M_SRC_CTRL_OFFSET);

    return reg_get_bits(tmpVal, GLB_GPADC_32M_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_GPADC_Clk_Output(uint8_t sel)
{
    if (sel == 0) {
        /* f32k clk */
        return 32000;
    } else {
        /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_GPADC_32M_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_GPADC_32M_SRC_CTRL_OFFSET);

    return reg_get_bits(tmpVal, GLB_GPADC_32M_CLK_SEL);
}

static uint32_t ATTR_CLOCK_SECTION Clock_GPADC_Clk_Mux_Output(void)
{
    uint32_t div = 0;

    div = Clock_Get_GPADC_Div_Val();
    return Clock_GPADC_Clk_Output(Clock_Get_GPADC_32M_Clk_Sel_Val()) / (div + 1);
}

static uint32_t ATTR_CLOCK_SECTION Clock_GPDAC_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    } else {
        /* rc32M */
        return (32 * 1000 * 1000);
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_GPDAC_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);

    return reg_get_bits(tmpVal, GLB_DIG_512K_DIV);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_GPDAC_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DIG32K_WAKEUP_CTRL_OFFSET);

    return reg_get_bits(tmpVal, GLB_DIG_CLK_SRC_SEL);
}

static uint32_t ATTR_CLOCK_SECTION Clock_PWM_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    } else if (sel == 1) {
        /* pbclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_BCLK);
    } else {
        /* f32k clk */
        return Clock_F32k_Mux_Output(Clock_Get_F32k_Sel_Val());
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_PWM_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(PWM_BASE + PWM_MC0_CONFIG0_OFFSET);
    tmpVal = reg_get_bits(tmpVal, PWM_CLK_DIV);

    return tmpVal;
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_PWM_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(PWM_BASE + PWM_MC0_CONFIG0_OFFSET);
    tmpVal = reg_get_bits(tmpVal, PWM_REG_CLK_SEL);

    return reg_get_bits(tmpVal, PWM_REG_CLK_SEL);
}


static uint32_t ATTR_CLOCK_SECTION Clock_IR_Clk_Mux_Output(void)
{
    /* xclk */
    return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_IR_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);

    return reg_get_bits(tmpVal, GLB_IR_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_CAM_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    else if (sel == 1) /* 42.66m */
        return (42660 * 1000);
    else if (sel == 2) /* 48m */
        return (48 * 1000 * 1000);
    else
        return Clock_Get_CLKPLL_PostDivOutput();
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_CAM_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_CAM_REF_CLK_SEL);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_CAM_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG5_OFFSET);

    return reg_get_bits(tmpVal, GLB_REG_CAM_REF_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_SF_SEL_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* sf sel2 */
        return 32000000;;
        //return Clock_SF_SEL2_Clk_Mux_Output(Clock_Get_SF_Clk_Sel2_Val()); TODO: ipro7
    } else if (sel == 1) {
        /* 72m */
        return (72 * 1000 * 1000);
    } else if (sel == 2) {
        /* pbclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_BCLK);
    } else {
        /* 96m */
        return (96 * 1000 * 1000);
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_SF_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);

    return reg_get_bits(tmpVal, GLB_SF_CLK_SEL);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_SF_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_CLK_CFG3_OFFSET);

    return reg_get_bits(tmpVal, GLB_SF_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_QDEC_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* xclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
    } else {
        /* f32k clk */
        return Clock_F32k_Mux_Output(Clock_Get_F32k_Sel_Val());
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_QDEC_Clk_Sel_Val(void)
{
    uint32_t tmpVal;
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    return reg_get_bits(tmpVal, GLB_CLK_1M_CLK_SEL);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_QDEC_Div_Val(void)
{
    uint32_t tmpVal;
    tmpVal = readl(GLB_BASE + GLB_CLK_CFG2_OFFSET);
    return reg_get_bits(tmpVal, GLB_CLK_1M_CLK_DIV);
}

static uint32_t ATTR_CLOCK_SECTION Clock_TIMER_Clk_Mux_Output(uint8_t sel)
{
    if (sel == 0) {
        /* fclk */
        return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_FCLK);
    } else if (sel == 1) {
        /* f32k clk */
        return Clock_F32k_Mux_Output(Clock_Get_F32k_Sel_Val());
    } else if (sel == 2) {
        /* 1k */
        return (1000);
    } else {
        /* 32m */
        return (32 * 1000 * 1000);
    }
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_TIMER0_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(TIMER_BASE + TIMER_TCCR_OFFSET);

    return reg_get_bits(tmpVal, TIMER_CS_2);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_TIMER1_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(TIMER_BASE + TIMER_TCCR_OFFSET);

    return reg_get_bits(tmpVal, TIMER_CS_3);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_WDT_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(TIMER_BASE + TIMER_TCCR_OFFSET);

    return reg_get_bits(tmpVal, TIMER_CS_WDT);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_TIMER0_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(TIMER_BASE + TIMER_TCDR_OFFSET);

    return reg_get_bits(tmpVal, TIMER_TCDR2);
}

static uint8_t ATTR_CLOCK_SECTION Clock_Get_TIMER1_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = readl(TIMER_BASE + TIMER_TCDR_OFFSET);

    return reg_get_bits(tmpVal, TIMER_TCDR3);
}

/****************************************************************************/ /**
 * @brief  Get Peripheral Clock
 *
 * @param  type: Peripheral clock type
 *
 * @return Peripheral clock value
 *
*******************************************************************************/
uint32_t Clock_Peripheral_Clock_Get(Drv_Peripheral_Type_Type type)
{
#if defined(CONFIG_FPGA)
    return 32000000; // FPGA always use 32M clock
#endif
    uint32_t clock = 0;
    uint32_t div = 0;

    switch (type) {
        /*!< uart0 clock */
        case DRV_PERIPHERAL_CLOCK_UART0:
            ATTR_FALLTHROUGH();

        /*!< uart1 clock */
        case DRV_PERIPHERAL_CLOCK_UART1:
            clock = Clock_UART_Clk_Mux_Output(Clock_Get_UART_Clk_Sel_Val());
            div = Clock_Get_UART_Div_Val();
            return clock / (div + 1);

        /*!< FLASH clock */
        case DRV_PERIPHERAL_CLOCK_FLASH:
            clock = Clock_SF_SEL_Clk_Mux_Output(Clock_Get_SF_Clk_Sel_Val());
            div = Clock_Get_SF_Div_Val();
            return clock / (div + 1);

        /*!< spi0 clock */
        case DRV_PERIPHERAL_CLOCK_SPI0:
            clock = Clock_SPI_Clk_Mux_Output();
            div = Clock_Get_SPI_Div_Val();
            return clock / (div + 1);

        /*!< i2c0 clock */
        case DRV_PERIPHERAL_CLOCK_I2C0:
            clock = Clock_I2C_Clk_Mux_Output();
            div = Clock_Get_I2C_Div_Val();
            return clock / (div + 1);

        /*!< I2S clock */
        case DRV_PERIPHERAL_CLOCK_I2S:
            clock = Clock_I2S_Clk_Mux_Output();
            return clock;

        /*!< GPADC clock */
        case DRV_PERIPHERAL_CLOCK_GPADC:
            return Clock_GPADC_Clk_Mux_Output();

        /*!< GPDAC clock */
        case DRV_PERIPHERAL_CLOCK_GPDAC:
            clock = Clock_GPDAC_Clk_Mux_Output(Clock_Get_GPDAC_Clk_Sel_Val());
            div = Clock_Get_GPDAC_Div_Val();
            return clock / (div + 1);

        /*!< PWM clock */
        case DRV_PERIPHERAL_CLOCK_PWM:
            clock = Clock_PWM_Clk_Mux_Output(Clock_Get_PWM_Clk_Sel_Val());
            div = Clock_Get_PWM_Div_Val();
            return clock / (div + 1);

        /*!< IR clock */
        case DRV_PERIPHERAL_CLOCK_IR:
            clock = Clock_IR_Clk_Mux_Output();
            div = Clock_Get_IR_Div_Val();
            return clock / (div + 1);

        /*!< CAM clock */
        case DRV_PERIPHERAL_CLOCK_CAM:
            clock = Clock_CAM_Clk_Mux_Output(Clock_Get_CAM_Clk_Sel_Val());
            div = Clock_Get_CAM_Div_Val();
            return clock / (div + 1);

        /*!< QDEC clock */
        case DRV_PERIPHERAL_CLOCK_QDEC:
            clock = Clock_QDEC_Clk_Mux_Output(Clock_Get_QDEC_Clk_Sel_Val());
            div = Clock_Get_QDEC_Div_Val();
            return clock / (div + 1);

        /*!< TIMER0 clock */
        case DRV_PERIPHERAL_CLOCK_TIMER0:
            clock = Clock_TIMER_Clk_Mux_Output(Clock_Get_TIMER0_Clk_Sel_Val());
            div = Clock_Get_TIMER0_Div_Val();
            return clock / (div + 1);

        /*!< TIMER1 clock */
        case DRV_PERIPHERAL_CLOCK_TIMER1:
            clock = Clock_TIMER_Clk_Mux_Output(Clock_Get_TIMER1_Clk_Sel_Val());
            div = Clock_Get_TIMER1_Div_Val();
            return clock / (div + 1);

        /*!< WDT clock */
        case DRV_PERIPHERAL_CLOCK_WDT:
            return Clock_TIMER_Clk_Mux_Output(Clock_Get_WDT_Clk_Sel_Val());

        default:
            return 0;
    }
}

