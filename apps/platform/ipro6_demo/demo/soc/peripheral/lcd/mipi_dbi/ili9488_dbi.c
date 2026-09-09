/**
 * @file ili9488_dbi.c
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
#include "platform_common.h"

#if defined(LCD_DBI_ILI9488)



#include "ili9488_dbi.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "drv_dbi.h"
#include "drv_dma.h"
#include "drv_glb.h"



#define ILI9488_DBI_DMA_ID    DMA0_ID
#define ILI9488_DBI_DMA_INDEX DMA0_CH0_INDEX


/* asynchronous flush interrupt callback */
typedef void (*ili9488_dbi_callback)(void);
static volatile ili9488_dbi_callback ili9488_dbi_async_callback = NULL;
static volatile bool ili9488_dbi_sync_flush_flag = 0;
void ili9488_dbi_dma_flush_callback(void *arg, uint32_t event);

/* DMA handle */
static dma_device_t g_dma_dev = {
    .id = ILI9488_DBI_DMA_ID,
    .ch = ILI9488_DBI_DMA_INDEX,
    .transfer_mode = DMA_LLI_ONCE_MODE,
    .direction = DMA_MEMORY_TO_PERIPH,
    .src_req = DMA_REQUEST_NONE,
    .dst_req = DMA_REQUEST_DBI_TX,
    .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_size = DMA_BURST_INCR1,
    .dst_burst_size = DMA_BURST_INCR1,
    .src_width = DMA_TRANSFER_WIDTH_32BIT,
    .dst_width = DMA_TRANSFER_WIDTH_32BIT,
    .lli_cfg = NULL,
    .callback = ili9488_dbi_dma_flush_callback,
};

/* DBI cfg */
static DBI_CFG_Type dbi_cfg;

/* high speed  CLK cfg */
static DBI_Period_CFG_Type period_hs;
/* low speed  CLK cfg */
static DBI_Period_CFG_Type period_ls;

#if 1 /*original init flow ==> OK for DBI TYPE C 4 WIRE*/
const ili9488_dbi_init_cmd_t ili9488_dbi_init_cmds[] = {
    { 0x01, NULL, 0 },  /* software reset */
    { 0xFF, NULL, 10 }, /* delay 10ms */

    { 0x11, NULL, 0 },   /* Sleep Out */
    { 0xFF, NULL, 120 }, /* delay 120ms */

    { 0xE0, "\x00\x07\x0F\x0D\x1B\x0A\x3C\x78\x4A\x07\x0E\x09\x1B\x1E\x0F", 15 }, /* PGAMCTRL (Positive Gamma Control) */
    { 0xE1, "\x00\x22\x24\x26\x12\x07\x36\x47\x47\x06\x0A\x07\x30\x37\x0F", 15 }, /* NGAMCTRL (Negative Gamma Control) */

    { 0xC0, "\x10\x10", 2 }, /* Power Control 1 */
    { 0xC1, "\x41", 1 },     /* Power Control 2 */

    { 0xC5, "\x00\x2C\x80", 3 }, /* VCOM Control */
#if (ILI9488_DBI_ROTATE_FOR_480x320 == 0)
    /*for 320x480 source*/
    { 0x36, "\x08", 1 },         /* Memory Access Control , BGR=1*/
#else
    /*for 480x320 source*/
    // { 0x36, "\xa8", 1 },         /* Memory Access Control , MY=1, MV=1, BGR=1*/
    { 0x36, "\xa0", 1 },         /* Memory Access Control , MY=1, MV=1, BGR=0*/
#endif

#if (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB565)
    { 0x3A, "\x55", 1 }, /* Interface Pixel Format RGB565 */
#elif (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB666)
    { 0x3A, "\x66", 1 }, /* Interface Pixel Format RGB666 */
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
#else /*porting from TFT_eSPI*/
const ili9488_dbi_init_cmd_t ili9488_dbi_init_cmds[] = {
// Configure ILI9488 display
    { 0x01, NULL, 0 },  /* software reset */
    { 0xFF, NULL, 10 }, /* delay 10ms */

    { 0xE0, "\x00\x03\x09\x08\x16\x0A\x3F\x78\x4C\x09\x0A\x08\x16\x1A\x0F", 15 }, /* PGAMCTRL (Positive Gamma Control) */
    { 0xE1, "\x00\x16\x19\x03\x0F\x05\x32\x45\x46\x04\x0E\x0D\x35\x37\x0F", 15 }, /* NGAMCTRL (Negative Gamma Control) */


    { 0XC0, "\x17\x15", 2), // Power Control 1
    { 0xC1, "\x41", 1},     // Power Control 2

    { 0xC5, "\x00\x12\x80", 3}, // VCOM Control

    { 0x36, "\x48", 1},     // Memory Access Control // MX, BGR


#if (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB565)  /*defined (TFT_PARALLEL_8_BIT) || defined (TFT_PARALLEL_16_BIT) || defined (RPI_DISPLAY_TYPE)*/
    { 0x3A,"\x55", 1},  // Pixel Interface Format // 16-bit colour for parallel
#else   /*#elif (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB666)*/
    { 0x3A,"\x66", 1},  // Pixel Interface Format // 18-bit colour for SPI
#endif

    { 0xB0,"\x00", 1},  // Interface Mode Control

    { 0xB1,"\xA0", 1},  // Frame Rate Control


    { 0xB4,"\x02", 1},  // Display Inversion Control

    { 0xB6,"\x02\x02\x3B", 3},  // Display Function Control

    { 0xB7,"\xC6", 1},  // Entry Mode Set

    { 0xF7,"\xA9\x51\x2C\x82", 4},  // Adjust Control 3

    { 0x11, NULL, 0 },   //Exit Sleep
    { 0xFF, NULL, 120 }, /* delay 10ms */

    { 0x29, NULL, 0 }, /* Display On */
    { 0xFF, NULL, 25 },

// End of ILI9488 display configuration
};
#endif


/**
 * @brief ili9488_dbi_dma_flush_callback
 *
 * @return
 */

void ili9488_dbi_dma_flush_callback(void *arg, uint32_t event)
{
    if (ili9488_dbi_sync_flush_flag == true) {
        ili9488_dbi_sync_flush_flag = false;
        return;
    }

    while (lcd_draw_is_busy()) {
    };
    if (ili9488_dbi_async_callback != NULL) {
        ili9488_dbi_async_callback();
    }
}

/**
 * @brief ili9488_dbi_async_callback_register
 *
 * @return
 */
void ili9488_dbi_async_callback_register(void (*callback)(void))
{
    ili9488_dbi_async_callback = callback;
}

#if !defined(CONFIG_FPGA)
static void ili9488_dbi_realchip_init_gpio(void)
{
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[4];
    uint8_t num;
    uint8_t ucFuncType;

    printf("ili9488_dbi_realchip_init_gpio bf 0x%08x = 0x%08x \r\n", 0x200008d4, *(unsigned int *)0x200008d4);
    printf("ili9488_dbi_realchip_init_gpio bf 0x%08x = 0x%08x \r\n", 0x200008d8, *(unsigned int *)0x200008d8);
    printf("ili9488_dbi_realchip_init_gpio bf 0x%08x = 0x%08x \r\n", 0x200008dc, *(unsigned int *)0x200008dc);
    printf("ili9488_dbi_realchip_init_gpio bf 0x%08x = 0x%08x \r\n", 0x200008e0, *(unsigned int *)0x200008e0);

    //memset(&gpiopins[0], 0, sizeof(gpiopins));

    gpiopins[ 0] = ILI9488_DBI_WR_PIN;
    gpiopins[ 1] = ILI9488_DBI_CS_PIN;
    gpiopins[ 2] = ILI9488_DBI_RD_PIN;
    gpiopins[ 3] = ILI9488_DBI_DC_PIN;
    num = 4;
    
    #if (ILI9488_DBI_INTERFACE == ILI9488_DBI_TYPE_B)
        ucFuncType = GPIO_FUN_DBI_B;
    #else
        ucFuncType = GPIO_FUN_DBI_C;
    #endif

    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.outputMode = 0;
    cfg.gpioFun = ucFuncType;

    for (uint8_t i = 0; i < num; i++) {
        cfg.gpioPin = gpiopins[i];
        cfg.gpioMode = GPIO_MODE_AF;
        GLB_GPIO_Init(&cfg);
    }

    printf("ili9488_dbi_realchip_init_gpio af 0x%08x = 0x%08x \r\n", 0x200008d4, *(unsigned int *)0x200008d4);
    printf("ili9488_dbi_realchip_init_gpio af 0x%08x = 0x%08x \r\n", 0x200008d8, *(unsigned int *)0x200008d8);
    printf("ili9488_dbi_realchip_init_gpio af 0x%08x = 0x%08x \r\n", 0x200008dc, *(unsigned int *)0x200008dc);
    printf("ili9488_dbi_realchip_init_gpio af 0x%08x = 0x%08x \r\n", 0x200008e0, *(unsigned int *)0x200008e0);


     *(unsigned int *)0x200008d4=0x401742;
     *(unsigned int *)0x200008d8=0x401742;
     *(unsigned int *)0x200008dc=0x401742;
     *(unsigned int *)0x200008e0=0x401742;

    printf("ili9488_dbi_realchip_init_gpio manual set 0x%08x = 0x%08x \r\n", 0x200008d4, *(unsigned int *)0x200008d4);
    printf("ili9488_dbi_realchip_init_gpio manual set 0x%08x = 0x%08x \r\n", 0x200008d8, *(unsigned int *)0x200008d8);
    printf("ili9488_dbi_realchip_init_gpio manual set 0x%08x = 0x%08x \r\n", 0x200008dc, *(unsigned int *)0x200008dc);
    printf("ili9488_dbi_realchip_init_gpio manual set 0x%08x = 0x%08x \r\n", 0x200008e0, *(unsigned int *)0x200008e0);

}
#endif

/**
 * @brief ili9488_dbi_peripheral_init
 *
 * @return int  0:succes  1:error
 */
static int ili9488_dbi_peripheral_init(void)
{
#if !defined(CONFIG_FPGA)
    ili9488_dbi_realchip_init_gpio();
#endif
#if (ILI9488_DBI_INTERFACE == ILI9488_DBI_TYPE_C_3_WIRE)
    period_ls.startLen      = 0;/*2;*/
    period_ls.dataPhase0Len = 0;/*2;*/
    period_ls.dataPhase1Len = 0;/*2;*/
    period_ls.intervalLen   = 0;/*2;*/
    period_hs.startLen      = 0;/*1;*/
    period_hs.dataPhase0Len = 0;/*1;*/
    period_hs.dataPhase1Len = 0;/*1;*/
    period_hs.intervalLen   = 0;/*1;*/

    dbi_cfg.mode = DBI_TYPE_C_3_WIRE;
    dbi_cfg.continueEn = ENABLE; /* Enable:CS will stay asserted between each consecutive pixel, disable:CS will de-assert between each pixel */
    dbi_cfg.dummyEn = DISABLE;   /* Enable:dummy cycle will be inserted between command phase adn data phase, disable:no dummy cycle */
    dbi_cfg.dummyCnt = 0;        /* Dummy cycle count,effective only in type C(fixed to 1 in type B) */
    dbi_cfg.clkPhase = 0;        /* DBI clock phase */
    dbi_cfg.clkPolarity = 0;     /* DBI clock polarity */
    //dbi_cfg.period = period_ls;  /* */
    dbi_cfg.period.startLen        = period_hs.startLen     ;
    dbi_cfg.period.dataPhase0Len   = period_hs.dataPhase0Len;
    dbi_cfg.period.dataPhase1Len   = period_hs.dataPhase1Len;
    dbi_cfg.period.intervalLen     = period_hs.intervalLen  ;

#elif (ILI9488_DBI_INTERFACE == ILI9488_DBI_TYPE_C_4_WIRE)
    period_ls.startLen      = 0;/*1;*/ /*need to add ground connection between LCD module an FPGA to make display stable*/
    period_ls.dataPhase0Len = 0;/*1;*/ /*need to add ground connection between LCD module an FPGA to make display stable*/
    period_ls.dataPhase1Len = 0;/*1;*/ /*need to add ground connection between LCD module an FPGA to make display stable*/
    period_ls.intervalLen   = 0;/*1;*/ /*need to add ground connection between LCD module an FPGA to make display stable*/
    period_hs.startLen      = 0;/*1;*/ /*need to add ground connection between LCD module an FPGA to make display stable*/
    period_hs.dataPhase0Len = 0;/*1;*/ /*need to add ground connection between LCD module an FPGA to make display stable*/
    period_hs.dataPhase1Len = 0;/*1;*/ /*need to add ground connection between LCD module an FPGA to make display stable*/
    period_hs.intervalLen   = 0;/*1;*/ /*need to add ground connection between LCD module an FPGA to make display stable*/

    dbi_cfg.mode = DBI_TYPE_C_4_WIRE;
    dbi_cfg.continueEn = ENABLE;            /* Enable:CS will stay asserted between each consecutive pixel, disable:CS will de-assert between each pixel */
    dbi_cfg.dummyEn = DISABLE;              /* Enable:dummy cycle will be inserted between command phase adn data phase, disable:no dummy cycle */
    dbi_cfg.dummyCnt = 0;                   /* Dummy cycle count,effective only in type C(fixed to 1 in type B) */
    dbi_cfg.clkPhase = 0;                   /* DBI clock phase */
    dbi_cfg.clkPolarity = 0;                /* DBI clock polarity */
    //dbi_cfg.period = period_ls;  /* */
    dbi_cfg.period.startLen        = period_hs.startLen     ;
    dbi_cfg.period.dataPhase0Len   = period_hs.dataPhase0Len;
    dbi_cfg.period.dataPhase1Len   = period_hs.dataPhase1Len;
    dbi_cfg.period.intervalLen     = period_hs.intervalLen  ;
#elif (ILI9488_DBI_INTERFACE == ILI9488_DBI_TYPE_B)
    /*for running DBI type B on FPGA, */
    /*method 1. DC/WR/CS/RD need to connect to MM board 3.3v level shifter output and data pins all direct connect to 1.8v FPGA output*/
    /*method 2. DC/WR/CS/RD/data pins all connect to MM board 3.3v level shifter output and data pins must add serial 680ohm resister to make it work.*/
    period_ls.startLen      = 0;/*1;*/ /*4;*/
    period_ls.dataPhase0Len = 0;/*1;*/ /*4;*/
    period_ls.dataPhase1Len = 0;/*1;*/ /*4;*/
    period_ls.intervalLen   = 0;/*1;*/ /*4;*/
    period_hs.startLen      = 0;/*1;*/ /*2;*/
    period_hs.dataPhase0Len = 0;/*1;*/ /*2;*/
    period_hs.dataPhase1Len = 0;/*1;*/ /*2;*/
    period_hs.intervalLen   = 0;/*1;*/ /*2;*/


    dbi_cfg.mode = DBI_TYPE_B;
    dbi_cfg.continueEn = ENABLE; /* Enable:CS will stay asserted between each consecutive pixel, disable:CS will de-assert between each pixel */
    dbi_cfg.dummyEn = DISABLE;   /* Enable:dummy cycle will be inserted between command phase adn data phase, disable:no dummy cycle */
    dbi_cfg.dummyCnt = 0;        /* Dummy cycle count,effective only in type C(fixed to 1 in type B) */
    dbi_cfg.clkPhase = 0;        /* DBI clock phase */
    dbi_cfg.clkPolarity = 0;     /* DBI clock polarity */
    //dbi_cfg.period = period_ls;  /* */
    dbi_cfg.period.startLen        = period_hs.startLen     ;
    dbi_cfg.period.dataPhase0Len   = period_hs.dataPhase0Len;
    dbi_cfg.period.dataPhase1Len   = period_hs.dataPhase1Len;
    dbi_cfg.period.intervalLen     = period_hs.intervalLen  ;
#endif

    dbi_cfg.CSStretchMode = DBI_CS_STRETCHMODE_DISABLE;

#if (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB565)
    dbi_cfg.pixelFormat = DBI_PIXEL_RGB565;   /* DBI pixel format */
    dbi_cfg.fifoFormat = DBI_FIFO_565_RGBRGB; /* DBI fifo format */
    #if (ILI9488_DBI_FIFO_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB666)
        dbi_cfg.fifoFormat = DBI_FIFO_888_NRGB; /* DBI fifo format *//*abner test*/
    #endif
#elif (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB666)
    dbi_cfg.pixelFormat = DBI_PIXEL_RGB666; /* DBI pixel format */
    dbi_cfg.fifoFormat = DBI_FIFO_888_NRGB; /* DBI fifo format */
    //dbi_cfg.fifoFormat = DBI_FIFO_888_NBGR; /*abner test */
#endif


    DBI_Init(&dbi_cfg);
    DBI_SetTxFifoThreshold(3);

    /* DMA config */
    g_dma_dev.direction = DMA_MEMORY_TO_PERIPH;
    g_dma_dev.src_req = DMA_REQUEST_NONE;
    g_dma_dev.dst_req = DMA_REQUEST_DBI_TX;
    g_dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    g_dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    g_dma_dev.src_width = DMA_TRANSFER_WIDTH_32BIT;
    g_dma_dev.dst_width = DMA_TRANSFER_WIDTH_32BIT;
    dma_setConfig(&g_dma_dev);
    dma_init(g_dma_dev.id, g_dma_dev.ch);
    dma_ioctl(g_dma_dev.id, g_dma_dev.ch, DMA_CTRL_SET_INT, NULL);
    //L1C_DCACHE_CLEAN_RANGE((unsigned long)send_data, DBI_TEST_DATA_LEN);
    //dma_reload(g_dma_dev.id, g_dma_dev.ch, (uint32_t)send_data, (uint32_t)DMA_ADDR_DBI_TDR, DBI_TEST_DATA_LEN, NULL);
    //dma_channel_start(g_dma_dev.id, g_dma_dev.ch);

    return 0;
}

/**
 * @brief ili9488_dbi_draw_is_busy, After the call ili9488_dbi_draw_picture_dma must check this,
 *         if ili9488_dbi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int ili9488_dbi_draw_is_busy(void)
{
    if (dma_channel_check_busy(g_dma_dev.id, g_dma_dev.ch)) {
        return 1;
    } else {
        /* Wait transfer end */
        while (DBI_GetBusBusyStatus()) {
        }
        /* disable DBI-DMA request */
        DBI_SetDMA(DISABLE);
        /* clear interrupt and fifo*/
        DBI_IntClear();
        /* low speed mode */
        DBI_SetPeriod(&period_ls);
        return 0;
    }

}

/**
 * @brief ili9488_dbi_init
 *
 * @return int
 */
int ili9488_dbi_init()
{
    GLB_Set_DBI_CLK(ENABLE, GLB_DBI_CLK_MCU_MUXPLL_160M, 0);

    ili9488_dbi_peripheral_init();

    for (uint16_t i = 0; i < (sizeof(ili9488_dbi_init_cmds) / sizeof(ili9488_dbi_init_cmd_t)); i++) {
        if (ili9488_dbi_init_cmds[i].cmd == 0xFF) {
            arch_delay_ms(ili9488_dbi_init_cmds[i].databytes); /*bflb_platform_delay_ms(ili9488_dbi_init_cmds[i].databytes);*/
        } else {
            // DBI_SendCmdWithNormalData(ili9488_dbi_init_cmds[i].cmd, ili9488_dbi_init_cmds[i].databytes, (uint8_t *)ili9488_dbi_init_cmds[i].data);
            DBI_SetPhaseState(ENABLE, DISABLE);
            DBI_SendCmdWithNormalData(ili9488_dbi_init_cmds[i].cmd, 0, NULL);
            DBI_SetPhaseState(DISABLE, ENABLE);
            for (uint8_t j = 0; j < ili9488_dbi_init_cmds[i].databytes; j++) {
                DBI_SendCmdWithNormalData(0xFF, 1, (uint8_t *)&ili9488_dbi_init_cmds[i].data[j]);
            }
        }
    }
    DBI_SetPhaseState(ENABLE, ENABLE);
    return 0;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int ili9488_dbi_set_dir(uint8_t dir, uint8_t mir_flag)
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
    DBI_SendCmdWithNormalData(0x36, 1, &param);
    return dir;
}

/**
 * @brief ili9488_dbi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void ili9488_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if ILI9488_DBI_OFFSET_X
    x1 += ILI9488_DBI_OFFSET_X;
    x2 += ILI9488_DBI_OFFSET_X;
#endif
#if ILI9488_DBI_OFFSET_Y
    y1 += ILI9488_DBI_OFFSET_Y;
    y2 += ILI9488_DBI_OFFSET_Y;
#endif

    uint8_t param[4];

    param[0] = (y1 >> 8) & 0xFF;
    param[1] = y1 & 0xFF;
    param[2] = (y2 >> 8) & 0xFF;
    param[3] = y2 & 0xFF;
    DBI_SetPhaseState(ENABLE, ENABLE);
    DBI_SendCmdWithNormalData(0x2B, 4, param);

    param[0] = (x1 >> 8) & 0xFF;
    param[1] = x1 & 0xFF;
    param[2] = (x2 >> 8) & 0xFF;
    param[3] = x2 & 0xFF;
    DBI_SendCmdWithNormalData(0x2A, 4, param);
}

/**
 * @brief ili9488_dbi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void ili9488_dbi_draw_point(uint16_t x, uint16_t y, ili9488_dbi_color_t color)
{
    /* set window */
    ili9488_dbi_set_draw_window(x, y, x, y);
    /* set command */
    DBI_SetCommand(0x2C);
    /* send data of pixel */
    DBI_SetPhaseState(ENABLE, ENABLE);
    DBI_SendCmdWithPixelData(0x2C, 1, (uint32_t *)(&color));
}

/**
 * @brief ili9488_dbi_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void ili9488_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_dbi_color_t color)
{
    uint32_t pixelDataSize, PixelCount = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;
    dma_trans_param_t tdmaTransParam;

    ili9488_dbi_set_draw_window(x1, y1, x2, y2);

    /* disable transfer */
    DBI_Disable();

    /* high speed mode */
    DBI_SetPeriod(&period_hs);

    /* clear fifo and interrupt */
    DBI_TxFifoClear();
    DBI_IntClear();

    /* set command */
    DBI_SetCommand(0x2C);

    /* Data type select, and set data count */
    DBI_SetPhaseState(ENABLE, ENABLE);
    DBI_SetDataTypeAndCount(DBI_DATA_PIXEL, PixelCount);

    /* get pixel Data Size */
//#if (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB565)
#if (ILI9488_DBI_FIFO_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB565)  /**abner test*/
    pixelDataSize = ((PixelCount + 1) >> 1) << 2;
    color_src = color << 16 | color;
//#elif (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB666)
#elif (ILI9488_DBI_FIFO_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB666)  /**abner test*/
    pixelDataSize = PixelCount << 2;
    color_src = color;
#endif

    /* sync mode, temporary shutdown interrupt */
    ili9488_dbi_sync_flush_flag = true;

    /* DMA config */
    dma_close(g_dma_dev.id, g_dma_dev.ch);
    g_dma_dev.direction = DMA_MEMORY_TO_PERIPH;
    g_dma_dev.src_req = DMA_REQUEST_NONE;
    g_dma_dev.dst_req = DMA_REQUEST_DBI_TX;
    g_dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;    /*draw area source is the same*/
    g_dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    g_dma_dev.src_width = DMA_TRANSFER_WIDTH_32BIT;
    g_dma_dev.dst_width = DMA_TRANSFER_WIDTH_32BIT;
    dma_setConfig(&g_dma_dev);
    dma_init(g_dma_dev.id, g_dma_dev.ch);
    dma_ioctl(g_dma_dev.id, g_dma_dev.ch, DMA_CTRL_SET_INT, NULL);

    L1C_DCACHE_CLEAN_RANGE((unsigned long)&color_src, sizeof(color_src));
    dma_reload(g_dma_dev.id, g_dma_dev.ch, (uint32_t)&color_src, (uint32_t)DMA_ADDR_DBI_TDR, pixelDataSize, NULL);


    /* enable DBI-DMA request */
    DBI_SetDMA(ENABLE);

    /* Enable DMA */
    dma_channel_start(g_dma_dev.id, g_dma_dev.ch);

    /* Enable DBI transfer */
    DBI_Enable();

    while (ili9488_dbi_draw_is_busy()) {
    }

    /* enable src_addr_inc */
    tdmaTransParam.map          = DMA_PARAM_UPDATE_SRC_ADDR_INC;
    tdmaTransParam.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    dma_ioctl(g_dma_dev.id, g_dma_dev.ch, DMA_CTRL_CHANNEL_TRANS_PARAM_UPDATE, (void*)&tdmaTransParam);
    dma_close(g_dma_dev.id, g_dma_dev.ch);
}

/**
 * @brief ili9488_dbi_draw_area_nonblockinng
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void ili9488_dbi_draw_area_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_dbi_color_t color)
{
    uint32_t pixelDataSize, PixelCount = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;
    //dma_trans_param_t tdmaTransParam;

    ili9488_dbi_set_draw_window(x1, y1, x2, y2);

    /* disable transfer */
    DBI_Disable();

    /* high speed mode */
    DBI_SetPeriod(&period_hs);

    /* clear fifo and interrupt */
    DBI_TxFifoClear();
    DBI_IntClear();

    /* set command */
    DBI_SetCommand(0x2C);

    /* Data type select, and set data count */
    DBI_SetPhaseState(ENABLE, ENABLE);
    DBI_SetDataTypeAndCount(DBI_DATA_PIXEL, PixelCount);

    /* get pixel Data Size */
//#if (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB565)
#if (ILI9488_DBI_FIFO_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB565)  /**abner test*/
    pixelDataSize = ((PixelCount + 1) >> 1) << 2;
    color_src = color << 16 | color;
//#elif (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB666)
#elif (ILI9488_DBI_FIFO_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB666)  /**abner test*/
    pixelDataSize = PixelCount << 2;
    color_src = color;
#endif

    ///* sync mode, temporary shutdown interrupt */
    //ili9488_dbi_sync_flush_flag = true;

    /* DMA config */
    dma_close(g_dma_dev.id, g_dma_dev.ch);  /*free g_ptlliCfg to avoid out of memory issue*/
    g_dma_dev.direction = DMA_MEMORY_TO_PERIPH;
    g_dma_dev.src_req = DMA_REQUEST_NONE;
    g_dma_dev.dst_req = DMA_REQUEST_DBI_TX;
    g_dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;    /*draw area source is the same*/
    g_dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    g_dma_dev.src_width = DMA_TRANSFER_WIDTH_32BIT;
    g_dma_dev.dst_width = DMA_TRANSFER_WIDTH_32BIT;
    dma_setConfig(&g_dma_dev);
    dma_init(g_dma_dev.id, g_dma_dev.ch);
    dma_ioctl(g_dma_dev.id, g_dma_dev.ch, DMA_CTRL_SET_INT, NULL);

    L1C_DCACHE_CLEAN_RANGE((unsigned long)&color_src, sizeof(color_src));
    dma_reload(g_dma_dev.id, g_dma_dev.ch, (uint32_t)&color_src, (uint32_t)DMA_ADDR_DBI_TDR, pixelDataSize, NULL);


    /* enable DBI-DMA request */
    DBI_SetDMA(ENABLE);

    /* Enable DMA */
    dma_channel_start(g_dma_dev.id, g_dma_dev.ch);

    /* Enable DBI transfer */
    DBI_Enable();

    //while (ili9488_dbi_draw_is_busy()) {
    //}


    /* enable src_addr_inc */
    //tdmaTransParam.map          = DMA_PARAM_UPDATE_SRC_ADDR_INC;
    //tdmaTransParam.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    //dma_ioctl(g_dma_dev.id, g_dma_dev.ch, DMA_CTRL_CHANNEL_TRANS_PARAM_UPDATE, (void*)&tdmaTransParam);
}

/**
 * @brief ili9488_dbi_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (ili9488_dbi_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */

void ili9488_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_dbi_color_t *picture)
{
    uint32_t pixelDataSize, PixelCount = (x2 - x1 + 1) * (y2 - y1 + 1);

    ili9488_dbi_set_draw_window(x1, y1, x2, y2);

    /* disable transfer */
    DBI_Disable();

    /* high speed mode */
    DBI_SetPeriod(&period_hs);

    /* clear fifo and interrupt */
    DBI_TxFifoClear();
    DBI_IntClear();

    /* set command */
    DBI_SetCommand(0x2C);

    /* Data type select, and set data count */
    DBI_SetPhaseState(ENABLE, ENABLE);
    DBI_SetDataTypeAndCount(DBI_DATA_PIXEL, PixelCount);

    /* DMA config */
//#if (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB565)
#if (ILI9488_DBI_FIFO_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB565)  /**abner test*/

    pixelDataSize = ((PixelCount + 1) >> 1) << 2;
//#elif (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB666)
#elif (ILI9488_DBI_FIFO_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB666)  /**abner test*/
    pixelDataSize = PixelCount << 2;
#endif

    /* DMA config */
    dma_close(g_dma_dev.id, g_dma_dev.ch);  /*free g_ptlliCfg to avoid out of memory issue*/
    g_dma_dev.direction = DMA_MEMORY_TO_PERIPH;
    g_dma_dev.src_req = DMA_REQUEST_NONE;
    g_dma_dev.dst_req = DMA_REQUEST_DBI_TX;
    g_dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    g_dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    g_dma_dev.src_width = DMA_TRANSFER_WIDTH_32BIT;
    g_dma_dev.dst_width = DMA_TRANSFER_WIDTH_32BIT;
    dma_setConfig(&g_dma_dev);
    dma_init(g_dma_dev.id, g_dma_dev.ch);
    dma_ioctl(g_dma_dev.id, g_dma_dev.ch, DMA_CTRL_SET_INT, NULL);

    L1C_DCACHE_CLEAN_RANGE((void *)picture, pixelDataSize);
    dma_reload(g_dma_dev.id, g_dma_dev.ch, (uint32_t)picture, (uint32_t)DMA_ADDR_DBI_TDR, pixelDataSize, NULL);

    /* enable DBI-DMA request */
    DBI_SetDMA(ENABLE);

    /* Enable DMA */
    dma_channel_start(g_dma_dev.id, g_dma_dev.ch);

    /* Enable DBI transfer */
    DBI_Enable();
}

/**
 * @brief ili9488_dbi_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void ili9488_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_dbi_color_t *picture)
{
#if 0
    /* set window */
    ili9488_dbi_set_draw_window( x1,  y1,  x2,  y2);
    /* set command */
    DBI_SetCommand(0x2C);
    /* high speed mode */
    DBI_SetPeriod(&period_hs);
    /* send data of pixel */
    DBI_SetPhaseState(ENABLE, ENABLE);
    DBI_SendCmdWithPixelData(0x2C, (x2- x1 + 1) * (y2 - y1 + 1), (uint32_t *)picture);
    /* low speed mode */
    DBI_SetPeriod(&period_ls);
#else
    /* sync mode, temporary shutdown interrupt */
    ili9488_dbi_sync_flush_flag = true;

    ili9488_dbi_draw_picture_nonblocking(x1, y1, x2, y2, picture);
    while (ili9488_dbi_draw_is_busy()) {
    }
#endif
}

#elif defined(LCD_DBI_ILI9488)
#error "Devices that do not support DBI! Replace the driver port (lcd.h)"
#endif
