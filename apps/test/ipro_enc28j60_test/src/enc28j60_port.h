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
 * @file enc28j60_port.h
 * @brief Board/SoC wiring for the ENC28J60 on the IPRO7 Arduino board.
 *
 * GPIO16/17/19 are a HARDWARE SPI0 group — the signal each pin carries is
 * fixed by its position (verified in bsp/.../drv_gpio.h):
 *     GPIO16 = SPI0 MOSI  ( -> ENC28J60 SI )
 *     GPIO17 = SPI0 MISO  ( <- ENC28J60 SO )
 *     GPIO19 = SPI0 SCLK  ( -> ENC28J60 SCK )
 *
 * CS / RST / INT are plain GPIOs (software-driven, freely assignable):
 *     GPIO18 = CS   ( -> ENC28J60 CS,  active low )  -- software chip-select
 *     GPIO14 = RST  ( -> ENC28J60 RST, active low )
 *     GPIO15 = INT  ( <- ENC28J60 INT, active low )  -- polled, not an IRQ
 *
 * Power: ENC28J60 module VCC -> 3.3~5V (board has an onboard 3.3V LDO),
 *        GND -> GND. The IPRO7 GPIOs are 3.3V; signal pins are 3.3V safe.
 */
#ifndef ENC28J60_PORT_H
#define ENC28J60_PORT_H

/* ---- SPI peripheral ---- */
#define ENC_SPI_ID         0           /* SPI0 */
#define ENC_SPI_CLK_HZ     8000000u    /* 8 MHz (ENC28J60 max is 20 MHz)   */

/* ENC28J60 is SPI mode 0,0 (CPOL=0, CPHA=0). */
#define ENC_SPI_CPOL       0           /* SPI_POLARITY_LOW  */
#define ENC_SPI_CPHA       0           /* SPI_PHASE_1EDGE   */

/*
 * MOSI/MISO swap. Default (0): GPIO16=MOSI, GPIO17=MISO per the SoC pinmux.
 * If `enc id` returns 0x00/0xFF with the wiring double-checked, the two data
 * lines are crossed — flip this to 1 (or physically swap GPIO16/17).
 */
#define ENC_SPI_PIN_SWAP   0

/* ---- Hardware SPI0 pins (muxed to SPI function) ---- */
#define ENC_PIN_MOSI       16
#define ENC_PIN_MISO       17
#define ENC_PIN_SCLK       19

/* ---- Software-driven control pins (plain GPIO) ---- */
#define ENC_PIN_CS         18          /* output, active low */
#define ENC_PIN_RST        14          /* output, active low */
#define ENC_PIN_INT        15          /* input,  active low (polled) */

#endif /* ENC28J60_PORT_H */
