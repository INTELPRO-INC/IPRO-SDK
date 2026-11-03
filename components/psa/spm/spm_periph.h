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
 * NSPE peripherals: MMIO windows U-mode may use, each with one ECLIC
 * source; and single SPE registers whose U-mode lw/sw the SPE does.
 */
#pragma once

#include <stdint.h>

struct spm_periph {
    uint32_t base;
    uint32_t size;
    uint32_t irq;
    /*
     * sets the block's NSPE start state at every NSPE launch (clock
     * on, no bus-master traffic); or NULL
     */
    void (*reset)(void);
};

struct spm_reg_grant {
    uint32_t addr;
    /* bits a U-mode store may change; the others keep their value */
    uint32_t wmask;
    /* bits a U-mode store sets to fval, whatever it wrote */
    uint32_t fmask;
    uint32_t fval;
};

/* entries of spm_periphs[] and spm_reg_grants[] in spm_periph.c */
#ifdef CONFIG_PSA_NSPE_DMA_USB
#define SPM_PERIPH_COUNT    3u
#define SPM_REG_GRANT_COUNT 4u
#else
#define SPM_PERIPH_COUNT    1u
#define SPM_REG_GRANT_COUNT 0u
#endif

/* Peripheral i; NULL when i >= SPM_PERIPH_COUNT */
const struct spm_periph *spm_periph_at(uint32_t i);

/* 1 when irq is the ECLIC source of a table peripheral */
int spm_periph_irq_ok(uint32_t irq);

/* Runs the reset hook of every table peripheral */
void spm_periph_reset(void);

/* Grant of the register at addr; NULL when addr has none */
const struct spm_reg_grant *spm_reg_grant_find(uint32_t addr);
