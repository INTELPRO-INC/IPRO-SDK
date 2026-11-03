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

/*
 * fhost entry points macsw calls in a fully hosted build. The FullMAC
 * firmware has no fhost: the Wi-Fi task is the only macsw task.
 */
#include <stdint.h>

#include "fhost.h"

int rtos_wifi_task_create(void);

const char nx_build_fhost[] = "FMAC\n";

int fhost_init(bool mac_init)
{
    return rtos_wifi_task_create();
}

int fhost_application_init(void)
{
    return 0;
}

void fhost_error_ind(void)
{
}

void fhost_task_ready(int task_id)
{
    (void)task_id;
}

/* LMAC globals that export_func.c holds in the fhost builds */

/* A-MPDU NAV protection, a hw_nav_protection value: 1 = NAV_PROT_SELF_CTS */
uint8_t g_ampdu_protect_enable = 1;

const char nx_build_date[] = __DATE__ " " __TIME__;
