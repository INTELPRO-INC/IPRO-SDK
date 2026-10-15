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
 * @file uvc_isp.c
 * @brief Continuous DVP -> ISP -> MJPEG pipeline for the IPRO7 UVC camera.
 *
 * The UVC streaming path only: no PIR motion flow, no AI/NPU inference, no
 * single-shot capture / frame retrieval API. The sensor streams continuously and every
 * hardware-encoded MJPEG frame is pushed to the host via uvc_send_jpg().
 *
 * Note: the ISP firmware library calls back into ipro_isp_stats_ready_config()
 * (declared in img_proc.h) to register its fast-AE statistics handler. That
 * handler must be invoked on every Start-of-Frame, so the registration hook
 * and the stats_ready_cb dispatch in mm_seof0_isr() are required here.
 */

#include "compiler/common.h"
#include <generated/autoconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "camera.h"
#include "hal_i2c.h"
#include "hal_mjpeg.h"
#include "sccb.h"
#include "uvc_isp.h"

#include <drv_mm_misc.h>

#include "drv_clock.h"
#include "drv_dvp2axi.h"
#include "drv_isp.h"
#include "drv_glb.h"
#include "drv_mjpeg.h"
#include "hal_isp.h"
#include "img_proc.h"
#include <sensor.h>

/* Two AI flavours share this pipeline: the EVB's YOLOv8 person/pet detector
 * (uvc_ai.c, boxes) and the Reef's rock-paper-scissors game (uvc_rps.c,
 * CONFIG_UVC_RPS). Both feed the NPU from ISP channel 1, one frame in flight. */
#if defined(CONFIG_UVC_RPS) && !defined(CONFIG_AI_SUPPORT)
#error "CONFIG_UVC_RPS needs CONFIG_AI_SUPPORT"
#endif
#if defined(CONFIG_AI_SUPPORT) && !defined(CONFIG_UVC_RPS)
#define UVC_AI_YOLO 1
#else
#define UVC_AI_YOLO 0
#endif

#if UVC_AI_YOLO
#include "uvc_ai.h"
#include "detect/yolov8_person_pet_detect.h"
#endif
#ifdef CONFIG_UVC_RPS
#include "rps_model.h"
#include "uvc_rps.h"
#endif

/* Encoded MJPEG frame sink (uvc.c), split so the ring frame is released
 * between staging (copy-out) and the blocking USB push: */
extern int uvc_frame_fill(uint32_t addr,
                          uint32_t size); /* copy into UVC buf; 1=staged */
extern void uvc_frame_flush(void);        /* blocking iso send */
extern uint8_t uvc_frame_index(void);     /* frame the host committed, 1-based */

#define YUV422_FRAME_SIZE(X, Y) ((X) * (Y) * 2)
#define YUV400_FRAME_SIZE(X, Y) ((X) * (Y))
#define ARGB_FRAME_SIZE(X, Y) ((X) * (Y) * 4)

/* Global crop = full SC101IOT sensor (1280x720). */
#define CROP_START_X (0)
#define CROP_START_Y (0)
#define CROP_WIDTH (1280)
#define CROP_HEIGHT (720)

#define OUT_RING_LINE (16)

/*
 * Channel 1 (ARGB) output. Feeds the NPU. Must match the model's input
 * resolution. The BIG-A + SPPF-lite bring-up model is 256x256x3; the old
 * YOLOv8 person/pet model was 640x360. NOTE: scaling the 16:9 sensor crop
 * straight to a square 256x256 squashes the aspect ratio — fine for bring-up
 * (random-weight pipeline validation); for a real-weight model, center-crop
 * the ch1 scaler input to a square (720x720 from the 1280x720 crop) first.
 */
/* ch1 -> NPU input resolution. MUST match the loaded model AND uvc_ai.c.
 *   128       = TinyNet CIFAR-10 classifier (UVC_AI_MODE_CLASSIFY=1 in uvc_ai.c)
 *   256x256   = big_a_sppf / gesture-digit DFL detector, centered square-crop
 *   640x360   = gesture-digit 640x384 letterbox model (AI_INPUT_640X384=1): ch1
 *               outputs the un-padded 640x360 content; uvc_ai.c pads it to 640x384.
 * AI_INPUT_640X384 MUST stay in sync with the same switch in uvc_ai.c. */
#ifndef AI_INPUT_640X384
#define AI_INPUT_640X384 1
#endif
#if AI_INPUT_640X384
#define OUT_WIDTH_1  (640)
#define OUT_HEIGHT_1 (360)        /* content rows; model is 640x384 after letterbox */
#define AI_LB_Y      ((384 - OUT_HEIGHT_1) / 2)  /* 12 gray rows top+bottom in model space */
#else
#define OUT_WIDTH_1  (256)
#define OUT_HEIGHT_1 (256)
#endif

/* ch0 / MJPEG / UVC preview + box-draw target. QVGA 320x240 to fit the 640x384
 * model in PSRAM (must match uvc.c WIDTH/HEIGHT + main.c uvc_isp_set_param). The
 * box remap scales the 640x384 model space into these dims. Restore 640x480 when
 * the model's activation buffer is <= ~4.4 MB. (Name kept as VGA_* for the remap.) */
#define VGA_W (320)
#define VGA_H (240)
#if UVC_AI_YOLO && (UVC_WIDTH != VGA_W || UVC_HEIGHT != VGA_H)
#error "AI box remap assumes the QVGA preview: derive VGA_W/VGA_H from UVC_WIDTH/UVC_HEIGHT first"
#endif

/*
 * Camera control pins — IPRO7 EVB pinout (matches CONFIG_EVB in sccb.h and the
 * reference DVP bring-up in apps/platform/ipro7_demo):
 *   I2C0 SDA/SCL       = GPIO36 / GPIO37  (SCCB; muxed in cam_i2c_init)
 *   MCLK (CAM_REF_CLK) = GPIO41
 *   Sensor PWR         = GPIO34  (GPIO output, driven high to power the sensor)
 *   Sensor RST         = GPIO35  (GPIO output, driven high to release reset)
 */
#if defined(CONFIG_IPRO7AI_REEF)
/*
 * IPRO7AI Reef carrier, FLEX-24 OV5640 (ipro7ai_reef_board_config.h):
 * XCLK on GPIO22, RESETB GPIO34 (active low), PWDN GPIO35 (active high).
 * Same DVP data / sync / SCCB pins as the EVB.
 */
#include "board_config.h"
#define SNS_CAMCLK_PIN  (BOARD_CAM_MCLK_PIN)
#define SNS_I2C_SDA_PIN (BOARD_CAM_SDA_PIN)
#define SNS_I2C_SCL_PIN (BOARD_CAM_SCL_PIN)
#define SNS_PWR_PIN     (BOARD_CAM_PWDN_PIN)
#define SNS_PWR_ON      (0)             /* PWDN low = running */
#define SNS_RST_PIN     (BOARD_CAM_RST_PIN)
#else
#define SNS_CAMCLK_PIN  (GLB_GPIO_PIN_41)
#define SNS_I2C_SDA_PIN (GLB_GPIO_PIN_36)
#define SNS_I2C_SCL_PIN (GLB_GPIO_PIN_37)
#define SNS_PWR_PIN     (GLB_GPIO_PIN_34)
#define SNS_PWR_ON      (1)             /* sensor power switch, high = on */
#define SNS_RST_PIN     (GLB_GPIO_PIN_35)
#endif

/* Encode task stack, words. The game overlay formats its text with snprintf
 * in this task, so it gets more than the plain webcam. */
#ifdef CONFIG_UVC_RPS
#define UVC_JPG_TASK_STACK (1536)
#else
#define UVC_JPG_TASK_STACK (512)
#endif

#define ISP_INIT_DROP_FRAMES (5)
#define SEOF_TEST_LINE_CNT (100)

/* Encoder output buffer. 64KB holds a VGA q50 JPEG (~20-35KB typical); a
 * too-small buffer raises MJENC_HAL_INT_ERR_MEM_OVERWRITE (event 4, drops that
 * frame). Kept small so the two full-frame VGA ping-pong buffers (2x614KB) fit
 * in PSRAM alongside the ~6MB resident AI model. Raise to 96-128KB if busy q50
 * scenes drop frames. 720p (Reef build, no AI model) gets 192KB. */
#if UVC_WIDTH >= 1280
#define JPG_OUT_BUF_SIZE (192 * 1024)
#else
#define JPG_OUT_BUF_SIZE (64 * 1024)
#endif

/*
 * MJPEG quality (1..100, higher = better but larger). The IPRO7 USB device is
 * Full Speed (~1 MB/s iso ceiling), so at 720p the per-frame size sets the
 * frame rate: ~67KB @ q50 -> ~15fps; ~33KB needed for 30fps. Lower this to
 * trade image quality for frame rate. (hal default is 50.)
 */
#define UVC_JPEG_QUALITY 50

static uint8_t ATTR_PSRAM_SECTION jpg_head_buf[800] = {0};
static uint32_t jpg_head_len;

static const rt_camera_desc *cam_desc;

static SemaphoreHandle_t gJencNotify;

static uint32_t gJencAddr = 0;

/* ch0 full-frame YUYV ping-pong buffers (PSRAM). The ISP writes one while the
 * encode task draws boxes into + encodes the other. g_yuv_ready = address of the
 * most-recently completed frame (set in isp_event_cb). */
static uint32_t g_yuv_pp[2] = {0, 0};
static volatile uint32_t g_yuv_ready = 0;
static volatile uint32_t g_frame_seq = 0;   /* ch0 frames completed */

/* The Reef's FLEX-24 OV5640 reads out top-to-bottom mirrored and the sensor
 * cannot flip in its binned mode without breaking the demosaic
 * (components/sensor/ov5640.c), so the capture DMA writes bottom-up. In flip
 * mode DVP2AXI puts the first line at the start address and each next one a
 * line lower, so it is given each buffer's last line
 * (apps/platform/ipro7ai_reef_demo/src/cam.c does the same). */
#if defined(CONFIG_IPRO7AI_REEF)
#define UVC_DVP_VFLIP 1
#else
#define UVC_DVP_VFLIP 0
#endif
static uint32_t g_flip_off = 0;     /* DMA start = buffer + this */
/* SW-mode MJPEG: first encode arms the engine (mjpeg_set_mode+start), the rest
 * use mjpeg_sw_run_next. Reset whenever the MJPEG is (re)configured. */
static bool g_mjpeg_first_run = true;

static bool gIsTaskRunning = false;

static uint8_t gConvergedFrm = ISP_INIT_DROP_FRAMES;
static bool gStartStream = true;

static ipro_isp_stats_ready_cb stats_ready_cb;

static isp_res_mode_e gResMode = ISP_RES_MODE_VGA;
static bool gIsColor = true;
static volatile bool gSizeConfigured = false;

#ifdef CONFIG_UVC_RPS
/* Model loaded + NPU task up; false = plain webcam with a "NO MODEL" box. */
static bool gRpsReady = false;
/* ch1 DMA start offset into the model's input frame (see UVC_DVP_VFLIP). */
static uint32_t g_flip_off1 = 0;
/* Private ch0-sized frame: the game is drawn here and encoded from here. */
static uint32_t g_rps_out = 0;

static void rps_npu_done(void *arg);
#endif

#if UVC_AI_YOLO
/* ISP channel 1 (ARGB) feeds the NPU; single buffer in PSRAM. */
static uint32_t gIspOutput_1 = 0;
/* Set once the model is loaded + inference task is up. When false the app is a
 * plain webcam (no ch1, no boxes), so a missing model never breaks streaming. */
static bool gAiReady = false;
/* Latest detection snapshot drawn by the encode task. */
static yolov8_result_t ATTR_PSRAM_SECTION gAiResult;
static volatile uint8_t gAiResultUsing = 0;
static volatile uint8_t gAiResultUpdate = 0;

/* The NPU model wants a SQUARE input. Rather than squashing the full 16:9 frame
 * into 256x256 (distorts products horizontally vs the model's training), ch1
 * takes a centered SQUARE crop of the sensor frame and scales that to 256x256.
 * Detection boxes (256 model space) therefore map back to this centered
 * 720x720 region of the 1280x720 sensor, then to VGA — see remap_model_to_vga().
 * Side = min(W,H); centered horizontally. */
#define AI_CROP_SIDE  (CROP_HEIGHT)                        /* 720 = min(1280,720) */
#define AI_CROP_OFF_X (((CROP_WIDTH) - (AI_CROP_SIDE)) / 2) /* 280 */
#define AI_CROP_OFF_Y (((CROP_HEIGHT) - (AI_CROP_SIDE)) / 2)/* 0   */

static void get_ai_result_proc(void *arg);
static void draw_all_detections(uint8_t *buf, int W, int H);
#endif

static void isp_dvp_pin_mux(void);
static void cam_i2c_init(void);
static void isp_enable_camera_module(int enable);
static void get_mode_size(isp_res_mode_e res_mode, uint16_t *width,
                          uint16_t *height);
static void set_isp_channel0_cfg(void);
static void set_mjpg_cfg_n_start(void);
static void set_seof0_cfg(void);
static void update_isp_fps(uint8_t port, uint8_t src, uint8_t dst);
static void mjpeg_enc_event_cb(uint32_t event);
static void isp_event_cb(isp_out_frame_info_t info);
static void mm_seof0_isr(void);
static void isp_pipe_init(void);
static void uvc_get_jpg_proc(void *arg);

// ============================================================================
// local function
// ============================================================================
static void isp_dvp_pin_mux(void) {
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[] = {GLB_GPIO_PIN_0,  GLB_GPIO_PIN_1, GLB_GPIO_PIN_2,
                          GLB_GPIO_PIN_3,  GLB_GPIO_PIN_4, GLB_GPIO_PIN_5,
                          GLB_GPIO_PIN_6,  GLB_GPIO_PIN_7, GLB_GPIO_PIN_38,
                          GLB_GPIO_PIN_39, GLB_GPIO_PIN_40};
    int i;

    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive = 0;
    cfg.smtCtrl = 1;

    for (i = 0; i < sizeof(gpiopins) / sizeof(gpiopins[0]); i++) {
        cfg.gpioPin = gpiopins[i];
        cfg.gpioFun = GPIO_FUN_CAM;
        GLB_GPIO_Init(&cfg);
    }
}

/* SCCB / camera I2C0 on the EVB pins GPIO36 (SDA) / GPIO37 (SCL). Both pins take
 * GPIO_FUN_I2C; the SoC fixes which is SCL vs SDA. Equivalent to the shared
 * SCCB_Init() for CONFIG_EVB, kept local so the app owns its pin bring-up. */
static void cam_i2c_init(void) {
    GLB_GPIO_Cfg_Type cfg;
    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioFun = GPIO_FUN_I2C;

    cfg.gpioPin = SNS_I2C_SCL_PIN;
    GLB_GPIO_Init(&cfg);
    cfg.gpioPin = SNS_I2C_SDA_PIN;
    GLB_GPIO_Init(&cfg);

    i2c_init(I2C0_INDEX);
}

static void isp_enable_camera_module(int enable) {
    GLB_GPIO_Cfg_Type cfg;

    /* CAM_REF_CLK (MCLK) on GPIO41. */
    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive = 3;
    cfg.smtCtrl = 1;
    cfg.gpioPin = SNS_CAMCLK_PIN;
    cfg.gpioFun = GPIO_FUN_CAM_MISC;
    GLB_GPIO_Init(&cfg);

    /* PLL48M -> CAM_REF_CLK = 24MHz */
    GLB_Set_CAM_CLK(DISABLE, GLB_CAM_CLK_PLL48M, 1);

    /* Sensor power (EVB: power switch GPIO34; Reef: PWDN GPIO35) and reset
     * are SoC-controlled. Configure both as GPIO outputs, held off / in reset
     * until the clock is enabled. */
    cfg.gpioMode = GPIO_MODE_OUTPUT;
    cfg.pullType = GPIO_PULL_UP;
    cfg.gpioFun = GPIO_FUN_GPIO;

    cfg.gpioPin = SNS_PWR_PIN;
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_Write(SNS_PWR_PIN, !SNS_PWR_ON);
    GLB_GPIO_OUTPUT_Enable(SNS_PWR_PIN);

    cfg.gpioPin = SNS_RST_PIN;
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_Write(SNS_RST_PIN, 0);
    GLB_GPIO_OUTPUT_Enable(SNS_RST_PIN);

    if (enable) {
#if defined(CONFIG_IPRO7AI_REEF)
        /* OV5640 datasheet sec. 2.7: XVCLK >= 1 ms before PWDN is released,
         * RESETB high >= 20 ms before the first SCCB access. */
        GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_PLL48M, 1);
        vTaskDelay(5);
        GLB_GPIO_Write(SNS_PWR_PIN, SNS_PWR_ON);
        vTaskDelay(5);
        GLB_GPIO_Write(SNS_RST_PIN, 1);
        vTaskDelay(25);
#else
        GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_PLL48M, 1);
        vTaskDelay(1);

        GLB_GPIO_Write(SNS_PWR_PIN, SNS_PWR_ON); /* power on */
        GLB_GPIO_Write(SNS_RST_PIN, 1); /* release reset */
        vTaskDelay(12);                 /* let sensor + MCLK settle before I2C probing */
#endif
    }
}

static void get_mode_size(isp_res_mode_e res_mode, uint16_t *width,
                          uint16_t *height) {
    switch (res_mode) {
    case ISP_RES_MODE_VGA:
        *width = 640;
        *height = 480;
        break;
    case ISP_RES_MODE_QVGA:
        *width = 320;
        *height = 240;
        break;
    case ISP_RES_MODE_360P:
        *width = 640;
        *height = 360;
        break;
    case ISP_RES_MODE_180P:
        *width = 320;
        *height = 184; // due to JPG hw limitation for 8-alignment, JPG header
                       // update to 320x180
        break;
    case ISP_RES_MODE_1080P:
        *width = 1920;
        *height = 1080;
        break;
    case ISP_RES_MODE_720P:
    default:
        *width = 1280;
        *height = 720;
        break;
    }
}

static void set_isp_channel0_cfg(void) {
    uint16_t width, height;
    isp_hal_out_chnl_0_t chn0_cfg;

    get_mode_size(gResMode, &width, &height);

    // update channel 0 size
    chn0_cfg.enable = 1;
    chn0_cfg.scaler_cfg.in.start_x = 0;
    chn0_cfg.scaler_cfg.in.start_y = 0;
    chn0_cfg.scaler_cfg.in.width = CROP_WIDTH;
    chn0_cfg.scaler_cfg.in.height = CROP_HEIGHT;
    chn0_cfg.scaler_cfg.out.width = width;
    chn0_cfg.scaler_cfg.out.height = height;
    chn0_cfg.format =
        (gIsColor) ? ISP_HAL_OUT_FMT0_YUYV : ISP_HAL_OUT_FMT0_Y_ONLY;
    chn0_cfg.ring_line = 0; /* full-frame mode (no MJPEG ring handshake) */
    chn0_cfg.chn_common.fr_ctrl.src_fps = 30;
    chn0_cfg.chn_common.fr_ctrl.dst_fps =
        0; /* drop first, start stream when sensor converged */
    g_flip_off = UVC_DVP_VFLIP ? (uint32_t)(height - 1u) * width * (gIsColor ? 2u : 1u) : 0u;
    chn0_cfg.chn_common.addr[0] = g_yuv_pp[0] + g_flip_off;
    chn0_cfg.chn_common.addr[1] = g_yuv_pp[1] + g_flip_off;
    chn0_cfg.chn_common.pp_num = 2; /* ping-pong: ISP writes one, CPU draws+encodes other */
    chn0_cfg.chn_common.out_crop.start_x = 0;
    chn0_cfg.chn_common.out_crop.start_y = 0;
    chn0_cfg.chn_common.out_crop.width = width;
    chn0_cfg.chn_common.out_crop.height = height;

    isp_update_chn0_cfg(chn0_cfg);
}

static void set_mjpg_cfg_n_start(void) {
    struct mjpeg_hal_cfg_t mjpeg_cfg;
    uint16_t width, height;

    get_mode_size(gResMode, &width, &height);

    mjpeg_cfg.format =
        (gIsColor) ? MJPEG_HAL_FORMAT_YUV422_YUYV : MJPEG_HAL_FORMAT_GRAY;
    mjpeg_cfg.quality = UVC_JPEG_QUALITY;
    mjpeg_cfg.rows = height; /* full frame (not ring) */
    mjpeg_cfg.resolution_x = width;
    mjpeg_cfg.resolution_y = height;
    /* YUYV422 is packed; the driver wants the Y-plane base and the "UV" base
     * (here = base + W*H, the convention used in full-frame SW mode). */
    mjpeg_cfg.input_bufaddr0 = g_yuv_pp[0];
    mjpeg_cfg.input_bufaddr1 = g_yuv_pp[0] + YUV400_FRAME_SIZE(width, height);
    mjpeg_cfg.output_bufaddr = gJencAddr;
    mjpeg_cfg.input_yy_table = NULL;
    mjpeg_cfg.input_uv_table = NULL;
    mjpeg_cfg.output_swap_mode_en = 0; /* 0 frame mode, 1 swap mode */
    mjpeg_cfg.hw_mode_swen_en = 0;     /* SW mode: encode task drives each frame */
    mjpeg_cfg.output_bufsize = JPG_OUT_BUF_SIZE;
    mjpeg_cfg.hw_mode_sshot_en = 0;
    mjpeg_cfg.callback = mjpeg_enc_event_cb;

    if (ISP_RES_MODE_180P == gResMode) {
        height = 180; // 180P height hardcode for JPG header (JPG hw limit by
                      // 8-align)
    }

    mjpeg_init(&mjpeg_cfg);
    /* SW mode: do NOT auto-start here; the encode task arms the engine on its
     * first frame (mjpeg_set_mode + mjpeg_start) and uses mjpeg_sw_run_next
     * thereafter. */
    g_mjpeg_first_run = true;
    if (gIsColor)
        jpg_head_len = mjpeg_headerCreate(YUV_MODE_422, UVC_JPEG_QUALITY, width,
                                          height, jpg_head_buf);
    else
        jpg_head_len = mjpeg_headerCreate(YUV_MODE_400, UVC_JPEG_QUALITY, width,
                                          height, jpg_head_buf);
    mjpeg_fill_header_tail(jpg_head_buf, jpg_head_len);
}

static void set_seof0_cfg(void) {
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
    isp_hal_frame_rate_t frc = {.src_fps = src, .dst_fps = dst};
    isp_fps_cfg(port, frc);
}

#ifdef CONFIG_UVC_RPS
/* Set by the NPU task when it has read the ch1 frame; mm_seof0_isr re-arms
 * ch1. Kept out of task context because isp_fps_cfg() re-applies the
 * frame-skip of every channel with a pending flag, unguarded, and the SEOF ISR
 * re-arms ch0 every frame: one context for all frame-skip writes. */
static volatile bool g_rps_rearm = false;

/* NPU finished reading the ch1 frame: let the ISP write the next one. */
static void rps_npu_done(void *arg) {
    (void)arg;
    g_rps_rearm = true;
}
#endif

static void mjpeg_enc_event_cb(uint32_t event) {
    if (event == MJENC_HAL_INT_NORMAL) {
        xSemaphoreGiveFromISR(gJencNotify, NULL);
    } else if (event == MJENC_HAL_INT_IDLE) {
        // do nothing
    } else {
        printf("[UVC_ISP] Unknown jenc event %lu\r\n", event);
    }
}

static void isp_event_cb(isp_out_frame_info_t info) {
    /* Channel 0 (MJPEG source) streams continuously; nothing to do on NORMAL.
     * Channel 1 (when AI is enabled) carries ARGB frames for the NPU. */
    if (info.event == ISP_HAL_INT_NORMAL) {
        if (ISP_CHANNEL_0 == info.out_port) {
            /* Latest completed full-frame YUYV buffer; the encode task draws
             * boxes into it then SW-encodes it. */
            g_yuv_ready = info.out_addr - g_flip_off;
            g_frame_seq++;
        }
#if UVC_AI_YOLO
        if (gAiReady && ISP_CHANNEL_1 == info.out_port && 0 == gConvergedFrm) {
            /* One frame in flight: stop ch1 until the NPU consumes this one
             * (re-armed to 30fps by get_ai_result_proc). Keeps ch1 from
             * stealing DVP/MJPEG bandwidth from the UVC stream. */
            update_isp_fps(info.out_port, 30, 0);
            uvc_ai_push_buffer(info.out_addr);
        }
#endif
#ifdef CONFIG_UVC_RPS
        if (gRpsReady && ISP_CHANNEL_1 == info.out_port && 0 == gConvergedFrm) {
            /* Same one-frame-in-flight gate; rps_npu_done re-arms ch1. The
             * NPU gets the frame base, not the flipped DMA start. */
            update_isp_fps(info.out_port, 30, 0);
            if (rps_model_push(info.out_addr - g_flip_off1) != 0) {
                g_rps_rearm = true;     /* NPU busy: try the next frame */
            }
        }
#endif
    }
    if (info.event == ISP_HAL_INT_FIFO_OVERWRITE) {
        printf("[UVC_ISP] OUT %ld FIFO_OVERWRITE\r\n", info.out_port);
    }
    if (info.event == ISP_HAL_INT_VSYNC_CNT_ERROR) {
        printf("[UVC_ISP] OUT %ld VSYNC_CNT_ERROR\r\n", info.out_port);
    }
    if (info.event == ISP_HAL_INT_HSYNC_CNT_ERROR) {
        printf("[UVC_ISP] OUT %ld HSYNC_CNT_ERROR\r\n", info.out_port);
    }
}

static void mm_seof0_isr(void) {
    MM_MISC_SEOF_TYPE_E type_status;
    MM_MISC_Get_SEOF_TypeStatus(MM_MISC_SEOF0, &type_status);
    if (type_status.BF.SoF) {
        /* Drive the ISP firmware's fast-AE statistics handler. */
        if (stats_ready_cb) {
            stats_ready_cb(NULL);
        }
    }
    if (type_status.BF.EoF) {
    }
    if (type_status.BF.lineCnt) {
        if (0 != gConvergedFrm)
            gConvergedFrm--;
        else {
            // image converged: configure size once, then stream both outputs
            if (gStartStream) {
                if (!gSizeConfigured) // ~550us
                {
                    set_mjpg_cfg_n_start();
                    set_isp_channel0_cfg();
                    /* Start ch1 capture once. Thereafter it is throttled
                     * one-frame-in-flight by isp_event_cb / get_ai_result_proc
                     * (rps_npu_done for the game); do NOT re-arm it every frame
                     * here or the gate is lost. */
#if UVC_AI_YOLO
                    if (gAiReady) {
                        update_isp_fps(ISP_CHANNEL_1, 30, 30);
                    }
#endif
#ifdef CONFIG_UVC_RPS
                    if (gRpsReady) {
                        update_isp_fps(ISP_CHANNEL_1, 30, 30);
                    }
#endif
                    gSizeConfigured = true;
                }
                update_isp_fps(ISP_CHANNEL_0, 30, 30);
#ifdef CONFIG_UVC_RPS
                if (g_rps_rearm) {
                    g_rps_rearm = false;
                    update_isp_fps(ISP_CHANNEL_1, 30, 30);
                }
#endif
            }
        }
    }
    MM_MISC_Clr_SEOF_TypeStatus(MM_MISC_SEOF0, type_status);
}

int ipro_isp_stats_ready_config(ipro_isp_stats_ready_cb cb, void *usrdata) {
    (void)usrdata;
    stats_ready_cb = cb;

    return 0;
}

static void isp_pipe_init(void) {
    isp_hal_cfg_t isp_cfg = {0};
    isp_hal_vin_cfg_t vin_cfg = {0};

    cam_i2c_init();              /* camera I2C0 on GPIO36/37 */
    isp_enable_camera_module(1); /* MCLK GPIO41 + PWR GPIO34 / RST GPIO35 */
    isp_dvp_pin_mux();

    printf("[UVC_ISP] camera_module_init\r\n");
    while (1) {
        cam_desc = camera_module_init(); // probe + reset sensor
        if (NULL == cam_desc) {
            printf("[UVC_ISP] Error: cam_desc is NULL\r\n");
            continue;
        }
        break;
    }

    /* sensor config and stream */
    camera_module_start(cam_desc);

    /* reset isp cfg */
    memset((void *)&isp_cfg, 0, sizeof(isp_cfg));

    /* ch0 full-frame YUYV ping-pong buffers (PSRAM). Allocated AFTER the AI
     * model loads (uvc_ai_init runs first in uvc_isp_init) so the model's
     * resident buffer is reserved before these. */
    {
        uint16_t w, h;
        uint32_t yuv_sz;
        get_mode_size(gResMode, &w, &h);
        yuv_sz = YUV422_FRAME_SIZE(w, h); /* VGA: 640*480*2 = 614400 */
        for (int i = 0; i < 2; i++) {
            if (0 == g_yuv_pp[i]) {
                g_yuv_pp[i] = (uint32_t)pvPortMalloc(yuv_sz);
                if (!g_yuv_pp[i]) {
                    printf("[UVC_ISP] %s[%d] ch0 buf %d malloc failed (%u)\r\n",
                           __func__, __LINE__, i, (unsigned)yuv_sz);
                    return;
                }
            }
        }
#ifdef CONFIG_UVC_RPS
        /* The frame the game is drawn on and the encoder reads. Without it
         * the stream is drawn in place and tears. */
        if (0 == g_rps_out) {
            g_rps_out = (uint32_t)pvPortMalloc(yuv_sz);
            if (!g_rps_out) {
                printf("[UVC_ISP] game frame malloc failed (%u); streaming without "
                       "the game\r\n", (unsigned)yuv_sz);
            }
        }
#endif
    }

    /* jpg output buffer */
    if (0 == gJencAddr) {
        gJencAddr = (uint32_t)pvPortMalloc(JPG_OUT_BUF_SIZE);
        if (!gJencAddr) {
            printf("[UVC_ISP] %s[%d] error: jenc malloc failed! (size: %u)\r\n",
                   __func__, __LINE__, JPG_OUT_BUF_SIZE);
            return;
        }
    }

    /* isp config */
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

    /* Channel 0 (MJPEG source) configured later in mm_seof0_isr once converged.
     */
    isp_cfg.out_ch_0.enable = 0;

    /*
     * Channel 1 (ARGB 640x360) feeds the NPU when AI is enabled. Without AI it
     * stays disabled (out_ch_1.enable == 0 from the zero-init): a constant
     * 30fps ch1 stole DVP/ISP/memory bandwidth and halved the ch0 stream to
     * ~15fps. With AI it is captured one-frame-in-flight (see isp_event_cb /
     * get_ai_result_proc) so the steady-state ch0 cost stays low.
     */
#ifdef CONFIG_UVC_RPS
    /* Rock-paper-scissors: the centred 512x512 play box scaled 4:1 to the
     * model's 128x128 ARGB, written straight into the model's input frame
     * (the NPU reads it via ext_input). Flipped like ch0 on the Reef. */
    if (gRpsReady) {
        uint32_t base = rps_model_input_addr();

        g_flip_off1 = UVC_DVP_VFLIP ? (uint32_t)(RPS_IN_H - 1u) * RPS_IN_W * 4u : 0u;
        isp_cfg.out_ch_1.enable = 1;
        isp_cfg.out_ch_1.scaler_cfg.in.start_x = UVC_RPS_ROI_X(CROP_WIDTH);
        isp_cfg.out_ch_1.scaler_cfg.in.start_y = UVC_RPS_ROI_Y(CROP_HEIGHT);
        isp_cfg.out_ch_1.scaler_cfg.in.width = UVC_RPS_ROI_SIDE;
        isp_cfg.out_ch_1.scaler_cfg.in.height = UVC_RPS_ROI_SIDE;
        isp_cfg.out_ch_1.scaler_cfg.out.width = RPS_IN_W;
        isp_cfg.out_ch_1.scaler_cfg.out.height = RPS_IN_H;
        isp_cfg.out_ch_1.format = ISP_HAL_OUT_FMT1_ARGB;
        isp_cfg.out_ch_1.chn_common.fr_ctrl.src_fps = 30;
        isp_cfg.out_ch_1.chn_common.fr_ctrl.dst_fps = 0; /* gated on */
        isp_cfg.out_ch_1.chn_common.addr[0] = base + g_flip_off1;
        isp_cfg.out_ch_1.chn_common.addr[1] = base + g_flip_off1;
        isp_cfg.out_ch_1.chn_common.pp_num = 1;
        isp_cfg.out_ch_1.chn_common.out_crop.start_x = 0;
        isp_cfg.out_ch_1.chn_common.out_crop.start_y = 0;
        isp_cfg.out_ch_1.chn_common.out_crop.width = RPS_IN_W;
        isp_cfg.out_ch_1.chn_common.out_crop.height = RPS_IN_H;
    }
#endif
#if UVC_AI_YOLO
    if (gAiReady) {
        uint32_t ch1_size = ARGB_FRAME_SIZE(OUT_WIDTH_1, OUT_HEIGHT_1);
        if (0 == gIspOutput_1) {
            /* Prefer the model's input buffer: the int8 gesture model shares one
             * buffer for ch1 + NPU input (ISP writes the content into its letterbox
             * window, the inference task sign-flips in place). Sharing frees the
             * separate ch1 frame (~0.9MB) so the 640x384 model + VGA preview fit.
             * Returns 0 for models that feed the raw frame (big_a/yolov8) -> then
             * allocate a private single buffer (one-frame-in-flight gating makes
             * single-buffer safe; the model already eats ~6MB of PSRAM). */
            gIspOutput_1 = uvc_ai_get_ch1_target();
            if (0 == gIspOutput_1) {
                gIspOutput_1 = (uint32_t)pvPortMalloc(ch1_size);
            }
        }
        if (gIspOutput_1) {
            isp_cfg.out_ch_1.enable = 1;
#if AI_INPUT_640X384
            /* Full field-of-view: scale the whole 1280x720 crop to 640x360 (clean
             * 2x, no distortion). uvc_ai.c letterboxes 640x360 -> 640x384 for the
             * model. Boxes map back via remap_model_to_vga (full-frame branch). */
            isp_cfg.out_ch_1.scaler_cfg.in.start_x = 0;
            isp_cfg.out_ch_1.scaler_cfg.in.start_y = 0;
            isp_cfg.out_ch_1.scaler_cfg.in.width = CROP_WIDTH;
            isp_cfg.out_ch_1.scaler_cfg.in.height = CROP_HEIGHT;
#else
            /* Center square-crop the 16:9 frame, then scale to 256x256 (no
             * aspect distortion). See AI_CROP_* and remap_coordinates(). */
            isp_cfg.out_ch_1.scaler_cfg.in.start_x = AI_CROP_OFF_X;
            isp_cfg.out_ch_1.scaler_cfg.in.start_y = AI_CROP_OFF_Y;
            isp_cfg.out_ch_1.scaler_cfg.in.width = AI_CROP_SIDE;
            isp_cfg.out_ch_1.scaler_cfg.in.height = AI_CROP_SIDE;
#endif
            isp_cfg.out_ch_1.scaler_cfg.out.width = OUT_WIDTH_1;
            isp_cfg.out_ch_1.scaler_cfg.out.height = OUT_HEIGHT_1;
            isp_cfg.out_ch_1.format = ISP_HAL_OUT_FMT1_ARGB;
            isp_cfg.out_ch_1.chn_common.fr_ctrl.src_fps = 30;
            isp_cfg.out_ch_1.chn_common.fr_ctrl.dst_fps = 0; /* gated on */
            isp_cfg.out_ch_1.chn_common.addr[0] = gIspOutput_1;
            isp_cfg.out_ch_1.chn_common.addr[1] = gIspOutput_1;
            isp_cfg.out_ch_1.chn_common.pp_num = 1;
            isp_cfg.out_ch_1.chn_common.out_crop.start_x = 0;
            isp_cfg.out_ch_1.chn_common.out_crop.start_y = 0;
            isp_cfg.out_ch_1.chn_common.out_crop.width = OUT_WIDTH_1;
            isp_cfg.out_ch_1.chn_common.out_crop.height = OUT_HEIGHT_1;
        } else {
            printf("[UVC_ISP] ch1 malloc failed; AI detection disabled\r\n");
            gAiReady = false;
        }
    }
#endif

    isp_cfg.out_callback = isp_event_cb;

    /* isp initialize */
    isp_init(&isp_cfg);
#if UVC_DVP_VFLIP
    /* ch0 = dvp2axi port A; DVP2AXI_Init read-modify-writes the config, so
     * the bit survives ch0 being configured later */
    DVP2AXI_FlipModeEnable(DVP2AXI_PORT_A);
#ifdef CONFIG_UVC_RPS
    /* ch1 = dvp2axi port C (hal_isp.c get_dvp_port_by_output): the NPU must
     * see the hand the right way up too. */
    if (gRpsReady) {
        DVP2AXI_FlipModeEnable(DVP2AXI_PORT_C);
    }
#endif
    {
        /* same colour tuning as the Reef demo / remote (ipro7ai_reef_demo cam.c) */
        ISP_SATURATION_S sat = { 0x90, 0x90 };

        ISP_Set_Saturation(&sat);
    }
#endif

#if UVC_AI_YOLO
    if (gAiReady && gIspOutput_1) {
        L1C_DCACHE_CLEAN_INVALID_RANGE(
            (unsigned long)gIspOutput_1,
            (unsigned long)ARGB_FRAME_SIZE(OUT_WIDTH_1, OUT_HEIGHT_1));
    }
#endif
#ifdef CONFIG_UVC_RPS
    if (gRpsReady) {
        L1C_DCACHE_CLEAN_INVALID_RANGE(
            (unsigned long)rps_model_input_addr(),
            (unsigned long)ARGB_FRAME_SIZE(RPS_IN_W, RPS_IN_H));
    }
#endif

    /* isp proc ctrl for isp module and fast ae. Only the SC101IOT has an IQ
     * table here (uvc_iq_sc101iot.c); the OmniVision FLEX-24 sensors run their
     * own AEC/AWB and stream finished YUV, so the ISP firmware stays off. */
    if (strstr(cam_desc->name, "sc101iot")) {
        vin_cfg.in_size.width = isp_cfg.in_size.width;
        vin_cfg.in_size.height = isp_cfg.in_size.height;
        vin_cfg.glb_crop.width = isp_cfg.glb_crop.width;
        vin_cfg.glb_crop.height = isp_cfg.glb_crop.height;
        memcpy(&vin_cfg.sensor_name, "SC101IOT\0", sizeof(char) * 10);
        imgproc_init(&vin_cfg);
        imgproc_start_fast_isp(cam_desc);
    } else {
        printf("[UVC_ISP] %s runs its own 3A; ISP firmware 3A not started\r\n",
               cam_desc->name);
    }

    /* set seof interrupt */
    set_seof0_cfg();

    /* hook callback for seof interrupt and enable */
    Interrupt_Handler_Register(ISP_MM_SEOF0_IRQn, mm_seof0_isr);
    CPU_Interrupt_Enable(ISP_MM_SEOF0_IRQn);

    /* NPU bandwidth limiter (MM_MISC IPROAI_LMTR_RD/WR): in proactive mode the
     * NPU's AXI commands give way while the listed DVP2AXI masters are busy.
     * Channel 1 uses dvp2axi_c. The game also lists dvp2axi_a (ch0): without
     * it every inference overran ch0's write FIFO on the Reef - measured, one
     * FIFO_OVERWRITE + VSYNC_CNT_ERROR per inference, encode 16 fps; with it
     * 0 errors, 31 fps, the NPU unchanged at ~15 inferences/s. */
#ifdef CONFIG_UVC_RPS
    MM_MISC_Set_IPROAIPRO_Rcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Rcmd_MasterEn(1, 0, 1);
    MM_MISC_Set_IPROAIPRO_Wcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Wcmd_MasterEn(1, 0, 1);
#else
    MM_MISC_Set_IPROAIPRO_Rcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Rcmd_MasterEn(0, 0, 1);
    MM_MISC_Set_IPROAIPRO_Wcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Wcmd_MasterEn(0, 0, 1);
#endif

    /* jpg source select (ring mode) */
    MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_Y, MM_MISC_DVP2AXI_PORT_A);
    MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_C, MM_MISC_DVP2AXI_PORT_B);

    /* start isp */
    isp_start();
}

/* Full-frame SW-mode encode loop: grab the latest completed YUYV ch0 frame,
 * software-draw all detection boxes into it (if AI), SW-encode it, push to UVC.
 * Replaces the old ring-mode HW auto-encode consumer. */
static void uvc_get_jpg_proc(void *arg) {
    (void)arg;
    uint32_t jenc_size, jenc_addr;
    uint32_t enc_cnt = 0, sent_cnt = 0, sent_bytes = 0, last_size = 0;
    TickType_t last = xTaskGetTickCount();
    uint16_t w, h;
    uint32_t yuv_sz;
    bool streaming_hook_done = false;
    uint32_t last_seq = 0, skip_until = 0;

    get_mode_size(gResMode, &w, &h);
    yuv_sz = YUV422_FRAME_SIZE(w, h);

    while (1) {
#if defined(UVC_ALT_WIDTH)
        /* The host picked the other frame descriptor: rebuild the encoder and
         * ch0 at that size. The SEOF ISR does both once gSizeConfigured is
         * cleared; frames already in flight at the old size are skipped. */
        {
            isp_res_mode_e want = (uvc_frame_index() == 2) ? UVC_ALT_RES_MODE : UVC_RES_MODE;

            if (want != gResMode && gSizeConfigured) {
                mjpeg_stop();
                mjpeg_deinit();
                gResMode = want;
                get_mode_size(gResMode, &w, &h);
                yuv_sz = YUV422_FRAME_SIZE(w, h);
                g_yuv_ready = 0;
                gSizeConfigured = false;
                skip_until = g_frame_seq + 3;
                printf("[UVC] host selected %ux%u\r\n", w, h);
                continue;
            }
        }
#endif
        /* Wait for a new, converged, completed full frame (encoding the same
         * frame twice only inflates the sent count). */
        if (0 == g_yuv_ready || !gSizeConfigured || gConvergedFrm > 0 ||
            g_frame_seq == last_seq || (int32_t)(g_frame_seq - skip_until) < 0) {
            vTaskDelay(pdMS_TO_TICKS(2));
            continue;
        }
        last_seq = g_frame_seq;
        uint32_t yuv = g_yuv_ready;

        /* Frames flow: the sensor's once-per-stream settings (rt_camera_desc). */
        if (!streaming_hook_done) {
            streaming_hook_done = true;
            if (cam_desc && cam_desc->streaming && cam_desc->streaming() != 0) {
                printf("[UVC_ISP] %s streaming hook failed\r\n", cam_desc->name);
            }
        }

        /* ISP DMA wrote this buffer bypassing L1 — invalidate before CPU draws. */
        L1C_DCACHE_INVALID_RANGE((unsigned long)yuv, (unsigned long)yuv_sz);

#if UVC_AI_YOLO
        if (gAiReady) {
            draw_all_detections((uint8_t *)yuv, w, h);
            /* Flush CPU box writes so the MJPEG DMA reads them. */
            L1C_DCACHE_CLEAN_RANGE((unsigned long)yuv, (unsigned long)yuv_sz);
        }
#endif
#ifdef CONFIG_UVC_RPS
        /* Game screen on a private copy of this frame (also when the model
         * failed: the box then says why), and the copy is what gets encoded -
         * see uvc_rps_overlay(). Flush so the MJPEG DMA reads it. */
        if (g_rps_out) {
            uvc_rps_overlay((uint8_t *)g_rps_out, (const uint8_t *)yuv, w, h,
                            CROP_WIDTH, CROP_HEIGHT);
            L1C_DCACHE_CLEAN_RANGE((unsigned long)g_rps_out, (unsigned long)yuv_sz);
            yuv = g_rps_out;
        }
#endif

        /* Point the encoder at this frame and run one SW-mode encode. */
        mjpeg_set_input_addr(MJPEG_SET_ADDR_Y, yuv);
        mjpeg_set_input_addr(MJPEG_SET_ADDR_UV, yuv + YUV400_FRAME_SIZE(w, h));
        if (g_mjpeg_first_run) {
            mjpeg_set_mode(1, 1, 1); /* sw mode, frame mode, 1 frame */
            mjpeg_start(1);
            g_mjpeg_first_run = false;
        } else {
            mjpeg_sw_run_next();
        }

        /* Wait for encode-done (mjpeg_enc_event_cb gives gJencNotify). */
        if (xSemaphoreTake(gJencNotify, pdMS_TO_TICKS(3000)) != pdTRUE) {
            continue;
        }
        if (mjpeg_get_one_frame(&jenc_addr, &jenc_size) != 0) {
            continue;
        }
        L1C_DCACHE_CLEAN_INVALID_RANGE((unsigned long)jenc_addr,
                                       (unsigned long)jenc_size);
        enc_cnt++;
        last_size = jenc_size;
        if (uvc_frame_fill(jenc_addr, jenc_size)) {
            mjpeg_drop_one_frame();
            uvc_frame_flush();
            sent_cnt++;
            sent_bytes += jenc_size;
        } else {
            mjpeg_drop_one_frame();
        }

        /* Once/sec: enc vs sent fps + USB throughput + avg frame size. */
        TickType_t now = xTaskGetTickCount();
        if ((now - last) >= pdMS_TO_TICKS(1000)) {
            printf("[UVC] enc %lu fps, sent %lu fps, %lu KB/s, frame %lu KB\r\n",
                   (unsigned long)enc_cnt, (unsigned long)sent_cnt,
                   (unsigned long)(sent_bytes / 1024),
                   (unsigned long)(last_size / 1024));
            enc_cnt = 0;
            sent_cnt = 0;
            sent_bytes = 0;
            last = now;
        }
    }
}

#if UVC_AI_YOLO
/* YUYV422 packed: 2px = 4 bytes [Y0 U Y1 V]; U/V shared by the even/odd pair.
 * px (x,y) byte base = ((y*W+x)/2)*4; even x -> Y@+0,U@+1; odd x -> Y@+2,V@+3. */
static inline void yuyv_put(uint8_t *buf, int W, int x, int y, uint8_t Y,
                            uint8_t U, uint8_t V) {
    int px = y * W + x;
    int b = (px >> 1) * 4;
    if (px & 1) {
        buf[b + 2] = Y;
        buf[b + 3] = V;
    } else {
        buf[b + 0] = Y;
        buf[b + 1] = U;
    }
}

/* Hollow rectangle outline (thickness th) into a WxH YUYV422 buffer. */
static void draw_rect_yuyv(uint8_t *buf, int W, int H, int x1, int y1, int x2,
                           int y2, uint8_t Y, uint8_t U, uint8_t V, int th) {
    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 > W - 1) x2 = W - 1;
    if (y2 > H - 1) y2 = H - 1;
    if (x1 >= x2 || y1 >= y2) return;
    for (int t = 0; t < th; t++) {
        int yt = y1 + t, yb = y2 - t;
        if (yt <= y2)
            for (int x = x1; x <= x2; x++) yuyv_put(buf, W, x, yt, Y, U, V);
        if (yb >= y1)
            for (int x = x1; x <= x2; x++) yuyv_put(buf, W, x, yb, Y, U, V);
        int xl = x1 + t, xr = x2 - t;
        if (xl <= x2)
            for (int y = y1; y <= y2; y++) yuyv_put(buf, W, xl, y, Y, U, V);
        if (xr >= x1)
            for (int y = y1; y <= y2; y++) yuyv_put(buf, W, xr, y, Y, U, V);
    }
}

/* class_id -> YUV color. 10 distinct, saturated colors (BT.601 YUV of bright RGB)
 * so every detected digit is clearly visible AND tells digits apart. The old
 * scheme sent class_id >= 3 to faint gray (127,128,128) -> looked like white/faint
 * boxes; since the gesture model fires mostly on one class, all boxes looked alike. */
static void det_color(int class_id, uint8_t *Y, uint8_t *U, uint8_t *V) {
    static const uint8_t pal[10][3] = {
        { 76,  84, 255}, /* 0 red     */
        {149,  43,  21}, /* 1 green   */
        { 29, 255, 107}, /* 2 blue    */
        {226,   0, 149}, /* 3 yellow  */
        {179, 171,   0}, /* 4 cyan    */
        {105, 212, 235}, /* 5 magenta */
        {151,  42, 202}, /* 6 orange  */
        {188,  22,  85}, /* 7 lime    */
        { 67, 234, 171}, /* 8 violet  */
        { 91, 149, 245}, /* 9 pink    */
    };
    int i = (class_id >= 0 && class_id < 10) ? class_id : 0;
    *Y = pal[i][0]; *U = pal[i][1]; *V = pal[i][2];
}

/* Map a detection box coord (256 model space = centered AI_CROP_SIDE square of
 * the 1280x720 sensor) to a VGA pixel. Integer math; numerator before divide. */
static inline void remap_model_to_vga(int mx, int my, int *vx, int *vy) {
#if AI_INPUT_640X384
    /* model space = 640x384 letterbox of the FULL 1280x720 frame: content rows are
     * [AI_LB_Y .. AI_LB_Y+OUT_HEIGHT_1). Full width maps straight to VGA width; the
     * content rows map to the full VGA height (ch0 also fills VGA from the full
     * frame, so the vertical squash is consistent). */
    *vx = mx * VGA_W / OUT_WIDTH_1;                 /* 640 -> 640 */
    *vy = (my - AI_LB_Y) * VGA_H / OUT_HEIGHT_1;    /* (my-12) * 480/360 */
#else
    *vx = (AI_CROP_OFF_X * VGA_W * OUT_WIDTH_1 + mx * AI_CROP_SIDE * VGA_W) /
          (CROP_WIDTH * OUT_WIDTH_1);
    *vy = (AI_CROP_OFF_Y * VGA_H * OUT_HEIGHT_1 + my * AI_CROP_SIDE * VGA_H) /
          (CROP_HEIGHT * OUT_HEIGHT_1);
#endif
    if (*vx < 0) *vx = 0;
    if (*vx > VGA_W - 1) *vx = VGA_W - 1;
    if (*vy < 0) *vy = 0;
    if (*vy > VGA_H - 1) *vy = VGA_H - 1;
}

/* Draw ALL current detections into the just-completed YUYV frame (no 8-box HW
 * cap). Lock-free handshake with get_ai_result_proc via gAiResultUsing. */
static void draw_all_detections(uint8_t *buf, int W, int H) {
    if (gAiResultUsing) return; /* snapshot mid-write; skip this frame */
    gAiResultUsing = 1;
    int n = gAiResult.num_detections;
    if (n > YOLOV8_MAX_DETECTIONS) n = YOLOV8_MAX_DETECTIONS;
    for (int i = 0; i < n; i++) {
        const yolov8_detection_t *d = &gAiResult.detections[i];
        int x1, y1, x2, y2;
        remap_model_to_vga((int)d->box.x1, (int)d->box.y1, &x1, &y1);
        remap_model_to_vga((int)d->box.x2, (int)d->box.y2, &x2, &y2);
        uint8_t Y, U, V;
        det_color(d->class_id, &Y, &U, &V);
        draw_rect_yuyv(buf, W, H, x1, y1, x2, y2, Y, U, V, 2);
    }
    gAiResultUpdate = 0;
    gAiResultUsing = 0;
}

/* Pull each detection result, snapshot it for the encode task, then re-arm ch1
 * so the next frame is captured (one-frame-in-flight throttle). */
static void get_ai_result_proc(void *arg) {
    (void)arg;
    uint32_t result_addr;
    while (1) {
        if (0 != uvc_ai_get_result(&result_addr)) {
            break;
        }
        if (0 == gAiResultUsing) {
            memcpy((void *)&gAiResult, (void *)result_addr,
                   sizeof(yolov8_result_t));
            gAiResultUpdate = 1;
        }
        update_isp_fps(ISP_CHANNEL_1, 30, 30);
    }
    vTaskDelete(NULL);
}
#endif /* UVC_AI_YOLO */

#if defined(CONFIG_IPRO7AI_REEF) && defined(CONFIG_SHELL)
/* ----------------------------------------------------------------------------
 * `cam` - OV5640 tuning on the stream (lens shading calibration):
 *   cam rd <reg> [n]        read n consecutive registers (hex)
 *   cam wr <reg> <hexbytes> write consecutive registers, e.g. cam wr 5800 2314
 *   cam grid [cols rows]    mean Y/U/V per block of the newest ch0 frame
 * --------------------------------------------------------------------------*/
#include "shell.h"

#define OV5640_SCCB_ADDR 0x3C

static int hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static void cam_grid(int cols, int rows) {
    uint16_t w, h;
    uint32_t addr = g_yuv_ready;

    if (!addr || cols < 1 || rows < 1 || cols > 32 || rows > 18) {
        printf("cam grid: no frame\r\n");
        return;
    }
    get_mode_size(gResMode, &w, &h);
    L1C_DCACHE_INVALID_RANGE((unsigned long)addr, (unsigned long)YUV422_FRAME_SIZE(w, h));
    const uint8_t *f = (const uint8_t *)addr;
    printf("GRID %d %d %u %u\r\n", cols, rows, w, h);
    for (int ch = 0; ch < 3; ch++) {
        for (int r = 0; r < rows; r++) {
            printf("%c%02d", "YUV"[ch], r);
            for (int c = 0; c < cols; c++) {
                uint32_t sum = 0, n = 0;
                int x0 = (c * w / cols) & ~1, x1 = ((c + 1) * w / cols) & ~1;
                int y0 = r * h / rows, y1 = (r + 1) * h / rows;
                for (int y = y0; y < y1; y += 4) {
                    const uint8_t *row = f + (size_t)y * w * 2;
                    for (int x = x0; x < x1; x += 4) {
                        const uint8_t *p = row + x * 2;   /* [Y0 U Y1 V] */
                        sum += ch == 0 ? (uint32_t)(p[0] + p[2]) / 2u : ch == 1 ? p[1] : p[3];
                        n++;
                    }
                }
                printf(" %3lu", (unsigned long)(n ? sum / n : 0));
            }
            printf("\r\n");
        }
    }
    printf("GRIDEND\r\n");
}

static int cmd_cam(int argc, char *argv[]) {
    const char *sub = argc > 1 ? argv[1] : "";

    if (!strcmp(sub, "rd") && argc > 2) {
        uint16_t reg = (uint16_t)strtoul(argv[2], NULL, 16);
        int n = argc > 3 ? atoi(argv[3]) : 1;
        printf("RD %04x", reg);
        for (int i = 0; i < n && i < 128; i++) {
            uint8_t v = 0;
            SCCB_Read_Reg16(OV5640_SCCB_ADDR, (uint16_t)(reg + i), &v, 1);
            printf(" %02x", v);
        }
        printf("\r\n");
    } else if (!strcmp(sub, "wr") && argc > 3) {
        uint16_t reg = (uint16_t)strtoul(argv[2], NULL, 16);
        const char *s = argv[3];
        int n = 0;
        for (; hexval(s[0]) >= 0 && hexval(s[1]) >= 0; s += 2, n++) {
            uint8_t v = (uint8_t)(hexval(s[0]) << 4 | hexval(s[1]));
            SCCB_Write_Reg16(OV5640_SCCB_ADDR, (uint16_t)(reg + n), &v);
        }
        printf("WR %04x %d\r\n", reg, n);
    } else if (!strcmp(sub, "grid")) {
        cam_grid(argc > 2 ? atoi(argv[2]) : 8, argc > 3 ? atoi(argv[3]) : 6);
    } else {
        printf("usage: cam rd <reg> [n] | cam wr <reg> <hexbytes> | cam grid [cols rows]\r\n");
        return -1;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cam, cam, OV5640 tuning: cam rd|wr|grid);
#endif

// ============================================================================
// public function
// ============================================================================

void uvc_isp_set_param(isp_res_mode_e res_mode, bool is_color) {
    if (res_mode < ISP_RES_MODE_MAX)
        gResMode = res_mode;

    gIsColor = is_color;
}

void uvc_isp_init(void) {
    /* enable MM clock */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

    /* set I2C divider */
    GLB_Set_I2C_CLK(ENABLE, GLB_I2C_CLK_SRC_BCLK, 0);

    if (!gJencNotify)
        gJencNotify = xSemaphoreCreateCounting(1, 0);

    if (!gIsTaskRunning) {
        xTaskCreate(uvc_get_jpg_proc, "uvc_jpg_proc", UVC_JPG_TASK_STACK, NULL, 10, NULL);
        gIsTaskRunning = true;
    }

#ifdef CONFIG_UVC_RPS
    /* Model first, for the same reason as below; ~0.3 s for the 299 KB RPS
     * model. On failure the stream runs and the box says why. */
    if (uvc_rps_init() == 0) {
        gRpsReady = true;
        rps_model_set_done_cb(rps_npu_done, NULL);
    }
#endif

#if UVC_AI_YOLO
    /* Load the model (from XIP, no PSRAM copy) FIRST, then let isp_pipe_init
     * allocate jenc + ch1 from what remains. A large model's resident activation
     * buffer (model->buffer = patch_size*patch_count, e.g. ~4MB for BIG-A) must
     * be allocated while the heap is still mostly free; pre-allocating the stream
     * buffers first would starve it and the model would fail to load. If the
     * model or a stream buffer can't fit, gAiReady stays false / ch1 is dropped
     * and the device streams as a plain webcam. */
    if (uvc_ai_init() == 0) {
        gAiReady = true;
        /* Drawing happens in the encode task (uvc_get_jpg_proc); this task only
         * snapshots results + re-arms ch1. */
        xTaskCreate(get_ai_result_proc, "uvc_ai_res", 512, NULL, 10, NULL);
    }
#endif

    /* run isp pipe init: sensor + isp + jpg, then stream continuously */
    isp_pipe_init();
}
