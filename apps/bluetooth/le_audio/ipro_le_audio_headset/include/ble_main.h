/**
 * @file ble_main.h
 * @brief LE Audio Main API
 *
 * Copyright (C) IPRO SDK 2026
 */

#ifndef BLE_MAIN_H
#define BLE_MAIN_H

/**
 * @brief Initialize all LE Audio profiles
 *
 * Initializes CAP, BAP, and VCP services in the correct order.
 *
 * @return 0 on success, negative error code on failure
 */
int le_audio_init(void);

#endif /* BLE_MAIN_H */
