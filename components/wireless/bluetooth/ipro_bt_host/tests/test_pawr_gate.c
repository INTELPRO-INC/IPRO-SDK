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

/* The same admission/drain protocol used by the board fixture. */
#include <assert.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
typedef _Atomic int atomic_t;
typedef int atomic_val_t;
static atomic_t fail_once;
static int atomic_get(atomic_t *p) { return atomic_load(p); }
static bool atomic_cas(atomic_t *p, int old, int next)
{
    if (atomic_exchange(&fail_once, 0)) return false;
    return atomic_compare_exchange_strong(p, &old, next);
}
static int atomic_or(atomic_t *p, int bits) { return atomic_fetch_or(p, bits); }
static int atomic_sub(atomic_t *p, int n) { return atomic_fetch_sub(p, n); }
#define PAWR_GATE_ATOMICS_PROVIDED
#include "board/pawr_gate.h"
static atomic_t racing_gate;
static atomic_t entered;
static atomic_t release_callback;
static void *callback(void *unused)
{
    (void)unused;
    assert(pawr_gate_enter(&racing_gate));
    atomic_store(&entered, 1);
    while (!atomic_load(&release_callback)) {}
    pawr_gate_leave(&racing_gate);
    return NULL;
}
int main(void)
{
    atomic_t gate = PAWR_GATE_CLOSED;
    assert(!pawr_gate_enter(&gate));
    assert(pawr_gate_open(&gate));
    atomic_store(&fail_once, 1); /* exercise retry after competing CAS */
    assert(pawr_gate_enter(&gate));
    assert(pawr_gate_enter(&gate));
    pawr_gate_close(&gate);
    assert(!pawr_gate_enter(&gate));
    assert(!pawr_gate_drained(&gate));
    assert(!pawr_gate_open(&gate));
    pawr_gate_leave(&gate);
    assert(!pawr_gate_drained(&gate));
    pawr_gate_leave(&gate);
    assert(pawr_gate_drained(&gate));
    pawr_gate_close(&gate); /* idempotent retry after stop timeout */
    assert(pawr_gate_open(&gate));
    assert(pawr_gate_enter(&gate));
    pawr_gate_leave(&gate);
    pawr_gate_close(&gate);
    assert(pawr_gate_drained(&gate));
    /* Deterministic in-flight HCI surrogate: closing cannot authorize delete
     * until this callback returns, and cannot admit a late callback. */
    pthread_t thread;
    assert(!pthread_create(&thread, NULL, callback, NULL));
    while (!atomic_load(&entered)) {}
    pawr_gate_close(&racing_gate);
    assert(!pawr_gate_drained(&racing_gate));
    assert(!pawr_gate_enter(&racing_gate));
    atomic_store(&release_callback, 1);
    assert(!pthread_join(thread, NULL));
    assert(pawr_gate_drained(&racing_gate));
    return 0;
}
