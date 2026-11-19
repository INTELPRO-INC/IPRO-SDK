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

#pragma once
#include <stdbool.h>
#include <openthread/error.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ipro_matter_br_is_network_worker(void);

/* Terminal reset-worker operation AFTER the CHIP loop has stopped/drained.
 * Application admission must already be closed and fail-safe rollback finished.
 * Drivers must drain CHIP-dependent notifications before stopping that loop;
 * this final SDK fence cannot service a stopped Matter event loop.
 * Rejects ISR, stopped scheduler,
 * OT worker and uninitialized OT. First valid caller owns all future retries
 * and must remain alive until reboot, including BUSY/error returns.
 * First closes/drains the SDK tracked SCAN operation. A timed-out or uncertain
 * scan remains BUSY until its worker and pending completion are resolved; this
 * does not fence unrelated legacy scans. Then permanently closes manager/fhost
 * CONNECT admission and drains its callbacks
 * and requests first, then confirms supplicant autoconnect disabled. Then
 * closes Wi-Fi profile writes, waits for completion,
 * and nonblocking-
 * acquires the one SDK OT mutex. BUSY requires retry by that same task.
 * Success retains the mutex until reboot, even if subsequent deletion fails:
 * never delete the caller task or restart CHIP/network activity afterward.
 * This does not stop all Wi-Fi writers (direct WPA/autoreconnect/DHCP/lwIP),
 * gracefully withdraw networks, or delete data. Their lifecycle remains the
 * SDK owner's responsibility; CONNECT drain alone is not radio quiescence.
 */
otError ipro_matter_br_reset_fence(void);

#ifdef __cplusplus
}
#endif
