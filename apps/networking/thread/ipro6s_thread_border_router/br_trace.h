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

#ifndef BR_TRACE_H
#define BR_TRACE_H
#include <stddef.h>
#include <stdint.h>
struct pbuf;
enum br_trace_stage { BR_TRACE_WIFI_IN, BR_TRACE_QUEUED, BR_TRACE_OT_SEND,
                      BR_TRACE_OT_RX, BR_TRACE_ROUTE_DROP, BR_TRACE_QUEUE_DROP,
                      BR_TRACE_ALLOC_DROP, BR_TRACE_WIFI_TX };
/* Stage 5 is queue-full only; stage 6 is pbuf clone failure. Schema 5 adds
 * stage 7 at the lwIP-to-Wi-Fi admission boundary and keeps prior IDs stable. */
void br_trace_bytes(enum br_trace_stage stage, const uint8_t *header, size_t length, int status);
void br_trace_pbuf(enum br_trace_stage stage, const struct pbuf *p, int status);
#endif
