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
 * @file lp_demo.c
 * @brief IPRO6 Low Power demo — PDS Level 31 via hal_lp framework (No BLE)
 *
 * Ported into ipro6_demo from the standalone ipro_lp_test app. Enters PDS sleep
 * with RTC wakeup and a DTIM re-sleep simulation driven by the staged lp_fw
 * Stage-1 image. After wakeup it reinitializes UART and SysTick.
 */

#include "lp_demo.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "hal_lp.h"
#include "hal_pm.h"             /* pm_pds_resleep */
#include "hal_hbnram_mgnt.h"    /* hal_lp_is_fastboot_wakeup / wake context */
#include "lp_fw.h"              /* iot2lp_para_t, IOT2LP_PARA, decision/cycle defs */
#include "drv_uart.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_l1c.h"
#include "system_soc.h"
#include "compiler/common.h"   /* ATTR_TCM_SECTION */

/*
 * EMBEDDED lp_fw STAGE-1 IMAGE
 *
 * The standalone lp_fw image (apps/platform/ipro6_lpfw) is built separately
 * and embedded here via lpfw_blob.S. It must be memcpy'd to its run address at
 * boot so the HBN stub can enter it after a PDS wake.
 *
 * The run address is in the reserved top 32K of OCRAM (0x11048000), NOT WRAM:
 * OCRAM is retained across PDS31 by the HW default, whereas WRAM is in the WiFi
 * power domain (gated off during PDS) so its contents are lost on wake.
 */
extern const uint8_t __lpfw_start[];
extern const uint8_t __lpfw_end[];

#define LPFW_RUN_ADDR   0x11048000u     /* lp_fw VMA (reserved OCRAM slot, cacheable) */

/* Wait for UART TX to complete before entering PDS */
static void uart_flush(void)
{
    /* tx_fifo_cnt counts FREE slots (reset 0x20 = empty, see uart_reg.h): the
     * FIFO is drained when all 32 are free again. The old `> 0` test waited
     * for FIFO-FULL and only escaped by racing printf's last byte (free==0);
     * any IRQ/preemption >87us (one byte at 115200) in that window let a byte
     * drain first and the loop span forever. Bounded: a full FIFO drains in
     * ~2.8ms at 115200 and the flush is best-effort before sleep. */
    for (uint32_t spin = 0; spin < 200000u; spin++) {
        if (UART_GetTxFifoCount(UART0_ID) >= 32 &&
            !UART_GetTxBusBusyStatus(UART0_ID))
            break;
    }
    /* Extra guard: ~1ms at 115200 for last byte shift out */
    for (volatile int i = 0; i < 10000; i++) {}
}

/* Minimum sleep duration to bother entering PDS (ms) */
#define LP_MIN_SLEEP_MS     5

/*
 * PDS ENTRY TRAMPOLINE
 *
 * Problem: FreeRTOS task stacks may be in non-retention OCRAM (heap at
 * 0x11020000+). PDS only retains the first 64KB (0x11010000~0x11020000).
 * On wakeup, stack contents at heap addresses are garbage → crash.
 *
 * Solution: Switch to a small stack in retention OCRAM (.data section)
 * before calling hal_lp_enter().
 */
#define PDS_STACK_SIZE      2048

static uint8_t __attribute__((aligned(16), used))
    s_pds_stack[PDS_STACK_SIZE];

/*
 * MODULE STATE
 */

static bool     s_initialized   = false;
static bool     s_auto_enabled  = false;
static uint32_t s_total_ms      = 0;
static uint32_t s_sleep_count   = 0;
static uint32_t s_last_ms       = 0;
static int32_t  s_last_wakeup   = 0;

/* Manual sleep arguments (naked function can't forward args) */
uint32_t s_lp_demo_sleep_ms;
uint32_t s_lp_demo_sleep_cnt;   /* DTIM cycles before wake (0 => default) */

/*
 * WAKEUP HOOK — reinitialize peripherals after PDS wakeup
 */

static int lp_wakeup_hook(int wakeup_reason, void *arg)
{
    (void)arg;
    (void)wakeup_reason;

    /* PDS31 leaves the clock tree and MISC-domain pinmux in a reset state: the
     * MCU root, HCLK/BCLK dividers, UART clock select and the UART0 pad mux no
     * longer match the pre-sleep (boot) configuration. The framework wake path
     * only restores the MCU core clock for XIP; HCLK/BCLK and the console pins
     * are still wrong, so the UART driver computes its baud divisor against the
     * wrong clock and the console prints garbage.
     *
     * Re-run the board bring-up to restore the exact boot clock tree (PLL 240M
     * + HCLK/BCLK div) and UART0 pinmux (GPIO28 TX / GPIO29 RX), then pin the
     * UART clock back to XCLK (matching system_soc startup) before re-init. */
    extern void board_init(void);
    board_init();
    GLB_Set_UART_CLK(1, HBN_UART_CLK_XCLK, 0);

    /* Reinitialize UART for shell/logging — recomputes baud from the now-correct clock */
#ifdef CONFIG_SHELL
    extern void shell_uart_reinit(void);
    shell_uart_reinit();
#endif

    /* Reinitialize FreeRTOS SysTick timer */
    extern void vPortSetupTimerInterrupt(void);
    vPortSetupTimerInterrupt();

    return 0;
}

/*
 * PDS sleep implementation — runs on retention stack
 */
static void __attribute__((used)) lp_demo_sleep_impl(void)
{
    uint32_t sleep_ms = s_lp_demo_sleep_ms;
    uint32_t cycles = s_lp_demo_sleep_cnt ? s_lp_demo_sleep_cnt
                                          : LP_FW_SIM_DTIM_CYCLES;

    if (!s_initialized) {
        printf("[LP] Not initialized\r\n");
        return;
    }
    if (sleep_ms < LP_MIN_SLEEP_MS) {
        return;
    }

    hal_lp_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.rtc_wakeup_en = 1;
    cfg.rtc_sleep_us = (uint64_t)sleep_ms * 1000ULL;
    cfg.flash_power_down = 0;  /* Start simple: don't power down flash */
    cfg.ocram_retention = 1;

    /* DTIM simulation: arm lp_fw to re-sleep this same interval for `cycles`
     * beacons before waking the full firmware (cycles from `lp sleep <ms> <cnt>`,
     * default LP_FW_SIM_DTIM_CYCLES). The counter lives in always-on HBN RAM so it
     * survives every PDS cycle; reset it here so each `lp sleep` starts fresh. */
    volatile iot2lp_para_t *para = IOT2LP_PARA;
    para->resleep_us = (uint32_t)cfg.rtc_sleep_us;
    para->sim_dtim_count = 0;
    para->sim_dtim_target = cycles;

    hal_lp_result_t result;
    memset(&result, 0, sizeof(result));

    printf("[LP] PDS sleep: %lu ms x%lu DTIM cycles (rtc_us=%llu, flash_pd=%d, ocram_ret=%d)\r\n",
           (unsigned long)sleep_ms,
           (unsigned long)cycles,
           (unsigned long long)cfg.rtc_sleep_us,
           cfg.flash_power_down,
           cfg.ocram_retention);
    uart_flush();

    int ret = hal_lp_enter(&cfg, &result);
    (void)ret;

    /* === WAKEUP === */

    /* Compensate FreeRTOS tick counter */
    uint32_t actual_us = (uint32_t)result.sleep_duration_us;
    if (actual_us == 0) actual_us = sleep_ms * 1000;
    uint32_t actual_ms = actual_us / 1000;

    uint32_t sleep_ticks = actual_us / (1000000UL / configTICK_RATE_HZ);
    if (sleep_ticks > 0) {
        xTaskCatchUpTicks(sleep_ticks);
    }

    /* Update statistics */
    s_total_ms += actual_ms;
    s_sleep_count++;
    s_last_ms = actual_ms;
    s_last_wakeup = result.wakeup_reason;

    const char *rs = "UNKNOWN";
    switch (result.wakeup_reason) {
        case HAL_LP_WAKEUP_RTC:   rs = "RTC";   break;
        case HAL_LP_WAKEUP_GPIO:  rs = "GPIO";  break;
        case HAL_LP_WAKEUP_WIFI:  rs = "WIFI";  break;
        case HAL_LP_WAKEUP_ACOMP: rs = "ACOMP"; break;
        case HAL_LP_WAKEUP_BLE:   rs = "BLE";   break;
        default: break;
    }
    printf("[LP] Woke: reason=0x%lx (%s), %lu ms (cnt=%lu)\r\n",
           (unsigned long)result.wakeup_reason, rs,
           (unsigned long)actual_ms,
           (unsigned long)s_sleep_count);

    /* DTIM simulation result: lp_fw should have cycled `cycles` times (so
     * actual_ms ~= cycles * interval) and woken us with a WIFI cause. This is the
     * proof the full firmware stayed asleep through all the cycles. */
    printf("[LP] DTIM sim: lp_fw cycled %lu/%lu, lpfw_reason=0x%lx, aid=0x%04x\r\n",
           (unsigned long)para->sim_dtim_count,
           (unsigned long)cycles,
           (unsigned long)para->wakeup_reason,
           (unsigned)para->aid);
}

/*
 * Naked trampoline — switch to retention stack before PDS
 */
__attribute__((naked))
void lp_demo_sleep(uint32_t sleep_ms, uint32_t cnt)
{
    (void)sleep_ms;
    (void)cnt;
    __asm__ volatile(
        /* save sleep_ms (a0) and cnt (a1) before switching stack */
        "la   t1, s_lp_demo_sleep_ms\n"
        "sw   a0, 0(t1)\n"
        "la   t1, s_lp_demo_sleep_cnt\n"
        "sw   a1, 0(t1)\n"
        "mv   t0, sp\n"
        "la   sp, s_pds_stack\n"
        "li   t1, %0\n"
        "add  sp, sp, t1\n"
        "addi sp, sp, -8\n"
        "sw   t0, 0(sp)\n"
        "sw   ra, 4(sp)\n"
        "call lp_demo_sleep_impl\n"
        "lw   ra, 4(sp)\n"
        "lw   t0, 0(sp)\n"
        "mv   sp, t0\n"
        "ret\n"
        :
        : "i"(PDS_STACK_SIZE)
    );
}

/*
 * Stage the embedded lp_fw image into its retained-RAM run address.
 *
 * Copies the blob to the reserved OCRAM slot (0x11048000) and cleans the range
 * so it reaches physical RAM: the HBN stub enters lp_fw after a PDS wake with a
 * cold, power-cycled D-cache, so the bytes must already be committed to RAM.
 */
static void lpfw_load(void)
{
    uint32_t size = (uint32_t)(__lpfw_end - __lpfw_start);

    memcpy((void *)LPFW_RUN_ADDR, __lpfw_start, size);
    /* The blob is freshly-written *code*: clean the D-cache so the bytes reach
     * physical RAM, then fence.i to invalidate the I-cache and sync the
     * instruction stream -- without this the core may fetch stale/garbage when
     * it jumps to 0x11048000 (RISC-V does not guarantee I/D coherence). */
    L1C_DCACHE_CLEAN_INVALID_RANGE(LPFW_RUN_ADDR, size);
    L1C_ICache_Invalid_All();
    __asm__ volatile("fence.i" ::: "memory");

    printf("[LP] lp_fw image staged: %lu bytes -> 0x%08lx\r\n",
           (unsigned long)size, (unsigned long)LPFW_RUN_ADDR);
}

/*
 * Stage-1 wake hook override (strong; overrides the weak default in
 * hal_lpfw_main.c). Runs from TCM on the PDS wake path after early init but
 * before the Stage-2 resume. Enters the staged lp_fw image at its run address:
 * lp_fw is self-contained (installs its own gp/sp, no flash XIP needed) and
 * returns the wake decision in a0.
 *
 * DTIM simulation (Phase 1): on SLEEP, re-arm the RTC and re-enter PDS here
 * WITHOUT resuming the full firmware — the full fw's saved context stays
 * untouched and only lp_fw cycles. pm_pds_resleep() never returns; the next PDS
 * wake re-enters hal_lp_main -> this hook. On WAKE we return the decision and
 * hal_lp_main completes the Stage-2 resume as usual.
 */
int ATTR_TCM_SECTION hal_lp_stage1_hook(void)
{
    int decision = ((int (*)(void))(uintptr_t)LPFW_RUN_ADDR)();

    if (decision == LP_FW_DECISION_SLEEP) {
        pm_pds_resleep((uint32_t)HAL_LP_US_TO_RTC_CNT(IOT2LP_PARA->resleep_us));
        /* not reached */
    }
    return decision;
}

/*
 * PUBLIC API
 */

void lp_demo_app_init(void)
{
    s_initialized = false;
    s_auto_enabled = false;
    s_total_ms = 0;
    s_sleep_count = 0;
    s_last_ms = 0;
    s_last_wakeup = 0;

    /* Detect LP fastboot wakeup — HBN RAM context survived cold boot */
    if (hal_lp_is_fastboot_wakeup()) {
        const volatile hal_hbnram_lp_wake_ctx_t *ctx = hal_lp_get_wake_context();
        printf("[LP] *** FASTBOOT WAKE #%lu (reason=0x%lx) ***\r\n",
               (unsigned long)ctx->wake_counter,
               (unsigned long)ctx->wake_reason);
    }

    /* Bring up the Low Power framework */
    int ret = hal_lp_init();
    if (ret != 0) {
        printf("[LP] hal_lp_init failed: %d\r\n", ret);
        return;
    }

    /* Stage the embedded lp_fw Stage-1 image into retained OCRAM and register
     * the post-wake peripheral re-init hook. */
    lpfw_load();
    hal_lp_register_wakeup_hook(lp_wakeup_hook, NULL);

    /* HBN-RAM retention probe: print the 'lp hbn' test scratch (0x20090E00) on
     * every boot. HBN cold-boots through BROM->boot2->FW, so this runs after a
     * hibernate wake: from HBN level 0 the magic survives (RETAINED); from level
     * 1 the AON HBN core is gated so it is gone (LOST). Garbage on a fresh boot
     * is normal (no stamp yet). */
    {
        volatile uint32_t *hbnram = (volatile uint32_t *)0x20090E00u;
        uint32_t magic = hbnram[0], level = hbnram[1], seq = hbnram[2];
        printf("[LP] HBN-RAM probe @0x20090E00: magic=0x%08lx level=%lu seq=%lu %s\r\n",
               (unsigned long)magic, (unsigned long)level, (unsigned long)seq,
               (magic == 0x48424E00u) ? "(RETAINED)" : "(no stamp / LOST)");
    }

    s_initialized = true;
    printf("[LP] demo init done — 'lp help' for commands\r\n");
}

void lp_demo_idle(void)
{
    if (s_auto_enabled) {
        /* Sleep 1 second per idle cycle — RTC wakeup (0 => default DTIM cycles) */
        lp_demo_sleep(1000, 0);
    }
}

void lp_demo_auto_enable(bool enable)
{
    s_auto_enabled = enable;
    printf("[LP] Auto PDS %s\r\n", enable ? "enabled" : "disabled");
}

bool lp_demo_auto_is_enabled(void)
{
    return s_auto_enabled;
}

void lp_demo_get_stats(uint32_t *total_sleep_ms,
                       uint32_t *sleep_count,
                       uint32_t *last_sleep_ms,
                       int32_t *last_wakeup_reason)
{
    if (total_sleep_ms)   *total_sleep_ms   = s_total_ms;
    if (sleep_count)      *sleep_count      = s_sleep_count;
    if (last_sleep_ms)    *last_sleep_ms    = s_last_ms;
    if (last_wakeup_reason) *last_wakeup_reason = s_last_wakeup;
}

void lp_demo_reset_stats(void)
{
    s_total_ms = 0;
    s_sleep_count = 0;
    s_last_ms = 0;
    s_last_wakeup = 0;
}
