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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <openthread/platform/misc.h>


void otPlatReset(otInstance *aInstance)
{
	/* TODO: Implement actual platform reset */
}


otPlatResetReason otPlatGetResetReason(otInstance *aInstance)
{

    // BL_RST_REASON_E rstinfo = bl_sys_rstinfo_get();

    // switch (rstinfo) {
    //     case BL_RST_WDT:
    //     return OT_PLAT_RESET_REASON_WATCHDOG;
    //     case BL_RST_BOR:
    //     return OT_PLAT_RESET_REASON_OTHER;
    //     case BL_RST_HBN:
    //     return OT_PLAT_RESET_REASON_EXTERNAL;
    //     case BL_RST_POR:
    //     return OT_PLAT_RESET_REASON_EXTERNAL;
    //     case BL_RST_SOFTWARE:
    //     return OT_PLAT_RESET_REASON_SOFTWARE;
    //     default:
    //     return OT_PLAT_RESET_REASON_UNKNOWN;
    // }
}

void otPlatAssertFail(const char *aFilename, int aLineNumber)
{
    /* Handled by ot_stub.c otPlatAssertFail */
}

void otPlatWakeHost(void)
{}

otError otPlatSetMcuPowerState(otInstance *aInstance, otPlatMcuPowerState aState)
{
    return OT_ERROR_NONE;
}
otPlatMcuPowerState otPlatGetMcuPowerState(otInstance *aInstance)
{
    return OT_PLAT_MCU_POWER_STATE_ON;
}