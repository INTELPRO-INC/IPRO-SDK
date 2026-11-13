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
 * The IPRO7AI Sensor Tag board: what is on it besides the chip, and the pins.
 *
 *   IMU          ISM330BX, 3-wire SPI by GPIO   CLK 31, MISO 32, MOSI 33, CS 34
 *   Fuel gauge   CW2015, I2C0                   SDA 29, SCL 30
 *   Button       SW9, to ground                 GPIO 10 (pressed = low)
 *   LEDs         D2 red, D4 green, to 3V3       GPIO 26, GPIO 27 (on = low)
 *   Buzzer       passive, PWM0 channel 2        GPIO 2
 *   Console      the board's own USB (CDC)
 */
#ifndef TAG_BOARD_H
#define TAG_BOARD_H

#include <stdbool.h>
#include <stdint.h>

#define TAG_IMU_SCLK_GPIO   31
#define TAG_IMU_MISO_GPIO   32
#define TAG_IMU_MOSI_GPIO   33
#define TAG_IMU_CS_GPIO     34
#define TAG_BUTTON_GPIO     10
#define TAG_LED_RED_GPIO    26
#define TAG_LED_GREEN_GPIO  27
#define TAG_BUZZER_GPIO     2

/* Pins of the LEDs, the button and the buzzer. Call once, before the others. */
void tag_board_init(void);

void tag_led_red(bool on);
void tag_led_green(bool on);
bool tag_button_pressed(void);
/* 1 kHz tone on or off. */
void tag_buzzer(bool on);

#endif /* TAG_BOARD_H */
