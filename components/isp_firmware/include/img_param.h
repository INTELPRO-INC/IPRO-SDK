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

#ifndef __IMG_PARAM_H__
#define __IMG_PARAM_H__

#include <stdint.h>
#include "isp_comm_sns.h"
#include "img_struct.h"
/*
typedef enum {
    ISP_HSL_LIGHT_BY_HUE = 0,
    ISP_HSL_SAT_BY_HUE,
    ISP_HSL_SAT_BY_LIGHT,
    ISP_HSL_GAIN_TBL_NUM
} ISP_HSV_GAIN_TBL_E;

typedef enum {
    ISP_PFC_HUE = 0,
    ISP_PFC_SAT,
    ISP_PFC_LIGHT,
    ISP_PFC_COLOR_NUM
} ISP_PFC_COLOR_E;
*/
#if 0
int img_param_malloc_sw_param_init_buffer(void);
int img_param_free_sw_param_init_buffer(void);

//******* calib *********************************************************************************//
int img_param_calib_load(const char* sensor_name);
int img_param_calib_get_wb_info(int param_idx, wb_info_t **wb_info);
int img_param_calib_set_wb_info(const wb_info_t *wb_info_cal, int wb_info_num);
int img_param_calib_get_wb_info_num(void);
int img_param_calib_get_awb(wb3_cal_bin_t **wb3_cal);
int img_param_calib_set_awb(const wb3_cal_bin_t *wb3_cal);
/*int img_param_calib_get_gamma(int param_idx, gamma_cal_t **gamma);
int img_param_calib_set_gamma(const gamma_cal_t *gamma_cal);
*/
#endif
//******* fw *********************************************************************************//
int img_param_load(const char* sensor_name, int iq_param_type);
#if 0
int img_param_get_module(const IPRO_IMG_PARAM_DESC_T **module);
int img_param_get_states(const Fun_Type **state);


int img_param_get_ae_agc_db_tbl(const ae_agc_db_tbl **cfg);
int img_param_get_ae_slow_shutter_conf(const ae_slow_shutter_conf_t **cfg);
#endif

int img_param_get_wb_info(int param_idx, const wb_info_t **wb_info);
int img_param_set_wb_info(int param_idx, wb_info_t *wb_info);
int img_param_get_num_wb_info(void);
int img_param_set_num_wb_info(int num);
int img_param_get_wb3_cal(const wb3_cal_bin_t **wb3_cal);
int img_param_set_wb3_cal(wb3_cal_bin_t *wb3_cal);
#if 0
int img_param_get_cs_trans(const cs_trans_tbl_t **trans_tbl, int *tbl_num);
#endif
//******* sw *********************************************************************************//
//int img_param_sw_tbl_init(void);
//int img_param_get_sw_bin_Addr(uint32_t *addr);
int img_param_sw_load(const char* sensor_name);

int img_param_sw_get_module(const IPRO_IMG_PARAM_SW_DESC_T **module);
bool img_param_sw_get_states(int param_idx);
int img_param_sw_set_states(int param_idx, const bool state);

bool img_param_sw_get_ae_manual_mode(void);
int img_param_sw_set_ae_manual_mode(const bool isMnaual);
int img_param_sw_get_ae_manual(ae_param_t **cfg);
int img_param_sw_set_ae_manual(const ae_param_t *cfg);
int img_param_sw_get_ae(int param_idx, ae_sub_attr **cfg);
int img_param_sw_set_ae(int param_idx, const ae_sub_attr *cfg);
int img_param_sw_get_ae_metering_tbl(int param_idx, ae_metering_tbl_t **cfg);
int img_param_sw_set_ae_metering_tbl(const ae_metering_tbl_t *cfg);
/*int img_param_sw_get_ae_highlight_weight_en(int *en);
int img_param_sw_set_ae_highlight_weight_en(const int en);
int img_param_sw_get_ae_highlight_weight(int param_idx, ae_hightlight_weight **cfg);
int img_param_sw_set_ae_highlight_weight(int param_idx, const ae_hightlight_weight *cfg);
int img_param_sw_get_ae_highlight_weight_conf(ae_hightlight_weight_conf **cfg);
int img_param_sw_set_ae_highlight_weight_conf(const ae_hightlight_weight_conf *cfg);*/
int img_param_sw_get_ae_metering_mode(AE_METERING *cfg);
int img_param_sw_set_ae_metering_mode(const AE_METERING cfg);
/*int img_param_sw_get_ae_speed_level(int *cfg);
int img_param_sw_set_ae_speed_level(const int cfg);*/

bool img_param_sw_get_awb_manual_mode(void);
int img_param_sw_set_awb_manual_mode(const bool isMnaual);
int img_param_sw_get_awb_manual(awb_param_t **cfg);
int img_param_sw_set_awb_manual(const awb_param_t *cfg);
/*int img_param_sw_get_awb_ct(awb_ct_param_t **cfg);
int img_param_sw_set_awb_ct(const awb_ct_param_t *cfg);
int img_param_sw_get_awb_speed_level(int *cfg);
int img_param_sw_set_awb_speed_level(const int cfg);*/
int img_param_sw_get_awb_special_color_en(int *en);
int img_param_sw_set_awb_special_color_en(const int en);
int img_param_sw_get_awb_special_color(int param_idx, awb_special_color_t **cfg);
int img_param_sw_set_awb_special_color(int param_idx, const awb_special_color_t *cfg);
/*int img_param_sw_get_awb_preference_en(int *en);
int img_param_sw_set_awb_preference_en(const int en);
int img_param_sw_get_awb_preference_gain(int param_idx, awb_bais_t **cfg);
int img_param_sw_set_awb_preference_gain(int param_idx, const awb_bais_t *cfg);*/

int img_param_sw_get_gamma_curve_mode(GAMMA_CURVE_MODE_E *mode);
int img_param_sw_set_gamma_curve_mode(const GAMMA_CURVE_MODE_E mode);
int img_param_sw_get_gamma(gamma_curve_t **cfg);
int img_param_sw_set_gamma(const gamma_curve_t *cfg);

int img_param_sw_get_ct_color_gamut(CT_GAMUT_E *color_gamut);
int img_param_sw_set_ct_color_gamut(const CT_GAMUT_E color_gamut);
int img_param_sw_get_ct_manual(ct_param_t **cfg);
int img_param_sw_set_ct_manual(const ct_param_t *cfg);

bool img_param_sw_get_tm_manual_mode(void);
int img_param_sw_set_tm_manual_mode(const bool isMnaual);
int img_param_sw_get_tm_type(TM_TYPE_E *type);
int img_param_sw_set_tm_type(const TM_TYPE_E type);

int img_param_sw_get_tm_manual(tm_param_t **cfg);
int img_param_sw_set_tm_manual(const tm_param_t *cfg);
int img_param_sw_get_tm(int param_idx, tm_param_t **cfg);
int img_param_sw_set_tm(int param_idx, const tm_param_t *cfg);
int img_param_sw_get_tm_manual_curve(tm_curve_t **cfg);
int img_param_sw_set_tm_manual_curve(const tm_curve_t *cfg);
int img_param_sw_get_tm_auto_curve(int param_idx, tm_curve_t **cfg);
int img_param_sw_set_tm_auto_curve(int param_idx, const tm_curve_t *cfg);


bool img_param_sw_get_defog_manual_mode(void);
int img_param_sw_set_defog_manual_mode(const bool isMnaual);
int img_param_sw_get_defog_manual(defog_param_t **cfg);
int img_param_sw_set_defog_manual(const defog_param_t *cfg);
int img_param_sw_get_defog(int param_idx, defog_param_t **cfg);
int img_param_sw_set_defog(int param_idx, const defog_param_t *cfg);

bool img_param_sw_get_cs_manual_mode(void);
int img_param_sw_set_cs_manual_mode(const bool isMnaual);
int img_param_sw_get_cs_manual(cs_param_t **cfg);
int img_param_sw_set_cs_manual( const cs_param_t *cfg);
int img_param_sw_get_cs(int param_idx, cs_param_t **cfg);
int img_param_sw_set_cs(int param_idx, const cs_param_t *cfg);
int img_param_sw_get_cs_trnas_tbl(int param_idx, cs_trans_tbl_t **cfg);
int img_param_sw_set_cs_trnas_tbl(int param_idx, const cs_trans_tbl_t *cfg);

bool img_param_sw_get_yuvctrl_manual_mode(void);
int img_param_sw_set_yuvctrl_manual_mode(const bool isMnaual);
int img_param_sw_get_yuvctrl_manual(yuvctrl_param_t **cfg);
int img_param_sw_set_yuvctrl_manual(const yuvctrl_param_t *cfg);
int img_param_sw_get_yuvctrl(int param_idx, yuvctrl_param_t **cfg);
int img_param_sw_set_yuvctrl(int param_idx, const yuvctrl_param_t *cfg);

int img_param_sw_get_yuvadj_mode(YUVADJ_RANGE_MODE_E *mode);
int img_param_sw_set_yuvadj_mode(const YUVADJ_RANGE_MODE_E mode);
int img_param_sw_get_yuvadj_manual(yuvadj_param_t **cfg);
int img_param_sw_set_yuvadj_manual(const yuvadj_param_t *cfg);

void print_sw_tbl(void);
#endif /* __IMG_PARAM_H__ */

