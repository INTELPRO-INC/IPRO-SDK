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

/* minimal FreeRTOS surface for host tests of ipro_pm */
#ifndef HOST_FREERTOS_H
#define HOST_FREERTOS_H
#include <stddef.h>
#include <stdint.h>
typedef uint32_t TickType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
typedef void *TaskHandle_t;
typedef void *TimerHandle_t;
#define pdFALSE 0
#define pdTRUE 1
#define pdPASS 1
#define portMAX_DELAY 0xffffffffUL
#define portTICK_PERIOD_MS 1
#define configUSE_TICKLESS_IDLE 1
#define configUSE_IDLE_HOOK 1
#define configSYSTICK_CLOCK_HZ 4000000UL
#define pdMS_TO_TICKS(ms) ((TickType_t)(ms))
#define taskENTER_CRITICAL() ((void)0)
#define taskEXIT_CRITICAL() ((void)0)
#define taskENTER_CRITICAL_FROM_ISR() (0UL)
#define taskEXIT_CRITICAL_FROM_ISR(x) ((void)(x))
extern TickType_t host_tick;
#endif
