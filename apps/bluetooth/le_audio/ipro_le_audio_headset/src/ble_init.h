/**
 * @file ble_init.h
 * @brief BLE Stack Initialization API (Zephyr blestack)
 *
 * Provides clean initialization interface for LE Audio application.
 * Replaces RivieraWaves Component Framework with standard Zephyr API.
 *
 * Copyright (C) IPRO SDK 2026
 */

#ifndef BLE_INIT_H
#define BLE_INIT_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize BLE controller and Zephyr host stack
 * 
 * This function:
 * 1. Initializes BLE controller (ble_controller_init)
 * 2. Initializes HCI driver (hci_driver_init)
 * 3. Enables Zephyr bluetooth stack (bt_enable)
 * 
 * @return 0 on success, negative error code otherwise
 */
int ble_stack_init(void);

/**
 * @brief Check if BLE stack is initialized
 * @return true if initialized, false otherwise
 */
bool ble_stack_is_ready(void);

/**
 * @brief Start BLE advertising
 * 
 * Starts connectable advertising with device name and appearance.
 * 
 * @return 0 on success, negative error code otherwise
 */
int ble_advertising_start(void);

/**
 * @brief Stop BLE advertising
 * @return 0 on success, negative error code otherwise
 */
int ble_advertising_stop(void);

/**
 * @brief Register connection callbacks
 * 
 * Called internally by ble_stack_init(). Applications should not call directly.
 */
void ble_conn_callbacks_register(void);

#endif /* BLE_INIT_H */
