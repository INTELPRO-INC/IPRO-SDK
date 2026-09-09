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
 * @file enc28j60.c
 * @brief Minimal ENC28J60 SPI Ethernet driver — bring-up / raw-frame test.
 *
 * SPI transaction model: every ENC28J60 instruction (opcode + its data) is a
 * single CS-low window. CS is software-driven (GPIO), so each helper does
 * select -> transfer(s) -> deselect. Banked control registers are selected
 * via ECON1[1:0]; MAC/MII registers return a dummy byte first on read.
 */

#include "enc28j60.h"
#include "enc28j60_regs.h"
#include "enc28j60_port.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "hal_spi.h"
#include "hal_gpio.h"
#include "drv_glb.h"   /* GLB_GPIO_Func_Init */
#include "drv_gpio.h"  /* GPIO_FUN_SPI */
#include "ipro_log.h"

#define LOG_TAG "ENC28J60"

/* Bounded spin used while polling MISTAT.BUSY / TXRTS so a missing chip can
 * never hang the shell task. ~tens of µs worst case per SPI op. */
#define ENC_POLL_LIMIT   200000u

static uint8_t  s_bank     = 0xFF;        /* cached ECON1 bank (0xFF = unknown) */
static uint16_t s_next_pkt = ENC_RXSTART; /* next-packet pointer in RX FIFO     */
static uint8_t  s_pin_swap = ENC_SPI_PIN_SWAP; /* runtime-overridable MOSI/MISO swap */

/* ----------------------------------------------------------------------- */
/* SPI primitives (software chip-select)                                   */
/* ----------------------------------------------------------------------- */

static inline void enc_select(void) { gpio_write(ENC_PIN_CS, 0); }

/* spi_transmit() returns once the last byte is loaded into the TX register,
 * not when it has finished clocking out. Wait for the bus to go idle before
 * raising CS, or the final byte of a write is chopped off mid-transfer. */
static inline void enc_deselect(void)
{
    while (spi_ioctl(ENC_SPI_ID, SPI_CTRL_GET_BUS_BUSY_STATUS, NULL)) {
    }
    gpio_write(ENC_PIN_CS, 1);
}

/* One CS-low transfer (full-duplex if both buffers given). */
static void enc_xfer(const uint8_t *tx, uint8_t *rx, uint32_t len)
{
    enc_select();
    if (tx && rx) {
        spi_transmit_receive(ENC_SPI_ID, tx, rx, len);
    } else if (tx) {
        spi_transmit(ENC_SPI_ID, (void *)tx, len);
    } else if (rx) {
        spi_receive(ENC_SPI_ID, rx, len);
    }
    enc_deselect();
}

/* Bank-select for a banked register. Common registers (addr >= 0x1B) and the
 * MII/MAC banking is handled here transparently. */
static void enc_set_bank(uint8_t reg);

static void enc_op2(uint8_t op, uint8_t arg)
{
    uint8_t tx[2] = { op, arg };
    enc_xfer(tx, NULL, 2);
}

static uint8_t enc_rcr(uint8_t reg)
{
    enc_set_bank(reg);
    uint8_t addr = ENC_REG_ADDR(reg);
    if (ENC_REG_IS_MAC_MII(reg)) {
        uint8_t tx[3] = { (uint8_t)(ENC_OP_RCR | addr), 0, 0 };
        uint8_t rx[3] = { 0, 0, 0 };
        enc_xfer(tx, rx, 3);          /* [opcode][dummy][data] */
        return rx[2];
    } else {
        uint8_t tx[2] = { (uint8_t)(ENC_OP_RCR | addr), 0 };
        uint8_t rx[2] = { 0, 0 };
        enc_xfer(tx, rx, 2);          /* [opcode][data] */
        return rx[1];
    }
}

static void enc_wcr(uint8_t reg, uint8_t val)
{
    enc_set_bank(reg);
    enc_op2((uint8_t)(ENC_OP_WCR | ENC_REG_ADDR(reg)), val);
}

static void enc_bfs(uint8_t reg, uint8_t mask)
{
    enc_set_bank(reg);
    enc_op2((uint8_t)(ENC_OP_BFS | ENC_REG_ADDR(reg)), mask);
}

static void enc_bfc(uint8_t reg, uint8_t mask)
{
    enc_set_bank(reg);
    enc_op2((uint8_t)(ENC_OP_BFC | ENC_REG_ADDR(reg)), mask);
}

static void enc_set_bank(uint8_t reg)
{
    /* EIE/EIR/ESTAT/ECON2/ECON1 (addr 0x1B-0x1F) are bank-independent. */
    if (ENC_REG_ADDR(reg) >= 0x1B) {
        return;
    }
    uint8_t bank = ENC_REG_BANK(reg);
    if (bank == s_bank) {
        return;
    }
    /* ECON1 is itself bank-independent, so these BFC/BFS recurse into the
     * early-return above — no infinite loop. */
    enc_bfc(ENC_ECON1, ECON1_BSEL1 | ECON1_BSEL0);
    enc_bfs(ENC_ECON1, (uint8_t)(bank & 0x03));
    s_bank = bank;
}

/* Buffer-memory streaming: opcode then N bytes, all in one CS-low window. */
static void enc_read_buffer(uint8_t *buf, uint16_t len)
{
    uint8_t op = ENC_OP_RBM;
    enc_select();
    spi_transmit(ENC_SPI_ID, &op, 1);
    if (len) {
        spi_receive(ENC_SPI_ID, buf, len);
    }
    enc_deselect();
}

static void enc_write_buffer(const uint8_t *buf, uint16_t len)
{
    uint8_t op = ENC_OP_WBM;
    enc_select();
    spi_transmit(ENC_SPI_ID, &op, 1);
    if (len) {
        spi_transmit(ENC_SPI_ID, (void *)buf, len);
    }
    enc_deselect();
}

static void enc_soft_reset(void)
{
    uint8_t op = ENC_OP_SRC;
    enc_select();
    spi_transmit(ENC_SPI_ID, &op, 1);
    enc_deselect();
    s_bank = 0xFF;
}

/* ----------------------------------------------------------------------- */
/* PHY access (via MII)                                                    */
/* ----------------------------------------------------------------------- */

uint16_t enc28j60_phy_read(uint8_t addr)
{
    enc_wcr(ENC_MIREGADR, addr);
    enc_bfs(ENC_MICMD, MICMD_MIIRD);
    uint32_t spin = ENC_POLL_LIMIT;
    while ((enc_rcr(ENC_MISTAT) & MISTAT_BUSY) && --spin) {
        /* MII read completes in ~10.24 µs */
    }
    enc_bfc(ENC_MICMD, MICMD_MIIRD);
    uint16_t v = enc_rcr(ENC_MIRDL);
    v |= (uint16_t)enc_rcr(ENC_MIRDH) << 8;
    return v;
}

void enc28j60_phy_write(uint8_t addr, uint16_t val)
{
    enc_wcr(ENC_MIREGADR, addr);
    enc_wcr(ENC_MIWRL, (uint8_t)(val & 0xFF));
    enc_wcr(ENC_MIWRH, (uint8_t)(val >> 8));     /* writing MIWRH starts the op */
    uint32_t spin = ENC_POLL_LIMIT;
    while ((enc_rcr(ENC_MISTAT) & MISTAT_BUSY) && --spin) {
    }
}

/* ----------------------------------------------------------------------- */
/* Hardware bring-up                                                       */
/* ----------------------------------------------------------------------- */

static void enc_gpio_spi_init(void)
{
    /* GPIO16/17/19 -> hardware SPI0 (signal fixed by pin position). */
    uint8_t spi_pins[3] = { ENC_PIN_MOSI, ENC_PIN_MISO, ENC_PIN_SCLK };
    GLB_GPIO_Func_Init(GPIO_FUN_SPI, spi_pins, 3);

    /* CS=18 / RST=14 / INT=15 all default to an SPI alt-function (SS / SS /
     * SCLK). gpio_set_mode() alone does NOT clear the alt-function select, so
     * the SPI peripheral would keep driving the pin — CS would never toggle
     * and the chip is never selected. Force them to plain GPIO first. */
    uint8_t gpio_pins[3] = { ENC_PIN_CS, ENC_PIN_RST, ENC_PIN_INT };
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, gpio_pins, 3);

    /* Control pins as plain GPIO. CS and RST idle high (RST is active-low). */
    gpio_set_mode(ENC_PIN_CS, GPIO_OUTPUT_MODE);
    gpio_write(ENC_PIN_CS, 1);
    gpio_set_mode(ENC_PIN_RST, GPIO_OUTPUT_MODE);
    gpio_write(ENC_PIN_RST, 1);
    gpio_set_mode(ENC_PIN_INT, GPIO_INPUT_PP_MODE);

    /* spi_setConfig() only writes the config struct; spi_init() READS that
     * struct to program the hardware. So setConfig MUST come BEFORE init —
     * otherwise init applies the board default SPI0_CONFIG (ignore_cs=0 AND
     * pin_swap_enable=1, which crosses MOSI/MISO) and our settings are lost. */
    spi_device_t dev = {
        .caps            = 0,
        .id              = ENC_SPI_ID,
        .clk             = ENC_SPI_CLK_HZ,
        .mode            = SPI_MASTER_MODE,
        .direction       = SPI_MSB_BYTE0_DIRECTION_FIRST,
        .ignore_cs       = 1,                  /* software CS (GPIO above) */
        .clk_polaraity   = ENC_SPI_CPOL,       /* mode 0: CPOL=0 */
        .clk_phase       = ENC_SPI_CPHA,       /* mode 0: CPHA=0 */
        .datasize        = SPI_DATASIZE_8BIT,
        .fifo_threshold  = 0,
        .pin_swap_enable = s_pin_swap,         /* 0 = GPIO16 MOSI, GPIO17 MISO */
        .delitch_cnt     = 0,
        .callback        = NULL,
    };
    spi_setConfig(&dev);
    spi_init(ENC_SPI_ID);
    spi_enable(ENC_SPI_ID, 1);
}

static void enc_hard_reset(void)
{
    gpio_write(ENC_PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(2));      /* hold reset */
    gpio_write(ENC_PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(2));      /* oscillator start-up */
}

static void enc_apply_rx_filter(uint8_t fcon)
{
    /* Datasheet: disable receive before changing the filter. */
    enc_bfc(ENC_ECON1, ECON1_RXEN);
    enc_wcr(ENC_ERXFCON, fcon);
    enc_bfs(ENC_ECON1, ECON1_RXEN);
}

int enc28j60_init(const uint8_t mac[6], bool full_duplex)
{
    if (!mac) {
        return ENC28J60_ERR_PARAM;
    }

    enc_gpio_spi_init();
    enc_hard_reset();
    enc_soft_reset();
    /* Erratum #2: CLKRDY is unreliable right after a soft reset — wait ≥1 ms
     * before touching any other register. */
    vTaskDelay(pdMS_TO_TICKS(2));

    uint32_t spin = 1000;
    while (!(enc_rcr(ENC_ESTAT) & ESTAT_CLKRDY) && --spin) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    if (spin == 0) {
        IPRO_LOGE(LOG_TAG, "CLKRDY never asserted (chip absent / no power?)");
        return ENC28J60_ERR_NO_CLKRDY;
    }

    uint8_t rev = enc_rcr(ENC_EREVID);
    if (rev == 0x00 || rev == 0xFF) {
        IPRO_LOGE(LOG_TAG, "EREVID=0x%02X — SPI link dead (wiring/MISO-MOSI swap?)", rev);
        return ENC28J60_ERR_NO_CHIP;
    }

    /* --- Receive FIFO --- */
    enc_wcr(ENC_ERXSTL, ENC_RXSTART & 0xFF);
    enc_wcr(ENC_ERXSTH, ENC_RXSTART >> 8);
    enc_wcr(ENC_ERXNDL, ENC_RXEND & 0xFF);
    enc_wcr(ENC_ERXNDH, ENC_RXEND >> 8);
    enc_wcr(ENC_ERXRDPTL, ENC_RXEND & 0xFF);   /* odd, per erratum #14 */
    enc_wcr(ENC_ERXRDPTH, ENC_RXEND >> 8);
    enc_wcr(ENC_ERDPTL, ENC_RXSTART & 0xFF);
    enc_wcr(ENC_ERDPTH, ENC_RXSTART >> 8);
    s_next_pkt = ENC_RXSTART;

    /* --- Transmit area --- */
    enc_wcr(ENC_ETXSTL, ENC_TXSTART & 0xFF);
    enc_wcr(ENC_ETXSTH, ENC_TXSTART >> 8);
    enc_wcr(ENC_ETXNDL, ENC_TXEND & 0xFF);
    enc_wcr(ENC_ETXNDH, ENC_TXEND >> 8);

    /* --- Receive filter: unicast (our MAC) + broadcast + CRC valid --- */
    enc_wcr(ENC_ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);

    /* --- MAC --- */
    uint8_t macon1 = MACON1_MARXEN;
    if (full_duplex) {
        macon1 |= MACON1_TXPAUS | MACON1_RXPAUS;
    }
    enc_wcr(ENC_MACON1, macon1);

    uint8_t macon3 = MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN;
    if (full_duplex) {
        macon3 |= MACON3_FULDPX;
    }
    enc_wcr(ENC_MACON3, macon3);
    enc_wcr(ENC_MACON4, MACON4_DEFER);

    enc_wcr(ENC_MAMXFLL, ENC_MAX_FRAMELEN & 0xFF);
    enc_wcr(ENC_MAMXFLH, ENC_MAX_FRAMELEN >> 8);

    enc_wcr(ENC_MABBIPG, full_duplex ? 0x15 : 0x12);
    enc_wcr(ENC_MAIPGL, 0x12);
    if (!full_duplex) {
        enc_wcr(ENC_MAIPGH, 0x0C);
    }

    enc_wcr(ENC_MAADR1, mac[0]);
    enc_wcr(ENC_MAADR2, mac[1]);
    enc_wcr(ENC_MAADR3, mac[2]);
    enc_wcr(ENC_MAADR4, mac[3]);
    enc_wcr(ENC_MAADR5, mac[4]);
    enc_wcr(ENC_MAADR6, mac[5]);

    /* --- PHY duplex --- */
    if (full_duplex) {
        enc28j60_phy_write(ENC_PHCON1, PHCON1_PDPXMD);
    } else {
        enc28j60_phy_write(ENC_PHCON2, PHCON2_HDLDIS);
    }

    /* --- Enable receive --- */
    enc_bfs(ENC_ECON2, ECON2_AUTOINC);
    enc_bfs(ENC_ECON1, ECON1_RXEN);

    IPRO_LOGI(LOG_TAG, "init OK: rev=0x%02X %s-duplex, MAC %02X:%02X:%02X:%02X:%02X:%02X",
              rev, full_duplex ? "full" : "half",
              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return ENC28J60_OK;
}

/* ----------------------------------------------------------------------- */
/* Public register / status accessors                                      */
/* ----------------------------------------------------------------------- */

uint8_t enc28j60_read_reg(uint8_t reg)            { return enc_rcr(reg); }
void    enc28j60_write_reg(uint8_t reg, uint8_t v){ enc_wcr(reg, v); }
uint8_t enc28j60_read_revid(void)                 { return enc_rcr(ENC_EREVID); }
uint8_t enc28j60_pktcnt(void)                     { return enc_rcr(ENC_EPKTCNT); }

uint32_t enc28j60_read_phid(void)
{
    uint32_t id = (uint32_t)enc28j60_phy_read(ENC_PHID1) << 16;
    id |= enc28j60_phy_read(ENC_PHID2);
    return id;
}

bool enc28j60_link_up(void)
{
    return (enc28j60_phy_read(ENC_PHSTAT2) & PHSTAT2_LSTAT) != 0;
}

void enc28j60_set_promiscuous(bool on)
{
    enc_apply_rx_filter(on ? 0x00
                           : (ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN));
}

/* ----------------------------------------------------------------------- */
/* Frame TX / RX                                                           */
/* ----------------------------------------------------------------------- */

int enc28j60_send(const uint8_t *frame, uint16_t len)
{
    if (!frame || len == 0 || len > ENC_MAX_FRAMELEN) {
        return ENC28J60_ERR_PARAM;
    }

    /* Erratum #12: reset the transmit logic before every frame to clear a
     * possible stale TXRTS / internal abort latch. */
    enc_bfs(ENC_ECON1, ECON1_TXRST);
    enc_bfc(ENC_ECON1, ECON1_TXRST);
    enc_bfc(ENC_EIR, EIR_TXIF | EIR_TXERIF);

    /* Write pointer -> start of TX area; lay down control byte + frame. */
    enc_wcr(ENC_EWRPTL, ENC_TXSTART & 0xFF);
    enc_wcr(ENC_EWRPTH, ENC_TXSTART >> 8);
    uint8_t ctrl = ENC_TX_CTRL_BYTE;
    enc_write_buffer(&ctrl, 1);
    enc_write_buffer(frame, len);

    /* ETXND points at the last data byte: TXSTART(ctrl) + len. */
    uint16_t end = (uint16_t)(ENC_TXSTART + len);
    enc_wcr(ENC_ETXNDL, end & 0xFF);
    enc_wcr(ENC_ETXNDH, end >> 8);

    enc_bfs(ENC_ECON1, ECON1_TXRTS);

    uint32_t spin = ENC_POLL_LIMIT;
    while ((enc_rcr(ENC_ECON1) & ECON1_TXRTS) && --spin) {
    }
    if (spin == 0) {
        enc_bfc(ENC_ECON1, ECON1_TXRTS);
        return ENC28J60_ERR_TX_TIMEOUT;
    }
    if (enc_rcr(ENC_ESTAT) & ESTAT_TXABRT) {
        return ENC28J60_ERR_TX_ABORT;
    }
    return ENC28J60_OK;
}

int enc28j60_recv(uint8_t *buf, uint16_t maxlen)
{
    if (!buf) {
        return ENC28J60_ERR_PARAM;
    }
    if (enc28j60_pktcnt() == 0) {
        return 0;
    }

    /* Point the read pointer at the start of the oldest pending packet. */
    enc_wcr(ENC_ERDPTL, s_next_pkt & 0xFF);
    enc_wcr(ENC_ERDPTH, s_next_pkt >> 8);

    uint8_t hdr[ENC_RSV_SIZE];
    enc_read_buffer(hdr, ENC_RSV_SIZE);
    uint16_t next  = (uint16_t)(hdr[0] | (hdr[1] << 8));
    uint16_t rxlen = (uint16_t)(hdr[2] | (hdr[3] << 8));
    uint8_t  stat  = hdr[4];                 /* RSV bits 16-23; RXOK = bit 7 */

    int copied = 0;
    if ((stat & RSV_RXOK) && rxlen >= 4) {
        rxlen -= 4;                          /* strip the 4-byte FCS */
        copied = (rxlen > maxlen) ? maxlen : (int)rxlen;
        enc_read_buffer(buf, (uint16_t)copied);
        /* Any bytes beyond maxlen are left unread; the FIFO slot is freed
         * when ERXRDPT advances below, so the next packet stays aligned. */
    }

    /* Advance to the next packet. Guard against a corrupt pointer (would
     * otherwise walk us out of the FIFO forever). */
    if (next > ENC_RXEND || (next & 1)) {
        next = ENC_RXSTART;
    }
    s_next_pkt = next;

    uint16_t rdpt = (next == ENC_RXSTART) ? ENC_RXEND : (uint16_t)(next - 1);
    enc_wcr(ENC_ERXRDPTL, rdpt & 0xFF);      /* stays odd (erratum #14) */
    enc_wcr(ENC_ERXRDPTH, rdpt >> 8);

    enc_bfs(ENC_ECON2, ECON2_PKTDEC);        /* one fewer packet pending */
    return copied;
}

/* ----------------------------------------------------------------------- */
/* Diagnostics                                                             */
/* ----------------------------------------------------------------------- */

void enc28j60_set_pin_swap(bool on)
{
    s_pin_swap = on ? 1 : 0;
}

void enc28j60_bus_init(void)
{
    enc_gpio_spi_init();
}

void enc28j60_pin_out(uint8_t pin, int level)
{
    uint8_t p = pin;
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, &p, 1);
    gpio_set_mode(pin, GPIO_OUTPUT_MODE);
    gpio_write(pin, level ? 1 : 0);
}

int enc28j60_pin_in(uint8_t pin)
{
    uint8_t p = pin;
    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, &p, 1);
    gpio_set_mode(pin, GPIO_INPUT_PP_MODE);
    return gpio_read(pin);
}

void enc28j60_loopback(const uint8_t *tx, uint8_t *rx, uint16_t n)
{
    /* Configure the SPI bus (no chip reset) and clock the pattern out. With
     * GPIO16(MOSI) jumpered to GPIO17(MISO), a working SoC SPI echoes tx->rx.
     * CS state is irrelevant for the external bridge. */
    enc_gpio_spi_init();
    enc_select();
    spi_transmit_receive(ENC_SPI_ID, tx, rx, n);
    enc_deselect();
}
