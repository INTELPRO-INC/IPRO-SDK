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
 * @file gesture_dfl_detect.h
 * @brief Post-processor for the YOLOv8-ReLU anchor-free + DFL hand-sign-digit
 *        detector (gesture_digit_w017), IPRO7AI NPU.
 *
 * 3 raw output heads, each [1, H, W, 74]:
 *   ch 0..63  = DFL box regression (4 sides x 16 bins)
 *   ch 64..73 = 10 class logits (digits 0..9), NO objectness
 *
 * Decode (per the delivery MODEL_CARD / infer_tflite_det.py; f = scale*(q - zp)):
 *   reg[side][b] = f(ch[side*16 + b]); dist[side] = Sum_b softmax(reg[side])[b]*b  (b=0..15)
 *   ax,ay = gx+0.5, gy+0.5
 *   x1=(ax-dist0)*stride  y1=(ay-dist1)*stride  x2=(ax+dist2)*stride  y2=(ay+dist3)*stride
 *   score[c]=sigmoid(f(ch[64+c]));  cls=argmax(score);  conf=max(score);  conf>thr -> NMS(0.45)
 *
 * Reuses yolov8_detect.h result structs + yolov8_nms()/yolov8_box_iou().
 * NPU output memory: per cell, channels padded to align(out_c,4)=76 bytes;
 * value(h,w,c) = data_buffer + layer.out_layer_mem*patch_size, then [(h*W+w)*76 + c].
 */

#ifndef __GESTURE_DFL_DETECT_H__
#define __GESTURE_DFL_DETECT_H__

#include "yolov8_detect.h"   /* yolov8_box_t / yolov8_detection_t / yolov8_result_t / yolov8_nms */

#ifdef __cplusplus
extern "C" {
#endif

#define GDFL_NUM_LAYERS     3      /* P3,P4,P5            */
#define GDFL_REG_MAX        16     /* DFL bins per side   */
#define GDFL_NUM_CLASSES    10     /* digits 0..9         */
#define GDFL_OUT_CH         (4 * GDFL_REG_MAX + GDFL_NUM_CLASSES)  /* 74 */
#define GDFL_MAX_DETECTIONS 50
#define GDFL_MAX_CANDIDATES 512
#define GDFL_DEFAULT_CONF   0.25f
#define GDFL_DEFAULT_IOU    0.45f

extern const char *gdfl_class_names[];     /* "0".."9" */

typedef struct {
    uint32_t layer_idx;
    uint32_t out_w, out_h;
    uint32_t ch_stride;   /* align(out_c,4) = 76 */
    uint32_t stride;      /* input / out_w (8/16/32) */
    float    scale;
    int32_t  zero_point;
} gdfl_level_t;

typedef struct {
    uint32_t input_width, input_height;
    float    conf_threshold, iou_threshold;
    struct iproai_net_info_t *net;
    gdfl_level_t levels[GDFL_NUM_LAYERS];
    struct {
        float              *boxes;          /* [4*MAX_CANDIDATES] cx/cy/w/h planar */
        float              *scores;
        int                *class_ids;
        yolov8_detection_t *raw_detections;
    } mem_pool;
} gdfl_config_t;

int  gdfl_init(gdfl_config_t *cfg, struct iproai_net_info_t *net);
int  gdfl_process(gdfl_config_t *cfg, struct iproai_net_info_t *net,
                  uint8_t *data_buffer, yolov8_result_t *result);
void gdfl_deinit(gdfl_config_t *cfg);
void gdfl_print_results(const yolov8_result_t *result);

#ifdef __cplusplus
}
#endif

#endif /* __GESTURE_DFL_DETECT_H__ */
