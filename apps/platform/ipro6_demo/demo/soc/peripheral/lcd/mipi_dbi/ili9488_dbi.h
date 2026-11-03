/**
 * @file ili9488_dpi.h
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

#ifndef _ILI9488_DBI_H_
#define _ILI9488_DBI_H_
#pragma message "_ILI9488_DBI_H_  Definded!!!"

#include "platform_common.h"
#include "drv_gpio.h"
#include "hal_gpio.h"

#define ILI9488_DBI_WR_PIN          GLB_GPIO_PIN_4
#define ILI9488_DBI_CS_PIN          GLB_GPIO_PIN_5
#define ILI9488_DBI_RD_PIN          GLB_GPIO_PIN_6
#define ILI9488_DBI_DC_PIN          GLB_GPIO_PIN_7


/* Optional MIPI interface type */
#define ILI9488_DBI_TYPE_C_4_WIRE 1
#define ILI9488_DBI_TYPE_C_3_WIRE 2
#define ILI9488_DBI_TYPE_B        3

/* Optional pixel data format type */
#define ILI9488_DBI_PIXEL_FORMAT_RGB666 666 /* Equivalent to RGB888 */
#define ILI9488_DBI_PIXEL_FORMAT_RGB565 565

#define ILI9488_DBI_ROTATE_FOR_480x320 0    /*1: rotate for synchronizing sensor orintation. Not rotate for draw_area API fill whole screen.*/
//#define ENABLE_INVERSION_FOR_IPS_LCD_PANNEL (1) /*define to enable inversion for IPS LCD Pannel*/


/* Selecting an interface type*/
#if 0
    #define ILI9488_DBI_INTERFACE  ILI9488_DBI_TYPE_B
#else
    #define ILI9488_DBI_INTERFACE  ILI9488_DBI_TYPE_C_4_WIRE
#endif

#if (ILI9488_DBI_INTERFACE == ILI9488_DBI_TYPE_B)
/* Select pixel format */
#define ILI9488_DBI_PIXEL_FORMAT ILI9488_DBI_PIXEL_FORMAT_RGB565
#else
/* Do not modify, ili9488-TypeC mode Only support RGB666 */
#define ILI9488_DBI_PIXEL_FORMAT ILI9488_DBI_PIXEL_FORMAT_RGB666
#endif

#if 1
#define ILI9488_DBI_FIFO_PIXEL_FORMAT ILI9488_DBI_PIXEL_FORMAT_RGB666   /*abner test ==> always use NRGB image for fifo format*/
#else
#define ILI9488_DBI_FIFO_PIXEL_FORMAT ILI9488_DBI_PIXEL_FORMAT_RGB565
#endif

/* ILI9488 LCD width */
#define ILI9488_DBI_W 320
/* ILI9488 LCD height */
#define ILI9488_DBI_H 480

/* The offset of the area can be displayed */
#define ILI9488_DBI_OFFSET_X 0
#define ILI9488_DBI_OFFSET_Y 0

#if (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB565)
#define ILI9488_DBI_COLOR_DEPTH 16
typedef uint16_t ili9488_dbi_color_t;
#elif (ILI9488_DBI_PIXEL_FORMAT == ILI9488_DBI_PIXEL_FORMAT_RGB666)
#define ILI9488_DBI_COLOR_DEPTH 32
typedef uint32_t ili9488_dbi_color_t;
#endif

typedef struct {
    uint8_t cmd; /* 0xFF : delay(databytes)ms */
    const char *data;
    uint8_t databytes; /* Num of data in data; or delay time */
} ili9488_dbi_init_cmd_t;

int ili9488_dbi_init();
void ili9488_dbi_async_callback_register(void (*callback)(void));
int ili9488_dbi_set_dir(uint8_t dir, uint8_t mir_flag);
void ili9488_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ili9488_dbi_draw_point(uint16_t x, uint16_t y, ili9488_dbi_color_t color);
void ili9488_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_dbi_color_t color);
void ili9488_dbi_draw_area_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_dbi_color_t color);
void ili9488_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_dbi_color_t *picture);
void ili9488_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_dbi_color_t *picture);
int ili9488_dbi_draw_is_busy(void);

#endif
