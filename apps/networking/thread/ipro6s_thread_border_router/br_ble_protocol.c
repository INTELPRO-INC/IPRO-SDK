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

#include "br_ble_protocol.h"
#include <string.h>

static uint16_t read16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static void write16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

void br_ble_message_clear(struct br_ble_message *message)
{
    if (message) {
        volatile uint8_t *p = (volatile uint8_t *)message;
        for (size_t i = 0; i < sizeof(*message); ++i) p[i] = 0;
    }
}

enum br_ble_parse_result br_ble_message_accept(struct br_ble_message *message,
                                               const uint8_t *frame, size_t size)
{
    if (!message) return BR_BLE_INVALID;
    if (!frame || size < BR_BLE_HEADER_SIZE || size > BR_BLE_MAX_FRAME || message->complete)
        goto invalid;
    const uint8_t opcode = frame[1];
    const uint16_t id = read16(frame + 2);
    const uint16_t length = read16(frame + 4);
    const uint16_t offset = read16(frame + 6);
    const size_t count = size - BR_BLE_HEADER_SIZE;
    if (frame[0] != BR_BLE_PROTOCOL_VERSION || !opcode || !id ||
        length > BR_BLE_MAX_PAYLOAD || offset > length || count > (size_t)(length - offset) ||
        (count == 0 && length != 0)) goto invalid;
    if (!message->active) {
        if (offset != 0) goto invalid;
        message->opcode = opcode;
        message->id = id;
        message->length = length;
        message->active = true;
    }
    if (opcode != message->opcode || id != message->id || length != message->length ||
        offset != message->used) goto invalid;
    memcpy(message->payload + message->used, frame + BR_BLE_HEADER_SIZE, count);
    message->used += (uint16_t)count;
    message->complete = message->used == message->length;
    return message->complete ? BR_BLE_COMPLETE : BR_BLE_MORE;
invalid:
    br_ble_message_clear(message);
    return BR_BLE_INVALID;
}

size_t br_ble_frame_encode(uint8_t opcode, uint16_t id,
                           const uint8_t *payload, size_t length, size_t offset,
                           uint16_t mtu, uint8_t *frame, size_t capacity)
{
    if (!opcode || !id || !frame || (!payload && length) || length > BR_BLE_MAX_PAYLOAD ||
        offset > length || (offset == length && length != 0) || mtu < 23 || mtu > 517 ||
        capacity < BR_BLE_HEADER_SIZE) return 0;
    size_t count = length - offset;
    const size_t available = (size_t)mtu - 3 - BR_BLE_HEADER_SIZE;
    if (count > available) count = available;
    if (capacity < BR_BLE_HEADER_SIZE + count) return 0;
    frame[0] = BR_BLE_PROTOCOL_VERSION;
    frame[1] = opcode;
    write16(frame + 2, id);
    write16(frame + 4, (uint16_t)length);
    write16(frame + 6, (uint16_t)offset);
    if (count) memcpy(frame + BR_BLE_HEADER_SIZE, payload + offset, count);
    return BR_BLE_HEADER_SIZE + count;
}

bool br_lan_opcode_allowed(uint8_t opcode)
{
    switch (opcode) {
    case BR_BLE_STATUS:
    case BR_BLE_WIFI_SCAN:
    case BR_BLE_WIFI_CONFIGURE:
    case BR_BLE_THREAD_DATASET_READ:
    case BR_BLE_THREAD_DATASET_WRITE:
    case BR_BLE_REBOOT:
    case BR_BLE_FACTORY_RESET:
        return true;
    default:
        return false;
    }
}
