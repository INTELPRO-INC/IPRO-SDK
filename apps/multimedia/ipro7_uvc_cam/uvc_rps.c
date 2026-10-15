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

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define MAX_CHARS 40

/* BT.601 video-range colours. */
typedef struct {
    uint8_t y, u, v;
} yuv_t;

static const yuv_t C_WHITE  = {235, 128, 128};
static const yuv_t C_YELLOW = {210,  16, 146};
static const yuv_t C_GREEN  = {145,  54,  34};
static const yuv_t C_RED    = { 81,  90, 240};
static const yuv_t C_CYAN   = {170, 166,  16};

/* Selfie view, as on the EVK: your right hand on the right. The NPU sees the
 * unmirrored ROI (the model was trained with flips). Turn off for a viewer
 * that mirrors the local preview itself (Photo Booth, FaceTime). */
static bool s_mirror = true;

/* ---------------------------------------------------------------------------
 * YUYV drawing. Packed [Y0 U Y1 V]: two pixels share one U/V pair.
 * -------------------------------------------------------------------------*/

static inline void put_px(uint8_t *f, int w, int x, int y, yuv_t c)
{
    int px = y * w + x;
    uint8_t *p = f + (px >> 1) * 4;

    if (px & 1) {
        p[2] = c.y;
        p[3] = c.v;
    } else {
        p[0] = c.y;
        p[1] = c.u;
    }
}

static void fill_rect(uint8_t *f, int w, int h, int x0, int y0, int x1, int y1, yuv_t c)
{
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > w) x1 = w;
    if (y1 > h) y1 = h;
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            put_px(f, w, x, y, c);
        }
    }
}

/* Dark grey scrim behind text: luma to a third, chroma neutral. Works on
 * whole YUYV words, so x is widened to even boundaries. */
static void darken_rect(uint8_t *f, int w, int h, int x0, int y0, int x1, int y1)
{
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > w) x1 = w;
    if (y1 > h) y1 = h;
    x0 &= ~1;
    x1 = (x1 + 1) & ~1;
    for (int y = y0; y < y1; y++) {
        uint32_t *row = (uint32_t *)(void *)(f + ((size_t)y * w + x0) * 2);
        for (int i = 0; i < (x1 - x0) / 2; i++) {
            uint32_t v = row[i];
            uint32_t y0l = (v & 0xFFu) / 3u, y1l = ((v >> 16) & 0xFFu) / 3u;
            row[i] = y0l | (0x80u << 8) | (y1l << 16) | (0x80u << 24);
        }
    }
}

static void outline_rect(uint8_t *f, int w, int h, int x0, int y0, int x1, int y1,
                         int t, yuv_t c)
{
    fill_rect(f, w, h, x0, y0, x1 + 1, y0 + t, c);
    fill_rect(f, w, h, x0, y1 + 1 - t, x1 + 1, y1 + 1, c);
    fill_rect(f, w, h, x0, y0, x0 + t, y1 + 1, c);
    fill_rect(f, w, h, x1 + 1 - t, y0, x1 + 1, y1 + 1, c);
}

/* Copy one frame out of the ISP's buffer, mirrored or not. Mirroring a YUYV
 * row = reverse its word order and swap Y0/Y1 inside each word (U/V stay with
 * their pair). One read of the source: the ISP comes back to it 33 ms later. */
static void copy_frame(uint8_t *dst, const uint8_t *src, int w, int h, bool mirror)
{
    const int n = w / 2;

    if (!mirror) {
        memcpy(dst, src, (size_t)w * h * 2);
        return;
    }
    for (int y = 0; y < h; y++) {
        const uint32_t *s = (const uint32_t *)(const void *)(src + (size_t)y * w * 2);
        uint32_t *d = (uint32_t *)(void *)(dst + (size_t)y * w * 2);
        for (int i = 0; i < n; i++) {
            uint32_t v = s[n - 1 - i];
            d[i] = (v & 0xFF00FF00u) | ((v & 0xFFu) << 16) | ((v >> 16) & 0xFFu);
        }
    }
}

static int text_w(const char *s, int scale)
{
    int n = (int)strlen(s);
    return n ? (n * FONT_ADVANCE - 1) * scale : 0;
}

/* Text at (x,y) = top-left of the glyphs; with a scrim padded by pad. */
static void draw_text(uint8_t *f, int w, int h, int x, int y, int scale, const char *s,
                      yuv_t c, int pad)
{
    if (pad > 0) {
        darken_rect(f, w, h, x - pad, y - pad, x + text_w(s, scale) + pad,
                    y + FONT_H * scale + pad);
    }
    for (int i = 0; s[i]; i++) {
        const uint8_t *g = font5x7_glyph(s[i]);
        int gx = x + i * FONT_ADVANCE * scale;
        for (int col = 0; col < FONT_W; col++) {
            for (int row = 0; row < FONT_H; row++) {
                if (font5x7_pixel(g, col, row)) {
                    fill_rect(f, w, h, gx + col * scale, y + row * scale,
                              gx + (col + 1) * scale, y + (row + 1) * scale, c);
                }
            }
        }
    }
}

/* ---------------------------------------------------------------------------
 * Overlay
 * -------------------------------------------------------------------------*/

static yuv_t box_colour(const rps_game_view_t *v)
{
    switch (v->state) {
    case RPS_ST_COUNTDOWN:
    case RPS_ST_CAPTURE:
        return C_YELLOW;
    case RPS_ST_RESULT:
    case RPS_ST_COOLDOWN:
        return v->outcome == RPS_OUTCOME_PLAYER ? C_GREEN
             : v->outcome == RPS_OUTCOME_BOARD  ? C_RED
             : v->outcome == RPS_OUTCOME_DRAW   ? C_CYAN : C_WHITE;
    default:
        return C_WHITE;
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

    copy_frame(f, src, w, h, s_mirror);

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

    outline_rect(f, w, h, x1, y1, x2, y2, t, box_colour(&v));

    /* Live label just inside the top-left corner of the box. */
    const int ls = s > 2 ? s - 1 : 2;
    if (!rps_model_ready()) {
        snprintf(buf, sizeof(buf), "NO MODEL: %s", rps_model_fail_reason());
        draw_text(f, w, h, x1 + t + ls, y1 + t + ls, ls, buf, C_YELLOW, ls);
    } else if (v.live_cls != RPS_NONE) {
        snprintf(buf, sizeof(buf), "%s %u%%", rps_class_name(v.live_cls), v.live_conf);
        draw_text(f, w, h, x1 + t + ls, y1 + t + ls, ls, buf, C_WHITE, ls);
    }

    /* Centre of the box: 3-2-1, GO, then both moves. */
    if (v.state == RPS_ST_COUNTDOWN) {
        snprintf(buf, sizeof(buf), "%d", v.countdown);
        int bs = 4 * s;
        draw_text(f, w, h, cx - text_w(buf, bs) / 2, cy - FONT_H * bs / 2, bs, buf,
                  C_WHITE, s);
    } else if (v.state == RPS_ST_CAPTURE) {
        int bs = 3 * s;
        draw_text(f, w, h, cx - text_w("GO", bs) / 2, cy - FONT_H * bs / 2, bs, "GO",
                  C_YELLOW, s);
    } else if ((v.state == RPS_ST_RESULT || v.state == RPS_ST_COOLDOWN) &&
               v.player != RPS_NONE) {
        const char *you = rps_class_name(v.player);
        int bs = 2 * s;
        while (bs > s && text_w(you, bs) > x2 - x1 - 2 * t) {
            bs--;                   /* "SCISSORS" must stay inside the box */
        }
        draw_text(f, w, h, cx - text_w(you, bs) / 2, cy - FONT_H * bs, bs, you, C_WHITE, s);
        snprintf(buf, sizeof(buf), "ME: %s", rps_class_name(v.board));
        draw_text(f, w, h, cx - text_w(buf, s) / 2, cy + 2 * s, s, buf, C_YELLOW, s);
    }

    /* Top bar: score left, NPU time or the board's move right. */
    darken_rect(f, w, h, 0, 0, w, bar);
    snprintf(buf, sizeof(buf), "YOU %u  ME %u  DRAW %u", v.wins, v.losses, v.draws);
    draw_text(f, w, h, 2 * pad, pad, s, buf, C_WHITE, 0);
    if ((v.state == RPS_ST_RESULT || v.state == RPS_ST_COOLDOWN) && v.board != RPS_NONE) {
        snprintf(buf, sizeof(buf), "ME: %s", rps_class_name(v.board));
    } else if (rps_model_ready()) {
        snprintf(buf, sizeof(buf), "NPU %luMS", (unsigned long)r.npu_ms);
    } else {
        snprintf(buf, sizeof(buf), "NO MODEL");
    }
    draw_text(f, w, h, w - text_w(buf, s) - 2 * pad, pad, s, buf, C_WHITE, 0);

    /* Bottom bar: what to do next. */
    darken_rect(f, w, h, 0, h - bar, w, h);
    const char *msg = v.message ? v.message : "";
    draw_text(f, w, h, (w - text_w(msg, s)) / 2, h - bar + pad, s, msg, C_WHITE, 0);
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
