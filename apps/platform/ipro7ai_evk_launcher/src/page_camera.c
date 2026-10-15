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
 * @file page_camera.c
 * @brief Page 1: live camera preview with the gesture box.
 *
 *   ROCK      freeze / resume the preview (a "snapshot" - the panel keeps
 *             the last frame, the overlays keep updating)
 *   SCISSORS  mirror on/off
 */

#include "ui.h"

#include <stdio.h>

#define BOX_T           2

static bool s_frozen;
static evk_rect_t s_box;
static evk_text_t s_label;

static void enter(void)
{
    s_frozen = false;
}

static void prepare(const ui_frame_t *f)
{
    ui_gesture_box(&f->g, &s_box);
    s_box.thickness = BOX_T;
    s_box.color = st7789p3_swap(f->hold_cls != INPUT_CLS_NONE ? ST_YELLOW : ST_WHITE);

    if (!f->preview && !s_frozen) {
        evk_text_set(&s_label, "NO CAMERA - CHECK J6 PIN 3-7", 1, ST_YELLOW, true, 4, 4);
    } else if (s_frozen) {
        evk_text_set(&s_label, "FROZEN", 2, ST_CYAN, true, 4, 4);
    } else {
        s_label.len = 0;
    }

    if (s_frozen) {
        ui_set_bottom("ROCK=RESUME", ST_CYAN);
    } else {
        ui_set_bottom("PAPER=NEXT  ROCK=FREEZE  SCISSORS=MIRROR", ST_WHITE);
    }
}

static void row(const ui_frame_t *f, uint16_t *r, int w, int y)
{
    (void)f;
    if (s_box.x2 > s_box.x1) {
        evk_rect_row(&s_box, r, w, y);
    }
    evk_text_row(&s_label, r, w, y);
}

static void event(input_event_t ev)
{
    if (ev == INPUT_EV_SELECT) {
        s_frozen = !s_frozen;
    } else if (ev == INPUT_EV_ALT) {
        ui_set_mirror(!ui_get_mirror());
    }
}

/* Freeze = do not repaint the page area; the panel keeps the last frame. */
static bool hold(void)
{
    return s_frozen;
}

const ui_page_t ui_page_camera = {
    .name          = "CAMERA",
    .wants_preview = true,
    .enter         = enter,
    .leave         = NULL,
    .prepare       = prepare,
    .row           = row,
    .event         = event,
    .hold          = hold,
};
