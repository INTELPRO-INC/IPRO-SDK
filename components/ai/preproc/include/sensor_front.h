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
 * @file sensor_front.h
 * @brief A window of samples -> the network's 64x64x8 int8 input.
 *
 * The C twin of IPRO AI Studio's ipro_ai_core/sensor_teach.py FrontEnd; the
 * two must agree value for value (test_host/ checks it against numpy):
 *
 *  spectrogram  per channel, 64 frames of 128 samples at integer starts
 *               (t * (window - 128)) / 63; row 0 is asinh of the frame's
 *               mean (the level - orientation, offset - the FFT does not
 *               see); the frame minus its mean, times a symmetric Hann, FFT,
 *               log(|X| + 1) of bins 1..63 are rows 1..63; column = frame.
 *  wave         per channel, the window linearly resampled to 4096 points,
 *               folded row by row into 64x64.
 *
 * Then per channel (v - mean) / std, int8 = round(z / in_scale) + in_zp.
 * Channels past cfg->channels are left at in_zp. Pure C (math.h only).
 */
#ifndef SENSOR_FRONT_H
#define SENSOR_FRONT_H

#include <stdint.h>

#define SF_SIDE       64u
#define SF_MAX_CH     8u
#define SF_NFFT       128u
#define SF_WAVE_N     (SF_SIDE * SF_SIDE)
#define SF_IMG_BYTES  (SF_SIDE * SF_SIDE * SF_MAX_CH)   /* HWC, 8 channels per pixel */

typedef enum { SF_SPECTROGRAM = 0, SF_WAVE = 1 } sf_mode_t;

typedef struct {
    uint32_t channels;            /* 1..SF_MAX_CH */
    uint32_t window;              /* samples per channel */
    uint32_t mode;                /* sf_mode_t */
    float    mean[SF_MAX_CH];
    float    std[SF_MAX_CH];
    float    in_scale;
    int32_t  in_zp;
} sf_cfg_t;

/** Sample i (0 = oldest) of channel ch of the window. */
typedef float (*sf_sample_fn)(void *ctx, uint32_t ch, uint32_t i);

/** Fill img (SF_IMG_BYTES). 0, or -1 for a configuration it cannot do. */
int sensor_front(const sf_cfg_t *cfg, sf_sample_fn sample, void *ctx, int8_t *img);

#endif /* SENSOR_FRONT_H */
