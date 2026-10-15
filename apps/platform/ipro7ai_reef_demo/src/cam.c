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
 * @brief FLEX-24 camera bring-up and headless YUV stream. See cam.h.
 *
 * Power-up order from the OV5640 datasheet (sec. 2.7): rails are fixed on
 * this board, XVCLK runs >= 1 ms before PWDN is released, DVDD stable >= 5 ms
 * before PWDN, RESETB high >= 20 ms before the first SCCB access.
 *
 * The MM-block reset / teardown order is the one apps/platform/
 * ipro7ai_evk_demo/src/cam.c arrived at the hard way: reset DVP_AS only with
 * the sensor held in reset, stop the sensor before the ISP, never gate the
 * MM clock between runs.
 */

#include "cam.h"
#include "board_config.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "camera.h"
#include "sensor.h"
#include "sccb.h"
#include "hal_gpio.h"
#include "hal_isp.h"
#include "hal_mtimer.h"
#include "drv_glb.h"
#include "drv_mm_misc.h"
#include "drv_dvp2axi.h"
#include "drv_isp.h"
#include <system_soc.h>
#include "ipro_log.h"

#define LOG_TAG "CAM"

#ifndef CAM_SCALE_DIV                /* an app linking this file may override */
#define CAM_SCALE_DIV    4u          /* ch0 output = sensor / 4 */
#endif
#define CAM_PP_NUM       2u
#define CAM_FIRST_FRAMES     2u      /* ch0 frames that prove capture runs */
#define CAM_CAPTURE_TRIES    5u
#define CAM_START_TRIES      5u
#ifndef CAM_FRAME_STRETCH
#define CAM_FRAME_STRETCH    2u      /* AEC night mode: frames up to 2x */
#endif

/* The Reef's OV5640 module reads out top-to-bottom mirrored and the sensor
 * cannot flip without breaking its demosaic (components/sensor/ov5640.c),
 * so the capture DMA writes the lines bottom-up instead. In flip mode
 * DVP2AXI writes the first line at the start address and each next one a
 * line LOWER (bench, 2026-10-07: a guard frame in front of the buffer filled
 * up), so the address it is given is the buffer's last line. */
#ifndef CAM_DVP_VFLIP
#define CAM_DVP_VFLIP    1
#endif

/* IPRO7 ISP Cb / Cr saturation (YUVC, 0x80 = 1.0; it acts on the YUV path).
 * On a screen chart the OV5640 rendered colours at ~0.8 of their chroma;
 * 0x90 (1.125) brings them to ~0.9 without tinting the greys. */
#ifndef CAM_ISP_SATURATION
#define CAM_ISP_SATURATION 0x90
#endif
#define CAM_SNAP_MAX_COLS 96u

static const rt_camera_desc *s_desc;
static bool      s_probed, s_running, s_isp_up;
static uint32_t  s_pp[CAM_PP_NUM];
static uint32_t  s_pp_bytes;
static uint32_t  s_flip_off;        /* DMA start = buffer + this */
static bool      s_streaming_hook_done;
static uint16_t  s_ow, s_oh;
static volatile uint32_t s_latest;
static cam_stats_t s_stats;
static uint32_t  s_fps_t0_us, s_fps_frames0;
static int       s_lower_first = 1, s_vsync_high = 1, s_hsync_high = 1;

/* ---------------------------------------------------------------------------
 * Pads and clocks
 * -------------------------------------------------------------------------*/

static void cam_gpio_out(uint8_t pin, int level)
{
    uint8_t pins[1] = { pin };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pins, 1);
    gpio_set_mode(pin, GPIO_OUTPUT_MODE);
    gpio_write(pin, (uint32_t)level);
}

void cam_xclk(int on)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioPin  = BOARD_CAM_MCLK_PIN,
        .gpioFun  = GPIO_FUN_CAM_MISC,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_NONE,
        .drive    = 3,
        .smtCtrl  = 1,
    };

    if (on) {
        GLB_GPIO_Init(&cfg);
        GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_PLL48M, 1);   /* 48 / 2 = 24 MHz */
    } else {
        GLB_Set_CAM_CLK(DISABLE, GLB_CAM_CLK_PLL48M, 1);
        cam_gpio_out(BOARD_CAM_MCLK_PIN, 0);
    }
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
        BOARD_CAM_D0_PIN + 0, BOARD_CAM_D0_PIN + 1, BOARD_CAM_D0_PIN + 2, BOARD_CAM_D0_PIN + 3,
        BOARD_CAM_D0_PIN + 4, BOARD_CAM_D0_PIN + 5, BOARD_CAM_D0_PIN + 6, BOARD_CAM_D0_PIN + 7,
        BOARD_CAM_PCLK_PIN, BOARD_CAM_VSYNC_PIN, BOARD_CAM_HSYNC_PIN,
    };

    for (unsigned i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        cfg.gpioPin = pins[i];
        GLB_GPIO_Init(&cfg);
    }
}

/* Reset + power-down, XCLK on, release PWDN, release reset, wait for SCCB. */
static void cam_power_up(void)
{
    cam_gpio_out(BOARD_CAM_RST_PIN, 0);
    cam_gpio_out(BOARD_CAM_PWDN_PIN, 1);
    cam_xclk(1);
    vTaskDelay(pdMS_TO_TICKS(5));
    gpio_write(BOARD_CAM_PWDN_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    gpio_write(BOARD_CAM_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(25));
}

void cam_park(void)
{
    cam_gpio_out(BOARD_CAM_PWDN_PIN, 1);
    cam_gpio_out(BOARD_CAM_RST_PIN, 0);
    cam_xclk(0);
}

/*
 * Put the MM blocks we drive back into a known state. MUST be called with
 * the sensor held in reset: resetting DVP_AS while data is flowing wedges the
 * capture side mid-frame.
 */
static void cam_mm_reset(void)
{
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP_AS, 1);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP_AS, 0);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP2AXI_A, 1);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_DVP2AXI_A, 0);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_ISP, 1);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_ISP, 0);
}

/* ---------------------------------------------------------------------------
 * Probe
 * -------------------------------------------------------------------------*/

/*
 * Diagnostic for the case where camera_module_init() finds nothing: read the
 * two OmniVision IDs directly. If one answers here, the framework's 8-bit
 * SCCB scan is what missed it (see components/sensor/camera.c), not the FPC.
 */
static void cam_raw_id_check(void)
{
    uint8_t hi = 0, lo = 0;

    if (SCCB_Read_Reg16(0x3C, 0x300A, &hi, 1) == 0 && SCCB_Read_Reg16(0x3C, 0x300B, &lo, 1) == 0 &&
        !(hi == 0xFF && lo == 0xFF)) {
        IPRO_LOGW(LOG_TAG, "raw read: 0x3C answers with id 0x%02X%02X - framework scan missed it", hi, lo);
        return;
    }
    hi = 0x01;
    if (SCCB_Write(0x30, 0xFF, &hi) == 0 && SCCB_Read(0x30, 0x0A, &hi, 1) == 0 &&
        SCCB_Read(0x30, 0x0B, &lo, 1) == 0 && !(hi == 0xFF && lo == 0xFF)) {
        IPRO_LOGW(LOG_TAG, "raw read: 0x30 answers with pid 0x%02X ver 0x%02X - framework scan missed it", hi, lo);
        return;
    }
    IPRO_LOGW(LOG_TAG, "raw read: nothing at 0x3C or 0x30 either");
}

int cam_probe(void)
{
    cam_power_up();
    cam_dvp_pins();
    SCCB_Init();          /* GPIO36/37 -> I2C0, per components/sensor/sccb.h */

    /* camera_module_init() walks the .camera_desc section, scans each
     * driver's SCCB address and resets the one that answers. A cold sensor
     * can need a couple of tries while its PLL locks. */
    for (int retry = 0; retry < 3; retry++) {
        s_desc = camera_module_init();
        if (s_desc) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    if (!s_desc) {
        cam_raw_id_check();
        IPRO_LOGE(LOG_TAG, "no sensor answered - check FPC orientation "
                           "(pads 2/15 to GND), the 2.8/1.5 V rails and XCLK");
        s_probed = false;
        return CAM_ERR_NODEV;
    }
    s_probed = true;
    IPRO_LOGI(LOG_TAG, "sensor %s @0x%02X, %ux%u, DVP pclk %u Hz, total %ux%u",
              s_desc->name, s_desc->addr, (unsigned)s_desc->width, (unsigned)s_desc->height,
              (unsigned)s_desc->pclk, (unsigned)s_desc->total_width, (unsigned)s_desc->total_height);
    return CAM_OK;
}

const char *cam_sensor_name(void) { return s_desc ? s_desc->name : NULL; }
uint8_t     cam_sensor_addr(void) { return s_desc ? s_desc->addr : 0u; }
bool        cam_is_running(void)  { return s_running; }

void cam_sensor_size(uint16_t *w, uint16_t *h)
{
    if (w) *w = s_desc ? s_desc->width  : 0;
    if (h) *h = s_desc ? s_desc->height : 0;
}

void cam_out_size(uint16_t *w, uint16_t *h)
{
    if (w) *w = s_ow;
    if (h) *h = s_oh;
}

uint32_t cam_latest_frame(uint32_t *frames)
{
    if (frames) *frames = s_stats.frames;
    return s_running ? s_latest : 0u;
}

/* ---------------------------------------------------------------------------
 * ISP
 * -------------------------------------------------------------------------*/

static void cam_isp_cb(isp_out_frame_info_t info)
{
    switch (info.event) {
    case ISP_HAL_INT_FIFO_OVERWRITE:  s_stats.fifo_err++;  return;
    case ISP_HAL_INT_VSYNC_CNT_ERROR: s_stats.vsync_err++; return;
    case ISP_HAL_INT_HSYNC_CNT_ERROR: s_stats.hsync_err++; return;
    case ISP_HAL_INT_NORMAL:          break;
    default:                          return;
    }
    if (info.out_port != ISP_CHANNEL_0) {
        return;
    }
    s_stats.frames++;
    s_latest = info.out_addr - s_flip_off;
}

static int cam_isp_up(void)
{
    isp_hal_cfg_t cfg = {0};
    uint16_t sw, sh;

    /* Every write into the MM domain is dropped until this is done; the
     * sensor still probes (I2C is another domain) and nothing streams. */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);

    cam_sensor_size(&sw, &sh);
    s_ow = (uint16_t)(sw / CAM_SCALE_DIV);
    s_oh = (uint16_t)(sh / CAM_SCALE_DIV);
    s_pp_bytes = (uint32_t)s_ow * s_oh * 2u;          /* YUYV */
    s_flip_off = CAM_DVP_VFLIP ? (uint32_t)(s_oh - 1u) * s_ow * 2u : 0u;
    for (unsigned i = 0; i < CAM_PP_NUM; i++) {
        if (!s_pp[i]) {
            s_pp[i] = (uint32_t)(uintptr_t)pvPortMalloc(s_pp_bytes);
            if (!s_pp[i]) {
                IPRO_LOGE(LOG_TAG, "PSRAM alloc %u B for ch0 buffer %u failed",
                          (unsigned)s_pp_bytes, i);
                return CAM_ERR_NOMEM;
            }
        }
        memset((void *)(uintptr_t)s_pp[i], 0, s_pp_bytes);
        L1C_DCACHE_CLEAN_INVALID_RANGE(s_pp[i], s_pp_bytes);
    }

    cfg.isp_src                       = ISP_HAL_SRC_DVP_AS;
    cfg.in_size.width                 = sw;
    cfg.in_size.height                = sh;
    cfg.src_cfg.dvp_input.hsync       = s_hsync_high ? ISP_HAL_DVP_ACTIVE_HIGH : ISP_HAL_DVP_ACTIVE_LOW;
    cfg.src_cfg.dvp_input.vsync       = s_vsync_high ? ISP_HAL_DVP_ACTIVE_HIGH : ISP_HAL_DVP_ACTIVE_LOW;
    cfg.src_cfg.dvp_input.order       = s_lower_first ? ISP_HAL_DATA_BYTE_LOWER : ISP_HAL_DATA_BYTE_HIGHER;
    cfg.src_cfg.dvp_input.pclk        = s_desc->pclk;
    cfg.src_cfg.dvp_input.totalHPixels = s_desc->total_width;
    cfg.glb_crop.start_x              = 0;
    cfg.glb_crop.start_y              = 0;
    cfg.glb_crop.width                = sw;
    cfg.glb_crop.height               = sh;
    cfg.yuv_adj_en                    = 0;

    cfg.out_ch_0.enable                     = 1;
    cfg.out_ch_0.scaler_cfg.in.start_x      = 0;
    cfg.out_ch_0.scaler_cfg.in.start_y      = 0;
    cfg.out_ch_0.scaler_cfg.in.width        = sw;
    cfg.out_ch_0.scaler_cfg.in.height       = sh;
    cfg.out_ch_0.scaler_cfg.out.width       = s_ow;
    cfg.out_ch_0.scaler_cfg.out.height      = s_oh;
    cfg.out_ch_0.format                     = ISP_HAL_OUT_FMT0_YUYV;
    cfg.out_ch_0.ring_line                  = 0;
    cfg.out_ch_0.chn_common.fr_ctrl.src_fps = 30;
    cfg.out_ch_0.chn_common.fr_ctrl.dst_fps = 30;
    cfg.out_ch_0.chn_common.addr[0]         = s_pp[0] + s_flip_off;
    cfg.out_ch_0.chn_common.addr[1]         = s_pp[1] + s_flip_off;
    cfg.out_ch_0.chn_common.pp_num          = CAM_PP_NUM;
    cfg.out_ch_0.chn_common.out_crop.start_x = 0;
    cfg.out_ch_0.chn_common.out_crop.start_y = 0;
    cfg.out_ch_0.chn_common.out_crop.width   = s_ow;
    cfg.out_ch_0.chn_common.out_crop.height  = s_oh;
    cfg.out_ch_1.enable                     = 0;
    cfg.out_callback                        = cam_isp_cb;

    isp_init(&cfg);
    {
        ISP_SATURATION_S sat = { CAM_ISP_SATURATION, CAM_ISP_SATURATION };

        ISP_Set_Saturation(&sat);
    }
#if CAM_DVP_VFLIP
    DVP2AXI_Disable(DVP2AXI_PORT_A);            /* ch0 = dvp2axi port A */
    DVP2AXI_FlipModeEnable(DVP2AXI_PORT_A);
    DVP2AXI_Enable(DVP2AXI_PORT_A);
#endif

    /* Channel 0 writes through dvp2axi port A; its bus master has to be on. */
    MM_MISC_Set_IPROAIPRO_Rcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Rcmd_MasterEn(1, 0, 0);
    MM_MISC_Set_IPROAIPRO_Wcmd_Mode(MM_MISC_IPROAIPRO_MODE_PROACTIVE);
    MM_MISC_Set_IPROAIPRO_Wcmd_MasterEn(1, 0, 0);

    isp_start();
    s_isp_up = true;
    return CAM_OK;
}

static void cam_stats_reset(void)
{
    memset(&s_stats, 0, sizeof(s_stats));
    s_latest = 0;
    s_fps_t0_us = (uint32_t)mtimer_get_time_us();
    s_fps_frames0 = 0;
}

/* Longest sensor frame period in ms: total pixels at 2 PCLK each,
 * stretched by the AEC night mode, rounded up. */
static uint32_t cam_frame_ms_max(void)
{
    uint64_t pclks = (uint64_t)s_desc->total_width * s_desc->total_height * 2u;

    return (uint32_t)((pclks * 1000u * CAM_FRAME_STRETCH + s_desc->pclk - 1u) / s_desc->pclk);
}

/* Disable DVP2AXI port A and wait, at most two of the longest frame
 * periods, for the frame in flight to end. A starved port never goes
 * idle; only its AXI master does. */
static void cam_capture_quiesce(void)
{
    const uint32_t idle = DVP2AXI_ST_IDLE_MSK | DVP2AXI_AXI_IDLE_MSK;
    const uint32_t ms = 2u * cam_frame_ms_max();
    volatile uint32_t *sts = (volatile uint32_t *)(D2XA_BASE + DVP2AXI_STATUS_OFFSET);
    TickType_t t0 = xTaskGetTickCount();

    DVP2AXI_Disable(DVP2AXI_PORT_A);
    while ((*sts & idle) != idle) {
        if ((xTaskGetTickCount() - t0) >= pdMS_TO_TICKS(ms)) {
            if (!(*sts & DVP2AXI_AXI_IDLE_MSK)) {
                IPRO_LOGW(LOG_TAG, "dvp2axi A busy after %u ms (status 0x%08lx)",
                          (unsigned)ms, (unsigned long)*sts);
            }
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

/* True once ch0 has delivered CAM_FIRST_FRAMES frames, within `ms` */
static bool cam_first_frames(uint32_t ms)
{
    TickType_t t0 = xTaskGetTickCount();

    while ((xTaskGetTickCount() - t0) < pdMS_TO_TICKS(ms)) {
        if (s_stats.frames >= CAM_FIRST_FRAMES) {
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    return false;
}

/* Tear down and rebuild the ISP / DVP_AS / DVP2AXI side only; the
 * sensor keeps streaming. */
static int cam_capture_reinit(void)
{
    cam_capture_quiesce();
    isp_deinit();
    cam_mm_reset();
    s_isp_up = false;
    cam_stats_reset();
    return cam_isp_up();
}

/* Sensor first, then the ISP, then reset the blocks with MM still
 * clocked - see the file header. */
static void cam_teardown(void)
{
    if (s_isp_up) {
        cam_capture_quiesce();
    }
    GLB_GPIO_Write(BOARD_CAM_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    if (s_isp_up) {
        isp_deinit();
        cam_mm_reset();
        s_isp_up = false;
    }
    cam_park();
}

/* One sensor start; DVP_AS can come up seeing no VSYNC, so the capture
 * side is rebuilt until ch0 frames arrive, at most CAM_CAPTURE_TRIES
 * times. CAM_ERR_STATE when ch0 stays dark. */
static int cam_start_once(unsigned *rebuilds)
{
    int rc;

    /* Known state first: MM clocked, sensor in reset, blocks reset. Then a
     * fresh probe so the DVP stream begins at a clean frame boundary. */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
    cam_gpio_out(BOARD_CAM_RST_PIN, 0);
    cam_mm_reset();

    rc = cam_probe();
    if (rc != CAM_OK) {
        return rc;
    }
    camera_module_start(s_desc);          /* setFMT: YUV422 + geometry + PLL */

    cam_stats_reset();
    rc = cam_isp_up();
    /* Up to one frame to the first VSYNC, the frames, one spare */
    const uint32_t first_ms = (CAM_FIRST_FRAMES + 2u) * cam_frame_ms_max();

    for (unsigned i = 1; rc == CAM_OK && !cam_first_frames(first_ms); i++) {
        if (i >= CAM_CAPTURE_TRIES) {
            IPRO_LOGW(LOG_TAG, "ch0 dark after %u capture starts", i);
            cam_teardown();
            return CAM_ERR_STATE;
        }
        rc = cam_capture_reinit();
        (*rebuilds)++;
    }
    return rc;
}

int cam_start(void)
{
    unsigned rebuilds = 0, starts = 0;
    int rc;

    if (s_running) {
        return CAM_OK;
    }
    do {
        rc = cam_start_once(&rebuilds);
        starts++;
    } while (rc == CAM_ERR_STATE && starts < CAM_START_TRIES);
    if (rc != CAM_OK) {
        IPRO_LOGE(LOG_TAG, "start failed (%d) after %u sensor starts", rc, starts);
        return rc;
    }
    s_fps_t0_us = (uint32_t)mtimer_get_time_us();
    s_fps_frames0 = 0;
    s_streaming_hook_done = false;
    s_running = true;
    IPRO_LOGI(LOG_TAG, "streaming %s %ux%u -> ch0 YUYV %ux%u, byte order %s, "
                       "vsync %s, hsync %s, sensor starts %u, capture rebuilds %u",
              s_desc->name, (unsigned)s_desc->width, (unsigned)s_desc->height,
              (unsigned)s_ow, (unsigned)s_oh, s_lower_first ? "lower-first" : "higher-first",
              s_vsync_high ? "high" : "low", s_hsync_high ? "high" : "low", starts, rebuilds);
    return CAM_OK;
}

void cam_stop(void)
{
    if (!s_running) {
        cam_park();
        return;
    }
    s_running = false;
    cam_teardown();
    IPRO_LOGI(LOG_TAG, "stopped after %u frames (fifo %u, vsync %u, hsync %u)",
              (unsigned)s_stats.frames, (unsigned)s_stats.fifo_err,
              (unsigned)s_stats.vsync_err, (unsigned)s_stats.hsync_err);
}

/* ---------------------------------------------------------------------------
 * Readouts
 * -------------------------------------------------------------------------*/

static uint32_t cam_luma_mean(uint32_t addr)
{
    const uint8_t *p = (const uint8_t *)(uintptr_t)addr;
    uint32_t sum = 0, n = 0;

    L1C_DCACHE_INVALID_RANGE(addr, s_pp_bytes);
    /* Y is every other byte of YUYV; sample every 8th pixel. */
    for (uint32_t i = 0; i < s_pp_bytes; i += 16) {
        sum += p[i];
        n++;
    }
    return n ? sum / n : 0;
}

void cam_service(void)
{
    if (!s_running || s_streaming_hook_done || s_stats.frames < 2u) {
        return;
    }
    s_streaming_hook_done = true;
    if (s_desc && s_desc->streaming && s_desc->streaming() != 0) {
        IPRO_LOGW(LOG_TAG, "%s: streaming hook failed", s_desc->name);
    }
}

void cam_get_stats(cam_stats_t *out)
{
    uint32_t now = (uint32_t)mtimer_get_time_us();

    cam_service();          /* the demo's `cam` shell is its only camera loop */
    uint32_t dt  = now - s_fps_t0_us;

    if (dt > 200000u) {
        s_stats.fps_x10 = (uint32_t)(((uint64_t)(s_stats.frames - s_fps_frames0) * 10000000u) / dt);
        s_fps_t0_us = now;
        s_fps_frames0 = s_stats.frames;
    }
    if (s_latest) {
        s_stats.luma_mean = cam_luma_mean(s_latest);
    }
    if (out) {
        *out = s_stats;
    }
}

int cam_snap(unsigned cols)
{
    static const char ramp[] = " .:-=+*#%@";
    uint32_t addr = s_latest;
    const uint8_t *p;
    unsigned rows, sx, sy;

    cam_service();
    if (!s_running || !addr) {
        return CAM_ERR_STATE;
    }
    if (cols < 16u) cols = 16u;
    if (cols > CAM_SNAP_MAX_COLS) cols = CAM_SNAP_MAX_COLS;
    sx   = s_ow / cols;
    if (sx == 0) sx = 1;
    sy   = sx * 2u;                          /* terminal cells are ~2:1 */
    rows = s_oh / sy;

    L1C_DCACHE_INVALID_RANGE(addr, s_pp_bytes);
    p = (const uint8_t *)(uintptr_t)addr;
    printf("%ux%u YUYV frame %u, %u x %u cells\n", s_ow, s_oh,
           (unsigned)s_stats.frames, cols, rows);
    for (unsigned r = 0; r < rows; r++) {
        char line[CAM_SNAP_MAX_COLS + 1];
        for (unsigned c = 0; c < cols; c++) {
            uint32_t y = (uint32_t)(r * sy) * s_ow + (uint32_t)(c * sx);
            uint8_t  Y = p[y * 2u];          /* Y byte of that pixel */
            line[c] = ramp[(Y * (sizeof(ramp) - 2)) / 255u];
        }
        line[cols] = 0;
        printf("|%s|\n", line);
    }
    return CAM_OK;
}

int cam_reg_read(int addr, int *val)
{
    if (!s_desc || !s_desc->get_register) {
        return CAM_ERR_STATE;
    }
    return s_desc->get_register(addr, val) ? CAM_ERR_NODEV : CAM_OK;
}

int cam_reg_write(int addr, int val)
{
    if (!s_desc || !s_desc->set_register) {
        return CAM_ERR_STATE;
    }
    return s_desc->set_register(addr, val) ? CAM_ERR_NODEV : CAM_OK;
}

int cam_colorbar(bool on)
{
    int v;

    if (!s_desc) {
        return CAM_ERR_STATE;
    }
    if (s_desc->addr == 0x3C) {              /* OV5640: 0x503D bit7 */
        return cam_reg_write(0x503D, on ? 0x80 : 0x00);
    }
    if (s_desc->addr == 0x30) {              /* OV2640: sensor bank COM7 bit1 */
        if (cam_reg_read(0x112, &v) != CAM_OK) {
            return CAM_ERR_NODEV;
        }
        return cam_reg_write(0x112, on ? (v | 0x02) : (v & ~0x02));
    }
    return CAM_ERR_PARAM;
}

/* ---------------------------------------------------------------------------
 * Pad activity
 * -------------------------------------------------------------------------*/

typedef struct {
    const char *name;
    uint8_t     pin;
} cam_pad_t;

static const cam_pad_t s_pads[] = {
    { "D0",    BOARD_CAM_D0_PIN + 0 }, { "D1", BOARD_CAM_D0_PIN + 1 },
    { "D2",    BOARD_CAM_D0_PIN + 2 }, { "D3", BOARD_CAM_D0_PIN + 3 },
    { "D4",    BOARD_CAM_D0_PIN + 4 }, { "D5", BOARD_CAM_D0_PIN + 5 },
    { "D6",    BOARD_CAM_D0_PIN + 6 }, { "D7", BOARD_CAM_D0_PIN + 7 },
    { "PCLK",  BOARD_CAM_PCLK_PIN },   { "VSYNC", BOARD_CAM_VSYNC_PIN },
    { "HREF",  BOARD_CAM_HSYNC_PIN },
};
#define CAM_PAD_NUM (sizeof(s_pads) / sizeof(s_pads[0]))

/* GPIO_CFGCTL31/32 hold the input level of GPIO0-31 / 32-63 - but only for
 * pads muxed to GPIO_FUN_GPIO: with GPIO_FUN_CAM selected they read 0 even
 * with IE set (bench, 2026-10-06). So the DVP pads are switched to plain
 * inputs for the measurement and handed back to the camera block after.
 * XCLK is an output the sensor needs to keep running and is left alone. */
static void cam_pads_to_gpio_in(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioFun  = GPIO_FUN_GPIO,
        .gpioMode = GPIO_MODE_INPUT,
        .pullType = GPIO_PULL_NONE,
        .drive    = 0,
        .smtCtrl  = 1,
    };

    for (unsigned i = 0; i < CAM_PAD_NUM; i++) {
        cfg.gpioPin = s_pads[i].pin;
        GLB_GPIO_Init(&cfg);
    }
}

static inline uint64_t cam_pads_read(void)
{
    uint64_t lo = *(volatile uint32_t *)(GLB_BASE + GLB_GPIO_CFGCTL31_OFFSET);
    uint64_t hi = *(volatile uint32_t *)(GLB_BASE + GLB_GPIO_CFGCTL32_OFFSET);

    return lo | (hi << 32);
}

void cam_pins_report(unsigned ms)
{
    uint32_t edges[CAM_PAD_NUM] = {0};
    uint32_t highs[CAM_PAD_NUM] = {0};
    uint32_t n = 0, t0, dt;
    uint64_t mask = 0, prev, cur, rise;

    /* Busy-waits in the shell task; keep it short. */
    if (ms < 50u) ms = 50u;
    if (ms > 5000u) ms = 5000u;
    for (unsigned i = 0; i < CAM_PAD_NUM; i++) {
        mask |= 1ull << s_pads[i].pin;
    }
    cam_pads_to_gpio_in();
    prev = cam_pads_read() & mask;
    t0 = (uint32_t)mtimer_get_time_us();
    do {
        for (unsigned k = 0; k < 256u; k++) {
            cur  = cam_pads_read() & mask;
            rise = cur & ~prev;
            for (unsigned i = 0; i < CAM_PAD_NUM; i++) {
                edges[i] += (uint32_t)((rise >> s_pads[i].pin) & 1u);
                highs[i] += (uint32_t)((cur >> s_pads[i].pin) & 1u);
            }
            prev = cur;
        }
        n += 256u;
        dt = (uint32_t)mtimer_get_time_us() - t0;
    } while (dt < ms * 1000u);
    cam_dvp_pins();
    /* Taking the pads away from the camera block mid-frame leaves DVP_AS
     * half-way through a frame and the ISP never recovers on its own (bench:
     * blank frames, 3 fps of garbage). A running stream gets a clean restart. */
    if (s_running) {
        cam_stop();
        if (cam_start() != CAM_OK) {
            printf("stream restart after the measurement failed\n");
        }
    }

    printf("%u samples in %u us (%u ns each); PCLK edges alias, only >0 matters\n",
           (unsigned)n, (unsigned)dt, (unsigned)((uint64_t)dt * 1000u / n));
    printf("pad    gpio  rising   per s      high%%\n");
    for (unsigned i = 0; i < CAM_PAD_NUM; i++) {
        printf("%-6s %4u  %7u  %8u   %3u\n", s_pads[i].name, s_pads[i].pin,
               (unsigned)edges[i], (unsigned)((uint64_t)edges[i] * 1000000u / dt),
               (unsigned)((uint64_t)highs[i] * 100u / n));
    }
}

void cam_set_byte_order(int lower_first) { s_lower_first = lower_first ? 1 : 0; }

void cam_set_sync(int vsync_high, int hsync_high)
{
    s_vsync_high = vsync_high ? 1 : 0;
    s_hsync_high = hsync_high ? 1 : 0;
}

void cam_get_dvp_opts(int *lower_first, int *vsync_high, int *hsync_high)
{
    if (lower_first) *lower_first = s_lower_first;
    if (vsync_high)  *vsync_high  = s_vsync_high;
    if (hsync_high)  *hsync_high  = s_hsync_high;
}
