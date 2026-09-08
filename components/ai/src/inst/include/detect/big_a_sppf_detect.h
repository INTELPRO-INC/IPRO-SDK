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
 * @file big_a_sppf_detect.h
 * @brief Post-processor for the BIG-A + SPPF-lite detector (SKU-110K, IPRO7-AI NPU).
 *
 * This model is a v5-style anchor-based detector (NOT the anchor-free YOLOv8
 * person/pet model). It has 3 output levels (P3/P4/P5), each [1, H, W, 18]
 * where 18 = 3 anchors x (4 box + 1 obj + 1 class), single class.
 *
 * Decode (per the delivery README; logits = (uint8 - zero_point) * scale):
 *     cx = (sigmoid(tx) + grid_x) / grid_w     (normalised 0..1)
 *     cy = (sigmoid(ty) + grid_y) / grid_h
 *     bw = exp(tw)                              (normalised 0..1, NO anchor prior)
 *     bh = exp(th)
 *     score = sigmoid(obj) * sigmoid(cls)
 *     box xyxy = [cx-bw/2, cy-bh/2, cx+bw/2, cy+bh/2]  -> greedy NMS (IoU 0.45)
 *
 * Reuses the yolov8_detect.h result structs (yolov8_box_t / yolov8_detection_t /
 * yolov8_result_t) and the yolov8_nms()/yolov8_box_iou() helpers so the existing
 * OSD overlay path in uvc_isp.c does not need to change.
 */

#ifndef __BIG_A_SPPF_DETECT_H__
#define __BIG_A_SPPF_DETECT_H__

#include "yolov8_detect.h"   /* yolov8_box_t / yolov8_detection_t / yolov8_result_t / yolov8_quant_param_t / yolov8_nms */

#ifdef __cplusplus
extern "C" {
#endif

/* ===== BIG-A + SPPF-lite configuration ===== */
#define BIG_A_SPPF_NUM_LAYERS    3                 /* P3, P4, P5                     */
#define BIG_A_SPPF_ANCHORS       3                 /* anchor slots per cell          */
#define BIG_A_SPPF_NUM_CLASSES   1                 /* SKU-110K single class          */
#define BIG_A_SPPF_FIELDS        6                 /* tx,ty,tw,th,obj,cls per anchor */
#define BIG_A_SPPF_OUT_CH        (BIG_A_SPPF_ANCHORS * BIG_A_SPPF_FIELDS)  /* 18 */

#define BIG_A_SPPF_MAX_DETECTIONS 64
#define BIG_A_SPPF_MAX_CANDIDATES 512               /* dense retail -> many boxes    */

#define BIG_A_SPPF_DEFAULT_CONF   0.25f
#define BIG_A_SPPF_DEFAULT_IOU    0.45f

/** Single-class name table (index 0). */
extern const char *big_a_sppf_class_names[];

/** Per-output-level descriptor resolved from net_info at init time. */
typedef struct {
    uint32_t layer_idx;     /* index into net->layers[]                       */
    uint32_t out_w;         /* grid width  (e.g. 32 / 16 / 8 at 256 input)    */
    uint32_t out_h;         /* grid height                                    */
    uint32_t ch_stride;     /* bytes per cell = align(out_c, 4) (18 -> 20)    */
    float    scale;         /* output_scale (dequant)                         */
    int32_t  zero_point;    /* tf_output_offset (dequant)                     */
} big_a_sppf_level_t;

/** Detector configuration + scratch state. */
typedef struct {
    uint32_t input_width;
    uint32_t input_height;
    uint32_t num_classes;
    float    conf_threshold;
    float    iou_threshold;

    struct iproai_net_info_t *net;
    big_a_sppf_level_t levels[BIG_A_SPPF_NUM_LAYERS];

    /* Pre-allocated scratch (avoid malloc during inference). */
    struct {
        float              *boxes;          /* [4 * MAX_CANDIDATES], cx/cy/w/h planar */
        float              *scores;         /* [MAX_CANDIDATES]                       */
        int                *class_ids;      /* [MAX_CANDIDATES]                       */
        yolov8_detection_t *raw_detections; /* [MAX_DETECTIONS] (NMS output)          */
    } mem_pool;
} big_a_sppf_config_t;

/**
 * @brief Initialise the BIG-A + SPPF-lite post-processor.
 * @param config Config to initialise.
 * @param net    Network info from iproai_getNetInfo().
 * @return 0 on success, negative on failure (e.g. output_num != 3).
 */
int big_a_sppf_init(big_a_sppf_config_t *config, struct iproai_net_info_t *net);

/**
 * @brief Run decode + NMS over the 3 INT8 output tensors.
 * @param config      Detector config (from big_a_sppf_init).
 * @param net         Network info.
 * @param data_buffer NPU data region (from iproai_getInputBuffer()).
 * @param result      Output detections (boxes in input-pixel coords).
 * @return 0 on success, negative on failure.
 */
int big_a_sppf_process(big_a_sppf_config_t *config, struct iproai_net_info_t *net,
                       uint8_t *data_buffer, yolov8_result_t *result);

/** @brief Release scratch buffers. */
void big_a_sppf_deinit(big_a_sppf_config_t *config);

/** @brief Print detections to the console. */
void big_a_sppf_print_results(const yolov8_result_t *result);

#ifdef __cplusplus
}
#endif

#endif /* __BIG_A_SPPF_DETECT_H__ */
