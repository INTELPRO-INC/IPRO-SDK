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
 * @file lp_test.c
 * @brief Low Power Test — PDS Level 31 via hal_lp framework (No BLE)
 *
 * Simple PDS sleep test without BLE. Uses RTC timer wakeup.
 * After wakeup, reinitializes UART and SysTick.
 */

#include "lp_test.h"
#include "ipro_log.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "hal_lp.h"
#include "drv_uart.h"
#include "drv_gpio.h"
#include "drv_glb.h"

#define LOG_TAG "LP_TEST"

/* Wait for UART TX to complete before entering PDS */
static void uart_flush(void)
{
    /* Wait for TX FIFO to drain and bus to go idle */
    while (UART_GetTxFifoCount(UART0_ID) > 0) {}
    while (UART_GetTxBusBusyStatus(UART0_ID)) {}
    /* Extra guard: ~1ms at 115200 for last byte shift out */
    for (volatile int i = 0; i < 10000; i++) {}
}

/* Minimum sleep duration to bother entering PDS (ms) */
#define LP_MIN_SLEEP_MS     5

/*
 * PDS ENTRY TRAMPOLINE
 *
 * Problem: FreeRTOS task stacks may be in non-retention OCRAM (heap at
 * 0x11020000+). PDS only retains the first 64KB (0x11010000~0x11020000).
 * On wakeup, stack contents at heap addresses are garbage → crash.
 *
 * Solution: Switch to a small stack in retention OCRAM (.data section)
 * before calling hal_lp_enter().
 */
#define PDS_STACK_SIZE      2048

static uint8_t __attribute__((aligned(16), used))
    s_pds_stack[PDS_STACK_SIZE];

/*
 * MODULE STATE
 */

static bool     s_initialized   = false;
static bool     s_auto_enabled  = false;
static uint32_t s_total_ms      = 0;
static uint32_t s_sleep_count   = 0;
static uint32_t s_last_ms       = 0;
static int32_t  s_last_wakeup   = 0;

/* Manual sleep argument (naked function can't forward args) */
uint32_t s_lp_test_sleep_ms;

/*
 * WAKEUP HOOK — reinitialize peripherals after PDS wakeup
 */

static int lp_wakeup_hook(int wakeup_reason, void *arg)
{
    (void)arg;

    /* PDS31 resets MISC domain — GPIO pin mux is lost.
     * Restore UART GPIO pins and signal routing before shell_uart_reinit. */
    {
        GLB_GPIO_Cfg_Type uart_gpio = {
            .gpioPin = GLB_GPIO_PIN_20,
            .gpioFun = 7,  /* UART function */
            .gpioMode = GPIO_MODE_AF,
            .pullType = GPIO_PULL_UP,
            .drive = 0,
            .smtCtrl = 1,
        };
        GLB_GPIO_Init(&uart_gpio);
        uart_gpio.gpioPin = GLB_GPIO_PIN_21;
        GLB_GPIO_Init(&uart_gpio);
        /* Restore UART signal routing */
        GLB_UART_Fun_Sel(GLB_UART_SIG_8, GLB_UART_SIG_FUN_UART0_TXD);
        GLB_UART_Fun_Sel(GLB_UART_SIG_9, GLB_UART_SIG_FUN_UART0_RXD);
    }

    /* Reinitialize UART for shell/logging */
#ifdef CONFIG_SHELL
    extern void shell_uart_reinit(void);
    shell_uart_reinit();
#endif

    /* Reinitialize FreeRTOS SysTick timer */
    extern void vPortSetupTimerInterrupt(void);
    vPortSetupTimerInterrupt();

    return 0;
}

/*
 * PDS sleep implementation — runs on retention stack
 */
static void __attribute__((used)) lp_test_sleep_impl(void)
{
    uint32_t sleep_ms = s_lp_test_sleep_ms;

    if (!s_initialized) {
        IPRO_LOGW(LOG_TAG, "Not initialized");
        return;
    }
    if (sleep_ms < LP_MIN_SLEEP_MS) {
        return;
    }

    hal_lp_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.rtc_wakeup_en = 1;
    cfg.rtc_sleep_us = (uint64_t)sleep_ms * 1000ULL;
    cfg.flash_power_down = 0;  /* Start simple: don't power down flash */
    cfg.ocram_retention = 1;

    hal_lp_result_t result;
    memset(&result, 0, sizeof(result));

    IPRO_LOGI(LOG_TAG, "PDS sleep: %lu ms (rtc_us=%llu, flash_pd=%d, ocram_ret=%d)",
              (unsigned long)sleep_ms,
              (unsigned long long)cfg.rtc_sleep_us,
              cfg.flash_power_down,
              cfg.ocram_retention);
    uart_flush();

    int ret = hal_lp_enter(&cfg, &result);
    (void)ret;

    /* === WAKEUP === */

    /* Compensate FreeRTOS tick counter */
    uint32_t actual_us = (uint32_t)result.sleep_duration_us;
    if (actual_us == 0) actual_us = sleep_ms * 1000;
    uint32_t actual_ms = actual_us / 1000;

    uint32_t sleep_ticks = actual_us / (1000000UL / configTICK_RATE_HZ);
    if (sleep_ticks > 0) {
        xTaskCatchUpTicks(sleep_ticks);
    }

    /* Update statistics */
    s_total_ms += actual_ms;
    s_sleep_count++;
    s_last_ms = actual_ms;
    s_last_wakeup = result.wakeup_reason;

    IPRO_LOGI(LOG_TAG, "Woke: reason=0x%lx, %lu ms (cnt=%lu)",
              (unsigned long)result.wakeup_reason,
              (unsigned long)actual_ms,
              (unsigned long)s_sleep_count);
}

/*
 * Naked trampoline — switch to retention stack before PDS
 */
__attribute__((naked))
void lp_test_sleep(uint32_t sleep_ms)
{
    (void)sleep_ms;
    __asm__ volatile(
        /* save sleep_ms (a0) before switching stack */
        "la   t1, s_lp_test_sleep_ms\n"
        "sw   a0, 0(t1)\n"
        "mv   t0, sp\n"
        "la   sp, s_pds_stack\n"
        "li   t1, %0\n"
        "add  sp, sp, t1\n"
        "addi sp, sp, -8\n"
        "sw   t0, 0(sp)\n"
        "sw   ra, 4(sp)\n"
        "call lp_test_sleep_impl\n"
        "lw   ra, 4(sp)\n"
        "lw   t0, 0(sp)\n"
        "mv   sp, t0\n"
        "ret\n"
        :
        : "i"(PDS_STACK_SIZE)
    );
}

/*
 * PUBLIC API
 */

void lp_test_init(void)
{
    s_initialized = false;
    s_auto_enabled = false;
    s_total_ms = 0;
    s_sleep_count = 0;
    s_last_ms = 0;
    s_last_wakeup = 0;

    hal_lp_register_wakeup_hook(lp_wakeup_hook, NULL);

    s_initialized = true;
    IPRO_LOGI(LOG_TAG, "LP test init done");
}

void lp_test_auto_enable(bool enable)
{
    s_auto_enabled = enable;
    IPRO_LOGI(LOG_TAG, "Auto PDS %s", enable ? "enabled" : "disabled");
}

bool lp_test_auto_is_enabled(void)
{
    return s_auto_enabled;
}

void lp_test_get_stats(uint32_t *total_sleep_ms,
                       uint32_t *sleep_count,
                       uint32_t *last_sleep_ms,
                       int32_t *last_wakeup_reason)
{
    if (total_sleep_ms)   *total_sleep_ms   = s_total_ms;
    if (sleep_count)      *sleep_count      = s_sleep_count;
    if (last_sleep_ms)    *last_sleep_ms    = s_last_ms;
    if (last_wakeup_reason) *last_wakeup_reason = s_last_wakeup;
}

void lp_test_reset_stats(void)
{
    s_total_ms = 0;
    s_sleep_count = 0;
    s_last_ms = 0;
    s_last_wakeup = 0;
}
