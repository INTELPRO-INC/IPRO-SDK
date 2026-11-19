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

#include <openthread/config.h>
#include <openthread/platform/logging.h>
#include <openthread_port.h>


void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    va_list argp;

    va_start(argp, aFormat);
    ot_uartLog(aFormat, argp);
    va_end(argp);
}



void otPlatLogLine(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aLogLine)
{
    otPlatLog(aLogLevel, aLogRegion, "%s\r\n", aLogLine);
}
