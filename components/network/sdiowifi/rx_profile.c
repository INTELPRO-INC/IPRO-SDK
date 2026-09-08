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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "rx_profile.h"

extern uint32_t inline_hal_machw_time(void);

/* [ZCEN3 funding 07-24] 128 -> 32 recent-sample entries (28B each, ram_tcm
 * .bss): the aggregate rxprof_stats histograms are untouched, only the raw
 * dump depth shrinks. Frees 2688B for the upld chain state + heap floor.
 * [P4 funding 07-24] 32 -> 8: frees another 672B toward the DL hot-code
 * OCRAM set (must stay a power of two for RXPROF_MASK). */
#define RXPROF_RING_SZ  8u
#define RXPROF_MASK     (RXPROF_RING_SZ - 1u)
#define RXPROF_STAGES   5u

struct rxprof_entry {
    uint32_t t[6];
    uint16_t len;
    uint16_t flags;
};

static struct rxprof_entry rxprof_ring[RXPROF_RING_SZ];
static volatile uint32_t   rxprof_head;
static volatile uint32_t   rxprof_total;

struct rxprof_stage_stat {
    uint32_t count;
    uint32_t lt5us;
    uint32_t lt20us;
    uint32_t lt100us;
    uint32_t ge100us;
    uint32_t max_us;
    uint64_t sum_us;
};

static struct rxprof_stage_stat rxprof_stats[RXPROF_STAGES];

static const char *stage_names[RXPROF_STAGES] = {
    "D01 sd_cb",
    "D12 dispatch",
    "D23 alloc",
    "D34 memcpy",
    "D45 wifi_tx",
};

static inline void update_stat(struct rxprof_stage_stat *s, uint32_t dur)
{
    s->count++;
    s->sum_us += dur;
    if (dur > s->max_us) s->max_us = dur;
    if      (dur < 5u)   s->lt5us++;
    else if (dur < 20u)  s->lt20us++;
    else if (dur < 100u) s->lt100us++;
    else                 s->ge100us++;
}

void rxprof_t0(uint16_t len)
{
    struct rxprof_entry *e = &rxprof_ring[rxprof_head & RXPROF_MASK];
    e->t[0] = inline_hal_machw_time();
    e->t[1] = 0; e->t[2] = 0; e->t[3] = 0; e->t[4] = 0; e->t[5] = 0;
    e->len = len;
    e->flags = 0;
}

void rxprof_t1(void) { rxprof_ring[rxprof_head & RXPROF_MASK].t[1] = inline_hal_machw_time(); }
void rxprof_t2(void) { rxprof_ring[rxprof_head & RXPROF_MASK].t[2] = inline_hal_machw_time(); }
void rxprof_t3(void) { rxprof_ring[rxprof_head & RXPROF_MASK].t[3] = inline_hal_machw_time(); }
void rxprof_t4(void) { rxprof_ring[rxprof_head & RXPROF_MASK].t[4] = inline_hal_machw_time(); }

void rxprof_t5(void)
{
    struct rxprof_entry *e = &rxprof_ring[rxprof_head & RXPROF_MASK];
    uint32_t now = inline_hal_machw_time();
    e->t[5] = now;

    if (e->t[0] && e->t[1]) update_stat(&rxprof_stats[0], e->t[1] - e->t[0]);
    if (e->t[1] && e->t[2]) update_stat(&rxprof_stats[1], e->t[2] - e->t[1]);
    if (e->t[2] && e->t[3]) update_stat(&rxprof_stats[2], e->t[3] - e->t[2]);
    if (e->t[3] && e->t[4]) update_stat(&rxprof_stats[3], e->t[4] - e->t[3]);
    if (e->t[4]           ) update_stat(&rxprof_stats[4], now      - e->t[4]);

    e->flags = 1;
    rxprof_head++;
    rxprof_total++;
}

void rxprof_print(void)
{
    uint32_t total = rxprof_total;
    uint32_t i;

    printf("[RXPROF] total=%lu now=%lu\r\n",
           (unsigned long)total, (unsigned long)inline_hal_machw_time());
    printf("# stage           n   mean   max   <5us  <20us <100us >=100us\r\n");
    for (uint32_t s = 0; s < RXPROF_STAGES; s++) {
        struct rxprof_stage_stat *st = &rxprof_stats[s];
        uint32_t mean = st->count ? (uint32_t)(st->sum_us / st->count) : 0u;
        printf("%-13s %6lu %6lu %5lu %6lu %6lu %6lu %7lu\r\n",
               stage_names[s],
               (unsigned long)st->count,
               (unsigned long)mean,
               (unsigned long)st->max_us,
               (unsigned long)st->lt5us,
               (unsigned long)st->lt20us,
               (unsigned long)st->lt100us,
               (unsigned long)st->ge100us);
    }

    /* Dump the last 16 ring entries with full per-stage timings */
    uint32_t count = (rxprof_head < RXPROF_RING_SZ) ? rxprof_head : RXPROF_RING_SZ;
    uint32_t show  = (count < 16u) ? count : 16u;
    uint32_t start = rxprof_head - show;
    printf("# i  len  T0_us    D01  D12  D23  D34  D45  total\r\n");
    for (i = 0; i < show; i++) {
        uint32_t n = start + i;
        struct rxprof_entry *e = &rxprof_ring[n & RXPROF_MASK];
        if (!(e->flags & 1u)) continue;
        printf("[%02lu] %4u %8lu %4lu %4lu %4lu %4lu %4lu %5lu\r\n",
               (unsigned long)i, e->len,
               (unsigned long)e->t[0],
               (unsigned long)(e->t[1] - e->t[0]),
               (unsigned long)(e->t[2] - e->t[1]),
               (unsigned long)(e->t[3] - e->t[2]),
               (unsigned long)(e->t[4] - e->t[3]),
               (unsigned long)(e->t[5] - e->t[4]),
               (unsigned long)(e->t[5] - e->t[0]));
    }
}

void rxprof_reset(void)
{
    memset(rxprof_ring,  0, sizeof(rxprof_ring));
    memset(rxprof_stats, 0, sizeof(rxprof_stats));
    rxprof_head  = 0;
    rxprof_total = 0;
    printf("rxprof reset\r\n");
}
