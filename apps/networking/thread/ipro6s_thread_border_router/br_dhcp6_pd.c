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
#include "lwip/tcpip.h"
#include "lwip/dhcp6.h"
#include "fhost.h"
#include "wifi_mgmr_ext.h"
#include "openthread_port.h"
#include <openthread/link.h>
#include <openthread/platform/infra_if.h>
#include "br_dhcp6_pd.h"
#include "br_wifi_link_epoch.h"

/* Fixed storage: no allocation or OpenThread calls on network ingress. The OT
 * owner also uses static copy buffers rather than large task-stack frames. */
#define PD_MAX_PAYLOAD 2048
#define PD_RX_DEPTH 2
struct pd_packet {
    uint16_t length;
    uint8_t data[PD_MAX_PAYLOAD];
};
static struct {
    otInstance *owner;
    uint32_t index;
    uint32_t epoch;
    uint8_t duid[12];
    bool attached;
    unsigned head, count;
    struct pbuf *queue[PD_RX_DEPTH];
} sClient;
static struct pd_packet sDelivery;
/* RX delivery and TX run on the single OT owner. Append copies the complete
 * delivery into an independent otMessage before HandleReceived can send a
 * synchronous response, so TX can reuse this scratch without touching RX. */

static int receive(void *context, struct netif *netif, struct pbuf *p,
                   const ip_addr_t *source, u16_t port);

/* All shared client state and lwIP operations are under the core lock. Flushing
 * on every listener/association change is the queue's generation fence. */
static void detach(void)
{
    if (sClient.attached) dhcp6_external_detach(receive, &sClient);
    sClient.attached = false;
    while (sClient.count) {
        struct pbuf *p = sClient.queue[sClient.head];
        sClient.queue[sClient.head] = NULL;
        sClient.head = (sClient.head + 1) % PD_RX_DEPTH;
        --sClient.count;
        pbuf_free(p);
    }
    sClient.head = 0;
}

void br_dhcp6_pd_stop(void)
{
    LOCK_TCPIP_CORE();
    detach();
    UNLOCK_TCPIP_CORE();
}

static struct netif *ready_interface(void)
{
    struct netif *netif = fhost_to_net_if(MGMR_VIF_STA);
    uint32_t epoch = br_wifi_link_epoch();
    if (!sClient.owner || !netif || netif_get_index(netif) != sClient.index || !epoch ||
        !netif_is_up(netif) || !netif_is_link_up(netif) ||
        !ip6_addr_isvalid(netif_ip6_addr_state(netif, 0)) ||
        !ip6_addr_islinklocal(netif_ip6_addr(netif, 0))) {
        detach();
        return NULL;
    }
    /* Only native listener restart can authorize a different association.
     * Never renew/release an old lease on a newly connected network. */
    if (sClient.epoch != epoch) {
        detach();
        return NULL;
    }
    return netif;
}

static struct netif *attach(void)
{
    struct netif *netif = ready_interface();
    if (netif && !sClient.attached)
        sClient.attached = dhcp6_external_attach(receive, &sClient) == ERR_OK;
    return sClient.attached ? netif : NULL;
}

/* Bounded TLV walk only for demultiplexing, not a second DHCP client parser.
 * The native client validates XID, ServerID, IA_PD and all lease semantics. */
enum ownership { UNOWNED, OWN_MALFORMED, OWN_VALID };
static enum ownership matches_client(const struct pbuf *p)
{
    bool owned = false, valid = true, found = false;
    for (unsigned offset = 4; offset < p->tot_len;) {
        uint8_t header[4];
        if (p->tot_len - offset < sizeof(header) ||
            pbuf_copy_partial(p, header, sizeof(header), offset) != sizeof(header))
            return owned ? OWN_MALFORMED : UNOWNED;
        unsigned type = ((unsigned)header[0] << 8) | header[1];
        unsigned length = ((unsigned)header[2] << 8) | header[3];
        offset += sizeof(header);
        if (type == 1) {
            uint8_t duid[12];
            bool match = pbuf_copy_partial(p, duid, sizeof(duid), offset) == sizeof(duid) &&
                         !memcmp(duid, sClient.duid, sizeof(duid));
            owned |= match;
            if (found || length != sizeof(duid) || !match) valid = false;
            found = true;
        }
        if (length > p->tot_len - offset) return owned ? OWN_MALFORMED : UNOWNED;
        offset += length;
    }
    return !owned ? UNOWNED : valid ? OWN_VALID : OWN_MALFORMED;
}

static int receive(void *context, struct netif *netif, struct pbuf *p,
                   const ip_addr_t *source, u16_t port)
{
    (void)context;
    /* The shared UDP ingress has already checked checksum, UDP length/port. */
    if (!p || !source || !IP_IS_V6(source) || port != 547 ||
        ip6_addr_isany(ip_2_ip6(source)) || ip6_addr_ismulticast(ip_2_ip6(source)) ||
        p->tot_len < 20 || p->tot_len > PD_MAX_PAYLOAD ||
        netif != ready_interface() || !sClient.attached) return 0;
    enum ownership owner = matches_client(p);
    if (owner == UNOWNED) return 0;
    if (owner == OWN_MALFORMED) return 1;
    /* Wi-Fi custom RX pbufs release their backing buffers only on final free.
     * A bare borrowed PBUF_REF has no such lifetime guarantee: do not retain it.
     * The common DHCP callback releases its own reference after we return. */
    for (const struct pbuf *part = p; part; part = part->next)
        if (PBUF_NEEDS_COPY(part) && !(part->flags & PBUF_FLAG_IS_CUSTOM)) return 1;
    if (otrIsStopped()) return 1; /* OpenThread will never drain the queue */
    /* Claimed packets never fall through to the stateless client, including
     * queue overflow. References pin at most two datagrams (and their chains);
     * no allocation or OT call occurs here. */
    if (sClient.count < PD_RX_DEPTH) {
        pbuf_ref(p);
        sClient.queue[(sClient.head + sClient.count) % PD_RX_DEPTH] = p;
        ++sClient.count;
        otSysEventSignalPending();
    }
    return 1;
}

void otPlatInfraIfDhcp6PdClientSetListeningEnabled(otInstance *instance, bool enable, uint32_t index)
{
    uint8_t duid[12] = {0, 3, 0, 27}; /* DUID-LL, EUI-64 hardware type. */
    if (enable && instance) memcpy(duid + 4, otLinkGetExtendedAddress(instance)->m8, 8);
    LOCK_TCPIP_CORE();
    if (!enable) {
        if (sClient.owner == instance && sClient.index == index) {
            detach();
            sClient.owner = NULL;
        }
    } else if (instance && index) {
        if (sClient.owner != instance || sClient.index != index ||
            memcmp(sClient.duid, duid, sizeof(duid))) detach();
        sClient.owner = instance;
        sClient.index = index;
        sClient.epoch = br_wifi_link_epoch();
        memcpy(sClient.duid, duid, sizeof(duid));
        (void)attach();
    }
    UNLOCK_TCPIP_CORE();
}

void br_dhcp6_pd_process(otInstance *instance)
{
    for (unsigned i = 0; i < PD_RX_DEPTH; ++i) {
        uint32_t index = 0, epoch = 0;
        LOCK_TCPIP_CORE();
        if (instance == sClient.owner && attach() && sClient.count) {
            struct pbuf *p = sClient.queue[sClient.head];
            sClient.queue[sClient.head] = NULL;
            sClient.head = (sClient.head + 1) % PD_RX_DEPTH;
            --sClient.count;
            sDelivery.length = p->tot_len;
            if (pbuf_copy_partial(p, sDelivery.data, sDelivery.length, 0) == sDelivery.length) {
                index = sClient.index;
                epoch = sClient.epoch;
            }
            pbuf_free(p);
        }
        UNLOCK_TCPIP_CORE();
        if (!index) break;
        if (br_wifi_link_epoch() != epoch) continue;
        otMessage *message = otIp6NewMessage(instance, NULL);
        if (!message) continue;
        if (otMessageAppend(message, sDelivery.data, sDelivery.length) != OT_ERROR_NONE ||
            br_wifi_link_epoch() != epoch)
            otMessageFree(message);
        else otPlatInfraIfDhcp6PdClientHandleReceived(instance, message, index);
    }
}

void otPlatInfraIfDhcp6PdClientSend(otInstance *instance, otMessage *message,
                                  otIp6Address *destination, uint32_t index)
{
    static const uint8_t servers[16] = {0xff, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2};
    if (!message) return;
    uint16_t length = otMessageGetLength(message);
    ip6_addr_t address = {0};
    if (!destination || length < 4 || length > sizeof(sDelivery.data)) goto exit;
    memcpy(address.addr, destination->mFields.m8, 16);
    if (ip6_addr_isany(&address) ||
        (ip6_addr_ismulticast(&address) && memcmp(address.addr, servers, sizeof(servers))) ||
        otMessageRead(message, 0, sDelivery.data, length) != length) goto exit;
    LOCK_TCPIP_CORE();
    if (instance == sClient.owner && index == sClient.index) {
        struct netif *netif = attach();
        if (netif) {
            struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);
            if (p) {
                if (pbuf_take(p, sDelivery.data, length) == ERR_OK)
                    (void)dhcp6_external_send(netif, p, &address);
                pbuf_free(p);
            }
        }
    }
    UNLOCK_TCPIP_CORE();
exit:
    otMessageFree(message);
}
