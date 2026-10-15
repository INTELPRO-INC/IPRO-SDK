/**
 * @file ov5640.c
 * @brief OmniVision OV5640 (5 MP) DVP sensor driver, YUV422 output.
 *
 * Register tables and the PLL / windowing arithmetic are ported from
 * esp32-camera (sensors/ov5640.c, MIT, Copyright (c) 2013/2014 Ibrahim
 * Abdelkader), trimmed to what the IPRO7 DVP path needs:
 *
 *   - The IPRO7 ISP is YUV-domain only (no demosaic) and takes at most 720p,
 *     so the sensor runs in YUV422 (YUYV) and the 2592x1944 array is
 *     2x-binned through a 16:9 window down to 1280x720. RAW and JPEG modes
 *     are deliberately absent.
 *   - AEC / AGC / AWB run on the sensor's own ISP. The rt_camera_desc AE/AWB
 *     hooks are therefore no-ops that return success: apps that route this
 *     sensor through the ISP firmware's imgproc 3A keep working, but they
 *     should not bother - the sensor is already converged.
 *
 * SCCB: 7-bit address 0x3C, 16-bit register addresses (SCCB_*_Reg16).
 * XCLK: 24 MHz from the SoC's CAM_MISC reference clock (PLL48M / 2).
 *
 * Clock tree (esp32-camera's model of the datasheet, checked on the Reef):
 *   REFIN    = XCLK / pre_div            (0x3037[3:0], see pll_pre_div_x2)
 *   VCO      = REFIN * multiplier        (0x3036) [/2 if root_2x, 0x3037[4]]
 *   PLL_CLK  = VCO / sys_div * 2 / 5     (0x3035[7:4]; /5*2 = 10-bit mode 0x3034)
 *   PIX_RATE = PLL_CLK / 4               (sensor timing, 0x3108 = 0x16)
 *   DVP PCLK = PLL_CLK / 2 / pclk_div    (0x3824, manual via 0x460C[1])
 * HTS / VTS count PIX_RATE periods, so frame rate = PIX_RATE / (HTS x VTS).
 * 0x3824 only changes the DVP output clock: the VFIFO re-times each line.
 *
 * Bench, IPRO7AI Reef, 2026-10-06 (`cam pins`): VSYNC 12.2 Hz and HREF
 * 8,780 lines/s at PLL mult 20 for both pclk_div values; HREF duty 93 % at
 * pclk_div 2 (2560 bytes in a 2644-clock line: DVP PCLK = PIX_RATE = 24 MHz)
 * and 46 % at pclk_div 1 (DVP PCLK 48 MHz). So with 8-bit YUYV the DVP PCLK
 * must be 2 x PIX_RATE, the hal_isp model ("2T per pixel", total_width in
 * pixels). pclk_div 2 is not just slow: hal_isp sizes the DVP_AS FIFO
 * threshold as width x (1 - (pclk/2) / BCLK) + 10, which at 24 MHz against
 * the 96 MHz BCLK is 1130 - over the 1024-entry FIFO, so the timing
 * generator never starts and the ISP reports 0 frames and 0 errors.
 * At 48 MHz the threshold is 970, at 72 MHz 810, at 84 MHz (the default
 * below) 730.
 *
 * Tuning, 2026-10-07 (a chart on a Mac screen, measured patch by patch):
 * line timing is OmniVision's 720p one (HTS 1892, VTS 740), which at 84 MHz
 * is 30 fps; the 50/60 Hz banding steps are computed from it; night mode
 * lets the AEC stretch frames down to 15 fps in dim light instead of
 * running out of exposure at 30. The register set stays esp32-camera's.
 * OmniVision's full initialisation (Linux / Zephyr) was tried and dropped:
 * on this module it measured no better for colour, noise or sharpness,
 * compressed the highlights, its AWB tuning left greys blue, and its
 * 0x370C = 0x03 puts the demosaic one Bayer phase off (green comes out
 * magenta) - keep 0x370C at 0x02. Only its binned-mode read-out timing
 * (0x3708 / 0x3709, ov5640_set_720p) is taken: without it the array needs
 * the old 2644-clock line and HTS 1892 wraps each line into the next.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "sensor.h"
#include "sccb.h"
#include "camera.h"

#define OV5640_ADDR            0x3C
#define OV5640_CHIP_ID         0x5640
#define OV5640_XCLK_HZ         24000000u

/* Registers used by name below. */
#define REG_SYSTEM_CTROL0      0x3008   /* [7] soft reset, [6] power down */
#define REG_CHIP_ID_H          0x300A
#define REG_CHIP_ID_L          0x300B
#define REG_SC_PLL_CTRL0       0x3034   /* bit mode: 0x1A = 10-bit */
#define REG_SC_PLL_CTRL1       0x3035   /* [7:4] sys div, [3:0] mipi div */
#define REG_SC_PLL_CTRL2       0x3036   /* multiplier */
#define REG_SC_PLL_CTRL3       0x3037   /* [4] root 2x, [3:0] pre div */
#define REG_PLL_BYPASS         0x3039
#define REG_SYSTEM_ROOT_DIV    0x3108   /* [5:4] pclk root div */
#define REG_SCCB_SYSTEM_CTRL1  0x3103   /* [1] sysclk from PLL */
#define REG_X_ADDR_ST_H        0x3800
#define REG_X_ADDR_END_H       0x3804
#define REG_X_OUTPUT_SIZE_H    0x3808
#define REG_X_TOTAL_SIZE_H     0x380C
#define REG_X_OFFSET_H         0x3810
#define REG_X_INCREMENT        0x3814
#define REG_Y_INCREMENT        0x3815
#define REG_TIMING_TC_REG20    0x3820   /* [2:1] vflip, [0] v binning */
#define REG_TIMING_TC_REG21    0x3821   /* [2:1] hmirror, [0] h binning */
#define REG_PCLK_DIV           0x3824
#define REG_FORMAT_CTRL00      0x4300   /* 0x30 = YUV422 YUYV */
#define REG_VFIFO_CTRL0C       0x460C   /* [1] pclk manual */
#define REG_ISP_CONTROL_01     0x5001   /* [5] scale enable */
#define REG_FORMAT_MUX_CTRL    0x501F   /* 0 = YUV422 */
#define REG_AEC_PK_MANUAL      0x3503   /* [1] AGC manual, [0] AEC manual */
#define REG_AWB_MANUAL         0x3406   /* [0] AWB manual */
#define REG_TEST_PATTERN       0x503D   /* 0x80 = colour bar */

#define REG_DLY                0xFFFF   /* table entry: delay N ms */
#define REG_END                0x0000

/* ---- Output geometry: 1280x720 from the 16:9 window, 2x binned ----
 * OmniVision's 720p window: x 0..2623, y 250..1705 (1456 rows, 728 after
 * binning), ISP offset 16/4, and its 720p30 line / frame length HTS 1892 x
 * VTS 740. The window matters: esp32-camera's y 240..1711 reads 736 binned
 * rows, which leaves 4 lines of vertical blanking at VTS 740 - the IPRO7 ISP
 * then gets no frames at all at 30 fps, and only streams while the AEC has
 * stretched the frames (night mode). Every "stall after an exposure change"
 * on the Reef bench was a frame shrinking back to VTS 740 with that window. */
#define OV5640_OUT_W           1280
#define OV5640_OUT_H           720
#define OV5640_WIN_X_START     0
#define OV5640_WIN_Y_START     250
#define OV5640_WIN_X_END       2623
#define OV5640_WIN_Y_END       1705
#define OV5640_HTS             1892           /* pixel clocks per line */
#define OV5640_VTS             740            /* lines per frame */
#define OV5640_OFFSET_X        (32 / 2)
#define OV5640_OFFSET_Y        4              /* (728 - 720) / 2 */

/* ---- PLL ----
 * esp32-camera's HD preset is multiplier 10 / pre_div 2 / sys_div 1 /
 * pclk_root_div 1 (= /2) / pclk_div 2. Here pclk_div is 1, which puts the
 * DVP PCLK at 2 x PIX_RATE (see the file header for why it has to be 2x).
 * Multiplier 35: PIX_RATE 42 MHz, DVP PCLK 84 MHz, 30 fps, DVP_AS threshold
 * 730 (Reef bench: 29.7 fps, no FIFO / sync errors). 30 gives 72 MHz /
 * 25.7 fps (the SC101IOT's DVP rate on this path), 20 gives 48 MHz / 17 fps. */
#ifndef OV5640_PLL_MULT
#define OV5640_PLL_MULT        35
#endif

/* ---- Exposure / banding ----
 * Mains frequency for the banding filter: 60 (Taiwan, Americas), 50, or 0
 * to let the sensor detect it. A 50 Hz filter under 60 Hz lights leaves
 * rolling bars. Night mode: the AEC may stretch a frame to this many frame
 * times when the scene is too dark for one (2 = down to 15 fps); 1 keeps
 * the frame rate fixed. On the Reef bench at night, a fixed 30 fps ran out
 * of exposure and gain at half the AEC target. */
#ifndef OV5640_MAINS_HZ
#define OV5640_MAINS_HZ        60
#endif
/* Night mode stretches frames only once the gain is past this 0x3A17 [1:0]
 * step (power-on 01; 03 is the highest): the Reef bench at a desk lamp kept
 * 30 fps at 2.2x gain with 03, where 01 had already dropped to 15 fps at 4x. */
#ifndef OV5640_NIGHT_GAIN_TH
#define OV5640_NIGHT_GAIN_TH   0x03
#endif
#ifndef OV5640_NIGHT_FRAMES
#define OV5640_NIGHT_FRAMES    2
#endif

/* ---- Orientation ----
 * The FLEX-24 module on the IPRO7AI Reef reads out top-to-bottom mirrored.
 * Do not fix that in the sensor: in this binned, sub-sampled mode every
 * TIMING_TC_REG20/21 flip or mirror combination tried (with 0x370C either
 * way, and with the window moved by a row) puts the demosaic a Bayer phase
 * off - red/blue or green/magenta swap. The capture DMA flips instead
 * (DVP2AXI flip mode, apps/platform/ipro7ai_reef_demo/src/cam.c). */
#define OV5640_PLL_PRE_DIV     2        /* index into pll_pre_div_x2 -> /2 */
#define OV5640_PLL_SYS_DIV     1
#define OV5640_PLL_ROOT_2X     0
#define OV5640_PCLK_ROOT_DIV   1        /* index -> /2 */
#define OV5640_PCLK_DIV        1

/* Derived, kept as compile-time arithmetic so the descriptor is constant.
 * pre_div index 2 = /2. */
#define OV5640_REFIN_HZ        (OV5640_XCLK_HZ / 2)
#define OV5640_VCO_HZ          (OV5640_REFIN_HZ * OV5640_PLL_MULT)
#define OV5640_PLL_CLK_HZ      (OV5640_VCO_HZ / OV5640_PLL_SYS_DIV * 2 / 5)
#define OV5640_PIX_RATE_HZ     (OV5640_PLL_CLK_HZ / 4)
#define OV5640_DVP_PCLK_HZ     (OV5640_PLL_CLK_HZ / 2 / OV5640_PCLK_DIV)
#define OV5640_EXPECTED_FPS    (OV5640_PIX_RATE_HZ / (OV5640_HTS * OV5640_VTS))

/* Banding: exposure steps of one half mains period, in lines, and how many
 * fit in a frame (Linux ov5640_set_bandingfilter). */
#define OV5640_LINE_RATE       (OV5640_PIX_RATE_HZ / OV5640_HTS)
#define OV5640_BAND_STEP_50    (OV5640_LINE_RATE / 100)
#define OV5640_BAND_STEP_60    (OV5640_LINE_RATE / 120)
#define OV5640_MAX_BANDS_50    ((OV5640_VTS - 4) / OV5640_BAND_STEP_50)
#define OV5640_MAX_BANDS_60    ((OV5640_VTS - 4) / OV5640_BAND_STEP_60)
#define OV5640_MAX_EXPOSURE    (OV5640_VTS * OV5640_NIGHT_FRAMES)

/*
 * Power-on defaults (esp32-camera sensor_default_regs): AEC/AGC, 50/60 Hz
 * banding, AWB, colour matrix, sharpening, gamma and SDE. The sensor comes
 * out of this streaming with its own 3A enabled.
 */
static const uint16_t ov5640_default_regs[][2] = {
    {REG_SYSTEM_CTROL0, 0x82},          /* software reset */
    {REG_DLY, 10},
    {REG_SYSTEM_CTROL0, 0x42},          /* power down while configuring */
    {0x3103, 0x13},                     /* sysclk from PLL */
    {0x3017, 0xff},                     /* io direction: DVP outputs */
    {0x3018, 0xff},
    {0x302c, 0xc3},                     /* drive capability */
    {0x4740, 0x21},                     /* PCLK polarity */
    {0x4713, 0x02},
    {0x5001, 0x83},                     /* colour matrix, AWB, SDE */
    {0x3000, 0x20},                     /* reset MCU */
    {REG_DLY, 10},
    {0x3002, 0x1c},
    {0x3004, 0xff},                     /* clock enable */
    {0x3006, 0xc3},
    {0x5000, 0xa7},                     /* ISP control */
    {0x5001, 0xa3},
    {0x5003, 0x08},
    {0x370c, 0x02},
    {0x3634, 0x40},
    /* AEC / AGC */
    {0x3a02, 0x03}, {0x3a03, 0xd8}, {0x3a08, 0x01}, {0x3a09, 0x27},
    {0x3a0a, 0x00}, {0x3a0b, 0xf6}, {0x3a0d, 0x04}, {0x3a0e, 0x03},
    {0x3a0f, 0x30}, {0x3a10, 0x28}, {0x3a11, 0x60}, {0x3a13, 0x43},
    {0x3a14, 0x03}, {0x3a15, 0xd8}, {0x3a18, 0x00}, {0x3a19, 0xf8},
    {0x3a1b, 0x30}, {0x3a1e, 0x26}, {0x3a1f, 0x14},
    {0x3600, 0x08}, {0x3601, 0x33},
    /* 50/60 Hz banding */
    {0x3c01, 0xa4}, {0x3c04, 0x28}, {0x3c05, 0x98}, {0x3c06, 0x00},
    {0x3c07, 0x08}, {0x3c08, 0x00}, {0x3c09, 0x1c}, {0x3c0a, 0x9c},
    {0x3c0b, 0x40},
    {0x460c, 0x22},
    /* BLC */
    {0x4001, 0x02}, {0x4004, 0x02},
    /* AWB */
    {0x5180, 0xff}, {0x5181, 0xf2}, {0x5182, 0x00}, {0x5183, 0x14},
    {0x5184, 0x25}, {0x5185, 0x24}, {0x5186, 0x09}, {0x5187, 0x09},
    {0x5188, 0x09}, {0x5189, 0x75}, {0x518a, 0x54}, {0x518b, 0xe0},
    {0x518c, 0xb2}, {0x518d, 0x42}, {0x518e, 0x3d}, {0x518f, 0x56},
    {0x5190, 0x46}, {0x5191, 0xf8}, {0x5192, 0x04}, {0x5193, 0x70},
    {0x5194, 0xf0}, {0x5195, 0xf0}, {0x5196, 0x03}, {0x5197, 0x01},
    {0x5198, 0x04}, {0x5199, 0x12}, {0x519a, 0x04}, {0x519b, 0x00},
    {0x519c, 0x06}, {0x519d, 0x82}, {0x519e, 0x38},
    /* LENC B/R shading, 5x5 nodes 0x5824..0x583c: [7:4] B gain, [3:0] R gain.
     * Node rows run along image x, node columns along image y. With the
     * power-on table (aa outer, 99 ring, 88 centre) a flat field comes out
     * warm in the middle and blue-violet on the left edge: auto-AWB U/V spread
     * left/centre/right 144/131/132 and 138/143/132. Fitted on the Reef module
     * against a locked flat field (response matrix + smoothness, one
     * Gauss-Newton step): 137/136/135 and 137/136/135, U/V sd 5.2/4.2 -> 1.9/1.3
     * (2026-10-08). The luma (G) nodes 0x5800..0x5823 keep their defaults. */
    {0x5824, 0x99}, {0x5825, 0x6b}, {0x5826, 0x4b}, {0x5827, 0x3b}, {0x5828, 0x6a},
    {0x5829, 0x7b}, {0x582a, 0x4c}, {0x582b, 0x5a}, {0x582c, 0x4a}, {0x582d, 0x9b},
    {0x582e, 0xab}, {0x582f, 0x8a}, {0x5830, 0x93}, {0x5831, 0x79}, {0x5832, 0x9b},
    {0x5833, 0xbd}, {0x5834, 0x9f}, {0x5835, 0x8d}, {0x5836, 0x7e}, {0x5837, 0xad},
    {0x5838, 0x8a}, {0x5839, 0xaf}, {0x583a, 0xbf}, {0x583b, 0xbf}, {0x583c, 0xd9},
    /* colour matrix (saturation 0) */
    {0x5381, 0x1e}, {0x5382, 0x5b}, {0x5383, 0x08}, {0x5384, 0x0a},
    {0x5385, 0x7e}, {0x5386, 0x88}, {0x5387, 0x7c}, {0x5388, 0x6c},
    {0x5389, 0x10}, {0x538a, 0x01}, {0x538b, 0x98},
    /* CIP: sharpness / denoise. 0x5308 [6] = 1: fixed sharpening 0x5302.
     * 0x28 instead of esp32-camera's 0x18: chart edges +17 %, no halos, and
     * with JPEG q70 the stills come out smaller than 0x18 at q75 (Reef bench,
     * 2026-10-07). Stronger denoise (0x5306) only blurred the bars. */
    {0x5300, 0x10}, {0x5301, 0x10}, {0x5302, 0x28}, {0x5303, 0x19},
    {0x5304, 0x10}, {0x5305, 0x10}, {0x5306, 0x08}, {0x5307, 0x16},
    {0x5308, 0x40}, {0x5309, 0x10}, {0x530a, 0x10}, {0x530b, 0x04},
    {0x530c, 0x06},
    /* gamma */
    {0x5480, 0x01}, {0x5481, 0x00}, {0x5482, 0x1e}, {0x5483, 0x3b},
    {0x5484, 0x58}, {0x5485, 0x66}, {0x5486, 0x71}, {0x5487, 0x7d},
    {0x5488, 0x83}, {0x5489, 0x8f}, {0x548a, 0x98}, {0x548b, 0xa6},
    {0x548c, 0xb8}, {0x548d, 0xca}, {0x548e, 0xd7}, {0x548f, 0xe3},
    {0x5490, 0x1d},
    /* SDE: brightness / contrast */
    {0x5580, 0x06}, {0x5583, 0x40}, {0x5584, 0x10}, {0x5586, 0x20},
    {0x5587, 0x00}, {0x5588, 0x00}, {0x5589, 0x10}, {0x558a, 0x00},
    {0x558b, 0xf8}, {0x501d, 0x40},
    {REG_SYSTEM_CTROL0, 0x02},          /* power on */
    {0x3c00, 0x04},                     /* 50 Hz */
    {REG_DLY, 300},
    {REG_END, 0x00},
};

static const uint16_t ov5640_fmt_yuv422[][2] = {
    {REG_FORMAT_MUX_CTRL, 0x00},        /* YUV422 */
    {REG_FORMAT_CTRL00, 0x30},          /* YUYV: Y first, then U */
    {REG_END, 0x00},
};

/* ---------------------------------------------------------------------------
 * Register access
 * -------------------------------------------------------------------------*/

static int ov5640_rd(uint16_t reg, uint8_t *val)
{
    return SCCB_Read_Reg16(OV5640_ADDR, reg, val, 1);
}

static int ov5640_wr(uint16_t reg, uint8_t val)
{
    return SCCB_Write_Reg16(OV5640_ADDR, reg, &val);
}

static int ov5640_wr16(uint16_t reg, uint16_t val)
{
    int rc = ov5640_wr(reg, (uint8_t)(val >> 8));
    return rc ? rc : ov5640_wr(reg + 1, (uint8_t)val);
}

/* x into reg/reg+1, y into reg+2/reg+3 - the OV5640 keeps them adjacent. */
static int ov5640_wr_xy(uint16_t reg, uint16_t x, uint16_t y)
{
    int rc = ov5640_wr16(reg, x);
    return rc ? rc : ov5640_wr16(reg + 2, y);
}

static int ov5640_wr_table(const uint16_t (*regs)[2])
{
    for (unsigned i = 0; regs[i][0] != REG_END; i++) {
        if (regs[i][0] == REG_DLY) {
            vTaskDelay(pdMS_TO_TICKS(regs[i][1]));
            continue;
        }
        if (ov5640_wr(regs[i][0], (uint8_t)regs[i][1]) != 0) {
            printf("[ov5640] write 0x%04x failed\r\n", regs[i][0]);
            return -1;
        }
    }
    return 0;
}

/* ---------------------------------------------------------------------------
 * Bring-up
 * -------------------------------------------------------------------------*/

static int ov5640_set_pll(void)
{
    int rc = 0;

    rc |= ov5640_wr(REG_PLL_BYPASS, 0x00);
    rc |= ov5640_wr(REG_SC_PLL_CTRL0, 0x1A);                           /* 10-bit */
    rc |= ov5640_wr(REG_SC_PLL_CTRL1, 0x01 | ((OV5640_PLL_SYS_DIV & 0x0F) << 4));
    rc |= ov5640_wr(REG_SC_PLL_CTRL2, OV5640_PLL_MULT & 0xFF);
    rc |= ov5640_wr(REG_SC_PLL_CTRL3, (OV5640_PLL_PRE_DIV & 0x0F) | (OV5640_PLL_ROOT_2X ? 0x10 : 0x00));
    rc |= ov5640_wr(REG_SYSTEM_ROOT_DIV, ((OV5640_PCLK_ROOT_DIV & 0x3) << 4) | 0x06);
    rc |= ov5640_wr(REG_PCLK_DIV, OV5640_PCLK_DIV & 0x1F);
    rc |= ov5640_wr(REG_VFIFO_CTRL0C, 0x22);                           /* pclk manual */
    rc |= ov5640_wr(REG_SCCB_SYSTEM_CTRL1, 0x13);                      /* sysclk from PLL */
    return rc ? -1 : 0;
}

/* 1280x720: 16:9 window, 2x binning both axes, no ISP scaling. */
static int ov5640_set_720p(void)
{
    int rc = 0;

    rc |= ov5640_wr_xy(REG_X_ADDR_ST_H, OV5640_WIN_X_START, OV5640_WIN_Y_START);
    rc |= ov5640_wr_xy(REG_X_ADDR_END_H, OV5640_WIN_X_END, OV5640_WIN_Y_END);
    rc |= ov5640_wr_xy(REG_X_OUTPUT_SIZE_H, OV5640_OUT_W, OV5640_OUT_H);
    rc |= ov5640_wr_xy(REG_X_TOTAL_SIZE_H, OV5640_HTS, OV5640_VTS);
    rc |= ov5640_wr_xy(REG_X_OFFSET_H, OV5640_OFFSET_X, OV5640_OFFSET_Y);
    if (rc) {
        return -1;
    }
    /* scale off (bit 5 of 0x5001), keep the rest of the ISP enables */
    {
        uint8_t v = 0;
        if (ov5640_rd(REG_ISP_CONTROL_01, &v) != 0) {
            return -1;
        }
        rc |= ov5640_wr(REG_ISP_CONTROL_01, (uint8_t)(v & ~0x20u));
    }
    /* binning: TC_REG20/21 bit0, 0x4514 0xaa, 0x4520 0x0b, sub-sample 3/1 */
    rc |= ov5640_wr(REG_TIMING_TC_REG20, 0x01);
    rc |= ov5640_wr(REG_TIMING_TC_REG21, 0x01);
    rc |= ov5640_wr(0x4514, 0xaa);
    rc |= ov5640_wr(0x4520, 0x0b);
    rc |= ov5640_wr(REG_X_INCREMENT, 0x31);
    rc |= ov5640_wr(REG_Y_INCREMENT, 0x31);
    /* array read-out timing for binned modes (OmniVision / Linux): with the
     * power-on (full-resolution) values the line needs ~2644 clocks and HTS
     * 1892 wraps it into the next. 0x3618 / 0x3612 are OmniVision's binned
     * values too, but only to start: see ov5640_streaming(). */
    rc |= ov5640_wr(0x3618, 0x00);
    rc |= ov5640_wr(0x3612, 0x29);
    rc |= ov5640_wr(0x3708, 0x64);
    rc |= ov5640_wr(0x3709, 0x52);
    return rc ? -1 : 0;
}

static int ov5640_probe(void)
{
    uint8_t hi = 0, lo = 0;

    if (ov5640_rd(REG_CHIP_ID_H, &hi) != 0 || ov5640_rd(REG_CHIP_ID_L, &lo) != 0) {
        printf("[ov5640] SCCB read failed\r\n");
        return -1;
    }
    printf("[ov5640] chip id 0x%02x%02x\r\n", hi, lo);
    return (((hi << 8) | lo) == OV5640_CHIP_ID) ? 0 : -1;
}

static int ov5640_reset(void)
{
    /* Software reset, then the defaults (which reset again and power-cycle
     * the digital block themselves). ~450 ms all told. */
    (void)ov5640_wr(REG_SCCB_SYSTEM_CTRL1, 0x11);
    (void)ov5640_wr(REG_SYSTEM_CTROL0, 0x82);
    vTaskDelay(pdMS_TO_TICKS(100));
    return ov5640_wr_table(ov5640_default_regs);
}

/* Banding filter and exposure ceiling for the timing above. */
static int ov5640_set_exposure_limits(void)
{
    int rc = 0;
    uint8_t aec0 = 0;

    rc |= ov5640_wr16(0x3a08, OV5640_BAND_STEP_50);
    rc |= ov5640_wr16(0x3a0a, OV5640_BAND_STEP_60);
    rc |= ov5640_wr(0x3a0e, OV5640_MAX_BANDS_50);
    rc |= ov5640_wr(0x3a0d, OV5640_MAX_BANDS_60);
    rc |= ov5640_wr16(0x3a02, OV5640_MAX_EXPOSURE);  /* 60 Hz max exposure, lines */
    rc |= ov5640_wr16(0x3a14, OV5640_MAX_EXPOSURE);  /* 50 Hz max exposure */
    rc |= ov5640_wr(0x3a17, OV5640_NIGHT_GAIN_TH);
    /* AEC CTRL00: [5] banding on, [2] night mode */
    rc |= ov5640_rd(0x3a00, &aec0);
    aec0 = (uint8_t)((aec0 | 0x20) & ~0x04);
    rc |= ov5640_wr(0x3a00, (uint8_t)(aec0 | (OV5640_NIGHT_FRAMES > 1 ? 0x04 : 0x00)));
#if OV5640_MAINS_HZ == 0
    rc |= ov5640_wr(0x3c01, 0x24);                  /* detect 50 / 60 Hz */
#else
    rc |= ov5640_wr(0x3c01, 0xa4);                  /* manual */
    rc |= ov5640_wr(0x3c00, OV5640_MAINS_HZ == 50 ? 0x04 : 0x00);
#endif
    return rc ? -1 : 0;
}

static int ov5640_setFMT(void)
{
    if (ov5640_wr_table(ov5640_fmt_yuv422) != 0) {
        return -1;
    }
    if (ov5640_set_720p() != 0 || ov5640_set_pll() != 0 || ov5640_set_exposure_limits() != 0) {
        return -1;
    }
    printf("[ov5640] YUV422 %ux%u, HTS %u VTS %u, pixel rate %u Hz, DVP PCLK %u Hz, ~%u fps "
           "(night mode to 1/%u), banding %s\r\n",
           OV5640_OUT_W, OV5640_OUT_H, OV5640_HTS, OV5640_VTS,
           (unsigned)OV5640_PIX_RATE_HZ, (unsigned)OV5640_DVP_PCLK_HZ,
           (unsigned)OV5640_EXPECTED_FPS, (unsigned)OV5640_NIGHT_FRAMES,
           OV5640_MAINS_HZ == 0 ? "auto" : (OV5640_MAINS_HZ == 50 ? "50 Hz" : "60 Hz"));
    return 0;
}

/*
 * OmniVision's binned 0x3618 / 0x3612 bleed green into blue on this module (a
 * cyan patch comes out lavender, fine black-and-white lines pink), but the
 * array does not start streaming without them - with the power-on values from
 * the start the ISP gets no frames at all, and switching back 300 ms after
 * setFMT still lost most first frames. Switching once frames flow is clean
 * (Reef bench, 2026-10-07: thousands of frames, no FIFO / sync errors).
 */
static int ov5640_streaming(void)
{
    int rc = ov5640_wr(0x3618, 0x04);

    return rc ? rc : ov5640_wr(0x3612, 0x4b);
}

/* ---------------------------------------------------------------------------
 * rt_camera_desc hooks
 *
 * AE/AWB live on the sensor. The ISP firmware's imgproc calls these when an
 * app starts it anyway; answering "done" keeps it from dereferencing NULL.
 * SENSOR_CASE_* values are imgproc's, see components/isp_firmware.
 * -------------------------------------------------------------------------*/

static int ov5640_set_case(uint8_t case_id)
{
    (void)case_id;
    return 0;
}

static int ov5640_set_awb_gain(uint8_t enable, int r_gain, int b_gain)
{
    (void)enable; (void)r_gain; (void)b_gain;
    return 0;
}

static int ov5640_get_awb_register(uint8_t case_id, int *value)
{
    (void)case_id;
    if (value) *value = 0;
    return 0;
}

static int ov5640_set_ae_expo_gain(uint8_t enable, int gain_db)
{
    (void)enable; (void)gain_db;
    return 0;
}

static int ov5640_get_ae_register(uint8_t case_id, int *value)
{
    (void)case_id;
    if (value) *value = 0;
    return 0;
}

static int ov5640_set_register(int addr, int value)
{
    return ov5640_wr((uint16_t)addr, (uint8_t)value);
}

static int ov5640_get_register(int addr, int *value)
{
    uint8_t v = 0;
    int rc = ov5640_rd((uint16_t)addr, &v);
    if (value) *value = v;
    return rc;
}

const rt_camera_desc __rt_camera_ov5640_desc __attribute__((section(".camera_desc"))) =
{
    .name             = "__rti_name_ov5640",
    .addr             = OV5640_ADDR,
    .frm_vld_high     = 1,
    .probe            = ov5640_probe,
    .reset            = ov5640_reset,
    .setFMT           = ov5640_setFMT,
    .set_case         = ov5640_set_case,
    .set_awb_gain     = ov5640_set_awb_gain,
    .get_awb_register = ov5640_get_awb_register,
    .set_ae_expo_gain = ov5640_set_ae_expo_gain,
    .get_ae_register  = ov5640_get_ae_register,
    .set_register     = ov5640_set_register,
    .get_register     = ov5640_get_register,
    .width            = OV5640_OUT_W,
    .height           = OV5640_OUT_H,
    .pclk             = OV5640_DVP_PCLK_HZ, /* real DVP PCLK, see header */
    .total_width      = OV5640_HTS,
    .total_height     = OV5640_VTS,
    .streaming        = ov5640_streaming,
};
