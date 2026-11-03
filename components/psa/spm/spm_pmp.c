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
 * NSPE PMP table, mirrored into sPMP on TEE parts: U-mode gets these
 * regions only; SPE memory, eFuse, SEC_ENG, ECLIC, other MMIO and
 * the 0x2xxxxxxx aliases match no entry and fault in U.
 */
#include "spm_pmp.h"

#include <stddef.h>

#include "psa_ipro/memmap.h"
#include "riscv_encoding.h"
#include "spm_log.h"
#include "spm_periph.h"

#ifdef SPM_HOST
/* Recording stubs, defined by the host test. */
uint32_t spm_hw_mcfg_info(void);
void spm_hw_pmpaddr(uint32_t i, uint32_t addr);
void spm_hw_pmpcfg(uint32_t r, uint32_t cfg);
void spm_hw_spmpaddr(uint32_t i, uint32_t addr);
void spm_hw_spmpcfg(uint32_t r, uint32_t cfg);
void spm_hw_smpuswitch(uint32_t mask);
void spm_hw_fence(void);
#else
#include "system_soc.h"
/* reads __PMP_PRESENT from ipro7.h (via system_soc.h) */
#include "core_feature_pmp.h"

static uint32_t spm_hw_mcfg_info(void)
{
    return (uint32_t)__RV_CSR_READ(CSR_MCFG_INFO);
}

static void spm_hw_pmpaddr(uint32_t i, uint32_t addr)
{
    __set_PMPADDRx(i, addr);
}

static void spm_hw_pmpcfg(uint32_t r, uint32_t cfg)
{
    __set_PMPCFGx(r, cfg);
}

static void spm_hw_spmpaddr(uint32_t i, uint32_t addr)
{
    switch (i) {
    case 0:
        __RV_CSR_WRITE(CSR_SPMPADDR0, addr);
        break;
    case 1:
        __RV_CSR_WRITE(CSR_SPMPADDR1, addr);
        break;
    case 2:
        __RV_CSR_WRITE(CSR_SPMPADDR2, addr);
        break;
    case 3:
        __RV_CSR_WRITE(CSR_SPMPADDR3, addr);
        break;
    case 4:
        __RV_CSR_WRITE(CSR_SPMPADDR4, addr);
        break;
    case 5:
        __RV_CSR_WRITE(CSR_SPMPADDR5, addr);
        break;
    case 6:
        __RV_CSR_WRITE(CSR_SPMPADDR6, addr);
        break;
    case 7:
        __RV_CSR_WRITE(CSR_SPMPADDR7, addr);
        break;
    default:
        break;
    }
}

static void spm_hw_spmpcfg(uint32_t r, uint32_t cfg)
{
    if (r == 0) {
        __RV_CSR_WRITE(CSR_SPMPCFG0, cfg);
    } else {
        __RV_CSR_WRITE(CSR_SPMPCFG1, cfg);
    }
}

static void spm_hw_smpuswitch(uint32_t mask)
{
    __RV_CSR_WRITE(CSR_SMPUSWITCH0, mask);
}

static void spm_hw_fence(void)
{
    __RWMB();
}
#endif

/* every hardware entry: entries past the table are written off */
#define SPM_PMP_ENTRIES 8u
/* entries 0..3: NSPE memory, then one per spm_periph window */
#define SPM_PMP_MEM_ENTRIES 4u
/* (s)pmpcfg0 holds entries 0..3, (s)pmpcfg1 entries 4..7 */
#define SPM_PMP_CFG_REGS 2u
#define SPM_PMP_PERIPH_PERM (PMP_R | PMP_W)
/* smpuswitch0 bit i switches sPMP entry i on */
#define SPM_SPMP_SWITCH ((1u << SPM_PMP_ENTRIES) - 1u)
/* sPMP cfg bits implemented: A, X, W, R */
#define SPM_SPMP_CFG_BITS (PMP_A | PMP_X | PMP_W | PMP_R)

struct spm_pmp_entry {
    uint32_t base;
    uint32_t size;
    uint32_t perm;
};

static const struct spm_pmp_entry spm_pmp_mem[SPM_PMP_MEM_ENTRIES] = {
    { PSA_NSPE_FLASH_BASE,  PSA_NSPE_FLASH_SIZE,  PMP_R | PMP_X },
    { PSA_NSPE_OCRAM_BASE,  PSA_NSPE_OCRAM_SIZE,  PMP_R | PMP_W | PMP_X },
    { PSA_NSPE_OCRAM2_BASE, PSA_NSPE_OCRAM2_SIZE, PMP_R | PMP_W },
    { PSA_NSPE_PSRAM_BASE,  PSA_NSPE_PSRAM_SIZE,  PMP_R | PMP_W | PMP_X },
};

_Static_assert(SPM_PMP_MEM_ENTRIES + SPM_PERIPH_COUNT <= SPM_PMP_ENTRIES,
               "NSPE regions exceed the PMP table");
_Static_assert(SPM_PMP_ENTRIES <= 4u * SPM_PMP_CFG_REGS, "cfg regs");

/* Entry i: NSPE memory, a peripheral (R W), else size 0 (off) */
static struct spm_pmp_entry spm_pmp_entry(uint32_t i)
{
    struct spm_pmp_entry e = { 0, 0, 0 };
    const struct spm_periph *p;

    if (i < SPM_PMP_MEM_ENTRIES) {
        return spm_pmp_mem[i];
    }
    p = spm_periph_at(i - SPM_PMP_MEM_ENTRIES);
    if (p != NULL) {
        e.base = p->base;
        e.size = p->size;
        e.perm = SPM_PMP_PERIPH_PERM;
    }
    return e;
}

uint32_t spm_pmp_napot(uint32_t base, uint32_t size)
{
    return (base >> 2) | ((size >> 3) - 1u);
}

uint8_t spm_spmp_cfg(uint8_t pmpcfg)
{
    return (uint8_t)(pmpcfg & SPM_SPMP_CFG_BITS);
}

/*
 * Ssmpu: sPMP entry i is active only while smpuswitch0[i] = 1 and
 * its cfg.A != 0. A U access that no active entry allows takes the
 * sPMP page fault (mcause 12/13/15, mdcause 6).
 */
static void spm_spmp_apply(const uint32_t *addr, const uint32_t *cfg)
{
    for (uint32_t i = 0; i < SPM_PMP_ENTRIES; i++) {
        spm_hw_spmpaddr(i, addr[i]);
    }
    for (uint32_t r = 0; r < SPM_PMP_CFG_REGS; r++) {
        spm_hw_spmpcfg(r, cfg[r]);
    }
    spm_hw_smpuswitch(SPM_SPMP_SWITCH);
}

void spm_pmp_apply(void)
{
    uint32_t addr[SPM_PMP_ENTRIES] = { 0 };
    uint32_t cfg[SPM_PMP_CFG_REGS] = { 0 };
    uint32_t scfg[SPM_PMP_CFG_REGS] = { 0 };

    for (uint32_t i = 0; i < SPM_PMP_ENTRIES; i++) {
        struct spm_pmp_entry e = spm_pmp_entry(i);
        uint32_t shift = (i % 4u) * 8u;
        uint8_t byte = 0;

        if (e.size != 0) {
            addr[i] = spm_pmp_napot(e.base, e.size);
            byte = (uint8_t)(e.perm | PMP_A_NAPOT);
        }
        cfg[i / 4u] |= (uint32_t)byte << shift;
        scfg[i / 4u] |= (uint32_t)spm_spmp_cfg(byte) << shift;
        spm_hw_pmpaddr(i, addr[i]);
    }
    for (uint32_t r = 0; r < SPM_PMP_CFG_REGS; r++) {
        spm_hw_pmpcfg(r, cfg[r]);
    }
    if (spm_hw_mcfg_info() & MCFG_INFO_TEE) {
        spm_spmp_apply(addr, scfg);
    } else {
        SPM_LOG("no TEE: sPMP not programmed");
    }
    spm_hw_fence();
}
