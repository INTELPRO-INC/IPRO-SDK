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

#ifndef __BLE_TRANSPARENT_H__
#define __BLE_TRANSPARENT_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    BLE_STATE_IDLE = 0,
    BLE_STATE_ADVERTISING,
    BLE_STATE_CONNECTED,
} ble_state_t;

typedef void (*ble_rx_callback_t)(const uint8_t *data, uint16_t len);

/* Core */
int ble_transparent_init(void);
int ble_transparent_send(const uint8_t *data, uint16_t len);
void ble_transparent_set_rx_cb(ble_rx_callback_t cb);

/* Device name */
int ble_transparent_set_name(const char *name);
const char *ble_transparent_get_name(void);

/* Advertising */
int ble_transparent_adv_start(void);
int ble_transparent_adv_stop(void);
int ble_transparent_set_adv_interval(uint16_t interval_ms);
uint16_t ble_transparent_get_adv_interval(void);
int ble_transparent_set_adv_data(const uint8_t *data, uint16_t len);
int ble_transparent_set_scan_rsp_data(const uint8_t *data, uint16_t len);

/* Connection */
int ble_transparent_disconnect(void);
ble_state_t ble_transparent_get_state(void);
int ble_transparent_set_conn_params(uint16_t min_ms, uint16_t max_ms);
uint16_t ble_transparent_get_mtu(void);
int ble_transparent_get_rssi(int8_t *rssi);
int ble_transparent_get_conn_info(uint16_t *interval, uint16_t *latency,
                                   uint16_t *timeout, uint16_t *mtu);

/* TX power */
int ble_transparent_set_tx_power(int8_t dbm);
int8_t ble_transparent_get_tx_power(void);

/* Security */
int ble_transparent_set_bondable(bool enable);
bool ble_transparent_get_bondable(void);
int ble_transparent_set_passkey(uint32_t passkey);
uint32_t ble_transparent_get_passkey(void);
int ble_transparent_set_security(uint8_t level);
int ble_transparent_clear_bonds(void);
int ble_transparent_get_bonds(char *buf, uint16_t buf_size);

/* PHY */
int ble_transparent_set_phy(uint8_t phy);
int ble_transparent_get_phy(uint8_t *tx_phy, uint8_t *rx_phy);

/* Data Length Extension */
int ble_transparent_set_data_length(uint16_t tx_octets, uint16_t tx_time);

/* Address */
int ble_transparent_get_addr(char *buf, uint16_t buf_size);

/* Sleep */
int ble_transparent_enter_sleep(void);

#endif /* __BLE_TRANSPARENT_H__ */
