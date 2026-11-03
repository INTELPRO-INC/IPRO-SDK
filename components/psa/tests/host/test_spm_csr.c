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
 * Host test of spm_csr.h: field decode of real Zicsr encodings and
 * the read-form test the U-mode CSR emulator applies.
 */
#include <stdint.h>

#include "check.h"
#include "spm_csr.h"

#define CSR_CYCLE    0xC00u
#define CSR_TIME     0xC01u
#define CSR_INSTRET  0xC02u
#define CSR_CYCLEH   0xC80u
#define CSR_TIMEH    0xC81u
#define CSR_INSTRETH 0xC82u
#define REG_A0       10u
#define REG_A1       11u

struct row {
    uint32_t insn;
    const char *text;
    uint32_t csr;
    uint32_t rd;
    uint32_t rs1;
    uint32_t op;
    uint32_t imm;
    int is_read;
};

/* encodings from riscv64-unknown-elf-as, -M no-aliases listing */
static const struct row rows[] = {
    { 0xc80025f3u, "rdcycleh a1", CSR_CYCLEH, REG_A1, 0, SPM_CSR_RS, 0, 1 },
    { 0xc00025f3u, "rdcycle a1", CSR_CYCLE, REG_A1, 0, SPM_CSR_RS, 0, 1 },
    { 0xc01025f3u, "rdtime a1", CSR_TIME, REG_A1, 0, SPM_CSR_RS, 0, 1 },
    { 0xc0102573u, "rdtime a0", CSR_TIME, REG_A0, 0, SPM_CSR_RS, 0, 1 },
    { 0xc8102573u, "rdtimeh a0", CSR_TIMEH, REG_A0, 0, SPM_CSR_RS, 0, 1 },
    { 0xc0202573u, "rdinstret a0", CSR_INSTRET, REG_A0, 0, SPM_CSR_RS, 0,
      1 },
    { 0xc8202573u, "rdinstreth a0", CSR_INSTRETH, REG_A0, 0, SPM_CSR_RS,
      0, 1 },
    { 0xc0003573u, "csrrc a0, cycle, x0", CSR_CYCLE, REG_A0, 0, SPM_CSR_RC,
      0, 1 },
    { 0xc0006573u, "csrrsi a0, cycle, 0", CSR_CYCLE, REG_A0, 0, SPM_CSR_RS,
      1, 1 },
    { 0xc0002073u, "csrrs x0, cycle, x0", CSR_CYCLE, 0, 0, SPM_CSR_RS, 0,
      1 },
    { 0xc0051073u, "csrw cycle, a0", CSR_CYCLE, 0, REG_A0, SPM_CSR_RW, 0,
      0 },
    { 0xc0001573u, "csrrw a0, cycle, x0", CSR_CYCLE, REG_A0, 0, SPM_CSR_RW,
      0, 0 },
    { 0xc000e573u, "csrrsi a0, cycle, 1", CSR_CYCLE, REG_A0, 1, SPM_CSR_RS,
      1, 0 },
    { 0xc005a573u, "csrrs a0, cycle, a1", CSR_CYCLE, REG_A0, REG_A1,
      SPM_CSR_RS, 0, 0 },
};

/* not Zicsr: ecall, mret, wfi, addi x0, x0, 0 */
static const uint32_t not_csr[] = {
    0x00000073u, 0x30200073u, 0x10500073u, 0x00000013u,
};

static void test_rows(void)
{
    char what[96];

    for (uint32_t i = 0; i < sizeof(rows) / sizeof(rows[0]); i++) {
        const struct row *r = &rows[i];
        struct spm_csr_insn d;
        int ok = spm_csr_decode(r->insn, &d) == 0 &&
                 d.csr == r->csr && d.rd == r->rd && d.rs1 == r->rs1 &&
                 d.op == r->op && d.imm == r->imm;

        snprintf(what, sizeof(what), "%08x %s: fields", r->insn, r->text);
        check(ok, what);
        snprintf(what, sizeof(what), "%08x %s: %s", r->insn, r->text,
                 r->is_read ? "read form, emulated" : "writes, not emulated");
        check(ok && spm_csr_is_read(&d) == r->is_read, what);
    }
}

static void test_not_csr(void)
{
    char what[64];

    for (uint32_t i = 0; i < sizeof(not_csr) / sizeof(not_csr[0]); i++) {
        struct spm_csr_insn d;

        snprintf(what, sizeof(what), "%08x: not a Zicsr instruction",
                 not_csr[i]);
        check(spm_csr_decode(not_csr[i], &d) != 0, what);
    }
}

int main(void)
{
    test_rows();
    test_not_csr();
    return check_summary();
}
