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

#ifndef __IPRO_COEX_CTX_H__
#define __IPRO_COEX_CTX_H__

#include <stdint.h>
#include <FreeRTOS.h>
#include <timers.h>

#include <ipro_coex.h>

/**
 * Timing-control state for the BT-side backoff window. Layout is the
 * reference's. Nothing creates coexTimer yet -- upstream never did either; the
 * only writer today is the A2DP-underrun event, which widens the window.
 */
struct ipro_coex_ctx {
    TimerHandle_t coexTimer;
    uint32_t timer_now;
    uint32_t timer_max;
    uint32_t timer_toggle_start;
    uint32_t timer_toggle_end;
    uint32_t timeus_last_called;
    uint32_t time_step_inc;
    uint32_t time_step_dec;
};

/**
 * The reference declares this extern in every impl_<chip>.c and defines it in
 * no translation unit at all, so the module as shipped does not link once the
 * event path is referenced. Defined in ipro_coex_ctx.c.
 */
extern struct ipro_coex_ctx ipro_coex_timing_control_ctx;

int ipro_coex_bt_inc(struct ipro_coex_ctx *ctx, int step);
int ipro_coex_bt_dec(struct ipro_coex_ctx *ctx, int step);

#endif /* __IPRO_COEX_CTX_H__ */
