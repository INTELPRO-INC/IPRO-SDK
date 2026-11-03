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
 * spm_tzc_apply() on a TZC_SEC register model: locked registers drop
 * writes, lock registers only set bits. Policy values, the APB slot
 * of UART0, the CCI ID per config, the SEC_ENG group0 claim, and
 * spm_fatal when a lock does not hold, a value does not read back or
 * the claim fails.
 */
#include <setjmp.h>
#include <stdint.h>
#include <string.h>

#include "check.h"
#include "hal_sec_common_v2.h"
#include "spm_tzc.h"

#define REGS 0x1000u

struct model {
    uint32_t r[REGS / 4u];
    uint32_t broken_lock;   /* this offset ignores its lock */
    uint32_t stuck_zero;    /* this offset always reads 0 */
};

static struct model g_m;
static int g_claim_ret;
static int g_claims;
static jmp_buf g_fatal_jmp;
static int g_fatal;

static const struct spm_tzc_reg *find(uint32_t off)
{
    for (uint32_t i = 0; i < spm_tzc_count(); i++) {
        if (spm_tzc_at(i)->off == off) {
            return spm_tzc_at(i);
        }
    }
    return NULL;
}

/* 1 when any lock bit of the policy entry at off is set */
static int locked(uint32_t off)
{
    const struct spm_tzc_reg *r = find(off);

    if (r == NULL || off == g_m.broken_lock) {
        return 0;
    }
    return (g_m.r[r->lock_off / 4u] & r->lock) != 0;
}

static int is_lock_reg(uint32_t off)
{
    if (find(off) != NULL) {
        return 0;
    }
    for (uint32_t i = 0; i < spm_tzc_count(); i++) {
        const struct spm_tzc_reg *r = spm_tzc_at(i);

        if (r->lock_off == off && r->lock_off != r->off) {
            return 1;
        }
    }
    return 0;
}

uint32_t spm_hw_tzc_rd(uint32_t off)
{
    return off == g_m.stuck_zero ? 0 : g_m.r[off / 4u];
}

void spm_hw_tzc_wr(uint32_t off, uint32_t val)
{
    const struct spm_tzc_reg *r = find(off);

    if (is_lock_reg(off)) {
        g_m.r[off / 4u] |= val;
    } else if (!locked(off)) {
        g_m.r[off / 4u] = val;
    } else if (r != NULL && r->lock_off == off) {
        /* inline lock bits stay set */
        g_m.r[off / 4u] |= val & r->lock;
    }
}

int hal_sec_v2_claim(void)
{
    g_claims++;
    return g_claim_ret;
}

void spm_fatal(void)
{
    g_fatal = 1;
    longjmp(g_fatal_jmp, 1);
}

static void reset_model(void)
{
    memset(&g_m, 0, sizeof(g_m));
    g_m.broken_lock = REGS;
    g_m.stuck_zero = REGS;
    g_m.r[0x020 / 4u] = 0x0000FFFFu;
    g_m.r[0x060 / 4u] = 0x0000FFFFu;
    g_m.r[0x100 / 4u] = 0x007F0000u;
    g_m.r[0x10C / 4u] = 0xFFFFFFFFu;
    g_m.r[0x110 / 4u] = 0xFFFFFFFFu;
    g_m.r[0x200 / 4u] = 0x00FF0000u;
    g_m.r[0x108 / 4u] = 0x00000FFFu;
    g_m.r[0xF40 / 4u] = 0x00050FFFu;
    g_m.r[0xF44 / 4u] = 0x0000000Fu;
    g_fatal = 0;
    g_claim_ret = HAL_SEC_V2_OK;
    g_claims = 0;
}

static int run(void)
{
    if (setjmp(g_fatal_jmp) == 0) {
        spm_tzc_apply();
    }
    return g_fatal;
}

int main(void)
{
    reset_model();
    check(run() == 0, "good model: no fatal");
    check(g_claims == 1, "SEC_ENG group0 claimed once");
    check(g_m.r[0x028 / 4u] == 0x0040007Fu,
          "OCRAM R0 = 0x11010000..0x1101FFFF");
    check(g_m.r[0x068 / 4u] == 0x0040007Fu, "MM2OCRAM R0 = OCRAM R0");
    check(g_m.r[0x020 / 4u] == 0x0F01FFF3u, "OCRAM ctrl R0 ID0, locked");
    check(g_m.r[0x280 / 4u] == 0x3F03FFFFu, "SF RX ID0, locked");
    check(g_m.r[0x10C / 4u] == 0x55D55555u, "APB 0x00-0x0F ID0 but UART0");
    check(g_m.r[0x110 / 4u] == 0x55555555u, "APB 0x10-0x1F ID0");
    check(g_m.r[0x114 / 4u] == 0xFFFFFFFFu, "APB slot locks");
    check(g_m.r[0x200 / 4u] == 0x00FF00FFu && g_m.r[0x204 / 4u] == 0xFFu,
          "DMA channels ID1, locked");
#ifdef CONFIG_PSA_TZC_CCI_ID1
    check(g_m.r[0x100 / 4u] == 0x007F004Fu, "TZMID: CCI ID1");
#else
    check(g_m.r[0x100 / 4u] == 0x007F004Du, "TZMID: CCI ID0");
#endif
    check((g_m.r[0x100 / 4u] & 0x30u) == 0, "CPU ports ID0");
    check(g_m.r[0x104 / 4u] == 0x7Fu, "TZMID locks");
    check(g_m.r[0x108 / 4u] == 0x003F055Du,
          "AHB slaves ID0 but USB, locked");
    check(g_m.r[0xF40 / 4u] == 0x00051555u, "SE slaves ID0, CRMD on");
    check(g_m.r[0xF44 / 4u] == 0x00000015u, "SF_CR, SF_SEC ID0, CRMD on");
    check(g_m.r[0xF48 / 4u] == 0x0007007Fu, "SE and SF locks");

    check(run() == 0, "second apply on the locked model: no fatal");

    reset_model();
    g_m.broken_lock = 0x10C;
    check(run() == 1, "APB lock that does not hold: fatal");
    check(g_m.r[0x10C / 4u] == 0xFFFFFFFFu, "tamper value took");

    reset_model();
    g_m.broken_lock = 0x028;
    check(run() == 1, "OCRAM R0 lock that does not hold: fatal");

    reset_model();
    g_m.broken_lock = 0xF44;
    check(run() == 1, "SF slave lock that does not hold: fatal");

    reset_model();
    g_m.stuck_zero = 0x200;
    check(run() == 1, "DMA_M_ID readback 0: fatal");

    reset_model();
    g_claim_ret = HAL_SEC_V2_ERR_ACCESS;
    check(run() == 1, "SEC_ENG claim failed: fatal");
    check(g_m.r[0x020 / 4u] == 0x0000FFFFu, "no TZC write after it");

    check(spm_tzc_at(spm_tzc_count()) == NULL, "spm_tzc_at past the end");
    return check_summary();
}
