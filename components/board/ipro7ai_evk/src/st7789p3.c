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
 * @file st7789p3.c
 * @brief ST7789P3 4-wire SPI LCD driver - see st7789p3.h for the interface.
 */

#include "st7789p3.h"
#include "st7789p3_init_seq.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "hal_spi.h"
#include "hal_gpio.h"
#include "drv_glb.h"    /* GLB_GPIO_Func_Init */
#include "drv_gpio.h"   /* GPIO_FUN_SPI / GPIO_FUN_GPIO */
#include "drv_common.h" /* arch_delay_us / arch_delay_ms */
#include "ipro_log.h"

#define LOG_TAG "ST7789P3"

/* MADCTL (36h) bit names, datasheet 9.1.28. */
#define MADCTL_MY   0x80    /* row address order      */
#define MADCTL_MX   0x40    /* column address order   */
#define MADCTL_MV   0x20    /* row/column exchange    */
#define MADCTL_ML   0x10    /* vertical refresh order */
#define MADCTL_BGR  0x08    /* RGB/BGR order          */
#define MADCTL_MH   0x04    /* horizontal refresh order */

/* Bounded spin so a wiring fault can never hang the shell task. */
#define ST_BUSY_SPIN_LIMIT  2000000u

/* Rows between scheduler yields inside a large blit. */
#define ST_BLIT_YIELD_ROWS  16

static bool     s_bus_ready;
static bool     s_ready;
static uint32_t s_spi_clk_hz = ST_SPI_CLK_HZ;
static int      s_framing    = ST7789P3_FRAME_WINDOW;
static int      s_backlight  = 1;
static int      s_seq        = ST7789P3_SEQ_MINIMAL;

static uint8_t  s_dir;
static uint8_t  s_mirror;
static int      s_bgr;
static int      s_inversion  = 1;   /* the init sequences send INVON */
static uint8_t  s_madctl;

static uint16_t s_width  = ST_PANEL_W;
static uint16_t s_height = ST_PANEL_H;

/* Single shared row buffer, held in wire byte order. 320 * 2 = 640 bytes. */
static uint16_t s_rowbuf[ST_MAX_DIM];

/* Guards the SPI bus and s_rowbuf. Recursive so a caller that already holds it
 * (a pattern generator, the camera preview) can still call fill_rect().
 * Created lazily in bus_init so main() needs no ordering rule. */
static SemaphoreHandle_t s_bus_mutex;

/* ---------------------------------------------------------------------------
 * Primitives
 * -------------------------------------------------------------------------*/

static void st_delay_ms(uint32_t ms)
{
    if (ms == 0u) {
        return;
    }
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        vTaskDelay(pdMS_TO_TICKS(ms));
    } else {
        arch_delay_ms(ms);
    }
}

static inline void st_cs(int level)  { gpio_write(ST_PIN_CS, (uint32_t)level); }
static inline void st_dc(int level)  { gpio_write(ST_PIN_DC, (uint32_t)level); }

/*
 * spi_transmit() returns once the last frame is queued, and the hardware busy
 * flag may not have ASSERTED yet - so a naive "wait while busy" can fall
 * straight through while bits are still on the wire. Dropping CS or flipping
 * D/C there truncates the byte. Give busy a bounded chance to assert first,
 * then wait for it to clear. (Same failure mode and fix as the GH7007 driver
 * in apps/test/ipro_gh7007_test.)
 */
static void st_wait_idle(void)
{
    uint32_t spin = 256;
    while (!spi_ioctl(ST_SPI_ID, SPI_CTRL_GET_BUS_BUSY_STATUS, NULL) && --spin) {
    }
    spin = ST_BUSY_SPIN_LIMIT;
    while (spi_ioctl(ST_SPI_ID, SPI_CTRL_GET_BUS_BUSY_STATUS, NULL) && --spin) {
    }
}

void st7789p3_bus_lock(void)
{
    if (s_bus_mutex && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        xSemaphoreTakeRecursive(s_bus_mutex, portMAX_DELAY);
    }
}

void st7789p3_bus_unlock(void)
{
    if (s_bus_mutex && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        xSemaphoreGiveRecursive(s_bus_mutex);
    }
}

static void st_frame_size(uint32_t datasize)
{
    spi_ioctl(ST_SPI_ID, SPI_CTRL_SET_FRAME_SIZE, (void *)(uintptr_t)datasize);
}

/* Program SPI0 from the current clock setting. Split out so `lcd clk` can
 * retune without redoing pinmux. */
static void st_spi_program(void)
{
    spi_device_t dev = {
        .caps            = 0,
        .id              = ST_SPI_ID,
        .clk             = s_spi_clk_hz,
        .mode            = SPI_MASTER_MODE,
        .direction       = SPI_MSB_BYTE0_DIRECTION_FIRST,
        /* CS is a plain GPIO here, so the peripheral must not drive its own. */
        .ignore_cs       = 0,
        .clk_polaraity   = ST_SPI_CPOL,
        .clk_phase       = ST_SPI_CPHA,
        .datasize        = SPI_DATASIZE_8BIT,
        .fifo_threshold  = 0,
        .pin_swap_enable = 0,
        .delitch_cnt     = 0,
        .callback        = NULL,
    };

    /* spi_setConfig() only fills the driver's config struct; spi_init() reads
     * it back to program the hardware. setConfig MUST come first, or init
     * applies the board default (which can swap MOSI/MISO and use hardware CS). */
    spi_setConfig(&dev);
    spi_init(ST_SPI_ID);
    spi_enable(ST_SPI_ID, 1);
}

void st7789p3_bus_init(void)
{
    uint8_t spi_pins[2]  = { ST_PIN_SCLK, ST_PIN_MOSI };
    uint8_t gpio_pins[4] = { ST_PIN_CS, ST_PIN_DC, ST_PIN_RST, ST_PIN_BL };

    if (!s_bus_mutex) {
        s_bus_mutex = xSemaphoreCreateRecursiveMutex();
    }

    /* Only SCLK/MOSI reach the SPI engine. GPIO17 sits on the SPI0 MISO pad
     * but stays a GPIO here because the module has no separate read line. */
    GLB_GPIO_Func_Init(GPIO_FUN_SPI, spi_pins, sizeof(spi_pins));
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, gpio_pins, sizeof(gpio_pins));

    gpio_set_mode(ST_PIN_CS,  GPIO_OUTPUT_MODE);
    gpio_set_mode(ST_PIN_DC,  GPIO_OUTPUT_MODE);
    gpio_set_mode(ST_PIN_RST, GPIO_OUTPUT_MODE);
    gpio_set_mode(ST_PIN_BL,  GPIO_OUTPUT_MODE);

    st_cs(1);                                   /* CS idle high (deselected) */
    st_dc(1);                                   /* DC idle high (data)       */
    gpio_write(ST_PIN_RST, 1);                  /* RESET inactive            */
    gpio_write(ST_PIN_BL, (uint32_t)(s_backlight ? 1 : 0));

    st_spi_program();
    s_bus_ready = true;
}

void st7789p3_reset(void)
{
    st_cs(1);
    gpio_write(ST_PIN_RST, 1);
    st_delay_ms(10);
    gpio_write(ST_PIN_RST, 0);   /* datasheet 7.4.5: >9 us asserts a reset */
    st_delay_ms(10);
    gpio_write(ST_PIN_RST, 1);
    st_delay_ms(150);            /* >=120 ms blanking sequence + NVM reload */
}

/* ---------------------------------------------------------------------------
 * Register access
 * -------------------------------------------------------------------------*/

static void st_tx8(const void *buf, uint32_t nbytes)
{
    spi_transmit(ST_SPI_ID, (void *)buf, nbytes);
    st_wait_idle();
}

void st7789p3_write_reg(uint8_t cmd, const uint8_t *args, uint32_t nargs)
{
    if (!s_bus_ready) {
        return;
    }

    if (s_framing == ST7789P3_FRAME_BYTE) {
        /* One CS window per byte - the framing the SDK's proven ili9341_spi.c
         * uses. Kept as an escape hatch for a marginal link. */
        st_dc(0);
        st_cs(0);
        st_tx8(&cmd, 1);
        st_cs(1);
        st_dc(1);
        for (uint32_t i = 0; i < nargs; i++) {
            st_cs(0);
            st_tx8(&args[i], 1);
            st_cs(1);
        }
        return;
    }

    /* Default: one CS-low window spanning the command and all its parameters,
     * matching the datasheet's 4-line write figure (section 8.3.5). D/C only
     * moves once the command byte has fully clocked out. */
    st_dc(0);
    st_cs(0);
    st_tx8(&cmd, 1);
    if (nargs && args) {
        st_dc(1);
        st_tx8(args, nargs);
    }
    st_cs(1);
    st_dc(1);
}

void st7789p3_write_cmd(uint8_t cmd)
{
    st7789p3_write_reg(cmd, NULL, 0);
}

void st7789p3_write_data(const uint8_t *buf, uint32_t len)
{
    if (!s_bus_ready || !buf || !len) {
        return;
    }
    st_dc(1);
    st_cs(0);
    st_tx8(buf, len);
    st_cs(1);
}

int st7789p3_read_reg(uint8_t cmd, uint8_t *out, uint32_t nbytes)
{
    uint8_t spi_pins[2] = { ST_PIN_SCLK, ST_PIN_MOSI };

    if (!out || nbytes == 0u || nbytes > 8u) {
        return ST7789P3_ERR_PARAM;
    }
    if (!s_bus_ready) {
        st7789p3_bus_init();
    }
    st_wait_idle();

    /* Take SCLK/MOSI away from the SPI engine and bit-bang. The module has one
     * bidirectional SDA line (datasheet "Interface-I"), so the read has to
     * turn that same wire around. */
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, spi_pins, sizeof(spi_pins));
    gpio_set_mode(ST_PIN_SCLK, GPIO_OUTPUT_MODE);
    gpio_set_mode(ST_PIN_MOSI, GPIO_OUTPUT_MODE);
    gpio_write(ST_PIN_SCLK, 0);          /* mode 0: SCLK idles low */

    st_cs(0);
    st_dc(0);

    /* Command byte, MSB first: data set while SCLK is low, latched on rising. */
    for (int i = 7; i >= 0; i--) {
        gpio_write(ST_PIN_MOSI, (uint32_t)((cmd >> i) & 1u));
        arch_delay_us(1);
        gpio_write(ST_PIN_SCLK, 1);
        arch_delay_us(1);
        gpio_write(ST_PIN_SCLK, 0);
    }

    st_dc(1);
    gpio_set_mode(ST_PIN_MOSI, GPIO_INPUT_MODE);   /* release SDA to the chip */
    arch_delay_us(2);

    /* Exactly one dummy clock before the first data bit (datasheet 8.3.5). */
    gpio_write(ST_PIN_SCLK, 1);
    arch_delay_us(1);
    gpio_write(ST_PIN_SCLK, 0);
    arch_delay_us(1);

    for (uint32_t b = 0; b < nbytes; b++) {
        uint8_t v = 0;
        for (int i = 7; i >= 0; i--) {
            gpio_write(ST_PIN_SCLK, 1);
            arch_delay_us(1);
            v = (uint8_t)(v | ((gpio_read(ST_PIN_MOSI) ? 1u : 0u) << i));
            gpio_write(ST_PIN_SCLK, 0);
            arch_delay_us(1);
        }
        out[b] = v;
    }

    st_cs(1);

    /* Hand the pins back to the SPI engine. */
    st7789p3_bus_init();
    return ST7789P3_OK;
}

/* ---------------------------------------------------------------------------
 * Configuration
 * -------------------------------------------------------------------------*/

uint32_t st7789p3_get_spi_clk(void) { return s_spi_clk_hz; }

void st7789p3_set_spi_clk(uint32_t hz)
{
    if (hz == 0u) {
        return;
    }
    s_spi_clk_hz = hz;
    if (s_bus_ready) {
        st_wait_idle();
        st_spi_program();
    }
}

void st7789p3_set_framing(int mode)
{
    s_framing = (mode == ST7789P3_FRAME_BYTE) ? ST7789P3_FRAME_BYTE
                                              : ST7789P3_FRAME_WINDOW;
}

int st7789p3_get_framing(void) { return s_framing; }

void st7789p3_set_backlight(int on)
{
    s_backlight = on ? 1 : 0;
    if (s_bus_ready) {
        gpio_write(ST_PIN_BL, (uint32_t)s_backlight);
    }
}

int st7789p3_get_backlight(void) { return s_backlight; }

static void st_apply_madctl(void)
{
    uint8_t v;

    switch (s_dir & 3u) {
        case 0:  v = 0;                          break;  /* 240x320 portrait  */
        case 1:  v = MADCTL_MV | MADCTL_MX;      break;  /* 320x240 landscape */
        case 2:  v = MADCTL_MY | MADCTL_MX;      break;  /* portrait flipped  */
        default: v = MADCTL_MV | MADCTL_MY;      break;  /* landscape flipped */
    }
    if (s_mirror) {
        v ^= MADCTL_MX;
    }
    if (s_bgr) {
        v |= MADCTL_BGR;
    }

    s_madctl = v;
    st7789p3_write_reg(0x36, &v, 1);
}

int st7789p3_set_dir(uint8_t dir, uint8_t mirror)
{
    if (dir > 3u) {
        return ST7789P3_ERR_PARAM;
    }
    s_dir    = dir;
    s_mirror = mirror ? 1u : 0u;

    if (dir & 1u) {
        s_width  = ST_PANEL_H;
        s_height = ST_PANEL_W;
    } else {
        s_width  = ST_PANEL_W;
        s_height = ST_PANEL_H;
    }

    st_apply_madctl();
    return ST7789P3_OK;
}

uint8_t st7789p3_get_dir(void) { return s_dir; }

void st7789p3_set_bgr(int bgr)
{
    s_bgr = bgr ? 1 : 0;
    st_apply_madctl();
}

int st7789p3_get_bgr(void) { return s_bgr; }

void st7789p3_set_inversion(int on)
{
    s_inversion = on ? 1 : 0;
    st7789p3_write_cmd(s_inversion ? 0x21 : 0x20);
}

int st7789p3_get_inversion(void) { return s_inversion; }

void st7789p3_set_sleep(int sleep_in)
{
    st7789p3_write_cmd(sleep_in ? 0x10 : 0x11);
    st_delay_ms(120);      /* datasheet 9.1.11/9.1.12: 120 ms before next cmd */
}

void st7789p3_set_display(int on)
{
    st7789p3_write_cmd(on ? 0x29 : 0x28);
    st_delay_ms(10);
}

void st7789p3_gamma_default(void)
{
    st7789p3_write_reg(0xE0, ST7789P3_GAMMA_P_DEFAULT,
                       sizeof(ST7789P3_GAMMA_P_DEFAULT));
    st7789p3_write_reg(0xE1, ST7789P3_GAMMA_N_DEFAULT,
                       sizeof(ST7789P3_GAMMA_N_DEFAULT));
}

uint8_t st7789p3_get_madctl(void) { return s_madctl; }

int  st7789p3_get_seq(void)   { return s_seq; }
bool st7789p3_is_ready(void)  { return s_ready; }

uint16_t st7789p3_width(void)  { return s_width; }
uint16_t st7789p3_height(void) { return s_height; }

/* ---------------------------------------------------------------------------
 * Init
 * -------------------------------------------------------------------------*/

static void st_run_sequence(const st7789p3_cmd_t *seq, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        st7789p3_write_reg(seq[i].cmd, seq[i].args, seq[i].nargs);
        st_delay_ms(seq[i].delay_ms);
    }
}

int st7789p3_init(int seq, int clear)
{
    const st7789p3_cmd_t *table;
    uint32_t len;

    if (seq == ST7789P3_SEQ_MINIMAL) {
        table = st7789p3_init_minimal;
        len   = ST7789P3_INIT_MINIMAL_LEN;
    } else if (seq == ST7789P3_SEQ_TUNED) {
        table = st7789p3_init_tuned;
        len   = ST7789P3_INIT_TUNED_LEN;
    } else {
        return ST7789P3_ERR_PARAM;
    }

    s_ready = false;
    st7789p3_bus_init();
    st7789p3_reset();

    /* The sequences write MADCTL 00h, so re-sync the cached orientation state
     * instead of letting a stale `lcd dir` linger across a re-init. */
    s_dir       = 0;
    s_mirror    = 0;
    s_bgr       = 0;
    s_inversion = 1;
    s_madctl    = 0;
    s_width     = ST_PANEL_W;
    s_height    = ST_PANEL_H;

    st_run_sequence(table, len);
    s_seq   = seq;
    s_ready = true;

    /* The sequences leave MADCTL at 0 (portrait). Apply the board's mounting
     * orientation so every path - boot banner, `lcd init`, camera preview -
     * starts the same way up. */
    st7789p3_set_dir(ST_DEFAULT_DIR, 0);

    IPRO_LOGI(LOG_TAG, "init done: seq=%s %ux%u RGB565 dir=%u sclk=%u Hz "
                       "framing=%s",
              (seq == ST7789P3_SEQ_TUNED) ? "tuned" : "minimal",
              (unsigned)s_width, (unsigned)s_height, (unsigned)s_dir,
              (unsigned)s_spi_clk_hz,
              (s_framing == ST7789P3_FRAME_BYTE) ? "per-byte" : "per-window");

    if (clear) {
        st7789p3_fill_screen(ST_BLACK);
    }
    return ST7789P3_OK;
}

/* ---------------------------------------------------------------------------
 * Pixel output
 * -------------------------------------------------------------------------*/

void st7789p3_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t args[4];

    /* GATECTRL defaults put the visible area at column 0 / row 0 on this
     * 240x320 glass, so no start offset is applied here. */
    args[0] = (uint8_t)(x0 >> 8);
    args[1] = (uint8_t)(x0 & 0xFF);
    args[2] = (uint8_t)(x1 >> 8);
    args[3] = (uint8_t)(x1 & 0xFF);
    st7789p3_write_reg(0x2A, args, 4);          /* CASET */

    args[0] = (uint8_t)(y0 >> 8);
    args[1] = (uint8_t)(y0 & 0xFF);
    args[2] = (uint8_t)(y1 >> 8);
    args[3] = (uint8_t)(y1 & 0xFF);
    st7789p3_write_reg(0x2B, args, 4);          /* RASET */
}

void st7789p3_blit_start(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    const uint8_t ramwr = 0x2C;

    st7789p3_set_window(x0, y0, x1, y1);

    /* RAMWR opens the burst; CS stays low for the whole pixel stream. */
    st_dc(0);
    st_cs(0);
    st_tx8(&ramwr, 1);
    st_dc(1);

    st_frame_size(SPI_DATASIZE_16BIT);
}

void st7789p3_blit_push(const uint16_t *pixels, uint16_t npix)
{
    if (!pixels || !npix) {
        return;
    }
    spi_transmit(ST_SPI_ID, (void *)pixels, npix);   /* size is in frames */
}

void st7789p3_blit_end(void)
{
    st_wait_idle();
    st_frame_size(SPI_DATASIZE_8BIT);
    st_cs(1);
}

/* Clip an inclusive rectangle to the panel. Returns false if nothing is left. */
static bool st_clip(uint16_t *x0, uint16_t *y0, uint16_t *x1, uint16_t *y1)
{
    if (*x1 < *x0 || *y1 < *y0) {
        return false;
    }
    if (*x0 >= s_width || *y0 >= s_height) {
        return false;
    }
    if (*x1 >= s_width) {
        *x1 = (uint16_t)(s_width - 1u);
    }
    if (*y1 >= s_height) {
        *y1 = (uint16_t)(s_height - 1u);
    }
    return true;
}

void st7789p3_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                        uint16_t color)
{
    uint16_t w, h;
    uint16_t wire;

    if (!s_bus_ready || !st_clip(&x0, &y0, &x1, &y1)) {
        return;
    }

    st7789p3_bus_lock();
    w = (uint16_t)(x1 - x0 + 1u);
    h = (uint16_t)(y1 - y0 + 1u);
    wire = st7789p3_swap(color);
    for (uint16_t i = 0; i < w; i++) {
        s_rowbuf[i] = wire;
    }

    st7789p3_blit_start(x0, y0, x1, y1);
    for (uint16_t row = 0; row < h; row++) {
        st7789p3_blit_push(s_rowbuf, w);
        if ((row % ST_BLIT_YIELD_ROWS) == (ST_BLIT_YIELD_ROWS - 1u) &&
            xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
            st_wait_idle();
            taskYIELD();
        }
    }
    st7789p3_blit_end();
    st7789p3_bus_unlock();
}

void st7789p3_fill_screen(uint16_t color)
{
    st7789p3_fill_rect(0, 0, (uint16_t)(s_width - 1u),
                       (uint16_t)(s_height - 1u), color);
}

void st7789p3_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    st7789p3_fill_rect(x, y, x, y, color);
}

void st7789p3_draw_row(uint16_t x0, uint16_t y, const uint16_t *row, uint16_t npix)
{
    uint16_t x1;

    if (!s_bus_ready || !row || !npix || y >= s_height || x0 >= s_width) {
        return;
    }
    if ((uint32_t)x0 + npix > s_width) {
        npix = (uint16_t)(s_width - x0);
    }
    x1 = (uint16_t)(x0 + npix - 1u);

    st7789p3_bus_lock();
    for (uint16_t i = 0; i < npix; i++) {
        s_rowbuf[i] = st7789p3_swap(row[i]);
    }

    st7789p3_blit_start(x0, y, x1, y);
    st7789p3_blit_push(s_rowbuf, npix);
    st7789p3_blit_end();
    st7789p3_bus_unlock();
}

/* ---------------------------------------------------------------------------
 * Diagnostics
 * -------------------------------------------------------------------------*/

void st7789p3_pin_out(uint8_t pin, int level)
{
    uint8_t p[1] = { pin };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, p, 1);
    gpio_set_mode(pin, GPIO_OUTPUT_MODE);
    gpio_write(pin, (uint32_t)(level ? 1 : 0));
    s_bus_ready = false;     /* pinmux disturbed - force a re-init before use */
}

int st7789p3_pin_in(uint8_t pin)
{
    uint8_t p[1] = { pin };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, p, 1);
    gpio_set_mode(pin, GPIO_INPUT_MODE);
    s_bus_ready = false;
    return gpio_read(pin);
}
