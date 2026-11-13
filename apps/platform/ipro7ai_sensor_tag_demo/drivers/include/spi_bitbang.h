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
 * @file spi_bitbang.h
 * @brief Software SPI master (Mode 3) for sensors whose wiring does not
 *        land on the IPRO7 SPI1 peripheral's silicon-fixed GPIO4-7 pins.
 *
 * Designed for the ISM330BX on this board (GPIO31 SCLK, GPIO32 MISO,
 * GPIO33 MOSI, GPIO34 CS) — low data-rate use only (≤ ~1 MHz effective).
 */

#ifndef SPI_BITBANG_H
#define SPI_BITBANG_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise a bit-bang SPI bus.
 *   bus_id: caller-assigned id (use 0xB0+ so it cannot collide with hw ids).
 *   sclk / miso / mosi / cs: GPIO pin numbers (already muxed as GPIO).
 *   clk_hz_hint: target clock (very approximate — limited by CPU speed).
 *
 * Mode 3 (CPOL=1, CPHA=1), MSB first, 8-bit frames.
 */
int spi_bitbang_init(uint8_t bus_id,
                     uint8_t sclk, uint8_t miso,
                     uint8_t mosi, uint8_t cs,
                     uint32_t clk_hz_hint);

/** Bit-bang full-duplex transfer. CS auto-driven. */
/** Re-apply the pad function / direction / idle levels of an initialised bus.
 *  spi_bitbang_init() is a no-op once the bus is up, so an IMU re-init after a
 *  pad reset (PDS31 can clear GLB GPIO state, review 2026-10-19) calls this to
 *  actually repair the pins. */
int spi_bitbang_restore_pads(uint8_t bus_id);

int spi_bitbang_xfer(uint8_t bus_id,
                     const uint8_t *tx, uint8_t *rx, uint32_t len);

/** Read `len` bytes starting at `reg` (header byte = reg | 0x80). */
int spi_bitbang_read(uint8_t bus_id, uint8_t reg, uint8_t *buf, uint32_t len);

/** Write `len` bytes starting at `reg` (header byte = reg & 0x7F). */
int spi_bitbang_write(uint8_t bus_id, uint8_t reg, const uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* SPI_BITBANG_H */
