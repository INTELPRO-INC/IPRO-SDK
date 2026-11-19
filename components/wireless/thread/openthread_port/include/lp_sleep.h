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
 * @file lp_sleep.h
 * @brief PDS31 tickless-idle integration for SED applications
 *
 * Part of openthread_port — the same library that owns the SED fast-poll
 * engine. Together they implement the "wake briefly to poll, deep-sleep
 * between polls" pattern for sleepy end devices. Apps just enable
 * `CONFIG_THREAD_LP_SLEEP=y` and call `lp_sleep_init()` once before
 * the FreeRTOS scheduler starts; the engine drives the gate
 * (`lp_sleep_enable_pds`) automatically based on attach state.
 *
 * When CONFIG_THREAD_LP_SLEEP is set, this module provides:
 *   - hal_lp_init() + register a wakeup hook that re-inits RF analog,
 *     resyncs the M154 baseband, and re-runs the SysTick / UART setup
 *     that the post-PDS warm boot would otherwise leave broken.
 *   - vApplicationSleep(xExpectedIdleTime), invoked by the FreeRTOS
 *     idle task whenever all tasks are blocked for at least
 *     CONFIG_THREAD_LP_MIN_SLEEP_MS. Configures hal_lp_cfg with
 *     PSRAM + OCRAM retention so OpenThread / lwIP heaps survive
 *     PDS31, then calls hal_lp_enter() and catches up FreeRTOS ticks
 *     on return.
 *
 * Apps that need to re-init UART / shell at wake time should provide
 * a non-weak `otr_lp_uart_reinit_hook(void)` — the default uses
 * `board_get_debug_uart_index()` + CONFIG_PLATFORM_UART_BAUDRATE.
 */

#ifndef OPENTHREAD_PORT_LP_SLEEP_H
#define OPENTHREAD_PORT_LP_SLEEP_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize hal_lp and register the M154/RF wakeup hook.
 * No-op when CONFIG_THREAD_LP_SLEEP is not set (caller still allowed
 * to call this — a successful 0 is returned and nothing else happens).
 *
 * Must be called BEFORE `vTaskStartScheduler()` so the wakeup hook is
 * registered before the first PDS sleep attempt.
 *
 * @return 0 on success, negative on error.
 */
int lp_sleep_init(void);

/**
 * The effective PDS31 entry gate is the AND of two independent intents:
 *
 *   PDS-enabled = lp_sleep_enable_pds(idle) && lp_sleep_set_attached(att)
 *
 * Both must be true before vApplicationSleep deep-sleeps; otherwise it
 * falls back to plain WFI and the M154 baseband stays powered.
 *
 * lp_sleep_enable_pds() is the poll-cadence / ICD-idle signal:
 *   - SED fast-poll: the engine calls it when `is_ready()` flips.
 *   - Matter ICD: CHIP _SetPollingInterval calls it (long interval -> on).
 *
 * Calling with the same value twice is cheap; the flag is idempotent.
 * Exposed publicly mainly for diagnostics and unusual use cases (e.g.,
 * force-disable during OTA).
 */
void lp_sleep_enable_pds(bool enable);

/*
 * Refresh the quiescence holdoff on a received downlink frame (call from the OT
 * radio RX-done path, task context). Keeps deep-PDS gated OFF for
 * PDS_QUIESCE_HOLDOFF_MS after any genuine RX, so a multi-step downlink exchange
 * (e.g. CASE Sigma1/2/3) stays on WFI and the next message is retrievable, even
 * before CHIP flips the gate active. No-op when LP sleep is not compiled in.
 */
void lp_sleep_note_rx_activity(void);

/**
 * Gate PDS31 entry by Thread attach state (the second AND input above).
 *
 * Defaults to TRUE so apps that never call it (e.g. ipro_thread_mtd,
 * whose is_ready()/poll cadence already encodes attach) are unaffected.
 *
 * Matter contact-sensor wires this from an OT role-change callback:
 * detach -> false IMMEDIATELY. Without this, a detached ICD arms PDS on
 * the idle poll interval, sleeps PAST the keepalive/poll deadline, and
 * spins in a detach loop (our_short=0xfffe, never re-attaches).
 */
void lp_sleep_set_attached(bool attached);

/**
 * App veto, a third input ANDed (inverted) into the PDS31 gate:
 *   PDS-enabled = idle && attached && !app_hold
 * Defaults to FALSE (no hold) so apps that never call it are unaffected.
 * The fall-neck tag holds PDS off while it runs on USB power, so its USB
 * console and boot2 rescue keep working while it charges.
 */
void lp_sleep_set_app_hold(bool hold);

/**
 * Milliseconds since CHIP was last ACTIVE (last lp_sleep_enable_pds(false)).
 *
 * A no-PSRAM 64K Matter SED CANNOT retain the CHIP managers across PDS31
 * (measured ~62 KB, the front-64K holds ~12 KB of headroom), so deep-PDS is
 * only safe when no live subscription/session remains to be touched on wake.
 * A live Matter subscription reports at least once per negotiated
 * max-interval (~300 s) and each report flips the ICD active, so the app
 * ANDs `lp_sleep_ms_since_active() > holdoff` (holdoff > max-interval) into
 * the deep-PDS gate: below the holdoff it stays in WFI (managers retained,
 * stable); only after a full holdoff of CHIP silence does deep-PDS arm.
 * Returns 0 when the low-power path is compiled out.
 */
uint32_t lp_sleep_ms_since_active(void);

/**
 * Optional app-side hook for re-initializing the debug UART after a
 * PDS31 wake. The default implementation (weak) uses
 * `board_get_debug_uart_index()` + `CONFIG_PLATFORM_UART_BAUDRATE`,
 * which is the right answer for almost all apps.
 *
 * Apps that ship a CONFIG_SHELL build typically want to call their
 * shell-specific reinit (so RX callback + IRQ are restored). To do
 * that, define a non-weak `otr_lp_uart_reinit_hook()` in the app
 * sources — the linker will prefer it over the weak default.
 *
 * Runs in the wakeup hook context (TCM, before the SysTick is
 * re-armed). Keep it short and avoid blocking calls.
 */
void otr_lp_uart_reinit_hook(void);

/**
 * Optional app wake hook (weak no-op). Called in the wake hook right after
 * otr_lp_uart_reinit_hook(), TCM context, scheduler not yet resumed. PDS31
 * entry sets the PDS pad keep latch, which freezes every pad; nothing on the
 * warm-wake path clears it (a cold boot does, in board_pin_mux_init()), so an
 * app that drives or reads GPIOs after a wake calls PDS_Disable_GPIO_Keep()
 * here. Keep it short; put it in ATTR_TCM_SECTION.
 */
void otr_lp_app_wake_hook(void);

/**
 * Optional app-side hooks to quiesce / restore a live BLE controller
 * across PDS31. Weak no-op defaults, so Thread-only apps (no BLE
 * controller linked) are unaffected. A BLE-coex app (e.g. the Matter
 * contact-sensor) defines non-weak versions that drive the link1
 * controller's sleep API — PDS resets the BZ domain, and without these
 * the wake path (M154-only) leaves a stale BLE_IRQn / dead controller
 * that faults a task after resume.
 *
 *   otr_lp_ble_pre_sleep()        : before PDS entry, on the retention
 *       stack with the scheduler suspended. Mask the BLE IRQ and save the
 *       BLE IP registers. Return non-zero to ABORT this PDS attempt
 *       (fall back to WFI) when the controller is mid-event.
 *   otr_lp_ble_wake_restore()     : in the wake hook (TCM, after the M154
 *       re-init). Re-ungate the BLE clock, restore controller HW, re-bind
 *       the ISR. Leave the BLE IRQ MASKED.
 *   otr_lp_ble_post_wake(lpcycles): after PDS returns. Advance the
 *       controller CLKN by `lpcycles` (elapsed 32.768 kHz cycles) and
 *       re-enable the BLE IRQ.
 */
int  otr_lp_ble_pre_sleep(void);
void otr_lp_ble_wake_restore(void);
void otr_lp_ble_post_wake(uint32_t lpcycles);

/**
 * PDS31 sleep telemetry — snapshot of the counters maintained by
 * `lp_pds_sleep_impl`. Used by `sed_fastpoll`'s periodic stats log
 * and the optional `lp_stats` shell command. Zero on first read after
 * boot means "PDS never fired yet".
 *
 * Safe to call from any context — fields are individual 32-bit reads.
 * No-op stub when CONFIG_THREAD_LP_SLEEP is not compiled in.
 */
typedef struct {
    uint32_t enter_count;       /* vApplicationSleep entries */
    uint32_t short_count;       /* WFI fallback (gate threshold / TX pending) */
    uint32_t pds_attempted;     /* hal_lp_enter calls issued */
    uint32_t pds_completed;     /* PDS31 cycles that came back through wake-hook */
    uint32_t last_req_us;       /* requested sleep duration of last call */
    uint32_t last_actual_us;    /* RTC-measured actual sleep duration of last cycle */
    uint32_t total_sleep_ms;    /* all PDS31 time since boot (duty-cycle telemetry) */
    uint32_t short_gate;        /* WFI: idle window under the gate / floor */
    uint32_t short_holdoff;     /* WFI: RX quiescence holdoff */
    uint32_t short_tx;          /* WFI: 802.15.4 TX in flight */
    uint32_t short_rx;          /* WFI: indirect-frame RX window */
    bool     gate_enabled;      /* effective gate = idle_req && attached */
} lp_sleep_stats_t;

void lp_sleep_get_stats(lp_sleep_stats_t *out);

#ifdef __cplusplus
}
#endif

#endif /* OPENTHREAD_PORT_LP_SLEEP_H */
