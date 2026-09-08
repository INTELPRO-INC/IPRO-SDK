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
 * @file throughput_service.h
 * @brief Throughput Test Service (Peripheral/Slave side)
 *
 * Custom GATT service for throughput testing using native Zephyr bt_* APIs:
 * - TX Characteristic: Slave sends data to Master (Notify)
 * - RX Characteristic: Slave receives data from Master (Write without response)
 */

#ifndef THROUGHPUT_SERVICE_H
#define THROUGHPUT_SERVICE_H

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

/// Maximum data length for throughput characteristic
#define THROUGHPUT_MAX_DATA_LEN     244

/// Service callback events
typedef enum {
    THROUGHPUT_SVC_EVENT_DATA_RECEIVED = 0,   ///< Data received from Master
    THROUGHPUT_SVC_EVENT_NOTIFY_ENABLED,      ///< Notifications enabled by Master
    THROUGHPUT_SVC_EVENT_NOTIFY_DISABLED,     ///< Notifications disabled by Master
    THROUGHPUT_SVC_EVENT_NOTIFY_COMPLETE,     ///< Notification sent successfully
} throughput_svc_event_t;

/// Service event data
typedef struct {
    throughput_svc_event_t event;
    uint8_t conn_id;
    uint16_t length;
    const uint8_t *data;
} throughput_svc_event_data_t;

/// Service callback function type
typedef void (*throughput_svc_callback_t)(const throughput_svc_event_data_t *event_data, void *user_data);

/// Service configuration
typedef struct {
    throughput_svc_callback_t callback;
    void *user_data;
} throughput_svc_config_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 * @brief Initialize throughput service
 * @param config Service configuration
 * @return 0 on success
 */
int throughput_service_init(const throughput_svc_config_t *config);

/**
 * @brief Send notification to connected master
 * @param conn Connection pointer
 * @param data Data to send
 * @param length Data length
 * @return 0 on success, -2 if congested
 */
int throughput_service_notify(struct bt_conn *conn,
                             const uint8_t *data,
                             uint16_t length);

/**
 * @brief Check if notifications are enabled
 * @return true if enabled
 */
bool throughput_service_is_notify_enabled(void);

#ifdef __cplusplus
}
#endif

#endif // THROUGHPUT_SERVICE_H
