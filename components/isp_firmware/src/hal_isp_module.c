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

#include <FreeRTOS.h>
#include <task.h>
#include <string.h>
#include "compiler/common.h"

#include "hal_isp_module.h"
#include "img_utils.h"
#include "img_param.h"
/*#include "ae_algo.h"
#include "awb_algo.h"
#include "tm_algo.h"
#include "defog_algo.h"*/

const static ISP_CE_COEF_S ce_coef_bt601_yuv2rgb  =  {{ 512,    0,    718,   512, -176,   -366,  512,  907,      0,  },};
const static ISP_CE_OFFSET_S ce_pre_offset_bt601_yuv2rgb = {.offset = {0, -2048, -2048}};
const static ISP_CE_OFFSET_S ce_post_offset_bt601_yuv2rgb = {.offset = {0, 0, 0}};

/*******************************************************************/
int isp_set_AE_state(Fun_Type state)
{
    ISP_Set_AE_State(state);
    return 0;
}

int isp_set_AE_state_enable(void)
{
    ISP_Set_AE_State(ENABLE);
    ISP_Set_AE_Hist_State(ENABLE);
    return 0;
}

int isp_set_AE_stats_conf (isp_ae_stats_cfg_t *conf)
{
    isp_ae_stats_conf_t ae_stats_conf;

    ae_stats_conf.grid_num_x  = conf->grid_num_x;
    ae_stats_conf.grid_num_y  = conf->grid_num_y;
    ae_stats_conf.img_width   = conf->img_width;
    ae_stats_conf.img_height  = conf->img_height;
    ae_stats_conf.grid_width  = even_division(conf->img_width, conf->grid_num_x);
    ae_stats_conf.grid_height = even_division(conf->img_height, conf->grid_num_y);
    ae_stats_conf.roi_start_x = (ae_stats_conf.img_width \
        - conf->grid_num_x*ae_stats_conf.grid_width)/2 ;
    ae_stats_conf.roi_start_y = (ae_stats_conf.img_height \
        - conf->grid_num_y*ae_stats_conf.grid_height)/2 ;
    ae_stats_conf.roi_width   = conf->grid_num_x*ae_stats_conf.grid_width;
    ae_stats_conf.roi_height  = conf->grid_num_y*ae_stats_conf.grid_height;

    ISP_Set_AE_Stats_Conf(&ae_stats_conf);
    return 0;
}

int isp_get_AE_stats_conf (isp_ae_stats_cfg_t *conf)
{
    isp_ae_stats_conf_t ae_stats_conf;
    ISP_Get_AE_Stats_Conf(&ae_stats_conf);

    conf->grid_num_x = ae_stats_conf.grid_num_x;
    conf->grid_num_y = ae_stats_conf.grid_num_y;
    conf->img_width = ae_stats_conf.img_width;
    conf->img_height = ae_stats_conf.img_height;

    return 0;
}

int isp_get_AE_stats(isp_ae_grid_sum_t *stats)
{
    int i = 0;
    isp_ae_sum_t sum[AE_STATS_GRID_NUM];
    ISP_Get_AE_Stats(AE_STATS_GRID_NUM, (uint32_t *)&sum);

    for (i = 0; i < AE_STATS_GRID_NUM; i++) {
        stats->y_sum[i] = sum[i].y_sum;
        stats->r_sum[i] = sum[i].r_sum;
        stats->g_sum[i] = sum[i].g_sum;
        stats->b_sum[i] = sum[i].b_sum;
    }

    return 0;
}

int isp_get_AE_hist_state(Fun_Type *state)
{
    return ISP_Get_AE_Hist_State(state);
}

int isp_set_AE_hist_state(Fun_Type state)
{
    ISP_Set_AE_Hist_State(state);
    return 0;
}

int isp_get_AE_hist_mode(AE_HIST_MODE_TYPE_E *mode)
{
    return ISP_Get_AE_Hist_Mode(mode);
}

int isp_set_AE_hist_mode(AE_HIST_MODE_TYPE_E mode)
{
    ISP_Set_AE_Hist_Mode(mode);
    return 0;
}

int isp_get_AE_hist_roi(uint16_t *x_min, uint16_t *x_max, uint16_t *y_min, uint16_t *y_max)
{
    isp_ae_hist_roi_t roi;

    ISP_Get_AE_Hist_ROI(true, &roi);
    *x_min = roi.ae_hist_x_min;
    *x_max = roi.ae_hist_x_max;
    *y_min = roi.ae_hist_y_min;
    *y_max = roi.ae_hist_y_max;

    return 0;
}

int isp_set_AE_hist_roi(uint16_t x_min, uint16_t  x_max, uint16_t y_min, uint16_t y_max)
{
    isp_ae_hist_roi_t roi;

    roi.ae_hist_x_min = x_min;
    roi.ae_hist_x_max = x_max;
    roi.ae_hist_y_min = y_min;
    roi.ae_hist_y_max = y_max;

    return ISP_Set_AE_Hist_ROI(true, &roi);
}

int isp_get_AE_hist_sum_y(isp_ae_hist_y_t *y_sum)
{
    int ret = 0;
    AE_HIST_MODE_TYPE_E mode;

    ISP_Get_AE_Hist_Mode(&mode);
    if(mode == AE_HIST_MODE_BAYER) return -1;
    ret = ISP_Get_AE_Hist_SUM_Y((uint32_t *)y_sum);

    return ret;
}

int isp_get_AE_hist_sum_bayer(isp_ae_hist_bayer_t *sum)
{
    int ret = 0;
    AE_HIST_MODE_TYPE_E mode;

    ISP_Get_AE_Hist_Mode(&mode);
    if(mode == AE_HIST_MODE_Y) return -1;
    ret = ISP_Get_AE_Hist_SUM_Bayer(sum->r, sum->gr, sum->gb, sum->b);

    return ret;
}

int isp_set_AWB_state(Fun_Type state)
{
    ISP_Set_AWB3_Stats_State(state);
    return 0;
}

//****** algo 3*****************************************************************//
int isp_set_AWB3_stats_conf(isp_awb3_stats_conf_t *awb_stats_conf)
{
    ISP_Set_AWB3_Stats_Conf(awb_stats_conf);
    return 0;
}

int isp_get_AWB3_stats_conf(isp_awb3_stats_conf_t *awb_stats_conf)
{
    ISP_Get_AWB3_Stats_Conf(awb_stats_conf);
    return 0;
}

int isp_get_AWB3_gDivisor(int *value)
{
    return ISP_Get_AWB3_G_Divisor(value);
}

int isp_set_AWB3_gDivisor(int value)
{
    ISP_Set_AWB3_G_Divisor(value);
    return 0;
}

int isp_get_AWB3_sum(isp_awb3_sum_t *awb3_sum)
{
    return ISP_Get_AWB3_Sum(awb3_sum);
}

int ipro_get_WDR_state(Fun_Type *state)
{
    ISP_WDR_Get_WDR_State(state);
    return 0;
}

int ipro_set_WDR_state(Fun_Type state)
{
    ISP_WDR_Set_WDR_State(state);
    return 0;
}

int ipro_get_WDR_hist_roi_info(isp_wdr_roi_info_t *info)
{
    ISP_WDR_Get_ROI_State(info->id, (Fun_Type *)&info->state);
    ISP_WDR_Get_ROI_Weight(info->id, &info->weight);
    ISP_WDR_Get_ROI_Position(info->id, &info->x_start, &info->x_end, &info->y_start, &info->y_end);

    return 0;
}

int ipro_set_WDR_hist_roi_info(isp_wdr_roi_info_t *info)
{
    ISP_WDR_Set_ROI_State(info->id, info->state);
    ISP_WDR_Set_ROI_Weight(info->id, info->weight);
    ISP_WDR_Set_ROI_Position(info->id, info->x_start, info->x_end, info->y_start, info->y_end);
    return 0;
}

int ipro_get_WDR_hist_data(isp_wdr_hist_data_t *data)
{
    ISP_WDR_Hist_Data_Type hist;
    ISP_WDR_Get_Hist_Data(&hist);
    memcpy(data, &hist, sizeof(isp_wdr_hist_data_t));
    return 0;

}

int isp_get_TM_state(Fun_Type *state)
{
   // tm_get_algo_state(state);
    return 0;
}

int isp_set_TM_state(Fun_Type state)
{
   // tm_set_algo_state(state);
    ISP_WDR_Set_WDR_Curve_State(state);
    return 0;
}

int isp_get_TM_curve_state(Fun_Type *state)
{
    ISP_WDR_Get_WDR_Curve_State(state);
    return 0;
}

int isp_Set_TM_curve_state(Fun_Type state)
{
    ISP_WDR_Set_WDR_Curve_State(state);
    return 0;
}

int isp_get_TM_curve(isp_tm_curve_t *data)
{
    ISP_WDR_Curve_Type wdr_curve;
    ISP_WDR_Get_WDR_Curve(&wdr_curve);
    memcpy(data, &wdr_curve,  sizeof(uint16_t) * ISP_WDR_CURVE_COEF_NUM);
    return 0;
}

int isp_set_TM_curve(isp_tm_curve_t *data)
{
    ISP_WDR_Curve_Type wdr_curve;
    memcpy(&wdr_curve, data, sizeof(uint16_t) * ISP_WDR_CURVE_COEF_NUM);
    ISP_WDR_Set_WDR_Curve(&wdr_curve);
    return 0;
}

int isp_get_TM_curve_weight(int *weight)
{
    ISP_WDR_Get_WDR_Curve_Weight(weight);
    return 0;
}

int isp_set_TM_curve_weight(int weight)
{
    ISP_WDR_Set_WDR_Curve_Weight(weight);
    return 0;
}

int isp_get_DEFOG_state(Fun_Type *state)
{
    ISP_WDR_Get_Post_Enhance_State(state);
    return 0;
}

int isp_set_DEFOG_state(Fun_Type state)
{
    ISP_WDR_Set_Post_Enhance_State(state);
    return 0;
}

int isp_get_DEFOG_enh(isp_defog_t *conf)
{
    ISP_WDR_Get_Post_Enhance_Weight(&conf->weight);
    ISP_WDR_Get_Post_Enhance_Y_Offset(&conf->y_offset);
    ISP_WDR_Get_Post_Enhance_Luma_Gain(&conf->y_gain);
    ISP_WDR_Get_Post_Enhance_Chroma_Gain(&conf->c_gain);

    return 0;
}

int isp_set_DEFOG_enh(isp_defog_t *conf)
{
    ISP_WDR_Set_Post_Enhance_Weight(conf->weight);
    ISP_WDR_Set_Post_Enhance_Y_Offset(conf->y_offset);
    ISP_WDR_Set_Post_Enhance_Luma_Gain(conf->y_gain);
    ISP_WDR_Set_Post_Enhance_Chroma_Gain(conf->c_gain);

    return 0;
}

int isp_get_CS_state(Fun_Type *state)
{
    ISP_WDR_Get_CS_State(state);
    return 0;
}

int isp_set_CS_state(Fun_Type state)
{
    ISP_WDR_Set_CS_State(state);
    return 0;
}

int isp_get_CS_conf(isp_cs_conf_t *conf)
{
    ISP_WDR_Get_CS_Config((ISP_Chroma_Suppress_Type*)conf);
    return 0;
}

int isp_set_CS_conf(isp_cs_conf_t *conf)
{
    ISP_WDR_Set_CS_Config((ISP_Chroma_Suppress_Type*)conf);
    return 0;
}

int isp_get_GAMMA_state(Fun_Type *state)
{
    ISP_Get_Gamma_State(state);
    return 0;
}

int isp_set_GAMMA_state(Fun_Type state)
{
    ISP_Set_Gamma_State(state);
    return 0;
}

int isp_get_GAMMA_curve(isp_gamma_curve_t *curve)
{
    ISP_Get_Gamma_Coef((ISP_Gamma_Curve_Type*)curve, ISP_GAMMA_CHANNEL_G);
    return 0;
}

int isp_set_GAMMA_curve(isp_gamma_curve_t *curve)
{
    //curve->coef[256] = 4095; // hardcode to test gamma (add entry from 256 -> 257, but iq code still use 256)
    ISP_Set_Gamma_Coef((ISP_Gamma_Curve_Type*)curve, ISP_GAMMA_CHANNEL_R);
    ISP_Set_Gamma_Coef((ISP_Gamma_Curve_Type*)curve, ISP_GAMMA_CHANNEL_G);
    ISP_Set_Gamma_Coef((ISP_Gamma_Curve_Type*)curve, ISP_GAMMA_CHANNEL_B);
    return 0;
}

int isp_disable_ct_soft(void)
{
    ISP_Set_CE_Coef(&ce_coef_bt601_yuv2rgb);
    ISP_Set_CE_Pre_Offset(&ce_pre_offset_bt601_yuv2rgb);
    ISP_Set_CE_Post_Offset(&ce_post_offset_bt601_yuv2rgb);
    return 0;
}

int isp_get_CT_conf(isp_ct_coef_s *coef)
{
    ISP_Get_CE_Coef((ISP_CE_COEF_S*)coef);
    return 0;
}

int isp_set_CT_conf(isp_ct_coef_s *coef)
{
    ISP_Set_CE_Coef((ISP_CE_COEF_S*)coef);
    return 0;
}

int isp_get_CT_pre_offset(isp_ct_offset_s *offset)
{
    ISP_Get_CE_Pre_Offset((ISP_CE_OFFSET_S*)offset);
    return 0;
}

int isp_set_CT_pre_offset(isp_ct_offset_s *offset)
{
    ISP_Set_CE_Pre_Offset((ISP_CE_OFFSET_S*)offset);
    return 0;
}

int isp_get_CT_post_offset(isp_ct_offset_s *offset)
{
    ISP_Get_CE_Post_Offset((ISP_CE_OFFSET_S*)offset);
    return 0;
}

int isp_set_CT_post_offset(isp_ct_offset_s *offset)
{
    ISP_Set_CE_Post_Offset((ISP_CE_OFFSET_S*)offset);
    return 0;
}


int isp_disable_yuvctrl_soft()
{
    isp_yuvctrl_conf_t conf;

    conf.brightness = 50;
    conf.contrast = 0;
    conf.saturation = 128;
    isp_set_YUVCTRL_conf(&conf);

    return 0;
}

int isp_get_YUVCTRL_conf(isp_yuvctrl_conf_t *conf)
{
    int brightness = 0;
    int contrast = 0;
    ISP_SATURATION_S saturation = {0};

    ISP_Get_Brightness(&brightness);
    ISP_Get_Contrast(&contrast);
    ISP_Get_Saturation(&saturation);

    conf->brightness = brightness + 50;
    conf->contrast = contrast;
    conf->saturation = saturation.saturation_cb;

    return 0;
}

int isp_set_YUVCTRL_conf(isp_yuvctrl_conf_t *conf)
{
    int brightness = conf->brightness - 50;

    const ISP_SATURATION_S saturation = {
        .saturation_cb = conf->saturation,
        .saturation_cr = conf->saturation,
    };

    ISP_Set_Brightness(brightness);
    ISP_Set_Contrast(conf->contrast);
    ISP_Set_Saturation(&saturation);
    return 0;
}

int isp_set_YUVADJ_state(Fun_Type state)
{
    ISP_Set_YUV_Adjust_State(state);

    return 0;
}

int isp_set_YUVADJ_cfg(isp_yuv_adj_cfg_t *cfg)
{

    ISP_Set_YUV_Adjust_Cfg((ISP_YUV_Adjust_Cfg_Type*)cfg);

    return 0;
}

int isp_get_YUVADJ_cfg(isp_yuv_adj_cfg_t *cfg)
{
    ISP_Get_YUV_Adjust_Cfg((ISP_YUV_Adjust_Cfg_Type*)cfg);
    
    return 0;
}
#if 0
int isp_get_addr_data(uint32_t addr,uint32_t *data)
{
    uint32_t tmpVal;
    tmpVal = DRV_RD_WORD(addr);
    *data = tmpVal;

    return 0;
}

int isp_set_addr_data(uint32_t addr,uint32_t data)
{
    DRV_WR_WORD(addr, data);

    return 0;
}
#endif


static ATTR_PSRAM_SECTION ISP_WDR_Curve_Type wdr_curve =  {
        {
            0, 
            16, 
            32, 
            48, 
            64, 
            80, 
            96, 
            112, 
            129, 
            147, 
            166, 
            185, 
            202, 
            218, 
            231, 
            243, 
            256, 
            271, 
            286, 
            303, 
            320, 
            338, 
            355, 
            373, 
            390, 
            407, 
            423, 
            441, 
            459, 
            478, 
            497, 
            513, 
            528, 
            543, 
            558, 
            576, 
            594, 
            612, 
            630, 
            647, 
            662, 
            675, 
            687, 
            698, 
            711, 
            728, 
            747, 
            765, 
            782, 
            799, 
            815, 
            830, 
            845, 
            860, 
            875, 
            890, 
            905, 
            918, 
            930, 
            943, 
            954, 
            966, 
            981, 
            1002, 
        },
};
static ATTR_PSRAM_SECTION isp_awb3_stats_conf_t awb_stats_conf  =  {
                /*stat_x_min*/              0, 
                /*stat_x_max*/              1279, 
                /*stat_y_min*/              0, 
                /*stat_y_max*/              719, 
                /*r_ratio_ofst*/              230, 
                /*b_ratio_ofst*/              300, 
                /*r_ratio_mult*/              175, 
                /*b_ratio_mult*/              115, 
                /*ratio_sum_min*/              192, 
                /*ratio_sum_max*/              288, 
                /*g_divisor*/              0, 
                /*weight_ratio_en*/              1, 
                /*weight_ratio_gain*/              16, 
                { 
{  0,  0,  0,  0,  0,  8,  16,  16,  8, }, 
{  0,  0,  0,  0,  8,  48,  96,  48,  16, }, 
{  0,  0,  0,  8,  96,  192,  64,  16,  0, }, 
{  0,  0,  0,  96,  255,  48,  8,  0,  0, }, 
{  0,  0,  48,  160,  160,  0,  0,  0,  0, }, 
{  0,  16,  96,  64,  64,  0,  0,  0,  0, }, 
{  8,  48,  48,  0,  0,  0,  0,  0,  0, }, 
{  16,  48,  16,  0,  0,  0,  0,  0,  0, }, 
{  8,  16,  0,  0,  0,  0,  0,  0,  0, }, 
                }, 
                /*y_r_coeff*/              77, 
                /*y_g_coeff*/              150, 
                /*y_b_coeff*/              29, 
                /*weight_y_use_g*/              1, 
                /*weight_y_en*/              1, 
                /*weight_y_gain*/              16, 
                { 
{  0,  32,  64,  96,  128,  160,  192,  224,  255, }, 
{  0,  32,  64,  96,  128,  160,  192,  224,  255, }, 
{  0,  32,  64,  96,  128,  160,  192,  224,  255, }, 
{  0,  32,  64,  96,  128,  160,  192,  224,  255, }, 
{  0,  32,  64,  96,  128,  160,  192,  224,  255, }, 
{  0,  32,  64,  96,  128,  160,  192,  224,  255, }, 
{  0,  32,  64,  96,  128,  160,  192,  224,  255, }, 
{  0,  32,  64,  96,  128,  160,  192,  224,  255, }, 
{  0,  32,  64,  96,  128,  160,  192,  224,  255, }, 
                }, 
                /*count_zero_weight*/ 
                0, 
                /*ink_type*/ 
                0, 
                /*ink_gain*/ 
                1, 
};

void isp_set_default_iq_param(uint16_t in_width, uint16_t in_height)
{
    // yuvctrl
    ISP_Set_Brightness(0);
    ISP_Set_Contrast(0);
    ISP_SATURATION_S saturation = {128, 128};
    ISP_Set_Saturation(&saturation);
    ISP_Set_Special_Mode(0);
    ISP_Set_Sepia(137, 115);
    ISP_Set_Solar_Thred(128);

    // wdr
    ISP_WDR_Set_WDR_Curve(&wdr_curve);
    ISP_WDR_Set_WDR_Curve_Weight(16);
    ISP_WDR_Set_WDR_Curve_State(1);
    ISP_Chroma_Suppress_Type wdr_cs_type = { 4195, 8, 48,};
    ISP_WDR_Set_CS_Config(&wdr_cs_type);
    ISP_WDR_Set_CS_State(1);
    ISP_WDR_Set_Post_Enhance_Weight(16);
    ISP_WDR_Set_Post_Enhance_Y_Offset(260);
    ISP_WDR_Set_Post_Enhance_Luma_Gain(5030);
    ISP_WDR_Set_Post_Enhance_Chroma_Gain(5815);
    ISP_WDR_Set_Post_Enhance_State(1);

    // change to check from input size
    ISP_WDR_Set_ROI_Position(0, 0, in_width - 1, 0, in_height - 1);
    ISP_WDR_Set_ROI_Weight(0, 1);
    ISP_WDR_Set_ROI_State(0, 1);
    ISP_WDR_Set_WDR_State(1);

    // rgb2bayer
    ISP_Set_RGBtoBayer(BAYER_PATTERN_RG);

    // gamma
    //ISP_Set_Gamma_Coef(&gamma_r, ISP_GAMMA_CHANNEL_R);
    //ISP_Set_Gamma_Coef(&gamma_g, ISP_GAMMA_CHANNEL_G);
    //ISP_Set_Gamma_Coef(&gamma_b, ISP_GAMMA_CHANNEL_B);
    ISP_Set_Gamma_State(0);
    ISP_Set_Gamma_Update();

    // color_transform
    ISP_Set_CE_Coef(&ce_coef_bt601_yuv2rgb);
    ISP_Set_CE_Pre_Offset(&ce_pre_offset_bt601_yuv2rgb);
    ISP_Set_CE_Post_Offset(&ce_post_offset_bt601_yuv2rgb);
    ISP_Set_CE_State(1);

    // awb
    ISP_Set_AWB3_Stats_Conf(&awb_stats_conf);
    ISP_Set_AWB3_Stats_State(1);

    // ae_stat
    isp_ae_stats_cfg_t conf = {in_width, in_height, AE_STATS_GRID_NUM_X, AE_STATS_GRID_NUM_Y};
    isp_set_AE_stats_conf (&conf);
    ISP_Set_AE_Ysum_Coeff(77, 150, 29);
    ISP_Set_AE_State(1);

    // ae_hist
    ISP_Set_AE_Hist_Mode(AE_HIST_MODE_Y);
    isp_ae_hist_roi_t roi_cfg = {0, in_width - 1, 0, in_height - 1};
    ISP_Set_AE_Hist_ROI(1, &roi_cfg);
    ISP_Set_AE_Hist_State(1);

    // disable LPF (check with SoC)
    ISP_Set_LPF_Filting_Mode(1);
}

void isp_set_cal_iq_param(uint16_t in_width, uint16_t in_height)
{
    // yuvctrl
    ISP_Set_Brightness(0);
    ISP_Set_Contrast(0);
    ISP_SATURATION_S saturation = {128, 128};
    ISP_Set_Saturation(&saturation);
    ISP_Set_Special_Mode(0);
    //ISP_Set_Sepia(137, 115);
    //ISP_Set_Solar_Thred(128);

    // wdr
    //ISP_WDR_Set_WDR_Curve(&wdr_curve);
    //ISP_WDR_Set_WDR_Curve_Weight(16);
    ISP_WDR_Set_WDR_Curve_State(0);
    //ISP_Chroma_Suppress_Type wdr_cs_type = { 4195, 8, 48,};
    //ISP_WDR_Set_CS_Config(&wdr_cs_type);
    ISP_WDR_Set_CS_State(0);
    //ISP_WDR_Set_Post_Enhance_Weight(16);
    //ISP_WDR_Set_Post_Enhance_Y_Offset(260);
    //ISP_WDR_Set_Post_Enhance_Luma_Gain(5030);
    //ISP_WDR_Set_Post_Enhance_Chroma_Gain(5815);
    ISP_WDR_Set_Post_Enhance_State(0);

    // change to check from input size
    //ISP_WDR_Set_ROI_Position(0, 0, in_width - 1, 0, in_height - 1);
    //ISP_WDR_Set_ROI_Weight(0, 1);
    //ISP_WDR_Set_ROI_State(0, 1);
    ISP_WDR_Set_WDR_State(0);

    // rgb2bayer
    ISP_Set_RGBtoBayer(BAYER_PATTERN_RG);

    // gamma
    //ISP_Set_Gamma_Coef(&gamma_r, ISP_GAMMA_CHANNEL_R);
    //ISP_Set_Gamma_Coef(&gamma_g, ISP_GAMMA_CHANNEL_G);
    //ISP_Set_Gamma_Coef(&gamma_b, ISP_GAMMA_CHANNEL_B);
    ISP_Set_Gamma_State(0);
    ISP_Set_Gamma_Update();

    // color_transform for 2A
    ISP_Set_CE_Coef(&ce_coef_bt601_yuv2rgb);
    ISP_Set_CE_Pre_Offset(&ce_pre_offset_bt601_yuv2rgb);
    ISP_Set_CE_Post_Offset(&ce_post_offset_bt601_yuv2rgb);
    ISP_Set_CE_State(1);

    // awb
    ISP_Set_AWB3_Stats_Conf(&awb_stats_conf);
    ISP_Set_AWB3_Stats_State(1);

    // ae_stat
    isp_ae_stats_cfg_t conf = {in_width, in_height, AE_STATS_GRID_NUM_X, AE_STATS_GRID_NUM_Y};
    isp_set_AE_stats_conf (&conf);
    ISP_Set_AE_Ysum_Coeff(77, 150, 29);
    ISP_Set_AE_State(1);

    // ae_hist
    ISP_Set_AE_Hist_Mode(AE_HIST_MODE_Y);
    isp_ae_hist_roi_t roi_cfg = {0, in_width - 1, 0, in_height - 1};
    ISP_Set_AE_Hist_ROI(1, &roi_cfg);
    ISP_Set_AE_Hist_State(1);

    // disable LPF (check with SoC)
    ISP_Set_LPF_Filting_Mode(1);
}

