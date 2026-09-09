/*
 * E Ink Spectra 6 EL036MF1 driver for IPRO7.
 *
 * Register values and ordering are transcribed from E Ink's MSP430 reference
 * (EL036.c, (C) 2023 E Ink Holdings) rather than derived, because two of the
 * required steps cannot be found in the published documentation:
 *
 *   - command 0xAA with six parameters, ahead of everything else
 *   - a SECOND BTST2 write during the refresh, with different values from
 *     the one in init
 *
 * Where this file departs from the reference it is only to replace MSP430
 * register pokes with IPRO7 GPIO calls, and busy-waits with vTaskDelay.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "epd_el036.h"
#include "epd_pio.h"

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>

#include "hal_gpio.h"
#include "hal_spi.h"
#include "drv_spi.h"
#include "drv_glb.h"
#include "ipro_log.h"

#define TAG "EPD036"

/* --- Command set (EL036.h from the reference) --------------------------- */
#define CMD_PSR     0x00
#define CMD_PWR     0x01
#define CMD_POF     0x02
#define CMD_POFS    0x03
#define CMD_PON     0x04
#define CMD_BTST1   0x05
#define CMD_BTST2   0x06
#define CMD_DSLP    0x07
#define CMD_BTST3   0x08
#define CMD_DTM     0x10
#define CMD_DRF     0x12
#define CMD_PLL     0x30
#define CMD_CDI     0x50
#define CMD_TCON    0x60
#define CMD_TRES    0x61
#define CMD_REV     0x70
#define CMD_VDCS    0x82
#define CMD_T_VDCS  0x84
#define CMD_PWS     0xE3

/* --- Pin assignment -----------------------------------------------------
 * Runtime rather than compile-time, so a wrong guess costs a shell command
 * instead of a rebuild + reflash. The EPD_PIN_* defines in the header are the
 * power-on defaults; epd_set_pin() overrides them. */
static uint8_t s_pin[EPD_PIN_COUNT] = {
    [EPD_PIN_IDX_CSB]  = EPD_PIN_CSB,
    [EPD_PIN_IDX_SDA]  = EPD_PIN_SDA,
    [EPD_PIN_IDX_DC]   = EPD_PIN_DC,
    [EPD_PIN_IDX_SCL]  = EPD_PIN_SCL,
    [EPD_PIN_IDX_RSTN] = EPD_PIN_RSTN,
    [EPD_PIN_IDX_BUSY] = EPD_PIN_BUSY,
    [EPD_PIN_IDX_PWR]  = EPD_PIN_PWR,
};

static const char *const k_pin_name[EPD_PIN_COUNT] = {
    [EPD_PIN_IDX_CSB]  = "csb",
    [EPD_PIN_IDX_SDA]  = "sda",
    [EPD_PIN_IDX_DC]   = "dc",
    [EPD_PIN_IDX_SCL]  = "scl",
    [EPD_PIN_IDX_RSTN] = "rst",
    [EPD_PIN_IDX_BUSY] = "busy",
    [EPD_PIN_IDX_PWR]  = "pwr",
};

const char *epd_pin_name(epd_pin_idx_t which)
{
    return (which < EPD_PIN_COUNT) ? k_pin_name[which] : "?";
}

uint8_t epd_get_pin(epd_pin_idx_t which)
{
    return (which < EPD_PIN_COUNT) ? s_pin[which] : 0xFF;
}

#define PIN_CSB   s_pin[EPD_PIN_IDX_CSB]
#define PIN_SDA   s_pin[EPD_PIN_IDX_SDA]
#define PIN_DC    s_pin[EPD_PIN_IDX_DC]
#define PIN_SCL   s_pin[EPD_PIN_IDX_SCL]
#define PIN_RSTN  s_pin[EPD_PIN_IDX_RSTN]
#define PIN_BUSY  s_pin[EPD_PIN_IDX_BUSY]
#define PIN_PWR   s_pin[EPD_PIN_IDX_PWR]

/* --- Pin helpers -------------------------------------------------------- */
#define CSB_L()   gpio_write(PIN_CSB, 0)
#define CSB_H()   gpio_write(PIN_CSB, 1)
#define SCL_L()   gpio_write(PIN_SCL, 0)
#define SCL_H()   gpio_write(PIN_SCL, 1)
#define SDA_L()   gpio_write(PIN_SDA, 0)
#define SDA_H()   gpio_write(PIN_SDA, 1)
#define DC_L()    gpio_write(PIN_DC,  0)
#define DC_H()    gpio_write(PIN_DC,  1)
#define RSTN_L()  gpio_write(PIN_RSTN, 0)
#define RSTN_H()  gpio_write(PIN_RSTN, 1)
#define BUSY()    gpio_read(PIN_BUSY)
/* PWR polarity is an assumption, not a documented fact: E Ink's reference uses
 * SWC_H to switch its board's supply on, so active-high is the default here.
 * A board that gates the boost converter with an active-low enable would be
 * held OFF for the entire session by that assumption -- no VGH/VGL/VSH/VSL, so
 * the panel never answers anything, which is indistinguishable from a wiring
 * or protocol fault. Settable at runtime so it can be ruled out in one command
 * rather than one rebuild. */
static bool s_pwr_active_high = true;

#define PWR_ON()   gpio_write(PIN_PWR, s_pwr_active_high ? 1 : 0)
#define PWR_OFF()  gpio_write(PIN_PWR, s_pwr_active_high ? 0 : 1)

void epd_set_pwr_active_high(bool high) { s_pwr_active_high = high; }
bool epd_get_pwr_active_high(void)      { return s_pwr_active_high; }

static epd_bus_mode_t s_bus = EPD_BUS_MODE_DEFAULT;

void epd_set_bus_mode(epd_bus_mode_t mode) { s_bus = mode; }
epd_bus_mode_t epd_get_bus_mode(void)      { return s_bus; }

static inline void bit_delay(void)
{
    for (volatile int i = 0; i < EPD_SPI_DELAY_LOOPS; i++) {
        __asm__ volatile("nop");
    }
}

/* --- Bus primitives ----------------------------------------------------- */

/* --- Hardware SPI -------------------------------------------------------
 * 4-wire only. CSB stays a software-driven GPIO (ignore_cs=1): the panel wants
 * it framed per byte, which the controller's own chip-select will not do.
 * DC is likewise a plain level held across the transfer.
 */
#if EPD_HW_SPI_ENABLE
static bool s_hw_spi = true;
static bool s_hw_ready;

/*
 * MOSI/MISO swap. 0/1 = forced, -1 = derive from the pin number.
 *
 * Default OFF, on measurement rather than theory. The derivation rule -- the
 * SoC fixing each pin's SPI role by its number mod 4, n%4==1 MOSI and n%4==0
 * MISO, with the swap exchanging those two -- is inferred from the SDK's board
 * config files, not documented. By that rule SDA on GPIO16 (16%4==0) would
 * need the swap ON, but the board only works with it OFF. So either the rule
 * does not hold for this pin group, or pin_swap_enable does not mean what the
 * field name suggests; either way the hardware wins.
 *
 * Kept overridable (epd_swap) because the correct value is a property of the
 * board, and getting it wrong is invisible on the console -- the clock still
 * runs, the data just leaves on a pin nothing is connected to.
 */
static int s_hw_swap = 0;

static void hw_spi_release(void);

/* Requested SCL rate. Runtime-settable so it can be swept against the panel's
 * own limits (Table 3: 20 MHz writes, 6.67 MHz reads) without a rebuild.
 * hal_spi.c derives its divider from a 96 MHz source and truncates, so the
 * rate actually produced can be lower than asked for -- measure, do not
 * assume. Note SPI0's TX DMA handshake is unreliable above 8 MHz on this
 * silicon; this driver polls rather than using DMA, so that limit should not
 * apply here, but it has not been verified above 8 MHz. */
static uint32_t s_hw_hz = EPD_HW_SPI_HZ;

void epd_set_hw_spi_hz(uint32_t hz)
{
    if (hz == 0u) {
        return;
    }
    s_hw_hz = hz;
    if (s_hw_ready) {
        hw_spi_release();   /* re-init on the next transfer */
    }
}

uint32_t epd_get_hw_spi_hz(void) { return s_hw_hz; }

static uint8_t hw_spi_swap(void)
{
    if (s_hw_swap >= 0) {
        return (uint8_t)s_hw_swap;
    }
    return (PIN_SDA % 4 == 0) ? 1 : 0;   /* 'auto': the inferred rule */
}

void epd_set_hw_swap(int mode)
{
    s_hw_swap = (mode < 0) ? -1 : (mode ? 1 : 0);
    if (s_hw_ready) {
        hw_spi_release();   /* re-init on the next transfer so it takes effect */
    }
}

int epd_get_hw_swap(void)     { return s_hw_swap; }
uint8_t epd_hw_swap_effective(void) { return hw_spi_swap(); }

static void hw_spi_init(void)
{
    /* Ungate the SPI block before touching it. Nothing else in the SDK calls
     * GLB_Set_SPI_CLK() -- hal_spi.c's spi_init() only sets the master/slave
     * mode bit -- so if the block comes out of reset gated, every transfer
     * sits in SPI_SendData() forever: hal_spi.c passes SPI_TIMEOUT_DISABLE, so
     * there is nothing to time out. The symptom is the whole command producing
     * no output at all, which is what an unclocked PIO block did too.
     *
     * BCLK with div = 0 is the same 96 MHz source hal_spi.c assumes for its
     * own divider maths (SPI_MODULE_FREQ). */
    GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_BCLK, 0);

    uint8_t pins[] = { PIN_SCL, PIN_SDA };
    GLB_GPIO_Func_Init(GPIO_FUN_SPI, pins, sizeof(pins));

    spi_device_t dev = {
        .id             = EPD_HW_SPI_ID,
        .clk            = s_hw_hz,
        .mode           = SPI_MASTER_MODE,
        .direction      = SPI_MSB_BYTE0_DIRECTION_FIRST,
        .ignore_cs      = 1,          /* CSB driven by software, per byte */
        .clk_polaraity  = 0,          /* Mode 0: idle low, sample leading */
        .clk_phase      = 0,
        .datasize       = SPI_FRAME_SIZE_8,
        .fifo_threshold = 1,
        .pin_swap_enable = hw_spi_swap(),
        .caps           = 0,          /* polled; no DMA, no interrupt handler */
    };

    /* Order matters. spi_init() configures the hardware from the global
     * spi_devices[] table, and spi_setConfig() is what writes into that table
     * -- calling init first applies the board defaults (18 MHz, CS driven by
     * the controller, MOSI/MISO swapped) and everything set here is silently
     * ignored. */
    spi_setConfig(&dev);
    spi_init(EPD_HW_SPI_ID);

    /* spi_init() ends with the block DISABLED -- it opens with
     * SPI_Disable() and never pairs it, leaving enabling to the separate
     * spi_enable() call. Without this the FIFO accepts writes but nothing is
     * ever shifted out, so SPI_SendData() sits in its drain loop until it
     * times out, once per byte. */
    spi_enable(EPD_HW_SPI_ID, 1);

    s_hw_ready = true;
}

/* Bounded send. hal_spi.c's spi_transmit() calls SPI_SendData() with
 * SPI_TIMEOUT_DISABLE, so a misconfigured or unclocked block spins forever and
 * the command produces no output at all -- indistinguishable from a crash, and
 * exactly what was seen when the SPI clock gate was missing. Ask the driver for
 * the timeout instead and report, so a stall names itself. */
static void hw_spi_send(void *buf, uint32_t len)
{
    if (SPI_SendData(EPD_HW_SPI_ID, buf, len, SPI_TIMEOUT_ENABLE) != 0) {
        IPRO_LOGE(TAG, "hardware SPI send timed out (%lu bytes) — "
                       "check the SPI clock gate and pin mux",
                  (unsigned long)len);
    }
}

/* Hand SCL/SDA back to the bit-banger. */
static void hw_spi_release(void)
{
    spi_enable(EPD_HW_SPI_ID, 0);   /* stop driving before handing the pads back */

    uint8_t pins[] = { PIN_SCL, PIN_SDA };
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pins, sizeof(pins));
    gpio_set_mode(PIN_SCL, GPIO_OUTPUT_MODE);
    gpio_set_mode(PIN_SDA, GPIO_OUTPUT_MODE);
    SCL_L();
    SDA_H();
    s_hw_ready = false;
}

static bool hw_spi_active(void)
{
    return s_hw_spi && s_bus == EPD_BUS_4WIRE;
}

void epd_set_hw_spi(bool on)
{
    if (on == s_hw_spi) {
        return;
    }
    s_hw_spi = on;
    if (!on && s_hw_ready) {
        hw_spi_release();
    }
}

bool epd_get_hw_spi(void) { return s_hw_spi; }
#else
static bool hw_spi_active(void) { return false; }
void epd_set_hw_spi(bool on)   { (void)on; }
bool epd_get_hw_spi(void)      { return false; }
void epd_set_hw_swap(int mode) { (void)mode; }
int  epd_get_hw_swap(void)     { return -1; }
void epd_set_hw_spi_hz(uint32_t hz) { (void)hz; }
uint32_t epd_get_hw_spi_hz(void)    { return 0; }
uint8_t epd_hw_swap_effective(void) { return 0; }
#endif

/* --- Engine selection ---------------------------------------------------
 * PIO where available, bit-bang otherwise. The PIO owns SCL/SDA/CSB while it
 * is running, so switching engines has to hand the pads back explicitly --
 * a pad under PIO control ignores gpio_write() entirely, which shows up as a
 * clock that never moves rather than as an error.
 */
#if EPD_PIO_ENABLE
/* Bit-bang is the default despite being the slowest. This package exists to
 * answer "does the panel light up at all", and the bit-bang path is a
 * line-by-line port of E Ink's own firmware -- so when something does not work
 * it is the one path where the driver can be ruled out. Switch to PIO with
 * 'epd_engine pio' once the panel is known good. */
static epd_engine_t s_engine = EPD_ENGINE_BITBANG;
static uint32_t s_pio_hz = EPD_PIO_HZ;

static bool pio_active(void)
{
    return s_engine == EPD_ENGINE_PIO;
}

static void pio_arm(void)
{
    if (!epd_pio_ready()) {
        /* Frame width follows the bus mode: 9 bits in 3-wire (D/C rides in
         * the frame), 8 in 4-wire (D/C is its own line, driven by the caller
         * exactly as for hardware SPI). */
        epd_pio_init(PIN_SCL, PIN_SDA, PIN_CSB, s_pio_hz,
                     (s_bus == EPD_BUS_3WIRE) ? 9 : 8);
    }
}

void epd_set_engine(epd_engine_t e)
{
    if (e == s_engine) {
        return;
    }
    if (s_engine == EPD_ENGINE_PIO) {
        epd_pio_deinit();
    }
    s_engine = e;
}

/* Runtime clock sweep -- avoids a rebuild + reflash per frequency tried.
 * Takes effect on the next transfer; a transfer already armed keeps running at
 * its old rate until epd_pio_deinit() (bus-mode switch, gpio_setup(), or a
 * bit-bang read) forces the next one to re-init. */
void epd_set_pio_hz(uint32_t hz)
{
    if (hz == 0u) {
        return;
    }
    s_pio_hz = hz;
    if (epd_pio_ready()) {
        epd_pio_deinit();
    }
}

uint32_t epd_get_pio_hz(void) { return s_pio_hz; }
#else
static epd_engine_t s_engine = EPD_ENGINE_BITBANG;
static bool pio_active(void) { return false; }
static void pio_arm(void)    { }
void epd_set_engine(epd_engine_t e) { (void)e; }
void epd_set_pio_hz(uint32_t hz)    { (void)hz; }
uint32_t epd_get_pio_hz(void)       { return 0; }
#endif

epd_engine_t epd_get_engine(void) { return s_engine; }

void epd_set_pin(epd_pin_idx_t which, uint8_t gpio)
{
    if (which >= EPD_PIN_COUNT) {
        return;
    }

    /* Whichever engine currently owns SCL/SDA has them muxed to a peripheral
     * function on the OLD pin numbers. Drop those claims before the numbers
     * change, or the old pads stay muxed away and the new ones never get
     * configured -- which looks like the new pins simply not working. */
#if EPD_HW_SPI_ENABLE
    if (s_hw_ready) {
        hw_spi_release();
    }
#endif
#if EPD_PIO_ENABLE
    if (epd_pio_ready()) {
        epd_pio_deinit();
    }
#endif

    s_pin[which] = gpio;
}

static void spi_write(uint8_t val, bool is_data)
{
#if EPD_PIO_ENABLE
    /* PIO carries the D/C flag in the frame itself, so this covers 3-wire
     * without the bit-bang path's leading-bit dance. */
    if (pio_active()) {
        pio_arm();
        if (s_bus == EPD_BUS_4WIRE) {
            /* 8-bit frames carry no D/C bit, so the line has to be at the
             * right level before the transfer starts, and stay there. */
            if (is_data) { DC_H(); } else { DC_L(); }
        }
        epd_pio_write_byte(val, is_data);
        epd_pio_drain();
        return;
    }
#endif
#if EPD_HW_SPI_ENABLE
    if (hw_spi_active()) {
        if (!s_hw_ready) {
            hw_spi_init();
        }
        if (is_data) { DC_H(); } else { DC_L(); }
        CSB_L();
        hw_spi_send(&val, 1);
        CSB_H();
        return;
    }
#endif

    CSB_L();
    bit_delay();

    if (s_bus == EPD_BUS_3WIRE) {
        /* 9-bit frame: the D/C flag is clocked out ahead of the byte. */
        if (is_data) { SDA_H(); } else { SDA_L(); }
        bit_delay();
        SCL_H();
        bit_delay();
        SCL_L();
        bit_delay();
    } else {
        /* 8-bit frame: D/C is a level held across the transfer. */
        if (is_data) { DC_H(); } else { DC_L(); }
        bit_delay();
    }

    for (int i = 0; i < 8; i++) {
        if (val & 0x80) { SDA_H(); } else { SDA_L(); }
        bit_delay();
        SCL_H();
        bit_delay();
        SCL_L();
        val <<= 1;
    }

    SDA_L();
    bit_delay();
    CSB_H();
    bit_delay();
}

static inline void spi_cmd(uint8_t c)  { spi_write(c, false); }
static inline void spi_data(uint8_t d) { spi_write(d, true);  }

/*
 * Read one byte back.
 *
 * Framed exactly like the reference: every read byte is its own CSB cycle
 * and begins with a D/C=1 bit, the same as a data write, before SDA is
 * turned around. Dropping either the CSB framing or that leading bit makes
 * the panel return nothing, which is indistinguishable from a wiring fault.
 *
 * Note the Application Note (p.5) states CSB must stay low for the whole read
 * transaction, which contradicts the per-byte toggling E Ink's own firmware
 * does. The shipped firmware wins here; if a read ever misbehaves this is the
 * first thing worth trying the other way round.
 */
static uint8_t spi_read_byte(void)
{
    uint8_t v = 0;

    /* Reads are always bit-banged: SI0 has to be turned around mid-transfer,
     * which neither the SPI controller nor the PIO program can do on a single
     * wire. Whichever engine holds the pads has to let go first -- while a pad
     * is muxed away, gpio_write() on it does nothing at all, so the clock never
     * moves and the read silently returns garbage rather than failing. The next
     * write re-arms whichever engine is selected. */
#if EPD_HW_SPI_ENABLE
    if (s_hw_ready) {
        hw_spi_release();
    }
#endif
#if EPD_PIO_ENABLE
    if (epd_pio_ready()) {
        epd_pio_deinit();
        /* deinit hands the pads back as plain GPIO outputs; restore idle levels
         * before framing a read. */
        SCL_L();
        SDA_H();
        CSB_H();
    }
#endif

    CSB_L();
    bit_delay();

    if (s_bus == EPD_BUS_3WIRE) {
        SDA_H();                    /* D/C = 1, as for a data byte */
        bit_delay();
        SCL_H();
        bit_delay();
        SCL_L();
        bit_delay();
    } else {
        DC_H();
        bit_delay();
    }

    gpio_set_mode(PIN_SDA, GPIO_INPUT_MODE);
    bit_delay();
    bit_delay();

    for (int i = 0; i < 8; i++) {
        v <<= 1;
        SCL_H();
        bit_delay();
        if (gpio_read(PIN_SDA)) {
            v |= 0x01;
        }
        SCL_L();
        bit_delay();
    }

    gpio_set_mode(PIN_SDA, GPIO_OUTPUT_MODE);
    SDA_L();
    bit_delay();
    CSB_H();
    bit_delay();

    return v;
}

/* --- Busy handling ------------------------------------------------------
 * BUSY_N is low while the controller is working. The reference always waits
 * 2 ms before sampling, because the pin does not fall immediately after a
 * command is accepted and polling too early reads the previous idle state.
 * The timeout is ours: the reference spins forever, which during bring-up
 * hides a dead panel as a hang instead of reporting it.
 */
/* Bench mode: carry on as if BUSY_N had gone high. With no panel attached the
 * pin floats low, so every sequence aborts before a single SPI edge is driven
 * and there is nothing to capture. Ignoring BUSY lets the bus waveform be
 * verified on a logic analyser without the panel. Never leave this on with a
 * panel connected -- writing while the controller is busy is what the signal
 * exists to prevent. */
static bool s_ignore_busy;

void epd_set_ignore_busy(bool on) { s_ignore_busy = on; }
bool epd_get_ignore_busy(void)    { return s_ignore_busy; }

/* Refresh timeout, adjustable at runtime. A Spectra 6 colour update is tens of
 * seconds and gets markedly slower when cold, so a panel that misses the
 * default is not necessarily faulty -- raising this is how to tell "slow" from
 * "stuck" without a rebuild. */
static uint32_t s_refresh_timeout_ms = 60000;

void epd_set_refresh_timeout(uint32_t ms) { s_refresh_timeout_ms = ms ? ms : 1; }
uint32_t epd_get_refresh_timeout(void)    { return s_refresh_timeout_ms; }

static int wait_busy_stage(uint32_t timeout_ms, const char *stage)
{
    vTaskDelay(pdMS_TO_TICKS(2));

    if (s_ignore_busy) {
        /* Still give the controller the settling time the sequence assumes. */
        vTaskDelay(pdMS_TO_TICKS(20));
        return 0;
    }

    for (uint32_t t = 0; t < timeout_ms; t += 5) {
        if (BUSY()) {
            return 0;
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    /* Name the stage: which wait failed says very different things. A stall at
     * reset or PON means the panel is not talking at all; a stall at DRF means
     * it accepted the image and started the update, so the interface is fine
     * and the problem is downstream -- supply rails under refresh load, or
     * simply a colder/slower panel than the timeout allows. */
    IPRO_LOGE(TAG, "BUSY stuck low for %ums at %s", timeout_ms, stage);
    return -1;
}

/* --- Reset --------------------------------------------------------------
 * Two low pulses, not one (Application Note Figure 10). A single pulse
 * leaves the controller partially initialised and every later command is
 * silently ignored.
 */
static void epd_reset(void)
{
    RSTN_H();
    vTaskDelay(pdMS_TO_TICKS(20));

    RSTN_L();
    vTaskDelay(pdMS_TO_TICKS(35));      /* at least 30 ms */
    RSTN_H();
    vTaskDelay(pdMS_TO_TICKS(35));
    RSTN_L();
    vTaskDelay(pdMS_TO_TICKS(35));
    RSTN_H();
    vTaskDelay(pdMS_TO_TICKS(35));
}

static void gpio_setup(void)
{
    uint8_t outs[] = { PIN_CSB, PIN_SCL, PIN_SDA,
                       PIN_DC,  PIN_RSTN, PIN_BUSY };
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, outs, sizeof(outs));

    /* Drive each pad to its idle level immediately after turning it into an
     * output. Configuring them all first and setting levels afterwards leaves a
     * window where the pad is an output at the register's reset value, which on
     * RST_N is a low pulse -- an unintended reset visible on a scope. It is only
     * a few microseconds against the 30 ms the panel needs to actually reset, so
     * it is harmless, but it is also free to avoid. */
    gpio_set_mode(PIN_RSTN, GPIO_OUTPUT_MODE);
    RSTN_H();
    gpio_set_mode(PIN_CSB,  GPIO_OUTPUT_MODE);
    CSB_H();
    gpio_set_mode(PIN_SCL,  GPIO_OUTPUT_MODE);
    SCL_L();
    gpio_set_mode(PIN_SDA,  GPIO_OUTPUT_MODE);
    SDA_H();
    gpio_set_mode(PIN_DC,   GPIO_OUTPUT_MODE);
    DC_L();
    gpio_set_mode(PIN_BUSY, GPIO_INPUT_MODE);

    /* PWR gates the board's boost regulator (VGH/VGL/VSH/VSL) -- the same
     * role as the reference firmware's SWC pin. It comes up LAST, after every
     * other signal is already at its idle level, and E Ink's own init
     * (spi_9b_init) follows it with a settle delay before the first reset.
     * Reused here on every gpio_setup() call rather than once at boot like
     * the reference does, since this driver re-syncs pin state on every
     * command -- the extra 10 ms is cheap next to the tens of seconds a
     * refresh takes, and turning an already-on rail on again is a no-op.
     * 10 ms is not from the datasheet (undocumented, like PWR itself); it is
     * carried over from the reference's own delay(10) and has not been
     * measured against real rail settle time on this board. */
    gpio_set_mode(PIN_PWR, GPIO_OUTPUT_MODE);
    PWR_ON();
    vTaskDelay(pdMS_TO_TICKS(10));

#if EPD_HW_SPI_ENABLE
    /* gpio_setup() has just pulled SCL/SDA back to plain GPIO, so any earlier
     * SPI pin mux is gone. Re-arm on the next transfer rather than here: the
     * mode may still be switched before one happens. */
    s_hw_ready = false;
#endif
#if EPD_PIO_ENABLE
    epd_pio_release_claim();   /* same reason, for the PIO's pads */
#endif
}

/* --- Public ------------------------------------------------------------- */

int epd_read_rev(uint8_t *out_rev)
{
    gpio_setup();
    epd_reset();
    if (wait_busy_stage(5000, "reset (before REV)") != 0) {
        return -1;
    }

    spi_cmd(CMD_REV);
    (void)spi_read_byte();
    (void)spi_read_byte();
    uint8_t v = spi_read_byte();

    if (out_rev) {
        *out_rev = v;
    }
    return 0;
}

int epd_init(void)
{
    gpio_setup();
    epd_reset();
    if (wait_busy_stage(5000, "reset (before init)") != 0) {
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(30));

    /* Undocumented unlock — absent from the Application Note, present in
     * E Ink's reference. Nothing works without it. */
    spi_cmd(0xAA);
    spi_data(0x49); spi_data(0x55); spi_data(0x20);
    spi_data(0x08); spi_data(0x09); spi_data(0x18);

    spi_cmd(CMD_PWR);
    spi_data(0x3F);

    spi_cmd(CMD_PSR);
    spi_data(0x5F); spi_data(0x69);

    spi_cmd(CMD_BTST1);
    spi_data(0x40); spi_data(0x1F); spi_data(0x1F); spi_data(0x2C);

    spi_cmd(CMD_BTST3);
    spi_data(0x6F); spi_data(0x1F); spi_data(0x1F); spi_data(0x22);

    /* First of two BTST2 settings; the second is applied mid-refresh. */
    spi_cmd(CMD_BTST2);
    spi_data(0x6F); spi_data(0x1F); spi_data(0x17); spi_data(0x17);

    spi_cmd(CMD_POFS);
    spi_data(0x00); spi_data(0x54); spi_data(0x00); spi_data(0x44);

    spi_cmd(CMD_TCON);
    spi_data(0x02); spi_data(0x00);

    /* The reference flags this as mandatory on version 2 silicon. */
    spi_cmd(CMD_PLL);
    spi_data(0x08);

    spi_cmd(CMD_CDI);
    spi_data(0x3F);

    spi_cmd(CMD_TRES);                  /* 0x0190 x 0x0258 = 400 x 600 */
    spi_data(0x01); spi_data(0x90);
    spi_data(0x02); spi_data(0x58);

    spi_cmd(CMD_PWS);
    spi_data(0x2F);

    spi_cmd(CMD_T_VDCS);
    spi_data(0x01);

    IPRO_LOGI(TAG, "init done (%s)",
              s_bus == EPD_BUS_3WIRE ? "3-wire" : "4-wire");
    return 0;
}

/*
 * Shared tail of every update: power on, apply the second booster profile,
 * refresh, power off. The BTST2 here is not a repeat of the init value — the
 * refresh uses a different drive profile and skipping it gives a blank or
 * badly formed image.
 */
static int refresh_tail(void)
{
    spi_cmd(CMD_PON);
    if (wait_busy_stage(10000, "PON (power on)") != 0) {
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    spi_cmd(CMD_BTST2);
    spi_data(0x6F); spi_data(0x1F); spi_data(0x16); spi_data(0x29);

    spi_cmd(CMD_DRF);
    spi_data(0x00);
    /* A six-colour update is tens of seconds; allow generous headroom. */
    if (wait_busy_stage(s_refresh_timeout_ms, "DRF (display refresh)") != 0) {
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    spi_cmd(CMD_POF);
    spi_data(0x00);
    if (wait_busy_stage(10000, "POF (power off)") != 0) {
        return -1;
    }
    return 0;
}

/* --- Burst writes -------------------------------------------------------
 * Per-byte framing costs about 20 us of CSB toggling and call overhead per
 * byte, which at 120000 bytes swamps the clock rate entirely -- 4 MHz buys
 * nothing over bit-banging unless the framing goes away too. Holding CSB low
 * and streaming the frame as one transfer is what actually makes a refresh
 * fast (~0.24 s instead of ~2.4 s).
 *
 * OFF by default, because it departs from E Ink's reference: their SPI_DATA()
 * toggles CSB once per byte. The Application Note only states the CSB-low
 * requirement for reads, and says nothing either way about write bursts, so
 * this has to be confirmed against a real panel before it can be trusted.
 */
static bool s_burst;

void epd_set_burst(bool on) { s_burst = on; }
bool epd_get_burst(void)    { return s_burst; }

/* Stream len bytes as data with a single CSB assertion. 4-wire hardware SPI
 * only -- there is no 9-bit frame for 3-wire, and bit-banging gains nothing. */
static bool burst_active(void)
{
#if EPD_HW_SPI_ENABLE
    return s_burst && hw_spi_active();
#else
    return false;
#endif
}

static void spi_data_burst(const uint8_t *buf, uint32_t len)
{
#if EPD_HW_SPI_ENABLE
    if (!s_hw_ready) {
        hw_spi_init();
    }
    DC_H();
    CSB_L();
    hw_spi_send((void *)buf, len);
    CSB_H();
#else
    (void)buf; (void)len;
#endif
}

/* Emit `n` copies of one data byte through whichever engine is active. Shared
 * by the solid fill and the colour-bar pattern -- both are runs of a constant,
 * so neither needs a frame buffer. */
static void write_run(uint8_t b, uint32_t n)
{
    if (pio_active()) {
        pio_arm();
        if (s_bus == EPD_BUS_4WIRE) {
            DC_H();   /* the whole run is data; 8-bit frames carry no D/C bit */
        }
        epd_pio_fill(b, n);
        epd_pio_drain();
    } else if (burst_active()) {
        /* CSB is re-asserted per chunk rather than per byte, so the framing
         * overhead drops by the chunk size. */
        static uint8_t chunk[256];
        memset(chunk, b, sizeof(chunk));
        while (n) {
            uint32_t k = (n > sizeof(chunk)) ? sizeof(chunk) : n;
            spi_data_burst(chunk, k);
            n -= k;
        }
    } else {
        while (n--) {
            spi_data(b);
        }
    }
}

int epd_fill(epd_colour_t colour)
{
    uint8_t b = (uint8_t)((colour << 4) | colour);   /* two pixels per byte */

    IPRO_LOGI(TAG, "fill 0x%X (byte 0x%02X), %lu bytes...",
              colour, b, (unsigned long)EPD_FRAME_BYTES);

    spi_cmd(CMD_DTM);

    write_run(b, EPD_FRAME_BYTES);
    return refresh_tail();
}

/*
 * Six horizontal bands, one per colour, in a single refresh.
 *
 * Worth having as its own command: epd_cycle proves each colour in turn but
 * takes six full updates (minutes), and a fault that only affects one colour
 * channel is far easier to see with all six side by side. The frame divides
 * exactly -- 120000 / 6 = 20000 bytes = 100 of the 600 rows per band -- so it
 * still needs no frame buffer, just six runs of a constant byte.
 */
int epd_bars(void)
{
    static const epd_colour_t order[6] = {
        EPD_BLACK, EPD_WHITE, EPD_YELLOW, EPD_RED, EPD_BLUE, EPD_GREEN
    };
    const uint32_t band = EPD_FRAME_BYTES / 6u;

    IPRO_LOGI(TAG, "six colour bars, %lu bytes per band (%u rows)...",
              (unsigned long)band, (unsigned)(EPD_HEIGHT / 6));

    spi_cmd(CMD_DTM);
    for (int i = 0; i < 6; i++) {
        uint8_t b = (uint8_t)((order[i] << 4) | order[i]);
        write_run(b, band);
    }
    return refresh_tail();
}

int epd_draw(const uint8_t *frame)
{
    if (frame == NULL) {
        return -1;
    }
    spi_cmd(CMD_DTM);

    if (pio_active()) {
        pio_arm();
        if (s_bus == EPD_BUS_4WIRE) {
            DC_H();
        }
        epd_pio_write_data(frame, EPD_FRAME_BYTES);
        epd_pio_drain();
    } else if (burst_active()) {
        spi_data_burst(frame, EPD_FRAME_BYTES);
    } else {
        for (uint32_t i = 0; i < EPD_FRAME_BYTES; i++) {
            spi_data(frame[i]);
        }
    }
    return refresh_tail();
}

void epd_sleep(void)
{
    spi_cmd(CMD_DSLP);
    spi_data(0xA5);
    IPRO_LOGI(TAG, "deep sleep — only a hardware reset will wake it");
}

/*
 * Board power switch (PWR / SWC), separate from the panel's own DSLP.
 *
 * gpio_setup() turns this on automatically before every command, so calling
 * it with true is not normally needed. Calling it with false cuts the boost
 * regulator supplying VGH/VGL/VSH/VSL -- the reference firmware does this
 * exactly once, at the very end of the whole program, with no DSLP command
 * beforehand. Whether power should be cut before or after a DSLP sequence is
 * not stated anywhere in the Application Note; this driver does not decide
 * that for the caller.
 */
void epd_power_enable(bool on)
{
    gpio_set_mode(PIN_PWR, GPIO_OUTPUT_MODE);
    if (on) {
        PWR_ON();
    } else {
        PWR_OFF();
    }
}
