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
 * @brief TFLM model-zoo benchmark on IPRO7.
 *
 * Four models spanning 288 to 7,157,888 MAC, timed identically. The spread is
 * the deliverable: it says where on the MAC axis the CPU path stops being
 * viable, which is the question a customer actually asks.
 *
 * Companion measurements:
 *   apps/test/ipro_tflm_yolo  YOLOv8, 461 M MAC, same interpreter  -> 9.5 s
 *   apps/test/ipro_npu_yolo   the same YOLOv8 compiled to .inm    -> 102 ms
 *
 * Which kernel backend is active is a build-time choice
 * (CONFIG_TFLM_NMSIS_NN_KERNELS); the banner reports it so two logs are
 * distinguishable.
 *
 * Shell: zoo [iters]
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <generated/autoconf.h>

#include <FreeRTOS.h>
#include <task.h>

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#include "freertos_app_hooks.h"

#include "src/model_zoo.h"

#define CPU_MHZ 192   /* measured 191.9988 MHz - see ipro_nmsis_nn_test cpuclk */

static void print_banner(void)
{
    printf("\r\nTFLM model zoo on IPRO7\r\n");
#ifdef CONFIG_TFLM_NMSIS_NN_KERNELS
    printf("  kernel backend  : NMSIS-NN\r\n");
#else
    printf("  kernel backend  : reference (portable C++)\r\n");
#endif
#if defined(__riscv_xxldspn2x)
    printf("  march DSP level : xxldspn2x\r\n");
#elif defined(__riscv_xxldsp)
    printf("  march DSP level : xxldsp\r\n");
#else
    printf("  march DSP level : NONE\r\n");
#endif
    printf("\r\n");
}

static void run_zoo(int iters)
{
    print_banner();

    printf("  %-18s %10s %11s %8s %14s %12s %10s\r\n",
           "model", "MAC", "warm cyc", "ms", "cold cyc", "cyc/MAC x100", "arena");
    printf("  %-18s %10s %11s %8s %14s %12s %10s\r\n",
           "------------------", "----------", "-----------", "--------",
           "--------------", "------------", "----------");

    for (int i = 0; i < zoo_count(); i++) {
        zoo_result_t r;
        if (!zoo_run(i, iters, &r)) {
            printf("  %-18s  FAILED\r\n", zoo_name(i));
            continue;
        }
        /* Sub-millisecond models are the interesting ones here, so report
         * microseconds too rather than a column of zeroes. */
        const uint32_t us = (uint32_t)(r.warm_cycles / CPU_MHZ);
        const uint32_t cpm100 = r.total_mac
                              ? (uint32_t)((r.warm_cycles * 100ULL) / r.total_mac)
                              : 0;
        printf("  %-18s %10llu %11llu %5lu.%02lu %14llu %12lu %10lu\r\n",
               r.name,
               (unsigned long long)r.total_mac,
               (unsigned long long)r.warm_cycles,
               (unsigned long)(us / 1000), (unsigned long)((us % 1000) / 10),
               (unsigned long long)r.cold_cycles,
               (unsigned long)cpm100,
               (unsigned long)r.arena_used);
    }

    printf("\r\n  cyc/MAC x100: 394 = 3.94 cycles per MAC. Small models are\r\n");
    printf("  interpreter-bound, so their figure says nothing about kernels.\r\n");
}

#ifdef CONFIG_SHELL
static int cmd_zoo(int argc, char **argv)
{
    int iters = (argc > 1) ? atoi(argv[1]) : 5;
    if (iters < 2)  iters = 2;    /* need at least one warm run */
    if (iters > 50) iters = 50;
    run_zoo(iters);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_zoo, zoo, run the TFLM model zoo benchmark [iters]);
#endif

static void app_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(500));
    run_zoo(5);
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    ipro_platform_init();
    xTaskCreate(app_task, "zoo", 8192, NULL, 5, NULL);
    vTaskStartScheduler();
    while (1) {}
}
