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

#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "shell.h"
#include "platform_common.h"
#include "hal_boot2.h"
#include "hal_uart.h"
#include "freertos_app_hooks.h"
#include "lwip/tcpip.h"

extern int aic_wifi_demo_test(void);
extern void cmd_test(const char *command);
#ifdef AIC_RELEASE_TEST
extern void aic_release_smoke_register(void);
#endif

static TaskHandle_t s_aic_task;
static volatile int s_aic_started;

static void aic_start_task(void *arg)
{
    (void)arg;
    puts("aic8800: starting D80 SDIO Wi-Fi/BT driver\r");
    if (aic_wifi_demo_test() == 0) {
        s_aic_started = 1;
        puts("aic8800: driver and lwIP netif initialized\r");
    } else {
        s_aic_started = 0;
        puts("aic8800: ERROR: driver initialization failed\r");
    }
    s_aic_task = NULL;
    vTaskDelete(NULL);
}

static int cmd_aic_start(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (s_aic_started) {
        puts("aic8800: already initialized; ignore duplicate aic_start\r");
        return 0;
    }
    if (s_aic_task) { puts("aic8800: start already in progress\r"); return 0; }
    return xTaskCreate(aic_start_task, "aic_start", 8192, NULL,
                       configMAX_PRIORITIES - 3, &s_aic_task) == pdPASS ? 0 : -1;
}

static int cmd_aic(int argc, char **argv)
{
    int i, used = 0;
    char line[256];
    if (argc < 2) { puts("usage: aic <AIC Wi-Fi command>\r"); return -1; }
    for (i = 1; i < argc && used < (int)sizeof(line) - 2; ++i)
        used += snprintf(line + used, sizeof(line) - (size_t)used, "%s%s", i == 1 ? "" : " ", argv[i]);
    cmd_test(line);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_aic_start, aic_start, "Initialize AIC8800 D80 over SDIO");
SHELL_CMD_EXPORT_ALIAS(cmd_aic, aic, "Forward a command to the AIC Wi-Fi CLI");

static void main_task(void *arg)
{
    (void)arg;
#ifdef AIC_RELEASE_TEST
    aic_release_smoke_register();
#endif
    puts("\r\nIPRO7 AIC8800 D80 bring-up\r");
    /* IPRO7 platform_init() does not start lwIP.  tcpip_init() creates the
     * core-lock semaphore and TCP/IP thread required by sockets and DHCP. */
    tcpip_init(NULL, NULL);
    puts("lwIP TCP/IP core initialized\r");
    puts("Run: aic_start, then use: aic <command>\r");
    shell_init_with_task(UART0_INDEX);
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_setup_heap();
    platform_init(0);
    hal_boot2_init();
    xTaskCreate(main_task, "main", 4096, NULL, configMAX_PRIORITIES - 1, NULL);
    vTaskStartScheduler();
    for (;;) {}
}
