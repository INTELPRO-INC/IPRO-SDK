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

#ifndef IPRO_BLE_CTLR_PDS_SNAPSHOT_H_
#define IPRO_BLE_CTLR_PDS_SNAPSHOT_H_

#include <stdbool.h>
#include <stdint.h>

#define IPRO_BLE_CTLR_PDS_CONTEXT_MAGIC   UINT32_C(0x42545044) /* "BTPD" */
#define IPRO_BLE_CTLR_PDS_CONTEXT_VERSION UINT16_C(1)

enum ipro_ble_ctlr_pds_state {
    IPRO_BLE_CTLR_PDS_DISABLED = 0,
    IPRO_BLE_CTLR_PDS_WFI_BASELINE,
    IPRO_BLE_CTLR_PDS_QUIESCE,
    IPRO_BLE_CTLR_PDS_SNAPSHOT_READY,
    IPRO_BLE_CTLR_PDS_ASLEEP,
    IPRO_BLE_CTLR_PDS_SYSTEM_RESTORED,
    IPRO_BLE_CTLR_PDS_CONTROLLER_RESTORE,
    IPRO_BLE_CTLR_PDS_FIRST_EVENT_RECOVERY,
    IPRO_BLE_CTLR_PDS_SYNCED,
    IPRO_BLE_CTLR_PDS_FALLBACK_WFI,
};

struct ipro_ble_ctlr_pds_diag {
    volatile uint32_t enter_count;
    volatile uint32_t wake_count;
    volatile uint32_t restore_count;
    volatile uint32_t failure_count;
    volatile uint32_t fallback_count;
    volatile uint32_t register_mismatch_count;
    volatile uint32_t requested_us;
    volatile uint32_t elapsed_us;
    volatile uint32_t snapshot_us;
    volatile uint32_t restore_us;
    volatile uint32_t clock_saved_hs;
    volatile uint32_t phase_saved_hus;
    volatile uint32_t rtc_delta_us;
    volatile uint32_t rtc_corrected_us;
    volatile uint32_t clock_corrected_hs;
    volatile uint32_t phase_corrected_hus;
    volatile uint32_t clock_expected_hs;
    volatile uint32_t phase_expected_hus;
    volatile int32_t clock_error_hus;
    volatile uint32_t state;
};

extern struct ipro_ble_ctlr_pds_diag ipro_ble_ctlr_pds_diag;
extern volatile uint32_t ipro_ble_ctlr_pds_probe_run_calls;
extern volatile uint32_t ipro_ble_ctlr_pds_probe_consume_count;
extern volatile uint32_t ipro_ble_ctlr_pds_auto_attempt_count;
extern volatile uint32_t ipro_ble_ctlr_pds_auto_enter_count;
extern volatile uint32_t ipro_ble_ctlr_pds_auto_enter_evt;
extern volatile uint32_t ipro_ble_ctlr_pds_auto_enter_instant;
extern volatile uint32_t ipro_ble_ctlr_pds_auto_enter_guard;
extern volatile uint32_t ipro_ble_ctlr_pds_actual_enter_total;
extern volatile uint32_t ipro_ble_ctlr_pds_actual_wake_total;
extern volatile uint32_t ipro_ble_ctlr_pds_actual_restore_total;
extern volatile uint32_t ipro_ble_ctlr_pds_actual_failure_total;
extern volatile uint32_t ipro_ble_ctlr_pds_wall_raw_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_wall_comp_hus;
extern volatile int32_t ipro_ble_ctlr_pds_wall_comp_ppm;
extern volatile uint32_t ipro_ble_ctlr_pds_clean_rx_gate_reject_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_clean_rx_gate_refresh_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_clean_event_gate_reject_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_clean_event_gate_last_delta;
extern volatile uint32_t ipro_ble_ctlr_pds_tickless_reject_policy_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_tickless_reject_instant_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_tickless_reject_deep_guard_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_tickless_reject_recovery_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_tickless_reject_state_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_tickless_reject_clean_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_life_armed;
extern volatile uint32_t ipro_ble_ctlr_pds_life_auto_enabled;
extern volatile uint32_t ipro_ble_ctlr_pds_life_auto_connected;
extern volatile uint32_t ipro_ble_ctlr_pds_life_cycle_auto;
extern volatile uint32_t ipro_ble_ctlr_pds_life_tickless_grant;
extern volatile uint32_t ipro_ble_ctlr_pds_life_pending_platform_body;
extern volatile uint32_t ipro_ble_ctlr_pds_life_last_req_us;
extern volatile uint32_t ipro_ble_ctlr_pds_life_last_elapsed_us;
extern volatile int32_t ipro_ble_ctlr_pds_life_last_rc;
extern volatile uint32_t ipro_ble_ctlr_pds_life_last_evt;
extern volatile uint32_t ipro_ble_ctlr_pds_life_last_state;
extern volatile uint32_t ipro_ble_ctlr_pds_life_last_slack_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_life_last_required_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_native_aon_programmed_cycles;
extern volatile uint32_t ipro_ble_ctlr_pds_native_aon_deadline_us;
extern volatile uint32_t ipro_ble_ctlr_pds_native_aon_sleep_us;
extern volatile uint32_t ipro_ble_ctlr_pds_native_aon_lead_cycles;
extern volatile uint32_t ipro_ble_ctlr_pds_native_deadline_rtc_low;
extern volatile uint32_t ipro_ble_ctlr_pds_native_deadline_rtc_high;
extern volatile uint32_t ipro_ble_ctlr_pds_native_deadline_arm_delta_ticks;
extern volatile uint32_t ipro_ble_ctlr_pds_native_deadline_override_state;
extern volatile uint32_t ipro_ble_ctlr_pds_native_precomplete_count;
extern volatile uint32_t ipro_ble_ctlr_pds_native_precomplete_ok_count;
extern volatile uint32_t ipro_ble_ctlr_pds_native_precomplete_fail_count;
extern volatile uint32_t ipro_ble_ctlr_pds_native_precomplete_ds_before;
extern volatile uint32_t ipro_ble_ctlr_pds_native_precomplete_ds_after;
extern volatile uint32_t ipro_ble_ctlr_pds_native_precomplete_intstat_after;
extern volatile uint32_t ipro_ble_ctlr_pds_retained_native_ds_at_hal_return;
extern volatile uint32_t ipro_ble_ctlr_pds_retained_native_intstat_at_hal_return;
extern volatile uint32_t ipro_ble_ctlr_pds_retained_native_wake_ctrl_at_hal_return;
extern volatile uint32_t ipro_ble_ctlr_pds_retained_native_clk_cfg2_at_hal_return;
extern volatile uint32_t ipro_ble_ctlr_pds_retained_native_rtc_at_hal_return;
extern volatile uint32_t ipro_ble_ctlr_pds_retained_timer_wait_count;
extern volatile uint32_t ipro_ble_ctlr_pds_retained_timer_wait_ok_count;
extern volatile uint32_t ipro_ble_ctlr_pds_retained_timer_wait_fail_count;
extern volatile uint32_t ipro_ble_ctlr_pds_retained_timer_wait_ticks;
extern volatile uint32_t ipro_ble_ctlr_pds_retained_timer_ds_before;
extern volatile uint32_t ipro_ble_ctlr_pds_retained_timer_ds_after;
extern volatile uint32_t ipro_ble_ctlr_corr_tuple_wall_hus;
extern volatile uint32_t ipro_ble_ctlr_corr_tuple_expected_hs;
extern volatile uint32_t ipro_ble_ctlr_corr_tuple_expected_hus;
extern volatile uint32_t ipro_ble_ctlr_corr_tuple_current_hs;
extern volatile uint32_t ipro_ble_ctlr_corr_tuple_current_hus;
extern volatile uint32_t ipro_ble_ctlr_corr_tuple_delta_hs;
extern volatile int32_t ipro_ble_ctlr_corr_tuple_signed_hus;
extern volatile uint32_t ipro_ble_ctlr_corr_tuple_effective_hus;
extern volatile uint32_t ipro_ble_ctlr_corr_tuple_encoded_hs;
extern volatile uint32_t ipro_ble_ctlr_corr_tuple_encoded_fine;
extern volatile uint32_t ipro_ble_ctlr_pds_corr_irq_seen;
extern uint32_t ipro_ble_ctlr_platform_pds_effective_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_phase_advance_call_count;
extern volatile uint32_t ipro_ble_ctlr_pds_phase_advance_nonzero_count;
extern volatile uint32_t ipro_ble_ctlr_pds_phase_advance_first_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_phase_advance_last_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_phase_advance_max_hus;
extern volatile int32_t ipro_ble_ctlr_pds_phase_advance_last_error_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_wait_count;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_wait_fail_count;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_last_link;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_last_evt_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_last_old_hs;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_last_old_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_last_new_hs;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_last_new_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_last_rxwin_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_last_phase_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_last_bandwidth_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_last_prog_updated;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_late_skip_count;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_late_skip_last;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_late_skip_last_now;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_late_skip_last_target;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_prog_examined_mask;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_prog_last_et_idx;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_prog_last_dummy;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_prog_last_csptr;
extern volatile uint32_t ipro_ble_ctlr_pds_retime_prog_target_csptr;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_rearm_count;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_rearm_wait_empty;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_rearm_evt_prog_present;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_rearm_immediate_before;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_rearm_immediate_after;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_rearm_current_hs;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_rearm_current_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_rearm_first_hs;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_rearm_first_hus;
extern volatile int32_t ipro_ble_ctlr_pds_scharb_rearm_dist_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_count;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_wait_empty;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_evt_prog_present;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_current_hs;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_current_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_first_hs;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_first_hus;
extern volatile int32_t ipro_ble_ctlr_pds_scharb_start_last_dist_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_last_branch;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_break_count;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_pop_count;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_cb_count;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_cancel_count;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_end_count;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_started_hs;
extern volatile uint32_t ipro_ble_ctlr_pds_scharb_start_started_hus;
extern volatile uint32_t ipro_ble_ctlr_first_anchor_valid;
extern volatile uint32_t ipro_ble_ctlr_first_anchor_evt;
extern volatile uint32_t ipro_ble_ctlr_first_anchor_pre_hs;
extern volatile uint32_t ipro_ble_ctlr_first_anchor_pre_hus;
extern volatile uint32_t ipro_ble_ctlr_first_anchor_sync_hs;
extern volatile uint32_t ipro_ble_ctlr_first_anchor_sync_hus;
extern volatile int32_t ipro_ble_ctlr_first_anchor_jump_hus;
extern volatile uint32_t ipro_ble_ctlr_first_anchor_post_hs;
extern volatile uint32_t ipro_ble_ctlr_first_anchor_suppressed;
extern volatile uint32_t ipro_ble_ctlr_rxcr_pre_rwblecntl;
extern volatile uint32_t ipro_ble_ctlr_rxcr_pre_radiopwrupdn0;
extern volatile uint32_t ipro_ble_ctlr_rxcr_pre_radiotxrxtim0;
extern volatile uint32_t ipro_ble_ctlr_rxcr_pre_coexifcntl2;
extern volatile uint32_t ipro_ble_ctlr_rxcr_first_rwblecntl;
extern volatile uint32_t ipro_ble_ctlr_rxcr_first_radiopwrupdn0;
extern volatile uint32_t ipro_ble_ctlr_rxcr_first_radiotxrxtim0;
extern volatile uint32_t ipro_ble_ctlr_rxcr_first_coexifcntl2;
extern volatile uint32_t ipro_ble_ctlr_pds_gpio_marker_state;
extern volatile uint32_t ipro_ble_ctlr_pds_gpio_marker_pulse_count[4];

enum ipro_ble_ctlr_pds_debug_con_state_region {
    IPRO_BLE_CTLR_PDS_DBG_CON_PRE = 0,
    IPRO_BLE_CTLR_PDS_DBG_CON_PRE_RX_IRQ,
    IPRO_BLE_CTLR_PDS_DBG_CON_PRE_RX_DESC,
    IPRO_BLE_CTLR_PDS_DBG_CON_FIRST_CALLBACK,
    IPRO_BLE_CTLR_PDS_DBG_CON_CALLBACK_TIMING,
    IPRO_BLE_CTLR_PDS_DBG_CON_FRAME,
    IPRO_BLE_CTLR_PDS_DBG_CON_RX_IRQ,
    IPRO_BLE_CTLR_PDS_DBG_CON_RX_DESC,
    IPRO_BLE_CTLR_PDS_DBG_CON_PRE_CS,
    IPRO_BLE_CTLR_PDS_DBG_CON_FIRST_CS,
};

void ipro_ble_ctlr_pds_snapshot_init(void);
int ipro_ble_ctlr_pds_enable(bool enable);
int ipro_ble_ctlr_pds_auto_config(bool enable, uint32_t sleep_us);
int ipro_ble_ctlr_pds_tickless_grant(uint32_t sleep_us);
void ipro_ble_ctlr_pds_auto_connected(bool connected);
bool ipro_ble_ctlr_pds_auto_enabled(void);
bool ipro_ble_ctlr_pds_snapshot_enabled(void);
/* Measure the reconstructed controller fast timebase against the always-on
 * 32 kHz RTC.  Returns false until a platform-PDS restore established a
 * reference pair. */
bool ipro_ble_ctlr_pds_clock_drift(int32_t *error_hus, int32_t *ppm,
                                  uint32_t *elapsed_ms);
int ipro_ble_ctlr_pds_probe_arm(uint32_t sleep_us, bool connected);
bool ipro_ble_ctlr_pds_probe_pending(void);
bool ipro_ble_ctlr_pds_connected(void);
bool ipro_ble_ctlr_pds_rw_sleep_required(void);
void ipro_ble_ctlr_pds_breadcrumb_set(uint32_t stage, uint32_t data);
void ipro_ble_ctlr_pds_breadcrumb_report_and_clear(void);
void ipro_ble_ctlr_pds_gpio_marker_set(uint32_t index, uint32_t level);
void ipro_ble_ctlr_pds_gpio_marker_pulse(uint32_t index);
uint32_t ipro_ble_ctlr_pds_probe_requested_us(void);
void ipro_ble_ctlr_pds_probe_dump(void);
void ipro_ble_ctlr_pds_post_loop_checkpoint(void);
uint32_t ipro_ble_ctlr_pds_debug_con_state_word(uint32_t region,
                                                uint32_t index);

/* True only between a platform-PDS restore and the first successfully
 * programmed advertising event.  During this short recovery window the
 * controller task periodically services the retained arbiter queue, because
 * the BLE target comparator itself is part of the reset register domain. */
bool ipro_ble_ctlr_pds_arbiter_recovery_active(void);
bool ipro_ble_ctlr_pds_connected_recovery(void);
bool ipro_ble_ctlr_pds_connected_csa_override(void);
uint32_t ipro_ble_ctlr_pds_connected_phase_advance_hus(void);
uint32_t ipro_ble_ctlr_pds_recovery_rx_local_drift_ppm(uint32_t interval_hs);
bool ipro_ble_ctlr_pds_arbiter_target_due(void);
void ipro_ble_ctlr_pds_recovery_heartbeat(void);
/* Complete the two-stage post-reset wake: after the deliberately early
 * recovery IRQ drains SLP/CLKN state, restore the scheduler's retained TGT1. */
void ipro_ble_ctlr_pds_rearm_normal_target(void);

#endif /* IPRO_BLE_CTLR_PDS_SNAPSHOT_H_ */
