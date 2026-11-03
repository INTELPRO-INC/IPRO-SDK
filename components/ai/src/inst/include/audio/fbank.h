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

#ifndef FBANK_H
#define FBANK_H

#include <stdint.h>
#include "generated/autoconf.h"   /* CONFIG_AI_FBANK_INT (per-app .config) */

/* Feature dimensions — must match training config */
#define FBANK_SEQ_LEN     300     /* frames  (3s @ 100fps)              */
#define FBANK_N_MELS       80     /* mel bins                           */
#define FBANK_SAMPLE_RATE 16000   /* Hz                                 */
#define FBANK_FRAME_LEN   400     /* samples (25ms @ 16kHz)             */
#define FBANK_FRAME_SHIFT 160     /* samples (10ms @ 16kHz)             */
#define FBANK_FFT_SIZE    512     /* next power-of-2 >= FBANK_FRAME_LEN */
#define FBANK_N_BINS     (FBANK_FFT_SIZE / 2 + 1)  /* 257 bins         */
#define FBANK_LOW_FREQ    20.0f   /* Hz  (Kaldi default)                */
#define FBANK_HIGH_FREQ 8000.0f   /* Hz  (Nyquist for 16kHz)            */
#define FBANK_PREEMPH     0.97f   /* pre-emphasis coefficient           */

/* Pre-computed context (mel filter matrix + window).
   Call fbank_init() once at startup, then reuse.

   Two back-ends share this header and the same public API; the one compiled is
   selected by CONFIG_AI_FBANK_INT (per-app .config) so callers never change:
     - float  back-end -> fbank.c     (default)
     - integer back-end -> fbank_int.c (Q15/Q8 fixed point, DSP/xxldsp-friendly)
   The struct layout differs (float vs Q15 tables) but is opaque to callers,
   which only do pvPortMalloc(sizeof(FBankContext)) + fbank_init/fbank_compute. */
#if defined(CONFIG_AI_FBANK_INT)
typedef struct {
    int16_t window_q15[FBANK_FRAME_LEN];            /* Povey window, Q15   */
    int16_t mel_fb_q15[FBANK_N_MELS][FBANK_N_BINS]; /* mel filters, Q15    */
    int16_t mel_lo[FBANK_N_MELS];                   /* nonzero bin span    */
    int16_t mel_hi[FBANK_N_MELS];
} FBankContext;
#else
/* Every FFT bin lies under at most two of the triangles. */
#define FBANK_MEL_WEIGHTS (2 * FBANK_N_BINS)
typedef struct {
    float window[FBANK_FRAME_LEN];                  /* Povey window     */
    /* The mel filter bank, nonzero spans only: filter m's weights for bins
       mel_lo[m]..mel_hi[m] are mel_w[mel_off[m]] onwards. Each triangle covers
       a few contiguous bins, so this is 2 KB where the full 80 x 257 table was
       82 KB, and the dot product reads the same weights in the same order -
       bit-identical to summing the whole row (the rest is exactly 0). */
    float    mel_w[FBANK_MEL_WEIGHTS];
    int16_t  mel_lo[FBANK_N_MELS];
    int16_t  mel_hi[FBANK_N_MELS];
    uint16_t mel_off[FBANK_N_MELS];
} FBankContext;
#endif

/* Initialise context (compute window + mel filters). */
void fbank_init(FBankContext *ctx);

/*
 * Compute log mel-filterbank features.
 *
 * samples  : int16 PCM, mono, FBANK_SAMPLE_RATE Hz
 * n_samples: length of samples array
 * out      : output buffer [FBANK_SEQ_LEN][FBANK_N_MELS], row-major
 *
 * Frames shorter than SEQ_LEN are zero-padded.
 * Frames beyond SEQ_LEN are discarded.
 * Global mean normalisation is applied after all frames are computed.
 *
 * Returns number of valid frames extracted (before padding).
 */
int fbank_compute(const FBankContext *ctx,
                  const int16_t *samples, int n_samples,
                  float out[FBANK_SEQ_LEN][FBANK_N_MELS]);

#endif /* FBANK_H */
