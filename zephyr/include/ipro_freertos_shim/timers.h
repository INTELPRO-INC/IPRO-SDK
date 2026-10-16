/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * FreeRTOS timers.h surface referenced (by type only) by the SDK wireless
 * components built under Zephyr.
 */
#ifndef IPRO_FREERTOS_SHIM_TIMERS_H
#define IPRO_FREERTOS_SHIM_TIMERS_H

#include "FreeRTOS.h"

typedef struct freertos_shim_timer *TimerHandle_t;
typedef void (*TimerCallbackFunction_t)(TimerHandle_t);

#endif
