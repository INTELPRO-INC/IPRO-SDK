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

#ifndef APP_NCM_H
#define APP_NCM_H

#include "lwip/netif.h"

/**
 * @brief Initialize USB CDC NCM + lwIP stack
 *
 * Sets up USB device, NCM class driver, and lwIP netif.
 * DHCP is handled automatically when the USB host connects.
 */
void app_ncm_init(void);

/**
 * @brief Get pointer to the NCM netif
 */
struct netif *app_ncm_get_netif(void);

#endif /* APP_NCM_H */
