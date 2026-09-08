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
 * @file yolov8_detect.h
 * @brief YOLOv8 Detection Header for IPROAI Framework
 *
 * This is adapted from ipro85_linux implementation for use with IPROAI
 * framework. Supports 6-output YOLOv8 models (3 box outputs + 3 class outputs).
 */

#ifndef __YOLOV8_DETECT_H__
#define __YOLOV8_DETECT_H__

#include "iproai_core.h"
#include "iproai_def_internal.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========== YOLOv8 Configuration Constants ========== */
#define YOLOV8_INPUT_WIDTH 640
#define YOLOV8_INPUT_HEIGHT 360
#define YOLOV8_NUM_CLASSES 2 // Person + Pet
#define YOLOV8_NUM_LAYERS 3  // P3, P4, P5
#define YOLOV8_NUM_OUTPUTS 6 // 3 box + 3 class
#define YOLOV8_REG_MAX 1     // DFL regression max (headless model)

/* Strides for each detection layer */
#define YOLOV8_STRIDE_P3 8
#define YOLOV8_STRIDE_P4 16
#define YOLOV8_STRIDE_P5 32

/* Maximum detections and candidates */
#define YOLOV8_MAX_DETECTIONS 50  // Maximum output detections after NMS
#define YOLOV8_MAX_CANDIDATES 200 // Maximum candidates before NMS

/* Default thresholds */
#define YOLOV8_DEFAULT_CONF_THRESHOLD 0.25f
#define YOLOV8_DEFAULT_IOU_THRESHOLD 0.45f

/* ========== Data Structures ========== */

/**
 * @brief Bounding box structure
 */
typedef struct {
    float x1; // Top-left x
    float y1; // Top-left y
    float x2; // Bottom-right x
    float y2; // Bottom-right y
} yolov8_box_t;

/**
 * @brief Single detection result
 */
typedef struct {
    yolov8_box_t box;       // Bounding box
    float confidence;       // Detection confidence
    int class_id;           // Class ID
    const char *class_name; // Class name (pointer to string, not owned)
} yolov8_detection_t;

/**
 * @brief YOLOv8 detection results
 */
typedef struct {
    yolov8_detection_t detections[YOLOV8_MAX_DETECTIONS];
    int num_detections;
    uint32_t inference_time_ms; // For performance tracking
} yolov8_result_t;

/**
 * @brief Anchor grid for YOLOv8 (anchor-free but uses anchor points)
 */
typedef struct {
    float *anchors;       // Anchor points (num_anchors x 2)
    float *strides;       // Stride for each anchor
    uint32_t num_anchors; // Total number of anchors
} yolov8_anchor_grid_t;

/**
 * @brief Quantization parameters for each output tensor
 */
typedef struct {
    float scale;
    int32_t zero_point;
} yolov8_quant_param_t;

/**
 * @brief YOLOv8 configuration and state
 */
typedef struct {
    // Model configuration
    uint32_t input_width;
    uint32_t input_height;
    uint32_t num_classes;
    uint32_t reg_max;
    const char **class_names;

    // Thresholds
    float conf_threshold;
    float iou_threshold;

    // Network information (from IPROAI)
    struct iproai_net_info_t *net;

    // Anchor grid
    yolov8_anchor_grid_t *anchor_grid;

    // Quantization parameters for 6 outputs
    yolov8_quant_param_t quant_params[YOLOV8_NUM_OUTPUTS];

    // Layer sizes (number of anchors per layer)
    uint32_t layer_sizes[YOLOV8_NUM_LAYERS];
    uint32_t total_anchors;

    // Sigmoid lookup table for fast computation
    float *sigmoid_table;
    uint32_t sigmoid_table_size;

    // Memory pools (pre-allocated to avoid malloc during inference)
    struct {
        float *box_data;                    // For box predictions
        float *cls_data;                    // For class predictions
        float *candidates;                  // For candidate filtering
        yolov8_detection_t *raw_detections; // Before NMS
    } mem_pool;

} yolov8_config_t;

/* ========== Core Functions ========== */

/**
 * @brief Initialize YOLOv8 detector
 * @param config Pointer to configuration structure
 * @param net Network information from IPROAI
 * @return 0 on success, negative on failure
 */
int yolov8_init(yolov8_config_t *config, struct iproai_net_info_t *net);

/**
 * @brief Process YOLOv8 detection (6-output model)
 * @param config Detector configuration
 * @param net Network information from IPROAI
 * @param data_buffer Pointer to model's data buffer (from iproai_getInputBuffer
 * or model->buffer)
 * @param result Output detection results
 * @return 0 on success, negative on failure
 */
int yolov8_process_6out(yolov8_config_t *config, struct iproai_net_info_t *net,
                        uint8_t *data_buffer, yolov8_result_t *result);

/**
 * @brief Cleanup YOLOv8 detector
 * @param config Detector configuration
 */
void yolov8_deinit(yolov8_config_t *config);

/**
 * @brief Print detection results
 * @param result Detection results to print
 */
void yolov8_print_results(const yolov8_result_t *result);

/* ========== Helper Functions ========== */

/**
 * @brief Generate anchor grid for YOLOv8
 * @param img_height Input image height
 * @param img_width Input image width
 * @return Pointer to anchor grid, or NULL on failure
 */
yolov8_anchor_grid_t *yolov8_generate_anchors(uint32_t img_height,
                                              uint32_t img_width);

/**
 * @brief Free anchor grid
 * @param grid Anchor grid to free
 */
void yolov8_free_anchor_grid(yolov8_anchor_grid_t *grid);

/**
 * @brief Apply softmax function
 * @param input Input array
 * @param output Output array
 * @param size Array size
 */
void yolov8_softmax(const float *input, float *output, uint32_t size);

/**
 * @brief Distribution Focal Loss (DFL) transformation
 * @param box_pred Box predictions
 * @param num_anchors Number of anchors
 * @param reg_max Regression max value
 * @param output Output distances
 */
void yolov8_dfl(const float *box_pred, uint32_t num_anchors, uint32_t reg_max,
                float *output);

/**
 * @brief Convert distances to bounding boxes
 * @param distances Distance predictions
 * @param anchors Anchor grid
 * @param num_anchors Number of anchors
 * @param boxes Output bounding boxes (xywh format)
 */
void yolov8_dist2bbox(const float *distances,
                      const yolov8_anchor_grid_t *anchors, uint32_t num_anchors,
                      float *boxes);

/**
 * @brief Non-Maximum Suppression
 * @param boxes Bounding boxes
 * @param scores Confidence scores
 * @param classes Class IDs
 * @param num_boxes Number of boxes
 * @param iou_threshold IOU threshold for NMS
 * @param output Output detection results
 * @return Number of detections after NMS
 */
int yolov8_nms(const float *boxes, const float *scores, const int *classes,
               int num_boxes, float iou_threshold, yolov8_detection_t *output);

/**
 * @brief Calculate IOU between two boxes
 * @param box1 First box (x1, y1, x2, y2)
 * @param box2 Second box (x1, y1, x2, y2)
 * @return IOU value
 */
float yolov8_box_iou(const yolov8_box_t *box1, const yolov8_box_t *box2);

/**
 * @brief Fast sigmoid using lookup table
 * @param sigmoid_table Precomputed sigmoid table
 * @param table_size Size of lookup table
 * @param x Input value
 * @return Sigmoid(x)
 */
float yolov8_sigmoid_lut(const float *sigmoid_table, uint32_t table_size,
                         float x);

#ifdef __cplusplus
}
#endif

#endif /* __YOLOV8_DETECT_H__ */
