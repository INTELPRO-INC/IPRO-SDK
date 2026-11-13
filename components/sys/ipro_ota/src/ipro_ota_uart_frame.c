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

/*
 * Transport-agnostic OTA staging core (see ota_common.h).
 */

/* The frames of OTA over UART, without any HAL (see ipro_ota_uart.h). */
#include "../include/ipro_ota_uart.h"

enum { S_SOF0, S_SOF1, S_SEQ0, S_SEQ1, S_LEN0, S_LEN1, S_PAYLOAD, S_CRC0, S_CRC1 };

uint16_t ota_uart_crc16(uint16_t crc, const uint8_t *data, uint32_t len)
{
    while (len--) {
        crc ^= (uint16_t)(*data++) << 8;
        for (int bit = 0; bit < 8; bit++) {
            crc = (crc & 0x8000u) ? (uint16_t)((crc << 1) ^ 0x1021u) : (uint16_t)(crc << 1);
        }
    }
    return crc;
}

void ota_uart_rx_reset(ota_uart_rx_t *rx)
{
    rx->state = S_SOF0;
    rx->got = 0;
}

int ota_uart_rx_push(ota_uart_rx_t *rx, uint8_t byte)
{
    switch (rx->state) {
    case S_SOF0:
        if (byte == OTA_UART_SOF0) rx->state = S_SOF1;
        break;
    case S_SOF1:
        /* A5 A5 5A: the second A5 may be the real start. */
        rx->state = (byte == OTA_UART_SOF1) ? S_SEQ0 : (byte == OTA_UART_SOF0) ? S_SOF1 : S_SOF0;
        break;
    case S_SEQ0: rx->seq = byte; rx->state = S_SEQ1; break;
    case S_SEQ1: rx->seq |= (uint16_t)byte << 8; rx->state = S_LEN0; break;
    case S_LEN0: rx->len = byte; rx->state = S_LEN1; break;
    case S_LEN1:
        rx->len |= (uint16_t)byte << 8;
        rx->got = 0;
        if (rx->len == 0 || rx->len > OTA_UART_MAX_PAYLOAD) {
            rx->state = S_SOF0;
            return OTA_UART_RX_BAD;
        }
        rx->state = S_PAYLOAD;
        break;
    case S_PAYLOAD:
        rx->payload[rx->got++] = byte;
        if (rx->got == rx->len) rx->state = S_CRC0;
        break;
    case S_CRC0: rx->crc = byte; rx->state = S_CRC1; break;
    case S_CRC1: {
        uint8_t head[4] = { (uint8_t)rx->seq, (uint8_t)(rx->seq >> 8), (uint8_t)rx->len, (uint8_t)(rx->len >> 8) };
        uint16_t crc = ota_uart_crc16(0xFFFFu, head, sizeof(head));

        rx->crc |= (uint16_t)byte << 8;
        rx->state = S_SOF0;
        crc = ota_uart_crc16(crc, rx->payload, rx->len);
        return crc == rx->crc ? OTA_UART_RX_FRAME : OTA_UART_RX_BAD;
    }
    default:
        rx->state = S_SOF0;
        break;
    }
    return OTA_UART_RX_MORE;
}
