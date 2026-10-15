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
 * @file app_cli.c
 * @brief `rps`, `cam` and `lcd` shell commands.
 *
 * `rps capture <label>` streams the 128x128 frame the NPU just saw as base64,
 * framed by CAPBEGIN / CAPEND lines, for tools/rps_capture.py to turn into
 * labelled training images. 3 bytes per pixel RGB (the ISP's 4th byte is
 * padding), ~64 KB of text per frame - 0.7 s at 921600 baud.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_SHELL

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "rps_model.h"
#include "rps_game.h"
#include "rps_ui.h"
#include "evk_cam.h"
#include "evk_cam_port.h"
#include "st7789p3.h"

static int parse_onoff(const char *s, bool *out)
{
    if (!strcmp(s, "on") || !strcmp(s, "1"))  { *out = true;  return 0; }
    if (!strcmp(s, "off") || !strcmp(s, "0")) { *out = false; return 0; }
    return -1;
}

/* ---------------------------------------------------------------------------
 * rps
 * -------------------------------------------------------------------------*/

static void rps_usage(void)
{
    printf(
      "rps - rock / paper / scissors against the NPU\n"
      "  rps status             model, game, rates\n"
      "  rps reset              scores back to zero\n"
      "  rps go                 start a round now (no hand needed)\n"
      "  rps conf <10..99>      confidence to count a hand (default 60)\n"
      "  rps feed <raw|xor|copy> how the frame reaches the NPU (bench knob)\n"
      "  rps print <on|off>     print every classification\n"
      "  rps uvswap <on|off>    swap U/V if skin comes out blue\n"
      "  rps mirror <on|off>    mirror the preview like a selfie camera (default on)\n"
      "  rps npu <on|off>       feed the NPU / stop (preview stays)\n"
      "  rps capture <label>    dump the next NPU frame as base64 (training data)\n");
}

static const char *feed_name(rps_feed_t f)
{
    return f == RPS_FEED_RAW ? "raw" : f == RPS_FEED_XOR ? "xor" : "copy";
}

static void rps_status(void)
{
    rps_model_stats_t ms;
    rps_result_t      r;
    rps_game_view_t   v;
    rps_ui_stats_t    us;
    evk_cam_stats_t       cs;

    rps_model_get_stats(&ms);
    rps_model_get_result(&r);
    rps_game_get_view(&v);
    rps_ui_get_stats(&us);
    evk_cam_get_stats(&cs);

    if (rps_model_ready()) {
        printf("model   : %u B from '%s', loaded in %u ms, %ux%u ext_input %u unsigned %u,"
               " out scale %.5f zp %d, %u KB PSRAM left\n",
               (unsigned)ms.model_bytes, ms.source, (unsigned)ms.load_ms, (unsigned)ms.in_w,
               (unsigned)ms.in_h, ms.ext_input, ms.unsigned_input, (double)ms.out_scale,
               ms.out_zp, (unsigned)(ms.free_heap / 1024u));
    } else {
        printf("model   : NOT LOADED (%s)\n", rps_model_fail_reason());
    }
    printf("npu     : %s, feed %s, %lu.%lu fps, %u frames, %u errors, last %lu ms\n",
           evk_cam_npu_enabled() ? "running" : "stopped", feed_name(rps_model_get_feed()),
           (unsigned long)(ms.fps_x10 / 10u), (unsigned long)(ms.fps_x10 % 10u),
           (unsigned)ms.frames, (unsigned)ms.errors, (unsigned long)r.npu_ms);
    printf("last    : #%lu %s %u%%  p=%u/%u/%u/%u  raw=%d %d %d %d\n",
           (unsigned long)r.seq, rps_class_name(r.cls), r.conf,
           r.prob[0], r.prob[1], r.prob[2], r.prob[3],
           r.logit[0], r.logit[1], r.logit[2], r.logit[3]);
    printf("game    : state %d, round %u, you %u me %u draw %u, conf %d%%, msg \"%s\"\n",
           (int)v.state, v.round, v.wins, v.losses, v.draws, rps_game_get_conf(),
           v.message ? v.message : "");
    printf("preview : %lu.%lu fps, %u ms/frame, %u drawn, %u dropped; isp ch0 %u ch1 %u "
           "(%u to NPU) err %u/%u/%u\n",
           (unsigned long)(us.fps_x10 / 10u), (unsigned long)(us.fps_x10 % 10u),
           (unsigned)us.blit_ms, (unsigned)us.drawn, (unsigned)us.dropped,
           (unsigned)cs.frames0, (unsigned)cs.frames1, (unsigned)cs.pushed,
           (unsigned)cs.fifo_err, (unsigned)cs.vsync_err, (unsigned)cs.hsync_err);
}

static const char B64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void b64_line(const uint8_t *p, unsigned n)
{
    char out[4 * 20 + 1];       /* 57 input bytes -> 76 chars max */
    unsigned o = 0;

    for (unsigned i = 0; i < n; i += 3) {
        uint32_t v = (uint32_t)p[i] << 16;
        if (i + 1 < n) v |= (uint32_t)p[i + 1] << 8;
        if (i + 2 < n) v |= p[i + 2];
        out[o++] = B64[(v >> 18) & 63];
        out[o++] = B64[(v >> 12) & 63];
        out[o++] = (i + 1 < n) ? B64[(v >> 6) & 63] : '=';
        out[o++] = (i + 2 < n) ? B64[v & 63] : '=';
    }
    out[o] = '\0';
    printf("%s\n", out);
}

static int rps_capture(const char *label)
{
    const uint8_t *argb;
    static uint8_t rgb[57];         /* one base64 line = 57 bytes = 19 pixels */
    uint32_t sum = 0;
    unsigned n = 0;

    if (rps_model_capture(&argb, 2000) != 0) {
        printf("capture failed (NPU not running? `rps npu on`)\n");
        return -1;
    }
    printf("CAPBEGIN %s %d %d 3\n", label, RPS_IN_W, RPS_IN_H);
    for (unsigned p = 0; p < (unsigned)(RPS_IN_W * RPS_IN_H); p++) {
        for (int c = 0; c < 3; c++) {
            uint8_t b = argb[p * 4 + (unsigned)c];
            rgb[n++] = b;
            sum += b;
        }
        if (n == sizeof(rgb)) {
            b64_line(rgb, n);
            n = 0;
        }
    }
    if (n) {
        b64_line(rgb, n);
    }
    printf("CAPEND %lu\n", (unsigned long)sum);
    return 0;
}

static int cmd_rps(int argc, char **argv)
{
    bool on;

    if (argc < 2 || !strcmp(argv[1], "status")) { rps_status(); return 0; }
    if (!strcmp(argv[1], "reset")) { rps_game_reset(); printf("scores reset\n"); return 0; }
    if (!strcmp(argv[1], "go"))    { rps_game_force_start(); return 0; }
    if (!strcmp(argv[1], "conf") && argc > 2) {
        if (rps_game_set_conf(atoi(argv[2])) != 0) { printf("conf must be 10..99\n"); return -1; }
        return 0;
    }
    if (!strcmp(argv[1], "feed") && argc > 2) {
        rps_feed_t f;
        if (!strcmp(argv[2], "raw"))       f = RPS_FEED_RAW;
        else if (!strcmp(argv[2], "xor"))  f = RPS_FEED_XOR;
        else if (!strcmp(argv[2], "copy")) f = RPS_FEED_COPY;
        else { rps_usage(); return -1; }
        rps_model_set_feed(f);
        printf("feed %s\n", feed_name(f));
        return 0;
    }
    if (!strcmp(argv[1], "capture")) {
        return rps_capture(argc > 2 ? argv[2] : "unlabeled");
    }
    if (argc > 2 && parse_onoff(argv[2], &on) == 0) {
        if (!strcmp(argv[1], "print"))  { rps_model_set_print(on); return 0; }
        if (!strcmp(argv[1], "uvswap")) { rps_ui_set_uv_swap(on);  return 0; }
        if (!strcmp(argv[1], "mirror")) { rps_ui_set_mirror(on);   return 0; }
        if (!strcmp(argv[1], "npu"))    { evk_cam_npu_set_enabled(on); return 0; }
    }
    rps_usage();
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_rps, rps, rock paper scissors);

/* ---------------------------------------------------------------------------
 * cam / lcd - the knobs that matter once the wiring is proven
 * -------------------------------------------------------------------------*/

static int cmd_cam(int argc, char **argv)
{
    int rc;

    if (argc > 1 && !strcmp(argv[1], "start")) {
        rc = evk_cam_start();
        if (rc == EVK_CAM_OK) rps_ui_start(); else printf("cam start failed (%d)\n", rc);
        return rc;
    }
    if (argc > 1 && !strcmp(argv[1], "stop")) {
        rps_ui_stop();
        evk_cam_stop();
        rps_ui_splash("PREVIEW OFF", "cam start");
        return 0;
    }
    if (argc > 2 && !strcmp(argv[1], "fps")) {
        rc = evk_cam_set_fps(atoi(argv[2]));
        if (rc != EVK_CAM_OK) printf("fps must be 1..30\n");
        return rc;
    }
    if (argc > 2 && !strcmp(argv[1], "rot")) {
        rc = evk_cam_set_rot(atoi(argv[2]));
        if (rc == EVK_CAM_OK) printf("rot %d - `cam stop` / `cam start`\n", evk_cam_get_rot());
        return rc;
    }
    if (argc > 2 && !strcmp(argv[1], "rst")) { evk_cam_reset_pin(atoi(argv[2])); return 0; }

    evk_cam_geometry_t g;
    evk_cam_stats_t    st;
    evk_cam_get_geometry(&g);
    evk_cam_get_stats(&st);
    printf("cam - SC101IOT -> ISP -> preview + NPU ROI\n"
           "  cam start | stop | fps <1..30> | rot <0..3> | rst <0|1>\n"
           "pipeline: %s, %d fps, rot %d; preview %ux%u at (%u,%u); ROI %ux%u+%u+%u\n"
           "frames  : ch0 %u, ch1 %u (%u to NPU), errors fifo %u vsync %u hsync %u\n"
           "wiring  : SCL=%d SDA=%d D0-7=%d-%d PCLK=%d VSYNC=%d HSYNC=%d MCLK=%d RST=%d\n",
           evk_cam_is_running() ? "running" : "stopped", evk_cam_get_fps(), evk_cam_get_rot(),
           (unsigned)g.img_w, (unsigned)g.img_h, (unsigned)g.ox, (unsigned)g.oy,
           (unsigned)g.roi_w, (unsigned)g.roi_h, (unsigned)g.roi_x, (unsigned)g.roi_y,
           (unsigned)st.frames0, (unsigned)st.frames1, (unsigned)st.pushed,
           (unsigned)st.fifo_err, (unsigned)st.vsync_err, (unsigned)st.hsync_err,
           EVK_CAM_PIN_SCL, EVK_CAM_PIN_SDA, EVK_CAM_PIN_D0, EVK_CAM_PIN_D0 + 7, EVK_CAM_PIN_PCLK,
           EVK_CAM_PIN_VSYNC, EVK_CAM_PIN_HSYNC, EVK_CAM_PIN_MCLK, EVK_CAM_PIN_RST);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cam, cam, camera pipeline);

static int cmd_lcd(int argc, char **argv)
{
    if (argc > 2 && !strcmp(argv[1], "clk")) {
        st7789p3_set_spi_clk((uint32_t)strtoul(argv[2], NULL, 0));
        printf("SPI clock request %lu Hz\n", (unsigned long)st7789p3_get_spi_clk());
        return 0;
    }
    if (argc > 2 && !strcmp(argv[1], "bl"))  { st7789p3_set_backlight(atoi(argv[2])); return 0; }
    if (argc > 2 && !strcmp(argv[1], "bgr")) { st7789p3_set_bgr(atoi(argv[2])); return 0; }
    printf("lcd - ST7789P3 panel\n  lcd clk <hz> | lcd bl <0|1> | lcd bgr <0|1>\n");
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lcd, lcd, ST7789P3 panel knobs);

#endif /* CONFIG_SHELL */
