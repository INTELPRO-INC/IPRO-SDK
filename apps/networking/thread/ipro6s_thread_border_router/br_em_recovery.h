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
 * @file br_em_recovery.h
 * @brief Software workaround for the cold-reset BLE EM self-test failure.
 *
 * When btdm_init() fails (typically `[BTDM_EMTEST] rc=-1`), the BR used to
 * return from main() before the scheduler started and sit dead until an
 * external reset. Field evidence (2026-09-19) shows the next reset almost
 * always passes, so retry by software reboot, bounded by a counter kept in
 * HBN RAM (hal_hbnram_sysinfo->reserved[2]); the counter survives warm
 * resets and is cleared on POR and on a successful controller start.
 * The hardware root cause (EM aperture / clock-enable timing) is tracked
 * separately; this only turns a hang into a bounded restart.
 */
#ifndef BR_EM_RECOVERY_H
#define BR_EM_RECOVERY_H

#include <stdint.h>

/** Consecutive software reboots allowed before giving up and halting. */
#ifndef BR_EM_RECOVERY_MAX_REBOOTS
#define BR_EM_RECOVERY_MAX_REBOOTS 5u
#endif

/**
 * Called after br_ble_controller_prepare() fails.
 * Reboots the board (does not return) while the retained attempt counter is
 * below BR_EM_RECOVERY_MAX_REBOOTS; otherwise logs the exhausted state,
 * leaves the counter for post-mortem and returns so the caller can halt.
 * @param rc the btdm_init() return code, for the log line.
 * @return number of reboots already spent (only when giving up).
 */
uint32_t br_em_recovery_on_failure(int rc);

/** Called after a successful controller start: clears the retained counter. */
void br_em_recovery_on_success(void);

/** Retained consecutive-failure count (0 when none); for shell/diagnostics. */
uint32_t br_em_recovery_count(void);

#endif /* BR_EM_RECOVERY_H */
