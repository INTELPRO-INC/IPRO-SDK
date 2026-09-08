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
#include "hal_isp.h"
#include "drv_glb.h"

#define YUV422_FRAME_SIZE(X, Y)        ( (X) * (Y) * 2 )
#define YUV420_Y_FRAME_SIZE(X, Y)      ( (X) * (Y) )
#define YUV420_UV_FRAME_SIZE(X, Y)     ( (X) * (Y) / 2 )
#define YUV400_FRAME_SIZE(X, Y)        ( (X) * (Y) )
#define ARGB_FRAME_SIZE(X, Y)          ( (X) * (Y) * 4 )
#define RGB_FRAME_SIZE(X, Y)           ( (X) * (Y) * 3 )

// #define LOG_EN

#ifdef LOG_EN
#define LOG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define LOG_PRINTF(fmt, ...)
#endif

// #define _ISP_TEST_SAMPLE_VERIFICATION_

#define IN_WIDTH      (0) // by sensor
#define IN_HEIGHT     (0) // by sensor

#define CROP_START_X  (0)
#define CROP_START_Y  (0)
#define CROP_WIDTH    (240)
#define CROP_HEIGHT   (320)

#define OUT_WIDTH_0   (240)
#define OUT_HEIGHT_0  (320)
#define OUT_FORMAT_0  (ISP_HAL_OUT_FMT0_YUYV)

#define OUT_WIDTH_1   (0)
#define OUT_HEIGHT_1  (0)
#define OUT_FORMAT_1  (ISP_HAL_OUT_FMT1_ARGB)

#define SNS_CAMCLK_PIN (GLB_GPIO_PIN_0)//(GLB_GPIO_PIN_41)

#define SEOF_TEST_LINE_CNT     (200)

#define USE_SPI_0

#if defined(_ISP_TEST_SAMPLE_VERIFICATION_)
#include "isp_dvt_api.h"
#endif

static const rt_camera_desc *cam_desc;

// ============================================================================
// local function
// ============================================================================
static void isp_spi_pin_mux(void)
{
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[] = { GLB_GPIO_PIN_3, GLB_GPIO_PIN_4};
    int i;

    cfg.gpioMode=GPIO_MODE_AF;
    cfg.pullType=GPIO_PULL_UP;
    cfg.drive=1;
    cfg.smtCtrl=1;

    for(i=0;i<sizeof(gpiopins)/sizeof(gpiopins[0]);i++){
        cfg.gpioPin=gpiopins[i];
        #ifdef USE_SPI_0
        cfg.gpioFun=GPIO_FUN_SPI;
        #else
        cfg.gpioFun=GPIO_FUN_SPI1;
        #endif
        GLB_GPIO_Init(&cfg);
    }
}

static void isp_enable_camera_module(int enable)
{
#if defined(CONFIG_FPGA)
#else
    GLB_GPIO_Cfg_Type cfg;
    // CamRefClk
    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive    = 3;
    cfg.smtCtrl  = 1;
    cfg.gpioPin  = SNS_CAMCLK_PIN;

    cfg.gpioFun  = GPIO_FUN_CAM_MISC;
    GLB_GPIO_Init(&cfg);
    // PLL 48Mhz and send to CAMRefClk to 24Mhz
    GLB_Set_CAM_CLK(DISABLE, GLB_CAM_CLK_PLL48M, 1);

    if (enable) {
        GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_PLL48M, 1);
        vTaskDelay(1);
    }
#endif
}

static void set_seof0_cfg(void)
{
    MM_MISC_SEOF_TYPE_E seof_type = {0};
    seof_type.BF.SoF = 1;
    seof_type.BF.EoF = 1;
    seof_type.BF.lineCnt = 1;

    // MM seof 0 set in ISP_Out, and enable SoF and EoF
    MM_MISC_Set_SEOF_LineCnt(MM_MISC_SEOF0, SEOF_TEST_LINE_CNT);
    MM_MISC_Set_SEOF_Src(MM_MISC_SEOF0, MM_MISC_SEOF_INTR_SRC_ISP_OUTPUT);
    MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF0, seof_type);
}

static void isp_event_cb(isp_out_frame_info_t info)
{
    if (info.event == ISP_HAL_INT_NORMAL)
    {
        LOG_PRINTF("OUT %ld NORMAL\r\n", info.out_port);
    }
    if (info.event == ISP_HAL_INT_FIFO_OVERWRITE)
    {
        printf("OUT %ld FIFO_OVERWRITE\r\n", info.out_port);
    }
    if (info.event == ISP_HAL_INT_VSYNC_CNT_ERROR)
    {
        printf("OUT %ld VSYNC_CNT_ERROR\r\n", info.out_port);
    }
    if (info.event == ISP_HAL_INT_HSYNC_CNT_ERROR)
    {
        printf("OUT %ld HSYNC_CNT_ERROR\r\n", info.out_port);
    }
}

static void mm_seof0_isr(void)
{
    MM_MISC_SEOF_TYPE_E type_status;
    MM_MISC_Get_SEOF_TypeStatus(MM_MISC_SEOF0, &type_status);
    if (type_status.BF.SoF)
    {
#if defined(_ISP_TEST_SAMPLE_VERIFICATION_)
        sof_update_param(1);
#endif
        LOG_PRINTF("seof 0 SoF %llu\r\n", xTaskGetTickCountFromISR());
    }
    if (type_status.BF.EoF)
    {
        LOG_PRINTF("seof 0 EoF %llu\r\n", xTaskGetTickCountFromISR());
    }
    if (type_status.BF.lineCnt)
    {
        LOG_PRINTF("seof 0 LineCnt %llu\r\n", xTaskGetTickCountFromISR());
    }
    MM_MISC_Clr_SEOF_TypeStatus(MM_MISC_SEOF0, type_status);
}

static int sample_isp_spi(void)
{
    isp_hal_cfg_t isp_cfg = {0};
    MM_MISC_SPI2DVP_Status info;

    uint32_t out_frame_size_0, out_frame_addr_0 = 0;
    uint32_t out_frame_size_1, out_frame_addr_1 = 0;

    printf("SCCB_Init\r\n");
    SCCB_Init();

    printf("isp_enable_camera_module\r\n");
    isp_enable_camera_module(1);

    isp_spi_pin_mux();

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
    memset((void*)&isp_cfg, 0, sizeof(isp_cfg));

    // Output 0 size and address
    if ((ISP_HAL_OUT_FMT0_YUYV == OUT_FORMAT_0) || (ISP_HAL_OUT_FMT0_YUV422SP == OUT_FORMAT_0))
        out_frame_size_0 = YUV422_FRAME_SIZE(OUT_WIDTH_0, OUT_HEIGHT_0);
    else if (ISP_HAL_OUT_FMT0_NV12 == OUT_FORMAT_0)
        out_frame_size_0 = YUV420_Y_FRAME_SIZE(OUT_WIDTH_0, OUT_HEIGHT_0) + YUV420_UV_FRAME_SIZE(OUT_WIDTH_0, OUT_HEIGHT_0);
    else
        out_frame_size_0 = YUV400_FRAME_SIZE(OUT_WIDTH_0, OUT_HEIGHT_0);

    if (out_frame_size_0)
        out_frame_addr_0 = (uint32_t) pvPortMalloc( out_frame_size_0 );

    printf("out_frame_size_0 = %lu\r\n", out_frame_size_0);
    printf("out_frame_addr_0 = 0x%08lx\r\n", out_frame_addr_0);

    // Output 1 size and address
    if (ISP_HAL_OUT_FMT1_ARGB == OUT_FORMAT_1)
        out_frame_size_1 = ARGB_FRAME_SIZE(OUT_WIDTH_1, OUT_HEIGHT_1);
    else
        out_frame_size_1 = RGB_FRAME_SIZE(OUT_WIDTH_1, OUT_HEIGHT_1);

    if (out_frame_size_1)
        out_frame_addr_1 = (uint32_t) pvPortMalloc( out_frame_size_1 );

    printf("out_frame_size_1 = %lu\r\n", out_frame_size_1);
    printf("out_frame_addr_1 = 0x%08lx\r\n", out_frame_addr_1);

    isp_cfg.isp_src = ISP_HAL_SRC_SPI2DVP;
    isp_cfg.in_size.width = cam_desc->width;
    isp_cfg.in_size.height = cam_desc->height;

    isp_cfg.src_cfg.spi2dvp_input.order   = ISP_HAL_SPI2DVP_ORDER_UV_FIRST;
    isp_cfg.src_cfg.spi2dvp_input.format  = ISP_HAL_SPI2DVP_FORMAT_YUV422;
    isp_cfg.src_cfg.spi2dvp_input.mode    = ISP_HAL_SPI2DVP_MODE_1;
    isp_cfg.src_cfg.spi2dvp_input.endian  = ISP_HAL_SPI2DVP_ENDIAN_LSB;
    #ifdef USE_SPI_0
    isp_cfg.src_cfg.spi2dvp_input.spi_sel = ISP_HAL_SPI2DVP_SEL_SPI_0;
    #else
    isp_cfg.src_cfg.spi2dvp_input.spi_sel = ISP_HAL_SPI2DVP_SEL_SPI_1;
    #endif

    isp_cfg.glb_crop.start_x = CROP_START_X;
    isp_cfg.glb_crop.start_y = CROP_START_Y;
    isp_cfg.glb_crop.width = CROP_WIDTH;
    isp_cfg.glb_crop.height = CROP_HEIGHT;

    isp_cfg.yuv_adj_en = 0;     // enable yuvadj by default

    isp_cfg.out_ch_0.enable = (out_frame_size_0)?1:0;
    isp_cfg.out_ch_0.scaler_cfg.in.start_x = 0;
    isp_cfg.out_ch_0.scaler_cfg.in.start_y = 0;
    isp_cfg.out_ch_0.scaler_cfg.in.width = CROP_WIDTH;
    isp_cfg.out_ch_0.scaler_cfg.in.height = CROP_HEIGHT;
    isp_cfg.out_ch_0.scaler_cfg.out.width = OUT_WIDTH_0;
    isp_cfg.out_ch_0.scaler_cfg.out.height = OUT_HEIGHT_0;
    isp_cfg.out_ch_0.format = OUT_FORMAT_0;
    isp_cfg.out_ch_0.ring_line = 0;

    isp_cfg.out_ch_0.chn_common.fr_ctrl.src_fps = 15;
    isp_cfg.out_ch_0.chn_common.fr_ctrl.dst_fps = 15;
    isp_cfg.out_ch_0.chn_common.addr[0] = out_frame_addr_0;
    isp_cfg.out_ch_0.chn_common.pp_num = 1;
    isp_cfg.out_ch_0.chn_common.out_crop.start_x = 0;
    isp_cfg.out_ch_0.chn_common.out_crop.start_y = 0;
    isp_cfg.out_ch_0.chn_common.out_crop.width = OUT_WIDTH_0;
    isp_cfg.out_ch_0.chn_common.out_crop.height = OUT_HEIGHT_0;

    isp_cfg.out_ch_1.enable = (out_frame_size_1)?1:0;
    isp_cfg.out_ch_1.scaler_cfg.in.start_x = 0;
    isp_cfg.out_ch_1.scaler_cfg.in.start_y = 0;
    isp_cfg.out_ch_1.scaler_cfg.in.width = CROP_WIDTH;
    isp_cfg.out_ch_1.scaler_cfg.in.height = CROP_HEIGHT;
    isp_cfg.out_ch_1.scaler_cfg.out.width = OUT_WIDTH_1;
    isp_cfg.out_ch_1.scaler_cfg.out.height = OUT_HEIGHT_1;
    isp_cfg.out_ch_1.format = OUT_FORMAT_1;

    isp_cfg.out_ch_1.chn_common.fr_ctrl.src_fps = 15;
    isp_cfg.out_ch_1.chn_common.fr_ctrl.dst_fps = 15;
    isp_cfg.out_ch_1.chn_common.addr[0] = out_frame_addr_1;
    isp_cfg.out_ch_1.chn_common.pp_num = 1;
    isp_cfg.out_ch_1.chn_common.out_crop.start_x = 0;
    isp_cfg.out_ch_1.chn_common.out_crop.start_y = 0;
    isp_cfg.out_ch_1.chn_common.out_crop.width = OUT_WIDTH_1;
    isp_cfg.out_ch_1.chn_common.out_crop.height = OUT_HEIGHT_1;

    isp_cfg.out_callback = isp_event_cb;

    isp_init(&isp_cfg);

#if defined(_ISP_TEST_SAMPLE_VERIFICATION_)
    uint32_t blend_img_addr = 0;

    #ifdef OSD_BLEND_EN
    blend_img_addr = (uint32_t) pvPortMalloc( 512*512*4 );
    printf("osd_blend_addr = 0x%08lx\r\n", blend_img_addr);
    #endif

    isp_set_default_param(blend_img_addr, cam_desc->width, cam_desc->height); // use the default parameter to do test, later may use IQ lib to get the parameters
    isp_module_interrupt(1);
#endif

    // set seof interrupt
    set_seof0_cfg();

    // hook callback function for seof interrupt and enable
    Interrupt_Handler_Register(ISP_MM_SEOF0_IRQn, mm_seof0_isr);
    CPU_Interrupt_Enable(ISP_MM_SEOF0_IRQn);

    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)out_frame_addr_0, (unsigned long)out_frame_size_0);
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)out_frame_addr_1, (unsigned long)out_frame_size_1);

    /* start isp and ready to get image */
    isp_start();

    // camera_module_start(cam_desc);
    MM_MISC_SPI2DVP_WaitReady();
    MM_MISC_Get_SPI2DVP_Status(&info);
    printf("img_width:%d, height:%d\r\n", info.spi2dvp_img_w, info.spi2dvp_img_h);
    printf("img_data_size:%d\r\n", info.spi2dvp_data_size);

    return 0;
}
// ============================================================================
// public function
// ============================================================================
void isp_spi_test(void)
{
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SPI0);
    sample_isp_spi();
}
