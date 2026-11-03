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
 * Host stand-in for the FreeRTOS.h that portable_ipro7_nspe/port.c
 * uses under -DSPM_HOST: port types, constants, nspe_runtime.h.
 */
#pragma once

#include <stdint.h>

#include "nspe_runtime.h"

typedef uint32_t StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
typedef void (*TaskFunction_t)(void *);

#define pdFALSE ((BaseType_t)0)
#define pdTRUE  ((BaseType_t)1)

#define configCPU_CLOCK_HZ (4000000UL)
#define configTICK_RATE_HZ (1000UL)

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack,
                                   TaskFunction_t pxCode,
                                   void *pvParameters);
BaseType_t xPortStartScheduler(void);
void vPortEndScheduler(void);
void vPortFaultHook(const struct psa_nspe_ctrl *ctrl);
