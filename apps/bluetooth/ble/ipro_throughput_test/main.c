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
 * @brief IPRO BLE Throughput Test Application Main Entry Point
 * 
 * This application demonstrates BLE throughput testing between two IPRO7 boards.
 * - One board runs as Slave (Peripheral) - receives data from Master
 * - Other board runs as Master (Central) - sends data to Slave
 * 
 * Usage:
 *   1. Flash both boards with this firmware
 *   2. On Slave board: Run "tp slave" command
 *   3. On Master board: Run "tp master" command (or "tp master XX:XX:XX:XX:XX:XX")
 *   4. After connection, run "tp start [size] [duration]" to begin test
 *   5. Run "tp stats" to view current statistics
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

// IPRO SDK includes
#ifdef CONFIG_SHELL
#include "shell.h"
#include "hal_uart.h"
#endif

/* Filesystem */
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
#include "evfs.h"
#include "evfs_posix.h"
#endif

// BLE controller initialization
#include "ble_lib_api.h"
#include <ipro_ble_ctlr_arch.h>   /* ipro_ble_ctlr_controller_init() */
#include "hal_wdt.h"
#include "throughput_app.h"
#include "ipro_log.h"
#include "freertos_app_hooks.h"

#define LOG_TAG "TP_MAIN"

/**
 * @brief AOS loop processing task
 */
static void aos_loop_proc(void *arg)
{
    (void)arg;

    // Create throughput test task
    static StackType_t app_task_stack[2048];
    static StaticTask_t app_task_tcb;

    TaskHandle_t app_task_handle = xTaskCreateStatic(
        throughput_app_task,
        "TPTest",
        2048,
        NULL,
        3,
        app_task_stack,
        &app_task_tcb
    );

    if (app_task_handle == NULL) {
        printf("ERROR: Failed to create throughput test task\n");
    }

    // Print welcome message
    printf("\n");
    printf("========================================\n");
    printf("  IPRO BLE Throughput Test Application\n");
    printf("========================================\n");
    printf("Use 'tp help' for command list\n");
    printf("\n");

    /* Keep task alive - shell runs in its own task */
    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

// BLE 5.4 stack callback - required by the BLE stack
void app_init(void)
{
    printf("BLE 5.4 stack app_init() called\r\n");
}

static void proc_wdt_feed(void *arg)
{
    while(1) {
        wdt_feed();
        vTaskDelay(1000);
    }
    vTaskDelete(NULL);
}
void init_wdt(void)
{
    wdt_init(0, 10000);
    xTaskCreate(proc_wdt_feed, "wdt_feeder", 256, NULL, 10, NULL);

}
/**
 * @brief Application entry point
 */
int main(void)
{
    static StackType_t aos_loop_proc_stack[1024];
    static StaticTask_t aos_loop_proc_task;

    // Platform initialization
    ipro_platform_init();

    printf("\n\n");
    printf("=== IPRO7 BLE Throughput Test ===\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);

    // Initialize BLE controller
    printf("ipro_ble_ctlr_controller_init...\r\n");
    /* Before the scheduler, so no delay here: this only creates the
     * controller task. The HCI driver init that has to follow it runs from
     * aos_loop_proc(), by which time the controller task has been running. */
    if (ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 3) != 0) {
        printf("BLE controller init failed\r\n");
    }
    init_wdt();
    // Create AOS task
    xTaskCreateStatic(
        aos_loop_proc,
        "aos",
        sizeof(aos_loop_proc_stack)/4,
        NULL,
        10,
        aos_loop_proc_stack,
        &aos_loop_proc_task
    );

    // Start scheduler
    printf("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();

    // Should never reach here
    printf("ERROR: FreeRTOS scheduler exited unexpectedly\n");
    return -1;
}
