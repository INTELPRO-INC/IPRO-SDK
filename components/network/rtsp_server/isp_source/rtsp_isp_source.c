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

/*
 * Phase 5 ISP -> MJPEG pull source for the RTSP server.
 *
 * ISP Ch0 freerun ping-pong + MJPEG SW frame-mode pattern, without AI/NPU
 * or low-power paths. The encoder is driven on-demand from
 * the RTSP library's get_frm_t callback (set_strm_cb), so frames appear at
 * the rate the RTSP server pulls them, naturally gated by the latest
 * ISP frame's availability.
 *
 * Buffer ownership:
 *   ISP Ch0 ping-pong YUV422 frames live in PSRAM (2x VGA = ~600 KB).
 *   The encode-done IRQ stores the latest completed buffer addr in
 *   g_yuv_ready. encode_one_frame() copies that frame's address into
 *   MJPEG, kicks SW frame encode, blocks on the encode-done sem, then
 *   memcpy's the JPEG out into a stable copy buffer. The copy buffer
 *   is what the RTSP send path reads from — guarantees the data is
 *   stable while the next encode runs.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_RTSP_ISP_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "compiler/common.h"

#include "sccb.h"
#include "camera.h"
#include "sensor.h"
#include "hal_isp.h"
#include "hal_mjpeg.h"
#include "drv_glb.h"
#include "drv_clock.h"
#include "drv_gpio.h"
#include "drv_mm_misc.h"
#include "drv_mjpeg.h"
#include "ipro7.h"
#include "system_soc.h"
#include "img_proc.h"

#include "librtspsrv.h"
#include "rtsp_isp_source.h"

#define TAG "RTSP_ISP"

/* ---------------- Hardware pin map (EVB only) ----------------
 * EVB camera daughter board: CAMCLK on GPIO 41, sensor RST on GPIO 34,
 * sensor PWDN on GPIO 35. DVP data lines are GPIO 0..7 + 38..40 (set
 * to GPIO_FUN_CAM at runtime — overrides JTAG default on 0/1/2). */
#if defined(CONFIG_EVB)
#define SNS_CAMCLK_PIN      GLB_GPIO_PIN_41
#define SNS_RST_PIN         GLB_GPIO_PIN_34
#define SNS_PWDN_PIN        GLB_GPIO_PIN_35
#define SNS_HAS_RST_PWDN    1
#elif defined(CONFIG_DVK)
#define SNS_CAMCLK_PIN      GLB_GPIO_PIN_37
#define SNS_HAS_RST_PWDN    0
#else
#error "RTSP_ISP_SOURCE: no board variant selected (need CONFIG_EVB or CONFIG_DVK)"
#endif

/* ---------------- Pipeline shape ---------------- */
#define CROP_WIDTH          1280
#define CROP_HEIGHT         720
#define OUT_WIDTH           640
#define OUT_HEIGHT          480
#define JPEG_QUALITY        MJPEG_DEFAULT_QUALITY  /* 50 */
#define JPG_OUT_BUF_SIZE    (256 * 1024)  /* VGA q=50 ~10-15 KB; 2× HAL slots fit easily */
#define CONVERGE_FRAMES     8

/* ISP Ch0 ring-line buffer height. ISP outputs 16 lines at a time into a
 * single small PSRAM buffer, MJPEG HW reads from the same buffer through
 * DVP2AXI ports. Massive PSRAM bandwidth saving vs full-frame ping-pong
 * (1280×720×2 = 1.84 MB/frame × 2 buffers = 3.69 MB → 1280×16×2 = 40 KB
 * one-shot). Any value HW supports works (8/16/32 typical); 16 is what
 * the other IPRO7 camera pipelines use. */
#define RING_LINE           16

#define YUV422_FRAME_SIZE(X, Y)   ((X) * (Y) * 2)
#define YUV400_FRAME_SIZE(X, Y)   ((X) * (Y))

#define ENC_TIMEOUT_MS      3000

/* ---------------- State ---------------- */

static bool g_installed;
static const rt_camera_desc *g_cam_desc;

/* ISP Ch0 YUV ring-line buffer (PSRAM). pp_num=1, ring_line=16. ISP and
 * MJPEG HW share this buffer via DVP2AXI port routing — no CPU touch. */
static uint8_t *g_yuv_ring;

/* MJPEG output buffer (PSRAM). HAL transparently splits this into 2 slots
 * (MJPEG_OUT_INFO_MAX_VALID_CNT in hal_mjpeg.c) to handle the IPRO7 input
 * ring "stuck at 255 frame" workaround: HW is stop+restart-pointed at the
 * other half on each NORMAL IRQ, giving us natural double buffering. */
static uint32_t g_jenc_addr;
static uint8_t g_jpg_header[800];
static uint32_t g_jpg_header_len;

/* RTP pacing */
static uint32_t g_frm_size;
static uint32_t g_frm_rtp_ts;
static TickType_t g_frm_last_tick;

/* Convergence + counters */
static volatile uint8_t g_converge_cnt;
static volatile uint32_t g_dbg_isp_ch0_frames;
static volatile uint32_t g_dbg_isp_fifo_overwrite;
static volatile uint32_t g_dbg_isp_vsync_err;
static volatile uint32_t g_dbg_isp_hsync_err;
static volatile uint32_t g_dbg_seof_cnt;
static volatile uint32_t g_dbg_jenc_done;       /* MJPEG NORMAL IRQs */
static volatile uint32_t g_dbg_jenc_err;        /* CAM/MEM/FRM overwrite */
static uint32_t g_dbg_encode_ok;
static uint32_t g_dbg_encode_fail;
static uint32_t g_dbg_no_frame_yet;

/* Stats-ready callback registered by img_proc.c. The IPRO ISP firmware
 * library calls this on each SEOF event so user code (us) can drive AE
 * stats updates. We just forward to whatever cb img_proc registered. */
static ipro_isp_stats_ready_cb g_stats_cb;

int ipro_isp_stats_ready_config(ipro_isp_stats_ready_cb cb, void *usrdata)
{
    (void)usrdata;
    g_stats_cb = cb;
    return 0;
}

/* ---------------- ISRs ---------------- */

/* HW-auto MJPEG callback. The HAL workaround already records (addr, len)
 * into a 2-slot internal ring on each NORMAL IRQ — see hal_mjpeg.c
 * mjpeg_irq_handler around line 280. We only need to count events here.
 * The CAM/MEM/FRM_OVERWRITE codes mean ISP→MJPEG / MJPEG→memory was
 * faster than we drained, which shows up as dropped frames downstream. */
static void isp_mjpeg_isr_cb(uint32_t event)
{
    switch (event) {
    case MJENC_HAL_INT_NORMAL:
        g_dbg_jenc_done++;
        break;
    case MJENC_HAL_INT_IDLE:
        /* HAL restarts HW for the next frame here; no action for us. */
        break;
    case MJENC_HAL_INT_ERR_CAM_OVERWRITE:
    case MJENC_HAL_INT_ERR_MEM_OVERWRITE:
    case MJENC_HAL_INT_ERR_FRAME_OVERWRITE:
        g_dbg_jenc_err++;
        break;
    default:
        break;
    }
}

static void isp_out_isr_cb(isp_out_frame_info_t info)
{
    if (info.event == ISP_HAL_INT_FIFO_OVERWRITE) {
        g_dbg_isp_fifo_overwrite++;
        return;
    }
    if (info.event == ISP_HAL_INT_VSYNC_CNT_ERROR) {
        g_dbg_isp_vsync_err++;
        return;
    }
    if (info.event == ISP_HAL_INT_HSYNC_CNT_ERROR) {
        g_dbg_isp_hsync_err++;
        return;
    }
    if (info.event != ISP_HAL_INT_NORMAL)
        return;

    /* Ring-line mode fires NORMAL per ring fill, not per frame; treat as
     * a liveness counter only. JPEG frame events come via isp_mjpeg_isr_cb. */
    if (info.out_port == ISP_CHANNEL_0)
        g_dbg_isp_ch0_frames++;
}

static void isp_seof_isr(void)
{
    MM_MISC_SEOF_TYPE_E status;
    MM_MISC_Get_SEOF_TypeStatus(MM_MISC_SEOF0, &status);

    if (status.BF.lineCnt) {
        g_dbg_seof_cnt++;
        if (g_converge_cnt > 0)
            g_converge_cnt--;
        if (g_stats_cb)
            g_stats_cb(NULL);
    }
    MM_MISC_Clr_SEOF_TypeStatus(MM_MISC_SEOF0, status);
}

/* ---------------- HW bring-up ---------------- */

static void isp_dvp_pin_mux(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_NONE,
        .drive    = 0,
        .smtCtrl  = 1,
    };
    static const uint8_t pins[] = {
        GLB_GPIO_PIN_0, GLB_GPIO_PIN_1, GLB_GPIO_PIN_2, GLB_GPIO_PIN_3,
        GLB_GPIO_PIN_4, GLB_GPIO_PIN_5, GLB_GPIO_PIN_6, GLB_GPIO_PIN_7,
        GLB_GPIO_PIN_38, GLB_GPIO_PIN_39, GLB_GPIO_PIN_40,
    };
    for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        cfg.gpioPin = pins[i];
        cfg.gpioFun = GPIO_FUN_CAM;
        GLB_GPIO_Init(&cfg);
    }
}

static void isp_enable_sensor(int enable)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_NONE,
        .drive    = 3,
        .smtCtrl  = 1,
        .gpioPin  = SNS_CAMCLK_PIN,
        .gpioFun  = GPIO_FUN_CAM_MISC,
    };
    GLB_GPIO_Init(&cfg);
    GLB_Set_CAM_CLK(DISABLE, GLB_CAM_CLK_PLL48M, 1);

#if SNS_HAS_RST_PWDN
    cfg.gpioMode = GPIO_MODE_OUTPUT;
    cfg.pullType = GPIO_PULL_UP;
    cfg.gpioFun  = GPIO_FUN_GPIO;

    cfg.gpioPin = SNS_PWDN_PIN;
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_Write(SNS_PWDN_PIN, 0);
    GLB_GPIO_OUTPUT_Enable(SNS_PWDN_PIN);

    cfg.gpioPin = SNS_RST_PIN;
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_Write(SNS_RST_PIN, 0);
    GLB_GPIO_OUTPUT_Enable(SNS_RST_PIN);
#endif

    if (enable) {
        GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_PLL48M, 1);
#if SNS_HAS_RST_PWDN
        vTaskDelay(1);
        GLB_GPIO_Write(SNS_PWDN_PIN, 1);
        GLB_GPIO_Write(SNS_RST_PIN, 1);
#endif
        vTaskDelay(12);
    }
}

static int isp_pipeline_init(void)
{
    isp_hal_cfg_t isp_cfg = {0};
    isp_hal_vin_cfg_t vin_cfg = {0};

    g_converge_cnt = CONVERGE_FRAMES;
    g_dbg_jenc_done = 0;
    g_dbg_jenc_err = 0;

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
    GLB_Set_I2C_CLK(ENABLE, GLB_I2C_CLK_SRC_BCLK, 0);

    SCCB_Init();
    isp_enable_sensor(1);
    isp_dvp_pin_mux();

    for (int retry = 0; retry < 10; retry++) {
        g_cam_desc = camera_module_init();
        if (g_cam_desc)
            break;
        printf("[%s] sensor probe retry %d/10\r\n", TAG, retry + 1);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    if (!g_cam_desc) {
        printf("[%s] sensor probe failed\r\n", TAG);
        return -1;
    }
    camera_module_start(g_cam_desc);
    imgproc_pir_init(g_cam_desc, false);

    /* Ch0 ring-line buffer (PSRAM): one small block, ISP cycles into it
     * line-by-line at the sensor rate while MJPEG HW reads from the same
     * region through DVP2AXI. Total = OUT_WIDTH × RING_LINE × 2 bpp. */
    if (!g_yuv_ring) {
        size_t ring_sz = (size_t)OUT_WIDTH * RING_LINE * 2;
        g_yuv_ring = (uint8_t *)pvPortMalloc(ring_sz);
        if (!g_yuv_ring) {
            printf("[%s] yuv ring alloc fail (%zu B)\r\n", TAG, ring_sz);
            return -1;
        }
    }
    /* MJPEG output buffer (PSRAM). HAL splits into 2 internal slots so
     * we get natural double buffering. Caller-side "frame ring" is just
     * mjpeg_get_one_frame / mjpeg_drop_one_frame. */
    if (!g_jenc_addr) {
        g_jenc_addr = (uint32_t)pvPortMalloc(JPG_OUT_BUF_SIZE);
        if (!g_jenc_addr) {
            printf("[%s] jenc buf alloc fail\r\n", TAG);
            return -1;
        }
    }

    /* ISP — Ch0 YUV422 ring-line, src=30 dst=fps from RTSP config */
    isp_cfg.isp_src = ISP_HAL_SRC_DVP_AS;
    isp_cfg.in_size.width  = g_cam_desc->width;
    isp_cfg.in_size.height = g_cam_desc->height;
    isp_cfg.src_cfg.dvp_input.hsync = ISP_HAL_DVP_ACTIVE_HIGH;
    isp_cfg.src_cfg.dvp_input.vsync = ISP_HAL_DVP_ACTIVE_HIGH;
    isp_cfg.src_cfg.dvp_input.order = ISP_HAL_DATA_BYTE_LOWER;
    isp_cfg.src_cfg.dvp_input.pclk  = g_cam_desc->pclk;
    isp_cfg.src_cfg.dvp_input.totalHPixels = g_cam_desc->total_width;
    isp_cfg.glb_crop.width  = CROP_WIDTH;
    isp_cfg.glb_crop.height = CROP_HEIGHT;

    isp_cfg.out_ch_0.enable = 1;
    isp_cfg.out_ch_0.scaler_cfg.in.width  = CROP_WIDTH;
    isp_cfg.out_ch_0.scaler_cfg.in.height = CROP_HEIGHT;
    isp_cfg.out_ch_0.scaler_cfg.out.width  = OUT_WIDTH;
    isp_cfg.out_ch_0.scaler_cfg.out.height = OUT_HEIGHT;
    isp_cfg.out_ch_0.format = ISP_HAL_OUT_FMT0_YUYV;
    isp_cfg.out_ch_0.ring_line = RING_LINE;     /* line-ring mode */
    isp_cfg.out_ch_0.chn_common.fr_ctrl.src_fps = 30;
    isp_cfg.out_ch_0.chn_common.fr_ctrl.dst_fps =
        CONFIG_RTSP_DEFAULT_VIDEO_FPS;
    isp_cfg.out_ch_0.chn_common.addr[0] = (uint32_t)g_yuv_ring;
    isp_cfg.out_ch_0.chn_common.pp_num  = 1;     /* single ring slot */
    isp_cfg.out_ch_0.chn_common.out_crop.width  = OUT_WIDTH;
    isp_cfg.out_ch_0.chn_common.out_crop.height = OUT_HEIGHT;

    isp_cfg.out_ch_1.enable = 0;                 /* RTSP doesn't need Ch1 */

    isp_cfg.out_callback = isp_out_isr_cb;
    isp_init(&isp_cfg);

    /* MJPEG — HW auto mode driven by ISP Ch0 line ring. The HAL workaround
     * (_MJPEG_SW_OUT_PING_PONG_MODE_, auto-defined for IPRO7) handles the
     * "input ring stuck at 255 frame" silicon bug by stop+restart-pointing
     * the HW output address to alternating halves of g_jenc_addr on each
     * NORMAL IRQ; mjpeg_get_one_frame() reads the next finished slot.
     *  - hw_mode_sshot_en=0 → continuous (don't stop after one frame)
     *  - hw_mode_swen_en=0  → wait for first frame done before signalling
     *  - output_swap_mode_en=0 → frame mode (HAL provides per-frame ring)
     *  - rows = RING_LINE (ISP feeds 16 lines at a time) */
    struct mjpeg_hal_cfg_t mjpeg_cfg = {0};
    mjpeg_cfg.format        = MJPEG_HAL_FORMAT_YUV422_YUYV;
    mjpeg_cfg.quality       = JPEG_QUALITY;
    mjpeg_cfg.rows          = RING_LINE;
    mjpeg_cfg.resolution_x  = OUT_WIDTH;
    mjpeg_cfg.resolution_y  = OUT_HEIGHT;
    mjpeg_cfg.input_bufaddr0 = (uint32_t)g_yuv_ring;
    /* For YUYV interleaved input, bufaddr1 expectation in IPRO7 HW is
     * the second half-block of the ring used for input ping-pong on the
     * sensor side; the same offset the other IPRO7 camera pipelines use. */
    mjpeg_cfg.input_bufaddr1 = (uint32_t)g_yuv_ring +
                               YUV400_FRAME_SIZE(OUT_WIDTH, RING_LINE);
    mjpeg_cfg.output_bufaddr  = g_jenc_addr;
    mjpeg_cfg.output_bufsize  = JPG_OUT_BUF_SIZE;
    mjpeg_cfg.output_swap_mode_en = 0;
    mjpeg_cfg.hw_mode_swen_en     = 0;
    mjpeg_cfg.hw_mode_sshot_en    = 0;
    mjpeg_cfg.callback = isp_mjpeg_isr_cb;
    mjpeg_init(&mjpeg_cfg);

    g_jpg_header_len = mjpeg_headerCreate(YUV_MODE_422, JPEG_QUALITY,
                                          OUT_WIDTH, OUT_HEIGHT,
                                          g_jpg_header);
    mjpeg_fill_header_tail(g_jpg_header, g_jpg_header_len);

    /* imgproc (AE/AWB) */
    vin_cfg.in_size.width  = isp_cfg.in_size.width;
    vin_cfg.in_size.height = isp_cfg.in_size.height;
    vin_cfg.glb_crop.width  = CROP_WIDTH;
    vin_cfg.glb_crop.height = CROP_HEIGHT;
    memcpy(&vin_cfg.sensor_name, "SC101IOT\0", 10);
    imgproc_init(&vin_cfg);
    imgproc_start_pir();

    /* SEOF interrupt for AE convergence ticks */
    MM_MISC_SEOF_TYPE_E seof_type = {0};
    seof_type.BF.SoF = 1;
    seof_type.BF.EoF = 1;
    seof_type.BF.lineCnt = 1;
    MM_MISC_Set_SEOF_LineCnt(MM_MISC_SEOF0, 100);
    MM_MISC_Set_SEOF_Src(MM_MISC_SEOF0, MM_MISC_SEOF_INTR_SRC_ISP_OUTPUT);
    MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF0, seof_type);
    Interrupt_Handler_Register(ISP_MM_SEOF0_IRQn, isp_seof_isr);
    CPU_Interrupt_Enable(ISP_MM_SEOF0_IRQn);

    /* JPEG src selection: Y from DVP2AXI port A, C from port B (matches
     * how ISP Ch0 routes YUYV onto the two AXI ports). */
    MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_Y, MM_MISC_DVP2AXI_PORT_A);
    MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL_C, MM_MISC_DVP2AXI_PORT_B);

    isp_start();
    mjpeg_start(0);          /* HW auto mode (continuous) */

    printf("[%s] pipeline up (HW auto, line-ring): %dx%d @ %d fps q=%d, "
           "ring=%d lines\r\n", TAG, OUT_WIDTH, OUT_HEIGHT,
           CONFIG_RTSP_DEFAULT_VIDEO_FPS, JPEG_QUALITY, RING_LINE);
    return 0;
}

static void isp_pipeline_deinit(void)
{
#if SNS_HAS_RST_PWDN
    GLB_GPIO_Write(SNS_RST_PIN, 0);
#else
    GLB_Set_CAM_CLK(DISABLE, GLB_CAM_CLK_PLL48M, 1);
#endif

    mjpeg_deinit();
    isp_deinit();

    MM_MISC_SEOF_TYPE_E seof_off = {0};
    MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF0, seof_off);
    CPU_Interrupt_Disable(ISP_MM_SEOF0_IRQn);
    Interrupt_Handler_Register(ISP_MM_SEOF0_IRQn, NULL);

    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_JPG, 1);
    MM_MISC_Set_SW_Reset(MM_MISC_SW_RESET_ID_JPG, 0);
}

/* ---------------- RTSP pull callback ---------------- */
/*
 * HW auto MJPEG runs continuously; HAL maintains an internal 2-slot
 * frame ring. We pop one finished frame per cb call.
 *
 * Buffer lifetime: the slot pointer returned here is one half of
 * g_jenc_addr. HW won't reuse it until two more frames have been
 * encoded into the OTHER half (so we have ~2 × 1/fps = 134 ms at 15 fps
 * before HW comes back). librtspsrv copies the JPEG payload into RTP
 * packets synchronously inside send_mjpeg_frm before returning to its
 * delay loop, so the consumer is done within ~10–25 ms — well within
 * the safety window.
 *
 * mjpeg_drop_one_frame() advances HAL's read_idx; we call it after
 * recording the pointer/size into fi->* so the lib is free to use them.
 */
static int isp_get_frm_cb(struct strm_info *si, struct frm_info *fi)
{
    (void)si;

    /* Pace: librtspsrv polls cb every ~1 ms when no frame is pending. We
     * gate by the configured fps so we don't deliver more than the
     * sender can comfortably push. */
    TickType_t now = xTaskGetTickCount();
    TickType_t min_gap = pdMS_TO_TICKS(1000 / CONFIG_RTSP_DEFAULT_VIDEO_FPS);
    if (g_frm_last_tick != 0 &&
        (TickType_t)(now - g_frm_last_tick) < min_gap)
        return 0;

    /* Wait for AE convergence before exposing junk frames to the client */
    if (g_converge_cnt > 0)
        return 0;

    uint32_t addr = 0, size = 0;
    if (mjpeg_get_one_frame(&addr, &size) != 0 || size == 0) {
        g_dbg_no_frame_yet++;
        return 0;
    }
    if (size > JPG_OUT_BUF_SIZE / 2) {
        /* Frame larger than HAL slot; drop and account. Should never hit
         * with q=50 at this resolution, but guard anyway. */
        mjpeg_drop_one_frame();
        g_dbg_encode_fail++;
        return 0;
    }

    /* HW DMA bypasses cache — flush+invalidate so CPU sees fresh data
     * when send_mjpeg_frm memcpys it out. */
    L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)addr, size);

    /* librtspsrv re-scales frm_info->timestamp by *90 in send_mjpeg_frm
     * to produce 90 kHz RTP units, so the value must be in ms. */
    g_frm_rtp_ts += (1000U / CONFIG_RTSP_DEFAULT_VIDEO_FPS);
    fi->frm_buf   = (char *)addr;
    fi->frm_sz    = (unsigned int)size;
    fi->frm_type  = FRM_TYPE_M;
    fi->timestamp = g_frm_rtp_ts;

    /* Release the slot back to HAL. Safe now: librtspsrv reads this
     * buffer synchronously inside its caller (prefetch_frm), and HW
     * needs ~2 frame intervals before it overwrites this half. */
    mjpeg_drop_one_frame();

    g_frm_last_tick = now;
    g_frm_size = size;
    g_dbg_encode_ok++;
    return (int)size;
}

/* ---------------- Public API ---------------- */

int rtsp_isp_source_install(void)
{
    if (g_installed)
        return 0;

    if (isp_pipeline_init() != 0) {
        printf("[%s] pipeline init failed\r\n", TAG);
        return -1;
    }

    g_frm_last_tick = 0;
    g_frm_rtp_ts = 0;
    set_strm_cb(isp_get_frm_cb);
    g_installed = true;
    printf("[%s] installed\r\n", TAG);
    return 0;
}

void rtsp_isp_source_uninstall(void)
{
    if (!g_installed)
        return;
    set_strm_cb(NULL);
    isp_pipeline_deinit();
    g_installed = false;
    printf("[%s] uninstalled\r\n", TAG);
}

void rtsp_isp_source_stats(void)
{
    printf("rtsp_isp_source:\r\n");
    printf("  installed       = %d\r\n", g_installed);
    printf("  converge_cnt    = %u\r\n", (unsigned)g_converge_cnt);
    printf("  isp_ch0_frames  = %lu\r\n", (unsigned long)g_dbg_isp_ch0_frames);
    printf("  isp_seof_cnt    = %lu\r\n", (unsigned long)g_dbg_seof_cnt);
    printf("  isp_fifo_ovw    = %lu\r\n", (unsigned long)g_dbg_isp_fifo_overwrite);
    printf("  isp_vsync_err   = %lu\r\n", (unsigned long)g_dbg_isp_vsync_err);
    printf("  isp_hsync_err   = %lu\r\n", (unsigned long)g_dbg_isp_hsync_err);
    printf("  jenc_done       = %lu\r\n", (unsigned long)g_dbg_jenc_done);
    printf("  jenc_err        = %lu\r\n", (unsigned long)g_dbg_jenc_err);
    printf("  encode_ok       = %lu (cb-side pulls)\r\n",
           (unsigned long)g_dbg_encode_ok);
    printf("  encode_fail     = %lu\r\n", (unsigned long)g_dbg_encode_fail);
    printf("  no_frame_yet    = %lu (cb saw empty ring)\r\n",
           (unsigned long)g_dbg_no_frame_yet);
    printf("  last_jpg_size   = %lu\r\n", (unsigned long)g_frm_size);
}

#endif /* CONFIG_RTSP_ISP_SOURCE */
