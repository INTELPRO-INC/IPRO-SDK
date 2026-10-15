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
 * @file display.c
 * @brief Preview + detection overlay on the ST7789P3 panel. See display.h.
 */

#include "display.h"
#include "evk_cam.h"
#include "detect.h"
#include "font5x7.h"
#include "st7789p3.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <system_soc.h>    /* L1C_DCACHE_INVALID_RANGE */
#include "ipro_log.h"

#define LOG_TAG "DISP"

/* Below the detector (see detect.c) and above the shell (20): the polling
 * SPI driver spins for the whole blit, so this is the task that everyone
 * else has to be able to preempt. */
#define DISP_TASK_STACK   1536
#define DISP_TASK_PRIO    (configMAX_PRIORITIES - 6)

/* Box outline thickness in preview pixels. */
#define BOX_T             2

/* HUD: line 1 at 2x (10x14 px glyphs), line 2 at 1x, both on a black backing
 * in the top-left corner of the preview image. */
#define HUD_X             4
#define HUD_Y             3
#define HUD_PAD           2
#define HUD_SCALE1        2
#define HUD_SCALE2        1
#define HUD_LINE_GAP      2
#define HUD_MAX_CHARS     40

/* Colours, RGB565 host order (swapped once when stored in the row). */
#define COL_PERSON        ST_GREEN
#define COL_PET           ST_YELLOW
#define COL_HUD_FG        ST_WHITE
#define COL_HUD_WARN      ST_YELLOW
#define COL_HUD_BG        ST_BLACK

/* ---------------------------------------------------------------------------
 * State
 * -------------------------------------------------------------------------*/

typedef struct {
    int16_t  x1, y1, x2, y2;       /* preview pixels, clipped, x1<=x2      */
    uint16_t color;                /* wire order                           */
} disp_box_t;

typedef struct {
    char     text[HUD_MAX_CHARS + 1];
    int      len;
    int      scale;
    uint16_t fg;                   /* wire order */
    int      x, y, w, h;           /* pixel box incl. padding, image coords */
} hud_line_t;

static volatile bool      s_running;
static TaskHandle_t       s_task;
static SemaphoreHandle_t  s_frame_sem;
static volatile uint32_t  s_latest;

static bool s_uv_swap;
static bool s_pets = true;
static bool s_hud  = true;

static display_stats_t s_stats;

/* Per-frame scratch, static so the task stack stays small. */
static uint16_t     s_row[ST_MAX_DIM];
static disp_box_t   s_boxes[DETECT_MAX];
static int          s_nboxes;
static hud_line_t   s_hud1, s_hud2;
static uint32_t     s_hud_seq = 0xFFFFFFFFu;    /* result seq the HUD shows */

/* ---------------------------------------------------------------------------
 * YUYV -> RGB565
 * -------------------------------------------------------------------------*/

static inline uint8_t clamp_u8(int v)
{
    return (uint8_t)(v < 0 ? 0 : (v > 255 ? 255 : v));
}

/*
 * BT.601 full-range, fixed point (x256):
 *   R = Y + 1.402 (V-128)
 *   G = Y - 0.344 (U-128) - 0.714 (V-128)
 *   B = Y + 1.772 (U-128)
 * Two pixels per 32-bit word, [Y0 U Y1 V] on the wire (uvc_cam's verified
 * channel-0 layout); the swap knob exchanges the chroma bytes.
 */
static void yuyv_row_to_rgb565(const uint32_t *src, uint16_t *dst, int npix)
{
    const int cb_idx = s_uv_swap ? 3 : 1;
    const int cr_idx = s_uv_swap ? 1 : 3;

    for (int i = 0; i < npix; i += 2) {
        uint32_t w  = *src++;
        int      y0 = (int)(w & 0xFFu);
        int      y1 = (int)((w >> 16) & 0xFFu);
        int      cb = (int)((w >> (8 * cb_idx)) & 0xFFu) - 128;
        int      cr = (int)((w >> (8 * cr_idx)) & 0xFFu) - 128;
        int      dr = (359 * cr) >> 8;
        int      dg = (88 * cb + 183 * cr) >> 8;
        int      db = (454 * cb) >> 8;

        dst[i]     = st7789p3_swap(ST_RGB565(clamp_u8(y0 + dr),
                                             clamp_u8(y0 - dg),
                                             clamp_u8(y0 + db)));
        dst[i + 1] = st7789p3_swap(ST_RGB565(clamp_u8(y1 + dr),
                                             clamp_u8(y1 - dg),
                                             clamp_u8(y1 + db)));
    }
}

/* ---------------------------------------------------------------------------
 * Boxes
 * -------------------------------------------------------------------------*/

/* Model pixel (640x360 = sensor/2) -> preview pixel through the crop window. */
static int16_t map_x(const evk_cam_geometry_t *g, int mx)
{
    int sx = mx * 2 - (int)g->crop_x;
    int px = (sx * (int)g->img_w) / (int)g->crop_w;
    return (int16_t)(px < 0 ? 0 : (px >= g->img_w ? g->img_w - 1 : px));
}

static int16_t map_y(const evk_cam_geometry_t *g, int my)
{
    int sy = my * 2 - (int)g->crop_y;
    int py = (sy * (int)g->img_h) / (int)g->crop_h;
    return (int16_t)(py < 0 ? 0 : (py >= g->img_h ? g->img_h - 1 : py));
}

static void build_boxes(const detect_result_t *r, const evk_cam_geometry_t *g)
{
    s_nboxes = 0;
    for (int i = 0; i < r->count && s_nboxes < DETECT_MAX; i++) {
        const detect_box_t *b = &r->box[i];
        disp_box_t *d = &s_boxes[s_nboxes];

        if (b->cls == DETECT_CLASS_PET && !s_pets) {
            continue;
        }
        d->x1 = map_x(g, b->x1);
        d->x2 = map_x(g, b->x2);
        d->y1 = map_y(g, b->y1);
        d->y2 = map_y(g, b->y2);
        if (d->x2 - d->x1 < BOX_T || d->y2 - d->y1 < BOX_T) {
            continue;                      /* fully outside the crop, or a dot */
        }
        d->color = st7789p3_swap(b->cls == DETECT_CLASS_PERSON ? COL_PERSON
                                                                : COL_PET);
        s_nboxes++;
    }
}

static void overlay_boxes_row(uint16_t *row, int y)
{
    for (int i = 0; i < s_nboxes; i++) {
        const disp_box_t *b = &s_boxes[i];

        if (y < b->y1 || y > b->y2) {
            continue;
        }
        if (y - b->y1 < BOX_T || b->y2 - y < BOX_T) {
            for (int x = b->x1; x <= b->x2; x++) {
                row[x] = b->color;             /* top / bottom edge */
            }
        } else {
            for (int t = 0; t < BOX_T; t++) {  /* left / right edge */
                row[b->x1 + t] = b->color;
                row[b->x2 - t] = b->color;
            }
        }
    }
}

/* ---------------------------------------------------------------------------
 * HUD text
 * -------------------------------------------------------------------------*/

static void hud_set(hud_line_t *l, const char *text, int scale, uint16_t fg,
                    int x, int y)
{
    l->len = (int)strlen(text);
    if (l->len > HUD_MAX_CHARS) {
        l->len = HUD_MAX_CHARS;
    }
    memcpy(l->text, text, (size_t)l->len);
    l->text[l->len] = '\0';
    l->scale = scale;
    l->fg    = st7789p3_swap(fg);
    l->x     = x;
    l->y     = y;
    l->w     = l->len ? (l->len * FONT_ADVANCE - 1) * scale + 2 * HUD_PAD : 0;
    l->h     = FONT_H * scale + 2 * HUD_PAD;
}

/* Composite one text line into a row of the image (row y, width w). */
static void hud_compose_row(const hud_line_t *l, uint16_t *row, int w, int y)
{
    const uint16_t bg = st7789p3_swap(COL_HUD_BG);
    int ry = y - l->y;                 /* row within the padded box */

    if (!l->len || ry < 0 || ry >= l->h) {
        return;
    }
    int x_beg = l->x < 0 ? 0 : l->x;      /* a line wider than the panel */
    int x_end = l->x + l->w;
    if (x_end > w) {
        x_end = w;
    }
    for (int x = x_beg; x < x_end; x++) {
        row[x] = bg;
    }
    int gy = (ry - HUD_PAD) / l->scale;            /* glyph row 0..6 */
    if (ry < HUD_PAD || gy >= FONT_H) {
        return;                                    /* padding rows */
    }
    for (int i = 0; i < l->len; i++) {
        const uint8_t *g  = font5x7_glyph(l->text[i]);
        int            gx = l->x + HUD_PAD + i * FONT_ADVANCE * l->scale;

        for (int c = 0; c < FONT_W; c++) {
            if (!font5x7_pixel(g, c, gy)) {
                continue;
            }
            for (int s = 0; s < l->scale; s++) {
                int x = gx + c * l->scale + s;
                if (x >= 0 && x < w) {
                    row[x] = l->fg;
                }
            }
        }
    }
}

static void hud_update(const detect_result_t *r)
{
    /* Wider than HUD_MAX_CHARS so snprintf's worst case (%lu x4) fits;
     * hud_set() clips to what the panel can show. */
    char line[64];
    detect_stats_t ds;
    int y1 = HUD_Y;
    int y2;

    if (!detect_is_ready()) {
        hud_set(&s_hud1, "NO MODEL", HUD_SCALE1, COL_HUD_WARN, HUD_X, y1);
        y2 = y1 + s_hud1.h + HUD_LINE_GAP;
        hud_set(&s_hud2, detect_fail_reason(), HUD_SCALE2, COL_HUD_WARN, HUD_X, y2);
        return;
    }
    if (!evk_cam_npu_enabled()) {
        hud_set(&s_hud1, "DETECT OFF", HUD_SCALE1, COL_HUD_WARN, HUD_X, y1);
        y2 = y1 + s_hud1.h + HUD_LINE_GAP;
        hud_set(&s_hud2, "det start", HUD_SCALE2, COL_HUD_FG, HUD_X, y2);
        return;
    }

    detect_get_stats(&ds);
    snprintf(line, sizeof(line), "PERSON %u  PET %u",
             (unsigned)r->persons, (unsigned)r->pets);
    hud_set(&s_hud1, line, HUD_SCALE1, COL_HUD_FG, HUD_X, y1);
    y2 = y1 + s_hud1.h + HUD_LINE_GAP;
    snprintf(line, sizeof(line), "NPU %luMS  %lu.%luFPS  LCD %luFPS",
             (unsigned long)r->npu_ms,
             (unsigned long)(ds.fps_x10 / 10u), (unsigned long)(ds.fps_x10 % 10u),
             (unsigned long)(s_stats.fps_x10 / 10u));
    hud_set(&s_hud2, line, HUD_SCALE2, COL_HUD_FG, HUD_X, y2);
}

/* ---------------------------------------------------------------------------
 * Splash (no camera frame involved)
 * -------------------------------------------------------------------------*/

void display_splash(const char *line1, const char *line2)
{
    hud_line_t l1, l2;
    uint16_t w = st7789p3_width(), h = st7789p3_height();
    int y_start;

    hud_set(&l1, line1 ? line1 : "", 2, COL_HUD_FG, 0, 0);
    hud_set(&l2, line2 ? line2 : "", 1, COL_HUD_FG, 0, 0);
    l1.x = (w - l1.w) / 2;
    l2.x = (w - l2.w) / 2;
    y_start = (h - (l1.h + HUD_LINE_GAP + l2.h)) / 2;
    l1.y = y_start;
    l2.y = y_start + l1.h + HUD_LINE_GAP;

    st7789p3_bus_lock();
    st7789p3_fill_screen(ST_BLACK);
    st7789p3_blit_start(0, (uint16_t)l1.y, (uint16_t)(w - 1u),
                        (uint16_t)(l2.y + l2.h - 1));
    for (int y = l1.y; y < l2.y + l2.h; y++) {
        memset(s_row, 0, (size_t)w * sizeof(s_row[0]));  /* black, any order */
        hud_compose_row(&l1, s_row, w, y);
        hud_compose_row(&l2, s_row, w, y);
        st7789p3_blit_push(s_row, w);                    /* already wire order */
    }
    st7789p3_blit_end();
    st7789p3_bus_unlock();
}

/* ---------------------------------------------------------------------------
 * Render task
 * -------------------------------------------------------------------------*/

static void frame_cb(uint32_t addr, void *arg)
{
    BaseType_t woken = pdFALSE;

    (void)arg;
    if (s_latest) {
        s_stats.dropped++;          /* render is slower than the ISP */
    }
    s_latest = addr;
    if (s_frame_sem) {
        xSemaphoreGiveFromISR(s_frame_sem, &woken);
        portYIELD_FROM_ISR(woken);
    }
}

static void render_frame(uint32_t addr)
{
    evk_cam_geometry_t  g;
    detect_result_t r;
    TickType_t      t0 = xTaskGetTickCount();
    const uint32_t *src;

    evk_cam_get_geometry(&g);
    detect_get_result(&r);
    build_boxes(&r, &g);
    if (r.seq != s_hud_seq || !detect_is_ready() || !evk_cam_npu_enabled()) {
        hud_update(&r);
        s_hud_seq = r.seq;
    }

    /* The ISP wrote this frame by DMA; drop any stale cache lines first. */
    L1C_DCACHE_INVALID_RANGE(addr, (uint32_t)g.img_w * g.img_h * 2u);
    src = (const uint32_t *)(uintptr_t)addr;

    st7789p3_bus_lock();
    st7789p3_blit_start(g.ox, g.oy,
                        (uint16_t)(g.ox + g.img_w - 1u),
                        (uint16_t)(g.oy + g.img_h - 1u));
    for (int y = 0; y < g.img_h; y++) {
        yuyv_row_to_rgb565(src + (uint32_t)y * g.img_w / 2u, s_row, g.img_w);
        overlay_boxes_row(s_row, y);
        if (s_hud) {
            hud_compose_row(&s_hud1, s_row, g.img_w, y);
            hud_compose_row(&s_hud2, s_row, g.img_w, y);
        }
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
                s_stats.fps_x10 = s_stats.fps_x10
                                ? (s_stats.fps_x10 * 3u + inst) / 4u : inst;
            }
        }
        last = now;
    }
    s_stats.drawn++;
}

static void display_task(void *arg)
{
    (void)arg;

    while (s_running) {
        if (xSemaphoreTake(s_frame_sem, pdMS_TO_TICKS(500)) != pdTRUE) {
            continue;               /* no frame - re-check s_running */
        }
        uint32_t addr = s_latest;
        s_latest = 0;
        if (addr && s_running) {
            render_frame(addr);
        }
    }

    s_task = NULL;
    vTaskDelete(NULL);
}

/* ---------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

int display_init(void)
{
    if (st7789p3_init(ST7789P3_SEQ_MINIMAL, 1) != ST7789P3_OK) {
        IPRO_LOGE(LOG_TAG, "panel init failed");
        return -1;
    }
    display_splash("PERSON DETECT", "IPRO7AI EVK  LOADING MODEL");
    return 0;
}

int display_start(void)
{
    if (s_running) {
        return 0;
    }
    if (!s_frame_sem) {
        s_frame_sem = xSemaphoreCreateBinary();
        if (!s_frame_sem) {
            return -1;
        }
    }
    s_latest  = 0;
    s_hud_seq = 0xFFFFFFFFu;
    memset(&s_stats, 0, sizeof(s_stats));
    s_running = true;
    if (xTaskCreate(display_task, "display", DISP_TASK_STACK, NULL,
                    DISP_TASK_PRIO, &s_task) != pdPASS) {
        s_running = false;
        IPRO_LOGE(LOG_TAG, "render task create failed");
        return -1;
    }
    evk_cam_set_frame_cb(frame_cb, NULL);
    return 0;
}

void display_stop(void)
{
    if (!s_running) {
        return;
    }
    evk_cam_set_frame_cb(NULL, NULL);
    s_running = false;
    if (s_frame_sem) {
        xSemaphoreGive(s_frame_sem);
    }
    for (int i = 0; i < 50 && s_task; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool display_is_running(void)      { return s_running; }
void display_set_uv_swap(bool on)  { s_uv_swap = on; }
bool display_get_uv_swap(void)     { return s_uv_swap; }
void display_set_pets(bool on)     { s_pets = on; }
bool display_get_pets(void)        { return s_pets; }
void display_set_hud(bool on)      { s_hud = on; }
bool display_get_hud(void)         { return s_hud; }

void display_get_stats(display_stats_t *out)
{
    if (out) {
        *out = s_stats;
    }
}
