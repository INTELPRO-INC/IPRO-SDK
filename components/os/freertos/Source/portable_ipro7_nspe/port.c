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
 * FreeRTOS port for the IPRO7 NSPE in U-mode. A task's saved context
 * is the struct psa_nspe_frame the SPE pushes on its stack at a vIRQ;
 * NSPE_ECALL_IRQ_RETURN resumes it.
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

/* struct psa_nspe_frame x[] index of register xN is N - 1 */
#define FRAME_RA 0u
#define FRAME_SP 1u
#define FRAME_GP 2u
#define FRAME_TP 3u
#define FRAME_A0 9u

_Static_assert(offsetof(struct psa_nspe_frame, x) == 4 * 1, "x1");
_Static_assert(offsetof(struct psa_nspe_frame, fstate) == 4 * 32, "fstate");
_Static_assert(offsetof(struct psa_nspe_frame, f) == 4 * 34, "f0");
_Static_assert(sizeof(struct psa_nspe_frame) <= PSA_NSPE_FRAME_SIZE, "size");
_Static_assert(sizeof(StackType_t) == sizeof(uint32_t), "StackType_t");
#ifndef SPM_HOST
_Static_assert(sizeof(struct nspe_trap_ret) == 8, "a0/a1 pair");
#endif

/* pxTopOfStack, the first member of the running task's TCB */
static volatile StackType_t **current_top(void)
{
    return (volatile StackType_t **)(void *)xTaskGetCurrentTaskHandle();
}

static struct psa_nspe_frame *current_frame(void)
{
    return (struct psa_nspe_frame *)(uintptr_t)*current_top();
}

static void prvTaskExitError(void)
{
    printf("[NSPE] task %s returned\r\n", pcTaskGetName(NULL));
    vTaskDelete(NULL);
    for (;;) {
    }
}

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack,
                                   TaskFunction_t pxCode,
                                   void *pvParameters)
{
    uintptr_t end = (uintptr_t)(pxTopOfStack + 1);
    struct psa_nspe_frame *f = (struct psa_nspe_frame *)(
        (end - PSA_NSPE_FRAME_SIZE) & ~(uintptr_t)15);

    memset(f, 0, sizeof(*f));
    f->epc = (uint32_t)(uintptr_t)pxCode;
    f->x[FRAME_RA] = (uint32_t)(uintptr_t)prvTaskExitError;
    f->x[FRAME_SP] = (uint32_t)(uintptr_t)f;
    f->x[FRAME_GP] = nspe_gp();
    f->x[FRAME_TP] = nspe_tp();
    f->x[FRAME_A0] = (uint32_t)(uintptr_t)pvParameters;
    f->fstate = 0;
    return (StackType_t *)f;
}

BaseType_t xPortStartScheduler(void)
{
    if (nspe_runtime_init() != 0) {
        printf("[NSPE] NSPE_ECALL_INIT refused\r\n");
        return pdFALSE;
    }
    if (nspe_ecall(NSPE_ECALL_TIMER_SET,
                   configCPU_CLOCK_HZ / configTICK_RATE_HZ,
                   0, 0, 0, 0, 0) != 0) {
        printf("[NSPE] NSPE_ECALL_TIMER_SET refused\r\n");
        return pdFALSE;
    }
    nspe_crit_nesting = 0;
    nspe_start_first_task(current_frame());
}

void vPortEndScheduler(void)
{
    printf("[NSPE] vTaskEndScheduler is not supported\r\n");
}

/*
 * Report the fault, then delete the faulting task. Faults of the idle
 * task or outside a running scheduler reset the NSPE.
 */
void vPortFaultDefault(const struct psa_nspe_ctrl *ctrl)
{
    TaskHandle_t task = xTaskGetCurrentTaskHandle();

    printf("[NSPE] fault mcause=%lu mtval=%08lx mepc=%08lx task %s\r\n",
           (unsigned long)ctrl->fault_mcause,
           (unsigned long)ctrl->fault_mtval,
           (unsigned long)ctrl->fault_mepc,
           task ? pcTaskGetName(task) : "-");
    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING ||
        task == xTaskGetIdleTaskHandle()) {
        (void)nspe_ecall(NSPE_ECALL_RESET, 0, 0, 0, 0, 0, 0);
    }
    vTaskDelete(NULL);
}

/* Weak default; a replacement must not resume the faulting frame */
__attribute__((weak)) void vPortFaultHook(const struct psa_nspe_ctrl *ctrl)
{
    vPortFaultDefault(ctrl);
}

/*
 * irq is the peripheral id when its handler ran, else NSPE_IRQ_NONE.
 * A peripheral id with no handler is detached; VIRQ_NOTIFY has no
 * handler in this port.
 */
struct nspe_trap_ret nspe_trap_c(uint32_t id, struct psa_nspe_frame *f)
{
    struct nspe_trap_ret ret = { f, NSPE_IRQ_NONE };
    uint32_t saved = nspe_trap_enter();

    switch (id) {
    case VIRQ_TICK:
        if (xTaskIncrementTick() != pdFALSE) {
            nspe_yield();
        }
        break;
    case VIRQ_YIELD:
        nspe_yield();
        break;
    case VIRQ_FAULT:
        vPortFaultHook(&nspe_ctrl);
        break;
    case VIRQ_NOTIFY:
        break;
    default:
        if (nspe_irq_dispatch(id) == 0) {
            ret.irq = id;
        } else {
            (void)nspe_irq_detach(id);
        }
        break;
    }
    if (nspe_switch_take()) {
        *current_top() = (volatile StackType_t *)f;
        vTaskSwitchContext();
        ret.frame = current_frame();
    }
    nspe_trap_leave(saved);
    return ret;
}
