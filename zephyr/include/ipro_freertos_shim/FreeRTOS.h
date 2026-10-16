/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * FreeRTOS API surface used by the IPRO BLE controller platform layer
 * (plf/ipro7, hcitl), mapped onto Zephyr by hal_glue/ipro_bt_ctlr_glue.c.
 * Only what the controller sources reference is declared; this is not a
 * FreeRTOS compatibility layer for applications.
 */
#ifndef IPRO_FREERTOS_SHIM_FREERTOS_H
#define IPRO_FREERTOS_SHIM_FREERTOS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef long BaseType_t;
typedef unsigned long UBaseType_t;
typedef uint32_t TickType_t;
typedef unsigned long StackType_t;

#define pdTRUE  1
#define pdFALSE 0
#define pdPASS  1
#define pdFAIL  0

/* IPRO7 FreeRTOS port: configTICK_RATE_HZ = 1000 -> 1 tick = 1 ms. */
#define configTICK_RATE_HZ   1000U
#define portTICK_PERIOD_MS   1U
#define pdMS_TO_TICKS(ms)    ((TickType_t)(ms))
#define portMAX_DELAY        0xFFFFFFFFUL

#ifdef __cplusplus
}
#endif

#endif /* IPRO_FREERTOS_SHIM_FREERTOS_H */
