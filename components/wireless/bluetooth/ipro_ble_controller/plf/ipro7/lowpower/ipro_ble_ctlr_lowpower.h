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
 * @file ipro_ble_ctlr_lowpower.h
 *
 * @brief IPRO7 ipro_ble_controller low-power policy and diagnostics.
 ****************************************************************************************
 */

#ifndef IPRO_BLE_CTLR_LOWPOWER_H_
#define IPRO_BLE_CTLR_LOWPOWER_H_

#include <stdint.h>

/* rwip_sleep()/platform-PDS accounting.  A true platform PDS entry is accepted
 * only after APP/FreeRTOS tickless has granted a full BLE idle window; otherwise
 * the controller task falls back to core-retentive WFI with BLE timing kept on
 * XTAL32M. */
extern volatile uint32_t ipro_ble_ctlr_lp_deep_requests;
extern volatile uint32_t ipro_ble_ctlr_lp_32k_xtal;
extern volatile int32_t ipro_ble_ctlr_lp_32k_ppm;
extern volatile uint32_t ipro_ble_ctlr_lp_32k_fallback;
extern volatile uint32_t ipro_ble_ctlr_lp_sleep_drift_ppm;

/* Select and qualify the BLE sleep clock before rwip_init().  This value is
 * used only by RW native sleep / platform-PDS elapsed-time handling.  Plain
 * no-sleep operation stays on XTAL32M and uses active-clock drift instead.
 * XTAL32K is preferred; RC32K is calibrated and selected if XTAL32K exceeds
 * 500 ppm or does not oscillate. Returns zero/cap code, or a negative error. */
int ipro_ble_ctlr_lowpower_init(void);

/* Isolated rwip_driver patch entry point. */
void ipro_ble_ctlr_rwip_set_lp_clock_ppm(int32_t ppm);

/* Return the positive sleep-clock drift bound that should be advertised to
 * RWIP through PARAM_ID_LPCLK_DRIFT.  This is an accuracy bound, not the signed
 * residual used by ipro_ble_ctlr_rwip_set_lp_clock_ppm(). */
uint16_t ipro_ble_ctlr_lowpower_sleep_drift_ppm_get(void);

#endif /* IPRO_BLE_CTLR_LOWPOWER_H_ */
