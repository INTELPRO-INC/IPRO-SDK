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

/*
 * ipro_pio_sdh_test — dedicated PIO SDH (GPIO22-27) + FatFS validation app.
 *
 * Shell commands:
 *   pio_sdcard ...            PIO 4-bit SD host (src/pio_sdcard_cmd.c)
 *   sdcard / sdh_speed_test / format_sd
 *                             hardware SDH on GPIO28-33 — the on-board A/B
 *                             reference host (src/hw_sdcard_cmd.c)
 *
 * See README.md for the wiring, the canonical test sequence, and the
 * failure-triage ladder (raw vs FS, wdiag patterns, busy interpretation).
 */
#include <generated/autoconf.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
#include "evfs.h"
#include "evfs_posix.h"
#endif
#include "freertos_app_hooks.h"

#ifdef CONFIG_COMPONENTS_CHERRYUSB_ENABLE
#include "usb_console.h"

/* Per-device USB serial — a fixed value is enough to enumerate one test board
 * (avoids the easyflash/TRNG-backed apps/turnkey/common/device_serial.c). */
void device_serial_get(uint8_t out[8])
{
    static const uint8_t sn[8] = { 0x50, 0x10, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x01 };
    for (int i = 0; i < 8; i++) {
        out[i] = sn[i];
    }
}
#endif

static void app_init_task(void *arg)
{
    (void)arg;
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
    evfs_init();
#endif
    printf("\r\n==== PIO SD Host - demo / evaluation / benchmark ====\r\n");
    printf("Quick start (PIO card on GPIO22-27 -> /piosd):\r\n");
    printf("  pio_sdcard info         identify the card\r\n");
    printf("  pio_sdcard format       fresh exFAT + mount   (ERASES CARD)\r\n");
    printf("  pio_sdcard benchmark    write/read/small-file suite + report\r\n");
    printf("  pio_sdcard status       live progress of a running test\r\n");
    printf("  pio_sdcard help         full command list\r\n");
    printf("Dual host: `sdcard` (HW SDH -> /sdcard) can run at the same time;\r\n");
    printf("           `pio_sdcard dual` then tests both cards together.\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_platform_init();

#ifdef CONFIG_COMPONENTS_CHERRYUSB_ENABLE
    /* USB CDC console — bring up before the banner so printf/log (redirected
     * via --wrap) buffer into the console ring and flush once the host opens
     * the port. The 1200-baud touch into the boot2 USB downloader is handled
     * by components/boot/uboot_trigger. */
    usb_console_init();
#endif

    xTaskCreate(app_init_task, "app_init", 1024, NULL, 5, NULL);
    vTaskStartScheduler();
    while (1) {}
}
