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

#include "br_cli_timing_store.h"

void br_cli_timing_push(struct br_cli_timing_store *store,
                        const struct br_cli_timing_event *event)
{
    store->events[store->next] = *event;
    store->next = (store->next + 1) % BR_CLI_TIMING_CAPACITY;
    if (store->count < BR_CLI_TIMING_CAPACITY) store->count++;
    else store->overwritten++;
}
