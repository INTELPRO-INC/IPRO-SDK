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
 * @file st7789p3_port.h
 * @brief Board/SoC wiring for the HEM LCM-T2D4BP-084 (ST7789P3) SPI LCD.
 *
 * Module: Jiangxi Huaersheng LCM-T2D4BP-084 V1.0 (spec 2024-09-11)
 *   2.4" IPS TFT, transmissive / normally black, 240(H) x 320(V) dots,
 *   driver IC Sitronix ST7789P3, 4-line SPI ("SPI-4" / 4L1P), 17-pin FPC.
 *   VDD = VDDIO = 2.8..3.3 V; backlight = 4 white LEDs, Vf 3.0 V @ 80 mA.
 *
 * FPC pinout (module spec section 7) and how we use it:
 *
 *   1  GND      -> GND
 *   2  TP_SDA   \
 *   3  TP_SCL    |  capacitive touch controller - NOT CONNECTED in this build.
 *   4  TP_RST    |  The module spec names no touch IC and gives no register
 *   5  TP_INT    |  map, so touch is out of scope here.
 *   6  TP_VDD   /
 *   7  TE       -> NOT CONNECTED. Without it there is no tearing-effect sync;
 *                  full-frame writes can shear against the panel scan. Wire it
 *                  to a GPIO and use TEON (35h) if that ever matters.
 *   8  RESET    -> GPIO14   (active low)
 *   9  SDA      -> GPIO16   (SPI0 MOSI; also the read-back line, see below)
 *   10 SCL      -> GPIO15   (SPI0 SCLK)
 *   11 CS       -> GPIO10   (active low, driven as a plain GPIO)
 *   12 D/C      -> GPIO17   (0 = command, 1 = parameter/pixel data)
 *   13 GND      -> GND
 *   14 VDD      -> 3.3 V
 *   15 LEDA     \  backlight anode/cathode -> board boost / constant-current
 *   16 LEDK     /  rail. NOT a GPIO. GPIO12 below is only a board-side ENABLE.
 *   17 GND      -> GND
 *
 * IPRO7 SPI0 pad roles follow (pin % 4): 0 = MOSI, 1 = MISO, 2 = SS, 3 = SCLK.
 * So GPIO16 = MOSI and GPIO15 = SCLK are the hardware-SPI pads, while GPIO10
 * (SS position) and GPIO14 (SS position) are used as plain GPIOs, and GPIO17
 * (MISO position) carries D/C. Only SCLK and MOSI are muxed to GPIO_FUN_SPI;
 * everything else stays GPIO_FUN_GPIO.
 *
 * This is the same header the SDK's existing ili9341_spi.c drives
 * (apps/test/ipro_lvgl_demo, apps/multimedia/ipro_agora_*), so the wiring is
 * already proven on this board - only the panel is new.
 */
#ifndef ST7789P3_PORT_H
#define ST7789P3_PORT_H

/* ---- SPI peripheral ---- */
#define ST_SPI_ID        0          /* SPI0 */

/* Default SCLK. The ST7789P3 datasheet (Version 0.0, 2021/11) lists EVERY
 * serial AC timing as "TBD" - there is no published maximum to design against.
 * 16 MHz is chosen because it is the top of the IPRO7 XCLK-sourced SPI range
 * (SPI_SetClock switches XCLK -> BCLK above 16 MHz), which keeps the divider
 * on the cleaner path, and it is well under what ST7789-family parts normally
 * sustain. Full 240x320 RGB565 frame at 16 MHz ~= 77 ms. Raise it from the
 * shell with `lcd clk <hz>` once the link is proven on real hardware. */
#define ST_SPI_CLK_HZ    16000000u

/* SPI mode 0 (CPOL=0, CPHA=0): the ST7789P3 latches SDA on the SCL rising
 * edge with SCL idling low - matches the datasheet 4-line write figure and the
 * proven ili9341_spi.c settings on this same header. */
#define ST_SPI_CPOL      0          /* SPI_POLARITY_LOW  */
#define ST_SPI_CPHA      0          /* SPI_PHASE_1EDGE   */

/* ---- Pin map (user-supplied, matches ili9341_spi.h) ---- */
#define ST_PIN_MOSI      16         /* module SDA  -> SPI0 MOSI */
#define ST_PIN_SCLK      15         /* module SCL  -> SPI0 SCLK */
#define ST_PIN_CS        10         /* module CS,  active low, software GPIO */
#define ST_PIN_DC        17         /* module D/C, 0 = command 1 = data */
#define ST_PIN_RST       14         /* module RESET, active low */
#define ST_PIN_BL        12         /* board backlight ENABLE (not LEDA/LEDK) */

/* ---- Panel geometry ----
 * Native GRAM window is 240 columns x 320 rows. GATECTRL (E4h) defaults to
 * NL=27h -> (0x27+1)*8 = 320 gate lines and SCN=00h, so the visible area
 * starts at column 0 / row 0: unlike 240x240 or 135x240 ST7789 modules, this
 * panel needs NO CASET/RASET offset. */
#define ST_PANEL_W       240
#define ST_PANEL_H       320

/* Largest row length any orientation can ask for (landscape = 320 px). */
#define ST_MAX_DIM       320

/*
 * Orientation the panel comes up in, applied by st7789p3_init() after the
 * init sequence (which leaves MADCTL at 0 = portrait).
 *
 *   0 = portrait 240x320   1 = landscape 320x240
 *   2 = portrait flipped   3 = landscape flipped
 *
 * 3 rather than 1: both are landscape, 180 degrees apart, and 1 comes out
 * upside down with this panel's mounting. Bench-checked, not guessed.
 *
 * Setting it here means the boot colour bars, `lcd init` and the camera
 * preview all agree without anyone having to type `lcd dir` or `cam rot`.
 */
#define ST_DEFAULT_DIR   3

#endif /* ST7789P3_PORT_H */
