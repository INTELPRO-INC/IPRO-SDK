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

#ifndef IPRO_BR_THREAD_NETIF_H
#define IPRO_BR_THREAD_NETIF_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <openthread/instance.h>
bool br_thread_netif_process(otInstance *instance);
/* OpenThread stopped for good: free the packets queued for Thread (any task). */
void br_thread_netif_drop_queued(void);
struct netif;
/* Static-lifetime netif; caller holds the lwIP core lock. NULL before init. */
struct netif *br_thread_netif_get(void);
/* Caller holds lwIP core lock; record only successfully transmitted RAs. */
bool br_thread_record_ra(const uint8_t *ra, size_t length);
#endif
