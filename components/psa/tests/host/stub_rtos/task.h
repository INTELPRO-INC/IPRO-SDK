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
 * Host stand-in for the task.h calls of portable_ipro7_nspe/port.c;
 * the host test defines them and struct tskTaskControlBlock.
 */
#pragma once

#include "FreeRTOS.h"

struct tskTaskControlBlock;
typedef struct tskTaskControlBlock *TaskHandle_t;

#define taskSCHEDULER_SUSPENDED   ((BaseType_t)0)
#define taskSCHEDULER_NOT_STARTED ((BaseType_t)1)
#define taskSCHEDULER_RUNNING     ((BaseType_t)2)

BaseType_t xTaskIncrementTick(void);
void vTaskSwitchContext(void);
TaskHandle_t xTaskGetCurrentTaskHandle(void);
TaskHandle_t xTaskGetIdleTaskHandle(void);
BaseType_t xTaskGetSchedulerState(void);
char *pcTaskGetName(TaskHandle_t task);
void vTaskDelete(TaskHandle_t task);
