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
 * @file yolov8_person_pet_detect.h
 * @brief YOLOv8 Person+Pet Detection Header
 *
 * Wrapper around yolov8_detect.h for person and pet detection.
 * Provides simplified interface for demo_ai.c usage.
 */

#ifndef __YOLOV8_PERSON_PET_DETECT_H__
#define __YOLOV8_PERSON_PET_DETECT_H__

#include "yolov8_detect.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========== Person+Pet Specific Configuration ========== */

/**
 * @brief Class names for person+pet detection
 */
extern const char* yolov8_person_pet_class_names[];

/**
 * @brief Number of classes (person + pet)
 */
#define YOLOV8_PERSON_PET_NUM_CLASSES  2

/* ========== Person+Pet Detection Functions ========== */

/**
 * @brief Initialize YOLOv8 person+pet detector
 *
 * This function wraps yolov8_init() with person+pet specific configuration.
 *
 * @param config Pointer to configuration structure (will be initialized)
 * @param net Network information from IPROAI
 * @return 0 on success, negative on failure
 */
int yolov8_person_pet_init(yolov8_config_t* config, struct iproai_net_info_t* net);

/**
 * @brief Process YOLOv8 person+pet detection
 *
 * This function wraps yolov8_process_6out() for person+pet detection.
 * Handles 6-output models (3 box + 3 class outputs).
 *
 * @param config Detector configuration
 * @param hdl IPROAI model handle
 * @param result Output detection results
 * @return 0 on success, negative on failure
 */
int yolov8_person_pet_process(yolov8_config_t* config, struct iproai_net_info_t* net, uint8_t* data_buffer, yolov8_result_t* result);

/**
 * @brief Process YOLOv8 person+pet detection (6-output model)
 *
 * @deprecated Use yolov8_person_pet_process() instead. This function is kept for backward compatibility.
 *
 * @param config Detector configuration
 * @param hdl IPROAI model handle
 * @param result Output detection results
 * @return 0 on success, negative on failure
 */
int yolov8_person_pet_process_6out(yolov8_config_t* config, iproai_model_hdl_t hdl, yolov8_result_t* result);

/**
 * @brief Cleanup YOLOv8 person+pet detector
 *
 * This function wraps yolov8_deinit().
 *
 * @param config Detector configuration
 */
void yolov8_person_pet_deinit(yolov8_config_t* config);

/**
 * @brief Print person+pet detection results
 *
 * This function wraps yolov8_print_results() with person+pet specific formatting.
 *
 * @param result Detection results to print
 */
void yolov8_person_pet_print_results(const yolov8_result_t* result);

#ifdef __cplusplus
}
#endif

#endif /* __YOLOV8_PERSON_PET_DETECT_H__ */
