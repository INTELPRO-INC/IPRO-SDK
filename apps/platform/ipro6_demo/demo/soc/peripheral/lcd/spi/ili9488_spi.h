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

#ifndef _ILI9488_SPI_H_
#define _ILI9488_SPI_H_

#include "drv_gpio.h"
#include "hal_gpio.h"

#define ILI9488_SPI_CS_PIN          GLB_GPIO_PIN_10
#define ILI9488_SPI_DC_PIN          GLB_GPIO_PIN_17
#define ILI9488_SPI_SCLK_PIN        GLB_GPIO_PIN_15
//#define ILI9488_SPI_MISO_PIN        GLB_GPIO_PIN_17
#define ILI9488_SPI_MOSI_PIN        GLB_GPIO_PIN_16


#define ILI9488_SPI_CS_HIGH gpio_write(ILI9488_SPI_CS_PIN, 1)
#define ILI9488_SPI_CS_LOW  gpio_write(ILI9488_SPI_CS_PIN, 0)
#define ILI9488_SPI_DC_HIGH gpio_write(ILI9488_SPI_DC_PIN, 1)
#define ILI9488_SPI_DC_LOW  gpio_write(ILI9488_SPI_DC_PIN, 0)


/* ILI9488 LCD width */
#define ILI9488_SPI_W 320
/* ILI9488 LCD height */
#define ILI9488_SPI_H 480

/* The offset of the area can be displayed */
#define ILI9488_SPI_OFFSET_X 0
#define ILI9488_SPI_OFFSET_Y 0

#define ILI9488_SPI_COLOR_DEPTH 32

//#define ENABLE_INVERSION_FOR_IPS_LCD_PANNEL /*define to support IPS version LCD pannel*/
#define ILI9488_SPI_ROTATE_FOR_480x320 1    /*1: rotate for synchronizing sensor orintation.*/

typedef struct {
    uint8_t cmd; /* 0xFF : delay(databytes)ms */
    const char *data;
    uint8_t databytes; /* Num of data in data; or delay time */
} ili9488_spi_init_cmd_t;

typedef uint32_t ili9488_spi_color_t;

int ili9488_spi_init();
void ili9488_spi_async_callback_register(void (*callback)(void));
int ili9488_spi_set_dir(uint8_t dir, uint8_t mir_flag);
void ili9488_spi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ili9488_spi_draw_point(uint16_t x, uint16_t y, ili9488_spi_color_t color);
void ili9488_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_spi_color_t color);
void ili9488_spi_draw_area_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_spi_color_t color);
void ili9488_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_spi_color_t *picture);
void ili9488_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_spi_color_t *picture);
int ili9488_spi_draw_is_busy(void);

#endif
