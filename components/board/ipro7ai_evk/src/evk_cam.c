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
 * @file evk_cam.c
 * @brief SC101IOT DVP camera -> ISP (ch0 preview, ch1 NPU). See evk_cam.h.
 *
 * Pin bring-up, probe, and the start/stop sequence are lifted from
 * apps/platform/ipro7ai_evk_demo/src/cam.c, which was debugged on this exact
 * board. What is new here is the second output channel and the gating that
 * hands its frames to the NPU consumer one at a time.
 */

#include "evk_cam.h"
#include "evk_cam_port.h"
#include "st7789p3.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "camera.h"        /* camera_module_init / camera_module_start */
#include "sensor.h"        /* rt_camera_desc                           */
#include "hal_i2c.h"
#include "hal_isp.h"
#include "img_proc.h"      /* imgproc_* + ipro_isp_stats_ready_cb      */
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_mm_misc.h"
#include "hal_cache_alloc.h"  /* pvPortMallocCacheAligned */
#include <system_soc.h>    /* L1C_DCACHE_* */
#include "ipro_log.h"

#define LOG_TAG "EVK_CAM"

#define YUYV_FRAME_SIZE(w, h)  ((uint32_t)(w) * (h) * 2u)
#define ARGB_FRAME_SIZE(w, h)  ((uint32_t)(w) * (h) * 4u)

/* SEOF line-count trigger, matching the SDK's camera apps. */
#define EVK_CAM_SEOF_LINE_CNT   100

/* ISP frames to let AE/AWB settle before anything is shown or detected. Below
 * this the image is a black or blown-out mess and looks like a wiring fault. */
#define EVK_CAM_CONVERGE_FRAMES 6

/* ---------------------------------------------------------------------------
 * State
 * -------------------------------------------------------------------------*/

static const rt_camera_desc *s_desc;
static volatile bool s_running;
static bool     s_isp_up;

static uint32_t s_yuyv[2];              /* ch0 ping-pong frames in PSRAM */
static uint32_t s_yuyv_bytes;
static uint8_t  s_pp_num;

static evk_cam_frame_cb_t s_frame_cb;
static void          *s_frame_arg;

static int      s_fit = EVK_CAM_FIT_LETTERBOX;
static uint8_t  s_rot = ST_DEFAULT_DIR;
/* Preview rate. A 320x180 RGB565 frame is 58 ms on the wire at the panel's
 * proven 16 MHz SPI clock, and the SPI driver polls, so 10 fps leaves ~40% of
 * the CPU for the NPU dispatch, box decode and the shell. `lcd clk 32000000`
 * then `cam fps 15` if the wiring takes the faster clock. */
static int      s_fps = 10;
static evk_cam_geometry_t s_geo;

static volatile uint32_t s_converge;
static volatile bool     s_ch1_on;       /* channel 1 configured this session */
static volatile bool     s_npu_en;       /* feeding the NPU is wanted        */
static evk_cam_npu_input_t   s_npu_in;       /* channel-1 geometry + buffer      */
static evk_cam_npu_frame_cb_t s_npu_cb;
static void             *s_npu_arg;
static evk_cam_stats_t s_stats;

/* The ISP firmware registers its AE/AWB statistics handler here and expects
 * the application to call it once per start-of-frame. Without this hook 3A
 * never runs and every frame comes out at the sensor's power-on exposure. */
static ipro_isp_stats_ready_cb s_stats_cb;

int ipro_isp_stats_ready_config(ipro_isp_stats_ready_cb cb, void *usrdata)
{
    (void)usrdata;
    s_stats_cb = cb;
    return 0;
}

/* ---------------------------------------------------------------------------
 * Pin bring-up
 * -------------------------------------------------------------------------*/

static void evk_cam_i2c_pins(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive    = 0,
        .smtCtrl  = 1,
        .gpioFun  = GPIO_FUN_I2C,        /* I2C0; silicon fixes SCL/SDA by parity */
    };

    cfg.gpioPin = EVK_CAM_PIN_SCL;
    GLB_GPIO_Init(&cfg);
    cfg.gpioPin = EVK_CAM_PIN_SDA;
    GLB_GPIO_Init(&cfg);

    i2c_init(I2C0_INDEX);
}

static void evk_cam_dvp_pins(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_NONE,
        .drive    = 0,
        .smtCtrl  = 1,
        .gpioFun  = GPIO_FUN_CAM,
    };
    const uint8_t pins[] = {
        EVK_CAM_PIN_D0 + 0, EVK_CAM_PIN_D0 + 1, EVK_CAM_PIN_D0 + 2, EVK_CAM_PIN_D0 + 3,
        EVK_CAM_PIN_D0 + 4, EVK_CAM_PIN_D0 + 5, EVK_CAM_PIN_D0 + 6, EVK_CAM_PIN_D0 + 7,
        EVK_CAM_PIN_PCLK, EVK_CAM_PIN_VSYNC, EVK_CAM_PIN_HSYNC,
    };

    for (unsigned i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        cfg.gpioPin = pins[i];
        GLB_GPIO_Init(&cfg);
    }
}

static void evk_cam_power_up(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_NONE,
        .drive    = 3,
        .smtCtrl  = 1,
        .gpioPin  = EVK_CAM_PIN_MCLK,
        .gpioFun  = GPIO_FUN_CAM_MISC,
    };

    /* MCLK on GPIO41: PLL48M / 2 = 24 MHz. */
    GLB_GPIO_Init(&cfg);
    GLB_Set_CAM_CLK(DISABLE, GLB_CAM_CLK_PLL48M, 1);

    /* RESET on GPIO35, driven low first. There is no PWDN pin on this board -
     * GPIO34 is SD_DET, so it must not be touched here. */
    cfg.gpioMode = GPIO_MODE_OUTPUT;
    cfg.pullType = GPIO_PULL_UP;
    cfg.gpioFun  = GPIO_FUN_GPIO;
    cfg.gpioPin  = EVK_CAM_PIN_RST;
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_Write(EVK_CAM_PIN_RST, 0);
    GLB_GPIO_OUTPUT_Enable(EVK_CAM_PIN_RST);

    GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_PLL48M, 1);
    vTaskDelay(pdMS_TO_TICKS(2));

    GLB_GPIO_Write(EVK_CAM_PIN_RST, 1);      /* release reset (level-shifted) */
    vTaskDelay(pdMS_TO_TICKS(20));       /* let MCLK + internal PLL settle */
}

/*
 * Put the MM blocks we drive back into a known state.
 *
 * MUST be called with the sensor held in reset. Resetting DVP_AS while the
 * sensor is streaming into it wedges the capture side mid-frame - that is the
 * exact failure this reset exists to prevent, and doing it at the wrong moment
 * reproduces it faithfully.
 */
static void evk_cam_mm_reset(void)
{
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP_AS, 1);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP_AS, 0);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP2AXI_C, 1);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP2AXI_C, 0);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_ISP, 1);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_ISP, 0);
}

void evk_cam_reset_pin(int level)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_OUTPUT,
        .pullType = GPIO_PULL_UP,
        .drive    = 0,
        .smtCtrl  = 1,
        .gpioPin  = EVK_CAM_PIN_RST,
        .gpioFun  = GPIO_FUN_GPIO,
    };

    GLB_GPIO_Init(&cfg);
    GLB_GPIO_Write(EVK_CAM_PIN_RST, (uint32_t)(level ? 1 : 0));
    GLB_GPIO_OUTPUT_Enable(EVK_CAM_PIN_RST);
}

/* ---------------------------------------------------------------------------
 * Probe
 * -------------------------------------------------------------------------*/

int evk_cam_probe(void)
{
    evk_cam_i2c_pins();
    evk_cam_power_up();
    evk_cam_dvp_pins();

    /* camera_module_init() walks the .camera_desc linker section, scans SCCB
     * for each registered sensor's address and resets the one that answers.
     * A cold sensor can need a few tries while its internal PLL locks. */
    for (int retry = 0; retry < 5; retry++) {
        s_desc = camera_module_init();
        if (s_desc) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    if (!s_desc) {
        IPRO_LOGE(LOG_TAG, "no sensor answered on SCCB (SCL=GPIO%d SDA=GPIO%d)",
                  EVK_CAM_PIN_SCL, EVK_CAM_PIN_SDA);
        return EVK_CAM_ERR_NODEV;
    }

    IPRO_LOGI(LOG_TAG, "sensor %s @0x%02X, %ux%u, pclk %u Hz, total %ux%u",
              s_desc->name, s_desc->addr,
              (unsigned)s_desc->width, (unsigned)s_desc->height,
              (unsigned)s_desc->pclk,
              (unsigned)s_desc->total_width, (unsigned)s_desc->total_height);
    return EVK_CAM_OK;
}

const char *evk_cam_sensor_name(void) { return s_desc ? s_desc->name : NULL; }
uint8_t     evk_cam_sensor_addr(void) { return s_desc ? s_desc->addr : 0u; }
bool        evk_cam_is_running(void)  { return s_running; }
int         evk_cam_get_fit(void)     { return s_fit; }
int         evk_cam_get_rot(void)     { return s_rot; }
int         evk_cam_get_fps(void)     { return s_fps; }
bool        evk_cam_npu_enabled(void) { return s_npu_en; }

void evk_cam_set_npu_input(const evk_cam_npu_input_t *in)
{
    if (in) {
        s_npu_in = *in;
    } else {
        memset(&s_npu_in, 0, sizeof(s_npu_in));
    }
}

void evk_cam_set_npu_frame_cb(evk_cam_npu_frame_cb_t cb, void *arg)
{
    s_npu_cb  = cb;
    s_npu_arg = arg;
}

void evk_cam_sensor_size(uint16_t *w, uint16_t *h)
{
    if (w) { *w = s_desc ? s_desc->width  : EVK_CAM_SENSOR_W; }
    if (h) { *h = s_desc ? s_desc->height : EVK_CAM_SENSOR_H; }
}

void evk_cam_set_frame_cb(evk_cam_frame_cb_t cb, void *arg)
{
    s_frame_cb  = cb;
    s_frame_arg = arg;
}

void evk_cam_get_geometry(evk_cam_geometry_t *out)
{
    if (out) {
        *out = s_geo;
    }
}

void evk_cam_get_stats(evk_cam_stats_t *out)
{
    if (out) {
        *out = s_stats;
    }
}

int evk_cam_set_rot(int rot)
{
    if (rot < 0 || rot > 3) {
        return EVK_CAM_ERR_PARAM;
    }
    s_rot = (uint8_t)rot;
    return EVK_CAM_OK;
}

int evk_cam_set_fit(int fit)
{
    if (fit != EVK_CAM_FIT_LETTERBOX && fit != EVK_CAM_FIT_CROP) {
        return EVK_CAM_ERR_PARAM;
    }
    s_fit = fit;
    return EVK_CAM_OK;
}

static void set_fps(uint8_t channel, uint8_t dst)
{
    isp_hal_frame_rate_t frc = { .src_fps = 30, .dst_fps = dst };
    isp_fps_cfg(channel, frc);
}

int evk_cam_set_fps(int fps)
{
    if (fps < 1 || fps > 30) {
        return EVK_CAM_ERR_PARAM;
    }
    s_fps = fps;
    if (s_isp_up && s_running) {
        set_fps(ISP_CHANNEL_0, (uint8_t)fps);
    }
    return EVK_CAM_OK;
}

/* ---------------------------------------------------------------------------
 * Channel-1 gating
 * -------------------------------------------------------------------------*/

void evk_cam_npu_rearm(void)
{
    if (s_npu_en && s_isp_up && s_running && s_ch1_on && s_converge == 0) {
        set_fps(ISP_CHANNEL_1, 30);
    }
}

void evk_cam_npu_set_enabled(bool on)
{
    s_npu_en = on;
    if (on) {
        /* If an inference is still running the frame this lets through is
         * refused by the consumer, the callback gates channel 1 again, and
         * the consumer's own done path re-opens it. */
        evk_cam_npu_rearm();
    } else if (s_isp_up && s_ch1_on) {
        set_fps(ISP_CHANNEL_1, 0);
    }
}

/* ---------------------------------------------------------------------------
 * ISP callbacks
 * -------------------------------------------------------------------------*/

static void evk_cam_isp_cb(isp_out_frame_info_t info)
{
    switch (info.event) {
        case ISP_HAL_INT_FIFO_OVERWRITE:  s_stats.fifo_err++;  return;
        case ISP_HAL_INT_VSYNC_CNT_ERROR: s_stats.vsync_err++; return;
        case ISP_HAL_INT_HSYNC_CNT_ERROR: s_stats.hsync_err++; return;
        case ISP_HAL_INT_NORMAL:          break;
        default:                          return;
    }

    if (info.out_port == ISP_CHANNEL_0) {
        s_stats.frames0++;
        if (s_converge) {
            if (--s_converge == 0 && s_ch1_on && s_npu_en) {
                /* Exposure has settled: let the first frame through to the
                 * NPU. From here channel 1 is re-armed only by the consumer. */
                set_fps(ISP_CHANNEL_1, 30);
            }
            return;
        }
        if (s_frame_cb) {
            s_frame_cb(info.out_addr, s_frame_arg);
        }
        return;
    }

    if (info.out_port == ISP_CHANNEL_1) {
        s_stats.frames1++;
        /* One frame in flight: stop channel 1 before anything else, then hand
         * the frame over. evk_cam_npu_rearm() (from the consumer, when it is
         * done) opens it again. */
        set_fps(ISP_CHANNEL_1, 0);
        if (s_npu_en && s_npu_cb && s_npu_cb(info.out_addr, s_npu_arg) == 0) {
            s_stats.pushed++;
        }
    }
}

static void evk_cam_seof_isr(void)
{
    MM_MISC_SEOF_TYPE_E status;

    MM_MISC_Get_SEOF_TypeStatus(MM_MISC_SEOF0, &status);
    if (status.BF.SoF && s_stats_cb) {
        s_stats_cb(NULL);     /* drives the ISP firmware's AE/AWB statistics */
    }
    MM_MISC_Clr_SEOF_TypeStatus(MM_MISC_SEOF0, status);
}

/* ---------------------------------------------------------------------------
 * Pipeline
 * -------------------------------------------------------------------------*/

/* Work out the preview geometry for the current fit mode. The sensor is 16:9
 * and the panel in landscape is 4:3, so something has to give. */
static void evk_cam_calc_geometry(uint16_t panel_w, uint16_t panel_h)
{
    uint16_t sw, sh;

    evk_cam_sensor_size(&sw, &sh);
    s_geo.panel_w = panel_w;
    s_geo.panel_h = panel_h;

    if (s_fit == EVK_CAM_FIT_CROP) {
        /* Centre-crop to 4:3 (960x720 of 1280x720), then a clean 3x downscale
         * fills the whole 320x240 panel with no distortion. */
        uint16_t cw = (uint16_t)((uint32_t)sh * panel_w / panel_h);
        if (cw > sw) {
            cw = sw;
        }
        s_geo.crop_x = (uint16_t)((sw - cw) / 2u);
        s_geo.crop_y = 0;
        s_geo.crop_w = cw;
        s_geo.crop_h = sh;
        s_geo.img_w  = panel_w;
        s_geo.img_h  = panel_h;
    } else {
        /* Whole field of view, letterboxed: 1280x720 -> 320x180 (a clean 4x),
         * centred with 30 black rows above and below. */
        s_geo.crop_x = 0;
        s_geo.crop_y = 0;
        s_geo.crop_w = sw;
        s_geo.crop_h = sh;
        s_geo.img_w  = panel_w;
        s_geo.img_h  = (uint16_t)((uint32_t)sh * panel_w / sw);
        if (s_geo.img_h > panel_h) {
            s_geo.img_h = panel_h;
        }
    }
    /* YUYV packs two pixels per 32-bit word; keep the row even. */
    s_geo.img_w &= (uint16_t)~1u;

    s_geo.ox = (uint16_t)((panel_w - s_geo.img_w) / 2u);
    s_geo.oy = (uint16_t)((panel_h - s_geo.img_h) / 2u);

    s_geo.roi_x = s_npu_in.roi_x;
    s_geo.roi_y = s_npu_in.roi_y;
    s_geo.roi_w = s_npu_in.roi_w;
    s_geo.roi_h = s_npu_in.roi_h;
}

static int evk_cam_alloc_preview(void)
{
    uint32_t need = YUYV_FRAME_SIZE(s_geo.img_w, s_geo.img_h);

    if (s_yuyv[0] && s_yuyv_bytes < need) {
        /* Geometry grew (letterbox -> crop) since the buffers were carved. */
        for (int i = 0; i < 2; i++) {
            if (s_yuyv[i]) {
                vPortFreeCacheAligned((void *)(uintptr_t)s_yuyv[i]);
                s_yuyv[i] = 0;
            }
        }
    }
    s_yuyv_bytes = s_yuyv[0] ? s_yuyv_bytes : need;

    /* Cache-line aligned: the display invalidates these ranges before every
     * read, and pvPortMalloc's 8-byte alignment would let that invalidate
     * take out the heap header sharing the first line. */
    for (int i = 0; i < 2; i++) {
        if (!s_yuyv[i]) {
            s_yuyv[i] = (uint32_t)(uintptr_t)pvPortMallocCacheAligned(s_yuyv_bytes);
        }
    }
    if (!s_yuyv[0]) {
        IPRO_LOGE(LOG_TAG, "PSRAM alloc %u B for the preview frame failed",
                  (unsigned)s_yuyv_bytes);
        return EVK_CAM_ERR_NOMEM;
    }
    /* Ping-pong if both fit; a single buffer still works (the panel may tear
     * when the ISP overtakes the blit) and is better than no preview. */
    s_pp_num = s_yuyv[1] ? 2 : 1;
    if (s_pp_num == 1) {
        IPRO_LOGW(LOG_TAG, "only one preview buffer fits - expect tearing");
    }
    return EVK_CAM_OK;
}

static int evk_cam_isp_up(void)
{
    isp_hal_cfg_t     cfg = {0};
    isp_hal_vin_cfg_t vin = {0};
    uint16_t          sw, sh;
    uint32_t          npu_addr = s_npu_in.addr;
    int               rc;

    /* Un-gate the multimedia AHB clock BEFORE touching any ISP / DVP2AXI /
     * MM_MISC register. Without it every write into that domain is dropped:
     * the sensor still probes fine (SCCB is I2C0, a different domain) and
     * isp_init() returns success, but nothing ever streams and no completion
     * interrupt fires - the counters sit at 0 frames with 0 errors. */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

    evk_cam_sensor_size(&sw, &sh);
    evk_cam_calc_geometry(st7789p3_width(), st7789p3_height());

    rc = evk_cam_alloc_preview();
    if (rc != EVK_CAM_OK) {
        return rc;
    }

    cfg.isp_src                     = ISP_HAL_SRC_DVP_AS;
    cfg.in_size.width               = sw;
    cfg.in_size.height              = sh;
    cfg.src_cfg.dvp_input.hsync     = ISP_HAL_DVP_ACTIVE_HIGH;
    cfg.src_cfg.dvp_input.vsync     = ISP_HAL_DVP_ACTIVE_HIGH;
    cfg.src_cfg.dvp_input.order     = ISP_HAL_DATA_BYTE_LOWER;
    cfg.src_cfg.dvp_input.pclk      = s_desc->pclk;
    cfg.src_cfg.dvp_input.totalHPixels = s_desc->total_width;
    cfg.glb_crop.start_x            = 0;
    cfg.glb_crop.start_y            = 0;
    cfg.glb_crop.width              = sw;
    cfg.glb_crop.height             = sh;
    cfg.yuv_adj_en                  = 0;

    /* Channel 0: the preview. Full-frame YUYV into PSRAM, no ring (that is
     * the MJPEG handshake mode), ping-pong so the display can read one frame
     * while the ISP writes the other. */
    cfg.out_ch_0.enable                     = 1;
    cfg.out_ch_0.scaler_cfg.in.start_x      = s_geo.crop_x;
    cfg.out_ch_0.scaler_cfg.in.start_y      = s_geo.crop_y;
    cfg.out_ch_0.scaler_cfg.in.width        = s_geo.crop_w;
    cfg.out_ch_0.scaler_cfg.in.height       = s_geo.crop_h;
    cfg.out_ch_0.scaler_cfg.out.width       = s_geo.img_w;
    cfg.out_ch_0.scaler_cfg.out.height      = s_geo.img_h;
    cfg.out_ch_0.format                     = ISP_HAL_OUT_FMT0_YUYV;
    cfg.out_ch_0.ring_line                  = 0;
    cfg.out_ch_0.chn_common.fr_ctrl.src_fps = 30;
    cfg.out_ch_0.chn_common.fr_ctrl.dst_fps = (uint8_t)s_fps;
    cfg.out_ch_0.chn_common.addr[0]         = s_yuyv[0];
    cfg.out_ch_0.chn_common.addr[1]         = s_yuyv[1] ? s_yuyv[1] : s_yuyv[0];
    cfg.out_ch_0.chn_common.pp_num          = s_pp_num;
    cfg.out_ch_0.chn_common.out_crop.start_x = 0;
    cfg.out_ch_0.chn_common.out_crop.start_y = 0;
    cfg.out_ch_0.chn_common.out_crop.width   = s_geo.img_w;
    cfg.out_ch_0.chn_common.out_crop.height  = s_geo.img_h;

    /* Channel 1: the NPU feed. The configured sensor ROI scaled to the model
     * input size, ARGB, into the consumer's single buffer. Starts gated
     * (dst_fps 0) and is opened once AE has converged; after that one frame
     * at a time. */
    s_ch1_on = (npu_addr != 0 && s_npu_in.out_w && s_npu_in.out_h);
    if (s_ch1_on) {
        cfg.out_ch_1.enable                     = 1;
        cfg.out_ch_1.scaler_cfg.in.start_x      = s_npu_in.roi_x;
        cfg.out_ch_1.scaler_cfg.in.start_y      = s_npu_in.roi_y;
        cfg.out_ch_1.scaler_cfg.in.width        = s_npu_in.roi_w;
        cfg.out_ch_1.scaler_cfg.in.height       = s_npu_in.roi_h;
        cfg.out_ch_1.scaler_cfg.out.width       = s_npu_in.out_w;
        cfg.out_ch_1.scaler_cfg.out.height      = s_npu_in.out_h;
        cfg.out_ch_1.format                     = ISP_HAL_OUT_FMT1_ARGB;
        cfg.out_ch_1.chn_common.fr_ctrl.src_fps = 30;
        cfg.out_ch_1.chn_common.fr_ctrl.dst_fps = 0;
        cfg.out_ch_1.chn_common.addr[0]         = npu_addr;
        cfg.out_ch_1.chn_common.addr[1]         = npu_addr;
        cfg.out_ch_1.chn_common.pp_num          = 1;
        cfg.out_ch_1.chn_common.out_crop.start_x = 0;
        cfg.out_ch_1.chn_common.out_crop.start_y = 0;
        cfg.out_ch_1.chn_common.out_crop.width   = s_npu_in.out_w;
        cfg.out_ch_1.chn_common.out_crop.height  = s_npu_in.out_h;
    }

    cfg.out_callback = evk_cam_isp_cb;

    isp_init(&cfg);

    for (int i = 0; i < s_pp_num; i++) {
        L1C_DCACHE_CLEAN_INVALID_RANGE(s_yuyv[i], s_yuyv_bytes);
    }
    if (s_ch1_on) {
        L1C_DCACHE_CLEAN_INVALID_RANGE(npu_addr,
                                       ARGB_FRAME_SIZE(s_npu_in.out_w, s_npu_in.out_h));
    }

    /* ISP firmware 3A. imgproc registers its statistics handler through
     * ipro_isp_stats_ready_config() above; the SEOF ISR then drives it. */
    vin.in_size.width   = cfg.in_size.width;
    vin.in_size.height  = cfg.in_size.height;
    vin.glb_crop.width  = cfg.glb_crop.width;
    vin.glb_crop.height = cfg.glb_crop.height;
    memcpy(&vin.sensor_name, EVK_CAM_SENSOR_NAME "\0", sizeof(EVK_CAM_SENSOR_NAME) + 1);
    if (imgproc_init(&vin) != 0) {
        /* Non-fatal: the pipeline still streams, but AE/AWB never converge so
         * the picture stays at the sensor's power-on exposure. */
        IPRO_LOGE(LOG_TAG, "imgproc_init failed: 3A will not run, expect a "
                           "fixed-exposure image");
    }
    /* Re-arm the fast AE/AWB convergence. fast_flag is a static that latches
     * at 1 after the first convergence, so without this a restarted stream
     * keeps the previous scene's gains. */
    imgproc_reset_fast_flag();
    imgproc_start_fast_isp(s_desc);

    /* Start-of-frame interrupt: without it 3A never ticks. */
    {
        MM_MISC_SEOF_TYPE_E seof = {0};
        seof.BF.SoF     = 1;
        seof.BF.EoF     = 1;
        seof.BF.lineCnt = 1;
        MM_MISC_Set_SEOF_LineCnt(MM_MISC_SEOF0, EVK_CAM_SEOF_LINE_CNT);
        MM_MISC_Set_SEOF_Src(MM_MISC_SEOF0, MM_MISC_SEOF_INTR_SRC_ISP_OUTPUT);
        MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF0, seof);
        Interrupt_Handler_Register(ISP_MM_SEOF0_IRQn, evk_cam_seof_isr);
        CPU_Interrupt_Enable(ISP_MM_SEOF0_IRQn);
    }

    /* Both channels write through dvp2axi; its bus masters have to be on. */
    MM_MISC_Set_IPROAIPRO_Rcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Rcmd_MasterEn(0, 0, 1);
    MM_MISC_Set_IPROAIPRO_Wcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Wcmd_MasterEn(0, 0, 1);

    isp_start();
    s_isp_up = true;
    return EVK_CAM_OK;
}

int evk_cam_start(void)
{
    int rc;

    if (s_running) {
        return EVK_CAM_OK;
    }
    if (!st7789p3_is_ready()) {
        IPRO_LOGE(LOG_TAG, "panel not initialised");
        return EVK_CAM_ERR_STATE;
    }

    /*
     * Bring the MM blocks up in a known state before anything streams into
     * them. The sensor is held in reset for the duration: resetting DVP_AS
     * with data flowing is what wedges the capture side, so the order here is
     * load-bearing, not tidiness. Doing it on EVERY start is what makes the
     * first start behave like the second (colour matrix state included).
     */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
    evk_cam_reset_pin(0);
    evk_cam_mm_reset();

    /* Re-probe on every start: evk_cam_probe() re-pulses reset and re-writes the
     * sensor registers, so the DVP stream begins at a clean frame boundary.
     * Re-enabling DVP_AS part-way through a frame leaves the capture side
     * waiting for a boundary that never comes. */
    rc = evk_cam_probe();
    if (rc != EVK_CAM_OK) {
        return rc;
    }

    st7789p3_set_dir(s_rot, 0);
    st7789p3_fill_screen(ST_BLACK);

    camera_module_start(s_desc);

    memset(&s_stats, 0, sizeof(s_stats));
    s_converge = EVK_CAM_CONVERGE_FRAMES;
    s_npu_en   = (s_npu_in.addr != 0);

    rc = evk_cam_isp_up();
    if (rc != EVK_CAM_OK) {
        return rc;
    }
    s_running = true;

    IPRO_LOGI(LOG_TAG, "preview %ux%u at (%u,%u), %s, %d fps, %s buffer(s)",
              (unsigned)s_geo.img_w, (unsigned)s_geo.img_h,
              (unsigned)s_geo.ox, (unsigned)s_geo.oy,
              (s_fit == EVK_CAM_FIT_CROP) ? "centre-crop" : "letterbox", s_fps,
              (s_pp_num == 2) ? "two" : "one");
    if (s_ch1_on) {
        IPRO_LOGI(LOG_TAG, "NPU feed: sensor %ux%u+%u+%u -> %ux%u ARGB, one frame "
                           "in flight",
                  (unsigned)s_npu_in.roi_w, (unsigned)s_npu_in.roi_h,
                  (unsigned)s_npu_in.roi_x, (unsigned)s_npu_in.roi_y,
                  (unsigned)s_npu_in.out_w, (unsigned)s_npu_in.out_h);
    } else {
        IPRO_LOGI(LOG_TAG, "NPU feed off (no model)");
    }
    return EVK_CAM_OK;
}

void evk_cam_stop(void)
{
    /*
     * Park the sensor even when nothing was running. Its RESETB defaults to
     * released - R56 pulls it up to DOVDD1V8 the moment GPIO35 goes high-Z -
     * so a sensor that was left configured keeps streaming, and it draws that
     * current from the same 1V8 LDO that feeds the SoC's VDDIO_1 bank. Leaving
     * it streaming across a reset is what stops the board booting with the
     * camera attached.
     */
    evk_cam_reset_pin(0);

    if (!s_running) {
        return;
    }
    s_running = false;
    s_npu_en  = false;

    if (s_isp_up) {
        MM_MISC_SEOF_TYPE_E seof_off = {0};

        /*
         * Teardown order matters:
         *   1. sensor stopped first (already in reset above) - tearing the
         *      ISP down with DVP data still arriving wedges it mid-frame;
         *   2. SEOF silenced before its handler goes away;
         *   3. pipeline down;
         *   4. software-reset the blocks we drive while MM is still clocked.
         * GLB_AHB_CLOCK_MM stays un-gated: MM-domain state does not survive
         * the clock going away and isp_init() does not put it back.
         *
         * imgproc_stop() is deliberately NOT called: it is a one-way door
         * (img_proc.status never returns to IMGPROC_INIT), after which the
         * next start's 3A silently does nothing.
         */
        MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF0, seof_off);
        CPU_Interrupt_Disable(ISP_MM_SEOF0_IRQn);
        Interrupt_Handler_Register(ISP_MM_SEOF0_IRQn, NULL);

        isp_deinit();
        evk_cam_mm_reset();          /* sensor is held in reset above */

        s_isp_up = false;
        s_ch1_on = false;
    }
    IPRO_LOGI(LOG_TAG, "stopped (%u preview frames, %u to the NPU)",
              (unsigned)s_stats.frames0, (unsigned)s_stats.pushed);
}
