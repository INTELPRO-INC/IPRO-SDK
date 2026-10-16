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
 * @file ui_monk.c
 * @brief Monk character image + animation state machine
 *
 * Uses Y-position and opacity animations only — NO image scaling.
 * Scale transform conflicts with DMA SPI flush (cache coherency issue
 * with PSRAM draw buffers). Position/opacity changes don't trigger
 * LVGL's SW image transform, avoiding the corruption.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_APP_LCD_ENABLE

#include "ui_monk.h"
#include "monk_img.h"

/* ------------------------------------------------------------------ */
/* Animation parameters                                                */
/* ------------------------------------------------------------------ */

/* IDLE: gentle floating (Y bob up/down) */
#define IDLE_Y_MIN          0
#define IDLE_Y_MAX          (-8)   /* float up 8px */
#define IDLE_DURATION_MS    3000

/* LISTENING: faster bob + slight opacity pulse */
#define LISTEN_Y_MIN        0
#define LISTEN_Y_MAX        (-5)
#define LISTEN_OPA_MIN      210
#define LISTEN_OPA_MAX      255
#define LISTEN_DURATION_MS  1200

/* THINKING: opacity pulse (meditation) */
#define THINK_OPA_MIN       120
#define THINK_OPA_MAX       255
#define THINK_DURATION_MS   800

/* SPEAKING: energetic bob */
#define SPEAK_Y_MIN         0
#define SPEAK_Y_MAX         (-12)  /* bigger bounce */
#define SPEAK_DURATION_MS   600

/* ------------------------------------------------------------------ */
/* State                                                               */
/* ------------------------------------------------------------------ */

static lv_obj_t *s_img_monk;
static lv_anim_t s_anim_y;
static lv_anim_t s_anim_opa;
static rtc_ai_state_t s_cur_state = RTC_AI_STATE_IDLE;
static int32_t s_base_y;  /* original Y position */

/* ------------------------------------------------------------------ */
/* Animation callbacks                                                 */
/* ------------------------------------------------------------------ */

static void anim_y_cb(void *obj, int32_t val)
{
    lv_obj_set_y((lv_obj_t *)obj, s_base_y + val);
}

static void anim_opa_cb(void *obj, int32_t val)
{
    lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)val, 0);
}

/* ------------------------------------------------------------------ */
/* Stop all running animations                                         */
/* ------------------------------------------------------------------ */

static void stop_all_anims(void)
{
    lv_anim_delete(s_img_monk, anim_y_cb);
    lv_anim_delete(s_img_monk, anim_opa_cb);

    if (s_img_monk) {
        lv_obj_set_y(s_img_monk, s_base_y);
        lv_obj_set_style_opa(s_img_monk, LV_OPA_COVER, 0);
    }
}

/* ------------------------------------------------------------------ */
/* Animation helpers                                                   */
/* ------------------------------------------------------------------ */

static void start_y_anim(int32_t min_val, int32_t max_val,
                          uint32_t duration, lv_anim_path_cb_t path)
{
    lv_anim_init(&s_anim_y);
    lv_anim_set_var(&s_anim_y, s_img_monk);
    lv_anim_set_values(&s_anim_y, min_val, max_val);
    lv_anim_set_duration(&s_anim_y, duration);
    lv_anim_set_exec_cb(&s_anim_y, anim_y_cb);
    lv_anim_set_path_cb(&s_anim_y, path);
    lv_anim_set_playback_duration(&s_anim_y, duration);
    lv_anim_set_repeat_count(&s_anim_y, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&s_anim_y);
}

static void start_opa_anim(int32_t min_val, int32_t max_val,
                            uint32_t duration, lv_anim_path_cb_t path)
{
    lv_anim_init(&s_anim_opa);
    lv_anim_set_var(&s_anim_opa, s_img_monk);
    lv_anim_set_values(&s_anim_opa, max_val, min_val);
    lv_anim_set_duration(&s_anim_opa, duration);
    lv_anim_set_exec_cb(&s_anim_opa, anim_opa_cb);
    lv_anim_set_path_cb(&s_anim_opa, path);
    lv_anim_set_playback_duration(&s_anim_opa, duration);
    lv_anim_set_repeat_count(&s_anim_opa, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&s_anim_opa);
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

void ui_monk_create(lv_obj_t *parent)
{
    s_img_monk = lv_image_create(parent);
    lv_image_set_src(s_img_monk, &img_monk);
    lv_obj_center(s_img_monk);

    /* Save base Y for animation offset */
    lv_obj_update_layout(s_img_monk);
    s_base_y = lv_obj_get_y(s_img_monk);

    /* Start idle floating */
    start_y_anim(IDLE_Y_MIN, IDLE_Y_MAX, IDLE_DURATION_MS,
                 lv_anim_path_ease_in_out);
}

void ui_monk_set_state(rtc_ai_state_t state)
{
    if (state == s_cur_state) return;
    s_cur_state = state;

    stop_all_anims();

    switch (state) {
    case RTC_AI_STATE_IDLE:
        /* Gentle floating — slow Y bob */
        start_y_anim(IDLE_Y_MIN, IDLE_Y_MAX, IDLE_DURATION_MS,
                     lv_anim_path_ease_in_out);
        break;

    case RTC_AI_STATE_LISTENING:
        /* Attentive — Y bob + opacity pulse */
        start_y_anim(LISTEN_Y_MIN, LISTEN_Y_MAX, LISTEN_DURATION_MS,
                     lv_anim_path_ease_in_out);
        start_opa_anim(LISTEN_OPA_MIN, LISTEN_OPA_MAX, LISTEN_DURATION_MS,
                       lv_anim_path_ease_in_out);
        break;

    case RTC_AI_STATE_THINKING:
        /* Meditation — deep opacity pulse, no movement */
        start_opa_anim(THINK_OPA_MIN, THINK_OPA_MAX, THINK_DURATION_MS,
                       lv_anim_path_ease_in_out);
        break;

    case RTC_AI_STATE_SPEAKING:
        /* Speaking — energetic bounce */
        start_y_anim(SPEAK_Y_MIN, SPEAK_Y_MAX, SPEAK_DURATION_MS,
                     lv_anim_path_bounce);
        break;

    default:
        start_y_anim(IDLE_Y_MIN, IDLE_Y_MAX, IDLE_DURATION_MS,
                     lv_anim_path_ease_in_out);
        break;
    }
}

#endif /* CONFIG_APP_LCD_ENABLE */
