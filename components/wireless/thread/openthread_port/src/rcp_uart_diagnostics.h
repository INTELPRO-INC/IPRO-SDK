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

#ifndef IPRO_RCP_UART_DIAGNOSTICS_H
#define IPRO_RCP_UART_DIAGNOSTICS_H
#include <stdint.h>
/* Boot-lifetime counters modulo 2^32. Snapshot under the OT lock with IRQs
 * masked; poll at least every five minutes and accumulate unsigned deltas.
 * TX counts submissions, not confirmed delivery. Decode counts input bytes,
 * not accepted HDLC frames. No payload or credentials are retained. */
typedef struct {
    volatile uint32_t read_bytes;
    volatile uint32_t read_invalid;
    volatile uint32_t queued_bytes;
    volatile uint32_t drop_bytes;
    volatile uint32_t overflow_events;
    volatile uint32_t decode_bytes;
    volatile uint32_t tx_bytes;
    volatile uint32_t tx_calls;
    volatile uint32_t tx_done;
} RcpUartDiagnostics;
#endif
