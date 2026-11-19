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

#include "br_trace_store.h"
#include <string.h>

bool br_trace_ip4_echo(const uint8_t *h, size_t length, uint16_t *id, uint16_t *seq)
{
    /* Diagnostic fixed-header parsing only, not checksum validation. */
    if (!h || !id || !seq || length < 28 || h[0] != 0x45 || h[9] != 1 ||
        (((unsigned)h[2] << 8) | h[3]) < 28 ||
        (h[6] & 0x3f) || h[7] || h[20] != 8 || h[21]) return false;
    *id = ((uint16_t)h[24] << 8) | h[25];
    *seq = ((uint16_t)h[26] << 8) | h[27];
    return true;
}

bool br_trace_decode(const uint8_t *header, size_t length, struct br_trace_event *event)
{
    if (!header || !event || length < 48 || (header[0] >> 4) != 6 ||
        ((unsigned)header[4] << 8 | header[5]) < 8) return false;
    const uint16_t payload_length = (uint16_t)((unsigned)header[4] << 8 | header[5]);
    if (header[6] == 17) {
        const uint16_t source = (uint16_t)((unsigned)header[40] << 8 | header[41]);
        const uint16_t destination = (uint16_t)((unsigned)header[42] << 8 | header[43]);
        const uint16_t udp_length = (uint16_t)((unsigned)header[44] << 8 | header[45]);
        if ((source != 5540 && destination != 5540) || udp_length < 8 ||
            udp_length != payload_length) return false;
        event->type = 17;
        event->identifier = source;
        event->sequence = destination;
        event->udp_length = udp_length;
        event->udp_checksum = (uint16_t)((unsigned)header[46] << 8 | header[47]);
    } else {
        if (header[6] != 58 || (header[40] != 128 && header[40] != 129) ||
            header[41] != 0) return false;
        event->type = header[40];
        event->identifier = (uint16_t)((unsigned)header[44] << 8 | header[45]);
        event->sequence = (uint16_t)((unsigned)header[46] << 8 | header[47]);
        event->udp_length = 0;
        event->udp_checksum = 0;
    }
    memcpy(event->source, header + 8, 16);
    memcpy(event->destination, header + 24, 16);
    return true;
}

void br_trace_push(struct br_trace_store *store, const struct br_trace_event *event)
{
    store->events[store->next] = *event;
    store->next = (store->next + 1) % BR_TRACE_CAPACITY;
    if (store->count < BR_TRACE_CAPACITY) store->count++;
    else store->overwritten++;
}
