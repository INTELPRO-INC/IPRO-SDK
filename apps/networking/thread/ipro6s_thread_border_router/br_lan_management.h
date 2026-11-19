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

#ifndef BR_LAN_MANAGEMENT_H
#define BR_LAN_MANAGEMENT_H
#include <stdint.h>
#include <stdbool.h>
#include <openthread/instance.h>
#define BR_LAN_KEY "br-lan-v1"
int br_lan_start(void);
/* Export only through the authenticated L4 BLE management link. */
int br_lan_credentials(uint8_t output[70]);
void br_lan_publish(otInstance *instance, bool running);
#endif
