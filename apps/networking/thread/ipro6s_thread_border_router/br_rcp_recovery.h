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
 * @file br_rcp_recovery.h
 * @brief BR policy when OpenThread stops on a fatal RCP error.
 *
 * When the RCP stops answering (or answers garbage) and OpenThread's own
 * recovery gives up, ot_rcp_fatal() stops the stack (ot_rcp_health.h) and
 * calls ot_rcp_on_fatal(), implemented here: stop taking LAN traffic for
 * OpenThread (and free what was queued for it), withdraw the Thread routes
 * from the LAN and reboot the BR, which re-runs the whole RCP bring-up (with a
 * platform reset of the RCP, THREAD_RCP_BOOT_PLATFORM_RESET). The schedule is
 * bounded and backed off so a dead RCP does not keep the BR rebooting quickly;
 * Wi-Fi and the shell stay up between attempts. Consecutive stops are counted
 * in HBN RAM (hal_hbnram_sysinfo->reserved[1]; reserved[2] is
 * br_em_recovery's), which survives the software reboot; the count clears
 * after Thread has run for BR_RCP_HEALTHY_CLEAR_S. An incompatible RCP image
 * (exit code 3) is not rebooted for: Thread stays down until someone flashes
 * the RCP and reboots.
 */
#ifndef BR_RCP_RECOVERY_H
#define BR_RCP_RECOVERY_H

#include <stdbool.h>
#include <stdint.h>

/** Stops rebooted after BR_RCP_FAST_DELAY_S; later ones back off. */
#define BR_RCP_FAST_REBOOTS 3u
#define BR_RCP_FAST_DELAY_S 5u
/** First backed-off delay, doubled per further stop up to BR_RCP_MAX_DELAY_S. */
#define BR_RCP_SLOW_DELAY_S 60u
#define BR_RCP_MAX_DELAY_S 3600u
/** Running time after which the stop count is cleared. */
#define BR_RCP_HEALTHY_CLEAR_S 600u

/**
 * Delay before the reboot for the count-th consecutive stop (count >= 1).
 * A stop outside the OpenThread task always gets the fast delay: that task
 * was parked with its own locks (BLE/LAN management, CHIP).
 */
uint32_t br_rcp_recovery_delay_s(uint32_t count, bool in_ot_task);

/** At boot: report a retained stop count. */
void br_rcp_recovery_boot(void);

/** OpenThread is up (OpenThread task): clear the count once it has stayed up long enough. */
void br_rcp_recovery_on_ready(void);

/** A bench console took UART1 over (rcpcon): record stops, never reboot. */
void br_rcp_recovery_hold(void);

/** Retained consecutive-stop count (0 when none). */
uint32_t br_rcp_recovery_count(void);

#endif /* BR_RCP_RECOVERY_H */
