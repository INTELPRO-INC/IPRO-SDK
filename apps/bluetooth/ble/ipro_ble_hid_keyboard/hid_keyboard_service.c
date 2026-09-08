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
 * @file hid_keyboard_service.c
 * @brief BLE HID Keyboard Service Implementation (blestack host)
 *
 * Implements a standard HID over GATT Profile (HOGP) keyboard service
 * using the Zephyr-based blestack GATT APIs.
 */

#include "hid_keyboard_service.h"
#include "ipro_log.h"
#include "FreeRTOS.h"
#include "task.h"

#include <string.h>

#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#define LOG_TAG "HID_SVC"

/*
 * HID Information Value (4 bytes)
 * - bcdHID: HID version 1.11
 * - bCountryCode: Not localized
 * - Flags: Normally Connectable
 */
static const uint8_t hid_info_value[] = {
    0x11, 0x01,  /* bcdHID: HID 1.11 */
    0x00,        /* bCountryCode: Not localized */
    0x02         /* Flags: Normally Connectable */
};

/*
 * Protocol Mode: Report Protocol (0x01)
 */
static uint8_t protocol_mode = 0x01;

/*
 * HID Control Point
 */
static uint8_t ctrl_point;

/*
 * HID Report Map - Standard Keyboard
 */
static const uint8_t hid_report_map[] = {
    0x05, 0x01,        /* Usage Page (Generic Desktop) */
    0x09, 0x06,        /* Usage (Keyboard) */
    0xA1, 0x01,        /* Collection (Application) */
    0x85, 0x01,        /*   Report ID (1) */

    /* Modifier byte (8 bits for Ctrl, Shift, Alt, GUI) */
    0x05, 0x07,        /*   Usage Page (Keyboard/Keypad) */
    0x19, 0xE0,        /*   Usage Minimum (Left Control) */
    0x29, 0xE7,        /*   Usage Maximum (Right GUI) */
    0x15, 0x00,        /*   Logical Minimum (0) */
    0x25, 0x01,        /*   Logical Maximum (1) */
    0x75, 0x01,        /*   Report Size (1) */
    0x95, 0x08,        /*   Report Count (8) */
    0x81, 0x02,        /*   Input (Data, Variable, Absolute) */

    /* Reserved byte */
    0x95, 0x01,        /*   Report Count (1) */
    0x75, 0x08,        /*   Report Size (8) */
    0x81, 0x01,        /*   Input (Constant) */

    /* LED output report */
    0x95, 0x05,        /*   Report Count (5) */
    0x75, 0x01,        /*   Report Size (1) */
    0x05, 0x08,        /*   Usage Page (LEDs) */
    0x19, 0x01,        /*   Usage Minimum (Num Lock) */
    0x29, 0x05,        /*   Usage Maximum (Kana) */
    0x91, 0x02,        /*   Output (Data, Variable, Absolute) */

    /* LED padding (3 bits) */
    0x95, 0x01,        /*   Report Count (1) */
    0x75, 0x03,        /*   Report Size (3) */
    0x91, 0x01,        /*   Output (Constant) */

    /* Keycode array (6 keys) */
    0x95, 0x06,        /*   Report Count (6) */
    0x75, 0x08,        /*   Report Size (8) */
    0x15, 0x00,        /*   Logical Minimum (0) */
    0x25, 0x65,        /*   Logical Maximum (101) */
    0x05, 0x07,        /*   Usage Page (Keyboard/Keypad) */
    0x19, 0x00,        /*   Usage Minimum (0) */
    0x29, 0x65,        /*   Usage Maximum (101) */
    0x81, 0x00,        /*   Input (Data, Array) */

    0xC0               /* End Collection */
};

/*
 * Report Reference Descriptor - Input Report
 */
static const struct {
    uint8_t id;
    uint8_t type;
} __attribute__((packed)) input_report_ref = {
    .id = 0x01,
    .type = 0x01,  /* Input */
};

/*
 * HID Keyboard Report
 */
typedef struct {
    uint8_t modifier;
    uint8_t reserved;
    uint8_t keycode[6];
} __attribute__((packed)) hid_report_t;

static hid_report_t current_report;
static bool notify_enabled;

/*
 * GATT Callbacks
 */

static ssize_t read_hid_info(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                             void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             hid_info_value, sizeof(hid_info_value));
}

static ssize_t read_report_map(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             hid_report_map, sizeof(hid_report_map));
}

static ssize_t write_ctrl_point(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    uint8_t *value = attr->user_data;

    if (offset + len > sizeof(ctrl_point)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value + offset, buf, len);
    IPRO_LOGI(LOG_TAG, "HID Control Point: %s", ctrl_point == 0 ? "Suspend" : "Exit Suspend");

    return len;
}

static ssize_t read_protocol_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                  void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             &protocol_mode, sizeof(protocol_mode));
}

static ssize_t write_protocol_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                   const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (len != 1) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    const uint8_t *data = buf;
    protocol_mode = data[0];
    IPRO_LOGI(LOG_TAG, "Protocol Mode: %s", protocol_mode == 0 ? "Boot" : "Report");

    return len;
}

static ssize_t read_input_report(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                 void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, NULL, 0);
}

static void input_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "Input Report notifications %s", notify_enabled ? "enabled" : "disabled");
}

static ssize_t read_report_ref(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             attr->user_data, sizeof(input_report_ref));
}

/*
 * HID Service GATT Attribute Table
 *
 * Attribute index layout:
 * [0]  Primary Service (HIDS)
 * [1]  Characteristic Declaration (HID Info)
 * [2]  HID Information Value
 * [3]  Characteristic Declaration (Report Map)
 * [4]  Report Map Value
 * [5]  Characteristic Declaration (Control Point)
 * [6]  Control Point Value
 * [7]  Characteristic Declaration (Protocol Mode)
 * [8]  Protocol Mode Value
 * [9]  Characteristic Declaration (Input Report)
 * [10] Input Report Value        <-- used for bt_gatt_notify
 * [11] CCC Descriptor
 * [12] Report Reference Descriptor
 */
#define HID_INPUT_REPORT_ATTR_IDX 10

static struct bt_gatt_attr hid_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_HIDS),

    /* HID Information */
    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_INFO, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, read_hid_info, NULL, NULL),

    /* Report Map */
    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT_MAP, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, read_report_map, NULL, NULL),

    /* HID Control Point */
    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_CTRL_POINT,
                           BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, write_ctrl_point, &ctrl_point),

    /* Protocol Mode */
    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_PROTOCOL_MODE,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           read_protocol_mode, write_protocol_mode, NULL),

    /* Input Report */
    BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ_AUTHEN,
                           read_input_report, NULL, NULL),
    BT_GATT_CCC(input_ccc_changed,
                BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN),
    BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ,
                       read_report_ref, NULL, (void *)&input_report_ref),
};

static struct bt_gatt_service hid_svc = BT_GATT_SERVICE(hid_attrs);

/*
 * Convert char to keycode
 */
static uint8_t char_to_keycode(char c, uint8_t *modifier)
{
    *modifier = 0;
    if (c >= 'a' && c <= 'z') return 0x04 + (c - 'a');
    if (c >= 'A' && c <= 'Z') { *modifier = HID_KEY_MOD_LSHIFT; return 0x04 + (c - 'A'); }
    if (c >= '1' && c <= '9') return 0x1E + (c - '1');
    if (c == '0') return 0x27;
    if (c == ' ') return HID_KEY_SPACE;
    if (c == '\n') return HID_KEY_ENTER;
    return HID_KEY_NONE;
}

/*
 * Public API
 */

int hid_keyboard_service_init(void)
{
    int err = bt_gatt_service_register(&hid_svc);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Failed to register HID service: %d", err);
        return err;
    }

    IPRO_LOGI(LOG_TAG, "HID Keyboard service registered");
    return 0;
}

int hid_keyboard_send_key(struct bt_conn *conn, uint8_t keycode, uint8_t modifier)
{
    if (!notify_enabled) {
        return -1;
    }

    memset(&current_report, 0, sizeof(current_report));
    current_report.modifier = modifier;
    current_report.keycode[0] = keycode;

    return bt_gatt_notify(conn, &hid_attrs[HID_INPUT_REPORT_ATTR_IDX],
                          &current_report, sizeof(current_report));
}

int hid_keyboard_release_all(struct bt_conn *conn)
{
    if (!notify_enabled) {
        return -1;
    }

    memset(&current_report, 0, sizeof(current_report));

    return bt_gatt_notify(conn, &hid_attrs[HID_INPUT_REPORT_ATTR_IDX],
                          &current_report, sizeof(current_report));
}

int hid_keyboard_send_string(struct bt_conn *conn, const char *str, uint32_t delay_ms)
{
    if (!notify_enabled || !str) return -1;

    IPRO_LOGI(LOG_TAG, "Sending string: %s", str);

    while (*str) {
        uint8_t modifier = 0;
        uint8_t keycode = char_to_keycode(*str, &modifier);

        if (keycode != HID_KEY_NONE) {
            hid_keyboard_send_key(conn, keycode, modifier);
            vTaskDelay(pdMS_TO_TICKS(delay_ms / 2));

            hid_keyboard_release_all(conn);
            vTaskDelay(pdMS_TO_TICKS(delay_ms / 2));
        }
        str++;
    }
    return 0;
}

bool hid_keyboard_is_notify_enabled(void)
{
    return notify_enabled;
}
