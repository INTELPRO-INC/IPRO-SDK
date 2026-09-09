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

/* Set by the LP path; read by the exception dump below. Defined here because
 * this file is in every build, WiFi or not. */
volatile const char *g_lp_phase = "boot";

#include <generated/autoconf.h>
#include <stdint.h>
#include <stdio.h>
#include "drv_common.h"
#include "drv_glb.h"
#include "platform_common.h"
#include "core_feature_eclic.h"
#include "hal_sys.h"
#include "hal_hbnram_mgnt.h"

/*
 * Layout of the frame that exc_entry pushes.
 *
 * MUST match the producer, and the producer is NOT the exc_entry in
 * startup/GCC/intexc_ipro6.S -- that one is `.weak` and is overridden at link
 * time by the strong definition FreeRTOS ships in
 * components/os/freertos/Source/portable_ipro7/GCC/portasm.S (used by both
 * chips). The two layouts differ, and decoding one with the other silently
 * mislabels every register in every crash dump this port has ever produced:
 * with the old struct, `s0`/`s1` printed a0/a1, `a4`/`a5` printed mcause/mepc,
 * and the `cause`/`epc` line printed msubm and a6.
 *
 * The FreeRTOS frame is 20 words and saves ONLY caller-saved registers --
 * there is no s0 or s1 in it, which is why the backtracer below cannot walk a
 * frame-pointer chain. Word offsets, verified against the linked binary
 * (`objdump -d` of exc_entry):
 *   0 ra   1 tp   2 t0   3 t1   4 t2   5..10 a0-a5
 *   11 mcause  12 mepc  13 msubm      (SAVE_CSR_CONTEXT pushes to 11/12/13)
 *   14 a6  15 a7  16 t3  17 t4  18 t5  19 t6
 * If exc_entry is ever changed, re-check this struct the same way.
 */
typedef struct EXC_Frame {
    unsigned long ra;                /* ra: x1, return address for jump */
    unsigned long tp;                /* tp: x4, thread pointer */
    unsigned long t0;                /* t0: x5, temporary register 0 */
    unsigned long t1;                /* t1: x6, temporary register 1 */
    unsigned long t2;                /* t2: x7, temporary register 2 */
    unsigned long a0;                /* a0: x10, return value or function argument 0 */
    unsigned long a1;                /* a1: x11, return value or function argument 1 */
    unsigned long a2;                /* a2: x12, function argument 2 */
    unsigned long a3;                /* a3: x13, function argument 3 */
    unsigned long a4;                /* a4: x14, function argument 4 */
    unsigned long a5;                /* a5: x15, function argument 5 */
    unsigned long cause;             /* cause: machine/supervisor mode cause csr register */
    unsigned long epc;               /* epc: machine/ supervisor mode exception program counter csr register */
    unsigned long msubm;             /* msubm: machine sub-mode csr register */
#ifndef __riscv_32e
    unsigned long a6;                /* a6: x16, function argument 6 */
    unsigned long a7;                /* a7: x17, function argument 7 */
    unsigned long t3;                /* t3: x28, temporary register 3 */
    unsigned long t4;                /* t4: x29, temporary register 4 */
    unsigned long t5;                /* t5: x30, temporary register 5 */
    unsigned long t6;                /* t6: x31, temporary register 6 */
#endif
} EXC_Frame_Type;

void Exception_PrintBacktrace(unsigned long sp, int limit);
void Exception_DumpFrame(unsigned long sp, uint8_t mode);

/** default eclic interrupt or exception interrupt handler */
extern void default_intexc_handler(void);
/** eclic s-mode software interrupt handler in eclic mode */
__WEAK __MACHINE_INTERRUPT void eclic_msip_handler(void)
{

}
/** eclic s-mode time interrupt handler in eclic mode */
__WEAK __MACHINE_INTERRUPT void eclic_mtip_handler(void)
{

}

static unsigned long vector_table[__ECLIC_INTNUM] __attribute__((section (".vectable"), aligned(512))) =
{
    (unsigned long)(default_intexc_handler),        /* 0: Reserved */
    (unsigned long)(default_intexc_handler),        /* 1: Reserved */
    (unsigned long)(default_intexc_handler),        /* 2: Reserved */

    (unsigned long)(eclic_msip_handler),            /* 3: supervisor software interrupt in eclic mode */

    (unsigned long)(default_intexc_handler),        /* 4: Reserved */
    (unsigned long)(default_intexc_handler),        /* 5: Reserved */
    (unsigned long)(default_intexc_handler),        /* 6: Reserved */

    (unsigned long)(eclic_mtip_handler),            /* 7: supervisor timer interrupt in eclic mode */

    (unsigned long)(default_intexc_handler),        /* 8: Reserved */
    (unsigned long)(default_intexc_handler),        /* 9: Reserved */
    (unsigned long)(default_intexc_handler),        /* 10: Reserved */
    (unsigned long)(default_intexc_handler),        /* 11: Reserved */

    (unsigned long)(default_intexc_handler),        /* 12: Reserved */
    (unsigned long)(default_intexc_handler),        /* 13: Reserved */
    (unsigned long)(default_intexc_handler),        /* 14: Reserved */
    (unsigned long)(default_intexc_handler),        /* 15: Reserved */

    (unsigned long)(default_intexc_handler),        /* 16: Reserved */
    (unsigned long)(default_intexc_handler),        /* 17: Reserved */
    (unsigned long)(default_intexc_handler),        /* 18: Reserved */
};

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)

/* for the following variables, see intexc_evalsoc.S and intexc_evalsoc_s.S */
/** default entry for s-mode non-vector irq entry */
extern void irq_entry_s(void);
/** default entry for s-mode exception entry */
extern void exc_entry_s(void);

#ifndef __ICCRISCV__
/** eclic s-mode software interrupt handler in eclic mode */
extern void eclic_ssip_handler(void) __WEAK;
/** eclic s-mode time interrupt handler in eclic mode */
extern void eclic_stip_handler(void) __WEAK;
#else
/** eclic s-mode software interrupt handler in eclic mode */
__WEAK __SUPERVISOR_INTERRUPT void eclic_ssip_handler(void)
{

}
/** eclic s-mode time interrupt handler in eclic mode */
__WEAK __SUPERVISOR_INTERRUPT __WEAK void eclic_stip_handler(void)
{

}
#endif

/* default s-mode exception handler, which user can modify it at your need */
static void system_default_exception_handler_s(unsigned long scause, unsigned long sp);

#ifndef __ICCRISCV__
#define __SMODE_VECTOR_ATTR   __attribute__((section (".text.vtable_s"), aligned(512)))
#else
#define __SMODE_VECTOR_ATTR   __attribute__((section (".sintvec"), aligned(512)))
#endif
// TODO: change the aligned(512) to match stvt alignment requirement according to your eclic max interrupt number
// TODO: place your interrupt handler into this vector table, important if your vector table is in flash
/**
 * \var unsigned long vector_table_s[__ECLIC_INTNUM]
 * \brief vector interrupt storing ISRs for supervisor mode
 * \details
 *  vector_table_s is hold by stvt register, the address must align according
 *  to actual interrupt numbers as below, now align to 512 bytes considering we put up to 128 interrupts here
 *  alignment must comply to table below if you increase or decrease vector interrupt number
 *  interrupt number      alignment
 *    0 to 16               64-byte
 *    17 to 32              128-byte
 *    33 to 64              256-byte
 *    65 to 128             512-byte
 *    129 to 256              1KB
 *    257 to 512              2KB
 *    513 to 1024             4KB
 */
const unsigned long vector_table_s[__ECLIC_INTNUM] __SMODE_VECTOR_ATTR =
{
    (unsigned long)(default_intexc_handler),        /* 0: Reserved */
    (unsigned long)(default_intexc_handler),        /* 1: Reserved */
    (unsigned long)(default_intexc_handler),        /* 2: Reserved */

    (unsigned long)(eclic_ssip_handler),            /* 3: supervisor software interrupt in eclic mode */

    (unsigned long)(default_intexc_handler),        /* 4: Reserved */
    (unsigned long)(default_intexc_handler),        /* 5: Reserved */
    (unsigned long)(default_intexc_handler),        /* 6: Reserved */

    (unsigned long)(eclic_stip_handler),            /* 7: supervisor timer interrupt in eclic mode */

    (unsigned long)(default_intexc_handler),        /* 8: Reserved */
    (unsigned long)(default_intexc_handler),        /* 9: Reserved */
    (unsigned long)(default_intexc_handler),        /* 10: Reserved */
    (unsigned long)(default_intexc_handler),        /* 11: Reserved */

    (unsigned long)(default_intexc_handler),        /* 12: Reserved */
    (unsigned long)(default_intexc_handler),        /* 13: Reserved */
    (unsigned long)(default_intexc_handler),        /* 14: Reserved */
    (unsigned long)(default_intexc_handler),        /* 15: Reserved */

    (unsigned long)(default_intexc_handler),        /* 16: Reserved */
    (unsigned long)(default_intexc_handler),        /* 17: Reserved */
    (unsigned long)(default_intexc_handler),        /* 18: Reserved */
/* TODO other external interrupt handler don't provide default value, if you want to provide default value, please do it by yourself */
};
#endif

#define MAX_SYSTEM_EXCEPTION_NUM        26
static unsigned long SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM + 1];

typedef void (*EXC_HANDLER)(unsigned long cause, unsigned long sp);
typedef void (*INT_HANDLER)(unsigned long cause, unsigned long sp);

#define SYSTEM_CORE_INTNUM      12 // 0-11 stop at machine external interrupt
static unsigned long SystemCoreInterruptHandlers[SYSTEM_CORE_INTNUM];

static void core_interrupt_handler(unsigned long exccode, unsigned long sp);

uint32_t core_exception_handler(unsigned long mcause, unsigned long sp);
static INT_HANDLER system_core_interrupt_handler = NULL;

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
static unsigned long SystemExceptionHandlers_S[MAX_SYSTEM_EXCEPTION_NUM];
#endif

/**
 * \brief      System Default Exception Handler
 * \details
 * This function provides a default exception and NMI handler for all exception ids.
 * By default, It will just print some information for debug, Vendor can customize it according to its requirements.
 * \param [in]  mcause    code indicating the reason that caused the trap in machine mode
 * \param [in]  sp        stack pointer
 */
static void system_default_exception_handler(unsigned long mcause, unsigned long sp)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;

    /* Park the fault identity in the uncached retained-OCRAM debug page
     * FIRST: the printf dump below is silently DROPPED whenever the fault
     * preempted a context that was inside vprint (its `busy` reentrancy
     * flag stays latched), and JTAG needs a pinmux that PDS wipes — these
     * four words are then the only autopsy channel (read via CCI).
     * Layout: FFC0=magic 'EXCP', FFC4=mcause, FFC8=mepc, FFCC=mtval. */
    *(volatile unsigned long *)0x2104FFC0u = 0x45584350u;
    *(volatile unsigned long *)0x2104FFC4u = mcause;
    *(volatile unsigned long *)0x2104FFC8u = __RV_CSR_READ(CSR_MEPC);
    *(volatile unsigned long *)0x2104FFCCu = __RV_CSR_READ(CSR_MTVAL);

    /* Which phase of the PDS enter/resume sequence were we in? A Load Access
     * Fault on a WRAM descriptor says the WiFi power domain was not there; this
     * says WHO was touching it. Updated by lp_demo (power down / back) and by
     * LP_RSM_MARK at every resume stage. */
    printf("LPPHASE: %s\r\n", g_lp_phase ? g_lp_phase : "?");
    printf("=== ENHANCED EXCEPTION ANALYSIS ===\r\n");
    printf("MCAUSE : 0x%lx\r\n", mcause);
    printf("MDCAUSE: 0x%lx\r\n", __RV_CSR_READ(CSR_MDCAUSE));
    printf("MEPC   : 0x%lx\r\n", __RV_CSR_READ(CSR_MEPC));
    printf("MTVAL  : 0x%lx\r\n", __RV_CSR_READ(CSR_MTVAL));
    printf("HARTID : %u\r\n", (unsigned int)__get_hart_id());

    // 分析異常類型
    unsigned long exccode = mcause & 0xFFF;
    if (mcause & 0x80000000) {
        printf("Exception Type: INTERRUPT (code: %lu)\r\n", exccode);
    } else {
        printf("Exception Type: EXCEPTION (code: %lu - ", exccode);
        switch(exccode) {
            case 0: printf("Instruction Address Misaligned"); break;
            case 1: printf("Instruction Access Fault"); break;
            case 2: printf("Illegal Instruction"); break;
            case 3: printf("Breakpoint"); break;
            case 4: printf("Load Address Misaligned"); break;
            case 5: printf("Load Access Fault"); break;
            case 6: printf("Store/AMO Address Misaligned"); break;
            case 7: printf("Store/AMO Access Fault"); break;
            case 8: printf("Environment Call from U-mode"); break;
            case 9: printf("Environment Call from S-mode"); break;
            case 11: printf("Environment Call from M-mode"); break;
            case 12: printf("Instruction Page Fault"); break;
            case 13: printf("Load Page Fault"); break;
            case 15: printf("Store/AMO Page Fault"); break;
            default: printf("Unknown Exception"); break;
        }
        printf(")\r\n");
    }

    // 分析異常地址
    unsigned long epc = __RV_CSR_READ(CSR_MEPC);
    printf("\r\n=== ADDRESS ANALYSIS ===\r\n");
    printf("Exception PC: 0x%08lx\r\n", epc);

    // 檢查地址範圍
    if (epc >= 0x13000000 && epc < 0x13400000) {
        printf("Location: FLASH region (0x13000000-0x133fffff)\r\n");
    } else if (epc >= 0x1e400000 && epc < 0x1ec00000) {
        printf("Location: PSRAM region (0x1e400000-0x1ebfffff)\r\n");
    } else if (epc >= 0x11010000 && epc < 0x11050000) {
        printf("Location: OCRAM region (0x11010000-0x1104ffff)\r\n");
    } else if (epc >= 0x20010000 && epc < 0x20011000) {
        printf("Location: HBNRAM region (0x20010000-0x20010fff)\r\n");
    } else {
        printf("Location: INVALID/UNKNOWN region - PROBLEM DETECTED!\r\n");
        printf("This indicates memory corruption or wild pointer jump!\r\n");
    }

    // 檢查返回地址
    printf("Return Address (ra): 0x%08lx\r\n", exc_frame->ra);
    if ((exc_frame->ra >= 0x13000000 && exc_frame->ra < 0x13400000) ||
        (exc_frame->ra >= 0x11010000 && exc_frame->ra < 0x11050000)) {
        printf("Return address looks valid\r\n");
    } else {
        printf("Return address looks INVALID - stack corruption suspected!\r\n");
    }

    // 檢查棧指針
    printf("Stack Pointer (sp): 0x%08lx\r\n", sp);
    extern char __StackBottom[];
    extern char __StackTop[];
    if (sp >= (unsigned long)__StackBottom && sp <= (unsigned long)__StackTop) {
        printf("Stack pointer within valid range\r\n");
        unsigned long stack_used = (unsigned long)__StackTop - sp;
        printf("Stack usage: %lu bytes\r\n", stack_used);
    } else {
        printf("Stack pointer OUTSIDE valid range - stack corruption!\r\n");
        printf("Valid stack range: 0x%08lx - 0x%08lx\r\n",
               (unsigned long)__StackBottom, (unsigned long)__StackTop);
    }

    printf("\r\n=== MEMORY LAYOUT CHECK ===\r\n");
#ifdef CONFIG_ENABLE_NVDS
    extern uint8_t __nvds_start[];
    extern uint8_t __nvds_end[];
    printf("NVDS region: 0x%08lx - 0x%08lx (size: %lu KB)\r\n",
           (unsigned long)__nvds_start,
           (unsigned long)__nvds_end,
           ((unsigned long)__nvds_end - (unsigned long)__nvds_start) / 1024);
#else
    printf("NVDS: disabled\r\n");
#endif

    Exception_DumpFrame(sp, (uint8_t) PRV_M);

    /* Print software backtrace */
    Exception_PrintBacktrace(sp, 15);

    printf("\r\n=== RECOMMENDED ACTIONS ===\r\n");
    if (exccode == 2) {
        printf("1. Check if PC jumped to invalid memory region\r\n");
        printf("2. Verify function pointers are properly initialized\r\n");
        printf("3. Check for stack overflow\r\n");
        printf("4. Verify interrupt vector table integrity\r\n");
    }

#ifdef CONFIG_EXCEPTION_AUTO_REBOOT
    /*
     * Auto reboot on exception (optional, default off)
     * Enable with CONFIG_EXCEPTION_AUTO_REBOOT=y in proj.conf
     */
    printf("\r\n=== AUTO REBOOT ===\r\n");
    printf("CONFIG_EXCEPTION_AUTO_REBOOT enabled, rebooting...\r\n");

    /* Set exception reset reason in HBN RAM */
    hal_rst_reason_set(HAL_RST_REASON_EXCEPTION);

    /* Trigger system reset */
    GLB_SW_POR_Reset();
#endif

    while (1);
}

/**
 * \brief      System Default Interrupt Handler for CLINT/PLIC Interrupt Mode
 * \details
 * This function provided a default interrupt handling code for all interrupt ids.
 */
__attribute__((unused)) static void system_default_interrupt_handler(unsigned long mcause, unsigned long sp)
{
    printf("Trap in Interrupt\r\n");
    printf("MCAUSE: 0x%lx\r\n", mcause);
    printf("MEPC  : 0x%lx\r\n", __RV_CSR_READ(CSR_MEPC));
    printf("MTVAL : 0x%lx\r\n", __RV_CSR_READ(CSR_MBADADDR));
}
/**
 * \brief      Initialize all the default core exception handlers
 * \details
 * The core exception handler for each exception id will be initialized to \ref system_default_exception_handler.
 * \note
 * Called in \ref _init function, used to initialize default exception handlers for all exception IDs
 * SystemExceptionHandlers contains NMI, but SystemExceptionHandlers_S not, because NMI can't be delegated to S-mode.
 */
void Exception_Init(void)
{
    for (int i = 0; i < MAX_SYSTEM_EXCEPTION_NUM; i++) {
        SystemExceptionHandlers[i] = (unsigned long)system_default_exception_handler;
#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
        SystemExceptionHandlers_S[i] = (unsigned long)system_default_exception_handler_s;
#endif
    }
    SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM] = (unsigned long)system_default_exception_handler;
}

/**
 * \brief       Register an core interrupt handler for core interrupt number
 * \details
 * * For irqn <=  10, it will be registered into SystemCoreInterruptHandlers[irqn-1].
 * \param   irqn    See \ref IRQn
 * \param   int_handler     The core interrupt handler for this interrupt code irqn
 * \remarks
 *          You can only use it when you are not in ECLIC interrupt mode.
 */
void Interrupt_Register_CoreIRQ(uint32_t irqn, unsigned long int_handler)
{
    if ((irqn < SYSTEM_CORE_INTNUM) && (irqn >= 0)) {
        SystemCoreInterruptHandlers[irqn] = int_handler;
    }
}

/**
 * \brief       Get an core interrupt handler for core interrupt number
 * \param   irqn    See \ref IRQn
 * \return
 * The core interrupt handler for this interrupt code irqn
 * \remarks
 *          You can only use it when you are not in ECLIC interrupt mode.
 */
unsigned long Interrupt_Get_CoreIRQ(uint32_t irqn)
{
    if ((irqn < SYSTEM_CORE_INTNUM) && (irqn >= 0)) {
        return SystemCoreInterruptHandlers[irqn];
    }
    return 0;
}

/**
 * \brief      Dump Exception Frame
 * \details
 * This function provided feature to dump exception frame stored in stack.
 * \param [in]  sp    stackpoint
 * \param [in]  mode  privileged mode to decide whether to dump msubm CSR
 */
void Exception_DumpFrame(unsigned long sp, uint8_t mode)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;

    /* One printf per line. The old single call passed 21 varargs, which is
     * both fragile and hard to line up against the frame when something is
     * off. s0/s1 are absent because exc_entry does not save them. */
    printf("ra: 0x%lx, tp: 0x%lx, t0: 0x%lx, t1: 0x%lx, t2: 0x%lx\r\n",
           exc_frame->ra, exc_frame->tp, exc_frame->t0,
           exc_frame->t1, exc_frame->t2);
#ifndef __riscv_32e
    printf("t3: 0x%lx, t4: 0x%lx, t5: 0x%lx, t6: 0x%lx\r\n",
           exc_frame->t3, exc_frame->t4, exc_frame->t5, exc_frame->t6);
    printf("a0: 0x%lx, a1: 0x%lx, a2: 0x%lx, a3: 0x%lx\r\n",
           exc_frame->a0, exc_frame->a1, exc_frame->a2, exc_frame->a3);
    printf("a4: 0x%lx, a5: 0x%lx, a6: 0x%lx, a7: 0x%lx\r\n",
           exc_frame->a4, exc_frame->a5, exc_frame->a6, exc_frame->a7);
#else
    printf("a0: 0x%lx, a1: 0x%lx, a2: 0x%lx, a3: 0x%lx\r\n",
           exc_frame->a0, exc_frame->a1, exc_frame->a2, exc_frame->a3);
    printf("a4: 0x%lx, a5: 0x%lx\r\n", exc_frame->a4, exc_frame->a5);
#endif
    printf("cause: 0x%lx, epc: 0x%lx\r\n", exc_frame->cause, exc_frame->epc);

    if (PRV_M == mode) {
        /* msubm is exclusive to machine mode */
        printf("msubm: 0x%lx\r\n", exc_frame->msubm);
    }
}

/**
 * \brief       Register an exception handler for exception code EXCn
 * \details
 * - For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will be registered into SystemExceptionHandlers[EXCn-1].
 * - For EXCn == NMI_EXCn, it will be registered into SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM].
 * \param [in]  EXCn    See \ref EXCn_Type
 * \param [in]  exc_handler     The exception handler for this exception code EXCn
 */
void Exception_Register_EXC(uint32_t EXCn, unsigned long exc_handler)
{
#if defined(CODESIZE) && (CODESIZE == 1)

#else
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        SystemExceptionHandlers[EXCn] = exc_handler;
    } else if (EXCn == NMI_EXCn) {
        SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM] = exc_handler;
    }
#endif
}

/**
 * \brief       Get current exception handler for exception code EXCn
 * \details
 * - For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will return SystemExceptionHandlers[EXCn-1].
 * - For EXCn == NMI_EXCn, it will return SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM].
 * \param [in]  EXCn    See \ref EXCn_Type
 * \return  Current exception handler for exception code EXCn, if not found, return 0.
 */
unsigned long Exception_Get_EXC(uint32_t EXCn)
{
#if defined(CODESIZE) && (CODESIZE == 1)
    return 0;
#else
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        return SystemExceptionHandlers[EXCn];
    } else if (EXCn == NMI_EXCn) {
        return SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM];
    } else {
        return 0;
    }
#endif
}

/**
 * \brief      Common Interrupt handler entry when in clint/plic mode
 * \details
 * This function provided a command entry for interrupt in clint/plic mode
 * \param [in]  exccode   Exception Code
 * \param [in]  sp        stack pointer
 * \remarks
 * - This is not used for clic interrupt mode, which is only used for clint/plic interrupt mode,
 *   you should call \ref CLINT_Interrupt_Init or \ref PLIC_Interrupt_Init first to make sure this handler entry registered
 * - If you are not in eclic interrupt mode, please use please use \ref Interrupt_Register_CoreIRQ to register internal interrupt
 *   and use \ref Interrupt_Register_ExtIRQ to register external interrupt
 */
static void core_interrupt_handler(unsigned long exccode, unsigned long sp)
{
    INT_HANDLER int_handler = NULL;
    int_handler = (INT_HANDLER)(SystemCoreInterruptHandlers[exccode]);
    if (int_handler != NULL) {
        int_handler(exccode, sp);
    }
}


/**
 * \brief      Common NMI/Exception/Interrupt handler entry
 * \details
 * This function provided a command entry for NMI and exception. Silicon Vendor could modify
 * this template implementation according to requirement.
 * \param [in]  mcause    code indicating the reason that caused the trap in machine mode
 * \param [in]  sp        stack pointer
 * \remarks
 * - RISCV provided common entry for all types of exception and interrupt if not in eclic mode. This is proposed code template
 *   for exception entry function, Silicon Vendor could modify the implementation.
 * - For the core_exception_handler template, we provided exception register function \ref Exception_Register_EXC
 *   which can help developer to register your exception handler for specific exception number.
 * - If you are in eclic interrupt mode, please use \ref ECLIC_Register_IRQ to register both internal and external interrupt
 * - If you are not in eclic interrupt mode, please use please use \ref Interrupt_Register_CoreIRQ to register internal interrupt
 *   and use \ref Interrupt_Register_ExtIRQ to register external interrupt
 */
uint32_t core_exception_handler(unsigned long mcause, unsigned long sp)
{
#if defined(CODESIZE) && (CODESIZE == 1)
    // TODO when CODESIZE macro is defined
    // Exception_xxx APIs will not be used, all the m-mode exception handlers
    // will goto this function, and you can handle it here by yourself
    while (1);
#else

    unsigned long exccode = (mcause & MCAUSE_CAUSE);
    EXC_HANDLER exc_handler;

    if (mcause & MCAUSE_INTR) {
        if (system_core_interrupt_handler != NULL) {
            system_core_interrupt_handler(exccode, sp);
        }
    } else {
        if (exccode < MAX_SYSTEM_EXCEPTION_NUM) {
            exc_handler = (EXC_HANDLER)SystemExceptionHandlers[exccode];
        } else if (exccode == NMI_EXCn) {
            exc_handler = (EXC_HANDLER)SystemExceptionHandlers[MAX_SYSTEM_EXCEPTION_NUM];
        } else {
            exc_handler = (EXC_HANDLER)system_default_exception_handler;
        }
        if (exc_handler != NULL) {
            exc_handler(mcause, sp);
        }
    }

    return 0;
#endif
}

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
/**
 * \brief      Supervisor mode system Default Exception Handler
 * \details
 * This function provided a default supervisor mode exception and NMI handling code for all exception ids.
 * By default, It will just print some information for debug, Vendor can customize it according to its requirements.
 * \param [in]  scause    code indicating the reason that caused the trap in supervisor mode
 * \param [in]  sp        stack pointer
 */
static void system_default_exception_handler_s(unsigned long scause, unsigned long sp)
{
#if defined(CODESIZE) && (CODESIZE == 1)
#else
    /* TODO: Uncomment this if you have implement printf function */
    printf("SCAUSE : 0x%lx\r\n", scause);
    printf("SDCAUSE: 0x%lx\r\n", __RV_CSR_READ(CSR_SDCAUSE));
    printf("SEPC   : 0x%lx\r\n", __RV_CSR_READ(CSR_SEPC));
    printf("STVAL  : 0x%lx\r\n", __RV_CSR_READ(CSR_STVAL));
    Exception_DumpFrame(sp, PRV_S);
#if defined(SIMULATION_MODE)
    // directly exit if in SIMULATION
    extern void simulation_exit(int status);
    simulation_exit(1);
#else
    while (1);
#endif
#endif
}

/**
 * \brief       Register an exception handler for exception code EXCn of supervisor mode
 * \details
 * -For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will be registered into SystemExceptionHandlers_S[EXCn-1].
 * -For EXCn == NMI_EXCn, The NMI (Non-maskable-interrupt) cannot be trapped to the supervisor-mode or user-mode for any
 *    configuration, so NMI won't be registered into SystemExceptionHandlers_S.
 * \param [in]  EXCn            See \ref EXCn_Type
 * \param [in]  exc_handler     The exception handler for this exception code EXCn
 */
void Exception_Register_EXC_S(uint32_t EXCn, unsigned long exc_handler)
{
#if defined(CODESIZE) && (CODESIZE == 1)
#else
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        SystemExceptionHandlers_S[EXCn] = exc_handler;
    }
#endif
}

/**
 * \brief       Get current exception handler for exception code EXCn of supervisor mode
 * \details
 * - For EXCn < \ref MAX_SYSTEM_EXCEPTION_NUM, it will return SystemExceptionHandlers_S[EXCn-1].
 * \param [in]  EXCn    See \ref EXCn_Type
 * \return  Current exception handler for exception code EXCn, if not found, return 0.
 */
unsigned long Exception_Get_EXC_S(uint32_t EXCn)
{
#if defined(CODESIZE) && (CODESIZE == 1)
    return 0;
#else
    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        return SystemExceptionHandlers[EXCn];
    } else {
        return 0;
    }
#endif
}

/**
 * \brief      common Exception handler entry of supervisor mode
 * \details
 * This function provided a supervisor mode common entry for exception. Silicon Vendor could modify
 * this template implementation according to requirement.
 * \param [in]  scause    code indicating the reason that caused the trap in supervisor mode
 * \param [in]  sp        stack pointer
 * \remarks
 * - RISCV provided supervisor mode common entry for all types of exception. This is proposed code template
 *   for exception entry function, Silicon Vendor could modify the implementation.
 * - For the core_exception_handler_s template, we provided exception register function \ref Exception_Register_EXC_S
 *   which can help developer to register your exception handler for specific exception number.
 */
uint32_t core_exception_handler_s(unsigned long scause, unsigned long sp)
{
#if defined(CODESIZE) && (CODESIZE == 1)
    // TODO when CODESIZE macro is defined
    // Exception_xxx_S APIs will not be used, all the s-mode exception handlers
    // will goto this function, and you can handle it here by yourself
    while(1);
#else
    uint32_t EXCn = (uint32_t)(scause & 0X00000fff);
    EXC_HANDLER exc_handler;

    if (EXCn < MAX_SYSTEM_EXCEPTION_NUM) {
        exc_handler = (EXC_HANDLER)SystemExceptionHandlers_S[EXCn];
    } else {
        exc_handler = (EXC_HANDLER)system_default_exception_handler_s;
    }
    if (exc_handler != NULL) {
        exc_handler(scause, sp);
    }
    return 0;
}
#endif
#endif

/** @} */ /* End of Doxygen Group NMSIS_Core_ExceptionAndNMI */


#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
extern void irq_entry(void);
#endif
extern void exc_entry(void);

/**
 * \brief Do ECLIC Interrupt configuration
 * \details
 * This function will initialize cpu interrupt mode to eclic mode. It will
 * - set common non-vector entry to irq_entry
 * - set vector interrupt table to vector_base
 * - set exception entry to exc_entry
 * - set eclic mth to 0, and nlbits to the bigest bits it supports
 * - set s-mode common non-vector entry to irq_entry_s if tee present
 * - set s-mode vector interrupt table to vector_base_s if tee present
 * - set s-mode exception entry to exc_entry_s if tee present
 * - set eclic sth to 0 if tee present
 */
void ECLIC_Interrupt_Init(void)
{
#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
    unsigned long mcfg_info;

    mcfg_info = __RV_CSR_READ(CSR_MCFG_INFO);
    if (mcfg_info & MCFG_INFO_CLIC) {
        /* Set ECLIC vector interrupt base address to vector_base */
        __RV_CSR_WRITE(CSR_MTVT, (unsigned long)vector_table);
        /* Set ECLIC non-vector entry to irq_entry */
        __RV_CSR_WRITE(CSR_MTVT2, (unsigned long)irq_entry | 0x1);
        /* Set as CLIC interrupt mode */
        __RV_CSR_WRITE(CSR_MTVEC, (unsigned long)exc_entry | 0x3);
        /* Global Configuration about MTH and NLBits.
         * TODO: Please adapt it according to your system requirement.
         * This function is called in _init function */
        ECLIC_SetMth(0);
        ECLIC_SetCfgNlbits(__ECLIC_INTCTLBITS);

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
        /*
         * Intialize ECLIC supervisor mode vector interrupt
         * base address stvt to vector_table_s
         */
        __RV_CSR_WRITE(CSR_STVT, (unsigned long)vector_table_s);
        /*
         * Set ECLIC supervisor mode non-vector entry to be controlled
         * by stvt2 CSR register.
         * Intialize supervisor mode ECLIC non-vector interrupt
         * base address stvt2 to irq_entry_s.
        */
        __RV_CSR_WRITE(CSR_STVT2, (unsigned long)irq_entry_s);
        __RV_CSR_SET(CSR_STVT2, 0x01);
        /*
         * Set supervisor exception entry stvec to exc_entry_s */
        __RV_CSR_WRITE(CSR_STVEC, (unsigned long)exc_entry_s);
        /* Global Configuration about STH */
        ECLIC_SetSth(0);
#endif
    } else {
        /* Set as CLINT interrupt mode */
        __RV_CSR_WRITE(CSR_MTVEC, (unsigned long)exc_entry);
    }
#endif
}

/**
 * \brief Do CLINT Interrupt configuration
 * \details
 * This function will initialize cpu interrupt mode to clint mode. It will
 * - Set exception/interrupt entry to exc_entry, now interrupt and exception share the same entry point
 * - Register interrupt handling routine system_core_interrupt_handler to core_interrupt_handler function,
 *   which will be called in core_exception_handler function
 */
void CLINT_Interrupt_Init(void)
{
    /* Register core interrupt handler for clint/plic interrupt mode */
    system_core_interrupt_handler = core_interrupt_handler;
    /* Set as CLINT interrupt mode */
    __RV_CSR_WRITE(CSR_MTVEC, (unsigned long)exc_entry);
}

/**
 * \brief initialize interrupt controller
 * \details
 * Do CPU interrupt initialization, if plic present, init it, then init eclic if present.
 * So if ECLIC present, the interrupt will default configured to ECLIC interrupt mode,
 * if you want to switch to PLIC interrupt mode, you need to call PLIC_Interrupt_Init in
 * you application code.
 *
 * By default, if ECLIC present, eclic interrupt mode will be set, otherwise it will be
 * clint/plic interrupt mode
 * \remarks
 * This function previously was ECLIC_Init, now ECLIC_Init is removed
 */
void System_Interrupt_Init(void)
{
    /* Set as CLINT interrupt mode */
    __RV_CSR_WRITE(CSR_MTVEC, (unsigned long)exc_entry);
    ECLIC_Interrupt_Init();
}

#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
/**
 * \brief  Initialize a specific IRQ and register the handler
 * \details
 * This function set vector mode, trigger mode and polarity, interrupt level and priority,
 * assign handler for specific IRQn.
 * \param [in]  IRQn        NMI interrupt handler address
 * \param [in]  shv         \ref ECLIC_NON_VECTOR_INTERRUPT means non-vector mode, and \ref ECLIC_VECTOR_INTERRUPT is vector mode
 * \param [in]  trig_mode   see \ref ECLIC_TRIGGER_Type
 * \param [in]  lvl         interupt level
 * \param [in]  priority    interrupt priority
 * \param [in]  handler     interrupt handler, if NULL, handler will not be installed
 * \return       -1 means invalid input parameter. 0 means successful.
 * \remarks
 * - This function use to configure specific eclic interrupt and register its interrupt handler and enable its interrupt.
 * - If the vector table is placed in read-only section(FLASHXIP mode), handler could not be installed
 */
int32_t ECLIC_Register_IRQ(IRQn_Type IRQn, uint8_t shv, ECLIC_TRIGGER_Type trig_mode, uint8_t lvl, uint8_t priority, void* handler)
{
    if ((IRQn > __ECLIC_INTNUM) || (shv > ECLIC_VECTOR_INTERRUPT) \
        || (trig_mode > ECLIC_NEGTIVE_EDGE_TRIGGER)) {
        return -1;
    }

    /* set interrupt vector mode */
    ECLIC_SetShvIRQ(IRQn, shv);
    /* set interrupt trigger mode and polarity */
    ECLIC_SetTrigIRQ(IRQn, trig_mode);
    /* set interrupt level */
    ECLIC_SetLevelIRQ(IRQn, lvl);
    /* set interrupt priority */
    ECLIC_SetPriorityIRQ(IRQn, priority);
    if (handler != NULL) {
        /* set interrupt handler entry to vector table */
        ECLIC_SetVector(IRQn, (rv_csr_t)handler);
    }
    /* enable interrupt */
    ECLIC_EnableIRQ(IRQn);
    return 0;
}
#endif

/**
 * \brief  Register a riscv core interrupt and register the handler
 * \details
 * This function set interrupt handler for core interrupt
 * \param [in]  irqn        interrupt number
 * \param [in]  handler     interrupt handler, if NULL, handler will not be installed
 * \return       -1 means invalid input parameter. 0 means successful.
 * \remarks
 * - This function use to configure riscv core interrupt and register its interrupt handler and enable its interrupt.
 * - You can only use it when you are not in eclic interrupt mode
 */
int32_t Core_Register_IRQ(uint32_t irqn, void *handler)
{
    if ((irqn > SYSTEM_CORE_INTNUM)) {
        return -1;
    }

    if (handler != NULL) {
        /* register interrupt handler entry to core handlers */
        Interrupt_Register_CoreIRQ(irqn, (unsigned long)handler);
    }
    switch (irqn) {
        case SysTimerSW_IRQn:
            __enable_sw_irq();
            break;
        case SysTimer_IRQn:
            __enable_timer_irq();
            break;
        default:
            break;
    }

    return 0;
}

#if defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)
/**
 * \brief  Initialize a specific IRQ and register the handler for supervisor mode
 * \details
 * This function set vector mode, trigger mode and polarity, interrupt level and priority,
 * assign handler for specific IRQn.
 * \param [in]  IRQn        NMI interrupt handler address
 * \param [in]  shv         \ref ECLIC_NON_VECTOR_INTERRUPT means non-vector mode, and \ref ECLIC_VECTOR_INTERRUPT is vector mode
 * \param [in]  trig_mode   see \ref ECLIC_TRIGGER_Type
 * \param [in]  lvl         interupt level
 * \param [in]  priority    interrupt priority
 * \param [in]  handler     interrupt handler, if NULL, handler will not be installed
 * \return       -1 means invalid input parameter. 0 means successful.
 * \remarks
 * - This function use to configure specific eclic S-mode interrupt and register its interrupt handler and enable its interrupt.
 * - If the vector table is placed in read-only section (FLASHXIP mode), handler could not be installed.
 */
int32_t ECLIC_Register_IRQ_S(IRQn_Type IRQn, uint8_t shv, ECLIC_TRIGGER_Type trig_mode, uint8_t lvl, uint8_t priority, void* handler)
{
    if ((IRQn > __ECLIC_INTNUM) || (shv > ECLIC_VECTOR_INTERRUPT) \
        || (trig_mode > ECLIC_NEGTIVE_EDGE_TRIGGER)) {
        return -1;
    }

    /* set interrupt vector mode */
    ECLIC_SetShvIRQ_S(IRQn, shv);
    /* set interrupt trigger mode and polarity */
    ECLIC_SetTrigIRQ_S(IRQn, trig_mode);
    /* set interrupt level */
    ECLIC_SetLevelIRQ_S(IRQn, lvl);
    /* set interrupt priority */
    ECLIC_SetPriorityIRQ_S(IRQn, priority);
    if (handler != NULL) {
        /* set interrupt handler entry to vector table */
        ECLIC_SetVector_S(IRQn, (rv_csr_t)handler);
    }
    /* enable interrupt */
    ECLIC_EnableIRQ_S(IRQn);
    return 0;
}
#endif

uint32_t __irq_save(void)
{
    uint32_t oldstat;

    /* Read mstatus & clear machine interrupt enable (MIE) in mstatus */

    asm volatile("csrrc %0, mstatus, %1"
                 : "=r"(oldstat)
                 : "r"(MSTATUS_MIE));
    return oldstat;
}

void __irq_restore(uint32_t flags)
{
    /* Write flags to mstatus */

    asm volatile("csrw mstatus, %0"
                 : /* no output */
                 : "r"(flags));
}

/**
 * \brief      Check if address is in valid code region (excludes stack area)
 * \param [in] addr        Address to check
 * \param [in] stack_low   Lower bound of stack (to exclude)
 * \param [in] stack_high  Upper bound of stack (to exclude)
 * \return     1 if valid code address, 0 otherwise
 */
static int is_valid_code_addr_ex(unsigned long addr, unsigned long stack_low, unsigned long stack_high)
{
    /* Exclude stack region first */
    if (addr >= stack_low && addr < stack_high) {
        return 0;
    }

    /* Flash: 0x13000000 - 0x13400000 (primary code region) */
    if (addr >= 0x13000000 && addr < 0x13400000) return 1;
    /* PSRAM: 0x1e400000 - 0x1ec00000 */
    if (addr >= 0x1e400000 && addr < 0x1ec00000) return 1;

    /*
     * OCRAM code region: check against linker symbols if available
     * For RAM boot, code is in OCRAM but separate from stack
     */
    extern char __text_start[] __attribute__((weak));
    extern char __text_end[] __attribute__((weak));
    if (&__text_start && &__text_end) {
        if (addr >= (unsigned long)__text_start && addr < (unsigned long)__text_end) {
            return 1;
        }
    }

    return 0;
}

/**
 * \brief      Check if address is in valid stack region
 * \param [in] addr       Address to check
 * \param [in] stack_low  Lower bound of stack
 * \param [in] stack_high Upper bound of stack
 * \return     1 if valid stack address, 0 otherwise
 */
static int is_valid_stack_addr(unsigned long addr, unsigned long stack_low, unsigned long stack_high)
{
    return (addr >= stack_low) && (addr < stack_high) && ((addr & 0x3) == 0);
}

/**
 * \brief      Scan stack for potential return addresses (fallback method)
 * \details    When frame pointer chain is broken, scan stack for values
 *             that look like valid code addresses.
 * \param [in] sp_start    Start of scan range
 * \param [in] sp_end      End of scan range
 * \param [in] stack_low   Lower bound of valid stack
 * \param [in] stack_high  Upper bound of valid stack
 * \param [in] limit       Maximum addresses to find
 * \return     Number of potential return addresses found
 */
static int backtrace_scan_stack(unsigned long sp_start, unsigned long sp_end,
                                unsigned long stack_low, unsigned long stack_high,
                                int limit)
{
    int found = 0;
    unsigned long *scan;

    printf("\r\n--- Stack Scan (potential return addresses) ---\r\n");

    for (scan = (unsigned long *)sp_start;
         scan < (unsigned long *)sp_end && found < limit;
         scan++) {
        unsigned long val = *scan;

        /* Check if value looks like a valid code address */
        if (is_valid_code_addr_ex(val, stack_low, stack_high)) {
            printf("  [0x%08lx] -> 0x%08lx (potential RA)\r\n",
                   (unsigned long)scan, val);
            found++;
        }
    }

    if (found == 0) {
        printf("  No potential return addresses found in scan range\r\n");
    }

    return found;
}

/**
 * \brief      Print software stack backtrace using Frame Pointers
 * \details    Walks the stack frame chain using saved s0(fp) register
 *             to print return addresses for debugging. Falls back to
 *             stack scanning if frame pointer chain is broken.
 *
 * GCC RISC-V frame layout (with -fno-omit-frame-pointer):
 *   Function prologue typically does:
 *     addi sp, sp, -N
 *     sw   ra, N-4(sp)
 *     sw   s0, N-8(sp)
 *     addi s0, sp, N      ; s0 points to top of frame
 *
 *   So at runtime:
 *     [s0-4]  = saved ra (return address)
 *     [s0-8]  = saved s0 (previous frame pointer)
 *
 * \param [in] sp     Current stack pointer (exception frame base)
 * \param [in] limit  Maximum depth to trace
 */
void Exception_PrintBacktrace(unsigned long sp, int limit)
{
    EXC_Frame_Type *exc_frame = (EXC_Frame_Type *)sp;
    unsigned long fp;
    unsigned long ra;
    unsigned long prev_fp;
    int depth = 0;
    int fp_walk_failed = 0;

    /* Get memory boundaries from linker */
    extern char __StackBottom[];
    extern char __StackTop[];
    extern char __heap_start[];
    extern char __heap_end[];

    /*
     * Determine actual stack range based on current SP.
     * SP might be in:
     * 1. Main stack (__StackBottom to __StackTop)
     * 2. FreeRTOS task stack (allocated from heap)
     * 3. Interrupt stack
     */
    unsigned long main_stack_low = (unsigned long)__StackBottom;
    unsigned long main_stack_high = (unsigned long)__StackTop;
    unsigned long heap_low = (unsigned long)__heap_start;
    unsigned long heap_high = (unsigned long)__heap_end;

    unsigned long stack_low, stack_high;
    const char *stack_type;

    if (sp >= main_stack_low && sp < main_stack_high) {
        /* SP is in main stack */
        stack_low = main_stack_low;
        stack_high = main_stack_high;
        stack_type = "main";
    } else if (sp >= heap_low && sp < heap_high) {
        /*
         * SP is in heap region - likely a FreeRTOS task stack.
         * Use a heuristic: assume task stack is 4KB around SP
         */
        stack_low = (sp > 2048) ? (sp - 2048) : heap_low;
        stack_high = sp + 2048;
        if (stack_high > heap_high) stack_high = heap_high;
        stack_type = "task(heap)";
    } else {
        /*
         * SP is outside known regions - use generous bounds
         * This handles interrupt stacks or other special cases
         */
        stack_low = (sp > 4096) ? (sp - 4096) : 0x11010000;
        stack_high = sp + 4096;
        stack_type = "unknown";
    }

    printf("\r\n=== STACK BACKTRACE ===\r\n");
    printf("SP: 0x%08lx (%s stack)\r\n", sp, stack_type);
    printf("Stack range: 0x%08lx - 0x%08lx\r\n", stack_low, stack_high);
    printf("Saved ra: 0x%08lx\r\n", exc_frame->ra);
    printf("\r\n");
    printf("Depth |   Frame Ptr  |  Return Addr  | Info\r\n");
    printf("------|--------------|---------------|------------------\r\n");

    /* Frame 0: Exception point (from mepc) */
    printf("  %02d  |  0x%08lx  |  0x%08lx  | << Exception PC\r\n",
           depth++, sp, exc_frame->epc);

    /* Frame 1: Caller of faulting function (from saved ra in exception frame) */
    if (is_valid_code_addr_ex(exc_frame->ra, stack_low, stack_high)) {
        printf("  %02d  |      -       |  0x%08lx  | Caller (exc ra)\r\n",
               depth++, exc_frame->ra);
    }

#ifdef CONFIG_ENABLE_FRAME_POINTER
    /* The exception frame has no s0 slot (exc_entry saves caller-saved
     * registers only), so the frame cannot seed this walk -- but the chain is
     * still reachable from the OTHER end. Everything in this image compiles
     * with -fno-omit-frame-pointer, and exc_entry never touches s0 (objdump
     * of the linked entry shows zero x8/x9 references), so the interrupted
     * s0 arrived intact at core_exception_handler, whose prologue spilled it
     * like any callee-saved register. Anchoring on OUR OWN live frame
     * therefore walks up through the handler frames (below the exception
     * frame) and crosses seamlessly into the faulting context above it; the
     * crossing frame's saved ra points into exc_entry itself. A synchronous
     * exception never swaps sp (mscratchcswl swaps only on an interrupt-LEVEL
     * change), so the whole chain sits on one stack and one range check
     * covers it. Rows tagged "dump path" are this handler's own frames --
     * they prove the walk is live but carry no fault information. */
    fp = (unsigned long)__builtin_frame_address(0);
    if (!is_valid_stack_addr(fp, stack_low, stack_high)) {
        printf("  --  | own fp 0x%08lx outside stack range, FP chain unavailable\r\n", fp);
        fp_walk_failed = 1;
    }
#else
    /* Without CONFIG_ENABLE_FRAME_POINTER there is no chain to walk: s0 is a
     * plain callee-saved register and follows no frame discipline. */
    fp = 0;
    printf("  --  | no frame pointers in this build, FP chain unavailable\r\n");
    fp_walk_failed = 1;
#endif

    /* Walk the frame pointer chain */
    int crossed = 0;
    while (!fp_walk_failed && depth < limit) {
        /*
         * GCC RISC-V with -fno-omit-frame-pointer:
         *   [fp-4] = saved ra
         *   [fp-8] = saved previous fp (s0)
         */
        ra = *(unsigned long *)(fp - 4);
        prev_fp = *(unsigned long *)(fp - 8);

        /* Validate return address is code, not stack */
        if (!is_valid_code_addr_ex(ra, stack_low, stack_high)) {
            printf("  --  | ra 0x%08lx not valid code, chain broken\r\n", ra);
            fp_walk_failed = 1;
            break;
        }

        /* Frames above the exception frame belong to the interrupted code. */
        if (!crossed && fp > sp) {
            crossed = 1;
            printf("  ----- exception boundary: rows below are the faulting context -----\r\n");
        }
        printf("  %02d  |  0x%08lx  |  0x%08lx  | %s\r\n",
               depth++, fp, ra, crossed ? "Caller" : "dump path");

        /* Validate next frame pointer */
        if (prev_fp == 0) {
            /* Reached bottom of call stack (main or thread entry) */
            break;
        }

        if (prev_fp <= fp) {
            printf("  --  | prev_fp 0x%08lx <= fp, chain broken\r\n", prev_fp);
            fp_walk_failed = 1;
            break;
        }

        if (!is_valid_stack_addr(prev_fp, stack_low, stack_high)) {
            printf("  --  | prev_fp 0x%08lx out of range, chain broken\r\n", prev_fp);
            fp_walk_failed = 1;
            break;
        }

        fp = prev_fp;
    }

    printf("------|--------------|---------------|------------------\r\n");
    printf("FP chain: %d frames\r\n", depth);

    /*
     * Fallback: scan stack for potential return addresses
     * Only do this if FP chain failed early
     */
    if (fp_walk_failed && depth <= 3) {
        /* Scan from current SP upward, limit to reasonable range */
        unsigned long scan_start = sp;
        unsigned long scan_end = sp + 1024;  /* Scan 1KB / 256 words */
        if (scan_end > stack_high) {
            scan_end = stack_high;
        }
        backtrace_scan_stack(scan_start, scan_end, stack_low, stack_high, 20);
    }

    printf("\r\nDecode: riscv64-unknown-elf-addr2line -fe <elf> <addresses>\r\n");
}

