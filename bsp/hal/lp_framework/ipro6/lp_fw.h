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
 * @file lp_fw.h
 * @brief IPRO6 standalone low-power firmware (lp_fw) — Stage-1 wake stub.
 *
 * Unlike the IPRO7 resume-in-place model (sleep -> bootrom -> longjmp straight
 * back into the app), IPRO6 needs to do real work between sleep cycles: wake on
 * the DTIM beacon, run a trimmed WiFi RX, check the TIM bitmap, and only wake
 * the full firmware when there is buffered data for us — otherwise go straight
 * back to PDS. That logic cannot run resume-in-place, so it lives in a separate,
 * size-trimmed firmware image (this module) loaded into retained RAM and entered
 * directly by the bootrom on wake.
 *
 * Two stages:
 *   Stage 1 (this module, XTAL 40 MHz, no PLL): restore XIP, WiFi RX, TIM check.
 *           Decision: re-enter PDS (nothing for us) or proceed to Stage 2.
 *   Stage 2 (fw resume): turn on WIFIPLL, hclk=240M / bclk=80M / flash=80M(1T),
 *           hand control back to the full firmware.
 *
 * The WiFi RX / TIM path is developed in lp_fw.c independently; see the
 * iot2lp_para_t contract below (consumed by components/wireless/lpfw_private).
 */
#ifndef __LP_FW_H__
#define __LP_FW_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Wake-cause bitmask reported by lp_fw back to the full firmware (iot2lp_para_t
 * .wakeup_reason). Mirrors the reference LP-firmware causes. */
#define LP_FW_WAKEUP_TIMEOUT    (1u << 0)   /* RTC timeout elapsed */
#define LP_FW_WAKEUP_WIFI       (1u << 1)   /* TIM/beacon flagged data for us */
#define LP_FW_WAKEUP_AP_LOSS    (1u << 2)   /* beacon loss threshold crossed */
#define LP_FW_WAKEUP_IO         (1u << 3)   /* GPIO wake */
#define LP_FW_WAKEUP_ACOMP      (1u << 4)   /* analog comparator wake */

/* Beacon-loss escalation (P3.3 graded two-phase table — see lp_loss_decide
 * below). A cycle that sees no beacon bumps iot2lp continuous_loss_cnt and
 * RE-SLEEPS (retry) instead of waking the full firmware — a single miss is
 * routine (collision/noise). lp_loss_decide(cnt) grades the response: the
 * front phase adds early_wake to the resleep lead to catch a drifted TBTT,
 * the back phase stretches the listen interval to save power, and at
 * LP_FW_LOSS_RECONNECT_CAP consecutive misses the blob wakes the full
 * firmware with LP_FW_WAKEUP_AP_LOSS so its supervision can reconnect. Any
 * received beacon resets the counter. */

/* Stage-1 decision returned by the TIM check. */
typedef enum {
    LP_FW_DECISION_SLEEP = 0,   /* nothing for us -> re-enter PDS */
    LP_FW_DECISION_WAKE  = 1,   /* wake the full firmware (Stage 2) */
} lp_fw_decision_t;

/* Phase-1 DTIM simulation (no WiFi yet): lp_fw re-sleeps for this many beacon
 * intervals (one PDS cycle each, full firmware staying asleep), then on the Nth
 * wake pretends a TIM bit indicates buffered data and wakes the full firmware. */
#define LP_FW_SIM_DTIM_CYCLES   10

#define LP_FW_BCN_DELAY_WIN_MAX 8   /* beacon-delay sliding window depth */

/* --- wake-trace ring (retained OCRAM), MTIME-timestamped ------------------
 * A stamp per wake-path stage, written by BOTH the blob and the carrier. It
 * replaces UART markers on the wake path: a store is ~10 ns where a character
 * at 115200 baud is 87 us, and lp_uart_flush() additionally spins 20000 NOPs
 * (~500 us at XCLK 40 MHz) waiting for the shift register. Those markers cost
 * ~3.5 ms of every wake -- about 40% of the measured wake latency -- and the
 * latency compensation hides that from the beacon schedule while the CPU still
 * burns the time.
 *
 * The defines used to be copy-pasted into four files, and the copies drifted:
 * lp_fw.c wrapped at 184 while lp_demo.c/hal_lp.c/hal_lpfw_main.c wrapped at
 * 200. That is not cosmetic -- 0x2104FFC0-FFCF is where the exception handler
 * parks EXCP/mcause/mepc/mtval, so the 200-byte copies overwrote the crash
 * record every ~200 stamps, destroying exactly the evidence a WDT-reset
 * autopsy needs. One definition, here, is the fix.
 *
 * EACH ENTRY NOW CARRIES A TIME. The order-only ring could prove a stage ran;
 * it could not say what any stage COST, so the 3.18 ms between the bootrom
 * hand-off and blob entry stayed one opaque block worth 4.36 uA, split between
 * BROM, the 40 MHz crystal start-up and our own early init by inference from a
 * single anchor. One u32 per entry: the marker in [31:24], the MTIME delta
 * since the previous entry in [23:0] (16.7 s at 1 MHz -- a wake is 7 ms).
 *
 * UNITS ARE THE WHOLE POINT, and before this the wake path had none. MTIME is
 * XCLK/MTIME_TOGGLE_CNT, and on IPRO6S that cnt is MCU_MISC MAIN_MCU_CFG2
 * (0x30009104[15:0], CPU_Set_MTimer_Toggle_Cnt) -- NOT vendor's mcu_e907_rtc
 * at MCU_MISC+0x14, which is where an 08-19 probe wrongly moved it; the SDK
 * assumes 1 MHz (ipro6.h SOC_TIMER_TICK_PER_US = 1) and board.c sets
 * cnt = XCLK/1e6 at boot. MCU_MISC sits in the CPU domain PDS power-gates, and
 * per lp_fw.c the cnt comes back "un-toggled/frozen" -- MTIME does not merely
 * run at the wrong rate across the early wake, it does not count at all until
 * something programs it, which used to be the blob, ~3.2 ms in. hal_lp_main now
 * programs it ahead of stamp '0'.
 *
 * The cnt is DERIVED, not assumed: XCLK is RC32M or XTAL40M depending on
 * HBN_GLB[0] (HBN_ROOT_CLK_SEL bit 0; bit 1 picks xclk vs pll), so 32 or 40.
 * IPRO6 runs RC32M + XTAL40M and this bench reads HBN_GLB[1:0] = 0b11, i.e.
 * XCLK = XTAL40M -> 40. Deriving also keeps MTIME at 1 MHz across the blob's
 * own RC32M -> XTAL switch, where a hardcoded cnt would change the tick rate
 * in the middle of the very trace this ring exists to produce.
 *
 * hdr[0] magic  hdr[1] total stamps  hdr[2] toggle cnt  hdr[3] last MTIME */
/* Wake-trace ring, v2 (timestamped).
 *
 * Layout at LP_FW_DBG_RING_BASE, 192 B total, ending exactly at the 0x2104FFC0
 * exception park:
 *   hdr[0] magic 'DART'   hdr[1] write count   hdr[2] MTIME cnt readback
 *   hdr[3] previous stamp's MTIME     entries[44] at +16, one word each
 * Entry = char << 24 | payload[23:0]. Payload is the MICROSECONDS SINCE THE
 * PREVIOUS STAMP for a marker, or a raw value for the '#' (LP_FW_DBG_VAL_MARK)
 * entries lp_fw_dbg_puthex() writes -- a '#' does not advance the clock, so a
 * value in the middle of the trace does not distort the delta after it. Any
 * reader that sums the payload column MUST skip '#'.
 *
 * 44 entries is a HARD ceiling, not a tuning knob: the exception park sits at
 * 0x2104FFC0 above, and the bootrom's BPRM flash-config block owns 0x2104FE80
 * below (see LP_BCN_RING_ADDR), so this page cannot grow in either direction.
 *
 * The buffer is continuous ACROSS wakes (see lp_fw_dbg_init), which is what
 * makes the cheap cycle readable at all: a resleep runs ~28 stamps and a
 * full-FW wake ~24, so a read taken once the console is back still carries the
 * resleep before it, and what falls off the end is the leading 0-7 stub run --
 * the most fixed and least interesting part -- while '@' onward survives.
 * Deltas never cross a wake boundary: '0' re-anchors, because MTIME does not
 * run in PDS.
 *
 * v1 stored one char per byte and no time at all, which answered "did we get
 * this far" but never "what did it cost" -- and the cost breakdown of the wake
 * is the entire reason the ring exists. t=0 is hal_lp_main's first stamp: MTIME
 * is dead until the stub programs the toggle count (PDS power-gates MCU_MISC),
 * so everything before that -- PDS exit, the ~1.6 ms crystal settle, BROM -- is
 * invisible here by construction. That span is a PPK2 measurement, not a
 * firmware one.
 *
 * Markers, in the order a normal wake emits them:
 *   stub   0 entry  1 caches  2 APB clk  3-6 saved_reg 0..3  7 uart+pinmux
 *          @ uart live  A early init done
 *   blob   e entry  P xtal+PLL locked  E 240M + bring-up regs done
 *          p wl_lp_init  q wl_lp_config  r rwnx_platform
 *          s rwnxl_init  u rwnx_lpfw_init  k first beacon check  d first delay
 *          b beacon rx  f find_tim done
 *          then one of: t TIM hit->WAKE (< > settle) | o budget->WAKE |
 *                       n no TIM->resleep | l loss (-> y retry / B budget /
 *                       L cap->reconnect)
 *   carrier (lp_demo.c, after the blob returns its decision)
 *          H entering staged blob  K blob skipped (bench toggle)
 *          S re-entering PDS       W waking the full FW
 *   resleep (hal_pm.c)  R entry  x armed  a #sleep_time  . #dbg_lo
 *                       w cfg done  ! WFI
 *   stage-2 C hal_lp c_entry (full-FW resume begins)  X XIP restored
 *   faults  M MTIME frozen in a delay   ? exception trapped (mcause/mepc
 *           land at 0x2104FFE0/E4). NOT 'X' -- 'X' is hal_lp's "XIP
 *           restored", which every healthy full-FW wake emits.
 *
 * '.' is BOTH a real marker (hal_pm.c, the separator before the second hex
 * value) and what the CLI substitutes for a non-printable byte. In a healthy
 * trace it only ever appears right after 'a', so anywhere else means a corrupt
 * entry, not a separator.
 *          a+n from lp_s2_spin is a PARK, not the resleep 'a' -- tell them apart
 *          by position: a park is the last entry in the ring.
 * hal_pm.c's 'E' (line 557) and the blob's 'E' collide; same rule applies. */
#define LP_FW_DBG_RING_BASE  0x2104FF00u
#define LP_FW_DBG_RING_MAGIC 0x54524144u    /* 'DART' -- v2, timestamped */
#define LP_FW_DBG_RING_CAP   44u            /* (0xC0-0x10)/4, clear of the park */
#define LP_FW_DBG_MTIME_REG  0x30009104u    /* MCU_MISC_MAIN_MCU_CFG2, toggle cnt = [15:0] */
#define LP_FW_DBG_VAL_MARK   '#'            /* entry carries a value, not a delta */

/* Product power path keeps retained wake telemetry off. Diagnostics opt in
 * explicitly with `lp telemetry on`; the exact magic rejects POR garbage. */
#ifndef LP_FW_TELEMETRY_KNOB_ADDR
#define LP_FW_TELEMETRY_KNOB_ADDR 0x2104FDB4u
#endif
#define LP_FW_TELEMETRY_MAGIC 0xD1A60001u

__attribute__((always_inline)) static inline int lp_fw_telemetry_enabled(void)
{
    return *(volatile uint32_t *)LP_FW_TELEMETRY_KNOB_ADDR ==
           LP_FW_TELEMETRY_MAGIC;
}

#define LP_FW_HBN_GLB_ADDR          0x3008F030u
#define LP_FW_MTIME_CFG_ADDR        0x30009104u
#define LP_FW_XCLK_RC32M_MHZ        32u
#define LP_FW_XCLK_XTAL40M_MHZ      40u
#define LP_FW_AID_MAX               2007u

/* Measurement-only TIM isolation: zero or an invalid override keeps the real
 * association AID. Normal standby never supplies an override. */
__attribute__((always_inline)) static inline uint16_t
lp_fw_timonly_effective_aid(uint16_t associated_aid, uint16_t test_aid)
{
    return test_aid > 0u && test_aid <= LP_FW_AID_MAX
               ? test_aid
               : associated_aid;
}

/* XCLK and MTIME_TOGGLE_CNT are one clock-state transition. Keeping their
 * writes here prevents a caller from selecting RC32M with cnt=40 (0.8 MHz
 * MTIME) or XTAL40M with cnt=32 (1.25 MHz MTIME). Root is forced to XCLK too:
 * the wake stub must leave the dead pre-sleep PLL root before using MTIME.
 *
 * always_inline is required because the wake stub calls this before XIP is
 * restored. The two stores cannot be physically simultaneous; selector first
 * bounds the mismatched interval to the following APB store. Callers must only
 * select XTAL40M after AON_XTAL_RDY is observed. */
__attribute__((always_inline)) static inline void
lp_fw_sync_mtime_to_xclk(void)
{
    volatile uint32_t *glb = (volatile uint32_t *)LP_FW_HBN_GLB_ADDR;
    volatile uint32_t *cfg = (volatile uint32_t *)LP_FW_MTIME_CFG_ADDR;
    uint32_t mhz = (*glb & 1u) ? LP_FW_XCLK_XTAL40M_MHZ
                               : LP_FW_XCLK_RC32M_MHZ;

    *cfg = (*cfg & ~0xFFFFu) | mhz;
}

__attribute__((always_inline)) static inline void
lp_fw_select_xclk_mtime(uint32_t select_xtal40m)
{
    volatile uint32_t *glb = (volatile uint32_t *)LP_FW_HBN_GLB_ADDR;

    *glb = (*glb & ~3u) | (select_xtal40m ? 1u : 0u);
    lp_fw_sync_mtime_to_xclk();
}

/* always_inline is load-bearing, not a hint: the carrier stamps this ring from
 * TCM-resident wake-path code that runs BEFORE XIP is restored (hal_lp.c
 * lp_s2_spin, hal_lpfw_main.c early init). An out-of-line copy would be emitted
 * into .text in XIP flash that is not mapped yet, and the wake dies there --
 * observed 2026-08-08 as a mid-measurement reboot with every tracking field
 * back at its cold-boot zero. */
__attribute__((always_inline)) static inline uint32_t lp_fw_dbg_mtime(void)
{
    uint32_t lo;
    __asm__ volatile("csrr %0, 0xC01" : "=r"(lo));   /* time (mtime low) */
    return lo;
}

/* Branchless on purpose: this is always_inline at ~23 sites in the blob, and
 * carrying the magic check inline overflowed the blob's RAM region by 144 B.
 * Initialisation moved to lp_fw_dbg_init(), called once per wake from the stub
 * -- which also makes each dump exactly one cycle, wake through resleep. An
 * uninitialised ring still writes in bounds (hdr[1] % CAP), just with garbage
 * deltas, and the next wake's init clears it. */
__attribute__((always_inline)) static inline void lp_fw_dbg_putc(char c)
{
    volatile uint32_t *hdr = (volatile uint32_t *)LP_FW_DBG_RING_BASE;
    volatile uint32_t *buf = (volatile uint32_t *)(LP_FW_DBG_RING_BASE + 16u);
    uint32_t now = lp_fw_dbg_mtime();
    buf[hdr[1] % LP_FW_DBG_RING_CAP] =
        ((uint32_t)(uint8_t)c << 24) | ((now - hdr[3]) & 0x00FFFFFFu);
    hdr[3] = now;
    hdr[1]++;
}

/* always_inline for the same reason lp_fw_dbg_putc is: hal_lp_main is
 * ATTR_TCM_SECTION and calls this before XIP is restored, so an out-of-line
 * copy in .text would be a call into unmapped flash. Both ELFs happen to
 * inline it at -Os today -- that is the optimiser's choice, not a guarantee,
 * and the failure mode is a hard hang on the first wake. Single call site, so
 * forcing it costs nothing. */
__attribute__((always_inline)) static inline void lp_fw_dbg_init(void)
{
    volatile uint32_t *hdr = (volatile uint32_t *)LP_FW_DBG_RING_BASE;
    /* Do NOT reset the write cursor per wake. A DTIM standby spends most of its
     * cycles in the cheap path -- wake, no TIM, resleep -- and those are the
     * ones that dominate the energy, but they are also the ones you can never
     * read back: the console only exists after a full-FW wake, so a ring that
     * restarts every wake can only ever show you the expensive cycle. Letting
     * it run as one continuous circular buffer costs nothing and keeps the tail
     * of the previous cycle(s); '0' marks each wake boundary. */
    if (hdr[0] != LP_FW_DBG_RING_MAGIC) {
        hdr[0] = LP_FW_DBG_RING_MAGIC;
        hdr[1] = 0u;
    }
    /* the cnt actually IN the register, not the one anyone asked for: if the
     * stub's write ahead of stamp '0' has not landed, every delta is
     * meaningless and this field is what says so. 40 = XCLK XTAL40M, 32 =
     * RC32M. */
    hdr[2] = *(volatile uint32_t *)LP_FW_DBG_MTIME_REG & 0xFFFFu;
    /* Re-anchor per wake: MTIME is power-gated in PDS, so a delta measured
     * across a sleep is not a sleep duration, it is garbage. '0' therefore
     * always reads ~0 and means "new wake starts here". */
    hdr[3] = lp_fw_dbg_mtime();
}

/* One entry, not eight. The old version emitted a nibble per character, so the
 * two values hal_pm.c logs per sleep cost 16 of the ring's slots -- more than a
 * third of it -- and a cycle no longer fit. 24 bits covers a sleep interval in
 * us (16.7 s) and the low bits of an RTC stamp, which is what it logs. */
__attribute__((always_inline)) static inline void lp_fw_dbg_puthex(uint32_t v)
{
    volatile uint32_t *hdr = (volatile uint32_t *)LP_FW_DBG_RING_BASE;
    volatile uint32_t *buf = (volatile uint32_t *)(LP_FW_DBG_RING_BASE + 16u);
    buf[hdr[1] % LP_FW_DBG_RING_CAP] =
        ((uint32_t)(uint8_t)LP_FW_DBG_VAL_MARK << 24) | (v & 0x00FFFFFFu);
    hdr[1]++;
}

/**
 * @brief Shared parameter block between the full firmware and lp_fw.
 *
 * Lives in its own retained HBN-RAM region (HAL_HBNRAM_IOT2LP_*), SEPARATE from
 * the resume-in-place register snapshot (hal_lp_param_t) — both must survive PDS
 * because Stage-2 resumes the full firmware via that snapshot. Field names match
 * what components/wireless/lpfw_private/lpfw_wifi.c consumes, so it can be wired
 * in unchanged during Phase 2.
 *
 * Phase 1 keeps only the wake-interface fields populated; the WiFi
 * RX / beacon-tracking fields below are declared for the Phase-2 wire-up and
 * are otherwise zero.
 */
typedef struct {
    /* --- wake interface (Stage-1 <-> Stage-2) --- */
    int32_t  wakeup_reason;             /* LP_FW_WAKEUP_* bitmask, set by lp_fw */
    uint16_t aid;                       /* association id for TIM bitmap match */
    uint8_t  rtc_wakeup_en;             /* RTC timeout wake enabled */
    uint8_t  do_xip_recovery;           /* full fw must rebuild XIP on resume */

    /* --- WiFi / AP context (Phase 2 populates from the connected STA) --- */
    uint8_t  bssid[6];                  /* target AP MAC */
    uint8_t  local_mac[6];              /* our STA MAC */
    uint8_t  ap_channel;
    uint16_t beacon_interval_tu;        /* beacon interval, TUs */
    uint8_t  dtim_num;                  /* DTIM period the flow USES. Owned by
                                         * the user (`lp dtim <n>`), never by
                                         * the AP — see ap_dtim_num_seen. */
    uint8_t  bcn_target_level;

    /* --- beacon timestamp tracking (Phase 2) --- */
    uint64_t last_beacon_stamp_rtc_us;
    uint64_t last_beacon_stamp_beacon_us;
    uint8_t  last_beacon_stamp_rtc_valid;
    /* C1 candidate chain (padding hole, no offset shift): last rejected
     * sample's low words + run length of mutually consistent rejects. */
    uint8_t  trk_cand_cnt;
    uint16_t trk_armed_extra_us;   /* TRKW: lead applied at the last arm beyond
                                    * rx_lead_us (early-wake, acquire, churn,
                                    * carrier extra); LP_FW_TRKW_UNTRACKED when
                                    * the sleep was not a tracked hop */
    uint32_t trk_cand_tsf_lo;

    /* --- RC32K calibration from beacon drift (Phase 2) --- */
    uint64_t last_rc32trim_stamp_rtc_us;
    uint64_t last_rc32trim_stamp_beacon_us;
    uint8_t  last_rc32trim_stamp_valid;
    uint8_t  rc32k_auto_cal_en;         /* carrier: 1 only when RC32K is the sleep clock */
    uint8_t  rc32k_trim_hold;           /* catches left before the next trim step */
    uint8_t  rc32k_trim_cnt;            /* trim steps applied this session (bench witness) */
    int32_t  rc32k_ppm_per_lsb;         /* CODE_FR_EXT slope measured by the boot cal; 0 = nominal */

    int32_t  rc32k_fr_ext;              /* RC32K frequency trim code */
    int32_t  rtc32k_error_ppm;          /* measured RC32K error, ppm */

    /* --- beacon-delay sliding window (Phase 2) --- */
    int32_t  bcn_delay_offset;
    int32_t  bcn_delay_sliding_win_buff[LP_FW_BCN_DELAY_WIN_MAX];
    uint32_t bcn_delay_sliding_win_point;
    uint32_t bcn_delay_sliding_win_size;
    uint32_t bcn_delay_sliding_win_status;
    int32_t  last_beacon_delay_us;
    uint32_t bcn_loss_level;
    uint32_t continuous_loss_cnt;

    /* --- Phase-1 DTIM simulation (no WiFi yet) --- */
    uint32_t resleep_us;        /* PDS interval to use when lp_fw returns SLEEP */
    uint32_t sim_dtim_count;    /* beacons elapsed; wake when it reaches sim_dtim_target */
    uint32_t sim_dtim_target;   /* DTIM cycles to re-sleep before wake; 0 => LP_FW_SIM_DTIM_CYCLES */

    /* --- Phase-2 beacon-buffer address map (lpfw_wifi.c lpfw_wifi_addr_init) --- *
     * These hold the runtime addresses of the MAC DMA beacon ring fields that
     * lp_fw parses after the WiFi RX completes.  Populated by lpfw_wifi_addr_init()
     * before the first PDS entry; read by every DTIM wake.
     */
    uint32_t addr_beacon_start;
    uint32_t addr_beacon_bssid;
    uint32_t addr_beacon_fixed_part;
    uint32_t addr_beacon_interval_part;
    uint32_t addr_beacon_variable_part;
    uint32_t addr_beacon_subtype;
    uint32_t ram_wifi_start_addr;
    uint32_t beacon_data_rate;  /* data rate code written by full-FW beacon hook */

    /* Vendor-compat pointer: lpfw_static_val_addr_init() stores the address of
     * the lp_fw's local static-variable block here so the full FW can locate it.
     * Stored as void* to keep lp_fw.h self-contained (no external LP header dependency). */
    void    *lpfw_static_val;

    /* --- Phase-3: DTIM score / beacon tracking (append-only: field offsets
     * above are blob<->carrier ABI). The score record ring itself lives in
     * retained OCRAM inside the blob slot (see LP_BCN_RING_ADDR), NOT here —
     * the 256B HBN budget only carries the pointer + config scalars. --- */
    uint32_t bcn_ring_addr;     /* lp_bcn_ring_t* (uncached OCRAM alias); 0 = off */
    uint32_t rx_lead_us;        /* P3.2: tracking RX-window lead; 0 = fixed resleep */
    uint32_t ppm_stable_cnt;    /* consecutive ppm samples within band; gates the
                                 * poll-window shrink (see lp_fw_track_converged) */

    /* --- APPEND ONLY below this line ------------------------------------
     * This struct is a shared-memory ABI: the host and the staged blob are
     * linked separately and meet here in HBN RAM. Inserting a field above
     * silently re-points every field after it for whichever object was not
     * recompiled -- done on 2026-08-08 and the blob hunted beacons at the
     * wrong addresses (100% loss, poll giving up in <2 ms). */
    uint8_t  ap_dtim_num_seen;          /* DTIM period the AP advertises (TIM
                                         * ie_data[1]). REFERENCE ONLY -- the
                                         * schedule follows dtim_num, which the
                                         * user owns. */
    int32_t  wake_late_us;              /* learned: how much later than asked
                                         * the chip really wakes (PDS arm +
                                         * XTAL settle + blob entry); carried
                                         * in the lead so the asked-for lead is
                                         * the delivered lead. */
    uint32_t last_lead_applied_us;      /* the lead THIS hop was armed with --
                                         * base + loss-ladder extra + the
                                         * learned offset. Telemetry only now
                                         * (`lp track` prints it); the wake
                                         * latency is measured directly at blob
                                         * entry, see armed_wake_rtc_us. */
    uint32_t armed_wake_rtc_us;         /* low 32 bits of the absolute RTC-us
                                         * this hop MEANT to wake at, written by
                                         * lp_fw_track_next_sleep_gap_us and
                                         * consumed once at blob entry to
                                         * measure the wake latency. 0 = the
                                         * armed sleep did not come from the
                                         * tracker -- do not measure against it. */

    /* The wake latency is a property of the ARM PATH, not of the board, so it
     * is learned twice. The blob arms from a trimmed stage-1 with the scheduler
     * already frozen; the carrier arms after a full-firmware resume, with WiFi
     * suspend + the retention-stack switch + PDS entry still ahead of it.
     * Pooling them into one IIR moved the estimate from 284-288 us to 1079 us
     * as soon as carrier hops started reaching it (2026-08-08) -- a number that
     * described neither path and over-led every blob hop by ~800 us.
     * armed_by_carrier records which path stamped armed_wake_rtc_us so the
     * sample lands in the right accumulator. */
    int32_t  wake_late_carrier_us;      /* learned latency, carrier-armed hops */
    uint8_t  armed_by_carrier;          /* 1 = armed_wake_rtc_us is a carrier arm */

    /* Post-beacon MAC quiesce before re-entering PDS, us. 0 = the built-in
     * LP_FW_QUIESCE_DEFAULT_US. Runtime-settable (`lp quiesce <us>`) so the
     * value can be A/B'd by ALTERNATING legs inside one power cycle instead of
     * one reflash per leg -- the failure it guards against accumulates over
     * cycles, and single before/afters on this bench have produced retractions. */
    uint32_t quiesce_us;

    /* Extra lead added to CARRIER-armed hops only, us. 0xFFFFFFFF = unset ->
     * the built-in LP_CARRIER_EXTRA_LEAD_US (0 is a legitimate value here, so
     * it cannot double as "unset" the way quiesce_us does).
     *
     * This padding was sized on 2026-08-08 against carrier hops that were
     * landing ~2 ms late because the wake-latency estimator pooled both arm
     * paths and so never learned the carrier's own cost. 8ad430047 split the
     * accumulator, wake_late_carrier_us now measures 533-556 us directly, and
     * the padding has been sitting on top of that correction ever since.
     * Runtime-settable so the two can be separated by measurement rather than
     * by argument -- at `lp timonly 2` (re-entry every other cycle, the
     * product cadence) 3.5 ms of padding is 1.75 ms/cycle, ~8.9 uA, far above
     * the bench's noise; at the bench's own ~1-in-17 re-entry rate it is 0.2
     * ms/cycle and invisible. */
    uint32_t carrier_lead_us;

    /* rx_listen_park (`lp rxpark 1`, default 0): HW-debug mode. ==1 arms a
     * FOREVER busy-loop park at the top of the next wake's RX listen (RX
     * armed, wait divider + LISTENGATE in force) so the HW team can attach
     * CCI and read a static plateau. The blob validates ==1 exactly (HBN
     * POR garbage cannot park a wake); cold boot clears it (lp_demo.c).
     * The park never returns and never enters PDS -- exit is the relay /
     * a power cycle. The CGEN0 gate is skipped while armed so a NEW CCI
     * session can still be established at the park. */
    uint32_t rx_listen_park;
    uint32_t trk_cand_rtc_lo;   /* C1 candidate RTC low word (was reserved0) */

    /* Bench (2026-08-10): CPU/bus divider applied ONLY across the beacon wait.
     * 0 = off (240/120 for the whole wake, shipped behaviour); n = 240/(n+1)
     * for BOTH hclk and bclk while waiting, restored before find_tim.
     *
     * Placement copies the reference SDK's lp_fw -- drop once the MAC RX is
     * armed, raise again to parse the TIM. It targets the beacon wait, 71% of
     * the cycle's charge (3006 us at 78.8 mA), and leaves the MAC/RF bring-up
     * at 240 MHz where it is known good.
     *
     * 3 (60 MHz) is the floor measured in the carrier: 60 MHz ran 3/3 clean
     * (ping 6/6), 48 MHz failed 2/2. mac_core is moved to 40 MHz with it,
     * because bclk must stay ABOVE mac_core -- equal was only marginal.
     *
     * (This word previously selected the whole-wake clock and a "skip the
     * switch entirely" mode. Both were tested to destruction -- every route to
     * a 40 MHz wake reboots -- so the word is reused rather than adding a
     * field: iot2lp_para_t is at its 256-byte HBN-RAM limit, enforced by a
     * _Static_assert in hal_lp.c which fired when this was added as a new one.)
     *
     * Historical note on the old meaning: n gave 240/(n+1) for the whole wake.
     *
     * Worth doing because the stage ladder prices this as the biggest single
     * item in the wake: pre-PLL is 13.8 mA (stage 2) and the part is 60.2 mA
     * the moment PLL + 240M + the WiFi AHB clocks are up (stage 3) -- 46.4 mA,
     * against 17.5 mA for the radio and 15.7 mA for the core's dynamic power.
     *
     * A DIVIDER, deliberately, not a source change. The reference SDK's own
     * lp_fw never runs its blob at
     * 240 MHz: it keeps WIFIPLL as the source and writes GLB_REG_HCLK_DIV --
     * 40 MHz for the beacon wait, back up to parse the TIM. (Its
     * lpfw_set_hclk_div() ignores its own `div` argument and always writes 7,
     * so in practice the shipping vendor blob is at 40 MHz throughout.)
     *
     * This is NOT the experiment the call site records as dead on 2026-08-09:
     * those moved the ROOT MUX to XCLK and the part rebooted on the first PDS
     * wake. The MAC core clock derives from the system clock, so keeping
     * WIFIPLL as the source and only dividing is the form that should survive. */
    uint32_t clk_mcu_div;

} iot2lp_para_t;


/* Flag bit inside clk_mcu_div: also drop the supply (DCDC 1.8 -> 1.5 V and
 * bypass the now-redundant LDO15RF) for the DTIM standby session.
 *
 * The BLOB does not read this bit -- the carrier does, in lp_tickless.c, which
 * drops the rail once when it enters PS and raises it once when it leaves. It
 * lives here anyway so one knob (`lp clkdiv`) still selects both halves of the
 * shipped low-power point, and it rides inside clk_mcu_div because
 * iot2lp_para_t is at its 256-byte HBN-RAM limit -- adding a field trips the
 * _Static_assert in hal_lp.c. Low 3 bits stay the blob's wait divider. */
#define LP_FW_CLK_SUPPLY_LOW  0x100u



/* Flag bit inside clk_mcu_div: park the core in WFI for each poll step of the
 * beacon wait instead of spinning in arch_delay_us. Worth -4.2 mA at
 * 80 MHz/1.5 V (stage-7 vs stage-5 hold, alternated, loss 0.00%). The wait
 * chunking is unchanged -- one WFI per LP_FW_POLL_STEP_US, so a WFI that
 * returns immediately degrades to today's spin, never hangs. */
#define LP_FW_CLK_WFI_WAIT    0x200u

/* Flag bit inside clk_mcu_div: hold mac_core at its 60 MHz boot default
 * (wlp1_cfg0[27:24]=1, 240/4) across the beacon wait instead of the blob's
 * 40 MHz (field 0, 240/6). Carrier rule: plf(bclk) must stay STRICTLY above
 * mac_core, so pair only with a wait of 80 MHz+ (low bits <= 2).
 * OBSOLETE NOTE (2026-08-18): mac_core is hard-pinned at 60 MHz on this
 * silicon -- wlp1[27:24] writes hold the register value but never retune the
 * clock (fmetre-proven at every placement), so this flag and the pairing
 * rule above are decorative. Kept for the knob ABI only. */
#define LP_FW_CLK_MAC60       0x8u

/* SHIPPED DEFAULT for the beacon wait: 40 MHz + DCDC 1.5 V + LDO15RF bypass,
 * paired with the blob's per-wake plf-80 CRM stage (LP_FW_CRM_STAGE_DEFAULT).
 *
 * Measured on 2026-08-11, alternated in one run, beacon loss flat across arms:
 *   poll loop   78.75 -> 46.77 mA   (-41%)
 *   DTIM-10     341.3 -> 257.9 uA   (-24%), the two groups do not overlap
 * (that was the original 80 MHz point; the further 80 -> 40 step is worth
 *  another ~-5 mA of wait plateau, -40..60 uJ/cyc, A/B/A 2026-08-18.)
 *
 * Why 40 MHz is legal now -- the HW bridge rule (2026-08-18): 2*bclk >=
 * mac clock >= bclk, binding ONLY between BCLK and the MAC-side clocks, and
 * only while the MAC is enabled. The MAC-side clocks do NOT follow the GLB
 * dividers: mac_core/mac_wt are hard-pinned at 60 MHz on this silicon and
 * plf is a WIFIPLL/8 CRM tap, 120 MHz stock -- CRM/wlp1 writes do not latch
 * on a running MAC; the divider/mux samples on the WLAN domain bring-up,
 * which only the blob re-runs, once per PDS wake. With the blob staging
 * plfclksel=1 each wake, the wait runs bclk 40 / mac 60 / plf 80: every
 * mac-side clock sits inside [bclk, 2*bclk] = [40, 80] (mac 60 strictly,
 * plf 80 on the 2*bclk boundary -- HW-blessed "should be ok", then
 * qualified by 2x 30-min soaks at loss ~3.0%). The full-FW resume relatches
 * plf 120 (where bclk 120 is legal again), so no out-of-rule span exists
 * anywhere in the flow. At stock plf 120 the old bands were: 80 legal, 60
 * the 2*bclk==plf boundary ("failed 1 of 4 legs overnight"), 48/40 outside
 * (48 failed 2/2) -- a 40 MHz wait WITHOUT the plf stage must not ship.
 * 1.5 V and not lower: that is the RF block's safe floor -- the DCDC has
 * 1.15/1.125 V codes and they must not be used here.
 *
 * WFI wait added to the default 2026-08-11 pm: 16 captures alternated
 * 770/258/770/258, wait plateau 47.68 vs 52.14 mA (zero overlap, -4.46 mA,
 * matching the stage-7 prediction), loss identical to control -- worth
 * ~-11 uA of DTIM-10 standby.
 *
 * Set to 0 to get the old 240 MHz / 1.8 V spin behaviour back (`lp clkdiv 0`).
 *
 * Scope note: the 80 MHz half applies only to the blob's beacon wait, but the
 * supply half covers the WHOLE standby session -- PDS sleep and the entire
 * wake path, not just the wait -- because the carrier owns it.
 *
 * SUPPLY_LOW left the default 2026-09-05: the whole-standby 1.5 V rail is the
 * REJECTED form documented at LP_FW_RAILGATE_AON_DCDC (stochastic PDS-entry
 * brown-out: the ~4 mA resleep-WFI hang / 1.1 mA park caught in every
 * default-config catcher leg at win 2..117, median ~35, and never in seed
 * legs, which skip tickless_ps_arm()). Same-image A/B `lp clkdiv 773` vs
 * `517` on 2026-09-05. The listen-window-only depth (DCDCLG) and the
 * soak-qualified RAILGATE sel 21 remain the standby-rail savings paths.
 * `lp clkdiv 773` still selects the old point for A/B work. */
#define LP_FW_CLK_WAIT_DEFAULT  (5u | LP_FW_CLK_WFI_WAIT)

/*
 * Phase-3 DTIM score: one record per blob wake cycle + KPI aggregates.
 *
 * KPI-1 = beacon loss (recv_cnt/loss_cnt). KPI-2 = idle time from PDS wake to
 * the first beacon: t_ready_us (blob entry after MTIME setup -> MAC RX armed,
 * the fixed warm-restore cost) + t_wait_us (RX armed -> beacon hit, the
 * alignment error that P3.2 tracking shrinks). All MTIME-based (1 MHz), both
 * endpoints inside one wake. The ring lives in the retained-OCRAM blob slot
 * (uncached 0x21 alias), survives PDS cycles AND the wake-armed WDT reset;
 * the blob (re)initializes it whenever the magic is missing (e.g. after POR).
 */
typedef struct {
    uint32_t seq;           /* blob cycle number (ring->wr at write time) */
    uint32_t rtc_us_lo;     /* RTC time at hit/timeout, us (low 32 bits; wraps
                             * @ ~71 min — offline diffs must be unsigned and
                             * between ADJACENT records only) */
    uint32_t tsf_us_lo;     /* beacon TSF us (low 32, same ~71 min wrap); 0 on loss */
    uint32_t t_ready_us;    /* entry -> RX armed */
    uint32_t t_wait_us;     /* RX armed -> beacon hit (or full window on loss) */
    /* Field semantics split by record kind (2026-07-21, beacon-track debug):
     * BEACON records carry tracking telemetry — delay_us = beacon-grid delay
     * excess (last_beacon_delay_us), err_ppm = live rtc32k_error_ppm,
     * loss_level = adcpow[7:0] RF-margin trend. LOSS records keep the cont21
     * ghost-hunt RF/AGC snapshot — delay_us = rc_static 0x24C0C000, err_ppm =
     * RC2_ST1 0x24C0C904, loss_level = P3.3 graded level. trim_code carries
     * the poll window (ms, clamped 255) on BOTH kinds — the per-cycle view of
     * the adaptive shrink. */
    int32_t  delay_us;
    uint16_t flags;         /* LP_BCNREC_* */
    uint8_t  loss_level;
    uint8_t  trim_code;     /* poll window in ms this cycle (255 = clamped) */
    int32_t  err_ppm;
} lp_bcn_record_t;          /* 32 bytes */

#define LP_BCNREC_BEACON    0x0001u  /* beacon received this cycle */
#define LP_BCNREC_TIM_HIT   0x0002u  /* TIM had our AID -> WIFI wake */
#define LP_BCNREC_LOSS      0x0004u  /* poll window ended with no beacon */
#define LP_BCNREC_BUDGET    0x0008u  /* DTIM budget exhausted -> timed wake */
#define LP_BCNREC_RC2_LIVE  0x0010u  /* RC2_ST1 changed across the poll window
                                      * (clear on a full-window loss = AGC FSM
                                      * frozen, the cont3e deaf-RX signature) */
#define LP_BCNREC_DTIM      0x0020u  /* TIM parsed with DTIM count == 0: the
                                      * caught beacon was a DTIM beacon */
#define LP_BCNREC_PARSEFAIL 0x0080u  /* beacon received but lpfw_find_tim
                                      * returned NULL (walk exhausted, not a
                                      * genuine TIM-less beacon: every real AP
                                      * beacons a TIM). The offline sim
                                      * (scratchpad findtim_sim.c, 2026-08-18)
                                      * shows this is the ONLY failure shape a
                                      * rate-seed mismatch can produce -- the
                                      * verdict is never built from unlanded
                                      * bytes -- so the record flag is the
                                      * bench's parse-health witness for the
                                      * 250 us poll-step retry. */
#define LP_BCNREC_CONVERGED 0x0040u  /* this cycle ran with the SHRUNK poll
                                      * window (lp_fw_track_shrink_ok) — also
                                      * disambiguates trim_code==255 (a clamped
                                      * big lead vs the 2s unconverged window) */
#define LP_BCNREC_RX_HEAD    0x0100u  /* loss exit found a non-zero 802.11 FC
                                      * byte in the cleared RX buffer */
#define LP_BCNREC_RX_BEACON  0x0200u  /* that late frame is a beacon */
#define LP_BCNREC_RX_AP      0x0400u  /* that late beacon carries our AP BSSID:
                                      * it landed after the final poll check */
#define LP_BCNREC_REJ_ID     0x0800u  /* catch: slot identity re-check failed after
                                       * the TSF read (overwritten before it): TSF
                                       * not trusted, anchor untouched */
#define LP_BCNREC_NOANCH     0x1000u  /* catch: TSF trusted but track_update rejected
                                       * the pair (C1 phase/band/chain, bound, stale) */
#define LP_BCNREC_FOREIGN_ABORT 0x2000u /* loss: window cut short after a foreign frame
                                          * landed past the predicted TBTT (v3 policy) */

/* Classify bytes left in the RX buffer when the poll loop declares a loss.
 * lpfw_beacon_status_clear() zeroes FC and BSSID before MAC RX is armed, so a
 * non-zero FC is a per-wake delivery witness, not stale retained RAM. Keep the
 * implications nested: an AP match is meaningful only for a beacon, and a
 * beacon is meaningful only after the MAC delivered a frame header. */
static inline uint16_t
lp_fw_rx_tail_flags(uint8_t subtype, const volatile uint8_t *seen_bssid,
                    const volatile uint8_t *target_bssid)
{
    if (subtype == 0u) {
        return 0u;
    }

    uint16_t flags = LP_BCNREC_RX_HEAD;
    if (subtype != 0x80u) {
        return flags;
    }

    flags |= LP_BCNREC_RX_BEACON;
    for (uint32_t i = 0; i < 6u; i++) {
        if (seen_bssid[i] != target_bssid[i]) {
            return flags;
        }
    }
    return flags | LP_BCNREC_RX_AP;
}

typedef struct {
    uint32_t magic;         /* LP_BCN_RING_MAGIC when initialized */
    uint32_t wr;            /* total records written; slot = wr % depth */
    uint32_t depth;         /* record capacity (LP_BCN_RING_DEPTH) */
    uint32_t recv_cnt;      /* KPI-1: cycles with a beacon received */
    uint32_t loss_cnt;      /* KPI-1: cycles that timed out beaconless */
    uint32_t idle_sum_us;   /* KPI-2: sum(t_ready+t_wait); wraps @ ~71 min idle */
    uint32_t idle_max_us;
    uint32_t tready_sum_us; /* fixed bring-up cost, separated */
    uint32_t hist[4];       /* t_wait buckets: <2ms / 2-10ms / 10-50ms / >=50ms */
    uint32_t rsvd[4];
    lp_bcn_record_t rec[];  /* LP_BCN_RING_DEPTH records */
} lp_bcn_ring_t;            /* 64-byte header */

#define LP_BCN_RING_MAGIC   0x4E434252u  /* "RBCN" */
/* Fixed home: top of the reserved blob OCRAM slot, below the 0x2104FF00 debug
 * page. Uncached 0x21 alias of 0x1104F800 (blob link must stay under 0x9800).
 * MUST end at 0x2104FE00: the waitreg table (LP_FW_WAITREG_ADDR) owns the
 * 128 B from there to the bootrom's brom_param region at 0x1104FE80 (ROM ld
 * reserves 0x100 B; the BPRM+FCFG+SPI_Flash_Cfg_Type push actually writes
 * ~0x74 B on every full boot — see ipro6_demo/main.c BROM_PARAM_NC_ADDR).
 * Also note the ring body overlaps the ROM's eflash_loader_logbuf
 * (0x1104F590-FD90, written during download flows only) — harmless because
 * both the ring and the loader die across any full boot anyway. */
#define LP_BCN_RING_ADDR    0x2104F800u
/* 0x600 -> 0x5A0 (depth 46 -> 42): the freed tail 0x2104FDA0-FDFF hosts the
 * wake-witness snapshot slots (wlp1/speed-witness/CFG0 x6/CFG8). Their first
 * home (0x2104FFD0-FFF4) trampled CONTRACT-OWNED words -- the trap handler's
 * mcause/mepc dump at 0x2104FFE0/E4 and the WDT opt-out flag at 0x2104FFF4 --
 * per-wake, which is exactly the class of scribble the 08-13 shape-shifting
 * wake deaths pointed at. The ring self-heals across the size change via its
 * magic+depth check.
 * 0x5A0 -> 0x580 (one more row, depth 43 -> 42): 0x2104FD80-FD9F freed for
 * the post-TIM tap-gate knob block (LP_FW_MACGATE_KNOB_ADDR) -- the FDA0-FDFF
 * witness page is full.
 * 0x580 -> 0x560 (depth 42 -> 41, 2026-08-19): 0x2104FD60-FD7F freed to break
 * an ADDRESS COLLISION. hal_lp.c's flash-recovery witness had been writing
 * 0x2104FDE4 since the 08-17 resume-death hunt, and 0x2104FDE4 was later
 * handed to LP_FW_XTAL_SEQRDY_KNOB -- so the first full wake of every session
 * overwrote the knob with (timeout<<24)|jdec and the whole SEQRDY shape
 * silently reverted to legacy mid-session. Caught 2026-08-19 by reading the
 * word post-leg: 0x011640EF, the witness format exactly. The witness moved
 * here; the knob keeps its documented address so bench procedure is unchanged.
 * 0x540 -> 0x500 (depth 40 -> 38, 2026-08-21 pm): 0x2104FD00-FD3F freed for
 * the crystal-wait FLOOR investigation, which needs a knob+witness pair per
 * suspect and will need more. Depth 38 still wraps inside a 150 s DTIM-10
 * window (~146 wakes), so the ring loses nothing that was being read.
 * 0x560 -> 0x540 (depth 41 -> 40, 2026-08-21): 0x2104FD40-FD5F freed because
 * the FD60-FDFF page ran out -- a whole-tree scan of 0x2104FDxx came back with
 * zero free words. Hosts LP_FW_FASTXTAL_KNOB (below). Depth is immaterial at
 * DTIM-10: a 150 s window is ~146 wakes, so the ring wraps either way.
 * COST OF THE BUG: the 08-18 "SEQRDY loses its A/B/A" verdict measured a
 * mongrel -- SEQRDY for the cycles before the first wifi wake, legacy after,
 * and in between a carrier/stub mismatch (PDS armed waitXtalRdy=1 while the
 * stub read a clobbered knob and did the legacy crystal setup). That verdict
 * is RETRACTED; the shape has never been measured cleanly. */
#define LP_BCN_RING_BYTES   0x500u
#define LP_BCN_RING_DEPTH   ((LP_BCN_RING_BYTES - 64u) / 32u)   /* 38 */

/* WFI-parked crystal wait, ALWAYS ON (A/B/A 2026-08-25: t1b 51.51 ->
 * 40.94 uJ/cycle, -10.3 uA). The ~1.86 ms crystal ramp is real hardware
 * start-up time; the park stops the core spinning on it at 14.6 mA.
 * Bounded by MAX_US so a ready bit that never asserts degrades to the
 * legacy behaviour instead of hanging; chunked at STEP_US so the ready
 * edge is noticed within one step. */
#define LP_FW_XTALWFI_STEP_US       50u
#define LP_FW_XTALWFI_MAX_US        4000u

/*
 * IPRO6 mask-ROM bootrom PDS-wake scratch (immutable silicon contract): the
 * bootrom's HBN-jump wake path (startup `la sp, __StackTopTemp` ->
 * ipro_bootrom_early_init / ipro_bootrom_hbn_jump_check) runs on a temporary
 * stack topping at 0x1104B440 (bootrom ld dtcm_data end) and pushes one
 * 8-byte frame {s0, ra=0x10FE0052} at 0x1104B438 on EVERY PDS wake — inside
 * the blob slot, trampling whatever the image keeps there. Layout lottery:
 * agcmem[] word hit -> AGC-program poisoning -> environment-dependent RX
 * deafness (the cont21 "margin x window" ghost); code hit -> illegal
 * instruction wedge. The 38K slot cannot avoid the address, so the carrier
 * stashes the staged bytes of a 64-byte guard window at load time
 * (lpfw_load) and the blob restores them at entry on every wake (startup.S).
 */
#define LPFW_BROM_SCRATCH_ADDR  0x2104B400u  /* guard window, uncached alias */
#define LPFW_BROM_SCRATCH_SIZE  64u
#define LPFW_BROM_STASH_ADDR    0x2104F7C0u  /* stash, just below the ring */

/*
 * Absolute HBN-RAM address of the iot2lp_para_t block (HBN_RAM_BASE + 0xF00).
 *
 * The full firmware reaches these same bytes through the SDK's
 * HAL_HBNRAM_IOT2LP_ADDR; the standalone lp_fw image (which cannot include the
 * SDK headers) uses this literal directly. A _Static_assert in hal_lp.c pins
 * this literal to HAL_HBNRAM_IOT2LP_ADDR, so moving the region in the HBN-RAM
 * map fails the build here instead of silently desyncing the two images.
 */
#define IOT2LP_PARA_ADDR    0x20090F00u
#define IOT2LP_PARA         ((volatile iot2lp_para_t *)IOT2LP_PARA_ADDR)

/*
 * --- P3.2 beacon tracking: sliding window + ppm-compensated TBTT resleep ---
 *
 * Runtime-gated by rx_lead_us (0 = legacy fixed resleep; `lp track <us>` CLI)
 * so one build serves both the TRACK=n baseline and the tracking A/B leg.
 *
 * WHAT rx_lead_us SHOULD BE: 5000. Swept on HW 2026-08-09 in one boot and one
 * association, 5000 re-measured between every point (DTIM10, LDO11 RT=6):
 *
 *     lead    4000     4500     5000            5500
 *     loss    13.67%   0.95%    0.19-0.49%      0.00%
 *     idle    12249    4725     3840-4592       4167    us   <- min at 5000
 *
 * idle = bcnstat KPI-2, mean(t_ready + t_wait) over every cycle. It is the
 * metric to use because it holds BOTH sides of the trade at once: a shorter
 * lead shrinks it, a lost beacon inflates it. It repeated to 4 us across
 * adjacent controls; the PPK2 clean-window mean cannot resolve this at all
 * (it drops windows containing the loss events, and read 46.05-49.42 uA on
 * five identical 5000 legs).
 *
 * It is a JITTER floor, not slack. t_wait quantises to the ~1001 us poll
 * iteration, so a 2005 record means the beacon truly arrived in (1004, 2005];
 * the median cycle has ~3 ms of margin but the worst of 200+ has ~1 ms, and
 * the lead has to cover the worst. The low tail is OURS: t_wait = rx_lead -
 * t_ready + AP_delay + (TBTT_actual - TBTT_predicted), and AP_delay is
 * positive-only, so it cannot push t_wait down. Splitting the records by their
 * own `dly` confirms it — every near-miss happened with dly < 200 us, i.e. the
 * AP on time. The jitter is RC32K: two 10 s windows ten seconds apart read
 * +43192 then +42998 ppm, and 194 ppm of wander is 199 us of prediction error
 * per 1.024 s period. So rx_lead_us is a dependent variable of the 32 kHz
 * source, not a tuning knob — see project_ipro6_rx_lead_floor_and_xtal32k for
 * the XTAL32K path (worth ~18 uA, currently blocked: the RTC counts fine on
 * the crystal but PDS never wakes, 0/5 against an RC32K control of 5/5).
 *
 * Prices, if this is ever re-opened: RX-on is 5250 uA, i.e. 5.13 uA per ms
 * over a 1024 ms period (from the wake waveform, NOT from the standby-vs-idle
 * regression, whose 2.55 uA/ms is diluted by that clean-window filter). One
 * miss costs 60 ms of failed poll plus ~15 ms of ladder retry, or ~175 ms if
 * the retry also misses, so 1% loss is 2-4 uA and 1 ms of lead pays for itself
 * only under +0.6-1.3% extra loss. 4500 spent 0.48% and gained nothing.
 *
 * The math lives here as pure static inlines because THREE parties consume
 * it: the blob (per-cycle resleep after each beacon / loss retry), the
 * carrier (aligning the FIRST interval of every hal_lp_enter — on the
 * tickless path most cycles ARE first cycles), and the host unit test
 * (includes this header verbatim; no copied-function drift).
 *
 * Timebase contract (measured 2026-07-13, DESIGN.md §13): the RC32K-driven
 * RTC runs ~+75000 ppm vs the AP's TSF on this board. All targets are
 * computed in the RTC timebase: TSF-side quantities (beacon interval) must
 * be scaled by rtc32k_error_ppm before being added to RTC stamps. The ppm
 * estimator below feeds that from consecutive beacon stamp pairs (the
 * software half of P3.4; hardware trim comes later).
 *
 * last_beacon_stamp_rtc_valid doubles as the anchor-health state:
 *   0 = no anchor; 1 = valid anchor; 2 = suspect (one inconsistent stamp
 *   pair seen — either that TSF read was corrupt, or the anchor is). A
 *   second consecutive inconsistent pair re-anchors on the current pair
 *   (a corrupted TSF read was observed live: 1/200 cycles, cont26).
 */
/* Minimum armed-wake interval: the tracking loop keeps skipping whole TBTTs
 * until the target is at least this far past `now + lead`, so a computed sleep
 * is NEVER shorter than this. It MUST clear pm_pds_mode_enter's fixed arm/XTAL-
 * settle latency (~2.5 ms, DESIGN §14 "effective lead ~= lead - 2.5ms") with
 * margin: at 3 ms a stale-anchor first-interval could arm a ~3 ms sleep that
 * races the PDS power-down and parks the core in PDS forever (HW-observed 1/422
 * tickless wedge, cont27/cont28; CCI LINK 0x30000100 all-1s, FFF8 armed 3135 us).
 * 20 ms lands on the next safe TBTT instead (keeps alignment — a re-align costs
 * one skipped beacon, never a wedge). */
/* Post-beacon settle before PDS re-entry, us. 0 = none, and 0 is the default:
 * the 4000 this replaces had no mechanism behind it.
 *
 * It arrived in the bring-up commit (d598bb3b4, 2026-07-09) as "+ 4ms
 * MAC-quiesce settle", against an RX degradation said to build up over cycles.
 * No measurement was ever recorded behind the number.
 *
 * A mechanism for that IS possible -- do not assume otherwise. PDS gates the
 * MAC/RF LOGIC off, but WRAM CONTENT SURVIVES it (canary across the full 160 KB
 * RAM_WIFI span, 4 of 4 intact through a PDS cycle, 2026-08-08; see the WRAM
 * retention note in hal_pm.c). The RX-DMA ring lives in WRAM, so a burst cut
 * mid-write can in principle leave a torn descriptor that outlives the
 * power-down.
 *
 * It is removed because it does not HAPPEN, not because it cannot: 9026
 * consecutive DTIM10 cycles at quiesce=0 over 3 h gave 0.62% loss with no
 * upward trend (first 3 buckets 0.40% vs last 3 0.47%), and the one excursion
 * to 3.19% healed to 0.20% in the next bucket -- reversible, so environmental,
 * since accumulated damage does not repair itself.
 *
 * Cost measured: DTIM10 steady state 89.4 -> 66.0 uA, i.e. 23.4 uA +/- 1.8
 * (n=18 vs 14, alternating legs, window = 5 wake periods so the estimator has
 * one mode). That HALVES everything above the 44 uA PDS floor; the implied
 * current across the removed window is ~6 mA, above the active-window average.
 *
 * Removing it does not move the wake instant: the tracker computes the next
 * sleep from an ABSOLUTE target with a fresh RTC read taken after this point,
 * so time not spent here becomes time spent asleep. (That self-correction needs
 * tracking ON; the untracked fallback and the loss ladder's back phase arm
 * fixed durations and would drift instead.) */
#define LP_FW_QUIESCE_DEFAULT_US   0u
/* iot2lp lives in HBN RAM, which is POR GARBAGE until something writes it, so
 * the blob must not spin on whatever happens to be in the field. Anything above
 * this is not a setting, it is garbage -- fall back to the default. */
#define LP_FW_QUIESCE_MAX_US       20000u

#define LP_FW_TRACK_MIN_GUARD_US   20000u   /* now -> armed-wake minimum (races PDS below ~3ms) */
#define LP_FW_TRACK_WAKE_LATE_MAX_US 40000u /* sane bound on the learned wake latency */
/* Cold-boot default for rx_lead_us (tracking RX-window lead). 1000 since
 * 2026-08-26 (ladder v3 + 30-min soak): with the LATEPT full-path learn ON
 * by default the tracker delivers t_wait ~ rx_lead + AP delay, and rung
 * 1000 measured loss 0.66% vs shipped-3000 controls 2.11-6.20% with the
 * ring all-green (700 = estimator stability edge, 500 = thrash; see
 * measurements/2026-08-26_lead_ladder_v3/). VALID ONLY with the LATEPT
 * default: without the full-path learn, 1000 sits under the ~2.5 ms wake
 * path and every wake lands late (the v1 ladder's 27.7%-loss cliff).
 * `lp track <us>` still overrides at runtime; `lp track 0` turns tracking
 * off. */
#define LP_FW_TRACK_RX_LEAD_DEFAULT_US 1000u

/* Measure the wake latency directly: the tracker stamps the absolute RTC-us it
 * MEANT to wake at (armed_wake_rtc_us), the blob reads the RTC on its very
 * first instruction, and the difference IS the latency -- arm-side skew, the
 * us->tick conversion, PDS exit, XTAL settle, BROM and blob entry, all of it.
 *
 * Measuring here rather than from a beacon catch matters for three reasons:
 *   - both sides are RTC-domain, so the RC32K scale error (~4% on a board with
 *     no XTAL32K) cancels in the subtraction. The old catch-fed estimator
 *     differenced an RTC-domain lead against an MTIME-domain t_wait.
 *   - it samples every wake, including losses, so it converges in a couple of
 *     cycles and cannot be starved by a bad link.
 *   - a catch can only ever be sampled on hops the loss ladder already
 *     rescued, which biases the estimate towards the ladder's extra lead.
 *
 * Consume-and-clear: a zero stamp means the last sleep was NOT armed by the
 * tracker (cold boot, budget fence, back-phase skip), so there is nothing to
 * measure against and we must not learn from a stale target. */




/* LISTENGATE build option (default ON; blob-side compile switch -- the bare
 * lpfw Makefile cannot see Kconfig, so override with
 * `make EXTRA_CFLAGS=-DLP_FW_LISTENGATE=0` if a build ever needs it off).
 * The qualified 2026-08-30 listen/wake downgrades, formerly the runtime
 * WAKEGATE knob arm 0x6A7F077B (real-RX C/G/C + 30-min 1800-cycle soak
 * PASS; st7 listen 52.2 -> 41.2 mA):
 *  - CGEN0 BCLK gate for the whole wake (idle peripherals, -9.8 mA @ st3);
 *    restored on every WAKE exit in lpfw_core_clk_to_xclk(), skipped while
 *    rx_listen_park is armed (the gate blocks NEW CCI sessions)
 *  - CFG8 spare-tap thin at PLL-up + top-of-wait re-thin (-3.6 mA)
 *  - listen window only: vcore code 7 (0.95 V, -7.0 mA), lna_bm 6->4
 *    (-1.7), adc_ti off (-1.3), CRM CLKGATEPHYFCTRL0 b27 off (-1.0) --
 *    undone at EVERY wait exit before the clock speed-up (the vcore nibble
 *    is PDS-retained HBN_GLB; a missed undo runs the next wake's 240 MHz
 *    init undervolted)
 * vcore code 6 hung the part on record and must never be configured. */
#ifndef LP_FW_LISTENGATE
#define LP_FW_LISTENGATE            1
#endif
#define LP_FW_LISTENGATE_VCORE_CODE 7u           /* HBN_GLB[19:16], 0.95 V */
#define LP_FW_LISTENGATE_CGEN0_ADDR 0x30000580u
#define LP_FW_LISTENGATE_CGEN0_KEEP 0x00000063u  /* ocram|wram|cci|uart0 */
#define LP_FW_LISTENGATE_CGEN0_RST  0x07FFFFFFu  /* register reset value */
#define LP_FW_LISTENGATE_CFG8_THIN  0x00001F50u  /* div3/5/8/10/12/20/30 */
#define LP_FW_LISTENGATE_HBN_GLB    0x3008F030u
#define LP_FW_LISTENGATE_RF_LNA     0x30001078u
#define LP_FW_LISTENGATE_RF_ADDA2   0x30001094u
#define LP_FW_LISTENGATE_CRM_GATEF0 0x24940010u

/* DCDC18 rail register. The RAILGATE feature itself is full-FW code
 * (lp_tickless.c) behind Kconfig CONFIG_LP_RAILGATE / _DCDC18_SEL; only
 * the register address lives here so the map has one home. sel 21
 * (1.65 V) is the soak-qualified point; sel 15 (1.5 V) is REJECTED
 * (stochastic wake brown-out into the 1.2 mA zombie). */
#define LP_FW_RAILGATE_AON_DCDC    0x3008F814u

/* DCDCLG (build option, default ON like LP_FW_LISTENGATE -- the blob has
 * no Kconfig; override with `make EXTRA_CFLAGS=-DLP_FW_DCDCLG=0`): deepen
 * the deep RF rail (DCDC18 -> 1.5 V AND LDO15RF bypass) for the t3 LISTEN
 * WINDOW ONLY. Applied at the tail of lpfw_listen_gate_apply() (vcore already
 * at the listen level, LDO15RF already powered) and restored FIRST in
 * lpfw_listen_gate_undo() + SETTLE_US on EVERY listen exit, beacon or not,
 * before the vcore raise, every clock speed-up, the t4 resleep and the t5
 * PDS entry -- so t2 init, PDS entry and all full-FW phases keep the standby
 * rail (stock 1.8 V, or the RAILGATE sel when that option is on; the live sel
 * is saved at apply, the two options compose). Apply lowers the DCDC then
 * bypasses; undo powers PU_LDO15RF OFF (voiding the bypass) then raises the
 * DCDC (tickless.c up-order). Was briefly widened to t2+t3 on 2026-09-05;
 * that hit a rare resleep hang (2026-09-06) so it is back to t3-only.
 * Qualified 2026-08-31/09-01: LISTEN_SEL 15 (1.5 V, the LDO15RF
 * zero-dropout boundary -- deeper sags the RF rail) drops the listen
 * level 38.15 -> 34.74 mA; trk1000 leg 111.2 uA / loss 0/430; a 1.425 V
 * variant passed a 1745-cycle soak (loss 0.56%) for margin. The
 * whole-standby AND t2+t3 forms of the same depth are REJECTED (PDS-entry
 * brown-out A/B 2026-09-05, t2+t3 resleep hang 2026-09-06); only t3 runs low. */
#ifndef LP_FW_DCDCLG
#define LP_FW_DCDCLG            1
#endif
#define LP_FW_DCDCLG_LISTEN_SEL 15u    /* [4:0], 25 mV/step: 15 = 1.5 V */
#define LP_FW_DCDCLG_SETTLE_US  100u   /* rail-raise settle before vcore/speed-up.
                                        * 150->100 qualified 2026-09-01 (A/B: 100 us
                                        * loss 0/430 == its bracket; 50 us had one
                                        * elevated-loss leg -- the 1.5->1.8 V slew
                                        * needs ~75 us before the vcore raise is
                                        * safe, 50 rides the line). ~1.7 uA saved. */

/* LDO15RF listen-only gating (production default, 2026-09-01). PU_LDO15RF
 * (AON rf_top_aon bit1) is AON-retained; HW gates the rail across PDS but
 * the LDO burned through t1 (2 ms crystal ramp) and t2 (RF warm restore)
 * on every wake. Measured: the 40 MHz crystal does NOT ride this LDO
 * (shell poke survives pu=0), and same-image A/B proved RX healthy with
 * the LDO off through ALL of t1+t2, powered only at listen entry (t3-only
 * loss 0.7% vs brackets 4.9/13.2%). Flow: full FW clears pu at PDS entry
 * (hal_lp_enter -- RF quiesced by then); the listen gate apply sets it
 * (the >=751 us pre-beacon wait absorbs the 90 us LDO settle); it is
 * cleared again the moment lpfw_find_tim returns (TIM/TSF bytes
 * DMA-landed -- clearing at the earlier beacon-detect would cut the
 * still-in-flight TIM reception) and at the loss resleep tail, so every
 * path into a blob wake already carries pu=0 through t1+t2. Every
 * full-FW wake gets RF back from rwnxl_phy_reinit() (assoc resume) and
 * the hal_lp wake funnel (disassociated) -- the blob never restores pu. */
#define LP_FW_RF_TOP_AON        0x3008F880u
#define LP_FW_RF_PU_LDO15RF     (1u << 1)
#define LP_FW_RF_LDO15RF_BYPASS (1u << 27)  /* rf_top_aon: feed RF directly from DCDC (1.5 V) */
/* LDOEARLY (knob, default off): power PU_LDO15RF at t2 entry (past PLL
 * bring-up, before the RF warm restore) instead of at listen arm, so the
 * LDO ramp + RF settle overlap t2. Bench 2026-09-02: at rx_lead 500 the
 * converged window loses on-time beacons and catches only AP-delayed ones
 * (dly >= ~300 us) -- the listen-arm power-up eats the lead. Same-image A/B
 * knob for the lead-floor proof; costs the LDO for ~t2 (~0.4 ms). */
#ifndef LP_FW_LDOEARLY_KNOB_ADDR
#define LP_FW_LDOEARLY_KNOB_ADDR 0x2104FDCCu  /* retained scratch, magic-gated */
#endif
#define LP_FW_LDOEARLY_MAGIC     0x7E0E0001u

/* NDTIM phase (knob, default off): track the beacon AFTER each DTIM beacon
 * instead of the DTIM beacon itself. Every tracked beacon today is a DTIM
 * beacon, and the AP transmits its buffered broadcast/multicast right
 * behind it; converged-window losses carry RX_HEAD and the 08-28 slot-1
 * witness held a broadcast QoS-data frame -- the single RX slot is being
 * overwritten by the DTIM-following multicast before the poll sees the
 * beacon. Unicast TIM bits are in every beacon, so a TIM-only standby loses
 * nothing by listening one beacon later. Same-image A/B for the mechanism. */
#ifndef LP_FW_NDTIM_KNOB_ADDR
#define LP_FW_NDTIM_KNOB_ADDR    0x2104FDDCu  /* retained scratch, magic-gated */
#endif
#define LP_FW_NDTIM_MAGIC        0x7D710001u

/* BCNONLY (knob, default off): LP MAC RX filter = ACCEPT_BEACON only (drop
 * ACCEPT_BROADCAST) -- rwnx_lpfw_init_ex(). Selected before mm_active(). */
#ifndef LP_FW_BCNONLY_KNOB_ADDR
#define LP_FW_BCNONLY_KNOB_ADDR  0x2104FDE0u  /* retained scratch, magic-gated */
#endif
#define LP_FW_BCNONLY_MAGIC      0x7B0C0001u

/* Loss-FC witness (diagnostic, cleared by the bench per leg): what frame
 * header sits in the single RX slot when a converged/wide window closes
 * without our beacon. FDE8 = histogram [7:0] mgmt (non-beacon) / [15:8]
 * ctrl / [23:16] data / [31:24] beacon (other BSSID); FDEC = last loss
 * (fc0 << 24) | (fc1 << 16) | loss count. */
#define LP_FW_LOSSFC_HIST_ADDR   0x2104FDE8u
#define LP_FW_LOSSFC_LAST_ADDR   0x2104FDECu

/* LPRSSI (knob, default off): LP AGC RSSI target handed to wl_lp_config()
 * by `lp populate` (which otherwise hardwires 0 -> the "strong SOI" profile
 * with a -50 dBm energy gate: energy detect ON, signal detect OFF, so in
 * busy air the AGC fires on every frame above -50 dBm). [31:16] magic,
 * [7:0] signed dBm target, e.g. 0x7A5500EA = -22 dBm -> gate at -32. */
#ifndef LP_FW_LPRSSI_KNOB_ADDR
#define LP_FW_LPRSSI_KNOB_ADDR   0x2104FD24u
#endif
#define LP_FW_LPRSSI_MAGIC       0x7A55u
/* Same knob word, magic 0x7A56 = SKIP wl_lp_config() entirely -- the
 * reference SDK LP FW never calls it (its RX profile is whatever wl_lp_init's
 * warm restore leaves); the IPRO port added the call with target 0. */
#define LP_FW_LPRSSI_SKIP_MAGIC  0x7A56u
/* LPRSSI variant 0x7A58 (2026-09-03): same [7:0] target, and the weak-SOI
 * branch of wl_lp_config() (signal detect, DSSS-only) keeps the strong
 * branch's LNA cap (idx max 4) -- for a -20 dBm AP that wants signal detect
 * without the extra front-end gain. Carried to wl_lp_config as cfg bit 30. */
#define LP_FW_LPRSSI_LNA4_MAGIC  0x7A58u
#define LP_FW_LPAGC_CFG_LNA4     (1u << 30)

/* POLLX (knob, default off): converged-window right pad in ms instead of
 * LP_FW_TRACK_POLL_EXTRA_MS (15). [31:16] magic, [7:0] ms. Tests whether the
 * busy-air losses are AP beacon deferrals beyond 15 ms (the reference SDK
 * sizes this window adaptively from the observed delays). */
#ifndef LP_FW_POLLX_KNOB_ADDR
#define LP_FW_POLLX_KNOB_ADDR    0x2104FD28u
#endif
#define LP_FW_POLLX_MAGIC        0x7A57u

/* RXB2 (knob, default off): force MAC RX buffer 2 DISABLED in the LP rxl
 * init. The LP layout gives buffer 2 a 4-byte stub (rxl_hwdesc.c
 * rxl_hwdesc_env_para) yet passes the HW's current disable bit through --
 * after the PDS MAC reset that is 0 = enabled, so a BA / trigger frame is
 * steered into a 4-byte buffer. Air capture 2026-09-02 18:1x: lost beacons
 * were on air, on time, clean FCS, quiet air before them, and the DUT's slot
 * was empty (0x0044) on alternate wakes. */
#define LP_FW_RXB2_KNOB_ADDR     0x2104FD2Cu
#define LP_FW_RXB2_MAGIC         0x7A580001u

/* LGOFF (knob, default off): skip the LISTENGATE listen downgrades (vcore,
 * LNA bm, ADC TI, CRM PHY-F gate, DCDCLG) but keep the LDO15RF power-on.
 * 2026-09-02: every busy-air LOSS record lacks RC2_LIVE (AGC status frozen
 * across the whole window) while the 08-28 losses had it; LISTENGATE
 * became unconditional on 08-30. A/B for listen-downgrade deafness. */
#define LP_FW_LGOFF_KNOB_ADDR    0x2104FD30u
#define LP_FW_LGOFF_MAGIC        0x7A590001u

/* TRKP (knob, default off): the ppm feedback-trap fix (task #12). */
#ifndef LP_FW_TRKP_KNOB_ADDR
#define LP_FW_TRKP_KNOB_ADDR     0x2104FD34u
#endif
#define LP_FW_TRKP_MAGIC         0x7A5A0001u
#ifndef LP_FW_HBN_GLB_RD
#define LP_FW_HBN_GLB_RD()       (*(volatile uint32_t *)LP_FW_HBN_GLB_ADDR)
#endif
#define LP_FW_F32K_SEL_POS       3u        /* HBN_GLB[4:3]: 0 RC32K, 1 XTAL32K */
#define LP_FW_F32K_SEL_XTAL      1u

/* TRKW (knob, default off): bounded poll windows + stale-anchor re-anchor
 * (task #13, GOAL lever 1). 0x2104FD3C == 0x7A5C0001. */
#ifndef LP_FW_TRKW_KNOB_ADDR
#define LP_FW_TRKW_KNOB_ADDR     0x2104FD3Cu
#endif
#define LP_FW_TRKW_MAGIC         0x7A5C0001u
#define LP_FW_TRKW_OFF           0x7A5C0000u  /* bench override: keep the legacy tracker */
#define LP_FW_TRKW_EXTRA_MS      1u        /* poll margin past the expected beacon (8 -> 3 -> 1,
                                            * 2026-09-06: see the loss policy note -- every ms of
                                            * window costs ~5 uA at DTIM10 and buys no power back) */
#define LP_FW_TRKW_WIDE_MS       110u      /* no-prediction window: one beacon interval
                                            * + AP jitter. Was 2000 (82 mC per event, 20
                                            * beacons' worth); a 102.4 ms interval already
                                            * puts a beacon inside a 110 ms window, and a
                                            * miss there just costs another 110 ms. */
#define LP_FW_TRKW_UNTRACKED     0xFFFFu   /* trk_armed_extra_us: not a tracked hop */
#define LP_FW_TRKW_MARGIN_CAP_US 1000u     /* adaptive margin on top of EXTRA_MS (12 -> 4 -> 1 ms) */

/* CCIMUX (knob, default off; bench diagnostics): re-arm the S_CCI pad mux on
 * GPIO10-13 at every RX arm so a host-side non-halting CCI session survives
 * PDS cycling (the mux is lost across every PDS wake). Pure GLB pad-config
 * writes, the same four the carrier issues with `lp mw`. 0x2104FD74 =
 * 0xCC1A0001 (the 2026-08-30 park tools' address/magic). */
#ifndef LP_FW_CCIMUX_KNOB_ADDR
#define LP_FW_CCIMUX_KNOB_ADDR   0x2104FD74u
#endif
#define LP_FW_CCIMUX_MAGIC       0xCC1A0001u
#define LP_FW_CCIMUX_PAD0        0x300008ECu   /* GPIO10..13 cfg, 4 words */
#define LP_FW_CCIMUX_S_CCI       0x00400F03u

/* RXPARK (knob, default off; bench diagnostics): park in the beacon poll
 * loop forever with RX armed -- the poll never expires and a detected
 * beacon does not end it, so every accepted frame keeps landing in slot 1
 * while a host-side CCI session samples the slot and the MAC/RIU state.
 * Replaces the removed `lp stage 7` listen park. Escape = relay reset.
 * 0x2104FD70 = 0x7A5E0001. */
#ifndef LP_FW_RXPARK_KNOB_ADDR
#define LP_FW_RXPARK_KNOB_ADDR   0x2104FD70u
#endif
#define LP_FW_RXPARK_MAGIC       0x7A5E0001u

/* RXREARM (knob, default off; GOAL lever 2 remedy candidate): the LP MAC
 * delivers ONE accepted frame per RX arm and never refills its single
 * descriptor (listen park 2026-09-03 05:38: our beacon landed once, then
 * 33,707 CCI samples over 40 s saw nothing while ~10 beacons/s were on air).
 * With this knob the poll loop re-arms the RX buffer (rxl_hwdesc_init) and
 * clears the slot whenever the slot holds a completed frame that is not our
 * beacon, so the next accepted frame is delivered. Knob word 0x2104FD6C =
 * 0x7A5F << 16 | re-arm count this wake (blob-written witness). */
#ifndef LP_FW_RXREARM_KNOB_ADDR
#define LP_FW_RXREARM_KNOB_ADDR  0x2104FD6Cu
#endif
#define LP_FW_RXREARM_MAGIC      0x7A5Fu

/* BCNONLY, done right (2026-09-03 06:4x): the beacon-only accept set is
 * written by rwnx_lpfw_init_ex() directly into RX_CNTRL while the MAC is
 * still IDLE. Its umac/lmac filter calls never narrowed the live register
 * because mm_rx_filter_set() ORs the RETAINED mm_env.rx_filter_host -- the
 * full firmware's 0x15078788 accept set (beacon|probe req/resp|other mgmt|
 * broadcast|my unicast|data|QoS data/null|BA|BAR|PS-poll) -- so every LP
 * wake so far listened with the full set and the 09-02 "beacon-only" A/B was
 * a null. With the single RX descriptor delivering ONE accepted frame per
 * arm, any accepted non-beacon of our BSSID that lands before the beacon is
 * a LOSS|RXH. TRAP (re-hit as build 19/20, 2026-09-03 06:27): a blob-side
 * RX_CNTRL write AFTER mm_active() reboots the part on the first PDS wake
 * (the 08-28 waitreg note said so already); the write must stay in IDLE.
 * Witness (written only under the knob): RX_CNTRL read back right after the
 * IDLE-state write. Free retained scratch word. */
#define LP_FW_RXCW_AFTER_ADDR    0x2104FD50u

/* DMAT (knob, default off, bench diagnostic 2026-09-03): does the RX DMA land
 * a frame's bytes progressively (find_tim's time-gated IE walk assumes so)
 * or at the end of reception? Measures, per beacon hit, the time from the
 * poll-loop detect to lpfw_find_tim() returning, and whether the beacon
 * interval field was already non-zero at the detect. Progressive landing
 * at 1 Mbps puts the TIM IE ~300-500 us after the FC; end-of-frame landing
 * makes find_tim return in a few us. Witness (OCRAM + MTIME only, no MAC
 * register access): FD54 = (min_us << 16) | max_us, FD58 = (hits << 16) |
 * (interval-nonzero-at-detect hits). */
#define LP_FW_DMAT_KNOB_ADDR     0x2104FD44u
#define LP_FW_DMAT_MAGIC         0x7A600001u
#define LP_FW_DMAT_MINMAX_ADDR   0x2104FD54u
#define LP_FW_DMAT_COUNT_ADDR    0x2104FD58u

/* LOSSNAP (2026-09-03 01:56, REMOVED): a blob-side read of NXMAC_DEBUG_HWSM_1
 * (0x24B00500) / RX_BUF_1_WR_PTR at the detect and at the loss decision
 * rebooted the part on its first PDS wakes (build 12, knob armed). MAC-side
 * state at the loss decision must come from non-halting CCI, not from the
 * blob (the earlier per-wake CCI diffs read these registers safely). */

/* defined with the C2 inlines below; the tracker drops a pending attribution on a stale re-anchor */
static inline void lp_fw_track_c2_reset_pending(void);
#define LP_FW_TRKP_XTAL_BOUND_PPM 500     /* XTAL32K: physical |ppm| bound */
#define LP_FW_TRKP_JITTER_US      200     /* stamp noise allowance per pair */



/* ppm-seed validity state (INTERNAL tracker state, not a knob):
 * rtc32k_error_ppm == 0 doubles as the "never estimated" sentinel, and 0 is
 * a reachable estimate -- re-seeding on it injects raw quantisation noise
 * (sd ~120 ppm). The seeded flag must survive PDS (blob .bss is zeroed
 * every wake), so it lives in retained OCRAM scratch: the tracker
 * self-arms bit0 on its first accepted sample and thereafter NEVER
 * re-seeds. Cold-boot noise without the magic = seed once, then arm. */
#ifndef LP_FW_TRKFIX_KNOB_ADDR
#define LP_FW_TRKFIX_KNOB_ADDR     0x2104FDC4u  /* retained scratch, magic-gated */
#endif
#define LP_FW_TRKFIX_MAGIC         0x7F1D0001u
#define LP_FW_TRKFIX_CHURN_GUARD_US 200 /* window sample "delayed" above offset+this */
#define LP_FW_TRKFIX_CHURN_PAD_US  250u /* extra lead per delayed window sample */

/* C1 re-acquisition hygiene (TRACKER_BRAINSTORM C1, bench 2026-09-02). The
 * lead-500 loss excess was post-loss re-anchor feedback, not the steady
 * prediction tail (lead-1000 error p50 -28 us, min -275 us / 246 cycles):
 * wide-window catches -- often slot-1-corrupted TSF -- went straight into
 * the anchor, and the 2-strike rule re-anchored on the SECOND garbage sample
 * (T1000B seq 419-422 -> 822 ms wait). Knob-gated for same-image A/B. */
#ifndef LP_FW_TRKC1_KNOB_ADDR
#define LP_FW_TRKC1_KNOB_ADDR      0x2104FDC8u  /* retained scratch, magic-gated */
#endif
#define LP_FW_TRKC1_MAGIC          0x7F1C0001u

/* Knob readers. TRKW is the tracker-v2 SUPERSET knob: it implies TRKP (ppm
 * bounds, clock identity, post-loss IIR feed, slot identity re-check) and
 * C1 (candidate chain, phase gate), because its bounded windows and its
 * stale-anchor rule are only safe on top of them (Codex TRKW review round 2,
 * NB1: "TRKW alone does not provide the behavior its comments promise").
 * Every bench leg that measured TRKW ran all three, so the superset IS the
 * measured configuration. TRKP and C1 stay independently armable. */
static inline int lp_fw_trkw_on(void)
{
    return *(volatile uint32_t *)LP_FW_TRKW_KNOB_ADDR == LP_FW_TRKW_MAGIC;
}
static inline int lp_fw_trkp_on(void)
{
    return *(volatile uint32_t *)LP_FW_TRKP_KNOB_ADDR == LP_FW_TRKP_MAGIC ||
           lp_fw_trkw_on();
}
static inline int lp_fw_trkc1_on(void)
{
    return *(volatile uint32_t *)LP_FW_TRKC1_KNOB_ADDR == LP_FW_TRKC1_MAGIC ||
           lp_fw_trkw_on();
}
#define LP_FW_TRKC1_PHASE_EARLY_US 1000  /* delay may sit this far below the 2nd-min baseline */
#define LP_FW_TRKC1_PHASE_MIN_WIN  4u    /* baseline trusted once this many window samples exist */
#define LP_FW_TRKC1_STRICT_PPM     2000  /* post-loss / candidate pair vs the estimate */
#define LP_FW_TRKC1_REANCHOR_N     3u    /* mutually consistent fresh samples before re-anchoring */

/* C2 adaptive lead (TRACKER_BRAINSTORM C2, owner setpoint 1% loss). The lead
 * has two jobs: cover the receiver's arm-to-live latency (ladder 2026-09-02:
 * ~450-500 us detect-time basis with LDO15RF powered at arm; lead 500 lost
 * 43%, 700 5.4%, 800 1.6%) and the prediction's early tail. C2 servos the
 * live lead from measurements instead of a constant: (1) a leaky minimum of
 * the converged-window detect time t_wait (= lead + e) is held at or above
 * floor + MARGIN -- a beacon seen close to the floor raises the lead at once
 * by the shortfall, before anything is lost; (2) AIMD on top: every
 * converged-window loss adds RISE, every clean cycle decays ~1 us, so the
 * loss equilibrium sits near RISE^-1 = 1%. Live state lives in retained
 * scratch (iot2lp is at its 256 B limit); knob word carries the floor. */
#ifndef LP_FW_TRKC2_KNOB_ADDR
#define LP_FW_TRKC2_KNOB_ADDR    0x2104FDD0u  /* [31:16] magic, [15:0] RX-live floor us */
#endif
#ifndef LP_FW_TRKC2_STATE_ADDR
#define LP_FW_TRKC2_STATE_ADDR   0x2104FDD4u  /* [15:0] live lead us, [31:16] leaky-min t_wait us */
#endif
#ifndef LP_FW_TRKC2_STREAK_ADDR
#define LP_FW_TRKC2_STREAK_ADDR  0x2104FDD8u  /* [7:0] clean streak, [15:8] losses, [31:16] rises (witness) */
#endif
#define LP_FW_TRKC2_MAGIC        0x7C20u
#define LP_FW_TRKC2_MARGIN_US    100u   /* leaky-min detect time kept >= floor + this */
#define LP_FW_TRKC2_RISE_US      100u   /* (unused since the post-loss attribution) */
#define LP_FW_TRKC2_DECAY_N      16u    /* clean cycles per decay step */
#define LP_FW_TRKC2_DECAY_US     16u    /* = 1 us per clean cycle -> ~1% equilibrium vs RISE */
#define LP_FW_TRKC2_CREEP_US     2u     /* leaky-min forgets an early beacon at 2 us/cycle */
#define LP_FW_TRKC2_CEIL_US      4000u
#define LP_FW_TRKC2_RISE_MAX_US  500u   /* cap per attributed loss */
#define LP_FW_TRKC2_PENDING      0x80000000u /* streak word: loss awaiting its recovery catch */

#ifndef LP_FW_PPMSEED_STATE_ADDR
#define LP_FW_PPMSEED_STATE_ADDR   0x2104FD48u
#endif
#define LP_FW_PPMSEED_MAGIC        0x5EEDu


static inline void lp_fw_track_learn_wake(volatile iot2lp_para_t *para,
                                          uint32_t rtc_now_us)
{
    uint32_t armed = para->armed_wake_rtc_us;
    if (armed == 0u) { return; }                     /* no tracker-armed target */
    para->armed_wake_rtc_us = 0u;                    /* consume: one sample per arm */
    /* u32 wrap-safe: the stamp holds the low 32 bits of an RTC-us count that
     * rolls every ~71 min, and the latency is milliseconds. */
    uint32_t late = rtc_now_us - armed;
    if (late == 0u || late > LP_FW_TRACK_WAKE_LATE_MAX_US) {
        return;                                      /* early wake or garbage */
    }
    /* Credit the path that armed it -- the two cost very different amounts. */
    volatile int32_t *slot = para->armed_by_carrier ? &para->wake_late_carrier_us
                                                    : &para->wake_late_us;
    int32_t cur  = *slot;
    /* This number is not an average of anything -- it is subtracted from the
     * sleep target so RX is armed BEFORE the beacon, i.e. it must COVER the
     * wake path, not describe its centre. A symmetric IIR/4 mean is late on
     * roughly half the samples by construction, and every late arm is a miss:
     * the beacon is gone, the cycle re-acquires (~451 uC against a normal
     * cycle's ~83), and the next sleep is computed from a worse estimate. That
     * feedback is what put a floor under rx_lead and made lead 600 learn
     * 1933 us against a true ~2600 (LATEPT witness).
     *
     * So: rise fast, decay slow -- a cheap one-sided estimator that sits near
     * the high percentile instead of the mean. /2 up means a single outlier
     * moves it only halfway (a pure max would let one 39 ms garbage sample pin
     * the lead until reboot).
     *
     * DECAY IS /32. It was changed to /8 mid-investigation on a theory about
     * outliers inflating the lead, and BOTH the theory and the change were
     * wrong -- measured at lead 600 with landing over converged non-loss
     * cycles (t3gain.py), the two decays reclaim the same margin but differ on
     * loss: /32 gives loss/leg 1.5 -> 1.0 against its control, /8 gives
     * 1.0 -> 2.0. Same saving on paper, opposite sign on the thing that
     * actually costs 451 uC a time. /8 is reverted.
     *
     * Selected by LP_FW_LATEPT knob bit1 so it can be A/B'd against the mean
     * on one image -- no reflash between arms, which matters when flashing is
     * the flakiest step on this bench. */
    int32_t d = (int32_t)late - cur;
    int32_t next = cur + d / 4;                      /* IIR/4 mean */
    if (next < 0) { next = 0; }
    if (next > (int32_t)LP_FW_TRACK_WAKE_LATE_MAX_US) {
        next = (int32_t)LP_FW_TRACK_WAKE_LATE_MAX_US;
    }
    *slot = next;
}
#define LP_FW_TRACK_PPM_CLAMP      150000   /* |sane| RC32K error, ppm */
#define LP_FW_TRACK_EXTRA_CLAMP_US 20000    /* max per-beacon TX-delay excess */
#define LP_FW_TRACK_PPM_DELAY_GUARD_US 1000 /* AP contention is not clock drift */
#define LP_FW_TRACK_POLL_EXTRA_MS  15u      /* converged poll cap beyond lead */
/* CRM clock stage (PRODUCT path since 2026-08-18): the blob RMWs field
 * bits (mask 0x0F01: maccore freq_sel [11:8] + plfclksel [0]) into CRM
 * 0x24940008 after the WIFI AHB ungate, every wake. 0x0401: plfclksel=1
 * takes plf 120 -> 80 MHz for the wake it is staged in -- the 40 MHz
 * wait's legality partner (see LP_FW_CLK_WAIT_DEFAULT); freq_sel kept at
 * its stock 4 (maccore is hard-pinned at 60 MHz on this silicon). The
 * full-FW resume relatches plf 120 on its own. */
#define LP_FW_CRM_STAGE_DEFAULT      0x0401u
/* PDS 40M-crystal wake shape: hardcoded to the shipping legacy values
 * (waitXtalRdy=0, socEnbForceOn=1, xtalOff=1) in hal_pm.c. The SEQRDY
 * alternative (sequencer-held crystal ramp) was retired 2026-08-31: it
 * lost its A/B/A (loss 5.36% vs 0.00%) and the 2026-08-26 reprice found
 * no energy separation. Deliberate non-vendor pdsCfg fields kept
 * (documented at each): wifipllOff=0, cpuWfiMask=1, miscMemStby/
 * miscGateClk=1. */



/* Post-TIM WIFIPLL tap gate, ALWAYS ON (A/B/A 2026-08-25: -0.43 uJ/cycle,
 * loss clean): once the cycle's RX verdict is in, clear every WIFIPLL
 * output tap except DIV4 (bit5, the 240M trunk this CPU roots on) --
 * mask 0x1FD0 of CFG8. SLEEP tails only; never on a WAKE path (the
 * full-FW resume expects CFG8 whole), and no restore path (PDS resets
 * GLB and the next wake rewrites CFG8 from scratch). */
#define LP_FW_MACGATE_CFG8_MASK      0x00001FD0u

/*
 * Beacon-poll step. The blob checks the RX ring, then delays this long, so a
 * beacon is noticed up to one step AFTER it landed while the receiver burns
 * 5212 uA. The beacon's phase against the poll grid is effectively uniform, so
 * the mean waste is half a step -- 500 us here, ~2.5 uA over a 1024 ms DTIM10
 * period at the measured 5.09 uA/ms.
 *
 * HISTORICAL REJECTION: 250 us was tried on HW 2026-08-09, alternated A/B/A/B in one
 * boot and one association, and it is WORSE by 3.24 uA with zero overlap on
 * either metric:
 *
 *     step   idle_avg (us)         loss
 *     1000   3489 / 3597 / 3836    0.00% / 0.00% / 0.00%
 *      250   4161 / 4393           0.49% / 0.49%
 *
 * The tail cut is real -- t_wait's median fell ~490 us exactly as predicted --
 * and it is then more than paid back by beacons that stop being received at
 * all, and one miss costs 60 ms of failed poll.
 *
 * The reasoning that said this was risk-free was wrong in a specific way worth
 * keeping. It is true that a shorter step does not move the instant RX is
 * ARMED, and true that lpfw_find_tim's gate still gets frame_start <= iter_t0.
 * But that inequality only proves the gate does not start EARLIER than the
 * frame; it says nothing about whether the gate then waits long enough for the
 * TIM IE to finish DMA'ing. At 1000 us we almost always look back at a frame
 * that has fully landed, and THAT slack -- not the inequality -- is what makes
 * the parse safe. At 250 us we catch beacons mid-flight and the outcome rests
 * entirely on the gate's airtime model, which has a history: see the
 * "(num!=0, len==0)" note in lp_fw.c's poll loop.
 *
 * RE-EXAMINED 2026-08-18 (offline, scratchpad/clkmatrix_harness/findtim_sim):
 * the walk was transcribed verbatim into a progressive-DMA simulation and fed
 * the real NUC-AP beacon at every mid-flight detection offset x 4 stale-ring
 * patterns (incl. a shifted copy planting a fake TIM). With the rate seed
 * exact or slower than the air rate the fixed walk PASSES everything: verdict
 * always the true TIM, never influenced by unlanded bytes. A rate seed FASTER
 * than the air (under-wait) yields a clean NULL only -- flagged per record as
 * LP_BCNREC_PARSEFAIL, and TSF stays landed-safe either way. The 08-09 loss/
 * idle_avg signature is instead fully explained by the then-unfixed
 * "(num!=0, len==0)" infinite latch (mid-flight catch -> stale-pair spin until
 * the NEXT beacon's DMA overwrites it -> window expired = loss + ~100 ms
 * listen), so that rejection measured a bug that no longer exists.
 *
 * 250 us landed 2026-08-20 after the stale-pair fix. Loss-free t_wait medians,
 * 41-record rings, X-bracket on:
 *
 *   rx_lead / step   loss/41   t_wait p50
 *   4000 / 1000      0, 0        2145     <- the old default
 *   4000 /  250      1, 5        1755     -390 us, the step alone
 *   3500 / 1000      4           2144     lead alone buys nothing
 *   3000 / 1000      12          5206     lead alone falls over
 *   3500 /  250      0           1206
 *   3000 /  250      0, 2         651     -1494 us = 52.3 uA @3.3 V
 *   2500 /  250      12          9962     <- the cliff
 *
 * THE TWO ARE COUPLED, and that is the whole finding. Cutting rx_lead alone
 * does nothing to the median wait (2147 -> 2144) and only eats margin; the
 * 1 ms step hides sub-ms landing changes and averages 500 us of late
 * detection on top. Fine step + short lead together take t_wait from 2145 to
 * 651 us, which is BELOW the reference SDK's whole t3 budget (23.3 vs 32.0 uC).
 * Any future attempt to tune one of them without the other will re-derive the
 * 2026-08-09 "rx_lead is not tunable" conclusion and be wrong again.
 *
 * rx_lead itself stays a bench/integration parameter (`lp track`, shipping
 * default 0 = tracking off). Recommended pairing with this define: 3000.
 * NOTE 3000 sits only 500 us above the 2500 cliff and 3500 does NOT reach
 * vendor parity (110.8 vs 91.3 uA), so the margin question is real and is
 * what the soak has to answer.
 *
 * Must divide 1000 exactly: the window is specified in whole ms
 * (rx_lead/1000 + POLL_EXTRA_MS) and s_poll_window_ms keeps that unit so the
 * record's `win` field, its uint8 saturation and the loss ladder are unchanged.
 *
 * 100 us became the default on 2026-08-28. Two 180 s controls at 250 us
 * measured 148.692/149.010 uA; two interleaved 100 us legs measured
 * 143.030/137.466 uA. Loss was 1/357 controls versus 1/360 candidates. The
 * deterministic result is t_wait p50 943-948 -> 875-877 us and about 179 us
 * less thresholded active duration. The 8.603 uA leg-mean delta includes
 * air-tail covariance; use the timing reduction, not that full delta, as the
 * portable saving claim. Evidence: 2026-08-28_poll100_long_3v3.
 */
/* In-window root->XCLK bracket, ALWAYS ON (qualified 2026-08-20:
 * dtim_base A/B/A/B zero-overlap, 6 h soak, deterministic st7 park A/B/A
 * -9.6 mA on the CPU-side 240M trunk across the whole beacon wait):
 * HBN_GLB[1] cleared for the idle half of the beacon wait, restored at
 * BOTH wait exits before any parse/track/resleep math runs. Implemented
 * in lp_fw.c (lpfw_xbrk_apply/restore). */


/* Extra RX-window lead for sleeps armed on the CARRIER path (this file) rather
 * than by the blob's per-cycle resleep.
 *
 * para->wake_late_us cannot cover this: it is ONE IIR fed by whichever side
 * stamped armed_wake_rtc_us, and the two paths do not cost the same. The blob
 * arms from a trimmed stage-1 with the scheduler already frozen; the carrier
 * gets here after a full-firmware resume — WiFi suspend, the retention-stack
 * switch and the PDS entry itself all sit between the HBN_Get_RTC_Timer_Val
 * below and the actual power-down. Pooling both into one average leaves the
 * carrier's hops systematically late.
 *
 * Measured 2026-08-08 at DTIM10: with the fence bypassed, re-entry cycles came
 * in at t_wait 1-5 ms against the 2-3 ms of blob-armed cycles (i.e. ~2 ms
 * later), and 2 of ~8 overshot the beacon entirely and scored a loss. 5 ms puts
 * the carrier's hops back inside the window with margin and still leaves ~35 ms
 * of the 42 ms the fence used to waste. */
#define LP_CARRIER_EXTRA_LEAD_US  5000u

#define LP_FW_POLL_STEP_US          100u
#define LP_FW_TRACK_PPM_STABLE_BAND 5000    /* |inst-avg| within band = stable sample */
#define LP_FW_TRACK_PPM_STABLE_MIN  4u      /* stable samples before poll shrink */
#define LP_FW_TRACK_ACQUIRE_LEAD_US 2000u    /* transient margin until tracking is warm */
#define LP_FW_TRACK_DTIM_MAX        32u     /* sane ceiling for a TIM-supplied DTIM period AND
                                             * for the user-owned skip (lp dtim, up to the
                                             * fhost listen-interval cap of 20; 2026-09-06)
                                             * period (real APs use 1-10). An
                                             * unclamped ie_data[1] up to 255 would
                                             * stretch the loss-escalation ladder to
                                             * hours AND overflow the u32 back-phase
                                             * resleep multiply into a near-zero arm
                                             * (pm_pds_resleep(0) = NO wake source). */

/* Loss-policy constants (rationale: the v2/v3/v4 loss-policy notes further
 * down). The XTAL32K clock set carries these values verbatim; RC32K gets
 * its own in lp_fw_clk_params_rc32k. */
#define LP_FW_LOSS_RECONNECT_CAP    120u     /* ~2 min at DTIM10 (was 60): a deaf-receiver
                                              * storm runs 40-50 misses (SOAK_V5 21:01) and a
                                              * full reconnect costs ~165 mJ; the awake side's
                                              * 30 s keepalive already supervises a real link */
#define LP_FW_LOSS_REALIGN_EVERY    20u
#define LP_FW_LOSS_LATE_FROM        3u       /* misses that add no pad */
#define LP_FW_LOSS_LATE_STEP_US     1000u    /* late pad 1, 2, 3 ms then holds */
#define LP_FW_LOSS_LATE_CAP_US      1000u    /* (was 3000) RTC drift over a streak is ~50 ppm x 20 s = 1 ms */
#define LP_FW_LOSS_REALIGN_LATE_US  105000u  /* one beacon interval + 2.6 ms */

/* --- Per-sleep-clock DTIM parameters (2026-09-13) ---
 * The sleep clock is either the 32.768 kHz crystal (adopted at the first
 * arm: ~+/-50 ppm, ~1 ppm of wander) or the RC32K oscillator (free-runs at
 * +4..8 % untrimmed, ~200 ppm of short-term wander, 366 ppm Allan @ 1 s).
 * Everything the tracker sizes from clock quality lives here: one struct,
 * two initialisers, chosen by the live HBN_GLB F32K_SEL -- the identity
 * lp_fw_trkp_clk_xtal() already reads, so a runtime clock switch can never
 * leave a stale parameter set behind. The XTAL set IS the pre-2026-09-13
 * behavior; the RC set starts at the bench sweep's opening point. */
typedef struct {
    uint32_t rx_lead_us;      /* tracker lead the carrier applies at the first arm */
    uint32_t ppm_bound;       /* TRKP estimate bound and sample-bound base */
    uint8_t  poll_extra_ms;   /* legacy converged poll cap beyond the lead */
    uint8_t  wide_ms;         /* no-prediction poll window */
    uint8_t  ppm_stable_min;  /* TRKW: stable samples before a bounded window */
    uint8_t  auto_cal_en;     /* beacon-TSF RC32K trim, lp_fw_rc32k_trim_step() */
    /* post-loss policy (lp_loss_decide): a miss streak on RC32K is a phase
     * step the tracker cannot see (losses feed no sample), so the window
     * must open on BOTH sides quickly and re-align early; XTAL32K keeps the
     * v4 numbers (no early wake, 1 ms pad from the 4th miss, re-align at 20). */
    uint8_t  loss_late_from;      /* misses that add no pad */
    uint8_t  loss_realign_every;  /* every N misses the poll spans a beacon interval */
    uint8_t  ppm_iir_div;         /* rtc32k_error_ppm IIR divisor per accepted sample */
    uint16_t loss_late_step_us;   /* late pad per miss past loss_late_from */
    uint16_t loss_late_cap_us;
    uint16_t loss_early_step_us;  /* early wake per miss past loss_late_from */
    uint16_t loss_early_cap_us;
    uint32_t carrier_lead_us;     /* extra lead on full-FW re-entry hops (lp_demo.c populate);
                                   * `lp carrierlead <us>` overrides */
} lp_fw_clk_params_t;

/* Largest rtc32k_error_ppm change one accepted sample may make. */
#define LP_FW_TRACK_PPM_STEP_MAX     500

#define LP_FW_TRACK_RX_LEAD_RC32K_US 1000u   /* LAB10 sweep 2026-09-13: 1000/1500/2000/3000 all lose 6-12 %,
                                              * the extra listen only costs power -- same lead as XTAL */

static const lp_fw_clk_params_t lp_fw_clk_params_xtal = {
    .rx_lead_us     = LP_FW_TRACK_RX_LEAD_DEFAULT_US,
    .ppm_bound      = LP_FW_TRKP_XTAL_BOUND_PPM,
    .poll_extra_ms  = LP_FW_TRACK_POLL_EXTRA_MS,
    .wide_ms        = LP_FW_TRKW_WIDE_MS,
    .ppm_stable_min = 0u,
    .auto_cal_en    = 0u,
    .loss_late_from     = LP_FW_LOSS_LATE_FROM,
    .loss_realign_every = LP_FW_LOSS_REALIGN_EVERY,
    .ppm_iir_div        = 8u,
    .loss_late_step_us  = LP_FW_LOSS_LATE_STEP_US,
    .loss_late_cap_us   = LP_FW_LOSS_LATE_CAP_US,
    .loss_early_step_us = 0u,
    .loss_early_cap_us  = 0u,
    .carrier_lead_us    = LP_CARRIER_EXTRA_LEAD_US,
};
static const lp_fw_clk_params_t lp_fw_clk_params_rc32k = {
    .rx_lead_us     = LP_FW_TRACK_RX_LEAD_RC32K_US,
    .ppm_bound      = LP_FW_TRACK_PPM_CLAMP,
    .poll_extra_ms  = LP_FW_TRACK_POLL_EXTRA_MS,
    .wide_ms        = LP_FW_TRKW_WIDE_MS,
    .ppm_stable_min = LP_FW_TRACK_PPM_STABLE_MIN,  /* cold seed can miss by tens of ms */
    .auto_cal_en    = 1u,
    /* R6_1000 2026-09-13: 16-miss streaks of a ~2.5-4 ms late wake (wide
     * catches at +102.47 / +98.3 ms, acquire catch at +0.49 ms) with the
     * clock ratio steady at 138 ppm across them: widen +/-2 ms per miss to
     * +/-6 ms and re-align every 5, so a phase step costs ~3 misses, not 20. */
    .loss_late_from     = 0u,
    .loss_realign_every = 5u,
    /* RC32K drifts 18-48 ppm per 1.024 s hop for tens of hops; a 1/8 IIR
     * lags that ramp by ~8x the slope and the wake slides past the lead. */
    .ppm_iir_div        = 2u,
    .loss_late_step_us  = 2000u,
    .loss_late_cap_us   = 6000u,
    .loss_early_step_us = 2000u,
    .loss_early_cap_us  = 6000u,
    /* KA_short_rc 2026-09-13 ring: the re-entry hops catch at t_wait 7.5 ms
     * with the 5 ms pad -- the carrier hop is not where RC32K loses. */
    .carrier_lead_us    = LP_CARRIER_EXTRA_LEAD_US,
};

/* Sleep-clock identity straight from the hardware (HBN_GLB F32K_SEL): no
 * shadow flag to go stale behind an HBN_32K_Sel() caller. */
static inline int lp_fw_clk_is_xtal(void)
{
    return ((LP_FW_HBN_GLB_RD() >> LP_FW_F32K_SEL_POS) & 3u) == LP_FW_F32K_SEL_XTAL;
}
static inline const lp_fw_clk_params_t *lp_fw_clk_params(void)
{
    return lp_fw_clk_is_xtal() ? &lp_fw_clk_params_xtal : &lp_fw_clk_params_rc32k;
}

/* --- Beacon-TSF RC32K trim (2026-09-13) ---
 * The tracker's ppm estimate already absorbs the RC32K scale error, but a
 * trimmed oscillator keeps every cold seed, re-anchor and full-wake
 * re-entry near 0 ppm instead of tens of ms per hop. Once the estimate is
 * stable (LP_FW_TRACK_PPM_STABLE_MIN samples inside the band) and beyond
 * the threshold, step HBN_RC32K_CTRL0.CODE_FR_EXT by round(ppm / slope),
 * shift the estimate by the same amount so the prediction stays consistent,
 * and hold for LP_FW_RC32K_TRIM_HOLD catches. The slope is the one the boot
 * cal measured (para->rc32k_ppm_per_lsb), else the vendor nominal: IPRO6's
 * HBN_Recal_RC32K steps one LSB per 64 Hz = ~1950 ppm (the IPRO7 R-code's
 * 78 ppm/LSB does NOT apply -- a 78-based step drove this part to -527000
 * ppm on 2026-09-13). Hysteresis: the estimate must exceed 3/4 of a step
 * before the code moves (an error sitting on the +/- slope/2 rounding
 * boundary hunted 327 <-> 328 every hold period on 2026-09-13, R3_2000);
 * after a move the residual is inside +/- slope/4 and it takes a full LSB of
 * drift to move again. The tracker IIR carries the residual. The vendor blob
 * writes the same register from the same place (lpfw_recal_rc32k), so the
 * write is safe under the sleeping RTC. XTAL32K: auto_cal_en 0, never runs. */
#ifndef LP_FW_RC32K_CTRL0_ADDR
#define LP_FW_RC32K_CTRL0_ADDR       0x3008F200u  /* HBN_RC32K_CTRL0 */
#endif
#define LP_FW_RC32K_CODE_POS         22u
#define LP_FW_RC32K_CODE_MSK         (0x3FFu << LP_FW_RC32K_CODE_POS)
#define LP_FW_RC32K_HWCODE_POS       6u           /* HW-cal code while EXT_CODE_EN is clear */
#define LP_FW_RC32K_EXT_CODE_EN      (1u << 19)
#define LP_FW_RC32K_TRIM_THR_PPM     300
#define LP_FW_RC32K_PPM_PER_LSB_NOM  1950         /* vendor: 64 Hz per LSB at 32768 Hz */
#define LP_FW_RC32K_PPM_PER_LSB_MIN  50           /* sane measured-slope band (IPRO7 R-code is 78) */
#define LP_FW_RC32K_PPM_PER_LSB_MAX  20000
#define LP_FW_RC32K_TRIM_STEP_MAX    4
#define LP_FW_RC32K_TRIM_HOLD        8u

static inline int32_t lp_fw_rc32k_ppm_per_lsb(const volatile iot2lp_para_t *para)
{
    int32_t s = para->rc32k_ppm_per_lsb;
    return (s >= LP_FW_RC32K_PPM_PER_LSB_MIN && s <= LP_FW_RC32K_PPM_PER_LSB_MAX)
               ? s : LP_FW_RC32K_PPM_PER_LSB_NOM;
}

static inline uint32_t lp_fw_rc32k_code_rd(uint32_t ctrl)
{
    return (ctrl & LP_FW_RC32K_EXT_CODE_EN)
               ? (ctrl >> LP_FW_RC32K_CODE_POS) & 0x3FFu
               : (ctrl >> LP_FW_RC32K_HWCODE_POS) & 0x3FFu;
}

/* One call per accepted tracker sample. Returns 1 when the code moved. */
static inline int lp_fw_rc32k_trim_step(volatile iot2lp_para_t *para)
{
    if (!lp_fw_clk_params()->auto_cal_en || para->rc32k_auto_cal_en == 0u) {
        return 0;
    }
    if (para->rc32k_trim_hold != 0u) {
        para->rc32k_trim_hold--;
        return 0;
    }
    if (para->ppm_stable_cnt < LP_FW_TRACK_PPM_STABLE_MIN) {
        return 0;
    }
    int32_t est = para->rtc32k_error_ppm;
    if (est > -LP_FW_RC32K_TRIM_THR_PPM && est < LP_FW_RC32K_TRIM_THR_PPM) {
        return 0;
    }
    int32_t slope = lp_fw_rc32k_ppm_per_lsb(para);
    int32_t thr = (slope * 3) / 4;
    if (est > -thr && est < thr) {
        return 0;                            /* inside the hysteresis band: the IIR owns it */
    }
    int32_t half = slope / 2;
    int32_t step = (est + (est < 0 ? -half : half)) / slope;
    if (step > LP_FW_RC32K_TRIM_STEP_MAX)  step = LP_FW_RC32K_TRIM_STEP_MAX;
    if (step < -LP_FW_RC32K_TRIM_STEP_MAX) step = -LP_FW_RC32K_TRIM_STEP_MAX;
    volatile uint32_t *ctrl = (volatile uint32_t *)LP_FW_RC32K_CTRL0_ADDR;
    uint32_t v = *ctrl;
    int32_t old = (int32_t)lp_fw_rc32k_code_rd(v);
    int32_t code = old + step;               /* RTC fast (+ppm) -> higher code -> slower */
    if (code < 0)    code = 0;
    if (code > 1023) code = 1023;
    if (code == old) {
        para->rc32k_trim_hold = LP_FW_RC32K_TRIM_HOLD;   /* on the rail: stop asking */
        return 0;
    }
    *ctrl = (v & ~LP_FW_RC32K_CODE_MSK) | ((uint32_t)code << LP_FW_RC32K_CODE_POS) |
            LP_FW_RC32K_EXT_CODE_EN;
    para->rc32k_fr_ext      = code;
    para->rtc32k_error_ppm  = est - (code - old) * slope;
    para->rc32k_trim_hold   = LP_FW_RC32K_TRIM_HOLD;
    if (para->rc32k_trim_cnt < 255u) para->rc32k_trim_cnt++;
    return 1;
}

static inline uint32_t lp_fw_track_bcn_int_us(const volatile iot2lp_para_t *para)
{
    uint32_t tu = para->beacon_interval_tu ? para->beacon_interval_tu : 100u;
    return tu * 1024u;
}

static inline uint32_t lp_fw_track_interval_us(const volatile iot2lp_para_t *para)
{
    uint32_t n = para->dtim_num ? para->dtim_num : 1u;
    return lp_fw_track_bcn_int_us(para) * n;
}

/* TRKP: the physical |ppm| bound of the sleep clock over a given span.
 * XTAL32K cannot be more than a few hundred ppm off, but the RTC stamp is
 * the 100 us poll step that saw the beacon (+30.5 us tick), so a pair
 * carries ~+/-130 us of detection noise whatever its span: 1270 ppm over
 * one 102.4 ms interval, 127 ppm over a 1.024 s DTIM10 hop. The bound is
 * therefore the clock bound plus that jitter scaled by the span. Outside
 * it the pair is a corrupt stamp, not a measurement. RC32K keeps the wide
 * sanity clamp. Knob off = clamp. */
/* Knob off = never XTAL (legacy clamp); on = the hardware identity. */
static inline int lp_fw_trkp_clk_xtal(void)
{
    return lp_fw_trkp_on() && lp_fw_clk_is_xtal();
}

/* SAMPLE bound: what one (RTC,TSF) pair may read and still be a measurement
 * of this clock over this span. */
static inline uint32_t lp_fw_trkp_bound_ppm(const volatile iot2lp_para_t *para,
                                            uint64_t span_us)
{
    (void)para;
    if (!lp_fw_trkp_on()) {
        return (uint32_t)LP_FW_TRACK_PPM_CLAMP;      /* knob off = legacy clamp */
    }
    if (span_us < 1000ull) {
        span_us = 1000ull;
    }
    return lp_fw_clk_params()->ppm_bound +
           (uint32_t)(((uint64_t)LP_FW_TRKP_JITTER_US * 1000000ull) / span_us);
}

/* ESTIMATE bound: what the clock itself can be. Measurement noise never
 * becomes an estimate beyond it (seed and IIR results are clamped), and a
 * retained value beyond it is poison, not a reference. */
static inline uint32_t lp_fw_trkp_est_bound_ppm(void)
{
    return lp_fw_trkp_on() ? lp_fw_clk_params()->ppm_bound
                           : (uint32_t)LP_FW_TRACK_PPM_CLAMP;
}

static inline int32_t lp_fw_trkp_clamp_est(int32_t ppm)
{
    int32_t b = (int32_t)lp_fw_trkp_est_bound_ppm();
    return ppm > b ? b : (ppm < -b ? -b : ppm);
}

/* Scale a TSF-timebase duration into the (erroneous) RTC timebase. */
static inline uint64_t lp_fw_track_scale_rtc_us(const volatile iot2lp_para_t *para,
                                                uint64_t us)
{
    int32_t  ppm = para->rtc32k_error_ppm;
    uint32_t mag = (ppm < 0) ? 0u - (uint32_t)ppm : (uint32_t)ppm;   /* no -INT32_MIN */
    /* HBN RAM is POR garbage and iot2lp fields are only ever written
     * piecemeal — treat an implausible ppm as "no estimate yet". */
    if (mag > lp_fw_trkp_est_bound_ppm()) {
        return us;
    }
    /* unsigned-only 64-bit ops: the blob links __udivdi3/__umoddi3 but not
     * the signed variants. Worst input is the gap hop: 255*102400 us * 150000
     * ppm ~= 3.9e15, still far under UINT64_MAX. */
    uint64_t adj = (us * mag) / 1000000u;
    return (ppm < 0) ? (us - adj) : (us + adj);
}

/* Beacon-interval period expressed in the (erroneous) RTC timebase. */
static inline uint64_t lp_fw_track_period_rtc_us(const volatile iot2lp_para_t *para)
{
    return lp_fw_track_scale_rtc_us(para, lp_fw_track_interval_us(para));
}


/*
 * Fold one (RTC, TSF) beacon stamp pair into the tracking state: validate it
 * against the anchor, update the ppm IIR + delay sliding window + stamps.
 * Returns 1 when the pair was accepted (anchor advanced), 0 when rejected.
 */
static inline int lp_fw_track_update(volatile iot2lp_para_t *para,
                                     uint64_t rtc_us, uint64_t tsf_us)
{
    uint32_t bcn_int  = lp_fw_track_bcn_int_us(para);
    uint32_t win_size = para->bcn_delay_sliding_win_size;
    int32_t delay = (int32_t)(tsf_us % bcn_int);
    int32_t current_extra = delay - para->bcn_delay_offset;
    int ppm_sample_clean;

    if (current_extra < 0) {
        current_extra = 0;
    }
    /* RTC-vs-TSF deltas only measure the sleep clock when both beacons were
     * sent on the established phase.  A contention-delayed beacon moves the
     * peer TSF and our DMA-detection stamp by different amounts (HW: a
     * +3.7 ms TSF phase produced a -768 ppm pair against clean +345 ppm).
     * Feeding that into the IIR made the socEnbForceOn-clear shape miss every
     * fourth DTIM.  The delay window already names these samples, so exclude
     * them from the ppm estimator while still accepting them as anchors. */
    /* TRKP (knob): a post-loss catch spans k periods and is a BETTER clock
     * sample, not a worse one; excluding it left the IIR frozen on a
     * poisoned estimate for the rest of the session (every converged window
     * missed, only post-loss catches, none fed back -- HW 2026-09-02, ring
     * err_ppm constant -2279/+3007). The delay guard still excludes
     * contention-delayed beacons; the |ppm| bound below excludes corrupt
     * stamps. */
    int trkp = lp_fw_trkp_on();
    ppm_sample_clean = (trkp || para->continuous_loss_cnt == 0u) &&
        (para->bcn_delay_sliding_win_status < 4u ||
         (para->last_beacon_delay_us <= LP_FW_TRACK_PPM_DELAY_GUARD_US &&
          current_extra <= LP_FW_TRACK_PPM_DELAY_GUARD_US));

    if (win_size == 0u || win_size > LP_FW_BCN_DELAY_WIN_MAX) {
        win_size = LP_FW_BCN_DELAY_WIN_MAX;
        para->bcn_delay_sliding_win_size = win_size;
    }

    /* C1 (knob): three rules. (1) Once the delay window is warm a real
     * beacon's phase sits inside the learned baseline band; a slot-1 garbage
     * TSF has a random phase. (2) A post-loss catch must agree with the
     * anchor + ppm estimate within a STRICT band, not the 15% sanity clamp.
     * (3) A rejected sample never touches the anchor: it becomes a candidate
     * and only REANCHOR_N mutually consistent fresh samples re-anchor (a real
     * AP phase change), never a single pair. The anchor stays HEALTHY on a
     * reject so the next hop dead-reckons whole periods from it (the caller
     * drops the TIM-count first hop when the update was not accepted). */
    int c1 = lp_fw_trkc1_on();   /* C1 knob, or TRKW (superset) */
    int phase_ok = 1;
    if (c1 && para->bcn_delay_sliding_win_status >= LP_FW_TRKC1_PHASE_MIN_WIN) {
        phase_ok = (delay >= para->bcn_delay_offset - LP_FW_TRKC1_PHASE_EARLY_US &&
                    delay <= para->bcn_delay_offset + LP_FW_TRACK_EXTRA_CLAMP_US);
    }
    if (c1 && para->last_beacon_stamp_rtc_valid == 2u) {
        para->last_beacon_stamp_rtc_valid = 1u;   /* legacy suspect state never coasts on fallback */
    }

    if (para->last_beacon_stamp_rtc_valid != 0u) {
        uint64_t dt_tsf = tsf_us - para->last_beacon_stamp_beacon_us;
        uint64_t dt_rtc = rtc_us - para->last_beacon_stamp_rtc_us;
        int trkw = lp_fw_trkw_on();
        /* TRKW: an anchor older than the consistency window (by the LOCAL
         * RTC -- a peer TSF can go backwards and cannot declare staleness)
         * can neither judge a sample nor hop; it is dropped, but a single
         * unverified sample is not an anchor either: two mutually consistent
         * samples re-anchor (C1 chain at N=2). Without this C1 needed three,
         * which interleaved slot-1 garbage rarely allowed: 18 frozen cycles
         * on the bench, 2026-09-02. */
        int stale = trkw && dt_rtc > 12000000ull;
        int consistent = !stale &&
                         (dt_tsf >= 50000ull && dt_tsf <= 10000000ull &&
                          dt_rtc >= 50000ull && dt_rtc <= 12000000ull);
        uint32_t bound = lp_fw_trkp_bound_ppm(para, dt_tsf);
        int32_t inst = 0;
        if (consistent) {
            uint64_t num;
            int neg = (dt_rtc < dt_tsf);
            num = (neg ? (dt_tsf - dt_rtc) : (dt_rtc - dt_tsf)) * 1000000ull;
            uint32_t mag = (uint32_t)(num / dt_tsf);
            if (mag > bound) {
                consistent = 0;
            } else {
                inst = neg ? -(int32_t)mag : (int32_t)mag;
            }
        }
        /* TRKP: an estimate whose implied error over ONE hop exceeds the
         * bound is poison, not a reference: it is exactly what makes the
         * converged window miss. Judged against the hop span (a property of
         * the session, not of this sample) so "poisoned" cannot flip between
         * consecutive catches of different spans. Never compare against it;
         * re-seed from the next in-bound pair instead. */
        int32_t  est  = para->rtc32k_error_ppm;
        uint32_t emag = (est < 0) ? 0u - (uint32_t)est : (uint32_t)est;
        int est_ok = !trkp || emag <= lp_fw_trkp_est_bound_ppm();
        if (c1 && consistent && est_ok && para->continuous_loss_cnt != 0u &&
            para->ppm_stable_cnt >= LP_FW_TRACK_PPM_STABLE_MIN) {
            int64_t r = (int64_t)inst - (int64_t)est;   /* no int32 overflow on garbage est */
            if (r < 0) r = -r;
            if (r > LP_FW_TRKC1_STRICT_PPM) consistent = 0;
        }
        int reject = !consistent || (c1 && !phase_ok);
        if (c1 && !reject) {
            para->trk_cand_cnt = 0u;   /* accepted sample breaks any candidate chain */
        }
        if (reject && c1) {
            /* Candidate chain: does this reject agree with the previous one?
             * Same pair test as the anchor, on low words (deltas < 10 s). */
            uint32_t ct = (uint32_t)tsf_us, cr = (uint32_t)rtc_us;
            int chain = 0;
            if (para->trk_cand_cnt != 0u) {
                uint32_t dtt = ct - para->trk_cand_tsf_lo;
                uint32_t dtr = cr - para->trk_cand_rtc_lo;
                if (dtt >= 50000u && dtt <= 10000000u &&
                    dtr >= 50000u && dtr <= 12000000u) {
                    int cneg = (dtr < dtt);
                    uint64_t cnum = (uint64_t)(cneg ? dtt - dtr : dtr - dtt) * 1000000ull;
                    uint32_t cmag = (uint32_t)(cnum / dtt);
                    int64_t cr_ppm = (int64_t)(cneg ? -(int32_t)cmag : (int32_t)cmag) -
                                     (int64_t)para->rtc32k_error_ppm;
                    if (cr_ppm < 0) cr_ppm = -cr_ppm;
                    chain = cmag <= lp_fw_trkp_bound_ppm(para, dtt) &&
                            (para->ppm_stable_cnt < LP_FW_TRACK_PPM_STABLE_MIN ||
                             !est_ok || cr_ppm <= LP_FW_TRKC1_STRICT_PPM);
                }
            }
            para->trk_cand_cnt = chain ? (uint8_t)(para->trk_cand_cnt + 1u) : 1u;
            para->trk_cand_tsf_lo = ct;
            para->trk_cand_rtc_lo = cr;
            if (para->trk_cand_cnt < (stale ? 2u : LP_FW_TRKC1_REANCHOR_N)) {
                return 0;   /* anchor untouched and still healthy */
            }
            /* N fresh samples agree with each other but not with the anchor
             * (or the baseline): the retained state is what went stale. */
            para->trk_cand_cnt = 0u;
            if (!phase_ok || stale) {
                para->bcn_delay_sliding_win_status = 0u;
                para->bcn_delay_sliding_win_point = 0u;
            }
            para->ppm_stable_cnt = 0u;
            if (trkw) {
                lp_fw_track_c2_reset_pending();   /* new anchor epoch: old attribution void */
            }
        } else if (reject) {
            if (para->last_beacon_stamp_rtc_valid == 1u) {
                /* first strike: keep the old anchor, flag suspect */
                para->last_beacon_stamp_rtc_valid = 2u;
                return 0;
            }
            /* second strike: the anchor itself is bad — re-anchor here */
            para->bcn_delay_sliding_win_status = 0u;
            para->bcn_delay_sliding_win_point = 0u;
            para->ppm_stable_cnt = 0u;
        } else if (ppm_sample_clean) {
            /* Seed on the first measurement instead of averaging against a
             * zero that was never one. A single pair already gives the ratio
             * to ~500 ppm (measured sd of `inst`, 2026-08-08); the IIR needs
             * ~25 more catches to walk there, and on a board whose RC32K is
             * +42000 ppm off that walk IS the error -- HW-measured at DTIM10,
             * t_wait decayed 48 -> 3 ms over seq 0..25 while err_ppm ramped
             * 0 -> 42000, i.e. ~25 s of extra radio-on per anchor-from-cold.
             * Zero is the "never estimated" sentinel here (cold-boot hygiene
             * zeroes it, POR garbage is caught by the PPM_CLAMP gate), and on
             * a board that genuinely runs at 0 ppm seeding costs one sample's
             * jitter -- half a millisecond over a DTIM10 period.
             *
             * PPMSEED knob: the sentinel is also a REACHABLE estimate, and an
             * estimate that walks onto 0 gets re-seeded with raw noise (see
             * LP_FW_PPMSEED_STATE_ADDR). Armed, an explicit seeded bit in
             * retained scratch decides instead of the value. */
            volatile uint32_t *ss =
                (volatile uint32_t *)LP_FW_PPMSEED_STATE_ADDR;
            uint32_t sv = *ss;
            /* Self-arming (default since 2026-08-26, ladder v3
             * prerequisite): the word self-arms bit0 on the first accepted
             * sample, so the seeded bit -- not the reachable 0-ppm value --
             * decides re-seeding from then on. */
            if ((sv >> 16) == LP_FW_PPMSEED_MAGIC && (sv & 1u) != 0u && est_ok) {
                int32_t step = (inst - para->rtc32k_error_ppm) /
                               (int32_t)lp_fw_clk_params()->ppm_iir_div;
                if (step > LP_FW_TRACK_PPM_STEP_MAX) {
                    step = LP_FW_TRACK_PPM_STEP_MAX;
                } else if (step < -LP_FW_TRACK_PPM_STEP_MAX) {
                    step = -LP_FW_TRACK_PPM_STEP_MAX;
                }
                para->rtc32k_error_ppm += step;
            } else {
                para->rtc32k_error_ppm = inst;
                *ss = ((uint32_t)LP_FW_PPMSEED_MAGIC << 16) | 1u;
                if (trkp) {
                    para->ppm_stable_cnt = 0u;   /* a re-seed voids the old convergence */
                }
            }
            if (trkp) {
                /* a short-span sample is mostly poll jitter; the estimate
                 * it feeds stays a clock estimate */
                para->rtc32k_error_ppm = lp_fw_trkp_clamp_est(para->rtc32k_error_ppm);
            }
            /* Convergence gauge: count consecutive instantaneous estimates
             * that land inside the band around the IIR average. The poll
             * shrink waits for this — the 8-deep window fills in 8 beacons
             * but with a cold estimate on a +75000 ppm board the residual
             * is still tens of ms per DTIM10 period. */
            int64_t resid = (int64_t)inst - (int64_t)para->rtc32k_error_ppm;
            if (resid < 0) resid = -resid;
            if (resid <= LP_FW_TRACK_PPM_STABLE_BAND) {
                if (para->ppm_stable_cnt < 255u) para->ppm_stable_cnt++;
            } else {
                para->ppm_stable_cnt = 0u;
            }
            (void)lp_fw_rc32k_trim_step(para);
        }
    }

    /* accepted (or re-anchored): stamps become the new anchor */
    para->last_beacon_stamp_rtc_us    = rtc_us;
    para->last_beacon_stamp_beacon_us = tsf_us;
    para->last_beacon_stamp_rtc_valid = 1u;

    /* delay = this beacon's TSF phase past its nominal TBTT. Grid = the
     * BEACON interval, NOT the DTIM interval: every beacon (DTIM or not)
     * sits on the beacon-interval grid, so values cluster at the AP's TX
     * delay (0..few ms) regardless of which beacon the wake caught. Using
     * the DTIM interval here poisoned the window with k*bcn_int garbage
     * whenever a non-DTIM beacon was caught (dtim10: ~35% tracked loss,
     * anchor never converged — HW-measured 2026-07-21). */
    para->bcn_delay_sliding_win_buff[para->bcn_delay_sliding_win_point] = delay;
    para->bcn_delay_sliding_win_point =
        (para->bcn_delay_sliding_win_point + 1u) % win_size;
    if (para->bcn_delay_sliding_win_status < win_size) {
        para->bcn_delay_sliding_win_status++;
    }

    /* offset = 2nd-smallest of the window (tolerates one bad sample) once a
     * few samples exist; plain minimum before that. */
    {
        int32_t min1 = (int32_t)0x7FFFFFFF, min2 = (int32_t)0x7FFFFFFF;
        uint32_t n = para->bcn_delay_sliding_win_status;
        for (uint32_t i = 0; i < n; i++) {
            int32_t v = para->bcn_delay_sliding_win_buff[i];
            if (v < min1) { min2 = min1; min1 = v; }
            else if (v < min2) { min2 = v; }
        }
        para->bcn_delay_offset = (n >= 4u) ? min2 : min1;
    }

    /* this beacon's excess delay beyond the baseline: subtracted from the
     * RTC stamp when predicting the next TBTT (reference SDK last_beacon_delay_us) */
    {
        int32_t extra = delay - para->bcn_delay_offset;
        if (extra < 0) extra = 0;
        if (extra > LP_FW_TRACK_EXTRA_CLAMP_US) extra = LP_FW_TRACK_EXTRA_CLAMP_US;
        para->last_beacon_delay_us = extra;
    }
    return 1;
}

/* C2: floor from the knob word; 0 = C2 off. */
static inline uint32_t lp_fw_trkc2_floor_us(void)
{
    uint32_t k = *(volatile uint32_t *)LP_FW_TRKC2_KNOB_ADDR;
    uint32_t f = ((k >> 16) == LP_FW_TRKC2_MAGIC) ? (k & 0xFFFFu) : 0u;
    /* floor + margin must sit below the ceiling or the servo "corrects"
     * against an unreachable lo every clean catch */
    if (f > LP_FW_TRKC2_CEIL_US - LP_FW_TRKC2_MARGIN_US) {
        f = LP_FW_TRKC2_CEIL_US - LP_FW_TRKC2_MARGIN_US;
    }
    return f;
}

/* The lead the next hop should use: the C2 live lead once seeded, else the
 * configured rx_lead_us. Blob and carrier both read it here. */
static inline uint32_t lp_fw_track_lead_us(const volatile iot2lp_para_t *para)
{
    if (lp_fw_trkc2_floor_us() != 0u) {
        uint32_t live = *(volatile uint32_t *)LP_FW_TRKC2_STATE_ADDR & 0xFFFFu;
        if (live != 0u && live <= LP_FW_TRKC2_CEIL_US) {
            return live;
        }
    }
    return para->rx_lead_us;
}

/* C2 feedback, one call per CONVERGED-window outcome (the blob skips
 * post-loss / acquisition windows: their t_wait carries early-wake or the
 * acquire margin). t_wait_us = detect time since RX arm; loss = 1 when the
 * converged window closed without our beacon. */
static inline void lp_fw_track_c2_observe(volatile iot2lp_para_t *para,
                                          uint32_t t_wait_us, int loss)
{
    uint32_t floor = lp_fw_trkc2_floor_us();
    if (floor == 0u) {
        return;
    }
    volatile uint32_t *st = (volatile uint32_t *)LP_FW_TRKC2_STATE_ADDR;
    volatile uint32_t *sk = (volatile uint32_t *)LP_FW_TRKC2_STREAK_ADDR;
    uint32_t live   = *st & 0xFFFFu;
    uint32_t mintw  = *st >> 16;
    uint32_t streak = *sk & 0xFFu;
    uint32_t losses = (*sk >> 8) & 0xFFu;
    uint32_t rises  = (*sk >> 16) & 0x7FFFu;
    uint32_t pend   = *sk & LP_FW_TRKC2_PENDING;
    uint32_t lo     = floor + LP_FW_TRKC2_MARGIN_US;
    if (live == 0u || live > LP_FW_TRKC2_CEIL_US) {
        /* seed from the configured lead; the min re-measures from scratch */
        live = para->rx_lead_us;
        mintw = 0xFFFFu;
        streak = 0u;
    }
    if (loss) {
        /* A converged-window loss is NOT lead evidence by itself (bench
         * 2026-09-02: in busy air losses are RX-path and a larger lead
         * loses MORE -- the first servo ratcheted to the ceiling). Mark it
         * pending; lp_fw_track_c2_postloss() decides from where the
         * recovery catch actually found the beacon. */
        if (losses < 255u) losses++;
        streak = 0u;
        pend = LP_FW_TRKC2_PENDING;
    } else {
        uint32_t tw = (t_wait_us > 0xFFFFu) ? 0xFFFFu : t_wait_us;
        if (tw < mintw) {
            mintw = tw;
        } else if (mintw + LP_FW_TRKC2_CREEP_US < 0xFFFFu) {
            mintw += LP_FW_TRKC2_CREEP_US;
        }
        if (mintw < lo) {
            /* raise by the shortfall; t_wait shifts with the lead, so the
             * tracked min moves with it */
            live += lo - mintw;
            mintw = lo;
            if (rises < 0x7FFFu) rises++;   /* 15 bits: bit31 is PENDING */
            streak = 0u;
        } else {
            streak++;
            if (streak >= LP_FW_TRKC2_DECAY_N) {
                streak = 0u;
                if (mintw >= lo + LP_FW_TRKC2_DECAY_US &&
                    live >= lo + LP_FW_TRKC2_DECAY_US) {
                    live  -= LP_FW_TRKC2_DECAY_US;
                    mintw -= LP_FW_TRKC2_DECAY_US;
                }
            }
        }
    }
    if (live < lo) live = lo;
    if (live > LP_FW_TRKC2_CEIL_US) live = LP_FW_TRKC2_CEIL_US;
    if (mintw > 0xFFFFu) mintw = 0xFFFFu;
    *st = (mintw << 16) | live;
    *sk = pend | (rises << 16) | (losses << 8) | streak;
}

/* C2 post-loss attribution: called at the first catch after a loss (any
 * window). e_catch = where the beacon was relative to the prediction that
 * the LOST cycle used: t_wait - early_wake - live. The lost cycle could
 * have caught it only if e_catch >= -(live - lo); anything earlier is lead
 * evidence and the lead rises by the shortfall (capped). A late or on-time
 * recovery catch says the loss was RX-path: no rise. */
/* C2: drop a pending attribution without judging it (budget wake,
 * reconnect, back-phase recovery: the arm that caught was not lead-based). */
static inline void lp_fw_track_c2_reset_pending(void)
{
    if (lp_fw_trkc2_floor_us() == 0u) {
        return;
    }
    volatile uint32_t *sk = (volatile uint32_t *)LP_FW_TRKC2_STREAK_ADDR;
    *sk &= ~LP_FW_TRKC2_PENDING;
}

static inline void lp_fw_track_c2_postloss(volatile iot2lp_para_t *para,
                                           uint32_t t_wait_us,
                                           uint32_t early_wake_us,
                                           int lead_based)
{
    uint32_t floor = lp_fw_trkc2_floor_us();
    if (floor == 0u) {
        return;
    }
    volatile uint32_t *st = (volatile uint32_t *)LP_FW_TRKC2_STATE_ADDR;
    volatile uint32_t *sk = (volatile uint32_t *)LP_FW_TRKC2_STREAK_ADDR;
    if ((*sk & LP_FW_TRKC2_PENDING) == 0u) {
        return;
    }
    if (!lead_based) {
        /* back-phase recovery: the arm that caught was interval*mult from
         * the loss, not lead-based -- its t_wait says nothing about the lead */
        *sk &= ~LP_FW_TRKC2_PENDING;
        return;
    }
    uint32_t live  = *st & 0xFFFFu;
    uint32_t mintw = *st >> 16;
    uint32_t rises = (*sk >> 16) & 0x7FFFu;
    uint32_t lo    = floor + LP_FW_TRKC2_MARGIN_US;
    if (live == 0u || live > LP_FW_TRKC2_CEIL_US) {
        live = para->rx_lead_us;
    }
    int32_t e_catch = (int32_t)t_wait_us - (int32_t)early_wake_us - (int32_t)live;
    int32_t catchable = -(int32_t)(live - lo);
    if (e_catch < catchable) {
        uint32_t short_us = (uint32_t)(catchable - e_catch);
        if (short_us > LP_FW_TRKC2_RISE_MAX_US) short_us = LP_FW_TRKC2_RISE_MAX_US;
        live += short_us;
        if (live > LP_FW_TRKC2_CEIL_US) live = LP_FW_TRKC2_CEIL_US;
        mintw = 0xFFFFu;          /* the lead moved: re-measure the min */
        if (rises < 0x7FFFu) rises++;
        (void)para;
    }
    *st = (mintw << 16) | live;
    *sk = (*sk & ~(LP_FW_TRKC2_PENDING | 0x7FFF0000u)) | (rises << 16);
}

/*
 * Next sleep interval (us, RTC timebase) landing rx_lead_us ahead of the
 * next expected TBTT. Callers pass their own `now` (blob: lp_rtc_now_us();
 * carrier: HBN RTC read). Falls back to `fallback_us` whenever tracking is
 * off, the anchor is not healthy, or the target maths degenerate.
 */
static inline uint32_t lp_fw_track_next_sleep_gap_us(volatile iot2lp_para_t *para,
                                                     uint64_t now_us, uint32_t fallback_us,
                                                     uint32_t gap_bcns,
                                                     uint32_t extra_lead_us,
                                                     int from_carrier)
{
    if (para->rx_lead_us == 0u || para->last_beacon_stamp_rtc_valid != 1u) {
        para->trk_armed_extra_us = (uint16_t)LP_FW_TRKW_UNTRACKED;
        return fallback_us;
    }
    uint32_t dtim_n = para->dtim_num ? para->dtim_num : 1u;
    uint64_t first_span = lp_fw_track_scale_rtc_us(
        para, (uint64_t)gap_bcns * lp_fw_track_bcn_int_us(para));
    /* The normal DTIM hop scales the same interval for first_span and period.
     * Reuse it; non-DTIM catches still calculate the distinct repeat period. */
    uint64_t period = (gap_bcns == dtim_n) ? first_span
                                           : lp_fw_track_period_rtc_us(para);
    uint64_t base   = para->last_beacon_stamp_rtc_us
                      - (uint64_t)(uint32_t)para->last_beacon_delay_us;
    /* Wake later than asked and every hop lands past its beacon: the wake
     * costs a fixed PDS arm + XTAL settle + blob entry that the target never
     * accounted for (MIN_GUARD only bounds the SHORTEST sleep, it never
     * subtracts). Measured ~11.5 ms on this board 2026-08-08: with a 5 ms lead
     * every tracked wake landed past its beacon and was scored a loss, while
     * the loss ladder's +10 ms early-wake rescued the retry -- two errors
     * cancelling into a stable 50% loss rate. Confirmed on HW by sweeping
     * rx_lead_us with nothing else changed: 5 ms -> 45.97% loss, 9 ms ->
     * 33.21%, 12 ms -> 1.21% (5687 -> 1891 uA), i.e. the loss disappears
     * exactly when the lead crosses the latency. Carry the measured offset in
     * the lead so the asked-for lead is the delivered lead; rx_lead_us then
     * means what it says -- margin between RX-armed and the TBTT. */
    int32_t  late   = from_carrier ? para->wake_late_carrier_us
                                   : para->wake_late_us;
    if (late < 0) { late = 0; }
    if (late > (int32_t)LP_FW_TRACK_WAKE_LATE_MAX_US) {
        late = (int32_t)LP_FW_TRACK_WAKE_LATE_MAX_US;
    }
    uint64_t lead   = (uint64_t)lp_fw_track_lead_us(para) + extra_lead_us +
                      (uint32_t)late;
    uint32_t applied = extra_lead_us;   /* TRKW: lead beyond rx_lead (late cancels) */
    /* A bad first ppm pair can shift a DTIM10 target by more than rx_lead_us.
     * If that first miss turns into alternating loss/reacquire, no consecutive
     * clean pair remains to repair the estimate.  Hold a small acquisition
     * margin until both convergence inputs are warm; it disappears from every
     * steady-state hop, so it does not become permanent RX-on time. */
    if (para->bcn_delay_sliding_win_size == 0u ||
        para->bcn_delay_sliding_win_status < para->bcn_delay_sliding_win_size ||
        para->ppm_stable_cnt < LP_FW_TRACK_PPM_STABLE_MIN) {
        lead += LP_FW_TRACK_ACQUIRE_LEAD_US;
        applied += LP_FW_TRACK_ACQUIRE_LEAD_US;
    }
    /* TRKFIX B -- churn-adaptive lead: every window sample sitting above
     * the baseline guard says the air is contended and the anchor phase is
     * less trustworthy. A clean window adds nothing; a fully churned
     * window adds 8*PAD = 2 ms. Decays by itself as clean beacons refill
     * the window. */
    if (*(volatile uint32_t *)LP_FW_TRKFIX_KNOB_ADDR == LP_FW_TRKFIX_MAGIC) {
        uint32_t nwin = para->bcn_delay_sliding_win_status;
        uint32_t churn = 0u;
        for (uint32_t i = 0u; i < nwin; i++) {
            if (para->bcn_delay_sliding_win_buff[i] >
                para->bcn_delay_offset + (int32_t)LP_FW_TRKFIX_CHURN_GUARD_US) {
                churn++;
            }
        }
        lead += (uint64_t)churn * LP_FW_TRKFIX_CHURN_PAD_US;
        applied += churn * LP_FW_TRKFIX_CHURN_PAD_US;
    }
    /* First hop: gap_bcns beacon intervals from the anchor — the TIM's DTIM
     * count makes this land exactly on the next DTIM beacon even when the
     * anchor is a non-DTIM catch (one-beacon phase lock, any dtim_num).
     * Further hops step whole DTIM periods, staying on that phase. */
    uint64_t target = base + first_span;
    int k;
    for (k = 0; k < 16 && target < now_us + lead + LP_FW_TRACK_MIN_GUARD_US; k++) {
        target += period;
    }
    if (k >= 16) {
        para->trk_armed_extra_us = (uint16_t)LP_FW_TRKW_UNTRACKED;
        return fallback_us;   /* anchor too stale — let the fixed interval ride */
    }
    uint64_t sleep_us = target - lead - now_us;
    /* Final plausibility clamp: any garbage that slipped through the gates
     * (POR-garbage HBN fields, insane lead, wild TIM count) must not arm a
     * PDS interval that never wakes. A sane tracked sleep is always within
     * ~2 periods. */
    if (sleep_us < 1000u || sleep_us > 2u * period + 100000u) {
        para->trk_armed_extra_us = (uint16_t)LP_FW_TRKW_UNTRACKED;
        return fallback_us;
    }
    /* Stamp AFTER every bail-out above: a fallback_us return arms a sleep this
     * function did not shape, and pairing that wake with our target would feed
     * the latency estimator a meaningless sample. `target - lead` is the
     * absolute RTC-us we intend to wake at -- stamping the intent rather than
     * the duration also folds the arm-side skew and the us->tick conversion
     * error into what the blob measures. */
    {
        volatile iot2lp_para_t *w = (volatile iot2lp_para_t *)para;
        uint32_t stamp = (uint32_t)(target - lead);
        w->last_lead_applied_us = (uint32_t)lead;
        w->armed_wake_rtc_us    = stamp ? stamp : 1u;   /* 0 is the "unset" flag */
        w->armed_by_carrier     = (uint8_t)(from_carrier ? 1u : 0u);
    }
    para->trk_armed_extra_us = (uint16_t)(applied > 0xFFFEu ? 0xFFFEu : applied);
    return (uint32_t)sleep_us;
}

/* One full DTIM period from the anchor (anchor assumed on DTIM phase). */
static inline uint32_t lp_fw_track_next_sleep_us_ex(volatile iot2lp_para_t *para,
                                                    uint64_t now_us, uint32_t fallback_us,
                                                    uint32_t extra_lead_us)
{
    uint32_t n = para->dtim_num ? para->dtim_num : 1u;
    return lp_fw_track_next_sleep_gap_us(para, now_us, fallback_us, n,
                                         extra_lead_us, 0);
}

/* Same, but armed by the CARRIER (full firmware re-entering standby) rather
 * than by the blob's per-cycle resleep. Leads with, and learns into, the
 * carrier's own wake latency — see wake_late_carrier_us. */
static inline uint32_t lp_fw_track_next_sleep_us_carrier(volatile iot2lp_para_t *para,
                                                         uint64_t now_us,
                                                         uint32_t fallback_us,
                                                         uint32_t extra_lead_us)
{
    uint32_t n = para->dtim_num ? para->dtim_num : 1u;
    return lp_fw_track_next_sleep_gap_us(para, now_us, fallback_us, n,
                                         extra_lead_us, 1);
}

/* P3.2 wrapper: no extra lead. P3.3's front-phase early-wake passes it via _ex. */
static inline uint32_t lp_fw_track_next_sleep_us(volatile iot2lp_para_t *para,
                                                 uint64_t now_us, uint32_t fallback_us)
{
    return lp_fw_track_next_sleep_us_ex(para, now_us, fallback_us, 0u);
}

/* Poll-window shrink gate: anchor healthy, delay window warm AND the ppm
 * estimate proven stable. Window-full alone is NOT enough: it fills in 8
 * beacons while a cold IIR on a +75000 ppm board still carries a ~26000 ppm
 * residual = tens of ms of prediction error per DTIM10 period — shrinking
 * the poll to lead+55 ms at that point loses beacons. */
static inline int lp_fw_track_converged(const volatile iot2lp_para_t *para)
{
    return para->rx_lead_us != 0u &&
           para->last_beacon_stamp_rtc_valid == 1u &&
           para->bcn_delay_sliding_win_size != 0u &&
           para->bcn_delay_sliding_win_status >= para->bcn_delay_sliding_win_size &&
           para->ppm_stable_cnt >= LP_FW_TRACK_PPM_STABLE_MIN;
}

/* The FULL poll-shrink gate exactly as the blob applies it (converged AND no
 * loss streak in progress). Shared so the blob, the LP_BCNREC_CONVERGED record
 * flag and the CLI's conv= print can never disagree. */
static inline int lp_fw_track_shrink_ok(const volatile iot2lp_para_t *para)
{
    return lp_fw_track_converged(para) && para->continuous_loss_cnt == 0u;
}

/* --- Beacon-loss policy (2026-09-06 v2: a miss is normal and must be cheap) --
 * Air truth on the lp bench (README 2026-09-06 17:2x): a beacon miss is an
 * independent random event -- ~5-10 % per DTIM for the blob's ~1 ms window,
 * ~30 % for a receiver that listens continuously -- because strong off-channel
 * 1 Mbps frames are received as phantoms (valid PLCP, bad FCS) and hold the
 * receiver for their airtime. The AP's TBTT grid is deterministic and the RTC
 * anchor predicts it to ~150 us per DTIM10 hop, so a miss says nothing about
 * timing. Policy, the way consumer LP devices handle it: a miss costs one
 * normal window -- no early wake, no widening beyond a late pad of 1/2/3 ms
 * (cap) that covers RTC drift over the streak -- and the blob keeps sleeping
 * on the anchor. Every LP_FW_LOSS_REALIGN_EVERY misses the poll spans one
 * full beacon interval so ANY beacon re-anchors TSF and DTIM phase (the
 * tracker re-phases a non-DTIM catch from its TIM count). Only after
 * LP_FW_LOSS_RECONNECT_CAP consecutive misses (~2 min at DTIM10) does the blob
 * wake the full firmware with LP_FW_WAKEUP_AP_LOSS. The widening ladder this
 * replaces spent 5-13 mC per step (111/211/311 ms windows) and still saw
 * nothing while the receiver was busy, then paid ~165 mJ for a reconnect after
 * 6 misses: two such storms per hour dominated the standby mean.
 */
/* v4 (2026-09-06): the poll window past the prediction is ~2 ms total
 * (TRKW EXTRA 1 ms + adaptive cap 1 ms) and single misses add NO pad. A ch5
 * monitor capture of 16.5 k LAB02 beacons (this bench's air) put 12-26 % of
 * them > 1 ms past their TBTT, 5-11 % > 5 ms, 2-3 % > 10 ms: the AP itself
 * defers behind traffic. Expected listen time per cycle rises monotonically
 * with the window (every ms ~ +5 uA at DTIM10) because a late beacon caught
 * costs its own delay, while a miss costs the whole window -- so for power
 * the window ends ~2 ms after the prediction and a late beacon is a miss
 * (+1 DTIM of TIM latency, ~+0.2 s mean; AP_LOSS needs 120 in a row). The
 * pad only starts at LP_FW_LOSS_LATE_FROM+1 consecutive misses (RTC drift
 * over a real streak), 1/2/3 ms then holds. */
/* v3 (2026-09-06): a NEW frame landing in the slot after the predicted TBTT
 * that is not our beacon means the beacon overlapped it (phantom or in-channel)
 * or the AP deferred behind it; the AP's deferral is bounded by that frame's
 * airtime, so wait LP_FW_LOSS_FOREIGN_TAIL_US more and then call the miss
 * instead of sitting out the whole margin. Rows carry LP_BCNREC_FOREIGN_ABORT. */
#define LP_FW_LOSS_FOREIGN_TAIL_US  2000u
typedef struct {
    uint8_t  reconnect;     /* 1 = give up -> AP_LOSS full reconnect */
    uint8_t  level;         /* bcn_loss_level: 0 first miss, 1 cnt 2-3, 2 streak,
                             * 3 re-align window, 4 long streak, 5 reconnect */
    uint32_t early_wake_us; /* extra lead for next_sleep_us_ex (always 0 now) */
    uint32_t late_us;       /* extra poll past the prediction */
} lp_loss_action_t;
static inline lp_loss_action_t lp_loss_decide(uint32_t cnt)
{
    const lp_fw_clk_params_t *cp = lp_fw_clk_params();
    lp_loss_action_t a = { 1u, 5u, 0u, 0u };   /* default = reconnect */
    if (cnt == 0u) { a.reconnect = 0u; a.level = 0u; return a; }
    if (cnt >= LP_FW_LOSS_RECONNECT_CAP) { return a; }
    a.reconnect = 0u;
    uint32_t past = (cnt > cp->loss_late_from) ? cnt - cp->loss_late_from : 0u;
    a.late_us = past * cp->loss_late_step_us;
    if (a.late_us > cp->loss_late_cap_us) { a.late_us = cp->loss_late_cap_us; }
    a.early_wake_us = past * cp->loss_early_step_us;
    if (a.early_wake_us > cp->loss_early_cap_us) { a.early_wake_us = cp->loss_early_cap_us; }
    uint32_t realign = cp->loss_realign_every ? cp->loss_realign_every : LP_FW_LOSS_REALIGN_EVERY;
    a.level = (cnt == 1u) ? 0u : (cnt <= 3u) ? 1u : (cnt < realign) ? 2u : 4u;
    if ((cnt % realign) == 0u) {
        a.late_us = LP_FW_LOSS_REALIGN_LATE_US;
        a.level   = 3u;
    }
    return a;
}

/* TRKW: the sleep just armed is not a tracked hop (fallback interval, carrier
 * budget fence, back-phase resleep): the next wake must poll wide. (stub) */
static inline void lp_fw_track_arm_untracked(volatile iot2lp_para_t *para)
{
    para->trk_armed_extra_us = (uint16_t)LP_FW_TRKW_UNTRACKED;
}

/* TRKW: poll margin past the expected beacon, us. (stub) */
static inline uint32_t lp_fw_trkw_margin_us(const volatile iot2lp_para_t *para)
{
    /* 8 ms plus the largest beacon delay excess the delay window has seen
     * (capped): a contended AP keeps its late tail inside the bounded poll
     * instead of walking the loss ladder. Post-loss catches feed the window
     * too, so a tail the short window censors is still learned from the
     * wider recovery windows. */
    uint32_t n = para->bcn_delay_sliding_win_status;
    if (n > LP_FW_BCN_DELAY_WIN_MAX) n = LP_FW_BCN_DELAY_WIN_MAX;
    int32_t worst = 0;
    for (uint32_t i = 0u; i < n; i++) {
        int32_t extra = para->bcn_delay_sliding_win_buff[i] - para->bcn_delay_offset;
        if (extra > worst) worst = extra;
    }
    if (worst > (int32_t)LP_FW_TRKW_MARGIN_CAP_US) worst = (int32_t)LP_FW_TRKW_MARGIN_CAP_US;
    return LP_FW_TRKW_EXTRA_MS * 1000u + (uint32_t)worst;
}

/* Poll window for this wake, ms. Legacy: converged -> lead + 15 ms, anything
 * else -> the 2 s wide window. TRKW: the wide window is only for a wake
 * that cannot predict the beacon (no anchor, untracked, RC32K before the
 * ppm is proven, back-phase/reconnect ladder). With a healthy anchor the
 * prediction is good to well under a ms (XTAL32K is inside +/-500 ppm from
 * the first pair under TRKP), so re-acquisition and front-phase post-loss
 * wakes poll only what they can expect: early_wake + lead + margin. A miss
 * then costs one period, not up to 2 s of listening (HW 2026-09-02: 105-1429
 * ms per cycle, 5-45 mC, for 18 cycles on a stale anchor). */
static inline uint32_t lp_fw_track_poll_ms(const volatile iot2lp_para_t *para)
{
    const lp_fw_clk_params_t *cp = lp_fw_clk_params();
    int trkw = lp_fw_trkw_on();
    if (!trkw) {
        if (lp_fw_track_shrink_ok(para)) {
            return lp_fw_track_lead_us(para) / 1000u + cp->poll_extra_ms;
        }
        return cp->wide_ms;
    }
    /* Only a tracked hop can predict the beacon: the arm that shaped this
     * sleep recorded the lead it applied beyond rx_lead_us (early-wake,
     * acquire, churn, carrier extra) -- a fallback interval, the carrier
     * budget fence and the back-phase resleep record UNTRACKED. */
    uint32_t applied = para->trk_armed_extra_us;
    if (applied == LP_FW_TRKW_UNTRACKED ||
        para->rx_lead_us == 0u || para->last_beacon_stamp_rtc_valid != 1u) {
        return cp->wide_ms;
    }
    if (para->ppm_stable_cnt < cp->ppm_stable_min) {
        return cp->wide_ms;          /* RC32K cold seed can miss by tens of ms */
    }
    uint32_t late = 0u;
    if (para->continuous_loss_cnt != 0u) {
        /* Loss policy: no early wake any more (early_wake_us is 0), only
         * the late pad -- and on a re-align cycle a full beacon interval. */
        lp_loss_action_t a = lp_loss_decide(para->continuous_loss_cnt);
        if (a.early_wake_us > applied) applied = a.early_wake_us;
        late = a.late_us;
    }
    uint32_t poll_us = lp_fw_track_lead_us(para) + applied + lp_fw_trkw_margin_us(para) + late;
    return (poll_us + 999u) / 1000u;   /* round up: the deadline itself is polled */
}

/* Resleep interval for a beacon-loss retry (blob loss branch): a tracked hop
 * to the next TBTT on the RTC anchor (early_wake_us is 0 under the v2 policy;
 * the hook stays so a future policy can wake early). No interval-multiplying
 * back phase (it once compounded resleep_us into a PDS interval that never
 * woke; the fixed form then delayed disconnect detection by minutes). */
static inline uint32_t lp_fw_loss_next_resleep(volatile iot2lp_para_t *para,
                                               uint64_t now_us, lp_loss_action_t act)
{
    return lp_fw_track_next_sleep_us_ex(para, now_us, para->resleep_us, act.early_wake_us);
}

#ifdef __cplusplus
}
#endif

#endif /* __LP_FW_H__ */
