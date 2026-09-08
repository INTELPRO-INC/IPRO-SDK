/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Scheduler includes. */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"
#include "compiler/common.h"

/* Forward declarations for HW stack protection */
#if ( configHW_STACK_PROTECTION == 1 )
void vPortInitStackProtection(void);
void vPortUpdateStackProtection(void);
#endif

// #define ENABLE_KERNEL_DEBUG

#ifdef ENABLE_KERNEL_DEBUG
#define FREERTOS_PORT_DEBUG(...)                printf(__VA_ARGS__)
#else
#define FREERTOS_PORT_DEBUG(...)
#endif

#ifndef configSYSTICK_CLOCK_HZ
#define configSYSTICK_CLOCK_HZ                  SOC_TIMER_FREQ
#endif

#ifndef configKERNEL_INTERRUPT_PRIORITY
#define configKERNEL_INTERRUPT_PRIORITY         0
#endif

#ifndef configMAX_SYSCALL_INTERRUPT_PRIORITY
// See function prvCheckMaxSysCallPrio and prvCalcMaxSysCallMTH
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    255
#endif

/* Constants required to check the validity of an interrupt priority. */
#define portFIRST_USER_INTERRUPT_NUMBER ( 18 )

#define SYSTICK_TICK_CONST          (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ)

/* Masks off all bits but the ECLIC MTH bits in the MTH register. */
#define portMTH_MASK                ( 0xFFUL )

/* Constants required to set up the initial stack. */
#define portINITIAL_MSTATUS         ( MSTATUS_MPP | MSTATUS_MPIE | MSTATUS_FS_INITIAL | MSTATUS_VS_INITIAL)
#define portINITIAL_EXC_RETURN      ( 0xfffffffd )

/* The systick is a 64-bit counter. */
#define portMAX_BIT_NUMBER          ( SysTimer_MTIMER_Msk )

/* A fiddle factor to estimate the number of SysTick counts that would have
occurred while the SysTick counter is stopped during tickless idle
calculations. */
#define portMISSED_COUNTS_FACTOR    ( 45UL )

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
#define portTASK_RETURN_ADDRESS configTASK_RETURN_ADDRESS
#else
#define portTASK_RETURN_ADDRESS prvTaskExitError
#endif

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt(void);

/*
 * Exception handlers.
 */
void xPortSysTickHandler(void);

/*
 * Update hardware stack protection after context switch.
 * Defined in configHW_STACK_PROTECTION section below (or stub if disabled).
 */
void vPortUpdateStackProtection(void);

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
extern void prvPortStartFirstTask(void) __attribute__((naked));

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError(void);

#define xPortSysTickHandler     eclic_mtip_handler

/*-----------------------------------------------------------*/

ATTR_OCRAM_SECTION BaseType_t TrapNetCounter = 0;
ATTR_OCRAM_SECTION const BaseType_t  *pTrapNetCounter = &TrapNetCounter;

#if ( configHW_STACK_PROTECTION == 1 )
/* Cached stack bound for the current task. Assembly reads this via
 * pulCurrentStackBound to restore the bound when returning from ISR
 * to thread mode, avoiding false HW traps on the ISR→task SP transition. */
ATTR_OCRAM_SECTION uint32_t ulCurrentStackBound = 0;
ATTR_OCRAM_SECTION const uint32_t *pulCurrentStackBound = &ulCurrentStackBound;

/* ISR stack bound — bottom of interrupt stack (linker symbols).
 * Assembly reads via pulIsrStackBound in SAVE_CONTEXT. */
extern char _sp[];
extern char __STACK_SIZE[];
ATTR_OCRAM_SECTION uint32_t ulIsrStackBound;
ATTR_OCRAM_SECTION const uint32_t *pulIsrStackBound = &ulIsrStackBound;
#endif

/* Each task maintains its own interrupt status in the critical nesting
variable. */
#if ( configNUMBER_OF_CORES == 1 )
ATTR_OCRAM_SECTION UBaseType_t uxCriticalNesting = 0xaaaaaaaa;
#else /* #if ( configNUMBER_OF_CORES == 1 ) */
ATTR_OCRAM_SECTION UBaseType_t uxCriticalNestings[ configNUMBER_OF_CORES ] = { 0 };
#endif /* #if ( configNUMBER_OF_CORES == 1 ) */


/*
 * Record the real MTH calculated by the configMAX_SYSCALL_INTERRUPT_PRIORITY
 * The configMAX_SYSCALL_INTERRUPT_PRIORITY is not the left-aligned level value,
 * See equations below:
 * Level Bits number: lvlbits = min(nlbits, CLICINTCTLBITS)
 * Left align Bits number: lfabits = 8-lvlbits
 * 0 < configMAX_SYSCALL_INTERRUPT_PRIORITY <= (2^lvlbits-1)
 * uxMaxSysCallMTH = (configMAX_SYSCALL_INTERRUPT_PRIORITY << lfabits) | ((2^lfabits)-1)
 * If nlbits = 3, CLICINTCTLBITS=3, then lvlbits = 3, lfabits = 5
 * Set configMAX_SYSCALL_INTERRUPT_PRIORITY to 6
 * Then uxMaxSysCallMTH = (6<<5) | (2^5 - 1) = 223
 *
 * See function prvCheckMaxSysCallPrio and prvCalcMaxSysCallMTH
 */
ATTR_OCRAM_SECTION uint8_t uxMaxSysCallMTH = 255;

/*
 * The number of SysTick increments that make up one tick period.
 */
#if( configUSE_TICKLESS_IDLE == 1 )
static TickType_t ulTimerCountsForOneTick = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if( configUSE_TICKLESS_IDLE == 1 )
static TickType_t xMaximumPossibleSuppressedTicks = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
#if( configUSE_TICKLESS_IDLE == 1 )
static TickType_t ulStoppedTimerCompensation = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Used by the portASSERT_IF_INTERRUPT_PRIORITY_INVALID() macro to ensure
 * FreeRTOS API functions are not called from interrupts that have been assigned
 * a priority above configMAX_SYSCALL_INTERRUPT_PRIORITY.
 */
#if( configASSERT_DEFINED == 1 )
static uint8_t ucMaxSysCallPriority = 0;
#endif /* configASSERT_DEFINED */

#if ( configNUMBER_OF_CORES > 1 )

spin_lock_t hw_sync_locks[portRTOS_SPINLOCK_COUNT] = {0, 0};

/* Note this is a single method with uxAcquire parameter since we have
* static vars, the method is always called with a compile time constant for
* uxAcquire, and the compiler should do the right thing! */
void vPortRecursiveLock(unsigned long ulLockNum, spin_lock_t *pxSpinLock, BaseType_t uxAcquire)
{
    static uint8_t ucOwnedByCore[portMAX_CORE_COUNT];
    static uint8_t ucRecursionCountByLock[portRTOS_SPINLOCK_COUNT];

    configASSERT(ulLockNum < portRTOS_SPINLOCK_COUNT);
    unsigned long ulCoreNum = __get_hart_index();
    unsigned long ulLockBit = 1u << ulLockNum;
    configASSERT(ulLockBit < 256u);

    if (uxAcquire) {
        if ((!*pxSpinLock == 0)) {
            if (ucOwnedByCore[ulCoreNum] & ulLockBit) {
                configASSERT(ucRecursionCountByLock[ulLockNum] != 255u);
                ucRecursionCountByLock[ulLockNum]++;
                return;
            }

            while ((!*pxSpinLock == 0)) {
            }
        }

        do {
            if (__AMOSWAP_W(pxSpinLock, 1) == 0) {
                break;
            }
        } while (1);

        configASSERT(ucRecursionCountByLock[ulLockNum] == 0);
        ucRecursionCountByLock[ulLockNum] = 1;
        ucOwnedByCore[ulCoreNum] |= ulLockBit;
    } else {
        configASSERT((ucOwnedByCore[ulCoreNum] & ulLockBit) != 0);
        configASSERT(ucRecursionCountByLock[ulLockNum] != 0);

        if (!--ucRecursionCountByLock[ulLockNum]) {
            ucOwnedByCore[ulCoreNum] &= ~ulLockBit;
            *pxSpinLock = 0;
        }
    }
}
#endif
#if ( configNUMBER_OF_CORES > 1 )
static unsigned long ulSchedulerReady = 0;
#endif
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 * As per the standard RISC-V ABI pxTopcOfStack is passed in in a0, pxCode in
 * a1, and pvParameters in a2.  The new top of stack is passed out in a0.
 *
 * RISC-V maps registers to ABI names as follows (X1 to X31 integer registers
 * for the 'I' profile, X1 to X15 for the 'E' profile, currently I assumed).
 *
 * Register        ABI Name    Description                         Saver
 * x0              zero        Hard-wired zero                     -
 * x1              ra          Return address                      Caller
 * x2              sp          Stack pointer                       Callee
 * x3              gp          Global pointer                      -
 * x4              tp          Thread pointer                      -
 * x5-7            t0-2        Temporaries                         Caller
 * x8              s0/fp       Saved register/Frame pointer        Callee
 * x9              s1          Saved register                      Callee
 * x10-11          a0-1        Function Arguments/return values    Caller
 * x12-17          a2-7        Function arguments                  Caller
 * x18-27          s2-11       Saved registers                     Callee
 * x28-31          t3-6        Temporaries                         Caller
 *
 * The RISC-V context is saved RTOS tasks in the following stack frame,
 * where the global and thread pointers are currently assumed to be constant so
 * are not saved:
 *
 * mstatus
 * #ifndef __riscv_32e
 * x31
 * x30
 * x29
 * x28
 * x27
 * x26
 * x25
 * x24
 * x23
 * x22
 * x21
 * x20
 * x19
 * x18
 * x17
 * x16
 * #endif
 * x15
 * x14
 * x13
 * x12
 * x11
 * pvParameters
 * x9
 * x8
 * x7
 * x6
 * x5
 * portTASK_RETURN_ADDRESS
 * pxCode
 */
StackType_t* pxPortInitialiseStack(StackType_t* pxTopOfStack, TaskFunction_t pxCode, void* pvParameters)
{
    /* Simulate the stack frame as it would be created by a context switch
    interrupt. */

    /* Offset added to account for the way the MCU uses the stack on entry/exit
    of interrupts, and to ensure alignment. */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_MSTATUS;    /* MSTATUS */

    /* Save code space by skipping register initialisation. */
#ifndef __riscv_32e
    pxTopOfStack -= 22;    /* X11 - X31. */
#else
    pxTopOfStack -= 6;    /* X11 - X15. */
#endif
    *pxTopOfStack = (StackType_t) pvParameters;      /* X10/A0 */
    pxTopOfStack -= 6; /* X5 - X9 */
    *pxTopOfStack = (StackType_t) portTASK_RETURN_ADDRESS;      /* RA, X1 */

    pxTopOfStack --;
    *pxTopOfStack = ((StackType_t) pxCode) ;        /* PC */

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError(void)
{
    volatile uint32_t ulDummy = 0;

    /* A function that implements a task must not exit or attempt to return to
    its caller as there is nothing to return to.  If a task wants to exit it
    should instead call vTaskDelete( NULL ).

    Artificially force an assert() to be triggered if configASSERT() is
    defined, then stop here so application writers can catch the error. */
    configASSERT(portGET_CRITICAL_NESTING_COUNT() == ~0UL);
    portDISABLE_INTERRUPTS();
    while (ulDummy == 0) {
        /* This file calls prvTaskExitError() after the scheduler has been
        started to remove a compiler warning about the function being defined
        but never called.  ulDummy is used purely to quieten other warnings
        about code appearing after this function is called - making ulDummy
        volatile makes the compiler think the function could return and
        therefore not output an 'unreachable code' warning for code that appears
        after it. */
        /* Sleep and wait for interrupt */
        __WFI();
    }
}
/*-----------------------------------------------------------*/

static uint8_t prvCheckMaxSysCallPrio(uint8_t max_syscall_prio)
{
    uint8_t nlbits = __ECLIC_GetCfgNlbits();
    uint8_t intctlbits = __ECLIC_INTCTLBITS;
    uint8_t lvlbits, temp;

    if (nlbits <= intctlbits) {
        lvlbits = nlbits;
    } else {
        lvlbits = intctlbits;
    }

    temp = ((1 << lvlbits) - 1);
    if (max_syscall_prio > temp) {
        max_syscall_prio = temp;
    }
    return max_syscall_prio;
}

static uint8_t prvCalcMaxSysCallMTH(uint8_t max_syscall_prio)
{
    uint8_t nlbits = __ECLIC_GetCfgNlbits();
    uint8_t intctlbits = __ECLIC_INTCTLBITS;
    uint8_t lvlbits, lfabits;
    uint8_t maxsyscallmth = 0;
    uint8_t temp;

    if (nlbits <= intctlbits) {
        lvlbits = nlbits;
    } else {
        lvlbits = intctlbits;
    }

    lfabits = 8 - lvlbits;

    temp = ((1 << lvlbits) - 1);
    if (max_syscall_prio > temp) {
        max_syscall_prio = temp;
    }

    maxsyscallmth = (max_syscall_prio << lfabits) | ((1 << lfabits) - 1);

    return maxsyscallmth;
}

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler(void)
{
    /* configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0. */
    configASSERT(configMAX_SYSCALL_INTERRUPT_PRIORITY);

    /* Get the real MTH should be set to ECLIC MTH register */
    uxMaxSysCallMTH = prvCalcMaxSysCallMTH(configMAX_SYSCALL_INTERRUPT_PRIORITY);
    FREERTOS_PORT_DEBUG("Max SysCall MTH is set to 0x%x\n", uxMaxSysCallMTH);

#if( configASSERT_DEFINED == 1 )
    {
        /* Use the same mask on the maximum system call priority. */
        ucMaxSysCallPriority = prvCheckMaxSysCallPrio(configMAX_SYSCALL_INTERRUPT_PRIORITY);
        FREERTOS_PORT_DEBUG("Max SysCall Priority is set to %d\n", ucMaxSysCallPriority);
    }
#endif /* conifgASSERT_DEFINED */

    __disable_irq();

#if ( configNUMBER_OF_CORES > 1 )
    if (__get_hart_index() == BOOT_HARTID) {
        ulSchedulerReady = 1;
    } else {
        // other cores wait for scheduler ready signal
        while (ulSchedulerReady == 0);
    }
#endif

    /* Start the timer that generates the tick ISR.  Interrupts are disabled
    here already. */
    vPortSetupTimerInterrupt();

    /* Initialise the critical nesting count ready for the first task. */
    portSET_CRITICAL_NESTING_COUNT(0);

    /* Initialise base priority to zero. */
    vPortSetBASEPRI(0);

#if ( configHW_STACK_PROTECTION == 1 )
    /* Register HW stack overflow/underflow exception handlers */
    vPortInitStackProtection();
#endif

    /* Start the first task. */
    prvPortStartFirstTask();

    /* Should never get here as the tasks will now be executing!  Call the task
    exit error function to prevent compiler warnings about a static function
    not being called in the case that the application writer overrides this
    functionality by defining configTASK_RETURN_ADDRESS.  Call
    vTaskSwitchContext() so link time optimisation does not remove the
    symbol. */
#if ( configNUMBER_OF_CORES > 1 )
    vTaskSwitchContext( portGET_CORE_ID() );
#else
    vTaskSwitchContext();
#endif
    prvTaskExitError();

    /* Should not get here! */
    return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler(void)
{
    /* Not implemented in ports where there is nothing to return to.
    Artificially force an assert. */
    configASSERT(portGET_CRITICAL_NESTING_COUNT() == 1000UL);
}
/*-----------------------------------------------------------*/

void vPortEnterCritical(void)
{
    portDISABLE_INTERRUPTS();
    portINCREMENT_CRITICAL_NESTING_COUNT();

    /* This is not the interrupt safe version of the enter critical function so
    assert() if it is being called from an interrupt context.  Only API
    functions that end in "FromISR" can be used in an interrupt.  Only assert if
    the critical nesting count is 1 to protect against recursive calls if the
    assert function also uses a critical section. */
    if (portGET_CRITICAL_NESTING_COUNT() == 1) {
        configASSERT((__ECLIC_GetMth() & portMTH_MASK) == uxMaxSysCallMTH);
    }
}
/*-----------------------------------------------------------*/

void vPortExitCritical(void)
{
    configASSERT(portGET_CRITICAL_NESTING_COUNT());
    portDECREMENT_CRITICAL_NESTING_COUNT();
    if (portGET_CRITICAL_NESTING_COUNT() == 0) {
        portENABLE_INTERRUPTS();
    }
}
/*-----------------------------------------------------------*/

void vPortAssert(int32_t x)
{
    TaskHandle_t th;
    if ((x) == 0) {
        taskDISABLE_INTERRUPTS();
#if (INCLUDE_xTaskGetCurrentTaskHandle == 1)
        th = xTaskGetCurrentTaskHandle();
        if (th) {
            printf("Assert in task %s\n", pcTaskGetName(th));
        }
#endif
        while (1) {
            /* Sleep and wait for interrupt */
            __WFI();
        };
    }
}
/*-----------------------------------------------------------*/


void xPortTaskSwitch(void)
{
    portDISABLE_INTERRUPTS();
    /* Clear Software IRQ, A MUST */
    SysTimer_ClearSWIRQ();
#if ( configNUMBER_OF_CORES > 1 )
    vTaskSwitchContext( portGET_CORE_ID() );
#else
    vTaskSwitchContext();
#endif

    vPortUpdateStackProtection();

    portENABLE_INTERRUPTS();
}

/*-----------------------------------------------------------*/

#if ( configHW_STACK_PROTECTION == 1 )

/* CSR addresses for hardware stack check (from riscv_encoding.h) */
#define CSR_MSTACK_CTRL     0x7C6
#define CSR_MSTACK_BOUND    0x7C7
#define CSR_MSTACK_BASE     0x7C8

/* Exception codes */
#define EXC_STACK_OVERFLOW   0x18
#define EXC_STACK_UNDERFLOW  0x19

/*
 * Update mstack_bound CSR to match the current task's stack boundary.
 * Called from xPortTaskSwitch() after context switch selects the new task.
 *
 * - mstack_bound = pxCurrentTCB->pxStack (lowest valid SP)
 * - mstack_ctrl  = 0x1 (overflow check, MODE=0, OVF_TRACK_EN=1)
 *
 * CRITICAL: write BOUND before CTRL per hardware spec.
 */
extern StackType_t * pxTaskGetStackBase(TaskHandle_t xTask);

void vPortUpdateStackProtection(void)
{
    extern void *pxCurrentTCB;
    if (!pxCurrentTCB) return;

    StackType_t *pxStack = pxTaskGetStackBase(
        (TaskHandle_t)pxCurrentTCB);

    if (!pxStack) return;

    uint32_t bound_val = (uint32_t)pxStack;

    /* Cache bound for assembly ISR exit path (pulCurrentStackBound) */
    ulCurrentStackBound = bound_val;

    /* Write bound CSR — caller (eclic_msip_handler / prvPortStartFirstTask)
     * controls when mstack_ctrl enable bit is toggled. */
    __asm__ volatile("csrw 0x7C7, %0" :: "r"(bound_val));
}

static void prvStackOverflowHandler(unsigned long mcause, unsigned long sp)
{
    uint32_t mepc;
    __asm__ volatile("csrr %0, mepc" : "=r"(mepc));

    /* Read the original task SP from MSCRATCHCSWL (after exc_entry swap) */
    uint32_t task_sp;
    __asm__ volatile("csrr %0, 0x349" : "=r"(task_sp));

    extern int board_get_debug_uart_index(void);
    extern void *pxCurrentTCB;

    /* Detect ISR vs Task overflow using TrapNetCounter.
     * irq_entry increments TrapNetCounter on ISR entry. If > 0, the
     * overflow happened during ISR execution (ISR stack overflow).
     * If 0, the overflow happened during task execution or context
     * switch (task stack overflow). */
    extern BaseType_t TrapNetCounter;
    uint32_t isr_bottom = ulIsrStackBound;
    uint32_t isr_top = (uint32_t)_sp;
    int is_isr = (TrapNetCounter > 0);

    uint32_t scan_bound;

    if (is_isr) {
        /* ISR overflow — the original ISR SP at the point of overflow
         * is not recoverable (nested exc_entry clobbered it). Print
         * ISR stack bounds and MEPC for addr2line decode. */
        uint32_t isr_size = isr_top - isr_bottom;

        printf("\r\n\r\n*** ISR STACK OVERFLOW ***\r\n");
        printf("  MEPC      : 0x%08lX\r\n", (unsigned long)mepc);
        printf("  ISR Top   : 0x%08lX\r\n", (unsigned long)isr_top);
        printf("  ISR Bound : 0x%08lX\r\n", (unsigned long)isr_bottom);
        printf("  ISR StackSz: %lu bytes\r\n", (unsigned long)isr_size);
        scan_bound = isr_bottom;
    } else {
        uint32_t bound = ulCurrentStackBound;
        printf("\r\n\r\n*** HW STACK OVERFLOW ***\r\n");
        printf("  MEPC   : 0x%08lX\r\n", (unsigned long)mepc);
        printf("  TaskSP : 0x%08lX\r\n", (unsigned long)task_sp);
        printf("  Bound  : 0x%08lX\r\n", (unsigned long)bound);

        if (pxCurrentTCB) {
            TaskHandle_t xTask = (TaskHandle_t)pxCurrentTCB;
            StackType_t *pxStack = pxTaskGetStackBase(xTask);
            UBaseType_t uxPrio = uxTaskPriorityGet(xTask);
            configSTACK_DEPTH_TYPE hwm_words =
                uxTaskGetStackHighWaterMark(xTask);

            printf("  Task   : %s (prio %lu)\r\n",
                   pcTaskGetName(xTask), (unsigned long)uxPrio);
            printf("  pxStack: 0x%08lX\r\n", (unsigned long)pxStack);
            printf("  HWM    : %lu words free\r\n",
                   (unsigned long)hwm_words);
        }
        scan_bound = bound;
    }

    /* Stack boundary scan: 128 bytes below, 64 bytes above bound.
     * Look for flash return addresses (0x1300_0000 - 0x133F_FFFF). */
    {
        uint32_t scan_start = scan_bound - 128;
        uint32_t scan_end = scan_bound + 64;
        uint32_t addrs[16];
        int addr_count = 0;

        printf("=== Stack boundary scan (0x%08lX ± bytes) ===\r\n",
               (unsigned long)scan_bound);

        for (uint32_t a = scan_start; a < scan_end; a += 4) {
            uint32_t val = *(volatile uint32_t *)a;
            if (val >= 0x13000000 && val < 0x13400000) {
                printf("  [%+04ld] 0x%08lX",
                       (long)((int32_t)(a - scan_bound)),
                       (unsigned long)val);
                if (val == mepc)
                    printf("  <- MEPC");
                printf("\r\n");
                if (addr_count < 16)
                    addrs[addr_count++] = val;
            }
        }

        if (addr_count > 0) {
            printf("Decode: riscv64-unknown-elf-addr2line -fe "
                   "ipro7_demo_IPRO7.elf 0x%08lX",
                   (unsigned long)mepc);
            for (int i = 0; i < addr_count; i++) {
                if (addrs[i] != mepc)
                    printf(" 0x%08lX", (unsigned long)addrs[i]);
            }
            printf("\r\n");
        }
    }

#ifdef CONFIG_EXC_DUMP
    {
        unsigned long sp_val;
        __asm__ volatile("mv %0, sp" : "=r"(sp_val));
        extern void exc_dump_wait_and_send(unsigned long sp,
                                           int uart_index);
        exc_dump_wait_and_send(sp_val, board_get_debug_uart_index());
    }
#endif

    while (1) {
        __WFI();
    }
}

static void prvStackUnderflowHandler(unsigned long mcause, unsigned long sp)
{
    uint32_t mepc;
    __asm__ volatile("csrr %0, mepc" : "=r"(mepc));

    printf("\r\n\r\n*** HW STACK UNDERFLOW ***\r\n");
    printf("  MEPC : 0x%08lX\r\n", (unsigned long)mepc);
    printf("  SP   : 0x%08lX\r\n", (unsigned long)sp);

    while (1) {
        __WFI();
    }
}

void vPortInitStackProtection(void)
{
    /* Compute ISR stack bound from linker symbols */
    ulIsrStackBound = (uint32_t)_sp - (uint32_t)__STACK_SIZE;

    /* Register exception handlers for stack overflow/underflow */
    extern void Exception_Register_EXC(uint32_t EXCn,
                                       unsigned long exc_handler);
    Exception_Register_EXC(EXC_STACK_OVERFLOW,
                           (unsigned long)prvStackOverflowHandler);
    Exception_Register_EXC(EXC_STACK_UNDERFLOW,
                           (unsigned long)prvStackUnderflowHandler);
}

#else /* configHW_STACK_PROTECTION not enabled */

/* Stub so portasm.S call compiles without #ifdef in assembly */
void vPortUpdateStackProtection(void) {}

#endif /* configHW_STACK_PROTECTION */
/*-----------------------------------------------------------*/

void xPortSysTickHandler(void)
{
    /* The SysTick runs at the lowest interrupt priority, so when this interrupt
    executes all interrupts must be unmasked.  There is therefore no need to
    save and then restore the interrupt mask value as its value is already
    known. */
#if ( configNUMBER_OF_CORES == 1 )
    portDISABLE_INTERRUPTS();
    {
        SysTick_Reload(SYSTICK_TICK_CONST);
        /* Increment the RTOS tick. */
        if (xTaskIncrementTick() != pdFALSE) {
            /* A context switch is required.  Context switching is performed in
            the SWI interrupt.  Pend the SWI interrupt. */
            portYIELD();
        }
    }
    portENABLE_INTERRUPTS();
#else
    UBaseType_t ulPreviousMask;
    /* Tasks or ISRs running on other cores may still in critical section in
     * multiple cores environment. Incrementing tick needs to performed in
     * critical section. */
    ulPreviousMask = taskENTER_CRITICAL_FROM_ISR();
    {
        SysTick_Reload(SYSTICK_TICK_CONST);
        /* Increment the RTOS tick. */
        if (xTaskIncrementTick() != pdFALSE) {
            /* A context switch is required.  Context switching is performed in
            the SWI interrupt.  Pend the SWI interrupt. */
            portYIELD();
        }
    }
    taskEXIT_CRITICAL_FROM_ISR( ulPreviousMask );
#endif
}
/*-----------------------------------------------------------*/

#if( configUSE_TICKLESS_IDLE == 1 )

/* Applications may clamp the kernel's next-task idle interval with a hardware
 * deadline (radio, audio, etc.).  The default preserves existing behaviour.
 * The hook must be lock-free and must never extend the RTOS interval. */
__attribute__((weak)) TickType_t xPortApplicationClampIdleTime(
    TickType_t xExpectedIdleTime)
{
    return xExpectedIdleTime;
}

/* Optional, lock-free measurement hook.  wake_by_timer is true when the
 * programmed tickless timer expired; aborted is true when the scheduler
 * rejected sleep after the application deadline had been selected. */
__attribute__((weak)) void vPortApplicationTicklessObservation(
    TickType_t xSelectedIdleTime, TickType_t xCompletedIdleTime,
    BaseType_t wake_by_timer, BaseType_t aborted)
{
    (void)xSelectedIdleTime;
    (void)xCompletedIdleTime;
    (void)wake_by_timer;
    (void)aborted;
}

/* Called after the scheduler has confirmed sleep and with interrupts masked.
 * Applications may atomically revalidate a previously published hardware
 * deadline here.  The default has no side effects. */
__attribute__((weak)) void vPortApplicationPreSleepRevalidate(
    TickType_t xSelectedIdleTime)
{
    (void)xSelectedIdleTime;
}

/* Called after the pre-sleep revalidation hook.  Return pdTRUE when that hook
 * made a higher-priority task ready and the port should return to the
 * scheduler immediately instead of executing WFI for this idle attempt. */
__attribute__((weak)) BaseType_t xPortApplicationPreSleepSkipWfi(void)
{
    return pdFALSE;
}

/* Runs immediately after WFI while interrupts are still masked.  This allows
 * an application to disarm an always-on dry-run wake source before its pending
 * IRQ can vector when MIE is restored. */
__attribute__((weak)) void vPortApplicationPostSleepMasked(void)
{
}

/* Optional always-on-clock correction for platforms where the SysTimer does
 * not advance at the normal rate during a hardware sleep state. */
__attribute__((weak)) TickType_t xPortApplicationCorrectElapsedTicks(
    TickType_t xExpectedIdleTime, TickType_t xPortMeasuredTicks)
{
    (void)xExpectedIdleTime;
    return xPortMeasuredTicks;
}

__attribute__((weak)) void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements;
    volatile TickType_t xModifiableIdleTime, XLastLoadValue;

    FREERTOS_PORT_DEBUG("Enter TickLess %d\n", (uint32_t)xExpectedIdleTime);

    /* Make sure the SysTick reload value does not overflow the counter. */
    if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks) {
        xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
    }

    xExpectedIdleTime = xPortApplicationClampIdleTime(xExpectedIdleTime);
    if (xExpectedIdleTime < 2U) {
        return;
    }

    /* The application hook may reduce the value further; if it ever returns a
     * larger diagnostic value, keep the hardware reload bounded before using
     * it for either WFI or platform-PDS admission. */
    if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks) {
        xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
    }

    /* Stop the SysTick momentarily.  The time the SysTick is stopped for
    is accounted for as best it can be, but using the tickless mode will
    inevitably result in some tiny drift of the time maintained by the
    kernel with respect to calendar time. */
    SysTimer_Stop();

    /* Calculate the reload value required to wait xExpectedIdleTime
    tick periods.  -1 is used because this code will execute part way
    through one of the tick periods. */
    ulReloadValue = (ulTimerCountsForOneTick * (xExpectedIdleTime - 1UL));
    if (ulReloadValue > ulStoppedTimerCompensation) {
        ulReloadValue -= ulStoppedTimerCompensation;
    }

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
    method as that will mask interrupts that should exit sleep mode. */
    __disable_irq();

    /* If a context switch is pending or a task is waiting for the scheduler
    to be unsuspended then abandon the low power entry. */
    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
        vPortApplicationTicklessObservation(xExpectedIdleTime, 0U,
                                             pdFALSE, pdTRUE);
        /* Restart from whatever is left in the count register to complete
        this tick period. */
        /* Restart SysTick. */
        SysTimer_Start();

        /* Reset the reload register to the value required for normal tick
           periods. */
        SysTick_Reload(ulTimerCountsForOneTick);

        /* Re-enable interrupts - see comments above the cpsid instruction()
           above. */
        __enable_irq();
    } else {
        /* Set the new reload value. */
        SysTick_Reload(ulReloadValue);

        /* Get System timer load value before sleep */
        XLastLoadValue = SysTimer_GetLoadValue();

        /* Restart SysTick. */
        SysTimer_Start();
        ECLIC_EnableIRQ(SysTimer_IRQn);
        __RWMB();

        /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
        set its parameter to 0 to indicate that its implementation contains
        its own wait for interrupt or wait for event instruction, and so wfi
        should not be executed again.  However, the original expected idle
        time variable must remain unmodified, so a copy is taken. */
        xModifiableIdleTime = xExpectedIdleTime;
        vPortApplicationPreSleepRevalidate(xExpectedIdleTime);
        if (xPortApplicationPreSleepSkipWfi() != pdFALSE) {
            /* The application has consumed or rejected this idle window and
             * requested an immediate return to the scheduler.  Do not run the
             * normal tickless elapsed-time path with no WFI: repeatedly doing
             * so reloads the SysTimer before it can expire, so delayed tasks
             * and one-tick controller waits stop making progress. */
            vPortApplicationTicklessObservation(xExpectedIdleTime, 0U,
                                                pdFALSE, pdTRUE);
            SysTick_Reload(ulTimerCountsForOneTick);
            ECLIC_EnableIRQ(SysTimer_IRQn);
            __enable_irq();
            return;
        }
        configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
        if (xModifiableIdleTime > 0) {
            __WFI();
        }
        vPortApplicationPostSleepMasked();
        configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

        /* Re-enable interrupts to allow the interrupt that brought the MCU
        out of sleep mode to execute immediately. */
        __enable_irq();

        /* Make sure interrupt enable is executed */
        __RWMB();
        __FENCE_I();
        __NOP();

        /* Disable interrupts again because the clock is about to be stopped
           and interrupts that execute while the clock is stopped will increase
           any slippage between the time maintained by the RTOS and calendar
           time. */
        __disable_irq();

        /* Disable the SysTick clock.  Again,
           the time the SysTick is stopped for is accounted for as best it can
           be, but using the tickless mode will inevitably result in some tiny
           drift of the time maintained by the kernel with respect to calendar
           time*/
        ECLIC_DisableIRQ(SysTimer_IRQn);

        /* Determine if SysTimer Interrupt is not yet happened,
        (in which case an interrupt other than the SysTick
        must have brought the system out of sleep mode). */
        if (SysTimer_GetLoadValue() >= (XLastLoadValue + ulReloadValue)) {
            /* As the pending tick will be processed as soon as this
            function exits, the tick value maintained by the tick is stepped
            forward by one less than the time spent waiting. */
            ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
            FREERTOS_PORT_DEBUG("TickLess - SysTimer Interrupt Entered!\n");
        } else {
            /* Something other than the tick interrupt ended the sleep.
            Work out how long the sleep lasted rounded to complete tick
            periods (not the ulReload value which accounted for part
            ticks). */
            xModifiableIdleTime = SysTimer_GetLoadValue();
            if (xModifiableIdleTime > XLastLoadValue) {
                ulCompletedSysTickDecrements = (xModifiableIdleTime - XLastLoadValue);
            } else {
                ulCompletedSysTickDecrements = (xModifiableIdleTime + portMAX_BIT_NUMBER - XLastLoadValue);
            }

            /* How many complete tick periods passed while the processor
            was waiting? */
            ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;

            /* The reload value is set to whatever fraction of a single tick
            period remains. */
            SysTick_Reload(ulTimerCountsForOneTick);
            FREERTOS_PORT_DEBUG("TickLess - External Interrupt Happened!\n");
        }

        ulCompleteTickPeriods = (uint32_t)xPortApplicationCorrectElapsedTicks(
            xExpectedIdleTime, (TickType_t)ulCompleteTickPeriods);
        FREERTOS_PORT_DEBUG("End TickLess %d\n", (uint32_t)ulCompleteTickPeriods);

        vPortApplicationTicklessObservation(
            xExpectedIdleTime, (TickType_t)ulCompleteTickPeriods,
            (ulCompleteTickPeriods == (xExpectedIdleTime - 1U)) ? pdTRUE : pdFALSE,
            pdFALSE);

        /* Restart SysTick */
        vTaskStepTick(ulCompleteTickPeriods);

        /* Exit with interrupts enabled. */
        ECLIC_EnableIRQ(SysTimer_IRQn);
        __enable_irq();
    }
}

#endif /* #if configUSE_TICKLESS_IDLE */
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
__attribute__((weak)) void vPortSetupTimerInterrupt(void)
{
    /* Calculate the constants required to configure the tick interrupt. */
#if( configUSE_TICKLESS_IDLE == 1 )
    {
        ulTimerCountsForOneTick = (SYSTICK_TICK_CONST);
        /* This port programs the suppressed interval through the 32-bit
         * ulReloadValue below, even though the underlying MTIMER is 64-bit.
         * Bound suppressed ticks to the implemented reload width; otherwise a
         * very large xExpectedIdleTime can overflow ulReloadValue and the
         * application low-power policy observes an impossible idle window. */
        xMaximumPossibleSuppressedTicks = 0xffffffffUL / ulTimerCountsForOneTick;
        /* Convert the entry/exit overhead from CPU-clock counts to SysTimer
         * counts without first truncating the clock ratio.  IPRO7 declares a
         * 4 MHz SysTimer and a 1 MHz reference CPU clock, so the old
         *   factor / (cpu_hz / systick_hz)
         * evaluated to division by zero as soon as tickless mode 1 was used. */
        ulStoppedTimerCompensation =
            (portMISSED_COUNTS_FACTOR * configSYSTICK_CLOCK_HZ +
             configCPU_CLOCK_HZ - 1UL) / configCPU_CLOCK_HZ;
        FREERTOS_PORT_DEBUG("CountsForOneTick, SuppressedTicks and TimerCompensation: %u, %u, %u\n", \
                            (uint32_t)ulTimerCountsForOneTick, (uint32_t)xMaximumPossibleSuppressedTicks, (uint32_t)ulStoppedTimerCompensation);
    }
#endif /* configUSE_TICKLESS_IDLE */
    TickType_t ticks = SYSTICK_TICK_CONST;

    /* Make SWI and SysTick the lowest priority interrupts. */
    /* Stop and clear the SysTimer. SysTimer as Non-Vector Interrupt */
#if ( configNUMBER_OF_CORES > 1 )
    if (__get_hart_index() == BOOT_HARTID) {
#else
    if (1) {
#endif
        SysTick_Config(ticks);
        ECLIC_DisableIRQ(SysTimer_IRQn);
        ECLIC_SetLevelIRQ(SysTimer_IRQn, configKERNEL_INTERRUPT_PRIORITY);
        ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT);
        ECLIC_EnableIRQ(SysTimer_IRQn);
    }

    /* Set SWI interrupt level to lowest level/priority, SysTimerSW as Vector Interrupt */
    ECLIC_SetShvIRQ(SysTimerSW_IRQn, ECLIC_VECTOR_INTERRUPT);
    ECLIC_SetLevelIRQ(SysTimerSW_IRQn, configKERNEL_INTERRUPT_PRIORITY);
    ECLIC_EnableIRQ(SysTimerSW_IRQn);
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

#if( configASSERT_DEFINED == 1 )

void vPortValidateInterruptPriority(void)
{
    uint32_t ulCurrentInterrupt;
    uint8_t ucCurrentPriority;

    /* FromISR APIs are also legal from TASK context (e.g. under a suspended
     * scheduler, where only the FromISR variants may be used). Outside a trap
     * mcause holds the LAST trap's residue: validating it here asserted the
     * system dead whenever such a call happened to follow an exception-class
     * trap (live-caught: the first PDS wake after a WiFi disconnect, and any
     * wake after a JTAG debug halt). Only validate inside a real trap. */
    if (!xPortIsInsideInterrupt())
    {
        return;
    }

    /* Obtain the number of the currently executing interrupt. */
    CSR_MCAUSE_Type mcause;
    mcause.d = __RV_CSR_READ(CSR_MCAUSE);
    /* Make sure current trap type is interrupt */
    configASSERT(mcause.b.interrupt == 1);
    if (mcause.b.interrupt) {
        ulCurrentInterrupt = mcause.b.exccode;
        /* Is the interrupt number a user defined interrupt? */
        if (ulCurrentInterrupt >= portFIRST_USER_INTERRUPT_NUMBER) {
            /* Look up the interrupt's priority. */
            ucCurrentPriority = __ECLIC_GetLevelIRQ(ulCurrentInterrupt);
            /* The following assertion will fail if a service routine (ISR) for
            an interrupt that has been assigned a priority above
            ucMaxSysCallPriority calls an ISR safe FreeRTOS API
            function.  ISR safe FreeRTOS API functions must *only* be called
            from interrupts that have been assigned a priority at or below
            ucMaxSysCallPriority.

            Numerically low interrupt priority numbers represent logically high
            interrupt priorities, therefore the priority of the interrupt must
            be set to a value equal to or numerically *higher* than
            ucMaxSysCallPriority.

            Interrupts that use the FreeRTOS API must not be left at their
            default priority of zero as that is the highest possible priority,
            which is guaranteed to be above ucMaxSysCallPriority,
            and therefore also guaranteed to be invalid.

            FreeRTOS maintains separate thread and ISR API functions to ensure
            interrupt entry is as fast and simple as possible.

            The following links provide detailed information:
            http://www.freertos.org/FAQHelp.html */
            configASSERT(ucCurrentPriority <= ucMaxSysCallPriority);
        }
    }
}

#endif /* configASSERT_DEFINED */
