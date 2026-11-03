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

#include <generated/autoconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler/common.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "sccb.h"
#include "hal_mjpeg.h"
#include "camera.h"

#include <sensor.h>
#include "hal_cam.h"
#include "drv_glb.h"
#include "drv_clock.h"
#include "drv_mjpeg.h"
#include "drv_mm_misc.h"

#define YUV422_FRAME_SIZE(X, Y)        ( (X) * (Y) * 2 )
#define YUV400_FRAME_SIZE(X, Y)        ( (X) * (Y) )

// #define LOG_EN

#ifdef LOG_EN
#define LOG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define LOG_PRINTF(fmt, ...)
#endif

#define OUT_RING_LINE (16)
#define OUT_FORMAT    (CAM_HAL_OUT_FMT_YUYV)



static uint8_t  jpg_head_buf[800] = { 0 };
static uint32_t jpg_head_len;

static const rt_camera_desc *cam_desc;
static SemaphoreHandle_t gJencNotify;

static uint8_t *gPicStreamBuf = 0;
static uint32_t gPicStreamBufSize = 512*1024;
static uint32_t gPicStreamBufPtr = 0;
static uint8_t gPicStreamFull = 0;

// ============================================================================
// local function
// ============================================================================
static void cam_pin_mux(void)
{

}

static void cam_enable_camera_module(int enable)
{
    writel(0, 0x30017000); // pull down reset and power down pin

    if (enable) {
        vTaskDelay(1);

        writel(0xC0000000, 0x30017000); // pull up reset and power down pin

        vTaskDelay(12); // see sensor datasheet
    }
}

static void mjpeg_enc_event_cb(uint32_t event)
{
    if (event == MJENC_HAL_INT_NORMAL)
        xSemaphoreGiveFromISR(gJencNotify, NULL);
    else
    {
        if (event == MJENC_HAL_INT_IDLE)
        {
            //do nothing
        }
        else
            printf("Unknown jenc event %lu\r\n", event);
    }
}

static void cam_event_cb(cam_out_frame_info_t info)
{
    if (info.event == CAM_HAL_INT_NORMAL)
    {
        LOG_PRINTF("NORMAL\r\n");
    }
    if (info.event == CAM_HAL_INT_FIFO_OVERWRITE)
    {
        printf("FIFO_OVERWRITE\r\n");
    }
    if (info.event == CAM_HAL_INT_VSYNC_CNT_ERROR)
    {
        printf("VSYNC_CNT_ERROR\r\n");
    }
    if (info.event == CAM_HAL_INT_HSYNC_CNT_ERROR)
    {
        printf("HSYNC_CNT_ERROR\r\n");
    }
}

static void get_mjpg_info(uint8_t cam_format, uint8_t *mjpg_format, uint8_t *mjpg_mode)
{
    switch(cam_format)
    {
        case CAM_HAL_OUT_FMT_YUYV:
            *mjpg_format = MJPEG_HAL_FORMAT_YUV422_YUYV;
            *mjpg_mode = YUV_MODE_422;
        break;
        case CAM_HAL_OUT_FMT_YUV422SP:
            *mjpg_format = MJPEG_HAL_FORMAT_YUV422SP_NV16;
            *mjpg_mode = YUV_MODE_422;
        break;
        case CAM_HAL_OUT_FMT_Y_ONLY:
            *mjpg_format = MJPEG_HAL_FORMAT_GRAY;
            *mjpg_mode = YUV_MODE_400;
        break;
        default:
            *mjpg_format = MJPEG_HAL_FORMAT_YUV422_YUYV;
            *mjpg_mode = YUV_MODE_422;
            printf("Unknown cam format 0 %d\r\n", cam_format);
        break;
    }
}

static void stream_pipe_init(int use_dvp)
{
    cam_hal_cfg_t cam_cfg = {0};
    uint32_t out_buffer_height_0 = 0;

    uint32_t out_frame_size_0, out_frame_addr_0 = 0;

    struct mjpeg_hal_cfg_t mjpeg_cfg;
    uint32_t jenc_size, jenc_addr;
    uint8_t mjpg_format, mjpg_yuv_mode;

    gPicStreamBuf = pvPortMalloc(gPicStreamBufSize);

    printf("SCCB_Init\r\n");
    SCCB_Init();

    printf("cam_enable_camera_module\r\n");
    cam_enable_camera_module(1);

    cam_pin_mux();

    printf("camera_module_init\r\n");
    while(1) {
        cam_desc = camera_module_init(); // probe sensor + reset sensor (bringup sensor)
        if (NULL == cam_desc) {
            printf("Error: cam_desc is NULL\r\n");
            continue;
        }
        break;
    }

    camera_module_start(cam_desc);

    // reset cfg
    memset((void*)&cam_cfg, 0, sizeof(cam_cfg));

    out_buffer_height_0 = (OUT_RING_LINE == 0)?cam_desc->height:OUT_RING_LINE;

    // Output 0 size and address
    if ((CAM_HAL_OUT_FMT_YUYV == OUT_FORMAT) || (CAM_HAL_OUT_FMT_YUV422SP == OUT_FORMAT))
        out_frame_size_0 = YUV422_FRAME_SIZE(cam_desc->width, out_buffer_height_0);
    else
        out_frame_size_0 = YUV400_FRAME_SIZE(cam_desc->width, out_buffer_height_0);

    if (out_frame_size_0)
        out_frame_addr_0 = (uint32_t) pvPortMalloc( out_frame_size_0 );

    printf("out_frame_size_0 = %lu\r\n", out_frame_size_0);
    printf("out_frame_addr_0 = 0x%08lx\r\n", out_frame_addr_0);

    jenc_size = 256*1024;
    jenc_addr = (uint32_t) pvPortMalloc( jenc_size );
    if (!jenc_addr) {
        printf("%s[%d] error: memory malloc failed! (size: %lu)\r\n", __func__, __LINE__, jenc_size);
        return;
    }
    if (1 == use_dvp)
    {
    cam_cfg.cam_src = CAM_HAL_SRC_DVP_AS;
    cam_cfg.in_size.width = cam_desc->width;
    cam_cfg.in_size.height = cam_desc->height;

    cam_cfg.src_cfg.dvp_input.hsync = CAM_HAL_DVP_ACTIVE_HIGH;
    cam_cfg.src_cfg.dvp_input.vsync = CAM_HAL_DVP_ACTIVE_HIGH;
    cam_cfg.src_cfg.dvp_input.order = CAM_HAL_DATA_BYTE_LOWER;
    cam_cfg.src_cfg.dvp_input.pclk = cam_desc->pclk;
    cam_cfg.src_cfg.dvp_input.totalHPixels = cam_desc->total_width;
    }
    else
    {
        cam_cfg.cam_src = CAM_HAL_SRC_SPI2DVP;
        cam_cfg.in_size.width = cam_desc->width;
        cam_cfg.in_size.height = cam_desc->height;
        cam_cfg.src_cfg.spi2dvp_input.order = CAM_HAL_SPI2DVP_ORDER_UV_FIRST;
        cam_cfg.src_cfg.spi2dvp_input.format = CAM_HAL_SPI2DVP_FORMAT_YUV422;
        cam_cfg.src_cfg.spi2dvp_input.mode = CAM_HAL_SPI2DVP_MODE_1;
        cam_cfg.src_cfg.spi2dvp_input.endian = CAM_HAL_SPI2DVP_ENDIAN_LSB;
    }

    cam_cfg.output_cfg.format = OUT_FORMAT;
    cam_cfg.output_cfg.ring_line = OUT_RING_LINE;

    cam_cfg.output_cfg.fr_ctrl.src_fps = 30;
    cam_cfg.output_cfg.fr_ctrl.dst_fps = 30;
    cam_cfg.output_cfg.addr[0] = out_frame_addr_0;
    cam_cfg.output_cfg.pp_num = 1;
    cam_cfg.output_cfg.out_crop.start_x = 0;
    cam_cfg.output_cfg.out_crop.start_y = 0;
    cam_cfg.output_cfg.out_crop.width = cam_desc->width;
    cam_cfg.output_cfg.out_crop.height = cam_desc->height;

    cam_cfg.out_callback = cam_event_cb;

    cam_init(&cam_cfg);


    get_mjpg_info(cam_cfg.output_cfg.format, &mjpg_format, &mjpg_yuv_mode);

    mjpeg_cfg.format = mjpg_format;
    mjpeg_cfg.quality = MJPEG_DEFAULT_QUALITY;
    mjpeg_cfg.rows = OUT_RING_LINE;
    mjpeg_cfg.resolution_x = cam_desc->width;
    mjpeg_cfg.resolution_y = cam_desc->height;
    mjpeg_cfg.input_bufaddr0 = out_frame_addr_0;
    mjpeg_cfg.input_bufaddr1 = out_frame_addr_0 + YUV400_FRAME_SIZE(cam_desc->width, out_buffer_height_0);
    mjpeg_cfg.output_bufaddr = jenc_addr;
    mjpeg_cfg.output_bufsize = jenc_size;
    mjpeg_cfg.input_yy_table = NULL;
    mjpeg_cfg.input_uv_table = NULL;
    mjpeg_cfg.output_swap_mode_en = 0;    /*enable encode output buffer swap mode, 0 frame mode, 1 swap mode.*/
    mjpeg_cfg.hw_mode_swen_en = 1;
    mjpeg_cfg.callback = mjpeg_enc_event_cb;

    mjpeg_init(&mjpeg_cfg);

    jpg_head_len = mjpeg_headerCreate(mjpg_yuv_mode, MJPEG_DEFAULT_QUALITY, cam_desc->width, cam_desc->height, jpg_head_buf);
    mjpeg_fill_header_tail(jpg_head_buf, jpg_head_len);
    mjpeg_start(0);

    /* start cam and ready to get image */
    cam_start();
    if (0 == use_dvp)
    {
        MM_MISC_SPI2DVP_Status info;
        MM_MISC_SPI2DVP_WaitReady();
        MM_MISC_Get_SPI2DVP_Status(&info);
        printf("img_width:%d, height:%d\r\n", info.spi2dvp_img_w, info.spi2dvp_img_h);
        printf("img_data_size:%d\r\n", info.spi2dvp_data_size);
    }
}

static void strmctrl_get_output_proc(void *arg) // task
{
    int ret = 0;
    uint32_t jenc_size, jenc_addr, jenc_sec_half_addr, jenc_sec_half_len, jenc_total_size;

    while (1) {
        xSemaphoreTake(gJencNotify, portMAX_DELAY);
        do{
        ret = mjpeg_get_one_frame(&jenc_addr, &jenc_size, &jenc_sec_half_addr, &jenc_sec_half_len);
        if (ret == 0) {
            MInvalDCacheRange((unsigned long)jenc_addr, (unsigned long)jenc_size);
            jenc_total_size = jenc_size;

            if (jenc_sec_half_len != 0)
            {
                MInvalDCacheRange(jenc_sec_half_addr, jenc_sec_half_len);
                jenc_total_size += jenc_sec_half_len;
            }
            // show_stream_buf(jenc_addr, jenc_size, jenc_sec_half_addr, jenc_sec_half_len);

            if (!gPicStreamFull) {
                if (gPicStreamBufPtr + jenc_total_size > gPicStreamBufSize) {
                    printf("\r\n\r\nStream Full, dump binary memory test.mjpeg 0x%lx 0x%lx\r\n\r\n", (uint32_t)gPicStreamBuf, (uint32_t)gPicStreamBuf + gPicStreamBufPtr);
                    gPicStreamFull = 1;
                } else {
                        printf("Frame ready 0x%lx 0x%lx\r\n", (uint32_t)jenc_addr, (uint32_t)jenc_addr + jenc_size);
                    memcpy(gPicStreamBuf + gPicStreamBufPtr, (void *)jenc_addr, jenc_size);
                    gPicStreamBufPtr += jenc_size;
                    if (jenc_sec_half_len != 0)
                    {
                            printf("Sec Frame ready 0x%lx 0x%lx\r\n", (uint32_t)jenc_sec_half_addr, (uint32_t)jenc_sec_half_addr + jenc_sec_half_len);
                        memcpy(gPicStreamBuf + gPicStreamBufPtr, (void *)jenc_sec_half_addr, jenc_sec_half_len);
                        gPicStreamBufPtr += jenc_sec_half_len;
                    }
                }
            }
                mjpeg_drop_one_frame();
        }
        }while(0 == ret);
    }

    vSemaphoreDelete(gJencNotify);
    gJencNotify = NULL;
    vTaskDelete(NULL);
}

// ============================================================================
// public function
// ============================================================================

void strmctrl_init(int use_dvp)
{
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_CAM);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM_MISC);

    gJencNotify = xSemaphoreCreateCounting(1, 0);
    xTaskCreate(strmctrl_get_output_proc, "getoutput", 512, NULL, 10, NULL);

    stream_pipe_init(use_dvp); // config cam + jpg + sensor stream
}

void strmctrl_get_next(void)
{
    gPicStreamFull = 0;
    gPicStreamBufPtr = 0;
    memset(gPicStreamBuf, 0, gPicStreamBufSize);

    mjpeg_drop_one_frame();
}

void strmctrl_set_frc(int demo_id)
{
    cam_hal_frame_rate_t frc_cfg = {0};
    // 0 for always output
    // 1 for half frame rate
    // 2 for drop

    // assume input is 30fps
    if (0 == demo_id)
    {
        frc_cfg.src_fps = 30;
        frc_cfg.dst_fps = 30;
    }
    else if (1 == demo_id)
    {
        frc_cfg.src_fps = 30;
        frc_cfg.dst_fps = 15;
    }
    else if (2 == demo_id)
    {
        frc_cfg.src_fps = 30;
        frc_cfg.dst_fps = 0;
    }

    cam_fps_cfg(frc_cfg);
}
