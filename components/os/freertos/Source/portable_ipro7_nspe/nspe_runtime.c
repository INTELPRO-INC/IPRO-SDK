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
 * NSPE runtime of the U-mode FreeRTOS port. The first half is pure C
 * (host-tested with -DSPM_HOST); the second half holds the FreeRTOS
 * application hooks and the heap setup of the NSPE image.
 */
#include <stddef.h>

#include "nspe_runtime.h"

#ifdef SPM_HOST
#define NSPE_OCRAM_BSS
#else
#define NSPE_OCRAM_BSS __attribute__((section(".ocram_bss")))
#endif

struct psa_nspe_ctrl nspe_ctrl NSPE_OCRAM_BSS __attribute__((aligned(64)));

volatile uint32_t nspe_crit_nesting = NSPE_CRIT_BOOT;

static volatile uint32_t nspe_trap_active;
static volatile uint32_t nspe_switch_req;
static uint32_t nspe_bound;
static nspe_irq_handler_t nspe_irq_table[NSPE_IRQ_IDS];

int nspe_runtime_init(void)
{
    int ret;

    if (nspe_bound) {
        return 0;
    }
    nspe_virq_disable();
    ret = nspe_ecall(NSPE_ECALL_INIT, (uint32_t)(uintptr_t)nspe_trap_entry,
                     (uint32_t)(uintptr_t)&nspe_ctrl, 0, 0, 0, 0);
    nspe_bound = ret == 0;
    return ret;
}

void nspe_critical_enter(void)
{
    nspe_virq_disable();
    nspe_crit_nesting++;
}

void nspe_critical_exit(void)
{
    nspe_crit_nesting--;
    if (nspe_crit_nesting == 0) {
        nspe_virq_enable();
    }
}

uint32_t nspe_mask_save(void)
{
    uint32_t prev = nspe_ctrl.virq_mask;

    nspe_virq_disable();
    return prev;
}

void nspe_mask_restore(uint32_t prev)
{
    if (prev == 0) {
        nspe_virq_enable();
    }
}

/* Returns the task nesting (0: delivery needs the mask clear). */
uint32_t nspe_trap_enter(void)
{
    uint32_t saved = nspe_crit_nesting;

    nspe_crit_nesting = 1;
    nspe_trap_active = 1;
    return saved;
}

/* virq_mask stays 1: NSPE_ECALL_IRQ_RETURN clears it in the SPE. */
void nspe_trap_leave(uint32_t saved)
{
    nspe_trap_active = 0;
    nspe_crit_nesting = saved;
}

int nspe_in_trap(void)
{
    return nspe_trap_active != 0;
}

void nspe_yield(void)
{
    if (nspe_trap_active) {
        nspe_switch_req = 1;
    } else {
        (void)nspe_ecall(NSPE_ECALL_YIELD, 0, 0, 0, 0, 0, 0);
    }
}

int nspe_switch_take(void)
{
    int req = nspe_switch_req != 0;

    nspe_switch_req = 0;
    return req;
}

/* The handler is in the table before the SPE can deliver the id. */
int nspe_irq_attach(uint32_t id, nspe_irq_handler_t h)
{
    if (id >= NSPE_IRQ_IDS || h == NULL) {
        return -1;
    }
    nspe_irq_table[id] = h;
    if (nspe_ecall(NSPE_ECALL_IRQ_ATTACH, id, 0, 0, 0, 0, 0) != 0) {
        nspe_irq_table[id] = NULL;
        return -1;
    }
    return 0;
}

int nspe_irq_detach(uint32_t id)
{
    int ret;

    if (id >= NSPE_IRQ_IDS) {
        return -1;
    }
    ret = nspe_ecall(NSPE_ECALL_IRQ_DETACH, id, 0, 0, 0, 0, 0);
    nspe_irq_table[id] = NULL;
    return ret == 0 ? 0 : -1;
}

int nspe_irq_dispatch(uint32_t id)
{
    nspe_irq_handler_t h;

    if (id >= NSPE_IRQ_IDS) {
        return -1;
    }
    h = nspe_irq_table[id];
    if (h == NULL) {
        return -1;
    }
    h();
    return 0;
}

#ifndef SPM_HOST
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

/* heap_5 records the last failed request here */
volatile size_t g_last_malloc_failed_size;
void *volatile g_last_malloc_failed_caller;

extern char __heap_psram_start[];
extern char __heap_psram_end[];

static HeapRegion_t nspe_heap_regions[2];

void nspe_heap_init(void)
{
    nspe_heap_regions[0].pucStartAddress = (uint8_t *)__heap_psram_start;
    nspe_heap_regions[0].xSizeInBytes =
        (size_t)(__heap_psram_end - __heap_psram_start);
    vPortDefineHeapRegions(nspe_heap_regions);
}

uint32_t nspe_run_time_counter(void)
{
    return (uint32_t)(nspe_cycles() >> 8);
}

static StaticTask_t nspe_idle_tcb NSPE_OCRAM_BSS;
static StackType_t nspe_idle_stack[configMINIMAL_STACK_SIZE] NSPE_OCRAM_BSS;
static StaticTask_t nspe_timer_tcb NSPE_OCRAM_BSS;
static StackType_t nspe_timer_stack[configTIMER_TASK_STACK_DEPTH]
    NSPE_OCRAM_BSS;

void vApplicationGetIdleTaskMemory(StaticTask_t **tcb, StackType_t **stack,
                                   configSTACK_DEPTH_TYPE *depth)
{
    *tcb = &nspe_idle_tcb;
    *stack = nspe_idle_stack;
    *depth = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **tcb, StackType_t **stack,
                                    configSTACK_DEPTH_TYPE *depth)
{
    *tcb = &nspe_timer_tcb;
    *stack = nspe_timer_stack;
    *depth = configTIMER_TASK_STACK_DEPTH;
}

/* The SPE sleeps until a vIRQ is pending, then returns. */
__attribute__((weak)) void vApplicationIdleHook(void)
{
    (void)nspe_ecall(NSPE_ECALL_WFI, 0, 0, 0, 0, 0, 0);
}

/* malloc(0) only warns; a real out-of-memory resets the NSPE. */
__attribute__((weak)) void vApplicationMallocFailedHook(void)
{
    printf("[NSPE] malloc %u failed, free %u\r\n",
           (unsigned)g_last_malloc_failed_size,
           (unsigned)xPortGetFreeHeapSize());
    if (g_last_malloc_failed_size == 0) {
        return;
    }
    (void)nspe_ecall(NSPE_ECALL_RESET, 0, 0, 0, 0, 0, 0);
    for (;;) {
    }
}

__attribute__((weak)) void vApplicationStackOverflowHook(TaskHandle_t task,
                                                         char *name)
{
    (void)task;
    printf("[NSPE] stack overflow in %s, reset\r\n", name ? name : "?");
    (void)nspe_ecall(NSPE_ECALL_RESET, 0, 0, 0, 0, 0, 0);
    for (;;) {
    }
}

__attribute__((weak)) void vAssertCalled(const char *const file,
                                         unsigned long line)
{
    nspe_virq_disable();
    printf("[NSPE] ASSERT %s:%lu\r\n", file ? file : "?", line);
    for (;;) {
    }
}
#endif
