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
 * KW307 frame history - the ring that makes this part different from an M0.
 *
 * The module emits 20 frames per second and says nothing about what happened
 * a minute ago. Every analysis whose window is longer than one frame - dwell
 * statistics, breathing, a behavioural baseline, or the time axis of a
 * range-time map for the NPU - needs that past kept somewhere. At 26 bytes a
 * sample that is 520 B/s, so an hour costs 1.87 MB and the 8 MB PSRAM on this
 * part holds several hours. A Cortex-M0 host, which is what the module vendor
 * tells customers to use, holds seconds.
 *
 * The ring lives in the PSRAM heap and is written from one producer (the radar
 * task) and read by analysis code on the same task, so it carries no locking.
 */
#ifndef __KW307_HISTORY_H__
#define __KW307_HISTORY_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "kw307_proto.h"

#define KW307_HISTORY_RATE_HZ  20

/* One retained frame. Distances are the CORRECTED range in mm (see
 * kw307_distance_mm), not the raw wire value - the history is what later
 * analysis reads, and it should not each have to remember the correction. */
typedef struct {
    uint16_t index;          /* module frame counter, for gap detection      */
    uint8_t  human_flag;
    uint8_t  valid_mask;     /* bit0 = moving[0], bit1 = moving[1], bit2 = still */
    uint16_t m0_dist_mm;  int16_t m0_ang;  uint16_t m0_mag;
    uint16_t m1_dist_mm;  int16_t m1_ang;  uint16_t m1_mag;
    uint16_t s_dist_mm;   int16_t s_ang;   uint16_t s_mag;
    uint8_t  m0_cnt, m1_cnt, s_cnt, _pad;
} kw307_sample_t;            /* 26 bytes */

#define KW307_VALID_M0    0x01
#define KW307_VALID_M1    0x02
#define KW307_VALID_STILL 0x04

/* Which scalar series to pull out of the ring for analysis. */
typedef enum {
    KW307_SERIES_STILL_MAG,      /* stationary magnitude - the breathing proxy */
    KW307_SERIES_STILL_DIST,
    KW307_SERIES_M0_MAG,
    KW307_SERIES_M0_DIST,
    KW307_SERIES_HUMAN_FLAG,
} kw307_series_t;

/* Allocate the ring for `seconds` of history. Returns 0, or negative when the
 * PSRAM heap cannot satisfy it. Safe to call again to resize (the old ring is
 * released first). seconds = 0 frees the ring. */
int  kw307_history_init(uint32_t seconds);
void kw307_history_free(void);

void kw307_history_push(const kw307_frame_t *frame, bool advanced);
void kw307_history_clear(void);

uint32_t kw307_history_count(void);      /* samples currently held */
uint32_t kw307_history_capacity(void);   /* samples the ring can hold */
uint32_t kw307_history_bytes(void);
uint32_t kw307_history_dropped(void);    /* oldest samples overwritten */

/* Read sample `age` frames back: 0 is the newest. Returns false when the ring
 * does not hold that many. */
bool kw307_history_at(uint32_t age, kw307_sample_t *out);

/* Copy the newest `n` samples of one series into `out`, oldest first, so an
 * FFT or a difference filter sees time running forwards. Returns how many were
 * written (< n when the ring holds less). Values are the raw field, unscaled -
 * conversion to q15 is the caller's business because the useful full-scale
 * differs per series. */
uint32_t kw307_history_series(kw307_series_t series, uint32_t n, int16_t *out);

/* Samples whose slot was not Valid contribute a gap. This reports how many of
 * the last `n` samples actually carried the series, which is the honest
 * denominator for any average taken over that window. */
uint32_t kw307_history_series_valid(kw307_series_t series, uint32_t n);

#endif /* __KW307_HISTORY_H__ */
