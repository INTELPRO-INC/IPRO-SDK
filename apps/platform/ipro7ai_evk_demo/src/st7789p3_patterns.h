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
 * @file st7789p3_patterns.h
 * @brief Test patterns for ST7789P3 bring-up.
 *
 * All of these draw into the current orientation's full visible area, so they
 * follow `lcd dir` automatically.
 */
#ifndef ST7789P3_PATTERNS_H
#define ST7789P3_PATTERNS_H

#include <stdint.h>

/** @brief 8 vertical colour bars: white, yellow, cyan, green, magenta, red,
 *         blue, black. The classic first-light check. */
void st7789p3_pattern_bars(void);

/**
 * @brief Three large primary blocks (red / green / blue) over a white and a
 *        black reference strip.
 *
 * This is the RGB-vs-BGR discriminator: the blocks are drawn top-to-bottom in
 * R, G, B order. If the top block looks blue and the bottom one red, the panel
 * wants MADCTL bit 3 set - run `lcd bgr on`.
 */
void st7789p3_pattern_rgbtest(void);

/** @brief Checkerboard of sq-pixel squares (sq >= 1), white on black. Reveals
 *         clock/setup problems as smeared or doubled columns. */
void st7789p3_pattern_checker(uint16_t sq);

/** @brief Four horizontal bands: red, green, blue and grey ramps left to right.
 *         Banding or clipping here points at gamma or VRHS/VCOMS drive. */
void st7789p3_pattern_gradient(void);

/** @brief 32-step grey staircase, black to white. */
void st7789p3_pattern_grayramp(void);

/** @brief 1 px border, centre crosshair and corner ticks - proves every edge
 *         of the addressable area is reached and nothing is offset. */
void st7789p3_pattern_frame(uint16_t color);

/** @brief Marker grid: white lines every 100 px, red every 50, with ticks down
 *         the left edge. Used to measure any addressing offset. */
void st7789p3_pattern_ruler(void);

#endif /* ST7789P3_PATTERNS_H */
