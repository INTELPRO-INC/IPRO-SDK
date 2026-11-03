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
 * @brief IPRO7AI Reef - the three Teach tools in one firmware.
 *
 * Image Teach (USB camera + classifier, image/), voiceprints (voice/) and
 * Sensor Teach (sensor/), one at a time: the mode in flash picks which one
 * starts, each with its own USB identity, so IPRO AI Studio and the browser
 * pages find it. APP <mode> on any of them (TAPP on the image one) stores another
 * mode and restarts into it; so does the shell's `mode <image|voice|sensor>`.
 * Each keeps its model in its own slot of mfg (src/reef_mode.h), so switching
 * loses none of them.
 */
#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "app_watchdog.h"
#include "ipro_log.h"
#include "reef_mode.h"

#define LOG_TAG "RTCH"

int reef_apps_start(reef_mode_t mode);          /* src/reef_apps.c */

int main(void)
{
    ipro_platform_init();   /* heap + boot2 + log + shell */
    const reef_mode_t mode = reef_mode_load();
    IPRO_LOGI(LOG_TAG, "==========================================");
    IPRO_LOGI(LOG_TAG, "  IPRO7AI Reef Teach: image, voice, sensor");
    IPRO_LOGI(LOG_TAG, "  mode: %s (`mode <image|voice|sensor>` to switch)", reef_mode_name(mode));
    IPRO_LOGI(LOG_TAG, "==========================================");
    if (reef_apps_start(mode) != 0) {
        IPRO_LOGE(LOG_TAG, "could not start the %s mode", reef_mode_name(mode));
    }
    app_watchdog_start();   /* CONFIG_APP_WATCHDOG: an NPU bus stall stops the SoC, the AON watchdog restarts it */
    vTaskStartScheduler();
    while (1) {
    }
    return 0;
}
