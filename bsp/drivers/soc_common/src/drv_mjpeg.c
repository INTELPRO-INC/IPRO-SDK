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

#include "soc_base.h"
#include "drv_mjpeg.h"
#include "drv_mjcomb.h"
#include "drv_glb.h"

static const uint16_t q_table_50_y[64] = {
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99
};

static const uint16_t q_table_50_uv[64] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};

volatile MJCOMBO_HW_STATE g_nMJComboHWState = MJCOMBO_HW_STATE_UNINIT;
drv_mjpeg_sw_mode_e  g_nMJpegSwMode = MJPEG_SW_FRAME_MODE;
uint32_t  g_udOutBufSize, g_udOutBufAddr;

static void drv_mjpeg_set_yuv422_interleave_order(uint8_t y0, uint8_t u0, uint8_t y1, uint8_t v0)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_6_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_Y0_ORDER, y0);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_U0_ORDER, u0);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_V0_ORDER, v0);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_Y1_ORDER, y1);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_6_OFFSET);
}

static void drv_mjpeg_set_framesize(uint16_t x, uint16_t y)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_FRAME_WBLK, x);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_FRAME_HBLK, y);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_3_OFFSET);
}

int drv_mjpeg_init(const struct drv_mjpeg_config_t *config)
{
    uint32_t tmpVal = 0;
    uint16_t blocks;

    if(g_nMJComboHWState != MJCOMBO_HW_STATE_UNINIT)
    {
        /*for JPEG combo HW not support using encode and decode at the same time*/
        if(!IS_MJCOMBO_HW_STATE_FOR_ENC(g_nMJComboHWState))
            return -1;
    }
    g_nMJComboHWState = MJCOMBO_HW_STATE_ENC_INIT;


    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);
//    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_MJCOMB_ENABLE);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_MJCOMB_MODE);   // mode = 0: encoder
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);

    #if defined(CONFIG_IPRO6)
        /*work around to fix IPRO6 run case17 with 4K swap buffer will hangup issue.*/        //bug fixed with ipro6s_MMJDDR4_20260119175500 bit file. 
        //tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_AXI_ROBUST_DIS, 1);
    #endif
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_W_XLEN, MJCOMB_AXI_XLEN_INCR16);

    switch (config->format) {
        case MJPEG_FORMAT_YUV422_YUYV:
            tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_YUV_MODE, 3);
            drv_mjpeg_set_yuv422_interleave_order(0, 1, 2, 3);
            drv_mjpeg_set_framesize((config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV422_YVYU:
            tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_YUV_MODE, 3);
            drv_mjpeg_set_yuv422_interleave_order(0, 3, 2, 1);
            drv_mjpeg_set_framesize((config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV422_UYVY:
            tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_YUV_MODE, 3);
            drv_mjpeg_set_yuv422_interleave_order(1, 0, 3, 2);
            drv_mjpeg_set_framesize((config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV422_VYUY:
            tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_YUV_MODE, 3);
            drv_mjpeg_set_yuv422_interleave_order(1, 2, 3, 0);
            drv_mjpeg_set_framesize((config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV422SP_NV16:
            tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_YUV_MODE, 2);
            tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ORDER_U_EVEN);
            drv_mjpeg_set_framesize((config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV422SP_NV61:
            tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_YUV_MODE, 2);
            tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ORDER_U_EVEN);
            drv_mjpeg_set_framesize((config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            break;
        case MJPEG_FORMAT_YUV420SP_NV12:    /*Y+UV*/
            tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_YUV_MODE, 0);
            tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ORDER_U_EVEN);
            //if (config->resolution_x % 16) {
            //    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_LAST_HF_WBLK_DMY);
            //}
            drv_mjpeg_set_framesize((config->resolution_x + 15) >> 4, (config->resolution_y + 15) >> 4);
            break;
        case MJPEG_FORMAT_YUV420SP_NV21:    /*Y+VU*/
            tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_YUV_MODE, 0);
            tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ORDER_U_EVEN);
            if (config->resolution_x % 16) {
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_LAST_HF_WBLK_DMY);
            }
            drv_mjpeg_set_framesize((config->resolution_x + 15) >> 4, (config->resolution_y + 15) >> 4);
            break;
        case MJPEG_FORMAT_GRAY:
            tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_YUV_MODE, 1);

            //if (config->resolution_x % 16) {
            //    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_LAST_HF_WBLK_DMY);
            //}
            //
            //if (config->resolution_y % 16) {
            //    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_LAST_HF_HBLK_DMY);
            //}

            drv_mjpeg_set_framesize((config->resolution_x + 7) >> 3, (config->resolution_y + 7) >> 3);
            break;
        default:
            break;
    }
    switch (config->format) {
        case MJPEG_FORMAT_YUV422_YUYV:
        case MJPEG_FORMAT_YUV422_YVYU:
        case MJPEG_FORMAT_YUV422_UYVY:
        case MJPEG_FORMAT_YUV422_VYUY:
        case MJPEG_FORMAT_YUV422SP_NV16:
        case MJPEG_FORMAT_YUV422SP_NV61:
            if (config->resolution_x % 16) {
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_LAST_HF_WBLK_DMY);
            }
            if (config->resolution_y % 8) {
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_LAST_HF_HBLK_DMY);
            }
            break;
        case MJPEG_FORMAT_YUV420SP_NV12:    /*Y+UV*/
        case MJPEG_FORMAT_YUV420SP_NV21:    /*Y+VU*/
            if (config->resolution_x % 16) {
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_LAST_HF_WBLK_DMY);
            }
            if (config->resolution_y % 16) {
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_LAST_HF_HBLK_DMY);
            }
            break;
        case MJPEG_FORMAT_GRAY:
            if (config->resolution_x % 8) {
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_LAST_HF_WBLK_DMY);
            }
            if (config->resolution_y % 8) {
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_LAST_HF_HBLK_DMY);
            }
            break;

        default:
            break;
    }
       
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_SWAP_MODE, config->output_swap_mode_en);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_FRAME_CNT_TRGR_INT, 1); /* trigger int with one frame */
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_WAIT_CYCLE, 0x100);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_MODE);
    //removed //tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL, config->yy_dvp2axi_sel); /*for HW auto mode, dvp2axi_sel for yy input*/
    //removed //tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL, config->uv_dvp2axi_sel); /*for HW auto mode, dvp2axi_sel for uv input*/
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);

    writel(config->input_bufaddr0, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_1_OFFSET);
    writel(config->input_bufaddr1, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_2_OFFSET);
    
    /*for setting input memory buffer size*/
    blocks = config->rows / 8;

    switch (config->format) {
        case MJPEG_FORMAT_YUV422_YUYV:
        case MJPEG_FORMAT_YUV422_YVYU:
        case MJPEG_FORMAT_YUV422_UYVY:
        case MJPEG_FORMAT_YUV422_VYUY:
            writel((0 << 16) + blocks, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_3_OFFSET); /* uv << 16 + yy */
            break;
        case MJPEG_FORMAT_YUV422SP_NV16:
        case MJPEG_FORMAT_YUV422SP_NV61:
            writel((blocks << 16) + blocks, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_3_OFFSET);  /*YY_MEM_HBLK unit is 8 for 420. UV_MEM_HBLK unit is 8 for 420*/
            break;
        case MJPEG_FORMAT_YUV420SP_NV12:
        case MJPEG_FORMAT_YUV420SP_NV21:
            writel(((blocks/2) << 16) + (blocks/2), MJPEG_BASE + MJCOMB_CSR_JENC_CSR_3_OFFSET);  /*YY_MEM_HBLK unit is 16 for 420. UV_MEM_HBLK unit is 16 for 420*/
            break;
        case MJPEG_FORMAT_GRAY:
            writel((0 << 16) + blocks, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_3_OFFSET);
            break;

        default:
            break;
    }

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_JP_ADDR, config->output_bufaddr >> 2);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_4_OFFSET);
    writel(config->output_bufsize >> 7, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_2_OFFSET);
    g_udOutBufSize = (config->output_bufsize >> 7) << 7;
    g_udOutBufAddr = (config->output_bufaddr >> 2) << 2;


    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_4_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_NORMAL_EN);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_CAM_EN);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_MEM_EN);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_FRAME_EN);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_IDLE_EN);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_SWAP_EN);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_4_OFFSET);

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_5_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_HEAD_BYTE, 0);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_TAIL_EXP, 0);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_5_OFFSET);

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET);
    tmpVal |= 0xFF; // Clear Interrupt
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET);

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_0_OFFSET);
    //tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_READ_FWRAP);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_HW_FRAME, 0x0);    /*HW encode frames and then auto stop for HW auto mode, 0: Non auto stop, range 0~63*/

    //removed //tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_HW_KICK_MODE, config->hw_kick_mode_en);    /*for HW auto mode, enable kick mode. 1'b0 - Auto mode, 1'b1 - Kick mode*/
    //tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_W_LOOP_MODE, 0x0);   /*Write bitstream loop mode disable*/
    //tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_READ_FWRAP);
    //tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_REFLECT_DMY);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_HW_MODE_SWEN, config->hw_mode_swen_en);
    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_BIT_ORDER);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_0_OFFSET);


    uint16_t tmp_table_y[64] = { 0 };
    uint16_t tmp_table_uv[64] = { 0 };

    if (config->input_yy_table) {
        drv_mjpeg_calculate_quantize_table(config->quality, config->input_yy_table, tmp_table_y);
    } else {
        drv_mjpeg_calculate_quantize_table(config->quality, (uint16_t *)q_table_50_y, tmp_table_y);
    }
    if (config->input_uv_table) {
        drv_mjpeg_calculate_quantize_table(config->quality, config->input_uv_table, tmp_table_uv);
    } else {
        drv_mjpeg_calculate_quantize_table(config->quality, (uint16_t *)q_table_50_uv, tmp_table_uv);
    }

    drv_mjpeg_fill_quantize_table(tmp_table_y, tmp_table_uv);
    
    return 0;
}

int drv_mjpeg_deinit(void)
{
    int ret = 0;

    if(g_nMJComboHWState != MJCOMBO_HW_STATE_UNINIT)
    {
        if(IS_MJCOMBO_HW_STATE_FOR_ENC(g_nMJComboHWState))
            g_nMJComboHWState = MJCOMBO_HW_STATE_UNINIT;
        else
            ret = -1;   /*Error! Current MJCOMBO_HW_STATE is for decode.*/
    }

    return ret;
}

void drv_mjpeg_start(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_MJCOMB_ENABLE);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);
}

void drv_mjpeg_stop(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_MJCOMB_ENABLE);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);

#if  defined(CONFIG_IPRO6)  
    /*Set kick mode 0 bcz set kick_mode=1 will enter kick mode directly and doesn't need to set sw_mode/su_run now after ipro6s_MMJDDR4_20260121144334 bit file.*/
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_KICK_MODE);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_SW_FRAME, 0);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    return;
#endif

}

void drv_mjpeg_sw_enable(uint8_t frame_count, drv_mjpeg_sw_mode_e mode)
{
    g_nMJpegSwMode = mode;

#if defined(CONFIG_IPRO7) || defined(CONFIG_IPRO6)
    uint32_t tmpVal;
#endif

#if  defined(CONFIG_IPRO7)
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_SW_KICK_MODE, mode);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_SW_FRAME, frame_count);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);

    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_MODE);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_MODE);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
#endif
#if  defined(CONFIG_IPRO6)
    if(mode == MJPEG_SW_KICK_MODE)  /*IPRO6 SW kick mode dosen't need to set sw_mode/sw_run/sw_frame, just set kick_mode=1 then kick.*/
    {
        tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
        tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_SW_KICK_MODE, mode);
        writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    }
    else
    {
        tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
        tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_SW_KICK_MODE, mode);
        tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_SW_FRAME, frame_count);
        writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);

        tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_MODE);
        writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);

        tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_MODE);
        writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    }
#endif

}

void drv_mjpeg_sw_run(void)
{
    volatile uint32_t tmpVal;

#if  defined(CONFIG_IPRO6)
    if(g_nMJpegSwMode == MJPEG_SW_KICK_MODE)    /*IPRO6 SW kick mode dosen't need to set sw_mode/sw_run/sw_frame, just set kick_mode=1 then kick.*/
        return;
#endif
    
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);  /*need to set sw_mode then sw_run, or HW woun't start.*/
    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_MODE);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_RUN);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_MODE);
    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_RUN);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_RUN);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
}

void drv_mjpeg_sw_run_abort(void)
{
    uint32_t tmpVal = 0;
        
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_ABORT);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_ABORT);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
}


void drv_mjpeg_sw_kick_block(uint16_t kick_count)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_9_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_SW_KICK_HBLK, kick_count);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_9_OFFSET);
}

void drv_mjpeg_sw_kick_stop(void)
{
    uint32_t tmpVal;
#if  defined(CONFIG_IPRO6)  /*IPRO6 SW kick mode dosen't need to set sw_mode/sw_run/sw_frame, just set kick_mode=1 then kick.*/
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_KICK_MODE);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_SW_FRAME, 0);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    return;
#endif
    
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_KICK_MODE);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_SW_FRAME, 0);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_MODE);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_MODE);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
}

void drv_mjpeg_sw_kick(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_SW_KICK);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
}

//removed //void drv_mjpeg_hw_kick(void)
//removed //{
//removed //    uint32_t tmpVal;
//removed //
//removed //    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET);
//removed //    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_HW_KICK);
//removed //    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET);
//removed //}

void drv_mjpeg_tcint_mask(bool mask)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_4_OFFSET);

    if (mask) {
        tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_NORMAL_EN);
    } else {
        tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_NORMAL_EN);
    }

    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_4_OFFSET);
}

void drv_mjpeg_swapint_mask(bool mask)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_4_OFFSET);

    if (mask) {
        tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_SWAP_EN);

    } else {
        tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_SWAP_EN);
    }

    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_4_OFFSET);
}
void drv_mjpeg_idleint_mask(bool mask)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_4_OFFSET);

    if (mask) {
        tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_IDLE_EN);

    } else {
        tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_IDLE_EN);
    }

    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_4_OFFSET);
}
void drv_mjpeg_errint_mask(bool mask)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_4_OFFSET);

    if (mask) {
        tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_CAM_EN);
        tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_MEM_EN);
        tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_FRAME_EN);
        tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_IDLE_EN);
        tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_SWAP_EN);

    } else {
        tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_CAM_EN);
        tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_MEM_EN);
        tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_FRAME_EN);
//        tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_IDLE_EN);
//        tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_INT_SWAP_EN);
    }

    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_4_OFFSET);
}

uint32_t drv_mjpeg_get_intstatus(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET);
    tmpVal &= 0x3f00;

    return tmpVal;
}

void drv_mjpeg_int_clear(uint32_t int_clear)
{
    uint32_t tmpVal;

    int_clear = (int_clear >> 8) & 0x3f;
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET);
    tmpVal |= int_clear; // Clear Interrupt
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET);
}

uint8_t drv_mjpeg_get_frame_count(void)
{
    return reg_get_bits(readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET), MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT);
}

void drv_mjpeg_pop_one_frame(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET);
    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_RFIFO_POP);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET);
}

uint32_t drv_mjpeg_get_frame_info(uint32_t *addr)
{
    uint32_t bytes;

    *addr = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_10_OFFSET);
    bytes = (readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_11_OFFSET) + 7) >> 3;

    return bytes;
}

void drv_mjpeg_calculate_quantize_table(uint8_t quality, uint16_t *input_table, uint16_t *output_table)
{
    uint32_t scale_factor, i;

    if (quality == 0) {
        quality = 1;
    } else if (quality > 100) {
        quality = 100;
    }

    if (quality < 50) {
        scale_factor = 5000000 / quality;
    } else {
        scale_factor = 200000 - quality * 2000;
    }

    for (i = 0; i < 64; i++) {
        output_table[i] = (input_table[i] * scale_factor + 50000) / 100000;

        if (output_table[i] == 0) {
            output_table[i] = 1;
        } else if (output_table[i] > 0xff) {
            output_table[i] = 0xff;
        }
    }
}

void drv_mjpeg_fill_quantize_table(uint16_t *input_yy, uint16_t *input_uv)
{
#define MJPEG_Q_PARAM_00_OFFSET (0x400)
#define MJPEG_Q_PARAM_40_OFFSET (0x480)

    uint8_t i, j;
    uint16_t tmp1;
    uint16_t tmp2;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            tmp1 = 2048 / input_yy[16 * j + i];
            tmp2 = 2048 / input_yy[16 * j + i + 8];

            if (20480 / input_yy[16 * j + i] % 10 > 4) {
                tmp1++;
            }

            if (20480 / input_yy[16 * j + i + 8] % 10 > 4) {
                tmp2++;
            }

            writel(tmp1 | tmp2 << 16, MJPEG_BASE + MJPEG_Q_PARAM_00_OFFSET + (i * 4 + j) * 4);
        }
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            tmp1 = 2048 / input_uv[16 * j + i];
            tmp2 = 2048 / input_uv[16 * j + i + 8];

            if (20480 / input_uv[16 * j + i] % 10 > 4) {
                tmp1++;
            }

            if (20480 / input_uv[16 * j + i + 8] % 10 > 4) {
                tmp2++;
            }

            writel(tmp1 | tmp2 << 16, MJPEG_BASE + MJPEG_Q_PARAM_40_OFFSET + (i * 4 + j) * 4);
        }
    }
}

void drv_mjpeg_fill_jpeg_header_tail(uint8_t *header, uint32_t header_len)
{
    uint32_t tmpVal;

    arch_memcpy_fast((void *)(uintptr_t)(MJPEG_BASE + 0x800), header, header_len);

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_5_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_HEAD_BYTE, header_len);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_TAIL_EXP, 1);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_5_OFFSET);
}

//removed //void drv_mjpeg_set_yuv420sp_cam_input(uint8_t yy, uint8_t uv)
//removed //{
//removed //    uint32_t tmpVal;
//removed //
//removed //    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
//removed //    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_YY_DVP2AXI_SEL, yy);
//removed //    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_UV_DVP2AXI_SEL, uv);
//removed //    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_1_OFFSET);
//removed //}

void drv_mjpeg_update_input_output_buff(void *input_buf0, void *input_buf1, void *output_buff, size_t output_buff_size)
{
    uint32_t tmpVal = 0;

    if (input_buf0 != NULL) {
        writel((uint32_t)(uintptr_t)input_buf0, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_1_OFFSET);
    }

    if (input_buf1 != NULL) {
        writel((uint32_t)(uintptr_t)input_buf1, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_2_OFFSET);
    }

    if (output_buff != NULL) {
        tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_4_OFFSET);
        tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_JP_ADDR, (uint32_t)(uintptr_t)output_buff >> 2);
        writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_4_OFFSET);

        writel((uint32_t)output_buff_size >> 7, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_2_OFFSET);

    }
}

int drv_mjpeg_set_input_addr(int type, uint32_t addr)
{
    if (type == MJPEG_SET_ADDR_Y)
        writel(addr, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_1_OFFSET);
    else if (type == MJPEG_SET_ADDR_UV)
        writel(addr, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_2_OFFSET);
    else
        return -1;

    return 0;
}

int drv_mjpeg_set_out_size_addr(uint32_t size, uint32_t addr)
{
    uint32_t tmpVal = 0;
    /*TODO: check size/addr valid*/
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_JP_ADDR, addr >> 2);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_4_OFFSET);
    writel(size >> 7, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_2_OFFSET);

    g_udOutBufSize = (size >> 7) << 7;
    g_udOutBufAddr = (addr >> 2) << 2;
    return 0;
}

void drv_mjpeg_get_out_size_addr_info(uint32_t *size, uint32_t *addr)
{
    *size = g_udOutBufSize;
    *addr = g_udOutBufAddr;
}

void drv_mjpeg_set_hw_mode_swen(uint8_t value)
{
    uint32_t tmpVal = 0;
    value &= 1;
    
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ENC_HW_MODE_SWEN, value);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_0_OFFSET);
}
uint8_t drv_mjpeg_get_hw_mode_swen(void)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_0_OFFSET);
    return reg_get_bits(tmpVal, MJCOMB_CSR_REG_ENC_HW_MODE_SWEN);

}

void drv_mjpeg_get_swap_status(drv_mjpeg_swap_sts_info *swap_sts)
{
    swap_sts->jenc_csr_7.WORD = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_7_OFFSET);
    swap_sts->jenc_csr_8.WORD = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_8_OFFSET);
}
void drv_mjpeg_free_swap_memory(void)
{
    uint32_t tmpVal = 0;
        
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET);
    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_ENC_W_SWAP_CLR);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JENC_CSR_5_OFFSET);
}
