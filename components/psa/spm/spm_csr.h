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
 * Zicsr instruction decoder of the U-mode CSR emulator in
 * spm_trap.c; header only, tests/host/test_spm_csr.c builds it too.
 */
#pragma once

#include <stdint.h>

#define SPM_INSN_SYSTEM 0x73u
/* funct3 & 3 of csrrw(i), csrrs(i), csrrc(i) */
#define SPM_CSR_RW      1u
#define SPM_CSR_RS      2u
#define SPM_CSR_RC      3u

struct spm_csr_insn {
    uint32_t csr;
    uint32_t rd;
    /* rs1 register number, or the uimm when imm is 1 */
    uint32_t rs1;
    uint32_t op;
    uint32_t imm;
};

/* 0 with d filled when insn is a Zicsr instruction, else -1 */
static inline int spm_csr_decode(uint32_t insn, struct spm_csr_insn *d)
{
    uint32_t funct3 = (insn >> 12) & 7u;

    if ((insn & 0x7Fu) != SPM_INSN_SYSTEM || (funct3 & 3u) == 0) {
        return -1;
    }
    d->csr = insn >> 20;
    d->rd = (insn >> 7) & 0x1Fu;
    d->rs1 = (insn >> 15) & 0x1Fu;
    d->op = funct3 & 3u;
    d->imm = funct3 >> 2;
    return 0;
}

/* 1 for the forms that leave the CSR unchanged: csrrs/csrrc with x0,
 * csrrsi/csrrci with uimm 0; csrrw(i) always writes */
static inline int spm_csr_is_read(const struct spm_csr_insn *d)
{
    return d->op != SPM_CSR_RW && d->rs1 == 0;
}
