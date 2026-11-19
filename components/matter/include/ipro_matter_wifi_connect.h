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

#ifdef __cplusplus
extern "C" {
#endif

/* Fixed-width in-process C ABI, not a serialized/network format. No native
 * SDK structs, enum parameters, callbacks or credential pointers cross it.
 * Personal WPA profiles only: nonempty SSID without NUL; printable key 8..63
 * bytes or 64 hex digits. security: 0=manager default, 1=WPA2, 2=WPA3; pmf:0..2.
 * Open/enterprise networks are not implemented by this profile path.
 */
typedef struct ipro_wifi_connect_request {
    uint8_t ssid[32];
    uint8_t key[64];
    uint32_t timeout_ms; /* Positive <= INT32_MAX; not a whole-operation deadline. */
    uint8_t ssid_length, key_length, security, pmf;
} ipro_wifi_connect_request;
#define IPRO_WIFI_CONNECT_REQUEST_SIZE 104u

#define IPRO_WIFI_CONNECT_PREPARATION_FAILED 0u
#define IPRO_WIFI_CONNECT_PREPARATION_TIMEOUT 1u
#define IPRO_WIFI_CONNECT_TASK_FAILED 2u
#define IPRO_WIFI_CONNECT_FAILED 3u
#define IPRO_WIFI_CONNECT_CONFIGURED 4u
#define IPRO_WIFI_CONNECT_CONNECTED 5u
#define IPRO_WIFI_CONNECT_NOT_CONNECTED 6u

/* Call from serialized driver task context, with no OT/lwIP/critical locks.
 * Driver must separately own live Wi-Fi configuration and persistent profiles;
 * these APIs do not claim ownership, implement rollback or persist credentials.
 * Request and output must not overlap; request stays immutable during the call.
 * Start: 0=admitted/ID set, 1=busy, -1=invalid/rejected; ID zero on failure.
 * Copies credentials into SDK; temporary adapter copies are wiped, not caller
 * or manager-owned copies. Caller must wipe its own request when finished.
 */
int ipro_wifi_connect_start(const ipro_wifi_connect_request *request, uint32_t request_size,
                           uint32_t *operation);
/* Take: 0=result consumed once, 1=pending, -1=invalid/closed. Output zero when
 * unavailable. CONNECTED is a WPA snapshot, not DHCP/IPv6 or Matter readiness. */
int ipro_wifi_connect_take(uint32_t operation, uint32_t *result);
#define IPRO_WIFI_DISCONNECT_SUCCEEDED 0u
#define IPRO_WIFI_DISCONNECT_FAILED 1u

/* Live asynchronous network disable, sharing the CONNECT operation slot.
 * Consume prior CONNECT results first. Start: 0=queued/ID set, 1=busy,
 * -1=rejected; ID zero on failure. Take: 0=consumed, 1=pending, -1=invalid;
 * result zero unless consumed. Only a consumed SUCCEEDED means command success.
 * Worker pauses manager/native CONNECT, drains accepted work, disables automatic
 * reconnect and the WPA network, then releases the pause before completion.
 * Does not remove the interface, erase credentials, restore a stored profile,
 * drain scans/DHCP or exclude direct WPA/IPC writers. Caller must own those
 * paths separately. No deadline/forced task deletion: a stuck WPA request or
 * lease release remains pending. connect_close/drain also wait for this work.
 * Same task/context and ownership requirements as connect_start. */
int ipro_wifi_disconnect_start(uint32_t *operation);
int ipro_wifi_disconnect_take(uint32_t operation, uint32_t *result);
/* Close only this operation slot, terminally; 1=outstanding, 0=mailbox drained,
 * -1=wrong context. Does not cancel the connection or wait for worker cleanup. */
int ipro_wifi_connect_close(void);
/* Final reset stage AFTER fail-safe rollback: permanently closes all manager
 * CONNECT/autoconnect-enable admission and polls worker+mailbox cleanup and
 * supplicant autoconnect disable. 1=busy,0=drained,-1=error (retry, not reopen).
 * Unresponsive WPA remains busy indefinitely on a dedicated worker. This does
 * not disconnect/drain DHCP/lwIP or make all network storage reset-safe. */
int ipro_wifi_connect_drain(void);

#ifdef __cplusplus
}
#endif
