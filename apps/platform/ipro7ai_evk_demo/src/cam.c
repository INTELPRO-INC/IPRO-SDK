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
 * @file cam.c
 * @brief SC101IOT DVP camera -> ST7789P3 LCD preview. See cam.h.
 */

#include "cam.h"
#include "cam_port.h"
#include "st7789p3.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "camera.h"        /* camera_module_init / camera_module_start */
#include "sensor.h"        /* rt_camera_desc                           */
#include "sccb.h"
#include "hal_i2c.h"
#include "hal_isp.h"
#include "img_proc.h"      /* imgproc_* + ipro_isp_stats_ready_cb      */
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_mm_misc.h"
#include <system_soc.h>    /* L1C_DCACHE_INVALID_RANGE */
#include "ipro_log.h"

#define LOG_TAG "CAM"

#define ARGB_FRAME_SIZE(w, h)  ((uint32_t)(w) * (h) * 4u)

/* SEOF line-count trigger, matching the SDK's camera apps. */
#define CAM_SEOF_LINE_CNT   100

/* ISP frames to let AE/AWB settle before the preview starts drawing. Below
 * this the image is a black or blown-out mess and looks like a wiring fault. */
#define CAM_CONVERGE_FRAMES 6

#define CAM_PREVIEW_STACK   1024
#define CAM_PREVIEW_PRIO    (configMAX_PRIORITIES - 3)

/* ---------------------------------------------------------------------------
 * State
 * -------------------------------------------------------------------------*/

static const rt_camera_desc *s_desc;
static bool     s_probed;
static volatile bool s_running;
static bool     s_isp_up;

static uint32_t s_argb[2];              /* ch1 ping-pong frames in PSRAM */
static uint32_t s_argb_bytes;
static volatile uint32_t s_latest;      /* newest completed frame address */
static SemaphoreHandle_t s_frame_sem;
static TaskHandle_t      s_task;

static int      s_fit   = CAM_FIT_LETTERBOX;
/* Panel orientation the preview forces at cam_start(). 1 and 3 are the two
 * landscape orientations (180 apart), 0 and 2 the two portrait ones. Which
 * one reads as upright depends on how the panel is mounted, so make it a
 * runtime choice instead of baking one in. */
static uint8_t  s_rot   = ST_DEFAULT_DIR;
static int      s_order = CAM_ORDER_RGB;
static int      s_fps = 10;
static uint16_t s_pw = 320, s_ph = 180; /* preview image size */
static uint16_t s_ox, s_oy;             /* top-left on a 320x240 panel */

static volatile uint32_t s_converge;
static cam_stats_t s_stats;

/* Row staging for the ARGB -> RGB565 repack (wire byte order). */
static uint16_t s_row[ST_MAX_DIM];

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

static void cam_i2c_pins(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive    = 0,
        .smtCtrl  = 1,
        .gpioFun  = GPIO_FUN_I2C,        /* I2C0; silicon fixes SCL/SDA by parity */
    };

    cfg.gpioPin = CAM_PIN_SCL;
    GLB_GPIO_Init(&cfg);
    cfg.gpioPin = CAM_PIN_SDA;
    GLB_GPIO_Init(&cfg);

    i2c_init(I2C0_INDEX);
}

static void cam_dvp_pins(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_NONE,
        .drive    = 0,
        .smtCtrl  = 1,
        .gpioFun  = GPIO_FUN_CAM,
    };
    const uint8_t pins[] = {
        CAM_PIN_D0 + 0, CAM_PIN_D0 + 1, CAM_PIN_D0 + 2, CAM_PIN_D0 + 3,
        CAM_PIN_D0 + 4, CAM_PIN_D0 + 5, CAM_PIN_D0 + 6, CAM_PIN_D0 + 7,
        CAM_PIN_PCLK, CAM_PIN_VSYNC, CAM_PIN_HSYNC,
    };

    for (unsigned i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        cfg.gpioPin = pins[i];
        GLB_GPIO_Init(&cfg);
    }
}

static void cam_power_up(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_NONE,
        .drive    = 3,
        .smtCtrl  = 1,
        .gpioPin  = CAM_PIN_MCLK,
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
    cfg.gpioPin  = CAM_PIN_RST;
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_Write(CAM_PIN_RST, 0);
    GLB_GPIO_OUTPUT_Enable(CAM_PIN_RST);

    GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_PLL48M, 1);
    vTaskDelay(pdMS_TO_TICKS(2));

    GLB_GPIO_Write(CAM_PIN_RST, 1);      /* release reset (level-shifted) */
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
static void cam_mm_reset(void)
{
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP_AS, 1);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP_AS, 0);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP2AXI_C, 1);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP2AXI_C, 0);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_ISP, 1);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_ISP, 0);
}

void cam_reset_pin(int level)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_OUTPUT,
        .pullType = GPIO_PULL_UP,
        .drive    = 0,
        .smtCtrl  = 1,
        .gpioPin  = CAM_PIN_RST,
        .gpioFun  = GPIO_FUN_GPIO,
    };

    GLB_GPIO_Init(&cfg);
    GLB_GPIO_Write(CAM_PIN_RST, (uint32_t)(level ? 1 : 0));
    GLB_GPIO_OUTPUT_Enable(CAM_PIN_RST);
}

/* ---------------------------------------------------------------------------
 * Probe
 * -------------------------------------------------------------------------*/

int cam_probe(void)
{
    cam_i2c_pins();
    cam_power_up();
    cam_dvp_pins();

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
                  CAM_PIN_SCL, CAM_PIN_SDA);
        s_probed = false;
        return CAM_ERR_NODEV;
    }

    s_probed = true;
    IPRO_LOGI(LOG_TAG, "sensor %s @0x%02X, %ux%u, pclk %u Hz, total %ux%u",
              s_desc->name, s_desc->addr,
              (unsigned)s_desc->width, (unsigned)s_desc->height,
              (unsigned)s_desc->pclk,
              (unsigned)s_desc->total_width, (unsigned)s_desc->total_height);
    return CAM_OK;
}

const char *cam_sensor_name(void) { return s_desc ? s_desc->name : NULL; }
uint8_t     cam_sensor_addr(void) { return s_desc ? s_desc->addr : 0u; }
bool        cam_is_running(void)  { return s_running; }
int         cam_get_fit(void)     { return s_fit; }
int         cam_get_order(void)   { return s_order; }
int         cam_get_rot(void)     { return s_rot; }

int cam_set_rot(int rot)
{
    if (rot < 0 || rot > 3) {
        return CAM_ERR_PARAM;
    }
    s_rot = (uint8_t)rot;
    return CAM_OK;
}

int cam_set_order(int order)
{
    if (order != CAM_ORDER_RGB && order != CAM_ORDER_BGR) {
        return CAM_ERR_PARAM;
    }
    s_order = order;
    return CAM_OK;
}
int         cam_get_fps(void)     { return s_fps; }

void cam_sensor_size(uint16_t *w, uint16_t *h)
{
    if (w) { *w = s_desc ? s_desc->width  : CAM_SENSOR_W; }
    if (h) { *h = s_desc ? s_desc->height : CAM_SENSOR_H; }
}

void cam_preview_size(uint16_t *w, uint16_t *h)
{
    if (w) { *w = s_pw; }
    if (h) { *h = s_ph; }
}

int cam_set_fit(int fit)
{
    if (fit != CAM_FIT_LETTERBOX && fit != CAM_FIT_CROP) {
        return CAM_ERR_PARAM;
    }
    s_fit = fit;
    return CAM_OK;
}

int cam_set_fps(int fps)
{
    isp_hal_frame_rate_t frc;

    if (fps < 1 || fps > 30) {
        return CAM_ERR_PARAM;
    }
    s_fps = fps;
    if (s_isp_up && s_running) {
        frc.src_fps = 30;
        frc.dst_fps = (uint8_t)fps;
        isp_fps_cfg(ISP_CHANNEL_1, frc);
    }
    return CAM_OK;
}

void cam_get_stats(cam_stats_t *out)
{
    if (out) {
        *out = s_stats;
    }
}

/* ---------------------------------------------------------------------------
 * ISP callbacks
 * -------------------------------------------------------------------------*/

static void cam_isp_cb(isp_out_frame_info_t info)
{
    BaseType_t woken = pdFALSE;

    switch (info.event) {
        case ISP_HAL_INT_FIFO_OVERWRITE:  s_stats.fifo_err++;  return;
        case ISP_HAL_INT_VSYNC_CNT_ERROR: s_stats.vsync_err++; return;
        case ISP_HAL_INT_HSYNC_CNT_ERROR: s_stats.hsync_err++; return;
        case ISP_HAL_INT_NORMAL:          break;
        default:                          return;
    }

    if (info.out_port != ISP_CHANNEL_1) {
        return;
    }

    s_stats.frames++;
    if (s_converge) {
        s_converge--;
        return;
    }
    if (s_latest) {
        /* The previous frame was never picked up - the panel blit is slower
         * than the ISP. Count it and take the newer one. */
        s_stats.dropped++;
    }
    s_latest = info.out_addr;

    if (s_frame_sem) {
        xSemaphoreGiveFromISR(s_frame_sem, &woken);
        portYIELD_FROM_ISR(woken);
    }
}

static void cam_seof_isr(void)
{
    MM_MISC_SEOF_TYPE_E status;

    MM_MISC_Get_SEOF_TypeStatus(MM_MISC_SEOF0, &status);
    if (status.BF.SoF && s_stats_cb) {
        s_stats_cb(NULL);     /* drives the ISP firmware's AE/AWB statistics */
    }
    MM_MISC_Clr_SEOF_TypeStatus(MM_MISC_SEOF0, status);
}

/* ---------------------------------------------------------------------------
 * Preview task
 * -------------------------------------------------------------------------*/

/*
 * Repack one ARGB row into RGB565, already byte-swapped for the panel.
 *
 * Byte 0 of an ISP ch1 pixel is RED - but only once the MM blocks have been
 * reset before configuring, which cam_isp_up() now always does.
 *
 * This took three measurements to pin down, and two of them were misleading:
 *
 *   - With the sensor's IQ table silently missing (before the
 *     -usc101iot_img_param link fix), the ISP ran on the hardcoded matrix in
 *     hal_isp.c isp_yuv2rgb_init() and byte 0 was RED.
 *   - Once the IQ table loaded, the first start came out swapped and the
 *     SECOND came out correct - same binary, same settings. That killed the
 *     idea that the order is a fixed property at all: it depends on the state
 *     the YUV-to-RGB block happens to be in, and only the second start was
 *     preceded by a reset.
 *   - With the reset moved to the start of every bring-up, both agree on RED.
 *
 * `cam order rgb|bgr` still switches it live and `cam pixels` reads the answer
 * off a red scene - do not treat this as settled without looking.
 *
 * The branch is hoisted out of the pixel loop - this runs 320 times per row,
 * 180 rows per frame.
 */
static void cam_pack_row(const uint32_t *src, uint16_t npix)
{
    if (s_order == CAM_ORDER_RGB) {
        for (uint16_t i = 0; i < npix; i++) {
            uint32_t px = src[i];
            uint8_t  r  = (uint8_t)(px & 0xFFu);
            uint8_t  g  = (uint8_t)((px >> 8) & 0xFFu);
            uint8_t  b  = (uint8_t)((px >> 16) & 0xFFu);
            s_row[i] = st7789p3_swap(ST_RGB565(r, g, b));
        }
    } else {
        for (uint16_t i = 0; i < npix; i++) {
            uint32_t px = src[i];
            uint8_t  b  = (uint8_t)(px & 0xFFu);
            uint8_t  g  = (uint8_t)((px >> 8) & 0xFFu);
            uint8_t  r  = (uint8_t)((px >> 16) & 0xFFu);
            s_row[i] = st7789p3_swap(ST_RGB565(r, g, b));
        }
    }
}

static void cam_blit_frame(uint32_t addr)
{
    const uint32_t *px = (const uint32_t *)(uintptr_t)addr;

    /* The ISP wrote this frame by DMA; drop any stale cache lines first. */
    L1C_DCACHE_INVALID_RANGE(addr, s_argb_bytes);

    st7789p3_bus_lock();
    st7789p3_blit_start(s_ox, s_oy,
                        (uint16_t)(s_ox + s_pw - 1u),
                        (uint16_t)(s_oy + s_ph - 1u));
    for (uint16_t y = 0; y < s_ph; y++) {
        cam_pack_row(px + (uint32_t)y * s_pw, s_pw);
        st7789p3_blit_push(s_row, s_pw);
    }
    st7789p3_blit_end();
    st7789p3_bus_unlock();

    s_stats.drawn++;
    s_stats.last_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

static void cam_preview_task(void *arg)
{
    (void)arg;

    while (s_running) {
        if (xSemaphoreTake(s_frame_sem, pdMS_TO_TICKS(500)) != pdTRUE) {
            continue;               /* no frame - let the loop re-check s_running */
        }
        uint32_t addr = s_latest;
        s_latest = 0;
        if (addr && s_running) {
            cam_blit_frame(addr);
        }
    }

    s_task = NULL;
    vTaskDelete(NULL);
}

/* ---------------------------------------------------------------------------
 * Pipeline
 * -------------------------------------------------------------------------*/

/* Work out the preview geometry for the current fit mode. The sensor is 16:9
 * and the panel in landscape is 4:3, so something has to give. */
static void cam_calc_geometry(uint16_t panel_w, uint16_t panel_h,
                              isp_hal_crop_t *scaler_in)
{
    uint16_t sw, sh;

    cam_sensor_size(&sw, &sh);

    if (s_fit == CAM_FIT_CROP) {
        /* Centre-crop to 4:3 (960x720 of 1280x720), then a clean 3x downscale
         * fills the whole 320x240 panel with no distortion. */
        uint16_t cw = (uint16_t)((uint32_t)sh * panel_w / panel_h);
        if (cw > sw) {
            cw = sw;
        }
        scaler_in->start_x = (uint16_t)((sw - cw) / 2u);
        scaler_in->start_y = 0;
        scaler_in->width   = cw;
        scaler_in->height  = sh;
        s_pw = panel_w;
        s_ph = panel_h;
    } else {
        /* Whole field of view, letterboxed: 1280x720 -> 320x180 (a clean 4x),
         * centred with 30 black rows above and below. */
        scaler_in->start_x = 0;
        scaler_in->start_y = 0;
        scaler_in->width   = sw;
        scaler_in->height  = sh;
        s_pw = panel_w;
        s_ph = (uint16_t)((uint32_t)sh * panel_w / sw);
        if (s_ph > panel_h) {
            s_ph = panel_h;
        }
    }

    s_ox = (uint16_t)((panel_w - s_pw) / 2u);
    s_oy = (uint16_t)((panel_h - s_ph) / 2u);
}

static int cam_isp_up(void)
{
    isp_hal_cfg_t     cfg = {0};
    isp_hal_vin_cfg_t vin = {0};
    isp_hal_crop_t    scaler_in;
    uint16_t          panel_w = st7789p3_width();
    uint16_t          panel_h = st7789p3_height();
    uint16_t          sw, sh;

    /* Un-gate the multimedia AHB clock BEFORE touching any ISP / DVP2AXI /
     * MM_MISC register. Without it every write into that domain is dropped:
     * the sensor still probes fine (SCCB is I2C0, a different domain) and
     * isp_init() returns success, but nothing ever streams and no completion
     * interrupt fires - `cam status` sits at 0 frames with 0 errors. Every
     * working camera app in the SDK does this first (uvc_isp.c:883,
     * mc_camera.c:952, pircam_isp.c:725). */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

    cam_sensor_size(&sw, &sh);
    cam_calc_geometry(panel_w, panel_h, &scaler_in);

    s_argb_bytes = ARGB_FRAME_SIZE(s_pw, s_ph);
    for (int i = 0; i < 2; i++) {
        if (!s_argb[i]) {
            s_argb[i] = (uint32_t)(uintptr_t)pvPortMalloc(s_argb_bytes);
            if (!s_argb[i]) {
                IPRO_LOGE(LOG_TAG, "PSRAM alloc %u B for ch1 buffer %d failed",
                          (unsigned)s_argb_bytes, i);
                return CAM_ERR_NOMEM;
            }
        }
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

    /* Channel 0 stays off: nothing here consumes YUV, and a second live
     * channel only steals DVP/ISP/memory bandwidth from the preview. */
    cfg.out_ch_0.enable = 0;

    cfg.out_ch_1.enable                     = 1;
    cfg.out_ch_1.scaler_cfg.in              = scaler_in;
    cfg.out_ch_1.scaler_cfg.out.width       = s_pw;
    cfg.out_ch_1.scaler_cfg.out.height      = s_ph;
    cfg.out_ch_1.format                     = ISP_HAL_OUT_FMT1_ARGB;
    cfg.out_ch_1.chn_common.fr_ctrl.src_fps = 30;
    cfg.out_ch_1.chn_common.fr_ctrl.dst_fps = (uint8_t)s_fps;
    cfg.out_ch_1.chn_common.addr[0]         = s_argb[0];
    cfg.out_ch_1.chn_common.addr[1]         = s_argb[1];
    cfg.out_ch_1.chn_common.pp_num          = 2;
    cfg.out_ch_1.chn_common.out_crop.start_x = 0;
    cfg.out_ch_1.chn_common.out_crop.start_y = 0;
    cfg.out_ch_1.chn_common.out_crop.width   = s_pw;
    cfg.out_ch_1.chn_common.out_crop.height  = s_ph;

    cfg.out_callback = cam_isp_cb;

    isp_init(&cfg);

    for (int i = 0; i < 2; i++) {
        L1C_DCACHE_CLEAN_INVALID_RANGE(s_argb[i], s_argb_bytes);
    }

    /* ISP firmware 3A. imgproc registers its statistics handler through
     * ipro_isp_stats_ready_config() above; the SEOF ISR then drives it. */
    vin.in_size.width   = cfg.in_size.width;
    vin.in_size.height  = cfg.in_size.height;
    vin.glb_crop.width  = cfg.glb_crop.width;
    vin.glb_crop.height = cfg.glb_crop.height;
    memcpy(&vin.sensor_name, CAM_SENSOR_NAME "\0", sizeof(CAM_SENSOR_NAME) + 1);
    if (imgproc_init(&vin) != 0) {
        /* Non-fatal: the pipeline still streams, but AE/AWB never converge so
         * the picture stays at the sensor's power-on exposure. Worth saying
         * out loud rather than swallowing - it prints its own
         * "cannot match any IQ param" line just above this. */
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
        MM_MISC_Set_SEOF_LineCnt(MM_MISC_SEOF0, CAM_SEOF_LINE_CNT);
        MM_MISC_Set_SEOF_Src(MM_MISC_SEOF0, MM_MISC_SEOF_INTR_SRC_ISP_OUTPUT);
        MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF0, seof);
        Interrupt_Handler_Register(ISP_MM_SEOF0_IRQn, cam_seof_isr);
        CPU_Interrupt_Enable(ISP_MM_SEOF0_IRQn);
    }

    /* Channel 1 writes through dvp2axi port C; its bus master has to be on. */
    MM_MISC_Set_IPROAIPRO_Rcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Rcmd_MasterEn(0, 0, 1);
    MM_MISC_Set_IPROAIPRO_Wcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Wcmd_MasterEn(0, 0, 1);

    isp_start();
    s_isp_up = true;
    return CAM_OK;
}

int cam_start(void)
{
    int rc;

    if (s_running) {
        return CAM_OK;
    }
    if (!st7789p3_is_ready()) {
        IPRO_LOGE(LOG_TAG, "panel not initialised - run `lcd init` first");
        return CAM_ERR_STATE;
    }
    /*
     * Bring the MM blocks up in a known state before anything streams into
     * them. The sensor is held in reset for the duration: resetting DVP_AS
     * with data flowing is what wedges the capture side, so the order here is
     * load-bearing, not tidiness.
     *
     * Doing this on EVERY start (not just after a cam_stop) is what makes the
     * first start behave like the second - including the ISP's colour channel
     * order, which otherwise came out swapped on the first start only.
     */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
    cam_reset_pin(0);
    cam_mm_reset();

    /*
     * Re-probe on every start, not just the first.
     *
     * isp_deinit() drops DVP_AS, and re-enabling it part-way through a frame
     * leaves the capture side waiting for a frame boundary that never comes -
     * the second `cam start` came back with zero completions and zero errors.
     * The first start works because cam_probe() has just pulsed the sensor's
     * reset, so the DVP stream begins at a clean frame start.
     *
     * cam_probe() re-pulses reset and re-writes the sensor registers, which is
     * what makes a restart deterministic. It costs ~200 ms and reprints the
     * SCCB scan; that is a fair price for a bring-up tool that restarts often.
     */
    rc = cam_probe();
    if (rc != CAM_OK) {
        return rc;
    }

    /* Orientation is the user's call - see s_rot. */
    st7789p3_set_dir(s_rot, 0);
    st7789p3_fill_screen(ST_BLACK);

    camera_module_start(s_desc);

    if (!s_frame_sem) {
        s_frame_sem = xSemaphoreCreateBinary();
        if (!s_frame_sem) {
            return CAM_ERR_NOMEM;
        }
    }

    memset(&s_stats, 0, sizeof(s_stats));
    s_latest   = 0;
    s_converge = CAM_CONVERGE_FRAMES;

    rc = cam_isp_up();
    if (rc != CAM_OK) {
        return rc;
    }

    s_running = true;
    if (xTaskCreate(cam_preview_task, "cam_prev", CAM_PREVIEW_STACK, NULL,
                    CAM_PREVIEW_PRIO, &s_task) != pdPASS) {
        s_running = false;
        IPRO_LOGE(LOG_TAG, "preview task create failed");
        return CAM_ERR_NOMEM;
    }

    IPRO_LOGI(LOG_TAG, "preview %ux%u at (%u,%u), %s, %d fps, byte0=%s",
              (unsigned)s_pw, (unsigned)s_ph, (unsigned)s_ox, (unsigned)s_oy,
              (s_fit == CAM_FIT_CROP) ? "centre-crop" : "letterbox", s_fps,
              (s_order == CAM_ORDER_RGB) ? "R" : "B");
    IPRO_LOGI(LOG_TAG, "panel orientation %u (%s)", (unsigned)s_rot,
              (s_rot & 1u) ? "landscape 320x240" : "portrait 240x320");
    return CAM_OK;
}

void cam_stop(void)
{
    /*
     * Park the sensor even when nothing was running. Its RESETB defaults to
     * released - R56 pulls it up to DOVDD1V8 the moment GPIO35 goes high-Z -
     * so a sensor that was left configured keeps streaming, and it draws that
     * current from the same 1V8 LDO that feeds the SoC's VDDIO_1 bank. Leaving
     * it streaming across a reset is what stops the board booting with the
     * camera attached.
     */
    cam_reset_pin(0);

    if (!s_running) {
        return;
    }
    s_running = false;
    if (s_frame_sem) {
        xSemaphoreGive(s_frame_sem);     /* wake the task so it can exit */
    }
    for (int i = 0; i < 50 && s_task; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    if (s_isp_up) {
        MM_MISC_SEOF_TYPE_E seof_off = {0};

        /*
         * Teardown order matters, and getting it wrong is what made the second
         * `cam start` come back with zero completions and zero errors.
         *
         * 1. Stop the SENSOR first. Tearing the ISP down while DVP data is
         *    still arriving leaves the capture side wedged mid-frame - the
         *    SDK's own miniclaw teardown calls this out in as many words
         *    ("Stop sensor before tearing down ISP - prevents stale DVP
         *    data"). Re-pulsing the sensor at the next start is NOT enough on
         *    its own; the wedge is on the ISP side.
         * 2. Silence SEOF before the handler goes away.
         * 3. Tear the pipeline down.
         * 4. Software-reset the blocks we actually drive, while MM is still
         *    clocked, so none of them come back holding stale state.
         *
         * GLB_AHB_CLOCK_MM is deliberately left un-gated: gating it also broke
         * the restart, because state in the MM domain does not survive the
         * clock going away and isp_init() does not put it back.
         */
        GLB_GPIO_Write(CAM_PIN_RST, 0);          /* hold the sensor in reset */

        MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF0, seof_off);
        CPU_Interrupt_Disable(ISP_MM_SEOF0_IRQn);
        Interrupt_Handler_Register(ISP_MM_SEOF0_IRQn, NULL);

        /*
         * Deliberately NOT calling imgproc_stop() here.
         *
         * img_proc.status only ever holds IMGPROC_INIT from its static
         * zero-initialiser. imgproc_stop() moves it to IMGPROC_PAUSE and
         * nothing in the component ever moves it back, while
         * imgproc_start_fast_isp() does its entire job inside
         * "if (img_proc.status == IMGPROC_INIT)". So stopping imgproc is a
         * one-way door: the second start silently does nothing, 3A never runs
         * again, and the preview comes back with stale white-balance gains -
         * yellow rendering as blue. Leaving the task running across the ISP
         * restart is the only way to get AE/AWB back without patching the
         * component.
         */
        isp_deinit();

        cam_mm_reset();          /* sensor is held in reset above */

        s_isp_up = false;
    }
    IPRO_LOGI(LOG_TAG, "preview stopped (%u drawn, %u dropped)",
              (unsigned)s_stats.drawn, (unsigned)s_stats.dropped);
}

int cam_dump_pixels(uint32_t n)
{
    const uint32_t *px;
    uint32_t addr = s_argb[0];

    if (!addr || !s_stats.frames) {
        return CAM_ERR_STATE;
    }
    if (n == 0u || n > 16u) {
        n = 8u;
    }

    L1C_DCACHE_INVALID_RANGE(addr, n * 4u);
    px = (const uint32_t *)(uintptr_t)addr;
    IPRO_LOGI(LOG_TAG, "point the sensor at something strongly RED, then read "
                       "which column holds the large value:");
    for (uint32_t i = 0; i < n; i++) {
        uint32_t v  = px[i];
        unsigned b0 = (unsigned)(v & 0xFFu);
        unsigned b1 = (unsigned)((v >> 8) & 0xFFu);
        unsigned b2 = (unsigned)((v >> 16) & 0xFFu);
        IPRO_LOGI(LOG_TAG,
                  "argb[%u]=0x%08X  byte0=%3u byte1=%3u byte2=%3u pad=%3u  "
                  "-> rgb order gives R=%u B=%u",
                  (unsigned)i, (unsigned)v, b0, b1, b2,
                  (unsigned)((v >> 24) & 0xFFu), b0, b2);
    }
    IPRO_LOGI(LOG_TAG, "current setting: byte0 = %s (`cam order rgb|bgr`)",
              (s_order == CAM_ORDER_RGB) ? "R" : "B");
    return CAM_OK;
}
