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

#include "br_nat64.h"

#if defined(CONFIG_THREAD_BR_NAT64) && CONFIG_THREAD_BR_NAT64
#include <string.h>
#include <openthread/nat64.h>
#include <openthread/message.h>
#include "lwip/ip4.h"
#include "lwip/ip4_napt.h"
#include "lwip/lwip_napt.h"
#include "lwip/tcpip.h"
#include "fhost.h"
#include "wifi_mgmr_ext.h"
#include "openthread_port.h"
#include "br_wifi_link_epoch.h"

#if !IP_NAPT || !IP_FORWARD
#error "The NAT64 boundary requires lwIP NAPT and IPv4 forwarding"
#endif

#ifndef BR_NAT64_SESSIONS
#define BR_NAT64_SESSIONS 32
#endif
#define NAT64_RX_DEPTH 2
#define NAT64_MAX_PACKET 1500

struct uplink_snapshot {
    struct netif *netif;
    ip4_addr_t address, mask, gateway;
    uint32_t epoch;
    uint8_t index;
};

/* Shared state and pbuf ownership are protected by the lwIP core lock. OT
 * configuration/callbacks run on its owner; no OT API is called under that lock. */
static struct {
    struct netif interface;
    struct uplink_snapshot uplink;
    struct pbuf *rx[NAT64_RX_DEPTH];
    unsigned head, count;
    bool initialized, ready;
} sBoundary;
static otInstance *sOwner;
static bool sConfigured;

static void private_addresses(ip4_addr_t *local, ip4_addr_t *translator)
{
    IP4_ADDR(local, 192, 0, 0, 1);
    IP4_ADDR(translator, 192, 0, 0, 2);
}

static bool snapshot(struct uplink_snapshot *out)
{
    struct netif *wifi = fhost_to_net_if(MGMR_VIF_STA);
    uint32_t epoch = br_wifi_link_epoch();
    if (!wifi || !epoch || !netif_is_up(wifi) || !netif_is_link_up(wifi) ||
        ip4_addr_isany_val(*netif_ip4_addr(wifi)) || netif_default != wifi)
        return false;
    out->netif = wifi;
    out->index = netif_get_index(wifi);
    out->epoch = epoch;
    out->address = *netif_ip4_addr(wifi);
    out->mask = *netif_ip4_netmask(wifi);
    out->gateway = *netif_ip4_gw(wifi);
    return true;
}

static bool same_uplink(const struct uplink_snapshot *a, const struct uplink_snapshot *b)
{
    return a->netif == b->netif && a->index == b->index && a->epoch == b->epoch &&
           ip4_addr_cmp(&a->address, &b->address) && ip4_addr_cmp(&a->mask, &b->mask) &&
           ip4_addr_cmp(&a->gateway, &b->gateway);
}

static bool private_range_available(void);

static bool live(void)
{
    struct uplink_snapshot current;
    return sBoundary.ready && sBoundary.interface.napt && ip_napt_is_active() &&
           netif_is_up(&sBoundary.interface) && netif_is_link_up(&sBoundary.interface) &&
           snapshot(&current) && !current.netif->napt &&
           same_uplink(&current, &sBoundary.uplink) && private_range_available();
}

static void withdraw(void)
{
    sBoundary.ready = false;
    if (sBoundary.initialized) {
        netif_set_link_down(&sBoundary.interface);
        netif_set_down(&sBoundary.interface);
        ip_napt_enable_no(sBoundary.interface.num, 0);
    }
    while (sBoundary.count) {
        pbuf_free(sBoundary.rx[sBoundary.head]);
        sBoundary.rx[sBoundary.head] = NULL;
        sBoundary.head = (sBoundary.head + 1) % NAT64_RX_DEPTH;
        --sBoundary.count;
    }
    sBoundary.head = 0;
}

static bool private_range_available(void)
{
    struct netif *netif;
    ip4_addr_t local, translator;
    private_addresses(&local, &translator);
    NETIF_FOREACH(netif) {
        if (netif != &sBoundary.interface && netif_is_up(netif) &&
            !ip4_addr_isany_val(*netif_ip4_addr(netif)) &&
            (ip4_addr_netcmp(&local, netif_ip4_addr(netif), netif_ip4_netmask(netif)) ||
             ip4_addr_netcmp(&translator, netif_ip4_addr(netif), netif_ip4_netmask(netif)) ||
             ip4_addr_cmp(&local, netif_ip4_gw(netif)) ||
             ip4_addr_cmp(&translator, netif_ip4_gw(netif))))
            return false;
    }
    return true;
}

static err_t ipv4_return(struct netif *netif, struct pbuf *p, const ip4_addr_t *destination)
{
    (void)netif;
    if (!live() || !(p->flags & IP_NAPT_RECV_FLAG) ||
        !ip4_addr_cmp(destination, netif_ip4_gw(&sBoundary.interface)))
        return ERR_RTE;
    if (p->tot_len < IP_HLEN || p->tot_len > NAT64_MAX_PACKET)
        return ERR_VAL;
    if (sBoundary.count == NAT64_RX_DEPTH)
        return ERR_MEM;
    struct pbuf *copy = pbuf_clone(PBUF_RAW, PBUF_RAM, p);
    if (!copy) return ERR_MEM;
    unsigned slot = (sBoundary.head + sBoundary.count) % NAT64_RX_DEPTH;
    sBoundary.rx[slot] = copy;
    ++sBoundary.count;
    otSysEventSignalPending();
    return ERR_OK;
}

static err_t interface_init(struct netif *netif)
{
    netif->name[0] = 'n'; netif->name[1] = '6';
    netif->flags = 0;
    /* No second fragmentation stage here. The OT translation/IPv6 path must
     * handle its own MTU; the queue retains complete IPv4 datagrams. */
    netif->mtu = 0;
    netif->output = ipv4_return;
    return ERR_OK;
}

static void ipv4_from_thread(otMessage *message, void *context)
{
    (void)context;
    uint16_t length = otMessageGetLength(message);
    struct pbuf *p = NULL;
    LOCK_TCPIP_CORE();
    if (live() && length >= IP_HLEN && length <= NAT64_MAX_PACKET)
        p = pbuf_alloc(PBUF_LINK, length, PBUF_RAM);
    UNLOCK_TCPIP_CORE();
    bool copied = p && otMessageRead(message, 0, p->payload, length) == length;
    otMessageFree(message);
    if (!p) return;
    LOCK_TCPIP_CORE();
    ip4_addr_t source, destination;
    bool valid = copied && live() && pbuf_copy_partial(p, &source, 4, 12) == 4 &&
                 pbuf_copy_partial(p, &destination, 4, 16) == 4 &&
                 ip4_addr_cmp(&source, netif_ip4_gw(&sBoundary.interface)) &&
                 ip4_route(&destination) == sBoundary.uplink.netif;
    if (valid) {
        /* Synchronous input under the core lock: a queued tcpip message must
         * never inject an old association's translated packet on a new link. */
        ip4_input(p, &sBoundary.interface); /* consumes p on every outcome */
    } else {
        pbuf_free(p);
    }
    UNLOCK_TCPIP_CORE();
}

void br_nat64_process(otInstance *instance, bool forwarding_ready)
{
    if (!instance) return;
    if (!sOwner) {
        sOwner = instance;
        otNat64SetReceiveIp4Callback(instance, ipv4_from_thread, NULL);
        otNat64SetEnabled(instance, false);
    }
    if (instance != sOwner) return;
    struct uplink_snapshot current;
    LOCK_TCPIP_CORE();
    bool usable = forwarding_ready && snapshot(&current) && private_range_available();
    bool unchanged = usable && live();
    if (!unchanged) withdraw();
    UNLOCK_TCPIP_CORE();
    if (!unchanged && sConfigured) {
        otNat64SetEnabled(instance, false);
        otNat64ClearIp4Cidr(instance);
        sConfigured = false;
    }
    if (usable && !sConfigured) {
        LOCK_TCPIP_CORE();
        if (!sBoundary.initialized) {
            ip4_addr_t local, translator, mask;
            private_addresses(&local, &translator);
            IP4_ADDR(&mask, 255, 255, 255, 255);
            sBoundary.initialized = netif_add(&sBoundary.interface, &local, &mask, &translator,
                                               NULL, interface_init, ip4_input) != NULL;
        }
        bool allocated = sBoundary.initialized &&
                         ip_napt_enable_netif(&sBoundary.interface, BR_NAT64_SESSIONS, 0) == ERR_OK;
        UNLOCK_TCPIP_CORE();
        otIp4Cidr cidr = { .mAddress = { .mFields = { .m8 = {192, 0, 0, 2} } }, .mLength = 32 };
        if (allocated && otNat64SetIp4Cidr(instance, &cidr) == OT_ERROR_NONE) {
            sConfigured = true;
            LOCK_TCPIP_CORE();
            struct uplink_snapshot confirmed;
            if (snapshot(&confirmed) && same_uplink(&current, &confirmed) && private_range_available()) {
                sBoundary.uplink = confirmed;
                sBoundary.ready = true;
                netif_set_up(&sBoundary.interface);
                netif_set_link_up(&sBoundary.interface);
            }
            bool ready = sBoundary.ready;
            UNLOCK_TCPIP_CORE();
            otNat64SetEnabled(instance, ready);
        }
        LOCK_TCPIP_CORE();
        if (!sBoundary.ready) withdraw();
        UNLOCK_TCPIP_CORE();
    }
    for (unsigned count = 0; count < NAT64_RX_DEPTH; ++count) {
        LOCK_TCPIP_CORE();
        if (!live()) withdraw();
        struct pbuf *p = NULL;
        uint32_t delivery_epoch = sBoundary.uplink.epoch;
        if (sBoundary.count) {
            p = sBoundary.rx[sBoundary.head];
            sBoundary.rx[sBoundary.head] = NULL;
            sBoundary.head = (sBoundary.head + 1) % NAT64_RX_DEPTH;
            --sBoundary.count;
        }
        UNLOCK_TCPIP_CORE();
        if (!p) break;
        otMessage *message = otIp4NewMessage(instance, NULL);
        bool appended = message && otMessageAppend(message, p->payload, p->tot_len) == OT_ERROR_NONE;
        LOCK_TCPIP_CORE();
        pbuf_free(p);
        bool valid = live();
        UNLOCK_TCPIP_CORE();
        if (appended && valid && br_wifi_link_epoch() == delivery_epoch) {
            otError error = otNat64Send(instance, message); /* always consumes */
            (void)error;
        } else if (message) otMessageFree(message);
    }
}
#else
void br_nat64_process(otInstance *instance, bool forwarding_ready)
{
    (void)instance;
    (void)forwarding_ready;
}
#endif
