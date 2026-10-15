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
 * @file rps_model.h
 * @brief Rock / paper / scissors / none classifier on the IPRO7 NPU.
 *
 * A small all-convolution int8 network (128x128x3 in, 4 logits out) read
 * over XIP - linked into the firmware (CONFIG_RPS_EMBED_MODEL) or from the
 * "mfg" flash partition - and loaded into the NPU once. The
 * camera hands over one ISP channel-1 frame at a time (the sensor ROI scaled
 * to 128x128 ARGB); an inference task runs the NPU, softmaxes the four
 * logits on the CPU and publishes the class and confidence.
 *
 * The frame can reach the NPU three ways - see rps_feed_t. Which one is
 * right depends on how the model was quantised and whether its first layer
 * is the NPU's external-input conv; the default is picked from the model
 * header and `rps feed` switches it live, so the question is settled on the
 * bench rather than by argument.
 */
#ifndef RPS_MODEL_H
#define RPS_MODEL_H

#include <stdint.h>
#include <stdbool.h>

#define RPS_IN_W      128
#define RPS_IN_H      128
#define RPS_NCLASS    4

typedef enum {
    RPS_ROCK = 0,
    RPS_PAPER,
    RPS_SCISSORS,
    RPS_NONE,                   /* no hand in the box */
} rps_class_t;

typedef enum {
    RPS_FEED_RAW = 0,           /* NPU reads the ISP ARGB frame directly (ext_input,
                                   uint8 pixels; what person_pet_detect does)   */
    RPS_FEED_XOR,               /* as RAW, but every byte ^ 0x80 first (int8 input
                                   with zero point -128, fed via ext_input)     */
    RPS_FEED_COPY,              /* CPU packs RGB, -128, into the model's own
                                   input buffer (models whose layer 0 is not the
                                   external-input conv)                           */
} rps_feed_t;

typedef struct {
    rps_class_t cls;            /* argmax                                  */
    uint8_t     conf;           /* softmax of the winner, 0..100 %         */
    uint8_t     prob[RPS_NCLASS]; /* softmax, 0..100 %                     */
    int8_t      logit[RPS_NCLASS];/* raw int8 output, for the bench        */
    uint32_t    seq;            /* bumps once per published result         */
    uint32_t    npu_ms;
} rps_result_t;

typedef struct {
    uint32_t frames, errors;
    uint32_t fps_x10;
    uint32_t model_bytes, load_ms, free_heap;
    const char *source;         /* "firmware" (embedded) or "mfg"          */
    uint32_t in_w, in_h;        /* what the model header says              */
    uint8_t  ext_input, unsigned_input;
    float    out_scale;
    int      out_zp;
} rps_model_stats_t;

typedef void (*rps_done_cb_t)(void *arg);

/** @brief Allocate the input frame, load the model, start the task.
 *  ~0.3 s. Call before the camera allocates its frames. */
int  rps_model_init(void);
bool rps_model_ready(void);
const char *rps_model_fail_reason(void);

/** @brief ISP channel-1 destination (128x128 ARGB), 0 before init. */
uint32_t rps_model_input_addr(void);

/** @brief Hand a completed frame to the NPU. ISR-safe. 0 = taken. */
int  rps_model_push(uint32_t addr);
bool rps_model_busy(void);
void rps_model_set_done_cb(rps_done_cb_t cb, void *arg);

void rps_model_get_result(rps_result_t *out);

/**
 * @brief Copy of the next frame the NPU processes (128x128 ARGB, 4 B/px, as
 *        the ISP wrote it - before any feed conversion). Blocks up to
 *        @p timeout_ms. For collecting training data off the board.
 * @return 0 and *out valid until the next call; <0 on timeout / not ready.
 */
int rps_model_capture(const uint8_t **out, uint32_t timeout_ms);
void rps_model_get_stats(rps_model_stats_t *out);

int        rps_model_set_feed(rps_feed_t f);
rps_feed_t rps_model_get_feed(void);

/** @brief Print every result to the console (default off). */
void rps_model_set_print(bool on);
bool rps_model_get_print(void);

const char *rps_class_name(rps_class_t c);

#endif /* RPS_MODEL_H */
