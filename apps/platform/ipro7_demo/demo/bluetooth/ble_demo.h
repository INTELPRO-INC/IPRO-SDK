/**
 * @file ble_demo.h
 * @brief BLE Demo API Header
 * 
 * Provides BLE peripheral and central functionality using Direct API.
 * 
 * Features:
 * - Peripheral: Advertise and accept connections (auto-restart after disconnect)
 * - Central: Scan and connect to devices (auto-connect to IPRO7_DEMO)
 * - Both roles can work simultaneously
 * 
 * Copyright (C) IPRO SDK 2026
 */

#ifndef BLE_DEMO_H
#define BLE_DEMO_H

#include <stdint.h>

#ifdef CONFIG_BLUETOOTH_LE_ENABLE

// Forward declaration from RivieraWaves stack
struct ke_msg_handler;

/**
 * @brief Initialize BLE demo
 * 
 * Initializes the BLE controller.
 * Must be called before using any BLE functionality.
 * 
 * @return 0 on success, negative error code on failure
 */
int ble_demo_init(void);

/**
 * @brief Start as BLE peripheral (Direct API)
 * 
 * Device will start advertising as "IPRO7_DEMO" and wait for connection.
 * Auto-restarts advertising after disconnection.
 * Supports pairing with Just Works authentication.
 * 
 * CLI Usage:
 * @code
 * ble_peripheral
 * @endcode
 * 
 * @return 0 on success, negative error code on failure
 */
int ble_demo_start_peripheral(void);

/**
 * @brief Process FSM state machine
 * 
 * Call this function in a dedicated task loop.
 * Uses task notification for immediate event response.
 * 
 * Typical usage in task:
 * @code
 * void fsm_task(void* arg) {
 *     app_set_fsm_task_handle(xTaskGetCurrentTaskHandle());
 *     while(1) {
 *         app_process_fsm();  // Blocks waiting for events or 100ms timeout
 *     }
 * }
 * @endcode
 */
void app_process_fsm(void);

/**
 * @brief Set FSM task handle for event notification
 * 
 * Call this from the FSM task to enable immediate event notifications.
 * 
 * @param task_handle Handle of the FSM task
 */
void app_set_fsm_task_handle(TaskHandle_t task_handle);

/**
 * @brief Start BLE central scan (Direct API)
 * 
 * Scans for BLE devices for 10 seconds.
 * If auto-connect is enabled, automatically connects to "IPRO7_DEMO" device.
 * 
 * CLI Usage:
 * @code
 * ble_central
 * @endcode
 */
void ble_central_scan(void);

/**
 * @brief List all scanned devices
 * 
 * Prints scan results to console with index, address, RSSI, and name.
 * 
 * CLI Usage:
 * @code
 * ble_list
 * @endcode
 */
void ble_central_list_devices(void);

/**
 * @brief Connect to a scanned device by index
 * 
 * @param device_index Index from scan results (1-based, use ble_list to see indices)
 * 
 * CLI Usage:
 * @code
 * ble_connect 1
 * @endcode
 * 
 * @return 0 on success, negative error code on failure
 */
int ble_central_connect(uint8_t device_index);

/**
 * @brief Disconnect from connected device
 * 
 * Works for both peripheral and central roles.
 * 
 * CLI Usage:
 * @code
 * ble_disconnect
 * @endcode
 * 
 * @return 0 on success, negative error code on failure
 */
int ble_central_disconnect(void);

#endif /* CONFIG_BLUETOOTH_LE_ENABLE */

#endif /* BLE_DEMO_H */
