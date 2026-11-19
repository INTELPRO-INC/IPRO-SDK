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

#include "br_nd_proxy_packet.h"
#include "br_nd.h"
#include <stdbool.h>
#include <string.h>

static const uint8_t zero[16];
static const uint8_t all_nodes[16] = {0xff, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

static uint32_t sum_bytes(const uint8_t *data, size_t length)
{
    uint32_t result = 0;
    for (size_t i = 0; i < length; i += 2)
        result += ((uint32_t)data[i] << 8) | data[i + 1];
    return result; /* All fixed-size inputs to this helper have even length. */
}

static size_t advertise(const uint8_t *target, const uint8_t *local_source, const uint8_t *mac,
                        const uint8_t *destination, bool solicited, uint32_t age,
                        uint8_t *output, size_t capacity)
{
    if (!target || !local_source || !mac || !output || capacity < BR_ND_PROXY_NA_SIZE) return 0;
    /* A remote DUA is a non-link-local unicast address, never a local alias. */
    if (target[0] == 0xff || (target[0] == 0xfe && (target[1] & 0xc0) == 0x80) ||
        (memcmp(target, zero, 15) == 0 && target[15] <= 1) ||
        (memcmp(target, zero, 10) == 0 && target[10] == 0xff && target[11] == 0xff) ||
        local_source[0] != 0xfe || (local_source[1] & 0xc0) != 0x80 ||
        (mac[0] & 1) || memcmp(mac, zero, 6) == 0) return 0;

    uint8_t packet[BR_ND_PROXY_NA_SIZE] = {0};
    packet[0] = 0x60;
    packet[5] = 32;
    packet[6] = 58;
    packet[7] = 255;
    memcpy(packet + 8, local_source, 16);
    memcpy(packet + 24, destination, 16);
    packet[40] = 136;
    packet[44] = 0x80 | (solicited ? 0x40 : 0) | (age <= BR_DUA_RECENT_SECONDS ? 0x20 : 0);
    memcpy(packet + 48, target, 16);
    packet[64] = 2;
    packet[65] = 1;
    memcpy(packet + 66, mac, 6);
    uint32_t sum = sum_bytes(packet + 8, 32) + 32 + 58 + sum_bytes(packet + 40, 32);
    while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);
    uint16_t checksum = (uint16_t)~sum;
    packet[42] = (uint8_t)(checksum >> 8);
    packet[43] = (uint8_t)checksum;
    memcpy(output, packet, sizeof(packet));
    return sizeof(packet);
}

size_t br_nd_proxy_na(const uint8_t *request, size_t length,
                     const uint8_t target[16], const uint8_t local_source[16],
                     const uint8_t mac[6], uint32_t age_seconds,
                     uint8_t *output, size_t capacity)
{
    int offset = br_nd_ns_validate(request, length);
    if (!offset || !target || memcmp(target, request + offset + 8, 16) != 0) return 0;
    bool dad = memcmp(request + 8, zero, 16) == 0;
    return advertise(target, local_source, mac, dad ? all_nodes : request + 8,
                     !dad, age_seconds, output, capacity);
}

size_t br_nd_proxy_announce(const uint8_t target[16], const uint8_t local_source[16],
                           const uint8_t mac[6], uint32_t age_seconds,
                           uint8_t *output, size_t capacity)
{
    return advertise(target, local_source, mac, all_nodes, false, age_seconds, output, capacity);
}
