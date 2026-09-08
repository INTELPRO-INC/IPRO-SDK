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
 * Exception Crash Dump Engine (DUT side)
 *
 * Streams OCRAM, PSRAM, peripheral registers, and CPU state over UART
 * using a reliable block-transfer protocol with CRC-16 checksums.
 *
 * Runs entirely in exception context:
 *   - All interrupts disabled (mstatus.MIE = 0)
 *   - UART interrupt masked at peripheral level
 *   - Polling-mode TX/RX via direct register access
 *   - Zero dynamic allocation (static buffers only)
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "exc_dump.h"
#include "drv_uart.h"
#include "drv_glb.h"
#include "drv_clock.h"
#include "drv_timer.h"
#include "uart_reg.h"
#include "risc-v/Core/Include/core_feature_base.h"

/* ------------------------------------------------------------------ */
/*  Exception frame (must match interrupt.c definition)                */
/* ------------------------------------------------------------------ */

typedef struct {
    unsigned long ra;
    unsigned long tp;
    unsigned long t0, t1, t2;
    unsigned long s0, s1;
    unsigned long a0, a1, a2, a3, a4, a5;
    unsigned long cause, epc, msubm;
#ifndef __riscv_32e
    unsigned long a6, a7;
    unsigned long t3, t4, t5, t6;
#endif
} exc_frame_t;

/* ------------------------------------------------------------------ */
/*  UART base addresses                                                */
/* ------------------------------------------------------------------ */

static const uint32_t uart_bases[] = { UART0_BASE, UART1_BASE };

/* ------------------------------------------------------------------ */
/*  Peripheral register regions to dump                                */
/* ------------------------------------------------------------------ */

typedef struct {
    uint32_t base;
    uint32_t size;
} periph_region_t;

static const periph_region_t periph_regions[] = {
    { 0x30000000, 0x1000 },  /* GLB       */
    { 0x30004000, 0x0100 },  /* GPIP      */
    { 0x30005000, 0x0100 },  /* SEC_DBG   */
    { 0x30008000, 0x0100 },  /* EF_CTRL   */
    { 0x3000A000, 0x0200 },  /* SF_CTRL   */
    { 0x3000B000, 0x0100 },  /* UART0     */
    { 0x3000C000, 0x0100 },  /* UART1     */
    { 0x3000D000, 0x0100 },  /* SPI       */
    { 0x3000E000, 0x0100 },  /* I2C       */
    { 0x3000F000, 0x0200 },  /* PWM       */
    { 0x30010000, 0x0100 },  /* TIMER     */
    { 0x30011000, 0x0100 },  /* IR        */
    { 0x3001A000, 0x0500 },  /* DMA       */
    { 0x3008F000, 0x0200 },  /* HBN       */
    { 0x30090000, 0x0100 },  /* PDS       */
};

#define NUM_PERIPH_REGIONS  (sizeof(periph_regions) / sizeof(periph_regions[0]))

/* ------------------------------------------------------------------ */
/*  Static TX buffer (block header + max payload + CRC)                */
/* ------------------------------------------------------------------ */

/* 2 sync + 1 type + 2 seq + 4 addr + 2 len = 11 header bytes */
#define BLOCK_HDR_SIZE  11
#define BLOCK_MAX_FRAME (BLOCK_HDR_SIZE + EXC_DUMP_BLOCK_SIZE + 2)

static uint8_t tx_buf[BLOCK_MAX_FRAME];

/* ------------------------------------------------------------------ */
/*  CRC-16 / CCITT-FALSE  (poly 0x1021, init 0xFFFF)                  */
/* ------------------------------------------------------------------ */

static uint16_t crc16_ccitt(const uint8_t *data, uint32_t len)
{
    uint16_t crc = 0xFFFF;

    for (uint32_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/* ------------------------------------------------------------------ */
/*  Timing helpers (mcycle based)                                      */
/* ------------------------------------------------------------------ */

static uint32_t cpu_freq_hz;

static inline uint64_t get_mcycle(void)
{
    uint32_t lo, hi, hi2;

    do {
        hi  = __RV_CSR_READ(CSR_MCYCLEH);
        lo  = __RV_CSR_READ(CSR_MCYCLE);
        hi2 = __RV_CSR_READ(CSR_MCYCLEH);
    } while (hi != hi2);

    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t ms_to_cycles(uint32_t ms)
{
    return (uint64_t)ms * (cpu_freq_hz / 1000);
}

/* ------------------------------------------------------------------ */
/*  Low-level polling UART I/O                                         */
/* ------------------------------------------------------------------ */

static void uart_tx_flush(uint32_t base)
{
    /* Wait until TX FIFO is completely empty and bus is idle */
    while (readl(base + UART_STATUS_OFFSET) & UART_STS_UTX_BUS_BUSY_MSK)
        ;
}

static void uart_tx_byte(uint32_t base, uint8_t byte)
{
    /* Wait for at least 1 free slot in TX FIFO */
    while (((readl(base + UART_FIFO_CONFIG_1_OFFSET) & UART_TX_FIFO_CNT_MSK)
            >> UART_TX_FIFO_CNT_POS) == 0)
        ;
    writeb(byte, base + UART_FIFO_WDATA_OFFSET);
}

static void uart_tx_data(uint32_t base, const uint8_t *data, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        uart_tx_byte(base, data[i]);
}

static void uart_puts(uint32_t base, const char *s)
{
    while (*s)
        uart_tx_byte(base, (uint8_t)*s++);
}

static int uart_rx_byte(uint32_t base, uint32_t timeout_ms)
{
    uint64_t deadline = get_mcycle() + ms_to_cycles(timeout_ms);

    while (get_mcycle() < deadline) {
        uint32_t cnt = (readl(base + UART_FIFO_CONFIG_1_OFFSET)
                        & UART_RX_FIFO_CNT_MSK) >> UART_RX_FIFO_CNT_POS;
        if (cnt > 0)
            return readb(base + UART_FIFO_RDATA_OFFSET);
    }
    return -1;
}

static void uart_rx_flush(uint32_t base)
{
    /* Drain any stale bytes in RX FIFO */
    while (((readl(base + UART_FIFO_CONFIG_1_OFFSET)
             & UART_RX_FIFO_CNT_MSK) >> UART_RX_FIFO_CNT_POS) > 0)
        readb(base + UART_FIFO_RDATA_OFFSET);
}

/* ------------------------------------------------------------------ */
/*  Protocol: send one block with ACK/NAK retry                        */
/* ------------------------------------------------------------------ */

/*
 * Frame layout in tx_buf[]:
 *   [0]    sync_hi  0xAA
 *   [1]    sync_lo  0x55
 *   [2]    type
 *   [3..4] seq      (LE)
 *   [5..8] addr     (LE)
 *   [9..10] len     (LE)
 *   [11 .. 11+len-1] payload
 *   [11+len .. 11+len+1] crc16 (LE)
 *
 * CRC is over bytes [2 .. 11+len-1] (type through end of payload).
 */
static int send_block(uint32_t base, uint8_t type, uint32_t seq,
                      uint32_t addr, const uint8_t *payload, uint16_t len)
{
    uint32_t pos = 0;

    /* Clamp payload to max block size */
    if (len > EXC_DUMP_BLOCK_SIZE)
        len = EXC_DUMP_BLOCK_SIZE;

    /* Sync */
    tx_buf[pos++] = EXC_DUMP_SYNC_HI;
    tx_buf[pos++] = EXC_DUMP_SYNC_LO;

    /* Header (CRC starts here) */
    uint32_t crc_start = pos;
    tx_buf[pos++] = type;
    tx_buf[pos++] = (uint8_t)(seq & 0xFF);
    tx_buf[pos++] = (uint8_t)((seq >> 8) & 0xFF);
    tx_buf[pos++] = (uint8_t)(addr >>  0);
    tx_buf[pos++] = (uint8_t)(addr >>  8);
    tx_buf[pos++] = (uint8_t)(addr >> 16);
    tx_buf[pos++] = (uint8_t)(addr >> 24);
    tx_buf[pos++] = (uint8_t)(len & 0xFF);
    tx_buf[pos++] = (uint8_t)(len >> 8);

    /* Payload */
    if (payload && len > 0)
        memcpy(&tx_buf[pos], payload, len);
    pos += len;

    /* CRC over [type .. payload] */
    uint16_t crc = crc16_ccitt(&tx_buf[crc_start], pos - crc_start);
    tx_buf[pos++] = (uint8_t)(crc & 0xFF);
    tx_buf[pos++] = (uint8_t)(crc >> 8);

    /* Transmit with retry */
    for (int retry = 0; retry < EXC_DUMP_MAX_RETRIES; retry++) {
        uart_tx_data(base, tx_buf, pos);
        uart_tx_flush(base);

        int ack = uart_rx_byte(base, 2000);
        if (ack == EXC_DUMP_ACK)
            return 0;
        /* NAK or timeout -> retransmit */
    }

    return -1;  /* give up after max retries */
}

/* ------------------------------------------------------------------ */
/*  Wait for magic from PC tool                                        */
/* ------------------------------------------------------------------ */

static int wait_for_magic(uint32_t base, uint32_t timeout_ms)
{
    static const uint8_t magic[] = {
        EXC_DUMP_MAGIC_0, EXC_DUMP_MAGIC_1,
        EXC_DUMP_MAGIC_2, EXC_DUMP_MAGIC_3
    };
    int matched = 0;
    uint64_t deadline = get_mcycle() + ms_to_cycles(timeout_ms);

    while (get_mcycle() < deadline) {
        int b = uart_rx_byte(base, 100);
        if (b < 0)
            continue;
        if ((uint8_t)b == magic[matched]) {
            matched++;
            if (matched == EXC_DUMP_MAGIC_LEN)
                return 0;
        } else {
            /* Restart matching (handle partial overlap) */
            matched = ((uint8_t)b == magic[0]) ? 1 : 0;
        }
    }

    return -1;
}

/* ------------------------------------------------------------------ */
/*  Build and send: dump header                                        */
/* ------------------------------------------------------------------ */

/* Linker symbols for memory layout */
extern char _ld_ram_addr1[];   /* OCRAM origin  */
extern char _ld_ram_size1[];   /* OCRAM length  */
extern char _ld_ram_addr2[];   /* HBNRAM origin */
extern char _ld_ram_size2[];   /* HBNRAM length */

/* PSRAM symbols (weak: only defined when PSRAM enabled) */
extern char __heap_psram_start[] __attribute__((weak));
extern char __heap_psram_end[]   __attribute__((weak));

static uint32_t send_dump_header(uint32_t base, unsigned long sp, uint32_t seq)
{
    uint8_t buf[256];
    uint32_t pos = 0;

    uint32_t ocram_base = (uint32_t)_ld_ram_addr1;
    uint32_t ocram_size = (uint32_t)_ld_ram_size1;
    uint32_t hbnram_base = (uint32_t)_ld_ram_addr2;
    uint32_t hbnram_size = (uint32_t)_ld_ram_size2;

    /* Determine PSRAM range */
    uint32_t psram_base = 0;
    uint32_t psram_size = 0;

    if ((uint32_t)__heap_psram_end > 0x1e000000) {
        psram_base = 0x1e000000;
        psram_size = (uint32_t)__heap_psram_end - psram_base;
        if (psram_size > 8 * 1024 * 1024)
            psram_size = 8 * 1024 * 1024;
    }

    /* Count peripheral region total size */
    uint32_t periph_total = 0;
    for (uint32_t i = 0; i < NUM_PERIPH_REGIONS; i++)
        periph_total += periph_regions[i].size;

    /* Count regions */
    uint32_t nregions = 1;  /* OCRAM always */
    if (psram_size > 0) nregions++;
    nregions++;  /* HBNRAM */
    nregions++;  /* peripherals (aggregate) */

    /* Header struct */
    exc_dump_header_t hdr;
    hdr.magic    = EXC_DUMP_HEADER_MAGIC;
    hdr.version  = 1;
    hdr.mepc     = __RV_CSR_READ(CSR_MEPC);
    hdr.mcause   = __RV_CSR_READ(CSR_MCAUSE);
    hdr.mtval    = __RV_CSR_READ(CSR_MTVAL);
    hdr.mdcause  = __RV_CSR_READ(CSR_MDCAUSE);
    hdr.sp       = (uint32_t)sp;
    hdr.num_regions = nregions;

    memcpy(&buf[pos], &hdr, sizeof(hdr));
    pos += sizeof(hdr);

    /* Region descriptors with bounds check */
    exc_dump_region_t r;

    /* OCRAM */
    r.id = EXC_DUMP_REGION_OCRAM;
    r.base = ocram_base;
    r.size = ocram_size;
    if (pos + sizeof(r) <= sizeof(buf)) {
        memcpy(&buf[pos], &r, sizeof(r));
        pos += sizeof(r);
    }

    /* PSRAM (if present) */
    if (psram_size > 0 && pos + sizeof(r) <= sizeof(buf)) {
        r.id = EXC_DUMP_REGION_PSRAM;
        r.base = psram_base;
        r.size = psram_size;
        memcpy(&buf[pos], &r, sizeof(r));
        pos += sizeof(r);
    }

    /* HBNRAM */
    if (pos + sizeof(r) <= sizeof(buf)) {
        r.id = EXC_DUMP_REGION_HBNRAM;
        r.base = hbnram_base;
        r.size = hbnram_size;
        memcpy(&buf[pos], &r, sizeof(r));
        pos += sizeof(r);
    }

    /* Peripherals (aggregate) */
    if (pos + sizeof(r) <= sizeof(buf)) {
        r.id = EXC_DUMP_REGION_PERIPH;
        r.base = periph_regions[0].base;
        r.size = periph_total;
        memcpy(&buf[pos], &r, sizeof(r));
        pos += sizeof(r);
    }

    send_block(base, EXC_DUMP_TYPE_HEADER, seq, 0, buf, (uint16_t)pos);
    return seq + 1;
}

/* ------------------------------------------------------------------ */
/*  Build and send: CPU registers                                      */
/* ------------------------------------------------------------------ */

static uint32_t send_registers(uint32_t base, unsigned long sp, uint32_t seq)
{
    exc_frame_t *f = (exc_frame_t *)sp;
    exc_dump_regs_t regs;

    /* GPRs from exception frame */
    regs.ra = (uint32_t)f->ra;
    regs.sp = (uint32_t)sp;
    regs.tp = (uint32_t)f->tp;
    regs.t0 = (uint32_t)f->t0;
    regs.t1 = (uint32_t)f->t1;
    regs.t2 = (uint32_t)f->t2;
    regs.s0 = (uint32_t)f->s0;
    regs.s1 = (uint32_t)f->s1;
    regs.a0 = (uint32_t)f->a0;
    regs.a1 = (uint32_t)f->a1;
    regs.a2 = (uint32_t)f->a2;
    regs.a3 = (uint32_t)f->a3;
    regs.a4 = (uint32_t)f->a4;
    regs.a5 = (uint32_t)f->a5;

#ifndef __riscv_32e
    regs.a6 = (uint32_t)f->a6;
    regs.a7 = (uint32_t)f->a7;
    regs.t3 = (uint32_t)f->t3;
    regs.t4 = (uint32_t)f->t4;
    regs.t5 = (uint32_t)f->t5;
    regs.t6 = (uint32_t)f->t6;
#else
    regs.a6 = regs.a7 = 0;
    regs.t3 = regs.t4 = regs.t5 = regs.t6 = 0;
#endif

    /* gp is constant after startup */
    uint32_t gp_val;
    __asm__ volatile("mv %0, gp" : "=r"(gp_val));
    regs.gp = gp_val;

    /* CSRs */
    regs.mepc     = __RV_CSR_READ(CSR_MEPC);
    regs.mcause   = __RV_CSR_READ(CSR_MCAUSE);
    regs.mtval    = __RV_CSR_READ(CSR_MTVAL);
    regs.mstatus  = __RV_CSR_READ(CSR_MSTATUS);
    regs.mie      = __RV_CSR_READ(CSR_MIE);
    regs.mip      = __RV_CSR_READ(CSR_MIP);
    regs.mscratch = __RV_CSR_READ(CSR_MSCRATCH);
    regs.msubm    = (uint32_t)f->msubm;
    regs.mdcause  = __RV_CSR_READ(CSR_MDCAUSE);
    regs.mtvec    = __RV_CSR_READ(CSR_MTVEC);
    regs.mtvt     = __RV_CSR_READ(CSR_MTVT);
    regs.mtvt2    = __RV_CSR_READ(CSR_MTVT2);
    regs.mcycle_lo   = __RV_CSR_READ(CSR_MCYCLE);
    regs.mcycle_hi   = __RV_CSR_READ(CSR_MCYCLEH);
    regs.minstret_lo = __RV_CSR_READ(CSR_MINSTRET);
    regs.minstret_hi = __RV_CSR_READ(CSR_MINSTRETH);
    regs.mhartid  = __RV_CSR_READ(CSR_MHARTID);
    regs.misa     = __RV_CSR_READ(CSR_MISA);

    send_block(base, EXC_DUMP_TYPE_REGS, seq, 0,
               (const uint8_t *)&regs, sizeof(regs));
    return seq + 1;
}

/* ------------------------------------------------------------------ */
/*  Send a contiguous memory region in 1024-byte blocks                */
/* ------------------------------------------------------------------ */

static uint32_t send_memory_region(uint32_t uart_base, uint8_t type,
                                   uint32_t mem_base, uint32_t mem_size,
                                   uint32_t seq)
{
    uint32_t offset = 0;

    while (offset < mem_size) {
        uint32_t chunk = mem_size - offset;
        if (chunk > EXC_DUMP_BLOCK_SIZE)
            chunk = EXC_DUMP_BLOCK_SIZE;

        /* Feed watchdog to prevent reset during long dumps */
        WDT_ResetCounterValue(WDT0_ID);

        send_block(uart_base, type, seq,
                   mem_base + offset,
                   (const uint8_t *)(mem_base + offset),
                   (uint16_t)chunk);
        seq++;
        offset += chunk;
    }

    return seq;
}

/* ------------------------------------------------------------------ */
/*  Send peripheral register regions (skip active dump UART)           */
/* ------------------------------------------------------------------ */

static uint32_t send_peripherals(uint32_t uart_base, uint32_t seq)
{
    /* Mask to page-align: UART regs are within a 0x100 page */
    uint32_t active_uart_page = uart_base & ~0xFFFU;

    for (uint32_t i = 0; i < NUM_PERIPH_REGIONS; i++) {
        /* Skip the UART we are actively using for dump transfer
         * to avoid consuming RX FIFO data or clearing IRQ status */
        if (periph_regions[i].base == active_uart_page)
            continue;
        seq = send_memory_region(uart_base, EXC_DUMP_TYPE_PERIPH,
                                periph_regions[i].base,
                                periph_regions[i].size, seq);
    }
    return seq;
}

/* ------------------------------------------------------------------ */
/*  Send footer                                                        */
/* ------------------------------------------------------------------ */

static void send_footer(uint32_t uart_base, uint32_t seq)
{
    uint32_t total = seq;
    send_block(uart_base, EXC_DUMP_TYPE_FOOTER, seq, 0,
               (const uint8_t *)&total, sizeof(total));
}

/* ------------------------------------------------------------------ */
/*  Public: main entry point                                           */
/* ------------------------------------------------------------------ */

/* Total wait time before giving up (5 minutes) */
#define EXC_DUMP_TOTAL_WAIT_S  300

void exc_dump_wait_and_send(unsigned long sp, int uart_id)
{
    if (uart_id < 0 || uart_id >= UART_ID_MAX)
        return;

    uint32_t base = uart_bases[uart_id];

    /* Ensure mcycle counter is running (clear inhibit bit 0) */
    __RV_CSR_CLEAR(CSR_MCOUNTINHIBIT, 0x1);

    /* Cache CPU frequency for timeout calculations */
    cpu_freq_hz = Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_FCLK);
    if (cpu_freq_hz == 0)
        cpu_freq_hz = 320000000;  /* fallback: 320 MHz */

    /* Save original baudrate for restoration after dump */
    uint32_t orig_baudrate = 0;
    {
        uint32_t uart_clk = Clock_Peripheral_Clock_Get(
            DRV_PERIPHERAL_CLOCK_UART0);
        uint32_t div_reg = readl(base + UART_BIT_PRD_OFFSET);
        uint32_t divisor = (div_reg & 0xFFFF) + 1;
        if (divisor > 0 && uart_clk > 0)
            orig_baudrate = uart_clk / divisor;
    }

    /*
     * Step 1: Disable UART interrupt at peripheral level.
     * We are already in exception context (mstatus.MIE=0) so ISRs
     * cannot run, but disabling at ECLIC prevents any latent
     * pending interrupt from firing when we eventually unmask.
     */
    UART_IntMask((UART_ID_Type)uart_id, UART_INT_ALL, 1);

    /* Drain stale RX data (shell echo, etc.) */
    uart_rx_flush(base);

    /*
     * Step 2: Print DUMP_READY banner and wait for magic.
     * Reprints periodically so a PC tool connecting later can detect it.
     * Total wait capped at EXC_DUMP_TOTAL_WAIT_S to allow auto-reboot.
     */
    int magic_received = 0;
    uint64_t give_up = get_mcycle() +
                        ms_to_cycles(EXC_DUMP_TOTAL_WAIT_S * 1000);

    while (get_mcycle() < give_up) {
        /* Feed watchdog during wait to prevent reset */
        WDT_ResetCounterValue(WDT0_ID);

        /* Use polling uart_puts instead of printf to avoid
         * stdio dependency (heap/mutex) in exception context */
        uart_puts(base, "\r\n==DUMP_READY:");

        /* Print baudrate as decimal string */
        {
            char num[12];
            uint32_t v = EXC_DUMP_BAUDRATE;
            int i = 0;
            do {
                num[i++] = '0' + (v % 10);
                v /= 10;
            } while (v > 0);
            while (i > 0)
                uart_tx_byte(base, (uint8_t)num[--i]);
        }
        uart_puts(base, "==\r\n");
        uart_tx_flush(base);

        if (wait_for_magic(base, EXC_DUMP_READY_INTERVAL_S * 1000) == 0) {
            magic_received = 1;
            break;
        }
    }

    if (!magic_received)
        return;  /* timeout: let caller handle (auto-reboot or while(1)) */

    /*
     * Step 3: Acknowledge and switch to dump baudrate.
     * Send ACK at current baudrate so PC tool knows when to switch.
     */
    uart_puts(base, "DUMP_ACK\r\n");
    uart_tx_flush(base);

    /* Small delay for UART TX to finish (last few bits) */
    {
        uint64_t end = get_mcycle() + ms_to_cycles(50);
        while (get_mcycle() < end)
            ;
    }

    /* Switch UART baudrate */
    UART_SetBaudRate((UART_ID_Type)uart_id, EXC_DUMP_BAUDRATE);

    /* Wait for PC tool to also switch baudrate (needs time for
     * serial port close + reopen on USB-serial adapters) */
    {
        uint64_t end = get_mcycle() + ms_to_cycles(500);
        while (get_mcycle() < end)
            ;
    }

    /* Drain any garbage from baudrate transition */
    uart_rx_flush(base);

    /*
     * Step 4: Send sync pattern repeatedly until PC ACKs.
     * Each round: 32 bytes of 0xAA 0x55, then wait for ACK.
     * Retry up to 10 times (5 seconds total) to handle slow
     * USB-serial port reopen on PC side.
     */
    int sync_ok = 0;
    for (int attempt = 0; attempt < 10; attempt++) {
        for (int i = 0; i < 16; i++) {
            uart_tx_byte(base, 0xAA);
            uart_tx_byte(base, 0x55);
        }
        uart_tx_flush(base);

        int ack = uart_rx_byte(base, 500);
        if (ack == EXC_DUMP_ACK) {
            sync_ok = 1;
            break;
        }
    }

    if (!sync_ok) {
        /* PC tool did not acknowledge sync, restore baudrate and abort */
        if (orig_baudrate > 0)
            UART_SetBaudRate((UART_ID_Type)uart_id, orig_baudrate);
        return;
    }

    /*
     * Step 5: Stream dump data.
     */
    uint32_t seq = 0;

    /* 5a. Dump header (memory map + exception summary) */
    seq = send_dump_header(base, sp, seq);

    /* 5b. CPU registers */
    seq = send_registers(base, sp, seq);

    /* 5c. OCRAM */
    {
        uint32_t ocram_base = (uint32_t)_ld_ram_addr1;
        uint32_t ocram_size = (uint32_t)_ld_ram_size1;
        seq = send_memory_region(base, EXC_DUMP_TYPE_MEMORY,
                                 ocram_base, ocram_size, seq);
    }

    /* 5d. PSRAM (if enabled) */
    if ((uint32_t)__heap_psram_end > 0x1e000000) {
        uint32_t psram_base = 0x1e000000;
        uint32_t psram_size = (uint32_t)__heap_psram_end - psram_base;
        if (psram_size > 8 * 1024 * 1024)
            psram_size = 8 * 1024 * 1024;
        seq = send_memory_region(base, EXC_DUMP_TYPE_MEMORY,
                                 psram_base, psram_size, seq);
    }

    /* 5e. HBNRAM */
    {
        uint32_t hbnram_base = (uint32_t)_ld_ram_addr2;
        uint32_t hbnram_size = (uint32_t)_ld_ram_size2;
        seq = send_memory_region(base, EXC_DUMP_TYPE_MEMORY,
                                 hbnram_base, hbnram_size, seq);
    }

    /* 5f. Peripheral registers */
    seq = send_peripherals(base, seq);

    /* 5g. Footer */
    send_footer(base, seq);

    /* Restore original baudrate so final message is readable */
    if (orig_baudrate > 0)
        UART_SetBaudRate((UART_ID_Type)uart_id, orig_baudrate);

    uart_puts(base, "\r\nDUMP_COMPLETE\r\n");
    uart_tx_flush(base);
}
