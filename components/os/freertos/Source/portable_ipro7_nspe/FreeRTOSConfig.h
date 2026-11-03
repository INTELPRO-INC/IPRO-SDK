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
 * FreeRTOS configuration of the IPRO7 NSPE (U-mode under the PSA
 * SPE): kernel options as portable_ipro7, tick from VIRQ_TICK,
 * heap_5 on the PSRAM heap of the NSPE linker script.
 */
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <misc.h>
#include <stdio.h>
#include "compiler/common.h"

#define configFREERTOS_DATA_IN_PSRAM            0

#define configSUPPORT_STATIC_ALLOCATION         1
#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     1
#define configUSE_TICK_HOOK                     0
/* SysTimer rate: NSPE_ECALL_TIMER_SET counts in these ticks */
#define configCPU_CLOCK_HZ                      (4000000UL)
#define configTICK_RATE_HZ                      ((TickType_t)1000)
#define configMAX_PRIORITIES                    (32)
#define configMINIMAL_STACK_SIZE                ((unsigned short)1024)
#define configMAX_TASK_NAME_LEN                 (16)
#define configUSE_TRACE_FACILITY                1
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 0
#define configUSE_MUTEXES                       1
#define configQUEUE_REGISTRY_SIZE               8
#define configCHECK_FOR_STACK_OVERFLOW          2
/* No mstack_* hardware check: configCHECK_FOR_STACK_OVERFLOW only */
#define configHW_STACK_PROTECTION               0
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_MALLOC_FAILED_HOOK            1
#define configUSE_APPLICATION_TASK_TAG          1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_STATS_FORMATTING_FUNCTIONS    2
#define configUSE_TICKLESS_IDLE                 0

/* heap_5 size: the PSRAM heap region (nspe_heap_init) */
extern char __heap_psram_start[];
extern char __heap_psram_end[];
#define configTOTAL_HEAP_SIZE \
    ((size_t)(__heap_psram_end - __heap_psram_start))

/* Run time: CPU cycles / 256 (rdcycle), see nspe_runtime.c */
#define configGENERATE_RUN_TIME_STATS           1
uint32_t nspe_run_time_counter(void);
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() ((void)0)
#define portGET_RUN_TIME_COUNTER_VALUE()         nspe_run_time_counter()

#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         (2)

#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES - 5)
#define configTIMER_QUEUE_LENGTH                16
#ifdef CONFIG_FREERTOS_TIMER_TASK_STACK_WORDS
#define configTIMER_TASK_STACK_DEPTH  CONFIG_FREERTOS_TIMER_TASK_STACK_WORDS
#else
#define configTIMER_TASK_STACK_DEPTH  (configMINIMAL_STACK_SIZE * 4)
#endif

#ifndef uartPRIMARY_PRIORITY
#define uartPRIMARY_PRIORITY                    (configMAX_PRIORITIES - 3)
#endif

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_xTaskGetHandle                  1
#define INCLUDE_xSemaphoreGetMutexHolder        1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetIdleTaskHandle          1
#define INCLUDE_xTaskGetSchedulerState          1

/* Prints file:line with vIRQs masked and spins */
void vAssertCalled(const char *const pcFileName, unsigned long ulLine);
#define configASSERT(x) \
    if ((x) == 0)       \
    vAssertCalled(__FILE__, __LINE__)

#define portUSING_MPU_WRAPPERS 0
#endif /* FREERTOS_CONFIG_H */
