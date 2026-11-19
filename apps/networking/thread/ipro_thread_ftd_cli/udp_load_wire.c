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

#include "udp_load_wire.h"

#include <string.h>

static uint32_t crc32(const uint8_t *data, size_t length)
{
    uint32_t crc = UINT32_C(0xffffffff);
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (unsigned bit = 0; bit < 8; ++bit) {
            uint32_t mask = 0u - (crc & 1u);
            crc = (crc >> 1) ^ (UINT32_C(0xedb88320) & mask);
        }
    }
    return ~crc;
}

static void write_u64(uint8_t *destination, uint64_t value)
{
    for (unsigned i = 0; i < 8; ++i) {
        destination[7u - i] = (uint8_t)value;
        value >>= 8;
    }
}

static uint64_t read_u64(const uint8_t *source)
{
    uint64_t value = 0;
    for (unsigned i = 0; i < 8; ++i) {
        value = (value << 8) | source[i];
    }
    return value;
}

int udp_load_encode(uint8_t *destination, size_t capacity,
                    uint64_t run_id, uint64_t sequence, uint64_t sent_ns)
{
    if (destination == NULL || capacity < UDP_LOAD_PACKET_SIZE) return -1;
    memcpy(destination, "IPU1", 4);
    write_u64(destination + 4, run_id);
    write_u64(destination + 12, sequence);
    write_u64(destination + 20, sent_ns);
    for (unsigned i = 0; i < 96; ++i) destination[28 + i] = (uint8_t)i;
    uint32_t crc = crc32(destination, 124);
    for (unsigned i = 0; i < 4; ++i) {
        destination[127u - i] = (uint8_t)crc;
        crc >>= 8;
    }
    return 0;
}

int udp_load_decode(const uint8_t *source, size_t length,
                    uint64_t *run_id, uint64_t *sequence, uint64_t *sent_ns)
{
    if (source == NULL || run_id == NULL || sequence == NULL || sent_ns == NULL) return -1;
    if (length != UDP_LOAD_PACKET_SIZE || memcmp(source, "IPU1", 4) != 0) return -2;
    uint32_t received = 0;
    for (unsigned i = 124; i < 128; ++i) received = (received << 8) | source[i];
    if (received != crc32(source, 124)) return -2;
    /* Read all fields before writing outputs, even if outputs alias source. */
    uint64_t run = read_u64(source + 4);
    uint64_t seq = read_u64(source + 12);
    uint64_t stamp = read_u64(source + 20);
    *run_id = run;
    *sequence = seq;
    *sent_ns = stamp;
    return 0;
}
