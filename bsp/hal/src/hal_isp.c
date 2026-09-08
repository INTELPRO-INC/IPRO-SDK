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

#include "hal_isp.h"
#include "drv_dvp2axi.h"
#include "drv_glb.h"
#include "drv_mm_misc.h"
#include "drv_isp.h"
#include "drv_isp_wdr.h"
#include "drv_scaler_hydre.h"
#include "drv_tsrc.h"
#include "drv_osd_blend.h"
#include "drv_osd_draw.h"
#include "drv_clock.h"

#define DVP_AS_THRESHOLD_MARGIN        (10)

#define TSRC_H_BLANK      (4000) // magic number for scaler up usage
#define TSRC_V_BLANK      (50)

#define PHY_CHANNEL_NUM   (2)                      // ipro7 hw define two channel (scaler output)
#define CHANNEL_NUM       (ISP_CHANNEL_MAX_NUM)    // ipro7 sw define two channel + extend one channel

#define EARLY_LINE_CNT(height)  ((height)>>1) // set half line count first

#define SCALE_COEF_MULTI_ACCURACY (1024)

enum
{
    ISP_UPDATE_FLAG_SCALER,
    ISP_UPDATE_FLAG_FRC,
    ISP_UPDATE_FLAG_MAX,
};

// Keep this size to be reference
static SCALER_CONFIG_S scaler_size_tmp[PHY_CHANNEL_NUM] = {0};
static isp_hal_frame_rate_t frc_tmp[CHANNEL_NUM] = {0};

static uint8_t update_flag[ISP_UPDATE_FLAG_MAX][CHANNEL_NUM] = {0};

static isp_hal_out_format0_e out_format0_tmp;

static uint8_t pp_num_tmp[CHANNEL_NUM] = {0};
static uint8_t pp_idx[CHANNEL_NUM] = {0};
static uint32_t out_addr_tmp[CHANNEL_NUM][ISP_MAX_BUFFER_NUM] = {0};

static void (*g_ispCallback)(isp_out_frame_info_t info);

static uint8_t channel_en[CHANNEL_NUM] = {0};

static isp_hal_src_e isp_src_tmp;

static isp_hal_cfg_t g_isp_cfg_tmp = {0};

const uint8_t g_scale_hor_coeff[19][2] = {
    // SMOOTH 24 (set, ratio)
    {
        3,  8
    },
    // SMOOTH 22 (set, ratio)
    {
        3,  6
    },
    // SMOOTH 21 (set, ratio)
    {
        3,  5
    },
    // SMOOTH 19 (set, ratio)
    {
        3,  3
    },
    // SMOOTH 18 (set, ratio)
    {
        3,  2
    },
    // SMOOTH 16 (set, ratio)
    {
        3,  0
    },
    // SMOOTH 14 (set, ratio)
    {
        2,  6
    },
    // SMOOTH 13 (set, ratio)
    {
        2,  5
    },
    // SMOOTH 12 (set, ratio)
    {
        2,  4
    },
    // SMOOTH 10 (set, ratio)
    {
        2,  2
    },
    // SMOOTH 9 (set, ratio)
    {
        2,  1
    },
    // SMOOTH 7 (set, ratio)
    {
        1,  7
    },
    // SMOOTH 6 (set, ratio)
    {
        1,  6
    },
    // SMOOTH 5 (set, ratio)
    {
        1,  5
    },
    // SMOOTH 4 (set, ratio)
    {
        1,  4
    },
    // SMOOTH 3 (set, ratio)
    {
        1,  3
    },
    // SMOOTH 2 (set, ratio)
    {
        1,  2
    },
    // SMOOTH 1 (set, ratio)
    {
        1,  1
    },
    // SMOOTH 0 (set, ratio)
    {
        1,  0
    }
};

const uint8_t g_scale_ver_coeff[19][2] = {
    // SMOOTH 24 (set, ratio)
    {
        1,  8
    },
    // SMOOTH 22 (set, ratio)
    {
        1,  8
    },
    // SMOOTH 21 (set, ratio)
    {
        1,  8
    },
    // SMOOTH 19 (set, ratio)
    {
        1,  8
    },
    // SMOOTH 18 (set, ratio)
    {
        1,  8
    },
    // SMOOTH 16 (set, ratio)
    {
        1,  8
    },
    // SMOOTH 14 (set, ratio)
    {
        1,  8
    },
    // SMOOTH 13 (set, ratio)
    {
        1,  8
    },
    // SMOOTH 12 (set, ratio)
    {
        1,  8
    },
    // SMOOTH 10 (set, ratio)
    {
        1,  8
    },
    // SMOOTH 9 (set, ratio)
    {
        1,  8
    },
    // SMOOTH 7 (set, ratio)
    {
        1,  7
    },
    // SMOOTH 6 (set, ratio)
    {
        1,  6
    },
    // SMOOTH 5 (set, ratio)
    {
        1,  5
    },
    // SMOOTH 4 (set, ratio)
    {
        1,  4
    },
    // SMOOTH 3 (set, ratio)
    {
        1,  3
    },
    // SMOOTH 2 (set, ratio)
    {
        1,  2
    },
    // SMOOTH 1 (set, ratio)
    {
        1,  1
    },
    // SMOOTH 0 (set, ratio)
    {
        1,  0
    }
};

// ============================================================================
// private function declare
// ============================================================================
static DVP2AXI_PORT_E get_dvp_port_by_output(uint8_t output);
static void update_next_buffer(uint8_t output);
static void update_scaler_in_cfg(void);
static uint8_t find_scaler_coef(uint16_t input, uint16_t output);
static void update_fps_cfg(void);
static void select_ext_chn_src_size(isp_hal_cfg_t *isp_hal_cfg, uint16_t *width, uint16_t *height);
static int check_zoom_cfg(uint8_t channel, SCALER_CONFIG_S *zoom_cfg);
static int check_fps_cfg(uint8_t src_fps, uint8_t dst_fps);
static int check_out_crop_cfg(uint8_t channel, isp_hal_cfg_t *isp_hal_cfg);

static void isp_isr(uint8_t output);
static void isp_seof1_isr(void);
static void isp_channel0_ISR(void);
static void isp_channel1_ISR(void);
static void isp_channel_ext_ISR(void);
static void isp_scaler_path_init(void);
static void isp_osd_path_init(void);
static void isp_yuv422To420_init(void);
static void isp_yuv2rgb_init(void);
static uint32_t get_isp_clk(void);
static uint32_t isp_cal_dvp_as_threshold(uint32_t width, uint32_t isp_clk, uint32_t sns_pclk);
static uint32_t isp_cal_total_pix(uint32_t total_width, uint32_t isp_clk, uint32_t sns_pclk);
static void isp_spi2dvp_cfg(isp_hal_cfg_t *isp_hal_cfg);
static void isp_dvp_as_cfg(isp_hal_cfg_t *isp_hal_cfg);
static void isp_tsrc_cfg(uint16_t width, uint16_t height, uint32_t addr, bool conti);;
static void isp_scaler_size_cfg(uint8_t channel, SCALER_CONFIG_S size_cfg);
static void isp_scaler_coef_cfg(uint8_t channel, SCALER_CONFIG_S size_cfg);
static void isp_restore_parameter(isp_hal_cfg_t *isp_hal_cfg);
static void isp_glb_crop_cfg(isp_hal_cfg_t *isp_hal_cfg);
static void isp_yuv_adj_cfg(isp_hal_cfg_t *isp_hal_cfg);
static void isp_seof1_set_callback(uint16_t line_cnt);
static void isp_seof1_clr_callback(void);
static void isp_scaler_cfg(uint8_t channel, isp_hal_cfg_t *isp_hal_cfg);
static void isp_dvp2axi_set_callback(uint8_t channel);
static void isp_dvp2axi_clr_callback(uint8_t channel);
static void isp_set_channel_frame_rate(uint8_t src_fps, uint8_t dst_fps, uint32_t *bitwise_vld_frm, uint8_t *period_cnt);
static void isp_dvp2axi_cfg(uint8_t channel, isp_hal_cfg_t *isp_hal_cfg);

// ============================================================================
// private function
// ============================================================================
static DVP2AXI_PORT_E get_dvp_port_by_output(uint8_t output)
{
    if (ISP_CHANNEL_0 == output) return DVP2AXI_PORT_A;
    else if (ISP_CHANNEL_1 == output) return DVP2AXI_PORT_C;
    else return DVP2AXI_PORT_B;
}

static void update_next_buffer(uint8_t output)
{
    uint32_t frameAddr, byteCnt = 0;
    DVP2AXI_PORT_E port = get_dvp_port_by_output(output);

    if (pp_num_tmp[output] > 1) // do ping-pong if num > 1
    {
        pp_idx[output] = (pp_idx[output] + 1) % pp_num_tmp[output];
        DVP2AXI_SetAddress(port, out_addr_tmp[output][pp_idx[output]]);
        if (ISP_CHANNEL_0 == output)
        {
            if ((out_format0_tmp == ISP_HAL_OUT_FMT0_YUV422SP) || (out_format0_tmp == ISP_HAL_OUT_FMT0_NV12))
            {
                DVP2AXI_GetFrameBuffer(port, &frameAddr, &byteCnt);
                DVP2AXI_SetAddress(DVP2AXI_PORT_B, out_addr_tmp[output][pp_idx[output]] + byteCnt);
            }
        }
    }
}

static void update_scaler_in_cfg(void)
{
    uint8_t i;
    for (i = 0; i < PHY_CHANNEL_NUM; i++)
    {
        if (update_flag[ISP_UPDATE_FLAG_SCALER][i])
        {
            isp_scaler_size_cfg((SCALER_HYDRE_TYPE_T)i, scaler_size_tmp[i]);
            isp_scaler_coef_cfg((SCALER_HYDRE_TYPE_T)i, scaler_size_tmp[i]);
            update_flag[ISP_UPDATE_FLAG_SCALER][i] = 0;
        }
    }
}

static void update_fps_cfg(void)
{
    uint8_t i;
    uint8_t period_cnt = 0;
    uint32_t vld_frm = 0;

    for (i = 0; i < CHANNEL_NUM; i++)
    {
        if (update_flag[ISP_UPDATE_FLAG_FRC][i])
        {
            isp_set_channel_frame_rate(frc_tmp[i].src_fps, frc_tmp[i].dst_fps, &vld_frm, &period_cnt);
            if (i == ISP_CHANNEL_0)
            {
                DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_A, vld_frm, (period_cnt - 1));
                if ((out_format0_tmp == ISP_HAL_OUT_FMT0_YUV422SP) || (out_format0_tmp == ISP_HAL_OUT_FMT0_NV12))
                    DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_B, vld_frm, (period_cnt - 1));
            }
            else if (i == ISP_CHANNEL_1)
                DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_C, vld_frm, (period_cnt - 1));
            else
                DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_B, vld_frm, (period_cnt - 1));

            update_flag[ISP_UPDATE_FLAG_FRC][i] = 0;
        }
    }
}

static void isp_isr(uint8_t output)
{
    isp_out_frame_info_t frame_info;
    uint32_t frameAddr, byteCnt = 0;
    uint32_t frameAddr1, byteCnt1 = 0;

    DVP2AXI_PORT_E port = get_dvp_port_by_output(output);

    DVP2AXI_GetFrameBuffer(port, &frameAddr, &byteCnt);
    if (ISP_CHANNEL_0 == output)
    {
        if ((out_format0_tmp == ISP_HAL_OUT_FMT0_YUV422SP) || (out_format0_tmp == ISP_HAL_OUT_FMT0_NV12))
            DVP2AXI_GetFrameBuffer(DVP2AXI_PORT_B, &frameAddr1, &byteCnt1);
    }

    frame_info.out_port = output;
    frame_info.out_addr = frameAddr;
    frame_info.out_size = byteCnt + byteCnt1;

    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_NORMAL)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_NORMAL);
        frame_info.event = ISP_HAL_INT_NORMAL;
        update_next_buffer(output);
        if (g_ispCallback)
            g_ispCallback(frame_info);
    }
    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_FIFO_OVERWRITE)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_FIFO_OVERWRITE);
        // reset dvp2axi
        DVP2AXI_Disable(port);
        DVP2AXI_Enable(port);
        frame_info.event = ISP_HAL_INT_FIFO_OVERWRITE;
        if (g_ispCallback)
            g_ispCallback(frame_info);
    }
    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_HSYNC_CNT_ERROR)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_HSYNC_CNT_ERROR);
        frame_info.event = ISP_HAL_INT_HSYNC_CNT_ERROR;
        if (g_ispCallback)
            g_ispCallback(frame_info);
    }
    if (DVP2AXI_IntStatus(port, DVP2AXI_INT_VSYNC_CNT_ERROR)) {
        DVP2AXI_IntClr(port, DVP2AXI_INT_VSYNC_CNT_ERROR);
        frame_info.event = ISP_HAL_INT_VSYNC_CNT_ERROR;
        if (g_ispCallback)
            g_ispCallback(frame_info);
    }
}

static void isp_seof1_isr(void)
{
    MM_MISC_SEOF_TYPE_E type_status;
    MM_MISC_Get_SEOF_TypeStatus(MM_MISC_SEOF1, &type_status);
    if (type_status.BF.SoF)
    {
        // sof
    }
    if (type_status.BF.EoF)
    {
        // eof
    }
    if (type_status.BF.lineCnt)
    {
        // early
        update_scaler_in_cfg();
    }
    MM_MISC_Clr_SEOF_TypeStatus(MM_MISC_SEOF1, type_status);
}

static void isp_channel0_ISR(void)
{
    isp_isr(0); // for output channel 0 (callback from dvp2axi_a)
}

static void isp_channel1_ISR(void)
{
    isp_isr(1); // for output channel 1 (callback from dvp2axi_c)
}

static void isp_channel_ext_ISR(void)
{
    isp_isr(2); // for output channel extend (callback from dvp2axi_b)
}

static void isp_scaler_path_init(void)
{
    // scaler input from ISP output
    MM_MISC_Set_YUV422_In_Mux(MM_MISC_YUV422_IN_MUX_ID_SCALER, MM_MISC_YUV422_IN_MUX_SEL_ISP_OUTPUT);
}

static void isp_osd_path_init(void)
{
    // osd draw input from scaler output 0
    MM_MISC_Set_YUV422_In_Mux(MM_MISC_YUV422_IN_MUX_ID_OSD_DRAW, MM_MISC_YUV422_IN_MUX_SEL_SCALER_F0);

    // osd blend input from osd draw
    MM_MISC_Set_YUV422_In_Mux(MM_MISC_YUV422_IN_MUX_ID_OSD_BLEND, MM_MISC_YUV422_IN_MUX_SEL_OSD_DRAW);
}

static void isp_yuv422To420_init(void)
{
    // yuv422to420 input from osd blend
    MM_MISC_Set_YUV422_In_Mux(MM_MISC_YUV422_IN_MUX_ID_YUV422TO420, MM_MISC_YUV422_IN_MUX_SEL_OSD_BLEND);
}

static void isp_yuv2rgb_init(void)
{
    MM_MISC_YUV2RGB_Cfg_Type cfg =
    {
#if 1 // RGB
        .enable = 1,
        .preOffset0 = 0,
        .preOffset1 = 0x180, //0xff80, // -128, 2's Complement, hw reg valid bit is 9
        .preOffset2 = 0x180, //0xff80, // -128, 2's Complement, hw reg valid bit is 9
        .postOffset0 = 0,
        .postOffset1 = 0,
        .postOffset2 = 0,
        .matrix00 = 512,
        .matrix01 = 0,
        .matrix02 = 718,
        .matrix10 = 512,
        .matrix11 = 0xf50, //0xff50, // -176, 2's Complement, hw reg valid bit is 12
        .matrix12 = 0xe92, //0xfe92, // -366, 2's Complement, hw reg valid bit is 12
        .matrix20 = 512,
        .matrix21 = 907,
        .matrix22 = 0,
#else // BGR
        .enable = 1,
        .preOffset0 = 0,
        .preOffset1 = 0x180, //0xff80, // -128, 2's Complement, hw reg valid bit is 9
        .preOffset2 = 0x180, //0xff80, // -128, 2's Complement, hw reg valid bit is 9
        .postOffset0 = 0,
        .postOffset1 = 0,
        .postOffset2 = 0,
        .matrix00 = 512,
        .matrix01 = 907,
        .matrix02 = 0,
        .matrix10 = 512,
        .matrix11 = 0xf50, //0xff50, // -176, 2's Complement, hw reg valid bit is 12
        .matrix12 = 0xe92, //0xfe92, // -366, 2's Complement, hw reg valid bit is 12
        .matrix20 = 512,
        .matrix21 = 0,
        .matrix22 = 718,
#endif
    };

    MM_MISC_YUV2RGB_Init(&cfg);

    // yuv422to444 input from scaler output 1
    MM_MISC_Set_YUV422_In_Mux(MM_MISC_YUV422_IN_MUX_ID_YUV422TO444, MM_MISC_YUV422_IN_MUX_SEL_SCALER_F1);

    // yuv2rgb input from yuv422to444
    MM_MISC_Set_YUV444_In_Mux(MM_MISC_YUV444_IN_MUX_ID_YUVTORGB, MM_MISC_YUV444_IN_MUX_SEL_YUV422TO444);
}

static uint32_t get_isp_clk(void)
{
#if defined(CONFIG_FPGA)
    // FPGA is 50Mhz
    return 50000000;
#else
    // return 96000000;
    return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_BCLK);
#endif
}

static uint32_t isp_cal_dvp_as_threshold(uint32_t width, uint32_t isp_clk, uint32_t sns_pclk)
{
    /* FIFO_thresh = width x (isp_clk - pclk>>1) / isp_clk + margin, yuyv 2T for 1 pixel */
    return (uint64_t)width * (isp_clk - (sns_pclk>>1)) / isp_clk + DVP_AS_THRESHOLD_MARGIN;
}

static uint32_t isp_cal_total_pix(uint32_t total_width, uint32_t isp_clk, uint32_t sns_pclk)
{
    /* total pix = sensor total pix x (isp_clk /(pclk>>1)) + half sensor total pix, yuyv 2T for 1 pixel */
    return (uint64_t)total_width * isp_clk / (sns_pclk>>1) + (total_width >> 1);
}

static void isp_spi2dvp_cfg(isp_hal_cfg_t *isp_hal_cfg)
{
    MM_MISC_SPI2DVP_Cfg cfg;

    // below setting by spi sensor
    cfg.dataOrder   = isp_hal_cfg->src_cfg.spi2dvp_input.order;
    cfg.dataFormat  = isp_hal_cfg->src_cfg.spi2dvp_input.format;
    cfg.spiMode     = isp_hal_cfg->src_cfg.spi2dvp_input.mode;
    cfg.dataEndian  = isp_hal_cfg->src_cfg.spi2dvp_input.endian;

    MM_MISC_Set_DVP_AS_SRAM_Mode(MM_MISC_SHARED_SRAM_MODE_SPI2DVP);

    MM_MISC_SPI2DVP_Init(cfg);
    MM_MISC_Set_SPI2DVP_Select(isp_hal_cfg->src_cfg.spi2dvp_input.spi_sel);
    MM_MISC_SPI2DVP_INT_Enable();
    // MM_MISC_Set_SPI2DVP_State(1);
}

static void isp_dvp_as_cfg(isp_hal_cfg_t *isp_hal_cfg)
{
    MM_MISC_DVP_AS_TG_Cfg dvp_as_tg = {0};
    MM_MISC_DVP_AS_Cfg dvp_as = {0};
    MM_MISC_S2P_Cfg_Type dvp_s2p = {0};
    uint32_t isp_clk = 0;

    isp_clk = get_isp_clk();

    dvp_as.hSyncLevel = MM_MISC_ACTIVE_HIGH;
    dvp_as.vSyncLevel = MM_MISC_ACTIVE_HIGH;
    dvp_as.fifoThreshold = isp_cal_dvp_as_threshold(isp_hal_cfg->in_size.width, isp_clk, isp_hal_cfg->src_cfg.dvp_input.pclk);

    dvp_as_tg.totalHPixels = isp_cal_total_pix(isp_hal_cfg->src_cfg.dvp_input.totalHPixels, isp_clk, isp_hal_cfg->src_cfg.dvp_input.pclk);
    dvp_as_tg.actStartPixel = 0;
    dvp_as_tg.actPixelWidth = isp_hal_cfg->in_size.width;
    dvp_as_tg.actStartLine = 0;
    dvp_as_tg.actLineHeight = isp_hal_cfg->in_size.height;

    dvp_s2p.hSyncLevel = isp_hal_cfg->src_cfg.dvp_input.hsync;
    dvp_s2p.vSyncLevel = isp_hal_cfg->src_cfg.dvp_input.vsync;
    dvp_s2p.dataOrder = isp_hal_cfg->src_cfg.dvp_input.order;

    MM_MISC_Set_DVP_AS_SRAM_Mode(MM_MISC_SHARED_SRAM_MODE_DVP_AS);

    MM_MISC_S2P_Init(&dvp_s2p);
    MM_MISC_Set_S2P_State(ENABLE);
    MM_MISC_Set_DVP_AS_TG_Cfg(&dvp_as_tg);
    MM_MISC_Set_DVP_AS_TG_State(ENABLE);
    MM_MISC_DVP_AS_Set_Cfg(&dvp_as);
    // MM_MISC_Set_DVP_AS_State(ENABLE);
}

static void isp_tsrc_cfg(uint16_t width, uint16_t height, uint32_t addr, bool conti)
{
    TSRC_Cfg_Type tsrc_cfg = {0};
    TSRC_TG_Cfg tsrc_tg_cfg = {0};

    tsrc_cfg.format = TSRC_PIXEL_YUV422_16BIT;
    tsrc_cfg.burst = TSRC_BURST_TYPE_INCR64;
    tsrc_cfg.pushMode = (conti)?0:1;
    tsrc_cfg.byte0 = 0;
    tsrc_cfg.byte1 = 1;
    tsrc_cfg.byte2 = 2;
    tsrc_cfg.imgSize.width = width;
    tsrc_cfg.imgSize.height = height;
    tsrc_cfg.imgSize.hBlank = TSRC_H_BLANK;
    tsrc_cfg.imgSize.vBlank = TSRC_V_BLANK;
    tsrc_cfg.memStart = addr;
    tsrc_cfg.prefetch = TSRC_V_BLANK - 4;

    tsrc_tg_cfg.totalHPixels = width + TSRC_H_BLANK;
    tsrc_tg_cfg.actStartPixel = 0;
    tsrc_tg_cfg.actPixelWidth = width;
    tsrc_tg_cfg.actStartLine = 0;
    tsrc_tg_cfg.actLineHeight = height;

    MM_MISC_Set_DVP_AS_SRAM_Mode(MM_MISC_SHARED_SRAM_MODE_DVP_TSRC);

    TSRC_Init(&tsrc_cfg);
    TSRC_Set_TG_Cfg(&tsrc_tg_cfg);
    TSRC_Set_TG_State(1);
    // TSRC_Enable();
}

static uint8_t find_scaler_coef(uint16_t input, uint16_t output)
{
    uint32_t rate; // ouput/input

    rate = output * SCALE_COEF_MULTI_ACCURACY / input;

    if (rate >= SCALE_COEF_MULTI_ACCURACY) {
        return 18;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 51 / 64) {
        return 17;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 42 / 64) {
        return 16;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 39 / 64) {
        return 15;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 36 / 64) {
        return 14;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 34 / 64) {
        return 13;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 32 / 64) {
        return 12;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 30 / 64) {
        return 11;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 29 / 64) {
        return 10;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 26 / 64) {
        return 9;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 25 / 64) {
        return 8;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 22 / 64) {
        return 7;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 21 / 64) {
        return 6;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 19 / 64) {
        return 5;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 18 / 64) {
        return 4;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 16 / 64) {
        return 3;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 15 / 64) {
        return 2;
    } else if (rate >= SCALE_COEF_MULTI_ACCURACY * 13 / 64) {
        return 1;
    } else {
        return 0;
    }
}

static void isp_scaler_size_cfg(uint8_t channel, SCALER_CONFIG_S size_cfg)
{
    if (0 == channel)
        SCALER_HYDRE_SetSizeCfg(SCALER_HYDRE_TYPE_A, size_cfg);
    else
        SCALER_HYDRE_SetSizeCfg(SCALER_HYDRE_TYPE_B, size_cfg);
}

static void isp_scaler_coef_cfg(uint8_t channel, SCALER_CONFIG_S size_cfg)
{
    SCALER_COEF_S coef = {0};
    uint8_t h_coef_idx, v_coef_idx;
#if 0
    // assume to call firmware to get the coef, current use hardcode to test
    if ( size_cfg.output.width >= size_cfg.input.width)
    {
        coef.x_coeff_set = 0;
        coef.x_coeff_ratio = 0;
    }
    else
    {
        coef.x_coeff_set = 2;
        coef.x_coeff_ratio = 8;
    }

    if ( size_cfg.output.height>= size_cfg.input.height)
    {
        coef.y_coeff_set = 0;
        coef.y_coeff_ratio = 0;
    }
    else
    {
        coef.y_coeff_set = 0;
        coef.y_coeff_ratio = 8;
    }
#else
    h_coef_idx = find_scaler_coef(size_cfg.input.width, size_cfg.output.width);
    v_coef_idx = find_scaler_coef(size_cfg.input.height, size_cfg.output.height);

    coef.x_coeff_set = g_scale_hor_coeff[h_coef_idx][0];
    coef.x_coeff_ratio = g_scale_hor_coeff[h_coef_idx][1];
    coef.y_coeff_set = g_scale_ver_coeff[v_coef_idx][0];
    coef.y_coeff_ratio = g_scale_ver_coeff[v_coef_idx][1];

#endif
    if (0 == channel)
        SCALER_HYDRE_SetXYCoef(SCALER_HYDRE_TYPE_A, coef);
    else
        SCALER_HYDRE_SetXYCoef(SCALER_HYDRE_TYPE_B, coef);
}

static void isp_restore_parameter(isp_hal_cfg_t *isp_hal_cfg)
{
    uint8_t i;

    // restore isp source
    isp_src_tmp = isp_hal_cfg->isp_src;

    // restore Callback function
    g_ispCallback = isp_hal_cfg->out_callback;

    // restore out port 0 format
    out_format0_tmp = isp_hal_cfg->out_ch_0.format;

    // check buffer pingpong number
    if (0 == isp_hal_cfg->out_ch_0.chn_common.pp_num) isp_hal_cfg->out_ch_0.chn_common.pp_num = 1; // force to set 1 when 0
    if (ISP_MAX_BUFFER_NUM < isp_hal_cfg->out_ch_0.chn_common.pp_num) isp_hal_cfg->out_ch_0.chn_common.pp_num = ISP_MAX_BUFFER_NUM; // force to set max buffer when overflow

    if (0 == isp_hal_cfg->out_ch_1.chn_common.pp_num) isp_hal_cfg->out_ch_1.chn_common.pp_num = 1; // force to set 1 when 0
    if (ISP_MAX_BUFFER_NUM < isp_hal_cfg->out_ch_1.chn_common.pp_num) isp_hal_cfg->out_ch_1.chn_common.pp_num = ISP_MAX_BUFFER_NUM; // force to set 1 when overflow

    if (0 == isp_hal_cfg->out_ch_ext.chn_common.pp_num) isp_hal_cfg->out_ch_ext.chn_common.pp_num = 1; // force to set 1 when 0
    if (ISP_MAX_BUFFER_NUM < isp_hal_cfg->out_ch_ext.chn_common.pp_num) isp_hal_cfg->out_ch_ext.chn_common.pp_num = ISP_MAX_BUFFER_NUM; // force to set 1 when overflow

    // restore out port pp number and address
    pp_num_tmp[0] = (isp_hal_cfg->out_ch_0.ring_line)?1:isp_hal_cfg->out_ch_0.chn_common.pp_num;
    pp_num_tmp[1] = isp_hal_cfg->out_ch_1.chn_common.pp_num;
    pp_num_tmp[2] = isp_hal_cfg->out_ch_ext.chn_common.pp_num;

    // reset pp_idx
    memset((void*)&pp_idx[0], 0, sizeof(pp_idx));

    for (i = 0; i < ISP_MAX_BUFFER_NUM; i++)
    {
        out_addr_tmp[0][i] = isp_hal_cfg->out_ch_0.chn_common.addr[i];
        out_addr_tmp[1][i] = isp_hal_cfg->out_ch_1.chn_common.addr[i];
        out_addr_tmp[2][i] = isp_hal_cfg->out_ch_ext.chn_common.addr[i];
    }

    // reset update flag
    memset((void*)&update_flag[0][0], 0, sizeof(update_flag));

    // reset channel enable
    memset((void*)&channel_en[0], 0, sizeof(channel_en));
}

static void isp_glb_crop_cfg(isp_hal_cfg_t *isp_hal_cfg)
{
    ISP_Crop_Cfg_Type crop_cfg = {0};

    // check crop config valid or not
    if (isp_hal_cfg->glb_crop.start_x & 0x1)
    {
        printf("global crop start_x should be even\r\n");
        isp_hal_cfg->glb_crop.start_x = 0;
    }
    if (isp_hal_cfg->in_size.width & 0x1)
    {
        printf("global crop width should be even\r\n");
        isp_hal_cfg->glb_crop.width = isp_hal_cfg->in_size.width;
    }
    if (isp_hal_cfg->glb_crop.start_x > isp_hal_cfg->in_size.width)
    {
        printf("global crop error start_x\r\n");
        isp_hal_cfg->glb_crop.start_x = 0;
    }
    if (isp_hal_cfg->glb_crop.start_y > isp_hal_cfg->in_size.height)
    {
        printf("global crop error start_y\r\n");
        isp_hal_cfg->glb_crop.start_y = 0;
    }
    if ((isp_hal_cfg->glb_crop.start_x + isp_hal_cfg->glb_crop.width) > isp_hal_cfg->in_size.width)
    {
        printf("global crop error start_x + width\r\n");
        isp_hal_cfg->glb_crop.start_x = 0;
        isp_hal_cfg->glb_crop.width = isp_hal_cfg->in_size.width;
    }
    if ((isp_hal_cfg->glb_crop.start_y + isp_hal_cfg->glb_crop.height) > isp_hal_cfg->in_size.height)
    {
        printf("global crop error start_y + height\r\n");
        isp_hal_cfg->glb_crop.start_y = 0;
        isp_hal_cfg->glb_crop.height = isp_hal_cfg->in_size.height;
    }

    crop_cfg.hsyncStart = isp_hal_cfg->glb_crop.start_x;
    crop_cfg.hsyncEnd = isp_hal_cfg->glb_crop.start_x + isp_hal_cfg->glb_crop.width;
    crop_cfg.vsyncStart = isp_hal_cfg->glb_crop.start_y;
    crop_cfg.vsyncEnd = isp_hal_cfg->glb_crop.start_y + isp_hal_cfg->glb_crop.height;

    ISP_Set_Crop_Cfg(&crop_cfg);
    ISP_Set_Crop_State(1);
}

static void isp_yuv_adj_cfg(isp_hal_cfg_t *isp_hal_cfg)
{
    if (isp_hal_cfg->yuv_adj_en)
    {
        ISP_YUV_Adjust_Init();
    }

    ISP_Set_YUV_Adjust_State(isp_hal_cfg->yuv_adj_en);
}

static void isp_seof1_set_callback(uint16_t line_cnt)
{
    // mm_seof1
    MM_MISC_SEOF_TYPE_E seof_type = {0};
    seof_type.BF.SoF = 1;
    seof_type.BF.EoF = 1;
    seof_type.BF.lineCnt = 1;
    MM_MISC_Set_SEOF_LineCnt(MM_MISC_SEOF1, line_cnt);
    MM_MISC_Set_SEOF_Src(MM_MISC_SEOF1, MM_MISC_SEOF_INTR_SRC_ISP_OUTPUT);
    MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF1, seof_type);

    Interrupt_Handler_Register(ISP_MM_SEOF1_IRQn, isp_seof1_isr);
    CPU_Interrupt_Enable(ISP_MM_SEOF1_IRQn);
}

static void isp_seof1_clr_callback(void)
{
    MM_MISC_SEOF_TYPE_E seof_type = {0};
    seof_type.BF.SoF = 0;
    seof_type.BF.EoF = 0;
    seof_type.BF.lineCnt = 0;
    MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF1, seof_type);

    CPU_Interrupt_Disable(ISP_MM_SEOF1_IRQn);
    Interrupt_Handler_Register(ISP_MM_SEOF1_IRQn, NULL);
}

static int check_zoom_cfg(uint8_t channel, SCALER_CONFIG_S *zoom_cfg)
{
    int ret = 0;

    // check crop config valid or not
    if (zoom_cfg->input.start_x & 0x1)
    {
        printf("channel %d zoom crop start_x should be even\r\n", channel);
        zoom_cfg->input.start_x = 0;
        ret = 1;
    }
    if (zoom_cfg->input.width & 0x1)
    {
        printf("channel %d zoom crop width should be even\r\n", channel);
        zoom_cfg->input.width = zoom_cfg->input.in_width;
        ret = 1;
    }
    if (zoom_cfg->input.start_x > zoom_cfg->input.in_width)
    {
        printf("channel %d zoom crop error start_x\r\n", channel);
        zoom_cfg->input.start_x = 0;
        ret = 1;
    }
    if (zoom_cfg->input.start_y > zoom_cfg->input.in_height)
    {
        printf("channel %d zoom crop error start_y\r\n", channel);
        zoom_cfg->input.start_y = 0;
        ret = 1;
    }
    if ((zoom_cfg->input.start_x + zoom_cfg->input.width) > zoom_cfg->input.in_width)
    {
        printf("channel %d zoom crop error start_x + width\r\n", channel);
        zoom_cfg->input.start_x = 0;
        zoom_cfg->input.width = zoom_cfg->input.in_width;
        ret = 1;
    }
    if ((zoom_cfg->input.start_y + zoom_cfg->input.height) > zoom_cfg->input.in_height)
    {
        printf("channel %d zoom crop error start_y + height\r\n", channel);
        zoom_cfg->input.start_y = 0;
        zoom_cfg->input.height = zoom_cfg->input.in_height;
        ret = 1;
    }

    return ret;
}

static void isp_scaler_cfg(uint8_t channel, isp_hal_cfg_t *isp_hal_cfg)
{
    SCALER_CONFIG_S scaler_size_cfg = {0};

    if (channel > 1)
    {
        printf("Channel overflow, force to set channel 0\r\n");
        channel = 0;
    }

    scaler_size_cfg.input.in_width = isp_hal_cfg->glb_crop.width;
    scaler_size_cfg.input.in_height = isp_hal_cfg->glb_crop.height;
    if (ISP_CHANNEL_0 == channel)
    {
        scaler_size_cfg.input.start_x = isp_hal_cfg->out_ch_0.scaler_cfg.in.start_x;
        scaler_size_cfg.input.start_y = isp_hal_cfg->out_ch_0.scaler_cfg.in.start_y;
        scaler_size_cfg.input.width = isp_hal_cfg->out_ch_0.scaler_cfg.in.width;
        scaler_size_cfg.input.height = isp_hal_cfg->out_ch_0.scaler_cfg.in.height;
        scaler_size_cfg.output.width = isp_hal_cfg->out_ch_0.scaler_cfg.out.width;
        scaler_size_cfg.output.height = isp_hal_cfg->out_ch_0.scaler_cfg.out.height;
    }
    else
    {
        scaler_size_cfg.input.start_x = isp_hal_cfg->out_ch_1.scaler_cfg.in.start_x;
        scaler_size_cfg.input.start_y = isp_hal_cfg->out_ch_1.scaler_cfg.in.start_y;
        scaler_size_cfg.input.width = isp_hal_cfg->out_ch_1.scaler_cfg.in.width;
        scaler_size_cfg.input.height = isp_hal_cfg->out_ch_1.scaler_cfg.in.height;
        scaler_size_cfg.output.width = isp_hal_cfg->out_ch_1.scaler_cfg.out.width;
        scaler_size_cfg.output.height = isp_hal_cfg->out_ch_1.scaler_cfg.out.height;
    }

    check_zoom_cfg(channel, &scaler_size_cfg);

    isp_scaler_size_cfg(channel, scaler_size_cfg);
    isp_scaler_coef_cfg(channel, scaler_size_cfg);

    // keep the scaler configuration, and later use this to do digital zoom
    memcpy((void*)&scaler_size_tmp[channel], (void*)&scaler_size_cfg, sizeof(scaler_size_cfg));
}

static void isp_dvp2axi_set_callback(uint8_t channel)
{
    if (ISP_CHANNEL_0 == channel)
    {
        Interrupt_Handler_Register(D2XA_IRQn, isp_channel0_ISR);
        CPU_Interrupt_Enable(D2XA_IRQn);
    }
    else if (ISP_CHANNEL_1 == channel)
    {
        Interrupt_Handler_Register(D2XC_IRQn, isp_channel1_ISR);
        CPU_Interrupt_Enable(D2XC_IRQn);
    }
    else //extend channel
    {
        Interrupt_Handler_Register(D2XB_IRQn, isp_channel_ext_ISR);
        CPU_Interrupt_Enable(D2XB_IRQn);
    }
}

static void isp_dvp2axi_clr_callback(uint8_t channel)
{
    if (ISP_CHANNEL_0 == channel)
    {
        CPU_Interrupt_Disable(D2XA_IRQn);
        Interrupt_Handler_Register(D2XA_IRQn, NULL);
    }
    else if (ISP_CHANNEL_1 == channel)
    {
        CPU_Interrupt_Disable(D2XC_IRQn);
        Interrupt_Handler_Register(D2XC_IRQn, NULL);
    }
    else
    {
        CPU_Interrupt_Disable(D2XB_IRQn);
        Interrupt_Handler_Register(D2XB_IRQn, NULL);
    }
}

static int check_fps_cfg(uint8_t src_fps, uint8_t dst_fps)
{
    if ((src_fps < 1) || (dst_fps > src_fps))
        return 1; // invalid fps configuration
    else
        return 0;
}

static void isp_set_channel_frame_rate(uint8_t src_fps, uint8_t dst_fps, uint32_t *bitwise_vld_frm, uint8_t *period_cnt)
{
    uint32_t bitwise_valid_frm = 0;
    int frm_cnt = 0;
    int frm_period_cnt = 0;

    // output drop
    if (0 == dst_fps)
    {
        *bitwise_vld_frm = 0;
        *period_cnt = 1;
        return;
    }

    // do frame rate
    while (1) {
        bitwise_valid_frm <<= 1;
        frm_cnt += dst_fps;
        frm_period_cnt++;
        if (frm_cnt >= src_fps) {
            bitwise_valid_frm |= 1;
            frm_cnt -= src_fps;
            if (frm_cnt == 0) {
                break;
            }
        }
    }
    *bitwise_vld_frm = bitwise_valid_frm;
    *period_cnt = frm_period_cnt;
}

static void select_ext_chn_src_size(isp_hal_cfg_t *isp_hal_cfg, uint16_t *width, uint16_t *height)
{
    MM_MISC_DVP2AXI_SRC ext_src;

    switch(isp_hal_cfg->out_ch_ext.ext_src)
    {
        case ISP_HAL_EXT_SRC_RAW:
            // tricky assign
            ext_src = (MM_MISC_DVP2AXI_SRC)isp_hal_cfg->isp_src;
            *width = isp_hal_cfg->in_size.width;
            *height = isp_hal_cfg->in_size.height;
        break;
        case ISP_HAL_EXT_SRC_OUT_CHN0_IN:
            ext_src = MM_MISC_DVP2AXI_SRC_SCALER_F0;
            *width = isp_hal_cfg->out_ch_0.scaler_cfg.out.width;
            *height = isp_hal_cfg->out_ch_0.scaler_cfg.out.height;
        break;
        case ISP_HAL_EXT_SRC_OUT_CHN1_IN:
            if (isp_hal_cfg->out_ch_ext.format <= ISP_HAL_OUT_FMT_EXT_Y_ONLY)
                ext_src = MM_MISC_DVP2AXI_SRC_SCALER_F1;
            else
                ext_src = MM_MISC_DVP2AXI_SRC_YUV2RGB;

            *width = isp_hal_cfg->out_ch_1.scaler_cfg.out.width;
            *height = isp_hal_cfg->out_ch_1.scaler_cfg.out.height;
        break;
        case ISP_HAL_EXT_SRC_ISP_OUT:
        default:
            ext_src = ISP_HAL_EXT_SRC_ISP_OUT;
            *width = isp_hal_cfg->glb_crop.width;
            *height = isp_hal_cfg->glb_crop.height;
    }

    MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_B, ext_src);
}

static int check_out_crop_cfg(uint8_t channel, isp_hal_cfg_t *isp_hal_cfg)
{
    int ret = 0;
    isp_hal_crop_t *out_crop;
    uint16_t width, height;

    if (0 == channel)
    {
        out_crop = &isp_hal_cfg->out_ch_0.chn_common.out_crop;
        width = isp_hal_cfg->out_ch_0.scaler_cfg.out.width;
        height = isp_hal_cfg->out_ch_0.scaler_cfg.out.height;
    }
    else if (1 == channel)
    {
        out_crop = &isp_hal_cfg->out_ch_1.chn_common.out_crop;
        width = isp_hal_cfg->out_ch_1.scaler_cfg.out.width;
        height = isp_hal_cfg->out_ch_1.scaler_cfg.out.height;
    }
    else
    {
        out_crop = &isp_hal_cfg->out_ch_ext.chn_common.out_crop;
        switch(isp_hal_cfg->out_ch_ext.ext_src)
        {
            case ISP_HAL_EXT_SRC_RAW:
                width = isp_hal_cfg->in_size.width;
                height = isp_hal_cfg->in_size.height;
            break;
            case ISP_HAL_EXT_SRC_OUT_CHN0_IN:
                width = isp_hal_cfg->out_ch_0.scaler_cfg.out.width;
                height = isp_hal_cfg->out_ch_0.scaler_cfg.out.height;
            break;
            case ISP_HAL_EXT_SRC_OUT_CHN1_IN:
                width = isp_hal_cfg->out_ch_1.scaler_cfg.out.width;
                height = isp_hal_cfg->out_ch_1.scaler_cfg.out.height;
            break;
            case ISP_HAL_EXT_SRC_ISP_OUT:
            default:
                width = isp_hal_cfg->glb_crop.width;
                height = isp_hal_cfg->glb_crop.height;
        }
    }

    // check crop config valid or not
    if (out_crop->start_x & 0x1)
    {
        printf("channel %d out crop start_x should be even\r\n", channel);
        ret = 1;
    }
    if (out_crop->width & 0x1)
    {
        printf("channel %d out crop width should be even\r\n", channel);
        ret = 1;
    }

    if ((0 == channel) && (ISP_HAL_OUT_FMT0_NV12 == isp_hal_cfg->out_ch_0.format))
    {
        if (out_crop->start_y & 0x1)
        {
            printf("channel %d out crop start_y should be even\r\n", channel);
            ret = 1;
        }
        if (out_crop->height & 0x1)
        {
            printf("channel %d out crop height should be even\r\n", channel);
            ret = 1;
        }
    }

    if (out_crop->start_x > out_crop->width)
    {
        printf("channel %d out crop error start_x\r\n", channel);
        ret = 1;
    }
    if (out_crop->start_y > out_crop->height)
    {
        printf("channel %d out crop error start_y\r\n", channel);
        ret = 1;
    }
    if ((out_crop->start_x + out_crop->width) > width)
    {
        printf("channel %d out crop error start_x + width\r\n", channel);
        ret = 1;
    }
    if ((out_crop->start_y + out_crop->height) > height)
    {
        printf("channel %d out crop error start_y + height\r\n", channel);
        ret = 1;
    }

    return ret;
}

static void isp_dvp2axi_cfg(uint8_t channel, isp_hal_cfg_t *isp_hal_cfg)
{
    DVP2AXI_INIT_CFG_S dvp2axi_cfg = {0};
    uint16_t width, height;
    uint32_t vld_frm = 0;
    uint8_t period_cnt = 0;
    uint16_t h_start, h_end, v_start, v_end; // for dvp2axi output crop

    // common config
    dvp2axi_cfg.framePol = DVP2AXI_DATA_ACTIVE_HIGH;
    dvp2axi_cfg.linePol = DVP2AXI_DATA_ACTIVE_HIGH;
    dvp2axi_cfg.swIntCnt = 1;
    dvp2axi_cfg.sensorMode = DVP2AXI_DATA_VALID_V_AND_H;

    if (ISP_CHANNEL_0 == channel) // channel 0 uses dvp2axi_A or dvp2axi_A + dvp2axi_B by format
    {
        if (check_fps_cfg(isp_hal_cfg->out_ch_0.chn_common.fr_ctrl.src_fps, isp_hal_cfg->out_ch_0.chn_common.fr_ctrl.dst_fps))
        {
            // invalid setting, always output
            vld_frm = 1;
            period_cnt = 1;
        }
        else
            isp_set_channel_frame_rate(isp_hal_cfg->out_ch_0.chn_common.fr_ctrl.src_fps, isp_hal_cfg->out_ch_0.chn_common.fr_ctrl.dst_fps, &vld_frm, &period_cnt);

        if (check_out_crop_cfg(channel, isp_hal_cfg)) // invalid cfg, no crop, dvp2axi out size refer scaler out size
        {
            width = isp_hal_cfg->out_ch_0.scaler_cfg.out.width;
            height = isp_hal_cfg->out_ch_0.scaler_cfg.out.height;
            h_start = 0;
            h_end   = 0x3FFF;
            v_start = 0;
            v_end   = 0x3FFF;
        }
        else // valid cfg, dvp2axi out size refer out crop size
        {
            width = isp_hal_cfg->out_ch_0.chn_common.out_crop.width;
            height = isp_hal_cfg->out_ch_0.chn_common.out_crop.height;
            h_start = isp_hal_cfg->out_ch_0.chn_common.out_crop.start_x;
            h_end   = isp_hal_cfg->out_ch_0.chn_common.out_crop.start_x + isp_hal_cfg->out_ch_0.chn_common.out_crop.width;
            v_start = isp_hal_cfg->out_ch_0.chn_common.out_crop.start_y;
            v_end   = isp_hal_cfg->out_ch_0.chn_common.out_crop.start_y + isp_hal_cfg->out_ch_0.chn_common.out_crop.height;
        }

        // yuyv format and use dvp2axi_A only
        if ((ISP_HAL_OUT_FMT0_YUYV == isp_hal_cfg->out_ch_0.format) || (ISP_HAL_OUT_FMT0_Y_ONLY == isp_hal_cfg->out_ch_0.format))
        {
            if (ISP_HAL_OUT_FMT0_YUYV == isp_hal_cfg->out_ch_0.format)
            {
                MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_A, MM_MISC_DVP2AXI_SRC_OSD_BLEND);
                dvp2axi_cfg.bitWidth = DVP2AXI_DATA_16_BIT;
            }
            else // Y only
            {
                MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_A, MM_MISC_DVP2AXI_SRC_YUV422TO420);
                dvp2axi_cfg.bitWidth = DVP2AXI_DATA_8_BIT;
            }

            DVP2AXI_Disable(DVP2AXI_PORT_A);

            dvp2axi_cfg.subSampleEn = 0;
            dvp2axi_cfg.wrapEn = (0 == isp_hal_cfg->out_ch_0.ring_line)?0:1;
            dvp2axi_cfg.wrapLine = isp_hal_cfg->out_ch_0.ring_line;
            dvp2axi_cfg.byte_sel = DVP2AXI_SEL_LOW_BYTE; // YUYV don't care, Y only select low byte
            dvp2axi_cfg.memStart = isp_hal_cfg->out_ch_0.chn_common.addr[0];
            dvp2axi_cfg.imgWidth = width;
            dvp2axi_cfg.imgHeight = height;

            DVP2AXI_Init(DVP2AXI_PORT_A, &dvp2axi_cfg);
            DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_A, isp_hal_cfg->out_ch_0.scaler_cfg.out.width, isp_hal_cfg->out_ch_0.scaler_cfg.out.height);

            DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_A, vld_frm, (period_cnt - 1));

            DVP2AXI_IntMask(DVP2AXI_PORT_A, DVP2AXI_INT_ALL, UNMASK);

            DVP2AXI_Crop(DVP2AXI_PORT_A, h_start, h_end, v_start, v_end);

            DVP2AXI_Enable(DVP2AXI_PORT_A);
        }
        else // nv12 format and use dvp2axi_A for Y and dvp2axi_B for UV
        {
            if (ISP_HAL_OUT_FMT0_YUV422SP == isp_hal_cfg->out_ch_0.format)
            {
                MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_A, MM_MISC_DVP2AXI_SRC_OSD_BLEND);
                MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_B, MM_MISC_DVP2AXI_SRC_OSD_BLEND);
            }
            else // NV12
            {
                MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_A, MM_MISC_DVP2AXI_SRC_YUV422TO420);
                MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_B, MM_MISC_DVP2AXI_SRC_YUV422TO420);
            }

            DVP2AXI_Disable(DVP2AXI_PORT_A);
            DVP2AXI_Disable(DVP2AXI_PORT_B);

            // Config dvp2axi a
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_8_BIT;
            dvp2axi_cfg.subSampleEn = 0;
            dvp2axi_cfg.wrapEn = (0 == isp_hal_cfg->out_ch_0.ring_line)?0:1;
            dvp2axi_cfg.wrapLine = isp_hal_cfg->out_ch_0.ring_line;
            dvp2axi_cfg.byte_sel = DVP2AXI_SEL_LOW_BYTE;
            dvp2axi_cfg.memStart = isp_hal_cfg->out_ch_0.chn_common.addr[0];
            dvp2axi_cfg.imgWidth = width;
            dvp2axi_cfg.imgHeight = height;

            DVP2AXI_Init(DVP2AXI_PORT_A, &dvp2axi_cfg);
            DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_A, isp_hal_cfg->out_ch_0.scaler_cfg.out.width, isp_hal_cfg->out_ch_0.scaler_cfg.out.height);

            DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_A, vld_frm, (period_cnt - 1));

            DVP2AXI_IntMask(DVP2AXI_PORT_A, DVP2AXI_INT_ALL, UNMASK);

            DVP2AXI_Crop(DVP2AXI_PORT_A, h_start, h_end, v_start, v_end);

            // Config dvp2axi b
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_8_BIT;
            dvp2axi_cfg.wrapEn = (0 == isp_hal_cfg->out_ch_0.ring_line)?0:1;
            if (ISP_HAL_OUT_FMT0_YUV422SP == isp_hal_cfg->out_ch_0.format)
                dvp2axi_cfg.wrapLine = isp_hal_cfg->out_ch_0.ring_line;
            else
                dvp2axi_cfg.wrapLine = (isp_hal_cfg->out_ch_0.ring_line >> 1);

            dvp2axi_cfg.byte_sel = DVP2AXI_SEL_HIGH_BYTE;

            if (0 == dvp2axi_cfg.wrapEn)
                dvp2axi_cfg.memStart = isp_hal_cfg->out_ch_0.chn_common.addr[0] + DVP2AXI_GetLineOffset(width) * height;
            else
                dvp2axi_cfg.memStart = isp_hal_cfg->out_ch_0.chn_common.addr[0] + DVP2AXI_GetLineOffset(width) * isp_hal_cfg->out_ch_0.ring_line;

            if (ISP_HAL_OUT_FMT0_YUV422SP == isp_hal_cfg->out_ch_0.format)
            {
                dvp2axi_cfg.subSampleEn = 0;
                dvp2axi_cfg.imgHeight = height;
            }
            else // NV12
            {
                dvp2axi_cfg.subSampleEn = 1;
                dvp2axi_cfg.imgHeight = (height >> 1);
            }

            DVP2AXI_Init(DVP2AXI_PORT_B, &dvp2axi_cfg);
            DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_B, isp_hal_cfg->out_ch_0.scaler_cfg.out.width, isp_hal_cfg->out_ch_0.scaler_cfg.out.height);

            DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_B, vld_frm, (period_cnt - 1));

            DVP2AXI_IntMask(DVP2AXI_PORT_B, DVP2AXI_INT_ALL, UNMASK);

            DVP2AXI_Crop(DVP2AXI_PORT_B, h_start, h_end, v_start, v_end);

            DVP2AXI_Enable(DVP2AXI_PORT_A);
            DVP2AXI_Enable(DVP2AXI_PORT_B);
        }
    }
    else if (ISP_CHANNEL_1 == channel) // channel 1 uses dvp2axi_C
    {
        MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT_C, MM_MISC_DVP2AXI_SRC_YUV2RGB);

        DVP2AXI_Disable(DVP2AXI_PORT_C);

        if (check_fps_cfg(isp_hal_cfg->out_ch_1.chn_common.fr_ctrl.src_fps, isp_hal_cfg->out_ch_1.chn_common.fr_ctrl.dst_fps))
        {
            // invalid setting, always output
            vld_frm = 1;
            period_cnt = 1;
        }
        else
            isp_set_channel_frame_rate(isp_hal_cfg->out_ch_1.chn_common.fr_ctrl.src_fps, isp_hal_cfg->out_ch_1.chn_common.fr_ctrl.dst_fps, &vld_frm, &period_cnt);

        if (check_out_crop_cfg(channel, isp_hal_cfg)) // invalid cfg, no crop, dvp2axi out size refer scaler out size
        {
            width = isp_hal_cfg->out_ch_1.scaler_cfg.out.width;
            height = isp_hal_cfg->out_ch_1.scaler_cfg.out.height;
            h_start = 0;
            h_end   = 0x3FFF;
            v_start = 0;
            v_end   = 0x3FFF;
        }
        else // valid cfg, dvp2axi out size refer out crop size
        {
            width = isp_hal_cfg->out_ch_1.chn_common.out_crop.width;
            height = isp_hal_cfg->out_ch_1.chn_common.out_crop.height;
            h_start = isp_hal_cfg->out_ch_1.chn_common.out_crop.start_x;
            h_end   = isp_hal_cfg->out_ch_1.chn_common.out_crop.start_x + isp_hal_cfg->out_ch_1.chn_common.out_crop.width;
            v_start = isp_hal_cfg->out_ch_1.chn_common.out_crop.start_y;
            v_end   = isp_hal_cfg->out_ch_1.chn_common.out_crop.start_y + isp_hal_cfg->out_ch_1.chn_common.out_crop.height;
        }

        if (ISP_HAL_OUT_FMT1_RGB == isp_hal_cfg->out_ch_1.format)
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_24_BIT;
        else
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_24_EXP_32BIT;

        dvp2axi_cfg.subSampleEn = 0;
        dvp2axi_cfg.wrapEn = 0;
        dvp2axi_cfg.wrapLine = 0;
        dvp2axi_cfg.byte_sel = DVP2AXI_SEL_LOW_BYTE; // don't care when 24-bit or 32-bit mode
        dvp2axi_cfg.memStart = isp_hal_cfg->out_ch_1.chn_common.addr[0];
        dvp2axi_cfg.imgWidth = width;
        dvp2axi_cfg.imgHeight = height;

        DVP2AXI_Init(DVP2AXI_PORT_C, &dvp2axi_cfg);
        DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_C, isp_hal_cfg->out_ch_1.scaler_cfg.out.width, isp_hal_cfg->out_ch_1.scaler_cfg.out.height);

        DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_C, vld_frm, (period_cnt - 1));

        DVP2AXI_IntMask(DVP2AXI_PORT_C, DVP2AXI_INT_ALL, UNMASK);

        DVP2AXI_Crop(DVP2AXI_PORT_C, h_start, h_end, v_start, v_end);

        DVP2AXI_Enable(DVP2AXI_PORT_C);
    }
    else // channel 2(extend) uses dvp2axi_B when channel 0 uses single dvp2axi (dvp2axi_A)
    {
        uint16_t ext_in_width, ext_in_height;
        select_ext_chn_src_size(isp_hal_cfg, &ext_in_width, &ext_in_height);

        DVP2AXI_Disable(DVP2AXI_PORT_B);

        if (check_fps_cfg(isp_hal_cfg->out_ch_ext.chn_common.fr_ctrl.src_fps, isp_hal_cfg->out_ch_ext.chn_common.fr_ctrl.dst_fps))
        {
            // invalid setting, always output
            vld_frm = 1;
            period_cnt = 1;
        }
        else
            isp_set_channel_frame_rate(isp_hal_cfg->out_ch_ext.chn_common.fr_ctrl.src_fps, isp_hal_cfg->out_ch_ext.chn_common.fr_ctrl.dst_fps, &vld_frm, &period_cnt);

        if (check_out_crop_cfg(channel, isp_hal_cfg)) // invalid cfg, no crop, dvp2axi out size refer scaler out size
        {
            width = ext_in_width;
            height = ext_in_height;
            h_start = 0;
            h_end   = 0x3FFF;
            v_start = 0;
            v_end   = 0x3FFF;
        }
        else // valid cfg, dvp2axi out size refer out crop size
        {
            width = isp_hal_cfg->out_ch_ext.chn_common.out_crop.width;
            height = isp_hal_cfg->out_ch_ext.chn_common.out_crop.height;
            h_start = isp_hal_cfg->out_ch_ext.chn_common.out_crop.start_x;
            h_end   = isp_hal_cfg->out_ch_ext.chn_common.out_crop.start_x + isp_hal_cfg->out_ch_ext.chn_common.out_crop.width;
            v_start = isp_hal_cfg->out_ch_ext.chn_common.out_crop.start_y;
            v_end   = isp_hal_cfg->out_ch_ext.chn_common.out_crop.start_y + isp_hal_cfg->out_ch_ext.chn_common.out_crop.height;
        }

        if (ISP_HAL_OUT_FMT_EXT_YUYV == isp_hal_cfg->out_ch_ext.format)
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_16_BIT;
        else if (ISP_HAL_OUT_FMT_EXT_Y_ONLY == isp_hal_cfg->out_ch_ext.format)
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_8_BIT;
        else if (ISP_HAL_OUT_FMT_EXT_RGB == isp_hal_cfg->out_ch_ext.format)
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_24_BIT;
        else
            dvp2axi_cfg.bitWidth = DVP2AXI_DATA_24_EXP_32BIT;

        dvp2axi_cfg.subSampleEn = 0;
        dvp2axi_cfg.byte_sel = DVP2AXI_SEL_LOW_BYTE; // Y only select low byte, others don't care
        dvp2axi_cfg.wrapEn = 0;
        dvp2axi_cfg.wrapLine = 0;
        dvp2axi_cfg.memStart = isp_hal_cfg->out_ch_ext.chn_common.addr[0];
        dvp2axi_cfg.imgWidth = width;
        dvp2axi_cfg.imgHeight = height;

        DVP2AXI_Init(DVP2AXI_PORT_B, &dvp2axi_cfg);
        DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_B, ext_in_width, ext_in_height);

        DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_B, vld_frm, (period_cnt - 1));

        DVP2AXI_IntMask(DVP2AXI_PORT_B, DVP2AXI_INT_ALL, UNMASK);

        DVP2AXI_Crop(DVP2AXI_PORT_B, h_start, h_end, v_start, v_end);

        DVP2AXI_Enable(DVP2AXI_PORT_B);
    }
}

// ============================================================================
// public function
// ============================================================================
int isp_init(isp_hal_cfg_t *isp_hal_cfg)
{
    if (isp_hal_cfg->in_size.width > ISP_MAX_IN_WIDTH)
    {
        printf("isp input width over %d\r\n", ISP_MAX_IN_WIDTH);
        return 1;
    }

    // restore some setting
    isp_restore_parameter(isp_hal_cfg);

    // enable seof1 interrupt and set callback func
    isp_seof1_set_callback(EARLY_LINE_CNT(isp_hal_cfg->in_size.height));

    // set isp input path
    MM_MISC_Set_YUV422_In_Mux(MM_MISC_YUV422_IN_MUX_ID_ISP, (MM_MISC_YUV422_IN_MUX_SEL)isp_hal_cfg->isp_src);
    if (ISP_HAL_SRC_DVP_AS == isp_hal_cfg->isp_src)
        isp_dvp_as_cfg(isp_hal_cfg);
    else if (ISP_HAL_SRC_SPI2DVP == isp_hal_cfg->isp_src)
        isp_spi2dvp_cfg(isp_hal_cfg);
    else
        isp_tsrc_cfg(isp_hal_cfg->in_size.width, isp_hal_cfg->in_size.height, isp_hal_cfg->src_cfg.tsrc_input.input_addr, isp_hal_cfg->src_cfg.tsrc_input.tsrc_conti);

    // set isp default path
    isp_scaler_path_init();
    isp_osd_path_init();
    isp_yuv422To420_init();
    isp_yuv2rgb_init();

    // crop function in isp output, it would affect sub_system input size
    isp_glb_crop_cfg(isp_hal_cfg);

    // yuvadj config
    isp_yuv_adj_cfg(isp_hal_cfg);

    // set output path 0 if enable
    if (0 != isp_hal_cfg->out_ch_0.enable)
    {
        // set scaler path 0
        isp_scaler_cfg(ISP_CHANNEL_0, isp_hal_cfg);

        // set dvp2axi A & B for channel 0
        isp_dvp2axi_cfg(ISP_CHANNEL_0, isp_hal_cfg);

        // enable interrupt and set callback func
        isp_dvp2axi_set_callback(ISP_CHANNEL_0);

        channel_en[ISP_CHANNEL_0] = 1;
    }

    // set output path 1 if enable
    if (0 != isp_hal_cfg->out_ch_1.enable)
    {
        // set scaler path 1
        isp_scaler_cfg(ISP_CHANNEL_1, isp_hal_cfg);

        // set dvp2axi C for channel 1
        isp_dvp2axi_cfg(ISP_CHANNEL_1, isp_hal_cfg);

        // enable interrupt and set callback func
        isp_dvp2axi_set_callback(ISP_CHANNEL_1);

        channel_en[ISP_CHANNEL_1] = 1;
    }

    // set output path extend if enable
    if (0 != isp_hal_cfg->out_ch_ext.enable)
    {
        // check channel 0 format and enable or not
        if (isp_hal_cfg->out_ch_0.enable && ((ISP_HAL_OUT_FMT0_YUV422SP == isp_hal_cfg->out_ch_0.format) || (ISP_HAL_OUT_FMT0_NV12 == isp_hal_cfg->out_ch_0.format)))
        {
            // two dvp2axi used for channel 0, extend channel can not be used here
            printf("Extend channel can not be used because of channel 0 format\r\n");
        }
        else
        {
            // set dvp2axi B for channel extend
            isp_dvp2axi_cfg(ISP_CHANNEL_EXT, isp_hal_cfg);

            // enable interrupt and set callback func
            isp_dvp2axi_set_callback(ISP_CHANNEL_EXT);

            channel_en[ISP_CHANNEL_EXT] = 1;
        }
    }

    memcpy((void*)&g_isp_cfg_tmp, isp_hal_cfg, sizeof(isp_hal_cfg_t));

    return 0;
}

int isp_start(void)
{
    if (ISP_HAL_SRC_SPI2DVP == isp_src_tmp)
        MM_MISC_Set_SPI2DVP_State(ENABLE);
    else if (ISP_HAL_SRC_DVP_AS == isp_src_tmp)
        MM_MISC_Set_DVP_AS_State(ENABLE);
    else if (ISP_HAL_SRC_TSRC == isp_src_tmp)
        TSRC_Enable();

    return 0;
}

int isp_deinit(void)
{
    // clear callback function
    if (channel_en[ISP_CHANNEL_0])
        isp_dvp2axi_clr_callback(ISP_CHANNEL_0);
    if (channel_en[ISP_CHANNEL_1])
        isp_dvp2axi_clr_callback(ISP_CHANNEL_1);
    if (channel_en[ISP_CHANNEL_EXT])
        isp_dvp2axi_clr_callback(ISP_CHANNEL_EXT);

    if (ISP_HAL_SRC_SPI2DVP == isp_src_tmp)
        MM_MISC_Set_SPI2DVP_State(DISABLE);
    else if (ISP_HAL_SRC_DVP_AS == isp_src_tmp)
        MM_MISC_Set_DVP_AS_State(DISABLE);
    else if (ISP_HAL_SRC_TSRC == isp_src_tmp)
        TSRC_Disable();

    isp_seof1_clr_callback();

    return 0;
}

int isp_tsrc_trigger(void)
{
    TSRC_Push_Mode_Trigger();
    return 0;
}

int isp_zoom_cfg(uint8_t channel, isp_hal_crop_t scaler_in)
{
    int ret = 0;

    if (channel >= PHY_CHANNEL_NUM)
    {
        printf("Zoom support channel 0 & 1 only \r\n");
        return 1;
    }

    if (0 == channel_en[channel])
    {
        printf("Channel %d does not enable \r\n", channel);
        return 2;
    }

    // assume isp is running and update this cfg in line cnt interrupt
    scaler_size_tmp[channel].input.start_x = scaler_in.start_x;
    scaler_size_tmp[channel].input.start_y = scaler_in.start_y;
    scaler_size_tmp[channel].input.width = scaler_in.width;
    scaler_size_tmp[channel].input.height = scaler_in.height;
    ret = check_zoom_cfg(channel, &scaler_size_tmp[channel]);

    if (0 == ret)
        update_flag[ISP_UPDATE_FLAG_SCALER][channel] = 1;

    return ret;
}

int isp_fps_cfg(uint8_t channel, isp_hal_frame_rate_t frc)
{
    int ret = 0;

    if (channel >= CHANNEL_NUM)
    {
        printf("Invalid channel\r\n");
        return 1;
    }

    if (0 == channel_en[channel])
    {
        printf("Channel %d does not enable \r\n", channel);
        return 2;
    }

    ret = check_fps_cfg(frc.src_fps, frc.dst_fps);

    if (0 == ret)
    {
        frc_tmp[channel].src_fps = frc.src_fps;
        frc_tmp[channel].dst_fps = frc.dst_fps;
        update_flag[ISP_UPDATE_FLAG_FRC][channel] = 1;
    }

    // direct set this value
    update_fps_cfg();
    return ret;
}

int isp_update_chn0_cfg(isp_hal_out_chnl_0_t chn0_cfg)
{
    memcpy((void*)&g_isp_cfg_tmp.out_ch_0, (void*)&chn0_cfg, sizeof(isp_hal_out_chnl_0_t));

    // set output path 0 if enable
    if (0 != g_isp_cfg_tmp.out_ch_0.enable)
    {
        // set scaler path 0
        isp_scaler_cfg(ISP_CHANNEL_0, &g_isp_cfg_tmp);

        // set dvp2axi A & B for channel 0
        isp_dvp2axi_cfg(ISP_CHANNEL_0, &g_isp_cfg_tmp);

        if (0 == channel_en[ISP_CHANNEL_0])
        {
            // enable interrupt and set callback func
            isp_dvp2axi_set_callback(ISP_CHANNEL_0);
        }

        channel_en[ISP_CHANNEL_0] = 1;
    }
    else
    {
        // do disable (TBD)
        channel_en[ISP_CHANNEL_0] = 0;
    }

    return 0;
}

int isp_update_chn1_cfg(isp_hal_out_chnl_1_t chn1_cfg)
{
    memcpy((void*)&g_isp_cfg_tmp.out_ch_1, (void*)&chn1_cfg, sizeof(isp_hal_out_chnl_1_t));

    // set output path 1 if enable
    if (0 != g_isp_cfg_tmp.out_ch_1.enable)
    {
        // set scaler path 1
        isp_scaler_cfg(ISP_CHANNEL_1, &g_isp_cfg_tmp);

        // set dvp2axi C for channel 1
        isp_dvp2axi_cfg(ISP_CHANNEL_1, &g_isp_cfg_tmp);

        if (0 == channel_en[ISP_CHANNEL_1])
        {
            // enable interrupt and set callback func
            isp_dvp2axi_set_callback(ISP_CHANNEL_1);
        }

        channel_en[ISP_CHANNEL_1] = 1;
    }
    else
    {
        // do disable (TBD)
        channel_en[ISP_CHANNEL_1] = 0;
    }

    return 0;
}

void isp_get_input_resolution(uint16_t *width, uint16_t *height)
{
    *width  = g_isp_cfg_tmp.in_size.width;
    *height = g_isp_cfg_tmp.in_size.height;
}

