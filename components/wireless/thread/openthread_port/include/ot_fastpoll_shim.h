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
 * @file ot_fastpoll_shim.h
 * @brief C-callable bridge for SED fast-poll path into OT internals
 *
 * The SED fast-poll module (sed_fastpoll.c) needs OT-side state that has
 * no public OT C API:
 *   - current MAC key bytes (KeyManager::GetCurrentMacKey())
 *   - current 802.15.4 MAC frame counter (Get154MacFrameCounter)
 *   - ability to commit a new frame counter back (MacFrameCounterUsed)
 *   - current key sequence (for key-rotation detection)
 *
 * This header exposes a minimal C ABI. The OT external lock (ot_extLock /
 * otrLock/otrUnlock) is a NON-recursive FreeRTOS mutex, and the OT task
 * holds it across tasklet + driver processing (ot_freertos.c around the
 * `otTaskletsProcess` / `otSysProcessDrivers` calls). State-changed
 * callbacks and tasklets therefore run with the lock already held.
 *
 * **Locking contract**: shim functions DO NOT take the lock — callers
 * must hold it.
 *   - From inside an OT state-changed callback / tasklet / OT public API
 *     callback: lock is already held by the OT task. Call directly.
 *   - From any other task (shell command, periodic SED task, etc.): wrap
 *     the call with otrLock() / otrUnlock().
 *
 * Plan A reference: .claude/plans/sed-fastpoll-bypass-ot-mac.md
 */

#ifndef OT_FASTPOLL_SHIM_H_
#define OT_FASTPOLL_SHIM_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Atomic snapshot of all OT state needed to build a secured Data Request. */
typedef struct {
    /* Network identity */
    uint16_t pan_id;
    uint16_t our_short;
    uint8_t  our_ext[8];       /* OT m8[] order (MSB first / display order) —
                                * matches 802.15.4-2015 §9.3.1 CCM* nonce
                                * convention; if PSDU writers need wire-order
                                * (LSB first) they must reverse. */

    /* Parent (set only when child role + parent kStateValid) */
    bool     parent_valid;
    uint16_t parent_short;
    uint8_t  parent_ext[8];    /* same byte order as our_ext */

    /* 802.15.4 link key + sequence (literal key mode only) */
    uint8_t  mac_key[16];
    uint32_t key_sequence;     /* increments on each key rotation */
    uint8_t  key_id;           /* current key index per Thread spec (1..255) */

    /* Frame counter — next value to use; commit with ot_fastpoll_commit_counter */
    uint32_t mac_frame_counter;

    /* Radio channel from OT's PAN channel */
    uint8_t  channel;
} ot_fastpoll_state_t;

/** Returns 0 on success, negative on failure (e.g. OT not initialised).
 *  Caller MUST hold the OT lock (see locking contract above). */
int ot_fastpoll_snapshot(ot_fastpoll_state_t *out);

/** Inform OT that we've used MAC frame counter values up to and including
 *  @p used_counter. Internally calls KeyManager::MacFrameCounterUsed +
 *  SubMac::SetFrameCounter so OT-initiated TXes don't reuse our counters.
 *  Caller MUST hold the OT lock. Returns 0 on success. */
int ot_fastpoll_commit_counter(uint32_t used_counter);

/** True when the fast-path is allowed to TX: device is attached as a child
 *  and parent neighbour entry is in kStateValid.
 *  Caller MUST hold the OT lock. */
bool ot_fastpoll_is_ready(void);

/** Stop OT's internal DataPollSender so it no longer drives data poll TX
 *  cadence (including the kFastPollPeriod 188 ms burst mode that
 *  `otLinkSetPollPeriod()` cannot suppress). After this returns, the SED
 *  fast-path engine is the only entity emitting Data Request frames.
 *
 *  MLE keep-alive, Child Update Request, and IPv6 TX still go through the
 *  normal OT TX path — only the periodic background poll is silenced.
 *
 *  Caller MUST hold the OT lock. */
void ot_fastpoll_stop_ot_poll_sender(void);

/** Re-enable OT's internal DataPollSender. Used when the fast-path engine
 *  is disabled and OT should resume owning the poll cadence. Caller MUST
 *  hold the OT lock. */
void ot_fastpoll_start_ot_poll_sender(void);

#ifdef __cplusplus
}
#endif

#endif /* OT_FASTPOLL_SHIM_H_ */
