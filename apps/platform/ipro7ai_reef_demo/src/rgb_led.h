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
 * @file rgb_led.h
 * @brief WS2812-class RGB LED (LED1) on GPIO24, driven through SPI0.
 *
 * One SPI byte carries two WS2812 bits: each bit is four SPI bits at
 * 3.2 MHz (312.5 ns), "1000" for a 0 and "1100" for a 1, which gives
 * T0H 312 / T0L 937 ns and T1H 625 / T1L 625 ns - inside the WS2812B window
 * (T0H 220-380, T1H 580-1000, TxL >= 580 ns). GPIO24 % 4 == 0 is SPI0's MOSI
 * position with the MOSI/MISO swap OFF (pin_swap_enable = 0; the board default
 * swap 1 makes it MISO), so only that one pad is muxed; SCLK and CS stay as
 * plain GPIOs.
 *
 * The LED's supply is VBUS minus one diode (~4.3 V), which is what lets a
 * 3.3 V data line clear its 0.7 x VDD input threshold.
 */
#ifndef RGB_LED_H
#define RGB_LED_H

#include <stdint.h>

/** Mux GPIO24 to SPI0 and set the bit clock. Safe to call more than once. */
int rgb_led_init(void);

/** Latch one colour into the LED (8-bit per channel). */
int rgb_led_set(uint8_t r, uint8_t g, uint8_t b);

/** Turn the LED off and hand GPIO24 back to plain GPIO, driven low. */
void rgb_led_off(void);

/** Last colour written, for `led status`. */
void rgb_led_get(uint8_t *r, uint8_t *g, uint8_t *b);

#endif /* RGB_LED_H */
