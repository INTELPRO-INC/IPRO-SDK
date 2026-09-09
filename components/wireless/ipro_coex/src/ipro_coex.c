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

#include <ipro_coex.h>

#include "ipro_coex_impl.h"

int ipro_coex_dump_all(void)
{
    return ipro_coex_dump_all_impl();
}

int ipro_coex_init_default(void)
{
    return ipro_coex_init_default_impl();
}

int ipro_coex_force_wlan(void)
{
    return ipro_coex_force_wlan_impl();
}

int ipro_coex_force_bt(void)
{
    return ipro_coex_force_bt_impl();
}

int ipro_coex_force_mode(enum IPRO_COEX_FORCE_MODE mode)
{
    switch (mode) {
    case IPRO_COEX_FORCE_MODE_PTA_FORCE:
    case IPRO_COEX_FORCE_MODE_PTI_DEFAULT_FORCE:
    case IPRO_COEX_FORCE_MODE_PTI_PRIORITY_FORCE:
    case IPRO_COEX_FORCE_MODE_PTI_PRIORITY_FORCE2:
        return ipro_coex_force_mode_impl(mode);
    default:
        return -1;
    }
}

int ipro_coex_event_notify(enum IPRO_COEX_EVENT event, void *event_arg)
{
    return ipro_coex_event_handler_impl(event, event_arg);
}
