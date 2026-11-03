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
 * Host test for the virtual IRQ state machine (spm_virq.c built
 * with -DSPM_HOST). ECLIC and SysTimer access go to the recording
 * stubs below. Runs every check; exits 1 when any check failed.
 */
#include <string.h>

#include "check.h"
#include "spm_virq.h"

#define NSPE_ENTRY 0x13040100u
#define TICK_PERIOD 4000u
#define STUB_IDS 128u

static uint32_t g_disabled[STUB_IDS / 32u];
static uint32_t g_enabled[STUB_IDS / 32u];
static uint64_t g_now;
static uint64_t g_cmp;
static struct psa_nspe_ctrl g_ctrl;

static void mark(uint32_t *map, uint32_t id)
{
    if (id < STUB_IDS) {
        map[id / 32u] |= 1u << (id % 32u);
    }
}

static int marked(const uint32_t *map, uint32_t id)
{
    return id < STUB_IDS && ((map[id / 32u] >> (id % 32u)) & 1u);
}

void spm_hw_irq_config(uint32_t id)
{
    (void)id;
}

void spm_hw_irq_enable(uint32_t id)
{
    mark(g_enabled, id);
}

void spm_hw_irq_disable(uint32_t id)
{
    mark(g_disabled, id);
}

uint64_t spm_hw_now(void)
{
    return g_now;
}

void spm_hw_tick_set(uint64_t cmp)
{
    g_cmp = cmp;
}

static int pending(uint32_t id)
{
    return (g_ctrl.virq_pending[id / 32] >> (id % 32)) & 1u;
}

static void clear_stubs(void)
{
    memset(g_disabled, 0, sizeof(g_disabled));
    memset(g_enabled, 0, sizeof(g_enabled));
}

static void fresh(void)
{
    spm_virq_reset();
    memset(&g_ctrl, 0, sizeof(g_ctrl));
    clear_stubs();
    g_now = 0;
    g_cmp = 0;
    spm_virq_bind(&g_ctrl, NSPE_ENTRY);
}

static void test_basic(void)
{
    fresh();
    check(spm_virq_bind(&g_ctrl, NSPE_ENTRY) == -1, "second bind refused");
    check(spm_virq_entry() == NSPE_ENTRY, "entry stored");

    spm_virq_attach(19);
    check(marked(g_enabled, 19), "attach enables the source");
    g_ctrl.virq_mask = 1;
    spm_virq_hw_irq(19);
    check(pending(19), "masked: hw irq 19 pending");
    check(spm_virq_next() == -1, "masked: nothing to deliver");
    check(spm_virq_pending_any() == 1, "masked: pending_any sees 19");
    check(marked(g_disabled, 19), "masked: source 19 disabled at ECLIC");

    g_ctrl.virq_mask = 0;
    check(spm_virq_next() == 19, "unmasked: next is 19");
    spm_virq_take(19);
    check(!pending(19) && spm_virq_next() == -1, "take clears pending");
    check(spm_virq_pending_any() == 0, "pending_any 0 when none");

    fresh();
    spm_virq_hw_irq(25);
    check(!pending(25) && spm_virq_next() == -1, "unattached 25 ignored");
    check(marked(g_disabled, 25), "unattached 25 disabled at ECLIC");
}

static void test_word1(void)
{
    fresh();
    spm_virq_attach(48);
    spm_virq_attach(49);
    spm_virq_attach(63);
    check(marked(g_enabled, 48) && marked(g_enabled, 49) &&
          marked(g_enabled, 63), "attach 48, 49, 63 enables them");
    spm_virq_hw_irq(63);
    spm_virq_hw_irq(49);
    spm_virq_hw_irq(48);
    check(g_ctrl.virq_pending[0] == 0 &&
          g_ctrl.virq_pending[1] == 0x80030000u &&
          g_ctrl.virq_pending[2] == 0, "48, 49, 63 pending in word 1");
    check(marked(g_disabled, 48) && marked(g_disabled, 49) &&
          marked(g_disabled, 63), "48, 49, 63 disabled at ECLIC");
    check(spm_virq_next() == 48, "48 first");
    spm_virq_take(48);
    check(spm_virq_next() == 49, "then 49");
    spm_virq_take(49);
    check(spm_virq_next() == 63, "then 63");
    spm_virq_take(63);
    check(g_ctrl.virq_pending[1] == 0 && spm_virq_next() == -1,
          "word 1 empty after take");
}

static void test_out_of_range(void)
{
    fresh();
    check(spm_virq_attach(96) == -1, "attach 96 refused");
    check(!marked(g_enabled, 96), "96 never enabled");
    spm_virq_hw_irq(96);
    check(marked(g_disabled, 96), "hw irq 96 disabled at ECLIC");
    spm_virq_raise(96);
    spm_virq_raise(200);
    spm_virq_take(96);
    check(g_ctrl.virq_pending[0] == 0 && g_ctrl.virq_pending[1] == 0 &&
          g_ctrl.virq_pending[2] == 0 && spm_virq_pending_any() == 0,
          "ids >= 96 never pending");
    check(spm_virq_detach(96) == -1, "detach 96 refused");
}

static void test_detach_done(void)
{
    fresh();
    spm_virq_attach(49);
    g_ctrl.virq_mask = 1;
    spm_virq_hw_irq(49);
    clear_stubs();
    check(spm_virq_detach(49) == 0, "detach attached 49");
    check(!pending(49) && spm_virq_pending_any() == 0,
          "detach clears 49 pending");
    check(marked(g_disabled, 49), "detach disables 49 at ECLIC");
    check(spm_virq_detach(49) == -1, "second detach refused");
    clear_stubs();
    spm_virq_hw_irq(49);
    check(!pending(49) && marked(g_disabled, 49),
          "hw irq after detach ignored and disabled");

    clear_stubs();
    spm_virq_irq_done(49);
    check(!marked(g_enabled, 49), "irq_done on detached 49 no enable");
    spm_virq_attach(48);
    clear_stubs();
    spm_virq_irq_done(48);
    check(marked(g_enabled, 48), "irq_done re-enables attached 48");
}

static void test_irq_return(void)
{
    fresh();
    spm_virq_attach(48);
    g_ctrl.virq_mask = 1;
    clear_stubs();
    spm_virq_irq_return(48);
    check(g_ctrl.virq_mask == 0, "irq_return clears the mask");
    check(marked(g_enabled, 48), "irq_return re-enables 48");
    g_ctrl.virq_mask = 1;
    clear_stubs();
    spm_virq_irq_return(0xFFFFFFFFu);
    check(g_ctrl.virq_mask == 0 && !marked(g_enabled, 48),
          "irq_return without irq only unmasks");
}

static void test_tick(void)
{
    fresh();
    g_now = 1000;
    spm_virq_timer_set(TICK_PERIOD);
    check(g_cmp == 1000u + TICK_PERIOD && marked(g_enabled, VIRQ_TICK),
          "timer_set arms now + period, enables 7");

    g_now = 5100;
    spm_virq_hw_irq(VIRQ_TICK);
    check(g_cmp == 9000u, "late tick: compare += period");
    check(pending(VIRQ_TICK) && !marked(g_disabled, VIRQ_TICK),
          "tick pending, source stays enabled");

    g_now = 17000;
    spm_virq_hw_irq(VIRQ_TICK);
    check(g_cmp == 21000u, "two periods behind: now + period");

    g_now = 25000;
    spm_virq_hw_irq(VIRQ_TICK);
    check(g_cmp == 25000u + TICK_PERIOD,
          "exactly one period behind: now + period");

    spm_virq_attach(19);
    spm_virq_hw_irq(19);
    check(spm_virq_next() == VIRQ_TICK, "pending 7 and 19: 7 first");
    spm_virq_take(VIRQ_TICK);
    check(spm_virq_next() == 19, "then 19");

    clear_stubs();
    spm_virq_timer_set(0);
    check(marked(g_disabled, VIRQ_TICK) && !pending(VIRQ_TICK),
          "timer_set 0 detaches the tick");
}

static void test_fault_reset(void)
{
    fresh();
    g_ctrl.virq_mask = 1;
    check(spm_virq_fault(7, 0x11010400u, 0x13040200u) == -1,
          "fault while masked asks for reset");
    g_ctrl.virq_mask = 0;
    check(spm_virq_fault(7, 0x11010400u, 0x13040200u) == 0 &&
          g_ctrl.fault_mcause == 7 && g_ctrl.fault_mepc == 0x13040200u,
          "fault while unmasked recorded");

    fresh();
    spm_virq_attach(48);
    spm_virq_attach(49);
    spm_virq_hw_irq(48);
    clear_stubs();
    spm_virq_reset();
    check(marked(g_disabled, 48) && marked(g_disabled, 49),
          "reset disables every attached source");
    check(spm_virq_ctrl() == NULL && spm_virq_masked() == 1,
          "reset unbinds: masked, no ctrl");
    check(spm_virq_pending_any() == 0 && spm_virq_next() == -1,
          "reset clears pending");
    clear_stubs();
    spm_virq_hw_irq(48);
    check(spm_virq_pending_any() == 0 && marked(g_disabled, 48),
          "after reset 48 is unattached");
    check(spm_virq_bind(&g_ctrl, NSPE_ENTRY) == 0, "bind after reset");
}

int main(void)
{
    test_basic();
    test_word1();
    test_out_of_range();
    test_detach_done();
    test_irq_return();
    test_tick();
    test_fault_reset();
    return check_summary();
}
