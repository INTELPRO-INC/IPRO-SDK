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
 * Word load/store decoder of the register-grant emulator in
 * spm_trap.c: lw, sw, c.lw, c.sw. Header only, tests/host/
 * test_spm_ldst.c builds it too.
 */
#pragma once

#include <stdint.h>

#define SPM_INSN_LOAD   0x03u
#define SPM_INSN_STORE  0x23u
#define SPM_FUNCT3_W    2u
/* quadrant 0 funct3 of c.lw and c.sw */
#define SPM_C_LW        2u
#define SPM_C_SW        6u

struct spm_ldst_insn {
    uint32_t store;
    /* instruction length in bytes: 2 or 4 */
    uint32_t len;
    uint32_t rs1;
    /* rd of a load, rs2 of a store */
    uint32_t reg;
    int32_t imm;
};

/* c.lw / c.sw offset: uimm[5:3] = insn[12:10], [2] = [6], [6] = [5] */
static inline int32_t spm_c_lsw_imm(uint32_t insn)
{
    return (int32_t)((((insn >> 10) & 7u) << 3) | (((insn >> 6) & 1u) << 2) |
                     (((insn >> 5) & 1u) << 6));
}

/* 0 with d filled when insn is a 32-bit word load or store, else -1 */
static inline int spm_ldst_decode(uint32_t insn, struct spm_ldst_insn *d)
{
    uint32_t funct3;

    if ((insn & 3u) != 3u) {
        funct3 = (insn >> 13) & 7u;
        if ((insn & 3u) != 0 ||
            (funct3 != SPM_C_LW && funct3 != SPM_C_SW)) {
            return -1;
        }
        d->store = funct3 == SPM_C_SW;
        d->len = 2;
        d->rs1 = 8u + ((insn >> 7) & 7u);
        d->reg = 8u + ((insn >> 2) & 7u);
        d->imm = spm_c_lsw_imm(insn);
        return 0;
    }
    funct3 = (insn >> 12) & 7u;
    if (funct3 != SPM_FUNCT3_W) {
        return -1;
    }
    d->len = 4;
    d->rs1 = (insn >> 15) & 0x1Fu;
    if ((insn & 0x7Fu) == SPM_INSN_LOAD) {
        d->store = 0;
        d->reg = (insn >> 7) & 0x1Fu;
        d->imm = (int32_t)insn >> 20;
    } else if ((insn & 0x7Fu) == SPM_INSN_STORE) {
        d->store = 1;
        d->reg = (insn >> 20) & 0x1Fu;
        /* imm[11:5] = insn[31:25], imm[4:0] = insn[11:7] */
        d->imm = (int32_t)(((insn >> 25) << 25) |
                           (((insn >> 7) & 0x1Fu) << 20)) >> 20;
    } else {
        return -1;
    }
    return 0;
}
