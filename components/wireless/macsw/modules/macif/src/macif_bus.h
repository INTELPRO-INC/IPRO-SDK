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
 ****************************************************************************************
 *
 * @file macif_bus.h
 *
 * @brief Host side of macif_bus.c (CONFIG_IPRO6_FMAC): the FullMAC firmware
 * app implements these to move KE messages and frames over its bus.
 *
 ****************************************************************************************
 */
#ifndef _MACIF_BUS_H_
#define _MACIF_BUS_H_

#include <stdbool.h>
#include <stdint.h>


/// Sends a KE message to the host: body is the message from its id on,
/// len bytes. isr: called from the Wi-Fi ISR. The app frees msg with
/// macif_bus_msg_done() once body is sent
void fmac_host_msg_fwd(void *msg, const void *body, uint32_t len, bool isr);
struct me_config_req;
struct mm_start_req;
/// Fills the default ME_CONFIG / MM_START parameters (macif_bus_cfg.c)
void macif_bus_cfg_default(struct me_config_req *me_config, struct mm_start_req *start);

/// Frees a message fmac_host_msg_fwd() handed over
void macif_bus_msg_done(void *msg);
/// Pushes a host KE message (id, dest_id, src_id, param_len, param) to
/// the Wi-Fi task; returns 0, or -1 when malformed or out of memory
int macif_bus_msg_push(const void *body, uint32_t len);

/// A received frame waits for the host (see macif_bus_rx_peek). isr: under
/// a MAC reset, interrupts are off
void fmac_host_rx_ready(bool isr);
/// Oldest received frame for the host: 0 when none. hdr gets flags, freq
/// and RSSI, *frame / *len the frame, *id names it for macif_bus_rx_done
int macif_bus_rx_peek(void *hdr, const uint8_t **frame, uint32_t *len,
                      uint32_t *id);
/// The frame macif_bus_rx_peek returned is sent: its buffer is free again
void macif_bus_rx_done(uint32_t id);

/// Queues one host 802.3 frame (hdr = fmac_tx_hdr_t) for the MAC; -1 when
/// malformed or no TX slot is free
int macif_bus_tx_push(const void *hdr, const uint8_t *eth, uint32_t len);
/// TX slots, all free at start
uint32_t macif_bus_tx_slots(void);
/// Prints MAC HW MIB counter deltas since the last call
void macif_bus_mib_diag(void);
/// Prints LMAC BA/BAR deltas and the BaW gate totals
void macif_bus_ba_diag(void);
/// RX pool: flow-off count, then since the last call the lowest free
/// count and most buffers held by reorder, then free now, A-MSDU checks
/// and A-MSDU flow-offs (6 words)
void macif_bus_rx_diag(uint32_t *v);
/// TX counters: pushed, confirmed, SW-retry drops, free slots, then per AC
/// in-flight | pending << 8, then confirms without ACK (9 words)
void macif_bus_tx_diag(uint32_t *v);
/// TX slots freed since the last call
uint32_t macif_bus_tx_freed_take(void);
/// A TX slot was freed. isr: under a MAC reset, interrupts are off
void fmac_host_tx_cfm_ready(bool isr);
/// FMAC_T_MGMT_CFM for a host management frame
void fmac_host_mgmt_cfm(uint32_t cookie, bool acked, bool isr);
/// FMAC_T_MGMT: fmac_mgmt_hdr_t + an 802.11 frame; 0 when sent to the MAC
int macif_bus_mgmt_push(const uint8_t *body, uint32_t len);

#endif // _MACIF_BUS_H_
