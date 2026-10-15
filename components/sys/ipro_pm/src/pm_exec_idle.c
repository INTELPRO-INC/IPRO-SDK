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
 * @file pm_exec_idle.c
 * @brief ipro_pm idle-task PDS executor.
 *
 * For radios that do not run a PDS transaction themselves (802.15.4 / Thread,
 * or no radio at all), the idle task enters PDS31 right from the tickless
 * hook, with interrupts masked and the scheduler suspended:
 *
 *   size the sleep     min(RTOS deadline, participants' deadline) minus the
 *                      wake path and the 32 kHz clock's error bound
 *   vetoes, devices    the application and the radio participants may refuse
 *   enter              hal_lp_enter() with the executor's domain policy and
 *                      an absolute RTC target; the entry aborts (nothing lost)
 *                      if an interrupt is pending or PDS never engages
 *   restore            MTIME written forward by the RTC-measured sleep, stale
 *                      timer interrupts dropped, RTOS ticks stepped (the rest
 *                      caught up from the idle hook), console, devices
 *
 * The port then returns to the scheduler without WFI (PM_OFFER_EXECUTED).
 * Never used while another participant can be the executor: only the BLE
 * controller can keep the BLE core across a PDS, so the arbiter only gets
 * here when every present participant is READY.
 */
#include <stdio.h>
#include <string.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include "ipro7.h"
#include "drv_common.h"
#include "glb_reg.h"
#include "hal_lp.h"
#include "hal_pm.h"
#include "ipro_pm.h"
#include "pm_internal.h"

/* Domain policy for the executor's PDS: MISC (UART, the whole OCRAM) kept
 * live and out of reset, the BLE domain powered down, RF under PDS control.
 * BLE_PWR_OFF also keeps the SoC reset line asserted through the PDS
 * (pdsRstSocEn): without it a PDS about one time in 150-250 never wakes up
 * (no HAL wake marker, the watchdog ends it).  The SoC reset clears the
 * 802.15.4 MAC, the timer block's clock setup and the AHB clock gates; the
 * gates are restored here, the rest by their owners (see ot_pm.c). */
#ifndef CONFIG_IPRO_PM_IDLE_PDS_DOMAIN_POLICY
#define CONFIG_IPRO_PM_IDLE_PDS_DOMAIN_POLICY                                                                  \
    (PM_PDS_BLE_CTLR_DOMAIN_POLICY_ENABLE | PM_PDS_BLE_CTLR_DOMAIN_POLICY_BLE_PWR_OFF |                        \
     PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_MISC | PM_PDS_BLE_CTLR_DOMAIN_POLICY_SAFE_OCRAM |                      \
     PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_MISC_LIVE)
#endif
/* RTC target to the CPU back in this function, then MTIME, console, devices */
#ifndef CONFIG_IPRO_PM_IDLE_PDS_WAKE_US
#define CONFIG_IPRO_PM_IDLE_PDS_WAKE_US 1500
#endif
#ifndef CONFIG_IPRO_PM_IDLE_PDS_GUARD_US
#define CONFIG_IPRO_PM_IDLE_PDS_GUARD_US 300
#endif
#ifndef CONFIG_IPRO_PM_IDLE_PDS_MIN_SLEEP_US
#define CONFIG_IPRO_PM_IDLE_PDS_MIN_SLEEP_US 10000
#endif
#ifndef CONFIG_IPRO_PM_IDLE_PDS_ENTRY_FALLBACK_US
#define CONFIG_IPRO_PM_IDLE_PDS_ENTRY_FALLBACK_US 50000
#endif

/* MTIME counts per RTOS tick */
#define PM_COUNTS_PER_TICK ((uint64_t)configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ)
/* consecutive aborted entries before the executor backs off to WFI */
#define PM_IDLE_ABORT_LIMIT 4U
#define PM_IDLE_BACKOFF_MIN_MS 10000U
#define PM_IDLE_BACKOFF_MAX_MS 600000U

static struct {
    uint64_t epoch_mtime;
    TickType_t epoch_tick;
    volatile TickType_t catchup;
    /* the idle task is handing owed ticks to the kernel (taken out of
     * catchup, not yet in the tick count): a task sync must not count them
     * again */
    volatile bool catchup_busy;
    bool valid;
} s_tick;

/* run-time copies of the Kconfig values (bench: `pm xpolicy`, `pm xfallback`) */
volatile uint32_t ipro_pm_idle_domain_policy = CONFIG_IPRO_PM_IDLE_PDS_DOMAIN_POLICY;
volatile uint32_t ipro_pm_idle_entry_fallback_us = CONFIG_IPRO_PM_IDLE_PDS_ENTRY_FALLBACK_US;
/* bench: shortest awake time between a PDS wake and the next entry (`pm xmingap`, us) */
volatile uint32_t ipro_pm_idle_min_awake_us;
volatile uint32_t ipro_pm_idle_too_soon;
#if defined(CONFIG_IPRO_PM_FAULT_INJECT)
/* bench: PDS with no radio participant (`pm xnoradio`) */
volatile uint32_t ipro_pm_idle_no_radio;
/* bench: the console TX pad parked as a GPIO driven high across the sleep
 * (`pm xtxpark`; the console restore muxes it back to UART0) */
volatile uint32_t ipro_pm_idle_tx_park;
#endif
static uint64_t s_last_wake_mtime;

static uint32_t s_consecutive_aborts;
static uint32_t s_backoff_ms = PM_IDLE_BACKOFF_MIN_MS;
static volatile bool s_backoff_request;
static TimerHandle_t s_backoff_timer;

#if defined(CONFIG_IPRO_PM_FAULT_INJECT)
/* bench: the next N entries start with the scheduler SWI pending, mode 1
 * with the HAL's pending-interrupt check (abort before entry), mode 2
 * without it; mode 3 makes PDS refuse to arm, so the WFI ends only on the
 * bounded fallback and the entry returns through the abort path */
volatile uint32_t ipro_pm_inject_abort_count;
volatile uint32_t ipro_pm_inject_abort_mode;
#endif

#if defined(CONFIG_IPRO_PM_LP_WDT)
/* Bench post-mortem: the AON watchdog runs on the 32 kHz clock through PDS,
 * so a hang (in or after a PDS) resets the chip without cutting power; the
 * next boot prints how far the executor and the HAL got (HBN RAM survives). */
#include "drv_hbn.h"
#include "hal_aon_wdt.h"
#define PM_BC_DEPTH 8U
static volatile uint32_t s_bc[2U + (2U * PM_BC_DEPTH)] __attribute__((section(".hbn_noinit"), used, aligned(4)));
static TickType_t s_wdt_fed;

static void pm_bc(uint32_t stage, uint32_t val)
{
    uint32_t i = s_bc[1] % PM_BC_DEPTH;

    s_bc[0] = UINT32_C(0x504d4243); /* "PMBC" */
    s_bc[2U + (2U * i)] = (stage << 24) | ((uint32_t)SysTimer_GetLoadValue() & 0x00ffffffU);
    s_bc[3U + (2U * i)] = val;
    s_bc[1]++;
}

static void pm_lp_wdt_boot_report(void)
{
    extern volatile uint32_t pm_pds_hbn_arm_diag[40];

    printf("[pmboot] wdt=%d rst_ev(por/ext/sw/pwr/bor)=%d%d%d%d%d bc magic=%08lx n=%lu hal stage=%08lx/%08lx"
           " ent=%08lx/%08lx\r\n",
           aon_wdt_get_reset_status(), HBN_Get_Reset_Event(HBN_RST_EVENT_POR_OUT),
           HBN_Get_Reset_Event(HBN_RST_EVENT_EXT_RST_N), HBN_Get_Reset_Event(HBN_RST_EVENT_SW_RST),
           HBN_Get_Reset_Event(HBN_RST_EVENT_PWR_RST_N), HBN_Get_Reset_Event(HBN_RST_EVENT_BOR_OUT),
           (unsigned long)s_bc[0], (unsigned long)s_bc[1], (unsigned long)pm_pds_hbn_arm_diag[30],
           (unsigned long)pm_pds_hbn_arm_diag[31], (unsigned long)pm_pds_hbn_arm_diag[36],
           (unsigned long)pm_pds_hbn_arm_diag[37]);
    /* the HAL's record of the last RTC arm: [1] HBN IRQ stat, [2..3] RTC
     * now, [4..5] target, [6] HBN_CTL armed, [7] PDS_INT, [8] "PDS0",
     * [15..18] domain words written, [19..23] PDS_CTL..STAT read back */
    for (uint32_t i = 0U; i < 24U; i += 8U) {
        printf("[pmboot] diag[%02lu] %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\r\n", (unsigned long)i,
               (unsigned long)pm_pds_hbn_arm_diag[i], (unsigned long)pm_pds_hbn_arm_diag[i + 1U],
               (unsigned long)pm_pds_hbn_arm_diag[i + 2U], (unsigned long)pm_pds_hbn_arm_diag[i + 3U],
               (unsigned long)pm_pds_hbn_arm_diag[i + 4U], (unsigned long)pm_pds_hbn_arm_diag[i + 5U],
               (unsigned long)pm_pds_hbn_arm_diag[i + 6U], (unsigned long)pm_pds_hbn_arm_diag[i + 7U]);
    }
    if (s_bc[0] == UINT32_C(0x504d4243)) {
        uint32_t n = s_bc[1];
        uint32_t k = (n < PM_BC_DEPTH) ? n : PM_BC_DEPTH;

        printf("[pmboot] last:");
        for (uint32_t j = 0U; j < k; j++) {
            uint32_t i = (n - k + j) % PM_BC_DEPTH;

            printf(" %lu:%06lx:%08lx", (unsigned long)(s_bc[2U + (2U * i)] >> 24),
                   (unsigned long)(s_bc[2U + (2U * i)] & 0x00ffffffU), (unsigned long)s_bc[3U + (2U * i)]);
        }
        printf("\r\n");
    }
    (void)HBN_Clear_Reset_Event();
    aon_wdt_clear_reset_status();
    s_bc[0] = 0U;
    s_bc[1] = 0U;
    aon_wdt_init(AON_WDT_TIMEOUT_64S);
    s_wdt_fed = xTaskGetTickCount();
}
#define PM_BC(stage, val) pm_bc((stage), (uint32_t)(val))
#else
#define PM_BC(stage, val) ((void)0)
#endif

volatile uint32_t ipro_pm_idle_executed;
volatile uint32_t ipro_pm_idle_aborted;
volatile uint32_t ipro_pm_idle_too_short;
volatile uint32_t ipro_pm_idle_vetoed;
volatile uint32_t ipro_pm_idle_dev_vetoed;
volatile uint32_t ipro_pm_idle_wake_rtc;
volatile uint32_t ipro_pm_idle_wake_gpio;
volatile uint32_t ipro_pm_idle_wake_other;
volatile uint32_t ipro_pm_idle_last_sleep_us;
volatile uint32_t ipro_pm_idle_last_elapsed_us;
volatile int32_t ipro_pm_idle_late_us_max;  /* woke later than planned (+) */
volatile uint32_t ipro_pm_idle_ticks_stepped;
volatile uint32_t ipro_pm_idle_ticks_caught_up;
/* ticks the port's tickless WFI would have credited in excess */
volatile uint32_t ipro_pm_idle_ticks_corrected;
/* the tick epoch restarted: the kernel led by more than a tick, or MTIME
 * went back */
volatile uint32_t ipro_pm_idle_tick_rebases;
/* AHB clock gates (GLB CGEN_CFG0..2) found changed after a wake, and the
 * last change: register index, before, after */
volatile uint32_t ipro_pm_idle_cgen_restores;
volatile uint32_t ipro_pm_idle_cgen_last[3];

/* -------------------------------------------------------------------------
 * RTOS tick bookkeeping: an epoch on MTIME, which the rebase keeps continuous
 * (idle executor and, in a BLE build, the controller's PDS).  The one tick
 * authority of a timebase build: the port's tickless exit, the executor and
 * the controller's native waits all step the kernel to it.
 * -----------------------------------------------------------------------*/
/* Taken with the scheduler running (the first executor pass): the kernel's
 * tick 0 is when it started, not when ipro_pm was initialised.  @p ahead:
 * ticks the kernel will have on top of xTaskGetTickCount() once the caller
 * is done (a step or a pended tick interrupt not counted yet). */
static void pm_idle_tick_epoch_init(TickType_t ahead)
{
    s_tick.epoch_mtime = SysTimer_GetLoadValue();
    s_tick.epoch_tick = xTaskGetTickCount() + ahead;
    s_tick.catchup = 0U;
    s_tick.valid = true;
}

/* The kernel's lead over the epoch's wall ticks: one tick is quantisation
 * (the tick interrupt fires part-way through an epoch tick) and stays;
 * more - another clock stepped it, or MTIME was not carried across a sleep -
 * is accepted as the new epoch rather than stalling the kernel until the
 * wall catches up.  Returns the ticks the kernel misses (0 when ahead). */
static TickType_t pm_idle_tick_missing(TickType_t current, TickType_t pended)
{
    uint64_t now = SysTimer_GetLoadValue();
    TickType_t wall;
    TickType_t missing;

    if (now < s_tick.epoch_mtime) {
        /* MTIME went back (a PDS nothing rebased): restart the epoch here */
        s_tick.epoch_mtime = now;
        s_tick.epoch_tick = current + pended;
        ipro_pm_idle_tick_rebases++;
        return 0U;
    }
    wall = (TickType_t)((now - s_tick.epoch_mtime) / PM_COUNTS_PER_TICK);
    missing = s_tick.epoch_tick + wall - (current + pended);
    if (missing >= (portMAX_DELAY / 2U)) {
        if ((TickType_t)(current + pended - (s_tick.epoch_tick + wall)) > 1U) {
            s_tick.epoch_tick = current + pended - wall;
            ipro_pm_idle_tick_rebases++;
        }
        return 0U;
    }
    return missing;
}

/* Scheduler suspended: step what the kernel may take now (never past the
 * selected window, the kernel's own next unblock at most), owe the rest. */
static void pm_idle_tick_sync(TickType_t cap)
{
    TickType_t missing = pm_idle_tick_missing(xTaskGetTickCount() + s_tick.catchup, 0U);

    if (missing != 0U) {
        TickType_t step = (missing < cap) ? missing : cap;

        if (step != 0U) {
            vTaskStepTick(step);
            ipro_pm_idle_ticks_stepped += step;
        }
        s_tick.catchup += missing - step;
    }
}

TickType_t pm_exec_idle_wall_ticks(TickType_t kernel_now)
{
    uint64_t now = SysTimer_GetLoadValue();
    TickType_t wall;

    if (!s_tick.valid || (now < s_tick.epoch_mtime)) {
        return kernel_now;
    }
    wall = s_tick.epoch_tick + (TickType_t)((now - s_tick.epoch_mtime) / PM_COUNTS_PER_TICK);
    /* the kernel may lead the floor-based wall by a tick */
    return ((TickType_t)(wall - kernel_now) < (portMAX_DELAY / 2U)) ? wall : kernel_now;
}

void pm_exec_idle_tick_task_sync(void)
{
    TickType_t owed;

    taskENTER_CRITICAL();
    if (!s_tick.valid) {
        pm_idle_tick_epoch_init(0U);
        taskEXIT_CRITICAL();
        return;
    }
    if (s_tick.catchup_busy) {
        taskEXIT_CRITICAL(); /* the next native wait syncs, a tick later */
        return;
    }
    owed = s_tick.catchup + pm_idle_tick_missing(xTaskGetTickCount() + s_tick.catchup, 0U);
    s_tick.catchup = 0U;
    taskEXIT_CRITICAL();
    if (owed != 0U) {
        ipro_pm_idle_ticks_caught_up += owed;
        /* unlike vTaskStepTick(), this walks past task deadlines safely */
        (void)xTaskCatchUpTicks(owed);
    }
}

/* The port's tickless WFI arms its compare relative to the running MTIME,
 * part-way through a tick, and credits whole ticks on the wake: up to one
 * tick fast per entry, ~10 % with a receiver on and short idle windows (a
 * MED: OpenThread's 750 ms Parent Request wait lasted 675 ms).  Count them
 * on the MTIME epoch instead - the ticks the kernel is missing, less the one
 * the tick interrupt pends when it ended the sleep (the port measures
 * xExpectedIdleTime - 1 exactly then, and less on any other wake); what the
 * kernel cannot take now is caught up from the idle hook.  IRQs masked
 * (port tickless exit, scheduler suspended). */
TickType_t xPortApplicationCorrectElapsedTicks(TickType_t xExpectedIdleTime, TickType_t xPortMeasuredTicks)
{
    TickType_t pended = (xPortMeasuredTicks == (xExpectedIdleTime - 1U)) ? 1U : 0U;
    TickType_t missing;
    TickType_t step;

    if (!s_tick.valid) {
        pm_idle_tick_epoch_init(xPortMeasuredTicks + pended);
        return xPortMeasuredTicks;
    }
    missing = pm_idle_tick_missing(xTaskGetTickCount() + s_tick.catchup, pended);
    step = (missing < (xExpectedIdleTime - 1U)) ? missing : (xExpectedIdleTime - 1U);
    s_tick.catchup += missing - step;
    ipro_pm_idle_ticks_corrected += (xPortMeasuredTicks > step) ? (xPortMeasuredTicks - step) : 0U;
    return step;
}

/* -------------------------------------------------------------------------
 * Console across PDS
 * -----------------------------------------------------------------------*/
#if defined(CONFIG_IPRO7)
#include "drv_glb.h"
#include "lp_uart.h"
#endif

/* UART0 on GPIO20/21 as the console (same sequence as the BLE controller's
 * post-PDS restore).  Weak: boards with another console override it. */
__attribute__((weak)) void ipro_pm_console_restore(void)
{
#if defined(CONFIG_IPRO7)
    GLB_GPIO_Cfg_Type uart_gpio = {
        .gpioPin = GLB_GPIO_PIN_20,
        .gpioFun = 7,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive = 0,
        .smtCtrl = 1,
    };

    GLB_GPIO_Init(&uart_gpio);
    uart_gpio.gpioPin = GLB_GPIO_PIN_21;
    GLB_GPIO_Init(&uart_gpio);
    GLB_UART_Fun_Sel(GLB_UART_SIG_8, GLB_UART_SIG_FUN_UART0_TXD);
    GLB_UART_Fun_Sel(GLB_UART_SIG_9, GLB_UART_SIG_FUN_UART0_RXD);
    writel((readl(UINT32_C(0x3000000c)) & ~UINT32_C(0x7f)) | 0x10U, UINT32_C(0x3000000c));
    writel(readl(UINT32_C(0x3008f030)) | (1U << 15), UINT32_C(0x3008f030));
    lp_uart_init(0, CONFIG_PLATFORM_UART_BAUDRATE);
#if defined(CONFIG_SHELL)
    {
        extern void shell_uart_reinit(void);
        shell_uart_reinit();
    }
#endif
#endif
}

/* -------------------------------------------------------------------------
 * Backoff after repeated aborted entries (a level interrupt that stays
 * pending, a sequencer that refuses): WFI for a while, doubling.
 * -----------------------------------------------------------------------*/
static void pm_idle_backoff_end(TimerHandle_t timer)
{
    (void)timer;
    pm_core_auto_max_set(IPRO_PM_MODE_PDS);
}

void pm_exec_idle_housekeeping(void)
{
    TickType_t owed;

#if defined(CONFIG_IPRO_PM_LP_WDT)
    if ((TickType_t)(xTaskGetTickCount() - s_wdt_fed) >= pdMS_TO_TICKS(4000U)) {
        s_wdt_fed = xTaskGetTickCount();
        aon_wdt_feed();
    }
#endif

    taskENTER_CRITICAL();
    owed = s_tick.catchup;
    s_tick.catchup = 0U;
    s_tick.catchup_busy = (owed != 0U);
    taskEXIT_CRITICAL();
    if (owed != 0U) {
        ipro_pm_idle_ticks_caught_up += owed;
        (void)xTaskCatchUpTicks(owed);
        s_tick.catchup_busy = false;
    }
    if (s_backoff_request) {
        s_backoff_request = false;
        if ((s_backoff_timer != NULL) &&
            (xTimerChangePeriod(s_backoff_timer, pdMS_TO_TICKS(s_backoff_ms), 0U) == pdPASS)) {
            pm_core_auto_max_set(IPRO_PM_MODE_WFI);
            s_backoff_ms = (s_backoff_ms >= (PM_IDLE_BACKOFF_MAX_MS / 2U)) ? PM_IDLE_BACKOFF_MAX_MS
                                                                          : (s_backoff_ms * 2U);
        }
    }
    pm_core_devices_resume_late_defer();
}

/* -------------------------------------------------------------------------
 * The transaction
 * -----------------------------------------------------------------------*/
/* AHB clock gates: the SoC reset of the PDS puts them back to their reset
 * value, leaving whatever the application ungated since boot (the 802.15.4
 * MAC, the security engine, DMA...) without a clock */
#define PM_CGEN_N 3U
static uint32_t s_cgen[PM_CGEN_N];

static void pm_idle_cgen_save(void)
{
    for (uint32_t i = 0U; i < PM_CGEN_N; i++) {
        s_cgen[i] = readl(GLB_BASE + GLB_CGEN_CFG0_OFFSET + (4U * i));
    }
}

static void pm_idle_cgen_restore(void)
{
    bool changed = false;

    for (uint32_t i = 0U; i < PM_CGEN_N; i++) {
        uint32_t now = readl(GLB_BASE + GLB_CGEN_CFG0_OFFSET + (4U * i));

        if (now != s_cgen[i]) {
            ipro_pm_idle_cgen_last[0] = i;
            ipro_pm_idle_cgen_last[1] = s_cgen[i];
            ipro_pm_idle_cgen_last[2] = now;
            writel(s_cgen[i], GLB_BASE + GLB_CGEN_CFG0_OFFSET + (4U * i));
            changed = true;
        }
    }
    if (changed) {
        ipro_pm_idle_cgen_restores++;
    }
}

static void pm_idle_timer_irqs_clean(void)
{
    /* a stale pended scheduler SWI or timer interrupt from before the PDS
     * must not fire into the half-restored kernel; the port re-arms the tick
     * from a clean edge on its skip path */
    SysTimer_ClearSWIRQ();
    ECLIC_ClearPendingIRQ(SysTimer_IRQn);
    ECLIC_ClearPendingIRQ(SysTimerSW_IRQn);
}

pm_offer_t pm_exec_idle_run(TickType_t selected_ticks, uint32_t part_deadline_hus)
{
    uint64_t mtime0;
    uint64_t cmp0;
    uint64_t back_us;
    pm_clkpair_t before;
    hal_lp_cfg_t cfg;
    hal_lp_result_t res;
    uint32_t saved_policy;
    uint32_t sleep_us;
    uint32_t fallback_us;
    bool abort_on_irq;
    uint64_t target;
    pm_txn_t *txn;
    int rc;

    if (!s_tick.valid) {
        pm_idle_tick_epoch_init(0U);
    }
    mtime0 = SysTimer_GetLoadValue();
    cmp0 = SysTimer_GetCompareValue();
    back_us = (cmp0 > mtime0) ? ((cmp0 - mtime0) / (configSYSTICK_CLOCK_HZ / 1000000UL)) : 0U;
    if ((part_deadline_hus != PM_DEADLINE_NONE) && ((part_deadline_hus / 2U) < back_us)) {
        back_us = part_deadline_hus / 2U;
    }
    if ((ipro_pm_idle_min_awake_us != 0U) && (s_last_wake_mtime != 0U) &&
        ((mtime0 - s_last_wake_mtime) < ((uint64_t)ipro_pm_idle_min_awake_us * (configSYSTICK_CLOCK_HZ / 1000000UL)))) {
        ipro_pm_idle_too_soon++;
        return PM_OFFER_REJECTED;
    }
    sleep_us = pm_exec_idle_plan(back_us, CONFIG_IPRO_PM_IDLE_PDS_WAKE_US, CONFIG_IPRO_PM_IDLE_PDS_GUARD_US,
                                 pm_lpclk_bound_ppm());
    if (sleep_us < CONFIG_IPRO_PM_IDLE_PDS_MIN_SLEEP_US) {
        ipro_pm_idle_too_short++;
        return PM_OFFER_REJECTED;
    }
    if (!pm_core_vetoes_allow(sleep_us)) {
        ipro_pm_idle_vetoed++;
        pm_core_pds_rejected();
        return PM_OFFER_REJECTED;
    }
    txn = pm_core_txn_begin(PM_EXEC_IDLE, sleep_us);
    if (!pm_core_devices_suspend()) {
        /* the devices already suspended were resumed; resume_late follows
         * from the idle hook */
        ipro_pm_idle_dev_vetoed++;
        pm_core_pds_rejected();
        return PM_OFFER_REJECTED;
    }
    pm_core_notify(IPRO_PM_EVT_PDS_ENTER, sleep_us);

    saved_policy = pm_pds_ble_ctlr_domain_policy_get();
    txn->domain_policy = ipro_pm_idle_domain_policy;
    pm_pds_ble_ctlr_domain_policy_set(txn->domain_policy);

    pm_idle_cgen_save();
    pm_lpclk_pair_now(&before);
    pm_lpclk_span_end(&before);
    target = (before.rtc + pm_lpclk_us_to_ticks(sleep_us)) & ((UINT64_C(1) << 45) - UINT64_C(1));
    pm_pds_rtc_target_override_set(target);

    memset(&cfg, 0, sizeof(cfg));
    memset(&res, 0, sizeof(res));
    cfg.rtc_wakeup_en = 1U;
    cfg.rtc_sleep_us = sleep_us;
    cfg.ocram_retention = 1U;
#if defined(CONFIG_USE_PSRAM)
    cfg.psram_retention = 1U;
#endif
    abort_on_irq = true;
    /* a PDS that never engages ends on this compare: not later than the
     * planned wake (the CSL window, the RTOS deadline) */
    fallback_us = ipro_pm_idle_entry_fallback_us;
    if ((fallback_us == 0U) || (fallback_us > sleep_us)) {
        fallback_us = sleep_us;
    }
#if defined(CONFIG_IPRO_PM_FAULT_INJECT)
    if (ipro_pm_inject_abort_count != 0U) {
        ipro_pm_inject_abort_count--;
        if (ipro_pm_inject_abort_mode == 3U) {
            /* PDS refuses to arm: only the bounded fallback ends the WFI */
            extern volatile uint32_t hal_lp_test_entry_ticks;

            hal_lp_test_entry_ticks = 10U;
        } else {
            if (ipro_pm_inject_abort_mode == 2U) {
                abort_on_irq = false;
            }
            SysTimer_SetSWIRQ();
        }
    }
#endif
#if defined(CONFIG_IPRO_PM_FAULT_INJECT) && defined(CONFIG_IPRO7)
    if (ipro_pm_idle_tx_park != 0U) {
        GLB_GPIO_Cfg_Type tx = {
            .gpioPin = GLB_GPIO_PIN_20,
            .gpioFun = GPIO_FUN_GPIO,
            .gpioMode = GPIO_MODE_OUTPUT,
            .pullType = GPIO_PULL_UP,
            .drive = 0,
            .smtCtrl = 1,
        };

        GLB_GPIO_Write(GLB_GPIO_PIN_20, 1U);
        GLB_GPIO_Init(&tx);
        GLB_GPIO_Write(GLB_GPIO_PIN_20, 1U);
    }
#endif
    PM_BC(1U, target);
    hal_lp_set_entry_guard(abort_on_irq, fallback_us);
    rc = hal_lp_enter(&cfg, &res);
    PM_BC(2U, rc);

    pm_pds_ble_ctlr_domain_policy_set(saved_policy);

    if (rc < 0) {
        /* nothing was powered down: MTIME ran on, the awake span goes on */
        pm_pds_rtc_target_override_set(0U);
        pm_lpclk_span_start(&before);
        pm_idle_tick_sync(selected_ticks);
        pm_core_devices_resume_early();
        pm_core_pds_aborted(rc);
        ipro_pm_idle_aborted++;
        if (++s_consecutive_aborts >= PM_IDLE_ABORT_LIMIT) {
            s_consecutive_aborts = 0U;
            s_backoff_request = true;
        }
        return PM_OFFER_EXECUTED;
    }

    if (((uint32_t)rc & HAL_LP_WAKEUP_RTC) != 0U) {
        ipro_pm_idle_wake_rtc++;
    }
    if (((uint32_t)rc & HAL_LP_WAKEUP_GPIO) != 0U) {
        ipro_pm_idle_wake_gpio++;
    }
    if (((uint32_t)rc & (HAL_LP_WAKEUP_RTC | HAL_LP_WAKEUP_GPIO)) == 0U) {
        ipro_pm_idle_wake_other++;
    }
    txn->elapsed_us = pm_lpclk_rebase(&before);
    s_last_wake_mtime = SysTimer_GetLoadValue();
    txn->wake_reason = (uint32_t)rc;
    txn->slept = true;
    pm_idle_cgen_restore();
    pm_idle_timer_irqs_clean();
    pm_idle_tick_sync(selected_ticks);
    ipro_pm_console_restore();
    PM_BC(3U, txn->elapsed_us);
    pm_core_devices_resume_early();
    PM_BC(4U, txn->generation);
    pm_core_account(IPRO_PM_MODE_PDS, txn->elapsed_us);
    pm_core_notify(IPRO_PM_EVT_PDS_EXIT, txn->elapsed_us);
    ipro_pm_idle_executed++;
    ipro_pm_idle_last_sleep_us = sleep_us;
    ipro_pm_idle_last_elapsed_us = txn->elapsed_us;
    if (((uint32_t)rc & HAL_LP_WAKEUP_RTC) != 0U) {
        int32_t late = (int32_t)txn->elapsed_us - (int32_t)sleep_us;

        if (late > ipro_pm_idle_late_us_max) {
            ipro_pm_idle_late_us_max = late;
        }
    }
    s_consecutive_aborts = 0U;
    s_backoff_ms = PM_IDLE_BACKOFF_MIN_MS;
    return PM_OFFER_EXECUTED;
}

void pm_exec_idle_init(void)
{
#if defined(CONFIG_IPRO_PM_LP_WDT)
    pm_lp_wdt_boot_report();
#endif
    s_tick.valid = false;
    (void)hal_lp_init();
    /* the executor always gives an absolute RTC target; exact conversion for
     * anything that still uses the relative path, keeping a wake delay the
     * BLE controller measured (ipro_pm may be initialised after it) */
    {
        uint32_t policy = 0U;
        uint32_t delay_us = 0U;

        hal_lp_get_timing_policy(&policy, &delay_us);
        hal_lp_set_timing_policy(HAL_LP_TIMING_POLICY_EXACT_US_TO_RTC |
                                     (policy & HAL_LP_TIMING_POLICY_PDS_WAKE_DELAY_VALID),
                                 delay_us);
    }
    if (s_backoff_timer == NULL) {
        s_backoff_timer = xTimerCreate("pm_idle_bo", pdMS_TO_TICKS(PM_IDLE_BACKOFF_MIN_MS), pdFALSE, NULL,
                                       pm_idle_backoff_end);
    }
    if (hal_lp_wakeup_hook_count() != 0U) {
        /* a global HAL wake hook (e.g. the legacy Thread lp_sleep) would run
         * after every PDS this executor enters */
        printf("[ipro_pm] warning: %lu hal_lp wakeup hook(s) registered\r\n",
               (unsigned long)hal_lp_wakeup_hook_count());
    }
}
