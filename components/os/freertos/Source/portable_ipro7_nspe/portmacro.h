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
 * FreeRTOS port macros of the IPRO7 NSPE (U-mode under the PSA SPE).
 * Interrupt masking is the vIRQ mask in nspe_ctrl; a context switch
 * is NSPE_ECALL_YIELD, or a flag the trap handler honours.
 */
#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "nspe_runtime.h"

#define portCHAR                    char
#define portFLOAT                   float
#define portDOUBLE                  double
#define portLONG                    long
#define portSHORT                   short
#define portSTACK_TYPE              unsigned long
#define portBASE_TYPE               long
#define portPOINTER_SIZE_TYPE       unsigned long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

/* 64-bit like the M-mode port: the SysTimer is 64-bit */
typedef uint64_t TickType_t;
#define portMAX_DELAY               ((TickType_t)0xFFFFFFFFFFFFFFFFULL)

/*
 * Task stacks hold the 272-byte frame the SPE pushes at a vIRQ; the
 * handler runs on the boot stack (_nspe_stack_top), free once the
 * scheduler runs. vIRQs stay masked until then.
 */
#define portSTACK_GROWTH            (-1)
#define portTICK_PERIOD_MS          ((TickType_t)1000 / configTICK_RATE_HZ)
#define portBYTE_ALIGNMENT          8

#define portYIELD()                 nspe_yield()
#define portEND_SWITCHING_ISR(x)    \
    do {                            \
        if ((x) != pdFALSE) {       \
            nspe_yield();           \
        }                           \
    } while (0)
#define portYIELD_FROM_ISR(x)       portEND_SWITCHING_ISR(x)

#define portSET_INTERRUPT_MASK_FROM_ISR()       nspe_mask_save()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)    nspe_mask_restore(x)
#define portDISABLE_INTERRUPTS()                nspe_virq_disable()
#define portENABLE_INTERRUPTS()                 nspe_virq_enable()
#define portENTER_CRITICAL()                    nspe_critical_enter()
#define portEXIT_CRITICAL()                     nspe_critical_exit()

#define portTASK_FUNCTION_PROTO(vFunction, pvParameters) \
    void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters) \
    void vFunction(void *pvParameters)

#define portNOP()                   __asm__ volatile("nop")
#define portINLINE                  __inline
#ifndef portFORCE_INLINE
#define portFORCE_INLINE            inline __attribute__((always_inline))
#endif
#define portMEMORY_BARRIER()        __asm__ volatile("" ::: "memory")

/* 1 while nspe_trap_c runs a vIRQ */
portFORCE_INLINE static BaseType_t xPortIsInsideInterrupt(void)
{
    return nspe_in_trap();
}

/* VIRQ_FAULT, in the trap: weak default = vPortFaultDefault */
void vPortFaultHook(const struct psa_nspe_ctrl *ctrl);
/* Prints the fault and deletes the faulting task (or resets) */
void vPortFaultDefault(const struct psa_nspe_ctrl *ctrl);

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
