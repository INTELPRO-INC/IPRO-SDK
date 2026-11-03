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
 * @file reef_apps.c
 * @brief Start one Teach, as its own firmware's main.c does.
 */
#include <stdint.h>
#include <stdio.h>
#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "ipro_log.h"
#include "reef_mode.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#include "rgb_led.h"
#include "voice.h"
#include "voice_link.h"
#include "sensor_app.h"
#include "sensor_link.h"
#include "sensor_model.h"
#include "uvc_isp.h"

#define LOG_TAG "RTCH"

extern void uvc_init(void);                     /* apps/ai/ipro_reef_teach/image/uvc.c */

static void start_image(void)
{
    /* as ipro7_uvc_cam's main.c: USB (UVC + serial) first, then camera -> ISP -> MJPEG */
    uvc_init();
    uvc_isp_set_param(UVC_RES_MODE, true);
    uvc_isp_init();
    IPRO_LOGI(LOG_TAG, "image Teach ready - shell `teach`, IPRO AI Studio Teach page over USB-C");
}

static void start_voice(void)
{
    if (rgb_led_init() == 0) {
        rgb_led_set(0, 0, 20);
    }
    const int model = voice_init();
    if (voice_link_start() != 0) {
        IPRO_LOGE(LOG_TAG, "USB link failed to start");
    }
    rgb_led_off();
    IPRO_LOGI(LOG_TAG, "voice %s - shell `spk`, IPRO AI Studio Audio page over USB-C",
              model == 0 ? "ready" : "NO MODEL: flash the voice model to its mfg slot");
}

static void start_sensor(void)
{
    rgb_led_init();
    if (sensor_app_init() != 0) {
        IPRO_LOGE(LOG_TAG, "sensor task failed to start");
    }
    if (sensor_link_start() != 0) {
        IPRO_LOGE(LOG_TAG, "USB link failed to start");
    }
    IPRO_LOGI(LOG_TAG, "sensor Teach ready - shell `sens`, IPRO AI Studio Sensor page over USB-C; "
              "model: %s", sensor_model_cfg() ? "deployed" : sensor_model_why());
}

static void app_task(void *param)
{
    switch ((reef_mode_t)(uintptr_t)param) {
    case REEF_MODE_VOICE:
        start_voice();
        break;
    case REEF_MODE_SENSOR:
        start_sensor();
        break;
    default:
        start_image();
        break;
    }
    vTaskDelete(NULL);
}

int reef_apps_start(reef_mode_t mode)
{
    /* each at its own firmware's start priority: the camera start-up retries
     * while no camera answers, and at the voice/sensor priority it starved the
     * image Teach link (configMAX_PRIORITIES - 5), so the board went deaf */
    const UBaseType_t prio = mode == REEF_MODE_IMAGE ? 10 : configMAX_PRIORITIES - 2;

    return xTaskCreate(app_task, "app", 2048, (void *)(uintptr_t)mode, prio, NULL) == pdPASS
               ? 0 : -1;
}

#ifdef CONFIG_SHELL
static int cmd_mode(int argc, char **argv)
{
    if (argc < 2) {
        printf("mode: %s (of %s); `mode <image|voice|sensor>` restarts into another\r\n",
               reef_teach_mode(), reef_teach_modes());
        return 0;
    }
    const int rc = reef_teach_switch(argv[1]);
    if (rc != 0) {
        printf("mode: %s\r\n", rc == -2 ? "image, voice or sensor" : "could not store it");
    }
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mode, mode, Teach mode: mode [image|voice|sensor]);
#endif
