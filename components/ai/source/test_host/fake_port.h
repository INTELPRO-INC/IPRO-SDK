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

/* Fake platform for the source framework's host tests: a clock the test moves. */
#ifndef FAKE_SRC_PORT_H
#define FAKE_SRC_PORT_H

#include <stdint.h>

extern uint64_t g_now_us;            /* ai_src_port_now_us(); sleeps advance it */
extern uint32_t g_slept_ms;
extern int      g_periodic_fail;     /* next periodic_start fails */
extern uint32_t g_periodic_rate;     /* rate of the running periodic task, 0 when none */
extern void   (*g_periodic_tick)(void *);
extern void    *g_periodic_arg;

#endif
