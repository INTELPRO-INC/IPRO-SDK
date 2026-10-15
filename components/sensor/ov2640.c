/**
 * @file ov2640.c
 * @brief OmniVision OV2640 (2 MP) DVP sensor driver, YUV422 output.
 *
 * Two sources, used for what each has proven:
 *
 *   - The UXGA initialisation list is the one Bouffalo ships for this very
 *     DVP front-end (bouffalo_sdk bl616d, bsp/common/image_sensor/ov2640.h,
 *     Apache-2.0, Copyright (c) 2025 Bouffalolab): 1600x1200 YUV422 YUYV,
 *     CLKRC 0x00 (24 MHz XCLK, no doubler), DSP PCLK auto (0xD3 = 0x82),
 *     stated PCLK 72 MHz. That settles sync polarity, byte order and the
 *     clock for the UXGA base on this silicon.
 *   - The DSP window / zoom arithmetic and the SVGA base table come from
 *     esp32-camera (sensors/ov2640.c, MIT, Copyright (c) 2013/2014 Ibrahim
 *     Abdelkader; ov2640_settings.h, Apache-2.0, Espressif). The IPRO7 ISP
 *     takes at most 720p, so the UXGA stream is cropped to a 1600x900 (16:9)
 *     window and scaled to 1280x720 on the sensor before it reaches the DVP.
 *
 * Building with -DOV2640_DEFAULT_VGA uses the SVGA base (800x600, 30 fps
 * class) scaled to 640x480 instead - the most-travelled OV2640 path.
 *
 * AEC/AGC/AWB run on the sensor; the rt_camera_desc AE/AWB hooks are no-ops
 * (see ov5640.c). RAW and JPEG are not offered; the SoC's MJPEG encoder does
 * compression.
 *
 * SCCB: 7-bit address 0x30, 8-bit registers behind a bank-select register
 * (0xFF: 0 = DSP bank, 1 = sensor bank). XCLK: 24 MHz.
 *
 * Clock reported to hal_isp: the vendor's 72 MHz UXGA figure. Under-stating
 * it is NOT safe: hal_isp sizes the DVP_AS FIFO threshold as
 * width x (1 - (pclk/2) / BCLK) + 10 against a 1024-entry FIFO and the 96 MHz
 * BCLK, so at 1280 wide any pclk below ~40 MHz pushes the threshold past the
 * FIFO, the timing generator never starts and the ISP reports 0 frames and
 * 0 errors (`[WARN] dvp as threshold over 1024` at isp_init). That is what the
 * OV5640 did on the IPRO7AI Reef until its PCLK was raised (see ov5640.c).
 * Not yet measured on an OV2640: check the warning, then `cam pins` (HREF
 * duty) / `cam status` (fps) on the Reef demo. The 640x480 SVGA build is safe
 * at any pclk (threshold <= 650).
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "sensor.h"
#include "sccb.h"
#include "camera.h"

#define OV2640_ADDR         0x30
#define OV2640_PID          0x26        /* REG_PID; VER is 0x41 or 0x42 */

/* DSP bank */
#define R_BYPASS            0x05
#define QS                  0x44
#define CTRLI               0x50
#define HSIZE               0x51
#define VSIZE               0x52
#define XOFFL               0x53
#define YOFFL               0x54
#define VHYX                0x55
#define TEST                0x57
#define ZMOW                0x5A
#define ZMOH                0x5B
#define ZMHH                0x5C
#define BPADDR              0x7C
#define BPDATA              0x7D
#define CTRL2               0x86
#define CTRL3               0x87
#define SIZEL               0x8C
#define HSIZE8              0xC0
#define VSIZE8              0xC1
#define CTRL0               0xC2
#define CTRL1               0xC3
#define R_DVP_SP            0xD3
#define IMAGE_MODE          0xDA
#define RESET               0xE0
#define MC_BIST             0xF9
#define BANK_SEL            0xFF

#define BANK_DSP            0
#define BANK_SENSOR         1

#define CTRLI_LP_DP         0x80
#define CTRL2_DCW_EN        0x20
#define CTRL3_WPC_EN        0x40
#define R_DVP_SP_AUTO_MODE  0x80
#define R_BYPASS_DSP_EN     0x00
#define R_BYPASS_DSP_BYPAS  0x01
#define IMAGE_MODE_YUV422   0x00
#define RESET_JPEG          0x10
#define RESET_DVP           0x04
#define MC_BIST_RESET       0x80
#define MC_BIST_BOOT_ROM_SEL 0x40

/* Sensor bank */
#define COM1                0x03
#define REG04               0x04
#define COM2                0x09
#define REG_PID             0x0A
#define REG_VER             0x0B
#define COM4                0x0D
#define CLKRC               0x11
#define COM7                0x12
#define COM8                0x13
#define COM9                0x14
#define HSTART              0x17
#define HSTOP               0x18
#define VSTART              0x19
#define VSTOP               0x1A
#define AEW                 0x24
#define AEB                 0x25
#define VV                  0x26
#define REG32               0x32
#define ARCOM2              0x34
#define BD50                0x4F
#define BD60                0x50
#define HISTO_LOW           0x61
#define HISTO_HIGH          0x62

#define REG04_DEFAULT       0x28
#define COM2_OUT_DRIVE_3x   0x02
#define COM7_SRST           0x80
#define COM7_RES_UXGA       0x00
#define COM7_RES_SVGA       0x40
#define REG32_UXGA          0x36
#define REG32_SVGA          0x09

/* ---- Output mode ---- */
#ifdef OV2640_DEFAULT_VGA
#define OV2640_OUT_W        640
#define OV2640_OUT_H        480
#define OV2640_BASE_SVGA    1
/* 4:3 window of the SVGA base: 800x600, no offset. */
#define OV2640_WIN_OX       0
#define OV2640_WIN_OY       0
#define OV2640_WIN_MX       800
#define OV2640_WIN_MY       600
#define OV2640_HTS_NOMINAL  1190        /* SVGA line length, pixels */
#define OV2640_VTS_NOMINAL  620
#else
#define OV2640_OUT_W        1280
#define OV2640_OUT_H        720
#define OV2640_BASE_SVGA    0
/* 16:9 window of the UXGA base (esp32-camera ratio_table): 1600x900 at
 * (0,150). */
#define OV2640_WIN_OX       0
#define OV2640_WIN_OY       150
#define OV2640_WIN_MX       1600
#define OV2640_WIN_MY       900
#define OV2640_HTS_NOMINAL  1922        /* UXGA line, pixels (vendor h_blank 0x6A gives 1706; larger is the safe side) */
#define OV2640_VTS_NOMINAL  1248
#endif

/* DVP PCLK reported to hal_isp - the vendor's UXGA figure, see the header. */
#define OV2640_DVP_PCLK_HZ  72000000u

/*
 * UXGA base: 1600x1200 YUV422 YUYV with 3A enabled, verbatim from Bouffalo's
 * bsp/common/image_sensor/ov2640.h (the register tables there are
 * bank-select interleaved just like this). Ends with the DSP enabled and a
 * full-frame window; ov2640_set_window() re-windows it.
 */
static const uint8_t ov2640_init_uxga[][2] = {
    {BANK_SEL, BANK_DSP},
    {0x2c, 0xff}, {0x2e, 0xdf},
    {BANK_SEL, BANK_SENSOR},
    {0x3c, 0x32},
    {CLKRC, 0x00},                          /* 24 MHz XCLK straight through */
    {COM2, COM2_OUT_DRIVE_3x},
    {COM8, 0xe5},                           /* banding, AGC, AEC on */
    {COM9, 0x48},                           /* AGC ceiling 8x */
    {0x2c, 0x0c}, {0x33, 0x78}, {0x3a, 0x33}, {0x3b, 0xfB}, {0x3e, 0x00},
    {0x43, 0x11}, {0x16, 0x10}, {0x39, 0x92}, {0x35, 0xda}, {0x22, 0x1a},
    {0x37, 0xc3}, {0x23, 0x00}, {ARCOM2, 0xc0}, {0x36, 0x1a}, {0x06, 0x88},
    {0x07, 0xc0}, {COM4, 0x87}, {0x0e, 0x41}, {0x4c, 0x00}, {0x48, 0x00},
    {0x5B, 0x00}, {0x42, 0x03}, {0x4a, 0x81}, {0x21, 0x99},
    {AEW, 0x40}, {AEB, 0x38}, {VV, 0x82}, {0x5c, 0x00}, {0x63, 0x00},
    {0x46, 0x00}, {0x0c, 0x3c}, {HISTO_LOW, 0x70}, {HISTO_HIGH, 0x80},
    {0x7c, 0x05}, {0x20, 0x80}, {0x28, 0x30}, {0x6c, 0x00}, {0x6d, 0x80},
    {0x6e, 0x00}, {0x70, 0x02}, {0x71, 0x94}, {0x73, 0xc1}, {0x3d, 0x34},
    {0x5a, 0x57},
    {COM7, COM7_RES_UXGA},                  /* 1600x1200 */
    {HSTART, 0x11}, {HSTOP, 0x75}, {VSTART, 0x01}, {VSTOP, 0x97},
    {REG32, REG32_UXGA}, {COM1, 0x0f}, {0x37, 0x40}, {BD50, 0xca},
    {BD60, 0xa8}, {0x5a, 0x23}, {0x6d, 0x00}, {0x6d, 0x38},
    {BANK_SEL, BANK_DSP},
    {0xe5, 0x7f},
    {MC_BIST, 0xc0},
    {0x41, 0x24},
    {RESET, 0x14},                          /* JPEG + DVP held in reset */
    {0x76, 0xff}, {0x33, 0xa0}, {0x42, 0x20}, {0x43, 0x18}, {0x4c, 0x00},
    {CTRL3, 0xd5}, {0x88, 0x3f}, {0xd7, 0x03}, {0xd9, 0x10},
    {R_DVP_SP, R_DVP_SP_AUTO_MODE | 0x02},  /* PCLK auto */
    {0xc8, 0x08}, {0xc9, 0x80},
    {BPADDR, 0x00}, {BPDATA, 0x00}, {BPADDR, 0x03}, {BPDATA, 0x48},
    {BPDATA, 0x48}, {BPADDR, 0x08}, {BPDATA, 0x20}, {BPDATA, 0x10},
    {BPDATA, 0x0e},
    {0x90, 0x00}, {0x91, 0x0e}, {0x91, 0x1a}, {0x91, 0x31}, {0x91, 0x5a},
    {0x91, 0x69}, {0x91, 0x75}, {0x91, 0x7e}, {0x91, 0x88}, {0x91, 0x8f},
    {0x91, 0x96}, {0x91, 0xa3}, {0x91, 0xaf}, {0x91, 0xc4}, {0x91, 0xd7},
    {0x91, 0xe8}, {0x91, 0x20},
    {0x92, 0x00}, {0x93, 0x06}, {0x93, 0xe3}, {0x93, 0x05}, {0x93, 0x05},
    {0x93, 0x00}, {0x93, 0x04}, {0x93, 0x00}, {0x93, 0x00}, {0x93, 0x00},
    {0x93, 0x00}, {0x93, 0x00}, {0x93, 0x00}, {0x93, 0x00},
    {0x96, 0x00}, {0x97, 0x08}, {0x97, 0x19}, {0x97, 0x02}, {0x97, 0x0c},
    {0x97, 0x24}, {0x97, 0x30}, {0x97, 0x28}, {0x97, 0x26}, {0x97, 0x02},
    {0x97, 0x98}, {0x97, 0x80}, {0x97, 0x00}, {0x97, 0x00},
    {CTRL1, 0xef}, {0xa4, 0x00}, {0xa8, 0x00}, {0xc5, 0x11}, {0xc6, 0x51},
    {0xbf, 0x80}, {0xc7, 0x10}, {0xb6, 0x66}, {0xb8, 0xA5}, {0xb7, 0x64},
    {0xb9, 0x7C}, {0xb3, 0xaf}, {0xb4, 0x97}, {0xb5, 0xFF}, {0xb0, 0xC5},
    {0xb1, 0x94}, {0xb2, 0x0f}, {0xc4, 0x5c},
    {HSIZE8, 0xc8}, {VSIZE8, 0x96}, {SIZEL, 0x00},   /* sensor 1600x1200 */
    {CTRL2, 0x3d}, {CTRLI, 0x00},
    {HSIZE, 0x90}, {VSIZE, 0x2c}, {XOFFL, 0x00}, {YOFFL, 0x00},
    {VHYX, 0x88},                                     /* window 1600x1200 */
    {ZMOW, 0x90}, {ZMOH, 0x2C}, {ZMHH, 0x05},         /* output 1600x1200 */
    {CTRL1, 0xed}, {0x7f, 0x00},
    {IMAGE_MODE, IMAGE_MODE_YUV422},                  /* YUYV */
    {0xe5, 0x1f}, {0xe1, 0x67},
    {RESET, 0x00},
    {0xdd, 0x7f},
    {R_BYPASS, R_BYPASS_DSP_EN},
    {0, 0},
};

static const uint8_t ov2640_settings_to_svga[][2] = {
    {BANK_SEL, BANK_SENSOR},
    {COM7, COM7_RES_SVGA},
    {COM1, 0x0A}, {REG32, REG32_SVGA},
    {HSTART, 0x11}, {HSTOP, 0x43}, {VSTART, 0x00}, {VSTOP, 0x4b},
    {0x37, 0xc0}, {BD50, 0xca}, {BD60, 0xa8}, {0x5a, 0x23}, {0x6d, 0x00},
    {0x3d, 0x38}, {0x39, 0x92}, {0x35, 0xda}, {0x22, 0x1a}, {0x37, 0xc3},
    {0x23, 0x00}, {ARCOM2, 0xc0}, {0x06, 0x88}, {0x07, 0xc0}, {COM4, 0x87},
    {0x0e, 0x41}, {0x42, 0x03}, {0x4c, 0x00},
    {BANK_SEL, BANK_DSP},
    {RESET, RESET_DVP},
    {HSIZE8, 0x64}, {VSIZE8, 0x4B}, {SIZEL, 0x00},
    {HSIZE, 0xC8}, {VSIZE, 0x96}, {XOFFL, 0x00}, {YOFFL, 0x00},
    {VHYX, 0x00}, {TEST, 0x00},
    {CTRL2, CTRL2_DCW_EN | 0x1D},
    {CTRLI, CTRLI_LP_DP | 0x00},
    {0, 0},
};

static const uint8_t ov2640_settings_yuv422[][2] = {
    {BANK_SEL, BANK_DSP},
    {RESET, RESET_DVP},
    {IMAGE_MODE, IMAGE_MODE_YUV422},   /* YUYV, Y first */
    {0xD7, 0x01},
    {0xE1, 0x67},
    {RESET, 0x00},
    {0, 0},
};

/* ---------------------------------------------------------------------------
 * Register access
 * -------------------------------------------------------------------------*/

static int ov2640_wr(uint8_t bank, uint8_t reg, uint8_t val)
{
    uint8_t b = bank;
    if (SCCB_Write(OV2640_ADDR, BANK_SEL, &b) != 0) {
        return -1;
    }
    return SCCB_Write(OV2640_ADDR, reg, &val);
}

static int ov2640_rd(uint8_t bank, uint8_t reg, uint8_t *val)
{
    uint8_t b = bank;
    if (SCCB_Write(OV2640_ADDR, BANK_SEL, &b) != 0) {
        return -1;
    }
    return SCCB_Read(OV2640_ADDR, reg, val, 1);
}

/* Tables carry their own BANK_SEL entries and end with {0, 0}. */
static int ov2640_wr_table(const uint8_t (*regs)[2])
{
    for (unsigned i = 0; !(regs[i][0] == 0 && regs[i][1] == 0); i++) {
        uint8_t v = regs[i][1];
        if (SCCB_Write(OV2640_ADDR, regs[i][0], &v) != 0) {
            printf("[ov2640] write 0x%02x failed\r\n", regs[i][0]);
            return -1;
        }
    }
    return 0;
}

/* ---------------------------------------------------------------------------
 * Bring-up
 * -------------------------------------------------------------------------*/

/* Window (max_x x max_y at offset) and DSP zoom to w x h, esp32-camera
 * set_window(). Sizes are in units of 4 pixels in the DSP registers. */
static int ov2640_set_window(int base_svga, int offset_x, int offset_y,
                             int max_x, int max_y, int w, int h)
{
    uint8_t win_regs[][2] = {
        {BANK_SEL, BANK_DSP},
        {HSIZE, (uint8_t)((max_x / 4) & 0xFF)},
        {VSIZE, (uint8_t)((max_y / 4) & 0xFF)},
        {XOFFL, (uint8_t)(offset_x & 0xFF)},
        {YOFFL, (uint8_t)(offset_y & 0xFF)},
        {VHYX, (uint8_t)((((max_y / 4) >> 1) & 0x80) | ((offset_y >> 4) & 0x70) |
                         (((max_x / 4) >> 5) & 0x08) | ((offset_x >> 8) & 0x07))},
        {TEST, (uint8_t)(((max_x / 4) >> 2) & 0x80)},
        {ZMOW, (uint8_t)((w / 4) & 0xFF)},
        {ZMOH, (uint8_t)((h / 4) & 0xFF)},
        {ZMHH, (uint8_t)((((h / 4) >> 6) & 0x04) | (((w / 4) >> 8) & 0x03))},
        {0, 0},
    };
    /* UXGA base: the vendor list already holds CLKRC 0x00 / R_DVP_SP auto, so
     * only the window moves. SVGA base: esp32-camera's mode table, with its
     * clocking for a non-ESP32 host (CLKRC 2x + /8, PCLK auto /8). */
    int rc = 0;

    rc |= ov2640_wr(BANK_DSP, R_BYPASS, R_BYPASS_DSP_BYPAS);
    if (base_svga) {
        rc |= ov2640_wr_table(ov2640_settings_to_svga);
        rc |= ov2640_wr(BANK_SENSOR, CLKRC, 0x80 | 0x07);
        rc |= ov2640_wr(BANK_DSP, R_DVP_SP, R_DVP_SP_AUTO_MODE | 8);
    }
    rc |= ov2640_wr_table((const uint8_t (*)[2])win_regs);
    rc |= ov2640_wr(BANK_DSP, R_BYPASS, R_BYPASS_DSP_EN);
    vTaskDelay(pdMS_TO_TICKS(10));
    return rc ? -1 : 0;
}

static int ov2640_probe(void)
{
    uint8_t pid = 0, ver = 0;

    if (ov2640_rd(BANK_SENSOR, REG_PID, &pid) != 0 || ov2640_rd(BANK_SENSOR, REG_VER, &ver) != 0) {
        printf("[ov2640] SCCB read failed\r\n");
        return -1;
    }
    printf("[ov2640] pid 0x%02x ver 0x%02x\r\n", pid, ver);
    return (pid == OV2640_PID) ? 0 : -1;
}

static int ov2640_reset(void)
{
    if (ov2640_wr(BANK_SENSOR, COM7, COM7_SRST) != 0) {
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    return ov2640_wr_table(ov2640_init_uxga);
}

static int ov2640_setFMT(void)
{
    if (ov2640_set_window(OV2640_BASE_SVGA, OV2640_WIN_OX, OV2640_WIN_OY,
                          OV2640_WIN_MX, OV2640_WIN_MY, OV2640_OUT_W, OV2640_OUT_H) != 0) {
        return -1;
    }
    /* Pixel format has to be re-applied after a window change. */
    if (ov2640_wr_table(ov2640_settings_yuv422) != 0) {
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    printf("[ov2640] YUV422 %ux%u from the %s base (window %ux%u at %u,%u)\r\n",
           OV2640_OUT_W, OV2640_OUT_H, OV2640_BASE_SVGA ? "SVGA" : "UXGA",
           OV2640_WIN_MX, OV2640_WIN_MY, OV2640_WIN_OX, OV2640_WIN_OY);
#if !OV2640_BASE_SVGA
    printf("[ov2640] reporting DVP PCLK %u Hz (vendor UXGA figure, not measured on "
           "this host yet) - if 0 frames or FIFO errors, measure it, see ov2640.c\r\n",
           (unsigned)OV2640_DVP_PCLK_HZ);
#endif
    return 0;
}

/* ---------------------------------------------------------------------------
 * rt_camera_desc hooks - AE/AWB stay on the sensor, see ov5640.c.
 * -------------------------------------------------------------------------*/

static int ov2640_set_case(uint8_t case_id)
{
    (void)case_id;
    return 0;
}

static int ov2640_set_awb_gain(uint8_t enable, int r_gain, int b_gain)
{
    (void)enable; (void)r_gain; (void)b_gain;
    return 0;
}

static int ov2640_get_awb_register(uint8_t case_id, int *value)
{
    (void)case_id;
    if (value) *value = 0;
    return 0;
}

static int ov2640_set_ae_expo_gain(uint8_t enable, int gain_db)
{
    (void)enable; (void)gain_db;
    return 0;
}

static int ov2640_get_ae_register(uint8_t case_id, int *value)
{
    (void)case_id;
    if (value) *value = 0;
    return 0;
}

/* addr: bit 8 selects the bank (0x1xx = sensor bank), low byte = register. */
static int ov2640_set_register(int addr, int value)
{
    return ov2640_wr((addr & 0x100) ? BANK_SENSOR : BANK_DSP, (uint8_t)addr, (uint8_t)value);
}

static int ov2640_get_register(int addr, int *value)
{
    uint8_t v = 0;
    int rc = ov2640_rd((addr & 0x100) ? BANK_SENSOR : BANK_DSP, (uint8_t)addr, &v);
    if (value) *value = v;
    return rc;
}

const rt_camera_desc __rt_camera_ov2640_desc __attribute__((section(".camera_desc"))) =
{
    .name             = "__rti_name_ov2640",
    .addr             = OV2640_ADDR,
    .frm_vld_high     = 1,
    .probe            = ov2640_probe,
    .reset            = ov2640_reset,
    .setFMT           = ov2640_setFMT,
    .set_case         = ov2640_set_case,
    .set_awb_gain     = ov2640_set_awb_gain,
    .get_awb_register = ov2640_get_awb_register,
    .set_ae_expo_gain = ov2640_set_ae_expo_gain,
    .get_ae_register  = ov2640_get_ae_register,
    .set_register     = ov2640_set_register,
    .get_register     = ov2640_get_register,
    .width            = OV2640_OUT_W,
    .height           = OV2640_OUT_H,
    .pclk             = OV2640_DVP_PCLK_HZ,
    .total_width      = OV2640_HTS_NOMINAL,
    .total_height     = OV2640_VTS_NOMINAL,
};
