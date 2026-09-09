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
 * Range-time map - the second dimension that makes this an NPU workload.
 *
 * One KW307 frame is three numbers; no convolutional network has anything to
 * convolve over. Stack a minute of frames and the same data becomes a 2D
 * image - range on one axis, time on the other - which is exactly what a small
 * conv net eats. That is the whole argument for putting this module on a part
 * with 8 MB of PSRAM and an NPU instead of the M0 the module vendor suggests:
 * the memory is what buys the time axis, and the time axis is what the NPU
 * needs.
 *
 * TENSOR CONTRACT for whatever model gets trained on this:
 *   shape   KW307_MAP_RANGE_BINS x KW307_MAP_TIME_STEPS, uint8, row-major
 *           row = range bin (near to far), column = time (oldest to newest)
 *   value   target magnitude, saturating at KW307_MAP_MAG_FULL_SCALE
 *   0       no target in that range bin at that time
 *
 * DESIGN CONSTRAINT, decided up front: the model must convolve over this as a
 * 2D image (Conv2D). A Conv1D over the time axis with a non-square kernel does
 * not map onto this NPU - that is a silicon limit, not a converter gap - and
 * would silently fall back to the CPU. Keeping the map 2D keeps the model on
 * the NPU.
 */
#ifndef __KW307_MAP_H__
#define __KW307_MAP_H__

#include <stdbool.h>
#include <stdint.h>

#include "kw307_history.h"

#define KW307_MAP_RANGE_BINS   32
#define KW307_MAP_TIME_STEPS   64
#define KW307_MAP_BIN_MM       250    /* 32 x 250 mm = 8 m of coverage */
#define KW307_MAP_CELLS        (KW307_MAP_RANGE_BINS * KW307_MAP_TIME_STEPS)

/* Magnitudes seen on a real subject run 400..3100, so this saturates only on a
 * very strong return. Fixed rather than auto-scaled per map: a network trained
 * on auto-scaled input loses the absolute signal level, which is most of what
 * distinguishes a person from a small reflector. */
#define KW307_MAP_MAG_FULL_SCALE 4096

typedef struct {
    uint16_t window_s;          /* seconds the map spans                     */
    uint16_t samples_used;      /* history samples that went into it         */
    uint16_t samples_per_step;  /* time-axis decimation                      */
    uint8_t  occupancy_pct;     /* share of cells that are non-zero          */
    uint8_t  peak;              /* largest cell value                        */
} kw307_map_info_t;

/* Render the last `window_s` seconds into `out` (KW307_MAP_CELLS bytes).
 * Returns false when the history holds less than the window. Each cell takes
 * the MAXIMUM magnitude in its time slice rather than the mean: a target
 * crossing a bin for a fraction of a second is a real event, and averaging
 * would dilute it into the noise. */
bool kw307_map_build(uint32_t window_s, uint8_t *out, kw307_map_info_t *info);

/* Print the map to the console as a waterfall, newest column on the right.
 * Rows are range bins, near at the top. */
void kw307_map_print(const uint8_t *map, const kw307_map_info_t *info);

/* Emit the map as one CSV line (info fields, then all cells) for collecting
 * training data over the USB console. `label` is copied through so a capture
 * session can be sorted afterwards. */
void kw307_map_dump_csv(const uint8_t *map, const kw307_map_info_t *info,
                        const char *label);

#endif /* __KW307_MAP_H__ */
