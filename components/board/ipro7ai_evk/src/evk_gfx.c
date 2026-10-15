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
 * @file evk_gfx.c
 * @brief Row-compositing text and overlay helpers. See evk_gfx.h.
 */

#include "evk_gfx.h"

#include <string.h>

/* One row buffer for the direct-drawing helpers. They all take the panel bus
 * lock for the whole burst, so sharing it between them is safe. */
static uint16_t s_row[ST_MAX_DIM];

/* ---------------------------------------------------------------------------
 * Text
 * -------------------------------------------------------------------------*/

int evk_text_width(const char *s, int scale)
{
    int n = s ? (int)strlen(s) : 0;

    if (n > EVK_GFX_MAX_CHARS) {
        n = EVK_GFX_MAX_CHARS;
    }
    return n ? (n * FONT_ADVANCE - 1) * scale + 2 * EVK_GFX_PAD : 0;
}

int evk_text_height(int scale)
{
    return FONT_H * scale + 2 * EVK_GFX_PAD;
}

void evk_text_set(evk_text_t *t, const char *s, int scale, uint16_t fg,
                  bool backing, int x, int y)
{
    if (!s) {
        s = "";
    }
    t->len = (int)strlen(s);
    if (t->len > EVK_GFX_MAX_CHARS) {
        t->len = EVK_GFX_MAX_CHARS;
    }
    memcpy(t->text, s, (size_t)t->len);
    t->text[t->len] = '\0';
    t->scale   = scale < 1 ? 1 : scale;
    t->fg      = st7789p3_swap(fg);
    t->backing = backing;
    t->x = x;
    t->y = y;
    t->w = t->len ? (t->len * FONT_ADVANCE - 1) * t->scale + 2 * EVK_GFX_PAD : 0;
    t->h = FONT_H * t->scale + 2 * EVK_GFX_PAD;
}

void evk_text_row(const evk_text_t *t, uint16_t *row, int w, int y)
{
    int ry = y - t->y;

    if (!t->len || ry < 0 || ry >= t->h) {
        return;
    }
    if (t->backing) {
        const uint16_t bg = st7789p3_swap(ST_BLACK);
        int x0 = t->x < 0 ? 0 : t->x;
        int x1 = t->x + t->w > w ? w : t->x + t->w;
        for (int x = x0; x < x1; x++) {
            row[x] = bg;
        }
    }
    int gy = (ry - EVK_GFX_PAD) / t->scale;
    if (ry < EVK_GFX_PAD || gy >= FONT_H) {
        return;
    }
    for (int i = 0; i < t->len; i++) {
        const uint8_t *g  = font5x7_glyph(t->text[i]);
        int            gx = t->x + EVK_GFX_PAD + i * FONT_ADVANCE * t->scale;
        for (int c = 0; c < FONT_W; c++) {
            if (!font5x7_pixel(g, c, gy)) {
                continue;
            }
            for (int s = 0; s < t->scale; s++) {
                int x = gx + c * t->scale + s;
                if (x >= 0 && x < w) {
                    row[x] = t->fg;
                }
            }
        }
    }
}

/* ---------------------------------------------------------------------------
 * Shapes
 * -------------------------------------------------------------------------*/

void evk_rect_row(const evk_rect_t *r, uint16_t *row, int w, int y)
{
    int t = r->thickness < 1 ? 1 : r->thickness;

    if (y < r->y1 || y > r->y2 || r->x1 > r->x2) {
        return;
    }
    int x1 = r->x1 < 0 ? 0 : r->x1;
    int x2 = r->x2 >= w ? w - 1 : r->x2;

    if (y - r->y1 < t || r->y2 - y < t) {
        for (int x = x1; x <= x2; x++) {
            row[x] = r->color;
        }
        return;
    }
    for (int i = 0; i < t; i++) {
        int xl = r->x1 + i, xr = r->x2 - i;
        if (xl >= 0 && xl < w) row[xl] = r->color;
        if (xr >= 0 && xr < w) row[xr] = r->color;
    }
}

/* ---------------------------------------------------------------------------
 * YUYV -> RGB565
 * -------------------------------------------------------------------------*/

static inline uint8_t clamp_u8(int v)
{
    return (uint8_t)(v < 0 ? 0 : (v > 255 ? 255 : v));
}

void evk_gfx_yuyv_row(const uint32_t *src, uint16_t *dst, int npix,
                      bool uv_swap, bool mirror)
{
    const int cb_i = uv_swap ? 3 : 1, cr_i = uv_swap ? 1 : 3;
    const int step = mirror ? -1 : 1;
    int o = mirror ? npix - 1 : 0;

    for (int i = 0; i < npix; i += 2) {
        uint32_t w  = *src++;
        int y0 = (int)(w & 0xFFu), y1 = (int)((w >> 16) & 0xFFu);
        int cb = (int)((w >> (8 * cb_i)) & 0xFFu) - 128;
        int cr = (int)((w >> (8 * cr_i)) & 0xFFu) - 128;
        int dr = (359 * cr) >> 8, dg = (88 * cb + 183 * cr) >> 8, db = (454 * cb) >> 8;
        dst[o] = st7789p3_swap(ST_RGB565(clamp_u8(y0 + dr), clamp_u8(y0 - dg), clamp_u8(y0 + db)));
        o += step;
        dst[o] = st7789p3_swap(ST_RGB565(clamp_u8(y1 + dr), clamp_u8(y1 - dg), clamp_u8(y1 + db)));
        o += step;
    }
}

/* ---------------------------------------------------------------------------
 * Direct drawing
 * -------------------------------------------------------------------------*/

void evk_gfx_draw_bar(int y0, int h, const evk_text_t *a, const evk_text_t *b,
                      uint16_t bg)
{
    uint16_t w   = st7789p3_width();
    uint16_t bgw = st7789p3_swap(bg);

    if (h <= 0 || y0 < 0 || y0 + h > st7789p3_height()) {
        return;
    }
    st7789p3_bus_lock();
    st7789p3_blit_start(0, (uint16_t)y0, (uint16_t)(w - 1u), (uint16_t)(y0 + h - 1));
    for (int y = y0; y < y0 + h; y++) {
        for (int x = 0; x < w; x++) {
            s_row[x] = bgw;
        }
        if (a) evk_text_row(a, s_row, w, y);
        if (b) evk_text_row(b, s_row, w, y);
        st7789p3_blit_push(s_row, w);
    }
    st7789p3_blit_end();
    st7789p3_bus_unlock();
}

void evk_gfx_text_at(int x, int y, const char *s, int scale, uint16_t fg,
                     uint16_t bg)
{
    evk_text_t t;
    uint16_t   bgw = st7789p3_swap(bg);
    int        pw  = st7789p3_width(), ph = st7789p3_height();

    evk_text_set(&t, s, scale, fg, false, x, y);
    if (!t.len || t.x >= pw || t.y >= ph) {
        return;
    }
    int x0 = t.x < 0 ? 0 : t.x;
    int x1 = t.x + t.w > pw ? pw : t.x + t.w;
    int y0 = t.y < 0 ? 0 : t.y;
    int y1 = t.y + t.h > ph ? ph : t.y + t.h;
    if (x1 <= x0 || y1 <= y0) {
        return;
    }

    st7789p3_bus_lock();
    st7789p3_blit_start((uint16_t)x0, (uint16_t)y0, (uint16_t)(x1 - 1), (uint16_t)(y1 - 1));
    for (int yy = y0; yy < y1; yy++) {
        /* Build the row in panel coordinates, push only the box. */
        for (int xx = x0; xx < x1; xx++) {
            s_row[xx] = bgw;
        }
        evk_text_row(&t, s_row, pw, yy);
        st7789p3_blit_push(&s_row[x0], (uint16_t)(x1 - x0));
    }
    st7789p3_blit_end();
    st7789p3_bus_unlock();
}

void evk_gfx_text_centered(int y, const char *s, int scale, uint16_t fg,
                           uint16_t bg)
{
    int w = evk_text_width(s, scale);
    evk_gfx_text_at((st7789p3_width() - w) / 2, y, s, scale, fg, bg);
}

void evk_gfx_splash(const char *line1, const char *line2)
{
    evk_text_t a, b;
    uint16_t w = st7789p3_width(), h = st7789p3_height();
    int aw = evk_text_width(line1, 2), bw = evk_text_width(line2, 1);
    int ah = evk_text_height(2), bh = evk_text_height(1);
    int y0 = (h - (ah + 2 + bh)) / 2;

    evk_text_set(&a, line1, 2, ST_WHITE, false, (w - aw) / 2, y0);
    evk_text_set(&b, line2, 1, ST_GRAY,  false, (w - bw) / 2, y0 + ah + 2);

    st7789p3_bus_lock();
    st7789p3_fill_screen(ST_BLACK);
    evk_gfx_draw_bar(y0, ah + 2 + bh, &a, &b, ST_BLACK);
    st7789p3_bus_unlock();
}
