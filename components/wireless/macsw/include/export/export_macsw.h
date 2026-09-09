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

#ifndef _EXPORT_MACSW_H_
#define _EXPORT_MACSW_H_
#include "stdio.h"
#include "check_cfg.h"
#include "rwnx_config.h"
#include "export/common/co_list.h"
#include "export/common/co_status.h"
#include "phy.h"
#include "llc.h"
#include "export/ke/ke_task.h"

#include "export_struct.h"

#define struct_func_alloc_def(name)                                             \
    void *name##_alloc();                                                       \

#include "export_malloc_list.h"
#undef struct_func_alloc_def

void *vif_info_get_vif(int index);
uint8_t mac_vif_get_sta_ap_id(void *macif);
uint16_t mac_vif_get_bcn_int(void *macvif);
void mac_vif_get_channel(void *macvif, struct mac_chan_op* chan);
void mac_vif_get_sta_status(void *macvif, struct mac_addr *bssid, uint16_t *aid, int8_t *rssi);
struct co_list *mac_vif_get_sta_list(void *macif);
int mac_vif_get_key_info_len(void *macif);
struct key_info_tag *mac_vif_get_key(void *macif, int key_idx);
uint8_t mac_vif_get_index(void *macif);
enum mac_vif_type mac_vif_get_type(void *macif);
bool mac_vif_get_active(void *macif);

uint8_t sta_get_staid(void *p);
void *sta_getp_mac_addr(void *p);
void *sta_get_list_next(void *p);
uint32_t sta_idx_get_last_active(uint16_t sta_idx);
void *sta_idx_getp_mac_addr(uint16_t sta_idx);
uint8_t sta_idx_get_mlink_idx(uint16_t sta_idx);
uint8_t sta_idx_get_key_idx(uint16_t sta_idx);
void *sta_idx_getp_key_info(uint16_t sta_idx);
bool sta_idx_get_valid(uint16_t sta_idx);
uint8_t sta_idx_get_inst_nbr(uint16_t sta_idx);
uint8_t sta_idx_get_uapsd_queues(uint16_t sta_idx);

uint8_t export_rc_get_format_mod(uint32_t rate_config);
uint8_t export_rc_get_mcs_index(uint32_t rate_config);
uint8_t export_rc_get_nss(uint32_t rate_config);
uint8_t export_rc_get_bw(uint32_t rate_config);
uint8_t export_rc_get_pre_type(uint32_t rate_config);
uint8_t export_rc_get_sgi(uint32_t rate_config);

int8_t export_hal_desc_get_rssi(void *rx_vec_1, int8_t *rx_rssi);
uint16_t export_hal_desc_get_ht_length(void *rx_vec_1);
uint8_t export_MM_STA_TO_KEY(uint16_t sta_idx);
uint8_t export_MM_MLINK_TO_MFP_KEY(uint8_t key_idx, uint8_t mlink_idx);
uint8_t export_MM_MLINK_TO_KEY(uint8_t key_idx, uint8_t mlink_idx);
uint8_t export_MM_VIF_TO_KEY(uint8_t key_idx, uint8_t vif_idx);
uint8_t export_MM_VIF_TO_MFP_KEY(uint8_t key_idx, uint8_t vif_idx);
void me_env_set_ps_mode(int mode);
int get_time_SINCE_EPOCH(uint32_t *sec, uint32_t *usec);
int get_time_SINCE_BOOT(uint32_t *sec, uint32_t *usec);

struct fhost_tx_desc_tag;
struct txdesc;
bool inline_hal_machw_he_support(void);
uint32_t inline_hal_machw_time(void);
bool inline_hal_machw_time_cmp(uint32_t time1, uint32_t time2);
bool inline_hal_machw_time_past(uint32_t time);
int get_nx_txdesc_cnt(int i);
uint8_t inline_nxmac_tkip_getf();
uint8_t inline_nxmac_ccmp_getf();
uint8_t inline_nxmac_gcmp_getf();

/* These structs are defined in release headers (mm_timer.h, me_task.h,
   rxu_cntrl.h). Skip when compiling macsw internally. */
#ifndef MACSW_INTERNAL
typedef void (*cb_timer_func_ptr)(void *);
struct mm_timer_tag
{
    /// List element for chaining
    struct co_list_hdr list_hdr;
    /// Function to be called upon timer expiry
    cb_timer_func_ptr cb;
    /// Pointer to be passed to the callback
    void *env;
    /// Absolute expiry time
    uint32_t time;
};

/// Structure containing the parameters of the @ref ME_CHAN_CONFIG_REQ message
struct me_chan_config_req
{
    /// List of 2.4GHz supported channels
    struct mac_chan_def chan2G4[MAC_DOMAINCHANNEL_24G_MAX];
    /// List of 5GHz supported channels
    struct mac_chan_def chan5G[MAC_DOMAINCHANNEL_5G_MAX];
    /// Number of 2.4GHz channels in the list
    uint8_t chan2G4_cnt;
    /// Number of 5GHz channels in the list
    uint8_t chan5G_cnt;
};

/// RX status value structure (as expected by Upper Layers)
struct rxu_stat_val
{
    /// Host Buffer Address
    uint32_t host_id;
    /// Length
    uint32_t frame_len;
    /// Status (@ref rx_status_bits)
    uint16_t status;
};

void mm_timer_set(struct mm_timer_tag *timer, uint32_t value);
void mm_timer_clear(struct mm_timer_tag *timer);
#endif /* MACSW_INTERNAL */
void tx_txdesc_release(struct txdesc *txdesc);
void me_init_chan(struct mac_chan_op *chan);
uint8_t vif_mgmt_get_staid(void *vif, const struct mac_addr *sta_addr);
uint32_t dbg_snprintf(char *buffer, uint32_t size, const char *fmt, ...);
int macif_kmsg_push(int msg_id, int task_id, void *param, int param_len);
int macif_kmsg_push_t(int msg_id, int task_id, int nb_msg, ...);
void macif_rx_buf_ind(void);
void macif_tx_data_ind(int queue_idx);
void time_init(uint32_t sec, uint32_t usec);

bool mfp_is_robust_frame(uint16_t frame_cntl, uint8_t action);

#define INVALID_VIF_IDX 0xFF
#define LLC_ETHER_MTU                1500
#define LLC_ETHER_HDR_LEN            14
#define LLC_LLC_LEN                  3
#define LLC_SNAP_LEN                 5
#define LLC_802_2_HDR_LEN            (LLC_LLC_LEN + LLC_SNAP_LEN)
#define RX_MAX_AMSDU_SUBFRAME_LEN (LLC_ETHER_MTU + LLC_ETHER_HDR_LEN + LLC_802_2_HDR_LEN)
#define TX_PBD_CNT            5
#define BCN_MAX_CSA_CPT 2
#define SCAN_SSID_MAX   2
#define SCAN_CHANNEL_MAX (MAC_DOMAINCHANNEL_24G_MAX + MAC_DOMAINCHANNEL_5G_MAX)
#define RC_DCM_MOD_OFT           14
#define RC_DCM_MOD_MASK          CO_BIT(RC_DCM_MOD_OFT)
#define RX_FLAGS_IS_AMSDU_BIT         CO_BIT(0)
#define RX_FLAGS_IS_MPDU_BIT          CO_BIT(1)
#define RX_FLAGS_4_ADDR_BIT           CO_BIT(2)
#define RX_FLAGS_NEW_MESH_PEER_BIT    CO_BIT(3)
#define RX_FLAGS_NON_MSDU_MSK        (RX_FLAGS_IS_MPDU_BIT)
/* macsw bitfield layout (see rxu_cntrl.h rx_flags_bf): VIF_IDX 4-bit,
 * STA_IDX 10-bit, DST_STA_IDX 10-bit. Older 8-bit definitions caused
 * STA_IDX bit 12 to leak into VIF_INDEX read on reconnect (sta_idx>=1),
 * making fhost_vif lookup fail (vif_idx=17) and silently drop EAPOL. */
#define RX_FLAGS_VIF_INDEX_OFT  8
#define RX_FLAGS_VIF_INDEX_MSK  (0xF << RX_FLAGS_VIF_INDEX_OFT)
#define RX_FLAGS_STA_INDEX_OFT  12
#define RX_FLAGS_STA_INDEX_MSK  (0x3FF << RX_FLAGS_STA_INDEX_OFT)
#define RX_FLAGS_DST_INDEX_OFT  22
#define RX_FLAGS_DST_INDEX_MSK  (0x3FF << RX_FLAGS_DST_INDEX_OFT)
#define TXU_CNTRL_RETRY         CO_BIT(0)
#define TXU_CNTRL_UNDER_BA      CO_BIT(1)
#define TXU_CNTRL_MORE_DATA     CO_BIT(2)
#define TXU_CNTRL_MGMT          CO_BIT(3)
#define TXU_CNTRL_MGMT_NO_CCK   CO_BIT(4)
#define TXU_CNTRL_MGMT_ROBUST   CO_BIT(7)
#define TXU_CNTRL_EOSP          CO_BIT(9)
#define TXU_CNTRL_REUSE_SN      CO_BIT(15)
#define TX_STATUS_DONE              CO_BIT(0)
#define TX_STATUS_RETRY_REQUIRED    CO_BIT(1)
#define TX_STATUS_SW_RETRY_REQUIRED CO_BIT(2)
#define TX_STATUS_ACKNOWLEDGED      CO_BIT(3)
#define TX_STATUS_SN_OFT            4
#define TX_STATUS_SN_MSK            (0xFFF << (TX_STATUS_SN_OFT))
#define FORMATMOD_NON_HT          0
#define FORMATMOD_NON_HT_DUP_OFDM 1
#define FORMATMOD_HT_MF           2
#define FORMATMOD_HT_GF           3
#define FORMATMOD_VHT             4
#define FORMATMOD_HE_SU           5
#define FORMATMOD_HE_MU           6
#define FORMATMOD_HE_ER           7

struct fhost_statistic {
    //TX
    //AMPDU related
    uint32_t tx_ampdu_retry_limit_count;
    uint32_t tx_ampdu_lft_expired_count;
    uint32_t tx_ampdu_sw_retry;
    uint32_t tx_ampdu_unvalid_ba;
    uint32_t tx_ampdu_agg_force_finish;
    uint32_t tx_ampdu_rts_drop_to_cts_self;

    // MPDU success tracking
    uint32_t tx_mpdu_total;
    uint32_t tx_mpdu_ok;
    uint32_t tx_ampdu_ba_valid;
    uint32_t tx_ampdu_ba_invalid;
    uint32_t ba_fail_keyidx;
    uint32_t ba_fail_sta;
    uint32_t ba_fail_addr;
    uint32_t ba_fail_len;
    uint32_t ba_fail_tid;
    uint32_t ba_fail_ih_empty;
    uint32_t ih_mismatch_cnt;
    uint32_t ih_mismatch_last_fc;
    uint32_t ih_ndp_cnt;
    uint32_t bar_done_cnt;
    uint32_t bar_success_cnt;
    uint32_t bar_fail_cnt;
    uint32_t ba_fail_cnt;       // BAR success but BA not in IH buffer (soft recovery)
    uint32_t bar_last_status;
    uint32_t bar_last_mci2;
    uint32_t bar_last_mci1;
    uint32_t athd_last_status;
    uint32_t bar_addr;
    uint32_t athd_addr;
    uint32_t bar_pol_addr;

    // Phase 2 instrumentation
    // 1. Confirmation latency (us, from chain to cfm_evt last MPDU)
    uint32_t cfm_latency_sum;
    uint32_t cfm_latency_cnt;
    uint32_t cfm_latency_max;
    // 2. AMPDU close source. The first four are attributed by the value the CALLER
    // left in txl_agg_close_source, so they mean "closed while <path> was the last
    // to write that global" - NOT the closing site. The four below them are set at
    // the site itself and do mean what they say. See RC_P9_0809.md.
    uint32_t agg_close_macif;    // closed under the macif_tx_evt drain
    uint32_t agg_close_cfm;      // closed under txl_cfm_evt
    uint32_t agg_close_fexch;    // closed under txl_frame_exchange_done
    uint32_t agg_close_push;     // closed under the non-agg/singleton push path
    uint32_t agg_close_maxcnt;   // curr_cnt hit agg->max_cnt        (txl_agg.c)
    uint32_t agg_close_maxlen;   // next subframe exceeds max_len    (txl_agg.c)
    uint32_t agg_close_incompat; // next txdesc not an aggregatable non-first MPDU
                                 // (rc_tx_check_aggregation refused)  (txl_agg.c)
    uint32_t agg_close_timer;    // hold-down timer force-close        (txl_agg.c)
    uint32_t agg_close_incsta;   // next txdesc sta_idx/tid differs    (txl_agg.c)
    // Trial accounting (rc.c:3183). rc_trial_set counts every trial the RC
    // schedules; rc_trial_first the subset that was under BA and therefore got
    // TX_SWDESC_UMAC_AMPDU_FIRST - i.e. the ones that can close an A-MPDU. A
    // pinned arm must show ZERO in both, which is the counter's own sanity check.
    uint32_t rc_trial_set;
    uint32_t rc_trial_first;
    // 3. DMA idle ratio
    uint32_t dma_newtail_cnt;    // pipeline (DMA was active)
    uint32_t dma_newhead_cnt;    // restart (DMA was idle)
    // Latency breakdown: air_time = done - chain, sched_delay = cfm - done
    uint32_t air_time_sum;
    uint32_t air_time_max;
    uint32_t sched_delay_sum;
    uint32_t sched_delay_max;
    // 4. AMPDU size histogram
    uint32_t ampdu_hist_1;       // singleton AMPDU (1 MPDU)
    uint32_t ampdu_hist_2_4;     // 2-4 MPDUs
    uint32_t ampdu_hist_5_8;     // 5-8 MPDUs
    uint32_t ampdu_hist_9_16;    // 9-16 MPDUs
    uint32_t ampdu_hist_17p;     // 17+ MPDUs
    // 5. Pipeline depth tracking
    uint32_t ppdu_cnt_max;       // max ppdu_cnt observed
    uint32_t ppdu_cnt_sum;       // sum of ppdu_cnt at chain time (for avg)
    uint32_t ppdu_cnt_samples;   // number of samples
    // 6. ISR timing
    uint32_t isr_time_sum;       // total time in txl_frame_exchange_done (us)
    uint32_t isr_call_cnt;       // number of ISR calls
    uint32_t isr_ppdu_done_sum;  // total PPDUs completed across all ISR calls
    // 7. last_frame_exch NULL source tracking
    uint32_t lfe_null_empty;     // transmitting list empty
    uint32_t lfe_null_nonext;    // no next in chain
    uint32_t lfe_null_bar;       // BAR done, no next PPDU
    uint32_t lfe_null_guard;     // check_saved_agg_desc guard
    uint32_t lfe_orphan_restart; // ISR popped DONE_HW, restarted orphaned next
    uint32_t lfe_restart_ampdu;  // lfe_clear_and_restart: newhead for AMPDU a_thd
    uint32_t lfe_restart_sglt;   // lfe_clear_and_restart: newhead for singleton
    uint32_t lfe_restart_done;   // lfe_clear_and_restart: head DONE_HW, skipped
    uint32_t lfe_restart_post;   // post-pop restart: newhead for pending
    // 9. newhead tracking
    uint32_t newhead_skip_cnt;   // state=2 guard skipped newhead
    // DEBUG 2026-07-25 wedge #2: newhead refused because the AC DMA was in
    // state 3 (latched). Gated by g_txl_newhead_st0.
    uint32_t newhead_st3_skip_cnt;
    uint32_t newhead_ok_cnt;     // newhead actually written to HW
    uint32_t newtail_idle_cnt;   // newtail race: HW idle → fallback newhead
    uint32_t newhead_last_addr;  // last successful newhead address (HW)
    uint32_t newhead_last_skip;  // last skipped newhead address (HW)
    uint8_t  newhead_last_ac;    // last successful newhead AC
    // 10. recover_stale extended recovery
    uint32_t recover_bar_stuck;  // BAR stuck in BAR_THD_CHK_STATE
    uint32_t recover_dma_idle;   // DMA idle orphan restart
    uint32_t assert_rec_cnt;     // recover_stale failed → actual ASSERT_REC
    uint32_t stale_fail_cnt;     // recover_stale: DMA busy or head DONE_HW
    uint32_t lfe_stale_cnt;      // stale last_frame_exch caught by re-read
    uint32_t stale_gen_cnt;      // cfm_evt gen mismatch (agg_desc reused before cfm)
    // 10b. fexch close diagnostics
    uint32_t fexch_check_cnt;    // ISR "no next" path reached (singleton)
    uint32_t fexch_ppdu_ne1;     // ppdu_cnt != 1 at check time
    uint32_t fexch_agg_null;     // ppdu_cnt==1 but agg[0].desc==NULL
    // 10c. Chain event trace (ring buffer, last 8 events)
    uint32_t chain_trace_idx;
    struct {
        uint32_t thd_addr;     // first_thd address
        uint8_t  method;       // 0=newhead, 1=newtail, 2=nt_idle_fallback
        uint8_t  dma_state;    // per-AC DMA state at chain time
        uint8_t  nh_result;    // newhead return: 1=ok, 0=skip
        uint8_t  ac;
    } chain_trace[8];
    uint32_t ba_rx_total;        // BA frames seen in RX DMA handler
    uint32_t ba_rx_filtered;     // BA frames filtered (missing RSP_FRM|SUCCESS)
    // 11. Push flow counters (macif_tx_data_ind → macif_tx_evt → fhost_tx_schedule → cfm)
    uint32_t macif_tx_evt_cnt;       // macif_tx_evt entry count
    uint32_t macif_tx_evt_pushed;    // frames pushed by macif_tx_evt
    uint32_t macif_tx_data_ind_cnt;  // macif_tx_data_ind call count
    uint32_t fhost_tx_sched_cnt;     // fhost_tx_schedule call count
    uint32_t fhost_tx_sched_pushed;  // frames pushed by fhost_tx_schedule
    uint32_t fhost_tx_cfm_cnt;       // fhost_tx_cfm_push call count
    // 8. ppdu_cnt event trace (ring buffer, first 64 events after reset)
    uint32_t tx_repush_cnt;     // BAM retry → txu_cntrl_repush calls
    uint32_t tx_repush_deferred; // repush held while HW current pointer owns THD
    uint32_t tx_repush_deferred_drained;
    uint32_t tx_repush_deferred_wake;
    uint32_t agg_check_ppdu0;   // txl_agg_check called with ppdu_cnt==0
    uint32_t agg_check_ppduN;   // txl_agg_check called with ppdu_cnt>0
    // 12. TCP RX diagnostic counters
    uint32_t macif_tx_preempt_cnt;   // macif_tx_evt yielded to HIGH_PRIO_EVT
    uint32_t macif_tx_single_push;   // macif_tx_evt pushed exactly 1 frame
    uint32_t ppdu_leak_reset_cnt;    // ppdu_cnt force-reset (leak detected)
    uint32_t ppdu_underflow_cnt;      // total ppdu_cnt-- when already 0
    uint8_t  ppdu_underflow_site[20]; // per-site underflow count (saturating)
    // Per-site ppdu_cnt++ tracker (mirror of ppdu_underflow_site). Wedge
    // hypothesis: a site is inc'ing without matching dec. Sites are
    // enumerated alongside the inc call (txl_cntrl.c/txl_agg.c/txl_he.c/
    // txl_mu.c). 11 used; spare slots for future sites.
    uint32_t ppdu_inc_site[16];
    // txl_agg_finish accounting (incl. confirming whether the suspect
    // operator-precedence guard at txl_agg.c:2266 actually fires).
    uint32_t agg_finish_entered;     // total txl_agg_finish entries
    uint32_t agg_finish_skipped;     // early return via "PUSHED not set" guard
    uint32_t agg_finish_emitted;     // reached terminal ppdu_cnt++ inc
    // BA bitmap race diagnostic (Suspect #1 — is CFM reading ssc_bitmap
    // before BA decode completes?)
    uint32_t ba_extract_ack_cnt;      // txl_ba_extract_ack total invocations (~= tx_mpdu_total)
    uint32_t ba_null_at_cfm;          // ssc_bitmap == NULL at CFM (HE all-ack OR race win)
    uint32_t ba_decoded_total;        // txl_agg_ba_handle valid BA count (sets AGG_BA_VALID)
    // 2026-08-07: the two silent zero-return branches of txl_ba_extract_ack.
    // Both consume an MPDU as "not acked" and return without touching agg_ok,
    // so an SSN desync routes 100% of MPDUs through one of them forever while
    // tx_mpdu_total keeps climbing - which is exactly the fingerprint of the
    // one-arm-in-eight uplink collapse (mpdu_ok delta 0, mpdu total moving,
    // downlink fine, console alive, association up). Without these two the
    // failure is indistinguishable from "the air was bad".
    uint32_t ba_sn_below_win;         // txdesc SN below BA window start
    uint32_t ba_sn_out_of_bmap;       // SN past the end of the BA bitmap
    uint32_t ppdu_trace_idx;
    struct {
        uint16_t time_lo;   // lower 16 bits of hal_machw_time (wraps at 65ms)
        uint8_t  ppdu_cnt;  // ppdu_cnt AFTER the change
        uint8_t  source;    // 0=agg_finish, 1=singleton, 2=pool_empty, 3=bar_done, 4=sglt_done, 5=pre_chain
    } ppdu_trace[64];
    // PM12 instrument: aggregates-list push/pop asymmetry per AC.
    // Hypothesis: recover_stale case 4 dec ppdu_cnt without popping
    // owning agg_desc from txl->aggregates → orphan agg_desc accumulates.
    // push - pop == agg=N seen in txl_dump_queues post-wedge.
    uint16_t aggregates_push_cnt[NX_TXQ_CNT];      // txl_agg_chain_ampdu
    uint16_t aggregates_pop_cnt[NX_TXQ_CNT];       // sum of all pop sites
    uint16_t aggregates_pop_ba[NX_TXQ_CNT];        // ba_push (txl_cfm.c)
    uint16_t aggregates_pop_bar_ok[NX_TXQ_CNT];    // BAR success + BA missed
    uint16_t aggregates_pop_bar_fail[NX_TXQ_CNT];  // BAR failed
    uint16_t aggregates_pop_bar_stuck[NX_TXQ_CNT]; // recover_bar_stuck
    uint16_t aggregates_pop_recover[NX_TXQ_CNT];   // PM12 fix: case 1/2/4 drain pop owning agg_desc
    uint16_t case4_drain_cnt[NX_TXQ_CNT];          // case 4 drain fires (per call)
    uint16_t case4_drain_mpdus[NX_TXQ_CNT];        // total MPDUs drained in case 4
    uint16_t case4_first_agg_cnt[NX_TXQ_CNT];      // case 4 drained mpdu_first with agg_desc
    uint16_t case12_drain_mpdus[NX_TXQ_CNT];       // case 1/2 zeroed/done_hw drain
    uint16_t case12_first_agg_cnt[NX_TXQ_CNT];     // case 1/2 mpdu_first with agg_desc
    // PM12 Phase 5: HW-safe deferred release of orphan agg_desc.
    // Queued on unlink; drained when HW head_ptr clears + DMA idle +
    // grace period (1ms) elapsed.
    uint16_t pending_release_queued[NX_TXQ_CNT];   // total pushed to pending list
    uint16_t pending_release_drained[NX_TXQ_CNT];  // successfully released back to pool
    uint16_t pending_release_skip_busy[NX_TXQ_CNT];// drain attempt blocked by DMA busy / HW head match
    // PM14 instrument: AC_TMO timer arming reliability.
    // PM11e / PM13 observation: when AC_BE wedges, only AC_VO timer fires
    // → recover_stale never runs for AC_BE. Hypothesis is that
    // txl_timer_clear at line 1050 (singleton fexch_done, no next) lacks
    // the safety net that line 1224 (BAR-done) has, so after a clear the
    // subsequent txl_timer_move calls are silent no-ops (move only updates
    // the deadline, does NOT unmask). Counters here separate "timer was
    // armed and fired" from "move was called on a masked timer".
    uint16_t timer_fired[NX_TXQ_CNT];        // AC_TMO IRQ fired for this AC
    uint16_t timer_arm_start[NX_TXQ_CNT];    // txl_timer_start calls
    uint16_t timer_arm_clear[NX_TXQ_CNT];    // txl_timer_clear calls
    uint16_t timer_arm_move[NX_TXQ_CNT];     // txl_timer_move calls (timer was armed)
    uint16_t timer_move_skip[NX_TXQ_CNT];    // txl_timer_move on masked timer (HEALED: re-armed)
    uint16_t timer_backstop_arm[NX_TXQ_CNT]; // backstop poll re-armed a dead timer (queue busy)
    // PM27 instrument: UNLINK_AGG_HEAD_IF_MATCH paths split by guard.
    // skip_gen: agg_desc was recycled (gen mismatch) — HW-safe leak is
    //   correct; slot owned by a new AMPDU now.
    // extract_nonhead: gen ok, agg_desc still in aggregates list but at
    //   non-head position — extracted via co_list_extract (PM27 fix).
    //   Position in SW list doesn't gate HW safety; deferred release in
    //   txl_pending_release_drain still gates by DMA state + grace + HW
    //   head_ptr.
    // skip_head: gen ok but agg_desc not in aggregates list at all —
    //   truly lost (some other path popped it without our knowledge).
    //   This is the residual leak path after PM27 fix.
    uint16_t unlink_skip_gen[NX_TXQ_CNT];        // agg_desc reused (gen mismatch)
    uint16_t unlink_extract_nonhead[NX_TXQ_CNT]; // PM27 fix: extracted at non-head
    uint16_t unlink_skip_head[NX_TXQ_CNT];       // agg_desc not in aggregates list
    // PM28 instrument: HW DMA stall classifier. After PM27, residual leak
    // is AMPDU whose first td is neither DONE_HW nor si==0, blocking
    // case1/2/4 drain. Two paths reach the stuck state:
    // path A: case4 entered (dma_state!=2, !DONE_HW), inner loop broke
    //         immediately on head with si!=0 → no drain progress.
    // path B: case4 NOT entered (dma_state==2 → DMA active), case1/2
    //         broke on head si!=0 !DONE_HW, popped==0 → stale_fail++.
    // last_si captures the offending statinfo so we can match it to
    // TX_STATUS_* bits (or partial PHY error states) and design a
    // force-drain rule.
    uint16_t case_stuck_a[NX_TXQ_CNT];        // case4 entered, head si!=0
    uint16_t case_stuck_b[NX_TXQ_CNT];        // case4 skipped, popped==0
    uint32_t case_stuck_si_last[NX_TXQ_CNT];  // most recent stuck head statinfo
    uint32_t case_stuck_mci2_last[NX_TXQ_CNT];// most recent stuck head macctrlinfo2
    uint8_t  case_stuck_dma_last[NX_TXQ_CNT]; // dma_state at the moment of capture
    // PM29 instrument: track CONSECUTIVE stale_fail fires on the same
    // head_ptr — distinguishes "transient stuck (resolved by BA/BAR)"
    // from "permanent stuck (HW state=2 forever)". Threshold for
    // declaring "stuck": e.g., 5 consecutive (≈80ms at AC_TMO cadence).
    // Also captures GEN_INT_STATUS (0x24B0806C) at the moment of stuck
    // fingerprint — bit 14 (ac1_tx_dma_dead) tells if HW itself
    // acknowledges the DMA died.
    uint16_t consec_stuck_max[NX_TXQ_CNT];    // lifetime max consecutive
    uint16_t consec_stuck_now[NX_TXQ_CNT];    // current run length
    uint32_t gen_int_status_at_stuck;          // GEN_INT_STATUS at last stuck capture
    // PM30 instrument: close-path forensics for class (d) wedge.
    // Class (d) fingerprint: a0_cnt=N a0_desc=1 agg=0 ppdu_cnt=0 post-burst,
    // all PMs (14/27/28/29) silent. Hypothesis vectors:
    //   H1: cfm_evt repush drains MPDU into a NEW agg[0] build; txl_agg_check
    //       sees ppdu_cnt>0 so skips close_pending → timer never armed.
    //   H2: close_pending called → defers → timer armed; cb fires but its
    //       `ppdu_cnt==0 && desc!=NULL` guard fails → cb returns with armed=0
    //       and no retry path → wedge.
    //   H4: repushed MPDU enters agg[0] without ever reaching close_pending,
    //       and no subsequent macif/cfm/fexch/push trigger fires.
    uint32_t agg_cp_calls[NX_TXQ_CNT];        // close_pending invocations
    uint32_t agg_cp_skip_null[NX_TXQ_CNT];    // agg[0].desc == NULL on entry
    uint32_t agg_cp_defer[NX_TXQ_CNT];        // defer branch (skip_count++)
    uint32_t agg_cp_force[NX_TXQ_CNT];        // force-close (txl_agg_finish reached)
    uint32_t agg_hold_cb_total;               // mm_timer cb invocations
    uint32_t agg_hold_cb_guard_fail;          // cb guard miss (ppdu_cnt!=0 OR desc==NULL)
    // DEBUG 2026-07-25: guard_fail conflated two very different things, which
    // is why "81% guard_fail" read as a cause instead of a symptom:
    //   gf_ppdu -- ppdu_cnt >= pipeline_max with a build still open, so the cb
    //              RE-ARMS itself every hold_us. Measured 3.8k/s during the
    //              wedge: a spin, and pure consequence of ppdu_cnt being
    //              pinned at the pipeline budget.
    //   gf_null -- build already closed by another path. Genuine no-op.
    // rearm_max is the longest unbroken re-arm streak (a live wedge never
    // breaks it).
    uint32_t agg_hold_cb_gf_ppdu;
    uint32_t agg_hold_cb_gf_null;
    uint32_t agg_hold_cb_rearm_now;
    uint32_t agg_hold_cb_rearm_max;
    uint32_t agg_push_retry[NX_TXQ_CNT];      // txl_agg_push_mpdu with UMAC_RETRY flag
    // PM32: class (c) force-drain counter. Fires when path_b has seen
    // consec_stuck_now[ac] >= STABLE_STUCK_THRESHOLD (~1.5s of same
    // head_ptr stuck with HW DMA state=2). The original path_b assumes HW
    // will transition out of state=2 on its own — bench evidence shows
    // 687 consecutive misses with no progress. Force-drain pops the head,
    // marks DONE_SW, hands the txdesc to cfm path, and issues newhead.
    // Deferred-release of agg_desc keeps HW writes safe per PM12 rationale.
    uint16_t case_stuck_force[NX_TXQ_CNT];
    // PM34 v3c: class (e) orphan-in-aggregates direct release.
    // Root cause: HW DMA halts mid-AMPDU after completing mpdu 1..K, K<N.
    // Mpdu 1..K go to cfm_list with valid backref via happy-path. case 4 si=0
    // drain drains mpdu K+1..N with `td->lmac.agg_desc=NULL` (defensive against
    // agg_desc-reuse race). mpdu_last (mpdu N) thus loses backref → cfm_evt
    // routes it through singleton path → txl_agg_release never called → agg_desc
    // leaks in pool (user_cnt=1, no holder decrements).
    // PM32 misses this because consec_stuck never accumulates: mpdu 1..K are
    // popped via happy-path (head keeps changing), mpdu K+1 is drained by case
    // 4 before consec_stuck reaches 50.
    // v3c scans txl->aggregates head in recover_stale. If stale chain_time + HW
    // DMA not touching THDs + user_cnt==1 (no other holder), pop and call
    // txl_agg_release directly. No AGG_DONE/ke_evt — those were v1's double-free
    // vector via stale cfm_list backrefs. v3c is race-free because by class (e)
    // signature no member txdesc holds backref to this agg_desc anywhere.
    uint16_t aggregates_stale_drain[NX_TXQ_CNT];
    // PM34 v3c: aging-scan candidate skipped because HW DMA head_ptr matches
    // this AMPDU's a_thd or bar_thd (HW still touching). Defers to PM32 stable-
    // stuck force-drain (class c) or next recover_stale tick when HW has moved.
    uint16_t aggregates_stale_skip_hw[NX_TXQ_CNT];
    // PM34 v3c: aging-scan candidate skipped because user_cnt != 1, meaning
    // some other path holds a chain reference (e.g. agg_desc_prev). Skipping
    // is correct — that other path is responsible for the release. Non-zero
    // value indicates either a v3c invariant violation or a legitimate chained
    // AMPDU still being processed; investigate if seen alongside leak > 0.
    uint16_t aggregates_stale_skip_uc[NX_TXQ_CNT];
    // PM34 root-cause: cfm_evt skipped its is_mpdu_last release because
    // AGG_PENDING_RELEASE was set (UNLINK_AGG_HEAD_IF_MATCH owned the desc
    // via txl_pending_release_drain). Counts the coordination handshake;
    // non-zero is normal under any class (c)/(d) wedge that fires UNLINK.
    uint16_t cfm_skip_pending_release[NX_TXQ_CNT];
    // PM34 root-cause: cfm_evt extracted agg_desc from txl->aggregates
    // before release. Fires for class (e) wedges where HW halt mid-AMPDU
    // left agg_desc in aggregates list with no UNLINK / BAR-done pop. This
    // is the canonical class (e) release point; v3c stale_drain should
    // approach 0 in healthy benches as this picks up the work.
    uint16_t cfm_aggregates_extract[NX_TXQ_CNT];
    // PM33: per-site ppdu_cnt_dec mirror. Pairs with ppdu_inc_site[]
    // (which tracks PPDU_CNT_INC). Class (e) wedge observed under healthy
    // RF showed ppdu_cnt pinned at 25 with inc sites 0=25 5=35784 but only
    // isr ppdu_done=918 — by exposing per-site dec counts we can find the
    // exact inc/dec asymmetry. Site numbering is the second arg of
    // ppdu_cnt_dec() at each call site (see txl_cntrl.c/txl_agg.c/txl_he.c
    // — sites 0..16 occupied as of PM33).
    uint32_t ppdu_dec_site[20];
    // PM36: no-progress force-drain. PM34 v3c uses 100ms chain_time gate
    // to drain class (e) orphans. Bench evidence on LAB02 (28698cb7d) shows
    // tx_mpdu_ok flatlining while aggregates list keeps churning at
    // ~30 push/pop per second — PM34 v3c drains at ~1/s, can't keep up.
    // PM36 lowers the chain_time gate to 10ms whenever tx_mpdu_ok has been
    // static for >= 1s on any AC. Counter records each PM36-triggered
    // drain (PM34 v3c uses the same body; this counter only counts the
    // ones gated by the lowered chain_time threshold).
    uint16_t no_progress_force[NX_TXQ_CNT];
    // Phase 8 / Option B: queue+retry at every txl_cntrl_newhead caller
    // that historically dropped the false return (PM41 enumerated 7 such
    // sites). When newhead returns false (HW DMA state=2), the caller
    // pushes the (hw_desc, ac) onto a per-AC FIFO; cfm_evt and the next
    // chain entry retry from the FIFO so orphaned AMPDUs cannot stall the
    // DMA pipeline. Counters:
    //   queued     -- total push (false-return events)
    //   drained    -- successful retry from FIFO
    //   max_depth  -- high-water mark of queue depth (capacity check)
    //   overflow   -- FIFO full when push attempted (lost slot)
    //   drain_call -- drain function entries (sanity: should track cfm_evt
    //                 fire rate when queue non-empty)
    uint16_t pending_newhead_queued[NX_TXQ_CNT];
    uint16_t pending_newhead_drained[NX_TXQ_CNT];
    uint16_t pending_newhead_stale[NX_TXQ_CNT];
    uint16_t pending_newhead_max_depth[NX_TXQ_CNT];
    uint16_t pending_newhead_overflow[NX_TXQ_CNT];
    uint16_t pending_newhead_drain_call[NX_TXQ_CNT];
    // PM45: HW TX FSM no-advance detector. SW recovery (case 4 dma_idle,
    // PM34 v3c stale_drain) can claim success while HW state machine
    // stays stuck (state=3 waiting for ACK on PHY-underflow-garbled
    // AMPDU). Detector tracks per-AC HW head_ptr across consecutive
    // AC_TMO fires; if head doesn't advance AND tx_mpdu_ok doesn't
    // progress for >= PM45_NO_ADVANCE_THRESHOLD fires (~3s at 635ms
    // cadence), declares HW wedge and triggers KE_EVT_RESET (MAC reset),
    // matching bl_iot_sdk's unconditional ASSERT_REC behavior.
    //   hw_head_prev   -- HW head_ptr at last AC_TMO entry (sentinel 0)
    //   no_advance_now -- consecutive AC_TMO fires with no head advance
    //                     AND no tx_mpdu_ok progress
    //   no_advance_max -- lifetime max
    //   assert_rec_fired -- KE_EVT_RESET triggers attributable to PM45
    uint32_t pm45_hw_head_prev[NX_TXQ_CNT];
    uint16_t pm45_no_advance_now[NX_TXQ_CNT];
    uint16_t pm45_no_advance_max[NX_TXQ_CNT];
    uint16_t pm45_assert_rec_fired[NX_TXQ_CNT];
};
extern struct fhost_statistic fhost_statistic;

/* PM30: expose agg_hold internals so txstat can read live close-path state. */
uint8_t  txl_agg_hold_armed_get(uint8_t ac);
uint32_t txl_agg_hold_skip_count_get(uint8_t ac);

// AMPDU RTS/CTS protection toggle (mirrors ipro_macsw
// me_env.param_tx_ampdu_protect_enable). Non-zero = enable RTS/CTS on every
// AMPDU with CTS-to-self fallback on retry limit. 0 = no protection.
extern uint8_t g_ampdu_protect_enable;

// Safe ppdu_cnt decrement — prevents uint8_t underflow (0→255)
#define PPDU_CNT_DEC(txlist) do { \
    if ((txlist)->ppdu_cnt > 0) { \
        (txlist)->ppdu_cnt--; \
    } else { \
        fhost_statistic.ppdu_underflow_cnt++; \
        fhost_statistic.ppdu_underflow_last_lr = (uint32_t)__builtin_return_address(0); \
    } \
} while (0)

// Per-site ppdu_cnt++ tracker. Pair with ppdu_cnt_dec(site) to find sites
// that inc without matching dec (silent leak that keeps txl_agg_check
// permanently gated on ppdu_cnt > 0). Sites are also enumerated in
// txl_cntrl.c next to ppdu_cnt_dec(). Slot numbers must stay stable.
#define PPDU_CNT_INC(txlist, site) do { \
    (txlist)->ppdu_cnt++; \
    if ((site) < 16) \
        fhost_statistic.ppdu_inc_site[site]++; \
} while (0)

// Diagnostic: dump per-AC TX queue state to fhost_printf. Used by the
// `txl_dump_queues` shell command to figure out where stuck pbufs are
// parked when the sdiowifi pool wedges.
void txl_dump_queues(void);

static inline void ppdu_trace_record(uint8_t ppdu_cnt, uint8_t source)
{
    extern uint32_t hal_machw_time(void);
    uint32_t idx = fhost_statistic.ppdu_trace_idx & 63; // ring buffer
    {
        fhost_statistic.ppdu_trace[idx].time_lo = (uint16_t)hal_machw_time();
        fhost_statistic.ppdu_trace[idx].ppdu_cnt = ppdu_cnt;
        fhost_statistic.ppdu_trace[idx].source = source;
        fhost_statistic.ppdu_trace_idx = idx + 1;
    }
}

// 4-layer A-B trace ring buffer (ISR-safe). LP beacon-RX (IPRO_WIFI_LP_FW)
// never transmits, so shrink this TX-agg debug ring to 16 B. Power-of-2.
// [PERF 07-24] 2048 -> 64 entries (16KB -> 0.5KB) funds hot-code-to-RAM
// relocation. KEEP IN SYNC with dbg_statistic.h.
//
// [08-09] 16 -> 512. Measured 8103 entries/s under UDP UL 40M, so 16 entries
// held 1.97 ms - half an A-MPDU cycle. 512 = 4KB = 63 ms and matches the `p`
// shell command's 1024-unit ceiling exactly (512 x 8 B). See dbg_statistic.h.
// [08-09] Overridable per build (-DAB_TRACE_SIZE=512, see
// cmake/base_sdkConfig.cmake). The DEFAULT stays 16 because it has to fit the
// tightest config: at 8103 entries/s 16 entries hold only 1.97 ms - half an
// A-MPDU cycle - but 512 costs 4 KB of .bss and defconfig_ipro6_sdio has 1,104 B
// above its 97K heap floor. Raising the default globally breaks that build.
// See dbg_statistic.h and RC_P9_0809.md.
#if IPRO_WIFI_LP_FW
#undef  AB_TRACE_SIZE
#define AB_TRACE_SIZE 2
#elif !defined(AB_TRACE_SIZE)
#define AB_TRACE_SIZE 16
#endif
struct ab_trace_entry {
    uint32_t ts;       // hal_machw_time() timestamp (1us)
    uint16_t id;       // trace point ID (layer << 4 | sub)
    uint16_t val;      // payload value
};
struct ab_trace_buf {
    struct ab_trace_entry ring[AB_TRACE_SIZE];
    volatile uint32_t idx;     // write index (wraps via mask)
    uint32_t snapshot_ts;      // last Layer 4 snapshot timestamp
    volatile uint32_t freeze;  // DEBUG: nonzero halts all AB_TRACE writes so a
                               // console ring dump reads one consistent epoch
};
extern struct ab_trace_buf ab_trace;

#define AB_TRACE(tid, tval) do { \
    extern uint32_t hal_machw_time(void); \
    if (!ab_trace.freeze) { \
        uint32_t _i = ab_trace.idx; \
        ab_trace.ring[_i & (AB_TRACE_SIZE - 1)] = \
            (struct ab_trace_entry){hal_machw_time(), (tid), (tval)}; \
        ab_trace.idx = _i + 1; \
    } \
} while (0)

enum ME_PARAM_ID_E {
    ME_PARAM_ID_TX_AMPDU_RETRY_CNT_LIMIT,
    ME_PARAM_ID_TX_AMPDU_PROTECT_ENABLE,
    ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD,
};
enum ME_PARAM_CMD_E {
    ME_PARAM_CMD_SET,
    ME_PARAM_CMD_GET,
};

/// Structure containing the parameters of the @ref ME_PARAM_REQ message
struct me_param_req
{
    /// Indicates the param ID
    enum ME_PARAM_ID_E id;
    enum ME_PARAM_CMD_E cmd;//GET or SET.
    /// payload of the param. Max is 32 Bytes
    uint8_t value[32];//GET or SET through value
};

void tx_desc_init_for_fhost(struct fhost_tx_desc_tag *desc, int seg_cnt, uint32_t *seg_addr, uint16_t *seg_len);

/* Phase 10P Step 5: append a pbd entry to extend the HW DMA chain for
 * multi-MSDU A-MSDU TX from fhost. Returns 0 on success, -1 on bad args. */
int tx_desc_append_pbd_for_fhost(struct fhost_tx_desc_tag *desc, int pbd_idx,
                                  uint32_t seg_addr, uint16_t seg_len);

/* Rate-control snapshot, for the `rc` console command.
 *
 * The RC's tables are private to macsw and ME_RC_STATS_REQ -- the message that
 * used to carry them to fhost -- was removed in v6.41.2, which is why
 * fhost_rc_print_stats_sta() had been #if 0'd out and `rc` printed nothing.
 * Reading them here and letting fhost format them is the replacement.
 *
 * Read from the shell task while the RC runs on the WiFi task. Every field
 * copied is word sized or smaller and the snapshot is only ever used for
 * display, so a torn read shows a stale number rather than corrupting
 * anything. This is a debug instrument, not a control path.
 */
#define MACSW_RC_MAX_SAMPLES    16      /* >= RC_MAX_N_SAMPLE + NX_HE */
#define MACSW_RC_STEPS          4       /* == RATE_CONTROL_STEPS */

struct macsw_rc_sample {
    /// Rate configuration word (same encoding as the policy table)
    uint32_t rate_config;
    uint16_t attempts;
    uint16_t success;
    /// EWMA success probability, RC units (scale by 1000 >> 16 for per-mille)
    uint16_t probability;
    uint8_t  sample_skipped;
};

struct macsw_rc_snapshot {
    struct macsw_rc_sample sample[MACSW_RC_MAX_SAMPLES];
    /// Live policy table -- what the MAC HW is transmitting with right now
    uint32_t hw_step[MACSW_RC_STEPS];
    /// Indexes into sample[] forming the retry chain
    uint8_t  retry_step_idx[MACSW_RC_STEPS];
    uint8_t  no_samples;
    uint8_t  sw_retry_step;
    /// @ref rc_mode_t: 0 active, 1 fixed requested, 2 fixed
    uint8_t  mode;
    uint16_t ampdu_len;
    uint16_t ampdu_packets;
    uint32_t avg_ampdu_len;
    uint16_t trial_sample_wait;
    uint32_t fixed_rate_config;
    uint8_t  format_mod;
    uint8_t  bw_max;
    uint16_t curr_amsdu_len;
    /// Peer MAC, carried here so callers need no macsw station internals
    uint8_t  peer_addr[6];
};

/**
 * Fill @p out for @p sta_idx. Returns 0 on success, -1 if the station has no
 * rate-control or policy-table state yet.
 *
 * Expected throughput per sample is NOT included: the RC computes it into a
 * stack array during its update and never stores it, so there is nothing to
 * read. Do not add a column for it without also storing it.
 */
int macsw_rc_snapshot_get(uint16_t sta_idx, struct macsw_rc_snapshot *out);

/* Advertise (or hide) HE in the association request.
 *
 * me_build_assoc_req() emits the HE capability IE only when the local HE
 * capability flag is set, so clearing it makes the AP fall back to HT for this
 * STA -- on 2.4 GHz that is the only other option, VHT does not apply. That is
 * a DUT-side way to compare HE against HT on ONE AP without reconfiguring it,
 * which matters because the interesting APs here are production kit we cannot
 * touch.
 *
 * Takes effect at the next association: the IE is built when the assoc request
 * is assembled, so set it before wifi_sta_connect.
 *
 * Returns the previous state (0/1), or -1 if the build has no HE at all. */
int macsw_local_he_capa_set(int enable);
int macsw_local_he_capa_get(void);

#include "export_enum.h"
#endif
