/**
 ****************************************************************************************
 *
 * @file fhost_rx.c
 *
 * @brief Implementation of the fully hosted RX task.
 *
 * Copyright (C) RivieraWaves 2017-2021
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup FHOST_RX
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "export/adapter/rtos.h"
#include "fhost.h"
#include "fhost_rx.h"
#include "fhost_rx_def.h"
#include "mac_frame.h"
#include "export/common/co_endian.h"
#include "cfgrwnx.h"
#include "fhost_tx.h"
#if NX_FHOST_RX_STATS
#include "fhost_config.h"
#endif
#include "fhost_cntrl.h"
#if RW_MESH_EN
#include "mesh.h"
#endif

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
struct fhost_rx_env_tag fhost_rx_env;

/// Pool of RX buffers
#ifdef CONFIG_FHOST_RX_BUF_SECTION
static struct fhost_rx_buf_tag fhost_rx_buf_mem[FHOST_RX_BUF_CNT] __attribute__((section(CONFIG_FHOST_RX_BUF_SECTION)));
#else
static struct fhost_rx_buf_tag fhost_rx_buf_mem[FHOST_RX_BUF_CNT] __SHAREDRAM;
#endif
#if NX_UF_EN
/// Pool of UF buffers
static struct fhost_rx_uf_buf_tag fhost_rx_uf_buf_mem[FHOST_RX_BUF_CNT] __SHAREDRAM;
#endif // NX_UF_EN

#if NX_FHOST_RX_STATS
struct fhost_rx_stats_sta *rx_stats[CFG_STA_MAX];
#endif // NX_FHOST_RX_STATS

/* How deep the RX buffer pool actually goes. The free pool is a FreeRTOS queue
 * and has no low-water mark, so count the complement instead: a buffer is
 * PINNED from its lease (the macif pop) until its last ref puts it back on the
 * queue. min-free = FHOST_RX_BUF_CNT - [0]. Sizing FHOST_RX_BUF_CNT off
 * NX_MAX_BA_RX * NX_AMPDU_RX_BUF_SIZE assumes every BA session pins a full
 * window at once; this says whether that ever happens.
 *   [0] = pinned high-water   [1] = pinned now
 * Read by symbol from the ELF, so there is no print path. */
volatile uint32_t g_rxbuf_pinned[2];

/* Phase 2A.10 Counter I: per-frame buffer hold-time histogram.
 * Lease ts is set in macif_rx_buf_get (macif_fhost.c) on each pop;
 * release ts and accumulation happen here in the ref==0 branch below.
 * Read via tx_diag, reset via tx_diag_reset. */
volatile uint32_t macsw_rxbuf_hold_cnt    = 0;
volatile uint32_t macsw_rxbuf_hold_sum_us = 0;
volatile uint32_t macsw_rxbuf_hold_max_us = 0;
volatile uint32_t macsw_rxbuf_hold_lt100us  = 0;
volatile uint32_t macsw_rxbuf_hold_lt500us  = 0;
volatile uint32_t macsw_rxbuf_hold_lt1ms    = 0;
volatile uint32_t macsw_rxbuf_hold_lt5ms    = 0;
volatile uint32_t macsw_rxbuf_hold_lt10ms   = 0;
volatile uint32_t macsw_rxbuf_hold_lt50ms   = 0;
volatile uint32_t macsw_rxbuf_hold_ge50ms   = 0;

/* Phase 2A.11 Counter K: bisect the 11349us pre-submit window. Three
 * segments measured between Counter I lease and Counter J submit:
 *   (a) mac_us  = K0 - lease   : macsw RX (HW DMA + reorder + queue push)
 *   (b) fhost_us= K1 - K0      : fhost_rx_thread wait + buf_forward
 *   (c) k1push_us = push - K1  : net_al_input + lwIP + hook + USB pipe
 *                                (subtract Counter J avg=103us to get
 *                                 lwIP+hook segment).
 * Gated on K0!=0 && K1!=0 (forwarded data frames only). */
volatile uint32_t k_cnt           = 0;
volatile uint32_t k_mac_sum_us    = 0;
volatile uint32_t k_mac_max_us    = 0;
volatile uint32_t k_mac_lt100us   = 0;
volatile uint32_t k_mac_lt500us   = 0;
volatile uint32_t k_mac_lt2ms     = 0;
volatile uint32_t k_mac_lt10ms    = 0;
volatile uint32_t k_mac_ge10ms    = 0;
volatile uint32_t k_fhost_sum_us  = 0;
volatile uint32_t k_fhost_max_us  = 0;
volatile uint32_t k_fhost_lt50us  = 0;
volatile uint32_t k_fhost_lt200us = 0;
volatile uint32_t k_fhost_lt1ms   = 0;
volatile uint32_t k_fhost_lt5ms   = 0;
volatile uint32_t k_fhost_ge5ms   = 0;
volatile uint32_t k_k1push_sum_us = 0;
volatile uint32_t k_k1push_max_us = 0;
volatile uint32_t k_k1push_lt500us = 0;
volatile uint32_t k_k1push_lt2ms   = 0;
volatile uint32_t k_k1push_lt10ms  = 0;
volatile uint32_t k_k1push_ge10ms  = 0;

/* Phase 2A.12 Counter L: split Counter K (a) macsw segment into two:
 *   (a1) macrx_us  = L0 - lease : HW DMA + rxl_dma_evt + rxu_cntrl_evt
 *                                 + BA reorder (the suspected cap)
 *   (a2) qdwell_us = K0 - L0    : queue_desc enqueue + dwell +
 *                                 fhost_rx_task wakeup latency
 * Gated on K0!=0 && K1!=0 && L0!=0. Uses a separate cnt (kL_cnt) so
 * frames missing L0 (UF, non-dispatch paths) only count toward the
 * existing K (a)/(b)/(c) totals. */
volatile uint32_t kL_cnt          = 0;
volatile uint32_t kL_macrx_sum_us = 0;
volatile uint32_t kL_macrx_max_us = 0;
volatile uint32_t kL_macrx_lt100us = 0;
volatile uint32_t kL_macrx_lt500us = 0;
volatile uint32_t kL_macrx_lt2ms   = 0;
volatile uint32_t kL_macrx_lt10ms  = 0;
volatile uint32_t kL_macrx_ge10ms  = 0;
volatile uint32_t kL_qdwell_sum_us = 0;
volatile uint32_t kL_qdwell_max_us = 0;
volatile uint32_t kL_qdwell_lt10us = 0;
volatile uint32_t kL_qdwell_lt100us = 0;
volatile uint32_t kL_qdwell_lt1ms   = 0;
volatile uint32_t kL_qdwell_lt10ms  = 0;
volatile uint32_t kL_qdwell_ge10ms  = 0;
/*
 * FUNCTIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Send FHOST_TX_RESEND_REQ message to Tx task
 *
 * @param[in] buf Pointer to Rx buffer structure
 ****************************************************************************************
 */
void fhost_rx_do_resend_req(struct fhost_rx_buf_tag *buf, net_al_tx_t net_buf_tx, bool forward)
{
    struct fhost_tx_msg_tag msg;

    if(forward)
    {
        buf->ref++;
    }
    buf->info_tx.net_buf = net_buf_tx;
    // Prepare the message
    msg.msg_id = FHOST_TX_RESEND_REQ;
    msg.u.resend.rx_buf = buf;

    rtos_queue_write(fhost_tx_env.queue_msg, &msg, -1, false);
}

#if NX_UF_EN
/**
 ****************************************************************************************
 * @brief Push an unsupported frame buffer to the WiFi task.
 *
 * @param[in] net_buf   Pointer to the RX buffer to push
 ****************************************************************************************
 */
static void fhost_rx_uf_buf_push(void *net_buf)
{
    struct fhost_rx_uf_buf_tag *buf = net_buf;
    struct fhost_rx_uf_buf_desc_tag buf_desc;
    int res;

    // Push back the RX buffer to the MAC
    buf->info.pattern = 0;
    buf_desc.host_id = (uint32_t)buf;

    res = rtos_queue_write(fhost_rx_env.queue_uf_buf, &buf_desc, 0, false);
    ASSERT_ERR(res == 0);
}
#endif // NX_UF_EN

/**
 ****************************************************************************************
 * @brief Forward/Resend a RX buffer to the networking stack/Tx path.
 *
 * @param[in] buf                Pointer to the RX buffer to forward
 * @param[in] rx_buf_action      RX send action status
 * @param[in] net_if             Network interface to which the buffer is intended
 * @param[in] length             Length of Rx buffer to send
 * @param[in] offset             Offset to point on RX buffer
 * @param[in] skip_after_eth_hdr Offset to that should be skipped for the forward
 ****************************************************************************************
 */
static void fhost_rx_buf_send(struct fhost_rx_buf_tag *buf,
                              uint8_t rx_buf_action,
                              net_al_if_t net_if,
                              uint16_t length,
                              uint8_t offset,
                              uint8_t skip_after_eth_hdr)
{
    /* Phase 2A.11 Counter K1: fhost->lwIP handoff timestamp. Gated by
     * lease_us!=0 (init-time pushes skipped). Only set on the FORWARD
     * branch so AMSDU subframes (forwarded via fhost_rx_amsdu_forward)
     * are NOT double-stamped. */
    if (buf && buf->lease_us != 0 && (rx_buf_action & RX_BUF_FORWARD)) {
        uint32_t now = inline_hal_machw_time();
        buf->fhost_to_lwip_us = now ? now : 1;
    }

    if (rx_buf_action & RX_BUF_RESEND)
    {
        // Resend to another sta connected to the same AP
        net_al_rx_resend(rx_buf_action & RX_BUF_FORWARD, buf, buf->payload, length, offset, sizeof(struct mac_eth_hdr) + skip_after_eth_hdr);
    }

    if (rx_buf_action & RX_BUF_FORWARD)
    {
        {
            struct mac_eth_hdr *eth = (struct mac_eth_hdr *)((uint8_t *)buf->payload + offset);
            uint16_t etype = co_ntohs(eth->len);
            static int send_cnt = 0;
            if (send_cnt++ < 3)
                printf("NET_IN len=%d etype=0x%04x\r\n", length, etype);
            // Dump EAPOL frames (EtherType 0x888e)
            if (etype == 0x888e) {
                uint8_t *eapol = (uint8_t *)(eth + 1);
                printf("EAPOL len=%d ver=%d type=%d bodylen=%d\r\n",
                       length, eapol[0], eapol[1],
                       (eapol[2] << 8) | eapol[3]);
                // Dump first 40 bytes of EAPOL-Key body (includes key_info, key_len, replay_counter, key_nonce)
                if (length > (int)sizeof(struct mac_eth_hdr) + 20) {
                    int dump_len = length - sizeof(struct mac_eth_hdr);
                    if (dump_len > 40) dump_len = 40;
                    printf("EAPOL_DATA[%d]:", dump_len);
                    for (int i = 0; i < dump_len; i++)
                        printf(" %02x", eapol[i]);
                    printf("\r\n");
                }
            }
        }
        net_al_input((net_al_rx_t)buf->fnet_buf, buf->payload, net_if, length, offset, skip_after_eth_hdr, fhost_rx_buf_push);
    }
}

/**
 ****************************************************************************************
 * @brief Forward a RX buffer containing a A-MSDU to the networking stack.
 *
 * @param[in] buf                Pointer to the RX buffer to forward
 * @param[in] net_if             Network interface to which the buffer is intended
 * @param[in] buf_rx_action      RX send action status
 * @param[in] skip_after_eth_hdr Offset to that should be skipped for the forward
 ****************************************************************************************
 */
static void fhost_rx_amsdu_forward(struct fhost_rx_buf_tag *buf, net_al_if_t net_if,
                                   uint8_t buf_rx_action, uint8_t skip_after_eth_hdr)
{
    struct rx_info info;
    uint8_t *payload;
    uint16_t len;
    int subframe_idx = 0;
    struct llc_snap *llc_snap;
    uint8_t offset;

    memcpy(&info, &buf->info, sizeof(info));

    do
    {
        offset = 0;

        // Get payload pointer
        payload = (uint8_t *)buf->payload;

        // Get the subframe length
        len = co_ntohs(co_read16(&payload[LLC_ETHERTYPE_LEN_OFT])) + LLC_ETHER_HDR_LEN;

        // Map LLC/SNAP structure on buffer
        llc_snap = (struct llc_snap *)&payload[LLC_ETHER_HDR_LEN];
        if ((!memcmp(llc_snap, &llc_rfc1042_hdr, sizeof(llc_rfc1042_hdr))
             //&& (llc_snap->proto_id != LLC_ETHERTYPE_AARP) - Appletalk depracated ?
             && (llc_snap->proto_id != LLC_ETHERTYPE_IPX))
            || (!memcmp(llc_snap, &llc_bridge_tunnel_hdr, sizeof(llc_bridge_tunnel_hdr))))
        {
            // Packet becomes
            /********************************************
             *  DA  |  SA  |  SNAP->ETHERTYPE  |  DATA  |
             ********************************************/
            // We remove the LLC/SNAP, so adjust the length and the offset
            len -= LLC_802_2_HDR_LEN;
            offset = LLC_802_2_HDR_LEN;

            // Move the source/dest addresses at the right place
            MAC_ADDR_CPY(&payload[offset + MAC_ADDR_LEN], &payload[MAC_ADDR_LEN]);
            MAC_ADDR_CPY(&payload[offset], &payload[0]);

            // No need to copy the Ethertype which is already in place
        }

        // Forward/Resend
        fhost_rx_buf_send(buf, buf_rx_action, net_if, len, offset, skip_after_eth_hdr);

        // Check if we may still have some subframes
        if (subframe_idx == (NX_MAX_MSDU_PER_RX_AMSDU - 1))
            break;

        // Get next subframe
        buf = (struct fhost_rx_buf_tag *)info.amsdu_hostids[subframe_idx++];

        // Copy rx_info into subframe (fix MIOT616-233)
        if (buf) {
            memcpy(&buf->info, &info, sizeof(info));
        }

    } while (buf != NULL);
}

/**
 ****************************************************************************************
 * @brief Forward a MGMT frame to the registered callback.
 *
 * @param[in] buf        Pointer to the RX buffer to forward
 * @param[in] fhost_vif  Interface that received the frame (can be NULL)
 ****************************************************************************************
 */
static void fhost_rx_mgmt_buf_forward(struct fhost_rx_buf_tag *buf,
                                      struct fhost_vif_tag *fhost_vif)
{
    struct fhost_frame_info info;

    if (fhost_rx_env.mgmt_cb == NULL)
        return;

    info.payload = (uint8_t *)buf->payload;
    info.length = buf->info.vect.frmlen;
    info.freq = PHY_INFO_CHAN(buf->info.phy_info);
    info.rssi = export_hal_desc_get_rssi((void *)&buf->info.vect.rx_vec_1, NULL);

    if (fhost_vif)
    {
        info.fvif_idx = CO_GET_INDEX(fhost_vif, fhost_env.vif);
        fhost_rx_env.mgmt_cb(&info, fhost_rx_env.mgmt_cb_arg);
    }
    else
    {
        for (int i = 0; i < NX_VIRT_DEV_MAX; i++)
        {
            if (fhost_env.vif[i].mac_vif)
            {
                info.fvif_idx = i;
                fhost_rx_env.mgmt_cb(&info, fhost_rx_env.mgmt_cb_arg);
            }
        }
    }
}

/**
 ****************************************************************************************
 * @brief Select which action to send RX buffer should be activated: Resend/Forward.
 *
 * @param[in] fhost_vif          virtual interfaces information
 * @param[in] mac_addr           MAC address of the destination buffer
 * @param[in] rx_info_flags      SW flags about the RX packet
 * @param[in] vif_idx            Interface the station belongs to
 * @param[in] skip_after_eth_hdr Offset to that should be skipped for the forward
 *
 * @return the Bitfield indicating the RX action to perform
 ****************************************************************************************
 */
static uint8_t fhost_rx_select_action(struct fhost_vif_tag *fhost_vif,
                                      struct fhost_rx_buf_tag *buf,
                                      uint32_t rx_info_flags,
                                      uint8_t vif_idx,
                                      uint8_t *skip_after_eth_hdr)
{
    uint8_t rx_buf_action = RX_BUF_FORWARD;

    if (!fhost_vif->isolation_mode && (mac_vif_get_type(fhost_vif->mac_vif) == VIF_AP))
    {
        struct mac_eth_hdr *hdr = (struct mac_eth_hdr *)buf->payload;

        if (MAC_ADDR_GROUP(hdr))
        {
            rx_buf_action |= RX_BUF_RESEND;
        }
        else
        {
            uint8_t dest_sta_idx = (rx_info_flags & RX_FLAGS_DST_INDEX_MSK) >> RX_FLAGS_DST_INDEX_OFT;
            if (dest_sta_idx != INVALID_STA_IDX)
            {
                if (sta_idx_get_valid(dest_sta_idx) && (sta_idx_get_inst_nbr(dest_sta_idx) == vif_idx))
                {
                    // Reset forward and set to resend
                    rx_buf_action = RX_BUF_RESEND;
                }
            }
        }
    }
    #if RW_MESH_EN
    else if (mac_vif_get_type(fhost_vif->mac_vif) == VIF_MESH_POINT)
    {
        struct mac_eth_hdr *hdr = (struct mac_eth_hdr *)buf->payload;
        struct rx_info *info = &buf->info;
        uint8_t dest_sta_idx = (info->flags & RX_FLAGS_DST_INDEX_MSK) >> RX_FLAGS_DST_INDEX_OFT;

        rx_buf_action = RX_BUF_FORWARD;

        if (dest_sta_idx != INVALID_STA_IDX)
        {
            // TO DO: LLC-SNAP is already removed in AMSDU.
            ASSERT_ERR(!(info->flags & RX_FLAGS_IS_AMSDU_BIT));

            // MC/BC frames are uploaded with mesh control (6 bytes) and LLC/snap (8 bytes)
            // (so they can be mesh forwarded) that need to be removed.
            uint8_t *mesh_ctrl = (uint8_t *)(hdr + 1);
            *skip_after_eth_hdr = 8 + 6;
            if ((*mesh_ctrl & MESH_CTRL_FLAGS_AE_MODE_MASK) == MESH_CTRL_MESH_AE_ADDR4)
                *skip_after_eth_hdr += sizeof(struct mac_addr);
            else if ((*mesh_ctrl & MESH_CTRL_FLAGS_AE_MODE_MASK) == MESH_CTRL_MESH_AE_ADDR5_6)
                *skip_after_eth_hdr += 2 * sizeof(struct mac_addr);

            if (MAC_ADDR_GROUP(hdr))
            {
                rx_buf_action |= RX_BUF_RESEND;
            }
            else
            {
                rx_buf_action = RX_BUF_RESEND;
            }
        }
    }
    #endif

    return(rx_buf_action);
}

#if NX_FHOST_RX_STATS && 0
/**
 ****************************************************************************************
 * @brief Update RX statistics with the received payload
 *
 * @param[in] info      Information about the received payload
 ****************************************************************************************
 */
static void fhost_rx_save_stats(struct rx_info *info)
{
    uint8_t staid = (info->flags & RX_FLAGS_STA_INDEX_MSK) << RX_FLAGS_STA_INDEX_OFT;
    int rate_idx, mcs = 0, sgi = 0, nss = 0;
    struct rx_vector_1 *rx_vect = &info->vect.rx_vec_1;

    // STA idx invalid, do not save statistics
    if (!rx_stats[staid])
        return;

    switch (rx_vect->format_mod) {
        case FORMATMOD_NON_HT:
        case FORMATMOD_NON_HT_DUP_OFDM:
        {
            int idx = rxv2macrate[rx_vect->leg_rate];
            if (idx < 4) {
                rate_idx = idx * 2 + rx_vect->pre_type;
            } else {
                rate_idx = N_CCK + idx - 4;
            }
            break;
        }
        case FORMATMOD_HT_MF:
        case FORMATMOD_HT_GF:
            #if NX_MAC_VER >= 20
            mcs = rx_vect->ht.mcs % 8;
            nss = rx_vect->ht.mcs / 8;
            sgi = rx_vect->ht.short_gi;
            #else
            mcs = rx_vect->mcs % 8;
            nss = rx_vect->mcs / 8;
            sgi = rx_vect->short_gi;
            #endif
            rate_idx = fhost_conf_rx.first_ht + nss * (8 * fhost_conf_rx.ht_rates_per_mcs) +
                       mcs * fhost_conf_rx.ht_rates_per_mcs + rx_vect->ch_bw * 2 + sgi;
            break;
        case FORMATMOD_VHT:
            #if NX_MAC_VER < 20
            default:
            mcs = rx_vect->mcs;
            nss = rx_vect->stbc ? rx_vect->n_sts/2 : rx_vect->n_sts;
            sgi = rx_vect->short_gi;
            #else
            mcs = rx_vect->vht.mcs;
            nss = rx_vect->vht.nss;
            sgi = rx_vect->vht.short_gi;
            #endif
            rate_idx = fhost_conf_rx.first_vht + nss * (fhost_conf_rx.max_vht_mcs * fhost_conf_rx.vht_rates_per_mcs) +
                       mcs * fhost_conf_rx.vht_rates_per_mcs + rx_vect->ch_bw * 2 + sgi;
            break;
        #if NX_MAC_VER >= 20
        case FORMATMOD_HE_SU:
            mcs = rx_vect->he.mcs;
            nss = rx_vect->he.nss;
            sgi = rx_vect->he.gi_type;
            rate_idx = fhost_conf_rx.first_he_su + nss * (fhost_conf_rx.max_he_mcs * fhost_conf_rx.he_su_rates_per_mcs) +
                       mcs * fhost_conf_rx.he_su_rates_per_mcs + rx_vect->ch_bw * 3 + sgi;
            break;
        case FORMATMOD_HE_MU:
            mcs = rx_vect->he.mcs;
            nss = rx_vect->he.nss;
            sgi = rx_vect->he.gi_type;
            rate_idx = fhost_conf_rx.first_he_mu + nss * fhost_conf_rx.max_he_mcs * fhost_conf_rx.he_mu_rates_per_mcs +
                       mcs * fhost_conf_rx.he_mu_rates_per_mcs + rx_vect->he.ru_size * 3 + sgi;
            break;
        default:
            mcs = rx_vect->he.mcs;
            nss = rx_vect->he.nss;
            sgi = rx_vect->he.gi_type;
            rate_idx = fhost_conf_rx.first_he_er + rx_vect->ch_bw * 9 +
                       mcs * fhost_conf_rx.he_er_rates_per_mcs + sgi;
        #endif
    }

    rx_stats[staid]->table[rate_idx]++;
    rx_stats[staid]->cpt++;
    rx_stats[staid]->last_rx = info->vect.rx_vec_1;
}
#endif // NX_FHOST_RX_STATS

/**
 ****************************************************************************************
 * @brief Forward a RX buffer to the networking stack.
 *
 * @param[in] buf Pointer to the RX buffer to forward
 ****************************************************************************************
 */
static void fhost_rx_buf_forward(struct fhost_rx_buf_tag *buf)
{
    struct rx_info *info = &buf->info;
    struct fhost_vif_tag *fhost_vif = NULL;
    uint8_t vif_idx = (info->flags & RX_FLAGS_VIF_INDEX_MSK) >> RX_FLAGS_VIF_INDEX_OFT;
    uint8_t skip_after_eth_hdr = 0;

    if (vif_idx < NX_VIRT_DEV_MAX)
        fhost_vif = fhost_env.mac2fhost_vif[vif_idx];

    if(info->flags & RX_FLAGS_NEW_MESH_PEER_BIT)
    {
        fhost_cntrl_new_peer_candidate(vif_idx, buf);
        fhost_rx_buf_push(buf);
    }
    else if (info->flags & RX_FLAGS_NON_MSDU_MSK)
    {
        // Mgmt frame may be received with invalid vif idx
        fhost_rx_mgmt_buf_forward(buf, fhost_vif);
        fhost_rx_buf_push(buf);
    }
    else if (fhost_vif)
    {
        uint8_t rx_buf_action;
        rx_buf_action = fhost_rx_select_action(fhost_vif, buf, info->flags,
                                               mac_vif_get_index(fhost_vif->mac_vif), &skip_after_eth_hdr);

        #if NX_FHOST_RX_STATS
        fhost_rx_save_stats(info);
        #endif

        if (info->flags & RX_FLAGS_IS_AMSDU_BIT)
        {
            fhost_rx_amsdu_forward(buf, fhost_vif->net_if, rx_buf_action, skip_after_eth_hdr);
        }
        else
        {
            fhost_rx_buf_send(buf, rx_buf_action, fhost_vif->net_if, info->vect.frmlen, 0, skip_after_eth_hdr);
        }
    }
    else
    {
        ASSERT_ERR(0);
        fhost_rx_buf_push(buf);
    }
}

/**
 ****************************************************************************************
 * @brief Update the length of a reassembled RX buffer.
 *
 * @param[in] buf Pointer to the reassembled RX buffer
 * @param[in] status Pointer to the status descriptor
 ****************************************************************************************
 */
static void fhost_rx_buf_len_update(struct fhost_rx_buf_tag *buf, struct rxu_stat_val *status)
{
    struct rx_info *info =&buf->info;
    info->vect.frmlen = status->frame_len;

    if (status->status & RX_STAT_ETH_LEN_UPDATE)
    {
        // Update Length Field inside the Ethernet Header
        struct mac_eth_hdr *hdr = (struct mac_eth_hdr *)buf->payload;

        hdr->len = co_htons(status->frame_len - sizeof(struct mac_eth_hdr));
    }
}

/**
 ****************************************************************************************
 * @brief Call registered monitor callback for the received buffer.
 *
 * Extract useful information from RX buffer and call the monitor callback with this as
 * parameter. Returns immediately if no monitor callback is registered.
 *
 * @param[in] buf  Pointer to the RX buffer
 * @param[in] uf   Whether frame has been decoded or not by the modem.
 *                 (false: decoded frame, true: unsupported frame)
 ****************************************************************************************
 */
static void fhost_rx_monitor(struct fhost_rx_buf_tag *buf, bool uf)
{
    struct fhost_frame_info info;
    uint8_t vif_idx;

    if (fhost_rx_env.monitor_cb == NULL)
        return;

    if (uf)
    {
        info.payload = NULL;
        info.length = export_hal_desc_get_ht_length((void *)&buf->info.vect.rx_vec_1);
    }
    else
    {
        info.payload = (uint8_t *)buf->payload;
        info.length = buf->info.vect.frmlen;
    }

    info.freq = PHY_INFO_CHAN(buf->info.phy_info);
    info.rssi = export_hal_desc_get_rssi((void *)&buf->info.vect.rx_vec_1, NULL);

    vif_idx = (buf->info.flags & RX_FLAGS_VIF_INDEX_MSK) >> RX_FLAGS_VIF_INDEX_OFT;
    if (vif_idx < NX_VIRT_DEV_MAX)
    {
        struct fhost_vif_tag *fhost_vif = fhost_env.mac2fhost_vif[vif_idx];
        info.fvif_idx = CO_GET_INDEX(fhost_vif, fhost_env.vif);
    }
    else
        info.fvif_idx = -1;

    fhost_rx_env.monitor_cb(&info, fhost_rx_env.monitor_cb_arg);
}

/**
 ****************************************************************************************
 * @brief RX task implementation.
 ****************************************************************************************
 */
static RTOS_TASK_FCT(fhost_rx_task)
{
    // Push the RX buffers
    for (int i=0; i<FHOST_RX_BUF_CNT; i++)
    {
        struct fhost_rx_buf_tag *buf = &fhost_rx_buf_mem[i];
        #if NX_UF_EN
        struct fhost_rx_uf_buf_tag *uf_buf = &fhost_rx_uf_buf_mem[i];
        // Indicate the UF buffer to the MAC
        fhost_rx_uf_buf_push(uf_buf);
        #endif // NX_UF_EN

        // Initialize all buffer Rx to 1 in order to be pushed
        buf->ref = 1;

        // Indicate the RX buffer to the MAC
        fhost_rx_buf_push(buf);
    }
    fhost_task_ready(RX_TASK);

    for (;;)
    {
        struct fhost_rx_msg_tag msg;
        struct fhost_rx_buf_tag *buf;

        // Wait for a message on the RX queue
        rtos_queue_read(fhost_rx_env.queue_desc, &msg, -1, false);

        // Get the pointer to the RX buffer associated to the event
        buf = (struct fhost_rx_buf_tag *)msg.desc.host_id;

        /* Phase 2A.11 Counter K0: macsw->fhost handoff timestamp. Gate
         * on lease_us!=0 to skip init-time pushes whose buf was not
         * routed through HW DMA. */
        if (buf && buf->lease_us != 0) {
            uint32_t now = inline_hal_machw_time();
            buf->mac_to_fhost_us = now ? now : 1;
        }

        // Handle the received message
        if (msg.desc.status & RX_STAT_LEN_UPDATE)
        {
            // Update the length
            fhost_rx_buf_len_update(buf, &msg.desc);
        }
        if (msg.desc.status & RX_STAT_FORWARD)
        {
            // Forward the buffer to the networking stack
            fhost_rx_buf_forward(buf);
        }
        else if (msg.desc.status & RX_STAT_ALLOC)
        {
            // fhost_printf("Not handler in fhost_rx");
        }
        #if NX_UF_EN
        if (msg.desc.status & RX_STAT_UF)
        {
            struct fhost_rx_uf_buf_tag *uf_buf;
            uf_buf = (struct fhost_rx_uf_buf_tag *)msg.desc.host_id;

            fhost_rx_monitor((struct fhost_rx_buf_tag *)uf_buf, true);
            fhost_rx_uf_buf_push(uf_buf);
        }
        #endif // NX_UF_EN
        if (msg.desc.status & RX_STAT_MONITOR)
        {
            fhost_rx_monitor(buf, false);
            fhost_rx_buf_push(buf);
        }
        else if (msg.desc.status & RX_STAT_DELETE)
        {
            // Push back the buffer to the MAC
            fhost_rx_buf_push(buf);
        }

        if (msg.desc.status & RX_STAT_SPURIOUS)
        {
            // Push back the buffer to the MAC
            fhost_rx_buf_push(buf);
        }
    }
}

/* Phase 2A.10 Counter I helper. macif_fhost.c calls this after popping
 * a buffer from the RX queue to record the lease timestamp. host_id is
 * (uint32_t)cast of struct fhost_rx_buf_tag*. lease_us=0 is the "idle"
 * sentinel, so substitute 1 if hal_machw_time() returns 0. */
/* host_id is a raw (uint32_t) cast of a pool pointer, so a stale or garbage
 * value dereferences wherever it points, and a NULL test is the only thing
 * standing in the way. During the 2026-08 disconnect-crash hunt a Load Access
 * Fault was observed in the helpers below on a stale mgmt-frame host_id; that
 * specific repro died with the sta_idx fix (8/8 disconnect cycles clean on the
 * fixed tree, 2026-08-06), so this is hardening, not a live-bug fix -- but the
 * exposure is structural: these are diagnostics fed IDs off the wire, and a
 * diagnostic must never be able to fault. Validate range AND stride so a
 * pointer into the middle of an element is rejected too. */
static bool fhost_rx_buf_id_valid(uint32_t host_id)
{
    uint32_t base = (uint32_t)(uintptr_t)&fhost_rx_buf_mem[0];
    uint32_t span = (uint32_t)sizeof(fhost_rx_buf_mem);
    uint32_t elem = (uint32_t)sizeof(fhost_rx_buf_mem[0]);

    if ((host_id < base) || (host_id >= base + span))
        return false;
    return ((host_id - base) % elem) == 0;
}

void fhost_rx_buf_lease(uint32_t host_id)
{
    if (!fhost_rx_buf_id_valid(host_id)) return;
    struct fhost_rx_buf_tag *b = (struct fhost_rx_buf_tag *)host_id;
    uint32_t now = inline_hal_machw_time();
    if (now == 0) now = 1;
    /* Count the transition, not the call: a re-lease of an already-leased
     * buffer only restamps and must not double-count it as pinned. */
    if (b->lease_us == 0) {
        if (++g_rxbuf_pinned[1] > g_rxbuf_pinned[0]) g_rxbuf_pinned[0] = g_rxbuf_pinned[1];
    }
    b->lease_us = now;
}

/* Phase 2A.12 Counter L0 helper. macif_rx_desc_upload calls this just
 * before rtos_queue_write into fhost_rx_env.queue_desc, so the timestamp
 * marks the macsw -> fhost dispatch boundary (after BA reorder). */
void fhost_rx_buf_dispatch(uint32_t host_id)
{
    if (!fhost_rx_buf_id_valid(host_id)) return;
    struct fhost_rx_buf_tag *b = (struct fhost_rx_buf_tag *)host_id;
    if (b->lease_us == 0) return;   /* skip init-time / non-leased */
    uint32_t now = inline_hal_machw_time();
    if (now == 0) now = 1;
    b->mac_dispatch_us = now;
}

void fhost_rx_buf_push(void *net_buf)
{
    struct fhost_rx_buf_tag *buf = net_buf;
    struct fhost_rx_buf_desc_tag buf_desc;
    int res __MAYBE_UNUSED;

    ASSERT_ERR(buf->ref != 0);
    buf->ref--;

    if (buf->ref == 0)
    {
        /* Counter I: accumulate hold-time histogram if this buffer was
         * leased (lease_us != 0). Init-time pushes have lease_us = 0
         * because fhost_rx_buf_mem is .bss-zeroed. */
        if (buf->lease_us != 0) {
            uint32_t now = inline_hal_machw_time();
            uint32_t hold = now - buf->lease_us;
            if (g_rxbuf_pinned[1]) g_rxbuf_pinned[1]--;
            macsw_rxbuf_hold_cnt++;
            macsw_rxbuf_hold_sum_us += hold;
            if (hold > macsw_rxbuf_hold_max_us) macsw_rxbuf_hold_max_us = hold;
            if      (hold <   100) macsw_rxbuf_hold_lt100us++;
            else if (hold <   500) macsw_rxbuf_hold_lt500us++;
            else if (hold <  1000) macsw_rxbuf_hold_lt1ms++;
            else if (hold <  5000) macsw_rxbuf_hold_lt5ms++;
            else if (hold < 10000) macsw_rxbuf_hold_lt10ms++;
            else if (hold < 50000) macsw_rxbuf_hold_lt50ms++;
            else                   macsw_rxbuf_hold_ge50ms++;

            /* Counter K: bisect pre-submit window. Requires both K0 and
             * K1 to be set (i.e. forwarded data frame). */
            if (buf->mac_to_fhost_us != 0 && buf->fhost_to_lwip_us != 0) {
                uint32_t mac    = buf->mac_to_fhost_us  - buf->lease_us;
                uint32_t fhost  = buf->fhost_to_lwip_us - buf->mac_to_fhost_us;
                uint32_t k1push = now                   - buf->fhost_to_lwip_us;
                k_cnt++;
                k_mac_sum_us    += mac;
                k_fhost_sum_us  += fhost;
                k_k1push_sum_us += k1push;
                if (mac    > k_mac_max_us)    k_mac_max_us    = mac;
                if (fhost  > k_fhost_max_us)  k_fhost_max_us  = fhost;
                if (k1push > k_k1push_max_us) k_k1push_max_us = k1push;
                if      (mac <   100) k_mac_lt100us++;
                else if (mac <   500) k_mac_lt500us++;
                else if (mac <  2000) k_mac_lt2ms++;
                else if (mac < 10000) k_mac_lt10ms++;
                else                  k_mac_ge10ms++;
                if      (fhost <   50) k_fhost_lt50us++;
                else if (fhost <  200) k_fhost_lt200us++;
                else if (fhost < 1000) k_fhost_lt1ms++;
                else if (fhost < 5000) k_fhost_lt5ms++;
                else                   k_fhost_ge5ms++;
                if      (k1push <   500) k_k1push_lt500us++;
                else if (k1push <  2000) k_k1push_lt2ms++;
                else if (k1push < 10000) k_k1push_lt10ms++;
                else                     k_k1push_ge10ms++;

                /* Counter L: split (a) into (a1) lease->L0 + (a2) L0->K0.
                 * Only when L0 was stamped by macif_rx_desc_upload. */
                if (buf->mac_dispatch_us != 0) {
                    uint32_t a1 = buf->mac_dispatch_us - buf->lease_us;
                    uint32_t a2 = buf->mac_to_fhost_us - buf->mac_dispatch_us;
                    kL_cnt++;
                    kL_macrx_sum_us  += a1;
                    kL_qdwell_sum_us += a2;
                    if (a1 > kL_macrx_max_us)  kL_macrx_max_us  = a1;
                    if (a2 > kL_qdwell_max_us) kL_qdwell_max_us = a2;
                    if      (a1 <   100) kL_macrx_lt100us++;
                    else if (a1 <   500) kL_macrx_lt500us++;
                    else if (a1 <  2000) kL_macrx_lt2ms++;
                    else if (a1 < 10000) kL_macrx_lt10ms++;
                    else                 kL_macrx_ge10ms++;
                    if      (a2 <    10) kL_qdwell_lt10us++;
                    else if (a2 <   100) kL_qdwell_lt100us++;
                    else if (a2 <  1000) kL_qdwell_lt1ms++;
                    else if (a2 < 10000) kL_qdwell_lt10ms++;
                    else                 kL_qdwell_ge10ms++;
                }
            }

            buf->lease_us = 0;
            buf->mac_to_fhost_us = 0;
            buf->fhost_to_lwip_us = 0;
            buf->mac_dispatch_us = 0;
        }

        // Push back the RX buffer to the MAC
        buf->info.pattern = 0;
        buf_desc.host_id = (uint32_t)buf;
        buf_desc.addr = (uint32_t)&buf->info;
        // Initialise the number of reference
        buf->ref = 1;

        res = rtos_queue_write(fhost_rx_env.queue_buf, &buf_desc, 0, false);
        ASSERT_ERR(res == 0);

        // Warn the WiFi task about the new buffer availability, if required
        macif_rx_buf_ind();
    }
}

int fhost_rx_init(void)
{
    struct fhost_rx_buf_tag *buf0 = &fhost_rx_buf_mem[0];
    printf("RX_INFO: sizeof(rx_info)=%d sizeof(rx_vector)=%d\r\n",
           (int)sizeof(struct rx_info), (int)sizeof(struct rx_vector));
    printf("RX_BUF: info_off=%d payl_off=%d gap=%d\r\n",
           (int)((uintptr_t)&buf0->info - (uintptr_t)buf0),
           (int)((uintptr_t)&buf0->payload - (uintptr_t)buf0),
           (int)((uintptr_t)&buf0->payload - (uintptr_t)&buf0->info));

    // Initialize monitor callback
    fhost_rx_env.monitor_cb     = NULL;
    fhost_rx_env.monitor_cb_arg = NULL;

    // Create the RX queue
    if (rtos_queue_create(sizeof(struct fhost_rx_msg_tag), FHOST_RX_QUEUE_DESC_ELT_CNT,
                          &fhost_rx_env.queue_desc))
    {
        return 1;
    }

    // Create the RX buffer queue
    if (rtos_queue_create(sizeof(struct fhost_rx_buf_desc_tag), FHOST_RX_BUF_CNT, &fhost_rx_env.queue_buf))
    {
        return 2;
    }

    #if NX_UF_EN
    // Create the UF buffer queue
    if (rtos_queue_create(sizeof(struct fhost_rx_uf_buf_desc_tag), FHOST_RX_BUF_CNT, &fhost_rx_env.queue_uf_buf))
    {
        return 3;
    }
    #endif // NX_UF_EN

    // Create the RX task
    if (rtos_task_create(fhost_rx_task, "RX", RX_TASK, FHOST_RX_STACK_SIZE, NULL,
                         fhost_rx_priority, NULL))
    {
        return 4;
    }

    return 0;
}

void fhost_rx_set_mgmt_cb(void *arg, cb_fhost_rx cb)
{
    fhost_rx_env.mgmt_cb = cb;
    fhost_rx_env.mgmt_cb_arg = arg;
}

void fhost_rx_set_monitor_cb(void *arg, cb_fhost_rx cb)
{
    fhost_rx_env.monitor_cb = cb;
    fhost_rx_env.monitor_cb_arg = arg;
}

/// @}
