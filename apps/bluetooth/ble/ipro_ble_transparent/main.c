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
 * @brief BLE Transparent Module - Main Entry Point
 *
 * UART-BLE bridge with AT command support.
 * External MCU communicates via UART AT commands in command mode,
 * and raw data is bridged in transparent (data) mode.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <generated/autoconf.h>
#ifdef CONFIG_SHELL
#include "shell.h"
#include "hal_uart.h"
#endif
#include <FreeRTOS.h>
#include <task.h>

#ifdef CONFIG_EASYFLASH4
#include "easyflash.h"
#endif

#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
#include "evfs.h"
#include "evfs_posix.h"
#endif

#include "hal_uart.h"
#include "ble_lib_api.h"
#include "ipro_log.h"
#include "freertos_app_hooks.h"

#include "app_config.h"
#include "ble_transparent.h"
#include "at_command.h"
#include "uart_bridge.h"

extern void trans_cli_init(void);

#define LOG_TAG "MAIN"

/* BLE stack callback (required by controller) */
void app_init(void)
{
    IPRO_LOGI(LOG_TAG, "BLE 5.4 stack initialized");
}

/* Application task */
static void app_task(void *pvParameters)
{
    (void)pvParameters;

    IPRO_LOGI(LOG_TAG, "");
    IPRO_LOGI(LOG_TAG, "========================================");
    IPRO_LOGI(LOG_TAG, "  IPRO BLE Transparent Module v%s", APP_VERSION);
    IPRO_LOGI(LOG_TAG, "========================================");

    /* Initialize BLE */
    int ret = ble_transparent_init();
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "BLE init failed: %d", ret);
        vTaskDelete(NULL);
        return;
    }

    /* Initialize UART bridge */
    uart_bridge_init();

    /* Initialize AT command parser */
    at_command_init(NULL); /* Response goes through uart_bridge internally */

    /* Load persistent config */
    at_command_load_config();

    /* Register CLI commands */
    trans_cli_init();

    IPRO_LOGI(LOG_TAG, "Ready. AT commands on UART%d @ %lu baud",
              BRIDGE_UART_ID, (unsigned long)DEFAULT_UART_BAUDRATE);

    /* Main loop - nothing to do, UART task handles everything */
    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

static void aos_loop_proc(void *pvParameters)
{
    (void)pvParameters;

#ifdef CONFIG_EASYFLASH4
    easyflash_init();
#endif

    /* Create app task */
    static StackType_t app_stack[2048];
    static StaticTask_t app_tcb;
    xTaskCreateStatic(app_task, "BLETrans", 2048, NULL,
                      configMAX_PRIORITIES - 2, app_stack, &app_tcb);

    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

int main(void)
{
    static StackType_t aos_stack[1024];
    static StaticTask_t aos_tcb;

    ipro_platform_init();

    xTaskCreateStatic(aos_loop_proc, "aos_loop",
                      sizeof(aos_stack) / 4, NULL, 10,
                      aos_stack, &aos_tcb);

    vTaskStartScheduler();
    return -1;
}
