/**
 * @file ipro_osal.h
 * @brief IPRO OS Abstraction Layer
 *
 * Provides a unified API for OS primitives, abstracting FreeRTOS.
 *
 * Copyright (C) 2025 IPRO Technology Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IPRO_OSAL_H
#define IPRO_OSAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Return codes */
#define IPRO_OSAL_OK            0
#define IPRO_OSAL_ERR          -1
#define IPRO_OSAL_ERR_NOMEM    -2
#define IPRO_OSAL_ERR_TIMEOUT  -3
#define IPRO_OSAL_ERR_PARAM    -4

/* Wait options */
#define IPRO_OSAL_WAIT_FOREVER  0xFFFFFFFFU
#define IPRO_OSAL_NO_WAIT       0

/*
 * Mutex
 */
typedef struct {
    void *handle;
} ipro_osal_mutex_t;

/**
 * @brief Create a mutex
 * @param mutex Pointer to mutex object
 * @return IPRO_OSAL_OK on success, error code otherwise
 */
int ipro_osal_mutex_create(ipro_osal_mutex_t *mutex);

/**
 * @brief Delete a mutex
 * @param mutex Pointer to mutex object
 */
void ipro_osal_mutex_delete(ipro_osal_mutex_t *mutex);

/**
 * @brief Lock a mutex
 * @param mutex Pointer to mutex object
 * @param timeout_ms Timeout in milliseconds (IPRO_OSAL_WAIT_FOREVER for infinite)
 * @return IPRO_OSAL_OK on success, IPRO_OSAL_ERR_TIMEOUT on timeout
 */
int ipro_osal_mutex_lock(ipro_osal_mutex_t *mutex, uint32_t timeout_ms);

/**
 * @brief Unlock a mutex
 * @param mutex Pointer to mutex object
 * @return IPRO_OSAL_OK on success
 */
int ipro_osal_mutex_unlock(ipro_osal_mutex_t *mutex);

/**
 * @brief Check if mutex is valid
 * @param mutex Pointer to mutex object
 * @return true if valid, false otherwise
 */
bool ipro_osal_mutex_is_valid(ipro_osal_mutex_t *mutex);

/*
 * Semaphore
 */
typedef struct {
    void *handle;
} ipro_osal_sem_t;

/**
 * @brief Create a counting semaphore
 * @param sem Pointer to semaphore object
 * @param init_count Initial count
 * @return IPRO_OSAL_OK on success
 */
int ipro_osal_sem_create(ipro_osal_sem_t *sem, uint32_t init_count);

/**
 * @brief Delete a semaphore
 * @param sem Pointer to semaphore object
 */
void ipro_osal_sem_delete(ipro_osal_sem_t *sem);

/**
 * @brief Wait on a semaphore (decrement)
 * @param sem Pointer to semaphore object
 * @param timeout_ms Timeout in milliseconds
 * @return IPRO_OSAL_OK on success, IPRO_OSAL_ERR_TIMEOUT on timeout
 */
int ipro_osal_sem_wait(ipro_osal_sem_t *sem, uint32_t timeout_ms);

/**
 * @brief Signal a semaphore (increment)
 * @param sem Pointer to semaphore object
 * @return IPRO_OSAL_OK on success
 */
int ipro_osal_sem_signal(ipro_osal_sem_t *sem);

/**
 * @brief Check if semaphore is valid
 * @param sem Pointer to semaphore object
 * @return true if valid, false otherwise
 */
bool ipro_osal_sem_is_valid(ipro_osal_sem_t *sem);

/**
 * @brief Signal a semaphore from ISR context
 * @param sem Pointer to semaphore object
 * @param need_yield Output: true if a context switch should be requested
 * @return IPRO_OSAL_OK on success
 */
int ipro_osal_sem_signal_isr(ipro_osal_sem_t *sem, bool *need_yield);

/*
 * Task
 */
typedef struct {
    void *handle;
} ipro_osal_task_t;

typedef void (*ipro_osal_task_fn_t)(void *arg);

/**
 * @brief Create a task
 * @param task Pointer to task object (can be NULL if handle not needed)
 * @param name Task name
 * @param fn Task function
 * @param arg Argument passed to task function
 * @param stack_size Stack size in bytes
 * @param priority Task priority (higher value = higher priority)
 * @return IPRO_OSAL_OK on success
 */
int ipro_osal_task_create(ipro_osal_task_t *task, const char *name,
                          ipro_osal_task_fn_t fn, void *arg,
                          uint32_t stack_size, uint32_t priority);

/**
 * @brief Delete a task
 * @param task Pointer to task object (NULL to delete current task)
 */
void ipro_osal_task_delete(ipro_osal_task_t *task);

/**
 * @brief Get current task name
 * @return Task name string
 */
const char *ipro_osal_task_name(void);

/**
 * @brief Yield to other tasks
 */
void ipro_osal_task_yield(void);

/*
 * Queue
 */
typedef struct {
    void *handle;
} ipro_osal_queue_t;

/**
 * @brief Create a queue
 * @param queue Pointer to queue object
 * @param item_size Size of each item in bytes
 * @param item_count Maximum number of items
 * @return IPRO_OSAL_OK on success
 */
int ipro_osal_queue_create(ipro_osal_queue_t *queue, uint32_t item_size, uint32_t item_count);

/**
 * @brief Delete a queue
 * @param queue Pointer to queue object
 */
void ipro_osal_queue_delete(ipro_osal_queue_t *queue);

/**
 * @brief Send item to queue
 * @param queue Pointer to queue object
 * @param item Pointer to item data
 * @param timeout_ms Timeout in milliseconds
 * @return IPRO_OSAL_OK on success, IPRO_OSAL_ERR_TIMEOUT on timeout
 */
int ipro_osal_queue_send(ipro_osal_queue_t *queue, const void *item, uint32_t timeout_ms);

/**
 * @brief Receive item from queue
 * @param queue Pointer to queue object
 * @param item Pointer to buffer for item data
 * @param timeout_ms Timeout in milliseconds
 * @return IPRO_OSAL_OK on success, IPRO_OSAL_ERR_TIMEOUT on timeout
 */
int ipro_osal_queue_recv(ipro_osal_queue_t *queue, void *item, uint32_t timeout_ms);

/**
 * @brief Check if queue is valid
 * @param queue Pointer to queue object
 * @return true if valid, false otherwise
 */
bool ipro_osal_queue_is_valid(ipro_osal_queue_t *queue);

/**
 * @brief Send item to queue from ISR context
 * @param queue Pointer to queue object
 * @param item Pointer to item data
 * @param need_yield Output: true if a context switch should be requested
 * @return IPRO_OSAL_OK on success, IPRO_OSAL_ERR on failure
 */
int ipro_osal_queue_send_isr(ipro_osal_queue_t *queue, const void *item,
                             bool *need_yield);

/*
 * Timer
 */
typedef struct {
    void *handle;
} ipro_osal_timer_t;

typedef void (*ipro_osal_timer_fn_t)(ipro_osal_timer_t *timer, void *arg);

/**
 * @brief Create a timer
 * @param timer Pointer to timer object
 * @param name Timer name
 * @param fn Timer callback function
 * @param arg Argument passed to callback
 * @param period_ms Timer period in milliseconds
 * @param repeat true for periodic timer, false for one-shot
 * @param auto_start true to start immediately
 * @return IPRO_OSAL_OK on success
 */
int ipro_osal_timer_create(ipro_osal_timer_t *timer, const char *name,
                           ipro_osal_timer_fn_t fn, void *arg,
                           uint32_t period_ms, bool repeat, bool auto_start);

/**
 * @brief Delete a timer
 * @param timer Pointer to timer object
 */
void ipro_osal_timer_delete(ipro_osal_timer_t *timer);

/**
 * @brief Start a timer
 * @param timer Pointer to timer object
 * @return IPRO_OSAL_OK on success
 */
int ipro_osal_timer_start(ipro_osal_timer_t *timer);

/**
 * @brief Stop a timer
 * @param timer Pointer to timer object
 * @return IPRO_OSAL_OK on success
 */
int ipro_osal_timer_stop(ipro_osal_timer_t *timer);

/**
 * @brief Change timer period
 * @param timer Pointer to timer object
 * @param period_ms New period in milliseconds
 * @return IPRO_OSAL_OK on success
 */
int ipro_osal_timer_change_period(ipro_osal_timer_t *timer, uint32_t period_ms);

/*
 * Memory
 */

/**
 * @brief Allocate memory
 * @param size Size in bytes
 * @return Pointer to allocated memory, NULL on failure
 */
void *ipro_osal_malloc(size_t size);

/**
 * @brief Allocate and zero memory
 * @param num Number of elements
 * @param size Size of each element
 * @return Pointer to allocated memory, NULL on failure
 */
void *ipro_osal_calloc(size_t num, size_t size);

/**
 * @brief Reallocate memory
 * @param ptr Pointer to existing memory (can be NULL)
 * @param size New size in bytes
 * @return Pointer to reallocated memory, NULL on failure
 */
void *ipro_osal_realloc(void *ptr, size_t size);

/**
 * @brief Free memory
 * @param ptr Pointer to memory to free
 */
void ipro_osal_free(void *ptr);

/**
 * @brief Get free heap size
 * @return Free heap size in bytes
 */
size_t ipro_osal_get_free_heap(void);

/*
 * Time
 */

/**
 * @brief Get system tick count in milliseconds
 * @return Tick count in milliseconds
 */
uint32_t ipro_osal_get_time_ms(void);

/**
 * @brief Get system time in microseconds (64-bit)
 * @return Time in microseconds
 */
uint64_t ipro_osal_get_time_us(void);

/**
 * @brief Delay for specified milliseconds
 * @param ms Delay time in milliseconds
 */
void ipro_osal_delay_ms(uint32_t ms);

/**
 * @brief Delay for specified microseconds (busy wait)
 * @param us Delay time in microseconds
 */
void ipro_osal_delay_us(uint32_t us);

/*
 * ISR Yield
 */

/**
 * @brief Request context switch from ISR if needed
 * @param need_yield Value from _isr() calls (true = switch needed)
 */
void ipro_osal_yield_from_isr(bool need_yield);

/*
 * Critical Section
 */

/**
 * @brief Enter critical section (disable interrupts)
 * @return Interrupt state to be restored
 */
uint32_t ipro_osal_critical_enter(void);

/**
 * @brief Exit critical section (restore interrupts)
 * @param state Interrupt state from ipro_osal_critical_enter()
 */
void ipro_osal_critical_exit(uint32_t state);

/*
 * System
 */

/**
 * @brief Reboot the system
 */
void ipro_osal_reboot(void);

/**
 * @brief Get OSAL version string
 * @return Version string
 */
const char *ipro_osal_version(void);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_OSAL_H */
