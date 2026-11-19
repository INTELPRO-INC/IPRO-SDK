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

#include "br_nd.h"
#include <stdbool.h>
#include <string.h>

static const uint8_t zero[16];

static bool valid_unicast(const uint8_t *address)
{
    return address[0] != 0xff &&
           !(memcmp(address, zero, 15) == 0 && address[15] <= 1) &&
           !(memcmp(address, zero, 10) == 0 && address[10] == 0xff && address[11] == 0xff);
}

static uint32_t sum_bytes(const uint8_t *data, size_t length)
{
    uint32_t sum = 0;
    while (length >= 2) {
        sum += ((uint32_t)data[0] << 8) | data[1];
        data += 2;
        length -= 2;
    }
    if (length) sum += (uint32_t)data[0] << 8;
    return sum;
}

static int validate(const uint8_t *packet, size_t length, bool ns_only)
{
    if (!packet || length < 40 || (packet[0] >> 4) != 6 || packet[7] != 255) return 0;
    size_t end = 40 + ((size_t)packet[4] << 8) + packet[5];
    if (end > length || end > 1500) return 0;
    size_t offset = 40;
    uint8_t next = packet[6];
    unsigned extensions = 0;
    unsigned destination_seen = 0;
    while (next == 0 || next == 60) {
        if (++extensions > 8 || offset + 2 > end) return 0;
        if ((next == 0 && offset != 40) || (next == 60 && destination_seen++)) return 0;
        size_t size = ((size_t)packet[offset + 1] + 1) * 8;
        if (size > end - offset) return 0;
        for (size_t option = offset + 2; option < offset + size;) {
            uint8_t type = packet[option];
            if (type == 0) { ++option; continue; } /* Pad1 */
            if (option + 2 > offset + size || (type & 0xc0)) return 0;
            size_t option_size = 2 + (size_t)packet[option + 1];
            if (option_size > offset + size - option) return 0;
            if (type == 5 && packet[option + 1] != 2) return 0; /* Router Alert */
            option += option_size;
        }
        next = packet[offset];
        offset += size;
    }
    /* Fragmented ND is forbidden (RFC 6980). Other protocols are not ND. */
    if (next != 58 || end - offset < 8) return 0;
    const uint8_t *icmp = packet + offset;
    size_t minimum;
    switch (icmp[0]) {
    case 133: minimum = 8; break;
    case 134: minimum = 16; break;
    case 135: case 136: minimum = 24; break;
    case 137: minimum = 40; break;
    default: return 0;
    }
    size_t icmp_length = end - offset;
    if (icmp[1] != 0 || icmp_length < minimum) return 0;
    bool dad = memcmp(packet + 8, zero, 16) == 0;
    if (!ns_only) {
        if (icmp[0] == 133) {
            /* RFC 4861 4.1: a host may solicit before its address is ready,
             * or use any unicast address assigned to its interface. */
            if (!dad && !valid_unicast(packet + 8)) return 0;
        } else if (packet[8] != 0xfe || (packet[9] & 0xc0) != 0x80) return 0;
    }
    if (ns_only) {
        if (icmp[0] != 135 || (!dad && !valid_unicast(packet + 8)) ||
            !valid_unicast(icmp + 8)) return 0;
        uint8_t solicited[16] = {0xff, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0xff};
        memcpy(solicited + 13, icmp + 21, 3);
        bool multicast = memcmp(packet + 24, solicited, 16) == 0;
        if (!multicast && (dad || memcmp(packet + 24, icmp + 8, 16) != 0)) return 0;
    }
    /* Reject malformed options before forwarding to OpenThread. */
    for (size_t option = minimum; option < icmp_length;) {
        if (icmp_length - option < 2 || !icmp[option + 1]) return 0;
        size_t size = (size_t)icmp[option + 1] * 8;
        if (size > icmp_length - option) return 0;
        if (ns_only && icmp[option] == 1 && (dad || size != 8)) return 0;
        if (!ns_only && icmp[0] == 133 && icmp[option] == 1 && (dad || size != 8)) return 0;
        option += size;
    }
    uint32_t sum = sum_bytes(packet + 8, 32) + (uint32_t)icmp_length + 58;
    sum += sum_bytes(icmp, icmp_length);
    while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);
    return sum == 0xffff ? (int)offset : 0;
}

int br_nd_validate(const uint8_t *packet, size_t length)
{
    return validate(packet, length, false);
}

int br_nd_ns_validate(const uint8_t *packet, size_t length)
{
    return validate(packet, length, true);
}
