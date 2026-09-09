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

/* PDS phase, printed by the exception dump (bsp .../interrupt.c) so a fault
 * names the stage it happened in rather than leaving it to be guessed from a
 * register dump. Outside the WiFi guards: the PDS path exists in every
 * config. */
extern volatile const char *g_lp_phase;

/*
 * EMBEDDED lp_fw STAGE-1 IMAGE
 *
 * The standalone lp_fw image (apps/platform/ipro6_lpfw) is built separately
 * and embedded here via lpfw_blob.S. It must be memcpy'd to its run address at
 * boot so the HBN stub can enter it after a PDS wake.
 *
 * The run address is in the reserved top 40K of OCRAM (0x11046000), NOT WRAM:
 * OCRAM is retained across PDS31 by the HW default, whereas WRAM is in the WiFi
 * power domain (gated off during PDS) so its contents are lost on wake.
 */
extern const uint8_t __lpfw_start[];
extern const uint8_t __lpfw_end[];

#define LPFW_RUN_ADDR   0x11046000u     /* lp_fw VMA — MUST equal ipro6_lpfw.ld ORIGIN
                                         * and the ram_tcm carve end (RAM_TCM_SIZE 280K) */

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
/* Retention-stack sizing (cont26 hang root cause): lp_demo_sleep_impl's
 * printf chain (newlib vfprintf ~1 KB) plus an interrupt context save
 * (~256 B incl. FPU regs, pushed onto the CURRENT stack) peaked within
 * ~40 B of the old 2 KB — P3.2's slightly larger impl frame tipped it over
 * whenever a WiFi IRQ landed mid-printf (flaky, associated-only). Overflow
 * runs straight into shell_exec_argv/shell_task_tcb below the array =
 * dead shell + wild scheduler. 4 KB restores real margin. */
#define PDS_STACK_SIZE      4096

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
 * LP (DTIM) PARAMETER POPULATION — seed iot2lp_para from the connected STA.
 *
 * The lp_fw blob reads this block on every PDS wake to warm-restore the RF on
 * the AP's channel, admit only the AP's beacon (BSSID filter), and test our
 * AID's TIM bit. In deployment the full firmware fills it from the live STA
 * association context before it ever enters PDS — replacing the manual `lp mw`
 * seeding used during blob bring-up.
 */
#ifdef CONFIG_WIFI6
/* macsw export (components/wireless/macsw/include/export/export_func.c):
 * snapshot the associated-STA context. Declared extern rather than including
 * export_macsw.h, which drags in the heavy UMAC/enum headers. */
extern int mac_get_connected_sta_ctx(uint8_t bssid[6], uint8_t local_mac[6],
                                     uint8_t *ap_channel, uint16_t *aid,
                                     uint16_t *bcn_int_tu);

/* fhost STA power-save handshake (components/wireless/wifi6/ipro_fhost/src/
 * application/wifi_mgmr_ext.c). ps_enter sends a null-data frame with PM=1 so
 * the AP starts buffering our unicast and sets our AID's TIM bit; ps_exit sends
 * PM=0 to pull the buffered frames and mark the STA active again. Both block on
 * a cross-task cfgrwnx round-trip. Declared extern to avoid pulling
 * wifi_mgmr_ext.h's heavy dependency chain (same reason as above). */
extern int wifi_mgmr_sta_ps_enter_static(void);
extern int wifi_mgmr_sta_ps_exit(void);

#ifdef CONFIG_IPRO_WIFI_PS
/* macsw WiFi datapath resume across PDS (components/wireless/macsw/ip/lmac/src/
 * rwnx/rwnx.c, gated CFG_IPRO_WIFI_PS_ENABLE). pds_store + configs_restore snapshot
 * the on-chip key RAM (CCMP PTK/GTK) + operating channel before PDS; resume_wifi
 * re-programs the full data-operational MAC HW after a WIFI-wake. Declared extern to
 * avoid pulling macsw's internal rwnx.h (which is gated + uses macsw-private paths). */
extern int  rwnxl_pds_store(void);
extern int  rwnxl_configs_restore(void);
extern void rwnxl_resume_wifi(void);
/* PDS power-off duration in us, consumed by rwnxl_restore_mactimer() to put
 * MAC time back where it would have been. The OS tick cannot supply it here:
 * the resume runs before vTaskStepTick() catches the tick up. */
extern uint32_t rwnxl_pds_slept_us;
extern void rtos_wifi_task_freeze(void);
extern int  rwnxl_me_is_idle(void);
extern void rwnxl_pds_gate_sample(void);
extern uint32_t rwnxl_ps_sleep_reason(void);
/* PDS-entry TX gate accounting. Inside the WiFi-PS guard: the No-WiFi config has
 * no PDS entry to gate and -Werror=unused-variable rejects a stray static. */
volatile uint32_t s_pds_tx_busy;       /* entries that hit the gate             */
volatile uint32_t s_pds_tx_forced;     /* deadline expired: powered down anyway */
static uint32_t   s_pds_tx_wait_us;    /* machw stamp of the first refusal, 0 = none */
volatile uint32_t s_pds_tx_maxheld_us; /* longest wait that CLEARED on its own --
                                        * tells a slow confirm from a dead one  */
static uint8_t    s_pds_tx_stuck_said; /* one complaint per stuck episode       */
volatile uint32_t s_pds_prevent_calls; /* gate passes sampled                   */
volatile uint32_t s_pds_prevent_hit;   /* ...with ps_env.prevent_sleep != 0      */
volatile uint32_t s_pds_prevent_bits;  /* union of the bits ever seen set        */
#define LP_PDS_STUCK_US 2000000u       /* 2 s with no confirm = say so; still
                                        * refuse, never power down on it        */

/* Deadline on the wait. The reference gates UNBOUNDED and can afford to: on bl616
 * pck_cnt drains reliably. Here it does not always -- measured climbing 4, 8 and
 * not returning to zero in one run -- and an unbounded gate then stops PDS FOREVER
 * (bench: s_pds_entries == 0 cycle after cycle, refusals +56k/cycle). So bound it,
 * and make the bound TIME: an earlier version counted idle passes, which is no
 * bound at all because the hook is re-entered back-to-back and eight passes can
 * elapse in under a millisecond while a confirm needs milliseconds.
 *
 * 20 ms measured: 3% of gated entries expire (vs 10.5% at the pass count), PDS
 * entries 269/228 per cycle against a ~220 baseline, wake latency 0.9-2.0 s against
 * a 0.9-5.5 s baseline. When it expires the cost is one lost frame -- exactly the
 * ungated behaviour -- so the fallback is never worse than not gating.
 *
 * 2026-07-31: RAISED 20 ms -> 200 ms. The 20 ms figure came from the worry above,
 * not from the confirm-latency distribution, and it left 3.8% of gated entries
 * expiring (709 of 18853 over a 68-cycle run, ~29 forced per standby). Each
 * expiry powers down on a frame the MAC still owns, and that frame's pbuf ref is
 * never dropped -- which pins lwIP's retransmit forever (tcp_output_segment_busy
 * -> tcp_rexmit_rto_prepare returns ERR_VAL; measured uref=2 ubusy=1 on 188 of
 * 188 wedged keepalives). So an expiry is NOT "one lost frame" as first written:
 * it can permanently mute the socket. Buy the tail instead.
 *
 * The bound is NOT optional, and the reason is worth stating exactly because it
 * was doubted and then re-proved. Removing it entirely was tried on this build:
 * PDS entries per standby went 224, then 0 for every cycle after, with the TX
 * bit set on 634705 of 634729 gate samples and pckmax pinned at 4. There is a
 * frame that never confirms -- and it is not a PDS artifact, since that run
 * took zero power-downs after the first. An unbounded gate therefore waits on
 * something that will never arrive. (An earlier note here dismissed this
 * evidence as an artifact of the flush build; that dismissal was wrong. The
 * "busy is only 15% of idle passes" figure that motivated it was measured WITH
 * a bounded gate, i.e. inside dynamics the gate itself creates.)
 *
 * 2026-07-31 later: 200 ms -> 5 ms, and for a changed REASON rather than a
 * changed number. Once the resume path re-arms the surviving descriptors
 * (txl_cntrl_ps_rearm) instead of discarding them, a forced power-down is no
 * longer destructive -- measured 392 forced entries across 3 cycles with a
 * heap slope of ZERO, where the discarding build leaked ~2.5 KB per cycle. So
 * the gate stops being a barrier that must hold at any cost and becomes cheap
 * opportunism: take the confirm if it is about to arrive, otherwise sleep.
 *
 * At 200 ms it had turned into pure waste. A frame that never confirms (there
 * is one: pck_cnt sits at 6-7 with site[1] running 4 ahead of the confirms,
 * awake, with zero PDS entries) makes EVERY entry pay the full deadline, and
 * PDS entries per standby fell 179 -> 161 -> 98 against a ~220 baseline.
 * Waiting only pays when the wait can succeed.
 *
 * 5 ms is read off the distribution, not guessed: with the re-arm in place the
 * longest wait that CLEARED on its own is 1.41 ms (s_pds_tx_maxheld_us), so
 * 5 ms covers every recoverable case and wastes at most 5 ms on a stuck one. */
#define LP_PDS_GATE_US  5000u

/* 1 us MAC monotonic counter (hal_machw_time). Readable with interrupts masked and
 * the scheduler suspended, unlike xTaskGetTickCount on this path, and the MAC is
 * still powered here -- we have not slept yet. */
extern uint32_t inline_hal_machw_time(void);
extern uint32_t rwnxl_ps_prevent_sleep(void);

volatile uint32_t g_lp_tx_cfm_drain_pending;  /* kept: still read by the app */
/* RWNXL_NOSLEEP_* bits from macsw's internal rwnx.h, mirrored here for the same
 * reason as the externs above.
 *
 * The MASK is the bl616 reference's gate, not a set invented here. Its tickless
 * checks WiFi twice before committing to PDS -- rwnxl_ps_sleep_common_check() then
 * rwnxl_ps_sleep_check() -- and the FIRST of those is exactly ke_evt + queue_sent
 * + queue_saved + txl_sleep_check(), i.e. everything EXCEPT the PS bit. That
 * exclusion is what makes it usable: measured over ~220 entries per standby cycle,
 * ps is true 60% of the time (normal for a DTIM STA, so gating on it starves PDS
 * outright) while tx is 1-3% and kevt/qsent/qsaved are 0. TIMER is included because
 * the reference's connected check has it and it measured 0-2 per cycle -- free.
 *
 * The invariant is "do not remove power from a MAC that is mid-transaction". It is
 * NOT an argument about whether the in-flight frame matters: an earlier revert here
 * reasoned that the frame is usually an idempotent gratuitous ARP and so not worth
 * waiting for, which confused "the frame is expendable" with "the MAC may be cut
 * off mid-transaction". The buffer that transaction owns is what leaks. */
#define LP_NOSLEEP_PS     (1u << 0)
#define LP_NOSLEEP_TX     (1u << 1)
#define LP_NOSLEEP_TIMER  (1u << 2)
#define LP_NOSLEEP_KEVT   (1u << 3)
#define LP_NOSLEEP_QSENT  (1u << 4)
#define LP_NOSLEEP_QSAVED (1u << 5)
#define LP_NOSLEEP_MASK   (LP_NOSLEEP_TX | LP_NOSLEEP_TIMER | LP_NOSLEEP_KEVT | \
                           LP_NOSLEEP_QSENT | LP_NOSLEEP_QSAVED)
/* WiFi MAC HW ISR (macsw intc) — the app owns its WIFI_IRQn wiring on IPRO6
 * (see hal_wifi_start_firmware_task in main.c). */
extern void interrupt0_handler(void);
#endif
#endif

/* Settle margin after ps_enter so the AP's last-known state for us is PM=1
 * before PDS powers the MAC off. ps_enter already blocks until the null-frame
 * is TX'd + ACK'd, so this is pure margin (< half a 100 TU beacon interval). */
#define LP_PS_SETTLE_MS     30u

int lpfw_populate_from_sta(void)
{
#ifdef CONFIG_WIFI6
    volatile iot2lp_para_t *para = IOT2LP_PARA;
    uint8_t  bssid[6], local_mac[6], ch = 0;
    uint16_t aid = 0, bcn_tu = 0;

    if (mac_get_connected_sta_ctx(bssid, local_mac, &ch, &aid, &bcn_tu) != 0) {
        printf("[LP] populate: not associated -- keeping seeded values\r\n");
        return -1;
    }

    /* P3.2: a different AP means a different TBTT phase — invalidate the
     * beacon-tracking anchor + delay window. Same-BSSID re-populates (every
     * `lp sleep`, tickless re-arm) keep the warm tracking state; the RC32K
     * ppm estimate is a board property and survives either way. */
    for (int i = 0; i < 6; i++) {
        if (para->bssid[i] != bssid[i]) {
            para->last_beacon_stamp_rtc_valid  = 0;
            para->bcn_delay_sliding_win_status = 0;
            para->bcn_delay_sliding_win_point  = 0;
            para->last_beacon_delay_us         = 0;
            para->ppm_stable_cnt               = 0;
            break;
        }
    }
    para->bcn_delay_sliding_win_size = LP_FW_BCN_DELAY_WIN_MAX;

    for (int i = 0; i < 6; i++) {
        para->bssid[i]     = bssid[i];
        para->local_mac[i] = local_mac[i];
    }
    para->ap_channel          = ch;
    para->aid                 = aid;
    para->beacon_interval_tu  = bcn_tu;
    para->continuous_loss_cnt = 0;          /* fresh standby = fresh loss streak (P3.3-lite) */
    para->dtim_num            = 1;          /* live DTIM count comes from the beacon TIM; 1 = wake-every-DTIM */
    para->beacon_data_rate    = 2;          /* 1 Mbit CCK: find_tim divide-guard (interval_start=0 bypasses the time gate) */
    para->bcn_target_level    = 0;          /* LP RSSI target: 0 = driver default */
    para->ram_wifi_start_addr = 0x21050000u;/* IPRO6 WRAM: MAC RX-DMA ring base the blob parses */

    printf("[LP] populate: BSSID %02x:%02x:%02x:%02x:%02x:%02x ch%u AID%u bcn%uTU\r\n",
           bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
           (unsigned)ch, (unsigned)aid, (unsigned)bcn_tu);
    return 0;
#else
    printf("[LP] populate: build has no WiFi (CONFIG_WIFI6=n) -- use `lp mw`\r\n");
    return -1;
#endif
}

/*
 * PDS sleep implementation — runs on retention stack
 */
/* Set while entering PDS from the tickless idle hook: quiets the per-entry
 * prints (idle re-enters every few hundred ms; the UART would drown) except
 * for WIFI wakes, which are the events worth logging. */
static volatile bool s_from_idle = false;

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

    /* NOTE: iot2lp_para seeding (lpfw_populate_from_sta) and the WiFi PS-enter
     * handshake now run in lp_demo_sleep() BEFORE the retention-stack switch —
     * they block on cross-task cfgrwnx IPC, which is unsafe on this tiny stack
     * with the scheduler about to be frozen. By the time we get here the params
     * are already populated and the AP has been told we are entering power-save. */

    /* DTIM simulation: arm lp_fw to re-sleep this same interval for `cycles`
     * beacons before waking the full firmware (cycles from `lp sleep <ms> <cnt>`,
     * default LP_FW_SIM_DTIM_CYCLES). The counter lives in always-on HBN RAM so it
     * survives every PDS cycle; reset it here so each `lp sleep` starts fresh. */
    volatile iot2lp_para_t *para = IOT2LP_PARA;
    para->resleep_us = (uint32_t)cfg.rtc_sleep_us;
    para->sim_dtim_count = 0;
    para->sim_dtim_target = cycles;

    /* P3.2: align the FIRST PDS interval to the next TBTT from the retained
     * beacon anchor (lp_fw.h math; no-op while `lp track` is off or no valid
     * anchor exists). Matters most on the tickless path: expected idle is
     * ≤250 ticks, so most blob cycles are the first cycle of an entry.
     * resleep_us above stays the NOMINAL interval — it is only the fallback;
     * the blob recomputes it per cycle when tracking is on. */
    {
        uint32_t lo = 0, hi = 0;
        HBN_Get_RTC_Timer_Val(&lo, &hi);
        uint64_t now_us = ((((uint64_t)hi << 32) | lo) * 15625u) >> 9;
        cfg.rtc_sleep_us = lp_fw_track_next_sleep_us(para, now_us,
                                                     (uint32_t)cfg.rtc_sleep_us);
    }

    hal_lp_result_t result;
    memset(&result, 0, sizeof(result));

    if (!s_from_idle) {
        printf("[LP] PDS sleep: %lu ms x%lu DTIM cycles (rtc_us=%llu, flash_pd=%d, ocram_ret=%d)\r\n",
               (unsigned long)sleep_ms,
               (unsigned long)cycles,
               (unsigned long long)cfg.rtc_sleep_us,
               cfg.flash_power_down,
               cfg.ocram_retention);
    }
    uart_flush();

    g_lp_phase = "pds-off";
    int ret = hal_lp_enter(&cfg, &result);
    g_lp_phase = "pds-back";
    (void)ret;

    /* === WAKEUP === */

    /* Compensate FreeRTOS tick counter */
    uint32_t actual_us = (uint32_t)result.sleep_duration_us;
    if (actual_us == 0) actual_us = sleep_ms * 1000;
    uint32_t actual_ms = actual_us / 1000;

    /* Tick compensation moved to lp_demo_sleep() (task path, after the
     * trampoline unmasks interrupts): xTaskCatchUpTicks must not run with
     * the tick masked, and the whole trampoline window is now IRQ-masked
     * (cont26 wild-mret fix — see lp_demo_sleep). The tickless idle hook
     * compensates in vApplicationSleep via vTaskStepTick using s_last_ms. */

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
    if (!s_from_idle || result.wakeup_reason == HAL_LP_WAKEUP_WIFI) {
        printf("[LP] Woke: reason=0x%lx (%s), %lu ms (cnt=%lu)\r\n",
               (unsigned long)result.wakeup_reason, rs,
               (unsigned long)actual_ms,
               (unsigned long)s_sleep_count);
    }

    /* DTIM simulation result: lp_fw should have cycled `cycles` times (so
     * actual_ms ~= cycles * interval) and woken us with a WIFI cause. This is the
     * proof the full firmware stayed asleep through all the cycles. */
    if (!s_from_idle || result.wakeup_reason == HAL_LP_WAKEUP_WIFI) {
        printf("[LP] DTIM sim: lp_fw cycled %lu/%lu, lpfw_reason=0x%lx, aid=0x%04x\r\n",
               (unsigned long)para->sim_dtim_count,
               (unsigned long)cycles,
               (unsigned long)para->wakeup_reason,
               (unsigned)para->aid);
    }
    if (para->wakeup_reason & LP_FW_WAKEUP_AP_LOSS) {
        printf("[LP] AP LOSS: %u consecutive beaconless cycles -- standby ended, "
               "reconnect needed\r\n", (unsigned)LP_FW_LOSS_RECONNECT_CAP);
    }

    /* wake-trace hang catcher: full resume completed — disarm the WDT armed by
     * the wake path (hal_lpfw_main lp_wdt_arm). Debug scaffold. */
    {
        volatile uint32_t *t = (volatile uint32_t *)0x30011000u;
        t[0x9C/4] = 0xBABAu; t[0xA0/4] = 0xEB10u;
        t[0x64/4] = 0x00000000u;
    }
}

/*
 * Naked trampoline — switch to the retention stack, then run lp_demo_sleep_impl
 * (which enters PDS). Wrapped by lp_demo_sleep() below, which does the WiFi
 * PS handshake in task context before/after this call.
 */
__attribute__((naked))
static void lp_demo_pds_trampoline(uint32_t sleep_ms, uint32_t cnt)
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
 * PDS sleep entry (task context).
 *
 * The DTIM wake trigger needs the AP to buffer our unicast and set our AID's
 * TIM bit while we sleep — which only happens if the AP knows we entered
 * power-save. So BEFORE the PDS dance we send a null-data frame with PM=1
 * (wifi_mgmr_sta_ps_enter). On wake we send PM=0 (wifi_mgmr_sta_ps_exit) to pull
 * the buffered frames and mark the STA active so fhost traffic flows again.
 *
 * These wifi_mgmr calls block on a cross-task cfgrwnx round-trip, so they MUST
 * run here in task context on the real task stack. lp_demo_pds_trampoline then
 * switches to the 2 KB retention stack with the scheduler about to be frozen,
 * where blocking would corrupt state — hence the split.
 */
void lp_demo_sleep(uint32_t sleep_ms, uint32_t cnt)
{
#ifdef CONFIG_WIFI6
    bool ps_announced = false;

    /* Only touch PS when we are actually going to sleep (mirror the guards in
     * lp_demo_sleep_impl) — otherwise we would leave the STA parked in PS. */
    if (s_initialized && sleep_ms >= LP_MIN_SLEEP_MS) {
        /* Seed iot2lp_para from the live association; its return value is the
         * authoritative "associated?" gate for the PS handshake. */
        if (lpfw_populate_from_sta() == 0) {
            /* null-data PM=1 (STATIC PS) → AP buffers our unicast + sets our TIM
             * bit and keeps doing so even as pings flow, because static PS never
             * toggles PM back to 0. Blocks until the SET_PS_MODE cfm (TX'd+ACK'd). */
            if (wifi_mgmr_sta_ps_enter_static() == 0) {
                vTaskDelay(pdMS_TO_TICKS(LP_PS_SETTLE_MS));
                ps_announced = true;
                printf("[LP] PS enter: null PM=1 sent, AP buffering\r\n");
#ifdef CONFIG_IPRO_WIFI_PS
                /* Don't carry an in-flight ME<->MM transaction into PDS: its MM cfm
                 * would be destroyed with the MAC and ME would park in ME_BUSY forever
                 * (saving ps_exit's request on wake). Transitions finish in a few ms
                 * (null TX + cfm), so a bounded wait covers it; the wake side also
                 * force-idles ME as the safety net if we still sleep mid-transition. */
                for (int spin = 0; spin < 100 && !rwnxl_me_is_idle(); spin++) {
                    vTaskDelay(pdMS_TO_TICKS(2));
                }

                /* Snapshot the MAC HW state that PDS will wipe: the on-chip key RAM
                 * (CCMP PTK/GTK) + the operating channel. rwnxl_resume_wifi() re-
                 * programs the MAC from these after the WIFI-wake. Done here while the
                 * MAC is still live, just before the retention-stack switch. A failed
                 * snapshot (MAC bus unreadable — doze race) means sleeping would
                 * restore garbage keys: undo the PS announce and skip this sleep. */
                if (rwnxl_pds_store() != 0) {
                    printf("[LP] keyram snapshot failed (MAC unreadable) -- sleep aborted\r\n");
                    wifi_mgmr_sta_ps_exit();
                    return;
                }
                rwnxl_configs_restore();
                /* Freeze the WiFi task until rwnxl_resume_wifi() completes: on wake
                 * the blob has run its own beacon RX over the same ring, so any early
                 * resume (kmsg/timer/ISR notification) would let the task walk stale
                 * RX descriptors — seen as a wild-pointer Load Access Fault in
                 * rxl_cntrl_evt ~4ms after "Woke". */
                rtos_wifi_task_freeze();
#endif
            } else {
                printf("[LP] PS enter FAILED -- sleeping without TIM trigger\r\n");
            }
        }
    }
#endif

    /* cont26c wild-mret fix: the pre-PDS printf/uart_flush inside the trampoline
     * runs on the retention stack; an IRQ landing in vprint's uart_write could
     * return through a corrupted mepc — mret INTO the vprint string buffer
     * (fetch fault, CCI-parked mcause=0x0800000C). The from_idle path never
     * wedged because vApplicationSleep enters with interrupts masked; converge
     * the task path onto that environment by masking across the trampoline.
     * Tick compensation runs after unmasking (xTaskCatchUpTicks must not run
     * with the tick masked). */
    s_last_ms = 0;   /* impl early-returns must not replay a stale catchup */
    portDISABLE_INTERRUPTS();
    lp_demo_pds_trampoline(sleep_ms, cnt);
    portENABLE_INTERRUPTS();
    {
        uint32_t catchup_ticks = s_last_ms * (configTICK_RATE_HZ / 1000u);
        if (catchup_ticks > 0) {
            xTaskCatchUpTicks(catchup_ticks);
        }
    }

#ifdef CONFIG_WIFI6
    /* === WAKE: full firmware resumed here === */
    if (ps_announced) {
#ifdef CONFIG_IPRO_WIFI_PS
        /* Re-program the full data-operational MAC HW from the SW state that survived
         * PDS (STA ctx, CCMP keys via keyram, MAC/BSSID filters, PHY/channel). MUST run
         * before ps_exit so the MAC is actually live to TX the PM=0 null + RX the
         * AP-buffered data — the blob left the MAC in RX-beacon-only warm state, and
         * phy_init alone does not recover the datapath. Also lifts the WiFi-task
         * freeze set before the trampoline. */
        rwnxl_resume_wifi();

        /* PDS powered off the ECLIC: the per-IRQ enable + priority of WIFI_IRQn
         * (armed once by hal_wifi_start_firmware_task at stack_wifi) are gone, so
         * MAC/PHY interrupts never reach the CPU — RX delivery and TX cfm are dead
         * while the ke/mm message flow (SW-driven) still works. Re-arm AFTER the MAC
         * resume: the enable is immediate and WIFI_IRQn is level-triggered, so any
         * interrupt status latched during the blob's tenure fires the macsw ISR the
         * instant this returns — it must land on a fully re-programmed MAC, not the
         * half-restored one (stale-ring processing wedged interrupt0_handler's
         * unbounded status loop / faulted rxl_cntrl_evt). ps_exit below still gets a
         * live ISR for the PM=0 null TX cfm. */
        Interrupt_Handler_Register(WIFI_IRQn, interrupt0_handler);
#endif
        /* null-data PM=0 → AP flushes the frames it buffered while we slept and
         * the STA is active again; fhost datapath continues. */
        if (wifi_mgmr_sta_ps_exit() == 0) {
            printf("[LP] PS exit: null PM=0 sent, fhost resumed\r\n");
        } else {
            printf("[LP] PS exit FAILED -- WiFi datapath may need re-init\r\n");
        }
    }
#endif
}

#if defined(CONFIG_WIFI6) && defined(CONFIG_IPRO_WIFI_PS)
/*
 * PDS sleep entry for the FreeRTOS tickless idle hook (lp_tickless.c
 * vApplicationSleep). Runs in the IDLE task with the scheduler SUSPENDED:
 * nothing here may block. The blocking pieces of lp_demo_sleep() are hoisted
 * to the tickless ARM step — iot2lp populate + the static PM=1 announce run
 * ONCE there, so the AP keeps buffering across every idle-entered PDS cycle.
 * This variant runs only the non-blocking pre-sleep gates, the PDS trampoline
 * (which tick-compensates via xTaskCatchUpTicks — suspension-safe, it pends
 * the ticks) and the register-level wake resume. It deliberately does NOT
 * ps_exit: dataless wakes fall straight back to idle and re-enter PDS; only
 * the tickless service task exits PS when a WIFI wake has data to deliver.
 *
 * Returns the HAL_LP_WAKEUP_* reason, or -1 if the sleep was aborted (ME
 * transaction in flight — the WiFi task cannot advance it while the
 * scheduler is suspended, so waiting here would be futile; the caller just
 * returns and the next idle pass retries).
 */
int lp_demo_sleep_from_idle(uint32_t dtim_ms, uint32_t cycles, bool ps_active,
                            uint32_t *slept_ms)
{
    if (slept_ms) {
        *slept_ms = 0;
    }
    if (!s_initialized || dtim_ms < LP_MIN_SLEEP_MS) {
        return -1;
    }
    /* The MAC snapshot + wake resume are the PS-ASSOCIATED dance — mirror
     * lp_demo_sleep's ps_announced gate exactly. Running rwnxl_resume_wifi
     * on a never-PS-entered MAC (seed plumbing mode) faults the carrier. */
    if (ps_active) {
        /* An in-flight ME<->MM transaction's cfm would be destroyed by PDS
         * and park ME_BUSY forever (cont20 Bug Z). Unlike lp_demo_sleep()'s
         * bounded wait, ABORT here: the WiFi task that would finish the
         * transaction cannot run until the scheduler resumes. */
        if (!rwnxl_me_is_idle()) {
            return -1;
        }
        /* Record how often this power-down would have been refused by macsw's
         * own doze conditions (PS state, TX path drained, no imminent MAC
         * timer), then act on the ONE that costs us memory.
         *
         * An earlier attempt armed the whole check and had to be reverted for
         * blocking PDS outright; the note blamed the TX-path condition, and the
         * counters say that was wrong. Measured on 028HXHKN 2026-07-30 over
         * ~220 entries per standby cycle: ps=60%, tx=1%, everything else ~0. It
         * is the PS condition that blocks PDS, and PS being "not sleepable" is
         * normal for a DTIM STA -- gating on it can never work here.
         *
         * The TX condition is different, and it is not free to ignore: entering
         * PDS with packets still in the TX path loses them, and the buffer's
         * reference (net_if_output pbuf_ref's before fhost_tx_start, released
         * only on a TX confirm that now never comes) is never dropped. The
         * lwIP-heap live-allocation trace pinned it exactly -- surviving
         * etharp_raw pbufs equalled TX-busy PDS entries 1:1 across cycles
         * (1==1, 3==3, 5==5, 512 B each), which at ~2/cycle walks the 48 KB heap
         * to ERR_MEM in ~25 min and then no MAC-level recovery can clear it.
         * bl616 does the same thing for the same reason (its tickless
         * __enter_connected: "if (rwnxl_ps_sleep_check() != 0) { __WFI();
         * return; }").
         *
         * Refusing costs one idle pass: vApplicationSleep treats <0 as "aborted,
         * next idle pass retries", and at 1% of entries the DTIM cycle is
         * unaffected. */
        rwnxl_pds_gate_sample();
        /* Do not remove power from a MAC that is mid-transaction.
         *
         * This is the bl616 reference's contract: its tickless refuses the
         * connected power-down while WiFi is not quiescent and parks in __WFI()
         * instead (bl616_demo_sdiowifi/tickless.c -- rwnxl_ps_sleep_common_check()
         * at the top and rwnxl_ps_sleep_check() in the connected branch). Ours only
         * SAMPLED the same predicates and powered down regardless, and the cost is
         * measured: a frame destroyed by the power-down never confirms, so the
         * reference net_if_output took with pbuf_ref before fhost_tx_start is never
         * dropped and one lwIP heap block is pinned forever -- stranded blocks
         * equalled these events 1:1 across cycles, and at ~2/cycle that walks the
         * 48 KB heap to permanent ERR_MEM in ~25 min.
         *
         * LP_NOSLEEP_MASK deliberately omits the PS bit (see its definition): only
         * the reference's first-layer subset is usable here. Releasing the buffers
         * instead of waiting was also tried and is reverted -- txl_cntrl_flush_all()
         * left them held and made pck_cnt grow, because the frame is in neither
         * list it walks. */
        /* MEASURE BEFORE GATING (the mistake made twice already today), and on
         * EVERY pass, not only the TX-busy ones -- the question is what share
         * of power-downs prevent_sleep would block if it joined the mask.
         *
         * LP_NOSLEEP_MASK drops the PS bit because ps_sleep_check() is true
         * ~60% of the time, but that check ORs "PS is not on" (ordinary for a
         * DTIM STA) with ps_env.prevent_sleep, which is a set of genuinely
         * un-interruptible procedures -- and IDLE_REQ_PENDING among them means
         * the MAC is transitioning to HW_IDLE with a 50 ms watchdog armed in a
         * HW register PDS is about to destroy. If prevent_sleep turns out to be
         * rare, it belongs in the mask; if it is as common as the whole check,
         * adding it would starve PDS exactly as the unbounded gate did. */
        {
            uint32_t pv = rwnxl_ps_prevent_sleep();

            s_pds_prevent_calls++;
            if (pv != 0u) {
                s_pds_prevent_hit++;
                s_pds_prevent_bits |= pv;
            }
        }
        if (rwnxl_ps_sleep_reason() & LP_NOSLEEP_MASK) {
            uint32_t now = inline_hal_machw_time();

            s_pds_tx_busy++;
            if (s_pds_tx_wait_us == 0u) {
                s_pds_tx_wait_us = now;
                /* 0 doubles as "no wait pending", so a stamp landing there must not
                 * read as unarmed. */
                if (s_pds_tx_wait_us == 0u) {
                    s_pds_tx_wait_us = 1u;
                }
            }
            /* NO DEADLINE. A pending descriptor means the MAC is preparing to
             * transmit, and there is no length of time after which powering down
             * on it becomes correct -- the frame is not late, it is scheduled.
             * Every deadline tried here was the same mistake at a different
             * scale: 8 idle passes, then 20 ms, then 200 ms. The confirm-latency
             * tail did not converge (54.9 -> 59.1 -> 92.0 ms as sampling grew),
             * so no threshold exists that is both safe and finite.
             *
             * What the expiry cost was not "one lost frame". PDS destroys the
             * MAC-side state while the pbuf and its driver reference survive in
             * retained RAM, so the reference is never dropped, and lwIP then
             * refuses to retransmit that segment FOREVER
             * (tcp_output_segment_busy() is seg->p->ref > 1, which makes
             * tcp_rexmit_rto_prepare() return ERR_VAL every slow tick). The
             * socket goes permanently mute while still reading ESTABLISHED with
             * send() succeeding. Measured on 192 of 192 wedged keepalives.
             *
             * If a frame genuinely never confirms, this gate now blocks PDS and
             * says so below rather than hiding it by destroying the frame. That
             * is a bug to fix at its own layer, not something to paper over
             * here. */
            if ((uint32_t)(now - s_pds_tx_wait_us) >= LP_PDS_STUCK_US &&
                !s_pds_tx_stuck_said) {
                s_pds_tx_stuck_said = 1u;
                printf("[LP-TL] PDS held %lu ms by a TX that has not "
                       "confirmed\r\n",
                       (unsigned long)((now - s_pds_tx_wait_us) / 1000u));
            }
            if ((uint32_t)(now - s_pds_tx_wait_us) < LP_PDS_GATE_US) {
                /* -2, not -1: the caller must PARK, not retry immediately. A
                 * bare return spins the idle task at ~47k samples/s because
                 * nothing waits for the confirm that clears the condition. The
                 * reference does __WFI() here for the same reason. */
                return -2;
            }
            s_pds_tx_forced++;
            s_pds_tx_wait_us = 0u;
        }
        else {
            /* The wait ended on its own -- one sample of how long a confirm
             * actually takes. Kept now that the gate no longer has a deadline to
             * size: it is how a never-confirming frame would be told apart from
             * a merely slow one. */
            s_pds_tx_stuck_said = 0u;
            if (s_pds_tx_wait_us != 0u) {
                uint32_t held = (uint32_t)(inline_hal_machw_time() -
                                           s_pds_tx_wait_us);

                if (held > s_pds_tx_maxheld_us) {
                    s_pds_tx_maxheld_us = held;
                }
            }
            s_pds_tx_wait_us = 0u;  /* quiescent: any wait is over */
        }
        /* Aborted snapshot (MAC bus unreadable — doze race): PDS on a
         * garbage snapshot restores garbage keys and the unclamped readback
         * once rampaged the ke heap. Retry on the next idle pass. */
        if (rwnxl_pds_store() != 0) {
            return -1;
        }
        rwnxl_configs_restore();
        rtos_wifi_task_freeze();
    }

    s_from_idle = true;
    lp_demo_pds_trampoline(dtim_ms, cycles);
    s_from_idle = false;

    /* Re-arm the FreeRTOS tick timer (CLINT mtimecmp) — bl616's wifi_lp_exit
     * does the same on every wake. The task-context path self-heals off a
     * pending MTIP at snapshot time, but this path snapshots with interrupts
     * masked, so a stale mtimecmp would leave the tick dead forever (tasks
     * block on never-expiring timeouts; observed as a silent time-frozen
     * system). Designed to be called with interrupts disabled. */
    {
        extern void vPortSetupTimerInterrupt(void);
        vPortSetupTimerInterrupt();
    }

    if (ps_active) {
        /* Register-level MAC datapath resume + ECLIC re-arm (same ordering
         * contract as lp_demo_sleep: IRQ enable only lands on the fully
         * re-programmed MAC). Lifts the WiFi-task freeze. rsm markers are
         * silenced on this ~3/s path — their printf volume races the PDS
         * UART deinit/reinit (UART_SendData dead-FIFO spin observed). */
        extern int rwnxl_rsm_quiet;
        rwnxl_pds_slept_us = s_last_ms * 1000u;
        rwnxl_rsm_quiet = 1;
        rwnxl_resume_wifi();
        rwnxl_rsm_quiet = 0;
        Interrupt_Handler_Register(WIFI_IRQn, interrupt0_handler);
    }

    if (slept_ms) {
        *slept_ms = s_last_ms;      /* actual slept time, for vTaskStepTick */
    }
    return (int)s_last_wakeup;
}
#endif /* CONFIG_WIFI6 && CONFIG_IPRO_WIFI_PS */

/*
 * Stage the embedded lp_fw image into its retained-RAM run address.
 *
 * Copies the blob to the reserved OCRAM slot (0x11046000) and cleans the range
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
     * it jumps to the slot (RISC-V does not guarantee I/D coherence). */
    L1C_DCACHE_CLEAN_INVALID_RANGE(LPFW_RUN_ADDR, size);
    L1C_ICache_Invalid_All();
    __asm__ volatile("fence.i" ::: "memory");

    /* Stash the bootrom PDS-wake scratch window (lp_fw.h contract): the
     * mask-ROM wake path tramples 0x1104B438-B43F on every PDS wake; the
     * blob restores the window from this stash at entry (startup.S). Read
     * via the uncached alias AFTER the clean so the stash holds the exact
     * staged bytes. */
    memcpy((void *)LPFW_BROM_STASH_ADDR,
           (const void *)LPFW_BROM_SCRATCH_ADDR, LPFW_BROM_SCRATCH_SIZE);

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
/* wake-trace ring (retained OCRAM, see lp_fw.c lpdbg_putc) — debug scaffold */
static inline __attribute__((always_inline)) void lp_trace_putc(char c)
{
    volatile unsigned int *hdr = (volatile unsigned int *)0x2104FF00u;
    volatile unsigned char *buf = (volatile unsigned char *)(0x2104FF00u + 8u);
    if (hdr[0] != 0x54524143u) { hdr[0] = 0x54524143u; hdr[1] = 0u; }
    buf[hdr[1] % 200u] = (unsigned char)c;
    hdr[1]++;
}

int ATTR_TCM_SECTION hal_lp_stage1_hook(void)
{
    lp_trace_putc('H');   /* wake-trace: entering staged blob */
    int decision = ((int (*)(void))(uintptr_t)LPFW_RUN_ADDR)();

    if (decision == LP_FW_DECISION_SLEEP) {
        lp_trace_putc('S');   /* wake-trace: re-entering PDS */
        /* P3.3: the blob's lp_loss_decide already baked graded early-wake
         * (front phase) and listen-stretch (back phase) into resleep_us, so
         * the carrier just re-arms PDS for exactly that interval. */
        pm_pds_resleep((uint32_t)HAL_LP_US_TO_RTC_CNT(IOT2LP_PARA->resleep_us));
        /* not reached */
    }
    lp_trace_putc('W');   /* wake-trace: waking full FW */
    return decision;
}

/*
 * PUBLIC API
 */

void lp_demo_app_init(void)
{
    /* wake-trace hang catcher: the WDT config survives its own reset, so a
     * captured hang otherwise boot-loops every ~30 s. Disarm at every cold
     * boot. Debug scaffold. */
    {
        volatile uint32_t *t = (volatile uint32_t *)0x30011000u;
        t[0x9C/4] = 0xBABAu; t[0xA0/4] = 0xEB10u;
        t[0x64/4] = 0x00000000u;
    }

    /* P3.2: HBN RAM holds POR garbage and iot2lp fields are only written
     * piecemeal — every field the blob READS needs a cold-boot owner. The
     * tracking switch + ppm estimate start clean each power session
     * (`lp track <us>` re-arms; the ppm re-converges in ~8 beacons). */
    {
        volatile iot2lp_para_t *para = IOT2LP_PARA;
        para->rx_lead_us                   = 0;
        para->rtc32k_error_ppm             = 0;
        para->last_beacon_stamp_rtc_valid  = 0;
        para->bcn_delay_sliding_win_status = 0;
        para->bcn_delay_sliding_win_point  = 0;
        para->bcn_delay_sliding_win_size   = LP_FW_BCN_DELAY_WIN_MAX;
        para->last_beacon_delay_us         = 0;
        para->bcn_delay_offset             = 0;
        para->ppm_stable_cnt               = 0;
    }

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

    /* Continue a multi-cycle `lp raw <ms> <cnt>` run. That command cold-boots the
     * part on every wake, so the remaining count lives in HBN RAM rather than in
     * a variable; this is the only place that can pick it back up. Never returns
     * while cycles remain. Last, so a wedged run still leaves a fully initialised
     * console behind on the cycle that does come back. */
#if defined(CONFIG_SHELL)
    {
        extern int lp_raw_resume_if_queued(void);
        (void)lp_raw_resume_if_queued();
    }
#endif
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
