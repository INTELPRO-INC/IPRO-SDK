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

/* host stub: Nuclei SysTimer (MTIME) */
#ifndef HOST_CORE_FEATURE_TIMER_H
#define HOST_CORE_FEATURE_TIMER_H
#include <stdint.h>
extern uint64_t host_mtime;
extern int host_mtime_stopped;
/* a test can make time pass on every read (spin loops) */
extern uint32_t host_mtime_autostep;
static inline uint64_t SysTimer_GetLoadValue(void)
{
    uint64_t v = host_mtime;

    if (!host_mtime_stopped) {
        host_mtime += host_mtime_autostep;
    }
    return v;
}
static inline void SysTimer_SetLoadValue(uint64_t v) { host_mtime = v; }
static inline void SysTimer_Start(void) { host_mtime_stopped = 0; }
static inline void SysTimer_Stop(void) { host_mtime_stopped = 1; }
static inline uint32_t SysTimer_GetControlValue(void) { return (uint32_t)host_mtime_stopped; }
#endif
