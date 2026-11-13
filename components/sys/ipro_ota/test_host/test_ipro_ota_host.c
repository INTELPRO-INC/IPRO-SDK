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

/* Host tests for ipro_ota: the package header, the sink, and a package checked in flash. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipro_ota.h"
#include "ipro_ota_uart.h"
#include "stubs.h"

static int failures;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while (0)

/* A package as tools/create_ota.py writes it: 512-byte header, then the payload. */
static uint8_t *make_package(const char *type, uint32_t payload_len, uint32_t *total)
{
    uint8_t *pkg = malloc(OTA_HEADER_SIZE + payload_len);

    memset(pkg, 0xFF, OTA_HEADER_SIZE);
    memcpy(pkg, "IPRO_OTA_Ver1.0", 16);
    memcpy(pkg + 16, type, 4);
    pkg[20] = (uint8_t)payload_len; pkg[21] = (uint8_t)(payload_len >> 8); pkg[22] = (uint8_t)(payload_len >> 16); pkg[23] = (uint8_t)(payload_len >> 24);
    memset(pkg + 32, 0, 32);
    strcpy((char *)pkg + 32, "IPRO7_HW_v1.1");
    strcpy((char *)pkg + 48, "202611131542");
    for (uint32_t i = 0; i < payload_len; i++) pkg[OTA_HEADER_SIZE + i] = (uint8_t)(i * 31u + (i >> 8));
    fake_sha256(pkg + OTA_HEADER_SIZE, payload_len, pkg + 64);
    *total = OTA_HEADER_SIZE + payload_len;
    return pkg;
}

static void test_parse(void)
{
    uint32_t total;
    uint8_t *pkg = make_package("XZ  ", 1000, &total);
    ota_pkg_info_t info;

    CHECK(ota_pkg_parse(pkg, &info) == OTA_PKG_OK);
    CHECK(info.type == OTA_PKG_TYPE_XZ && info.payload_len == 1000 && !info.has_signature);
    CHECK(strcmp(info.hw_version, "IPRO7_HW_v1.1") == 0 && strcmp(info.fw_version, "202611131542") == 0);
    CHECK(memcmp(info.sha256, pkg + 64, 32) == 0);

    memcpy(pkg + 16, "RAW ", 4);
    CHECK(ota_pkg_parse(pkg, &info) == OTA_PKG_OK && info.type == OTA_PKG_TYPE_RAW);
    memcpy(pkg + 16, "ZIP ", 4);
    CHECK(ota_pkg_parse(pkg, &info) == OTA_PKG_BAD_TYPE);
    memcpy(pkg + 16, "XZ  ", 4);
    memcpy(pkg + 96, "GSG1", 4);
    CHECK(ota_pkg_parse(pkg, &info) == OTA_PKG_OK && info.has_signature);
    memset(pkg + 20, 0, 4);
    CHECK(ota_pkg_parse(pkg, &info) == OTA_PKG_BAD_LENGTH);
    /* A firmware image sent as it is (it starts with its bootheader), not a package. */
    memcpy(pkg, "IPNP", 4);
    CHECK(ota_pkg_parse(pkg, &info) == OTA_PKG_BAD_MAGIC);
    free(pkg);
}

/* Feeds the package in uneven pieces, as a transport does. */
static int feed(const uint8_t *pkg, uint32_t total, uint32_t piece)
{
    for (uint32_t off = 0; off < total; off += piece) {
        uint32_t n = total - off < piece ? total - off : piece;
        if (ota_sink(pkg + off, n) != 0) return -1;
    }
    return ota_sink_flush();
}

static void test_sink(void)
{
    uint32_t total, addr, max;
    uint8_t *pkg = make_package("XZ  ", 3 * OTA_FLASH_BUF + 777, &total);

    /* The old way: the caller erased the slot. */
    fake_reset(0xFF);
    CHECK(ota_get_fw_b(&addr, &max) == 0 && addr == FAKE_FLASH_BASE && max == FAKE_FLASH_SIZE);
    CHECK(ota_sink_begin(addr, max) == 0);
    CHECK(ota_total_len() == 0);
    CHECK(feed(pkg, total, 238) == 0);
    CHECK(ota_recv_len() == total && ota_total_len() == total && !ota_overflowed());
    CHECK(ota_validate() == OTA_PKG_OK);
    CHECK(memcmp(fake_flash, pkg, total) == 0 && fake_erases == 0);
    ota_sink_end();

    /* A slot that holds something else, not erased by the caller: NOR writes only clear bits. */
    fake_reset(0x5A);
    CHECK(ota_sink_begin(addr, max) == 0);
    CHECK(feed(pkg, total, 238) == 0);
    CHECK(memcmp(fake_flash, pkg, total) != 0);
    ota_sink_end();

    /* The sink erases as it writes: the same slot comes out right, 4 KB at a time, and no further. */
    fake_reset(0x5A);
    CHECK(ota_sink_begin_ex(addr, max, OTA_SINK_ERASE_AS_WRITTEN) == 0);
    CHECK(feed(pkg, total, 513) == 0);
    CHECK(ota_validate() == OTA_PKG_OK);
    CHECK(memcmp(fake_flash, pkg, total) == 0);
    CHECK(fake_erases == 4 && fake_erased_bytes == 4 * OTA_FLASH_BUF);
    CHECK(fake_flash[4 * OTA_FLASH_BUF] == 0x5A);
    ota_sink_end();

    /* What ota_validate() refuses. */
    fake_reset(0xFF);
    CHECK(ota_sink_begin(addr, max) == 0 && feed(pkg, 300, 100) == 0 && ota_validate() == OTA_PKG_SHORT);
    ota_sink_end();
    CHECK(ota_sink_begin(addr, max) == 0 && feed(pkg, total - 5, 400) == 0 && ota_validate() == OTA_PKG_BAD_LENGTH);
    ota_sink_end();
    pkg[OTA_HEADER_SIZE + 9] ^= 1;
    fake_reset(0xFF);
    CHECK(ota_sink_begin(addr, max) == 0 && feed(pkg, total, 400) == 0 && ota_validate() == OTA_PKG_BAD_SHA);
    ota_sink_end();
    pkg[OTA_HEADER_SIZE + 9] ^= 1;
    memcpy(pkg + 16, "RAW ", 4);
    fake_reset(0xFF);
    CHECK(ota_sink_begin(addr, max) == 0 && feed(pkg, total, 400) == 0 && ota_validate() == OTA_PKG_BAD_TYPE);
    ota_sink_end();
    memcpy(pkg + 16, "XZ  ", 4);

    /* A package larger than the slot stops the sink. */
    fake_reset(0xFF);
    CHECK(ota_sink_begin(addr, 2 * OTA_FLASH_BUF) == 0);
    CHECK(feed(pkg, total, 400) != 0 && ota_overflowed());
    ota_sink_end();
    free(pkg);
}

static void test_validate_flash(void)
{
    uint32_t total;
    uint8_t *pkg = make_package("XZ  ", 5 * OTA_FLASH_BUF + 123, &total);

    fake_reset(0xFF);
    memcpy(fake_flash, pkg, total);
    CHECK(ota_validate_flash(FAKE_FLASH_BASE, total) == OTA_PKG_OK);
    CHECK(ota_validate_flash(FAKE_FLASH_BASE, total - 1) == OTA_PKG_BAD_LENGTH);
    CHECK(ota_validate_flash(FAKE_FLASH_BASE, 100) == OTA_PKG_SHORT);
    fake_flash[total - 3] ^= 0x10;                  /* one bit of the last sector */
    CHECK(ota_validate_flash(FAKE_FLASH_BASE, total) == OTA_PKG_BAD_SHA);
    fake_flash[total - 3] ^= 0x10;
    memcpy(fake_flash + 16, "RAW ", 4);
    CHECK(ota_validate_flash(FAKE_FLASH_BASE, total) == OTA_PKG_BAD_TYPE);
    memcpy(fake_flash, "IPNP", 4);                  /* a firmware image, not a package */
    CHECK(ota_validate_flash(FAKE_FLASH_BASE, total) == OTA_PKG_BAD_MAGIC);
    memcpy(fake_flash, pkg, total);
    fake_flash_fail = 1;
    CHECK(ota_validate_flash(FAKE_FLASH_BASE, total) == OTA_PKG_IO);
    free(pkg);
}

static void test_slot(void)
{
    uint32_t addr, max;

    fake_reset(0xFF);
    CHECK(ota_mark_fwb_active() == 0 && fake_pt_updates == 1 && fake_fw.activeIndex == 1);
    /* A table with one FW slot has nowhere to stage a package. */
    fake_reset(0xFF);
    fake_fw.Address[1] = 0;
    fake_fw.maxLen[1] = 0;
    CHECK(ota_get_fw_b(&addr, &max) != 0);
}

/* A frame as the host sends it. */
static uint32_t make_frame(uint8_t *out, uint16_t seq, const uint8_t *payload, uint16_t len)
{
    uint16_t crc;

    out[0] = OTA_UART_SOF0; out[1] = OTA_UART_SOF1;
    out[2] = (uint8_t)seq; out[3] = (uint8_t)(seq >> 8);
    out[4] = (uint8_t)len; out[5] = (uint8_t)(len >> 8);
    memcpy(out + 6, payload, len);
    crc = ota_uart_crc16(0xFFFFu, out + 2, 4u + len);
    out[6 + len] = (uint8_t)crc; out[7 + len] = (uint8_t)(crc >> 8);
    return 8u + len;
}

static int push_all(ota_uart_rx_t *rx, const uint8_t *bytes, uint32_t n)
{
    int last = OTA_UART_RX_MORE;

    for (uint32_t i = 0; i < n; i++) {
        int got = ota_uart_rx_push(rx, bytes[i]);
        if (got != OTA_UART_RX_MORE) last = got;
    }
    return last;
}

static void test_uart_frames(void)
{
    static ota_uart_rx_t rx;
    static uint8_t frame[OTA_UART_MAX_PAYLOAD + 16], payload[OTA_UART_MAX_PAYLOAD];
    uint32_t n;

    /* CRC-16/CCITT-FALSE: the check value of "123456789". */
    CHECK(ota_uart_crc16(0xFFFFu, (const uint8_t *)"123456789", 9) == 0x29B1);

    for (uint32_t i = 0; i < sizeof(payload); i++) payload[i] = (uint8_t)(i ^ 0xA5);
    ota_uart_rx_reset(&rx);
    n = make_frame(frame, 7, payload, 300);
    CHECK(push_all(&rx, frame, n) == OTA_UART_RX_FRAME && rx.seq == 7 && rx.len == 300 && memcmp(rx.payload, payload, 300) == 0);

    /* The largest payload, and console text in front of the frame (with an A5 in it). */
    CHECK(push_all(&rx, (const uint8_t *)"I (12) log \xA5 line\r\n", 20) == OTA_UART_RX_MORE);
    n = make_frame(frame, 0x1234, payload, OTA_UART_MAX_PAYLOAD);
    CHECK(push_all(&rx, frame, n) == OTA_UART_RX_FRAME && rx.seq == 0x1234 && rx.len == OTA_UART_MAX_PAYLOAD);

    /* A5 right before the start. */
    frame[0] = OTA_UART_SOF0;
    n = make_frame(frame + 1, 3, payload, 16);
    CHECK(push_all(&rx, frame, n + 1) == OTA_UART_RX_FRAME && rx.seq == 3);

    /* One wrong bit is a bad frame, and the next good one is taken. */
    n = make_frame(frame, 4, payload, 64);
    frame[20] ^= 0x04;
    CHECK(push_all(&rx, frame, n) == OTA_UART_RX_BAD);
    n = make_frame(frame, 4, payload, 64);
    CHECK(push_all(&rx, frame, n) == OTA_UART_RX_FRAME && rx.seq == 4);

    /* A length the board has no room for, or none. */
    n = make_frame(frame, 5, payload, 8);
    frame[4] = 0x01; frame[5] = 0x04;               /* 1025 */
    CHECK(push_all(&rx, frame, 6) == OTA_UART_RX_BAD);
    frame[4] = 0; frame[5] = 0;
    CHECK(push_all(&rx, frame, 6) == OTA_UART_RX_BAD);
}

int main(void)
{
    test_uart_frames();
    test_parse();
    test_sink();
    test_validate_flash();
    test_slot();
    if (failures) {
        printf("ipro_ota host tests: %d FAILED\n", failures);
        return 1;
    }
    printf("ipro_ota host tests: all passed\n");
    return 0;
}
