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


#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <ipro_log.h>
#include <openthread-core-config.h>
#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/ncp.h>
#include <openthread/tasklet.h>
#include <semphr.h>
#include <task.h>
#include <timers.h>

#include "openthread_port.h"
#if defined(CONFIG_IPRO_PM_THREAD)
#include "ot_pm.h"
#endif

#include <mbedtls/platform.h>

#ifdef CONFIG_IEEE802154_ENABLE
#include "ieee802154.h"
#endif

ot_system_event_t ot_system_event_var = OT_SYSTEM_EVENT_NONE;
static SemaphoreHandle_t ot_extLock = NULL;
static otInstance *ot_instance = NULL;
static TaskHandle_t ot_taskHandle = NULL;
static volatile bool ot_initialized = false;
/* Set once by a fatal error (otrMarkStopped); never cleared until reboot. */
static volatile bool ot_stopped = false;
/* Reserved under the task critical section before static storage is touched.
 * This is a start request, not readiness; otrGetInstance checks readiness. */
static bool ot_start_requested = false;

/* OT task buffers in .ocram_bss → 64 KB retention region (banks 0-3).
 * Under CONFIG_OCRAM_NORET_SPLIT, regular .bss goes to ocram_noret
 * which is wiped on every PDS31 wake. Without explicit pinning, the
 * OT task's blocked stack frame is corrupted and the scheduler resumes
 * into garbage. */
static __attribute__((section(".ocram_bss")))
    StackType_t g_ot_stack[OT_TASK_SIZE];
static __attribute__((section(".ocram_bss"))) StaticTask_t g_ot_tcb;
static __attribute__((section(".ocram_bss"))) StaticQueue_t g_ot_lock;

/* Diagnostic counter — how many OT-task wakeups had `notified > 0`
 * (i.e. were triggered by an explicit OT_NOTIFY from alarm.c / radio
 * IRQ / shell / etc., as opposed to the safety heartbeat timeout).
 * Lets us tell whether OT housekeeping is running because of real
 * events or just the timeout heartbeat. */
volatile uint32_t g_ot_notified_wake_count;
volatile uint32_t g_ot_timeout_wake_count;

/* Safety-net heartbeat of the OT task (see the while-loop comment):
 * CONFIG_OT_HEARTBEAT_MS, 1500 ms by default.  Every OT event notifies the
 * task, so the heartbeat only bounds a missed notification; each one is an
 * RTOS deadline that ends a deep-sleep window, so low-power builds use a long
 * one (0 = none).  ot_freertos_set_heartbeat_ms() changes it at run time. */
#ifdef CONFIG_OT_HEARTBEAT_MS
#define OT_HEARTBEAT_MS_DEFAULT ((uint32_t)CONFIG_OT_HEARTBEAT_MS)
#else
#define OT_HEARTBEAT_MS_DEFAULT 1500u
#endif
static volatile uint32_t g_ot_heartbeat_ms = OT_HEARTBEAT_MS_DEFAULT;

uint32_t ot_freertos_get_heartbeat_ms(void) {
    return g_ot_heartbeat_ms;
}

void ot_freertos_set_heartbeat_ms(uint32_t ms) {
    if ((ms != 0u) && (ms < 100u)) ms = 100u; /* don't busy-spin */
    if (ms > 600000u) ms = 600000u;           /* cap at 10 min */
    g_ot_heartbeat_ms = ms;
}

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
#define ENDPOINT_CT OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_NUM
otInstance *instances[ENDPOINT_CT] = {NULL};

#elif OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
size_t otInstanceBufferLength = 0;
uint8_t *otInstanceBuffer = NULL;
#endif

__attribute__((weak)) void otrAppProcess(ot_system_event_t sevent) {}

void otTaskletsSignalPending(otInstance *aInstance) {
    if (aInstance) {
        OT_NOTIFY(OT_SYSTEM_EVENT_OT_TASKLET);
    }
}

otInstance *otrGetInstanceIfReady(void) {
    if (xPortIsInsideInterrupt() || xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        return NULL;
    }
    taskENTER_CRITICAL();
    otInstance *instance = (ot_initialized && !ot_stopped) ? ot_instance : NULL;
    taskEXIT_CRITICAL();
    return instance;
}

void otrMarkStopped(void) {
    ot_stopped = true;
}

bool otrIsStopped(void) {
    return ot_stopped;
}

otInstance *otrGetInstance(void) {
    if (xPortIsInsideInterrupt()) {
        return NULL;
    }
    // Five-second polling delay budget; scheduler delays may extend wall time.
    int timeout = 5000;
    while (!ot_initialized && !ot_stopped && timeout > 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
        timeout -= 10;
    }
    /* Allocation precedes radio/user initialization. A timeout must not
     * expose that partially initialized instance to Matter or other users. */
    return (ot_initialized && !ot_stopped) ? ot_instance : NULL;
}

void otSysProcessDrivers(otInstance *aInstance) {
    ot_system_event_t sevent = OT_SYSTEM_EVENT_NONE;

    OT_GET_NOTIFY(sevent);

    ot_alarmTask(sevent);
    ot_uartTask(sevent);
    ot_radioTask(sevent);
    otrAppProcess(sevent);
}

void otSysEventSignalPending(void) {
    if (ot_taskHandle == NULL) {
        IPRO_LOGE("OT-FreeRTOS",
                  "Task handle is NULL, cannot send notification");
        return;
    }

    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR(ot_taskHandle, &pxHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    } else {
        xTaskNotifyGive(ot_taskHandle);
    }
}

bool otrTryLock(void) {
    /* Same nonrecursive mutex as the SDK worker; never block or use the
     * task-only semaphore API in interrupt context. Readiness is separate. */
    if (xPortIsInsideInterrupt() || !ot_extLock) {
        return false;
    }
    return xSemaphoreTake(ot_extLock, 0) == pdTRUE;
}

bool otrIsLockOwner(void) {
    return !xPortIsInsideInterrupt() && ot_extLock != NULL &&
           xSemaphoreGetMutexHolder(ot_extLock) == xTaskGetCurrentTaskHandle();
}

bool otrIsCurrentThread(void) {
    return !xPortIsInsideInterrupt() && ot_taskHandle != NULL &&
           xTaskGetCurrentTaskHandle() == ot_taskHandle;
}

void otrLock(void) {
    if (!ot_extLock) {
        return;
    }
    if (xSemaphoreTake(ot_extLock, pdMS_TO_TICKS(3000)) == pdTRUE) {
        return;
    }
    /* Stalled >3 s waiting for ot_extLock. This is the fingerprint of the
     * silent "hang after a downlink command following a PDS wake": the CHIP
     * event-loop task blocks here inside SendPreparedMessage->otUdpSend while
     * the OpenThread threadTask strands ot_extLock (workflow root-cause, the
     * ONLY infinite-block in the Msg-TX -> Retransmission-scheduled window).
     * There is NO CPU trap for a deadlock, so the existing exception handler
     * never runs — dump the lock holder + both task states + radio TX
     * counters ONCE so the next board run pins the variant without guessing:
     *   caller=CHIP,       holder=threadTask  -> OT task wedged holding lock
     *   caller==holder (same task)            -> self-nested otrLock deadlock
     *   caller=threadTask, holder=CHIP(running)-> CHIP wedged inside otUdpSend
     * eTaskState: 0=running 1=ready 2=blocked 3=suspended 4=deleted 5=invalid.
     * SANE states => a real SW deadlock (fixable here). GARBAGE states / a
     * holder in an impossible state => wake-side memory corruption
     * (see MEMORY ipro7_pds31_silicon_variance) — swap IC before chasing SW. */
    {
        TaskHandle_t self   = xTaskGetCurrentTaskHandle();
        TaskHandle_t holder = xSemaphoreGetMutexHolder(ot_extLock);
        printf("\r\n[OTLOCK-STALL] >3s waiting for ot_extLock\r\n");
        printf("  caller=%s state=%d\r\n",
               self ? pcTaskGetName(self) : "?",
               self ? (int)eTaskGetState(self) : -1);
        printf("  holder=%s state=%d\r\n",
               holder ? pcTaskGetName(holder) : "<none/free>",
               holder ? (int)eTaskGetState(holder) : -1);
        if (ot_taskHandle) {
            printf("  threadTask state=%d hwm=%lu words\r\n",
                   (int)eTaskGetState(ot_taskHandle),
                   (unsigned long)uxTaskGetStackHighWaterMark(ot_taskHandle));
        }
        extern void ot_radio_dbg_dump(void);
        ot_radio_dbg_dump();
    }
    /* Behavior otherwise unchanged: keep waiting for the lock to free. */
    xSemaphoreTake(ot_extLock, portMAX_DELAY);
}

void otrUnlock(void) {
    if (ot_extLock) {
        xSemaphoreGive(ot_extLock);
    }
}

void otrStackInit(void) {
#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    for (int i = 0; i < ENDPOINT_CT; i++) {
        instances[i] = otInstanceInitMultiple(i);

        assert(instances[i]);
    }
    ot_instance = instances[0];
#elif OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

    // Call to query the buffer size
    (void)otInstanceInit(NULL, &otInstanceBufferLength);

    // Call to allocate the buffer
    otInstanceBuffer = (uint8_t *)malloc(otInstanceBufferLength);
    assert(otInstanceBuffer);

    // Initialize OpenThread with the buffer
    ot_instance = otInstanceInit(otInstanceBuffer, &otInstanceBufferLength);

#else
    ot_instance = otInstanceInitSingle();
#endif
    assert(ot_instance);
}

static void otrStackTask(void *aContext) {
    /** This task is an example to handle both main event loop of openthread
     * task lets and hardware drivers for openthread, such as radio, alarm timer
     * and also uart shell. Customer can implement own task for both of two
     * these missions with other privoded APIs.  */

    IPRO_LOGI("OT-FreeRTOS", "OpenThread task started");
    OT_THREAD_SAFE(
        // Enable the SEC_ENG hardware TRNG before OpenThread/mbedTLS can draw
        // entropy. IPRO7 system_soc.c does NOT auto-enable it, and the
        // compiled entropy source random.c -> hal_rand_stream()
        // holds taskENTER_CRITICAL() (IRQs OFF) across the TRNG read. Without
        // this, the first entropy draw that crosses the 32-byte refill boundary
        // (the DTLS/EC-JPAKE Joiner handshake) spins Sec_Eng_Trng_Read for
        // 16M iterations with interrupts off -> tick-dead whole-system hang.
        // Mirrors a working third-party port's ot_entropy_init() and the
        // fix already shipped in apps/matter/contact-sensor/ipro7/main.c.
        extern int hal_sec_trng_init(void);
        (void)hal_sec_trng_init();
        // Initialize platform layers before creating OpenThread instance
        ot_alarmInit(); // Must initialize alarm BEFORE otInstanceInitSingle()
        ot_radioInit(ot_instance);
        otrStackInit(); // This will create OpenThread instance which needs
                        // alarm
        otrInitUser(ot_instance);
#if defined(CONFIG_IPRO_PM_THREAD)
        ot_pm_init(ot_instance); // platform PDS between radio activity (ipro_pm)
#endif
        ot_initialized = true; // Signal initialization complete
        IPRO_LOGI("OT-FreeRTOS", "OpenThread initialized"););
    // Event-driven loop: every OT event (alarm, radio IRQ, tasklet, CLI)
    // notifies this task.  The heartbeat (CONFIG_OT_HEARTBEAT_MS) is only a
    // safety net for a missed notification; a short one bounds every idle
    // window, so it is long (or off) in low-power builds.
    /* Forward decl: defined in ot_radio_ieee802154.c. Returns true and
     * runs ieee802154_reset() if the LP wake hook flagged a pending
     * reinit; otherwise no-op. Must run before otTaskletsProcess
     * because tasklet handlers can immediately invoke radio APIs
     * (otPlatRadioReceive / otPlatRadioTransmit) and would hit a
     * post-PDS POR M154 if the reset hadn't run yet. */
    extern bool ot_radio_drain_pending_reinit(void);

    while (true) {
        uint32_t heartbeat_ms = ot_freertos_get_heartbeat_ms();
        uint32_t notified  = 0;
        {
            /* Notifications wake this owner; they are not per-packet tokens.
             * Drivers drain durable rings/queues and OT_GET_NOTIFY consumes
             * the complete event-bit snapshot. Keeping old counted credits
             * would run high-priority empty passes after that work is drained,
             * starving lower-priority tasks. Coalesce only at the wait: signals
             * arriving during processing must remain for the next iteration. */
            notified = ulTaskNotifyTake(pdTRUE, (heartbeat_ms == 0u) ? portMAX_DELAY
                                                                       : pdMS_TO_TICKS(heartbeat_ms));
            if (notified > 0u) g_ot_notified_wake_count++;
            else g_ot_timeout_wake_count++;
        }

        // Acquire lock to synchronize with external callers (e.g., shell
        // task calling otCliInputLine via otrLock/otrUnlock). This prevents
        // races where a CLI command posts a tasklet and the OT task preempts
        // before the CLI function finishes updating internal state.
        otrLock();
        ot_radio_drain_pending_reinit();
#if defined(CONFIG_IPRO_PM_THREAD)
        ot_pm_process();
#endif
        otTaskletsProcess(ot_instance);
        otSysProcessDrivers(ot_instance);
        otrUnlock();
    }

    otInstanceFinalize(ot_instance);
    ot_instance = NULL;

    vTaskDelete(NULL);
}

void otrStart(void) {
    if (xPortIsInsideInterrupt()) {
        return;
    }
    /* Multiple application/Matter callers must never recreate a live static
     * mutex or TCB. Reserve once, then create outside the critical section;
     * concurrent callers use otrGetInstance() to wait for initialization. */
    taskENTER_CRITICAL();
    if (ot_start_requested) {
        taskEXIT_CRITICAL();
        return;
    }
    ot_start_requested = true;
    taskEXIT_CRITICAL();

    ot_extLock = xSemaphoreCreateMutexStatic(&g_ot_lock);
    configASSERT(ot_extLock != NULL);

    OT_THREAD_SAFE(ot_taskHandle = xTaskCreateStatic(
                       otrStackTask, "threadTask", OT_TASK_SIZE, ot_instance,
                       OT_TASK_PRORITY, g_ot_stack, &g_ot_tcb););
    configASSERT(ot_taskHandle != NULL);
}

#ifdef CONFIG_SHELL
#include "shell.h"

/* `ot_task`: where is the OpenThread task? Prints its state and the context
 * it was switched out with - the saved frame at the top of its stack holds
 * mepc (the PC) and ra - plus the stack words above it, so a stalled task can
 * be located with addr2line against the image's ELF. Runs in the shell task,
 * without the OpenThread lock. */
static int cmd_ot_task(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (ot_taskHandle == NULL) {
        printf("threadTask not started\r\n");
        return 0;
    }
    const StackType_t *top = *(StackType_t *const *)ot_taskHandle; /* TCB.pxTopOfStack */
    const StackType_t *end = &g_ot_stack[OT_TASK_SIZE];

    printf("threadTask state=%d prio=%u hwm=%lu words top=%p stack=[%p,%p)\r\n",
           (int)eTaskGetState(ot_taskHandle), (unsigned)uxTaskPriorityGet(ot_taskHandle),
           (unsigned long)uxTaskGetStackHighWaterMark(ot_taskHandle),
           (const void *)top, (const void *)g_ot_stack, (const void *)end);
    if (top < g_ot_stack || top >= end) {
        printf("top of stack outside the task stack\r\n");
        return 0;
    }
    for (unsigned i = 0; &top[i] < end && i < 192u; i += 8u) {
        printf("%03u:", i);
        for (unsigned j = i; j < i + 8u && &top[j] < end; j++) {
            printf(" %08lx", (unsigned long)top[j]);
        }
        printf("\r\n");
    }
#if defined(CONFIG_THREAD_RADIO_SPINEL)
    extern void ot_radio_dbg_dump(void);
    ot_radio_dbg_dump();
#endif
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ot_task, ot_task, Dump the OpenThread task context and stack);
#endif
