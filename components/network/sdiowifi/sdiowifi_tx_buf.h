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

#pragma once

#include <lwip/pbuf.h>
#include <utils_list.h>

// Default high performance off
#ifndef CONFIG_SDIO_HIGH_PERFORMANCE
#define CONFIG_SDIO_HIGH_PERFORMANCE    (0)
#endif

struct sdiowifi_tx_buf {
    struct utils_list buf_list;
    uint32_t window_size;       // capacity
    uint32_t free_size;
    uint32_t una_seq;
    /* Authoritative pool-exhausted state: set ONLY by a failed alloc under
     * the pool lock, cleared by the first buffer return (tx_buf_free). The
     * intake path reads it lock-free to bail before the alloc ceremony on
     * doomed frames (throughput win under UDP overload) WITHOUT the false
     * positives a raw lock-free free_size==0 read has mid-update — those
     * spurious drops read as congestion to TCP and collapse its cwnd. */
    volatile uint32_t pool_empty;
};

int sdiowifi_tx_buf_init(struct sdiowifi_tx_buf *tb);
int sdiowifi_tx_buf_lpmem_register(struct sdiowifi_tx_buf *tb);
int sdiowifi_tx_buf_mem_register(struct sdiowifi_tx_buf *tb, void *mem, size_t size);
struct pbuf *sdiowifi_tx_buf_alloc(struct sdiowifi_tx_buf *tb);
/* DEBUG 2026-07-23 ZC-probe: pool slot index of a pool pbuf, -1 if foreign. */
int sdiowifi_tx_buf_slot(struct pbuf *p);
void sdiowifi_tx_buf_update_credit(struct sdiowifi_tx_buf *tb);
void sdiowifi_tx_buf_init_done(struct sdiowifi_tx_buf *tb);
bool sdiowifi_tx_buf_is_all_free(struct sdiowifi_tx_buf *tb);

