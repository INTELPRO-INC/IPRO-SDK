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

#ifndef IPRO_FTD_UDP_LOAD_WIRE_H
#define IPRO_FTD_UDP_LOAD_WIRE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UDP_LOAD_PACKET_SIZE 128u

/* IPU1 network-byte-order payload, shared with tools/tests/thread_udp_load.py.
 * Return 0 on success. Failure leaves destination/output values unchanged.
 * No allocation, OpenThread, RTOS, or clock dependency. */
int udp_load_encode(uint8_t *destination, size_t capacity,
                    uint64_t run_id, uint64_t sequence, uint64_t sent_ns);
int udp_load_decode(const uint8_t *source, size_t length,
                    uint64_t *run_id, uint64_t *sequence, uint64_t *sent_ns);

#ifdef __cplusplus
}
#endif
#endif
