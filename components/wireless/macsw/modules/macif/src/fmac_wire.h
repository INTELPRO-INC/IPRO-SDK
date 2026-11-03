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
 * IPRO6S FMAC wire format after READY, FW <-> host, on SDIO and USB.
 * A transfer holds one or more frames, each fmac_hdr_t + len body bytes,
 * starting 4-byte aligned; a header with type 0, or the end of the
 * transfer, ends the list. Download protocol requests and acks
 * (fmac_dl.h) still pass at any time, one per transfer; they start with a
 * magic whose low 16 bits exceed any frame length. The host driver keeps
 * a copy of this file; change both together.
 */
#ifndef FMAC_WIRE_H
#define FMAC_WIRE_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#include "fmac_dl.h"

#define FMAC_WIRE_VER         2u
#define FMAC_HDR_LEN          8u
/* Frames in one host -> FW transfer, bytes: a FW receive buffer is
 * FMAC_DL_PKT_MAX and a USB sender may add 4 bytes to end it short */
#define FMAC_XFER_MAX         (FMAC_DL_PKT_MAX - 4u)
#define FMAC_BODY_MAX         (FMAC_XFER_MAX - FMAC_HDR_LEN)
/* FW -> host transfers, bytes: a host receive buffer is this large */
#define FMAC_UPLD_MAX         13824u
#define FMAC_ALIGN4(x)        (((x) + 3u) & ~3u)

enum fmac_frame_type {
    FMAC_T_END = 0,
    /* body = fmac_lmac_msg_t; host -> FW request, FW -> host cfm / ind */
    FMAC_T_MSG = 1,
    /* body = UTF-8 text, FW -> host */
    FMAC_T_DBG = 2,
    /* host -> FW: empty; FW -> host: fmac_info_t */
    FMAC_T_INFO = 3,
    /* FW -> host: fmac_rx_hdr_t, then the frame (802.3, or 802.11 when
     * FMAC_RX_80211 is set) */
    FMAC_T_RX = 4,
    /* host -> FW: fmac_tx_hdr_t, then an 802.3 frame */
    FMAC_T_TX = 5,
    /* FW -> host: fmac_txcfm_t */
    FMAC_T_TXCFM = 6,
    /* FW -> host: fmac_flow_t */
    FMAC_T_FLOW = 7,
    /* host -> FW: fmac_mgmt_hdr_t, then an 802.11 frame; takes a TX slot
     * like FMAC_T_TX. Only a FW with FMAC_FW_CAP_AP takes it */
    FMAC_T_MGMT = 8,
    /* FW -> host: fmac_mgmt_cfm_t */
    FMAC_T_MGMT_CFM = 9,
};

typedef struct {
    uint16_t len;
    uint16_t type;
    uint16_t queue;
    uint16_t rsv;
} fmac_hdr_t;

/* A KE message: the macsw struct ke_msg without its list header */
typedef struct {
    uint16_t id;
    uint16_t dest_id;
    uint16_t src_id;
    uint16_t param_len;
    uint32_t param[];
} fmac_lmac_msg_t;

/* MAC address: efuse, or a locally administered random one. tx_slots:
 * frames the host may have queued in the FW (its starting TX credit) */
typedef struct {
    uint8_t mac[6];
    uint16_t tx_slots;
    uint32_t fw_ver;
    /* Host transfers the FW can hold at once, and the FW's count of host
     * transfers handled so far (this INFO request included) */
    uint16_t dnld_slots;
    uint16_t dnld_done;
    /* Since boot, wrapping: host frames taken into TX slots, and TX slots
     * freed (fmac_txcfm_t sent or not). Free TX slots now:
     * tx_slots - (tx_taken - tx_freed) */
    uint16_t tx_taken;
    uint16_t tx_freed;
} fmac_info_t;

/* Host -> FW flow control: the host keeps at most dnld_slots transfers
 * that the FW has not handled yet (dnld_done counts them, mod 2^16) */
typedef struct {
    uint16_t dnld_done;
    uint16_t dnld_slots;
} fmac_flow_t;

/* tid 0-7 (4 and 5 go out as 6); sta_idx: the AP's station index from
 * SM_CONNECT_IND, or on an AP VIF the station's from ME_STA_ADD_CFM,
 * bcmc_idx of APM_START_CFM for group addresses */
typedef struct {
    uint16_t sta_idx;
    uint8_t tid;
    uint8_t vif_idx;
    /* 0, or on a FW with FMAC_FW_CAP_AP: the FW reports the frame's ACK
     * in an FMAC_T_MGMT_CFM with this cookie */
    uint32_t cookie;
} fmac_tx_hdr_t;

/* freed: TX slots freed since the last fmac_txcfm_t; a host frame the
 * FW dropped without a slot counts */
typedef struct {
    uint16_t freed;
    uint16_t rsv;
} fmac_txcfm_t;

/* fmac_info_t.fw_ver: version in the low 16 bits, capabilities above */
#define FMAC_FW_VER_NUM(v)    ((v) & 0xFFFFu)
#define FMAC_FW_CAP_AP        0x00010000u

#define FMAC_MGMT_NO_CCK      0x01u
#define FMAC_MGMT_NO_ACK      0x02u

typedef struct {
    uint8_t vif_idx;
    uint8_t flags;
    uint16_t rsv;
    uint32_t cookie;
} fmac_mgmt_hdr_t;

/* acked: the peer acknowledged the frame */
typedef struct {
    uint32_t cookie;
    uint8_t acked;
    uint8_t rsv[3];
} fmac_mgmt_cfm_t;

/* flags: the macsw RX flags */
#define FMAC_RX_AMSDU         0x00000001u
#define FMAC_RX_80211         0x00000002u
#define FMAC_RX_TID(f)        (((f) >> 4) & 0x7u)
#define FMAC_RX_VIF(f)        (((f) >> 8) & 0xFu)
#define FMAC_RX_STA(f)        (((f) >> 12) & 0x3FFu)

typedef struct {
    uint32_t flags;
    /* Primary 20 MHz channel, MHz */
    uint16_t freq;
    int8_t rssi;
    uint8_t rsv;
} fmac_rx_hdr_t;

_Static_assert(sizeof(fmac_hdr_t) == FMAC_HDR_LEN, "hdr size");
_Static_assert(sizeof(fmac_lmac_msg_t) == 8, "lmac msg size");
_Static_assert((FMAC_DL_REQ_MAGIC & 0xFFFFu) > FMAC_DL_PKT_MAX &&
               (FMAC_DL_ACK_MAGIC & 0xFFFFu) > FMAC_DL_PKT_MAX,
               "a dl magic must not read as a frame length");

#endif
