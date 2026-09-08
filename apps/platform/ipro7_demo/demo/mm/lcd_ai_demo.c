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
#include "../soc/peripheral/lcd/lcd.h"

#include "img_proc.h"
#include "./isp/isp_dvt_api.h"
#include "isp_cmd_parse.h"

#ifdef CONFIG_AI_SUPPORT
#include "detect/yolov8_person_pet_detect.h"
#endif

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

#define OUT_WIDTH_1   (640)
#define OUT_HEIGHT_1  (360)
#define OUT_FORMAT_1  (ISP_HAL_OUT_FMT1_ARGB)

#define OUT_WIDTH_EXT  (480)
#define OUT_HEIGHT_EXT (270)
#define OUT_FORMAT_EXT (ISP_HAL_OUT_FMT_EXT_ARGB)

#define SNS_PWR_PIN    (GLB_GPIO_PIN_34)
#define SNS_RST_PIN    (GLB_GPIO_PIN_35)
#define SNS_CAMCLK_PIN (GLB_GPIO_PIN_41)

#define ISP_INIT_DROP_FRAMES   (6)
#define SEOF_TEST_LINE_CNT     (200)

static const rt_camera_desc *cam_desc;

// For isp output channel 1
static uint32_t show_lcd_addr, show_lcd_size;
volatile static SemaphoreHandle_t g_tDbiShowPicNotify;

static uint32_t ai_addr, ai_size;
static uint8_t converged_frm_cnt = ISP_INIT_DROP_FRAMES;
static uint8_t start_stream = 0;

#ifdef CONFIG_AI_SUPPORT
static yolov8_result_t ATTR_PSRAM_SECTION tmp_result;
static volatile uint8_t ai_result_using = 0;
#endif

static ipro_isp_stats_ready_cb stats_ready_cb;

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

static void update_isp_fps(uint8_t port, uint8_t src, uint8_t dst) {
    isp_hal_frame_rate_t frc = { .src_fps = src, .dst_fps = dst };
    isp_fps_cfg(port, frc);
}

static void isp_event_cb(isp_out_frame_info_t info)
{
    if (info.event == ISP_HAL_INT_NORMAL)
    {
        LOG_PRINTF("OUT %ld NORMAL\r\n", info.out_port);
        if (ISP_CHANNEL_1 == info.out_port)
        {
            ai_addr = info.out_addr;
            ai_size = info.out_size;
            if (0 == converged_frm_cnt)
            {
                // ready to ai and stop streaming (channel drop)
                update_isp_fps(info.out_port, 30, 0);
#ifdef CONFIG_AI_SUPPORT
                extern int ai_ext_push_buffer(uint32_t addr);
                ai_ext_push_buffer(ai_addr);
#endif
            }
        }
        if (ISP_CHANNEL_EXT == info.out_port)
        {
            show_lcd_addr = info.out_addr;
            show_lcd_size = info.out_size;

            // ready to lcd draw and stop streaming (channel drop)
            update_isp_fps(info.out_port, 30, 0); // set drop
            xSemaphoreGiveFromISR(g_tDbiShowPicNotify, NULL);
        }
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
        if (stats_ready_cb) {
            stats_ready_cb(NULL);
        }
    }
    if (type_status.BF.EoF)
    {
        LOG_PRINTF("seof 0 EoF %llu\r\n", xTaskGetTickCountFromISR());
    }
    if (type_status.BF.lineCnt)
    {
        if (0 != converged_frm_cnt)
            converged_frm_cnt--;
        else
        {
            // image coverged and start stream
            if (0 == start_stream)
            {
                update_isp_fps(ISP_CHANNEL_1, 30, 30);
                update_isp_fps(ISP_CHANNEL_EXT, 30, 30);
                start_stream = 1;
            }
        }
        LOG_PRINTF("seof 0 LineCnt %llu\r\n", xTaskGetTickCountFromISR());
    }
    MM_MISC_Clr_SEOF_TypeStatus(MM_MISC_SEOF0, type_status);
}

static void flush_async_callback(void)
{
    // lcd draw done and start streaming
    update_isp_fps(ISP_CHANNEL_EXT, 30, 30);
}

int ipro_isp_stats_ready_config(ipro_isp_stats_ready_cb cb, void *usrdata)
{
    stats_ready_cb = cb;

    return 0;
}

static void stream_pipe_init(void)
{
    isp_hal_cfg_t isp_cfg = {0};

    uint32_t out_frame_size_1, out_frame_addr_1 = 0;
    uint32_t out_frame_size_2, out_frame_addr_2 = 0;

    isp_hal_vin_cfg_t vin_cfg = {0};

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

    // Output 1 size and address
    if (ISP_HAL_OUT_FMT1_ARGB == OUT_FORMAT_1)
        out_frame_size_1 = ARGB_FRAME_SIZE(OUT_WIDTH_1, OUT_HEIGHT_1);
    else
        out_frame_size_1 = RGB_FRAME_SIZE(OUT_WIDTH_1, OUT_HEIGHT_1);

    if (out_frame_size_1)
        out_frame_addr_1 = (uint32_t) pvPortMalloc( out_frame_size_1 * 2);

    printf("out_frame_size_1 = %lu\r\n", out_frame_size_1);
    printf("out_frame_addr_1 = 0x%08lx\r\n", out_frame_addr_1);

    // Output 1 size and address
    out_frame_size_2 = ARGB_FRAME_SIZE(OUT_WIDTH_EXT, OUT_HEIGHT_EXT);

    if (out_frame_size_2)
        out_frame_addr_2 = (uint32_t) pvPortMalloc( out_frame_size_2); // alloc two ping pong buffer for normal usage

    printf("out_frame_size_2 = %lu\r\n", out_frame_size_2);
    printf("out_frame_addr_2 = 0x%08lx\r\n", out_frame_addr_2);

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

    isp_cfg.out_ch_0.enable = 0;

    isp_cfg.out_ch_1.enable = (out_frame_size_1)?1:0;
    isp_cfg.out_ch_1.scaler_cfg.in.start_x = 0;
    isp_cfg.out_ch_1.scaler_cfg.in.start_y = 0;
    isp_cfg.out_ch_1.scaler_cfg.in.width = CROP_WIDTH;
    isp_cfg.out_ch_1.scaler_cfg.in.height = CROP_HEIGHT;
    isp_cfg.out_ch_1.scaler_cfg.out.width = OUT_WIDTH_1;
    isp_cfg.out_ch_1.scaler_cfg.out.height = OUT_HEIGHT_1;
    isp_cfg.out_ch_1.format = OUT_FORMAT_1;

    isp_cfg.out_ch_1.chn_common.fr_ctrl.src_fps = 30;
    isp_cfg.out_ch_1.chn_common.fr_ctrl.dst_fps = 0; // drop first, enable when converged
    isp_cfg.out_ch_1.chn_common.addr[0] = out_frame_addr_1;
    isp_cfg.out_ch_1.chn_common.addr[1] = out_frame_addr_1 + out_frame_size_1;
    isp_cfg.out_ch_1.chn_common.pp_num = 2;
    isp_cfg.out_ch_1.chn_common.out_crop.start_x = 0;
    isp_cfg.out_ch_1.chn_common.out_crop.start_y = 0;
    isp_cfg.out_ch_1.chn_common.out_crop.width = OUT_WIDTH_1;
    isp_cfg.out_ch_1.chn_common.out_crop.height = OUT_HEIGHT_1;

    isp_cfg.out_ch_ext.enable = (out_frame_size_2)?1:0;
    isp_cfg.out_ch_ext.ext_src = ISP_HAL_EXT_SRC_OUT_CHN1_IN;
    isp_cfg.out_ch_ext.format = OUT_FORMAT_EXT;

    isp_cfg.out_ch_ext.chn_common.fr_ctrl.src_fps = 30;
    isp_cfg.out_ch_ext.chn_common.fr_ctrl.dst_fps = 0; // drop first, enable when converged
    isp_cfg.out_ch_ext.chn_common.addr[0] = out_frame_addr_2;
    isp_cfg.out_ch_ext.chn_common.addr[1] = out_frame_addr_2 + out_frame_size_2;
    isp_cfg.out_ch_ext.chn_common.pp_num = 1;
    isp_cfg.out_ch_ext.chn_common.out_crop.start_x = ((OUT_WIDTH_1 - OUT_WIDTH_EXT) >> 1);
    isp_cfg.out_ch_ext.chn_common.out_crop.start_y = ((OUT_HEIGHT_1 - OUT_HEIGHT_EXT) >> 1);
    isp_cfg.out_ch_ext.chn_common.out_crop.width = OUT_WIDTH_EXT;
    isp_cfg.out_ch_ext.chn_common.out_crop.height = OUT_HEIGHT_EXT;

    isp_cfg.out_callback = isp_event_cb;

    /* isp initialize */
    isp_init(&isp_cfg);

    isp_set_default_param(0, cam_desc->width, cam_desc->height); // use the default parameter to do test, later may use IQ lib to get the parameters

    /* isp proc ctrl for isp module and fast ae */
    vin_cfg.in_size.width  = isp_cfg.in_size.width;
    vin_cfg.in_size.height = isp_cfg.in_size.height;
    vin_cfg.glb_crop.width    = isp_cfg.glb_crop.width;
    vin_cfg.glb_crop.height   = isp_cfg.glb_crop.height;
    memcpy(&vin_cfg.sensor_name, "SC101IOT\0", sizeof(char)*10);
    imgproc_init(&vin_cfg);
    //imgproc_start_fast_isp(cam_desc);
    imgproc_start(cam_desc);

    MM_MISC_Set_IPROAIPRO_Rcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Rcmd_MasterEn(0, 1, 1);
    MM_MISC_Set_IPROAIPRO_Wcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Wcmd_MasterEn(0, 1, 1);

    // set seof interrupt
    set_seof0_cfg();

    // hook callback function for seof interrupt and enable
    Interrupt_Handler_Register(ISP_MM_SEOF0_IRQn, mm_seof0_isr);
    CPU_Interrupt_Enable(ISP_MM_SEOF0_IRQn);

    /* start isp and ready to get image */
    isp_start();
}

#ifdef CONFIG_AI_SUPPORT
static inline uint16_t clamp_u16(uint16_t v, uint16_t lo, uint16_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static uint8_t remap_coordinates(const yolov8_detection_t *det,
                                 LCD_DRAW_IN_BUF_CFG *cfg)
{
#define X_LOW  ((OUT_WIDTH_1  - OUT_WIDTH_EXT)  >> 1)
#define X_HIGH (OUT_WIDTH_EXT  + X_LOW)
#define Y_LOW  ((OUT_HEIGHT_1 - OUT_HEIGHT_EXT) >> 1)
#define Y_HIGH (OUT_HEIGHT_EXT + Y_LOW)

    cfg->line_x1 = clamp_u16(det->box.x1, X_LOW, X_HIGH) - X_LOW;
    cfg->line_y1 = clamp_u16(det->box.y1, Y_LOW, Y_HIGH) - Y_LOW;
    cfg->line_x2 = clamp_u16(det->box.x2, X_LOW, X_HIGH) - X_LOW;
    cfg->line_y2 = clamp_u16(det->box.y2, Y_LOW, Y_HIGH) - Y_LOW;

    if (cfg->line_x2 <= cfg->line_x1) return 1;
    if (cfg->line_y2 <= cfg->line_y1) return 1;
    if ((cfg->line_x2 - cfg->line_x1) <= cfg->thickness) return 1;
    if ((cfg->line_y2 - cfg->line_y1) <= cfg->thickness) return 1;

    return 0;
}

static inline lcd_color_t class_to_color(int class_id)
{
    if (class_id == 0)
        return LCD_COLOR_RGB(0, 0, 255);   // human
    if (class_id == 1)
        return LCD_COLOR_RGB(0, 255, 0);   // pet
    return LCD_COLOR_RGB(128, 128, 128);   // unknown
}

extern void lcd_draw_rec_in_buf(LCD_DRAW_IN_BUF_CFG a_tDrawCfg);

static void get_ai_result_proc(void *arg)
{
    uint32_t result_addr;
    int ret;
    while(1)
    {
        extern int ai_get_result(uint32_t *result_addr);
        ret = ai_get_result(&result_addr);
        if (0 != ret) break;
        if (0 == ai_result_using)
            memcpy((void*)&tmp_result, (void*)result_addr, sizeof(yolov8_result_t));

        // ai done and start streaming
        update_isp_fps(ISP_CHANNEL_1, 30, 30);
    }

    vTaskDelete(NULL);
}
#endif

static void show_lcd_proc(void *arg)
{
#ifdef CONFIG_AI_SUPPORT
    LCD_DRAW_IN_BUF_CFG draw_cfg = {
        .buffer_width = OUT_WIDTH_EXT,
        .buffer_height = OUT_HEIGHT_EXT,
        .thickness = 4
    };
#endif
    while (1) {
        xSemaphoreTake(g_tDbiShowPicNotify, portMAX_DELAY);
#ifdef CONFIG_AI_SUPPORT
        // use CPU to draw image
        ai_result_using = 1;
        if (tmp_result.num_detections > 0)
        {
            draw_cfg.buffer_addr = (lcd_color_t *)(show_lcd_addr + 0x10000000);
            for (int i = 0; i < tmp_result.num_detections; i++) {
                const yolov8_detection_t *det = &tmp_result.detections[i];
                draw_cfg.color = class_to_color(det->class_id);

                if (remap_coordinates(det, &draw_cfg) == 0)
                {
                    // lcd draw coordinate (x2, y2) should minus 1
                    draw_cfg.line_x2 -= 1;
                    draw_cfg.line_y2 -= 1;
                    // printf("box (%.1f, %.1f) (%.1f, %.1f)\r\n", det->box.x1, det->box.y1, det->box.x2, det->box.y2);
                    // printf("draw (%d, %d) (%d, %d)\r\n", draw_cfg.line_x1, draw_cfg.line_y1, draw_cfg.line_x2, draw_cfg.line_y2);
                    lcd_draw_rec_in_buf(draw_cfg);
                }
            }
            // L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)show_lcd_addr, (unsigned long)show_lcd_size);
        }
        ai_result_using = 0;
#endif
        lcd_draw_picture_nonblocking(0, 25, 479, 25+269, (lcd_color_t *)(show_lcd_addr));
    }
    vSemaphoreDelete(g_tDbiShowPicNotify);
    g_tDbiShowPicNotify = NULL;
    vTaskDelete(NULL);
}

// ============================================================================
// public function
// ============================================================================

void lcd_ai_init()
{
    g_tDbiShowPicNotify = xSemaphoreCreateCounting(1, 0);
    xTaskCreate(show_lcd_proc, "show lcd", 2048, NULL, 10, NULL);
    lcd_init();
    lcd_async_callback_register(flush_async_callback);

#ifdef CONFIG_AI_SUPPORT
    xTaskCreate(get_ai_result_proc, "get ai result", 512, NULL, 10, NULL);
#endif

    stream_pipe_init(); // config isp + lcd + ai + sensor stream
}

#ifdef CONFIG_AI_SUPPORT
void record_ai_result(yolov8_result_t *result)
{
    if (0 == ai_result_using)
        memcpy((void*)&tmp_result, result, sizeof(yolov8_result_t));
    // ai done and start streaming
    update_isp_fps(ISP_CHANNEL_1, 30, 30);
}
#endif

// ============================================================================
// Shell function
// ============================================================================
#include "shell.h"

int cmd_lcd_ai_demo(int argc, char **argv)
{
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
    lcd_ai_init();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lcd_ai_demo, lcd_ai_demo, show lcd + AI);

void demo_lcd_ai_init(void)
{
}


static void isp_cmd(char *buf, int len, int argc, char **argv)
{
    isp_cmd2(argc, argv);
}

static int isp_cmd_msh(int argc, char **argv)
{
    isp_cmd(NULL, 0, argc, argv);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(isp_cmd_msh, isp, "read write isp cmd");

