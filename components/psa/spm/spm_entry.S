/*
 * SPE trap entry, the return to U-mode and the stack fill. exc_entry
 * takes every exception and, through irq_entry, every non-vectored
 * interrupt.
 */
#include "riscv_encoding.h"
#include "psa_ipro/abi.h"
#include "spm_stack.h"

#if defined(__riscv_flen) && __riscv_flen != 32
#error "struct psa_nspe_frame holds 32-bit f registers"
#endif

/* struct psa_nspe_frame byte offsets; xN is at 4 * N */
#define FR_EPC      0
#define FR_X(n)     (4 * (n))
#define FR_FSTATE   (4 * 32)
#define FR_FCSR     (4 * 33)
#define FR_F(n)     (4 * (34 + (n)))

.if FR_F(32) != 4 * PSA_NSPE_FRAME_WORDS
.error "frame offsets do not match struct psa_nspe_frame"
.endif

/* M-mode trap frame: intexc_ipro7.S SAVE_CONTEXT slots, 96 B */
#define MT_BYTES    (4 * 24)
#define MT_MCAUSE   (4 * 13)
#define MT_MEPC     (4 * 14)
#define MT_MSUBM    (4 * 15)

/* Nuclei mcause.MPIL, previous interrupt level */
#define MCAUSE_MPIL 0x00FF0000

/* Caller-saved registers in their SAVE_CONTEXT slots */
.macro MT_REGS op
    \op x1, 4 * 0(sp)
    \op x4, 4 * 1(sp)
    \op x5, 4 * 2(sp)
    \op x6, 4 * 3(sp)
    \op x7, 4 * 4(sp)
    \op x8, 4 * 5(sp)
    \op x9, 4 * 6(sp)
    \op x10, 4 * 7(sp)
    \op x11, 4 * 8(sp)
    \op x12, 4 * 9(sp)
    \op x13, 4 * 10(sp)
    \op x14, 4 * 11(sp)
    \op x15, 4 * 12(sp)
    \op x16, 4 * 16(sp)
    \op x17, 4 * 17(sp)
    \op x28, 4 * 18(sp)
    \op x29, 4 * 19(sp)
    \op x30, 4 * 20(sp)
    \op x31, 4 * 21(sp)
.endm

/* Linker relaxation off: no gp-relative access in this file */
    .option norelax

    .section .text.trap, "ax", @progbits
    .align 6
    .globl exc_entry
    .type exc_entry, @function
exc_entry:
    /* swaps with mscratch (SPE stack top) only when MPP != M */
    csrrw sp, CSR_MSCRATCHCSW, sp
    addi sp, sp, -16
    sw t0, 0(sp)
    /* previous privilege: mstatus.MPP, mirrored in mcause[29:28] */
    csrr t0, CSR_MSTATUS
    srli t0, t0, 11
    andi t0, t0, 3
    bnez t0, spm_trap_from_m

    la t0, spm_uframe
    .irp n, 1, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, \
            19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    sw x\n, FR_X(\n)(t0)
    .endr
    lw t1, 0(sp)
    sw t1, FR_X(5)(t0)
    addi sp, sp, 16
    csrr t1, CSR_MSCRATCH
    sw t1, FR_X(2)(t0)
    csrr t1, CSR_MEPC
    sw t1, FR_EPC(t0)
    li t2, 0
#if defined(__riscv_flen)
    /* f0..f31 + fcsr saved when FS is Clean or Dirty */
    csrr t1, CSR_MSTATUS
    li t3, MSTATUS_FS_CLEAN
    and t1, t1, t3
    beqz t1, 1f
    .irp n, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, \
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    fsw f\n, FR_F(\n)(t0)
    .endr
    frcsr t1
    sw t1, FR_FCSR(t0)
    li t2, 1
1:
#endif
    sw t2, FR_FSTATE(t0)
    la gp, __global_pointer$
    csrr a0, CSR_MCAUSE
    mv a1, t0
    call spm_trap_c
    /* spm_trap_c leaves through spm_mret_to_u; a return halts */
    j spm_fatal

spm_trap_from_m:
    lw t0, 0(sp)
    addi sp, sp, 16 - MT_BYTES
    MT_REGS sw
    csrr t0, CSR_MCAUSE
    sw t0, MT_MCAUSE(sp)
    csrr t0, CSR_MEPC
    sw t0, MT_MEPC(sp)
    csrr t0, CSR_MSUBM
    sw t0, MT_MSUBM(sp)
    csrr a0, CSR_MCAUSE
    mv a1, sp
    call spm_mtrap_c
    MT_REGS lw
    addi sp, sp, MT_BYTES
    mret
    .size exc_entry, . - exc_entry

    .align 2
    .globl irq_entry
    .type irq_entry, @function
irq_entry:
    j exc_entry
    .size irq_entry, . - irq_entry

    .section .text.spm_mret_to_u, "ax", @progbits
    .align 2
    .globl spm_mret_to_u
    .type spm_mret_to_u, @function
spm_mret_to_u:
    csrci CSR_MSTATUS, MSTATUS_MIE
    mv t0, a0
    lw t1, FR_EPC(t0)
    csrw CSR_MEPC, t1
    /* t3: mstatus.FS for U */
    li t3, MSTATUS_FS_INITIAL
#if defined(__riscv_flen)
    li t1, MSTATUS_FS
    csrs CSR_MSTATUS, t1
    lw t1, FR_FSTATE(t0)
    beqz t1, 1f
    /* live FP frame: load it, FS = Clean */
    .irp n, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, \
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    flw f\n, FR_F(\n)(t0)
    .endr
    lw t1, FR_FCSR(t0)
    fscsr t1
    li t3, MSTATUS_FS_CLEAN
    j 2f
1:
    /* no FP frame: f0..f31 and fcsr zeroed, FS = Initial */
    .irp n, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, \
            17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    fmv.w.x f\n, zero
    .endr
    fscsr zero
2:
#endif
    li t1, MSTATUS_MPP | MSTATUS_FS
    csrc CSR_MSTATUS, t1
    li t1, MSTATUS_MPIE
    or t1, t1, t3
    csrs CSR_MSTATUS, t1
    li t1, MCAUSE_MPIL
    csrc CSR_MCAUSE, t1
    csrw CSR_MSUBM, zero
    la t1, __StackTop
    csrw CSR_MSCRATCH, t1
    .irp n, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, \
            18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    lw x\n, FR_X(\n)(t0)
    .endr
    lw t0, FR_X(5)(t0)
    mret
    .size spm_mret_to_u, . - spm_mret_to_u

    .section .text.spm_stack_fill, "ax", @progbits
    .align 2
    .globl spm_stack_fill
    .type spm_stack_fill, @function
spm_stack_fill:
    la t0, __StackLimit
    li t1, SPM_STACK_FILL
1:
    bgeu t0, sp, 2f
    sw t1, 0(t0)
    addi t0, t0, 4
    j 1b
2:
    ret
    .size spm_stack_fill, . - spm_stack_fill
