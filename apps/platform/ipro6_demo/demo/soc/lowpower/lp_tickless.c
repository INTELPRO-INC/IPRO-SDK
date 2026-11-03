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
 * reference-SDK-style usage (its lp demo tickless.c): once ARMED, every
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
#include "lp_wake_matcher.h"
#if defined(CONFIG_IPRO6) && defined(CONFIG_USE_MACSW) && defined(CONFIG_LWIP_LP) && !IPRO_WIFI_LP_FW
#include "lp_rx_witness.h"
#define LP_RXW_DIAG 1
#endif

#if defined(CONFIG_WIFI6) && defined(CONFIG_LWIP) && \
    defined(CONFIG_IPRO_WIFI_PS) && defined(CONFIG_FREERTOS_USE_TICKLESS_IDLE)

#include <sys/socket.h>
#include <sys/time.h>      /* struct timeval for SO_SNDTIMEO */
#include <errno.h>
#include <FreeRTOS.h>
#include <task.h>
#include "hal_lp.h"
#include "hal_clock.h"  /* clock_xtal32k_adopt, clock_rc32k_coarse_cal */
#include "drv_hbn.h"    /* HBN_32K_Sel, HBN_Trim_RC32K: sleep-clock resolution */
#include "lp_fw.h"      /* IOT2LP_PARA / LP_FW_WAKEUP_AP_LOSS */
/* After the FreeRTOS headers on purpose: pulled in ahead of them, lwipopts.h
 * shadows the port's TaskHandle_t/TickType_t definitions and the file stops
 * compiling. */
#include "lwip/stats.h" /* lwip_stats: name the pool behind an ERR_MEM */
#include "lwip/memp.h"  /* MEMP_MAX */
#include "lwip/mem.h"   /* mem_trace_dump: what is still LIVE, and who took it */
#include "lwip/tcpip.h" /* LOCK_TCPIP_CORE */
#include "lwip/timeouts.h" /* tcp_timer_fast_interval_set */

extern int wifi_mgmr_sta_ps_enter_static(void);
extern int wifi_mgmr_sta_ps_exit(void);
extern int wifi_mgmr_sta_autoconnect_enable(void);
extern int wifi_mgmr_sta_tx_ac_min_set(int ac);
extern int wifi_mgmr_sta_tx_ac_min_get(void);
/* macsw (rwnx.c): reclaim a control plane wedged in ME_BUSY by an orphaned
 * PS-mode transition. Safe no-op when ME is idle; runs from the idle hook.
 * Returns 1 on the pass that breaks a wedge. */
extern int rwnxl_control_plane_wdog(void);
/* Same reclaim with the timing decision left to the caller: rwnx.c times the
 * wedge with hal_machw_time(), which lagged 10x during the 2026-08-07 wedge.
 * The sleep-gate refusal streak is a trustworthy OS-tick clock, so that path
 * owns the deadline and calls this. Returns 1 if a BUSY ME was reclaimed. */
extern int rwnxl_control_plane_reclaim_if_busy(void);
/* Bench scaffold: force ME into BUSY to reproduce the wedge on demand. */
extern void rwnxl_control_plane_force_busy(void);
/* How long the sleep gate may keep refusing with ME BUSY before that ME<->MM
 * transaction is declared orphaned. Matches rwnx.c's own RWNX_ME_ORPHAN_US
 * bound (a legitimate transition is a few ms); only the clock differs. */
#define TICKLESS_ME_ORPHAN_MS   2000u
/* AC-watchdog backstop; its usual caller (sdiowifi keep-ready) is not built in
 * the no-SDIO low-power configuration, so the idle hook drives it here. */
extern void txl_watchdog_backstop_poll(void);
extern int net_al_sta_arp_announce(void);
extern int net_al_sta_arp_announce_to(const char *peer);
extern int net_al_sta_arp_reply_nexthop(const char *peer);
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
#define TICKLESS_RX_POLLS       30   /* 3 s: connect-time HI/ECHO round-trip */
/* P4 (2026-08-26): the post-PM=0 flush window on a TIM wake. Was the shared
 * 3 s constant above -- but a race wake (TIM set by a frame lwIP consumes
 * silently: ARP, hostapd poll, broadcast) delivers nothing to the socket and
 * paid all 30 polls fully awake (~3.5 s with the re-arm, ~450 mJ, ~130 uA
 * amortized over one DTIM-10 cycle). The host's genuine WAKE command is the
 * very frame that set the TIM bit, so it arrives with the flush in the first
 * polls; the only loss case is an air-dropped TCP segment, whose retransmit
 * sets TIM again and gets a fresh drain one DTIM cycle later -- retry by
 * TIM, latency +<=2 cycles worst case, no lost wakes. */
#define TICKLESS_WAKE_DRAIN_POLLS 5  /* 0.5 s */
#define TICKLESS_WAKE_DRAIN_STEP_MS 10  /* TIM drain poll once the PM=0 null is out */
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
/* One-shot mode, host unreachable after the link was up: retry for ever,
 * parked in DTIM standby between attempts, on this backoff (last repeats). */
static const uint32_t s_host_backoff_ms[] = { 5000u, 10000u, 30000u, 60000u, 300000u };
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
#define TICKLESS_KEEPALIVE_MS    15000 /* standby keepalive TX cadence — keep a
                                        * router's ARP/neigh for us + the NAT
                                        * map fresh so a buffered wake is not
                                        * dropped at L3->L2 (HW root cause of
                                        * the tracked ~50% miss). 30 s was
                                        * outside every reaper: LAB03 deauths
                                        * idle STAs at ~21 s and a Linux
                                        * neigh entry decays reachable->stale
                                        * in ~30 s; 15 s keeps both fresh
                                        * (HW 2026-08-04: a 6-min standby
                                        * with zero keepalives lost its ARP
                                        * entry, then hostapd reaped the STA
                                        * at 300 s — the host wake TCP frame
                                        * died in the sender's ARP queue). */
#define TICKLESS_KA_DRAIN_POLLS  5   /* ~0.5 s: let the WiFi task push the KA   */
/*
 * Keepalive exit-on-ACK (2026-09-07). The wake server never answers a KA (it
 * answers "HI" only) and a TCP ACK is invisible to recv(), so the KA drain
 * above always ran its full 0.5 s, and TICKLESS_ACK_SETTLE_MS then waited
 * another 0.5 s for an ACK that -- with PM=0 held across the send -- had
 * landed within tens of ms (Linux delayed-ACK <= 40 ms): ~1.0 s awake at
 * ~55 mA per keepalive, measured (KA_WIT3). The reference SDK
 * (its sdiowifi demo main.c test_tcp_keepalive) has no drain at all: it
 * writes and lets the idle hook sleep, and the LP lwIP's on-demand tcp timer
 * bounds that sleep to the retransmit deadline while the segment is unacked.
 * Same predicate here, read closed-loop via lwip_sock_tx_pending(): hold
 * PM=0 only until the pcb has nothing unsent/unacked, then announce PM=1
 * with no settle. Bound = the old drain budget; past it (lost KA) keep the
 * settle and let lwIP's RTO + a TIM wake carry the retransmit. Default on;
 * `lp kaack 0` restores the fixed windows for a same-image A/B.
 */
#define TICKLESS_KA_ACK_POLL_MS  10
#define TICKLESS_KA_ACK_BOUND_MS 500
#define TICKLESS_MIN_PDS_MS     40   /* idle below this: WFI, no PDS round.
                                      * 40 matches the reference SDK's
                                      * configEXPECTED_IDLE_TIME_BEFORE_SLEEP=40
                                      * (its sdiowifi demo proj_config.mk) —
                                      * there the KERNEL never calls the sleep
                                      * hook below 40 ms; ours uses the FreeRTOS
                                      * default of 2, so this guard is the only
                                      * short-sleep gate and the policy lives
                                      * HERE, not in hal_lp (whose 1220 us
                                      * clamp sits below the ~3 ms arm-vs-PDS
                                      * race noted at LP_FW_TRACK_MIN_GUARD_US
                                      * and is now unreachable from this path). */

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
static uint32_t s_beat_tick, s_beat_def;    /* heartbeat recorded in the idle hook */
static volatile uint8_t s_beat_pending;     /* ... printed by the service task */
static uint32_t s_tick_deficit_ms;   /* oversleep not yet credited to the
                                      * kernel clock (see tick compensation) */
static volatile TickType_t s_ka_due; /* next standby keepalive TX (ticks)   */
static volatile int s_dl_drain_only; /* candidate: unsolicited RX never schedules KA */
static volatile int s_ka_pending;    /* budget wake carried a due keepalive */
/* DIAGNOSTIC SCAFFOLD (uncommitted): how many refusals skip the WFI and take a
 * plain return instead, so the streak can be observed advancing. Bounded --
 * a bare return spins this hook at ~51k/s and starves console and beacons. */
#define LP_TL_NOWFI_REFUSALS 256u
extern volatile uint32_t s_pds_last_refuse_info;
#define s_pds_last_refuse_info2 s_pds_last_refuse_info
static uint32_t s_refuse_streak;     /* stall bisect: consecutive sleep refusals */
static TickType_t s_refuse_since;    /* tick the current refusal streak began  */
static uint32_t s_timonly_cycles;    /* bench: TIM-only standby. Non-zero =
                                      * force this DTIM budget and mute every
                                      * app-side RTC wake source, so the only
                                      * thing that can wake us is a TIM hit
                                      * (for measuring pure DTIM beacon power) */
static uint16_t s_timonly_test_aid;  /* optional measurement-only TIM AID */
static uint32_t s_mebusy_at;         /* bench scaffold: entry count at which to
                                      * fault-inject ME_BUSY (0 = disarmed)   */
static TickType_t s_mebusy_until;    /* ... and how long to HOLD it there: a
                                      * single write is wiped by the next PDS
                                      * resume (rwnxl_resume_wifi sets ME_IDLE
                                      * unconditionally), so a faithful wedge
                                      * has to be re-asserted every pass       */
static uint32_t s_mebusy_hold_ms = 5000u;
static uint32_t s_rx_seen;           /* RX witness baseline: unsolicited-DL
                                      * detector (see the containment block)  */
static uint32_t s_dl_drains;         /* drains forced by that detector        */
static volatile int s_dl_uc_only;    /* candidate: witness counts unicast only */

/*
 * What the unsolicited-DL detector watches.
 *
 * The guard exists for ONE hazard: a frame the AP released to us out of its
 * per-STA PS queue leaves the MAC PS state in a shape a following PDS
 * power-down strands (2026-08-04, three corpses). Only UNICAST-to-us traffic
 * is delivered that way -- group-addressed frames are buffered per BSS and
 * flushed after a DTIM beacon, and the blob ignores the TIM group bit
 * entirely, so they never touch the state the guard protects.
 *
 * The original witness was lwIP's ip.recv + etharp.recv, which is a proxy for
 * "something arrived", not for "the AP released our PS queue". Two ways it
 * lies, both measured on LAB10 (2026-09-09, DUT associated and awake, 30 s):
 *
 *   - etharp.recv counts EVERY ARP frame, including broadcast who-has for
 *     other hosts: 23.4/s on a production /22 (44380 -> 45082). The same
 *     window saw rxl_uc_rx_count grow by ZERO. On the NUC -- a private AP
 *     with no other station -- that rate is ~0, which is why this never
 *     showed up before the bench AP changed.
 *   - the lwIP counters are bumped by the tcpip thread, so a frame already
 *     received but not yet dispatched is invisible to a sync taken from the
 *     service task and reads as foreign on the next hook pass.
 *
 * Cost of the lie: tickless_rx_seen_sync() absorbs only what landed before
 * it; the next stray broadcast (mean gap 43 ms) re-trips the detector, which
 * forces a drain, which sends another keepalive, whose window catches the
 * next broadcast. Observed: one arrival chaining 6 KA inside 250 ms, 32 KA in
 * a 10-minute leg instead of 20, those drains 69.1% of the leg's energy.
 *
 * rxl_uc_rx_count is the counter that means what the guard means: unicast
 * DATA/MGT/BAR addressed to us, incremented in the MAC RX dispatch (before
 * lwIP, so no cross-thread lag). Over the same 10-minute leg the AP sent the
 * DUT 23 such frames -- all ACKs to our own keepalives, which the drain's own
 * sync absorbs -- against 99688 group-addressed frames it would now ignore.
 *
 * WHAT NEITHER WITNESS SEES, so nobody reads this as closing the hazard: the
 * AP dequeuing our PS-buffered frame is an AP-side act, but rxl_frame_handle()
 * only reaches the increment after FCS/decrypt SUCCESS, a valid key index and
 * a known STA -- it returns early otherwise (rxl_cntrl.c, the UPLOAD returns
 * ahead of the counter). A unicast frame the AP really did release but we
 * failed to decode is therefore invisible here. It was equally invisible to
 * the lwIP witness, which never sees an undecodable frame either, so this is
 * not a regression -- but on a weak link the guard is incomplete either way.
 *
 * Knob-gated for the same-image A/B; cold boot is the legacy witness.
 */
static uint32_t tickless_dl_witness(void)
{
    extern volatile uint32_t rxl_uc_rx_count;

    if (s_dl_uc_only) {
        return rxl_uc_rx_count;
    }
    return (uint32_t)lwip_stats.ip.recv + (uint32_t)lwip_stats.etharp.recv;
}
static uint32_t s_ka_sent;           /* keepalive branches run this cycle   */
static uint32_t s_ka_ms = TICKLESS_KEEPALIVE_MS; /* keepalive cadence in use   */

static uint32_t s_arp_bcast_fallback;  /* announces that had to go broadcast */
/* 0 = ARP request for our address to the peer (broadcast when uncached);
 * 1 = unicast ARP reply to the next hop, which a Linux neighbour table
 * takes as reachability confirmation. */
static uint32_t s_ka_arp_mode = 1;
static uint32_t s_arp_nohop;           /* replies skipped: next hop uncached */

/* Announce our address to the keepalive peer alone. Broadcast only when its
 * MAC is not cached, and say so once: every broadcast is a potential TIM wake. */
static void tickless_arp_announce(void)
{
    if (s_ka_arp_mode == 1u) {
        if (net_al_sta_arp_reply_nexthop(s_host) == 1 && s_arp_nohop++ == 0u) {
            printf("[LP-TL] arp reply: next hop to %s not in ARP table, skipped\r\n", s_host);
        }
        return;
    }
    if (net_al_sta_arp_announce_to(s_host) == 1 && s_arp_bcast_fallback++ == 0u) {
        printf("[LP-TL] arp announce: peer %s not in ARP table, broadcast\r\n", s_host);
    }
}

/* Called only when due. Keep phase after a late wake without catch-up bursts. */
static void tickless_ka_advance(TickType_t now)
{
    TickType_t period = pdMS_TO_TICKS(s_ka_ms);
    TickType_t late = now - s_ka_due;
    s_ka_due += (late / period + 1u) * period;
    if (s_ka_due == 0u)
        s_ka_due = 1u; /* Zero is the disabled sentinel. */
}
#ifdef CONFIG_LWIP_LP
static int s_ka_ack_exit = 1;        /* keepalive: PM=1 as soon as the KA is ACKed */
#else
static const int s_ka_ack_exit = 0;  /* no lwip_sock_tx_pending() without the LP lwIP */
#endif
static uint32_t s_ka_ack_timeouts;   /* KAs whose ACK missed TICKLESS_KA_ACK_BOUND_MS */
static int s_ever_linked;            /* distinguishes a rebuild from setup  */
static uint32_t s_ap_losses;         /* CONSECUTIVE beacon-loss give-ups    */
static uint32_t s_link_giveups;      /* dead-host link retries (repeat mode) */
static uint32_t s_escalations;       /* wedge recoveries driven from here    */
static uint32_t s_catchup_calls;     /* idle-hook passes: is the hook reached? */
static uint32_t s_catchup_total_ms;  /* oversleep replayed by lp_tickless_tick_catchup */
static uint32_t s_dbg_last_cycles;   /* beat: blob residency budget last entry */
static uint32_t s_dbg_last_slept;    /* beat: slept_ms the blob reported back  */
/* Who set the budget. cyc=271 (277 s) was handed to the blob on a leg whose
 * service task caps its own block at the 30 s keepalive period, so either
 * the OS deadline the hook saw was not that task's, or the task's wait was
 * not what its arithmetic intends. Record both and stop inferring. */
static uint32_t s_dbg_last_exp;      /* beat: xExpectedIdleTime at that entry */
static uint32_t s_dbg_last_wait;     /* beat: service task's last block (ticks) */
static uint8_t  s_dbg_last_wait_src; /* beat: 0 period 1 ka_due 2 overdue 3 guard */

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
static struct lp_wake_matcher s_wake_matcher;

static int tickless_recv_stream(int fd, void *buf, size_t len, int flags)
{
    int n = recv(fd, buf, len, flags);
    if (n > 0) {
        lp_wake_feed(&s_wake_matcher, buf, (size_t)n);
    }
    return n;
}

static int tickless_wake_take(void)
{
    return lp_wake_take(&s_wake_matcher);
}

/*
 * RAILGATE (Kconfig CONFIG_LP_RAILGATE, default y, sel from
 * CONFIG_LP_RAILGATE_DCDC18_SEL, default 21 = 1.65 V soak-qualified):
 * DCDC18 drop for the standby span -- applied at arm, restored at EVERY
 * disarm path (service task exit, seed-cap self-disarm, `lp tickless
 * stop`). Both helpers are idempotent, so a stop racing the task's own
 * exit restores exactly once. The AON word is PDS-retained, which is the
 * whole point: the lowered sel rides every sleep/wake of the session.
 */
#ifdef CONFIG_LP_RAILGATE
static uint32_t s_railgate_saved_sel;
static uint8_t s_railgate_on;

static void railgate_apply(void)
{
    volatile uint32_t *r = (volatile uint32_t *)LP_FW_RAILGATE_AON_DCDC;
    if (s_railgate_on) {
        return;
    }
    s_railgate_saved_sel = *r & 0x1Fu;
    *r = (*r & ~0x1Fu) | (CONFIG_LP_RAILGATE_DCDC18_SEL & 0x1Fu);
    s_railgate_on = 1u;
    printf("[LP-TL] railgate: dcdc18 sel %lu -> %u for this standby\r\n",
           (unsigned long)s_railgate_saved_sel,
           (unsigned)(CONFIG_LP_RAILGATE_DCDC18_SEL & 0x1Fu));
}

static void railgate_restore(void)
{
    volatile uint32_t *r = (volatile uint32_t *)LP_FW_RAILGATE_AON_DCDC;
    if (!s_railgate_on) {
        return;
    }
    s_railgate_on = 0u;
    *r = (*r & ~0x1Fu) | s_railgate_saved_sel;
    printf("[LP-TL] railgate: dcdc18 restored to sel %lu\r\n",
           (unsigned long)s_railgate_saved_sel);
}
#else
static void railgate_apply(void) { }
static void railgate_restore(void) { }
#endif

/*
 * Keepalive phase: keep the keepalive out of the AP's post-DTIM group flush.
 *
 * ROOT CAUSE (measured on LAB10, 2026-09-09). The LP blob wakes on DTIM
 * beacons, so the keepalive's PM=0 null goes out 21 ms and the keepalive
 * itself 41 ms (medians) after a DTIM beacon. An AP buffers group-addressed
 * traffic for the whole DTIM period and flushes it right after each DTIM
 * beacon; on this BSS that is ~155 frames/s of foreign broadcast, all at
 * 1 Mbps, so the flush is 120-160 ms of solid airtime. The keepalive's TCP
 * ACK reaches the AP about a millisecond after the keepalive is sent and
 * queues behind that flush. Measured: 88.6% channel occupancy inside the
 * wait against 18.5% in an ordinary window, and the DUT's own RX dispatch
 * count tracks the wait exactly -- ack 36 ms at 20 frames, ack 306 ms at 209.
 * Nothing is broken at either end: the AP answers a ping to an awake DUT in
 * 9 ms, our receiver reports zero FCS/decrypt failures throughout, and our
 * TIM bit is never set. We simply transmit into our own AP's busiest window.
 *
 * FIX. When the keepalive comes due, do not send it on this wake. Shorten
 * the NEXT blob sleep so the following wake lands mid-DTIM-period, where the
 * flush has drained, and send it there. Shorten rather than lengthen: the
 * association listen interval covers dtim_num beacons and the AP discards
 * our buffered unicast past it, so a longer one-shot skip would trade this
 * problem for a worse one. The keepalive arrives up to one DTIM period
 * later, which is nothing against a 30 s cadence.
 *
 * Only IOT2LP_PARA->dtim_num is written here, never lp_demo_set_dtim_num():
 * that setter also pushes a listen interval through cfgrwnx, which must not
 * be called from the idle hook.
 *
 * The phase is STABLE, so one shift is enough and it sticks. Air capture of
 * 14 consecutive keepalives: the AP's DTIM count at our PM=0 null was only
 * ever 0 or 9 and the null landed 3-30 ms after a DTIM beacon every single
 * time. Our wake period (dtim_num beacons) equals the AP's DTIM period, so
 * we are locked to it -- which is exactly why every keepalive lands in the
 * flush, and also why moving once moves us for good.
 *
 * The shift is UNCONDITIONAL, once per keepalive, and that is not laziness:
 * the blob re-aligns to DTIM beacons by design (it has to catch them for the
 * group TIM and the loss policy), so the offset cycle does not persist -- the
 * next keepalive starts from the DTIM boundary again. Measured with the shift
 * gated on the previous wait, the quiet keepalives were exactly the deferred
 * ones and every un-deferred one fell straight back into the flush:
 * 298, 30, 232, 56, 180, 186, 36, 196, 28, 146, 310, 40, 150. So every
 * keepalive needs its own shift.
 *
 * No AP DTIM count is needed for this. Air capture of 14 consecutive
 * keepalives put the AP's DTIM count at our PM=0 null at 0 or 9 every time,
 * 3-30 ms after a DTIM beacon: we are always starting from the boundary, so
 * "half a period from here" is a fixed offset, not something to look up.
 */
/* Longer than this and the keepalive was waiting behind a group flush; the
 * measured split is unambiguous (quiet waits 8-70 ms, flushed 160-322 ms). */
#define TL_KA_PHASE_BUSY_MS 110

static volatile uint8_t s_ka_phase_skip;  /* beacons, 0 = knob off */
static uint8_t s_ka_phase_armed;          /* this KA is waiting for the offset wake */
static uint8_t s_ka_phase_saved;          /* dtim_num to put back */
static uint32_t s_ka_phase_defers;
static int s_ka_last_ack_ms = -1;         /* previous keepalive's ACK wait */

/* Why the last keepalive was not carried on a shortened cycle.
 *
 * The shift is a PERFECT predictor of a quiet keepalive: over two gate-length
 * legs (KAPHASE_B1_on/B2_on, 39 keepalives) every keepalive preceded by the
 * ~505 ms shortened cycle waited 40-50 ms for its ACK (20 of 20 under 110 ms)
 * and every keepalive preceded by a full-length sleep waited 170-180 ms
 * (0 of 19 under 110 ms). So the skip VALUE is already right and the only
 * thing that matters is how often the shift actually runs -- it ran for about
 * half of them. These witnesses name the branch that skipped it instead of
 * leaving it to be inferred from sleep lengths. Diagnostic only: they are
 * printed under `lp kaprof`, which is default OFF. */
#define TL_KAPH_DEFERRED  0   /* shift armed: the next wake carries the KA */
#define TL_KAPH_OFF       1   /* knob is 0 */
#define TL_KAPH_ARMED     2   /* already armed -- this IS the offset wake */
#define TL_KAPH_NODTIM    3   /* dtim_num read back as 0 */
#define TL_KAPH_TOOLONG   4   /* skip >= dtim_num: would outrun the listen interval */
#define TL_KAPH_SRC_HOOK  1
#define TL_KAPH_SRC_TASK  2

static uint8_t s_ka_phase_why;            /* TL_KAPH_* from the last defer() call */
static uint8_t s_ka_phase_user;           /* dtim_num defer() actually read */
static uint8_t s_ka_phase_src;            /* which caller sent this keepalive */

/* Did the shortened cycle the deferral asked for actually RUN?
 *
 * The witnesses above answer "did the deferral arm" (why=ARMED on 21/21,
 * defers=19/21) but not "did the blob then sleep the short cycle", and the
 * sleep length cannot answer it: s_dbg_last_slept still holds the PREVIOUS
 * sleep when the keepalive is sent, which is how a run of full 1024 ms
 * periods was misread as a run of shortened ones (kaphase_budget_candidate,
 * rationale retracted).
 *
 * s_pds_entries is the counter that means "the blob ran a cycle". Snapshot it
 * at the arm, subtract at the send:
 *
 *   pdsdelta == 0  the shortened cycle never happened -- the keepalive went
 *                  out without any blob cycle in between, so the send beat
 *                  the sleep rather than the sleep landing at the wrong phase
 *   pdsdelta >= 1  the cycle ran; the phase miss is somewhere else
 *
 * The other two witnesses test the standing lead for the pdsdelta == 0 case:
 * the backstop holds its deadline with s_ka_due = now + hold_ms, and
 * lp_tickless_tick_catchup() then replays banked oversleep through
 * xTaskCatchUpTicks(), which can carry the tick past that deadline at once.
 *   duelead  ticks the send is PAST the held deadline (>= 0 means the hold
 *            had already expired when the backstop looked again)
 *   defms    oversleep replayed between the arm and the send
 * Diagnostic only, printed under the default-OFF `lp kaprof`. */
static uint32_t s_ka_phase_pds0;          /* s_pds_entries at the arm */
static uint32_t s_ka_phase_def0;          /* s_catchup_total_ms at the arm */
static TickType_t s_ka_phase_due0;        /* deadline the backstop held */
static uint8_t s_ka_phase_held;           /* a deadline was held for this KA */
static uint32_t s_ka_phase_pdsdelta;      /* blob cycles between arm and send */
static uint32_t s_ka_phase_defms;         /* oversleep replayed in that window */
static int32_t s_ka_phase_duelead;        /* ticks past the held deadline */
static uint8_t s_ka_phase_fresh;          /* the KA about to be sent consumed an arm */

/* Arm-time reset. s_pds_entries restarts at 0 on every arm, so a snapshot
 * taken before it would make the first delta underflow into millions. */
static void tickless_ka_phase_witness_reset(void)
{
    s_ka_phase_pds0 = 0;
    s_ka_phase_def0 = s_catchup_total_ms;
    s_ka_phase_due0 = 0;
    s_ka_phase_held = 0u;
    s_ka_phase_pdsdelta = 0;
    s_ka_phase_defms = 0;
    s_ka_phase_duelead = 0;
}

/* Called from the wake hook when the keepalive is due. Returns 1 if the
 * keepalive was deferred to the next (offset) wake, 0 to send it now. */
static int tickless_ka_phase_defer(void)
{
    uint8_t skip = s_ka_phase_skip;
    uint8_t user;

    if (skip == 0u || s_ka_phase_armed) {
        s_ka_phase_why = skip == 0u ? TL_KAPH_OFF : TL_KAPH_ARMED;
        return 0;                       /* off, or this is the offset wake */
    }
    user = IOT2LP_PARA->dtim_num;
    s_ka_phase_user = user;
    if (user == 0u || skip >= user) {
        s_ka_phase_why = user == 0u ? TL_KAPH_NODTIM : TL_KAPH_TOOLONG;
        return 0;                       /* a longer skip would outrun the
                                         * listen interval -- never do that */
    }
    s_ka_phase_why = TL_KAPH_DEFERRED;
    s_ka_phase_saved = user;
    IOT2LP_PARA->dtim_num = skip;
    s_ka_phase_armed = 1u;
    s_ka_phase_defers++;
    s_ka_phase_pds0 = s_pds_entries;
    s_ka_phase_def0 = s_catchup_total_ms;
    s_ka_phase_held = 0u;       /* the backstop sets the deadline below */
    return 1;
}

/* How long the caller must hold the keepalive deadline for the shortened
 * cycle to happen exactly ONCE.
 *
 * The divisor is the SAVED period, not IOT2LP_PARA->dtim_num: by the time
 * this runs, tickless_ka_phase_defer() has already overwritten dtim_num with
 * the skip. Reading it back gave skip * dtim_ms / skip = one FULL period, so
 * the blob slept two shortened cycles instead of one -- 2 x 5 = 10 beacons --
 * and landed back exactly where it started. That is why the shift never
 * stuck: it cancelled itself every time.
 */
static uint32_t tickless_ka_phase_hold_ms(void)
{
    uint32_t per = s_ka_phase_saved;    /* beacons in a normal cycle */

    if (per == 0u) {
        return s_dtim_ms;
    }
    return (uint32_t)s_ka_phase_skip * s_dtim_ms / per;
}

/* Put the schedule back before the keepalive runs on the offset wake. */
static void tickless_ka_phase_restore(void)
{
    if (!s_ka_phase_armed) {
        return;
    }
    s_ka_phase_armed = 0u;
    IOT2LP_PARA->dtim_num = s_ka_phase_saved;
    /* Both send sites call this immediately before the keepalive goes out, so
     * it is the one place that sees the end of the deferral window. */
    s_ka_phase_pdsdelta = s_pds_entries - s_ka_phase_pds0;
    s_ka_phase_defms = s_catchup_total_ms - s_ka_phase_def0;
    s_ka_phase_duelead = s_ka_phase_held ?
        (int32_t)(xTaskGetTickCount() - s_ka_phase_due0) : 0;
    s_ka_phase_fresh = 1u;
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
        s_rx_seen = tickless_dl_witness();
        /* DL absorbed while awake/draining is benign — keep the baseline
         * fresh so it never triggers. ip+etharp, NOT link: nothing in this
         * port's RX path increments the link-level counters (verified: the
         * detector never fired once on pings that demonstrably reached
         * lwIP), while ip4_input/etharp_input always count. */
        portENABLE_INTERRUPTS();
        __asm__ volatile("wfi");
        return;
    }
    /* Unsolicited-DL containment (2026-08-04, board03 rounds 6-12). A frame
     * delivered through the AP's PS machinery while standby cycles (ARP
     * request, ping — anything the host did not solicit) leaves the MAC PS
     * state in a shape where a following PDS power-down strands the
     * sequencer for good: wake never fires, deaf to console and flasher,
     * relay-reset-only. Measured: ~1/15 per entry once poisoned, three
     * corpses + a 2 s ping-kill reproducer; a ka-style drain 0.6 s after
     * the delivery re-normalized the state and the system survived 400+
     * further entries (round 12). So: any growth of the link-level input
     * counter outside a drain forces an immediate drain INSTEAD of the
     * next PDS entry. Side bonus: the host's TCP wake message now triggers
     * its drain at once instead of waiting for the 15 s keepalive. Cost:
     * broadcast chatter on a busy BSS turns into extra drains (~100-300 ms
     * awake each) — power, not correctness. */
    uint32_t dl_witness = tickless_dl_witness();
    if (s_timonly_cycles == 0u && dl_witness != s_rx_seen) {
        s_rx_seen = dl_witness;
        s_dl_drains++;
        printf("[LP-TL] unsolicited DL -> drain #%lu (entries=%lu)\r\n",
               (unsigned long)s_dl_drains, (unsigned long)s_pds_entries);
        if (s_task != NULL) {
            if (!s_dl_drain_only) {
                s_ka_due = xTaskGetTickCountFromISR() +
                           pdMS_TO_TICKS(s_ka_ms);
                s_ka_pending = 1;
            }
            /* A scheduled KA already pending is retained. Otherwise the
             * service task uses the ordinary PM=0/drain/re-arm path. */
            s_pause = 1;
            BaseType_t woken = pdFALSE;
            vTaskNotifyGiveFromISR(s_task, &woken);
            (void)woken;        /* schedule runs when the idle task resumes */
        }
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
     * than a cycle are not worth a PDS round-trip.
     *
     * Round UP. With floor() the blob came back 0..1 DTIM before the
     * deadline, the next hook entry saw a sub-cycle window, took the forced
     * single cycle below (often twice), and every OS deadline cost 2-3
     * full-FW wakes at ~7.5 mC each instead of one (2026-09-07: 157 of 243
     * wake gaps were exactly one DTIM). ceil() lands the wake up to one
     * cycle AFTER the deadline, which is the contract the forced cycle
     * already relies on; same image A/B: 242 -> 113 PDS entries per 600 s. */
    uint32_t cycles = (xExpectedIdleTime + s_dtim_ms - 1u) / s_dtim_ms;
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
    /* Bench: TIM-only standby. Ignore the OS deadline entirely so the blob
     * keeps re-sleeping beacon to beacon and only a TIM hit brings us back.
     * The oversleep is banked by the tick compensation below exactly as the
     * ordinary cycles floor already is -- this just makes it much larger. */
    if (s_timonly_cycles != 0u) {
        cycles = s_timonly_cycles;
    }


    /* Bench scaffold: hold the injected wedge. Re-assert every pass, else the
     * next thing that touches ME clears it and the refusal streak never
     * reaches the deadline the reclaim is supposed to enforce. */
    if (s_mebusy_until != 0u) {
        if ((int32_t)(xTaskGetTickCount() - s_mebusy_until) < 0) {
            rwnxl_control_plane_force_busy();
        } else {
            s_mebusy_until = 0u;
            printf("[LP-TL] TEST: ME_BUSY hold released\r\n");
        }
    }

    uint32_t slept_ms = 0;
    int reason;
    s_dbg_last_cycles = cycles;          /* beat: residency budget this entry */
    s_dbg_last_exp = xExpectedIdleTime;  /* beat: the deadline it came from */
    HEAP_ACC(s_heap_acc_pds,
             reason = lp_demo_sleep_from_idle(s_dtim_ms, cycles,
                                              s_ps_active != 0, &slept_ms));
    s_dbg_last_slept = slept_ms;         /* beat: what the blob reported back */
    if (reason < 0) {
        portENABLE_INTERRUPTS();
        /* Stall bisect (debug scaffold 2026-08-04): the -2 quiescence-gate
         * refusal is silent — a permanently poisoned gate (pck_cnt-leak
         * family) looks exactly like the observed E-marker stop. Print the
         * first refusal of a streak and then every 4096th; the streak resets
         * on the next successful entry below. */
        s_refuse_streak++;
        if (s_refuse_streak == 1u) {
            s_refuse_since = xTaskGetTickCount();
        }
        if (s_refuse_streak == 1u || (s_refuse_streak % 4096u) == 0u) {
            /* bits: low16 = quiescence-mask reasons, high16 = prevent_sleep
             * (see s_pds_last_refuse_info) — names which gate/bits refused,
             * for the strand-follows-refusal forensics. */
            extern volatile uint32_t s_pds_last_refuse_info;
            printf("[LP-TL] sleep refused r=%d streak=%lu bits=%08lx\r\n",
                   reason, (unsigned long)s_refuse_streak,
                   (unsigned long)s_pds_last_refuse_info);
        }
        /* EVERY refusal waits for an interrupt. The gate conditions all clear
         * on a HW event (a TX confirm for the quiescence gate, an ME<->MM cfm
         * for the -1 me-not-idle gate), so a bare return just has the idle task
         * re-enter this hook at once: measured 2026-08-07 at ~51k samples/s for
         * 20 s straight, which starved the console and cost 34 beacons -> link
         * lost -> a missed DTIM wake. The old `reason <= -2` split had no basis;
         * -1 waits for a HW event exactly like -2 does. The reference SDK
         * does __WFI(); return; at its own gate too. UNMASK first -- the Nuclei
         * ECLIC WFI does not idle while masked (same hazard as the two WFI
         * sites above). */
        __asm__ volatile("wfi");

        /* Orphaned-ME backstop. rwnx.c bounds a BUSY ME at 2 s, but it measures
         * with hal_machw_time(), and in the 2026-08-07 wedge that clock lagged:
         * the reclaim landed 19.9 s after the streak began, long after the link
         * was gone. The refusal streak proves ME never went idle in between, so
         * bound the same condition on the OS tick -- which keeps running here --
         * and reclaim when it expires. Harmless if ME is already idle. */
        if ((xTaskGetTickCount() - s_refuse_since) >=
            pdMS_TO_TICKS(TICKLESS_ME_ORPHAN_MS)) {
            s_refuse_since = xTaskGetTickCount();
            if (rwnxl_control_plane_reclaim_if_busy()) {
                printf("[LP-TL] ME orphan (tick-bound) -> reclaimed after %lu refusals\r\n",
                       (unsigned long)s_refuse_streak);
            }
        }
        return;             /* aborted — next idle pass retries */
    }
    s_pds_entries++;
    s_refuse_streak = 0;    /* stall bisect: a successful entry ends the streak */

    /* Bench scaffold (uncommitted): fire the armed ME_BUSY fault injection.
     * Here, not from the shell, because an armed DUT eats console bytes and a
     * JTAG write against the powered-down core kills the DAP session -- this
     * is the only place that can reach the wedge state while standby runs. */
    if (s_mebusy_at != 0u && s_pds_entries >= s_mebusy_at) {
        s_mebusy_at = 0u;
        s_mebusy_until = xTaskGetTickCount() + pdMS_TO_TICKS(s_mebusy_hold_ms);
        printf("[LP-TL] TEST: injecting ME_BUSY at entry %lu, holding %lu ms\r\n",
               (unsigned long)s_pds_entries, (unsigned long)s_mebusy_hold_ms);
        rwnxl_control_plane_force_busy();
    }

    /* Tickless tick compensation. The reference SDK only ever sleeps UP TO
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
        railgate_restore();
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
    } else if (s_task != NULL && s_timonly_cycles == 0u && s_ka_due != 0u &&
               (int32_t)(xTaskGetTickCountFromISR() - s_ka_due) >= 0) {
        if (tickless_ka_phase_defer()) {
            return;         /* re-sleep a short cycle; send on the next wake */
        }
        s_ka_phase_src = TL_KAPH_SRC_HOOK;
        tickless_ka_phase_restore();
        tickless_ka_advance(xTaskGetTickCountFromISR());
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

/* Re-base the hook's unsolicited-DL detector on what lwIP has received so
 * far. Call before handing the idle hook back (s_pause = 0) after any drain
 * the service task itself solicited: the hook refreshes s_rx_seen only in its
 * parked branch, so a frame that lands between its last parked pass and the
 * release -- the keepalive's own TCP ACK, once exit-on-ACK made the tail
 * short enough (KA_ACK_ON1: "ka drained" -> "unsolicited DL -> drain" -> a
 * second KA, 4x per 10 min) -- would read as foreign and cost an extra drain. */
static void tickless_rx_seen_sync(void)
{
    s_rx_seen = tickless_dl_witness();
}

/* Standby heartbeat, printed from the service task after the PM=1 re-announce
 * (the idle hook only records it -- see the beat gate in vApplicationSleep). */
static void tickless_beat_flush(void)
{
    if (!s_beat_pending) {
        return;
    }
    s_beat_pending = 0u;
    printf("[LP-TL] beat: tick=%lu due=%lu pend=%d def=%lu pds=%lu ka=%lu cu=%lu cyc=%lu slept=%lu "
           "| exp=%lu wait=%lu wsrc=%u\r\n",
           (unsigned long)s_beat_tick, (unsigned long)s_ka_due,
           s_ka_pending, (unsigned long)s_beat_def, (unsigned long)s_pds_entries,
           (unsigned long)s_ka_sent, (unsigned long)s_catchup_calls,
           (unsigned long)s_dbg_last_cycles, (unsigned long)s_dbg_last_slept,
           (unsigned long)s_dbg_last_exp, (unsigned long)s_dbg_last_wait,
           (unsigned)s_dbg_last_wait_src);
}

/* One non-blocking look at the socket: >0 bytes, 0 nothing there, -1 dead. */
static int tickless_recv_once(char *buf, size_t len)
{
    int n;

    s_rxp_why = RXP_DATA;
    s_rxp_errno = 0;
    n = tickless_recv_stream(s_sock, buf, len - 1, MSG_DONTWAIT);
    if (n > 0) {
        buf[n] = '\0';
        return n;
    }
    if (n == 0) {
        s_rxp_why = RXP_FIN;
        return -1;                              /* orderly FIN: peer is gone */
    }
    if (errno != EWOULDBLOCK && errno != EAGAIN) {
        s_rxp_why = RXP_ERR;
        s_rxp_errno = errno;
        return -1;                              /* RST / hard socket error */
    }
    return 0;
}

static int tickless_recv_poll(char *buf, size_t len, int polls)
{
    for (int i = 0; i < polls; i++) {
        int n = tickless_recv_once(buf, len);
        if (n != 0) {
            return n;
        }
        vTaskDelay(pdMS_TO_TICKS(TICKLESS_RX_POLL_MS));
    }
    s_rxp_why = RXP_TIMEOUT;
    return 0;
}

/* TIM-wake drain. After the PM=0 null the AP flushes what it buffered for us;
 * the frame that arrives with More Data clear says its queue is empty (the
 * 802.11 rule ps_check_frame applies while PS is on -- PS is off during this
 * drain), so leave one poll after it instead of holding PM=0 for the full
 * TICKLESS_WAKE_DRAIN_POLLS x 100 ms. uc0 = rxl_uc_rx_count before the null.
 * Measured 2026-09-07 (NUC ping every 60 s, same image): 542 -> 69 ms and
 * 43 -> 6 mC per TIM wake, every ping answered. The old bound stays for a TIM
 * race with nothing behind it. */
static int tickless_wake_drain(char *buf, size_t len, uint32_t uc0)
{
    extern volatile uint32_t rxl_uc_rx_count;
    extern volatile uint8_t rxl_uc_rx_more_data;
    const uint32_t bound_ms = TICKLESS_WAKE_DRAIN_POLLS * TICKLESS_RX_POLL_MS;
    uint32_t waited = 0;
    int seen = 0;

    for (;;) {
        int n = tickless_recv_once(buf, len);
        if (n != 0) {
            return n;
        }
        if (seen) {
            return 0;                    /* the poll after the queue-empty frame */
        }
        if (rxl_uc_rx_count != uc0 && !rxl_uc_rx_more_data) {
            seen = 1;
        }
        if (waited >= bound_ms) {
            s_rxp_why = RXP_TIMEOUT;
            return 0;
        }
        vTaskDelay(pdMS_TO_TICKS(TICKLESS_WAKE_DRAIN_STEP_MS));
        waited += TICKLESS_WAKE_DRAIN_STEP_MS;
    }
}

/*
 * Keepalive ACK-wait profiler (2026-09-09, default OFF).
 *
 * With the unsolicited-DL cascade fixed, this wait IS the standby budget:
 * measured on LAB10, 93% of a keepalive's awake time is spent here, ~150 ms
 * of it at a flat 72.7 mA, 19 times per 10-minute leg. Before changing it,
 * split it at the three boundaries that can each move independently:
 *
 *   send() ---T1---> our frame confirmed by the MAC (macsw_cfm_total)
 *          ---T2---> first unicast frame from the AP reaches us (rxl_uc_rx_count)
 *          ---T3---> lwIP has processed the TCP ACK (tx_pending == 0)
 *
 * T1 is our TX path including 802.11 retries; T2 is the air round trip plus
 * whatever the AP does with it; T3 is our own RX/lwIP handling. All three
 * counters already exist and are maintained outside this file, so profiling
 * costs three volatile reads per poll and nothing at all when the knob is 0.
 *
 * Known confound, stated rather than hidden: the gratuitous ARP is sent just
 * before the keepalive, so its confirm can land inside this window. n_cfm
 * makes that visible (expect 2 when it does) and cfm_last is the later one.
 *
 * The knob doubles as the poll period, because at the shipping 10 ms a T1 or
 * T3 of a few ms is unresolvable. A profiled leg therefore polls more often
 * than the product does and is a DIAGNOSTIC leg, never a power result.
 */
#include "drv_glb.h"            /* GLB_PER_Clock_Gate/UnGate, CGEN_CFG0 */

/*
 * Peripheral clock gating across the keepalive's ACK wait (default OFF).
 *
 * The wait parks the CPU in WFI at 72.7 mA for 150-250 ms per keepalive with
 * nothing running but a poll loop and the receiver. Everything below is a
 * module this product never uses in standby, so its bus clock is dead weight
 * for the whole window. Console (uart0), CCI, OCRAM/WRAM, the serial flash
 * that XIP fetches from, the bus controller, the crypto the supplicant uses,
 * DMA, the timers and the two RF/MAC gates are all deliberately NOT in this
 * set -- gating any of them would break the very drain we are measuring.
 *
 * Applied through GLB_PER_Clock_Gate() rather than a raw mask: the driver's
 * IP->bit table is the checked one (test_cgen_map.py pins it to the register
 * declaration), and the old hand-rolled masks in this tree are exactly what
 * went wrong -- the same API used to gate ocram when asked for CPU.
 */
#define TL_CGEN_IDLE_IPS  (GLB_AHB_CLOCK_GPIP | GLB_AHB_CLOCK_SDBG | \
                           GLB_AHB_CLOCK_UART1 | GLB_AHB_CLOCK_EF_CTRL | \
                           GLB_AHB_CLOCK_SPI | GLB_AHB_CLOCK_I2C | \
                           GLB_AHB_CLOCK_I2C1 | GLB_AHB_CLOCK_PWM | \
                           GLB_AHB_CLOCK_CHECKSUM | GLB_AHB_CLOCK_I2S | \
                           GLB_AHB_CLOCK_USB11 | GLB_AHB_CLOCK_EMAC | \
                           GLB_AHB_CLOCK_USB20_SDU | \
                           GLB_AHB_CLOCK_BT_BLE_NORMAL | \
                           GLB_AHB_CLOCK_DBI | GLB_AHB_CLOCK_SPI2)
/* The same set as a CGEN_CFG0 word, used ONLY to check that every one of them
 * is ungated before we gate it. Restoring with UnGate would otherwise be a
 * guess: if something else had already gated one, we would hand it back
 * ungated. If the check fails we gate nothing and say so. */
#define TL_CGEN_IDLE_MASK 0x0671DF8Cu
#define TL_CGEN_CFG0_ADDR 0x30000580u

static volatile int s_cgen_idle_on;     /* knob, cold boot OFF */
static uint32_t s_cgen_idle_skips;      /* refused because something was gated */

#ifdef CONFIG_LWIP_LP   /* only the LP idle hook calls these */
static uint8_t s_cgen_idle_held;        /* gating currently applied */
static void tickless_cgen_idle_apply(void)
{
    if (!s_cgen_idle_on || s_cgen_idle_held) {
        return;
    }
    if ((*(volatile uint32_t *)TL_CGEN_CFG0_ADDR & TL_CGEN_IDLE_MASK)
        != TL_CGEN_IDLE_MASK) {
        s_cgen_idle_skips++;            /* someone else owns one of these */
        return;
    }
    GLB_PER_Clock_Gate(TL_CGEN_IDLE_IPS);
    s_cgen_idle_held = 1u;
}

static void tickless_cgen_idle_restore(void)
{
    if (!s_cgen_idle_held) {
        return;
    }
    s_cgen_idle_held = 0u;
    GLB_PER_Clock_UnGate(TL_CGEN_IDLE_IPS);
}
#endif /* CONFIG_LWIP_LP */

#define KAP_NONE 0xFFFFu

static volatile uint32_t s_ka_prof_ms;   /* 0 = off, else poll period in ms */
static uint16_t s_kap_cfm_first, s_kap_cfm_last, s_kap_uc, s_kap_ack;
static uint32_t s_kap_cfm_n, s_kap_uc_n, s_kap_retr;
/* RX-side split (2026-09-09 round 2). rxl_uc_rx_count only counts frames that
 * passed FCS/decrypt and mapped to a known STA, so "the AP never transmitted"
 * and "the AP transmitted and we could not decode it" look identical in it.
 * macsw_rxl_total counts every dispatched descriptor and macsw_rxl_up_nosucc
 * counts the ones the PHY/MAC could not validate at all, which separates the
 * two: if nosucc climbs through the wait and stops when the ACK finally lands,
 * we were deaf, not idle. */
static uint32_t s_kap_rx_total, s_kap_rx_nosucc, s_kap_rx_nokey;
static uint16_t s_kap_nosucc_half;   /* nosucc accumulated in the first half */

#ifdef CONFIG_LWIP_LP
static uint32_t kap_cfm_total(void)
{
    extern volatile uint32_t macsw_cfm_total[];
    uint32_t sum = 0;

    for (uint32_t i = 0; i < 5u; i++) {   /* NX_TXQ_CNT on this build */
        sum += macsw_cfm_total[i];
    }
    return sum;
}

/* Total retransmissions across every frame confirmed so far: the NUM_RETRIES
 * histogram weighted by its index. A delta of 0 over the window means our
 * keepalive went out first try. */
static uint32_t kap_retr_sum(void)
{
    extern volatile uint32_t macsw_cfm_retries[];
    uint32_t sum = 0;

    for (uint32_t i = 1u; i < 8u; i++) {
        sum += i * macsw_cfm_retries[i];
    }
    return sum;
}

static void kap_rx_snapshot(uint32_t *total, uint32_t *nosucc, uint32_t *nokey)
{
    extern volatile uint32_t macsw_rxl_total;
    extern volatile uint32_t macsw_rxl_up_nosucc;
    extern volatile uint32_t macsw_rxl_up_nokey;

    *total = macsw_rxl_total;
    *nosucc = macsw_rxl_up_nosucc;
    *nokey = macsw_rxl_up_nokey;
}
#endif

/* Hold until the socket's pcb has nothing unsent/unacked. >= 0: ms waited,
 * -1: not drained within bound_ms, -2: not a live TCP socket. The caller
 * holds PM=0, so the peer's ACK arrives directly, not via the AP's PS queue. */
#ifdef CONFIG_LWIP_LP
static int tickless_tx_ack_wait(uint32_t bound_ms)
{
    extern volatile uint32_t rxl_uc_rx_count;
    uint32_t waited = 0;
    uint32_t prof = s_ka_prof_ms;
    uint32_t poll = prof ? prof : (uint32_t)TICKLESS_KA_ACK_POLL_MS;
    uint32_t cfm0 = 0, uc0 = 0, retr0 = 0;
    uint32_t rxt0 = 0, rxn0 = 0, rxk0 = 0;
    uint32_t half_mark = bound_ms / 2u;

    tickless_cgen_idle_apply();
    if (prof) {
        cfm0 = kap_cfm_total();
        uc0 = rxl_uc_rx_count;
        retr0 = kap_retr_sum();
        kap_rx_snapshot(&rxt0, &rxn0, &rxk0);
        s_kap_cfm_first = s_kap_cfm_last = s_kap_uc = s_kap_ack = KAP_NONE;
        s_kap_cfm_n = s_kap_uc_n = s_kap_retr = 0;
        s_kap_rx_total = s_kap_rx_nosucc = s_kap_rx_nokey = 0;
        s_kap_nosucc_half = 0;
    }

    for (;;) {
        int pending = lwip_sock_tx_pending(s_sock);

        if (prof) {
            uint32_t cfm = kap_cfm_total();
            uint32_t uc = rxl_uc_rx_count;

            if (cfm != cfm0) {
                if (s_kap_cfm_first == KAP_NONE) {
                    s_kap_cfm_first = (uint16_t)waited;
                }
                s_kap_cfm_last = (uint16_t)waited;
                s_kap_cfm_n = cfm - cfm0;
                s_kap_retr = kap_retr_sum() - retr0;
            }
            if (uc != uc0) {
                if (s_kap_uc == KAP_NONE) {
                    s_kap_uc = (uint16_t)waited;
                }
                s_kap_uc_n = uc - uc0;
            }
            {
                uint32_t rxt, rxn, rxk;

                kap_rx_snapshot(&rxt, &rxn, &rxk);
                s_kap_rx_total = rxt - rxt0;
                s_kap_rx_nosucc = rxn - rxn0;
                s_kap_rx_nokey = rxk - rxk0;
                if (waited <= half_mark && s_kap_rx_nosucc <= 0xFFFFu) {
                    s_kap_nosucc_half = (uint16_t)s_kap_rx_nosucc;
                }
            }
        }
        if (pending < 0) {
            tickless_cgen_idle_restore();
            return -2;
        }
        if (pending == 0) {
            if (prof) {
                s_kap_ack = (uint16_t)waited;
            }
            tickless_cgen_idle_restore();
            return (int)waited;
        }
        if (waited >= bound_ms) {
            tickless_cgen_idle_restore();
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(poll));
        waited += poll;
    }
}
#else
static int tickless_tx_ack_wait(uint32_t bound_ms)
{
    (void)bound_ms;
    return -2;
}
#endif

/* Announce static PS and give the host's TCP ACKs settle_ms to drain — a
 * buffered ACK sets our TIM bit and converts the next cycle into an
 * instant wake (the lp_tcpdtim lesson). A caller that has already SEEN the
 * ACK land (keepalive exit-on-ACK) passes 0. Returns 1 if the host's wake
 * command was found in the drained bytes: under TIM-wake churn (e.g. a
 * pinged bench) this drain runs every few seconds, and a WAKE landing
 * in its window used to be silently discarded — host thinks it woke
 * us, DUT stays in standby forever (observed live). */
/* Supply for the whole DTIM standby session.
 *
 * The part runs the DCDC at 1.8 V and feeds RF through LDO15RF (which drops it
 * to 1.5 V). Nothing in standby needs 1.8 V, so drop the rail once on the way
 * in and put it back once on the way out -- NOT per DTIM wake. An earlier
 * version toggled it inside the blob at every beacon wait, i.e. twice a second
 * for the whole session; tying it to PS entry/exit instead cuts that to two
 * transitions per session AND covers the parts a per-wake switch cannot: the
 * PDS sleep itself and the entire wake path, not just the ~3 ms beacon wait.
 *
 *   dcdc_top_0 @0x3008F814 [4:0] vout_sel   (SVD: 15 = 1.5 V, 27 = 1.8 V)
 *   rf_top_aon @0x3008F880 [27]  ldo15rf_bypass
 *
 * ORDER IS A SAFETY PROPERTY. Down: lower the DCDC first, then bypass --
 * bypassing while the input is still 1.8 V would put 1.8 V on an RF rail
 * specified at 1.5 V. Up: drop the bypass first, then raise the DCDC.
 * 1.5 V is the floor; the lower DCDC codes must not be used here.
 *
 * Measured with THIS code (2026-08-11, 4 legs alternated 258/0/258/0, PPK2):
 * DTIM-10 standby 341.3 -> 257.9 uA (-24%), arms do not overlap (258: 258.80,
 * 256.93; 0: 351.62, 331.06). Beacon loss is unchanged, 1.36/0.68% low vs
 * 0.68/2.05% high -- the low rail costs no beacons.
 *
 * Moving this out of the blob did NOT buy extra current over the old per-cycle
 * switch (-24% here vs -23% then, same bench, one day apart): PDS sleep is
 * already at the ~30 uA floor, where the DCDC setpoint has almost nothing to
 * contribute. It is here because it is SIMPLER -- two register transitions per
 * standby session instead of two per DTIM cycle -- not because it is cheaper. */
#define TL_DCDC_REG    (*(volatile uint32_t *)0x3008F814u)
#define TL_RFTOP_REG   (*(volatile uint32_t *)0x3008F880u)
#define TL_DCDC_1V5    15u
#define TL_DCDC_1V8    27u

static uint8_t s_supply_low;        /* 1 = rail currently dropped for standby */

static void tickless_supply_low(int low)
{
    if (low) {
        /* Same knob as the wait divider (`lp clkdiv`), so an A/B leg selects
         * both halves of the low-power point or neither. */
        if (s_supply_low ||
            (IOT2LP_PARA->clk_mcu_div & LP_FW_CLK_SUPPLY_LOW) == 0u) {
            return;
        }
        TL_DCDC_REG  = (TL_DCDC_REG & ~0x1Fu) | TL_DCDC_1V5;
        TL_RFTOP_REG = TL_RFTOP_REG | (1u << 27);
        s_supply_low = 1;
    } else {
        if (!s_supply_low) {
            return;
        }
        TL_RFTOP_REG = TL_RFTOP_REG & ~(1u << 27);
        TL_DCDC_REG  = (TL_DCDC_REG & ~0x1Fu) | TL_DCDC_1V8;
        s_supply_low = 0;
    }
}

/* Every route out of PS goes through here, so the rail cannot be left at 1.5 V
 * by a path that returns early -- the standby loop has twelve of them. */
static int tickless_ps_exit(void)
{
    tickless_supply_low(0);
    return wifi_mgmr_sta_ps_exit();
}

/* XTAL32K: today's adopt+probe path (a crystal-less board keeps RC32K).
 * RC32K: crystal down; efuse trim when the part carries one, else a coarse
 * MTIMER cal (lp_demo_cli `lp rctrim` is the same routine); the blob's
 * beacon-TSF trim then keeps it near 0 ppm. The coarse residual seeds the
 * tracker so the first hops predict from it instead of the 0-ppm sentinel.
 * Finally the per-clock parameter set (lp_fw_clk_params) is applied --
 * except rx_lead when `lp track <us>` set it this boot. */
static void tickless_sleep_clock_resolve(void)
{
    volatile iot2lp_para_t *para = IOT2LP_PARA;
    enum lp_clk32k ovr = lp_clk32k_get();
    int on_xtal = 0;

    /* board_init() already resolved the system 32 kHz at boot (the clock runs
     * the PDS/HBN sequencer, so a crystal-less board must not reach this far on
     * a dead selection). Read what it settled on instead of probing again; only
     * a late override (`lp clk32k` after boot) still needs acting on here. */
    on_xtal = (((readl(HBN_BASE + HBN_GLB_OFFSET) & HBN_F32K_SEL_MSK)
                >> HBN_F32K_SEL_POS) == HBN_32K_XTAL) && clock_32k_is_alive();
    if (lp_clk32k_want_xtal()) {
        if (!on_xtal) {
            on_xtal = clock_xtal32k_adopt(1000u);  /* override asked for it after boot */
        }
        printf("[LP-TL] xtal32k: %s\r\n",
               on_xtal ? "in use (resolved at boot)"
                       : "not detected (RTC not moving at crystal rate) -> RC32K");
    } else if (on_xtal) {
        HBN_Keep_On_RC32K();
        HBN_32K_Sel(HBN_32K_RC);
        HBN_Power_Off_Xtal_32K();
        on_xtal = 0;
    }
    if (!on_xtal) {
        int32_t resid = 0, slope = 0;
        if (HBN_Trim_RC32K() == SUCCESS) {
            printf("[LP-TL] rc32k: efuse trim applied\r\n");
        } else {
            int r = clock_rc32k_coarse_cal(200u, &resid, &slope);
            printf("[LP-TL] rc32k: no efuse trim, MTIMER cal %s (%d rounds, %ld ppm left, %ld ppm/LSB)\r\n",
                   r < 0 ? "FAILED" : "done", r, (long)resid, (long)slope);
        }
        para->rc32k_auto_cal_en  = 1;
        para->rc32k_ppm_per_lsb  = slope;
        para->rtc32k_error_ppm  = resid;
        para->ppm_stable_cnt    = 0;
        *(volatile uint32_t *)LP_FW_PPMSEED_STATE_ADDR =
            ((uint32_t)LP_FW_PPMSEED_MAGIC << 16) | 1u;
    } else {
        para->rc32k_auto_cal_en = 0;
    }
    if (!lp_track_user_set()) {
        para->rx_lead_us = lp_fw_clk_params()->rx_lead_us;
    }
    printf("[LP-TL] sleep clock: %s (%s) rx_lead=%lu us auto_cal=%u\r\n",
           on_xtal ? "XTAL32K" : "RC32K",
           ovr != LP_CLK32K_AUTO ? "override" :
#ifdef CONFIG_LP_FORCE_RC32K
           "forced-rc",
#else
           "auto-probe",
#endif
           (unsigned long)para->rx_lead_us, (unsigned)para->rc32k_auto_cal_en);
}

static int tickless_ps_arm_ex(uint32_t settle_ms)
{
    char scratch[64];
    int woken = 0;
    int n;

    if (wifi_mgmr_sta_ps_enter_static() != 0) {
        return -1;
    }
    if (settle_ms != 0u) {
        vTaskDelay(pdMS_TO_TICKS(settle_ms));
    }
    while ((n = tickless_recv_stream(s_sock, scratch, sizeof(scratch) - 1, MSG_DONTWAIT)) > 0) {
        scratch[n] = '\0';
        if (tickless_wake_take()) {
            woken = 1;
        }
    }
    tickless_supply_low(1);
    return woken;
}

static int tickless_ps_arm(void)
{
    return tickless_ps_arm_ex(TICKLESS_ACK_SETTLE_MS);
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
     * but nothing ever read the counters out. The reference SDK gates its
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

    lp_wake_reset(&s_wake_matcher);
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
            lp_wake_reset(&s_wake_matcher);
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
        lp_wake_reset(&s_wake_matcher);
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

#ifdef CONFIG_LWIP_LP
static u32_t s_tcp_fast_saved;       /* fast-timer period to restore */
static uint8_t s_tcp_lp;             /* 1 = precise timers selected here */
#endif

/* Select the precise on-demand TCP timers (on = 1) or restore the saved
 * fast-timer period (on = 0). */
static void tickless_tcp_timers_lp(int on)
{
#ifdef CONFIG_LWIP_LP
    if ((uint8_t)on == s_tcp_lp) {
        return;
    }
    LOCK_TCPIP_CORE();
    if (on) {
        s_tcp_fast_saved = tcp_timer_fast_interval_get();
        tcp_timer_fast_interval_set(0u);
    } else {
        tcp_timer_fast_interval_set(s_tcp_fast_saved);
    }
    UNLOCK_TCPIP_CORE();
    s_tcp_lp = (uint8_t)on;
    printf("[LP-TL] tcp timers %s (fast %lu ms)\r\n", on ? "precise" : "restored",
           (unsigned long)s_tcp_fast_saved);
#else
    (void)on;
#endif
}

#define TICKLESS_TX_AC_VO       3    /* wifi_mgmr_sta_tx_ac_min_set: AC_VO */
static uint8_t s_tx_vo = 1;          /* lp kavo: standby data on AC_VO */
static int s_tx_ac_saved = -1;       /* AC floor to restore, -1 = none held */

/* Standby network profile (on = 1): precise TCP timers and, with
 * `lp kavo 1`, every data frame on AC_VO; on = 0 restores both. */
static void tickless_net_lp(int on)
{
    tickless_tcp_timers_lp(on);
    if (on && s_tx_vo && s_tx_ac_saved < 0) {
        s_tx_ac_saved = wifi_mgmr_sta_tx_ac_min_get();
        (void)wifi_mgmr_sta_tx_ac_min_set(TICKLESS_TX_AC_VO);
    } else if (!on && s_tx_ac_saved >= 0) {
        (void)wifi_mgmr_sta_tx_ac_min_set(s_tx_ac_saved);
        s_tx_ac_saved = -1;
    }
}

/*
 * Wait ms with the idle hook allowed into DTIM PDS; the STA stays associated.
 * No route: awake wait, the supplicant needs the CPU to re-associate. A TIM
 * wake lets its frames in and parks again. Returns 1 on `lp tickless stop`.
 */
static int tickless_park_ms(uint32_t ms)
{
    TickType_t end = xTaskGetTickCount() + pdMS_TO_TICKS(ms);
    int stopped = 0;

    if (!net_al_sta_has_route()) {
        (void)tickless_wait_route(ms);
        return 0;
    }
    if (lpfw_populate_from_sta() != 0 || wifi_mgmr_sta_ps_enter_static() != 0) {
        vTaskDelay(pdMS_TO_TICKS(ms));
        return 0;
    }
    tickless_supply_low(1);
    tickless_net_lp(1);
    s_ps_active = 1;
    s_ka_due = 0u;
    s_pause = 0;
    s_en = 1;
    for (;;) {
        TickType_t now = xTaskGetTickCount();

        if ((int32_t)(end - now) <= 0) {
            break;
        }
        (void)ulTaskNotifyTake(pdTRUE, end - now);
        if (!s_en) {
            stopped = 1;
            break;
        }
        if (s_pause) {
            tickless_ps_exit();
            vTaskDelay(pdMS_TO_TICKS(TICKLESS_ACK_SETTLE_MS));
            if (wifi_mgmr_sta_ps_enter_static() != 0) {
                break;
            }
            tickless_supply_low(1);
            s_pause = 0;
        }
    }
    s_en = 0;
    s_pause = 0;
    tickless_ps_exit();
    tickless_net_lp(0);
    return stopped;
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
 * have failed, or 2 for a received WAKE before the next arm. On failure the
 * caller must stay awake rather than park on
 * a link that cannot wake us.
 */
static int tickless_link_check(void)
{
    char buf[64];
    int n;

    if (s_sock >= 0) {
        /* A pending WAKE is an application command, not stale link-check data. */
        if (s_wake_matcher.pending) {
            return 2;
        }
        while ((n = tickless_recv_stream(s_sock, buf, sizeof(buf) - 1, MSG_DONTWAIT)) > 0) {
            if (s_wake_matcher.pending) {
                return 2;
            }
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
            if (s_wake_matcher.pending) {
                return 2;
            }
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
        lp_wake_reset(&s_wake_matcher);
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
            if (s_wake_matcher.pending) {
                return 2;
            }
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
#ifdef LP_RXW_DIAG
/* Result reason: 1=WAKE in drain, 2=send/recv failure, 3=re-arm failure,
 * 4=WAKE in re-arm drain, 5=normal re-arm. INT32_MIN means not attempted. */
static void tickless_rxw_ka(int reason, int sn, int kn, int send_errno,
                            int recv_errno, int ack, int rearm)
{
    if (LP_RXW_ON) {
        uint32_t data[] = {
            reason, s_ka_sent, sn, kn, send_errno, recv_errno, ack, rearm,
            s_sock, s_en | (s_pause << 8) | (s_ps_active << 16),
            s_rxp_why | ((uint32_t)s_rxp_errno << 8)
        };
        lp_rxw_emit(LP_RXW_RESUME, LP_RXW_KA_RESULT, data, LP_RXW_WORDS);
    }
}
#define RXW_KA(reason, rearm) tickless_rxw_ka(reason, sn, kn, rxw_send_errno, rxw_recv_errno, ack, rearm)
#else
#define RXW_KA(reason, rearm) do { } while (0)
#endif

static int tickless_standby_cycle_body(void)
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
        tickless_ps_exit();
        printf("[LP-TL] wake command before arm -> cycle done\r\n");
        return 1;
    }
    s_pds_entries = 0;
    s_wifi_wakes = 0;
    tickless_ka_phase_witness_reset();
    s_ps_active = 1;                /* static PS announced above */
    s_en = 1;
    s_ka_pending = 0;
    s_ka_sent = 0;
    s_ka_due = xTaskGetTickCount() + pdMS_TO_TICKS(s_ka_ms);
    /* DO NOT call wifi_mgmr_sta_autoconnect_enable() here: repeat mode already
     * enables it once in lp_tickless_start(), so a call here changes no state
     * and only costs a supplicant round-trip right after entering PS. */
    railgate_apply();
    printf("[LP-TL] ARMED: dtim=%lums, host %s:%u wakes us in ~%lus; "
           "idle now enters DTIM PDS\r\n",
           (unsigned long)s_dtim_ms, s_host, (unsigned)s_port,
           (unsigned long)s_delay_s);
    /* Enter standby with peers' ARP cache already warm; the keepalive refresh
     * only starts one TICKLESS_KEEPALIVE_MS later. */
    HEAP_ACC(s_heap_acc_arp, tickless_arp_announce());

    int span = -1;                  /* post-wait branch whose span is open */

    while (s_en || s_pause) {
        HEAP_SPAN_CLOSE(span);
        span = -1;
        /* The standby socket health-check runs at the keepalive cadence.
         * It used to run every 10 s (TICKLESS_SOCK_CHECK_MS), which bounded
         * the blob's residency to ~10 DTIMs and cost 2-3 full-FW wakes per
         * check; the frames it looks for (a FIN/WAKE lwIP consumed while
         * the host happened to be awake) are still caught, one keepalive
         * period later at worst, and every unicast keeps setting the TIM
         * bit. Measured 2026-09-07 with the ceil budget and the supplicant's
         * cleanup timer relaxed: 52 PDS entries per 600 s, 3.99 -> 1.41 mA. */
        TickType_t wait = pdMS_TO_TICKS(s_ka_ms);
        s_dbg_last_wait_src = 0u;
        /* Wake this wait AT the keepalive deadline, not one health-check
         * after it — with the reaper margins above, "up to a period late"
         * is the difference between refreshed and deauthed. */
        if (s_ka_due != 0u) {
            TickType_t now = xTaskGetTickCount();
            if ((int32_t)(s_ka_due - now) > 0 &&
                (TickType_t)(s_ka_due - now) < wait) {
                wait = s_ka_due - now;
                s_dbg_last_wait_src = 1u;
            } else if ((int32_t)(now - s_ka_due) >= 0) {
                /* A WiFi drain may have crossed the real deadline.
                 * Let the existing task backstop run without another period. */
                wait = 0;
                s_dbg_last_wait_src = 2u;
            }
        }
        if (s_repeat != 0u) {
            TickType_t now = xTaskGetTickCount();
            if (now >= deadline) {
                s_en = 0;
                s_pause = 0;
                tickless_ps_exit();
                printf("[LP-TL] no wake command in %lums -> standby guard "
                       "fired after %lu PDS entries\r\n",
                       (unsigned long)guard_ms,
                       (unsigned long)s_pds_entries);
                return 2;
            }
            if (deadline - now < wait) {
                wait = deadline - now;
                s_dbg_last_wait_src = 3u;
            }
        }
        s_dbg_last_wait = wait;
        uint32_t notified;
        HEAP_ACC(s_heap_acc_wait, notified = ulTaskNotifyTake(pdTRUE, wait));
        HEAP_SPAN_OPEN();
        if (notified == 0 && !s_pause &&
            s_en && s_ka_due != 0u &&
            (int32_t)(xTaskGetTickCount() - s_ka_due) >= 0) {
            /* Keepalive BACKSTOP (2026-08-04). The original due-check lives
             * in the post-PDS wake hook, so it only ever runs if PDS entries
             * keep happening — HW-observed: a 6-min standby stopped entering
             * PDS ~27 s in and the keepalive never fired once; the host-side
             * ARP entry decayed, hostapd's inactivity reaper deauthed the
             * STA, and the +360 s host wake died undeliverable in the
             * server's TCP send queue. This timeout path runs on the live
             * scheduler regardless of PDS activity: evaluate the deadline
             * here too and take the minimal-KA branch below. s_pause parks
             * the idle hook across the TX exactly as the wake-hook path
             * does; the KA branch tail releases it. */
            /* The keepalive really goes out from HERE, not from the wake
             * hook's due-check: the hook path only runs while PDS entries
             * keep happening (2026-08-04), so the phase deferral has to live
             * on this backstop too. Hold the deadline just past the shortened
             * cycle, or this backstop fires again before the offset wake. */
            if (tickless_ka_phase_defer()) {
                uint32_t hold = tickless_ka_phase_hold_ms();
                /* The tick count here has NOT yet absorbed the oversleep the
                 * last blob entry banked: the idle hook replays it through
                 * xTaskCatchUpTicks() only after this task blocks. A hold
                 * measured from the un-replayed tick is eaten by that replay
                 * whenever the deficit reaches the hold -- the tick lands
                 * past the deadline before the blob can sleep, this backstop
                 * fires again at once, and the keepalive goes out with no
                 * shortened cycle. HW 2026-09-10, 19 keepalives: every one
                 * with a deficit < 512 ms ran the cycle (ACK 20-56 ms), every
                 * one with >= 512 ms did not (ACK 158-328 ms), no exception.
                 * Add the pending deficit so the hold survives the replay.
                 * Reading it here is race-free: the idle hook is its only
                 * consumer and cannot run until this task blocks.
                 *
                 * Then leave ONE BEACON of slack. hold_ms() is skip beacons
                 * of s_dtim_ms (the arm argument, 1024), but the blob's
                 * shortened cycle is skip REAL beacons (5 x ~101.6 = 508 ms):
                 * with the deficit compensated exactly, the wake lands 2-8 ms
                 * short of a 512 ms hold and the hook books a SECOND
                 * shortened cycle -- 10 beacons, straight back onto the DTIM
                 * boundary (HW 2026-09-10: pdsdelta=2 on 17/19, ACK 142-296
                 * ms). Before the deficit was added that same deficit ate the
                 * few ms by accident, which is why 512 ever worked. The
                 * deadline only has to fall INSIDE the first shortened cycle
                 * and above the WFI-not-PDS floor. */
                {
                    uint32_t beacon = s_ka_phase_saved ?
                        s_dtim_ms / s_ka_phase_saved : 0u;
                    if (hold > beacon + TICKLESS_MIN_PDS_MS) {
                        hold -= beacon;
                    }
                }
                s_ka_due = xTaskGetTickCount() + pdMS_TO_TICKS(hold ? hold : 1u)
                         + pdMS_TO_TICKS(s_tick_deficit_ms);
                s_ka_phase_due0 = s_ka_due;
                s_ka_phase_held = 1u;
                span = HEAP_SPAN_GAP;
                continue;
            }
            s_ka_phase_src = TL_KAPH_SRC_TASK;
            tickless_ka_phase_restore();
            tickless_ka_advance(xTaskGetTickCount());
            s_pause = 1;
            s_ka_pending = 1;
        } else if (notified == 0 && !s_pause) {
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
            int pn = tickless_recv_stream(s_sock, buf, sizeof(buf) - 1, MSG_DONTWAIT);
            if (pn > 0) {
                buf[pn] = '\0';
                printf("[LP-TL] host msg (idle-gap delivery) after %lu PDS "
                       "entries: %s\r\n", (unsigned long)s_pds_entries, buf);
                if (tickless_wake_take()) {
                    s_en = 0;
                    s_pause = 0;
                    tickless_ps_exit();
                    printf("[LP-TL] wake command -> tickless DISARMED\r\n");
                    return 1;
                }
            } else if (pn == 0 || (errno != EWOULDBLOCK && errno != EAGAIN)) {
                s_en = 0;
                s_pause = 0;
                tickless_ps_exit();
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
            tickless_ps_exit();    /* best-effort PM=0 */
            printf("[LP-TL] AP loss after %lu PDS entries -> tickless "
                   "DISARMED, staying awake for reconnect\r\n",
                   (unsigned long)s_pds_entries);
            return 3;
        }
        if (s_ka_pending) {
            /* Keepalive wake (no TIM). This used to be a bare TX with NO PS
             * exit ("static PS is still in effect and a PS STA TXes data
             * fine") — which was true only while the PM bit never reached
             * the air. With the PM-bit mirror fix the AP genuinely buffers
             * for us, so a bare KA leaves the server's TCP ACK in the AP's
             * PS queue -> TIM -> the MAC's auto-PS-Poll delivery dance runs
             * unsupervised against the next PDS entries (r19 air capture:
             * PS-Poll +130 ms after the KA, console dead +0.5 s). Bracket
             * the KA like the TIM drain below: PM=0 so the ACK flows into
             * the drain window while fully awake, then re-announce. */
            s_ka_pending = 0;
            s_ka_sent++;
            /* Cadence proof on the console (stall bisect 2026-08-04): one
             * short line per keepalive, every TICKLESS_KEEPALIVE_MS. */
            printf("[LP-TL] ka #%lu (entries=%lu)\r\n",
                   (unsigned long)s_ka_sent, (unsigned long)s_pds_entries);
            tickless_ps_exit();
            /* Keep the PEER's ARP cache for us warm, unicast: the broadcast
             * form drew ARP replies from two other hosts on LAB10's LAN, held
             * by the AP until DTIM, landing after PM=1 -- one TIM wake per
             * keepalive for nothing (8/8 "WIFI wake without data", 2026-09-10).
             * DTIM standby drops ~42% of
             * broadcast frames (the lp_fw blob ignores the TIM group bit), so
             * a peer whose ARP entry aged out cannot re-resolve us by broadcast
             * ARP and gives up on the flow — the "AP reaped my socket" symptom
             * that is actually ours. We are already awake for the keepalive, so
             * announcing here costs no extra wake. */
            HEAP_ACC(s_heap_acc_arp, tickless_arp_announce());
            int sn;
            HEAP_ACC(s_heap_acc_snd, sn = send(s_sock, "KA\n", 3, MSG_DONTWAIT));
#ifdef LP_RXW_DIAG
            int rxw_send_errno = sn < 0 ? errno : 0;
            int rxw_recv_errno = INT32_MIN;
#endif
            int kn = -1;
            int ack = -2;       /* < 0 keeps the settle (see TICKLESS_KA_ACK_*) */
            if (sn == 3) {
                if (s_ka_ack_exit) {
                    ack = tickless_tx_ack_wait(TICKLESS_KA_ACK_BOUND_MS);
                    if (ack == -1) {
                        s_ka_ack_timeouts++;
                    }
                    /* One look for a WAKE that landed meanwhile; a later
                     * one sets our TIM bit and arrives on that wake. */
                    HEAP_ACC(s_heap_acc_drain,
                             kn = tickless_recv_once(buf, sizeof(buf)));
                } else {
                    HEAP_ACC(s_heap_acc_drain,
                             kn = tickless_recv_poll(buf, sizeof(buf),
                                                     TICKLESS_KA_DRAIN_POLLS));
                }
            }
#ifdef LP_RXW_DIAG
            rxw_recv_errno = sn == 3 ? s_rxp_errno : INT32_MIN;
#endif
            if (kn > 0 && tickless_wake_take()) {
                RXW_KA(1, INT32_MIN);
                s_en = 0;
                s_pause = 0;
                tickless_ps_exit();
                printf("[LP-TL] wake command (keepalive drain) -> "
                       "tickless DISARMED\r\n");
                return 1;
            }
            if (sn != 3 || kn < 0) {
                RXW_KA(2, INT32_MIN);
                s_en = 0;
                s_pause = 0;
                /* Every other standby exit restores PM=0; this one used to
                 * leave static PS announced, so the reconnect loop that
                 * follows ran with the AP still buffering our inbound. */
                tickless_ps_exit();
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
            /* Re-announce PM=1 (the bracket's closing half; see the branch
             * head). Same contract as the TIM drain's re-arm below. */
            {
                int ka_rearm = tickless_ps_arm_ex(ack >= 0 ? 0u
                                                  : TICKLESS_ACK_SETTLE_MS);
                if (ka_rearm < 0) {
                    RXW_KA(3, ka_rearm);
                    printf("[LP-TL] PS re-enter failed -> tickless DISARMED\r\n");
                    s_en = 0;
                    s_pause = 0;
                    return -1;
                }
                if (ka_rearm > 0) {
                    RXW_KA(4, ka_rearm);
                    s_en = 0;
                    s_pause = 0;
                    tickless_ps_exit();
                    printf("[LP-TL] wake command (ka re-arm drain) -> "
                           "tickless DISARMED\r\n");
                    return 1;
                }
                RXW_KA(5, ka_rearm);
            }
            tickless_beat_flush();
            tickless_rx_seen_sync();
            s_pause = 0;                 /* resume PDS: standby continues */
            /* Feeds the phase loop. A timeout (-1) or a socket that is not
             * live (-2) says nothing about the channel, so leave the last
             * good sample standing rather than treating it as "busy". */
            if (ack >= 0) {
                s_ka_last_ack_ms = ack;
            }
            printf("[LP-TL] ka drained (ack %d ms, %lu ack timeouts), PS re-armed, "
                   "PDS resumes\r\n", ack, (unsigned long)s_ka_ack_timeouts);
            if (s_ka_prof_ms) {
                /* -1 = never observed inside the wait. cfm1/cfmN bracket the
                 * TX confirms (n=2 means the gratuitous ARP's confirm is in
                 * here too); uc is the first unicast the AP got back to us. */
                printf("[LP-TL] kaprof #%lu poll=%lums cfm1=%d cfmN=%d n=%lu "
                       "retr=%lu | uc=%d n=%lu | ack=%d | rx tot=%lu "
                       "nosucc=%lu (half %u) nokey=%lu\r\n",
                       (unsigned long)s_ka_sent, (unsigned long)s_ka_prof_ms,
                       s_kap_cfm_first == KAP_NONE ? -1 : (int)s_kap_cfm_first,
                       s_kap_cfm_last == KAP_NONE ? -1 : (int)s_kap_cfm_last,
                       (unsigned long)s_kap_cfm_n, (unsigned long)s_kap_retr,
                       s_kap_uc == KAP_NONE ? -1 : (int)s_kap_uc,
                       (unsigned long)s_kap_uc_n,
                       s_kap_ack == KAP_NONE ? -1 : (int)s_kap_ack,
                       (unsigned long)s_kap_rx_total,
                       (unsigned long)s_kap_rx_nosucc,
                       (unsigned)s_kap_nosucc_half,
                       (unsigned long)s_kap_rx_nokey);
                /* why=2 is the healthy shape: the shift ran at the previous
                 * wake and this is the offset wake carrying the keepalive.
                 * Anything else here means the keepalive went out on the DTIM
                 * boundary, straight into the AP's group flush. */
                if (s_ka_phase_fresh) {
                    printf("[LP-TL] kaphase #%lu why=%u dtim_num=%u src=%u "
                           "skip=%u defers=%lu slept=%lu | pdsdelta=%lu "
                           "duelead=%ld defms=%lu\r\n",
                           (unsigned long)s_ka_sent, (unsigned)s_ka_phase_why,
                           (unsigned)s_ka_phase_user, (unsigned)s_ka_phase_src,
                           (unsigned)s_ka_phase_skip,
                           (unsigned long)s_ka_phase_defers,
                           (unsigned long)s_dbg_last_slept,
                           (unsigned long)s_ka_phase_pdsdelta,
                           (long)s_ka_phase_duelead,
                           (unsigned long)s_ka_phase_defms);
                    s_ka_phase_fresh = 0u;
                } else {
                    /* This keepalive did not come through the phase path (the
                     * unsolicited-DL drain sends its own), so every field
                     * below belongs to an EARLIER keepalive. Print the count
                     * and nothing that would be read as this one's. */
                    printf("[LP-TL] kaphase #%lu STALE (no defer/restore this "
                           "KA) defers=%lu\r\n",
                           (unsigned long)s_ka_sent,
                           (unsigned long)s_ka_phase_defers);
                }
            }
            span = HEAP_SPAN_KA;
            continue;
        }
        /* WIFI wake: PM=0 null -> AP flushes what it buffered, then read. */
        /* Stall bisect (debug scaffold 2026-08-04): the +27 s standby zombie
         * left the service task silent with s_pause held — a block inside
         * ps_exit (cfgrwnx round-trip) is invisible without an entry marker.
         * "drainA" without "drainB" on the console names the blocker. */
        printf("[LP-TL] drainA (entries=%lu)\r\n",
               (unsigned long)s_pds_entries);
        int n;
        {
            extern volatile uint32_t rxl_uc_rx_count;
            uint32_t uc0 = rxl_uc_rx_count;   /* what the AP flushes lands after the null */
            tickless_ps_exit();
            printf("[LP-TL] drainB\r\n");
            n = tickless_wake_drain(buf, sizeof(buf), uc0);
        }
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
            if (tickless_wake_take()) {
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
         * itself fish out a WAKE that raced the PS re-announce. No settle:
         * the drain polls above already held PM=0 for TICKLESS_WAKE_DRAIN_POLLS
         * x 100 ms, so whatever the AP had buffered has been delivered; the
         * extra 500 ms was 37 mC of the 85 mC a TIM wake cost (2026-09-07,
         * NUC ping every 60 s: 1130 -> 636 ms per wake, 10/10 pings answered). */
        if (s_dl_drain_only && !s_en) {
            s_pause = 0;
            tickless_ps_exit();
            return 0;
        }
        int rearm = tickless_ps_arm_ex(0u);
        if (s_dl_drain_only && !s_en) {
            s_pause = 0;
            tickless_ps_exit();
            return 0;
        }
        if (rearm < 0) {
            printf("[LP-TL] PS re-enter failed -> tickless DISARMED\r\n");
            s_en = 0;
            s_pause = 0;
            return -1;
        }
        if (rearm > 0) {
            s_en = 0;
            s_pause = 0;
            tickless_ps_exit();
            printf("[LP-TL] wake command (in re-arm drain) -> cycle done\r\n");
            return 1;
        }
        tickless_beat_flush();
        tickless_rx_seen_sync();
        s_pause = 0;
        span = HEAP_SPAN_WAKE;
    }
    return 0;
}

/* One standby under the standby network profile, restored whenever the
 * standby ends (wake command, stop, dead socket, guard). */
static int tickless_standby_cycle(void)
{
    tickless_net_lp(1);
    int r = tickless_standby_cycle_body();
    tickless_net_lp(0);
    return r;
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
        lp_wake_reset(&s_wake_matcher);
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
     * what earns that ban (see the SAE lab03 bug notes), so this escalation
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
#ifdef LP_RXW_DIAG
    int rxw_out_reason = 0;
    int rxw_cycle_result = INT32_MIN;
#endif

    (void)arg;

    /* Diagnostic: catch the first real fault in TCM. The catcher lives in
     * hal_lpfw_main.c now, next to the WDT that turns its park into a clean
     * reboot — this install covers the running full firmware, hal_lp_main's
     * covers the wake path (where mtvec is still the boot-time exc_entry).
     * Keep the ECLIC mode bits (mtvec[5:0] = 0x3) of the original vector.
     * Zero the heal counter and entry scratch so post-mortem counts are
     * trustworthy from the first session on. */
    *(volatile uint32_t *)0x2104FDF0u = 0u;
    *(volatile uint32_t *)0x2104FDF4u = 0u;
    __asm volatile("csrw mtvec, %0" :: "r"((uint32_t)lp_trap_catcher | 0x3u));

    /* Resolve the sleep clock once per boot, before the first cycle arms a
     * sleep on it (PDS keeps the HBN domain, so wakes never re-pay it). */
    {
        static uint8_t s_clk_resolved;
        if (!s_clk_resolved) {
            s_clk_resolved = 1;
            tickless_sleep_clock_resolve();
        }
    }

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
        int link_result = tickless_link_check();
        if (link_result == 2) {
            s_en = 0;
            s_pause = 0;
            (void)tickless_wake_take();
#ifdef LP_RXW_DIAG
            rxw_out_reason = 6; /* WAKE received before the next standby arm. */
#endif
            tickless_ps_exit();
            printf("[LP-TL] wake command (link check) -> tickless DISARMED\r\n");
            goto out;
        }
        if (link_result != 0) {
            /* The host was unreachable for the whole reconnect budget. In
             * one-shot mode that ends the run. In repeat mode it is almost
             * always a transient TX/link outage — the STA is still associated —
             * so retry rather than abandoning the duty cycle: when the outage
             * clears the next link_check reconnects and cycling resumes with NO
             * external reset. Bounded, so a genuinely dead host eventually lets
             * us stop instead of retrying awake forever. */
            if (s_repeat == 0u && !s_ever_linked) {
#ifdef LP_RXW_DIAG
                rxw_out_reason = 1;
#endif
                goto out;
            }
            if (s_repeat == 0u) {
                uint32_t k = s_link_giveups++;
                uint32_t n = sizeof(s_host_backoff_ms) / sizeof(s_host_backoff_ms[0]);
                uint32_t ms = s_host_backoff_ms[k < n ? k : n - 1u];

                printf("[LP-TL] host unreachable -> retry %lu in %lums (standby)\r\n",
                       (unsigned long)(k + 1u), (unsigned long)ms);
                if (tickless_park_ms(ms)) {
                    goto out;
                }
                continue;
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
            lp_wake_reset(&s_wake_matcher);
            if (s_repeat == 0u) {
#ifdef LP_RXW_DIAG
                rxw_out_reason = 2;
#endif
                goto out;
            }
            continue;       /* next pass rebuilds the socket */
        }
        if (lpfw_populate_from_sta() != 0) {
            printf("[LP-TL] not associated — cannot arm\r\n");
            if (s_repeat == 0u) {
#ifdef LP_RXW_DIAG
                rxw_out_reason = 3;
#endif
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
        IOT2LP_PARA->aid = lp_fw_timonly_effective_aid(
            IOT2LP_PARA->aid, s_timonly_test_aid);
        int r = tickless_standby_cycle();
#ifdef LP_RXW_DIAG
        rxw_cycle_result = r;
#endif
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
                lp_wake_reset(&s_wake_matcher);
            }
            continue;
        }
        if (r == 0 || s_repeat == 0u) {
#ifdef LP_RXW_DIAG
            rxw_out_reason = 4;
#endif
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
#ifdef LP_RXW_DIAG
            rxw_out_reason = 5;
#endif
            goto out;
        }
        /* Duty cycle: a periodic app is awake doing work between standbys.
         * Stay at full power for the window before re-arming. */
        printf("[LP-TL] awake %lums before next cycle\r\n",
               (unsigned long)s_awake_ms);
        vTaskDelay(pdMS_TO_TICKS(s_awake_ms));
    }

out:
#ifdef LP_RXW_DIAG
    if (LP_RXW_ON) {
        uint32_t data[] = {
            rxw_out_reason, rxw_cycle_result, s_repeat, s_cycles_done,
            s_pds_entries, s_wifi_wakes, s_ka_sent, s_sock,
            s_en | (s_pause << 8) | (s_ps_active << 16), errno,
            (uint32_t)(uintptr_t)s_task
        };
        lp_rxw_emit(LP_RXW_RESUME, LP_RXW_SERVICE_EXIT, data, LP_RXW_WORDS);
    }
#endif
    if (s_sock >= 0) {
        close(s_sock);
        s_sock = -1;
        lp_wake_reset(&s_wake_matcher);
    }
    railgate_restore();
    tickless_net_lp(0);
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
        tickless_ka_phase_witness_reset();
        s_pause = 0;
        s_ps_active = 0;            /* no association, no PS dance */
        s_en = 1;
        railgate_apply();
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
    /* The service retries the host for ever, so the STA must re-associate
     * by itself after an AP loss; wifi_mgmr.c starts with
     * disable_autoreconnect = 1. */
    if (wifi_mgmr_sta_autoconnect_enable() != 0) {
        printf("[LP-TL] warning: could not enable STA autoconnect; an AP "
               "loss will not recover\r\n");
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
    railgate_restore();     /* idempotent vs the task's own exit restore */
    if (s_task != NULL) {
        /* The STA is still parked in static PS: exit here in shell-task
         * context (blocking OK) so the DUT is reachable again. */
        tickless_ps_exit();
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

/* Standby keepalive cadence override (ms); 0 queries. Default is
 * TICKLESS_KEEPALIVE_MS -- the product cadence chosen against the measured
 * AP/router reapers above; a longer period is a test setting, not a default. */
uint32_t lp_tickless_keepalive_ms(uint32_t ms)
{
    if (ms != 0u) {
        s_ka_ms = ms;
    }
    return s_ka_ms;
}

/* Same-image cadence candidate: 0/1 sets, -1 queries. Cold boot is OFF. */
int lp_tickless_dl_drain_only(int on)
{
    if (on >= 0) {
        s_dl_drain_only = (on != 0);
    }
    return s_dl_drain_only;
}

/* Unsolicited-DL witness source: 0/1 sets, -1 queries. Cold boot is OFF =
 * the legacy lwIP ip+etharp witness (the same-image A/B control). ON counts
 * unicast-to-us only -- see tickless_dl_witness(). */
int lp_tickless_dl_uc_only(int on)
{
    if (on >= 0) {
        s_dl_uc_only = (on != 0);
        /* The baseline was taken with the OTHER formula: comparing it against
         * this one would manufacture exactly one drain on the next hook pass.
         * Re-base so a knob flip mid-standby is silent. */
        tickless_rx_seen_sync();
    }
    return s_dl_uc_only;
}

/* Keepalive phase: beacons to sleep for the one cycle that carries the
 * keepalive to a quiet part of the AP's DTIM period. 0 disables,
 * (uint32_t)-1 queries. Must be shorter than dtim_num. */
uint32_t lp_tickless_ka_phase(uint32_t beacons)
{
    if (beacons != 0xFFFFFFFFu) {
        s_ka_phase_skip = (uint8_t)(beacons > 255u ? 255u : beacons);
    }
    return s_ka_phase_skip;
}

uint32_t lp_tickless_ka_phase_defers(void)
{
    return s_ka_phase_defers;
}

int lp_tickless_ka_last_ack(void)
{
    return s_ka_last_ack_ms;
}

/* Peripheral clock gating across the keepalive ACK wait: 0/1 sets, -1
 * queries. Cold boot is OFF (the same-image A/B control). */
int lp_tickless_cgen_idle(int on)
{
    if (on >= 0) {
        s_cgen_idle_on = (on != 0);
    }
    return s_cgen_idle_on;
}

uint32_t lp_tickless_cgen_idle_skips(void)
{
    return s_cgen_idle_skips;
}

/* Keepalive ACK-wait profiler: ms sets the poll period and enables it,
 * (uint32_t)-1 queries, 0 disables. A profiled leg polls faster than the
 * product does -- it is a diagnostic, never a power result. */
uint32_t lp_tickless_ka_prof(uint32_t ms)
{
    if (ms != 0xFFFFFFFFu) {
        s_ka_prof_ms = ms;
    }
    return s_ka_prof_ms;
}

/* TCP fast-timer period in ms: sets it unless ms is (uint32_t)-1, returns
 * the period in use; 0 selects the precise on-demand timers. */
uint32_t lp_tickless_tcp_fast_ms(uint32_t ms)
{
#ifdef CONFIG_LWIP_LP
    u32_t v;

    LOCK_TCPIP_CORE();
    if (ms != 0xFFFFFFFFu) {
        tcp_timer_fast_interval_set(ms);
    }
    v = tcp_timer_fast_interval_get();
    UNLOCK_TCPIP_CORE();
    return v;
#else
    (void)ms;
    return 0;
#endif
}

/* Standby data on AC_VO: 0/1 sets, -1 queries; returns the setting. The
 * live floor follows at the next standby entry. */
int lp_tickless_ka_vo(int on)
{
    if (on == 0 || on == 1) {
        s_tx_vo = (uint8_t)on;
    }
    return s_tx_vo;
}

/* Keepalive ARP form: 0 request to the peer, 1 reply to the next hop;
 * (uint32_t)-1 queries. See s_ka_arp_mode. */
uint32_t lp_tickless_ka_arp_mode(uint32_t m)
{
    if (m <= 1u) {
        s_ka_arp_mode = m;
    }
    return s_ka_arp_mode;
}

/* Keepalive exit-on-ACK: 0/1 sets, -1 queries. Off = the fixed 0.5 s drain +
 * 0.5 s settle windows (the same-image A/B control). */
int lp_tickless_ka_ack_exit(int on)
{
#ifdef CONFIG_LWIP_LP
    if (on >= 0) {
        s_ka_ack_exit = (on != 0);
    }
#else
    (void)on;
#endif
    return s_ka_ack_exit;
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

    /* Total oversleep replayed into the kernel clock. The keepalive witness
     * takes a delta of this across a phase deferral: xTaskCatchUpTicks() below
     * advances the tick by everything the blob overslept, which can carry the
     * tick straight past a deadline the backstop only just set. */

    uint32_t d = s_tick_deficit_ms;
    if (d != 0u) {
        s_tick_deficit_ms = 0u;
        s_catchup_total_ms += d;
        xTaskCatchUpTicks(pdMS_TO_TICKS(d));
    }
    /* Standby heartbeat (debug 2026-08-03): the wake0803 pcap showed the 30 s
     * keepalive, the 10 s socket check and the guard deadline ALL silent over
     * 150 s of armed standby -- every one of them keyed off the tick count this
     * function is responsible for advancing. One line per ~16 PDS entries makes
     * the kernel clock's progress visible against wall time without the shell
     * (eaten while armed). */
    if (s_en) {
        static uint32_t last_beat_pds;
        static TickType_t last_beat_tick;
        TickType_t now = xTaskGetTickCount();
        /* Also fire on elapsed TIME: in settled standby a blob residency is
         * ~10 s (sock-check bound), so a pure entry-count gate goes quiet for
         * 160 s and deep standby is indistinguishable from a wedge. */
        if (s_pds_entries - last_beat_pds >= 16u ||
            (last_beat_tick != 0 && (now - last_beat_tick) >= pdMS_TO_TICKS(8000))) {
            last_beat_pds = s_pds_entries;
            last_beat_tick = now;
            /* Record only: this runs inside vApplicationSleep with the scheduler
             * suspended, and the 8 s gate makes it fire at every keepalive wake,
             * so a ~120-character printf here (10 ms at 115200) stalled every
             * task -- the WiFi RX path included -- exactly while the AP's reply
             * to the keepalive was landing (2026-09-07). The service task prints
             * it once PM=1 is re-announced. */
            s_beat_tick = now;
            s_beat_def = d;
            s_beat_pending = 1u;
        } else if (last_beat_tick == 0) {
            last_beat_tick = now;
        }
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

/* Bench scaffold (uncommitted): arm the ME_BUSY fault injection. Fires from
 * the sleep hook <entries> PDS entries after arming; see `lp mebusy`. Lives
 * in the WiFi branch -- the statics it touches only exist there, and the
 * the No-WiFi trim (defconfig_ipro6_lp) is a hard build break otherwise. */
/* Bench: enter/leave TIM-only standby. <cycles> is the forced DTIM budget
 * (0 = normal operation). No keepalive, no drain-on-DL, no OS-deadline
 * bound -- expect the AP to age the STA out eventually; that is the price of
 * measuring the beacon path on its own. */
void lp_tickless_test_timonly(uint32_t cycles, uint32_t test_aid)
{
    s_timonly_cycles = cycles;
    s_timonly_test_aid = cycles != 0u && test_aid <= LP_FW_AID_MAX
                             ? (uint16_t)test_aid
                             : 0u;
    printf("[LP-TL] TEST: TIM-only standby %s (budget %lu DTIM cycles, "
           "test AID %u)\r\n",
           cycles ? "ON" : "OFF", (unsigned long)cycles,
           (unsigned)s_timonly_test_aid);
}

void lp_tickless_test_mebusy(uint32_t entries, uint32_t hold_ms)
{
    s_mebusy_hold_ms = hold_ms ? hold_ms : 5000u;
    s_mebusy_at = s_pds_entries + entries;
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

uint32_t lp_tickless_keepalive_ms(uint32_t ms)
{
    (void)ms;
    return 0u;
}

int lp_tickless_ka_ack_exit(int on)
{
    (void)on;
    return 0;
}

int lp_tickless_dl_drain_only(int on)
{
    (void)on;
    return 0;
}

int lp_tickless_dl_uc_only(int on)
{
    (void)on;
    return 0;
}

uint32_t lp_tickless_ka_prof(uint32_t ms)
{
    (void)ms;
    return 0;
}

uint32_t lp_tickless_ka_arp_mode(uint32_t m)
{
    (void)m;
    return 0;
}

uint32_t lp_tickless_tcp_fast_ms(uint32_t ms)
{
    (void)ms;
    return 0;
}

int lp_tickless_ka_vo(int on)
{
    (void)on;
    return 0;
}

int lp_tickless_cgen_idle(int on)
{
    (void)on;
    return 0;
}

uint32_t lp_tickless_cgen_idle_skips(void)
{
    return 0;
}

uint32_t lp_tickless_ka_phase(uint32_t beacons)
{
    (void)beacons;
    return 0;
}

uint32_t lp_tickless_ka_phase_defers(void)
{
    return 0;
}

int lp_tickless_ka_last_ack(void)
{
    return -1;
}

void lp_tickless_on_disconnect(void)
{
}

void lp_tickless_tick_catchup(void)
{
}



#endif
