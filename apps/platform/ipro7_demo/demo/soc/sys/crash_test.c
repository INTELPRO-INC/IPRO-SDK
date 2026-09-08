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
 * Exception Crash Test Commands
 *
 * Shell commands to trigger various CPU exceptions for testing
 * the exception handler and crash dump system.
 *
 * Usage:
 *   crash_test null_read     - Load Access Fault (read NULL)
 *   crash_test null_write    - Store Access Fault (write NULL)
 *   crash_test illegal       - Illegal Instruction
 *   crash_test misalign_load - Load Address Misaligned
 *   crash_test overflow      - Stack Overflow (recursive)
 *   crash_test wild_jump     - Jump to invalid address
 *   crash_test assert        - FreeRTOS configASSERT failure
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <shell.h>
#include "exc_dump.h"
#include "ipro7.h"
#include "core_feature_eclic.h"

/* ------------------------------------------------------------------ */
/*  Override weak vAssertCalled to include crash dump                   */
/* ------------------------------------------------------------------ */

extern int board_get_debug_uart_index(void);

void vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    taskDISABLE_INTERRUPTS();
    printf("\r\n[!] ASSERT FAILED: %s:%lu\r\n", pcFileName ? pcFileName : "?", ulLine);
    printf("  Caller: %p\r\n", __builtin_return_address(0));

    /*
     * Build a minimal register snapshot for the dump.
     * We are not in exception context so sp points to our own stack.
     * Pass current sp so the dump header captures it.
     */
    unsigned long sp_val;
    __asm__ volatile("mv %0, sp" : "=r"(sp_val));
    exc_dump_wait_and_send(sp_val, board_get_debug_uart_index());

    while (1) { }
}

/* ------------------------------------------------------------------ */
/*  Test cases                                                         */
/* ------------------------------------------------------------------ */

static void crash_null_read(void)
{
    volatile int *p = (volatile int *)0;
    printf("Reading from NULL...\r\n");
    int val = *p;
    printf("Read value: %d (no fault?!)\r\n", val);
}

static void crash_null_write(void)
{
    volatile int *p = (volatile int *)0;
    printf("Writing to NULL...\r\n");
    *p = 0xDEADBEEF;
    printf("Write succeeded (no fault?!)\r\n");
}

static void crash_illegal_instruction(void)
{
    printf("Executing illegal instruction...\r\n");
    __asm__ volatile(".word 0x00000000");
    printf("No fault?!\r\n");
}

static void crash_misaligned_load(void)
{
    printf("Misaligned load...\r\n");
    volatile uint32_t *p = (volatile uint32_t *)0x11010001;
    uint32_t val = *p;
    printf("Read value: 0x%lx (no fault?!)\r\n", (unsigned long)val);
}

static void __attribute__((noinline)) crash_stack_overflow(int depth)
{
    volatile char buf[256];
    memset((void *)buf, depth & 0xFF, sizeof(buf));
    if (depth > 0)
        crash_stack_overflow(depth - 1);
    printf("depth=%d buf[0]=%d\r\n", depth, buf[0]);
}

static void crash_wild_jump(void)
{
    printf("Jumping to invalid address...\r\n");
    void (*fn)(void) = (void (*)(void))0xDEAD0000;
    fn();
    printf("No fault?!\r\n");
}

static void crash_assert(void)
{
    printf("Taking semaphore from NULL handle (triggers configASSERT)...\r\n");
    xSemaphoreTake(NULL, 0);
    printf("No assert?!\r\n");
}

/* ------------------------------------------------------------------ */
/*  HW Stack Overflow Test Scenarios                                   */
/* ------------------------------------------------------------------ */

/* Scenario 2: Single huge local blows past bound in one addi */
static void __attribute__((noinline)) crash_overflow_biglocal(void)
{
    printf("[overflow_biglocal] 8192-byte local on task stack...\r\n");
    volatile char buf[8192];
    memset((void *)buf, 0xBB, sizeof(buf));
    printf("buf[0]=%d (no trap?!)\r\n", buf[0]);
}

/* Scenario 3: Runtime-sized VLA */
static void __attribute__((noinline)) crash_overflow_vla(int size)
{
    printf("[overflow_vla] VLA size=%d on task stack...\r\n", size);
    volatile char buf[size];
    memset((void *)buf, 0xCC, size);
    printf("buf[0]=%d (no trap?!)\r\n", buf[0]);
}

/* Scenario 4: ISR stack overflow via software interrupt */
static void overflow_isr_handler(void)
{
    printf("[ISR] handler entered, allocating 4KB...\r\n");
    volatile char buf[4096];
    memset((void *)buf, 0xDD, sizeof(buf));
    printf("[ISR] buf[0]=%d (no trap?!)\r\n", buf[0]);
}

/* Scenario 5: Context switch pushes over the edge */
#define CTXSWITCH_STACK_WORDS 128  /* 512 bytes */
static StackType_t ctxswitch_stack[CTXSWITCH_STACK_WORDS];
static StaticTask_t ctxswitch_tcb;

static void overflow_ctxswitch_task(void *arg)
{
    /* Fill ~82% of 512-byte stack. portCONTEXT_SIZE (~120 bytes)
     * during context switch will push SP below bound. */
    volatile char pad[420];
    memset((void *)pad, 0xEE, sizeof(pad));
    printf("[overflow_ctxswitch] pad done, yielding...\r\n");
    for (;;) taskYIELD();
}

/* Scenario 6: Tiny task with insufficient stack */
#define TINYTASK_STACK_WORDS 64  /* 256 bytes */
static StackType_t tinytask_stack[TINYTASK_STACK_WORDS];
static StaticTask_t tinytask_tcb;

static void __attribute__((noinline)) overflow_tinytask_fn(void *arg)
{
    /* 256 bytes total stack. FreeRTOS context takes ~120 bytes,
     * leaving ~136 for the function. 200-byte local blows it. */
    volatile char buf[200];
    memset((void *)buf, 0xFF, sizeof(buf));
    printf("buf[0]=%d (no trap?!)\r\n", buf[0]);
    vTaskDelete(NULL);
}

/* Scenario 7: Deep non-recursive call chain, 1024-byte locals each.
 * 10 * 1024 = 10KB > 8KB shell_exec stack → guaranteed overflow. */
static void __attribute__((noinline)) deep_j(void) {
    volatile char b[1024]; memset((void *)b, 'j', sizeof(b));
}
static void __attribute__((noinline)) deep_i(void) {
    volatile char b[1024]; memset((void *)b, 'i', sizeof(b)); deep_j();
}
static void __attribute__((noinline)) deep_h(void) {
    volatile char b[1024]; memset((void *)b, 'h', sizeof(b)); deep_i();
}
static void __attribute__((noinline)) deep_g(void) {
    volatile char b[1024]; memset((void *)b, 'g', sizeof(b)); deep_h();
}
static void __attribute__((noinline)) deep_f(void) {
    volatile char b[1024]; memset((void *)b, 'f', sizeof(b)); deep_g();
}
static void __attribute__((noinline)) deep_e(void) {
    volatile char b[1024]; memset((void *)b, 'e', sizeof(b)); deep_f();
}
static void __attribute__((noinline)) deep_d(void) {
    volatile char b[1024]; memset((void *)b, 'd', sizeof(b)); deep_e();
}
static void __attribute__((noinline)) deep_c(void) {
    volatile char b[1024]; memset((void *)b, 'c', sizeof(b)); deep_d();
}
static void __attribute__((noinline)) deep_b(void) {
    volatile char b[1024]; memset((void *)b, 'b', sizeof(b)); deep_c();
}
static void __attribute__((noinline)) deep_a(void) {
    volatile char b[1024]; memset((void *)b, 'a', sizeof(b)); deep_b();
}

static void cmd_crash_test(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: crash_test <type>\r\n");
        printf("Types:\r\n");
        printf("  null_read          - Load from NULL pointer\r\n");
        printf("  null_write         - Store to NULL pointer\r\n");
        printf("  illegal            - Illegal instruction\r\n");
        printf("  misalign_load      - Misaligned 32-bit load\r\n");
        printf("  overflow           - Stack overflow (recursive)\r\n");
        printf("  wild_jump          - Jump to invalid address\r\n");
        printf("  assert             - FreeRTOS configASSERT failure\r\n");
        printf("  overflow_biglocal  - Single 8KB local variable\r\n");
        printf("  overflow_vla [sz]  - Runtime VLA (default 8192)\r\n");
        printf("  overflow_isr       - ISR stack overflow\r\n");
        printf("  overflow_ctxswitch - Context switch overflow\r\n");
        printf("  overflow_tinytask  - 256-byte task runs printf\r\n");
        printf("  overflow_deep      - 10-level deep call chain\r\n");
        return;
    }

    if (strcmp(argv[1], "null_read") == 0) {
        crash_null_read();
    } else if (strcmp(argv[1], "null_write") == 0) {
        crash_null_write();
    } else if (strcmp(argv[1], "illegal") == 0) {
        crash_illegal_instruction();
    } else if (strcmp(argv[1], "misalign_load") == 0) {
        crash_misaligned_load();
    } else if (strcmp(argv[1], "overflow") == 0) {
        printf("Triggering stack overflow...\r\n");
        crash_stack_overflow(1000);
    } else if (strcmp(argv[1], "wild_jump") == 0) {
        crash_wild_jump();
    } else if (strcmp(argv[1], "assert") == 0) {
        crash_assert();
    } else if (strcmp(argv[1], "overflow_biglocal") == 0) {
        crash_overflow_biglocal();
    } else if (strcmp(argv[1], "overflow_vla") == 0) {
        int size = 8192;
        if (argc >= 3)
            size = atoi(argv[2]);
        crash_overflow_vla(size);
    } else if (strcmp(argv[1], "overflow_isr") == 0) {
        printf("[overflow_isr] Registering ISR overflow handler...\r\n");
        extern int32_t ECLIC_Register_IRQ(IRQn_Type IRQn, uint8_t shv,
            ECLIC_TRIGGER_Type trig_mode, uint8_t lvl, uint8_t priority,
            void *handler);
        int32_t ret = ECLIC_Register_IRQ(IRTX_IRQn,
                           ECLIC_NON_VECTOR_INTERRUPT,
                           ECLIC_POSTIVE_EDGE_TRIGGER, 1, 0,
                           (void *)overflow_isr_handler);
        printf("[overflow_isr] Register ret=%ld, pending=%ld\r\n",
               (long)ret,
               (long)ECLIC_GetPendingIRQ(IRTX_IRQn));
        printf("[overflow_isr] Triggering interrupt...\r\n");
        ECLIC_SetPendingIRQ(IRTX_IRQn);
        printf("[overflow_isr] pending=%ld, waiting...\r\n",
               (long)ECLIC_GetPendingIRQ(IRTX_IRQn));
        vTaskDelay(pdMS_TO_TICKS(200));
        printf("No trap?!\r\n");
    } else if (strcmp(argv[1], "overflow_ctxswitch") == 0) {
        printf("[overflow_ctxswitch] Creating 512-byte task...\r\n");
        xTaskCreateStatic(overflow_ctxswitch_task, "ovf_ctx",
                          CTXSWITCH_STACK_WORDS, NULL, 15,
                          ctxswitch_stack, &ctxswitch_tcb);
    } else if (strcmp(argv[1], "overflow_tinytask") == 0) {
        printf("[overflow_tinytask] Creating 256-byte task...\r\n");
        xTaskCreateStatic(overflow_tinytask_fn, "ovf_tiny",
                          TINYTASK_STACK_WORDS, NULL, 15,
                          tinytask_stack, &tinytask_tcb);
    } else if (strcmp(argv[1], "overflow_deep") == 0) {
        printf("[overflow_deep] 10-level call chain, 1024B each...\r\n");
        deep_a();
    } else {
        printf("Unknown type: %s\r\n", argv[1]);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_crash_test, crash_test, Exception crash test);

/* ------------------------------------------------------------------ */
/*  Hardware Stack Protection CSR probe                                */
/* ------------------------------------------------------------------ */

/*
 * Hardware stack check CSRs:
 *   mstack_ctrl  (0x7C6) - stack check control
 *   mstack_bound (0x7C7) - stack bottom boundary
 *   mstack_base  (0x7C8) - stack top base
 *
 * If the CPU doesn't implement these, reading them triggers
 * Illegal Instruction (mcause.EXCCODE == 2).
 *
 * Strategy: temporarily set mtvec to a local handler that skips
 * the faulting instruction (mepc += 4), then try CSR reads.
 */

static volatile int _probe_trapped;

/* Minimal trap handler: skip the faulting instruction and set flag */
static void __attribute__((naked, aligned(4))) _probe_trap_handler(void)
{
    __asm__ volatile(
        "li   t0, 1\n"
        "la   t1, _probe_trapped\n"
        "sw   t0, 0(t1)\n"       /* _probe_trapped = 1 */
        "csrr t0, mepc\n"
        "addi t0, t0, 4\n"       /* skip faulting instruction */
        "csrw mepc, t0\n"
        "mret\n"
    );
}

static int probe_csr(uint32_t *out, const char *name, int csr_id)
{
    uint32_t saved_mtvec;
    uint32_t val = 0;

    _probe_trapped = 0;

    /* Save current mtvec and install probe handler */
    __asm__ volatile("csrr %0, mtvec" : "=r"(saved_mtvec));
    __asm__ volatile("csrw mtvec, %0" :: "r"((uint32_t)_probe_trap_handler));

    /* Fence to ensure mtvec write is visible */
    __asm__ volatile("fence" ::: "memory");

    /* Try reading the CSR */
    switch (csr_id) {
    case 0x7C6:
        __asm__ volatile("csrr %0, 0x7C6" : "=r"(val));
        break;
    case 0x7C7:
        __asm__ volatile("csrr %0, 0x7C7" : "=r"(val));
        break;
    case 0x7C8:
        __asm__ volatile("csrr %0, 0x7C8" : "=r"(val));
        break;
    default:
        break;
    }

    /* Fence again before restoring */
    __asm__ volatile("fence" ::: "memory");

    /* Restore original mtvec */
    __asm__ volatile("csrw mtvec, %0" :: "r"(saved_mtvec));

    if (_probe_trapped) {
        printf("  %-14s (0x%03X): NOT SUPPORTED (Illegal Instruction)\r\n",
               name, csr_id);
        return 0;
    }

    *out = val;
    printf("  %-14s (0x%03X): 0x%08lX\r\n", name, csr_id, (unsigned long)val);
    return 1;
}

static void cmd_stack_check(int argc, char **argv)
{
    uint32_t ctrl = 0, bound = 0, base = 0;
    int supported = 0;

    printf("=== Hardware Stack Protection Probe ===\r\n");

    taskDISABLE_INTERRUPTS();

    supported  = probe_csr(&ctrl,  "mstack_ctrl",  0x7C6);
    supported &= probe_csr(&bound, "mstack_bound", 0x7C7);
    supported &= probe_csr(&base,  "mstack_base",  0x7C8);

    taskENABLE_INTERRUPTS();

    printf("\r\nResult: HW stack check %s\r\n",
           supported ? "SUPPORTED" : "NOT supported");

    if (supported) {
        printf("  ctrl[0] (enable) = %lu\r\n",
               (unsigned long)(ctrl & 1));
        uint32_t sp_val;
        __asm__ volatile("mv %0, sp" : "=r"(sp_val));
        printf("  Current SP       = 0x%08lX\r\n",
               (unsigned long)sp_val);

#if ( configHW_STACK_PROTECTION == 1 )
        extern StackType_t * pxTaskGetStackBase(TaskHandle_t xTask);
        StackType_t *pxStack = pxTaskGetStackBase(NULL);
        printf("  pxStack (bound)  = 0x%08lX\r\n",
               (unsigned long)pxStack);
#endif
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_stack_check, stack_check, Probe HW stack protection CSRs);
