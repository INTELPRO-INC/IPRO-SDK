/**
 ****************************************************************************************
 *
 * @file fhost_tx.c
 *
 * @brief Implementation of the fully hosted TX task.
 *
 * Copyright (C) RivieraWaves 2017-2021
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup FHOST_TX
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "export/adapter/rtos.h"
#include "fhost_tx.h"
#include "fhost.h"
#include "mac_frame.h"
#include "export/common/co_endian.h"
#include "fhost_rx.h"
#if RW_MESH_EN
#include "mesh_task.h"
#endif

/* 2026-07-15 SAE-connect fix: host->flags is an in-memory descriptor field that
 * fhost writes and the macsw HW path reads. The two sides use DIFFERENT headers
 * whose TXU_CNTRL_* bit assignments have diverged:
 *
 *   export_macsw.h (this TU): MGMT=bit3, MGMT_NO_CCK=bit4, MGMT_ROBUST=bit7
 *   txu_cntrl.h  (HW path)  : MGMT=bit1, MGMT_NO_CCK=bit2, AMSDU=bit3, ROBUST=bit4
 *
 * So export MGMT (bit3) is read by txu_cntrl.c / txl_cntrl.c / txl_buffer.c as
 * TXU_CNTRL_AMSDU. A host mgmt frame (WPA3-SAE auth commit) tagged with the
 * export value is therefore (a) NOT seen as mgmt and (b) mis-seen as an A-MSDU
 * data frame: the HW TX FSM loads the descriptor but never keys up (stuck at
 * AC_VO state 1), the frame is lifetime-flushed after 2 s, SAE auth times out
 * and the STA never gets an IP. The Phase-11 (2026-05-13) #undef in txl_cntrl.c
 * / txl_agg.c flipped those TUs from the export value to the enum and is what
 * exposed this. Fix at the producer: make fhost emit the internal-enum values
 * so every macsw consumer agrees. (AMSDU stays bit3 via FHOST_TX_AMSDU_BIT and
 * is unaffected — bit1 does not collide with anything fhost sets.) */
#undef  TXU_CNTRL_MGMT
#undef  TXU_CNTRL_MGMT_NO_CCK
#undef  TXU_CNTRL_MGMT_ROBUST
#define TXU_CNTRL_MGMT          CO_BIT(1)  /* == txu_cntrl.h enum */
#define TXU_CNTRL_MGMT_NO_CCK   CO_BIT(2)  /* == txu_cntrl.h enum */
#define TXU_CNTRL_MGMT_ROBUST   CO_BIT(4)  /* == txu_cntrl.h enum */

/* Phase 10P Step 2-4 fix (2026-05-10): mm_timer_set/clear from FHOST_TX
 * context races with MM task's mm_timer_schedule on the shared
 * mm_timer_env.prog list (no internal lock). Wrap call sites in OSAL
 * critical section (rtos_protect/unprotect from rtos_al.h, already
 * pulled in via rtos.h above) to make list mutation atomic against MM
 * task. Use the OSAL wrapper, NOT direct FreeRTOS taskENTER_CRITICAL,
 * so the code stays OS-portable. */

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
struct fhost_tx_env_tag fhost_tx_env;

/// Fake function used to detected too small TX headroom
void net_all_tx_headroom_too_small(void);

/* Forward decls for tx_cfm_lat helpers (definitions further down). */
static void tx_cfm_lat_enter(void *desc);
static void tx_cfm_lat_complete(void *desc);

/* DEBUG (uncommitted): shim so sdiowifi TUs can write the macsw AB_TRACE ring
 * without redeclaring ab_trace (see components/network/sdiowifi/abtrace_dbg.h).
 * Open-coded AB_TRACE body: the macro's hal_machw_time() is macsw-internal
 * inline, not linkable from fhost — use exported inline_hal_machw_time().
 * IRQ-safe, same rules as AB_TRACE itself. */
void abdbg_trace(uint16_t id, uint16_t val)
{
    if (!ab_trace.freeze) {
        uint32_t _i = ab_trace.idx;
        ab_trace.ring[_i & (AB_TRACE_SIZE - 1)] =
            (struct ab_trace_entry){inline_hal_machw_time(), id, val};
        ab_trace.idx = _i + 1;
    }
}

/* F5 cfm batch-lock: hold FHOST_TX_LOCK across the whole txl_cfm_evt confirm
 * batch instead of taking it per-MPDU. Per-MPDU locking lets the prio-31 SMID
 * worker interleave a full inject (~270us, lock held) after EVERY confirmed
 * MPDU (t3 ring trace: 5 back-to-back injects inside one 1.8ms fhcfm stall).
 * With the batch holding the mutex, a blocked worker donates its priority
 * (FreeRTOS mutex priority inheritance) so the batch finishes at donor
 * priority, then the queued injects run as one burst. The reset-defer drain
 * path calls fhost_tx_cfm_push from other contexts; the owner check routes
 * those through the normal per-call lock.
 *
 * WHAT THIS IS WORTH, honestly. +1.20 Mbps median (24.80 -> 25.95), 4/4 pairs
 * positive over 4 boots in a HEALTHY window, hard_fail 0. That is +4.8%. The
 * "+16-26%" credited on 07-23 came from a degraded window and does not
 * reproduce - do not quote it.
 *
 * AND WHAT IT DOES NOT ESTABLISH. The gain is carried by the CYCLE
 * (5.03 -> 4.78ms, fill flat), not by dG3: dG3 -0.13 and dG2a -0.15 are 4/4
 * consistent in sign but each sits inside its own spread, so the pre-registered
 * dG3 mechanism bar was NOT met. Taking this as proof of a cfm lock convoy is
 * therefore unsupported by its own A/B - frames/s across every 07-30 arm is
 * pinned near 2.4k regardless, which points at a per-frame admission cost
 * rather than at this lock. Ship it because it converts, not because it
 * explains the ceiling.
 *
 * Default ON. Poke fhost_tx_cfm_batch_lock_en 0/1 while idle to A/B it. */
volatile uint32_t fhost_tx_cfm_batch_lock_en = 1;
static rtos_task_handle fhost_tx_cfm_batch_owner = NULL;

/* [W1c gate 07-24] cfm batch-end scheduling: while an F5 cfm batch holds the
 * lock, per-MPDU fhost_tx_cfm calls only PEND their AC kick; batch_end issues
 * one fhost_tx_schedule per pending AC before unlocking. Only effective when
 * fhost_tx_cfm_batch_lock_en=1 (the F5 batch provides the brackets).
 * Poke fhost_tx_cfm_sched_batch_en 0/1 while idle for A/B. */
volatile uint8_t fhost_tx_cfm_sched_batch_en = 0;
/* deferred-kick state is declared further down (F1 burst machinery) */
extern volatile uint8_t  fhost_tx_sched_deferred;
extern volatile uint8_t  fhost_tx_sched_pending_ac;
extern volatile uint32_t fhost_tx_sched_kicks_saved;
/* P1 credit-conservation counters are defined further down with the other
 * DEBUG globals; the push site below needs them earlier. */
extern volatile uint32_t fhost_cred_pushed;
extern volatile uint32_t fhost_cred_returned;
extern volatile uint32_t fhost_cred_leak_nodesc;
extern volatile uint32_t fhost_cred_leak_txqdis;
extern volatile uint32_t fhost_cred_leak_status;
extern volatile uint32_t fhost_cred_upd_dropped;
extern volatile int32_t  fhost_cred_upd_pos;
extern volatile int32_t  fhost_cred_upd_neg;
extern volatile uint32_t fhost_cred_upd_n;
extern volatile uint32_t fhost_cred_repairs;
extern volatile uint32_t fhost_cred_repaired;
static void fhost_tx_schedule(int ac);

void fhost_tx_cfm_batch_begin(void)
{
    if (!fhost_tx_cfm_batch_lock_en)
        return;
    FHOST_TX_LOCK();
    fhost_tx_cfm_batch_owner = rtos_get_task_handle();
    if (fhost_tx_cfm_sched_batch_en)
        fhost_tx_sched_deferred = 1;    /* under lock; cfm sites pend kicks */
}

void fhost_tx_cfm_batch_end(void)
{
    if (fhost_tx_cfm_batch_owner &&
        fhost_tx_cfm_batch_owner == rtos_get_task_handle()) {
        /* [W1c] flush deferred kicks while STILL holding the lock
         * (fhost_tx_schedule keeps its runs-under-FHOST_TX_LOCK invariant;
         * fhost_tx_sched_release() would re-take the non-recursive mutex). */
        if (fhost_tx_sched_deferred) {
            uint8_t pending = fhost_tx_sched_pending_ac;
            fhost_tx_sched_deferred   = 0;
            fhost_tx_sched_pending_ac = 0;
            for (int ac = 0; ac < AC_MAX; ac++) {
                if (pending & (1 << ac))
                    fhost_tx_schedule(ac);
            }
        }
        fhost_tx_cfm_batch_owner = NULL;
        FHOST_TX_UNLOCK();
    }
}

/* Phase 7 Step C: sub-bucket counters for tx_desc_init_for_fhost (di) and
 * fhost_tx_schedule (sc). Defined here so fhost_tx_req_do (below) can use the
 * inline helpers. The dur/lock/body buckets live further down with the rest
 * of the tx_trace block. */
static volatile uint32_t tx_trace_di_lt5us   = 0;
static volatile uint32_t tx_trace_di_lt20us  = 0;
static volatile uint32_t tx_trace_di_lt100us = 0;
static volatile uint32_t tx_trace_di_lt500us = 0;
static volatile uint32_t tx_trace_di_ge500us = 0;
static volatile uint32_t tx_trace_di_max_us  = 0;
static volatile uint32_t tx_trace_di_sum_us  = 0;
static volatile uint32_t tx_trace_di_n       = 0;

static volatile uint32_t tx_trace_sc_lt5us   = 0;
static volatile uint32_t tx_trace_sc_lt20us  = 0;
static volatile uint32_t tx_trace_sc_lt100us = 0;
static volatile uint32_t tx_trace_sc_lt500us = 0;
static volatile uint32_t tx_trace_sc_ge500us = 0;
static volatile uint32_t tx_trace_sc_max_us  = 0;
static volatile uint32_t tx_trace_sc_sum_us  = 0;
static volatile uint32_t tx_trace_sc_n       = 0;

static inline void tx_trace_subbucket_di(uint32_t us)
{
    if      (us <    5) tx_trace_di_lt5us++;
    else if (us <   20) tx_trace_di_lt20us++;
    else if (us <  100) tx_trace_di_lt100us++;
    else if (us <  500) tx_trace_di_lt500us++;
    else                tx_trace_di_ge500us++;
    if (us > tx_trace_di_max_us) tx_trace_di_max_us = us;
    tx_trace_di_sum_us += us;
    tx_trace_di_n++;
}

static inline void tx_trace_subbucket_sc(uint32_t us)
{
    if      (us <    5) tx_trace_sc_lt5us++;
    else if (us <   20) tx_trace_sc_lt20us++;
    else if (us <  100) tx_trace_sc_lt100us++;
    else if (us <  500) tx_trace_sc_lt500us++;
    else                tx_trace_sc_ge500us++;
    if (us > tx_trace_sc_max_us) tx_trace_sc_max_us = us;
    tx_trace_sc_sum_us += us;
    tx_trace_sc_n++;
}

/* sc sub-stage decomposition (inside fhost_tx_schedule): isolate where the
 * ~305us per-call cost lives. Three suspects:
 *   deq  = fhost_tx_dequeue_for_push + txdesc field update (descriptor/metadata
 *          access; the uncached-WRAM hypothesis lives here)
 *   qw   = rtos_queue_write(queue_tx) hand-off to the WiFi task (FreeRTOS
 *          critical section; blocks only if queue_tx is full)
 *   ind  = macif_tx_data_ind (ke_evt_set + task notify; once per call)
 * Raw us sums over the 1us HW timer (quantization averages out over N).
 * null = two back-to-back timer reads = measurement floor to subtract. */
static volatile uint32_t sc_deq_sum_us  = 0;   /* per pushed MPDU */
static volatile uint32_t sc_deq_n       = 0;
static volatile uint32_t sc_qw_sum_us   = 0;   /* per pushed MPDU */
static volatile uint32_t sc_qw_n        = 0;
static volatile uint32_t sc_qw_max_us   = 0;
static volatile uint32_t sc_ind_sum_us  = 0;   /* per call (once, when pushed) */
static volatile uint32_t sc_ind_n       = 0;
static volatile uint32_t sc_null_sum_us = 0;   /* per call: back-to-back read floor */
static volatile uint32_t sc_null_n      = 0;
static volatile uint32_t sc_pushed_max  = 0;   /* max MPDUs pushed in one call */
/* CPU-cycle mirrors (mcycle CSR @240MHz -> us): mcycle counts the whole hart
 * (incl. other tasks after a switch and uncached stalls), so wall==cpu-us proves
 * only that the CPU stayed busy across the window, not that this thread ran. */
static volatile uint32_t sc_qw_cyc_us   = 0;   /* qw CPU-us (cycles/240) */
static volatile uint32_t sc_ind_cyc_us  = 0;   /* ind CPU-us (cycles/240) */
static const char *volatile sc_producer_name = "?";  /* who runs fhost_tx_schedule */

/* 2026-07-23 (T2 profile): name the ~100us currently unaccounted inside
 * fhost_tx_req_do (body ~166us = di 15 + sc 49 + ~100 unknown). Three raw-us
 * sub-buckets over the 1us HW timer (quantization averages out over N).
 * Early-return / drop paths do NOT accumulate — parse/queue only tick once the
 * frame actually reaches tx_desc_init. Printed by fhost_tx_trace_print(). */
static volatile uint32_t req_info_sum_us  = 0, req_info_n  = 0;  /* net_buf_tx_info */
static volatile uint32_t req_parse_sum_us = 0, req_parse_n = 0;  /* eth parse + staid + txq lookup */
static volatile uint32_t req_queue_sum_us = 0, req_queue_n = 0;  /* tx_cfm_lat_enter + fhost_tx_queue */

static inline uint32_t sc_rdcycle(void)
{
    uint32_t c;
    __asm__ volatile("csrr %0, mcycle" : "=r"(c));
    return c;
}

/* [W1b gate 07-24] per-frame profiling reads (MAC-timer MMIO + mcycle CSR)
 * cost real us on the hot path (~15-20 reads/frame). Default OFF: buckets
 * stop accumulating; poke g_txprof_en=1 for profiling arms. Functional
 * timeout reads of inline_hal_machw_time() are NOT gated. */
volatile uint8_t g_txprof_en = 0;
static inline uint32_t prof_time(void)  { return g_txprof_en ? inline_hal_machw_time() : 0; }
static inline uint32_t prof_cycle(void) { return g_txprof_en ? sc_rdcycle() : 0; }

/*
 * FUNCTIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Return pointer to fhost_tx_desc_tag from a txdesc pointer
 *
 * @param[in] txdesc TX desc
 * @return pointer to the fhost_tx_desc_tag structure that contains this txdesc
 ****************************************************************************************
 */
#if 0
__INLINE struct fhost_tx_desc_tag *fhost_tx_desc(struct txdesc *txdesc)
{
    void *desc = txdesc - offsetof(struct fhost_tx_desc_tag, txdesc);
    return desc;
}
#endif

#if NX_BEACONING
/**
 ****************************************************************************************
 * @brief Start PS clean up timer for a STA
 *
 * This timer is used to drop packet that remain queued for too long
 * (cf @ref fhost_tx_ps_cleanup_cb)
 * Does nothing if timer is already started.
 *
 * @param[in] sta_id  Station Index
 ****************************************************************************************
 */
void fhost_tx_ps_cleanup_timer_start(int sta_id)
{
    struct fhost_tx_sta_traffic *tfc = &fhost_tx_env.traffic[sta_id];
    uint32_t timer_value;

    if (tfc->ps_cleanup_tmr.time)
        // timer already set
        return;

    timer_value = inline_hal_machw_time() + tfc->ps_max_queue_time;

    // The time value is tested to check whether the timer is started or not
    // so ensure that timer is never configured with time=0
    if (!timer_value)
        timer_value ++;

    mm_timer_set(&tfc->ps_cleanup_tmr, timer_value);
}

/**
 ****************************************************************************************
 * @brief Stop PS clean up timer for a STA
 *
 * Does nothing if timer is not started.
 *
 * @param[in] sta_id  Station Index
 ****************************************************************************************
 */
void fhost_tx_ps_cleanup_timer_stop(int sta_id)
{
    struct fhost_tx_sta_traffic *tfc = &fhost_tx_env.traffic[sta_id];

    if (!tfc->ps_cleanup_tmr.time)
        // timer not active
        return;

    mm_timer_clear(&tfc->ps_cleanup_tmr);
    tfc->ps_cleanup_tmr.time = 0;
}

/**
 ****************************************************************************************
 * @brief Send traffic information update to Wifi Task
 *
 * No need to wait confirmation from Wifi Task for this
 * It also start/stop the PS cleanup timer.
 *
 * @param[in] sta_id    Station Index
 * @param[in] tx_avail  Whether traffic is available or not
 * @param[in] ps_type   Type of PS traffic updated
 ****************************************************************************************
 */
static void fhost_tx_ps_traffic_ind(uint8_t sta_id, bool tx_avail,
                                    enum fhost_tx_ps_type ps_type)
{
    void *req = me_traffic_ind_req_alloc();

    me_traffic_ind_req_set_sta_idx(req, sta_id);
    me_traffic_ind_req_set_tx_avail(req, tx_avail);
    me_traffic_ind_req_set_uapsd(req, ps_type == PS_UAPSD);
    macif_kmsg_push(ME_TRAFFIC_IND_REQ, TASK_ME, req, me_traffic_ind_req_len());

    if (tx_avail)
        fhost_tx_ps_cleanup_timer_start(sta_id);
    else
        fhost_tx_ps_cleanup_timer_stop(sta_id);

    rtos_free(req);
}

#endif // NX_BEACONING

/**
 ****************************************************************************************
 * @brief Queue one buffer in the ready list of a TXQ
 *
 * @param[in] txq    TXQ structure
 * @param[in] desc   Descriptor of the buffer to queue
 * @param[in] retry  True if this is a retry pkt
 ****************************************************************************************
 */
static void fhost_tx_queue(struct fhost_tx_queue_tag *txq,
                           struct fhost_tx_desc_tag *desc,
                           bool retry)
{
    #if NX_BEACONING
    uint16_t sta_id = desc->txdesc.api.host.sta_idx;

    if ((sta_id != INVALID_STA_IDX) && (!retry))
    {
        struct fhost_tx_sta_traffic *tfc = &fhost_tx_env.traffic[sta_id];
        if (tfc->ps_ready[txq->ps_type] >= 0)
        {
            tfc->ps_ready[txq->ps_type]++;
            if (tfc->ps_ready[txq->ps_type] == 1)
                fhost_tx_ps_traffic_ind(sta_id, true, txq->ps_type);
        }

        desc->ctrl.timeout = inline_hal_machw_time() + tfc->ps_max_queue_time;
    }
    #endif

    txq->nb_ready++;
    if (retry)
    {
        co_list_insert_after_fast(&txq->ready,
                                  (txq->last_retry) ? &txq->last_retry->hdr : NULL,
                                  &desc->hdr);
        txq->last_retry = desc;
    }
    else
        co_list_push_back(&txq->ready, &desc->hdr);
}

/**
 ****************************************************************************************
 * @brief Dequeue the oldest buffer from the ready list of a TXQ
 *
 * @param[in] txq  TXQ structure
 * @return The oldest pkt queued for this TXQ (may be NULL)
 ****************************************************************************************
 */
static struct fhost_tx_desc_tag *fhost_tx_dequeue(struct fhost_tx_queue_tag *txq)
{
    struct fhost_tx_desc_tag *desc;

    desc = (struct fhost_tx_desc_tag *)co_list_pop_front(&txq->ready);
    /* 2026-07-25: only count down when something was actually popped. The
     * flush loop in fhost_tx_txq_disable runs `while ((desc = dequeue(txq)))`,
     * so its last call pops NULL from an empty list - and the unconditional
     * decrement took nb_ready from 0 to 65535 on every disable. Seen live on
     * txq[54] (`fhostq` shows nb_ready=65535, status 0x00). A stale 65535
     * makes an empty queue look schedulable (`!(status & TXQ_STOP) &&
     * nb_ready`) and sets txq->limit to 65535; today it is masked because
     * fhost_tx_txq_enable resets nb_ready to 0. */
    if (desc)
        txq->nb_ready--;

    if (desc == txq->last_retry)
        txq->last_retry = NULL;

    return desc;
}

/**
 ****************************************************************************************
 * @brief Dequeue the oldest buffer from the ready list of a TXQ and update its
 * descriptor before pushing it WIFI Task.
 *
 * @param[in] txq  TXQ structure
 * @return The oldest pkt queued for this TXQ
 ****************************************************************************************
 */
static struct fhost_tx_desc_tag *fhost_tx_dequeue_for_push(struct fhost_tx_queue_tag *txq)
{
    struct fhost_tx_desc_tag *desc = fhost_tx_dequeue(txq);
    ASSERT_ERR(desc);

    #if NX_BEACONING
    uint16_t sta_id = desc->txdesc.api.host.sta_idx;
    if (sta_id != INVALID_STA_IDX)
    {
        struct fhost_tx_sta_traffic *tfc = &fhost_tx_env.traffic[sta_id];
        if (tfc->sp_cnt[txq->ps_type] >= 0)
        {
            tfc->sp_cnt[txq->ps_type]--;
            if ((tfc->sp_cnt[txq->ps_type] == 0) &&
                (txq->ps_type == PS_UAPSD))
                desc->txdesc.api.host.flags |= TXU_CNTRL_EOSP;

            if (tfc->sp_cnt[txq->ps_type] ||
                tfc->ps_ready[txq->ps_type])
                desc->txdesc.api.host.flags |= TXU_CNTRL_MORE_DATA;
            else
                fhost_tx_ps_traffic_ind(sta_id, false, txq->ps_type);
        }
    }
    #endif

    // LMAC firmware expects a TID of 0xFF for non Qos frame
    if (desc->txdesc.api.host.tid == TID_MGT)
        desc->txdesc.api.host.tid = 0xFF;

    return desc;
}

/**
 ****************************************************************************************
 * @brief Retrieve the destination station index from the destination MAC address and VIF
 * information in case of MESH.
 *
 * Parse all existing mesh_path of the vif and check if the destination adress is present
 * as Next Hop STA. If it is not the case, create a new path with a MESH_PATH_CREATE_REQ
 * request.
 *
 * @param[in] vif Pointer to the VIF structure
 * @param[in] dst_addr Pointer to the destination MAC address in the Ethernet header
 *
 * @return The station index
 ****************************************************************************************
 */
#if RW_MESH_EN
static uint8_t fhost_tx_get_mesh_staid(struct fhost_vif_tag *vif, struct mac_addr *dst_addr)
{
    /* Path to be used */
    struct fhost_mesh_path *mesh_path;
    mesh_path = (struct fhost_mesh_path *) co_list_pick(&vif->mpath_list);
    bool found = 0;

    while (mesh_path != NULL)
    {
        if (!memcmp(&mesh_path->tgt_mac_addr, dst_addr, MAC_ADDR_LEN))
        {
            found = 1;
            break;
        }
        mesh_path = (struct fhost_mesh_path *) co_list_next((struct co_list_hdr *)mesh_path);
    }

    if (found)
    {
        return mesh_path->nhop_sta;
    }
    else
    {
        struct mesh_path_create_req req;
        req.vif_idx = mac_vif_get_index(vif->mac_vif);
        req.has_orig_addr = false;
        memcpy(&req.tgt_mac_addr, dst_addr, MAC_ADDR_LEN);

        macif_kmsg_push(MESH_PATH_CREATE_REQ, TASK_MESH, &req, sizeof(req));

        return INVALID_STA_IDX;
    }
}
#endif

/**
 ****************************************************************************************
 * @brief Retrieve the destination station index from the destination MAC address and VIF
 * information.
 *
 * @param[in] vif Pointer to the VIF structure
 * @param[in] dst_addr Pointer to the destination MAC address in the Ethernet header
 * @param[in] mgmt_frame Whether Frame is management or not
 *
 * @return The station index
 ****************************************************************************************
 */
uint16_t fhost_tx_get_staid(struct fhost_vif_tag *vif, struct mac_addr *dst_addr, bool mgmt_frame)
{
    uint16_t sta_id = INVALID_STA_IDX;

    switch (mac_vif_get_type(vif->mac_vif))
    {
        case VIF_STA:
            if (mac_vif_get_active(vif->mac_vif)) {
                /* PM46: vif->ap_id is uint8_t, but INVALID_STA_IDX is 0x3FF.
                 * When STA is not yet added (pre-association, e.g. SAE auth
                 * phase), ap_id holds INVALID_STA_IDX truncated to 0xFF.
                 * Translate the truncated sentinel back so the caller's
                 * `sta_id == INVALID_STA_IDX` check works and downstream
                 * ASSERT_ERR(sta_id < STA_MAX || sta_id == INVALID_STA_IDX)
                 * does not panic. */
                sta_id = (vif->ap_id == 0xFF) ? INVALID_STA_IDX : vif->ap_id;
            }
            break;
        case VIF_AP:
            if (MAC_ADDR_GROUP(dst_addr))
                sta_id = VIF_TO_BCMC_IDX(mac_vif_get_index(vif->mac_vif));
            else
                sta_id = vif_mgmt_get_staid(vif->mac_vif, dst_addr);
            break;
        #if RW_MESH_EN
        case VIF_MESH_POINT:
            if (MAC_ADDR_GROUP(dst_addr))
                sta_id = VIF_TO_BCMC_IDX(mac_vif_get_index(vif->mac_vif));
            else if (mgmt_frame)
                sta_id = vif_mgmt_get_staid(vif->mac_vif, dst_addr);
            else
                sta_id = fhost_tx_get_mesh_staid(vif, dst_addr);
            break;
        #endif //RW_MESH_EN
        default:
            break;
    }
    ASSERT_ERR(sta_id < STA_MAX || sta_id == INVALID_STA_IDX);

    return sta_id;
}

/**
 ****************************************************************************************
 * @brief Retrieve the TX queue index to use for a transmission
 *
 * If the sta_id is valid, then the function returns the TXQ associated to the given
 * sta_id/tid pair. In this case vif_idx is not used.
 * If sta_id is not valid, then the function returns the TXQ associated to the VIF and
 * in this case tid is not used
 * If neither sta_id nor vif_idx is valid then NULL is returned.
 *
 * @param[in] vif_idx  Index of the MAC VIF
 * @param[in] sta_id   Index of the destination STA
 * @param[in] tid      TID to use for the transmission
 *
 * @return The associated TXQ or NULL if cannot find the txq
 ****************************************************************************************
 */
static struct fhost_tx_queue_tag *fhost_tx_get_txq(uint8_t vif_idx, uint16_t sta_id,
                                                   uint8_t tid)
{
    uint8_t txq_id;
    uint8_t ac;

    if (sta_id < STA_MAX)
    {
        if (tid > TID_MAX)
            tid = 0;

        txq_id = (sta_id * TID_MAX + tid);
        ac = mac_tid2ac[tid];
    }
    else if (vif_idx < NX_VIRT_DEV_MAX)
    {
        txq_id = (STA_MAX * TID_MAX) + vif_idx;
        ac = AC_VO;
    }
    else
    {
        return NULL;
    }

    ASSERT_ERR(txq_id < FHOST_TXQ_CNT);
    fhost_tx_env.tx_q[txq_id].ac = ac;
    return &fhost_tx_env.tx_q[txq_id];
}


/// Mapping table of access category and traffic ID
static const int fhost_tx_ac2tid[3] = {
    [AC_BK] = TID_2,
    [AC_BE] = TID_0,
    [AC_VI] = TID_5,
};

/**
 ****************************************************************************************
 * @brief return the downgraded tid which corresponds VIF
 *
 * @param[in] vif Pointer to the VIF structure
 * @param[in] tid Current TID
 *
 * @return The downgraded TID
 ****************************************************************************************
 */
static uint8_t fhost_tx_downgrade_ac(struct fhost_vif_tag *vif, uint8_t tid)
{
    int8_t ac = mac_tid2ac[tid];

    while ((vif->acm & CO_BIT(ac)) && (ac > AC_BK))
    {
        ac--;
        tid = fhost_tx_ac2tid[ac];
    }
    return tid;
}

/**
 ****************************************************************************************
 * @brief Add a TX queue to the scheduling process.
 * If the queue is already in the scheduling process, do nothing. If no queue is currently
 * in the scheduling process, the TXQ pushed will be the first scheduled. Otherwise the
 * TXQ is scheduled right after the queue currently scheduled.
 *
 * @param[in]     txq    Pointer to the TX queue to add to the scheduling process
 * @param[in,out] sched  Pointer to the scheduling element
 ****************************************************************************************
 */
static int fhost_tx_add_txq_to_sched(struct fhost_tx_queue_tag *txq,
                                      struct fhost_tx_queue_sched_tag *sched)
{
    // Nothing to do if the queue is already in the scheduling list
    if (txq->status & TXQ_IN_HWQ_LIST) {
        //fhost_printf("[FH]%s  Nothing to do if the queue is already in the scheduling lis (Line:%d)!\r\n", __func__, __LINE__);
        return -4;
    }

    // Check if some TXQs are already part of this scheduling element
    if (sched->current)
    {
        struct fhost_tx_queue_tag *current = sched->current;

        // Insert the TXQ right after the currently scheduled queue
        txq->next = current->next;
        txq->prev = current;
        current->next->prev = txq;
        current->next = txq;
    }
    else
    {
        // No TXQ being scheduled now, add this one
        sched->current = txq;
        txq->next = txq;
        txq->prev = txq;
    }

    // Update the status
    txq->status |= TXQ_IN_HWQ_LIST;
    return 0;
}

/**
 ****************************************************************************************
 * @brief Remove a TX queue from the scheduling process.
 * The TX queue is extracted from the sheduling list. If the TX queue is the one currently
 * scheduled, then the pointer to the current queue is updated.
 *
 * @param[in]     txq    Pointer to the TX queue to remove from the scheduling process
 * @param[in,out] sched  Pointer to the scheduling element
 ****************************************************************************************
 */
static void fhost_tx_del_txq_from_sched(struct fhost_tx_queue_tag *txq,
                                        struct fhost_tx_queue_sched_tag *sched)
{
    // Sanity check - The queue shall be in the scheduling list
    ASSERT_ERR((txq->status & TXQ_IN_HWQ_LIST) || (!txq->status));

    // Check if the TXQ is alone in the scheduling element
    if (txq->next == txq)
    {
        sched->current = NULL;
    }
    else
    {
        // Extract the element from the scheduling list and update the current one
        txq->next->prev = txq->prev;
        txq->prev->next = txq->next;
        if (sched->current == txq)
        {
            sched->current = txq->next;
        }
    }

    // Reset the status of the queue
    txq->status &= ~TXQ_IN_HWQ_LIST;
}

static void fhost_tx_txq_disable(struct fhost_tx_queue_tag *txq)
{
    // Delete txq from sched before disable
    if (txq->status & TXQ_IN_HWQ_LIST)
    {
        fhost_tx_del_txq_from_sched(txq, &fhost_tx_env.sched[txq->ac]);
    }

    // TX queue is now disabled
    txq->status = TXQ_DISABLED;
}

// Push flow diagnostic counters (read via tx_diag shell command)
volatile uint32_t fhost_tx_sched_cnt = 0, fhost_tx_sched_pushed = 0;
volatile uint32_t fhost_tx_cfm_cnt = 0;

/**
 ****************************************************************************************
 * @brief Schedule the TX queues ready on a specific access category.
 *
 * This function goes through the queues attached to the scheduling element of this
 * access category and pushes as many packets as possible to the MAC.
 *
 * @param[in] ac Access category to be scheduled
 ****************************************************************************************
 */
static void fhost_tx_schedule(int ac)
{
    struct fhost_tx_queue_sched_tag *sched = &fhost_tx_env.sched[ac];
    bool pushed = false;
    uint32_t sched_pushed_local = 0;

    fhost_tx_sched_cnt++;

    sc_producer_name = pcTaskGetName(NULL);

    // sc-decomp measurement floor: two back-to-back HW-timer reads
    {
        uint32_t tn0 = prof_time();
        uint32_t tn1 = prof_time();
        sc_null_sum_us += (tn1 - tn0);
        sc_null_n++;
    }

    // Loop until there is a TXQ ready to be scheduled, and there is space in the AC queue
    while (fhost_tx_env.hwq_credits[ac] && sched->current)
    {
        struct fhost_tx_queue_tag *txq = sched->current;

        // Go through the TX queue and push for transmission as many packets as possible
        while (fhost_tx_env.hwq_credits[ac])
        {
            struct txdesc_host *txdesc;
            struct fhost_tx_desc_tag *desc;
            uint32_t t_a, t_b, t_c, cy_b, cy_c;

            // Check if we can still get data from the TX queue
            if (co_list_is_empty(&txq->ready) || (txq->credits <= 0))
            {
                // Queue empty or no credits anymore, remove TXQ from scheduling
                // This will also update the currently scheduled queue
                fhost_tx_del_txq_from_sched(txq, sched);
                break;
            }

            // Stage deq: descriptor/metadata access (uncached-WRAM suspect)
            t_a = prof_time();
            // Get the packet descriptor from the TX queue and update it if needed
            desc = fhost_tx_dequeue_for_push(txq);
            txdesc = &desc->txdesc;
            txdesc->ctrl.hwq = ac;
            t_b = prof_time();

            // Stage qw: push the packet to the WiFi task (wall + CPU cycles)
            cy_b = prof_cycle();
            rtos_queue_write(fhost_tx_env.queue_tx, &txdesc, -1, false);
            cy_c = prof_cycle();
            t_c = prof_time();

            sc_deq_sum_us += (t_b - t_a);
            sc_deq_n++;
            sc_qw_sum_us += (t_c - t_b);
            sc_qw_cyc_us += (cy_c - cy_b) / 240;
            sc_qw_n++;
            if ((t_c - t_b) > sc_qw_max_us)
                sc_qw_max_us = (t_c - t_b);

            // Update the number of credits of the queue and its status if required
            pushed = true;
            sched_pushed_local++;
            fhost_tx_env.hwq_credits[ac]--;
            txq->credits--;
            fhost_cred_pushed++;
            if (txq->credits <= 0)
                txq->status |= TXQ_STOP_FULL;

            #if NX_BEACONING
            if (txq->limit && (--txq->limit == 0))
            {
                fhost_tx_del_txq_from_sched(txq, sched);
                break;
            }
            #endif
        }
    }

    fhost_tx_sched_pushed += sched_pushed_local;
    if (sched_pushed_local > sc_pushed_max)
        sc_pushed_max = sched_pushed_local;

    // Check if we need to signal the pushed packets to the WiFi task
    if (pushed)
    {
        // Stage ind: ke_evt_set + WiFi-task notify (once per call). Wall + CPU
        // cycles. This 3-line notify measures ~220us because it synchronously
        // triggers the WiFi task to run txu_cntrl_push for the pushed MPDU(s).
        uint32_t t_i = prof_time();
        uint32_t cy_i = prof_cycle();
        macif_tx_data_ind(ac);
        uint32_t cy_i2 = prof_cycle();
        sc_ind_sum_us += (prof_time() - t_i);
        sc_ind_cyc_us += (cy_i2 - cy_i) / 240;
        sc_ind_n++;
    } else {
#ifdef FHOST_TX_STATISTIC
        fhost_tx_env.sched_nopush[ac]++;
#endif
    }
}

/**
 ****************************************************************************************
 * @brief Release a Rx buffer pushed by upper layer
 *
 * This is called whether the Tx buffer has been transmitted or discarded.
 *
 * @param[in] buf_rx     Rx Buffer to release
 ****************************************************************************************
 */
static void fhost_tx_release_buf_rx(struct fhost_rx_buf_tag *buf_rx)
{
    if (buf_rx != NULL)
        fhost_rx_buf_push(buf_rx);
}

/**
 ****************************************************************************************
 * @brief Release a buffer pushed by upper layer
 *
 * This is called whether the buffer has been transmitted or discarded.
 *
 * @param[in] net_buf     Net Buffer to release
 * @param[in] tx_status   Buffer TX status as reported by umac (0 if discarded)
 * @param[in] cfm_cb      Confirmation callback for the buffer
 * @param[in] cfm_cb_arg  Confirmation callback private argument
 * @param[in] buf_rx      Rx Buffer to release
 ****************************************************************************************
 */
void fhost_tx_release_buf(net_al_tx_t net_buf, uint32_t tx_status,
                                 cb_fhost_tx cfm_cb, void *cfm_cb_arg,
                                 struct fhost_rx_buf_tag *buf_rx )
{
    if (cfm_cb)
        cfm_cb((uint32_t)net_buf, tx_status & TX_STATUS_ACKNOWLEDGED, cfm_cb_arg);
    net_buf_tx_free(net_buf);

    fhost_tx_release_buf_rx(buf_rx);
}

#if NX_BEACONING
/**
 ****************************************************************************************
 * @brief PS clean up timer callback
 *
 * Check each TXQ for the STA and drop packet that reached their timeout limit.
 * Timer is restarted if PS packet are still queued.
 *
 * @param[in] env  Callback registered parameter, in this case it is the station index
 ****************************************************************************************
 */
static void fhost_tx_ps_cleanup_cb(void *env)
{
    struct fhost_tx_queue_tag *txq;
    struct fhost_tx_sta_traffic *tfc;
    int sta_id = (int)env;
    int ps, tid;
    int txq_cnt = TID_MAX;
    int ps_dropped[PS_TYPE_CNT];

    if (sta_id > STA_MAX)
        return;

    txq = fhost_tx_get_txq(sta_idx_get_inst_nbr(sta_id), sta_id, 0);
    if (!txq)
        return;

    if (sta_id > CFG_STA_MAX)
        txq_cnt = 1;

    memset(ps_dropped, 0, sizeof(ps_dropped));
    tfc = &fhost_tx_env.traffic[sta_id];
    tfc->ps_cleanup_tmr.time = 0;

    for (tid = 0; txq_cnt > 0; txq_cnt--, txq++, tid++)
    {
        struct fhost_tx_desc_tag *desc;

        if (!(txq->status & TXQ_ENABLED))
            return;

        if (!txq->nb_ready)
            continue;

        FHOST_TX_LOCK();
        desc = (struct fhost_tx_desc_tag *)co_list_pick(&txq->ready);
        while (desc)
        {
            if (!inline_hal_machw_time_past(desc->ctrl.timeout))
                break;

            desc = fhost_tx_dequeue(txq);
            if (tfc->sp_cnt[txq->ps_type] > 0)
            {
                ps_dropped[txq->ps_type]++;
                tfc->sp_cnt[txq->ps_type]--;
                if (txq->limit)
                    txq->limit--;
            }
            else if (tfc->ps_ready[txq->ps_type] > 0)
            {
                ps_dropped[txq->ps_type]++;
                tfc->ps_ready[txq->ps_type]--;
            }

            fhost_tx_release_buf(desc->ctrl.buf, 0,
                                 desc->ctrl.cfm_cb, desc->ctrl.cfm_cb_arg,
                                 desc->ctrl.buf_rx);

            desc = (struct fhost_tx_desc_tag *)co_list_pick(&txq->ready);
        }

        // very unlikely
        if (!desc && (txq->status & TXQ_IN_HWQ_LIST))
            fhost_tx_del_txq_from_sched(txq, &fhost_tx_env.sched[txq->ac]);

        FHOST_TX_UNLOCK();
    }


    for (ps = PS_LEGACY; ps < PS_TYPE_CNT; ps++)
    {
        if (ps_dropped[ps] && (tfc->ps_ready[ps] + tfc->sp_cnt[ps]) == 0)
            fhost_tx_ps_traffic_ind(sta_id, false, ps);
        else if ((tfc->ps_ready[ps] > 0) || (tfc->sp_cnt[ps] > 0))
            fhost_tx_ps_cleanup_timer_start(sta_id);
    }
}

#endif // NX_BEACONING

#if NX_AMSDU_TX
/*
 * ============================================================================
 * Phase 10P Path A: real A-MSDU TX batching (Step 2-4)
 * ----------------------------------------------------------------------------
 * Step 2: append/commit eligibility logic (fhost_tx_amsdu_try_append)
 * Step 3: build hostdesc from first held pbuf + queue (commit_locked).
 *         single-MSDU only here; Step 5 adds multi-MSDU framing.
 * Step 4: cfm-path multi-pbuf release (fhost_tx_amsdu_release_batch)
 *
 * NOTE on the AMSDU flag bit: macsw txu_cntrl.h:66 defines
 *   TXU_CNTRL_AMSDU = CO_BIT(3)
 * which numerically overlaps export_macsw.h's TXU_CNTRL_MGMT (also CO_BIT(3)).
 * The two namespaces are disjoint (export header is older, internal is what
 * the HW path reads). Bit 3 is set explicitly in Step 5's multi-MSDU branch,
 * AFTER fhost's MGMT branching has run. Step 2-4 commit_locked handles the
 * count == 1 path only and never sets the bit.
 * ============================================================================
 */
#define FHOST_TX_AMSDU_BIT  CO_BIT(3)

/* Step 6 counters (extern'd via fhost_tx.h for tx_diag shell cmd). */
volatile uint32_t fhost_amsdu_committed = 0;
volatile uint32_t fhost_amsdu_size_hist[FHOST_AMSDU_MAX_BATCH + 1] = {0};
volatile uint32_t fhost_amsdu_close_full = 0;
volatile uint32_t fhost_amsdu_close_timer = 0;
volatile uint32_t fhost_amsdu_drop = 0;
volatile uint32_t fhost_amsdu_release_cnt = 0;

/* Step 7 runtime tunables. Bench tunes via shell or memory poke; defaults
 * match the compile-time values from Step 1/2.
 *  - fhost_amsdu_enabled: master kill-switch. 0 = batching off (Step 1
 *    layout still resident but never triggers). Default 1.
 *  - fhost_amsdu_timeout_us: timer-close period.
 *  - fhost_amsdu_max_batch_run: runtime batch cap (clamped to compile MAX). */
volatile uint32_t fhost_amsdu_timeout_us    = FHOST_AMSDU_TIMEOUT_US;
volatile uint8_t  fhost_amsdu_max_batch_run = FHOST_AMSDU_MAX_BATCH;
/* DEBUG 2026-07-23: runtime override of the per-MPDU payload cap (see the
 * projection check in fhost_tx_amsdu_try_append). Default = the BL616-measured
 * compile-time value; raise via `amsdu payl N` to probe IPRO6 silicon. */
volatile uint32_t fhost_amsdu_payl_max      = 1710;
/* Phase 10P 2026-05-10: default kill-switch ON (en=0) until the bring-up
 * deadlock is fully understood. Pristine 3f9e0a10a (pre-Phase-10P) bench
 * confirms en=0 path is the canonical-working baseline. Bench opt-in via
 * `amsdu en 1` mid-flight to study Path A engagement on SDIO bridge
 * traffic without breaking SDIO RX during WiFi association. */
volatile uint8_t  fhost_amsdu_enabled       = 0;

/* Phase 10P Step 2-4 debug (2026-05-10): per-condition eligibility
 * rejection counters. Bench observed `committed=0` across 30 s x 35M
 * iperf with `en=1`, meaning try_append never held a frame. Add atomic
 * counters to identify which check rejects SDIO bridge traffic. */
volatile uint32_t fhost_amsdu_rej_disabled = 0;
volatile uint32_t fhost_amsdu_rej_type     = 0;
volatile uint32_t fhost_amsdu_rej_flags    = 0;
volatile uint32_t fhost_amsdu_rej_tid      = 0;
volatile uint32_t fhost_amsdu_rej_sta      = 0;
volatile uint32_t fhost_amsdu_rej_vif      = 0;
volatile uint32_t fhost_amsdu_rej_eth      = 0;
volatile uint32_t fhost_amsdu_rej_busy     = 0;
volatile uint32_t fhost_amsdu_rej_full     = 0;
volatile uint32_t fhost_amsdu_rej_size     = 0;  /* HW FIFO cap projection */

/* 2026-05-14 TP investigation: profile the 13.5% pre-MAC drop. Increment
 * at fhost_tx_req_do entry / each early-return; cross-check against
 * iperf-c sent vs MAC mpdu total to localize the drop. */
volatile uint32_t fhost_tx_req_in          = 0;
volatile uint32_t fhost_tx_drop_desc       = 0;
volatile uint32_t fhost_tx_drop_sta        = 0;
volatile uint32_t fhost_tx_drop_txq        = 0;
volatile uint32_t fhost_amsdu_pass         = 0;

/* DEBUG 2026-07-25 P1 (sustained-over-offer zero-copy slot wedge).
 *
 * txq->credits is a free-running accumulator: one credit is taken at push
 * time and given back by the confirmation. Any confirmation path that
 * returns without adding the credit back is therefore a PERMANENT, one-way
 * leak. Once credits <= 0 with nothing in flight the TXQ is deadlocked by
 * construction - no push, so no cfm, so no credit, forever - and every pbuf
 * on its ready list (and every zero-copy SMID slot those pbufs pin) is held
 * for good while the link stays associated.
 *
 * Observed live 2026-07-25: txq[9] credits=-16 status=TXQ_ENABLED|STOP_FULL
 * nb_ready=28, hwq_credits[BE]=32 (full, i.e. zero in flight), TXL queues
 * empty, smid zc inflight=22.
 *
 * cred_pushed - cred_returned is the conservation check: with nothing in
 * flight it MUST be 0. The per-site counters name whichever branch leaks.
 */
volatile uint32_t fhost_cred_pushed        = 0;  /* credits taken at push     */
volatile uint32_t fhost_cred_returned      = 0;  /* credits added back at cfm */
volatile uint32_t fhost_cred_leak_nodesc   = 0;  /* cfm with NULL hostid      */
volatile uint32_t fhost_cred_leak_txqdis   = 0;  /* cfm while TXQ disabled    */
volatile uint32_t fhost_cred_leak_status   = 0;  /* cfm neither DONE nor RTY  */
volatile uint32_t fhost_cred_upd_dropped   = 0;  /* BA offset dropped: TXQ dis*/
volatile int32_t  fhost_cred_upd_pos       = 0;  /* sum of positive BA offsets*/
volatile int32_t  fhost_cred_upd_neg       = 0;  /* sum of negative BA offsets*/
volatile uint32_t fhost_cred_upd_n         = 0;  /* BA offsets applied        */
/* Deadlock repairs performed at enqueue (see the block comment at the repair
 * site in fhost_tx_req_do). In a correct system both stay 0 forever; every
 * nonzero value is a TX-credit drift that would have wedged the queue. */
volatile uint32_t fhost_cred_repairs       = 0;  /* times the repair fired    */
volatile uint32_t fhost_cred_repaired      = 0;  /* credits restored in total */

/* DEBUG perf F1 (uncommitted): batch the TX schedule kick across a CMD53
 * de-agg burst. The SMID host aggregates up to 4 eth frames into one CMD53
 * write; the device de-agg loop (trcver_sdio_ipro6.c smid_rx_process) injects
 * them one-by-one via fhost_tx_start, and each one pays a fhost_tx_schedule()
 * MAC kick (~49us 'sc' sub-bucket). With the hold flag set, fhost_tx_req_do
 * only enqueues the desc + records the AC bit; fhost_tx_sched_release() then
 * issues at most one kick per pending AC at the end of the burst.
 *
 * Only the SDIO worker task ever sets fhost_tx_sched_deferred (via
 * fhost_tx_sched_hold). Other producers (mgmt path, lwIP) that reach
 * fhost_tx_req_do while a burst is in flight would observe deferred==1 and
 * defer too, but their AC bit is caught by the same release() (release kicks
 * EVERY pending AC), so their frames are never stranded. */
volatile uint8_t  fhost_tx_sched_deferred    = 0;   /* set while a de-agg burst is injecting */
volatile uint8_t  fhost_tx_sched_pending_ac  = 0;   /* bitmask of ACs awaiting a kick */
volatile uint32_t fhost_tx_sched_kicks_saved = 0;   /* per-frame schedule() calls skipped */
/* Runtime A/B gate: pokeb to 1 to enable burst-deferred kicks. Default 0
 * (legacy per-frame kicks): 8-arm interleaved A/B 2026-07-23 showed batching
 * saves ~25us/frame of worker CPU but buys NO throughput (saving is ~5% of
 * the 8ms serialization cycle, inside window noise) and the A+B combination
 * trended consistently NEGATIVE (burst arrival at the MAC suspected to hurt
 * aggregate formation). Keep the mechanism for future critical-path work. */
volatile uint8_t  fhost_tx_sched_batch_en    = 0;

/* Begin a deferred de-agg burst: subsequent fhost_tx_req_do enqueue-only, no
 * per-frame MAC kick. Takes NO lock (just a flag) so it never holds
 * FHOST_TX_LOCK across the caller's de-agg loop. */
void fhost_tx_sched_hold(void)
{
    if (fhost_tx_sched_batch_en)
        fhost_tx_sched_deferred = 1;
}

/* End a deferred de-agg burst: issue exactly one kick per pending AC. Called
 * WITHOUT FHOST_TX_LOCK held (the de-agg loop released it after each
 * fhost_tx_start). We take FHOST_TX_LOCK for the whole clear-and-kick so it is
 * atomic w.r.t. fhost_tx_req_do (which sets the pending bits under the same
 * lock): a concurrent producer either runs its req_do fully before this LOCK
 * (its bit is caught here) or fully after this UNLOCK (it sees deferred==0 and
 * kicks normally) -- no bit can be lost, so no frame is stranded.
 * fhost_tx_schedule keeps its "runs under FHOST_TX_LOCK" invariant. Fast
 * (no schedule calls) when nothing pended, and idempotent (mask cleared). */
void fhost_tx_sched_release(void)
{
    uint8_t pending;
    int ac;

    FHOST_TX_LOCK();
    fhost_tx_sched_deferred   = 0;
    pending                   = fhost_tx_sched_pending_ac;
    fhost_tx_sched_pending_ac = 0;
    for (ac = 0; ac < AC_MAX; ac++)
    {
        if (pending & (1 << ac))
            fhost_tx_schedule(ac);
    }
    FHOST_TX_UNLOCK();
}

/* Phase 10P 2026-05-14: deferred-close metrics. Timer cb is now lock-free
 * (post-only); FHOST_TX task runs commit. post_fail counts queue_write
 * failures (queue full); deferred counts successful posts. */
volatile uint32_t fhost_amsdu_close_post_fail = 0;
volatile uint32_t fhost_amsdu_close_deferred  = 0;

static void fhost_tx_amsdu_timer_cb(void *env);
static int  fhost_tx_amsdu_commit_locked(uint8_t vif_idx, uint8_t sta_idx);

static struct fhost_tx_amsdu_batch_tag *
fhost_tx_amsdu_slot(uint8_t vif_idx, uint8_t sta_idx)
{
    if (vif_idx >= NX_VIRT_DEV_MAX) return NULL;
    if (sta_idx >= CFG_STA_MAX)     return NULL;
    return &fhost_tx_env.amsdu_batch[vif_idx][sta_idx];
}

/* Eligibility: data frames only, TID 0, valid unicast STA, IPv4. Skips
 * MGMT/EAPOL/MCAST/BCAST/non-IP. host->flags must already be 0 at this
 * point in fhost_tx_req_do (data path; mgmt has TXU_CNTRL_MGMT set).
 *
 * Phase 10P 2026-05-10: per-check rejection counters added so bench can
 * tell which clause rejects SDIO bridge traffic when committed=0. */
static bool fhost_tx_amsdu_eligible(const struct hostdesc *host,
                                    enum fhost_tx_buf_type type)
{
    if (!fhost_amsdu_enabled)                   { fhost_amsdu_rej_disabled++; return false; }
    if (type != IEEE802_3)                      { fhost_amsdu_rej_type++;     return false; }
    if (host->flags)                            { fhost_amsdu_rej_flags++;    return false; }
    if (host->tid != 0)                         { fhost_amsdu_rej_tid++;      return false; }
    if (host->sta_idx >= CFG_STA_MAX)           { fhost_amsdu_rej_sta++;      return false; }
    if (host->vif_idx >= NX_VIRT_DEV_MAX)       { fhost_amsdu_rej_vif++;      return false; }
    if (co_ntohs(host->ethertype) != LLC_ETHERTYPE_IP) { fhost_amsdu_rej_eth++; return false; }
    fhost_amsdu_pass++;
    return true;
}

/* Effective max batch: runtime tunable clamped to compile-time max.
 * Explicitly static-inline: __INLINE resolves to a plain C99 `inline` in this
 * translation unit, which emits no out-of-line copy, so any call GCC 14 chooses
 * not to inline becomes an undefined external reference at link. */
static inline uint8_t fhost_tx_amsdu_max_batch(void)
{
    uint8_t m = fhost_amsdu_max_batch_run;
    if (m == 0)                          return 1;
    if (m > FHOST_AMSDU_MAX_BATCH)       return FHOST_AMSDU_MAX_BATCH;
    return m;
}

static void fhost_tx_amsdu_arm_timer(struct fhost_tx_amsdu_batch_tag *batch,
                                     uint8_t vif_idx, uint8_t sta_idx)
{
    uint32_t to = fhost_amsdu_timeout_us;
    if (to == 0) to = FHOST_AMSDU_TIMEOUT_US; /* defensive against 0 */
    batch->timer.cb  = fhost_tx_amsdu_timer_cb;
    batch->timer.env = (void *)(uintptr_t)(((uint32_t)vif_idx << 8) | sta_idx);
    /* Phase 10P fix (2026-05-10): mm_timer_set mutates mm_timer_env.prog
     * list without internal locking; designed for MM-task-only context.
     * Calling from FHOST_TX races with mm_timer_schedule -> list corruption
     * -> MM task hang -> SDIO drain dies. rtos_protect (OSAL critical
     * section) makes the list mutation atomic against MM task. */
    {
        uint32_t cs = rtos_protect();
        mm_timer_set(&batch->timer, inline_hal_machw_time() + to);
        rtos_unprotect(cs);
    }
}

/* Phase 10P 2026-05-14: lock-free deferred close.
 *
 * Old cb (a01c9f7b4) ran in MM context and called FHOST_TX_LOCK() ->
 * fhost_tx_amsdu_commit_locked(). When FHOST_TX task held the lock under
 * fhost_tx_req_do (e.g. SDIO bridge IPv4 burst), MM task blocked here
 * arbitrarily long, MM event loop stalled, mm_timer_schedule itself
 * couldn't fire, SDIO drain dependencies wedged -> ipro_sdio_write
 * timeouts on RPi side (139 timeouts in 30 s x 35M iperf, bench-confirmed).
 *
 * New cb just posts FHOST_TX_AMSDU_CLOSE to queue_msg with timeout=0
 * (non-blocking). FHOST_TX task pops it and runs commit_locked under
 * its own lock acquisition in fhost_tx_task dispatch loop. MM context
 * spends microseconds here, never blocks. */
static void fhost_tx_amsdu_timer_cb(void *env)
{
    uint32_t pkd = (uint32_t)(uintptr_t)env;
    struct fhost_tx_msg_tag msg;

    msg.msg_id = FHOST_TX_AMSDU_CLOSE;
    msg.u.amsdu_close.vif_idx = (uint8_t)((pkd >> 8) & 0xFF);
    msg.u.amsdu_close.sta_idx = (uint8_t)(pkd & 0xFF);

    if (rtos_queue_write(fhost_tx_env.queue_msg, &msg, 0, false) != 0)
    {
        /* Queue full: held pbuf will commit on next try_append batch-full
         * path, or stay held until a later append fires another timer. At
         * normal queue depth (64) this should be rare; if frequent, bump
         * FHOST_TX_QUEUE_MSG_ELT_CNT or back off arm rate. */
        fhost_amsdu_close_post_fail++;
        return;
    }
    fhost_amsdu_close_deferred++;
}

/*
 * Phase 10P Step 5: build A-MSDU subframe header for non-first MSDUs.
 *
 * macsw `txu_cntrl_amsdu_hdr_append` (txu_cntrl.c:753) writes ONE amsdu_hdr
 * for packet[0] only. For multi-MSDU TX, fhost must write amsdu_hdr+LLC
 * for packets 1..N-1 in their own pbuf headroom (Phase 10G provides 448 B
 * headroom which leaves >= 22 B before the post-Eth-strip packet_addr).
 *
 * Layout written backwards in pbuf headroom (sub_hdr_addr = where amsdu_hdr
 * lands, payload_addr = original post-Eth-strip data start):
 *   [amsdu_hdr (14)][LLC/SNAP (8)][post-Eth-strip MSDU payload]
 *
 * Returns address of amsdu_hdr (= sub_hdr_addr); subframe length advanced
 * by AMSDU_HDR_LEN + LLC_802_2_HDR_LEN.
 */
#define AMSDU_HDR_LEN  ((uint16_t)sizeof(struct amsdu_hdr))   /* 14 */
#define AMSDU_LLC_LEN  ((uint16_t)LLC_802_2_HDR_LEN)          /* 8  */
#define AMSDU_PREPEND  (AMSDU_HDR_LEN + AMSDU_LLC_LEN)        /* 22 */

/* BL616 silicon TX FIFO cap ~1820 wlan bytes per MPDU (bench 2026-05-14):
 * 1470 B UDP datagrams → 2-subframe AMSDU needs frmlen=3090 → HW truncates
 * to ~1820 wlan, only 1 MSDU + 220 B partial of MSDU[1] reach air.
 * 600 B UDP datagrams → 2-subframe AMSDU frmlen=1350 → HW transmits all 2
 * subframes correctly (sniffer-verified 305 frames @ 1410 wlan).
 *
 * Per [PUSH] trace: frmlen = head_len(60) + payl_len + tail_len(8) + 4.
 * Set safe limit at 1748: payl_len + 72 <= 1820 → payl_len <= 1748.
 * Conservative 1710 leaves 38 B margin for size-class variations.
 *
 * Above this, force count==1 (singleton fallback) to avoid HW truncation. */
#define FHOST_AMSDU_PAYL_MAX  1710

static uint32_t
fhost_tx_amsdu_write_subframe_hdr(uint32_t payload_addr, uint16_t payload_len,
                                  const struct mac_addr *da,
                                  const struct mac_addr *sa,
                                  uint16_t ethertype_be)
{
    /* Write LLC/SNAP at [payload_addr - 8 .. payload_addr - 1].
     * Format: AA AA 03 00 00 00 <ethertype 2B network order>. */
    uint8_t *llc = (uint8_t *)(uintptr_t)(payload_addr - AMSDU_LLC_LEN);
    llc[0] = 0xAA; llc[1] = 0xAA; llc[2] = 0x03;
    llc[3] = 0x00; llc[4] = 0x00; llc[5] = 0x00;
    llc[6] = (uint8_t)(ethertype_be & 0xFF);        /* host->ethertype is */
    llc[7] = (uint8_t)((ethertype_be >> 8) & 0xFF); /* already net order  */

    /* Write amsdu_hdr at [payload_addr - 22 .. payload_addr - 9].
     * len field = LLC + payload (does NOT include amsdu_hdr itself). */
    uint32_t hdr_addr = payload_addr - AMSDU_PREPEND;
    struct amsdu_hdr *hdr = (struct amsdu_hdr *)(uintptr_t)hdr_addr;
    hdr->da  = *da;
    hdr->sa  = *sa;
    hdr->len = co_htons((uint16_t)(payload_len + AMSDU_LLC_LEN));

    return hdr_addr;
}

/* Bail-out path: release every held pbuf via the standard cfm mechanism
 * (cfm_cb + net_buf_tx_free). Called when commit can't proceed and we
 * must drop the batch. Slot resets to IDLE. */
static void fhost_tx_amsdu_drop_held(struct fhost_tx_amsdu_batch_tag *batch,
                                     uint32_t status)
{
    fhost_amsdu_drop++;
    for (int i = 0; i < batch->count; i++)
    {
        fhost_tx_release_buf(batch->net_buf[i], status,
                             batch->cfm_cb[i], batch->cfm_cb_arg[i],
                             batch->buf_rx[i]);
        batch->net_buf[i] = NULL;
    }
    batch->count = 0;
    batch->state = FHOST_AMSDU_IDLE;
}

/* Returns 1 if packet held in slot (caller must NOT continue normal TX,
 * the desc will be queued via commit path). Returns 0 if not eligible OR
 * slot busy -- caller continues with single-packet path.
 *
 * `desc` MUST be the same pointer returned by net_buf_tx_info() in the
 * caller (typically fhost_tx_req_do). commit_locked uses this cached desc
 * directly so it can SKIP a second net_buf_tx_info() call — re-calling on a
 * pbuf whose payload is already shifted by pbuf_header() in the first call
 * asserts inside lwIP and kills the FHOST_TX task. */
static int fhost_tx_amsdu_try_append(net_al_tx_t net_buf,
                                     enum fhost_tx_buf_type type,
                                     const struct hostdesc *host,
                                     struct fhost_tx_desc_tag *desc,
                                     uint32_t seg0_addr, uint16_t seg0_len,
                                     cb_fhost_tx cfm_cb, void *cfm_cb_arg,
                                     struct fhost_rx_buf_tag *buf_rx)
{
    struct fhost_tx_amsdu_batch_tag *batch;
    int i;

    if (!fhost_tx_amsdu_eligible(host, type)) return 0;

    batch = fhost_tx_amsdu_slot(host->vif_idx, host->sta_idx);
    if (!batch)                                  { fhost_amsdu_rej_sta++;  return 0; }
    if (batch->state == FHOST_AMSDU_COMMITTED)   { fhost_amsdu_rej_busy++; return 0; }

    i = batch->count;
    if (i >= fhost_tx_amsdu_max_batch())         { fhost_amsdu_rej_full++; return 0; }

    /* Phase 11 Layer 3b: project umac->payl_len after this append and bail
     * if it would exceed BL616's HW TX FIFO cap (~1820 wlan bytes per MPDU).
     * commit_locked count>1 sets host->packet_len[0] post-strip and
     * host->packet_len[i>0] post-strip + AMSDU_PREPEND. macsw umacdesc_prep
     * strips packet_len[0] back to post-strip then sums all packet_len[i]
     * into payl_len. Predict that sum now, before locking in this MSDU.
     *
     * At i==0 (batch start): pessimistically assume future appends are
     * same-size; reject opening if 2 same-size MSDUs already overflow.
     * Without this, large MSDUs open a batch then every append rejects ->
     * batch sits 2 ms waiting on timer -> 113 useless singleton commits in
     * 10 s instead of fast direct singleton path. */
    {
        uint32_t projected;
        if (i == 0)
        {
            if (fhost_tx_amsdu_max_batch() < 2)
                goto size_ok;                        /* max=1 has no append */
            projected = (uint32_t)seg0_len * 2 + AMSDU_PREPEND;
        }
        else
        {
            projected = batch->packet_len[0];        /* post-strip */
            for (int j = 1; j < i; j++)
                projected += batch->packet_len[j] + AMSDU_PREPEND;
            projected += (uint32_t)seg0_len + AMSDU_PREPEND;
        }
        /* DEBUG 2026-07-23: runtime-tunable cap (was compile-time
         * FHOST_AMSDU_PAYL_MAX). The 1710 value is the BL616 silicon TX FIFO
         * measurement; IPRO6 silicon may take larger MPDUs — `amsdu payl N`
         * probes it live. NOT for commit. */
        {
            extern volatile uint32_t fhost_amsdu_payl_max;
            if (projected > fhost_amsdu_payl_max)
            {
                fhost_amsdu_rej_size++;
                return 0;
            }
        }
    }
size_ok:

    batch->net_buf[i]     = net_buf;
    batch->cfm_cb[i]      = cfm_cb;
    batch->cfm_cb_arg[i]  = cfm_cb_arg;
    batch->buf_rx[i]      = buf_rx;
    batch->packet_addr[i] = seg0_addr;
    batch->packet_len[i]  = seg0_len;
    batch->desc[i]        = desc;
    if (i == 0)
    {
        batch->eth_dest_addr = host->eth_dest_addr;
        batch->eth_src_addr  = host->eth_src_addr;
        batch->ethertype     = host->ethertype;
        batch->state         = FHOST_AMSDU_ACCUMULATING;
        fhost_tx_amsdu_arm_timer(batch, host->vif_idx, host->sta_idx);
    }
    batch->count = i + 1;

    if (batch->count >= fhost_tx_amsdu_max_batch())
    {
        /* Full batch: commit immediately. If commit fails, held pbufs are
         * already released by drop path; report held=1 so caller doesn't
         * re-release. */
        fhost_amsdu_close_full++;
        (void)fhost_tx_amsdu_commit_locked(host->vif_idx, host->sta_idx);
    }
    return 1;
}

/* Build a hostdesc from the held batch, queue it, mark slot COMMITTED.
 * Reuses the FIRST held pbuf's headroom desc.
 *
 * count == 1 -> normal singleton path with no AMSDU bit (Eth hdr stripped
 * here, matching the non-AMSDU FULLY_HOSTED convention).
 * count > 1  -> Step 5 multi-MSDU AMSDU framing: leave Eth hdr on packet[0]
 * (macsw txu_cntrl.c:284-285 strips when AMSDU bit is set), pre-write
 * amsdu_hdr+LLC for MSDUs 1..N-1, extend pbd[] DMA chain. */
static int fhost_tx_amsdu_commit_locked(uint8_t vif_idx, uint8_t sta_idx)
{
    struct fhost_tx_amsdu_batch_tag *batch = fhost_tx_amsdu_slot(vif_idx, sta_idx);
    struct fhost_tx_desc_tag *desc;
    struct hostdesc *host;
    struct fhost_tx_queue_tag *txq;
    void *first_buf;
    uint32_t seg_addr[TX_PBD_CNT];
    uint16_t seg_len[TX_PBD_CNT];
    int seg_cnt;
    uint16_t buf_len;

    /* Phase 11 2026-05-12 debug: trace commit_locked entry BEFORE any
     * early-returns. Shows the batch state seen by commit_locked, which
     * lets us correlate `pass=N committed=0 close_to=N` mismatch with the
     * exact early-return clause. */
    {
        extern volatile uint32_t txu_amsdu_dbg_remaining;
        if (txu_amsdu_dbg_remaining > 0)
        {
            printf("[COMMIT_IN] vi=%u si=%u batch=%p state=%d count=%d\r\n",
                vif_idx, sta_idx, (void *)batch,
                batch ? (int)batch->state : -1,
                batch ? (int)batch->count : -1);
        }
    }

    if (!batch)                                   return -1;
    if (batch->count == 0)                        return -1;
    if (batch->state == FHOST_AMSDU_COMMITTED)    return -1; /* already in flight */

    /* Cancel timer (no-op if not armed / already fired and in this cb).
     * Same mm_timer race concern as arm_timer -> rtos_protect/unprotect. */
    {
        uint32_t cs = rtos_protect();
        mm_timer_clear(&batch->timer);
        rtos_unprotect(cs);
    }

    /* Phase 11 2026-05-12 fix: use cached desc from try_append.
     * Re-calling net_buf_tx_info() here would call pbuf_header() on a pbuf
     * whose payload was already shifted in fhost_tx_req_do's earlier call,
     * which asserts (lwIP guard) -> _exit(1) -> kills FHOST_TX task. The
     * cached desc points at the same pbuf headroom slot the first call set up.
     *
     * cached batch->packet_addr[i]/packet_len[i] are POST-Eth-strip (the
     * strip happened in fhost_tx_req_do before passing seg0 to try_append).
     * For count==1 (singleton): use them directly, no second strip.
     * For count>1 (AMSDU): macsw txu_cntrl.c:284-285 expects packet[0] WITH
     *   Eth hdr present and strips it itself when TXU_CNTRL_AMSDU is set.
     *   Reconstruct pre-strip addr/len by undoing the strip:
     *     pre_strip_addr = post_strip_addr - sizeof(mac_eth_hdr)
     *     pre_strip_len  = post_strip_len  + sizeof(mac_eth_hdr) */
    first_buf = batch->net_buf[0];
    desc = batch->desc[0];
    if (!desc)
    {
        fhost_tx_amsdu_drop_held(batch, 0);
        return -1;
    }

    host = &desc->txdesc.api.host;
    desc->ctrl.buf        = first_buf;
    desc->ctrl.buf_rx     = batch->buf_rx[0];
    desc->ctrl.cfm_cb     = batch->cfm_cb[0];
    desc->ctrl.cfm_cb_arg = batch->cfm_cb_arg[0];
    host->hostid          = desc;
    host->vif_idx         = vif_idx;
    host->sta_idx         = sta_idx;
    host->tid             = 0;
    host->eth_dest_addr   = batch->eth_dest_addr;
    host->eth_src_addr    = batch->eth_src_addr;
    host->ethertype       = batch->ethertype;
    host->sn_for_retry    = 0;

    /* Phase 11 2026-05-12 debug: trace commit_locked past early-returns.
     * Includes cached desc validity check above. */
    {
        extern volatile uint32_t txu_amsdu_dbg_remaining;
        if (txu_amsdu_dbg_remaining > 0)
        {
            printf("[COMMIT] vi=%u si=%u cnt=%u desc=%p addr0=%lx len0=%u\r\n",
                vif_idx, sta_idx, batch->count, (void *)desc,
                (unsigned long)batch->packet_addr[0],
                (unsigned)batch->packet_len[0]);
        }
    }

    if (batch->count == 1)
    {
        /* Single-MSDU: use cached POST-strip addr/len. No second strip. */
        seg_addr[0] = batch->packet_addr[0];
        seg_len[0]  = batch->packet_len[0];
        seg_cnt     = 1;
        buf_len     = batch->packet_len[0];

        host->flags          = 0;
        host->packet_addr[0] = batch->packet_addr[0];
        host->packet_len[0]  = batch->packet_len[0];
        host->packet_cnt     = 1;
    }
    else
    {
        /* Multi-MSDU AMSDU TX (Step 5). packet[0] needs Eth hdr present
         * because macsw txu_cntrl.c:284-285 strips it itself when
         * TXU_CNTRL_AMSDU is set. Reconstruct pre-strip from cached
         * post-strip by adding back sizeof(mac_eth_hdr).
         *
         * packet[1..N-1]: macsw only writes ONE amsdu_hdr; we (fhost)
         * pre-write amsdu_hdr_i + LLC_i in each pbuf's headroom (22 B
         * before the post-Eth-strip packet_addr captured at append). */
        seg_addr[0] = batch->packet_addr[0] - sizeof(struct mac_eth_hdr);
        seg_len[0]  = batch->packet_len[0]  + sizeof(struct mac_eth_hdr);
        seg_cnt     = 1;
        buf_len     = seg_len[0];

        host->flags          = FHOST_TX_AMSDU_BIT;
        host->packet_addr[0] = seg_addr[0]; /* pre-strip; macsw strips */
        host->packet_len[0]  = seg_len[0];
        for (int i = 1; i < batch->count; i++)
        {
            uint32_t hdr_addr = fhost_tx_amsdu_write_subframe_hdr(
                batch->packet_addr[i], batch->packet_len[i],
                &batch->eth_dest_addr, &batch->eth_src_addr,
                batch->ethertype);
            host->packet_addr[i] = hdr_addr;
            host->packet_len[i]  = batch->packet_len[i] + AMSDU_PREPEND;
        }
        host->packet_cnt = batch->count;
    }

    {
        uint32_t t_di_a = prof_time();
        tx_desc_init_for_fhost(desc, seg_cnt, seg_addr, seg_len);

        /* Step 5: extend pbd[] chain to cover MSDUs 1..N-1 from the other
         * pbufs. tx_desc_init_for_fhost set up pbd[0..seg_cnt-1] for the
         * first pbuf only; we append additional pbd entries pointing at
         * each subsequent MSDU's [amsdu_hdr+LLC+payload] region via the
         * tx_desc_append_pbd_for_fhost macsw export helper (struct
         * txl_buffer_tag is opaque to fhost). */
        if (batch->count > 1 && seg_cnt < TX_PBD_CNT)
        {
            int pbd_idx = seg_cnt;
            for (int i = 1; i < batch->count && pbd_idx < TX_PBD_CNT;
                 i++, pbd_idx++)
            {
                tx_desc_append_pbd_for_fhost(desc, pbd_idx,
                                             host->packet_addr[i],
                                             host->packet_len[i]);
            }
        }

        tx_trace_subbucket_di(prof_time() - t_di_a);
    }
    tx_cfm_lat_enter(desc);

    txq = fhost_tx_get_txq(vif_idx, sta_idx, 0);
    if (!txq || !(txq->status & TXQ_ENABLED))
    {
        fhost_tx_amsdu_drop_held(batch, 0);
        return -1;
    }

    fhost_tx_queue(txq, desc, false);

    /* Step 6 stats. Capture cnt for hist before slot reset clobbers it. */
    fhost_amsdu_committed++;
    if (batch->count <= FHOST_AMSDU_MAX_BATCH)
        fhost_amsdu_size_hist[batch->count]++;

    /* Phase 11 2026-05-12 fix: release slot for count==1 path. The cfm hook
     * (fhost_tx_cfm) only release_batch's when host->flags has AMSDU_BIT and
     * packet_cnt > 1; for count==1 (singleton fallback) host->flags=0 so
     * the slot would stay in COMMITTED forever -> subsequent try_appends
     * see busy -> bypass Path A entirely. The first-frame-ever wins, every
     * other frame bypasses. Reset to IDLE here so the next frame can use
     * Path A. desc->ctrl.buf already owns first_buf; cfm releases via ctrl. */
    if (batch->count == 1)
    {
        batch->net_buf[0] = NULL;
        batch->count      = 0;
        batch->state      = FHOST_AMSDU_IDLE;
    }
    else
    {
        batch->state = FHOST_AMSDU_COMMITTED;
    }

    if (!(txq->status & TXQ_STOP))
    {
        uint32_t t_sc_a = prof_time();
        fhost_tx_add_txq_to_sched(txq, &fhost_tx_env.sched[txq->ac]);
        fhost_tx_schedule(txq->ac);
        tx_trace_subbucket_sc(prof_time() - t_sc_a);
    }
    return 0;
}

/* Step 4: invoked from fhost_tx_cfm when host->flags & FHOST_TX_AMSDU_BIT
 * AND host->packet_cnt > 1. Releases MSDUs 1..N-1 (MSDU 0 is released by
 * cfm caller via desc->ctrl which was filled with [0]'s cfm_cb/buf at
 * commit). Resets slot to IDLE.
 *
 * Today this is dead code (Step 2-4 commit never sets AMSDU bit / never
 * commits count > 1) but the hook in fhost_tx_cfm checks for it; Step 5
 * starts producing AMSDU MPDUs and this fires. */
static void fhost_tx_amsdu_release_batch(uint8_t vif_idx, uint8_t sta_idx,
                                         uint32_t status)
{
    struct fhost_tx_amsdu_batch_tag *batch = fhost_tx_amsdu_slot(vif_idx, sta_idx);
    if (!batch)                                  return;
    if (batch->state != FHOST_AMSDU_COMMITTED)   return;

    fhost_amsdu_release_cnt++;
    for (int i = 1; i < batch->count; i++)
    {
        fhost_tx_release_buf(batch->net_buf[i], status,
                             batch->cfm_cb[i], batch->cfm_cb_arg[i],
                             batch->buf_rx[i]);
        batch->net_buf[i] = NULL;
    }
    batch->net_buf[0] = NULL;
    batch->count      = 0;
    batch->state      = FHOST_AMSDU_IDLE;
}
#endif /* NX_AMSDU_TX */

/**
 ****************************************************************************************
 * @brief Push a network buffer to a TX queue.
 *
 * This function is directly call from the network stack thread.
 * It is called with the tx mutex hold
 *
 * @param[in] net_if      Pointer to the net interface for which the packet is pushed
 * @param[in] net_buf     Pointer to the net buffer to transmit.
 * @param[in] type        Type of buffer
 * @param[in] cfm_cb      Confirmation callback for the buffer (Only if type == IEEE802_11)
 * @param[in] cfm_cb_arg  Private argument for confirmation callback
 * @param[in] buf_rx      Pointer to Rx buffer structure
 * @param[in] no_cck      Whether CCK rates must not be used to transmit this frame
 ****************************************************************************************
 */
int fhost_tx_req(net_al_if_t net_if, net_al_tx_t net_buf, enum fhost_tx_buf_type type,
                 cb_fhost_tx cfm_cb, void *cfm_cb_arg, struct fhost_rx_buf_tag* buf_rx,
                 int no_cck)
{
    struct net_al_tx_req req;
    req.net_if = net_if;
    req.net_buf = net_buf;
    req.type = (int)type;
    req.cfm_cb = (void *)cfm_cb;
    req.cfm_cb_arg = (void *)cfm_cb_arg;
    req.buf_rx = (void *) buf_rx;
    req.no_cck = no_cck;

    return net_al_tx_req(req);
}

int fhost_tx_req_do(net_al_if_t net_if, net_al_tx_t net_buf, enum fhost_tx_buf_type type,
                    cb_fhost_tx cfm_cb, void *cfm_cb_arg, struct fhost_rx_buf_tag* buf_rx,
                    int no_cck)
{
    struct fhost_tx_desc_tag *desc;
    struct fhost_tx_ctrl_tag *ctrl;
    struct hostdesc *host;
    struct fhost_tx_queue_tag *txq;
    struct fhost_vif_tag *vif;
    uint32_t seg_addr[TX_PBD_CNT];
    uint16_t seg_len[TX_PBD_CNT];
    int seg_cnt = TX_PBD_CNT;
    uint16_t buf_len;
    uint32_t headroom_len = 0;

    fhost_tx_req_in++;

    /* 2026-07-23 (T2 profile): t0 entry -> req_info (net_buf_tx_info). */
    uint32_t t0 = prof_time();
    desc = net_buf_tx_info(net_buf, &buf_len, &seg_cnt, seg_addr, seg_len, &headroom_len);//len

    if (!desc || !seg_cnt || seg_cnt > TX_PBD_CNT || (headroom_len < sizeof(struct fhost_tx_desc_tag)))
    {
        fhost_tx_drop_desc++;
        fhost_tx_release_buf(net_buf, 0, cfm_cb, cfm_cb_arg, buf_rx);
        fhost_printf("[FH]%s relese this buf(Line:%d), len:%d-%d\r\n", __func__, __LINE__, headroom_len, sizeof(struct fhost_tx_desc_tag));
        return -1;
    }

    uint32_t t1 = prof_time();
    req_info_sum_us += t1 - t0;
    req_info_n++;

    vif = net_if_vif_info(net_if);

    ctrl = &desc->ctrl;
    host = &desc->txdesc.api.host;
    ctrl->buf = net_buf;
    ctrl->buf_rx = buf_rx;
    ctrl->cfm_cb = cfm_cb;
    ctrl->cfm_cb_arg = cfm_cb_arg;
    host->hostid = desc;
    if (no_cck)
        host->flags = TXU_CNTRL_MGMT_NO_CCK;
    else
        host->flags = 0;
    host->vif_idx = mac_vif_get_index(vif->mac_vif);

    if (type == IEEE802_3)
    {
        struct mac_eth_hdr *eth_hdr = (struct mac_eth_hdr *)seg_addr[0];

        // Parse Ethernet header
        host->eth_dest_addr = eth_hdr->da;
        host->eth_src_addr = eth_hdr->sa;
        host->ethertype = eth_hdr->len;

        if (co_ntohs(host->ethertype) == LLC_ETHERTYPE_IP)
        {
            uint8_t dscp;

            // Read DSCP field in the IP header to determine TID (2nd byte after the header)
            if (buf_rx != NULL)
                dscp = *((uint8_t *) seg_addr[1] + 1 );
            else
                dscp = *((uint8_t *)(eth_hdr + 1) + 1);

            host->tid = (dscp & 0xFC) >> 5;
        }
        else if (co_ntohs(host->ethertype) == LLC_ETHERTYPE_EAP_T)
        {
            /* 2026-07-16 4-way-handshake fix: route EAPOL to the high-priority
             * AC_VO (TID_7) instead of AC_BE (tid 0). On IPRO6, after the
             * SAE-auth AC_VO burst, the first AC_BE data frame (EAPOL M2)
             * wedges the AC_BE TX DMA at HW state 1 (fetched, never DONE_HW).
             * The blocking net_l2_send_do then stalls the whole wpa_supplicant
             * handshake thread so it can never process M3 / send M4 -> the AP
             * retransmits M3 and deauths. Handshake frames belong on the
             * high-priority AC anyway (standard EAPOL prioritization). */
            host->tid = TID_7;
        }
        else
        {
            host->tid = 0;
        }

        // and skip it once info have been extracted
        seg_addr[0] += sizeof(*eth_hdr);
        seg_len[0] -= sizeof(*eth_hdr);
        buf_len -= sizeof(*eth_hdr);
    }
    else if (type == IEEE802_11)
    {
        struct mac_hdr *mac_hdr = (struct mac_hdr *)seg_addr[0];
        mac_hdr->seq = 0;
        host->eth_dest_addr = mac_hdr->addr1;
        host->tid = TID_MGT;
        host->flags |= TXU_CNTRL_MGMT;
        #if NX_MFP
        if (mfp_is_robust_frame(mac_hdr->fctl, *((uint8_t *)(mac_hdr + 1))))
            host->flags |= TXU_CNTRL_MGMT_ROBUST;
        #endif
    }

    host->packet_addr[0] = seg_addr[0];
    host->packet_len[0] = buf_len;
#if NX_AMSDU_TX
    // Phase 10D-3: single-MSDU per descriptor (no SDIO RX batching yet).
    // packet_cnt MUST be set: txl_buffer.c:150,205 reads it unconditionally
    // when NX_AMSDU_TX=1; uninit (0) under-allocates buffer (skips tail_len).
    host->packet_cnt = 1;
#endif

    // Get destination STA id
    host->sta_idx = fhost_tx_get_staid(vif, &host->eth_dest_addr, host->flags & TXU_CNTRL_MGMT);
    if (host->sta_idx == INVALID_STA_IDX && !(host->flags & TXU_CNTRL_MGMT)) {
        fhost_tx_drop_sta++;
        fhost_tx_release_buf(net_buf, 0, cfm_cb, cfm_cb_arg, buf_rx);
        return -2;
    }

    // Update TID if necessary
    if (vif->acm)
        host->tid = fhost_tx_downgrade_ac(vif, host->tid);

#ifndef CFG_BCN_AC_MAX
    // XXX hacks here, remove it later
    if (type == IEEE802_3 && (host->tid == TID_4 || host->tid == TID_5))
    {
        host->tid = TID_6;
    }
#endif

    // Check if the queue is active
    txq = fhost_tx_get_txq(host->vif_idx, host->sta_idx, host->tid);
    if (!txq || !(txq->status & TXQ_ENABLED))
    {
        fhost_tx_drop_txq++;
        fhost_tx_release_buf(net_buf, 0, cfm_cb, cfm_cb_arg, buf_rx);
        fhost_printf("[FH]%s relese this buf(Line:%d)!\r\n", __func__, __LINE__);
        return -3;
    }

#if NX_AMSDU_TX
    /* Phase 10P Step 2: A-MSDU TX batching attempt. After all standard
     * checks (sta valid, txq active) so eligibility means packet WILL TX.
     * If held, skip tx_desc_init + queue (commit path does it later).
     *
     * Phase 11 2026-05-12 fix: pass desc (= net_buf_tx_info() result) so
     * commit_locked can reuse it instead of re-shifting the pbuf payload. */
    if (fhost_tx_amsdu_try_append(net_buf, type, host, desc, seg_addr[0], buf_len,
                                  cfm_cb, cfm_cb_arg, buf_rx))
        return 0;
#endif

    #if RW_MESH_EN
    if ((vif->mac_vif->type == VIF_MESH_POINT) &&
        (host->sta_idx < CFG_STA_MAX))
    {
        host->flags |= TXU_CNTRL_USE_4ADDR;
    }

    if ((vif->mac_vif->type == VIF_MESH_POINT) &&
        (vif->is_resending))
    {
        host->flags |= TXU_CNTRL_MESH_FWD;
    }
    #endif

    /* 2026-07-23 (T2 profile): req_parse = t1 -> here (eth parse + staid +
     * txq lookup + amsdu-append attempt). Only reached when the frame commits
     * to tx_desc_init; drop_sta / drop_txq / amsdu-held all returned earlier. */
    {
        uint32_t t2 = prof_time();
        req_parse_sum_us += t2 - t1;
        req_parse_n++;
    }
    {
        uint32_t t_di_a = prof_time();
        tx_desc_init_for_fhost(desc, seg_cnt, seg_addr, seg_len);
        tx_trace_subbucket_di(prof_time() - t_di_a);
    }
    /* 2026-07-23 (T2 profile): req_queue = tx_cfm_lat_enter + fhost_tx_queue. */
    {
        uint32_t t3 = prof_time();
        tx_cfm_lat_enter(desc);
        // Queue descriptor
        fhost_tx_queue(txq, desc, false);
        req_queue_sum_us += prof_time() - t3;
        req_queue_n++;
    }

    /* 2026-07-25: TX-credit deadlock repair.
     *
     * txq->credits is a free-running accumulator - taken at push time, given
     * back by the confirmation, and offset in bulk by every BA agreement
     * add/delete. Any confirmation branch that returns without adding its
     * credit back, or a BA delete offset that lands while the granted credits
     * have already been spent, drives it negative permanently: credits <= 0
     * means TXQ_STOP_FULL, which means no push, which means no confirmation,
     * which means no credit. The queue then holds its ready list forever
     * while the link stays ASSOCIATED - and on the SDIO bridge every held
     * pbuf also pins a zero-copy SMID slot, so the host datapath dies with
     * it. Observed live: txq[9] credits=-16 nb_ready=28, LMAC queues empty,
     * 22 zero-copy slots leaked.
     *
     * The state is self-evidently impossible: credits counts what is in
     * flight, so with nothing in flight at all it cannot be negative. When
     * the whole AC is idle (hwq_credits back at its full value) and the queue
     * still refuses to schedule, the accounting has drifted - re-floor it.
     * This can never mask genuine back-pressure: real back-pressure means
     * frames ARE in flight, so hwq_credits is below full and the repair does
     * not fire. No gate: an unschedulable non-empty queue is always a bug.
     *
     * The hwq_credits read is deliberately unsynchronised - fhost_tx_req_do
     * runs outside FHOST_TX_LOCK, while cfm (which both increments it and can
     * call fhost_tx_schedule) runs under the lock. The only way to misread is
     * to catch the pre-decrement value of a push the cfm task just made, i.e.
     * exactly one frame in flight, so a spurious repair over-credits by one.
     * hwq_credits still caps the AC as a whole, so nothing can overflow;
     * taking the lock on this hot path to close a one-frame window on an
     * error path is not worth it. */
    if ((txq->status & (TXQ_ENABLED | TXQ_STOP)) == (TXQ_ENABLED | TXQ_STOP_FULL) &&
        txq->credits <= 0 && txq->nb_ready &&
        fhost_tx_env.hwq_credits[txq->ac] == get_nx_txdesc_cnt(txq->ac))
    {
        /* Restore the AC's descriptor count, not NX_DEFAULT_TX_CREDIT_CNT.
         * Re-flooring to the bare default while a 32-deep BA agreement is up
         * throttles the queue to 1/8 of its pipeline depth - measured on the
         * injected wedge: 5.1 Mbit/s at 83% loss, against 28.9 Mbit/s healthy.
         * The AC descriptor count is the right ceiling: hwq_credits already
         * caps the AC as a whole, and it is exactly the BA entitlement for the
         * single-data-STA bridge. With several active STAs the worst case is
         * one queue briefly monopolising an AC that was provably idle, which
         * one cfm round resolves - acceptable on an error path that must not
         * exist at all. */
        int16_t lim = (int16_t)get_nx_txdesc_cnt(txq->ac);

        /* txq->credits is int8_t: an AC with more than 127 descriptors would
         * wrap it negative and leave the queue stopped, i.e. repair on every
         * single frame forever. Today the counts are 1/32/1/1/4. */
        if (lim > INT8_MAX)
            lim = INT8_MAX;
        fhost_cred_repairs++;
        fhost_cred_repaired += (uint32_t)(lim - txq->credits);
        txq->credits = lim;
        txq->status &= ~TXQ_STOP_FULL;
    }

    // Check if we can add the TX queue to the schedule list
    if (!(txq->status & TXQ_STOP))
    {
        uint32_t t_sc_a = prof_time();
        fhost_tx_add_txq_to_sched(txq, &fhost_tx_env.sched[txq->ac]);
        /* DEBUG perf F1: during a de-agg burst, enqueue only and record the AC;
         * fhost_tx_sched_release() issues one kick per pending AC at burst end. */
        if (fhost_tx_sched_deferred == 0)
        {
            fhost_tx_schedule(txq->ac);
        }
        else
        {
            fhost_tx_sched_pending_ac |= (uint8_t)(1 << txq->ac);
            fhost_tx_sched_kicks_saved++;
        }
        tx_trace_subbucket_sc(prof_time() - t_sc_a);
    }
    /* desc was committed to txq->ready by fhost_tx_queue() above. Returning
     * non-zero would make net_if_output (net_al.c) pbuf_free the buffer,
     * but the queued desc still gets transmitted later and its cfm path
     * would free the same pbuf again -> LWIP_ASSERT(p->ref > 0). Always
     * return 0 once the desc is enqueued; the TX pipeline drains the queue
     * after TXQ_STOP clears. */
    return 0;
}

/**
 ****************************************************************************************
 * @brief Process a TX confirmation from the MAC.
 *
 * This function is directly called from the MAC thread after processing a CFM event
 * from the MACHW.
 * It is called with the tx mutex hold
 * Depending on the status of the confirmation, the packet might be freed and the
 * TXQ corresponding to the transmission scheduled again for transmission.
 * If the packet needs to be retried, it is pushed back to the MAC immediately.
 *
 * @param[in] txdesc Pointer to the TX descriptor that is confirmed.
 ****************************************************************************************
 */
static void fhost_tx_cfm(struct txdesc *txdesc)
{
    struct fhost_tx_queue_tag *txq;
    struct fhost_tx_desc_tag *desc = txdesc->host.hostid;
    struct fhost_tx_ctrl_tag *ctrl;
    struct hostdesc *host;
    int ac;
    uint32_t status = txdesc->host.cfm.status;
    int16_t credits = txdesc->host.cfm.credits;

    // Guard: stale/corrupted txdesc from recover_stale or txl_cfm_flush
    // may have NULL hostid. Release descriptor and restore HW credits.
    // Use ac from cfm status if available, fallback to AC_BE.
    if (!desc)
    {
        int cfm_ac = AC_BE;
        if (txdesc->host.tid != 0xFF) {
            txq = fhost_tx_get_txq(txdesc->host.vif_idx,
                                    txdesc->host.sta_idx, txdesc->host.tid);
            if (txq)
                cfm_ac = txq->ac;
        }
        fhost_tx_env.hwq_credits[cfm_ac]++;
        /* P1: the HW-queue credit is restored here but txq->credits is NOT -
         * the frame consumed one at push time and never gives it back. */
        fhost_cred_leak_nodesc++;
        tx_txdesc_release(txdesc);
        return;
    }

    ctrl = &desc->ctrl;
    host = &desc->txdesc.api.host;

    tx_cfm_lat_complete(desc);

    if (txdesc->host.tid == 0xFF)
        txdesc->host.tid = TID_MGT;

    txq = fhost_tx_get_txq(txdesc->host.vif_idx, txdesc->host.sta_idx, txdesc->host.tid);
    ASSERT_ERR(txq);

    // Get AC
    ac = txq->ac;

    // New credits available
    fhost_tx_env.hwq_credits[ac]++;

    // Release the MAC descriptor
    tx_txdesc_release(txdesc);

#if NX_AMSDU_TX
    /* Phase 10P Step 4: multi-MSDU cfm path.
     * For AMSDU MPDU: SW_RETRY would replay packet_addr[1..N-1] which we
     * are about to free, so we treat retry-of-AMSDU as drop. DONE/DISABLED
     * paths release MSDU 0 via ctrl below + extra MSDUs via release_batch
     * just before that ctrl release. Today this fires only after Step 5
     * starts producing multi-MSDU MPDUs. */
    if ((host->flags & FHOST_TX_AMSDU_BIT) && host->packet_cnt > 1)
    {
        if (status & TX_STATUS_SW_RETRY_REQUIRED)
        {
            fhost_cred_returned += (uint32_t)credits;
            txq->credits += credits;
            if (txq->credits > 0)
                txq->status &= ~TXQ_STOP_FULL;
            status &= ~TX_STATUS_SW_RETRY_REQUIRED;
            status |= TX_STATUS_DONE;
        }
        fhost_tx_amsdu_release_batch(host->vif_idx, host->sta_idx, status);
    }
#endif

    // Check if the queue is active
    if (!(txq->status & TXQ_ENABLED))
    {
        /* P1: buffer released but the push-time credit is not returned. */
        fhost_cred_leak_txqdis++;
        fhost_tx_release_buf(ctrl->buf, status,
                             ctrl->cfm_cb, ctrl->cfm_cb_arg,
                             ctrl->buf_rx);
    }
    // Check the status of the packet
    else if (status & TX_STATUS_SW_RETRY_REQUIRED)
    {
        fhost_cred_returned += (uint32_t)credits;
        txq->credits += credits;
        if (txq->credits > 0)
        {
            txq->status &= ~TXQ_STOP_FULL;
        }
        if (status & TX_STATUS_RETRY_REQUIRED) {
            uint16_t *flags = &host->flags;
            *flags |= (TXU_CNTRL_REUSE_SN | TXU_CNTRL_RETRY);
            host->sn_for_retry = (status & TX_STATUS_SN_MSK) >> TX_STATUS_SN_OFT;
        }
        fhost_tx_queue(txq, desc, true);

        // Schedule the retry for re-push. Without this, retry frames
        // sit in txq->ready indefinitely — hwq_credits and pool inflate
        // by +1 per stuck retry, stalling the TX pipeline.
        if (!(txq->status & TXQ_STOP) && !co_list_is_empty(&txq->ready))
        {
            fhost_tx_add_txq_to_sched(txq, &fhost_tx_env.sched[ac]);
            /* [W1c] pend the kick during an F5 cfm batch */
            if (fhost_tx_sched_deferred) {
                fhost_tx_sched_pending_ac |= (uint8_t)(1 << ac);
                fhost_tx_sched_kicks_saved++;
            } else {
                fhost_tx_schedule(ac);
            }
        }
    }
    else if (status & TX_STATUS_DONE)
    {
        // Update the number of credits of the queue
        fhost_cred_returned += (uint32_t)credits;
        txq->credits += credits;
        if (txq->credits > 0)
        {
            txq->status &= ~TXQ_STOP_FULL;
        }

        fhost_tx_release_buf(ctrl->buf, status,
                             ctrl->cfm_cb, ctrl->cfm_cb_arg,
                             ctrl->buf_rx);

        // Check if we can add the TX queue to the schedule list
        if (!(txq->status & TXQ_STOP) && !co_list_is_empty(&txq->ready))
        {
            fhost_tx_add_txq_to_sched(txq, &fhost_tx_env.sched[ac]);
            /* [W1c] pend the kick during an F5 cfm batch */
            if (fhost_tx_sched_deferred) {
                fhost_tx_sched_pending_ac |= (uint8_t)(1 << ac);
                fhost_tx_sched_kicks_saved++;
            } else {
                fhost_tx_schedule(ac);
            }
        }
        #if NX_BEACONING
        else if (txq->limit && ((txq->status & TXQ_STOP) == TXQ_STOP_STA_PS))
        {
            fhost_tx_add_txq_to_sched(txq, &fhost_tx_env.sched[ac]);
            if (fhost_tx_sched_deferred) {
                fhost_tx_sched_pending_ac |= (uint8_t)(1 << ac);
                fhost_tx_sched_kicks_saved++;
            } else {
                fhost_tx_schedule(ac);
            }
        }
        #endif // NX_BEACONING
    }
    else
    {
        /* P1: status is neither DONE nor SW_RETRY_REQUIRED. Today this branch
         * did not exist, so the frame returns NEITHER its credit NOR its
         * buffer. Counted only - do not change behaviour in the diagnostic
         * build, or we cannot measure what we are trying to explain. */
        fhost_cred_leak_status++;
    }
    net_al_tx_cfm();
}

/**
 ****************************************************************************************
 * @brief Update the credits of a TX queue, upon the creation or deletion of a BlockAck
 * agreement of the association STA/TID pair.
 *
 * This function is called in the FHOST TX thread after getting the
 * @ref FHOST_TX_CREDITS_UPDATE message that was sent from the MAC thread.
 *
 * @param[in] sta_id Index of the station
 * @param[in] tid Priority
 * @param[in] credits Number of credits to add/remove on the TX queue
 ****************************************************************************************
 */
static void fhost_tx_do_credits_update(uint8_t sta_id, uint8_t tid, int8_t credits)
{
    struct fhost_tx_queue_tag *txq = fhost_tx_get_txq(INVALID_VIF_IDX, sta_id, tid);
    uint8_t ac = txq->ac;

    // Check if queue is active
    if (!(txq->status & TXQ_ENABLED))
    {
        /* P1: a BA offset dropped here is asymmetric - the +offset of an
         * ADDBA can be lost while the -offset of the matching DELBA still
         * lands later, driving credits permanently negative. */
        fhost_cred_upd_dropped++;
        return;
    }

    // Update the credits
    fhost_cred_upd_n++;
    if (credits >= 0)
        fhost_cred_upd_pos += credits;
    else
        fhost_cred_upd_neg += credits;
    txq->credits += credits;
    if (txq->credits > 0)
    {
        txq->status &= ~TXQ_STOP_FULL;
    }
    else
    {
        txq->status |= TXQ_STOP_FULL;
    }

    // Check if we have to add the TX queue to the schedule list
    if (!(txq->status & TXQ_STOP) && !co_list_is_empty(&txq->ready))
    {
        fhost_tx_add_txq_to_sched(txq, &fhost_tx_env.sched[ac]);
    }
    #if NX_BEACONING
    else if (txq->limit && ((txq->status & TXQ_STOP) == TXQ_STOP_STA_PS))
    {
        fhost_tx_add_txq_to_sched(txq, &fhost_tx_env.sched[ac]);
    }
    #endif // NX_BEACONING
    else if (txq->status & TXQ_IN_HWQ_LIST)
    {
        fhost_tx_del_txq_from_sched(txq, &fhost_tx_env.sched[ac]);
    }

    if (fhost_tx_env.sched[ac].current)
        fhost_tx_schedule(ac);
}


/**
 ****************************************************************************************
 * @brief Enable a TXQ queue.
 *
 * Set a TXQ as enabled and reset its internal status
 *
 * @param[in] txq      TXQ to enabled
 * @param[in] ps_type  PS type of the queue (only used for STA's TXQ)
 ****************************************************************************************
 */
static void fhost_tx_txq_enable(struct fhost_tx_queue_tag *txq, int8_t ps_type)
{
    txq->status = TXQ_ENABLED;
    txq->credits = NX_DEFAULT_TX_CREDIT_CNT;
    co_list_init(&txq->ready);
    txq->nb_ready = 0;
    txq->last_retry = NULL;
    #if NX_BEACONING
    txq->ps_type = ps_type;
    txq->limit = 0;
    #endif
}

/**
 ****************************************************************************************
 * @brief Enable the TX queues for the given STA.
 *
 * This function is called in the FHOST TX thread after getting the @ref FHOST_TX_STA_ADD
 * message that was sent from the control thread.
 *
 * @param[in] sta_id Station index to add
 ****************************************************************************************
 */
static void fhost_tx_do_sta_add(uint8_t sta_id)
{
    for (int tid = 0; tid < TID_MAX; tid++)
    {
        struct fhost_tx_queue_tag *txq = fhost_tx_get_txq(INVALID_VIF_IDX, sta_id, tid);
        int8_t ps_type = PS_LEGACY;

        #if NX_BEACONING
        if (sta_idx_get_uapsd_queues(sta_id) & mac_ac2uapsd[mac_tid2ac[tid]])
            ps_type = PS_UAPSD;
        #endif

        fhost_tx_txq_enable(txq, ps_type);
    }

    #if NX_BEACONING
    fhost_tx_env.traffic[sta_id].ps_ready[PS_LEGACY] = -1;
    fhost_tx_env.traffic[sta_id].ps_ready[PS_UAPSD] = -1;
    fhost_tx_env.traffic[sta_id].sp_cnt[PS_LEGACY] = 0;
    fhost_tx_env.traffic[sta_id].sp_cnt[PS_UAPSD] = 0;
    fhost_tx_env.traffic[sta_id].ps_cleanup_tmr.cb = fhost_tx_ps_cleanup_cb;
    fhost_tx_env.traffic[sta_id].ps_cleanup_tmr.env = (void *)((int)sta_id);
    fhost_tx_env.traffic[sta_id].ps_cleanup_tmr.time = 0;
    #endif
}

/**
 ****************************************************************************************
 * @brief Disable the TX queues for the given STA.
 *
 * This function is called in the FHOST TX thread after getting the @ref FHOST_TX_STA_DEL
 * message that was sent from the control thread. It goes through all the TX queues
 * attached to this STA and free the packets pending in those queues.
 *
 * @param[in] sta_id Station index to delete
 ****************************************************************************************
 */
static void fhost_tx_do_sta_del(uint8_t sta_id)
{
    #if NX_BEACONING
    fhost_tx_ps_cleanup_timer_stop(sta_id);
    #endif

    net_al_tx_do_sta_del(sta_id, (release_buf_cb)fhost_tx_release_buf);
    for (int tid = 0; tid < TID_MAX; tid++)
    {
        struct fhost_tx_queue_tag *txq = fhost_tx_get_txq(INVALID_VIF_IDX, sta_id, tid);
        struct fhost_tx_desc_tag *desc;

        // Go through the TX queue and free all the packets
        while ((desc = fhost_tx_dequeue(txq)))
        {
            fhost_tx_release_buf(desc->ctrl.buf, 0,
                                 desc->ctrl.cfm_cb,desc->ctrl.cfm_cb_arg,
                                 desc->ctrl.buf_rx);
        }

        fhost_tx_txq_disable(txq);
    }
}

/**
 ****************************************************************************************
 * @brief Enable the TX queues for the given VIF.
 *
 * This function is called in the FHOST TX thread after receiving the
 * @ref FHOST_TX_VIF_ENABLE message sent from the control thread.
 *
 * @param[in] vif_idx  MAC VIF index
 ****************************************************************************************
 */
static void fhost_tx_do_vif_enable(uint8_t vif_idx)
{
    struct fhost_tx_queue_tag *txq = fhost_tx_get_txq(vif_idx, INVALID_STA_IDX, 0);
    fhost_tx_txq_enable(txq, PS_LEGACY);
}

/**
 ****************************************************************************************
 * @brief Disable the TX queues for the given VIF.
 *
 * This function is called in the FHOST TX thread after receiving the
 * @ref FHOST_TX_VIF_DISABLE message sent from the control thread.
 *
 * @param[in] vif_idx  MAC VIF index
 ****************************************************************************************
 */
static void fhost_tx_do_vif_disable(uint8_t vif_idx)
{
    struct fhost_tx_queue_tag *txq = fhost_tx_get_txq(vif_idx, INVALID_STA_IDX, 0);
    struct fhost_tx_desc_tag *desc;

    // Go through the TX queue and free all the packets
    while ((desc = fhost_tx_dequeue(txq)))
    {
        fhost_tx_release_buf(desc->ctrl.buf, 0,
                             desc->ctrl.cfm_cb, desc->ctrl.cfm_cb_arg,
                             desc->ctrl.buf_rx);
    }

    fhost_tx_txq_disable(txq);
}

/**
 ****************************************************************************************
 * @brief Stop TX queues for a STA that enabled PS mode.
 *
 * @param[in] sta_id  Index of the station
 ****************************************************************************************
 */
static void fhost_tx_do_sta_enter_ps(uint8_t sta_id)
{
    #if NX_BEACONING
    struct fhost_tx_queue_tag *txq = fhost_tx_get_txq(INVALID_VIF_IDX, sta_id, 0);
    struct fhost_tx_sta_traffic *tfc = &fhost_tx_env.traffic[sta_id];
    int tid, ac, ps;

    for (ps = PS_LEGACY; ps < PS_TYPE_CNT; ps++)
    {
        tfc->ps_ready[ps] = 0;
        tfc->sp_cnt[ps] = 0;
    }

    for (tid = 0; tid < TID_MAX; tid++, txq++)
    {
        ac = txq->ac;
        txq->status |= TXQ_STOP_STA_PS;
        if (txq->status & TXQ_IN_HWQ_LIST)
            fhost_tx_del_txq_from_sched(txq, &fhost_tx_env.sched[ac]);
        tfc->ps_ready[txq->ps_type] += txq->nb_ready;
    }

    for (ps = PS_LEGACY; ps < PS_TYPE_CNT; ps++)
    {
        if (tfc->ps_ready[ps])
            fhost_tx_ps_traffic_ind(sta_id, true, ps);
    }
    #endif
}

/**
 ****************************************************************************************
 * @brief (re)start TXQ for a given reason
 *
 * @param[in] sta_id  Index of the station
 * @param[in] status  TXQ status flag
 ****************************************************************************************
 */
static void fhost_tx_sta_txq_start(uint8_t sta_id, int reason)
{
    int tid, ac, ac_to_sched = 0;
    struct fhost_tx_queue_tag *txq = fhost_tx_get_txq(INVALID_VIF_IDX, sta_id, 0);

    for (tid = 0; tid < TID_MAX; tid++, txq++)
    {
        ac = txq->ac;
        #if NX_BEACONING
        if (reason & TXQ_STOP_STA_PS)
            txq->limit = 0;
        #endif
        txq->status &= ~reason;

        if (!(txq->status & TXQ_STOP) && txq->nb_ready)
        {
            fhost_tx_add_txq_to_sched(txq, &fhost_tx_env.sched[ac]);
            ac_to_sched |= CO_BIT(ac);
        }
    }

    for (ac = AC_VO; ac >= AC_BK; ac--)
    {
        if (ac_to_sched & CO_BIT(ac))
            fhost_tx_schedule(ac);
    }
}

/**
 ****************************************************************************************
 * @brief Stop TXQ for a given reason
 *
 * @param[in] sta_id  Index of the station
 ****************************************************************************************
 */
static void fhost_tx_sta_txq_stop(uint8_t sta_id, int reason)
{
    int tid, ac;
    struct fhost_tx_queue_tag *txq = fhost_tx_get_txq(INVALID_VIF_IDX, sta_id, 0);

    if (!txq || !(txq->status & TXQ_ENABLED))
        return;

    for (tid = 0; tid < TID_MAX; tid++, txq++)
    {
        ac = txq->ac;
        txq->status |= reason;

        if (txq->status & TXQ_IN_HWQ_LIST)
            fhost_tx_del_txq_from_sched(txq, &fhost_tx_env.sched[ac]);
    }
}

/**
 ****************************************************************************************
 * @brief Restart TX queues for a STA that disabled PS mode.
 *
 * @param[in] sta_id  Index of the station
 ****************************************************************************************
 */
static void fhost_tx_do_sta_exit_ps(uint8_t sta_id)
{
    #if NX_BEACONING
    struct fhost_tx_sta_traffic *tfc = &fhost_tx_env.traffic[sta_id];
    int ps = 0;

    for (ps = PS_LEGACY; ps < PS_TYPE_CNT; ps++)
    {
        if (tfc->ps_ready[ps] != -1)
            fhost_tx_ps_traffic_ind(sta_id, false, ps);
        tfc->ps_ready[ps] = -1;
        tfc->sp_cnt[ps] = 0;
    }

    fhost_tx_sta_txq_start(sta_id, TXQ_STOP_STA_PS);

    #endif
}

/**
 ****************************************************************************************
 * @brief Call the function given as argument on all STA associated to the current VIF.
 *
 * @param[in] vif_idx  Index of the VIF
 * @param[in] control  Function pointer
 ****************************************************************************************
 */
static void fhost_tx_txq_vif_foreach_sta(uint8_t vif_idx, void (*control)(uint8_t, int))
{
    struct fhost_vif_tag *fhost_vif = NULL;
    fhost_vif = &fhost_env.vif[vif_idx];

    void *sta = co_list_pick(mac_vif_get_sta_list(fhost_vif->mac_vif));

    while (sta != NULL)
    {
        control(sta_get_staid(sta), TXQ_STOP_VIF_PS);
        sta = sta_get_list_next(sta);
    }
}

/**
 ****************************************************************************************
 * @brief Stop all TX queues of the VIF.
 *
 * This function is called in the FHOST TX thread after receiving the
 * @ref FHOST_TX_VIF_ENTER_PS message sent from the control thread.
 *
 * @param[in] vif_idx  Index of the VIF
 ****************************************************************************************
 */
static void fhost_tx_do_vif_enter_ps(uint8_t vif_idx)
{
    struct fhost_tx_queue_tag *txq = fhost_tx_get_txq(vif_idx, INVALID_STA_IDX, 0);

    if (txq && (txq->status & TXQ_ENABLED))
    {
        txq->status |= TXQ_STOP_VIF_PS;
        if (txq->status & TXQ_IN_HWQ_LIST)
            fhost_tx_del_txq_from_sched(txq, &fhost_tx_env.sched[txq->ac]);
    }

    fhost_tx_txq_vif_foreach_sta(vif_idx, fhost_tx_sta_txq_stop);
}

/**
 ****************************************************************************************
 * @brief Start all TX queues of the VIF.
 *
 * This function is called in the FHOST TX thread after receiving the
 * @ref FHOST_TX_VIF_EXIT_PS message sent from the control thread.
 *
 * @param[in] vif_idx  Index of the VIF
 ****************************************************************************************
 */
static void fhost_tx_do_vif_exit_ps(uint8_t vif_idx)
{
    struct fhost_tx_queue_tag *txq = fhost_tx_get_txq(vif_idx, INVALID_STA_IDX, 0);

    txq->status &= ~TXQ_STOP_VIF_PS;
    if (!(txq->status & TXQ_STOP) && txq->nb_ready)
    {
        fhost_tx_add_txq_to_sched(txq, &fhost_tx_env.sched[txq->ac]);
        fhost_tx_schedule(txq->ac);
    }

    fhost_tx_txq_vif_foreach_sta(vif_idx, fhost_tx_sta_txq_start);
}

/**
 ****************************************************************************************
 * @brief Configure STA's TXQ in order to push requested packet
 *
 *  This function is called in the FHOST TX thread after receiving the
 *  @ref FHOST_TX_PS_TRAFFIC_REQ message sent from the control thread.
 *
 * @param[in] sta_id   Index of the station
 * @param[in] req_cnt  Number of packet requested (0 means all)
 * @param[in] ps_type  Type of PS traffic requested
 ****************************************************************************************
 */
static void fhost_tx_do_ps_traffic_req(uint8_t sta_id, int req_cnt,
                                       enum fhost_tx_ps_type ps_type)
{
    #if NX_BEACONING
    struct fhost_tx_queue_tag *txq = fhost_tx_get_txq(INVALID_VIF_IDX, sta_id, TID_MGT);
    struct fhost_tx_sta_traffic *tfc = &fhost_tx_env.traffic[sta_id];
    int tid, sp_size = 0, ac, ac_to_sched = 0;;

    if (tfc->ps_ready[ps_type] == -1)
    {
        // ignore traffic request if STA is not in PS mode
#ifdef CFG_ENABLE_AP_PS
        ASSERT_WARN(0);
#endif
        return;
    }

    //bugfix: check sp_cnt before processing
    if (tfc->sp_cnt[ps_type] != 0) {
        return;
    }

    for (tid = TID_MGT; tid >= 0; tid--, txq--)
    {
        if (!txq->nb_ready || (ps_type != txq->ps_type))
            continue;

        if (!req_cnt || (txq->nb_ready < req_cnt))
            txq->limit = txq->nb_ready;
        else
            txq->limit = req_cnt;

        ac = txq->ac;

        if ((txq->status & TXQ_STOP) == TXQ_STOP_STA_PS)
        {
            fhost_tx_add_txq_to_sched(txq, &fhost_tx_env.sched[ac]);
            ac_to_sched |= CO_BIT(ac);
        }

        sp_size += txq->limit;
        if (req_cnt)
        {
            req_cnt -= txq->limit;
            if (!req_cnt)
                break;
        }
    }

    tfc->sp_cnt[ps_type] = sp_size;
    tfc->ps_ready[ps_type] -= sp_size;

    for (ac = AC_MAX; ac >= AC_BK; ac--)
    {
        if (ac_to_sched & CO_BIT(ac))
        {
            fhost_tx_schedule(ac);
        }
    }
    #endif
}

/**
 ****************************************************************************************
 * @brief Push Tx buffer to a TX queueSend Tx buffer and push RX buffer to the WiFi task
 *
 * @param[in] buf Pointer to the Rx buffer
 ****************************************************************************************
 */
static void fhost_tx_do_resend_req(struct fhost_rx_buf_tag* buf)
{
    struct fhost_vif_tag *fhost_vif = NULL;
    uint8_t vif_idx = (buf->info.flags & RX_FLAGS_VIF_INDEX_MSK) >> RX_FLAGS_VIF_INDEX_OFT;

    fhost_vif = fhost_env.mac2fhost_vif[vif_idx];
    #if RW_MESH_EN
    fhost_vif->is_resending = true;
    #endif
    fhost_tx_req(fhost_vif->net_if, buf->info_tx.net_buf, IEEE802_3, NULL, NULL, buf, 0);
    #if RW_MESH_EN
    fhost_vif->is_resending = false;
    #endif
}

/**
 ****************************************************************************************
 * @brief TX task implementation.
 ****************************************************************************************
 */
static RTOS_TASK_FCT(fhost_tx_task)
{
    fhost_task_ready(TX_TASK);

    for (;;)
    {
        struct fhost_tx_msg_tag msg;

        // Wait for a message on the queue
        rtos_queue_read(fhost_tx_env.queue_msg, &msg, -1, false);

        FHOST_TX_LOCK();
        switch (msg.msg_id)
        {
            case FHOST_TX_CREDITS_UPDATE:
                fhost_tx_do_credits_update(msg.u.credits_upd.sta_id,
                                           msg.u.credits_upd.tid,
                                           msg.u.credits_upd.credits);
                break;

            case FHOST_TX_STA_ADD:
                fhost_tx_do_sta_add(msg.u.sta.sta_id);
                break;

            case FHOST_TX_STA_DEL:
                fhost_tx_do_sta_del(msg.u.sta.sta_id);
                break;

            case FHOST_TX_VIF_ENABLE:
                fhost_tx_do_vif_enable(msg.u.vif.vif_idx);
                break;

            case FHOST_TX_VIF_DISABLE:
                fhost_tx_do_vif_disable(msg.u.vif.vif_idx);
                break;

            case FHOST_TX_STA_ENTER_PS:
                fhost_tx_do_sta_enter_ps(msg.u.sta.sta_id);
                break;

            case FHOST_TX_STA_EXIT_PS:
                fhost_tx_do_sta_exit_ps(msg.u.sta.sta_id);
                break;

            case FHOST_TX_PS_TRAFFIC_REQ:
                fhost_tx_do_ps_traffic_req(msg.u.ps_traffic.sta_id, msg.u.ps_traffic.req_cnt,
                                           msg.u.ps_traffic.ps_type);
                break;

            case FHOST_TX_RESEND_REQ:
                fhost_tx_do_resend_req(msg.u.resend.rx_buf);
                break;

            case FHOST_TX_VIF_ENTER_PS:
                fhost_tx_do_vif_enter_ps(msg.u.vif.vif_idx);
                break;

            case FHOST_TX_VIF_EXIT_PS:
                fhost_tx_do_vif_exit_ps(msg.u.vif.vif_idx);
                break;

#if NX_AMSDU_TX
            case FHOST_TX_AMSDU_CLOSE:
            {
                /* Phase 10P 2026-05-14: deferred batch close. Timer cb
                 * posted us; commit under FHOST_TX_LOCK (already held by
                 * dispatch loop above). */
                uint8_t vi = msg.u.amsdu_close.vif_idx;
                uint8_t si = msg.u.amsdu_close.sta_idx;
                struct fhost_tx_amsdu_batch_tag *batch =
                    fhost_tx_amsdu_slot(vi, si);
                if (batch &&
                    batch->state == FHOST_AMSDU_ACCUMULATING &&
                    batch->count > 0)
                    fhost_amsdu_close_timer++;
                (void)fhost_tx_amsdu_commit_locked(vi, si);
                break;
            }
#endif

            default:
                // Unknown message ID
                ASSERT_ERR(0);
                break;
        }
        FHOST_TX_UNLOCK();
    }
}

int fhost_tx_init(void)
{
    // Ensure the required headroom is enough to store fhost_tx_desc_tag
    if ((sizeof(struct fhost_tx_desc_tag) + 3) > NET_AL_TX_HEADROOM)
    {
        net_all_tx_headroom_too_small();
    }

    // Reset the environment
    memset(&fhost_tx_env, 0, sizeof(fhost_tx_env));

    net_al_tx_init();

    // Create the message queue
    if (rtos_queue_create(sizeof(struct fhost_tx_msg_tag), FHOST_TX_QUEUE_MSG_ELT_CNT,
                          &fhost_tx_env.queue_msg))
    {
        return 1;
    }

    // Create the TX queue
    /* [W3a 07-24] +32 headroom: producers on the WiFi task itself (cfm
     * retry re-push under F5, inject-ring refill) write this queue while its
     * consumer macif_tx_evt runs on the SAME task — a full queue would
     * self-deadlock behind a blocking write. Worst case = hwq credits (32)
     * + inject-drain budget (16) < NX_TXDESC_CNT + 32. Cost: 32 pointers. */
    if (rtos_queue_create(sizeof(void *), NX_TXDESC_CNT + 32, &fhost_tx_env.queue_tx))
    {
        return 2;
    }

    // Create the TX mutex
    if (rtos_mutex_create(&fhost_tx_env.tx_lock))
    {
        return 3;
    }

    // Create the TX task
    if (rtos_task_create(fhost_tx_task, "TX", TX_TASK, FHOST_TX_STACK_SIZE, NULL,
                         fhost_tx_priority, NULL))
    {
        return 4;
    }

    // Initialize per HW-queue credits
    for (int i = 0; i < NX_TXQ_CNT; i++)
    {
        fhost_tx_env.hwq_credits[i] = get_nx_txdesc_cnt(i);
    }

#if NX_BEACONING
    // Phase 10D-3: pre-init traffic[] to avoid the BSS-zero race that lets
    // fhost_tx_queue's "ps_ready >= 0" check (line 215) arm the cleanup
    // timer with cb still NULL. ps_ready BSS-zero (=0) satisfies ">=0", so
    // the FIRST enqueue calls fhost_tx_ps_traffic_ind -> timer_start ->
    // mm_timer_set BEFORE fhost_tx_do_sta_add (line 1188-1192) sets cb.
    // Latent race even in baseline; AMSDU enable shifts struct sizes
    // enough to make it actually fire (mm_timer_schedule -> NULL cb call).
    for (int sta = 0; sta < STA_MAX; sta++)
    {
        fhost_tx_env.traffic[sta].ps_ready[PS_LEGACY] = -1;
        fhost_tx_env.traffic[sta].ps_ready[PS_UAPSD]  = -1;
        fhost_tx_env.traffic[sta].ps_cleanup_tmr.cb   = fhost_tx_ps_cleanup_cb;
        fhost_tx_env.traffic[sta].ps_cleanup_tmr.env  = (void *)((int)sta);
    }
#endif

    return 0;
}

volatile uint32_t tx_start_cnt = 0, tx_start_ok = 0;
volatile uint32_t tx_cfm_cnt = 0, tx_cfm_ok = 0;

/*
 * ============================================================================
 * tx_trace: per-call timing ring for fhost_tx_start (cliff debug)
 * ----------------------------------------------------------------------------
 * Distinguishes two root-cause classes when LWIP -> wifi throughput cliffs:
 *   (1) producer idle     -> call_interval grows (>10ms / >80ms bins)
 *   (2) fhost_tx_start blocks -> dur_lock / dur_total grows
 * Single writer (LWIP tcpip thread); racy with shell reader tolerated.
 * ============================================================================
 */
/* [P4 funding 07-24] 64 -> 16 recent-sample entries (16B each): only the
 * raw dump depth shrinks, aggregate histograms untouched. Frees 768B of
 * ram_tcm .bss for the DL hot-code OCRAM set (heap floor 80K enforced).
 * Must stay a power of two (index uses & (SZ-1)). */
#define TX_TRACE_RING_SZ 16u
struct tx_trace_entry {
    uint32_t t_enter_us;
    uint32_t dur_lock_us;    /* FHOST_TX_LOCK acquisition latency */
    uint32_t dur_total_us;   /* entry -> exit */
    int16_t  ret;
    uint16_t reserved;
};
static volatile struct tx_trace_entry tx_trace_ring[TX_TRACE_RING_SZ];
static volatile uint32_t tx_trace_idx = 0;
static volatile uint32_t tx_trace_last_enter_us = 0;

static volatile uint32_t tx_trace_intv_lt1ms  = 0;
static volatile uint32_t tx_trace_intv_lt10ms = 0;
static volatile uint32_t tx_trace_intv_lt80ms = 0;
static volatile uint32_t tx_trace_intv_ge80ms = 0;
static volatile uint32_t tx_trace_intv_max_us = 0;

/* Phase 7: fine-grained buckets for D45 long-tail. dur=total, lock=lock-acq,
 * body=total-lock. Boundaries match tx_profile scheme so cross-correlation is
 * trivial. Coarse intv_* buckets above stay for cliff (>10ms) detection. */
static volatile uint32_t tx_trace_dur_lt5us   = 0;
static volatile uint32_t tx_trace_dur_lt20us  = 0;
static volatile uint32_t tx_trace_dur_lt100us = 0;
static volatile uint32_t tx_trace_dur_lt500us = 0;
static volatile uint32_t tx_trace_dur_ge500us = 0;
static volatile uint32_t tx_trace_dur_max_us  = 0;
static volatile uint32_t tx_trace_dur_sum_us  = 0;

static volatile uint32_t tx_trace_lock_lt5us   = 0;
static volatile uint32_t tx_trace_lock_lt20us  = 0;
static volatile uint32_t tx_trace_lock_lt100us = 0;
static volatile uint32_t tx_trace_lock_lt500us = 0;
static volatile uint32_t tx_trace_lock_ge500us = 0;
static volatile uint32_t tx_trace_lock_max_us  = 0;
static volatile uint32_t tx_trace_lock_sum_us  = 0;

static volatile uint32_t tx_trace_body_lt5us   = 0;
static volatile uint32_t tx_trace_body_lt20us  = 0;
static volatile uint32_t tx_trace_body_lt100us = 0;
static volatile uint32_t tx_trace_body_lt500us = 0;
static volatile uint32_t tx_trace_body_ge500us = 0;
static volatile uint32_t tx_trace_body_max_us  = 0;
static volatile uint32_t tx_trace_body_sum_us  = 0;

/* Phase 7 Step C: drill body into desc-init vs sched sub-stages.
 * di = tx_desc_init_for_fhost (writes HW-visible descriptor)
 * sc = fhost_tx_add_txq_to_sched + fhost_tx_schedule (kicks MAC HW)
 * Counters declared near top of file (before fhost_tx_req_do call sites). */

int fhost_tx_start(net_al_if_t net_if, net_al_tx_t net_buf,
                   cb_fhost_tx cfm_cb, void *cfm_cb_arg)
{
    uint32_t t_enter = prof_time();
    uint32_t t_locked;
    uint32_t dur_lock_us;
    uint32_t dur_total_us;
    int ret;
    uint32_t slot;

    tx_start_cnt++;
    FHOST_TX_LOCK();
    t_locked = prof_time();
    dur_lock_us = t_locked - t_enter;
    ret = fhost_tx_req(net_if, net_buf, IEEE802_3, cfm_cb, cfm_cb_arg, NULL, 0);
    FHOST_TX_UNLOCK();
    dur_total_us = prof_time() - t_enter;
    tx_start_ok++;

    slot = (tx_trace_idx++) & (TX_TRACE_RING_SZ - 1);
    tx_trace_ring[slot].t_enter_us   = t_enter;
    tx_trace_ring[slot].dur_lock_us  = dur_lock_us;
    tx_trace_ring[slot].dur_total_us = dur_total_us;
    tx_trace_ring[slot].ret          = (int16_t)ret;

    {
        uint32_t last = tx_trace_last_enter_us;
        tx_trace_last_enter_us = t_enter;
        if (last != 0) {
            uint32_t intv = t_enter - last;
            if      (intv <  1000) tx_trace_intv_lt1ms++;
            else if (intv < 10000) tx_trace_intv_lt10ms++;
            else if (intv < 80000) tx_trace_intv_lt80ms++;
            else                   tx_trace_intv_ge80ms++;
            if (intv > tx_trace_intv_max_us) tx_trace_intv_max_us = intv;
        }
    }

    if      (dur_total_us <    5) tx_trace_dur_lt5us++;
    else if (dur_total_us <   20) tx_trace_dur_lt20us++;
    else if (dur_total_us <  100) tx_trace_dur_lt100us++;
    else if (dur_total_us <  500) tx_trace_dur_lt500us++;
    else                          tx_trace_dur_ge500us++;
    if (dur_total_us > tx_trace_dur_max_us) tx_trace_dur_max_us = dur_total_us;
    tx_trace_dur_sum_us += dur_total_us;

    if      (dur_lock_us <    5) tx_trace_lock_lt5us++;
    else if (dur_lock_us <   20) tx_trace_lock_lt20us++;
    else if (dur_lock_us <  100) tx_trace_lock_lt100us++;
    else if (dur_lock_us <  500) tx_trace_lock_lt500us++;
    else                         tx_trace_lock_ge500us++;
    if (dur_lock_us > tx_trace_lock_max_us) tx_trace_lock_max_us = dur_lock_us;
    tx_trace_lock_sum_us += dur_lock_us;

    {
        uint32_t dur_body_us = dur_total_us - dur_lock_us;
        if      (dur_body_us <    5) tx_trace_body_lt5us++;
        else if (dur_body_us <   20) tx_trace_body_lt20us++;
        else if (dur_body_us <  100) tx_trace_body_lt100us++;
        else if (dur_body_us <  500) tx_trace_body_lt500us++;
        else                         tx_trace_body_ge500us++;
        if (dur_body_us > tx_trace_body_max_us) tx_trace_body_max_us = dur_body_us;
        tx_trace_body_sum_us += dur_body_us;
    }

    return ret;
}

/*
 * ============================================================================
 * tx_cfm_lat: round-trip latency from fhost_tx_req_do enqueue to fhost_tx_cfm
 * ----------------------------------------------------------------------------
 * Parallel hash map keyed by desc pointer. At enqueue time we stash
 * inline_hal_machw_time() into slot; at cfm entry we compute delta and
 * bucket it. Collisions (two descs hashing to same slot) just overwrite
 * and the second cfm sees a miss (counted). With 128 slots and <32
 * in-flight desc simultaneously, collisions are rare.
 * ----------------------------------------------------------------------------
 * OFF BY DEFAULT. The map costs 1 KB of OCRAM and, more to the point, the
 * enqueue side writes two words per MPDU on the TX hot path for a number no
 * shipping build ever reads. The runtime g_txprof_en gate stops the writes but
 * not the KB. Build it with -DTX_CFM_LAT_PROFILE; the print/reset entry points
 * stay linkable either way (ipro6le_demo calls them).
 * ============================================================================
 */
#ifdef TX_CFM_LAT_PROFILE
#define TX_CFM_LAT_SLOTS 128u
struct tx_cfm_lat_slot { void *desc; uint32_t t_enter_us; };
static volatile struct tx_cfm_lat_slot tx_cfm_lat_map[TX_CFM_LAT_SLOTS];

static volatile uint32_t tx_cfm_lat_lt1ms   = 0;
static volatile uint32_t tx_cfm_lat_lt10ms  = 0;
static volatile uint32_t tx_cfm_lat_lt80ms  = 0;
static volatile uint32_t tx_cfm_lat_lt500ms = 0;
static volatile uint32_t tx_cfm_lat_ge500ms = 0;
static volatile uint32_t tx_cfm_lat_max_us  = 0;
static volatile uint32_t tx_cfm_lat_samples = 0;
static volatile uint32_t tx_cfm_lat_miss    = 0;

#define CFM_LAT_HASH(p) ((((uintptr_t)(p)) >> 4) & (TX_CFM_LAT_SLOTS - 1))

static void tx_cfm_lat_enter(void *desc)
{
    if (desc && g_txprof_en) {
        uint32_t h = CFM_LAT_HASH(desc);
        tx_cfm_lat_map[h].desc = desc;
        tx_cfm_lat_map[h].t_enter_us = inline_hal_machw_time();
    }
}

static void tx_cfm_lat_complete(void *desc)
{
    if (!desc || !g_txprof_en) return;
    uint32_t h = CFM_LAT_HASH(desc);
    if (tx_cfm_lat_map[h].desc == desc) {
        uint32_t lat = inline_hal_machw_time() - tx_cfm_lat_map[h].t_enter_us;
        tx_cfm_lat_samples++;
        if      (lat <   1000) tx_cfm_lat_lt1ms++;
        else if (lat <  10000) tx_cfm_lat_lt10ms++;
        else if (lat <  80000) tx_cfm_lat_lt80ms++;
        else if (lat < 500000) tx_cfm_lat_lt500ms++;
        else                   tx_cfm_lat_ge500ms++;
        if (lat > tx_cfm_lat_max_us) tx_cfm_lat_max_us = lat;
        tx_cfm_lat_map[h].desc = NULL;
    } else {
        tx_cfm_lat_miss++;
    }
}

void fhost_tx_cfm_lat_print(void)
{
    printf("[CFMLAT] samples=%lu miss=%lu max_us=%lu "
           "<1ms=%lu <10ms=%lu <80ms=%lu <500ms=%lu >=500ms=%lu\r\n",
           (unsigned long)tx_cfm_lat_samples,
           (unsigned long)tx_cfm_lat_miss,
           (unsigned long)tx_cfm_lat_max_us,
           (unsigned long)tx_cfm_lat_lt1ms,
           (unsigned long)tx_cfm_lat_lt10ms,
           (unsigned long)tx_cfm_lat_lt80ms,
           (unsigned long)tx_cfm_lat_lt500ms,
           (unsigned long)tx_cfm_lat_ge500ms);
}

void fhost_tx_cfm_lat_reset(void)
{
    uint32_t i;
    for (i = 0; i < TX_CFM_LAT_SLOTS; i++) {
        tx_cfm_lat_map[i].desc = NULL;
        tx_cfm_lat_map[i].t_enter_us = 0;
    }
    tx_cfm_lat_lt1ms = tx_cfm_lat_lt10ms = 0;
    tx_cfm_lat_lt80ms = tx_cfm_lat_lt500ms = 0;
    tx_cfm_lat_ge500ms = 0;
    tx_cfm_lat_max_us = 0;
    tx_cfm_lat_samples = 0;
    tx_cfm_lat_miss = 0;
    printf("tx_cfm_lat reset\r\n");
}
#else /* !TX_CFM_LAT_PROFILE */
static void tx_cfm_lat_enter(void *desc)    { (void)desc; }
static void tx_cfm_lat_complete(void *desc) { (void)desc; }

void fhost_tx_cfm_lat_print(void)
{
    printf("[CFMLAT] not built (rebuild with -DTX_CFM_LAT_PROFILE)\r\n");
}

void fhost_tx_cfm_lat_reset(void)
{
}
#endif /* TX_CFM_LAT_PROFILE */

void fhost_tx_trace_print(void)
{
    uint32_t idx = tx_trace_idx;
    uint32_t count = (idx < TX_TRACE_RING_SZ) ? idx : TX_TRACE_RING_SZ;
    uint32_t start = idx - count;
    uint32_t i;

    printf("[TRACE] entries=%lu total_calls=%lu now=%lu\r\n",
           (unsigned long)count,
           (unsigned long)idx,
           (unsigned long)inline_hal_machw_time());
    printf("# i   t_enter_us  dur_lock  dur_total  ret\r\n");
    for (i = 0; i < count; i++) {
        uint32_t n = start + i;
        uint32_t slot = n & (TX_TRACE_RING_SZ - 1);
        printf("[%02lu] %10lu %7lu %8lu %4d\r\n",
               (unsigned long)i,
               (unsigned long)tx_trace_ring[slot].t_enter_us,
               (unsigned long)tx_trace_ring[slot].dur_lock_us,
               (unsigned long)tx_trace_ring[slot].dur_total_us,
               (int)tx_trace_ring[slot].ret);
    }
    printf("[TRACE] intv(us): <1ms=%lu <10ms=%lu <80ms=%lu >=80ms=%lu max=%lu\r\n",
           (unsigned long)tx_trace_intv_lt1ms,
           (unsigned long)tx_trace_intv_lt10ms,
           (unsigned long)tx_trace_intv_lt80ms,
           (unsigned long)tx_trace_intv_ge80ms,
           (unsigned long)tx_trace_intv_max_us);
    {
        uint32_t n = tx_trace_idx ? tx_trace_idx : 1;
        printf("[TRACE] dur  (us): <5=%lu <20=%lu <100=%lu <500=%lu >=500=%lu max=%lu mean=%lu\r\n",
               (unsigned long)tx_trace_dur_lt5us,
               (unsigned long)tx_trace_dur_lt20us,
               (unsigned long)tx_trace_dur_lt100us,
               (unsigned long)tx_trace_dur_lt500us,
               (unsigned long)tx_trace_dur_ge500us,
               (unsigned long)tx_trace_dur_max_us,
               (unsigned long)(tx_trace_dur_sum_us / n));
        printf("[TRACE] lock (us): <5=%lu <20=%lu <100=%lu <500=%lu >=500=%lu max=%lu mean=%lu\r\n",
               (unsigned long)tx_trace_lock_lt5us,
               (unsigned long)tx_trace_lock_lt20us,
               (unsigned long)tx_trace_lock_lt100us,
               (unsigned long)tx_trace_lock_lt500us,
               (unsigned long)tx_trace_lock_ge500us,
               (unsigned long)tx_trace_lock_max_us,
               (unsigned long)(tx_trace_lock_sum_us / n));
        printf("[TRACE] body (us): <5=%lu <20=%lu <100=%lu <500=%lu >=500=%lu max=%lu mean=%lu\r\n",
               (unsigned long)tx_trace_body_lt5us,
               (unsigned long)tx_trace_body_lt20us,
               (unsigned long)tx_trace_body_lt100us,
               (unsigned long)tx_trace_body_lt500us,
               (unsigned long)tx_trace_body_ge500us,
               (unsigned long)tx_trace_body_max_us,
               (unsigned long)(tx_trace_body_sum_us / n));
    }
    {
        uint32_t nd = tx_trace_di_n ? tx_trace_di_n : 1;
        uint32_t ns = tx_trace_sc_n ? tx_trace_sc_n : 1;
        printf("[TRACE] di   (us): <5=%lu <20=%lu <100=%lu <500=%lu >=500=%lu max=%lu mean=%lu n=%lu\r\n",
               (unsigned long)tx_trace_di_lt5us,
               (unsigned long)tx_trace_di_lt20us,
               (unsigned long)tx_trace_di_lt100us,
               (unsigned long)tx_trace_di_lt500us,
               (unsigned long)tx_trace_di_ge500us,
               (unsigned long)tx_trace_di_max_us,
               (unsigned long)(tx_trace_di_sum_us / nd),
               (unsigned long)tx_trace_di_n);
        printf("[TRACE] sc   (us): <5=%lu <20=%lu <100=%lu <500=%lu >=500=%lu max=%lu mean=%lu n=%lu\r\n",
               (unsigned long)tx_trace_sc_lt5us,
               (unsigned long)tx_trace_sc_lt20us,
               (unsigned long)tx_trace_sc_lt100us,
               (unsigned long)tx_trace_sc_lt500us,
               (unsigned long)tx_trace_sc_ge500us,
               (unsigned long)tx_trace_sc_max_us,
               (unsigned long)(tx_trace_sc_sum_us / ns),
               (unsigned long)tx_trace_sc_n);
    }
    /* sc decomposition: raw sums + n so the reader computes precise fractional
     * means (deq/qw/ind may be sub-us; subtract null floor from each). */
    printf("[SCDEC] deq sum=%lu n=%lu | qw sum=%lu cyc=%lu n=%lu max=%lu | ind sum=%lu cyc=%lu n=%lu | null sum=%lu n=%lu | pushed_max=%lu\r\n",
           (unsigned long)sc_deq_sum_us,  (unsigned long)sc_deq_n,
           (unsigned long)sc_qw_sum_us,   (unsigned long)sc_qw_cyc_us,  (unsigned long)sc_qw_n,  (unsigned long)sc_qw_max_us,
           (unsigned long)sc_ind_sum_us,  (unsigned long)sc_ind_cyc_us, (unsigned long)sc_ind_n,
           (unsigned long)sc_null_sum_us, (unsigned long)sc_null_n,
           (unsigned long)sc_pushed_max);
    printf("[SCDEC] producer=%s (see macif_prof for the WiFi-task push split)\r\n",
           sc_producer_name ? sc_producer_name : "?");
    /* DEBUG perf F1: per-frame schedule() kicks skipped via de-agg batching. */
    printf("[SCHED] deferred=%u pending_ac=0x%02x kicks_saved=%lu\r\n",
           (unsigned)fhost_tx_sched_deferred,
           (unsigned)fhost_tx_sched_pending_ac,
           (unsigned long)fhost_tx_sched_kicks_saved);
    /* 2026-07-23 (T2 profile): fhost_tx_req_do body decomposition (the ~100us
     * previously unaccounted between di and sc). Integer avg = sum/n, div0-guarded. */
    printf("[REQSUB] info avg=%lu n=%lu | parse avg=%lu n=%lu | queue avg=%lu n=%lu\r\n",
           (unsigned long)(req_info_n  ? req_info_sum_us  / req_info_n  : 0), (unsigned long)req_info_n,
           (unsigned long)(req_parse_n ? req_parse_sum_us / req_parse_n : 0), (unsigned long)req_parse_n,
           (unsigned long)(req_queue_n ? req_queue_sum_us / req_queue_n : 0), (unsigned long)req_queue_n);
}

void fhost_tx_trace_reset(void)
{
    uint32_t i;
    for (i = 0; i < TX_TRACE_RING_SZ; i++) {
        tx_trace_ring[i].t_enter_us   = 0;
        tx_trace_ring[i].dur_lock_us  = 0;
        tx_trace_ring[i].dur_total_us = 0;
        tx_trace_ring[i].ret          = 0;
    }
    tx_trace_idx = 0;
    tx_trace_last_enter_us = 0;
    tx_trace_intv_lt1ms = tx_trace_intv_lt10ms = 0;
    tx_trace_intv_lt80ms = tx_trace_intv_ge80ms = 0;
    tx_trace_intv_max_us = 0;
    tx_trace_dur_lt5us = tx_trace_dur_lt20us = 0;
    tx_trace_dur_lt100us = tx_trace_dur_lt500us = tx_trace_dur_ge500us = 0;
    tx_trace_dur_max_us = tx_trace_dur_sum_us = 0;
    tx_trace_lock_lt5us = tx_trace_lock_lt20us = 0;
    tx_trace_lock_lt100us = tx_trace_lock_lt500us = tx_trace_lock_ge500us = 0;
    tx_trace_lock_max_us = tx_trace_lock_sum_us = 0;
    tx_trace_body_lt5us = tx_trace_body_lt20us = 0;
    tx_trace_body_lt100us = tx_trace_body_lt500us = tx_trace_body_ge500us = 0;
    tx_trace_body_max_us = tx_trace_body_sum_us = 0;
    tx_trace_di_lt5us = tx_trace_di_lt20us = 0;
    tx_trace_di_lt100us = tx_trace_di_lt500us = tx_trace_di_ge500us = 0;
    tx_trace_di_max_us = tx_trace_di_sum_us = tx_trace_di_n = 0;
    tx_trace_sc_lt5us = tx_trace_sc_lt20us = 0;
    tx_trace_sc_lt100us = tx_trace_sc_lt500us = tx_trace_sc_ge500us = 0;
    tx_trace_sc_max_us = tx_trace_sc_sum_us = tx_trace_sc_n = 0;
    sc_deq_sum_us = sc_deq_n = 0;
    sc_qw_sum_us = sc_qw_n = sc_qw_max_us = sc_qw_cyc_us = 0;
    sc_ind_sum_us = sc_ind_n = sc_ind_cyc_us = 0;
    sc_null_sum_us = sc_null_n = 0;
    sc_pushed_max = 0;
    req_info_sum_us = req_info_n = 0;
    req_parse_sum_us = req_parse_n = 0;
    req_queue_sum_us = req_queue_n = 0;
    printf("tx_trace reset\r\n");
}

void fhost_tx_cfm_push(uint8_t access_category, struct txdesc *txdesc)
{
    tx_cfm_cnt++;
    fhost_tx_cfm_cnt++;
    abdbg_trace(0x5C, (uint16_t)tx_cfm_cnt); /* L5: fhost cfm enter (pre-lock) */
    if (fhost_tx_cfm_batch_owner == rtos_get_task_handle()) {
        /* F5: cfm batch already holds the TX mutex */
        fhost_tx_cfm(txdesc);
    } else {
        FHOST_TX_LOCK();
        fhost_tx_cfm(txdesc);
        FHOST_TX_UNLOCK();
    }
    abdbg_trace(0x5B, (uint16_t)tx_cfm_ok);  /* L5: fhost cfm exit (post-unlock) */
    tx_cfm_ok++;
}

/* DEBUG 2026-07-25 P1: minimal accessor so the `fhostq` census printer can
 * live in cli_al.c (flash). fhost_tx.c.o has its whole .text relocated into
 * OCRAM by the app linker script, so only this getter may sit here - the
 * format strings and the loop must not spend the hot-code budget.
 * Returns FHOST_TXQ_CNT for idx < 0, else 1 (valid) / 0 (out of range). */
int fhost_tx_txq_probe(int idx, uint16_t *nb_ready, int8_t *credits,
                       uint8_t *status, uint8_t *ac, uint32_t *first)
{
    struct fhost_tx_queue_tag *txq;

    if (idx < 0)
        return FHOST_TXQ_CNT;
    if (idx >= FHOST_TXQ_CNT)
        return 0;

    txq = &fhost_tx_env.tx_q[idx];
    *nb_ready = txq->nb_ready;
    *credits  = txq->credits;
    *status   = txq->status;
    #if NX_BEACONING
    *ac       = txq->ac;
    #else
    *ac       = 0;
    #endif
    *first    = (uint32_t)txq->ready.first;
    return 1;
}

int16_t fhost_tx_hwq_credits_get(int ac)
{
    return fhost_tx_env.hwq_credits[ac];
}

/* DEBUG 2026-07-23 F2b: runtime override of the per-AC outstanding-MPDU cap.
 * Poke while IDLE only (the counter is live inc/dec under traffic). Used to
 * cap MAC-outstanding at 24 while the SDIO TX pool is 32 so the extra 8
 * slots act as a staging area (host refills during the air phase). */
void fhost_tx_hwq_credits_set(int ac, int16_t v)
{
    fhost_tx_env.hwq_credits[ac] = (uint8_t)v;
}

void fhost_tx_credits_update(uint8_t sta_id, uint8_t tid, int8_t credits)
{
    struct fhost_tx_msg_tag msg;

    // Prepare the message
    msg.msg_id = FHOST_TX_CREDITS_UPDATE;
    msg.u.credits_upd.sta_id = sta_id;
    msg.u.credits_upd.tid = tid;
    msg.u.credits_upd.credits = credits;

    // Push the message to the queue
    rtos_queue_write(fhost_tx_env.queue_msg, &msg, -1, false);
}

void fhost_tx_sta_add(uint8_t sta_id, uint32_t buf_timeout)
{
    struct fhost_tx_msg_tag msg;

    if (sta_id >= STA_MAX)
        return;

    // Prepare the message
    msg.msg_id = FHOST_TX_STA_ADD;
    msg.u.sta.sta_id = sta_id;

    #if NX_BEACONING
    fhost_tx_env.traffic[sta_id].ps_max_queue_time = buf_timeout;
    #endif

    // Push the message to the queue
    rtos_queue_write(fhost_tx_env.queue_msg, &msg, -1, false);
}

void fhost_tx_sta_del(uint8_t sta_id)
{
    struct fhost_tx_msg_tag msg;

    if (sta_id >= STA_MAX)
        return;

    // Prepare the message
    msg.msg_id = FHOST_TX_STA_DEL;
    msg.u.sta.sta_id = sta_id;

    // Push the message to the queue
    rtos_queue_write(fhost_tx_env.queue_msg, &msg, -1, false);
}

void fhost_tx_vif_txq_enable(struct fhost_vif_tag *fhost_vif)
{
    struct fhost_tx_msg_tag msg;

    if (!fhost_vif || ! fhost_vif->mac_vif)
        return;

    // vif TXQ is used to send data to 'unknown' STA
    // AP: For all frames generated by wpa_supplicant (probe resp, auth ...)
    // STA: Needed for external authentication only
    // MONITOR: Needed by some external OS framework
    if ((mac_vif_get_type(fhost_vif->mac_vif) != VIF_MONITOR) &&
        (mac_vif_get_type(fhost_vif->mac_vif) != VIF_AP) &&
        (mac_vif_get_type(fhost_vif->mac_vif) != VIF_STA))
        return;

    msg.msg_id = FHOST_TX_VIF_ENABLE;
    msg.u.vif.vif_idx = mac_vif_get_index(fhost_vif->mac_vif);

    // Push the message to the queue
    rtos_queue_write(fhost_tx_env.queue_msg, &msg, -1, false);
}

void fhost_tx_vif_txq_disable(struct fhost_vif_tag *fhost_vif)
{
    struct fhost_tx_msg_tag msg;

    if (!fhost_vif || ! fhost_vif->mac_vif)
        return;

    if ((mac_vif_get_type(fhost_vif->mac_vif) != VIF_MONITOR) &&
        (mac_vif_get_type(fhost_vif->mac_vif) != VIF_AP) &&
        (mac_vif_get_type(fhost_vif->mac_vif) != VIF_STA))
        return;

    msg.msg_id = FHOST_TX_VIF_DISABLE;
    msg.u.vif.vif_idx = mac_vif_get_index(fhost_vif->mac_vif);

    // Push the message to the queue
    rtos_queue_write(fhost_tx_env.queue_msg, &msg, -1, false);
}

#if NX_BEACONING
int fhost_tx_sta_ps_enable(uint8_t sta_id, bool enable)
{
    struct fhost_tx_msg_tag msg;
    struct fhost_tx_queue_tag *txq;

    if (sta_id >= STA_MAX)
        return -1;

    txq = fhost_tx_get_txq(INVALID_VIF_IDX, sta_id, 0);
    if (!txq || !(txq->status & TXQ_ENABLED))
        return -1;

    if (enable)
        msg.msg_id = FHOST_TX_STA_ENTER_PS;
    else
        msg.msg_id = FHOST_TX_STA_EXIT_PS;
    msg.u.sta.sta_id = sta_id;

    return rtos_queue_write(fhost_tx_env.queue_msg, &msg, -1, false);
}

int fhost_tx_sta_ps_traffic_req(uint8_t sta_id, int pkt_cnt, bool uapsd)
{
    struct fhost_tx_msg_tag msg;
    struct fhost_tx_queue_tag *txq;

    if (sta_id >= STA_MAX)
        return -1;

    txq = fhost_tx_get_txq(INVALID_VIF_IDX, sta_id, 0);
    if (!txq || !(txq->status & TXQ_ENABLED))
        return -1;

    msg.msg_id = FHOST_TX_PS_TRAFFIC_REQ;
    msg.u.ps_traffic.sta_id = sta_id;
    msg.u.ps_traffic.req_cnt = pkt_cnt;
    msg.u.ps_traffic.ps_type = uapsd ? PS_UAPSD : PS_LEGACY;

    return rtos_queue_write(fhost_tx_env.queue_msg, &msg, -1, false);
}
#endif // NX_BEACONING

int fhost_tx_vif_ps_enable(uint8_t vif_idx, uint8_t enable)
{
    struct fhost_tx_msg_tag msg;

    if (enable)
        msg.msg_id = FHOST_TX_VIF_ENTER_PS;
    else
        msg.msg_id = FHOST_TX_VIF_EXIT_PS;
    msg.u.vif.vif_idx = vif_idx;

    return rtos_queue_write(fhost_tx_env.queue_msg, &msg, -1, false);
}

// Firmware internal mgmt frame TX (in macif_fhost.c, bypasses fhost TX scheduler)
extern int macif_tx_mgmt_frame(uint8_t vif_idx, const uint8_t *frame, uint32_t length, int no_cck);

uint32_t fhost_send_80211_frame(int fvif_idx, const uint8_t *frame, uint32_t length,
                                int no_cck, cb_fhost_tx cfm_cb, void *cfm_cb_arg)
{
    struct fhost_vif_tag *fhost_vif;

    if (fvif_idx >= NX_VIRT_DEV_MAX)
        return 0;

    fhost_vif = &fhost_env.vif[fvif_idx];
    if (!fhost_vif->mac_vif)
        return 1;

    // PM46: keep acad8d250's macif_tx_mgmt_frame bypass for mgmt TX.
    // Reverting to upstream net_buf_tx_alloc_fill + fhost_tx_req exposes
    // macsw-internal type-truncation bugs (IPRO widened INVALID_STA_IDX
    // from 0xFF to 0x3FF but only fixed half the chain); the upstream
    // path then hangs in txu_cntrl logic_port_filter on sta_idx=1023.
    // The starvation race that previously caused LAB03 first-connect to
    // fail ~75% is now mitigated by PM46's retry-on-NULL inside
    // macif_tx_mgmt_frame (up to 500ms wait for scan probe-req TX cfm).
    if (macif_tx_mgmt_frame(mac_vif_get_index(fhost_vif->mac_vif), frame, length, no_cck)) {
        printf("[fhost_tx] macif_tx_mgmt_frame FAILED vif=%d len=%u\r\n",
               mac_vif_get_index(fhost_vif->mac_vif), length);
        return 2;
    }

    return 4; // success (must be >= 4 for caller check)
}

/// @}

