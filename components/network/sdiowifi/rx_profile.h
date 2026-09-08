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

#ifndef RX_PROFILE_H
#define RX_PROFILE_H

#include <stdint.h>

/* Per-frame T0..T5 timestamps spanning SDIO RX -> wifi_eth_tx hand-off.
 *
 *   T0  trcver_sdio sd_read_callback() about to fire
 *   T1  sdio_recv_cb() entry
 *   T2  handle_eth_wifi_frame_recv() entry
 *   T3  sdiowifi_tx_buf_alloc() returned pbuf
 *   T4  pbuf_take() finished payload memcpy
 *   T5  wifi_eth_tx() returned (queued for fhost TX)
 *
 * Only complete frames (T0..T5 all set) are aggregated into stats. */

void rxprof_t0(uint16_t len);
void rxprof_t1(void);
void rxprof_t2(void);
void rxprof_t3(void);
void rxprof_t4(void);
void rxprof_t5(void);

void rxprof_print(void);
void rxprof_reset(void);

#endif
