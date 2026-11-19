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



#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include <openthread-core-config.h>
#include <openthread/config.h>

#include <openthread/platform/debug_uart.h>
#include <openthread/platform/logging.h>
#include <openthread_port.h>



void otSysProcessDrivers(otInstance *aInstance) 
{
    ot_system_event_t sevent = OT_SYSTEM_EVENT_NONE;

    OT_GET_NOTIFY(sevent);
    ot_alarmTask(sevent);
    ot_uartTask(sevent);
    ot_radioTask(sevent);
}

