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

#ifndef BR_MLD_SYNC_H
#define BR_MLD_SYNC_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Match the OT listener-table default; the build contract checks this bound. */
#define BR_MLD_MAX_GROUPS 75
typedef uint8_t br_mld_address[16];
typedef bool (*br_mld_operation)(void *context, const uint8_t address[16]);

/* Zero-initialize once. All access must be serialized by the caller.
 * Owns one reference per successful join, independent of other subscribers. */
struct br_mld_sync {
    br_mld_address owned[BR_MLD_MAX_GROUPS];
    size_t count;
    bool ready;
    uint32_t joins, leaves, join_errors, leave_errors, invalid_snapshots;
};

/* Required callbacks return true only when the reference was acquired/released.
 * The desired snapshot must not alias state. Inactive/invalid input revokes
 * authorization immediately, even if a leave fails and must be retried. */
void br_mld_sync_update(struct br_mld_sync *state, const br_mld_address *desired,
                        size_t count, bool active, br_mld_operation join,
                        br_mld_operation leave, void *context);
bool br_mld_sync_contains(const struct br_mld_sync *state, const uint8_t address[16]);
#endif
