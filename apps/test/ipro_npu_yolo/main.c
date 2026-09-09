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
 * @brief YOLOv8 person/pet on the IPRO7 NPU - the NPU half of an NPU-vs-CPU
 *        comparison.
 *
 * person_pet_detect.inm is what ips_aitool produces from
 * yolov8_best_headless_6out_full_integer_quant.tflite. apps/test/ipro_tflm_yolo
 * runs that same .tflite through TFLM on the CPU. Same weights, same graph,
 * two execution paths - which is the only way to get an honest number for what
 * the NPU is worth.
 *
 * No post-processing here, matching the TFLM side: decoding boxes would
 * measure yolov8_person_pet_process(), not the model.
 *
 * With CONFIG_AI_CYCLE_PROFILE the report also splits NPU-busy from CPU-busy,
 * which is the concurrency budget a co-running TFLM task could use.
 *
 * Shell: npu_run [iters]
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <generated/autoconf.h>

#include <FreeRTOS.h>
#include <task.h>

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#include "freertos_app_hooks.h"

#include "iproai_def.h"
#include "iproai_core.h"
#include "iproai_inst_npu.h"
#include "iproai_cycle_profile.h"
#include "hal_npu.h"
#include "ipro_cycles.h"

#define CPU_MHZ 192
/* Same figure the TFLM app reports against, computed from the .tflite tensor
 * shapes: 461,120,000 MAC across 59 CONV_2D. */
#define YOLO_TOTAL_MAC 461120000ULL

extern const unsigned char g_inm_model[];
extern const unsigned char g_inm_model_end[];

/* ISP feeds 640x360 ARGB in the pircam app; the model's own dims are read back
 * after load and reported, so a mismatch shows up rather than silently
 * producing a fast-but-wrong number. Plain .bss => PSRAM on this build.
 * +4 for the size header the ext_input path expects at imgBuffer+0. */
#define NPU_IN_MAX (640 * 360 * 4 + 4)
static __attribute__((aligned(16))) uint8_t s_in_buf[NPU_IN_MAX];

/* components/ai reads the external input from imgBuffer+4; overriding the weak
 * definition in hal_npu.c is how speaker_embed.c does it too. */
char *imgBuffer = NULL;

static iproai_model_hdl_t s_hdl;

/* 64-bit: mcycle alone wraps every 22.4 s at 192 MHz, and a wrapped result
 * looks like a plausible fast number rather than an obvious error. */
#define rd_cycles() ipro_cycles64()

static int npu_setup(void)
{
    const uint32_t model_bytes = (uint32_t)(g_inm_model_end - g_inm_model);

    printf("\r\nYOLOv8 person/pet on IPRO7 NPU (.inm)\r\n");
    printf("  model           : %lu bytes (.inm, linked in)\r\n",
           (unsigned long)model_bytes);

    /* Fill before load: the ext_input branch in iproai_inst_cpu.c programs the
     * NPU's DM2 address from imgBuffer+4 while parsing the header. */
    int32_t seed = 20260821;
    for (size_t i = 0; i < sizeof(s_in_buf); i++) {
        seed = seed * 1103515245 + 12345;
        s_in_buf[i] = (uint8_t)(seed >> 16);
    }
    imgBuffer = (char *)s_in_buf;

    iproai_npu_init();

    s_hdl = iproai_create();
    if (s_hdl == NULL) {
        printf("  iproai_create failed\r\n");
        return -1;
    }

    /* The runtime copies what it needs out of this buffer, but it is already
     * resident in XIP flash so there is nothing to stage. */
    const uint64_t t0 = rd_cycles();
    const IPROAI_Status_e st =
        iproai_load_model_from_buffer(s_hdl, (const uint8_t *)g_inm_model);
    const uint64_t t1 = rd_cycles();

    if (st != IPROAI_STATUS_NO_ERROR) {
        printf("  load_model_from_buffer failed (status=%d)\r\n", (int)st);
        return -1;
    }
    printf("  load            : %llu cycles (%lu ms)\r\n",
           (unsigned long long)(t1 - t0),
           (unsigned long)((t1 - t0) / (CPU_MHZ * 1000)));

    struct iproai_net_info_t *net = iproai_getNetInfo(s_hdl);
    if (net == NULL) {
        printf("  getNetInfo failed\r\n");
        return -1;
    }

    printf("  input           : %dx%dx%d\r\n", net->w, net->h, net->c);
    printf("  layers          : %d total, %d NPU\r\n",
           net->layer_cnt, net->npu_layer_cnt);
    printf("  ext_input       : %d   use_npu_accel: %d\r\n",
           net->ext_input, net->use_npu_accel);

    const uint32_t need = (uint32_t)net->w * net->h * net->c;
    if (need + 4u > sizeof(s_in_buf)) {
        printf("  WARNING: model wants %lu bytes, buffer is %u\r\n",
               (unsigned long)need, (unsigned)sizeof(s_in_buf));
    }

    /* Point the NPU at the synthetic frame, the way pircam does per ISP frame. */
    NPU_Img_Ext_Addr_Cfg((uint32_t)(uintptr_t)(s_in_buf + 4));
    return 0;
}

static void run_npu(int iters)
{
    if (s_hdl == NULL && npu_setup() != 0) {
        printf("  SETUP FAILED\r\n");
        return;
    }

    printf("\r\n  %5s %14s %10s %14s\r\n", "run", "cycles", "ms", "cyc/MAC x100");
    printf("  %5s %14s %10s %14s\r\n", "-----", "--------------", "----------",
           "--------------");

    iproai_cycle_reset();

    uint64_t warm = 0;
    int warm_n = 0;
    for (int i = 0; i < iters; i++) {
        const uint64_t t0 = rd_cycles();
        const IPROAI_Status_e st = iproai_startCompute(s_hdl);
        const uint64_t t1 = rd_cycles();

        if (st != IPROAI_STATUS_NO_ERROR) {
            printf("  compute failed at run %d (status=%d)\r\n", i, (int)st);
            return;
        }
        const uint64_t c = t1 - t0;
        printf("  %5d %14llu %10lu %14lu\r\n", i, (unsigned long long)c,
               (unsigned long)(c / (CPU_MHZ * 1000)),
               (unsigned long)((c * 100ULL) / YOLO_TOTAL_MAC));
        if (i > 0) { warm += c; warm_n++; }
    }

    if (warm_n > 0) {
        const uint64_t mean = warm / (uint64_t)warm_n;
        const uint32_t ms = (uint32_t)(mean / (CPU_MHZ * 1000));
        printf("\r\n  warm mean       : %llu cycles = %lu ms",
               (unsigned long long)mean, (unsigned long)ms);
        if (ms > 0) {
            printf(" (%lu fps)", (unsigned long)(1000u / ms));
        }
        printf("\r\n");
    }

    printf("\r\n");
    iproai_cycle_report();
}

#ifdef CONFIG_SHELL
static int cmd_npu_run(int argc, char **argv)
{
    int iters = (argc > 1) ? atoi(argv[1]) : 5;
    if (iters < 1) iters = 1;
    if (iters > 100) iters = 100;
    run_npu(iters);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_npu_run, npu_run, run YOLOv8 on the NPU [iters]);
#endif

static void app_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(500));
    run_npu(5);
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    ipro_platform_init();
    xTaskCreate(app_task, "npu_yolo", 8192, NULL, 5, NULL);
    vTaskStartScheduler();
    while (1) {}
}
