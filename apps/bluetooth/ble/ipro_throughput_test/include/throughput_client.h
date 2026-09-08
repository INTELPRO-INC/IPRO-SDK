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
 * @file throughput_client.h
 * @brief Throughput Test Client (Central/Master side)
 *
 * GATT client for discovering and interacting with the throughput service.
 * Uses native Zephyr bt_gatt_discover / bt_gatt_subscribe / bt_gatt_write APIs.
 */

#ifndef THROUGHPUT_CLIENT_H
#define THROUGHPUT_CLIENT_H

#include <stdint.h>
#include <stdbool.h>

struct bt_conn;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * DEFINITIONS
 ****************************************************************************************
 */

/// Client callback events
typedef enum {
    THROUGHPUT_CLIENT_EVENT_DISCOVERY_COMPLETE = 0,  ///< Service discovery complete
    THROUGHPUT_CLIENT_EVENT_DISCOVERY_FAILED,        ///< Service discovery failed
    THROUGHPUT_CLIENT_EVENT_DATA_RECEIVED,           ///< Data received via notification
    THROUGHPUT_CLIENT_EVENT_WRITE_COMPLETE,          ///< Write command sent
    THROUGHPUT_CLIENT_EVENT_NOTIFY_ENABLED,          ///< Notifications enabled
} throughput_client_event_t;

/// Client event data
typedef struct {
    throughput_client_event_t event;
    uint8_t conn_id;
    uint16_t length;
    const uint8_t *data;
    uint16_t handle;
} throughput_client_event_data_t;

/// Client callback function type
typedef void (*throughput_client_callback_t)(const throughput_client_event_data_t *event_data, void *user_data);

/// Client configuration
typedef struct {
    throughput_client_callback_t callback;
    void *user_data;
} throughput_client_config_t;

/// Discovered service handles
typedef struct {
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t tx_char_handle;      ///< Handle to receive notifications from slave
    uint16_t tx_cccd_handle;      ///< CCCD to enable notifications
    uint16_t rx_char_handle;      ///< Handle to write data to slave
    bool discovery_complete;
} throughput_client_handles_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 * @brief Initialize throughput client
 * @param config Client configuration
 * @return 0 on success
 */
int throughput_client_init(const throughput_client_config_t *config);

/**
 * @brief Start service discovery
 * @param conn Connection pointer
 * @return 0 on success
 */
int throughput_client_discover(struct bt_conn *conn);

/**
 * @brief Enable notifications from slave
 * @param conn Connection pointer
 * @return 0 on success
 */
int throughput_client_enable_notify(struct bt_conn *conn);

/**
 * @brief Write data to slave (write without response)
 * @param conn Connection pointer
 * @param data Data to write
 * @param length Data length
 * @return 0 on success
 */
int throughput_client_write(struct bt_conn *conn,
                           const uint8_t *data,
                           uint16_t length);

/**
 * @brief Get discovered handles
 * @return Pointer to discovered handles
 */
const throughput_client_handles_t *throughput_client_get_handles(void);

/**
 * @brief Check if discovery is complete
 * @return true if complete
 */
bool throughput_client_is_ready(void);

/**
 * @brief Deinitialize throughput client
 * @return 0 on success
 */
int throughput_client_deinit(void);

#ifdef __cplusplus
}
#endif

#endif // THROUGHPUT_CLIENT_H
