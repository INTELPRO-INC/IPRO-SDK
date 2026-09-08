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
 * @file st7789p3_patterns.c
 * @brief Test patterns for ST7789P3 bring-up - see st7789p3_patterns.h.
 */

#include "st7789p3_patterns.h"
#include "st7789p3.h"

#include <FreeRTOS.h>
#include <task.h>

/* Row staging buffer, host byte order; swapped into wire order on push. */
static uint16_t s_row[ST_MAX_DIM];
static uint16_t s_wire[ST_MAX_DIM];

/* Rows between scheduler yields while streaming a full frame. */
#define PAT_YIELD_ROWS  16

/* Push s_row[0..w-1] as the next row of an open full-screen blit. */
static void pat_push_row(uint16_t w)
{
    for (uint16_t i = 0; i < w; i++) {
        s_wire[i] = st7789p3_swap(s_row[i]);
    }
    st7789p3_blit_push(s_wire, w);
}

/* Open a blit covering the whole visible area, holding the bus for the whole
 * frame so a concurrent drawer (camera preview, another shell command) cannot
 * interleave its pixels into our RAMWR window. */
static void pat_begin(uint16_t *w, uint16_t *h)
{
    st7789p3_bus_lock();
    *w = st7789p3_width();
    *h = st7789p3_height();
    st7789p3_blit_start(0, 0, (uint16_t)(*w - 1u), (uint16_t)(*h - 1u));
}

static void pat_end(void)
{
    st7789p3_blit_end();
    st7789p3_bus_unlock();
}

static void pat_maybe_yield(uint16_t row)
{
    if ((row % PAT_YIELD_ROWS) == (PAT_YIELD_ROWS - 1u) &&
        xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        taskYIELD();
    }
}

void st7789p3_pattern_bars(void)
{
    static const uint16_t bar[8] = {
        ST_WHITE, ST_YELLOW, ST_CYAN, ST_GREEN,
        ST_MAGENTA, ST_RED, ST_BLUE, ST_BLACK
    };
    uint16_t w, h;

    pat_begin(&w, &h);
    for (uint16_t x = 0; x < w; x++) {
        uint16_t idx = (uint16_t)((uint32_t)x * 8u / w);
        s_row[x] = bar[idx > 7u ? 7u : idx];
    }
    for (uint16_t y = 0; y < h; y++) {
        pat_push_row(w);
        pat_maybe_yield(y);
    }
    pat_end();
}

void st7789p3_pattern_rgbtest(void)
{
    uint16_t w, h;
    uint16_t b0, b1, b2, b3;

    pat_begin(&w, &h);

    /* Five horizontal bands: red, green, blue, white, black. */
    b0 = (uint16_t)(h / 5u);
    b1 = (uint16_t)(2u * h / 5u);
    b2 = (uint16_t)(3u * h / 5u);
    b3 = (uint16_t)(4u * h / 5u);

    for (uint16_t y = 0; y < h; y++) {
        uint16_t c;
        if      (y < b0) c = ST_RED;
        else if (y < b1) c = ST_GREEN;
        else if (y < b2) c = ST_BLUE;
        else if (y < b3) c = ST_WHITE;
        else             c = ST_BLACK;

        for (uint16_t x = 0; x < w; x++) {
            s_row[x] = c;
        }
        pat_push_row(w);
        pat_maybe_yield(y);
    }
    pat_end();
}

void st7789p3_pattern_checker(uint16_t sq)
{
    uint16_t w, h;

    if (sq == 0u) {
        sq = 1u;
    }
    pat_begin(&w, &h);
    for (uint16_t y = 0; y < h; y++) {
        uint16_t yodd = (uint16_t)((y / sq) & 1u);
        for (uint16_t x = 0; x < w; x++) {
            uint16_t xodd = (uint16_t)((x / sq) & 1u);
            s_row[x] = (xodd ^ yodd) ? ST_WHITE : ST_BLACK;
        }
        pat_push_row(w);
        pat_maybe_yield(y);
    }
    pat_end();
}

void st7789p3_pattern_gradient(void)
{
    uint16_t w, h, band;

    pat_begin(&w, &h);
    band = (uint16_t)(h / 4u);
    if (band == 0u) {
        band = 1u;
    }

    for (uint16_t y = 0; y < h; y++) {
        uint16_t which = (uint16_t)(y / band);
        if (which > 3u) {
            which = 3u;
        }
        for (uint16_t x = 0; x < w; x++) {
            uint8_t v = (uint8_t)((uint32_t)x * 255u / (w ? (w - 1u) : 1u));
            switch (which) {
                case 0:  s_row[x] = ST_RGB565(v, 0, 0); break;
                case 1:  s_row[x] = ST_RGB565(0, v, 0); break;
                case 2:  s_row[x] = ST_RGB565(0, 0, v); break;
                default: s_row[x] = ST_RGB565(v, v, v); break;
            }
        }
        pat_push_row(w);
        pat_maybe_yield(y);
    }
    pat_end();
}

void st7789p3_pattern_grayramp(void)
{
    uint16_t w, h;

    pat_begin(&w, &h);
    for (uint16_t x = 0; x < w; x++) {
        uint32_t step = (uint32_t)x * 32u / (w ? w : 1u);
        uint8_t  v    = (uint8_t)((step * 255u) / 31u);
        s_row[x] = ST_RGB565(v, v, v);
    }
    for (uint16_t y = 0; y < h; y++) {
        pat_push_row(w);
        pat_maybe_yield(y);
    }
    pat_end();
}

void st7789p3_pattern_frame(uint16_t color)
{
    uint16_t w  = st7789p3_width();
    uint16_t h  = st7789p3_height();
    uint16_t cx = (uint16_t)(w / 2u);
    uint16_t cy = (uint16_t)(h / 2u);

    st7789p3_bus_lock();

    st7789p3_fill_screen(ST_BLACK);

    /* 1 px border on all four edges. */
    st7789p3_fill_rect(0, 0, (uint16_t)(w - 1u), 0, color);
    st7789p3_fill_rect(0, (uint16_t)(h - 1u), (uint16_t)(w - 1u),
                       (uint16_t)(h - 1u), color);
    st7789p3_fill_rect(0, 0, 0, (uint16_t)(h - 1u), color);
    st7789p3_fill_rect((uint16_t)(w - 1u), 0, (uint16_t)(w - 1u),
                       (uint16_t)(h - 1u), color);

    /* Centre crosshair. */
    st7789p3_fill_rect(0, cy, (uint16_t)(w - 1u), cy, color);
    st7789p3_fill_rect(cx, 0, cx, (uint16_t)(h - 1u), color);

    /* Solid 8x8 corner blocks - a missing one means that corner is not being
     * addressed (offset or truncated window). */
    st7789p3_fill_rect(0, 0, 7, 7, ST_RED);
    st7789p3_fill_rect((uint16_t)(w - 8u), 0, (uint16_t)(w - 1u), 7, ST_GREEN);
    st7789p3_fill_rect(0, (uint16_t)(h - 8u), 7, (uint16_t)(h - 1u), ST_BLUE);
    st7789p3_fill_rect((uint16_t)(w - 8u), (uint16_t)(h - 8u),
                       (uint16_t)(w - 1u), (uint16_t)(h - 1u), ST_YELLOW);
    st7789p3_bus_unlock();
}

void st7789p3_pattern_ruler(void)
{
    uint16_t w, h;

    pat_begin(&w, &h);
    for (uint16_t y = 0; y < h; y++) {
        for (uint16_t x = 0; x < w; x++) {
            uint16_t c = ST_BLACK;
            if ((x % 100u) == 0u) {
                c = ST_WHITE;
            } else if ((x % 50u) == 0u) {
                c = ST_RED;
            }
            /* Left-edge row ticks: 8 px long, white every 100, red every 50. */
            if (x < 8u) {
                if ((y % 100u) == 0u) {
                    c = ST_WHITE;
                } else if ((y % 50u) == 0u) {
                    c = ST_RED;
                }
            }
            s_row[x] = c;
        }
        pat_push_row(w);
        pat_maybe_yield(y);
    }
    pat_end();
}
