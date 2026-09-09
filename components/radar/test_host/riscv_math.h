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
 * Host stub for the handful of NMSIS-DSP q15 calls kw307_spectro.c makes.
 *
 * A naive DFT in double, scaled the way CMSIS/NMSIS scales its q15 transform
 * (one halving per radix-2 stage, i.e. 1/N overall). The point is not to test
 * the vendor's FFT - it is to run OUR pipeline (decimation rate, detrend,
 * window, bin mapping, log compression) against a signal whose frequency is
 * known, on a machine where floating point is free.
 */
#ifndef RISCV_MATH_HOST_STUB_H
#define RISCV_MATH_HOST_STUB_H

#include <math.h>
#include <stdint.h>

#define RISCV_MATH_SUCCESS 0

typedef struct { uint32_t fftLenReal; } riscv_rfft_instance_q15;

static inline int riscv_rfft_init_q15(riscv_rfft_instance_q15 *S,
                                      uint32_t fftLenReal,
                                      uint32_t ifftFlagR, uint32_t bitReverseFlag)
{
    (void)ifftFlagR; (void)bitReverseFlag;
    S->fftLenReal = fftLenReal;
    return RISCV_MATH_SUCCESS;
}

static inline void riscv_rfft_q15(const riscv_rfft_instance_q15 *S,
                                  int16_t *pSrc, int16_t *pDst)
{
    const uint32_t n = S->fftLenReal;
    for (uint32_t k = 0; k <= n / 2; k++) {
        double re = 0.0, im = 0.0;
        for (uint32_t t = 0; t < n; t++) {
            const double a = -2.0 * M_PI * (double)k * (double)t / (double)n;
            re += (double)pSrc[t] * cos(a);
            im += (double)pSrc[t] * sin(a);
        }
        /* Match the q15 transform's 1/N growth control. */
        re /= (double)n;
        im /= (double)n;
        if (k < n / 2) {
            pDst[2 * k]     = (int16_t)re;
            pDst[2 * k + 1] = (int16_t)im;
        }
    }
}

static inline void riscv_cmplx_mag_q15(const int16_t *pSrc, int16_t *pDst,
                                       uint32_t numSamples)
{
    for (uint32_t i = 0; i < numSamples; i++) {
        const double re = pSrc[2 * i], im = pSrc[2 * i + 1];
        double m = sqrt(re * re + im * im);
        if (m > 32767.0) m = 32767.0;
        pDst[i] = (int16_t)m;
    }
}

#endif
