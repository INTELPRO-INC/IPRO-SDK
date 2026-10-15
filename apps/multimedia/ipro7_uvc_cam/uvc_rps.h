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
 * @file uvc_rps.h
 * @brief Rock-paper-scissors on the UVC stream (IPRO7AI Reef, CONFIG_UVC_RPS).
 *
 * The classifier and the game are the EVK app's own
 * (apps/ai/ipro7ai_evk_rps/src/rps_model.c, rps_game.c). This module owns
 * what the host sees: the play box, the live label, the countdown, the moves
 * and the score bars, drawn into each YUYV frame before it is encoded.
 *
 *   OV5640 1280x720 ─ ISP ─┬─ ch0 YUYV 1280x720 / 640x360 ─► overlay ─► MJPEG ─► UVC
 *                          └─ ch1 centre 512x512 -> 128x128 ARGB ─► NPU ─► game
 */
#ifndef UVC_RPS_H
#define UVC_RPS_H

#include <stdint.h>

/* The play box in ISP crop coordinates: a centred square that ISP channel 1
 * scales to the model's 128x128 (4:1, the same ROI as the EVK app). */
#define UVC_RPS_ROI_SIDE        512u
#define UVC_RPS_ROI_X(crop_w)   (((crop_w) - UVC_RPS_ROI_SIDE) / 2u)
#define UVC_RPS_ROI_Y(crop_h)   (((crop_h) - UVC_RPS_ROI_SIDE) / 2u)

/** @brief Load the model and start its task. Call before the ISP allocates
 *         its frame buffers. 0 = ready; otherwise the stream still runs and
 *         the overlay says why. */
int uvc_rps_init(void);

/**
 * @brief Advance the game and draw it into one frame.
 *
 * The frame is copied out of the ISP's ping-pong buffer first (mirrored for
 * the selfie view) and drawn on the copy: drawing and encoding in place took
 * longer than the 33 ms before the ISP rewrote that buffer, and the stream
 * tore.
 *
 * @param dst     private frame the encoder reads (w*h*2 bytes)
 * @param src     the ISP's completed YUYV422 frame, already invalidated
 * @param w, h    frame size (640x360 on the Reef game build)
 * @param crop_w, crop_h  ISP crop the frame was scaled from (maps the ROI)
 */
void uvc_rps_overlay(uint8_t *dst, const uint8_t *src, int w, int h, int crop_w, int crop_h);

#endif /* UVC_RPS_H */
