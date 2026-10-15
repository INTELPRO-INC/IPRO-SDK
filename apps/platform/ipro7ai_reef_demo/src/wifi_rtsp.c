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

/*
 * RTSP MJPEG stream of the Reef camera over the IPRO6S_FIN Wi-Fi:
 * cam.c's ch0 YUYV frames, one SW-mode MJPEG encode per RTSP pull,
 * served at rtsp://<ip>:8554/av0_0. Shell: rtsp_cam on [fps]|off|stats.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <generated/autoconf.h>
#include <system_soc.h>
#include "shell.h"
#include "hal_mjpeg.h"
#include "drv_mm_misc.h"

#include "librtspsrv.h"
#include "ipronet_host.h"
#include "cam.h"

#define RTSP_JPEG_QUALITY      MJPEG_DEFAULT_QUALITY
#define RTSP_JPG_BUF_SIZE      (128u * 1024u)    /* HAL splits it in 2 slots */
#define RTSP_ENC_TIMEOUT_MS    500u
#define RTSP_CAM_FLOW_MS       1000u
#define RTSP_FPS_MAX           30

typedef struct {
    uint32_t pulls;
    uint32_t encoded;
    uint32_t enc_timeout;
    uint32_t enc_err;
    uint32_t too_big;
    uint32_t last_size;
    uint32_t max_size;
    uint64_t bytes;
} rtsp_cam_stats_t;

static SemaphoreHandle_t s_enc_done;
static SemaphoreHandle_t s_lock;
static uint32_t s_jpg_buf;
static uint8_t  s_jpg_head[800];
static uint16_t s_w, s_h;
static bool     s_srv_up;
static bool     s_on;
static bool     s_first_run;
static bool     s_mjpeg_up;
static uint32_t s_last_seq;
static uint32_t s_ts_ms;
static TickType_t s_last_tick;
static TickType_t s_on_tick;
static rtsp_cam_stats_t s_st;

static void rtsp_mjpeg_event(uint32_t event)
{
    if (event == MJENC_HAL_INT_NORMAL) {
        xSemaphoreGiveFromISR(s_enc_done, NULL);
    } else if (event != MJENC_HAL_INT_IDLE) {
        s_st.enc_err++;
    }
}

static int rtsp_mjpeg_up(uint32_t yuv)
{
    struct mjpeg_hal_cfg_t cfg;
    uint32_t len;

    memset(&cfg, 0, sizeof(cfg));
    cam_out_size(&s_w, &s_h);
    if (!s_jpg_buf) {
        s_jpg_buf = (uint32_t)(uintptr_t)pvPortMalloc(RTSP_JPG_BUF_SIZE);
        if (!s_jpg_buf) {
            printf("rtsp_cam: %u B JPEG buffer alloc failed\r\n",
                   (unsigned)RTSP_JPG_BUF_SIZE);
            return -1;
        }
    }
    cfg.format         = MJPEG_HAL_FORMAT_YUV422_YUYV;
    cfg.quality        = RTSP_JPEG_QUALITY;
    cfg.rows           = s_h;
    cfg.resolution_x   = s_w;
    cfg.resolution_y   = s_h;
    cfg.input_bufaddr0 = yuv;
    cfg.input_bufaddr1 = yuv + (uint32_t)s_w * s_h;
    cfg.output_bufaddr = s_jpg_buf;
    cfg.output_bufsize = RTSP_JPG_BUF_SIZE;
    cfg.callback       = rtsp_mjpeg_event;
    if (mjpeg_init(&cfg) != 0) {
        printf("rtsp_cam: mjpeg_init failed\r\n");
        return -1;
    }
    len = mjpeg_headerCreate(YUV_MODE_422, RTSP_JPEG_QUALITY, s_w, s_h, s_jpg_head);
    mjpeg_fill_header_tail(s_jpg_head, len);
    MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_Y, MM_MISC_DVP2AXI_PORT_A);
    MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_C, MM_MISC_DVP2AXI_PORT_B);
    s_first_run = true;
    return 0;
}

static int rtsp_encode(uint32_t yuv, uint32_t *addr, uint32_t *size)
{
    mjpeg_set_input_addr(MJPEG_HAL_SET_ADDR_Y, yuv);
    mjpeg_set_input_addr(MJPEG_HAL_SET_ADDR_UV, yuv + (uint32_t)s_w * s_h);
    if (s_first_run) {
        mjpeg_set_mode(1, 1, 1);     /* SW mode, frame mode, 1 frame */
        mjpeg_start(1);
        s_first_run = false;
    } else {
        mjpeg_sw_run_next();
    }
    if (xSemaphoreTake(s_enc_done, pdMS_TO_TICKS(RTSP_ENC_TIMEOUT_MS)) != pdTRUE) {
        s_st.enc_timeout++;
        return -1;
    }
    if (mjpeg_get_one_frame(addr, size) != 0 || *size == 0u) {
        s_st.enc_err++;
        return -1;
    }
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)*addr, (unsigned long)*size);
    return 0;
}

/* get_frm_t: one fresh camera frame per pull, paced to the RTSP fps */
static int rtsp_cam_get_frm(struct strm_info *si, struct frm_info *fi)
{
    uint32_t seq, yuv, addr, size;
    int fps = rtsp_get_video_fps();
    TickType_t now = xTaskGetTickCount();

    (void)si;
    if (!s_on || !cam_is_running()) {
        return 0;
    }
    if (fps < 1) {
        fps = 1;
    }
    if (s_last_tick != 0 &&
        (TickType_t)(now - s_last_tick) < pdMS_TO_TICKS(1000 / fps)) {
        return 0;
    }
    cam_service();
    yuv = cam_latest_frame(&seq);
    if (yuv == 0u || seq == s_last_seq) {
        return 0;
    }
    if (xSemaphoreTake(s_lock, 0) != pdTRUE) {
        return 0;
    }
    s_st.pulls++;
    if (rtsp_encode(yuv, &addr, &size) != 0) {
        xSemaphoreGive(s_lock);
        return 0;
    }
    if (size > RTSP_JPG_BUF_SIZE / 2u) {
        mjpeg_drop_one_frame();
        s_st.too_big++;
        xSemaphoreGive(s_lock);
        return 0;
    }
    s_last_seq  = seq;
    s_last_tick = now;
    s_ts_ms    += 1000u / (uint32_t)fps;      /* librtspsrv scales ms x90 */
    fi->frm_buf   = (char *)(uintptr_t)addr;
    fi->frm_sz    = (unsigned int)size;
    fi->frm_type  = FRM_TYPE_M;
    fi->timestamp = s_ts_ms;
    mjpeg_drop_one_frame();                   /* next encode uses the other slot */
    s_st.encoded++;
    s_st.bytes    += size;
    s_st.last_size = size;
    if (size > s_st.max_size) {
        s_st.max_size = size;
    }
    xSemaphoreGive(s_lock);
    return (int)size;
}

/* True once the camera has delivered `want` new frames within `ms` */
static bool rtsp_cam_frames_flow(uint32_t want, uint32_t ms)
{
    uint32_t seq0 = 0, seq = 0;
    TickType_t t0 = xTaskGetTickCount();

    (void)cam_latest_frame(&seq0);
    while ((xTaskGetTickCount() - t0) < pdMS_TO_TICKS(ms)) {
        if (cam_latest_frame(&seq) != 0u && seq - seq0 >= want) {
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    return false;
}

/* Camera streaming with frames flowing; starts it when stopped */
static int rtsp_cam_ensure(void)
{
    if (!cam_is_running()) {
        int rc = cam_start();
        if (rc != CAM_OK) {
            printf("rtsp_cam: cam_start -> %d\r\n", rc);
            return rc;
        }
    }
    if (!rtsp_cam_frames_flow(2u, RTSP_CAM_FLOW_MS)) {
        printf("rtsp_cam: camera gives no frames - `cam stop`, then retry\r\n");
        return -1;
    }
    cam_service();                            /* sensor streaming hook */
    return 0;
}

static void rtsp_cam_print_url(void)
{
    ipronet_ip_info_t ip;

    if (ipronet_get_ip(&ip) == IPRONET_OK) {
        printf("rtsp_cam: rtsp://%u.%u.%u.%u:%d/av0_0\r\n",
               ip.ip4[0], ip.ip4[1], ip.ip4[2], ip.ip4[3], CONFIG_RTSP_LISTEN_PORT);
    } else {
        printf("rtsp_cam: no IP yet - rtsp://<iproeth0 IP>:%d/av0_0 once connected\r\n",
               CONFIG_RTSP_LISTEN_PORT);
    }
}

static int rtsp_cam_on(int fps)
{
    if (s_on) {
        rtsp_set_video_fps(fps);
        printf("rtsp_cam: already on, fps -> %d\r\n", fps);
        rtsp_cam_print_url();
        return 0;
    }
    if (!s_enc_done) {
        s_enc_done = xSemaphoreCreateBinary();
        s_lock     = xSemaphoreCreateMutex();
        if (!s_enc_done || !s_lock) {
            printf("rtsp_cam: semaphore alloc failed\r\n");
            return -1;
        }
    }
    if (rtsp_cam_ensure() != 0) {
        printf("rtsp_cam: camera not streaming\r\n");
        return -1;
    }
    if (!s_mjpeg_up) {
        if (rtsp_mjpeg_up(cam_latest_frame(NULL)) != 0) {
            return -1;
        }
        s_mjpeg_up = true;
    }
    memset(&s_st, 0, sizeof(s_st));
    s_last_seq  = 0;
    s_last_tick = 0;
    s_ts_ms     = 0;
    s_on_tick   = xTaskGetTickCount();
    set_strm_cb(rtsp_cam_get_frm);
    rtsp_set_video_en(1);
    rtsp_set_videoFmt(RTSP_VIDEOFMT_MJPEG);
    rtsp_set_video_fps(fps);
    s_on = true;
    if (!s_srv_up) {
        if (rtsp_init_lib() != 0) {
            printf("rtsp_cam: rtsp_init_lib failed\r\n");
            s_on = false;
            return -1;
        }
        s_srv_up = true;
    }
    printf("rtsp_cam: on, %s %ux%u JPEG q%d, %d fps\r\n",
           cam_sensor_name() ? cam_sensor_name() : "?", (unsigned)s_w,
           (unsigned)s_h, RTSP_JPEG_QUALITY, fps);
    rtsp_cam_print_url();
    return 0;
}

/* Stops serving frames; the camera, encoder and server keep running */
static void rtsp_cam_off(void)
{
    if (!s_on) {
        printf("rtsp_cam: already off\r\n");
        return;
    }
    s_on = false;
    xSemaphoreTake(s_lock, portMAX_DELAY);    /* no encode in flight */
    xSemaphoreGive(s_lock);
    printf("rtsp_cam: off (camera keeps running, `cam stop` stops it)\r\n");
}

static void rtsp_cam_stats(void)
{
    uint32_t secs = (uint32_t)((xTaskGetTickCount() - s_on_tick) / configTICK_RATE_HZ);
    uint32_t avg  = s_st.encoded ? (uint32_t)(s_st.bytes / s_st.encoded) : 0u;
    uint32_t seq  = 0;

    (void)cam_latest_frame(&seq);
    printf("rtsp_cam: %s, server %s, %ux%u q%d, fps cfg %d\r\n",
           s_on ? "on" : "off", s_srv_up ? "up" : "down", (unsigned)s_w,
           (unsigned)s_h, RTSP_JPEG_QUALITY, rtsp_get_video_fps());
    printf("  camera %s, %lu frames since its start\r\n",
           cam_is_running() ? "streaming" : "stopped", (unsigned long)seq);
    printf("  pulls %lu, encoded %lu (~%lu fps over %lu s)\r\n",
           (unsigned long)s_st.pulls, (unsigned long)s_st.encoded,
           (unsigned long)(secs ? s_st.encoded / secs : 0u), (unsigned long)secs);
    printf("  jpeg last %lu B, avg %lu B, max %lu B, total %lu KB\r\n",
           (unsigned long)s_st.last_size, (unsigned long)avg,
           (unsigned long)s_st.max_size, (unsigned long)(s_st.bytes / 1024u));
    printf("  enc timeout %lu, enc err %lu, too big %lu\r\n",
           (unsigned long)s_st.enc_timeout, (unsigned long)s_st.enc_err,
           (unsigned long)s_st.too_big);
}

static int cmd_rtsp_cam(int argc, char **argv)
{
    if (argc >= 2 && !strcmp(argv[1], "on")) {
        int fps = (argc >= 3) ? atoi(argv[2]) : CONFIG_RTSP_DEFAULT_VIDEO_FPS;
        if (fps < 1 || fps > RTSP_FPS_MAX) {
            printf("rtsp_cam: fps 1..%d\r\n", RTSP_FPS_MAX);
            return -1;
        }
        return rtsp_cam_on(fps);
    }
    if (argc >= 2 && !strcmp(argv[1], "off")) {
        rtsp_cam_off();
        return 0;
    }
    if (argc >= 2 && !strcmp(argv[1], "stats")) {
        rtsp_cam_stats();
        return 0;
    }
    printf("usage: rtsp_cam on [fps] | off | stats\r\n");
    return -1;
}

SHELL_CMD_EXPORT_ALIAS(cmd_rtsp_cam, rtsp_cam, "RTSP MJPEG camera stream: on [fps] | off | stats");
