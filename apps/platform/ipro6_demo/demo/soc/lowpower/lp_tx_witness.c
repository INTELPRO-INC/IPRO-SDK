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

/* Bounded TX ownership diagnostics. No allocation and no TX state mutation. */
#if defined(LP_TXW_HOST_TEST) || (defined(CONFIG_IPRO6) && defined(CONFIG_USE_MACSW) && \
    defined(CONFIG_LWIP_LP) && !IPRO_WIFI_LP_FW)
#define LP_TXW_IMPLEMENTATION
#include "lp_tx_witness.h"
#include <stdio.h>
#include <string.h>

#ifdef LP_TXW_HOST_TEST
#define txw_irq_save test_irq_save
#define txw_irq_restore test_irq_restore
#define txw_rtc test_rtc
#else
#include "drv_hbn.h"
#include "system_soc.h"
#define txw_irq_save __irq_save
#define txw_irq_restore __irq_restore
#define txw_rtc HBN_Get_RTC_Timer_Val
#endif

/* Carrier BSS only; this array is outside the LPFW descriptor arena. */
static struct lp_txw_row txw_rows[LP_TXW_ROWS];
struct lp_txw_ring {
    uint32_t next;
    uint32_t used;
    uint32_t total;
    uint32_t overwritten;
};
static struct lp_txw_ring txw_ring;
static uint32_t txw_sequence;
volatile uint32_t lp_txw_enabled;

void lp_txw_record(uint32_t site, int32_t delta, uint32_t ac, uintptr_t txdesc,
                   uint32_t pck_after, uintptr_t caller)
{
    uint32_t irq;
    struct lp_txw_row *row;

    if (!lp_txw_enabled)
        return;

    /* This protects only ring reservation/write, after the original pck RMW. */
    irq = txw_irq_save();
    if (!lp_txw_enabled) {
        txw_irq_restore(irq);
        return;
    }

    row = &txw_rows[txw_ring.next];
    row->sequence = ++txw_sequence;
    txw_rtc(&row->rtc_lo, &row->rtc_hi);
    row->site = site;
    row->delta = delta;
    row->ac = ac;
    row->txdesc = (uint32_t)txdesc;
    row->pck_after = pck_after;
    row->caller = (uint32_t)caller;
    txw_ring.next = (txw_ring.next + 1) % LP_TXW_ROWS;
    txw_ring.total++;
    if (txw_ring.used < LP_TXW_ROWS)
        txw_ring.used++;
    else
        txw_ring.overwritten++;
    txw_irq_restore(irq);
}

void lp_txw_control(const char *operation)
{
    uint32_t irq;

    if (strcmp(operation, "on") && strcmp(operation, "off") &&
        strcmp(operation, "clear") && strcmp(operation, "dump")) {
        printf("[TXW] usage: lp txw on|off|clear|dump\r\n");
        return;
    }

    irq = txw_irq_save();
    lp_txw_enabled = !strcmp(operation, "on");
    if (!strcmp(operation, "clear")) {
        txw_ring.next = 0;
        txw_ring.used = 0;
        txw_ring.total = 0;
        txw_ring.overwritten = 0;
        txw_sequence = 0;
    }
    txw_irq_restore(irq);

    if (!strcmp(operation, "dump")) {
        uint32_t first = (txw_ring.next + LP_TXW_ROWS - txw_ring.used) % LP_TXW_ROWS;
        printf("[TXW] frozen; timing is perturbed while enabled; total=%lu kept=%lu overwritten=%lu\r\n",
               (unsigned long)txw_ring.total, (unsigned long)txw_ring.used,
               (unsigned long)txw_ring.overwritten);
        for (uint32_t i = 0; i < txw_ring.used; i++) {
            const struct lp_txw_row *row = &txw_rows[(first + i) % LP_TXW_ROWS];
            printf("[TXW] n=%lu rtc=%08lx:%08lx site=%lu d=%ld ac=%lu td=%08lx pck=%lu caller=%08lx\r\n",
                   (unsigned long)row->sequence, (unsigned long)row->rtc_hi,
                   (unsigned long)row->rtc_lo, (unsigned long)row->site,
                   (long)row->delta, (unsigned long)row->ac,
                   (unsigned long)row->txdesc, (unsigned long)row->pck_after,
                   (unsigned long)row->caller);
        }
    } else {
        printf("[TXW] %s enabled=%lu\r\n", operation,
               (unsigned long)lp_txw_enabled);
    }
}
#endif
