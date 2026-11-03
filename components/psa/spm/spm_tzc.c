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
 * TZC_SEC policy of the SPE. Region ID_EN: 4 bits, 3 = ID0 only;
 * slave filter: 2 bits {ID0, ID1}, 01 = ID0 only. A denied ID1
 * access reads 0xDEADxxxx and drops writes, with no bus error.
 */
#include "spm_tzc.h"

#include <stddef.h>

#include "hal_sec_common_v2.h"
#include "psa_ipro/memmap.h"
#include "spm_log.h"
#include "spm_periph.h"

#define SPM_TZC_BASE 0x30006000u

#ifdef SPM_HOST
/* Register model, defined by the host test. */
uint32_t spm_hw_tzc_rd(uint32_t off);
void spm_hw_tzc_wr(uint32_t off, uint32_t val);
#else
static uint32_t spm_hw_tzc_rd(uint32_t off)
{
    return *(volatile uint32_t *)(uintptr_t)(SPM_TZC_BASE + off);
}

static void spm_hw_tzc_wr(uint32_t off, uint32_t val)
{
    *(volatile uint32_t *)(uintptr_t)(SPM_TZC_BASE + off) = val;
}
#endif

/* OCRAM and MM2OCRAM blocks: 1 KB units of addr[17:10], inclusive */
#define SPM_TZC_KB(a)        (((a) >> 10) & 0xFFu)
#define SPM_TZC_SPE_RAM_R0   ((SPM_TZC_KB(PSA_BOOT2_PARAM_BASE) << 16) | \
                              SPM_TZC_KB(PSA_SPE_RAM_END - 1u))

_Static_assert(PSA_BOOT2_PARAM_BASE >= 0x11010000u &&
                   PSA_SPE_RAM_END <= 0x11050000u &&
                   SPM_TZC_KB(PSA_BOOT2_PARAM_BASE) != 0u,
               "SPE RAM outside the OCRAM block or at region value 0");
_Static_assert(SPM_TZC_KB(PSA_BOOT2_PARAM_BASE) <
                   SPM_TZC_KB(PSA_SPE_RAM_END - 1u),
               "SPE RAM wraps the 8-bit region value");

/* Memory block ctrl: R0 ID0 only and on; R1, R2, RX all IDs and off */
#define SPM_TZC_RAM_CTRL     0x0001FFF3u
#define SPM_TZC_RAM_LOCK     0x0F000000u
#define SPM_TZC_RAM_R0_LOCK  0x01000000u
#define SPM_TZC_RAM_MASK_LOCK 0x00010000u
/* SF: RX (all of flash, raw offsets) ID0 only, R0-R3 off */
#define SPM_TZC_SF_CTRL      0x0103FFFFu
#define SPM_TZC_SF_LOCK      0x3E000000u
#define SPM_TZC_SF_MASK_LOCK 0x80000000u

#define SPM_TZMID_M154       (1u << 0)
#define SPM_TZMID_CCI        (1u << 1)
#define SPM_TZMID_EMAC       (1u << 2)
#define SPM_TZMID_USB        (1u << 3)
#define SPM_TZMID_SDHA       (1u << 6)
#define SPM_TZMID_SEL        0x007F0000u
#ifdef CONFIG_PSA_TZC_CCI_ID1
#define SPM_TZC_CCI          1u
#else
#define SPM_TZC_CCI          0u
#endif
#define SPM_TZMID_CCI_ID     (SPM_TZC_CCI * SPM_TZMID_CCI)
/* MCU_IPORT / MCU_DPORT (the CPU) stay ID0 */
#define SPM_TZMID_VAL        (SPM_TZMID_SEL | SPM_TZMID_M154 | \
                              SPM_TZMID_EMAC | SPM_TZMID_USB | \
                              SPM_TZMID_SDHA | SPM_TZMID_CCI_ID)

/*
 * AHB slaves PWR (PDS, HBN, HBN RAM), EMAC, DMA, MM, SDHA ID0 only,
 * USB both IDs (its EP FIFOs stay open to DMA0); locks inline
 */
#define SPM_TZC_AHB          0x0000055Du
#define SPM_TZC_AHB_LOCK     0x003F0000u
/*
 * SE_CTRL_0: SHA, AES, TRNG, PKA, CDET, GMAC ID0 only, CRMD on, WDT
 * delay 5; SE_CTRL_1: SF_CR, SF_SEC ID0 only, CRMD on. Without CRMD
 * the SE and SF filters pass ID1. Locks in SE_CTRL_2.
 */
#define SPM_TZC_SE           0x00051555u
#define SPM_TZC_SE_LOCK      0x0000007Fu
#define SPM_TZC_SE_OPEN      0x00050FFFu
#define SPM_TZC_SF_SLV       0x00000015u
#define SPM_TZC_SF_SLV_LOCK  0x00070000u

/* 16 APB slots of 2 bits per register; slot n = 0x30000000 + n * 4K */
#define SPM_TZC_APB_ID0      0x55555555u
#define SPM_TZC_APB_SLOTS    16u
#define SPM_TZC_APB_BASE     0x30000000u
#define SPM_TZC_APB_END      0x30020000u

enum {
    SPM_TZC_APB_0 = 7,
    SPM_TZC_APB_1 = 8,
};

static struct spm_tzc_reg spm_tzc_regs[] = {
    { 0x020, 0x020, SPM_TZC_RAM_CTRL, SPM_TZC_RAM_LOCK, 0x0000FFFFu },
    { 0x024, 0x024, 0, SPM_TZC_RAM_MASK_LOCK, 0xFFu },
    { 0x028, 0x020, SPM_TZC_SPE_RAM_R0, SPM_TZC_RAM_R0_LOCK, 0 },
    { 0x060, 0x060, SPM_TZC_RAM_CTRL, SPM_TZC_RAM_LOCK, 0x0000FFFFu },
    { 0x064, 0x064, 0, SPM_TZC_RAM_MASK_LOCK, 0xFFu },
    { 0x068, 0x060, SPM_TZC_SPE_RAM_R0, SPM_TZC_RAM_R0_LOCK, 0 },
    { 0x100, 0x104, SPM_TZMID_VAL, 0x7Fu, SPM_TZMID_SEL },
    [SPM_TZC_APB_0] = { 0x10C, 0x114, SPM_TZC_APB_ID0, 0x0000FFFFu,
                        0xFFFFFFFFu },
    [SPM_TZC_APB_1] = { 0x110, 0x114, SPM_TZC_APB_ID0, 0xFFFF0000u,
                        0xFFFFFFFFu },
    /* DMA channels 0-7 ID1 */
    { 0x200, 0x204, 0x00FF00FFu, 0xFFu, 0x00FF0000u },
    /* DMA_CTL, DMA_CH0-7 and WDT0, TMR2, TMR3 registers ID0 only */
    { 0x208, 0x20C, 0x00015555u, 0x1FFu, 0x0003FFFFu },
    { 0x220, 0x224, 0x15u, 0x7u, 0x3Fu },
    { 0x280, 0x280, SPM_TZC_SF_CTRL, SPM_TZC_SF_LOCK, 0x000FFFFFu },
    { 0x284, 0x284, 0, SPM_TZC_SF_MASK_LOCK, 0xFFu },
    { 0x108, 0x108, SPM_TZC_AHB, SPM_TZC_AHB_LOCK, 0x00000FFFu },
    { 0xF40, 0xF48, SPM_TZC_SE, SPM_TZC_SE_LOCK, SPM_TZC_SE_OPEN },
    { 0xF44, 0xF48, SPM_TZC_SF_SLV, SPM_TZC_SF_SLV_LOCK, 0x0000000Fu },
};

#define SPM_TZC_COUNT (sizeof(spm_tzc_regs) / sizeof(spm_tzc_regs[0]))

/* Opens both IDs on the APB slot of each spm_periph window */
static void spm_tzc_build(void)
{
    uint32_t apb[2] = { SPM_TZC_APB_ID0, SPM_TZC_APB_ID0 };

    for (uint32_t i = 0; i < SPM_PERIPH_COUNT; i++) {
        const struct spm_periph *p = spm_periph_at(i);

        for (uint32_t a = p->base; a < p->base + p->size; a += 0x1000u) {
            uint32_t slot;

            if (a < SPM_TZC_APB_BASE || a >= SPM_TZC_APB_END) {
                continue;
            }
            slot = (a - SPM_TZC_APB_BASE) >> 12;
            apb[slot / SPM_TZC_APB_SLOTS] |=
                3u << (2u * (slot % SPM_TZC_APB_SLOTS));
        }
    }
    spm_tzc_regs[SPM_TZC_APB_0].val = apb[0];
    spm_tzc_regs[SPM_TZC_APB_1].val = apb[1];
}

uint32_t spm_tzc_count(void)
{
    return SPM_TZC_COUNT;
}

const struct spm_tzc_reg *spm_tzc_at(uint32_t i)
{
    spm_tzc_build();
    return i < SPM_TZC_COUNT ? &spm_tzc_regs[i] : NULL;
}

/* Expected readback of r: value, plus its lock bits when inline */
static uint32_t spm_tzc_expect(const struct spm_tzc_reg *r)
{
    return r->lock_off == r->off ? r->val | r->lock : r->val;
}

static int spm_tzc_check(const char *when)
{
    int bad = 0;

    for (uint32_t i = 0; i < SPM_TZC_COUNT; i++) {
        const struct spm_tzc_reg *r = &spm_tzc_regs[i];
        uint32_t v = spm_hw_tzc_rd(r->off);
        uint32_t l = spm_hw_tzc_rd(r->lock_off);

        if (v != spm_tzc_expect(r) || (l & r->lock) != r->lock) {
            SPM_LOG("TZC %s +%03x = %08lx lock %08lx, want %08lx",
                    when, (unsigned)r->off, (unsigned long)v,
                    (unsigned long)l, (unsigned long)spm_tzc_expect(r));
            bad = 1;
        }
    }
    return bad;
}

void spm_tzc_apply(void)
{
    if (hal_sec_v2_claim() != HAL_SEC_V2_OK) {
        SPM_LOG("SEC_ENG group0 claim failed");
        spm_fatal();
    }
    spm_tzc_build();
    for (uint32_t i = 0; i < SPM_TZC_COUNT; i++) {
        spm_hw_tzc_wr(spm_tzc_regs[i].off, spm_tzc_regs[i].val);
    }
    for (uint32_t i = 0; i < SPM_TZC_COUNT; i++) {
        const struct spm_tzc_reg *r = &spm_tzc_regs[i];

        spm_hw_tzc_wr(r->lock_off, spm_hw_tzc_rd(r->lock_off) | r->lock);
    }
    if (spm_tzc_check("lock")) {
        spm_fatal();
    }
    for (uint32_t i = 0; i < SPM_TZC_COUNT; i++) {
        spm_hw_tzc_wr(spm_tzc_regs[i].off, spm_tzc_regs[i].open);
    }
    if (spm_tzc_check("tamper")) {
        spm_fatal();
    }
    SPM_LOG("TZC isolation locked: APB %08lx %08lx, CCI ID%u, "
            "SEC_ENG group0",
            (unsigned long)spm_tzc_regs[SPM_TZC_APB_0].val,
            (unsigned long)spm_tzc_regs[SPM_TZC_APB_1].val,
            (unsigned)SPM_TZC_CCI);
}
