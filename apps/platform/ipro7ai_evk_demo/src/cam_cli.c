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
 * @file cam_cli.c
 * @brief `cam` shell command for the SC101IOT preview.
 */

#include "cam.h"
#include "cam_port.h"
#include "st7789p3.h"

#ifdef CONFIG_SHELL

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>

static void usage(void)
{
    printf(
      "cam - SC101IOT DVP camera preview on the ST7789P3 panel\n"
      "  cam status              probe state, geometry, frame counters\n"
      "  cam probe               SCCB + MCLK + reset + sensor probe (no stream)\n"
      "  cam start               start the ISP pipeline and live preview\n"
      "  cam stop                stop the preview\n"
      "  cam fit <box|crop>      16:9 sensor on a 4:3 panel:\n"
      "                            box  = whole frame letterboxed 320x180\n"
      "                            crop = centre 960x720 fills 320x240\n"
      "                          takes effect on the next `cam start`\n"
      "  cam fps <1..30>         preview frame rate target (applied live)\n"
      "  cam rot <0..3>          panel orientation; 1 and 3 landscape,\n"
      "                          0 and 2 portrait. Restart the preview.\n"
      "  cam order <rgb|bgr>     which channel is byte 0 of an ISP pixel;\n"
      "                          use it when red and blue look swapped\n"
      "  cam pixels [n]          dump raw ARGB words of the newest frame\n"
      "  cam rst <0|1>           drive the sensor RESET line (0 = asserted)\n"
      "  wiring: SCL=GPIO%d SDA=GPIO%d  D0-D7=GPIO%d-%d  PCLK=GPIO%d\n"
      "          VSYNC=GPIO%d HSYNC=GPIO%d MCLK=GPIO%d RST=GPIO%d (1.8V bus)\n",
      CAM_PIN_SCL, CAM_PIN_SDA, CAM_PIN_D0, CAM_PIN_D0 + 7, CAM_PIN_PCLK,
      CAM_PIN_VSYNC, CAM_PIN_HSYNC, CAM_PIN_MCLK, CAM_PIN_RST);
}

static void cmd_status(void)
{
    cam_stats_t st;
    uint16_t sw, sh, pw, ph;

    cam_sensor_size(&sw, &sh);
    cam_preview_size(&pw, &ph);
    cam_get_stats(&st);

    if (cam_sensor_name()) {
        printf("sensor  : %s @ 0x%02X, %ux%u\n", cam_sensor_name(),
               cam_sensor_addr(), (unsigned)sw, (unsigned)sh);
    } else {
        printf("sensor  : not probed (run `cam probe`)\n");
    }
    printf("preview : %ux%u, %s, %d fps target, %s\n",
           (unsigned)pw, (unsigned)ph,
           (cam_get_fit() == CAM_FIT_CROP) ? "centre-crop" : "letterbox",
           cam_get_fps(), cam_is_running() ? "RUNNING" : "stopped");
    printf("colour  : byte0 = %s\n",
           (cam_get_order() == CAM_ORDER_RGB) ? "R (rgb)" : "B (bgr)");
    printf("rotation: %d (%s)\n", cam_get_rot(),
           (cam_get_rot() & 1) ? "landscape 320x240" : "portrait 240x320");
    printf("frames  : %u from ISP, %u drawn, %u dropped\n",
           (unsigned)st.frames, (unsigned)st.drawn, (unsigned)st.dropped);
    printf("isp err : fifo %u, vsync %u, hsync %u\n",
           (unsigned)st.fifo_err, (unsigned)st.vsync_err,
           (unsigned)st.hsync_err);

    if (st.frames && !st.drawn) {
        printf("hint    : ISP is producing frames but none reached the panel -\n"
               "          check that `lcd init` ran and the preview task started.\n");
    }
    if (st.drawn && cam_is_running()) {
        printf("hint    : frames are reaching the panel. If the screen still\n"
               "          looks blank the image itself is dark - `cam pixels`\n"
               "          shows whether the ISP is delivering non-zero pixels.\n");
    }
    if (!st.frames && cam_is_running()) {
        printf("hint    : still no ISP completions after waiting. PCLK/VSYNC/\n"
               "          HSYNC or the 1.8 V VDDIO1 rail are the usual causes;\n"
               "          `cam rst 0` then `cam rst 1` re-pulses the sensor.\n");
    }
}

int cmd_cam(int argc, char **argv)
{
    const char *sub;

    if (argc < 2) {
        usage();
        return 0;
    }
    sub = argv[1];

    if (strcmp(sub, "help") == 0) {
        usage();
    } else if (strcmp(sub, "status") == 0) {
        cmd_status();
    } else if (strcmp(sub, "probe") == 0) {
        if (cam_probe() == CAM_OK) {
            cmd_status();
        } else {
            printf("no sensor found on SCCB\n");
        }
    } else if (strcmp(sub, "start") == 0) {
        int rc = cam_start();
        if (rc == CAM_OK) {
            /*
             * Wait for the first frame before reporting. cam_start() returns as
             * soon as the pipeline is armed, but at 10 fps with 6 convergence
             * frames discarded the first one is ~700 ms away - printing
             * immediately showed "0 frames from ISP" and a hint telling you to
             * go measure the 1.8 V rail, on a camera that was about to work.
             */
            cam_stats_t st;
            for (int i = 0; i < 40; i++) {
                cam_get_stats(&st);
                if (st.drawn) {
                    break;      /* a frame has actually reached the panel */
                }
                vTaskDelay(pdMS_TO_TICKS(50));
            }
            cmd_status();
        } else if (rc == CAM_ERR_STATE) {
            printf("run `lcd init` first\n");
        } else if (rc == CAM_ERR_NODEV) {
            printf("no sensor found on SCCB\n");
        } else if (rc == CAM_ERR_NOMEM) {
            printf("out of PSRAM for the frame buffers\n");
        } else {
            printf("start failed (%d)\n", rc);
        }
    } else if (strcmp(sub, "stop") == 0) {
        cam_stop();
        printf("preview stopped\n");
    } else if (strcmp(sub, "fit") == 0) {
        if (argc < 3) {
            printf("usage: cam fit <box|crop>\n");
        } else if (strcmp(argv[2], "box") == 0) {
            cam_set_fit(CAM_FIT_LETTERBOX);
            printf("fit = letterbox (restart preview to apply)\n");
        } else if (strcmp(argv[2], "crop") == 0) {
            cam_set_fit(CAM_FIT_CROP);
            printf("fit = centre-crop (restart preview to apply)\n");
        } else {
            printf("usage: cam fit <box|crop>\n");
        }
    } else if (strcmp(sub, "rot") == 0) {
        if (argc < 3) {
            printf("rotation = %d\n", cam_get_rot());
        } else if (cam_set_rot(atoi(argv[2])) != CAM_OK) {
            printf("rotation must be 0..3 (1 and 3 landscape, 0 and 2 portrait)\n");
        } else {
            printf("rotation = %d (%s) - restart the preview to apply:\n"
                   "  cam stop ; cam start\n", cam_get_rot(),
                   (cam_get_rot() & 1) ? "landscape 320x240"
                                       : "portrait 240x320");
        }
    } else if (strcmp(sub, "fps") == 0) {
        if (argc < 3) {
            printf("fps = %d\n", cam_get_fps());
        } else if (cam_set_fps(atoi(argv[2])) != CAM_OK) {
            printf("fps must be 1..30\n");
        } else {
            printf("fps = %d\n", cam_get_fps());
        }
    } else if (strcmp(sub, "order") == 0) {
        if (argc < 3) {
            printf("order = %s\n",
                   (cam_get_order() == CAM_ORDER_RGB) ? "rgb" : "bgr");
        } else if (strcmp(argv[2], "rgb") == 0) {
            cam_set_order(CAM_ORDER_RGB);
            printf("order = rgb (byte0 = R)\n");
        } else if (strcmp(argv[2], "bgr") == 0) {
            cam_set_order(CAM_ORDER_BGR);
            printf("order = bgr (byte0 = B)\n");
        } else {
            printf("usage: cam order <rgb|bgr>\n");
        }
    } else if (strcmp(sub, "pixels") == 0) {
        uint32_t n = (argc > 2) ? (uint32_t)atoi(argv[2]) : 8u;
        if (cam_dump_pixels(n) != CAM_OK) {
            printf("no frame captured yet - run `cam start` first\n");
        }
    } else if (strcmp(sub, "rst") == 0) {
        if (argc < 3) {
            printf("usage: cam rst <0|1>\n");
        } else {
            int lvl = atoi(argv[2]);
            cam_reset_pin(lvl);
            printf("GPIO%d (sensor RESET) driven %s -> sensor reset %s\n",
                   CAM_PIN_RST, lvl ? "HIGH" : "LOW",
                   lvl ? "released" : "asserted");
        }
    } else {
        usage();
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_cam, cam, SC101IOT camera preview);

#endif /* CONFIG_SHELL */
