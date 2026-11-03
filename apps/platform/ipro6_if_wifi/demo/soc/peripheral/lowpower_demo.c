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

#include <generated/autoconf.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"
#include "hal_sys.h"
#include "hal_timer.h"
#if defined(IPRO7)
#include "ipro7.h"
#elif defined(IPRO6)
#include "ipro6.h"
#endif
#include "hal_pm.h"

#if defined(IPRO7)
/* LP Framework only available on IPRO7 */
#include "hal_lp.h"

/* FreeRTOS timer setup */
extern void vPortSetupTimerInterrupt(void);

static uint8_t g_lp_initialized = 0;

/**
 * @brief Wakeup hook for peripheral reinitialization
 *
 * This hook is called after every LP wakeup. It reinitializes:
 * - FreeRTOS SysTick timer (for task scheduling)
 * - UART console (for log output and CLI input)
 *
 * Users can add additional peripheral reinitialization here.
 */
extern void shell_uart_reinit(void);
static int lp_wakeup_reinit_hook(int wakeup_reason, void *arg)
{
    (void)arg;
    (void)wakeup_reason;

    /* Reinitialize UART console for log output and CLI input */
    shell_uart_reinit();

    /* Reinitialize FreeRTOS timer for scheduler*/
    vPortSetupTimerInterrupt();

    /*
     * TODO: Add your peripheral reinitialization here if needed.
     * Examples:
     *   - Reinitialize I2C/SPI peripherals
     *   - Reinitialize BLE HW
     */

    return 0;
}

/**
 * @brief LP Framework Test Command
 *
 * Usage: lp_test [sleep_ms] [loop_count]
 *   sleep_ms   - Sleep duration in milliseconds (default: 1000)
 *   loop_count - Number of sleep/wakeup cycles (default: 1)
 *
 * This command tests the LP firmware wakeup flow:
 *   1. Initialize LP framework (hal_lp_init)
 *   2. Configure RTC wakeup
 *   3. Enter PDS sleep via hal_lp_enter
 *   4. LP firmware (hal_lp_wakeup_entry) handles wakeup:
 *      - Assembly entry: restore sp/gp, jump to C handler
 *      - C handler: XIP recovery, PSRAM restore, CPU restore
 *   5. Return here and print wakeup info
 */
static int cmd_lp_test(int argc, char **argv)
{
    uint32_t sleep_ms = 1000;
    uint32_t loop_count = 1;
    int ret;

    if (argc >= 2) {
        sleep_ms = atoi(argv[1]);
    }
    if (argc >= 3) {
        loop_count = atoi(argv[2]);
    }

    printf("\r\n=== LP Framework Test ===\r\n");
    printf("Sleep duration: %lu ms\r\n", sleep_ms);
    printf("Loop count: %lu\r\n", loop_count);

    /* Initialize LP framework if not done */
    if (!g_lp_initialized) {
        printf("Initializing LP framework...\r\n");
        ret = hal_lp_init();
        if (ret != 0) {
            printf("ERROR: hal_lp_init failed: %d\r\n", ret);
            return ret;
        }

        /* Register wakeup hook for peripheral reinitialization */
        hal_lp_register_wakeup_hook(lp_wakeup_reinit_hook, NULL);

        g_lp_initialized = 1;
        printf("LP framework initialized\r\n");
    }

    /* Configure LP settings */
    hal_lp_cfg_t lp_cfg = {
        .rtc_wakeup_en = 1,
        .rtc_sleep_us = sleep_ms * 1000ULL,
        .gpio_wakeup_en = 0,
        .gpio_cfg = NULL,
        .flash_power_down = 1,      /* Power down flash to save power */
        .psram_retention = 1,       /* Keep PSRAM in retention mode */
        .ocram_retention = 1,
        .pre_sleep_cb = NULL,
        .post_wakeup_cb = NULL,
    };

    hal_lp_result_t lp_result;

    for (uint32_t i = 0; i < loop_count; i++) {
        printf("\r\n--- Cycle %lu/%lu ---\r\n", i + 1, loop_count);
        printf("Entering PDS sleep for %lu ms...\r\n", sleep_ms);
        arch_delay_ms(10);  /* Allow UART to flush */

        /* Enter low power mode */
        ret = hal_lp_enter(&lp_cfg, &lp_result);

        /* After wakeup */
        printf("\r\n*** Wakeup! ***\r\n");
        printf("Wakeup reason: 0x%x", ret);
        if (ret & HAL_LP_WAKEUP_RTC) {
            printf(" [RTC]");
        }
        if (ret & HAL_LP_WAKEUP_GPIO) {
            printf(" [GPIO: 0x%lx]", lp_result.wakeup_gpio_bits);
        }
        printf("\r\n");
        printf("Sleep duration: %llu us\r\n", lp_result.sleep_duration_us);
        printf("Virtual time: %llu ms\r\n", hal_lp_get_virtual_ms());

        if (ret < 0) {
            printf("ERROR: hal_lp_enter failed: %d\r\n", ret);
            break;
        }

        /* Small delay between cycles */
        if (i < loop_count - 1) {
            arch_delay_ms(10000);
        }
    }

    printf("\r\n=== LP Wakeup Complete ===\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lp_test, lp_test, LP Framework Test [sleep_ms] [loop_count]);

/**
 * @brief LP GPIO Wakeup Test Command
 *
 * Usage: lp_gpio_test <gpio_pin> [trigger_mode]
 *   gpio_pin     - GPIO pin number for wakeup (0-31)
 *   trigger_mode - 0=falling, 1=rising, 2=low, 3=high (default: 0)
 *
 * This command tests GPIO wakeup with LP firmware.
 */
static int cmd_lp_gpio_test(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: lp_gpio_test <gpio_pin> [trigger_mode]\r\n");
        printf("  trigger_mode: 0=falling, 1=rising, 2=low, 3=high\r\n");
        return -1;
    }

    uint8_t gpio_pin = atoi(argv[1]);
    hal_lp_gpio_trig_t trig_mode = HAL_LP_GPIO_TRIG_FALLING_EDGE;

    if (argc >= 3) {
        trig_mode = (hal_lp_gpio_trig_t)atoi(argv[2]);
    }

    if (gpio_pin > 31) {
        printf("ERROR: Invalid GPIO pin %d\r\n", gpio_pin);
        return -1;
    }

    printf("\r\n=== LP GPIO Wakeup Test ===\r\n");
    printf("GPIO pin: %d\r\n", gpio_pin);
    printf("Trigger mode: %d\r\n", trig_mode);

    /* Initialize LP framework if not done */
    if (!g_lp_initialized) {
        printf("Initializing LP framework...\r\n");
        int ret = hal_lp_init();
        if (ret != 0) {
            printf("ERROR: hal_lp_init failed: %d\r\n", ret);
            return ret;
        }
        g_lp_initialized = 1;
    }

    /* Configure GPIO wakeup */
    hal_lp_gpio_cfg_t gpio_cfg = {
        .wakeup_mask = (1 << gpio_pin),
        .default_trig = trig_mode,
        .default_res = HAL_LP_GPIO_RES_PULL_UP,
        .pin_cfg = NULL,
        .pin_cfg_count = 0,
    };

    hal_lp_cfg_t lp_cfg = {
        .rtc_wakeup_en = 1,
        .rtc_sleep_us = 30000000ULL,    /* 30 second timeout */
        .gpio_wakeup_en = 1,
        .gpio_cfg = &gpio_cfg,
        .flash_power_down = 1,
        .psram_retention = 1,
        .ocram_retention = 1,
        .pre_sleep_cb = NULL,
        .post_wakeup_cb = NULL,
    };

    hal_lp_result_t lp_result;

    printf("Entering PDS sleep (GPIO wakeup or 30s timeout)...\r\n");
    arch_delay_ms(10);

    int ret = hal_lp_enter(&lp_cfg, &lp_result);

    printf("\r\n*** Wakeup! ***\r\n");
    printf("Wakeup reason: 0x%x", ret);
    if (ret & HAL_LP_WAKEUP_RTC) {
        printf(" [RTC timeout]");
    }
    if (ret & HAL_LP_WAKEUP_GPIO) {
        printf(" [GPIO: 0x%lx]", lp_result.wakeup_gpio_bits);
    }
    printf("\r\n");
    printf("Sleep duration: %llu us\r\n", lp_result.sleep_duration_us);

    printf("=== LP GPIO Test Complete ===\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lp_gpio_test, lp_gpio_test, LP GPIO Wakeup Test <gpio_pin> [trigger_mode]);

#else /* !IPRO7 - IPRO6 stub commands */

static int cmd_lp_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("lp_test: LP Framework not available on this platform\r\n");
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lp_test, lp_test, LP Framework Test [sleep_ms] [loop_count]);

static int cmd_lp_gpio_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("lp_gpio_test: LP Framework not available on this platform\r\n");
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lp_gpio_test, lp_gpio_test, LP GPIO Wakeup Test <gpio_pin> [trigger_mode]);

#endif /* IPRO7 */

/* PDS/HBN demos - available on all platforms */
static int cmd_pds_demo(int argc, char **argv)
{
    int pds_timeout = 0;
    if (argc == 2)
        pds_timeout = atoi(argv[1])*32768;
    printf("PDS Timeout: %u\r\n", pds_timeout);
    arch_delay_ms(10);
    pm_pds_mode_enter(PM_PDS_LEVEL_31, pds_timeout);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pds_demo, pds_demo, PDS Demo);

static int cmd_hbn_demo(int argc, char **argv)
{
    int timeout = 0;
    if (argc == 2)
        timeout = atoi(argv[1])*32768;
    printf("HBN Timeout: %u\r\n", timeout);
    arch_delay_ms(10);
    pm_hbn_mode_enter(PM_HBN_LEVEL_0, timeout);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_hbn_demo, hbn_demo, HBN Demo);

/* Init function to ensure this file is linked */
void lowpower_demo_init(void) {}
