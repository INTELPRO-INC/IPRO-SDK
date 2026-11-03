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

/* Host harness for sensor_front.c: stdin = sf_cfg_t fields + float32 samples
 * (window x channels, interleaved), stdout = SF_IMG_BYTES int8. */
#include <stdio.h>
#include <stdlib.h>

#include "sensor_front.h"

typedef struct { const float *x; uint32_t ch; } buf_t;

static float sample(void *ctx, uint32_t ch, uint32_t i)
{
    const buf_t *b = ctx;
    return b->x[(size_t)i * b->ch + ch];
}

int main(void)
{
    sf_cfg_t cfg;
    int8_t img[SF_IMG_BYTES];

    if (fread(&cfg.channels, 4, 3, stdin) != 3 || fread(cfg.mean, 4, SF_MAX_CH, stdin) != SF_MAX_CH ||
        fread(cfg.std, 4, SF_MAX_CH, stdin) != SF_MAX_CH || fread(&cfg.in_scale, 4, 1, stdin) != 1 ||
        fread(&cfg.in_zp, 4, 1, stdin) != 1) {
        return 2;
    }
    const size_t n = (size_t)cfg.window * cfg.channels;
    float *x = malloc(n * sizeof(float));
    if (!x || fread(x, sizeof(float), n, stdin) != n) {
        return 2;
    }
    buf_t b = { x, cfg.channels };
    if (sensor_front(&cfg, sample, &b, img) != 0) {
        return 1;
    }
    fwrite(img, 1, sizeof(img), stdout);
    free(x);
    return 0;
}
