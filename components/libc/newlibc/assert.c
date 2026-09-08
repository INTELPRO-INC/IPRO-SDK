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
#include <assert.h>

#include <FreeRTOS.h>
#include <task.h>

/* for exit() and abort() */
void __attribute__ ((noreturn))
__assert (const char *file, int line, const char *cond)
{
    printf("Assertion Failed: %s, in %s:%d\r\n", cond, file, line);
    configASSERT(0);
    while(1);
}

void __attribute__ ((noreturn))
__assert_func (const char *file, int line, const char *func, const char *cond)
{
    printf("Assertion Failed: %s, in %s %s:%d\r\n", cond, func, file, line);
    configASSERT(0);
    while(1);
}

void __attribute__ ((noreturn))
abort(void) {
    configASSERT(0);
    while(1);
}
