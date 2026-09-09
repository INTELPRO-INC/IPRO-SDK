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
 * Magnitude spectrogram - the dense 2D representation, and the NPU input.
 *
 * WHY NOT RANGE-TIME. The obvious 2D picture from a radar is range against
 * time, and kw307_map.h builds one. On this module it is almost empty: the
 * part reports a tracked point, not a range profile, so exactly one range bin
 * is lit per column and a 32-row map runs at about 2% occupancy. Measured on
 * hardware 2026-08-31. It is a fine picture of a trajectory and a poor tensor.
 *
 * The information that is actually there sits in how the magnitude of that one
 * track FLUCTUATES - breathing, postural sway, limb motion. A short-time
 * Fourier transform turns that fluctuation into a time-frequency image where
 * every cell carries energy, which is both the standard representation for
 * radar activity classification and a tensor worth spending an NPU on.
 *
 * TENSOR CONTRACT:
 *   shape   KW307_SPECTRO_BINS x KW307_SPECTRO_STEPS, uint8, row-major
 *           row = frequency bin, LOW at the top (slow motion first)
 *           column = time, oldest to newest
 *   value   log-compressed band energy, 0..255
 *
 * The same Conv2D constraint as kw307_map.h applies: convolve over this as an
 * image. A Conv1D with a non-square kernel does not map onto this NPU.
 */
#ifndef __KW307_SPECTRO_H__
#define __KW307_SPECTRO_H__

#include <stdbool.h>
#include <stdint.h>

#include "kw307_history.h"

/* The magnitude series is decimated 20 Hz -> 5 Hz before the transform. At the
 * full rate a 128-point FFT would spread 0..10 Hz over 64 bins and put the
 * entire breathing band inside the first three - the band of interest would be
 * quantised away. At 5 Hz the same transform covers 0..2.5 Hz at 0.039 Hz per
 * bin, so breathing (0.2..0.5 Hz) lands across bins 5..13 with room to move. */
#define KW307_SPECTRO_DECIMATE   4
#define KW307_SPECTRO_RATE_HZ    (KW307_HISTORY_RATE_HZ / KW307_SPECTRO_DECIMATE)

#define KW307_SPECTRO_FFT_N      128     /* 25.6 s per column at 5 Hz */
#define KW307_SPECTRO_HOP        8       /* 1.6 s between columns     */
#define KW307_SPECTRO_BINS       32      /* keep 0.039..1.25 Hz = 2.3..75 /min */
#define KW307_SPECTRO_STEPS      64
#define KW307_SPECTRO_CELLS      (KW307_SPECTRO_BINS * KW307_SPECTRO_STEPS)

/* History the transform needs, in samples at the native 20 Hz rate. */
#define KW307_SPECTRO_NEED_SAMPLES \
    ((KW307_SPECTRO_FFT_N + (KW307_SPECTRO_STEPS - 1) * KW307_SPECTRO_HOP) \
     * KW307_SPECTRO_DECIMATE)

typedef struct {
    uint16_t span_s;          /* seconds the image covers                   */
    uint16_t valid_samples;   /* of the source window, how many were Valid  */
    uint8_t  peak;
    uint8_t  peak_bin;
    uint16_t peak_mhz;        /* peak frequency in milli-hertz              */
} kw307_spectro_info_t;

/* Allocate the transform scratch (~2 KB from the PSRAM heap). Returns 0, or
 * negative when the heap cannot satisfy it or NMSIS-DSP is absent. */
int  kw307_spectro_init(void);
void kw307_spectro_free(void);

/* Build the spectrogram from the newest history. Returns false when there is
 * not yet KW307_SPECTRO_NEED_SAMPLES of it, or when the stationary slot was
 * Valid too rarely for the result to mean anything. */
bool kw307_spectro_build(uint8_t *out, kw307_spectro_info_t *info);

void kw307_spectro_print(const uint8_t *img, const kw307_spectro_info_t *info);
void kw307_spectro_dump_csv(const uint8_t *img, const kw307_spectro_info_t *info,
                            const char *label);

/* Frequency of a bin, in milli-hertz - the row labels and any later model
 * output both need it, so it is derived in one place. */
uint32_t kw307_spectro_bin_mhz(uint32_t bin);

#endif /* __KW307_SPECTRO_H__ */
