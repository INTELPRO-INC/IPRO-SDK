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
/* After the FreeRTOS headers on purpose — ahead of them, lwipopts.h shadows the
 * port's TaskHandle_t/TickType_t and the file stops compiling.
 *
 * Presence-tested rather than assumed: defconfig_ipro6_lp is the No-WiFi trim
 * and does not put lwIP on this file's include path, so an unconditional
 * include is a hard build break there (caught 2026-07-30 — exactly what
 * building all three configs is for). */
#if defined(__has_include)
#  if __has_include("lwip/stats.h")
#    define LP_CLI_HAVE_LWIP 1
#  endif
#endif
#ifdef LP_CLI_HAVE_LWIP
#include "lwip/stats.h"
#include "lwip/memp.h"
#endif

#if defined(CONFIG_PHY_CLI)
/* B1 verify hook: cmd_init_do() is the ipro6_rf phyrf cold-cal entry (cli_app.c).
 * It runs phy_init + wl_init(en_full_cal=1); with WL_API_RMEM_EN=1 the resulting
 * wl_rmem_t cal is written into WLCAL (HAL_HBNRAM_WLCAL_ADDR = 0x20090B00,
 * retained HBN RAM), the exact slot the lp_fw warm-restore reads. Faithful,
 * self-contained (AON capcode, no rfparam), and HW-proven as `phy_init`. */
extern void cmd_init_do(uint32_t xtal_freq_hz, uint8_t capcode);
/* B2 verify hook: wl_lp_init() is the WiFi warm-restore entry (phy/wl_api.c). It
 * runs lp_default_init() + lp_rf_pri_init() (restores RF cal regs from the
 * WLCAL wl_cal) + lp_phy_init() (PHY/AGC/channel warm config). Run `lp rfcal`
 * first to seed WLCAL, then `lp rfwarm` to exercise the warm path. */
extern signed char wl_lp_init(unsigned char *rmem, unsigned short channelfreq_MHz);
#define LP_WLCAL_ADDR   0x20090B00UL

/* ---- `lp raw <ms> [cnt] [nousb]` cross-reboot state -----------------------
 * lp raw clears HBN_RSV0 so the wake is a full cold boot: BROM -> boot2 -> FW.
 * A repeat count therefore cannot live in a variable — it has to survive that,
 * so it goes in HBN RAM, which a PDS wake retains (only a power cut clears it).
 *
 * Slot picked from the gaps in the 4 KB HBN RAM map: 0x...0000 lp_fw image,
 * 0x0400 hal_lp param, 0x0B00 WLCAL, 0x0E00 the `lp hbn` scratch, 0x0E40 the
 * retention canary, 0x0F00 iot2lp_para. 0x0E80 is free.                      */
#define LP_RAW_CTL_ADDR   0x20090E80UL
#define LP_RAW_CTL_MAGIC  0x5241574CUL   /* "RAWL" */
#define LP_RAW_MAX_CNT    10000000u      /* ~347 days at 3 s/cycle — effectively no limit */

/* Word 4 is a checksum over the block. The count used to be bounded to 1000 and
 * that bound doubled as the guard against garbage in HBN RAM: a stale word that
 * happened to match the magic could otherwise carry a huge count and put the
 * board straight back to sleep on every boot, unrecoverable except by a power
 * cut. With the cap raised to 10M the range no longer discriminates, so the
 * block carries its own checksum instead. */
#define LP_RAW_SUM(l, m, u) (LP_RAW_CTL_MAGIC ^ (l) ^ ((m) * 2654435761u) ^ ((u) + 1u))

/* boot2's USB-download request word (apps/platform/ipro_boot2/boot2_usb_download.c).
 * Boot2 reads HBN_RSV3, and a magic of "US" with code SKIP makes it return before
 * usb_init() — no USB bring-up, no enumeration, no 500 ms poll. The request is
 * ONE-SHOT: boot2 clears it after reading, preserving [7:0], which on IPRO6 is the
 * BROM XTAL byte. So it must be re-armed before every sleep, and our write must
 * preserve [7:0] for the same reason. */
#define LP_HBN_RSV3_ADDR     0x3008F10CUL
#define LP_USB_DL_MAGIC      0x55530000UL
#define LP_USB_DL_SKIP_WORD  (LP_USB_DL_MAGIC | (0x00UL << 8))

/* Ask boot2 to skip its USB-download window on the next boot. */
static void lp_raw_arm_usb_skip(void)
{
    volatile uint32_t *rsv3 = (volatile uint32_t *)LP_HBN_RSV3_ADDR;
    *rsv3 = LP_USB_DL_SKIP_WORD | (*rsv3 & 0x000000FFu);
}

/* HBN_RSV2 USER_BOOT_SEL, consumed by BROM: 1 = force the BROM download
 * interface (boot2 uses that as its last-resort recovery, port_ipro7.c), 2 =
 * boot from media unconditionally. */
#define LP_USER_BOOT_MEDIA   2

/* Make the next boot come up from flash whatever the download strap says.
 *
 * The wake from PDS31 re-runs BROM, which samples the download pad -- and on a
 * board that pulls that pad up, every `lp raw` cycle lands in download mode
 * instead of booting, which is indistinguishable from a dead part on the
 * console. The override is the board-independent answer: it costs nothing on a
 * board strapped the normal way and it is what makes `lp raw` portable.
 *
 * HBN_RSV2 is in the AON domain, so this survives the PDS wake and is only
 * lost to a real power cut. Re-armed on every entry anyway -- BROM is not
 * documented to leave the field alone, and re-arming is one register write.
 * Use the driver API rather than storing 0x48000000: it is a read-modify-write
 * that keeps the LDO18IO power-delay fields sharing this register. */
static void lp_raw_arm_boot_media(void)
{
    HBN_Set_User_Boot_Config(LP_USER_BOOT_MEDIA);
}
#endif

/* Wait for UART TX to complete */
static void cli_uart_flush(void)
{
    /* tx_fifo_cnt = FREE slots (32 = drained); `> 0` was inverted — see the
     * uart_flush comment in lp_demo.c. Bounded, best-effort. */
    for (uint32_t spin = 0; spin < 200000u; spin++) {
        if (UART_GetTxFifoCount(UART0_ID) >= 32 &&
            !UART_GetTxBusBusyStatus(UART0_ID))
            break;
    }
    for (volatile int i = 0; i < 10000; i++) {}
}

static void cmd_lp_help(void)
{
    printf("LP Demo Commands:\r\n");
    printf("  lp sleep <ms> [cnt] - PDS sleep via hal_lp (cnt DTIM cycles before wake)\r\n");
    printf("  lp tcpdtim <host_ip> [period_s=20] [max_wakes=30] [port=5010] [dtim_ms=100] [cnt=60]\r\n");
    printf("                   - DTIM standby test: host sends 1 TCP msg per round\r\n");
    printf("  lp auto on/off   - Enable/disable auto PDS in idle\r\n");
    printf("  lp status        - Show sleep statistics\r\n");
    printf("  lp reset         - Reset statistics\r\n");
    printf("  lp heap          - Show free heap\r\n");
    printf("  lp rtc           - Show RTC counter (run twice to verify counting)\r\n");
    printf("  lp 32k [rc|xtal] [settle_ms] - Pick the f32k source; setting it also\r\n");
    printf("                     stops the sleep paths force-selecting RC32K\r\n");
    printf("  lp rtcrate [ms]  - Measure RTC Hz + ppm vs the MTimer (default 1000 ms)\r\n");
    printf("  lp raw <ms> [cnt] [nousb] - Raw PDS test (bypasses hal_lp, board reboots)\r\n");
    printf("                     cnt repeats across the reboot; nousb skips boot2's\r\n");
    printf("                     USB window. `lp raw stop` cancels a queued run.\r\n");
    printf("  lp raw2 <ms> [fdpr] - Minimal PDS test (f=flash d=dll p=pll r=rf)\r\n");
    printf("  lp dcdc [vsel] [en] - PDS DCDC11 rail (the live one; LDO11 is off in PDS)\r\n");
    printf("  lp hbn           - Show HBN register state\r\n");
    printf("  lp hbn <0|1> <ms>- HBN retention test (0=RAM kept,1=RAM lost; cold-boots)\r\n");
    printf("  lp clk           - Show saved sys_clk/xtal and current HW clock state\r\n");
    printf("  lp fastboot <ms> - Fastboot wake (cold boot on wake, does NOT return)\r\n");
    printf("  lp wakectx       - Show last fastboot wake context from HBN RAM\r\n");
#if defined(CONFIG_PHY_CLI)
    printf("  lp rfcal         - Run RF cold-cal -> WLCAL 0x20090b00 (seed lp_fw warm-restore)\r\n");
    printf("  lp rfwarm        - Warm-restore PHY/RF from WLCAL (wl_lp_init); run after rfcal\r\n");
#endif
    printf("  lp md <hex> [n]  - Dump n words from memory (e.g. lp md 0x20090b00 16)\r\n");
    printf("  lp lwipmem       - lwIP heap + memp pool usage (find ERR_MEM leaks)\r\n");
    printf("  lp tickless <host> [port] [delay_s] [dtim_ms] - arm FreeRTOS\r\n"
           "                     tickless DTIM standby; host wake msg disarms\r\n");
    printf("  lp tickless stop - disarm tickless standby\r\n");
    printf("  lp bcnlog [n]    - Decode last n DTIM-score records (default 16)\r\n");
    printf("  lp bcnstat [clear] - DTIM score: loss rate + idle + loss-level hist\r\n");
    printf("  lp track [lead_us] - P3.2 beacon tracking: show state / set RX lead\r\n"
           "                     (0 = off = fixed resleep; try 5000)\r\n");
    printf("  lp populate      - Seed iot2lp_para (BSSID/ch/AID) from connected STA\r\n");
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

/**
 * @brief Enter one raw PDS31 cycle. Never returns if PDS engages.
 *
 * @param ms      sleep length
 * @param left    cycles still to run AFTER this one; parked in HBN RAM so it
 *                survives the cold boot this causes. 0 = last cycle.
 * @param nousb   re-arm boot2's skip-the-USB-window request for the next boot.
 */
static void lp_raw_enter(uint32_t ms, uint32_t left, uint32_t nousb)
{
    volatile uint32_t *ctl = (volatile uint32_t *)LP_RAW_CTL_ADDR;

    /* Park the remaining count BEFORE sleeping. Written ahead of the entry, not
     * after the wake, so a cycle that never comes back cannot leave a stale
     * count behind to restart the run on the next manual boot. */
    if (left > 0) {
        ctl[1] = left;
        ctl[2] = ms;
        ctl[3] = nousb;
        ctl[4] = LP_RAW_SUM(left, ms, nousb);
        ctl[0] = LP_RAW_CTL_MAGIC;      /* magic LAST: the block is only claimed
                                         * valid once its contents are in place */
    } else {
        ctl[0] = 0;
    }

    /* Unconditional: the wake cold-boots through BROM, and a board that pulls
     * the download pad up would otherwise land in download mode every cycle. */
    lp_raw_arm_boot_media();

    if (nousb) {
        lp_raw_arm_usb_skip();
    }

    /* Clear HBN RSV0 so bootrom does cold boot (not LP wakeup) */
    *(volatile uint32_t *)(HBN_BASE + HBN_RSV0_OFFSET) = 0;
    *(volatile uint32_t *)(HBN_BASE + HBN_RSV1_OFFSET) = 0xFFFFFFFF;

    uint64_t sleep_us = (uint64_t)ms * 1000ULL;
    uint32_t rtc_ticks = (uint32_t)HAL_LP_US_TO_RTC_CNT(sleep_us);

    printf("Raw PDS Level 31: %lu ms (%lu rtc ticks)%s%s\r\n",
           (unsigned long)ms, (unsigned long)rtc_ticks,
           nousb ? ", boot2 USB wait SKIPPED" : "",
           left ? "" : " [last cycle]");
    if (left) {
        printf("  %lu more cycle(s) queued after this one\r\n", (unsigned long)left);
    }
    printf("Board should reboot in %lu ms...\r\n", (unsigned long)ms);
    cli_uart_flush();

    /* Direct call — pm_pds_mode_enter handles RTC setup + PDS entry */
    pm_pds_mode_enter(PM_PDS_LEVEL_31, rtc_ticks);

    /* Should not reach here for PDS31 (CPU reset, bootrom runs) */
    printf("ERROR: PDS did not reset CPU!\r\n");
    ctl[0] = 0;                      /* the run is over; do not resume on reboot */
}

/**
 * @brief Continue a multi-cycle `lp raw` run after the cold boot it caused.
 *
 * Called from lp_demo_init(). Returns 0 when there is nothing queued, so the
 * normal boot carries on; otherwise it never returns.
 */
int lp_raw_resume_if_queued(void)
{
    volatile uint32_t *ctl = (volatile uint32_t *)LP_RAW_CTL_ADDR;

    if (ctl[0] != LP_RAW_CTL_MAGIC || ctl[1] == 0 || ctl[1] > LP_RAW_MAX_CNT) {
        return 0;
    }

    uint32_t left  = ctl[1] - 1;
    uint32_t ms    = ctl[2];
    uint32_t nousb = ctl[3];

    /* Checksum before acting. Without it, garbage that happens to hit the magic
     * would sleep the board on every boot with no way back short of a power cut
     * — the count alone stopped being a sanity check once the cap went to 10M. */
    if (ctl[4] != LP_RAW_SUM(ctl[1], ms, nousb) || ms < 10) {
        printf("[LP] raw run: stale/corrupt control block, ignoring\r\n");
        ctl[0] = 0;
        return 0;
    }

    printf("[LP] raw run: %lu cycle(s) left, sleeping %lu ms again\r\n",
           (unsigned long)ctl[1], (unsigned long)ms);
    cli_uart_flush();
    lp_raw_enter(ms, left, nousb);
    return 1;
}

static void cmd_lp_raw(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: lp raw <ms> [cnt] [nousb]\r\n");
        printf("  This bypasses hal_lp and does a raw PDS Level 31.\r\n");
        printf("  Board should REBOOT after <ms> (no HBN RAM restore).\r\n");
        printf("  cnt   - repeat the sleep+reboot cnt times (default 1, max %lu).\r\n",
               (unsigned long)LP_RAW_MAX_CNT);
        printf("          The count rides in HBN RAM, which survives the reboot.\r\n");
        printf("  nousb - ask boot2 to skip its USB-download window on each wake\r\n");
        printf("          (HBN_RSV3 magic; boot2 clears it, so it is re-armed\r\n");
        printf("          every cycle).\r\n");
        printf("  `lp raw stop` clears a queued run.\r\n");
        return;
    }

    if (strcmp(argv[2], "stop") == 0) {
        *(volatile uint32_t *)LP_RAW_CTL_ADDR = 0;
        printf("Queued raw run cleared.\r\n");
        return;
    }

    uint32_t ms = (uint32_t)atoi(argv[2]);
    if (ms < 10) {
        printf("Min 10 ms\r\n");
        return;
    }

    uint32_t cnt = (argc > 3) ? (uint32_t)atoi(argv[3]) : 1u;
    if (cnt == 0) {
        cnt = 1;
    }
    if (cnt > LP_RAW_MAX_CNT) {
        printf("cnt capped at %lu\r\n", (unsigned long)LP_RAW_MAX_CNT);
        cnt = LP_RAW_MAX_CNT;
    }

    uint32_t nousb = 0;
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "nousb") == 0 || strcmp(argv[i], "u") == 0) {
            nousb = 1;
        }
    }

    lp_raw_enter(ms, cnt - 1u, nousb);
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
/*
 * Phase-3 DTIM score readout (ring layout: lp_fw.h lp_bcn_ring_t, written by
 * the blob at LP_BCN_RING_ADDR in retained OCRAM). All printf here runs in the
 * carrier AFTER wake — the blob itself only writes RAM records.
 */
static void cmd_lp_bcnlog(int argc, char **argv)
{
    volatile lp_bcn_ring_t *ring = (volatile lp_bcn_ring_t *)LP_BCN_RING_ADDR;
    uint32_t n = (argc > 2) ? (uint32_t)atoi(argv[2]) : 16u;

    if (ring->magic != LP_BCN_RING_MAGIC) {
        printf("[LP] bcnlog: ring not initialized (no blob cycle since POR)\r\n");
        return;
    }
    uint32_t avail = (ring->wr < ring->depth) ? ring->wr : ring->depth;
    if (n > avail) {
        n = avail;
    }
    printf("[LP] bcnlog: %lu records total, showing last %lu (depth %lu)\r\n",
           (unsigned long)ring->wr, (unsigned long)n, (unsigned long)ring->depth);
    /* Column semantics split by record kind (see lp_fw.h lp_bcn_record_t):
     * BCN rows:  dly = beacon-grid delay excess us, a/l = adcpow[7:0]
     *            (live 0x60-0x70), ppm = live rtc32k_error_ppm.
     * LOSS rows: dly = rc_static 0x24C0C000 (rf_sel_2g4 = bit8), a/l = P3.3
     *            graded level, ppm = RC2_ST1 raw.
     * win = poll window ms this cycle (adaptive shrink visibility),
     * st +/- = RC2_ST1 cycled/frozen across the window. */
    printf("  seq    rtc_us     tsf_us     t_rdy  t_wait  dly        a/l win ppm/rc2    st flags\r\n");
    for (uint32_t i = ring->wr - n; i < ring->wr; i++) {
        volatile lp_bcn_record_t *r = &ring->rec[i % ring->depth];
        printf("  %-6lu %-10lu %-10lu %-6lu %-7lu ",
               (unsigned long)r->seq,
               (unsigned long)r->rtc_us_lo,
               (unsigned long)r->tsf_us_lo,
               (unsigned long)r->t_ready_us,
               (unsigned long)r->t_wait_us);
        if (r->flags & LP_BCNREC_LOSS) {
            /* raw register snapshots (rc_static / RC2_ST1) — hex */
            printf("%08lx   %02x  %-3u %08lx   ",
                   (unsigned long)(uint32_t)r->delay_us,
                   (unsigned)r->loss_level,
                   (unsigned)r->trim_code,
                   (unsigned long)(uint32_t)r->err_ppm);
        } else {
            /* tracking telemetry (grid delay us / live ppm) — decimal */
            printf("%-10ld %02x  %-3u %-10ld ",
                   (long)r->delay_us,
                   (unsigned)r->loss_level,
                   (unsigned)r->trim_code,
                   (long)r->err_ppm);
        }
        printf("%c  %s%s%s%s%s%s\r\n",
               (r->flags & LP_BCNREC_RC2_LIVE)  ? '+' : '-',
               (r->flags & LP_BCNREC_BEACON)    ? "BCN "  : "",
               (r->flags & LP_BCNREC_DTIM)      ? "DTIM " : "",
               (r->flags & LP_BCNREC_TIM_HIT)   ? "TIM "  : "",
               (r->flags & LP_BCNREC_BUDGET)    ? "BUDG " : "",
               (r->flags & LP_BCNREC_CONVERGED) ? "CNV "  : "",
               (r->flags & LP_BCNREC_LOSS)      ? "LOSS"  : "");
    }
}

static void cmd_lp_bcnstat(int argc, char **argv)
{
    volatile lp_bcn_ring_t *ring = (volatile lp_bcn_ring_t *)LP_BCN_RING_ADDR;

    if (ring->magic != LP_BCN_RING_MAGIC) {
        printf("[LP] bcnstat: ring not initialized (no blob cycle since POR)\r\n");
        return;
    }
    if (argc > 2 && strcmp(argv[2], "clear") == 0) {
        /* Invalidate the magic: the blob re-zeroes + re-inits on its next run. */
        ring->magic = 0u;
        printf("[LP] bcnstat: cleared\r\n");
        return;
    }
    uint32_t cycles = ring->recv_cnt + ring->loss_cnt;
    printf("[LP] DTIM score (%lu cycles):\r\n", (unsigned long)cycles);
    printf("  KPI-1 recv %lu, loss %lu",
           (unsigned long)ring->recv_cnt, (unsigned long)ring->loss_cnt);
    if (cycles) {
        printf(" (loss %lu.%02lu%%)",
               (unsigned long)(ring->loss_cnt * 100u / cycles),
               (unsigned long)((ring->loss_cnt * 10000u / cycles) % 100u));
    }
    printf("\r\n");
    if (cycles) {
        printf("  KPI-2 idle avg %lu us, max %lu us (t_ready avg %lu us)\r\n",
               (unsigned long)(ring->idle_sum_us / cycles),
               (unsigned long)ring->idle_max_us,
               (unsigned long)(ring->tready_sum_us / cycles));
    }
    printf("  t_wait hist: <2ms %lu | 2-10ms %lu | 10-50ms %lu | >=50ms %lu\r\n",
           (unsigned long)ring->hist[0], (unsigned long)ring->hist[1],
           (unsigned long)ring->hist[2], (unsigned long)ring->hist[3]);
    /* P3.3 loss-level histogram: bin the retained LOSS records by their graded
     * level (L0..L5). Reflects the levels still in the ring (last 'depth'
     * cycles), so V-3 injection shows the two-phase escalation front (L0-2
     * early-wake) -> back (L3-5 listen-stretch). Counts differ from KPI-1 loss
     * once the ring wraps -- this is the recent window, loss_cnt is lifetime. */
    uint32_t loss_hist[6] = {0};
    uint32_t avail = (ring->wr < ring->depth) ? ring->wr : ring->depth;
    for (uint32_t i = ring->wr - avail; i < ring->wr; i++) {
        volatile lp_bcn_record_t *r = &ring->rec[i % ring->depth];
        if ((r->flags & LP_BCNREC_LOSS) && r->loss_level < 6u) {
            loss_hist[r->loss_level]++;
        }
    }
    printf("  loss-level hist L0..L5: %lu %lu %lu %lu %lu %lu\r\n",
           (unsigned long)loss_hist[0], (unsigned long)loss_hist[1],
           (unsigned long)loss_hist[2], (unsigned long)loss_hist[3],
           (unsigned long)loss_hist[4], (unsigned long)loss_hist[5]);
}

/*
 * Dump the lwIP heap and every non-idle memp pool.
 *
 * Exists because the interesting failure -- connect() returning ERR_MEM, after
 * which the DUT can never rebuild its host socket and only an external reset
 * recovers it -- was only observable during a random DTIM episode, 13-43 min
 * apart. Measured 2026-07-30: the lwIP heap grows exactly 540 B per FAILED
 * connect and is never reclaimed, so the retry ladder exhausts the 48 KB heap
 * itself and the DUT's own recovery attempts are what make recovery impossible.
 * With this command the leak is a controlled experiment instead of a stakeout:
 * read, force some failed connects (connect to a dead port), read again.
 */
static void cmd_lp_lwipmem(void)
{
#if !defined(LP_CLI_HAVE_LWIP)
    printf("[LWIP] no lwIP in this build\r\n");
#else
#if LWIP_STATS && MEM_STATS
    printf("[LWIP] heap used=%u max=%u avail=%u err=%u illegal=%u\r\n",
           (unsigned)lwip_stats.mem.used, (unsigned)lwip_stats.mem.max,
           (unsigned)lwip_stats.mem.avail, (unsigned)lwip_stats.mem.err,
           (unsigned)lwip_stats.mem.illegal);
#else
    printf("[LWIP] MEM_STATS not compiled in\r\n");
#endif
#if LWIP_STATS && MEMP_STATS
    /* Index, not MEMP_*: the pool set differs between lwIP configs and a wrong
     * name is a build break. Map with the LWIP_*MEMPOOL order in
     * lwip/priv/memp_std.h. */
    for (int i = 0; i < MEMP_MAX; i++) {
        const struct stats_mem *ms = lwip_stats.memp[i];
        if (ms != NULL && (ms->used != 0 || ms->err != 0)) {
            printf("[LWIP] memp[%2d] used=%4u max=%4u avail=%4u err=%u\r\n",
                   i, (unsigned)ms->used, (unsigned)ms->max,
                   (unsigned)ms->avail, (unsigned)ms->err);
        }
    }
#endif
#endif /* LP_CLI_HAVE_LWIP */
}

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

/* Jump into the staged lp_fw Stage-1 image (reserved OCRAM slot 0x11046000) to
 * verify it executes from retained RAM. Phase-1 lp_fw is stamp-only (no console
 * on the clock-gated wake path): it writes proof-of-execution into HBN RAM and
 * returns the wake decision. Read it back afterwards with `lp md 0x20090f00`
 * (expect [0]=0x1 wakeup_reason, [4] low16=0x5A5A aid). In this live FreeRTOS
 * context the return (with interrupts masked) trips a FreeRTOS assert -- that is
 * expected, reset the board afterwards. The real wake path returns cleanly to
 * the HBN stub. */
static void cmd_lp_fwjump(void)
{
    printf("Jumping to lp_fw @0x11046000 (stamps 0x20090f00; read back with 'lp md')\r\n");
    /* let the console drain before handing the core to lp_fw */
    for (volatile int i = 0; i < 2000000; i++) {
    }
    /* lp_fw_start hijacks sp/gp for its own context. In this live FreeRTOS
     * test it runs with interrupts on, so a tick IRQ would re-enter the full
     * firmware's ISR under lp_fw's gp -> garbage globals. Mask interrupts
     * globally first; the real wake path already enters lp_fw interrupts-off. */
    __asm__ volatile("csrci mstatus, 0x8" ::: "memory"); /* clear MIE */
    ((void (*)(void))(uintptr_t)0x11046000u)();
}

/**
 * @brief Measure the live HBN RTC rate against the MTimer, and report ppm.
 *
 * The MTimer is the reference: it runs off XCLK (the 40 MHz crystal, divided to
 * 1 MHz by CPU_Set_MTimer_Toggle_Cnt in board_clock_init), so it is accurate to
 * the crystal and completely independent of F32K_SEL. Counting RTC ticks over a
 * measured MTimer window therefore gives the true 32 kHz rate whichever source
 * is selected.
 *
 * Expect roughly: RC32K ~34200 Hz (+43000 ppm, and it WANDERS by a few hundred
 * ppm run to run); XTAL32K ~32768 Hz (tens of ppm, stable). A reading pinned at
 * exactly 0 delta means the 32 kHz domain is dead — which is what selecting a
 * crystal that has not started looks like.
 */
static void cmd_lp_rtcrate(int argc, char **argv)
{
    uint32_t win_ms = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 1000;
    if (win_ms < 50) {
        win_ms = 50;        /* under ~50 ms the tick quantisation dominates */
    }

    uint32_t rl, rh;
    HBN_Get_RTC_Timer_Val(&rl, &rh);
    uint64_t r0 = ((uint64_t)rh << 32) | rl;
    uint64_t t0 = CPU_Get_MTimer_US();

    uint64_t t1;
    do {
        t1 = CPU_Get_MTimer_US();
    } while ((t1 - t0) < (uint64_t)win_ms * 1000ULL);

    HBN_Get_RTC_Timer_Val(&rl, &rh);
    uint64_t r1 = ((uint64_t)rh << 32) | rl;

    uint64_t drtc = r1 - r0;
    uint64_t dus  = t1 - t0;

    uint8_t is_xtal = 0, forced = 0;
    pm_pds_get_32k_src(&is_xtal, &forced);

    if (drtc == 0 || dus == 0) {
        printf("[LP] rtcrate: RTC did NOT advance in %lu us (src=%s) — 32k domain dead\r\n",
               (unsigned long)dus, is_xtal ? "XTAL32K" : "RC32K");
        return;
    }

    uint32_t hz = (uint32_t)((drtc * 1000000ULL) / dus);
    /* ppm vs the nominal 32768 Hz, computed without leaving integer maths:
     * ppm = (drtc / (32768 * dus / 1e6) - 1) * 1e6. Worst case here is
     * ~3.3e16, well inside int64. */
    int64_t ppm = ((int64_t)drtc * 1000000LL * 1000000LL)
                  / (32768LL * (int64_t)dus) - 1000000LL;

    printf("[LP] rtcrate: %lu Hz  (%ld ppm vs 32768)  src=%s%s\r\n",
           (unsigned long)hz, (long)ppm,
           is_xtal ? "XTAL32K" : "RC32K",
           forced ? " [sleep still forces RC]" : "");
    printf("             ticks=%lu over %lu us (window %lu ms)\r\n",
           (unsigned long)drtc, (unsigned long)dus, (unsigned long)win_ms);
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
    } else if (strcmp(argv[1], "dcdc") == 0) {
        /* PDS powers LDO11 down (pdsCtl.ldo11Off = 1) and rides DCDC11, so this
         * is the rail that can actually move the sleep floor — the LDO11 vout
         * sweep measured flat across its whole range for exactly that reason.
         * Has to be a runtime knob rather than an `lp mw` poke: PDS_Enable()
         * rewrites the entire PDS_CTL word from pdsCfgLevel15 at every entry,
         * so a register write from the console never survives to the sleep.
         * `en` (pdsDcdc11VselEn) is exposed too — without it a flat vsel result
         * cannot be told apart from an inert control path. */
        uint8_t vsel = 0, en = 0;
        if (argc > 2) {
            uint8_t want_vsel = (uint8_t)strtoul(argv[2], NULL, 0);
            uint8_t want_en   = (argc > 3) ? (uint8_t)atoi(argv[3]) : 1;
            pm_pds_set_dcdc11_vsel(want_vsel, want_en);
        }
        pm_pds_get_dcdc11_vsel(&vsel, &en);
        printf("[LP] pds dcdc11: vsel=%u (0x%02x) en=%u\r\n",
               (unsigned)vsel, (unsigned)vsel, (unsigned)en);
    } else if (strcmp(argv[1], "32k") == 0) {
        /* Pick the 32 kHz source at run time so an RC-vs-XTAL A/B needs no
         * reflash. Setting it also stops the sleep paths force-selecting RC —
         * without that the choice never reaches the sleep (see
         * pm_pds_set_32k_src). Switching to XTAL from cold needs a real settle:
         * HBN_Power_On_Xtal_32K() waits 1.1 ms despite its "Delay >1s" comment,
         * and selecting a crystal that is not yet oscillating stops the whole
         * 32 kHz domain — HBN RTC and the PDS/AON sequencer both. */
        if (argc > 2) {
            uint32_t settle = (argc > 3) ? (uint32_t)strtoul(argv[3], NULL, 0) : 500;
            if (strcmp(argv[2], "xtal") == 0) {
                printf("[LP] 32k -> XTAL32K (settle %lu ms)\r\n", (unsigned long)settle);
                cli_uart_flush();
                pm_pds_set_32k_src(1, settle);
            } else if (strcmp(argv[2], "rc") == 0) {
                pm_pds_set_32k_src(0, 0);
            } else {
                printf("Usage: lp 32k [rc|xtal] [settle_ms]\r\n");
                return 0;
            }
        }
        uint8_t is_xtal = 0, forced = 0;
        pm_pds_get_32k_src(&is_xtal, &forced);
        printf("[LP] f32k = %s%s\r\n", is_xtal ? "XTAL32K" : "RC32K",
               forced ? "  (sleep paths still FORCE RC — set it once to take over)" : "");
    } else if (strcmp(argv[1], "rtcrate") == 0) {
        cmd_lp_rtcrate(argc, argv);
    } else if (strcmp(argv[1], "hbn") == 0) {
        cmd_lp_hbn(argc, argv);
    } else if (strcmp(argv[1], "clk") == 0) {
        cmd_lp_clk();
    } else if (strcmp(argv[1], "raw") == 0) {
        cmd_lp_raw(argc, argv);
    } else if (strcmp(argv[1], "raw2") == 0) {
        cmd_lp_raw2(argc, argv);
    } else if (strcmp(argv[1], "rfcal") == 0) {
#if defined(CONFIG_PHY_CLI)
        printf("RF cold-cal (phy_init + wl_init full_cal) -> WLCAL 0x20090b00 ...\r\n");
        cli_uart_flush();
        cmd_init_do(40000000, 32);
        printf("RF cal done. Verify: lp md 0x20090b00 16\r\n");
#else
        printf("rfcal not built (needs CONFIG_PHY_CLI=y)\r\n");
#endif
    } else if (strcmp(argv[1], "rfwarm") == 0) {
#if defined(CONFIG_PHY_CLI)
        printf("Warm-restore: wl_lp_init(0x20090b00, 2412) ...\r\n");
        cli_uart_flush();
        signed char r = wl_lp_init((unsigned char *)LP_WLCAL_ADDR, 2412);
        printf("wl_lp_init ret=%d. AGC mem @0x24c0a000:\r\n", (int)r);
        for (int i = 0; i < 4; i++) {
            unsigned long a = 0x24C0A000UL + (unsigned long)i * 4;
            printf("  [0x%08lx] = 0x%08lx\r\n", a,
                   (unsigned long)*(volatile unsigned int *)(uintptr_t)a);
        }
#else
        printf("rfwarm not built (needs CONFIG_PHY_CLI=y)\r\n");
#endif
    } else if (strcmp(argv[1], "populate") == 0) {
        /* Seed iot2lp_para from the connected STA — the same call the sleep
         * path makes automatically. Run after connecting to an AP; verify the
         * whole block with: lp md 0x20090f00 16 */
        lpfw_populate_from_sta();
    } else if (strcmp(argv[1], "seed") == 0) {
        /* Association-free iot2lp_para seeding for deterministic blob repro:
         * mirrors lpfw_populate_from_sta's write set with the bench AP's
         * (INTELPRO-LAB02) identity. The blob then RXes the AP's beacons and
         * DTIM-cycles exactly as when associated — the AID's TIM bit just
         * never gets set. Usage: lp seed [ch] [aid] */
        volatile iot2lp_para_t *para = IOT2LP_PARA;
        static const uint8_t seed_bssid[6] = {0xbc,0x22,0x28,0x8f,0x39,0xe1};
        static const uint8_t seed_lmac[6]  = {0x12,0x23,0x34,0x00,0x45,0x56};
        uint8_t  seed_ch  = (argc > 2) ? (uint8_t)atoi(argv[2]) : 2;
        uint16_t seed_aid = (argc > 3) ? (uint16_t)atoi(argv[3]) : 1;
        for (int i = 0; i < 6; i++) {
            para->bssid[i]     = seed_bssid[i];
            para->local_mac[i] = seed_lmac[i];
        }
        para->ap_channel          = seed_ch;
        para->aid                 = seed_aid;
        para->beacon_interval_tu  = 100;
        para->dtim_num            = 1;
        para->beacon_data_rate    = 2;
        para->bcn_target_level    = 0;
        para->ram_wifi_start_addr = 0x21050000u;
        /* P3.2: seeding rewrites the AP identity — reset the tracking anchor
         * + delay window (mirror of lpfw_populate_from_sta's BSSID gate). */
        para->last_beacon_stamp_rtc_valid  = 0;
        para->bcn_delay_sliding_win_status = 0;
        para->bcn_delay_sliding_win_point  = 0;
        para->last_beacon_delay_us         = 0;
        para->ppm_stable_cnt               = 0;
        para->bcn_delay_sliding_win_size   = LP_FW_BCN_DELAY_WIN_MAX;
        printf("[LP] seeded LAB02 ctx: ch%u aid%u\r\n",
               (unsigned)seed_ch, (unsigned)seed_aid);
    } else if (strcmp(argv[1], "track") == 0) {
        /* P3.2 beacon tracking runtime switch + state dump. rx_lead_us gates
         * everything (0 = legacy fixed resleep) so one build serves both the
         * baseline and the tracking leg of the A/B. */
        volatile iot2lp_para_t *para = IOT2LP_PARA;
        if (argc > 2) {
            para->rx_lead_us = (uint32_t)atoi(argv[2]);
        }
        printf("[LP] track: rx_lead_us=%lu (%s) err_ppm=%ld win %lu/%lu "
               "offset=%ld last_extra=%ld anchor=%u stab=%lu conv=%d\r\n",
               (unsigned long)para->rx_lead_us,
               para->rx_lead_us ? "ON" : "OFF",
               (long)para->rtc32k_error_ppm,
               (unsigned long)para->bcn_delay_sliding_win_status,
               (unsigned long)para->bcn_delay_sliding_win_size,
               (long)para->bcn_delay_offset,
               (long)para->last_beacon_delay_us,
               (unsigned)para->last_beacon_stamp_rtc_valid,
               (unsigned long)para->ppm_stable_cnt,
               lp_fw_track_shrink_ok(para));
    } else if (strcmp(argv[1], "tcpdtim") == 0) {
        if (argc < 3) {
            printf("Usage: lp tcpdtim <host_ip> [period_s] [max_wakes] [port] [dtim_ms] [cnt]\r\n");
        } else {
            uint32_t period = (argc > 3) ? (uint32_t)atoi(argv[3]) : 20;
            uint32_t maxw   = (argc > 4) ? (uint32_t)atoi(argv[4]) : 30;
            uint16_t port   = (argc > 5) ? (uint16_t)atoi(argv[5]) : 5010;
            uint32_t dtim   = (argc > 6) ? (uint32_t)atoi(argv[6]) : 100;
            uint32_t cnt    = (argc > 7) ? (uint32_t)atoi(argv[7]) : 60;
            lp_tcpdtim_run(argv[2], port, period, maxw, dtim, cnt);
        }
    } else if (strcmp(argv[1], "macset") == 0) {
        /* lp macset [rand|<12 hex digits>] — MUST run BEFORE stack_wifi.
         * Bench tool: the lab APs escalate a per-MAC SAE throttle after
         * repeated same-MAC connects ('tx auth frame alloc failure'); a
         * fresh locally-administered random MAC dodges it. */
#ifdef CONFIG_WIFI6
        extern int wifi_mgmr_mac_set(uint8_t mac[6]);
        extern uint64_t CPU_Get_MTimer_Counter(void);
        uint8_t mac[6];
        if (argc > 2 && strlen(argv[2]) == 12) {
            for (int i = 0; i < 6; i++) {
                char b[3] = { argv[2][2 * i], argv[2][2 * i + 1], 0 };
                mac[i] = (uint8_t)strtoul(b, NULL, 16);
            }
        } else {
            uint64_t t = CPU_Get_MTimer_Counter();
            mac[0] = 0x02;              /* locally administered, unicast */
            mac[1] = 0x23;
            mac[2] = 0x34;
            mac[3] = (uint8_t)(t >> 16);
            mac[4] = (uint8_t)(t >> 8);
            mac[5] = (uint8_t)t;
        }
        wifi_mgmr_mac_set(mac);
        printf("[LP] STA MAC set: %02x:%02x:%02x:%02x:%02x:%02x "
               "(run before stack_wifi)\r\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#else
        printf("[LP] macset: No-WiFi build\r\n");
#endif
    } else if (strcmp(argv[1], "tickless") == 0) {
        /* lp tickless <host> [port=3333] [delay_s=20] [dtim_ms=100]
         *                    [repeat=0] [awake_s=60]
         * lp tickless stop */
        if (argc > 2 && strcmp(argv[2], "stop") == 0) {
            lp_tickless_stop();
        } else if (argc < 3) {
            printf("Usage: lp tickless <host> [port] [delay_s] [dtim_ms] "
                   "[repeat] [awake_s] "
                   "| lp tickless seed [entries] [dtim_ms] "
                   "| lp tickless stop\r\n");
        } else if (strcmp(argv[2], "seed") == 0) {
            /* plumbing mode: entries cap rides the delay_s parameter */
            uint32_t entries = (argc > 3) ? (uint32_t)atoi(argv[3]) : 20;
            uint32_t dtim    = (argc > 4) ? (uint32_t)atoi(argv[4]) : 100;
            lp_tickless_start("seed", 0, entries, dtim, 0, 0);
        } else {
            uint16_t port    = (argc > 3) ? (uint16_t)atoi(argv[3]) : 3333;
            uint32_t delay_s = (argc > 4) ? (uint32_t)atoi(argv[4]) : 20;
            uint32_t dtim    = (argc > 5) ? (uint32_t)atoi(argv[5]) : 100;
            uint32_t repeat  = (argc > 6) ? (uint32_t)atoi(argv[6]) : 0;
            uint32_t awake_s = (argc > 7) ? (uint32_t)atoi(argv[7]) : 60;
            lp_tickless_start(argv[2], port, delay_s, dtim, repeat, awake_s);
        }
    } else if (strcmp(argv[1], "lwipmem") == 0) {
        cmd_lp_lwipmem();
    } else if (strcmp(argv[1], "bcnlog") == 0) {
        cmd_lp_bcnlog(argc, argv);
    } else if (strcmp(argv[1], "bcnstat") == 0) {
        cmd_lp_bcnstat(argc, argv);
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
