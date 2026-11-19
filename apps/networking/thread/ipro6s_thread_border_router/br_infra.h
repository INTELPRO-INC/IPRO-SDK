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

#ifndef IPRO_BR_INFRA_H
#define IPRO_BR_INFRA_H
#include <openthread/instance.h>
int br_infra_prepare(void);
void br_infra_process(otInstance *instance, bool forwarding_ready);
/* OpenThread stopped for good: withdraw the routes of its last RA from the
 * LAN (router lifetime 0, RIO lifetimes 0; sent 3 times, 4 s apart). Returns
 * the number of routes in the first send, 0 if it failed. Any task. */
unsigned br_infra_withdraw_routes(void);
/* OpenThread stopped for good: free the LAN ND packets queued for it. */
void br_infra_drop_queued(void);
#endif
