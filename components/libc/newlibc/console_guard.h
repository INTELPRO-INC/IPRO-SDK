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

#ifndef NEWLIBC_CONSOLE_GUARD_H
#define NEWLIBC_CONSOLE_GUARD_H
#include <stdbool.h>
#include <stdint.h>

enum console_guard_token {
    CONSOLE_GUARD_DROP = 0,
    CONSOLE_GUARD_NONBLOCK,
    CONSOLE_GUARD_LOCKED,
};

/* BR-only: initialize once before starting the scheduler. No heap allocation. */
bool newlibc_console_init(void);
/* Tasks wait only in a schedulable context; ISR/early/critical callers try once.
 * Pair each successful enter with leave from the same context. No logging here. */
enum console_guard_token newlibc_console_enter(void);
void newlibc_console_leave(enum console_guard_token token);
uint32_t newlibc_console_drops(void);
#endif
