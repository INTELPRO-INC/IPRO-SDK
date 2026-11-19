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
#include "task.h"
#include "queue.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/mld6.h"
#include <openthread/backbone_router_ftd.h>
#include <openthread/thread.h>
#include "openthread-core-config.h"
#include "ipro_log.h"
#include "br_multicast.h"
#include "br_mld_sync.h"
#include "br_multicast_policy.h"
#include "openthread_port.h"
_Static_assert(BR_MLD_MAX_GROUPS >= OPENTHREAD_CONFIG_MAX_MULTICAST_LISTENERS,
               "MLD synchronizer must hold the configured OT listener table");
_Static_assert(MEMP_NUM_MLD6_GROUP >= BR_MLD_MAX_GROUPS + 21,
               "MLD pool needs listener capacity plus control-group headroom");
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

/* Snapshot/dirty state belongs to OT. Membership and diagnostics are protected
 * by the lwIP core lock. Never invoke OT APIs while holding that lock. */
static br_mld_address sDesired[BR_MLD_MAX_GROUPS];
static struct br_mld_sync sMembership;
static struct netif *sBoundNetif;
static uint32_t sBoundIndex;
static bool sCallbackSet, sDirty = true;
static bool sLastPrimary, sLastActive;
static uint32_t sLastIndex;
static TickType_t sLastPoll;
static unsigned sDesiredCount;
static bool sPrimary, sInfraReady;
static uint8_t sMeshPrefix[8];
#define MCAST_QUEUE_DEPTH 4
struct mcast_event { struct pbuf *packet; uint32_t generation; };
static StaticQueue_t sIngressControl;
static uint8_t sIngressStorage[MCAST_QUEUE_DEPTH * sizeof(struct mcast_event)];
static QueueHandle_t sIngress;
static uint32_t sGeneration, sToThread, sToWifi, sQueueDrops, sTxErrors, sPolicyDrops;

static void listener_changed(void *context, otBackboneRouterMulticastListenerEvent event,
                             const otIp6Address *address)
{
    (void)context;
    (void)event;
    (void)address;
    sDirty = true;
    LOCK_TCPIP_CORE();
    ++sGeneration;
    sMembership.ready = false; /* Revoke stale packet authorization immediately. */
    UNLOCK_TCPIP_CORE();
}

static struct netif *bound_netif(void)
{
    /* The Wi-Fi adapter creates its netif once; it does not remove it on link
     * loss. Reject a different object rather than release refs on a replacement.
     * A removed netif's refs have already been freed by lwIP mld6_stop(). */
    struct netif *netif = sBoundIndex ? netif_get_by_index((uint8_t)sBoundIndex) : NULL;
    return netif == sBoundNetif ? netif : NULL;
}

static void to_lwip(ip6_addr_t *out, const uint8_t address[16], struct netif *netif)
{
    memset(out, 0, sizeof(*out));
    memcpy(out->addr, address, 16);
    ip6_addr_assign_zone(out, IP6_MULTICAST, netif);
}

static bool join_group(void *context, const uint8_t address[16])
{
    (void)context;
    struct netif *netif = bound_netif();
    if (!netif) return false;
    ip6_addr_t group;
    to_lwip(&group, address, netif);
    /* Always acquire our own reference, including an already-present group. */
    struct mld_group *entry = mld6_lookfor_group(netif, &group);
    if (entry && entry->use == UINT8_MAX) return false;
    return mld6_joingroup_netif(netif, &group) == ERR_OK;
}

static bool leave_group(void *context, const uint8_t address[16])
{
    (void)context;
    struct netif *netif = bound_netif();
    if (!netif) return true; /* netif teardown already released its references. */
    ip6_addr_t group;
    to_lwip(&group, address, netif);
    if (!mld6_lookfor_group(netif, &group)) return true;
    return mld6_leavegroup_netif(netif, &group) == ERR_OK;
}

static void sync_membership(otInstance *instance, uint32_t index, bool infra_ready)
{
    if (!sCallbackSet) {
        otBackboneRouterSetMulticastListenerCallback(instance, listener_changed, NULL);
        sCallbackSet = true; /* Registration replays existing listeners. */
    }
    bool primary = otBackboneRouterGetState(instance) == OT_BACKBONE_ROUTER_STATE_PRIMARY;
    otDeviceRole role = otThreadGetDeviceRole(instance);
    bool active = primary && infra_ready && role >= OT_DEVICE_ROLE_CHILD;
    TickType_t now = xTaskGetTickCount();
    if (!sDirty && primary == sLastPrimary && active == sLastActive && index == sLastIndex &&
        (TickType_t)(now - sLastPoll) < pdMS_TO_TICKS(1000)) return;
    sDirty = false;
    sLastPrimary = primary;
    sLastActive = active;
    sLastIndex = index;
    sLastPoll = now;

    size_t count = 0;
    otBackboneRouterMulticastListenerIterator iterator =
        OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ITERATOR_INIT;
    otBackboneRouterMulticastListenerInfo listener;
    otError error;
    while ((error = otBackboneRouterMulticastListenerGetNext(instance, &iterator, &listener)) ==
           OT_ERROR_NONE) {
        if (count == BR_MLD_MAX_GROUPS) {
            ++count; /* Overflow fails closed; never claim a truncated table is ready. */
            break;
        }
        /* mTimeout is rounded down to seconds. Zero can still be a live lease;
         * the core's expiration/removal event, not this rounding, removes it. */
        memcpy(sDesired[count++], listener.mAddress.mFields.m8, 16);
    }
    if (error != OT_ERROR_NONE && error != OT_ERROR_NOT_FOUND) count = BR_MLD_MAX_GROUPS + 1;

    LOCK_TCPIP_CORE();
    if (!sBoundIndex && index > 0 && index <= 255) {
        sBoundNetif = netif_get_by_index((uint8_t)index);
        if (sBoundNetif) sBoundIndex = index;
    }
    struct netif *netif = bound_netif();
    bool link = netif && index == sBoundIndex && netif_is_up(netif) &&
                netif_is_link_up(netif) && (netif->flags & NETIF_FLAG_MLD6);
    if (sPrimary != primary || sInfraReady != (infra_ready && link)) ++sGeneration;
    sPrimary = primary;
    sInfraReady = infra_ready && link;
    sDesiredCount = (unsigned)count;
    br_mld_sync_update(&sMembership, sDesired, count, active && link,
                       join_group, leave_group, NULL);
    UNLOCK_TCPIP_CORE();
}

/* Called under the core lock. Packet policy runs on a contiguous copy, never
 * on unchecked/chained input. OT still owns extension-header processing and
 * untrusted-host filtering; this is routing policy, not a transport endpoint. */
static bool packet_allowed(const struct pbuf *p, bool from_thread)
{
    struct netif *netif = bound_netif();
    bool ready = sInfraReady && sMembership.ready && netif &&
                 netif_is_up(netif) && netif_is_link_up(netif);
    const uint8_t *bytes = p->payload;
    return br_multicast_eligible(bytes, p->tot_len, sMeshPrefix, sPrimary, ready,
                                 from_thread, br_mld_sync_contains(&sMembership, bytes + 24));
}

void br_multicast_wifi_input(struct pbuf *p, struct netif *netif)
{
    if (!sIngress || netif != bound_netif() || !sMembership.ready ||
        p->tot_len < 40 || pbuf_get_at(p, 24) != 0xff) return;
    unsigned length = 40u + ((unsigned)pbuf_get_at(p, 4) << 8) + pbuf_get_at(p, 5);
    if (length > p->tot_len || length > 1280 || uxQueueSpacesAvailable(sIngress) == 0) {
        ++sQueueDrops;
        return;
    }
    struct pbuf *copy = pbuf_alloc(PBUF_RAW, (u16_t)length, PBUF_RAM);
    if (!copy) { ++sQueueDrops; return; }
    if (pbuf_copy_partial(p, copy->payload, (u16_t)length, 0) != length ||
        !packet_allowed(copy, false)) {
        ++sPolicyDrops;
        pbuf_free(copy);
        return;
    }
    struct mcast_event event = { copy, sGeneration };
    if (xQueueSend(sIngress, &event, 0) != pdTRUE) {
        ++sQueueDrops;
        pbuf_free(copy);
    } else otSysEventSignalPending();
}

void br_multicast_from_thread(otInstance *instance, const otMessage *message)
{
    /* MPL retransmission clones are HOST_TRUSTED, not fresh Thread ingress.
     * Do not reflect them (or a host injection) back to the infrastructure. */
    if (otMessageGetOrigin(message) != OT_MESSAGE_ORIGIN_THREAD_NETIF ||
        otBackboneRouterGetState(instance) != OT_BACKBONE_ROUTER_STATE_PRIMARY ||
        otThreadGetDeviceRole(instance) < OT_DEVICE_ROLE_CHILD) return;
    uint16_t length = otMessageGetLength(message);
    if (length < 40 || length > 1280) return;
    uint8_t destination;
    if (otMessageRead(message, 24, &destination, 1) != 1 || destination != 0xff) return;
    LOCK_TCPIP_CORE();
    struct pbuf *p = pbuf_alloc(PBUF_LINK, length, PBUF_RAM);
    UNLOCK_TCPIP_CORE();
    bool copied = p && otMessageRead(message, 0, p->payload, length) == length;
    LOCK_TCPIP_CORE();
    if (!p) ++sTxErrors;
    else {
        if (copied && packet_allowed(p, true)) {
            uint8_t *bytes = p->payload;
            --bytes[7]; /* OT passes to host before decrementing the Thread hop. */
            ip6_addr_t group;
            struct netif *netif = bound_netif();
            to_lwip(&group, bytes + 24, netif);
            /* Direct interface output retains IPv6 header, with Ethernet
             * headroom; it does not request lwIP multicast loopback. */
            if (netif->output_ip6(netif, p, &group) == ERR_OK) ++sToWifi;
            else ++sTxErrors;
        } else ++sPolicyDrops;
        pbuf_free(p);
    }
    UNLOCK_TCPIP_CORE();
}

void br_multicast_drop_queued(void)
{
    struct mcast_event event;
    while (sIngress && xQueueReceive(sIngress, &event, 0) == pdTRUE) pbuf_free(event.packet);
}

void br_multicast_process(otInstance *instance, uint32_t index, bool infra_ready)
{
    if (!sIngress) {
        LOCK_TCPIP_CORE();
        sIngress = xQueueCreateStatic(MCAST_QUEUE_DEPTH, sizeof(struct mcast_event),
                                       sIngressStorage, &sIngressControl);
        UNLOCK_TCPIP_CORE();
        if (!sIngress) return;
    }
    const otMeshLocalPrefix *mesh = otThreadGetMeshLocalPrefix(instance);
    LOCK_TCPIP_CORE();
    if (memcmp(sMeshPrefix, mesh->m8, 8) != 0) {
        memcpy(sMeshPrefix, mesh->m8, 8);
        ++sGeneration;
    }
    UNLOCK_TCPIP_CORE();
    sync_membership(instance, index, infra_ready);
    for (unsigned i = 0; i < MCAST_QUEUE_DEPTH; ++i) {
        struct mcast_event event;
        if (xQueueReceive(sIngress, &event, 0) != pdTRUE) break;
        struct pbuf *p = event.packet;
        LOCK_TCPIP_CORE();
        bool allowed = event.generation == sGeneration && packet_allowed(p, false);
        if (!allowed) ++sPolicyDrops;
        UNLOCK_TCPIP_CORE();
        if (allowed) {
            --((uint8_t *)p->payload)[7]; /* HOST_UNTRUSTED does not decrement in OT. */
            otMessage *message = otIp6NewMessage(instance, NULL);
            otError error = OT_ERROR_NO_BUFS;
            if (message) {
                error = otMessageAppend(message, p->payload, p->tot_len);
                if (error == OT_ERROR_NONE) {
                    otMessageSetOrigin(message, OT_MESSAGE_ORIGIN_HOST_UNTRUSTED);
                    otMessageSetLoopbackToHostAllowed(message, false);
                    error = otIp6Send(instance, message); /* Always consumes message. */
                } else otMessageFree(message);
            }
            LOCK_TCPIP_CORE();
            if (error == OT_ERROR_NONE) ++sToThread;
            else ++sTxErrors;
            UNLOCK_TCPIP_CORE();
        }
        LOCK_TCPIP_CORE();
        pbuf_free(p);
        UNLOCK_TCPIP_CORE();
    }
}

#ifdef CONFIG_SHELL
static void cmd_multicast_stats(int argc, char **argv)
{
    ip6_addr_t group;
    if (argc > 2 || (argc == 2 && !ip6addr_aton(argv[1], &group))) {
        IPRO_LOGI("BR-Mcast", "Usage: br_mcast_stats [IPv6 group]");
        return;
    }
    unsigned values[15], present = 0, references = 0, authorized = 0;
    LOCK_TCPIP_CORE();
    values[0] = sPrimary; values[1] = sInfraReady; values[2] = sDesiredCount;
    values[3] = (unsigned)sMembership.count; values[4] = sMembership.ready;
    values[5] = sMembership.joins; values[6] = sMembership.leaves;
    values[7] = sMembership.join_errors; values[8] = sMembership.leave_errors;
    values[9] = sMembership.invalid_snapshots;
    values[10] = sToThread; values[11] = sToWifi; values[12] = sQueueDrops;
    values[13] = sTxErrors; values[14] = sPolicyDrops;
    struct netif *netif = bound_netif();
    if (argc == 2 && netif) {
        ip6_addr_assign_zone(&group, IP6_MULTICAST, netif);
        struct mld_group *entry = mld6_lookfor_group(netif, &group);
        present = entry != NULL;
        if (entry) references = entry->use;
        authorized = br_mld_sync_contains(&sMembership, (const uint8_t *)group.addr);
    }
    UNLOCK_TCPIP_CORE();
    IPRO_LOGI("BR-Mcast", "primary=%u infra=%u desired=%u owned=%u membership_ready=%u "
              "joins=%u leaves=%u join_errors=%u leave_errors=%u invalid=%u",
              values[0], values[1], values[2], values[3], values[4], values[5],
              values[6], values[7], values[8], values[9]);
    IPRO_LOGI("BR-Mcast", "to_thread=%u to_wifi=%u queue_drops=%u tx_errors=%u policy_drops=%u",
              values[10], values[11], values[12], values[13], values[14]);
    if (argc == 2) IPRO_LOGI("BR-Mcast", "group_present=%u group_refs=%u group_authorized=%u",
                            present, references, authorized);
}
SHELL_CMD_EXPORT_ALIAS(cmd_multicast_stats, br_mcast_stats, MLR to MLD membership counters);
#endif
