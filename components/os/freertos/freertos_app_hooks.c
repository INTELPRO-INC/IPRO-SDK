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

/**
 * @file freertos_app_hooks.c
 * @brief Default FreeRTOS hooks, heap setup, and platform init.
 *
 * Provides weak default implementations so apps don't need to
 * copy-paste the same boilerplate. Apps can override any function
 * by defining their own non-weak version.
 */
#include <stdint.h>
#include <stdio.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "platform_common.h"
#include "hal_boot2.h"
#include "compiler/common.h"

#ifdef CONFIG_IPRO_LOG_ENABLE
#include "ipro_log.h"
#define HOOKS_TAG "SYS"
#endif

#ifdef CONFIG_SHELL
#include "shell.h"
#include "hal_uart.h"
#endif

/* -----------------------------------------------------------------------
 * Heap configuration from linker symbols
 * -----------------------------------------------------------------------*/
/* Heap regions are described by START/END ADDRESS symbols and the size is
 * computed as (end - start). We deliberately do NOT read the linker *size*
 * symbols (__heap_*_size): their value is a size, so ld emits them in the
 * *ABS* section, and on the no-PSRAM OCRAM split the relaxer rewrites a load
 * of an ABS symbol to GP-relative — which truncates (R_RISCV_GPREL_I out of
 * range) once the GP / layout shifts. A difference of two real address symbols
 * is a plain link-time constant: no ABS read, no GP-relax, and the optimizer
 * can't fold it to "non-zero" either. */
extern char __heap_start[], __heap_end[];
extern char __heap_psram_start[], __heap_psram_end[];

/* Non-retention OCRAM heap (weak — only present when linker defines ocram_noret) */
extern char __heap_noret_start[] __attribute__((weak));
extern char __heap_noret_end[]   __attribute__((weak));

#if defined(CONFIG_LINK_FLASH) && defined(CONFIG_USE_PSRAM)
extern uint32_t __psram_available;
#endif

/* Persistent storage heap_5 keeps a pointer to (filled in ipro_setup_heap). */
static HeapRegion_t xHeapRegions[3];

void ipro_setup_heap(void)
{
    HeapRegion_t cand[2];
    int n = 0;

    /* Primary heap region. SKIP if zero-size: on the OCRAM split, once
     * .bss_ocram fills the retained front-64K, that region's heap leftover
     * is 0 bytes — handing a 0-byte region to heap_5 trips its configASSERT
     * (heap_5.c). The noret leftover below then becomes the sole heap. */
#if defined(CONFIG_LINK_FLASH) && defined(CONFIG_USE_PSRAM)
    size_t primary_sz = (size_t)(__heap_psram_end - __heap_psram_start);
    char  *primary_st = __heap_psram_start;
#else
    size_t primary_sz = (size_t)(__heap_end - __heap_start);
    char  *primary_st = __heap_start;
#endif
    if (primary_sz > 0) {
        cand[n].pucStartAddress = (uint8_t *)primary_st;
        cand[n].xSizeInBytes    = primary_sz;
        n++;
    }

    /* Non-retention OCRAM heap (ocram_noret leftover). Skip if empty or
     * coincident with the primary region (RAM==ocram non-split builds). */
    size_t noret_sz = (size_t)(__heap_noret_end - __heap_noret_start);
    if (noret_sz > 0 && __heap_noret_start != primary_st) {
        cand[n].pucStartAddress = (uint8_t *)__heap_noret_start;
        cand[n].xSizeInBytes    = noret_sz;
        n++;
    }

    /* Sort by ascending address (required by heap_5). */
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (cand[j].pucStartAddress > cand[j + 1].pucStartAddress) {
                HeapRegion_t tmp = cand[j];
                cand[j] = cand[j + 1];
                cand[j + 1] = tmp;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        xHeapRegions[i] = cand[i];
        printf("[HEAP] %zuKB @ %p\r\n",
               cand[i].xSizeInBytes >> 10, cand[i].pucStartAddress);
    }
    xHeapRegions[n].pucStartAddress = NULL;
    xHeapRegions[n].xSizeInBytes    = 0;

    vPortDefineHeapRegions(xHeapRegions);
}

/* -----------------------------------------------------------------------
 * Platform init sequence
 * -----------------------------------------------------------------------*/
void ipro_platform_init(void)
{
    platform_init(0);
    ipro_setup_heap();
    hal_boot2_init();

#if defined(CONFIG_LINK_FLASH) && defined(CONFIG_USE_PSRAM)
    if (!__psram_available) {
        printf("[PSRAM] WARNING: init failed\r\n");
    }
#endif

#ifdef CONFIG_IPRO_LOG_ENABLE
    ipro_log_init(NULL);
#endif

#ifdef CONFIG_SHELL
    shell_init_with_task(UART0_INDEX);
#endif
}

/* -----------------------------------------------------------------------
 * Runtime stats
 * -----------------------------------------------------------------------*/
void __attribute__((weak)) vConfigureTimerForRunTimeStats(void)
{
}

/*
 * FreeRTOS keeps per-task run time in a 32-bit counter, so what matters here is
 * not resolution but WRAP PERIOD. `ps` divides by (ulTotalRunTime - previous);
 * once the counter wraps inside the window that denominator collapses to the
 * true span modulo 2^32 and every task's CPU% is inflated by the same factor.
 * That is what made ps rows sum to 809% instead of 100%, which in turn left "is
 * this CPU-bound?" with no verdict for a whole session of uplink profiling.
 *
 * On RV32 `csrr mcycle` returns only the LOW 32 bits of the 64-bit cycle
 * counter, so it wraps every 2^32 cycles - under 30 s at any IPRO6 core clock,
 * i.e. shorter than a single iperf leg. Shifting that 32-bit read right, as
 * this function used to, does NOT fix it: it coarsens the step while leaving
 * the period at 2^32 cycles. Reading the full counter is the fix.
 *
 * Read mcycleh:mcycle with the usual carry retry and shift THAT, so the visible
 * 32-bit counter advances once per 256 cycles and wraps after 2^40 cycles -
 * over an hour, against seconds before. Resolution stays ~1 us, still about
 * 1000x the 1 kHz tick, which is the ratio FreeRTOS asks for.
 *
 * Weak, but there is no longer any reason for an app to override it; several
 * did, each with a byte-identical copy of the truncating version, which is how
 * one bug came to have eight homes.
 */
unsigned long __attribute__((weak)) ulGetRunTimeCounterValue(void)
{
    uint64_t cycles;

#if __riscv_xlen == 32
    uint32_t hi1, hi2, lo;
    do {
        __asm__ volatile("csrr %0, mcycleh" : "=r"(hi1));
        __asm__ volatile("csrr %0, mcycle" : "=r"(lo));
        __asm__ volatile("csrr %0, mcycleh" : "=r"(hi2));
    } while (hi1 != hi2);
    cycles = ((uint64_t)hi1 << 32) | lo;
#else
    __asm__ volatile("csrr %0, mcycle" : "=r"(cycles));
#endif

    return (unsigned long)(cycles >> 8);
}

/* -----------------------------------------------------------------------
 * FreeRTOS application hooks (all weak — apps can override)
 * -----------------------------------------------------------------------*/
/* Dump every task's stack high-water mark (minimum free stack ever, in bytes).
 * A small free value means the task nearly overflowed its allocation, so it
 * cannot be shrunk; a large free value means the stack is over-provisioned and
 * can be cut. Uses a static buffer + uxTaskGetSystemState (no heap), so it is
 * safe to call from the malloc-failed / stack-overflow hooks. */
void ipro_dump_task_stacks(const char *where)
{
#if (configUSE_TRACE_FACILITY == 1)
    static TaskStatus_t st[20];
    UBaseType_t n = uxTaskGetSystemState(st, 20, NULL);
    printf("[STACKS] %s heap free=%u:\r\n", where ? where : "",
           (unsigned) xPortGetFreeHeapSize());
    for (UBaseType_t i = 0; i < n; i++) {
        printf("  %-13s free=%u B\r\n", st[i].pcTaskName,
               (unsigned) (st[i].usStackHighWaterMark * sizeof(StackType_t)));
    }
#else
    (void) where;
#endif
}

void __attribute__((weak)) vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    printf("[!] Stack overflow: %s\r\n", pcTaskName ? pcTaskName : "?");
    ipro_dump_task_stacks("on stack overflow");
    while (1) { }
}

volatile size_t g_last_malloc_failed_size = 0;

void __attribute__((weak)) vApplicationMallocFailedHook(void)
{
    /* Keep only the immediate caller.  Walking above the current frame with
     * __builtin_return_address(1) is target/optimization dependent on RISC-V
     * and GCC warns correctly; an OOM hook must be deterministic. */
    void *ra = __builtin_return_address(0);
    printf("[!] Malloc failed, requested: %u, free: %u, min_ever: %u, caller: %p\r\n",
           (unsigned)g_last_malloc_failed_size,
           (unsigned)xPortGetFreeHeapSize(),
           (unsigned)xPortGetMinimumEverFreeHeapSize(),
           ra);
    if (g_last_malloc_failed_size == 0) {
        /* malloc(0) is not a real OOM — skip hang, just warn */
        printf("[!] WARNING: pvPortMalloc(0) called — not a real OOM, continuing\r\n");
        return;
    }
    ipro_dump_task_stacks("at OOM");
    while (1) { }
}

void __attribute__((weak)) vApplicationIdleHook(void)
{
    __WFI();
}

/* IDLE / Timer task buffers pinned to .ocram_bss → 64 KB retention
 * region. Under CONFIG_OCRAM_NORET_SPLIT, regular .bss goes to
 * ocram_noret which is wiped on every PDS31 wake. The scheduler hands
 * control back to IDLE after every PDS exit; if its stack was outside
 * the retained banks, the resumed frame is garbage. Hoisted to
 * file-scope because function-static + section attribute didn't
 * reliably place into .ocram_bss in some GCC versions. */
static __attribute__((section(".ocram_bss"))) StaticTask_t g_idle_tcb;
static __attribute__((section(".ocram_bss")))
    StackType_t g_idle_stack[configMINIMAL_STACK_SIZE];
static __attribute__((section(".ocram_bss"))) StaticTask_t g_timer_tcb;
static __attribute__((section(".ocram_bss")))
    StackType_t g_timer_stack[configTIMER_TASK_STACK_DEPTH];

void __attribute__((weak)) vApplicationGetIdleTaskMemory(
    StaticTask_t **ppxIdleTaskTCBBuffer,
    StackType_t **ppxIdleTaskStackBuffer,
    configSTACK_DEPTH_TYPE *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &g_idle_tcb;
    *ppxIdleTaskStackBuffer = g_idle_stack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void __attribute__((weak)) vApplicationGetTimerTaskMemory(
    StaticTask_t **ppxTimerTaskTCBBuffer,
    StackType_t **ppxTimerTaskStackBuffer,
    uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &g_timer_tcb;
    *ppxTimerTaskStackBuffer = g_timer_stack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/* -----------------------------------------------------------------------
 * Assert handler
 * -----------------------------------------------------------------------*/
void __attribute__((weak)) vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    taskDISABLE_INTERRUPTS();
    printf("[!] ASSERT %s:%lu\r\n", pcFileName ? pcFileName : "?", ulLine);
    printf("  Caller: %p\r\n", __builtin_return_address(0));
    while (1) { }
}

void user_vAssertCalled(const char *const pcFileName, unsigned long ulLine)
    __attribute__((weak, alias("vAssertCalled")));
