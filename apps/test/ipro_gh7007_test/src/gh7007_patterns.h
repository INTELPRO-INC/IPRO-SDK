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
 * @file gh7007_patterns.h
 * @brief Test patterns for GH7007 bring-up — drawn into the GRAM input plane
 *        (the scaler upscales to the native 1024x600 panel).
 */
#ifndef GH7007_PATTERNS_H
#define GH7007_PATTERNS_H

#include <stdint.h>

/** @brief 8 vertical colour bars: white, yellow, cyan, green, magenta, red,
 *         blue, black (classic SMPTE-ish order) across the full width. */
void gh7007_pattern_bars(void);

/* geometry ruler — 50/100-px marker grid over the full 512x320 probe plane */
void gh7007_pattern_ruler(void);

/* per-channel bit-weight probe (run with `lcd bitrev 0`) */
void gh7007_pattern_bitprobe(void);

/** @brief Checkerboard of @p sq-pixel squares (sq>=1), white/black. */
void gh7007_pattern_checker(uint16_t sq);

/** @brief Horizontal R/G/B + grayscale gradient bands (4 vertical quarters). */
void gh7007_pattern_gradient(void);

/** @brief 1px-frame border + crosshair, to verify the full plane is addressed
 *         and the scaler mapping reaches every edge. */
void gh7007_pattern_frame(uint16_t color);

/** @brief Stream a detailed, per-pixel "real image" (gradient + grid + moving
 *         diagonal) for @p frames frames back-to-back — the same path the vendor
 *         sample's LCD_ShowPicture() uses (0x2C first row / 0x3C rest). frames>1
 *         animates it like the sample's while(1) loop. */
void gh7007_pattern_image(int frames);

#endif /* GH7007_PATTERNS_H */
