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
 * @file ipro_ble_ctlr_arch.h
 *
 * @brief 移植層對外的介面 —— 啟動 controller,以及把 controller task 叫醒。
 ****************************************************************************************
 */

#ifndef IPRO_BLE_CTLR_ARCH_H_
#define IPRO_BLE_CTLR_ARCH_H_

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

/**
 * 啟動 BLE controller。
 *
 * 取代上游裸機版的 `rw_main()`:開時脈 → 備妥 HCI 傳輸 → `rwip_init()`
 * → 建 task → 開中斷。呼叫前 FreeRTOS 排程器不必已經啟動。
 *
 * @param[in] task_priority controller task 的優先權。它有硬性的無線電時限,
 *                          應該高於應用層的 task。
 * @return 0 成功;-1 已經啟動過;-2 建 task 失敗。
 */
int ipro_ble_ctlr_controller_init(unsigned int task_priority);

/**
 * 叫醒 controller task。
 *
 * 從 task 或 ISR 都可以呼叫 —— 內部用 `xPortIsInsideInterrupt()` 分派。
 *
 * 用途:controller 在沒事做的時候會阻塞在主迴圈裡(對應上游的 `WFI()`)。
 * 任何「讓它有事做」的外部事件都必須叫它一次,否則它會一直睡。
 * 目前的呼叫者是 `hcitl/` —— host 送了 HCI 位元組,或 host 讀走了資料
 * 讓卡住的傳輸有空間繼續。
 *
 * 在 controller 還沒啟動時呼叫是安全的(什麼都不做)。
 */
void ipro_ble_ctlr_ctrl_notify(void);
TaskHandle_t ipro_ble_ctlr_controller_task_handle(void);

/**
 * 平台深睡槽 —— 上游主迴圈 `RWIP_DEEP_SLEEP` 分支那個
 * `// add here platform specific deep sleep code` 的落點。
 *
 * 預設的 weak 版本什麼都不做,等同上游行為(CPU 停在主迴圈的阻塞點,
 * BLE IP 自己深睡,核心不掉電)。
 *
 * 🔴 IPRO7 的 PDS 只能覆寫這個函式來實作,而且實作必須放在
 * `plf/ipro7/lowpower/`。上游檔案與 `arch_main.c` 都不得出現 PDS 程式碼。
 *
 * @return true 表示平台已自行處理完這次睡眠(主迴圈不再阻塞);
 *         false 表示照上游行為走。
 */
bool ipro_ble_ctlr_plf_deep_sleep(void);
void ipro_ble_ctlr_controller_kick(void);
void ipro_ble_ctlr_tickless_connection_active_set(bool active);
bool ipro_ble_ctlr_tickless_connection_active_get(void);
bool ipro_ble_ctlr_clock_drift_uses_active(void);
void ipro_ble_ctlr_platform_sleep_drift_enable(bool enable);
void ipro_ble_ctlr_platform_sleep_drift_floor_set(uint32_t ppm);
bool ipro_ble_ctlr_pds_candidate_revalidate(uint32_t token,
                                            uint32_t *fresh_slack_hus);
bool ipro_ble_ctlr_rwip_native_sleep_complete(void);
bool ipro_ble_ctlr_rwip_finish_native_ip_wake(void);
void ipro_ble_ctlr_rwip_set_platform_pds_native_elapsed(uint32_t dur_hus);
uint32_t ipro_ble_ctlr_rwip_arb_slack_hus(void);
bool ipro_ble_ctlr_pds_complete_native_before_platform(void);

/// 主迴圈跑過的圈數。純診斷:卡住時這個數字會停。
extern volatile uint32_t ipro_ble_ctlr_loop_cnt;

/// rwip_sleep() 模式與 core-retentive wait 計數，用於低功耗保活驗收。
extern volatile uint32_t ipro_ble_ctlr_sleep_active_cnt;
extern volatile uint32_t ipro_ble_ctlr_sleep_idle_cnt;
extern volatile uint32_t ipro_ble_ctlr_sleep_deep_cnt;
extern volatile uint32_t ipro_ble_ctlr_sleep_wait_cnt;
extern volatile uint32_t ipro_ble_ctlr_sleep_wake_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_pending_seen_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_ready_seen_cnt;
extern volatile uint32_t ipro_ble_ctlr_pds_gate_mask;
extern volatile uint32_t ipro_ble_ctlr_pds_deadline_reject_cnt;
/* Atomic publication for the FreeRTOS tickless policy.  It is advisory for
 * WFI only; platform PDS must still be admitted inside the same rwip_sleep()
 * ownership transaction. */
extern volatile uint32_t ipro_ble_ctlr_tickless_slack_hus;
extern volatile uint32_t ipro_ble_ctlr_irq_deadline_samples;
extern volatile uint32_t ipro_ble_ctlr_irq_deadline_slack_hus;
extern volatile uint32_t ipro_ble_ctlr_irq_deadline_elapsed_hus;
extern volatile int32_t ipro_ble_ctlr_irq_deadline_error_hus;
extern volatile uint32_t ipro_ble_ctlr_irq_deadline_intstat;
extern volatile uint32_t ipro_ble_ctlr_irq_deadline_early;
extern volatile uint32_t ipro_ble_ctlr_irq_deadline_late;
extern volatile uint32_t ipro_ble_ctlr_irq_total;
/* CLKN, SLP, CRYPT, SW, FINE, TGT1, TGT2, TGT3, FIFO respectively. */
extern volatile uint32_t ipro_ble_ctlr_irq_bit_count[9];
/* Phase 0 is TIMESTAMPTGT1 (arbiter timer), phase 1 is FIFO. */
extern volatile uint32_t ipro_ble_ctlr_irq_phase_count[2];
extern volatile int32_t ipro_ble_ctlr_irq_phase_last_error_hus[2];
extern volatile int32_t ipro_ble_ctlr_irq_phase_min_error_hus[2];
extern volatile int32_t ipro_ble_ctlr_irq_phase_max_error_hus[2];
extern volatile uint32_t ipro_ble_ctlr_full_window_count;
extern volatile uint32_t ipro_ble_ctlr_full_window_last_hus;
extern volatile uint32_t ipro_ble_ctlr_full_window_min_hus;
extern volatile uint32_t ipro_ble_ctlr_full_window_max_hus;
extern volatile uint64_t ipro_ble_ctlr_full_window_total_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_candidate_slack_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_candidate_token;
extern volatile uint32_t ipro_ble_ctlr_pds_full_generation;
extern volatile uint32_t ipro_ble_ctlr_pds_last_slack_hus;
extern volatile uint32_t ipro_ble_ctlr_native_sleep_enabled;
extern volatile uint32_t ipro_ble_ctlr_platform_sleep_drift_enabled;
extern volatile uint32_t ipro_ble_ctlr_platform_sleep_drift_floor_ppm;

/// Build-time controller profile diagnostics.  These are intentionally public
/// so UART-only APP tests can record the exact controller archive/source
/// profile without CCI or debugger assistance.
extern const uint32_t ipro_ble_ctlr_profile_cfg_dbg;
extern const uint32_t ipro_ble_ctlr_profile_bt53;
extern const uint32_t ipro_ble_ctlr_profile_bt54;
extern const uint32_t ipro_ble_ctlr_profile_iso_con;
extern const uint32_t ipro_ble_ctlr_profile_native_sleep_default;
extern const uint32_t ipro_ble_ctlr_profile_cci_diag;
extern const uint32_t ipro_ble_ctlr_profile_trace_buffers;
extern const uint32_t ipro_ble_ctlr_profile_cis_rxwin_diag_extra_us;

/// RW BLE-IP deep-sleep timer diagnostics (nominal 32 MHz cycles).
extern volatile uint32_t ipro_ble_ctlr_rwip_sleep_programmed;
extern volatile uint32_t ipro_ble_ctlr_rwip_sleep_observed;
extern volatile uint32_t ipro_ble_ctlr_rwip_wakeup_count;
extern volatile uint32_t ipro_ble_ctlr_rwip_native_sleep_owned;
extern volatile uint32_t ipro_ble_ctlr_rwip_wakeup_reserve;
extern volatile int32_t ipro_ble_ctlr_rwip_sleep_overshoot;

extern volatile uint32_t ipro_ble_ctlr_con_evt_start_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_evt_cancel_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_cancel_clock;
extern volatile uint32_t ipro_ble_ctlr_con_cancel_target_clock;
extern volatile uint32_t ipro_ble_ctlr_con_cancel_target_hus;
extern volatile uint32_t ipro_ble_ctlr_con_evt_eof_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_evt_rx_irq_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_evt_skip_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_rx_desc_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_rx_sync_err_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_rx_crc_err_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_rx_crc_ok_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_rx_crc_gate_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_rx_invalid_gate_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_last_start_clock;
extern volatile uint32_t ipro_ble_ctlr_con_last_target_clock;
extern volatile uint32_t ipro_ble_ctlr_con_last_evt_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_tx_irq_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_tx_desc_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_tx_desc_hist[5];
extern volatile int32_t ipro_ble_ctlr_con_last_good_offset_hus;
extern volatile int32_t ipro_ble_ctlr_con_good_offset_min_hus;
extern volatile int32_t ipro_ble_ctlr_con_good_offset_max_hus;
extern volatile uint32_t ipro_ble_ctlr_con_last_good_evt_cnt;
extern volatile uint32_t ipro_ble_ctlr_con_last_good_clock;
extern volatile uint32_t ipro_ble_ctlr_con_last_rx_win_hus;
extern volatile uint32_t ipro_ble_ctlr_con_last_rx_win_add_hus;
extern volatile uint32_t ipro_ble_ctlr_con_last_interval_hs;
extern volatile uint32_t ipro_ble_ctlr_con_last_good_channel;
extern volatile uint32_t ipro_ble_ctlr_con_last_good_expected_hs;
extern volatile uint32_t ipro_ble_ctlr_con_last_good_expected_hus;
extern volatile uint32_t ipro_ble_ctlr_con_last_good_sync_hus;
extern volatile uint32_t ipro_ble_ctlr_con_last_fail_channel;
extern volatile uint32_t ipro_ble_ctlr_con_last_fail_status;
extern volatile uint32_t ipro_ble_ctlr_con_pending_instant_type;
extern volatile uint32_t ipro_ble_ctlr_con_pending_instant_evt;
extern volatile uint32_t ipro_ble_ctlr_con_applied_instant_type;
extern volatile uint32_t ipro_ble_ctlr_con_applied_instant_evt;
extern volatile uint32_t ipro_ble_ctlr_con_chmap_lo;
extern volatile uint32_t ipro_ble_ctlr_con_chmap_hi;
extern volatile uint32_t ipro_ble_ctlr_con_hop_sel_1;
extern volatile uint32_t ipro_ble_ctlr_con_last_hw_channel;
extern volatile uint32_t ipro_ble_ctlr_con_deep_sleep_guard;
extern volatile uint32_t ipro_ble_ctlr_con_instant_active;
extern volatile uint32_t ipro_ble_ctlr_con_deep_sleep_recovery;

/// 上游 assert 命中次數。量產前應為 0。
extern volatile uint32_t ipro_ble_ctlr_assert_err_cnt;
extern volatile uint32_t ipro_ble_ctlr_assert_param_cnt;
extern volatile uint32_t ipro_ble_ctlr_assert_warn_cnt;

#endif /* IPRO_BLE_CTLR_ARCH_H_ */
