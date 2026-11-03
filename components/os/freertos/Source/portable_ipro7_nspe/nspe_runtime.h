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
 * NSPE runtime of the U-mode FreeRTOS port: vIRQ control block,
 * critical sections, trap bracket, IRQ table, trap entry points.
 * The inline asm becomes host stubs under -DSPM_HOST.
 */
#pragma once

#include <stdint.h>

#include "psa_ipro/abi.h"
#include "psa_ipro/ecall.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ECLIC ids covered by virq_pending[] and the IRQ table */
#define NSPE_IRQ_IDS   (32u * VIRQ_WORDS)
/* NSPE_ECALL_IRQ_RETURN irq argument: no source to re-enable */
#define NSPE_IRQ_NONE  0xFFFFFFFFu
/* Nesting until the scheduler starts: an exit never unmasks */
#define NSPE_CRIT_BOOT 0xAAAAAAAAu

typedef void (*nspe_irq_handler_t)(void);

/* NSPE_ECALL_IRQ_RETURN arguments, returned in a0/a1 on RV32 */
struct nspe_trap_ret {
    struct psa_nspe_frame *frame;
    uint32_t irq;
};

/* vIRQ control block bound with NSPE_ECALL_INIT (rules in abi.h) */
extern struct psa_nspe_ctrl nspe_ctrl;

/* Critical section depth; 1 while the trap handler runs */
extern volatile uint32_t nspe_crit_nesting;

/* GCC/portasm.S vIRQ entry: a0 = id, a1 = frame, on the boot stack */
void nspe_trap_entry(void);

/* GCC/portasm.S: NSPE_ECALL_IRQ_RETURN(f, NSPE_IRQ_NONE) */
void nspe_start_first_task(struct psa_nspe_frame *f)
    __attribute__((noreturn));

/* port.c: handles vIRQ id taken on frame f, returns what to resume */
struct nspe_trap_ret nspe_trap_c(uint32_t id, struct psa_nspe_frame *f);

#ifdef SPM_HOST
uint32_t nspe_gp(void);
uint32_t nspe_tp(void);
#else
static inline uint32_t nspe_gp(void)
{
    uint32_t v;

    __asm__ volatile("mv %0, gp" : "=r"(v));
    return v;
}

static inline uint32_t nspe_tp(void)
{
    uint32_t v;

    __asm__ volatile("mv %0, tp" : "=r"(v));
    return v;
}

/* CPU cycles (rdcycle; the SPE emulates it when it traps) */
static inline uint64_t nspe_cycles(void)
{
    uint32_t hi;
    uint32_t lo;
    uint32_t hi2;

    do {
        __asm__ volatile("rdcycleh %0" : "=r"(hi));
        __asm__ volatile("rdcycle %0" : "=r"(lo));
        __asm__ volatile("rdcycleh %0" : "=r"(hi2));
    } while (hi != hi2);
    return ((uint64_t)hi << 32) | lo;
}

/* SysTimer mtime at 4 MHz (rdtime; the SPE emulates it on a trap) */
static inline uint64_t nspe_mtime(void)
{
    uint32_t hi;
    uint32_t lo;
    uint32_t hi2;

    do {
        __asm__ volatile("rdtimeh %0" : "=r"(hi));
        __asm__ volatile("rdtime %0" : "=r"(lo));
        __asm__ volatile("rdtimeh %0" : "=r"(hi2));
    } while (hi != hi2);
    return ((uint64_t)hi << 32) | lo;
}
#endif

/* 1 when the SPE has any vIRQ pending, masked or not */
static inline int nspe_virq_pending_any(void)
{
    uint32_t any = 0;

    for (uint32_t w = 0; w < VIRQ_WORDS; w++) {
        any |= nspe_ctrl.virq_pending[w];
    }
    return any != 0;
}

static inline void nspe_virq_disable(void)
{
    nspe_ctrl.virq_mask = 1;
    __asm__ volatile("" ::: "memory");
}

/* Unmask; a vIRQ already pending is delivered by IRQ_DELIVER. */
static inline void nspe_virq_enable(void)
{
    nspe_ctrl.virq_mask = 0;
    __asm__ volatile("" ::: "memory");
    if (nspe_virq_pending_any()) {
        (void)nspe_ecall(NSPE_ECALL_IRQ_DELIVER, 0, 0, 0, 0, 0, 0);
    }
}

/* Mask, then NSPE_ECALL_INIT once; 0 when the SPE accepted it */
int nspe_runtime_init(void);

void nspe_critical_enter(void);
void nspe_critical_exit(void);

/* portSET_INTERRUPT_MASK_FROM_ISR: masks, returns the old mask */
uint32_t nspe_mask_save(void);
/* portCLEAR_INTERRUPT_MASK_FROM_ISR: unmasks when prev is 0 */
void nspe_mask_restore(uint32_t prev);

/* Trap handler bracket: in_trap 1, nesting 1; the mask is the SPE's */
uint32_t nspe_trap_enter(void);
void nspe_trap_leave(uint32_t saved);
int nspe_in_trap(void);

/* Context switch: NSPE_ECALL_YIELD, or a flag inside the trap */
void nspe_yield(void);
/* 1 once after nspe_yield() ran inside the trap, else 0 */
int nspe_switch_take(void);

/* Handler table + NSPE_ECALL_IRQ_ATTACH/DETACH; 0 or -1 */
int nspe_irq_attach(uint32_t id, nspe_irq_handler_t h);
int nspe_irq_detach(uint32_t id);
/* Run the handler attached to id; -1 when there is none */
int nspe_irq_dispatch(uint32_t id);

/* heap_5 over the PSRAM heap of the NSPE linker script */
void nspe_heap_init(void);

#ifdef __cplusplus
}
#endif
