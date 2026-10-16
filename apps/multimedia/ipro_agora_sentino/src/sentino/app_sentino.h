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

#ifndef APP_SENTINO_H
#define APP_SENTINO_H

#include <stdint.h>

/* Sentino MQTT broker configuration */
#define SENTINO_MQTT_HOST     "mqtt-iot.sentino.jp"
#define SENTINO_MQTT_PORT     1883
#define SENTINO_MQTT_KEEPALIVE 60     /* seconds */

/**
 * @brief Message handler callback type for incoming MQTT messages
 * @param code  Event/command code string
 * @param payload  Full JSON payload (mutable, may be modified by parser)
 */
typedef void (*sentino_msg_handler_t)(const char *code, char *payload);

/**
 * @brief Initialize Sentino MQTT client and connect to broker
 *
 * Requires network to be ready (call after network_wait_ready).
 * Spawns a FreeRTOS task for MQTT processing and reconnect.
 *
 * @return 0 on success, negative on failure
 */
int app_sentino_init(void);

/**
 * @brief Check if Sentino MQTT is connected
 * @return 1 if connected, 0 otherwise
 */
int app_sentino_is_connected(void);

/**
 * @brief Publish a report event to Sentino cloud
 * @param code      Event code (e.g. "info", "time", "reset")
 * @param data_json Inner JSON for data field (without braces), or NULL
 * @param ack       0 = no reply needed, 1 = reply expected
 * @return 0 on success, negative on failure
 */
int app_sentino_report(const char *code, const char *data_json, int ack);

/**
 * @brief Publish raw payload to a specific topic
 * @param topic Full topic string
 * @param payload JSON payload string
 * @return 0 on success, negative on failure
 */
int app_sentino_publish(const char *topic, const char *payload);

/**
 * @brief Register handler for incoming issue commands
 * @param handler Callback for dispatching commands by code
 */
void app_sentino_set_issue_handler(sentino_msg_handler_t handler);

/**
 * @brief Register handler for report responses
 * @param handler Callback for dispatching responses by code
 */
void app_sentino_set_response_handler(sentino_msg_handler_t handler);

/**
 * @brief Publish an issue response (echo back with res=0)
 * @param issue_payload Original issue JSON payload (extracts id, code, data)
 * @return 0 on success, negative on failure
 */
int app_sentino_issue_response(const char *issue_payload);

/**
 * @brief Get the device triple info (for other modules to build topics)
 * @param pid  Output: product ID
 * @param uuid Output: device UUID
 */
void app_sentino_get_device_info(const char **pid, const char **uuid);

/**
 * @brief Disconnect and cleanup Sentino MQTT client
 */
void app_sentino_deinit(void);

#endif /* APP_SENTINO_H */
