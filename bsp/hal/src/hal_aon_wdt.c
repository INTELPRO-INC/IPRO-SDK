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

#include "hal_aon_wdt.h"
#include "drv_hbn.h"

/*
 * The HBN AON-WDT driver (HBN_AON_WDT_*) only exists on SoCs that advertise
 * CONFIG_AON_WDT_SUPPORT (ipro7, ipro6le). On SoCs without it (ipro6) these
 * wrappers stay defined as no-ops so callers — notably platform_common.c's
 * aon_wdt_disable() in the !CONFIG_AON_WDT_ENABLE path — still link.
 */

void aon_wdt_init(uint8_t mask_bit)
{
#if defined(CONFIG_AON_WDT_SUPPORT)
    HBN_AON_WDT_Init(mask_bit);
#else
    (void)mask_bit;
#endif
}

void aon_wdt_feed(void)
{
#if defined(CONFIG_AON_WDT_SUPPORT)
    HBN_AON_WDT_Feed();
#endif
}

void aon_wdt_disable(void)
{
#if defined(CONFIG_AON_WDT_SUPPORT)
    HBN_AON_WDT_Disable();
#endif
}

int aon_wdt_get_reset_status(void)
{
#if defined(CONFIG_AON_WDT_SUPPORT)
    return HBN_AON_WDT_Get_Reset_Status();
#else
    return 0;
#endif
}

void aon_wdt_clear_reset_status(void)
{
#if defined(CONFIG_AON_WDT_SUPPORT)
    HBN_AON_WDT_Clear_Reset_Status();
#endif
}
