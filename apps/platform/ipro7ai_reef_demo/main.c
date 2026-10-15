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
 * @brief IPRO7AI Reef carrier board demo.
 *
 * Every block on the carrier is reachable from the shell (115200 8N1 on the
 * U6 debug wafer, GPIO20/21):
 *
 *     reef status / reef selftest     board summary; LED + tone + slot + camera
 *     led <r> <g> <b> | led rainbow  WS2812 on GPIO24
 *     btn status | btn watch [s]     SW1(BOOT)/SW3/SW4/SW5
 *     spk tone <hz> <ms> [amp%] [hp] DAC -> PAM8302A speaker / TLV9061 headphone
 *     mic rec <pdm|headset|aux> <s>  then `mic play`; `mic loop <src>` monitors
 *     sd mount | sd ls | sd test     card in the SDH slot
 *     fin | fin cycle                IPRO6S Wi-Fi fin supply in the same slot
 *     cam probe | cam start | cam snap  FLEX-24 module (OV5640 / OV2640) -> ISP
 *     hp                             headphone jack detect level
 *     ipronet_up <ssid> <psk>        fin Wi-Fi (CONFIG_WIFI_USE_IPRO6S)
 *
 * Board support is bsp/board/ipro7/ipro7ai_reef_board_config.h
 * (CONFIG_IPRO7AI_REEF); this file only wires the modules together.
 *
 * At power-on the LED breathes blue, then goes dark; the keys drive the LED
 * (SW3 red, SW4 green, SW5 blue, BOOT white) so the board shows signs of life
 * with nothing typed.
 */

#include <stdint.h>
#include <stddef.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "ipro_log.h"
#include "board_config.h"

#include "rgb_led.h"
#include "keys.h"
#include "audio.h"
#include "cam.h"

#define LOG_TAG "REEF"

/* Key -> colour, so a bare board answers a finger. */
static void key_led_cb(key_id_t key, bool pressed, void *ctx)
{
    (void)ctx;
    if (!pressed) {
        rgb_led_set(0, 0, 0);
        return;
    }
    switch (key) {
    case KEY_SW3:  rgb_led_set(40, 0, 0);   break;
    case KEY_SW4:  rgb_led_set(0, 40, 0);   break;
    case KEY_SW5:  rgb_led_set(0, 0, 40);   break;
    case KEY_BOOT: rgb_led_set(30, 30, 30); break;
    default: break;
    }
}

static void led_breathe_blue(void)
{
    for (int i = 0; i <= 40; i++) {
        rgb_led_set(0, 0, (uint8_t)i);
        vTaskDelay(pdMS_TO_TICKS(15));
    }
    for (int i = 40; i >= 0; i--) {
        rgb_led_set(0, 0, (uint8_t)i);
        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

static void app_task(void *param)
{
    (void)param;

    /* Sensor to its lowest-power state first: RESETB is already held by R34,
     * this also asserts PWDN and makes sure no XCLK runs. */
    cam_park();

    if (rgb_led_init() == 0) {
        led_breathe_blue();
    } else {
        IPRO_LOGE(LOG_TAG, "RGB LED init failed");
    }

    if (keys_init() == 0) {
        keys_set_callback(key_led_cb, NULL);
    }
    if (audio_init() != 0) {
        IPRO_LOGE(LOG_TAG, "audio init failed");
    }

#ifdef CONFIG_SHELL
    IPRO_LOGI(LOG_TAG, "shell ready - `reef`, `led`, `btn`, `spk`, `mic`, `sd`, "
                       "`fin`, `cam`, `hp`");
#ifdef CONFIG_WIFI_USE_IPRO6S
    IPRO_LOGI(LOG_TAG, "IPRO-WiFi - `ipronet_up <ssid> <psk>`");
#ifdef CONFIG_RTSP_SERVER
    IPRO_LOGI(LOG_TAG, "RTSP - `rtsp_cam on [fps]`");
#endif
#endif
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

    IPRO_LOGI(LOG_TAG, "==========================================");
    IPRO_LOGI(LOG_TAG, "  IPRO7AI Reef carrier demo");
    IPRO_LOGI(LOG_TAG, "  UART0 TX=%d RX=%d  RGB=G%d  SPK_EN=G%d(hi)",
              BOARD_UART_TX_PIN, BOARD_UART_RX_PIN, BOARD_RGB_LED_PIN, BOARD_SPK_EN_PIN);
    IPRO_LOGI(LOG_TAG, "  CAM: FLEX-24 D0-7=G0-7 XCLK=G%d RST=G%d PWDN=G%d",
              BOARD_CAM_MCLK_PIN, BOARD_CAM_RST_PIN, BOARD_CAM_PWDN_PIN);
    IPRO_LOGI(LOG_TAG, "  MIC: PDM G%d/G%d  HS AIN%d/%d  AUX AIN%d/%d  SD G28-33",
              BOARD_MIC_CLK_PIN, BOARD_MIC_DAT_PIN, BOARD_HSMIC_POS_CH,
              BOARD_HSMIC_NEG_CH, BOARD_AUX_L_CH, BOARD_AUX_R_CH);
    IPRO_LOGI(LOG_TAG, "==========================================");

    xTaskCreateStatic(app_task, "evk_app",
                      sizeof(s_app_stack) / sizeof(StackType_t), NULL,
                      configMAX_PRIORITIES - 2,
                      s_app_stack, &s_app_tcb);

    vTaskStartScheduler();
    while (1) {
    }
    return 0;
}
