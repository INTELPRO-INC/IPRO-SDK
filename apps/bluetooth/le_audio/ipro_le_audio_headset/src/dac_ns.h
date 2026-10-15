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
 * @file dac_ns.h
 * @brief 16x oversampling noise shaper for the 10-bit GPDAC
 *
 * The GPDAC is 10 bits. Fed at 32 kHz, its quantisation noise is white over
 * 0-16 kHz and sits about 60 dB under full scale - an audible hiss in
 * headphones. The DAC also runs at 512 kHz in DMA mode (16x 32 kHz), so this
 * module interpolates each 32 kHz sample into 16 DAC codes and quantises them
 * with error feedback. The noise moves above the audio band, where the
 * board's RC filter, the headphone and the ear take it out:
 *
 *   order 1   NTF (1 - z^-1)      in-band A-weighted noise about -38 dB
 *   order 2   NTF (1 - z^-1)^2    about -55 dB, twice the noise above 20 kHz
 *
 * (offline model of the whole chain; the DAC's own analogue noise and
 * nonlinearity are not in it, and will be the floor in practice.)
 *
 * Integer only: the IPRO7 FreeRTOS port does not save FPU registers, and
 * this runs in its own task beside the LC3 decoder. On the target the loop
 * runs from OCRAM (.tcm_code), so its speed does not depend on flash cache
 * misses.
 */

#ifndef DAC_NS_H
#define DAC_NS_H

#include <stdint.h>

/* Output samples per 32 kHz input sample (512 kHz / 32 kHz). */
#define DAC_NS_OSR          16

/*
 * Input scale: an int16-domain sample x means DAC code 512 + x / 16, the
 * same scale as app_dac.c's DAC_GAIN_SHIFT 4. Inputs are clipped by the
 * caller to +-DAC_NS_TARGET_MAX. The quantiser error stays under one code,
 * so the fed-back correction is under three codes for order 2 and the
 * output never reaches the rails: 8 codes of headroom is enough without any
 * clamp in the loop (a clamped error-feedback loop goes unstable anyway).
 */
#define DAC_NS_IN_SHIFT     4
#define DAC_NS_TARGET_MAX   ((512 - 8) << DAC_NS_IN_SHIFT)

typedef struct {
    int32_t  prev;      /* last input sample, the interpolation start point */
    int32_t  e1, e2;    /* quantiser error history, in 1/256 code */
    uint32_t rng;       /* dither generator state */
    uint8_t  order;     /* 1 or 2 */
} dac_ns_t;

/** @brief Reset to silence. @p order 1 or 2 (anything else means 1). */
void dac_ns_reset(dac_ns_t *ns, uint8_t order);

/**
 * @brief Expand @p n 32 kHz input samples into n * DAC_NS_OSR DAC codes.
 *
 * Linear interpolation from each previous input to the next, then
 * quantisation to 0..1023 with rectangular dither and error feedback.
 * Output lags input by one 32 kHz sample. Inputs must already be within
 * +-DAC_NS_TARGET_MAX; nothing is clamped inside, so the loop stays short
 * (the first version, with clamps, xorshift TPDF and one call per input
 * sample from flash, cost 81 cycles per output sample - a fifth of the CPU
 * at 512 kHz - and the LE Audio stream lost 13% of its ISO packets).
 */
void dac_ns_run(dac_ns_t *ns, const int16_t *in, uint32_t n, uint16_t *out);

#endif /* DAC_NS_H */
