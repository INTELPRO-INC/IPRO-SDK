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
 * @file pm_shell.c
 * @brief `pm` shell command: status, statistics and policy.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include "ipro_pm.h"
#include "pm_internal.h"
#include "shell.h"
#if defined(CONFIG_IPRO7)
#include "ipro7.h"
#endif

static const char *const s_mode_names[IPRO_PM_MODE_COUNT] = { "active", "wfi", "native", "pds" };

static int pm_mode_parse(const char *s, ipro_pm_mode_t *out)
{
    for (uint32_t m = 0U; m < IPRO_PM_MODE_COUNT; m++) {
        if (strcmp(s, s_mode_names[m]) == 0) {
            *out = (ipro_pm_mode_t)m;
            return 0;
        }
    }
    return -1;
}

static void pm_print_stats(void)
{
    ipro_pm_stats_t st;
    uint64_t win;

    ipro_pm_stats_get(&st);
    win = (st.window_us != 0U) ? st.window_us : 1U;
    printf("pm stats window %lu ms | est avg %lu uA | auto max %s\r\n", (unsigned long)(st.window_us / 1000U),
           (unsigned long)st.est_avg_ua, s_mode_names[st.auto_max]);
    for (uint32_t m = 0U; m < IPRO_PM_MODE_COUNT; m++) {
        printf("  %-6s %10lu ms %3lu.%01lu%% entries %lu\r\n", s_mode_names[m],
               (unsigned long)(st.residency_us[m] / 1000U),
               (unsigned long)((st.residency_us[m] * 100U) / win),
               (unsigned long)(((st.residency_us[m] * 1000U) / win) % 10U), (unsigned long)st.entries[m]);
    }
    printf("  pds failures %lu rejects %lu aborts %lu demotions %lu\r\n", (unsigned long)st.pds_failures,
           (unsigned long)st.pds_rejects, (unsigned long)st.pds_aborts, (unsigned long)st.demotions);
}

static const char *const s_part_names[] = { "absent", "ready", "executor", "retry", "busy" };

static void pm_print_participants(void)
{
    for (uint32_t i = 0U; i < pm_arbiter_count(); i++) {
        const pm_participant_t *p = pm_arbiter_get(i);
        pm_part_state_t st;
        uint32_t dl;

        if (p == NULL) {
            continue;
        }
        /* the same view the idle task gets, IRQs masked */
        taskENTER_CRITICAL();
        st = p->pds_state();
        dl = (p->pds_deadline_hus != NULL) ? p->pds_deadline_hus() : PM_DEADLINE_NONE;
        taskEXIT_CRITICAL();
        if (dl == PM_DEADLINE_NONE) {
            printf("  radio %-8s %-8s deadline none\r\n", p->name ? p->name : "?",
                   ((uint32_t)st < 5U) ? s_part_names[st] : "?");
        } else {
            printf("  radio %-8s %-8s deadline %lu us\r\n", p->name ? p->name : "?",
                   ((uint32_t)st < 5U) ? s_part_names[st] : "?", (unsigned long)(dl / 2U));
        }
    }
}

static void pm_print_status(void)
{
    pm_reason_view_t reasons[16];
    uint32_t n = pm_core_reasons_dump(reasons, 16U);

    printf("pm policy %s | allowed now %s | auto max %s\r\n", s_mode_names[ipro_pm_policy_get()],
           s_mode_names[ipro_pm_allowed_mode()], s_mode_names[pm_core_auto_max_get()]);
    pm_print_participants();
    for (uint32_t i = 0U; i < n; i++) {
        printf("  reason %-12s max %-6s holds %u%s\r\n", reasons[i].name ? reasons[i].name : "?",
               s_mode_names[reasons[i].max_mode], (unsigned)reasons[i].count, reasons[i].timed ? " timed" : "");
    }
}

#if defined(CONFIG_IPRO_PM_IDLE_PDS)
#include "hal_lp.h"

extern volatile uint32_t ipro_pm_idle_executed;
extern volatile uint32_t ipro_pm_idle_aborted;
extern volatile uint32_t ipro_pm_idle_too_short;
extern volatile uint32_t ipro_pm_idle_vetoed;
extern volatile uint32_t ipro_pm_idle_dev_vetoed;
extern volatile uint32_t ipro_pm_idle_wake_rtc;
extern volatile uint32_t ipro_pm_idle_wake_gpio;
extern volatile uint32_t ipro_pm_idle_wake_other;
extern volatile uint32_t ipro_pm_idle_last_sleep_us;
extern volatile uint32_t ipro_pm_idle_last_elapsed_us;
extern volatile int32_t ipro_pm_idle_late_us_max;
extern volatile uint32_t ipro_pm_idle_ticks_stepped;
extern volatile uint32_t ipro_pm_idle_ticks_caught_up;
extern volatile uint32_t ipro_pm_idle_ticks_corrected;
extern volatile uint32_t ipro_pm_idle_tick_rebases;
extern volatile uint32_t ipro_pm_idle_cgen_restores;
extern volatile uint32_t ipro_pm_idle_cgen_last[3];

static void pm_print_exec(void)
{
    printf("pm idle-exec executed %lu aborted %lu too-short %lu vetoed %lu dev-vetoed %lu\r\n",
           (unsigned long)ipro_pm_idle_executed, (unsigned long)ipro_pm_idle_aborted,
           (unsigned long)ipro_pm_idle_too_short, (unsigned long)ipro_pm_idle_vetoed,
           (unsigned long)ipro_pm_idle_dev_vetoed);
    printf("  last sleep %lu us elapsed %lu us | late max %ld us | ticks stepped %lu caught-up %lu"
           " | hal aborts %lu last-irq %lu\r\n",
           (unsigned long)ipro_pm_idle_last_sleep_us, (unsigned long)ipro_pm_idle_last_elapsed_us,
           (long)ipro_pm_idle_late_us_max, (unsigned long)ipro_pm_idle_ticks_stepped,
           (unsigned long)ipro_pm_idle_ticks_caught_up, (unsigned long)hal_lp_entry_aborts,
           (unsigned long)hal_lp_last_abort_irq);
    printf("  tickless WFI ticks corrected %lu | tick epoch rebases %lu\r\n",
           (unsigned long)ipro_pm_idle_ticks_corrected, (unsigned long)ipro_pm_idle_tick_rebases);
    printf("  wake rtc %lu gpio %lu other %lu | clock gates restored %lu (last CGEN_CFG%lu %08lx -> %08lx)\r\n",
           (unsigned long)ipro_pm_idle_wake_rtc, (unsigned long)ipro_pm_idle_wake_gpio,
           (unsigned long)ipro_pm_idle_wake_other, (unsigned long)ipro_pm_idle_cgen_restores,
           (unsigned long)ipro_pm_idle_cgen_last[0], (unsigned long)ipro_pm_idle_cgen_last[1],
           (unsigned long)ipro_pm_idle_cgen_last[2]);
}
#endif

#if defined(PM_HAVE_TIMEBASE)
#include "core_feature_timer.h"
#include "hal_pm.h"

extern volatile uint64_t ipro_pm_lpclk_last_mtime_before;
extern volatile uint64_t ipro_pm_lpclk_last_mtime_woke;
extern volatile uint64_t ipro_pm_lpclk_last_mtime_target;
extern volatile uint32_t ipro_pm_lpclk_last_mtimectl_woke;
extern volatile uint32_t ipro_pm_lpclk_rebases;
extern volatile uint32_t ipro_pm_lpclk_windows;
extern volatile int32_t ipro_pm_lpclk_last_window_ppm;
extern volatile uint32_t ipro_pm_lpclk_edge_timeouts;

static void pm_print_time(void)
{
    extern volatile int32_t ipro_pm_lpclk_rc32k_cal;
    uint8_t use_xtal = 0U;
    uint8_t forced_rc = 0U;

    pm_pds_get_32k_src(&use_xtal, &forced_rc);
    printf("pm 32k: %s (legacy forced rc %u) boot cal %ld\r\n", use_xtal ? "xtal32k" : "rc32k/other",
           (unsigned)forced_rc, (long)ipro_pm_lpclk_rc32k_cal);
    printf("pm lpclk %ld ppm bound %lu | windows %lu last %ld ppm | edge timeouts %lu\r\n",
           (long)pm_lpclk_ppm(), (unsigned long)pm_lpclk_bound_ppm(), (unsigned long)ipro_pm_lpclk_windows,
           (long)ipro_pm_lpclk_last_window_ppm, (unsigned long)ipro_pm_lpclk_edge_timeouts);
    printf("  rebases %lu | last: before %llu woke %llu (ctl %lx) target %llu counts\r\n",
           (unsigned long)ipro_pm_lpclk_rebases, (unsigned long long)ipro_pm_lpclk_last_mtime_before,
           (unsigned long long)ipro_pm_lpclk_last_mtime_woke, (unsigned long)ipro_pm_lpclk_last_mtimectl_woke,
           (unsigned long long)ipro_pm_lpclk_last_mtime_target);
    printf("  now: mtime %llu us | rtc uptime %llu us | ticks %lu\r\n",
           (unsigned long long)(SysTimer_GetLoadValue() / (configSYSTICK_CLOCK_HZ / 1000000UL)),
           (unsigned long long)ipro_pm_uptime_us(), (unsigned long)xTaskGetTickCount());
#if defined(IPRO_PM_HAS_BLE)
    {
        extern volatile uint32_t ipro_pm_ble_txn_rebases;
        extern volatile uint32_t ipro_pm_ble_txn_no_pds;

        printf("  ble pds: mtime rebases %lu | entries without pds %lu\r\n",
               (unsigned long)ipro_pm_ble_txn_rebases, (unsigned long)ipro_pm_ble_txn_no_pds);
    }
#endif
}
#endif

/* RAM canary across PDS (bench): a heap block filled with a pattern. */
static uint32_t *s_canary;
static uint32_t s_canary_words;

static void pm_canary(int argc, char **argv)
{
    if ((argc >= 3) && (strcmp(argv[2], "check") != 0)) {
        uint32_t kb = (uint32_t)strtoul(argv[2], NULL, 0);

        if ((s_canary == NULL) && (kb != 0U)) {
            s_canary = (uint32_t *)pvPortMalloc(kb * 1024U);
            s_canary_words = (s_canary != NULL) ? (kb * 256U) : 0U;
        }
        for (uint32_t i = 0U; i < s_canary_words; i++) {
            s_canary[i] = 0xA5C30000U ^ (i * 2654435761U);
        }
        printf("pm canary %lu words at %p\r\n", (unsigned long)s_canary_words, (void *)s_canary);
        return;
    }
    {
        uint32_t bad = 0U;
        uint32_t first = 0U;

        for (uint32_t i = 0U; i < s_canary_words; i++) {
            if (s_canary[i] != (0xA5C30000U ^ (i * 2654435761U))) {
                if (bad == 0U) {
                    first = i;
                }
                bad++;
            }
        }
        printf("pm canary %lu words, %lu bad (first %lu)\r\n", (unsigned long)s_canary_words,
               (unsigned long)bad, (unsigned long)first);
    }
}

/* `pm policy <mode> <ms>`: that ceiling for a while, then back (bench: a
 * console that cannot reach a sleeping part comes back by itself). */
static TimerHandle_t s_policy_timer;
static ipro_pm_mode_t s_policy_restore;

static void pm_policy_timer_cb(TimerHandle_t t)
{
    (void)t;
    ipro_pm_policy_set(s_policy_restore);
    printf("pm policy %s (timed)\r\n", s_mode_names[s_policy_restore]);
}

static void pm_policy_timed(ipro_pm_mode_t mode, uint32_t ms)
{
    if (s_policy_timer == NULL) {
        s_policy_timer = xTimerCreate("pm_pol", pdMS_TO_TICKS(ms), pdFALSE, NULL, pm_policy_timer_cb);
    }
    if ((s_policy_timer == NULL) || (xTimerChangePeriod(s_policy_timer, pdMS_TO_TICKS(ms), 0U) != pdPASS)) {
        printf("pm policy: no timer\r\n");
        return;
    }
    s_policy_restore = ipro_pm_policy_get();
    ipro_pm_policy_set(mode);
    printf("pm policy %s for %lu ms\r\n", s_mode_names[mode], (unsigned long)ms);
}

#if defined(CONFIG_IPRO7)
#include "hal_pm.h"
#include "drv_pds.h"
#include "pds_reg.h"
#include "drv_common.h"

/* `pm gpiostat <pin> <ms>`: arm the pin as a PDS wake source and sample the
 * PDS domain's view of it (bench: does PDS see the pad at all?). */
static void pm_gpiostat(int argc, char **argv)
{
    uint32_t pin = (argc >= 3) ? (uint32_t)strtoul(argv[2], NULL, 0) : 21U;
    uint32_t ms = (argc >= 4) ? (uint32_t)strtoul(argv[3], NULL, 0) : 3000U;
    uint32_t lows = 0U;
    uint32_t samples = 0U;
    uint32_t ints = 0U;
    TickType_t end;

    if (pm_pds_gpio_wakeup_set((uint8_t)pin, PDS_GPIO_INT_ASYNC_LOW_LEVEL) != 0) {
        printf("pm gpiostat: pin %lu has no PDS group\r\n", (unsigned long)pin);
        return;
    }
    pm_pds_gpio_wakeup_dump();
    end = xTaskGetTickCount() + pdMS_TO_TICKS(ms);
    while ((TickType_t)(end - xTaskGetTickCount()) < (portMAX_DELAY / 2U)) {
        uint32_t stat = readl(PDS_BASE + PDS_GPIO_STAT_OFFSET);

        samples++;
        if (((stat >> pin) & 1U) == 0U) {
            lows++;
        }
        if (PDS_Get_GPIO_Pad_IntStatus((PDS_GPIO_Type)pin)) {
            ints++;
        }
    }
    pm_pds_gpio_wakeup_unset((uint8_t)pin);
    printf("pm gpiostat pin %lu: %lu/%lu samples low, int seen %lu, PDS_INT %08lx\r\n", (unsigned long)pin,
           (unsigned long)lows, (unsigned long)samples, (unsigned long)ints,
           (unsigned long)readl(PDS_BASE + PDS_INT_OFFSET));
}
#endif

#if defined(CONFIG_IPRO_PM_FAULT_INJECT)
static volatile uint32_t s_xtimer_fires;

static void pm_xtimer_cb(TimerHandle_t t)
{
    (void)t;
    s_xtimer_fires++;
}
#endif

static void cmd_pm(int argc, char **argv)
{
    ipro_pm_mode_t mode;

    if ((argc < 2) || (strcmp(argv[1], "status") == 0)) {
        pm_print_status();
    } else if (strcmp(argv[1], "stats") == 0) {
        pm_print_stats();
#if defined(CONFIG_IPRO_PM_IDLE_PDS)
        pm_print_exec();
#endif
#if defined(CONFIG_IPRO_PM_IDLE_PDS)
    } else if (strcmp(argv[1], "exec") == 0) {
        pm_print_exec();
#endif
#if defined(PM_HAVE_TIMEBASE)
    } else if (strcmp(argv[1], "time") == 0) {
        pm_print_time();
#endif
    } else if (strcmp(argv[1], "port") == 0) {
        pm_port_dump();
#if defined(IPRO_PM_HAS_BLE)
    } else if (strcmp(argv[1], "ble") == 0) {
        pm_ble_dump();
#endif
    } else if (strcmp(argv[1], "canary") == 0) {
        pm_canary(argc, argv);
#if defined(CONFIG_IPRO_PM_IDLE_PDS) && defined(CONFIG_IPRO_PM_FAULT_INJECT)
    } else if ((strcmp(argv[1], "xpolicy") == 0) || (strcmp(argv[1], "xfallback") == 0) ||
               (strcmp(argv[1], "xedge") == 0) || (strcmp(argv[1], "xmingap") == 0)) {
        extern volatile uint32_t ipro_pm_idle_domain_policy;
        extern volatile uint32_t ipro_pm_idle_entry_fallback_us;
        extern volatile uint32_t ipro_pm_lpclk_edge_sync;
        extern volatile uint32_t ipro_pm_idle_min_awake_us;
        volatile uint32_t *v = (argv[1][1] == 'p')   ? &ipro_pm_idle_domain_policy
                               : (argv[1][1] == 'e') ? &ipro_pm_lpclk_edge_sync
                               : (argv[1][1] == 'm') ? &ipro_pm_idle_min_awake_us
                                                     : &ipro_pm_idle_entry_fallback_us;

        if (argc >= 3) {
            *v = (uint32_t)strtoul(argv[2], NULL, 0);
        }
        printf("pm %s 0x%lx (%lu)\r\n", argv[1], (unsigned long)*v, (unsigned long)*v);
#endif
#if defined(CONFIG_IPRO_PM_FAULT_INJECT) && defined(CONFIG_IPRO_PM_IDLE_PDS)
    } else if (strcmp(argv[1], "xnoradio") == 0) {
        extern volatile uint32_t ipro_pm_idle_no_radio;

        if (argc >= 3) {
            ipro_pm_idle_no_radio = (uint32_t)strtoul(argv[2], NULL, 0);
        }
        printf("pm xnoradio %lu\r\n", (unsigned long)ipro_pm_idle_no_radio);
    } else if (strcmp(argv[1], "xtxpark") == 0) {
        extern volatile uint32_t ipro_pm_idle_tx_park;

        if (argc >= 3) {
            ipro_pm_idle_tx_park = (uint32_t)strtoul(argv[2], NULL, 0);
        }
        printf("pm xtxpark %lu\r\n", (unsigned long)ipro_pm_idle_tx_park);
    } else if (strcmp(argv[1], "xtimer") == 0) {
        /* a periodic RTOS deadline: PDS cycles at a fixed rhythm (bench) */
        static TimerHandle_t t;
        uint32_t ms = (argc >= 3) ? (uint32_t)strtoul(argv[2], NULL, 0) : 0U;

        if (t == NULL) {
            t = xTimerCreate("pm_xt", pdMS_TO_TICKS(1000), pdTRUE, NULL, pm_xtimer_cb);
        }
        if (t != NULL) {
            if (ms == 0U) {
                (void)xTimerStop(t, 0U);
            } else {
                (void)xTimerChangePeriod(t, pdMS_TO_TICKS(ms), 0U);
            }
        }
        printf("pm xtimer %lu ms\r\n", (unsigned long)ms);
#endif
#if defined(CONFIG_IPRO_PM_FAULT_INJECT)
    } else if (strcmp(argv[1], "wakes") == 0) {
        extern volatile uint32_t ipro_pm_wake_irq_hist[];

        printf("pm wakes (tickless WFI ended by irq):");
        for (uint32_t i = 0U; i <= (uint32_t)__ECLIC_INTNUM; i++) {
            if (ipro_pm_wake_irq_hist[i] != 0U) {
                if (i == 0U) {
                    printf(" none %lu", (unsigned long)ipro_pm_wake_irq_hist[i]);
                } else {
                    printf(" %lu:%lu", (unsigned long)(i - 1U), (unsigned long)ipro_pm_wake_irq_hist[i]);
                }
            }
            if ((argc >= 3) && (argv[2][0] == 'r')) {
                ipro_pm_wake_irq_hist[i] = 0U;
            }
        }
        printf("\r\n");
#endif
#if defined(CONFIG_IPRO_PM_FAULT_INJECT) && defined(CONFIG_IPRO_PM_IDLE_PDS)
    } else if ((strcmp(argv[1], "inject") == 0) && (argc >= 3)) {
        extern volatile uint32_t ipro_pm_inject_abort_count;
        extern volatile uint32_t ipro_pm_inject_abort_mode;

        ipro_pm_inject_abort_mode = (strcmp(argv[2], "abort3") == 0)   ? 3U
                                    : (strcmp(argv[2], "abort2") == 0) ? 2U
                                                                       : 1U;
        ipro_pm_inject_abort_count = (argc >= 4) ? (uint32_t)strtoul(argv[3], NULL, 0) : 1U;
        printf("pm inject abort mode %lu x%lu\r\n", (unsigned long)ipro_pm_inject_abort_mode,
               (unsigned long)ipro_pm_inject_abort_count);
#endif
#if defined(CONFIG_IPRO7)
    } else if (strcmp(argv[1], "gpiostat") == 0) {
        pm_gpiostat(argc, argv);
#endif
    } else if (strcmp(argv[1], "reset") == 0) {
        ipro_pm_stats_reset();
        printf("pm stats reset\r\n");
    } else if ((strcmp(argv[1], "policy") == 0) && (argc >= 4) && (pm_mode_parse(argv[2], &mode) == 0)) {
        pm_policy_timed(mode, (uint32_t)strtoul(argv[3], NULL, 0));
    } else if ((strcmp(argv[1], "policy") == 0) && (argc >= 3) && (pm_mode_parse(argv[2], &mode) == 0)) {
        ipro_pm_policy_set(mode);
        printf("pm policy %s\r\n", s_mode_names[mode]);
    } else if ((strcmp(argv[1], "auto") == 0) && (argc >= 3) && (pm_mode_parse(argv[2], &mode) == 0)) {
        pm_core_auto_max_set(mode);
        printf("pm auto max %s\r\n", s_mode_names[mode]);
    } else {
        printf("usage: pm [status|stats|exec|time|port|ble|reset|policy <mode> [ms]|auto <mode>|"
               "canary <kb>|canary check]\r\n");
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_pm, pm, power management status and policy);

void pm_shell_init(void)
{
    /* the command registers itself through the linker section */
}
