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

#include "br_matter_wifi.h"
#include <ipro_matter_wifi.h>
#include <FreeRTOS.h>
#include <task.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <wifi_mgmr.h>
#include <fhost.h>
#include <net_al.h>
#include <string.h>

static bool sReady;
static bool sEnabled;
static bool sExhausted;
static ipro_wifi_connectivity_snapshot sSnapshot;

static bool task_context(void)
{
    return !xPortIsInsideInterrupt() && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING;
}

int br_matter_wifi_init(void)
{
    if (!task_context()) return -1;
    LOCK_TCPIP_CORE();
    taskENTER_CRITICAL();
    sReady = true;
    taskEXIT_CRITICAL();
    UNLOCK_TCPIP_CORE();
    return 0;
}

int br_matter_wifi_set_enabled(bool enabled)
{
    if (!task_context()) return -1;
    taskENTER_CRITICAL();
    if (!sReady) {
        taskEXIT_CRITICAL();
        return -1;
    }
    sEnabled = enabled;
    taskEXIT_CRITICAL();
    return 0;
}

int ipro_wifi_get_station_mac(uint8_t output[6])
{
    if (!output) return -1;
    memset(output, 0, 6);
    if (!task_context()) return -1;
    taskENTER_CRITICAL();
    const bool ready = sReady;
    taskEXIT_CRITICAL();
    if (!ready) return -1;

    uint8_t mac[6] = {0};
    LOCK_TCPIP_CORE();
    taskENTER_CRITICAL();
    struct netif *sta = fhost_to_net_if(MGMR_VIF_STA);
    if (sta && sta->hwaddr_len == sizeof(mac))
        memcpy(mac, sta->hwaddr, sizeof(mac));
    taskEXIT_CRITICAL();
    UNLOCK_TCPIP_CORE();
    uint8_t any = 0;
    for (unsigned i = 0; i < sizeof(mac); ++i) any |= mac[i];
    if (!any || (mac[0] & 1u)) return -1;
    memcpy(output, mac, sizeof(mac));
    return 0;
}

int ipro_wifi_connectivity_read(ipro_wifi_connectivity_snapshot * output)
{
    if (!output) return -1;
    memset(output, 0, sizeof(*output));
    if (!task_context()) return -1;
    /* A read before init must not touch an uninitialized tcpip mutex. */
    taskENTER_CRITICAL();
    const bool ready = sReady && !sExhausted;
    taskEXIT_CRITICAL();
    if (!ready) return -1;

    /* Lock order is core -> short single-core critical section. No waiting,
     * network calls, credentials copying or flash access while critical.
     * Published fields are one normalized sample, not an event history: state
     * writers may be between association and netif updates when sampled. */
    LOCK_TCPIP_CORE();
    taskENTER_CRITICAL();
    if (!sReady || sExhausted) {
        taskEXIT_CRITICAL();
        UNLOCK_TCPIP_CORE();
        return -1;
    }
    ipro_wifi_connectivity_snapshot next = {0};
    next.ready = 1;
    next.enabled = sEnabled;
    next.provisioned = wifiMgmr.sta_connect_param.ssid_len > 0 &&
                       wifiMgmr.sta_connect_param.ssid_len <= sizeof(wifiMgmr.sta_connect_param.ssid);
    next.associated = sEnabled && wifi_mgmr_sta_state_get();
    struct netif * sta = fhost_to_net_if(MGMR_VIF_STA);
    if (next.associated && sta && netif_is_up(sta) && netif_is_link_up(sta)) {
        next.ipv4_assigned = !ip4_addr_isany_val(*netif_ip4_addr(sta));
        for (unsigned i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
            if (ip6_addr_isvalid(netif_ip6_addr_state(sta, i)) && !ip6_addr_isany(netif_ip6_addr(sta, i))) {
                next.ipv6_assigned = 1;
                break;
            }
        }
    }
    const bool changed = next.ready != sSnapshot.ready || next.enabled != sSnapshot.enabled ||
        next.provisioned != sSnapshot.provisioned || next.associated != sSnapshot.associated ||
        next.ipv4_assigned != sSnapshot.ipv4_assigned || next.ipv6_assigned != sSnapshot.ipv6_assigned;
    if (changed && sSnapshot.revision == UINT64_MAX) {
        sExhausted = true;
        taskEXIT_CRITICAL();
        UNLOCK_TCPIP_CORE();
        return -1;
    }
    next.revision = sSnapshot.revision + (changed ? 1u : 0u);
    sSnapshot = next;
    *output = next;
    taskEXIT_CRITICAL();
    UNLOCK_TCPIP_CORE();
    return 0;
}
