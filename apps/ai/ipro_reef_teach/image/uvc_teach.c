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
 * @file uvc_teach.c
 * @brief The Teach classifier's result drawn around the box. See uvc_teach.h.
 *
 * Runs in the encode task (uvc_isp.c), once per frame, on a private copy of
 * the frame the encoder is about to read (drawing on the ISP's ping-pong
 * buffer tore the stream, see uvc_rps.c).
 *
 * Layout at 640x360: top bar with the NPU time; the box (outline outside the
 * ROI); left of it the winning class and its confidence; right of it every
 * class with its probability; bottom bar with the model's state.
 */

#include "uvc_teach.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "font5x7.h"
#include "teach_model.h"
#include "uvc_draw.h"
#include "uvc_teach_link.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define MAX_CHARS      40
#define LIST_LABEL_MAX 10   /* label characters shown in the per-class list */

/* Off: the stream shows the scene as the NPU sees it, so Studio's crop of the
 * box is the model's input. Studio mirrors its own preview if it wants to. */
static bool s_mirror = false;

void uvc_teach_overlay(uint8_t *f, const uint8_t *src, int w, int h, int crop_w, int crop_h)
{
    char buf[MAX_CHARS + 1];
    teach_result_t r;
    teach_stats_t st;
    const bool ready = teach_model_ready();

    teach_model_get_result(&r);
    teach_model_get_stats(&st);
    ov_copy_frame(f, src, w, h, s_mirror);

    /* One text unit per 120 rows: 3 at 360p. */
    const int s   = h / 120 < 2 ? 2 : h / 120;
    const int pad = 2 * s;
    const int bar = FONT_H * s + 2 * pad;
    const int ts  = s > 2 ? s - 1 : 2;          /* side-panel text */

    /* The box = the ROI the NPU reads, mapped from crop to frame; the outline
     * goes around it, never on it. */
    const int rx = (int)UVC_TEACH_ROI_X((unsigned)crop_w);
    const int ry = (int)UVC_TEACH_ROI_Y((unsigned)crop_h);
    const int x1 = rx * w / crop_w, x2 = (rx + (int)UVC_TEACH_ROI_SIDE) * w / crop_w - 1;
    const int y1 = ry * h / crop_h, y2 = (ry + (int)UVC_TEACH_ROI_SIDE) * h / crop_h - 1;
    const int t  = s > 2 ? 2 : 1;
    const bool sure = ready && r.seq && r.conf >= 60u;

    ov_outline_rect(f, w, h, x1 - t, y1 - t, x2 + t, y2 + t, t, sure ? OV_GREEN : OV_WHITE);

    /* Left: the winner, big, with its confidence under it. */
    const int lx = pad, lw = x1 - t - 2 * pad;
    if (ready && r.seq) {
        const char *lab = teach_model_label(r.cls);
        int bs = 2 * ts;
        while (bs > ts && ov_text_w(lab, bs) > lw) {
            bs--;
        }
        ov_draw_text(f, w, h, lx, y1 + pad, bs, lab, sure ? OV_GREEN : OV_WHITE, ts);
        snprintf(buf, sizeof(buf), "%u%%", r.conf);
        ov_draw_text(f, w, h, lx, y1 + pad + (FONT_H + 3) * bs, 2 * ts, buf, OV_WHITE, ts);
    }

    /* Right: every class and its probability, the winner highlighted. */
    if (ready) {
        const int rxp = x2 + t + pad;
        for (unsigned k = 0; k < st.nclass; k++) {
            char lab[LIST_LABEL_MAX + 1];
            snprintf(lab, sizeof(lab), "%s", teach_model_label(k));
            snprintf(buf, sizeof(buf), "%-*s%4u%%", LIST_LABEL_MAX, lab, r.seq ? r.prob[k] : 0u);
            const int ly = y1 + (int)k * (FONT_H + 4) * ts;
            if (ly + FONT_H * ts > y2) {
                break;
            }
            ov_draw_text(f, w, h, rxp, ly, ts, buf,
                         (r.seq && k == r.cls) ? OV_GREEN : OV_GREY, ts / 2);
        }
    }

    /* Top bar: title left, NPU time right. */
    ov_darken_rect(f, w, h, 0, 0, w, bar);
    ov_draw_text(f, w, h, 2 * pad, pad, s, "IPRO AI TEACH", OV_WHITE, 0);
    if (ready) {
        snprintf(buf, sizeof(buf), "NPU %luMS %lu.%luFPS", (unsigned long)r.npu_ms,
                 (unsigned long)(st.fps_x10 / 10u), (unsigned long)(st.fps_x10 % 10u));
    } else {
        snprintf(buf, sizeof(buf), "NO MODEL");
    }
    ov_draw_text(f, w, h, w - ov_text_w(buf, s) - 2 * pad, pad, s, buf, OV_WHITE, 0);

    /* Bottom bar: the model's state. */
    ov_darken_rect(f, w, h, 0, h - bar, w, h);
    if (ready) {
        snprintf(buf, sizeof(buf), "%u CLASSES%s", st.nclass,
                 st.labelled ? "" : " (UNLABELLED)");
    } else {
        snprintf(buf, sizeof(buf), "NO MODEL: %s", teach_model_fail_reason());
    }
    ov_draw_text(f, w, h, (w - ov_text_w(buf, s)) / 2, h - bar + pad, s, buf,
                 ready ? OV_WHITE : OV_YELLOW, 0);
}

int uvc_teach_init(void)
{
    /* Deploy first and regardless: a board with no (or a broken) model is
     * exactly the one Studio needs to reach. */
    uvc_teach_link_start();
    if (teach_model_init() != 0) {
        printf("[TEACH] model not loaded: %s - streaming without it\r\n",
               teach_model_fail_reason());
        return -1;
    }
    return 0;
}

/* ---------------------------------------------------------------------------
 * Shell
 * -------------------------------------------------------------------------*/

#ifdef CONFIG_SHELL
/* One ch1 frame (128x128 ARGB, what the NPU reads) as base64 between
 * CAPBEGIN/CAPEND lines, then the logits the NPU gave that frame (CAPLOGITS),
 * for checking both against Studio's crop of the stream and TFLite. 64 KB,
 * about 8 s at 115200. */
static void dump_frame(void)
{
    static const char b64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const uint32_t len = (uint32_t)TEACH_IN_W * TEACH_IN_H * 4u;
    const uint8_t *p;
    uint32_t sum = 0, seq = 0;
    teach_result_t r;
    char line[77];
    int n = 0;

    if (teach_model_capture(&p, &seq, 2000) != 0) {
        printf("teach dump: no frame (model ready %d)\r\n", teach_model_ready());
        return;
    }
    teach_model_set_print(false);
    printf("CAPBEGIN %u %u %lu\r\n", TEACH_IN_W, TEACH_IN_H, (unsigned long)len);
    for (uint32_t i = 0; i < len; i += 3) {
        uint32_t a = p[i], b = i + 1 < len ? p[i + 1] : 0, c = i + 2 < len ? p[i + 2] : 0;
        uint32_t v = (a << 16) | (b << 8) | c;
        sum += a + b + c;
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

    /* The logits the NPU produced for this very frame (it was inferred while
     * the dump printed), for comparing against TFLite on the host. */
    if (teach_model_capture_result(&r) == 0 && r.seq == seq) {
        printf("CAPLOGITS %lu", (unsigned long)seq);
        for (unsigned k = 0; k < TEACH_MAX_CLASSES; k++) {
            printf(" %d", r.logit_x100[k]);
        }
        printf("\r\n");
    } else {
        printf("CAPLOGITS none (frame seq %lu)\r\n", (unsigned long)seq);
    }
}

static int cmd_teach(int argc, char *argv[])
{
    const char *sub = argc > 1 ? argv[1] : "";
    const char *arg = argc > 2 ? argv[2] : "";

    if (!strcmp(sub, "mirror")) {
        s_mirror = strcmp(arg, "off") != 0;
    } else if (!strcmp(sub, "print")) {
        teach_model_set_print(strcmp(arg, "off") != 0);
    } else if (!strcmp(sub, "dump")) {
        dump_frame();
        return 0;
    } else if (*sub) {
        printf("usage: teach [mirror on|off|print on|off|dump]\r\n");
        return -1;
    }

    teach_stats_t st;
    teach_result_t r;
    teach_model_get_stats(&st);
    teach_model_get_result(&r);
    printf("model   : %s, %lu B, loaded in %lu ms, %lu KB heap left\r\n",
           teach_model_ready() ? "ready" : teach_model_fail_reason(),
           (unsigned long)st.model_bytes, (unsigned long)st.load_ms,
           (unsigned long)(st.free_heap / 1024u));
    printf("output  : %ux%ux%u pitch %u, scale %.5f zp %d\r\n", st.out_w, st.out_h, st.out_c,
           st.out_pitch, (double)st.out_scale, st.out_zp);
    printf("classes : %u%s\r\n", st.nclass, st.labelled ? "" : " (no label block)");
    for (unsigned k = 0; k < st.nclass; k++) {
        printf("  %u %-16s %3u%%  logit x100 %d\r\n", k, teach_model_label(k), r.prob[k],
               r.logit_x100[k]);
    }
    uvc_teach_link_print();
    printf("npu     : %lu frames, %lu errors, %lu.%lu fps, last %lu ms, mirror %s\r\n",
           (unsigned long)st.frames, (unsigned long)st.errors,
           (unsigned long)(st.fps_x10 / 10u), (unsigned long)(st.fps_x10 % 10u),
           (unsigned long)r.npu_ms, s_mirror ? "on" : "off");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_teach, teach, User-trained classifier: teach [mirror|print|dump]);
#endif /* CONFIG_SHELL */
