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
 * @file main.c
 * @brief NMSIS-NN bring-up + A/B benchmark against the components/ai kernels.
 *
 * Answers two questions on real silicon:
 *   1. Does upstream NMSIS-NN run correctly on IPRO7 with the vendor march?
 *   2. How much does the P-extension (xxldsp) actually buy over the plain-C
 *      uint8 convolution that components/ai ships today?
 *
 * Build the same app twice - once with an xxldsp march, once without - to get
 * the DSP-on/off delta; the banner prints which one is active.
 *
 * Shell: nn_bench
 */

#include <stdint.h>
#include <stdio.h>

#include <generated/autoconf.h>

#include <FreeRTOS.h>
#include <task.h>

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#include "freertos_app_hooks.h"

#include "src/nn_bench.h"
#include "src/cpuclk.h"

/* Reported by the datasheet as up to 192MHz; only used for the ms estimate. */
#define CPU_MHZ 192

/* Layers picked to span the shapes a small vision/audio model actually hits. */
static const nn_bench_shape_t k_shapes[] = {
    /* name                    in_h in_w in_c   k_h k_w out_c  stride pad */
    { "3x3  32x32x16 ->32",      32,  32,  16,    3,  3,   32,      1,  1 },
    { "1x1  32x32x32 ->32",      32,  32,  32,    1,  1,   32,      1,  0 },
    { "3x3  16x16x32 ->64",      16,  16,  32,    3,  3,   64,      1,  1 },
    { "3x3  64x64x8  ->16",      64,  64,   8,    3,  3,   16,      2,  1 },
};

static void print_dsp_banner(void)
{
    printf("NMSIS-NN bring-up / benchmark\r\n");
#if defined(__riscv_xxldspn3x)
    printf("  march DSP level : xxldspn3x\r\n");
#elif defined(__riscv_xxldspn2x)
    printf("  march DSP level : xxldspn2x\r\n");
#elif defined(__riscv_xxldspn1x)
    printf("  march DSP level : xxldspn1x\r\n");
#elif defined(__riscv_xxldsp)
    printf("  march DSP level : xxldsp\r\n");
#else
    printf("  march DSP level : NONE (plain rv32imafc)\r\n");
#endif
#if defined(CONFIG_AI_SUPPORT) && defined(CONFIG_AI_CPU_NN_OPS)
    printf("  components/ai   : present (u8 A/B enabled)\r\n");
#else
    printf("  components/ai   : absent (s8 only)\r\n");
#endif
    printf("\r\n");
}

static void run_bench(nn_bench_mem_t mem)
{
    print_dsp_banner();

    nn_bench_arena_select(mem);
    printf("  buffers in      : %s (0x%08lx)\r\n",
           mem == NN_BENCH_MEM_PSRAM ? "PSRAM" : "OCRAM",
           (unsigned long)(uintptr_t)nn_bench_arena_base());
    printf("\r\n");

    printf("%-24s %12s %12s %9s %10s\r\n",
           "layer", "s8 cycles", "u8 cycles", "speedup", "s8 scratch");
    printf("%-24s %12s %12s %9s %10s\r\n",
           "------------------------", "------------", "------------",
           "---------", "----------");

    for (unsigned i = 0; i < sizeof(k_shapes) / sizeof(k_shapes[0]); i++) {
        const nn_bench_shape_t *s = &k_shapes[i];
        nn_bench_result_t r8 = { 0 }, ru = { 0 };

        nn_bench_s8(s, &r8);
        nn_bench_u8(s, &ru);

        char sp[16];
        if (r8.ran && ru.ran && r8.cycles > 0) {
            /* one decimal, no float printf dependency */
            const uint32_t x10 = (ru.cycles * 10u) / r8.cycles;
            snprintf(sp, sizeof(sp), "%lu.%lux", (unsigned long)(x10 / 10),
                     (unsigned long)(x10 % 10));
        } else {
            snprintf(sp, sizeof(sp), "-");
        }

        printf("%-24s %12lu %12lu %9s %10ld\r\n",
               s->name,
               (unsigned long)(r8.ran ? r8.cycles : 0),
               (unsigned long)(ru.ran ? ru.cycles : 0),
               sp,
               (long)r8.scratch);

        if (r8.ran && r8.checksum == 0) {
            printf("  WARNING: s8 checksum is 0 - kernel may have no-opped\r\n");
        }
    }

    printf("\r\n  (cycles are the kernel call only; @%d MHz, 1000 cycles ~ %d us)\r\n",
           CPU_MHZ, 1000 / CPU_MHZ);
}

#ifdef CONFIG_SHELL
static int cmd_nn_bench(int argc, char **argv)
{
    /* nn_bench [ocram|psram]  - default runs both, which is the useful case:
     * the ratio between them is what says whether a result is memory-bound. */
    if (argc > 1 && argv[1][0] == 'o') {
        run_bench(NN_BENCH_MEM_OCRAM);
    } else if (argc > 1 && argv[1][0] == 'p') {
        run_bench(NN_BENCH_MEM_PSRAM);
    } else {
        run_bench(NN_BENCH_MEM_OCRAM);
        run_bench(NN_BENCH_MEM_PSRAM);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_nn_bench, nn_bench, run NMSIS-NN A/B benchmark);

static int cmd_cpuclk(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    cpuclk_report();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cpuclk, cpuclk, measure and cross-check the core clock);
#endif

static void app_task(void *arg)
{
    (void)arg;

    vTaskDelay(pdMS_TO_TICKS(500));
    cpuclk_report();
    run_bench(NN_BENCH_MEM_OCRAM);
    run_bench(NN_BENCH_MEM_PSRAM);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    ipro_platform_init();
    xTaskCreate(app_task, "nn_bench", 4096, NULL, 5, NULL);
    vTaskStartScheduler();
    while (1) {
    }
}
