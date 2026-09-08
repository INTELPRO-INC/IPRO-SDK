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
 * RT-Thread -> FreeRTOS compatibility shim for the ATBM6162 (Oceanus) Wi-Fi driver.
 *
 * The vendor `os/fh_rtt` OSAL was written against RT-Thread. Rather than edit every
 * call site, this shim provides the small RT-Thread API subset the Wi-Fi port actually
 * uses, implemented on top of FreeRTOS (IPRO7). The fh_rtt sources include <rtdef.h>
 * and <rtthread.h>; with `port_ipro7/` on the include path those resolve here.
 *
 * IPRO7 port — Phase 0 scaffolding. See .claude/plans/atbm6162-integration.md.
 */
#ifndef ATBM_RTTHREAD_COMPAT_H
#define ATBM_RTTHREAD_COMPAT_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>   /* vendor uses EINVAL/ENOMEM/... without including this */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- RT-Thread scalar types -------------------------------------------------- */
typedef int             rt_err_t;
typedef long            rt_base_t;
typedef unsigned long   rt_ubase_t;
typedef unsigned int    rt_tick_t;
typedef signed char     rt_int8_t;
typedef signed short    rt_int16_t;
typedef signed int      rt_int32_t;
typedef unsigned char   rt_uint8_t;
typedef unsigned short  rt_uint16_t;
typedef unsigned int    rt_uint32_t;
typedef unsigned int    rt_size_t;
typedef int             rt_bool_t;

/* lwIP-style fixed-width aliases used by the vendor os_api.h (struct sockaddr).
 * Identical to lwip/arch/cc.h, so co-inclusion with lwIP is a valid C11
 * same-type typedef redefinition. (ATBM_OS_CPU_SR is defined by the vendor's
 * os/include/atbm_type.h — do not redefine it here.) */
#ifndef ATBM_LWIP_SCALARS_DEFINED
#define ATBM_LWIP_SCALARS_DEFINED
typedef uint8_t  u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef int8_t   s8_t;
typedef int16_t  s16_t;
typedef int32_t  s32_t;
#endif

/* ---- RT-Thread constants ----------------------------------------------------- */
#define RT_NULL              ((void *)0)
#define RT_EOK               0
#define RT_ERROR             1
#define RT_ETIMEOUT          116
#define RT_EFULL             117
#define RT_EEMPTY            118
#define RT_WAITING_FOREVER   ((rt_int32_t)-1)
#define RT_WAITING_NO        0
#define RT_TICK_PER_SECOND   (configTICK_RATE_HZ)

#define RT_IPC_FLAG_FIFO     0x00
#define RT_IPC_FLAG_PRIO     0x01

#define RT_TIMER_FLAG_DEACTIVATED  0x0
#define RT_TIMER_FLAG_ACTIVATED    0x1
#define RT_TIMER_FLAG_ONE_SHOT     0x0
#define RT_TIMER_FLAG_PERIODIC     0x2
#define RT_TIMER_FLAG_HARD_TIMER   0x0
#define RT_TIMER_FLAG_SOFT_TIMER   0x4
#define RT_TIMER_CTRL_SET_TIME     0x1

/* iot_printf (vendor os_api.h does: #define iot_printf rt_kprintf) -> IPRO7 console */
#ifndef rt_kprintf
#define rt_kprintf printf
#endif

/* ---- IPC / kernel objects (opaque wrappers over FreeRTOS handles) ------------- */
struct rt_thread {
    TaskHandle_t h;
};
typedef struct rt_thread *rt_thread_t;

/* A single object type backs both rt_sem_t and rt_mutex_t so the vendor's
 * cross-assignment (atbm_mutex = rt_sem_t = rt_mutex_create()) type-checks. */
struct rt_ipc_object {
    SemaphoreHandle_t h;
    rt_uint8_t        is_mutex;
};
typedef struct rt_ipc_object *rt_sem_t;
typedef struct rt_ipc_object *rt_mutex_t;

struct rt_timer {
    TimerHandle_t h;
    void (*cb)(void *param);
    void *param;
};
typedef struct rt_timer *rt_timer_t;

struct rt_messagequeue {
    QueueHandle_t h;
    rt_uint32_t   msg_size;
};
typedef struct rt_messagequeue *rt_mq_t;

/* ---- Threads ----------------------------------------------------------------- */
rt_thread_t rt_thread_create(const char *name, void (*entry)(void *param),
                             void *param, rt_uint32_t stack_size,
                             rt_uint8_t priority, rt_uint32_t tick);
rt_err_t    rt_thread_startup(rt_thread_t thread);
rt_err_t    rt_thread_delete(rt_thread_t thread);
rt_thread_t rt_thread_self(void);
rt_err_t    rt_thread_delay(rt_tick_t tick);

/* ---- Semaphores -------------------------------------------------------------- */
rt_sem_t rt_sem_create(const char *name, rt_uint32_t value, rt_uint8_t flag);
rt_err_t rt_sem_take(rt_sem_t sem, rt_int32_t timeout);
rt_err_t rt_sem_trytake(rt_sem_t sem);
rt_err_t rt_sem_release(rt_sem_t sem);
rt_err_t rt_sem_delete(rt_sem_t sem);

/* ---- Mutexes ----------------------------------------------------------------- */
rt_mutex_t rt_mutex_create(const char *name, rt_uint8_t flag);
rt_err_t   rt_mutex_take(rt_mutex_t mutex, rt_int32_t timeout);
rt_err_t   rt_mutex_release(rt_mutex_t mutex);
rt_err_t   rt_mutex_delete(rt_mutex_t mutex);

/* ---- Soft timers ------------------------------------------------------------- */
rt_timer_t rt_timer_create(const char *name, void (*timeout)(void *param),
                           void *param, rt_tick_t time, rt_uint8_t flag);
rt_err_t   rt_timer_start(rt_timer_t timer);
rt_err_t   rt_timer_stop(rt_timer_t timer);
rt_err_t   rt_timer_delete(rt_timer_t timer);
rt_err_t   rt_timer_control(rt_timer_t timer, int cmd, void *arg);

/* ---- Message queues ---------------------------------------------------------- */
rt_err_t rt_mq_init(rt_mq_t mq, const char *name, void *msgpool,
                    rt_size_t msg_size, rt_size_t pool_size, rt_uint8_t flag);
rt_err_t rt_mq_send(rt_mq_t mq, void *buffer, rt_size_t size);
rt_err_t rt_mq_recv(rt_mq_t mq, void *buffer, rt_size_t size, rt_int32_t timeout);
rt_err_t rt_mq_detach(rt_mq_t mq);
rt_err_t rt_mq_delete(rt_mq_t mq);

/* ---- Tick -------------------------------------------------------------------- */
static inline rt_tick_t rt_tick_get(void)
{
    return (rt_tick_t)xTaskGetTickCount();
}

/* ---- Interrupt enable/disable (save/restore mask) ----------------------------
 * Used by the vendor atomic/spinlock helpers. Implemented via the RISC-V mstatus
 * CSR so it works in both task and ISR context (single-core IPRO7). MIE = bit 3. */
static inline rt_base_t rt_hw_interrupt_disable(void)
{
    rt_base_t old;
    __asm volatile("csrrci %0, mstatus, 0x8" : "=r"(old));
    return old;
}

static inline void rt_hw_interrupt_enable(rt_base_t level)
{
    if (level & 0x8) {
        __asm volatile("csrsi mstatus, 0x8");
    }
}

/* ---- Scheduler critical section (used by spinlock macros) -------------------- */
static inline void rt_enter_critical(void) { vTaskSuspendAll(); }
static inline void rt_exit_critical(void)  { (void)xTaskResumeAll(); }

#ifdef __cplusplus
}
#endif

#endif /* ATBM_RTTHREAD_COMPAT_H */
