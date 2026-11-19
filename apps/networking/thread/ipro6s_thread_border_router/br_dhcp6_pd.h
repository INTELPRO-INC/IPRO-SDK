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
#include <openthread/instance.h>

/* Retry transport readiness and drain bounded RX on the OpenThread owner only. */
void br_dhcp6_pd_process(otInstance *instance);
/* OpenThread stopped for good: stop listening and free what is queued for it.
 * Any task; takes the lwIP core lock. */
void br_dhcp6_pd_stop(void);
