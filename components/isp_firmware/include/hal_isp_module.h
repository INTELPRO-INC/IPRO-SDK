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

#ifndef __HAL_ISP_MODULE_H__
#define __HAL_ISP_MODULE_H__

#include <drv_isp.h>
#include <drv_isp_wdr.h>
#include "isp_comm_sns.h"
#include "img_utils.h"

typedef struct
{
    isp_hal_size_t         in_size;                    /*!< isp input size */
    isp_hal_crop_t         glb_crop;                   /*!< isp output global crop before scaler */
    char                   sensor_name[32];            /*!< isp sensor IQ */
} isp_hal_vin_cfg_t;

typedef struct {
    uint32_t y[ISP_AE_HIST_Y_BIN_NUM];
} isp_ae_hist_y_t;

typedef struct {
    uint32_t r[ISP_AE_HIST_BAYER_BIN_NUM];
    uint32_t gr[ISP_AE_HIST_BAYER_BIN_NUM];
    uint32_t gb[ISP_AE_HIST_BAYER_BIN_NUM];
    uint32_t b[ISP_AE_HIST_BAYER_BIN_NUM];
} isp_ae_hist_bayer_t;

typedef struct {
    uint8_t r_sum[AE_STATS_GRID_NUM];
    uint8_t g_sum[AE_STATS_GRID_NUM];
    uint8_t b_sum[AE_STATS_GRID_NUM];
    uint8_t y_sum[AE_STATS_GRID_NUM];
} isp_ae_grid_sum_t;

typedef struct {
    uint16_t            img_width;
    uint16_t            img_height;
    uint8_t             grid_num_x;
    uint8_t             grid_num_y;
} isp_ae_stats_cfg_t;

typedef struct {
    int id;
    int state;
    int weight;
    int x_start;
    int x_end;
    int y_start;
    int y_end;
} isp_wdr_roi_info_t;

typedef struct {
    uint32_t hist_bin[ISP_WDR_HIST_BIN_NUM];
} isp_wdr_hist_data_t;


typedef struct {
    uint16_t coef[ISP_WDR_CURVE_COEF_NUM];
} isp_tm_curve_t;

typedef struct {
    int weight;
    int y_offset;
    int y_gain;
    int c_gain;
} isp_defog_t;

typedef struct {
    uint16_t gain;
    uint8_t  weight;
    uint8_t  grey_thr;
} isp_cs_conf_t;

typedef struct {
    uint16_t coef[GAMMA_COEF_NUM];
} isp_gamma_curve_t;

typedef struct {
    int coef[CE_COEF_NUM];
} isp_ct_coef_s;

typedef struct {
    int offset[CE_OFFSET_NUM];
} isp_ct_offset_s;

typedef struct {
    int coef[CE_COEF_NUM];//[-2048, 2047], 1x:512
    int pre_offset[CE_OFFSET_NUM];//[-4096, 4095],1x:16
    int post_offset[CE_OFFSET_NUM];//[-4096, 4095],1x:16
} isp_ct_param_t;

typedef struct {
    uint8_t  brightness;
    uint8_t  contrast;
    uint8_t  saturation;
} isp_yuvctrl_conf_t;
#if 0

typedef struct {
    uint16_t enable;
    uint16_t start_x;
    uint16_t start_y;
    uint16_t end_x;
    uint16_t end_y;
} ipro_isp_crop_roi_t;
#endif
typedef struct {
    uint16_t y_luma;  /** range:0~511                 **/
    uint16_t y_mul0;  /** 1x:1024, range:0~2407       **/
    uint16_t y_mul1;  /** 1x:1024, range:0~2407       **/
    uint8_t y_min;    /** range:0~255                 **/
    uint8_t y_max;    /** range:0~255                 **/
    uint16_t uv_mul0; /** 1x:1024, range:0~2407       **/
    uint16_t uv_mul1; /** 1x:1024, range:0~2407       **/
    uint8_t uv_min;   /** range:0~255                 **/
    uint8_t uv_max;   /** range:0~255                 **/
} isp_yuv_adj_cfg_t;

/*******************************************************************/
int isp_set_AE_state(Fun_Type state);
int isp_set_AE_state_enable(void);
int isp_get_AE_stats_conf (isp_ae_stats_cfg_t *conf);
int isp_set_AE_stats_conf (isp_ae_stats_cfg_t *conf);
int isp_get_AE_stats(isp_ae_grid_sum_t *stats);
int isp_get_AE_hist_state(Fun_Type *state);
int isp_set_AE_hist_state(Fun_Type state);
int isp_get_AE_hist_mode(AE_HIST_MODE_TYPE_E *mode);
int isp_set_AE_hist_mode(AE_HIST_MODE_TYPE_E mode);
int isp_get_AE_hist_roi(uint16_t *x_min, uint16_t *x_max, uint16_t *y_min, uint16_t *y_max);
int isp_set_AE_hist_roi(uint16_t x_min, uint16_t x_max, uint16_t y_min, uint16_t y_max);
int isp_get_AE_hist_sum_y(isp_ae_hist_y_t *y_sum);
int isp_get_AE_hist_sum_bayer(isp_ae_hist_bayer_t *sum);

int isp_set_AWB_state(Fun_Type state);
int isp_set_AWB3_stats_conf(isp_awb3_stats_conf_t *awb_stats_conf);
int isp_get_AWB3_stats_conf(isp_awb3_stats_conf_t *awb_stats_conf);
int isp_get_AWB3_gDivisor(int *value);
int isp_set_AWB3_gDivisor(int value);
int isp_get_AWB3_sum(isp_awb3_sum_t *awb3_sum);

int ipro_get_WDR_state(Fun_Type *state);
int ipro_set_WDR_state(Fun_Type state);
int ipro_get_WDR_hist_roi_info(isp_wdr_roi_info_t *info);
int ipro_set_WDR_hist_roi_info(isp_wdr_roi_info_t *info);
int ipro_get_WDR_hist_data(isp_wdr_hist_data_t *data);

int isp_get_TM_state(Fun_Type *state);
int isp_set_TM_state(Fun_Type state);
int isp_get_TM_curve_state(Fun_Type *state);
int isp_Set_TM_curve_state(Fun_Type state);
int isp_get_TM_curve(isp_tm_curve_t *data);
int isp_set_TM_curve(isp_tm_curve_t *data);
int isp_get_TM_curve_weight(int *weight);
int isp_set_TM_curve_weight(int weight);

int isp_get_DEFOG_state(Fun_Type *state);
int isp_set_DEFOG_state(Fun_Type state);
int isp_get_DEFOG_enh(isp_defog_t *conf);
int isp_set_DEFOG_enh(isp_defog_t *conf);

int isp_get_CS_state(Fun_Type *state);
int isp_set_CS_state(Fun_Type state);
int isp_get_CS_conf(isp_cs_conf_t *conf);
int isp_set_CS_conf(isp_cs_conf_t *conf);

int isp_get_GAMMA_state(Fun_Type *state);
int isp_set_GAMMA_state(Fun_Type state);
int isp_get_GAMMA_curve(isp_gamma_curve_t *curve);
int isp_set_GAMMA_curve(isp_gamma_curve_t *curve);

int isp_disable_ct_soft(void);
int isp_get_CT_conf(isp_ct_coef_s *coef);
int isp_set_CT_conf(isp_ct_coef_s *coef);
int isp_get_CT_pre_offset(isp_ct_offset_s *offset);
int isp_set_CT_pre_offset(isp_ct_offset_s *offset);
int isp_get_CT_post_offset(isp_ct_offset_s *offset);
int isp_set_CT_post_offset(isp_ct_offset_s *offset);


int isp_disable_yuvctrl_soft();
int isp_get_YUVCTRL_conf(isp_yuvctrl_conf_t *conf);
int isp_set_YUVCTRL_conf(isp_yuvctrl_conf_t *conf);

int isp_set_YUVADJ_state(Fun_Type state);
int isp_set_YUVADJ_cfg(isp_yuv_adj_cfg_t *cfg);
int isp_get_YUVADJ_cfg(isp_yuv_adj_cfg_t *cfg);
#if 0
int isp_get_addr_data(uint32_t addr, uint32_t *data);
int isp_set_addr_data(uint32_t addr,  uint32_t data);
#endif
void isp_set_default_iq_param(uint16_t in_width, uint16_t in_height);
void isp_set_cal_iq_param(uint16_t in_width, uint16_t in_height);

#endif /* __HAL_ISP_MODULE__ */

