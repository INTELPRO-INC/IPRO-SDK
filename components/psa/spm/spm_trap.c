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
 * Trap dispatch for the NSPE: hardware interrupts become vIRQs,
 * U ecalls are served, counter and M-only CSR reads, the MIE toggle
 * and word accesses to granted registers are emulated, every other
 * U exception is VIRQ_FAULT.
 */
#include <stddef.h>
#include <string.h>

#include "spm.h"
#include "spm_csr.h"
#include "spm_ldst.h"
#include "spm_log.h"
#include "spm_nspe.h"
#include "spm_periph.h"
#include "spm_services.h"
#include "spm_virq.h"
#include "system_soc.h"

#define SPM_MCAUSE_INTR   0x80000000u
#define SPM_MCAUSE_CODE   0x00000FFFu
#define SPM_LOG_MAX       128u
/* PMP access faults 1, 5, 7 and sPMP page faults 12, 13, 15 */
#define SPM_FAULT_CODES   ((1u << CAUSE_FAULT_FETCH) |          \
                           (1u << CAUSE_FAULT_LOAD) |           \
                           (1u << CAUSE_FAULT_STORE) |          \
                           (1u << CAUSE_FETCH_PAGE_FAULT) |     \
                           (1u << CAUSE_LOAD_PAGE_FAULT) |      \
                           (1u << CAUSE_STORE_PAGE_FAULT))
#define SPM_STORE_CODES   ((1u << CAUSE_FAULT_STORE) |          \
                           (1u << CAUSE_STORE_PAGE_FAULT))
#define SPM_LOAD_CODES    ((1u << CAUSE_FAULT_LOAD) |           \
                           (1u << CAUSE_LOAD_PAGE_FAULT))

_Static_assert(offsetof(struct psa_nspe_frame, x) == 4 * 1, "x1");
_Static_assert(offsetof(struct psa_nspe_frame, fstate) == 4 * 32, "fstate");
_Static_assert(offsetof(struct psa_nspe_frame, fcsr) == 4 * 33, "fcsr");
_Static_assert(offsetof(struct psa_nspe_frame, f) == 4 * 34, "f0");

struct psa_nspe_frame spm_uframe __attribute__((aligned(16)));

void spm_fatal(void)
{
    __disable_irq();
    SPM_LOG("halted");
    for (;;) {
    }
}

/* 1 for a PMP access fault or sPMP page fault exception code */
static int spm_access_fault(uint32_t code)
{
    return code < 32u && ((SPM_FAULT_CODES >> code) & 1u) != 0;
}

/* Copy f below the NSPE sp and enter the NSPE trap entry. */
static int spm_push_frame(struct psa_nspe_frame *f, uint32_t id)
{
    uint32_t dst = (f->x[SPM_X_SP] - PSA_NSPE_FRAME_SIZE) & ~15u;

    if (!spm_nspe_range_ok(dst, PSA_NSPE_FRAME_SIZE, 1)) {
        return -1;
    }
    memcpy((void *)(uintptr_t)dst, f, sizeof(*f));
    f->x[SPM_X_A0] = id;
    f->x[SPM_X_A1] = dst;
    f->x[SPM_X_SP] = dst;
    f->epc = spm_virq_entry();
    spm_virq_set_mask(1);
    return 0;
}

void spm_return_to_nspe(struct psa_nspe_frame *f)
{
    int id = spm_virq_next();

    if (id >= 0) {
        spm_virq_take(id);
        if (spm_push_frame(f, (uint32_t)id) != 0) {
            SPM_LOG("NSPE sp %08lx unusable",
                    (unsigned long)f->x[SPM_X_SP]);
            spm_nspe_reset();
        }
    }
    spm_mret_to_u(f);
}

static void spm_fault(struct psa_nspe_frame *f, uint32_t code)
    __attribute__((noreturn));

static void spm_fault(struct psa_nspe_frame *f, uint32_t code)
{
    uint32_t mtval = (uint32_t)__RV_CSR_READ(CSR_MTVAL);

    SPM_LOG("NSPE fault mcause=%lu mdcause=%lu mepc=%08lx mtval=%08lx",
            (unsigned long)code,
            (unsigned long)__RV_CSR_READ(CSR_MDCAUSE),
            (unsigned long)f->epc, (unsigned long)mtval);
    if (spm_virq_fault(code, mtval, f->epc) != 0 ||
        spm_push_frame(f, VIRQ_FAULT) != 0) {
        spm_nspe_reset();
    }
    spm_return_to_nspe(f);
}

static uint32_t spm_reg(const struct psa_nspe_frame *f, uint32_t r)
{
    return r == 0 ? 0 : f->x[r - 1u];
}

static void spm_set_reg(struct psa_nspe_frame *f, uint32_t r, uint32_t v)
{
    if (r != 0) {
        f->x[r - 1u] = v;
    }
}

/* Virtual mstatus: only MIE, the inverse of the vIRQ mask. */
static void spm_mstatus_write(uint32_t op, uint32_t src)
{
    if (op == 1u) {
        spm_virq_set_mask((src & MSTATUS_MIE) == 0);
    } else if (src & MSTATUS_MIE) {
        spm_virq_set_mask(op == 3u);
    }
}

/*
 * U-visible value of an emulated CSR: cycle, time and instret come
 * from mcycle, SysTimer mtime and minstret; -1 when not emulated.
 */
static int spm_csr_value(uint32_t csr, uint32_t *v)
{
    switch (csr) {
    case CSR_CYCLE:
    case CSR_MCYCLE:
        *v = (uint32_t)__RV_CSR_READ(CSR_MCYCLE);
        break;
    case CSR_CYCLEH:
    case CSR_MCYCLEH:
        *v = (uint32_t)__RV_CSR_READ(CSR_MCYCLEH);
        break;
    case CSR_TIME:
        *v = (uint32_t)SysTimer_GetLoadValue();
        break;
    case CSR_TIMEH:
        *v = (uint32_t)(SysTimer_GetLoadValue() >> 32);
        break;
    case CSR_INSTRET:
        *v = (uint32_t)__RV_CSR_READ(CSR_MINSTRET);
        break;
    case CSR_INSTRETH:
        *v = (uint32_t)__RV_CSR_READ(CSR_MINSTRETH);
        break;
    case CSR_MHARTID:
        *v = (uint32_t)__RV_CSR_READ(CSR_MHARTID);
        break;
    case CSR_MSTATUS:
        *v = spm_virq_masked() ? 0 : MSTATUS_MIE;
        break;
    default:
        return -1;
    }
    return 0;
}

/*
 * 0 when insn is an emulated CSR access, applied to f: read forms of
 * every emulated CSR, writes of mstatus.MIE only.
 */
static int spm_emulate_csr(struct psa_nspe_frame *f, uint32_t insn)
{
    struct spm_csr_insn d;
    uint32_t src;
    uint32_t old;
    int writes;

    if (spm_csr_decode(insn, &d) != 0 || spm_csr_value(d.csr, &old) != 0) {
        return -1;
    }
    src = d.imm ? d.rs1 : spm_reg(f, d.rs1);
    writes = !spm_csr_is_read(&d);
    if (writes && (d.csr != CSR_MSTATUS || (src & ~MSTATUS_MIE) != 0)) {
        return -1;
    }
    spm_set_reg(f, d.rd, old);
    if (writes) {
        spm_mstatus_write(d.op, src);
    }
    return 0;
}

static void spm_emulate(struct psa_nspe_frame *f) __attribute__((noreturn));

static void spm_emulate(struct psa_nspe_frame *f)
{
    struct psa_nspe_ctrl *c = spm_virq_ctrl();
    uint32_t insn;

    if (spm_copy_in(&insn, f->epc, sizeof(insn)) != 0 ||
        spm_emulate_csr(f, insn) != 0) {
        spm_fault(f, CAUSE_ILLEGAL_INSTRUCTION);
    }
    f->epc += 4u;
    if (c != NULL) {
        c->stat_emul++;
    }
    spm_return_to_nspe(f);
}

/* The instruction at epc, 16 or 32 bits; -1 outside NSPE memory */
static int spm_fetch(uint32_t epc, uint32_t *insn)
{
    uint16_t lo;
    uint16_t hi = 0;

    if (spm_copy_in(&lo, epc, sizeof(lo)) != 0 ||
        ((lo & 3u) == 3u && spm_copy_in(&hi, epc + 2u, sizeof(hi)) != 0)) {
        return -1;
    }
    *insn = (uint32_t)lo | ((uint32_t)hi << 16);
    return 0;
}

/*
 * 0 when a load/store fault at mtval is a word lw/sw on a register of
 * spm_reg_grants[], done here: a store changes only wmask bits, fmask
 * bits become fval.
 */
static int spm_emulate_grant(struct psa_nspe_frame *f, uint32_t code)
{
    struct psa_nspe_ctrl *c = spm_virq_ctrl();
    const struct spm_reg_grant *g;
    struct spm_ldst_insn d;
    volatile uint32_t *reg;
    uint32_t insn;
    uint32_t store;
    uint32_t addr;

    if (code >= 32u ||
        (((SPM_STORE_CODES | SPM_LOAD_CODES) >> code) & 1u) == 0 ||
        spm_fetch(f->epc, &insn) != 0 || spm_ldst_decode(insn, &d) != 0) {
        return -1;
    }
    store = (SPM_STORE_CODES >> code) & 1u;
    addr = spm_reg(f, d.rs1) + (uint32_t)d.imm;
    g = spm_reg_grant_find(addr);
    if (g == NULL || d.store != store ||
        addr != (uint32_t)__RV_CSR_READ(CSR_MTVAL)) {
        return -1;
    }
    reg = (volatile uint32_t *)(uintptr_t)g->addr;
    if (store) {
        uint32_t v = (*reg & ~g->wmask) | (spm_reg(f, d.reg) & g->wmask);

        *reg = (v & ~g->fmask) | g->fval;
    } else {
        spm_set_reg(f, d.reg, *reg);
    }
    f->epc += d.len;
    if (c != NULL) {
        c->stat_emul++;
    }
    return 0;
}

static void spm_irq_return(uint32_t frame, uint32_t irq)
    __attribute__((noreturn));

static void spm_irq_return(uint32_t frame, uint32_t irq)
{
    if ((frame & 3u) != 0 ||
        !spm_nspe_range_ok(frame, PSA_NSPE_FRAME_SIZE, 1)) {
        SPM_LOG("NSPE frame %08lx refused", (unsigned long)frame);
        spm_nspe_reset();
    }
    memcpy(&spm_uframe, (const void *)(uintptr_t)frame, sizeof(spm_uframe));
    spm_virq_irq_return(irq);
    spm_return_to_nspe(&spm_uframe);
}

static uint32_t spm_nspe_init(uint32_t entry, uint32_t ctrl)
{
    if ((ctrl & 3u) != 0 ||
        !spm_nspe_range_ok(ctrl, sizeof(struct psa_nspe_ctrl), 1) ||
        !spm_nspe_entry_ok(entry)) {
        return (uint32_t)-1;
    }
    return (uint32_t)spm_virq_bind(
        (struct psa_nspe_ctrl *)(uintptr_t)ctrl, entry);
}

static uint32_t spm_nspe_log(uint32_t buf, uint32_t len)
{
    char line[SPM_LOG_MAX + 1];

    if (len > SPM_LOG_MAX) {
        len = SPM_LOG_MAX;
    }
    if (spm_copy_in(line, buf, len) != 0) {
        return (uint32_t)-1;
    }
    line[len] = '\0';
    printf("%s", line);
    return 0;
}

/*
 * Sleep unless a vIRQ is pending, masked or not. An attached source
 * that fires is taken in M-mode and marks its vIRQ pending.
 */
static void spm_wfi(void)
{
    if (spm_virq_pending_any()) {
        return;
    }
    __WFI();
    __enable_irq();
    __disable_irq();
}

/* a0 result of the ecalls that return to the caller */
static uint32_t spm_ecall_value(uint32_t id, const uint32_t *a)
{
    switch (id) {
    case PSA_ECALL_FRAMEWORK_VERSION:
        return PSA_FRAMEWORK_VERSION_VALUE;
    case PSA_ECALL_VERSION:
        return spm_psa_version(a[0]);
    case PSA_ECALL_CALL:
        return (uint32_t)spm_psa_call((psa_handle_t)a[0], (int32_t)a[1],
                                      a[2], a[3], a[4], a[5]);
    case NSPE_ECALL_INIT:
        return spm_nspe_init(a[0], a[1]);
    case NSPE_ECALL_IRQ_DELIVER:
        return 0;
    case NSPE_ECALL_IRQ_ATTACH:
        return spm_periph_irq_ok(a[0]) ? (uint32_t)spm_virq_attach(a[0])
                                       : (uint32_t)-1;
    case NSPE_ECALL_IRQ_DETACH:
        return spm_periph_irq_ok(a[0]) ? (uint32_t)spm_virq_detach(a[0])
                                       : (uint32_t)-1;
    case NSPE_ECALL_TIMER_SET:
        return (uint32_t)spm_virq_timer_set(a[0]);
    case NSPE_ECALL_YIELD:
        spm_virq_raise(VIRQ_YIELD);
        return 0;
    case NSPE_ECALL_WFI:
        spm_wfi();
        return 0;
    case NSPE_ECALL_LOG:
        return spm_nspe_log(a[0], a[1]);
    case PSA_ECALL_CONNECT:
    case PSA_ECALL_CLOSE:
    default:
        return (uint32_t)PSA_ERROR_NOT_SUPPORTED;
    }
}

static void spm_ecall(struct psa_nspe_frame *f) __attribute__((noreturn));

static void spm_ecall(struct psa_nspe_frame *f)
{
    uint32_t id = f->x[SPM_X_A7];
    uint32_t *a = &f->x[SPM_X_A0];

    f->epc += 4u;
    if (id == NSPE_ECALL_IRQ_RETURN) {
        spm_irq_return(a[0], a[1]);
    }
    if (id == NSPE_ECALL_RESET) {
        spm_nspe_reset();
    }
    a[0] = spm_ecall_value(id, a);
    spm_return_to_nspe(f);
}

/*
 * Interrupts become vIRQs, ecall (8) is served, code 2 is emulated
 * or faults, a load/store fault on a granted register is emulated;
 * any other code, PMP access faults 1/5/7 and sPMP page faults
 * 12/13/15 among them, is delivered as VIRQ_FAULT.
 */
void spm_trap_c(uint32_t mcause, struct psa_nspe_frame *f)
{
    uint32_t code = mcause & SPM_MCAUSE_CODE;
    struct psa_nspe_ctrl *c = spm_virq_ctrl();

    if (c != NULL) {
        c->stat_traps++;
    }
    if (mcause & SPM_MCAUSE_INTR) {
        spm_virq_hw_irq(code);
        spm_return_to_nspe(f);
    } else if (code == CAUSE_USER_ECALL) {
        spm_ecall(f);
    } else if (code == CAUSE_ILLEGAL_INSTRUCTION) {
        spm_emulate(f);
    } else if (spm_emulate_grant(f, code) == 0) {
        spm_return_to_nspe(f);
    } else {
        spm_fault(f, code);
    }
}

/* SAVE_CONTEXT slot of each caller-saved register */
static const struct {
    uint8_t slot;
    char name[3];
} spm_mregs[] = {
    { 0, "ra" }, { 1, "tp" }, { 2, "t0" }, { 3, "t1" }, { 4, "t2" },
    { 5, "s0" }, { 6, "s1" }, { 7, "a0" }, { 8, "a1" }, { 9, "a2" },
    { 10, "a3" }, { 11, "a4" }, { 12, "a5" }, { 16, "a6" },
    { 17, "a7" }, { 18, "t3" }, { 19, "t4" }, { 20, "t5" },
    { 21, "t6" },
};

void spm_mtrap_c(uint32_t mcause, uint32_t *regs)
{
    uint32_t code = mcause & SPM_MCAUSE_CODE;

    if (mcause & SPM_MCAUSE_INTR) {
        spm_virq_hw_irq(code);
        return;
    }
    SPM_LOG("M-mode %s mcause=%08lx mepc=%08lx mtval=%08lx mdcause=%lx",
            spm_access_fault(code) ? "fault" : "trap",
            (unsigned long)mcause, (unsigned long)regs[14],
            (unsigned long)__RV_CSR_READ(CSR_MTVAL),
            (unsigned long)__RV_CSR_READ(CSR_MDCAUSE));
    for (uint32_t i = 0; i < sizeof(spm_mregs) / sizeof(spm_mregs[0]); i++) {
        printf("%s=%08lx%s", spm_mregs[i].name,
               (unsigned long)regs[spm_mregs[i].slot],
               (i % 5u) == 4u ? "\r\n" : " ");
    }
    printf("\r\n");
    spm_fatal();
}
