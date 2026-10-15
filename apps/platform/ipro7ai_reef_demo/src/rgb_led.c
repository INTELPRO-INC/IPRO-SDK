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
 * @file rgb_led.c
 * @brief WS2812 over SPI0 MOSI. See rgb_led.h for the encoding.
 */

#include "rgb_led.h"
#include "board_config.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "hal_spi.h"
#include "hal_gpio.h"
#include "drv_glb.h"
#include "ipro_log.h"

#define LOG_TAG "RGB"

/* 3.2 MHz = 32 MHz XCLK / (1 * 2 * 5): SPI_SetClock lands on it exactly. */
#define WS2812_SPI_HZ          3200000u
#define WS2812_BITS_PER_LED    24u
#define WS2812_SPI_BYTES       (WS2812_BITS_PER_LED / 2u)   /* 4 SPI bits per WS bit */

/* Bytes of zeros to hold the line low after the frame. 40 bytes at 2.5 us
 * each = 100 us, past the 50 us reset of the classic part; the newer 280 us
 * parts get their reset from the idle line between calls. */
#define WS2812_RESET_BYTES     40u

static uint8_t s_r, s_g, s_b;
static int     s_ready;

static void ws2812_pin_spi(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioPin  = BOARD_RGB_LED_PIN,
        .gpioFun  = GPIO_FUN_SPI,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_NONE,
        .drive    = 1,
        .smtCtrl  = 1,
    };
    GLB_GPIO_Init(&cfg);
}

static void ws2812_pin_gpio_low(void)
{
    uint8_t pin[1] = { BOARD_RGB_LED_PIN };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(BOARD_RGB_LED_PIN, GPIO_OUTPUT_MODE);
    gpio_write(BOARD_RGB_LED_PIN, 0);
}

int rgb_led_init(void)
{
    if (s_ready) {
        return 0;
    }

    /* spi_setConfig() must precede spi_init(): init reads the device table. */
    spi_device_t dev = {
        .id              = SPI0_INDEX,
        .clk             = WS2812_SPI_HZ,
        .mode            = SPI_MASTER_MODE,
        .direction       = SPI_MSB_BYTE0_DIRECTION_FIRST,
        .ignore_cs       = 1,
        .clk_polaraity   = SPI_POLARITY_LOW,
        .clk_phase       = SPI_PHASE_1EDGE,
        .datasize        = SPI_DATASIZE_8BIT,
        .fifo_threshold  = 0,
        /* pin % 4 == 0 is MOSI only with the swap OFF; the board default
         * (swap 1) makes it MISO and the LED never sees a bit (bench,
         * 2026-10-06). See ipro7 SPI pin-role table: n%4 0/1 = MOSI/MISO. */
        .pin_swap_enable = 0,
        .delitch_cnt     = 0,
    };
    spi_setConfig(&dev);
    if (spi_init(SPI0_INDEX) != 0) {
        IPRO_LOGE(LOG_TAG, "spi_init failed");
        return -1;
    }
    spi_enable(SPI0_INDEX, 1);
    ws2812_pin_spi();
    s_ready = 1;
    return 0;
}

/* Expand one colour byte into four SPI bytes, MSB first. */
static void encode_byte(uint8_t v, uint8_t *dst)
{
    static const uint8_t nibble[4] = { 0x88, 0x8C, 0xC8, 0xCC }; /* 00 01 10 11 */

    dst[0] = nibble[(v >> 6) & 3];
    dst[1] = nibble[(v >> 4) & 3];
    dst[2] = nibble[(v >> 2) & 3];
    dst[3] = nibble[(v >> 0) & 3];
}

int rgb_led_set(uint8_t r, uint8_t g, uint8_t b)
{
    /* GRB on the wire, plus the low tail. Static so it is not on a task
     * stack while the SPI engine reads it. */
    static uint8_t frame[WS2812_SPI_BYTES + WS2812_RESET_BYTES];
    int rc;

    if (!s_ready && rgb_led_init() != 0) {
        return -1;
    }

    encode_byte(g, &frame[0]);
    encode_byte(r, &frame[4]);
    encode_byte(b, &frame[8]);
    memset(&frame[WS2812_SPI_BYTES], 0, WS2812_RESET_BYTES);

    /* The TX FIFO is fed by the CPU; a scheduler tick in the middle of the
     * 12 data bytes would open a gap on the line. 30 us in a critical
     * section is cheap. */
    taskENTER_CRITICAL();
    rc = spi_transmit(SPI0_INDEX, frame, sizeof(frame));
    taskEXIT_CRITICAL();
    if (rc != 0) {
        IPRO_LOGE(LOG_TAG, "spi_transmit failed (%d)", rc);
        return -1;
    }

    s_r = r;
    s_g = g;
    s_b = b;
    return 0;
}

void rgb_led_off(void)
{
    if (s_ready) {
        (void)rgb_led_set(0, 0, 0);
        spi_enable(SPI0_INDEX, 0);
        s_ready = 0;
    }
    ws2812_pin_gpio_low();
    s_r = s_g = s_b = 0;
}

void rgb_led_get(uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (r) *r = s_r;
    if (g) *g = s_g;
    if (b) *b = s_b;
}
