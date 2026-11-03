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
 * Virtual IRQ state for the NSPE: attached ECLIC sources, pending
 * bitmap, virtual mask and fault record. Pure C (host-testable).
 */
#pragma once

#include <stdint.h>

#include "psa_ipro/abi.h"

/* Bind the NSPE control block and trap entry; -1 when bound. */
int spm_virq_bind(struct psa_nspe_ctrl *ctrl, uint32_t entry);

/* Unbind, disable every attached source, clear all state. */
void spm_virq_reset(void);

struct psa_nspe_ctrl *spm_virq_ctrl(void);
uint32_t spm_virq_entry(void);

/* Hardware interrupt id taken by the SPE (from U or in WFI). */
void spm_virq_hw_irq(uint32_t id);

/* Mark an SPE event (VIRQ_YIELD, VIRQ_NOTIFY) pending. */
void spm_virq_raise(uint32_t id);

int spm_virq_attach(uint32_t id);
int spm_virq_detach(uint32_t id);

/* Re-enable an attached source after the NSPE handled it. */
void spm_virq_irq_done(uint32_t id);

/* End of a vIRQ handler: unmask, then spm_virq_irq_done(id). */
void spm_virq_irq_return(uint32_t id);

/* Periodic VIRQ_TICK every period SysTimer ticks; 0 stops it. */
int spm_virq_timer_set(uint32_t period);

/* Virtual interrupt mask; reads 1 while no ctrl is bound. */
uint32_t spm_virq_masked(void);
void spm_virq_set_mask(uint32_t masked);

/* Lowest pending id when unmasked, else -1. */
int spm_virq_next(void);
void spm_virq_take(int id);

/* 1 when any vIRQ is pending, masked or not. */
int spm_virq_pending_any(void);

/* Record a NSPE fault; 0 when VIRQ_FAULT can be delivered now. */
int spm_virq_fault(uint32_t mcause, uint32_t mtval, uint32_t mepc);
