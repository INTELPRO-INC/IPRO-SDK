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

/*
 * Fatal-error handling for the OpenThread Spinel host (RCP over UART).
 *
 * OpenThread treats a lost or misbehaving RCP as fatal: DieNow() -> exit(),
 * which on POSIX ends otbr-agent and lets a supervisor restart it. Here
 * newlib's _exit() only deleted the calling task, with the OpenThread mutex
 * held, so the BR kept running with Thread dead and nothing reported it
 * (2026-09-28: an RCP silent after RX enable -> 3 restorations -> _exit).
 *
 * Every such path now ends in ot_rcp_fatal(): a fatal exit()/abort()/assert()
 * in a task running OpenThread (its own task, or one holding the OpenThread
 * mutex) through newlibc_fatal_hook() - the application wraps exit() so this
 * runs before atexit handlers - plus the port's radio command checks and
 * otPlatAssertFail(). It records the fault, turns the RCP's radio off (a live
 * RCP would keep auto-ACKing for a host that is gone), stops the Spinel UART,
 * marks OpenThread stopped (otrIsStopped(); the instance getters return NULL)
 * and calls the application's policy, ot_rcp_on_fatal(). The failing task then
 * parks itself, still holding the OpenThread mutex, so nothing re-enters the
 * broken stack.
 */
#ifndef OT_RCP_HEALTH_H
#define OT_RCP_HEALTH_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int exit_code;     /* otExitCode: 1 failure, 3 incompatible RCP, ... */
    const char *cause; /* "exit", "abort", "assert", "radio-cmd", "not-rcp", "rcp-flapping", "uart-released" */
    const char *file;  /* source location when known, else NULL */
    int line;
    uint32_t uptime_ms;
    bool in_ot_task;   /* false: another task (holding the OpenThread mutex) was parked with its own locks */
    char task[16];     /* task that hit it */
} ot_rcp_fault_t;

/* Record the fault, stop OpenThread and its radio link, run the policy and
 * park the calling task. Task context only; never returns. */
void ot_rcp_fatal(int exit_code, const char *cause, const char *file, int line) __attribute__((noreturn));

/* The host is resetting the RCP (a Spinel RESET frame). OpenThread's recovery
 * retries a request for as long as each recovery succeeds, so an RCP that
 * recovers and then fails again at the same step loops forever inside one
 * request while Thread is unusable. More than THREAD_RCP_FLAP_MAX_RESETS resets
 * in 5 minutes with no radio progress in between stop OpenThread instead
 * (cause "rcp-flapping"). Transport task context only. */
void ot_rcp_note_reset(void);

/* A frame was received or a transmission completed: the RCP is doing useful
 * work, so earlier resets were transient. */
void ot_rcp_note_progress(void);

/* Copy of the recorded fault; false while OpenThread has not failed. */
bool ot_rcp_get_fault(ot_rcp_fault_t *out);

/* Application policy (weak, default: nothing - Thread stays down until the
 * next boot). Called once, from the failing task with the OpenThread mutex
 * held: no OpenThread APIs and no long blocking here. */
void ot_rcp_on_fatal(const ot_rcp_fault_t *fault);

/* RCP version, capabilities, recovery counters and the fault, for a shell. */
void ot_rcp_print_status(void);

/* Turn the RCP's radio off (best effort) and stop the Spinel transport. */
void ot_radio_spinel_stop(void);

/* Another user takes UART1 over (a bench console): Spinel stops using it and
 * no longer touches its interrupts. OpenThread fails soon after. */
void ot_radio_spinel_release_uart(void);

#ifdef __cplusplus
}
#endif

#endif /* OT_RCP_HEALTH_H */
