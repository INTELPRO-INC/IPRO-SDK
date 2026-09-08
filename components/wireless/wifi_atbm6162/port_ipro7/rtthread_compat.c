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
 * RT-Thread -> FreeRTOS compatibility shim implementation for the ATBM6162 driver.
 * Implements the small RT-Thread API subset used by the vendor `os/fh_rtt` OSAL.
 * IPRO7 port — Phase 0. See .claude/plans/atbm6162-integration.md.
 */
#include "rtthread.h"

/* ---------------------------------------------------------------------------
 * Priority mapping: RT-Thread uses LOWER number = HIGHER priority, with the
 * Wi-Fi tasks clustered around WIFI_THREAD_PRIO_START (80). FreeRTOS uses
 * HIGHER number = HIGHER priority. Invert and clamp into the valid range.
 * NOTE: if configMAX_PRIORITIES is small, several Wi-Fi tasks collapse onto the
 * top band; revisit for Phase 2+ scheduling tuning.
 * ------------------------------------------------------------------------- */
#define ATBM_RT_PRIO_BASE 80u

static UBaseType_t rt_prio_to_freertos(rt_uint8_t rt_prio)
{
    int delta = (int)rt_prio - (int)ATBM_RT_PRIO_BASE; /* 0 = highest Wi-Fi prio */
    int fp = (int)configMAX_PRIORITIES - 2 - delta;
    if (fp < 1) {
        fp = 1;
    }
    if (fp > (int)configMAX_PRIORITIES - 1) {
        fp = (int)configMAX_PRIORITIES - 1;
    }
    return (UBaseType_t)fp;
}

static TickType_t rt_timeout_to_ticks(rt_int32_t timeout)
{
    if (timeout == RT_WAITING_FOREVER) {
        return portMAX_DELAY;
    }
    if (timeout < 0) {
        timeout = 0;
    }
    return (TickType_t)timeout;
}

/* ---- Threads --------------------------------------------------------------- */
rt_thread_t rt_thread_create(const char *name, void (*entry)(void *param),
                             void *param, rt_uint32_t stack_size,
                             rt_uint8_t priority, rt_uint32_t tick)
{
    struct rt_thread *t;
    (void)tick;

    t = (struct rt_thread *)pvPortMalloc(sizeof(*t));
    if (t == RT_NULL) {
        return RT_NULL;
    }
    /* RT-Thread stack_size is in bytes; FreeRTOS depth is in words. */
    if (xTaskCreate(entry, name ? name : "atbm", stack_size / sizeof(StackType_t),
                    param, rt_prio_to_freertos(priority), &t->h) != pdPASS) {
        vPortFree(t);
        return RT_NULL;
    }
    return t;
}

rt_err_t rt_thread_startup(rt_thread_t thread)
{
    /* xTaskCreate already made the task ready. */
    (void)thread;
    return RT_EOK;
}

rt_err_t rt_thread_delete(rt_thread_t thread)
{
    if (thread == RT_NULL) {
        return -RT_ERROR;
    }
    vTaskDelete(thread->h);
    vPortFree(thread);
    return RT_EOK;
}

rt_thread_t rt_thread_self(void)
{
    /* Only used for an opaque thread-id; the returned pointer is not dereferenced
     * (atbm_getThreadStackFreesize is patched to use uxTaskGetStackHighWaterMark). */
    return (rt_thread_t)xTaskGetCurrentTaskHandle();
}

rt_err_t rt_thread_delay(rt_tick_t tick)
{
    vTaskDelay((TickType_t)tick);
    return RT_EOK;
}

/* ---- Semaphores ------------------------------------------------------------ */
rt_sem_t rt_sem_create(const char *name, rt_uint32_t value, rt_uint8_t flag)
{
    struct rt_ipc_object *s;
    (void)name;
    (void)flag;

    s = (struct rt_ipc_object *)pvPortMalloc(sizeof(*s));
    if (s == RT_NULL) {
        return RT_NULL;
    }
    s->is_mutex = 0;
    s->h = xSemaphoreCreateCounting(0x7fffffff, value);
    if (s->h == NULL) {
        vPortFree(s);
        return RT_NULL;
    }
    return s;
}

rt_err_t rt_sem_take(rt_sem_t sem, rt_int32_t timeout)
{
    if (sem == RT_NULL) {
        return -RT_ERROR;
    }
    if (xSemaphoreTake(sem->h, rt_timeout_to_ticks(timeout)) == pdTRUE) {
        return RT_EOK;
    }
    return -RT_ETIMEOUT;
}

rt_err_t rt_sem_trytake(rt_sem_t sem)
{
    return rt_sem_take(sem, 0);
}

rt_err_t rt_sem_release(rt_sem_t sem)
{
    if (sem == RT_NULL) {
        return -RT_ERROR;
    }
    return (xSemaphoreGive(sem->h) == pdTRUE) ? RT_EOK : -RT_ERROR;
}

rt_err_t rt_sem_delete(rt_sem_t sem)
{
    if (sem == RT_NULL) {
        return -RT_ERROR;
    }
    vSemaphoreDelete(sem->h);
    vPortFree(sem);
    return RT_EOK;
}

/* ---- Mutexes --------------------------------------------------------------- */
rt_mutex_t rt_mutex_create(const char *name, rt_uint8_t flag)
{
    struct rt_ipc_object *m;
    (void)name;
    (void)flag;

    m = (struct rt_ipc_object *)pvPortMalloc(sizeof(*m));
    if (m == RT_NULL) {
        return RT_NULL;
    }
    m->is_mutex = 1;
    m->h = xSemaphoreCreateRecursiveMutex();
    if (m->h == NULL) {
        vPortFree(m);
        return RT_NULL;
    }
    return m;
}

rt_err_t rt_mutex_take(rt_mutex_t mutex, rt_int32_t timeout)
{
    if (mutex == RT_NULL) {
        return -RT_ERROR;
    }
    if (xSemaphoreTakeRecursive(mutex->h, rt_timeout_to_ticks(timeout)) == pdTRUE) {
        return RT_EOK;
    }
    return -RT_ETIMEOUT;
}

rt_err_t rt_mutex_release(rt_mutex_t mutex)
{
    if (mutex == RT_NULL) {
        return -RT_ERROR;
    }
    return (xSemaphoreGiveRecursive(mutex->h) == pdTRUE) ? RT_EOK : -RT_ERROR;
}

rt_err_t rt_mutex_delete(rt_mutex_t mutex)
{
    if (mutex == RT_NULL) {
        return -RT_ERROR;
    }
    vSemaphoreDelete(mutex->h);
    vPortFree(mutex);
    return RT_EOK;
}

/* ---- Soft timers ----------------------------------------------------------- */
static void rt_timer_trampoline(TimerHandle_t xTimer)
{
    struct rt_timer *t = (struct rt_timer *)pvTimerGetTimerID(xTimer);
    if (t && t->cb) {
        t->cb(t->param);
    }
}

rt_timer_t rt_timer_create(const char *name, void (*timeout)(void *param),
                           void *param, rt_tick_t time, rt_uint8_t flag)
{
    struct rt_timer *t;
    UBaseType_t auto_reload = (flag & RT_TIMER_FLAG_PERIODIC) ? pdTRUE : pdFALSE;

    t = (struct rt_timer *)pvPortMalloc(sizeof(*t));
    if (t == RT_NULL) {
        return RT_NULL;
    }
    t->cb = timeout;
    t->param = param;
    if (time == 0) {
        time = 1; /* FreeRTOS rejects a 0-tick period */
    }
    t->h = xTimerCreate(name ? name : "atbm", (TickType_t)time, auto_reload,
                        (void *)t, rt_timer_trampoline);
    if (t->h == NULL) {
        vPortFree(t);
        return RT_NULL;
    }
    return t;
}

rt_err_t rt_timer_start(rt_timer_t timer)
{
    if (timer == RT_NULL) {
        return -RT_ERROR;
    }
    return (xTimerStart(timer->h, 0) == pdPASS) ? RT_EOK : -RT_ERROR;
}

rt_err_t rt_timer_stop(rt_timer_t timer)
{
    if (timer == RT_NULL) {
        return -RT_ERROR;
    }
    return (xTimerStop(timer->h, 0) == pdPASS) ? RT_EOK : -RT_ERROR;
}

rt_err_t rt_timer_delete(rt_timer_t timer)
{
    if (timer == RT_NULL) {
        return -RT_ERROR;
    }
    xTimerDelete(timer->h, 0);
    vPortFree(timer);
    return RT_EOK;
}

rt_err_t rt_timer_control(rt_timer_t timer, int cmd, void *arg)
{
    if (timer == RT_NULL) {
        return -RT_ERROR;
    }
    if (cmd == RT_TIMER_CTRL_SET_TIME) {
        TickType_t ticks = *(rt_tick_t *)arg;
        if (ticks == 0) {
            ticks = 1;
        }
        return (xTimerChangePeriod(timer->h, ticks, 0) == pdPASS) ? RT_EOK : -RT_ERROR;
    }
    return -RT_ERROR;
}

/* ---- Message queues -------------------------------------------------------- */
rt_err_t rt_mq_init(rt_mq_t mq, const char *name, void *msgpool,
                    rt_size_t msg_size, rt_size_t pool_size, rt_uint8_t flag)
{
    rt_size_t depth;
    (void)name;
    (void)msgpool; /* FreeRTOS allocates its own queue storage */
    (void)flag;

    if (mq == RT_NULL || msg_size == 0) {
        return -RT_ERROR;
    }
    depth = pool_size / msg_size;
    if (depth == 0) {
        depth = 1;
    }
    mq->msg_size = msg_size;
    mq->h = xQueueCreate(depth, msg_size);
    return (mq->h != NULL) ? RT_EOK : -RT_ERROR;
}

rt_err_t rt_mq_send(rt_mq_t mq, void *buffer, rt_size_t size)
{
    (void)size;
    if (mq == RT_NULL) {
        return -RT_ERROR;
    }
    return (xQueueSend(mq->h, buffer, 0) == pdTRUE) ? RT_EOK : -RT_EFULL;
}

rt_err_t rt_mq_recv(rt_mq_t mq, void *buffer, rt_size_t size, rt_int32_t timeout)
{
    (void)size;
    if (mq == RT_NULL) {
        return -RT_ERROR;
    }
    return (xQueueReceive(mq->h, buffer, rt_timeout_to_ticks(timeout)) == pdTRUE)
               ? RT_EOK
               : -RT_ETIMEOUT;
}

rt_err_t rt_mq_detach(rt_mq_t mq)
{
    if (mq == RT_NULL || mq->h == NULL) {
        return -RT_ERROR;
    }
    vQueueDelete(mq->h);
    mq->h = NULL;
    return RT_EOK;
}

rt_err_t rt_mq_delete(rt_mq_t mq)
{
    return rt_mq_detach(mq);
}
