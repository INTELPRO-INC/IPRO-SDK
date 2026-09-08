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
 * IPRO6 SMID (SDIO 3.0 device) WiFi transport — device-side data path.
 *
 * Replaces the legacy SDU bitmap-mailbox trcver_sdio.c for IPRO6. Keeps the
 * public interface (ipro_sdu_init / ipro_sdio_handshake / ipro_sdio_read_cb_register /
 * ipro_sdio_write_pbuf / ipro_sdio_write_cmd / ipro_sdio_tx_timer_stop) and the
 * sdio_top_msg 12-byte wire framing byte-for-byte, so the upper layers
 * (net_wifi_transceiver / sdiowifi_mgmr) are unchanged.
 *
 * Transport model (vs the legacy 16-port RD/WR bitmap):
 *   TX (device->host, "upload")  : ipro6_sdio3_upld_push() queues the frame and
 *       signals the host (FUNCTION1_CONTROL read-len); UPLD_CPL confirms it.
 *   RX (host->device, "download"): pre-post buffers with ipro6_sdio3_dnld_push();
 *       DNLD_CPL hands back {buff,len}; we deliver it, then re-arm the buffer.
 *
 * The SMID driver ISR callback runs in IRQ context, so it only records
 * completions in an SPSC event ring and wakes the worker task; the worker does
 * the pbuf / read-callback / buffer re-arm / TX-confirm work (task context),
 * mirroring the legacy deferred-worker model.
 *
 * One sdio_top_msg per SDIO transfer (no [2B len]+sentinel aggregation): the
 * SMID queue model already delimits each transfer by length, and the host side
 * is ours to define (Option A). Aggregation can be layered on later for TX
 * throughput if the bench shows CMD53-overhead binding.
 */

#include <sdiowifi_securec_wrap.h>
#include <stdint.h>
#include <stdbool.h>
#include "abtrace_dbg.h"    /* DEBUG: AB_TRACE timeline events */

#include "ipro6_sdio3.h"
#include "ipro_irq.h"   /* ipro_irq_save/restore: ISR-safe critical section (the
                         * SMID ISR callback cannot use taskENTER_CRITICAL) */
#include "drv_l1c.h"

#include <sdiowifi_utils.h>
#include "trcver_sdio.h"
#include "net_wifi_transceiver.h"
#include "rx_profile.h"
#include "sdiowifi_mgmr.h"
#include "sdiowifi_mgmr_type.h"

#include <sdiowifi_platform_adapt.h>
#include <sdio_port.h>
#include "sdio_port_ipro6.h"

/* ========================================================================= */
/* Constants / sizing                                                        */
/* ========================================================================= */

#define SMID_FUNC_WIFI        (1)                              /* SMID func # (1-based) */
#define SMID_RX_QUEUE_DEPTH   IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX   /* dnld HW queue depth (8) */
/* The dnld buffer POOL is intentionally larger than the HW queue depth. On every
 * DNLD_CPL the ISR re-arms a FRESH pool buffer immediately, while the just-filled
 * buffer is still being delivered by the worker. With pool == queue depth (the old
 * behaviour, re-arm done in the worker) a host blind-write burst drains all 8 slots
 * before the deferred worker re-arms one -> frame 9 hits ERR_DNLD_QUEUE -> host
 * CMD53 -110 -> card removed -> soft reset. 2x the depth gives an 8-frame cushion
 * for worker scheduling latency; bump if a sustained burst still depletes it. */
/* With dnld aggregation each posted buffer holds many frames, so far fewer
 * buffers are needed for the same frame throughput; keep pool > queue depth so
 * the ISR re-arm still has a cushion (DAT0 back-pressure is the real safety).
 * 10 x 6144 = 60 KB nocache (vs 16 x 2048 = 32 KB before). */
/* [PERF 07-24] 10 -> 9: one 6KB buffer freed to help fund the hot-code-to-RAM
 * relocation in ipro6_if_wifi_flash.ld (.data globs). Must stay > the 8-deep
 * dnld HW queue (static_assert below) so the ISR re-arm keeps a cushion in
 * the disarmed/legacy fallback mode. */
#define SMID_RX_POOL_CNT      (9)                              /* dnld buffer pool       */

/* DEBUG 2026-07-24 Phase-Z v1 (zero-copy TX inject, gated g_sdio_zerocopy):
 * additional dnld buffers that live in ram_wifi (SHRAM) and are armed at an
 * offset chosen so the eth frame lands EXACTLY at slot + 452
 * (= PBUF_LINK_ENCAPSULATION_HLEN in the wifi6_lwip_adapter lwipopts — the
 * 376B fhost desc + 802.11 headers build in the 452B in front). Wire layout
 * per frame is [2B len][4B bl602_sdio_msg][2B subtype][frame] = 8B before the
 * frame, so arming at slot+444 makes a completely UNCHANGED host wire format
 * land zero-copy-ready: the delivered pbuf can point straight into the slot
 * and the SDIO->pool memcpy + pool alloc both disappear for that frame.
 * The slot is owned by the pbuf from claim until TX-cfm free (pbuf custom
 * free -> smid_zc_slot_free -> pool_put + refill). Gate OFF -> the slots act
 * as small ordinary dnld buffers on the legacy copy path (zero behavior
 * change; ADMA overflow is impossible either way: armed data_len caps the
 * transfer at SLOT_SZ-ARM_OFF and the host sends single ~1.5KB frames).
 * 8 slots x 2KB = 16KB ram_wifi (fits the ~19.4KB currently free; the full
 * pool conversion that retires the WRAM TX-payload pool is Phase-Z v2). */
/* Phase-Z v2a (2026-07-24): FULL pool conversion. v1 (8 slots alongside the
 * 24-slot copy pool) measured +15% with only ~26% zc coverage and showed the
 * dominant gain is in-flight CAPACITY (depth hist 9-16: 410->700). v2a makes
 * zero-copy the primary path: 24 zc slots (in-flight capacity for the whole
 * BAW/txdesc window) while the WRAM copy pool shrinks 24->6 and becomes the
 * bounce/pressure fallback. ram_wifi net -2.7KB. Gate-off now runs on the
 * 6-slot copy pool only (DEBUG degenerate mode, NOT the historical legacy
 * baseline — compare v2a against the v1 same-window numbers instead). */
/* MEASURED 2026-07-29, do not shrink this on an argument. The mem-32K campaign
 * proposed 24 -> 20 (8,192 B of ram_wifi) on the theory that the pool was slack,
 * citing sdiowifi_tx_buf.c's F2 note that A-MPDU depth cannot exceed 16 because
 * depth = frames producible per 8 ms cycle. That note predates the hot-code
 * relocation and was concluded at ~9 Mbps uplink; at 18 Mbps it is simply false.
 * g_zc_depth (popcount of smid_zc_out_map, sampled in the claim below) over a
 * 30 M uplink on LAB02:
 *     high-water 23 of 24, and of 19,104 claims
 *     depth > 16 in 37.5%   depth > 18 in 25.3%   depth 21-24 in 14.1%
 * The pool is not slack; it is the binding resource, and a claim that finds it
 * empty stalls the host's CMD53 (with ctg_fb=0 that stall IS the batcher, but
 * at the ceiling it becomes a direct uplink cap). Re-measure g_zc_depth before
 * proposing this again.
 *
 * [P1 07-29] 24 -> 32, the other direction. Little's Law says the pool sets the
 * in-flight window: 24 slots x 1400 B / 21 Mbps implies a 12.8 ms slot residency
 * while one A-MPDU exchange is only 1.9-3.6 ms, so a slot spends most of its
 * life waiting, not airing. Every other stage ceilings above 45 Mbps (air 74-100,
 * SDIO host->dev ~78, CPU ~45-54), so this pool binds first at ~21 Mbps. Funded
 * from the 38,016 B ram_wifi tail the mem-32K campaign left spendable; 32 also
 * equalizes the three in-flight windows (pool, BAW=32, CFG_TXDESC1=32).
 * 32 is the HARD ceiling here: smid_zc_out_map is a uint32_t bitmap. */
#define SMID_ZC_SLOT_CNT      (32)
#define SMID_ZC_SLOT_SZ       (2048)
#define SMID_ZC_ARM_OFF       (444)   /* frame @ +444+8 = +452 = lwip HLEN   */
#define SMID_RX_POOL_TOTAL    (SMID_RX_POOL_CNT + SMID_ZC_SLOT_CNT)
#define SMID_TX_QUEUE_DEPTH   IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX   /* upld queue depth (8)  */
#define SMID_TX_MASK          (SMID_TX_QUEUE_DEPTH - 1)

/* dnld RX buffer size. The host packs 1..N frames [2B len][body]... + a 0x0000
 * sentinel into ONE CMD53 write (TX aggregation), so this is the legacy
 * SDIO_DNLD_AGG_BUF_SIZE (6144) -- NOT the 2048 single-frame size. Kept SMID-
 * local so the shared SDIO_RX_BUF_SIZE (legacy ipro6le path) is untouched. */
#define SMID_DNLD_BUF_SIZE    SDIO_DNLD_AGG_BUF_SIZE

/* upld (device->host) aggregate cap: the device packs 1..N frames
 * [2B len][body]... + 0x0000 sentinel into one buffer and sets the read-count
 * to the total; the host reads the whole block in one sdio_readsb and walks the
 * lengths. Bounded by the 16-bit read-count register; matched to the dnld size. */
#define SMID_UPLD_BUF_SIZE    SDIO_DNLD_AGG_BUF_SIZE

#define SDIO_TX_WDT_TIMEOUT   (450)   /* ms; TX-confirm watchdog                       */
#define SMID_KEEP_READY_MS    (1)     /* ms; poll cadence for the silent host IO-reset */

/* Task priorities (FreeRTOS configMAX_PRIORITIES = 32; higher = more urgent).
 * The worker runs at the top so completions are serviced ahead of the WiFi
 * stack. The keep-ready poll MUST also run at the top: it is the ONLY recovery
 * for the host IO-reset (CCCR 0x06[3]), which silently clears Card-Init-Done and
 * raises NO interrupt on ipro6s. At the WiFi got-IP / scan / DHCP burst the
 * stack runs tasks at prio 26-30 (tcpip 30, fhost rx 30, tx 29, cntrl 27, DHCP
 * 26); a keep-ready poll below that band is starved for the whole burst -- HW
 * measured up to ~888 ms with the old prio 20. If the host's silent IO-reset
 * lands in that window, Card-Init-Done stays cleared -> the card reads
 * not-ready -> the host removes it (the D-2b got-IP flap). At prio 31 the poll
 * preempts the WiFi burst and re-asserts within SMID_KEEP_READY_MS, capping the
 * not-ready window at ~1 ms. The poll is trivial (a couple of register reads on
 * a 1 ms sleep) so it cannot starve the WiFi stack in turn. */
#define SMID_WORKER_PRIO      (31)
#define SMID_KEEP_READY_PRIO  (31)

#define SDU_WORKER_EV_NONE    (0)
#define SDU_WORKER_EV_RESET   (1 << 0)
#define SDU_WORKER_EV_TX_WDT  (1 << 1)

/* ========================================================================= */
/* State                                                                     */
/* ========================================================================= */

typedef void (*sdio_tx_cfm_cb_t)(int idx, void *arg);

#pragma pack(push, 1)
struct sdio_top_msg {
    uint8_t type_lsb;
    uint8_t type_msb;
    uint8_t len_lsb;            /* length of pld, excluding padding */
    uint8_t len_msb;
    uint8_t pld_off;
    uint8_t is_amsdu;
    uint8_t has_twin;
    uint8_t subtype_lsb;
    uint8_t subtype_msb;
    uint8_t first_part_len_lsb;
    uint8_t first_part_len_msb;
    uint8_t _pad0[1];
    uint8_t pld_w_pad[];
};
#pragma pack(pop)

static ipro_sdio_read_cb_t sd_read_callback = NULL;
static sdiowifi_task_t   sdu_irq_handle   = NULL;
static bool              sdu_software_reset = false;

static long              sdu_worker_ev;
static sdiowifi_timer_handle_t sdio_tx_wdt_timer;
static sdiowifi_task_t   smid_keep_ready_handle = NULL;

/* RX buffers: ADMA writes host data here, then the CPU reads it. Placed in the
 * non-cacheable section so reads are coherent without an explicit invalidate
 * (the SMID driver uses the same section for its ADMA descriptor). One per pool
 * entry (the pool is larger than the HW queue depth; see SMID_RX_POOL_CNT);
 * 32-byte aligned for the DMA engine. */
static ATTR_NOCACHE_RAM_SECTION uint8_t
    smid_rx_buf[SMID_RX_POOL_CNT][SMID_DNLD_BUF_SIZE] __attribute__((aligned(32)));

/* Phase-Z v1 zero-copy slots: ram_wifi/SHRAM (the WiFi MAC HW can only fetch
 * TX descriptors/payload from the SHRAM window — OCRAM-alias TX hangs the
 * exchange, bench-proven 07-24). Placed in .lwip_mem.sdiowifi like the TX
 * payload pool so the linker keeps it inside ram_wifi.
 *
 * aligned(4096), NOT 32 — measured 2026-07-29. Each slot carries the MAC TX
 * descriptors in-line (THD at +108, policy table at +180, tx_pbd at +244..264)
 * and the MAC TX DMA fails a descriptor fetch that straddles a 4 KB page:
 * fetch engine halts (AC_x_TX_DMA_DEAD + HW_ERR), the PHY starves, and the
 * leg delivers ZERO packets with a phyif-underrun storm. With aligned(32) the
 * array base phase floats with whatever links below it, and 5 of the 64
 * reachable phases put a page boundary inside the descriptor block: crossing
 * 128/160 (splits the THD) 4/4 and 2/4 dead legs, 224 (policy) 2/4, 256
 * (tx_pbd) 36/36 dead across every layout that hit it — that is what killed
 * SDIO_TX_NORMAL_BUFFER_COUNT 6->18 on 07-28. Slot size is 2048, so with the
 * array 4 KB-aligned every slot starts at offset 0 or 2048 inside its page and
 * the descriptor block [108,264) can never straddle: measured 4/4 clean, 0
 * underruns (exp P), with positive control 4/4 dead in the same window.
 * Costs up to 4 KB-32 B of ram_wifi padding; the tail guard in
 * ipro6_if_wifi_flash.ld relaxes in exchange.
 *
 * SECTION NAME IS LOAD-BEARING. It is its own section, not the shared
 * .lwip_mem.sdiowifi, so ipro6_if_wifi_flash.ld can bracket exactly this array
 * with __smid_zc_slot_start/_end and verify the alignment it actually got.
 * Rename it here and the guard silently stops guarding. */
static __attribute__((section(".lwip_mem.smid_zc_slot"), aligned(4096)))
    uint8_t smid_zc_slot[SMID_ZC_SLOT_CNT][SMID_ZC_SLOT_SZ];

/* The linker checks the END of the bracket, because a start marker is recorded
 * before ld pads to the input section's alignment and so reads the pre-padding
 * address. End-aligned implies base-aligned only while the array is a whole
 * number of pages, so pin that here rather than letting a slot-count change
 * turn the guard into a false alarm. */
_Static_assert(sizeof(smid_zc_slot) % 4096 == 0,
               "smid_zc_slot must span whole 4 KB pages, or the linker-side "
               "alignment guard in ipro6_if_wifi_flash.ld stops being valid");

/* Phase-Z gate + delivery-claim state (worker context only) + counters. */
volatile uint32_t g_sdio_zerocopy = 0;
/* v2b: zero-copy slots enter the dnld pool ONLY after the host opts in (in-band
 * USER_EXT "ZCEN1" -> smid_zc_host_enable). Boot default 0 keeps the pool
 * OCRAM-big-buffer-only, so a legacy host that aggregates >1604B per CMD53 can
 * never hit a small-capped slot (the -110 card-removed landmine). */
static volatile uint8_t smid_zc_armed = 0;
static const uint8_t *smid_zc_cur_arm = NULL;   /* armed ptr being delivered  */
static uint8_t  smid_zc_cur_single = 0;         /* buffer holds exactly 1 frm */
static uint8_t  smid_zc_claimed = 0;            /* consumer took ownership    */
volatile uint32_t smid_zc_claims = 0;
volatile uint32_t smid_zc_frees = 0;
volatile uint32_t smid_zc_reserve_skips = 0;
/* Bitmap of slots currently owned by an in-flight TX pbuf. Needed so a
 * transport reseed (host reset) does NOT reclaim a claimed slot into the
 * pool — the pbuf still returns it later via smid_zc_slot_free(), and a
 * double-entry would let two CMD53s DMA into a slot a TX is reading. */
static volatile uint32_t smid_zc_out_map = 0;   /* v2a: 24 slots */

/* In-flight zero-copy slot depth. The stats line's `claims - frees` is an
 * instantaneous sample taken from a task and never sees the peak, so it cannot
 * answer the only question that matters when sizing this pool: how close does
 * the depth actually get to SMID_ZC_SLOT_CNT? Sampled inside the claim's
 * existing critical section, so the bitmap and the depth can never disagree;
 * rv32 zbb turns the popcount into one instruction.
 *
 *   [0]      high-water depth
 *   [1..7]   residency histogram: <=8 / 9-12 / 13-14 / 15-16 / 17-18 / 19-20 /
 *            21-24. The boundaries are where a pool cut changes behaviour, not
 *            round numbers -- a depth that merely touches 18 once is a very
 *            different animal from one that rides there.
 *
 * Read by symbol from the ELF (`p 0x<addr> 8`), so there is no print path and
 * no flash cost. */
volatile uint32_t g_zc_depth[8];

/* [P2d 07-30] Slot RESIDENCY, the decomposition g_zc_depth cannot give.
 *
 * Little's Law pins tau between 6.4 and 12.8 ms from the depth histogram and the
 * delivered rate, but a bound is not an anatomy: one A-MPDU exchange is only
 * 1.9-3.6 ms, so most of a slot's life is spent somewhere else and the histogram
 * cannot say where. mcycle is sampled at claim and differenced at free. Both
 * sites already sit inside the SAME IRQ-off critical section that maintains
 * smid_zc_out_map, so a timestamp can never be paired across a reseed's
 * disarm-then-rebuild.
 *
 * mcycle counts through preemption on this hart (see txl_cfm.c:287), which is
 * exactly the wall-clock semantics a residency wants - a slot held while the
 * WiFi task is descheduled is still held.
 *
 * Thresholds are in CYCLES, not microseconds, deliberately: this runs per frame
 * on the TX hot path and a RISC-V divide is multi-cycle. The free path therefore
 * costs one csrr, one unsigned subtract and a compare chain; the conversion to
 * microseconds is the reader's job. uint32 subtraction wraps correctly across a
 * single mcycle wrap (2^32 / 240 MHz = 17.9 s).
 *
 * Layout: [0] max cycles, [1..9] buckets, [10] n, [11] sum(cyc)>>12,
 *         [12] discarded (delta > 100 ms: a parked slot or a wrapped counter,
 *              which is not a residency and must not pollute the distribution).
 * 128 B of timestamps + 52 B of counters. */
#define ZC_CPU_MHZ        240u
#define ZC_RES_US(us)     ((uint32_t)(us) * ZC_CPU_MHZ)
static uint32_t zc_claim_cyc[SMID_ZC_SLOT_CNT];
volatile uint32_t g_zc_resid[13];

static inline uint32_t zc_rdcycle(void)
{
    uint32_t c;
    __asm__ volatile("csrr %0, mcycle" : "=r"(c));
    return c;
}

/* Called with IRQs already off, from the claim/free critical sections. */
static inline void zc_resid_note(uint32_t dc)
{
    if (dc > ZC_RES_US(100000)) {      /* parked or wrapped - not a residency */
        g_zc_resid[12]++;
        return;
    }
    if (dc > g_zc_resid[0]) g_zc_resid[0] = dc;
    if      (dc < ZC_RES_US(100))   g_zc_resid[1]++;
    else if (dc < ZC_RES_US(250))   g_zc_resid[2]++;
    else if (dc < ZC_RES_US(500))   g_zc_resid[3]++;
    else if (dc < ZC_RES_US(1000))  g_zc_resid[4]++;
    else if (dc < ZC_RES_US(2000))  g_zc_resid[5]++;
    else if (dc < ZC_RES_US(4000))  g_zc_resid[6]++;
    else if (dc < ZC_RES_US(8000))  g_zc_resid[7]++;
    else if (dc < ZC_RES_US(16000)) g_zc_resid[8]++;
    else                            g_zc_resid[9]++;
    g_zc_resid[10]++;
    g_zc_resid[11] += dc >> 12;
}

static inline int smid_zc_is_slot_arm(const void *armed)
{
    return ((const uint8_t *)armed >= &smid_zc_slot[0][SMID_ZC_ARM_OFF]) &&
           ((const uint8_t *)armed <  &smid_zc_slot[0][0] + sizeof(smid_zc_slot));
}

/* DEBUG 2026-07-24 ZCEN2-stride (multi-frame CMD53, zero-copy preserved):
 * the host packs K frames at a FIXED SMID_ZC_SLOT_SZ wire stride (one frame
 * per stride, [2B len][body][0x0000] + zero-pad each) into ONE CMD53; the
 * device arms one dnld queue element as an ADMA scatter chain of K zc slots
 * (driver ipro6_sdio3_sg_desc_t: stride k's first slot_cap bytes -> slot[k],
 * the pad tail sinks into the same slot's headroom). Every frame therefore
 * lands at its own slot + 452 and the per-slot pre-classify / zc-claim /
 * pbuf-wrap flow is UNCHANGED — this batches the SDIO IRQ + worker-wake
 * ceremony (~30-45us/frame at 1 frame/CMD53) K-for-1 without giving up
 * zero-copy. Handshake: host USER_EXT "ZCEN2" -> reply "ZCOK2 <nslot>
 * <stride>"; boot default DISARMED and gate g_sdio_zc2_en default OFF (a
 * gate-off device answers nslot=0 and arms the v2b single-frame slots
 * instead, so the host falls back to zc_single).
 *
 * nslot default 3: 3 x 2048 = 6144 == SMID_DNLD_BUF_SIZE, so the advertised
 * dnld max never changes and ANY armed element (pre-enable OCRAM contiguous
 * or chain) accepts any post-ZCOK2 write — no arm-transition hazard at all.
 * nslot=4 (8192 capacity) is pokeable for A/B; it relies on the host-side
 * warm-up (first 8 post-ZCOK2 writes capped at 6144) to drain pre-enable
 * 6144-capped elements before any 8192 write, and on chain-only arming
 * (no OCRAM fallback) afterwards. */
#define SMID_ZC2_ELEM_CNT     IPRO6_SDIO3_FUNC_QUEUE_NUM_MAX
volatile uint32_t g_sdio_zc2_en = 1;      /* allow negotiated ZCEN2 stride arming  */
volatile uint32_t g_sdio_zc2_nslot = 3;   /* advertised chain length (2..4)        */
/* OCRAM contiguous fallback when the zc slots starve. DEFAULT OFF: the
 * fallback absorbs slot pressure through the copy path, so DAT0
 * back-pressure never reaches the host, the host qdisc never backlogs, and
 * stride packing never engages (bench 07-24: 30M blast stayed at ~1
 * frame/CMD53 with 58% of frames on the copy path). OFF = arm-starvation
 * stalls the host (deferred PROGRAM_DONE) -> host packs nslot frames/write
 * -> chain CPL returns nslot slots -> batched equilibrium. Cost: while a TX
 * wedge pins every slot, host cmd writes stall until the 450ms TX WDT frees
 * them (DEBUG-acceptable; poke to 1 to restore the always-armed behavior). */
volatile uint32_t g_sdio_zc2_ctg_fb = 0;
/* DAT0 stall bound (ms). With ctg_fb=0 the arm-starvation stall IS the
 * batcher — but on pathological air (LAB03 underrun storm, mpdu ok ~27%)
 * TX cfm slows so much that all slots stay pinned and the stall exceeds the
 * host's mmc timeout: CMD53 EBUSY cascade -> card drop (bench-proven 07-24).
 * The keep-ready 1ms poller counts consecutive stalled ticks; at the bound
 * it sets smid_zc2_relief, which lets the refill's contiguous-OCRAM branch
 * run (bounded ctg_fb) so the blocked host write completes into the copy
 * path. Relief self-clears on the next successful chain arm (slots freed) —
 * batching resumes with no operator action. 50ms = ~20x a normal batching
 * stall, ~20x under the mmc timeout. */
volatile uint32_t g_sdio_zc2_stall_ms = 50;
static volatile uint8_t  smid_zc2_relief = 0;
static volatile uint32_t smid_zc2_relief_trips = 0;
static volatile uint8_t smid_zc2_armed = 0;   /* stride contract live this session */
static uint8_t smid_zc2_nslot_armed;          /* nslot latched at enable           */
static ipro6_sdio3_sg_desc_t smid_zc2_elem[SMID_ZC2_ELEM_CNT];
static ipro6_sdio3_sg_desc_t *smid_zc2_elem_stack[SMID_ZC2_ELEM_CNT];
static uint8_t smid_zc2_elem_top;
static volatile uint32_t smid_zc2_chains;      /* chain CPLs walked                 */
static volatile uint32_t smid_zc2_strides;     /* filled strides delivered          */
static volatile uint32_t smid_zc2_partial;     /* unfilled chain slots returned     */
static volatile uint32_t smid_zc2_ctg;         /* contiguous-fallback arms in zc2   */
static volatile uint32_t smid_zc2_arm_starve;  /* chain arm blocked: slots/elem dry */

static inline int smid_zc2_is_elem(const void *buff)
{
    return ((const uint8_t *)buff >= (const uint8_t *)&smid_zc2_elem[0]) &&
           ((const uint8_t *)buff <  (const uint8_t *)&smid_zc2_elem[0] + sizeof(smid_zc2_elem));
}

/* elem free-stack: ISR (refill/arm) + worker (CPL release) -> irq-save guard */
static void smid_zc2_elem_put(ipro6_sdio3_sg_desc_t *e)
{
    uintptr_t flag = ipro_irq_save();
    if (smid_zc2_elem_top < SMID_ZC2_ELEM_CNT) {
        smid_zc2_elem_stack[smid_zc2_elem_top++] = e;
    }
    ipro_irq_restore(flag);
}

static ipro6_sdio3_sg_desc_t *smid_zc2_elem_get(void)
{
    ipro6_sdio3_sg_desc_t *e = NULL;
    uintptr_t flag = ipro_irq_save();
    if (smid_zc2_elem_top) {
        e = smid_zc2_elem_stack[--smid_zc2_elem_top];
    }
    ipro_irq_restore(flag);
    return e;
}

/* Reset the elem stack to all-free. Caller holds the irq-save section (reseed). */
static void smid_zc2_elem_reset(void)
{
    smid_zc2_elem_top = 0;
    for (int i = 0; i < SMID_ZC2_ELEM_CNT; i++) {
        smid_zc2_elem_stack[smid_zc2_elem_top++] = &smid_zc2_elem[i];
    }
}

/* DEBUG 2026-07-24 ZCEN3 upld stride aggregation (device->host mirror of the
 * ZCEN2 dnld stride): under host-read backlog the device chains K<=4 FILLED
 * upld copy buffers into ONE upld transfer via the SAME ipro6_sdio3_sg_desc_t
 * ADMA scatter path -- READ_START shares the TDF_SG dispatch with WRITE_START,
 * so the driver needs ZERO change. stride == slot_cap == 2048 and pad_off == 0
 * mean no pad-sink lines: <= 8 ADMA lines per chain. Wire per stride:
 * [2B LE len][sdio_top_msg hdr+payload]; intermediate strides occupy the full
 * 2048 (the tail past the frame is stale cbuf content = frames this same host
 * already received, so it is not zeroed), the LAST stride block-rounds to 512,
 * and the doorbell carries the total -- always a 512 multiple, so the upld
 * queue elem stays block-mode. The host walks fixed 2048 strides
 * (k = ceil(count/2048)); a single-frame transfer is just the k==1 case, so
 * the walker is the one universal parser for the new wire.
 *
 * Batching is OPPORTUNISTIC (no latency timer): a frame pushes immediately
 * while the upld queue is idle; once transfers back up (inflight >= thresh)
 * new frames append to an OPEN chain that is pushed at K frames or on the
 * next upld cfm (drain flush) -- batch depth tracks host-read pressure the
 * same way DAT0 back-pressure drives the dnld-side packing.
 *
 * Wire switch: the [2B len] prefix applies to EVERY upld transfer once the
 * host's "ZCEN3" opt-in is granted. The grant reply "ZCOK3 ..." itself goes
 * out on the OLD wire; smid_zc3_wire_arm() flips the format right after the
 * response is pushed, so the strict upld FIFO gives the host a clean old->new
 * boundary (see rnm_user_ext.c). The contract dies on reseed, like ZCEN2. */
#define SMID_ZC3_STRIDE     (2048)   /* wire stride; must equal SMID_UPLD_COPY_SIZE */
#define SMID_ZC3_WIRE_MAX   (IPRO6_SDIO3_SG_MAX * SMID_ZC3_STRIDE)
/* Chain elems: pushed chains hold >= 2 cbufs each, so the 8-cbuf pool bounds
 * in-flight chains at 4; the OPEN chain holds no elem (acquired at push).
 * 4 is exact -- elem-dry keeps the chain open (retried on the next cfm). */
#define SMID_ZC3_ELEM_CNT   (4)
volatile uint32_t g_sdio_zc3_en = 1;      /* allow negotiated ZCEN3 upld agg     */
volatile uint32_t g_sdio_zc3_uslots = 4;  /* advertised max chain depth (2..4)   */
volatile uint32_t g_sdio_zc3_thresh = 1;  /* open a chain at inflight >= thresh  */
static volatile uint8_t smid_zc3_wire = 0;        /* [2B len] upld wire live     */
static volatile uint8_t smid_zc3_arm_pending = 0; /* flip after the ZCOK3 push   */
static uint8_t smid_zc3_uslots_armed;
static ipro6_sdio3_sg_desc_t smid_zc3_elem[SMID_ZC3_ELEM_CNT];
static ipro6_sdio3_sg_desc_t *smid_zc3_elem_stack[SMID_ZC3_ELEM_CNT];
static uint8_t smid_zc3_elem_top;
/* OPEN chain (filled cbufs not yet pushed) -- guarded by the trcver tx_lock:
 * every sender already holds it and the worker's cfm flush takes it too. */
static void    *smid_zc3_open_slot[IPRO6_SDIO3_SG_MAX];
static uint16_t smid_zc3_open_wire[IPRO6_SDIO3_SG_MAX];  /* block-rounded wire len */
static volatile uint8_t smid_zc3_open_cnt;
static volatile uint32_t smid_zc3_chains;     /* chained transfers pushed        */
static volatile uint32_t smid_zc3_frames;     /* frames carried by those chains  */
static volatile uint32_t smid_zc3_single;     /* zc3-wire single-frame pushes    */
static volatile uint32_t smid_zc3_defer;      /* push refused -> chain kept open */
static volatile uint32_t smid_zc3_flush;      /* cfm-driven open-chain flushes   */

/* elem free-stack: worker (cfm release) + tx_lock holders (push) -> irq-save
 * guard for symmetry with the zc2 pool (cheap, and future-proof vs ISR use) */
static void smid_zc3_elem_put(ipro6_sdio3_sg_desc_t *e)
{
    uintptr_t flag = ipro_irq_save();
    if (smid_zc3_elem_top < SMID_ZC3_ELEM_CNT) {
        smid_zc3_elem_stack[smid_zc3_elem_top++] = e;
    }
    ipro_irq_restore(flag);
}

static ipro6_sdio3_sg_desc_t *smid_zc3_elem_get(void)
{
    ipro6_sdio3_sg_desc_t *e = NULL;
    uintptr_t flag = ipro_irq_save();
    if (smid_zc3_elem_top) {
        e = smid_zc3_elem_stack[--smid_zc3_elem_top];
    }
    ipro_irq_restore(flag);
    return e;
}

/* Reset the elem stack to all-free. Caller holds the irq-save section (reseed). */
static void smid_zc3_elem_reset(void)
{
    smid_zc3_elem_top = 0;
    for (int i = 0; i < SMID_ZC3_ELEM_CNT; i++) {
        smid_zc3_elem_stack[smid_zc3_elem_top++] = &smid_zc3_elem[i];
    }
}

/* TX-confirm FIFO: upld completes in queue order (the driver advances upld_out
 * in order), so a head/tail ring parallel to the upld queue maps each UPLD_CPL
 * back to the caller's cfm callback. Guarded by the SDIO critical section. */
struct smid_tx_cfm {
    void             *buff;
    sdio_tx_cfm_cb_t  cb;
    void             *cb_arg;
    sdiowifi_tick_t   tick;
    bool              used;
};
static struct smid_tx_cfm smid_tx_cfm[SMID_TX_QUEUE_DEPTH];
static uint8_t            smid_tx_cfm_head;   /* consumer (confirm) */
static uint8_t            smid_tx_cfm_tail;   /* producer (send)    */
static size_t            smid_tx_inflight;

/* SPSC event ring: producer = SMID driver ISR callback, consumer = worker. */
#define SMID_EVT_RX      (0)   /* dnld complete: {buff,len} carry the frame */
#define SMID_EVT_TX      (1)   /* upld complete: a queued TX finished       */
#define SMID_EVT_RXERR   (2)   /* dnld abort/crc: buffer stays queued       */
#define SMID_EVT_RESET   (3)   /* SMID soft / func reset                    */
/* Phase-Z v2a: 64 (was 32) — the pool grew to 34 entries (10 legacy + 24 zc)
 * and the sizing assert needs ring-1 >= pool_total + tx queue depth.
 * [ZCEN3 funding 07-24] 64 -> 48: the assert floor is 34 + 8 + 1 = 43; the
 * indices use %, not a mask, so no power-of-two requirement. Frees 128B of
 * ram_tcm .bss for the upld chain elems/state (heap floor 80K enforced).
 * [P1 07-29] 48 -> 56: the zc pool grew 24 -> 32, so the assert floor rose to
 * (9 + 32) + 8 + 1 = 50 and 48 no longer clears it. 56 restores the 6-entry
 * headroom 48 had at 24 slots. Costs 64 B of ram_tcm .bss (8 entries x 8 B) --
 * paying back part of what the 64 -> 48 cut borrowed. */
#define SMID_EVT_RING_LEN (56)
struct smid_evt {
    uint8_t  type;
    uint16_t len;
    void    *buff;
};
static volatile struct smid_evt smid_evt_ring[SMID_EVT_RING_LEN];
static volatile uint8_t smid_evt_in;
static volatile uint8_t smid_evt_out;

/* The ring must never overflow: worst case it holds every completed RX buffer not
 * yet processed (<= pool) plus the in-flight TX completions (one slot is reserved
 * to disambiguate full vs empty). */
_Static_assert(SMID_EVT_RING_LEN - 1 >= SMID_RX_POOL_TOTAL + SMID_TX_QUEUE_DEPTH,
               "SMID event ring too small for the RX pool + TX queue");
_Static_assert(SMID_RX_POOL_CNT > SMID_RX_QUEUE_DEPTH,
               "RX pool must exceed the HW queue depth for ISR re-arm to help");

/* diag counters (read via a debug shell if wired) */
static volatile uint32_t sdio_upld_timeout;
static volatile uint32_t smid_rx_err_cnt;
static volatile uint32_t smid_reset_cnt;
static volatile uint32_t smid_evt_drop;
static volatile uint32_t smid_rx_agg_frames;   /* dnld sub-frames de-aggregated      */
static volatile uint32_t smid_rx_agg_ovf;      /* dnld aggregate overrun (malformed) */
/* per-IRQ-event histogram (index = IPRO6_SDIO3_IRQ_EVENT_*, 0..12) — diag */
static volatile uint32_t smid_evt_count[13];

/* ========================================================================= */
/* Worker notify (shared with sdiowifi_mgmr.c reset path)                    */
/* ========================================================================= */

void sdu_software_reset_set(int enable)
{
    sdu_software_reset = !!enable;
}

void notify_sdu_worker(int event, int isr)
{
    if (!sdu_irq_handle) {
        HR_LOGD("%s(%d): sdu worker task is not found\r\n", __func__, __LINE__);
        return;
    }
    if (event) {
        sdu_worker_ev |= event;
    }
    sdiowifi_task_notify(sdu_irq_handle, isr);
}

/* ========================================================================= */
/* TX watchdog timer                                                         */
/* ========================================================================= */

static void sdio_tx_timer_cb(sdiowifi_timer_handle_t timer, void *arg)
{
    (void)timer; (void)arg;
    notify_sdu_worker(SDU_WORKER_EV_TX_WDT, 0);
}

static void start_tx_wdt(void)
{
    if (!sdiowifi_timer_is_valid(sdio_tx_wdt_timer)) {
        sdiowifi_timer_start(sdio_tx_wdt_timer);
    }
}

void ipro_sdio_tx_timer_stop(void)
{
    if (sdiowifi_timer_is_valid(sdio_tx_wdt_timer)) {
        sdiowifi_timer_stop(sdio_tx_wdt_timer);
    }
}

/* ========================================================================= */
/* TX path: device -> host (SMID "upload")                                   */
/* ========================================================================= */

/* Device-owned upld copy pool. The real WiFi upld (ipro_sdio_write_pbuf) COPIES each
 * frame here and releases the source pbuf immediately, instead of queueing the pbuf
 * itself. A WiFi RX pbuf is a transient view into the RX DMA ring: the hardware
 * reuses the ring slot regardless of the software ref, so a zero-copy upld held
 * across the (slow, doorbell-polled) SMID host read let a later RX frame's DMA
 * overwrite a still-queued upld buffer -- the host then CMD53-read descriptor/pbuf
 * pointer garbage (is_amsdu/has_twin != 0 -> amsdu-sanity drop -> the residual D-2b
 * flap). Copying decouples the upld lifetime from the RX ring. Buffers are cached
 * (cleaned before the ADMA) and returned to the pool when the upload completes. */
#define SMID_UPLD_COPY_SIZE  (2048)   /* sdio_top_msg(12) + one WiFi MSDU (~1526);
                                       * larger frames (big AMSDU) are dropped +
                                       * counted -- aggregation is the perf path. */
/* [PERF 07-24] 12 -> 9: still >= HW queue depth (8) + margin; measured
 * 12/12 free during a 25M downlink flood, so 2 buffers were pure slack.
 * Freed 4KB .bss funds the hot-code-to-RAM relocation. */
/* [PERF 07-24 ZCEN2] 9 -> 8: funds the stride chain code+state (~1.8KB into
 * the RAM-resident trcver). 8 == the upld HW queue depth: a frame arriving
 * with all 8 queued drops via smid_upld_nobuf (normal ring-overflow
 * semantics); the same flood measurement showed the pool fully idle, and the
 * Phase-2 upld aggregation rework re-budgets this pool anyway. */
#define SMID_UPLD_COPY_CNT   (8)     /* == HW queue depth (8) */
/* Block-align the upld transfer to the host block size. A sub-block (byte-mode)
 * CMD53 read races the READ_START ISR that programs the ADMA: under the RX flood
 * the ISR is delayed past the host's data phase, which then clocks out the ADMA
 * descriptor / stale FIFO instead of the frame (host reads has_twin!=0 garbage ->
 * amsdu-sanity drop). A pure block read lets the device pace the data phase. The
 * pad is inside the copy buffer; the host de-frames via the header len + ignores
 * the tail. (Matches the host SMID_BLOCK_SIZE and the D-3 aggregation alignment.) */
#define SMID_UPLD_BLOCK      (512)
static uint8_t smid_upld_cbuf[SMID_UPLD_COPY_CNT][SMID_UPLD_COPY_SIZE] __attribute__((aligned(32)));
/* ZCEN3 chains ride the cbufs at wire-stride granularity: the ADMA reads a
 * full stride from every intermediate slot, so the stride can never exceed
 * (and for the fixed-stride host walker must exactly equal) the cbuf size. */
_Static_assert(SMID_ZC3_STRIDE == SMID_UPLD_COPY_SIZE,
               "ZCEN3 wire stride must equal the upld copy buffer size");
static void   *smid_upld_cbuf_free[SMID_UPLD_COPY_CNT];
static uint8_t smid_upld_cbuf_top;
static volatile uint32_t smid_upld_oversize;   /* frame > copy buf -> dropped   */
static volatile uint32_t smid_upld_nobuf;      /* pool empty -> dropped (host slow) */
/* [PERF gate 07-24] upld copy: 0 = MEMCPY_SAFE byte loop, 1 = arch_memcpy_fast.
 * Poke by address (nm g_upld_fastcpy). */
volatile uint8_t g_upld_fastcpy = 0;

static void smid_upld_cbuf_put(void *b)
{
    uintptr_t flag = ipro_irq_save();
    if (smid_upld_cbuf_top < SMID_UPLD_COPY_CNT) {
        smid_upld_cbuf_free[smid_upld_cbuf_top++] = b;
    }
    ipro_irq_restore(flag);
}

static void *smid_upld_cbuf_get(void)
{
    void *b = NULL;
    uintptr_t flag = ipro_irq_save();
    if (smid_upld_cbuf_top) {
        b = smid_upld_cbuf_free[--smid_upld_cbuf_top];
    }
    ipro_irq_restore(flag);
    return b;
}

static void smid_upld_cbuf_init(void)
{
    smid_upld_cbuf_top = 0;
    for (int i = 0; i < SMID_UPLD_COPY_CNT; i++) {
        smid_upld_cbuf_put(smid_upld_cbuf[i]);
    }
}

/* upld completion: return the copy buffer to the pool (worker context) */
static void smid_upld_cbuf_free_cb(int idx, void *arg)
{
    (void)idx;
    smid_upld_cbuf_put(arg);
}

/* Core upload: reserve a TX-confirm slot, queue the buffer on the SMID upld
 * queue, and arm the watchdog. `data` must already be D-cache clean (ADMA reads
 * it) and stay alive until its cfm cb fires. flags: 0 = contiguous buffer,
 * IPRO6_SDIO3_TDF_SG = data is an ipro6_sdio3_sg_desc_t chain (ZCEN3).
 * Returns 0 on success. */
static int smid_upld(void *data, uint16_t total, uint8_t flags,
                     sdio_tx_cfm_cb_t cb, void *cb_arg)
{
    ipro6_smid_dev_t   *dev = sdio_smid_dev();
    struct smid_tx_cfm *slot;
    int                 ret;

    /* SMID_ZC3_WIRE_MAX (8192) > SMID_UPLD_BUF_SIZE (6144): only ZCEN3
     * chains exceed the legacy cap, and both fit the 16-bit doorbell. */
    if (total == 0 || total > SMID_ZC3_WIRE_MAX) {
        return IPRO_MSG_ERR_TIMEOUT;
    }

    /* Reserve the cfm slot BEFORE the push so the UPLD_CPL (which can fire the
     * instant the host reads) always finds it. Serialized by the caller's
     * trcver tx_lock, so head/tail need only the SDIO critical section. */
    sdiowifi_enter_critical();
    if (smid_tx_inflight >= SMID_TX_QUEUE_DEPTH) {
        sdiowifi_exit_critical(0);
        sdio_upld_timeout++;
        return IPRO_MSG_ERR_TIMEOUT;
    }
    slot = &smid_tx_cfm[smid_tx_cfm_tail];
    slot->buff   = data;
    slot->cb     = cb;
    slot->cb_arg = cb_arg;
    slot->tick   = sdiowifi_tick_now(0);
    slot->used   = true;
    smid_tx_cfm_tail = (smid_tx_cfm_tail + 1) & SMID_TX_MASK;
    smid_tx_inflight++;
    sdiowifi_exit_critical(0);

    ipro6_sdio3_trans_desc_t d = {
        .func     = SMID_FUNC_WIFI,
        .flags    = flags,
        .data_len = total,
        .buff     = data,
    };
    ret = ipro6_sdio3_upld_push(dev, &d);
    if (ret != 0) {
        /* roll back the reserved slot (no UPLD_CPL can fire for a failed push) */
        sdiowifi_enter_critical();
        smid_tx_cfm_tail = (smid_tx_cfm_tail - 1) & SMID_TX_MASK;
        smid_tx_inflight--;
        slot->used = false;
        sdiowifi_exit_critical(0);
        sdio_upld_timeout++;
        return IPRO_MSG_ERR_TIMEOUT;
    }

    start_tx_wdt();
    return 0;
}

/* ZCEN3 chain cfm: one cfm entry per TRANSFER -> free every chained cbuf and
 * release the elem. Fired from the worker (UPLD_CPL / WDT / drain). */
static void smid_zc3_chain_free_cb(int idx, void *arg)
{
    ipro6_sdio3_sg_desc_t *e = (ipro6_sdio3_sg_desc_t *)arg;

    (void)idx;
    for (uint8_t k = 0; k < e->cnt; k++) {
        smid_upld_cbuf_put(e->slot[k]);
    }
    smid_zc3_elem_put(e);
}

/* Push the OPEN chain as one upld transfer (a 1-frame chain pushes the cbuf
 * contiguously -- identical wire, no elem burned). Caller holds tx_lock. On a
 * full queue (or elem dry) the chain simply STAYS OPEN and the next cfm/append
 * retries -- the frames are already accepted, so this is deferral, not loss. */
static void smid_zc3_push_open(void)
{
    uint8_t n = smid_zc3_open_cnt;

    if (n == 0) {
        return;
    }
    if (n == 1) {
        if (smid_upld(smid_zc3_open_slot[0], smid_zc3_open_wire[0], 0,
                      smid_upld_cbuf_free_cb, smid_zc3_open_slot[0]) != 0) {
            smid_zc3_defer++;
            return;
        }
        smid_zc3_single++;
        smid_zc3_open_cnt = 0;
        return;
    }

    ipro6_sdio3_sg_desc_t *e = smid_zc3_elem_get();
    if (!e) {
        smid_zc3_defer++;
        return;
    }
    e->cnt      = n;
    e->rsvd     = 0;
    e->stride   = SMID_ZC3_STRIDE;
    e->slot_cap = SMID_ZC3_STRIDE;   /* == stride, pad_off 0: no pad-sink lines */
    e->pad_off  = 0;
    for (uint8_t k = 0; k < n; k++) {
        e->slot[k] = smid_zc3_open_slot[k];
    }
    /* intermediate strides ride full-width; only the last is block-rounded */
    uint16_t total = (uint16_t)((n - 1) * SMID_ZC3_STRIDE + smid_zc3_open_wire[n - 1]);
    if (smid_upld(e, total, IPRO6_SDIO3_TDF_SG, smid_zc3_chain_free_cb, e) != 0) {
        smid_zc3_elem_put(e);
        smid_zc3_defer++;
        return;
    }
    smid_zc3_chains++;
    smid_zc3_frames += n;
    smid_zc3_open_cnt = 0;
}

/* Worker-side flush: push the open chain once the queue drained a slot. The
 * unlocked peek is safe single-core: a chain opened after the peek is caught
 * by the opener's own inflight==0 re-check or by the next cfm. */
static void smid_zc3_cfm_flush(void)
{
    if (!smid_zc3_open_cnt) {
        return;
    }
    sdiowifi_mutex_lock(g_sdiowifi_mgmr.trcver.tx_lock);
    if (smid_zc3_open_cnt) {
        smid_zc3_flush++;
        smid_zc3_push_open();
    }
    sdiowifi_mutex_unlock(g_sdiowifi_mgmr.trcver.tx_lock);
}

/* Drop the OPEN chain (transport reset drain): the frames can never be read
 * by the host now, so just return the cbufs. Worker context. */
static void smid_zc3_drop_open(void)
{
    sdiowifi_mutex_lock(g_sdiowifi_mgmr.trcver.tx_lock);
    for (uint8_t k = 0; k < smid_zc3_open_cnt; k++) {
        smid_upld_cbuf_put(smid_zc3_open_slot[k]);
    }
    smid_zc3_open_cnt = 0;
    sdiowifi_mutex_unlock(g_sdiowifi_mgmr.trcver.tx_lock);
}

/* ZCEN3 accept: chain-or-push one filled cbuf. Caller holds tx_lock; the cbuf
 * already carries the [2B len] wire prefix and is cache-clean for xfer bytes.
 * Returns 0 accepted (pushed, or parked on the open chain and pushed later),
 * <0 dropped (queue saturated; caller returns the cbuf and counts). */
static int smid_zc3_accept(void *buf, uint16_t xfer)
{
    size_t inflight;

    if (smid_zc3_open_cnt >= smid_zc3_uslots_armed) {
        smid_zc3_push_open();           /* full chain deferred earlier: retry */
        if (smid_zc3_open_cnt) {
            return -1;                  /* queue still full -> drop this frame */
        }
    }
    if (smid_zc3_open_cnt) {
        smid_zc3_open_slot[smid_zc3_open_cnt] = buf;
        smid_zc3_open_wire[smid_zc3_open_cnt] = xfer;
        smid_zc3_open_cnt++;
        if (smid_zc3_open_cnt >= smid_zc3_uslots_armed) {
            smid_zc3_push_open();       /* may stay open on a full queue */
        }
        return 0;
    }
    sdiowifi_enter_critical();
    inflight = smid_tx_inflight;
    sdiowifi_exit_critical(0);
    if (inflight >= g_sdio_zc3_thresh) {
        smid_zc3_open_slot[0] = buf;
        smid_zc3_open_wire[0] = xfer;
        smid_zc3_open_cnt = 1;
        /* lost-flush guard: the last inflight may have retired between the
         * check and the open -- then no cfm is coming, so push ourselves */
        sdiowifi_enter_critical();
        inflight = smid_tx_inflight;
        sdiowifi_exit_critical(0);
        if (inflight == 0) {
            smid_zc3_push_open();
        }
        return 0;
    }
    if (smid_upld(buf, xfer, 0, smid_upld_cbuf_free_cb, buf) != 0) {
        return -1;
    }
    smid_zc3_single++;
    return 0;
}

int sdu_send_data(void *data, uint32_t len, bool is_cmd, sdio_tx_cfm_cb_t cb, void *cb_arg)
{
    struct sdio_top_msg *m = (struct sdio_top_msg *)data;

    (void)is_cmd;
    (void)len;

    /* Send exactly header + payload (pld_off + pld_len). The SMID queue model
     * carries the length per transfer, so we never pad to a fixed size. */
    uint16_t pld_len = (uint16_t)(m->len_lsb | (m->len_msb << 8));
    uint16_t total   = (uint16_t)(m->pld_off + pld_len);

    /* ZCEN3 wire: every upld transfer is [2B LE len]-prefixed. The cmd builder
     * (ipro_sdio_write_cmd) reserved 2B of headroom in front of the msg for
     * exactly this; the wire flag is stable under the caller's tx_lock. Byte-
     * mode counts stay fine: the host walker sees k = ceil(cnt/2048) = 1. */
    if (smid_zc3_wire) {
        uint8_t *w = (uint8_t *)data - 2;
        w[0] = (uint8_t)(total & 0xff);
        w[1] = (uint8_t)(total >> 8);
        L1C_DCache_Clean_By_Addr((uintptr_t)w, (uint32_t)total + 2);
        return smid_upld(w, (uint16_t)(total + 2), 0, cb, cb_arg);
    }

    /* ADMA reads `data` from cached RAM (pbuf / heap) -> clean the D-cache. */
    L1C_DCache_Clean_By_Addr((uintptr_t)data, total);

    return smid_upld(data, total, 0, cb, cb_arg);
}

#if defined(CONFIG_SMID_RAW_TX_TEST)
/* ---- raw send (data-plane bring-up test; no sdio_top_msg framing) --------
 * Copies `data` into a pre-allocated pool buffer returned to the pool when the
 * upload completes. A malloc-per-aggregate (the old scheme) exhausts the small
 * TCM heap under a sustained device->host stream (~6 x 6144 = 36 KB) and trips
 * the malloc-failed hook, hanging the device -- that, not any doorbell race, is
 * why the synthetic sustained-upld test wedged. The real WiFi TX path
 * (ipro_sdio_write_pbuf) uses pbuf memory and never malloc'd; this pool only backs
 * the raw test sender. Exposed via sdio_port_ipro6.h for the loopback/TX test. */
#define SMID_TX_POOL_CNT  (4)   /* pool-based producer refills faster than the host
                                 * drains, so the queue need not be full-depth; 4 x
                                 * 6144 = 24 KB keeps TCM in budget */
static uint8_t smid_tx_pool[SMID_TX_POOL_CNT][SMID_UPLD_BUF_SIZE] __attribute__((aligned(32)));
static void   *smid_tx_pool_buf[SMID_TX_POOL_CNT];
static uint8_t smid_tx_pool_top;

static void smid_tx_pool_put(void *b)
{
    uintptr_t flag = ipro_irq_save();
    if (smid_tx_pool_top < SMID_TX_POOL_CNT) {
        smid_tx_pool_buf[smid_tx_pool_top++] = b;
    }
    ipro_irq_restore(flag);
}

static void *smid_tx_pool_get(void)
{
    void *b = NULL;
    uintptr_t flag = ipro_irq_save();
    if (smid_tx_pool_top) {
        b = smid_tx_pool_buf[--smid_tx_pool_top];
    }
    ipro_irq_restore(flag);
    return b;
}

static void smid_tx_pool_init(void)
{
    smid_tx_pool_top = 0;
    for (int i = 0; i < SMID_TX_POOL_CNT; i++) {
        smid_tx_pool_put(smid_tx_pool[i]);
    }
}

static void smid_raw_free_cb(int idx, void *arg)
{
    (void)idx;
    smid_tx_pool_put(arg);
}

int ipro_sdio_send_raw(const void *data, uint16_t len)
{
    if (len == 0 || len > SMID_UPLD_BUF_SIZE) {
        return -1;
    }
    void *buf = smid_tx_pool_get();
    if (!buf) {
        return -1;   /* pool empty -> caller retries (host drains -> cfm frees) */
    }
    MEMCPY_SAFE(buf, len, data, len);
    L1C_DCache_Clean_By_Addr((uintptr_t)buf, len);   /* cached pool -> clean for ADMA */

    int ret = smid_upld(buf, len, 0, smid_raw_free_cb, buf);
    if (ret != 0) {
        smid_tx_pool_put(buf);
    }
    return ret;
}
#endif /* CONFIG_SMID_RAW_TX_TEST */

/* confirm one completed upload (worker context) */
static void smid_tx_confirm_one(void *buff)
{
    struct smid_tx_cfm done = { 0 };
    bool have = false;

    sdiowifi_enter_critical();
    if (smid_tx_inflight) {
        struct smid_tx_cfm *c = &smid_tx_cfm[smid_tx_cfm_head];
        done = *c;
        c->used = false;
        smid_tx_cfm_head = (smid_tx_cfm_head + 1) & SMID_TX_MASK;
        smid_tx_inflight--;
        have = true;
    }
    sdiowifi_exit_critical(0);

    (void)buff;   /* FIFO order; buff is only a sanity hint */
    if (have && done.cb) {
        done.cb(0, done.cb_arg);
    }
}

static void smid_tx_check_timeout(void)
{
    sdiowifi_tick_t now = sdiowifi_tick_now(0);

    for (;;) {
        struct smid_tx_cfm done = { 0 };
        bool have = false;

        sdiowifi_enter_critical();
        if (smid_tx_inflight) {
            struct smid_tx_cfm *c = &smid_tx_cfm[smid_tx_cfm_head];
            if (c->used &&
                (now - c->tick) > sdiowifi_ms2ticks(SDIO_TX_WDT_TIMEOUT)) {
                done = *c;
                c->used = false;
                smid_tx_cfm_head = (smid_tx_cfm_head + 1) & SMID_TX_MASK;
                smid_tx_inflight--;
                have = true;
            }
        }
        sdiowifi_exit_critical(0);

        if (!have) {
            break;
        }
        if (done.cb) {
            done.cb(0, done.cb_arg);
        }
    }
}

/* ========================================================================= */
/* RX path: host -> device (SMID "download")                                 */
/* ========================================================================= */

/* Free dnld-buffer pool (LIFO). A buffer is "checked out" from the moment it is
 * armed in the dnld queue, through the host write and the worker delivery, until
 * the worker returns it here. The ISR (pool_get to re-arm on DNLD_CPL) and the
 * worker (pool_get in refill, pool_put after delivery) both touch it, so it is
 * guarded by ipro_irq_save/restore -- an ISR-safe critical section. */
static void   *smid_rx_pool[SMID_RX_POOL_TOTAL];
static uint8_t  smid_rx_pool_top;   /* count of free buffers on the stack */

static void smid_rx_pool_put(void *buff)
{
    uintptr_t flag = ipro_irq_save();
    if (smid_rx_pool_top < SMID_RX_POOL_TOTAL) {
        smid_rx_pool[smid_rx_pool_top++] = buff;
    }
    ipro_irq_restore(flag);
}

static void *smid_rx_pool_get(void)
{
    void *buff = NULL;
    uintptr_t flag = ipro_irq_save();
    if (smid_rx_pool_top) {
        /* Phase-Z: the pool is a LIFO, so a hot subset of buffers recycles
         * and the zero-copy slots tend to sink to the stack bottom (measured
         * ~7% claim rate). With the gate on, prefer a zero-copy slot when one
         * is anywhere in the stack (swap-remove; <=18-entry scan under the
         * IRQ-save section) so data frames land zero-copy-eligible. */
        int pick = (int)smid_rx_pool_top - 1;
        if (g_sdio_zerocopy) {
            for (int i = pick; i >= 0; i--) {
                if (smid_zc_is_slot_arm(smid_rx_pool[i])) {
                    pick = i;
                    break;
                }
            }
        }
        buff = smid_rx_pool[pick];
        smid_rx_pool[pick] = smid_rx_pool[--smid_rx_pool_top];
    }
    ipro_irq_restore(flag);
    return buff;
}

/* Pop a free buffer of one kind only: 1 = zero-copy slot arm, 0 = OCRAM
 * contiguous buffer. Scan-based swap-remove like the preference scan above
 * (<= pool-total entries under the irq-save section). */
static void *smid_rx_pool_get_kind(int zc_kind)
{
    void *buff = NULL;
    uintptr_t flag = ipro_irq_save();
    for (int i = (int)smid_rx_pool_top - 1; i >= 0; i--) {
        if (!!smid_zc_is_slot_arm(smid_rx_pool[i]) == !!zc_kind) {
            buff = smid_rx_pool[i];
            smid_rx_pool[i] = smid_rx_pool[--smid_rx_pool_top];
            break;
        }
    }
    ipro_irq_restore(flag);
    return buff;
}

/* Return a delivered-but-unclaimed buffer. A zero-copy slot arriving here
 * while DISARMED (a reseed raced the delivery) is PARKED instead of pooled —
 * pooling it would hand a 1604B-capped slot to a legacy-host session; the
 * next host enable re-adds every slot not held by out_map. */
static void smid_rx_buf_dispose(void *buff)
{
    uintptr_t flag = ipro_irq_save();
    if (smid_zc_is_slot_arm(buff) && !smid_zc_armed) {
        ipro_irq_restore(flag);
        return;
    }
    if (smid_rx_pool_top < SMID_RX_POOL_TOTAL) {
        smid_rx_pool[smid_rx_pool_top++] = buff;
    }
    ipro_irq_restore(flag);
}

/* Arm one buffer on the SMID dnld queue. Returns the driver push result (0 = ok,
 * <0 if the HW queue is full). */
static int smid_rx_arm(void *buff)
{
    ipro6_sdio3_trans_desc_t d = {
        .func     = SMID_FUNC_WIFI,
        /* Phase-Z: zero-copy slots are small — cap the ADMA transfer so a
         * host aggregate can never overrun the 2KB slot. */
        .data_len = smid_zc_is_slot_arm(buff)
                        ? (uint16_t)(SMID_ZC_SLOT_SZ - SMID_ZC_ARM_OFF)
                        : (uint16_t)SMID_DNLD_BUF_SIZE,
        .buff     = buff,
    };
    return ipro6_sdio3_dnld_push(sdio_smid_dev(), &d);
}

/* ZCEN2: arm one dnld queue element as a scatter chain of nslot zc slots.
 * ALL-OR-NOTHING: a shorter chain would advertise less capacity than the host
 * may write (truncation -> the -110 cascade class), so a partial pop rolls
 * back. Returns 0 armed, -1 no free elem (== queue full: elem cnt == queue
 * depth), -2 slot starve (caller may fall back), -3 push refused. */
static int smid_zc2_arm_chain(void)
{
    ipro6_sdio3_sg_desc_t *e = smid_zc2_elem_get();
    uint8_t n = smid_zc2_nslot_armed;
    uint8_t k;

    if (!e) {
        return -1;
    }
    for (k = 0; k < n; k++) {
        e->slot[k] = smid_rx_pool_get_kind(1);
        if (!e->slot[k]) {
            while (k--) {
                smid_rx_pool_put(e->slot[k]);
            }
            smid_zc2_elem_put(e);
            smid_zc2_arm_starve++;
            return -2;
        }
    }
    e->cnt      = n;
    e->stride   = SMID_ZC_SLOT_SZ;
    e->slot_cap = SMID_ZC_SLOT_SZ - SMID_ZC_ARM_OFF;
    e->pad_off  = SMID_ZC_ARM_OFF;

    ipro6_sdio3_trans_desc_t d = {
        .func     = SMID_FUNC_WIFI,
        .flags    = IPRO6_SDIO3_TDF_SG,
        .data_len = (uint16_t)(n * SMID_ZC_SLOT_SZ),   /* chain receive capacity */
        .buff     = e,
    };
    if (ipro6_sdio3_dnld_push(sdio_smid_dev(), &d) != 0) {
        for (k = 0; k < n; k++) {
            smid_rx_pool_put(e->slot[k]);
        }
        smid_zc2_elem_put(e);
        return -3;
    }
    return 0;
}

/* Top up the dnld HW queue from the free pool, stopping when the queue is full or
 * the pool is empty. Safe from both the ISR (immediate re-arm on DNLD_CPL) and the
 * worker (recovery after a buffer is returned, when the pool had run dry). */
static void smid_rx_refill(void)
{
    if (smid_zc2_armed) {
        /* ZCEN2 stride mode: chains only. When the zc slots run dry (all held
         * by in-flight TX pbufs / delivery), fall back to an OCRAM contiguous
         * buffer to keep the queue armed — but ONLY while the advertised
         * chain capacity fits it (nslot=3: 6144 == SMID_DNLD_BUF_SIZE); the
         * worker stride-walks it on delivery. At nslot=4 the fallback is
         * skipped and DAT0 back-pressure paces the host instead. */
        for (;;) {
            int ret = smid_zc2_arm_chain();
            if (ret == 0) {
                smid_zc2_relief = 0;    /* slots recovered -> batching resumes */
                continue;
            }
            if (ret != -2) {
                break;              /* queue full */
            }
            if (!g_sdio_zc2_ctg_fb && !smid_zc2_relief) {
                break;              /* slot starve -> DAT0 back-pressure (see gate) */
            }
            if ((uint32_t)smid_zc2_nslot_armed * SMID_ZC_SLOT_SZ > SMID_DNLD_BUF_SIZE) {
                break;              /* nslot=4: no safe contiguous fallback */
            }
            void *buff = smid_rx_pool_get_kind(0);
            if (!buff) {
                break;              /* pool fully drained */
            }
            if (smid_rx_arm(buff) != 0) {
                smid_rx_pool_put(buff);
                break;
            }
            smid_zc2_ctg++;
        }
        return;
    }
    for (;;) {
        void *buff = smid_rx_pool_get();
        if (!buff) {
            break;                      /* pool exhausted */
        }
        if (smid_rx_arm(buff) != 0) {
            smid_rx_pool_put(buff);     /* HW queue full -> return buffer, done */
            break;
        }
    }
}

/* Reclaim every RX buffer to the free pool and re-arm the dnld queue from scratch.
 * Used at handshake (initial seed) and on a transport re-sync after a host reset:
 * the driver's reset handler emptied the dnld queue, so the buffers that were armed
 * are now orphaned -- put them all back and re-arm, restoring the exact initial
 * post-handshake pool/queue state (no leaked or double-armed buffers). */
static void smid_rx_pool_reseed(void)
{
    uintptr_t flag = ipro_irq_save();
    smid_rx_pool_top = 0;
    /* v2b: a reseed is a HOST-CONTRACT reset (boot handshake, or a transport
     * re-sync after the host func-reset on re-enumeration). The new host may
     * be a legacy aggregator, so the zero-copy opt-in dies with the old
     * session: disarm, close the gate, seed OCRAM big buffers only. A v2b
     * host re-advertises (ZCEN1) on every probe and gets the slots back.
     * In-flight slots (out_map) return via smid_zc_slot_free(), which parks
     * them while disarmed. Disarm inside this critical section so a
     * concurrent slot_free can never re-add a slot to the rebuilt pool. */
    smid_zc_armed = 0;
    g_sdio_zerocopy = 0;
    /* ZCEN2: the stride contract dies with the session too; every chain elem
     * armed in the (zeroed) driver queue is dead, so reset the elem stack. */
    smid_zc2_armed = 0;
    smid_zc2_relief = 0;
    smid_zc2_elem_reset();
    /* ZCEN3: the upld wire contract dies with the session too. In-flight
     * chains were already reclaimed by the tx drain (resync runs drain first);
     * on the boot-time handshake path nothing is in flight. */
    smid_zc3_wire = 0;
    smid_zc3_arm_pending = 0;
    smid_zc3_elem_reset();
    ipro_irq_restore(flag);
    for (int i = 0; i < SMID_RX_POOL_CNT; i++) {
        smid_rx_pool_put(smid_rx_buf[i]);
    }
    smid_rx_refill();
}

/* v2b host opt-in: arm the zero-copy slots into the dnld pool and open the
 * claim gate. Called from the RNM USER_EXT handler when the host advertises
 * single-frame TX ("ZCEN1"), or from the DEBUG CLI. Idempotent. Returns the
 * number of slots armed (0 on repeat calls). Task context. */
int smid_zc_host_enable(void)
{
    int armed = 0;
    uintptr_t flag = ipro_irq_save();

    /* One critical section for the arm + slot adds: a concurrent
     * smid_zc_slot_free (cfm of a slot claimed before the last reseed) sees
     * armed only after every free slot is already pooled, so it can never
     * double-add the slot it just cleared from out_map. */
    if (!smid_zc_armed) {
        for (int i = 0; i < SMID_ZC_SLOT_CNT; i++) {
            if (!(smid_zc_out_map & (1u << i)) &&
                smid_rx_pool_top < SMID_RX_POOL_TOTAL) {
                smid_rx_pool[smid_rx_pool_top++] = &smid_zc_slot[i][SMID_ZC_ARM_OFF];
                armed++;
            }
        }
        smid_zc_armed = 1;
    }
    g_sdio_zerocopy = 1;
    ipro_irq_restore(flag);
    if (armed) {
        smid_rx_refill();
    }
    return armed;
}

/* ZCEN2 host opt-in: arm the zc slots (if not already) and switch the refill
 * policy to scatter chains. Returns the slot count armed into the pool, or
 * <0 when stride mode is REFUSED (-1 gate off, -2 a v2b ZCEN1 session already
 * armed single elements whose 1604B cap a stride write would overrun) — the
 * caller then answers nslot=0 and the host stays on the zc_single contract.
 * Task context (RNM handler). */
int smid_zc2_host_enable(int *nslot_out, int *stride_out)
{
    int armed = 0;
    uint32_t nslot = g_sdio_zc2_nslot;

    *stride_out = SMID_ZC_SLOT_SZ;
    *nslot_out = 0;
    if (!g_sdio_zc2_en) {
        return -1;
    }
    if (nslot < 2) {
        nslot = 2;
    } else if (nslot > IPRO6_SDIO3_SG_MAX) {
        nslot = IPRO6_SDIO3_SG_MAX;
    }

    uintptr_t flag = ipro_irq_save();
    if (smid_zc_armed && !smid_zc2_armed) {
        /* ZCEN1 came first this session: single zc elements are already in
         * the dnld queue and cannot be recalled — refuse stride. */
        ipro_irq_restore(flag);
        return -2;
    }
    if (!smid_zc_armed) {
        for (int i = 0; i < SMID_ZC_SLOT_CNT; i++) {
            if (!(smid_zc_out_map & (1u << i)) &&
                smid_rx_pool_top < SMID_RX_POOL_TOTAL) {
                smid_rx_pool[smid_rx_pool_top++] = &smid_zc_slot[i][SMID_ZC_ARM_OFF];
                armed++;
            }
        }
        smid_zc_armed = 1;
    }
    smid_zc2_nslot_armed = (uint8_t)nslot;
    smid_zc2_armed = 1;
    g_sdio_zerocopy = 1;
    ipro_irq_restore(flag);

    smid_rx_refill();
    *nslot_out = (int)nslot;
    return armed;
}

/* ZCEN3 host opt-in: ZCEN2 dnld-stride semantics PLUS the upld aggregation
 * contract ([2B len]-prefixed upld wire, chained transfers). Returns the zc2
 * slot count armed (>= 0) with *uslots_out > 0 when upld agg is granted, or
 * < 0 when the dnld stride is refused (the caller falls back exactly like
 * ZCEN2 -- no upld agg either: one mode, one wire). The wire itself flips in
 * smid_zc3_wire_arm() AFTER the caller pushes the ZCOK3 response, so the
 * response still travels on the OLD wire and the strict upld FIFO hands the
 * host a clean format boundary. Task context (RNM handler). */
int smid_zc3_host_enable(int *nslot_out, int *stride_out, int *uslots_out)
{
    int armed;
    uint32_t u = g_sdio_zc3_uslots;

    *uslots_out = 0;
    armed = smid_zc2_host_enable(nslot_out, stride_out);
    if (armed < 0) {
        return armed;
    }
    if (!g_sdio_zc3_en) {
        return armed;       /* dnld stride granted, upld agg refused */
    }
    if (u < 2) {
        u = 2;
    } else if (u > IPRO6_SDIO3_SG_MAX) {
        u = IPRO6_SDIO3_SG_MAX;
    }
    smid_zc3_uslots_armed = (uint8_t)u;
    smid_zc3_arm_pending = 1;
    *uslots_out = (int)u;
    return armed;
}

/* Flip the upld wire to the [2B len]-prefixed format. MUST be called right
 * after the ZCOK3 response is pushed (rnm_user_ext.c). Takes the tx_lock so
 * the flip can never land inside another sender's build+push. A sender that
 * slips between the response push and this call still emits one old-wire
 * transfer ordered after ZCOK3 -- the host walker drops that single frame
 * (WiFi-lossy, benign); arming BEFORE the response would instead prefix the
 * ZCOK3 itself and lose the whole grant. */
void smid_zc3_wire_arm(void)
{
    if (!smid_zc3_arm_pending) {
        return;
    }
    sdiowifi_mutex_lock(g_sdiowifi_mgmr.trcver.tx_lock);
    smid_zc3_arm_pending = 0;
    smid_zc3_wire = 1;
    sdiowifi_mutex_unlock(g_sdiowifi_mgmr.trcver.tx_lock);
}

/* ========================================================================= */
/* Phase-Z v1 zero-copy claim / free interface (consumer:                    */
/* net_wifi_transceiver.c handle_sta_ap_to_wifi_tx)                          */
/* ========================================================================= */

/* Claim the CURRENT delivery buffer for zero-copy TX. Succeeds only when the
 * frame being delivered is the single frame of a zero-copy slot, the gate is
 * on, and the free pool keeps a re-arm reserve. On success the slot is NOT
 * returned to the pool after delivery — ownership passes to the caller's
 * pbuf until smid_zc_slot_free(). Worker context only. Returns the slot BASE
 * (pbuf payload_mem; the frame sits at base + 452) or NULL. */
void *smid_zc_try_claim(const void *frame, uint16_t frame_len)
{
    if (!g_sdio_zerocopy || !smid_zc_cur_arm || !smid_zc_cur_single)
        return NULL;
    /* Placement guard, checked on the real address so no linker/section change
     * can defeat it. Two invariants, same safe failure: latch the gate off and
     * fall back to the legacy copy path rather than corrupt silently.
     *   1. Address space (SoC rule: 0x1xxxxxxx = cacheable, 0x2xxxxxxx =
     *      non-cacheable). The whole zero-copy path — SMID ADMA write, CPU
     *      de-agg read, fhost desc/THD build, MAC HW fetch — is coherent ONLY
     *      because the slots sit in non-cacheable space.
     *   2. 4 KB alignment. Each slot carries its MAC descriptors in-line and
     *      the TX DMA halts on a descriptor fetch that straddles a page
     *      (measured 36/36 dead legs). ipro6_if_wifi_flash.ld asserts this at
     *      link time too, but that assert brackets the array's own input
     *      section and goes quiet if the section is ever renamed here — this
     *      runtime check is the backstop that cannot be renamed away. */
    if ((((uintptr_t)&smid_zc_slot[0][0]) >> 28) != 0x2u ||
        (((uintptr_t)&smid_zc_slot[0][0]) & 4095) != 0) {
        g_sdio_zerocopy = 0;
        return NULL;
    }
    if ((const uint8_t *)frame != smid_zc_cur_arm + 8 || frame_len > 1514)
        return NULL;
    /* Re-arm reserve: never let a claim leave the free pool too shallow for
     * the ISR to keep the dnld HW queue armed (host would hit ERR_DNLD_QUEUE
     * -> CMD53 -110). Refusing here just falls back to the legacy copy path.
     * ZCEN2 stride mode skips the reserve: chains hold 3-4 slots each so the
     * pool legitimately runs near-empty at depth, refused claims would push
     * the load onto the 6-buffer bounce pool instead, and arm-starvation is
     * SAFE there — the driver withholds PROGRAM_DONE (DAT0 back-pressure)
     * until a cfm frees slots, so no -110 can result. */
    if (!smid_zc2_armed && smid_rx_pool_top < 2) {
        smid_zc_reserve_skips++;
        return NULL;
    }
    smid_zc_claimed = 1;
    smid_zc_claims++;
    {
        uint8_t *base = (uint8_t *)smid_zc_cur_arm - SMID_ZC_ARM_OFF;
        int idx = (int)((base - &smid_zc_slot[0][0]) / SMID_ZC_SLOT_SZ);
        uintptr_t flag = ipro_irq_save();
        uint32_t  d;
        smid_zc_out_map |= (1u << idx);
        d = (uint32_t)__builtin_popcount(smid_zc_out_map);
        if (d > g_zc_depth[0]) g_zc_depth[0] = d;
        if      (d <=  8) g_zc_depth[1]++;
        else if (d <= 12) g_zc_depth[2]++;
        else if (d <= 14) g_zc_depth[3]++;
        else if (d <= 16) g_zc_depth[4]++;
        else if (d <= 18) g_zc_depth[5]++;
        else if (d <= 20) g_zc_depth[6]++;
        else              g_zc_depth[7]++;
        zc_claim_cyc[idx] = zc_rdcycle() | 1u;   /* |1: 0 means "never claimed" */
        ipro_irq_restore(flag);
        return base;
    }
}

/* Return a claimed slot to the dnld pool (pbuf custom-free hook; runs in the
 * WiFi task at TX-cfm time). pool_put is IRQ-safe; refill re-arms. v2b: if a
 * reseed disarmed zero-copy meanwhile (host contract reset), the slot is
 * PARKED (bit cleared, not pooled) — the next smid_zc_host_enable() re-adds
 * it. armed-check + put share one critical section so this can't race the
 * reseed's disarm-then-rebuild. */
void smid_zc_slot_free(void *slot_base)
{
    int idx = (int)(((uint8_t *)slot_base - &smid_zc_slot[0][0]) / SMID_ZC_SLOT_SZ);
    int pooled;
    uintptr_t flag = ipro_irq_save();
    smid_zc_out_map &= ~(1u << idx);
    if (idx >= 0 && idx < SMID_ZC_SLOT_CNT && zc_claim_cyc[idx]) {
        zc_resid_note(zc_rdcycle() - zc_claim_cyc[idx]);
        zc_claim_cyc[idx] = 0;
    }
    pooled = smid_zc_armed;
    if (pooled && smid_rx_pool_top < SMID_RX_POOL_TOTAL) {
        smid_rx_pool[smid_rx_pool_top++] = (uint8_t *)slot_base + SMID_ZC_ARM_OFF;
    }
    ipro_irq_restore(flag);
    smid_zc_frees++;
    if (pooled) {
        smid_rx_refill();
    }
}

/* Slot index for the consumer's per-slot pbuf wrappers. */
int smid_zc_slot_index(const void *slot_base)
{
    int idx = (int)(((const uint8_t *)slot_base - &smid_zc_slot[0][0]) / SMID_ZC_SLOT_SZ);
    return (idx >= 0 && idx < SMID_ZC_SLOT_CNT) ? idx : -1;
}

/* DEBUG F6 (uncommitted): runtime override of the SMID worker priority for
 * A/B. Default SMID_WORKER_PRIO=31 outranks every WiFi data task (fhost_tx
 * 29, rx 28, wifi 27) — under uplink blast the admitted-inject bursts starve
 * cfm/push/close on the lower tasks. Shell: `wprio 26` (cli_al.c). */
void smid_worker_prio_set(uint32_t prio)
{
    extern void rtos_priority_set(rtos_task_handle handle, rtos_prio priority);
    if (sdu_irq_handle) {
        rtos_priority_set(sdu_irq_handle, prio);
    }
}

/* Deliver one contiguous [2B len][body]...[0x0000] region to the upper layer
 * WITHOUT any buffer disposition (the caller pools / parks / releases). This
 * is the whole legacy smid_rx_process body minus the pool_put: for a ZCEN2
 * chain it runs once per stride slot, so the Phase-Z pre-classify + claim
 * window per zero-copy slot is byte-for-byte UNCHANGED. Returns 1 if the
 * (slot) buffer was claimed by a zero-copy TX pbuf, 0 otherwise.
 *
 * `buff` is non-cacheable; the ADMA write is already visible. The region is
 * 1..N frames as [2B LE body_len][body]... terminated by a 0x0000 sentinel
 * (a lone frame that fills the region exactly has no sentinel — the bounds
 * check ends the walk). The loop reads a length only at frame boundaries
 * (advancing off by 2+body_len), so body bytes are never mis-parsed. */
static int smid_rx_deliver(void *buff, uint16_t len)
{
    /* DEBUG perf F1: batch the TX schedule kick across this de-agg burst.
     * Each subframe is injected via sd_read_callback -> wifi_eth_tx ->
     * fhost_tx_start (which takes FHOST_TX_LOCK internally, per subframe).
     * fhost_tx_sched_hold() only sets a flag (no lock), so the lock is never
     * held across this loop; fhost_tx_sched_release() then issues one kick per
     * pending AC. Both loop breaks (sentinel / overrun) fall through to the
     * release below, and there is no early return inside the loop, so release
     * is reached on every path once hold has run. */
    extern void fhost_tx_sched_hold(void);
    extern void fhost_tx_sched_release(void);
    uint8_t *p = (uint8_t *)buff;
    uint32_t off = 0;
    uint16_t nfrm = 0;

    /* Phase-Z: pre-classify the buffer for the zero-copy claim window. The
     * claim is only legal for the (single) frame of a zero-copy slot: single
     * iff the first entry is followed by the 0x0000 sentinel or ends the
     * region exactly. State is worker-local (single consumer). */
    smid_zc_claimed = 0;
    smid_zc_cur_arm = NULL;
    smid_zc_cur_single = 0;
    if (smid_zc_is_slot_arm(buff) && len >= 2U) {
        uint16_t flen0 = (uint16_t)(p[0] | (p[1] << 8));
        if (flen0 != 0 && 2U + flen0 <= len) {
            uint32_t nxt = 2U + flen0;
            if (nxt + 2U > len ||
                (uint16_t)(p[nxt] | (p[nxt + 1] << 8)) == 0) {
                smid_zc_cur_arm = p;
                smid_zc_cur_single = 1;
            }
        }
    }

    if (sd_read_callback) {
        ABDBG_TRACE(0x52, len);                 /* L5: inject start (worker) */
        fhost_tx_sched_hold();
        while (off + 2U <= len) {
            uint16_t flen = (uint16_t)(p[off] | (p[off + 1] << 8));
            if (flen == 0) {
                break;                          /* sentinel: no more frames */
            }
            if (off + 2U + flen > len) {
                smid_rx_agg_ovf++;              /* malformed / overrun */
                break;
            }
            rxprof_t0(flen);
            sd_read_callback(NULL, p + off + 2, flen);
            off += 2U + flen;
            smid_rx_agg_frames++;
            nfrm++;
        }
        fhost_tx_sched_release();
        {
            /* [W3a] one WiFi-task kick per de-agg burst for the inject ring */
            extern void sdio_inject_kick(void);
            sdio_inject_kick();
        }
        ABDBG_TRACE(0x53, nfrm);                /* L5: inject done, frames this buf */
    }
    /* Phase-Z: a claimed slot is now owned by the TX pbuf — it returns via
     * smid_zc_slot_free() at cfm time instead of the caller's disposition. */
    if (smid_zc_claimed) {
        smid_zc_claimed = 0;
        smid_zc_cur_arm = NULL;
        smid_zc_cur_single = 0;
        return 1;
    }
    smid_zc_cur_arm = NULL;
    smid_zc_cur_single = 0;
    return 0;
}

/* ZCEN2 chain CPL: walk each filled stride slot through the unchanged
 * per-slot delivery (pre-classify + claim per slot), return the unfilled
 * tail slots of a partial write, release the elem, and top the queue up
 * once at the end. */
static void smid_zc2_process_chain(ipro6_sdio3_sg_desc_t *e, uint16_t len)
{
    uint8_t n = (uint8_t)(((uint32_t)len + e->stride - 1) / e->stride);

    if (n > e->cnt) {
        smid_rx_agg_ovf++;      /* cannot happen: capacity clamped at arm */
        n = e->cnt;
    }
    for (uint8_t k = 0; k < e->cnt; k++) {
        if (k < n) {
            uint32_t sb = (uint32_t)len - (uint32_t)k * e->stride;
            if (sb > e->stride) {
                sb = e->stride;
            }
            uint16_t dlen = (sb > e->slot_cap) ? e->slot_cap : (uint16_t)sb;
            smid_zc2_strides++;
            if (!smid_rx_deliver(e->slot[k], dlen)) {
                smid_rx_buf_dispose(e->slot[k]);
            }
        } else {
            smid_zc2_partial++;
            smid_rx_buf_dispose(e->slot[k]);
        }
    }
    smid_zc2_chains++;
    smid_zc2_elem_put(e);
    smid_rx_refill();
}

static void smid_rx_process(void *buff, uint16_t len)
{
    /* ZCEN2 chain element: per-stride scatter delivery. Identified by elem
     * address, not by mode flags, so a chain armed just before a reseed still
     * walks correctly after the disarm. */
    if (smid_zc2_is_elem(buff)) {
        smid_zc2_process_chain((ipro6_sdio3_sg_desc_t *)buff, len);
        return;
    }
    /* ZCEN2 stride-format write landed on a contiguous OCRAM buffer (an
     * element armed before the enable, or the slot-pressure fallback): the
     * frames sit at fixed stride offsets, so walk each stride region through
     * the same delivery (copy path; claims need a zc slot and refuse here). */
    if (smid_zc2_armed && !smid_zc_is_slot_arm(buff)) {
        for (uint32_t off = 0; off < len; off += SMID_ZC_SLOT_SZ) {
            uint32_t sb = (uint32_t)len - off;
            if (sb > SMID_ZC_SLOT_SZ) {
                sb = SMID_ZC_SLOT_SZ;
            }
            smid_rx_deliver((uint8_t *)buff + off, (uint16_t)sb);
        }
        smid_rx_buf_dispose(buff);
        smid_rx_refill();
        return;
    }
    /* legacy / v2b single-buffer delivery */
    if (!smid_rx_deliver(buff, len)) {
        smid_rx_buf_dispose(buff);
        smid_rx_refill();
    }
}

/* ========================================================================= */
/* SMID driver event callback (IRQ context) — record + wake the worker       */
/* ========================================================================= */

static void smid_evt_push(uint8_t type, void *buff, uint16_t len)
{
    uint8_t in  = smid_evt_in;
    uint8_t nxt = (uint8_t)((in + 1) % SMID_EVT_RING_LEN);

    if (nxt == smid_evt_out) {
        smid_evt_drop++;   /* ring full (the _Static_assert sizes it so this cannot
                            * happen; recover the RX buffer anyway so a future sizing
                            * change can never silently leak it out of the pool). */
        if (type == SMID_EVT_RX && buff) {
            smid_rx_pool_put(buff);
        }
        return;
    }
    smid_evt_ring[in].type = type;
    smid_evt_ring[in].buff = buff;
    smid_evt_ring[in].len  = len;
    smid_evt_in = nxt;
}

static void smid_event_cb(void *arg, int event, void *data)
{
    ipro6_sdio3_trans_desc_t *d = (ipro6_sdio3_trans_desc_t *)data;

    (void)arg;
    if (event >= 0 && event < 13) {
        smid_evt_count[event]++;   /* diag: per-IRQ-event histogram */
    }
    switch (event) {
    case IPRO6_SDIO3_IRQ_EVENT_DNLD_CPL:
        if (d) {
            ABDBG_TRACE(0x58, d->data_len);     /* L5: SDIO arrival (IRQ) */
            /* Hand the filled buffer to the worker, then IMMEDIATELY re-arm a
             * fresh buffer from the pool so the HW queue never drains while the
             * worker is still busy delivering. This decouples the HW queue from
             * the heavy processing -- the fix for the frame-8 blind-burst wedge. */
            smid_evt_push(SMID_EVT_RX, d->buff, d->data_len);
            smid_rx_refill();
        }
        break;
    case IPRO6_SDIO3_IRQ_EVENT_UPLD_CPL:
        smid_evt_push(SMID_EVT_TX, d ? d->buff : NULL, 0);
        break;
    case IPRO6_SDIO3_IRQ_EVENT_ERR_DNLD_ABORT:
    case IPRO6_SDIO3_IRQ_EVENT_ERR_CRC:
        /* dnld error: the driver leaves the buffer at the queue OUT slot (it
         * does NOT advance dnld_out), so it is auto-reused — do not re-arm. */
        smid_evt_push(SMID_EVT_RXERR, NULL, 0);
        break;
    case IPRO6_SDIO3_IRQ_EVENT_SOFT_RESET:
    case IPRO6_SDIO3_IRQ_EVENT_FUNC1_RESET:
        smid_evt_push(SMID_EVT_RESET, NULL, 0);
        break;
    default:
        break;
    }

    if (sdu_irq_handle) {
        sdiowifi_task_notify(sdu_irq_handle, 1);
    }
}

/* ========================================================================= */
/* Reset path                                                                */
/* ========================================================================= */

/* Drop every in-flight upld TX confirm (fire the cb with a done status so the
 * pbuf/buffer is released). The driver just reset the upld queue to empty, so the
 * TRANSFER_COMPLETE events that would normally retire these will never arrive --
 * draining here keeps smid_tx_inflight + the cfm FIFO consistent with the empty
 * driver queue (otherwise inflight leaks until it wedges the upld path). */
static void smid_tx_drain(void)
{
    for (;;) {
        struct smid_tx_cfm done = { 0 };
        bool have = false;

        sdiowifi_enter_critical();
        if (smid_tx_inflight) {
            struct smid_tx_cfm *c = &smid_tx_cfm[smid_tx_cfm_head];
            done = *c;
            c->used = false;
            smid_tx_cfm_head = (smid_tx_cfm_head + 1) & SMID_TX_MASK;
            smid_tx_inflight--;
            have = true;
        }
        sdiowifi_exit_critical(0);
        if (!have) {
            break;
        }
        if (done.cb) {
            done.cb(0, done.cb_arg);
        }
    }
    /* ZCEN3: the OPEN (never-pushed) chain is not in the cfm FIFO; a dead
     * transport orphans it, so reclaim its cbufs here. */
    smid_zc3_drop_open();
}

/* Transport re-sync after a host reset (soft / func1), the trcver-side half of the
 * fix -- the driver ISR (ipro6_sdio3_reset_resync) already stopped the DMA and zeroed
 * BOTH queues (upld + dnld) + the doorbell. Here we (1) drain the now-orphaned upld
 * in-flight confirms so smid_tx_inflight stays consistent with the emptied queue, and
 * (2) reseed the dnld RX pool -- reclaim every buffer and re-arm from the zeroed queue,
 * restoring the exact post-handshake pool/queue state (so a reset can never leak or
 * strand a partially consumed buffer). We still deliberately do NOT set_present(false)
 * / re-init the MAC (cf. smid_software_reset): a host reset is part of NORMAL
 * enumeration (the host func-resets during enable), and tearing the transceiver down
 * there blocks upld (host_present=false, no re-hello) and drops the in-flight GET_MAC
 * response, breaking the handshake. The reseed only re-arms dnld buffers (no
 * present/MAC), and runs in the worker after all earlier RX events have been delivered
 * (the worker drains the event ring in order), so no in-flight buffer is re-armed while
 * still in use. */
static void smid_transport_resync(void)
{
    smid_tx_drain();
    smid_rx_pool_reseed();
}

static void smid_software_reset(void)
{
    sdiowifi_mgmr_t *sdm = &g_sdiowifi_mgmr;

    HR_LOGI("smid_software_reset\r\n");
    if (!sdm->init) {
        HR_LOGW("%s(%d): sdm is not initialized\r\n", __func__, __LINE__);
        return;
    }

    ipro_net_wifi_trcver_set_present(&sdm->trcver, false);

    /* flush in-flight TX confirms (drop) */
    smid_tx_drain();

    ipro_net_wifi_trcver_reinit(&sdm->trcver);
    sdiowifi_tx_buf_update_credit(&sdm->tx_desc);
    sdu_software_reset = false;
}

/* ========================================================================= */
/* keep-ready: re-assert Card-Init-Done after the silent host IO-reset       */
/* ========================================================================= */

static void smid_keep_ready_task(void *arg)
{
    ipro6_smid_dev_t *dev = sdio_smid_dev();
    /* MAC AC-watchdog backstop (txl_cntrl.c) — this task is the one
     * context guaranteed alive during the terminal TX wedge (all tx
     * pbufs pinned -> no pushes -> no timer_move -> dead watchdog),
     * so it periodically re-arms any dead-but-busy AC timer to keep
     * the recover_stale/PM45 chain schedulable. */
    extern void txl_watchdog_backstop_poll(void);
    uint32_t backstop_div = 0;

    /* ZCEN2 DAT0 stall bound: consecutive 1ms ticks with PROGRAM_DONE still
     * withheld. This task is the right watchdog context — top prio and the
     * one guaranteed alive while the data path is wedged. */
    uint32_t stall_ticks = 0;

    (void)arg;
    for (;;) {
        ipro6_sdio3_keep_ready(dev);
        if (++backstop_div >= 500U) {      /* every ~500 ms */
            backstop_div = 0;
            txl_watchdog_backstop_poll();
        }
        if (smid_zc2_armed && !g_sdio_zc2_ctg_fb &&
            ipro6_sdio3_dnld_pgmdone_deferred) {
            if (++stall_ticks >= g_sdio_zc2_stall_ms) {
                stall_ticks = 0;
                smid_zc2_relief_trips++;
                smid_zc2_relief = 1;
                /* refill: chain-first as always; with relief set the
                 * contiguous-OCRAM branch may arm, whose dnld_push releases
                 * the withheld PROGRAM_DONE and unblocks the host CMD53 */
                smid_rx_refill();
            }
        } else {
            stall_ticks = 0;
        }
        sdiowifi_delay_ms(SMID_KEEP_READY_MS);
    }
}

/* ========================================================================= */
/* Worker task                                                               */
/* ========================================================================= */

static void smid_worker_task(void *arg)
{
    (void)arg;
    sdu_irq_handle = sdiowifi_get_task_handle();

    for (;;) {
        sdiowifi_task_wait_notification(SDIOWIFI_WAIT_FOREVER);

        /* drain SMID completion events */
        while (smid_evt_out != smid_evt_in) {
            struct smid_evt e = smid_evt_ring[smid_evt_out];
            smid_evt_out = (uint8_t)((smid_evt_out + 1) % SMID_EVT_RING_LEN);

            switch (e.type) {
            case SMID_EVT_RX:
                smid_rx_process(e.buff, e.len);
                break;
            case SMID_EVT_TX:
                smid_tx_confirm_one(e.buff);
                /* ZCEN3: a queue slot just freed -- flush the open chain */
                smid_zc3_cfm_flush();
                break;
            case SMID_EVT_RXERR:
                smid_rx_err_cnt++;
                break;
            case SMID_EVT_RESET:
                smid_reset_cnt++;
                /* A host reset (soft / func1) desyncs the upld queue unless we
                 * re-sync. The driver ISR already reset the upld queue + doorbell
                 * (ipro6_sdio3_reset_resync); reconcile the trcver's upld TX
                 * bookkeeping here so it does not leak against the emptied queue.
                 * (Lightweight: does NOT reset present/MAC/dnld -- see the note in
                 * smid_transport_resync.) */
                smid_transport_resync();
                break;
            default:
                break;
            }
        }

        if (sdu_worker_ev & SDU_WORKER_EV_RESET) {
            sdu_worker_ev &= ~SDU_WORKER_EV_RESET;
            HR_LOGI("SDU_WORKER_EV_RESET\r\n");
            smid_software_reset();
        }
        if (sdu_worker_ev & SDU_WORKER_EV_TX_WDT) {
            sdu_worker_ev &= ~SDU_WORKER_EV_TX_WDT;
            smid_tx_check_timeout();
            /* ZCEN3: timed-out cfms freed queue slots -- flush the open chain
             * (harmless if the transport is truly dead; drain reclaims) */
            smid_zc3_cfm_flush();
        }
        if (smid_tx_inflight == 0) {
            ipro_sdio_tx_timer_stop();
        }
    }
}

/* ========================================================================= */
/* Init / handshake                                                          */
/* ========================================================================= */

int ipro_sdu_init(void)
{
    static ipro6_sdio3_config_s cfg = {
        .func_num            = 1,                       /* WiFi-only */
        .func1_if_code       = IPRO6_SDIO3_STD_IF_WLAN, /* host reads class=7 */
        .cap_flag            = 0,                       /* HS-only: keep SHS (-84 fix) */
        .ocr                 = 0x00FF8000,              /* 2.7-3.6V window */
        .func1_dnld_size_max = SMID_DNLD_BUF_SIZE,
        .func2_dnld_size_max = 0,
    };
    ipro6_smid_dev_t *dev = sdio_smid_dev();

    HR_LOGI("ipro_sdu_init (SMID)\r\n");

    /* HW bring-up: clock un-gate + pinmux (GPIO10-15 AF SDIO), controller reset */
    sdio_gpio_init();
    sdio_glb_reset();
    for (volatile int i = 0; i < 10000; i++) {
        __asm__ volatile("nop");
    }

    /* register the completion callback BEFORE ipro6_sdio3_init() enables the IRQ */
    ipro6_sdio3_irq_attach(dev, smid_event_cb, NULL);
    ipro6_sdio3_init(dev, &cfg);

    /* worker + TX watchdog + keep-ready poller */
    sdiowifi_task_create(smid_worker_task, "smid_worker", 0, 512, NULL, SMID_WORKER_PRIO, &sdu_irq_handle);
    sdio_tx_wdt_timer = sdiowifi_timer_create(sdio_tx_timer_cb, NULL, SDIO_TX_WDT_TIMEOUT, 1, 0);
    sdiowifi_task_create(smid_keep_ready_task, "smid_keepready", 0, 256, NULL, SMID_KEEP_READY_PRIO, &smid_keep_ready_handle);

    return 0;
}

int32_t ipro_sdio_handshake(void)
{
    ipro6_smid_dev_t *dev = sdio_smid_dev();

    /* mark the device ready: Card-Init-Done + queue ctrl READY + func ready */
    ipro6_sdio3_feature_control(dev, IPRO6_SDIO3_CMD_INIT_READY, SMID_FUNC_WIFI);
    ipro6_sdio3_feature_control(dev, IPRO6_SDIO3_CMD_SET_FUNC_CARD_READY, SMID_FUNC_WIFI);

    /* seed the dnld buffer pool, then arm the HW queue from it (fills the 8 queue
     * slots and leaves the remainder as the ISR re-arm cushion) so the host can
     * download immediately. */
    smid_rx_pool_reseed();

    /* seed the real-WiFi upld copy pool */
    smid_upld_cbuf_init();

#if defined(CONFIG_SMID_RAW_TX_TEST)
    /* seed the upld (raw test sender) buffer pool */
    smid_tx_pool_init();
#endif
    return 0;
}

int ipro_sdio_read_cb_register(void *env, ipro_sdio_read_cb_t cb, void *cb_arg)
{
    (void)env; (void)cb_arg;
    sd_read_callback = cb;
    return 0;
}

void trcver_sdio_stats_display(void)
{
    ipro6_smid_dev_t *dev = sdio_smid_dev();

    HR_LOGI("smid upld_wait:%d dnld_wait:%d pool:%u/%u txfly:%u rxerr:%lu rst:%lu drop:%lu\r\n",
            ipro6_sdio3_feature_control(dev, IPRO6_SDIO3_CMD_GET_UPLD_QUEUE_WAIT_NUM, SMID_FUNC_WIFI),
            ipro6_sdio3_feature_control(dev, IPRO6_SDIO3_CMD_GET_DNLD_QUEUE_WAIT_NUM, SMID_FUNC_WIFI),
            (unsigned)smid_rx_pool_top, (unsigned)SMID_RX_POOL_TOTAL,
            (unsigned)smid_tx_inflight,
            (unsigned long)smid_rx_err_cnt,
            (unsigned long)smid_reset_cnt,
            (unsigned long)smid_evt_drop);
    /* Phase-Z v1 zero-copy accounting (gate g_sdio_zerocopy). */
    HR_LOGI("smid zc armed:%u gate:%lu claims:%lu frees:%lu inflight:%lu rsv_skip:%lu\r\n",
            (unsigned)smid_zc_armed,
            (unsigned long)g_sdio_zerocopy,
            (unsigned long)smid_zc_claims, (unsigned long)smid_zc_frees,
            (unsigned long)(smid_zc_claims - smid_zc_frees),
            (unsigned long)smid_zc_reserve_skips);
    /* ZCEN2 stride accounting (gate g_sdio_zc2_en; armed by host "ZCEN2"). */
    HR_LOGI("smid zc2 armed:%u nslot:%u chains:%lu strides:%lu partial:%lu ctg:%lu starve:%lu elems:%u/%u relief:%u trips:%lu\r\n",
            (unsigned)smid_zc2_armed, (unsigned)smid_zc2_nslot_armed,
            (unsigned long)smid_zc2_chains, (unsigned long)smid_zc2_strides,
            (unsigned long)smid_zc2_partial, (unsigned long)smid_zc2_ctg,
            (unsigned long)smid_zc2_arm_starve,
            (unsigned)smid_zc2_elem_top, (unsigned)SMID_ZC2_ELEM_CNT,
            (unsigned)smid_zc2_relief, (unsigned long)smid_zc2_relief_trips);
    /* per-IRQ-event histogram: dnldCPL upldCPL softRST fn1RST 1v8 crc adma dnldABRT
     * upldABRT dnldQempty upldQempty unknown (indices match IPRO6_SDIO3_IRQ_EVENT_*) */
    HR_LOGI("smid evt dnldCPL:%lu upldCPL:%lu sRST:%lu f1RST:%lu crc:%lu adma:%lu "
            "dABRT:%lu uABRT:%lu dQemp:%lu uQemp:%lu unk:%lu\r\n",
            (unsigned long)smid_evt_count[0],  (unsigned long)smid_evt_count[1],
            (unsigned long)smid_evt_count[2],  (unsigned long)smid_evt_count[3],
            (unsigned long)smid_evt_count[6],  (unsigned long)smid_evt_count[7],
            (unsigned long)smid_evt_count[8],  (unsigned long)smid_evt_count[9],
            (unsigned long)smid_evt_count[10], (unsigned long)smid_evt_count[11],
            (unsigned long)smid_evt_count[12]);
    /* dnld length = Argument(count) * Command(block size); len_fallback counts
     * any out-of-range decode that was clamped to the buffer size (should be 0). */
    HR_LOGI("smid dnld_len fallback:%lu backpressure:%lu agg_frames:%lu agg_ovf:%lu\r\n",
            (unsigned long)ipro6_sdio3_dnld_len_fallback,
            (unsigned long)ipro6_sdio3_dnld_backpressure,
            (unsigned long)smid_rx_agg_frames,
            (unsigned long)smid_rx_agg_ovf);
    /* upld copy pool: drops indicate the frame exceeded the copy buffer (oversize)
     * or the pool was drained because the host read slower than the RX rate (nobuf). */
    HR_LOGI("smid upldcopy free:%u/%u oversize:%lu nobuf:%lu\r\n",
            (unsigned)smid_upld_cbuf_top, (unsigned)SMID_UPLD_COPY_CNT,
            (unsigned long)smid_upld_oversize,
            (unsigned long)smid_upld_nobuf);
    /* ZCEN3 upld aggregation (gate g_sdio_zc3_en; wire armed by host "ZCEN3").
     * chain ratio under flood ~= frames/chains; defer = queue-full deferrals
     * (retried, not lost); open = frames parked on the open chain right now. */
    HR_LOGI("smid zc3 wire:%u uslots:%u chains:%lu frames:%lu single:%lu "
            "defer:%lu flush:%lu open:%u elems:%u/%u\r\n",
            (unsigned)smid_zc3_wire, (unsigned)smid_zc3_uslots_armed,
            (unsigned long)smid_zc3_chains, (unsigned long)smid_zc3_frames,
            (unsigned long)smid_zc3_single, (unsigned long)smid_zc3_defer,
            (unsigned long)smid_zc3_flush, (unsigned)smid_zc3_open_cnt,
            (unsigned)smid_zc3_elem_top, (unsigned)SMID_ZC3_ELEM_CNT);
    /* DL profiler window (self-clears on every dump) */
    {
        extern void dlprof_dump(void);
        dlprof_dump();
    }
}

/* ========================================================================= */
/* Message build + send (chip-agnostic; mirrors the legacy trcver_sdio.c)    */
/* ========================================================================= */

#define LSB(x)  ((x) & 0xff)
#define SLSB(x) (((x) >> 8) & 0xff)
#define OFFSET_OF(mem, st) ((uintptr_t)&(((st *)0)->mem))

static void sd_cmd_cfm_cb(int idx, void *arg)
{
    (void)idx;
    vPortFree(arg);
}

static void *sd_cmd_buf_alloc(void)
{
    /* The D-cache clean happens in sdu_send_data() before the ADMA read, so a
     * plain heap buffer is fine here. */
    return pvPortMalloc(SDIO_TX_BUF_SIZE);
}

int ipro_sdio_write_cmd(void *env, uint16_t type, uint16_t subtype,
                      const void *headroom, uint16_t headroom_len,
                      const void *tailroom, uint16_t tailroom_len)
{
    const uint8_t *p1 = (const uint8_t *)headroom;
    size_t p1_len = (size_t)headroom_len;
    const uint8_t *p2 = (const uint8_t *)tailroom;
    size_t p2_len = (size_t)tailroom_len;

    int ret;
    struct sdio_top_msg *msg;
    void *base;
    void *p_buf;
    size_t buf_size = SDIO_TX_BUF_SIZE;
    size_t len = p1_len + p2_len;
    /* ZCEN3 wire: reserve 2B headroom for the [2B len] prefix that
     * sdu_send_data() writes at msg-2. Stable under the caller's tx_lock
     * (smid_zc3_wire_arm takes it), so the layout and the send agree. */
    size_t wire_hr = smid_zc3_wire ? 2u : 0u;

    (void)env;

    if (wire_hr + sizeof(struct sdio_top_msg) + len > SDIO_TX_BUF_SIZE) {
        return -1;
    }

    base = sd_cmd_buf_alloc();
    if (!base) {
        return -1;
    }

    msg = (struct sdio_top_msg *)((uint8_t *)base + wire_hr);
    buf_size -= wire_hr;
    p_buf = msg;
    msg->type_lsb = LSB(type);
    msg->type_msb = SLSB(type);
    msg->len_lsb = LSB(len);
    msg->len_msb = SLSB(len);
    msg->pld_off = OFFSET_OF(pld_w_pad, struct sdio_top_msg);
    msg->subtype_lsb = LSB(subtype);
    msg->subtype_msb = SLSB(subtype);
    msg->is_amsdu = false;
    msg->has_twin = 0;

    p_buf = (uint8_t *)p_buf + msg->pld_off;
    buf_size -= msg->pld_off;

    MEMCPY_SAFE((uint8_t *)p_buf, buf_size, p1, p1_len);
    if (p2) {
        p_buf = (uint8_t *)p_buf + p1_len;
        buf_size -= p1_len;
        MEMCPY_SAFE((uint8_t *)p_buf, buf_size, p2, p2_len);
    }

    /* free arg is the MALLOC base, not msg -- they differ by the wire headroom */
    ret = sdu_send_data(msg, len, true, sd_cmd_cfm_cb, base);
    if (ret) {
        sd_cmd_cfm_cb(-1, base);
    }
    return ret;
}

/* DEBUG DL profiler (07-24): the downlink wall measures ~1ms/frame effective
 * (12-14M on BOTH APs, air exonerated by sniffer) while the survey modeled
 * 250-450us. Histogram the write_pbuf ARRIVAL GAP (upstream pacing: air/MAC/
 * rxu/fhost) vs the IN-FUNCTION DURATION (trcver/SDIO side) to attribute the
 * missing time. machw time is us. Gate g_dlprof_en; print via sdio_stats. */
volatile uint8_t g_dlprof_en = 1;
static uint32_t dlprof_last_us;
static uint32_t dlprof_cnt;
static uint32_t dlprof_gap_h[5];   /* <100 <300 <700 <1500 >=1500 us */
static uint32_t dlprof_dur_h[5];
static uint32_t dlprof_gap_max, dlprof_dur_max;
static uint64_t dlprof_gap_sum, dlprof_dur_sum;

static inline void dlprof_bucket(uint32_t *h, uint32_t us)
{
    if (us < 100)       h[0]++;
    else if (us < 300)  h[1]++;
    else if (us < 700)  h[2]++;
    else if (us < 1500) h[3]++;
    else                h[4]++;
}

void dlprof_dump(void)
{
    HR_LOGI("dlprof n:%lu gap avg:%lu max:%lu h:%lu/%lu/%lu/%lu/%lu "
            "dur avg:%lu max:%lu h:%lu/%lu/%lu/%lu/%lu\r\n",
            (unsigned long)dlprof_cnt,
            (unsigned long)(dlprof_cnt ? dlprof_gap_sum / dlprof_cnt : 0),
            (unsigned long)dlprof_gap_max,
            (unsigned long)dlprof_gap_h[0], (unsigned long)dlprof_gap_h[1],
            (unsigned long)dlprof_gap_h[2], (unsigned long)dlprof_gap_h[3],
            (unsigned long)dlprof_gap_h[4],
            (unsigned long)(dlprof_cnt ? dlprof_dur_sum / dlprof_cnt : 0),
            (unsigned long)dlprof_dur_max,
            (unsigned long)dlprof_dur_h[0], (unsigned long)dlprof_dur_h[1],
            (unsigned long)dlprof_dur_h[2], (unsigned long)dlprof_dur_h[3],
            (unsigned long)dlprof_dur_h[4]);
    dlprof_cnt = 0;
    dlprof_gap_sum = dlprof_dur_sum = 0;
    dlprof_gap_max = dlprof_dur_max = 0;
    memset((void *)dlprof_gap_h, 0, sizeof(dlprof_gap_h));
    memset((void *)dlprof_dur_h, 0, sizeof(dlprof_dur_h));
}

int ipro_sdio_write_pbuf(void *env, uint16_t type, uint16_t subtype,
                       struct pbuf *p, bool is_amsdu, void *cb, void *cb_arg)
{
    extern uint32_t inline_hal_machw_time(void);
    uint32_t prof_t0 = 0;
    struct sdio_top_msg *msg;
    uint16_t data_len = p->tot_len;

    if (g_dlprof_en) {
        prof_t0 = inline_hal_machw_time();
        if (dlprof_cnt) {
            uint32_t gap = prof_t0 - dlprof_last_us;
            dlprof_bucket(dlprof_gap_h, gap);
            dlprof_gap_sum += gap;
            if (gap > dlprof_gap_max) dlprof_gap_max = gap;
        }
        dlprof_last_us = prof_t0;
        dlprof_cnt++;
    }
    /* ZCEN3 wire: [2B len][msg][payload]. pld_off grows 12 -> 14 so the
     * payload copy stays 4-byte aligned (prefix 2 + hdr 12 + slack 2 ->
     * payload at cbuf+16); the host locates it via msg->pld_off as always.
     * Stable under the caller's tx_lock (smid_zc3_wire_arm takes it). */
    uint8_t  wire     = smid_zc3_wire;
    uint8_t  pld_off  = (uint8_t)(sizeof(struct sdio_top_msg) + (wire ? 2 : 0));
    uint16_t total    = (uint16_t)(pld_off + data_len);
    uint16_t wire_len = (uint16_t)(total + (wire ? 2 : 0));
    uint8_t *buf;

    (void)env;

    if (p->next) {
        /* single-pbuf only (whole A-MSDU is one contiguous pbuf here) */
        HR_LOGE("A-AMSDU with two pbds not supported\r\n");
        return IPRO_MSG_ERR_TIMEOUT;
    }
    if (wire_len > SMID_UPLD_COPY_SIZE) {
        smid_upld_oversize++;   /* too big for a copy buffer (aggregation TODO) */
        return IPRO_MSG_ERR_TIMEOUT;
    }

    /* COPY into a device-owned stable buffer so the transient RX-ring pbuf is not
     * held across the host read (the zero-copy version let the RX DMA overwrite a
     * queued upld -> host read garbage -> flap). */
    buf = (uint8_t *)smid_upld_cbuf_get();
    if (!buf) {
        smid_upld_nobuf++;      /* pool drained (host reading slower than RX) -> drop */
        return IPRO_MSG_ERR_TIMEOUT;
    }

    if (wire) {
        /* [2B LE len] = hdr + payload bytes (excludes the prefix itself) */
        buf[0] = (uint8_t)(total & 0xff);
        buf[1] = (uint8_t)(total >> 8);
        msg = (struct sdio_top_msg *)(buf + 2);
    } else {
        msg = (struct sdio_top_msg *)buf;
    }
    msg->type_lsb = LSB(type);
    msg->type_msb = SLSB(type);
    msg->len_lsb  = LSB(data_len);
    msg->len_msb  = SLSB(data_len);
    msg->pld_off  = pld_off;
    msg->is_amsdu = is_amsdu;
    msg->has_twin = 0;
    msg->subtype_lsb = LSB(subtype);
    msg->subtype_msb = SLSB(subtype);
    msg->first_part_len_lsb = LSB(p->len);
    msg->first_part_len_msb = SLSB(p->len);
    /* [PERF gate 07-24] MEMCPY_SAFE resolves to the libc byte-loop memcpy
     * (~160us/1.5KB from the uncached fhost_rx_buf). arch_memcpy_fast is the
     * word-wise copy already proven on the uplink inject path. Poke
     * g_upld_fastcpy=1 to enable (default off for A/B). */
    if (g_upld_fastcpy) {
        extern void *arch_memcpy_fast(void *dst, const void *src, uint32_t n);
        arch_memcpy_fast((uint8_t *)msg + pld_off, p->payload, data_len);
    } else {
        MEMCPY_SAFE((uint8_t *)msg + pld_off,
                    SMID_UPLD_COPY_SIZE - wire_len + data_len,
                    p->payload, data_len);
    }

    /* block-align the transfer length so the host reads pure block-mode (see the
     * SMID_UPLD_BLOCK note); zero the pad so no stale frame bytes leak to the host.
     * (A chained intermediate stride's tail past this pad is NOT zeroed -- its
     * stale content is upld frames this same host already received.) */
    uint16_t xfer = (uint16_t)((wire_len + (SMID_UPLD_BLOCK - 1)) & ~(SMID_UPLD_BLOCK - 1));
    if (xfer > SMID_UPLD_COPY_SIZE) {
        xfer = SMID_UPLD_COPY_SIZE;     /* wire_len <= COPY_SIZE, so this never truncates below it */
    }
    if (xfer > wire_len) {
        memset(buf + wire_len, 0, xfer - wire_len);
    }

    /* cached pool -> clean the prefix + header + payload + pad for the ADMA read */
    L1C_DCache_Clean_By_Addr((uintptr_t)buf, xfer);

    if (wire) {
        /* opportunistic chain: push now while the queue is idle, batch under
         * host-read backlog (see the ZCEN3 block comment) */
        if (smid_zc3_accept(buf, xfer) != 0) {
            smid_upld_cbuf_put(buf);
            smid_upld_nobuf++;  /* queue saturated: same drop semantics as pool-dry */
            return IPRO_MSG_ERR_TIMEOUT;
        }
    } else if (smid_upld(buf, xfer, 0, smid_upld_cbuf_free_cb, buf) != 0) {
        smid_upld_cbuf_put(buf);
        return IPRO_MSG_ERR_TIMEOUT;
    }

    /* The frame is copied; release the source pbuf NOW so the RX ring can reuse it
     * (do NOT defer to the host read). Success -> the caller must not free it. */
    if (cb) {
        ((sdio_tx_cfm_cb_t)cb)(0, cb_arg);
    }
    if (g_dlprof_en) {
        uint32_t dur = inline_hal_machw_time() - prof_t0;
        dlprof_bucket(dlprof_dur_h, dur);
        dlprof_dur_sum += dur;
        if (dur > dlprof_dur_max) dlprof_dur_max = dur;
    }
    return 0;
}
