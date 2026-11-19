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
#include "br_dua_table.h"

void br_dua_group(const uint8_t address[16], uint8_t group[16])
{
    static const uint8_t prefix[13] = {0xff, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0xff};
    memcpy(group, prefix, sizeof(prefix));
    memcpy(group + 13, address + 13, 3);
}

bool br_dua_add(struct br_dua_table *table, const uint8_t address[16])
{
    if (!table || !address) return false;
    struct br_dua_entry *empty = NULL;
    for (unsigned i = 0; i < BR_DUA_CAPACITY; ++i) {
        struct br_dua_entry *entry = &table->entries[i];
        if ((entry->live || entry->owned) && memcmp(entry->address, address, 16) == 0) {
            entry->live = true;
            return true;
        }
        if (!entry->live && !entry->owned && !empty) empty = entry;
    }
    if (!empty) {
        table->overflow = true;
        return false;
    }
    memcpy(empty->address, address, 16);
    empty->live = true;
    return true;
}

void br_dua_remove(struct br_dua_table *table, const uint8_t address[16])
{
    if (!table || !address) return;
    for (unsigned i = 0; i < BR_DUA_CAPACITY; ++i) {
        struct br_dua_entry *entry = &table->entries[i];
        if (entry->live && memcmp(entry->address, address, 16) == 0) entry->live = false;
    }
}

void br_dua_clear(struct br_dua_table *table)
{
    if (!table) return;
    for (unsigned i = 0; i < BR_DUA_CAPACITY; ++i) table->entries[i].live = false;
    table->active = table->overflow = false;
}

bool br_dua_contains(const struct br_dua_table *table, const uint8_t address[16])
{
    if (!table || !address || !table->active || table->overflow) return false;
    for (unsigned i = 0; i < BR_DUA_CAPACITY; ++i) {
        const struct br_dua_entry *entry = &table->entries[i];
        if (entry->live && entry->owned && memcmp(entry->address, address, 16) == 0) return true;
    }
    return false;
}

void br_dua_sync(struct br_dua_table *table, bool active, br_dua_membership_fn join,
                 br_dua_membership_fn leave, void *context)
{
    if (!table) return;
    table->active = active && !table->overflow;
    for (unsigned i = 0; i < BR_DUA_CAPACITY; ++i) {
        struct br_dua_entry *entry = &table->entries[i];
        bool wanted = table->active && entry->live;
        uint8_t group[16];
        if (entry->owned == wanted) continue;
        br_dua_group(entry->address, group);
        if (wanted) {
            if (join && join(context, group)) entry->owned = true;
        } else if (leave && leave(context, group)) entry->owned = false;
    }
}
