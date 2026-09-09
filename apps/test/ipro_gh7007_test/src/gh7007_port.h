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
 * @file gh7007_port.h
 * @brief Board/SoC wiring for the GH7007-01 SPI LCD on the IPRO7 Arduino board.
 *
 * This is a 4-wire SPI panel (D/CX command/data line + SDA/SCL + CS + RESET).
 * The module's FPC exposes only SDA (no separate read-back line), so the link
 * is WRITE-ONLY — only MOSI and SCLK are muxed to the SPI peripheral; the chip
 * never drives data back, so there is no ID read-back.
 *
 * ⚠ WIRING TRUTH (bench-proven 2026-07-29): the harness attaches the SIGNAL
 * wires to the module FPC in REVERSED order (pins were counted from the wrong
 * end when the wires were glued). The module pinout itself is the documented
 * GND/RS/CS/SCL/SDA/RESET/VCC/GND/LEDA/LEDK — but our five signal wires land
 * mirrored, so the true function-to-GPIO map is:
 *
 *   GPIO13 = RESET ( active low — holding it low kills the panel: proven )
 *   GPIO14 = SDA   ( module data in )
 *   GPIO15 = SCL   ( module clock — center wire, unaffected by the mirror )
 *   GPIO16 = CS    ( active low )
 *   GPIO17 = DC/RS ( 0=command 1=data )
 *
 * GPIO14 is NOT a hardware-SPI MOSI-capable pad (it is the SPI0 SS position),
 * so with this harness the bus runs BIT-BANGED (GH_BUS_BITBANG=1). To go back
 * to hardware SPI (~7 MHz like the vendor demo), physically swap the G14/G16
 * wires at the SoC end and set GH_BUS_BITBANG=0.
 *
 * Power: module VCC -> 3.3V, both GND pins -> GND. LEDA/LEDK go to the board's
 * backlight boost/constant-current rail — NOT to a GPIO. GPIO12 (GH_PIN_BL)
 * is a board-side backlight-enable output driven high at init; harmless if the
 * rail has no enable input.
 */
#ifndef GH7007_PORT_H
#define GH7007_PORT_H

/* ---- SPI peripheral ---- */
#define GH_SPI_ID        0          /* SPI0 */
#define GH_SPI_CLK_HZ    20000000u  /* 20 MHz default (bench 2026-07-30: clean up to
                                     * ~40 MHz in Mode 3 — the DS "t_wc>=100ns/10MHz"
                                     * figure is clearly conservative for this module.
                                     * 20 MHz keeps 2x margin; full 320x240 frame
                                     * ~62 ms. Tune at runtime with `lcd clk <hz>`. */

/* NOTE: the GH7007 uses SPI Mode 3 (CPOL=1, CPHA=1) — hardcoded in
 * gh_spi_program() (gh7007.c), matching the vendor spi_lcd.c driver. The macros
 * below are CPOL=0/CPHA=0 and apply ONLY to the ILI9341 branch; do not "fix" the
 * GH7007 path back to Mode 0. */
#define GH_SPI_CPOL      0          /* ILI9341 only: SPI_POLARITY_LOW  */
#define GH_SPI_CPHA      0          /* ILI9341 only: SPI_PHASE_1EDGE   */

/* ---- Pin map after the SoC-end G14/G16 header swap (2026-07-30): the module
 * SDA wire now lands on G16 = SPI0 MOSI, unlocking the hardware SPI engine.
 * (History: the glued harness had the signal block reversed; SDA originally
 * fell on G14, forcing months... days of bit-bang.) ---- */
#define GH_PIN_MOSI      16         /* module SDA -> SPI0 MOSI (HW engine) */
#define GH_PIN_SCLK      15         /* module SCL -> SPI0 SCLK */
#define GH_PIN_CS        14         /* software CS, active low */
#define GH_PIN_DC        17         /* 0=command 1=data (RS) */
#define GH_PIN_RST       13         /* hardware RST pulse (bench-proven) */
#define GH_PIN_BL        12         /* backlight enable — driven HIGH at bus init */

/* Default bus engine: 0 = hardware SPI0 (SDA on GPIO16, post-swap);
 * 1 = bit-banged 4-wire (pre-swap harness fallback, `lcd map` compatible). */
#define GH_BUS_BITBANG   0

/* Chip-select mode. With CS on a free GPIO (10, outside the SPI0 pad group) the
 * software CS toggles cleanly — this is what the proven driver does.
 *   GH_CS_SOFT (0) : software GPIO CS on GH_PIN_CS  <-- proven/default
 *   GH_CS_HW   (1) : SPI0 hardware SS on GPIO18 (per-transfer CS)
 *   GH_CS_TIED (2) : CS physically wired to GND; firmware never drives it */
#define GH_CS_SOFT       0
#define GH_CS_HW         1
#define GH_CS_TIED       2
#define GH_CS_MODE       GH_CS_SOFT

/* ---- Panel select ----
 * Same wiring drives two very different panels; pick which one is connected.
 *   GH7007  : GoHi GH7007-01, 320x240 SPI input upscaled by its scaler to 1024x600.
 *   ILI9341 : generic 240x320 TFT (ILI9341 / ST7789), native — no scaler. Used as a
 *             known-good standard panel to prove the SPI path independent of GH7007. */
#define GH_PANEL_GH7007    0
#define GH_PANEL_ILI9341   1
#define GH_PANEL           GH_PANEL_GH7007    /* <-- select the panel under test */

/* ---- Panel geometry ----
 * GH7007: the 320x240 SPI/GRAM input plane its scaler upscales to 1024x600.
 * ILI9341/ST7789: the native 240x320 frame buffer (written 1:1, no scaler).
 * Override at runtime with `lcd dims <w> <h>`. */
#if GH_PANEL == GH_PANEL_ILI9341
#define GH_IN_WIDTH      240
#define GH_IN_HEIGHT     320
#else
/* Bench 2026-07-29: the scaler's real input plane is 400x300 (readout row
 * length = p0E r65 burst 200x2 = 400 px; frame-test geometry: content written
 * as 320x240 lands at exactly 80%x80% of the panel, crosshair x=160 at 40%).
 * The old 320x240 assumption left the right 80 columns / bottom 60 rows
 * unwritten = the fixed flickering garbage lines. 400x300 -> x2.56/x2 ->
 * 1024x600. */
#define GH_IN_WIDTH      512    /* buffer/probe ceiling (lcd ruler sweeps this) */
#define GH_IN_HEIGHT     320
/* Real input plane, pinned by the color-coded ruler (2026-07-29): green(300)
 * marker ~17px from the right bezel, cyan(row-200) tick ~40 rows from the
 * bottom => 320x240, i.e. the original geometry. The earlier "78%/40%"
 * readings that suggested 400x300 were an artifact of the MADCTL LR mirror
 * (mirrors around a wider readout window, translating the content) — pinned
 * only after the mirror was disabled. */
#define GH_DEF_WIDTH     320
#define GH_DEF_HEIGHT    240
#endif

#endif /* GH7007_PORT_H */
