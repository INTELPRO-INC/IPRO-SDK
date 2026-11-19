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
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/nd6.h"
#include "lwip/sys.h"
#include "fhost.h"
#include "wifi_mgmr_ext.h"
#include <openthread/ip6.h>
#include <openthread/message.h>
#include <openthread/netdata.h>
#include <openthread/thread.h>
#include <openthread/border_routing.h>
#include "openthread_port.h"
#include "ipro_log.h"
#include "br_route.h"
#include "br_thread_netif.h"
#include "br_multicast.h"
#include "br_dua_proxy.h"
#include "br_trace.h"
#include "br_onlink.h"
#include "br_wifi_link_epoch.h"
#include "openthread-core-config.h"

_Static_assert(BR_ONLINK_CAPACITY >= OPENTHREAD_CONFIG_BORDER_ROUTING_MAX_OLD_ON_LINK_PREFIXES + 3,
               "On-link cache must hold local/old and Domain Prefix PIOs");

/* Absorb short Wi-Fi bursts without extending one OT driver pass. Copies
 * share the 48 KiB lwIP heap; keep this bounded independently of the batch. */
#define THREAD_TX_DEPTH 8
#define THREAD_TX_BATCH 4
#define THREAD_PREFIX_MAX 16
static struct netif sThread;
static bool sInitialized;
static StaticQueue_t sTxControl;
static uint8_t sTxStorage[THREAD_TX_DEPTH * sizeof(struct pbuf *)];
static QueueHandle_t sTxQueue;
/* Only accessed under the lwIP core lock; no OT calls from the route hook. */
static struct br_route_prefix sPrefixes[THREAD_PREFIX_MAX];
static unsigned sPrefixCount;
static bool sRoutesReady;
static otIp6Prefix sLocalOnLink;
static struct br_route_prefix sDomainPrefix;
static struct br_onlink_cache sAdvertisedOnLink;
static uint32_t sOnLinkEpoch;

bool br_thread_record_ra(const uint8_t *ra, size_t length)
{
    uint32_t epoch = br_wifi_link_epoch();
    if (!epoch) return false;
    if (epoch != sOnLinkEpoch) {
        memset(&sAdvertisedOnLink, 0, sizeof(sAdvertisedOnLink));
        sOnLinkEpoch = epoch;
        nd6_clear_destination_cache();
    }
    struct br_onlink_cache before = sAdvertisedOnLink;
    if (!br_onlink_record(&sAdvertisedOnLink, ra, length, sys_now())) return false;
    for (unsigned i = 0; i < BR_ONLINK_CAPACITY; ++i) {
        if (!!before.entries[i].valid_ms != !!sAdvertisedOnLink.entries[i].valid_ms ||
            memcmp(before.entries[i].prefix, sAdvertisedOnLink.entries[i].prefix, 8) != 0) {
            nd6_clear_destination_cache();
            break;
        }
    }
    return true;
}

struct netif *br_thread_netif_get(void)
{
    return sThread.input ? &sThread : NULL;
}

static err_t thread_output(struct netif *netif, struct pbuf *p, const ip6_addr_t *destination)
{
    (void)netif;
    (void)destination;
    if (otrIsStopped() || !sRoutesReady || !netif_is_link_up(&sThread)) return ERR_RTE;
    if (p->tot_len < 40 || p->tot_len > 1280) return ERR_VAL;
    struct pbuf *copy = pbuf_clone(PBUF_RAW, PBUF_RAM, p);
    if (!copy) {
        br_trace_pbuf(BR_TRACE_ALLOC_DROP, p, ERR_MEM);
        return ERR_MEM;
    }
    if (xQueueSend(sTxQueue, &copy, 0) != pdTRUE) {
        br_trace_pbuf(BR_TRACE_QUEUE_DROP, p, ERR_MEM);
        pbuf_free(copy);
        return ERR_MEM;
    }
    br_trace_pbuf(BR_TRACE_QUEUED, p, 0);
    otSysEventSignalPending();
    return ERR_OK;
}

static err_t thread_init(struct netif *netif)
{
    netif->name[0] = 'o';
    netif->name[1] = 't';
    netif->mtu = 1280;
    netif->output_ip6 = thread_output;
    /* Not Ethernet: OpenThread handles link-layer addressing and multicast. */
    netif->flags = 0;
    return ERR_OK;
}

struct netif *br_ip6_route(const ip6_addr_t *source, const ip6_addr_t *destination)
{
    (void)source;
    if (sRoutesReady && netif_is_up(&sThread) && netif_is_link_up(&sThread)) {
        /* Local DUA delivery/loopback must work even on a Secondary BBR.
         * This is not authorization to proxy any remote Thread DUA. */
        for (unsigned i = 1; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
            if (ip6_addr_isvalid(netif_ip6_addr_state(&sThread, i)) &&
                ip6_addr_cmp_zoneless(destination, netif_ip6_addr(&sThread, i))) return &sThread;
        }
        if (br_dua_can_route((const uint8_t *)destination->addr)) return &sThread;
        if (br_route_to_thread((const uint8_t *)destination->addr, sPrefixes,
                               sPrefixCount, &sDomainPrefix)) return &sThread;
    }
    /* Do not select Thread merely because a forwarded source matches it. */
    struct netif *wifi = fhost_to_net_if(MGMR_VIF_STA);
    return wifi && netif_is_up(wifi) && netif_is_link_up(wifi) ? wifi : NULL;
}

const ip6_addr_t *br_nd6_gateway(struct netif *netif, const ip6_addr_t *destination)
{
    /* lwIP does not receive our outgoing RA. Retain all advertised PIOs
     * through valid lifetime, including deprecated old on-link prefixes. */
    if (netif == fhost_to_net_if(MGMR_VIF_STA) &&
        ((sOnLinkEpoch == br_wifi_link_epoch() &&
          br_onlink_contains(&sAdvertisedOnLink, (const uint8_t *)destination->addr, sys_now())) ||
         br_prefix_match((const uint8_t *)destination->addr,
                         sLocalOnLink.mPrefix.mFields.m8, sLocalOnLink.mLength) ||
         br_prefix_match((const uint8_t *)destination->addr,
                         sDomainPrefix.address, sDomainPrefix.length))) return destination;
    return NULL;
}

static void thread_receive(otMessage *message, void *context)
{
    uint8_t header[48];
    if (otMessageRead(message, 0, header, sizeof(header)) == sizeof(header))
        br_trace_bytes(BR_TRACE_OT_RX, header, sizeof(header), 0);
    br_multicast_from_thread(context, message);
    uint16_t length = otMessageGetLength(message);
    if (length >= 40 && length <= 1280) {
        /* Wi-Fi egress prepends Ethernet. RAW has no headroom and causes
         * ethernet_output() to reject otherwise valid forwarded replies. */
        struct pbuf *p = pbuf_alloc(PBUF_LINK, length, PBUF_RAM);
        if (p) {
            if (otMessageRead(message, 0, p->payload, length) != length ||
                tcpip_input(p, &sThread) != ERR_OK) pbuf_free(p);
        }
    }
    otMessageFree(message);
}

/* OT task only. Rebuild a bounded snapshot before acquiring the lwIP lock. */
static bool sync_addresses_routes(otInstance *instance)
{
    otIp6Prefix local_onlink = {0};
    if (otBorderRoutingGetState(instance) == OT_BORDER_ROUTING_STATE_RUNNING &&
        otBorderRoutingGetOnLinkPrefix(instance, &local_onlink) != OT_ERROR_NONE)
        memset(&local_onlink, 0, sizeof(local_onlink));
    struct br_route_prefix prefixes[THREAD_PREFIX_MAX];
    struct br_route_prefix domain = {0};
    bool domain_found = false;
    unsigned count = 0;
    bool overflow = false;
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otBorderRouterConfig config;
    while (otNetDataGetNextOnMeshPrefix(instance, &iterator, &config) == OT_ERROR_NONE) {
        /* Match BackboneRouter::Leader's first-mDp selection, not the local
         * BBR's configured prefix. Disabled/Secondary BBRs need this too. */
        if (config.mDp) {
            if (!domain_found) {
                domain.length = config.mPrefix.mLength;
                memcpy(domain.address, config.mPrefix.mPrefix.mFields.m8, 16);
                domain_found = true;
            }
            continue;
        }
        if (!config.mOnMesh || config.mPrefix.mLength == 0) continue;
        unsigned i;
        for (i = 0; i < count; ++i) {
            if (prefixes[i].length == config.mPrefix.mLength &&
                memcmp(prefixes[i].address, config.mPrefix.mPrefix.mFields.m8, 16) == 0) break;
        }
        if (i != count) continue;
        /* Continue scanning on overflow so a later selected DP is not lost. */
        if (count == THREAD_PREFIX_MAX) { overflow = true; continue; }
        prefixes[count].length = config.mPrefix.mLength;
        memcpy(prefixes[count++].address, config.mPrefix.mPrefix.mFields.m8, 16);
    }
    ip6_addr_t addresses[LWIP_IPV6_NUM_ADDRESSES];
    uint8_t states[LWIP_IPV6_NUM_ADDRESSES] = {0};
    memset(addresses, 0, sizeof(addresses));
    /* lwIP reserves slot zero for link-local; OT owns link-local traffic. */
    unsigned addr_count = 1;
    for (const otNetifAddress *addr = otIp6GetUnicastAddresses(instance); addr; addr = addr->mNext) {
        if (!addr->mValid || addr->mRloc || addr->mMeshLocal ||
            (addr->mAddress.mFields.m8[0] == 0xfe &&
             (addr->mAddress.mFields.m8[1] & 0xc0) == 0x80)) continue;
        if (addr_count == LWIP_IPV6_NUM_ADDRESSES) { overflow = true; break; }
        memcpy(addresses[addr_count].addr, addr->mAddress.mFields.m8, 16);
        states[addr_count++] = addr->mPreferred ? IP6_ADDR_PREFERRED : IP6_ADDR_DEPRECATED;
    }
    otDeviceRole role = otThreadGetDeviceRole(instance);
    bool attached = role == OT_DEVICE_ROLE_CHILD || role == OT_DEVICE_ROLE_ROUTER ||
                    role == OT_DEVICE_ROLE_LEADER;
    LOCK_TCPIP_CORE();
    uint32_t epoch = br_wifi_link_epoch();
    if (epoch != sOnLinkEpoch) {
        memset(&sAdvertisedOnLink, 0, sizeof(sAdvertisedOnLink));
        sOnLinkEpoch = epoch;
        nd6_clear_destination_cache();
    } else if (br_onlink_expire(&sAdvertisedOnLink, sys_now())) {
        nd6_clear_destination_cache();
    }
    if (sDomainPrefix.length != domain.length ||
        memcmp(sDomainPrefix.address, domain.address, 16) != 0) {
        sDomainPrefix = domain;
        nd6_clear_destination_cache();
    }
    if (sLocalOnLink.mLength != local_onlink.mLength ||
        memcmp(sLocalOnLink.mPrefix.mFields.m8, local_onlink.mPrefix.mFields.m8, 16) != 0) {
        sLocalOnLink = local_onlink;
        nd6_clear_destination_cache();
    }
    sPrefixCount = count;
    memcpy(sPrefixes, prefixes, count * sizeof(prefixes[0]));
    sRoutesReady = !overflow;
    for (unsigned i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
        if (!ip6_addr_cmp_zoneless(netif_ip6_addr(&sThread, i), &addresses[i]) ||
            netif_ip6_addr_state(&sThread, i) != states[i]) {
            netif_ip6_addr_set_state(&sThread, i, IP6_ADDR_INVALID);
            netif_ip6_addr_set(&sThread, i, &addresses[i]);
            netif_ip6_addr_set_state(&sThread, i, states[i]);
        }
    }
    if (attached && !overflow) {
        netif_set_up(&sThread);
        netif_set_link_up(&sThread);
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
        sThread.rs_count = 0; /* set_up/link_up restart lwIP RS otherwise. */
#endif
    } else {
        netif_set_link_down(&sThread);
        netif_set_down(&sThread);
    }
    UNLOCK_TCPIP_CORE();
    return !overflow;
}

void br_thread_netif_drop_queued(void)
{
    struct pbuf *p;
    while (sTxQueue && xQueueReceive(sTxQueue, &p, 0) == pdTRUE) pbuf_free(p);
}

bool br_thread_netif_process(otInstance *instance)
{
    if (!sInitialized) {
        sTxQueue = xQueueCreateStatic(THREAD_TX_DEPTH, sizeof(struct pbuf *), sTxStorage, &sTxControl);
        if (!sTxQueue) return false;
        LOCK_TCPIP_CORE();
        struct netif *added = netif_add_noaddr(&sThread, NULL, thread_init, tcpip_input);
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
        sThread.rs_count = 0; /* OpenThread owns ND on this non-Ethernet interface. */
#endif
        UNLOCK_TCPIP_CORE();
        if (!added) return false;
        /* Keep internal Thread UDP and ICMP echo handling in OT, not both stacks. */
        otIp6SetReceiveFilterEnabled(instance, true);
        otIp6SetReceiveCallback(instance, thread_receive, instance);
        sInitialized = true;
        IPRO_LOGI("BR-Netif", "Thread IPv6 forwarding interface ready");
    }
    bool ready = sync_addresses_routes(instance);
    struct pbuf *p;
    for (unsigned count = 0; count < THREAD_TX_BATCH &&
         xQueueReceive(sTxQueue, &p, 0) == pdTRUE; ++count) {
        /* A prefix may have changed since lwIP queued this packet. Check the
         * fresh snapshot before crossing into Thread; never use stale routes. */
        ip6_addr_t destination;
        ip6_addr_set_zero(&destination);
        bool allowed = pbuf_copy_partial(p, destination.addr, 16, 24) == 16;
        LOCK_TCPIP_CORE();
        allowed = allowed && br_ip6_route(NULL, &destination) == &sThread;
        UNLOCK_TCPIP_CORE();
        if (!allowed) {
            br_trace_pbuf(BR_TRACE_ROUTE_DROP, p, ERR_RTE);
            pbuf_free(p);
            continue;
        }
        otMessage *message = otIp6NewMessage(instance, NULL);
        otError error = OT_ERROR_NO_BUFS;
        if (message) {
            error = otMessageAppend(message, p->payload, p->tot_len);
            if (error == OT_ERROR_NONE) {
                otMessageSetOrigin(message, OT_MESSAGE_ORIGIN_HOST_UNTRUSTED);
                error = otIp6Send(instance, message); /* Consumed, including error paths. */
            } else otMessageFree(message);
        }
        br_trace_pbuf(BR_TRACE_OT_SEND, p, error);
        if (error != OT_ERROR_NONE) IPRO_LOGW("BR-Netif", "IPv6 send failed: %d", error);
        pbuf_free(p);
    }
    /* Task notifications are coalesced, not one token per queued packet.
     * Retain a wake for leftovers after this bounded pass. A producer racing
     * an empty check sends its own notification after successfully enqueuing. */
    if (uxQueueMessagesWaiting(sTxQueue) != 0) otSysEventSignalPending();
    return ready;
}
