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
 * @file page_detect.c
 * @brief Page 2: live preview with YOLOv8 person / pet boxes from the second
 *        model (sd:/models/person.inm).
 *
 *   ROCK      pet boxes on/off (people always)
 *   SCISSORS  mirror on/off
 *
 * Boxes come in model pixels (640x360 = sensor / 2) and are mapped onto
 * the preview through the crop window, as the person-detect app does.
 */

#include "ui.h"
#include "detect.h"

#include <stdio.h>
#include <string.h>

#define BOX_T           2
#define MAX_BOXES       DETECT_MAX

static bool       s_pets = true;
static evk_rect_t s_boxes[MAX_BOXES];
static int        s_nboxes;
static evk_rect_t s_gbox;              /* the gesture box, dim */
static evk_text_t s_hud;
static uint32_t   s_shown_seq = 0xFFFFFFFFu;

static int16_t map_x(const evk_cam_geometry_t *g, int mx)
{
    int sx = mx * 2 - (int)g->crop_x;                  /* model px -> sensor px */
    int px = (sx * (int)g->img_w) / (int)g->crop_w;
    return (int16_t)(px < 0 ? 0 : (px >= g->img_w ? g->img_w - 1 : px));
}

static int16_t map_y(const evk_cam_geometry_t *g, int my)
{
    int sy = my * 2 - (int)g->crop_y;
    int py = (sy * (int)g->img_h) / (int)g->crop_h;
    return (int16_t)(py < 0 ? 0 : (py >= g->img_h ? g->img_h - 1 : py));
}

static void prepare(const ui_frame_t *f)
{
    const evk_cam_geometry_t *g = &f->g;
    detect_result_t r;
    char buf[UI_MAX_TEXT + 1];

    ui_gesture_box(g, &s_gbox);
    s_gbox.thickness = 1;
    s_gbox.color = st7789p3_swap(f->hold_cls != INPUT_CLS_NONE ? ST_YELLOW : ST_GRAY);

    if (!detect_is_ready()) {
        s_nboxes = 0;
        snprintf(buf, sizeof(buf), "NO DETECTOR: %s", detect_fail_reason());
        evk_text_set(&s_hud, buf, 1, ST_YELLOW, true, 4, 4);
        ui_set_bottom("COPY person.inm TO SD:/MODELS/", ST_YELLOW);
        return;
    }

    detect_get_result(&r);
    if (r.seq != s_shown_seq && g->crop_w && g->crop_h) {
        s_shown_seq = r.seq;
        s_nboxes = 0;
        for (int i = 0; i < r.count && s_nboxes < MAX_BOXES; i++) {
            const detect_box_t *b = &r.box[i];
            if (b->cls == DETECT_CLASS_PET && !s_pets) {
                continue;
            }
            evk_rect_t *d = &s_boxes[s_nboxes];
            d->x1 = map_x(g, b->x1);
            d->x2 = map_x(g, b->x2);
            d->y1 = map_y(g, b->y1);
            d->y2 = map_y(g, b->y2);
            if (d->x2 <= d->x1 || d->y2 <= d->y1) {
                continue;
            }
            d->thickness = BOX_T;
            d->color = st7789p3_swap(b->cls == DETECT_CLASS_PERSON ? ST_GREEN : ST_YELLOW);
            s_nboxes++;
        }
    }
    snprintf(buf, sizeof(buf), "PERSON %u  PET %u  NPU %luMS", r.persons, r.pets,
             (unsigned long)r.npu_ms);
    evk_text_set(&s_hud, buf, 1, ST_WHITE, true, 4, 4);

    if (s_pets) {
        ui_set_bottom("ROCK=HIDE PETS  SCISSORS=MIRROR", ST_WHITE);
    } else {
        ui_set_bottom("ROCK=SHOW PETS  SCISSORS=MIRROR", ST_WHITE);
    }
}

static void row(const ui_frame_t *f, uint16_t *r, int w, int y)
{
    (void)f;
    /* Boxes are in unmirrored preview coordinates; mirror them with the
     * picture so they stay on the person. */
    bool mirror = ui_get_mirror();
    for (int i = 0; i < s_nboxes; i++) {
        if (mirror) {
            evk_rect_t m = s_boxes[i];
            m.x1 = w - 1 - s_boxes[i].x2;
            m.x2 = w - 1 - s_boxes[i].x1;
            evk_rect_row(&m, r, w, y);
        } else {
            evk_rect_row(&s_boxes[i], r, w, y);
        }
    }
    if (s_gbox.x2 > s_gbox.x1) {
        evk_rect_row(&s_gbox, r, w, y);
    }
    evk_text_row(&s_hud, r, w, y);
}

static void event(input_event_t ev)
{
    if (ev == INPUT_EV_SELECT) {
        s_pets = !s_pets;
        s_shown_seq = 0xFFFFFFFFu;         /* rebuild the box list */
    } else if (ev == INPUT_EV_ALT) {
        ui_set_mirror(!ui_get_mirror());
    }
}

const ui_page_t ui_page_detect = {
    .name          = "DETECT",
    .wants_preview = true,
    .enter         = NULL,
    .leave         = NULL,
    .prepare       = prepare,
    .row           = row,
    .event         = event,
    .hold          = NULL,
};
