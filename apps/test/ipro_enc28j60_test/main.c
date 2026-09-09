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
 * @brief ENC28J60 SPI Ethernet bring-up test — FreeRTOS entry point.
 *
 * Console + shell run over USB CDC (the IPRO7 Arduino board talks to the host
 * over USB; no UART adapter needed). Drive the chip from the shell:
 *
 *     enc init        # reset + configure, prints EREVID / PHY id / link
 *     enc id          # SPI sanity check
 *     enc tx          # send a broadcast test frame
 *     enc rx          # dump received frames
 *
 * Wiring (IPRO7 Arduino):
 *     SCK=GPIO19  MOSI=GPIO16(->SI)  MISO=GPIO17(<-SO)
 *     CS=GPIO18   RST=GPIO14         INT=GPIO15
 *     VCC=3.3~5V  GND=GND
 */

#include <stdint.h>
#include <stddef.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "usb_console.h"
#include "device_serial.h"
#include "ipro_log.h"

#define LOG_TAG "ENC_MAIN"

/* Per-device USB serial. The shared apps/turnkey/common/device_serial.c reads
 * a unique chip ID but pulls in EasyFlash; for a single test board a fixed
 * 8-byte value is enough to enumerate. */
void device_serial_get(uint8_t out[8])
{
    static const uint8_t sn[8] = { 0xE2, 0x8C, 0x60, 0x00, 0x00, 0x00, 0x00, 0x01 };
    for (int i = 0; i < 8; i++) {
        out[i] = sn[i];
    }
}

static void app_task(void *param)
{
    (void)param;

    IPRO_LOGI(LOG_TAG, "Shell ready. Type `enc` for commands, `enc init` to start.");

    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

int main(void)
{
    static StackType_t  s_app_stack[2048];
    static StaticTask_t s_app_tcb;

    ipro_platform_init();   /* heap + boot2 + log + shell */

    /* USB CDC console — bring up before the banner so printf/log buffer into
     * the console ring and flush once the host opens the port. The 1200-baud
     * touch into the boot2 USB downloader is handled by uboot_trigger. */
    usb_console_init();

    IPRO_LOGI(LOG_TAG, "==========================================");
    IPRO_LOGI(LOG_TAG, "  IPRO7 ENC28J60 SPI Ethernet bring-up");
    IPRO_LOGI(LOG_TAG, "  SPI0: SCK=19 MOSI=16 MISO=17 CS=18");
    IPRO_LOGI(LOG_TAG, "  RST=14 INT=15   console=USB CDC");
    IPRO_LOGI(LOG_TAG, "==========================================");

    xTaskCreateStatic(app_task, "enc_app",
                      sizeof(s_app_stack) / sizeof(StackType_t), NULL,
                      configMAX_PRIORITIES - 2,
                      s_app_stack, &s_app_tcb);

    vTaskStartScheduler();
    while (1) {
    }
    return 0;
}
