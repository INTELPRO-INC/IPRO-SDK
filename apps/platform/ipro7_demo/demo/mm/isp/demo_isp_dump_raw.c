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

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <drv_mm_misc.h>

#include <sccb.h>
#include <camera.h>
#include <sensor.h>
#include "hal_cam.h"
#include "drv_glb.h"
#include "drv_clock.h"

#define YUV422_FRAME_SIZE(X, Y)        ( (X) * (Y) * 2 )
#define YUV420_Y_FRAME_SIZE(X, Y)      ( (X) * (Y) )
#define YUV400_FRAME_SIZE(X, Y)        ( (X) * (Y) )

// #define LOG_EN

#ifdef LOG_EN
#define LOG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define LOG_PRINTF(fmt, ...)
#endif

#define OUT_RING_LINE (0)
#define OUT_FORMAT    (CAM_HAL_OUT_FMT_YUYV)

#define SNS_PWR_PIN        (GLB_GPIO_PIN_34)
#define SNS_RST_PIN        (GLB_GPIO_PIN_35)
#define SNS_DVP_CAMCLK_PIN (GLB_GPIO_PIN_41)
#define SNS_SPI_CAMCLK_PIN (GLB_GPIO_PIN_0)

#define SEOF_TEST_LINE_CNT (200)

static const rt_camera_desc *cam_desc;

// ============================================================================
// local function
// ============================================================================
static void cam_pin_mux(int in_src)
{
    GLB_GPIO_Cfg_Type cfg;
    int i;

    if (CAM_HAL_SRC_DVP_AS == in_src)
    {
        uint8_t gpiopins[] = { GLB_GPIO_PIN_0, GLB_GPIO_PIN_1, GLB_GPIO_PIN_2, GLB_GPIO_PIN_3,
                               GLB_GPIO_PIN_4, GLB_GPIO_PIN_5, GLB_GPIO_PIN_6, GLB_GPIO_PIN_7,
                               GLB_GPIO_PIN_38, GLB_GPIO_PIN_39, GLB_GPIO_PIN_40};

        cfg.gpioMode=GPIO_MODE_AF;
        cfg.pullType=GPIO_PULL_NONE;
        cfg.drive=0;
        cfg.smtCtrl=1;

        for(i=0;i<sizeof(gpiopins)/sizeof(gpiopins[0]);i++){
            cfg.gpioPin=gpiopins[i];
            cfg.gpioFun=GPIO_FUN_CAM;
            GLB_GPIO_Init(&cfg);
        }
    }
    else
    {
        uint8_t gpiopins[] = { GLB_GPIO_PIN_3, GLB_GPIO_PIN_4};

        cfg.gpioMode=GPIO_MODE_AF;
        cfg.pullType=GPIO_PULL_UP;
        cfg.drive=1;
        cfg.smtCtrl=1;

        for(i=0;i<sizeof(gpiopins)/sizeof(gpiopins[0]);i++){
            cfg.gpioPin=gpiopins[i];
            cfg.gpioFun=GPIO_FUN_SPI;
            GLB_GPIO_Init(&cfg);
        }
    }
}

static void cam_enable_camera_module(int enable, int in_src)
{
    GLB_GPIO_Cfg_Type cfg;
    // CamRefClk
    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive    = 3;
    cfg.smtCtrl  = 1;
    if (CAM_HAL_SRC_DVP_AS == in_src)
    {
        cfg.gpioPin  = SNS_DVP_CAMCLK_PIN;
        printf("dvp cam\r\n");
    }
    else
        cfg.gpioPin  = SNS_SPI_CAMCLK_PIN;

    cfg.gpioFun  = GPIO_FUN_CAM_MISC;
    GLB_GPIO_Init(&cfg);
    // PLL 48Mhz and send to CAMRefClk to 24Mhz
    GLB_Set_CAM_CLK(DISABLE, GLB_CAM_CLK_PLL48M, 1);

    if (CAM_HAL_SRC_DVP_AS == in_src)
    {
        // Cfg power and reset pin to GPIO function and GPO low
        cfg.gpioMode   = GPIO_MODE_OUTPUT;
        cfg.pullType   = GPIO_PULL_UP;

        // power pin
        cfg.gpioPin = SNS_PWR_PIN;
        cfg.gpioFun = GPIO_FUN_GPIO;
        GLB_GPIO_Init(&cfg);
        GLB_GPIO_Write(SNS_PWR_PIN, 0);
        GLB_GPIO_OUTPUT_Enable(SNS_PWR_PIN);

        // reset pin
        cfg.gpioPin = SNS_RST_PIN;
        cfg.gpioFun = GPIO_FUN_GPIO;
        GLB_GPIO_Init(&cfg);
        GLB_GPIO_Write(SNS_RST_PIN, 0);
        GLB_GPIO_OUTPUT_Enable(SNS_RST_PIN);
    }

    if (enable) {
        GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_PLL48M, 1);
        vTaskDelay(1);
        if (CAM_HAL_SRC_DVP_AS == in_src)
        {
            // power high
            GLB_GPIO_Write(SNS_PWR_PIN, 1);
            // reset high
            GLB_GPIO_Write(SNS_RST_PIN, 1);
            vTaskDelay(12);
        }
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

static int sample_cam_dump_raw(int in_src)
{
    cam_hal_cfg_t cam_cfg = {0};
    uint32_t out_buffer_height_0 = 0;

    uint32_t out_frame_size_0, out_frame_addr_0 = 0;

    printf("SCCB_Init\r\n");
    SCCB_Init();

    printf("cam_enable_camera_module\r\n");
    cam_enable_camera_module(1, in_src);

    cam_pin_mux(in_src);

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

    cam_cfg.cam_src = CAM_HAL_SRC_DVP_AS;
    cam_cfg.in_size.width = cam_desc->width;
    cam_cfg.in_size.height = cam_desc->height;

    cam_cfg.src_cfg.dvp_input.hsync = CAM_HAL_DVP_ACTIVE_HIGH;
    cam_cfg.src_cfg.dvp_input.vsync = CAM_HAL_DVP_ACTIVE_HIGH;
    cam_cfg.src_cfg.dvp_input.order = CAM_HAL_DATA_BYTE_LOWER;
    cam_cfg.src_cfg.dvp_input.pclk = cam_desc->pclk;
    cam_cfg.src_cfg.dvp_input.totalHPixels = cam_desc->total_width;

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

    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)out_frame_addr_0, (unsigned long)out_frame_size_0);

    /* start cam and ready to get image */
    cam_start();

    return 0;
}
// ============================================================================
// public function
// ============================================================================
void cam_dump_raw_test(int in_src)
{
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
    sample_cam_dump_raw(in_src);
}
