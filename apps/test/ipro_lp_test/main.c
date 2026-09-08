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
 * @brief Low Power Test — No BLE, No PSRAM
 *
 * Minimal project for testing PDS Level 31 on no-PSRAM boards.
 * Uses shell CLI for interactive PDS control.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <generated/autoconf.h>
#ifdef CONFIG_SHELL
#include "shell.h"
#endif
#include <FreeRTOS.h>
#include <task.h>

#include "hal_sys.h"
#include "hal_timer.h"
#include "hal_uart.h"
#include "hal_boot2.h"
#include "platform_common.h"

#include "hal_lp.h"
#include "hal_hbnram_mgnt.h"
#include "hal_gpio.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_pds.h"
#include "src/lp_test.h"

#ifdef CONFIG_EASYFLASH4
#include "easyflash.h"
#endif

#include "freertos_app_hooks.h"
#include "ipro_log.h"

#define LOG_TAG "MAIN"

/* LP diagnostic data saved from retained OCRAM before platform_init */
uint32_t g_lp_diag[4];
uint32_t g_lp_rsv2, g_lp_rsv3;  /* HBN trace saved before platform_init */

/*
 * FREERTOS HOOKS — idle hook calls PDS when auto-enabled
 */

void __attribute__((weak)) vApplicationIdleHook(void)
{
    if (lp_test_auto_is_enabled()) {
        /* Sleep 1 second per idle cycle — RTC wakeup */
        lp_test_sleep(1000);
    }
}

/* Smaller stacks for LP — no BLE, minimal workload */
void __attribute__((weak)) vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                                          StackType_t **ppxIdleTaskStackBuffer,
                                                          configSTACK_DEPTH_TYPE *pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[256];
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = 256;
}

void __attribute__((weak)) vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                                           StackType_t **ppxTimerTaskStackBuffer,
                                                           uint32_t *pulTimerTaskStackSize)
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[512];
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = 512;
}

/*
 * SYSTEM INIT TASK
 */

static void aos_loop_proc(void *pvParameters)
{
    (void)pvParameters;

    ipro_log_init(NULL);

#ifdef CONFIG_EASYFLASH4
    easyflash_init();
#endif

#ifdef CONFIG_SHELL
    shell_init_with_task(UART0_INDEX);
#endif

    /* Initialize LP test module */
    lp_test_init();

    IPRO_LOGI(LOG_TAG, "");
    IPRO_LOGI(LOG_TAG, "======================================");
    IPRO_LOGI(LOG_TAG, "  IPRO Low Power Test (No BLE)");
    IPRO_LOGI(LOG_TAG, "======================================");
    IPRO_LOGI(LOG_TAG, "CLI: 'lp help' for commands");
    IPRO_LOGI(LOG_TAG, "Free heap: %u bytes", (unsigned)xPortGetFreeHeapSize());
    IPRO_LOGI(LOG_TAG, "");

    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

/*
 * MAIN ENTRY
 */

int main(void)
{
    static StackType_t aos_loop_proc_stack[512];
    static StaticTask_t aos_loop_proc_task;

    /* Clear HBN_RSV0 immediately so hardware reset always cold boots.
     * Without this, if LP firmware crashes, reset re-enters LP → infinite crash loop. */
    *(volatile uint32_t *)(HBN_BASE + HBN_RSV0_OFFSET) = 0;

    /* Read LP diagnostic from OCRAM at 0x1101FFF0 (survives bootrom cold boot).
     * LP assembly writes here AFTER SRAM config restore (Stage 2+). */
    {
        volatile uint32_t *lp_diag = (volatile uint32_t *)0x1101FFF0;
        extern uint32_t g_lp_diag[4];
        g_lp_diag[0] = lp_diag[0];
        g_lp_diag[1] = lp_diag[1];
        g_lp_diag[2] = lp_diag[2];
        g_lp_diag[3] = lp_diag[3];
        lp_diag[0] = 0;  /* Clear for next cycle */
    }

    /* Save HBN trace registers BEFORE platform_init (which may clear them) */
    g_lp_rsv2 = *(volatile uint32_t *)(HBN_BASE + HBN_RSV2_OFFSET);
    g_lp_rsv3 = *(volatile uint32_t *)(HBN_BASE + HBN_RSV3_OFFSET);
    *(volatile uint32_t *)(HBN_BASE + HBN_RSV2_OFFSET) = 0;
    *(volatile uint32_t *)(HBN_BASE + HBN_RSV3_OFFSET) = 0;

    platform_init(0);
    ipro_setup_heap();

    /* IPRO7AI PDB: force sensor OFF.
     *   GPIO27 = 2V6_EN  (U87/U88 SGM2019 LDO enable chain) -> LOW kills sensor rails
     *   GPIO36 = XSHUTND (SC101 sensor active-low shutdown) -> LOW puts sensor in standby
     *
     * In ACTIVE: drive both LOW via normal GPIO.
     * In PDS31:  normal GPIO is power-gated, pins go Hi-Z. We enable the
     *            PDS-domain internal pull-down on both pins so they stay LOW
     *            through sleep. Without this, sensor wakes during PDS and
     *            board draws ~1.9 mA instead of ~uA. */
    {
        uint8_t sensor_off_pins[2] = { 27, 36 };
        GLB_GPIO_Func_Init(GPIO_FUN_GPIO, sensor_off_pins, sizeof(sensor_off_pins));
        gpio_set_mode(27, GPIO_OUTPUT_MODE);
        gpio_set_mode(36, GPIO_OUTPUT_MODE);
        gpio_write(27, 0);
        gpio_write(36, 0);

        /* PDS-domain pull-down so the LOW survives PDS31. */
        PDS_GPIO_Cfg_Type pd_cfg = { .pd_en = 1, .pu_en = 0, .ie_en = 0, .oe_en = 0 };
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_27, &pd_cfg);
        PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_PIN_36, &pd_cfg);
    }

    IPRO_LOGI(LOG_TAG, "IPRO SDK — Low Power Test (No PSRAM)");
    /* printf — IPRO_LOGI isn't initialized this early. */
    printf("[PDB] GPIO27=%d GPIO36=%d (both LOW = sensor off)\r\n",
           gpio_read(27), gpio_read(36));

    /* Print OCRAM diagnostic (survives bootrom) */
    printf("[LP_DIAG] OCRAM @0x1101FFF0: magic=0x%08lx stage=%lu mcause=0x%08lx mepc=0x%08lx\r\n",
           (unsigned long)g_lp_diag[0], (unsigned long)g_lp_diag[1],
           (unsigned long)g_lp_diag[2], (unsigned long)g_lp_diag[3]);
    if (g_lp_diag[0] == 0x4C504400) {
        printf("[LP_DIAG] >>> LP firmware ENTERED! Last stage=%lu\r\n",
               (unsigned long)g_lp_diag[1]);
        if (g_lp_diag[2] != 0)
            printf("[LP_DIAG] >>> EXCEPTION: mcause=0x%lx mepc=0x%lx\r\n",
                   (unsigned long)g_lp_diag[2], (unsigned long)g_lp_diag[3]);
    } else if (g_lp_diag[0] == 0xDEADBEEF) {
        printf("[LP_DIAG] >>> Pre-PDS marker intact — LP firmware NEVER RAN!\r\n");
        printf("[LP_DIAG] >>> Bootrom did NOT jump to HBN RAM.\r\n");
    } else if (g_lp_diag[0] == 0) {
        printf("[LP_DIAG] >>> No diagnostic data (fresh boot or zeroed)\r\n");
    } else {
        printf("[LP_DIAG] >>> Unknown marker — possibly heap garbage (no LP sleep)\r\n");
    }

    /* Detect PDS wakeup from RSV2 magic */
    printf("[LP_TRACE] RSV2=0x%08lx RSV3=0x%08lx\r\n",
           (unsigned long)g_lp_rsv2, (unsigned long)g_lp_rsv3);
    if (g_lp_rsv2 == 0x4C505753) {
        /* "LPWS" magic — this is a PDS wakeup cold boot! */
        uint32_t rtc_lo, rtc_hi;
        HBN_Get_RTC_Timer_Val(&rtc_lo, &rtc_hi);
        uint64_t rtc_now = ((uint64_t)rtc_hi << 32) | rtc_lo;

        printf("*****************************************************\r\n");
        printf("*** PDS WAKEUP DETECTED! (cold boot after PDS31) ***\r\n");
        printf("*** RTC now: %llu ms                               ***\r\n",
               (unsigned long long)(rtc_now * 1000ULL / 32768ULL));
        printf("*****************************************************\r\n");
    } else if ((g_lp_rsv2 & 0xFFFF0000) == 0x4C500000) {
        uint32_t step = g_lp_rsv2 & 0xFF;
        printf("[LP_TRACE] LP firmware step=0x%02lx saved_RA=0x%08lx\r\n",
               (unsigned long)step, (unsigned long)g_lp_rsv3);
    }

    hal_boot2_init();

    /* Detect LP fastboot wakeup — HBN RAM context survived cold boot */
    if (hal_lp_is_fastboot_wakeup()) {
        const volatile hal_hbnram_lp_wake_ctx_t *ctx = hal_lp_get_wake_context();
        printf("*** LP FASTBOOT WAKE #%lu (reason=0x%lx) ***\r\n",
               (unsigned long)ctx->wake_counter,
               (unsigned long)ctx->wake_reason);
    }

    /* Initialize Low Power framework */
    int ret = hal_lp_init();
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "LP init failed: %d", ret);
        return -1;
    }

    xTaskCreateStatic(aos_loop_proc, (char*)"aos_loop",
                      sizeof(aos_loop_proc_stack)/sizeof(StackType_t), NULL, 10,
                      aos_loop_proc_stack, &aos_loop_proc_task);

    vTaskStartScheduler();

    IPRO_LOGE(LOG_TAG, "Scheduler exited!");
    return -1;
}
