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

#include <lwip/pbuf.h>
#include <lwip/netifapi.h>
#include <sdiowifi_platform_adapt.h>

//#include <bl_wifi.h>
#include <wifi_mgmr_ext.h>
#include <net_pkt_filter.h>

#include <wifi_pkt_hooks.h>
#include <sdiowifi_mgmr.h>
#include <sdiowifi_config.h>
#include <trcver_sdio.h>
#include <net_wifi_transceiver.h>
#include <sdio_msg_ext_frame.h>
#include "ipro_dual_stack_input.h"
#include "sdiowifi_mgmr_type.h"

#include "sdio_port.h"
#include "rx_profile.h"
#include "misc.h"
#include "abtrace_dbg.h"    /* DEBUG: AB_TRACE timeline events */
#include "sdio_dma_copy.h"  /* DEBUG: gated DMA / cached-read inject-copy modes */

typedef void (*ipro_custom_tx_callback_t)(void *cb_arg, bool tx_ok);

struct wifi_custom_tx_cfm {
    ipro_custom_tx_callback_t cb;
    void *cb_arg;
};
int wifi_eth_tx(struct pbuf *p, bool is_sta, struct wifi_custom_tx_cfm *custom_cfm);

/* 2026-05-14 TP: count pre-fhost drops from empty wifi-TX pbuf pool. */
volatile uint32_t sdio_wifi_tx_pool_drops = 0;

/* 2026-07-24 T4 rework: cheap early-bail at pool exhaustion, MECHANISM not
 * knob. At overload the prio-31 worker burns the full alloc ceremony
 * (critical section + pbuf setup) per DOOMED frame, which subtracts from
 * served throughput (offer curve falls past 15M offered). The original gate
 * read free_size lock-free, whose transient-zero FALSE POSITIVES randomly
 * dropped admittable frames — invisible to UDP, but TCP reads every drop as
 * congestion: cwnd collapse + RTO stalls (bench 07-24: TCP TX 2.1M vs 10.4M).
 * Now the bail keys on tx_desc.pool_empty, set only by a genuinely failed
 * alloc under the pool lock and cleared on the first buffer return — cheap
 * intake read, zero false positives, correct for both protocols. */
volatile uint32_t sdio_earlydrop_cnt = 0;

/* DEBUG 2026-07-23 ZC-probe: can the MAC HW (and macsw CPU paths) run a TX
 * entirely out of uncached OCRAM (0x2102xxxx alias, where smid_rx_buf lives)?
 * Gate g_sdio_zcprobe reroutes every host frame through a per-pool-slot
 * staging area that mirrors the WRAM slot layout exactly (48B lwip headroom +
 * payload). The fhost desc/THD/PBDs then get built INSIDE the staging slot,
 * so the MAC HW fetches THD + PBD + payload from OCRAM — the full zero-copy
 * end state. Discriminator vs baseline: served / ok% / HW_ERR. The staging
 * slot index is tied to the pool slot, so lifetime handling is inherited.
 * net_al.c accepts the range via g_zcprobe_lo/hi (registered on first use). */
/* 2026-07-24 PROBE BUG FIX (user caught it): the first probe hardcoded 48 for
 * the headroom, but the wifi6_lwip_adapter lwipopts.h (which governs this
 * whole pbuf path — proven by sizeof(sdio_txbuf_payload_t)=1968=452+1514+pad)
 * sets PBUF_LINK_ENCAPSULATION_HLEN=452 (Phase 10G/H): the 376B fhost desc +
 * 802.11 headers live in the 452B before the frame. With only 48B of staging
 * headroom, fhost built the desc/THD ~404B BEFORE the slot -> corrupt THD ->
 * the AC_TMO exchange hang. That hang therefore proved NOTHING about MAC HW
 * reading OCRAM. Use the real macro so the staging mirrors the pool exactly. */
#define ZC_STAGE_HR   PBUF_LINK_ENCAPSULATION_HLEN     /* 452, from lwipopts */
#define ZC_STAGE_SZ   1984u                /* 452 + 1514 + pad, 32B-aligned   */
/* 8 slots (~15.5KB nocache): full-pool 24 slots would breach the ld's
 * __min_heap_size floor (link error now, not a boot wedge). Pool rotates
 * FIFO, so slots 0-7 still see ~1/3 of frames — enough to discriminate. */
#define ZC_STAGE_CNT  8u
/* [PERF 07-24] zcprobe staging RETIRED from the build: the probe answered its
 * question on 07-23 (staging-THD artifact; real zero-copy landed as v1/v2)
 * and its 15.5KB nocache array now funds hot-code-to-RAM round 3. Flip
 * SDIO_ZCPROBE_RETAIN to 1 to bring the diagnostic back. */
#define SDIO_ZCPROBE_RETAIN 0
#if SDIO_ZCPROBE_RETAIN
static __attribute__((section(".noncacheable"), aligned(32)))
    uint8_t zc_stage[ZC_STAGE_CNT][ZC_STAGE_SZ];
#endif
volatile uint32_t g_sdio_zcprobe = 0;
volatile uint32_t zc_probe_cnt = 0;
volatile uint32_t zc_probe_noslot = 0;
extern volatile uint32_t g_zcprobe_lo, g_zcprobe_hi;    /* net_al.c bypass */

/* DEBUG 2026-07-24 Phase-Z v1 zero-copy TX inject (gate g_sdio_zerocopy in
 * trcver_sdio_ipro6.c). The SMID dnld slot itself becomes the TX pbuf: the
 * frame already sits at slot+452 (= PBUF_LINK_ENCAPSULATION_HLEN, arm-offset
 * trick), so pbuf_alloced_custom() over the slot base lands payload exactly
 * on the received frame — no pool alloc, no memcpy, ram_wifi/SHRAM native.
 * Slot ownership: claim (worker) -> fhost/TX -> cfm -> zc_slot_free (WiFi
 * task) -> re-armed on the dnld queue. One wrapper per slot, indexed by the
 * trcver slot index, so a wrapper is never reused while its slot is in
 * flight. */
typedef struct {
    struct pbuf_custom pbuf;
    void *slot;
} zc_pbuf_t;
#define ZC_SLOT_CNT          32             /* == SMID_ZC_SLOT_CNT (trcver), P1 07-29 */
#define ZC_SLOT_PAYLOAD_LEN  (PBUF_LINK_ENCAPSULATION_HLEN + 1514)
static zc_pbuf_t zc_pbufs[ZC_SLOT_CNT];
volatile uint32_t sdio_zc_wraps = 0;
volatile uint32_t sdio_zc_wrap_fail = 0;

extern void *smid_zc_try_claim(const void *frame, uint16_t frame_len);
extern void  smid_zc_slot_free(void *slot_base);
extern int   smid_zc_slot_index(const void *slot_base);

static void zc_buf_free(struct pbuf *p)
{
    zc_pbuf_t *zp = (zc_pbuf_t *)p;
    smid_zc_slot_free(zp->slot);
}

/* Bug B dive (PM13 2026-05-29): SDIO upload(ACK) starvation instrumentation.
 * upld_attempt = WiFi-RX frames (incl. TCP ACKs) handed to the host;
 * upld_fail    = those dropped because the upload (sdu_send_data) failed —
 *                this is the ACK-loss site that collapses host TCP TX. */
volatile uint32_t sdio_upld_attempt = 0;
volatile uint32_t sdio_upld_fail    = 0;

/* 2026-06-06: unknown-type host messages (observed type=768/0x0300 at ~5-10/s)
 * were HR_LOGE'd from the SDIO RX hot path — each ~150-char line over the
 * 115200 console is a ~13 ms stall that shows up as ms-class lease->fhost
 * dwell spikes (tx_diag K-histogram max=80ms). Count instead; hexdump the
 * first occurrence once for protocol forensics. Counters shown in tx_diag. */
volatile uint32_t sdio_recv_unknown_cnt = 0;
volatile uint16_t sdio_recv_unknown_last_type = 0;

/* [W3a gate 07-24] inject-on-WiFi-task: instead of the prio-31 worker running
 * the whole fhost inject under FHOST_TX_LOCK (contending with the WiFi task's
 * cfm/push and causing the historical lock convoy), the worker only queues
 * the ready pbuf into a SPSC ring; macif_tx_evt (WiFi task) drains it just
 * before its queue_tx loop, so claim/copy (worker) pipelines with
 * inject+push+cfm (WiFi task) and the lock is uncontended by construction.
 * Poke g_sdio_inject_wifitask=1 to enable (default off for A/B). */
#define INJ_RING_SZ 64u               /* power of 2, > total pool (41: 9 + 32) */
static struct { struct pbuf *p; uint8_t is_sta; } inj_ring[INJ_RING_SZ];
static volatile uint32_t inj_head;    /* producer: smid_worker */
static volatile uint32_t inj_tail;    /* consumer: WiFi task   */
volatile uint8_t g_sdio_inject_wifitask = 0;
volatile uint32_t inj_ring_full = 0, inj_pushed = 0, inj_drained = 0;

void macif_tx_inject_drain(void)      /* strong def; weak stub in macif_fhost */
{
    int budget = 16;                  /* bound WiFi-task occupancy per event so
                                       * RX/high-prio events are not starved */
    while (inj_tail != inj_head && budget--) {
        uint32_t t = inj_tail;
        struct pbuf *p = inj_ring[t & (INJ_RING_SZ - 1)].p;
        bool is_sta = inj_ring[t & (INJ_RING_SZ - 1)].is_sta;
        inj_tail = t + 1;
        wifi_eth_tx(p, is_sta, &(struct wifi_custom_tx_cfm){NULL, NULL});
        /* wifi_eth_tx takes its own reference; drop the intake reference
         * exactly like the direct path does. Missing this leaked +1 ref per
         * frame -> cfm's pbuf_free never reached the custom-free -> zc slots
         * never re-armed -> pool wedge at 23 in-flight (bench 07-24). */
        pbuf_free(p);
        inj_drained++;
    }
    if (inj_tail != inj_head) {
        extern void macif_tx_data_ind(int queue_idx);
        macif_tx_data_ind(0);         /* remainder: re-arm the event */
    }
}

void sdio_inject_kick(void)           /* called at de-agg burst end (trcver) */
{
    if (inj_tail != inj_head) {
        extern void macif_tx_data_ind(int queue_idx);
        macif_tx_data_ind(0);
    }
}

static void handle_eth_frame_to_wifi_tx(sdiowifi_mgmr_t *sdm, struct pbuf *p, bool is_sta)
{
    if (g_sdio_inject_wifitask) {
        uint32_t h = inj_head;
        if ((h - inj_tail) < INJ_RING_SZ) {
            inj_ring[h & (INJ_RING_SZ - 1)].p = p;
            inj_ring[h & (INJ_RING_SZ - 1)].is_sta = (uint8_t)is_sta;
            inj_head = h + 1;
            inj_pushed++;
            return;                   /* kick fires at burst end */
        }
        inj_ring_full++;              /* full: fall through to direct path */
    }
    wifi_eth_tx(p, is_sta, &(struct wifi_custom_tx_cfm){NULL, NULL});
    rxprof_t5();
    pbuf_free(p);
}

static void handle_sta_ap_to_wifi_tx(sdiowifi_mgmr_t *sdm, const uint8_t *frame, uint16_t frame_len, bool is_sta)
{
    extern void trcver_sdio_stats_display(void);
    extern void sdiowifi_txbuf_stats_display(void);

    struct pbuf *p;

    if (frame == NULL || frame_len > 2048) {
        HR_LOGE("frame or frame_len error\r\n");
        return;
    }

    /* DEBUG 2026-07-24 Phase-Z v1: zero-copy fast path — wrap the SMID dnld
     * slot directly as the TX pbuf (see block comment at zc_pbufs). Claim
     * fails (NULL) when the gate is off, the frame is not a zero-copy slot's
     * single frame, or the dnld pool needs its re-arm reserve — all of which
     * fall through to the legacy copy path below. */
    {
        void *zslot = smid_zc_try_claim(frame, frame_len);
        if (zslot) {
            int zidx = smid_zc_slot_index(zslot);
            if (zidx >= 0 && zidx < ZC_SLOT_CNT) {
                zc_pbuf_t *zp = &zc_pbufs[zidx];
                zp->slot = zslot;
                zp->pbuf.custom_free_function = zc_buf_free;
                p = pbuf_alloced_custom(PBUF_RAW_TX, frame_len,
                        (PBUF_ALLOC_FLAG_DATA_CONTIGUOUS | PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP),
                        &zp->pbuf, zslot, ZC_SLOT_PAYLOAD_LEN);
                if (p) {
                    rxprof_t3();
                    rxprof_t4();
                    sdio_zc_wraps++;
                    handle_eth_frame_to_wifi_tx(sdm, p, is_sta);
                    return;
                }
            }
            /* Wrap failed (cannot happen with these constants) — return the
             * slot so the dnld queue keeps its buffer; the frame is dropped
             * (the slot may be re-armed/overwritten at any point after the
             * free, so falling through to the copy path would race). */
            sdio_zc_wrap_fail++;
            smid_zc_slot_free(zslot);
            return;
        }
    }

    /* T4 rework: bail before the alloc ceremony only while the pool is in
     * its authoritative exhausted state (see comment at sdio_earlydrop_cnt;
     * set/cleared under the pool lock in sdiowifi_tx_buf.c). */
    if (sdm->tx_desc.pool_empty) {
        sdio_earlydrop_cnt++;
        sdio_wifi_tx_pool_drops++;
        ABDBG_TRACE(0x55, (uint16_t)sdio_wifi_tx_pool_drops); /* L5: pool-full drop */
        return;
    }

    p = sdiowifi_tx_buf_alloc(&sdm->tx_desc);
    rxprof_t3();

    if (!p) {
        /* 2026-05-14 TP: exported counter (was file-static). This is the
         * pre-fhost drop site - SDIO RX got a frame but the wifi-TX pbuf
         * pool was empty. Pool size = SDIO_TX_NORMAL_BUFFER_COUNT; turnover
         * gated by cfm_lat. Surfaces in tx_diag for bench correlation. */
        extern volatile uint32_t sdio_wifi_tx_pool_drops;
        sdio_wifi_tx_pool_drops++;
        ABDBG_TRACE(0x55, (uint16_t)sdio_wifi_tx_pool_drops); /* L5: pool-full drop */
        HR_LOGD("no more tx pbuf (drops=%lu)\r\n", (unsigned long)sdio_wifi_tx_pool_drops);
        return;
    }

    /* IPRO6LE: pbuf_take's MEMCPY routes to byte-loop libc memcpy, costing
     * ~160us / 1520B (D34 in tx_profile). Source/dest both in ram_wifi
     * (non-cached cross-bus 0x23xxxxxx). Swap in arch_memcpy_fast which
     * uses 4-byte aligned word copies. Single-pbuf only -- the TX pool
     * always returns one contiguous pbuf with payload >= TX_PBUF_FRAME_LEN. */
    if (frame_len > p->tot_len) {
        HR_LOGE("frame too big: %u > %u\r\n", frame_len, p->tot_len);
        pbuf_free(p);
        return;
    }

    /* DEBUG 2026-07-23 ZC-probe (see gate comment at top): reroute the frame
     * through the OCRAM staging slot tied to this pool slot. Takes precedence
     * over the dmacpy copy modes. */
#if SDIO_ZCPROBE_RETAIN
    if (g_sdio_zcprobe) {
        int zslot = sdiowifi_tx_buf_slot(p);
        if (zslot >= 0 && zslot < (int)ZC_STAGE_CNT) {
            uint8_t *st = zc_stage[zslot];
            g_zcprobe_lo = (uint32_t)&zc_stage[0][0];
            g_zcprobe_hi = (uint32_t)&zc_stage[0][0] + sizeof(zc_stage);
            arch_memcpy_fast(st + ZC_STAGE_HR, frame, frame_len);
            p->payload = st + ZC_STAGE_HR;
            rxprof_t4();
            p->len = frame_len;
            p->tot_len = frame_len;
            zc_probe_cnt++;
            handle_eth_frame_to_wifi_tx(sdm, p, is_sta);
            return;
        }
        zc_probe_noslot++;
    }
#endif /* SDIO_ZCPROBE_RETAIN */
    /* DEBUG 2026-07-23 T1: gated copy-mode experiment (g_sdio_dmacpy_mode).
     * mode 1 overlaps the bulk DMA copy with the fhost inject below. The overlap
     * is safe by construction:
     *   - fhost_tx_req_do reads only the first ~16 bytes (eth hdr + DSCP), which
     *     sdio_dmacpy_start() always copies on the CPU (>=64B head) before it
     *     returns, so the inject never touches DMA-owned payload;
     *   - the payload push runs later on the WiFi task (prio 27), which cannot
     *     preempt this SDIO worker (prio 31) until the worker blocks;
     *   - sdio_dmacpy_finish() completes the DMA before this function returns,
     *     i.e. before this worker yields, so the DMA is always done before any
     *     consumer reads the payload.
     * mode 0 keeps the legacy single arch_memcpy_fast (no behavior change). */
    uint32_t dma_bytes = 0;
    if (g_sdio_dmacpy_mode == 1) {
        dma_bytes = sdio_dmacpy_start(p->payload, frame, frame_len);
    } else if (g_sdio_dmacpy_mode == 2) {
        sdio_dmacpy_cached_copy(p->payload, frame, frame_len);
    } else {
        arch_memcpy_fast(p->payload, frame, frame_len);
    }
    rxprof_t4();
    p->len = frame_len;
    p->tot_len = frame_len;

    handle_eth_frame_to_wifi_tx(sdm, p, is_sta);
    if (dma_bytes)
        sdio_dmacpy_finish();
}

static void handle_sniffer_tx(sdiowifi_mgmr_t *sdm, const uint8_t *frame, uint16_t frame_len)
{
#ifdef CFG_IPRO_WIFI
#else
    wifi_mgmr_raw_80211_send((uint8_t *)frame, frame_len);
#endif
}

static void handle_eth_wifi_frame_recv(sdiowifi_mgmr_t *sdm, const uint8_t *frame, uint16_t frame_len)
{
    uint16_t subtype;

    rxprof_t2();
    subtype = frame[0] + (frame[1] << 8);
    switch (subtype) {
    case IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_STA_TO_WIFI_TX:
        handle_sta_ap_to_wifi_tx(sdm, frame + 2, frame_len - 2, true);
        break;
    case IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_AP_TO_WIFI_TX:
        handle_sta_ap_to_wifi_tx(sdm, frame + 2, frame_len - 2, false);
        break;
    case IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_SNIFFER_TO_WIFI_TX:
        handle_sniffer_tx(sdm, frame + 2, frame_len - 2);
        break;
    case IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_EXT:
        sdiowifi_ext_frame_process(sdm, frame + 2, frame_len - 2);
        break;
    default:
        break;
    }
}

static void handle_cmd_recv(sdiowifi_mgmr_t *sdm, const uint8_t *data, uint16_t data_len)
{
    rnms_cmd_input_raw(sdm->rnm, data, data_len);
}

static void sdio_recv_cb(void *arg, const void *data_ptr, uint16_t data_len)
{
    bl602_sdio_msg_t *msg;
    sdiowifi_mgmr_t *sdm;

    rxprof_t1();
    msg = (bl602_sdio_msg_t *)data_ptr;
    sdm = &g_sdiowifi_mgmr;
    switch (msg->type) {
    case IPRO_MSG_TYPE_ETH_WIFI_FRAME:
        HR_LOGD("sdio_recv_cb IPRO_MSG_TYPE_ETH_WIFI_FRAME\r\n");
        handle_eth_wifi_frame_recv(sdm, msg->payload, msg->len - sizeof(*msg));
        break;
    case IPRO_MSG_TYPE_CMD:
        HR_LOGD("sdio_recv_cb IPRO_MSG_TYPE_CMD\r\n");
        handle_cmd_recv(sdm, msg->payload, msg->len - sizeof(*msg));
        break;
    default:
        /* No logging here: this runs in the SDIO RX hot path and the
         * unknown-type flood (~10/s under load) was stalling the datapath
         * ~13 ms per line. Count + remember type; dump first occurrence
         * once. Read via tx_diag. */
        sdio_recv_unknown_cnt++;
        sdio_recv_unknown_last_type = msg->type;
        if (sdio_recv_unknown_cnt == 1) {
            const uint8_t *b = (const uint8_t *)data_ptr;
            HR_LOGE("sdio_recv_cb UNKNOW type:%d data_len:%u first16: "
                    "%02X %02X %02X %02X %02X %02X %02X %02X "
                    "%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                    msg->type, data_len,
                    b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7],
                    b[8], b[9], b[10], b[11], b[12], b[13], b[14], b[15]);
        }
        break;
    }
}

static int ipro_net_wifi_trcver_write_mac(net_wifi_trcver_t *trcver)
{
    int i;
    uint8_t mac[6];

#ifdef CFG_IPRO_WIFI
    // FIXME
    wifi_mgmr_sta_mac_get(mac);
#else
    bl_wifi_mac_addr_get(mac);
#endif

    for (i = 0; i < 6; ++i) {
        ipro_write_s_reg(trcver, WIFI_MAC_ADDR_SCRATCH_OFFSET + i, mac[i]);
    }

    return 0;
}

#if SDIOWIFI_ACK_BOOT_SRC
static void start_ack_bootsrc(void *pvParameters)
{
    net_wifi_trcver_t *trcver;

    if ((uintptr_t)pvParameters & 1) {
        uint8_t v;
        sdiowifi_tick_t tick = sdiowifi_tick_now(0);
        trcver = (net_wifi_trcver_t *)((uintptr_t)pvParameters & ~1);

        while (1) {
            v = ipro_read_s_reg(trcver, BOOT_SRC_INFO_SCRATCH_OFFSET);
            if (v == IPRO_BOOT_SRC_PING_VALUE) {
                HR_LOGI("Got host sdio scratch reg ping, time cost %lu ms\r\n", sdiowifi_tick_now(0) - tick);
                ipro_write_s_reg(trcver, BOOT_SRC_INFO_SCRATCH_OFFSET, IPRO_BOOT_SRC_PONG_VALUE);
                break;
            }
            if (sdiowifi_tick_now(0) - tick > BOOT_SRC_PONG_TIMEOUT_MS) {
                HR_LOGE("Get host sdio scratch reg ping timed out\r\n");
                break;
            }
            sdiowifi_delay_ms(1);
        }
        sdiowifi_task_delete(NULL);
    } else {
        if (0 != sdiowifi_task_create(start_ack_bootsrc, "ack_bootsrc", 0, 1024, (void *)((uintptr_t)pvParameters | 1), 20, NULL)) {
            HR_LOGE("create ack_bootsrc task failed\r\n");
        }
    }
}
#endif


// XXX copied from bl_rx.h. Remember to keep identical!
/////////////// start of copy ///////////////
#define PBUF_FLAG_AMSDU 0x80U
/////////////// end of copy ///////////////

static void sdio_pbuf_cfm_cb(int idx, void *arg)
{
    pbuf_free(arg);
}

int __attribute__((section(".tcm_code"))) ipro_dual_stack_peer_input(void *pkt, void *arg)
{
    net_wifi_trcver_t *trcver = &g_sdiowifi_mgmr.trcver;
    struct pbuf *p = (struct pbuf *)pkt;
    bool is_amsdu = p->flags & PBUF_FLAG_AMSDU;
    int ret = ipro_msg_send_pbuf(trcver, IPRO_MSG_TYPE_ETH_WIFI_FRAME, IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_STA_FROM_WIFI_RX, p, is_amsdu, sdio_pbuf_cfm_cb, pkt);
    return ret;
}

static void *eth_input_hook(bool is_sta, void *pkt, void *arg)
{
    struct pbuf *p = (struct pbuf *)pkt;

#ifdef CFG_IPRO_WIFI
    if (npf_is_8021X(p)) {
        return pkt;
    }
#endif

    bool input_emb = true;
    bool input_host = false;
    struct pbuf *p_dup = NULL;
    struct pbuf *ret = NULL;

#ifdef DHCP_IN_EMB
    if (npf_is_arp(p)) {
        /* ARP duplicated: emb keeps the WiFi association ARP cache warm,
         * host needs it so its iproeth0 ARP table resolves. */
        input_emb = true;
        input_host = true;
    } else if (npf_is_dhcp4(p)) {
        /* DHCP terminates in emb (the DUT lwIP runs the DHCP client). */
        input_emb = true;
        input_host = false;
    } else {
        /* Everything else is data traffic for the host's iproeth0 stack;
         * the DUT-side lwIP never holds matching sockets so handing the
         * pbuf to emb is a guaranteed drop (or worse, an RST that breaks
         * inbound TCP). Forward to host only. */
        input_emb = false;
        input_host = true;
    }
#else
    input_emb = false;
    input_host = true;
#endif
    // TODO Add custom filter here
    // Example of handling some TCP traffic in emb:
#ifdef CONF_SDIOWIFI_TEST
    if (npf_is_tcp4_port(p, 50001) || npf_is_udp4_port(p, 50001)) {
        input_emb = true;
        input_host = false;
    }
    if (npf_is_icmp4(p)) {
        input_emb = true;
        input_host = false;
    }
#endif

    if (!input_emb && !input_host) {
        pbuf_free(p);
        return NULL;
    }

    if (input_emb && input_host) {
        p_dup = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
        if (p_dup == NULL) {
            pbuf_free(p);
            return NULL;
        }
        pbuf_copy(p_dup, p);
    }

    if (input_host) {
        // XXX distinguish STA/AP
        sdio_upld_attempt++;
        int ret = ipro_dual_stack_peer_input(p, NULL);
        if (ret) {
            /* printf("RX FRM swdesc %p failed, drop\r\n", swdesc); */
            sdio_upld_fail++;
            pbuf_free(p);
        }
    }

    if (p_dup) {
        // both emb & host
        ret = p_dup;
    } else if (input_emb) {
        ret = p;
    } else {
        ret = NULL;
    }
    return ret;
}

static void *eth_output_hook(bool is_sta, void *pkt, void *arg)
{
    net_wifi_trcver_t *trcver = (net_wifi_trcver_t *)arg;

    (void)trcver;
    struct pbuf *p = (struct pbuf *)pkt;

    if (npf_is_8021X(p)) {
        return pkt;
    }

#ifdef DHCP_IN_EMB
    // Allow ARP & DHCP output
    if (npf_is_arp(p) || npf_is_dhcp4(p)) {
        return pkt;
    }
#endif
    // TODO Add custom filter here
#ifdef CONF_SDIOWIFI_TEST
    if (npf_is_tcp4_port(p, 50001) || npf_is_udp4_port(p, 50001)) {
        return pkt;
    }
#endif
    HR_LOGI("dropping local TCP/IP stack traffic\r\n");

    return NULL;
}

static void register_pkt_hooks(net_wifi_trcver_t *trcver)
{
#ifndef SDIO_TEST_NO_HOST
    wifi_pkt_eth_input_hook_register(eth_input_hook, trcver);

    // Allowing all output.
    // Leave output hook here as it may be added back.
    (void)eth_output_hook;
    /* bl_pkt_eth_output_hook_register(eth_output_hook, trcver); */
#endif
}

int ipro_net_wifi_trcver_init(net_wifi_trcver_t *trcver, void *arg)
{
    sdiowifi_mutex_create(&trcver->tx_lock);
    ipro_sdu_init();
    ipro_sdio_handshake();
    ipro_sdio_read_cb_register(NULL, sdio_recv_cb, trcver);
    ipro_net_wifi_trcver_write_mac(trcver);
#if SDIOWIFI_ACK_BOOTSRC
    start_ack_bootsrc(trcver);
#endif
    register_pkt_hooks(trcver);
    trcver->arg = (void *)arg;
    return 0;
}

int ipro_net_wifi_trcver_reinit(net_wifi_trcver_t *trcver)
{
    ipro_net_wifi_trcver_write_mac(trcver);

    return 0;
}

int ipro_net_wifi_trcver_set_present(net_wifi_trcver_t *trcver, bool present)
{
    if (!trcver) {
        return -1;
    }
    trcver->host_present = present;
    return 0;
}

int ipro_msg_send(net_wifi_trcver_t *trcver, uint16_t type, const void *payload, uint16_t payload_len)
{
    int ret;
    if (!trcver->host_present) {
        return IPRO_MSG_ERR_HOST_NOT_READY;
    }
    sdiowifi_mutex_lock(trcver->tx_lock);
    ret = ipro_sdio_write_cmd(NULL, type, 0, payload, payload_len, NULL, 0);
    sdiowifi_mutex_unlock(trcver->tx_lock);
    return ret;
}

int ipro_msg_send_pbuf(net_wifi_trcver_t *trcver, uint16_t type, uint16_t subtype, struct pbuf *p, bool is_amsdu, void *cb, void *cb_arg)
{
    int ret;
    if (!trcver->host_present) {
        return IPRO_MSG_ERR_HOST_NOT_READY;
    }
    sdiowifi_mutex_lock(trcver->tx_lock);
    ret = ipro_sdio_write_pbuf(NULL, type, subtype, p, is_amsdu, cb, cb_arg);
    sdiowifi_mutex_unlock(trcver->tx_lock);
    if (ret == IPRO_MSG_ERR_DESC_USED) {
        sdiowifi_mgmr_reinit(false);
    }
    return ret;
}

int ipro_msg_send_frame_ext(net_wifi_trcver_t *trcver, uint16_t type, uint16_t subtype, const void *headroom, uint16_t headroom_len, const void *tailroom, uint16_t tailroom_len)
{
    int ret;
    if (!trcver->host_present) {
        return IPRO_MSG_ERR_HOST_NOT_READY;
    }

    sdiowifi_mutex_lock(trcver->tx_lock);
    ret = ipro_sdio_write_cmd(NULL, type, subtype, headroom, headroom_len, tailroom, tailroom_len);
    sdiowifi_mutex_unlock(trcver->tx_lock);
    return ret;
}

uint8_t ipro_read_s_reg(net_wifi_trcver_t *trcver, uint32_t offset)
{
    uint8_t ret;

    ret = sdu_read_s_reg(offset);
    return ret;
}

void ipro_write_s_reg(net_wifi_trcver_t *trcver, uint32_t offset, uint8_t val)
{
    sdu_write_s_reg(offset, val);
}
