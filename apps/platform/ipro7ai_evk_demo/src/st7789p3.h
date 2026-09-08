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
 * @file st7789p3.h
 * @brief ST7789P3 4-wire SPI LCD driver for the LCM-T2D4BP-084 (240x320 IPS).
 *
 * Scope: bring-up. Prove the panel is alive over SPI, then push RGB565 test
 * patterns into its GRAM. No LVGL, no framebuffer, no DMA - transfers are
 * blocking and row-at-a-time out of a single static row buffer, so the driver
 * needs no heap and no PSRAM.
 *
 * Pixel format is RGB565. Bytes go on the wire high-byte-first; the driver
 * keeps its row buffer pre-swapped so 16-bit SPI frames land in that order
 * (SPI_MSB_BYTE0_DIRECTION_FIRST sends byte0 of each 16-bit frame first).
 *
 * See st7789p3_port.h for wiring and st7789p3_init_seq.h for what the init
 * sequences do and why.
 */
#ifndef ST7789P3_H
#define ST7789P3_H

#include <stdint.h>
#include <stdbool.h>

#include "st7789p3_port.h"

#define ST7789P3_OK          0
#define ST7789P3_ERR_PARAM  -1

/* ---- RGB565 helpers ---- */
#define ST_RGB565(r, g, b) ((uint16_t)(((uint16_t)((r) & 0xF8) << 8) | \
                                       ((uint16_t)((g) & 0xFC) << 3) | \
                                       ((uint16_t)((b) & 0xF8) >> 3)))

#define ST_BLACK    0x0000u
#define ST_WHITE    0xFFFFu
#define ST_RED      0xF800u
#define ST_GREEN    0x07E0u
#define ST_BLUE     0x001Fu
#define ST_YELLOW   0xFFE0u
#define ST_CYAN     0x07FFu
#define ST_MAGENTA  0xF81Fu
#define ST_GRAY     0x8410u

/* ---- Init sequence variants (see st7789p3_init_seq.h) ---- */
#define ST7789P3_SEQ_MINIMAL  0   /* datasheet defaults preserved (default) */
#define ST7789P3_SEQ_TUNED    1   /* + conventional power/gamma block       */

/* ---- Command framing (bring-up escape hatch) ---- */
#define ST7789P3_FRAME_WINDOW  0  /* one CS-low window per register (default) */
#define ST7789P3_FRAME_BYTE    1  /* one CS-low window per byte               */

/* ---------------------------------------------------------------------------
 * Bring-up
 * -------------------------------------------------------------------------*/

/** @brief Mux pins, configure SPI0 and the control GPIOs. Idempotent, and does
 *         NOT touch RESET or send anything to the panel. */
void st7789p3_bus_init(void);

/** @brief Pulse RESET low then release, with settle time.
 *
 * The datasheet's reset table (7.4.5) has TRW and TRT as "TBD", so the widths
 * here come from its spike-rejection rule instead: a RESX pulse shorter than
 * 5 us is REJECTED, longer than 9 us resets. 10 ms low is three orders of
 * magnitude clear of that, and the 150 ms settle covers the documented 120 ms
 * maximum blanking sequence plus NVM reload. */
void st7789p3_reset(void);

/**
 * @brief Full bring-up: bus init + hardware reset + init sequence + clear.
 * @param seq   ST7789P3_SEQ_MINIMAL or ST7789P3_SEQ_TUNED.
 * @param clear Non-zero to fill the screen black once the panel is on.
 * @return ST7789P3_OK, or ST7789P3_ERR_PARAM for an unknown sequence.
 */
int st7789p3_init(int seq, int clear);

/** @brief Which sequence the last successful init used. */
int st7789p3_get_seq(void);

/** @brief True once st7789p3_init() has completed. */
bool st7789p3_is_ready(void);

/* ---------------------------------------------------------------------------
 * Bus / panel configuration
 * -------------------------------------------------------------------------*/

/** @brief Set SCLK in Hz; reprograms SPI0 immediately if the bus is up.
 *
 * IPRO7 SPI_SetClock sources from XCLK at or below 16 MHz and from BCLK above
 * it, then picks the closest achievable rate <= the request, so the effective
 * clock can be lower than asked. Use st7789p3_get_spi_clk() to read back what
 * was requested; measure SCLK on a scope for the truth. */
void     st7789p3_set_spi_clk(uint32_t hz);
uint32_t st7789p3_get_spi_clk(void);

/** @brief Command framing: ST7789P3_FRAME_WINDOW or ST7789P3_FRAME_BYTE. */
void st7789p3_set_framing(int mode);
int  st7789p3_get_framing(void);

/** @brief Backlight ENABLE pin (GPIO12). Does not dim - the module has no
 *         LEDPWM pin, brightness is a property of the board's LED driver. */
void st7789p3_set_backlight(int on);
int  st7789p3_get_backlight(void);

/**
 * @brief Set orientation via MADCTL (36h) and swap the reported dimensions.
 * @param dir 0 = portrait 240x320, 1 = landscape 320x240,
 *            2 = portrait flipped, 3 = landscape flipped.
 * @param mirror Non-zero mirrors horizontally (toggles MX within the mode).
 * @return ST7789P3_OK or ST7789P3_ERR_PARAM.
 */
int st7789p3_set_dir(uint8_t dir, uint8_t mirror);
uint8_t st7789p3_get_dir(void);

/** @brief Select RGB (0) or BGR (1) channel order - MADCTL bit 3. Re-applies
 *         MADCTL immediately. Use this when red and blue come out swapped. */
void st7789p3_set_bgr(int bgr);
int  st7789p3_get_bgr(void);

/** @brief Display inversion: INVON (21h) / INVOFF (20h). */
void st7789p3_set_inversion(int on);
int  st7789p3_get_inversion(void);

/** @brief Sleep in (10h) / sleep out (11h), with the datasheet's 120 ms wait. */
void st7789p3_set_sleep(int sleep_in);

/** @brief Display off (28h) / on (29h) without touching GRAM contents. */
void st7789p3_set_display(int on);

/** @brief Restore the datasheet default gamma curves (E0h/E1h). */
void st7789p3_gamma_default(void);

/** @brief Current MADCTL byte as last written. */
uint8_t st7789p3_get_madctl(void);

/* ---------------------------------------------------------------------------
 * Low-level access
 * -------------------------------------------------------------------------*/

/** @brief Command byte with optional parameter bytes, per the current framing. */
void st7789p3_write_reg(uint8_t cmd, const uint8_t *args, uint32_t nargs);

/** @brief Command byte alone (DC low). */
void st7789p3_write_cmd(uint8_t cmd);

/** @brief Parameter/data bytes alone (DC high). */
void st7789p3_write_data(const uint8_t *buf, uint32_t len);

/**
 * @brief Read back a register over the shared SDA line (bit-banged).
 *
 * The module exposes no separate SDO pin, so this is the datasheet's
 * "Interface-I" read (section 8.3.5): command byte out on SDA, then ONE dummy
 * clock, then the chip drives the same SDA line. The driver re-muxes SCLK and
 * MOSI to GPIO for the transfer and restores hardware SPI afterwards.
 *
 * Command-Table-2 registers are NOT readable unless SPIRD (E7h bit 0) is set;
 * Command-Table-1 reads (04h RDDID, 09h RDDST, 0Ah RDDPM, DAh/DBh/DCh RDID1-3)
 * work as shipped.
 *
 * @param cmd   Read command.
 * @param out   Destination buffer.
 * @param nbytes Bytes to clock in (1 for RDID1-3, 3 for RDDID, 4 for RDDST).
 * @return ST7789P3_OK, or ST7789P3_ERR_PARAM on a bad argument.
 */
int st7789p3_read_reg(uint8_t cmd, uint8_t *out, uint32_t nbytes);

/* ---------------------------------------------------------------------------
 * Geometry and pixel output
 * -------------------------------------------------------------------------*/

uint16_t st7789p3_width(void);
uint16_t st7789p3_height(void);

/** @brief CASET (2Ah) + RASET (2Bh) for an inclusive rectangle, then leave the
 *         chip ready for RAMWR. */
void st7789p3_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/** @brief Open a pixel burst: window + RAMWR (2Ch), CS held low, 16-bit frames. */
void st7789p3_blit_start(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/** @brief Push npix RGB565 pixels. The buffer must already be in wire order -
 *         use st7789p3_swap() when filling it. */
void st7789p3_blit_push(const uint16_t *pixels, uint16_t npix);

/** @brief Close the burst opened by st7789p3_blit_start(). */
void st7789p3_blit_end(void);

/**
 * @brief Take/release the panel bus lock (recursive).
 *
 * fill_rect() and draw_row() take it themselves. Anything that drives
 * blit_start()/blit_push()/blit_end() directly - the pattern generators, the
 * camera preview - must wrap the WHOLE burst, otherwise two tasks interleave
 * their pixels into one RAMWR window and the panel shows shredded rows.
 */
void st7789p3_bus_lock(void);
void st7789p3_bus_unlock(void);

/** @brief Byte order helper: RGB565 value -> wire order for the row buffer. */
static inline uint16_t st7789p3_swap(uint16_t c)
{
    return (uint16_t)((c >> 8) | (c << 8));
}

/** @brief Fill an inclusive rectangle with one colour (clipped to the panel). */
void st7789p3_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                        uint16_t color);

/** @brief Fill the whole visible area with one colour. */
void st7789p3_fill_screen(uint16_t color);

/** @brief Single pixel. Cheap for markers, far too slow for bulk drawing. */
void st7789p3_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

/** @brief Push one row of pixels at (x0,y) - the pattern generators' workhorse.
 *         Values are RGB565 in host order; the driver swaps them. */
void st7789p3_draw_row(uint16_t x0, uint16_t y, const uint16_t *row, uint16_t npix);

/* ---------------------------------------------------------------------------
 * Diagnostics
 * -------------------------------------------------------------------------*/

/** @brief Drive any pin as a plain GPIO output (multimeter wire checks).
 *         Leaves the pin muxed as GPIO - re-run `lcd init` afterwards. */
void st7789p3_pin_out(uint8_t pin, int level);

/** @brief Read any pin as a plain GPIO input. */
int  st7789p3_pin_in(uint8_t pin);

#endif /* ST7789P3_H */
