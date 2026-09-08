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

/* Standard C library headers - must come first for type definitions */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/* Project configuration */
#include <generated/autoconf.h>

/* Shell */
#ifdef CONFIG_SHELL
#include "shell.h"
#include "hal_uart.h"
#endif

/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>

/* Filesystem */
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
#include "evfs.h"
#include "evfs_posix.h"
#endif

/* Event Loop */
#ifdef CONFIG_IPRO_EVENT
#include "ipro_event.h"
#endif

/* Command Scheduler */
#ifdef CONFIG_CMD_SCHEDULER
#include "cmd_scheduler.h"
#endif

#ifdef CONFIG_IPRO_LOG_ENABLE
#include "ipro_log.h"
#endif

/* AON Watchdog */
#include "hal_hbnram_mgnt.h"
#include "hbn_reg.h"
#include "compiler/common.h"
#ifdef CONFIG_AON_WDT_ENABLE
#include "hal_aon_wdt.h"
#include <timers.h>
#endif

/* HAL headers */
#include "core_feature_base.h"
#include "hal_boot2.h"
#include "hal_sys.h"
#include "hal_timer.h"
#include "hal_uart.h"

/* Section attributes for memory placement */
#include "compiler/common.h"

/* USB support */
#ifdef CONFIG_CHERRYUSB
#include "usbd_core.h"
#endif

/* BLE support */
#ifdef CONFIG_BLUETOOTH_LE_ENABLE
#include "demo/bluetooth/ble_demo.h"
#endif

#if defined(CONFIG_ETHERNET) && defined(CONFIG_COMPONENTS_IPERF_ENABLE)
#include "iperf.h"
#endif

#ifdef CONFIG_RTSP_AUTOSTART
#include "librtspsrv.h"
#include "esp_hosted_netif.h"
#ifdef CONFIG_RTSP_TEST_SOURCE
#include "rtsp_test_source.h"
#endif
#endif

/* Platform common */
#include "platform_common.h"

/* Shared FreeRTOS hooks + heap setup */
#include "freertos_app_hooks.h"

/****************************************/
/*    CONSTATNT                         */
/****************************************/
#define _AUTO_RUN_UART_CMD_LIST_EN_  0 /*define 1 to auto run preset command list.*/


#if 0//def CONFIG_IPRO_EVENT
static void hello_event_action(void *arg)
{
    (void)arg;
    printf("[EVENT] Hello Event! - ipro_event delayed action works!\r\n");
}
#endif
#ifdef CONFIG_AON_WDT_ENABLE
static void aon_wdt_kicker_task(void *param)
{
    (void)param;
    TickType_t last = xTaskGetTickCount();
    for (;;) {
        aon_wdt_feed();
        vTaskDelayUntil(&last, pdMS_TO_TICKS(1000));
    }
}

static void aon_wdt_boot_ok_cb(TimerHandle_t xTimer)
{
    (void)xTimer;
    volatile hal_hbnram_sysinfo_t *si = hal_hbnram_sysinfo;
    si->wdt_reset_count = 0;
    si->boot_stage = BOOT_STAGE_FW_OK;
    si->boot_flags |= BOOT_FLAG_FW_BOOTED;
    printf("[AON_WDT] Boot OK, WDT counter cleared\r\n");
}
#endif

#ifdef CONFIG_RTSP_AUTOSTART
/* Poll for ESP-hosted DHCP lease, then start RTSP server + test source.
 * User still drives eh_init + eh_connect from the shell. */
static void rtsp_autostart_task(void *arg)
{
    (void)arg;
    while (!esp_hosted_netif_is_ip_ready()) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    printf("[rtsp] IP ready, starting server on port %d\r\n",
           CONFIG_RTSP_LISTEN_PORT);
    /* rtsp_init_lib() fails with rc=-1 unless a get_frm_t callback has
     * been set first; install the test source before init. */
#ifdef CONFIG_RTSP_TEST_SOURCE
    rtsp_test_source_install();
    printf("[rtsp] test source installed @ %d fps\r\n",
           CONFIG_RTSP_DEFAULT_VIDEO_FPS);
#endif
    rtsp_set_video_en(1);
    rtsp_set_videoFmt(RTSP_VIDEOFMT_MJPEG);
    rtsp_set_video_fps(CONFIG_RTSP_DEFAULT_VIDEO_FPS);
    int rc = rtsp_init_lib();
    if (rc != 0) {
        printf("[rtsp] rtsp_init_lib rc=%d\r\n", rc);
    }
    vTaskDelete(NULL);
}
#endif

#ifdef CONFIG_BLUETOOTH_LE_ENABLE
static void ble_fsm_task(void *arg)
{
    extern void app_process_fsm(void);
    extern void app_set_fsm_task_handle(TaskHandle_t task_handle);
    
    // Register task handle for event notifications
    app_set_fsm_task_handle(xTaskGetCurrentTaskHandle());
    
    while(1) {
        app_process_fsm();  // Blocks waiting for notification or 100ms timeout
    }
}
#endif
static void aos_loop_proc(void *arg)
{
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
    evfs_init();
#endif

#ifdef CONFIG_IPRO_EVENT
    /* Initialize event loop */
    int ret = ipro_event_loop_create_default();
    if (ret == IPRO_OK) {
        printf("[EVENT] Event loop initialized\r\n");
#ifdef CONFIG_CMD_SCHEDULER
        /* Initialize command scheduler */
        cmd_scheduler_init();
#ifdef CONFIG_DEMO_EMAC
        static const cmd_entry_t emac_lwip_sequence[] = {
            { "emac_lwip", 0 },
            CMD_LIST_END
        };
        cmd_scheduler_run(emac_lwip_sequence, 1000);
#else
	#ifdef CONFIG_DEMO_LCD_AI // Enable this definition for AI demo on LCD.
	        static const cmd_entry_t emac_lwip_sequence[] = {
	            { "ai_run_ext 1", 6000 },
	            { "lcd_ai_demo", 1000 },
	            CMD_LIST_END
	        };
	        cmd_scheduler_run(emac_lwip_sequence, 1000);
	#endif
#endif
#endif
    } else {
        printf("[EVENT] Failed to create event loop: %d\r\n", ret);
    }
#endif



#ifdef CONFIG_SHELL
    puts("Init Shell (RT-Thread FinSH based)\r\n");
    shell_init_with_task(UART0_INDEX);
#endif

#ifdef CONFIG_RTSP_AUTOSTART
    xTaskCreate(rtsp_autostart_task, "rtsp_start", 2048, NULL,
                tskIDLE_PRIORITY + 1, NULL);
    printf("[rtsp] autostart task created (polls for IP)\r\n");
#endif

    /* Initialization complete - delete this task to free scheduler resources.
     * Note: Static stack memory remains allocated but task is removed from scheduler.
     * Shell and event loop run in their own tasks. */
    vTaskDelete(NULL);
}

static ATTR_PSRAM_SECTION StackType_t aos_loop_proc_stack[1024];
static StaticTask_t aos_loop_proc_task;

#if defined(CONFIG_LINK_FLASH) && defined(CONFIG_USE_PSRAM)
/* External symbol from startup code - indicates PSRAM availability */
extern uint32_t __psram_available;
#endif

int main(void)
{
    platform_init(0);
    ipro_setup_heap();
    hal_boot2_init();

    /* Print reset reason */
    printf("[SYS] Reset reason: %s (%d)\r\n",
           hal_rst_reason_str(hal_rst_reason_get()),
           hal_rst_reason_get());

#if defined(CONFIG_LINK_FLASH) && defined(CONFIG_USE_PSRAM)
    /* Check PSRAM availability (initialized in _premain_init) */
    if (__psram_available) {
        printf("[PSRAM] PSRAM initialized successfully\r\n");
    } else {
        printf("[PSRAM] WARNING: PSRAM initialization failed, running without PSRAM\r\n");
    }
#endif
    ipro_log_init(NULL);

#ifdef CONFIG_AON_WDT_ENABLE
    {
        volatile hal_hbnram_sysinfo_t *si = hal_hbnram_sysinfo;

        /* Check if boot2 already enabled AON WDT */
        uint32_t wdt_reg = readl(HBN_BASE + HBN_WDT_OFFSET);
        if (wdt_reg & HBN_CR_ANO_WDT_EN_MSK) {
            printf("[AON_WDT] Enabled by boot2, extending timeout\r\n");
        } else {
            printf("[AON_WDT] Not enabled, initializing\r\n");
        }

        /* (Re)init with FW timeout: mask_bit=19 → ~32s */
        aon_wdt_init(19);
        si->boot_stage = BOOT_STAGE_FW_EARLY;
        si->boot_flags |= BOOT_FLAG_WDT_ENABLED;
        printf("[AON_WDT] Timeout ~32s\r\n");
    }
#endif
    /* When CONFIG_AON_WDT_ENABLE is not set, platform_init() already disabled WDT */

#ifdef CONFIG_BLUETOOTH_LE_ENABLE
    /* Initialize BLE demo */
    printf("[BLE] Initializing BLE demo...\r\n");
    int ble_ret = ble_demo_init();
    if (ble_ret != 0) {
        printf("[BLE] Failed to initialize BLE: %d\r\n", ble_ret);
    }
    
    /* Create BLE FSM processing task */
    static StackType_t ble_fsm_task_stack[512];
    static StaticTask_t ble_fsm_task_tcb;
    
    xTaskCreateStatic(
        ble_fsm_task,
        "ble_fsm",
        sizeof(ble_fsm_task_stack) / sizeof(StackType_t),
        NULL,
        10,  // Same priority as aos_loop
        ble_fsm_task_stack,
        &ble_fsm_task_tcb
    );
    printf("[BLE] BLE FSM task created\r\n");
#endif

    printf("[OS] Starting loop_proc1 task...\r\n");

    xTaskCreateStatic(aos_loop_proc, (char *)"aos_loop", sizeof(aos_loop_proc_stack) / 4, NULL, 10, aos_loop_proc_stack, &aos_loop_proc_task);

#ifdef CONFIG_AON_WDT_ENABLE
    /* Kicker task: feeds WDT every 1s */
    xTaskCreate(aon_wdt_kicker_task, "wdt_kick", 256, NULL,
                configMAX_PRIORITIES - 2, NULL);

    /* Boot success timer: clears WDT counter after 5s */
    hal_hbnram_sysinfo->boot_stage = BOOT_STAGE_FW_READY;
    {
        TimerHandle_t t = xTimerCreate("wdt_boot",
            pdMS_TO_TICKS(5000), pdFALSE, NULL, aon_wdt_boot_ok_cb);
        if (t)
            xTimerStart(t, 0);
    }
    printf("[AON_WDT] Kicker task + 5s boot timer started\r\n");
#endif

    printf("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();
}
