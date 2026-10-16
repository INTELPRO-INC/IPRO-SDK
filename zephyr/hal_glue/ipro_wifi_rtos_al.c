/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * The SDK Wi-Fi stack's RTOS abstraction (components/wireless/wifi6/
 * ipro_os_adapter/include/rtos_al.h, written for FreeRTOS) on the Zephyr
 * kernel: tasks are dynamic k_threads with a per-task notification
 * semaphore (FreeRTOS "task notification"), queues are k_msgq, semaphores
 * k_sem, mutexes k_mutex; rtos_protect() is a nested irq_lock().
 *
 * FreeRTOS priorities count upward (idle 0, the stack uses 2..30); Zephyr
 * preemptive priorities count downward, so prio p becomes
 * K_PRIO_PREEMPT(RTOS_PRIO_TOP - p) (CONFIG_NUM_PREEMPT_PRIORITIES must be
 * at least RTOS_PRIO_TOP + 1). Stack depths are FreeRTOS words (4 bytes).
 */
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include "rtos_def.h"
#include "rtos_al.h"

LOG_MODULE_REGISTER(ipro_wifi_rtos, CONFIG_LOG_DEFAULT_LEVEL);

#define RTOS_PRIO_TOP 31
BUILD_ASSERT(CONFIG_NUM_PREEMPT_PRIORITIES > RTOS_PRIO_TOP,
	     "CONFIG_NUM_PREEMPT_PRIORITIES must exceed the Wi-Fi stack's FreeRTOS priorities (31)");
BUILD_ASSERT(CONFIG_COMMON_LIBC_MALLOC_ARENA_SIZE >= 32768,
	     "the Wi-Fi stack needs a libc malloc arena of at least 32 KB");

/* One heap for everything the stack allocates: the MAC's export_malloc.c
 * calls libc malloc() and the fhost frees those buffers with rtos_free(), so
 * rtos_malloc()/rtos_free() must be libc malloc()/free() (a dedicated kernel
 * heap here corrupted both). Size it with CONFIG_COMMON_LIBC_MALLOC_ARENA_SIZE. */
#include <stdlib.h>

/* The FreeRTOS task priorities the SDK's rtos_al.c publishes. */
const int fhost_tcpip_priority = RTOS_TASK_PRIORITY(28);
const int fhost_wifi_priority = RTOS_TASK_PRIORITY(27);
const int fhost_wifi_priority_high = RTOS_TASK_PRIORITY(30);
const int fhost_cntrl_priority = RTOS_TASK_PRIORITY(27);
const int fhost_rx_priority = RTOS_TASK_PRIORITY(28);
const int fhost_tx_priority = RTOS_TASK_PRIORITY(29);
const int fhost_wpa_priority = RTOS_TASK_PRIORITY(26);
const int fhost_ipc_priority = RTOS_TASK_PRIORITY(29);
const int fhost_iperf_priority = RTOS_TASK_PRIORITY(27);
const int fhost_connect_priority = RTOS_TASK_PRIORITY(2);
const int fhost_tg_priority = RTOS_TASK_PRIORITY(5);
const int fhost_ping_priority = RTOS_TASK_PRIORITY(27);

struct freertos_shim_task {
	struct k_thread thread;
	k_thread_stack_t *stack;
	struct k_sem notify;
	char name[16];
	bool foreign; /* a Zephyr thread that was not created here */
	unsigned int crit_key;
	int crit_depth;
	sys_snode_t zombie; /* on `zombies` once it deleted itself */
};

/* A task that deletes itself cannot free the stack it runs on: it is parked
 * here and freed by the next rtos_task_create() once the thread is dead.
 * Every connect attempt creates and ends two tasks (CONNECT_ENTRY and
 * CONNECT, 3 KB of stack each), so without this the kernel heap ran out
 * after four or five attempts in one boot. */
static sys_slist_t zombies = SYS_SLIST_STATIC_INIT(&zombies);
static struct k_spinlock zombie_lock;

static void reap_zombies(void)
{
	for (;;) {
		struct freertos_shim_task *dead = NULL;
		sys_snode_t *n, *prev = NULL;
		k_spinlock_key_t key = k_spin_lock(&zombie_lock);

		SYS_SLIST_FOR_EACH_NODE(&zombies, n) {
			struct freertos_shim_task *z = CONTAINER_OF(n, struct freertos_shim_task, zombie);

			if (k_thread_join(&z->thread, K_NO_WAIT) == 0) {
				sys_slist_remove(&zombies, prev, n);
				dead = z;
				break;
			}
			prev = n;
		}
		k_spin_unlock(&zombie_lock, key);
		if (dead == NULL) {
			return;
		}
		k_thread_stack_free(dead->stack);
		free(dead);
	}
}

static int zprio(rtos_prio prio)
{
	int p = RTOS_PRIO_TOP - (int)prio;

	return K_PRIO_PREEMPT(CLAMP(p, 0, CONFIG_NUM_PREEMPT_PRIORITIES - 1));
}

static struct freertos_shim_task *task_of(k_tid_t tid)
{
	struct freertos_shim_task *t = k_thread_custom_data_get();

	if (tid == k_current_get() && t != NULL) {
		return t;
	}
	/* A thread not created through rtos_task_create() (e.g. the main
	 * thread waiting on a notification): give it a record on first use. */
	if (tid == k_current_get()) {
		t = malloc(sizeof(*t));
		if (t == NULL) {
			return NULL;
		}
		memset(t, 0, sizeof(*t));
		k_sem_init(&t->notify, 0, K_SEM_MAX_LIMIT);
		t->foreign = true;
		k_thread_custom_data_set(t);
		return t;
	}
	return (struct freertos_shim_task *)tid->custom_data;
}

/* ---- time ---------------------------------------------------------------- */

uint32_t rtos_al_ms2tick(int ms)
{
	return (uint32_t)ms; /* 1 kHz tick like the IPRO FreeRTOS port */
}

uint32_t rtos_now(bool isr)
{
	ARG_UNUSED(isr);
	return k_uptime_get_32();
}

static k_timeout_t timeout_of(int ms)
{
	return ms < 0 ? K_FOREVER : K_MSEC(ms);
}

/* ---- tasks --------------------------------------------------------------- */

static void task_entry(void *p1, void *p2, void *p3)
{
	struct freertos_shim_task *t = p1;
	void (*fn)(void *) = p2;

	k_thread_custom_data_set(t);
	fn(p3);
	/* A FreeRTOS task function never returns; if it does, delete it. */
	rtos_task_delete(NULL);
}

int rtos_task_create(rtos_task_fct func, const char *const name, enum rtos_task_id task_id,
		     const uint16_t stack_depth, void *const params, rtos_prio prio,
		     rtos_task_handle *const task_handle)
{
	ARG_UNUSED(task_id);
	size_t stack_bytes = (size_t)stack_depth * sizeof(uint32_t) + CONFIG_IPRO_WIFI_STACK_EXTRA;
	struct freertos_shim_task *t;

	reap_zombies();
	t = malloc(sizeof(*t));

	if (t == NULL) {
		LOG_ERR("no memory for task %s", name);
		return 1;
	}
	memset(t, 0, sizeof(*t));
	t->stack = k_thread_stack_alloc(stack_bytes, 0);
	if (t->stack == NULL) {
		LOG_ERR("no stack (%zu B) for task %s", stack_bytes, name);
		free(t);
		return 1;
	}
	k_sem_init(&t->notify, 0, K_SEM_MAX_LIMIT);
	strncpy(t->name, name != NULL ? name : "wifi", sizeof(t->name) - 1);
	/* Started only once the record is complete: the task may end (and be
	 * reaped by another task's rtos_task_create()) before we run again. */
	k_thread_create(&t->thread, t->stack, stack_bytes, task_entry, t, (void *)func, params,
			zprio(prio), 0, K_FOREVER);
	k_thread_name_set(&t->thread, t->name);
	if (task_handle != NULL) {
		*task_handle = t;
	}
	k_thread_start(&t->thread);
	return 0;
}

rtos_task_t rtos_task_get_handle(const char *task_name)
{
	ARG_UNUSED(task_name);
	return NULL;
}

void rtos_task_delete(rtos_task_handle task_handle)
{
	struct freertos_shim_task *t = task_handle;

	if (t == NULL) {
		t = task_of(k_current_get());
		if (t == NULL || t->foreign) {
			k_thread_abort(k_current_get());
			return;
		}
		/* Self-delete: the stack is still in use; reap_zombies() frees
		 * it once the thread is dead. */
		k_spinlock_key_t key = k_spin_lock(&zombie_lock);

		sys_slist_append(&zombies, &t->zombie);
		k_spin_unlock(&zombie_lock, key);
		k_thread_abort(&t->thread);
		return;
	}
	{
		k_spinlock_key_t key = k_spin_lock(&zombie_lock);

		(void)sys_slist_find_and_remove(&zombies, &t->zombie);
		k_spin_unlock(&zombie_lock, key);
	}
	k_thread_abort(&t->thread);
	k_thread_stack_free(t->stack);
	free(t);
}

void rtos_task_suspend(int duration)
{
	if (duration > 0) {
		k_msleep(duration);
	}
}

int rtos_task_init_notification(rtos_task_handle task)
{
	ARG_UNUSED(task);
	return 0;
}

int rtos_task_wait_notification(int timeout)
{
	struct freertos_shim_task *t = task_of(k_current_get());
	int n = 0;

	if (t == NULL) {
		return 0;
	}
	if (k_sem_take(&t->notify, timeout_of(timeout)) == 0) {
		n = 1;
		/* FreeRTOS ulTaskNotifyTake(clear=true) returns the count and
		 * zeroes it: drain any extra gives. */
		while (k_sem_take(&t->notify, K_NO_WAIT) == 0) {
			n++;
		}
	}
	return n;
}

void rtos_task_notify(rtos_task_handle task, bool isr)
{
	struct freertos_shim_task *t = task;

	ARG_UNUSED(isr);
	if (t != NULL) {
		k_sem_give(&t->notify);
	}
}

bool rtos_in_isr(void)
{
	return k_is_in_isr();
}

rtos_task_handle rtos_get_task_handle(void)
{
	return task_of(k_current_get());
}

void rtos_priority_set(rtos_task_handle handle, rtos_prio priority)
{
	struct freertos_shim_task *t = handle;

	k_thread_priority_set(t != NULL ? &t->thread : k_current_get(), zprio(priority));
}

/* ---- queues -------------------------------------------------------------- */

/* FreeRTOS builds semaphores and mutexes on top of queues, and the SDK
 * relies on that: eloop_rtos.c deletes a semaphore with rtos_queue_delete().
 * Every object the shim hands out therefore starts with a kind tag so the
 * delete functions can dispatch on what they were really given. */
enum shim_obj_kind {
	SHIM_OBJ_QUEUE,
	SHIM_OBJ_SEM,
	SHIM_OBJ_MUTEX,
};

struct shim_obj {
	enum shim_obj_kind kind;
	union {
		struct k_msgq msgq;
		struct k_sem sem;
		struct k_mutex mutex;
	};
};

static struct shim_obj *shim_obj_new(enum shim_obj_kind kind)
{
	struct shim_obj *o = malloc(sizeof(*o));

	if (o != NULL) {
		o->kind = kind;
	}
	return o;
}

static void shim_obj_delete(void *handle)
{
	struct shim_obj *o = handle;

	if (o == NULL) {
		return;
	}
	if (o->kind == SHIM_OBJ_QUEUE) {
		k_msgq_cleanup(&o->msgq);
	}
	free(o);
}

int rtos_queue_create(int elt_size, int nb_elt, rtos_queue *queue)
{
	struct shim_obj *o = shim_obj_new(SHIM_OBJ_QUEUE);

	if (o == NULL) {
		return -1;
	}
	if (k_msgq_alloc_init(&o->msgq, elt_size, nb_elt) != 0) {
		free(o);
		return -1;
	}
	*queue = &o->msgq;
	return 0;
}

void rtos_queue_delete(rtos_queue queue)
{
	shim_obj_delete(CONTAINER_OF(queue, struct shim_obj, msgq));
}

bool rtos_queue_is_empty(rtos_queue queue)
{
	return k_msgq_num_used_get(queue) == 0;
}

bool rtos_queue_is_full(rtos_queue queue)
{
	return k_msgq_num_free_get(queue) == 0;
}

int rtos_queue_cnt(rtos_queue queue)
{
	return (int)k_msgq_num_used_get(queue);
}

int rtos_queue_write(rtos_queue queue, void *msg, int timeout, bool isr)
{
	int r = k_msgq_put(queue, msg, isr ? K_NO_WAIT : timeout_of(timeout));

	return r != 0; /* 1 = full, like errQUEUE_FULL */
}

int rtos_queue_read(rtos_queue queue, void *msg, int timeout, bool isr)
{
	int r = k_msgq_get(queue, msg, isr ? K_NO_WAIT : timeout_of(timeout));

	return r != 0; /* 1 = empty */
}

/* ---- semaphores / mutexes ----------------------------------------------- */

int rtos_semaphore_create(rtos_semaphore *semaphore, int max_count, int init_count)
{
	struct shim_obj *o = shim_obj_new(SHIM_OBJ_SEM);

	if (o == NULL) {
		return -1;
	}
	k_sem_init(&o->sem, init_count, max_count);
	*semaphore = &o->sem;
	return 0;
}

void rtos_semaphore_delete(rtos_semaphore semaphore)
{
	shim_obj_delete(CONTAINER_OF(semaphore, struct shim_obj, sem));
}

int rtos_semaphore_get_count(rtos_semaphore semaphore)
{
	return (int)k_sem_count_get(semaphore);
}

int rtos_semaphore_wait(rtos_semaphore semaphore, int timeout)
{
	return k_sem_take(semaphore, timeout_of(timeout)) != 0;
}

int rtos_semaphore_signal(rtos_semaphore semaphore, bool isr)
{
	ARG_UNUSED(isr);
	k_sem_give(semaphore);
	return 0;
}

int rtos_mutex_create(rtos_mutex *mutex)
{
	struct shim_obj *o = shim_obj_new(SHIM_OBJ_MUTEX);

	if (o == NULL) {
		return -1;
	}
	k_mutex_init(&o->mutex);
	*mutex = &o->mutex;
	return 0;
}

void rtos_mutex_delete(rtos_mutex mutex)
{
	shim_obj_delete(CONTAINER_OF(mutex, struct shim_obj, mutex));
}

void rtos_mutex_lock(rtos_mutex mutex)
{
	(void)k_mutex_lock(mutex, K_FOREVER);
}

int rtos_mutex_lock_timeout(rtos_mutex mutex, int timeout_ms)
{
	return k_mutex_lock(mutex, K_MSEC(timeout_ms)) != 0;
}

void rtos_mutex_unlock(rtos_mutex mutex)
{
	(void)k_mutex_unlock(mutex);
}

/* ---- critical sections --------------------------------------------------- */

/* rtos_protect() hands its cookie back to rtos_unprotect(): pass the
 * irq_lock() key through. A global nesting counter cannot work here - a
 * FreeRTOS critical section is per task, and a section left open across a
 * blocking call in one thread must not keep interrupts off in the next. */
uint32_t rtos_protect(void)
{
	return irq_lock();
}

void rtos_unprotect(uint32_t protect)
{
	irq_unlock(protect);
}

/* FreeRTOS taskENTER_CRITICAL()/taskEXIT_CRITICAL() used by wifi_mgmr*.c:
 * nested, per thread (the key lives in the calling thread's record). */
void ipro_freertos_shim_enter_critical(void)
{
	unsigned int key = irq_lock();
	struct freertos_shim_task *t = task_of(k_current_get());

	if (t == NULL) {
		return;
	}
	if (t->crit_depth++ == 0) {
		t->crit_key = key;
	}
}

void ipro_freertos_shim_exit_critical(void)
{
	struct freertos_shim_task *t = task_of(k_current_get());

	if (t != NULL && t->crit_depth > 0 && --t->crit_depth == 0) {
		irq_unlock(t->crit_key);
	}
}

const char *pcTaskGetName(TaskHandle_t task)
{
	const char *name = k_thread_name_get(task != NULL ? &task->thread : k_current_get());

	return name != NULL ? name : "?";
}

/* ---- scheduler / memory -------------------------------------------------- */

void rtos_start_scheduler(void)
{
	/* Zephyr's scheduler is already running. */
}

int rtos_init(void)
{
	return 0;
}

void *rtos_malloc(uint32_t size)
{
	void *p = malloc(size);

	if (p == NULL) {
		LOG_ERR("rtos_malloc(%u) failed", size);
	}
	return p;
}

void *rtos_calloc(uint32_t nb_elt, uint32_t size)
{
	void *p = rtos_malloc(nb_elt * size);

	if (p != NULL) {
		memset(p, 0, nb_elt * size);
	}
	return p;
}

void rtos_free(void *ptr)
{
	free(ptr);
}

void rtos_heap_info(int *total_size, int *free_size, int *min_free_size)
{
	/* libc arena: no cheap free-space query; report the arena size. */
	*total_size = CONFIG_COMMON_LIBC_MALLOC_ARENA_SIZE;
	*free_size = 0;
	*min_free_size = 0;
}

void rtos_trace_task(int id, void *task)
{
	ARG_UNUSED(id); ARG_UNUSED(task);
}

void rtos_trace_mem(int id, void *ptr, int size, int free_size)
{
	ARG_UNUSED(id); ARG_UNUSED(ptr); ARG_UNUSED(size); ARG_UNUSED(free_size);
}
