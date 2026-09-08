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
 * hal_pio_sdh_crc.h - SD command/data checksums and command framing.
 *
 * Split out from hal_pio_sdh.c so the pure logic is host-unit-testable
 * without pulling in PIO/register headers. Ported from the carlk3/ZuluSCSI
 * rp2040_sdio reference driver.
 */
#ifndef HAL_PIO_SDH_CRC_H
#define HAL_PIO_SDH_CRC_H

#include <stdint.h>

/* Accumulate one byte into a running CRC7 (SD command checksum).
 * Seed with 0; the result holds CRC7 in bits [7:1], bit[0] = 0. */
uint8_t crc7_byte(uint8_t crc, uint8_t b);

/* CRC16 for the 4 DATA lines in parallel (4-bit bus mode). Each 32-bit word
 * carries 8 bits per line; the 64-bit result packs 4 x 16-bit lane CRCs. */
uint64_t sdio_crc16_4bit(const uint32_t *data, uint32_t num_words);

/* Build the two 32-bit words the sdio_cmd_clk PIO program expects for a
 * command: bit-count fields + 48-bit packet (MSB first) + CRC7 + end bit.
 * resp_bits = expected response length (48 or 136), or 0 for no response. */
void sdio_make_cmd_words(uint8_t cmd, uint32_t arg, uint8_t resp_bits,
                         uint32_t *w0, uint32_t *w1);

/* Verify the CRC7 of a 48-bit R1/R6/R7 response (resp0 = first 32 bits,
 * resp1 = last 16 bits in [31:16]... as read from the RX FIFO). Returns 0
 * if the CRC matches, negative otherwise. */
int sdio_check_r1_crc(uint32_t resp0, uint32_t resp1);

#endif /* HAL_PIO_SDH_CRC_H */
