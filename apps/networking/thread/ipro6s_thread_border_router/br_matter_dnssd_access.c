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

#include "ipro_matter_dnssd.h"
#include "openthread_port.h"

otError ipro_matter_dnssd_acquire(otInstance **instance)
{
    if (!instance) return OT_ERROR_INVALID_ARGS;
    *instance = NULL;
    otInstance *ready = otrGetInstanceIfReady();
    if (!ready) return OT_ERROR_INVALID_STATE;
    if (!otrTryLock()) return OT_ERROR_BUSY;
    *instance = ready;
    return OT_ERROR_NONE;
}

void ipro_matter_dnssd_release(void)
{
    otrUnlock();
}
