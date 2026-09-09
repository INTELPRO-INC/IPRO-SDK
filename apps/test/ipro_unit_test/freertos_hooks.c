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
 * @file freertos_hooks.c
 * @brief Required FreeRTOS hook functions for unit test project
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief FreeRTOS assertion handler
 */
void vAssertCalled(const char *file, uint32_t line)
{
    taskDISABLE_INTERRUPTS();
    printf("ASSERTION FAILED: %s:%u\n", file ? file : "unknown", line);
    for (;;);
}

/**
 * @brief Stack overflow hook
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    printf("STACK OVERFLOW: Task %s\n", pcTaskName ? pcTaskName : "unknown");
    for (;;);
}

/**
 * @brief Malloc failed hook
 */
void vApplicationMallocFailedHook(void)
{
    printf("MALLOC FAILED: Out of heap memory\n");
    for (;;);
}

/**
 * @brief Get idle task memory (for static allocation)
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
    
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/**
 * @brief Get timer task memory (for static allocation)
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize)
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
    
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/**
 * @brief Configure timer for run-time stats
 */
void vConfigureTimerForRunTimeStats(void)
{
    /* Optional: Configure hardware timer for runtime statistics */
}

/**
 * @brief Get runtime counter value
 */
unsigned long ulGetRunTimeCounterValue(void)
{
    /* Optional: Return hardware timer value for runtime statistics */
    return 0;
}
