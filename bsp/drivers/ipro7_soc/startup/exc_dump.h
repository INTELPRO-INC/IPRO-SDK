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
 * Exception Crash Dump over UART
 *
 * After exception diagnostics are printed, the handler waits for a
 * magic sequence from a PC tool.  Once received, it switches the UART
 * to high-speed mode and streams OCRAM, PSRAM, peripheral registers,
 * and CPU state using a block-transfer protocol with CRC-16 checksums.
 *
 * Protocol (little-endian, binary):
 *
 *  Block frame:
 *  ┌──────┬──────┬──────┬──────────┬───────┬──────────┬───────┐
 *  │ 0xAA │ 0x55 │ Type │ Seq (2B) │Addr(4)│ Len (2B) │Payload│→ CRC16(2)
 *  └──────┴──────┴──────┴──────────┴───────┴──────────┴───────┘
 *
 *  CRC-16/CCITT-FALSE over [Type .. Payload].
 *  PC replies: ACK 0x06 | NAK 0x15.
 */

#ifndef __EXC_DUMP_H__
#define __EXC_DUMP_H__

#include <stdint.h>

/* ------------------------------------------------------------------ */
/*  Protocol constants                                                 */
/* ------------------------------------------------------------------ */

/* Magic sent by PC tool to trigger dump (4 bytes) */
#define EXC_DUMP_MAGIC_0        0xDB
#define EXC_DUMP_MAGIC_1        0x5A
#define EXC_DUMP_MAGIC_2        0xA5
#define EXC_DUMP_MAGIC_3        0xDB
#define EXC_DUMP_MAGIC_LEN      4

/* Sync word at start of every block */
#define EXC_DUMP_SYNC_HI        0xAA
#define EXC_DUMP_SYNC_LO        0x55

/* ACK / NAK from PC */
#define EXC_DUMP_ACK            0x06
#define EXC_DUMP_NAK            0x15

/* Block payload size (bytes) */
#define EXC_DUMP_BLOCK_SIZE     1024

/* Dump UART baudrate */
#define EXC_DUMP_BAUDRATE       2000000

/* Retries per block before skip */
#define EXC_DUMP_MAX_RETRIES    3

/* Seconds between DUMP_READY reprints while waiting for magic */
#define EXC_DUMP_READY_INTERVAL_S  10

/* ------------------------------------------------------------------ */
/*  Block types                                                        */
/* ------------------------------------------------------------------ */

#define EXC_DUMP_TYPE_HEADER    0x01   /* dump header (memory map)  */
#define EXC_DUMP_TYPE_REGS      0x02   /* CPU registers + CSRs      */
#define EXC_DUMP_TYPE_MEMORY    0x03   /* memory data               */
#define EXC_DUMP_TYPE_PERIPH    0x04   /* peripheral registers      */
#define EXC_DUMP_TYPE_FOOTER    0xFE   /* end-of-dump marker        */
#define EXC_DUMP_TYPE_ERROR     0xFF   /* region unreadable         */

/* ------------------------------------------------------------------ */
/*  Memory region IDs (carried in dump header)                         */
/* ------------------------------------------------------------------ */

#define EXC_DUMP_REGION_OCRAM   0x01
#define EXC_DUMP_REGION_PSRAM   0x02
#define EXC_DUMP_REGION_HBNRAM  0x03
#define EXC_DUMP_REGION_PERIPH  0x04

/* ------------------------------------------------------------------ */
/*  Dump header (first block payload)                                  */
/* ------------------------------------------------------------------ */

#define EXC_DUMP_HEADER_MAGIC   0xDEAD0007

typedef struct __attribute__((packed)) {
    uint32_t magic;            /* EXC_DUMP_HEADER_MAGIC */
    uint32_t version;          /* protocol version (1)  */
    uint32_t mepc;
    uint32_t mcause;
    uint32_t mtval;
    uint32_t mdcause;
    uint32_t sp;
    uint32_t num_regions;
    /* followed by num_regions x exc_dump_region_t */
} exc_dump_header_t;

typedef struct __attribute__((packed)) {
    uint8_t  id;               /* EXC_DUMP_REGION_xxx   */
    uint32_t base;
    uint32_t size;
} exc_dump_region_t;

/* ------------------------------------------------------------------ */
/*  Register dump (second block payload)                               */
/* ------------------------------------------------------------------ */

typedef struct __attribute__((packed)) {
    /* GPRs */
    uint32_t ra, sp, gp, tp;
    uint32_t t0, t1, t2;
    uint32_t s0, s1;
    uint32_t a0, a1, a2, a3, a4, a5, a6, a7;
    uint32_t t3, t4, t5, t6;
    /* CSRs */
    uint32_t mepc, mcause, mtval, mstatus;
    uint32_t mie, mip, mscratch, msubm;
    uint32_t mdcause, mtvec, mtvt, mtvt2;
    uint32_t mcycle_lo, mcycle_hi;
    uint32_t minstret_lo, minstret_hi;
    uint32_t mhartid, misa;
} exc_dump_regs_t;

/* ------------------------------------------------------------------ */
/*  Public API                                                         */
/* ------------------------------------------------------------------ */

/**
 * Wait for PC magic, then stream full crash dump via UART.
 *
 * Called from exception handler after diagnostic prints.
 * Disables UART interrupts and switches to polling mode at
 * EXC_DUMP_BAUDRATE.  Never returns to caller on success;
 * returns on magic wait timeout.
 *
 * @param sp       Stack pointer at exception time (EXC_Frame_Type *)
 * @param uart_id  UART peripheral index (0 = UART0, 1 = UART1)
 */
void exc_dump_wait_and_send(unsigned long sp, int uart_id);

#endif /* __EXC_DUMP_H__ */
