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
#include "br_mld_sync.h"

static bool contains(const br_mld_address *addresses, size_t count, const uint8_t address[16])
{
    for (size_t i = 0; i < count; ++i)
        if (memcmp(addresses[i], address, 16) == 0) return true;
    return false;
}

void br_mld_sync_update(struct br_mld_sync *state, const br_mld_address *desired,
                        size_t count, bool active, br_mld_operation join,
                        br_mld_operation leave, void *context)
{
    bool valid = count <= BR_MLD_MAX_GROUPS && (count == 0 || desired != NULL);
    if (valid) for (size_t i = 0; i < count; ++i) {
        unsigned scope = desired[i][1] & 15;
        if (desired[i][0] != 0xff || scope <= 3 || scope == 15) valid = false;
    }
    if (!valid) ++state->invalid_snapshots;
    active = active && valid;
    state->ready = false;
    bool complete = true;

    /* Release old references first so a full table can replace a listener. */
    for (size_t i = 0; i < state->count;) {
        if (active && contains(desired, count, state->owned[i])) {
            ++i;
        } else if (leave(context, state->owned[i])) {
            --state->count;
            if (i < state->count)
                memmove(state->owned[i], state->owned[i + 1],
                        (state->count - i) * sizeof(br_mld_address));
            ++state->leaves;
        } else {
            ++state->leave_errors;
            complete = false;
            ++i;
        }
    }
    if (active) for (size_t i = 0; i < count; ++i) {
        if (contains(state->owned, state->count, desired[i])) continue;
        if (state->count == BR_MLD_MAX_GROUPS || !join(context, desired[i])) {
            ++state->join_errors;
            complete = false;
            continue;
        }
        memcpy(state->owned[state->count++], desired[i], 16);
        ++state->joins;
    }
    state->ready = active && complete;
}

bool br_mld_sync_contains(const struct br_mld_sync *state, const uint8_t address[16])
{
    return address && state->ready && contains(state->owned, state->count, address);
}
