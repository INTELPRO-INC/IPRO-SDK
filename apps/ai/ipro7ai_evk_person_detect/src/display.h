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
 * @file display.h
 * @brief Preview + detection overlay on the ST7789P3 panel.
 *
 * A render task takes the newest ISP channel-0 YUYV frame, converts it to
 * RGB565 one row at a time, composites the detector's boxes and a two-line
 * HUD into each row on the way past, and pushes the row to the panel. No
 * framebuffer: 320x180x2 would be another 115 KB and the panel is written
 * row by row anyway.
 *
 * Boxes arrive in model-input pixels (640x360 = sensor/2) and are remapped
 * through the camera geometry, so letterbox and crop previews both line up.
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t drawn;        /* frames rendered to the panel                  */
    uint32_t dropped;      /* frames superseded before the render started   */
    uint32_t fps_x10;      /* render rate, tenths of a frame per second     */
    uint32_t blit_ms;      /* wall time of the last frame, convert + SPI    */
} display_stats_t;

/** @brief Bring the panel up (minimal init sequence) and show the splash. */
int  display_init(void);

/** @brief Start the render task and hook the camera's frame callback. */
int  display_start(void);
void display_stop(void);
bool display_is_running(void);

/** @brief Two centred lines of 2x text on black. Used before the camera is
 *         up and as the "no camera" screen. Takes the panel bus lock. */
void display_splash(const char *line1, const char *line2);

/** @brief Swap the U and V chroma bytes when converting (if the picture has
 *         skin tones coming out blue). Default off = [Y0 U Y1 V]. */
void display_set_uv_swap(bool on);
bool display_get_uv_swap(void);

/** @brief Draw pet boxes too (default on; persons are always drawn). */
void display_set_pets(bool on);
bool display_get_pets(void);

/** @brief Show the HUD text band (default on). */
void display_set_hud(bool on);
bool display_get_hud(void);

void display_get_stats(display_stats_t *out);

#endif /* DISPLAY_H */
