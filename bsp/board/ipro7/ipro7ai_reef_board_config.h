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
 * @file ipro7ai_reef_board_config.h
 * @brief Board configuration for the IPRO7AI Reef carrier board.
 *
 * Source: EasyEDA project "IPRO7AI_EVK", schematic V3.0 (2026-09-16/17),
 * board "Board1", five sheets: 01_CORE_RF, 02_POWER, 03_CAM_SD_DEBUG,
 * 04_AUDIO, 05_GPIO. Pin map read from the exported netlist (U1 pin -> net ->
 * peer), not from a previous board header. The same project carries a second
 * board, "IPRO6S_FIN": an SD-card-edge Wi-Fi fin (IPRO6S QFN40, SDIO slave)
 * that plugs into this board's card slot.
 *
 * This is NOT the earlier IPRO7AI_EVK sub-board (CONFIG_IPRO7AI_EVK, schematic
 * IPRO7AI_MODULE_SUB_0812A): no LCD, no SC101IOT, no battery, and the IPRO7AI is
 * soldered straight onto the carrier as a QFN56 (U1, exposed pad = pin 27).
 *
 * ============================================================================
 * WHAT IS ON THE BOARD
 * ============================================================================
 *   SoC      IPRO7AI QFN56, 32 MHz crystal X1 (X3S032000B91H, 9 pF loads),
 *            RF pi-match (C12 1.6p / L2 3.4n / C13 1.5p), then R2 0R -> PCB
 *            antenna E1 or R1 0R -> u.FL RF1 (fit one).
 *   Camera   24-pin 0.5 mm FPC (FPC1, FPC-05F-24PH20, bottom contact) on the
 *            OmniVision FLEX-24 pinout (OV5640 / OV2640 modules). DVP 8-bit,
 *            SCCB on I2C0, XCLK from the CAM_MISC reference clock on GPIO22.
 *            Rails: DOVDD = 2.8 V, AVDD = 2.8 V (ferrite-filtered), DVDD = 1.5 V.
 *   Audio    On-chip DAC channel A (GPIO23) feeds BOTH a TLV9061 buffer ->
 *            3.5 mm headphone jack J2 (mono to tip+ring) AND a PAM8302A class-D
 *            -> J_SPK (PH2.0) speaker. Headset mic (J2 sleeve-ring) into the
 *            AUADC differential pair AIN7/AIN6. 3.5 mm AUX-IN jack J1 into
 *            AIN4/AIN5. MSM261DGT003 PDM microphone on GPIO16/17.
 *   Storage  Push-pull SD slot on the SDH controller (GPIO28-33), gated 3.3 V
 *            supply, card detect. The slot doubles as the IPRO6S_FIN SDIO slot.
 *   USB      USB-C (USB1) for power and device USB on GPIO8/9, RCLAMP ESD,
 *            5.1k CC pull-downs (UFP), F1 1.5 A PTC.
 *   LEDs     WS2812-class RGB LED (LED1, XL-2020RGBC-2812B) on GPIO24,
 *            supplied from VBUS through D3. Power LED LED2 on 3V3 (no GPIO).
 *   Keys     SW1 BOOT (GPIO41), SW2 RESET, SW3 (GPIO10), SW4 (GPIO27),
 *            SW5 (GPIO11).
 *   Header   J_IO 2x12 0.1": rails, reset, BOOT, UART, the SDIO lines and
 *            GPIO14/15/18/19/27.
 *   Debug    U6 SH1.0 6-pin wafer: UART0 + DTR->BOOT and RTS->RESET (1k each),
 *            so a probe can enter download mode without touching the keys.
 *
 * ============================================================================
 * POWER TREE
 * ============================================================================
 *   USB-C -> F1 PTC -> VBUS
 *   VBUS  -> MT2492 buck U4 (EN tied to VBUS, FB 100k/22k = 3.33 V) -> 3V3_BUCK
 *            -> JP2 (or R40 0R) -> 3V3
 *   VBUS  -> U1 VDD_BUS (pin 11): the SoC's own DCDC. SW_DC -> L1 2.2 uH ->
 *            DC_OUT -> VDDIO_PSRAM (pin 49) and, through R77 0R, AVDD_RF.
 *   VBUS  -> PAM8302A VDD (the amplifier runs off 5 V) and, via D3, LED_VDD.
 *   3V3   -> AVDD33 (15), VDDIO3 (43), R71 0R -> VDDIO1 (56)  [see JP1]
 *   3V3   -> ME6211-2.8 U2 -> VDD_2V8 (camera DOVDD, SCCB pull-ups, JP1 option)
 *            -> FB1 -> VDD_2V8A (camera AVDD)
 *   3V3   -> ME6211-1.5 U3 (CE from VDD_2V8 via 100k, so it follows 2V8)
 *            -> VDD_1V5 -> R8 0R -> VDD_1V5_CAM (camera DVDD)
 *   3V3   -> ME6211-1.8 U5 -> 1V8 (J_IO pin 7 only)
 *   3V3   -> R43 100R -> MIC_VDD (PDM mic), R49 2.2k -> headset mic bias,
 *            TLV9061 V+, R23 1k -> LED2
 *   3V3   -> JP3 -> SD_VDD, either direct (JP3 1-2) or through Q1 (JP3 2-3),
 *            an AO3401A P-FET whose gate is GPIO14 with a 100k pull-down.
 *
 * The camera rails have no GPIO control: they come up with 3V3 and stay up.
 * The sensor is held in reset by R34 10k on CAM_RST until GPIO34 drives it.
 *
 * ============================================================================
 * JUMPERS - SET THESE BEFORE POWERING UP
 * ============================================================================
 *   JP1  VDDIO1 bank supply (GPIO0-9, GPIO36-41 = camera DVP/SCCB + USB + BOOT)
 *          1-2 = 3.3 V        2-3 = 2.8 V (matches the sensor's DOVDD)
 *        R71 0R on the schematic hard-wires VDDIO1 to 3V3 and would short 3V3
 *        to VDD_2V8 through JP1 2-3; it is REMOVED by hand on the produced
 *        boards, so JP1 must be fitted for the bank to have a supply at all.
 *   JP2  3V3 source: 1-2 = from the MT2492 buck (R40 0R does the same job;
 *        fit one of the two, the jumper lets a bench supply feed 3V3).
 *   JP3  SD_VDD:  1-2 = always on (3V3 direct)
 *                 2-3 = switched by GPIO14 through Q1 (needed to power-cycle
 *                       the IPRO6S_FIN or a wedged card)
 *
 * ============================================================================
 * BOOT STRAP
 * ============================================================================
 * GPIO41 is the ISP strap. R27 20k pulls it LOW (normal boot); SW1 pulls it
 * HIGH through R28 1k to VDDIO1, and the debug wafer's DTR does the same
 * through R31 1k. Hold SW1 and tap SW2 for download mode. After boot GPIO41 is
 * a plain input and SW1 can be read as a user key (ACTIVE HIGH - the only key
 * on the board that is).
 */

#ifndef __IPRO7AI_REEF_BOARD_CONFIG_H__
#define __IPRO7AI_REEF_BOARD_CONFIG_H__

#include "drv_clock.h"
#include "drv_gpio.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_timer.h"
#include "drv_pwm.h"
#include "drv_adc.h"

/*==============================================================================
 * Board peripheral pin map (U1 pin -> net -> peer, from the V3.0 netlist)
 *============================================================================*/

/* ---- Console: UART0 on the U6 debug wafer (pins 3/4) and J_IO 13/15 ----
 * The wafer carries no TX/RX label; this follows the SDK-wide IPRO7 convention
 * of GPIO20 = TXD (UART_SIG_8) / GPIO21 = RXD (UART_SIG_9). Swap the routing
 * below if the probe cable turns out crossed. */
#define BOARD_UART_TX_PIN            20
#define BOARD_UART_RX_PIN            21

/* ---- USB device (USB-C USB1) ---- */
#define BOARD_USB_DP_PIN             8
#define BOARD_USB_DM_PIN             9

/* ---- Camera: OmniVision FLEX-24 module on FPC1 (OV5640 / OV2640) ----
 * FPC1 is a bottom-contact connector and the module's fingers face down, so
 * board pad j carries module pin (25 - j). Read that way, every FLEX-24 signal
 * lands where the SoC wants it:
 *   D0..D7 (Y2..Y9)  GPIO0..7       PCLK GPIO38   VSYNC GPIO39   HREF GPIO40
 *   XCLK GPIO22 (33R)  RESETB GPIO34 (33R, 10k pull-down)  PWDN GPIO35 (33R)
 *   SIOC GPIO36 / SIOD GPIO37, 5.1k pull-ups to VDD_2V8.
 * I2C0 roles are fixed by pin parity on IPRO7: even = SCL, odd = SDA.
 * OV5640 SCCB address 0x3C (16-bit registers), OV2640 0x30 (8-bit, banked). */
#define BOARD_CAM_D0_PIN             0    /* .. +7 = GPIO0..GPIO7 */
#define BOARD_CAM_SCL_PIN            36
#define BOARD_CAM_SDA_PIN            37
#define BOARD_CAM_PCLK_PIN           38
#define BOARD_CAM_VSYNC_PIN          39
#define BOARD_CAM_HSYNC_PIN          40
#define BOARD_CAM_MCLK_PIN           22   /* GPIO_FUN_CAM_MISC + GLB_Set_CAM_CLK */
#define BOARD_CAM_RST_PIN            34   /* RESETB, active low, 10k pull-down */
#define BOARD_CAM_PWDN_PIN           35   /* PWDN, active high (sensor pulls down) */
#define BOARD_CAM_HAS_PWDN           1

/* ---- Audio output: DAC channel A -> headphone buffer + class-D speaker ----
 * GPIO23 -> R44 1k + C50 4.7 nF (fc ~34 kHz) -> C51 1 uF -> TLV9061 unity
 * buffer (input biased at mid-rail by R45/R46 100k) -> R47/R48 10R ->
 * C53/C54 220 uF -> J2 tip and ring. One DAC channel, so the headphone is
 * mono on both ears.
 * GPIO23 -> R52 10k -> C60 1 uF -> PAM8302A IN+ (IN- AC-grounded through C61):
 * gain = 2 x 150k / 10k = 30 V/V (29.5 dB) - keep DAC amplitude modest.
 * PAM8302A SD# is GPIO25: HIGH = amplifier on, so the enable is ACTIVE HIGH.
 * GPIO22 is DAC channel B on the die but is spent on the camera clock here. */
#define BOARD_SPK_DAC_PIN            23   /* DAC channel A */
#define BOARD_SPK_EN_PIN             25   /* PAM8302A SD#, no external pull */
#define BOARD_SPK_EN_ACTIVE          1    /* level that turns the amp ON */

/* Headset jack J2 (SHOU HAN PJ-342, TRRS). Per the 2020-08-25 datasheet the
 * terminals are: 5 sleeve, 2 ring2, 6 ring1 SPRING with 7 its normally-closed
 * switch contact, 3 tip SPRING with 4 its switch contact. The board puts
 * HP_RING1 on 3 (tip), GPIO26 on 6 (ring1 spring) and HP_TIP on 7 (the switch
 * contact) - so with a plug in, GPIO26 sits on the ring1 earpiece coil and
 * reads LOW, while HP_TIP is cut off and that ear is silent. Bridging pads 6
 * and 7 puts HP_TIP on the ring1 spring too: both ears get the (mono) DAC and
 * GPIO26 still reads LOW plugged / HIGH unplugged (100k vs R51 1M) - checked
 * on the first board. The bridge leaves GPIO26 on the AC-coupled line: past
 * ~40 % DAC amplitude it swings below GND with a plug in (see the .md; a 10k
 * series resistor in the GPIO26 trace fixes it).
 * Wiring is OMTP (sleeve GND, ring2 mic); see the .md for CTIA headsets. */
#define BOARD_HP_DET_PIN             26
#define BOARD_HP_DET_ACTIVE          0    /* level that means "plug inserted" */

/* Headset microphone: J2 pin 2 biased by R49 2.2k from 3V3, AC-coupled by C55
 * into AIN7 (+). AIN6 (-) is AC-grounded through C56, so run the AUADC in
 * differential mode with these two channels. AIN index = position in the HAL's
 * {4,5,6,7,10,11,12,13} list. */
#define BOARD_HSMIC_POS_PIN          13   /* AIN7 */
#define BOARD_HSMIC_NEG_PIN          12   /* AIN6 */
#define BOARD_HSMIC_POS_CH           7
#define BOARD_HSMIC_NEG_CH           6

/* AUX-IN jack J1 (PJ-313D, 3-pole): tip -> C62 -> R53 1k -> AIN4, ring -> C63
 * -> R54 1k -> AIN5. The same two pins are the SW3/SW5 keys (to GND), so a pin
 * is either an analogue input or a key at any one time. */
#define BOARD_AUX_L_PIN              10   /* AIN4 */
#define BOARD_AUX_R_PIN              11   /* AIN5 */
#define BOARD_AUX_L_CH               4
#define BOARD_AUX_R_CH               5

/* PDM microphone MSM261DGT003 (MK1) on MIC_VDD = 3V3 via R43 100R. AUADC PDM
 * roles follow pin parity: even = PDM_CLK, odd = PDM_DIN. The mic's L/R select
 * (pin 2) is grounded -> LEFT slot. GPIO16/17 are ordinary pads (not the AON
 * pair GPIO18/19), so no HBN pad-ownership dance is needed here. */
#define BOARD_MIC_CLK_PIN            16
#define BOARD_MIC_DAT_PIN            17
#define BOARD_MIC_PDM_CHANNEL        0    /* 0 = left slot */

/* ---- RGB LED: WS2812-class, DI on GPIO24 ----
 * LED_VDD = VBUS - one 1N4148 drop (~4.3 V) so that a 3.3 V DI still clears
 * the 0.7 x VDD input threshold. GPIO24 % 4 == 0 is also SPI0's MOSI position
 * with the MOSI/MISO swap OFF (pin_swap_enable = 0 - the SDK default swap 1
 * makes it MISO), which is how the demo drives it with 4 SPI bits per WS2812
 * bit at 3.2 MHz. */
#define BOARD_RGB_LED_PIN            24
#define BOARD_RGB_LED_COUNT          1

/* ---- Keys ----
 * SW3/SW5 have no external pull - use the internal pull-up. SW4 has R78 100k
 * to 3V3. SW1 (BOOT) has R27 20k to GND and pulls HIGH when pressed. SW2 is
 * the reset button (SYS_RESET) and is not readable. */
#define BOARD_KEY_SW3_PIN            10   /* active low, internal pull-up; shares AIN4 */
#define BOARD_KEY_SW4_PIN            27   /* active low, 100k pull-up */
#define BOARD_KEY_SW5_PIN            11   /* active low, internal pull-up; shares AIN5 */
#define BOARD_KEY_BOOT_PIN           41   /* ACTIVE HIGH, 20k pull-down */
#define BOARD_FACTORY_RESET_GPIO     27   /* SW4, for apps that want one key */

/* ---- SD / SDIO slot on the SDH controller (SDH_0, function 27) ----
 * Every line has a 0R in series (R59-R64) and CMD/DAT0-3 have 5.1k pull-ups
 * to SD_VDD. The same six lines reach J_IO 6/8/10/12/14/16 through R65-R70
 * (0R), and the IPRO6S_FIN plugs into this slot as an SDIO slave.
 * GPIO14 gates SD_VDD through Q1 (P-FET, gate pulled down by R55 100k) when
 * JP3 is on 2-3: gate LOW = supply ON, which is also the power-on default.
 * GPIO15 is the slot's CD contact with R58 100k to 3V3; the contact closes to
 * the grounded shell when a card is seated, so LOW = card present. */
#define BOARD_SD_PWR_EN_PIN          14
#define BOARD_SD_PWR_ON_LEVEL        0
#define BOARD_SD_D1_PIN              28
#define BOARD_SD_D0_PIN              29
#define BOARD_SD_CLK_PIN             30
#define BOARD_SD_CMD_PIN             31
#define BOARD_SD_D3_PIN              32
#define BOARD_SD_D2_PIN              33
#define BOARD_SD_DET_PIN             15
#define BOARD_SD_DET_ACTIVE          0    /* level that means "card seated" */

/* ---- J_IO 2x12 header (0.1") ----
 *   1 3V3    2 GND      3 VBUS    4 GND      5 VDDIO1   6 SDIO_DAT1 (GPIO28)
 *   7 1V8    8 SDIO_DAT0 (GPIO29) 9 SYS_RESET  10 SDIO_CLK (GPIO30)
 *  11 GPIO41 (BOOT)     12 SDIO_CMD (GPIO31)   13 GPIO20 (UART TX)
 *  14 SDIO_DAT3 (GPIO32) 15 GPIO21 (UART RX)   16 SDIO_DAT2 (GPIO33)
 *  17 GPIO18  18 GPIO14 (SD_PWR_EN)  19 GPIO19  20 GPIO15 (SD_DET)
 *  21 GPIO27 (SW4)  22-24 GND
 * GPIO18/19 are the XTAL32K pads; nothing is fitted there, so they are free
 * GPIOs on the header. */
#define BOARD_HDR_GPIO18_PIN         18
#define BOARD_HDR_GPIO19_PIN         19

/* ---- Capability flags ---- */
#define BOARD_HAS_LCD                0
#define BOARD_HAS_CAMERA             1
#define BOARD_HAS_SPEAKER            1
#define BOARD_HAS_HEADPHONE          1
#define BOARD_HAS_HEADSET_MIC        1
#define BOARD_HAS_AUX_IN             1
#define BOARD_HAS_PDM_MIC            1
#define BOARD_HAS_SDCARD             1
#define BOARD_HAS_RGB_LED            1
#define BOARD_HAS_USB                1
#define BOARD_HAS_BATTERY            0
#define BOARD_HAS_WIFI_FIN_SLOT      1    /* IPRO6S_FIN over SDIO in the card slot */

/*==============================================================================
 * Pin Configuration Table
 *
 * Only what must be right from reset: the console, the SD/SDIO group, the
 * supply gate and card-detect, and the two outputs that should not float
 * (WS2812 DI, class-D enable). Camera, audio and analogue pins are muxed by
 * their drivers when the peripheral is brought up. There is no JTAG header
 * on this board, so nothing is muxed to function 14.
 *============================================================================*/

#ifdef CONFIG_DISABLE_SDC_PINMUX
/* External SF3 flash on GPIO28-33 - not how this board is built, kept so the
 * option still compiles. */
#define BOARD_PINMUX_TABLE \
    { GLB_GPIO_PIN_20, 7,  GPIO_MODE_AF,     GPIO_PULL_UP,   0, 1 }, /* UART0 TX */ \
    { GLB_GPIO_PIN_21, 7,  GPIO_MODE_AF,     GPIO_PULL_UP,   0, 1 }, /* UART0 RX */ \
    { GLB_GPIO_PIN_24, 11, GPIO_MODE_OUTPUT, GPIO_PULL_NONE, 0, 1 }, /* WS2812 DI */ \
    { GLB_GPIO_PIN_25, 11, GPIO_MODE_OUTPUT, GPIO_PULL_DOWN, 0, 1 }  /* SPK_EN */
#define BOARD_PINMUX_COUNT 4
#else
#define BOARD_PINMUX_TABLE \
    { GLB_GPIO_PIN_20, 7,  GPIO_MODE_AF,     GPIO_PULL_UP,   0, 1 }, /* UART0 TX */ \
    { GLB_GPIO_PIN_21, 7,  GPIO_MODE_AF,     GPIO_PULL_UP,   0, 1 }, /* UART0 RX */ \
    { GLB_GPIO_PIN_14, 11, GPIO_MODE_OUTPUT, GPIO_PULL_NONE, 0, 1 }, /* SD_PWR_EN */ \
    { GLB_GPIO_PIN_15, 11, GPIO_MODE_INPUT,  GPIO_PULL_UP,   0, 1 }, /* SD_DET */ \
    { GLB_GPIO_PIN_24, 11, GPIO_MODE_OUTPUT, GPIO_PULL_NONE, 0, 1 }, /* WS2812 DI */ \
    { GLB_GPIO_PIN_25, 11, GPIO_MODE_OUTPUT, GPIO_PULL_DOWN, 0, 1 }, /* SPK_EN */ \
    { GLB_GPIO_PIN_28, 27, GPIO_MODE_AF,     GPIO_PULL_UP,   2, 1 }, /* SD_D1 */ \
    { GLB_GPIO_PIN_29, 27, GPIO_MODE_AF,     GPIO_PULL_UP,   2, 1 }, /* SD_D0 */ \
    { GLB_GPIO_PIN_30, 27, GPIO_MODE_AF,     GPIO_PULL_NONE, 2, 1 }, /* SD_CLK */ \
    { GLB_GPIO_PIN_31, 27, GPIO_MODE_AF,     GPIO_PULL_UP,   2, 1 }, /* SD_CMD */ \
    { GLB_GPIO_PIN_32, 27, GPIO_MODE_AF,     GPIO_PULL_UP,   2, 1 }, /* SD_D3 */ \
    { GLB_GPIO_PIN_33, 27, GPIO_MODE_AF,     GPIO_PULL_UP,   2, 1 }  /* SD_D2 */
#define BOARD_PINMUX_COUNT 12
#endif

/**
 * Initialize all board pinmux settings.
 *
 * SD_PWR_EN goes to its ON level (which is also the hardware default through
 * R55), the amplifier is parked muted and the WS2812 data line low so the LED
 * does not latch garbage at power-up.
 */
#define BOARD_PINMUX_INIT() \
    do { \
        static GLB_GPIO_Cfg_Type pinmux_table[] = { BOARD_PINMUX_TABLE }; \
        for (int i = 0; i < BOARD_PINMUX_COUNT; i++) { \
            GLB_GPIO_Init(&pinmux_table[i]); \
        } \
        GLB_GPIO_Write(GLB_GPIO_PIN_24, 0); \
        GLB_GPIO_OUTPUT_Enable(GLB_GPIO_PIN_24); \
        GLB_GPIO_Write(GLB_GPIO_PIN_25, !BOARD_SPK_EN_ACTIVE); \
        GLB_GPIO_OUTPUT_Enable(GLB_GPIO_PIN_25); \
        GLB_GPIO_Write(GLB_GPIO_PIN_14, BOARD_SD_PWR_ON_LEVEL); \
        GLB_GPIO_OUTPUT_Enable(GLB_GPIO_PIN_14); \
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
 * MM (ISP / camera path) is deliberately NOT un-gated here; the camera driver
 * does it. A camera pipeline that forgets GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM)
 * gets zero frames and zero errors.
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
 * System Clock Configuration (external 32 MHz XTAL -> PLL 192 MHz)
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
 * RC32K. No 32.768 kHz crystal is fitted; its pads GPIO18/19 go to J_IO.
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

#endif /* __IPRO7AI_REEF_BOARD_CONFIG_H__ */
