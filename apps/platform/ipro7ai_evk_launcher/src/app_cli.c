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
 * @brief `app` shell command: pages, events, gesture and voice knobs, status.
 *
 * The board-level `lcd`, `cam`, `mic`, `spk`, `sd` commands come from
 * components/board/ipro7ai_evk (CONFIG_EVK_SHELL).
 */

#include <generated/autoconf.h>

#ifdef CONFIG_SHELL

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "ui.h"
#include "gesture.h"
#include "detect.h"
#include "voice.h"

void app_trace_set(int on);                /* main.c */

static int parse_onoff(const char *s, bool *out)
{
    if (!strcmp(s, "on") || !strcmp(s, "1"))  { *out = true;  return 0; }
    if (!strcmp(s, "off") || !strcmp(s, "0")) { *out = false; return 0; }
    return -1;
}

static void usage(void)
{
    printf(
      "app - IPRO7AI EVK launcher\n"
      "  app status               everything, one screen\n"
      "  app page <1..%d>          jump to a page\n"
      "  app det <1..99>          detector confidence threshold (default 25)\n"
      "  app detrate <ms>         minimum ms between detections (default 200)\n"
      "  app trace <on|off>       periodic health + run-time stats line\n"
      "  app detprint <on|off>    print every detection list\n"
      "  app next | select | alt  what PAPER / ROCK / SCISSORS would do\n"
      "  app rec [seconds]        record and play back (voice page action)\n"
      "  app play | stop\n"
      "  app gain <0..40>         playback make-up gain, dB\n"
      "  app hold <ms> <ms> <pct> gesture hold time, release time, confidence floor\n"
      "  app feed <raw|xor|copy>  how the frame reaches the NPU (bench knob)\n"
      "  app print <on|off>       print every classification\n"
      "  app mirror <on|off>      selfie mirror (default on)\n"
      "  app uvswap <on|off>      swap U/V if skin comes out blue\n"
      "  app npu <on|off>         feed the NPU / stop (preview stays)\n",
      ui_page_count());
}

static void status(void)
{
    gesture_stats_t  gs;
    gesture_result_t r;
    ui_stats_t       us;
    evk_cam_stats_t  cs;
    voice_status_t   vs;
    evk_mic_stats_t  ms;

    gesture_get_stats(&gs);
    gesture_get_result(&r);
    ui_get_stats(&us);
    evk_cam_get_stats(&cs);
    voice_get_status(&vs);
    evk_mic_get_stats(&ms);

    printf("page    : %d/%d %s, %lu events, ui %lu.%lu fps %lu ms/frame, %lu drawn %lu dropped\n",
           ui_page() + 1, ui_page_count(), ui_page_name(ui_page()), (unsigned long)us.events,
           (unsigned long)(us.fps_x10 / 10u), (unsigned long)(us.fps_x10 % 10u),
           (unsigned long)us.blit_ms, (unsigned long)us.drawn, (unsigned long)us.dropped);
    if (gesture_ready()) {
        printf("model   : %s, %u B, loaded in %u ms, %ux%u ext_input %u, scale q16 %u zp %d, "
               "%u KB heap left\n",
               gesture_source(), (unsigned)gs.model_bytes, (unsigned)gs.load_ms,
               (unsigned)gs.in_w, (unsigned)gs.in_h, gs.ext_input, (unsigned)gs.scale_q16,
               gs.out_zp, (unsigned)(gs.free_heap / 1024u));
    } else {
        printf("model   : NOT LOADED (%s)\n", gesture_fail_reason());
    }
    printf("npu     : %s, feed %s, %lu.%lu fps, %u frames, %u errors, last %lu ms\n",
           evk_cam_npu_enabled() ? "running" : "stopped", gesture_feed_name(gesture_get_feed()),
           (unsigned long)(gs.fps_x10 / 10u), (unsigned long)(gs.fps_x10 % 10u),
           (unsigned)gs.frames, (unsigned)gs.errors, (unsigned long)r.npu_ms);
    printf("last    : #%lu %s %u%%  p=%u/%u/%u/%u  raw=%u %u %u %u\n",
           (unsigned long)r.seq, gesture_name(r.cls), r.conf,
           r.prob[0], r.prob[1], r.prob[2], r.prob[3], r.raw[0], r.raw[1], r.raw[2], r.raw[3]);
    {
        detect_stats_t  ds;
        detect_result_t dr;
        detect_get_stats(&ds);
        detect_get_result(&dr);
        if (detect_is_ready()) {
            printf("detect  : sd card, loaded in %u ms, %lu.%lu fps, %u frames, %u errors; last #%lu "
                   "%u box (%u person %u pet) npu %lu ms post %lu ms, conf %d%%\n",
                   (unsigned)ds.load_ms, (unsigned long)(ds.fps_x10 / 10u),
                   (unsigned long)(ds.fps_x10 % 10u), (unsigned)ds.frames, (unsigned)ds.errors,
                   (unsigned long)dr.seq, dr.count, dr.persons, dr.pets,
                   (unsigned long)dr.npu_ms, (unsigned long)dr.post_ms, detect_get_conf());
        } else {
            printf("detect  : NOT LOADED (%s)\n", detect_fail_reason());
        }
    }
    printf("camera  : %s; isp ch0 %u ch1 %u (%u to detector) err %u/%u/%u\n",
           evk_cam_is_running() ? "running" : "stopped",
           (unsigned)cs.frames0, (unsigned)cs.frames1, (unsigned)cs.pushed,
           (unsigned)cs.fifo_err, (unsigned)cs.vsync_err, (unsigned)cs.hsync_err);
    printf("voice   : state %d, clip %lu ms, pos %lu ms, peak %u (%d.%d dBFS), gain +%d dB, "
           "mic isr %u delivered %u dropped %u%s%s\n",
           (int)vs.state, (unsigned long)vs.clip_ms, (unsigned long)vs.pos_ms, vs.peak,
           vs.peak_dbfs_x10 / 10, abs(vs.peak_dbfs_x10 % 10), voice_get_gain_db(),
           (unsigned)ms.isr_frames, (unsigned)ms.delivered, (unsigned)ms.dropped,
           vs.error[0] ? ", error: " : "", vs.error);
}

static int cmd_app(int argc, char **argv)
{
    bool on;

    if (argc < 2 || !strcmp(argv[1], "status")) { status(); return 0; }
    if (!strcmp(argv[1], "next"))   { ui_inject(INPUT_EV_NEXT);   return 0; }
    if (!strcmp(argv[1], "select")) { ui_inject(INPUT_EV_SELECT); return 0; }
    if (!strcmp(argv[1], "alt"))    { ui_inject(INPUT_EV_ALT);    return 0; }
    if (!strcmp(argv[1], "page") && argc > 2) {
        int p = atoi(argv[2]);
        if (p < 1 || p > ui_page_count()) { printf("page must be 1..%d\n", ui_page_count()); return -1; }
        ui_set_page(p - 1);
        return 0;
    }
    if (!strcmp(argv[1], "rec")) {
        uint32_t s = argc > 2 ? (uint32_t)atoi(argv[2]) : VOICE_DEFAULT_SECONDS;
        if (ui_page() != 1) ui_set_page(1);
        if (voice_record(s, true) != 0) { printf("record refused (busy, or 1..%d s)\n", VOICE_MAX_SECONDS); return -1; }
        return 0;
    }
    if (!strcmp(argv[1], "play")) {
        if (ui_page() != 1) ui_set_page(1);
        if (voice_play() != 0) { printf("nothing recorded, or busy\n"); return -1; }
        return 0;
    }
    if (!strcmp(argv[1], "stop"))   { voice_stop(); return 0; }
    if (!strcmp(argv[1], "detrate") && argc > 2) {
        if (detect_set_period_ms((uint32_t)atoi(argv[2])) != DETECT_OK) { printf("0..5000 ms\n"); return -1; }
        return 0;
    }
    if (!strcmp(argv[1], "det") && argc > 2) {
        if (detect_set_conf(atoi(argv[2])) != DETECT_OK) { printf("conf must be 1..99\n"); return -1; }
        return 0;
    }
    if (!strcmp(argv[1], "gain") && argc > 2) {
        if (voice_set_gain_db(atoi(argv[2])) != 0) { printf("gain must be 0..40 dB\n"); return -1; }
        return 0;
    }
    if (!strcmp(argv[1], "hold") && argc > 4) {
        ui_input_config((uint32_t)atoi(argv[2]), (uint32_t)atoi(argv[3]), atoi(argv[4]));
        return 0;
    }
    if (!strcmp(argv[1], "feed") && argc > 2) {
        gesture_feed_t f;
        if (!strcmp(argv[2], "raw"))       f = GESTURE_FEED_RAW;
        else if (!strcmp(argv[2], "xor"))  f = GESTURE_FEED_XOR;
        else if (!strcmp(argv[2], "copy")) f = GESTURE_FEED_COPY;
        else { usage(); return -1; }
        gesture_set_feed(f);
        return 0;
    }
    if (argc > 2 && parse_onoff(argv[2], &on) == 0) {
        if (!strcmp(argv[1], "print"))    { gesture_set_print(on);       return 0; }
        if (!strcmp(argv[1], "detprint")) { detect_set_print(on);        return 0; }
        if (!strcmp(argv[1], "trace"))    { app_trace_set(on ? 1 : 0);   return 0; }
        if (!strcmp(argv[1], "mirror")) { ui_set_mirror(on);             return 0; }
        if (!strcmp(argv[1], "uvswap")) { ui_set_uv_swap(on);            return 0; }
        if (!strcmp(argv[1], "npu"))    { evk_cam_npu_set_enabled(on);   return 0; }
    }
    usage();
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_app, app, EVK launcher);

#endif /* CONFIG_SHELL */
