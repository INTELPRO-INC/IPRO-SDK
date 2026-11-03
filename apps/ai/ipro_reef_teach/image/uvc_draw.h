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
 * @file uvc_draw.h
 * @brief Drawing into a packed YUYV422 frame before it is encoded: rectangles,
 *        a darkening scrim, 5x7 text and a (mirrored) frame copy. Shared by
 *        the Reef stream overlays (uvc_rps.c, uvc_teach.c).
 */
#ifndef UVC_DRAW_H
#define UVC_DRAW_H

#include <stdbool.h>
#include <stdint.h>

/* BT.601 video-range colour. */
typedef struct {
    uint8_t y, u, v;
} yuv_t;

#define OV_WHITE  ((yuv_t){235, 128, 128})
#define OV_GREY   ((yuv_t){150, 128, 128})
#define OV_YELLOW ((yuv_t){210,  16, 146})
#define OV_GREEN  ((yuv_t){145,  54,  34})
#define OV_RED    ((yuv_t){ 81,  90, 240})
#define OV_CYAN   ((yuv_t){170, 166,  16})

/* All take the frame f of width w, height h; rectangles are clipped to it and
 * x1/y1 are exclusive. */
void ov_fill_rect(uint8_t *f, int w, int h, int x0, int y0, int x1, int y1, yuv_t c);

/* Dark grey scrim behind text: luma to a third, chroma neutral. Works on whole
 * YUYV words, so x is widened to even boundaries. */
void ov_darken_rect(uint8_t *f, int w, int h, int x0, int y0, int x1, int y1);

/* Outline of thickness t on the inside of (x0,y0)-(x1,y1), corners inclusive. */
void ov_outline_rect(uint8_t *f, int w, int h, int x0, int y0, int x1, int y1, int t,
                     yuv_t c);

/* Copy one frame out of the ISP's buffer, mirrored or not. One read of the
 * source: the ISP comes back to it 33 ms later. */
void ov_copy_frame(uint8_t *dst, const uint8_t *src, int w, int h, bool mirror);

/* Width in pixels of s at the given scale. */
int ov_text_w(const char *s, int scale);

/* Text at (x,y) = top-left of the glyphs; with a scrim padded by pad if pad > 0. */
void ov_draw_text(uint8_t *f, int w, int h, int x, int y, int scale, const char *s,
                  yuv_t c, int pad);

#endif /* UVC_DRAW_H */
