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
 * Host test of port.c + nspe_runtime.c (-DSPM_HOST, stub_rtos/): the
 * IRQ_RETURN arguments nspe_trap_c returns per vIRQ id, the context
 * switch through pxTopOfStack, and the frame of a new task.
 */
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "task.h"

#define PERIPH_ID 48u
#define PERIPH_NO_HANDLER 50u
#define TEST_GP 0x11020800u
#define TEST_TP 0x1e000ba0u
#define STACK_WORDS 256u

/* pxTopOfStack is the first member, as in the kernel TCB */
struct tskTaskControlBlock {
    volatile StackType_t *top;
};

static struct tskTaskControlBlock g_tcb_a;
static struct tskTaskControlBlock g_tcb_b;
static struct psa_nspe_frame g_frame_a __attribute__((aligned(16)));
static struct psa_nspe_frame g_frame_b __attribute__((aligned(16)));
static StackType_t g_stack[STACK_WORDS] __attribute__((aligned(16)));
static char g_name[] = "t";
static TaskHandle_t g_current;
static BaseType_t g_tick_ret;
static int g_ticks;
static int g_switches;
static int g_faults;
static int g_handled;
static int g_handler_in_trap;
static int g_ncalls;
static uint32_t g_last_fn;
static uint32_t g_last_a0;
static const struct psa_nspe_ctrl *g_fault_ctrl;

int nspe_ecall(uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2,
               uint32_t a3, uint32_t a4, uint32_t a5)
{
    (void)a1;
    (void)a2;
    (void)a3;
    (void)a4;
    (void)a5;
    g_ncalls++;
    g_last_fn = fn;
    g_last_a0 = a0;
    return 0;
}

void nspe_trap_entry(void)
{
}

void nspe_start_first_task(struct psa_nspe_frame *f)
{
    (void)f;
    abort();
}

uint32_t nspe_gp(void)
{
    return TEST_GP;
}

uint32_t nspe_tp(void)
{
    return TEST_TP;
}

BaseType_t xTaskIncrementTick(void)
{
    g_ticks++;
    return g_tick_ret;
}

void vTaskSwitchContext(void)
{
    g_switches++;
    g_current = g_current == &g_tcb_a ? &g_tcb_b : &g_tcb_a;
}

TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
    return g_current;
}

TaskHandle_t xTaskGetIdleTaskHandle(void)
{
    return &g_tcb_b;
}

BaseType_t xTaskGetSchedulerState(void)
{
    return taskSCHEDULER_RUNNING;
}

char *pcTaskGetName(TaskHandle_t task)
{
    (void)task;
    return g_name;
}

/* As the kernel: deleting the running task yields */
void vTaskDelete(TaskHandle_t task)
{
    (void)task;
    nspe_yield();
}

void vPortFaultHook(const struct psa_nspe_ctrl *ctrl)
{
    g_faults++;
    g_fault_ctrl = ctrl;
    vTaskDelete(NULL);
}

static void handler(void)
{
    g_handled++;
    g_handler_in_trap = nspe_in_trap();
}

static void task_fn(void *arg)
{
    (void)arg;
}

/* Task A runs; task B is ready with g_frame_b; the SPE masked */
static void reset(void)
{
    g_current = &g_tcb_a;
    g_tcb_a.top = NULL;
    g_tcb_b.top = (volatile StackType_t *)&g_frame_b;
    g_tick_ret = pdFALSE;
    g_ticks = 0;
    g_switches = 0;
    g_faults = 0;
    g_handled = 0;
    g_handler_in_trap = 0;
    g_ncalls = 0;
    g_last_fn = 0;
    g_last_a0 = 0;
    g_fault_ctrl = NULL;
    nspe_crit_nesting = 0;
    nspe_ctrl.virq_mask = 1;
}

/* No switch: frame a back, bracket closed, mask left to the SPE */
static int resumes_a(struct nspe_trap_ret r)
{
    return r.frame == &g_frame_a && g_switches == 0 &&
           g_tcb_a.top == NULL && nspe_in_trap() == 0 &&
           nspe_crit_nesting == 0 && nspe_ctrl.virq_mask == 1;
}

/* Switch: frame a saved in task A, frame b resumed */
static int switched_to_b(struct nspe_trap_ret r)
{
    return r.frame == &g_frame_b && g_switches == 1 &&
           g_tcb_a.top == (volatile StackType_t *)&g_frame_a &&
           g_current == &g_tcb_b && nspe_in_trap() == 0 &&
           nspe_crit_nesting == 0 && nspe_ctrl.virq_mask == 1;
}

static void test_tick(void)
{
    struct nspe_trap_ret r;

    reset();
    r = nspe_trap_c(VIRQ_TICK, &g_frame_a);
    check(g_ticks == 1, "TICK: xTaskIncrementTick called");
    check(r.irq == NSPE_IRQ_NONE, "TICK: irq NONE");
    check(resumes_a(r) && g_ncalls == 0, "TICK, no switch: frame a, no ecall");

    reset();
    g_tick_ret = pdTRUE;
    r = nspe_trap_c(VIRQ_TICK, &g_frame_a);
    check(r.irq == NSPE_IRQ_NONE && switched_to_b(r),
          "TICK, switch due: frame a saved, frame b returned");
}

static void test_yield(void)
{
    struct nspe_trap_ret r;

    reset();
    r = nspe_trap_c(VIRQ_YIELD, &g_frame_a);
    check(r.irq == NSPE_IRQ_NONE, "YIELD: irq NONE");
    check(switched_to_b(r) && g_ticks == 0,
          "YIELD: old frame stored, new frame returned");
    check(g_ncalls == 0, "YIELD: no ecall inside the trap");
}

static void test_peripheral(void)
{
    struct nspe_trap_ret r;

    reset();
    check(nspe_irq_attach(PERIPH_ID, handler) == 0,
          "attach peripheral handler");
    g_ncalls = 0;
    r = nspe_trap_c(PERIPH_ID, &g_frame_a);
    check(g_handled == 1 && g_handler_in_trap == 1,
          "peripheral: handler called inside the trap");
    check(r.irq == PERIPH_ID, "peripheral: irq = id");
    check(resumes_a(r) && g_ncalls == 0,
          "peripheral: frame a, no ecall");

    reset();
    r = nspe_trap_c(PERIPH_NO_HANDLER, &g_frame_a);
    check(g_ncalls == 1 && g_last_fn == NSPE_ECALL_IRQ_DETACH &&
              g_last_a0 == PERIPH_NO_HANDLER,
          "peripheral without handler: IRQ_DETACH(id)");
    check(r.irq == NSPE_IRQ_NONE && resumes_a(r),
          "peripheral without handler: irq NONE, frame a");
}

static void test_notify(void)
{
    struct nspe_trap_ret r;

    reset();
    r = nspe_trap_c(VIRQ_NOTIFY, &g_frame_a);
    check(r.irq == NSPE_IRQ_NONE, "NOTIFY: irq NONE");
    check(g_handled == 0 && g_ncalls == 0 && resumes_a(r),
          "NOTIFY: no handler, no ecall, frame a");
}

static void test_fault(void)
{
    struct nspe_trap_ret r;

    reset();
    r = nspe_trap_c(VIRQ_FAULT, &g_frame_a);
    check(g_faults == 1 && g_fault_ctrl == &nspe_ctrl,
          "FAULT: vPortFaultHook(&nspe_ctrl)");
    check(r.irq == NSPE_IRQ_NONE && switched_to_b(r),
          "FAULT: hook deletes the task, switch to frame b");
}

static void test_stack_init(void)
{
    StackType_t *top = &g_stack[STACK_WORDS - 1u];
    void *param = &g_stack[1];
    struct psa_nspe_frame *f = (struct psa_nspe_frame *)(void *)
        pxPortInitialiseStack(top, task_fn, param);
    int zero = 1;

    check(((uintptr_t)f & 15u) == 0, "new frame 16-byte aligned");
    check((uintptr_t)f >= (uintptr_t)g_stack &&
              (uintptr_t)(f + 1) <= (uintptr_t)(top + 1),
          "new frame inside the stack");
    check(f->epc == (uint32_t)(uintptr_t)task_fn, "epc = task function");
    check(f->x[0] != 0, "ra = task exit trap");
    check(f->x[1] == (uint32_t)(uintptr_t)f, "sp = frame address");
    check(f->x[2] == TEST_GP && f->x[3] == TEST_TP, "gp, tp = current");
    check(f->x[9] == (uint32_t)(uintptr_t)param, "a0 = parameter");
    check(f->fstate == 0, "fstate 0");
    for (uint32_t i = 4; i < 31; i++) {
        if (i != 9 && f->x[i] != 0) {
            zero = 0;
        }
    }
    check(zero, "other registers 0");
}

int main(void)
{
    test_tick();
    test_yield();
    test_peripheral();
    test_notify();
    test_fault();
    test_stack_init();
    return check_summary();
}
