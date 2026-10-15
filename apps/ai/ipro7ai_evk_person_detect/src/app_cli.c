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
 * @brief `det`, `cam` and `lcd` shell commands.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_SHELL

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "detect.h"
#include "evk_cam.h"
#include "evk_cam_port.h"
#include "display.h"
#include "st7789p3.h"

static int parse_onoff(const char *s, bool *out)
{
    if (!strcmp(s, "on") || !strcmp(s, "1")) {
        *out = true;
        return 0;
    }
    if (!strcmp(s, "off") || !strcmp(s, "0")) {
        *out = false;
        return 0;
    }
    return -1;
}

/* ---------------------------------------------------------------------------
 * det
 * -------------------------------------------------------------------------*/

static void det_usage(void)
{
    printf(
      "det - YOLOv8 person/pet detection on the NPU\n"
      "  det status            model, rates, counters\n"
      "  det start | stop      feed frames to the NPU / stop (preview stays)\n"
      "  det conf <1..99>      confidence threshold in %% (default 25)\n"
      "  det pets <on|off>     draw pet boxes as well as persons\n"
      "  det hud <on|off>      show the text band on the preview\n"
      "  det print <on|off>    print every detection list to the console\n"
      "  det uvswap <on|off>   swap U/V if skin comes out blue\n");
}

static void det_status(void)
{
    detect_stats_t  ds;
    detect_result_t r;
    display_stats_t disp;
    evk_cam_stats_t     cs;

    detect_get_stats(&ds);
    detect_get_result(&r);
    display_get_stats(&disp);
    evk_cam_get_stats(&cs);

    if (detect_is_ready()) {
        printf("model   : %u B from 'mfg', loaded in %u ms, %u KB PSRAM left\n",
               (unsigned)ds.model_bytes, (unsigned)ds.load_ms,
               (unsigned)(ds.free_heap / 1024u));
    } else {
        printf("model   : NOT LOADED (%s)\n", detect_fail_reason());
    }
    printf("detect  : %s, conf %d%%, %lu.%lu fps, %u frames, %u errors\n",
           evk_cam_npu_enabled() ? "running" : "stopped", detect_get_conf(),
           (unsigned long)(ds.fps_x10 / 10u), (unsigned long)(ds.fps_x10 % 10u),
           (unsigned)ds.frames, (unsigned)ds.errors);
    printf("last    : #%lu %u box (%u person, %u pet), NPU %lu ms, decode %lu ms\n",
           (unsigned long)r.seq, r.count, r.persons, r.pets,
           (unsigned long)r.npu_ms, (unsigned long)r.post_ms);
    for (int i = 0; i < r.count; i++) {
        printf("          %-6s %3u%%  [%3d,%3d]-[%3d,%3d]\n",
               r.box[i].cls == DETECT_CLASS_PERSON ? "PERSON" : "PET",
               r.box[i].conf, r.box[i].x1, r.box[i].y1, r.box[i].x2, r.box[i].y2);
    }
    printf("preview : %lu.%lu fps drawn, %u ms/frame, %u drawn, %u dropped\n",
           (unsigned long)(disp.fps_x10 / 10u), (unsigned long)(disp.fps_x10 % 10u),
           (unsigned)disp.blit_ms, (unsigned)disp.drawn, (unsigned)disp.dropped);
    printf("isp     : ch0 %u frames, ch1 %u frames (%u to NPU), "
           "errors fifo %u vsync %u hsync %u\n",
           (unsigned)cs.frames0, (unsigned)cs.frames1, (unsigned)cs.pushed,
           (unsigned)cs.fifo_err, (unsigned)cs.vsync_err, (unsigned)cs.hsync_err);
    printf("options : pets %s, hud %s, print %s, uvswap %s\n",
           display_get_pets() ? "on" : "off", display_get_hud() ? "on" : "off",
           detect_get_print() ? "on" : "off", display_get_uv_swap() ? "on" : "off");
}

static int cmd_det(int argc, char **argv)
{
    bool on;

    if (argc < 2 || !strcmp(argv[1], "status")) {
        det_status();
        return 0;
    }
    if (!strcmp(argv[1], "start")) {
        if (!detect_is_ready()) {
            printf("no model loaded (%s)\n", detect_fail_reason());
            return -1;
        }
        evk_cam_npu_set_enabled(true);
        printf("detection on\n");
        return 0;
    }
    if (!strcmp(argv[1], "stop")) {
        evk_cam_npu_set_enabled(false);
        printf("detection off (preview continues)\n");
        return 0;
    }
    if (!strcmp(argv[1], "conf") && argc > 2) {
        if (detect_set_conf(atoi(argv[2])) != DETECT_OK) {
            printf("conf must be 1..99\n");
            return -1;
        }
        printf("conf %d%%\n", detect_get_conf());
        return 0;
    }
    if (argc > 2 && parse_onoff(argv[2], &on) == 0) {
        if (!strcmp(argv[1], "pets"))   { display_set_pets(on);    return 0; }
        if (!strcmp(argv[1], "hud"))    { display_set_hud(on);     return 0; }
        if (!strcmp(argv[1], "print"))  { detect_set_print(on);    return 0; }
        if (!strcmp(argv[1], "uvswap")) { display_set_uv_swap(on); return 0; }
    }
    det_usage();
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_det, det, NPU person detection);

/* ---------------------------------------------------------------------------
 * cam
 * -------------------------------------------------------------------------*/

static void evk_cam_usage(void)
{
    printf(
      "cam - SC101IOT camera -> ISP -> preview + NPU\n"
      "  cam status              sensor, geometry, counters\n"
      "  cam start | stop        pipeline + preview (stop parks the sensor)\n"
      "  cam fit <box|crop>      box = whole frame letterboxed 320x180,\n"
      "                          crop = centre 960x720 fills 320x240;\n"
      "                          takes effect on the next `cam start`\n"
      "  cam fps <1..30>         preview frame rate (live, default 10). Each\n"
      "                          frame is ~60 ms of CPU-polled SPI at 16 MHz;\n"
      "                          raise `lcd clk` before going above 12.\n"
      "  cam rot <0..3>          panel orientation; 1 and 3 landscape\n"
      "  cam rst <0|1>           drive the sensor RESET line (0 = asserted)\n"
      "  wiring: SCL=GPIO%d SDA=GPIO%d  D0-D7=GPIO%d-%d  PCLK=GPIO%d\n"
      "          VSYNC=GPIO%d HSYNC=GPIO%d MCLK=GPIO%d RST=GPIO%d (1.8V bus)\n",
      EVK_CAM_PIN_SCL, EVK_CAM_PIN_SDA, EVK_CAM_PIN_D0, EVK_CAM_PIN_D0 + 7, EVK_CAM_PIN_PCLK,
      EVK_CAM_PIN_VSYNC, EVK_CAM_PIN_HSYNC, EVK_CAM_PIN_MCLK, EVK_CAM_PIN_RST);
}

static void evk_cam_status(void)
{
    evk_cam_geometry_t g;
    evk_cam_stats_t    st;
    uint16_t       sw, sh;

    evk_cam_sensor_size(&sw, &sh);
    evk_cam_get_geometry(&g);
    evk_cam_get_stats(&st);

    if (evk_cam_sensor_name()) {
        printf("sensor  : %s @ 0x%02X, %ux%u\n", evk_cam_sensor_name(),
               evk_cam_sensor_addr(), (unsigned)sw, (unsigned)sh);
    } else {
        printf("sensor  : not probed\n");
    }
    printf("pipeline: %s, %s, %d fps, rot %d\n",
           evk_cam_is_running() ? "running" : "stopped",
           evk_cam_get_fit() == EVK_CAM_FIT_CROP ? "centre-crop" : "letterbox",
           evk_cam_get_fps(), evk_cam_get_rot());
    printf("preview : %ux%u at (%u,%u) on %ux%u, from sensor window "
           "%ux%u+%u+%u\n",
           (unsigned)g.img_w, (unsigned)g.img_h, (unsigned)g.ox, (unsigned)g.oy,
           (unsigned)g.panel_w, (unsigned)g.panel_h,
           (unsigned)g.crop_w, (unsigned)g.crop_h, (unsigned)g.crop_x,
           (unsigned)g.crop_y);
    printf("frames  : ch0 %u, ch1 %u (%u to NPU), errors fifo %u vsync %u "
           "hsync %u\n",
           (unsigned)st.frames0, (unsigned)st.frames1, (unsigned)st.pushed,
           (unsigned)st.fifo_err, (unsigned)st.vsync_err, (unsigned)st.hsync_err);
}

static int cmd_cam(int argc, char **argv)
{
    int rc;

    if (argc < 2 || !strcmp(argv[1], "status")) {
        evk_cam_status();
        return 0;
    }
    if (!strcmp(argv[1], "start")) {
        rc = evk_cam_start();
        if (rc == EVK_CAM_OK) {
            display_start();
        } else {
            printf("cam start failed (%d)\n", rc);
        }
        return rc;
    }
    if (!strcmp(argv[1], "stop")) {
        display_stop();
        evk_cam_stop();
        display_splash("PREVIEW OFF", "cam start");
        return 0;
    }
    if (!strcmp(argv[1], "fit") && argc > 2) {
        if (!strcmp(argv[2], "box"))       { rc = evk_cam_set_fit(EVK_CAM_FIT_LETTERBOX); }
        else if (!strcmp(argv[2], "crop")) { rc = evk_cam_set_fit(EVK_CAM_FIT_CROP); }
        else                               { rc = EVK_CAM_ERR_PARAM; }
        if (rc == EVK_CAM_OK) {
            printf("fit %s - restart with `cam stop` / `cam start`\n", argv[2]);
        }
        return rc;
    }
    if (!strcmp(argv[1], "fps") && argc > 2) {
        rc = evk_cam_set_fps(atoi(argv[2]));
        if (rc != EVK_CAM_OK) {
            printf("fps must be 1..30\n");
        }
        return rc;
    }
    if (!strcmp(argv[1], "rot") && argc > 2) {
        rc = evk_cam_set_rot(atoi(argv[2]));
        if (rc == EVK_CAM_OK) {
            printf("rot %d - restart with `cam stop` / `cam start`\n", evk_cam_get_rot());
        }
        return rc;
    }
    if (!strcmp(argv[1], "rst") && argc > 2) {
        evk_cam_reset_pin(atoi(argv[2]));
        return 0;
    }
    evk_cam_usage();
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cam, cam, SC101IOT camera pipeline);

/* ---------------------------------------------------------------------------
 * lcd - the handful of panel knobs that matter once the wiring is proven.
 * The full bring-up tool is apps/platform/ipro7ai_evk_demo (`lcd id`, patterns).
 * -------------------------------------------------------------------------*/

static int cmd_lcd(int argc, char **argv)
{
    if (argc > 2 && !strcmp(argv[1], "clk")) {
        st7789p3_set_spi_clk((uint32_t)strtoul(argv[2], NULL, 0));
        printf("SPI clock request %lu Hz\n", (unsigned long)st7789p3_get_spi_clk());
        return 0;
    }
    if (argc > 2 && !strcmp(argv[1], "bl")) {
        st7789p3_set_backlight(atoi(argv[2]));
        return 0;
    }
    if (argc > 2 && !strcmp(argv[1], "bgr")) {
        st7789p3_set_bgr(atoi(argv[2]));
        return 0;
    }
    if (argc > 1 && !strcmp(argv[1], "id")) {
        uint8_t id[3] = {0};
        st7789p3_read_reg(0x04, id, 3);
        printf("RDDID: %02X %02X %02X\n", id[0], id[1], id[2]);
        return 0;
    }
    printf("lcd - ST7789P3 panel\n"
           "  lcd clk <hz>     SPI clock (default %u; 32000000 doubles the\n"
           "                   preview rate if the wiring takes it)\n"
           "  lcd bl <0|1>     backlight enable\n"
           "  lcd bgr <0|1>    MADCTL colour order if red/blue are swapped\n"
           "  lcd id           read RDDID over the shared SDA line\n",
           (unsigned)ST_SPI_CLK_HZ);
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lcd, lcd, ST7789P3 panel knobs);

#endif /* CONFIG_SHELL */
