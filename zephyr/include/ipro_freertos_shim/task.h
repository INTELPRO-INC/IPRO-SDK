/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * FreeRTOS task API used by the IPRO BLE controller platform layer; the
 * implementations live in hal_glue/ipro_bt_ctlr_glue.c on Zephyr threads and
 * a counting semaphore per task (task notifications are latched, so a
 * semaphore count reproduces them).
 */
#ifndef IPRO_FREERTOS_SHIM_TASK_H
#define IPRO_FREERTOS_SHIM_TASK_H

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TaskFunction_t)(void *);
typedef struct freertos_shim_task *TaskHandle_t;

/* Storage the caller hands to xTaskCreateStatic(); Zephyr keeps its own
 * thread object, so only the size has to exist. */
typedef struct {
	uint32_t opaque[4];
} StaticTask_t;

typedef enum {
	eNoAction = 0,
	eSetBits,
	eIncrement,
	eSetValueWithOverwrite,
	eSetValueWithoutOverwrite
} eNotifyAction;

TaskHandle_t xTaskCreateStatic(TaskFunction_t code, const char *const name, StackType_t depth,
			       void *const params, UBaseType_t priority,
			       StackType_t *const stack_buffer, StaticTask_t *const task_buffer);
BaseType_t xTaskGenericNotify(TaskHandle_t task, UBaseType_t index, uint32_t value,
			      eNotifyAction action, uint32_t *prev);
void vTaskGenericNotifyGiveFromISR(TaskHandle_t task, UBaseType_t index,
				   BaseType_t *higher_prio_woken);
uint32_t ulTaskGenericNotifyTake(UBaseType_t index, BaseType_t clear_on_exit, TickType_t ticks);
TickType_t xTaskGetTickCount(void);
TickType_t xTaskGetExpectedIdleTimeExcludingTask(TaskHandle_t task);

#define xTaskNotifyGive(task) xTaskGenericNotify((task), 0, 0, eIncrement, NULL)
#define vTaskNotifyGiveFromISR(task, woken) vTaskGenericNotifyGiveFromISR((task), 0, (woken))
#define ulTaskNotifyTake(clear, ticks) ulTaskGenericNotifyTake(0, (clear), (ticks))
#define portYIELD_FROM_ISR(x) ((void)(x)) /* Zephyr reschedules on ISR exit */
BaseType_t xPortIsInsideInterrupt(void);
const char *pcTaskGetName(TaskHandle_t task);

/* taskENTER/EXIT_CRITICAL() as the Wi-Fi manager uses them: a nested
 * interrupt lock (hal_glue/ipro_wifi_rtos_al.c). */
void ipro_freertos_shim_enter_critical(void);
void ipro_freertos_shim_exit_critical(void);
#define taskENTER_CRITICAL() ipro_freertos_shim_enter_critical()
#define taskEXIT_CRITICAL()  ipro_freertos_shim_exit_critical()
void vPortSetupTimerInterrupt(void);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_FREERTOS_SHIM_TASK_H */
