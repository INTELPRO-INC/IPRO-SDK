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
 * Virtual IRQ state machine. Every NSPE source is level triggered
 * and stays disabled at the ECLIC from the moment it fires until the
 * NSPE returns from its handler; VIRQ_TICK re-arms itself instead.
 */
#include <string.h>

#include "spm_virq.h"

#ifdef SPM_HOST
/* Recording stubs, defined by the host test. */
void spm_hw_irq_config(uint32_t id);
void spm_hw_irq_enable(uint32_t id);
void spm_hw_irq_disable(uint32_t id);
uint64_t spm_hw_now(void);
void spm_hw_tick_set(uint64_t cmp);
#else
#include "system_soc.h"

static void spm_hw_irq_config(uint32_t id)
{
    ECLIC_SetShvIRQ((IRQn_Type)id, ECLIC_NON_VECTOR_INTERRUPT);
    ECLIC_SetTrigIRQ((IRQn_Type)id, ECLIC_LEVEL_TRIGGER);
    ECLIC_SetLevelIRQ((IRQn_Type)id, INTLEVEL);
    ECLIC_SetPriorityIRQ((IRQn_Type)id, INTPRIORITY);
}

static void spm_hw_irq_enable(uint32_t id)
{
    ECLIC_EnableIRQ((IRQn_Type)id);
}

static void spm_hw_irq_disable(uint32_t id)
{
    ECLIC_DisableIRQ((IRQn_Type)id);
}

static uint64_t spm_hw_now(void)
{
    return SysTimer_GetLoadValue();
}

static void spm_hw_tick_set(uint64_t cmp)
{
    SysTimer_SetCompareValue(cmp);
}
#endif

#define SPM_VIRQ_IDS (32u * VIRQ_WORDS)

struct spm_virq_state {
    struct psa_nspe_ctrl *ctrl;
    uint32_t entry;
    uint32_t period;
    uint64_t tick_cmp;
    uint32_t attached[VIRQ_WORDS];
    uint32_t pending_hw[VIRQ_WORDS];
};

static struct spm_virq_state spm_virq;

static uint32_t bit(uint32_t id)
{
    return 1u << (id % 32u);
}

static int is_attached(uint32_t id)
{
    return id < SPM_VIRQ_IDS && (spm_virq.attached[id / 32u] & bit(id));
}

static void publish(void)
{
    if (spm_virq.ctrl == NULL) {
        return;
    }
    for (uint32_t w = 0; w < VIRQ_WORDS; w++) {
        spm_virq.ctrl->virq_pending[w] = spm_virq.pending_hw[w];
    }
}

static void set_pending(uint32_t id)
{
    spm_virq.pending_hw[id / 32u] |= bit(id);
    publish();
}

/* compare += period; now + period when that is not in the future */
static void tick_rearm(void)
{
    uint64_t now = spm_hw_now();
    uint64_t next = spm_virq.tick_cmp + spm_virq.period;

    if (next <= now) {
        next = now + spm_virq.period;
    }
    spm_virq.tick_cmp = next;
    spm_hw_tick_set(next);
}

int spm_virq_bind(struct psa_nspe_ctrl *ctrl, uint32_t entry)
{
    if (spm_virq.ctrl != NULL) {
        return -1;
    }
    spm_virq.ctrl = ctrl;
    spm_virq.entry = entry;
    publish();
    return 0;
}

void spm_virq_reset(void)
{
    for (uint32_t id = 0; id < SPM_VIRQ_IDS; id++) {
        if (is_attached(id)) {
            spm_hw_irq_disable(id);
        }
    }
    memset(&spm_virq, 0, sizeof(spm_virq));
}

struct psa_nspe_ctrl *spm_virq_ctrl(void)
{
    return spm_virq.ctrl;
}

uint32_t spm_virq_entry(void)
{
    return spm_virq.entry;
}

void spm_virq_hw_irq(uint32_t id)
{
    if (!is_attached(id)) {
        spm_hw_irq_disable(id);
        return;
    }
    if (id == VIRQ_TICK) {
        tick_rearm();
    } else {
        spm_hw_irq_disable(id);
    }
    set_pending(id);
    if (spm_virq.ctrl != NULL) {
        spm_virq.ctrl->stat_irq++;
    }
}

void spm_virq_raise(uint32_t id)
{
    if (id < SPM_VIRQ_IDS) {
        set_pending(id);
    }
}

int spm_virq_attach(uint32_t id)
{
    if (id >= SPM_VIRQ_IDS) {
        return -1;
    }
    spm_virq.attached[id / 32u] |= bit(id);
    spm_hw_irq_config(id);
    spm_hw_irq_enable(id);
    return 0;
}

int spm_virq_detach(uint32_t id)
{
    if (!is_attached(id)) {
        return -1;
    }
    spm_hw_irq_disable(id);
    spm_virq.attached[id / 32u] &= ~bit(id);
    spm_virq.pending_hw[id / 32u] &= ~bit(id);
    publish();
    return 0;
}

void spm_virq_irq_done(uint32_t id)
{
    if (is_attached(id)) {
        spm_hw_irq_enable(id);
    }
}

void spm_virq_irq_return(uint32_t id)
{
    spm_virq_set_mask(0);
    spm_virq_irq_done(id);
}

int spm_virq_timer_set(uint32_t period)
{
    if (period == 0) {
        spm_virq_detach(VIRQ_TICK);
        spm_virq.period = 0;
        return 0;
    }
    spm_virq.period = period;
    spm_virq.tick_cmp = spm_hw_now() + period;
    spm_hw_tick_set(spm_virq.tick_cmp);
    return spm_virq_attach(VIRQ_TICK);
}

uint32_t spm_virq_masked(void)
{
    return spm_virq.ctrl == NULL || spm_virq.ctrl->virq_mask != 0;
}

void spm_virq_set_mask(uint32_t masked)
{
    if (spm_virq.ctrl != NULL) {
        spm_virq.ctrl->virq_mask = masked;
    }
}

static int lowest_pending(void)
{
    for (uint32_t w = 0; w < VIRQ_WORDS; w++) {
        if (spm_virq.pending_hw[w] != 0) {
            return (int)(w * 32u + (uint32_t)__builtin_ctz(
                                       spm_virq.pending_hw[w]));
        }
    }
    return -1;
}

int spm_virq_next(void)
{
    return spm_virq_masked() ? -1 : lowest_pending();
}

int spm_virq_pending_any(void)
{
    return lowest_pending() >= 0;
}

void spm_virq_take(int id)
{
    if (id >= 0 && (uint32_t)id < SPM_VIRQ_IDS) {
        spm_virq.pending_hw[(uint32_t)id / 32u] &= ~bit((uint32_t)id);
        publish();
    }
}

int spm_virq_fault(uint32_t mcause, uint32_t mtval, uint32_t mepc)
{
    struct psa_nspe_ctrl *c = spm_virq.ctrl;

    if (spm_virq_masked()) {
        return -1;
    }
    c->fault_mcause = mcause;
    c->fault_mtval = mtval;
    c->fault_mepc = mepc;
    return 0;
}
