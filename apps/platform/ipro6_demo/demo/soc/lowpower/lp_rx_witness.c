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

/* Bounded carrier diagnostics. No allocation, no idle printf, no LPFW writes. */
#if defined(LP_RXW_HOST_TEST) || (defined(CONFIG_WIFI6) && defined(CONFIG_USE_MACSW) && defined(CONFIG_LWIP_LP))
#define LP_RXW_IMPLEMENTATION
#include "lp_rx_witness.h"
#include <stdio.h>
#include <string.h>

#ifdef LP_RXW_HOST_TEST
#define rxw_irq_save test_irq_save
#define rxw_irq_restore test_irq_restore
#define rxw_rtc test_rtc
#else
#include "drv_hbn.h"
#include "system_soc.h"
#define rxw_irq_save __irq_save
#define rxw_irq_restore __irq_restore
#define rxw_rtc HBN_Get_RTC_Timer_Val
#endif

/* Ordinary carrier BSS survives PDS; it is outside the LPFW descriptor arena.
 * Cold boot clears it. The map must be checked on every witness build. */
static struct lp_rxw_row rx_rows[LP_RXW_RX_ROWS];
static struct lp_rxw_row tcp_rows[LP_RXW_TCP_ROWS];
static struct lp_rxw_row resume_rows[LP_RXW_RESUME_ROWS];
struct lp_rxw_ring {
    struct lp_rxw_row *rows;
    uint32_t capacity, next, used, total, overwritten;
};
static struct lp_rxw_ring rx_ring = { .rows = rx_rows, .capacity = LP_RXW_RX_ROWS };
static struct lp_rxw_ring tcp_ring = { .rows = tcp_rows, .capacity = LP_RXW_TCP_ROWS };
static struct lp_rxw_ring resume_ring = { .rows = resume_rows, .capacity = LP_RXW_RESUME_ROWS };
static struct lp_rxw_ring *const rings[] = { &rx_ring, &tcp_ring, &resume_ring };
static uint32_t event_sequence, pds_generation;
volatile uint32_t lp_rxw_enabled;

void lp_rxw_emit(uint32_t stream, uint32_t event, const uint32_t *data,
                 uint32_t count)
{
    if (!lp_rxw_enabled || stream >= sizeof(rings) / sizeof(rings[0]) ||
        count > LP_RXW_WORDS || (count && !data))
        return;

    uint32_t irq = rxw_irq_save();
    if (lp_rxw_enabled) {
        struct lp_rxw_ring *ring = rings[stream];
        struct lp_rxw_row *row = &ring->rows[ring->next];
        if (event == LP_RXW_PDS_ENTER)
            pds_generation++;
        row->sequence = ++event_sequence;
        row->pds = pds_generation;
        rxw_rtc(&row->rtc_lo, &row->rtc_hi);
        row->event = event;
        for (uint32_t i = 0; i < LP_RXW_WORDS; i++)
            row->data[i] = i < count ? data[i] : 0;
        ring->next = (ring->next + 1) % ring->capacity;
        ring->total++;
        if (ring->used < ring->capacity)
            ring->used++;
        else
            ring->overwritten++;
    }
    rxw_irq_restore(irq);
}

void lp_rxw_control(const char *operation)
{
    if (strcmp(operation, "on") && strcmp(operation, "off") &&
        strcmp(operation, "clear") && strcmp(operation, "dump")) {
        printf("[RXW] usage: lp rxw on|off|clear|dump\r\n");
        return;
    }
    uint32_t irq = rxw_irq_save();
    lp_rxw_enabled = !strcmp(operation, "on");
    if (!strcmp(operation, "clear")) {
        for (uint32_t i = 0; i < sizeof(rings) / sizeof(rings[0]); i++) {
            rings[i]->next = rings[i]->used = rings[i]->total = rings[i]->overwritten = 0;
        }
        event_sequence = pds_generation = 0;
    }
    rxw_irq_restore(irq);

    if (!strcmp(operation, "dump")) {
        /* Recording is frozen before printing. UART time is outside the leg. */
        printf("[RXW] v1 witness=" __DATE__ " " __TIME__ " frozen, no RX-to-pbuf token; RTC ticks, data hex\r\n");
        for (uint32_t s = 0; s < sizeof(rings) / sizeof(rings[0]); s++) {
            const struct lp_rxw_ring *ring = rings[s];
            printf("[RXW] ring=%lu total=%lu kept=%lu overwritten=%lu\r\n",
                   (unsigned long)s, (unsigned long)ring->total,
                   (unsigned long)ring->used, (unsigned long)ring->overwritten);
            uint32_t first = (ring->next + ring->capacity - ring->used) % ring->capacity;
            for (uint32_t i = 0; i < ring->used; i++) {
                const struct lp_rxw_row *row = &ring->rows[(first + i) % ring->capacity];
                printf("[RXW] s=%lu n=%lu pds=%lu rtc=%08lx:%08lx e=%lu d=",
                       (unsigned long)s, (unsigned long)row->sequence,
                       (unsigned long)row->pds, (unsigned long)row->rtc_hi,
                       (unsigned long)row->rtc_lo, (unsigned long)row->event);
                for (uint32_t j = 0; j < LP_RXW_WORDS; j++)
                    printf("%s%08lx", j ? "," : "", (unsigned long)row->data[j]);
                printf("\r\n");
            }
        }
    } else {
        printf("[RXW] v1 witness=" __DATE__ " " __TIME__ " %s enabled=%lu\r\n",
               operation, (unsigned long)lp_rxw_enabled);
    }
}
#endif
