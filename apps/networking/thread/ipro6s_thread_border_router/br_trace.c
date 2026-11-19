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

#include "br_trace.h"
#include "br_trace_store.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lwip/pbuf.h"
#include "ipro_log.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#include "lwip/tcpip.h"
#include "lwip/priv/nd6_priv.h"
#include <string.h>
#ifdef CONFIG_THREAD_BR_PACKET_TRACE
static struct br_trace_store sStore;
static bool sDumpBusy;
static uint32_t sDumpSkipped;
struct rx_echo { uint16_t id, seq; uint32_t lease, dispatch, dequeue, handoff; };
static struct rx_echo sRxEcho[16];
static unsigned sRxNext, sRxCount;
static uint32_t sRxOverwritten;
struct tx_echo { uint32_t key, time; int16_t status; uint8_t stage; };
static struct tx_echo sTxEcho[32];
static unsigned sTxNext, sTxCount;
static uint32_t sTxOverwritten;
#endif
#define TAG "BR-Trace"
#endif

void br_trace_bytes(enum br_trace_stage stage, const uint8_t *header, size_t length, int status)
{
#if defined(CONFIG_SHELL) && defined(CONFIG_THREAD_BR_PACKET_TRACE)
    struct br_trace_event event = {0};
    if (!br_trace_decode(header, length, &event)) return;
    event.tick = (uint32_t)xTaskGetTickCount();
    event.stage = (uint8_t)stage;
    event.status = (int16_t)status;
    taskENTER_CRITICAL();
    if (!sDumpBusy) br_trace_push(&sStore, &event);
    else if (sDumpSkipped != UINT32_MAX) ++sDumpSkipped;
    taskEXIT_CRITICAL();
#else
    (void)stage; (void)header; (void)length; (void)status;
#endif
}

#if defined(CONFIG_SHELL) && defined(CONFIG_THREAD_BR_PACKET_TRACE)
bool net_al_ip4_echo_reply_key(const uint8_t *header, size_t length, uint32_t *key)
{
    uint8_t normalized[28];
    uint16_t id, seq;
    if (!header || !key || length < sizeof(normalized) || header[20] != 0) return false;
    memcpy(normalized, header, sizeof(normalized));
    normalized[20] = 8;
    if (!br_trace_ip4_echo(normalized, sizeof(normalized), &id, &seq)) return false;
    *key = ((uint32_t)id << 16) | seq;
    return true;
}
void net_al_ip4_echo_tx_trace(uint32_t key, uint32_t time, unsigned stage, int status)
{
    const struct tx_echo event = {.key=key, .time=time, .status=(int16_t)status, .stage=(uint8_t)stage};
    taskENTER_CRITICAL();
    sTxEcho[sTxNext] = event;
    sTxNext = (sTxNext + 1) % 32;
    if (sTxCount < 32) ++sTxCount;
    else if (sTxOverwritten != UINT32_MAX) ++sTxOverwritten;
    taskEXIT_CRITICAL();
}
void net_al_ip6_tx_trace(const uint8_t *header, size_t length, int status)
{
    br_trace_bytes(BR_TRACE_WIFI_TX, header, length, status);
}
#endif

void br_trace_pbuf(enum br_trace_stage stage, const struct pbuf *p, int status)
{
#if defined(CONFIG_SHELL) && defined(CONFIG_THREAD_BR_PACKET_TRACE)
    uint8_t header[48];
    if (p && pbuf_copy_partial(p, header, sizeof(header), 0) == sizeof(header))
        br_trace_bytes(stage, header, sizeof(header), status);
#else
    (void)stage; (void)p; (void)status;
#endif
}

#ifdef CONFIG_SHELL
#ifdef CONFIG_THREAD_BR_PACKET_TRACE
void br_wifi_rx_echo(const uint8_t *header, size_t length, uint32_t lease,
                     uint32_t dispatch, uint32_t dequeue, uint32_t handoff)
{
    struct rx_echo event = {.lease=lease, .dispatch=dispatch, .dequeue=dequeue, .handoff=handoff};
    if (!br_trace_ip4_echo(header, length, &event.id, &event.seq)) return;
    taskENTER_CRITICAL();
    sRxEcho[sRxNext] = event;
    sRxNext = (sRxNext + 1) % 16;
    if (sRxCount < 16) ++sRxCount;
    else if (sRxOverwritten != UINT32_MAX) ++sRxOverwritten;
    taskEXIT_CRITICAL();
}
static void cmd_rx_echo(int argc, char **argv)
{
    (void)argc; (void)argv;
    struct rx_echo snapshot[16];
    taskENTER_CRITICAL();
    const unsigned count = sRxCount, next = sRxNext;
    const uint32_t overwritten = sRxOverwritten;
    memcpy(snapshot, sRxEcho, sizeof(snapshot));
    taskEXIT_CRITICAL();
    IPRO_LOGI(TAG, "rx_echo count=%u overwritten=%lu; fixed IPv4 requests only; shared forward hook may include AMSDU subframes", count, (unsigned long)overwritten);
    for (unsigned i=0; i<count; ++i) {
        const struct rx_echo *e = &snapshot[(next + 16 - count + i) % 16];
        IPRO_LOGI(TAG, "rx_echo id=%u seq=%u lease=%lu dispatch=%lu dequeue=%lu handoff=%lu valid=%u",
                  e->id, e->seq, (unsigned long)e->lease, (unsigned long)e->dispatch,
                  (unsigned long)e->dequeue, (unsigned long)e->handoff,
                  e->lease && e->dispatch && e->dequeue && e->handoff);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_rx_echo, br_wifi_rx_echo_trace, Snapshot bounded IPv4 Echo RX timestamps);
static void cmd_tx_echo(int argc, char **argv)
{
    (void)argc; (void)argv;
    struct tx_echo snapshot[32];
    taskENTER_CRITICAL();
    const unsigned count = sTxCount, next = sTxNext;
    const uint32_t overwritten = sTxOverwritten;
    memcpy(snapshot, sTxEcho, sizeof(snapshot));
    taskEXIT_CRITICAL();
    IPRO_LOGI(TAG, "tx_echo count=%u overwritten=%lu stages: 0=admission-result 1=completion-ack 2=completion-unacked-or-discarded", count, (unsigned long)overwritten);
    for (unsigned i=0; i<count; ++i) {
        const struct tx_echo *e = &snapshot[(next + 32 - count + i) % 32];
        IPRO_LOGI(TAG, "tx_echo id=%u seq=%u time=%lu stage=%u status=%d",
                  (unsigned)(e->key >> 16), (unsigned)(e->key & 0xffff),
                  (unsigned long)e->time, e->stage, e->status);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_tx_echo, br_wifi_tx_echo_trace, Snapshot bounded IPv4 Echo TX admission and completion);

#endif
static void cmd_trace(int argc, char **argv)
{
    (void)argc; (void)argv;
#ifdef CONFIG_THREAD_BR_PACKET_TRACE
    taskENTER_CRITICAL();
    if (sDumpBusy) {
        taskEXIT_CRITICAL();
        IPRO_LOGW(TAG, "dump already active");
        return;
    }
    sDumpBusy = true;
    sDumpSkipped = 0;
    const unsigned next = sStore.next;
    const unsigned count = sStore.count;
    const uint32_t overwritten = sStore.overwritten;
    taskEXIT_CRITICAL();
    IPRO_LOGI(TAG, "schema=5 collection_frozen=1 capacity=%u count=%u overwritten=%lu stages: 0=wifi-in 1=queued 2=ot-send 3=ot-rx 4=route-drop 5=queue-full 6=alloc-drop 7=wifi-tx",
              BR_TRACE_CAPACITY, count, (unsigned long)overwritten);
    unsigned first = (next + BR_TRACE_CAPACITY - count) % BR_TRACE_CAPACITY;
    for (unsigned i = 0; i < count; ++i) {
        const struct br_trace_event *e = &sStore.events[(first + i) % BR_TRACE_CAPACITY];
        char source[33], destination[33];
        static const char hex[] = "0123456789abcdef";
        for (unsigned j = 0; j < 16; ++j) {
            source[j * 2] = hex[e->source[j] >> 4];
            source[j * 2 + 1] = hex[e->source[j] & 15];
            destination[j * 2] = hex[e->destination[j] >> 4];
            destination[j * 2 + 1] = hex[e->destination[j] & 15];
        }
        source[32] = destination[32] = 0;
        const char *peer = (e->type == 128 || (e->type == 17 && e->sequence == 5540))
                               ? destination : source;
        IPRO_LOGI(TAG, "tick=%lu stage=%u type=%u id=%u seq=%u udp_len=%u udp_checksum=%u result=%d peer=%s src=%s dst=%s",
                  (unsigned long)e->tick, e->stage, e->type, e->identifier,
                  e->sequence, e->udp_length, e->udp_checksum, e->status, peer, source, destination);
    }
    taskENTER_CRITICAL();
    const uint32_t skipped = sDumpSkipped;
    sDumpBusy = false;
    taskEXIT_CRITICAL();
    IPRO_LOGI(TAG, "collection_frozen=0 skipped_during_dump=%lu (not forwarding drops)", (unsigned long)skipped);
#else
    IPRO_LOGI(TAG, "packet trace disabled; enable CONFIG_THREAD_BR_PACKET_TRACE for diagnostics");
#endif
}
SHELL_CMD_EXPORT_ALIAS(cmd_trace, br_packet_trace, Dump bounded Echo and Matter UDP metadata);

/* Snapshot only: never create a neighbour, send traffic, or invalidate a route. */
static void cmd_route_cache(int argc, char **argv)
{
    ip6_addr_t target, hop;
    if (argc != 2 || !ip6addr_aton(argv[1], &target) || ip6_addr_isany(&target)) {
        IPRO_LOGW(TAG, "usage: br_route_cache <nonzero IPv6 address>");
        return;
    }
    bool found = false;
    unsigned mtu = 0;
    int neighbor_state = -1, queued = -1;
    LOCK_TCPIP_CORE();
    for (unsigned i = 0; i < LWIP_ND6_NUM_DESTINATIONS; ++i) {
        if (!ip6_addr_cmp(&target, &destination_cache[i].destination_addr)) continue;
        ip6_addr_copy(hop, destination_cache[i].next_hop_addr);
        mtu = destination_cache[i].pmtu;
        found = true;
        for (unsigned j = 0; j < LWIP_ND6_NUM_NEIGHBORS; ++j) {
            if (neighbor_cache[j].state == ND6_NO_ENTRY ||
                !ip6_addr_cmp(&hop, &neighbor_cache[j].next_hop_address)) continue;
            neighbor_state = neighbor_cache[j].state;
            queued = neighbor_cache[j].q != NULL;
            break;
        }
        break;
    }
    UNLOCK_TCPIP_CORE();
    if (!found) {
        IPRO_LOGI(TAG, "route_cache found=0 (no retained destination entry)");
        return;
    }
    char destination_text[IP6ADDR_STRLEN_MAX], hop_text[IP6ADDR_STRLEN_MAX];
    ip6addr_ntoa_r(&target, destination_text, sizeof(destination_text));
    ip6addr_ntoa_r(&hop, hop_text, sizeof(hop_text));
    IPRO_LOGI(TAG, "route_cache found=1 dst=%s next_hop=%s mtu=%u neighbor_state=%d queued=%d",
              destination_text, hop_text, mtu, neighbor_state, queued);
}
SHELL_CMD_EXPORT_ALIAS(cmd_route_cache, br_route_cache, Read cached IPv6 next hop without sending traffic);
#endif
