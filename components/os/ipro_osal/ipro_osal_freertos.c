/**
 * @file ipro_osal_freertos.c
 * @brief IPRO OSAL FreeRTOS Implementation
 *
 * Copyright (C) 2025 IPRO Technology Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ipro_osal.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

#include <string.h>

#define IPRO_OSAL_VERSION "1.0.0"

/* Convert ms to ticks, handle WAIT_FOREVER */
static inline TickType_t ms_to_ticks(uint32_t ms)
{
    if (ms == IPRO_OSAL_WAIT_FOREVER) {
        return portMAX_DELAY;
    }
    return pdMS_TO_TICKS(ms);
}

/*
 * Mutex Implementation
 */

int ipro_osal_mutex_create(ipro_osal_mutex_t *mutex)
{
    if (!mutex) {
        return IPRO_OSAL_ERR_PARAM;
    }

    mutex->handle = xSemaphoreCreateMutex();
    if (!mutex->handle) {
        return IPRO_OSAL_ERR_NOMEM;
    }

    return IPRO_OSAL_OK;
}

void ipro_osal_mutex_delete(ipro_osal_mutex_t *mutex)
{
    if (mutex && mutex->handle) {
        vSemaphoreDelete((SemaphoreHandle_t)mutex->handle);
        mutex->handle = NULL;
    }
}

int ipro_osal_mutex_lock(ipro_osal_mutex_t *mutex, uint32_t timeout_ms)
{
    if (!mutex || !mutex->handle) {
        return IPRO_OSAL_ERR_PARAM;
    }

    if (xSemaphoreTake((SemaphoreHandle_t)mutex->handle, ms_to_ticks(timeout_ms)) == pdTRUE) {
        return IPRO_OSAL_OK;
    }

    return IPRO_OSAL_ERR_TIMEOUT;
}

int ipro_osal_mutex_unlock(ipro_osal_mutex_t *mutex)
{
    if (!mutex || !mutex->handle) {
        return IPRO_OSAL_ERR_PARAM;
    }

    if (xSemaphoreGive((SemaphoreHandle_t)mutex->handle) == pdTRUE) {
        return IPRO_OSAL_OK;
    }

    return IPRO_OSAL_ERR;
}

bool ipro_osal_mutex_is_valid(ipro_osal_mutex_t *mutex)
{
    return (mutex && mutex->handle);
}

/*
 * Semaphore Implementation
 */

int ipro_osal_sem_create(ipro_osal_sem_t *sem, uint32_t init_count)
{
    if (!sem) {
        return IPRO_OSAL_ERR_PARAM;
    }

    /* Use counting semaphore with max = 0xFFFF */
    sem->handle = xSemaphoreCreateCounting(0xFFFF, init_count);
    if (!sem->handle) {
        return IPRO_OSAL_ERR_NOMEM;
    }

    return IPRO_OSAL_OK;
}

void ipro_osal_sem_delete(ipro_osal_sem_t *sem)
{
    if (sem && sem->handle) {
        vSemaphoreDelete((SemaphoreHandle_t)sem->handle);
        sem->handle = NULL;
    }
}

int ipro_osal_sem_wait(ipro_osal_sem_t *sem, uint32_t timeout_ms)
{
    if (!sem || !sem->handle) {
        return IPRO_OSAL_ERR_PARAM;
    }

    if (xSemaphoreTake((SemaphoreHandle_t)sem->handle, ms_to_ticks(timeout_ms)) == pdTRUE) {
        return IPRO_OSAL_OK;
    }

    return IPRO_OSAL_ERR_TIMEOUT;
}

int ipro_osal_sem_signal(ipro_osal_sem_t *sem)
{
    if (!sem || !sem->handle) {
        return IPRO_OSAL_ERR_PARAM;
    }

    if (xSemaphoreGive((SemaphoreHandle_t)sem->handle) == pdTRUE) {
        return IPRO_OSAL_OK;
    }

    return IPRO_OSAL_ERR;
}

bool ipro_osal_sem_is_valid(ipro_osal_sem_t *sem)
{
    return (sem && sem->handle);
}

int ipro_osal_sem_signal_isr(ipro_osal_sem_t *sem, bool *need_yield)
{
    if (!sem || !sem->handle) {
        return IPRO_OSAL_ERR_PARAM;
    }

    BaseType_t woken = pdFALSE;
    if (xSemaphoreGiveFromISR((SemaphoreHandle_t)sem->handle, &woken) == pdTRUE) {
        if (need_yield)
            *need_yield = (woken == pdTRUE);
        return IPRO_OSAL_OK;
    }

    return IPRO_OSAL_ERR;
}

/*
 * Task Implementation
 */

int ipro_osal_task_create(ipro_osal_task_t *task, const char *name,
                          ipro_osal_task_fn_t fn, void *arg,
                          uint32_t stack_size, uint32_t priority)
{
    if (!fn) {
        return IPRO_OSAL_ERR_PARAM;
    }

    TaskHandle_t handle = NULL;
    BaseType_t ret = xTaskCreate(
        (TaskFunction_t)fn,
        name ? name : "task",
        stack_size / sizeof(StackType_t),  /* Convert bytes to words */
        arg,
        priority,
        &handle
    );

    if (ret != pdPASS) {
        return IPRO_OSAL_ERR_NOMEM;
    }

    if (task) {
        task->handle = handle;
    }

    return IPRO_OSAL_OK;
}

void ipro_osal_task_delete(ipro_osal_task_t *task)
{
    TaskHandle_t handle = NULL;

    if (task) {
        handle = (TaskHandle_t)task->handle;
        task->handle = NULL;
    }

    vTaskDelete(handle);  /* NULL deletes current task */
}

const char *ipro_osal_task_name(void)
{
    return pcTaskGetName(NULL);
}

void ipro_osal_task_yield(void)
{
    taskYIELD();
}

/*
 * Queue Implementation
 */

int ipro_osal_queue_create(ipro_osal_queue_t *queue, uint32_t item_size, uint32_t item_count)
{
    if (!queue || item_size == 0 || item_count == 0) {
        return IPRO_OSAL_ERR_PARAM;
    }

    queue->handle = xQueueCreate(item_count, item_size);
    if (!queue->handle) {
        return IPRO_OSAL_ERR_NOMEM;
    }

    return IPRO_OSAL_OK;
}

void ipro_osal_queue_delete(ipro_osal_queue_t *queue)
{
    if (queue && queue->handle) {
        vQueueDelete((QueueHandle_t)queue->handle);
        queue->handle = NULL;
    }
}

int ipro_osal_queue_send(ipro_osal_queue_t *queue, const void *item, uint32_t timeout_ms)
{
    if (!queue || !queue->handle || !item) {
        return IPRO_OSAL_ERR_PARAM;
    }

    if (xQueueSend((QueueHandle_t)queue->handle, item, ms_to_ticks(timeout_ms)) == pdTRUE) {
        return IPRO_OSAL_OK;
    }

    return IPRO_OSAL_ERR_TIMEOUT;
}

int ipro_osal_queue_recv(ipro_osal_queue_t *queue, void *item, uint32_t timeout_ms)
{
    if (!queue || !queue->handle || !item) {
        return IPRO_OSAL_ERR_PARAM;
    }

    if (xQueueReceive((QueueHandle_t)queue->handle, item, ms_to_ticks(timeout_ms)) == pdTRUE) {
        return IPRO_OSAL_OK;
    }

    return IPRO_OSAL_ERR_TIMEOUT;
}

bool ipro_osal_queue_is_valid(ipro_osal_queue_t *queue)
{
    return (queue && queue->handle);
}

int ipro_osal_queue_send_isr(ipro_osal_queue_t *queue, const void *item,
                             bool *need_yield)
{
    if (!queue || !queue->handle || !item) {
        return IPRO_OSAL_ERR_PARAM;
    }

    BaseType_t woken = pdFALSE;
    if (xQueueSendFromISR((QueueHandle_t)queue->handle, item, &woken) == pdTRUE) {
        if (need_yield)
            *need_yield = (woken == pdTRUE);
        return IPRO_OSAL_OK;
    }

    return IPRO_OSAL_ERR;
}

/*
 * Timer Implementation
 */

/* Timer context to store user callback and arg */
typedef struct {
    ipro_osal_timer_fn_t fn;
    void *arg;
    ipro_osal_timer_t *timer;
} timer_context_t;

static void timer_callback_wrapper(TimerHandle_t xTimer)
{
    timer_context_t *ctx = (timer_context_t *)pvTimerGetTimerID(xTimer);
    if (ctx && ctx->fn) {
        ctx->fn(ctx->timer, ctx->arg);
    }
}

int ipro_osal_timer_create(ipro_osal_timer_t *timer, const char *name,
                           ipro_osal_timer_fn_t fn, void *arg,
                           uint32_t period_ms, bool repeat, bool auto_start)
{
    if (!timer || !fn || period_ms == 0) {
        return IPRO_OSAL_ERR_PARAM;
    }

    /* Allocate timer context */
    timer_context_t *ctx = pvPortMalloc(sizeof(timer_context_t));
    if (!ctx) {
        return IPRO_OSAL_ERR_NOMEM;
    }

    ctx->fn = fn;
    ctx->arg = arg;
    ctx->timer = timer;

    TimerHandle_t handle = xTimerCreate(
        name ? name : "timer",
        pdMS_TO_TICKS(period_ms),
        repeat ? pdTRUE : pdFALSE,
        ctx,
        timer_callback_wrapper
    );

    if (!handle) {
        vPortFree(ctx);
        return IPRO_OSAL_ERR_NOMEM;
    }

    timer->handle = handle;

    if (auto_start) {
        if (xTimerStart(handle, 0) != pdPASS) {
            xTimerDelete(handle, 0);
            vPortFree(ctx);
            timer->handle = NULL;
            return IPRO_OSAL_ERR;
        }
    }

    return IPRO_OSAL_OK;
}

void ipro_osal_timer_delete(ipro_osal_timer_t *timer)
{
    if (timer && timer->handle) {
        TimerHandle_t handle = (TimerHandle_t)timer->handle;
        timer_context_t *ctx = (timer_context_t *)pvTimerGetTimerID(handle);

        xTimerDelete(handle, portMAX_DELAY);

        if (ctx) {
            vPortFree(ctx);
        }

        timer->handle = NULL;
    }
}

int ipro_osal_timer_start(ipro_osal_timer_t *timer)
{
    if (!timer || !timer->handle) {
        return IPRO_OSAL_ERR_PARAM;
    }

    if (xTimerStart((TimerHandle_t)timer->handle, 0) == pdPASS) {
        return IPRO_OSAL_OK;
    }

    return IPRO_OSAL_ERR;
}

int ipro_osal_timer_stop(ipro_osal_timer_t *timer)
{
    if (!timer || !timer->handle) {
        return IPRO_OSAL_ERR_PARAM;
    }

    if (xTimerStop((TimerHandle_t)timer->handle, 0) == pdPASS) {
        return IPRO_OSAL_OK;
    }

    return IPRO_OSAL_ERR;
}

int ipro_osal_timer_change_period(ipro_osal_timer_t *timer, uint32_t period_ms)
{
    if (!timer || !timer->handle || period_ms == 0) {
        return IPRO_OSAL_ERR_PARAM;
    }

    if (xTimerChangePeriod((TimerHandle_t)timer->handle, pdMS_TO_TICKS(period_ms), 0) == pdPASS) {
        return IPRO_OSAL_OK;
    }

    return IPRO_OSAL_ERR;
}

/*
 * Memory Implementation
 */

void *ipro_osal_malloc(size_t size)
{
    return pvPortMalloc(size);
}

void *ipro_osal_calloc(size_t num, size_t size)
{
    size_t total = num * size;
    void *ptr = pvPortMalloc(total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void *ipro_osal_realloc(void *ptr, size_t size)
{
    /* FreeRTOS doesn't have realloc, implement manually */
    if (!ptr) {
        return pvPortMalloc(size);
    }

    if (size == 0) {
        vPortFree(ptr);
        return NULL;
    }

    void *new_ptr = pvPortMalloc(size);
    if (new_ptr) {
        /* Note: We don't know the old size, so this is a simplified implementation.
         * In practice, you may need to track allocation sizes separately. */
        memcpy(new_ptr, ptr, size);
        vPortFree(ptr);
    }

    return new_ptr;
}

void ipro_osal_free(void *ptr)
{
    if (ptr) {
        vPortFree(ptr);
    }
}

size_t ipro_osal_get_free_heap(void)
{
    return xPortGetFreeHeapSize();
}

/*
 * Time Implementation
 */

uint32_t ipro_osal_get_time_ms(void)
{
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

uint64_t ipro_osal_get_time_us(void)
{
    /* Read RISC-V mcycle CSR for microsecond resolution */
    uint64_t cycles;

#if __riscv_xlen == 32
    uint32_t hi1, hi2, lo;
    do {
        __asm__ volatile("csrr %0, mcycleh" : "=r"(hi1));
        __asm__ volatile("csrr %0, mcycle" : "=r"(lo));
        __asm__ volatile("csrr %0, mcycleh" : "=r"(hi2));
    } while (hi1 != hi2);
    cycles = ((uint64_t)hi1 << 32) | lo;
#else
    __asm__ volatile("csrr %0, mcycle" : "=r"(cycles));
#endif

    /* Assuming ~160MHz CPU clock, divide by 160 to get microseconds */
    return cycles / 160;
}

void ipro_osal_delay_ms(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void ipro_osal_delay_us(uint32_t us)
{
    /* Busy wait for microsecond delays */
    uint64_t start = ipro_osal_get_time_us();
    while ((ipro_osal_get_time_us() - start) < us) {
        /* Busy wait */
    }
}

/*
 * ISR Yield Implementation
 */

void ipro_osal_yield_from_isr(bool need_yield)
{
    portYIELD_FROM_ISR(need_yield ? pdTRUE : pdFALSE);
}

/*
 * Critical Section Implementation
 */

uint32_t ipro_osal_critical_enter(void)
{
    taskENTER_CRITICAL();
    return 0;  /* FreeRTOS doesn't return state from taskENTER_CRITICAL */
}

void ipro_osal_critical_exit(uint32_t state)
{
    (void)state;
    taskEXIT_CRITICAL();
}

/*
 * System Implementation
 */

void ipro_osal_reboot(void)
{
    /* Platform-specific reboot - use HAL */
    extern void hal_sys_reset(void);
    hal_sys_reset();
}

const char *ipro_osal_version(void)
{
    return IPRO_OSAL_VERSION;
}
