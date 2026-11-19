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

#ifndef BR_TRACE_STORE_H
#define BR_TRACE_STORE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef CONFIG_THREAD_BR_PACKET_TRACE
#define BR_TRACE_CAPACITY 256
#else
#define BR_TRACE_CAPACITY 64
#endif
struct br_trace_event {
    uint32_t tick;
    uint16_t identifier, sequence;
    uint16_t udp_length, udp_checksum; /* Zero for ICMP; ports use identifier/sequence. */
    int16_t status;
    uint8_t stage, type;
    uint8_t source[16], destination[16]; /* IPv6 header addresses only; no payload. */
};
struct br_trace_store {
    struct br_trace_event events[BR_TRACE_CAPACITY];
    unsigned next, count;
    uint32_t overwritten;
};
/* Fixed IPv6 + ICMPv6 Echo or UDP port 5540 header only; extension headers are deliberately omitted.
 * This is diagnostic parsing, NOT packet validation or authorization. */
bool br_trace_decode(const uint8_t *header, size_t length, struct br_trace_event *event);
bool br_trace_ip4_echo(const uint8_t *header, size_t length, uint16_t *identifier, uint16_t *sequence);
/* Caller serializes access; store must be zero-initialized. */
void br_trace_push(struct br_trace_store *store, const struct br_trace_event *event);
#endif
