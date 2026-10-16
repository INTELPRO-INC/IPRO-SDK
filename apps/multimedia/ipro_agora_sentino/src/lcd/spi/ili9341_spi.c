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
 * @file ili9341_spi.c
 * @brief ILI9341 SPI LCD driver (240x320, RGB565) — DMA mode
 *
 * Pixel transfers use DMA0_CH2 (DAC uses CH1, no conflict).
 * Commands and small writes still use polling spi_transmit().
 * DMA completion fires an async callback for LVGL flush_ready().
 */

#include "../lcd.h"

#if defined(LCD_SPI_ILI9341)

#include "ili9341_spi.h"
#include "hal_spi.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "drv_glb.h"
#include "drv_spi.h"
#include "system_soc.h"  /* L1C_DCACHE_CLEAN_RANGE */
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define ILI9341_SPI_INDEX SPI0_INDEX

/* DMA channel for LCD SPI TX (DAC uses CH1) */
#define LCD_DMA_CH  DMA0_CH2_INDEX

static spi_device_t g_spi_dev = {
    .caps = 0,
    .id = ILI9341_SPI_INDEX,
    .clk = (36 * 1000 * 1000),
    .mode = SPI_MASTER_MODE,
    .direction = SPI_MSB_BYTE0_DIRECTION_FIRST,
    .clk_polaraity = SPI_POLARITY_LOW,
    .clk_phase = SPI_PHASE_1EDGE,
    .datasize = SPI_DATASIZE_8BIT,
    .fifo_threshold = 0,
    .pin_swap_enable = 0,
    .delitch_cnt = 0,
    .callback = NULL,
};

/* DMA device for SPI TX */
static dma_device_t g_lcd_dma = {
    .id             = DMA0_ID,
    .ch             = LCD_DMA_CH,
    .transfer_mode  = DMA_LLI_ONCE_MODE,
    .direction      = DMA_MEMORY_TO_PERIPH,
    .src_req        = DMA_REQUEST_NONE,
    .dst_req        = DMA_REQUEST_SPI0_TX,
    .src_addr_inc   = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc   = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_size = DMA_BURST_INCR1,
    .dst_burst_size = DMA_BURST_INCR1,
    .src_width      = DMA_TRANSFER_WIDTH_16BIT,
    .dst_width      = DMA_TRANSFER_WIDTH_16BIT,
    .lli_count      = 1,
    .lli_cfg        = NULL,
    .callback       = NULL,
};

/* Async callback (LVGL flush_ready) */
static void (*g_async_cb)(void) = NULL;
static volatile int g_dma_busy = 0;
static SemaphoreHandle_t g_dma_sem = NULL;

/* ILI9341 initialization command sequence */
const ili9341_spi_init_cmd_t ili9341_spi_init_cmds[] = {
    { 0x01, NULL, 0 },   /* Software Reset */
    { 0xFF, NULL, 10 },  /* delay 10ms */

    { 0x11, NULL, 0 },   /* Sleep Out */
    { 0xFF, NULL, 120 }, /* delay 120ms */

    { 0xCF, "\x00\xC1\x30", 3 },
    { 0xED, "\x64\x03\x12\x81", 4 },
    { 0xE8, "\x85\x00\x78", 3 },
    { 0xCB, "\x39\x2C\x00\x34\x02", 5 },
    { 0xF7, "\x20", 1 },
    { 0xEA, "\x00\x00", 2 },

    { 0xC0, "\x23", 1 },
    { 0xC1, "\x10", 1 },
    { 0xC5, "\x3E\x28", 2 },
    { 0xC7, "\x86", 1 },

    { 0x36, "\x88", 1 },  /* Memory Access Control: MY=1, BGR=1 (portrait, Y-flipped) */
    { 0x3A, "\x55", 1 },  /* Pixel Format: RGB565 */

    { 0xB1, "\x00\x18", 2 },
    { 0xB6, "\x08\x82\x27", 3 },

    { 0xF2, "\x00", 1 },
    { 0x26, "\x01", 1 },

    { 0xE0, "\x0F\x31\x2B\x0C\x0E\x08\x4E\xF1\x37\x07\x10\x03\x0E\x09\x00", 15 },
    { 0xE1, "\x00\x0E\x14\x03\x11\x07\x31\xC1\x48\x08\x0F\x0C\x31\x36\x0F", 15 },

    { 0x20, NULL, 0 },   /* Display Inversion OFF */
    { 0x29, NULL, 0 },   /* Display ON */
    { 0xFF, NULL, 10 },
};

/* ------------------------------------------------------------------ */
/* DMA completion callback                                             */
/* ------------------------------------------------------------------ */

static void lcd_dma_callback(void *dev, uint32_t event)
{
    (void)dev;
    if (event != HAL_DMA_INT_COMPLETE)
        return;

    /* DMA done but SPI FIFO may still be draining — wait for SPI idle */
    while (spi_ioctl(ILI9341_SPI_INDEX, SPI_CTRL_GET_BUS_BUSY_STATUS, NULL))
        ;

    /* Deassert CS after all data is clocked out */
    ILI9341_SPI_CS_HIGH;

    /* Disable SPI DMA TX, switch back to polling for commands */
    spi_ioctl(ILI9341_SPI_INDEX, SPI_CTRL_TX_DMA_SUSPEND, NULL);

    /* Restore 8-bit frame size for commands */
    spi_ioctl(ILI9341_SPI_INDEX, SPI_CTRL_SET_FRAME_SIZE,
              (void *)(uintptr_t)SPI_DATASIZE_8BIT);

    g_dma_busy = 0;

    /* Notify LVGL flush_ready via registered callback */
    if (g_async_cb)
        g_async_cb();

    /* Wake any blocking waiter */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (g_dma_sem)
        xSemaphoreGiveFromISR(g_dma_sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* ------------------------------------------------------------------ */
/* GPIO + SPI init                                                     */
/* ------------------------------------------------------------------ */

static void ili9341_spi_init_gpio(void)
{
    uint8_t gpiopins[2];
    gpiopins[0] = ILI9341_SPI_SCLK_PIN;
    gpiopins[1] = ILI9341_SPI_MOSI_PIN;
    GLB_GPIO_Func_Init(GPIO_FUN_SPI, &gpiopins[0], sizeof(gpiopins));
}

static int ili9341_spi_peripheral_init(void)
{
    ili9341_spi_init_gpio();

    gpio_set_mode(ILI9341_SPI_CS_PIN, GPIO_OUTPUT_MODE);
    gpio_set_mode(ILI9341_SPI_DC_PIN, GPIO_OUTPUT_MODE);
    gpio_set_mode(ILI9341_SPI_RST_PIN, GPIO_OUTPUT_MODE);
    gpio_set_mode(ILI9341_SPI_BL_PIN, GPIO_OUTPUT_MODE);

    ILI9341_SPI_CS_HIGH;
    ILI9341_SPI_DC_HIGH;

    /* Hardware reset sequence */
    ILI9341_SPI_RST_HIGH;
    arch_delay_ms(5);
    ILI9341_SPI_RST_LOW;
    arch_delay_ms(20);
    ILI9341_SPI_RST_HIGH;
    arch_delay_ms(150);

    /* Enable backlight */
    ILI9341_SPI_BL_HIGH;

    /* Configure SPI in 8-bit polling mode (default for commands) */
    g_spi_dev.caps = 0;
    g_spi_dev.datasize = SPI_DATASIZE_8BIT;
    spi_setConfig(&g_spi_dev);
    spi_init(ILI9341_SPI_INDEX);
    spi_enable(ILI9341_SPI_INDEX, 1);

    /* Configure DMA channel for LCD SPI TX */
    g_lcd_dma.callback = lcd_dma_callback;
    dma_setConfig(&g_lcd_dma);
    dma_init(g_lcd_dma.id, g_lcd_dma.ch);
    dma_ioctl(g_lcd_dma.id, g_lcd_dma.ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(g_lcd_dma.id, g_lcd_dma.ch, DMA_CTRL_SET_CALLBACK,
              lcd_dma_callback);

    /* Create semaphore for blocking waits */
    if (g_dma_sem == NULL)
        g_dma_sem = xSemaphoreCreateBinary();

    return 0;
}

/* ------------------------------------------------------------------ */
/* SPI command/data helpers (polling — fast, small transfers)           */
/* ------------------------------------------------------------------ */

static int ili9341_spi_write_cmd(uint8_t cmd)
{
    ILI9341_SPI_DC_LOW;
    ILI9341_SPI_CS_LOW;
    int res = spi_transmit(ILI9341_SPI_INDEX, &cmd, 1);
    ILI9341_SPI_CS_HIGH;
    ILI9341_SPI_DC_HIGH;
    return res;
}

static int ili9341_spi_write_data_byte(uint8_t data)
{
    ILI9341_SPI_CS_LOW;
    int res = spi_transmit(ILI9341_SPI_INDEX, &data, 1);
    ILI9341_SPI_CS_HIGH;
    return res;
}

static void ili9341_spi_set_frame_size(uint32_t datasize)
{
    spi_ioctl(ILI9341_SPI_INDEX, SPI_CTRL_SET_FRAME_SIZE,
              (void *)(uintptr_t)datasize);
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

int ili9341_spi_draw_is_busy(void)
{
    return g_dma_busy;
}

void ili9341_spi_async_callback_register(void (*callback)(void))
{
    g_async_cb = callback;
}

int ili9341_spi_init(void)
{
    int res = ili9341_spi_peripheral_init();
    if (res)
        return res;

    for (uint16_t i = 0; i < (sizeof(ili9341_spi_init_cmds) / sizeof(ili9341_spi_init_cmd_t)); i++) {
        if (ili9341_spi_init_cmds[i].cmd == 0xFF) {
            arch_delay_ms(ili9341_spi_init_cmds[i].databytes);
        } else {
            res |= ili9341_spi_write_cmd(ili9341_spi_init_cmds[i].cmd);
            for (uint8_t j = 0; j < (ili9341_spi_init_cmds[i].databytes & 0x7F); j++) {
                res |= ili9341_spi_write_data_byte(ili9341_spi_init_cmds[i].data[j]);
            }
            if (res)
                return res;
        }
    }

    ili9341_spi_set_draw_window(0, 0, ILI9341_SPI_W - 1, ILI9341_SPI_H - 1);
    return res;
}

int ili9341_spi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t param;
    switch (dir) {
        case 0: param = mir_flag ? 0x80 : 0x88; break;  /* MY=1 for Y-flip */
        case 1: param = mir_flag ? 0x20 : 0x28; break;
        case 2: param = mir_flag ? 0x80 : 0x88; break;
        case 3: param = mir_flag ? 0xE0 : 0xE8; break;
        default: return -1;
    }
    ili9341_spi_write_cmd(0x36);
    ili9341_spi_write_data_byte(param);
    return dir;
}

void ili9341_spi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    spi_enable(ILI9341_SPI_INDEX, 1);
#if ILI9341_SPI_OFFSET_X
    x1 += ILI9341_SPI_OFFSET_X;
    x2 += ILI9341_SPI_OFFSET_X;
#endif
#if ILI9341_SPI_OFFSET_Y
    y1 += ILI9341_SPI_OFFSET_Y;
    y2 += ILI9341_SPI_OFFSET_Y;
#endif

    ili9341_spi_write_cmd(0x2A);
    ili9341_spi_write_data_byte(x1 >> 8);
    ili9341_spi_write_data_byte(x1);
    ili9341_spi_write_data_byte(x2 >> 8);
    ili9341_spi_write_data_byte(x2);

    ili9341_spi_write_cmd(0x2B);
    ili9341_spi_write_data_byte(y1 >> 8);
    ili9341_spi_write_data_byte(y1);
    ili9341_spi_write_data_byte(y2 >> 8);
    ili9341_spi_write_data_byte(y2);

    ili9341_spi_write_cmd(0x2C);
}

void ili9341_spi_draw_point(uint16_t x, uint16_t y, ili9341_spi_color_t color)
{
    uint16_t c = ((color >> 8) & 0xFF) | (color << 8);

    ili9341_spi_set_draw_window(x, y, x, y);
    ILI9341_SPI_DC_HIGH;
    ILI9341_SPI_CS_LOW;
    spi_transmit(ILI9341_SPI_INDEX, &c, 2);
    ILI9341_SPI_CS_HIGH;
}

void ili9341_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_spi_color_t color)
{
    uint32_t width  = (uint32_t)(x2 - x1 + 1);
    uint32_t height = (uint32_t)(y2 - y1 + 1);
    uint16_t c = ((color >> 8) & 0xFF) | (color << 8);

    uint16_t line_buf[240];
    uint32_t row_len = (width <= 240) ? width : 240;
    for (uint32_t i = 0; i < row_len; i++)
        line_buf[i] = c;

    ili9341_spi_set_draw_window(x1, y1, x2, y2);

    ili9341_spi_set_frame_size(SPI_DATASIZE_16BIT);
    ILI9341_SPI_DC_HIGH;
    ILI9341_SPI_CS_LOW;

    for (uint32_t row = 0; row < height; row++) {
        spi_transmit(ILI9341_SPI_INDEX, line_buf, row_len);
    }

    ILI9341_SPI_CS_HIGH;
    ili9341_spi_set_frame_size(SPI_DATASIZE_8BIT);
}

void ili9341_spi_draw_area_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_spi_color_t color)
{
    /* Solid fill is fast enough with polling */
    ili9341_spi_draw_area(x1, y1, x2, y2, color);
}

/**
 * @brief Polling picture draw — CPU feeds SPI FIFO, no DMA.
 * Same speed as DMA (SPI clock is bottleneck), but frees DMA for DAC.
 * Sends row-by-row to avoid large spi_transmit calls (stack safety).
 */
void ili9341_spi_draw_picture_polling(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_spi_color_t *picture)
{
    uint32_t width = (uint32_t)(x2 - x1 + 1);
    uint32_t height = (uint32_t)(y2 - y1 + 1);

    ili9341_spi_set_draw_window(x1, y1, x2, y2);

    ili9341_spi_set_frame_size(SPI_DATASIZE_16BIT);
    ILI9341_SPI_DC_HIGH;
    ILI9341_SPI_CS_LOW;

    for (uint32_t row = 0; row < height; row++) {
        spi_transmit(ILI9341_SPI_INDEX, &picture[row * width], width);
    }

    ILI9341_SPI_CS_HIGH;
    ili9341_spi_set_frame_size(SPI_DATASIZE_8BIT);
}

/**
 * @brief Non-blocking picture draw via DMA.
 *
 * Sets up the draw window, enables SPI DMA TX, starts DMA transfer.
 * Returns immediately. DMA callback deasserts CS and calls g_async_cb.
 */
void ili9341_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_spi_color_t *picture)
{
    uint32_t pixel_count = (uint32_t)(x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t byte_count  = pixel_count * 2;

    /* Wait if previous DMA still running */
    while (g_dma_busy) {
        if (g_dma_sem)
            xSemaphoreTake(g_dma_sem, pdMS_TO_TICKS(50));
    }

    ili9341_spi_set_draw_window(x1, y1, x2, y2);

    /* Wait for any previous SPI polling transfer to finish */
    while (spi_ioctl(ILI9341_SPI_INDEX, SPI_CTRL_GET_BUS_BUSY_STATUS, NULL))
        ;

    /* Switch to 16-bit frame, clear FIFO, enable DMA TX */
    ili9341_spi_set_frame_size(SPI_DATASIZE_16BIT);
    spi_ioctl(ILI9341_SPI_INDEX, SPI_CTRL_CLEAR_TX_FIFO, NULL);
    spi_ioctl(ILI9341_SPI_INDEX, SPI_CTRL_ATTACH_TX_DMA, NULL);

    ILI9341_SPI_DC_HIGH;
    ILI9341_SPI_CS_LOW;

    /* Flush cache so DMA reads correct data */
    L1C_DCACHE_CLEAN_RANGE(picture, byte_count);

    g_dma_busy = 1;

    /* Start DMA: memory → SPI TX FIFO */
    dma_reload(g_lcd_dma.id, g_lcd_dma.ch,
               (uint32_t)(uintptr_t)picture,
               DMA_ADDR_SPI_TDR,
               byte_count, NULL);
    dma_channel_start(g_lcd_dma.id, g_lcd_dma.ch);
}

/**
 * @brief Blocking picture draw — starts DMA then waits for completion.
 */
void ili9341_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_spi_color_t *picture)
{
    ili9341_spi_draw_picture_nonblocking(x1, y1, x2, y2, picture);

    /* Wait for DMA completion */
    while (g_dma_busy) {
        if (g_dma_sem)
            xSemaphoreTake(g_dma_sem, pdMS_TO_TICKS(100));
    }
}

#endif
