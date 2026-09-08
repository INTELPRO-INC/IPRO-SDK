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
#include "drv_clock.h"
#include "drv_dvp2axi.h"
#include "drv_mjpeg.h"
#include "../soc/peripheral/lcd/lcd.h"

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
#define CROP_WIDTH    (1280)
#define CROP_HEIGHT   (720)

#define OUT_WIDTH_0   (1920)
#define OUT_HEIGHT_0  (1080)
#define OUT_RING_LINE (16)
#define OUT_FORMAT_0  (ISP_HAL_OUT_FMT0_YUYV)

#define OUT_WIDTH_1   (480) //(640)
#define OUT_HEIGHT_1  (270) //(360)
#define OUT_FORMAT_1  (ISP_HAL_OUT_FMT1_ARGB)

#define SNS_PWR_PIN    (GLB_GPIO_PIN_34)
#define SNS_RST_PIN    (GLB_GPIO_PIN_35)
#define SNS_CAMCLK_PIN (GLB_GPIO_PIN_41)

#define SEOF_TEST_LINE_CNT     (200)

#define USE_OCRAM // dvp2axi handshake buffer with jpg

#ifdef CONFIG_CHERRYUSB_DEVICE_VIDEO
#define MJPG_SEND_BY_UVC
#endif
#define SHOW_LCD  // send to lcd

#ifdef MJPG_SEND_BY_UVC
#undef OUT_WIDTH_0
#undef OUT_HEIGHT_0
#define OUT_WIDTH_0   (640) // output size same as uvc_stream_cfg.c
#define OUT_HEIGHT_0  (480) // output size same as uvc_stream_cfg.c
#endif

static uint8_t ATTR_PSRAM_SECTION jpg_head_buf[800] = { 0 };
static uint32_t jpg_head_len;

static const rt_camera_desc *cam_desc;
static SemaphoreHandle_t gJencNotify;

// For isp output channel 0
#ifdef MJPG_SEND_BY_UVC
extern void stream_uvc_init(void);
extern void stream_send_jpg(uint32_t addr, uint32_t size);
#else
static uint8_t *gPicStreamBuf = 0;
static uint32_t gPicStreamBufSize = 512*1024;
static uint32_t gPicStreamBufPtr = 0;
static uint8_t gPicStreamFull = 0;
#endif

// For isp output channel 1
#ifdef SHOW_LCD
static uint32_t show_lcd_addr, show_lcd_size;
static SemaphoreHandle_t g_tDbiShowPicNotify;
static uint8_t lcd_busy_flag = 0;
#endif

// ============================================================================
// local function
// ============================================================================
static void isp_dvp_pin_mux(void)
{
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[] = { GLB_GPIO_PIN_0, GLB_GPIO_PIN_1, GLB_GPIO_PIN_2, GLB_GPIO_PIN_3,
                           GLB_GPIO_PIN_4, GLB_GPIO_PIN_5, GLB_GPIO_PIN_6, GLB_GPIO_PIN_7,
                           GLB_GPIO_PIN_38, GLB_GPIO_PIN_39, GLB_GPIO_PIN_40};
    int i;

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

static void isp_enable_camera_module(int enable)
{
#if defined(CONFIG_FPGA)
    writel(0, 0x30000074); // pull down reset and power down pin

    if (enable) {
        vTaskDelay(1);

        writel(3, 0x30000074); // pull up reset and power down pin

        vTaskDelay(12); // see sensor datasheet
    }
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

    if (enable) {
        GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_PLL48M, 1);
        vTaskDelay(1);

        // power high
        GLB_GPIO_Write(SNS_PWR_PIN, 1);
        // reset high
        GLB_GPIO_Write(SNS_RST_PIN, 1);
        vTaskDelay(12);
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
#ifdef SHOW_LCD
        if (1 == info.out_port)
        {
            show_lcd_addr = info.out_addr;
            show_lcd_size = info.out_size;
            xSemaphoreGiveFromISR(g_tDbiShowPicNotify, NULL);
        }
#endif
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

/*
 * Optional frame observer. Set via strmctrl_set_jpeg_observer(). Fires on
 * every completed JPEG frame BEFORE the encoder buffer is released, so the
 * observer can memcpy/forward the bytes. Must not block.
 */
typedef void (*strmctrl_jpeg_observer_t)(const void *buf, uint32_t size, void *ctx);
static strmctrl_jpeg_observer_t s_jpeg_observer;
static void                    *s_jpeg_observer_ctx;

void strmctrl_set_jpeg_observer(strmctrl_jpeg_observer_t cb, void *ctx)
{
    s_jpeg_observer     = cb;
    s_jpeg_observer_ctx = ctx;
}

static void show_stream_buf(uint32_t addr, uint32_t size)
{
    /* Notify any registered observer while the encoder buffer is still live. */
    if (s_jpeg_observer) {
        L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)addr, (unsigned long)size);
        s_jpeg_observer((const void *)addr, size, s_jpeg_observer_ctx);
    }

#ifdef MJPG_SEND_BY_UVC // send to UVC
    stream_send_jpg(addr, size);
    mjpeg_drop_one_frame();
#else // local buffer, use jtag to download jpg
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
#endif
}

#ifdef SHOW_LCD
static void flush_async_callback(void)
{
    lcd_busy_flag = 0;
}
#endif

static void stream_pipe_init(void)
{
    isp_hal_cfg_t isp_cfg = {0};
    uint32_t out_buffer_height_0 = 0;

    uint32_t out_frame_size_0, out_frame_addr_0 = 0;
    uint32_t out_frame_size_1, out_frame_addr_1 = 0;

    struct mjpeg_hal_cfg_t mjpeg_cfg;
    uint32_t jenc_size, jenc_addr;
    uint8_t mjpg_format, mjpg_yuv_mode;

#ifndef MJPG_SEND_BY_UVC
    gPicStreamBuf = pvPortMalloc(gPicStreamBufSize);
#endif

    printf("SCCB_Init\r\n");
    SCCB_Init();

    printf("isp_enable_camera_module\r\n");
    isp_enable_camera_module(1);

    isp_dvp_pin_mux();

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
        out_frame_addr_0 = (uint32_t)0x1101C000;
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

    isp_cfg.isp_src = ISP_HAL_SRC_DVP_AS;
    isp_cfg.in_size.width = cam_desc->width;
    isp_cfg.in_size.height = cam_desc->height;

    isp_cfg.src_cfg.dvp_input.hsync = ISP_HAL_DVP_ACTIVE_HIGH;
    isp_cfg.src_cfg.dvp_input.vsync = ISP_HAL_DVP_ACTIVE_HIGH;
    isp_cfg.src_cfg.dvp_input.order = ISP_HAL_DATA_BYTE_LOWER;
    isp_cfg.src_cfg.dvp_input.pclk = cam_desc->pclk;
    isp_cfg.src_cfg.dvp_input.totalHPixels = cam_desc->total_width;

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

    isp_cfg.out_ch_0.chn_common.fr_ctrl.src_fps = 30;
    isp_cfg.out_ch_0.chn_common.fr_ctrl.dst_fps = 30;
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

    isp_cfg.out_ch_1.chn_common.fr_ctrl.src_fps = 30;
    isp_cfg.out_ch_1.chn_common.fr_ctrl.dst_fps = 30;
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
}

static void strmctrl_get_output0_proc(void *arg) // task
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

#ifdef SHOW_LCD
static void strmctrl_get_output1_proc(void *arg)
{
    while (1) {
        xSemaphoreTake(g_tDbiShowPicNotify, portMAX_DELAY);
        if (0 == lcd_busy_flag)
        {
            lcd_draw_picture_nonblocking(0, 25, 479, 25+269, (lcd_color_t *)(show_lcd_addr));
            lcd_busy_flag = 1;
        }
    }
    vSemaphoreDelete(g_tDbiShowPicNotify);
    g_tDbiShowPicNotify = NULL;
    vTaskDelete(NULL);
}
#endif

// ============================================================================
// public function
// ============================================================================

void strmctrl_init()
{
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

    gJencNotify = xSemaphoreCreateCounting(1, 0);
    xTaskCreate(strmctrl_get_output0_proc, "getoutput0", 512, NULL, 10, NULL);

#ifdef MJPG_SEND_BY_UVC
    stream_uvc_init();
#endif

#ifdef SHOW_LCD
    g_tDbiShowPicNotify = xSemaphoreCreateCounting(1, 0);
    xTaskCreate(strmctrl_get_output1_proc, "getoutput1", 512, NULL, 10, NULL);
    lcd_init();
    lcd_async_callback_register(flush_async_callback);
#endif

    stream_pipe_init(); // config isp + jpg + sensor stream
}

void strmctrl_get_next(void)
{
#ifndef MJPG_SEND_BY_UVC
    gPicStreamFull = 0;
    gPicStreamBufPtr = 0;
    memset(gPicStreamBuf, 0, gPicStreamBufSize);

    mjpeg_drop_one_frame();
#endif
}

void strmctrl_set_zoom(int demo_id)
{
    isp_hal_crop_t zoom_cfg = {0};
    // 0 for recover both channel
    // 1 for zoom channel 0 (half size, and center zoom)
    // 2 for zoom channel 1 (half size, and center zoom)
    if (0 == demo_id)
    {
        // recover channel 0 and 1
        zoom_cfg.start_x = 0;
        zoom_cfg.start_y = 0;
        zoom_cfg.width = CROP_WIDTH;
        zoom_cfg.height = CROP_HEIGHT;
        isp_zoom_cfg(ISP_CHANNEL_0, zoom_cfg);
        isp_zoom_cfg(ISP_CHANNEL_1, zoom_cfg);
    }
    else if (1 == demo_id)
    {
        // zoom for channel 0
        zoom_cfg.start_x = CROP_WIDTH >> 2;
        zoom_cfg.start_y = CROP_HEIGHT >> 2;
        zoom_cfg.width = CROP_WIDTH >> 1;
        zoom_cfg.height = CROP_HEIGHT >> 1;
        isp_zoom_cfg(ISP_CHANNEL_0, zoom_cfg);
    }
    else if (2 == demo_id)
    {
        // zoom for channel 1
        zoom_cfg.start_x = CROP_WIDTH >> 2;
        zoom_cfg.start_y = CROP_HEIGHT >> 2;
        zoom_cfg.width = CROP_WIDTH >> 1;
        zoom_cfg.height = CROP_HEIGHT >> 1;
        isp_zoom_cfg(ISP_CHANNEL_1, zoom_cfg);
    }
}

void strmctrl_set_frc(int channel, int demo_id)
{
    isp_hal_frame_rate_t frc_cfg = {0};
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

    isp_fps_cfg(channel, frc_cfg);
}

// ============================================================================
// Shell function
// ============================================================================
#include "shell.h"

static int cmd_sensor_read(int argc, char **argv)
{
    char *pEnd;
    int addr = strtol(argv[1], &pEnd, 16);

    int value = 0;
    cam_desc->get_register(addr, &value);

    printf("read sensor register: 0x%02x = 0x%02x\r\n", addr, value);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sensor_read, sensor_read, read sensor register);

static int cmd_sensor_write(int argc, char **argv)
{
    char *pEnd;
    int addr = strtol(argv[1], &pEnd, 16);
    int data = strtol(argv[2], &pEnd, 16);

    cam_desc->set_register(addr, data);

    printf("write sensor register: 0x%02x = 0x%02x\r\n", addr, data);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sensor_write, sensor_write, write sensor register);

