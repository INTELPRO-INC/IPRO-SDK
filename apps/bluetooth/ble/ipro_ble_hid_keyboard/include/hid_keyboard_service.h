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
 * @file hid_keyboard_service.h
 * @brief BLE HID Keyboard Service Header (blestack host)
 */

#ifndef HID_KEYBOARD_SERVICE_H
#define HID_KEYBOARD_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/conn.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Key codes
 */
#define HID_KEY_NONE                    0x00
#define HID_KEY_A                       0x04
#define HID_KEY_SPACE                   0x2C
#define HID_KEY_ENTER                   0x28

/*
 * Modifier keys
 */
#define HID_KEY_MOD_LSHIFT              0x02

/**
 * @brief Initialize HID Keyboard service (registers GATT service)
 */
int hid_keyboard_service_init(void);

/**
 * @brief Send key press
 */
int hid_keyboard_send_key(struct bt_conn *conn, uint8_t keycode, uint8_t modifier);

/**
 * @brief Release all keys
 */
int hid_keyboard_release_all(struct bt_conn *conn);

/**
 * @brief Send string as key presses
 */
int hid_keyboard_send_string(struct bt_conn *conn, const char *str, uint32_t delay_ms);

/**
 * @brief Check if notifications are enabled
 */
bool hid_keyboard_is_notify_enabled(void);

#ifdef __cplusplus
}
#endif

#endif // HID_KEYBOARD_SERVICE_H
