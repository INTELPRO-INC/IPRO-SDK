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
 * @brief GH7007-01 SPI LCD bring-up test — FreeRTOS entry point.
 *
 * Console + shell run over USB CDC (the IPRO7 Arduino board talks to the host
 * over USB; no UART adapter needed). Drive the panel from the shell:
 *
 *     lcd init           # reset + vendor init sequence + clear black
 *     lcd fill red       # solid colour fill
 *     lcd bars           # colour bars
 *     lcd checker 20     # checkerboard
 *     lcd grad           # gradient bands
 *
 * Wiring (IPRO7 Arduino  ->  GH7007 4-wire SPI module):
 *     SCL=GPIO19  SDA=GPIO16(MOSI)
 *     CS=GPIO18   RS/DC=GPIO15   RESET=GPIO14
 *     VDD=3.3V    GND=GND        LEDA/LEDK = backlight rail
 *
 * Module straps (verify on FPC): CMD_SEL=HIGH (SPI), SPI_CSB reaches GPIO18.
 */

#include <stdint.h>
#include <stddef.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "usb_console.h"
#include "ipro_log.h"

#define LOG_TAG "GH7007_MAIN"

/* Per-device USB serial — a fixed value is enough to enumerate one test board. */
void device_serial_get(uint8_t out[8])
{
    static const uint8_t sn[8] = { 0x67, 0x70, 0x07, 0x00, 0x00, 0x00, 0x00, 0x01 };
    for (int i = 0; i < 8; i++) {
        out[i] = sn[i];
    }
}

static void app_task(void *param)
{
    (void)param;

    IPRO_LOGI(LOG_TAG, "Shell ready. Type `lcd` for commands, `lcd init` to start.");

    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

int main(void)
{
    static StackType_t  s_app_stack[2048];
    static StaticTask_t s_app_tcb;

    ipro_platform_init();   /* heap + boot2 + log + shell */

    /* USB CDC console — bring up before the banner so log lines buffer into the
     * console ring and flush once the host opens the port. The 1200-baud touch
     * into the boot2 USB downloader is handled by uboot_trigger. */
    usb_console_init();

    IPRO_LOGI(LOG_TAG, "==========================================");
    IPRO_LOGI(LOG_TAG, "  IPRO7 GH7007-01 SPI LCD bring-up");
    IPRO_LOGI(LOG_TAG, "  SPI0: SCL=19 SDA(MOSI)=16  CS=18");
    IPRO_LOGI(LOG_TAG, "  DC/RS=15 RST=14   console=USB CDC");
    IPRO_LOGI(LOG_TAG, "  320x240 GRAM -> scaler -> 1024x600 panel");
    IPRO_LOGI(LOG_TAG, "==========================================");

    xTaskCreateStatic(app_task, "lcd_app",
                      sizeof(s_app_stack) / sizeof(StackType_t), NULL,
                      configMAX_PRIORITIES - 2,
                      s_app_stack, &s_app_tcb);

    vTaskStartScheduler();
    while (1) {
    }
    return 0;
}
