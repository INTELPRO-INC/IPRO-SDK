/* SPDX-License-Identifier: Apache-2.0 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <timers.h>

/* IPRO platform and Zephyr expose equivalent compiler-hint macros. Keep the
 * Zephyr definitions visible to pristine Host sources without Werror noise. */
#undef STRINGIFY
#undef likely
#undef unlikely

#include <zephyr/kernel.h>

#include "ipro_log.h"

#define IPRO_LOG_TAG "BT_KERNEL"

struct k_work_q k_sys_work_q;

#define IPRO_WORKQ_STATIC_SLOTS 6U
#define IPRO_SYS_WORKQ_STACK_WORDS 512U
#define IPRO_WORKQ_SLOTS_MAGIC 0x51574b31u
#define IPRO_QUEUE_MAGIC 0x51495545u
#define IPRO_WORKQ_MAGIC 0x57514d47u

struct ipro_workq_static_slot {
	struct k_work_q *queue;
	StaticTask_t tcb;
};

static struct ipro_workq_static_slot workq_static_slots[IPRO_WORKQ_STATIC_SLOTS];
static StackType_t sys_workq_stack[IPRO_SYS_WORKQ_STACK_WORDS];
static uint32_t workq_static_slots_magic;
static volatile void *s_workq_current_work;
static volatile void *s_workq_current_handler;

struct sched_lock_state {
	TaskHandle_t task;
	UBaseType_t original_priority;
	unsigned int nesting;
};

static struct sched_lock_state sched_locks[8];

static void ensure_workq_slots(void)
{
	taskENTER_CRITICAL();
	if (workq_static_slots_magic != IPRO_WORKQ_SLOTS_MAGIC) {
		memset(workq_static_slots, 0, sizeof(workq_static_slots));
		workq_static_slots_magic = IPRO_WORKQ_SLOTS_MAGIC;
	}
	taskEXIT_CRITICAL();
}

unsigned int irq_lock(void)
{
	taskENTER_CRITICAL();
	return 0;
}

void irq_unlock(unsigned int key)
{
	(void)key;
	taskEXIT_CRITICAL();
}

static void work_queue_main(void *arg)
{
	struct k_work_q *queue = arg;
	struct k_work *work;

	for (;;) {
		work = k_fifo_get(&queue->fifo, K_FOREVER);
		if (work != NULL) {
			bool cancelled;

			taskENTER_CRITICAL();
			cancelled = (work->flags & K_WORK_CANCELLED) != 0U;
			work->flags &= ~(K_WORK_QUEUED | K_WORK_CANCELLED);
			if (!cancelled) {
				work->flags |= K_WORK_RUNNING;
			}
			taskEXIT_CRITICAL();
			if (!cancelled) {
				s_workq_current_work = work;
				s_workq_current_handler = work->handler;
				work->handler(work);
				s_workq_current_handler = NULL;
				s_workq_current_work = NULL;
				taskENTER_CRITICAL();
				work->flags &= ~K_WORK_RUNNING;
				taskEXIT_CRITICAL();
			}
		}
		taskYIELD();
	}
}

static TickType_t timeout_ticks(k_timeout_t timeout)
{
	if (timeout == K_FOREVER) {
		return portMAX_DELAY;
	}
	return pdMS_TO_TICKS((uint32_t)timeout);
}

/*
 * Deliberately printf, not IPRO_LOG*: k_sem_give() is legal from an ISR (this
 * port even exports k_is_in_isr()), so this can run in interrupt context, and
 * ipro_log_write() takes a mutex. The two workqueue diagnostics further down
 * are task-context only and do use the logger.
 */
static bool semaphore_handle_is_valid(const char *op, void *handle, const void *owner)
{
	UBaseType_t item_size;

	if (handle == NULL) {
		printf("[ipro_bt_host] %s null semaphore owner=%p\r\n", op, owner);
		return false;
	}

	item_size = uxQueueGetQueueItemSize((QueueHandle_t)handle);
	if (item_size != 0U) {
		printf("[ipro_bt_host] %s invalid semaphore owner=%p handle=%p item_size=%lu\r\n",
		       op, owner, handle, (unsigned long)item_size);
		return false;
	}

	return true;
}

static void ensure_queue(struct k_queue *queue)
{
	if (queue->magic == IPRO_QUEUE_MAGIC && queue->initialized &&
	    queue->sem != NULL) {
		return;
	}
	taskENTER_CRITICAL();
	if (queue->magic != IPRO_QUEUE_MAGIC || !queue->initialized ||
	    queue->sem == NULL) {
		sys_slist_init(&queue->data_q);
		queue->sem = xSemaphoreCreateCountingStatic(UINT16_MAX, 0,
							    &queue->sem_buf);
		queue->magic = IPRO_QUEUE_MAGIC;
		queue->initialized = (queue->sem != NULL);
	}
	taskEXIT_CRITICAL();
}

void k_lifo_init(struct k_lifo *lifo)
{
	sys_slist_init(&lifo->queue.data_q);
	lifo->queue.sem = xSemaphoreCreateCountingStatic(UINT16_MAX, 0,
							 &lifo->queue.sem_buf);
	lifo->queue.magic = IPRO_QUEUE_MAGIC;
	lifo->queue.initialized = (lifo->queue.sem != NULL);
}

void k_fifo_init(struct k_fifo *fifo)
{
	sys_slist_init(&fifo->queue.data_q);
	fifo->queue.sem = xSemaphoreCreateCountingStatic(UINT16_MAX, 0,
							 &fifo->queue.sem_buf);
	fifo->queue.magic = IPRO_QUEUE_MAGIC;
	fifo->queue.initialized = (fifo->queue.sem != NULL);
}

void k_fifo_put(struct k_fifo *fifo, void *data)
{
	ensure_queue(&fifo->queue);
	if (fifo->queue.sem == NULL) {
		return;
	}
	taskENTER_CRITICAL();
	sys_slist_append(&fifo->queue.data_q, data);
	taskEXIT_CRITICAL();
	if (!semaphore_handle_is_valid("fifo_put", fifo->queue.sem, fifo)) {
		return;
	}
	(void)xSemaphoreGive((SemaphoreHandle_t)fifo->queue.sem);
}

void *k_fifo_get(struct k_fifo *fifo, k_timeout_t timeout)
{
	sys_snode_t *node;

	ensure_queue(&fifo->queue);
	if (fifo->queue.sem == NULL) {
		return NULL;
	}
	if (xSemaphoreTake((SemaphoreHandle_t)fifo->queue.sem,
			   timeout_ticks(timeout)) != pdPASS) {
		return NULL;
	}
	taskENTER_CRITICAL();
	node = sys_slist_get(&fifo->queue.data_q);
	taskEXIT_CRITICAL();
	return node;
}

void *k_fifo_peek_head(struct k_fifo *fifo)
{
	sys_snode_t *node;

	ensure_queue(&fifo->queue);
	taskENTER_CRITICAL();
	node = sys_slist_peek_head(&fifo->queue.data_q);
	taskEXIT_CRITICAL();
	return node;
}

bool k_fifo_is_empty(const void *fifo_ptr)
{
	struct k_fifo *fifo = (struct k_fifo *)fifo_ptr;

	ensure_queue(&fifo->queue);
	return sys_slist_is_empty(&fifo->queue.data_q);
}

int k_is_in_isr(void)
{
	return xPortIsInsideInterrupt() != pdFALSE;
}

void k_sched_lock(void)
{
	TaskHandle_t current = xTaskGetCurrentTaskHandle();
	struct sched_lock_state *free_slot = NULL;

	taskENTER_CRITICAL();
	for (size_t i = 0; i < ARRAY_SIZE(sched_locks); i++) {
		if (sched_locks[i].task == current) {
			sched_locks[i].nesting++;
			taskEXIT_CRITICAL();
			return;
		}
		if (free_slot == NULL && sched_locks[i].task == NULL) {
			free_slot = &sched_locks[i];
		}
	}
	if (free_slot != NULL) {
		free_slot->task = current;
		free_slot->original_priority = uxTaskPriorityGet(current);
		free_slot->nesting = 1;
		vTaskPrioritySet(current, configMAX_PRIORITIES - 1U);
	}
	taskEXIT_CRITICAL();
}

void k_sched_unlock(void)
{
	TaskHandle_t current = xTaskGetCurrentTaskHandle();

	taskENTER_CRITICAL();
	for (size_t i = 0; i < ARRAY_SIZE(sched_locks); i++) {
		if (sched_locks[i].task != current) {
			continue;
		}
		if (--sched_locks[i].nesting == 0U) {
			UBaseType_t priority = sched_locks[i].original_priority;

			sched_locks[i].task = NULL;
			vTaskPrioritySet(current, priority);
		}
		break;
	}
	taskEXIT_CRITICAL();
}

void k_oops(void)
{
	TaskHandle_t task = xTaskGetCurrentTaskHandle();
	const char *name = task != NULL ? pcTaskGetName(task) : "<none>";

	/* Same reason as assert_post_action(): this is the fatal path. */
	printf("\r\n[ipro_bt_host] k_oops task=%s handle=%p work=%p handler=%p ra=%p\r\n",
	       name, task, s_workq_current_work, s_workq_current_handler,
	       __builtin_return_address(0));
	abort();
}

int k_sem_init(struct k_sem *sem, unsigned int initial_count,
	       unsigned int limit)
{
	sem->hdl = xSemaphoreCreateCountingStatic(limit, initial_count,
						  &sem->sem_buf);
	sem->limit = limit;
	sem->initial = initial_count;
	return sem->hdl != NULL ? 0 : -ENOMEM;
}

static int ensure_sem(struct k_sem *sem)
{
	unsigned int limit;

	if (sem->hdl != NULL) {
		return 0;
	}
	limit = sem->limit;
	if (limit > (unsigned int)UINT16_MAX) {
		limit = UINT16_MAX;
	}
	return k_sem_init(sem, sem->initial, limit);
}

int k_sem_take(struct k_sem *sem, k_timeout_t timeout)
{
	if (ensure_sem(sem) != 0) {
		return -ENOMEM;
	}
	return xSemaphoreTake((SemaphoreHandle_t)sem->hdl,
			      timeout_ticks(timeout)) == pdPASS ? 0 : -EAGAIN;
}

void k_sem_give(struct k_sem *sem)
{
	if (ensure_sem(sem) != 0) {
		return;
	}
	if (!semaphore_handle_is_valid("sem_give", sem->hdl, sem)) {
		return;
	}
	(void)xSemaphoreGive((SemaphoreHandle_t)sem->hdl);
}

unsigned int k_sem_count_get(struct k_sem *sem)
{
	if (ensure_sem(sem) != 0) {
		return 0;
	}
	return uxSemaphoreGetCount((SemaphoreHandle_t)sem->hdl);
}

int k_mutex_init(struct k_mutex *mutex)
{
    mutex->hdl = NULL;
	mutex->lock_count = 0U;
	return 0;
}

int k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout)
{
	BaseType_t result;

	if (mutex->hdl == NULL) {
		mutex->hdl = xSemaphoreCreateRecursiveMutexStatic(&mutex->sem_buf);
		if (mutex->hdl == NULL) {
			return -ENOMEM;
		}
	}
	result = xSemaphoreTakeRecursive((SemaphoreHandle_t)mutex->hdl,
					timeout_ticks(timeout));
	if (result == pdPASS) {
		taskENTER_CRITICAL();
		mutex->lock_count++;
		taskEXIT_CRITICAL();
		return 0;
	}
	return -EAGAIN;
}

int k_mutex_unlock(struct k_mutex *mutex)
{
	if (xSemaphoreGiveRecursive((SemaphoreHandle_t)mutex->hdl) != pdPASS) {
		return -EPERM;
	}
	taskENTER_CRITICAL();
	if (mutex->lock_count > 0U) {
		mutex->lock_count--;
	}
	taskEXIT_CRITICAL();
	return 0;
}

int k_condvar_wait(struct k_condvar *condvar, struct k_mutex *mutex,
		   k_timeout_t timeout)
{
	int err;

	if (condvar->hdl == NULL) {
		condvar->hdl = xSemaphoreCreateBinaryStatic(&condvar->sem_buf);
		if (condvar->hdl == NULL) {
			return -ENOMEM;
		}
	}
	(void)k_mutex_unlock(mutex);
	err = xSemaphoreTake((SemaphoreHandle_t)condvar->hdl,
			     timeout_ticks(timeout)) == pdPASS ? 0 : -EAGAIN;
	(void)k_mutex_lock(mutex, K_FOREVER);
	return err;
}

int k_condvar_broadcast(struct k_condvar *condvar)
{
	if (condvar->hdl == NULL) {
		condvar->hdl = xSemaphoreCreateBinaryStatic(&condvar->sem_buf);
		if (condvar->hdl == NULL) {
			return -ENOMEM;
		}
	}
	if (!semaphore_handle_is_valid("condvar_broadcast", condvar->hdl, condvar)) {
		return -EINVAL;
	}
	return xSemaphoreGive((SemaphoreHandle_t)condvar->hdl) == pdPASS ? 0 : -EAGAIN;
}

int k_mem_slab_alloc(struct k_mem_slab *slab, void **memory,
		     k_timeout_t timeout)
{
	TickType_t deadline = xTaskGetTickCount() + timeout_ticks(timeout);

	for (;;) {
		uint32_t mask;
		unsigned int index;

		taskENTER_CRITICAL();
		mask = slab->free_mask;
		if (mask != 0U) {
			index = (unsigned int)__builtin_ctz(mask);
			slab->free_mask &= ~BIT(index);
			taskEXIT_CRITICAL();
			*memory = slab->buffer + (index * slab->block_size);
			return 0;
		}
		taskEXIT_CRITICAL();
		if (timeout == K_NO_WAIT ||
		    (timeout != K_FOREVER && (int32_t)(deadline - xTaskGetTickCount()) <= 0)) {
			return -ENOMEM;
		}
		vTaskDelay(1);
	}
}

void k_mem_slab_free(struct k_mem_slab *slab, void *memory)
{
	uintptr_t offset = (uintptr_t)memory - (uintptr_t)slab->buffer;
	unsigned int index = (unsigned int)(offset / slab->block_size);

	taskENTER_CRITICAL();
	slab->free_mask |= BIT(index);
	taskEXIT_CRITICAL();
}

void k_sleep(k_timeout_t timeout)
{
	vTaskDelay(timeout_ticks(timeout));
}

k_tid_t k_current_get(void)
{
	TaskHandle_t current = xTaskGetCurrentTaskHandle();

	/* Zephyr thread identity is the address of struct k_thread, not the
	 * underlying RTOS handle. hci_core uses this exact comparison to detect a
	 * synchronous command running on the system workqueue and avoid queueing
	 * TX work behind itself (which otherwise deadlocks for ten seconds). */
	if (current == (TaskHandle_t)k_sys_work_q.thread.hdl) {
		return &k_sys_work_q.thread;
	}

	return (k_tid_t)current;
}

void k_yield(void)
{
	taskYIELD();
}

void k_thread_name_set(struct k_thread *thread, const char *name)
{
	(void)thread;
	(void)name;
}

void k_thread_abort(struct k_thread *thread)
{
	if (thread->hdl != NULL) {
		vTaskDelete((TaskHandle_t)thread->hdl);
		thread->hdl = NULL;
	}
}

void k_work_init(struct k_work *work, k_work_handler_t handler)
{
	work->node.next = NULL;
	work->queue = NULL;
	work->handler = handler;
	work->flags = 0;
}

void k_work_queue_init(struct k_work_q *queue)
{
	sys_slist_init(&queue->fifo.queue.data_q);
	queue->fifo.queue.initialized = false;
	queue->fifo.queue.sem = NULL;
	queue->thread.hdl = NULL;
	queue->magic = IPRO_WORKQ_MAGIC;
}

void k_work_queue_start(struct k_work_q *queue, k_thread_stack_t *stack,
			size_t stack_size, int prio,
			const struct k_work_queue_config *cfg)
{
	const char *name = cfg != NULL && cfg->name != NULL ? cfg->name : "bt_work";
	struct ipro_workq_static_slot *slot = NULL;
	size_t stack_words = (stack_size + sizeof(StackType_t) - 1U) / sizeof(StackType_t);
	StackType_t *freertos_stack = (StackType_t *)stack;

	if (queue->magic != IPRO_WORKQ_MAGIC) {
		k_work_queue_init(queue);
	}
	if (queue->fifo.queue.magic != IPRO_QUEUE_MAGIC ||
	    !queue->fifo.queue.initialized) {
		k_fifo_init(&queue->fifo);
	}
	if (queue->thread.hdl != NULL) {
		return;
	}
	ensure_workq_slots();
	taskENTER_CRITICAL();
	for (size_t i = 0; i < IPRO_WORKQ_STATIC_SLOTS; i++) {
		if (workq_static_slots[i].queue == queue) {
			slot = &workq_static_slots[i];
			break;
		}
		if (slot == NULL && workq_static_slots[i].queue == NULL) {
			slot = &workq_static_slots[i];
			workq_static_slots[i].queue = queue;
			break;
		}
	}
	taskEXIT_CRITICAL();
	if (freertos_stack == NULL) {
		freertos_stack = sys_workq_stack;
		if (stack_words > IPRO_SYS_WORKQ_STACK_WORDS) {
			IPRO_LOGW(IPRO_LOG_TAG,
				  "workqueue %s stack request %lu words exceeds fallback %u words",
				  name, (unsigned long)stack_words, (unsigned)IPRO_SYS_WORKQ_STACK_WORDS);
			stack_words = IPRO_SYS_WORKQ_STACK_WORDS;
		}
	}
	if (slot != NULL) {
		queue->thread.hdl = xTaskCreateStatic(work_queue_main, name,
						      (uint32_t)stack_words, queue,
						      (UBaseType_t)prio,
						      freertos_stack, &slot->tcb);
	}
	if (queue->thread.hdl == NULL) {
		IPRO_LOGE(IPRO_LOG_TAG,
			  "workqueue %s create failed slot=%p stack=%p words=%lu prio=%d",
			  name, (void *)slot, (void *)freertos_stack,
			  (unsigned long)stack_words, prio);
	}
}

static void ensure_system_work_queue(void)
{
	static const struct k_work_queue_config cfg = { .name = "bt_sys_work" };

	if (k_sys_work_q.magic != IPRO_WORKQ_MAGIC ||
	    k_sys_work_q.thread.hdl == NULL) {
		k_work_queue_init(&k_sys_work_q);
		k_work_queue_start(&k_sys_work_q, NULL, 2048, 6, &cfg);
	}
}

int k_work_submit_to_queue(struct k_work_q *queue, struct k_work *work)
{
	int queued = 0;

	if (queue == NULL) {
		queue = &k_sys_work_q;
	}
	if (queue == &k_sys_work_q) {
		ensure_system_work_queue();
	}
	taskENTER_CRITICAL();
	if ((work->flags & K_WORK_QUEUED) == 0U) {
		work->flags |= K_WORK_QUEUED;
		work->queue = queue;
		queued = 1;
	}
	taskEXIT_CRITICAL();
	if (queued != 0) {
		if (queue->thread.hdl == NULL) {
			taskENTER_CRITICAL();
			work->flags &= ~K_WORK_QUEUED;
			work->queue = NULL;
			taskEXIT_CRITICAL();
			return -EIO;
		}
		k_fifo_put(&queue->fifo, work);
	}
	return queued;
}

int k_work_submit(struct k_work *work)
{
	return k_work_submit_to_queue(&k_sys_work_q, work);
}

int k_work_cancel(struct k_work *work)
{
	int was_queued;

	/* Do not unlink a FIFO node after its semaphore has potentially been
	 * consumed by the worker.  Mark it instead: the worker atomically removes
	 * the node and skips the handler, preserving FIFO/semaphore accounting. */
	taskENTER_CRITICAL();
	was_queued = (work->flags & K_WORK_QUEUED) != 0U;
	if (was_queued != 0) {
		work->flags |= K_WORK_CANCELLED;
	}
	taskEXIT_CRITICAL();

	return was_queued;
}

bool k_work_cancel_sync(struct k_work *work, struct k_work_sync *sync)
{
	bool was_busy;

	(void)sync;
	taskENTER_CRITICAL();
	was_busy = (work->flags & (K_WORK_QUEUED | K_WORK_RUNNING)) != 0U;
	taskEXIT_CRITICAL();
	(void)k_work_cancel(work);

	/* A handler cannot synchronously wait for itself.  In every other context,
	 * wait until a dequeued cancellation or an already-running handler has
	 * completed before RFCOMM reinitializes the DLC object. */
	if (s_workq_current_work != work) {
		while ((work->flags & (K_WORK_QUEUED | K_WORK_RUNNING)) != 0U) {
			vTaskDelay(1);
		}
	}

	return was_busy;
}

static void delayable_timeout(TimerHandle_t timer)
{
	struct k_work_delayable *dwork = pvTimerGetTimerID(timer);

	taskENTER_CRITICAL();
	dwork->work.flags &= ~K_WORK_DELAYED;
	taskEXIT_CRITICAL();
	(void)k_work_submit_to_queue((struct k_work_q *)dwork->queue,
				     &dwork->work);
}

void k_work_init_delayable(struct k_work_delayable *dwork,
			   k_work_handler_t handler)
{
	k_work_init(&dwork->work, handler);
	dwork->timer = NULL;
	dwork->queue = &k_sys_work_q;
}

static int delayable_start(struct k_work_delayable *dwork, k_timeout_t delay,
			   bool replace)
{
	TickType_t ticks;

	if (delay == K_NO_WAIT) {
		return k_work_submit_to_queue((struct k_work_q *)dwork->queue,
					      &dwork->work);
	}
	if (!replace && (dwork->work.flags & (K_WORK_DELAYED | K_WORK_QUEUED))) {
		return 0;
	}
	ticks = timeout_ticks(delay);
	if (ticks == 0) {
		ticks = 1;
	}
	if (dwork->timer == NULL) {
		dwork->timer = xTimerCreate("bt_delay", ticks, pdFALSE, dwork,
					  delayable_timeout);
		if (dwork->timer == NULL) {
			return -ENOMEM;
		}
	}
	if (xTimerChangePeriod((TimerHandle_t)dwork->timer, ticks,
			       portMAX_DELAY) != pdPASS) {
		return -EAGAIN;
	}
	taskENTER_CRITICAL();
	dwork->work.flags |= K_WORK_DELAYED;
	taskEXIT_CRITICAL();
	return 1;
}

int k_work_schedule(struct k_work_delayable *dwork, k_timeout_t delay)
{
	return delayable_start(dwork, delay, false);
}

int k_work_reschedule(struct k_work_delayable *dwork, k_timeout_t delay)
{
	return delayable_start(dwork, delay, true);
}

int k_work_cancel_delayable(struct k_work_delayable *dwork)
{
	if (dwork->timer != NULL) {
		(void)xTimerStop((TimerHandle_t)dwork->timer, portMAX_DELAY);
	}
	taskENTER_CRITICAL();
	dwork->work.flags &= ~K_WORK_DELAYED;
	taskEXIT_CRITICAL();
	(void)k_work_cancel(&dwork->work);
	return 0;
}

bool k_work_cancel_delayable_sync(struct k_work_delayable *dwork,
				  struct k_work_sync *sync)
{
	bool was_pending;

	taskENTER_CRITICAL();
	was_pending = (dwork->work.flags & (K_WORK_DELAYED | K_WORK_QUEUED)) != 0U;
	taskEXIT_CRITICAL();

	(void)k_work_cancel_delayable(dwork);
	(void)k_work_flush(&dwork->work, sync);

	return was_pending;
}

int k_work_delayable_busy_get(const struct k_work_delayable *dwork)
{
	return (int)dwork->work.flags;
}

uint32_t k_work_delayable_remaining_get(const struct k_work_delayable *dwork)
{
	TickType_t now;
	TickType_t expiry;
	TickType_t remaining;

	if (dwork->timer == NULL ||
	    xTimerIsTimerActive((TimerHandle_t)dwork->timer) == pdFALSE) {
		return 0U;
	}

	now = xTaskGetTickCount();
	expiry = xTimerGetExpiryTime((TimerHandle_t)dwork->timer);
	remaining = expiry - now;
	if ((int32_t)remaining <= 0) {
		return 0U;
	}

	return (uint32_t)remaining;
}

bool k_work_flush(struct k_work *work, struct k_work_sync *sync)
{
	(void)sync;
	if (s_workq_current_work == work) {
		return false;
	}
	while ((work->flags & (K_WORK_QUEUED | K_WORK_RUNNING)) != 0U) {
		vTaskDelay(1);
	}
	return true;
}

void k_queue_prepend(struct k_queue *queue, void *data)
{
	ensure_queue(queue);
	taskENTER_CRITICAL();
	sys_slist_prepend(&queue->data_q, data);
	taskEXIT_CRITICAL();
	if (!semaphore_handle_is_valid("queue_prepend", queue->sem, queue)) {
		return;
	}
	(void)xSemaphoreGive((SemaphoreHandle_t)queue->sem);
}

void *k_lifo_get(struct k_lifo *lifo, k_timeout_t timeout)
{
	sys_snode_t *node;

	ensure_queue(&lifo->queue);
	if (xSemaphoreTake((SemaphoreHandle_t)lifo->queue.sem,
			   timeout_ticks(timeout)) != pdPASS) {
		return NULL;
	}
	taskENTER_CRITICAL();
	node = sys_slist_get(&lifo->queue.data_q);
	taskEXIT_CRITICAL();
	return node;
}

k_spinlock_key_t k_spin_lock(struct k_spinlock *lock)
{
	(void)lock;
	taskENTER_CRITICAL();
	return 0;
}

void k_spin_unlock(struct k_spinlock *lock, k_spinlock_key_t key)
{
	(void)lock;
	(void)key;
	taskEXIT_CRITICAL();
}

k_timepoint_t sys_timepoint_calc(k_timeout_t timeout)
{
	if (timeout == K_FOREVER) {
		return UINT32_MAX;
	}
	return (k_timepoint_t)(xTaskGetTickCount() + timeout_ticks(timeout));
}

k_timeout_t sys_timepoint_timeout(k_timepoint_t timepoint)
{
	TickType_t now;
	TickType_t remaining;

	if (timepoint == UINT32_MAX) {
		return K_FOREVER;
	}
	now = xTaskGetTickCount();
	remaining = (TickType_t)(timepoint - now);
	if ((int32_t)remaining <= 0) {
		return K_NO_WAIT;
	}
	return (k_timeout_t)(remaining * portTICK_PERIOD_MS);
}

void *k_heap_alloc(struct k_heap *heap, size_t bytes, k_timeout_t timeout)
{
	(void)heap;
	(void)timeout;
	return pvPortMalloc(bytes);
}

void *k_heap_aligned_alloc(struct k_heap *heap, size_t align, size_t bytes,
			   k_timeout_t timeout)
{
	/* The current IPRO heap guarantees at least pointer alignment. Larger
	 * alignment is not used by the Bluetooth pools selected for this port. */
	if (align > sizeof(void *)) {
		return NULL;
	}
	return k_heap_alloc(heap, bytes, timeout);
}

void k_heap_free(struct k_heap *heap, void *memory)
{
	(void)heap;
	vPortFree(memory);
}
