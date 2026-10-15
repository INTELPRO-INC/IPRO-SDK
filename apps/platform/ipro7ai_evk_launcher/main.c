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
 * @brief IPRO7AI EVK launcher: camera + LCD + microphone + speaker, one image.
 *
 *   SC101IOT 1280x720 DVP
 *      -> ISP ch0  YUYV 320x180 @10 fps  -> preview -> ST7789P3
 *                  centre 128x128 of it   -> NPU gesture classifier (CPU-fed)
 *                                            -> PAPER / ROCK / SCISSORS
 *      -> ISP ch1  1280x720 -> 640x360 ARGB -> NPU YOLOv8 person/pet (DETECT page)
 *   IM69D129 PDM mic -> AUADC -> PSRAM clip -> DAC -> TPA2037D1 (voice page)
 *
 * Pages: CAMERA (preview, freeze, mirror), DETECT (person / pet boxes),
 * VOICE (waveform, record, play back), ABOUT (every block's live status).
 * PAPER moves to the next page, ROCK is the page's action, SCISSORS its
 * alternate. Everything starts at power-on; the shell (921600 8N1) has `app`
 * plus the board library's `lcd`, `cam`, `mic`, `spk`, `sd`.
 *
 * Two models share the NPU (npu_share.c). The gesture model is the RPS
 * app's rps_v3.inm: from sd:/models/gesture.inm if a card is in, else from
 * the "mfg" flash partition (./flash.sh --full). The detector is
 * person_pet_detect.inm from sd:/models/person.inm only (700 KB does not
 * fit next to the gesture model in mfg); without it the DETECT page says so.
 *
 * Board support: components/board/ipro7ai_evk (CONFIG_IPRO7AI_EVK). J6:
 * wire pin 3 (VDDIO1) to pin 7 (2V8_CAM) for the camera - the only VDDIO1
 * supply on V1 boards. BOOT is not on J6; see bsp/board/ipro7/IPRO7AI_EVK.md.
 */

#include <stdint.h>
#include <stddef.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "ipro_log.h"

#include "evk.h"
#include "gesture.h"
#include "detect.h"
#include "npu_share.h"
#include "voice.h"
#include "ui.h"

#define LOG_TAG "MAIN"

/* Periodic health line from the boot task (the highest priority in the
 * app, so it prints even when something below it hogs the CPU): every
 * TRACE_FAST_MS for the first TRACE_FAST_FOR_MS after boot, then every
 * TRACE_SLOW_MS. `app trace on|off` overrides. */
#define TRACE_FAST_MS       5000
#define TRACE_FAST_FOR_MS   60000
#define TRACE_SLOW_MS       60000

static volatile int s_trace = -1;          /* -1 auto, 0 off, 1 on */

void app_trace_set(int on)
{
    s_trace = on;
}

static void trace_line(void)
{
    static char stats[768];
    gesture_stats_t  gs;
    gesture_result_t gr;
    detect_stats_t   ds;
    detect_result_t  dr;
    ui_stats_t       us;
    evk_cam_stats_t  cs;

    gesture_get_stats(&gs);
    gesture_get_result(&gr);
    detect_get_stats(&ds);
    detect_get_result(&dr);
    ui_get_stats(&us);
    evk_cam_get_stats(&cs);

    printf("[TRACE] up %lus heap %luK | ui p%d %lu.%lufps %lums/f drop %lu | cam ch0 %lu ch1 %lu err %lu\r\n"
           "[TRACE] gesture %lu.%lufps run %lums wait %lums in 0x%08lx -> %s %u%% raw %u %u %u %u | "
           "detect %s %lu.%lufps run %lums wait %lums post %lums %u box err %lu\r\n",
           (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS / 1000u),
           (unsigned long)(xPortGetFreeHeapSize() / 1024u),
           us.page + 1, (unsigned long)(us.fps_x10 / 10u), (unsigned long)(us.fps_x10 % 10u),
           (unsigned long)us.blit_ms, (unsigned long)us.dropped,
           (unsigned long)cs.frames0, (unsigned long)cs.frames1,
           (unsigned long)(cs.fifo_err + cs.vsync_err + cs.hsync_err),
           (unsigned long)(gs.fps_x10 / 10u), (unsigned long)(gs.fps_x10 % 10u),
           (unsigned long)gr.npu_ms, (unsigned long)gs.wait_ms, (unsigned long)gs.in_sum,
           gesture_name(gr.cls), gr.conf, gr.raw[0], gr.raw[1], gr.raw[2], gr.raw[3],
           detect_is_ready() ? "on" : "off",
           (unsigned long)(ds.fps_x10 / 10u), (unsigned long)(ds.fps_x10 % 10u),
           (unsigned long)dr.npu_ms, (unsigned long)ds.wait_ms, (unsigned long)dr.post_ms,
           dr.count, (unsigned long)ds.errors);

    /* Run-time stats: name, abs time, percent - the CPU budget, task by task. */
    vTaskGetRunTimeStats(stats);
    printf("%s", stats);
}

/* ISP channel 1 -> the person detector, one frame in flight. */
static int ch1_frame(uint32_t addr, void *arg)
{
    (void)arg;
    return detect_push_frame(addr) == DETECT_OK ? 0 : -1;
}

static void detect_done(void *arg)
{
    (void)arg;
    evk_cam_npu_rearm();
}

static void app_task(void *param)
{
    int rc;

    (void)param;

    evk_begin();                            /* camera parked, amp off, panel up */
    if (ui_init() != 0) {
        IPRO_LOGE(LOG_TAG, "no panel - continuing headless");
    }

    /* Models before the camera: the detector's ~3 MB activation buffer wants
     * the PSRAM heap while it is still empty. Then the voice clip (320 KB). */
    npu_share_init();
    rc = gesture_init();
    if (rc != 0) {
        IPRO_LOGW(LOG_TAG, "gesture classifier unavailable (%s) - navigate with `app next`",
                  gesture_fail_reason());
    }
    rc = detect_init();
    if (rc == DETECT_OK) {
        /* Channel 1: the whole 1280x720 frame scaled 2x down to 640x360. */
        evk_cam_npu_input_t in = {
            .addr  = detect_input_addr(),
            .out_w = DETECT_IN_W, .out_h = DETECT_IN_H,
            .roi_x = 0, .roi_y = 0, .roi_w = EVK_CAM_SENSOR_W, .roi_h = EVK_CAM_SENSOR_H,
        };
        evk_cam_set_npu_input(&in);
        evk_cam_set_npu_frame_cb(ch1_frame, NULL);
        detect_set_done_cb(detect_done, NULL);
    } else {
        IPRO_LOGW(LOG_TAG, "person detector unavailable (%s) - DETECT page will say so",
                  detect_fail_reason());
    }

    if (voice_init() != 0) {
        IPRO_LOGE(LOG_TAG, "voice engine failed - the voice page will report it");
    }

    rc = evk_cam_start();
    if (rc != EVK_CAM_OK) {
        IPRO_LOGE(LOG_TAG, "camera start failed (%d) - check J6 pin 3-7 (VDDIO1)", rc);
    }
    /* With or without a camera: the UI runs (voice and about pages work
     * without one, and `app next` stands in for the gestures). */
    if (ui_start() != 0) {
        IPRO_LOGE(LOG_TAG, "ui start failed");
    }

#ifdef CONFIG_SHELL
    IPRO_LOGI(LOG_TAG, "shell ready - `app status`, `app next`, `app rec`, `spk tone`, `mic level`");
#endif
    for (;;) {
        uint32_t up = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
        vTaskDelay(pdMS_TO_TICKS(up < TRACE_FAST_FOR_MS ? TRACE_FAST_MS : TRACE_SLOW_MS));
        if (s_trace == 1 || (s_trace == -1 && up < TRACE_FAST_FOR_MS + TRACE_SLOW_MS)) {
            trace_line();
        }
    }
}

int main(void)
{
    static StackType_t  s_app_stack[2048];
    static StaticTask_t s_app_tcb;

    ipro_platform_init();   /* heap + boot2 + log + shell */

    /* Park the camera before anything else (its RESETB defaults to released;
     * a sensor left streaming loads the 1V8 LDO that feeds the BOOT strap).
     * evk_begin() does it again from the task; this one is for the boot. */
    evk_cam_stop();

    IPRO_LOGI(LOG_TAG, "==========================================");
    IPRO_LOGI(LOG_TAG, "  IPRO7AI EVK launcher");
    IPRO_LOGI(LOG_TAG, "  CAM 720p -> ISP ch0 320x180 preview -> LCD + NPU gestures");
    IPRO_LOGI(LOG_TAG, "             ISP ch1 640x360 -> NPU person/pet (sd:/models/person.inm)");
    IPRO_LOGI(LOG_TAG, "  MIC PDM -> PSRAM -> DAC -> SPK");
    IPRO_LOGI(LOG_TAG, "  PAPER=next page  ROCK=select  SCISSORS=alt");
    IPRO_LOGI(LOG_TAG, "==========================================");

    xTaskCreateStatic(app_task, "app", sizeof(s_app_stack) / sizeof(StackType_t),
                      NULL, configMAX_PRIORITIES - 2, s_app_stack, &s_app_tcb);
    vTaskStartScheduler();
    while (1) {
    }
    return 0;
}
