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
 * @file uvc_teach.h
 * @brief A user-trained classifier on the UVC stream (IPRO7AI Reef,
 *        CONFIG_UVC_TEACH): IPRO AI Studio's Teach page collects its training
 *        images from this stream and deploys the model it trains back here.
 *
 *   OV5640 1280x720 ─ ISP ─┬─ ch0 YUYV 640x360 ─► overlay ─► MJPEG ─► UVC ─► Studio (training images)
 *                          └─ ch1 centre 512x512 -> 128x128 ARGB ─► NPU (teach_model.c)
 *
 * Everything this module draws stays OUTSIDE the box: Studio crops the box's
 * interior out of the stream as training data, and anything drawn inside it
 * would be learnt. The stream is not mirrored by default, so that crop shows
 * the scene the way the NPU sees it.
 */
#ifndef UVC_TEACH_H
#define UVC_TEACH_H

#include <stdint.h>

/* The box in ISP crop coordinates: a centred square that ISP channel 1 scales
 * to the model's 128x128 (4:1). At 640x360 its interior is x 192..447,
 * y 52..307. */
#define UVC_TEACH_ROI_SIDE        512u
#define UVC_TEACH_ROI_X(crop_w)   (((crop_w) - UVC_TEACH_ROI_SIDE) / 2u)
#define UVC_TEACH_ROI_Y(crop_h)   (((crop_h) - UVC_TEACH_ROI_SIDE) / 2u)

/** @brief Load the model and start its task. Call before the ISP allocates
 *         its frame buffers. 0 = ready; otherwise the stream still runs and
 *         the overlay says why. */
int uvc_teach_init(void);

/**
 * @brief Draw the result around the box into one frame.
 *
 * @param dst     private frame the encoder reads (w*h*2 bytes)
 * @param src     the ISP's completed YUYV422 frame, already invalidated
 * @param w, h    frame size (640x360)
 * @param crop_w, crop_h  ISP crop the frame was scaled from (maps the ROI)
 */
void uvc_teach_overlay(uint8_t *dst, const uint8_t *src, int w, int h, int crop_w, int crop_h);

#endif /* UVC_TEACH_H */
