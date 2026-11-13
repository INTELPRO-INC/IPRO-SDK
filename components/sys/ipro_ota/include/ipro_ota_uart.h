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

/*
 * OTA over the console UART: the `ota_uart` shell command and its frames.
 *
 *   host:  ota_uart <package bytes> [noapply]\r
 *   board: ==OTA_UART_READY:<largest payload>==      then it takes the UART's input
 *   host:  A5 5A  seq(u16)  len(u16)  payload[len]  crc16(u16)        one frame at a time
 *   board: A5 06  seq(u16)   frame taken (also for a frame it already has)
 *          A5 15  seq(u16)   send frame <seq> (again): bad CRC, or nothing arrived for a while
 *   board: ==OTA_UART_OK==   package checked, partition switched, rebooting
 *          ==OTA_UART_FAIL:<reason>==
 *
 * Integers are little-endian. The CRC is CRC-16/CCITT-FALSE (poly 0x1021,
 * init 0xFFFF) over seq, len and payload: the crash dump's. Frames are numbered
 * from 0 and carry the package in order; the host sends the next one only after
 * the ACK, so the board may spend that time on the flash. Other tasks may print
 * between frames: the host looks for A5 06 / A5 15 and ignores the rest.
 * The package goes through the OTA core's sink with OTA_SINK_ERASE_AS_WRITTEN.
 */
#ifndef IPRO_OTA_UART_H
#define IPRO_OTA_UART_H

#include <stdint.h>

#define OTA_UART_SOF0           0xA5
#define OTA_UART_SOF1           0x5A
#define OTA_UART_ACK            0x06
#define OTA_UART_NAK            0x15
#define OTA_UART_MAX_PAYLOAD    1024

/* ---- frame decoder (pure: host tests build it) ---- */

typedef struct {
    uint8_t  state;
    uint16_t seq;
    uint16_t len;
    uint16_t got;
    uint16_t crc;
    uint8_t  payload[OTA_UART_MAX_PAYLOAD];
} ota_uart_rx_t;

enum {
    OTA_UART_RX_MORE    = 0,    /* no frame yet */
    OTA_UART_RX_FRAME   = 1,    /* rx->seq, rx->len, rx->payload hold a frame */
    OTA_UART_RX_BAD     = -1,   /* a frame ended with a wrong CRC, or named too long a payload */
};

void     ota_uart_rx_reset(ota_uart_rx_t *rx);
int      ota_uart_rx_push(ota_uart_rx_t *rx, uint8_t byte);
uint16_t ota_uart_crc16(uint16_t crc, const uint8_t *data, uint32_t len);

/* ---- the transfer ---- */

/* Receives a package of @p total bytes on the console UART and, unless
 * @p apply is 0, switches the partition table and reboots. Returns 0 or a
 * negative reason, which the command prints. Called by the `ota_uart` command. */
int ota_uart_receive(uint32_t total, int apply);

#endif /* IPRO_OTA_UART_H */
