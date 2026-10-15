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
 * @brief FLEX-24 camera on FPC1: bring-up, probe and a headless YUV stream.
 *
 * No display on this board, so "streaming" means: sensor -> DVP -> ISP ->
 * channel 0 YUYV, scaled 4x down, into a PSRAM ping-pong pair, with frame
 * and error counters, a luma readout and an ASCII thumbnail to prove that
 * real picture data is arriving. The sensor drivers (components/sensor/
 * ov5640.c, ov2640.c) run the sensor's own AEC/AGC/AWB; the ISP firmware's
 * imgproc 3A is not started.
 *
 * Pins (board header): D0-7 GPIO0-7, PCLK 38, VSYNC 39, HREF 40, XCLK 22,
 * RESETB 34 (active low, 10k pull-down), PWDN 35 (active high), SCL 36,
 * SDA 37 (5.1k to VDD_2V8).
 */
#ifndef CAM_H
#define CAM_H

#include <stdint.h>
#include <stdbool.h>

enum {
    CAM_OK = 0,
    CAM_ERR_NODEV = -1,
    CAM_ERR_STATE = -2,
    CAM_ERR_NOMEM = -3,
    CAM_ERR_PARAM = -4,
};

typedef struct {
    uint32_t frames;        /* ch0 completions */
    uint32_t fifo_err;
    uint32_t vsync_err;
    uint32_t hsync_err;
    uint32_t fps_x10;       /* measured over the last status window */
    uint32_t luma_mean;     /* 0..255 over the latest frame */
} cam_stats_t;

/** Power sequence + SCCB probe through the sensor framework. */
int  cam_probe(void);
/** Start the ISP pipeline (probes first if needed). */
int  cam_start(void);
void cam_stop(void);
/** PWDN high, RESETB low, XCLK off. */
void cam_park(void);
void cam_xclk(int on);

bool cam_is_running(void);
const char *cam_sensor_name(void);
uint8_t cam_sensor_addr(void);
void cam_sensor_size(uint16_t *w, uint16_t *h);
void cam_out_size(uint16_t *w, uint16_t *h);
void cam_get_stats(cam_stats_t *out);

/*
 * Address of the newest complete ch0 frame (YUYV, cam_out_size(), PSRAM; the
 * ISP's DMA wrote it, so invalidate the D-cache before reading), or 0 when
 * not streaming. *frames (optional) is the ch0 frame count since cam_start().
 * The buffer is one of two ping-pong buffers: it is rewritten two frames on.
 */
uint32_t cam_latest_frame(uint32_t *frames);

/** ASCII luma thumbnail of the latest frame, `cols` characters wide. */
int  cam_snap(unsigned cols);
/** Call from a task while streaming (often): runs the sensor's once-per-stream
 *  settings (rt_camera_desc.streaming) after the first frames have arrived.
 *  cam_get_stats() and cam_snap() call it too. */
void cam_service(void);
/** Sensor register access through the driver (OV2640: bit 8 = sensor bank). */
int  cam_reg_read(int addr, int *val);
int  cam_reg_write(int addr, int val);
/** Sensor test pattern (colour bars) on/off. */
int  cam_colorbar(bool on);
/**
 * Sample the DVP pads (D0-7, PCLK, VSYNC, HREF) for `ms` and print
 * rising edges and duty per pin. Tells "the sensor sends nothing" from "the
 * SoC capture side does not count it" without a scope. Busy-waits; a running
 * stream is restarted afterwards (the pads leave the camera block meanwhile).
 */
void cam_pins_report(unsigned ms);

/* DVP input options, applied at the next start. */
void cam_set_byte_order(int lower_first);
void cam_set_sync(int vsync_high, int hsync_high);
void cam_get_dvp_opts(int *lower_first, int *vsync_high, int *hsync_high);

#endif /* CAM_H */
