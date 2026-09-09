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

#include "pricetag_render.h"

#include <string.h>
#include <stdint.h>

#include "epd_gdey0213f52.h"
#include "templates.h"
#include "digit_font.h"

/* Panel byte-order constants — keep in sync with epd_gdey0213f52.h /
 * tools/img_to_epd.py packing. */
#define BUF_W            128
#define PANEL_H          250
#define BYTES_PER_ROW    (BUF_W / 4)        /* 32 */
#define LAND_W           250                /* PNG was designed in 250x122 landscape */

/* Working framebuffer in BSS (8000 bytes). One static instance is fine —
 * pricetag_render is only ever called from the shell task. */
static uint8_t s_fb[EPD_FRAME_BYTES];

/* Plot one panel pixel by overwriting the 2-bit colour at (px, py). */
static inline void fb_set(uint32_t px, uint32_t py, uint8_t color)
{
    if (px >= BUF_W || py >= PANEL_H) return;
    uint32_t off   = py * BYTES_PER_ROW + (px >> 2);
    uint8_t  shift = (uint8_t)(6 - (px & 3) * 2);
    s_fb[off] = (uint8_t)((s_fb[off] & ~(0x3 << shift)) | ((color & 0x3) << shift));
}

/* Blit a 1bpp glyph into the framebuffer in landscape coordinates.
 * (lx, ly) is the top-left of the glyph in the 250x122 landscape design
 * frame. Set pixels are written as `color`; clear pixels are left
 * untouched (preserving the underlying template). */
static void blit_glyph_landscape(const digit_glyph_t *g,
                                 int lx, int ly,
                                 uint8_t color)
{
    if (g->width == 0 || g->length == 0) return;

    const uint8_t *bits          = digit_font_bitmap + g->offset;
    const int      bytes_per_row = (g->width + 7) / 8;

    for (int gy = 0; gy < g->height; gy++) {
        const uint8_t *row = bits + gy * bytes_per_row;
        for (int gx = 0; gx < g->width; gx++) {
            if (!(row[gx >> 3] & (0x80 >> (gx & 7)))) continue;

            /* Landscape pixel (lx+gx, ly+gy) -> portrait (px, py).
             * Mirrors PIL.Image.transpose(ROTATE_90):
             *   px = old_y, py = LAND_W - 1 - old_x. */
            int px = ly + gy;
            int py = LAND_W - 1 - (lx + gx);
            if (px < 0 || py < 0) continue;
            fb_set((uint32_t)px, (uint32_t)py, color);
        }
    }
}

/* Sum widths of glyphs in `s` (plus 1px tracking) — for right-justify. */
static int measure_string(const char *s)
{
    int total = 0;
    int n     = 0;
    while (*s) {
        unsigned char c = (unsigned char)*s++;
        if (c >= 128) return -1;
        const digit_glyph_t *g = &digit_font_glyphs[c];
        if (g->width == 0 && g->length == 0 && c != ' ') return -1;
        total += g->width;
        n++;
    }
    return total + (n > 1 ? n - 1 : 0);
}

int pricetag_render(int idx, const char *price_str)
{
    if (idx < 0 || idx >= PRICETAG_TEMPLATE_COUNT) return -1;
    if (!price_str)                                return -1;

    /* Start from the static template — every pixel including off-glass. */
    memcpy(s_fb, pricetag_templates[idx].data, EPD_FRAME_BYTES);

    int sw = measure_string(price_str);
    if (sw < 0) return -1;

    /* Right-justify within the landscape price slot (defined in templates.h). */
    int draw_lx = PRICETAG_PRICE_LX + PRICETAG_PRICE_LW - sw;
    if (draw_lx < PRICETAG_PRICE_LX) draw_lx = PRICETAG_PRICE_LX;
    int draw_ly = PRICETAG_PRICE_LY;

    for (const char *p = price_str; *p; p++) {
        unsigned char c = (unsigned char)*p;
        const digit_glyph_t *g = &digit_font_glyphs[c];
        if (g->width > 0 || g->length > 0) {
            blit_glyph_landscape(g, draw_lx, draw_ly, /* RED */ 0x3);
        }
        draw_lx += g->width + 1;
    }

    return epd_display(s_fb, sizeof(s_fb));
}
