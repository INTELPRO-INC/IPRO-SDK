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

/*
 * E Ink Spectra 6 — EL036MF1 / ED2208-CFA
 * 3.6" 400x600, six colours (black white yellow red blue green)
 *
 * Ported from E Ink's own MSP430 reference (EL036.c, (C) 2023 E Ink Holdings)
 * so the register sequence is theirs verbatim — including the undocumented
 * 0xAA unlock that appears nowhere in the Application Note, and the second
 * BTST2 write that happens mid-refresh rather than during init. Both are
 * required; neither is guessable from the datasheet.
 *
 * ===========================================================================
 *  EVERYTHING YOU NEED TO EDIT IS IN THIS BLOCK
 * ===========================================================================
 */

#ifndef EPD_EL036_H
#define EPD_EL036_H

#include <stdint.h>
#include <stdbool.h>

/* --- 1. GPIO assignment ------------------------------------------------
 * Change these to match the board. All six are plain GPIO — the driver
 * bit-bangs the bus rather than using the SPI peripheral, so that a wrong
 * SPI clock/mode setting cannot be one of the things being debugged.
 * Once the panel is proven alive, hardware SPI can be swapped in for speed.
 */
#define EPD_PIN_CSB     17      /* Chip select   -> panel pin 31 (CSB)      */
#define EPD_PIN_SDA     16      /* Serial data   -> panel pin 33 (SI0)      */
#define EPD_PIN_DC      12      /* Data/command  -> panel pin 30 (DC)       */
#define EPD_PIN_SCL     15      /* Serial clock  -> panel pin 32 (SCL)      */
#define EPD_PIN_RSTN    11      /* Reset, active low -> panel pin 28 (RST_N)*/
#define EPD_PIN_BUSY    10      /* Busy state in -> panel pin 29 (BUSY_N)   */
/* SDA on GPIO16 is 16 % 4 == 0, which by the mod-4 rule below is the MISO
 * position -- yet the board works with pin_swap_enable OFF, not on. The rule
 * evidently does not tell the whole story here. Settable with `epd_swap`. */

/* Board power enable. Not part of the panel's own 6-signal interface -- this
 * is a switch on the board that gates the boost regulator supplying
 * VGH/VGL/VSH/VSL, the same role as the reference firmware's "SWC" (switch
 * control) pin (pindefine.h: SWC_L/SWC_H). The reference brings it up FIRST,
 * before the first reset, and only takes it down once at the very end of the
 * whole program. Follow that order here too: gpio_setup() drives it high last
 * (after the other five, so it does not glitch), epd_power_enable(false) is
 * a separate call the caller makes on their own schedule. */
#define EPD_PIN_PWR     14

/* SCL and SDA are not freely placeable once hardware SPI is in play. The SoC
 * derives the SPI signal role from the pin number modulo 4, independently of
 * which SPI instance the alternate function selects:
 *
 *     n % 4 == 0 -> MISO      n % 4 == 2 -> SS
 *     n % 4 == 1 -> MOSI      n % 4 == 3 -> SCLK
 *
 * (See bsp/board/ipro7/ipro7_dual_imu_board_config.h: GPIO37 AF=4 SPI0_MOSI,
 * GPIO39 AF=4 SPI0_SCLK, GPIO31 AF=26 SPI1_SCLK, GPIO33 AF=26 SPI1_MOSI.)
 *
 * SCL sits on GPIO15 (15 % 4 == 3, SCLK), which the rule predicts correctly.
 * SDA sits on GPIO16 (16 % 4 == 0, the MISO position), which the rule says
 * should need pin_swap_enable -- but the board only works with the swap OFF.
 * Treat the rule as a starting guess for SCL and verify SDA on hardware; the
 * `epd_swap` command exists for exactly that. CSB, DC,
 * RST_N, BUSY_N and PWR have no such constraint -- they are
 * plain GPIO under every engine (bit-bang, hardware SPI, and PIO's SET group,
 * which accepts any single pin). CSB in particular is always driven by
 * software regardless of engine, because the panel wants it toggled once per
 * byte, which no hardware chip-select will do.
 */

/* --- 2. Bus mode -------------------------------------------------------
 * The panel's BS0/BS1 straps decide this, and getting it wrong is the most
 * likely reason a correctly wired panel stays blank: in the wrong mode every
 * byte is misframed and no command is ever recognised.
 *
 *   Application Note Table 5:
 *     3-wire SPI (9-bit) : BS1 = Low, BS0 = High   <-- panel DEFAULT
 *     4-wire SPI         : BS1 = Low, BS0 = Low
 *
 * E Ink's own reference firmware runs 3-wire. If the board wired a separate
 * DC line for 4-wire, BS0 must be actively pulled LOW — floating is not
 * enough, since High is the default.
 *
 * This is a runtime setting (epd_set_bus_mode / `epd_bus` shell command) so
 * both can be tried on one build without reflashing.
 */
typedef enum {
    EPD_BUS_3WIRE = 0,  /* 9-bit: D/C travels as the first bit; DC pin unused */
    EPD_BUS_4WIRE = 1,  /* 8-bit: D/C is a separate line                      */
} epd_bus_mode_t;

/* This board straps BS0 and BS1 both to ground, which Table 5 defines as
 * 4-wire -- so the panel is NOT in its 3-wire default here, and the schematic's
 * separate DC line agrees. Confirmed from the schematic, not assumed. */
#define EPD_BUS_MODE_DEFAULT   EPD_BUS_4WIRE

/* --- 3. Bit-bang half-period -------------------------------------------
 * Loop iterations per half clock. Raise it if the panel is on long or
 * unterminated wires; E Ink notes 5 MHz is realistic on a proper PCB and
 * that trace attenuation, not the controller, sets the ceiling.
 */
#define EPD_SPI_DELAY_LOOPS    4

/*
 * Hardware SPI. 4-wire only: the controller can emit 8/16/24/32-bit frames but
 * not the 9 bits a 3-wire transfer needs, and the panel requires CSB to toggle
 * per byte (E Ink's own SPI_DATA() does), so the 9-bit stream cannot be packed
 * into 8-bit frames either. 3-wire therefore stays bit-banged.
 *
 * 4 MHz is inside the panel's limits -- Application Note Table 3 allows 20 MHz
 * for writes, 6.67 MHz for reads, and E Ink recommends 5 MHz in practice. It is
 * also below the 8 MHz above which SPI0's TX DMA handshake is unreliable on
 * this silicon. Note the clock divider truncates, so the rate actually produced
 * can be lower than requested; measure it rather than assume.
 */
/*
 * PIO engine. Unlike the SPI controller this handles 3-wire natively: nine-bit
 * frames and per-byte CSB are both in the state machine's program, and the FIFO
 * is DMA-fed (in principle -- DMA has not worked on this silicon so far; see
 * epd_pio.c), so a full frame costs no per-byte CPU time. It is the only engine
 * that is both fast and faithful to what the panel actually wants.
 *
 * Compiled in but NOT the default: bit-bang stays the default because it is a
 * line-by-line port of E Ink's reference firmware and is therefore the path to
 * trust while the panel itself is still unproven. Select this one at runtime
 * with 'epd_engine pio'.
 *
 * 8 MHz chosen after a runtime sweep with no panel attached (epd_pio_hz),
 * measured on a logic analyser:
 *
 *     4 MHz   clean, first value verified working (baseline)
 *     8 MHz   clean -- picked as the default
 *    12 MHz   glitches every time, reproducibly
 *    16 MHz   clean
 *
 * The 12 MHz-only failure with clean neighbours on both sides is not what a
 * firmware timing bug looks like (that would degrade monotonically with
 * frequency, worst at the top). The clkdiv itself is an exact integer at every
 * frequency tried (96 MHz / 4 = 24 MHz instruction rate for 12 MHz, no
 * fractional remainder), so the state machine's own timing should be jitter-free
 * regardless. The likely cause is standing-wave reflection on the open,
 * unterminated flying leads used for this bring-up (no panel attached, so
 * every wire is an open stub) hitting a bad node at that specific wavelength --
 * an FPC-mounted panel with short, controlled-impedance traces would not
 * necessarily reproduce this. Re-sweep once a real panel is connected before
 * trusting any value here at face value; this constant is a bring-up default,
 * not a validated ceiling. The program's own zero-margin limit (data
 * setup/hold >=30 ns, Application Note Table 3 tSDS/tSDH) is ~16.7 MHz --
 * see epd_set_pio_hz() in epd_el036.c.
 */
#define EPD_PIO_ENABLE         1
#define EPD_PIO_HZ             8000000

#define EPD_HW_SPI_ENABLE      1
#define EPD_HW_SPI_ID          0
#define EPD_HW_SPI_HZ          4000000

/* ===========================================================================
 *  Panel constants — do not change
 * =========================================================================== */

#define EPD_WIDTH        400
#define EPD_HEIGHT       600
/* Four bits per pixel, two pixels per byte: 400*600/2 */
#define EPD_FRAME_BYTES  120000UL

/* Colour codes, Application Note Table 8. Each nibble is one pixel, so a
 * solid fill byte is the code repeated: white = 0x11, red = 0x33. */
typedef enum {
    EPD_BLACK  = 0x0,
    EPD_WHITE  = 0x1,
    EPD_YELLOW = 0x2,
    EPD_RED    = 0x3,
    EPD_BLUE   = 0x5,
    EPD_GREEN  = 0x6,
} epd_colour_t;

/* ===========================================================================
 *  API
 * =========================================================================== */

/* Bench mode -- see epd_el036.c. Lets the sequences run with no panel
 * attached so the SPI waveform can be captured. */
/* Hardware SPI, 4-wire only; silently ignored in 3-wire mode. */
/* Stream frame data with one CSB assertion per chunk instead of per byte.
 * 4-wire + hardware SPI only. OFF by default -- unverified against a panel. */
typedef enum {
    EPD_ENGINE_BITBANG = 0,
    EPD_ENGINE_PIO,
} epd_engine_t;

/* Pin assignment is settable at runtime (epd_set_pin / the `epd_pins` shell
 * command) so a wrong guess costs a command instead of a rebuild. The
 * EPD_PIN_* defines above are only the power-on defaults. */
typedef enum {
    EPD_PIN_IDX_CSB = 0,
    EPD_PIN_IDX_SDA,
    EPD_PIN_IDX_DC,
    EPD_PIN_IDX_SCL,
    EPD_PIN_IDX_RSTN,
    EPD_PIN_IDX_BUSY,
    EPD_PIN_IDX_PWR,
    EPD_PIN_COUNT,
} epd_pin_idx_t;

/* Takes effect on the next command (each one re-runs gpio_setup()). Releases
 * any engine currently holding the pads. */
void    epd_set_pin(epd_pin_idx_t which, uint8_t gpio);
uint8_t epd_get_pin(epd_pin_idx_t which);
const char *epd_pin_name(epd_pin_idx_t which);

void epd_set_engine(epd_engine_t e);
epd_engine_t epd_get_engine(void);

/* Change the PIO SCL target without a rebuild. Takes effect on the next
 * transfer. See epd_el036.h EPD_PIO_HZ comment for the panel's own limits;
 * this does not re-derive or check them -- the caller is on their own. */
void epd_set_pio_hz(uint32_t hz);
uint32_t epd_get_pio_hz(void);

void epd_set_burst(bool on);
bool epd_get_burst(void);

void epd_set_hw_spi(bool on);
bool epd_get_hw_spi(void);

/* MOSI/MISO swap for the hardware SPI engine. -1 = derive from where SDA sits
 * (n%4==0 needs the swap), 0/1 = force. The derivation rule is inferred from
 * the SDK's board configs, not documented, so it stays overridable. */
/* Hardware SPI SCL rate, settable at runtime. Takes effect on the next
 * transfer. Panel limits: 20 MHz writes, 6.67 MHz reads (Table 3). */
void     epd_set_hw_spi_hz(uint32_t hz);
uint32_t epd_get_hw_spi_hz(void);

void    epd_set_hw_swap(int mode);
int     epd_get_hw_swap(void);
uint8_t epd_hw_swap_effective(void);

void epd_set_ignore_busy(bool on);
bool epd_get_ignore_busy(void);

void epd_set_bus_mode(epd_bus_mode_t mode);
epd_bus_mode_t epd_get_bus_mode(void);

/*
 * Read the revision register (REV, 0x70).
 *
 * Run this FIRST. It is a pure read-back: it needs no image, no waveform and
 * no power rails beyond logic, yet it only succeeds if wiring, bus mode,
 * reset timing and BUSY all work. A plausible value means the interface is
 * sound and any remaining fault is downstream; 0x00 or 0xFF means the
 * interface itself is wrong and displaying anything is hopeless.
 *
 * @param out_rev  receives the third byte returned by the panel
 * @return 0 on success, negative if BUSY never released
 */
int epd_read_rev(uint8_t *out_rev);

/* Reset + full register init. Safe to call repeatedly. */
int epd_init(void);

/* Fill the whole panel with one colour and refresh. Blocks for the whole
 * update — expect on the order of 10-30 s, which is normal for Spectra 6. */
int epd_fill(epd_colour_t colour);

/* Six horizontal colour bands in one refresh -- all six colours side by side
 * instead of epd_cycle's six separate updates. */
int epd_bars(void);

/*
 * Push a caller-supplied frame and refresh.
 * @param frame  EPD_FRAME_BYTES bytes, two pixels per byte, high nibble first
 */
int epd_draw(const uint8_t *frame);

/* Deep sleep (DSLP 0x07 + 0xA5). Only a hardware reset wakes the panel. */
void epd_sleep(void);

/* Board power switch (VGH/VGL/VSH/VSL boost enable, "PWR"/"SWC"). gpio_setup()
 * already turns this on before every command; call with false to cut power. */
void epd_power_enable(bool on);

/* How long to wait for the panel to finish a refresh (DRF). Default 60 s.
 * A Spectra 6 colour update is tens of seconds and slows down considerably
 * when cold, so raising this is how to tell a slow panel from a stuck one. */
void     epd_set_refresh_timeout(uint32_t ms);
uint32_t epd_get_refresh_timeout(void);

/* PWR polarity. Active-high is only an assumption carried over from E Ink's
 * reference (SWC_H switches its board on); a board with an active-low boost
 * enable would be held off permanently by that default. */
void epd_set_pwr_active_high(bool high);
bool epd_get_pwr_active_high(void);

#endif /* EPD_EL036_H */
