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
 * Host test of nspe_runtime.c (-DSPM_HOST): vIRQ mask and pending,
 * critical nesting, trap bracket, yield, IRQ table. nspe_ecall is
 * the recording stub below.
 */
#include <string.h>

#include "check.h"
#include "nspe_runtime.h"

#define MAX_CALLS 16

struct ecall_rec {
    uint32_t fn;
    uint32_t a0;
    uint32_t a1;
    uint32_t mask;
};

static struct ecall_rec g_calls[MAX_CALLS];
static int g_ncalls;
static int g_ret;
static int g_handled;

int nspe_ecall(uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2,
               uint32_t a3, uint32_t a4, uint32_t a5)
{
    (void)a2;
    (void)a3;
    (void)a4;
    (void)a5;
    if (g_ncalls < MAX_CALLS) {
        g_calls[g_ncalls].fn = fn;
        g_calls[g_ncalls].a0 = a0;
        g_calls[g_ncalls].a1 = a1;
        g_calls[g_ncalls].mask = nspe_ctrl.virq_mask;
    }
    g_ncalls++;
    return g_ret;
}

void nspe_trap_entry(void)
{
}

static void handler(void)
{
    g_handled++;
}

static void reset_calls(void)
{
    memset(g_calls, 0, sizeof(g_calls));
    g_ncalls = 0;
    g_ret = 0;
}

static void clear_pending(void)
{
    for (uint32_t w = 0; w < VIRQ_WORDS; w++) {
        nspe_ctrl.virq_pending[w] = 0;
    }
}

static void set_pending(uint32_t id)
{
    nspe_ctrl.virq_pending[id / 32u] |= 1u << (id % 32u);
}

static void test_init(void)
{
    reset_calls();
    nspe_ctrl.virq_mask = 0;
    check(nspe_runtime_init() == 0, "init returns 0");
    check(g_ncalls == 1 && g_calls[0].fn == NSPE_ECALL_INIT,
          "init issues one NSPE_ECALL_INIT");
    check(g_calls[0].a0 == (uint32_t)(uintptr_t)nspe_trap_entry &&
              g_calls[0].a1 == (uint32_t)(uintptr_t)&nspe_ctrl,
          "INIT args = trap entry, ctrl block");
    check(g_calls[0].mask == 1, "mask is 1 before INIT reaches the SPE");
    check(((uintptr_t)&nspe_ctrl & 63u) == 0, "ctrl block 64-byte aligned");
    check(nspe_crit_nesting == NSPE_CRIT_BOOT,
          "nesting starts at the boot value");
    check(nspe_runtime_init() == 0 && g_ncalls == 1,
          "second init: no second INIT ecall");

    reset_calls();
    nspe_critical_enter();
    nspe_critical_exit();
    check(nspe_ctrl.virq_mask == 1 && g_ncalls == 0,
          "critical exit before the scheduler keeps the mask");
}

static void test_enable_disable(void)
{
    reset_calls();
    clear_pending();
    nspe_virq_disable();
    check(nspe_ctrl.virq_mask == 1 && g_ncalls == 0,
          "disable: mask 1, no ecall");
    nspe_virq_enable();
    check(nspe_ctrl.virq_mask == 0 && g_ncalls == 0,
          "enable, nothing pending: mask 0, no ecall");

    nspe_virq_disable();
    set_pending(VIRQ_TICK);
    nspe_virq_enable();
    check(g_ncalls == 1 && g_calls[0].fn == NSPE_ECALL_IRQ_DELIVER,
          "enable with VIRQ_TICK pending: IRQ_DELIVER");
    check(g_calls[0].mask == 0, "IRQ_DELIVER issued after unmask");

    reset_calls();
    clear_pending();
    nspe_virq_disable();
    set_pending(NSPE_IRQ_IDS - 1u);
    nspe_virq_enable();
    check(g_ncalls == 1 && g_calls[0].fn == NSPE_ECALL_IRQ_DELIVER,
          "pending in the last bitmap word is seen");
    clear_pending();
}

static void test_nesting(void)
{
    reset_calls();
    nspe_crit_nesting = 0;
    nspe_ctrl.virq_mask = 0;
    nspe_critical_enter();
    nspe_critical_enter();
    check(nspe_ctrl.virq_mask == 1 && nspe_crit_nesting == 2,
          "two enters: mask 1, nesting 2");
    set_pending(VIRQ_YIELD);
    nspe_critical_exit();
    check(nspe_ctrl.virq_mask == 1 && g_ncalls == 0,
          "inner exit keeps the mask, no deliver");
    nspe_critical_exit();
    check(nspe_ctrl.virq_mask == 0 && nspe_crit_nesting == 0,
          "outer exit unmasks");
    check(g_ncalls == 1 && g_calls[0].fn == NSPE_ECALL_IRQ_DELIVER,
          "outer exit with pending: IRQ_DELIVER");
    clear_pending();
}

static void test_mask_from_isr(void)
{
    uint32_t outer;
    uint32_t inner;

    reset_calls();
    nspe_crit_nesting = 0;
    nspe_ctrl.virq_mask = 0;
    outer = nspe_mask_save();
    inner = nspe_mask_save();
    check(outer == 0 && inner == 1 && nspe_ctrl.virq_mask == 1,
          "mask_save returns the previous mask and masks");
    set_pending(VIRQ_TICK);
    nspe_mask_restore(inner);
    check(nspe_ctrl.virq_mask == 1 && g_ncalls == 0,
          "restore(1) keeps the mask");
    nspe_mask_restore(outer);
    check(nspe_ctrl.virq_mask == 0 && g_ncalls == 1 &&
              g_calls[0].fn == NSPE_ECALL_IRQ_DELIVER,
          "restore(0) unmasks and delivers");
    clear_pending();
}

static void test_trap_bracket(void)
{
    uint32_t saved;

    reset_calls();
    nspe_crit_nesting = 0;
    check(nspe_in_trap() == 0, "not in trap in task context");

    /* the SPE masks on delivery */
    nspe_ctrl.virq_mask = 1;
    saved = nspe_trap_enter();
    check(nspe_in_trap() == 1 && nspe_crit_nesting == 1,
          "trap enter: in_trap 1, nesting 1");
    set_pending(VIRQ_TICK);
    nspe_critical_enter();
    nspe_critical_exit();
    check(nspe_ctrl.virq_mask == 1 && g_ncalls == 0,
          "critical section in the trap keeps the mask");
    check(nspe_mask_save() == 1, "mask_save in the trap returns 1");
    nspe_mask_restore(1);

    nspe_yield();
    check(g_ncalls == 0, "yield in the trap: no ecall");
    check(nspe_switch_take() == 1 && nspe_switch_take() == 0,
          "yield in the trap: switch flag taken once");

    nspe_trap_leave(saved);
    check(nspe_in_trap() == 0 && nspe_crit_nesting == 0,
          "trap leave restores in_trap and nesting");
    check(nspe_ctrl.virq_mask == 1 && g_ncalls == 0,
          "trap leave leaves the mask to IRQ_RETURN");

    nspe_ctrl.virq_mask = 0;
    nspe_yield();
    check(g_ncalls == 1 && g_calls[0].fn == NSPE_ECALL_YIELD,
          "yield in a task: NSPE_ECALL_YIELD");
    check(nspe_switch_take() == 0, "yield in a task sets no flag");
    clear_pending();
}

static void test_irq_table(void)
{
    reset_calls();
    g_handled = 0;
    check(nspe_irq_attach(NSPE_IRQ_IDS, handler) == -1 && g_ncalls == 0,
          "attach id 96 refused without ecall");
    check(nspe_irq_attach(19, NULL) == -1 && g_ncalls == 0,
          "attach NULL handler refused");
    check(nspe_irq_dispatch(NSPE_IRQ_IDS) == -1,
          "dispatch id 96 refused");
    check(nspe_irq_dispatch(NSPE_IRQ_NONE) == -1,
          "dispatch NSPE_IRQ_NONE refused");

    check(nspe_irq_attach(NSPE_IRQ_IDS - 1u, handler) == 0,
          "attach id 95 ok");
    check(g_ncalls == 1 && g_calls[0].fn == NSPE_ECALL_IRQ_ATTACH &&
              g_calls[0].a0 == NSPE_IRQ_IDS - 1u,
          "attach issues IRQ_ATTACH(id)");
    check(nspe_irq_dispatch(NSPE_IRQ_IDS - 1u) == 0 && g_handled == 1,
          "dispatch runs the attached handler");

    reset_calls();
    check(nspe_irq_detach(NSPE_IRQ_IDS - 1u) == 0 &&
              g_ncalls == 1 && g_calls[0].fn == NSPE_ECALL_IRQ_DETACH,
          "detach issues IRQ_DETACH(id)");
    check(nspe_irq_dispatch(NSPE_IRQ_IDS - 1u) == -1 && g_handled == 1,
          "dispatch after detach finds no handler");
    check(nspe_irq_detach(NSPE_IRQ_IDS) == -1,
          "detach id 96 refused");

    reset_calls();
    g_ret = -1;
    check(nspe_irq_attach(30, handler) == -1,
          "attach refused by the SPE returns -1");
    check(nspe_irq_dispatch(30) == -1,
          "refused attach leaves no handler");
}

int main(void)
{
    test_init();
    test_enable_disable();
    test_nesting();
    test_mask_from_isr();
    test_trap_bracket();
    test_irq_table();
    return check_summary();
}
