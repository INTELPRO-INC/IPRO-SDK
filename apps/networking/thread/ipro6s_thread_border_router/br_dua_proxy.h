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

#ifndef IPRO_BR_DUA_PROXY_H
#define IPRO_BR_DUA_PROXY_H
#include <stdbool.h>
#include <stdint.h>
#include <openthread/instance.h>
struct pbuf;
struct netif;
/* OT task: register before CLI/Thread starts; refresh before routing TX drain. */
void br_dua_refresh(otInstance *instance);
void br_dua_process(otInstance *instance, uint32_t index, bool infra_ready);
/* lwIP core lock held by caller. */
void br_dua_wifi_input(struct pbuf *packet, struct netif *netif);
bool br_dua_can_route(const uint8_t address[16]);
/* OpenThread stopped for good: free what is queued for it (any task). */
void br_dua_drop_queued(void);
#endif
