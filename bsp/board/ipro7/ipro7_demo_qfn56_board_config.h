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

/**
 * @file ipro7_demo_qfn56_board_config.h
 * @brief Board configuration for the IPRO7 SEVB QFN56 demo board.
 *
 * QFN56 demo board (NOT the EVB). Has PSRAM + AI/NPU + a DVP camera.
 * Externally usable pins are the TB0 connector (TB0_1~12 = DVP/GPIO, TB0_15/16
 * = UART, TB0_17/18 = I2C). UART console is GPIO20/21. The factory-reset
 * button is on GPIO41 (TB0_12 / CAM_CLK).
 *
 * Selected by CONFIG_DEMO_QFN56 (see board_config.h). CONFIG_DEMO_QFN56 also
 * selects USE_PSRAM in Kconfig; enable AI with CONFIG_AI_SUPPORT.
 */

#ifndef __IPRO7_DEMO_QFN56_BOARD_CONFIG_H__
#define __IPRO7_DEMO_QFN56_BOARD_CONFIG_H__

#include "drv_clock.h"
#include "drv_gpio.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_timer.h"
#include "drv_pwm.h"
#include "drv_adc.h"

/*==============================================================================
 * Board GPIO assignments (referenced by apps)
 *============================================================================*/

/* Factory-reset button — TB0_12 (CAM_CLK pin). Non-JTAG so it does not clash
 * with SWD/JTAG debug. Apps long-press 3 s to erase + reboot. */
#define BOARD_FACTORY_RESET_GPIO    41u

/*==============================================================================
 * Pin Configuration Table
 *
 * Minimal: UART0 console (GPIO20/21) + JTAG (GPIO0/1/2/9). DVP/camera pins
 * (GPIO0-7, 36-41) are left at reset defaults — a camera app remuxes them.
 *============================================================================*/

#define BOARD_PINMUX_TABLE \
    { GLB_GPIO_PIN_0,  14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* GPIO0:  JTAG TMS */ \
    { GLB_GPIO_PIN_1,  14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* GPIO1:  JTAG TDI */ \
    { GLB_GPIO_PIN_2,  14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* GPIO2:  JTAG TCK */ \
    { GLB_GPIO_PIN_9,  14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* GPIO9:  JTAG TDO */ \
    { GLB_GPIO_PIN_20,  7, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* GPIO20: UART0 TX */ \
    { GLB_GPIO_PIN_21,  7, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }  /* GPIO21: UART0 RX */
#define BOARD_PINMUX_COUNT 6

#define BOARD_PINMUX_INIT() \
    do { \
        static GLB_GPIO_Cfg_Type pinmux_table[] = { BOARD_PINMUX_TABLE }; \
        for (int i = 0; i < BOARD_PINMUX_COUNT; i++) { \
            GLB_GPIO_Init(&pinmux_table[i]); \
        } \
    } while(0)

/*==============================================================================
 * UART Routing Configuration (UART0 console on GPIO20/21)
 *============================================================================*/

#define BOARD_GLB_UART_SIG_8_FUNC    GLB_UART_SIG_FUN_UART0_TXD
#define BOARD_GLB_UART_SIG_9_FUNC    GLB_UART_SIG_FUN_UART0_RXD

#define BOARD_UART_ROUTING_INIT() \
    do { \
        GLB_UART_Fun_Sel(GLB_UART_SIG_8, GLB_UART_SIG_FUN_UART0_TXD); \
        GLB_UART_Fun_Sel(GLB_UART_SIG_9, GLB_UART_SIG_FUN_UART0_RXD); \
    } while(0)

/*==============================================================================
 * Peripheral Clock Configuration
 *============================================================================*/

#define BOARD_PERIPHERAL_CLOCK_INIT() \
    do { \
        GLB_Set_UART_CLK(1, HBN_UART_CLK_XCLK, 0); \
        GLB_Set_I2S_CLK(1, GLB_I2S_OUT_REF_CLK_NONE); \
        TIMER_Set_Clock(TIMER0_ID, TIMER_CH0, TIMER_CLKSRC_XTAL, 31); \
        TIMER_Set_Clock(TIMER0_ID, TIMER_CH1, TIMER_CLKSRC_XTAL, 31); \
        WDT_Set_Clock(WDT0_ID, TIMER_CLKSRC_32K, 31); \
        GLB_Set_SDH_CLK(1, GLB_SDH_CLK_PLL_96M, 1); \
        GLB_Set_USB_CLK(1); \
        GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_BCLK, 0); \
    } while(0)

/*==============================================================================
 * Clock Gating Configuration
 *============================================================================*/

#define BOARD_CLOCK_GATING_INIT() \
    do { \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_0); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_UART0); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_UART1); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SPI0); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SPI1); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_TIMER); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDH); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_BLE); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_M154); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SEC_ENG); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_PKA); \
    } while(0)

/*==============================================================================
 * System Clock Configuration (external 32 MHz XTAL → PLL 192 MHz)
 *============================================================================*/

#define XTAL_TYPE                      EXTERNAL_XTAL_32M
#define BOARD_CLK_ROOT_SOURCE_MACRO    GLB_DLL_XTAL_32M
#define BOARD_CLK_SYS_CLK_MACRO        GLB_SYS_CLK_PLL192M
#define BOARD_CLK_HCLK_DIV             0
#define BOARD_CLK_BCLK_DIV             1

static inline int board_system_clock_already_at_target(void)
{
    uint32_t cfg0 = *(volatile uint32_t *)(GLB_BASE + GLB_CLK_CFG0_OFFSET);
    uint32_t root = (cfg0 >> GLB_HBN_ROOT_CLK_SEL_POS) &
                    ((1U << GLB_HBN_ROOT_CLK_SEL_LEN) - 1);
    uint32_t pll  = (cfg0 >> GLB_REG_PLL_SEL_POS) &
                    ((1U << GLB_REG_PLL_SEL_LEN) - 1);

    if ((BOARD_CLK_SYS_CLK_MACRO) == GLB_SYS_CLK_RC32M)
        return root == 0;
    if ((BOARD_CLK_SYS_CLK_MACRO) == GLB_SYS_CLK_XTAL)
        return root == 1;
    return (root == 2 || root == 3) &&
           pll == ((BOARD_CLK_SYS_CLK_MACRO) - GLB_SYS_CLK_DLL25P6M);
}

#define BOARD_SYSTEM_CLOCK_INIT() \
    do { \
        if (!board_system_clock_already_at_target()) { \
            GLB_Set_System_CLK(BOARD_CLK_ROOT_SOURCE_MACRO, BOARD_CLK_SYS_CLK_MACRO); \
        } \
        GLB_Set_System_CLK_Div(BOARD_CLK_HCLK_DIV, BOARD_CLK_BCLK_DIV); \
    } while(0)

#define BOARD_MTIMER_CLK_INIT() \
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_XCLK, \
        Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK) / 1000 / 1000 / 2 - 1)

#define BOARD_XCLK_INIT()    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL)

/*==============================================================================
 * 32K Clock Configuration
 *============================================================================*/

#define BOARD_CLK_32K_SOURCE    RC32K
#define BOARD_CLK_32K_SOURCE_MACRO    HBN_32K_RC

#define BOARD_32K_CLOCK_INIT() \
    do { \
        HBN_Power_Off_Xtal_32K(); \
        HBN_32K_Sel(HBN_32K_RC); \
    } while(0)

/*==============================================================================
 * Board Initialization
 *============================================================================*/

#define BOARD_INIT() \
    do { \
        BOARD_PINMUX_INIT(); \
        BOARD_UART_ROUTING_INIT(); \
        BOARD_CLOCK_GATING_INIT(); \
        BOARD_PERIPHERAL_CLOCK_INIT(); \
    } while(0)

#endif /* __IPRO7_DEMO_QFN56_BOARD_CONFIG_H__ */
