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
 * @file enc28j60_regs.h
 * @brief ENC28J60 register map, SPI opcodes and bit definitions.
 *
 * Control-register addresses are packed so a single value carries
 * everything enc_read_reg()/enc_write_reg() need:
 *
 *     bits [4:0] register address within the bank
 *     bits [6:5] bank number (0-3)
 *     bit  [7]   "MAC/MII" flag — these registers return a dummy byte
 *                first on a Read-Control-Register (RCR) transaction.
 */
#ifndef ENC28J60_REGS_H
#define ENC28J60_REGS_H

#define ENC_ADDR_MASK   0x1F
#define ENC_BANK_MASK   0x60
#define ENC_BANK_SHIFT  5
#define ENC_SPRD_FLAG   0x80   /* MAC/MII: dummy byte before data on RCR */

#define ENC_REG(bank, addr)      (((bank) << ENC_BANK_SHIFT) | ((addr) & ENC_ADDR_MASK))
#define ENC_MREG(bank, addr)     (ENC_REG(bank, addr) | ENC_SPRD_FLAG)

#define ENC_REG_ADDR(r)          ((r) & ENC_ADDR_MASK)
#define ENC_REG_BANK(r)          (((r) & ENC_BANK_MASK) >> ENC_BANK_SHIFT)
#define ENC_REG_IS_MAC_MII(r)    (((r) & ENC_SPRD_FLAG) != 0)

/* ---- SPI opcodes (3-bit op | 5-bit arg) ---- */
#define ENC_OP_RCR      0x00   /* Read  Control Register  | addr      */
#define ENC_OP_RBM      0x3A   /* Read  Buffer Memory     (fixed arg) */
#define ENC_OP_WCR      0x40   /* Write Control Register  | addr      */
#define ENC_OP_WBM      0x7A   /* Write Buffer Memory     (fixed arg) */
#define ENC_OP_BFS      0x80   /* Bit Field Set           | addr      */
#define ENC_OP_BFC      0xA0   /* Bit Field Clear         | addr      */
#define ENC_OP_SRC      0xFF   /* System Reset (soft)     (fixed)     */

/* ---- Common registers (same address in every bank) ---- */
#define ENC_EIE         ENC_REG(0, 0x1B)
#define ENC_EIR         ENC_REG(0, 0x1C)
#define ENC_ESTAT       ENC_REG(0, 0x1D)
#define ENC_ECON2       ENC_REG(0, 0x1E)
#define ENC_ECON1       ENC_REG(0, 0x1F)

/* ---- Bank 0 ---- */
#define ENC_ERDPTL      ENC_REG(0, 0x00)
#define ENC_ERDPTH      ENC_REG(0, 0x01)
#define ENC_EWRPTL      ENC_REG(0, 0x02)
#define ENC_EWRPTH      ENC_REG(0, 0x03)
#define ENC_ETXSTL      ENC_REG(0, 0x04)
#define ENC_ETXSTH      ENC_REG(0, 0x05)
#define ENC_ETXNDL      ENC_REG(0, 0x06)
#define ENC_ETXNDH      ENC_REG(0, 0x07)
#define ENC_ERXSTL      ENC_REG(0, 0x08)
#define ENC_ERXSTH      ENC_REG(0, 0x09)
#define ENC_ERXNDL      ENC_REG(0, 0x0A)
#define ENC_ERXNDH      ENC_REG(0, 0x0B)
#define ENC_ERXRDPTL    ENC_REG(0, 0x0C)
#define ENC_ERXRDPTH    ENC_REG(0, 0x0D)
#define ENC_ERXWRPTL    ENC_REG(0, 0x0E)
#define ENC_ERXWRPTH    ENC_REG(0, 0x0F)

/* ---- Bank 1 ---- */
#define ENC_ERXFCON     ENC_REG(1, 0x18)
#define ENC_EPKTCNT     ENC_REG(1, 0x19)

/* ---- Bank 2 (MAC/MII — need dummy byte on read) ---- */
#define ENC_MACON1      ENC_MREG(2, 0x00)
#define ENC_MACON3      ENC_MREG(2, 0x02)
#define ENC_MACON4      ENC_MREG(2, 0x03)
#define ENC_MABBIPG     ENC_MREG(2, 0x04)
#define ENC_MAIPGL      ENC_MREG(2, 0x06)
#define ENC_MAIPGH      ENC_MREG(2, 0x07)
#define ENC_MACLCON1    ENC_MREG(2, 0x08)
#define ENC_MACLCON2    ENC_MREG(2, 0x09)
#define ENC_MAMXFLL     ENC_MREG(2, 0x0A)
#define ENC_MAMXFLH     ENC_MREG(2, 0x0B)
#define ENC_MICMD       ENC_MREG(2, 0x12)
#define ENC_MIREGADR    ENC_MREG(2, 0x14)
#define ENC_MIWRL       ENC_MREG(2, 0x16)
#define ENC_MIWRH       ENC_MREG(2, 0x17)
#define ENC_MIRDL       ENC_MREG(2, 0x18)
#define ENC_MIRDH       ENC_MREG(2, 0x19)

/* ---- Bank 3 ---- */
#define ENC_MAADR5      ENC_MREG(3, 0x00)
#define ENC_MAADR6      ENC_MREG(3, 0x01)
#define ENC_MAADR3      ENC_MREG(3, 0x02)
#define ENC_MAADR4      ENC_MREG(3, 0x03)
#define ENC_MAADR1      ENC_MREG(3, 0x04)
#define ENC_MAADR2      ENC_MREG(3, 0x05)
#define ENC_MISTAT      ENC_MREG(3, 0x0A)
#define ENC_EREVID      ENC_REG(3, 0x12)   /* ETH reg — no dummy byte */
#define ENC_ECOCON      ENC_REG(3, 0x15)
#define ENC_EFLOCON     ENC_REG(3, 0x17)
#define ENC_EPAUSL      ENC_REG(3, 0x18)
#define ENC_EPAUSH      ENC_REG(3, 0x19)

/* ---- PHY registers (accessed via MII) ---- */
#define ENC_PHCON1      0x00
#define ENC_PHSTAT1     0x01
#define ENC_PHID1       0x02
#define ENC_PHID2       0x03
#define ENC_PHCON2      0x10
#define ENC_PHSTAT2     0x11
#define ENC_PHIE        0x12
#define ENC_PHIR        0x13
#define ENC_PHLCON      0x14

/* ---- Bit definitions ---- */
/* ESTAT */
#define ESTAT_CLKRDY    0x01
#define ESTAT_TXABRT    0x02
#define ESTAT_RXBUSY    0x04

/* ECON1 */
#define ECON1_BSEL0     0x01
#define ECON1_BSEL1     0x02
#define ECON1_RXEN      0x04
#define ECON1_TXRTS     0x08
#define ECON1_CSUMEN    0x10
#define ECON1_DMAST     0x20
#define ECON1_RXRST     0x40
#define ECON1_TXRST     0x80

/* ECON2 */
#define ECON2_VRPS      0x08
#define ECON2_PWRSV     0x20
#define ECON2_PKTDEC    0x40
#define ECON2_AUTOINC   0x80

/* EIR */
#define EIR_RXERIF      0x01
#define EIR_TXERIF      0x02
#define EIR_TXIF        0x08
#define EIR_LINKIF      0x10
#define EIR_DMAIF       0x20
#define EIR_PKTIF       0x40

/* EIE */
#define EIE_RXERIE      0x01
#define EIE_TXERIE      0x02
#define EIE_TXIE        0x08
#define EIE_LINKIE      0x10
#define EIE_DMAIE       0x20
#define EIE_PKTIE       0x40
#define EIE_INTIE       0x80

/* ERXFCON (receive filter) */
#define ERXFCON_BCEN    0x01
#define ERXFCON_MCEN    0x02
#define ERXFCON_HTEN    0x04
#define ERXFCON_MPEN    0x08
#define ERXFCON_PMEN    0x10
#define ERXFCON_CRCEN   0x20
#define ERXFCON_ANDOR   0x40
#define ERXFCON_UCEN    0x80

/* MACON1 */
#define MACON1_MARXEN   0x01
#define MACON1_PASSALL  0x02
#define MACON1_RXPAUS   0x04
#define MACON1_TXPAUS   0x08

/* MACON3 */
#define MACON3_FULDPX   0x01
#define MACON3_FRMLNEN  0x02
#define MACON3_HFRMLEN  0x04
#define MACON3_PHDREN   0x08
#define MACON3_TXCRCEN  0x10
#define MACON3_PADCFG0  0x20
#define MACON3_PADCFG1  0x40
#define MACON3_PADCFG2  0x80

/* MACON4 */
#define MACON4_DEFER    0x40

/* MICMD */
#define MICMD_MIIRD     0x01
#define MICMD_MIISCAN   0x02

/* MISTAT */
#define MISTAT_BUSY     0x01
#define MISTAT_SCAN     0x02
#define MISTAT_NVALID   0x04

/* PHCON1 */
#define PHCON1_PDPXMD   0x0100   /* PHY duplex (1 = full) */
#define PHCON1_PPWRSV   0x0800
#define PHCON1_PLOOPBK  0x4000
#define PHCON1_PRST     0x8000

/* PHCON2 */
#define PHCON2_HDLDIS   0x0100   /* disable loopback in half-duplex */

/* PHSTAT2 */
#define PHSTAT2_PLRITY  0x0020
#define PHSTAT2_DPXSTAT 0x0200
#define PHSTAT2_LSTAT   0x0400   /* link up */

/* Receive Status Vector bits (in rsv[2]) */
#define RSV_RXOK        0x80

/* ---- Receive-buffer status vector size + per-packet control byte ---- */
#define ENC_RSV_SIZE        6    /* 2 next-ptr + 4 status vector */
#define ENC_TX_CTRL_BYTE    0x00 /* per-packet control: use MACON3 defaults */

/* ---- 8 KB buffer memory layout (0x0000..0x1FFF) ----
 * Erratum #5: ERXST must be 0x0000 for reliable receive.            */
#define ENC_RXSTART     0x0000
#define ENC_RXEND       0x19FF   /* ~6.5 KB receive FIFO (odd: erratum #14) */
#define ENC_TXSTART     0x1A00
#define ENC_TXEND       0x1FFF   /* ~1.5 KB transmit area */

#define ENC_MAX_FRAMELEN 1518    /* DA+SA+type+payload+CRC, no VLAN */

#endif /* ENC28J60_REGS_H */
