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

#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include "FreeRTOS_POSIX.h"
#include <time.h>
#include <sys/_timeval.h>

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME     1     /**< The identifier of the system-wide clock measuring real time. */
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC    4     /**< The identifier for the system-wide monotonic clock.*/
#endif

struct timezone {
  int tz_minuteswest; /* Minutes west of GMT.  */
  int tz_dsttime;     /* Nonzero if DST is ever in effect.  */
};

int gettimeofday(struct timeval *tv, struct timezone *tz);
int settimeofday(struct timeval *tv, struct timezone *tz);

// in secondes
void setTimezone(int tz);
// in seconds
int getTimezone(void);


#define timersub(tvp, uvp, vvp)                                         \
        do {                                                            \
                (vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;          \
                (vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;       \
                if ((vvp)->tv_usec < 0) {                               \
                        (vvp)->tv_sec--;                                \
                        (vvp)->tv_usec += 1000000;                      \
                }                                                       \
        } while (0)

#endif
