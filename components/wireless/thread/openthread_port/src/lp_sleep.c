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
 * @file lp_sleep.c
 * @brief PDS31 tickless-idle integration — openthread_port library copy.
 *
 * Originally lived in apps/networking/thread/ipro_thread_mtd/. Moved into
 * openthread_port on 2026-05-19 so the entire low-power solution (engine
 * + wake/sleep) ships as one prebuilt library; apps just enable
 * CONFIG_THREAD_LP_SLEEP and call lp_sleep_init() before the scheduler.
 *
 * Source-of-truth for the M154 resync stays on the OT side via
 * `ieee802154_hal_lp_resync()` (re-runs lmac154_init + replays the cached
 * hal_state.config), so PDS doesn't have to know which M154 registers
 * exist.
 *
 *   sed task -> vTaskDelay(N ticks) -> task blocks
 *   FreeRTOS idle -> vApplicationSleep(N) -> lp_pds_sleep_impl
 *                 -> ieee802154_hal_is_tx_pending() gate -> hal_lp_enter
 *   PDS31 sleep
 *   warm wake -> hal_lp_main (TCM) -> sys clock to XTAL 32M, PLL/DLL off,
 *                                     flash on XCLK, hal_lp_xip_recovery
 *             -> lp_wakeup_hook -> GLB SWRST M154 + ieee802154_hal_lp_resync
 *                                  (+ rf_optimize_by_channel + tx power)
 *   hal_lp_enter returns -> vApplicationSleep returns -> idle task wakes
 *
 * Pre-attach gate (lp_sleep_enable_pds) keeps PDS31 disabled until the
 * SED fast-poll engine reports `is_ready()`. Entering PDS during attach
 * kills the mid-handshake radio state and the parent stops responding;
 * the engine drives the gate automatically — apps no longer need to.
 *
 * TX-pending gate inside lp_pds_sleep_impl is mandatory: OT's
 * otPlatRadioTransmit returns immediately, OT-MAC yields while waiting
 * for the radio callback, FreeRTOS hits tickless-idle in that window
 * and PDS31 will happily power-gate M154 mid-transmit, killing the
 * in-flight TX. The HAL exposes ieee802154_hal_is_tx_pending() — a
 * sticky flag set at triggerTxDelay and cleared in every TX terminal
 * callback — for exactly this case.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "lp_sleep.h"
#include <generated/autoconf.h>

#ifdef CONFIG_THREAD_LP_SLEEP

#include "compiler/common.h"
#include "core_feature_base.h"
#include "core_feature_cache.h" /* MInvalICache */
#include "drv_glb.h"
#include "hal_lp.h"
#include "hal_pm.h" /* pm_pds_set_misc_keep */
#include "ipro7.h"  /* GLB_AHB_CLOCK_M154, GLB_AHB_MCU_SW_M154 */
#include "ipro_log.h"
#include "lp_uart.h" /* lp_uart_puts — raw UART, survives post-PDS (debug) */
#include "system_soc.h" /* CPU_Interrupt_Pending_Clear */

#define LOG_TAG "LP_SLEEP"

/* ------------------------------------------------------------------------ *
 * Timing parameters
 * ------------------------------------------------------------------------ */

/* Absolute floor: below this, PDS overhead (~500 µs) eats the savings. */
#define LP_MIN_SLEEP_US 5000
/* Ceiling — keeps tick math inside uint32_t. */
#define LP_MAX_SLEEP_US 10000000
/* Empirical PDS entry/exit overhead margin. */
#define PDS_OVERHEAD_US 500

/* Config-driven gate: if FreeRTOS expects less than this idle window,
 * fall back to plain WFI. SED's poll period is 1000 ms, default gate
 * is 800 ms so only the long inter-poll idle takes the PDS path. */
#define LP_GATE_US ((uint32_t)CONFIG_THREAD_LP_MIN_SLEEP_MS * 1000UL)

/* Continuous-quiescence holdoff. Even with the idle_req && attached gate ON, do
 * NOT enter PDS until CHIP has been quiet this long. After a downlink command
 * burst CHIP briefly reports idle (1 s poll) while MRP retransmissions
 * (AT=4000 ms) + the exchange are still pending; PDS-ing into that window
 * power-gates the M154/radio mid-exchange and corrupts in-flight state — the
 * "downlink during sleep -> wake -> process -> crash" signature. 5 s clears the
 * MRP window + margin. Pure-idle PDS is unaffected: with no CHIP activity,
 * lp_sleep_ms_since_active() grows past the holdoff and PDS fires on the steady
 * ~1 s poll cadence (as the 115-clean-cycle idle runs already showed). */
#ifndef PDS_QUIESCE_HOLDOFF_MS
#if defined(CONFIG_THREAD_LP_QUIESCE_HOLDOFF_MS)
/* Apps without CHIP's MRP exchanges (plain CoAP SEDs) can shorten it. */
#define PDS_QUIESCE_HOLDOFF_MS ((uint32_t)CONFIG_THREAD_LP_QUIESCE_HOLDOFF_MS)
#else
#define PDS_QUIESCE_HOLDOFF_MS 5000u
#endif
#endif

/* ------------------------------------------------------------------------ *
 * Retention stack
 *
 * The naked vApplicationSleep trampoline switches sp here before calling
 * lp_pds_sleep_impl so PDS entry/exit doesn't depend on the caller's task
 * stack (which for this app lives in PSRAM). Lives in OCRAM (.ocram_bss
 * → output section .bss_ocram, retained across PDS31 by HW default).
 * ------------------------------------------------------------------------ */
/* Retention stack for lp_pds_sleep_impl AND the wake hook.
 *
 * Sized at 8 KB because the wake path is heavy in this app:
 * `ieee802154_hal_lp_resync()` re-runs `lmac154_init` which has deep
 * nested call frames (RF analog calibration tables, FPT slot rebuild,
 * register replay), plus `rf_optimize_by_channel` and the UART
 * re-init. Cumulative usage is ~3–4 KB on a quiet path and can spike
 * higher when lmac154 needs to retry calibration.
 *
 * The reference `ipro_ble_lowpower_test` runs fine on 2 KB because its
 * wake path only does `ble_controller_sleep_restore` (compact, mostly
 * register copies). We need more margin — measured 2026-05-19 when
 * a 4 KB stack overflowed with `misc_keep=false`, leaving a garbled
 * task name in the FreeRTOS overflow hook (TCB corruption).
 *
 * Living in OCRAM .ocram_bss.
 *
 * Size history: was 8K; trimmed 8K->5K (2026-06-01) for the no-PSRAM PDS
 * experiment (to free retained OCRAM). RESTORED to 8K (2026-06-02): 5 KB
 * covers only the QUIET poll-wake path (~4 KB), but a DOWNLINK-processing wake
 * (a buffered InvokeCommandRequest/ReportData retrieved on the poll -> radio
 * RX ISR + lmac154/OT frame handling nests on this stack on top of the wake-
 * restore) overflows 5 KB. Crash signature: SP ran ~600 B below the 5 KB base
 * (measured ~5.7 KB used), clobbering the adjacent .ocram_bss (s_sched/
 * s_m154_snapshot) + the return frame -> garbage ret -> PC=0x8 illegal-instr
 * on warm wake (intermittent, downlink-correlated). 8 K restores ~2.3 KB
 * margin over the observed peak. The PSRAM build has ample OCRAM (48/240 KB),
 * so the no-PSRAM-era trim no longer applies. */
#define PDS_STACK_SIZE 8192
static uint8_t __attribute__((aligned(16), section(".ocram_bss"),
                              used)) s_pds_stack[PDS_STACK_SIZE];

/* ------------------------------------------------------------------------ *
 * Module state
 *
 * s_pds_sleep_ticks is shared with the naked vApplicationSleep — the
 * naked stub can't use a C argument so the tick count goes through
 * this global instead.
 * ------------------------------------------------------------------------ */
/* s_lp_enabled and s_pds_sleep_ticks are referenced from the naked
 * vApplicationSleep asm by name, so they must have external linkage
 * (la <local-symbol> in a function-sectioned naked asm fails to
 * resolve at link time). */
/* .ocram_bss: retained across PDS31 so the gate state survives deep sleep
 * (otherwise wiped on the no-PSRAM split → gate logic resets each wake). */
bool __attribute__((section(".ocram_bss")))
s_lp_enabled; /* EFFECTIVE gate tested by the asm = idle_req && attached */
uint32_t __attribute__((section(".ocram_bss"))) s_pds_sleep_ticks;

/* Two intent inputs that AND into s_lp_enabled:
 *  - s_lp_idle_req: set by lp_sleep_enable_pds() — the poll-cadence /
 *    ICD-idle signal (thread_mtd: engine is_ready; Matter: CHIP
 *    _SetPollingInterval with a long interval).
 *  - s_lp_attached: set by lp_sleep_set_attached() — Thread attach state.
 *    Defaults TRUE so apps that never call the setter (e.g. thread_mtd,
 *    where is_ready already encodes attach) keep their old behavior;
 *    Matter calls it from its OT role-change handler so PDS NEVER fires
 *    while detached (which otherwise sleeps past the poll deadline and
 *    spins in a detach loop). */
static bool __attribute__((section(".ocram_bss"))) s_lp_idle_req;
/* .ocram_bss (retained, NOLOAD) so it can't carry an initializer — the default
 * TRUE is set in lp_sleep_init() instead, which preserves behavior for apps
 * that never call lp_sleep_set_attached() (thread_mtd/fall_neck) while letting
 * the value survive PDS31 for the no-PSRAM Matter SED. */
static bool __attribute__((section(".ocram_bss"))) s_lp_attached;
/* Third input, set by lp_sleep_set_app_hold(): the app vetoes PDS (e.g. the
 * fall-neck tag on USB power keeps its USB console). Cleared in lp_sleep_init()
 * — FALSE = no hold — so apps that never call the setter are unaffected. */
static bool __attribute__((section(".ocram_bss"))) s_lp_app_hold;

static uint32_t s_sleep_count;
static uint32_t s_last_actual_us;
static uint64_t s_total_actual_us;   /* all PDS31 time since boot */
/* Why lp_pds_sleep_impl fell back to WFI (telemetry; see lp_sleep_stats_t). */
static uint32_t s_dbg_short_gate;     /* idle window under the gate/floor */
static uint32_t s_dbg_short_holdoff;  /* RX quiescence holdoff */
static uint32_t s_dbg_short_tx;       /* 802.15.4 TX in flight */
static uint32_t s_dbg_short_rx;       /* indirect-frame RX window */

/* Tick of CHIP's last ACTIVE assertion (lp_sleep_enable_pds(false)).
 *
 * The no-PSRAM 64K split WIPES the CHIP managers (Server/IM/PlatformMgr,
 * ~62 KB, all in ocram_noret) on every PDS31 wake — measured, they do not
 * fit the retained front-64K. So deep-PDS is only SAFE when there is no
 * live CHIP session/subscription to lose: any CHIP activity (a subscription
 * report, a command, a CASE handshake) on the wiped state crashes.
 *
 * We can't query CHIP for "live subscription" from this C library without an
 * ABI-fragile header include, so we use a timing proxy instead: a live Matter
 * subscription is contractually reported at least once per negotiated
 * max-interval (~300 s) and each report flips the ICD to ACTIVE
 * (_SetPollingInterval(short) -> lp_sleep_enable_pds(false)). Therefore
 * "CHIP has been continuously idle for longer than the max-interval" is a
 * safe proxy for "no live session" — the app ANDs lp_sleep_ms_since_active()
 * past a holdoff into the deep-PDS gate. Retained so it survives PDS. */
static uint32_t __attribute__((section(".ocram_bss"))) s_last_active_tick;

/* ------------------------------------------------------------------------ *
 * Externs
 * ------------------------------------------------------------------------ */
extern void hal_lp_restore_cpu_regs(uint32_t save_addr);
extern void vPortSetupTimerInterrupt(void);

extern int board_get_debug_uart_index(void);
extern void uart_set_baudrate(int idx, int baudrate);
extern int uart_init(int idx, uint16_t caps);

#include "ieee802154_hal.h"
#include "sed_fastpoll.h"

/* ------------------------------------------------------------------------ *
 * M154 register snapshot — fast wake path (Strategy B, 2026-05-19)
 *
 * `ieee802154_hal_lp_resync()` re-runs the full `lmac154_init()` storm
 * (RFPHY init, M154 module clock, TX/RX/TCON FSM reset, OT-cfg replay,
 * FPT clear+replay, frame-filter setup, ECLIC vector register). PPK
 * trace 2026-05-19 measured ~2.85 ms wake-hook plateau attributable to
 * that storm — dominant cost when CPU is at XTAL 32 MHz post-wake.
 *
 * Mirrors apps/networking/thread/ipro_ieee154_sed_lp/src/sed_power.c:
 *   - `m154_snap_take()` runs once when the engine first reports
 *     ready (`lp_sleep_enable_pds(true)`). Captures M154 register file
 *     0x000–0x180 (covers GLB_CFG, MODE_CFG, TX_*, RX_*, PAN/addr,
 *     channel, frame filter, NBR_CTRL — everything OT touches via
 *     lmac154 setters).
 *   - `m154_snap_restore()` runs from the TCM wake hook: skip
 *     SWRST + SYM_CNT (the two writes that would themselves trigger
 *     reset / counter clear). Pure register memcpy, no init logic.
 *
 * What's NOT covered:
 *   - FPT table at 0x620+ (out of snap range). SED is the child so
 *     never writes FPT entries — irrelevant.
 *   - RF analog state. `rf_init(0)` still runs in the wake hook
 *     (warm path: `rf_restore_calibration_value` from `.noinit_data`
 *     retention; ~0.2 ms vs lmac154_init's ~2 ms).
 *   - ECLIC vector for M154_IRQn (lives in MISC, may be wiped).
 *     `ECLIC_Register_IRQ` is still called at the end of wake hook.
 *
 * Snap goes stale if OT changes channel / PAN / addresses /
 * tx_power / cca_mode after the snap was taken. For SED-Thread with
 * a committed Active Dataset and locked parent attachment, none of
 * those change after `is_ready()` → safe. If they do, the engine
 * would need to call `lp_sleep_m154_snap_take()` again to refresh.
 * ------------------------------------------------------------------------ */

#define LP_M154_BASE_ADDR (0x3C000000u)
#define LP_M154_SNAP_FIRST (0x000u)
#define LP_M154_SNAP_LAST (0x180u)
#define LP_M154_SNAP_COUNT ((LP_M154_SNAP_LAST - LP_M154_SNAP_FIRST) / 4 + 1)
#define LP_M154_SWRST_INDEX (0x008 / 4)
#define LP_M154_SYM_CNT_INDEX (0x040 / 4)

static uint32_t __attribute__((section(".ocram_bss"),
                               used)) s_m154_snapshot[LP_M154_SNAP_COUNT];
static bool s_m154_snap_valid;

/* PSRAM retention monitor REMOVED 2026-05-28.
 *
 * First attempt placed a 4 KB `.psram_bss` buffer for monitoring; boot
 * hangs reproducibly at the seam between sensor_driver_init and
 * factory_reset_button_init in main.c. Heap base shifted +4 KB and that
 * triggers some alignment/overlap issue at PSRAM init, before our
 * monitor code even gets a chance to run. Need a different approach:
 * sample the existing FreeRTOS heap content directly (e.g. snapshot
 * the first 4 KB after `__heap_psram_start` at boot, diff post-wake),
 * with no new BSS allocation. Pending. */

static void m154_snap_take(void) {
    for (uint32_t i = 0; i < LP_M154_SNAP_COUNT; i++) {
        s_m154_snapshot[i] =
            *(volatile uint32_t *)((uintptr_t)LP_M154_BASE_ADDR + i * 4);
    }
    s_m154_snap_valid = true;
}

static ATTR_TCM_SECTION void m154_snap_restore(void) {
    for (uint32_t i = 0; i < LP_M154_SNAP_COUNT; i++) {
        if (i == LP_M154_SWRST_INDEX || i == LP_M154_SYM_CNT_INDEX) {
            continue;
        }
        *(volatile uint32_t *)((uintptr_t)LP_M154_BASE_ADDR + i * 4) =
            s_m154_snapshot[i];
    }
}

/* ------------------------------------------------------------------------ *
 * UART re-init weak hook
 *
 * Default: re-init the debug UART at CONFIG_PLATFORM_UART_BAUDRATE so logs
 * survive the BSP's lp_uart_init(0, 115200)@XCLK hard-coded restore. Apps
 * that ship CONFIG_SHELL should provide a non-weak override that calls
 * shell_uart_reinit() so RX callback + IRQ are also restored.
 * ------------------------------------------------------------------------ */
/* Mirror platform_common.c: the app .config may leave the baud at the
 * platform default (CONFIG_PLATFORM_UART_BAUDRATE commented out), so this
 * shared file must not hard-require the macro. */
#ifndef CONFIG_PLATFORM_UART_BAUDRATE
#define CONFIG_PLATFORM_UART_BAUDRATE 115200
#endif
__attribute__((weak)) void otr_lp_uart_reinit_hook(void) {
    int uart_idx = board_get_debug_uart_index();
    uart_set_baudrate(uart_idx, CONFIG_PLATFORM_UART_BAUDRATE);
    uart_init(uart_idx, 0);
}

/* ------------------------------------------------------------------------ *
 * hal_lp_main OVERRIDE — runs early on PDS31 warm boot, from TCM
 *
 * Mirrors apps/networking/thread/ipro_ieee154_sed_lp/src/sed_power.c —
 * the reference SED that measures 7 µA average / ~10 mA wake-hook peak
 * on the same hardware. Key differences vs BSP default
 * (bsp/hal/lp_framework/ipro7/hal_lpfw_main.c):
 *
 *   - Default uses `hal_lp_wakeup_c_entry` which does *symmetric* clock
 *     save/restore: pre-sleep PLL192M → post-wake PLL192M. Wake-hook
 *     work and post-wake task work both run at full clock, drawing
 *     ~15-20 mA across the wake window (PPK 2026-05-19).
 *
 *   - This override forces XTAL 32 MHz with PLL+DLL POWERED OFF on
 *     every warm wake. Wake-hook work then runs at ~3-10 mA — matches
 *     sed_lp's profile.
 *
 *   - Skips the BSP default's `lp_uart_init(0, 115200)` early-debug
 *     UART setup. We don't need lp_print output and our `lp_wakeup_hook`
 *     calls `otr_lp_uart_reinit_hook()` later anyway, which re-derives
 *     the baud divider from whatever clock is current — so it adapts
 *     to XTAL 32 MHz automatically (921600 baud / 32 MHz = divider 35,
 *     0.8 % error, well within UART RX tolerance).
 *
 *   - Skips the JTAG pin restore (production LP build, no JTAG needed).
 *
 * Earlier attempt at this override (logged 2026-05-19 in the now-removed
 * note that sat here) reportedly caused UART to go silent after first
 * PDS wake — but we never localised the actual cause. Most likely it
 * was the UART block being touched before `vPortSetupTimerInterrupt`
 * had re-armed the FreeRTOS tick; the lp_wakeup_hook order now is
 * tick first, UART second, so the previous failure mode shouldn't
 * apply. If UART glitches again, the fallback is to re-add a
 * `lp_uart_init(0, CONFIG_PLATFORM_UART_BAUDRATE)` call here before
 * `hal_lp_restore_cpu_regs`.
 * ------------------------------------------------------------------------ */

#include "drv_glb.h" /* GLB_Set_System_CLK_Ex, GLB_Set_SF_CLK */
extern void hal_lp_xip_recovery(void);

/* DISABLED 2026-05-28 for Matter build.
 *
 * Our hal_lp_main override forces XTAL 32 MHz (PLL+DLL OFF) and calls
 * hal_lp_restore_cpu_regs() directly, skipping the BSP default's
 * hal_lp_wakeup_c_entry() — which means the system never restores
 * PLL192M after warm wake. Matter boots at PLL192M, so UART's baud
 * divider is computed for that clock; running post-wake at XTAL 32M
 * makes every char go out at the wrong rate. Symptom on 2026-05-28
 * current trace: chip wakes every ~1 s as expected but UART output
 * goes silent forever after the first PDS cycle.
 *
 * The SED reference (apps/networking/thread/ipro_thread_sed_nopsram)
 * survives this override because it never switches to PLL — it boots
 * and stays at XTAL 32 MHz throughout, so the override is consistent
 * with the rest of its runtime. Matter needs PLL192M for CHIP runtime
 * + crypto throughput, so we can't simply boot at XTAL.
 *
 * The wake-window power optimization (~10 mA saved over the ~3 ms wake
 * hook) is deferred — add it back when we have a hook that restores
 * PLL192M before scheduler resumes (e.g. switch back in lp_wakeup_hook
 * after RF init). For now, BSP default symmetric clock restore wins.
 *
 * BSP default: components/bsp/hal/lp_framework/ipro7/hal_lpfw_main.c
 * (weak hal_lp_main) — calls hal_lp_wakeup_c_entry which performs the
 * full clock restore back to whatever the app was running pre-sleep. */
#if 0
void ATTR_TCM_SECTION hal_lp_main(void) {
    static const uint32_t ATTR_TCM_SECTION early_restore_addrs[4] = {
        0x30000128,
        0x30000090,
        0x300000c8,
        0x300000cc,
    };

    /* Enable caches early — performance-critical for the wake hook. */
#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1)
    if (ICachePresent())
        EnableICache();
#endif
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1)
    if (DCachePresent())
        EnableDCache();
#endif
    __RWMB();
    __FENCE_I();

    /* MTimer = 32M / 16 — matches platform_init's boot setting. */
    writel(0x4000F, 0x300000A0);

    /* Restore the 4 peripheral clock / mux regs PDS cleared. */
    for (int i = 0; i < 4; i++) {
        writel(hal_lp_param->saved_regs[i], early_restore_addrs[i]);
    }

    /* SYSTEM CLOCK: XTAL 32 MHz with PLL+DLL POWERED OFF.
     * This is the big lever — wake-hook current drops from ~15-20 mA
     * (PLL192M sustained) to ~3-10 mA (XTAL 32M). */
    GLB_Set_System_CLK_Ex(GLB_DLL_XTAL_32M, GLB_SYS_CLK_XTAL,
                          /*pllOn=*/0, /*dllOn=*/0);

    /* Flash clock at XCLK — XIP recovery needs this before flash reads. */
    GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_XCLK, 0);

    if (hal_lp_param->do_xip_recovery) {
        hal_lp_xip_recovery();
        hal_lp_param->do_xip_recovery = 0;
    }

    {
        /* GLB_BASE + GLB_CLK_CFG3_OFFSET = 0x3000000C: div=0, enable */
        uint32_t tmpVal = readl(0x3000000C);
        tmpVal &= ~0x07;    /* Clear div bits */
        tmpVal |= (1 << 4); /* Enable UART clock */
        writel(tmpVal, 0x3000000C);
    }

    {
        /* HBN_BASE + HBN_GLB_OFFSET = 0x3008F030: select XCLK source */
        uint32_t tmpVal = readl(0x3008F030);
        tmpVal |= (1 << 15); /* HBN_UART_CLK_SEL2 for XCLK */
        writel(tmpVal, 0x3008F030);
    }

    hal_lp_param->wakeup_flag = 1;

    /* Determine wakeup reason from PDS_INT_STAT bits. */
    uint32_t pds_int_stat = readl(PDS_BASE + 0xC);
    if (pds_int_stat & (1 << 16))
        hal_lp_param->wakeup_reason |= HAL_LP_WAKEUP_RTC;
    if (pds_int_stat & (1 << 19))
        hal_lp_param->wakeup_reason |= HAL_LP_WAKEUP_GPIO;

    HBN_Disable_RTC_Compare();
    PDS_IntClear();
    HBN_Clear_All_IRQ();

    /* Returns to hal_lp_enter() caller via the saved ra. */
    hal_lp_restore_cpu_regs((uint32_t)hal_lp_param->cpu_regs);
    while (1)
        __asm__ volatile("nop");
}
#endif /* 0 — hal_lp_main override disabled, BSP weak default takes over */

/* ------------------------------------------------------------------------ *
 * BLE controller PDS hooks — weak no-ops by default so Thread-only apps
 * (ipro_thread_mtd / sed, which link no BLE controller) are byte-for-byte
 * unaffected. The Matter contact-sensor provides STRONG overrides that
 * quiesce + restore the link1 BLE controller across PDS31.
 *
 * Why this is needed: PDS31 resets and clock-gates the BZ (BLE) domain
 * every sleep (hal_pm.c BzRst/BzGateClk). The shared wake path here only
 * re-inits the 802.15.4 (M154) radio — it never touches the BLE
 * controller. So on a BLE-coex build the wake leaves a stale BLE_IRQn and
 * a dead/un-restored controller; the first task that touches BLE (or the
 * coex arbiter feeding M154) after resume faults/hangs — observed on the
 * contact-sensor as a hang right after re-attach ("Joining Multicast
 * groups"). Mirrors apps/bluetooth/ble/ipro_ble_lowpower_test/src/
 * lp_test_power.c.
 *
 *   otr_lp_ble_pre_sleep()       : before PDS — mask BLE IRQ; return
 *                                  non-zero to BAIL to WFI if BLE is busy;
 *                                  else save the BLE IP registers.
 *   otr_lp_ble_wake_restore()    : in the wake hook — re-ungate + restore
 *                                  BLE controller HW, re-bind its ISR
 *                                  (IRQ stays masked here).
 *   otr_lp_ble_post_wake(lpcyc)  : after PDS returns — software CLKN
 *                                  catch-up using elapsed 32.768 kHz
 *                                  cycles, then re-enable the BLE IRQ.
 * ------------------------------------------------------------------------ */
/* App wake hook — weak no-op by default. Runs in the wake hook (TCM, before the
 * scheduler resumes), right after the UART re-init. An app whose pads must
 * work again after a warm wake releases the PDS pad keep latch here (PDS31
 * entry freezes every pad and only a cold boot's board_pin_mux_init() clears
 * it); keep it short and TCM-resident. */
__attribute__((weak)) void otr_lp_app_wake_hook(void) {}

__attribute__((weak)) int otr_lp_ble_pre_sleep(void) { return 0; }
__attribute__((weak)) void otr_lp_ble_wake_restore(void) {}
__attribute__((weak)) void otr_lp_ble_post_wake(uint32_t lpcycles) {
    (void)lpcycles;
}

/* ------------------------------------------------------------------------ *
 * WAKEUP HOOK — runs after the default hal_lp_main restores XIP / cache
 * / clocks (symmetric system-clock restore, so we wake at PLL192M for
 * the MTD build).
 *
 * What gets done:
 *   1. Cache invalidate + post-XIP settle + re-invalidate (see big
 *      comment below for the SF-controller-glitch hypothesis).
 *   2. Re-arm the FreeRTOS SysTick (CPU-domain reset wiped it).
 *   3. UART re-init via the weak app hook. The default hal_lp_main
 *      hard-codes lp_uart_init(0, 115200) on XCLK so the LP firmware
 *      itself can emit early-debug output. Apps typically run at higher
 *      baud on a PLL-derived clock — without re-init every character
 *      after wake is garbled and the app *looks* stuck even though PDS
 *      is cycling and the radio is fine.
 *   4. ieee802154_hal_lp_resync() — full M154 baseband re-init from
 *      cached OT-level config + FPT slot software state + ECLIC binding.
 *      Covers M154 register file beyond 0x000–0x180 (FPT, NBR_CTRL, COEX,
 *      TX_PPDU_CTRL, RX_PPDU_STATUS) which a register-level snapshot
 *      can't safely cover.
 * ------------------------------------------------------------------------ */

static ATTR_TCM_SECTION int lp_wakeup_hook(int wakeup_reason, void *arg) {
    (void)wakeup_reason;
    (void)arg;

    /* GPIO 28 / 31 / 32 OE re-assert REMOVED 2026-05-28.
     *
     * These were debug-marker pins for sed_fastpoll RX-window /
     * tick / sleep-exit timing measurements on a vanilla EVB.
     * Conflict: when the board carries an external SF3 flash
     * (CONFIG_DISABLE_SDC_PINMUX=y), GPIO 28-33 are owned by the
     * SF3 controller as flash data/clock pins. Writing OE bits to
     * those pads from this wake hook can land mid-XIP fetch and
     * either crash the next instruction read or corrupt a flash
     * transaction in flight. Profile pulses elsewhere in
     * lp_pds_sleep_impl have already been pruned for the same
     * reason — debug instrumentation only, never load-bearing for
     * the SED path. */

    vPortSetupTimerInterrupt();

    /* Delegate UART re-init to the weak/strong app hook — see header. */
    otr_lp_uart_reinit_hook();
    otr_lp_app_wake_hook();

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_M154);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_M154);
    for (volatile int i = 0; i < 256; i++) {
    }
    // CPU_Interrupt_Pending_Clear(M154_IRQn);

    /* Re-init the security engine (HW SHA/AES) after PDS. Sec_Eng is in the
     * MISC power domain, which PDS31 power-gates + resets (hal_pm.c
     * pdsCfgLevel31.pdsCtl4 MiscPwrOff/MiscRst/MiscGateClk=1). Its AHB clock is
     * ungated only ONCE at boot (board_config / hal_common) and the wake path
     * does NOT restore CGEN_CFG0, so after wake the engine is clock-gated. The
     * first LINK-mode SHA the CHIP downlink path runs then hits a dead bus ->
     * garbage hash (mbedTLS -0xFFFF000F) -> auth failure -> CHIP session
     * corruption -> wild-pointer crash (all GPRs garbage, instruction page
     * fault). Re-ungate + soft-reset here (same pattern as M154 above); the
     * per-operation sec_hash_init_ex (Sec_Eng_SHA_Enable_Link) then sets up
     * link mode on a LIVE engine. Only matters when crypto runs right after a
     * PDS wake (the downlink path) — the engine-driven poll cadence happened to
     * avoid that window, which is why it never showed there. */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SEC_ENG);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SEC_ENG);
    for (volatile int i = 0; i < 256; i++) {
    }

    if (1) {
        extern void rf_init(uint32_t xtalfreq_hz);
        extern void rf_set_bz_mode(uint8_t mode);
        extern void rf_set_pwr_offset_zb(int8_t tx_pwr_os[16]);
        extern void rf_optimize_by_channel(uint32_t channel_mhz);
        extern uint8_t rf_set_tx_power_zb(int32_t target_power);

        rf_init(0);

        /* Full IPRO7 ZB TX power init sequence — partial application
         * leaves TX current drifting between cycles (occasional 25 mA
         * spike at 0 dBm observed 2026-05-20 when only rf_set_tx_power_zb
         * was reapplied). Matches the cold-boot pattern in
         * ot_radio_ieee802154.c::otPlatRadioEnable. Steps:
         *   1. bz_mode = ZB_ONLY → rf_set_tx_power_regs_zb writes the
         *      PA SHORT_N bits (low bias). Skipping this leaves PA at
         *      high BLE bias even though gain index says 0 dBm.
         *   2. pwr_offset_zb = {0} → wipes any drift from .noinit_data.
         *   3. rf_optimize_by_channel(freq) → re-derives the
         *      per-channel tx_pwr_offset_zb that rf_set_tx_power_zb
         *      reads next.
         *   4. rf_set_tx_power_zb(target) → applies actual PA state. */
        rf_set_bz_mode(/* IPRO7_RF_MODE_ZB_ONLY = */ 1u);
        {
            int8_t zero_zb_offsets[16] = {0};
            rf_set_pwr_offset_zb(zero_zb_offsets);
        }
        {
            uint8_t ch = sed_fastpoll_get_channel();
            if (ch >= 11u && ch <= 26u) {
                rf_optimize_by_channel(2405u + 5u * (uint32_t)(ch - 11u));
            }
        }
        (void)rf_set_tx_power_zb(0);

        m154_snap_restore();
    } else {
        (void)ieee802154_hal_lp_resync();
    }
    ECLIC_Register_IRQ(M154_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                       ECLIC_LEVEL_TRIGGER, 3, 0, NULL);

    /* Restore the BLE controller HW after the M154 radio (no-op weak
     * default; Matter overrides). PDS reset the BZ domain — this
     * re-ungates its clock, restores its IP registers, and re-binds the
     * BLE ISR. The BLE IRQ stays MASKED until otr_lp_ble_post_wake()
     * re-enables it after the RTC-based CLKN catch-up in
     * lp_pds_sleep_impl. */
    otr_lp_ble_wake_restore();

    return 0;
}

/* ------------------------------------------------------------------------ *
 * PDS sleep implementation — runs on the retention stack
 *
 * Called by the naked vApplicationSleep trampoline once the stack swap
 * is done. Takes the M154 snapshot, programs hal_lp_cfg, enters PDS,
 * then compensates FreeRTOS ticks for the elapsed RTC time.
 * ------------------------------------------------------------------------ */
/* Debug breadcrumb counters — exposed via lp_sleep_get_stats(). */
static uint32_t s_dbg_enter_count;
static uint32_t s_dbg_short_count;
static uint32_t s_dbg_pds_attempted_count;
static uint32_t s_dbg_last_sleep_us;

static void __attribute__((used)) lp_pds_sleep_impl(void) {
    s_dbg_enter_count++;

    uint32_t sleep_ticks = s_pds_sleep_ticks;

    /* Cap ticks before converting to µs to avoid uint32_t overflow. */
    uint32_t max_ticks = LP_MAX_SLEEP_US / (1000000UL / configTICK_RATE_HZ);
    if (sleep_ticks > max_ticks) {
        sleep_ticks = max_ticks;
    }

    uint32_t sleep_us = sleep_ticks * (1000000UL / configTICK_RATE_HZ);
    s_dbg_last_sleep_us = sleep_us;

    if (sleep_us <= PDS_OVERHEAD_US) {
        s_dbg_short_count++;
        s_dbg_short_gate++;
        __asm__ volatile("wfi");
        return;
    }

    uint32_t pds_sleep_us = sleep_us - PDS_OVERHEAD_US;

    /* Below LP_GATE_US the PDS overhead isn't worth it — just WFI. */
    if (pds_sleep_us < LP_GATE_US || pds_sleep_us < LP_MIN_SLEEP_US) {
        s_dbg_short_count++;
        s_dbg_short_gate++;
        __asm__ volatile("wfi");
        return;
    }

    /* Continuous-quiescence holdoff: stay on WFI until CHIP has been quiet for
     * PDS_QUIESCE_HOLDOFF_MS, so PDS never power-gates the radio/M154 while a
     * downlink command's MRP exchange/retransmissions are still in flight (the
     * downlink-during-sleep crash). Pure-idle PDS is unaffected — ms_since_active
     * grows unbounded with no CHIP activity. */
    if (lp_sleep_ms_since_active() < PDS_QUIESCE_HOLDOFF_MS) {
        s_dbg_short_count++;
        s_dbg_short_holdoff++;
        __asm__ volatile("wfi");
        return;
    }

    /* Gate PDS31 entry on TX-in-flight. OpenThread's RADIO platform
     * layer returns from otPlatRadioTransmit immediately after the
     * HAL queues triggerTxDelay, and OT-MAC then yields while waiting
     * for the radio's TX_DONE / ACK callback. If FreeRTOS hits
     * tickless-idle in that window and we enter PDS31, the M154
     * power-gate kills the in-flight TX — no TX_DONE IRQ ever fires
     * and OT times out the poll. The HAL keeps a sticky s_tx_in_flight
     * flag (set at triggerTxDelay, cleared in every TX terminal
     * callback) exactly so the LP sleep path can avoid this race.
     * Fall through to WFI instead — it's a sub-ms window in practice
     * (CSMA backoff + frame + ACK ≈ 5–8 ms). */
    /* Also gate on an in-flight INDIRECT-frame RX window: an FP=1 data-poll ACK
     * means the parent has a buffered downlink arriving a few ms from now. PDS31
     * here power-gates M154 and drops it (the first downlink during PDS idle — a
     * CASE Sigma1 / command — breaking the Matter session). Symmetric with the
     * tx-pending gate; self-clears after 120 ms so a missing frame can't wedge. */
    if (ieee802154_hal_is_tx_pending() || ieee802154_hal_is_rx_pending()) {
        s_dbg_short_count++;
        if (ieee802154_hal_is_tx_pending()) s_dbg_short_tx++;
        else s_dbg_short_rx++;
        __asm__ volatile("wfi");
        return;
    }

    s_dbg_pds_attempted_count++;
    /* NO LOG calls on this stack — we run on the 2-4 KB retention
     * stack with the scheduler suspended; the reference LP app
     * (apps/bluetooth/ble/ipro_ble_lowpower_test/src/lp_test_power.c)
     * keeps the impl purely counter-driven for exactly this reason.
     * Earlier draft did IPRO_LOGI here and overflowed the stack on
     * first PDS attempt (2026-05-19, no-PSRAM hardware bring-up).
     * Stats can be retrieved by `lp_sleep_get_stats()` from a normal
     * task stack — the engine heartbeat does this. */

    hal_lp_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.rtc_wakeup_en = 1;
    cfg.rtc_sleep_us = (uint64_t)pds_sleep_us;
    cfg.flash_power_down = 1;
    /* PSRAM retention is the single biggest PDS31 leakage contributor
     * (~50-100 µA self-refresh). Only retain when CONFIG_USE_PSRAM=y —
     * if the app placed FreeRTOS heap / OT message buffers / idle stack
     * in OCRAM, the PSRAM die isn't needed across sleep. */
#ifdef CONFIG_USE_PSRAM
    cfg.psram_retention = 1;
#else
    cfg.psram_retention = 0;
#endif

    hal_lp_result_t result;
    memset(&result, 0, sizeof(result));

    /* Stale M154 IRQ clear right before WFI so PDS actually engages —
     * without this a pending bit makes WFI return immediately. */
    CPU_Interrupt_Pending_Clear(M154_IRQn);

    /* Quiesce the BLE controller before PDS (no-op weak default; Matter
     * overrides to mask BLE IRQ + save BLE IP regs). If BLE is mid-event
     * the override returns non-zero — bail to WFI rather than tear the
     * controller down under it. */
    if (otr_lp_ble_pre_sleep() != 0) {
        s_dbg_short_count++;
        __asm__ volatile("wfi");
        return;
    }

    uint32_t rtc_lo_before, rtc_hi_before;
    HBN_Get_RTC_Timer_Val(&rtc_lo_before, &rtc_hi_before);
    uint64_t rtc_before = ((uint64_t)rtc_hi_before << 32) | rtc_lo_before;

    /* Enter PDS Level 31. Returns after warm wake (via HBN RAM FW). */
    (void)hal_lp_enter(&cfg, &result);

    /* Actual sleep duration from RTC (continues running through PDS).
     * Read BEFORE the direct fast-path TX so `s_last_actual_us` reports
     * pure PDS wall-clock time, not contaminated by the post-wake TX
     * work we're about to do. */
    uint32_t rtc_lo_after, rtc_hi_after;
    HBN_Get_RTC_Timer_Val(&rtc_lo_after, &rtc_hi_after);
    uint64_t rtc_after = ((uint64_t)rtc_hi_after << 32) | rtc_lo_after;
    uint32_t total_rtc_ticks = (uint32_t)(rtc_after - rtc_before);
    uint32_t total_us = HAL_LP_RTC_CNT_TO_US(total_rtc_ticks);

    s_sleep_count++;
    s_last_actual_us = total_us;
    s_total_actual_us += total_us;

    /* DEBUG wake-source probe: one line per PDS wake.
     *   req = xExpectedIdleTime FreeRTOS asked for (= time to the NEXT task's
     *         deadline). ~1000 ms = fast-poll owns it (expected). A SHORT req
     *         (e.g. 150-500 ms) means some OTHER task (CHIP ICD/subscription,
     *         OT MLE, mDNS/SRP, or the BLE controller) has a nearer timer and
     *         is the extra wake.
     *   act = actual PDS wall-clock. act << req → an IRQ (radio RX / BLE) cut
     *         the sleep short (an async wake, not a timer). */
    lp_uart_puts(0, "[lp]wk req=");
    lp_uart_putdec(0, s_pds_sleep_ticks); /* ticks == ms at 1000 Hz */
    lp_uart_puts(0, "ms act=");
    lp_uart_putdec(0, total_us / 1000u);
    lp_uart_puts(0, "ms\r\n");

    /* PSRAM warm-wake integrity probe print removed: its producer
     * (g_psram_wake_* set by hal_lp_psram_restore's diagnostic path) was
     * deleted with the DQS sweep recovery on 2026-05-28 — see the note
     * below. The orphaned externs broke linking for every USE_PSRAM +
     * THREAD_LP_SLEEP app. */

    /* Software-side BLE wake: advance the controller's CLKN by the elapsed
     * 32.768 kHz cycles (RTC and BLE LP clock share the 32 kHz source, so
     * RTC ticks == LP cycles) and re-enable the BLE IRQ. HW regs were
     * already restored in lp_wakeup_hook via otr_lp_ble_wake_restore().
     * No-op weak default; Matter overrides. */
    otr_lp_ble_post_wake(total_rtc_ticks);

    /* PSRAM retention monitor + DQS sweep recovery REMOVED 2026-05-28.
     * Diagnostic path proved the DUT has both DQS-edge issues (efuse
     * unprogrammed → wake DQS lands on table[15]=0xFFFF, marginal)
     * AND cell-level retention failures in PSRAM. dqs=14 wake override
     * mitigated ~70% but cell-level flips outside our 64KB monitor
     * window kept crashing FreeRTOS after a few cycles. PDS gate now
     * held OFF unconditionally in CHIP _SetPollingInterval — WFI
     * tickless idle is the ship-able baseline. See
     * [[ipro7-matter-pds31-blocked]] for the full chain. */

    /* DISABLED 2026-05-28: sed_fastpoll_post_pds_tx() suspected of
     * racing with vTaskStepTick below.
     *
     * Symptom: after ~30-40 PDS cycles in Matter build, FreeRTOS tripped
     * `configASSERT(0)` at tasks.c:4756 (custom uxTickSafetyLimit guard —
     * a cycle in pxDelayedTaskList->pxNext chain). xTaskIncrementTick
     * detected the iterator visiting more items than the list contains.
     *
     * Hypothesis: post_pds_tx runs with IRQs enabled (SysTick was re-
     * armed in lp_wakeup_hook, radio IRQs re-enabled). If an M154 IRQ
     * fires inside post_pds_tx and a TX callback un-blocks an OT task,
     * that task gets removed from pxDelayedTaskList. We then call
     * vTaskStepTick() which independently walks the same list — race
     * between ISR-driven removal and our explicit tick step corrupts
     * the list's pxNext linkage. The cycle detector catches it on the
     * next SysTick ISR's normal walk.
     *
     * Engine isn't actually driving polls in current Matter build
     * (`fp=0` in heartbeat stats — OT native DataPollSender owns the
     * poll cadence), so removing this call costs nothing. If/when we
     * wire the engine to actually drive polls, do the TX from OT task
     * context AFTER lp_pds_sleep_impl returns (not on retention stack
     * before vTaskStepTick). */
    /* (void)sed_fastpoll_post_pds_tx(); */

    /* Profile probe A REMOVED 2026-05-28 — see GPIO-OE comment in
     * lp_wakeup_hook above. Same SF3 flash pin conflict applies. */

    /* Fast-forward FreeRTOS ticks past the sleep.
     *
     * Cap to s_pds_sleep_ticks — anything more trips
     * configASSERT(xUpdatedTickCount <= xNextTaskUnblockTime) in
     * tasks.c:4530. PDS routinely overshoots its requested duration by
     * a tick or two because hal_lp_main / wake-hook overhead runs
     * before we re-read the RTC, so the unguarded path *did* fire the
     * assert on cycle 2 of every run, which looked like "TX hung after
     * PDS wake" because the scheduler died and no further OT logs
     * could come out. */
    uint32_t actual_ticks = total_us / (1000000UL / configTICK_RATE_HZ);
    if (actual_ticks > s_pds_sleep_ticks) {
        actual_ticks = s_pds_sleep_ticks;
    }
    if (actual_ticks > 0) {
        /* Mask IRQs across vTaskStepTick: by this point in the wake path the
         * M154 radio IRQ is live again, and a radio-RX ISR that unblocks an OT
         * task mutates pxDelayedTaskList while vTaskStepTick independently walks
         * the same list -> corrupted pxNext -> a task resumes on a garbage
         * frame (e.g. the ch=0/len=0 TX request, or worse) after ~tens of
         * cycles. xTaskIncrementTick runs ISR-masked for the same reason. */
        cpu_global_irq_disable();
        vTaskStepTick(actual_ticks);
        cpu_global_irq_enable();
    }

    /* Profile marker REMOVED 2026-05-28 — same SF3 flash pin
     * conflict as the GPIO-OE re-assert in lp_wakeup_hook. */
}

/* ------------------------------------------------------------------------ *
 * vApplicationSleep — FreeRTOS tickless idle mode 2 entry point
 *
 * Naked so we can switch SP to s_pds_stack before calling
 * lp_pds_sleep_impl. The caller's idle-task stack lives in PSRAM —
 * with psram_retention=1 it survives PDS, but switching to a known
 * OCRAM-resident stack for the sleep window adds defense-in-depth
 * against PSRAM bus glitches and keeps the call-graph simple to audit.
 *
 * If PDS is gated off (pre-attach), fall through to plain WFI.
 * ------------------------------------------------------------------------ */

__attribute__((naked)) void vApplicationSleep(uint32_t xExpectedIdleTime
                                              __attribute__((unused))) {
    __asm__ volatile(
        /* Check s_lp_enabled — fall through to WFI if disabled. */
        "la   t1, s_lp_enabled\n"
        "lb   t1, 0(t1)\n"
        "beqz t1, 1f\n"

        /* Save xExpectedIdleTime (a0) to s_pds_sleep_ticks. */
        "la   t1, s_pds_sleep_ticks\n"
        "sw   a0, 0(t1)\n"

        /* Switch to retention stack. */
        "mv   t0, sp\n"
        "la   sp, s_pds_stack\n"
        "li   t1, %0\n"
        "add  sp, sp, t1\n"
        "addi sp, sp, -8\n"
        "sw   t0, 0(sp)\n"
        "sw   ra, 4(sp)\n"
        "call lp_pds_sleep_impl\n"
        "lw   ra, 4(sp)\n"
        "lw   t0, 0(sp)\n"
        "mv   sp, t0\n"
        "ret\n"

        "1:\n"
        "wfi\n"
        "ret\n"
        :
        : "i"(PDS_STACK_SIZE));
}

/* ------------------------------------------------------------------------ *
 * Public API
 * ------------------------------------------------------------------------ */
int lp_sleep_init(void) {
    int rc = hal_lp_init();
    if (rc != 0) {
        IPRO_LOGE(LOG_TAG, "hal_lp_init failed: %d", rc);
        return rc;
    }

    rc = hal_lp_register_wakeup_hook(lp_wakeup_hook, NULL);
    if (rc != 0) {
        IPRO_LOGE(LOG_TAG, "hal_lp_register_wakeup_hook failed: %d", rc);
        return rc;
    }

    s_lp_enabled = false;
    /* Retained NOLOAD state survives a warm reboot: start with no idle request
     * so a stale TRUE from the previous boot cannot open the gate before the
     * app (or the fast-poll engine / CHIP) asks for PDS again. */
    s_lp_idle_req = false;
    /* Default the (now retained, NOLOAD) attach gate to TRUE here, replacing
     * the old `= true` initializer — apps that never call
     * lp_sleep_set_attached() (thread_mtd/fall_neck) keep their behavior, and
     * apps that do (Matter) override it right after. */
    s_lp_attached = true;
    s_lp_app_hold = false;
    /* Seed "last active" to now so the quiescence proxy starts the holdoff
     * fresh on cold boot — the device WFIs through commissioning + attach and
     * only considers deep-PDS after a full holdoff of CHIP silence. */
    s_last_active_tick = xTaskGetTickCount();

    IPRO_LOGI(
        LOG_TAG,
        "PDS31 tickless idle ready (gate %d ms, ieee802154_hal_lp_resync)",
        (int)CONFIG_THREAD_LP_MIN_SLEEP_MS);
    return 0;
}

/* ===== CONTROL-TEST SWITCH — REVERT TO 0 BEFORE SHIPPING =====
 * 1 = never enter PDS31 deep sleep; idle does plain tickless WFI (PSRAM, radio,
 *     BLE all stay powered, no warm-wake restore). Everything else (Thread
 *     attach, Matter, SED polling, _SetPollingInterval) is unchanged.
 * Used to isolate whether the PDS deep-sleep/warm-wake is what breaks the
 * Matter CASE session across idle, vs a Matter-layer issue independent of PDS.
 * If the session stays alive with this =1, the breakage is in the deep-sleep
 * path (ICD downlink/handshake resilience); if it still goes defunct, PDS is
 * not the cause. */
#define LP_DISABLE_PDS_DEEP_SLEEP   0

/* Recompute the effective gate (idle_req && attached && !app_hold) and act on
 * the false->true transition. */
static void lp_recompute_gate(void) {
    bool eff = s_lp_idle_req && s_lp_attached && !s_lp_app_hold;
#if defined(LP_DISABLE_PDS_DEEP_SLEEP) && (LP_DISABLE_PDS_DEEP_SLEEP)
    eff = false;   /* control test: force WFI-only, no PDS31 */
#endif
    if (s_lp_enabled == eff) {
        return;
    }
    s_lp_enabled = eff;

    /* On the false->true transition, capture the M154 register snapshot
     * so every wake hook uses the fast `m154_snap_restore` path instead
     * of the ~2.85 ms `lmac154_init` storm. M154 state at this moment
     * reflects steady-state idle: post-attach, OT config committed
     * (PAN/addr/channel/tx_power), RX off (SED). */
    if (eff) {
        m154_snap_take();
    }
}

/* Poll-cadence / ICD-idle signal. ANDed with attach state. */
void lp_sleep_enable_pds(bool enable) {
    if (!enable) {
        /* CHIP active — refresh the activity timestamp on EVERY call (even
         * repeats, before the idempotent return below) so the quiescence
         * proxy in lp_sleep_ms_since_active() measures true idle time. */
        s_last_active_tick = xTaskGetTickCount();
    }
    if (s_lp_idle_req == enable) {
        return; /* idempotent — engine / CHIP may call repeatedly */
    }
    s_lp_idle_req = enable;
    lp_recompute_gate();
}

/* Refresh the quiescence holdoff on a received downlink frame. CHIP only flips
 * the gate active (lp_sleep_enable_pds(false)) AFTER it decrypts+processes a
 * Matter message; between retrieving a frame and that happening — and across a
 * multi-step CASE Sigma1/2/3 handshake — the device must stay on WFI (no PDS) so
 * the next downlink is retrievable. Called from ot_radioTask RX-done (task
 * context). Any genuine RX holds deep-PDS off for PDS_QUIESCE_HOLDOFF_MS, then
 * true idle resumes deep sleep. Complements the per-poll rx-pending gate. */
void lp_sleep_note_rx_activity(void) {
    s_last_active_tick = xTaskGetTickCount();
}

/* Milliseconds since CHIP was last ACTIVE (see s_last_active_tick). Used by
 * Matter apps to keep deep-PDS gated OFF (WFI) while a subscription is live,
 * since a PDS wipe would destroy the un-retained CHIP managers. Tick-based
 * (tickless idle steps the count via vTaskStepTick), good enough for the
 * ~300 s holdoff granularity. */
uint32_t lp_sleep_ms_since_active(void) {
    uint32_t now = xTaskGetTickCount();
    return (uint32_t)(now - s_last_active_tick) * (uint32_t)portTICK_PERIOD_MS;
}

/* Thread attach state. PDS31 must NOT fire while detached (it would sleep
 * past the poll/keepalive deadline and never re-attach). Matter wires this
 * from its OT role-change handler; thread_mtd leaves it at the TRUE default
 * because its is_ready() already gates on attach. */
void lp_sleep_set_attached(bool attached) {
    if (s_lp_attached == attached) {
        return;
    }
    s_lp_attached = attached;
    lp_recompute_gate();
}

/* App veto, ANDed (inverted) into the gate. */
void lp_sleep_set_app_hold(bool hold) {
    if (s_lp_app_hold == hold) {
        return;
    }
    s_lp_app_hold = hold;
    lp_recompute_gate();
}

void lp_sleep_get_stats(lp_sleep_stats_t *out) {
    if (out == NULL) {
        return;
    }
    out->enter_count = s_dbg_enter_count;
    out->short_count = s_dbg_short_count;
    out->pds_attempted = s_dbg_pds_attempted_count;
    out->pds_completed = s_sleep_count;
    out->last_req_us = s_dbg_last_sleep_us;
    out->last_actual_us = s_last_actual_us;
    out->total_sleep_ms = (uint32_t)(s_total_actual_us / 1000u);
    out->short_gate = s_dbg_short_gate;
    out->short_holdoff = s_dbg_short_holdoff;
    out->short_tx = s_dbg_short_tx;
    out->short_rx = s_dbg_short_rx;
    out->gate_enabled = s_lp_enabled;
}

/* ------------------------------------------------------------------------ *
 * Optional shell stats command (compiled in only when CONFIG_SHELL=y)
 * ------------------------------------------------------------------------ */
#ifdef CONFIG_SHELL
#include "shell.h"

static int cmd_lp_stats(int argc, char **argv) {
    (void)argc;
    (void)argv;
    printf("vApplicationSleep entries : %u\r\n", (unsigned)s_dbg_enter_count);
    printf("  short-sleep WFI fallback: %u\r\n", (unsigned)s_dbg_short_count);
    printf("  PDS attempts            : %u\r\n",
           (unsigned)s_dbg_pds_attempted_count);
    printf("  PDS completed           : %u\r\n", (unsigned)s_sleep_count);
    printf("  last requested sleep us : %u\r\n", (unsigned)s_dbg_last_sleep_us);
    printf("  last actual sleep us    : %u\r\n", (unsigned)s_last_actual_us);
    printf("  gate state              : %s\r\n",
           s_lp_enabled ? "ENABLED" : "DISABLED");
    printf("  gate threshold (ms)     : %d\r\n",
           (int)CONFIG_THREAD_LP_MIN_SLEEP_MS);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lp_stats, lp_stats, Show PDS31 tickless idle stats);
#endif

#else /* !CONFIG_THREAD_LP_SLEEP */

int lp_sleep_init(void) { return 0; }
void lp_sleep_enable_pds(bool enable) { (void)enable; }
void lp_sleep_note_rx_activity(void) { }
void lp_sleep_set_attached(bool attached) { (void)attached; }
void lp_sleep_set_app_hold(bool hold) { (void)hold; }
uint32_t lp_sleep_ms_since_active(void) { return 0; }
void lp_sleep_get_stats(lp_sleep_stats_t *out) {
    if (out != NULL) {
        memset(out, 0, sizeof(*out));
    }
}

/* If FreeRTOS tickless idle is on but the deep-sleep path is compiled
 * out, FreeRTOSConfig.h still routes portSUPPRESS_TICKS_AND_SLEEP into
 * vApplicationSleep. Provide a WFI fallback so link succeeds. */
#if defined(CONFIG_FREERTOS_USE_TICKLESS_IDLE) &&                              \
    (CONFIG_FREERTOS_USE_TICKLESS_IDLE != 0)
void vApplicationSleep(uint32_t xExpectedIdleTime) {
    (void)xExpectedIdleTime;
    __asm__ volatile("wfi");
}
#endif

#endif /* CONFIG_THREAD_LP_SLEEP */
