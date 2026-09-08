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
 * @file lp_test_cli.c
 * @brief Shell commands for LP test
 */

#ifdef CONFIG_SHELL

#include "lp_test.h"
#include "ipro_log.h"
#include "shell.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include "drv_hbn.h"
#include "drv_uart.h"
#include "hal_pm.h"
#include "hal_lp.h"

#define LOG_TAG "LP_CLI"

/* Wait for UART TX to complete */
static void cli_uart_flush(void)
{
    while (UART_GetTxFifoCount(UART0_ID) > 0) {}
    while (UART_GetTxBusBusyStatus(UART0_ID)) {}
    for (volatile int i = 0; i < 10000; i++) {}
}

static void cmd_lp_help(void)
{
    printf("LP Test Commands:\r\n");
    printf("  lp sleep <ms>    - PDS sleep via hal_lp framework\r\n");
    printf("  lp auto on/off   - Enable/disable auto PDS in idle\r\n");
    printf("  lp status        - Show sleep statistics\r\n");
    printf("  lp reset         - Reset statistics\r\n");
    printf("  lp heap          - Show free heap\r\n");
    printf("  lp rtc           - Show RTC counter (run twice to verify counting)\r\n");
    printf("  lp raw <ms>      - Raw PDS test (bypasses hal_lp, board will reboot)\r\n");
    printf("  lp raw2 <ms> [fdpr] - Minimal PDS test (f=flash d=dll p=pll r=rf)\r\n");
    printf("  lp hbn           - Show HBN register state\r\n");
    printf("  lp clk           - Show saved sys_clk/xtal and current HW clock state\r\n");
    printf("  lp fastboot <ms> - Fastboot wake (cold boot on wake, does NOT return)\r\n");
    printf("  lp wakectx       - Show last fastboot wake context from HBN RAM\r\n");
    printf("  lp mem [ms]      - What OCRAM survives a PDS sleep, above/below 64 KB\r\n");
    printf("  lp misc on/off   - Keep the MISC power domain alive through sleep\r\n");
}

/*
 * lp mem [ms] — what OCRAM actually keeps across a PDS sleep.
 *
 * The figure this SDK is designed around is that PDS31 retains the first 64 KB
 * of OCRAM and nothing above it, and the app layout follows from that: the
 * linker's ocram_noret split, the BLE controller's non-retained heap, the
 * diagnostic magic parked at 0x1101fff0 just under the boundary. None of it
 * had ever been checked against the part.
 *
 * Measured 2026-08-18 on a no-PSRAM QFN32: everything is retained. A 190 KB
 * buffer spanning the boundary came back with 2 words lost out of 43008 above
 * it, over three sleeps and several boots. Four candidate retention controls
 * were tried and none of them moved the result:
 *
 *   PDS_RAM1 RET1 banks 4..15 (0x0000 -> 0xfff0)   no effect
 *   PDS_RAM1 RET1 banks 0..3  (0x0000 -> 0x000f)   hangs — RET1N is active low,
 *                                                  this drops the stack
 *   PDS_RAM1 RET2             (0xffff -> 0x000f)   no effect
 *   PDS_RAM2 PGEN             (0x0000 -> 0xfff0)   no effect
 *   PDS_RAM3 OCRAM_RET2       (0x0000 -> 0x0000f)  no effect
 *
 * Each write took and survived the sleep, so they are not being undone.
 * PM_PDS_CFG_Type.ocramRetetion has no reader anywhere in the tree, and
 * PDS_RAM_Config() has no caller and writes only PDS_RAM1. PDS_RAM3 is not
 * defined in the ipro7 register header at all; the offset here comes from the
 * ipro6 one, and the single comment in hal_pm.c describing it gives a reset
 * value of 0xfffff where the part reads 0x00000.
 *
 * So this reports rather than fixes. Run it after any change to the PDS
 * configuration to see whether the retention boundary has moved.
 */
#define MEM_OCRAM_BASE      0x11010000u
#define MEM_RETAIN_BOUNDARY (MEM_OCRAM_BASE + (64u * 1024u))
#define MEM_PDS_RAM1        (PDS_BASE + 0x20u)
#define MEM_PDS_RAM2        (PDS_BASE + 0x24u)
#define MEM_PDS_RAM3        (PDS_BASE + 0x48u)

static uint32_t mem_probe_word(uint32_t idx)
{
    return 0x5A5A0000u ^ (idx * 2246822519u);
}

/*
 * lp misc on|off — whether the MISC digital domain stays powered through PDS31.
 *
 * pdsCfgLevel31 defaults to powering it down (MiscPwrOff = 1). Only one place
 * in the tree turns that off: ipro_ble_lowpower_test's lp_test_power_init().
 *
 * That single call is what separates the two apps' retention results. With
 * MISC kept alive the whole 240 KB of OCRAM comes back from a sleep; with it
 * powered down, everything above 64 KB is dropped as the SoC team's figure
 * says it should be. Bank-level settings in PDS_RAM1/RAM2/RAM3 make no
 * difference either way, which is what you would expect if the domain
 * underneath them is simply never switched off.
 *
 * Pair it with 'lp mem' to see the boundary move within one boot:
 *   lp misc off ; lp mem 1000     -> above 64 KB lost
 *   lp misc on  ; lp mem 1000     -> above 64 KB retained
 *
 * Keeping it alive also costs leakage on its own, so 'on' is a diagnostic
 * rather than a setting anyone should ship.
 */
static void cmd_lp_misc(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: lp misc <on|off>\r\n");
        printf("  on  = MISC domain stays powered through PDS (retains all OCRAM)\r\n");
        printf("  off = MISC powered down (default; drops OCRAM above 64 KB)\r\n");
        return;
    }

    if (strcmp(argv[2], "on") == 0) {
        pm_pds_set_misc_keep(true);
        printf("MISC domain: kept alive through sleep\r\n");
    } else if (strcmp(argv[2], "off") == 0) {
        pm_pds_set_misc_keep(false);
        printf("MISC domain: powered down through sleep (default)\r\n");
    } else {
        printf("Usage: lp misc <on|off>\r\n");
    }
}

static void cmd_lp_mem(int argc, char **argv)
{
    uint32_t ms = (argc >= 3) ? (uint32_t)atoi(argv[2]) : 100u;
    uint32_t want, words, i, base, end;
    uint32_t bad_lo = 0, tot_lo = 0, bad_hi = 0, tot_hi = 0, highest_ok = 0;
    uint32_t *buf;

    if (ms < 10u) {
        printf("Min 10 ms\r\n");
        return;
    }

    printf("PDS RAM cfg: RAM1=%08lx (ret1=%04lx ret2=%04lx) RAM2=%08lx "
           "RAM3=%08lx\r\n",
           (unsigned long)*(volatile uint32_t *)MEM_PDS_RAM1,
           (unsigned long)(*(volatile uint32_t *)MEM_PDS_RAM1 & 0xFFFFu),
           (unsigned long)((*(volatile uint32_t *)MEM_PDS_RAM1 >> 16) & 0xFFFFu),
           (unsigned long)*(volatile uint32_t *)MEM_PDS_RAM2,
           (unsigned long)*(volatile uint32_t *)MEM_PDS_RAM3);

    want = (uint32_t)xPortGetFreeHeapSize();
    want = (want > 8192u) ? ((want - 8192u) & ~3u) : 0u;

    buf = (uint32_t *)pvPortMalloc(want);
    if (buf == NULL) {
        printf("cannot allocate %lu bytes\r\n", (unsigned long)want);
        return;
    }

    base = (uint32_t)(uintptr_t)buf;
    end = base + want;
    words = want / 4u;

    printf("probe %08lx..%08lx (%lu KB), boundary %08lx\r\n",
           (unsigned long)base, (unsigned long)end,
           (unsigned long)(want / 1024u), (unsigned long)MEM_RETAIN_BOUNDARY);

    if (end <= MEM_RETAIN_BOUNDARY) {
        printf("buffer does not reach past 64 KB — nothing to say about the "
               "region above it\r\n");
        vPortFree(buf);
        return;
    }

    for (i = 0; i < words; i++) {
        buf[i] = mem_probe_word(i);
    }

    lp_test_sleep(ms);

    for (i = 0; i < words; i++) {
        uint32_t addr = base + (i * 4u);
        int ok = (buf[i] == mem_probe_word(i));

        if (addr < MEM_RETAIN_BOUNDARY) {
            tot_lo++;
            if (!ok) bad_lo++;
        } else {
            tot_hi++;
            if (!ok) bad_hi++;
        }
        if (ok) highest_ok = addr;
    }

    printf("below 64K: %lu/%lu lost | above 64K: %lu/%lu lost | "
           "highest intact %08lx\r\n",
           (unsigned long)bad_lo, (unsigned long)tot_lo,
           (unsigned long)bad_hi, (unsigned long)tot_hi,
           (unsigned long)highest_ok);
    printf("expect 0 below and most of the words above lost; anything kept "
           "above the boundary is retention nobody asked for\r\n");

    /* Safe only while everything is retained — heap_4 keeps its free-list
     * nodes inside the heap, so freeing a buffer whose tail was power-gated
     * would walk destroyed pointers. The result above is printed first, so a
     * fault here is itself the answer. */
    vPortFree(buf);
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

static void cmd_lp_clk(void)
{
    /* Saved values from last PDS enter */
    uint8_t sys_clk = hal_lp_param->sys_clk_type;
    uint8_t xtal = hal_lp_param->xtal_type;

    /* Live HW state */
    uint32_t cfg0 = *(volatile uint32_t *)0x30000000;  /* GLB_CLK_CFG0 */
    uint32_t root = (cfg0 >> 6) & 0x3;
    uint32_t pll_sel = (cfg0 >> 4) & 0x3;
    uint32_t pll_en = cfg0 & 0x1;

    const char *sys_clk_name[] = {
        "RC32M", "XTAL", "DLL25P6M", "PLL160M", "PLL192M", "DLL128M"
    };
    const char *root_name[] = { "RC32M", "XTAL", "DLL/PLL", "?" };

    printf("Saved (from last hal_lp_enter):\r\n");
    printf("  sys_clk_type: %u (%s)\r\n", sys_clk,
           sys_clk < 6 ? sys_clk_name[sys_clk] : "?");
    printf("  xtal_type:    %u\r\n", xtal);
    printf("Live HW state (GLB_CLK_CFG0=0x%08lx):\r\n", (unsigned long)cfg0);
    printf("  root clock:   %lu (%s)\r\n", (unsigned long)root, root_name[root]);
    printf("  pll_sel:      %lu\r\n", (unsigned long)pll_sel);
    printf("  pll_en:       %lu\r\n", (unsigned long)pll_en);
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

static void cmd_lp_hbn(void)
{
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

static int cmd_lp(int argc, char **argv)
{
    if (argc < 2) {
        cmd_lp_help();
        return 0;
    }

    if (strcmp(argv[1], "sleep") == 0) {
        if (argc < 3) {
            printf("Usage: lp sleep <ms>\r\n");
            return 0;
        }
        uint32_t ms = (uint32_t)atoi(argv[2]);
        lp_test_sleep(ms);
    } else if (strcmp(argv[1], "auto") == 0) {
        if (argc < 3) {
            printf("Auto PDS: %s\r\n", lp_test_auto_is_enabled() ? "on" : "off");
            return 0;
        }
        if (strcmp(argv[2], "on") == 0) {
            lp_test_auto_enable(true);
        } else if (strcmp(argv[2], "off") == 0) {
            lp_test_auto_enable(false);
        } else {
            printf("Usage: lp auto on|off\r\n");
        }
    } else if (strcmp(argv[1], "status") == 0) {
        uint32_t total, count, last;
        int32_t reason;
        lp_test_get_stats(&total, &count, &last, &reason);
        printf("LP Status:\r\n");
        printf("  Auto PDS:     %s\r\n", lp_test_auto_is_enabled() ? "on" : "off");
        printf("  Sleep count:  %lu\r\n", (unsigned long)count);
        printf("  Total sleep:  %lu ms\r\n", (unsigned long)total);
        printf("  Last sleep:   %lu ms\r\n", (unsigned long)last);
        printf("  Last wakeup:  0x%lx\r\n", (unsigned long)reason);
        printf("  Free heap:    %u bytes\r\n", (unsigned)xPortGetFreeHeapSize());
    } else if (strcmp(argv[1], "reset") == 0) {
        lp_test_reset_stats();
        printf("Stats reset\r\n");
    } else if (strcmp(argv[1], "heap") == 0) {
        printf("Free heap: %u bytes\r\n", (unsigned)xPortGetFreeHeapSize());
    } else if (strcmp(argv[1], "rtc") == 0) {
        cmd_lp_rtc();
    } else if (strcmp(argv[1], "hbn") == 0) {
        cmd_lp_hbn();
    } else if (strcmp(argv[1], "clk") == 0) {
        cmd_lp_clk();
    } else if (strcmp(argv[1], "raw") == 0) {
        cmd_lp_raw(argc, argv);
    } else if (strcmp(argv[1], "raw2") == 0) {
        cmd_lp_raw2(argc, argv);
    } else if (strcmp(argv[1], "fastboot") == 0) {
        cmd_lp_fastboot(argc, argv);
    } else if (strcmp(argv[1], "wakectx") == 0) {
        cmd_lp_wakectx();
    } else if (strcmp(argv[1], "mem") == 0) {
        cmd_lp_mem(argc, argv);
    } else if (strcmp(argv[1], "misc") == 0) {
        cmd_lp_misc(argc, argv);
    } else {
        printf("Unknown: %s\r\n", argv[1]);
        cmd_lp_help();
    }

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_lp, lp, LP test commands);

#endif /* CONFIG_SHELL */
