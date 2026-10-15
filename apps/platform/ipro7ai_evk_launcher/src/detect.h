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
 * @brief YOLOv8 person/pet detection on the NPU - the launcher's second model.
 *
 * The person_pet_detect model (640x360 ARGB in, 6 quantised heads out,
 * 700 KB) does not fit next to the gesture model in the 688 KB "mfg" slot,
 * so it comes from the micro-SD only: sd:/models/person.inm. No card, no
 * file -> the DETECT page says so and everything else runs.
 *
 * Feed: ISP channel 1, the whole 1280x720 frame scaled by exactly 2 into
 * this module's buffer, one frame in flight (evk_cam gates the channel from
 * the frame-done interrupt until detect_done_cb fires). The NPU is shared
 * with the gesture classifier through npu_share.h.
 *
 * Box coordinates are in model-input pixels (DETECT_IN_W x DETECT_IN_H).
 * This is the one task in the app that uses floating point (the box decode
 * in components/ai); it is the only FPU user after boot, so the port's
 * missing FPU context save does not bite.
 *
 * Ported from apps/ai/ipro7ai_evk_person_detect/src/detect.[ch].
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
#define DETECT_ERR_NOFILE -1        /* no card, or no sd:/models/person.inm  */
#define DETECT_ERR_MODEL  -2        /* load failed, or not the model expected */
#define DETECT_ERR_NOMEM  -3
#define DETECT_ERR_STATE  -4

#define DETECT_SD_PATH    "sd:/models/person.inm"

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
    uint32_t frames;
    uint32_t errors;
    uint32_t fps_x10;
    uint32_t wait_ms;               /* last wait for the shared NPU       */
    uint32_t model_bytes;
    uint32_t load_ms;
    uint32_t free_heap;
} detect_stats_t;

typedef void (*detect_done_cb_t)(void *arg);

/** @brief Allocate the input frame, load the model from the card, start the
 *         task. ~1.2 s. Call after gesture_init() and before the camera. */
int  detect_init(void);
bool detect_is_ready(void);
const char *detect_fail_reason(void);

/** @brief ISP channel-1 destination (640x360 ARGB), 0 before init. */
uint32_t detect_input_addr(void);

/** @brief Hand a completed channel-1 frame to the NPU. ISR-safe. */
int  detect_push_frame(uint32_t addr);
bool detect_busy(void);
void detect_set_done_cb(detect_done_cb_t cb, void *arg);

void detect_get_result(detect_result_t *out);
int  detect_set_conf(int pct);
int  detect_get_conf(void);

/** @brief Minimum time between detections, ms (default 200). */
int      detect_set_period_ms(uint32_t ms);
uint32_t detect_get_period_ms(void);
void detect_set_print(bool on);
bool detect_get_print(void);
void detect_get_stats(detect_stats_t *out);
const char *detect_class_name(int cls);

#endif /* DETECT_H */
