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
 * @file uvc_ai.h
 * @brief AI/NPU person + pet detection for the IPRO7 UVC camera.
 *
 * Loads a YOLOv8 person/pet model from the "mfg" flash partition into the NPU,
 * then runs continuous inference on ISP channel-1 (ARGB 640x360) frames pushed
 * in via uvc_ai_push_buffer(). Detection results are retrieved with
 * uvc_ai_get_result() and drawn as bounding boxes by the ISP OSD layer.
 *
 * All entry points compile to no-ops unless CONFIG_AI_SUPPORT is set.
 */

#ifndef __UVC_AI_H__
#define __UVC_AI_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the NPU, load the model from flash, start the inference task.
 * @return 0 on success, <0 on error (partition missing, NPU load failed, OOM).
 *         On failure the caller should keep streaming UVC without detection.
 */
int uvc_ai_init(void);

/**
 * @brief Push a 640x360 ARGB frame buffer (ISP channel 1) for inference.
 *
 * ISR-safe; intended to be called from the ISP frame-ready callback. The queue
 * holds a single frame, so a push while the NPU is still busy is dropped.
 *
 * @param addr Physical address of the ARGB frame buffer.
 * @return 0 on success, <0 if AI not ready or the queue is full.
 */
int uvc_ai_push_buffer(uint32_t addr);

/**
 * @brief Block until a detection result is available.
 * @param result_addr Receives a pointer (as uint32_t) to a yolov8_result_t.
 * @return 0 on success, <0 if AI is not initialized.
 */
int uvc_ai_get_result(uint32_t *result_addr);

/**
 * @brief Address the ISP should DMA ch1 frame content to (shared buffer).
 *
 * For the int8 gesture model the ch1 frame and the NPU model-input buffer are the
 * SAME allocation (saves ~0.9 MB): the ISP writes the raw content into the
 * letterbox window of that buffer and the inference task sign-flips it in place.
 * Returns the write address, or 0 if there is no shared buffer (other models, or
 * AI not initialized yet) — in which case the ISP should allocate its own frame.
 *
 * @return ch1 write address, or 0 to allocate a private ch1 frame.
 */
uint32_t uvc_ai_get_ch1_target(void);

#ifdef __cplusplus
}
#endif

#endif /* __UVC_AI_H__ */
