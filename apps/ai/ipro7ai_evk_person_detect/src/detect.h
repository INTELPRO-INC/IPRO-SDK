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
 * @file detect.h
 * @brief YOLOv8 person/pet detection on the IPRO7 NPU.
 *
 * The model (person_pet_detect, 640x360 ARGB in, 6 quantised heads out) is
 * read straight out of the "mfg" flash partition over XIP and loaded into the
 * NPU once at boot. The camera then hands over one ISP channel-1 frame at a
 * time with detect_push_frame(); an inference task runs the NPU, decodes the
 * boxes and publishes a detect_result_t snapshot for the display to draw.
 *
 * One frame in flight: while the NPU is busy the camera keeps channel 1
 * gated off, and re-arms it from the done callback. That is what keeps the
 * NPU from ever reading a buffer the ISP is still writing.
 *
 * Box coordinates are in model-input pixels (DETECT_IN_W x DETECT_IN_H),
 * which is the whole 1280x720 sensor frame scaled by exactly 2.
 */
#ifndef DETECT_H
#define DETECT_H

#include <stdint.h>
#include <stdbool.h>

#define DETECT_IN_W   640
#define DETECT_IN_H   360
#define DETECT_MAX    50            /* = YOLOV8_MAX_DETECTIONS */

#define DETECT_CLASS_PERSON 0
#define DETECT_CLASS_PET    1

#define DETECT_OK          0
#define DETECT_ERR_NOPART -1        /* "mfg" partition missing from the table */
#define DETECT_ERR_MODEL  -2        /* header/size check failed, or NPU load  */
#define DETECT_ERR_NOMEM  -3
#define DETECT_ERR_STATE  -4

typedef struct {
    int16_t x1, y1, x2, y2;         /* model-input pixels, x1<=x2, y1<=y2 */
    uint8_t cls;                    /* DETECT_CLASS_*                     */
    uint8_t conf;                   /* 0..100 %                           */
} detect_box_t;

typedef struct {
    detect_box_t box[DETECT_MAX];
    uint8_t  count;
    uint8_t  persons;
    uint8_t  pets;
    uint32_t seq;                   /* bumps once per published result    */
    uint32_t npu_ms;                /* NPU compute wall time, last frame  */
    uint32_t post_ms;               /* box decode + NMS, last frame       */
} detect_result_t;

typedef struct {
    uint32_t frames;                /* inferences completed               */
    uint32_t errors;                /* NPU or post-process failures       */
    uint32_t fps_x10;               /* detection rate, tenths of a frame/s*/
    uint32_t model_bytes;
    uint32_t load_ms;
    uint32_t free_heap;             /* PSRAM heap left after model load   */
} detect_stats_t;

/** Called from the inference task once a result has been published. The
 *  camera uses it to re-enable channel 1 for the next frame. */
typedef void (*detect_done_cb_t)(void *arg);

/**
 * @brief Allocate the input frame, load the model from flash, start the task.
 *
 * Takes ~1.1 s (NPU weight upload). Call BEFORE the camera allocates its
 * frames: the model's ~3 MB activation buffer wants a mostly-empty heap.
 * On failure everything is left usable without detection - the camera
 * preview still runs, the HUD says why.
 */
int detect_init(void);

/** @brief True once the model is loaded and the task is running. */
bool detect_is_ready(void);

/** @brief Human-readable reason detect_init() failed, or "" when it did not. */
const char *detect_fail_reason(void);

/**
 * @brief Address the ISP must write channel 1 (640x360 ARGB) into.
 *
 * The frame is the NPU's external input buffer; there is exactly one, so the
 * ISP is configured with pp_num = 1 and both addr[] slots set to this.
 * Returns 0 before detect_init() succeeded.
 */
uint32_t detect_input_addr(void);

/** @brief Hand a completed channel-1 frame to the NPU. ISR-safe.
 *  @return DETECT_OK, or DETECT_ERR_STATE if the NPU is still busy / not ready. */
int detect_push_frame(uint32_t addr);

/** @brief True while an inference is in progress. */
bool detect_busy(void);

void detect_set_done_cb(detect_done_cb_t cb, void *arg);

/** @brief Copy the newest published result. Safe from any task. */
void detect_get_result(detect_result_t *out);

/** @brief Confidence threshold in percent (default 25). Applied to the next
 *         frame; no restart needed. */
int  detect_set_conf(int pct);
int  detect_get_conf(void);

/** @brief Print every detection list to the console (default off - at ~7 Hz
 *         it floods a 115200 UART). */
void detect_set_print(bool on);
bool detect_get_print(void);

void detect_get_stats(detect_stats_t *out);

#endif /* DETECT_H */
