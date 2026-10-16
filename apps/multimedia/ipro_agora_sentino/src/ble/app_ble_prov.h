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

#ifndef APP_BLE_PROV_H
#define APP_BLE_PROV_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Provisioning data received from phone
 */
typedef struct {
    char user_id[64];
    char asset_id[64];
} ble_prov_data_t;

/**
 * @brief Callback when provisioning is complete
 * @param data Provisioning data (userId, assetId)
 */
typedef void (*ble_prov_done_cb_t)(const ble_prov_data_t *data);

/**
 * @brief One AP in the list the phone asks for (thing.network.getwifis)
 */
typedef struct {
    char   ssid[33];
    int8_t rssi;     /* dBm */
    bool   secure;   /* anything but open */
} ble_prov_ap_t;

/**
 * @brief Scan for APs; runs on the provisioning worker task and may block
 *        for several seconds. @return entries written to aps, or < 0
 */
typedef int (*ble_prov_wifi_scan_fn)(ble_prov_ap_t *aps, int max);

/**
 * @brief Store the Wi-Fi credentials from thing.network.set (sid / pw).
 *        Runs on the BT RX thread: no blocking. @return 0 on success
 */
typedef int (*ble_prov_wifi_creds_fn)(const char *ssid, const char *psk);

/**
 * @brief Hook the Wi-Fi half of the provisioning protocol to a network
 *        backend. Without hooks getwifis answers an empty list and sid / pw
 *        are ignored (EMAC / LTE builds). Call before app_ble_prov_init().
 */
void app_ble_prov_set_wifi_hooks(ble_prov_wifi_scan_fn scan, ble_prov_wifi_creds_fn creds);

/**
 * @brief Initialize BLE provisioning
 * @param pid    Product ID (for ADV data)
 * @param uuid   Device UUID (for ADV scan response)
 * @param done_cb Callback when bind data received
 * @return 0 on success
 */
int app_ble_prov_init(const char *pid, const char *uuid,
                      ble_prov_done_cb_t done_cb);

/**
 * @brief Start BLE advertising for provisioning
 * @return 0 on success
 */
int app_ble_prov_start(void);

/**
 * @brief Stop BLE advertising
 * @return 0 on success
 */
int app_ble_prov_stop(void);

/**
 * @brief Check if device is bound (has provisioning data)
 */
bool app_ble_prov_is_bound(void);

/**
 * @brief Send notification to connected phone
 * @param data  Payload bytes
 * @param len   Payload length
 * @return 0 on success
 */
int app_ble_prov_notify(const uint8_t *data, uint16_t len);

/**
 * @brief Send a framed response to connected phone via BLE notify
 * @param json  JSON string payload
 */
void app_ble_prov_send_framed(const char *json);

/**
 * @brief Deinitialize BLE stack (disable host + controller, free memory)
 * @note Call after provisioning complete, before Agora init
 */
void app_ble_prov_deinit(void);

/**
 * @brief Clear bind state (reset to unbound)
 */
void app_ble_prov_clear_bind(void);

/**
 * @brief Get stored user ID (valid after init if bound)
 */
const char *app_ble_prov_get_user_id(void);

/**
 * @brief Get stored asset ID (valid after init if bound)
 */
const char *app_ble_prov_get_asset_id(void);

/**
 * @brief Get stored MQTT broker hostname (from BLE JSON "mq" field)
 */
const char *app_ble_prov_get_mqtt_host(void);

/**
 * @brief Get stored MQTT broker port (from BLE JSON "port" field)
 */
uint16_t app_ble_prov_get_mqtt_port(void);

#endif /* APP_BLE_PROV_H */
