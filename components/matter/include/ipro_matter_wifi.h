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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Required SDK-owned bridge; no fallback implementations. No credentials or
 * netif pointers cross this boundary. Read returns 0 only for an atomic snapshot.
 * revision is nonzero and increases on each different published sample;
 * this sampled interface does not preserve every transient network edge;
 * it must never wrap/restart during this Matter manager's lifetime.
 * ready describes snapshot infrastructure, NOT radio readiness: after attachment
 * it stays 1 across Wi-Fi shutdown, with enabled/associated/IP flags cleared. */
typedef struct ipro_wifi_connectivity_snapshot
{
    uint64_t revision;
    uint8_t ready;
    uint8_t enabled;
    uint8_t provisioned;
    uint8_t associated;
    uint8_t ipv4_assigned;
    uint8_t ipv6_assigned;
} ipro_wifi_connectivity_snapshot;

int ipro_wifi_connectivity_read(ipro_wifi_connectivity_snapshot * output);
/* Copy the existing STA netif MAC (six bytes). Task-only, same lock-order
 * contract as snapshot reads. No association required. Invalid/unavailable
 * identity fails and zeros the output; this never creates or changes a MAC. */
int ipro_wifi_get_station_mac(uint8_t output[6]);
/* True only in task context serialized by the CHIP stack lock/event loop;
 * false in ISR and foreign unprotected tasks. Callers must not hold the OT
 * mutex or lwIP core lock when entering this bridge (CHIP -> lwIP lock order). */
bool ipro_wifi_connectivity_in_chip_context(void);

#define IPRO_WIFI_LINK_SNAPSHOT_SIZE 42u
/* Fixed byte-only ABI. Last WPA link observation, not target credentials and
 * not a DHCP/IP-ready indication. observed=0 means no WPA observation yet;
 * observed=1,connected=0 means a down/init/deinit event was published.
 * connected=1,identity_valid=0 means WPA completed but association identity was
 * missing/invalid; never infer an SSID from staged/manager configuration then.
 * SSID is binary and length-delimited. No credentials or borrowed pointers. */
typedef struct {
    uint8_t observed, connected, identity_valid, ssid_length;
    uint8_t ssid[32], bssid[6];
} ipro_wifi_link_snapshot;
/* Task + CHIP stack lock required. Atomic copy of the most recent observation,
 * not an event history or atomic with a separate connectivity/IP snapshot.
 * 0=sample copied (possibly not yet observed), -1=bad context/input. Valid-size
 * output is cleared on error; a size mismatch leaves the buffer untouched.
 * Needs the source-built WPA observation hook; no cached-target fallback. */
int ipro_wifi_link_read(ipro_wifi_link_snapshot *output, uint32_t size);

#ifdef __cplusplus
}
#endif
