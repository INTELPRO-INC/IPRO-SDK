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
 * @file evk_gfx.h
 * @brief Row-compositing text and overlay helpers for the ST7789P3 panel.
 *
 * The panel driver has no framebuffer - the EVK apps stream the picture one
 * row at a time out of a single row buffer. Everything here works in that
 * model: an overlay is prepared once (position, colour, glyphs) and then
 * asked "what do you contribute to row y?" while the row is being built.
 * Pixels in a row buffer are always in WIRE order (st7789p3_swap()), and so
 * are the colours stored in the overlay structs.
 *
 * Extracted from the person-detect / rock-paper-scissors apps, where this
 * shape proved cheap enough to run beside a 10 fps camera preview.
 */
#ifndef EVK_GFX_H
#define EVK_GFX_H

#include <stdint.h>
#include <stdbool.h>

#include "st7789p3.h"
#include "font5x7.h"

#define EVK_GFX_MAX_CHARS   52      /* 53 x 6 px fit a 320 px row at scale 1 */
#define EVK_GFX_PAD         2       /* padding around a text box, in pixels */

/** A line of text at a fixed place, in row space. Prepare with
 *  evk_text_set(), composite with evk_text_row(). */
typedef struct {
    char     text[EVK_GFX_MAX_CHARS + 1];
    int      len, scale;
    uint16_t fg;                     /* wire order */
    bool     backing;                /* black box behind the glyphs? */
    int      x, y, w, h;             /* padded box, in the row space */
} evk_text_t;

/** An outlined rectangle, in row space. Colour in wire order. */
typedef struct {
    int      x1, y1, x2, y2;         /* inclusive */
    int      thickness;
    uint16_t color;                  /* wire order */
} evk_rect_t;

/* ---- text ---- */

/** @brief Prepare @p t to draw @p s at (x,y) (top-left of the padded box).
 *         @p fg is an RGB565 colour in host order; it is stored swapped. */
void evk_text_set(evk_text_t *t, const char *s, int scale, uint16_t fg,
                  bool backing, int x, int y);

/** @brief Padded box width/height of @p s at @p scale (0 for an empty string). */
int  evk_text_width(const char *s, int scale);
int  evk_text_height(int scale);

/** @brief Composite the part of @p t that intersects row @p y into @p row
 *         (width @p w). Clipped horizontally; a no-op outside the box. */
void evk_text_row(const evk_text_t *t, uint16_t *row, int w, int y);

/* ---- shapes ---- */

/** @brief Composite the outline of @p r that intersects row @p y. */
void evk_rect_row(const evk_rect_t *r, uint16_t *row, int w, int y);

/* ---- pixel conversion ---- */

/**
 * @brief One YUYV row (two pixels per 32-bit word, [Y0 U Y1 V], BT.601) ->
 *        RGB565 in wire order. @p npix must be even.
 * @param uv_swap  Treat the chroma bytes as [Y0 V Y1 U] (skin comes out blue
 *                 when this is wrong).
 * @param mirror   Write the row right-to-left (selfie view).
 */
void evk_gfx_yuyv_row(const uint32_t *src, uint16_t *dst, int npix,
                      bool uv_swap, bool mirror);

/* ---- direct drawing (takes the panel bus lock itself) ---- */

/** @brief Fill rows y0..y0+h-1 across the panel with @p bg and composite up
 *         to two prepared text lines onto them. */
void evk_gfx_draw_bar(int y0, int h, const evk_text_t *a, const evk_text_t *b,
                      uint16_t bg);

/** @brief Draw @p s at (x,y) with a solid @p bg box behind it. */
void evk_gfx_text_at(int x, int y, const char *s, int scale, uint16_t fg,
                     uint16_t bg);

/** @brief Draw @p s horizontally centred in rows y..y+height. */
void evk_gfx_text_centered(int y, const char *s, int scale, uint16_t fg,
                           uint16_t bg);

/** @brief Clear the panel and put two centred lines on it (big, then dim). */
void evk_gfx_splash(const char *line1, const char *line2);

#endif /* EVK_GFX_H */
