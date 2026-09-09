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
 * @file gh7007.c
 * @brief Minimal GH7007-01 4-wire SPI LCD driver — bring-up / test-pattern.
 *
 * Transaction model: software chip-select (GPIO). A command is one CS-low
 * window with DC=0; parameters/pixels follow with DC=1. spi_transmit() returns
 * once the last byte is in the TX register, NOT when it has finished clocking
 * out, so CS is only raised after the bus reports idle (or the final byte is
 * chopped). Reads are not supported — the module exposes no MISO line.
 */

#include "gh7007.h"
#include "gh7007_init_seq.h"
#if GH_PANEL == GH_PANEL_GH7007
#include "gh7007_init_seq_dspa04.h"
#include "gh7007_init_seq_v800.h"
#include "gh7007_init_seq_hybrid.h"
#endif

#include <FreeRTOS.h>
#include <task.h>

#include "hal_spi.h"
#include "hal_gpio.h"
#include "drv_glb.h"    /* GLB_GPIO_Func_Init */
#include "drv_gpio.h"   /* GPIO_FUN_SPI / GPIO_FUN_GPIO */
#include "drv_common.h" /* arch_delay_us */
#include "ipro_log.h"

#define LOG_TAG "GH7007"

/* Bounded spin polling SPI bus-busy so a wiring fault can never hang the shell
 * task. ~tens of µs worst case per byte at 8 MHz. */
#define GH_BUSY_SPIN_LIMIT  2000000u

static bool     s_bus_ready;
#ifdef GH_DEF_WIDTH
static uint16_t s_width  = GH_DEF_WIDTH;
static uint16_t s_height = GH_DEF_HEIGHT;
#else
static uint16_t s_width  = GH_IN_WIDTH;
static uint16_t s_height = GH_IN_HEIGHT;
#endif
static uint32_t s_spi_clk_hz = GH_SPI_CLK_HZ;   /* runtime-tunable (lcd clk) for SI on flywire */
static int      s_spi_mode = 3;                 /* runtime-tunable (lcd mode): 0 or 3. Default 3:
                                                 * on THIS SoC's SPI IP, Mode 0 produced scattered
                                                 * white lines INDEPENDENT of clock (hold-time-like
                                                 * corruption at the chip's rising-edge sampling);
                                                 * Mode 3 is bench-clean up to ~40 MHz and is what
                                                 * the vendor STM32 demo used. Bit-bang (Mode-0
                                                 * shaped, huge setup/hold) was also clean — the
                                                 * issue is the IP's Mode-0 data-transition timing,
                                                 * not the chip's sampling edge. */
static int      s_bus_3w;                       /* runtime (lcd bus): 0 = 4-wire (DCX pin, HW SPI);
                                                 * 1 = 3-wire 9-bit bit-bang (D/CX bit inline, DS
                                                 * §6.4) — for modules strapped 3-line whose FPC
                                                 * RS pad is not routed to the chip's DCX. */
static int      s_paranoid;                     /* runtime (lcd paranoid): re-mux pins + reprogram
                                                 * SPI before EVERY transaction — isolates "host
                                                 * SPI/pinmux state dies between transactions"
                                                 * from "chip goes deaf". */
static void gh_spi_program(void);               /* fwd: program SPI0 from s_spi_clk_hz */

/* One physical row, RGB565 big-endian. Sized for the widest input plane. */
static uint8_t  s_linebuf[GH_IN_WIDTH * 2];
static uint16_t s_rowbuf[GH_IN_WIDTH];

/* Bench truth table 2026-07-29: fill red→red, green→blue, blue→green — the
 * chip decodes the 16-bit pixel with the G and B FIELDS exchanged (R5-B5-G6
 * layout). Repack standard RGB565 into the chip's layout (lcd swapgb 0|1). */
static int s_swap_gb = 1;

int gh7007_get_swap_gb(void) { return s_swap_gb; }
void gh7007_set_swap_gb(int enable) { s_swap_gb = enable ? 1 : 0; }

/* Bench 2026-07-30, decoded by the `lcd bitprobe` pattern: the chip consumes
 * each channel field bit-ROTATED — 5-bit fields rotate-left-3, the 6-bit G
 * field rotate-left-2 (full-scale values are rotation-invariant, which is why
 * pure colors always looked right while ramps/mid-codes scrambled). We
 * pre-rotate RIGHT by the same amounts so it cancels (lcd bitrev 0|1). */
static int s_bitrev = 1;

int gh7007_get_bitrev(void) { return s_bitrev; }
void gh7007_set_bitrev(int enable) { s_bitrev = enable ? 1 : 0; }

/* Per-field pre-rotation amounts (lcd rot <r> <b> <g>) — bench-converged:
 * grad band shape is the oracle (mid-scale crash = off by one). */
static uint8_t s_rot_r = 3, s_rot_b = 3, s_rot_g = 2;

void gh7007_set_rot(uint8_t r, uint8_t b, uint8_t g)
{
    s_rot_r = (uint8_t)(r % 5);
    s_rot_b = (uint8_t)(b % 6);
    s_rot_g = (uint8_t)(g % 5);
}

void gh7007_get_rot(uint8_t out[3])
{
    out[0] = s_rot_r; out[1] = s_rot_b; out[2] = s_rot_g;
}

/* Chip pixel model, fully decoded by `lcd bitprobe` (2026-07-30 v2 — the G
 * band's MSB segment rendering BLUE was the giveaway): standard RGB565 field
 * POSITIONS, but the [10:5] 6-bit field drives the BLUE subpixels and the
 * [4:0] 5-bit field drives GREEN, and each field's bits are consumed rotated:
 *   R5 [15:11] rotate-left-3, B6 [10:5] rotate-left-3, G5 [4:0] rotate-left-1.
 * Full-scale values are rotation-invariant -> pure colors always looked OK.
 * gh_px() builds the wire word so all of it cancels. */
static inline uint16_t gh_px(uint16_t c)
{
    uint16_t r  = (uint16_t)((c >> 11) & 0x1F);
    uint16_t g6 = (uint16_t)((c >> 5) & 0x3F);
    uint16_t b5 = (uint16_t)(c & 0x1F);
    if (!s_swap_gb) {
        return c;
    }
    uint16_t b6 = (uint16_t)(((b5 << 1) | (b5 >> 4)) & 0x3F);  /* 5->6 bit */
    uint16_t g5 = (uint16_t)(g6 >> 1);                          /* 6->5 bit */
    if (s_bitrev) {
        r  = (uint16_t)(((r >> s_rot_r) | (r << (5 - s_rot_r))) & 0x1F);
        b6 = (uint16_t)(((b6 >> s_rot_b) | (b6 << (6 - s_rot_b))) & 0x3F);
        g5 = (uint16_t)(((g5 >> s_rot_g) | (g5 << (5 - s_rot_g))) & 0x1F);
    }
    return (uint16_t)((r << 11) |               /* R5 -> wire [15:11]        */
                      (b6 << 5) |               /* B6 -> wire [10:5] (BLUE!) */
                      g5);                      /* G5 -> wire [4:0]  (GREEN) */
}

/* ----------------------------------------------------------------------- */
/* SPI primitives (software chip-select)                                   */
/* ----------------------------------------------------------------------- */

/* Runtime pin map (lcd map): the NEW module's FPC pad order is NOT the old
 * module's — pad 2 (wired to G13 as "DC") proved to be its RESET (holding it
 * low kills the self-running stripes; the wired "RESET" G17 does nothing).
 * With glued wiring, remapping happens here in software: when a map is
 * active, ALL five signals run as bit-banged GPIOs (arbitrary pins can't use
 * the HW SPI mux). */
static uint8_t s_pin_rst = GH_PIN_RST;
static uint8_t s_pin_cs  = GH_PIN_CS;
static uint8_t s_pin_scl = GH_PIN_SCLK;
static uint8_t s_pin_sda = GH_PIN_MOSI;
static uint8_t s_pin_dc  = GH_PIN_DC;
static int     s_bitbang = GH_BUS_BITBANG;   /* default per port.h (harness) */

static inline void gh_cs(int level)
{
#if GH_CS_MODE == GH_CS_SOFT
    gpio_write(s_pin_cs, level);
#else
    (void)level;                     /* HW SS drives CS, or CS tied to GND */
#endif
}
static inline void gh_dc(int level)  { gpio_write(s_pin_dc, level); }

/* Bit-banged 4-wire byte: data set while SCL low, chip latches on rising
 * edge (DS §6.4.2), MSB first, SCL idles low. */
static void gh_bb_byte(uint8_t b)
{
    for (int i = 7; i >= 0; i--) {
        gpio_write(s_pin_sda, (b >> i) & 1);
        gpio_write(s_pin_scl, 1);
        gpio_write(s_pin_scl, 0);
    }
}

/* Fast bit-bang burst: direct writes to the GPIO output register with a
 * shadow value — no HAL call / no read-modify-write per edge (~5-10x the
 * gpio_write path). Valid for pins 0-31 (ours: 13-17); assumes nothing else
 * flips pins in that register during the burst (re-synced at every call). */
#include "glb_reg.h"
#define GH_GPIO_OUT (*(volatile uint32_t *)(GLB_BASE + GLB_GPIO_CFGCTL33_OFFSET))

static void gh_bb_burst_fast(const uint8_t *buf, uint32_t len)
{
    const uint32_t sda = 1u << s_pin_sda;
    const uint32_t scl = 1u << s_pin_scl;
    uint32_t v = GH_GPIO_OUT & ~(sda | scl);      /* data 0, SCL idle low */
    for (uint32_t i = 0; i < len; i++) {
        uint8_t b = buf[i];
        for (int k = 7; k >= 0; k--) {
            uint32_t d = ((b >> k) & 1u) ? (v | sda) : v;
            GH_GPIO_OUT = d;                       /* set data, SCL low   */
            GH_GPIO_OUT = d | scl;                 /* rising edge latches */
            GH_GPIO_OUT = d;                       /* SCL back low        */
        }
    }
}

static inline void gh_wait_idle(void)
{
    /* spi_transmit() returns once the last byte is queued — the busy flag may
     * not have ASSERTED yet, so a plain "wait not-busy" can fall straight
     * through while bits are still clocking out; raising CS (or flipping DC)
     * there chops the byte. Symptom on the bench: after init, only the FIRST
     * shell command landed — later (cache-hot, faster) calls always lost their
     * byte. So: give busy a bounded chance to assert, wait for it to clear,
     * then pad one byte-time at the current clock as a hard guarantee. */
    uint32_t spin = 256;
    while (!spi_ioctl(GH_SPI_ID, SPI_CTRL_GET_BUS_BUSY_STATUS, NULL) && --spin) {
    }
    spin = GH_BUSY_SPIN_LIMIT;
    while (spi_ioctl(GH_SPI_ID, SPI_CTRL_GET_BUS_BUSY_STATUS, NULL) && --spin) {
    }
    /* Tail pad is computed from the REQUESTED clock; if the SoC divider lands
     * below the request the real byte time is longer — pad 4x with a 20us
     * floor so an isolated 1-byte command can never be chopped at CS-raise. */
    uint32_t us = ((8u * 1000000u) / s_spi_clk_hz + 1u) * 4u;
    arch_delay_us(us < 20u ? 20u : us);
}

static void gh_delay_ms(uint32_t ms)
{
    if (ms) {
        vTaskDelay(pdMS_TO_TICKS(ms));
    }
}

/* Unified TX: HW SPI (+busy wait) or bit-bang when a pin map is active.
 * NO scheduler pauses inside a transfer — a 1 ms stall mid-row (CS low, row
 * half-written) corrupted the same byte offsets of every row = fixed-position
 * line artifacts. Pacing belongs BETWEEN rows (CS high), see fill paths. */
static void gh_tx(const uint8_t *buf, uint32_t len)
{
    if (s_bitbang) {
        if (s_pin_sda < 32 && s_pin_scl < 32) {
            gh_bb_burst_fast(buf, len);
        } else {
            for (uint32_t i = 0; i < len; i++) {
                gh_bb_byte(buf[i]);
            }
        }
    } else {
        spi_transmit(GH_SPI_ID, (void *)buf, len);
        gh_wait_idle();
    }
}

void gh7007_write_cmd(uint8_t cmd)
{
    gh_dc(1);            /* park high: window must OPEN with a DCX falling edge */
    arch_delay_us(10);
    gh_cs(0);
    arch_delay_us(10);
    gh_dc(0);            /* falling edge arms the command latch (bench law) */
    arch_delay_us(5);
    gh_tx(&cmd, 1);
    gh_cs(1);
}

void gh7007_write_data(const uint8_t *buf, uint32_t len)
{
    if (!buf || !len) {
        return;
    }
    gh_dc(1);            /* data */
    gh_cs(0);
    gh_tx(buf, len);
    gh_cs(1);
}

/* 3-wire 9-bit bit-bang write: each byte is one 9-bit symbol — D/CX bit first,
 * then D7..D0, MSB first; data set while SCLK low, chip latches on the rising
 * edge (DS §6.4.2), SCLK idles low (DS §6.4 figures). Yields periodically so a
 * full-frame stream (seconds at bit-bang speed) can't starve the scheduler. */
static void gh_3w_write(int dcx, const uint8_t *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        uint16_t sym = (uint16_t)(((dcx & 1) << 8) | buf[i]);
        for (int b = 8; b >= 0; b--) {
            gpio_write(GH_PIN_MOSI, (sym >> b) & 1);
            gpio_write(GH_PIN_SCLK, 1);
            gpio_write(GH_PIN_SCLK, 0);
        }
        if ((i & 0xFF) == 0xFF) {
            vTaskDelay(1);
        }
    }
}

int gh7007_get_bus_3w(void) { return s_bus_3w; }

void gh7007_set_bus_3w(int enable)
{
    s_bus_3w = enable ? 1 : 0;
    if (!s_bus_ready) {
        return;                     /* bus_init will apply it */
    }
    uint8_t pins[2] = { GH_PIN_MOSI, GH_PIN_SCLK };
    if (s_bus_3w) {
        GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pins, 2);
        gpio_set_mode(GH_PIN_SCLK, GPIO_OUTPUT_MODE);
        gpio_write(GH_PIN_SCLK, 0);          /* idle low */
        gpio_set_mode(GH_PIN_MOSI, GPIO_OUTPUT_MODE);
        gpio_write(GH_PIN_MOSI, 0);
    } else {
        GLB_GPIO_Func_Init(GPIO_FUN_SPI, pins, 2);
        gh_spi_program();
    }
}

/* Register write per the GH7007-01 SPI userguide ("Write 1-Byte Command + n-Byte
 * Data"): command AND parameters share ONE CS-low window; only DCX toggles between
 * the command byte (DCX=0) and the parameter bytes (DCX=1). If the chip closes the
 * command on CS rising, splitting into two CS windows silently drops every
 * parameter — so never send init/config registers with write_cmd + write_data. */
/* Window-opening setup delay (lcd dcsetup <us>) — sweeps the DCX-park /
 * CS-fall / DCX-drop spacing without reflashing. Bench sweep: 10us was not
 * enough where a seconds-old inherited DC level worked. */
static uint32_t s_dc_setup_us = 10;

static void gh_delay_setup(void)
{
    if (s_dc_setup_us >= 1000u) {
        vTaskDelay(pdMS_TO_TICKS(s_dc_setup_us / 1000u));
    } else {
        arch_delay_us(s_dc_setup_us);
    }
}

uint32_t gh7007_get_dc_setup(void) { return s_dc_setup_us; }
void gh7007_set_dc_setup(uint32_t us) { s_dc_setup_us = us; }

/* Verbatim replication of the vendor STM32 demo write primitives
 * (spi_lcd.c SPI_CmdWrite/SPI_DataWrite): EVERY byte — command and each
 * parameter — in its OWN CS window, DC set after CS falls. The demo's
 * actually-running init is write-identical to ours; this framing is the one
 * structural difference left between their working bench and ours. */
static int s_wr8;
int gh7007_get_wr8(void) { return s_wr8; }
void gh7007_set_wr8(int enable) { s_wr8 = enable ? 1 : 0; }

int gh7007_get_paranoid(void) { return s_paranoid; }

void gh7007_set_paranoid(int enable) { s_paranoid = enable ? 1 : 0; }

/* Re-apply the full bus setup: SPI pinmux + SPI config + control-pin GPIO
 * modes + idle levels. Used per-transaction in paranoid mode. */
static void gh_bus_reapply(void)
{
    uint8_t spi_pins[2] = { GH_PIN_MOSI, GH_PIN_SCLK };
    GLB_GPIO_Func_Init(GPIO_FUN_SPI, spi_pins, 2);
    uint8_t gpio_pins[3] = { GH_PIN_CS, GH_PIN_DC, GH_PIN_RST };
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, gpio_pins, 3);
    gpio_set_mode(GH_PIN_CS, GPIO_OUTPUT_MODE);
    gpio_write(GH_PIN_CS, 1);
    gpio_set_mode(GH_PIN_DC, GPIO_OUTPUT_MODE);
    gpio_set_mode(GH_PIN_RST, GPIO_OUTPUT_MODE);
    gpio_write(GH_PIN_RST, 1);
    gh_spi_program();
}

void gh7007_write_reg(uint8_t cmd, const uint8_t *args, uint32_t nargs)
{
    if (s_paranoid && !s_bus_3w && !s_bitbang) {
        gh_bus_reapply();
    }
    if (s_wr8 && !s_bus_3w && !s_bitbang) {
        /* vendor STM32-demo framing: per-byte CS windows, DC after CS-fall */
        gh_cs(0);
        gh_dc(0);
        spi_transmit(GH_SPI_ID, &cmd, 1);
        gh_wait_idle();
        gh_cs(1);
        for (uint32_t i = 0; i < nargs; i++) {
            gh_delay_setup();
            gh_cs(0);
            gh_dc(1);
            spi_transmit(GH_SPI_ID, (void *)&args[i], 1);
            gh_wait_idle();
            gh_cs(1);
        }
        return;
    }
    if (s_bus_3w) {
        gh_cs(0);
        gh_3w_write(0, &cmd, 1);
        if (args && nargs) {
            gh_3w_write(1, args, nargs);
        }
        gh_cs(1);
        return;
    }
    /* BENCH LAW (2026-07-29, refined): the chip latches a command PERSISTENTLY
     * only when its window OPENS with a DCX falling edge — every historical
     * "first command works" case had DC=1 (left over from a data phase) at
     * CS-fall. DC parked low at CS-fall = command ignored; a trailing DC rise
     * inside the window = transient half-execution aborted at CS-rise (screen
     * blips, nothing sticks). So: park DC high before CS falls, drop it for
     * the command byte, and never touch it again in a zero-param window. */
    gh_dc(1);
    gh_delay_setup();           /* DCX setup before CS falls — runtime-tunable
                                 * (lcd dcsetup <us>): inherited DC=1 worked
                                 * where freshly-parked didn't, so sweep this */
    gh_cs(0);
    gh_delay_setup();           /* CS-low settle before the falling edge */
    gh_dc(0);
    gh_delay_setup();
    gh_tx(&cmd, 1);
    if (args && nargs) {
        gh_dc(1);
        arch_delay_us(5);
        gh_tx(args, nargs);
    }
    gh_cs(1);
}

/* ----------------------------------------------------------------------- */
/* Bus + reset                                                             */
/* ----------------------------------------------------------------------- */

void gh7007_bus_init(void)
{
    if (s_bitbang) {
        /* bit-bang default (harness: SDA on a non-MOSI pad) — all five
         * signals as plain GPIOs; no SPI peripheral involved at all. */
        gh7007_set_map(1, s_pin_rst, s_pin_cs, s_pin_scl, s_pin_sda, s_pin_dc);
        uint8_t bl[1] = { GH_PIN_BL };
        GLB_GPIO_Func_Init(GPIO_FUN_GPIO, bl, 1);
        gpio_set_mode(GH_PIN_BL, GPIO_OUTPUT_MODE);
        gpio_write(GH_PIN_BL, 1);
        return;
    }
#if GH_CS_MODE == GH_CS_HW
    /* MOSI/SCLK + SS(18) -> hardware SPI0. The SPI block drives SS as CS. */
    uint8_t spi_pins[3] = { GH_PIN_MOSI, GH_PIN_SCLK, GH_PIN_CS };
    GLB_GPIO_Func_Init(GPIO_FUN_SPI, spi_pins, 3);
    uint8_t gpio_pins[2] = { GH_PIN_DC, GH_PIN_RST };
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, gpio_pins, 2);
#elif GH_CS_MODE == GH_CS_SOFT
    /* MOSI/SCLK -> SPI0; CS is a plain GPIO (only valid if GH_PIN_CS is OUTSIDE
     * the 16-19 group — otherwise SPI0 locks it and it never toggles). */
    uint8_t spi_pins[2] = { GH_PIN_MOSI, GH_PIN_SCLK };
    GLB_GPIO_Func_Init(GPIO_FUN_SPI, spi_pins, 2);
    uint8_t gpio_pins[3] = { GH_PIN_CS, GH_PIN_DC, GH_PIN_RST };
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, gpio_pins, 3);
    gpio_set_mode(GH_PIN_CS, GPIO_OUTPUT_MODE);
    gpio_write(GH_PIN_CS, 1);            /* CS idle high */
#else /* GH_CS_TIED — CS wired to GND, firmware never drives it */
    /* Only MOSI/SCLK go to SPI0; the panel CS pad is tied to GND (always selected). */
    uint8_t spi_pins[2] = { GH_PIN_MOSI, GH_PIN_SCLK };
    GLB_GPIO_Func_Init(GPIO_FUN_SPI, spi_pins, 2);
    uint8_t gpio_pins[2] = { GH_PIN_DC, GH_PIN_RST };
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, gpio_pins, 2);
#endif
    gpio_set_mode(GH_PIN_DC, GPIO_OUTPUT_MODE);
    gpio_write(GH_PIN_DC, 1);            /* DC idle high (data) */
    gpio_set_mode(GH_PIN_RST, GPIO_OUTPUT_MODE);
    gpio_write(GH_PIN_RST, 1);           /* RST inactive (active low) */

    /* Backlight enable (GPIO12, proven driver) — plain GPIO, drive HIGH so the panel
     * lights. Without this the display can be fully dark even when SPI works. */
    {
        uint8_t bl[1] = { GH_PIN_BL };
        GLB_GPIO_Func_Init(GPIO_FUN_GPIO, bl, 1);
        gpio_set_mode(GH_PIN_BL, GPIO_OUTPUT_MODE);
        gpio_write(GH_PIN_BL, 1);
    }

    /* setConfig() only fills the config struct; spi_init() READS it to program
     * the hardware — so setConfig MUST precede init, else init applies the
     * board-default SPI0 config (which may swap MOSI/MISO and use hardware CS). */
    gh_spi_program();
    s_bus_ready = true;
    if (s_bus_3w) {
        gh7007_set_bus_3w(1);       /* selected before init: apply GPIO bit-bang mux */
    }
}

/* Program SPI0 from the current s_spi_clk_hz. Split out so the clock can be changed at
 * runtime (lcd clk) without re-doing pinmux/GPIO — key for a flywire/clip setup where
 * 8 MHz corrupts the write-only link and a slower clock recovers it. */
static void gh_spi_program(void)
{
    spi_device_t dev = {
        .caps            = 0,
        .id              = GH_SPI_ID,
        .clk             = s_spi_clk_hz,
        .mode            = SPI_MASTER_MODE,
        .direction       = SPI_MSB_BYTE0_DIRECTION_FIRST,
        /* Match the proven agora ili9341 driver: ignore_cs=0 (CS handled manually via
         * GPIO). Forcing 1 can leave the peripheral not clocking data out on this SoC. */
        .ignore_cs       = 0,
#if GH_PANEL == GH_PANEL_GH7007
        /* runtime-selectable (lcd mode 0|3) — chip samples MOSI on SCLK rising in
         * both; they differ only in idle level / first-edge phase. */
        .clk_polaraity   = (s_spi_mode == 3) ? SPI_POLARITY_HIGH : SPI_POLARITY_LOW,
        .clk_phase       = (s_spi_mode == 3) ? SPI_PHASE_2EDGE   : SPI_PHASE_1EDGE,
#else
        .clk_polaraity   = GH_SPI_CPOL,        /* ILI9341: mode 0 (CPOL=0) */
        .clk_phase       = GH_SPI_CPHA,        /*          CPHA=0 */
#endif
        .datasize        = SPI_DATASIZE_8BIT,
        .fifo_threshold  = 0,
        .pin_swap_enable = 0,                  /* GPIO16 MOSI */
        .delitch_cnt     = 0,
        .callback        = NULL,
    };
    spi_setConfig(&dev);
    spi_init(GH_SPI_ID);
    spi_enable(GH_SPI_ID, 1);
}

uint32_t gh7007_get_spi_clk(void) { return s_spi_clk_hz; }

int gh7007_get_spi_mode(void) { return s_spi_mode; }

void gh7007_set_spi_mode(int mode)
{
    s_spi_mode = (mode == 0) ? 0 : 3;
    if (s_bus_ready) {
        gh_spi_program();
    }
}

void gh7007_set_spi_clk(uint32_t hz)
{
    if (hz == 0u) return;
    s_spi_clk_hz = hz;
    if (s_bus_ready) {
        gh_spi_program();
    }
}

void gh7007_reset(void)
{
    /* Verbatim vendor sample timing (spi_lcd.c spi_lcd_init lines 170-178):
     * 50ms settle -> RST high 10ms -> RST low 50ms -> RST high -> 200ms settle. */
    gh_cs(1);
    gh_delay_ms(50);
    gpio_write(s_pin_rst, 1);
    gh_delay_ms(10);
    gpio_write(s_pin_rst, 0);             /* reset asserted, low 50 ms */
    gh_delay_ms(50);
    gpio_write(s_pin_rst, 1);
    gh_delay_ms(200);                     /* reset complete + settle */
}

/* Init-sequence variant: 0 = default (TM050 20260629); 1 = DSPA04 V0.1 — same
 * chip, different GSRAM timing + glass params. Selectable so a module built
 * with different GSRAM can be tried from the shell without reflashing. */
static int s_init_variant;

/* Register patches (lcd patch): applied automatically after every init —
 * re-unlocks the manufacturer pages, writes each patch, returns to page 00,
 * then latches with a DISPOFF/DISPON cycle. For sweeping undocumented
 * geometry registers without reflashing. */
#define GH_MAX_PATCH 16
static struct { uint8_t page, reg, val; } s_patch[GH_MAX_PATCH];
static int s_npatch;

int gh7007_patch_add(uint8_t page, uint8_t reg, uint8_t val)
{
    if (s_npatch >= GH_MAX_PATCH) {
        return -1;
    }
    s_patch[s_npatch].page = page;
    s_patch[s_npatch].reg = reg;
    s_patch[s_npatch].val = val;
    return ++s_npatch;
}

void gh7007_patch_clear(void) { s_npatch = 0; }

int gh7007_patch_list(uint8_t out[][3], int max)
{
    int n = (s_npatch < max) ? s_npatch : max;
    for (int i = 0; i < n; i++) {
        out[i][0] = s_patch[i].page;
        out[i][1] = s_patch[i].reg;
        out[i][2] = s_patch[i].val;
    }
    return n;
}

static void gh_apply_patches(void)
{
    if (!s_npatch) {
        return;
    }
    uint8_t v, cur = 0xFF;
    v = 0x01; gh7007_write_reg(0xEE, &v, 1);
    v = 0x07; gh7007_write_reg(0xEA, &v, 1);
    v = 0x12; gh7007_write_reg(0xEB, &v, 1);
    for (int i = 0; i < s_npatch; i++) {
        if (s_patch[i].page != cur) {
            cur = s_patch[i].page;
            gh7007_write_reg(0xEE, &cur, 1);
        }
        gh7007_write_reg(s_patch[i].reg, &s_patch[i].val, 1);
    }
    v = 0x00; gh7007_write_reg(0xEE, &v, 1);
    gh7007_write_reg(0x28, NULL, 0);      /* latch: display off/on cycle */
    gh_delay_ms(20);
    gh7007_write_reg(0x29, NULL, 0);
    IPRO_LOGI(LOG_TAG, "applied %d register patch(es) + DISPOFF/ON latch", s_npatch);
}

int gh7007_init_select(int variant, int clear)
{
    s_init_variant = (variant >= 0 && variant <= 3) ? variant : 0;
    return gh7007_init_opts(clear);
}

int gh7007_init(void)
{
    return gh7007_init_opts(1);
}

int gh7007_init_opts(int clear)
{
    const gh_init_cmd_t *seq = GH7007_INIT_SEQ;
    size_t seq_len = GH7007_INIT_SEQ_LEN;
#if GH_PANEL == GH_PANEL_GH7007
    if (s_init_variant == 1) {
        seq = GH7007_INIT_SEQ_DSPA04;
        seq_len = GH7007_INIT_SEQ_DSPA04_LEN;
    } else if (s_init_variant == 2) {
        seq = GH7007_INIT_SEQ_V800;
        seq_len = GH7007_INIT_SEQ_V800_LEN;
    } else if (s_init_variant == 3) {
        seq = GH7007_INIT_SEQ_HYBRID;
        seq_len = GH7007_INIT_SEQ_HYBRID_LEN;
    }
#endif

    if (!s_bus_ready) {
        gh7007_bus_init();
    }
    gh7007_reset();

    for (size_t i = 0; i < seq_len; i++) {
        const gh_init_cmd_t *c = &seq[i];
#if GH_PANEL == GH_PANEL_GH7007
        /* One CS window per register (userguide waveform). The SPI4W-Video frame
         * path is different: the 0x2C/0x3C row header and the row pixels ARE in
         * separate CS windows (userguide p.2) — do not unify that one. */
        gh7007_write_reg(c->cmd, c->args, c->nargs);
#else
        gh7007_write_cmd(c->cmd);
        if (c->nargs) {
            gh7007_write_data(c->args, c->nargs);
        }
#endif
        gh_delay_ms(c->delay_ms);
    }

    /* Write-only bus (no MISO): completing the burst proves only that the SoC
     * clocked the bytes out, NOT that the panel received them. */
    IPRO_LOGI(LOG_TAG, "init sequence clocked out (%u writes, variant %d, clear %d) — "
              "write-only bus, verify by display or VDD current.",
              (unsigned)seq_len, s_init_variant, clear);
    gh_apply_patches();

    if (clear) {
        /* NOTE: this is 240 rows of 2C/3C video framing — suspect of poisoning
         * the command FSM (post-init "one command then deaf"). Use `lcd init nc`
         * to run the pure register sequence with NO video traffic. */
        gh7007_fill_screen(GH_BLACK);
    }
    return GH7007_OK;
}

/* Symptom-B experiment (DS p.108): SLPOUT "loads all display supplier's
 * factory default values to the registers" — the vendor sequence sends every
 * manufacturer-page write BEFORE 0x11, so on a module with blank/mismatched
 * OTP the whole GSRAM/scaler setup is wiped 5 ms after SLPOUT. This variant
 * runs the full sequence, then REPLAYS the entire manufacturer-page block
 * (everything before the 0xEA=0 teardown) after SLPOUT, then re-selects
 * page 00 (EA/EB left at the unlock values on purpose — the audit flagged the
 * vendor teardown as page-ambiguous). */
int gh7007_init_post11(int variant)
{
    s_init_variant = (variant == 1) ? 1 : 0;
    int r = gh7007_init_opts(0);
    if (r != GH7007_OK) {
        return r;
    }
    const gh_init_cmd_t *seq = GH7007_INIT_SEQ;
    size_t len = GH7007_INIT_SEQ_LEN;
#if GH_PANEL == GH_PANEL_GH7007
    if (s_init_variant == 1) {
        seq = GH7007_INIT_SEQ_DSPA04;
        len = GH7007_INIT_SEQ_DSPA04_LEN;
    }
#endif
    size_t end = len;
    for (size_t i = 0; i < len; i++) {
        if (seq[i].cmd == 0xEA && seq[i].nargs == 1 && seq[i].args[0] == 0x00) {
            end = i;
            break;
        }
    }
    for (size_t i = 0; i < end; i++) {
        gh7007_write_reg(seq[i].cmd, seq[i].args, seq[i].nargs);
        gh_delay_ms(seq[i].delay_ms);
    }
    uint8_t page0 = 0x00;
    gh7007_write_reg(0xEE, &page0, 1);
    IPRO_LOGI(LOG_TAG, "post11: replayed %u manufacturer-page ops after SLPOUT",
              (unsigned)end);
    return GH7007_OK;
}

/* ----------------------------------------------------------------------- */
/* Geometry                                                                */
/* ----------------------------------------------------------------------- */

void gh7007_set_dims(uint16_t w, uint16_t h)
{
    if (w == 0 || w > GH_IN_WIDTH) {
        w = GH_IN_WIDTH;
    }
    if (h == 0) {
        h = GH_IN_HEIGHT;
    }
    s_width  = w;
    s_height = h;
}

uint16_t gh7007_width(void)  { return s_width; }
uint16_t gh7007_height(void) { return s_height; }

/* ----------------------------------------------------------------------- */
/* Pixel output                                                            */
/* ----------------------------------------------------------------------- */

void gh7007_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t ca[4] = { (uint8_t)(x0 >> 8), (uint8_t)x0,
                      (uint8_t)(x1 >> 8), (uint8_t)x1 };
    uint8_t ra[4] = { (uint8_t)(y0 >> 8), (uint8_t)y0,
                      (uint8_t)(y1 >> 8), (uint8_t)y1 };
    gh7007_write_cmd(0x2A);              /* CASET */
    gh7007_write_data(ca, 4);
    gh7007_write_cmd(0x2B);              /* RASET */
    gh7007_write_data(ra, 4);
}

static uint16_t s_video_row;   /* GH7007 SPI4W-Video: current row within the frame */

/* Atomic row transfer (lcd atomic 0|1): suspend the scheduler across each
 * row's CS window so a task switch can never stall the SPI mid-row — probes
 * the "mid-row pause corrupts the row" failure class on the HW-SPI path. */
static int s_atomic;
int gh7007_get_atomic(void) { return s_atomic; }
void gh7007_set_atomic(int enable) { s_atomic = enable ? 1 : 0; }

void gh7007_blit_start(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
#if GH_PANEL == GH_PANEL_GH7007
    /* GH7007 SPI4W-Video (vendor 4.10.1): no CASET/RASET/RAMWR window. A frame is
     * streamed row by row; each row is its own CS-low window with a header byte
     * (0x2C on the first row, 0x3C on the rest), then that row's RGB565 pixels. */
    (void)x0; (void)y0; (void)x1; (void)y1;
    s_video_row = 0;
#else
    gh7007_set_window(x0, y0, x1, y1);
    gh7007_write_cmd(0x2C);              /* RAMWR */
    gh_dc(1);                            /* hold DC=data, CS low for the burst */
    gh_cs(0);
#endif
}

void gh7007_blit_push_row(const uint16_t *row, uint16_t npix)
{
    if (!row || !npix) {
        return;
    }
    if (npix > GH_IN_WIDTH) {
        npix = GH_IN_WIDTH;
    }
    for (uint16_t i = 0; i < npix; i++) {
        uint16_t px = gh_px(row[i]);
        s_linebuf[2 * i]     = (uint8_t)(px >> 8);       /* MSB first */
        s_linebuf[2 * i + 1] = (uint8_t)(px & 0xFF);
    }
#if GH_PANEL == GH_PANEL_GH7007
    {
        /* One CS-low window per row: header byte (0x2C first row / 0x3C after,
         * DCX=0) then that row's pixels (DCX=1), CS raised only at row end — the
         * same userguide packet format as register writes. The earlier split-CS
         * form (CS pulsed high between header and pixels, inferred from the vendor
         * MCU sample) left fills with NO effect on the new panel: the chip closes
         * the 2C/3C command on CS rising and drops the whole pixel window. */
        uint8_t hdr = (s_video_row == 0) ? 0x2C : 0x3C;
        if (s_atomic) {
            vTaskSuspendAll();
        }
        gh7007_write_reg(hdr, s_linebuf, (uint32_t)npix * 2);
        if (s_atomic) {
            xTaskResumeAll();
        }
        s_video_row++;
    }
#else
    spi_transmit(GH_SPI_ID, s_linebuf, (uint32_t)npix * 2);
#endif
}

void gh7007_blit_end(void)
{
#if GH_PANEL == GH_PANEL_GH7007
    /* each row already closed its own CS-low window */
#else
    gh_wait_idle();
    gh_cs(1);
#endif
}

void gh7007_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                      uint16_t color)
{
    if (x1 < x0 || y1 < y0) {
        return;
    }
    uint16_t w = (uint16_t)(x1 - x0 + 1);
    uint16_t h = (uint16_t)(y1 - y0 + 1);
    if (w > GH_IN_WIDTH) {
        w = GH_IN_WIDTH;
        x1 = (uint16_t)(x0 + w - 1);
    }
    for (uint16_t i = 0; i < w; i++) {
        s_rowbuf[i] = color;
    }
    gh7007_blit_start(x0, y0, x1, y1);
    for (uint16_t r = 0; r < h; r++) {
        gh7007_blit_push_row(s_rowbuf, w);
        if ((r & 0x0F) == 0x0F) {
            vTaskDelay(1);           /* pace BETWEEN rows (CS high) only */
        }
    }
    gh7007_blit_end();
}

void gh7007_fill_screen(uint16_t color)
{
    gh7007_fill_rect(0, 0, (uint16_t)(s_width - 1), (uint16_t)(s_height - 1),
                     color);
}

/* Experiment: STANDARD DBI full-frame write — CASET/RASET window, then one 0x2C
 * burst with the whole frame in a SINGLE CS window (vs the vendor 2C/3C
 * per-row-window protocol). For a chip whose power-on/init state expects plain
 * DBI memory writes rather than the SPI4W-Video row framing. 4-wire only. */
void gh7007_fill_dbi(uint16_t color)
{
    uint16_t w = s_width, h = s_height;
    uint8_t ca[4] = { 0, 0, (uint8_t)((w - 1) >> 8), (uint8_t)(w - 1) };
    uint8_t ra[4] = { 0, 0, (uint8_t)((h - 1) >> 8), (uint8_t)(h - 1) };
    gh7007_write_reg(0x2A, ca, 4);               /* CASET */
    gh7007_write_reg(0x2B, ra, 4);               /* RASET */

    uint16_t px = gh_px(color);
    for (uint16_t i = 0; i < w; i++) {
        s_linebuf[2 * i]     = (uint8_t)(px >> 8);
        s_linebuf[2 * i + 1] = (uint8_t)(px & 0xFF);
    }
    uint8_t cmd = 0x2C;                          /* RAMWR, one giant window */
    gh_dc(1);                                    /* opening falling edge */
    gh_cs(0);
    gh_dc(0);
    gh_tx(&cmd, 1);
    gh_dc(1);
    for (uint16_t r = 0; r < h; r++) {
        gh_tx(s_linebuf, (uint32_t)w * 2);
        if ((r & 0x1F) == 0x1F) {
            vTaskDelay(1);
        }
    }
    gh_cs(1);
}

/* Verbatim replication of the vendor demo's DMA fill framing
 * (spi_dma_lcd_fill): CASET/RASET (per current write mode), then 0x2C in its
 * OWN CS window, then the WHOLE frame as one DC=1 CS window. */
void gh7007_fill_vendor(uint16_t color)
{
    uint16_t w = s_width, h = s_height;
    uint8_t ca[4] = { 0, 0, (uint8_t)((w - 1) >> 8), (uint8_t)(w - 1) };
    uint8_t ra[4] = { 0, 0, (uint8_t)((h - 1) >> 8), (uint8_t)(h - 1) };
    gh7007_write_reg(0x2A, ca, 4);
    gh7007_write_reg(0x2B, ra, 4);
    gh7007_write_reg(0x2C, NULL, 0);             /* RAMWR in its own window */

    uint16_t px = gh_px(color);
    for (uint16_t i = 0; i < w; i++) {
        s_linebuf[2 * i]     = (uint8_t)(px >> 8);
        s_linebuf[2 * i + 1] = (uint8_t)(px & 0xFF);
    }
    gh_cs(0);
    gh_dc(1);                                    /* one data window, whole frame */
    for (uint16_t r = 0; r < h; r++) {
        gh_tx(s_linebuf, (uint32_t)w * 2);
        if ((r & 0x1F) == 0x1F) {
            vTaskDelay(1);
        }
    }
    gh_cs(1);
}

/* Windowed partial-draw test (LVGL partial-refresh feasibility): CASET/RASET
 * window + RAMWR, pixels only for the window. If the box lands at the right
 * place, the chip honors DBI windowing and LVGL can use partial mode; if it
 * lands at the top-left / doesn't show, only full-frame streaming works. */
void gh7007_box_windowed(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                         uint16_t color)
{
    if (x1 >= s_width) x1 = (uint16_t)(s_width - 1);
    if (y1 >= s_height) y1 = (uint16_t)(s_height - 1);
    if (x1 < x0 || y1 < y0) return;
    uint16_t w = (uint16_t)(x1 - x0 + 1);
    uint8_t ca[4] = { (uint8_t)(x0 >> 8), (uint8_t)x0,
                      (uint8_t)(x1 >> 8), (uint8_t)x1 };
    uint8_t ra[4] = { (uint8_t)(y0 >> 8), (uint8_t)y0,
                      (uint8_t)(y1 >> 8), (uint8_t)y1 };
    gh7007_write_reg(0x2A, ca, 4);
    gh7007_write_reg(0x2B, ra, 4);

    uint16_t px = gh_px(color);
    for (uint16_t i = 0; i < w; i++) {
        s_linebuf[2 * i]     = (uint8_t)(px >> 8);
        s_linebuf[2 * i + 1] = (uint8_t)(px & 0xFF);
    }
    gh_dc(1);
    gh_cs(0);
    gh_dc(0);
    uint8_t cmd = 0x2C;
    gh_tx(&cmd, 1);
    gh_dc(1);
    for (uint16_t r = y0; r <= y1; r++) {
        gh_tx(s_linebuf, (uint32_t)w * 2);
    }
    gh_cs(1);
}

/* Stream the same colour frame `frames` times back-to-back. The GH7007 is a
 * video interface (host feeds frames via PSRAM); if a single frame doesn't latch,
 * continuous streaming will. Yields periodically so the shell/watchdog survive. */
void gh7007_stream_color(uint16_t color, int frames)
{
    for (int i = 0; i < frames; i++) {
        gh7007_fill_screen(color);
        if ((i & 0x7) == 0x7) {
            vTaskDelay(1);
        }
    }
}

/* ----------------------------------------------------------------------- */
/* Diagnostics                                                             */
/* ----------------------------------------------------------------------- */

/* Crosstalk discriminator: with CS held HIGH the chip must ignore SCL/SDA
 * entirely (DS p.46: "SPI_CLK clock pulse and SPI_MOSI data are no effect"
 * while CSB is high). So if pure toggle traffic on these wires — with NO
 * valid transaction possible — still knocks the display back to its power-on
 * stripes, the disturbance is wire-level (RST glitch via flywire crosstalk,
 * or supply droop), not protocol. which: bit0 = SCLK, bit1 = MOSI. */
void gh7007_aggressor(int which, int seconds)
{
    uint8_t pins[2];
    uint8_t n = 0;
    if (which & 1) {
        pins[n++] = GH_PIN_SCLK;
    }
    if (which & 2) {
        pins[n++] = GH_PIN_MOSI;
    }
    if (!n) {
        return;
    }
    gh_cs(1);
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pins, n);
    for (uint8_t i = 0; i < n; i++) {
        gpio_set_mode(pins[i], GPIO_OUTPUT_MODE);
    }
    for (int s = 0; s < seconds; s++) {
        for (int ms = 0; ms < 1000; ms++) {
            for (int k = 0; k < 200; k++) {
                for (uint8_t i = 0; i < n; i++) {
                    gpio_write(pins[i], 1);
                }
                for (uint8_t i = 0; i < n; i++) {
                    gpio_write(pins[i], 0);
                }
            }
            if ((ms & 0x0F) == 0x0F) {
                vTaskDelay(1);
            }
        }
    }
    gh7007_set_bus_3w(s_bus_3w);   /* restore bus pinmux (drive resets to 1) */
}

/* Software anti-crosstalk: pad drive strength. Victim (RST/CS/DC) stronger =
 * lower source impedance against coupled glitches; aggressor (SCLK/MOSI)
 * weaker = slower edges = less coupling into the adjacent RST flywire.
 * NOTE: any later re-mux (lcd aggr / lcd bus / paranoid) resets drive to the
 * SDK default of 1 — re-apply after those. */
void gh7007_set_drive(int victim, int aggressor)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .smtCtrl = 1,
    };
    cfg.gpioFun = GPIO_FUN_SPI;
    cfg.drive = (uint8_t)(aggressor & 3);
    cfg.gpioPin = GH_PIN_SCLK;
    GLB_GPIO_Init(&cfg);
    cfg.gpioPin = GH_PIN_MOSI;
    GLB_GPIO_Init(&cfg);

    cfg.gpioFun = GPIO_FUN_GPIO;
    cfg.drive = (uint8_t)(victim & 3);
    const uint8_t ctl[3] = { GH_PIN_RST, GH_PIN_CS, GH_PIN_DC };
    const uint8_t lvl[3] = { 1, 1, 1 };
    for (int i = 0; i < 3; i++) {
        cfg.gpioPin = ctl[i];
        GLB_GPIO_Init(&cfg);
        gpio_set_mode(ctl[i], GPIO_OUTPUT_MODE);
        gpio_write(ctl[i], lvl[i]);
    }
}

/* Release / re-drive the RST line. Released = input with weak pull-up: the
 * SoC stops actively driving G17, so if anything on our side was yanking RST
 * the symptom changes; if the display still reverts on its own, the module
 * resets itself internally (module-side reset circuit / OTP boot loop). */
void gh7007_rst_release(int release)
{
    if (release) {
        gpio_set_mode(s_pin_rst, GPIO_INPUT_PP_MODE);
    } else {
        gpio_set_mode(s_pin_rst, GPIO_OUTPUT_MODE);
        gpio_write(s_pin_rst, 1);
    }
}

/* Apply a runtime pin map: all five signals become bit-banged GPIOs so any
 * FPC-order hypothesis is testable without touching the glued wires. Pass
 * enable=0 to restore the compile-time map + HW SPI. */
void gh7007_set_map(int enable, uint8_t rst, uint8_t cs, uint8_t scl,
                    uint8_t sda, uint8_t dc)
{
    if (enable) {
        s_pin_rst = rst; s_pin_cs = cs; s_pin_scl = scl;
        s_pin_sda = sda; s_pin_dc = dc;
        s_bitbang = 1;
        uint8_t pins[5] = { rst, cs, scl, sda, dc };
        GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pins, 5);
        for (int i = 0; i < 5; i++) {
            gpio_set_mode(pins[i], GPIO_OUTPUT_MODE);
        }
        gpio_write(s_pin_rst, 1);
        gpio_write(s_pin_cs, 1);
        gpio_write(s_pin_scl, 0);        /* idle low per DS figures */
        gpio_write(s_pin_sda, 0);
        gpio_write(s_pin_dc, 1);
        s_bus_ready = true;              /* bit-bang bus needs no SPI init */
    } else {
        /* restore compile-time defaults (incl. the default bus engine) */
        s_pin_rst = GH_PIN_RST; s_pin_cs = GH_PIN_CS; s_pin_scl = GH_PIN_SCLK;
        s_pin_sda = GH_PIN_MOSI; s_pin_dc = GH_PIN_DC;
        s_bitbang = GH_BUS_BITBANG;
        if (s_bus_ready) {
            if (s_bitbang) {
                gh7007_set_map(1, s_pin_rst, s_pin_cs, s_pin_scl, s_pin_sda,
                               s_pin_dc);
            } else {
                gh_bus_reapply();
            }
        }
    }
}

int gh7007_get_map(uint8_t out[5])
{
    out[0] = s_pin_rst; out[1] = s_pin_cs; out[2] = s_pin_scl;
    out[3] = s_pin_sda; out[4] = s_pin_dc;
    return s_bitbang;
}

/* Bit-banged I2C probe on the same two wires (SCL = GH_PIN_SCLK, SDA =
 * GH_PIN_MOSI). Datasheet Table 6.1: CMD_SEL strap 0 (power-on default) makes
 * the command interface I2C — a module built with CMD_SEL low ignores ALL SPI
 * framing. An ACK on any address here is definitive proof of an I2C-strapped
 * module (ADDR[1:0] straps pick one of four slave addresses, DS §6.1).
 * Open-drain emulation: release = input w/ pull-up, drive-0 = output low. */
#define I2C_HALF_US  10   /* ~50 kHz — generous for weak internal pull-ups */

static void i2c_sda(int hi)
{
    if (hi) {
        gpio_set_mode(GH_PIN_MOSI, GPIO_INPUT_PP_MODE);
    } else {
        gpio_set_mode(GH_PIN_MOSI, GPIO_OUTPUT_MODE);
        gpio_write(GH_PIN_MOSI, 0);
    }
}
static void i2c_scl(int hi)
{
    if (hi) {
        gpio_set_mode(GH_PIN_SCLK, GPIO_INPUT_PP_MODE);
    } else {
        gpio_set_mode(GH_PIN_SCLK, GPIO_OUTPUT_MODE);
        gpio_write(GH_PIN_SCLK, 0);
    }
}

static int i2c_probe_addr(uint8_t addr7)
{
    int ack;
    /* START */
    i2c_sda(1); i2c_scl(1); arch_delay_us(I2C_HALF_US);
    i2c_sda(0); arch_delay_us(I2C_HALF_US);
    i2c_scl(0); arch_delay_us(I2C_HALF_US);
    /* address + W */
    uint8_t byte = (uint8_t)(addr7 << 1);
    for (int b = 7; b >= 0; b--) {
        i2c_sda((byte >> b) & 1); arch_delay_us(I2C_HALF_US);
        i2c_scl(1); arch_delay_us(I2C_HALF_US);
        i2c_scl(0); arch_delay_us(I2C_HALF_US);
    }
    /* ACK bit: release SDA, clock once, sample */
    i2c_sda(1); arch_delay_us(I2C_HALF_US);
    i2c_scl(1); arch_delay_us(I2C_HALF_US);
    ack = (gpio_read(GH_PIN_MOSI) == 0);
    i2c_scl(0); arch_delay_us(I2C_HALF_US);
    /* STOP */
    i2c_sda(0); arch_delay_us(I2C_HALF_US);
    i2c_scl(1); arch_delay_us(I2C_HALF_US);
    i2c_sda(1); arch_delay_us(I2C_HALF_US);
    return ack;
}

int gh7007_i2c_scan(uint8_t *found, int max_found)
{
    uint8_t pins[2] = { GH_PIN_MOSI, GH_PIN_SCLK };
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pins, 2);
    i2c_sda(1); i2c_scl(1); arch_delay_us(100);

    /* line-level self-check: with both released (input + weak pull-up), the
     * wires MUST read high or the whole scan is inconclusive (no effective
     * pull-up / line loaded) — a no-ACK result would be a false negative. */
    {
        int sda_lvl = gpio_read(GH_PIN_MOSI);
        int scl_lvl = gpio_read(GH_PIN_SCLK);
        IPRO_LOGI(LOG_TAG, "i2c idle levels: SDA=%d SCL=%d %s", sda_lvl, scl_lvl,
                  (sda_lvl && scl_lvl) ? "(ok, scan valid)"
                                       : "(NOT HIGH — scan INCONCLUSIVE, lines "
                                         "won't rise on weak pull-up)");
    }

    int n = 0;
    for (uint8_t a = 0x03; a <= 0x77; a++) {
        if (i2c_probe_addr(a) && n < max_found) {
            found[n++] = a;
        }
        arch_delay_us(50);
    }
    gh7007_set_bus_3w(s_bus_3w);   /* restore current bus pinmux */
    return n;
}

void gh7007_pin_out(uint8_t pin, int level)
{
    uint8_t list[1] = { pin };
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, list, 1);
    gpio_set_mode(pin, GPIO_OUTPUT_MODE);
    gpio_write(pin, level ? 1 : 0);
}

int gh7007_pin_in(uint8_t pin)
{
    uint8_t list[1] = { pin };
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, list, 1);
    gpio_set_mode(pin, GPIO_INPUT_PP_MODE);
    return gpio_read(pin);
}
