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

#include "br_dns_upstream.h"
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64 && \
    (!defined(CONFIG_THREAD_BR_UPSTREAM_DNS) || !CONFIG_THREAD_BR_UPSTREAM_DNS)
#error "BR DNS64 requires the upstream DNS transport"
#endif
#if defined(CONFIG_THREAD_BR_UPSTREAM_DNS) && CONFIG_THREAD_BR_UPSTREAM_DNS
#include <string.h>
#include <openthread/platform/dns.h>
#include <openthread/dnssd_server.h>
#include <openthread/random_crypto.h>
#include <openthread/udp.h>
#if defined(CONFIG_THREAD_BR_NAT64) && CONFIG_THREAD_BR_NAT64
#include <openthread/platform/infra_if.h>
#include <openthread/nat64.h>
#endif
#include "lwip/dns.h"
#include "lwip/sys.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/ip.h"
#include "lwip/tcpip.h"
#include "fhost.h"
#include "wifi_mgmr_ext.h"
#include "openthread_port.h"
#include "br_wifi_link_epoch.h"
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
#include "br_dns64.h"
#include <openthread/nat64.h>
#include <openthread/netdata.h>
#endif

#if !LWIP_DNS || !LWIP_UDP || !LWIP_TCP
#error "BR upstream DNS requires lwIP DNS server configuration, UDP and TCP"
#endif
#define DNS_SLOTS 4
#if defined(CONFIG_THREAD_BR_NAT64) && CONFIG_THREAD_BR_NAT64
#define DNS_TOTAL_SLOTS (DNS_SLOTS + 1)
#else
#define DNS_TOTAL_SLOTS DNS_SLOTS
#endif
#define DNS_MAX_PACKET 1232
#define DNS_RETRY_MS 3000U
#define DNS_PORT_ATTEMPTS 32
#define DNS64_PHASE_MS 2000U

struct dns_slot {
    otPlatDnsUpstreamQuery *txn;
    struct udp_pcb *pcb;
    struct tcp_pcb *tcp;
    struct pbuf *tcp_response;
    uint16_t tcp_length, tcp_received, tcp_tx_offset;
    uint8_t tcp_prefix[2], tcp_prefix_length;
    bool want_tcp, tcp_connected, failed, discovery;
    struct pbuf *query, *response;
    struct netif *netif;
    ip_addr_t server, local;
    ip4_addr_t address, mask, gateway;
    uint32_t epoch, sent_at;
    uint16_t original_id, port_seed;
    uint8_t server_index, netif_index;
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
    bool dns64_a, dns64_candidate, dns64_ptr;
    uint8_t dns64_ptr_prefix_length;
    struct pbuf *dns64_original;
    struct pbuf *dns64_alias_original, *dns64_alias_response;
    uint32_t dns64_alias_at;
    uint8_t dns64_alias_hops;
    uint8_t dns64_prefix[32];
    uint32_t dns64_ttl, dns64_at, dns64_started_at;
    uint32_t dns64_policy_epoch;
#endif
};
/* PCB, pbuf and slot state are protected by the core lock. Platform entry
 * points and completion run on the OT owner. RX never calls an OT instance API. */
static struct dns_slot sSlots[DNS_TOTAL_SLOTS];
static otInstance *sOwner;
static bool sReady;
#if defined(CONFIG_THREAD_BR_NAT64) && CONFIG_THREAD_BR_NAT64
/* The cache and its notifications belong to the OT owner. A dedicated slot
 * bounds discovery without displacing client transactions. No DNS64 rewrite
 * or local synthesis is allowed for this infrastructure-only query. */
static struct {
    struct dns_slot route;
    struct br_dns64_prefix prefixes[BR_PREF64_MAX_PREFIXES];
    size_t count;
    uint32_t index, at, lifetime, refresh_at;
    bool cached, notify;
} sDiscovery;
#endif
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
static struct br_dns64_prefix sExclusions[BR_DNS64_MAX_EXCLUSIONS];
static size_t sExclusionCount;
static uint32_t sExclusionEpoch = 1;
#endif

static uint16_t get16(const struct pbuf *p, uint16_t offset)
{
    return (uint16_t)((pbuf_get_at(p, offset) << 8) | pbuf_get_at(p, offset + 1));
}

static bool read_name(const struct pbuf *p, uint16_t *offset, uint8_t name[255], unsigned *size)
{
    uint16_t pos = *offset;
    unsigned used = 0, steps = 0;
    bool jumped = false;
    while (pos < p->tot_len && ++steps <= p->tot_len) {
        uint8_t length = pbuf_get_at(p, pos++);
        if ((length & 0xc0) == 0xc0) {
            if (pos >= p->tot_len) return false;
            uint16_t target = (uint16_t)(((length & 0x3f) << 8) | pbuf_get_at(p, pos++));
            if (target < 12 || target >= pos - 2) return false;
            if (!jumped) *offset = pos;
            jumped = true; pos = target; continue;
        }
        if (length > 63 || used + 1U + length > 255 || pos + length > p->tot_len) return false;
        name[used++] = length;
        if (!length) {
            if (!jumped) *offset = pos;
            *size = used; return true;
        }
        for (unsigned i = 0; i < length; ++i) {
            uint8_t c = pbuf_get_at(p, pos++);
            name[used++] = c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
        }
    }
    return false;
}

static bool questions_match(const struct pbuf *query, const struct pbuf *reply)
{
    unsigned count = get16(query, 4);
    if (!count || count > 8 || count != get16(reply, 4)) return false;
    uint16_t qpos = 12, rpos = 12;
    for (unsigned i = 0; i < count; ++i) {
        uint8_t qname[255], rname[255];
        unsigned qsize, rsize;
        if (!read_name(query, &qpos, qname, &qsize) || !read_name(reply, &rpos, rname, &rsize) ||
            qsize != rsize || memcmp(qname, rname, qsize) ||
            qpos + 4U > query->tot_len || rpos + 4U > reply->tot_len ||
            get16(query, qpos) != get16(reply, rpos) ||
            get16(query, qpos + 2) != get16(reply, rpos + 2)) return false;
        qpos += 4; rpos += 4;
    }
    return true;
}

static bool select_server(struct dns_slot *slot, unsigned index)
{
    struct netif *netif = fhost_to_net_if(MGMR_VIF_STA);
    uint32_t epoch = br_wifi_link_epoch();
    if (!sReady || !netif || !epoch || !netif_is_up(netif) || !netif_is_link_up(netif)) return false;
    ip_addr_t server = *dns_getserver(index), local;
    if (ip_addr_isany(&server) || ip_addr_ismulticast(&server) || ip_addr_isloopback(&server)) return false;
    if (IP_IS_V4(&server)) {
        if (ip4_addr_isany_val(*netif_ip4_addr(netif)) ||
            ip4_addr_isbroadcast(ip_2_ip4(&server), netif)) return false;
        ip_addr_copy_from_ip4(local, *netif_ip4_addr(netif));
    } else {
        ip6_addr_assign_zone(ip_2_ip6(&server), IP6_UNICAST, netif);
        const ip_addr_t *source = ip6_select_source_address(netif, ip_2_ip6(&server));
        if (!source) return false;
        local = *source;
    }
    if (ip_route(&local, &server) != netif || ip_addr_cmp(&local, &server)) return false;
    slot->netif = netif; slot->netif_index = netif_get_index(netif); slot->epoch = epoch;
    slot->server = server; slot->local = local; slot->server_index = (uint8_t)index;
    slot->address = *netif_ip4_addr(netif); slot->mask = *netif_ip4_netmask(netif);
    slot->gateway = *netif_ip4_gw(netif);
    return true;
}

static bool live(const struct dns_slot *slot)
{
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
    if (slot->dns64_candidate && slot->dns64_policy_epoch != sExclusionEpoch) return false;
#endif
    struct dns_slot current = {0};
    return select_server(&current, slot->server_index) && current.netif == slot->netif &&
           current.netif_index == slot->netif_index && current.epoch == slot->epoch &&
           ip_addr_cmp(&current.server, &slot->server) && ip_addr_cmp(&current.local, &slot->local) &&
           ip4_addr_cmp(&current.address, &slot->address) && ip4_addr_cmp(&current.mask, &slot->mask) &&
           ip4_addr_cmp(&current.gateway, &slot->gateway);
}

static bool reply_matches(const struct dns_slot *slot, const struct pbuf *p)
{
    return p->tot_len >= 12 && p->tot_len <= DNS_MAX_PACKET &&
           get16(p, 0) == get16(slot->query, 0) && (pbuf_get_at(p, 2) & 0x80) &&
           (pbuf_get_at(p, 2) & 0x78) == (pbuf_get_at(slot->query, 2) & 0x78) &&
           questions_match(slot->query, p);
}

static void close_transport(struct dns_slot *slot)
{
    if (slot->pcb) { udp_remove(slot->pcb); slot->pcb = NULL; }
    if (slot->tcp) {
        /* A slot owns one connection, including cancellation and partial RX.
         * Do not leave detached FIN-WAIT/TIME-WAIT PCBs outside the slot budget. */
        struct tcp_pcb *tcp = slot->tcp;
        slot->tcp = NULL;
        tcp_arg(tcp, NULL); tcp_err(tcp, NULL); tcp_recv(tcp, NULL); tcp_sent(tcp, NULL);
        tcp_abort(tcp);
    }
    if (slot->tcp_response) { pbuf_free(slot->tcp_response); slot->tcp_response = NULL; }
    slot->want_tcp = slot->tcp_connected = slot->failed = false;
    slot->tcp_prefix_length = 0; slot->tcp_length = slot->tcp_received = slot->tcp_tx_offset = 0;
}

static void tcp_failed(void *context, err_t error)
{
    struct dns_slot *slot = context;
    (void)error;
    slot->tcp = NULL; /* lwIP already freed this PCB. */
    slot->failed = true;
    otSysEventSignalPending();
}

static void write_tcp_query(struct dns_slot *slot)
{
    if (!slot->tcp_connected || slot->failed || !live(slot)) return;
    uint16_t total = slot->query->tot_len + 2U;
    err_t error = ERR_OK;
    while (slot->tcp_tx_offset < total && tcp_sndbuf(slot->tcp)) {
        uint8_t data[256];
        uint16_t length = LWIP_MIN(sizeof(data), total - slot->tcp_tx_offset);
        length = LWIP_MIN(length, tcp_sndbuf(slot->tcp));
        uint16_t prefix = 0;
        if (slot->tcp_tx_offset == 0) data[prefix++] = slot->query->tot_len >> 8;
        if (slot->tcp_tx_offset + prefix == 1 && prefix < length)
            data[prefix++] = slot->query->tot_len & 255;
        if (length > prefix && pbuf_copy_partial(slot->query, data + prefix, length - prefix,
                slot->tcp_tx_offset + prefix - 2U) != length - prefix) {
            slot->failed = true; return;
        }
        error = tcp_write(slot->tcp, data, length, TCP_WRITE_FLAG_COPY);
        if (error != ERR_OK) break;
        slot->tcp_tx_offset += length;
    }
    if (error != ERR_OK && error != ERR_MEM) slot->failed = true;
    // Also retry output after a transient allocation failure, even when all
    // bytes have already been copied into lwIP's send queue.
    error = tcp_output(slot->tcp);
    if (error != ERR_OK && error != ERR_MEM) slot->failed = true;
}

static err_t tcp_acked(void *context, struct tcp_pcb *tcp, u16_t length)
{
    struct dns_slot *slot = context;
    (void)length;
    if (slot->tcp == tcp) write_tcp_query(slot);
    return ERR_OK;
}

static err_t tcp_connected(void *context, struct tcp_pcb *tcp, err_t error)
{
    struct dns_slot *slot = context;
    if (slot->tcp != tcp || error != ERR_OK || !live(slot)) slot->failed = true;
    else { slot->tcp_connected = true; write_tcp_query(slot); }
    otSysEventSignalPending();
    return ERR_OK;
}

static err_t tcp_received(void *context, struct tcp_pcb *tcp, struct pbuf *p, err_t error)
{
    struct dns_slot *slot = context;
    if (!p) { slot->failed = !slot->response; otSysEventSignalPending(); return ERR_OK; }
    if (slot->tcp != tcp || error != ERR_OK || !live(slot) || slot->failed) {
        slot->failed = true; pbuf_free(p); otSysEventSignalPending(); return ERR_OK;
    }
    uint16_t offset = 0;
    while (slot->tcp_prefix_length < 2 && offset < p->tot_len)
        slot->tcp_prefix[slot->tcp_prefix_length++] = pbuf_get_at(p, offset++);
    if (slot->tcp_prefix_length == 2 && !slot->tcp_response && !slot->response) {
        slot->tcp_length = (uint16_t)((slot->tcp_prefix[0] << 8) | slot->tcp_prefix[1]);
        if (slot->tcp_length < 12 || slot->tcp_length > DNS_MAX_PACKET ||
            !(slot->tcp_response = pbuf_alloc(PBUF_RAW, slot->tcp_length, PBUF_RAM))) slot->failed = true;
    }
    /* One query per connection; ignore any frame after the retained response. */
    if (!slot->response && !slot->failed && slot->tcp_response) {
        uint16_t length = p->tot_len - offset;
        if (length > slot->tcp_length - slot->tcp_received ||
            pbuf_copy_partial(p, (uint8_t *)slot->tcp_response->payload + slot->tcp_received, length, offset) != length)
            slot->failed = true;
        else slot->tcp_received += length;
        if (!slot->failed && slot->tcp_received == slot->tcp_length) {
            if (!reply_matches(slot, slot->tcp_response) || (pbuf_get_at(slot->tcp_response, 2) & 2))
                slot->failed = true;
            else { slot->response = slot->tcp_response; slot->tcp_response = NULL; }
        }
    }
    tcp_recved(tcp, p->tot_len);
    pbuf_free(p);
    if (slot->response || slot->failed) otSysEventSignalPending();
    return ERR_OK;
}

static bool start_tcp(struct dns_slot *slot)
{
    if (slot->pcb) { udp_remove(slot->pcb); slot->pcb = NULL; }
    slot->tcp = tcp_new_ip_type(IP_GET_TYPE(&slot->server));
    if (!slot->tcp) return false;
    tcp_arg(slot->tcp, slot); tcp_err(slot->tcp, tcp_failed); tcp_recv(slot->tcp, tcp_received);
    tcp_sent(slot->tcp, tcp_acked);
    err_t error = ERR_USE;
    for (unsigned attempt = 0; attempt < DNS_PORT_ATTEMPTS && error == ERR_USE; ++attempt)
        error = tcp_bind(slot->tcp, &slot->local, 49152U + ((slot->port_seed + slot->server_index + attempt) & 0x3fff));
    if (error == ERR_OK) {
        tcp_bind_netif(slot->tcp, slot->netif);
        error = tcp_connect(slot->tcp, &slot->server, 53, tcp_connected);
    }
    if (error != ERR_OK) { close_transport(slot); return false; }
    slot->sent_at = sys_now();
    return true;
}

static void receive(void *context, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *source, u16_t port)
{
    struct dns_slot *slot = context;
    if (!p) return;
    if ((!slot->txn && !slot->discovery) || slot->pcb != pcb || slot->response || slot->want_tcp || !live(slot) ||
        ip_current_input_netif() != slot->netif || !ip_addr_cmp(source, &slot->server) || port != 53 ||
        !reply_matches(slot, p)) {
        pbuf_free(p); return;
    }
    if (pbuf_get_at(p, 2) & 2) { slot->want_tcp = true; pbuf_free(p); }
    else slot->response = p;
    otSysEventSignalPending();
}

static bool transmit(struct dns_slot *slot, unsigned first_server)
{
    close_transport(slot);
    for (unsigned i = first_server; i < DNS_MAX_SERVERS; ++i) {
        if (!select_server(slot, i)) continue;
        slot->pcb = udp_new_ip_type(IP_GET_TYPE(&slot->server));
        if (!slot->pcb) return false;
        err_t error = ERR_USE;
        for (unsigned attempt = 0; attempt < DNS_PORT_ATTEMPTS && error == ERR_USE; ++attempt)
            error = udp_bind(slot->pcb, &slot->local, 49152U + ((slot->port_seed + i + attempt) & 0x3fff));
        if (error == ERR_OK) {
            udp_bind_netif(slot->pcb, slot->netif);
            udp_recv(slot->pcb, receive, slot);
            /* lwIP prepends headers to the supplied pbuf and leaves them
             * there. Retain an immutable DNS-only query for retries/RX checks. */
            struct pbuf *out = pbuf_clone(PBUF_TRANSPORT, PBUF_RAM, slot->query);
            error = out ? udp_sendto_if(slot->pcb, out, &slot->server, 53, slot->netif) : ERR_MEM;
            if (out) pbuf_free(out);
        }
        if (error == ERR_OK) { slot->sent_at = sys_now(); return true; }
        udp_remove(slot->pcb); slot->pcb = NULL;
    }
    return false;
}

/* Remove all shared references before OT completion can synchronously reuse a
 * transaction/slot. The returned response remains privately owned by the caller. */
static struct pbuf *detach(struct dns_slot *slot)
{
    struct pbuf *response = slot->response;
    close_transport(slot);
    if (slot->query) pbuf_free(slot->query);
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
    if (slot->dns64_original) pbuf_free(slot->dns64_original);
    if (slot->dns64_alias_original) pbuf_free(slot->dns64_alias_original);
    if (slot->dns64_alias_response) pbuf_free(slot->dns64_alias_response);
#endif
    memset(slot, 0, sizeof(*slot));
    return response;
}

#if defined(CONFIG_THREAD_BR_NAT64) && CONFIG_THREAD_BR_NAT64
otError otPlatInfraIfDiscoverNat64Prefix(uint32_t index)
{
    static const uint8_t query[] = {0,0,1,0,0,1,0,0,0,0,0,0,
        8,'i','p','v','4','o','n','l','y',4,'a','r','p','a',0,0,28,0,1};
    uint8_t entropy[4];
    if (!sOwner || !index) return OT_ERROR_FAILED;
    LOCK_TCPIP_CORE();
    struct netif *netif = fhost_to_net_if(MGMR_VIF_STA);
    bool valid = sReady && netif && netif_get_index(netif) == index;
    struct dns_slot *slot = &sSlots[DNS_SLOTS];
    bool busy = slot->discovery;
    bool cached = valid && sDiscovery.cached && sDiscovery.index == index && live(&sDiscovery.route) &&
        (uint32_t)(sys_now() - sDiscovery.at) < sDiscovery.refresh_at;
    UNLOCK_TCPIP_CORE();
    if (!valid) return OT_ERROR_FAILED;
    if (cached) { sDiscovery.notify = true; otSysEventSignalPending(); return OT_ERROR_NONE; }
    if (busy) return OT_ERROR_NONE;
    if (otRandomCryptoFillBuffer(entropy, sizeof(entropy)) != OT_ERROR_NONE) return OT_ERROR_FAILED;
    bool started = false;
    LOCK_TCPIP_CORE();
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(query), PBUF_RAM);
    if (p) {
        memcpy(p->payload, query, sizeof(query));
        memcpy(p->payload, entropy, 2);
        slot->query = p; slot->discovery = true;
        slot->port_seed = (uint16_t)((entropy[2] << 8) | entropy[3]);
        started = transmit(slot, 0);
        if (!started) (void)detach(slot);
    }
    UNLOCK_TCPIP_CORE();
    if (started) sDiscovery.index = index;
    return started ? OT_ERROR_NONE : OT_ERROR_FAILED;
}

static void discovery_complete(otInstance *instance, const struct dns_slot *completed,
                               struct pbuf *response, bool usable)
{
    uint8_t wire[DNS_MAX_PACKET]; uint32_t ttl = 0; size_t count = 0;
    struct br_dns64_prefix prefixes[BR_PREF64_MAX_PREFIXES];
    LOCK_TCPIP_CORE();
    bool valid = response && usable && live(completed) &&
        pbuf_copy_partial(response, wire, response->tot_len, 0) == response->tot_len;
    uint16_t length = response ? response->tot_len : 0;
    if (response) pbuf_free(response);
    UNLOCK_TCPIP_CORE();
    valid = valid && br_dns64_discover(wire, length, prefixes, &count, &ttl);
    LOCK_TCPIP_CORE();
    valid = valid && live(completed);
    if (valid) {
        memset(&sDiscovery.route, 0, sizeof(sDiscovery.route));
        valid = select_server(&sDiscovery.route, completed->server_index);
    }
    uint32_t now = sys_now();
    UNLOCK_TCPIP_CORE();
    if (valid && br_wifi_link_epoch() == completed->epoch) {
        memcpy(sDiscovery.prefixes, prefixes, count * sizeof(*prefixes));
        sDiscovery.count = ttl ? count : 0;
        sDiscovery.at = now;
        /* Bound unsigned elapsed time and retry zero-TTL replies no more
         * often than once per second. A zero-TTL prefix is never published. */
        sDiscovery.lifetime = LWIP_MIN(ttl, 86400U) * 1000U;
        if (!sDiscovery.lifetime) sDiscovery.lifetime = 1000;
        sDiscovery.refresh_at = sDiscovery.lifetime > 10000 ? sDiscovery.lifetime - 10000 : sDiscovery.lifetime;
        /* Negative answers must retain their full SOA-derived cache time. */
        if (!count) sDiscovery.refresh_at = sDiscovery.lifetime;
        sDiscovery.cached = true; sDiscovery.notify = true;
    } else if (sDiscovery.cached) {
        /* Keep a still-valid prior answer, without retrying on every poll. */
        sDiscovery.refresh_at = (uint32_t)(now - sDiscovery.at) + 60000U;
    } else {
        otIp6Prefix empty = {0};
        otPlatInfraIfDiscoverNat64PrefixDone(instance, sDiscovery.index, &empty);
    }
}

static void discovery_process(otInstance *instance)
{
    bool refresh = false;
    LOCK_TCPIP_CORE();
    if (sDiscovery.cached) {
        uint32_t age = sys_now() - sDiscovery.at;
        bool current = live(&sDiscovery.route);
        refresh = current && age >= sDiscovery.refresh_at && !sSlots[DNS_SLOTS].discovery;
        if (!current || age >= sDiscovery.lifetime) {
            sDiscovery.cached = false; sDiscovery.count = 0; sDiscovery.notify = true;
        }
    }
    UNLOCK_TCPIP_CORE();
    if (sDiscovery.notify) {
        otIp6Prefix prefix = {0};
        if (sDiscovery.count) {
            memcpy(prefix.mPrefix.mFields.m8, sDiscovery.prefixes[0].address, 16);
            prefix.mLength = sDiscovery.prefixes[0].length;
        }
        sDiscovery.notify = false;
        /* The pinned OT platform API accepts one prefix. Preserve DNS wire
         * order when selecting it; the codec still examines every AAAA. */
        otPlatInfraIfDiscoverNat64PrefixDone(instance, sDiscovery.index, &prefix);
    }
    if (refresh && otPlatInfraIfDiscoverNat64Prefix(sDiscovery.index) != OT_ERROR_NONE && sDiscovery.cached)
        sDiscovery.refresh_at = (uint32_t)(sys_now() - sDiscovery.at) + 60000U;
}
#endif

#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
bool br_dns_upstream_set_dns64_exclusions(otInstance *instance,
    const struct br_dns64_prefix *prefixes, size_t count)
{
    if (!instance || instance != sOwner || count > BR_DNS64_MAX_EXCLUSIONS || (count && !prefixes)) return false;
    for (size_t i = 0; i < count; ++i) if (prefixes[i].length > 128) return false;
    LOCK_TCPIP_CORE();
    if (count) memcpy(sExclusions, prefixes, count * sizeof(*prefixes));
    sExclusionCount = count;
    if (!++sExclusionEpoch) ++sExclusionEpoch;
    UNLOCK_TCPIP_CORE();
    return true;
}

static uint16_t dns64_question_end(const struct pbuf *query)
{
    uint8_t name[255]; unsigned length; uint16_t pos = 12;
    if (get16(query, 4) != 1 || !read_name(query, &pos, name, &length) ||
        pos + 4U > query->tot_len || get16(query, pos) != 28 ||
        get16(query, pos + 2) != 1 || (pbuf_get_at(query, 2) & 0xf8)) return 0;
    return pos + 4U;
}

static bool dns64_embed(void *context, const uint8_t ip4[4], uint8_t ip6[16])
{
    otIp4Address v4; otIp6Address v6;
    memcpy(v4.mFields.m8, ip4, 4);
    if (otNat64SynthesizeIp6Address(context, &v4, &v6) != OT_ERROR_NONE) return false;
    memcpy(ip6, v6.mFields.m8, 16); return true;
}

/* Fingerprint the actual preferred Network Data prefix through OT's public
 * RFC 6052 implementation. Zero/all-one IPv4 probes identify both the prefix
 * bits and IPv4 bit positions, including all supported non-/96 lengths. This
 * avoids assuming that this BR's local prefix won the network election. */
static bool dns64_prefix(otInstance *instance, uint8_t prefix[32])
{
    const uint8_t zero[4] = {0}, ones[4] = {255,255,255,255};
    return dns64_embed(instance, zero, prefix) && dns64_embed(instance, ones, prefix + 16);
}

/* PTR uses every current site NAT64 route, not only the preferred synthesis
 * route. Longest prefix wins for overlapping routes, matching IP routing.
 * The RFC 6052 well-known prefix is also recognized independently of routes. */
static bool dns64_ptr_extract(otInstance *instance, const uint8_t address[16],
    struct br_dns64_prefix *prefix, uint8_t ip4[4])
{
    static const uint8_t well_known[12] = {0,0x64,0xff,0x9b};
    memset(prefix, 0, sizeof(*prefix));
    if (!memcmp(address, well_known, sizeof(well_known))) {
        memcpy(prefix->address, well_known, sizeof(well_known)); prefix->length = 96;
    }
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otExternalRouteConfig route;
    while (otNetDataGetNextRoute(instance, &iterator, &route) == OT_ERROR_NONE) {
        uint8_t length = route.mPrefix.mLength;
        if (!route.mNat64 || (length != 32 && length != 40 && length != 48 && length != 56 && length != 64 && length != 96) ||
            length <= prefix->length || memcmp(address, route.mPrefix.mPrefix.mFields.m8, length / 8)) continue;
        memset(prefix->address, 0, sizeof(prefix->address));
        memcpy(prefix->address, route.mPrefix.mPrefix.mFields.m8, length / 8); prefix->length = length;
    }
    if (!prefix->length) return false;
    otIp6Address v6; otIp4Address v4;
    memcpy(v6.mFields.m8, address, 16);
    otIp4ExtractFromIp6Address(prefix->length, &v6, &v4);
    memcpy(ip4, v4.mFields.m8, 4); return true;
}

static bool dns64_ptr_prefix_live(otInstance *instance, const struct dns_slot *slot)
{
    struct br_dns64_prefix prefix; uint8_t ip4[4];
    return dns64_ptr_extract(instance, slot->dns64_prefix + 16, &prefix, ip4) &&
           prefix.length == slot->dns64_ptr_prefix_length && !memcmp(prefix.address, slot->dns64_prefix, 16);
}

/* Before first transmission, replace a recognized site reverse query. A
 * recognized but unsupported query fails closed instead of leaking IP6.ARPA.
 * The original pbuf is retained solely for the final question/CNAME assembly. */
static bool dns64_start_ptr(otInstance *instance, struct dns_slot *slot)
{
    uint8_t address[16], ip4[4]; struct br_dns64_prefix prefix;
    unsigned questions = get16(slot->query, 4);
    UNLOCK_TCPIP_CORE();
    bool match = false;
    for (unsigned i = 0; i < questions && !match; ++i)
        match = br_dns64_ptr_question_address(slot->query->payload, slot->query->tot_len, i, address) &&
                dns64_ptr_extract(instance, address, &prefix, ip4);
    LOCK_TCPIP_CORE();
    if (!match) return true;
    if (questions != 1) return false;
    struct pbuf *query = pbuf_alloc(PBUF_TRANSPORT, DNS_MAX_PACKET, PBUF_RAM);
    if (!query) return false;
    size_t length = 0;
    UNLOCK_TCPIP_CORE();
    bool prepared = br_dns64_ptr_query(slot->query->payload, slot->query->tot_len, ip4,
                                      query->payload, query->tot_len, &length);
    LOCK_TCPIP_CORE();
    if (!prepared) { pbuf_free(query); return false; }
    pbuf_realloc(query, (uint16_t)length);
    slot->dns64_original = slot->query; slot->query = query;
    slot->dns64_ptr = slot->dns64_candidate = true;
    slot->dns64_ptr_prefix_length = prefix.length;
    memcpy(slot->dns64_prefix, prefix.address, 16); memcpy(slot->dns64_prefix + 16, address, 16);
    return true;
}

/* Called with the core lock held, returning with it held. Slot mutation and
 * pbuf allocation/free use the lock. All OT prefix/entropy APIs and wire work
 * run on the owner outside it; RX sees the retained response and cannot replace
 * it. No API here dispatches tasklets/completions or permits slot reuse. */
static int dns64_alias_step(otInstance *instance, struct dns_slot *slot)
{
    if (!slot->dns64_candidate) return 0;
    if (slot->dns64_a || slot->dns64_ptr) {
        uint8_t prefix[32];
        UNLOCK_TCPIP_CORE();
        bool valid = slot->dns64_ptr ? dns64_ptr_prefix_live(instance, slot) :
            (dns64_prefix(instance, prefix) && !memcmp(prefix, slot->dns64_prefix, sizeof(prefix)));
        LOCK_TCPIP_CORE();
        if (!valid || !live(slot)) return -1;
    }
    /* At most eight continuation queries across both DNS64 phases. All
     * transport changes remain inside the original phase/total deadlines. */
    bool normalized = false;
    for (;;) {
        bool merge = slot->dns64_alias_original != NULL;
        struct pbuf *input = pbuf_clone(PBUF_RAW, PBUF_RAM, slot->response);
        struct pbuf *out = pbuf_alloc(PBUF_TRANSPORT, DNS_MAX_PACKET, PBUF_RAM);
        if (!input || !out) {
            if (input) pbuf_free(input);
            if (out) pbuf_free(out);
            return -1;
        }
        size_t length = 0; int result; bool terminal_response = false; uint8_t entropy[4] = {0};
        uint32_t elapsed = (uint32_t)(sys_now() - slot->dns64_alias_at);
        UNLOCK_TCPIP_CORE();
        if (!normalized) {
            result = br_dns64_dname_expand(slot->query->payload, slot->query->tot_len,
                input->payload, input->tot_len, out->payload, out->tot_len, &length);
            normalized = true;
            if (result) {
                LOCK_TCPIP_CORE(); pbuf_free(input);
                if (result < 0 || !live(slot)) { pbuf_free(out); return -1; }
                pbuf_free(slot->response); pbuf_realloc(out, (uint16_t)length); slot->response = out;
                continue;
            }
        }
        if (merge) {
            /* Classify this reply before joining the old chain. An empty
             * terminal NODATA reply without SOA must not requery the same
             * target merely because the merged packet retains old CNAMEs. */
            result = br_dns64_alias_query(slot->query->payload, slot->query->tot_len,
                input->payload, input->tot_len, out->payload, out->tot_len, &length);
            terminal_response = result == 0;
            if (result >= 0) result = br_dns64_alias_merge(slot->dns64_alias_original->payload, slot->dns64_alias_original->tot_len,
                slot->dns64_alias_response->payload, slot->dns64_alias_response->tot_len,
                slot->query->payload, slot->query->tot_len, input->payload, input->tot_len,
                elapsed / 1000U + !!(elapsed % 1000U), out->payload, out->tot_len, &length) ? 0 : -1;
        } else {
            result = br_dns64_alias_query(slot->query->payload, slot->query->tot_len,
                input->payload, input->tot_len, out->payload, out->tot_len, &length);
            if (result == 1 && otRandomCryptoFillBuffer(entropy, sizeof(entropy)) != OT_ERROR_NONE) result = -1;
        }
        LOCK_TCPIP_CORE();
        pbuf_free(input);
        if (!live(slot)) result = -1;
        if (result < 0 || (!merge && !result)) { pbuf_free(out); return result; }
        pbuf_realloc(out, (uint16_t)length);
        if (merge) {
            pbuf_free(slot->response); slot->response = out;
            pbuf_free(slot->query); slot->query = slot->dns64_alias_original;
            pbuf_free(slot->dns64_alias_response);
            slot->dns64_alias_original = slot->dns64_alias_response = NULL;
            if (terminal_response) return 0;
            continue;
        }
        if (slot->dns64_alias_hops >= 8) { pbuf_free(out); return -1; }
        uint32_t now = sys_now();
        if ((uint32_t)(now - slot->dns64_started_at) >= 2U * DNS64_PHASE_MS ||
            (slot->dns64_a && (uint32_t)(now - slot->dns64_at) >= DNS64_PHASE_MS)) {
            pbuf_free(out); return -1;
        }
        if (!slot->dns64_a && !slot->dns64_ptr &&
            (uint32_t)(now - slot->dns64_started_at) >= DNS64_PHASE_MS) {
            pbuf_free(out); return 0; /* Let the original AAAA phase try A. */
        }
        struct pbuf *retained = pbuf_clone(PBUF_RAW, PBUF_RAM, slot->response);
        if (!retained) { pbuf_free(out); return -1; }
        close_transport(slot);
        slot->dns64_alias_original = slot->query;
        slot->dns64_alias_response = retained;
        pbuf_free(slot->response); slot->response = NULL;
        slot->query = out; slot->dns64_alias_at = sys_now(); ++slot->dns64_alias_hops;
        ((uint8_t *)out->payload)[0] = entropy[0]; ((uint8_t *)out->payload)[1] = entropy[1];
        slot->port_seed = (uint16_t)((entropy[2] << 8) | entropy[3]);
        return transmit(slot, slot->server_index) ? 1 : -1;
    }
}

static int dns64_step(otInstance *instance, struct dns_slot *slot, unsigned first_server)
{
    if (!slot->dns64_candidate) return 0;
    uint8_t prefix[32];
    UNLOCK_TCPIP_CORE();
    bool prefix_valid = slot->dns64_ptr ? dns64_ptr_prefix_live(instance, slot) : dns64_prefix(instance, prefix);
    LOCK_TCPIP_CORE();
    if (!live(slot)) return -1;
    if (!prefix_valid) return slot->dns64_a || slot->dns64_ptr ? -1 : 0;
    if (slot->dns64_a && memcmp(prefix, slot->dns64_prefix, sizeof(prefix))) return -1;
    struct pbuf *input = pbuf_clone(PBUF_RAW, PBUF_RAM, slot->response);
    struct pbuf *out = pbuf_alloc(PBUF_TRANSPORT, DNS_MAX_PACKET, PBUF_RAM);
    if (!input || !out) {
        if (input) pbuf_free(input);
        if (out) pbuf_free(out);
        return -1;
    }
    uint32_t elapsed = (uint32_t)(sys_now() - slot->dns64_at) / 1000U;
    uint32_t ttl = slot->dns64_ttl > elapsed ? slot->dns64_ttl - elapsed : 0;
    size_t length = 0;
    int result = 0;
    uint8_t entropy[4] = {0};
    UNLOCK_TCPIP_CORE();
    if (slot->dns64_ptr) {
        if (!br_dns64_ptr_finish(slot->dns64_original->payload, slot->dns64_original->tot_len,
                slot->query->payload, slot->query->tot_len, input->payload, input->tot_len,
                out->payload, out->tot_len, &length)) result = -1;
    } else if (slot->dns64_a) {
        if (!br_dns64_finish(slot->query->payload, slot->query->tot_len, input->payload, input->tot_len,
                ttl, dns64_embed, instance, out->payload, out->tot_len, &length)) result = -1;
    } else {
        result = br_dns64_prepare_with_policy(slot->query->payload, slot->query->tot_len, input->payload, input->tot_len,
                                             sExclusions, sExclusionCount, out->payload, out->tot_len, &length, &ttl);
        if (result == BR_DNS64_QUERY_A && otRandomCryptoFillBuffer(entropy, sizeof(entropy)) != OT_ERROR_NONE)
            result = -1;
    }
    LOCK_TCPIP_CORE();
    pbuf_free(input);
    if (!live(slot)) result = -1;
    if (result == BR_DNS64_QUERY_A) {
        close_transport(slot);
        pbuf_free(slot->query); pbuf_free(slot->response); slot->response = NULL;
        pbuf_realloc(out, (uint16_t)length); slot->query = out; out = NULL;
        ((uint8_t *)slot->query->payload)[0] = entropy[0];
        ((uint8_t *)slot->query->payload)[1] = entropy[1];
        slot->port_seed = (uint16_t)((entropy[2] << 8) | entropy[3]);
        slot->dns64_a = true; slot->dns64_ttl = ttl; slot->dns64_at = sys_now();
        memcpy(slot->dns64_prefix, prefix, sizeof(prefix));
        if (!transmit(slot, first_server)) result = -1;
    } else if ((!result && (slot->dns64_a || slot->dns64_ptr)) || result == BR_DNS64_FILTERED) {
        pbuf_free(slot->response); pbuf_realloc(out, (uint16_t)length);
        slot->response = out; out = NULL;
    }
    if (out) pbuf_free(out);
    return result;
}

static int dns64_timeout(otInstance *instance, struct dns_slot *slot)
{
    /* Feed the same DNS64 policy a local SERVFAIL, without manufacturing any
     * answer/authority/OPT records. The original query still supplies DO/CD.
     * This runs only once, at exhaustion of the initial AAAA phase. */
    if (slot->dns64_alias_original) {
        /* A timeout cannot merge a new A response into an old AAAA partial.
         * Restart A at this phase's original name, which retains the client's
         * question and lets the recursive resolver supply its alias chain. */
        pbuf_free(slot->query); slot->query = slot->dns64_alias_original;
        pbuf_free(slot->dns64_alias_response);
        slot->dns64_alias_original = slot->dns64_alias_response = NULL;
    }
    uint16_t length = dns64_question_end(slot->query);
    if (!length || slot->dns64_a || slot->response) return -1;
    slot->response = pbuf_alloc(PBUF_RAW, length, PBUF_RAM);
    if (!slot->response) return -1;
    if (pbuf_copy_partial(slot->query, slot->response->payload, length, 0) != length) return -1;
    uint8_t *header = slot->response->payload;
    header[2] = 0x80 | (header[2] & 1); /* QR and original RD */
    header[3] = 0x82 | (header[3] & 0x10); /* RA, SERVFAIL, original CD */
    memset(header + 6, 0, 6);
    return dns64_step(instance, slot, 0);
}
#endif

bool otPlatDnsIsUpstreamQueryAvailable(otInstance *instance)
{
    if (!instance || instance != sOwner) return false;
    bool available = false;
    LOCK_TCPIP_CORE();
    struct dns_slot candidate = {0};
    for (unsigned i = 0; i < DNS_MAX_SERVERS && !available; ++i) available = select_server(&candidate, i);
    UNLOCK_TCPIP_CORE();
    return available;
}

void otPlatDnsStartUpstreamQuery(otInstance *instance, otPlatDnsUpstreamQuery *txn, const otMessage *query)
{
    if (!instance || !txn || !query) return;
    LOCK_TCPIP_CORE();
    bool duplicate = false;
    for (unsigned i = 0; i < DNS_SLOTS; ++i) if (sSlots[i].txn == txn) duplicate = true;
    UNLOCK_TCPIP_CORE();
    if (duplicate) return;
    uint16_t length = otMessageGetLength(query);
    uint8_t entropy[4];
    if (instance != sOwner || length < 12 || length > DNS_MAX_PACKET ||
        otRandomCryptoFillBuffer(entropy, sizeof(entropy)) != OT_ERROR_NONE) {
        otPlatDnsUpstreamQueryDone(instance, txn, NULL); return;
    }
    LOCK_TCPIP_CORE();
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);
    UNLOCK_TCPIP_CORE();
    bool copied = p && otMessageRead(query, 0, p->payload, length) == length;
    bool started = false;
    LOCK_TCPIP_CORE();
    if (copied && !(pbuf_get_at(p, 2) & 0x80) && questions_match(p, p)) {
        for (unsigned i = 0; i < DNS_SLOTS; ++i) {
            struct dns_slot *slot = &sSlots[i];
            if (slot->txn) continue;
            slot->txn = txn; slot->query = p; slot->original_id = get16(p, 0);
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
            slot->dns64_candidate = dns64_question_end(p) != 0;
            slot->dns64_started_at = sys_now();
            slot->dns64_policy_epoch = sExclusionEpoch;
#endif
            slot->port_seed = (uint16_t)((entropy[2] << 8) | entropy[3]);
            ((uint8_t *)p->payload)[0] = entropy[0]; ((uint8_t *)p->payload)[1] = entropy[1];
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
            started = dns64_start_ptr(instance, slot) && transmit(slot, 0);
#else
            started = transmit(slot, 0);
#endif
            if (!started) (void)detach(slot);
            p = NULL; break;
        }
    }
    if (p) pbuf_free(p);
    UNLOCK_TCPIP_CORE();
    if (!started) otPlatDnsUpstreamQueryDone(instance, txn, NULL);
}

void otPlatDnsCancelUpstreamQuery(otInstance *instance, otPlatDnsUpstreamQuery *txn)
{
    if (!instance || instance != sOwner || !txn) return;
    bool found = false;
    LOCK_TCPIP_CORE();
    for (unsigned i = 0; i < DNS_SLOTS; ++i) if (sSlots[i].txn == txn) {
        struct pbuf *p = detach(&sSlots[i]);
        if (p) pbuf_free(p);
        found = true; break;
    }
    UNLOCK_TCPIP_CORE();
    /* The pinned core requires synchronous Done(NULL) to release cancellation. */
    if (found) otPlatDnsUpstreamQueryDone(instance, txn, NULL);
}

void br_dns_upstream_process(otInstance *instance, bool forwarding_ready)
{
    if (!instance) return;
    if (!sOwner) sOwner = instance;
    if (instance != sOwner) return;
    LOCK_TCPIP_CORE();
    bool changed = sReady != forwarding_ready;
    sReady = forwarding_ready;
    UNLOCK_TCPIP_CORE();
    if (changed) otDnssdUpstreamQuerySetEnabled(instance, forwarding_ready);
#if defined(CONFIG_THREAD_BR_NAT64) && CONFIG_THREAD_BR_NAT64
    discovery_process(instance);
#endif
    for (unsigned i = 0; i < DNS_TOTAL_SLOTS; ++i) {
        LOCK_TCPIP_CORE();
        struct dns_slot *slot = &sSlots[i];
        bool active = slot->txn || slot->discovery;
        bool usable = active && live(slot);
        bool phase_expired = false;
        uint32_t retry_ms = DNS_RETRY_MS;
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
        if (usable && slot->dns64_candidate) {
            /* Fit both phases inside the pinned OT server's 6000-ms lifetime.
             * Neither DNS failover nor TCP connect resets these deadlines. */
            uint32_t now = sys_now();
            usable = (uint32_t)(now - slot->dns64_started_at) < 2U * DNS64_PHASE_MS;
            uint32_t phase_ms = slot->dns64_ptr ? 2U * DNS64_PHASE_MS : DNS64_PHASE_MS;
            phase_expired = (uint32_t)(now - (slot->dns64_a ? slot->dns64_at : slot->dns64_started_at)) >= phase_ms;
            retry_ms = LWIP_MAX(1U, phase_ms / DNS_MAX_SERVERS);
        }
#endif
        if (usable && !phase_expired && slot->want_tcp && !slot->tcp && !slot->failed)
            slot->failed = !start_tcp(slot);
        if (usable && !phase_expired && slot->tcp) write_tcp_query(slot);
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
        if (usable && slot->response && !slot->discovery) {
            int result = dns64_alias_step(instance, slot);
            if (!result) result = dns64_step(instance, slot, slot->server_index);
            if (result < 0) usable = false;
            else if (result == BR_DNS64_QUERY_A) phase_expired = false;
        }
#endif
        bool finish = active && (!usable || slot->response);
        if (active && !finish && (phase_expired || slot->failed || (uint32_t)(sys_now() - slot->sent_at) >= retry_ms)) {
            finish = phase_expired || !transmit(slot, slot->server_index + 1U);
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
            if (finish && usable && slot->dns64_candidate && !slot->dns64_a && !slot->dns64_ptr) {
                int result = dns64_timeout(instance, slot);
                if (result < 0) usable = false;
                finish = result != BR_DNS64_QUERY_A;
            }
#endif
        }
        struct dns_slot completed = {0};
        struct pbuf *response = NULL;
        if (finish) { completed = *slot; response = detach(slot); }
        UNLOCK_TCPIP_CORE();
#if defined(CONFIG_THREAD_BR_NAT64) && CONFIG_THREAD_BR_NAT64
        if (completed.discovery) {
            discovery_complete(instance, &completed, response, usable);
            discovery_process(instance);
            continue;
        }
#endif
        if (!completed.txn) continue;
        otMessage *message = NULL;
        if (response && usable) {
            message = otUdpNewMessage(instance, NULL);
            uint8_t data[256];
            for (uint16_t offset = 0; message && offset < response->tot_len;) {
                uint16_t length = LWIP_MIN(sizeof(data), response->tot_len - offset);
                if (pbuf_copy_partial(response, data, length, offset) != length) {
                    otMessageFree(message); message = NULL; break;
                }
                if (!offset) { data[0] = completed.original_id >> 8; data[1] = completed.original_id & 255; }
                if (otMessageAppend(message, data, length) != OT_ERROR_NONE) {
                    otMessageFree(message); message = NULL; break;
                }
                offset += length;
            }
        }
        LOCK_TCPIP_CORE();
        if (response) pbuf_free(response);
        bool current = live(&completed);
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
        if (completed.dns64_candidate && (uint32_t)(sys_now() - completed.dns64_started_at) >= 2U * DNS64_PHASE_MS)
            current = false;
#endif
        UNLOCK_TCPIP_CORE();
        if (message && (!current || br_wifi_link_epoch() != completed.epoch)) {
            otMessageFree(message); message = NULL;
        }
#if defined(CONFIG_THREAD_BR_DNS64) && CONFIG_THREAD_BR_DNS64
        if (message && completed.dns64_ptr && !dns64_ptr_prefix_live(instance, &completed)) {
            otMessageFree(message); message = NULL;
        }
        if (message && completed.dns64_a) {
            uint8_t prefix[32];
            if (!dns64_prefix(instance, prefix) || memcmp(prefix, completed.dns64_prefix, sizeof(prefix))) {
                otMessageFree(message); message = NULL;
            }
        }
#endif
        otPlatDnsUpstreamQueryDone(instance, completed.txn, message);
    }
}
#else
void br_dns_upstream_process(otInstance *instance, bool forwarding_ready)
{ (void)instance; (void)forwarding_ready; }
#endif
#if !defined(CONFIG_THREAD_BR_DNS64) || !CONFIG_THREAD_BR_DNS64
bool br_dns_upstream_set_dns64_exclusions(otInstance *instance,
    const struct br_dns64_prefix *prefixes, size_t count)
{ (void)instance; (void)prefixes; (void)count; return false; }
#endif
