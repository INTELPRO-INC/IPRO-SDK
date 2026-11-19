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

#ifndef BR_BLE_PROTOCOL_H
#define BR_BLE_PROTOCOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BR_BLE_PROTOCOL_VERSION 1u
#define BR_BLE_HEADER_SIZE 8u
#define BR_BLE_MAX_PAYLOAD 512u
#define BR_BLE_MAX_FRAME 514u

enum br_ble_opcode {
    BR_BLE_STATUS = 1,
    BR_BLE_WIFI_SCAN = 2,
    BR_BLE_WIFI_CONFIGURE = 3,
    BR_BLE_THREAD_DATASET_READ = 4,
    BR_BLE_THREAD_DATASET_WRITE = 5,
    BR_BLE_REBOOT = 6,
    BR_BLE_LAN_CREDENTIALS = 7, /* L4 BLE only: MAC[6], TLS pin[32], token[32]. */
    BR_BLE_FACTORY_RESET = 8,   /* Empty request, status-only reply, then reboot. */
};

enum br_ble_parse_result {
    BR_BLE_INVALID = -1,
    BR_BLE_MORE = 0,
    BR_BLE_COMPLETE = 1,
};

/* One instance per authenticated connection. Its owner serializes all access.
 * Wipe on disconnect, security loss, timeout, error and after command dispatch.
 * A completed payload is borrowed until clear; never log its contents. */
struct br_ble_message {
    uint8_t payload[BR_BLE_MAX_PAYLOAD];
    uint16_t id;
    uint16_t length;
    uint16_t used;
    uint8_t opcode;
    bool active;
    bool complete;
};

void br_ble_message_clear(struct br_ble_message *message);
enum br_ble_parse_result br_ble_message_accept(struct br_ble_message *message,
                                               const uint8_t *frame, size_t size);

/* Returns bytes written, zero for invalid parameters. offset is payload offset;
 * caller advances by return value - HEADER_SIZE. An empty payload is one frame. */
size_t br_ble_frame_encode(uint8_t opcode, uint16_t id,
                           const uint8_t *payload, size_t length, size_t offset,
                           uint16_t mtu, uint8_t *frame, size_t capacity);

/* LAN admits the shared management opcodes only. LAN_CREDENTIALS (7) stays on
 * the authenticated BLE link; the LAN server ends the session on it. */
bool br_lan_opcode_allowed(uint8_t opcode);

#endif
