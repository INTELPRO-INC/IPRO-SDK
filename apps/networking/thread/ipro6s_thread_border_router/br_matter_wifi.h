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
#include <stdbool.h>

/* Call once after tcpip_init has completed, from a normal task with no core or
 * critical lock held. Repeated calls do not reset revision/history/policy. */
int br_matter_wifi_init(void);
/* Explicit BR application station policy, not association or PHY mode bits.
 * Call with true when the existing station owner is enabled, false on shutdown.
 * Neither function initializes/connects/reconfigures Wi-Fi. */
int br_matter_wifi_set_enabled(bool enabled);
