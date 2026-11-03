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

/*
 * FreeRTOS port hooks and heap setup. Separate from
 * main.c, which is linked via __set_main_file() and must not also
 * be in the whole-archived `app` library.
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

extern uint32_t __heap_start, __heap_size;

static HeapRegion_t xHeapRegions[] = {
    { (uint8_t *)&__heap_start, (size_t)&__heap_size },
    { NULL, 0 }, /* Terminates the array. */
    { NULL, 0 }  /* Terminates the array. */
};

void setup_heap(void)
{
    printf("Heap Info start \r\n");

    for (int i = 0; i < 4; i++) {
        if (xHeapRegions[i].pucStartAddress == NULL)
            break;
        printf("Heap Info: %zd KB @ [%p ~ %p]\r\n",
               xHeapRegions[i].xSizeInBytes >> 10,
               xHeapRegions[i].pucStartAddress,
               xHeapRegions[i].pucStartAddress + xHeapRegions[i].xSizeInBytes);
    }
    vPortDefineHeapRegions(xHeapRegions);
}

void __attribute__((weak)) vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    puts("Stack Overflow checked\r\n");
    if (pcTaskName) {
        printf("Stack name %s\r\n", pcTaskName);
    }
    while (1) {
        /*empty here*/
    }
}

void __attribute__((weak)) vApplicationMallocFailedHook(void)
{
    printf("Memory Allocate Failed. Current left size is %d bytes\r\n",
           xPortGetFreeHeapSize());
    while (1) {
        /*empty here*/
    }
}

/* FreeRTOS runtime stats - uses RISC-V mcycle CSR */
static volatile unsigned long ulRunTimeCounter = 0;

void vConfigureTimerForRunTimeStats(void)
{
    ulRunTimeCounter = 0;
}

unsigned long ulGetRunTimeCounterValue(void)
{
    unsigned long cycles;
    __asm__ volatile("csrr %0, mcycle" : "=r"(cycles));
    return cycles;
}

void __attribute__((weak)) vApplicationIdleHook(void)
{
    /*empty*/
}

void __attribute__((weak)) vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, configSTACK_DEPTH_TYPE *pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void __attribute__((weak)) vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void user_vAssertCalled(const char *const pcFileName, unsigned long ulLine) __attribute__((weak, alias("vAssertCalled")));
void __attribute__((weak)) vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    printf("Assert raised by task %s, called by %p\r\n", pcTaskGetName(xTaskGetCurrentTaskHandle()), __builtin_return_address(0));
    taskDISABLE_INTERRUPTS();
    while (1)
        ;
}
