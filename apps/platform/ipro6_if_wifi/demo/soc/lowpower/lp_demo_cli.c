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

/**
 * @file lp_demo_cli.c
 * @brief Shell commands for the IPRO6 Low Power demo (`lp ...`)
 */

#ifdef CONFIG_SHELL

#include "lp_demo.h"
#include "shell.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include "drv_hbn.h"
#include "drv_uart.h"
#include "drv_clock.h"
#include "hal_pm.h"
#include "hal_lp.h"
#include "lp_fw.h"       /* LP_FW_SIM_DTIM_CYCLES default */

/* Wait for UART TX to complete */
static void cli_uart_flush(void)
{
    while (UART_GetTxFifoCount(UART0_ID) > 0) {}
    while (UART_GetTxBusBusyStatus(UART0_ID)) {}
    for (volatile int i = 0; i < 10000; i++) {}
}

static void cmd_lp_help(void)
{
    printf("LP Demo Commands:\r\n");
    printf("  lp sleep <ms> [cnt] - PDS sleep via hal_lp (cnt DTIM cycles before wake)\r\n");
    printf("  lp auto on/off   - Enable/disable auto PDS in idle\r\n");
    printf("  lp status        - Show sleep statistics\r\n");
    printf("  lp reset         - Reset statistics\r\n");
    printf("  lp heap          - Show free heap\r\n");
    printf("  lp rtc           - Show RTC counter (run twice to verify counting)\r\n");
    printf("  lp raw <ms>      - Raw PDS test (bypasses hal_lp, board will reboot)\r\n");
    printf("  lp raw2 <ms> [fdpr] - Minimal PDS test (f=flash d=dll p=pll r=rf)\r\n");
    printf("  lp hbn           - Show HBN register state\r\n");
    printf("  lp hbn <0|1> <ms>- HBN retention test (0=RAM kept,1=RAM lost; cold-boots)\r\n");
    printf("  lp clk           - Show saved sys_clk/xtal and current HW clock state\r\n");
    printf("  lp fastboot <ms> - Fastboot wake (cold boot on wake, does NOT return)\r\n");
    printf("  lp wakectx       - Show last fastboot wake context from HBN RAM\r\n");
}

static void cmd_lp_wakectx(void)
{
    const volatile hal_hbnram_lp_wake_ctx_t *ctx = hal_lp_get_wake_context();
    if (ctx->magic != HAL_HBNRAM_LP_WAKE_CTX_MAGIC) {
        printf("No fastboot wake context (magic=0x%08lx, expected 0x%08lx)\r\n",
               (unsigned long)ctx->magic,
               (unsigned long)HAL_HBNRAM_LP_WAKE_CTX_MAGIC);
        return;
    }
    printf("Fastboot wake context:\r\n");
    printf("  wake_counter:   %lu\r\n", (unsigned long)ctx->wake_counter);
    printf("  wake_reason:    0x%lx%s%s\r\n",
           (unsigned long)ctx->wake_reason,
           (ctx->wake_reason & HAL_LP_WAKEUP_RTC) ? " RTC" : "",
           (ctx->wake_reason & HAL_LP_WAKEUP_GPIO) ? " GPIO" : "");
    printf("  wake_gpio_bits: 0x%08lx\r\n", (unsigned long)ctx->wake_gpio_bits);
    printf("  rtc_cnt:        0x%08lx_%08lx\r\n",
           (unsigned long)ctx->rtc_cnt_high, (unsigned long)ctx->rtc_cnt_low);
}

static void cmd_lp_fastboot(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: lp fastboot <ms>\r\n");
        printf("  Enter PDS, wake through cold-boot (_start) path.\r\n");
        printf("  After boot, run 'lp wakectx' to see wake_counter/reason.\r\n");
        return;
    }
    uint32_t ms = (uint32_t)atoi(argv[2]);
    if (ms < 10) {
        printf("Min 10 ms\r\n");
        return;
    }

    hal_lp_cfg_t cfg = {
        .rtc_wakeup_en  = 1,
        .rtc_sleep_us   = (uint64_t)ms * 1000ULL,
        .gpio_wakeup_en = 0,
        .gpio_cfg       = NULL,
    };

    printf("Fastboot: %lu ms. Board will cold-boot on wake.\r\n",
           (unsigned long)ms);
    cli_uart_flush();

    int ret = hal_lp_enter_fastboot(&cfg);

    /* Only returns on error */
    printf("ERROR: hal_lp_enter_fastboot returned %d\r\n", ret);
}

/* Read the RISC-V core cycle counter (M-mode mcycle, CSR 0xB00). It counts
 * core clock cycles, so measuring its delta over an independent real-time
 * window yields the true MCU frequency regardless of how the clock-mux
 * registers read back through this SoC's address aliases. */
static inline uint32_t lp_rd_mcycle(void)
{
    unsigned long v;   /* XLEN-wide; 32-bit on RV32 ilp32 */
    __asm__ volatile ("csrr %0, mcycle" : "=r"(v));
    return (uint32_t)v;
}

static void cmd_lp_clk(void)
{
    /* Saved values from last PDS enter */
    uint8_t sys_clk = hal_lp_param->sys_clk_type;
    uint8_t xtal = hal_lp_param->xtal_type;

    /* Live clock-mux state. On this hybrid SoC the HBN/PDS register file reads
     * back only through the 0x3008_xxxx alias (the driver's 0x2000_xxxx base
     * reads dead), so read the validated aliases directly rather than the
     * driver getters, which would mis-report XCLK. */
    uint32_t hbn_glb  = *(volatile uint32_t *)0x3008f030UL; /* HBN_GLB: root_clk_sel[1:0] */
    uint32_t pds_cfg1 = *(volatile uint32_t *)0x3008e114UL; /* PDS_CPU_CORE_CFG1: pll_sel[5:4] */
    uint32_t root_pll = (hbn_glb >> 1) & 0x1;               /* 0=XCLK, 1=PLL */
    uint32_t pll_sel  = (pds_cfg1 >> 4) & 0x3;

    const char *sys_clk_name[] = {
        "RC32M", "XTAL", "DLL25P6M", "PLL160M", "PLL192M", "DLL128M"
    };
    const char *pll_sel_name[] = {
        "AUPLL/2", "AUPLL/1", "WIFIPLL_240M", "WIFIPLL_320M"
    };

    /* Measure the real MCU core frequency. The mtimer is sourced from MCU_CLK,
     * so an mcycle-vs-mtime ratio cancels the core frequency and only reflects
     * the mtimer divider -- useless here. Use the always-on 32768 Hz RTC
     * instead: it is independent of the core clock, so counting core cycles
     * (mcycle) over a fixed RTC window yields the true MCU frequency. The
     * WIFIPLL analog config is write-only on this part, so this is the
     * definitive proof of whether WIFIPLL actually feeds the core. */
    uint32_t rl, rh;
    HBN_Get_RTC_Timer_Val(&rl, &rh);
    uint64_t r0 = ((uint64_t)rh << 32) | rl;
    uint32_t c0 = lp_rd_mcycle();
    uint64_t r1;
    uint32_t guard = 0;
    do {                                /* spin ~100 ms: 0.1 s * 32768 = 3277 cnt */
        HBN_Get_RTC_Timer_Val(&rl, &rh);
        r1 = ((uint64_t)rh << 32) | rl;
    } while ((r1 - r0) < 3277ULL && ++guard < 50000000UL);
    uint32_t c1 = lp_rd_mcycle();
    uint32_t dcyc = c1 - c0;
    uint64_t dus  = HAL_LP_RTC_CNT_TO_US(r1 - r0);  /* real us from 32768 Hz RTC */
    uint32_t hz   = dus ? (uint32_t)((uint64_t)dcyc * 1000000ULL / dus) : 0;

    printf("Saved (last hal_lp_enter): sys_clk_type=%u (%s) xtal_type=%u\r\n",
           sys_clk, sys_clk < 6 ? sys_clk_name[sys_clk] : "?", xtal);
    printf("Live clock mux:\r\n");
    printf("  MCU root [0x3008f030 b1]   = %lu (%s)\r\n",
           (unsigned long)root_pll, root_pll ? "PLL" : "XCLK");
    printf("  pll_sel  [0x3008e114 b5:4] = %lu (%s)\r\n",
           (unsigned long)pll_sel, pll_sel_name[pll_sel]);
    printf("Measured MCU core clock (RTC-timed): %lu Hz (%lu.%02lu MHz)  [dcyc=%lu / %lu us]\r\n",
           (unsigned long)hz,
           (unsigned long)(hz / 1000000UL),
           (unsigned long)((hz % 1000000UL) / 10000UL),
           (unsigned long)dcyc, (unsigned long)dus);
    if (dcyc == 0) {
        printf("  NOTE: mcycle did not advance (counter inhibited?)\r\n");
    }

    /* Register-derived cross-check. XCLK is the MTimer source (xclk/toggle);
     * MTimer should read ~1 MHz when the toggle divider matches XCLK. */
    uint8_t xt = GLB_XTAL_NONE;
    HBN_Get_Xtal_Type(&xt);
    printf("XCLK=%lu Hz  MTimer=%lu Hz  MCU(SDK)=%lu Hz  xtalType=%u\r\n",
           (unsigned long)Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK),
           (unsigned long)CPU_Get_MTimer_Clock(),
           (unsigned long)Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK),
           (unsigned)xt);
}

static void cmd_lp_rtc(void)
{
    uint32_t rtc_low, rtc_high;
    HBN_Get_RTC_Timer_Val(&rtc_low, &rtc_high);
    uint64_t rtc_cnt = ((uint64_t)rtc_high << 32) | rtc_low;
    uint64_t rtc_us = HAL_LP_RTC_CNT_TO_US(rtc_cnt);
    printf("RTC: cnt=0x%lx_%08lx (%llu us = %llu ms)\r\n",
           (unsigned long)rtc_high, (unsigned long)rtc_low,
           (unsigned long long)rtc_us, (unsigned long long)(rtc_us / 1000));
}

/* HBN (Hibernate) retention test. HBN is the deepest mode and does NOT
 * resume-in-place — it cold-boots through BROM->boot2->FW. We stamp the HBN-RAM
 * scratch, hibernate, and the boot-time probe in lp_demo_app_init() reads it back:
 *   level 0 (HBN_PWRDN_HBN_CORE=0): AON HBN core kept -> HBN RAM RETAINED.
 *   level 1 (HBN_PWRDN_HBN_CORE=1): AON HBN core gated -> HBN RAM LOST.
 */
static void cmd_lp_hbn_test(int level, uint32_t ms)
{
    /* Stamp the HBN-RAM scratch at 0x20090E00 (free gap: LP_FW 0x000-0x400,
     * cpu_regs 0x400-0x600, IOT2LP 0xF00-0x1000 are used). */
    volatile uint32_t *hbnram = (volatile uint32_t *)0x20090E00u;
    uint32_t seq = (hbnram[0] == 0x48424E00u) ? hbnram[2] + 1u : 1u;
    hbnram[0] = 0x48424E00u;            /* 'HBN\0' magic */
    hbnram[1] = (uint32_t)level;
    hbnram[2] = seq;

    /* HBN cold-boots BROM->boot2->FW on wake; pm_hbn_mode_enter() deliberately does
     * not arm the PDS fast-wake path and clears HBN_RSV0, so no RSV setup is needed
     * here (the HBN-RAM stamp above is what we read back after the cold boot). */
    uint32_t rtc_ticks = (uint32_t)HAL_LP_US_TO_RTC_CNT((uint64_t)ms * 1000ULL);
    printf("HBN%d: stamped 0x20090E00 magic=0x48424E00 level=%d seq=%lu\r\n",
           level, level, (unsigned long)seq);
    printf("Entering HBN%d (%s HBN-RAM retention); cold-boot in %lu ms...\r\n",
           level, level ? "WITHOUT" : "WITH", (unsigned long)ms);
    cli_uart_flush();

    pm_hbn_mode_enter((enum pm_hbn_sleep_level)level, rtc_ticks);

    /* HBN never returns — the chip resets on wake. */
    printf("ERROR: HBN did not enter!\r\n");
}

static void cmd_lp_hbn(int argc, char **argv)
{
    /* 'lp hbn <0|1> <ms>' runs the retention test; 'lp hbn' shows register state. */
    if (argc >= 4) {
        int level = atoi(argv[2]);
        uint32_t ms = (uint32_t)atoi(argv[3]);
        if ((level != 0 && level != 1) || ms < 50) {
            printf("Usage: lp hbn <0|1> <ms>\r\n");
            printf("  level 0 = HBN RAM retained, 1 = HBN RAM lost; ms >= 50\r\n");
            return;
        }
        cmd_lp_hbn_test(level, ms);
        return;
    }

    uint32_t rsv0 = *(volatile uint32_t *)(HBN_BASE + HBN_RSV0_OFFSET);
    uint32_t rsv1 = *(volatile uint32_t *)(HBN_BASE + HBN_RSV1_OFFSET);
    uint32_t ctl  = *(volatile uint32_t *)(HBN_BASE + HBN_CTL_OFFSET);
    uint32_t time_l = *(volatile uint32_t *)(HBN_BASE + HBN_TIME_L_OFFSET);
    uint32_t time_h = *(volatile uint32_t *)(HBN_BASE + HBN_TIME_H_OFFSET);
    uint32_t rsv2 = *(volatile uint32_t *)(HBN_BASE + HBN_RSV2_OFFSET);
    uint32_t rsv3 = *(volatile uint32_t *)(HBN_BASE + HBN_RSV3_OFFSET);
    printf("HBN state:\r\n");
    printf("  RSV0 (flag):     0x%08lx", (unsigned long)rsv0);
    if (rsv0 == 0x4e424845)
        printf(" (LP flag SET)\r\n");
    else if (rsv0 == 0x4C500001)
        printf(" (LP entry OK)\r\n");
    else if (rsv0 == 0x4C50DEAD)
        printf(" (LP EXCEPTION!)\r\n");
    else
        printf(" (normal boot)\r\n");
    printf("  RSV1 (callback): 0x%08lx\r\n", (unsigned long)rsv1);
    printf("  RSV2 (stage):    0x%08lx", (unsigned long)rsv2);
    if ((rsv2 & 0xFFFF0000) == 0xDEAD0000)
        printf(" (EXCEPTION mcause=%lu)\r\n", (unsigned long)(rsv2 & 0xFFFF));
    else if (rsv2 >= 1 && rsv2 <= 30)
        printf(" (LP stage %lu reached)\r\n", (unsigned long)rsv2);
    else
        printf("\r\n");
    printf("  RSV3 (mepc):     0x%08lx\r\n", (unsigned long)rsv3);
    printf("  CTL:             0x%08lx\r\n", (unsigned long)ctl);
    printf("  TIME_L/H:        0x%08lx / 0x%08lx\r\n",
           (unsigned long)time_l, (unsigned long)time_h);
}

static void cmd_lp_raw(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: lp raw <ms>\r\n");
        printf("  This bypasses hal_lp and does a raw PDS Level 31.\r\n");
        printf("  Board should REBOOT after <ms> (no HBN RAM restore).\r\n");
        return;
    }
    uint32_t ms = (uint32_t)atoi(argv[2]);
    if (ms < 10) {
        printf("Min 10 ms\r\n");
        return;
    }

    /* Clear HBN RSV0 so bootrom does cold boot (not LP wakeup) */
    *(volatile uint32_t *)(HBN_BASE + HBN_RSV0_OFFSET) = 0;
    *(volatile uint32_t *)(HBN_BASE + HBN_RSV1_OFFSET) = 0xFFFFFFFF;

    uint64_t sleep_us = (uint64_t)ms * 1000ULL;
    uint32_t rtc_ticks = (uint32_t)HAL_LP_US_TO_RTC_CNT(sleep_us);

    printf("Raw PDS Level 31: %lu ms (%lu rtc ticks)\r\n",
           (unsigned long)ms, (unsigned long)rtc_ticks);
    printf("Board should reboot in %lu ms...\r\n", (unsigned long)ms);
    cli_uart_flush();

    /* Direct call — pm_pds_mode_enter handles RTC setup + PDS entry */
    pm_pds_mode_enter(PM_PDS_LEVEL_31, rtc_ticks);

    /* Should not reach here for PDS31 (CPU reset, bootrom runs) */
    printf("ERROR: PDS did not reset CPU!\r\n");
}

/*
 * Minimal PDS test — calls pm_pds_enable directly with configurable options.
 * Usage: lp raw2 <ms> [flags]
 *   flags: f=flash_pd, d=dll_off, p=pll_off, r=rf_off
 *   default (no flags): all OFF — most minimal PDS entry
 */
static void cmd_lp_raw2(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: lp raw2 <ms> [fdpr]\r\n");
        printf("  Minimal PDS31 via pm_pds_enable directly.\r\n");
        printf("  Flags: f=flash_pd d=dll_off p=pll_off r=rf_off\r\n");
        printf("  Default: none (minimal, safest)\r\n");
        return;
    }
    uint32_t ms = (uint32_t)atoi(argv[2]);
    if (ms < 10) { printf("Min 10 ms\r\n"); return; }

    /* Parse optional flags */
    int flash_pd = 0, dll_off = 0, pll_off = 0, rf_off = 0;
    if (argc >= 4) {
        const char *flags = argv[3];
        for (int i = 0; flags[i]; i++) {
            switch (flags[i]) {
                case 'f': flash_pd = 1; break;
                case 'd': dll_off = 1; break;
                case 'p': pll_off = 1; break;
                case 'r': rf_off = 1; break;
            }
        }
    }

    /* Clear HBN RSV0 so bootrom does cold boot */
    *(volatile uint32_t *)(HBN_BASE + HBN_RSV0_OFFSET) = 0;
    *(volatile uint32_t *)(HBN_BASE + HBN_RSV1_OFFSET) = 0xFFFFFFFF;

    uint64_t sleep_us = (uint64_t)ms * 1000ULL;
    uint32_t rtc_ticks = (uint32_t)HAL_LP_US_TO_RTC_CNT(sleep_us);

    /* Setup RTC wakeup */
    uint32_t rtc_low, rtc_high;
    HBN_Get_RTC_Timer_Val(&rtc_low, &rtc_high);
    uint64_t rtc_now = ((uint64_t)rtc_high << 32) | rtc_low;
    uint64_t rtc_target = rtc_now + rtc_ticks;
    HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_32T,
                      rtc_target & 0xFFFFFFFF,
                      (rtc_target >> 32) & 0x1fff, 1);

    PM_PDS_CFG_Type cfg = {
        .pdsLevel = PM_PDS_LEVEL_31,
        .turnOffRF = rf_off,
        .powerDownFlash = flash_pd,
        .ocramRetetion = 1,
        .turnoffPLL = pll_off,
        .turnoffDLL = dll_off,
        .pdsLdoEn = 0,
        .flashContRead = 0,
        .sleepTime = rtc_ticks,
        .flashCfg = NULL,
        .ldoLevel = 0xA,
        .preCbFun = NULL,
        .postCbFun = NULL,
    };

    /* Get flash cfg if flash_pd enabled */
    if (flash_pd) {
        uint32_t flash_cfg_len;
        extern void flash_get_cfg(uint8_t **cfg_addr, uint32_t *len);
        flash_get_cfg((uint8_t **)&cfg.flashCfg, &flash_cfg_len);
    }

    printf("Raw2 PDS31: %lu ms, flags=%s%s%s%s%s\r\n",
           (unsigned long)ms,
           flash_pd ? "flash_pd " : "",
           dll_off ? "dll_off " : "",
           pll_off ? "pll_off " : "",
           rf_off ? "rf_off " : "",
           (!flash_pd && !dll_off && !pll_off && !rf_off) ? "(minimal)" : "");
    cli_uart_flush();

    pm_pds_enable((uint32_t *)&cfg);

    /* If we get here, PDS woke up without CPU reset */
    printf("Woke up! (no CPU reset)\r\n");
}

/* Raw 32-bit register peek: lp md <hexaddr> [count] */
static void cmd_lp_md(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: lp md <hexaddr> [count]\r\n");
        return;
    }
    uint32_t addr = (uint32_t)strtoul(argv[2], NULL, 16);
    int n = (argc >= 4) ? atoi(argv[3]) : 1;
    if (n < 1) n = 1;
    if (n > 16) n = 16;
    for (int i = 0; i < n; i++) {
        uint32_t a = addr + (uint32_t)i * 4;
        uint32_t v = *(volatile uint32_t *)(uintptr_t)a;
        printf("  [0x%08lx] = 0x%08lx\r\n", (unsigned long)a, (unsigned long)v);
    }
    /* Decode em_sel when reading GLB_SRAM_CFG3 (0x3000060C) */
    if (addr == 0x3000060CUL) {
        uint32_t em = (*(volatile uint32_t *)(uintptr_t)0x3000060CUL) & 0xF;
        const char *nm = (em == 0) ? "WRAM160KB/EM0KB" :
                         (em == 1) ? "WRAM128KB/EM32KB" :
                         (em == 2) ? "WRAM96KB/EM64KB" : "(reset default / other)";
        printf("  em_sel[3:0] = 0x%lx  %s\r\n", (unsigned long)em, nm);
    }
}

static void cmd_lp_mw(int argc, char **argv)
{
    if (argc < 4) {
        printf("Usage: lp mw <hexaddr> <hexval>\r\n");
        return;
    }
    uint32_t addr = (uint32_t)strtoul(argv[2], NULL, 16);
    uint32_t val  = (uint32_t)strtoul(argv[3], NULL, 16);
    *(volatile uint32_t *)(uintptr_t)addr = val;
    printf("  [0x%08lx] <= 0x%08lx (now 0x%08lx)\r\n", (unsigned long)addr,
           (unsigned long)val, (unsigned long)*(volatile uint32_t *)(uintptr_t)addr);
}

/* Mux IPRO6 GPIO10-13 to S_CCI (func 15) so the IPRO7 probe's CCI master can
 * reach the DUT's CCI slave. Per the bench guide: GPIO_CFG[N] = 0x300008C4+4*N,
 * S_CCI value 0x00400F03. (GPIO10=CLK, 11=CS, 12=DI, 13=DO.) */
static void cmd_lp_cci(int argc, char **argv)
{
    (void)argc; (void)argv;
    static const uint32_t cfg[4] = {0x300008EC, 0x300008F0, 0x300008F4, 0x300008F8};
    for (int i = 0; i < 4; i++)
        *(volatile uint32_t *)(uintptr_t)cfg[i] = 0x00400F03;
    printf("  GPIO10-13 -> S_CCI (func 15); CCI master can now read DUT regs\r\n");
}

/* Jump into the staged lp_fw Stage-1 image (reserved OCRAM slot 0x11048000) to
 * verify it executes from retained RAM. Phase-1 lp_fw is stamp-only (no console
 * on the clock-gated wake path): it writes proof-of-execution into HBN RAM and
 * returns the wake decision. Read it back afterwards with `lp md 0x20090f00`
 * (expect [0]=0x1 wakeup_reason, [4] low16=0x5A5A aid). In this live FreeRTOS
 * context the return (with interrupts masked) trips a FreeRTOS assert -- that is
 * expected, reset the board afterwards. The real wake path returns cleanly to
 * the HBN stub. */
static void cmd_lp_fwjump(void)
{
    printf("Jumping to lp_fw @0x11048000 (stamps 0x20090f00; read back with 'lp md')\r\n");
    /* let the console drain before handing the core to lp_fw */
    for (volatile int i = 0; i < 2000000; i++) {
    }
    /* lp_fw_start hijacks sp/gp for its own context. In this live FreeRTOS
     * test it runs with interrupts on, so a tick IRQ would re-enter the full
     * firmware's ISR under lp_fw's gp -> garbage globals. Mask interrupts
     * globally first; the real wake path already enters lp_fw interrupts-off. */
    __asm__ volatile("csrci mstatus, 0x8" ::: "memory"); /* clear MIE */
    ((void (*)(void))(uintptr_t)0x11048000u)();
}

static int cmd_lp(int argc, char **argv)
{
    if (argc < 2) {
        cmd_lp_help();
        return 0;
    }

    if (strcmp(argv[1], "sleep") == 0) {
        if (argc < 3) {
            printf("Usage: lp sleep <ms> [cnt]\r\n");
            printf("  cnt = DTIM re-sleep cycles before wake (default %d)\r\n",
                   LP_FW_SIM_DTIM_CYCLES);
            return 0;
        }
        uint32_t ms = (uint32_t)atoi(argv[2]);
        uint32_t cnt = (argc >= 4) ? (uint32_t)atoi(argv[3]) : LP_FW_SIM_DTIM_CYCLES;
        if (cnt == 0) cnt = 1;
        lp_demo_sleep(ms, cnt);
    } else if (strcmp(argv[1], "auto") == 0) {
        if (argc < 3) {
            printf("Auto PDS: %s\r\n", lp_demo_auto_is_enabled() ? "on" : "off");
            return 0;
        }
        if (strcmp(argv[2], "on") == 0) {
            lp_demo_auto_enable(true);
        } else if (strcmp(argv[2], "off") == 0) {
            lp_demo_auto_enable(false);
        } else {
            printf("Usage: lp auto on|off\r\n");
        }
    } else if (strcmp(argv[1], "status") == 0) {
        uint32_t total, count, last;
        int32_t reason;
        lp_demo_get_stats(&total, &count, &last, &reason);
        printf("LP Status:\r\n");
        printf("  Auto PDS:     %s\r\n", lp_demo_auto_is_enabled() ? "on" : "off");
        printf("  Sleep count:  %lu\r\n", (unsigned long)count);
        printf("  Total sleep:  %lu ms\r\n", (unsigned long)total);
        printf("  Last sleep:   %lu ms\r\n", (unsigned long)last);
        printf("  Last wakeup:  0x%lx\r\n", (unsigned long)reason);
        printf("  Free heap:    %u bytes\r\n", (unsigned)xPortGetFreeHeapSize());
    } else if (strcmp(argv[1], "reset") == 0) {
        lp_demo_reset_stats();
        printf("Stats reset\r\n");
    } else if (strcmp(argv[1], "heap") == 0) {
        printf("Free heap: %u bytes\r\n", (unsigned)xPortGetFreeHeapSize());
    } else if (strcmp(argv[1], "rtc") == 0) {
        cmd_lp_rtc();
    } else if (strcmp(argv[1], "hbn") == 0) {
        cmd_lp_hbn(argc, argv);
    } else if (strcmp(argv[1], "clk") == 0) {
        cmd_lp_clk();
    } else if (strcmp(argv[1], "raw") == 0) {
        cmd_lp_raw(argc, argv);
    } else if (strcmp(argv[1], "raw2") == 0) {
        cmd_lp_raw2(argc, argv);
    } else if (strcmp(argv[1], "md") == 0) {
        cmd_lp_md(argc, argv);
    } else if (strcmp(argv[1], "mw") == 0) {
        cmd_lp_mw(argc, argv);
    } else if (strcmp(argv[1], "cci") == 0) {
        cmd_lp_cci(argc, argv);
    } else if (strcmp(argv[1], "fastboot") == 0) {
        cmd_lp_fastboot(argc, argv);
    } else if (strcmp(argv[1], "wakectx") == 0) {
        cmd_lp_wakectx();
    } else if (strcmp(argv[1], "fwjump") == 0) {
        cmd_lp_fwjump();
    } else {
        printf("Unknown: %s\r\n", argv[1]);
        cmd_lp_help();
    }

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_lp, lp, LP demo commands);

#endif /* CONFIG_SHELL */
