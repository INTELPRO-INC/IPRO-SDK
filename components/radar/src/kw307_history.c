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

#include "kw307_history.h"

#include <string.h>

#include <FreeRTOS.h>

static kw307_sample_t *s_ring;
static uint32_t s_cap;
static uint32_t s_count;
static uint32_t s_head;      /* index where the NEXT sample will be written */
static uint32_t s_dropped;

int kw307_history_init(uint32_t seconds)
{
    kw307_history_free();
    if (!seconds) {
        return 0;
    }

    uint32_t cap = seconds * KW307_HISTORY_RATE_HZ;
    size_t bytes = (size_t)cap * sizeof(kw307_sample_t);

    /* heap_5's primary region is PSRAM in every build that enables it, so this
     * lands there rather than in the scarce OCRAM. */
    s_ring = pvPortMalloc(bytes);
    if (!s_ring) {
        return -1;
    }
    s_cap = cap;
    s_count = s_head = s_dropped = 0;
    return 0;
}

void kw307_history_free(void)
{
    if (s_ring) {
        vPortFree(s_ring);
        s_ring = NULL;
    }
    s_cap = s_count = s_head = s_dropped = 0;
}

void kw307_history_clear(void)
{
    s_count = s_head = s_dropped = 0;
}

static void store_slot(const kw307_tracker_t *t, bool valid,
                       uint16_t *dist, int16_t *ang, uint16_t *mag, uint8_t *cnt)
{
    if (!valid) {
        *dist = 0; *ang = 0; *mag = 0; *cnt = 0;
        return;
    }
    *dist = kw307_distance_mm(t);      /* corrected range, not the wire value */
    *ang  = t->angle_tenth;
    *mag  = t->magnitude;
    *cnt  = t->counter;
}

void kw307_history_push(const kw307_frame_t *f, bool advanced)
{
    if (!s_ring) {
        return;
    }

    kw307_sample_t *s = &s_ring[s_head];
    memset(s, 0, sizeof(*s));

    const bool v0 = kw307_tracker_valid(&f->moving[0], true, advanced);
    const bool v1 = kw307_tracker_valid(&f->moving[1], true, advanced);
    const bool vs = kw307_tracker_valid(&f->stationary[0], false, advanced);

    s->index = f->index;
    s->human_flag = f->human_flag;
    s->valid_mask = (uint8_t)((v0 ? KW307_VALID_M0 : 0) |
                              (v1 ? KW307_VALID_M1 : 0) |
                              (vs ? KW307_VALID_STILL : 0));

    store_slot(&f->moving[0], v0, &s->m0_dist_mm, &s->m0_ang, &s->m0_mag, &s->m0_cnt);
    store_slot(&f->moving[1], v1, &s->m1_dist_mm, &s->m1_ang, &s->m1_mag, &s->m1_cnt);
    store_slot(&f->stationary[0], vs, &s->s_dist_mm, &s->s_ang, &s->s_mag, &s->s_cnt);

    s_head = (s_head + 1) % s_cap;
    if (s_count < s_cap) {
        s_count++;
    } else {
        s_dropped++;                    /* the ring is full: oldest fell off */
    }
}

uint32_t kw307_history_count(void)    { return s_count; }
uint32_t kw307_history_capacity(void) { return s_cap; }
uint32_t kw307_history_dropped(void)  { return s_dropped; }
uint32_t kw307_history_bytes(void)    { return s_cap * (uint32_t)sizeof(kw307_sample_t); }

/* Ring index of the sample `age` frames back from the newest. */
static uint32_t index_for_age(uint32_t age)
{
    return (s_head + s_cap - 1u - age) % s_cap;
}

bool kw307_history_at(uint32_t age, kw307_sample_t *out)
{
    if (!s_ring || age >= s_count) {
        return false;
    }
    *out = s_ring[index_for_age(age)];
    return true;
}

static int16_t sample_field(const kw307_sample_t *s, kw307_series_t series)
{
    switch (series) {
    case KW307_SERIES_STILL_MAG:  return (int16_t)(s->s_mag  & 0x7FFF);
    case KW307_SERIES_STILL_DIST: return (int16_t)(s->s_dist_mm  & 0x7FFF);
    case KW307_SERIES_M0_MAG:     return (int16_t)(s->m0_mag & 0x7FFF);
    case KW307_SERIES_M0_DIST:    return (int16_t)(s->m0_dist_mm & 0x7FFF);
    case KW307_SERIES_HUMAN_FLAG: return s->human_flag ? 1 : 0;
    default:                      return 0;
    }
}

static bool sample_has(const kw307_sample_t *s, kw307_series_t series)
{
    switch (series) {
    case KW307_SERIES_STILL_MAG:
    case KW307_SERIES_STILL_DIST: return (s->valid_mask & KW307_VALID_STILL) != 0;
    case KW307_SERIES_M0_MAG:
    case KW307_SERIES_M0_DIST:    return (s->valid_mask & KW307_VALID_M0) != 0;
    case KW307_SERIES_HUMAN_FLAG: return true;
    default:                      return false;
    }
}

uint32_t kw307_history_series(kw307_series_t series, uint32_t n, int16_t *out)
{
    if (!s_ring || !out || !n) {
        return 0;
    }
    if (n > s_count) {
        n = s_count;
    }

    /* Oldest first, so a difference filter or an FFT sees time run forwards. */
    for (uint32_t i = 0; i < n; i++) {
        const kw307_sample_t *s = &s_ring[index_for_age(n - 1u - i)];
        out[i] = sample_field(s, series);
    }
    return n;
}

uint32_t kw307_history_series_valid(kw307_series_t series, uint32_t n)
{
    if (!s_ring || !n) {
        return 0;
    }
    if (n > s_count) {
        n = s_count;
    }

    uint32_t valid = 0;
    for (uint32_t i = 0; i < n; i++) {
        if (sample_has(&s_ring[index_for_age(i)], series)) {
            valid++;
        }
    }
    return valid;
}
