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

#ifndef __IMG_PARAM_PARSE_H__
#define __IMG_PARAM_PARSE_H__

#include "isp_comm_sns.h"
#include "img_struct.h"

/*********** sw tbl ************************************************************************************************/
int calc_ae_sw_config_by_sys_gain(uint32_t sys_gain, ae_sub_attr *sw_config);
//int calc_ae_sw_highlight_weight_by_sys_gain(uint32_t sys_gain, int hist_val, int *idx);
//int calc_awb_sw_config_by_sys_gain(uint32_t sys_gain, IPRO_COLOR_TEMP color_temp, awb_rb_gain_t *rb_bias);

int calc_tm_sw_config_by_sys_gain(GAIN_DB sys_gain, tm_param_t *sw_config);
int calc_tm_curve_config_by_sys_gain(GAIN_DB sys_gain, tm_curve_t *sw_config);
int calc_defog_sw_config_by_sys_gain(GAIN_DB sys_gain, defog_param_t *sw_config);
int calc_cs_sw_config_by_iso(GAIN_DB gain, cs_param_t *sw_config);
int calc_cs_trans_tbl(const cs_param_t *sw_config, cs_trans_tbl_t *res);
int calc_yuvctrl_sw_config_by_iso(GAIN_DB gain, yuvctrl_param_t *sw_config);
#endif /* __IMG_PARAM_PARSE_H__ */

