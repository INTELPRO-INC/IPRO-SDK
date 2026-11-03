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
 * @file uvc_rps.c
 * @brief Rock-paper-scissors drawn into the UVC stream. See uvc_rps.h.
 *
 * Runs in the encode task (uvc_isp.c), once per frame, on the frame the
 * encoder is about to read. Integer only: the NPU task beside it uses the FPU
 * and the FreeRTOS port does not save FPU context.
 *
 * Layout follows the EVK game screen (apps/ai/ipro7ai_evk_rps/src/rps_ui.c),
 * scaled to the frame: score bar on top, message bar at the bottom, the play
 * box (= the NPU's ROI) with its live label, the 3-2-1 countdown, and after a
 * round both moves.
 */

#include "uvc_rps.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "font5x7.h"
#include "rps_game.h"
#include "rps_model.h"
#include "uvc_draw.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define MAX_CHARS 40

/* Selfie view, as on the EVK: your right hand on the right. The NPU sees the
 * unmirrored ROI (the model was trained with flips). Turn off for a viewer
 * that mirrors the local preview itself (Photo Booth, FaceTime). */
static bool s_mirror = true;

/* ---------------------------------------------------------------------------
 * Overlay
 * -------------------------------------------------------------------------*/

static yuv_t box_colour(const rps_game_view_t *v)
{
    switch (v->state) {
    case RPS_ST_COUNTDOWN:
    case RPS_ST_CAPTURE:
        return OV_YELLOW;
    case RPS_ST_RESULT:
    case RPS_ST_COOLDOWN:
        return v->outcome == RPS_OUTCOME_PLAYER ? OV_GREEN
             : v->outcome == RPS_OUTCOME_BOARD  ? OV_RED
             : v->outcome == RPS_OUTCOME_DRAW   ? OV_CYAN : OV_WHITE;
    default:
        return OV_WHITE;
    }
}

static void log_round(const rps_game_view_t *v)
{
    static uint16_t logged;

    if (v->state == RPS_ST_RESULT && v->round != logged) {
        logged = v->round;
        printf("[GAME] round %u: you %s, me %s -> %s  (you %u me %u draw %u)\r\n",
               v->round, rps_class_name(v->player), rps_class_name(v->board),
               v->outcome == RPS_OUTCOME_PLAYER ? "YOU WIN" :
               v->outcome == RPS_OUTCOME_BOARD  ? "I WIN" : "DRAW",
               v->wins, v->losses, v->draws);
    }
}

void uvc_rps_overlay(uint8_t *f, const uint8_t *src, int w, int h, int crop_w, int crop_h)
{
    char buf[MAX_CHARS + 1];
    rps_result_t r;
    rps_game_view_t v;

    rps_model_get_result(&r);
    rps_game_tick((uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS), &r);
    rps_game_get_view(&v);
    log_round(&v);

    ov_copy_frame(f, src, w, h, s_mirror);

    /* One text unit per 120 rows: 3 at 360p, 6 at 720p. */
    const int s   = h / 120 < 2 ? 2 : h / 120;
    const int pad = 2 * s;
    const int bar = FONT_H * s + 2 * pad;

    /* Play box = the ROI the NPU reads, mapped from crop to frame. Centred,
     * so the mirror does not move it. */
    const int rx = (int)UVC_RPS_ROI_X((unsigned)crop_w), ry = (int)UVC_RPS_ROI_Y((unsigned)crop_h);
    const int x1 = rx * w / crop_w, x2 = (rx + (int)UVC_RPS_ROI_SIDE) * w / crop_w - 1;
    const int y1 = ry * h / crop_h, y2 = (ry + (int)UVC_RPS_ROI_SIDE) * h / crop_h - 1;
    const int cx = (x1 + x2) / 2, cy = (y1 + y2) / 2;
    const int t  = s;

    ov_outline_rect(f, w, h, x1, y1, x2, y2, t, box_colour(&v));

    /* Live label just inside the top-left corner of the box. */
    const int ls = s > 2 ? s - 1 : 2;
    if (!rps_model_ready()) {
        snprintf(buf, sizeof(buf), "NO MODEL: %s", rps_model_fail_reason());
        ov_draw_text(f, w, h, x1 + t + ls, y1 + t + ls, ls, buf, OV_YELLOW, ls);
    } else if (v.live_cls != RPS_NONE) {
        snprintf(buf, sizeof(buf), "%s %u%%", rps_class_name(v.live_cls), v.live_conf);
        ov_draw_text(f, w, h, x1 + t + ls, y1 + t + ls, ls, buf, OV_WHITE, ls);
    }

    /* Centre of the box: 3-2-1, GO, then both moves. */
    if (v.state == RPS_ST_COUNTDOWN) {
        snprintf(buf, sizeof(buf), "%d", v.countdown);
        int bs = 4 * s;
        ov_draw_text(f, w, h, cx - ov_text_w(buf, bs) / 2, cy - FONT_H * bs / 2, bs, buf,
                  OV_WHITE, s);
    } else if (v.state == RPS_ST_CAPTURE) {
        int bs = 3 * s;
        ov_draw_text(f, w, h, cx - ov_text_w("GO", bs) / 2, cy - FONT_H * bs / 2, bs, "GO",
                  OV_YELLOW, s);
    } else if ((v.state == RPS_ST_RESULT || v.state == RPS_ST_COOLDOWN) &&
               v.player != RPS_NONE) {
        const char *you = rps_class_name(v.player);
        int bs = 2 * s;
        while (bs > s && ov_text_w(you, bs) > x2 - x1 - 2 * t) {
            bs--;                   /* "SCISSORS" must stay inside the box */
        }
        ov_draw_text(f, w, h, cx - ov_text_w(you, bs) / 2, cy - FONT_H * bs, bs, you, OV_WHITE, s);
        snprintf(buf, sizeof(buf), "ME: %s", rps_class_name(v.board));
        ov_draw_text(f, w, h, cx - ov_text_w(buf, s) / 2, cy + 2 * s, s, buf, OV_YELLOW, s);
    }

    /* Top bar: score left, NPU time or the board's move right. */
    ov_darken_rect(f, w, h, 0, 0, w, bar);
    snprintf(buf, sizeof(buf), "YOU %u  ME %u  DRAW %u", v.wins, v.losses, v.draws);
    ov_draw_text(f, w, h, 2 * pad, pad, s, buf, OV_WHITE, 0);
    if ((v.state == RPS_ST_RESULT || v.state == RPS_ST_COOLDOWN) && v.board != RPS_NONE) {
        snprintf(buf, sizeof(buf), "ME: %s", rps_class_name(v.board));
    } else if (rps_model_ready()) {
        snprintf(buf, sizeof(buf), "NPU %luMS", (unsigned long)r.npu_ms);
    } else {
        snprintf(buf, sizeof(buf), "NO MODEL");
    }
    ov_draw_text(f, w, h, w - ov_text_w(buf, s) - 2 * pad, pad, s, buf, OV_WHITE, 0);

    /* Bottom bar: what to do next. */
    ov_darken_rect(f, w, h, 0, h - bar, w, h);
    const char *msg = v.message ? v.message : "";
    ov_draw_text(f, w, h, (w - ov_text_w(msg, s)) / 2, h - bar + pad, s, msg, OV_WHITE, 0);
}

int uvc_rps_init(void)
{
    rps_game_init();
    if (rps_model_init() != 0) {
        printf("[RPS] model not loaded: %s - streaming without the game\r\n",
               rps_model_fail_reason());
        return -1;
    }
    return 0;
}

/* ---------------------------------------------------------------------------
 * Shell
 * -------------------------------------------------------------------------*/

#ifdef CONFIG_SHELL
/* One ch1 frame (128x128 ARGB, what the NPU reads) as base64 between
 * CAPBEGIN/CAPEND lines, so the ROI's orientation and colour can be checked
 * on the host. 64 KB, about 8 s at 115200. */
static void dump_frame(void)
{
    static const char b64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const uint32_t len = (uint32_t)RPS_IN_W * RPS_IN_H * 4u;
    const uint8_t *p;
    uint32_t sum = 0;
    char line[77];
    int n = 0;

    if (rps_model_capture(&p, 2000) != 0) {
        printf("rps dump: no frame (model ready %d)\r\n", rps_model_ready());
        return;
    }
    rps_model_set_print(false);
    printf("CAPBEGIN %u %u %lu\r\n", RPS_IN_W, RPS_IN_H, (unsigned long)len);
    for (uint32_t i = 0; i < len; i += 3) {
        uint32_t a = p[i], b = i + 1 < len ? p[i + 1] : 0, c = i + 2 < len ? p[i + 2] : 0;
        uint32_t v = (a << 16) | (b << 8) | c;
        sum += a + (i + 1 < len ? b : 0) + (i + 2 < len ? c : 0);
        line[n++] = b64[(v >> 18) & 63];
        line[n++] = b64[(v >> 12) & 63];
        line[n++] = i + 1 < len ? b64[(v >> 6) & 63] : '=';
        line[n++] = i + 2 < len ? b64[v & 63] : '=';
        if (n == 76 || i + 3 >= len) {
            line[n] = '\0';
            printf("%s\r\n", line);
            n = 0;
        }
    }
    printf("CAPEND %lu\r\n", (unsigned long)sum);
}

static int cmd_rps(int argc, char *argv[])
{
    const char *sub = argc > 1 ? argv[1] : "";
    const char *arg = argc > 2 ? argv[2] : "";

    if (!strcmp(sub, "reset")) {
        rps_game_reset();
    } else if (!strcmp(sub, "go")) {
        rps_game_force_start();
    } else if (!strcmp(sub, "mirror")) {
        s_mirror = strcmp(arg, "off") != 0;
    } else if (!strcmp(sub, "print")) {
        rps_model_set_print(strcmp(arg, "off") != 0);
    } else if (!strcmp(sub, "conf") && *arg) {
        rps_game_set_conf(atoi(arg));
    } else if (!strcmp(sub, "dump")) {
        dump_frame();
        return 0;
    } else if (*sub) {
        printf("usage: rps [reset|go|mirror on|off|print on|off|conf <pct>|dump]\r\n");
        return -1;
    }

    rps_model_stats_t st;
    rps_result_t r;
    rps_game_view_t v;
    rps_model_get_stats(&st);
    rps_model_get_result(&r);
    rps_game_get_view(&v);
    printf("model   : %s, %lu B from %s, loaded in %lu ms, %lu KB heap left\r\n",
           rps_model_ready() ? "ready" : rps_model_fail_reason(),
           (unsigned long)st.model_bytes, st.source ? st.source : "-",
           (unsigned long)st.load_ms, (unsigned long)(st.free_heap / 1024u));
    printf("npu     : %lu frames, %lu errors, %lu.%lu fps, last %lu ms\r\n",
           (unsigned long)st.frames, (unsigned long)st.errors,
           (unsigned long)(st.fps_x10 / 10u), (unsigned long)(st.fps_x10 % 10u),
           (unsigned long)r.npu_ms);
    printf("now     : %s %u%%  (r %u p %u s %u n %u)\r\n", rps_class_name(r.cls), r.conf,
           r.prob[0], r.prob[1], r.prob[2], r.prob[3]);
    printf("game    : round %u, you %u me %u draw %u, conf >= %d%%, mirror %s\r\n",
           v.round, v.wins, v.losses, v.draws, rps_game_get_conf(),
           s_mirror ? "on" : "off");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_rps, rps, Rock-paper-scissors: rps [reset|go|mirror|print|conf|dump]);
#endif /* CONFIG_SHELL */
