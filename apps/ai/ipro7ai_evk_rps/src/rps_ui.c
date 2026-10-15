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
 * @file rps_ui.c
 * @brief Game screen. See rps_ui.h.
 *
 * Row pipeline (per preview row): YUYV -> RGB565 -> ROI rectangle -> text
 * overlays that intersect this row -> panel. Same shape as the person
 * detection app's display.c; the overlays are what changed.
 */

#include "rps_ui.h"
#include "rps_game.h"
#include "rps_model.h"
#include "evk_cam.h"
#include "font5x7.h"
#include "st7789p3.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <system_soc.h>
#include "ipro_log.h"

#define LOG_TAG "RPS_UI"

/* Below the NPU task (see rps_model.c), above the shell (20). */
#define UI_TASK_STACK   1536
#define UI_TASK_PRIO    (configMAX_PRIORITIES - 6)

#define ROI_T           2                 /* box outline thickness */
#define MAX_CHARS       40

#define COL_BOX_IDLE    ST_WHITE
#define COL_BOX_ARMED   ST_YELLOW
#define COL_BOX_WIN     ST_GREEN
#define COL_BOX_LOSE    ST_RED
#define COL_BOX_DRAW    ST_CYAN
#define COL_TEXT        ST_WHITE
#define COL_TEXT_DIM    ST_GRAY
#define COL_BG          ST_BLACK

/* ---------------------------------------------------------------------------
 * Text lines composited into rows (wire-order colours)
 * -------------------------------------------------------------------------*/

typedef struct {
    char     text[MAX_CHARS + 1];
    int      len, scale;
    uint16_t fg;
    bool     backing;                /* black box behind the glyphs? */
    int      x, y, w, h;             /* padded box, in the row space */
} text_t;

#define PAD 2

static void text_set(text_t *t, const char *s, int scale, uint16_t fg, bool backing,
                     int x, int y)
{
    t->len = (int)strlen(s);
    if (t->len > MAX_CHARS) {
        t->len = MAX_CHARS;
    }
    memcpy(t->text, s, (size_t)t->len);
    t->text[t->len] = '\0';
    t->scale   = scale;
    t->fg      = st7789p3_swap(fg);
    t->backing = backing;
    t->x = x;
    t->y = y;
    t->w = t->len ? (t->len * FONT_ADVANCE - 1) * scale + 2 * PAD : 0;
    t->h = FONT_H * scale + 2 * PAD;
}

static int text_width(const char *s, int scale)
{
    int n = (int)strlen(s);
    return n ? (n * FONT_ADVANCE - 1) * scale + 2 * PAD : 0;
}

static void text_row(const text_t *t, uint16_t *row, int w, int y)
{
    int ry = y - t->y;

    if (!t->len || ry < 0 || ry >= t->h) {
        return;
    }
    if (t->backing) {
        const uint16_t bg = st7789p3_swap(COL_BG);
        int x0 = t->x < 0 ? 0 : t->x;
        int x1 = t->x + t->w > w ? w : t->x + t->w;
        for (int x = x0; x < x1; x++) {
            row[x] = bg;
        }
    }
    int gy = (ry - PAD) / t->scale;
    if (ry < PAD || gy >= FONT_H) {
        return;
    }
    for (int i = 0; i < t->len; i++) {
        const uint8_t *g  = font5x7_glyph(t->text[i]);
        int            gx = t->x + PAD + i * FONT_ADVANCE * t->scale;
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
 * State
 * -------------------------------------------------------------------------*/

static volatile bool     s_running;
static TaskHandle_t      s_task;
static SemaphoreHandle_t s_sem;
static volatile uint32_t s_latest;
static bool              s_uv_swap;
static bool              s_mirror = true;   /* selfie view: your right hand on the right */
static rps_ui_stats_t    s_stats;

static uint16_t s_row[ST_MAX_DIM];

/* Overlays on the preview image (image-row coordinates). */
static struct { int x1, y1, x2, y2; uint16_t color; } s_box;
static text_t s_live;                 /* "ROCK 92%" under the box top edge */
static text_t s_count;                /* big countdown digit / result move */

/* Bars, redrawn on change. */
static char s_top_left[MAX_CHARS + 1], s_top_right[MAX_CHARS + 1], s_bottom[MAX_CHARS + 1];
static bool s_bars_dirty = true;

/* ---------------------------------------------------------------------------
 * YUYV -> RGB565 (BT.601, two pixels per word, [Y0 U Y1 V])
 * -------------------------------------------------------------------------*/

static inline uint8_t clamp_u8(int v)
{
    return (uint8_t)(v < 0 ? 0 : (v > 255 ? 255 : v));
}

static void yuyv_row(const uint32_t *src, uint16_t *dst, int npix)
{
    const int cb_i = s_uv_swap ? 3 : 1, cr_i = s_uv_swap ? 1 : 3;
    /* Mirror = write the row right-to-left. The overlays are drawn afterwards
     * in screen coordinates, so text stays readable and the box stays put;
     * the NPU sees the unmirrored ROI (the model was trained with flips). */
    const int step = s_mirror ? -1 : 1;
    int o = s_mirror ? npix - 1 : 0;

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
 * Bars
 * -------------------------------------------------------------------------*/

static void draw_bar(int y0, int h, const text_t *a, const text_t *b)
{
    uint16_t w = st7789p3_width();

    st7789p3_bus_lock();
    st7789p3_blit_start(0, (uint16_t)y0, (uint16_t)(w - 1u), (uint16_t)(y0 + h - 1));
    for (int y = y0; y < y0 + h; y++) {
        memset(s_row, 0, (size_t)w * sizeof(s_row[0]));
        if (a) text_row(a, s_row, w, y);
        if (b) text_row(b, s_row, w, y);
        st7789p3_blit_push(s_row, w);
    }
    st7789p3_blit_end();
    st7789p3_bus_unlock();
}

static void draw_bars(const evk_cam_geometry_t *g)
{
    text_t l, r;
    int top_h = g->oy;
    int bot_y = g->oy + g->img_h;
    int bot_h = g->panel_h - bot_y;

    if (top_h > 0) {
        text_set(&l, s_top_left, 1, COL_TEXT, false, 4, (top_h - (FONT_H + 2 * PAD)) / 2);
        int rw = text_width(s_top_right, 2);
        text_set(&r, s_top_right, 2, COL_TEXT, false, g->panel_w - rw - 4,
                 (top_h - (FONT_H * 2 + 2 * PAD)) / 2);
        draw_bar(0, top_h, &l, &r);
    }
    if (bot_h > 0) {
        int bw = text_width(s_bottom, 2);
        text_set(&l, s_bottom, 2, COL_TEXT, false, (g->panel_w - bw) / 2,
                 bot_y + (bot_h - (FONT_H * 2 + 2 * PAD)) / 2);
        draw_bar(bot_y, bot_h, &l, NULL);
    }
}

/* ---------------------------------------------------------------------------
 * Per-frame overlay preparation
 * -------------------------------------------------------------------------*/

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

static void prepare(const evk_cam_geometry_t *g, const rps_game_view_t *v,
                    const rps_result_t *r)
{
    char buf[MAX_CHARS + 1];
    uint16_t col = COL_BOX_IDLE;

    /* The play box = the sensor ROI that feeds the NPU. */
    s_box.x1 = map_x(g, g->roi_x);
    s_box.x2 = map_x(g, g->roi_x + g->roi_w - 1);
    s_box.y1 = map_y(g, g->roi_y);
    s_box.y2 = map_y(g, g->roi_y + g->roi_h - 1);

    switch (v->state) {
    case RPS_ST_COUNTDOWN:
    case RPS_ST_CAPTURE: col = COL_BOX_ARMED; break;
    case RPS_ST_RESULT:
    case RPS_ST_COOLDOWN:
        col = v->outcome == RPS_OUTCOME_PLAYER ? COL_BOX_WIN
            : v->outcome == RPS_OUTCOME_BOARD  ? COL_BOX_LOSE
            : v->outcome == RPS_OUTCOME_DRAW   ? COL_BOX_DRAW : COL_BOX_IDLE;
        break;
    default: break;
    }
    s_box.color = st7789p3_swap(col);

    /* Live label just inside the top edge of the box. */
    if (!rps_model_ready()) {
        snprintf(buf, sizeof(buf), "NO MODEL: %s", rps_model_fail_reason());
        text_set(&s_live, buf, 1, ST_YELLOW, true, s_box.x1 + ROI_T + 1, s_box.y1 + ROI_T + 1);
    } else if (v->live_cls != RPS_NONE) {
        snprintf(buf, sizeof(buf), "%s %u%%", rps_class_name(v->live_cls), v->live_conf);
        text_set(&s_live, buf, 1, COL_TEXT, true, s_box.x1 + ROI_T + 1, s_box.y1 + ROI_T + 1);
    } else {
        s_live.len = 0;
    }

    /* Big centred text inside the box: the countdown digit, or the move. */
    const char *big = NULL;
    int scale = 5;
    if (v->state == RPS_ST_COUNTDOWN) {
        snprintf(buf, sizeof(buf), "%d", v->countdown);
        big = buf;
    } else if (v->state == RPS_ST_CAPTURE) {
        big = "GO";
        scale = 4;
    } else if ((v->state == RPS_ST_RESULT || v->state == RPS_ST_COOLDOWN) &&
               v->player != RPS_NONE) {
        big = rps_class_name(v->player);
        scale = 2;
    }
    if (big) {
        int bw = text_width(big, scale), bh = FONT_H * scale + 2 * PAD;
        int cx = (s_box.x1 + s_box.x2) / 2, cy = (s_box.y1 + s_box.y2) / 2;
        text_set(&s_count, big, scale, COL_TEXT, true, cx - bw / 2, cy - bh / 2);
    } else {
        s_count.len = 0;
    }

    /* Bars. */
    snprintf(buf, sizeof(buf), "YOU %u  ME %u  DRAW %u", v->wins, v->losses, v->draws);
    if (strcmp(buf, s_top_left)) { strcpy(s_top_left, buf); s_bars_dirty = true; }

    if ((v->state == RPS_ST_RESULT || v->state == RPS_ST_COOLDOWN) && v->board != RPS_NONE) {
        snprintf(buf, sizeof(buf), "ME: %s", rps_class_name(v->board));
    } else if (rps_model_ready()) {
        snprintf(buf, sizeof(buf), "NPU %luMS", (unsigned long)r->npu_ms);
    } else {
        strcpy(buf, "NO MODEL");
    }
    if (strcmp(buf, s_top_right)) { strcpy(s_top_right, buf); s_bars_dirty = true; }

    const char *msg = v->message ? v->message : "";
    if (strcmp(msg, s_bottom)) { strncpy(s_bottom, msg, MAX_CHARS); s_bottom[MAX_CHARS] = 0; s_bars_dirty = true; }
}

static void overlay_row(uint16_t *row, int w, int y)
{
    (void)w;
    if (y >= s_box.y1 && y <= s_box.y2) {
        if (y - s_box.y1 < ROI_T || s_box.y2 - y < ROI_T) {
            for (int x = s_box.x1; x <= s_box.x2; x++) {
                row[x] = s_box.color;
            }
        } else {
            for (int t = 0; t < ROI_T; t++) {
                row[s_box.x1 + t] = s_box.color;
                row[s_box.x2 - t] = s_box.color;
            }
        }
    }
    text_row(&s_live, row, w, y);
    text_row(&s_count, row, w, y);
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

static void render(uint32_t addr)
{
    evk_cam_geometry_t  g;
    rps_result_t    r;
    rps_game_view_t v;
    TickType_t      t0 = xTaskGetTickCount();

    evk_cam_get_geometry(&g);
    rps_model_get_result(&r);
    rps_game_tick((uint32_t)(t0 * portTICK_PERIOD_MS), &r);
    rps_game_get_view(&v);
    prepare(&g, &v, &r);

    /* One console line per finished round, for the bench. */
    {
        static uint16_t logged_round;
        if (v.state == RPS_ST_RESULT && v.round != logged_round) {
            logged_round = v.round;
            printf("[GAME] round %u: you %s, me %s -> %s  (you %u me %u draw %u)\r\n",
                   v.round, rps_class_name(v.player), rps_class_name(v.board),
                   v.outcome == RPS_OUTCOME_PLAYER ? "YOU WIN" :
                   v.outcome == RPS_OUTCOME_BOARD  ? "I WIN" : "DRAW",
                   v.wins, v.losses, v.draws);
        }
    }

    if (s_bars_dirty) {
        draw_bars(&g);
        s_bars_dirty = false;
    }

    L1C_DCACHE_INVALID_RANGE(addr, (uint32_t)g.img_w * g.img_h * 2u);
    const uint32_t *src = (const uint32_t *)(uintptr_t)addr;

    st7789p3_bus_lock();
    st7789p3_blit_start(g.ox, g.oy, (uint16_t)(g.ox + g.img_w - 1u),
                        (uint16_t)(g.oy + g.img_h - 1u));
    for (int y = 0; y < g.img_h; y++) {
        yuyv_row(src + (uint32_t)y * g.img_w / 2u, s_row, g.img_w);
        overlay_row(s_row, g.img_w, y);
        st7789p3_blit_push(s_row, g.img_w);
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
}

static void ui_task(void *arg)
{
    (void)arg;
    while (s_running) {
        if (xSemaphoreTake(s_sem, pdMS_TO_TICKS(500)) != pdTRUE) {
            continue;
        }
        uint32_t addr = s_latest;
        s_latest = 0;
        if (addr && s_running) {
            render(addr);
        }
    }
    s_task = NULL;
    vTaskDelete(NULL);
}

/* ---------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

void rps_ui_splash(const char *line1, const char *line2)
{
    text_t a, b;
    uint16_t w = st7789p3_width(), h = st7789p3_height();
    int aw = text_width(line1 ? line1 : "", 2), bw = text_width(line2 ? line2 : "", 1);
    int ah = FONT_H * 2 + 2 * PAD, bh = FONT_H + 2 * PAD;
    int y0 = (h - (ah + 2 + bh)) / 2;

    text_set(&a, line1 ? line1 : "", 2, COL_TEXT, false, (w - aw) / 2, y0);
    text_set(&b, line2 ? line2 : "", 1, COL_TEXT_DIM, false, (w - bw) / 2, y0 + ah + 2);

    st7789p3_bus_lock();
    st7789p3_fill_screen(ST_BLACK);
    draw_bar(y0, ah + 2 + bh, &a, &b);
    st7789p3_bus_unlock();
    s_bars_dirty = true;
}

int rps_ui_init(void)
{
    if (st7789p3_init(ST7789P3_SEQ_MINIMAL, 1) != ST7789P3_OK) {
        IPRO_LOGE(LOG_TAG, "panel init failed");
        return -1;
    }
    rps_ui_splash("ROCK PAPER SCISSORS", "IPRO7AI EVK  LOADING MODEL");
    return 0;
}

int rps_ui_start(void)
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
    s_top_left[0] = s_top_right[0] = s_bottom[0] = '\0';
    memset(&s_stats, 0, sizeof(s_stats));
    s_running = true;
    if (xTaskCreate(ui_task, "rps_ui", UI_TASK_STACK, NULL, UI_TASK_PRIO, &s_task) != pdPASS) {
        s_running = false;
        return -1;
    }
    evk_cam_set_frame_cb(frame_cb, NULL);
    return 0;
}

void rps_ui_stop(void)
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

bool rps_ui_running(void)         { return s_running; }
void rps_ui_set_uv_swap(bool on)  { s_uv_swap = on; }
bool rps_ui_get_uv_swap(void)     { return s_uv_swap; }
void rps_ui_set_mirror(bool on)   { s_mirror = on; }
bool rps_ui_get_mirror(void)      { return s_mirror; }

void rps_ui_get_stats(rps_ui_stats_t *out)
{
    if (out) {
        *out = s_stats;
    }
}
