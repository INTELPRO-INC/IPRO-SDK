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
 * @file main.c
 * @brief IPRO7AI EVK person detection: camera -> NPU (YOLOv8) -> LCD.
 *
 *   SC101IOT 1280x720 DVP
 *      -> ISP ch0  YUYV 320x180  -> CPU RGB565 + boxes + HUD -> ST7789P3 SPI
 *      -> ISP ch1  ARGB 640x360  -> NPU person_pet_detect -> boxes
 *
 * Everything starts at power-on: panel, model load (~1 s, from the "mfg"
 * flash partition), then the camera. A board without a model still shows
 * the live preview and says "NO MODEL" on the HUD. The shell (115200 8N1 on
 * the platform UART) has `det`, `cam` and `lcd` for the knobs.
 *
 * Board support is bsp/board/ipro7/ipro7ai_evk_board_config.h
 * (CONFIG_IPRO7AI_EVK). Wiring:
 *     LCD  SCL=GPIO15 SDA=GPIO16 CS=GPIO10 D/C=GPIO17 RESET=GPIO14 BL=GPIO12
 *     CAM  SCL=GPIO36 SDA=GPIO37 D0-D7=GPIO0-7 PCLK=38 VSYNC=39 HSYNC=40
 *          MCLK=41 RESET=35 (level-shifted)
 *     J6   pin 3 (VDDIO1) wired to pin 7 (2V8_CAM) - the only VDDIO1 supply
 *          on V1 boards; see bsp/board/ipro7/IPRO7AI_EVK.md
 */

#include <stdint.h>
#include <stddef.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "ipro_log.h"

#include "display.h"
#include "detect.h"
#include "evk_cam.h"

#define LOG_TAG "MAIN"

static int npu_frame(uint32_t addr, void *arg)
{
    (void)arg;
    return detect_push_frame(addr);
}

static void detect_done(void *arg)
{
    (void)arg;
    evk_cam_npu_rearm();                /* let the ISP capture the next NPU frame */
}

static void app_task(void *param)
{
    int rc;

    (void)param;

    if (display_init() != 0) {
        IPRO_LOGE(LOG_TAG, "no panel - continuing headless");
    }

    /* Model before camera: the NPU's activation buffer is one ~3 MB block
     * and wants the PSRAM heap while it is still empty. */
    rc = detect_init();
    if (rc == DETECT_OK) {
        /* Channel 1: the whole 1280x720 frame scaled 2x down to the model's
         * 640x360, into the detector's buffer. */
        evk_cam_npu_input_t in = {
            .addr  = detect_input_addr(),
            .out_w = DETECT_IN_W, .out_h = DETECT_IN_H,
            .roi_x = 0, .roi_y = 0, .roi_w = 1280, .roi_h = 720,
        };
        evk_cam_set_npu_input(&in);
        evk_cam_set_npu_frame_cb(npu_frame, NULL);
        detect_set_done_cb(detect_done, NULL);
    } else {
        IPRO_LOGW(LOG_TAG, "detection unavailable (%d: %s) - preview only",
                  rc, detect_fail_reason());
    }

    rc = evk_cam_start();
    if (rc != EVK_CAM_OK) {
        IPRO_LOGE(LOG_TAG, "camera start failed (%d) - check J6 (VDDIO1, pin 3 "
                           "wired to pin 7) and the FPC", rc);
        display_splash("NO CAMERA", "check J6 3-7 then `cam start`");
    } else {
        display_start();
    }

#ifdef CONFIG_SHELL
    IPRO_LOGI(LOG_TAG, "shell ready - `det status`, `det conf 30`, "
                       "`cam fit crop`, `lcd clk 32000000`");
#endif

    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

int main(void)
{
    static StackType_t  s_app_stack[2048];
    static StaticTask_t s_app_tcb;

    ipro_platform_init();   /* heap + boot2 + log + shell */

    /* Park the camera before anything else. The sensor's RESETB is released
     * by default, so a sensor left configured by a previous session keeps
     * streaming through the next boot and loads the 1V8 LDO that also feeds
     * the SoC's VDDIO_1 bank (where the BOOT strap lives). */
    evk_cam_stop();

    IPRO_LOGI(LOG_TAG, "==========================================");
    IPRO_LOGI(LOG_TAG, "  IPRO7AI EVK  person detection -> LCD");
    IPRO_LOGI(LOG_TAG, "  CAM SC101IOT 720p -> ISP ch0 320x180 preview");
    IPRO_LOGI(LOG_TAG, "                    -> ISP ch1 640x360 -> NPU");
    IPRO_LOGI(LOG_TAG, "  LCD ST7789P3 320x240 SPI0  model: mfg partition");
    IPRO_LOGI(LOG_TAG, "==========================================");

    xTaskCreateStatic(app_task, "app",
                      sizeof(s_app_stack) / sizeof(StackType_t), NULL,
                      configMAX_PRIORITIES - 2,
                      s_app_stack, &s_app_tcb);

    vTaskStartScheduler();
    while (1) {
    }
    return 0;
}
