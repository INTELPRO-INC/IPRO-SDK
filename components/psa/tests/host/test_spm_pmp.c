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
 * spm_pmp_apply() on recording CSR stubs: PMP table, sPMP mirror and
 * smpuswitch0 with and without mcfg_info.TEE; spm_spmp_cfg() bytes;
 * the spm_periph lookups and register grants. Built twice: as is,
 * and with CONFIG_PSA_NSPE_DMA_USB (DMA0 and USB windows).
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "psa_ipro/memmap.h"
#include "riscv_encoding.h"
#include "spm_periph.h"
#include "spm_pmp.h"
#include "system_soc.h"

#define HW_ENTRIES   8u
#define HW_CFG_REGS  2u
#define UNSET        0xA5A5A5A5u
#define GRAIN        1024u
/* mcfg_info of the QFN32 dev part, TEE bit (0) set */
#define MCFG_SILICON 0x0001b685u

struct hw {
    uint32_t pmpaddr[HW_ENTRIES];
    uint32_t pmpcfg[HW_CFG_REGS];
    uint32_t spmpaddr[HW_ENTRIES];
    uint32_t spmpcfg[HW_CFG_REGS];
    uint32_t smpuswitch;
    uint32_t bad_index;
    uint32_t seq;
    uint32_t spmp_last_seq;   /* last spmpaddr/spmpcfg write */
    uint32_t switch_seq;
    uint32_t fence_seq;
};

static struct hw g_hw;
static uint32_t g_mcfg_info;

static uint32_t next_seq(void)
{
    return ++g_hw.seq;
}

static void put(uint32_t *a, uint32_t n, uint32_t i, uint32_t v)
{
    next_seq();
    if (i < n) {
        a[i] = v;
    } else {
        g_hw.bad_index++;
    }
}

uint32_t spm_hw_mcfg_info(void)
{
    return g_mcfg_info;
}

void spm_hw_pmpaddr(uint32_t i, uint32_t addr)
{
    put(g_hw.pmpaddr, HW_ENTRIES, i, addr);
}

void spm_hw_pmpcfg(uint32_t r, uint32_t cfg)
{
    put(g_hw.pmpcfg, HW_CFG_REGS, r, cfg);
}

void spm_hw_spmpaddr(uint32_t i, uint32_t addr)
{
    put(g_hw.spmpaddr, HW_ENTRIES, i, addr);
    g_hw.spmp_last_seq = g_hw.seq;
}

void spm_hw_spmpcfg(uint32_t r, uint32_t cfg)
{
    put(g_hw.spmpcfg, HW_CFG_REGS, r, cfg);
    g_hw.spmp_last_seq = g_hw.seq;
}

void spm_hw_smpuswitch(uint32_t mask)
{
    g_hw.smpuswitch = mask;
    g_hw.switch_seq = next_seq();
}

/* spm_periph.c reference; the DMA halt path is not run on the host */
void spm_fatal(void)
{
    abort();
}

void spm_hw_fence(void)
{
    g_hw.fence_seq = next_seq();
}

static void hw_reset(uint32_t mcfg_info)
{
    memset(&g_hw, 0, sizeof(g_hw));
    for (uint32_t i = 0; i < HW_ENTRIES; i++) {
        g_hw.pmpaddr[i] = UNSET;
        g_hw.spmpaddr[i] = UNSET;
    }
    for (uint32_t r = 0; r < HW_CFG_REGS; r++) {
        g_hw.pmpcfg[r] = UNSET;
        g_hw.spmpcfg[r] = UNSET;
    }
    g_hw.smpuswitch = UNSET;
    g_mcfg_info = mcfg_info;
}

/* Expected NSPE view; cfg = A_NAPOT (0x18) | X 4 | W 2 | R 1 */
static const struct {
    uint32_t base;
    uint32_t size;
    uint8_t cfg;
} want[HW_ENTRIES] = {
    { PSA_NSPE_FLASH_BASE, PSA_NSPE_FLASH_SIZE, 0x1D },
    { PSA_NSPE_OCRAM_BASE, PSA_NSPE_OCRAM_SIZE, 0x1F },
    { PSA_NSPE_OCRAM2_BASE, PSA_NSPE_OCRAM2_SIZE, 0x1B },
    { PSA_NSPE_PSRAM_BASE, PSA_NSPE_PSRAM_SIZE, 0x1F },
    { PSA_NSPE_UART_BASE, PSA_NSPE_UART_SIZE, 0x1B },
#ifdef CONFIG_PSA_NSPE_DMA_USB
    { PSA_NSPE_DMA_BASE, PSA_NSPE_DMA_SIZE, 0x1B },
    { PSA_NSPE_USB_BASE, PSA_NSPE_USB_SIZE, 0x1B },
#endif
};

#ifdef CONFIG_PSA_NSPE_DMA_USB
#define WANT_ENTRIES 7u
#define WANT_CFG1    0x001B1B1Bu
#define VARIANT      "DMA_USB: "
#else
#define WANT_ENTRIES 5u
#define WANT_CFG1    0x0000001Bu
#define VARIANT      ""
#endif

static uint8_t cfg_byte(const uint32_t *cfg, uint32_t i)
{
    return (uint8_t)(cfg[i / 4u] >> ((i % 4u) * 8u));
}

/* [base, base + size) of a NAPOT pmpaddr */
static void napot_decode(uint32_t a, uint32_t *base, uint32_t *size)
{
    uint32_t t = 0;

    while (t < 29u && ((a >> t) & 1u) != 0) {
        t++;
    }
    *size = 1u << (t + 3u);
    *base = (a & ~((1u << t) - 1u)) << 2;
}

/* 1 when PMP entries 0..7 hold want[], the rest written off */
static int pmp_matches(const uint32_t *addr, const uint32_t *cfg)
{
    for (uint32_t i = 0; i < HW_ENTRIES; i++) {
        uint32_t base;
        uint32_t size;

        if (cfg_byte(cfg, i) != want[i].cfg) {
            return 0;
        }
        if (want[i].size == 0) {
            if (addr[i] != 0) {
                return 0;
            }
            continue;
        }
        napot_decode(addr[i], &base, &size);
        if (base != want[i].base || size != want[i].size) {
            return 0;
        }
    }
    return 1;
}

static void test_regions_fit_grain(void)
{
    int ok = 1;

    for (uint32_t i = 0; i < WANT_ENTRIES; i++) {
        uint32_t s = want[i].size;

        ok = ok && s >= GRAIN && (s & (s - 1u)) == 0 &&
             (want[i].base & (s - 1u)) == 0;
    }
    check(ok, VARIANT "regions: power of two >= 1 KB grain, aligned");
}

static void test_tee(void)
{
    struct hw first;

    hw_reset(MCFG_SILICON);
    spm_pmp_apply();
    check(g_hw.bad_index == 0, "TEE: every CSR index in range");
    check(pmp_matches(g_hw.pmpaddr, g_hw.pmpcfg),
          VARIANT "TEE: PMP 0..3 memory, then periph R W, rest off");
    check(g_hw.pmpcfg[0] == 0x1F1B1F1Du && g_hw.pmpcfg[1] == WANT_CFG1,
          VARIANT "TEE: pmpcfg0 0x1f1b1f1d, pmpcfg1");
    check(memcmp(g_hw.spmpaddr, g_hw.pmpaddr, sizeof(g_hw.pmpaddr)) == 0,
          "TEE: spmpaddr 0..7 == pmpaddr 0..7");
    check(memcmp(g_hw.spmpcfg, g_hw.pmpcfg, sizeof(g_hw.pmpcfg)) == 0,
          "TEE: spmpcfg0/1 == pmpcfg0/1 (no L, no bit 6)");
    check(g_hw.smpuswitch == 0xFFu, "TEE: smpuswitch0 = 0xff");
    check(g_hw.switch_seq > g_hw.spmp_last_seq,
          "TEE: smpuswitch0 written after the sPMP entries");
    check(g_hw.fence_seq == g_hw.seq, "TEE: fence is the last access");

    first = g_hw;
    hw_reset(MCFG_INFO_TEE);
    spm_pmp_apply();
    check(memcmp(first.pmpaddr, g_hw.pmpaddr, sizeof(first.pmpaddr)) == 0 &&
          memcmp(first.spmpcfg, g_hw.spmpcfg, sizeof(first.spmpcfg)) == 0 &&
          g_hw.smpuswitch == first.smpuswitch,
          "TEE: mcfg_info bit 0 alone selects the sPMP path");
}

/* 1 when [base, base + size) overlaps no enabled NAPOT entry */
static int outside_all(const uint32_t *addr, const uint32_t *cfg,
                       uint32_t base, uint32_t size)
{
    for (uint32_t i = 0; i < HW_ENTRIES; i++) {
        uint32_t b;
        uint32_t n;

        if ((cfg_byte(cfg, i) & 0x18u) == 0) {
            continue;
        }
        napot_decode(addr[i], &b, &n);
        if (base < b + n && b < base + size) {
            return 0;
        }
    }
    return 1;
}

/* SEC_ENG and TZC_SEC bases of bsp/drivers/ipro7_soc/regs/ipro7.h */
static void test_hw_key_blocks_denied(void)
{
    static const uint32_t blocks[][2] = {
        { 0x30080000u, 0x1000u },   /* SEC_ENG */
        { 0x30006000u, 0x1000u },   /* TZC_SEC */
        { 0x30008000u, 0x1000u },   /* EF_CTRL */
        { 0x30000000u, 0x1000u },   /* GLB */
        { 0x30081000u, 0x1000u },   /* SF_CTRL */
        { 0x30083000u, 0x1000u },   /* EMAC */
        { 0x30085000u, 0x1000u },   /* SDH */
    };
    int ok = 1;

    hw_reset(MCFG_SILICON);
    spm_pmp_apply();
    for (size_t i = 0; i < sizeof(blocks) / sizeof(blocks[0]); i++) {
        ok = ok && outside_all(g_hw.pmpaddr, g_hw.pmpcfg, blocks[i][0],
                               blocks[i][1]) &&
             outside_all(g_hw.spmpaddr, g_hw.spmpcfg, blocks[i][0],
                         blocks[i][1]);
    }
    check(ok, VARIANT "SEC_ENG, TZC_SEC, EF_CTRL, GLB, SF_CTRL, EMAC, SDH "
              "outside every PMP and sPMP entry");
}

static void test_no_tee(void)
{
    hw_reset(MCFG_SILICON & ~(uint32_t)MCFG_INFO_TEE);
    spm_pmp_apply();
    check(pmp_matches(g_hw.pmpaddr, g_hw.pmpcfg) &&
          g_hw.pmpcfg[0] == 0x1F1B1F1Du && g_hw.pmpcfg[1] == WANT_CFG1,
          "no TEE: PMP table unchanged");
    check(g_hw.spmpaddr[0] == UNSET && g_hw.spmpcfg[0] == UNSET &&
          g_hw.spmpcfg[1] == UNSET && g_hw.smpuswitch == UNSET &&
          g_hw.spmp_last_seq == 0 && g_hw.switch_seq == 0,
          "no TEE: no sPMP or smpuswitch0 access");
    check(g_hw.fence_seq == g_hw.seq, "no TEE: fence is the last access");
}

static void test_spmp_cfg(void)
{
    check(spm_spmp_cfg(0x00) == 0x00, "spmp_cfg 0x00 -> 0x00");
    check(spm_spmp_cfg(0x1F) == 0x1F, "spmp_cfg A|XWR kept");
    check(spm_spmp_cfg(0x9B) == 0x1B, "spmp_cfg L (bit 7) stripped");
    check(spm_spmp_cfg(0x5D) == 0x1D, "spmp_cfg bit 6 (U/S) stripped");
    check(spm_spmp_cfg(0x20) == 0x00, "spmp_cfg bit 5 stripped");
    check(spm_spmp_cfg(0xFF) == 0x1F, "spmp_cfg 0xff -> 0x1f");
    check(spm_spmp_cfg(0x08) == 0x08 && spm_spmp_cfg(0x10) == 0x10,
          "spmp_cfg TOR and NA4 kept");
}

static void test_periph(void)
{
    const struct spm_periph *p = spm_periph_at(0);

    check(SPM_PERIPH_COUNT == WANT_ENTRIES - 4u && p != NULL &&
          p->base == PSA_NSPE_UART_BASE && p->size == PSA_NSPE_UART_SIZE &&
          p->irq == (uint32_t)UART0_IRQn && p->reset == NULL,
          "periph 0 = UART0 window and UART0_IRQn, no reset hook");
    check(spm_periph_at(SPM_PERIPH_COUNT) == NULL &&
          spm_periph_at(0xFFFFFFFFu) == NULL,
          "periph past the table: NULL");
#ifdef CONFIG_PSA_NSPE_DMA_USB
    p = spm_periph_at(1);
    check(p != NULL && p->base == 0x30082000u && p->size == 0x1000u &&
          p->irq == (uint32_t)DMA0_ALL_IRQn && p->reset != NULL,
          "DMA_USB: periph 1 = DMA0 window, DMA0_ALL_IRQn, reset hook");
    p = spm_periph_at(2);
    check(p != NULL && p->base == 0x30084000u && p->size == 0x1000u &&
          p->irq == (uint32_t)USB_IRQn && p->reset != NULL,
          "DMA_USB: periph 2 = USB window, USB_IRQn, reset hook");
    check(spm_periph_irq_ok(DMA0_ALL_IRQn) && spm_periph_irq_ok(USB_IRQn),
          "DMA_USB: DMA0_ALL_IRQn and USB_IRQn attachable");
#else
    check(spm_periph_irq_ok(DMA0_ALL_IRQn) == 0 &&
          spm_periph_irq_ok(USB_IRQn) == 0,
          "DMA0_ALL_IRQn and USB_IRQn refused");
#endif
    check(spm_periph_irq_ok(UART0_IRQn) == 1, "UART0_IRQn attachable");
    /* UART0_IRQn - 1 is USB_IRQn, a table id with DMA_USB */
    check((USB_IRQn == UART0_IRQn - 1 &&
           spm_periph_irq_ok(UART0_IRQn - 1u) == (SPM_PERIPH_COUNT > 1u)) &&
              spm_periph_irq_ok(UART0_IRQn + 1u) == 0,
          "neighbour ECLIC ids (the other UART) refused");
    check(spm_periph_irq_ok(0) == 0 && spm_periph_irq_ok(7) == 0 &&
          spm_periph_irq_ok(0xFFFFFFFFu) == 0,
          "ids 0, 7 (timer), 0xffffffff refused");
}

static void test_reg_grants(void)
{
#ifdef CONFIG_PSA_NSPE_DMA_USB
    static const uint32_t want_g[][4] = {
        { 0x30000228u, 0x00F11770u, 0, 0 },
        { 0x3000022Cu, 0x77777FFFu, 0, 0 },
        { 0x30000110u, 0xE0FFE0FFu, 0x1F001F00u, 0x0A000A00u },
        { 0x30000190u, 0x00000300u, 0, 0 },
    };
    int ok = SPM_REG_GRANT_COUNT == 4u;

    for (size_t i = 0; i < sizeof(want_g) / sizeof(want_g[0]); i++) {
        const struct spm_reg_grant *g = spm_reg_grant_find(want_g[i][0]);

        ok = ok && g != NULL && g->addr == want_g[i][0] &&
             g->wmask == want_g[i][1] && g->fmask == want_g[i][2] &&
             g->fval == want_g[i][3] && (g->wmask & g->fmask) == 0;
    }
    check(ok, "DMA_USB: grants usb_xcvr, usb_xcvr_config, GPIO 8/9 cfg "
              "(func_sel fixed analog), GPIO 8/9 OE with their masks");
    check(spm_reg_grant_find(0x30000229u) == NULL &&
              spm_reg_grant_find(0x30000224u) == NULL &&
              spm_reg_grant_find(0x3000010Cu) == NULL &&
              spm_reg_grant_find(0x30000114u) == NULL &&
              spm_reg_grant_find(0x30000000u) == NULL,
          "DMA_USB: unaligned and neighbour GLB words have no grant");
#else
    check(SPM_REG_GRANT_COUNT == 0u &&
              spm_reg_grant_find(0x30000228u) == NULL &&
              spm_reg_grant_find(0x30000110u) == NULL,
          "no register grant without CONFIG_PSA_NSPE_DMA_USB");
#endif
}

int main(void)
{
    test_regions_fit_grain();
    test_tee();
    test_no_tee();
    test_hw_key_blocks_denied();
    test_spmp_cfg();
    test_periph();
    test_reg_grants();
    return check_summary();
}
