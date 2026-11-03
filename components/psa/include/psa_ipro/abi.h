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

#pragma once
#include "psa_ipro/asm.h"
#ifndef __ASSEMBLER__
#include <stdint.h>
#endif
/* ecall function ids (a7) */
#define PSA_ECALL_FRAMEWORK_VERSION PSA_U(0x00)
#define PSA_ECALL_VERSION           PSA_U(0x01)
#define PSA_ECALL_CALL              PSA_U(0x02)
#define PSA_ECALL_CONNECT           PSA_U(0x03)
#define PSA_ECALL_CLOSE             PSA_U(0x04)
#define NSPE_ECALL_INIT             PSA_U(0x10)
#define NSPE_ECALL_IRQ_RETURN       PSA_U(0x11)
#define NSPE_ECALL_IRQ_DELIVER      PSA_U(0x12)
#define NSPE_ECALL_IRQ_ATTACH       PSA_U(0x13)
#define NSPE_ECALL_IRQ_DETACH       PSA_U(0x14)
#define NSPE_ECALL_TIMER_SET        PSA_U(0x15)
#define NSPE_ECALL_YIELD            PSA_U(0x16)
#define NSPE_ECALL_WFI              PSA_U(0x17)
#define NSPE_ECALL_RESET            PSA_U(0x18)
#define NSPE_ECALL_LOG              PSA_U(0x20)
/* static handles (FF-M 1.1 stateless) */
#define PSA_HANDLE_CRYPTO   0x40000100
#define PSA_HANDLE_ITS      0x40000101
#define PSA_HANDLE_ATTEST   0x40000102
#define PSA_HANDLE_PLATFORM 0x40000103
#define PSA_HANDLE_FWU      0x40000104
#define PSA_FRAMEWORK_VERSION_VALUE PSA_U(0x0101)
#define PSA_MAX_IOVEC PSA_U(4)
/* virtual IRQ ids: ECLIC ids for peripherals, these for SPE events */
#define VIRQ_NOTIFY PSA_U(1)
#define VIRQ_FAULT  PSA_U(2)
#define VIRQ_YIELD  PSA_U(3)
#define VIRQ_TICK   PSA_U(7)
/* bitmap covers ECLIC ids 0..95 */
#define VIRQ_WORDS  PSA_U(3)
#define PSA_NSPE_FRAME_WORDS PSA_U(66)
/* 66*4 = 264 rounded up to 16 */
#define PSA_NSPE_FRAME_SIZE  PSA_U(272)
/* NSPE image header at PSA_NSPE_FLASH_BASE: "NSPE" */
#define NSPE_HDR_MAGIC PSA_U(0x4550534E)
#ifndef __ASSEMBLER__
/*
 * virq_mask: vIRQ delivery writes 1 (mirrors a trap clearing MIE).
 * NSPE_ECALL_IRQ_RETURN writes 0 after loading the frame and before
 * the pending check (mirrors mret copying MPIE to MIE).
 */
struct psa_nspe_ctrl {
    volatile uint32_t virq_mask;
    volatile uint32_t virq_pending[VIRQ_WORDS];
    volatile uint32_t fault_mcause;
    volatile uint32_t fault_mtval;
    volatile uint32_t fault_mepc;
    volatile uint32_t stat_traps;
    volatile uint32_t stat_emul;
    volatile uint32_t stat_irq;
    uint32_t reserved[6];
};
/* U-mode context pushed on the NSPE stack, restored by IRQ_RETURN */
struct psa_nspe_frame {
    uint32_t epc;               /* [0] */
    uint32_t x[31];             /* [1..31] = x1..x31; x[1] is sp */
    uint32_t fstate;            /* [32] 1 when f[] and fcsr valid */
    uint32_t fcsr;              /* [33] */
    uint32_t f[32];             /* [34..65] */
};
struct psa_nspe_header {
    uint32_t magic;
    uint32_t hdr_size;          /* 32 */
    uint32_t image_size;        /* bytes measured, from hdr start */
    uint32_t entry;             /* U-mode entry pc */
    uint32_t stack_top;         /* initial sp, inside NSPE OCRAM */
    uint32_t version;           /* NSPE sw version, 0xMMmmpppp */
    uint32_t reserved[2];
};
_Static_assert(sizeof(struct psa_nspe_ctrl) == 64, "psa_nspe_ctrl");
_Static_assert(sizeof(struct psa_nspe_frame) == 264, "psa_nspe_frame");
_Static_assert(sizeof(struct psa_nspe_header) == 32, "psa_nspe_header");
#endif
