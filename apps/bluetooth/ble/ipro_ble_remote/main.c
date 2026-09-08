/**
 * @file main.c
 * @brief BLE Remote Control Demo - Main Entry Point
 * 
 * Demonstrates BLE interaction with mobile phones through:
 * - LED Control Service
 * - Button Service
 * - Environment Service
 * - UART Service
 * 
 * Copyright (C) IPRO SDK 2025
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

/* Filesystem */
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
#include "evfs.h"
#include "evfs_posix.h"
#endif

#include "hal_sys.h"
#include "hal_timer.h"
#include "hal_uart.h"
#include "hal_boot2.h"
#include "platform_common.h"
#include "freertos_app_hooks.h"

/* Flash storage for BLE settings */
#ifdef CONFIG_EASYFLASH4
#include "easyflash.h"
#endif

#include "remote_app.h"
#include "ipro_log.h"

#define LOG_TAG "MAIN"

/*
 * BLE STACK CALLBACK
 ****************************************************************************************
 */

void app_init(void)
{
    IPRO_LOGI(LOG_TAG, "[*] BLE 5.4 stack initialized");
}

/*
 * APPLICATION TASK
 ****************************************************************************************
 */

// External function to process pending env notifications
extern void remote_app_process_env_notify(void);

// External function to process pending image transfer chunks
extern void image_transfer_process(void);

static void remote_demo_task(void *pvParameters)
{
    (void)pvParameters;
    
    // Print banner with IPRO_LOG for better formatting and control
    IPRO_LOGI(LOG_TAG, "");
    IPRO_LOGI(LOG_TAG, "╔════════════════════════════════════════════════════════════════╗");
    IPRO_LOGI(LOG_TAG, "║                                                                ║");
    IPRO_LOGI(LOG_TAG, "║             IPRO BLE Remote Control Demo                       ║");
    IPRO_LOGI(LOG_TAG, "║                                                                ║");
    IPRO_LOGI(LOG_TAG, "╚════════════════════════════════════════════════════════════════╝");
    IPRO_LOGI(LOG_TAG, "Features:");
    IPRO_LOGI(LOG_TAG, "   • LED Control         - RGB brightness & effects");
    IPRO_LOGI(LOG_TAG, "   • Button Detection    - Press/Release events");
    IPRO_LOGI(LOG_TAG, "   • Environment Sensor  - Temperature & Humidity");
    IPRO_LOGI(LOG_TAG, "   • UART Bridge         - Serial communication");
    IPRO_LOGI(LOG_TAG, "   • PIR Motion          - Movement detection");
    IPRO_LOGI(LOG_TAG, "   • Image Transfer      - Photo/Video streaming");
    IPRO_LOGI(LOG_TAG, "   • OTA Upgrade         - Firmware update over BLE");
    IPRO_LOGI(LOG_TAG, "Connection:");
    IPRO_LOGI(LOG_TAG, "   Device Name:  'IPRO-Remote'");
    IPRO_LOGI(LOG_TAG, "   Recommended:  nRF Connect, LightBlue");
    IPRO_LOGI(LOG_TAG, "CLI Commands:");
    IPRO_LOGI(LOG_TAG, "   Type 'remote help' for full command list");
    IPRO_LOGI(LOG_TAG, "   Type 'remote status' for device info");
    IPRO_LOGI(LOG_TAG, "════════════════════════════════════════════════════════════════");
    
    // Initialize application
    IPRO_LOGI(LOG_TAG, ">> Initializing remote application...");
    int ret = remote_app_init();
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "[ERROR] Failed to initialize remote app: %d", ret);
        vTaskDelete(NULL);
        return;
    }
    IPRO_LOGI(LOG_TAG, "[OK] Remote application ready");
    // Main loop - process pending notifications
    while (1) {
        // Process any pending environment notifications from timer
        remote_app_process_env_notify();
        
        // Process any pending image transfer chunks
        image_transfer_process();
        
        // Short delay to allow other tasks to run
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/*
 * SYSTEM INITIALIZATION
 ****************************************************************************************
 */

static void aos_loop_proc(void *pvParameters)
{
    (void)pvParameters;

    // Initialize logging
    ipro_log_init(NULL);

    // Initialize flash storage for BLE settings (required by BLE stack)
#ifdef CONFIG_EASYFLASH4
    IPRO_LOGI(LOG_TAG, "[*] Initializing EasyFlash for BLE settings storage");
    easyflash_init();
#endif

    // Initialize filesystem
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
#endif

#ifdef CONFIG_SHELL
    IPRO_LOGI(LOG_TAG, "[*] Initializing Shell (RT-Thread FinSH based)");
    shell_init_with_task(UART0_INDEX);
#endif

    // Create demo application task
    static StackType_t demo_task_stack[2048];
    static StaticTask_t demo_task_tcb;

    IPRO_LOGI(LOG_TAG, "[*] Creating BLE Remote task...");
    TaskHandle_t demo_task_handle = xTaskCreateStatic(
        remote_demo_task,
        "BLERemote",
        2048,
        NULL,
        configMAX_PRIORITIES - 2,  /* High priority for BLE initialization */
        demo_task_stack,
        &demo_task_tcb
    );

    if (demo_task_handle == NULL) {
        IPRO_LOGE(LOG_TAG, "[ERROR] Failed to create BLE remote task");
        vTaskDelete(NULL);
        return;
    }

    IPRO_LOGI(LOG_TAG, "[OK] BLE Remote task created");
    IPRO_LOGI(LOG_TAG, "");

    /* Keep task alive - shell runs in its own task */
    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

/*
 * MAIN ENTRY
 ****************************************************************************************
 */

int main(void)
{
    static StackType_t aos_loop_proc_stack[1024];
    static StaticTask_t aos_loop_proc_task;
    
    // Platform initialization
    platform_init(0);
    ipro_setup_heap();
    
    IPRO_LOGI(LOG_TAG, "");
    IPRO_LOGI(LOG_TAG, "╔════════════════════════════════════════════════════════════════╗");
    IPRO_LOGI(LOG_TAG, "║           IPRO SDK - BLE Remote Control                       ║");
    IPRO_LOGI(LOG_TAG, "╚════════════════════════════════════════════════════════════════╝");
    IPRO_LOGI(LOG_TAG, "");
    
    // Initialize Boot2 partition table (required for OTA)
    IPRO_LOGI(LOG_TAG, "[*] Initializing boot partition table...");
    hal_boot2_init();
    
    // Initialize Low Power framework - loads HBN RAM code
    // This MUST be done before ble_controller_init() because RF init functions
    // are located in HBN RAM and need to be loaded from flash first
    extern int hal_lp_init(void);
    IPRO_LOGI(LOG_TAG, "[*] Initializing low-power framework...");
    int ret = hal_lp_init();
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "[ERROR] Low-power init failed: %d", ret);
        return -1;
    }
    IPRO_LOGI(LOG_TAG, "[OK] Low-power framework ready");
    
    // NOTE: BLE controller will be initialized in remote_app_init()
    // to ensure proper initialization order with easyflash
    
    // Create AOS loop task
    IPRO_LOGI(LOG_TAG, "[*] Creating system event loop...");
    xTaskCreateStatic(aos_loop_proc, (char*)"aos_loop", 
                      sizeof(aos_loop_proc_stack)/4, NULL, 10, 
                      aos_loop_proc_stack, &aos_loop_proc_task);
    
    IPRO_LOGI(LOG_TAG, "[*] Starting FreeRTOS scheduler...");
    IPRO_LOGI(LOG_TAG, "");
    vTaskStartScheduler();
    
    // Should never reach here
    IPRO_LOGE(LOG_TAG, "[ERROR] CRITICAL: Scheduler exited unexpectedly!");
    return -1;
}
