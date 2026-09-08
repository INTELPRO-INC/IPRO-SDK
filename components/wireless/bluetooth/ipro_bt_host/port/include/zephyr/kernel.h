/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_ZEPHYR_KERNEL_H_
#define IPRO_BT_HOST_ZEPHYR_KERNEL_H_

/* IPRO owns this boundary. Do not include the legacy Zephyr compatibility
 * kernel here: it exports pre-namespaced misc/util macros which collide with
 * Zephyr 4.4. FreeRTOS primitives are introduced here module by module. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#ifndef ESHUTDOWN
#define ESHUTDOWN 108
#endif
#include <zephyr/sys/dlist.h>
#include <zephyr/sys/slist.h>
#include <zephyr/arch/common/ffs.h>
#include <FreeRTOS.h>
#include <semphr.h>

#ifndef __noinit
#define __noinit
#endif

typedef int32_t k_timeout_t;
typedef uint32_t k_timepoint_t;
typedef uintptr_t k_spinlock_key_t;
typedef void *k_tid_t;
typedef unsigned long k_thread_stack_t;

struct arch_esf;

#define K_NO_WAIT ((k_timeout_t)0)
#define K_FOREVER ((k_timeout_t)-1)
#define NSEC_PER_USEC 1000U
#define NSEC_PER_MSEC 1000000U
#define USEC_PER_MSEC 1000U
#define MSEC_PER_SEC 1000U
#define USEC_PER_SEC (USEC_PER_MSEC * MSEC_PER_SEC)
#define NSEC_PER_SEC (NSEC_PER_USEC * USEC_PER_SEC)
#define K_MSEC(ms) ((k_timeout_t)(ms))
#define K_USEC(us) ((k_timeout_t)((((uint32_t)(us)) + 999U) / 1000U))
#define K_SECONDS(s) K_MSEC((s) * 1000)
#define K_PRIO_COOP(prio) (prio)
#define K_TIMEOUT_EQ(a, b) ((a) == (b))

#define K_WORK_QUEUED  BIT(0)
#define K_WORK_DELAYED BIT(1)
#define K_WORK_RUNNING BIT(2)
#define K_WORK_CANCELLED BIT(3)

struct k_queue {
	sys_slist_t data_q;
	void *sem;
	StaticSemaphore_t sem_buf;
	uint32_t magic;
	bool initialized;
	sys_dlist_t poll_events;
};

struct k_lifo {
	struct k_queue queue;
};

struct k_fifo {
	union {
		struct k_queue queue;
		struct k_queue _queue;
	};
};

struct k_work;
typedef void (*k_work_handler_t)(struct k_work *work);

struct k_work {
	sys_snode_t node;
	void *queue;
	k_work_handler_t handler;
	uint32_t flags;
};

struct k_work_delayable {
	struct k_work work;
	void *timer;
	void *queue;
};

struct k_work_sync {
	void *sem;
};

struct k_thread {
	void *hdl;
};

struct k_work_q {
	struct k_fifo fifo;
	struct k_thread thread;
	uint32_t magic;
};

struct k_work_queue_config {
	const char *name;
	bool no_yield;
	bool essential;
};

extern struct k_work_q k_sys_work_q;

#ifndef POST_KERNEL
#define POST_KERNEL 0
#endif
#ifndef SYS_INIT
#define SYS_INIT(init_fn, level, prio)
#endif

#define K_KERNEL_STACK_DEFINE(name, size) \
	k_thread_stack_t name[((size) + sizeof(k_thread_stack_t) - 1U) / sizeof(k_thread_stack_t)]
#define K_THREAD_STACK_DEFINE(name, size) \
	k_thread_stack_t name[((size) + sizeof(k_thread_stack_t) - 1U) / sizeof(k_thread_stack_t)]
#define K_THREAD_STACK_SIZEOF(name) sizeof(name)
#define Z_WORK_INITIALIZER(handler_) \
	{ .node = { 0 }, .queue = NULL, .handler = (handler_), .flags = 0 }
#define K_WORK_DEFINE(name, handler_) \
	struct k_work name = Z_WORK_INITIALIZER(handler_)
#define Z_WORK_DELAYABLE_INITIALIZER(handler_) \
	{ .work = Z_WORK_INITIALIZER(handler_), .timer = NULL, .queue = NULL }
#define K_WORK_DELAYABLE_DEFINE(name, handler_) \
	struct k_work_delayable name = Z_WORK_DELAYABLE_INITIALIZER(handler_)
#define K_FIFO_DEFINE(name) struct k_fifo name = { 0 }
#define K_SEM_MAX_LIMIT UINT32_MAX
#define K_SEM_DEFINE(name, initial_count, count_limit) \
	struct k_sem name = { .hdl = NULL, .limit = (count_limit), \
			    .initial = (initial_count) }

struct k_spinlock {
	uintptr_t key;
};

struct k_heap {
	uintptr_t reserved;
};

struct k_sem {
	void *hdl;
	StaticSemaphore_t sem_buf;
	unsigned int limit;
	unsigned int initial;
};

struct k_mutex {
	void *hdl;
	StaticSemaphore_t sem_buf;
	uint32_t lock_count;
};

struct k_condvar {
	void *hdl;
	StaticSemaphore_t sem_buf;
};

struct k_mem_slab {
	uint8_t *buffer;
	size_t block_size;
	uint32_t num_blocks;
	uint32_t free_mask;
};

#define Z_MUTEX_INITIALIZER(obj) { .hdl = NULL, .lock_count = 0U }
#define Z_CONDVAR_INITIALIZER(obj) { .hdl = NULL }
#define K_MUTEX_DEFINE(name) \
	struct k_mutex name = Z_MUTEX_INITIALIZER(name)
#define K_MEM_SLAB_DEFINE_STATIC(name, size_, count_, align_) \
	static uint8_t name##_buffer[(size_) * (count_)] \
		__attribute__((aligned(align_))); \
	static struct k_mem_slab name = { \
		.buffer = name##_buffer, .block_size = (size_), \
		.num_blocks = (count_), \
		.free_mask = ((count_) >= 32 ? UINT32_MAX : ((1U << (count_)) - 1U)) \
	}

#define Z_LIFO_INITIALIZER(obj) \
	{ .queue = { .data_q = SYS_SLIST_STATIC_INIT(&(obj).queue.data_q) } }

void k_lifo_init(struct k_lifo *lifo);
void k_fifo_init(struct k_fifo *fifo);
void k_fifo_put(struct k_fifo *fifo, void *data);
void *k_fifo_get(struct k_fifo *fifo, k_timeout_t timeout);
void *k_fifo_peek_head(struct k_fifo *fifo);
bool k_fifo_is_empty(const void *fifo);
int k_is_in_isr(void);
void k_sched_lock(void);
void k_sched_unlock(void);
void k_oops(void) __attribute__((noreturn));
int k_sem_init(struct k_sem *sem, unsigned int initial_count,
	       unsigned int limit);
int k_sem_take(struct k_sem *sem, k_timeout_t timeout);
void k_sem_give(struct k_sem *sem);
unsigned int k_sem_count_get(struct k_sem *sem);
int k_mutex_init(struct k_mutex *mutex);
int k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout);
int k_mutex_unlock(struct k_mutex *mutex);
int k_condvar_wait(struct k_condvar *condvar, struct k_mutex *mutex,
		   k_timeout_t timeout);
int k_condvar_broadcast(struct k_condvar *condvar);
int k_mem_slab_alloc(struct k_mem_slab *slab, void **memory,
		     k_timeout_t timeout);
void k_mem_slab_free(struct k_mem_slab *slab, void *memory);
void k_sleep(k_timeout_t timeout);
k_tid_t k_current_get(void);
void k_yield(void);
void k_thread_name_set(struct k_thread *thread, const char *name);
void k_thread_abort(struct k_thread *thread);
void k_work_init(struct k_work *work, k_work_handler_t handler);
int k_work_submit(struct k_work *work);
int k_work_submit_to_queue(struct k_work_q *queue, struct k_work *work);
int k_work_cancel(struct k_work *work);
bool k_work_cancel_sync(struct k_work *work, struct k_work_sync *sync);
void k_work_init_delayable(struct k_work_delayable *dwork,
			   k_work_handler_t handler);
void k_work_queue_init(struct k_work_q *queue);
void k_work_queue_start(struct k_work_q *queue, k_thread_stack_t *stack,
			size_t stack_size, int prio,
			const struct k_work_queue_config *cfg);
int k_work_schedule(struct k_work_delayable *dwork, k_timeout_t delay);
int k_work_reschedule(struct k_work_delayable *dwork, k_timeout_t delay);
int k_work_cancel_delayable(struct k_work_delayable *dwork);
bool k_work_cancel_delayable_sync(struct k_work_delayable *dwork,
				  struct k_work_sync *sync);
int k_work_delayable_busy_get(const struct k_work_delayable *dwork);
uint32_t k_work_delayable_remaining_get(const struct k_work_delayable *dwork);
bool k_work_flush(struct k_work *work, struct k_work_sync *sync);

static inline bool k_work_is_pending(const struct k_work *work)
{
	return (work->flags & (K_WORK_QUEUED | K_WORK_DELAYED)) != 0U;
}

static inline k_tid_t k_work_queue_thread_get(struct k_work_q *queue)
{
	/* Zephyr k_tid_t identifies struct k_thread.  The FreeRTOS handle is an
	 * implementation detail stored in hdl and is only used by the port when
	 * mapping the currently running native task back to this object. */
	return &queue->thread;
}

static inline struct k_work_delayable *
k_work_delayable_from_work(struct k_work *work)
{
	return CONTAINER_OF(work, struct k_work_delayable, work);
}
void k_queue_prepend(struct k_queue *queue, void *data);
void *k_lifo_get(struct k_lifo *lifo, k_timeout_t timeout);
k_spinlock_key_t k_spin_lock(struct k_spinlock *lock);
void k_spin_unlock(struct k_spinlock *lock, k_spinlock_key_t key);
k_timepoint_t sys_timepoint_calc(k_timeout_t timeout);
k_timeout_t sys_timepoint_timeout(k_timepoint_t timepoint);
void *k_heap_alloc(struct k_heap *heap, size_t bytes, k_timeout_t timeout);
void *k_heap_aligned_alloc(struct k_heap *heap, size_t align, size_t bytes,
			   k_timeout_t timeout);
void k_heap_free(struct k_heap *heap, void *memory);

static inline void k_lifo_put(struct k_lifo *lifo, void *data)
{
	k_queue_prepend(&lifo->queue, data);
}

#endif /* IPRO_BT_HOST_ZEPHYR_KERNEL_H_ */
