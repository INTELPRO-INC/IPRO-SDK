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
#include "lwip/nd6.h"
#include <openthread/backbone_router_ftd.h>
#include <openthread/thread.h>
#include <openthread/netdata.h>
#include <openthread/border_routing.h>
#include "openthread_port.h"
#include "ipro_log.h"
#include "br_dua_proxy.h"
#include "br_dua_table.h"
#include "br_nd.h"
#include "br_nd_proxy_packet.h"
#include "br_route.h"
#include "br_domain_ra.h"
#include "openthread-core-config.h"
_Static_assert(BR_DUA_CAPACITY >= OPENTHREAD_CONFIG_NDPROXY_TABLE_ENTRY_NUM,
               "DUA adapter must hold the configured OT proxy table");
_Static_assert(MEMP_NUM_MLD6_GROUP >= BR_DUA_CAPACITY + OPENTHREAD_CONFIG_MAX_MULTICAST_LISTENERS + 21,
               "MLD pool needs full DUA, MLR and control-group capacity");
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define DUA_QUEUE_DEPTH 4
#define DUA_RX_PER_SECOND 32
#define DUA_TX_PER_SECOND 16
struct dua_request {
    uint32_t generation;
    uint16_t length;
    uint16_t target_offset;
    uint8_t packet[1500];
};
static struct br_dua_table sTable;
static struct {
    uint8_t mesh_iid[8];
    bool announce;
} sIdentity[BR_DUA_CAPACITY];
static bool sInitialized, sPrimary, sAttached, sInfraReady;
static struct netif *sNetif;
static uint32_t sIndex, sGeneration;
static QueueHandle_t sRequests;
static StaticQueue_t sRequestControl;
static uint8_t sRequestStorage[DUA_QUEUE_DEPTH * sizeof(struct dua_request *)];
static TickType_t sBudgetEpoch;
static unsigned sRxBudget, sTxBudget, sAnnounceCursor;
static unsigned sReplies, sAnnouncements, sDrops, sErrors, sJoins, sLeaves;
static struct br_domain_ra sDomainRa; /* OT task only. */
static uint8_t sDomainRaOptions[64];

static void sync_domain_pio(otInstance *instance)
{
    struct br_route_prefix domain = {0};
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otBorderRouterConfig config;
    while (otThreadGetDeviceRole(instance) >= OT_DEVICE_ROLE_CHILD &&
           otBackboneRouterGetState(instance) != OT_BACKBONE_ROUTER_STATE_DISABLED &&
           otNetDataGetNextOnMeshPrefix(instance, &iterator, &config) == OT_ERROR_NONE) {
        if (!config.mDp) continue;
        domain.length = config.mPrefix.mLength;
        memcpy(domain.address, config.mPrefix.mPrefix.mFields.m8, 16);
        break;
    }
    if (!br_domain_ra_set(&sDomainRa, &domain)) {
        /* Unsupported Domain Prefixes must not perpetuate an old active PIO. */
        memset(&domain, 0, sizeof(domain));
        (void)br_domain_ra_set(&sDomainRa, &domain);
    }
}

static otError prepare_domain_ra(bool final, const uint8_t **options, uint16_t *length, void *context)
{
    size_t bytes;
    /* Snapshot current Network Data here, not the last application poll. */
    sync_domain_pio(context);
    if (!br_domain_ra_prepare(&sDomainRa, final, sDomainRaOptions, sizeof(sDomainRaOptions), &bytes))
        return OT_ERROR_INVALID_STATE;
    *options = sDomainRaOptions;
    *length = (uint16_t)bytes;
    return OT_ERROR_NONE;
}

static void complete_domain_ra(bool final, otError error, void *context)
{
    (void)final;
    (void)context;
    br_domain_ra_complete(&sDomainRa, error == OT_ERROR_NONE);
}

/* Shared state below is protected by the lwIP core lock. All OT calls remain
 * outside that lock and on the OT task; shell diagnostics only copy counters. */
static struct netif *bound_netif(void)
{
    struct netif *netif = sIndex ? netif_get_by_index((uint8_t)sIndex) : NULL;
    return netif == sNetif ? netif : NULL;
}

static bool link_ready(void)
{
    struct netif *netif = bound_netif();
    return sInfraReady && netif && netif_is_up(netif) && netif_is_link_up(netif) &&
           (netif->flags & NETIF_FLAG_MLD6) && netif->hwaddr_len == 6 &&
           ip6_addr_isvalid(netif_ip6_addr_state(netif, 0)) &&
           ip6_addr_islinklocal(netif_ip6_addr(netif, 0));
}

static bool membership(void *context, const uint8_t address[16], bool join)
{
    (void)context;
    struct netif *netif = bound_netif();
    if (!netif) return !join; /* netif removal already frees its MLD refs. */
    ip6_addr_t group;
    ip6_addr_set_zero(&group);
    memcpy(group.addr, address, 16);
    ip6_addr_assign_zone(&group, IP6_MULTICAST, netif);
    struct mld_group *entry = mld6_lookfor_group(netif, &group);
    if (join && entry && entry->use == UINT8_MAX) { ++sErrors; return false; }
    if (!join && !entry) return true;
    err_t error = join ? mld6_joingroup_netif(netif, &group) : mld6_leavegroup_netif(netif, &group);
    if (error != ERR_OK) { ++sErrors; return false; }
    if (join) ++sJoins;
    else ++sLeaves;
    return true;
}

static bool join_group(void *context, const uint8_t address[16])
{
    return membership(context, address, true);
}

static bool leave_group(void *context, const uint8_t address[16])
{
    return membership(context, address, false);
}

static int entry_index(const uint8_t address[16])
{
    for (unsigned i = 0; i < BR_DUA_CAPACITY; ++i)
        if (sTable.entries[i].live && memcmp(sTable.entries[i].address, address, 16) == 0) return (int)i;
    return -1;
}

static void proxy_event(void *context, otBackboneRouterNdProxyEvent event, const otIp6Address *address)
{
    otInstance *instance = context;
    otBackboneRouterNdProxyInfo info;
    bool added = address && (event == OT_BACKBONE_ROUTER_NDPROXY_ADDED ||
                             event == OT_BACKBONE_ROUTER_NDPROXY_RENEWED);
    bool valid = added && otBackboneRouterGetNdProxyInfo(instance, address, &info) == OT_ERROR_NONE &&
                 info.mMeshLocalIid;
    LOCK_TCPIP_CORE();
    ++sGeneration; /* Invalidate all queued replies before changing authority. */
    nd6_clear_destination_cache();
    if (event == OT_BACKBONE_ROUTER_NDPROXY_CLEARED) {
        br_dua_clear(&sTable);
        memset(sIdentity, 0, sizeof(sIdentity));
    } else if (valid && br_dua_add(&sTable, address->mFields.m8)) {
        int i = entry_index(address->mFields.m8);
        if (i >= 0) {
            memcpy(sIdentity[i].mesh_iid, info.mMeshLocalIid->mFields.m8, 8);
            sIdentity[i].announce = true;
        }
    } else if (address) {
        int i = entry_index(address->mFields.m8);
        if (i >= 0) sIdentity[i].announce = false;
        br_dua_remove(&sTable, address->mFields.m8);
    }
    UNLOCK_TCPIP_CORE();
    otSysEventSignalPending();
}

bool br_dua_can_route(const uint8_t address[16])
{
    return sPrimary && sAttached && link_ready() && br_dua_contains(&sTable, address);
}

void br_dua_refresh(otInstance *instance)
{
    if (!sInitialized) {
        LOCK_TCPIP_CORE();
        sRequests = xQueueCreateStatic(DUA_QUEUE_DEPTH, sizeof(struct dua_request *),
                                       sRequestStorage, &sRequestControl);
        UNLOCK_TCPIP_CORE();
        /* No replay: this must run before the first BBR/Thread CLI command. */
        otBackboneRouterSetNdProxyCallback(instance, proxy_event, instance);
        if (otBorderRoutingSetManagedExtraRaCallbacks(instance, prepare_domain_ra, complete_domain_ra, instance) !=
            OT_ERROR_NONE) return;
        sInitialized = true;
    }
    sync_domain_pio(instance);
    bool primary = otBackboneRouterGetState(instance) == OT_BACKBONE_ROUTER_STATE_PRIMARY;
    bool attached = otThreadGetDeviceRole(instance) >= OT_DEVICE_ROLE_CHILD;
    LOCK_TCPIP_CORE();
    if (sPrimary != primary || sAttached != attached) {
        ++sGeneration;
        nd6_clear_destination_cache();
    }
    sPrimary = primary;
    sAttached = attached;
    if (!primary || !attached || !link_ready())
        br_dua_sync(&sTable, false, join_group, leave_group, NULL);
    UNLOCK_TCPIP_CORE();
    /* Revalidate known identities only; GetInfo alone is not DAD evidence.
     * Core Erase emits Removed even on proactive Backbone notifications. */
    for (unsigned i = 0; i < BR_DUA_CAPACITY; ++i) {
        if (!sTable.entries[i].live) continue; /* OT is the only writer. */
        otIp6Address address;
        memcpy(address.mFields.m8, sTable.entries[i].address, 16);
        otBackboneRouterNdProxyInfo info;
        if (otBackboneRouterGetNdProxyInfo(instance, &address, &info) != OT_ERROR_NONE ||
            !info.mMeshLocalIid || memcmp(info.mMeshLocalIid->mFields.m8, sIdentity[i].mesh_iid, 8) != 0)
            proxy_event(instance, OT_BACKBONE_ROUTER_NDPROXY_REMOVED, &address);
    }
}

static void refresh_budget(void)
{
    TickType_t now = xTaskGetTickCount();
    if ((TickType_t)(now - sBudgetEpoch) >= pdMS_TO_TICKS(1000)) {
        sBudgetEpoch = now;
        sRxBudget = DUA_RX_PER_SECOND;
        sTxBudget = DUA_TX_PER_SECOND;
    }
}

void br_dua_wifi_input(struct pbuf *packet, struct netif *netif)
{
    if (!sRequests || netif != bound_netif() || !sTable.active || !link_ready() ||
        packet->tot_len < 64 || packet->tot_len > 1500 || pbuf_get_at(packet, 7) != 255) return;
    uint8_t next = pbuf_get_at(packet, 6);
    if (next != 58 && next != 0 && next != 60) return;
    if (next == 58 && pbuf_get_at(packet, 40) != 135) return;
    refresh_budget();
    if (!sRxBudget || uxQueueSpacesAvailable(sRequests) == 0) { ++sDrops; return; }
    --sRxBudget;
    struct dua_request *request = pvPortMalloc(sizeof(*request));
    if (!request) { ++sDrops; return; }
    int offset = 0;
    if (pbuf_copy_partial(packet, request->packet, packet->tot_len, 0) == packet->tot_len)
        offset = br_nd_ns_validate(request->packet, packet->tot_len);
    if (!offset || !br_dua_can_route(request->packet + offset + 8)) {
        vPortFree(request);
        return;
    }
    request->length = packet->tot_len;
    request->target_offset = (uint16_t)(offset + 8);
    request->generation = sGeneration;
    if (xQueueSend(sRequests, &request, 0) != pdTRUE) {
        ++sDrops;
        vPortFree(request);
    } else otSysEventSignalPending();
}

/* OT task, with a second identity/role/link/MLD check immediately before TX. */
static bool send_na(otInstance *instance, const uint8_t address[16], const struct dua_request *request)
{
    LOCK_TCPIP_CORE();
    refresh_budget();
    bool eligible = sTxBudget && br_dua_can_route(address) &&
                    (!request || request->generation == sGeneration);
    UNLOCK_TCPIP_CORE();
    if (!eligible) return false;
    otIp6Address dua;
    memcpy(dua.mFields.m8, address, 16);
    otBackboneRouterNdProxyInfo info;
    if (otBackboneRouterGetState(instance) != OT_BACKBONE_ROUTER_STATE_PRIMARY ||
        otThreadGetDeviceRole(instance) < OT_DEVICE_ROLE_CHILD ||
        otBackboneRouterGetNdProxyInfo(instance, &dua, &info) != OT_ERROR_NONE || !info.mMeshLocalIid) return false;
    bool sent = false;
    LOCK_TCPIP_CORE();
    refresh_budget();
    int i = entry_index(address);
    if (i >= 0 && sTxBudget && br_dua_can_route(address) &&
        (!request || request->generation == sGeneration) &&
        memcmp(info.mMeshLocalIid->mFields.m8, sIdentity[i].mesh_iid, 8) == 0) {
        --sTxBudget;
        struct netif *netif = bound_netif();
        uint8_t output[BR_ND_PROXY_NA_SIZE];
        const uint8_t *source = (const uint8_t *)netif_ip6_addr(netif, 0)->addr;
        size_t size = request ? br_nd_proxy_na(request->packet, request->length, address,
                                               source, netif->hwaddr, info.mTimeSinceLastTransaction,
                                               output, sizeof(output)) :
                                br_nd_proxy_announce(address, source, netif->hwaddr,
                                                      info.mTimeSinceLastTransaction, output, sizeof(output));
        struct pbuf *p = size ? pbuf_alloc(PBUF_LINK, (u16_t)size, PBUF_RAM) : NULL;
        if (p) {
            ip6_addr_t destination;
            ip6_addr_set_zero(&destination);
            memcpy(destination.addr, output + 24, 16);
            ip6_addr_assign_zone(&destination, IP6_UNKNOWN, netif);
            sent = pbuf_take(p, output, (u16_t)size) == ERR_OK &&
                   netif->output_ip6(netif, p, &destination) == ERR_OK;
            pbuf_free(p);
        }
        if (!sent) ++sErrors;
        else if (request) ++sReplies;
        else ++sAnnouncements;
    }
    UNLOCK_TCPIP_CORE();
    return sent;
}

void br_dua_drop_queued(void)
{
    struct dua_request *request;
    while (sRequests && xQueueReceive(sRequests, &request, 0) == pdTRUE) vPortFree(request);
}

void br_dua_process(otInstance *instance, uint32_t index, bool infra_ready)
{
    LOCK_TCPIP_CORE();
    if (!sIndex && index && index <= 255) {
        sNetif = netif_get_by_index((uint8_t)index);
        if (sNetif) sIndex = index;
    }
    bool ready = infra_ready && index == sIndex;
    if (sInfraReady != ready) { ++sGeneration; nd6_clear_destination_cache(); }
    sInfraReady = ready;
    UNLOCK_TCPIP_CORE();
    /* main refreshed core identities before the Thread TX drain this turn.
     * Only infrastructure readiness changed here; do not scan OT twice. */
    LOCK_TCPIP_CORE();
    br_dua_sync(&sTable, sPrimary && sAttached && link_ready(), join_group, leave_group, NULL);
    UNLOCK_TCPIP_CORE();
    for (unsigned i = 0; i < DUA_QUEUE_DEPTH; ++i) {
        struct dua_request *request;
        if (xQueueReceive(sRequests, &request, 0) != pdTRUE) break;
        if (!send_na(instance, request->packet + request->target_offset, request)) {
            LOCK_TCPIP_CORE();
            ++sDrops;
            UNLOCK_TCPIP_CORE();
        }
        vPortFree(request);
    }
    /* Coalesce Added/Renewed announcements, bound work, and retry failures.
     * One announcement per event matches OTBR; this is not a retry-count rule. */
    for (unsigned n = 0, sent = 0; n < BR_DUA_CAPACITY && sent < DUA_QUEUE_DEPTH; ++n) {
        LOCK_TCPIP_CORE();
        refresh_budget();
        bool budget = sTxBudget != 0;
        UNLOCK_TCPIP_CORE();
        if (!budget) break;
        unsigned i = sAnnounceCursor++ % BR_DUA_CAPACITY;
        if (sTable.entries[i].live && sIdentity[i].announce &&
            send_na(instance, sTable.entries[i].address, NULL)) {
            LOCK_TCPIP_CORE();
            sIdentity[i].announce = false;
            UNLOCK_TCPIP_CORE();
            ++sent;
        }
    }
}

#ifdef CONFIG_SHELL
static void cmd_dua_stats(int argc, char **argv)
{
    ip6_addr_t address;
    if (argc > 2 || (argc == 2 && !ip6addr_aton(argv[1], &address))) {
        IPRO_LOGI("BR-DUA", "Usage: br_dua_stats [DUA]");
        return;
    }
    unsigned live = 0, owned = 0, active, primary, overflow, authorized = 0, values[6];
    LOCK_TCPIP_CORE();
    for (unsigned i = 0; i < BR_DUA_CAPACITY; ++i) {
        live += sTable.entries[i].live;
        owned += sTable.entries[i].owned;
    }
    active = sTable.active && link_ready(); primary = sPrimary; overflow = sTable.overflow;
    if (argc == 2) authorized = br_dua_can_route((const uint8_t *)address.addr);
    values[0] = sReplies; values[1] = sAnnouncements; values[2] = sDrops;
    values[3] = sErrors; values[4] = sJoins; values[5] = sLeaves;
    UNLOCK_TCPIP_CORE();
    IPRO_LOGI("BR-DUA", "primary=%u active=%u live=%u owned=%u overflow=%u authorized=%u",
              primary, active, live, owned, overflow, authorized);
    IPRO_LOGI("BR-DUA", "replies=%u announcements=%u drops=%u errors=%u joins=%u leaves=%u",
              values[0], values[1], values[2], values[3], values[4], values[5]);
}
SHELL_CMD_EXPORT_ALIAS(cmd_dua_stats, br_dua_stats, DUA proxy authorization and ND counters);
#endif
