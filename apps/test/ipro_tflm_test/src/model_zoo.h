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

#ifndef __MODEL_ZOO_H__
#define __MODEL_ZOO_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *name;
    uint32_t    model_bytes;
    uint64_t    total_mac;    /* computed offline from the flatbuffer shapes */
    uint32_t    arena_used;
    uint64_t    load_cycles;
    uint64_t    cold_cycles;  /* run 0 - pays the I-cache fill */
    uint64_t    warm_cycles;  /* mean of the rest */
    int32_t     checksum;     /* output sum; guards against a no-op run */
    bool        ok;
} zoo_result_t;

/** Number of models in the zoo. */
int  zoo_count(void);
/** Name of model idx, or NULL. */
const char *zoo_name(int idx);
/** Load + run model idx for `iters` inferences. */
bool zoo_run(int idx, int iters, zoo_result_t *out);

#ifdef __cplusplus
}
#endif

#endif
