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

/**
 * @file sentino_lifecycle.c
 * @brief Device lifecycle management — info, time, bind, reset
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "sentino_lifecycle.h"
#include "app_sentino.h"

#include "app_log.h"

#define LOG_TAG "LIFECYCLE"

int sentino_lifecycle_info_report(void)
{
    IPRO_LOGI(LOG_TAG, "Sending info report");
    return app_sentino_report("info",
        "\"bindStatus\":1,"
        "\"version\":\"1.0.0\","
        "\"checkPwd\":0",
        1);
}

int sentino_lifecycle_time_sync(void)
{
    IPRO_LOGI(LOG_TAG, "Requesting server time");
    return app_sentino_report("time", NULL, 1);
}

int sentino_lifecycle_bind(void)
{
    /* Handled in main.c with userId/assetId from BLE prov */
    return 0;
}

void sentino_lifecycle_reset(void)
{
#ifdef CONFIG_BLUETOOTH_LE_ENABLE
    extern void app_ble_prov_clear_bind(void);
    app_ble_prov_clear_bind();
#endif
    IPRO_LOGI(LOG_TAG, "Reset: bind state cleared, rebooting...");

    extern void hal_reboot(void);
    hal_reboot();
}
