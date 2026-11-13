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
 * @file spi_bitbang.c
 * @brief GPIO-driven Mode-3 SPI master for low-speed sensors.
 */

#include "spi_bitbang.h"

#include "drv_glb.h"
#include "drv_gpio.h"

#include <FreeRTOS.h>
#include <semphr.h>

#include <string.h>

/* ---- per-bus state (small fixed table indexed by bus_id & 0x0F) ---- */
typedef struct {
    bool              initialized;
    uint8_t           sclk, miso, mosi, cs;
    uint32_t          delay_loops;  /* tuned per clk_hz_hint */
    SemaphoreHandle_t lock;
    StaticSemaphore_t lock_buf;
} bb_ctx_t;

#define BB_MAX 2
static bb_ctx_t s_bb[BB_MAX];

static inline bb_ctx_t *ctx_of(uint8_t bus_id)
{
    uint8_t idx = bus_id & 0x0F;
    return (idx < BB_MAX) ? &s_bb[idx] : NULL;
}

/* Tight delay — tuned empirically. The IPRO7 runs @ 192 MHz, each loop body
 * below (≈ 4 insns) is about 25 ns, so clk_delay_loops ≈ (half_period_ns/25). */
static inline void delay_loops(uint32_t n)
{
    while (n--) {
        __asm__ volatile("nop");
    }
}

/* Byte-level shift: send tx, receive rx. Mode 3: CPOL=1 (idle high), CPHA=1
 * (data sampled on trailing/rising edge). So sequence per bit:
 *    drive MOSI with MSB, SCLK↓, half-delay, SCLK↑, sample MISO, half-delay.
 */
static uint8_t shift_byte(bb_ctx_t *b, uint8_t tx)
{
    uint8_t rx = 0;
    for (int i = 7; i >= 0; i--) {
        /* Set MOSI before falling edge */
        GLB_GPIO_Write(b->mosi, (tx >> i) & 1);
        GLB_GPIO_Write(b->sclk, 0);
        delay_loops(b->delay_loops);

        /* Sample MISO just before the rising edge */
        rx = (uint8_t)((rx << 1) | (GLB_GPIO_Read(b->miso) & 1));
        GLB_GPIO_Write(b->sclk, 1);
        delay_loops(b->delay_loops);
    }
    return rx;
}

/* ---- public API ---- */

static void apply_pads(const bb_ctx_t *b)
{
    /* Select GPIO (SWGPIO) function on all four pads FIRST. GLB_GPIO_OUTPUT_Enable
     * only flips the output-enable bit — it does NOT set the pad's function mux. If
     * a pin came out of reset (or a prior driver) in another function (e.g. GPIO28-33
     * can route to the SD host), the GPIO controller can't drive/read it and the SPI
     * read clocks out garbage. Setting GPIO_FUN_GPIO here makes the bus robust to the
     * pad's prior state. MISO is an input with a pull-up so a disconnected line reads a
     * definite 0xFF (vs. floating noise) during bring-up. */
    GLB_GPIO_Cfg_Type outc = { .gpioFun = GPIO_FUN_GPIO, .gpioMode = GPIO_MODE_OUTPUT,
                               .pullType = GPIO_PULL_NONE, .drive = 0, .smtCtrl = 0 };
    outc.gpioPin = b->sclk; GLB_GPIO_Init(&outc);
    outc.gpioPin = b->mosi; GLB_GPIO_Init(&outc);
    outc.gpioPin = b->cs;   GLB_GPIO_Init(&outc);

    GLB_GPIO_Cfg_Type inc = { .gpioPin = b->miso, .gpioFun = GPIO_FUN_GPIO,
                              .gpioMode = GPIO_MODE_INPUT, .pullType = GPIO_PULL_UP,
                              .drive = 0, .smtCtrl = 1 };
    GLB_GPIO_Init(&inc);
    GLB_GPIO_INPUT_Enable(b->miso);

    /* Drive outputs idle (SCLK high per Mode 3, CS high, MOSI low). */
    GLB_GPIO_OUTPUT_Enable(b->sclk);
    GLB_GPIO_OUTPUT_Enable(b->mosi);
    GLB_GPIO_OUTPUT_Enable(b->cs);
    GLB_GPIO_Write(b->sclk, 1);
    GLB_GPIO_Write(b->mosi, 0);
    GLB_GPIO_Write(b->cs,   1);
}

int spi_bitbang_init(uint8_t bus_id,
                     uint8_t sclk, uint8_t miso,
                     uint8_t mosi, uint8_t cs,
                     uint32_t clk_hz_hint)
{
    bb_ctx_t *b = ctx_of(bus_id);
    if (!b) return -1;
    if (b->initialized) return 0;

    b->sclk = sclk;
    b->miso = miso;
    b->mosi = mosi;
    b->cs   = cs;

    /* ~25 ns per empty loop iteration on 192 MHz core — scale to half-period */
    uint32_t half_ns = (clk_hz_hint > 0) ? (500000000u / clk_hz_hint) : 1000u;
    b->delay_loops = half_ns / 25u;
    if (b->delay_loops == 0) b->delay_loops = 1;

    b->lock = xSemaphoreCreateMutexStatic(&b->lock_buf);
    if (!b->lock) return -2;

    apply_pads(b);

    b->initialized = true;
    return 0;
}

int spi_bitbang_restore_pads(uint8_t bus_id)
{
    bb_ctx_t *b = ctx_of(bus_id);
    if (!b || !b->initialized) return -1;
    apply_pads(b);
    return 0;
}

int spi_bitbang_xfer(uint8_t bus_id, const uint8_t *tx, uint8_t *rx, uint32_t len)
{
    bb_ctx_t *b = ctx_of(bus_id);
    if (!b || !b->initialized || len == 0) return -1;

    xSemaphoreTake(b->lock, portMAX_DELAY);

    /* Assert CS */
    GLB_GPIO_Write(b->cs, 0);
    delay_loops(b->delay_loops);

    for (uint32_t i = 0; i < len; i++) {
        uint8_t t = tx ? tx[i] : 0xFF;
        uint8_t r = shift_byte(b, t);
        if (rx) rx[i] = r;
    }

    /* Release CS */
    GLB_GPIO_Write(b->sclk, 1);
    delay_loops(b->delay_loops);
    GLB_GPIO_Write(b->cs, 1);

    xSemaphoreGive(b->lock);
    return 0;
}

int spi_bitbang_read(uint8_t bus_id, uint8_t reg, uint8_t *buf, uint32_t len)
{
    if (!buf || len == 0 || len > 256) return -1;

    uint8_t tx[1 + 256];
    uint8_t rx[1 + 256];
    memset(tx, 0xFF, len + 1);
    tx[0] = reg | 0x80;

    int rc = spi_bitbang_xfer(bus_id, tx, rx, len + 1);
    if (rc != 0) return rc;

    memcpy(buf, &rx[1], len);
    return 0;
}

int spi_bitbang_write(uint8_t bus_id, uint8_t reg, const uint8_t *buf, uint32_t len)
{
    if (!buf || len == 0 || len > 256) return -1;

    uint8_t tx[1 + 256];
    tx[0] = reg & 0x7F;
    memcpy(&tx[1], buf, len);

    return spi_bitbang_xfer(bus_id, tx, NULL, len + 1);
}
