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

#include "kw307_map.h"

#include <stdio.h>
#include <string.h>

static uint8_t scale_mag(uint16_t mag)
{
    uint32_t v = (uint32_t)mag * 255u / KW307_MAP_MAG_FULL_SCALE;
    return v > 255u ? 255u : (uint8_t)v;
}

/* Range bin for a distance, or -1 when it falls outside the map. */
static int range_bin(uint16_t dist_mm)
{
    if (!dist_mm) {
        return -1;
    }
    const uint32_t bin = dist_mm / KW307_MAP_BIN_MM;
    return bin < KW307_MAP_RANGE_BINS ? (int)bin : -1;
}

static void place(uint8_t *col, uint16_t dist_mm, uint16_t mag)
{
    const int bin = range_bin(dist_mm);
    if (bin < 0) {
        return;
    }
    const uint8_t v = scale_mag(mag);
    if (v > col[bin]) {
        col[bin] = v;            /* max-pool: keep the strongest return */
    }
}

bool kw307_map_build(uint32_t window_s, uint8_t *out, kw307_map_info_t *info)
{
    if (!out || !window_s) {
        return false;
    }

    const uint32_t want = window_s * KW307_HISTORY_RATE_HZ;
    if (kw307_history_count() < want) {
        return false;
    }

    const uint32_t per_step = want / KW307_MAP_TIME_STEPS;
    if (!per_step) {
        return false;            /* window too short to fill the time axis */
    }

    memset(out, 0, KW307_MAP_CELLS);

    /* Column 0 is the oldest. age counts backwards from the newest sample, so
     * the oldest sample of the window is at age (want - 1). */
    for (uint32_t step = 0; step < KW307_MAP_TIME_STEPS; step++) {
        uint8_t *col = &out[step * KW307_MAP_RANGE_BINS];

        for (uint32_t k = 0; k < per_step; k++) {
            const uint32_t age = want - 1u - (step * per_step + k);
            kw307_sample_t s;
            if (!kw307_history_at(age, &s)) {
                continue;
            }
            if (s.valid_mask & KW307_VALID_M0)    place(col, s.m0_dist_mm, s.m0_mag);
            if (s.valid_mask & KW307_VALID_M1)    place(col, s.m1_dist_mm, s.m1_mag);
            if (s.valid_mask & KW307_VALID_STILL) place(col, s.s_dist_mm,  s.s_mag);
        }
    }

    if (info) {
        uint32_t nonzero = 0;
        uint8_t peak = 0;
        for (uint32_t i = 0; i < KW307_MAP_CELLS; i++) {
            if (out[i]) {
                nonzero++;
                if (out[i] > peak) peak = out[i];
            }
        }
        info->window_s = (uint16_t)window_s;
        info->samples_used = (uint16_t)(per_step * KW307_MAP_TIME_STEPS);
        info->samples_per_step = (uint16_t)per_step;
        info->occupancy_pct = (uint8_t)(nonzero * 100u / KW307_MAP_CELLS);
        info->peak = peak;
    }
    return true;
}

/* Coarse intensity ramp - enough to read a track by eye on a plain terminal. */
static char cell_char(uint8_t v)
{
    static const char ramp[] = " .:-=+*#%@";
    if (!v) {
        return ' ';
    }
    uint32_t i = 1u + (uint32_t)v * 8u / 255u;
    if (i > 9u) i = 9u;
    return ramp[i];
}

void kw307_map_print(const uint8_t *map, const kw307_map_info_t *info)
{
    if (!map) {
        return;
    }
    printf("range-time map: %u s, %u samples/step, occupancy %u%%, peak %u\r\n",
           info ? info->window_s : 0, info ? info->samples_per_step : 0,
           info ? info->occupancy_pct : 0, info ? info->peak : 0);
    printf("        oldest %*s newest\r\n", KW307_MAP_TIME_STEPS - 14, "->");

    for (uint32_t r = 0; r < KW307_MAP_RANGE_BINS; r++) {
        char line[KW307_MAP_TIME_STEPS + 1];
        for (uint32_t c = 0; c < KW307_MAP_TIME_STEPS; c++) {
            line[c] = cell_char(map[c * KW307_MAP_RANGE_BINS + r]);
        }
        line[KW307_MAP_TIME_STEPS] = '\0';
        const uint32_t mm = r * KW307_MAP_BIN_MM;
        printf("%2lu.%lum |%s|\r\n", (unsigned long)(mm / 1000),
               (unsigned long)((mm % 1000) / 100), line);
    }
}

void kw307_map_dump_csv(const uint8_t *map, const kw307_map_info_t *info,
                        const char *label)
{
    if (!map) {
        return;
    }
    printf("MAP,%s,%u,%u,%u,%u,%u,%u",
           label ? label : "-",
           KW307_MAP_RANGE_BINS, KW307_MAP_TIME_STEPS,
           info ? info->window_s : 0,
           info ? info->samples_per_step : 0,
           info ? info->occupancy_pct : 0,
           info ? info->peak : 0);

    /* Row-major, matching the tensor contract in the header, so a capture can
     * be reshaped straight into a training tensor without re-deriving order. */
    for (uint32_t r = 0; r < KW307_MAP_RANGE_BINS; r++) {
        for (uint32_t c = 0; c < KW307_MAP_TIME_STEPS; c++) {
            printf(",%u", map[c * KW307_MAP_RANGE_BINS + r]);
        }
    }
    printf("\r\n");
}
