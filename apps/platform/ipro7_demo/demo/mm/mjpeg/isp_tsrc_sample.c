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

#include "FreeRTOS.h"
#include "task.h"
#include "isp_tsrc_sample.h"
#include "drv_dvp2axi.h"
#include "drv_glb.h"
#include "drv_mm_misc.h"
#include "drv_tsrc.h"
#include "drv_scaler_hydre.h"
#include "demo_mjpeg_cfg.h"

#define TSRC_OUT_WIDTH         (1920)
#define TSRC_OUT_HEIGHT        (1080)
#define TSRC_OUT_Y_ADDR        (MJENC_TEST_YUV_BUF_ADDR)  /*OCRAM: (0x11010000)*/ /*PSRAM:(0x1eb00000)*/
#define TSRC_OUT_C_ADDR        (TSRC_OUT_Y_ADDR  + TSRC_OUT_WIDTH*TSRC_OUT_RING_LINE)   /*suppose uses to ring with JPEG, need 32 line for YUV420 format*/
#ifndef TSRC_OUT_RING_LINE
    #define TSRC_OUT_RING_LINE     (1080)                     /*move to demo_mjpeg_cfg.h*/
#endif
#ifndef TSRC_OUT_FORMAT
    #define TSRC_OUT_FORMAT        (ISP_TSRC_OUT_FMT0_YUYV)   /*move to demo_mjpeg_cfg.h*/
#endif



typedef enum
{
    ISP_TSRC_OUT_FMT0_YUYV,
    ISP_TSRC_OUT_FMT0_YUV422SP,
    ISP_TSRC_OUT_FMT0_NV12,
    ISP_TSRC_OUT_FMT0_Y_ONLY,
}ISP_TSRC_OUT_FMT0;




// static void (*g_ispCallback)(uint8_t output, uint32_t event);

static void isp_isr(uint8_t output)
{
    DVP2AXI_PORT_E port = (0 == output)?DVP2AXI_PORT_A:DVP2AXI_PORT_C;

    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_NORMAL)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_NORMAL);
        printf("OUT %d NORMAL\r\n", output);
        // if (g_ispCallback)
            // g_ispCallback(output, ISP_HAL_INT_NORMAL);
    }
    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_FIFO_OVERWRITE)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_FIFO_OVERWRITE);
        printf("OUT %d FIFO_OVERWRITE\r\n", output);
        // if (g_ispCallback)
            // g_ispCallback(output, ISP_HAL_INT_FIFO_OVERWRITE);
    }
    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_HSYNC_CNT_ERROR)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_HSYNC_CNT_ERROR);
        printf("OUT %d HSYNC_CNT_ERROR\r\n", output);
        // if (g_ispCallback)
            // g_ispCallback(output, ISP_HAL_INT_HSYNC_CNT_ERROR);
    }
    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_VSYNC_CNT_ERROR)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_VSYNC_CNT_ERROR);
        printf("OUT %d VSYNC_CNT_ERROR\r\n", output);
        // if (g_ispCallback)
            // g_ispCallback(output, ISP_HAL_INT_VSYNC_CNT_ERROR);
    }
}

static void isp_channel0_ISR(void)
{
    isp_isr(0); // for output channel 0 (callback from dvp2axi_a)
}

static void isp_scaler_path_init(void)
{
    // scaler input from ISP output
    MM_MISC_Set_YUV422_In_Mux(MM_MISC_YUV422_IN_MUX_ID_SCALER, MM_MISC_YUV422_IN_MUX_SEL_DVP_TSRC);
}

static void isp_yuv422To420_init(void)
{
    // yuv422to420 input from osd blend
    MM_MISC_Set_YUV422_In_Mux(MM_MISC_YUV422_IN_MUX_ID_YUV422TO420, MM_MISC_YUV422_IN_MUX_SEL_SCALER_F0);
}

static void isp_tsrc_cfg(uint16_t width, uint16_t height, uint32_t addr)
{
    TSRC_Cfg_Type tsrc_cfg = {0};
    TSRC_TG_Cfg tsrc_tg_cfg = {0};

    tsrc_cfg.format = TSRC_PIXEL_YUV422_16BIT;
    tsrc_cfg.burst = TSRC_BURST_TYPE_INCR64;
    tsrc_cfg.pushMode = 1;
    tsrc_cfg.byte0 = 0;
    tsrc_cfg.byte1 = 1;
    tsrc_cfg.byte2 = 2;
    tsrc_cfg.imgSize.width = width;
    tsrc_cfg.imgSize.height = height;
    tsrc_cfg.imgSize.hBlank = TSRC_H_BLANK;
    tsrc_cfg.imgSize.vBlank = TSRC_V_BLANK;
    tsrc_cfg.memStart = addr;
    tsrc_cfg.prefetch = TSRC_V_BLANK - 4;

    tsrc_tg_cfg.totalHPixels = width + TSRC_H_BLANK;
    tsrc_tg_cfg.actStartPixel = 0;
    tsrc_tg_cfg.actPixelWidth = width;
    tsrc_tg_cfg.actStartLine = 0;
    tsrc_tg_cfg.actLineHeight = height;

    TSRC_Init(&tsrc_cfg);
    TSRC_Set_TG_Cfg(&tsrc_tg_cfg);
    TSRC_Set_TG_State(1);
    TSRC_Enable();
}

static void isp_scaler_size_cfg(SCALER_CONFIG_S size_cfg)
{
    SCALER_HYDRE_SetSizeCfg(SCALER_HYDRE_TYPE_A, size_cfg);
}

static void isp_scaler_coef_cfg(SCALER_CONFIG_S size_cfg)
{
    SCALER_COEF_S coef = {0};

    // assume to call firmware to get the coef, current use hardcode to test
    if ( size_cfg.output.width>= size_cfg.input.width)
    {
        coef.x_coeff_set = 0;
        coef.x_coeff_ratio = 0;
    }
    else
    {
        coef.x_coeff_set = 2;
        coef.x_coeff_ratio = 8;
    }

    if ( size_cfg.output.height>= size_cfg.input.height)
    {
        coef.y_coeff_set = 0;
        coef.y_coeff_ratio = 0;
    }
    else
    {
        coef.y_coeff_set = 0;
        coef.y_coeff_ratio = 8;
    }

    SCALER_HYDRE_SetXYCoef(SCALER_HYDRE_TYPE_A, coef);
}

static void isp_scaler_cfg(void)
{
    SCALER_CONFIG_S scaler_size_cfg = {0};

    scaler_size_cfg.input.in_width = TSRC_WIDTH;
    scaler_size_cfg.input.in_height = TSRC_HEIGHT;
    scaler_size_cfg.input.start_x = 0; // assume default full image
    scaler_size_cfg.input.start_y = 0; // assume default full image
    scaler_size_cfg.input.width = TSRC_WIDTH;
    scaler_size_cfg.input.height = TSRC_HEIGHT;
    scaler_size_cfg.output.width = TSRC_OUT_WIDTH;
    scaler_size_cfg.output.height = TSRC_OUT_HEIGHT;
    isp_scaler_size_cfg(scaler_size_cfg);
    isp_scaler_coef_cfg(scaler_size_cfg);
}

static void isp_dvp2axi_callback(void)
{
    Interrupt_Handler_Register(D2XA_IRQn, isp_channel0_ISR);
    CPU_Interrupt_Enable(D2XA_IRQn);
    // CPU_Interrupt_Enable(D2XB_IRQn);
}

static void isp_dvp2axi_cfg(void)
{
    DVP2AXI_INIT_CFG_S dvp2axi_cfg = {0};

    // common config
    dvp2axi_cfg.framePol = DVP2AXI_DATA_ACTIVE_HIGH;
    dvp2axi_cfg.linePol = DVP2AXI_DATA_ACTIVE_HIGH;
    dvp2axi_cfg.swIntCnt = 1;
    dvp2axi_cfg.sensorMode = DVP2AXI_DATA_VALID_V_AND_H;

    // yuyv format and use dvp2axi_A only
    if ((ISP_TSRC_OUT_FMT0_YUYV == TSRC_OUT_FORMAT) || (ISP_TSRC_OUT_FMT0_Y_ONLY == TSRC_OUT_FORMAT))
    {
        if (ISP_TSRC_OUT_FMT0_YUYV == TSRC_OUT_FORMAT)
        {
            MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_A, MM_MISC_YUV422_IN_MUX_SEL_SCALER_F0);
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_16_BIT;
        }
        else // Y only
        {
            MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_A, MM_MISC_DVP2AXI_SRC_YUV422TO420);
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_8_BIT;
        }

        dvp2axi_cfg.subSampleEn = 0;
        dvp2axi_cfg.wrapEn = 1;
        dvp2axi_cfg.wrapLine = TSRC_OUT_RING_LINE;
        dvp2axi_cfg.byte_sel = DVP2AXI_SEL_LOW_BYTE; // YUYV don't care, Y only select low byte
        dvp2axi_cfg.memStart = TSRC_OUT_Y_ADDR;
        dvp2axi_cfg.imgWidth = TSRC_OUT_WIDTH;
        dvp2axi_cfg.imgHeight = TSRC_OUT_HEIGHT;
        DVP2AXI_Init(DVP2AXI_PORT_A, &dvp2axi_cfg);
        DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_A, TSRC_OUT_WIDTH, TSRC_OUT_HEIGHT);

        DVP2AXI_IntMask(DVP2AXI_PORT_A, DVP2AXI_INT_ALL, UNMASK);

        DVP2AXI_Enable(DVP2AXI_PORT_A);
    }
    else // nv12 format and use dvp2axi_A for Y and dvp2axi_B for UV
    {
        if (ISP_TSRC_OUT_FMT0_YUV422SP == TSRC_OUT_FORMAT)
        {
            MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_A, MM_MISC_YUV422_IN_MUX_SEL_SCALER_F0);
            MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_B, MM_MISC_YUV422_IN_MUX_SEL_SCALER_F0);
        }
        else // NV12
        {
            MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_A, MM_MISC_DVP2AXI_SRC_YUV422TO420);
            MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_B, MM_MISC_DVP2AXI_SRC_YUV422TO420);
        }

        // Config dvp2axi a
        dvp2axi_cfg.bitWidth = DVP2AXI_DATA_8_BIT;
        dvp2axi_cfg.subSampleEn = 0;
        dvp2axi_cfg.wrapEn = 1;
        dvp2axi_cfg.wrapLine = TSRC_OUT_RING_LINE;
        dvp2axi_cfg.byte_sel = DVP2AXI_SEL_LOW_BYTE;
        dvp2axi_cfg.memStart = TSRC_OUT_Y_ADDR;
        dvp2axi_cfg.imgWidth = TSRC_OUT_WIDTH;
        dvp2axi_cfg.imgHeight = TSRC_OUT_HEIGHT;
        DVP2AXI_Init(DVP2AXI_PORT_A, &dvp2axi_cfg);
        DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_A, TSRC_OUT_WIDTH, TSRC_OUT_HEIGHT);

        DVP2AXI_IntMask(DVP2AXI_PORT_A, DVP2AXI_INT_ALL, UNMASK);

        // Config dvp2axi b
        dvp2axi_cfg.bitWidth = DVP2AXI_DATA_8_BIT;
        dvp2axi_cfg.wrapEn = 1;
        dvp2axi_cfg.byte_sel = DVP2AXI_SEL_HIGH_BYTE;
        dvp2axi_cfg.memStart = TSRC_OUT_C_ADDR;

        dvp2axi_cfg.imgWidth = TSRC_OUT_WIDTH;

        if (ISP_TSRC_OUT_FMT0_YUV422SP == TSRC_OUT_FORMAT)
        {
            dvp2axi_cfg.subSampleEn = 0;
            dvp2axi_cfg.imgHeight = TSRC_OUT_HEIGHT;
            dvp2axi_cfg.wrapLine = TSRC_OUT_RING_LINE;
        }
        else // NV12
        {
            dvp2axi_cfg.subSampleEn = 1;
            dvp2axi_cfg.imgHeight = (TSRC_OUT_HEIGHT >> 1);
            dvp2axi_cfg.wrapLine = (TSRC_OUT_RING_LINE >> 1);
        }

        DVP2AXI_Init(DVP2AXI_PORT_B, &dvp2axi_cfg);
        DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_B, TSRC_OUT_WIDTH, TSRC_OUT_HEIGHT);

        DVP2AXI_IntMask(DVP2AXI_PORT_B, DVP2AXI_INT_ALL, UNMASK);

        DVP2AXI_Enable(DVP2AXI_PORT_A);
        DVP2AXI_Enable(DVP2AXI_PORT_B);
    }
}

int tsrc_init(void)
{
    MM_MISC_Set_DVP_AS_SRAM_Mode(MM_MISC_SHARED_SRAM_MODE_DVP_TSRC);

    isp_scaler_path_init();
    isp_yuv422To420_init();
    isp_scaler_cfg();

    // set dvp2axi A & B for channel 0
    isp_dvp2axi_cfg();

    // enable interrupt
    isp_dvp2axi_callback();

    isp_tsrc_cfg(TSRC_WIDTH, TSRC_HEIGHT, TSRC_IN_ADDR);

    MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_Y, MM_MISC_DVP2AXI_PORT_A);
    MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_C, MM_MISC_DVP2AXI_PORT_B);

    printf("Tsrc Init Info:\r\n");
    printf("    TSRC_WIDTH            = %d\r\n",     TSRC_WIDTH         );
    printf("    TSRC_HEIGHT           = %d\r\n",     TSRC_HEIGHT        );
    printf("    TSRC_IN_ADDR          = 0x%08x\r\n", TSRC_IN_ADDR       );
    printf("    SCL_OUT_WIDTH         = %d\r\n",     TSRC_OUT_WIDTH     );
    printf("    SCL_OUT_HEIGHT        = %d\r\n",     TSRC_OUT_HEIGHT    );
    printf("    DVP2AXI_OUT_Y_ADDR    = 0x%08x\r\n", TSRC_OUT_Y_ADDR    );
    printf("    DVP2AXI_OUT_C_ADDR    = 0x%08x\r\n", TSRC_OUT_C_ADDR    );
    printf("    DVP2AXI_OUT_RING_LINE = %d\r\n",     TSRC_OUT_RING_LINE );
    printf("    DVP2AXI_OUT_FORMAT    = %d\r\n",     TSRC_OUT_FORMAT    );
    printf("    TSRC_H_BLANK          = %d\r\n",     TSRC_H_BLANK       );
    


    return 0;
}

int tsrc_trigger(void)
{
    TSRC_Push_Mode_Trigger();
    return 0;
}
