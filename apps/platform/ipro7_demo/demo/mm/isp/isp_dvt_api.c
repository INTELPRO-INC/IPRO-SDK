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

#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "compiler/common.h"

#include "isp_dvt_api.h"
#include "demo_isp_cfg.h"
#include "hal_isp.h"

#include "drv_mm_misc.h"
#include "drv_isp.h"
#include "drv_isp_wdr.h"
#include "drv_dvp2axi.h"
#include "drv_tsrc.h"
#include "drv_scaler_hydre.h"
#include "drv_osd_draw.h"
#include "drv_osd_blend.h"

#include "default_ae_hist.h"
#include "default_ae_stat.h"
#include "default_awb.h"
#include "default_color_trans.h"
#include "default_gamma.h"
#include "default_rgbtobayer.h"
#include "default_wdr.h"
#include "default_yuvctrl.h"
#include "default_osd_blend.h"

typedef struct
{
    uint16_t YBrightness;
    uint16_t YContrast;
    uint16_t CbSaturation;
    uint16_t CrSaturation;
    uint16_t SpecialMode;
    uint16_t SepiaCb;
    uint16_t SepiaCr;
    uint16_t SolarThred;
}cmodel_yuvctrl;

typedef struct
{
    uint16_t cs_en;
    uint16_t hist_roi_en;
    uint16_t hist_roi_weight;
    uint16_t hist_x_min;
    uint16_t hist_x_max;
    uint16_t hist_y_min;
    uint16_t hist_y_max;
    uint16_t curve_en;
    uint16_t curve_weight;
    uint16_t enh_en;
    uint16_t enh_weight;
    uint16_t enh_y_offset;
    uint16_t enh_y_gain;
    uint16_t enh_c_gain;
}cmodel_wdr;

typedef struct
{
    uint16_t ysum_coeff_r;
    uint16_t ysum_coeff_g;
    uint16_t ysum_coeff_b;
}cmodel_ae_sum_coeff;

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t layer_en;
    uint16_t osd_x_min;
    uint16_t osd_x_max;
    uint16_t osd_y_min;
    uint16_t osd_y_max;
    uint16_t mem_req_line;
    uint16_t mem_addr;
    uint32_t mem_dw_cnt;
    uint16_t color_format;
    uint16_t order_a;
    uint16_t order_rv;
    uint16_t order_gy;
    uint16_t order_bu;
    uint16_t alpha_0;
    uint16_t alpha_1;
    uint16_t global_a_en;
    uint16_t global_a;
    uint16_t global_color_en;
    uint16_t global_rv;
    uint16_t global_gy;
    uint16_t global_bu;
    uint16_t update_index;
    uint16_t update_color;
    uint16_t update_trigger;
    uint16_t key_palette_en;
    uint16_t key_index_inv;
    uint16_t key_index_min;
    uint16_t key_index_max;
    uint16_t key_replace_index;
    uint16_t key_color_en;
    uint16_t key_a_inv;
    uint16_t key_a_min;
    uint16_t key_a_max;
    uint16_t key_rv_inv;
    uint16_t key_rv_min;
    uint16_t key_rv_max;
    uint16_t key_gy_inv;
    uint16_t key_gy_min;
    uint16_t key_gy_max;
    uint16_t key_bu_inv;
    uint16_t key_bu_min;
    uint16_t key_bu_max;
    uint16_t key_replace_a;
    uint16_t key_replace_rv;
    uint16_t key_replace_gy;
    uint16_t key_replace_bu;
}cmodel_osd_blend;

static ATTR_PSRAM_SECTION cmodel_yuvctrl yuvctrl = CMODEL_YUVCTRL;

static ATTR_PSRAM_SECTION cmodel_wdr wdr = CMODEL_WDR;
static ATTR_PSRAM_SECTION ISP_WDR_Curve_Type wdr_curve = CMODE_WDR_CURVE_TABLE;
static ATTR_PSRAM_SECTION ISP_Chroma_Suppress_Type wdr_cs_type = CMODEL_WDR_CS_TYPE;

static BAYER_PATTERN_E rgb2bayer = CMODEL_RGBTOBAYER;

static uint8_t gamma_en = CMODEL_GAMMA_EN;
static ISP_Gamma_Curve_Type ATTR_PSRAM_SECTION gamma_r = CMODEL_GAMMA_R;
static ISP_Gamma_Curve_Type ATTR_PSRAM_SECTION gamma_g = CMODEL_GAMMA_G;
static ISP_Gamma_Curve_Type ATTR_PSRAM_SECTION gamma_b = CMODEL_GAMMA_B;

static ATTR_PSRAM_SECTION ISP_CE_OFFSET_S ce_pre_offset = CMODEL_COLOR_TRANS_PRE_OFFSET;
static ATTR_PSRAM_SECTION ISP_CE_OFFSET_S ce_post_offset = CMODEL_COLOR_TRANS_POST_OFFSET;
static ATTR_PSRAM_SECTION ISP_CE_COEF_S ce_coef = CMODEL_COLOR_TRANS_MATRIX;

static uint8_t awb_en = CMODEL_AWB_EN;
static ATTR_PSRAM_SECTION isp_awb3_stats_conf_t awb = CMODEL_AWB;

static uint8_t ae_stat_en = CMODEL_AE_STAT_EN;
static ATTR_PSRAM_SECTION cmodel_ae_sum_coeff ae_sum_coeff = CMODE_AE_SUM_COEFF;
static ATTR_PSRAM_SECTION isp_ae_stats_conf_t ae_stat_cfg = CMODEL_AE_STAT;

static AE_HIST_MODE_TYPE_E hist_mode = CMODEL_AE_HIST_MODE;
static uint8_t ae_hist_roi_en = CMODEL_AE_HIST_ROI_EN;
static isp_ae_hist_roi_t roi_cfg = CMODEL_AE_HIST;

#ifdef OSD_BLEND_EN
static cmodel_osd_blend osd_blend = CMODEL_OSD_BLEND;
#endif

static int even_division(int dividend, int divisor)
{
    int tmp = dividend/divisor;
    return (tmp - tmp % 2);
}

static void mm_seof1_isr(void)
{
    MM_MISC_SEOF_TYPE_E type_status;
    MM_MISC_Get_SEOF_TypeStatus(MM_MISC_SEOF1, &type_status);
    if (type_status.BF.SoF)
    {
         printf("seof 1 SoF %llu\r\n", xTaskGetTickCountFromISR());
         // to check buf_idx for stat
         // printf("awb3_status 0x30048bcc : 0x%lx\r\n", readl(0x30048bcc));
         // printf("ae_status   0x30048c5c : 0x%lx\r\n", readl(0x30048c5c));
         // printf("ae_sta_dbg  0x30048c60 : 0x%lx\r\n", readl(0x30048c60));
         // printf("WDR_0       0x3004e000 : 0x%lx\r\n", readl(0x3004e000));
    }
    if (type_status.BF.EoF)
    {
         printf("seof 1 EoF %llu\r\n", xTaskGetTickCountFromISR());
         // to check buf_idx for stat
         // printf("awb3_status 0x30048bcc : 0x%lx\r\n", readl(0x30048bcc));
         // printf("ae_status   0x30048c5c : 0x%lx\r\n", readl(0x30048c5c));
         // printf("ae_sta_dbg  0x30048c60 : 0x%lx\r\n", readl(0x30048c60));
         // printf("WDR_0       0x3004e000 : 0x%lx\r\n", readl(0x3004e000));
    }
    if (type_status.BF.lineCnt)
    {
         printf("seof 1 LineCnt %llu\r\n", xTaskGetTickCountFromISR());
    }
    MM_MISC_Clr_SEOF_TypeStatus(MM_MISC_SEOF1, type_status);
}

static void dvp2axi_b_isr(void)
{
    if (DVP2AXI_IntStatus(DVP2AXI_PORT_B, DVP2AXI_INT_NORMAL)) {
        DVP2AXI_IntClr(DVP2AXI_PORT_B, DVP2AXI_INT_NORMAL);
        printf("dvp2axi_b normal\r\n");
    }
    if (DVP2AXI_IntStatus(DVP2AXI_PORT_B, DVP2AXI_INT_FIFO_OVERWRITE)) {
        DVP2AXI_IntClr(DVP2AXI_PORT_B, DVP2AXI_INT_FIFO_OVERWRITE);
        printf("dvp2axi_b fifo overwrite\r\n");
    }
    if (DVP2AXI_IntStatus(DVP2AXI_PORT_B, DVP2AXI_INT_HSYNC_CNT_ERROR)) {
        DVP2AXI_IntClr(DVP2AXI_PORT_B, DVP2AXI_INT_HSYNC_CNT_ERROR);
        printf("dvp2axi_b h count error\r\n");
    }
    if (DVP2AXI_IntStatus(DVP2AXI_PORT_B, DVP2AXI_INT_VSYNC_CNT_ERROR)) {
        DVP2AXI_IntClr(DVP2AXI_PORT_B, DVP2AXI_INT_VSYNC_CNT_ERROR);
        printf("dvp2axi_b v count error\r\n");
    }
}

static void ae_stat_isr(void)
{
    printf("ae stat done\r\n");
    ISP_Clr_2A_Interrupt_Status(ISP_2A_INT_ID_AE_STAT);
}

static void ae_hist_isr(void)
{
    printf("ae hist done\r\n");
    ISP_Clr_2A_Interrupt_Status(ISP_2A_INT_ID_AE_HIST);
}

static void awb_stat_isr(void)
{
    printf("awb stat done\r\n");
    ISP_Clr_2A_Interrupt_Status(ISP_2A_INT_ID_AWB_STAT);
}

static void wdr_isr(void)
{
    printf("wdr done\r\n");
    ISP_WDR_Clr_Interrupt_Status();
}

static void tsrc_isr(void)
{
    printf("tsrc drain error\r\n");
    TSRC_Clear_Drain_Error();
}

static void scaler_isr(void)
{
    SCALER_HYDRE_CheckStatus();
}
#ifdef OSD_BLEND_EN
static void osd_isr(void)
{
    osd_blend_interrupt_status_t osd_bld_sts;

    osd_blend_get_interrupt(OSD_A_BLEND_LAYER0, &osd_bld_sts);
    if (osd_bld_sts.seof_int)
    {
        printf("osd bld SEoF %llu\r\n", xTaskGetTickCountFromISR());
        osd_blend_clear_interrupt(OSD_A_BLEND_LAYER0, OSD_BLEND_INT_TYPE_SEOF);
    }
    if (osd_bld_sts.drain_err)
    {
        printf("osd bld drain err\r\n");
        osd_blend_clear_interrupt(OSD_A_BLEND_LAYER0, OSD_BLEND_INT_TYPE_DRAIN);
    }
}
#endif
#ifdef OSD_DRAW_EN
static void osd_set_draw_default(void)
{
    // set draw default (enable blend)
    osd_draw_init(OSD_A_DRAW_LAYER_L);
    osd_rectangle_t rectangle = {0};
    uint16_t x_step, y_step;
    uint8_t thinkness = 0;

    x_step = (OUT_WIDTH_0 >> 3) & 0xfffe; // start x each step cut in 8 for 8-layer
    y_step = (OUT_HEIGHT_0 >> 3) & 0xfffe; // start y each step cut in 8 for 8-layer

    // enable all set
    thinkness = 16;
    rectangle.desc.attr.all = OSD_COLOR_SILVER | thinkness;
    rectangle.desc.pos.x.start = x_step * 0;
    rectangle.desc.pos.x.end = (((OUT_WIDTH_0 + rectangle.desc.pos.x.start) >> 1) & 0xfffffffe) - 1;
    rectangle.desc.pos.y.start = y_step * 0;
    rectangle.desc.pos.y.end = (((OUT_HEIGHT_0 + rectangle.desc.pos.y.start) >> 1) & 0xfffffffe) - 1;
    rectangle.layer = 0;
    rectangle.style = OSD_RECT_STYLE_HOLLOW;
    osd_draw_rectangle(OSD_A_DRAW_LAYER_L, &rectangle);

    thinkness = 8;
    rectangle.desc.attr.all = OSD_COLOR_GRAY | thinkness;
    rectangle.desc.pos.x.start = x_step * 1;
    rectangle.desc.pos.x.end = (((OUT_WIDTH_0 + rectangle.desc.pos.x.start) >> 1) & 0xfffffffe) - 1;
    rectangle.desc.pos.y.start = y_step * 1;
    rectangle.desc.pos.y.end = (((OUT_HEIGHT_0 + rectangle.desc.pos.y.start) >> 1) & 0xfffffffe) - 1;
    rectangle.layer = 1;
    rectangle.style = OSD_RECT_STYLE_SOLID;
    osd_draw_rectangle(OSD_A_DRAW_LAYER_L, &rectangle);

    thinkness = 8;
    rectangle.desc.attr.all = OSD_COLOR_DIMGRAY | thinkness;
    rectangle.desc.pos.x.start = x_step * 2;
    rectangle.desc.pos.x.end = (((OUT_WIDTH_0 + rectangle.desc.pos.x.start) >> 1) & 0xfffffffe) - 1;
    rectangle.desc.pos.y.start = y_step * 2;
    rectangle.desc.pos.y.end = (((OUT_HEIGHT_0 + rectangle.desc.pos.y.start) >> 1) & 0xfffffffe) - 1;
    rectangle.layer = 2;
    rectangle.style = OSD_RECT_STYLE_HOLLOW;
    osd_draw_rectangle(OSD_A_DRAW_LAYER_L, &rectangle);

    thinkness = 4;
    rectangle.desc.attr.all = OSD_COLOR_RED | thinkness;
    rectangle.desc.pos.x.start = x_step * 3;
    rectangle.desc.pos.x.end = (((OUT_WIDTH_0 + rectangle.desc.pos.x.start) >> 1) & 0xfffffffe) - 1;
    rectangle.desc.pos.y.start = y_step * 3;
    rectangle.desc.pos.y.end = (((OUT_HEIGHT_0 + rectangle.desc.pos.y.start) >> 1) & 0xfffffffe) - 1;
    rectangle.layer = 3;
    rectangle.style = OSD_RECT_STYLE_SOLID;
    osd_draw_rectangle(OSD_A_DRAW_LAYER_L, &rectangle);

    thinkness = 4;
    rectangle.desc.attr.all = OSD_COLOR_GREEN | thinkness;
    rectangle.desc.pos.x.start = x_step * 4;
    rectangle.desc.pos.x.end = (((OUT_WIDTH_0 + rectangle.desc.pos.x.start) >> 1) & 0xfffffffe) - 1;
    rectangle.desc.pos.y.start = y_step * 4;
    rectangle.desc.pos.y.end = (((OUT_HEIGHT_0 + rectangle.desc.pos.y.start) >> 1) & 0xfffffffe) - 1;
    rectangle.layer = 4;
    rectangle.style = OSD_RECT_STYLE_HOLLOW;
    osd_draw_rectangle(OSD_A_DRAW_LAYER_L, &rectangle);

    thinkness = 2;
    rectangle.desc.attr.all = OSD_COLOR_BLUE | thinkness;
    rectangle.desc.pos.x.start = x_step * 5;
    rectangle.desc.pos.x.end = (((OUT_WIDTH_0 + rectangle.desc.pos.x.start) >> 1) & 0xfffffffe) - 1;
    rectangle.desc.pos.y.start = y_step * 5;
    rectangle.desc.pos.y.end = (((OUT_HEIGHT_0 + rectangle.desc.pos.y.start) >> 1) & 0xfffffffe) - 1;
    rectangle.layer = 5;
    rectangle.style = OSD_RECT_STYLE_SOLID;
    osd_draw_rectangle(OSD_A_DRAW_LAYER_L, &rectangle);

    thinkness = 2;
    rectangle.desc.attr.all = OSD_COLOR_SKYBLUE | thinkness;
    rectangle.desc.pos.x.start = x_step * 6;
    rectangle.desc.pos.x.end = (((OUT_WIDTH_0 + rectangle.desc.pos.x.start) >> 1) & 0xfffffffe) - 1;
    rectangle.desc.pos.y.start = y_step * 6;
    rectangle.desc.pos.y.end = (((OUT_HEIGHT_0 + rectangle.desc.pos.y.start) >> 1) & 0xfffffffe) - 1;
    rectangle.layer = 6;
    rectangle.style = OSD_RECT_STYLE_HOLLOW;
    osd_draw_rectangle(OSD_A_DRAW_LAYER_L, &rectangle);

    thinkness = 0;
    rectangle.desc.attr.all = OSD_COLOR_INDIGO | thinkness;
    rectangle.desc.pos.x.start = x_step * 7;
    rectangle.desc.pos.x.end = (((OUT_WIDTH_0 + rectangle.desc.pos.x.start) >> 1) & 0xfffffffe) - 1;
    rectangle.desc.pos.y.start = y_step * 7;
    rectangle.desc.pos.y.end = (((OUT_HEIGHT_0 + rectangle.desc.pos.y.start) >> 1) & 0xfffffffe) - 1;
    rectangle.layer = 7;
    rectangle.style = OSD_RECT_STYLE_SOLID;
    osd_draw_rectangle(OSD_A_DRAW_LAYER_L, &rectangle);
}
#endif

#ifdef OSD_BLEND_EN
void osd_set_blend_default(uint32_t bld_img_addr)
{
    // blend image = 512x512, ARGB1555
    osd_blend_image_set_t blend_img_set = {0};
    osd_blend_color_keying_set_t blend_key_set = {0};
    uint32_t color;

    // set blend default
    osd_blend_ctrl_layer(OSD_A_BLEND_LAYER0, OSD_BLEND_ST_DISABLE);

#ifndef OSD_BLD_FIX_BIT_TEST
    blend_img_set.data = (uintptr_t*)bld_img_addr;
    blend_img_set.stride_byte_size = 0;
    blend_img_set.frame_width_byte = 512*2;
    blend_img_set.line_head_fix_bit = 0;
    blend_img_set.line_tail_fix_bit = 0;
    blend_img_set.frame_height = 512;
    osd_blend_config_layer(OSD_A_BLEND_LAYER0, 384, 104, 895, 615, &blend_img_set);
#else
    // front & tail ignore 16-bit test
    blend_img_set.data = (uintptr_t*)bld_img_addr;
    blend_img_set.stride_byte_size = 0;
    blend_img_set.frame_width_byte = 512*2;
    blend_img_set.line_head_fix_bit = 16;
    blend_img_set.line_tail_fix_bit = 16;
    blend_img_set.frame_height = 512;
    // due to blend image ignore front and end 16bit with ARGB1555
    // the blend size would be 510x512, the woi pos minus two pix
    osd_blend_config_layer(OSD_A_BLEND_LAYER0, 384, 104, 893, 615, &blend_img_set);
#endif
    // osd_blend.color_format = OSD_BLEND_COLOR_FMT_BPP8AYUV;
    osd_blend_set_color_format(OSD_A_BLEND_LAYER0, osd_blend.color_format, osd_blend.order_a, osd_blend.order_rv, osd_blend.order_gy, osd_blend.order_bu);
    osd_blend_set_global_alpha(OSD_A_BLEND_LAYER0, osd_blend.global_a_en, osd_blend.global_a);
    osd_blend_set_global_color(OSD_A_BLEND_LAYER0, osd_blend.global_color_en, osd_blend.global_rv, osd_blend.global_gy, osd_blend.global_bu);
    osd_blend_set_alpha_argb1555(OSD_A_BLEND_LAYER0, osd_blend.alpha_0, osd_blend.alpha_1);

    blend_key_set.mode_alpha = osd_blend.key_a_inv;
    blend_key_set.min_key_alpha = osd_blend.key_a_min;
    blend_key_set.max_key_alpha = osd_blend.key_a_max;
    blend_key_set.mode_rv = osd_blend.key_rv_inv;
    blend_key_set.min_key_rv = osd_blend.key_rv_min;
    blend_key_set.max_key_rv = osd_blend.key_rv_max;
    blend_key_set.mode_gy = osd_blend.key_gy_inv;
    blend_key_set.min_key_gy = osd_blend.key_gy_min;
    blend_key_set.max_key_gy = osd_blend.key_gy_max;
    blend_key_set.mode_bu = osd_blend.key_bu_inv;
    blend_key_set.min_key_bu = osd_blend.key_bu_min;
    blend_key_set.max_key_bu = osd_blend.key_bu_max;
    blend_key_set.replace_alpha = osd_blend.key_replace_a;
    blend_key_set.replace_rv = osd_blend.key_replace_rv;
    blend_key_set.replace_gy = osd_blend.key_replace_gy;
    blend_key_set.replace_bu = osd_blend.key_replace_bu;

    osd_blend_color_keying(OSD_A_BLEND_LAYER0, osd_blend.key_color_en, &blend_key_set);

    osd_blend_palette_keying(OSD_A_BLEND_LAYER0, osd_blend.key_palette_en, osd_blend.key_index_inv, osd_blend.key_replace_index, osd_blend.key_index_min, osd_blend.key_index_max);

    uint8_t a, rv, gy, bv;
    // force to set this, only effect color_format > OSD_BLEND_COLOR_FMT_A8YUV
    for (uint32_t i = 0; i < 256; i++)
    {
        a = i; rv=i+1; gy=i+2; bv=i+3;
        color = (a << 24)|(rv << 16)|(gy << 8)|(bv << 0);
        osd_blend_update_palette(OSD_A_BLEND_LAYER0, color, i);
    }

    osd_blend_mem_access_line(OSD_A_BLEND_LAYER0, 128); // cycle number after vsync falling

    osd_blend_ctrl_layer(OSD_A_BLEND_LAYER0, OSD_BLEND_ST_ENABLE);

    osd_blend_layer_early_commit(OSD_A_BLEND_LAYER0);
}
#endif

void isp_set_default_param(uint32_t bld_img_addr, uint16_t in_width, uint16_t in_height)
{
    uint8_t state = 0;

    // yuvctrl
    ISP_Set_Brightness(yuvctrl.YBrightness);
    ISP_Set_Contrast(yuvctrl.YContrast);
    ISP_SATURATION_S saturation = {yuvctrl.CbSaturation, yuvctrl.CrSaturation};
    ISP_Set_Saturation(&saturation);
    ISP_Set_Special_Mode(yuvctrl.SpecialMode);
    ISP_Set_Sepia(yuvctrl.SepiaCr, yuvctrl.SepiaCb);
    ISP_Set_Solar_Thred(yuvctrl.SolarThred);

    // wdr
    state = 1;

    ISP_WDR_Set_WDR_Curve(&wdr_curve);
    ISP_WDR_Set_WDR_Curve_Weight(wdr.curve_weight);
    ISP_WDR_Set_WDR_Curve_State(wdr.curve_en);
    ISP_WDR_Set_CS_Config(&wdr_cs_type);
    ISP_WDR_Set_CS_State(wdr.cs_en);
    ISP_WDR_Set_Post_Enhance_Weight(wdr.enh_weight);
    ISP_WDR_Set_Post_Enhance_Y_Offset(wdr.enh_y_offset);
    ISP_WDR_Set_Post_Enhance_Luma_Gain(wdr.enh_y_gain);
    ISP_WDR_Set_Post_Enhance_Chroma_Gain(wdr.enh_c_gain);
    ISP_WDR_Set_Post_Enhance_State(wdr.enh_en);

    // change to check from input size
    ISP_WDR_Set_ROI_Position(0, 0, in_width - 1, 0, in_height - 1);
    ISP_WDR_Set_ROI_Weight(0, wdr.hist_roi_weight);
    ISP_WDR_Set_ROI_State(0, wdr.hist_roi_en);

    ISP_WDR_Set_WDR_State(state);

    // rgb2bayer
    ISP_Set_RGBtoBayer(rgb2bayer);

    // gamma
    ISP_Set_Gamma_Coef(&gamma_r, ISP_GAMMA_CHANNEL_R);
    ISP_Set_Gamma_Coef(&gamma_g, ISP_GAMMA_CHANNEL_G);
    ISP_Set_Gamma_Coef(&gamma_b, ISP_GAMMA_CHANNEL_B);
    ISP_Set_Gamma_State(gamma_en);
    ISP_Set_Gamma_Update();

    // color_transform
    state = 1;

    ISP_Set_CE_Coef(&ce_coef);
    ISP_Set_CE_Pre_Offset(&ce_pre_offset);
    ISP_Set_CE_Post_Offset(&ce_post_offset);
    ISP_Set_CE_State(state);

    // awb
    awb.stat_x_min = 0;
    awb.stat_x_max = in_width - 1;
    awb.stat_y_min = 0;
    awb.stat_y_max = in_height - 1;
    ISP_Set_AWB3_Stats_Conf(&awb);
    ISP_Set_AWB3_Stats_State(awb_en);

    // ae_stat
    ae_stat_cfg.img_width   = in_width;
    ae_stat_cfg.img_height  = in_height;
    ae_stat_cfg.grid_width  = even_division(in_width, ae_stat_cfg.grid_num_x);
    ae_stat_cfg.grid_height = even_division(in_height, ae_stat_cfg.grid_num_y);
    ae_stat_cfg.roi_start_x = (ae_stat_cfg.img_width - ae_stat_cfg.grid_num_x*ae_stat_cfg.grid_width)/2 ;
    ae_stat_cfg.roi_start_y = (ae_stat_cfg.img_height - ae_stat_cfg.grid_num_y*ae_stat_cfg.grid_height)/2 ;

    ISP_Set_AE_Stats_Conf(&ae_stat_cfg);
    ISP_Set_AE_Ysum_Coeff(ae_sum_coeff.ysum_coeff_r, ae_sum_coeff.ysum_coeff_g, ae_sum_coeff.ysum_coeff_b);
    ISP_Set_AE_State(ae_stat_en);

    // ae_hist
    state = 1;
    ISP_Set_AE_Hist_Mode(hist_mode);
    roi_cfg.ae_hist_x_min = 0;
    roi_cfg.ae_hist_x_max = in_width - 1;
    roi_cfg.ae_hist_y_min = 0;
    roi_cfg.ae_hist_y_max = in_height - 1;
    ISP_Set_AE_Hist_ROI(ae_hist_roi_en, &roi_cfg);
    ISP_Set_AE_Hist_State(state);

    // disable LPF (check with SoC)
    ISP_Set_LPF_Filting_Mode(1);

#ifdef OSD_DRAW_EN
    osd_set_draw_default();
#endif
#ifdef OSD_BLEND_EN
    osd_set_blend_default(bld_img_addr);
#endif
}

void isp_module_interrupt(uint8_t from_sensor)
{
    CPU_Interrupt_Disable(OSD_BLEND_IRQn);
    CPU_Interrupt_Disable(ISP_MM_SEOF1_IRQn);
    CPU_Interrupt_Disable(D2XB_IRQn);
    CPU_Interrupt_Disable(ISP_WDR_IRQn);
    CPU_Interrupt_Disable(ISP_AE_IRQn);
    CPU_Interrupt_Disable(ISP_AE_HIST_IRQn);
    CPU_Interrupt_Disable(ISP_AWB3_IRQn);
    CPU_Interrupt_Disable(DTSRC_IRQn);
    CPU_Interrupt_Disable(SCALER_HYDRE_IRQn);
    #ifdef OSD_BLEND_EN
    CPU_Interrupt_Disable(OSD_BLEND_IRQn);
    #endif

    // mm_seof1
    MM_MISC_SEOF_TYPE_E seof_type = {0};
    seof_type.BF.SoF = 1;
    seof_type.BF.EoF = 1;
    seof_type.BF.lineCnt = 1;
    MM_MISC_Set_SEOF_LineCnt(MM_MISC_SEOF1, 400); // hard code to test
    if (from_sensor)
        MM_MISC_Set_SEOF_Src(MM_MISC_SEOF1, MM_MISC_SEOF_INTR_SRC_DVP_AS);
    else
        MM_MISC_Set_SEOF_Src(MM_MISC_SEOF1, MM_MISC_SEOF_INTR_SRC_DVP_TSRC);
    MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF1, seof_type);

    Interrupt_Handler_Register(ISP_MM_SEOF1_IRQn, mm_seof1_isr);
    CPU_Interrupt_Enable(ISP_MM_SEOF1_IRQn);

    // dvp2axi_b
    DVP2AXI_IntMask(DVP2AXI_PORT_B, DVP2AXI_INT_ALL, UNMASK);
    Interrupt_Handler_Register(D2XB_IRQn, dvp2axi_b_isr);
    CPU_Interrupt_Enable(D2XB_IRQn);

    // isp ae/awb/ae_hist
    if (from_sensor) //from sensor do not enable this, from tsrc to check this
    {
        ISP_Set_2A_Interrupt(ISP_2A_INT_ID_AE_STAT, 0);
        ISP_Set_2A_Interrupt(ISP_2A_INT_ID_AE_HIST, 0);
        ISP_Set_2A_Interrupt(ISP_2A_INT_ID_AWB_STAT, 0);
    }
    else
    {
        ISP_Set_2A_Interrupt(ISP_2A_INT_ID_AE_STAT, 1);
        ISP_Set_2A_Interrupt(ISP_2A_INT_ID_AE_HIST, 1);
        ISP_Set_2A_Interrupt(ISP_2A_INT_ID_AWB_STAT, 1);
        Interrupt_Handler_Register(ISP_AE_IRQn, ae_stat_isr);
        Interrupt_Handler_Register(ISP_AE_HIST_IRQn, ae_hist_isr);
        Interrupt_Handler_Register(ISP_AWB3_IRQn, awb_stat_isr);
        CPU_Interrupt_Enable(ISP_AE_IRQn);
        CPU_Interrupt_Enable(ISP_AE_HIST_IRQn);
        CPU_Interrupt_Enable(ISP_AWB3_IRQn);
    }

    // wdr
    if (from_sensor) //from sensor do not enable this, from tsrc to check this
    {
        ISP_WDR_Set_Interrupt(0);
    }
    else
    {
        ISP_WDR_Set_Interrupt(1);
        Interrupt_Handler_Register(ISP_WDR_IRQn, wdr_isr);
        CPU_Interrupt_Enable(ISP_WDR_IRQn);
    }

    // tsrc, no interrupt mask (needs to get drain error to check, but how?)
    Interrupt_Handler_Register(DTSRC_IRQn, tsrc_isr);
    CPU_Interrupt_Enable(DTSRC_IRQn);

    // scaler_hydre (needs to get error to check, but how?)
    SCALER_HYDRE_ErrIntEn(1);
    Interrupt_Handler_Register(SCALER_HYDRE_IRQn, scaler_isr);
    CPU_Interrupt_Enable(SCALER_HYDRE_IRQn);

    #ifdef OSD_BLEND_EN
    osd_blend_set_interrupt_edge(OSD_A_BLEND_LAYER0, OSD_BLEND_INT_EDGE_RISING);
    osd_blend_enable_interrupt(OSD_A_BLEND_LAYER0, OSD_BLEND_ST_ENABLE, OSD_BLEND_INT_TYPE_SEOF);
    osd_blend_enable_interrupt(OSD_A_BLEND_LAYER0, OSD_BLEND_ST_ENABLE, OSD_BLEND_INT_TYPE_DRAIN);

    Interrupt_Handler_Register(OSD_BLEND_IRQn, osd_isr);
    CPU_Interrupt_Enable(OSD_BLEND_IRQn);
    #endif
}

void sof_update_param(uint8_t from_sensor)
{
    if (from_sensor)
        return;
    // yuvctrl.YBrightness = 0;
    // yuvctrl.YContrast = 0;
    yuvctrl.CbSaturation = 150;
    yuvctrl.CrSaturation = 150;
    // yuvctrl.SpecialMode = 0;
    // yuvctrl.SepiaCb = 0;
    // yuvctrl.SepiaCr = 0;
    // yuvctrl.SolarThred = 0;
    // ISP_Set_Brightness(yuvctrl.YBrightness);
    // ISP_Set_Contrast(yuvctrl.YContrast);
    ISP_SATURATION_S saturation = {yuvctrl.CbSaturation, yuvctrl.CrSaturation};
    ISP_Set_Saturation(&saturation);
    // ISP_Set_Special_Mode(yuvctrl.SpecialMode);
    // ISP_Set_Sepia(yuvctrl.SepiaCr, yuvctrl.SepiaCb);
    // ISP_Set_Solar_Thred(yuvctrl.SolarThred);
    // wdr
    // state = 1;

    // reference from ip85 some case
    wdr_cs_type.gain = 263;
    wdr_cs_type.weight = 7;
    wdr_cs_type.grey_thr = 3;

    wdr.enh_weight = 0;
    wdr.enh_y_offset = 884;
    wdr.enh_y_gain = 7547;
    wdr.enh_c_gain = 4505;

    // ISP_WDR_Set_WDR_Curve(&wdr_curve);
    // ISP_WDR_Set_WDR_Curve_Weight(wdr.curve_weight);
    // ISP_WDR_Set_WDR_Curve_State(wdr.curve_en);
    ISP_WDR_Set_CS_Config(&wdr_cs_type);
    // ISP_WDR_Set_CS_State(wdr.cs_en);
    ISP_WDR_Set_Post_Enhance_Weight(wdr.enh_weight);
    ISP_WDR_Set_Post_Enhance_Y_Offset(wdr.enh_y_offset);
    ISP_WDR_Set_Post_Enhance_Luma_Gain(wdr.enh_y_gain);
    ISP_WDR_Set_Post_Enhance_Chroma_Gain(wdr.enh_c_gain);
    // ISP_WDR_Set_Post_Enhance_State(wdr.enh_en);

    // change to check from input size
    // ISP_WDR_Set_ROI_Position(0, wdr.hist_x_min, wdr.hist_x_max, wdr.hist_y_min, wdr.hist_y_max);
    // ISP_WDR_Set_ROI_Position(0, 0, IN_WIDTH - 1, 0, IN_HEIGHT - 1);
    // ISP_WDR_Set_ROI_Weight(0, wdr.hist_roi_weight);
    // ISP_WDR_Set_ROI_State(0, wdr.hist_roi_en);

    // ISP_WDR_Set_WDR_State(state);

    // rgb2bayer
    // ISP_Set_RGBtoBayer(rgb2bayer);

    // gamma
    uint16_t tmp_r = 0, tmp_g = 0, tmp_b = 0;
    for(uint16_t i = 0; i < 257; i++)
    {
        tmp_r = i * 16;
        if (tmp_r > 4095) tmp_r = 4095;
        tmp_g = i * 16 + 1;
        if (tmp_g > 4095) tmp_g = 4095;
        tmp_b = i * 16 + 2;
        if (tmp_b > 4095) tmp_b = 4095;
        gamma_r.curve_coef[i] = tmp_r;
        gamma_g.curve_coef[i] = tmp_g;
        gamma_b.curve_coef[i] = tmp_b;
    }
    ISP_Set_Gamma_Coef(&gamma_r, ISP_GAMMA_CHANNEL_R);
    ISP_Set_Gamma_Coef(&gamma_g, ISP_GAMMA_CHANNEL_G);
    ISP_Set_Gamma_Coef(&gamma_b, ISP_GAMMA_CHANNEL_B);
    // ISP_Set_Gamma_State(gamma_en);
    ISP_Set_Gamma_Update();

    // color_transform
    // state = 1;

    // ISP_Set_CE_Coef(&ce_coef);
    // ISP_Set_CE_Pre_Offset(&ce_pre_offset);
    // ISP_Set_CE_Post_Offset(&ce_post_offset);
    // ISP_Set_CE_State(&state);

    // awb

    // awb.stat_x_min = 0;
    // awb.stat_x_max = IN_WIDTH - 1;
    // awb.stat_y_min = 0;
    // awb.stat_y_max = IN_HEIGHT - 1;
    // ISP_Set_AWB3_Stats_Conf(&awb);
    // ISP_Set_AWB3_Hist_State(awb_en);

    // // ae_stat
    // ae_stat_cfg.img_width   = IN_WIDTH;
    // ae_stat_cfg.img_height  = IN_HEIGHT;
    // ae_stat_cfg.grid_width  = even_division(IN_WIDTH, ae_stat_cfg.grid_num_x);
    // ae_stat_cfg.grid_height = even_division(IN_HEIGHT, ae_stat_cfg.grid_num_y);
    // ae_stat_cfg.roi_start_x = (ae_stat_cfg.img_width - ae_stat_cfg.grid_num_x*ae_stat_cfg.grid_width)/2 ;
    // ae_stat_cfg.roi_start_y = (ae_stat_cfg.img_height - ae_stat_cfg.grid_num_y*ae_stat_cfg.grid_height)/2 ;

    // ISP_Set_AE_Stats_Conf(&ae_stat_cfg);
    // ISP_Set_AE_Ysum_Coeff(ae_sum_coeff.ysum_coeff_r, ae_sum_coeff.ysum_coeff_g, ae_sum_coeff.ysum_coeff_b);
    // ISP_Set_AE_State(ae_stat_en);

    // ae_hist
    // state = 1;
    hist_mode = AE_HIST_MODE_Y;
    ISP_Set_AE_Hist_Mode(hist_mode);
    // roi_cfg.ae_hist_x_min = 0;
    // roi_cfg.ae_hist_x_max = IN_WIDTH - 1;
    // roi_cfg.ae_hist_y_min = 0;
    // roi_cfg.ae_hist_y_max = IN_HEIGHT - 1;
    // ISP_Set_AE_Hist_ROI(ae_hist_roi_en, &roi_cfg);
    // ISP_Set_AE_Hist_State(state);
}
