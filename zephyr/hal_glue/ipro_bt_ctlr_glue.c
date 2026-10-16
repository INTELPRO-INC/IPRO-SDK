/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SDK side of the Bluetooth integration.
 *
 * 1. Facade (ipro_bt_ctlr_*) over the prebuilt IPRO7 BLE controller: RF
 *    front-end in BLE mode, controller reset/init (arch_main.c) and the
 *    in-memory H4 transport (hcitl).
 *
 * 2. FreeRTOS compatibility shim: the prebuilt controller's platform layer
 *    runs its scheduling loop as a FreeRTOS task driven by task
 *    notifications. Exactly six FreeRTOS symbols are imported by the archive;
 *    they are implemented here on Zephyr threads and semaphores with the
 *    FreeRTOS ABI (IPRO7 port: TickType_t = uint32_t, 1000 Hz tick,
 *    StackType_t = unsigned long). Nothing else of FreeRTOS exists in the
 *    image.
 *
 * 3. Stubs for the SDK low-power framework and SDK shell hooks the archive
 *    references: the Zephyr port does not enter PDS yet, so those paths are
 *    inert.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>
#include <ipro_zephyr/ipro_bt_ctlr.h>
#include <string.h>

#include "ipro7.h"
#include "ipro_ble_ctlr_hcitl.h"

/* arch_main.c */
int ipro_ble_ctlr_controller_init(unsigned int task_priority);

/* components/wireless/rf/ipro7_rf */
extern void rf_set_bz_mode(uint8_t mode);
#define RF_BZ_MODE_BLE_ONLY 0

/* hcitl gives a no-argument callback; keep the Zephyr-side context here. */
static void (*hci_rx_notify)(void *ctx);
static void *hci_rx_ctx;

static void hcitl_rx_notify(void)
{
	if (hci_rx_notify != NULL) {
		hci_rx_notify(hci_rx_ctx);
	}
}

/* ---------------------------------------------------------------------- */
/* Facade                                                                  */
/* ---------------------------------------------------------------------- */

int ipro_bt_ctlr_start(unsigned int task_prio, void (*rx_notify)(void *ctx), void *ctx)
{
	int ret;

	hci_rx_notify = rx_notify;
	hci_rx_ctx = ctx;

	rf_set_bz_mode(RF_BZ_MODE_BLE_ONLY);
	ret = ipro_ble_ctlr_controller_init(task_prio);
	if (ret != 0) {
		return -EIO;
	}
	ipro_ble_ctlr_hcitl_host_set_rx_notify(hcitl_rx_notify);
	return 0;
}

uint32_t ipro_bt_ctlr_hci_write(const uint8_t *data, uint32_t len)
{
	return ipro_ble_ctlr_hcitl_host_write(data, len);
}

uint32_t ipro_bt_ctlr_hci_read(uint8_t *out, uint32_t max)
{
	return ipro_ble_ctlr_hcitl_host_read(out, max);
}

/* ---------------------------------------------------------------------- */
/* FreeRTOS task-notification shim (ABI of the SDK's IPRO7 port)           */
/* ---------------------------------------------------------------------- */

/* Types and prototypes shared with the controller sources when they are
 * built here instead of linked as an archive. */
#include "FreeRTOS.h"
#include "task.h"

/* One controller task is all the archive creates; leave room for a second. */
#define SHIM_MAX_TASKS 2

struct freertos_shim_task {
	struct k_thread thread;
	struct k_sem notify;
	TaskFunction_t fn;
	void *arg;
	bool used;
};

static struct freertos_shim_task shim_tasks[SHIM_MAX_TASKS];

#ifdef CONFIG_BT_IPRO_CTLR_STALL_DIAG
/*
 * Liveness diagnostic for the controller thread: the gap between consecutive
 * BLE-ISR wake-ups and between consecutive controller-loop iterations. A
 * multi-second loop gap with a small ISR gap is a starved/lost wake-up in the
 * shim; a multi-second ISR gap is the controller's timer/RF stopping; both
 * small while the link dies is a protocol-level desync inside the archive.
 */
static struct {
	uint32_t last_isr, last_loop;
	uint32_t max_isr_gap, max_loop_gap;
	uint32_t isr_cnt, loop_cnt;
} diag;
static struct k_work_delayable diag_work;

static inline void diag_mark(uint32_t *last, uint32_t *max_gap, uint32_t *cnt)
{
	uint32_t now = k_cycle_get_32();

	if (*cnt != 0U) {
		uint32_t gap = now - *last;

		if (gap > *max_gap) {
			*max_gap = gap;
		}
	}
	*last = now;
	(*cnt)++;
}

static void diag_report(struct k_work *work)
{
	uint32_t now = k_cycle_get_32();
	unsigned int key = irq_lock();
	uint32_t isr_gap = diag.max_isr_gap, loop_gap = diag.max_loop_gap;
	uint32_t isr_cnt = diag.isr_cnt, loop_cnt = diag.loop_cnt;
	uint32_t isr_age = now - diag.last_isr, loop_age = now - diag.last_loop;

	diag.max_isr_gap = 0;
	diag.max_loop_gap = 0;
	diag.isr_cnt = 0;
	diag.loop_cnt = 0;
	irq_unlock(key);

	printk("[ctlr] isr %u max_gap %u us age %u us | loop %u max_gap %u us age %u us\n",
	       isr_cnt, k_cyc_to_us_floor32(isr_gap), k_cyc_to_us_floor32(isr_age),
	       loop_cnt, k_cyc_to_us_floor32(loop_gap), k_cyc_to_us_floor32(loop_age));
	k_work_schedule(&diag_work, K_MSEC(CONFIG_BT_IPRO_CTLR_STALL_DIAG_PERIOD_MS));
}

static void diag_start(void)
{
	k_work_init_delayable(&diag_work, diag_report);
	k_work_schedule(&diag_work, K_MSEC(CONFIG_BT_IPRO_CTLR_STALL_DIAG_PERIOD_MS));
}
#define DIAG_ISR()  diag_mark(&diag.last_isr, &diag.max_isr_gap, &diag.isr_cnt)
#define DIAG_LOOP() diag_mark(&diag.last_loop, &diag.max_loop_gap, &diag.loop_cnt)
#else
#define DIAG_ISR()  do {} while (0)
#define DIAG_LOOP() do {} while (0)
static inline void diag_start(void) {}
#endif

static struct freertos_shim_task *shim_current(void)
{
	k_tid_t tid = k_current_get();

	for (int i = 0; i < SHIM_MAX_TASKS; i++) {
		if (shim_tasks[i].used && &shim_tasks[i].thread == tid) {
			return &shim_tasks[i];
		}
	}
	return NULL;
}

static void shim_entry(void *p1, void *p2, void *p3)
{
	struct freertos_shim_task *t = p1;

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);
	t->fn(t->arg);
}

static k_timeout_t ticks_to_timeout(TickType_t ticks)
{
	if (ticks == portMAX_DELAY) {
		return K_FOREVER;
	}
	/* IPRO7 FreeRTOS port: configTICK_RATE_HZ = 1000 -> 1 tick = 1 ms. */
	return K_MSEC(ticks);
}

TaskHandle_t xTaskCreateStatic(TaskFunction_t code, const char *const name, StackType_t depth,
			       void *const params, UBaseType_t priority,
			       StackType_t *const stack_buffer, StaticTask_t *const task_buffer)
{
	ARG_UNUSED(task_buffer); /* FreeRTOS TCB storage; Zephyr keeps its own */

	for (int i = 0; i < SHIM_MAX_TASKS; i++) {
		struct freertos_shim_task *t = &shim_tasks[i];

		if (t->used) {
			continue;
		}
		t->used = true;
		t->fn = code;
		t->arg = params;
		k_sem_init(&t->notify, 0, K_SEM_MAX_LIMIT);
		/*
		 * FreeRTOS: larger number = higher priority. Zephyr: cooperative
		 * priorities are negative, smaller = higher. Fold the FreeRTOS
		 * priority into the cooperative range the HCI driver configures.
		 */
		k_thread_create(&t->thread, (k_thread_stack_t *)stack_buffer,
				depth * sizeof(StackType_t), shim_entry, t, NULL, NULL,
				K_PRIO_COOP((int)priority), 0, K_NO_WAIT);
		k_thread_name_set(&t->thread, name != NULL ? name : "ipro_ble_ctlr");
		diag_start();
		return t;
	}
	return NULL;
}

BaseType_t xTaskGenericNotify(TaskHandle_t task, UBaseType_t index, uint32_t value,
			      eNotifyAction action, uint32_t *prev)
{
	ARG_UNUSED(index);
	ARG_UNUSED(value);
	ARG_UNUSED(action);
	if (prev != NULL) {
		*prev = k_sem_count_get(&task->notify);
	}
	k_sem_give(&task->notify);
	return pdTRUE;
}

void vTaskGenericNotifyGiveFromISR(TaskHandle_t task, UBaseType_t index,
				   BaseType_t *higher_prio_woken)
{
	ARG_UNUSED(index);
	if (higher_prio_woken != NULL) {
		*higher_prio_woken = pdFALSE; /* Zephyr reschedules on ISR exit */
	}
	DIAG_ISR();
	k_sem_give(&task->notify);
}

uint32_t ulTaskGenericNotifyTake(UBaseType_t index, BaseType_t clear_on_exit, TickType_t ticks)
{
	struct freertos_shim_task *t = shim_current();
	uint32_t count = 0;

	ARG_UNUSED(index);
	__ASSERT(t != NULL, "ulTaskNotifyTake from a non-shim thread");
	if (t == NULL) {
		return 0;
	}
	if (k_sem_take(&t->notify, ticks_to_timeout(ticks)) != 0) {
		DIAG_LOOP();
		return 0;
	}
	DIAG_LOOP();
	count = 1;
	if (clear_on_exit) {
		/* pdTRUE: return the whole value and reset it. */
		while (k_sem_take(&t->notify, K_NO_WAIT) == 0) {
			count++;
		}
	}
	return count;
}

TickType_t xTaskGetTickCount(void)
{
	return (TickType_t)k_uptime_get_32();
}

BaseType_t xPortIsInsideInterrupt(void)
{
	return k_is_in_isr() ? pdTRUE : pdFALSE;
}

/* Tickless-idle budget query of the FreeRTOS port; Zephyr has no PDS
 * tickless integration for the controller yet, so there is no budget. */
TickType_t xTaskGetExpectedIdleTimeExcludingTask(TaskHandle_t task)
{
	ARG_UNUSED(task);
	return 0;
}

/*
 * vPortSetupTimerInterrupt(): the FreeRTOS port re-arms the system tick after
 * a PDS wake. Under Zephyr the systimer driver owns mtimecmp; re-arm one tick
 * ahead so the kernel keeps ticking after any controller-driven recovery.
 */
extern void ipro_systimer_rearm(void);
void vPortSetupTimerInterrupt(void)
{
	ipro_systimer_rearm();
}

/* ---------------------------------------------------------------------- */
/* SDK low-power framework / shell hooks referenced by the archive         */
/* ---------------------------------------------------------------------- */

/* interrupt.c / port.c nesting counter read by the LP code. */
BaseType_t TrapNetCounter;

/* hal_pm.c diagnostics storage. */
volatile uint32_t pm_pds_last_domain_words[8];

int hal_lp_init(void)
{
	return 0;
}

int hal_lp_enter(void *cfg, void *result)
{
	ARG_UNUSED(cfg);
	ARG_UNUSED(result);
	return -1; /* PDS sleep is not available in this port yet */
}

uint32_t pm_pds_ble_ctlr_domain_policy_get(void)
{
	return 0;
}

void pm_pds_rtc_target_override_set(uint64_t target_ticks)
{
	ARG_UNUSED(target_ticks);
}

void pm_pds_set_32k_src(uint8_t use_xtal, uint32_t settle_ms)
{
	ARG_UNUSED(use_xtal);
	ARG_UNUSED(settle_ms);
}

void pm_pds_set_misc_keep(bool keep)
{
	ARG_UNUSED(keep);
}

void lp_uart_init(int uart_id, uint32_t baudrate)
{
	ARG_UNUSED(uart_id);
	ARG_UNUSED(baudrate);
}

void shell_uart_reinit(void)
{
}
