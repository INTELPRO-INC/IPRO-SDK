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
 * Panel stub for the host tests: a 320x240 virtual GRAM that records what the
 * gfx helpers push, so text placement and row compositing can be checked
 * without a board.
 */
#include <string.h>

#include "st7789p3.h"
#include "st7789p3_stub.h"

uint16_t g_gram[ST_MAX_DIM][ST_MAX_DIM];         /* wire order, as pushed; landscape 320x240 in use */
int      g_lock_depth;
int      g_lock_errors;                         /* blit outside a lock */

static uint16_t s_w = 320, s_h = 240;
static uint16_t s_x0, s_y0, s_x1, s_y1, s_cx, s_cy;
static int      s_in_blit;

void stub_reset(void)
{
    memset(g_gram, 0xEE, sizeof(g_gram));      /* a value no test writes */
    g_lock_depth = g_lock_errors = 0;
    s_in_blit = 0;
}

uint16_t stub_gram(int x, int y)
{
    return g_gram[y][x];
}

uint16_t st7789p3_width(void)  { return s_w; }
uint16_t st7789p3_height(void) { return s_h; }
bool     st7789p3_is_ready(void) { return true; }

void st7789p3_bus_lock(void)   { g_lock_depth++; }
void st7789p3_bus_unlock(void) { g_lock_depth--; }

void st7789p3_blit_start(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    if (g_lock_depth <= 0) g_lock_errors++;
    s_x0 = x0; s_y0 = y0; s_x1 = x1; s_y1 = y1;
    s_cx = x0; s_cy = y0;
    s_in_blit = 1;
}

void st7789p3_blit_push(const uint16_t *pixels, uint16_t npix)
{
    if (!s_in_blit) g_lock_errors++;
    for (uint16_t i = 0; i < npix; i++) {
        if (s_cy <= s_y1 && s_cx <= s_x1) {
            g_gram[s_cy][s_cx] = pixels[i];
        }
        if (++s_cx > s_x1) {
            s_cx = s_x0;
            s_cy++;
        }
    }
}

void st7789p3_blit_end(void)
{
    s_in_blit = 0;
}

void st7789p3_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t c = st7789p3_swap(color);
    for (uint16_t y = y0; y <= y1 && y < s_h; y++)
        for (uint16_t x = x0; x <= x1 && x < s_w; x++)
            g_gram[y][x] = c;
}

void st7789p3_fill_screen(uint16_t color)
{
    st7789p3_fill_rect(0, 0, (uint16_t)(s_w - 1), (uint16_t)(s_h - 1), color);
}
