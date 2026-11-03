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

/**
 * @file sensor_model.h
 * @brief The deployed model: .inm + SNS1 block from mfg, run on the NPU.
 *
 * IPRO AI Studio's Sensor Teach writes the payload (ipro_ai_core/
 * sensor_export.py documents the SNS1 layout): the .inm, then a SNS1 block
 * naming the classes and how to rebuild the network's input from samples -
 * 256 bytes (version 1), or 320 with each class's UNKNOWN floor (version 2).
 */
#ifndef SENSOR_MODEL_H
#define SENSOR_MODEL_H

#include <stdint.h>

/* Where the sensor model (.inm + SNS1) lives in the mfg partition. On its own the app takes
 * everything after the 4 KB partition header; the combined Teach firmware
 * (apps/ai/ipro_reef_teach) gives each Teach its own slot. */
#ifndef SENSOR_MFG_OFFSET
#define SENSOR_MFG_OFFSET 0x1000u
#endif
#ifndef SENSOR_MFG_SIZE
#define SENSOR_MFG_SIZE   0u                /* 0: to the end of the partition */
#endif
/* On its own the slot is big enough for two banks (A/B): a deploy writes the
 * bank not in use, and the running model stays loadable until the new one is
 * committed (components/ai/store). A V1.1 board's model sits in bank A. */
#ifndef SENSOR_MFG_BANKS
#define SENSOR_MFG_BANKS  ((SENSOR_MFG_SIZE) ? 1u : 2u)
#endif

#include "ipro_ai_app.h"
#include "ipro_ai_store.h"
#include "sensor_front.h"

#define SM_MAX_CLASSES 8u
#define SM_LABEL_LEN   16u
#define SM_OUT_POS     16u            /* 4 x 4 positions, averaged */

typedef struct {
    uint32_t n;                       /* classes */
    char     labels[SM_MAX_CLASSES][SM_LABEL_LEN];
    float    rate;
    char     source[16];
    sf_cfg_t front;
    float    out_scale;               /* one position's logits */
    uint32_t inm_bytes;
    uint32_t sns_bytes;               /* 256 (version 1) or 320 (version 2) */
    float    floor[SM_MAX_CLASSES];   /* UNKNOWN under these (version 2) */
    uint32_t unknown;                 /* 1: report UNKNOWN under the floor */
    char     size[12];                /* "small" / "medium" / "large" */
} sensor_cfg_t;

/** Load what mfg holds. 0, or -1 (sensor_model_why() says why). */
int sensor_model_init(void);
/** NULL while no model is loaded. */
const sensor_cfg_t *sensor_model_cfg(void);
const char *sensor_model_why(void);

/** The model's flash slot, for deploys (ipro_ai_store.h). */
const ipro_ai_slot_t *sensor_model_slot(void);

/** Run the NPU on an SF_IMG_BYTES int8 image; logits averaged over the
 *  positions (cfg->n of them). raw, if not NULL, receives the NPU's
 *  SM_OUT_POS x 8 int8 outputs. 0 or -1. */
int sensor_model_run(const int8_t *img, float logits[SM_MAX_CLASSES], int8_t *raw, uint32_t *us);

/** Live classification of an int8 image through the app framework
 *  (components/ai/app): the classification adapter's result, events published
 *  (and anomaly events for a "normal"/"anomaly" model). 0 or -1. */
int sensor_model_classify(const int8_t *img, uint64_t t_us, ipro_ai_result_t *res, uint32_t *us);
int sensor_model_metrics(ipro_ai_app_metrics_t *m);
/** RUN: the model on raw input bytes (SF_IMG_BYTES, offset-binary); its raw
 *  output. 0, -1 no model, -2 wrong size, -4 NPU failed. */
int sensor_model_run_raw(const uint8_t *in, uint32_t bytes, uint8_t *out, uint32_t cap,
                         uint32_t *out_bytes, uint32_t *us);
/** BENCH and MODEL reply lines (docs/edge_ai/06 §5). */
int sensor_model_bench(char *line, uint32_t size, uint32_t n);
int sensor_model_describe(char *line, uint32_t size);

#endif /* SENSOR_MODEL_H */
