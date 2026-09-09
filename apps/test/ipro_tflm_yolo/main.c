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
 * @brief YOLOv8 on the IPRO7 CPU via TFLM - the CPU half of an NPU comparison.
 *
 * The same yolov8_..._full_integer_quant.tflite that ips_aitool compiles into
 * person_pet_detect.inm (which runs 99% on the NPU) is run here through the
 * TFLM interpreter with NMSIS-NN kernels. Comparing the two answers "what does
 * the NPU actually buy" with one model rather than two.
 *
 * 461,120,000 MAC across 59 CONV_2D layers, so expect this to be slow - single
 * digit seconds per inference at best. That number is the point.
 *
 * Shell: yolo_run [iters]
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

#include "src/yolo_runner.h"

#define CPU_MHZ 192   /* measured 191.9988 MHz; see ipro_nmsis_nn_test cpuclk */

/* 461.12 M MAC, computed from the flatbuffer's tensor shapes. Used only to
 * report cycles/MAC, which is the figure that transfers to other models. */
#define YOLO_TOTAL_MAC 461120000ULL

static void print_banner(void)
{
    printf("\r\nYOLOv8 person/pet on IPRO7 CPU (TFLM)\r\n");
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
}

static void run_yolo(int iters)
{
    print_banner();

    yolo_info_t info;
    if (!yolo_init(&info)) {
        printf("  INIT FAILED\r\n");
        return;
    }

    printf("  model           : %lu bytes (.tflite, linked in)\r\n",
           (unsigned long)info.model_bytes);
    printf("  input           : %ldx%ldx%ld int8\r\n",
           (long)info.in_h, (long)info.in_w, (long)info.in_c);
    printf("  outputs         : %lu heads\r\n", (unsigned long)info.n_outputs);
    printf("  arena used      : %lu / %lu bytes (%lu KB)\r\n",
           (unsigned long)info.arena_used, (unsigned long)info.arena_size,
           (unsigned long)(info.arena_used / 1024));
    printf("  load+alloc      : %llu cycles (%lu ms)\r\n",
           (unsigned long long)info.load_cycles,
           (unsigned long)(info.load_cycles / (CPU_MHZ * 1000)));

    printf("\r\n  %5s %16s %12s %14s\r\n", "run", "cycles", "ms", "cyc/MAC x100");
    printf("  %5s %16s %12s %14s\r\n", "-----", "----------------", "------------",
           "--------------");

    uint64_t warm_total = 0;
    int warm_n = 0;
    for (int i = 0; i < iters; i++) {
        yolo_result_t r;
        if (!yolo_infer(&r)) {
            printf("  invoke failed at run %d\r\n", i);
            return;
        }
        const uint32_t ms = (uint32_t)(r.cycles / (CPU_MHZ * 1000));
        const uint32_t cpm100 = (uint32_t)((r.cycles * 100ULL) / YOLO_TOTAL_MAC);
        printf("  %5d %16llu %12lu %14lu\r\n", i,
               (unsigned long long)r.cycles, (unsigned long)ms, (unsigned long)cpm100);

        if (i == 0) {
            printf("        head checksums:");
            for (unsigned h = 0; h < r.n_heads; h++) {
                printf(" [%lu]%ld", (unsigned long)r.head_bytes[h],
                       (long)r.head_sum[h]);
            }
            printf("\r\n");
        }
        /* Run 0 pays the I-cache fill; it is reported but not averaged. */
        if (i > 0) { warm_total += r.cycles; warm_n++; }
    }

    if (warm_n > 0) {
        const uint64_t mean = warm_total / (uint64_t)warm_n;
        const uint32_t ms = (uint32_t)(mean / (CPU_MHZ * 1000));
        printf("\r\n  warm mean       : %llu cycles = %lu ms",
               (unsigned long long)mean, (unsigned long)ms);
        if (ms > 0) {
            /* fps x100, so a sub-1 fps result still shows something useful */
            const uint32_t fps100 = (uint32_t)(100000ULL / ms);
            printf(" (%lu.%02lu fps)", (unsigned long)(fps100 / 100),
                   (unsigned long)(fps100 % 100));
        }
        printf("\r\n");
        const uint32_t cpm100 = (uint32_t)((mean * 100ULL) / YOLO_TOTAL_MAC);
        printf("  efficiency      : %lu.%02lu cycles/MAC over %llu MAC\r\n",
               (unsigned long)(cpm100 / 100), (unsigned long)(cpm100 % 100),
               (unsigned long long)YOLO_TOTAL_MAC);
    }
}

#ifdef CONFIG_SHELL
static int cmd_yolo_run(int argc, char **argv)
{
    int iters = (argc > 1) ? atoi(argv[1]) : 3;
    if (iters < 1) iters = 1;
    if (iters > 20) iters = 20;
    run_yolo(iters);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_yolo_run, yolo_run, run YOLOv8 on the CPU [iters]);
#endif

static void app_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(500));
    run_yolo(3);
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    ipro_platform_init();
    /* TFLM's Invoke() recurses through the graph; 16 KB words is generous but
     * a stack overflow 6 seconds into a run would be an expensive way to find
     * out it was not. */
    xTaskCreate(app_task, "yolo", 16384, NULL, 5, NULL);
    vTaskStartScheduler();
    while (1) {}
}
