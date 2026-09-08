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
 * @file ipro_dual_imu_board_config.h
 * @brief Board configuration for Robust dual-IMU board (IPRO7)
 *
 * Hardware sensors:
 *   IIS3DWB  (3-axis vibration, 26.7 kHz) on SPI0
 *     GPIO37 AF=4  SPI0_MOSI
 *     GPIO38 GPIO  SPI0 CS (software)
 *     GPIO39 AF=4  SPI0_SCLK
 *     GPIO40 AF=4  SPI0_MISO
 *
 *   ISM330BX (6-axis IMU, up to 3.84 kHz) on SPI1
 *     GPIO31 AF=26 SPI1_SCLK
 *     GPIO32 AF=26 SPI1_MISO
 *     GPIO33 AF=26 SPI1_MOSI
 *     GPIO34 GPIO  SPI1 CS (software)
 *
 *   UART0 (console, 921600 bps) on GPIO20/21
 */

#ifndef __IPRO7_DUAL_IMU_BOARD_CONFIG_H__
#define __IPRO7_DUAL_IMU_BOARD_CONFIG_H__

#include "drv_clock.h"
#include "drv_gpio.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_timer.h"
#include "drv_pwm.h"
#include "drv_adc.h"

/*==============================================================================
 * Dual IMU sensor pin definitions (exposed for app-level GPIO control)
 *============================================================================*/

/* IIS3DWB on SPI0 — software CS */
#define BOARD_IIS3DWB_SPI_ID          0            /* SPI0_INDEX */
#define BOARD_IIS3DWB_CS_PIN          GLB_GPIO_PIN_38
#define BOARD_IIS3DWB_MOSI_PIN        GLB_GPIO_PIN_37
#define BOARD_IIS3DWB_SCLK_PIN        GLB_GPIO_PIN_39
#define BOARD_IIS3DWB_MISO_PIN        GLB_GPIO_PIN_40

/* ISM330BX on software bit-bang SPI (IPRO7 SPI1 peripheral is silicon-locked
 * to GPIO4-7, which the Robust PCB does not route). */
#define BOARD_ISM330BX_SPI_ID         0xB0         /* bit-bang bus id (0xB0 + index) */
#define BOARD_ISM330BX_CS_PIN         GLB_GPIO_PIN_34
#define BOARD_ISM330BX_SCLK_PIN       GLB_GPIO_PIN_31
#define BOARD_ISM330BX_MISO_PIN       GLB_GPIO_PIN_32
#define BOARD_ISM330BX_MOSI_PIN       GLB_GPIO_PIN_33

/*==============================================================================
 * Pin Configuration Table
 *============================================================================*/

/**
 * Pinmux configuration table for Robust dual-IMU board
 * Format: { pin, function, mode, pull, drive, smt }
 *
 * AF codes (from drv_gpio.h):
 *    4 = GPIO_FUN_SPI   (SPI0 — used for IIS3DWB)
 *    7 = GPIO_FUN_UART
 *   11 = GPIO_FUN_GPIO  (software GPIO)
 *   14 = GPIO_FUN_MCU_JTAG
 *
 * ISM330BX note: IPRO7's hardware SPI1 peripheral is silicon-routed to
 *               GPIO4-7 only (see apps/test/dvt/tc_storage_lp.c). Since the
 *               Robust board traces the sensor to GPIO31-34, that SPI bus
 *               is driven in software (see src/spi_bitbang.c) and these
 *               four pins are therefore configured as plain GPIOs.
 */
#define BOARD_PINMUX_TABLE \
    { GLB_GPIO_PIN_0,  14, GPIO_MODE_AF,     GPIO_PULL_UP,   0, 1 }, /* JTAG */ \
    { GLB_GPIO_PIN_1,  14, GPIO_MODE_AF,     GPIO_PULL_UP,   0, 1 }, /* JTAG */ \
    { GLB_GPIO_PIN_2,  14, GPIO_MODE_AF,     GPIO_PULL_UP,   0, 1 }, /* JTAG */ \
    { GLB_GPIO_PIN_11, 14, GPIO_MODE_AF,     GPIO_PULL_UP,   0, 1 }, /* JTAG */ \
    { GLB_GPIO_PIN_20,  7, GPIO_MODE_AF,     GPIO_PULL_UP,   0, 1 }, /* UART0 TX */ \
    { GLB_GPIO_PIN_21,  7, GPIO_MODE_AF,     GPIO_PULL_UP,   0, 1 }, /* UART0 RX */ \
    { GLB_GPIO_PIN_31, 11, GPIO_MODE_OUTPUT, GPIO_PULL_UP,   2, 1 }, /* ISM330BX SCLK (bit-bang) */ \
    { GLB_GPIO_PIN_32, 11, GPIO_MODE_INPUT,  GPIO_PULL_UP,   0, 1 }, /* ISM330BX MISO (bit-bang) */ \
    { GLB_GPIO_PIN_33, 11, GPIO_MODE_OUTPUT, GPIO_PULL_UP,   2, 1 }, /* ISM330BX MOSI (bit-bang) */ \
    { GLB_GPIO_PIN_34, 11, GPIO_MODE_OUTPUT, GPIO_PULL_UP,   2, 1 }, /* ISM330BX CS   (bit-bang) */ \
    { GLB_GPIO_PIN_37,  4, GPIO_MODE_AF,     GPIO_PULL_UP,   1, 1 }, /* SPI0 MOSI */ \
    { GLB_GPIO_PIN_38, 11, GPIO_MODE_OUTPUT, GPIO_PULL_UP,   2, 1 }, /* IIS3DWB CS (sw) */ \
    { GLB_GPIO_PIN_39,  4, GPIO_MODE_AF,     GPIO_PULL_UP,   1, 1 }, /* SPI0 SCLK */ \
    { GLB_GPIO_PIN_40,  4, GPIO_MODE_AF,     GPIO_PULL_UP,   1, 1 }  /* SPI0 MISO */

#define BOARD_PINMUX_COUNT 14

/**
 * Initialize all board pinmux settings.
 * Two CS pins are explicitly put into high state so sensors are not
 * accidentally selected during later peripheral init.
 */
#define BOARD_PINMUX_INIT() \
    do { \
        static GLB_GPIO_Cfg_Type pinmux_table[] = { BOARD_PINMUX_TABLE }; \
        for (int i = 0; i < BOARD_PINMUX_COUNT; i++) { \
            GLB_GPIO_Init(&pinmux_table[i]); \
        } \
        GLB_GPIO_OUTPUT_Enable(BOARD_IIS3DWB_CS_PIN); \
        GLB_GPIO_OUTPUT_Enable(BOARD_ISM330BX_CS_PIN); \
        GLB_GPIO_Write(BOARD_IIS3DWB_CS_PIN, 1); \
        GLB_GPIO_Write(BOARD_ISM330BX_CS_PIN, 1); \
    } while(0)

/*==============================================================================
 * UART Routing Configuration
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
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SPI0); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SPI1); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_TIMER); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_BLE); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SEC_ENG); \
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_PKA); \
    } while(0)

/*==============================================================================
 * System Clock Configuration
 *
 * Values below are the authoritative per-board clock setup.  PLL192M is used
 * because the USB peripheral requires a 48 MHz derived clock (192/4).
 *============================================================================*/

#define XTAL_TYPE                      EXTERNAL_XTAL_32M
#define BOARD_CLK_ROOT_SOURCE_MACRO    GLB_DLL_XTAL_32M
#define BOARD_CLK_SYS_CLK_MACRO        GLB_SYS_CLK_PLL192M
#define BOARD_CLK_HCLK_DIV             0      /* HCLK = SYS_CLK / (div + 1) */
#define BOARD_CLK_BCLK_DIV             1      /* BCLK = HCLK   / (div + 1) */

/**
 * Initialize system clock (root source and PLL).
 * Skip the destructive XTAL→PLL retune when the chip already runs at the
 * exact target — boot2 typically pre-sets PLL192M. The retune pauses BCLK
 * long enough to stall code/data in OCRAM (BCLK-clocked).
 */
static inline int board_system_clock_already_at_target(void)
{
    uint32_t cfg0 = *(volatile uint32_t *)(GLB_BASE + GLB_CLK_CFG0_OFFSET);
    uint32_t root = (cfg0 >> GLB_HBN_ROOT_CLK_SEL_POS) &
                    ((1U << GLB_HBN_ROOT_CLK_SEL_LEN) - 1);
    uint32_t pll  = (cfg0 >> GLB_REG_PLL_SEL_POS) &
                    ((1U << GLB_REG_PLL_SEL_LEN) - 1);

    if ((BOARD_CLK_SYS_CLK_MACRO) == GLB_SYS_CLK_RC32M) return root == 0;
    if ((BOARD_CLK_SYS_CLK_MACRO) == GLB_SYS_CLK_XTAL)  return root == 1;
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

/**
 * Initialize MTimer to run at the current XCLK frequency.
 */
#define BOARD_MTIMER_CLK_INIT() \
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_XCLK, \
        Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK) / 1000 / 1000 / 2 - 1)

/**
 * XCLK source selection — Robust dual-IMU board uses external 32 MHz XTAL.
 */
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

#endif /* __IPRO7_DUAL_IMU_BOARD_CONFIG_H__ */
