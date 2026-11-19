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

#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/ip6.h"
#include "lwip/inet_chksum.h"
#include "lwip/mld6.h"
#include "lwip/timeouts.h"
#include "fhost.h"
#include "wifi_mgmr_ext.h"
#include "openthread_port.h"
#include <openthread/border_routing.h>
#include <openthread/backbone_router_ftd.h>
#include <openthread/srp_server.h>
#include <openthread/platform/infra_if.h>
#include "ipro_log.h"
#include "br_infra.h"
#include "br_nd.h"
#include "br_mdns.h"
#include "br_matter_mdns.h"
#include "br_multicast.h"
#include "br_dua_proxy.h"
#include "br_domain.h"
#include "br_trace.h"
#include "br_dhcp6_pd.h"
#include "br_nat64.h"
#include "br_dns_upstream.h"
#include "br_wifi_link_epoch.h"
#include "br_thread_netif.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define ND_QUEUE_DEPTH 4
struct nd_event {
    uint32_t index;
    uint16_t length;
    uint16_t offset;
    uint8_t packet[1500];
};
static StaticQueue_t sQueueControl;
static uint8_t sQueueStorage[ND_QUEUE_DEPTH * sizeof(struct nd_event *)];
static QueueHandle_t sQueue;
static StaticTimer_t sTimerStorage;
static uint32_t sIndex;
static uint32_t sInfraEpoch;
static bool sRunning;
static bool sEnabled;
static bool sSrpConfigured;
/* lwIP core lock protects these counters. Direct ICMPv6 RS packets only;
 * extension-header traffic still follows the normal validator below. */
static struct {
    uint32_t seen, queued, invalid, queue_full, no_memory;
} sRsStats;
#ifdef IPRO_MATTER_PACKAGE_ACTIVE
static bool sPdConfigured;
#endif

static void poll_timer(TimerHandle_t timer)
{
    (void)timer;
    otSysEventSignalPending();
}

int br_infra_prepare(void)
{
    sQueue = xQueueCreateStatic(ND_QUEUE_DEPTH, sizeof(struct nd_event *),
                               sQueueStorage, &sQueueControl);
    TimerHandle_t timer = xTimerCreateStatic("br_link", pdMS_TO_TICKS(1000),
                                            pdTRUE, NULL, poll_timer, &sTimerStorage);
    return sQueue && timer && xTimerStart(timer, 0) == pdPASS ? 0 : -1;
}

/* Runs in lwIP's core context. Copy only; all OT calls belong to its task.
 * Return zero so lwIP still processes its own neighbour/address state. */
int br_wifi_ip6_input(struct pbuf *p, struct netif *netif)
{
    if (netif == fhost_to_net_if(MGMR_VIF_STA)) br_trace_pbuf(BR_TRACE_WIFI_IN, p, 0);
    if (otrIsStopped()) return 0; /* nothing drains the queues below any more */
    br_multicast_wifi_input(p, netif);
    br_dua_wifi_input(p, netif);
    if (!sQueue || netif != fhost_to_net_if(MGMR_VIF_STA) ||
        p->tot_len < 48 || p->tot_len > 1500) return 0;
    uint8_t next = pbuf_get_at(p, 6);
    if (next != 58 && next != 0 && next != 60) return 0;
    if (next == 58 && (pbuf_get_at(p, 40) < 133 || pbuf_get_at(p, 40) > 137)) return 0;
    bool direct_rs = next == 58 && pbuf_get_at(p, 40) == 133;
    if (direct_rs) ++sRsStats.seen;
    if (uxQueueSpacesAvailable(sQueue) == 0) {
        if (direct_rs) ++sRsStats.queue_full;
        return 0;
    }
    struct nd_event *event = pvPortMalloc(sizeof(*event));
    if (!event) {
        if (direct_rs) ++sRsStats.no_memory;
        return 0;
    }
    if (pbuf_copy_partial(p, event->packet, p->tot_len, 0) != p->tot_len) {
        vPortFree(event);
        return 0;
    }
    int offset = br_nd_validate(event->packet, p->tot_len);
    if (!offset) {
        if (direct_rs) ++sRsStats.invalid;
        vPortFree(event);
        return 0;
    }
    event->index = netif_get_index(netif);
    event->offset = (uint16_t)offset;
    event->length = 40 + ((uint16_t)event->packet[4] << 8) + event->packet[5];
    if (xQueueSend(sQueue, &event, 0) != pdTRUE) {
        if (direct_rs) ++sRsStats.queue_full;
        vPortFree(event);
    } else {
        if (direct_rs) ++sRsStats.queued;
        otSysEventSignalPending();
    }
    return 0;
}

void br_infra_drop_queued(void)
{
    struct nd_event *event;
    while (sQueue && xQueueReceive(sQueue, &event, 0) == pdTRUE) vPortFree(event);
}

void br_infra_process(otInstance *instance, bool forwarding_ready)
{
    if (!sSrpConfigured) {
        /* Follow BR readiness/withdrawal; do not serve before OMR publication. */
        otSrpServerSetAutoEnableMode(instance, true);
        sSrpConfigured = true;
    }
    bool running = false;
    uint32_t index = 0;
#if defined(CONFIG_MATTER_ENABLE) && CONFIG_MATTER_ENABLE
    uint8_t station_mac[6] = {0};
#endif
    LOCK_TCPIP_CORE();
    struct netif *netif = fhost_to_net_if(MGMR_VIF_STA);
    uint32_t epoch = br_wifi_link_epoch();
    if (netif) {
#if defined(CONFIG_MATTER_ENABLE) && CONFIG_MATTER_ENABLE
        if (netif->hwaddr_len == sizeof(station_mac))
            memcpy(station_mac, netif->hwaddr, sizeof(station_mac));
#endif
        index = netif_get_index(netif);
        running = epoch && netif_is_up(netif) && netif_is_link_up(netif) &&
                  ip6_addr_isvalid(netif_ip6_addr_state(netif, 0)) &&
                  ip6_addr_islinklocal(netif_ip6_addr(netif, 0));
        if (running && (!sIndex || sIndex == index)) {
            ip6_addr_t routers;
            ip6addr_aton("ff02::2", &routers);
            ip6_addr_assign_zone(&routers, IP6_MULTICAST, netif);
            if (!mld6_lookfor_group(netif, &routers) &&
                mld6_joingroup_netif(netif, &routers) != ERR_OK) running = false;
        }
    }
    UNLOCK_TCPIP_CORE();
    if (sIndex && index != sIndex) running = false; /* No implicit interface rebinding. */
    /* WPA observes even a disconnect/reconnect entirely between driver polls.
     * First withdraw/reset native state on the old link, then allow the normal
     * up path to start fresh. Adapter TX/RX refuses the new epoch meanwhile. */
    if (sRunning && epoch != sInfraEpoch) {
        if (otPlatInfraIfStateChanged(instance, sIndex, false) == OT_ERROR_NONE)
            sRunning = false;
        else running = false;
    }
    if (!sRunning) sInfraEpoch = epoch;
    if (!sIndex && index && running) {
        otError error = OT_ERROR_NONE;
#if defined(CONFIG_MATTER_ENABLE) && CONFIG_MATTER_ENABLE
        /* BorderRoutingInit can auto-enable mDNS; identity must precede it. */
        error = br_matter_mdns_prepare(instance, station_mac);
#endif
        if (error == OT_ERROR_NONE) error = otBorderRoutingInit(instance, index, running);
        if (error == OT_ERROR_NONE) {
            sIndex = index;
            sRunning = running;
            IPRO_LOGI("BR-Infra", "Initialized Wi-Fi ifindex=%u",
                      (unsigned)index);
        } else IPRO_LOGE("BR-Infra", "Initialization failed: %d", error);
    } else if (sIndex && sRunning != running) {
        if (otPlatInfraIfStateChanged(instance, sIndex, running) == OT_ERROR_NONE)
            sRunning = running;
    }
    if (sIndex && sEnabled != forwarding_ready) {
        otError error = otBorderRoutingSetEnabled(instance, forwarding_ready);
        if (error == OT_ERROR_NONE) {
            sEnabled = forwarding_ready;
            IPRO_LOGI("BR-Infra", "Routing manager enabled=%u", (unsigned)sEnabled);
        } else IPRO_LOGE("BR-Infra", "Routing manager state failed: %d", error);
    }
#ifdef IPRO_MATTER_PACKAGE_ACTIVE
    if (sIndex && !sPdConfigured) {
        otBorderRoutingDhcp6PdSetEnabled(instance, true);
        sPdConfigured = true;
    }
#endif
#if defined(IPRO_MATTER_PACKAGE_ACTIVE) || defined(CONFIG_THREAD_BR_AUTOMATIC_BACKBONE)
    /* Automatic backbone policy is independent of a local Matter server.
     * Standalone CLI builds can leave it off for manual BBR control.
     * Do not force Primary or restart Secondary elections.
     * STOPPED includes temporary Thread detach: OT Local::Reset owns that
     * transition and retains Enabled. Actual infrastructure/forwarding loss
     * disables BBR and triggers the core's service/listener withdrawal. */
    otBorderRoutingState routing_state = otBorderRoutingGetState(instance);
    bool backbone_ready = sIndex && sIndex == index && running && sRunning &&
                          forwarding_ready && sEnabled &&
                          (routing_state == OT_BORDER_ROUTING_STATE_STOPPED ||
                           routing_state == OT_BORDER_ROUTING_STATE_RUNNING);
    bool backbone_enabled = otBackboneRouterGetState(instance) != OT_BACKBONE_ROUTER_STATE_DISABLED;
    if (backbone_ready != backbone_enabled) {
        otBackboneRouterSetEnabled(instance, backbone_ready);
        IPRO_LOGI("BR-Infra", "Backbone Router enabled=%u", (unsigned)backbone_ready);
    }
#ifdef IPRO_THREAD_BR_DOMAIN_PREFIX
    br_domain_process(instance, backbone_ready);
#endif
#endif
    br_dhcp6_pd_process(instance);
#if defined(CONFIG_THREAD_BR_UPSTREAM_DNS) && CONFIG_THREAD_BR_UPSTREAM_DNS
    br_dns_upstream_process(instance, sRunning && sEnabled && forwarding_ready);
#endif
#if defined(CONFIG_THREAD_BR_NAT64) && CONFIG_THREAD_BR_NAT64
    br_nat64_process(instance, sRunning && sEnabled && forwarding_ready);
#endif
    br_mdns_process(instance, sIndex, sRunning);
    br_multicast_process(instance, sIndex, sRunning && forwarding_ready);
    br_dua_process(instance, sIndex, sRunning && forwarding_ready);
    struct nd_event *event;
    /* Bound work per driver iteration so ingress cannot starve radio/timers. */
    for (unsigned count = 0; count < ND_QUEUE_DEPTH &&
         xQueueReceive(sQueue, &event, 0) == pdTRUE; ++count) {
        if (sRunning && event->index == sIndex) {
            otIp6Address source;
            memcpy(source.mFields.m8, event->packet + 8, 16);
            otPlatInfraIfRecvIcmp6Nd(instance, sIndex, &source,
                                   event->packet + event->offset, event->length - event->offset);
        }
        vPortFree(event);
    }
}

bool otPlatInfraIfHasAddress(uint32_t index, const otIp6Address *address)
{
    bool found = false;
    if (!address || index == 0 || index > 255) return false;
    LOCK_TCPIP_CORE();
    struct netif *netif = netif_get_by_index((uint8_t)index);
    if (netif) for (unsigned i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
            memcmp(netif_ip6_addr(netif, i)->addr, address->mFields.m8, 16) == 0) found = true;
    }
    UNLOCK_TCPIP_CORE();
    return found;
}

/* Header and Route Information Options of the last multicast RA OpenThread
 * sent, kept so its routes can be withdrawn if OpenThread stops for good
 * (br_infra_withdraw_routes). RIOs are 8, 16 or 24 bytes (RFC 4191). */
#define RA_HEADER 16u
#define RA_MAX_RIOS 8u
static uint8_t sLastRa[RA_HEADER + RA_MAX_RIOS * 24u];
static uint16_t sLastRaLength;
static uint32_t sLastRaIndex;
/* The withdrawal goes out RA_WITHDRAW_SENDS times, RA_WITHDRAW_GAP_MS apart
 * (RFC 4861 MIN_DELAY_BETWEEN_RAS is 3 s): one lost multicast would leave LAN
 * hosts routing the Thread prefixes to this BR for the RIO lifetime (30 min). */
#define RA_WITHDRAW_SENDS 3u
#define RA_WITHDRAW_GAP_MS 4000u
static const otIp6Address kAllNodes = {.mFields.m8 = {0xff, 0x02, [15] = 0x01}};
static unsigned sWithdrawLeft;

static otError send_icmp6_nd_locked(uint32_t index, const otIp6Address *destination,
                                    const uint8_t *buffer, uint16_t length);

static void remember_ra_locked(uint32_t index, const uint8_t *ra, uint16_t length)
{
    uint16_t out = RA_HEADER;
    if (length < RA_HEADER) return;
    memcpy(sLastRa, ra, RA_HEADER);
    for (uint16_t off = RA_HEADER; off + 8u <= length;) {
        uint16_t len = (uint16_t)(ra[off + 1] * 8u);
        if (len == 0 || off + len > length) break;
        if (ra[off] == 24 && len <= 24u && out + len <= sizeof(sLastRa)) {
            memcpy(&sLastRa[out], &ra[off], len);
            out += len;
        }
        off += len;
    }
    sLastRaLength = out;
    sLastRaIndex = index;
}

otError otPlatInfraIfSendIcmp6Nd(uint32_t index, const otIp6Address *destination,
                               const uint8_t *buffer, uint16_t length)
{
    if (!destination || !buffer || length < 4 || length > 1460 || !index || index > 255)
        return OT_ERROR_INVALID_ARGS;
    LOCK_TCPIP_CORE();
    otError result = send_icmp6_nd_locked(index, destination, buffer, length);
    if (result == OT_ERROR_NONE && buffer[0] == 134 && destination->mFields.m8[0] == 0xff)
        remember_ra_locked(index, buffer, length);
    UNLOCK_TCPIP_CORE();
    return result;
}

/* lwIP core context (sys_timeout). */
static void withdraw_again(void *arg)
{
    (void)arg;
    if (sWithdrawLeft == 0) return;
    --sWithdrawLeft;
    (void)send_icmp6_nd_locked(sLastRaIndex, &kAllNodes, sLastRa, sLastRaLength);
    if (sWithdrawLeft) sys_timeout(RA_WITHDRAW_GAP_MS, withdraw_again, NULL);
}

/* OpenThread stopped: send an RA with router lifetime 0 and OpenThread's
 * last Route Information Options at lifetime 0 (RFC 4191), so LAN hosts stop
 * routing the Thread prefixes to this BR. Prefix options are left out, which
 * leaves hosts' on-link prefixes (and ours) to age out as advertised. */
unsigned br_infra_withdraw_routes(void)
{
    unsigned routes = 0;

    LOCK_TCPIP_CORE();
    if (sLastRaLength >= RA_HEADER) {
        bool router = sLastRa[6] || sLastRa[7];
        sLastRa[6] = sLastRa[7] = 0;              /* router lifetime */
        for (uint16_t off = RA_HEADER; off + 8u <= sLastRaLength; off += (uint16_t)(sLastRa[off + 1] * 8u)) {
            memset(&sLastRa[off + 4], 0, 4);      /* RIO route lifetime */
            ++routes;
        }
        if (routes || router) {
            if (send_icmp6_nd_locked(sLastRaIndex, &kAllNodes, sLastRa, sLastRaLength) != OT_ERROR_NONE)
                routes = 0;
            sys_untimeout(withdraw_again, NULL);
            sWithdrawLeft = RA_WITHDRAW_SENDS - 1u; /* resent even when this send failed */
            sys_timeout(RA_WITHDRAW_GAP_MS, withdraw_again, NULL);
        }
    }
    UNLOCK_TCPIP_CORE();
    return routes;
}

static otError send_icmp6_nd_locked(uint32_t index, const otIp6Address *destination,
                                    const uint8_t *buffer, uint16_t length)
{
    otError result = OT_ERROR_FAILED;
    struct netif *netif = netif_get_by_index((uint8_t)index);
    if (netif && netif_is_up(netif) && netif_is_link_up(netif) &&
        ip6_addr_isvalid(netif_ip6_addr_state(netif, 0)) &&
        ip6_addr_islinklocal(netif_ip6_addr(netif, 0))) {
        struct pbuf *p = pbuf_alloc(PBUF_IP, length, PBUF_RAM);
        if (p) {
            ip6_addr_t dest;
            memcpy(dest.addr, destination->mFields.m8, 16);
            ip6_addr_clear_zone(&dest);
            ip6_addr_assign_zone(&dest, IP6_UNKNOWN, netif);
            if (pbuf_take(p, buffer, length) == ERR_OK) {
                uint8_t *icmp = p->payload;
                icmp[2] = icmp[3] = 0;
                uint16_t checksum = ip6_chksum_pseudo(p, 58, length,
                                                     netif_ip6_addr(netif, 0), &dest);
                memcpy(icmp + 2, &checksum, sizeof(checksum));
                if (ip6_output_if(p, netif_ip6_addr(netif, 0), &dest, 255, 0, 58, netif) == ERR_OK) {
                    result = OT_ERROR_NONE;
                    if (buffer[0] == 134 && !br_thread_record_ra(buffer, length))
                        IPRO_LOGE("BR-Infra", "Cannot retain advertised on-link prefixes");
                }
            }
            pbuf_free(p);
        } else result = OT_ERROR_NO_BUFS;
    }
    return result;
}

otError otPlatGetInfraIfLinkLayerAddress(otInstance *instance, uint32_t index,
                                        otPlatInfraIfLinkLayerAddress *address)
{
    (void)instance;
    if (!address || !index || index > 255) return OT_ERROR_INVALID_ARGS;
    otError result = OT_ERROR_FAILED;
    LOCK_TCPIP_CORE();
    struct netif *netif = netif_get_by_index((uint8_t)index);
    if (netif && netif->hwaddr_len <= sizeof(address->mAddress)) {
        address->mLength = netif->hwaddr_len;
        memcpy(address->mAddress, netif->hwaddr, address->mLength);
        result = OT_ERROR_NONE;
    }
    UNLOCK_TCPIP_CORE();
    return result;
}

#ifdef CONFIG_SHELL
/* BR-only Wi-Fi adapter diagnostic, snapshots under the adapter's IRQ lock. */
extern void net_nd6_ra_tx_stats(uint32_t values[3]);
extern void net_nd6_rs_rx_stats(uint32_t values[5]);

static void cmd_nd_stats(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    uint32_t values[5];
    LOCK_TCPIP_CORE();
    values[0] = sRsStats.seen; values[1] = sRsStats.queued;
    values[2] = sRsStats.invalid; values[3] = sRsStats.queue_full;
    values[4] = sRsStats.no_memory;
    UNLOCK_TCPIP_CORE();
    IPRO_LOGI("BR-ND", "direct_rs seen=%lu queued=%lu invalid=%lu queue_full=%lu no_memory=%lu",
              (unsigned long)values[0], (unsigned long)values[1], (unsigned long)values[2],
              (unsigned long)values[3], (unsigned long)values[4]);
    uint32_t tx[3];
    net_nd6_ra_tx_stats(tx);
    IPRO_LOGI("BR-ND", "direct_ra submitted=%lu ack=%lu no_ack=%lu",
              (unsigned long)tx[0], (unsigned long)tx[1], (unsigned long)tx[2]);
    uint32_t rx[5];
    net_nd6_rs_rx_stats(rx);
    IPRO_LOGI("BR-ND", "adapter_rs ingress=%lu self_drop=%lu hook_drop=%lu post_fail=%lu posted=%lu",
              (unsigned long)rx[0], (unsigned long)rx[1], (unsigned long)rx[2],
              (unsigned long)rx[3], (unsigned long)rx[4]);
}
SHELL_CMD_EXPORT_ALIAS(cmd_nd_stats, br_nd_stats, Direct RS receive counters);
#endif
