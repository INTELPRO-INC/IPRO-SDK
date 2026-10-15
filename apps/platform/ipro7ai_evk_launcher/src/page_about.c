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
 * @file page_about.c
 * @brief Page 3: what the board is and what every block is doing right now.
 *
 * Text lines, refreshed twice a second. No gesture actions here.
 */

#include "ui.h"
#include "gesture.h"
#include "detect.h"
#include "voice.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#define LINES         12
#define LINE_SCALE    1
#define LINE_X        4
#define LINE_Y0       4
#define REFRESH_MS    500

static evk_text_t s_lines[LINES];
static uint32_t   s_last_ms;
static ui_stats_t s_ui;

static int line_h(void)
{
    return evk_text_height(LINE_SCALE) + 1;
}

static void set_line(int i, const char *s, uint16_t fg)
{
    if (i < LINES) {
        evk_text_set(&s_lines[i], s, LINE_SCALE, fg, false, LINE_X, LINE_Y0 + i * line_h());
    }
}

static void refresh(const ui_frame_t *f)
{
    char buf[96];                    /* evk_text_set() clips to UI_MAX_TEXT */
    gesture_stats_t gs;
    evk_cam_stats_t cs;
    evk_mic_stats_t ms;
    voice_status_t  vs;
    int n = 0;

    gesture_get_stats(&gs);
    evk_cam_get_stats(&cs);
    evk_mic_get_stats(&ms);
    voice_get_status(&vs);
    ui_get_stats(&s_ui);

    set_line(n++, EVK_BOARD_NAME "  IPRO7AI  NPU + ISP + AUDIO", ST_WHITE);

    if (gesture_ready()) {
        snprintf(buf, sizeof(buf), "NPU  %s FROM %s  %lu.%lu FPS  %lu MS",
                 "GESTURE 128X128", gesture_source(),
                 (unsigned long)(gs.fps_x10 / 10u), (unsigned long)(gs.fps_x10 % 10u),
                 (unsigned long)f->ges.npu_ms);
        set_line(n++, buf, ST_GREEN);
    } else {
        snprintf(buf, sizeof(buf), "NPU  NO MODEL: %s", gesture_fail_reason());
        set_line(n++, buf, ST_YELLOW);
    }

    if (detect_is_ready()) {
        detect_stats_t ds;
        detect_result_t dr;
        detect_get_stats(&ds);
        detect_get_result(&dr);
        snprintf(buf, sizeof(buf), "DET  PERSON/PET 640X360 FROM SD  %lu.%lu FPS  %lu MS",
                 (unsigned long)(ds.fps_x10 / 10u), (unsigned long)(ds.fps_x10 % 10u),
                 (unsigned long)dr.npu_ms);
        set_line(n++, buf, ST_GREEN);
    } else {
        snprintf(buf, sizeof(buf), "DET  OFF: %s", detect_fail_reason());
        set_line(n++, buf, ST_GRAY);
    }

    if (evk_cam_is_running()) {
        snprintf(buf, sizeof(buf), "CAM  %s 720P  %lu.%lu FPS  %lu FRAMES  ERR %lu",
                 evk_cam_sensor_name() ? evk_cam_sensor_name() : "?",
                 (unsigned long)(s_ui.fps_x10 / 10u), (unsigned long)(s_ui.fps_x10 % 10u),
                 (unsigned long)cs.frames0,
                 (unsigned long)(cs.fifo_err + cs.vsync_err + cs.hsync_err));
        set_line(n++, buf, ST_GREEN);
    } else {
        set_line(n++, "CAM  NOT RUNNING - CHECK J6 PIN 3-7", ST_YELLOW);
    }

    snprintf(buf, sizeof(buf), "LCD  ST7789P3 %uX%u  SPI %lu MHZ  %lu MS/FRAME",
             st7789p3_width(), st7789p3_height(),
             (unsigned long)(st7789p3_get_spi_clk() / 1000000u), (unsigned long)s_ui.blit_ms);
    set_line(n++, buf, ST_GREEN);

    snprintf(buf, sizeof(buf), "MIC  IM69D129 PDM  %lu HZ  %lu FRAMES  DROP %lu",
             (unsigned long)evk_mic_real_rate(), (unsigned long)ms.isr_frames,
             (unsigned long)ms.dropped);
    set_line(n++, buf, ms.isr_frames ? ST_GREEN : ST_GRAY);

    snprintf(buf, sizeof(buf), "SPK  DAC -> TPA2037D1  VOL %d%%  GAIN +%d DB  CLIP %s",
             evk_spk_get_volume(), voice_get_gain_db(),
             voice_has_clip() ? "YES" : "NO");
    set_line(n++, buf, ST_GREEN);

#if EVK_HAS_SDCARD
    snprintf(buf, sizeof(buf), "SD   %s%s", evk_sd_present() ? "CARD SEATED" : "NO CARD",
             evk_sd_mounted() ? ", MOUNTED AT SD:/" : "");
    set_line(n++, buf, evk_sd_present() ? ST_GREEN : ST_GRAY);
#endif

    snprintf(buf, sizeof(buf), "HEAP %lu KB FREE  UP %lu S  EVENTS %lu",
             (unsigned long)(xPortGetFreeHeapSize() / 1024u),
             (unsigned long)(f->now_ms / 1000u), (unsigned long)s_ui.events);
    set_line(n++, buf, ST_WHITE);

    set_line(n++, "", ST_WHITE);
    set_line(n++, "PAPER = NEXT PAGE   ROCK = SELECT   SCISSORS = ALT", ST_GRAY);
    set_line(n++, "SHELL: APP, LCD, CAM, MIC, SPK, SD", ST_GRAY);
    for (; n < LINES; n++) {
        set_line(n, "", ST_WHITE);
    }
}

static void enter(void)
{
    s_last_ms = 0;
}

static void prepare(const ui_frame_t *f)
{
    if (f->now_ms - s_last_ms >= REFRESH_MS || s_last_ms == 0) {
        s_last_ms = f->now_ms;
        refresh(f);
    }
    ui_set_bottom("PAPER=NEXT PAGE", ST_WHITE);
}

static void row(const ui_frame_t *f, uint16_t *r, int w, int y)
{
    (void)f;
    for (int i = 0; i < LINES; i++) {
        evk_text_row(&s_lines[i], r, w, y);
    }
}

const ui_page_t ui_page_about = {
    .name          = "ABOUT",
    .wants_preview = false,
    .enter         = enter,
    .leave         = NULL,
    .prepare       = prepare,
    .row           = row,
    .event         = NULL,
    .hold          = NULL,
};
