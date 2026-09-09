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

#include <stdio.h>
#include <stdint.h>

#include "ipro_coex_ctx.h"

#define COEX_CTX_DEBUG(fmt, args...) printf(fmt, ##args)

/* See ipro_coex_ctx.h: the reference never defines this anywhere. */
struct ipro_coex_ctx ipro_coex_timing_control_ctx;

int ipro_coex_bt_inc(struct ipro_coex_ctx *ctx, int step)
{
    int target = (int)ctx->timer_toggle_end + step;

    /* The reference bounds-checks the *old* timer_toggle_end against
     * timer_max, so the window is allowed to overshoot by one step before it
     * refuses. Check the value we are about to store. */
    if (target <= 0 || (uint32_t)target > ctx->timer_max) {
        return -1;
    }
    ctx->timer_toggle_end = (uint32_t)target;

    COEX_CTX_DEBUG("[COEX] [CTX] using timer_toggle_end %lu\r\n",
                   (unsigned long)ctx->timer_toggle_end);

    return 0;
}

int ipro_coex_bt_dec(struct ipro_coex_ctx *ctx, int step)
{
    (void)ctx;
    (void)step;

    /* Empty in the reference too -- there is no shrink path yet. */
    return 0;
}
