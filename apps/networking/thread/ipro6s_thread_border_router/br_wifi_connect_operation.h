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
#include <stdint.h>
#include <wifi_mgmr_ext.h>

/* SDK-native boundary: do not pass wifi_mgmr structs across Matter's enum ABI.
 * All APIs are task-only, caller holds no OT/lwIP/critical locks. A single static
 * slot owns callback state until consumed; IDs never wrap/reuse before reboot.
 * This module owns neither persistent profiles nor all live Wi-Fi writers.
 * Driver integration must acquire those separately before enabling mutation.
 */
int br_wifi_connect_operation_start(const wifi_mgmr_sta_connect_params_t *config,
                                  uint32_t *operation);
/* Start: 0=admitted (ID set), 1=slot busy, -1=rejected. Config is copied by mgmr
 * before return. Callback never touches caller memory or calls Matter. */
/* Take: 0=one result consumed, 1=pending, -1=invalid/closed; output zero on error.
 * Consumer must run in its serialized Matter context. Result is a worker
 * outcome, not IP readiness; consult connectivity snapshot separately. */
int br_wifi_connect_operation_take(uint32_t operation, uint32_t *result);
/* Asynchronous network disable for live rollback. Shares the CONNECT slot:
 * consume any prior result first; start returns 0=queued, 1=busy, -1=rejected.
 * A worker holds a temporary manager/native CONNECT pause, waits for accepted
 * work to drain, disables auto-reconnect and the WPA network, then releases the
 * pause. Take returns 0=consumed, 1=pending, -1=invalid; result 0=success, 1=error.
 * This does NOT erase profiles, remove the VIF, drain scans/DHCP or fence direct
 * WPA/IPC writers. Caller must separately own live configuration. No fabricated
 * timeout: a stuck WPA command or lease release keeps this pending. Close/drain
 * also wait for this worker; close can skip work not yet begun, not cancel WPA.
 * Success is command completion, not a guarantee against future reconnection. */
int br_wifi_disconnect_operation_start(uint32_t *operation);
int br_wifi_disconnect_operation_take(uint32_t operation, uint32_t *result);
/* Terminal admission close, discards queued result. 1 means submission or its
 * callback is still outstanding; poll again. 0 means this mailbox is drained,
 * NOT that the worker has deleted itself or that Wi-Fi is disconnected/quiescent.
 * This never cancels a connection. Reset/external task deletion may prevent
 * callback delivery indefinitely; no false drained result is returned. */
int br_wifi_connect_operation_close(void);

/* Final reset stage only, AFTER fail-safe rollback: permanently closes all
 * manager/fhost CONNECT admission, then polls mailbox plus worker cleanup.
 * 1=busy, 0=those paths drained and supplicant autoconnect disabled,
 * -1=wrong context or disable-command failure (retry, never reopen admission).
 * The legacy disable command runs in a dedicated worker; polling never waits
 * for its response. A nonresponding supplicant leaves this BUSY indefinitely,
 * not falsely drained. No queued request is freed on a fabricated timeout.
 * Unlike close(), no more
 * rollback connections may start anywhere through manager/fhost until reboot.
 * Does not cancel/disconnect an already running association or drain DHCP;
 * other Wi-Fi writers/lwIP must be fenced
 * separately before declaring the radio or persistent storage reset-safe. */
int br_wifi_connect_operation_drain(void);
