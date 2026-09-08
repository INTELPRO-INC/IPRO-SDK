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

#ifndef FREERTOS_APP_HOOKS_H
#define FREERTOS_APP_HOOKS_H

#include <stdint.h>

/**
 * @brief Setup FreeRTOS heap regions from linker symbols.
 *
 * Collects heap regions (OCRAM + optional PSRAM), sorts by address,
 * and calls vPortDefineHeapRegions(). Safe to call once from main().
 */
void ipro_setup_heap(void);

/**
 * @brief Standard platform init sequence before scheduler start.
 *
 * Calls: platform_init() → ipro_setup_heap() → hal_boot2_init()
 *        → PSRAM check → ipro_log_init() → shell init (if CONFIG_SHELL)
 *
 * After this returns, the app should create its tasks and call
 * vTaskStartScheduler().
 */
void ipro_platform_init(void);

/**
 * @brief Dump every task's stack high-water mark (min free stack ever, bytes).
 *
 * A small free value means the task nearly overflowed and cannot be shrunk; a
 * large free value means the stack is over-provisioned and can be cut. Uses a
 * static buffer + uxTaskGetSystemState (no heap), so it is safe from any
 * context. Requires configUSE_TRACE_FACILITY=1 (else prints nothing).
 *
 * @param where Caller label printed in the header line (may be NULL).
 */
void ipro_dump_task_stacks(const char *where);

/**
 * @brief Diagnostic: verify the IDLE task stack canary is intact.
 *
 * The method-2 stack-overflow check validates the 4 words at the low end of
 * the idle stack (g_idle_stack[0..3] == 0xa5a5a5a5). The idle task only runs
 * __WFI(), so it can never write there itself — if those words are clobbered
 * it is a stray write from elsewhere. Call this with a label at successive
 * boot/init steps to bisect WHICH step corrupts it; the printed value hints at
 * the source (0 = memset bounds bug, pointer = struct/list write, ASCII =
 * buffer overrun). Latched: prints only the FIRST corruption, so it is safe to
 * call in a tight loop.
 *
 * @param tag Label printed on first detected corruption (may be NULL).
 * @return 1 if the canary is corrupted, 0 if intact.
 */
int ipro_check_idle_canary(const char *tag);

#endif /* FREERTOS_APP_HOOKS_H */
