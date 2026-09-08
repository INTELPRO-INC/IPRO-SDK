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
 * @file ipro7ai_evk_board_config.h
 * @brief Board configuration for the IPRO7AI EVK sub-board.
 *
 * Source: schematic "IPRO7AI_MODULE_SUB_0805" (IPRO7 module, POC, 2026-08-05),
 * six sheets: 01_POWER, 02_MCU_MODULE, 03_SPK_SD_CARD, 04_LCM_CAMERA,
 * 05_BATTERY_CHARGER, 06_HEADERS.
 *
 * Unlike the other board headers here this one is hand-written from the
 * schematic rather than emitted by ipro_drv_gen, so it may be edited.
 *
 * ============================================================================
 * WHAT IS ON THE BOARD
 * ============================================================================
 *   Display  HEM LCM-T2D4BP-084, 2.4" IPS 240x320, Sitronix ST7789P3, 4-wire
 *            SPI at 3.3 V. Backlight enable on GPIO12.
 *   Camera   SmartSens SC101IOT, 1280x720 DVP, 8 of its 10 data lines tapped
 *            MSB-aligned, SCCB at 0x68. Runs at 1.8 V.
 *   Audio    On-chip DAC -> TPA2037D1 mono class-D -> J1 speaker connector;
 *            Infineon IM69D129FV01 PDM microphone into the AUADC.
 *   Storage  micro-SD on the SDH controller, with a switched supply.
 *   Battery  Li-ion via CN4 (DF65-3P-1D7V: VBAT / NTC / GND).
 *   USB      USB-C (CN1) for power and device USB.
 *
 * ============================================================================
 * POWER TREE
 * ============================================================================
 *   USB-C CN1 -> KTS1671 OVP (U1, OVLO 5.49 V) -> VBUS
 *   VBUS      -> RT9527GQW  (U7) Li-ion charger, Ichg 400 mA -> VBAT -> CN4
 *   VBAT      -> RT6158AWSC (U2) buck-boost, 2 A            -> 3V3_SOC
 *   3V3_SOC   -> RT9080N-08GJ5 (U3) LDO -> 2V8_CAM   (camera AVDD)
 *   3V3_SOC   -> RT9080N-08GJ5 (U4) LDO -> 1V8_SOC   (camera DOVDD/DVDD,
 *                                                     DMIC, VDDIO1 option)
 *   VBAT      -> R32 0R -> PA_VDD  (the class-D amplifier runs off the cell)
 *
 * >>> FIRMWARE CANNOT SEE THE BATTERY. <<<
 *
 * Two things follow from the schematic and both bite if assumed otherwise:
 *
 *   1. The charger's PGOOD and CHG outputs drive LEDs D14 (green) and D15
 *      (red). Neither reaches a GPIO, so charge state is not readable.
 *   2. VBAT appears on sheets 01, 03 and 05 only - buck-boost input, amplifier
 *      supply, charger output. It never reaches an ADC pin, and no divider is
 *      fitted. GPADC channel 18 ("VBAT/2") is an on-die tap of the SoC's own
 *      supply, which here is 3V3_SOC out of the buck-boost - NOT the cell.
 *
 * Measuring the actual cell needs a divider added into one of GPADC channels
 * 0-11. See BOARD_HAS_BATTERY_SENSE below.
 *
 * ============================================================================
 * J6 CONFIGURATION JUMPERS - GET THESE RIGHT BEFORE POWERING UP
 * ============================================================================
 *   VDDIO1 supply:  1.8 V = short J6 PIN1/PIN3      <- required for the camera
 *                   3.3 V = short J6 PIN3/PIN5
 *   BOOT (GPIO41):  pull LOW  = short J6 PIN6/PIN8  <- normal boot
 *                   pull HIGH = short J6 PIN8/PIN10 (ISP download)
 *
 * VDDIO1 supplies the VDDIO_1 pad bank (GPIO0-9 and GPIO36-41), which is
 * exactly the camera's DVP and SCCB pins - so it has to be 1.8 V to match the
 * sensor's DOVDD. The LCD sits in a different bank at 3.3 V.
 *
 * >>> GPIO41 IS BOTH THE BOOT STRAP AND THE CAMERA MCLK. <<<
 * The J6 jumper sets it at reset; once running, the CAM_MISC function drives
 * it as DVP_REF_CLK. Do not leave the ISP jumper fitted and then wonder why
 * the sensor will not clock.
 */

#ifndef __IPRO7AI_EVK_BOARD_CONFIG_H__
#define __IPRO7AI_EVK_BOARD_CONFIG_H__

#include "drv_clock.h"
#include "drv_gpio.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_timer.h"
#include "drv_pwm.h"
#include "drv_adc.h"

/*==============================================================================
 * Board peripheral pin map
 *
 * Every number below is read off the 02_MCU_MODULE sheet and cross-checked
 * against the 06_HEADERS breakouts.
 *============================================================================*/

/* ---- Console ---- */
#define BOARD_UART_TX_PIN            20
#define BOARD_UART_RX_PIN            21

/* ---- JTAG ---- */
#define BOARD_JTAG_TMS_PIN           24
#define BOARD_JTAG_TDI_PIN           25
#define BOARD_JTAG_TCK_PIN           26
#define BOARD_JTAG_TDO_PIN           11

/* ---- ST7789P3 display, 4-wire SPI on SPI0 (3.3 V bank) ----
 * IPRO7 SPI0 pad roles follow pin % 4: 0 = MOSI, 1 = MISO, 2 = SS, 3 = SCLK.
 * So GPIO16/GPIO15 are the hardware SPI pads; CS, RST (SS positions) and DC
 * (MISO position) are driven as plain GPIOs. */
#define BOARD_LCD_CS_PIN             10
#define BOARD_LCD_BL_PIN             12   /* backlight ENABLE, not a PWM dimmer */
#define BOARD_LCD_RST_PIN            14   /* active low */
#define BOARD_LCD_SCLK_PIN           15
#define BOARD_LCD_MOSI_PIN           16
#define BOARD_LCD_DC_PIN             17   /* 0 = command, 1 = data */

/* ---- SC101IOT DVP camera (1.8 V bank) ----
 * Sensor D0/D1 are left unconnected and D2..D9 land on DVP_D0..D7 - the
 * standard MSB-aligned 8-bit tap on a 10-bit output, so no shifting is needed.
 * SCCB roles are fixed by pin parity on IPRO7: even = SCL, odd = SDA. */
#define BOARD_CAM_D0_PIN             0    /* .. +7 = GPIO0..GPIO7 */
#define BOARD_CAM_SCL_PIN            36
#define BOARD_CAM_SDA_PIN            37
#define BOARD_CAM_PCLK_PIN           38
#define BOARD_CAM_VSYNC_PIN          39
#define BOARD_CAM_HSYNC_PIN          40
#define BOARD_CAM_MCLK_PIN           41   /* GPIO_FUN_CAM_MISC + GLB_Set_CAM_CLK */
#define BOARD_CAM_RST_PIN            35   /* active low, level-shifted by Q3 */
#define BOARD_CAM_HAS_PWDN           0    /* no PWDN pin - GPIO34 is SD_DET */

/* ---- Audio ----
 * SPK_EN drives the TPA2037D1 EN pin through a 1k/10k divider with R25
 * pulling down, so it is ACTIVE HIGH. The SDK's audio_output_dac.c assumes
 * the EVB's LM4871, whose SHUTDOWN is active low, and writes the enable GPIO
 * LOW on start and HIGH on stop - exactly inverted here. Re-assert the pin
 * after every audio_output_start()/stop().
 * GPIO22 is also DAC channel B, so using it as the enable makes this mono. */
#define BOARD_SPK_DAC_PIN            23   /* DAC channel A */
#define BOARD_SPK_EN_PIN             22
#define BOARD_SPK_EN_ACTIVE          1    /* level that turns the amp ON */

/* PDM microphone. AUADC roles follow pin parity like I2C: even = PDM_CLK,
 * odd = PDM_DIN. The mic's SELECT pin is grounded, so it drives the LEFT slot.
 *
 * GPIO18/19 are AON pads. The always-on block's per-pad ownership bits live in
 * TWO registers - HBN_PAD_CTRL_0[27:20] for the low 8 pads and
 * HBN_PAD_CTRL_2[11:10] for the top 2 - and this board boots with
 * PAD_CTRL_2 = 0xC00, i.e. the top two claimed with their input buffers off.
 * A pad in that state reads 0 forever and will not follow its own pull-up.
 * Clear en_aon_ctrl_msb before muxing PDM. */
#define BOARD_MIC_CLK_PIN            18
#define BOARD_MIC_DAT_PIN            19
#define BOARD_MIC_PDM_CHANNEL        0    /* 0 = left slot */

/* ---- micro-SD on the SDH controller ----
 * GPIO28-33 are the SDH_0 group (function 27). GPIO27 gates the card's supply
 * through a DMG3401LSN P-channel FET and GPIO34 is card-detect.
 *
 * BOARD_SD_PWR_ON_LEVEL is the level that turns the card supply ON. A
 * P-channel high-side switch conducts with its gate pulled low, which makes 0
 * the expected value, but the schematic does not make the drive polarity
 * unambiguous - confirm on the bench before trusting it. */
#define BOARD_SD_PWR_EN_PIN          27
#define BOARD_SD_PWR_ON_LEVEL        0
#define BOARD_SD_D1_PIN              28
#define BOARD_SD_D0_PIN              29
#define BOARD_SD_CLK_PIN             30
#define BOARD_SD_CMD_PIN             31
#define BOARD_SD_D3_PIN              32
#define BOARD_SD_D2_PIN              33
#define BOARD_SD_DET_PIN             34

/* ---- Spare ----
 * GPIO13 goes only to header J3; nothing else uses it. */
#define BOARD_SPARE_PIN              13

/* ---- Capability flags ---- */
#define BOARD_HAS_LCD                1
#define BOARD_HAS_CAMERA             1
#define BOARD_HAS_SPEAKER            1
#define BOARD_HAS_PDM_MIC            1
#define BOARD_HAS_SDCARD             1
#define BOARD_HAS_BATTERY            1
#define BOARD_HAS_BATTERY_SENSE      0   /* no divider from VBAT to any ADC pin */
#define BOARD_HAS_CHARGE_STATUS      0   /* PGOOD/CHG drive LEDs, not GPIOs */

/*==============================================================================
 * Pin Configuration Table
 *
 * Only the pins that must be live from reset: the console, JTAG, and the SD
 * card. The display, camera and audio pins are muxed by their own drivers when
 * that peripheral is brought up, so that an app which does not use them leaves
 * them alone.
 *============================================================================*/

#ifdef CONFIG_DISABLE_SDC_PINMUX
/* SF3 external flash mode: GPIO 28-33 are flash pins, NOT SD card */
#define BOARD_PINMUX_TABLE \
    { GLB_GPIO_PIN_11, 14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* JTAG_TDO */ \
    { GLB_GPIO_PIN_20, 7, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 },  /* UART0 TX */ \
    { GLB_GPIO_PIN_21, 7, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 },  /* UART0 RX */ \
    { GLB_GPIO_PIN_24, 14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* JTAG_TMS */ \
    { GLB_GPIO_PIN_25, 14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* JTAG_TDI */ \
    { GLB_GPIO_PIN_26, 14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }  /* JTAG_TCK */
#define BOARD_PINMUX_COUNT 6
#else
#define BOARD_PINMUX_TABLE \
    { GLB_GPIO_PIN_11, 14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* JTAG_TDO */ \
    { GLB_GPIO_PIN_20, 7, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 },  /* UART0 TX */ \
    { GLB_GPIO_PIN_21, 7, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 },  /* UART0 RX */ \
    { GLB_GPIO_PIN_24, 14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* JTAG_TMS */ \
    { GLB_GPIO_PIN_25, 14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* JTAG_TDI */ \
    { GLB_GPIO_PIN_26, 14, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 }, /* JTAG_TCK */ \
    { GLB_GPIO_PIN_27, 11, GPIO_MODE_OUTPUT, GPIO_PULL_NONE, 2, 1 }, /* SD_PWR_EN */ \
    { GLB_GPIO_PIN_28, 27, GPIO_MODE_AF, GPIO_PULL_UP, 2, 1 }, /* SD_D1 */ \
    { GLB_GPIO_PIN_29, 27, GPIO_MODE_AF, GPIO_PULL_UP, 2, 1 }, /* SD_D0 */ \
    { GLB_GPIO_PIN_30, 27, GPIO_MODE_AF, GPIO_PULL_NONE, 2, 1 }, /* SD_CLK */ \
    { GLB_GPIO_PIN_31, 27, GPIO_MODE_AF, GPIO_PULL_UP, 2, 1 }, /* SD_CMD */ \
    { GLB_GPIO_PIN_32, 27, GPIO_MODE_AF, GPIO_PULL_UP, 2, 1 }, /* SD_D3 */ \
    { GLB_GPIO_PIN_33, 27, GPIO_MODE_AF, GPIO_PULL_UP, 2, 1 }, /* SD_D2 */ \
    { GLB_GPIO_PIN_34, 11, GPIO_MODE_INPUT, GPIO_PULL_UP, 0, 1 }  /* SD_DET */
#define BOARD_PINMUX_COUNT 14
#endif

/**
 * Initialize all board pinmux settings.
 *
 * SD_PWR_EN is driven to its ON level here so a card is powered before the SDH
 * controller is brought up; see the polarity caveat above.
 */
#define BOARD_PINMUX_INIT() \
    do { \
        static GLB_GPIO_Cfg_Type pinmux_table[] = { BOARD_PINMUX_TABLE }; \
        for (int i = 0; i < BOARD_PINMUX_COUNT; i++) { \
            GLB_GPIO_Init(&pinmux_table[i]); \
        } \
        GLB_GPIO_Write(GLB_GPIO_PIN_27, BOARD_SD_PWR_ON_LEVEL); \
        GLB_GPIO_OUTPUT_Enable(GLB_GPIO_PIN_27); \
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
        GLB_Set_SDH_CLK(1, GLB_SDH_CLK_PLL_96M, 1); \
        GLB_Set_USB_CLK(1); \
        GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_BCLK, 0); \
    } while(0)

/*==============================================================================
 * Clock Gating Configuration
 *
 * MM (the ISP / camera path) is deliberately NOT un-gated here: it is a large
 * block, most apps do not use the camera, and the camera driver un-gates it
 * itself. Leaving it gated is also why a camera pipeline that forgets
 * GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM) silently produces zero frames.
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
 * System Clock Configuration
 *============================================================================*/

#define XTAL_TYPE                      EXTERNAL_XTAL_32M
#define BOARD_CLK_ROOT_SOURCE_MACRO    GLB_DLL_XTAL_32M
#define BOARD_CLK_SYS_CLK_MACRO        GLB_SYS_CLK_PLL192M
#define BOARD_CLK_HCLK_DIV             0      /* HCLK = SYS_CLK / (div + 1) */
#define BOARD_CLK_BCLK_DIV             1      /* BCLK = HCLK   / (div + 1) */

/**
 * Skip the destructive XTAL->PLL retune when the chip already runs at the
 * exact target - boot2 typically pre-sets PLL192M, and the retune pauses BCLK
 * long enough to stall code and data living in OCRAM.
 */
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

/** XCLK source. This board has an external 32 MHz crystal. */
#define BOARD_XCLK_INIT()    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL)

/*==============================================================================
 * 32K Clock Configuration
 *
 * RC32K. There is no 32.768 kHz crystal fitted, which matters because its pads
 * would be GPIO18/19 - the PDM microphone pins.
 *============================================================================*/

#define BOARD_CLK_32K_SOURCE           RC32K
#define BOARD_CLK_32K_SOURCE_MACRO     HBN_32K_RC

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

#endif /* __IPRO7AI_EVK_BOARD_CONFIG_H__ */
