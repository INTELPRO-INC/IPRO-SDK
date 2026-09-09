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
 * @file lp_tickless.c
 * @brief FreeRTOS tickless-idle integration of the DTIM standby flow.
 *
 * bl_iot_sdk-style usage (bl616_demo_lp tickless.c): once ARMED, every
 * FreeRTOS idle period longer than one DTIM interval enters PDS with the
 * lp_fw blob cycling beacons; the blob budget (sim_dtim_target) is derived
 * from xExpectedIdleTime so OS timers still fire on schedule (late by at
 * most one DTIM cycle, tick-compensated). The STA announces STATIC PS once
 * at arm time, so the AP buffers our unicast and sets our TIM bit across
 * every idle-entered cycle; dataless wakes fall straight back to idle and
 * re-enter PDS without any full-firmware traffic. A WIFI wake (TIM hit)
 * pauses tickless and notifies the service task, which exits PS (PM=0 ->
 * AP flushes), reads the TCP socket, and on the host's wake command disarms
 * tickless for good — the "exit on remote TCP wakeup" contract.
 *
 * Host protocol = lp_tcpdtim's server (scratchpad/tcpdtim_server.py):
 *   DUT -> "HI\n"        host -> "ECHO\n"      connectivity check
 *   DUT -> "REQ <sec>\n" host sends "WAKE <n>\n" <sec> seconds later
 *
 * REPEAT (regression) mode — `lp tickless <host> <port> <sec> <dtim> <cycles>
 * [awake_s]`: instead of ending the run on the host's wake command, stay awake
 * for <awake_s> (the duty cycle a real periodic app would spend working), send
 * a fresh "REQ <sec>" and re-arm, <cycles> times over the same socket. cycles=0
 * keeps the original one-shot contract, so existing callers are unaffected.
 *
 * Context rules:
 *   vApplicationSleep()  — IDLE task, scheduler SUSPENDED: no blocking APIs,
 *                          FromISR-variant notifies only.
 *   lp_tickless_task()   — plain task: owns the socket and every blocking
 *                          wifi_mgmr / lwIP call.
 */

#include <stdio.h>
#include <string.h>

#include "lp_demo.h"

#if defined(CONFIG_WIFI6) && defined(CONFIG_LWIP) && \
    defined(CONFIG_IPRO_WIFI_PS) && defined(CONFIG_FREERTOS_USE_TICKLESS_IDLE)

#include <sys/socket.h>
#include <sys/time.h>      /* struct timeval for SO_SNDTIMEO */
#include <errno.h>
#include <FreeRTOS.h>
#include <task.h>
#include "hal_lp.h"
#include "lp_fw.h"      /* IOT2LP_PARA / LP_FW_WAKEUP_AP_LOSS */
/* After the FreeRTOS headers on purpose: pulled in ahead of them, lwipopts.h
 * shadows the port's TaskHandle_t/TickType_t definitions and the file stops
 * compiling. */
#include "lwip/stats.h" /* lwip_stats: name the pool behind an ERR_MEM */
#include "lwip/memp.h"  /* MEMP_MAX */
#include "lwip/mem.h"   /* mem_trace_dump: what is still LIVE, and who took it */

extern int wifi_mgmr_sta_ps_enter_static(void);
extern int wifi_mgmr_sta_ps_exit(void);
extern int wifi_mgmr_sta_autoconnect_enable(void);
/* macsw (rwnx.c): reclaim a control plane wedged in ME_BUSY by an orphaned
 * PS-mode transition. Safe no-op when ME is idle; runs from the idle hook.
 * Returns 1 on the pass that breaks a wedge. */
extern int rwnxl_control_plane_wdog(void);
/* AC-watchdog backstop; its usual caller (sdiowifi keep-ready) is not built in
 * the no-SDIO low-power configuration, so the idle hook drives it here. */
extern void txl_watchdog_backstop_poll(void);
extern int net_al_sta_arp_announce(void);
extern int net_al_sta_has_route(void);
extern void net_al_tcp_dump(const char *tag);
static void tickless_pcksite_dump(const char *tag);
/* macsw's PDS-entry doze-condition counters (rwnx.c, NX_POWERSAVE only). Same
 * local-extern reason as above; RWNXL_NOSLEEP_CNT is 6. */
extern volatile uint32_t rwnxl_pds_gate_total;
extern volatile uint32_t rwnxl_pds_gate_busy;
extern volatile uint32_t rwnxl_pds_gate_last;
extern volatile uint32_t rwnxl_pds_gate_hits[6];
extern volatile uint32_t rwnxl_pds_gate_pck_max;
/* Our own PDS TX-gate counters (lp_demo.c, WiFi-PS builds only). */
/* macsw's per-site accounting for txl_cntrl_env.pck_cnt (txl_cntrl.c). */
extern volatile uint32_t txl_pck_site[6];
/* PDS-entry TX flush hand-off (lp_demo.c / macsw macif). */
extern volatile uint32_t g_lp_tx_cfm_drain_pending;
extern void macif_tx_reset_set(bool en);
extern void macif_tx_cfm_drain(void);
extern volatile uint32_t s_pds_tx_busy;
extern volatile uint32_t s_pds_tx_forced;
extern volatile uint32_t s_pds_tx_maxheld_us;
extern volatile uint32_t s_pds_prevent_calls;
extern volatile uint32_t s_pds_prevent_hit;
extern volatile uint32_t s_pds_prevent_bits;
extern volatile uint32_t machw_abstmr_hits;
extern volatile uint32_t machw_abstmr_pend;
extern volatile uint32_t machw_abstmr_state;
extern volatile uint32_t s_pds_tx_flushed;
/* macsw: queue a KE_EVT_RESET. The same remedy the AC watchdog's ASSERT_REC
 * path applies to an AC timer recover_stale could not clear, and the only cure
 * for a MAC/TX wedge that does not need an external reset. */
extern void dbg_mac_reset_request(void);

#define TICKLESS_ACK_SETTLE_MS  500  /* let the host TCP ACK drain pre-arm  */
#define TICKLESS_RX_POLL_MS     100
#define TICKLESS_RX_POLLS       30   /* 3 s post-PM=0 window for the flush  */
#define TICKLESS_KEEPALIVE_POLLS 15  /* 1.5 s per keepalive HI              */
/*
 * Keepalive HI attempts before the link is called dead. 1.5 s x 4 = 6 s, which
 * is the first deadline longer than lwIP's 3000 ms initial RTO (tcp.c:2071).
 *
 * A single 1.5 s attempt gives up BEFORE TCP's first retransmit, so one lost HI
 * was indistinguishable from a dead peer. Bench 2026-07-31: 8 of 8 cycles
 * declared the link dead, the host received ZERO keepalive HIs, and every
 * rebuild then succeeded on attempt 1 — the link was fine every time.
 *
 * Each false death is the whole leak. The HI sits unacked on the pcb that
 * close() abandons, pinning 1984 B (tcp_write), and the ARP that went with it
 * pins 512 B (etharp_raw): 2496 B/cycle against a measured +2512 B/cycle heap
 * slope, 1:1 with the reconnect count (live-allocation trace, 2026-07-31).
 */
#define TICKLESS_KEEPALIVE_TRIES 4
/*
 * Reconnect attempts per retry round. Was 5, and the tail of that was actively
 * self-destructive.
 *
 * Every FAILED connect() permanently pins 532 B of lwIP heap -- the SYN, built by
 * tcp_enqueue_flags and left on the pcb's unacked queue, which close() does not
 * reclaim (live-allocation trace, 2026-07-30). At the ceiling that heap IS the
 * reason connect() cannot succeed any more, so the retry ladder spends the exact
 * resource recovery needs: with 5 attempts x 6 rounds it reached 56 stranded SYNs
 * = 30 KB = 62% of the whole heap in a single episode, and the DUT could then
 * never reconnect at all. That is why no episode has ever self-recovered.
 *
 * The attempts beyond the second have never once worked. Across every soak today,
 * of 107 successful reconnects: 104 on attempt 1, 3 on attempt 2, ZERO on
 * attempts 3-5. So 2 keeps all 107 successes and drops 60% of the heap cost. The
 * OUTER budget (TICKLESS_LINK_GIVEUP_MAX rounds, 30 s apart) is what actually
 * provides patience, and it is untouched -- same wall-clock coverage.
 */
#define TICKLESS_RECONNECT_MAX  2    /* reconnects before giving up on link */
#define TICKLESS_RECONNECT_GAP_MS 2000
#define TICKLESS_SEND_TIMEO_S   3    /* bound every blocking send/connect    */
#define TICKLESS_LINK_RETRY_MS   30000 /* wait between dead-host link retries  */
/*
 * How often the route is re-checked while waiting for it. The bound is
 * TICKLESS_LINK_RETRY_MS on purpose: both places that used to sleep blindly
 * (before the attempts, and between rounds) are waiting for the same event, so
 * they get the same patience rather than two constants that can drift apart.
 *
 * A separate 20 s bound was tried first and was 4 s too short for the slowest
 * recovery measured (2026-08-02, injected drop: link_check saw the STA down at
 * 13:18:23, two association attempts timed out, and the route only returned at
 * 13:18:48 -- 25 s).
 */
#define TICKLESS_ROUTE_POLL_MS     500
/*
 * Failed rounds before ESCALATING to a MAC reset -- not before "conceding".
 *
 * Was 6, and that made the ladder a single strategy repeated twelve times: two
 * connects, wait 30 s, two connects, wait 30 s... ~44 s per round, so the one
 * action that can actually clear a MAC-level stall (dbg_mac_reset_request) was
 * ~264 s away. Bench 2026-07-31: the harness's own 300 s stall detector fired
 * first and re-associated the DUT from the host -- i.e. the escalation NEVER RAN
 * in the failure it exists for, and recovery stopped being in-place.
 *
 * When two connects have already failed identically, ten more are repetition,
 * not patience. Escalate at 2 rounds (~90 s), and reset the counter afterwards
 * so the ladder REPEATS -- fail, fail, MAC reset, fail, fail, MAC reset -- which
 * bounds the reset rate (one per ~90 s, well clear of the PM48 reset-storm
 * regime) while never conceding. Patience now comes from repeating the ladder,
 * not from postponing the only remedy in it.
 */
#define TICKLESS_LINK_GIVEUP_MAX 2
#define TICKLESS_AP_LOSS_MAX    5    /* consecutive beacon-loss give-ups    */
/* The supplicant's autoconnect has its own backoff before it even starts
 * scanning — measured ~57 s after a loss — so a short window gives up while the
 * reconnect is still pending and wastes a retry. 120 s covers the observed
 * scan + associate with margin. */
/* 40 x 5 s = 200 s. The probe is lpfw_populate_from_sta(), which logs a line
 * per miss, so a tighter poll just buries the console in "not associated"
 * during the very failure we would be reading the log to diagnose.
 *
 * The 200 s is not slack: on a repeated association failure wpa_supplicant
 * arms CTRL-EVENT-SSID-TEMP-DISABLED for 120 s, and the earlier 120 s budget
 * expired inside that window every time -- so the escalation below re-fired
 * while the supplicant was still backing off, which no MAC reset can help and
 * which only drove auth_failures higher (observed climbing to 14). Outlast the
 * supplicant's own backoff, then judge. */
#define TICKLESS_REASSOC_POLLS  40
#define TICKLESS_REASSOC_GAP_MS 5000
/* Slack on top of the requested wake so a merely slow wake is not mistaken for
 * a dead link: measured latency is ~3 s but degrades to ~16 s before RF gives
 * out entirely, so 30 s leaves headroom without stretching the deadlock window.
 */
#define TICKLESS_GUARD_MARGIN_MS 30000
#define TICKLESS_SOCK_CHECK_MS   10000 /* standby socket health-check cadence */
#define TICKLESS_KEEPALIVE_MS    30000 /* standby keepalive TX cadence — keep a
                                        * router's ARP/neigh for us + the NAT
                                        * map fresh so a buffered wake is not
                                        * dropped at L3->L2 (HW root cause of
                                        * the tracked ~50% miss). */
#define TICKLESS_KA_DRAIN_POLLS  5   /* ~0.5 s: let the WiFi task push the KA   */
#define TICKLESS_MIN_PDS_MS     20   /* idle below this: WFI, no PDS round   */

static volatile int s_en;            /* armed: idle may enter PDS           */
static volatile int s_pause;         /* WIFI wake being drained by the task */
static TaskHandle_t s_task;
static int s_sock = -1;
static uint32_t s_dtim_ms;
static uint32_t s_delay_s;
static char s_host[16];
static uint16_t s_port;
static uint32_t s_pds_entries;
static uint32_t s_wifi_wakes;
static uint32_t s_seed_max;          /* seed plumbing mode: auto-disarm cap */
static int s_ps_active;              /* STA announced static PS at arm time */
static uint32_t s_dbg_calls;         /* hook invocations while armed        */
static uint32_t s_dbg_max_expected;  /* max xExpectedIdleTime seen          */
static uint32_t s_repeat;            /* regression mode: cycles, 0=one-shot */
static uint32_t s_awake_ms;          /* awake window between repeat cycles  */
static uint32_t s_cycles_done;
static uint32_t s_reconnects;        /* sockets rebuilt after a dead link   */
static uint32_t s_ka_late;           /* keepalives that needed a retry --
                                      * the false-death rate a 1.5 s deadline
                                      * used to pay a rebuild for            */
static uint32_t s_guard_fires;       /* standby deadlines hit with no wake  */
static uint32_t s_tick_deficit_ms;   /* oversleep not yet credited to the
                                      * kernel clock (see tick compensation) */
static volatile TickType_t s_ka_due; /* next standby keepalive TX (ticks)   */
static volatile int s_ka_pending;    /* budget wake carried a due keepalive */
static uint32_t s_ka_sent;           /* keepalive branches run this cycle   */
static int s_ever_linked;            /* distinguishes a rebuild from setup  */
static uint32_t s_ap_losses;         /* CONSECUTIVE beacon-loss give-ups    */
static uint32_t s_link_giveups;      /* dead-host link retries (repeat mode) */
static uint32_t s_escalations;       /* wedge recoveries driven from here    */
static uint32_t s_catchup_calls;     /* idle-hook passes: is the hook reached? */

/*
 * Attribute the standby segment's growth to the operations inside it.
 *
 * heapseg (below) localised ~3-6 KB/cycle to tickless_standby_cycle() with the
 * keepalive and tail segments both at +0, so the cost is in the standby loop
 * itself. One delta across the whole segment cannot say which part; wrapping
 * individual calls cannot either, because the allocations are made by the tcpip
 * thread, NOT by this task -- a probe around recv() would show +0 while the heap
 * grew during the vTaskDelay next to it.
 *
 * So partition the loop by TIME rather than by call, into spans that between
 * them cover every moment of standby:
 *   wait  - blocked in ulTaskNotifyTake. The tcpip/WiFi tasks and the idle hook
 *           all run here, so this is a superset of pds.
 *   pds   - inside lp_demo_sleep_from_idle, i.e. the part of `wait` that was
 *           genuinely powered down. wait - pds is therefore "grew while awake
 *           but with this task blocked" -- the RX path's own footprint.
 *   wake  - the WIFI-wake handling branch: ps_exit, the recv drain, ps re-arm.
 *   ka    - the keepalive branch: arp announce, send, drain.
 *   gap   - the idle-timeout branch's socket health check.
 * The three post-wait branches all leave the loop body by continue/return, which
 * a statement-wrapping macro cannot span, so they are opened once after the wait
 * returns and closed at the top of the next iteration by index.
 * The PDS span runs in the idle hook with interrupts masked, so it may only ever
 * touch a counter -- never printf from there.
 *
 * Declared here rather than beside the other heap helpers because the first
 * HEAP_ACC use is vApplicationSleep, which comes earlier in the file.
 */
#define HEAP_SPAN_GAP   0
#define HEAP_SPAN_KA    1
#define HEAP_SPAN_WAKE  2
#define HEAP_SPANS      3

#if LWIP_STATS && MEM_STATS
static int s_heap_acc_pds;
static int s_heap_acc_wait;
static int s_heap_acc_span[HEAP_SPANS];
static unsigned s_heap_span_base;
/* Second stage: the span split named the keepalive branch (bench 2026-07-30:
 * wait=+0 pds=+0 wake=+0 gap=+0 ka=+4504 over 2 keepalives). Break that branch
 * down by call -- these three have no hidden control flow, so wrapping the
 * statement is enough, and recv_poll's internal vTaskDelay is inside the span. */
static int s_heap_acc_arp;
static int s_heap_acc_snd;
static int s_heap_acc_drain;

#define HEAP_ACC(acc, stmt)                                                \
    do {                                                                   \
        unsigned _heap_before = (unsigned)lwip_stats.mem.used;              \
        stmt;                                                              \
        (acc) += (int)(unsigned)lwip_stats.mem.used - (int)_heap_before;    \
    } while (0)
#define HEAP_SPAN_OPEN()                                                   \
    do { s_heap_span_base = (unsigned)lwip_stats.mem.used; } while (0)
#define HEAP_SPAN_CLOSE(w)                                                 \
    do {                                                                   \
        if ((w) >= 0) {                                                    \
            s_heap_acc_span[w] += (int)(unsigned)lwip_stats.mem.used        \
                                  - (int)s_heap_span_base;                 \
        }                                                                  \
    } while (0)
#else
#define HEAP_ACC(acc, stmt)        do { stmt; } while (0)
#define HEAP_SPAN_OPEN()           do { } while (0)
#define HEAP_SPAN_CLOSE(w)         do { (void)(w); } while (0)
#endif

/*
 * The host's wake command. strstr is case-sensitive, so the legacy upper-case
 * token and the regression harness's lower-case "wakeup" must BOTH be matched
 * here: an unmatched payload reads as "not the wake command" and sends the DUT
 * straight back to standby while the host believes it woke us — a silent
 * standby-forever hang, which is exactly the failure the drain-side match
 * already exists to prevent.
 */
static int tickless_is_wake(const char *s)
{
    return (strstr(s, "WAKE") != NULL) || (strstr(s, "wakeup") != NULL);
}

/*
 * FreeRTOS tickless hook: configUSE_TICKLESS_IDLE=2 maps
 * portSUPPRESS_TICKS_AND_SLEEP() here (FreeRTOSConfig.h). Called from the
 * IDLE task with the scheduler suspended; xExpectedIdleTime is in ticks
 * (1 kHz tick -> ms). Tick compensation happens HERE via vTaskStepTick —
 * the impl's xTaskCatchUpTicks path asserts under a suspended scheduler,
 * so it is gated to task-context sleeps only.
 */
void vApplicationSleep(uint32_t xExpectedIdleTime)
{
    if (!s_en || s_pause) {
        /* Park the idle task while the service task drains a wake (s_pause) or
         * while tickless is disarming (!s_en). UNMASK before WFI: the FreeRTOS
         * critical section wrapping this hook clears mstatus.MIE, and the
         * Nuclei ECLIC WFI never idles while masked — it returns at once, the
         * tick never advances, the idle task busy-loops and starves beacon RX
         * so the cycle's wake is missed. portENABLE_INTERRUPTS restores MIE;
         * the same idiom guards the two cycles==0 WFI sites below. (JTAG
         * 2026-07-22: MTH stays 0 on this thread-level path — no ECLIC
         * threshold mask to clear.) */
        portENABLE_INTERRUPTS();
        __asm__ volatile("wfi");
        return;
    }
    /* Freeze the tick from here to vTaskStepTick: xExpectedIdleTime bounds
     * the step against xNextTaskUnblockTime, and any tick that sneaks in
     * before PDS entry would break that bound (vTaskStepTick asserts).
     * CAUTION: the Nuclei ECLIC WFI does NOT wake for interrupts at or
     * below the MTH threshold this mask raises — never WFI while masked
     * (see the cycles==0 branch below). The PDS path itself is fine: its
     * wake is a PDS-controller hardware reset, not an interrupt take. */
    portDISABLE_INTERRUPTS();
    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
        portENABLE_INTERRUPTS();
        return;
    }
    s_dbg_calls++;
    if (xExpectedIdleTime > s_dbg_max_expected) {
        s_dbg_max_expected = xExpectedIdleTime;
    }
    /* Budget the blob to the next OS deadline: it re-sleeps one DTIM
     * interval per beacon cycle and takes a timed wake when the budget is
     * spent, so timers run at most one cycle late. Idle windows shorter
     * than a cycle are not worth a PDS round-trip. */
    uint32_t cycles = xExpectedIdleTime / s_dtim_ms;
    if (cycles == 0 && xExpectedIdleTime < TICKLESS_MIN_PDS_MS) {
        /* Idle window too short to be worth a PDS round trip. UNMASK
         * BEFORE parking: the Nuclei ECLIC WFI does not wake for
         * interrupts at or below MTH, so a masked WFI here is a
         * permanent park on an associated system (JTAG'd: MTH=0xFF, tick
         * IP=1/IE=1, mtime>>mtimecmp, CPU parked — the 0-3-entry wedge).
         * Unmasked, the pending tick is taken right here and the WFI rides
         * out the remainder of the window. */
        portENABLE_INTERRUPTS();
        __asm__ volatile("wfi");
        return;
    }
    if (cycles == 0) {
        /* Window is worth sleeping but shorter than one DTIM cycle. On an
         * associated system the OS deadline is lwIP's 250 ms tcp_tmr, so
         * with a long DTIM period (product DTIM10: ~1 s) a full cycle
         * NEVER fits — requiring one would starve the blob forever and
         * the hook would WFI at tick rate through the whole standby
         * (HW-verified: dtim=1024 armed 40 s -> calls=41452,
         * max_expected_idle=250, entries=0, every TCP wake lost). Take
         * ONE cycle: overshooting the deadline is the design's existing
         * "timers run at most one cycle late" contract, and the tick
         * step below already clamps to xExpectedIdleTime. */
        cycles = 1;
    }
    if (cycles > 0xFFFFu) {
        cycles = 0xFFFFu;
    }
    /* Seed plumbing mode: keep entries short (~1 s) so the auto-disarm cap
     * converges quickly — a quiet seeded system can have a near-unbounded
     * expected idle. */
    if (s_seed_max != 0u && cycles > 10u) {
        cycles = 10u;
    }

    uint32_t slept_ms = 0;
    int reason;
    HEAP_ACC(s_heap_acc_pds,
             reason = lp_demo_sleep_from_idle(s_dtim_ms, cycles,
                                              s_ps_active != 0, &slept_ms));
    if (reason < 0) {
        portENABLE_INTERRUPTS();
        if (reason == -2) {
            /* The WiFi-quiescence gate refused this power-down (see
             * lp_demo_sleep_from_idle). Its condition clears on a TX confirm, so
             * WAIT for one rather than returning: a bare return has the idle task
             * re-enter this hook at once and spin at ~47k gate samples/s. The
             * bl616 reference does exactly __WFI(); return; at its own gates.
             * UNMASK first -- the Nuclei ECLIC WFI does not idle while masked
             * (same hazard as the two WFI sites above). */
            __asm__ volatile("wfi");
        }
        return;             /* aborted — next idle pass retries */
    }
    s_pds_entries++;

    /* Tickless tick compensation. The bl616 reference only ever sleeps UP TO
     * xExpectedIdleTime, so its plain vTaskStepTick(clamp) never loses time.
     * Our cycles floor deliberately OVERSLEEPS the OS deadline (a full DTIM
     * period vs a ~250 ms lwIP-bounded window), and vTaskStepTick may not
     * exceed the bound — the excess used to vanish, running the kernel clock
     * at ~22% of wall time through a DTIM10 standby and stretching every
     * tick-based timeout ~4.5x (HW-measured: standby guard, lwIP timers,
     * DHCP renew). The excess CANNOT be credited here either:
     * xTaskCatchUpTicks ASSERTS inside the suspended-scheduler region this
     * hook runs in (HW-hit: IDLE assert at tasks.c:4574 on the first TIM
     * wake). Bank it; lp_tickless_tick_catchup() replays it from
     * vApplicationIdleHook microseconds later, where the scheduler is live.
     * 1 kHz tick -> ms == ticks. */
    uint32_t step = slept_ms;
    if (step > xExpectedIdleTime) {
        vTaskStepTick(xExpectedIdleTime);
        s_tick_deficit_ms += step - xExpectedIdleTime;
    } else if (step > 0) {
        vTaskStepTick(step);
    }
    portENABLE_INTERRUPTS();

    /* Seed plumbing mode: association-free hook/budget/tick-compensation
     * validation — self-disarm after the requested number of entries so the
     * shell comes back on its own (no TCP wake available without an AP). */
    if (s_seed_max != 0u && s_pds_entries >= s_seed_max) {
        s_en = 0;
        printf("[LP-TL] seed cap reached (%lu entries) -> DISARMED\r\n",
               (unsigned long)s_pds_entries);
        return;
    }

    /* Hand off to the service task on a TIM hit, or on a plain budget wake
     * when a keepalive is due (s_ka_due advanced HERE -> notify exactly once
     * per period; a keepalive-only wake is flagged so the task takes the
     * MINIMAL path — bare TX, no PS churn). */
    if (reason == HAL_LP_WAKEUP_WIFI && s_task != NULL) {
        s_wifi_wakes++;
        s_pause = 1;
        BaseType_t woken = pdFALSE;
        vTaskNotifyGiveFromISR(s_task, &woken);
        (void)woken;        /* schedule runs when the idle task resumes */
    } else if (s_task != NULL && s_ka_due != 0u &&
               (int32_t)(xTaskGetTickCountFromISR() - s_ka_due) >= 0) {
        s_ka_due = xTaskGetTickCountFromISR() + pdMS_TO_TICKS(TICKLESS_KEEPALIVE_MS);
        s_ka_pending = 1;
        s_pause = 1;
        BaseType_t woken = pdFALSE;
        vTaskNotifyGiveFromISR(s_task, &woken);
        (void)woken;
    }
}

/* >0 = bytes read, 0 = nothing within the window, -1 = LINK DEAD: an orderly
 * FIN (recv 0) or a real socket error (RST -> ECONNRESET etc — anything but
 * would-block). Both used to be treated as "no data yet" and polled forever:
 * HW-observed 2026-07-22, a server FIN fetched by the very TIM wake that
 * should have healed the link was dropped as "without data" and the DUT
 * zombie-standby'd on the dead socket for 15+ min. */
/* Why the last tickless_recv_poll() ended. A -1 conflates an orderly FIN with a
 * hard error, and the two mean opposite things for a keepalive: a FIN is the
 * peer leaving, an error is our own socket breaking. Waiting longer cures
 * neither, but only the timeout case is a lost packet worth retrying. */
#define RXP_DATA    0
#define RXP_TIMEOUT 1
#define RXP_FIN     2
#define RXP_ERR     3
static int s_rxp_why;
static int s_rxp_errno;

static const char *rxp_why_str(void)
{
    switch (s_rxp_why) {
    case RXP_DATA:    return "data";
    case RXP_TIMEOUT: return "timeout";
    case RXP_FIN:     return "FIN";
    default:          return "err";
    }
}

static int tickless_recv_poll(char *buf, size_t len, int polls)
{
    s_rxp_why = RXP_DATA;
    s_rxp_errno = 0;
    for (int i = 0; i < polls; i++) {
        int n = recv(s_sock, buf, len - 1, MSG_DONTWAIT);
        if (n > 0) {
            buf[n] = '\0';
            return n;
        }
        if (n == 0) {
            s_rxp_why = RXP_FIN;
            return -1;                          /* orderly FIN: peer is gone */
        }
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            s_rxp_why = RXP_ERR;
            s_rxp_errno = errno;
            return -1;                          /* RST / hard socket error */
        }
        vTaskDelay(pdMS_TO_TICKS(TICKLESS_RX_POLL_MS));
    }
    s_rxp_why = RXP_TIMEOUT;
    return 0;
}

/* Announce static PS and give the host's TCP ACKs time to drain — a
 * buffered ACK sets our TIM bit and converts the next cycle into an
 * instant wake (the lp_tcpdtim lesson). Returns 1 if the host's wake
 * command was found in the drained bytes: under TIM-wake churn (e.g. a
 * pinged bench) this drain runs every few seconds, and a WAKE landing
 * in its window used to be silently discarded — host thinks it woke
 * us, DUT stays in standby forever (observed live). */
static int tickless_ps_arm(void)
{
    char scratch[64];
    int woken = 0;
    int n;

    if (wifi_mgmr_sta_ps_enter_static() != 0) {
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(TICKLESS_ACK_SETTLE_MS));
    while ((n = recv(s_sock, scratch, sizeof(scratch) - 1, MSG_DONTWAIT)) > 0) {
        scratch[n] = '\0';
        if (tickless_is_wake(scratch)) {
            woken = 1;
        }
    }
    return woken;
}

/* Open the socket and complete the HI/ECHO handshake. Leaves s_sock == -1 on
 * failure so the caller can simply retry. */
/* How much the lwIP heap has to move before saying so. At rest `used` is ~60 B
 * and jitters to ~1.4 KB, so this stays quiet on a healthy link; the excursion
 * worth catching is the ~31 KB seen during an episode. */
#define TICKLESS_HEAP_DELTA_B  2048

#if LWIP_STATS && MEM_STATS
/*
 * Trajectory, not snapshots.
 *
 * Printing the heap only where a connect fails showed it climbing 540 B per
 * attempt with used == max and led me to call it a permanent leak. That was
 * wrong: `max` is a high-water mark that never comes down, so used == max is
 * true of ANY rising phase, and a controlled run (forced failed connects, then
 * idle) returned to the 60 B baseline exactly -- net leak zero. A rising
 * counter and a leaking counter are indistinguishable unless you also watch it
 * with the pressure removed. So sample at points that span BOTH: during the
 * retry ladder (rise), and again once the link is back and each cycle completes
 * (fall). Report only meaningful moves, plus any change in mem.err, which is
 * what turns into connect() returning ERR_MEM.
 */
static unsigned s_heap_last;
static unsigned s_heap_err_last;

/*
 * Bisect WHERE a cycle's growth happens.
 *
 * The watcher above establishes THAT the heap climbs ~2 KB/min through entirely
 * healthy cycling (bench 2026-07-30: 0 -> 18 KB in 9 min, err=0, zero connect
 * failures) until it pins at 48796/49152 and every TCP allocation fails. It
 * cannot say which part of a cycle is responsible. A threshold cannot answer
 * that either -- the per-segment moves are small and a threshold hides exactly
 * the ones we need. So mark the heap at each phase boundary and emit ONE line
 * per cycle carrying all the deltas: complete information, one line per ~2 min.
 */
#define HEAP_MARK_START     0   /* top of the cycle, before link_check   */
#define HEAP_MARK_LINKOK    1   /* keepalive / socket rebuild done       */
#define HEAP_MARK_STANDBY   2   /* standby + host wake returned          */
#define HEAP_MARK_END       3   /* cycle accounted                       */
#define HEAP_MARKS          4

static unsigned s_heap_mark[HEAP_MARKS];

static void tickless_heap_mark(int i)
{
    if (i >= 0 && i < HEAP_MARKS) {
        s_heap_mark[i] = (unsigned)lwip_stats.mem.used;
    }
}

static void tickless_heap_inner(void)
{
    printf("[LP-TL] heapin wait=%+d (pds=%+d) wake=%+d ka=%+d gap=%+d "
           "entries=%lu wakes=%lu\r\n",
           s_heap_acc_wait, s_heap_acc_pds,
           s_heap_acc_span[HEAP_SPAN_WAKE], s_heap_acc_span[HEAP_SPAN_KA],
           s_heap_acc_span[HEAP_SPAN_GAP],
           (unsigned long)s_pds_entries, (unsigned long)s_wifi_wakes);
    printf("[LP-TL] heapka arp=%+d snd=%+d drain=%+d (kas=%lu)\r\n",
           s_heap_acc_arp, s_heap_acc_snd, s_heap_acc_drain,
           (unsigned long)s_ka_sent);
    /* Was the chip powered down with the MAC busy? rwnxl_pds_gate_sample() has
     * been recording macsw's own doze conditions at every PDS entry all along,
     * but nothing ever read the counters out. The bl616 reference gates its
     * connected PDS entry on exactly these (tickless.c __enter_connected:
     * "if (rwnxl_ps_sleep_check() != 0) { __WFI(); return; }"), and ours does
     * not -- so print how often each condition was true when we slept anyway,
     * next to the frames that went missing. TX is the one to watch: it means we
     * powered down holding packets. */
    printf("[LP-TL] pdsgate n=%lu busy=%lu ps=%lu tx=%lu tmr=%lu kevt=%lu "
           "qs=%lu qv=%lu pckmax=%lu last=0x%lx\r\n",
           (unsigned long)rwnxl_pds_gate_total,
           (unsigned long)rwnxl_pds_gate_busy,
           (unsigned long)rwnxl_pds_gate_hits[0],
           (unsigned long)rwnxl_pds_gate_hits[1],
           (unsigned long)rwnxl_pds_gate_hits[2],
           (unsigned long)rwnxl_pds_gate_hits[3],
           (unsigned long)rwnxl_pds_gate_hits[4],
           (unsigned long)rwnxl_pds_gate_hits[5],
           (unsigned long)rwnxl_pds_gate_pck_max,
           (unsigned long)rwnxl_pds_gate_last);
    /* And whether OUR gate actually did anything. Without these two the gate's
     * effect was pure inference -- refusals = power-downs deferred to wait for a
     * confirm, forced = grace expired and we slept with the TX path busy anyway
     * (i.e. lost a frame). If forced tracks pdsgate's tx= count, the grace is
     * never long enough to save anything and the gate is doing nothing. */
    printf("[LP-TL] abstmr hits=%lu pend=0x%lx state=%lu\r\n",
           (unsigned long)machw_abstmr_hits,
           (unsigned long)machw_abstmr_pend,
           (unsigned long)machw_abstmr_state);
    printf("[LP-TL] psprev hit=%lu/%lu bits=0x%lx\r\n",
           (unsigned long)s_pds_prevent_hit,
           (unsigned long)s_pds_prevent_calls,
           (unsigned long)s_pds_prevent_bits);
    printf("[LP-TL] txgate busy=%lu forced=%lu catchup=%lu maxheld=%luus\r\n",
           (unsigned long)s_pds_tx_busy, (unsigned long)s_pds_tx_forced,
           (unsigned long)s_catchup_calls,
           (unsigned long)s_pds_tx_maxheld_us);
    /* WHICH packets are the ones never confirmed. txl_pck_site[] was already in
     * macsw and nobody printed it: [0..4] are the five sites that increment
     * txl_cntrl_env.pck_cnt (txl_cntrl.c 1527/3461/3594/3527 and txl_frame.c 432
     * for internal mgmt frames) and [5] is the single site that decrements it
     * (macif_tx_cfm_push). So sum([0..4]) - [5] == pck_cnt, and the site whose
     * count runs ahead names the traffic class whose confirms never arrive --
     * the one untouched root cause behind every leak measured here. */
    printf("[LP-TL] pcksite %lu %lu %lu %lu %lu cfm=%lu bal=%ld\r\n",
           (unsigned long)txl_pck_site[0], (unsigned long)txl_pck_site[1],
           (unsigned long)txl_pck_site[2], (unsigned long)txl_pck_site[3],
           (unsigned long)txl_pck_site[4], (unsigned long)txl_pck_site[5],
           (long)(txl_pck_site[0] + txl_pck_site[1] + txl_pck_site[2] +
                  txl_pck_site[3] + txl_pck_site[4]) -
           (long)txl_pck_site[5]);
#if LWIP_MEM_TRACE
    /* The question four earlier probes could not answer: of everything this
     * cycle allocated, WHICH BLOCKS ARE STILL HELD, and who allocated them?
     * PCs resolve offline with addr2line against the lpwifi ELF. */
    mem_trace_dump("cycle", 6);
#endif
    /* Pairs with the trace: if the abandoned sockets are never reaped, the pcb
     * count climbs in step with the live-block count. */
    net_al_tcp_dump("cyc");
    s_heap_acc_pds = 0;
    s_heap_acc_wait = 0;
    s_heap_acc_arp = 0;
    s_heap_acc_snd = 0;
    s_heap_acc_drain = 0;
    for (int i = 0; i < HEAP_SPANS; i++) {
        s_heap_acc_span[i] = 0;
    }
}

static void tickless_heap_segments(void)
{
    printf("[LP-TL] heapseg start=%u keepalive=%+d standby=%+d tail=%+d "
           "total=%+d err=%u\r\n",
           s_heap_mark[HEAP_MARK_START],
           (int)s_heap_mark[HEAP_MARK_LINKOK]  - (int)s_heap_mark[HEAP_MARK_START],
           (int)s_heap_mark[HEAP_MARK_STANDBY] - (int)s_heap_mark[HEAP_MARK_LINKOK],
           (int)s_heap_mark[HEAP_MARK_END]     - (int)s_heap_mark[HEAP_MARK_STANDBY],
           (int)s_heap_mark[HEAP_MARK_END]     - (int)s_heap_mark[HEAP_MARK_START],
           (unsigned)lwip_stats.mem.err);
}

static void tickless_heap_watch(const char *tag)
{
    unsigned u = (unsigned)lwip_stats.mem.used;
    unsigned e = (unsigned)lwip_stats.mem.err;
    unsigned d = (u > s_heap_last) ? (u - s_heap_last) : (s_heap_last - u);

    if (d >= TICKLESS_HEAP_DELTA_B || e != s_heap_err_last) {
        printf("[LP-TL] heap %s used=%u (was %u) max=%u avail=%u err=%u\r\n",
               tag, u, s_heap_last, (unsigned)lwip_stats.mem.max,
               (unsigned)lwip_stats.mem.avail, e);
        s_heap_last = u;
        s_heap_err_last = e;
    }
}
#else
#define tickless_heap_watch(tag)   do { } while (0)
#define tickless_heap_mark(i)      do { } while (0)
#define tickless_heap_segments()   do { } while (0)
#define tickless_heap_inner()      do { } while (0)
#endif

static int tickless_connect(void)
{
    struct sockaddr_in addr;
    char buf[64];
#if LWIP_STATS && MEM_STATS
    /* Localise the 540 B/failed-connect lwIP heap leak measured 2026-07-30 to a
     * single call: sample the heap before socket(), after a failed connect(),
     * and after close(). If afterclose > open, close() is not releasing what
     * connect() took. */
    unsigned h_open, h_sock, h_fail;
    h_open = (unsigned)lwip_stats.mem.used;
#endif

    /* Do not pay for an attempt that is already known to fail.
     *
     * With no route the connect returns ENETUNREACH, but lwIP has by then built a
     * SYN (tcp_enqueue_flags) and an ARP request for the gateway (etharp_request),
     * and on this platform both strand in the TX path and pin their heap blocks
     * for good. Measured at the point the DUT dies, those two are 18-58% and
     * 7-21% of the whole heap, accumulated almost entirely inside reconnect
     * episodes -- where the link is down and EVERY attempt is doomed. The retry
     * ladder was therefore spending, irreversibly, the exact resource it needs to
     * succeed later; that is why no episode has ever self-recovered.
     *
     * Skipping costs nothing: the caller's retry pacing is unchanged (its gap and
     * the 30 s outer round still run), and the WiFi stack's own reconnect is what
     * has to bring the route back, not this socket. Same principle as the
     * reconnect-budget cut, taken to its conclusion. */
    if (!net_al_sta_has_route()) {
        printf("[LP-TL] no route (STA down) -> skipping connect\r\n");
        return -1;
    }

    s_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_sock < 0) {
        printf("[LP-TL] socket() failed err=%d\r\n", errno);
        return -1;
    }
#if LWIP_STATS && MEM_STATS
    /* Split the per-attempt leak between socket() and connect(). Measured:
     * `sock=+0 conn=+540 close=+0`, and the live-allocation trace names the 540 B
     * as `tcp_enqueue_flags`, i.e. the SYN, sitting in unacked.
     *
     * SO_LINGER{1,0} was the obvious reclaim for that and is REFUTED ON THE
     * BENCH, not merely ineffective: `tcp_abort()` sends a RST, building the RST
     * allocates its own 524 B pbuf (`tcp_output_alloc_header_common`), and that
     * pbuf strands in the TX path exactly like everything else -- so close() went
     * from `+0` to `+532` and the per-attempt leak DOUBLED, 540 -> 1072. Reverted.
     * Note this ladder runs with tickless DISARMED (no PDS at all), so the
     * stranding here is not power-down loss: the TX path itself is not
     * confirming, awake. That is the primary defect; see
     * project_ipro6_pds_tx_strand_leak. */
    h_sock = (unsigned)lwip_stats.mem.used;
#endif
    /* Bound every blocking send (connect is bounded separately below). A
     * silently half-dead socket — peer gone, no RST — leaves unacked bytes in
     * the send buffer, and TCP then holds a blocking send for its full
     * retransmit budget: HW-observed 2026-07-23 as a ~300 s standby stall in
     * link_check's HI keepalive that
     * forced a heavy re-associate and cascaded into missed wakes. With a bound
     * the send fails fast, the dead-link path rebuilds the socket, and it
     * self-heals instantly — exactly as an RST death already does. A healthy
     * 3-byte send completes in well under a millisecond, so this never fires on
     * a live link. */
    {
        struct timeval tv = { .tv_sec = TICKLESS_SEND_TIMEO_S, .tv_usec = 0 };
        setsockopt(s_sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(s_port);
    addr.sin_addr.s_addr = inet_addr(s_host);
    /* Bounded connect. lwIP does NOT honor SO_SNDTIMEO for connect(), so a
     * connect to an unreachable host blocks ~25 s on the default TCP timeout —
     * five of them make link_check's reconnect budget take ~125 s. That is long
     * enough to trip both the standby stall detector and this file's own
     * watchdog on a link that is merely slow, not wedged (HW-diagnosed
     * 2026-07-24: hb_stale climbed 5->125 s per link_check with cyc frozen, yet
     * the task never wedged and the watchdog's MAC reset was a harmless false
     * fire). Drive it non-blocking and bound the wait with select() so a dead
     * link fails in TICKLESS_SEND_TIMEO_S, keeping the whole retry loop fast. */
    {
        int fl = fcntl(s_sock, F_GETFL, 0);
        int cr;
        /* Why it failed, not just that it did. A failing reconnect returns in
         * ~2 ms with no SYN on the air (AP-side tcpdump, 2026-07-27), so the
         * refusal is local to lwIP -- and "connect failed" alone cannot say
         * whether that is a missing route, an exhausted PCB pool or a socket
         * that never opened. */
        int why = 0;
        fcntl(s_sock, F_SETFL, fl | O_NONBLOCK);
        cr = connect(s_sock, (struct sockaddr *)&addr, sizeof(addr));
        why = errno;
        if (cr != 0 && errno == EINPROGRESS) {
            fd_set wf;
            struct timeval tv = { .tv_sec = TICKLESS_SEND_TIMEO_S, .tv_usec = 0 };
            FD_ZERO(&wf);
            FD_SET(s_sock, &wf);
            if (select(s_sock + 1, NULL, &wf, NULL, &tv) > 0) {
                int err = 0;
                socklen_t el = sizeof(err);
                getsockopt(s_sock, SOL_SOCKET, SO_ERROR, &err, &el);
                cr = err ? -1 : 0;
                why = err;
            } else {
                cr = -1;    /* timed out or select error -> treat as failure */
                why = -1;   /* our own bound, not an lwIP error */
            }
        }
        fcntl(s_sock, F_SETFL, fl);   /* back to blocking for HI/ECHO + REQ */
        if (cr != 0) {
            printf("[LP-TL] connect %s:%u failed fd=%d err=%d\r\n",
                   s_host, (unsigned)s_port, s_sock, why);
            /* err=-1 is OUR select() bound, i.e. the SYN got no answer. Did it
             * even reach the TX path? site[1] is the ordinary push. */
            tickless_pcksite_dump("connfail");
            /* Name the resource, not just the failure. Bench 2026-07-30: two
             * episodes ran ~130 connect failures each, and the errno flipped
             * from -1 (our own select() bound -- a real timeout) to 12 = ERR_MEM
             * mapped through err_to_errno, after which every connect failed
             * INSTANTLY (three inside one second) and never recovered until an
             * external reset. tickless_escalate() fired and did NOT clear it,
             * which fits: "MAC reset + re-associate" cannot reclaim an lwIP
             * allocation. So the open question is exactly which pool is empty,
             * and that is one read away -- LWIP_STATS is already on.
             *
             * Print the heap on any failure, and walk the memp pools only on
             * ENOMEM, reporting just the ones with a non-zero err. Naming pools
             * by index rather than by MEMP_* keeps this from depending on the
             * pool set; map the index with the LWIP_*MEMPOOL order in
             * lwip/priv/memp_std.h. */
#if LWIP_STATS && MEM_STATS
            h_fail = (unsigned)lwip_stats.mem.used;
            printf("[LP-TL]   lwip heap used=%u max=%u avail=%u err=%u\r\n",
                   (unsigned)lwip_stats.mem.used, (unsigned)lwip_stats.mem.max,
                   (unsigned)lwip_stats.mem.avail, (unsigned)lwip_stats.mem.err);
#endif
#if LWIP_STATS && MEMP_STATS
            if (why == ENOMEM) {
                for (int mi = 0; mi < MEMP_MAX; mi++) {
                    const struct stats_mem *ms = lwip_stats.memp[mi];
                    if (ms != NULL && ms->err != 0) {
                        printf("[LP-TL]   memp[%d] err=%u used=%u max=%u "
                               "avail=%u\r\n", mi, (unsigned)ms->err,
                               (unsigned)ms->used, (unsigned)ms->max,
                               (unsigned)ms->avail);
                    }
                }
            }
#endif
            close(s_sock);
            s_sock = -1;
#if LWIP_STATS && MEM_STATS
            printf("[LP-TL]   heap open=%u sock=%+d conn=%+d close=%+d "
                   "leak=%+d\r\n", h_open,
                   (int)h_sock - (int)h_open,
                   (int)h_fail - (int)h_sock,
                   (int)lwip_stats.mem.used - (int)h_fail,
                   (int)lwip_stats.mem.used - (int)h_open);
#endif
            return -1;
        }
    }
    send(s_sock, "HI\n", 3, 0);
    if (tickless_recv_poll(buf, sizeof(buf), TICKLESS_RX_POLLS) <= 0) {
        printf("[LP-TL] no ECHO from host\r\n");
        close(s_sock);
        s_sock = -1;
        return -1;
    }
    return 0;
}

/*
 * Wait, bounded, for the STA route to come back. Returns 1 once it is up.
 *
 * The WiFi stack's own re-association is what restores the route, and it needs
 * seconds: re-associate, 4-way, then DHCP. tickless_connect() refuses to spend
 * an attempt without a route (see its comment), so a reconnect round that runs
 * during that window never issues a single connect() -- yet it used to burn the
 * whole TICKLESS_RECONNECT_MAX budget doing nothing, in about as long as the
 * 2 s gap took, and hand the caller a "link DEAD" it then paid
 * TICKLESS_LINK_RETRY_MS for.
 *
 * HW-observed 2026-08-02 on LAB03, one episode end to end:
 *   08:37:00  disconnect, socket dies
 *   08:37:00  "no route (STA down)" -> reconnect 1/2 failed
 *   08:37:05  CTRL-EVENT-CONNECTED -- the link is BACK
 *   08:37:05  "no route" -> reconnect 2/2 failed -> "link DEAD after 2
 *             reconnects" -> waiting 30000ms, in that same second
 *   08:37:35  connect finally retried; host saw it at 08:37:33
 * A 33 s outage of which ~23 s was the flat wait, on a link that had been
 * usable since ~08:37:12 -- and the cycle missed its 45 s wake deadline by
 * 0.3 s. That single episode is the whole of `cycles_missed` in that run.
 *
 * An attempt that never issued a connect() is not a failed reconnect. Waiting
 * for the route spends the same wall-clock where it can actually help, and
 * leaves the outer ladder (and its escalation) for the case this bound proves:
 * the STA really is gone.
 */
static int tickless_wait_route(uint32_t timeout_ms)
{
    uint32_t waited = 0;

    if (net_al_sta_has_route()) {
        return 1;
    }
    printf("[LP-TL] STA down -> waiting up to %lums for the route\r\n",
           (unsigned long)timeout_ms);
    while (waited < timeout_ms) {
        vTaskDelay(pdMS_TO_TICKS(TICKLESS_ROUTE_POLL_MS));
        waited += TICKLESS_ROUTE_POLL_MS;
        if (net_al_sta_has_route()) {
            printf("[LP-TL] route back after %lums\r\n",
                   (unsigned long)waited);
            return 1;
        }
    }
    return 0;
}

/*
 * Keepalive: prove the socket still carries traffic BOTH ways before betting
 * another standby on it, and rebuild it if not.
 *
 * A standby only ends on the host's wake command, so a socket that dies while
 * we are parked strands us in DTIM forever: the STA stays associated (beacons
 * are fine, so the AP-loss path never fires) and the host can no longer reach
 * us to say anything. HW-observed 2026-07-16 — the host saw EHOSTUNREACH, its
 * session ended, and the DUT kept DTIM-cycling on a dead socket until an
 * external reset. TCP's own timeouts do not help: the peer is gone, so nothing
 * ever arrives to tell us.
 *
 * Returns 0 when the link is usable, -1 once TICKLESS_RECONNECT_MAX rebuilds
 * have failed — at which point the caller must stay awake rather than park on
 * a link that cannot wake us.
 */
static int tickless_link_check(void)
{
    char buf[64];
    int n;

    if (s_sock >= 0) {
        /* Drop anything stale first: a leftover "wakeup" would otherwise be
         * read as the keepalive reply and mask a dead link. */
        while ((n = recv(s_sock, buf, sizeof(buf) - 1, MSG_DONTWAIT)) > 0) {
            ;
        }
        /* Ride out at least one RTO before calling it dead -- see
         * TICKLESS_KEEPALIVE_TRIES. A silent HI is a lost packet far more often
         * than a lost peer, and TCP retransmits the very segment we are waiting
         * on, so the cure is to keep waiting rather than to rebuild. */
        for (uint32_t a = 1; a <= TICKLESS_KEEPALIVE_TRIES; a++) {
            int sn = send(s_sock, "HI\n", 3, 0);

            if (sn != 3) {
                printf("[LP-TL] ka try%lu send=%d errno=%d\r\n",
                       (unsigned long)a, sn, errno);
                break;                  /* socket refuses TX: really gone */
            }
            n = tickless_recv_poll(buf, sizeof(buf), TICKLESS_KEEPALIVE_POLLS);
            if (n < 0) {
                /* A FIN or a socket error, NOT a lost packet: retrying cannot
                 * help and the two have different culprits, so name which. */
                printf("[LP-TL] ka try%lu %s errno=%d\r\n", (unsigned long)a,
                       rxp_why_str(), s_rxp_errno);
                break;
            }
            if (n == 0) {
                printf("[LP-TL] ka try%lu timeout\r\n", (unsigned long)a);
                /* send() took the bytes but nothing came back and nothing
                 * errored. Ask TCP why it is not putting them on the wire. */
                net_al_tcp_dump("ka");
                continue;
            }
            if (strstr(buf, "ECHO") != NULL) {
                if (a > 1) {
                    /* The single-attempt build would have torn the link down
                     * here. Count them: this is the false-death rate. */
                    s_ka_late++;
                    printf("[LP-TL] keepalive ECHO on try %lu\r\n",
                           (unsigned long)a);
                }
                return 0;
            }
        }
        printf("[LP-TL] keepalive got no ECHO -> link dead, reconnecting\r\n");
        close(s_sock);
        s_sock = -1;
    }
    for (uint32_t i = 1; i <= TICKLESS_RECONNECT_MAX; i++) {
        /* Spend attempts on connects, not on a missing route -- see
         * tickless_wait_route(). Conceding here rather than looping keeps the
         * outer ladder's pacing and escalation exactly as they were: this bound
         * only replaces the part of the round that used to be spent failing
         * instantly. */
        if (!tickless_wait_route(TICKLESS_LINK_RETRY_MS)) {
            printf("[LP-TL] no route after %lums -> STA still down\r\n",
                   (unsigned long)TICKLESS_LINK_RETRY_MS);
            return -1;
        }
        if (tickless_connect() == 0) {
            /* Only a REBUILD is a reconnect. The initial setup comes through
             * this same loop (that is the point — one retry budget for both),
             * so it must not report itself as one or the counter lies. */
            if (s_ever_linked) {
                s_reconnects++;
            }
            printf("[LP-TL] %s on attempt %lu\r\n",
                   s_ever_linked ? "re-connected" : "connected",
                   (unsigned long)i);
            s_ever_linked = 1;
            return 0;
        }
        printf("[LP-TL] reconnect %lu/%u failed\r\n",
               (unsigned long)i, TICKLESS_RECONNECT_MAX);
        vTaskDelay(pdMS_TO_TICKS(TICKLESS_RECONNECT_GAP_MS));
    }
    printf("[LP-TL] link DEAD after %u reconnects\r\n", TICKLESS_RECONNECT_MAX);
    return -1;   /* caller's policy decides: one-shot exits, repeat retries */
}

/*
 * One standby cycle: announce PS, let the idle hook DTIM-cycle in PDS, and
 * service WIFI wakes until the host's wake command arrives.
 *
 * Returns 1 when the wake command was consumed (cycle complete — the ONLY
 * success exit), 2 when the standby guard expired with no wake command, 3 on
 * beacon-loss give-up (AP loss), 4 when the wake socket died mid-standby
 * (FIN/RST drained), 0 on `lp tickless stop`, <0 on PS failure.
 * Collapsing all three wake-command sites (pre-arm drain, post-wake drain,
 * re-arm drain) onto one return is what lets the caller loop without a special
 * case per site. This only REPORTS how standby ended; whether that is fatal is
 * the caller's policy, which differs between one-shot and repeat mode.
 */
static int tickless_standby_cycle(void)
{
    char buf[128];
    /* Guard deadline, ABSOLUTE from arm. It must not be a per-wait timeout:
     * dataless TIM wakes notify this task every few seconds and would reset a
     * relative one forever. repeat mode only — a one-shot caller (soak_runner)
     * drives wakes on its own schedule, which may legitimately run far past
     * the delay we requested, and must keep waiting indefinitely. */
    const uint32_t guard_ms = s_delay_s * 1000u + TICKLESS_GUARD_MARGIN_MS;
    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(guard_ms);

    int arm = tickless_ps_arm();
    if (arm < 0) {
        printf("[LP-TL] PS enter failed — cannot arm\r\n");
        return -1;
    }
    if (arm > 0) {
        /* Host wake command already on the socket (tiny delay_s): honor it
         * instead of arming — mirror the drain-side disarm. */
        wifi_mgmr_sta_ps_exit();
        printf("[LP-TL] wake command before arm -> cycle done\r\n");
        return 1;
    }
    s_pds_entries = 0;
    s_wifi_wakes = 0;
    s_ps_active = 1;                /* static PS announced above */
    s_en = 1;
    s_ka_pending = 0;
    s_ka_sent = 0;
    s_ka_due = xTaskGetTickCount() + pdMS_TO_TICKS(TICKLESS_KEEPALIVE_MS);
    /* DO NOT call wifi_mgmr_sta_autoconnect_enable() here: repeat mode already
     * enables it once in lp_tickless_start(), so a call here changes no state
     * and only costs a supplicant round-trip right after entering PS. */
    printf("[LP-TL] ARMED: dtim=%lums, host %s:%u wakes us in ~%lus; "
           "idle now enters DTIM PDS\r\n",
           (unsigned long)s_dtim_ms, s_host, (unsigned)s_port,
           (unsigned long)s_delay_s);
    /* Enter standby with peers' ARP cache already warm; the keepalive refresh
     * only starts one TICKLESS_KEEPALIVE_MS later. */
    HEAP_ACC(s_heap_acc_arp, net_al_sta_arp_announce());

    int span = -1;                  /* post-wait branch whose span is open */

    while (s_en || s_pause) {
        HEAP_SPAN_CLOSE(span);
        span = -1;
        TickType_t wait = pdMS_TO_TICKS(TICKLESS_SOCK_CHECK_MS);
        if (s_repeat != 0u) {
            TickType_t now = xTaskGetTickCount();
            if (now >= deadline) {
                s_en = 0;
                s_pause = 0;
                wifi_mgmr_sta_ps_exit();
                printf("[LP-TL] no wake command in %lums -> standby guard "
                       "fired after %lu PDS entries\r\n",
                       (unsigned long)guard_ms,
                       (unsigned long)s_pds_entries);
                return 2;
            }
            if (deadline - now < wait) {
                wait = deadline - now;
            }
        }
        uint32_t notified;
        HEAP_ACC(s_heap_acc_wait, notified = ulTaskNotifyTake(pdTRUE, wait));
        HEAP_SPAN_OPEN();
        if (notified == 0 && !s_pause) {
            /* Genuine idle timeout with NO wake pending (s_pause clear).
             *
             * The `&& !s_pause` is load-bearing: s_pause is the shared truth,
             * the notify is only a prompt. The hook sets s_pause=1 immediately
             * before vTaskNotifyGiveFromISR (both TIM and keepalive). If that
             * notify races this ulTaskNotifyTake timeout and the timeout wins,
             * the task returns 0 AND ulTaskNotifyTake(pdTRUE) clobbers the
             * just-arrived notification to 0 on exit — the wake is lost. Acting
             * on the notify alone would then health-check + continue, leaving
             * s_pause stuck at 1: the hook WFI-early-returns forever (no more
             * PDS, no more notifies) and the idle task spins. JTAG-caught
             * 2026-07-22: PC=0x13007778, s_pause=1, s_ka_pending=0 (a lost
             * TIM notify), MTH=0/MIE=1 (no masking) — a pure handoff race, not
             * a Nuclei port issue. Gating on s_pause makes a lost/raced notify
             * self-heal: the next timeout falls through to the drain below.
             *
             * Periodic standby socket health check — no TX, no PS exit
             * (lwIP socket state is local). This is the ONLY detector for
             * anything the peer sent while the host happened to be awake
             * between blob cycles: lwIP consumes it silently, no frame stays
             * buffered at the AP, so no TIM wake will ever announce it.
             * HW-observed 2026-07-22: a FIN landing in that gap EOF'd the
             * socket with zero console evidence and the DUT standby-cycled
             * on the dead link; a WAKE landing there would be lost the same
             * way. Timeout ticks only advance in the awake gaps (catch-up
             * replay), so this adds no extra wakes. */
            int pn = recv(s_sock, buf, sizeof(buf) - 1, MSG_DONTWAIT);
            if (pn > 0) {
                buf[pn] = '\0';
                printf("[LP-TL] host msg (idle-gap delivery) after %lu PDS "
                       "entries: %s\r\n", (unsigned long)s_pds_entries, buf);
                if (tickless_is_wake(buf)) {
                    s_en = 0;
                    s_pause = 0;
                    wifi_mgmr_sta_ps_exit();
                    printf("[LP-TL] wake command -> tickless DISARMED\r\n");
                    return 1;
                }
            } else if (pn == 0 || (errno != EWOULDBLOCK && errno != EAGAIN)) {
                s_en = 0;
                s_pause = 0;
                wifi_mgmr_sta_ps_exit();
                printf("[LP-TL] socket died in standby (after %lu PDS "
                       "entries) -> reconnect\r\n",
                       (unsigned long)s_pds_entries);
                return 4;
            }
            span = HEAP_SPAN_GAP;
            continue;                   /* healthy idle: keep standing by */
        }
        if (!s_en && !s_pause) {
            return 0;                           /* lp tickless stop */
        }
        /* Beacon-loss give-up (P3.3): the blob missed LP_FW_LOSS_RECONNECT_CAP
         * consecutive beacons — the AP is gone or we drifted out of sync.
         * End standby and stay awake so the WiFi stack's own supervision
         * detects the disconnect and reconnects; re-arming PS toward a dead
         * AP would just DTIM-cycle deaf forever. */
        if (IOT2LP_PARA->wakeup_reason & LP_FW_WAKEUP_AP_LOSS) {
            s_en = 0;
            s_pause = 0;
            s_ka_pending = 0;
            wifi_mgmr_sta_ps_exit();    /* best-effort PM=0 */
            printf("[LP-TL] AP loss after %lu PDS entries -> tickless "
                   "DISARMED, staying awake for reconnect\r\n",
                   (unsigned long)s_pds_entries);
            return 3;
        }
        if (s_ka_pending) {
            /* Keepalive-only wake (no TIM). MINIMAL: TX an L3 frame while the
             * WiFi task is live (s_pause held by the hook), then straight back
             * to standby. NO PS exit/re-announce — static PS is still in
             * effect and a PS STA TXes data fine, so tracked beacon reception
             * is undisturbed (the full ps_exit + 3 s recv + ps_arm drain DID
             * disturb it -> wake TIM beacons missed). The frame is forwarded
             * DUT -> AP -> gateway -> host, refreshing the gateway neigh + NAT.
             * The short drain gives the WiFi task CPU to push the frame and
             * catches a wake that raced in. */
            s_ka_pending = 0;
            s_ka_sent++;
            /* Keep peers' ARP cache for us warm. DTIM standby drops ~42% of
             * broadcast frames (the lp_fw blob ignores the TIM group bit), so
             * a peer whose ARP entry aged out cannot re-resolve us by broadcast
             * ARP and gives up on the flow — the "AP reaped my socket" symptom
             * that is actually ours. We are already awake for the keepalive, so
             * announcing here costs no extra wake. */
            HEAP_ACC(s_heap_acc_arp, net_al_sta_arp_announce());
            int sn;
            HEAP_ACC(s_heap_acc_snd, sn = send(s_sock, "KA\n", 3, MSG_DONTWAIT));
            int kn = -1;
            if (sn == 3) {
                HEAP_ACC(s_heap_acc_drain,
                         kn = tickless_recv_poll(buf, sizeof(buf),
                                                 TICKLESS_KA_DRAIN_POLLS));
            }
            if (kn > 0 && tickless_is_wake(buf)) {
                s_en = 0;
                s_pause = 0;
                wifi_mgmr_sta_ps_exit();
                printf("[LP-TL] wake command (keepalive drain) -> "
                       "tickless DISARMED\r\n");
                return 1;
            }
            if (sn != 3 || kn < 0) {
                s_en = 0;
                s_pause = 0;
                /* Every other standby exit restores PM=0; this one used to
                 * leave static PS announced, so the reconnect loop that
                 * follows ran with the AP still buffering our inbound. */
                wifi_mgmr_sta_ps_exit();
                printf("[LP-TL] keepalive link dead (after %lu PDS entries) "
                       "-> reconnect\r\n", (unsigned long)s_pds_entries);
                return 4;
            }
            /* NOTE, so nobody re-derives it: kn == 0 here is NORMAL, not a link
             * fault. The keepalive payload is "KA", and the wake server only
             * answers lines starting with "HI" (dtim_regr_server.py) -- a KA is
             * just logged as dut_line. So "no bytes in the drain window" says
             * nothing about the link, and bounding standby on it would end every
             * healthy standby after a few keepalives. link_check's keepalive does
             * send "HI", which is why ITS "no ECHO" IS a real signal. */
            s_pause = 0;                 /* resume PDS: standby continues */
            span = HEAP_SPAN_KA;
            continue;
        }
        /* WIFI wake: PM=0 null -> AP flushes what it buffered, then read. */
        wifi_mgmr_sta_ps_exit();
        int n = tickless_recv_poll(buf, sizeof(buf), TICKLESS_RX_POLLS);
        if (n < 0) {
            /* The socket died under us — often fetched by this very TIM wake
             * (a FIN/RST is unicast: it sets our TIM bit like any data).
             * End the cycle so the caller's link_check rebuilds the socket
             * and re-arms; treating it as "no data" put the DUT back to
             * standby on a dead link, unwakeable by the host (HW-observed
             * 15+ min zombie, 2026-07-22). */
            s_en = 0;
            s_pause = 0;
            printf("[LP-TL] socket died in standby (after %lu PDS entries) "
                   "-> reconnect\r\n", (unsigned long)s_pds_entries);
            return 4;
        }
        if (n > 0) {
            printf("[LP-TL] host msg after %lu PDS entries "
                   "(%lu WIFI wakes): %s\r\n",
                   (unsigned long)s_pds_entries,
                   (unsigned long)s_wifi_wakes, buf);
            if (tickless_is_wake(buf)) {
                s_en = 0;
                s_pause = 0;
                printf("[LP-TL] wake command -> tickless DISARMED\r\n");
                return 1;
            }
        } else {
            printf("[LP-TL] WIFI wake without data (race) -> "
                   "back to standby\r\n");
        }
        /* Not the wake command: back to standby. The re-arm drain may
         * itself fish out a WAKE that raced the PS re-announce. */
        int rearm = tickless_ps_arm();
        if (rearm < 0) {
            printf("[LP-TL] PS re-enter failed -> tickless DISARMED\r\n");
            s_en = 0;
            s_pause = 0;
            return -1;
        }
        if (rearm > 0) {
            s_en = 0;
            s_pause = 0;
            wifi_mgmr_sta_ps_exit();
            printf("[LP-TL] wake command (in re-arm drain) -> cycle done\r\n");
            return 1;
        }
        s_pause = 0;
        span = HEAP_SPAN_WAKE;
    }
    return 0;
}

/* Wait for the stack to put us back on the AP. Checks first, so it costs
 * nothing when the association survived (a MAC reset does not deauth). */
static int tickless_wait_assoc(void)
{
    for (uint32_t i = 0; i < TICKLESS_REASSOC_POLLS; i++) {
        if (lpfw_populate_from_sta() == 0) {
            return 0;
        }
        vTaskDelay(pdMS_TO_TICKS(TICKLESS_REASSOC_GAP_MS));
    }
    return -1;
}

/*
 * Recover from a state the ordinary retry budgets could not clear, and keep
 * cycling.
 *
 * Every caller of this used to end the run instead (goto out -> vTaskDelete),
 * which left the DUT awake, out of DTIM, and recoverable only by an external
 * reset — the one thing a deployed device cannot count on. And the give-up was
 * usually aimed at the wrong culprit: a link unusable for the whole budget
 * *while the STA is still associated* is the signature of a MAC/TX wedge, not
 * of a dead host, so conceding to the caller could never have helped.
 *
 * Ask for the KE_EVT_RESET that the AC watchdog's ASSERT_REC path already uses
 * for that exact state, drop the socket the wedge stranded, wait for the link,
 * and let the caller's loop rebuild from scratch. Pacing comes from the budget
 * that got us here (6 x 30 s), so this cannot become a tight loop.
 */
/* pck_site snapshot on demand. During a failure episode NO cycle completes, so
 * the per-cycle pcksite line never prints -- exactly when it matters most. Call
 * it at connect failure and at escalation to see whether the SYN was pushed into
 * the TX path at all (site[1] climbing) or never got there. */
static void tickless_pcksite_dump(const char *tag)
{
    printf("[LP-TL] pcksite/%s %lu %lu %lu %lu %lu cfm=%lu bal=%ld\r\n", tag,
           (unsigned long)txl_pck_site[0], (unsigned long)txl_pck_site[1],
           (unsigned long)txl_pck_site[2], (unsigned long)txl_pck_site[3],
           (unsigned long)txl_pck_site[4], (unsigned long)txl_pck_site[5],
           (long)(txl_pck_site[0] + txl_pck_site[1] + txl_pck_site[2] +
                  txl_pck_site[3] + txl_pck_site[4]) -
           (long)txl_pck_site[5]);
}

static void tickless_escalate(const char *why)
{
    s_escalations++;
    printf("[LP-TL] %s -> MAC reset + re-associate (escalation %lu)\r\n",
           why, (unsigned long)s_escalations);
    /* Escalate is a MAC-layer action; if the heap is at its ceiling here, that
     * is the evidence it cannot fix this failure (bench 2026-07-30: escalate
     * fired twice and the link stayed dead until an external reset). */
    tickless_heap_watch("escalate");
    tickless_pcksite_dump("escalate");
#if LWIP_MEM_TRACE
    /* The most informative moment in the whole failure, and it had no dump: the
     * per-cycle trace only runs when a cycle COMPLETES, and during an episode
     * none does -- so what is holding the full 48 KB at the ceiling was never
     * recorded, only the per-cycle increment. escalate fires once per exhausted
     * retry budget, so it is a bounded place to ask. With live > 24 the per-block
     * listing suppresses itself and only the by-site groups print, which is
     * exactly what is wanted here. */
    mem_trace_dump("ceiling", 8);
#endif

    if (s_sock >= 0) {
        close(s_sock);
        s_sock = -1;
    }
    /* Did the reset actually happen? Bench 2026-07-31: with the ladder fixed,
     * escalation fired 3 times and the link still never came back, which matches
     * the older unexplained observation that the heap does not move across
     * escalate while an ASSERT_REC-driven reset does clear it. dbg_mac_reset_arm()
     * NO-OPS when KE_EVT_RESET is already pending -- it neither arms nor notifies
     * -- so distinguish: armed did not move => the request was swallowed; armed
     * moved but evt stays set => the WiFi task never ran the reset; both moved
     * => the reset ran and was simply not enough. */
    {
        extern volatile uint32_t macsw_mac_reset_armed;
        uint32_t armed0 = macsw_mac_reset_armed;

        dbg_mac_reset_request();
        vTaskDelay(pdMS_TO_TICKS(200));
        printf("[LP-TL] escreset armed %lu->%lu%s\r\n",
               (unsigned long)armed0,
               (unsigned long)macsw_mac_reset_armed,
               (macsw_mac_reset_armed == armed0)
                   ? "  SWALLOWED (KE_EVT_RESET already pending)" : "  armed ok");
    }

    /* NOT a place to enable auto-reconnect: repeat mode already did that in
     * lp_tickless_start(), and the bench confirms it is live here --
     * "[STA] reconnect arm: dis=0" plus attempts at the full 10s/20s/30s
     * backoff, unprompted.
     *
     * A caution for whoever next sees this ladder spin without recovering:
     * 2026-08-01 that was the AP, not us. On LAB03 every attempt reached
     * "state=4WAY_HANDSHAKE" and was deauthenticated reason=1, ~8.7 s in, 51
     * times running -- while the same DUT, seconds later and unchanged,
     * associated with LAB02 first try and got an IP. Repeated MAC resets are
     * what earns that ban (see project_bl616_sae_lab03_bug), so this escalation
     * path can provoke it. Discriminate by switching AP BEFORE concluding the
     * DUT cannot re-associate. */
    if (tickless_wait_assoc() != 0) {
        printf("[LP-TL] not associated %lus after the reset -- cycling anyway\r\n",
               (unsigned long)(TICKLESS_REASSOC_POLLS * TICKLESS_REASSOC_GAP_MS)
               / 1000ul);
    }
    /* A fresh start deserves fresh budgets, or the next escalation fires on
     * the previous incident's leftovers. */
    s_link_giveups = 0;
    s_ap_losses = 0;
}

static void lp_tickless_task(void *arg)
{
    char buf[128];

    (void)arg;

    /* Each pass: get a working link, tell the host when to wake us, hand the
     * CPU budget to the idle hook, and stand by until its wake command comes
     * back. One-shot (s_repeat == 0) leaves after the first cycle, so its only
     * pass through link_check is the initial connect; regression mode stays
     * awake for the duty-cycle window and re-verifies before every re-arm.
     * Never park on an unverified link — that is what strands us in DTIM.
     *
     * The first connect goes through link_check like any other: it is just the
     * case where there is no socket yet, and it has no business being less
     * robust than a rebuild. A dropped HI used to fail the whole run here
     * (HW-observed 2026-07-16, "no ECHO from host" 3 s after a fine connect). */
    for (;;) {
        tickless_heap_mark(HEAP_MARK_START);
        if (tickless_link_check() != 0) {
            /* The host was unreachable for the whole reconnect budget. In
             * one-shot mode that ends the run. In repeat mode it is almost
             * always a transient TX/link outage — the STA is still associated —
             * so retry rather than abandoning the duty cycle: when the outage
             * clears the next link_check reconnects and cycling resumes with NO
             * external reset. Bounded, so a genuinely dead host eventually lets
             * us stop instead of retrying awake forever. */
            if (s_repeat == 0u) {
                goto out;
            }
            if (++s_link_giveups > TICKLESS_LINK_GIVEUP_MAX) {
                tickless_escalate("connects failing -> MAC reset before more of the same");
                /* Re-arm the ladder instead of escalating on every round from
                 * here on. Without this the counter stays over the limit and
                 * every subsequent failure fires another reset -- the storm
                 * pattern PM48 already proved is iatrogenic. */
                s_link_giveups = 0;
                continue;
            }
            printf("[LP-TL] host link retry %lu/%u -> waiting %lums\r\n",
                   (unsigned long)s_link_giveups,
                   (unsigned)TICKLESS_LINK_GIVEUP_MAX,
                   (unsigned long)TICKLESS_LINK_RETRY_MS);
            tickless_heap_watch("retry");     /* the RISE side */
            /* Same rule as the reconnect ladder, one layer up: wait for what
             * the round is actually blocked on.
             *
             * Route UP means the STA is fine and the HOST refused us, so the
             * flat period IS the point -- retrying a dead host sooner just
             * spins. Route DOWN means every millisecond after it returns is
             * wasted, and this delay used to burn them: measured 2026-08-02
             * with the ladder fix already in, the route came back 13:18:48 and
             * this sleep held the DUT until 13:19:16 -- 28 s, the very defect
             * the ladder fix removed, surviving one level higher. */
            if (net_al_sta_has_route()) {
                vTaskDelay(pdMS_TO_TICKS(TICKLESS_LINK_RETRY_MS));
            } else {
                tickless_wait_route(TICKLESS_LINK_RETRY_MS);
            }
            continue;
        }
        s_link_giveups = 0;     /* a working link resets the retry budget */
        tickless_heap_watch("linkok");         /* the FALL side */
        tickless_heap_mark(HEAP_MARK_LINKOK);
        int len = snprintf(buf, sizeof(buf), "REQ %lu\n",
                           (unsigned long)s_delay_s);
        if (send(s_sock, buf, len, 0) != len) {
            printf("[LP-TL] REQ send failed -> dropping link\r\n");
            close(s_sock);
            s_sock = -1;
            if (s_repeat == 0u) {
                goto out;
            }
            continue;       /* next pass rebuilds the socket */
        }
        if (lpfw_populate_from_sta() != 0) {
            printf("[LP-TL] not associated — cannot arm\r\n");
            if (s_repeat == 0u) {
                goto out;
            }
            /* The socket came up, so we were associated moments ago; the link
             * went while we were asking for the cycle. Wait for the stack, and
             * escalate only if it cannot get us back on its own. */
            if (tickless_wait_assoc() != 0) {
                tickless_escalate("still not associated when arming");
            }
            continue;
        }
        int r = tickless_standby_cycle();
        tickless_heap_mark(HEAP_MARK_STANDBY);
        if (r == 2) {
            /* Guard fired: the host never woke us. The link is the prime
             * suspect, so loop back and let the keepalive settle it. */
            s_guard_fires++;
            continue;
        }
        if (r == 4) {
            /* Socket died mid-standby (FIN/RST drained by a TIM wake). The
             * next pass's link_check rebuilds it and re-arms — one-shot and
             * repeat alike: a dead link means the requested wake can never
             * arrive, so "keep waiting" would be the zombie, not fidelity
             * to the caller's schedule. */
            continue;
        }
        if (r == 3 && s_repeat != 0u) {
            /* Beacon-loss give-up. The RF link died the way the socket can,
             * and it deserves the same treatment: retry, then quit for good.
             * Exiting on the first loss would end the run on any AP hiccup —
             * a channel change is routine — while retrying forever would
             * DTIM-cycle deaf at full power against an AP we cannot hear
             * (HW-observed 2026-07-16: LAB02 moved ch2 -> ch4 and every
             * standby came back 100% beacon-loss, so re-arming was hopeless).
             * The stack's own supervision does the reconnecting; we wait for
             * it rather than race it, since link_check's ~10 s reconnect
             * budget would burn out while the STA is still scanning. */
            if (++s_ap_losses > TICKLESS_AP_LOSS_MAX) {
                /* Repeated beacon-loss give-ups with the stack unable to get
                 * us back: same treatment as a dead link, since a wedged RX
                 * looks exactly like an absent AP from here. Never stop — an
                 * AP that is genuinely gone comes back, and we have to be
                 * cycling when it does. */
                tickless_escalate("AP unreachable across consecutive losses");
                continue;
            }
            printf("[LP-TL] AP loss %lu/%u -> waiting for the stack to "
                   "re-associate\r\n", (unsigned long)s_ap_losses,
                   TICKLESS_AP_LOSS_MAX);
            (void)tickless_wait_assoc();
            /* Re-association means a fresh DHCP lease, so the old socket is
             * bound to an address we no longer own. Drop it; link_check
             * rebuilds against the new one. */
            if (s_sock >= 0) {
                close(s_sock);
                s_sock = -1;
            }
            continue;
        }
        if (r == 0 || s_repeat == 0u) {
            goto out;       /* `lp tickless stop`, or one-shot's single cycle */
        }
        if (r != 1) {
            /* PS enter/re-enter failed (r < 0): the control plane refused the
             * mode change, which on this part means ME is wedged or the vif
             * went while we were arming. Nothing above cycles us again, so
             * escalate rather than end the run. */
            tickless_escalate("PS transition failed");
            continue;
        }
        s_cycles_done++;
        s_ap_losses = 0;    /* a clean cycle proves the AP is usable again */
        tickless_heap_watch("cycle");          /* baseline between episodes */
        tickless_heap_mark(HEAP_MARK_END);
        tickless_heap_segments();              /* which phase grew, per cycle */
        tickless_heap_inner();                 /* and which op inside standby */
        printf("[LP-TL] CYCLE %lu/%lu done (entries=%lu, wifi_wakes=%lu)\r\n",
               (unsigned long)s_cycles_done, (unsigned long)s_repeat,
               (unsigned long)s_pds_entries, (unsigned long)s_wifi_wakes);
        if (s_repeat == 0u || s_cycles_done >= s_repeat) {
            printf("[LP-TL] run complete -> staying awake\r\n");
            goto out;
        }
        /* Duty cycle: a periodic app is awake doing work between standbys.
         * Stay at full power for the window before re-arming. */
        printf("[LP-TL] awake %lums before next cycle\r\n",
               (unsigned long)s_awake_ms);
        vTaskDelay(pdMS_TO_TICKS(s_awake_ms));
    }

out:
    if (s_sock >= 0) {
        close(s_sock);
        s_sock = -1;
    }
    printf("[LP-TL] service task exit (cycles=%lu, entries=%lu, "
           "wifi_wakes=%lu, reconnects=%lu, ka_late=%lu, guard_fires=%lu, "
           "ap_losses=%lu, link_retries=%lu, escalations=%lu)\r\n",
           (unsigned long)s_cycles_done, (unsigned long)s_pds_entries,
           (unsigned long)s_wifi_wakes, (unsigned long)s_reconnects,
           (unsigned long)s_ka_late,
           (unsigned long)s_guard_fires, (unsigned long)s_ap_losses,
           (unsigned long)s_link_giveups, (unsigned long)s_escalations);
    s_task = NULL;
    vTaskDelete(NULL);
}

int lp_tickless_start(const char *host, uint16_t port, uint32_t delay_s,
                      uint32_t dtim_ms, uint32_t repeat, uint32_t awake_s)
{
    if (s_task != NULL || s_en) {
        printf("[LP-TL] already running\r\n");
        return -1;
    }
    /* Seed plumbing mode: `lp tickless seed <entries> [dtim_ms]` — caller
     * must have run `lp seed` first. No socket, no PS handshake, no service
     * task: pure hook/budget/tick-compensation validation, self-disarming
     * after <entries> (delay_s reused as the cap). */
    if (strcmp(host, "seed") == 0) {
        s_dtim_ms = dtim_ms ? dtim_ms : 100;
        s_seed_max = delay_s ? delay_s : 20;
        s_pds_entries = 0;
        s_wifi_wakes = 0;
        s_pause = 0;
        s_ps_active = 0;            /* no association, no PS dance */
        s_en = 1;
        printf("[LP-TL] SEED-ARMED: dtim=%lums, auto-disarm after %lu PDS "
               "entries\r\n", (unsigned long)s_dtim_ms,
               (unsigned long)s_seed_max);
        return 0;
    }
    s_seed_max = 0;
    strncpy(s_host, host, sizeof(s_host) - 1);
    s_host[sizeof(s_host) - 1] = '\0';
    s_port = port;
    s_delay_s = delay_s;
    s_dtim_ms = dtim_ms ? dtim_ms : 100;
    s_repeat = repeat;
    s_awake_ms = (awake_s ? awake_s : 60u) * 1000u;
    s_cycles_done = 0;
    s_reconnects = 0;
    s_ka_late = 0;
    s_guard_fires = 0;
    s_ever_linked = 0;
    s_ap_losses = 0;
    s_link_giveups = 0;
    s_escalations = 0;
    s_en = 0;
    s_pause = 0;
    if (repeat != 0u) {
        /* Repeat mode has to survive an AP loss unattended, and the AP-loss
         * path can only "stay awake for reconnect" if something actually
         * reconnects. Nothing does by default: wifi_mgmr.c inits
         * disable_autoreconnect = 1 and no caller in this app ever turns it
         * back on, so after a loss the supplicant just logs BSS-REMOVED and
         * sits there (HW-verified 2026-07-16: 180 s, no scan, no attempt).
         * One-shot mode is left alone — its caller drives association. */
        if (wifi_mgmr_sta_autoconnect_enable() != 0) {
            printf("[LP-TL] warning: could not enable STA autoconnect; an AP "
                   "loss will end the run\r\n");
        }
    }
    if (xTaskCreate(lp_tickless_task, "lp_tl", 2048, NULL,
                    tskIDLE_PRIORITY + 2, &s_task) != pdPASS) {
        printf("[LP-TL] task create failed\r\n");
        s_task = NULL;
        return -1;
    }
    return 0;
}

int lp_tickless_stop(void)
{
    printf("[LP-TL] hook stats: calls=%lu, max_expected_idle=%lu ticks, "
           "entries=%lu\r\n", (unsigned long)s_dbg_calls,
           (unsigned long)s_dbg_max_expected, (unsigned long)s_pds_entries);
    if (s_task == NULL && !s_en) {
        printf("[LP-TL] not running\r\n");
        return -1;
    }
    s_en = 0;
    s_pause = 0;
    if (s_task != NULL) {
        /* The STA is still parked in static PS: exit here in shell-task
         * context (blocking OK) so the DUT is reachable again. */
        wifi_mgmr_sta_ps_exit();
        xTaskNotifyGive(s_task);
    } else {
        printf("[LP-TL] DISARMED (seed mode)\r\n");
    }
    return 0;
}

/*
 * WiFi-disconnect hook (app event context). A deauthed/disassociated STA can
 * never receive the host's TCP wake command: left armed, standby cycles
 * DTIM-deaf forever while the AP has already dropped us (zombie standby,
 * observed live — the AP kicked the PM=1 STA mid-standby and the WAKE was
 * undeliverable). Kick the service task; its drain finds the PS re-announce
 * failing on the dead association and disarms through the existing
 * "PS re-enter failed" path, leaving the system awake for reconnect.
 */
void lp_tickless_on_disconnect(void)
{
    if (s_en && s_seed_max == 0u && s_task != NULL) {
        printf("[LP-TL] WiFi disconnected while armed -> waking service task "
               "to disarm\r\n");
        xTaskNotifyGive(s_task);
    }
}

/*
 * Replay banked oversleep into the kernel clock. Called from
 * vApplicationIdleHook — the idle task with the scheduler LIVE, the one
 * context where xTaskCatchUpTicks is legal (it asserts inside the
 * suspended-scheduler tickless hook where the deficit is banked). Both
 * writer and reader run in the idle task, so no locking is needed.
 */
void lp_tickless_tick_catchup(void)
{
    /* Is this hook even reached while tickless is ARMED? The PDS-entry TX flush
     * (reverted, see lp_demo.c) parked confirmations here for draining on the
     * assumption that it is -- an assumption never verified, and the most likely
     * reason the flush left macif's tx_in_reset latched and made things worse.
     * Counting it makes the precondition measurable before anyone retries. */
    s_catchup_calls++;

    uint32_t d = s_tick_deficit_ms;
    if (d != 0u) {
        s_tick_deficit_ms = 0u;
        xTaskCatchUpTicks(pdMS_TO_TICKS(d));
    }
    /* Reclaim a control plane wedged by an orphaned PS-mode transition
     * (ME_BUSY forever -> stuck SM_DISCONNECTING -> "Association request to the
     * driver failed"). The PDS-resume net misses it when the idle path aborts
     * on !me_is_idle and never sleeps; this idle pass runs regardless. */
    if (rwnxl_control_plane_wdog()) {
        printf("[LP-TL] control plane wedged (ME_BUSY orphan) -> reclaimed\r\n");
    }
    /* Drain vehicle for a future PDS-entry flush: with the scheduler live this is
     * the first legal point to push parked confirmations through to
     * fhost_tx_release_buf. Currently never armed -- nothing sets the flag since
     * the flush was reverted -- but kept so the retry has a working drain and,
     * with s_catchup_calls above, a way to prove this hook runs first. */
    if (g_lp_tx_cfm_drain_pending) {
        g_lp_tx_cfm_drain_pending = 0u;
        macif_tx_reset_set(false);
        macif_tx_cfm_drain();
    }

    /* Re-arm any AC watchdog that died while its queue still has frames in
     * flight. Without it the recover_stale/PM45 self-heal chain is
     * unschedulable and only a power-cycle recovers (bench 2026-07-20).
     *
     * txl_watchdog_backstop_poll() already exists for this, but its only
     * caller is the sdiowifi keep-ready task, which is not built here --
     * defconfig_ipro6_lpwifi has no SDIO transceiver, so the backstop was
     * dead code in this configuration. The idle hook is if anything a better
     * host: during a TX wedge every other task is blocked, so the idle task
     * is precisely the context still running.
     *
     * Rate-limited to the same ~500 ms the sdiowifi caller uses; the poll
     * briefly masks interrupts to arm a timer, so it should not run on every
     * idle pass. */
    {
        static TickType_t last_backstop;
        TickType_t now = xTaskGetTickCount();

        if ((TickType_t)(now - last_backstop) >= pdMS_TO_TICKS(500)) {
            last_backstop = now;
            txl_watchdog_backstop_poll();
        }
    }
}

#else /* !(CONFIG_WIFI6 && CONFIG_LWIP && CONFIG_IPRO_WIFI_PS && tickless) */

int lp_tickless_start(const char *host, uint16_t port, uint32_t delay_s,
                      uint32_t dtim_ms, uint32_t repeat, uint32_t awake_s)
{
    (void)host; (void)port; (void)delay_s; (void)dtim_ms;
    (void)repeat; (void)awake_s;
    printf("[LP-TL] not available (needs WIFI6+LWIP+IPRO_WIFI_PS+"
           "CONFIG_FREERTOS_USE_TICKLESS_IDLE)\r\n");
    return -1;
}

int lp_tickless_stop(void)
{
    printf("[LP-TL] not available\r\n");
    return -1;
}

void lp_tickless_on_disconnect(void)
{
}

void lp_tickless_tick_catchup(void)
{
}

#endif
