/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <misc.h>
#include <stdio.h>
#include "compiler/common.h"

/*-----------------------------------------------------------
 * IPRO7-specific: FreeRTOS scheduler control data placement.
 *
 * PRIVILEGED_DATA (pxCurrentTCB, pxReadyTasksLists, pxDelayedTaskList*,
 * xTickCount, the idle/timer handles, ...) is the kernel's core state.
 * Placing it in PSRAM saves ~1KB OCRAM, BUT on PDS31 deep-sleep apps the
 * PSRAM warm-wake restore (PLL settle + PSRAM clock switch) is the one
 * memory that can come back marginally corrupted on a wake. When the
 * SCHEDULER control state lives there, a single flipped word in
 * pxCurrentTCB / a list link makes the next context restore load a garbage
 * frame -> every GPR == one repeating value, SP wild (0x7c00xxxx), PC jumps
 * to a low invalid address, pxCurrentTCB reads back 0. That is exactly the
 * "hang/Instruction-Page-Fault right after [lp]wk" fingerprint on the Matter
 * contact-sensor (and matches MEMORY ipro7_pds31_silicon_variance).
 *
 * Keep the scheduler control state in OCRAM (full 256KB is HW-retained
 * across PDS31, reliably) so a marginal PSRAM wake cannot collapse the
 * kernel. The big CHIP/OT/BLE C++ runtime stays in PSRAM (it must — it does
 * not fit in OCRAM), but a PSRAM glitch then degrades to a single
 * recoverable/diagnosable task fault instead of a whole-scheduler wild jump.
 * OCRAM budget is ample here (64KB used of 240KB). Force OFF regardless of
 * CONFIG_USE_PSRAM.
 *----------------------------------------------------------*/
#define configFREERTOS_DATA_IN_PSRAM    0

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/
#define configSUPPORT_STATIC_ALLOCATION         1
#define CLINT_CTRL_ADDR                         (0x02000000UL)
#define configCLINT_BASE_ADDRESS                CLINT_CTRL_ADDR
#define configUSE_PREEMPTION                    1
#if defined(CONFIG_FREERTOS_IDLE_HOOK)
#define configUSE_IDLE_HOOK                     1
#else
#define configUSE_IDLE_HOOK                     0
#endif
#define configUSE_TICK_HOOK                     0
#if defined(CONFIG_IPRO7)
#define configSYSTICK_CLOCK_HZ                  (4000000UL)
#elif defined(CONFIG_IPRO6)
#define configSYSTICK_CLOCK_HZ                  (1000000UL)
#else
#error "Unknown platform"
#endif
#define configCPU_CLOCK_HZ                      (1000000UL)
#define configTICK_RATE_HZ                      ((TickType_t)1000)
#define configMAX_PRIORITIES                    (32)
#define configMINIMAL_STACK_SIZE                ((unsigned short)1024) /* Increased for BLE interrupt handlers (RISC-V uses task stack for ISRs). Was 512, increased to 1024 for LE Audio. */
#define configTOTAL_HEAP_SIZE                   ((size_t)56 * 1024)  /* Increased from 48KB to 56KB for ECC support (+8KB) */
#define configMAX_TASK_NAME_LEN                 (16)
#define configUSE_TRACE_FACILITY                1
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 0
#define configUSE_MUTEXES                       1
#define configQUEUE_REGISTRY_SIZE               8
#define configCHECK_FOR_STACK_OVERFLOW          2

/*-----------------------------------------------------------
 * Hardware Stack Protection (mstack_ctrl/bound/base CSRs)
 * Provides instant hardware trap on stack overflow — no polling needed.
 * Requires CFG_HAS_STACK_CHECK enabled in RTL.
 *----------------------------------------------------------*/
#ifndef configHW_STACK_PROTECTION
#define configHW_STACK_PROTECTION               0
#endif
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_MALLOC_FAILED_HOOK            1
#define configUSE_APPLICATION_TASK_TAG          1
#define configUSE_COUNTING_SEMAPHORES           1
#define configGENERATE_RUN_TIME_STATS           1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_STATS_FORMATTING_FUNCTIONS    2
#if defined(CONFIG_FREERTOS_USE_TICKLESS_IDLE)
#define configUSE_TICKLESS_IDLE                 CONFIG_FREERTOS_USE_TICKLESS_IDLE
#else
#define configUSE_TICKLESS_IDLE                 0
#endif

/* Runtime stats require a time base source */
extern void vConfigureTimerForRunTimeStats(void);
extern unsigned long ulGetRunTimeCounterValue(void);
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vConfigureTimerForRunTimeStats()
#define portGET_RUN_TIME_COUNTER_VALUE()         ulGetRunTimeCounterValue()

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES           0
#define configMAX_CO_ROUTINE_PRIORITIES (2)

/* Software timer definitions. */
#define configUSE_TIMERS             1
/* Timer service task must sit BELOW the BLE controller (MAX-1) and the
 * BT HCI host threads (TX MAX-3, RX MAX-4). Previously at MAX-1 the
 * timer service rounded-robin with the BLE controller — when a long
 * xTimer callback fired (e.g. ESL stats dump, EPD render kick, OT alarm
 * processing) the controller's BLE_INTSTAT and HCI event servicing
 * could be deferred for ms, causing TMAFS underruns and HCI 0xff
 * timeouts. MAX-5 keeps timers above the app task (MAX-6) and shell
 * (20) but well below the host stack. */
#define configTIMER_TASK_PRIORITY    (configMAX_PRIORITIES - 5)
#define configTIMER_QUEUE_LENGTH     16  /* Increased for OpenThread alarm operations */
/* RESTORED to *4 (2026-06-02): CHIP runs its System::Layer / MRP-retransmit /
 * exchange timer callbacks in the FreeRTOS timer-service task. On the downlink
 * command path those callbacks recurse deep into CHIP C++ (TLV, SecureChannel,
 * even C++ exception unwinding _Unwind_Find_FDE) — far deeper than the bare-poll
 * idle path. The *1 (~4KB) value, trimmed for the no-PSRAM 64KB-retention
 * experiment, OVERFLOWS the timer-task stack during command/MRP processing and
 * clobbers adjacent memory -> the mutating command-path crashes (crash RAs
 * landed in prvProcessTimerOrBlockTask + _Unwind_Find_FDE). Pure-idle poll
 * wakes never trigger those deep callbacks, which is why idle PDS ran 115+
 * cycles clean. The PSRAM build has ample heap, so restore the original *4
 * headroom. (Same class of bug as the s_pds_stack 5K->8K fix.) */
#define configTIMER_TASK_STACK_DEPTH (configMINIMAL_STACK_SIZE * 4)

/* Task priorities.  Allow these to be overridden. */
#ifndef uartPRIMARY_PRIORITY
#define uartPRIMARY_PRIORITY (configMAX_PRIORITIES - 3)
#endif

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       1
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_eTaskGetState               1
#define INCLUDE_xTimerPendFunctionCall      1
#define INCLUDE_xTaskAbortDelay             1
#define INCLUDE_xTaskGetHandle              1
#define INCLUDE_xSemaphoreGetMutexHolder    1
#define INCLUDE_uxTaskGetStackHighWaterMark 1

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
void vAssertCalled(const char *const pcFileName, unsigned long ulLine);
#define configASSERT(x) \
    if ((x) == 0)       \
    vAssertCalled(__FILE__, __LINE__)

/* Mode 1 uses the IPRO7 port's built-in core-retentive WFI implementation.
 * Mode 2 is reserved for applications which provide a platform sleep hook
 * (normally PDS).  Routing mode 1 through vApplicationSleep() made the port's
 * safe WFI-only implementation unreachable and also caused a link error for
 * applications that correctly selected mode 1 without defining a PDS hook. */
#if (configUSE_TICKLESS_IDLE == 2)
void vApplicationSleep(uint32_t xExpectedIdleTime);
#define portSUPPRESS_TICKS_AND_SLEEP(xExpectedIdleTime) vApplicationSleep(xExpectedIdleTime)
#endif

#define portUSING_MPU_WRAPPERS 0
#endif /* FREERTOS_CONFIG_H */
