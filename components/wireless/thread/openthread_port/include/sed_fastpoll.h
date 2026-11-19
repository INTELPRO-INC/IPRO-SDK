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
 * @file sed_fastpoll.h
 * @brief OT-bypass fast path for SED 802.15.4 Data Request TX.
 *
 * Singleton, OT-side. Public API only — implementation, frame layout,
 * cryptography, and timing internals live in the matching .c (which is
 * compiled into the openthread_port library).
 *
 * Locking contract: caller MUST hold the OT external lock (otrLock /
 * otrUnlock) around every call. Same reasoning as ot_fastpoll_shim.h.
 *
 * Typical sequence:
 *   sed_fastpoll_init();         (* once after OT attach *)
 *   ...
 *   sed_fastpoll_build_one();
 *   sed_fastpoll_tx(3000, &r);
 *   if (r == SED_FP_TX_FP_NO || r == SED_FP_TX_FP_YES)
 *       sed_fastpoll_commit();
 */
#ifndef SED_FASTPOLL_H_
#define SED_FASTPOLL_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SED_FP_TX_FP_NO   = 0,
    SED_FP_TX_FP_YES  = 1,
    SED_FP_TX_NO_ACK  = 2,
    SED_FP_TX_BUSY    = 3,
    SED_FP_TX_TIMEOUT = 4,
    SED_FP_TX_ERROR   = 5,
} sed_fastpoll_tx_result_t;

/* Refresh OT snapshot + rebuild any per-snapshot module state.
 * Call once after attach, and again on key rotation / re-attach / parent
 * change (typically wired from the OT state-changed callback). */
int sed_fastpoll_init(void);

/* Equivalent to init but cheaper if a snapshot already exists.
 * Use on routine state-change notifications. */
int sed_fastpoll_refresh(void);

/* Prepare one frame for TX. Must be called once per TX (counter / seq /
 * crypto are stamped). Cheap. */
int sed_fastpoll_build_one(void);

/* Synchronously TX the built frame and busy-wait up to @p wait_us
 * microseconds for ISR completion. Returns 0 on success and fills
 * *result. On NO_ACK / BUSY / TIMEOUT, do NOT call commit. */
int sed_fastpoll_tx(uint32_t wait_us, sed_fastpoll_tx_result_t *result);

/* On successful ACK (FP_NO / FP_YES) only, tell OT to advance its
 * KeyManager + SubMac frame counter so its own later TX doesn't reuse
 * the value we just burned. Returns 0 on success. */
int sed_fastpoll_commit(void);

/* Returns true when the module has a valid snapshot + attached child
 * + valid parent — i.e. sed_fastpoll_tx will not return ERROR. */
bool sed_fastpoll_is_ready(void);

/* Returns the 802.15.4 channel from the latest snapshot, or 0 if the
 * module isn't ready. Used by the LP wake hook to re-run the IPRO7
 * RF per-channel optimizer post-PDS without taking the OT lock. */
uint8_t sed_fastpoll_get_channel(void);

/* Toggle the early-close behavior of the FP=YES RX window. When
 * enabled (default), the observer stops RX as soon as any frame is
 * received during the window. When disabled, RX runs to the full
 * FP_RX_AFTER_YES_MS (150 ms) timeout — matches pre-2026-05-21
 * behavior, useful for A/B testing. */
void sed_fastpoll_set_rx_early_close(bool enable);
bool sed_fastpoll_get_rx_early_close(void);

/* Optional one-shot debug log of the prepared frame using the given log
 * tag. Implementation chooses what to print; treat output format as
 * unstable. Safe to call from any task that holds the OT lock. */
void sed_fastpoll_log_debug(const char *log_tag);

/* Convert tx result to a short string (no allocation). */
const char *sed_fastpoll_tx_result_str(sed_fastpoll_tx_result_t r);

/* Configure the fast-poll engine. The engine itself is driven from the
 * OT task loop (see ot_freertos.c) — no separate FreeRTOS task is
 * created here. Calling this once after attach:
 *   - Silences OT's DataPollSender by setting its external poll period
 *     to the maximum allowed (~18.6 h). MLE keep-alive is unaffected.
 *   - Sets the fast-poll cadence used by sed_fastpoll_tick().
 *
 * Caller must hold the OT lock. Idempotent. Pass @p period_ms = 0 to
 * use the built-in default. */
int sed_fastpoll_enable(uint32_t period_ms);

/* Choose who owns the PDS31 entry gate. Default (true) = the engine drives
 * lp_sleep_enable_pds() from its readiness — correct for Thread-only SED apps.
 * A Matter ICD must call this with `false` AFTER sed_fastpoll_enable() so that
 * CHIP's _SetPollingInterval() is the sole gate owner (idle=allow PDS,
 * active=block PDS); otherwise the engine forces the gate ON every tick and
 * the device deep-sleeps during ICD ACTIVE mode. */
void sed_fastpoll_set_drive_gate(bool enable);

/* Disable the fast-poll engine. The OT task loop will stop calling
 * sed_fastpoll_tick(). Does NOT restore OT's poll period — caller may
 * want to do that explicitly via otLinkSetPollPeriod(). */
void sed_fastpoll_disable(void);

/* Called from the OT task loop right after otSysProcessDrivers(),
 * while still holding the OT lock. Returns whether any state change
 * was made (for log noise reduction). */
void sed_fastpoll_tick(void);

/* Milliseconds until the next wake the fast-poll engine wants. Returns
 * UINT32_MAX when disabled or when no work is pending. Use to clamp the
 * OT task's sleep timeout. Lock-free read — values are computed under
 * the lock during tick(). */
uint32_t sed_fastpoll_next_wake_ms(void);

/* PDS31 fast path: called from lp_pds_sleep_impl directly after the
 * radio comes back up post-wake, BEFORE the FreeRTOS scheduler dispatches
 * the OT task. Performs the same secured Data Request TX as
 * sed_fastpoll_tick() but without paying for task switch / OT-lock
 * acquisition / alarm-callback dispatch.
 *
 * Locking: caller must NOT hold the OT lock (we run on the retention
 * stack out of the naked vApplicationSleep — no FreeRTOS task APIs).
 * The OT-side counter commit is deferred to the next sed_fastpoll_tick().
 *
 * Returns true iff a TX was actually performed; on the next OT-task
 * tick the engine consumes the cached result and skips its own TX. */
bool sed_fastpoll_post_pds_tx(void);

#ifdef __cplusplus
}
#endif

#endif /* SED_FASTPOLL_H_ */
