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

#include "FreeRTOS.h"
#include "freertos_app_hooks.h"
#include <ipro_log.h>
#include <stddef.h>

extern void vAssertCalled(const char *file, unsigned long line);
extern void newlibc_fatal_hook(const char *cause, int status, const char *file, int line);

/* BR-only linker wrapper: libc abort otherwise reaches _exit, which deletes
 * the current task even if it owns CHIP/lwIP locks. Keep the first failure
 * observable without walking untrusted stack frames or exposing packet data. */
void __attribute__((noreturn)) __wrap_abort(void)
{
    IPRO_LOGE("MatterBR", "abort caller=%p", __builtin_return_address(0));
    ipro_dump_task_stacks("BR abort");
    if (!xPortIsInsideInterrupt()) newlibc_fatal_hook("abort", 1, NULL, 0); /* OpenThread's own */
    vAssertCalled(__FILE__, __LINE__);
    for (;;) { }
}

/* newlib's assertion uses stderr, which is not a reliable UART diagnostic on
 * this target. Preserve its source location before entering the fatal hook. */
void __attribute__((noreturn)) __wrap___assert_func(const char *file, int line,
                                                  const char *function, const char *condition)
{
    IPRO_LOGE("MatterBR", "assert %s:%d function=%s condition=%s caller=%p",
              file, line, function ? function : "?", condition, __builtin_return_address(0));
    ipro_dump_task_stacks("BR assert");
    if (!xPortIsInsideInterrupt()) newlibc_fatal_hook("assert", 1, file, line); /* OpenThread's own */
    vAssertCalled(file, (unsigned long)line);
    for (;;) { }
}
