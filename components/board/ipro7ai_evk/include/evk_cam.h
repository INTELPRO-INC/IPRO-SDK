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
 * @file evk_cam.h
 * @brief SC101IOT DVP camera -> ISP with two outputs: LCD preview + NPU.
 *
 *   channel 0  YUYV, 320x180 (letterbox) or 320x240 (crop), ping-pong,
 *              runs continuously at the preview rate -> display task
 *   channel 1  ARGB, a sensor ROI scaled to the NPU input size (here the
 *              central 512x512 -> 128x128), single buffer owned by the
 *              classifier, one frame in flight -> NPU
 *
 * Channel 1 is gated (dst_fps = 0) from the moment a frame completes until
 * the detector reports it is done with it, so the NPU never reads a buffer
 * the ISP is still writing. The two channels are otherwise independent: the
 * preview keeps its rate whatever the NPU is doing.
 *
 * Board wiring and the three things this board does differently from the
 * SDK's stock EVB camera code are in evk_cam_port.h. The start/stop hygiene
 * (MM block reset with the sensor held in reset, sensor stopped before the
 * ISP is torn down, imgproc never stopped) is the set found the hard way in
 * apps/platform/ipro7ai_evk_demo/src/cam.c - see the comments in camera.c.
 */
#ifndef EVK_CAM_H
#define EVK_CAM_H

#include <stdint.h>
#include <stdbool.h>

#define EVK_CAM_OK           0
#define EVK_CAM_ERR_PARAM   -1
#define EVK_CAM_ERR_NODEV   -2   /* no sensor answered on SCCB              */
#define EVK_CAM_ERR_NOMEM   -3   /* PSRAM frame buffer allocation failed    */
#define EVK_CAM_ERR_STATE   -4   /* wrong state for this call               */

/** Preview aspect handling: the sensor is 16:9, the panel is 4:3. */
#define EVK_CAM_FIT_LETTERBOX 0  /* whole frame -> 320x180, black bars (default) */
#define EVK_CAM_FIT_CROP      1  /* centre 960x720 -> 320x240, fills the screen  */

/** Where the preview image sits and which part of the sensor it shows.
 *  The UI maps sensor-pixel rectangles (the NPU ROI) onto the preview with
 *  this. */
typedef struct {
    uint16_t panel_w, panel_h;   /* panel in the chosen orientation        */
    uint16_t img_w, img_h;       /* channel-0 frame size = preview image   */
    uint16_t ox, oy;             /* top-left of the image on the panel     */
    uint16_t crop_x, crop_y;     /* sensor window channel 0 is scaled from */
    uint16_t crop_w, crop_h;
    uint16_t roi_x, roi_y;       /* sensor window channel 1 (NPU) is scaled from */
    uint16_t roi_w, roi_h;
} evk_cam_geometry_t;

/** Channel-1 (NPU input) configuration, set before evk_cam_start(). */
typedef struct {
    uint32_t addr;               /* where the ISP writes; 0 = channel 1 off */
    uint16_t out_w, out_h;       /* scaler output = model input             */
    uint16_t roi_x, roi_y;       /* sensor window to scale from             */
    uint16_t roi_w, roi_h;
} evk_cam_npu_input_t;

typedef struct {
    uint32_t frames0;        /* channel-0 completions                        */
    uint32_t frames1;        /* channel-1 completions                        */
    uint32_t pushed;         /* channel-1 frames handed to the NPU           */
    uint32_t dropped;        /* channel-0 frames superseded before display   */
    uint32_t fifo_err;       /* ISP_HAL_INT_FIFO_OVERWRITE                   */
    uint32_t vsync_err;      /* ISP_HAL_INT_VSYNC_CNT_ERROR                  */
    uint32_t hsync_err;      /* ISP_HAL_INT_HSYNC_CNT_ERROR                  */
} evk_cam_stats_t;

/** New channel-0 frame. ISR context: record the address and wake a task. */
typedef void (*evk_cam_frame_cb_t)(uint32_t yuyv_addr, void *arg);

/** New channel-1 frame for the NPU. ISR context. Return 0 if taken; anything
 *  else means "busy, dropped" (channel 1 is re-opened by evk_cam_npu_rearm()). */
typedef int (*evk_cam_npu_frame_cb_t)(uint32_t argb_addr, void *arg);

/**
 * @brief Bring up SCCB, MCLK, reset and the DVP pins, then probe the sensor.
 * Safe to call repeatedly; the probe re-runs. Does NOT start the ISP.
 */
int evk_cam_probe(void);

/** @brief Channel-1 geometry and buffer. Takes effect on the next evk_cam_start();
 *         addr == 0 (the default) leaves channel 1 off. */
void evk_cam_set_npu_input(const evk_cam_npu_input_t *in);
void evk_cam_set_npu_frame_cb(evk_cam_npu_frame_cb_t cb, void *arg);

/**
 * @brief Start the ISP pipeline. Calls evk_cam_probe() itself.
 *
 * Channel 1 is only enabled if evk_cam_set_npu_input() gave it a buffer; without
 * one the camera is a plain preview. Puts the panel into the configured
 * orientation and clears it.
 */
int evk_cam_start(void);

/** @brief Stop the pipeline and park the sensor in reset. */
void evk_cam_stop(void);

bool evk_cam_is_running(void);

void evk_cam_set_frame_cb(evk_cam_frame_cb_t cb, void *arg);
void evk_cam_get_geometry(evk_cam_geometry_t *out);
void evk_cam_get_stats(evk_cam_stats_t *out);

const char *evk_cam_sensor_name(void);
uint8_t     evk_cam_sensor_addr(void);
void        evk_cam_sensor_size(uint16_t *w, uint16_t *h);

/** @brief Aspect handling; takes effect on the next evk_cam_start(). */
int evk_cam_set_fit(int fit);
int evk_cam_get_fit(void);

/** @brief Preview (channel 0) frame rate, 1..30. Applied live. */
int evk_cam_set_fps(int fps);
int evk_cam_get_fps(void);

/** @brief Panel orientation 0..3 (1 and 3 landscape). Next evk_cam_start(). */
int evk_cam_set_rot(int rot);
int evk_cam_get_rot(void);

/** @brief Feed the NPU or not (default on when channel 1 is configured). Off
 *         gates channel 1 and leaves the preview alone. */
void evk_cam_npu_set_enabled(bool on);
bool evk_cam_npu_enabled(void);

/** @brief Open channel 1 for one more frame, if the feed is enabled. Call it
 *         when the NPU is done with the previous one - one frame in flight. */
void evk_cam_npu_rearm(void);

/** @brief Drive the sensor RESET line by hand (0 = asserted). Wire checks. */
void evk_cam_reset_pin(int level);

#endif /* EVK_CAM_H */
