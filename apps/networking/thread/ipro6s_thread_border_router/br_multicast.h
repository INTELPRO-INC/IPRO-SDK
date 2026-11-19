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

#ifndef BR_MULTICAST_H
#define BR_MULTICAST_H
#include <stdbool.h>
#include <stdint.h>
#include <openthread/instance.h>
#include <openthread/message.h>
struct pbuf;
struct netif;

/* OT task only. Synchronizes MLR subscriptions and drains bounded ingress. */
void br_multicast_process(otInstance *instance, uint32_t index, bool infra_ready);
/* lwIP core only. Copies eligible ingress; never consumes the caller's pbuf. */
void br_multicast_wifi_input(struct pbuf *p, struct netif *netif);
/* OT task only. Does not consume the callback's message. */
void br_multicast_from_thread(otInstance *instance, const otMessage *message);
/* OpenThread stopped for good: free what is queued for it (any task). */
void br_multicast_drop_queued(void);
#endif
