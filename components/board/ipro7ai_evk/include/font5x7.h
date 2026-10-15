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
 * @file font5x7.h
 * @brief Minimal 5x7 bitmap font for the on-screen HUD: ASCII 0x20..0x5A
 *        (space, punctuation, digits, upper-case). Lower-case is folded to
 *        upper-case; anything else draws as a blank.
 *
 * Column-major, one byte per column, bit 0 = top row, bit 6 = bottom row -
 * the classic HD44780 / glcdfont layout.
 */
#ifndef FONT5X7_H
#define FONT5X7_H

#include <stdint.h>
#include <stdbool.h>

#define FONT_W       5
#define FONT_H       7
#define FONT_ADVANCE 6      /* glyph + 1 column gap */

/** @brief Five column bytes for @p ch (always a valid pointer). */
const uint8_t *font5x7_glyph(char ch);

/** @brief True if pixel (col,row) of @p ch is set. col 0..4, row 0..6. */
static inline bool font5x7_pixel(const uint8_t *glyph, int col, int row)
{
    return (glyph[col] >> row) & 1u;
}

#endif /* FONT5X7_H */
