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
 * @brief IPRO7AI EVK board demo - display, camera, speaker, microphone.
 *
 * Panel: HEM LCM-T2D4BP-084, 2.4" 240x320 IPS, 4-line SPI at 3.3 V.
 * Camera: SmartSens SC101IOT, 1280x720 DVP at 1.8 V, previewed on the panel.
 *
 * Console and shell run over the platform UART (115200 8N1). On boot the app
 * initialises the panel and draws colour bars, so a correctly wired module
 * shows first light with nothing typed. Everything after that is driven from
 * two shell commands:
 *
 *     lcd status            what the panel driver thinks is configured
 *     lcd id                read the chip back over SDA - is it even alive?
 *     lcd rgbtest           R/G/B/white/black bands (RGB vs BGR check)
 *     lcd init tuned        re-init with the conventional power/gamma block
 *     lcd clk 8000000       slow the bus down if the picture is corrupt
 *
 *     cam probe             SCCB + MCLK + reset, then look for the sensor
 *     cam start             live preview on the panel
 *     cam status            geometry and frame/error counters
 *
 *     spk tone 1000 2       1 kHz sine out of the class-D amplifier
 *     mic rec 3 / mic play  record from the PDM mic and play it back
 *     mic scope             live microphone waveform on the panel
 *
 * Board support is bsp/board/ipro7/ipro7ai_evk_board_config.h
 * (CONFIG_IPRO7AI_EVK); this file only wires the drivers together.
 *
 * Panel wiring (full FPC pinout in src/st7789p3_port.h):
 *     SCL=GPIO15  SDA=GPIO16  CS=GPIO10  D/C=GPIO17  RESET=GPIO14  BL_EN=GPIO12
 *     VDD=3.3V    GND=GND     LEDA/LEDK = board backlight rail (not a GPIO)
 *
 * Camera wiring (full FPC pinout and the board's three quirks in
 * src/cam_port.h):
 *     SCL=GPIO36 SDA=GPIO37  D0-D7=GPIO0-7  PCLK=GPIO38  VSYNC=GPIO39
 *     HSYNC=GPIO40  MCLK=GPIO41  RESET=GPIO35 (level-shifted, active low)
 *
 * Audio wiring (see src/audio_port.h):
 *     DAC=GPIO23 -> TPA2037D1 -> speaker;  amplifier EN=GPIO22, ACTIVE HIGH
 *     PDM mic IM69D129: CLK=GPIO18 DAT=GPIO19, left slot, on a 1.8 V rail
 */

#include <stdint.h>
#include <stddef.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "ipro_log.h"

#include "st7789p3.h"
#include "cam.h"
#include "vidplay.h"
#include "vbus.h"
#include "bt.h"
#include "st7789p3_patterns.h"

#define LOG_TAG "ST7789P3_MAIN"

static void app_task(void *param)
{
    (void)param;

#ifdef CONFIG_EVK_WAIT_VBUS
    /*
     * Hold the demo until the USB-C connector is powered.
     *
     * Only meaningful with a cell fitted: the SoC sits downstream of VBAT
     * (CN1 -> OVP -> charger -> VBAT -> buck-boost -> 3V3_SOC), so without a
     * battery VBUS is already true by the time any code runs and this returns
     * at once. With a battery it separates "running on the cell" from
     * "running on the wall", which is what the panel backlight, the class-D
     * amplifier and the camera's three rails all care about.
     *
     * The wait is deliberately here and not in main(): the console and the
     * BLE clients are tasks, so blocking before vTaskStartScheduler() would
     * take away the very things needed to find out why it is waiting. From
     * here the board stays fully reachable and `vbus skip` releases it.
     *
     * No timeout - a demo board is supposed to wait for its cable, and a
     * board that gave up and started on battery halfway through a show is
     * worse than one still waiting. `vbus skip` is the way out.
     */
    (void)vbus_wait(0u);
#endif

    IPRO_LOGI(LOG_TAG, "bringing up panel with the minimal sequence...");

    if (st7789p3_init(ST7789P3_SEQ_MINIMAL, 1) == ST7789P3_OK) {
        st7789p3_pattern_bars();
        IPRO_LOGI(LOG_TAG, "colour bars drawn. Nothing on screen? Check the "
                           "backlight rail first, then run `lcd id`.");
    } else {
        IPRO_LOGE(LOG_TAG, "init failed");
    }

#ifdef CONFIG_EVK_VIDEO_AUTOPLAY
    /*
     * Watch the card slot. Insertion is what triggers a mount and playback, so
     * a card seated at power-on and one pushed in later behave the same.
     * `vid auto off` stops it.
     *
     * Left out of the plain build: the watcher claims most of the PSRAM heap
     * for the clip buffer on its first play, and the camera's ISP frames come
     * from the same heap. A board being used for anything other than video
     * wants that memory free. `vid auto on` still starts it by hand there.
     */
    vidplay_auto_enable(true);
#endif

#ifdef CONFIG_SHELL
    IPRO_LOGI(LOG_TAG, "shell ready - `lcd` panel, `cam` camera, "
                       "`spk` speaker, `mic` microphone.");
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

    /*
     * Park the camera before anything else. The sensor's RESETB is released by
     * default - R56 pulls it to DOVDD1V8 as soon as GPIO35 goes high-Z at
     * reset - so a sensor left configured by a previous session keeps
     * streaming right through the next boot, loading the same 1V8 LDO that
     * feeds the SoC's VDDIO_1 bank (GPIO0-9, 36-41, which is where the BOOT
     * strap lives).
     *
     * This cannot rescue the boot ROM, which runs long before main(), but it
     * stops the sensor free-running for the rest of the session and means a
     * commanded reboot starts from an unconfigured, low-power sensor rather
     * than a streaming one.
     */
    cam_stop();

    IPRO_LOGI(LOG_TAG, "==========================================");
    IPRO_LOGI(LOG_TAG, "  IPRO7AI  ST7789P3 LCD + SC101IOT camera");
    IPRO_LOGI(LOG_TAG, "  LCD: LCM-T2D4BP-084  2.4\" IPS 240x320 RGB565");
    IPRO_LOGI(LOG_TAG, "  SPI0: SCL=15 SDA=16  CS=10 DC=17 RST=14 BL=12");
    IPRO_LOGI(LOG_TAG, "  CAM: SC101IOT 1280x720 DVP  D0-7=G0-7 MCLK=G41");
    IPRO_LOGI(LOG_TAG, "  AUD: DAC=G23 SPK_EN=G22(hi)  PDM=G18/G19");
    IPRO_LOGI(LOG_TAG, "==========================================");

#if defined(CONFIG_BLUETOOTH_LE_ENABLE) && !defined(CONFIG_EVK_NO_BT_AUTOSTART)
    /* Advertise from power-on. The remote clients have nothing to connect to
     * otherwise, and reaching for the serial console to run `bt init` first is
     * exactly what they exist to avoid. */
    bt_app_autostart();
#endif

    xTaskCreateStatic(app_task, "lcd_app",
                      sizeof(s_app_stack) / sizeof(StackType_t), NULL,
                      configMAX_PRIORITIES - 2,
                      s_app_stack, &s_app_tcb);

    vTaskStartScheduler();
    while (1) {
    }
    return 0;
}
