/*
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/* Please check the TODO items */

#include "riscv_encoding.h"

/* If BOOT_HARTID is not defined, default value is 0 */
    .equ BOOT_HARTID,    0

.macro DECLARE_INT_HANDLER  INT_HDL_NAME
#if defined(__riscv_xlen) && (__riscv_xlen == 32)
    .word \INT_HDL_NAME
#else
    .dword \INT_HDL_NAME
#endif
.endm

    .section .text.init
    .globl _start
    .type _start, @function

/**
 * Reset Handler called on controller reset
 */
_start:
    /* ===== Startup Stage 1 ===== */
    /* Disable Global Interrupt */
    csrc CSR_MSTATUS, MSTATUS_MIE

    /* If SMP_CPU_CNT is not defined,
     * assume that only 1 core is allowed to run,
     * the core hartid is defined via BOOT_HARTID.
     * other harts if run to here, just do wfi in __amp_wait
     */

    /* take bit 0-7 for hart id in a local cluster */
    csrr a0, CSR_MHARTID
    andi a0, a0, 0xFF
    /* BOOT_HARTID is configurable in Makefile via BOOT_HARTID variable */
    li a1, BOOT_HARTID
    bne a0, a1, __amp_wait

    /* Initialize GP and TP and jump table base when zcmt enabled */
    .option push
    .option norelax
    la gp, __global_pointer$
    la tp, __tls_base
#if defined(__riscv_zcmt)
    la t0, __jvt_base$
    csrw CSR_JVT, t0
#endif
    .option pop

    /* Set correct sp for current cpu */
    la sp, _sp

    /*
     * Set the the NMI base mnvec to share
     * with mtvec by setting CSR_MMISC_CTL
     * bit 9 NMI_CAUSE_FFF to 1
     */
    li t0, MMISC_CTL_NMI_CAUSE_FFF
    csrs CSR_MMISC_CTL, t0

    /*
     * Enable Zc feature when compiled zcmp & zcmt
     */
    li t0, MMISC_CTL_ZC
#if defined(__riscv_zcmp) || defined(__riscv_zcmt)
    csrs CSR_MMISC_CTL, t0
#else
    csrc CSR_MMISC_CTL, t0
#endif

    /*
     * Set Exception Entry MTVEC to early_exc_entry
     * Due to settings above, Exception and NMI
     * will share common entry.
     * This early_exc_entry is only used during early
     * boot stage before main
     * Set default interrupt mode to CLINT interrupt mode
     */
    la t0, early_exc_entry
    csrw CSR_MTVEC, t0

    /* unlock opi, set 0x30000070[1] = 0*/
    lui a5,0x30000
    lw  a4,112(a5)
    and a4,a4,-3
    sw  a4,112(a5)

#ifdef CONFIG_AI_SUPPORT
    /* set 0x3000008c[8] = 1 (required for NPU) */
    lw  a4,140(a5)
    ori a4,a4,0x100
    sw  a4,140(a5)
#endif

    /* ===== Startup Stage 2 ===== */

    /* Enable FPU and Vector Unit if f/d/v exist in march */
#if defined(__riscv_flen) && __riscv_flen > 0
    /* Enable FPU, and set state to initial */
    li t0, MSTATUS_FS
    csrc mstatus, t0
    li t0, MSTATUS_FS_INITIAL
    csrs mstatus, t0
#endif

    /* Enable mcycle and minstret counter */
    csrci CSR_MCOUNTINHIBIT, 0x5

    .size _start, . - _start

.type __init_common, @function
__init_common:
    /* ===== Startup Stage 3 ===== */

    /*
     * Save boot2 parameters BEFORE LMA to VMA copy
     * Boot2 passes parameters in OCRAM which may be overwritten
     * during the data section copy below.
     */
    /*call Boot2_Param_Save*/

    /*
     * Load text section from CODE ROM to CODE RAM
     * when text LMA is different with VMA
     */
    la a0, _text_lma
    la a1, _text
    /* If text LMA and VMA are equal
     * then no need to copy text section */
    beq a0, a1, 2f
    la a2, _etext
    bgeu a1, a2, 2f

1:
    /* Load code section if necessary */
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, 1b
2:
    /* Load data section */
    la a0, _data_lma
    la a1, _data
    /* If data vma=lma, no need to copy */
    beq a0, a1, 2f
    la a2, _edata
    bgeu a1, a2, 2f
1:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, 1b
2:
    /*
     * Synchronise the instruction stream with the two copies above.
     *
     * Both loops write CODE through the DATA path: .text.ram obviously, and
     * .data too, because .tcm_code lands inside it (the HBN/PDS/arch_* helpers
     * all execute from RAM). RISC-V does not guarantee an instruction fetch
     * sees a preceding store to the same address until a fence.i, so without
     * this the core may execute whatever the fetch path was still holding for
     * those addresses.
     *
     * A cold POR hides the omission -- nothing stale to hold. A PDS31 wake does
     * not: this RAM is RETAINED across the sleep, so the addresses were already
     * populated by the previous image. Symptom when it bites is an early jump to
     * a junk address and a park in early_exc_entry with no output at all, while
     * JTAG (which reads over the bus, not the fetch path) shows the copied bytes
     * as perfectly correct -- that contradiction is the fingerprint.
     */
    fence.i

    /* Clear bss section */
    la a0, __bss_start
    la a1, _end
    bgeu a0, a1, 2f
1:
    sw zero, (a0)
    addi a0, a0, 4
    bltu a0, a1, 1b
2:

    .size __init_common, . - __init_common

.globl _start_premain
.type _start_premain, @function
_start_premain:
    /*
     * Call vendor defined SystemInit to
     * initialize the micro-controller system
     * SystemInit will just be called by boot cpu
     */
    call SystemInit

    /*
     * Call C/C++ constructor start up code,
     * __libc_fini is defined in linker script,
     * so register_fini function will be called
     * and will run atexit (__libc_fini_array)
     * to do previous call atexit function
     */
    call __libc_init_array

    .size _start_premain, . - _start_premain

.type __skip_init, @function
__skip_init:

    /* do pre-init steps before main */
    /* _premain_init will be called by each cpu
     * please make sure the implementation of __premain_int
     * considered this
     * it will update mtvec according to eclic present or not
     * mtvec will set to exc_entry
     */
    call _premain_init

    /* BPU cold bringup need time, so enable BPU before enter to main */
    li t0, MMISC_CTL_BPU
    csrs CSR_MMISC_CTL, t0

    // Interrupt is still disabled here
    /* ===== Call SMP Main Function  ===== */
    /* argc = argv = 0 */
    li a0, 0
    li a1, 0

    call main

    /* do post-main steps after main
     * this function will be called by each cpu */
    call _postmain_fini

    .size __skip_init, . - __skip_init

.type __amp_wait, @function
__amp_wait:
1:
    wfi
    j 1b

    .size __amp_wait, . - __amp_wait

/* Early boot exception entry before main */
.align 6
.global early_exc_entry
.type early_exc_entry, @function
early_exc_entry:
    wfi
    j early_exc_entry

    .size early_exc_entry, . - early_exc_entry
