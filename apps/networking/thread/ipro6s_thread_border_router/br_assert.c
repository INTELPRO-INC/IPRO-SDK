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

/**
 * @file br_assert.c
 * @brief assert() in the BR (non-Matter builds, linked with --wrap=__assert_func).
 *
 * newlib's __assert_func prints to stderr (not a reliable console here) and
 * ends in _exit(), where the fault reaches ot_rcp_fatal() only as "exit" with no
 * location. Give OpenThread's fatal hook the assert's location first; any other
 * task keeps newlib's behaviour.
 */
#include <stdio.h>
#include "FreeRTOS.h"

extern void newlibc_fatal_hook(const char *cause, int status, const char *file, int line);
extern void __real___assert_func(const char *file, int line, const char *function, const char *condition)
    __attribute__((noreturn));

void __attribute__((noreturn)) __wrap___assert_func(const char *file, int line, const char *function,
                                                   const char *condition)
{
    printf("\r\nassert %s:%d %s: %s\r\n", file ? file : "?", line, function ? function : "?",
           condition ? condition : "?");
    if (!xPortIsInsideInterrupt()) {
        newlibc_fatal_hook("assert", 1, file, line);
    }
    __real___assert_func(file, line, function, condition);
}
