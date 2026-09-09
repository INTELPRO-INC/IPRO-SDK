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
 * GDEY0213F52 driver — JD79676 BWRY 2.13" e-paper (PDB J2 wiring).
 *
 * 1:1 port of GoodDisplay sample S-GDEY0213F52-V2.0 to IPRO7 SDK, adapted
 * for the IPRO7AI PDB which only exposes 4 SPI lines + power on J2.
 * RES and BUSY are NOT connected:
 *
 *   - RES: tie module RES pin to VCC externally. Without HW reset we cannot
 *     wake from DSLP, so epd_sleep() omits DSLP and leaves the panel in POF
 *     (PON re-wakes on next refresh, no reset needed).
 *
 *   - BUSY: all waits use fixed worst-case delays. epd_busy_level() returns
 *     a constant 1 (assumed idle) since the pin is electrically absent.
 *
 * The three "rules of thumb" of this chip still apply:
 *   1. Init = 0xE9 0x01 preamble + PON. OTP MTP holds correct LUT/voltage.
 *   2. Each byte gets its own CS_LOW...CS_HIGH frame.
 *   3. Each full refresh re-runs the LOCK+PON sequence (epd_display() calls
 *      epd_init() internally).
 */

#include "epd_gdey0213f52.h"

#include <FreeRTOS.h>
#include <task.h>
#include <string.h>
#include <stdio.h>

#include "hal_spi.h"
#include "hal_gpio.h"
#include "drv_glb.h"
#include "drv_spi.h"
#include "ipro_log.h"

#define TAG "EPD"

/* ========================= Pin map (IPRO7AI PDB J2) ====================== */
#define EPD_SPI_INDEX  SPI0_INDEX

#define EPD_PIN_SCK    15  /* SPI0 SCLK, J2.3 — PDB schematic confirms GPIO15 */
#define EPD_PIN_MOSI   16  /* SPI0 MOSI, J2 */
#define EPD_PIN_DC     17  /* GPIO output, J2 (re-purposed MISO pin; EPD is write-only) */
#define EPD_PIN_CS     10  /* GPIO output, J2 — software CS */
/* RES and BUSY are NOT wired on this board. RES on the e-paper module must
 * be tied to VCC (or 10k pull-up). BUSY is intentionally omitted; driver
 * uses fixed worst-case delays. */

#define DC_LOW()       gpio_write(EPD_PIN_DC,  0)
#define DC_HIGH()      gpio_write(EPD_PIN_DC,  1)
#define CS_LOW()       gpio_write(EPD_PIN_CS,  0)
#define CS_HIGH()      gpio_write(EPD_PIN_CS,  1)

/* ============================ JD79676 commands =========================== */
#define CMD_PON     0x04   /* Power ON */
#define CMD_POF     0x02   /* Power OFF */
#define CMD_DSLP    0x07   /* Deep sleep */
#define CMD_DTM     0x10   /* Data start transmission */
#define CMD_DRF     0x12   /* Display refresh */
#define CMD_LOCK    0xE9   /* Undocumented init preamble */

/* ============================== SPI helpers ============================== */
static spi_device_t s_spi = {
    .caps             = 0,
    .id               = EPD_SPI_INDEX,
    .clk              = 4 * 1000 * 1000,
    .mode             = SPI_MASTER_MODE,
    .direction        = SPI_MSB_BYTE0_DIRECTION_FIRST,
    .clk_polaraity    = SPI_POLARITY_LOW,
    .clk_phase        = SPI_PHASE_1EDGE,
    .datasize         = SPI_DATASIZE_8BIT,
    .fifo_threshold   = 0,
    .pin_swap_enable  = 0,
    .delitch_cnt      = 0,
    .callback         = NULL,
};

static void epd_pins_init(void)
{
    /* SCK + MOSI on SPI alt-function. */
    uint8_t spi_pins[2] = { EPD_PIN_SCK, EPD_PIN_MOSI };
    GLB_GPIO_Func_Init(GPIO_FUN_SPI, spi_pins, sizeof(spi_pins));

    /* Force CS/DC back to plain GPIO. Without this, GPIOs that power-up in
     * alt-function mode keep driving their alt signal even after
     * gpio_set_mode. RES/BUSY not wired on PDB so we don't touch them. */
    uint8_t gpio_pins[2] = { EPD_PIN_CS, EPD_PIN_DC };
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, gpio_pins, sizeof(gpio_pins));

    gpio_set_mode(EPD_PIN_CS, GPIO_OUTPUT_MODE);
    gpio_set_mode(EPD_PIN_DC, GPIO_OUTPUT_MODE);

    CS_HIGH();
    DC_HIGH();
}

static void epd_spi_init(void)
{
    spi_setConfig(&s_spi);
    spi_init(EPD_SPI_INDEX);
    spi_enable(EPD_SPI_INDEX, 1);
}

/* Per-byte CS framing — exactly mirrors GoodDisplay's EPD_W21_WriteCMD /
 * EPD_W21_WriteDATA. Every command and every data byte gets its own
 * CS_LOW...write...CS_HIGH frame.
 *
 * spi_transmit() in polling mode signals completion as soon as the byte is
 * loaded into the TX shift register, NOT after the last bit has actually
 * clocked out. Without an explicit busy-bus wait, CS goes high mid-byte
 * and the panel sees a chopped transmission — observable as a 31ms total
 * for 8000 bytes, well below the 8000 * 8 / 4MHz = 16ms shift time plus
 * any per-byte overhead. We poll the bus busy bit to guarantee CS only
 * deasserts once the byte is fully on the wire. */
static inline void spi_wait_idle(void)
{
    while (SPI_GetBusyStatus(EPD_SPI_INDEX)) {
        /* spin */
    }
}

static void epd_write_cmd(uint8_t cmd)
{
    DC_LOW();
    CS_LOW();
    spi_transmit(EPD_SPI_INDEX, &cmd, 1);
    spi_wait_idle();
    CS_HIGH();
}

static void epd_write_data(uint8_t data)
{
    DC_HIGH();
    CS_LOW();
    spi_transmit(EPD_SPI_INDEX, &data, 1);
    spi_wait_idle();
    CS_HIGH();
}

/* Fixed-delay wait — see file-top comment for why BUSY polling is unusable
 * on this wiring. Sized for the worst case of each operation. */
static void epd_wait(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/* ============================ Public API ================================ */
/* RES/BUSY not wired on PDB: BUSY level is unobservable, report "idle". */
int epd_busy_level(void) { return 1; }

/* Power-on settle. Original driver did a HW reset here (RES low->high) but
 * with RES tied to VCC externally we just wait long enough for the chip's
 * internal POR to load OTP. JD79676 power-on settles within tens of ms. */
static void epd_power_on_settle(void)
{
    epd_wait(120);
}

int epd_init(void)
{
    IPRO_LOGI(TAG, "init: SCK=%d MOSI=%d CS=%d DC=%d (RES/BUSY not wired)",
              EPD_PIN_SCK, EPD_PIN_MOSI, EPD_PIN_CS, EPD_PIN_DC);

    epd_pins_init();
    epd_spi_init();
    epd_power_on_settle();

    /* Undocumented preamble — must be present, value comes verbatim from
     * the official sample. */
    epd_write_cmd(CMD_LOCK);
    epd_write_data(0x01);

    epd_write_cmd(CMD_PON);
    epd_wait(200);    /* PON ramp; reference loops on BUSY */

    IPRO_LOGI(TAG, "init OK");
    return 0;
}

/* Pack one 2-bit color value into a byte (4 pixels), MSB-first. */
static inline uint8_t epd_pack4(epd_color_t c)
{
    uint8_t v = (uint8_t)c & 0x3;
    return (uint8_t)((v << 6) | (v << 4) | (v << 2) | v);
}

/* Send 0x10 then 8000 data bytes, each in its own CS frame. Then 0x12 0x00
 * and wait long enough for a full BWRY refresh waveform to complete. */
static int epd_send_frame_and_refresh(const uint8_t *buf, uint32_t len)
{
    IPRO_LOGI(TAG, "DTM: streaming %lu bytes", (unsigned long)len);
    TickType_t t0 = xTaskGetTickCount();
    epd_write_cmd(CMD_DTM);
    for (uint32_t i = 0; i < len; i++) {
        epd_write_data(buf[i]);
        if ((i & 0x3FF) == 0x3FF) vTaskDelay(1);   /* yield once per kB */
    }
    IPRO_LOGI(TAG, "DTM: done in %lu ms",
              (unsigned long)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS));

    IPRO_LOGI(TAG, "DRF: triggering refresh, waiting up to 18s");
    epd_write_cmd(CMD_DRF);
    epd_write_data(0x00);
    /* Spec: full refresh = 16s, fast = 11s. Budget 18s. */
    epd_wait(18000);
    IPRO_LOGI(TAG, "DRF: complete");
    return 0;
}

int epd_sleep(void)
{
    /* Without RES we can't wake from DSLP, so leave the panel in POF only.
     * PON in the next epd_init() will re-power it without a HW reset. */
    IPRO_LOGI(TAG, "sleep: POF (DSLP skipped — no RES wire)");
    epd_write_cmd(CMD_POF);
    epd_write_data(0x00);
    epd_wait(200);
    return 0;
}

/* Reference comment #5: "Re-initialization is required for every full
 * screen update." So a public refresh is always init + frame + sleep. */
int epd_display(const uint8_t *buf, uint32_t len)
{
    if (!buf || len != EPD_FRAME_BYTES) return -1;
    if (epd_init() != 0) return -1;
    if (epd_send_frame_and_refresh(buf, len) != 0) return -1;
    return epd_sleep();
}

int epd_fill(epd_color_t color)
{
    static uint8_t frame[EPD_FRAME_BYTES];
    memset(frame, epd_pack4(color), sizeof(frame));
    return epd_display(frame, sizeof(frame));
}

int epd_stripes_4color(void)
{
    /* With PSR UD=1 (default after reset) the FPC-bottom mounting puts
     * frame row 0 at the visual bottom. List bands bottom-to-top so the
     * viewer reads K -> W -> Y -> R top-to-bottom. */
    static const epd_color_t bands[4] = { EPD_RED, EPD_YELLOW, EPD_WHITE, EPD_BLACK };
    static uint8_t frame[EPD_FRAME_BYTES];

    for (uint32_t y = 0; y < EPD_PANEL_H; y++) {
        epd_color_t c = bands[(y * 4) / EPD_PANEL_H];
        memset(&frame[y * EPD_BYTES_PER_ROW], epd_pack4(c), EPD_BYTES_PER_ROW);
    }
    return epd_display(frame, sizeof(frame));
}

int epd_clear(int cycles)
{
    if (cycles <= 0) cycles = 1;
    for (int i = 0; i < cycles; i++) {
        if (epd_fill(EPD_WHITE) != 0) return -1;
    }
    return 0;
}

/* R17H AUTO + 0xA5 (PON->DRF->POF) — kept as a diagnostic shortcut. Does
 * not load DTM, so it refreshes whatever happens to be in SRAM. */
int epd_auto_refresh(void)
{
    if (epd_init() != 0) return -1;
    epd_write_cmd(0x17);
    epd_write_data(0xA5);
    epd_wait(18000);
    return epd_sleep();
}

/* Diagnostic helper for the high-resolution BUSY watcher in main.c —
 * issues DRF without any waiting so the watcher can take over polling. */
void epd_busy_dump_send_drf(void)
{
    epd_write_cmd(CMD_DRF);
    epd_write_data(0x00);
}
