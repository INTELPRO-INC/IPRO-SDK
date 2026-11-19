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

#ifndef BR_CLI_TIMING_STORE_H
#define BR_CLI_TIMING_STORE_H
#include <stdint.h>

#define BR_CLI_TIMING_CAPACITY 16u
/* Raw ticks, modulo 2^32. No command text, arguments or asynchronous Done time. */
struct br_cli_timing_event {
    uint32_t sequence, entered, submitted, started, returned;
};
struct br_cli_timing_store {
    struct br_cli_timing_event events[BR_CLI_TIMING_CAPACITY];
    unsigned next, count;
    uint32_t overwritten;
};
/* Caller supplies initialized storage and serializes access. */
void br_cli_timing_push(struct br_cli_timing_store *store,
                        const struct br_cli_timing_event *event);
#endif
