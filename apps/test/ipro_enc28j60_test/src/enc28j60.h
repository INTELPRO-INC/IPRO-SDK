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
 * @file enc28j60.h
 * @brief Minimal ENC28J60 SPI Ethernet driver (bring-up / raw-frame test).
 *
 * Scope: prove the chip is alive over SPI, configure MAC + PHY, and send /
 * receive raw Ethernet frames. lwIP integration is intentionally NOT part of
 * this layer — it can be added on top once the bench bring-up is verified.
 */
#ifndef ENC28J60_H
#define ENC28J60_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define ENC28J60_OK              0
#define ENC28J60_ERR_SPI        -1
#define ENC28J60_ERR_NO_CLKRDY  -2
#define ENC28J60_ERR_NO_CHIP    -3
#define ENC28J60_ERR_PARAM      -4
#define ENC28J60_ERR_TX_ABORT   -5
#define ENC28J60_ERR_TX_TIMEOUT -6

/**
 * @brief Initialise SPI/GPIO, hard-reset and configure the ENC28J60.
 *
 * @param mac        6-byte station MAC address (must be non-NULL).
 * @param full_duplex true for full-duplex, false for half-duplex (typical
 *                    for a direct link to a PC NIC that auto-negotiates).
 * @return ENC28J60_OK or a negative ENC28J60_ERR_* code.
 */
int enc28j60_init(const uint8_t mac[6], bool full_duplex);

/** @brief Read the silicon revision register (EREVID). 0/0xFF usually means
 *         the SPI link is dead (check wiring / MOSI-MISO swap). */
uint8_t enc28j60_read_revid(void);

/** @brief Read PHY identification (PHID1<<16 | PHID2). Expect 0x0083_14xx. */
uint32_t enc28j60_read_phid(void);

/** @brief true if the PHY reports link up (PHSTAT2.LSTAT). */
bool enc28j60_link_up(void);

/** @brief Enable/disable promiscuous receive (accept every frame). */
void enc28j60_set_promiscuous(bool on);

/**
 * @brief Transmit one raw Ethernet frame (dst+src+type+payload, no CRC —
 *        the MAC appends FCS). Blocks until the MAC reports done or aborts.
 * @return ENC28J60_OK or a negative ENC28J60_ERR_* code.
 */
int enc28j60_send(const uint8_t *frame, uint16_t len);

/**
 * @brief Fetch one pending received frame, if any.
 * @param buf   destination buffer.
 * @param maxlen capacity of @p buf.
 * @return >0 number of bytes copied (CRC stripped), 0 if no packet waiting,
 *         negative ENC28J60_ERR_* on error.
 */
int enc28j60_recv(uint8_t *buf, uint16_t maxlen);

/** @brief Number of frames waiting in the receive FIFO (EPKTCNT). */
uint8_t enc28j60_pktcnt(void);

/* ---- Diagnostics (shell `enc swap` / `enc loop`) ---- */

/** @brief Override the MOSI/MISO pin swap applied by the next init/loopback.
 *         Default comes from ENC_SPI_PIN_SWAP in enc28j60_port.h. */
void enc28j60_set_pin_swap(bool on);

/** @brief Configure just the SPI bus + control GPIOs (no chip reset). */
void enc28j60_bus_init(void);

/** @brief Drive any pin as a plain GPIO output (for multimeter wire checks). */
void enc28j60_pin_out(uint8_t pin, int level);

/** @brief Read any pin as a plain GPIO input. */
int  enc28j60_pin_in(uint8_t pin);

/** @brief MOSI->MISO loopback: clocks @p n bytes of @p tx out and stores what
 *         is read back in @p rx. Bridge GPIO16<->GPIO17 externally: a working
 *         SoC SPI echoes tx into rx. */
void enc28j60_loopback(const uint8_t *tx, uint8_t *rx, uint16_t n);

/* ---- Low-level register access (exposed for the `enc reg/phy` shell cmds) ---- */
uint8_t  enc28j60_read_reg(uint8_t reg);
void     enc28j60_write_reg(uint8_t reg, uint8_t val);
uint16_t enc28j60_phy_read(uint8_t addr);
void     enc28j60_phy_write(uint8_t addr, uint16_t val);

#endif /* ENC28J60_H */
