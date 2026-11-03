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

/* Host stand-in for ipro_ai_port_ipro7.c: records what the runtime asks of the platform. */
#ifndef FAKE_PORT_H
#define FAKE_PORT_H

#include "ipro_ai_port.h"

typedef struct {
    /* set by the test */
    ai_port_net_t net;        /* what a load reports */
    int  load_rc;             /* 0, or a negative error the load returns */
    int  run_rc;
    int  timeouts_left;       /* this many runs time out (IPRO_AI_E_TIMEOUT) before run_rc applies */
    int  fault_after;         /* consecutive timeouts that turn into a fault (0: never) */
    int  trylock_fails;       /* this many ai_port_trylock() calls fail first */
    uint32_t run_us;          /* clock advance per run */
    uint32_t out_bytes_avail; /* the work buffer's output slot */
    /* observed */
    int  lock_depth;
    int  max_lock_depth;
    int  locks;
    int  loads, unloads, runs;
    int  run_unlocked;        /* a run or load happened without the lock */
    const uint8_t *load_ext, *run_ext, *run_input;
    uint32_t run_in_bytes, run_out_bytes;
    uint32_t run_timeout_ms;  /* what the last run was given */
    ipro_ai_npu_health_t npu; /* the completion path's counters, as components/ai keeps them */
    int  clears;
    long allocs;              /* live allocations made through ai_port_alloc */
    size_t max_alloc;         /* largest single allocation */
    uint8_t out[4096];
} fake_port_t;

extern fake_port_t g_fake;

void fake_reset(void);

#endif
