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
 * @file ipro_pm_radio.h
 * @brief ipro_pm interface for radio drivers (BLE controller, 802.15.4 / Thread).
 *
 * A radio driver registers one participant.  For every idle window that may go
 * to PDS the framework asks each participant for its state and its earliest
 * deadline, picks who executes the PDS transaction and offers the window:
 *
 *   - a participant in PM_PART_EXECUTOR runs the transaction itself from its
 *     own task (the BLE controller while a link is in the PDS profile);
 *   - otherwise, when every present participant is PM_PART_READY, the idle
 *     task runs it (CONFIG_IPRO_PM_IDLE_PDS);
 *   - PM_PART_BUSY keeps the window at WFI, PM_PART_RETRY keeps it at WFI and
 *     offers it again after the next interrupt.
 *
 * Suspend/resume around the transaction goes through ipro_pm devices
 * (ipro_pm_device_register()), whichever task executes it.
 *
 * Every callback except the registration runs from the idle task's tickless
 * hooks, most with interrupts masked: short, lock-free, never blocking.
 */
#ifndef IPRO_PM_RADIO_H_
#define IPRO_PM_RADIO_H_

#include <stdbool.h>
#include <stdint.h>

#include <FreeRTOS.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Participant state for one idle window. */
typedef enum {
    PM_PART_ABSENT = 0, /**< not running: no constraint */
    PM_PART_READY,      /**< can be suspended and resumed around any executor's PDS */
    PM_PART_EXECUTOR,   /**< runs the PDS transaction itself (offer() is called) */
    PM_PART_RETRY,      /**< transient (frame in flight, work pending): offer again later */
    PM_PART_BUSY,       /**< cannot sleep in PDS now (e.g. BLE advertising) */
} pm_part_state_t;

/** Outcome of offering a window for PDS. */
typedef enum {
    PM_OFFER_REJECTED = 0, /**< window not usable: do not offer it again */
    PM_OFFER_GRANTED,      /**< an executor participant runs the PDS from its own task */
    PM_OFFER_RETRY,        /**< transient: the window stays valid, offer it again later */
    PM_OFFER_EXECUTED,     /**< the idle task ran (or attempted) the PDS; time is accounted */
    PM_OFFER_NONE,         /**< nobody can take a PDS now: nothing was offered */
} pm_offer_t;

/** No deadline (half-microseconds). */
#define PM_DEADLINE_NONE UINT32_MAX

typedef struct pm_participant {
    const char *name;
    /** Idle ticks hidden behind the driver's own short safety poll (0 if none).
     *  IRQs enabled. */
    TickType_t (*projected_idle_ticks)(void);
    /** Deadline to clamp a WFI window to, half-microseconds from now
     *  (PM_DEADLINE_NONE: none; the radio wakes the CPU from WFI itself). */
    uint32_t (*wfi_deadline_hus)(void);
    /** State for this window.  IRQs masked. */
    pm_part_state_t (*pds_state)(void);
    /** Latest time the CPU must be back and restored, half-microseconds from
     *  now (PM_DEADLINE_NONE: none).  IRQs masked. */
    uint32_t (*pds_deadline_hus)(void);
    /** EXECUTOR only: take the window (rtos_hus of RTOS idle budget; the other
     *  participants need the CPU back within part_deadline_hus).  IRQs masked. */
    pm_offer_t (*offer)(uint32_t rtos_hus, uint32_t part_deadline_hus);
    /** A projected window was not offered: forget per-window state. */
    void (*window_skipped)(void);
} pm_participant_t;

/** Register a radio participant (at most 4; @p p must stay valid). */
int pm_participant_register(const pm_participant_t *p);

/** Who runs a PDS transaction. */
typedef enum {
    PM_EXEC_NONE = 0,
    PM_EXEC_IDLE, /**< the idle task (ipro_pm) */
    PM_EXEC_BLE,  /**< the BLE controller task */
} pm_exec_id_t;

/**
 * The PDS transaction in progress, for ipro_pm device callbacks: valid from
 * suspend to resume_late.  resume_early may run without a sleep (an aborted
 * entry, a released grant): check @c slept.
 */
typedef struct {
    uint32_t generation;  /**< increments per transaction */
    pm_exec_id_t executor;
    uint32_t sleep_us;    /**< requested */
    uint32_t elapsed_us;  /**< measured on the corrected 32 kHz clock (0 if not slept) */
    uint32_t wake_reason; /**< HAL_LP_WAKEUP_* bits */
    uint32_t domain_policy; /**< PM_PDS_BLE_CTLR_DOMAIN_POLICY_* the PDS ran with */
    bool slept;           /**< the core really went through PDS31 */
} pm_txn_t;

const pm_txn_t *pm_txn_current(void);

/* ---- 32 kHz sleep clock, calibrated against the machine timer ---- */

/** Frequency error of the 32 kHz clock vs MTIME, ppm (positive: it runs fast). */
int32_t pm_lpclk_ppm(void);
/** Bound on the error of a sleep timed with pm_lpclk_ppm(), ppm. */
uint32_t pm_lpclk_bound_ppm(void);
/** 32 kHz ticks for @p us of MTIME time. */
uint64_t pm_lpclk_us_to_ticks(uint64_t us);
/** MTIME microseconds for @p ticks of the 32 kHz clock. */
uint64_t pm_lpclk_ticks_to_us(uint64_t ticks);
/** Feed an external measurement (e.g. a BLE anchor): the 32 kHz clock ran
 *  @p err_ppm fast over a span of @p span_us. */
void pm_lpclk_feedback(int32_t err_ppm, uint32_t span_us);

/* ---- the BLE link, for another radio lining its periodic work up ---- */

/** The connection interval (us) of the BLE link while it is in the PDS
 *  profile (the controller runs the PDS once per interval); 0 otherwise or
 *  without BLE. */
uint32_t ipro_pm_ble_link_interval_us(void);
/** When the BLE controller last published a full window, i.e. about when a
 *  connection event ended (MTIME us, low 32 bits); 0 before any. */
uint32_t ipro_pm_ble_last_window_us(void);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_PM_RADIO_H_ */
