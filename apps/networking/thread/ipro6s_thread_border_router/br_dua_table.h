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

#ifndef IPRO_BR_DUA_TABLE_H
#define IPRO_BR_DUA_TABLE_H
#include <stdbool.h>
#include <stdint.h>

/* Must cover the upstream ND proxy table, not a reduced demonstration limit. */
#define BR_DUA_CAPACITY 250
struct br_dua_entry {
    uint8_t address[16];
    bool live;
    bool owned;
};
struct br_dua_table {
    struct br_dua_entry entries[BR_DUA_CAPACITY];
    bool active;
    bool overflow;
};
typedef bool (*br_dua_membership_fn)(void *context, const uint8_t group[16]);

/* All functions serialized by caller. Add is only for DAD-completed OT
 * callbacks, never arbitrary NS targets or GetInfo-only discoveries. */
bool br_dua_add(struct br_dua_table *table, const uint8_t address[16]);
void br_dua_remove(struct br_dua_table *table, const uint8_t address[16]);
void br_dua_clear(struct br_dua_table *table);
bool br_dua_contains(const struct br_dua_table *table, const uint8_t address[16]);
void br_dua_group(const uint8_t address[16], uint8_t group[16]);
/* Own one reference per DUA, including colliding low-24-bit groups. Failed
 * leaves stay tracked but never authorized; failed joins are retried. An
 * overflow latches fail-closed until a core Cleared event resets the table. */
void br_dua_sync(struct br_dua_table *table, bool active, br_dua_membership_fn join,
                 br_dua_membership_fn leave, void *context);
#endif
