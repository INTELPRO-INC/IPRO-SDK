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

#include <string.h>
#include "img_param.h"
#include "img_param_parse.h"
#include "sensor.h"

#define GAIN_ALPHA_PRECISION         (8)

static void linear_intpl_by_gain(int value_L, int value_H, GAIN_DB gain, int *value_out)
{
    int idx;
    GAIN_DB gain_L, gain_H;

    idx = (gain / GAIN_6_DB);
    if (gain < 0) idx--;
    gain_L = idx * GAIN_6_DB;
    gain_H = gain_L + GAIN_6_DB;

    intpl_point_t point_L = create_intpl_point(gain_L, value_L);
    intpl_point_t point_H = create_intpl_point(gain_H, value_H);
    linear_interpolate(&point_L, &point_H, gain, value_out);
}

static int getSysGainIdx(GAIN_DB sys_gain)
{
    int param_idx = 0;
    int temp_idx = (sys_gain / GAIN_6_DB);
    if (sys_gain < 0) param_idx = temp_idx + 8;
    else param_idx = temp_idx + 9;

    return param_idx;
}

static int getIsoIdx(GAIN_DB gain)
{
    int param_idx = (gain <= 0) ? 0 : (gain / GAIN_6_DB);

    return param_idx;
}

#define MAX_ISO_TBL_NUM (16)
typedef struct {
    uint8_t idx[MAX_ISO_TBL_NUM];
    uint32_t iso[MAX_ISO_TBL_NUM];
    GAIN_DB gain[MAX_ISO_TBL_NUM];
} iso_trans_tbl_t;


/*********** sw tbl ************************************************************************************************/
int calc_ae_sw_config_by_sys_gain(uint32_t sys_gain, ae_sub_attr *sw_config)
{
    int param_idx, value;
    ae_sub_attr *sw_param_L, *sw_param_H;
    int num_target_tbl = MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO;
    ae_sub_attr *sw_config_t;

    if (sys_gain > MAX_SYS_GAIN_TBL) {
        img_param_sw_get_ae(num_target_tbl - 1, &sw_config_t);
        memcpy(sw_config, sw_config_t, sizeof(ae_sub_attr));
    } else if (sys_gain < MIN_SYS_GAIN_TBL) {
        img_param_sw_get_ae(0, &sw_config_t);
        memcpy(sw_config, sw_config_t, sizeof(ae_sub_attr));
    } else {
        param_idx = getSysGainIdx(sys_gain);
        
        img_param_sw_get_ae(param_idx, &sw_param_L);
        img_param_sw_get_ae(param_idx + 1, &sw_param_H);

        linear_intpl_by_gain(sw_param_L->ae_target, sw_param_H->ae_target, sys_gain, &value);
        sw_config->ae_target = value;
    }

     return 0;
}
#if 0
static int calc_ae_highlight_weight_by_sys_gain(uint32_t sys_gain, ae_hightlight_weight *sw_config)
{
    int param_idx, value;
    ae_hightlight_weight *sw_param_L, *sw_param_H;
    int num_target_tbl = MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO;
    ae_hightlight_weight *sw_config_t;

    if (sys_gain > MAX_SYS_GAIN_TBL) {
        img_param_sw_get_ae_highlight_weight(num_target_tbl - 1, &sw_config_t);
        memcpy(sw_config, sw_config_t, sizeof(ae_hightlight_weight));
    } else if (sys_gain < MIN_SYS_GAIN_TBL) {
        img_param_sw_get_ae_highlight_weight(0, &sw_config_t);
        memcpy(sw_config, sw_config_t, sizeof(ae_hightlight_weight));
    } else {
        param_idx = getSysGainIdx(sys_gain);

        img_param_sw_get_ae_highlight_weight(param_idx, &sw_param_L);
        img_param_sw_get_ae_highlight_weight(param_idx + 1, &sw_param_H);

        linear_intpl_by_gain(sw_param_L->idx[0], sw_param_H->idx[0], sys_gain, &value);
        sw_config->idx[0] = value;
        linear_intpl_by_gain(sw_param_L->idx[1], sw_param_H->idx[1], sys_gain, &value);
        sw_config->idx[1] = value;
        linear_intpl_by_gain(sw_param_L->idx[2], sw_param_H->idx[2], sys_gain, &value);
        sw_config->idx[2] = value;
    }

     return 0;
}

int calc_ae_sw_highlight_weight_by_sys_gain(uint32_t sys_gain, int hist_sum, int *idx)
{
    int value;
    ae_hightlight_weight sw_config;
    ae_hightlight_weight_conf *highlight_weight_conf;
    intpl_point_t point_L, point_H;

    calc_ae_highlight_weight_by_sys_gain(sys_gain, &sw_config);

    img_param_sw_get_ae_highlight_weight_conf(&highlight_weight_conf);
     if (hist_sum <= highlight_weight_conf->thr[0]) {
        *idx = (int)sw_config.idx[0];
    } else if (hist_sum < highlight_weight_conf->thr[1]) {
        point_L = create_intpl_point(highlight_weight_conf->thr[0], sw_config.idx[0]);
        point_H = create_intpl_point(highlight_weight_conf->thr[1], sw_config.idx[1]);
        linear_interpolate(&point_L, &point_H, hist_sum, &value);
        *idx = value;
    } else if (hist_sum < highlight_weight_conf->thr[2]) {
        *idx = (int)sw_config.idx[1];
    } else if (hist_sum < highlight_weight_conf->thr[3]) {
        point_L = create_intpl_point(highlight_weight_conf->thr[2], sw_config.idx[1]);
        point_H = create_intpl_point(highlight_weight_conf->thr[3], sw_config.idx[2]);
        linear_interpolate(&point_L, &point_H, hist_sum, &value);
        *idx = value;
    } else {
        *idx = (int)sw_config.idx[2];
    }
    return 0;
}

int calc_awb_sw_config_by_sys_gain(uint32_t sys_gain, IPRO_COLOR_TEMP color_temp, awb_rb_gain_t *rb_bias)
{
    int i = 0;
    int gain_idx, value;
    int ct_idx = 2;
    awb_bais_t *awb_bias_L, *awb_bias_H;
    awb_bais_t awb_bias_P;
    intpl_point_t point_L, point_H;
    int num_awb_bias = 19;
    int ct_idx_tbl[] = {2700, 5000, 7000};
    int num_ct = 3;
    //int sys_gain_db = 0;

    if (sys_gain > MAX_SYS_GAIN_TBL) {// 54 * 1536
        img_param_sw_get_awb_preference_gain(num_awb_bias - 1, &awb_bias_L);
        memcpy(&awb_bias_P, awb_bias_L, sizeof(awb_bais_t));
    } else if (sys_gain < MIN_SYS_GAIN_TBL) {// -54 * 1536
        img_param_sw_get_awb_preference_gain(0, &awb_bias_L);
        memcpy(&awb_bias_P, awb_bias_L, sizeof(awb_bais_t));
    } else {
        gain_idx = getSysGainIdx(sys_gain);

        img_param_sw_get_awb_preference_gain(gain_idx, &awb_bias_L);
        img_param_sw_get_awb_preference_gain(gain_idx + 1, &awb_bias_H);
        for (i = 0; i < num_ct; i++) {
            linear_intpl_by_gain((int)awb_bias_L->bais[i].r_gain, (int)awb_bias_H->bais[i].r_gain, sys_gain, &value);
            awb_bias_P.bais[i].r_gain = value;
            linear_intpl_by_gain((int)awb_bias_L->bais[i].b_gain, (int)awb_bias_H->bais[i].b_gain, sys_gain, &value);
            awb_bias_P.bais[i].b_gain = value;
        }
    }

    if (color_temp <= ct_idx_tbl[0]) {
        ct_idx = 0;
        rb_bias->r_gain = awb_bias_P.bais[ct_idx].r_gain;
        rb_bias->b_gain = awb_bias_P.bais[ct_idx].b_gain;
        return 0;
    } else if (color_temp >= ct_idx_tbl[2]) {
        ct_idx = 2;
        rb_bias->r_gain = awb_bias_P.bais[ct_idx].r_gain;
        rb_bias->b_gain = awb_bias_P.bais[ct_idx].b_gain;
        return 0;
    } else {
        for (i = 1; i < num_ct; i++) {
            if (color_temp <= ct_idx_tbl[i]) {
                ct_idx = i - 1;
                break;
            }
        }

        /* interpolate r_gain */
        point_L = create_intpl_point(ct_idx_tbl[ct_idx],   awb_bias_P.bais[ct_idx].r_gain);
        point_H = create_intpl_point(ct_idx_tbl[ct_idx+1], awb_bias_P.bais[ct_idx+1].r_gain);
        linear_interpolate(&point_L, &point_H, color_temp, &value);
        rb_bias->r_gain = value;

        /* interpolate b_gain */
        point_L = create_intpl_point(ct_idx_tbl[ct_idx],   awb_bias_P.bais[ct_idx].b_gain);
        point_H = create_intpl_point(ct_idx_tbl[ct_idx+1], awb_bias_P.bais[ct_idx+1].b_gain);
        linear_interpolate(&point_L, &point_H, color_temp, &value);
        rb_bias->b_gain = value;
    }

    return 0;
}
#endif
int calc_tm_sw_config_by_sys_gain(GAIN_DB sys_gain, tm_param_t *sw_config)
{
    int param_idx = 0, value = 0, i = 0;
    tm_param_t *sw_param_L, *sw_param_H;
    int num_tm_tbl = MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO;
    tm_param_t *sw_config_t;
    
    if (sys_gain > MAX_SYS_GAIN_TBL) {
        img_param_sw_get_tm(num_tm_tbl - 1, &sw_config_t);
        memcpy(sw_config, sw_config_t, sizeof(tm_param_t));
    } else if (sys_gain < MIN_SYS_GAIN_TBL) {
        img_param_sw_get_tm(0, &sw_config_t);
        memcpy(sw_config, sw_config_t, sizeof(tm_param_t));
    } else {
        param_idx = getSysGainIdx(sys_gain);

        img_param_sw_get_tm(param_idx, &sw_param_L);
        img_param_sw_get_tm(param_idx + 1, &sw_param_H);

        linear_intpl_by_gain(sw_param_L->curve_weight, sw_param_H->curve_weight, sys_gain, &value);
        sw_config->curve_weight = value;
        linear_intpl_by_gain(sw_param_L->dark_max_ratio, sw_param_H->dark_max_ratio, sys_gain, &value);
        sw_config->dark_max_ratio = value;
        linear_intpl_by_gain(sw_param_L->bright_max_ratio, sw_param_H->bright_max_ratio, sys_gain, &value);
        sw_config->bright_max_ratio = value;

        for (i = 0; i < MAX_ENTRY_NUM_TM_IDX_AUTO; i++) {
            linear_intpl_by_gain(sw_param_L->dark_weight[i], sw_param_H->dark_weight[i], sys_gain, &value);
            sw_config->dark_weight[i] = value;
            linear_intpl_by_gain(sw_param_L->bright_weight[i], sw_param_H->bright_weight[i], sys_gain, &value);
            sw_config->bright_weight[i] = value;
        }
    }

    return 0;
}

int calc_tm_curve_config_by_sys_gain(GAIN_DB sys_gain, tm_curve_t *sw_config)
{
    int param_idx = 0, value = 0, i = 0;
    tm_curve_t *sw_param_L, *sw_param_H;
    int num_tm_tbl = MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO;

    if (sys_gain > MAX_SYS_GAIN_TBL) {
        img_param_sw_get_tm_auto_curve(num_tm_tbl - 1, &sw_config);
    } else if (sys_gain < MIN_SYS_GAIN_TBL) {
        img_param_sw_get_tm_auto_curve(0, &sw_config);
    } else {
        param_idx = getSysGainIdx(sys_gain);

        img_param_sw_get_tm_auto_curve(param_idx, &sw_param_L);
        img_param_sw_get_tm_auto_curve(param_idx + 1, &sw_param_H);

        for (i = 0; i < MAX_ENTRY_NUM_TM_CURVE_COEF_NUM; i++) {
            linear_intpl_by_gain(sw_param_L->coef[i], sw_param_H->coef[i], sys_gain, &value);
            sw_config->coef[i] = value;
        }
    }

    return 0;
}

int calc_defog_sw_config_by_sys_gain(GAIN_DB sys_gain, defog_param_t *sw_config)
{

    int param_idx = 0, value = 0;
    defog_param_t *sw_param_L, *sw_param_H;
    int num_defog_tbl = MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO;
    defog_param_t *sw_config_t;
    
    if (sys_gain > MAX_SYS_GAIN_TBL) {
        img_param_sw_get_defog(num_defog_tbl - 1, &sw_config_t);
        memcpy(sw_config, sw_config_t, sizeof(defog_param_t));
    } else if (sys_gain < MIN_SYS_GAIN_TBL) {
        img_param_sw_get_defog(0, &sw_config_t);
        memcpy(sw_config, sw_config_t, sizeof(defog_param_t));
    } else {
        param_idx = getSysGainIdx(sys_gain);

        img_param_sw_get_defog(param_idx, &sw_param_L);
        img_param_sw_get_defog(param_idx + 1, &sw_param_H);

        linear_intpl_by_gain(sw_param_L->enh_weight, sw_param_H->enh_weight, sys_gain, &value);
        sw_config->enh_weight = value;
        linear_intpl_by_gain(sw_param_L->bright_enh_level, sw_param_H->bright_enh_level, sys_gain, &value);
        sw_config->bright_enh_level = value;
        linear_intpl_by_gain(sw_param_L->dark_enh_level, sw_param_H->dark_enh_level, sys_gain, &value);
        sw_config->dark_enh_level = value;
    }

    return 0;
}


int calc_cs_sw_config_by_iso(GAIN_DB gain, cs_param_t *sw_config)
{
    int param_idx, value;
    cs_param_t *sw_param_L, *sw_param_H;

    param_idx = getIsoIdx(gain);

    img_param_sw_get_cs(param_idx, &sw_param_L);
    img_param_sw_get_cs(param_idx + 1, &sw_param_H);

    linear_intpl_by_gain(sw_param_L->strength, sw_param_H->strength, gain, &value);
    sw_config->strength = value;

    return 0;
}

int calc_cs_trans_tbl(const cs_param_t *sw_config, cs_trans_tbl_t *res)
{
    int value;
    cs_trans_tbl_t *sw_param_L, *sw_param_H;
    intpl_point_t point_L, point_H;

    img_param_sw_get_cs_trnas_tbl(0, &sw_param_L);
    img_param_sw_get_cs_trnas_tbl(1, &sw_param_H);

    if (sw_config->strength <= sw_param_L->strength) {
        res->weight = sw_param_L->weight;
        res->grey_thr = sw_param_L->grey_thr;
    } else if (sw_config->strength >= sw_param_H->strength) {
        res->weight = sw_param_H->weight;
        res->grey_thr = sw_param_H->grey_thr;
    } else {
        point_L = create_intpl_point(sw_param_L->strength, sw_param_L->weight);
        point_H = create_intpl_point(sw_param_H->strength, sw_param_H->weight);
        linear_interpolate(&point_L, &point_H, sw_config->strength, &value);
        res->weight = value;

        point_L = create_intpl_point(sw_param_L->strength, sw_param_L->grey_thr);
        point_H = create_intpl_point(sw_param_H->strength, sw_param_H->grey_thr);
        linear_interpolate(&point_L, &point_H, sw_config->strength, &value);
        res->grey_thr = value;
    }
    return 0;


}
int calc_yuvctrl_sw_config_by_iso(GAIN_DB gain, yuvctrl_param_t *sw_config)
{
    int param_idx = 0, value = 0;
    yuvctrl_param_t *sw_param_L, *sw_param_H;

    param_idx = getIsoIdx(gain);

    img_param_sw_get_yuvctrl(param_idx, &sw_param_L);
    img_param_sw_get_yuvctrl(param_idx + 1, &sw_param_H);

    linear_intpl_by_gain(sw_param_L->brightness, sw_param_H->brightness, gain, &value);
    sw_config->brightness = (uint8_t)value;
    linear_intpl_by_gain(sw_param_L->contrast, sw_param_H->contrast, gain, &value);
    sw_config->contrast = (uint8_t)value;
    linear_intpl_by_gain(sw_param_L->saturation, sw_param_H->saturation, gain, &value);
    sw_config->saturation = (uint8_t)value;
 /*
    linear_intpl_by_gain(sw_param_L->specialMode, sw_param_H->specialMode, gain, &value);
    sw_config->specialMode = (uint8_t)value;
    linear_intpl_by_gain(sw_param_L->sepiaCb, sw_param_H->sepiaCb, gain, &value);
    sw_config->sepiaCb = (uint8_t)value;
    linear_intpl_by_gain(sw_param_L->sepiaCr, sw_param_H->sepiaCr, gain, &value);
    sw_config->sepiaCr = (uint8_t)value;
    linear_intpl_by_gain(sw_param_L->solarThred, sw_param_H->solarThred, gain, &value);
    sw_config->solarThred = (uint8_t)value;
*/
    return 0;
}
/*********** sw tbl end ************************************************************************************************/
