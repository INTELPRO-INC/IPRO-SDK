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
 * @file gh7007.h
 * @brief Minimal GH7007-01 4-wire SPI LCD driver (bring-up / test-pattern).
 *
 * Scope: prove the panel is alive over SPI — apply the vendor power-on /
 * scaler / GIP / gamma init sequence, then write RGB565 test patterns into the
 * GRAM input plane (the scaler upscales to the native 1024x600 panel). LVGL
 * integration is intentionally NOT part of this layer; it can be added on top
 * once the raw bring-up is verified.
 *
 * Pixel format: RGB565 (COLMOD 0x3A=0x75 in the init), sent MSB byte first.
 */
#ifndef GH7007_H
#define GH7007_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "gh7007_port.h"

#define GH7007_OK          0
#define GH7007_ERR_PARAM  -1

/* ---- RGB565 helpers ---- */
#define GH_RGB565(r, g, b) ((uint16_t)(((uint16_t)((r) & 0xF8) << 8) | \
                                       ((uint16_t)((g) & 0xFC) << 3) | \
                                       ((uint16_t)((b) & 0xF8) >> 3)))

#define GH_BLACK    0x0000u
#define GH_WHITE    0xFFFFu
#define GH_RED      0xF800u
#define GH_GREEN    0x07E0u
#define GH_BLUE     0x001Fu
#define GH_YELLOW   0xFFE0u
#define GH_CYAN     0x07FFu
#define GH_MAGENTA  0xF81Fu
#define GH_GRAY     0x8410u

/* ---- Bring-up ---- */

/** @brief Configure SPI0 bus + control GPIOs (no panel reset). Idempotent. */
void gh7007_bus_init(void);

/** @brief Stream the same solid-colour frame `frames` times back-to-back (GH7007
 *  video interface — tests whether the panel needs continuous frame refresh). */
void gh7007_stream_color(uint16_t color, int frames);

/** @brief Set the SPI clock in Hz at runtime (reprograms SPI0 if the bus is up).
 *  Lower it (e.g. 1 MHz) when driving the write-only link over flywire/clip leads,
 *  where 8 MHz corrupts the init/pixel bytes. Re-run `lcd init` after changing. */
void     gh7007_set_spi_clk(uint32_t hz);
uint32_t gh7007_get_spi_clk(void);

/** @brief Pulse the RESX line per datasheet timing (low >=30us, settle). */
void gh7007_reset(void);

/**
 * @brief Full bring-up: bus init + hardware reset + vendor init sequence +
 *        clear the visible plane to black.
 * @return GH7007_OK.
 */
int gh7007_init(void);

/* ---- Low-level access (also used by the `lcd cmd` shell command) ---- */

/** @brief Send one command byte (DC=0). */
void gh7007_write_cmd(uint8_t cmd);

/* Run init with a specific sequence variant: 0 = TM050 20260629 (default),
 * 1 = DSPA04 V0.1 (different GSRAM timing). Sticky until changed.
 * clear=0 skips the trailing full-frame black fill (no 2C/3C video traffic —
 * for isolating whether video framing poisons the command FSM). */
int gh7007_init_select(int variant, int clear);
int gh7007_init_opts(int clear);

/* Full init, then replay the manufacturer-page block AFTER SLPOUT — counters
 * the DS p.108 factory-default reload on modules with blank/mismatched OTP. */
int gh7007_init_post11(int variant);

/* Runtime SPI mode (0 or 3) — chip latches on SCLK rising in both; differ in
 * idle level. Reprograms SPI immediately; sticky until changed. */
int gh7007_get_spi_mode(void);
void gh7007_set_spi_mode(int mode);

/* Runtime bus format: 0 = 4-wire (DCX pin, HW SPI), 1 = 3-wire 9-bit bit-bang
 * (D/CX inline — for modules strapped 3-line). Re-muxes pins immediately. */
int gh7007_get_bus_3w(void);
void gh7007_set_bus_3w(int enable);

/* Bit-banged I2C address scan on SCL=SCLK / SDA=MOSI wires — detects a module
 * strapped CMD_SEL=0 (I2C command interface). Returns count of ACKed addrs. */
int gh7007_i2c_scan(uint8_t *found, int max_found);

/* Standard-DBI full-frame fill experiment: CASET/RASET + single-window 0x2C
 * burst (whole frame in one CS window), vs the vendor per-row 2C/3C framing. */
void gh7007_fill_dbi(uint16_t color);

/* Atomic per-row transfer (scheduler suspended across each row window). */
int gh7007_get_atomic(void);
void gh7007_set_atomic(int enable);

/* Fast video-row path (lcd fastrow 0|1, default 1): 2C/3C rows skip the
 * dcsetup delays — those are for cold isolated commands, and they run 2x long
 * anyway (SOC_TIMER_TICK_PER_US mismatch with the 2MHz board mtimer). */
int gh7007_get_fastrow(void);
void gh7007_set_fastrow(int enable);

/* Windowed partial-draw test (LVGL partial-refresh feasibility). */
void gh7007_box_windowed(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                         uint16_t color);

/* Paranoid mode: re-mux pins + reprogram SPI before every transaction —
 * discriminates host-side SPI/pinmux decay from chip-side deafness. */
int gh7007_get_paranoid(void);
void gh7007_set_paranoid(int enable);

/* Pure-software crosstalk discriminator: toggle SCLK/MOSI as GPIOs with CS
 * held HIGH (chip must ignore per DS p.46). which: bit0=SCLK bit1=MOSI. */
void gh7007_aggressor(int which, int seconds);

/* Pad drive strength: victim = RST/CS/DC, aggressor = SCLK/MOSI (0..3). */
void gh7007_set_drive(int victim, int aggressor);

/* Pixel G/B field repack (chip decodes 16-bit pixels as R5-B5-G6). */
int gh7007_get_swap_gb(void);
void gh7007_set_swap_gb(int enable);

/* Per-field bit rotation cancel (chip consumes fields bit-rotated). */
int gh7007_get_bitrev(void);
void gh7007_set_bitrev(int enable);
void gh7007_set_rot(uint8_t r, uint8_t b, uint8_t g);
void gh7007_get_rot(uint8_t out[3]);

/* Release RST to input+pull-up (SoC stops driving) / re-drive it high. */
void gh7007_rst_release(int release);

/* Window-opening setup delay in us (DCX park -> CS fall -> DCX drop spacing). */
uint32_t gh7007_get_dc_setup(void);
void gh7007_set_dc_setup(uint32_t us);

/* Vendor STM32-demo write framing: every byte in its own CS window. */
int gh7007_get_wr8(void);
void gh7007_set_wr8(int enable);

/* Vendor demo DMA-fill framing: 2A/2B, 2C own window, frame in one window. */
void gh7007_fill_vendor(uint16_t color);

/* Register patches: applied after every init (page unlock + write + display
 * off/on latch). For sweeping undocumented geometry registers. */
int gh7007_patch_add(uint8_t page, uint8_t reg, uint8_t val);
void gh7007_patch_clear(void);
int gh7007_patch_list(uint8_t out[][3], int max);

/* Runtime pin remap (bit-banged 4-wire): test any FPC-order hypothesis
 * without touching the glued wires. enable=0 restores defaults + HW SPI. */
void gh7007_set_map(int enable, uint8_t rst, uint8_t cs, uint8_t scl,
                    uint8_t sda, uint8_t dc);
int gh7007_get_map(uint8_t out[5]);

/* One-CS-window register write (cmd DCX=0, then params DCX=1) — the userguide
 * format required by the GH7007; use this for all config/init register access. */
void gh7007_write_reg(uint8_t cmd, const uint8_t *args, uint32_t nargs);

/** @brief Send @p len data/parameter bytes (DC=1). */
void gh7007_write_data(const uint8_t *buf, uint32_t len);

/* ---- Geometry (the GRAM input plane the scaler reads) ---- */

/** @brief Override the input-plane width/height used by fills/patterns. */
void gh7007_set_dims(uint16_t w, uint16_t h);

uint16_t gh7007_width(void);
uint16_t gh7007_height(void);

/* ---- Pixel output ---- */

/**
 * @brief Set the GRAM address window (CASET 0x2Ah + RASET 0x2Bh). Inclusive
 *        coordinates. A following RAMWR streams into this rectangle.
 */
void gh7007_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/**
 * @brief Begin a pixel burst into [x0,y0]..[x1,y1]: sets the window, issues
 *        RAMWR (0x2Ch) and holds CS low for the data phase.
 */
void gh7007_blit_start(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/** @brief Push one row of @p npix RGB565 pixels (npix <= GH_IN_WIDTH). */
void gh7007_blit_push_row(const uint16_t *row, uint16_t npix);

/** @brief Push one row of PRE-CONVERTED wire bytes (see gh7007_lut) — zero
 *         per-pixel work; the fast path for shadow-framebuffer streaming. */
void gh7007_blit_push_row_raw(const uint8_t *buf, uint16_t npix);

/** @brief Native RGB565 -> wire-format uint16 LUT (65536 entries; lazy-built,
 *         auto-rebuilt when swapgb/bitrev/rot change). Stored byte-swapped so
 *         the little-endian uint16 lays out MSB-first in memory. */
const uint16_t *gh7007_lut(void);
void gh7007_lut_rebuild(void);

/* ---- Background DMA frame streamer (HW-SPI 4-wire only) ----
 * Streams a complete wire-format frame (2C/3C row protocol) via DMA; per-row
 * window framing runs in the DMA-complete ISR, so the CPU is free during the
 * ~26 ms stream. Caller MUST cache-clean wirebuf before kicking (PSRAM is
 * cached, the DMA master does not snoop). One frame in flight at a time. */
int gh7007_stream_frame_async(const uint8_t *wirebuf, uint16_t rows,
                              uint32_t row_bytes);
int gh7007_stream_wait(uint32_t timeout_ms);   /* 0 = done, -1 = wedged+reset */
int gh7007_stream_busy(void);
void gh7007_stream_set_req(uint32_t req);      /* DMA req-mux A/B (7 vs 11) */
void gh7007_dma_probe(void);                   /* width x req truth table */
void gh7007_dma_probe2(void);                  /* arm-sequence bisection */

/** @brief End the burst started by gh7007_blit_start (drains SPI, raises CS). */
void gh7007_blit_end(void);

/** @brief Fill a rectangle with a solid RGB565 colour. */
void gh7007_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                      uint16_t color);

/** @brief Fill the whole input plane with a solid RGB565 colour. */
void gh7007_fill_screen(uint16_t color);

/* ---- Diagnostics (shell wire checks) ---- */

/** @brief Drive any pin as a plain GPIO output (multimeter wire checks). */
void gh7007_pin_out(uint8_t pin, int level);

/** @brief Read any pin as a plain GPIO input. */
int  gh7007_pin_in(uint8_t pin);

#endif /* GH7007_H */
