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

#include <drv_mm_misc.h>

#include <sensor.h>
#include "hal_isp.h"
#include "drv_glb.h"
#include "drv_dvp2axi.h"
#include "drv_mjpeg.h"

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

#define CROP_START_X  (0)
#define CROP_START_Y  (0)
#define CROP_WIDTH    (240)
#define CROP_HEIGHT   (320)

#define OUT_WIDTH_0   (240)
#define OUT_HEIGHT_0  (320)
#define OUT_RING_LINE (16)
#define OUT_FORMAT_0  (ISP_HAL_OUT_FMT0_YUYV)

#define OUT_WIDTH_1   (240)
#define OUT_HEIGHT_1  (320)
#define OUT_FORMAT_1  (ISP_HAL_OUT_FMT1_ARGB)

#define SNS_CAMCLK_PIN (GLB_GPIO_PIN_0)//(GLB_GPIO_PIN_41)

#define USE_SPI_0

#define SEOF_TEST_LINE_CNT     (200)

// #define USE_OCRAM // dvp2axi handshake buffer with jpg

static uint8_t ATTR_PSRAM_SECTION jpg_head_buf[800] = { 0 };
static uint32_t jpg_head_len;

static const rt_camera_desc *cam_desc;
static SemaphoreHandle_t gJencNotify;

// For isp output channel 0
static uint8_t *gPicStreamBuf = 0;
static uint32_t gPicStreamBufSize = 512*1024;
static uint32_t gPicStreamBufPtr = 0;
static uint8_t gPicStreamFull = 0;

#if defined(CONFIG_LINK_FLASH) && defined(USE_OCRAM)
#if (OUT_WIDTH_0 > 0)
// use global array to be ocram handshake buffer, assume max buf OUT_WIDTH_0 * 16 * 2 for yuyv
static unsigned char isp_jpg_handshake_buf[OUT_WIDTH_0*16*2] __attribute__((aligned(8)));
#else
static unsigned char isp_jpg_handshake_buf[16*2] __attribute__((aligned(8)));
#endif
#endif

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
        vTaskDelay(12);
    }
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

static void get_mjpg_info(uint8_t isp_format0, uint8_t *mjpg_format, uint8_t *mjpg_mode)
{
    switch(isp_format0)
    {
        case ISP_HAL_OUT_FMT0_YUYV:
            *mjpg_format = MJPEG_HAL_FORMAT_YUV422_YUYV;
            *mjpg_mode = YUV_MODE_422;
        break;
        case ISP_HAL_OUT_FMT0_YUV422SP:
            *mjpg_format = MJPEG_HAL_FORMAT_YUV422SP_NV16;
            *mjpg_mode = YUV_MODE_422;
        break;
        case ISP_HAL_OUT_FMT0_NV12:
            *mjpg_format = MJPEG_HAL_FORMAT_YUV420SP_NV12;
            *mjpg_mode = YUV_MODE_420;
        break;
        case ISP_HAL_OUT_FMT0_Y_ONLY:
            *mjpg_format = MJPEG_HAL_FORMAT_GRAY;
            *mjpg_mode = YUV_MODE_400;
        break;
        default:
            *mjpg_format = MJPEG_HAL_FORMAT_YUV422_YUYV;
            *mjpg_mode = YUV_MODE_422;
            printf("Unknown isp format 0 %d\r\n", isp_format0);
        break;
    }
}

static void show_stream_buf(uint32_t addr, uint32_t size)
{
    if (!gPicStreamFull) {
        if (gPicStreamBufPtr + size > gPicStreamBufSize) {
            printf("\r\n\r\nStream Full, dump binary memory test.mjpeg 0x%lx 0x%lx\r\n\r\n", (uint32_t)gPicStreamBuf, (uint32_t)gPicStreamBuf + gPicStreamBufPtr);
            gPicStreamFull = 1;
        } else {
            memcpy(gPicStreamBuf + gPicStreamBufPtr, (void *)addr, size);
            gPicStreamBufPtr += size;
        }
        mjpeg_drop_one_frame();
    }
}

static void cam_pipe_init(void)
{
    isp_hal_cfg_t isp_cfg = {0};
    MM_MISC_SPI2DVP_Status info;
    uint32_t out_buffer_height_0 = 0;

    uint32_t out_frame_size_0, out_frame_addr_0 = 0;
    uint32_t out_frame_size_1, out_frame_addr_1 = 0;

    struct mjpeg_hal_cfg_t mjpeg_cfg;
    uint32_t jenc_size, jenc_addr;
    uint8_t mjpg_format, mjpg_yuv_mode;

    gPicStreamBuf = pvPortMalloc(gPicStreamBufSize);

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

    out_buffer_height_0 = (OUT_RING_LINE == 0)?OUT_HEIGHT_0:OUT_RING_LINE;

    // Output 0 size and address
    if ((ISP_HAL_OUT_FMT0_YUYV == OUT_FORMAT_0) || (ISP_HAL_OUT_FMT0_YUV422SP == OUT_FORMAT_0))
        out_frame_size_0 = YUV422_FRAME_SIZE(OUT_WIDTH_0, out_buffer_height_0);
    else if (ISP_HAL_OUT_FMT0_NV12 == OUT_FORMAT_0)
        out_frame_size_0 = YUV420_Y_FRAME_SIZE(OUT_WIDTH_0, out_buffer_height_0) + YUV420_UV_FRAME_SIZE(OUT_WIDTH_0, out_buffer_height_0);
    else
        out_frame_size_0 = YUV400_FRAME_SIZE(OUT_WIDTH_0, out_buffer_height_0);

    if (out_frame_size_0)
    {
#ifdef USE_OCRAM
        #ifdef CONFIG_LINK_FLASH // flash use global array to keep the ring buffer
        out_frame_addr_0 = (uint32_t)&isp_jpg_handshake_buf[0];
        #else
        extern int _ld_ram_addr1;
        out_frame_addr_0 = (uint32_t)&_ld_ram_addr1;
        #endif
#else
        out_frame_addr_0 = (uint32_t) pvPortMalloc( out_frame_size_0 );
#endif
    }

    printf("out_frame_size_0 = %lu\r\n", out_frame_size_0);
    printf("out_frame_addr_0 = 0x%08lx\r\n", out_frame_addr_0);

    // Output 1 size and address
    if (ISP_HAL_OUT_FMT1_ARGB == OUT_FORMAT_1)
        out_frame_size_1 = ARGB_FRAME_SIZE(OUT_WIDTH_1, OUT_HEIGHT_1);
    else
        out_frame_size_1 = RGB_FRAME_SIZE(OUT_WIDTH_1, OUT_HEIGHT_1);

    if (out_frame_size_1)
        out_frame_addr_1 = (uint32_t) pvPortMalloc( out_frame_size_1 * 2); // alloc two ping pong buffer for normal usage

    printf("out_frame_size_1 = %lu\r\n", out_frame_size_1);
    printf("out_frame_addr_1 = 0x%08lx\r\n", out_frame_addr_1);

    jenc_size = 512*1024;
    jenc_addr = (uint32_t) pvPortMalloc( jenc_size );
    if (!jenc_addr) {
        printf("%s[%d] error: memory malloc failed! (size: %lu)\r\n", __func__, __LINE__, jenc_size);
        return;
    }

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

    isp_cfg.yuv_adj_en = 0;

    isp_cfg.out_ch_0.enable = (out_frame_size_0)?1:0;
    isp_cfg.out_ch_0.scaler_cfg.in.start_x = 0;
    isp_cfg.out_ch_0.scaler_cfg.in.start_y = 0;
    isp_cfg.out_ch_0.scaler_cfg.in.width = CROP_WIDTH;
    isp_cfg.out_ch_0.scaler_cfg.in.height = CROP_HEIGHT;
    isp_cfg.out_ch_0.scaler_cfg.out.width = OUT_WIDTH_0;
    isp_cfg.out_ch_0.scaler_cfg.out.height = OUT_HEIGHT_0;
    isp_cfg.out_ch_0.format = OUT_FORMAT_0;
    isp_cfg.out_ch_0.ring_line = OUT_RING_LINE;

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
    isp_cfg.out_ch_1.chn_common.addr[1] = out_frame_addr_1 + out_frame_size_1;
    isp_cfg.out_ch_1.chn_common.pp_num = 2;
    isp_cfg.out_ch_1.chn_common.out_crop.start_x = 0;
    isp_cfg.out_ch_1.chn_common.out_crop.start_y = 0;
    isp_cfg.out_ch_1.chn_common.out_crop.width = OUT_WIDTH_1;
    isp_cfg.out_ch_1.chn_common.out_crop.height = OUT_HEIGHT_1;

    isp_cfg.out_callback = isp_event_cb;

    isp_init(&isp_cfg);

    // set seof interrupt
    set_seof0_cfg();

    // hook callback function for seof interrupt and enable
    Interrupt_Handler_Register(ISP_MM_SEOF0_IRQn, mm_seof0_isr);
    CPU_Interrupt_Enable(ISP_MM_SEOF0_IRQn);

    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)out_frame_addr_0, (unsigned long)out_frame_size_0);
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)out_frame_addr_1, (unsigned long)out_frame_size_1);

    MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_Y, MM_MISC_DVP2AXI_PORT_A);
    MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_C, MM_MISC_DVP2AXI_PORT_B);

    get_mjpg_info(isp_cfg.out_ch_0.format, &mjpg_format, &mjpg_yuv_mode);

    mjpeg_cfg.format = mjpg_format; //MJPEG_HAL_FORMAT_YUV422SP_NV16; //MJPEG_HAL_FORMAT_YUV422_YUYV;
    mjpeg_cfg.quality = MJPEG_DEFAULT_QUALITY;
    mjpeg_cfg.rows = OUT_RING_LINE;
    mjpeg_cfg.resolution_x = OUT_WIDTH_0;
    mjpeg_cfg.resolution_y = OUT_HEIGHT_0;
    mjpeg_cfg.input_bufaddr0 = out_frame_addr_0;
    mjpeg_cfg.input_bufaddr1 = out_frame_addr_0 + YUV400_FRAME_SIZE(OUT_WIDTH_0, out_buffer_height_0);
    mjpeg_cfg.output_bufaddr = jenc_addr;
    mjpeg_cfg.output_bufsize = jenc_size;
    mjpeg_cfg.input_yy_table = NULL;
    mjpeg_cfg.input_uv_table = NULL;
    mjpeg_cfg.output_swap_mode_en = 0;    /*enable encode output buffer swap mode, 0 frame mode, 1 swap mode.*/
    mjpeg_cfg.hw_mode_swen_en = 1;
    mjpeg_cfg.hw_mode_sshot_en = 0;
    mjpeg_cfg.callback = mjpeg_enc_event_cb;

    mjpeg_init(&mjpeg_cfg);

    jpg_head_len = mjpeg_headerCreate(mjpg_yuv_mode, MJPEG_DEFAULT_QUALITY, OUT_WIDTH_0, OUT_HEIGHT_0, jpg_head_buf);
    mjpeg_fill_header_tail(jpg_head_buf, jpg_head_len);
    L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)jenc_addr, (unsigned long)jenc_size);
    mjpeg_start(0);

    /* start isp and ready to get image */
    isp_start();

    // camera_module_start(cam_desc);
    MM_MISC_SPI2DVP_WaitReady();
    MM_MISC_Get_SPI2DVP_Status(&info);
    printf("img_width:%d, height:%d\r\n", info.spi2dvp_img_w, info.spi2dvp_img_h);
    printf("img_data_size:%d\r\n", info.spi2dvp_data_size);
}

static void cam_get_output0_proc(void *arg) // task
{
    int ret = 0;
    uint32_t jenc_size, jenc_addr;

    while (1) {
        xSemaphoreTake(gJencNotify, portMAX_DELAY);
        ret = mjpeg_get_one_frame(&jenc_addr, &jenc_size);
        if (ret == 0) {
            L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)jenc_addr, (unsigned long)jenc_size);
            printf("Frame ready 0x%lx 0x%lx\r\n", (uint32_t)jenc_addr, (uint32_t)jenc_addr + jenc_size);
            show_stream_buf(jenc_addr, jenc_size);
        }
    }

    vSemaphoreDelete(gJencNotify);
    gJencNotify = NULL;
    vTaskDelete(NULL);
}

// ============================================================================
// public function
// ============================================================================

void cam_task_entry()
{
    gJencNotify = xSemaphoreCreateCounting(1, 0);
    xTaskCreate(cam_get_output0_proc, "getoutput0", 512, NULL, 10, NULL);

    cam_pipe_init(); // config isp + jpg + sensor stream
}

void cam_get_next(void)
{
    gPicStreamFull = 0;
    gPicStreamBufPtr = 0;
    memset(gPicStreamBuf, 0, gPicStreamBufSize);

    mjpeg_drop_one_frame();
}
