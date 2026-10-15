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
 * @file ui.c
 * @brief Render task, page switching, bars, gesture -> event. See ui.h.
 */

#include "ui.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <system_soc.h>
#include "ipro_log.h"

#define LOG_TAG "UI"

/* Below the NPU (-3) and voice (-4) tasks, above the shell (20): the polling
 * SPI driver spins for the whole blit, so this is the task everyone else must
 * be able to preempt. */
#define UI_TASK_STACK    1536
#define UI_TASK_PRIO     (configMAX_PRIORITIES - 6)

/* No camera: render anyway at this rate so the voice / about pages live. */
#define UI_IDLE_PERIOD_MS 100

#define HOLD_LINE_H      2               /* hold-progress cue, page-area rows 0..1 */

#define INPUT_HOLD_MS    500
#define INPUT_RELEASE_MS 300
#define INPUT_MIN_CONF   60

#define COL_BAR_BG       ST_BLACK
#define COL_TEXT         ST_WHITE
#define COL_TEXT_DIM     ST_GRAY
#define COL_HOLD         ST_YELLOW

static const ui_page_t *const s_pages[] = {
    &ui_page_camera,
    &ui_page_detect,
    &ui_page_voice,
    &ui_page_about,
};
#define PAGE_COUNT  ((int)(sizeof(s_pages) / sizeof(s_pages[0])))

/* ---------------------------------------------------------------------------
 * State
 * -------------------------------------------------------------------------*/

static volatile bool     s_running;
static TaskHandle_t      s_task;
static SemaphoreHandle_t s_sem;
static volatile uint32_t s_latest;
static bool              s_uv_swap;
static bool              s_mirror = true;    /* selfie view */
static ui_stats_t        s_stats;
static input_t           s_input;
static volatile int      s_page;
static volatile int      s_page_req = -1;    /* page change asked from another task */
static volatile int      s_inject   = INPUT_EV_NONE;
/* Gesture timing change asked from another task; applied inside render() so
 * input_t is only ever touched by the UI task. */
static volatile bool     s_input_cfg_req;
static uint32_t          s_input_cfg_hold, s_input_cfg_release;
static int               s_input_cfg_conf;

static uint16_t s_row[ST_MAX_DIM];

/* Bars. */
static char s_top_left[UI_MAX_TEXT + 1], s_top_right[UI_MAX_TEXT + 1], s_bottom[UI_MAX_TEXT + 1];
static uint16_t s_bottom_col = COL_TEXT;
static bool     s_bars_dirty = true;

/* ---------------------------------------------------------------------------
 * Geometry
 * -------------------------------------------------------------------------*/

/* The camera's preview geometry, or - when the pipeline is not up (no sensor,
 * or not started yet) - the same 16:9 letterbox laid out from the panel size,
 * so the pages that do not need a camera still have a place to draw. */
static void page_geometry(evk_cam_geometry_t *g)
{
    evk_cam_get_geometry(g);
    if (g->img_w && g->img_h) {
        return;
    }
    memset(g, 0, sizeof(*g));
    g->panel_w = st7789p3_width();
    g->panel_h = st7789p3_height();
    g->img_w   = g->panel_w;
    g->img_h   = (uint16_t)((uint32_t)g->panel_w * 9u / 16u);
    if (g->img_h > g->panel_h) {
        g->img_h = g->panel_h;
    }
    g->img_w &= (uint16_t)~1u;
    g->ox = (uint16_t)((g->panel_w - g->img_w) / 2u);
    g->oy = (uint16_t)((g->panel_h - g->img_h) / 2u);
}

static int map_x(const evk_cam_geometry_t *g, int sx)
{
    int px = ((sx - (int)g->crop_x) * (int)g->img_w) / (int)g->crop_w;
    return px < 0 ? 0 : (px >= g->img_w ? g->img_w - 1 : px);
}

static int map_y(const evk_cam_geometry_t *g, int sy)
{
    int py = ((sy - (int)g->crop_y) * (int)g->img_h) / (int)g->crop_h;
    return py < 0 ? 0 : (py >= g->img_h ? g->img_h - 1 : py);
}

void ui_gesture_box(const evk_cam_geometry_t *g, evk_rect_t *out)
{
    memset(out, 0, sizeof(*out));
    if (!g->crop_w || !g->crop_h) {
        return;
    }
    out->x1 = map_x(g, UI_GESTURE_ROI_X);
    out->x2 = map_x(g, UI_GESTURE_ROI_X + UI_GESTURE_ROI_W - 1);
    out->y1 = map_y(g, UI_GESTURE_ROI_Y);
    out->y2 = map_y(g, UI_GESTURE_ROI_Y + UI_GESTURE_ROI_H - 1);
}

/* Hand the gesture box of this preview frame to the classifier, if it is
 * idle. The box is 128x128 only in the letterbox geometry (sensor / 4);
 * centre-crop changes the scale and the classifier is skipped. */
static void feed_gesture(const evk_cam_geometry_t *g, uint32_t addr)
{
    if (!addr || !gesture_ready() || gesture_busy() || !g->crop_w) {
        return;
    }
    int w  = (UI_GESTURE_ROI_W * g->img_w) / g->crop_w;
    if (w != GESTURE_IN_W) {
        return;
    }
    int x0 = ((UI_GESTURE_ROI_X - (int)g->crop_x) * g->img_w) / g->crop_w & ~1;
    int y0 = ((UI_GESTURE_ROI_Y - (int)g->crop_y) * g->img_h) / g->crop_h;
    if (x0 < 0 || y0 < 0 || x0 + GESTURE_IN_W > g->img_w || y0 + GESTURE_IN_H > g->img_h) {
        return;
    }
    gesture_push_yuyv((const uint32_t *)(uintptr_t)addr, g->img_w, x0, y0);
}

/* ---------------------------------------------------------------------------
 * Bars
 * -------------------------------------------------------------------------*/

void ui_set_top_right(const char *s)
{
    if (s && strncmp(s, s_top_right, UI_MAX_TEXT)) {
        strncpy(s_top_right, s, UI_MAX_TEXT);
        s_top_right[UI_MAX_TEXT] = '\0';
        s_bars_dirty = true;
    }
}

void ui_set_bottom(const char *s, uint16_t color)
{
    if (s && (strncmp(s, s_bottom, UI_MAX_TEXT) || color != s_bottom_col)) {
        strncpy(s_bottom, s, UI_MAX_TEXT);
        s_bottom[UI_MAX_TEXT] = '\0';
        s_bottom_col = color;
        s_bars_dirty = true;
    }
}

static void set_top_left_for_page(void)
{
    char buf[UI_MAX_TEXT + 1];

    snprintf(buf, sizeof(buf), "%d/%d %s", s_page + 1, PAGE_COUNT, s_pages[s_page]->name);
    if (strcmp(buf, s_top_left)) {
        strcpy(s_top_left, buf);
        s_bars_dirty = true;
    }
}

static void draw_bars(const evk_cam_geometry_t *g)
{
    evk_text_t l, r;
    int top_h = g->oy;
    int bot_y = g->oy + g->img_h;
    int bot_h = g->panel_h - bot_y;

    if (top_h > 0) {
        evk_text_set(&l, s_top_left, 2, COL_TEXT, false, 4, (top_h - evk_text_height(2)) / 2);
        int rw = evk_text_width(s_top_right, 1);
        evk_text_set(&r, s_top_right, 1, COL_TEXT_DIM, false, g->panel_w - rw - 4,
                     (top_h - evk_text_height(1)) / 2);
        evk_gfx_draw_bar(0, top_h, &l, &r, COL_BAR_BG);
    }
    if (bot_h > 0) {
        int scale = evk_text_width(s_bottom, 2) <= g->panel_w - 8 ? 2 : 1;
        int bw = evk_text_width(s_bottom, scale);
        evk_text_set(&l, s_bottom, scale, s_bottom_col, false, (g->panel_w - bw) / 2,
                     bot_y + (bot_h - evk_text_height(scale)) / 2);
        evk_gfx_draw_bar(bot_y, bot_h, &l, NULL, COL_BAR_BG);
    }
}

/* ---------------------------------------------------------------------------
 * Pages
 * -------------------------------------------------------------------------*/

int         ui_page_count(void)      { return PAGE_COUNT; }
int         ui_page(void)            { return s_page; }
const char *ui_page_name(int idx)    { return (idx >= 0 && idx < PAGE_COUNT) ? s_pages[idx]->name : "?"; }

void ui_set_page(int idx)
{
    if (idx >= 0 && idx < PAGE_COUNT) {
        s_page_req = idx;
    }
}

void ui_next_page(void)
{
    ui_set_page((s_page + 1) % PAGE_COUNT);
}

void ui_inject(input_event_t ev)
{
    s_inject = (int)ev;
}

/* Runs on the render task only. */
static void switch_page(int idx)
{
    if (idx == s_page) {
        return;
    }
    if (s_pages[s_page]->leave) {
        s_pages[s_page]->leave();
    }
    s_page = idx;
    if (s_pages[s_page]->enter) {
        s_pages[s_page]->enter();
    }
    /* A hand still in the box must not act on the new page. */
    input_init(&s_input, s_input.hold_ms, s_input.release_ms, s_input.min_conf);
    set_top_left_for_page();
    /* The page area is repainted on the next frame; clear it now so a page
     * without a preview does not inherit the camera picture for a frame. */
    evk_cam_geometry_t g;
    page_geometry(&g);
    st7789p3_fill_rect(g.ox, g.oy, (uint16_t)(g.ox + g.img_w - 1u),
                       (uint16_t)(g.oy + g.img_h - 1u), ST_BLACK);
    IPRO_LOGI(LOG_TAG, "page %d/%d %s", s_page + 1, PAGE_COUNT, s_pages[s_page]->name);
}

static void dispatch(input_event_t ev)
{
    if (ev == INPUT_EV_NONE) {
        return;
    }
    s_stats.events++;
    if (ev == INPUT_EV_NEXT) {
        switch_page((s_page + 1) % PAGE_COUNT);
    } else if (s_pages[s_page]->event) {
        s_pages[s_page]->event(ev);
    }
}

/* ---------------------------------------------------------------------------
 * Render task
 * -------------------------------------------------------------------------*/

static void frame_cb(uint32_t addr, void *arg)
{
    BaseType_t woken = pdFALSE;

    (void)arg;
    if (s_latest) {
        s_stats.dropped++;
    }
    s_latest = addr;
    if (s_sem) {
        xSemaphoreGiveFromISR(s_sem, &woken);
        portYIELD_FROM_ISR(woken);
    }
}

static void hold_row(const ui_frame_t *f, uint16_t *row, int w, int y)
{
    if (y >= HOLD_LINE_H || f->hold_cls == INPUT_CLS_NONE || f->hold_pct <= 0) {
        return;
    }
    int x1 = w * f->hold_pct / 100;
    uint16_t c = st7789p3_swap(COL_HOLD);
    for (int x = 0; x < x1 && x < w; x++) {
        row[x] = c;
    }
}

static void render(uint32_t addr)
{
    ui_frame_t     f;
    const ui_page_t *pg;
    TickType_t     t0 = xTaskGetTickCount();

    /* Requests from other tasks. */
    if (s_page_req >= 0) {
        int req = s_page_req;
        s_page_req = -1;
        switch_page(req);
    }
    if (s_inject != INPUT_EV_NONE) {
        input_event_t ev = (input_event_t)s_inject;
        s_inject = INPUT_EV_NONE;
        dispatch(ev);
    }
    if (s_input_cfg_req) {
        s_input_cfg_req = false;
        input_init(&s_input, s_input_cfg_hold, s_input_cfg_release, s_input_cfg_conf);
    }
    pg = s_pages[s_page];

    memset(&f, 0, sizeof(f));
    page_geometry(&f.g);
    gesture_get_result(&f.ges);
    f.now_ms  = (uint32_t)(t0 * portTICK_PERIOD_MS);

    /* Gesture -> event, on the newest classifier result. */
    dispatch(input_update(&s_input, f.now_ms, (int)f.ges.cls, f.ges.conf, f.ges.seq));
    pg = s_pages[s_page];                     /* dispatch may have switched */
    f.preview  = pg->wants_preview && addr != 0;
    f.hold_cls = input_holding(&s_input, &f.hold_pct);

    /* Top-right: what the classifier sees. */
    {
        char buf[UI_MAX_TEXT + 1];
        if (!gesture_ready()) {
            snprintf(buf, sizeof(buf), "NO MODEL: %s", gesture_fail_reason());
        } else if (f.ges.cls != GESTURE_NONE) {
            snprintf(buf, sizeof(buf), "%s %u%%  %luMS", gesture_name(f.ges.cls), f.ges.conf,
                     (unsigned long)f.ges.npu_ms);
        } else {
            snprintf(buf, sizeof(buf), "SHOW A HAND  %luMS", (unsigned long)f.ges.npu_ms);
        }
        ui_set_top_right(buf);
    }

    if (pg->prepare) {
        pg->prepare(&f);
    }
    if (s_bars_dirty) {
        draw_bars(&f.g);
        s_bars_dirty = false;
    }

    /* The classifier sees every preview frame, whatever page is up and even
     * while the camera page is frozen - it is the input device. */
    if (addr) {
        L1C_DCACHE_INVALID_RANGE(addr, (uint32_t)f.g.img_w * f.g.img_h * 2u);
        feed_gesture(&f.g, addr);
    }

    if (pg->hold && pg->hold()) {
        /* Frozen: the panel keeps what it shows; only the bars were updated. */
        s_stats.drawn++;
        s_stats.page = s_page;
        return;
    }

    const uint32_t *src = f.preview ? (const uint32_t *)(uintptr_t)addr : NULL;

    st7789p3_bus_lock();
    st7789p3_blit_start(f.g.ox, f.g.oy, (uint16_t)(f.g.ox + f.g.img_w - 1u),
                        (uint16_t)(f.g.oy + f.g.img_h - 1u));
    for (int y = 0; y < f.g.img_h; y++) {
        if (src) {
            evk_gfx_yuyv_row(src + (uint32_t)y * f.g.img_w / 2u, s_row, f.g.img_w,
                             s_uv_swap, s_mirror);
        } else {
            memset(s_row, 0, (size_t)f.g.img_w * sizeof(s_row[0]));
        }
        if (pg->row) {
            pg->row(&f, s_row, f.g.img_w, y);
        }
        hold_row(&f, s_row, f.g.img_w, y);
        st7789p3_blit_push(s_row, f.g.img_w);
    }
    st7789p3_blit_end();
    st7789p3_bus_unlock();

    {
        static TickType_t last;
        TickType_t now = xTaskGetTickCount();
        s_stats.blit_ms = (uint32_t)((now - t0) * portTICK_PERIOD_MS);
        if (last) {
            uint32_t period = (uint32_t)((now - last) * portTICK_PERIOD_MS);
            if (period) {
                uint32_t inst = 10000u / period;
                s_stats.fps_x10 = s_stats.fps_x10 ? (s_stats.fps_x10 * 3u + inst) / 4u : inst;
            }
        }
        last = now;
    }
    s_stats.drawn++;
    s_stats.page = s_page;
}

static void ui_task(void *arg)
{
    (void)arg;
    if (s_pages[s_page]->enter) {
        s_pages[s_page]->enter();
    }
    while (s_running) {
        /* Woken by the camera; without one, tick at the idle rate so the
         * pages that do not need it still draw. */
        xSemaphoreTake(s_sem, pdMS_TO_TICKS(UI_IDLE_PERIOD_MS));
        uint32_t addr = s_latest;
        s_latest = 0;
        if (s_running) {
            render(addr);
        }
    }
    s_task = NULL;
    vTaskDelete(NULL);
}

/* ---------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

void ui_splash(const char *line1, const char *line2)
{
    evk_gfx_splash(line1, line2);
    s_bars_dirty = true;
}

int ui_init(void)
{
    if (!st7789p3_is_ready()) {
        return -1;
    }
    input_init(&s_input, INPUT_HOLD_MS, INPUT_RELEASE_MS, INPUT_MIN_CONF);
    ui_splash("IPRO7AI EVK", "LOADING MODEL");
    return 0;
}

int ui_start(void)
{
    if (s_running) {
        return 0;
    }
    if (!s_sem) {
        s_sem = xSemaphoreCreateBinary();
        if (!s_sem) {
            return -1;
        }
    }
    s_latest = 0;
    s_bars_dirty = true;
    s_top_right[0] = s_bottom[0] = '\0';
    set_top_left_for_page();
    memset(&s_stats, 0, sizeof(s_stats));
    st7789p3_fill_screen(ST_BLACK);
    s_running = true;
    if (xTaskCreate(ui_task, "ui", UI_TASK_STACK, NULL, UI_TASK_PRIO, &s_task) != pdPASS) {
        s_running = false;
        return -1;
    }
    evk_cam_set_frame_cb(frame_cb, NULL);
    return 0;
}

void ui_stop(void)
{
    if (!s_running) {
        return;
    }
    evk_cam_set_frame_cb(NULL, NULL);
    s_running = false;
    if (s_sem) {
        xSemaphoreGive(s_sem);
    }
    for (int i = 0; i < 50 && s_task; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool ui_running(void)          { return s_running; }
void ui_set_mirror(bool on)    { s_mirror = on; }
bool ui_get_mirror(void)       { return s_mirror; }
void ui_set_uv_swap(bool on)   { s_uv_swap = on; }
bool ui_get_uv_swap(void)      { return s_uv_swap; }

void ui_input_config(uint32_t hold_ms, uint32_t release_ms, int min_conf)
{
    s_input_cfg_hold    = hold_ms;
    s_input_cfg_release = release_ms;
    s_input_cfg_conf    = min_conf;
    s_input_cfg_req     = true;          /* consumed by render() on the UI task */
}

void ui_get_stats(ui_stats_t *out)
{
    if (out) {
        *out = s_stats;
    }
}
