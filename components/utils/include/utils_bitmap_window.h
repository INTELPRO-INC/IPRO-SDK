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

#include <stdint.h>

struct utils_bitmap_ctx {
    int ssn;
    int ssn_duration;
    union {
        uint8_t bytes[8];
        uint64_t map64;
    } bitmap;
};

int utils_bitmap_window_init(struct utils_bitmap_ctx *ctx, int ssn, int ssn_max);
int utils_bitmap_window_bit_set(struct utils_bitmap_ctx *ctx, int bit_pos);
uint64_t utils_bitmap_window_map_get(struct utils_bitmap_ctx *ctx);
int utils_bitmap_window_ssn_get(struct utils_bitmap_ctx *ctx);
int utils_bitmap_window_cli_init(void);
