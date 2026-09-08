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
 * @file cam.h
 * @brief SC101IOT DVP camera -> ST7789P3 LCD preview.
 *
 * Pipeline: sensor (1280x720 DVP, 8 of its 10 data lines) -> ISP -> scaler
 * output channel 1 in ARGB -> PSRAM -> CPU packs to RGB565 -> SPI to the panel.
 *
 * Channel 1 is used rather than channel 0 because the ISP's hardware YUV-to-RGB
 * block sits on that path: the CPU then only has to repack 8-8-8 into 5-6-5,
 * instead of doing a full YUV422 colour conversion per pixel.
 *
 * See cam_port.h for the board wiring and the three things this board does
 * differently from the SDK's stock EVB camera code.
 */
#ifndef CAM_H
#define CAM_H

#include <stdint.h>
#include <stdbool.h>

#define CAM_OK           0
#define CAM_ERR_PARAM   -1
#define CAM_ERR_NODEV   -2   /* no sensor answered on SCCB */
#define CAM_ERR_NOMEM   -3   /* PSRAM frame buffer allocation failed */
#define CAM_ERR_STATE   -4   /* wrong state for this call */

/**
 * Which colour channel sits in byte 0 of an ISP ch1 ARGB pixel.
 *
 * The correct value depends on whether the sensor's IQ parameters are loaded:
 * without them the ISP uses the hardcoded matrix in hal_isp.c and byte 0 is
 * RED; with them the IQ table's own colour matrix takes over and byte 0 is
 * BLUE. Both were measured on the bench, a link fix apart. CAM_ORDER_BGR is
 * the default because the IQ table now always loads.
 *
 * Treat neither as fixed - switch live with `cam order` and read the answer
 * off a red scene with `cam pixels`.
 */
#define CAM_ORDER_RGB 0   /* byte0 = R, byte1 = G, byte2 = B (default) */
#define CAM_ORDER_BGR 1   /* byte0 = B, byte1 = G, byte2 = R           */

/** Preview aspect handling: the sensor is 16:9, the panel is 4:3. */
#define CAM_FIT_LETTERBOX 0  /* whole frame -> 320x180, black bars (default) */
#define CAM_FIT_CROP      1  /* centre 960x720 -> 320x240, fills the screen  */

typedef struct {
    uint32_t frames;        /* ISP ch1 completions seen                     */
    uint32_t drawn;         /* frames actually blitted to the panel         */
    uint32_t dropped;       /* frames superseded before the blit could start*/
    uint32_t fifo_err;      /* ISP_HAL_INT_FIFO_OVERWRITE                   */
    uint32_t vsync_err;     /* ISP_HAL_INT_VSYNC_CNT_ERROR                  */
    uint32_t hsync_err;     /* ISP_HAL_INT_HSYNC_CNT_ERROR                  */
    uint32_t last_ms;       /* wall time of the last completed blit         */
} cam_stats_t;

/**
 * @brief Bring up SCCB, MCLK, reset and the DVP pins, then probe the sensor.
 *
 * Safe to call repeatedly; the probe re-runs. Does NOT start the ISP.
 * @return CAM_OK, or CAM_ERR_NODEV if nothing answered on the SCCB bus.
 */
int cam_probe(void);

/**
 * @brief Start the ISP pipeline and the preview task.
 *
 * Puts the panel into landscape (320x240) and clears it first. Calls
 * cam_probe() itself if that has not been done.
 * @return CAM_OK or a CAM_ERR_* code.
 */
int cam_start(void);

/** @brief Stop the preview task and halt the pipeline. */
void cam_stop(void);

/** @brief True while the preview is streaming. */
bool cam_is_running(void);

/** @brief Sensor name from the probed descriptor, or NULL if none. */
const char *cam_sensor_name(void);

/** @brief SCCB address of the probed sensor, 0 if none. */
uint8_t cam_sensor_addr(void);

/** @brief Native sensor resolution from the probed descriptor. */
void cam_sensor_size(uint16_t *w, uint16_t *h);

/** @brief Size of the preview image actually drawn on the panel. */
void cam_preview_size(uint16_t *w, uint16_t *h);

/**
 * @brief Panel orientation the preview uses, 0..3.
 *
 * 1 and 3 are the two landscape orientations (180 degrees apart); 0 and 2 are
 * the two portrait ones. Which reads as upright depends on how the panel is
 * mounted, so it is a runtime setting. Takes effect on the next cam_start().
 */
int cam_set_rot(int rot);
int cam_get_rot(void);

/** @brief Aspect handling; takes effect on the next cam_start(). */
int  cam_set_fit(int fit);
int  cam_get_fit(void);

/** @brief Target preview frame rate, 1..30. Applied live. */
int  cam_set_fps(int fps);
int  cam_get_fps(void);

/** @brief Channel order of the ISP ch1 frame. Applied live - use it when red
 *         and blue come out swapped. */
int  cam_set_order(int order);
int  cam_get_order(void);

/** @brief Snapshot of the running counters. */
void cam_get_stats(cam_stats_t *out);

/** @brief Drive the sensor RESET line by hand (0 = asserted). Wire checks
 *         only - it leaves the sensor in whatever state you put it in. */
void cam_reset_pin(int level);

/**
 * @brief Dump the first @p n ARGB words of the newest frame as hex.
 *
 * Prints each word with both channel orders spelled out, so the correct
 * setting for `cam order` can be read off a known scene rather than guessed.
 * @return CAM_OK or CAM_ERR_STATE if no frame has been captured yet.
 */
int cam_dump_pixels(uint32_t n);

#endif /* CAM_H */
