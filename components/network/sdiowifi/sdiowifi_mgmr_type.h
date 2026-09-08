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

#ifndef __SDIOWIFI_MGMR_TYPE_H__
#define __SDIOWIFI_MGMR_TYPE_H__

#include <sdiowifi_securec_wrap.h>
#include <stdbool.h>
#include <string.h>
#include <sdiowifi_platform_adapt.h>
#include <lwip/netif.h>
//#include <bl_wifi.h>
#include <wifi_mgmr_ext.h>
//#include <vfs.h>

#include <rnm_server.h>

#include "net_wifi_transceiver.h"
#include <sdiowifi_config.h>

typedef struct sdiowifi_mgmr {
    bool init;

    rnms_t *rnm;

    net_wifi_trcver_t trcver;

    struct sdiowifi_tx_buf tx_desc;
    sdiowifi_timer_handle_t ping_timer;
} sdiowifi_mgmr_t;

#endif /* __SDIOWIFI_MGMR_H__ */
