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

#ifndef __YOLO_RUNNER_H__
#define __YOLO_RUNNER_H__

#include <stdint.h>
#include <stdbool.h>

#define YOLO_MAX_HEADS 8

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t model_bytes;
    uint32_t arena_used;
    uint32_t arena_size;
    uint64_t load_cycles;   /* 32-bit wraps at 22.4 s @192MHz */
    uint32_t n_inputs, n_outputs;
    int32_t  in_h, in_w, in_c;
} yolo_info_t;

typedef struct {
    uint64_t cycles;        /* 32-bit wraps at 22.4 s @192MHz */
    uint32_t n_heads;
    int32_t  head_sum[YOLO_MAX_HEADS];
    uint32_t head_bytes[YOLO_MAX_HEADS];
} yolo_result_t;

/** Load the model and allocate tensors. Fills info; false on any failure. */
bool yolo_init(yolo_info_t *info);

/** One inference on a synthetic frame. Checksums each output head. */
bool yolo_infer(yolo_result_t *res);

#ifdef __cplusplus
}
#endif

#endif
