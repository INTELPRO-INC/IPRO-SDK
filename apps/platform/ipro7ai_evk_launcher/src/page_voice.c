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
 * @file page_voice.c
 * @brief Page 2: microphone waveform, record, play back through the speaker.
 *
 *   ROCK      record VOICE_DEFAULT_SECONDS then play it back
 *   SCISSORS  play the last recording again
 *
 * The page area is a scrolling waveform: one column per 20 ms mic frame,
 * newest on the right, height = the frame's peak. Green while listening and
 * recording, cyan while playing, red where a frame hit full scale.
 */

#include "ui.h"
#include "voice.h"

#include <stdio.h>
#include <string.h>

#define COL_WAVE_LISTEN   ST_GREEN
#define COL_WAVE_PLAY     ST_CYAN
#define COL_WAVE_CLIP     ST_RED
#define COL_MIDLINE       ST_GRAY

static voice_status_t s_vs;
static const uint8_t *s_wave;
static uint32_t       s_head;
static uint16_t       s_col_wave, s_col_clip, s_col_mid;
static evk_text_t     s_level;

static void enter(void)
{
    voice_wave_clear();
    voice_listen(true);
}

static void leave(void)
{
    voice_stop();
    voice_listen(false);
}

static void prepare(const ui_frame_t *f)
{
    char buf[UI_MAX_TEXT + 1];

    (void)f;
    voice_get_status(&s_vs);
    s_wave = voice_wave(&s_head);

    s_col_wave = st7789p3_swap(s_vs.state == VOICE_PLAYING ? COL_WAVE_PLAY : COL_WAVE_LISTEN);
    s_col_clip = st7789p3_swap(COL_WAVE_CLIP);
    s_col_mid  = st7789p3_swap(COL_MIDLINE);

    /* Level read-out in the top-left of the page area. */
    if (s_vs.error[0]) {
        evk_text_set(&s_level, s_vs.error, 1, ST_YELLOW, true, 4, 4);
    } else if (s_vs.state == VOICE_IDLE) {
        evk_text_set(&s_level, "MIC OFF", 1, ST_GRAY, true, 4, 4);
    } else {
        int db = s_vs.peak_dbfs_x10;
        snprintf(buf, sizeof(buf), "PEAK %d.%d DBFS", db / 10, (db < 0 ? -db : db) % 10);
        evk_text_set(&s_level, buf, 1, ST_WHITE, true, 4, 4);
    }

    switch (s_vs.state) {
    case VOICE_RECORDING:
        snprintf(buf, sizeof(buf), "RECORDING %lu.%lu / %lu S",
                 (unsigned long)(s_vs.pos_ms / 1000u), (unsigned long)((s_vs.pos_ms / 100u) % 10u),
                 (unsigned long)(s_vs.target_ms / 1000u));
        ui_set_bottom(buf, ST_RED);
        break;
    case VOICE_PLAYING:
        snprintf(buf, sizeof(buf), "PLAYING %lu.%lu / %lu.%lu S",
                 (unsigned long)(s_vs.pos_ms / 1000u), (unsigned long)((s_vs.pos_ms / 100u) % 10u),
                 (unsigned long)(s_vs.clip_ms / 1000u), (unsigned long)((s_vs.clip_ms / 100u) % 10u));
        ui_set_bottom(buf, ST_CYAN);
        break;
    default:
        if (voice_has_clip()) {
            ui_set_bottom("ROCK=RECORD  SCISSORS=REPLAY", ST_WHITE);
        } else {
            snprintf(buf, sizeof(buf), "ROCK=RECORD %d S AND PLAY BACK", VOICE_DEFAULT_SECONDS);
            ui_set_bottom(buf, ST_WHITE);
        }
        break;
    }
}

static void row(const ui_frame_t *f, uint16_t *r, int w, int y)
{
    int mid = f->g.img_h / 2;
    int d   = y - mid;

    if (d < 0) {
        d = -d;
    }
    if (s_wave && s_vs.state != VOICE_IDLE) {
        /* Column x shows the frame (head + 1 + x) - oldest on the left. */
        int span = mid - 1;
        for (int x = 0; x < w; x++) {
            uint32_t idx = (s_head + 1u + (uint32_t)x) % VOICE_WAVE_COLUMNS;
            uint8_t  a   = s_wave[idx];
            int half = (a * span) / 255;
            if (d <= half && a) {
                r[x] = (a == 255) ? s_col_clip : s_col_wave;
            } else if (d == 0) {
                r[x] = s_col_mid;
            }
        }
    } else if (d == 0) {
        for (int x = 0; x < w; x++) {
            r[x] = s_col_mid;
        }
    }
    evk_text_row(&s_level, r, w, y);
}

static void event(input_event_t ev)
{
    if (ev == INPUT_EV_SELECT) {
        if (s_vs.state == VOICE_RECORDING || s_vs.state == VOICE_PLAYING) {
            voice_stop();
        } else {
            voice_record(VOICE_DEFAULT_SECONDS, true);
        }
    } else if (ev == INPUT_EV_ALT) {
        voice_play();
    }
}

const ui_page_t ui_page_voice = {
    .name          = "VOICE",
    .wants_preview = false,
    .enter         = enter,
    .leave         = leave,
    .prepare       = prepare,
    .row           = row,
    .event         = event,
    .hold          = NULL,
};
