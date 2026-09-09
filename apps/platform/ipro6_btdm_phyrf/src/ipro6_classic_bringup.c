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

#include <generated/autoconf.h>

#if defined(CONFIG_IPRO6_CLASSIC_BRINGUP)

#include <stdbool.h>
#include <stdio.h>

#include <bluetooth.h>

static void ipro6s_classic_ready(int err)
{
    if (err != 0) {
        printf("[IPRO6_CLASSIC] bt_enable failed: %d\r\n", err);
        return;
    }

    err = bt_br_set_connectable(true);
    printf("[IPRO6_CLASSIC] connectable rc=%d\r\n", err);
    if (err != 0)
        return;

    err = bt_br_set_discoverable(true);
    printf("[IPRO6_CLASSIC] discoverable rc=%d\r\n", err);
    if (err == 0)
        printf("[IPRO6_CLASSIC] READY for PTS inquiry/page\r\n");
}

void ipro6s_classic_bringup_start(void)
{
    int err = bt_enable(ipro6s_classic_ready);

    if (err != 0)
        printf("[IPRO6_CLASSIC] bt_enable start failed: %d\r\n", err);
}

#endif
