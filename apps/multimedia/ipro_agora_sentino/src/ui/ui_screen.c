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
 * @file ui_screen.c
 * @brief Main screen layout — 240x320 portrait
 *
 * Layout (top to bottom):
 *   [Status bar 32px]    — connection dots + "Sentino AI"
 *   [Anim area 160x160]  — gray placeholder rect (Phase 4: Monk image)
 *   [AI state 24px]       — color-coded IDLE/LISTENING/THINKING/SPEAKING
 *   [TTS text ~96px]      — scrollable Chinese subtitle area
 */

#include <generated/autoconf.h>

#ifdef CONFIG_APP_LCD_ENABLE

#include "ui_screen.h"
#include "ui_theme.h"
#ifdef CONFIG_AGENT_MONK
#include "ui_monk.h"
#endif
#include "lvgl/lvgl.h"
#ifndef CONFIG_AGENT_MONK
#include "lv_port_tick.h"
#endif

/* Chinese font — SDK built-in Source Han Sans SC 16px CJK (const, in flash) */
extern const lv_font_t lv_font_source_han_sans_sc_16_cjk;
/* Supplementary Buddhist vocabulary font (fallback for chars missing in SDK font) */
extern const lv_font_t lv_font_buddhist_16;
/* Writable copy of SDK font descriptor with fallback chain attached */
static lv_font_t s_cjk_font;

/* ------------------------------------------------------------------ */
/* Widget handles                                                      */
/* ------------------------------------------------------------------ */

static lv_obj_t *s_status_bar;
static lv_obj_t *s_lbl_mqtt_dot;
static lv_obj_t *s_lbl_agora_dot;
static lv_obj_t *s_lbl_title;
static lv_obj_t *s_lbl_time;
static lv_obj_t *s_anim_area;
static lv_obj_t *s_lbl_ai_state;
static lv_obj_t *s_lbl_tts;
#ifndef CONFIG_AGENT_MONK
static lv_obj_t *s_lbl_fps;
#endif

/* ------------------------------------------------------------------ */
/* Status bar                                                          */
/* ------------------------------------------------------------------ */

static void create_status_bar(lv_obj_t *parent)
{
    s_status_bar = lv_obj_create(parent);
    lv_obj_set_size(s_status_bar, UI_SCR_W, UI_STATUSBAR_H);
    lv_obj_set_pos(s_status_bar, 0, 0);
    lv_obj_set_style_bg_color(s_status_bar, lv_color_hex(UI_STATUSBAR_BG), 0);
    lv_obj_set_style_bg_opa(s_status_bar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(s_status_bar, 0, 0);
    lv_obj_set_style_radius(s_status_bar, 0, 0);
    lv_obj_set_style_pad_all(s_status_bar, 4, 0);
    lv_obj_remove_flag(s_status_bar, LV_OBJ_FLAG_SCROLLABLE);

    /* MQTT connection dot */
    s_lbl_mqtt_dot = lv_label_create(s_status_bar);
    lv_label_set_text(s_lbl_mqtt_dot, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(s_lbl_mqtt_dot, lv_color_hex(UI_COLOR_DISCONN), 0);
    lv_obj_set_style_text_font(s_lbl_mqtt_dot, &lv_font_montserrat_14, 0);
    lv_obj_align(s_lbl_mqtt_dot, LV_ALIGN_LEFT_MID, 4, 0);

    /* Agora connection dot */
    s_lbl_agora_dot = lv_label_create(s_status_bar);
    lv_label_set_text(s_lbl_agora_dot, LV_SYMBOL_AUDIO);
    lv_obj_set_style_text_color(s_lbl_agora_dot, lv_color_hex(UI_COLOR_DISCONN), 0);
    lv_obj_set_style_text_font(s_lbl_agora_dot, &lv_font_montserrat_14, 0);
    lv_obj_align(s_lbl_agora_dot, LV_ALIGN_LEFT_MID, 24, 0);

    /* Title */
    s_lbl_title = lv_label_create(s_status_bar);
#ifdef CONFIG_AGENT_MONK
    lv_label_set_text(s_lbl_title, "Little Monk");
#else
    lv_label_set_text(s_lbl_title, "AI Assistant");
#endif
    lv_obj_set_style_text_color(s_lbl_title, lv_color_hex(UI_COLOR_TEXT), 0);
    lv_obj_set_style_text_font(s_lbl_title, &lv_font_montserrat_14, 0);
    lv_obj_align(s_lbl_title, LV_ALIGN_CENTER, 0, 0);

    /* Time display */
    s_lbl_time = lv_label_create(s_status_bar);
    lv_label_set_text(s_lbl_time, "--:--");
    lv_obj_set_style_text_color(s_lbl_time, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(s_lbl_time, &lv_font_montserrat_14, 0);
    lv_obj_align(s_lbl_time, LV_ALIGN_RIGHT_MID, -4, 0);
}

/* ------------------------------------------------------------------ */
/* Animation placeholder area                                          */
/* ------------------------------------------------------------------ */

/* Forward declaration — defined later in create_ai_state section */
static const uint32_t s_state_colors[];

/* ---- General agent: lightweight ring animation ---- */
#ifndef CONFIG_AGENT_MONK

/*
 * Uses pure lv_obj circles (border-radius) instead of lv_arc.
 * Rendering cost: ~2ms vs ~40ms for arcs (no trig, no anti-aliasing).
 *
 * Visual:
 *   [Outer ring]  140px — border only, pulsing size (breathing)
 *   [Middle ring] 100px — border only, pulsing opacity (opposite phase)
 *   [Core dot]     30px — solid color
 */

static lv_obj_t *s_ring_outer;
static lv_obj_t *s_ring_middle;
static lv_obj_t *s_core_dot;

/* Outer ring: pulsing size 130↔150 */
static void outer_pulse_cb(void *obj, int32_t val)
{
    lv_obj_set_size((lv_obj_t *)obj, val, val);
    lv_obj_center((lv_obj_t *)obj);
}

/* Middle ring: pulsing opacity 60↔200 */
static void middle_opa_cb(void *obj, int32_t val)
{
    lv_obj_set_style_border_opa((lv_obj_t *)obj, (lv_opa_t)val, 0);
}

static lv_anim_t s_anim_outer, s_anim_middle;

static lv_obj_t *create_ring(lv_obj_t *parent, int size, int border_w,
                              uint32_t color, lv_opa_t opa)
{
    lv_obj_t *ring = lv_obj_create(parent);
    lv_obj_remove_style_all(ring);
    lv_obj_set_size(ring, size, size);
    lv_obj_center(ring);
    lv_obj_set_style_radius(ring, 4, 0);  /* small radius = fast render, tech look */
    lv_obj_set_style_bg_opa(ring, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(ring, lv_color_hex(color), 0);
    lv_obj_set_style_border_width(ring, border_w, 0);
    lv_obj_set_style_border_opa(ring, opa, 0);
    lv_obj_remove_flag(ring, LV_OBJ_FLAG_SCROLLABLE);
    return ring;
}

static void start_ring_animations(uint32_t speed)
{
    /* Outer: breathing size */
    lv_anim_init(&s_anim_outer);
    lv_anim_set_var(&s_anim_outer, s_ring_outer);
    lv_anim_set_values(&s_anim_outer, 120, 148);
    lv_anim_set_duration(&s_anim_outer, speed);
    lv_anim_set_exec_cb(&s_anim_outer, outer_pulse_cb);
    lv_anim_set_playback_duration(&s_anim_outer, speed);
    lv_anim_set_repeat_count(&s_anim_outer, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&s_anim_outer);

    /* Middle: breathing opacity (opposite phase — start high) */
    lv_anim_init(&s_anim_middle);
    lv_anim_set_var(&s_anim_middle, s_ring_middle);
    lv_anim_set_values(&s_anim_middle, 200, 40);
    lv_anim_set_duration(&s_anim_middle, speed * 3 / 4);
    lv_anim_set_exec_cb(&s_anim_middle, middle_opa_cb);
    lv_anim_set_playback_duration(&s_anim_middle, speed * 3 / 4);
    lv_anim_set_repeat_count(&s_anim_middle, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&s_anim_middle);
}

static void create_tech_anim(lv_obj_t *parent)
{
    /* Outer breathing ring */
    s_ring_outer = create_ring(parent, 140, 2, UI_COLOR_IDLE, LV_OPA_80);

    /* Middle breathing ring */
    s_ring_middle = create_ring(parent, 100, 3, UI_COLOR_IDLE, LV_OPA_COVER);

    /* Core block */
    s_core_dot = lv_obj_create(parent);
    lv_obj_remove_style_all(s_core_dot);
    lv_obj_set_size(s_core_dot, 30, 30);
    lv_obj_center(s_core_dot);
    lv_obj_set_style_radius(s_core_dot, 4, 0);  /* small radius, fast render */
    lv_obj_set_style_bg_color(s_core_dot, lv_color_hex(UI_COLOR_IDLE), 0);
    lv_obj_set_style_bg_opa(s_core_dot, LV_OPA_COVER, 0);
    lv_obj_remove_flag(s_core_dot, LV_OBJ_FLAG_SCROLLABLE);

    start_ring_animations(1500);
}

/** Update colors + animation speed when AI state changes.
 * LCD uses polling SPI (no DMA) so animation runs freely during audio. */
static void tech_anim_set_state(int state_idx)
{
    lv_color_t c = lv_color_hex(s_state_colors[state_idx]);

    if (s_ring_outer)
        lv_obj_set_style_border_color(s_ring_outer, c, 0);
    if (s_ring_middle)
        lv_obj_set_style_border_color(s_ring_middle, c, 0);
    if (s_core_dot)
        lv_obj_set_style_bg_color(s_core_dot, c, 0);

    static const uint32_t speeds[] = { 2000, 1200, 500, 1000 };
    lv_anim_delete(s_ring_outer, outer_pulse_cb);
    lv_anim_delete(s_ring_middle, middle_opa_cb);
    start_ring_animations(speeds[state_idx]);
}

#endif /* !CONFIG_AGENT_MONK */

static void create_anim_area(lv_obj_t *parent)
{
    s_anim_area = lv_obj_create(parent);
    lv_obj_remove_style_all(s_anim_area);
    lv_obj_set_size(s_anim_area, UI_ANIM_SIZE, UI_ANIM_SIZE);
    lv_obj_align(s_anim_area, LV_ALIGN_TOP_MID, 0, UI_ANIM_Y);
    lv_obj_set_style_bg_color(s_anim_area, lv_color_hex(UI_ANIM_BG), 0);
    lv_obj_set_style_bg_opa(s_anim_area, LV_OPA_COVER, 0);
    lv_obj_remove_flag(s_anim_area, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_clip_corner(s_anim_area, false, 0);

#ifdef CONFIG_AGENT_MONK
    ui_monk_create(s_anim_area);
#else
    create_tech_anim(s_anim_area);
#endif
}

/* ------------------------------------------------------------------ */
/* AI state label                                                      */
/* ------------------------------------------------------------------ */

static const uint32_t s_state_colors[] = {
    UI_COLOR_IDLE,
    UI_COLOR_LISTENING,
    UI_COLOR_THINKING,
    UI_COLOR_SPEAKING,
};

static const char *s_state_labels[] = {
    "IDLE",
    "LISTENING",
    "THINKING",
    "SPEAKING",
};

static void create_ai_state(lv_obj_t *parent)
{
    s_lbl_ai_state = lv_label_create(parent);
    lv_label_set_text(s_lbl_ai_state, "IDLE");
    lv_obj_set_style_text_color(s_lbl_ai_state, lv_color_hex(UI_COLOR_IDLE), 0);
    lv_obj_set_style_text_font(s_lbl_ai_state, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(s_lbl_ai_state, 0, UI_STATE_Y);
    lv_obj_set_width(s_lbl_ai_state, UI_SCR_W);
    lv_obj_set_style_text_align(s_lbl_ai_state, LV_TEXT_ALIGN_CENTER, 0);
}

/* ------------------------------------------------------------------ */
/* TTS text area (scrollable, Chinese font)                            */
/* ------------------------------------------------------------------ */

static void create_tts_area(lv_obj_t *parent)
{
    /* Container for scrollable text */
    lv_obj_t *tts_cont = lv_obj_create(parent);
    lv_obj_set_size(tts_cont, UI_SCR_W - 8, UI_TTS_H);
    lv_obj_set_pos(tts_cont, 4, UI_TTS_Y);
    lv_obj_set_style_bg_color(tts_cont, lv_color_hex(0x16162A), 0);
    lv_obj_set_style_bg_opa(tts_cont, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(tts_cont, 1, 0);
    lv_obj_set_style_border_color(tts_cont, lv_color_hex(0x333355), 0);
    lv_obj_set_style_radius(tts_cont, 4, 0);
    lv_obj_set_style_pad_all(tts_cont, 6, 0);
    /* Enable vertical scroll */
    lv_obj_add_flag(tts_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(tts_cont, LV_SCROLLBAR_MODE_AUTO);

    s_lbl_tts = lv_label_create(tts_cont);
    lv_label_set_text(s_lbl_tts, "");
    lv_obj_set_width(s_lbl_tts, UI_SCR_W - 8 - 12);
    lv_label_set_long_mode(s_lbl_tts, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(s_lbl_tts, lv_color_hex(UI_COLOR_TEXT), 0);
    /* Build CJK font with fallback: SDK built-in → Buddhist supplement.
     * SDK font is const (flash), so copy descriptor to RAM and attach fallback. */
    s_cjk_font = lv_font_source_han_sans_sc_16_cjk;  /* struct copy (~40 bytes) */
    s_cjk_font.fallback = &lv_font_buddhist_16;
    lv_obj_set_style_text_font(s_lbl_tts, &s_cjk_font, 0);
    lv_obj_set_style_text_line_space(s_lbl_tts, 4, 0);
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

#ifndef CONFIG_AGENT_MONK
static void fps_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    if (!s_lbl_fps) return;
    uint32_t fps = lv_port_tick_get_fps();
    char buf[12];
    lv_snprintf(buf, sizeof(buf), "%lu FPS", (unsigned long)fps);
    lv_label_set_text(s_lbl_fps, buf);
}
#endif

void ui_screen_create(void)
{
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(UI_COLOR_BG), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    create_status_bar(scr);
    create_anim_area(scr);
    create_ai_state(scr);
    create_tts_area(scr);

#ifndef CONFIG_AGENT_MONK
    /* FPS counter label — bottom-right corner */
    s_lbl_fps = lv_label_create(scr);
    lv_label_set_text(s_lbl_fps, "-- FPS");
    lv_obj_set_style_text_color(s_lbl_fps, lv_color_hex(0x555555), 0);
    lv_obj_set_style_text_font(s_lbl_fps, &lv_font_montserrat_14, 0);
    lv_obj_align(s_lbl_fps, LV_ALIGN_BOTTOM_RIGHT, -4, -2);

    /* Update FPS every second */
    lv_timer_create(fps_timer_cb, 1000, NULL);
#endif
}

void ui_screen_set_ai_state(rtc_ai_state_t state)
{
    if (!s_lbl_ai_state) return;
    int idx = (int)state;
    if (idx < 0 || idx > 3) idx = 0;

    lv_label_set_text(s_lbl_ai_state, s_state_labels[idx]);
    lv_obj_set_style_text_color(s_lbl_ai_state, lv_color_hex(s_state_colors[idx]), 0);

#ifdef CONFIG_AGENT_MONK
    ui_monk_set_state(state);
#else
    tech_anim_set_state(idx);
#endif
}

void ui_screen_set_tts_text(const char *utf8_text)
{
    if (!s_lbl_tts || !utf8_text) return;
    lv_label_set_text(s_lbl_tts, utf8_text);
    /* Scroll to bottom to show latest text */
    lv_obj_t *cont = lv_obj_get_parent(s_lbl_tts);
    if (cont) {
        lv_obj_scroll_to_y(cont, LV_COORD_MAX, LV_ANIM_ON);
    }
}

void ui_screen_set_mqtt_status(bool connected)
{
    if (!s_lbl_mqtt_dot) return;
    lv_obj_set_style_text_color(s_lbl_mqtt_dot,
        lv_color_hex(connected ? UI_COLOR_CONNECTED : UI_COLOR_DISCONN), 0);
}

void ui_screen_set_agora_status(bool connected)
{
    if (!s_lbl_agora_dot) return;
    lv_obj_set_style_text_color(s_lbl_agora_dot,
        lv_color_hex(connected ? UI_COLOR_CONNECTED : UI_COLOR_DISCONN), 0);
}

void ui_screen_set_time(const char *time_str)
{
    if (!s_lbl_time || !time_str) return;
    lv_label_set_text(s_lbl_time, time_str);
}

#endif /* CONFIG_APP_LCD_ENABLE */
