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

#ifndef BR_MDNS_H
#define BR_MDNS_H

#include <stdbool.h>
#include <stdint.h>
#include <openthread/instance.h>

/* Called only by the OpenThread task, after the infrastructure link poll. */
void br_mdns_process(otInstance *instance, uint32_t index, bool running);
/* OpenThread stopped for good: free what is queued for it (any task). */
void br_mdns_drop_queued(void);

#endif
