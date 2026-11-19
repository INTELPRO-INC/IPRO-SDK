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
#include "lwip/igmp.h"
#include "lwip/mld6.h"
#include "openthread_port.h"
#include <openthread/mdns.h>
#include <openthread/platform/mdns_socket.h>
#include "ipro_log.h"
#include "br_mdns.h"
#ifdef CONFIG_THREAD_BR_LAN_MANAGEMENT
#include "br_lan_management.h"
#endif
#include "br_matter_publish.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define MDNS_PORT 5353
#define RX_DEPTH 4
#define MAX_DNS_SIZE 1500
#define ADDRESS_COUNT (LWIP_IPV6_NUM_ADDRESSES + 1)

struct mdns_rx {
    otPlatMdnsAddressInfo source;
    uint32_t generation;
    uint16_t length;
    bool unicast;
    uint8_t data[];
};

static StaticQueue_t sQueueControl;
static uint8_t sQueueStorage[RX_DEPTH * sizeof(struct mdns_rx *)];
static QueueHandle_t sQueue;
static struct udp_pcb *sPcb;
static uint32_t sIndex;
static uint32_t sGeneration;
static otIp6Address sAddresses[ADDRESS_COUNT];
static unsigned sAddressCount;
static bool sAddressesReported;

otError ipro_matter_dnssd_get_interface(otInstance *instance, uint32_t *interface_index)
{
    if (!interface_index) return OT_ERROR_INVALID_ARGS;
    *interface_index = 0;
    if (!instance) return OT_ERROR_INVALID_ARGS;
    if (instance != otrGetInstanceIfReady() || !otMdnsIsEnabled(instance) || !sPcb || !sIndex)
        return OT_ERROR_INVALID_STATE;
    /* sPcb/sIndex are changed only by the OT-owned enable/disable path (which
     * additionally holds lwIP core lock); holding OT excludes those writers. */
    *interface_index = sIndex;
    return OT_ERROR_NONE;
}

/* Updated under the lwIP core lock, including multicast loopback RX. */
static struct {
    uint32_t rx4, rx6, tx4, tx6, loop4, loop6, drops, tx_errors;
} sStats;

static void to_ot_address(otIp6Address *out, const ip_addr_t *in)
{
    memset(out, 0, sizeof(*out));
    if (IP_IS_V4(in)) {
        out->mFields.m8[10] = out->mFields.m8[11] = 0xff;
        memcpy(out->mFields.m8 + 12, &ip_2_ip4(in)->addr, 4);
    } else memcpy(out->mFields.m8, ip_2_ip6(in)->addr, 16);
}

static void to_lwip_address(ip_addr_t *out, const otIp6Address *in, struct netif *netif)
{
    static const uint8_t mapped[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff};
    memset(out, 0, sizeof(*out));
    if (memcmp(in->mFields.m8, mapped, sizeof(mapped)) == 0) {
        IP_SET_TYPE_VAL(*out, IPADDR_TYPE_V4);
        memcpy(&ip_2_ip4(out)->addr, in->mFields.m8 + 12, 4);
    } else {
        IP_SET_TYPE_VAL(*out, IPADDR_TYPE_V6);
        memcpy(ip_2_ip6(out)->addr, in->mFields.m8, 16);
        ip6_addr_assign_zone(ip_2_ip6(out), IP6_UNKNOWN, netif);
    }
}

/* lwIP core context: take a bounded copy; never call into OT here. */
static void receive(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                    const ip_addr_t *source, u16_t port)
{
    (void)arg;
    if (!p) return;
    struct netif *input = ip_current_input_netif();
    if (otrIsStopped() || pcb != sPcb || !input || netif_get_index(input) != sIndex ||
        p->tot_len < 12 || p->tot_len > MAX_DNS_SIZE ||
        uxQueueSpacesAvailable(sQueue) == 0) {
        ++sStats.drops;
        pbuf_free(p);
        return;
    }
    struct mdns_rx *event = pvPortMalloc(sizeof(*event) + p->tot_len);
    if (event) {
        bool loop = false;
        if (IP_IS_V4(source)) {
            ++sStats.rx4;
            loop = ip4_addr_cmp(ip_2_ip4(source), netif_ip4_addr(input));
            if (loop && ip_addr_ismulticast(ip_current_dest_addr())) ++sStats.loop4;
        } else {
            ++sStats.rx6;
            for (unsigned i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i)
                if (ip6_addr_isvalid(netif_ip6_addr_state(input, i)) &&
                    ip6_addr_cmp(ip_2_ip6(source), netif_ip6_addr(input, i))) loop = true;
            if (loop && ip_addr_ismulticast(ip_current_dest_addr())) ++sStats.loop6;
        }
        to_ot_address(&event->source.mAddress, source);
        event->source.mPort = port;
        event->source.mInfraIfIndex = sIndex;
        event->generation = sGeneration;
        event->length = p->tot_len;
        event->unicast = !ip_addr_ismulticast(ip_current_dest_addr());
        if (pbuf_copy_partial(p, event->data, event->length, 0) != event->length ||
            xQueueSend(sQueue, &event, 0) != pdTRUE) {
            ++sStats.drops;
            vPortFree(event);
        }
        else otSysEventSignalPending();
    } else ++sStats.drops;
    pbuf_free(p);
}

static void multicast_addresses(ip4_addr_t *v4, ip6_addr_t *v6, struct netif *netif)
{
    IP4_ADDR(v4, 224, 0, 0, 251);
    ip6addr_aton("ff02::fb", v6);
    ip6_addr_assign_zone(v6, IP6_MULTICAST, netif);
}

/* Caller holds the core lock. Invalidate queued RX before removing a PCB. */
static void close_socket(void)
{
    ++sGeneration;
    if (sPcb) {
        struct netif *netif = netif_get_by_index((uint8_t)sIndex);
        udp_remove(sPcb);
        sPcb = NULL;
        if (netif) {
            ip4_addr_t v4;
            ip6_addr_t v6;
            multicast_addresses(&v4, &v6, netif);
            igmp_leavegroup_netif(netif, &v4);
            mld6_leavegroup_netif(netif, &v6);
        }
    }
    sIndex = 0;
    sAddressCount = 0;
    sAddressesReported = false;
}

otError otPlatMdnsSetListeningEnabled(otInstance *instance, bool enable, uint32_t index)
{
    (void)instance;
#if defined(CONFIG_MATTER_ENABLE) && CONFIG_MATTER_ENABLE
    if (!enable) {
        br_matter_publish_network_down(instance);
        br_matter_browse_network_down(instance);
        br_matter_resolve_network_down(instance);
    }
#endif
    if (enable && (!index || index > 255)) return OT_ERROR_INVALID_ARGS;
    if (!sQueue) {
        sQueue = xQueueCreateStatic(RX_DEPTH, sizeof(struct mdns_rx *),
                                   sQueueStorage, &sQueueControl);
        if (!sQueue) return OT_ERROR_NO_BUFS;
    }
    otError result = OT_ERROR_FAILED;
    LOCK_TCPIP_CORE();
    if (!enable) {
        close_socket();
        result = OT_ERROR_NONE;
    } else if (!sPcb) {
        struct netif *netif = netif_get_by_index((uint8_t)index);
        if (netif && netif_is_up(netif) && netif_is_link_up(netif)) {
            struct udp_pcb *pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
            ip4_addr_t v4;
            ip6_addr_t v6;
            multicast_addresses(&v4, &v6, netif);
            if (pcb) {
                udp_bind_netif(pcb, netif);
                if (udp_bind(pcb, IP_ANY_TYPE, MDNS_PORT) == ERR_OK &&
                    igmp_joingroup_netif(netif, &v4) == ERR_OK) {
                    if (mld6_joingroup_netif(netif, &v6) == ERR_OK) {
                        sPcb = pcb;
                        sIndex = index;
                        ++sGeneration;
                        pcb->ttl = 255;
                        pcb->mcast_ttl = 255;
                        /* lwIP loops multicast and self-unicast through its input path. */
                        udp_set_flags(pcb, UDP_FLAGS_MULTICAST_LOOP);
                        udp_recv(pcb, receive, NULL);
                        result = OT_ERROR_NONE;
                    } else igmp_leavegroup_netif(netif, &v4);
                }
                if (result != OT_ERROR_NONE) udp_remove(pcb);
            } else result = OT_ERROR_NO_BUFS;
        }
    }
    UNLOCK_TCPIP_CORE();
    return result;
}

static void send_packet(struct pbuf *p, const ip_addr_t *dest, uint16_t port, struct netif *netif)
{
    if (udp_sendto_if(sPcb, p, dest, port, netif) != ERR_OK) ++sStats.tx_errors;
    else if (IP_IS_V4(dest)) ++sStats.tx4;
    else ++sStats.tx6;
}

/* All send callbacks run in OT context and unconditionally consume message. */
static void send_message(otMessage *message, uint32_t index,
                         const otPlatMdnsAddressInfo *destination)
{
    if (!message) return;
    uint16_t length = otMessageGetLength(message);
    LOCK_TCPIP_CORE();
    struct netif *netif = sPcb && index == sIndex ? netif_get_by_index((uint8_t)index) : NULL;
    if (netif && netif_is_up(netif) && netif_is_link_up(netif) &&
        length >= 12 && length <= MAX_DNS_SIZE) {
        struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);
        if (p && p->len == length && otMessageRead(message, 0, p->payload, length) == length) {
            ip_addr_t dest;
            if (destination) {
                to_lwip_address(&dest, &destination->mAddress, netif);
                send_packet(p, &dest, destination->mPort, netif);
            } else {
                /* Separate pbufs: UDP/IP output temporarily changes payload/header state. */
                ip_addr_set_zero_ip4(&dest);
                IP4_ADDR(ip_2_ip4(&dest), 224, 0, 0, 251);
                send_packet(p, &dest, MDNS_PORT, netif);
                pbuf_free(p);
                p = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);
                if (p && p->len == length && otMessageRead(message, 0, p->payload, length) == length) {
                    ip_addr_set_zero_ip6(&dest);
                    ip6addr_aton("ff02::fb", ip_2_ip6(&dest));
                    ip6_addr_assign_zone(ip_2_ip6(&dest), IP6_MULTICAST, netif);
                    send_packet(p, &dest, MDNS_PORT, netif);
                }
            }
        }
        if (p) pbuf_free(p);
    }
    UNLOCK_TCPIP_CORE();
    otMessageFree(message);
}

void otPlatMdnsSendMulticast(otInstance *instance, otMessage *message, uint32_t index)
{
    (void)instance;
    send_message(message, index, NULL);
}

void otPlatMdnsSendUnicast(otInstance *instance, otMessage *message,
                          const otPlatMdnsAddressInfo *destination)
{
    (void)instance;
    if (!destination) {
        if (message) otMessageFree(message);
        return;
    }
    send_message(message, destination->mInfraIfIndex, destination);
}

static void report_addresses(otInstance *instance)
{
    otIp6Address addresses[ADDRESS_COUNT];
    unsigned count = 0;
    LOCK_TCPIP_CORE();
    struct netif *netif = netif_get_by_index((uint8_t)sIndex);
    if (netif) {
        if (!ip4_addr_isany_val(*netif_ip4_addr(netif))) {
            ip_addr_t v4;
            ip_addr_copy_from_ip4(v4, *netif_ip4_addr(netif));
            to_ot_address(&addresses[count++], &v4);
        }
        for (unsigned i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
            if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)))
                memcpy(addresses[count++].mFields.m8, netif_ip6_addr(netif, i)->addr, 16);
        }
    }
    UNLOCK_TCPIP_CORE();
    if (!sAddressesReported || count != sAddressCount ||
        memcmp(addresses, sAddresses, count * sizeof(*addresses)) != 0) {
        otPlatMdnsHandleHostAddressRemoveAll(instance, sIndex);
        for (unsigned i = 0; i < count; ++i)
            otPlatMdnsHandleHostAddressEvent(instance, &addresses[i], true, sIndex);
        memcpy(sAddresses, addresses, count * sizeof(*addresses));
        sAddressCount = count;
        sAddressesReported = true;
    }
}

void br_mdns_drop_queued(void)
{
    struct mdns_rx *event;
    while (sQueue && xQueueReceive(sQueue, &event, 0) == pdTRUE) vPortFree(event);
}

void br_mdns_process(otInstance *instance, uint32_t index, bool running)
{
    bool enabled = otMdnsIsEnabled(instance);
    bool wanted = running && index != 0;
    /* This vendored core sets its enabled flag before invoking the platform.
     * Recover from socket/allocation failure (including automatic enable). */
    if (enabled && !sPcb) {
#ifdef CONFIG_THREAD_BR_LAN_MANAGEMENT
        br_lan_publish(instance, false);
#endif
        otMdnsSetEnabled(instance, false, index);
        enabled = false;
    }
    if (enabled != wanted) {
        otError error = otMdnsSetEnabled(instance, wanted, index);
        if (error == OT_ERROR_NONE)
            IPRO_LOGI("BR-mDNS", "Listening=%u ifindex=%u", wanted, (unsigned)index);
        else IPRO_LOGW("BR-mDNS", "State change failed: %d", error);
    }
    if (sPcb) report_addresses(instance);
#ifdef CONFIG_THREAD_BR_LAN_MANAGEMENT
    br_lan_publish(instance, wanted && sPcb);
#endif
    struct mdns_rx *event;
    for (unsigned i = 0; sQueue && i < RX_DEPTH &&
         xQueueReceive(sQueue, &event, 0) == pdTRUE; ++i) {
        if (sPcb && event->generation == sGeneration && event->source.mInfraIfIndex == sIndex) {
            otMessage *message = otIp6NewMessage(instance, NULL);
            if (message) {
                if (otMessageAppend(message, event->data, event->length) == OT_ERROR_NONE)
                    otPlatMdnsHandleReceive(instance, message, event->unicast, &event->source);
                else otMessageFree(message);
            }
        }
        vPortFree(event);
    }
}

#ifdef CONFIG_SHELL
static void cmd_mdns_stats(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    uint32_t values[8];
    LOCK_TCPIP_CORE();
    values[0] = sStats.rx4; values[1] = sStats.rx6;
    values[2] = sStats.tx4; values[3] = sStats.tx6;
    values[4] = sStats.loop4; values[5] = sStats.loop6;
    values[6] = sStats.drops; values[7] = sStats.tx_errors;
    UNLOCK_TCPIP_CORE();
    IPRO_LOGI("BR-mDNS", "rx4=%lu rx6=%lu tx4=%lu tx6=%lu loop4=%lu loop6=%lu drops=%lu tx_errors=%lu",
              (unsigned long)values[0], (unsigned long)values[1], (unsigned long)values[2],
              (unsigned long)values[3], (unsigned long)values[4], (unsigned long)values[5],
              (unsigned long)values[6], (unsigned long)values[7]);
}
SHELL_CMD_EXPORT_ALIAS(cmd_mdns_stats, br_mdns_stats, mDNS transport counters);
#endif
