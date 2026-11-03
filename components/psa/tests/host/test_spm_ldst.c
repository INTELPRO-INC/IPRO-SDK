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
 * spm_ldst.h: the word load/store decoder of the spm_trap.c
 * register-grant emulator. Encodings from riscv64-unknown-elf-as
 * -march=rv32imafc (objdump -M no-aliases).
 */
#include <stdint.h>

#include "check.h"
#include "spm_ldst.h"

static const struct {
    uint32_t insn;
    uint32_t store;
    uint32_t len;
    uint32_t rs1;
    uint32_t reg;
    int32_t imm;
    const char *what;
} good[] = {
    { 0x22872783u, 0, 4, 14, 15, 552, "lw a5,552(a4)" },
    { 0x22f72623u, 1, 4, 14, 15, 556, "sw a5,556(a4)" },
    { 0xffc42303u, 0, 4, 8, 6, -4, "lw t1,-4(s0)" },
    { 0x809fa023u, 1, 4, 31, 9, -2048, "sw s1,-2048(t6)" },
    { 0x7ff0a003u, 0, 4, 1, 0, 2047, "lw zero,2047(ra)" },
    { 0x431cu, 0, 2, 14, 15, 0, "c.lw a5,0(a4)" },
    { 0xdce8u, 1, 2, 9, 10, 124, "c.sw a0,124(s1)" },
    { 0x43a0u, 0, 2, 15, 8, 64, "c.lw s0,64(a5)" },
};

static const struct {
    uint32_t insn;
    const char *what;
} bad[] = {
    { 0x4792u, "c.lwsp a5,4(sp)" },
    { 0x00071783u, "lh a5,0(a4)" },
    { 0x00f70023u, "sb a5,0(a4)" },
    { 0x631cu, "c.flw fa5,0(a4)" },
    { 0x00000073u, "ecall" },
    { 0x0000u, "c.unimp (0)" },
};

int main(void)
{
    for (unsigned i = 0; i < sizeof(good) / sizeof(good[0]); i++) {
        struct spm_ldst_insn d;
        int ok = spm_ldst_decode(good[i].insn, &d) == 0 &&
                 d.store == good[i].store && d.len == good[i].len &&
                 d.rs1 == good[i].rs1 && d.reg == good[i].reg &&
                 d.imm == good[i].imm;

        check(ok, good[i].what);
    }
    for (unsigned i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
        struct spm_ldst_insn d;

        check(spm_ldst_decode(bad[i].insn, &d) != 0, bad[i].what);
    }
    return check_summary();
}
