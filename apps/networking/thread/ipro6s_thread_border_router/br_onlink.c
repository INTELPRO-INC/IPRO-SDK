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

#include "br_onlink.h"
#include <string.h>

static uint32_t read_u32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | p[3];
}

bool br_onlink_expire(struct br_onlink_cache *cache, uint32_t now_ms)
{
    bool changed = false;
    if (!cache) return false;
    for (unsigned i = 0; i < BR_ONLINK_CAPACITY; ++i) {
        struct br_onlink_entry *entry = &cache->entries[i];
        if (entry->valid_ms && now_ms - entry->updated_ms >= entry->valid_ms) {
            memset(entry, 0, sizeof(*entry));
            changed = true;
        }
    }
    return changed;
}

bool br_onlink_contains(const struct br_onlink_cache *cache,
                        const uint8_t address[16], uint32_t now_ms)
{
    if (!cache || !address) return false;
    for (unsigned i = 0; i < BR_ONLINK_CAPACITY; ++i) {
        const struct br_onlink_entry *entry = &cache->entries[i];
        if (entry->valid_ms && now_ms - entry->updated_ms < entry->valid_ms &&
            memcmp(entry->prefix, address, 8) == 0) return true;
    }
    return false;
}

bool br_onlink_record(struct br_onlink_cache *cache, const uint8_t *ra,
                      size_t length, uint32_t now_ms)
{
    if (!cache || !ra || length < 16 || ra[0] != 134 || ra[1] != 0) return false;
    struct br_onlink_cache next = *cache;
    (void)br_onlink_expire(&next, now_ms);
    for (size_t offset = 16; offset < length;) {
        if (length - offset < 2) return false;
        const uint8_t *option = ra + offset;
        size_t bytes = (size_t)option[1] * 8;
        if (!bytes || bytes > length - offset) return false;
        if (option[0] == 3) {
            if (bytes != 32) return false;
            /* Only the /64 ULA/GUA PIOs emitted by this BR. L, not A or
             * preferred lifetime, authorizes direct neighbour resolution. */
            if (option[2] == 64 && (option[3] & 0x80) &&
                ((option[16] & 0xfe) == 0xfc || (option[16] & 0xe0) == 0x20)) {
                uint32_t valid = read_u32(option + 4);
                if (read_u32(option + 8) > valid) return false;
                unsigned slot = BR_ONLINK_CAPACITY;
                for (unsigned i = 0; i < BR_ONLINK_CAPACITY; ++i) {
                    if (next.entries[i].valid_ms &&
                        memcmp(next.entries[i].prefix, option + 16, 8) == 0) {
                        slot = i;
                        break;
                    }
                    if (!next.entries[i].valid_ms && slot == BR_ONLINK_CAPACITY) slot = i;
                }
                if (slot == BR_ONLINK_CAPACITY) {
                    if (valid) return false;
                } else if (!valid) {
                    memset(&next.entries[slot], 0, sizeof(next.entries[slot]));
                } else {
                    struct br_onlink_entry *entry = &next.entries[slot];
                    memcpy(entry->prefix, option + 16, 8);
                    entry->updated_ms = now_ms;
                    /* Bound lifetime to half the wrapping millisecond clock.
                     * Our normal locally advertised lifetime is 1800 s. */
                    entry->valid_ms = valid > INT32_MAX / 1000u ? INT32_MAX : valid * 1000u;
                }
            }
        }
        offset += bytes;
    }
    *cache = next;
    return true;
}
