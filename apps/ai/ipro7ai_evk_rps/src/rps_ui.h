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
 * @file rps_ui.h
 * @brief Game screen on the ST7789P3: live preview with the "play box",
 *        countdown, live class label, and score / message bars.
 *
 * Layout (landscape 320x240, letterbox preview 320x180 at y=30):
 *
 *   rows   0- 29  top bar     YOU n  ME n  DRAW n            ME: PAPER
 *   rows  30-209  preview     ROI rectangle, live label, countdown digit
 *   rows 210-239  bottom bar  SHOW YOUR HAND / READY... / YOU WIN! ...
 *
 * The preview rows are composited per row as they stream to the panel (no
 * framebuffer). The two bars are outside the image and are redrawn only when
 * their text changes.
 */
#ifndef RPS_UI_H
#define RPS_UI_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t drawn, dropped, fps_x10, blit_ms;
} rps_ui_stats_t;

int  rps_ui_init(void);                 /* panel up + splash */
int  rps_ui_start(void);                /* render task, hooks the camera */
void rps_ui_stop(void);
bool rps_ui_running(void);
void rps_ui_splash(const char *line1, const char *line2);
void rps_ui_get_stats(rps_ui_stats_t *out);

void rps_ui_set_uv_swap(bool on);
bool rps_ui_get_uv_swap(void);

/** @brief Mirror the preview left-right like a selfie camera (default on).
 *         Overlays and the NPU input are unaffected. */
void rps_ui_set_mirror(bool on);
bool rps_ui_get_mirror(void);

#endif /* RPS_UI_H */
