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
 * @brief Rock-paper-scissors against the IPRO7AI EVK's NPU.
 *
 *   SC101IOT 1280x720 DVP
 *      -> ISP ch0  YUYV 320x180        -> preview + play box + scores -> ST7789P3
 *      -> ISP ch1  512x512 ROI -> 128x128 ARGB -> NPU classifier (rock/paper/
 *                                                 scissors/none) -> game
 *
 * Hold your hand in the box, the board counts 3-2-1, you both throw, it
 * scores. Everything starts at power-on; the shell (921600 8N1 - faster than
 * the person app because `rps capture` streams training frames over it) has
 * `rps`, `cam` and `lcd`.
 *
 * Board support: bsp/board/ipro7/ipro7ai_evk_board_config.h (CONFIG_IPRO7AI_EVK).
 * J6: wire pin 3 (VDDIO1) to pin 7 (2V8_CAM) for the camera - the only VDDIO1
 * supply on V1 boards. BOOT is not on J6; see bsp/board/ipro7/IPRO7AI_EVK.md.
 */

#include <stdint.h>
#include <stddef.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "ipro_log.h"

#include "rps_ui.h"
#include "rps_model.h"
#include "rps_game.h"
#include "evk_cam.h"

#define LOG_TAG "MAIN"

/* The play box: a 512x512 square in the middle of the 1280x720 sensor,
 * scaled 4x down to the model's 128x128. 4x is the ratio the preview
 * already uses, so no new scaler territory. */
#define ROI_W   512
#define ROI_H   512
#define ROI_X   ((1280 - ROI_W) / 2)
#define ROI_Y   ((720 - ROI_H) / 2)

static int npu_frame(uint32_t addr, void *arg)
{
    (void)arg;
    return rps_model_push(addr);
}

static void npu_done(void *arg)
{
    (void)arg;
    evk_cam_npu_rearm();
}

static void app_task(void *param)
{
    int rc;

    (void)param;

    if (rps_ui_init() != 0) {
        IPRO_LOGE(LOG_TAG, "no panel - continuing headless");
    }
    rps_game_init();

    rc = rps_model_init();
    if (rc == 0) {
        evk_cam_npu_input_t in = {
            .addr  = rps_model_input_addr(),
            .out_w = RPS_IN_W, .out_h = RPS_IN_H,
            .roi_x = ROI_X, .roi_y = ROI_Y, .roi_w = ROI_W, .roi_h = ROI_H,
        };
        evk_cam_set_npu_input(&in);
        evk_cam_set_npu_frame_cb(npu_frame, NULL);
        rps_model_set_done_cb(npu_done, NULL);
    } else {
        /* Still draw the box so the screen explains itself. */
        evk_cam_npu_input_t in = {
            .addr = 0, .out_w = RPS_IN_W, .out_h = RPS_IN_H,
            .roi_x = ROI_X, .roi_y = ROI_Y, .roi_w = ROI_W, .roi_h = ROI_H,
        };
        evk_cam_set_npu_input(&in);
        IPRO_LOGW(LOG_TAG, "classifier unavailable (%s) - preview only",
                  rps_model_fail_reason());
    }

    rc = evk_cam_start();
    if (rc != EVK_CAM_OK) {
        IPRO_LOGE(LOG_TAG, "camera start failed (%d) - check J6 pin 3-7 (VDDIO1)", rc);
        rps_ui_splash("NO CAMERA", "check J6 3-7 then `cam start`");
    } else {
        rps_ui_start();
    }

#ifdef CONFIG_SHELL
    IPRO_LOGI(LOG_TAG, "shell ready - `rps status`, `rps print on`, `rps capture rock`");
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

    /* Park the camera before anything else (its RESETB defaults to released;
     * a sensor left streaming loads the 1V8 LDO that feeds the BOOT strap). */
    evk_cam_stop();

    IPRO_LOGI(LOG_TAG, "==========================================");
    IPRO_LOGI(LOG_TAG, "  IPRO7AI EVK  rock / paper / scissors");
    IPRO_LOGI(LOG_TAG, "  CAM 720p -> ISP ch0 320x180 preview");
    IPRO_LOGI(LOG_TAG, "              ISP ch1 512x512 ROI -> 128x128 -> NPU");
#ifdef CONFIG_RPS_EMBED_MODEL
    IPRO_LOGI(LOG_TAG, "  LCD ST7789P3 320x240   model: in firmware");
#else
    IPRO_LOGI(LOG_TAG, "  LCD ST7789P3 320x240   model: mfg partition");
#endif
    IPRO_LOGI(LOG_TAG, "==========================================");

    xTaskCreateStatic(app_task, "app", sizeof(s_app_stack) / sizeof(StackType_t),
                      NULL, configMAX_PRIORITIES - 2, s_app_stack, &s_app_tcb);
    vTaskStartScheduler();
    while (1) {
    }
    return 0;
}
