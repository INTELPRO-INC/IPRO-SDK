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
 * @file gh7007_patterns.c
 * @brief Test patterns for GH7007 bring-up. Each builds one row at a time and
 *        streams it, so no full framebuffer is needed.
 */

#include "gh7007_patterns.h"
#include "gh7007.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

/* Scratch row, reused per scanline. Sized for the widest input plane. */
static uint16_t s_row[GH_IN_WIDTH];

/* Geometry ruler: writes the FULL probe plane (GH_IN_WIDTH x GH_IN_HEIGHT =
 * 512x320 — beyond the chip's real plane the extra is discarded/wrapped;
 * markers inside the real plane land true). Vertical lines: WHITE at every
 * multiple of 100 (3 px wide), RED at odd multiples of 50 (2 px). Left-edge
 * ticks every 50 rows: WHITE at multiples of 100, RED otherwise. Photograph
 * once: the rightmost visible line and its distance to the right bezel give
 * the real plane WIDTH; the lowest tick gives the HEIGHT. */
void gh7007_pattern_ruler(void)
{
    const uint16_t W = GH_IN_WIDTH, H = GH_IN_HEIGHT;

    gh7007_blit_start(0, 0, (uint16_t)(W - 1), (uint16_t)(H - 1));
    /* century lines are COLOR-CODED so a single photo reads out absolute
     * coordinates unambiguously (mirror/offset-proof):
     *   x=0 WHITE (double stripe = origin), 100 YELLOW, 200 CYAN,
     *   300 GREEN, 400 MAGENTA, 500 WHITE; halves (50/150/...) RED. */
    static const uint16_t century[6] = {
        GH_WHITE, GH_YELLOW, GH_CYAN, GH_GREEN, GH_MAGENTA, GH_WHITE,
    };
    for (uint16_t r = 0; r < H; r++) {
        for (uint16_t x = 0; x < W; x++) {
            if ((x % 100) < 3) {
                s_row[x] = century[x / 100];
            } else if (x >= 6 && x <= 8) {
                s_row[x] = GH_WHITE;             /* origin double-stripe */
            } else if ((x % 50) < 2) {
                s_row[x] = GH_RED;
            } else {
                s_row[x] = GH_BLACK;
            }
        }
        /* row ticks, same color code per century of rows */
        if ((r % 50) < 3) {
            uint16_t c = ((r % 100) < 3) ? century[r / 100] : GH_RED;
            for (uint16_t x = 0; x < 30; x++) {
                s_row[x] = c;
            }
        }
        gh7007_blit_push_row(s_row, W);
        if ((r & 0x0F) == 0x0F) {
            vTaskDelay(1);
        }
    }
    gh7007_blit_end();
}

/* Bit-weight probe: three bands (R / G / B). Each band is split into 5 (or 6)
 * segments; segment k lights ONLY bit k of that channel (leftmost = our MSB,
 * rightmost = our LSB). Run with `lcd bitrev 0` so the chip's raw bit weights
 * are exposed; the user reports each band's brightness ORDER left-to-right,
 * which IS the chip's channel bit-permutation table. */
void gh7007_pattern_bitprobe(void)
{
    uint16_t w = gh7007_width();
    uint16_t h = gh7007_height();
    uint16_t band = (uint16_t)(h / 3);

    gh7007_blit_start(0, 0, (uint16_t)(w - 1), (uint16_t)(h - 1));
    for (uint16_t r = 0; r < h; r++) {
        int ch = (r < band) ? 0 : (r < 2 * band) ? 1 : 2;   /* R, G, B */
        int nbits = (ch == 1) ? 6 : 5;
        for (uint16_t x = 0; x < w; x++) {
            int seg = (x * nbits) / w;                       /* 0..nbits-1 */
            int bit = (nbits - 1) - seg;                     /* left = MSB  */
            uint16_t c;
            if (ch == 0) {
                c = (uint16_t)(1u << (11 + bit));
            } else if (ch == 1) {
                c = (uint16_t)(1u << (5 + bit));
            } else {
                c = (uint16_t)(1u << bit);
            }
            /* thin black separators between segments */
            if ((x * nbits) % w < (uint16_t)nbits) {
                c = GH_BLACK;
            }
            s_row[x] = c;
        }
        gh7007_blit_push_row(s_row, w);
        if ((r & 0x0F) == 0x0F) {
            vTaskDelay(1);
        }
    }
    gh7007_blit_end();
}

void gh7007_pattern_bars(void)
{
    static const uint16_t bars[8] = {
        GH_WHITE, GH_YELLOW, GH_CYAN, GH_GREEN,
        GH_MAGENTA, GH_RED, GH_BLUE, GH_BLACK,
    };
    uint16_t w = gh7007_width();
    uint16_t h = gh7007_height();

    for (uint16_t x = 0; x < w; x++) {
        s_row[x] = bars[(x * 8) / w];
    }
    gh7007_blit_start(0, 0, (uint16_t)(w - 1), (uint16_t)(h - 1));
    for (uint16_t y = 0; y < h; y++) {
        gh7007_blit_push_row(s_row, w);
    }
    gh7007_blit_end();
}

void gh7007_pattern_checker(uint16_t sq)
{
    if (sq == 0) {
        sq = 16;
    }
    uint16_t w = gh7007_width();
    uint16_t h = gh7007_height();

    gh7007_blit_start(0, 0, (uint16_t)(w - 1), (uint16_t)(h - 1));
    for (uint16_t y = 0; y < h; y++) {
        uint16_t yband = (uint16_t)((y / sq) & 1u);
        for (uint16_t x = 0; x < w; x++) {
            uint16_t xband = (uint16_t)((x / sq) & 1u);
            s_row[x] = (xband ^ yband) ? GH_WHITE : GH_BLACK;
        }
        gh7007_blit_push_row(s_row, w);
    }
    gh7007_blit_end();
}

void gh7007_pattern_gradient(void)
{
    uint16_t w = gh7007_width();
    uint16_t h = gh7007_height();
    uint16_t band = (uint16_t)(h / 4);
    if (band == 0) {
        band = 1;
    }

    gh7007_blit_start(0, 0, (uint16_t)(w - 1), (uint16_t)(h - 1));
    for (uint16_t y = 0; y < h; y++) {
        uint16_t sel = (uint16_t)(y / band);     /* 0=R 1=G 2=B 3=gray */
        for (uint16_t x = 0; x < w; x++) {
            uint8_t v = (uint8_t)((x * 255) / (w ? w : 1));
            switch (sel) {
            case 0:  s_row[x] = GH_RGB565(v, 0, 0); break;
            case 1:  s_row[x] = GH_RGB565(0, v, 0); break;
            case 2:  s_row[x] = GH_RGB565(0, 0, v); break;
            default: s_row[x] = GH_RGB565(v, v, v); break;
            }
        }
        gh7007_blit_push_row(s_row, w);
    }
    gh7007_blit_end();
}

void gh7007_pattern_frame(uint16_t color)
{
    uint16_t w = gh7007_width();
    uint16_t h = gh7007_height();

    gh7007_blit_start(0, 0, (uint16_t)(w - 1), (uint16_t)(h - 1));
    for (uint16_t y = 0; y < h; y++) {
        /* background black */
        memset(s_row, 0, (size_t)w * sizeof(s_row[0]));
        /* top/bottom edge rows */
        if (y == 0 || y == h - 1) {
            for (uint16_t x = 0; x < w; x++) {
                s_row[x] = color;
            }
        } else {
            s_row[0]     = color;        /* left edge   */
            s_row[w - 1] = color;        /* right edge  */
            s_row[w / 2] = color;        /* vertical crosshair */
            if (y == h / 2) {            /* horizontal crosshair */
                for (uint16_t x = 0; x < w; x++) {
                    s_row[x] = color;
                }
            }
        }
        gh7007_blit_push_row(s_row, w);
    }
    gh7007_blit_end();
}

void gh7007_pattern_image(int frames)
{
    uint16_t w = gh7007_width();
    uint16_t h = gh7007_height();

    if (frames < 1) {
        frames = 1;
    }
    for (int f = 0; f < frames; f++) {
        int phase = f * 4;               /* animates the diagonal + hue */
        gh7007_blit_start(0, 0, (uint16_t)(w - 1), (uint16_t)(h - 1));
        for (uint16_t y = 0; y < h; y++) {
            for (uint16_t x = 0; x < w; x++) {
                uint8_t r = (uint8_t)(((uint32_t)x * 255) / w);   /* red ramp L->R  */
                uint8_t g = (uint8_t)(((uint32_t)y * 255) / h);   /* green ramp T->B */
                uint8_t b = (uint8_t)((x + y + phase) & 0xFF);    /* blue diagonal   */
                if ((x % 32) == 0 || (y % 32) == 0) {
                    r = g = b = 0xFF;                             /* white 32px grid */
                } else if (((x + y + phase) % 64) < 4) {
                    r = 0xFF; g = 0x00; b = 0x00;                 /* moving red band */
                }
                s_row[x] = GH_RGB565(r, g, b);
            }
            gh7007_blit_push_row(s_row, w);
        }
        gh7007_blit_end();
        if ((f & 3) == 0) {
            vTaskDelay(1);   /* yield during a long animated run */
        }
    }
}
