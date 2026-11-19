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
#include "lwip/tcpip.h"
#include "lwip/udp.h"
#include "lwip/ip.h"
#include "lwip/mld6.h"
#include "fhost.h"
#include "wifi_mgmr_ext.h"
#include "openthread_port.h"
#include <openthread/platform/udp.h>
#include "openthread-core-config.h"
#include "br_thread_netif.h"
#include "br_udp.h"

#define SOCKET_COUNT 8
#define GROUP_COUNT 4
#define RX_DEPTH 4
#define MAX_UDP_SIZE 2048

struct membership {
    ip6_addr_t address;
    uint8_t index;
};
struct udp_slot {
    struct udp_pcb *pcb;
    otUdpSocket *owner;
    uint32_t generation;
    otNetifIdentifier binding;
    struct membership groups[GROUP_COUNT];
};
struct udp_rx {
    otMessageInfo info;
    uint32_t generation;
    uint16_t length;
    uint8_t slot;
    uint8_t data[];
};
static struct udp_slot sSlots[SOCKET_COUNT];
static StaticQueue_t sQueueControl;
static uint8_t sQueueStorage[RX_DEPTH * sizeof(struct udp_rx *)];
static QueueHandle_t sQueue;

/* All PCB/slot mutation is protected by the lwIP core lock. */
static struct udp_slot *find_slot(const otUdpSocket *socket)
{
    if (!socket) return NULL;
    for (unsigned i = 0; i < SOCKET_COUNT; ++i)
        if (sSlots[i].owner == socket && socket->mHandle == &sSlots[i]) return &sSlots[i];
    return NULL;
}

static struct netif *bound_interface(otNetifIdentifier binding)
{
    if (binding == OT_NETIF_BACKBONE) return fhost_to_net_if(MGMR_VIF_STA);
    if (binding == OT_NETIF_THREAD_HOST) return br_thread_netif_get();
    return NULL;
}

static void to_ot(otIp6Address *out, const ip_addr_t *in)
{
    memset(out, 0, sizeof(*out));
    if (IP_IS_V4(in)) {
        out->mFields.m8[10] = out->mFields.m8[11] = 0xff;
        memcpy(out->mFields.m8 + 12, &ip_2_ip4(in)->addr, 4);
    } else memcpy(out->mFields.m8, ip_2_ip6(in)->addr, 16);
}

static void from_ot(ip_addr_t *out, const otIp6Address *in, struct netif *netif)
{
    static const uint8_t mapped[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff};
    memset(out, 0, sizeof(*out));
    if (memcmp(in->mFields.m8, mapped, sizeof(mapped)) == 0) {
        IP_SET_TYPE_VAL(*out, IPADDR_TYPE_V4);
        memcpy(&ip_2_ip4(out)->addr, in->mFields.m8 + 12, 4);
    } else {
        IP_SET_TYPE_VAL(*out, IPADDR_TYPE_V6);
        memcpy(ip_2_ip6(out)->addr, in->mFields.m8, 16);
        if (netif) ip6_addr_assign_zone(ip_2_ip6(out), IP6_UNKNOWN, netif);
    }
}

static otError map_error(err_t error)
{
    if (error == ERR_OK) return OT_ERROR_NONE;
    if (error == ERR_MEM || error == ERR_BUF) return OT_ERROR_NO_BUFS;
    if (error == ERR_RTE) return OT_ERROR_NO_ROUTE;
    if (error == ERR_ARG || error == ERR_VAL) return OT_ERROR_INVALID_ARGS;
    return OT_ERROR_FAILED;
}

static void receive(void *context, struct udp_pcb *pcb, struct pbuf *p,
                    const ip_addr_t *source, u16_t port)
{
    struct udp_slot *slot = context;
    struct netif *input = ip_current_input_netif();
    if (!p) return;
    if (otrIsStopped() || !slot->owner || slot->pcb != pcb || !input || p->tot_len > MAX_UDP_SIZE ||
        uxQueueSpacesAvailable(sQueue) == 0) {
        pbuf_free(p);
        return;
    }
    struct udp_rx *event = pvPortMalloc(sizeof(*event) + p->tot_len);
    if (event) {
        memset(&event->info, 0, sizeof(event->info));
        event->generation = slot->generation;
        event->slot = (uint8_t)(slot - sSlots);
        event->length = p->tot_len;
        to_ot(&event->info.mPeerAddr, source);
        to_ot(&event->info.mSockAddr, ip_current_dest_addr());
        event->info.mPeerPort = port;
        event->info.mSockPort = pcb->local_port;
        event->info.mIsHostInterface = input != br_thread_netif_get();
        if (IP_IS_V6(source)) {
            event->info.mHopLimit = IP6H_HOPLIM(ip6_current_header());
            event->info.mEcn = IP6H_TC(ip6_current_header()) & 3;
        } else {
            event->info.mHopLimit = IPH_TTL(ip4_current_header());
            event->info.mEcn = IPH_TOS(ip4_current_header()) & 3;
        }
        if (pbuf_copy_partial(p, event->data, event->length, 0) != event->length ||
            xQueueSend(sQueue, &event, 0) != pdTRUE) vPortFree(event);
        else otSysEventSignalPending();
    }
    pbuf_free(p);
}

static void close_slot(struct udp_slot *slot)
{
    ++slot->generation;
    if (slot->pcb) udp_remove(slot->pcb);
    for (unsigned i = 0; i < GROUP_COUNT; ++i) {
        struct membership *group = &slot->groups[i];
        struct netif *netif = group->index ? netif_get_by_index(group->index) : NULL;
        if (netif) mld6_leavegroup_netif(netif, &group->address);
        group->index = 0;
    }
    slot->pcb = NULL;
    slot->owner = NULL;
}

otError otPlatUdpSocket(otUdpSocket *socket)
{
    if (!socket || socket->mHandle || socket->mNetifId == OT_NETIF_THREAD_INTERNAL)
        return OT_ERROR_INVALID_ARGS;
    if (!sQueue) sQueue = xQueueCreateStatic(RX_DEPTH, sizeof(struct udp_rx *), sQueueStorage, &sQueueControl);
    if (!sQueue) return OT_ERROR_NO_BUFS;
    otError result = OT_ERROR_NO_BUFS;
    LOCK_TCPIP_CORE();
    for (unsigned i = 0; i < SOCKET_COUNT; ++i) {
        struct udp_slot *slot = &sSlots[i];
        if (slot->owner) continue;
        slot->pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
        if (!slot->pcb) break;
        ++slot->generation;
        slot->owner = socket;
        slot->binding = socket->mNetifId;
        socket->mHandle = slot;
        udp_recv(slot->pcb, receive, slot);
        result = OT_ERROR_NONE;
        break;
    }
    UNLOCK_TCPIP_CORE();
    return result;
}

otError otPlatUdpClose(otUdpSocket *socket)
{
    if (!socket) return OT_ERROR_INVALID_ARGS;
    LOCK_TCPIP_CORE();
    struct udp_slot *slot = find_slot(socket);
    if (slot) close_slot(slot);
    socket->mHandle = NULL;
    UNLOCK_TCPIP_CORE();
    return OT_ERROR_NONE;
}

otError otPlatUdpBindToNetif(otUdpSocket *socket, otNetifIdentifier binding)
{
    if (binding != OT_NETIF_BACKBONE && binding != OT_NETIF_THREAD_HOST &&
        binding != OT_NETIF_UNSPECIFIED) return OT_ERROR_INVALID_ARGS;
    otError result = OT_ERROR_INVALID_STATE;
    LOCK_TCPIP_CORE();
    struct udp_slot *slot = find_slot(socket);
    struct netif *netif = bound_interface(binding);
    if (slot && (netif || binding == OT_NETIF_UNSPECIFIED)) {
        udp_bind_netif(slot->pcb, netif);
        slot->binding = binding;
        result = OT_ERROR_NONE;
    }
    UNLOCK_TCPIP_CORE();
    return result;
}

otError otPlatUdpBind(otUdpSocket *socket)
{
    otError result = OT_ERROR_INVALID_STATE;
    LOCK_TCPIP_CORE();
    struct udp_slot *slot = find_slot(socket);
    if (slot) {
        ip_addr_t address;
        from_ot(&address, &socket->mSockName.mAddress, bound_interface(slot->binding));
        const ip_addr_t *bind_address = ip_addr_isany(&address) ? IP_ANY_TYPE : &address;
        result = map_error(udp_bind(slot->pcb, bind_address, socket->mSockName.mPort));
        if (result == OT_ERROR_NONE) socket->mSockName.mPort = slot->pcb->local_port;
    }
    UNLOCK_TCPIP_CORE();
    return result;
}

otError otPlatUdpConnect(otUdpSocket *socket)
{
    otError result = OT_ERROR_INVALID_STATE;
    LOCK_TCPIP_CORE();
    struct udp_slot *slot = find_slot(socket);
    if (slot) {
        ip_addr_t peer;
        from_ot(&peer, &socket->mPeerName.mAddress, bound_interface(slot->binding));
        if (ip_addr_isany(&peer) || !socket->mPeerName.mPort) {
            udp_disconnect(slot->pcb);
            result = OT_ERROR_NONE;
        } else result = map_error(udp_connect(slot->pcb, &peer, socket->mPeerName.mPort));
    }
    UNLOCK_TCPIP_CORE();
    return result;
}

otError otPlatUdpSend(otUdpSocket *socket, otMessage *message, const otMessageInfo *info)
{
    if (!socket || !message || !info || !info->mPeerPort) return OT_ERROR_INVALID_ARGS;
    uint16_t length = otMessageGetLength(message);
    if (length > MAX_UDP_SIZE) return OT_ERROR_NO_BUFS;
    otError result = OT_ERROR_INVALID_STATE;
    LOCK_TCPIP_CORE();
    struct udp_slot *slot = find_slot(socket);
    if (slot) {
        struct netif *netif = bound_interface(slot->binding);
        ip_addr_t source, destination;
        from_ot(&source, &info->mSockAddr, netif);
        from_ot(&destination, &info->mPeerAddr, netif);
        if (!netif && slot->binding == OT_NETIF_UNSPECIFIED) {
            if (IP_IS_V6(&destination) && ip6_addr_islinklocal(ip_2_ip6(&destination)))
                netif = info->mIsHostInterface ? fhost_to_net_if(MGMR_VIF_STA) : br_thread_netif_get();
            else netif = ip_route(&source, &destination);
        }
        if (netif && netif_is_up(netif) && netif_is_link_up(netif) && !ip_addr_isany(&destination)) {
            from_ot(&source, &info->mSockAddr, netif);
            from_ot(&destination, &info->mPeerAddr, netif);
            struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);
            result = OT_ERROR_NO_BUFS;
            if (p && p->len == length && otMessageRead(message, 0, p->payload, length) == length) {
                slot->pcb->ttl = info->mHopLimit || info->mAllowZeroHopLimit ?
                    info->mHopLimit : OPENTHREAD_CONFIG_IP6_HOP_LIMIT_DEFAULT;
                slot->pcb->mcast_ttl = slot->pcb->ttl;
                slot->pcb->tos = info->mEcn;
                udp_clear_flags(slot->pcb, UDP_FLAGS_MULTICAST_LOOP);
                if (info->mMulticastLoop) udp_set_flags(slot->pcb, UDP_FLAGS_MULTICAST_LOOP);
                err_t error = ip_addr_isany(&source) ?
                    udp_sendto_if(slot->pcb, p, &destination, info->mPeerPort, netif) :
                    udp_sendto_if_src(slot->pcb, p, &destination, info->mPeerPort, netif, &source);
                result = map_error(error);
            }
            if (p) pbuf_free(p);
        } else result = OT_ERROR_NO_ROUTE;
    }
    UNLOCK_TCPIP_CORE();
    if (result == OT_ERROR_NONE) otMessageFree(message);
    return result;
}

/* Backbone groups use lwIP MLD. Thread multicast membership mirroring is a
 * separate data-plane gate; never pretend it is implemented by joining Wi-Fi. */
static otError multicast_group(otUdpSocket *socket, otNetifIdentifier binding,
                                const otIp6Address *address, bool join)
{
    if (!address || address->mFields.m8[0] != 0xff) return OT_ERROR_INVALID_ARGS;
    if (binding != OT_NETIF_BACKBONE) return OT_ERROR_NOT_IMPLEMENTED;
    otError result = OT_ERROR_INVALID_STATE;
    LOCK_TCPIP_CORE();
    struct udp_slot *slot = find_slot(socket);
    struct netif *netif = bound_interface(binding);
    if (slot && netif) {
        ip6_addr_t group;
        memcpy(group.addr, address->mFields.m8, 16);
        ip6_addr_clear_zone(&group);
        ip6_addr_assign_zone(&group, IP6_MULTICAST, netif);
        uint8_t index = netif_get_index(netif);
        struct membership *free_entry = NULL, *entry = NULL;
        for (unsigned i = 0; i < GROUP_COUNT; ++i) {
            struct membership *candidate = &slot->groups[i];
            if (!candidate->index) free_entry = candidate;
            else if (candidate->index == index && ip6_addr_cmp(&candidate->address, &group)) entry = candidate;
        }
        if (join && entry) result = OT_ERROR_NONE;
        else if (!join && !entry) result = OT_ERROR_NOT_FOUND;
        else if (join && !free_entry) result = OT_ERROR_NO_BUFS;
        else if (join) {
            result = map_error(mld6_joingroup_netif(netif, &group));
            if (result == OT_ERROR_NONE) { free_entry->address = group; free_entry->index = index; }
        } else {
            result = map_error(mld6_leavegroup_netif(netif, &group));
            if (result == OT_ERROR_NONE) entry->index = 0;
        }
    }
    UNLOCK_TCPIP_CORE();
    return result;
}

otError otPlatUdpJoinMulticastGroup(otUdpSocket *socket, otNetifIdentifier binding, const otIp6Address *address)
{
    return multicast_group(socket, binding, address, true);
}

otError otPlatUdpLeaveMulticastGroup(otUdpSocket *socket, otNetifIdentifier binding, const otIp6Address *address)
{
    return multicast_group(socket, binding, address, false);
}

void br_udp_drop_queued(void)
{
    struct udp_rx *event;
    while (sQueue && xQueueReceive(sQueue, &event, 0) == pdTRUE) vPortFree(event);
}

void br_udp_process(otInstance *instance)
{
    /* CLI may switch a platform socket to THREAD_INTERNAL. The vendored core
     * then skips Plat::Close. Reconcile against live OT sockets without ever
     * dereferencing an owner that is no longer in the core's socket list. */
    LOCK_TCPIP_CORE();
    for (unsigned i = 0; i < SOCKET_COUNT; ++i) {
        struct udp_slot *slot = &sSlots[i];
        if (!slot->owner) continue;
        otUdpSocket *live = otUdpGetSockets(instance);
        while (live && live != slot->owner) live = live->mNext;
        if (!live || live->mHandle != slot || live->mNetifId == OT_NETIF_THREAD_INTERNAL) {
            if (live && live->mHandle == slot) live->mHandle = NULL;
            close_slot(slot);
        }
    }
    UNLOCK_TCPIP_CORE();
    struct udp_rx *event;
    for (unsigned i = 0; sQueue && i < RX_DEPTH && xQueueReceive(sQueue, &event, 0) == pdTRUE; ++i) {
        struct udp_slot *slot = &sSlots[event->slot];
        if (slot->owner && slot->owner->mHandle == slot && slot->generation == event->generation) {
            otMessageSettings settings = {false, OT_MESSAGE_PRIORITY_NORMAL};
            otMessage *message = otUdpNewMessage(instance, &settings);
            if (message) {
                if (otMessageAppend(message, event->data, event->length) == OT_ERROR_NONE && slot->owner->mHandler)
                    slot->owner->mHandler(slot->owner->mContext, message, &event->info);
                otMessageFree(message);
            }
        }
        vPortFree(event);
    }
}
