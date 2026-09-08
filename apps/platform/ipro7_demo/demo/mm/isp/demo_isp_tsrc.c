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

#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <drv_mm_misc.h>

#include <sccb.h>
#include <camera.h>
#include <sensor.h>
#include "hal_cam.h"
#include "hal_mjpeg.h"
#include "hal_isp.h"
#include "drv_glb.h"

#define YUV422_FRAME_SIZE(X, Y)        ( (X) * (Y) * 2 )
#define YUV420_Y_FRAME_SIZE(X, Y)      ( (X) * (Y) )
#define YUV420_UV_FRAME_SIZE(X, Y)     ( (X) * (Y) / 2 )
#define YUV400_FRAME_SIZE(X, Y)        ( (X) * (Y) )
#define ARGB_FRAME_SIZE(X, Y)          ( (X) * (Y) * 4 )
#define RGB_FRAME_SIZE(X, Y)           ( (X) * (Y) * 3 )

#define _ISP_TEST_SAMPLE_VERIFICATION_

#if !defined(_ISP_TEST_SAMPLE_VERIFICATION_)
#define IN_WIDTH      (1280)
#define IN_HEIGHT     (720)

#define CROP_START_X  (0)
#define CROP_START_Y  (0)
#define CROP_WIDTH    (1280)
#define CROP_HEIGHT   (720)

#define OUT_WIDTH_0   (1920)
#define OUT_HEIGHT_0  (1080)
#define OUT_FORMAT_0  (ISP_HAL_OUT_FMT0_NV12)

#define OUT_WIDTH_1   (640)
#define OUT_HEIGHT_1  (360)
#define OUT_FORMAT_1  (ISP_HAL_OUT_FMT1_RGB)

#else // set default behavior to do DVT
#include "demo_isp_cfg.h"
#include "isp_dvt_api.h"
#endif

#define SEOF_TEST_LINE_CNT     (200)

// ============================================================================
// local function
// ============================================================================
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
        printf("OUT %ld NORMAL\r\n", info.out_port);
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
        sof_update_param(0);
#endif
        printf("seof 0 SoF %llu\r\n", xTaskGetTickCountFromISR());
    }
    if (type_status.BF.EoF)
    {
        printf("seof 0 EoF %llu\r\n", xTaskGetTickCountFromISR());
    }
    if (type_status.BF.lineCnt)
    {
        printf("seof 0 LineCnt %llu\r\n", xTaskGetTickCountFromISR());
    }
    MM_MISC_Clr_SEOF_TypeStatus(MM_MISC_SEOF0, type_status);
}

static int sample_isp_tsrc(void)
{
    isp_hal_cfg_t isp_cfg = {0};
    uint32_t in_frame_size, in_frame_addr;
    uint32_t out_frame_size_0, out_frame_addr_0 = 0;
    uint32_t out_frame_size_1, out_frame_addr_1 = 0;

    // reset cfg
    memset((void*)&isp_cfg, 0, sizeof(isp_cfg));

    // Input size and address
    in_frame_size = YUV422_FRAME_SIZE(IN_WIDTH, IN_HEIGHT);
    in_frame_addr = (uint32_t) pvPortMalloc( in_frame_size );

    printf("in_frame_size = %lu\r\n", in_frame_size);
    printf("in_frame_addr = 0x%08lx\r\n", in_frame_addr);

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

    isp_cfg.isp_src = ISP_HAL_SRC_TSRC;
    isp_cfg.in_size.width = IN_WIDTH;
    isp_cfg.in_size.height = IN_HEIGHT;

    isp_cfg.src_cfg.tsrc_input.input_addr = in_frame_addr;
    isp_cfg.src_cfg.tsrc_input.tsrc_conti = false;

    isp_cfg.glb_crop.start_x = CROP_START_X;
    isp_cfg.glb_crop.start_y = CROP_START_Y;
    isp_cfg.glb_crop.width = CROP_WIDTH;
    isp_cfg.glb_crop.height = CROP_HEIGHT;

    isp_cfg.yuv_adj_en = 1;     // enable yuvadj by default

    isp_cfg.out_ch_0.enable = (out_frame_size_0)?1:0;
    isp_cfg.out_ch_0.scaler_cfg.in.start_x = 0;
    isp_cfg.out_ch_0.scaler_cfg.in.start_y = 0;
    isp_cfg.out_ch_0.scaler_cfg.in.width = CROP_WIDTH;
    isp_cfg.out_ch_0.scaler_cfg.in.height = CROP_HEIGHT;
    isp_cfg.out_ch_0.scaler_cfg.out.width = OUT_WIDTH_0;
    isp_cfg.out_ch_0.scaler_cfg.out.height = OUT_HEIGHT_0;
    isp_cfg.out_ch_0.format = OUT_FORMAT_0;
    isp_cfg.out_ch_0.ring_line = 0;

    isp_cfg.out_ch_0.chn_common.fr_ctrl.src_fps = 1;
    isp_cfg.out_ch_0.chn_common.fr_ctrl.dst_fps = 1;
    isp_cfg.out_ch_0.chn_common.addr[0] = out_frame_addr_0;
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

    isp_cfg.out_ch_1.chn_common.fr_ctrl.src_fps = 1;
    isp_cfg.out_ch_1.chn_common.fr_ctrl.dst_fps = 1;
    isp_cfg.out_ch_1.chn_common.addr[0] = out_frame_addr_1;
    isp_cfg.out_ch_1.chn_common.pp_num = 1;
    isp_cfg.out_ch_1.chn_common.out_crop.start_x = 0;
    isp_cfg.out_ch_1.chn_common.out_crop.start_y = 0;
    isp_cfg.out_ch_1.chn_common.out_crop.width = OUT_WIDTH_1;
    isp_cfg.out_ch_1.chn_common.out_crop.height = OUT_HEIGHT_1;

    isp_cfg.out_callback = isp_event_cb;

    isp_init(&isp_cfg);

#if defined(_ISP_TEST_SAMPLE_VERIFICATION_)
    // clear buffer
    // memset((void*)in_frame_addr, 0x0, in_frame_size);
    memset((void*)out_frame_addr_0, 0x0, out_frame_size_0);
    memset((void*)out_frame_addr_1, 0x0, out_frame_size_1);
    memset((void*)AE_AVG_BASE, 0x0, 768*2);
    memset((void*)AE_HIST_BASE, 0x0, 1040);

    uint32_t blend_img_addr = 0;

    #ifdef OSD_BLEND_EN
    blend_img_addr = (uint32_t) pvPortMalloc( 512*512*4 );
    printf("osd_blend_addr = 0x%08lx\r\n", blend_img_addr);
    #endif

    isp_set_default_param(blend_img_addr, IN_WIDTH, IN_HEIGHT); // use the default parameter to do test, later may use IQ lib to get the parameters
    isp_module_interrupt(0);
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

    return 0;
}

// ============================================================================
// public function
// ============================================================================
void isp_tsrc_test(void)
{
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
    sample_isp_tsrc();
}

// ============================================================================
// Shell function
// ============================================================================
#include "shell.h"

static int cmd_tsrc_trig(int argc, char **argv)
{
    isp_tsrc_trigger();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_tsrc_trig, tsrc_trig, trigger tsrc);
