/**
 * @file ili9488_spi.c
 * @brief
 *
 * Copyright (c) 2021 IntelPro team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "../lcd.h"

#if defined(LCD_SPI_ILI9488)

#include "ili9488_spi.h"
#include "hal_spi.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "drv_glb.h"
#include "drv_spi.h"


#define ILI9488_SPI_ID        0
#define ILI9488_SPI_INDEX     SPI0_INDEX
#define ILI9488_SPI_DMA_ID    DMA0_ID
#define ILI9488_SPI_DMA_INDEX DMA0_CH0_INDEX



/* asynchronous flush interrupt callback */
typedef void (*ili9488_spi_callback)(void);
static void ili9488_spi_irq_cb(void *arg, uint32_t event);
void ili9488_spi_dma_flush_callback(void *arg, uint32_t event);

static volatile ili9488_spi_callback ili9488_spi_async_callback = NULL;
static volatile bool ili9488_spi_sync_flush_flag = 0;

//static struct device *ili9488_spi;
//static struct device *dma_spi_tx;

static spi_device_t g_spi_dev = {
    .caps = 0,
    .id = ILI9488_SPI_INDEX,
    .clk = (48* 1000 * 1000),
    .mode = SPI_MASTER_MODE,
    .direction = SPI_MSB_BYTE0_DIRECTION_FIRST,
    .clk_polaraity = SPI_POLARITY_LOW,
    .clk_phase = SPI_PHASE_1EDGE,
    .datasize = SPI_DATASIZE_8BIT,
    .fifo_threshold = 0,
    .pin_swap_enable = 0, /*swap mosi and miso*/
    .delitch_cnt = 0,
    .callback = ili9488_spi_irq_cb,
};
static dma_device_t g_dma_dev = {
    .id = ILI9488_SPI_DMA_ID,
    .ch = ILI9488_SPI_DMA_INDEX,
    .transfer_mode = DMA_LLI_ONCE_MODE,
    .direction = DMA_MEMORY_TO_MEMORY,
    .src_req = DMA_REQUEST_NONE,
    .dst_req = DMA_REQUEST_NONE,
    .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_size = DMA_BURST_INCR1,
    .dst_burst_size = DMA_BURST_INCR1,
    .src_width = DMA_TRANSFER_WIDTH_16BIT,
    .dst_width = DMA_TRANSFER_WIDTH_16BIT,
    .lli_cfg = NULL,
    .callback = ili9488_spi_dma_flush_callback,
};


const ili9488_spi_init_cmd_t ili9488_spi_init_cmds[] = {
    { 0x01, NULL, 0 },  /* software reset */
    { 0xFF, NULL, 10 }, /* delay 10ms */

    { 0x11, NULL, 0 },   /* Sleep Out */
    { 0xFF, NULL, 120 }, /* delay 120ms */

    { 0xE0, "\x00\x07\x0F\x0D\x1B\x0A\x3C\x78\x4A\x07\x0E\x09\x1B\x1E\x0F", 15 }, /* PGAMCTRL (Positive Gamma Control) */
    { 0xE1, "\x00\x22\x24\x26\x12\x07\x36\x47\x47\x06\x0A\x07\x30\x37\x0F", 15 }, /* NGAMCTRL (Negative Gamma Control) */

    { 0xC0, "\x10\x10", 2 }, /* Power Control 1 */
    { 0xC1, "\x41", 1 },     /* Power Control 2 */

    { 0xC5, "\x00\x2C\x80", 3 }, /* VCOM Control */
#if (ILI9488_SPI_ROTATE_FOR_480x320 == 0)
    /*for 320x480 source*/
    /*{ 0x36, "\x08", 1 },*/         /* Memory Access Control , BGR=1*/
    { 0x36, "\x00", 1 },         /* Memory Access Control , BGR=0*/
#else
    /*for 480x320 source*/
     { 0x36, "\xa8", 1 },         /* Memory Access Control , MY=1, MV=1, BGR=1*/
    // { 0x36, "\xa0", 1 },         /* Memory Access Control , MY=1, MV=1, BGR=0*/
#endif

#if 0
    { 0x3A, "\x55", 1 }, /* Interface Pixel Format RGB565 */
#else
    { 0x3A, "\x66", 1 }, /* Interface Pixel Format RGB666  DBI type C 4 wire only support RGB666*/
#endif

    { 0xB0, "\x00", 1 },     /* Interface Mode Control */
    { 0xB1, "\xB0", 1 },     /* Frame rate 70Hz */
    { 0xB4, "\x02", 1 },     /* Display Inversion Control */
    { 0xB6, "\x02\x22", 2 }, /* Display Function Control */
    { 0xE9, "\x00", 1 },     /* Set Image Function */

    { 0xF7, "\xA9\x51\x2C\x82", 4 }, /* Adjust Control 3 */

#if defined(ENABLE_INVERSION_FOR_IPS_LCD_PANNEL)
    { 0x21, NULL, 0 }, /*Display Inversion ON*/
#endif

    { 0x29, NULL, 0 }, /* Display On */
    { 0xFF, NULL, 10 },
};

dma_lli_ctrl_t *g_ptlliCfg;

#if 1 /*#ifndef FPGA*/
static void ili9488_spi_dma_realchip_init_gpio(void)
{
#if 0
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[2];
    uint8_t num;
    GLB_GPIO_FUNC_Type nFuncType;

    //memset(&gpiopins[0], 0, sizeof(gpiopins));

    gpiopins[ 0] = ILI9488_SPI_SCLK_PIN;
    gpiopins[ 1] = ILI9488_SPI_MOSI_PIN;
    //gpiopins[ 2] = ILI9488_SPI_CS_PIN;
    //gpiopins[ 3] = ILI9488_SPI_DC_PIN;
    num = 4;
    nFuncType = GPIO_FUN_SPI;

    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.slRate = 0;
    cfg.outputMode = 0;
    cfg.gpioFun = nFuncType;

    for (uint8_t i = 0; i < num; i++) {
        cfg.gpioPin = gpiopins[i];
        cfg.gpioMode = GPIO_MODE_AF;
        GLB_GPIO_Init(&cfg);
    }
#else
    uint8_t gpiopins[2];

    gpiopins[ 0] = ILI9488_SPI_SCLK_PIN;
    gpiopins[ 1] = ILI9488_SPI_MOSI_PIN;

    GLB_GPIO_Func_Init(GPIO_FUN_SPI, &gpiopins[0], sizeof(gpiopins));
#endif
}
#endif

static void ili9488_spi_irq_cb(void *arg, uint32_t event)
{
    printf("SPI IRQ raised\r\n");
//    printf("spi send:\r\n");
//    dump_buffer_u8(send_data, SPI_TEST_DATA_LEN);

//    printf("\r\nspi recv:\r\n");
//    dump_buffer_u8(recv_data, SPI_TEST_DATA_LEN);
}


/**
 * @brief ili9488_spi_dma_flush_callback
 *
 * @return
 */
void ili9488_spi_dma_flush_callback(void *arg, uint32_t event)
{
    if (ili9488_spi_sync_flush_flag == true) {
        ili9488_spi_sync_flush_flag = false;
        return;
    }

    while (lcd_draw_is_busy()) {
    };

    if (ili9488_spi_async_callback != NULL) {
        ili9488_spi_async_callback();
    }

    ILI9488_SPI_CS_HIGH;
    free(g_ptlliCfg); /*try to fix free run out of memory error.*/

}

/**
 * @brief ili9488_spi_async_callback_register
 *
 * @return
 */
void ili9488_spi_async_callback_register(void (*callback)(void))
{
    ili9488_spi_async_callback = callback;
}

/**
 * @brief ili9488_spi_init
 *
 * @return int  0:succes  1:error
 */
static int ili9488_spi_peripheral_init(void)
{

    ili9488_spi_dma_realchip_init_gpio();   /*switch GPIO pins to SPI function.*/

    gpio_set_mode(ILI9488_SPI_CS_PIN, GPIO_OUTPUT_MODE);
    gpio_set_mode(ILI9488_SPI_DC_PIN, GPIO_OUTPUT_MODE);
    ILI9488_SPI_CS_HIGH;
    ILI9488_SPI_DC_HIGH;


    /* config spi using dma*/
    g_spi_dev.mode = SPI_MASTER_MODE;
    //g_spi_dev.caps = COMMON_CAPS_DMA_TX;
    g_spi_dev.caps = 0; /*COMMON_CAPS_INT_TX;*/ /*COMMON_CAPS_DMA_TX;*/
    spi_setConfig(&g_spi_dev);
    spi_init(ILI9488_SPI_INDEX);
    spi_enable(ILI9488_SPI_INDEX, 1);

    /* config dma */
    g_dma_dev.direction = DMA_MEMORY_TO_PERIPH;
    g_dma_dev.src_req = DMA_REQUEST_NONE;
    g_dma_dev.dst_req = DMA_REQUEST_SPI0_TX;
    g_dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    g_dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    g_dma_dev.src_width = DMA_TRANSFER_WIDTH_8BIT;
    g_dma_dev.dst_width = DMA_TRANSFER_WIDTH_8BIT;
    dma_setConfig(&g_dma_dev);
    dma_init(ILI9488_SPI_DMA_ID, g_dma_dev.ch);
    dma_ioctl(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX, DMA_CTRL_SET_INT, NULL);
    //L1C_DCACHE_CLEAN_RANGE((unsigned long)send_data, SPI_TEST_DATA_LEN);
    //dma_reload(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX, (uint32_t)send_data, (uint32_t)DMA_ADDR_SPI_TDR, SPI_TEST_DATA_LEN, NULL);
    //dma_channel_start(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX);

    return 0;
}

/**
 * @brief ili9488_spi_write_cmd
 *
 * @param cmd
 * @return int 0:succes  1:error
 */
static int ili9488_spi_write_cmd(uint8_t cmd)
{
    ILI9488_SPI_DC_LOW;
    ILI9488_SPI_CS_LOW;
    //int res = device_write(ili9488_spi, 0, &cmd, 1);
    int res = spi_transmit(ILI9488_SPI_INDEX, &cmd, 1);
    ILI9488_SPI_CS_HIGH;
    ILI9488_SPI_DC_HIGH;
    return res;
}

/**
 * @brief ili9488_spi_write_data_byte
 *
 * @param data
 * @return int 0:succes  1:error
 */
static int ili9488_spi_write_data_byte(uint8_t data)
{
    ILI9488_SPI_CS_LOW;
    //int res = device_write(ili9488_spi, 0, &data, 1);
    int res = spi_transmit(ILI9488_SPI_INDEX, &data, 1);
    ILI9488_SPI_CS_HIGH;
    return res;
}

/**
 * @brief ili9488_spi_draw_is_busy, After the call ili9488_spi_draw_picture_dma must check this,
 *         if ili9488_spi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int ili9488_spi_draw_is_busy(void)
{
    if (dma_channel_check_busy(g_dma_dev.id, g_dma_dev.ch)) {
        return 1;
    } else {
        /* Wait for tx FIFO to be empty */
        while (spi_ioctl(ILI9488_SPI_INDEX, SPI_CTRL_GET_TX_FIFO, NULL) < SPI_FIFO_LEN)
            ;
        /* Wait for the SPI bus to be idle */
        while (spi_ioctl(ILI9488_SPI_INDEX, SPI_CTRL_GET_BUS_BUSY_STATUS, NULL) != 0)
            ;
        /* Switch the SPI to non-DMA mode */
        spi_ioctl(ILI9488_SPI_INDEX, SPI_CTRL_TX_DMA_SUSPEND, NULL);
        ILI9488_SPI_CS_HIGH;

        spi_ioctl(ILI9488_SPI_INDEX, SPI_CTRL_SET_FRAME_SIZE, (void *)SPI_DATASIZE_8BIT);
        return 0;
    }
}

/**
 * @brief ili9488_spi_init
 *
 * @return int
 */
int ili9488_spi_init()
{
    int res = ili9488_spi_peripheral_init();
    if (res) {
        return res;
    }

    for (uint16_t i = 0; i < (sizeof(ili9488_spi_init_cmds) / sizeof(ili9488_spi_init_cmd_t)); i++) {
        if (ili9488_spi_init_cmds[i].cmd == 0xFF) {
            arch_delay_ms(ili9488_spi_init_cmds[i].databytes);
        } else {
            /* send register address */
            res |= ili9488_spi_write_cmd(ili9488_spi_init_cmds[i].cmd);

            /* send register data */
            for (uint8_t j = 0; j < (ili9488_spi_init_cmds[i].databytes & 0x7F); j++) {
                res |= ili9488_spi_write_data_byte(ili9488_spi_init_cmds[i].data[j]);
            }

            if (res) {
                return res;
            }
        }
    }
    ili9488_spi_set_draw_window(0, 0, ILI9488_SPI_H, ILI9488_SPI_W);
    return res;
}


/**
 * @brief lcd_swap_color_data16
 *
 * @param dst destination
 * @param src source
 * @param color_num color num
 * @return int
 */
//static int lcd_swap_color_data16(uint16_t *dst, uint16_t *src, uint32_t color_num)
//{
//    for (size_t i = 0; i < color_num; i++) {
//        dst[i] = (src[i] << 8) | (src[i] >> 8);
//    }
//    return 0;
//}



/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int ili9488_spi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t param;
    switch (dir) {
        case 0:
            if (!mir_flag)
                param = 0x00;
            else
                param = 0x01;
            break;
        case 1:
            if (!mir_flag)
                param = 0x60;
            else
                param = 0x20;
            break;
        case 2:
            if (!mir_flag)
                param = 0xC0;
            else
                param = 0x80;
            break;
        case 3:
            if (!mir_flag)
                param = 0xA0;
            else
                param = 0xE0;

            break;
        default:
            return -1;
            break;
    }
    ili9488_spi_write_cmd(0x36);
    ili9488_spi_write_data_byte(param);
    return dir;
}

/**
 * @brief ili9488_spi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void ili9488_spi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if ILI9488_SPI_OFFSET_X
    x1 += ILI9488_SPI_OFFSET_X;
    x2 += ILI9488_SPI_OFFSET_X;
#endif
#if ILI9488_SPI_OFFSET_Y
    y1 += ILI9488_SPI_OFFSET_Y;
    y2 += ILI9488_SPI_OFFSET_Y;
#endif

    ili9488_spi_write_cmd(0x2a);
    ili9488_spi_write_data_byte(x1 >> 8);
    ili9488_spi_write_data_byte(x1);
    ili9488_spi_write_data_byte(x2 >> 8);
    ili9488_spi_write_data_byte(x2);

    ili9488_spi_write_cmd(0x2b);
    ili9488_spi_write_data_byte(y1 >> 8);
    ili9488_spi_write_data_byte(y1);
    ili9488_spi_write_data_byte(y2 >> 8);
    ili9488_spi_write_data_byte(y2);

    ili9488_spi_write_cmd(0x2c); //move to right before start dma
}

/**
 * @brief ili9488_spi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void ili9488_spi_draw_point(uint16_t x, uint16_t y, ili9488_spi_color_t color)
{
    color = ((color >> 8) & 0xFF) | color << 8;

    ili9488_spi_set_draw_window(x, y, x, y);
    ILI9488_SPI_DC_HIGH;
    ILI9488_SPI_CS_LOW;
    //device_write(ili9488_spi, 0, &color, 2);
    spi_transmit(ILI9488_SPI_INDEX, &color, 2);

    ILI9488_SPI_CS_HIGH;
}

/**
 * @brief ili9488_spi_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void ili9488_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_spi_color_t color)
{
    uint32_t pixelDataSize, PixelCount = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;
    dma_trans_param_t tdmaTransParam;

    color_src = color;

    ILI9488_SPI_CS_HIGH;

    ili9488_spi_set_draw_window(x1, y1, x2, y2);

    /* get pixel Data Size */
    pixelDataSize = PixelCount << 2;    /*send 32bit ti SPI TX fifo and output 24bit only*/

    /* config spi using dma*/
    spi_deinit(ILI9488_SPI_INDEX);
    g_spi_dev.mode = SPI_MASTER_MODE;
    g_spi_dev.caps = COMMON_CAPS_DMA_TX;
    g_spi_dev.datasize = SPI_DATASIZE_24BIT;
    spi_setConfig(&g_spi_dev);
    spi_init(ILI9488_SPI_INDEX);
    spi_enable(ILI9488_SPI_INDEX, 1);

    /* config dma */
    dma_close(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX);
    g_dma_dev.direction = DMA_MEMORY_TO_PERIPH;
    g_dma_dev.src_req = DMA_REQUEST_NONE;
    g_dma_dev.dst_req = DMA_REQUEST_SPI0_TX;
    g_dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    g_dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    g_dma_dev.src_width = DMA_TRANSFER_WIDTH_32BIT;
    g_dma_dev.dst_width = DMA_TRANSFER_WIDTH_32BIT;
    dma_setConfig(&g_dma_dev);
    dma_init(ILI9488_SPI_DMA_ID, g_dma_dev.ch);
    dma_ioctl(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX, DMA_CTRL_SET_INT, NULL);

    ILI9488_SPI_CS_LOW;

    /* sync mode, temporary shutdown interrupt */
    ili9488_spi_sync_flush_flag = true;

    /* dma write */
    spi_ioctl(ILI9488_SPI_INDEX, SPI_CTRL_TX_DMA_RESUME, NULL);

    L1C_DCACHE_CLEAN_RANGE((unsigned long)&color_src, sizeof(color_src));
    dma_reload(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX, (uint32_t)&color_src, (uint32_t)DMA_ADDR_SPI_TDR, pixelDataSize, &g_ptlliCfg);
    dma_channel_start(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX);

    /* Wait to finish, and cs high */
    while (ili9488_spi_draw_is_busy()) {
    };

    ILI9488_SPI_CS_HIGH;
    free(g_ptlliCfg); /*try to fix free run out of memory error.*/

    /* enable src_addr_inc */
    tdmaTransParam.map          = DMA_PARAM_UPDATE_SRC_ADDR_INC;
    tdmaTransParam.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    dma_ioctl(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX, DMA_CTRL_CHANNEL_TRANS_PARAM_UPDATE, (void*)&tdmaTransParam);

}

void ili9488_spi_draw_area_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_spi_color_t color)
{
    uint32_t pixelDataSize, PixelCount = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;
    //dma_trans_param_t tdmaTransParam;

    color_src = color;

    ILI9488_SPI_CS_HIGH;

    ili9488_spi_set_draw_window(x1, y1, x2, y2);

    /* get pixel Data Size */
    pixelDataSize = PixelCount << 2;    /*send 32bit ti SPI TX fifo and output 24bit only*/


    /* config spi using dma*/
    spi_deinit(ILI9488_SPI_INDEX);
    g_spi_dev.mode = SPI_MASTER_MODE;
    g_spi_dev.caps = COMMON_CAPS_DMA_TX;
    g_spi_dev.datasize = SPI_DATASIZE_24BIT;
    spi_setConfig(&g_spi_dev);
    spi_init(ILI9488_SPI_INDEX);
    spi_enable(ILI9488_SPI_INDEX, 1);

    /* config dma */
    dma_close(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX);
    g_dma_dev.direction = DMA_MEMORY_TO_PERIPH;
    g_dma_dev.src_req = DMA_REQUEST_NONE;
    g_dma_dev.dst_req = DMA_REQUEST_SPI0_TX;
    g_dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    g_dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    g_dma_dev.src_width = DMA_TRANSFER_WIDTH_32BIT;
    g_dma_dev.dst_width = DMA_TRANSFER_WIDTH_32BIT;
    dma_setConfig(&g_dma_dev);
    dma_init(ILI9488_SPI_DMA_ID, g_dma_dev.ch);
    dma_ioctl(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX, DMA_CTRL_SET_INT, NULL);

    ILI9488_SPI_CS_LOW;

    /* sync mode, temporary shutdown interrupt */
    //ili9488_spi_sync_flush_flag = true;

    /* dma write */
    spi_ioctl(ILI9488_SPI_INDEX, SPI_CTRL_TX_DMA_RESUME, NULL);

    L1C_DCACHE_CLEAN_RANGE((unsigned long)&color_src, sizeof(color_src));
    dma_reload(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX, (uint32_t)&color_src, (uint32_t)DMA_ADDR_SPI_TDR, pixelDataSize, &g_ptlliCfg);
    dma_channel_start(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX);

}

/**
 * @brief ili9488_spi_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (ili9488_spi_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */

void ili9488_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_spi_color_t *picture)
{
    size_t picture_size = (x2 - x1 + 1) * (y2 - y1 + 1);

    ILI9488_SPI_CS_HIGH;

    ili9488_spi_set_draw_window(x1, y1, x2, y2);

    /* config spi using dma*/
    spi_deinit(ILI9488_SPI_INDEX);
    g_spi_dev.mode = SPI_MASTER_MODE;
    g_spi_dev.caps = COMMON_CAPS_DMA_TX;
    g_spi_dev.datasize = SPI_DATASIZE_24BIT;
    spi_setConfig(&g_spi_dev);
    spi_init(ILI9488_SPI_INDEX);
    spi_enable(ILI9488_SPI_INDEX, 1);


    /* config dma */
    dma_close(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX);
    g_dma_dev.direction = DMA_MEMORY_TO_PERIPH;
    g_dma_dev.src_req = DMA_REQUEST_NONE;
    g_dma_dev.dst_req = DMA_REQUEST_SPI0_TX;
    g_dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    g_dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    g_dma_dev.src_width = DMA_TRANSFER_WIDTH_32BIT;
    g_dma_dev.dst_width = DMA_TRANSFER_WIDTH_32BIT;
    dma_setConfig(&g_dma_dev);
    dma_init(ILI9488_SPI_DMA_ID, g_dma_dev.ch);
    dma_ioctl(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX, DMA_CTRL_SET_INT, NULL);


    ILI9488_SPI_CS_LOW;

    L1C_DCACHE_CLEAN_RANGE((unsigned long)picture, (picture_size << 2));
    dma_reload(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX, (uint32_t)picture, (uint32_t)DMA_ADDR_SPI_TDR, (picture_size << 2), &g_ptlliCfg);
    dma_channel_start(ILI9488_SPI_DMA_ID, ILI9488_SPI_DMA_INDEX);

}

/**
 * @brief ili9488_spi_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void ili9488_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_spi_color_t *picture)
{
    /* sync mode, temporary shutdown interrupt */
    ili9488_spi_sync_flush_flag = true;

    ili9488_spi_draw_picture_nonblocking(x1, y1, x2, y2, picture);
    while (ili9488_spi_draw_is_busy()) {
    };
}

#endif
