/**
 * @file dsp_demo.c
 * @brief NMSIS-DSP Library Demo for IPRO7
 *
 * Demonstrates DSP library functions:
 *   - FFT (Real FFT)
 *   - Matrix Operations (Multiply, Transpose)
 *   - FIR Filter
 *   - Statistics (Mean, Std, Max, Min)
 *
 * Commands:
 *   dsp_fft [size]      - FFT demo (default 256 points)
 *   dsp_mat [size]      - Matrix multiply demo (default 4x4)
 *   dsp_fir [len]       - FIR filter demo (default 64 taps)
 *   dsp_stat [size]     - Statistics demo (default 256 samples)
 *   dsp_all             - Run all demos
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 IPRO Technology Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <FreeRTOS.h>
#include <task.h>
#include <shell.h>

#include "riscv_math.h"
#include "dsp/transform_functions.h"
#include "dsp/matrix_functions.h"
#include "dsp/filtering_functions.h"
#include "dsp/statistics_functions.h"
#include "dsp/basic_math_functions.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

/* ============================================================================
 * FFT Demo
 * ============================================================================ */

/**
 * @brief FFT Demo - Generate sine wave and compute FFT
 */
static int cmd_dsp_fft(int argc, char **argv)
{
    uint32_t fft_size = 256;

    if (argc > 1) {
        fft_size = (uint32_t)strtoul(argv[1], NULL, 0);
    }

    /* FFT size must be power of 2 and <= 4096 */
    if (fft_size < 32 || fft_size > 4096 || (fft_size & (fft_size - 1)) != 0) {
        printf("Error: FFT size must be power of 2 (32-4096)\n");
        return -1;
    }

    printf("\n=== DSP FFT Demo ===\n");
    printf("FFT size: %lu points\n", (unsigned long)fft_size);

    /* Allocate buffers */
    float32_t *input = (float32_t *)pvPortMalloc(fft_size * sizeof(float32_t));
    float32_t *output = (float32_t *)pvPortMalloc(fft_size * sizeof(float32_t));

    if (!input || !output) {
        printf("Error: Memory allocation failed\n");
        vPortFree(input);
        vPortFree(output);
        return -1;
    }

    /* Generate test signal: sum of 2 sine waves */
    /* f1 = fs/8 (bin 32 for 256-point FFT) */
    /* f2 = fs/4 (bin 64 for 256-point FFT) */
    float32_t freq1 = (float32_t)fft_size / 8.0f;
    float32_t freq2 = (float32_t)fft_size / 4.0f;

    printf("Generating test signal: sin(2*pi*%.0f*n/N) + 0.5*sin(2*pi*%.0f*n/N)\n",
           freq1, freq2);

    for (uint32_t i = 0; i < fft_size; i++) {
        float32_t t = (float32_t)i / (float32_t)fft_size;
        input[i] = sinf(2.0f * M_PI * freq1 * t) +
                   0.5f * sinf(2.0f * M_PI * freq2 * t);
    }

    /* Initialize FFT instance */
    riscv_rfft_fast_instance_f32 fft_inst;
    riscv_status status = riscv_rfft_fast_init_f32(&fft_inst, fft_size);
    if (status != RISCV_MATH_SUCCESS) {
        printf("Error: FFT init failed (status=%d)\n", status);
        vPortFree(input);
        vPortFree(output);
        return -1;
    }

    /* Compute FFT */
    uint32_t start_tick = (uint32_t)xTaskGetTickCount();
    riscv_rfft_fast_f32(&fft_inst, input, output, 0);
    uint32_t end_tick = (uint32_t)xTaskGetTickCount();

    printf("FFT computation time: %lu ms\n",
           (unsigned long)((end_tick - start_tick) * portTICK_PERIOD_MS));

    /* Compute magnitude spectrum */
    printf("\nMagnitude spectrum (top 5 peaks):\n");
    printf("  Bin    Freq(norm)   Magnitude\n");

    /* Find top 5 peaks */
    typedef struct { uint32_t bin; float32_t mag; } peak_t;
    peak_t peaks[5] = {0};

    for (uint32_t k = 1; k < fft_size / 2; k++) {
        float32_t re = output[2 * k];
        float32_t im = output[2 * k + 1];
        float32_t mag = sqrtf(re * re + im * im) * 2.0f / fft_size;

        /* Insert into sorted peaks array */
        for (int p = 0; p < 5; p++) {
            if (mag > peaks[p].mag) {
                for (int q = 4; q > p; q--) {
                    peaks[q] = peaks[q - 1];
                }
                peaks[p].bin = k;
                peaks[p].mag = mag;
                break;
            }
        }
    }

    for (int p = 0; p < 5 && peaks[p].mag > 0.01f; p++) {
        printf("  %4lu   %8.4f     %8.4f\n",
               (unsigned long)peaks[p].bin,
               (float)peaks[p].bin / fft_size,
               peaks[p].mag);
    }

    vPortFree(input);
    vPortFree(output);

    printf("\nFFT demo complete.\n");
    return 0;
}

/* ============================================================================
 * Matrix Demo
 * ============================================================================ */

/**
 * @brief Matrix Demo - Matrix multiplication
 */
static int cmd_dsp_mat(int argc, char **argv)
{
    uint32_t mat_size = 4;

    if (argc > 1) {
        mat_size = (uint32_t)strtoul(argv[1], NULL, 0);
    }

    if (mat_size < 2 || mat_size > 32) {
        printf("Error: Matrix size must be 2-32\n");
        return -1;
    }

    printf("\n=== DSP Matrix Demo ===\n");
    printf("Matrix size: %lux%lu\n", (unsigned long)mat_size, (unsigned long)mat_size);

    uint32_t total_elements = mat_size * mat_size;

    /* Allocate matrices */
    float32_t *matA = (float32_t *)pvPortMalloc(total_elements * sizeof(float32_t));
    float32_t *matB = (float32_t *)pvPortMalloc(total_elements * sizeof(float32_t));
    float32_t *matC = (float32_t *)pvPortMalloc(total_elements * sizeof(float32_t));

    if (!matA || !matB || !matC) {
        printf("Error: Memory allocation failed\n");
        vPortFree(matA);
        vPortFree(matB);
        vPortFree(matC);
        return -1;
    }

    /* Initialize matrices */
    for (uint32_t i = 0; i < total_elements; i++) {
        matA[i] = (float32_t)(i % mat_size) + 1.0f;
        matB[i] = (float32_t)(i / mat_size) + 1.0f;
    }

    /* Initialize matrix instances */
    riscv_matrix_instance_f32 A, B, C;
    riscv_mat_init_f32(&A, mat_size, mat_size, matA);
    riscv_mat_init_f32(&B, mat_size, mat_size, matB);
    riscv_mat_init_f32(&C, mat_size, mat_size, matC);

    /* Matrix multiply: C = A * B */
    printf("Computing C = A * B\n");

    uint32_t start_tick = (uint32_t)xTaskGetTickCount();
    riscv_status status = riscv_mat_mult_f32(&A, &B, &C);
    uint32_t end_tick = (uint32_t)xTaskGetTickCount();

    if (status != RISCV_MATH_SUCCESS) {
        printf("Error: Matrix multiply failed (status=%d)\n", status);
        vPortFree(matA);
        vPortFree(matB);
        vPortFree(matC);
        return -1;
    }

    printf("Computation time: %lu ms\n",
           (unsigned long)((end_tick - start_tick) * portTICK_PERIOD_MS));

    /* Print result (for small matrices) */
    if (mat_size <= 6) {
        printf("\nMatrix A:\n");
        for (uint32_t i = 0; i < mat_size; i++) {
            printf("  ");
            for (uint32_t j = 0; j < mat_size; j++) {
                printf("%6.1f ", matA[i * mat_size + j]);
            }
            printf("\n");
        }

        printf("\nMatrix B:\n");
        for (uint32_t i = 0; i < mat_size; i++) {
            printf("  ");
            for (uint32_t j = 0; j < mat_size; j++) {
                printf("%6.1f ", matB[i * mat_size + j]);
            }
            printf("\n");
        }

        printf("\nResult C = A * B:\n");
        for (uint32_t i = 0; i < mat_size; i++) {
            printf("  ");
            for (uint32_t j = 0; j < mat_size; j++) {
                printf("%6.1f ", matC[i * mat_size + j]);
            }
            printf("\n");
        }
    } else {
        printf("Result C[0][0] = %.2f, C[n-1][n-1] = %.2f\n",
               matC[0], matC[total_elements - 1]);
    }

    vPortFree(matA);
    vPortFree(matB);
    vPortFree(matC);

    printf("\nMatrix demo complete.\n");
    return 0;
}

/* ============================================================================
 * FIR Filter Demo
 * ============================================================================ */

/**
 * @brief FIR Filter Demo - Low-pass FIR filter
 */
static int cmd_dsp_fir(int argc, char **argv)
{
    uint32_t num_taps = 32;
    uint32_t block_size = 256;

    if (argc > 1) {
        num_taps = (uint32_t)strtoul(argv[1], NULL, 0);
    }

    if (num_taps < 4 || num_taps > 128) {
        printf("Error: Number of taps must be 4-128\n");
        return -1;
    }

    printf("\n=== DSP FIR Filter Demo ===\n");
    printf("Filter taps: %lu\n", (unsigned long)num_taps);
    printf("Block size:  %lu samples\n", (unsigned long)block_size);

    /* Allocate buffers */
    float32_t *coeffs = (float32_t *)pvPortMalloc(num_taps * sizeof(float32_t));
    float32_t *state = (float32_t *)pvPortMalloc((num_taps + block_size - 1) * sizeof(float32_t));
    float32_t *input = (float32_t *)pvPortMalloc(block_size * sizeof(float32_t));
    float32_t *output = (float32_t *)pvPortMalloc(block_size * sizeof(float32_t));

    if (!coeffs || !state || !input || !output) {
        printf("Error: Memory allocation failed\n");
        vPortFree(coeffs);
        vPortFree(state);
        vPortFree(input);
        vPortFree(output);
        return -1;
    }

    /* Design simple low-pass FIR filter (sinc windowed by Hamming) */
    float32_t fc = 0.2f;  /* Normalized cutoff frequency */
    int M = num_taps - 1;

    printf("Designing low-pass filter (fc=%.2f, Hamming window)\n", fc);

    for (uint32_t n = 0; n < num_taps; n++) {
        float32_t x = (float32_t)n - (float32_t)M / 2.0f;
        float32_t sinc, window;

        /* Sinc function */
        if (fabsf(x) < 0.0001f) {
            sinc = 2.0f * fc;
        } else {
            sinc = sinf(2.0f * M_PI * fc * x) / (M_PI * x);
        }

        /* Hamming window */
        window = 0.54f - 0.46f * cosf(2.0f * M_PI * n / M);

        coeffs[n] = sinc * window;
    }

    /* Generate test signal: sum of low and high frequency components */
    printf("Test signal: sin(0.05*2pi*n) + sin(0.4*2pi*n)\n");
    printf("             (low freq: 0.05, high freq: 0.4, cutoff: %.2f)\n", fc);

    for (uint32_t i = 0; i < block_size; i++) {
        input[i] = sinf(2.0f * M_PI * 0.05f * i) +  /* Low freq - should pass */
                   sinf(2.0f * M_PI * 0.40f * i);   /* High freq - should be attenuated */
    }

    /* Initialize FIR filter */
    riscv_fir_instance_f32 fir_inst;
    riscv_fir_init_f32(&fir_inst, num_taps, coeffs, state, block_size);

    /* Apply filter */
    uint32_t start_tick = (uint32_t)xTaskGetTickCount();
    riscv_fir_f32(&fir_inst, input, output, block_size);
    uint32_t end_tick = (uint32_t)xTaskGetTickCount();

    printf("Filter computation time: %lu ms\n",
           (unsigned long)((end_tick - start_tick) * portTICK_PERIOD_MS));

    /* Compute input/output statistics */
    float32_t in_max, in_min, out_max, out_min;
    uint32_t in_max_idx, in_min_idx, out_max_idx, out_min_idx;

    riscv_max_f32(input, block_size, &in_max, &in_max_idx);
    riscv_min_f32(input, block_size, &in_min, &in_min_idx);
    riscv_max_f32(output, block_size, &out_max, &out_max_idx);
    riscv_min_f32(output, block_size, &out_min, &out_min_idx);

    printf("\nSignal statistics:\n");
    printf("  Input  range: [%.3f, %.3f]\n", in_min, in_max);
    printf("  Output range: [%.3f, %.3f]\n", out_min, out_max);
    printf("  High-freq attenuation: %.1f dB (approx)\n",
           20.0f * log10f((in_max - in_min) / (out_max - out_min + 0.001f)));

    vPortFree(coeffs);
    vPortFree(state);
    vPortFree(input);
    vPortFree(output);

    printf("\nFIR filter demo complete.\n");
    return 0;
}

/* ============================================================================
 * Statistics Demo
 * ============================================================================ */

/**
 * @brief Statistics Demo - Mean, Std, Max, Min, Variance
 */
static int cmd_dsp_stat(int argc, char **argv)
{
    uint32_t size = 256;

    if (argc > 1) {
        size = (uint32_t)strtoul(argv[1], NULL, 0);
    }

    if (size < 16 || size > 10000) {
        printf("Error: Size must be 16-10000\n");
        return -1;
    }

    printf("\n=== DSP Statistics Demo ===\n");
    printf("Sample size: %lu\n", (unsigned long)size);

    /* Allocate buffer */
    float32_t *data = (float32_t *)pvPortMalloc(size * sizeof(float32_t));

    if (!data) {
        printf("Error: Memory allocation failed\n");
        return -1;
    }

    /* Generate test data: Gaussian-like distribution using sum of uniform */
    printf("Generating pseudo-Gaussian data (mean~50, std~10)\n");

    for (uint32_t i = 0; i < size; i++) {
        float32_t sum = 0.0f;
        for (int j = 0; j < 12; j++) {
            sum += (float32_t)(rand() % 1000) / 1000.0f;
        }
        data[i] = (sum - 6.0f) * 10.0f + 50.0f;  /* Transform to mean=50, std~10 */
    }

    /* Compute statistics using DSP library */
    uint32_t start_tick = (uint32_t)xTaskGetTickCount();

    float32_t mean, var, std_dev, rms;
    float32_t max_val, min_val;
    uint32_t max_idx, min_idx;

    riscv_mean_f32(data, size, &mean);
    riscv_var_f32(data, size, &var);
    riscv_std_f32(data, size, &std_dev);
    riscv_rms_f32(data, size, &rms);
    riscv_max_f32(data, size, &max_val, &max_idx);
    riscv_min_f32(data, size, &min_val, &min_idx);

    uint32_t end_tick = (uint32_t)xTaskGetTickCount();

    printf("Computation time: %lu ms\n",
           (unsigned long)((end_tick - start_tick) * portTICK_PERIOD_MS));

    printf("\nStatistics results:\n");
    printf("  Mean:      %.4f\n", mean);
    printf("  Variance:  %.4f\n", var);
    printf("  Std Dev:   %.4f\n", std_dev);
    printf("  RMS:       %.4f\n", rms);
    printf("  Max:       %.4f (at index %lu)\n", max_val, (unsigned long)max_idx);
    printf("  Min:       %.4f (at index %lu)\n", min_val, (unsigned long)min_idx);
    printf("  Range:     %.4f\n", max_val - min_val);

    /* Compute power (sum of squares) */
    float32_t power;
    riscv_power_f32(data, size, &power);
    printf("  Power:     %.4f\n", power);

    vPortFree(data);

    printf("\nStatistics demo complete.\n");
    return 0;
}

/* ============================================================================
 * Run All Demos
 * ============================================================================ */

static int cmd_dsp_all(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("\n");
    printf("========================================\n");
    printf("  NMSIS-DSP Library Demo Suite\n");
    printf("========================================\n");

    char *fft_argv[] = {"dsp_fft", "256"};
    cmd_dsp_fft(2, fft_argv);

    char *mat_argv[] = {"dsp_mat", "4"};
    cmd_dsp_mat(2, mat_argv);

    char *fir_argv[] = {"dsp_fir", "32"};
    cmd_dsp_fir(2, fir_argv);

    char *stat_argv[] = {"dsp_stat", "1000"};
    cmd_dsp_stat(2, stat_argv);

    printf("\n========================================\n");
    printf("  All DSP demos completed!\n");
    printf("========================================\n");

    return 0;
}

/* ============================================================================
 * Shell Command Registration
 * ============================================================================ */

SHELL_CMD_EXPORT_ALIAS(cmd_dsp_fft, dsp_fft, DSP FFT demo [size]);
SHELL_CMD_EXPORT_ALIAS(cmd_dsp_mat, dsp_mat, DSP Matrix multiply demo [size]);
SHELL_CMD_EXPORT_ALIAS(cmd_dsp_fir, dsp_fir, DSP FIR filter demo [taps]);
SHELL_CMD_EXPORT_ALIAS(cmd_dsp_stat, dsp_stat, DSP Statistics demo [size]);
SHELL_CMD_EXPORT_ALIAS(cmd_dsp_all, dsp_all, Run all DSP demos);
