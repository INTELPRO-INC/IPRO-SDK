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

#ifndef IPRO_BLE_CTLR_IPRO6S_DIAG_H
#define IPRO_BLE_CTLR_IPRO6S_DIAG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint32_t dbg_em_sentinel_samples;
extern volatile uint32_t dbg_em_sentinel_last_good_poll;
extern volatile uint32_t dbg_em_sentinel_last_good_bt_diag;
extern volatile uint32_t dbg_em_sentinel_fail_count;
extern volatile uint32_t dbg_em_sentinel_fail_phase;
extern volatile uint32_t dbg_em_sentinel_fail_poll;
extern volatile uint32_t dbg_em_sentinel_fail_value;
extern volatile uint32_t dbg_em_sentinel_fail_bt_intstat;
extern volatile uint32_t dbg_em_sentinel_fail_bt_error;
extern volatile uint32_t dbg_em_sentinel_fail_bt_diag;
extern volatile uint32_t dbg_em_sentinel_fail_bt_actfifo;
extern volatile uint32_t dbg_em_sentinel_fail_bt_rxdesc;
extern volatile uint32_t dbg_em_sentinel_fail_wlp1;
extern volatile uint32_t dbg_em_sentinel_fail_sram3;
extern volatile uint32_t dbg_em_sentinel_fail_syscfg0;

#define IPRO6S_EM_TRACE_COUNT 32u

struct ipro6s_em_trace_entry {
    uint32_t sequence_phase;
    uint32_t poll_count;
    uint32_t sentinel_value;
    uint32_t bt_diagstat;
    uint32_t ip_intstat1;
    uint32_t bt_intstat0;
    uint32_t bt_actfifostat;
    uint32_t bt_currentrxdesc;
    uint32_t scheduler_push_count;
    uint32_t scheduler_pre_state;
    uint32_t scheduler_csptr_expected;
    uint32_t scheduler_extab;
    uint32_t scheduler_cs_frcntl;
    uint32_t scheduler_cs_linkcntl;
    uint32_t acl_tx_state;
    uint32_t acl_tx_buf_state;
};

extern volatile uint32_t dbg_em_trace_write_count;
extern volatile uint32_t dbg_em_trace_frozen_count;
extern volatile struct ipro6s_em_trace_entry
    dbg_em_trace[IPRO6S_EM_TRACE_COUNT];

#define IPRO6S_CON_DIAG_MAGIC        0x36434447u
#define IPRO6S_CON_DIAG_VERSION      3u
#define IPRO6S_CON_DIAG_SAMPLE_COUNT 8u
#define IPRO6S_CON_DIAG_CHANNEL_COUNT 37u

enum ipro6s_con_diag_sample_flags {
    IPRO6S_CON_DIAG_EVT_START = (1u << 0),
    IPRO6S_CON_DIAG_EVT_EOF   = (1u << 1),
    IPRO6S_CON_DIAG_RX_DESC   = (1u << 2),
    IPRO6S_CON_DIAG_RX_SYNC   = (1u << 3),
    IPRO6S_CON_DIAG_RX_CRC_OK = (1u << 4),
    IPRO6S_CON_DIAG_RX_CRCERR = (1u << 5),
    IPRO6S_CON_DIAG_RX_SYNERR = (1u << 6),
    IPRO6S_CON_DIAG_EVT_ABORT = (1u << 7),
};

struct ipro6s_con_diag_sample {
    uint32_t start_clock;
    uint32_t scheduled_hs;
    uint32_t eof_timestamp;
    uint32_t rx_sync_hs;
    uint32_t last_crc_ok_ts;
    uint32_t rf_sdm_hw;
    uint32_t agc_status;
    uint16_t event_counter;
    uint16_t hw_event_counter;
    uint16_t rxstat;
    uint16_t rxphce;
    uint16_t rxchass;
    uint16_t flags;
    uint8_t used_channel_index;
    uint8_t physical_channel;
    uint8_t hop_channel;
    uint8_t reserved;
};

struct ipro6s_con_diag {
    uint32_t magic;
    uint32_t version;
    uint32_t event_start_count;
    uint32_t event_eof_count;
    uint32_t event_cancel_count;
    uint32_t event_skip_count;
    uint32_t rx_desc_count;
    uint32_t rx_crc_ok_count;
    uint32_t rx_crc_err_count;
    uint32_t rx_sync_err_count;
    uint32_t consecutive_no_crc;
    uint32_t max_consecutive_no_crc;
    uint32_t freeze_count;
    uint32_t freeze_reason;
    uint32_t freeze_clock;
    uint32_t freeze_next_ts;
    uint32_t freeze_last_sync_ts;
    uint32_t freeze_last_crc_ok_ts;
    uint32_t freeze_interval_hs;
    uint32_t freeze_timeout_hs;
    uint16_t freeze_event_counter;
    uint16_t freeze_event_increment;
    uint16_t freeze_link_info;
    uint16_t freeze_state;
    uint16_t channel_index_oor_count;
    uint16_t reserved;
    uint32_t channel_map_lo;
    uint8_t channel_map_hi;
    uint8_t channel_count;
    uint16_t channel_total[IPRO6S_CON_DIAG_CHANNEL_COUNT];
    uint16_t channel_ok[IPRO6S_CON_DIAG_CHANNEL_COUNT];
    uint16_t channel_crc_err[IPRO6S_CON_DIAG_CHANNEL_COUNT];
    uint16_t channel_sync_err[IPRO6S_CON_DIAG_CHANNEL_COUNT];
    struct ipro6s_con_diag_sample live[IPRO6S_CON_DIAG_SAMPLE_COUNT];
    struct ipro6s_con_diag_sample frozen[IPRO6S_CON_DIAG_SAMPLE_COUNT];
};

extern volatile struct ipro6s_con_diag ipro6s_con_diag;

#ifdef __cplusplus
}
#endif

#endif
