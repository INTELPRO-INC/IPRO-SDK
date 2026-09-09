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
 * @brief TinyMaix Neural Network Inference Demo for IPRO7
 *
 * Demonstrates TinyMaix INT8 inference with MNIST digit recognition.
 * Shell commands: nn_info, nn_run, nn_benchmark
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

#include "tinymaix.h"
#include "src/model_data.h"
#include "src/mnist_sample.h"

/* ---------------------------------------------------------------------------
 * RISC-V cycle counter for benchmarking
 * ---------------------------------------------------------------------------*/
static inline uint32_t get_cycles(void)
{
    uint32_t cycles;
    __asm__ volatile("csrr %0, mcycle" : "=r"(cycles));
    return cycles;
}

/* Approximate CPU frequency — adjust if different */
#define CPU_MHZ 320

/* ---------------------------------------------------------------------------
 * TinyMaix inference helpers
 * ---------------------------------------------------------------------------*/
static tm_err_t layer_cb(tm_mdl_t *mdl, tml_head_t *lh)
{
    (void)mdl;
    (void)lh;
    return TM_OK;
}

static int run_inference(int print_results)
{
    tm_mdl_t mdl;
    tm_mat_t in_uint8 = {3, 28, 28, 1, {(mtype_t *)mnist_pic}};
    tm_mat_t in = {3, 28, 28, 1, {NULL}};
    tm_mat_t outs[1];
    tm_err_t res;

    res = tm_load(&mdl, mdl_data, NULL, layer_cb, &in);
    if (res != TM_OK) {
        printf("Model load error: %d\r\n", res);
        return -1;
    }

    res = tm_preprocess(&mdl, TMPP_UINT2INT, &in_uint8, &in);
    if (res != TM_OK) {
        printf("Preprocess error: %d\r\n", res);
        tm_unload(&mdl);
        return -1;
    }

    uint32_t t0 = get_cycles();
    res = tm_run(&mdl, &in, outs);
    uint32_t t1 = get_cycles();

    if (res != TM_OK) {
        printf("Inference error: %d\r\n", res);
        tm_unload(&mdl);
        return -1;
    }

    if (print_results) {
        uint32_t us = (t1 - t0) / CPU_MHZ;
        printf("Inference time: %lu us (%.1f ms)\r\n",
               (unsigned long)us, (float)us / 1000.0f);

        tm_mat_t out = outs[0];
        float *data = out.dataf;
        float maxp = 0;
        int maxi = -1;
        printf("\r\nDigit probabilities:\r\n");
        for (int i = 0; i < 10; i++) {
            printf("  [%d]: %.4f%s\r\n", i, data[i],
                   (data[i] > maxp) ? " <--" : "");
            if (data[i] > maxp) {
                maxi = i;
                maxp = data[i];
            }
        }
        printf("\r\nPrediction: digit %d (confidence %.3f)\r\n", maxi, maxp);
    }

    tm_unload(&mdl);
    return (int)((t1 - t0) / CPU_MHZ);  /* return time in us */
}

/* ---------------------------------------------------------------------------
 * Shell commands
 * ---------------------------------------------------------------------------*/
#ifdef CONFIG_SHELL

static int cmd_nn_info(int argc, char **argv)
{
    (void)argc; (void)argv;
    tm_mdlbin_t *bin = (tm_mdlbin_t *)mdl_data;

    printf("\r\n=== TinyMaix Model Info ===\r\n");
    printf("Model type:  INT8 (quantized)\r\n");
    printf("Input:       %dx%dx%d (dims=%d)\r\n",
           bin->in_dims[1], bin->in_dims[2], bin->in_dims[3], bin->in_dims[0]);
    printf("Output:      %dx%dx%d (dims=%d)\r\n",
           bin->out_dims[1], bin->out_dims[2], bin->out_dims[3], bin->out_dims[0]);
    printf("Layers:      %d\r\n", bin->layer_cnt);
    printf("Buffer size: %lu bytes\r\n", (unsigned long)bin->buf_size);
    printf("Sub-buffer:  %lu bytes\r\n", (unsigned long)bin->sub_size);
    printf("Model data:  %d bytes\r\n", (int)sizeof(mdl_data));

#if TM_ENABLE_STAT
    printf("\r\nLayer details:\r\n");
    tm_stat(bin);
#endif

    printf("===========================\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_nn_info, nn_info, Show neural network model info);

static int cmd_nn_run(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("\r\n=== MNIST Inference ===\r\n");
    printf("Input: 28x28 grayscale image of digit '2'\r\n\r\n");
    run_inference(1);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_nn_run, nn_run, Run neural network inference);

static int cmd_nn_benchmark(int argc, char **argv)
{
    (void)argc; (void)argv;
    int n = 100;
    if (argc > 1) {
        n = atoi(argv[1]);
        if (n <= 0 || n > 10000) n = 100;
    }

    printf("\r\n=== NN Benchmark (%d iterations) ===\r\n", n);

    uint32_t total_us = 0;
    uint32_t min_us = UINT32_MAX;
    uint32_t max_us = 0;

    for (int i = 0; i < n; i++) {
        int us = run_inference(0);
        if (us < 0) {
            printf("Inference failed at iteration %d\r\n", i);
            return -1;
        }
        total_us += (uint32_t)us;
        if ((uint32_t)us < min_us) min_us = (uint32_t)us;
        if ((uint32_t)us > max_us) max_us = (uint32_t)us;

        if ((i + 1) % 10 == 0) {
            printf("  %d/%d done...\r\n", i + 1, n);
        }
    }

    uint32_t avg_us = total_us / (uint32_t)n;
    printf("\r\nResults:\r\n");
    printf("  Iterations: %d\r\n", n);
    printf("  Average:    %lu us (%.1f ms)\r\n",
           (unsigned long)avg_us, (float)avg_us / 1000.0f);
    printf("  Min:        %lu us (%.1f ms)\r\n",
           (unsigned long)min_us, (float)min_us / 1000.0f);
    printf("  Max:        %lu us (%.1f ms)\r\n",
           (unsigned long)max_us, (float)max_us / 1000.0f);
    printf("  Throughput: %.1f inferences/sec\r\n",
           avg_us > 0 ? 1000000.0f / (float)avg_us : 0.0f);
    printf("================================\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_nn_benchmark, nn_benchmark, Benchmark NN inference [count]);

#endif /* CONFIG_SHELL */

/* ---------------------------------------------------------------------------
 * Application entry point
 * ---------------------------------------------------------------------------*/
int main(void)
{
    ipro_platform_init();

    printf("\r\n========================================\r\n");
    printf("  IPRO7 TinyMaix Demo\r\n");
    printf("  Model: MNIST INT8 (digit recognition)\r\n");
    printf("  Commands: nn_info, nn_run, nn_benchmark\r\n");
    printf("========================================\r\n\r\n");

    /* Auto-run one inference at boot */
    printf("[Auto] Running MNIST inference...\r\n");
    run_inference(1);

    /* Main loop — shell handles commands */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
