/*
 * SPDX-FileCopyrightText: 2025 IPRO Technology Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * IPRO Event Loop Library - Event Base Definitions
 *
 * Design inspired by ESP-IDF esp_event, reorganized for IPRO SDK.
 * This provides a lightweight, FreeRTOS-native event loop framework.
 */

#ifndef IPRO_EVENT_BASE_H
#define IPRO_EVENT_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Event base type - unique identifier for event sources
 *
 * Event bases are string constants that identify the source/category of events.
 * Each subsystem (WiFi, BLE, System, etc.) defines its own event base.
 */
typedef const char *ipro_event_base_t;

/**
 * @brief Declare an event base (use in header files)
 *
 * Example:
 *   IPRO_EVENT_DECLARE_BASE(WIFI_EVENT);
 */
#define IPRO_EVENT_DECLARE_BASE(id) extern ipro_event_base_t const id

/**
 * @brief Define an event base (use in source files)
 *
 * Example:
 *   IPRO_EVENT_DEFINE_BASE(WIFI_EVENT);
 */
#define IPRO_EVENT_DEFINE_BASE(id) ipro_event_base_t const id = #id

/**
 * @brief Special event base value to match any event base
 */
#define IPRO_EVENT_ANY_BASE     NULL

/**
 * @brief Special event ID to match any event within a base
 */
#define IPRO_EVENT_ANY_ID       (-1)

/**
 * @brief System event base - for core system events
 */
IPRO_EVENT_DECLARE_BASE(IPRO_SYSTEM_EVENT);

/**
 * @brief WiFi event base - for wireless events
 */
IPRO_EVENT_DECLARE_BASE(IPRO_WIFI_EVENT);

/**
 * @brief BLE event base - for Bluetooth Low Energy events
 */
IPRO_EVENT_DECLARE_BASE(IPRO_BLE_EVENT);

/**
 * @brief User event base - for application-defined events
 */
IPRO_EVENT_DECLARE_BASE(IPRO_USER_EVENT);

/*
 * Common system event IDs
 */
enum {
    IPRO_SYSTEM_EVENT_STARTED = 0,      /**< System startup complete */
    IPRO_SYSTEM_EVENT_IDLE,             /**< System idle */
    IPRO_SYSTEM_EVENT_LOW_MEMORY,       /**< Low memory warning */
    IPRO_SYSTEM_EVENT_TIMER,            /**< Timer event */
};

/*
 * Common WiFi event IDs (for compatibility with existing code)
 */
enum {
    IPRO_WIFI_EVENT_INIT_DONE = 1,      /**< WiFi initialization done */
    IPRO_WIFI_EVENT_MGMR_DONE,          /**< WiFi manager ready */
    IPRO_WIFI_EVENT_SCAN_DONE,          /**< Scan complete */
    IPRO_WIFI_EVENT_CONNECTED,          /**< Connected to AP */
    IPRO_WIFI_EVENT_DISCONNECTED,       /**< Disconnected from AP */
    IPRO_WIFI_EVENT_GOT_IP,             /**< Got IP address */
    IPRO_WIFI_EVENT_AP_STARTED,         /**< AP mode started */
    IPRO_WIFI_EVENT_AP_STOPPED,         /**< AP mode stopped */
    IPRO_WIFI_EVENT_AP_STA_CONNECTED,   /**< Station connected to our AP */
    IPRO_WIFI_EVENT_AP_STA_DISCONNECTED,/**< Station disconnected from our AP */
};

#ifdef __cplusplus
}
#endif

#endif /* IPRO_EVENT_BASE_H */
