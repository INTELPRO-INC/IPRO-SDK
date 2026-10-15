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
 * @file gesture.h
 * @brief Hand-gesture classifier (rock / paper / scissors / none) on the NPU.
 *
 * The launcher's input device. The same 128x128 four-class int8 network the
 * rock-paper-scissors app trained (apps/ai/ipro7ai_evk_rps/model/rps_v3.inm)
 * is loaded into the NPU once - from sd:/models/gesture.inm when a card is
 * in, else from the "mfg" flash partition over XIP. The UI task cuts the
 * gesture box out of each preview frame (gesture_push_yuyv) and an
 * inference task publishes the class and confidence.
 *
 * Unlike the RPS app the softmax is integer (softmax_q.h): this app runs an
 * audio task beside the NPU task and the FreeRTOS port does not save the
 * FPU across context switches, so no task may use float after boot.
 */
#ifndef GESTURE_H
#define GESTURE_H

#include <stdint.h>
#include <stdbool.h>

#define GESTURE_IN_W     128
#define GESTURE_IN_H     128
#define GESTURE_NCLASS   4

/* Same order as the model output and input.h's INPUT_CLS_*. */
typedef enum {
    GESTURE_ROCK = 0,
    GESTURE_PAPER,
    GESTURE_SCISSORS,
    GESTURE_NONE,
} gesture_class_t;

typedef enum {
    GESTURE_FEED_RAW = 0,     /* NPU reads the ISP ARGB frame directly (ext_input) */
    GESTURE_FEED_XOR,         /* as RAW, every byte ^ 0x80 first (int8, zp -128)   */
    GESTURE_FEED_COPY,        /* CPU packs into the model's own input buffer       */
} gesture_feed_t;

typedef struct {
    gesture_class_t cls;
    uint8_t         conf;                    /* winner's probability, percent */
    uint8_t         prob[GESTURE_NCLASS];    /* percent                       */
    uint8_t         raw[GESTURE_NCLASS];     /* NPU output codes              */
    uint32_t        seq;                     /* bumps once per result         */
    uint32_t        npu_ms;
} gesture_result_t;

typedef struct {
    uint32_t frames, errors, fps_x10;
    uint32_t wait_ms;                        /* last wait for the shared NPU */
    uint32_t in_sum;                         /* fingerprint of the last input */
    uint32_t model_bytes, load_ms, free_heap;
    uint32_t in_w, in_h;
    uint8_t  ext_input;
    int      out_zp;
    uint32_t scale_q16;
} gesture_stats_t;

typedef void (*gesture_done_cb_t)(void *arg);

/** @brief Allocate the input frame, load the model, start the task (~0.3 s).
 *         Call before the camera allocates its frames. */
int  gesture_init(void);
bool gesture_ready(void);
const char *gesture_fail_reason(void);

/** @brief Where the model came from: "sd card", "flash mfg" or "none". */
const char *gesture_source(void);

/** @brief The model's 128x128 ARGB input frame, 0 before init. */
uint32_t gesture_input_addr(void);

/**
 * @brief Build the input from a YUYV frame and queue an inference. Task
 *        context. @p yuyv is the frame, @p stride_px its width in pixels,
 *        (x0, y0) the top-left of the 128x128 window; x0 must be even.
 * @return 0 = queued; -1 = busy / not ready / bad window (frame skipped).
 */
int  gesture_push_yuyv(const uint32_t *yuyv, int stride_px, int x0, int y0);
bool gesture_busy(void);
void gesture_set_done_cb(gesture_done_cb_t cb, void *arg);

void gesture_get_result(gesture_result_t *out);
void gesture_get_stats(gesture_stats_t *out);

int            gesture_set_feed(gesture_feed_t f);
gesture_feed_t gesture_get_feed(void);
const char    *gesture_feed_name(gesture_feed_t f);

/** @brief Print every result to the console (default off). */
void gesture_set_print(bool on);
bool gesture_get_print(void);

const char *gesture_name(gesture_class_t c);

#endif /* GESTURE_H */
