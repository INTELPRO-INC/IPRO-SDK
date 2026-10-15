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
 * @file ui.h
 * @brief The launcher screen: pages, bars, gesture navigation.
 *
 * Landscape 320x240:
 *
 *   rows   0- 29  top bar     "1/3 CAMERA"                      "PAPER 92%"
 *   rows  30-209  page area   camera preview or the page's own drawing,
 *                             plus the page's overlays; a thin hold-progress
 *                             line along its top edge while a gesture is held
 *   rows 210-239  bottom bar  the page's hint / status line
 *
 * One render task, woken by the camera at the preview rate (or a 100 ms
 * timer when there is no camera), composites the page area row by row and
 * streams it to the panel. The bars are redrawn only when their text
 * changes. Gestures are turned into events here: PAPER moves to the next
 * page, ROCK / SCISSORS go to the page.
 */
#ifndef UI_H
#define UI_H

#include <stdint.h>
#include <stdbool.h>

#include "evk.h"
#include "gesture.h"
#include "input.h"

#define UI_BAR_H         30
#define UI_MAX_TEXT      EVK_GFX_MAX_CHARS

/* The gesture box: a 512x512 square in the middle of the 1280x720 sensor.
 * In the 320x180 letterbox preview (sensor / 4) that is exactly the
 * 128x128 window the classifier wants, so the UI task cuts it straight out
 * of the preview frame - no ISP channel needed for it. */
#define UI_GESTURE_ROI_W   512
#define UI_GESTURE_ROI_H   512
#define UI_GESTURE_ROI_X   ((EVK_CAM_SENSOR_W - UI_GESTURE_ROI_W) / 2)
#define UI_GESTURE_ROI_Y   ((EVK_CAM_SENSOR_H - UI_GESTURE_ROI_H) / 2)

/** Per-frame context handed to a page. */
typedef struct {
    evk_cam_geometry_t g;          /* page area = g.img_w x g.img_h at (g.ox, g.oy) */
    gesture_result_t   ges;        /* newest classifier result                       */
    int                hold_cls;   /* INPUT_CLS_* being held, or NONE                */
    int                hold_pct;   /* 0..100                                         */
    uint32_t           now_ms;
    bool               preview;    /* a camera frame is behind this page             */
} ui_frame_t;

/** A page. Rows handed to row() are in page-area coordinates (0..img_h-1)
 *  and already hold the preview (or black); the page composites on top. */
typedef struct {
    const char *name;
    bool        wants_preview;
    void (*enter)(void);
    void (*leave)(void);
    void (*prepare)(const ui_frame_t *f);
    void (*row)(const ui_frame_t *f, uint16_t *row, int w, int y);
    void (*event)(input_event_t ev);        /* SELECT / ALT */
    bool (*hold)(void);                     /* true = leave the page area as it is this frame */
} ui_page_t;

typedef struct {
    uint32_t drawn, dropped, fps_x10, blit_ms, events;
    int      page;
} ui_stats_t;

int  ui_init(void);                     /* panel splash */
int  ui_start(void);                    /* render task; hooks the camera */
void ui_stop(void);
bool ui_running(void);
void ui_splash(const char *line1, const char *line2);

/* Pages. */
int  ui_page_count(void);
int  ui_page(void);
void ui_set_page(int idx);
void ui_next_page(void);
const char *ui_page_name(int idx);

/** @brief Feed an event as if a gesture produced it (shell / tests). */
void ui_inject(input_event_t ev);

/** @brief Bar text. Pages call these from prepare(); a change marks the
 *         bars dirty. NULL leaves a field alone. */
void ui_set_top_right(const char *s);
void ui_set_bottom(const char *s, uint16_t color);

void ui_get_stats(ui_stats_t *out);
void ui_set_mirror(bool on);
bool ui_get_mirror(void);
void ui_set_uv_swap(bool on);
bool ui_get_uv_swap(void);

/** @brief Gesture hold time / release time / confidence floor. Applied by the
 *         render task on its next frame (the debounce state is UI-task only). */
void ui_input_config(uint32_t hold_ms, uint32_t release_ms, int min_conf);

/** @brief The gesture box in page-area pixels for @p g (zero-size when the
 *         camera geometry is not known). */
void ui_gesture_box(const evk_cam_geometry_t *g, evk_rect_t *out);

/* Pages, defined in page_*.c */
extern const ui_page_t ui_page_camera;
extern const ui_page_t ui_page_detect;
extern const ui_page_t ui_page_voice;
extern const ui_page_t ui_page_about;

#endif /* UI_H */
