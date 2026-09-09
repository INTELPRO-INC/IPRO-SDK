/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * lwIP 2.1 system-architecture layer on FreeRTOS (V11), shared by every
 * IPRO SoC port in this SDK (ipro7, ipro6, ipro6le).
 *
 * Units and contracts, because every one of them has bitten a port before:
 *
 *   - lwIP timeouts are in MILLISECONDS; FreeRTOS waits are in TICKS. The
 *     conversion rounds UP and never turns a non-zero wait into a poll
 *     (sys_ms_to_ticks). sys_now() must go the other way and multiply.
 *   - sys_thread_new()'s stacksize is passed to xTaskCreate() as-is, i.e. in
 *     StackType_t WORDS, matching how lwipopts.h sizes TCPIP_THREAD_STACKSIZE.
 *   - sys_arch_protect() may be reached from an ISR (SYS_LIGHTWEIGHT_PROT
 *     paths, pbuf_free from a driver's RX interrupt). The port's
 *     vPortEnterCritical() asserts when called with the interrupt threshold
 *     raised, so the ISR case has to take the *_FROM_ISR pair.
 *   - sys_current_is_tcpip() answers "is the caller the TCP/IP thread", and
 *     only that thread: the handle is captured by name, not by "last thread
 *     created".
 */

#include <string.h>

#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Handle of the thread named TCPIP_THREAD_NAME. Set once by sys_thread_new(). */
static TaskHandle_t s_tcpip_task;

/*---------------------------------------------------------------------------*/
/* Time conversion                                                            */
/*---------------------------------------------------------------------------*/

/* Milliseconds to ticks, rounding up. A non-zero wait must never collapse to
 * zero ticks: below one tick period pdMS_TO_TICKS() truncates to 0, which
 * makes xQueueReceive()/xSemaphoreTake() return immediately and the caller
 * reports a spurious SYS_ARCH_TIMEOUT. At configTICK_RATE_HZ == 1000 this is
 * invisible; at any slower tick it is not. */
static TickType_t sys_ms_to_ticks(u32_t ms)
{
    TickType_t ticks = pdMS_TO_TICKS(ms);

    if (ms != 0 && ticks == 0) {
        ticks = 1;
    }
    return ticks;
}

/* Milliseconds elapsed since `start`, as lwIP wants it returned from a wait.
 * SYS_ARCH_TIMEOUT is reserved for "timed out", so a wait that genuinely
 * lasted 0xFFFFFFFF ms (49.7 days) is reported one short rather than as a
 * timeout. */
static u32_t sys_elapsed_ms(TickType_t start)
{
    u32_t ms = (u32_t)((xTaskGetTickCount() - start) * portTICK_PERIOD_MS);

    if (ms == SYS_ARCH_TIMEOUT) {
        ms--;
    }
    return ms;
}

/*---------------------------------------------------------------------------*/
/* Mailboxes                                                                  */
/*---------------------------------------------------------------------------*/

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    *mbox = xQueueCreate((UBaseType_t)size, sizeof(void *));
    if (*mbox == NULL) {
        SYS_STATS_INC(mbox.err);
        return ERR_MEM;
    }
    SYS_STATS_INC_USED(mbox);
    return ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    /* Messages left behind mean lwIP freed a live mailbox. Count it; do not
     * hang here, the caller is on a teardown path. */
    if (uxQueueMessagesWaiting(*mbox) != 0) {
        SYS_STATS_INC(mbox.err);
    }
    vQueueDelete(*mbox);
    SYS_STATS_DEC(mbox.used);
}

/* Task context only. lwIP relies on this call never failing. */
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    while (xQueueSendToBack(*mbox, &msg, portMAX_DELAY) != pdTRUE) {
    }
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    if (xQueueSendToBack(*mbox, &msg, 0) != pdTRUE) {
        SYS_STATS_INC(mbox.err);
        return ERR_MEM;
    }
    return ERR_OK;
}

/* The one mailbox entry point lwIP permits from an ISR
 * (tcpip_callbackmsg_trycallback_fromisr). Must use the FromISR queue API and
 * hand a pending context switch back to the port. */
err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
    BaseType_t woken = pdFALSE;

    if (xQueueSendToBackFromISR(*mbox, &msg, &woken) != pdTRUE) {
        SYS_STATS_INC(mbox.err);
        return ERR_MEM;
    }
    portYIELD_FROM_ISR(woken);
    return ERR_OK;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    void *dummy;
    TickType_t start = xTaskGetTickCount();

    if (msg == NULL) {
        msg = &dummy;
    }

    if (timeout == 0) {
        /* 0 means "wait forever" in this API. */
        while (xQueueReceive(*mbox, msg, portMAX_DELAY) != pdTRUE) {
        }
        return sys_elapsed_ms(start);
    }

    if (xQueueReceive(*mbox, msg, sys_ms_to_ticks(timeout)) != pdTRUE) {
        *msg = NULL;
        return SYS_ARCH_TIMEOUT;
    }
    return sys_elapsed_ms(start);
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    void *dummy;

    if (msg == NULL) {
        msg = &dummy;
    }
    if (xQueueReceive(*mbox, msg, 0) != pdTRUE) {
        return SYS_MBOX_EMPTY;
    }
    return 0;
}

int sys_mbox_valid(sys_mbox_t *mbox)
{
    return *mbox != SYS_MBOX_NULL;
}

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    *mbox = SYS_MBOX_NULL;
}

/*---------------------------------------------------------------------------*/
/* Semaphores                                                                 */
/*---------------------------------------------------------------------------*/

/* lwIP only ever asks for count 0 or 1; a binary semaphore is the right
 * object. xSemaphoreCreateBinary() hands it back EMPTY, so a non-zero initial
 * count is one give. */
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    *sem = xSemaphoreCreateBinary();
    if (*sem == NULL) {
        SYS_STATS_INC(sem.err);
        return ERR_MEM;
    }
    if (count != 0) {
        (void)xSemaphoreGive(*sem);
    }
    SYS_STATS_INC_USED(sem);
    return ERR_OK;
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    TickType_t start = xTaskGetTickCount();

    if (timeout == 0) {
        while (xSemaphoreTake(*sem, portMAX_DELAY) != pdTRUE) {
        }
        return sys_elapsed_ms(start);
    }

    if (xSemaphoreTake(*sem, sys_ms_to_ticks(timeout)) != pdTRUE) {
        return SYS_ARCH_TIMEOUT;
    }
    return sys_elapsed_ms(start);
}

void sys_sem_signal(sys_sem_t *sem)
{
    (void)xSemaphoreGive(*sem);
}

void sys_sem_free(sys_sem_t *sem)
{
    SYS_STATS_DEC(sem.used);
    vSemaphoreDelete(*sem);
}

int sys_sem_valid(sys_sem_t *sem)
{
    return *sem != SYS_SEM_NULL;
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
    *sem = SYS_SEM_NULL;
}

/*---------------------------------------------------------------------------*/
/* Mutexes                                                                    */
/*---------------------------------------------------------------------------*/

#if LWIP_COMPAT_MUTEX == 0

err_t sys_mutex_new(sys_mutex_t *mutex)
{
    *mutex = xSemaphoreCreateMutex();
    if (*mutex == NULL) {
        SYS_STATS_INC(mutex.err);
        return ERR_MEM;
    }
    SYS_STATS_INC_USED(mutex);
    return ERR_OK;
}

void sys_mutex_free(sys_mutex_t *mutex)
{
    SYS_STATS_DEC(mutex.used);
    vSemaphoreDelete(*mutex);
}

void sys_mutex_lock(sys_mutex_t *mutex)
{
    while (xSemaphoreTake(*mutex, portMAX_DELAY) != pdTRUE) {
    }
}

void sys_mutex_unlock(sys_mutex_t *mutex)
{
    (void)xSemaphoreGive(*mutex);
}

/* Used by the Wi-Fi adapter's LWIP_ASSERT_CORE_LOCKED(). */
int sys_mutex_is_locked(sys_mutex_t *mutex)
{
    return uxSemaphoreGetCount(*mutex) == 0;
}

#endif /* LWIP_COMPAT_MUTEX == 0 */

/*---------------------------------------------------------------------------*/
/* Threads                                                                    */
/*---------------------------------------------------------------------------*/

void sys_init(void)
{
    s_tcpip_task = NULL;
}

/* `stacksize` is in StackType_t words, exactly as xTaskCreate() takes it. */
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg,
                            int stacksize, int prio)
{
    TaskHandle_t handle = NULL;

    if (stacksize <= 0) {
        stacksize = SYS_DEFAULT_THREAD_STACK_DEPTH;
    }
    if (xTaskCreate(thread, name, (configSTACK_DEPTH_TYPE)stacksize, arg,
                    (UBaseType_t)prio, &handle) != pdPASS) {
        return NULL;
    }

    /* Remember the TCP/IP thread and nothing else. Recording "the last
     * thread this function created" made sys_current_is_tcpip() answer for
     * whichever helper thread happened to be started after tcpip_init(), and
     * LWIP_ASSERT_CORE_LOCKED() then fired from the real TCP/IP thread. */
    if (s_tcpip_task == NULL && strcmp(name, TCPIP_THREAD_NAME) == 0) {
        s_tcpip_task = handle;
    }
    return handle;
}

int sys_current_is_tcpip(void)
{
    return s_tcpip_task != NULL && s_tcpip_task == xTaskGetCurrentTaskHandle();
}

int sys_is_inside_interrupt(void)
{
    return xPortIsInsideInterrupt() ? 1 : 0;
}

/*---------------------------------------------------------------------------*/
/* Critical sections                                                          */
/*---------------------------------------------------------------------------*/

/* Interrupt context takes the FromISR pair and carries the saved mask in the
 * return value; task context nests through the ordinary critical section and
 * the value is unused. The context cannot change between protect() and
 * unprotect(), so unprotect() re-asks the port rather than encoding it. */
sys_prot_t sys_arch_protect(void)
{
    if (xPortIsInsideInterrupt()) {
        return (sys_prot_t)taskENTER_CRITICAL_FROM_ISR();
    }
    taskENTER_CRITICAL();
    return 0;
}

void sys_arch_unprotect(sys_prot_t pval)
{
    if (xPortIsInsideInterrupt()) {
        taskEXIT_CRITICAL_FROM_ISR((UBaseType_t)pval);
    } else {
        (void)pval;
        taskEXIT_CRITICAL();
    }
}

/*---------------------------------------------------------------------------*/
/* Misc                                                                       */
/*---------------------------------------------------------------------------*/

/* Ticks to milliseconds. The previous port divided here, which only worked
 * because the tick happens to be 1 ms; every lwIP timer (TCP retransmit,
 * ARP, DHCP, PPP) is driven off this value. */
u32_t sys_now(void)
{
    return (u32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

/*---------------------------------------------------------------------------*/
/* Per-thread netconn semaphore (LWIP_NETCONN_SEM_PER_THREAD)                 */
/*---------------------------------------------------------------------------*/

#if LWIP_NETCONN_SEM_PER_THREAD

#if configNUM_THREAD_LOCAL_STORAGE_POINTERS < 1
#error "LWIP_NETCONN_SEM_PER_THREAD needs configNUM_THREAD_LOCAL_STORAGE_POINTERS >= 1 in FreeRTOSConfig.h"
#endif

#define SYS_TLS_SEM_INDEX 0

/* Standard FreeRTOS thread-local storage. There is no delete hook in stock
 * FreeRTOS, so a thread that used the netconn API must call
 * sys_thread_sem_deinit() (netconn_thread_cleanup()) before it exits, as
 * lwIP documents. */
static sys_sem_t *sys_thread_sem_alloc(void)
{
    sys_sem_t *sem = (sys_sem_t *)pvPortMalloc(sizeof(*sem));

    LWIP_ASSERT("failed to allocate memory for TLS semaphore", sem != NULL);
    if (sem == NULL) {
        return NULL;
    }
    if (sys_sem_new(sem, 0) != ERR_OK) {
        LWIP_ASSERT("failed to initialise TLS semaphore", 0);
        vPortFree(sem);
        return NULL;
    }
    vTaskSetThreadLocalStoragePointer(NULL, SYS_TLS_SEM_INDEX, sem);
    return sem;
}

void *sys_thread_sem_get(void)
{
    sys_sem_t *sem = (sys_sem_t *)pvTaskGetThreadLocalStoragePointer(NULL, SYS_TLS_SEM_INDEX);

    if (sem == NULL) {
        sem = sys_thread_sem_alloc();
    }
    return sem;
}

void sys_thread_sem_init(void)
{
    (void)sys_thread_sem_alloc();
}

void sys_thread_sem_deinit(void)
{
    sys_sem_t *sem = (sys_sem_t *)pvTaskGetThreadLocalStoragePointer(NULL, SYS_TLS_SEM_INDEX);

    if (sem != NULL) {
        sys_sem_free(sem);
        vPortFree(sem);
        vTaskSetThreadLocalStoragePointer(NULL, SYS_TLS_SEM_INDEX, NULL);
    }
}

#endif /* LWIP_NETCONN_SEM_PER_THREAD */
