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
 * @file uvc_draw.c
 * @brief YUYV422 overlay drawing. See uvc_draw.h.
 *
 * Packed [Y0 U Y1 V]: two pixels share one U/V pair.
 */

#include "uvc_draw.h"

#include <stddef.h>
#include <string.h>

#include "font5x7.h"

static inline void put_px(uint8_t *f, int w, int x, int y, yuv_t c)
{
    int px = y * w + x;
    uint8_t *p = f + (px >> 1) * 4;

    if (px & 1) {
        p[2] = c.y;
        p[3] = c.v;
    } else {
        p[0] = c.y;
        p[1] = c.u;
    }
}

void ov_fill_rect(uint8_t *f, int w, int h, int x0, int y0, int x1, int y1, yuv_t c)
{
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > w) x1 = w;
    if (y1 > h) y1 = h;
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            put_px(f, w, x, y, c);
        }
    }
}

void ov_darken_rect(uint8_t *f, int w, int h, int x0, int y0, int x1, int y1)
{
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > w) x1 = w;
    if (y1 > h) y1 = h;
    x0 &= ~1;
    x1 = (x1 + 1) & ~1;
    for (int y = y0; y < y1; y++) {
        uint32_t *row = (uint32_t *)(void *)(f + ((size_t)y * w + x0) * 2);
        for (int i = 0; i < (x1 - x0) / 2; i++) {
            uint32_t v = row[i];
            uint32_t y0l = (v & 0xFFu) / 3u, y1l = ((v >> 16) & 0xFFu) / 3u;
            row[i] = y0l | (0x80u << 8) | (y1l << 16) | (0x80u << 24);
        }
    }
}

void ov_outline_rect(uint8_t *f, int w, int h, int x0, int y0, int x1, int y1, int t,
                     yuv_t c)
{
    ov_fill_rect(f, w, h, x0, y0, x1 + 1, y0 + t, c);
    ov_fill_rect(f, w, h, x0, y1 + 1 - t, x1 + 1, y1 + 1, c);
    ov_fill_rect(f, w, h, x0, y0, x0 + t, y1 + 1, c);
    ov_fill_rect(f, w, h, x1 + 1 - t, y0, x1 + 1, y1 + 1, c);
}

/* Copy one frame out of the ISP's buffer, mirrored or not. Mirroring a YUYV
 * row = reverse its word order and swap Y0/Y1 inside each word (U/V stay with
 * their pair). One read of the source: the ISP comes back to it 33 ms later. */
void ov_copy_frame(uint8_t *dst, const uint8_t *src, int w, int h, bool mirror)
{
    const int n = w / 2;

    if (!mirror) {
        memcpy(dst, src, (size_t)w * h * 2);
        return;
    }
    for (int y = 0; y < h; y++) {
        const uint32_t *s = (const uint32_t *)(const void *)(src + (size_t)y * w * 2);
        uint32_t *d = (uint32_t *)(void *)(dst + (size_t)y * w * 2);
        for (int i = 0; i < n; i++) {
            uint32_t v = s[n - 1 - i];
            d[i] = (v & 0xFF00FF00u) | ((v & 0xFFu) << 16) | ((v >> 16) & 0xFFu);
        }
    }
}

int ov_text_w(const char *s, int scale)
{
    int n = (int)strlen(s);
    return n ? (n * FONT_ADVANCE - 1) * scale : 0;
}

void ov_draw_text(uint8_t *f, int w, int h, int x, int y, int scale, const char *s,
                  yuv_t c, int pad)
{
    if (pad > 0) {
        ov_darken_rect(f, w, h, x - pad, y - pad, x + ov_text_w(s, scale) + pad,
                       y + FONT_H * scale + pad);
    }
    for (int i = 0; s[i]; i++) {
        const uint8_t *g = font5x7_glyph(s[i]);
        int gx = x + i * FONT_ADVANCE * scale;
        for (int col = 0; col < FONT_W; col++) {
            for (int row = 0; row < FONT_H; row++) {
                if (font5x7_pixel(g, col, row)) {
                    ov_fill_rect(f, w, h, gx + col * scale, y + row * scale,
                                 gx + (col + 1) * scale, y + (row + 1) * scale, c);
                }
            }
        }
    }
}
