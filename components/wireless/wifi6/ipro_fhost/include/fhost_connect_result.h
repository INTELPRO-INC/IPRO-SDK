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

#ifndef FHOST_CONNECT_RESULT_H
#define FHOST_CONNECT_RESULT_H

#include <stdint.h>

/* Worker outcomes shared by fhost and the public Wi-Fi manager completion API.
 * These are not IP reachability or Matter commissioning status. */
enum fhost_connect_result
{
    FHOST_CONNECT_PREPARATION_FAILED,
    FHOST_CONNECT_PREPARATION_TIMEOUT,
    FHOST_CONNECT_TASK_FAILED,
    FHOST_CONNECT_FAILED,
    FHOST_CONNECT_CONFIGURED, /* timeout_ms == 0: association was not awaited. */
    FHOST_CONNECT_CONNECTED,  /* WPA state was CONNECTED when worker finished. */
    FHOST_CONNECT_NOT_CONNECTED
};

/* Fixed-width result keeps callbacks independent of -fshort-enums, including
 * future consumers built with Matter's different enum compilation flags. */
typedef void (*fhost_connect_completion_fn)(void *context, int vif, uint32_t result);

#endif
