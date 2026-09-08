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

#include <stdint.h>
#include "tm_algo.h"
#include "firmware_tonemapping.h"
#include "hal_isp_module.h"
#include "img_param.h"
#include "img_param_parse.h"

#define TM_CONVERGE_FRAME_CNT     5

static tm_fw_config_t    tm_fw;
static int tm_need_update = TM_CONVERGE_FRAME_CNT;

void tm_hw_update(void)
{
    tm_algo_conf_t conf = {0};

    tm_algo_get_conf(&conf);
    isp_set_TM_curve(&conf.curve);
    isp_set_TM_curve_weight(conf.curve_weight);
}

int tm_init()
{
    tm_config_t cfg = {0};
    cfg.stats_cfg.act_win.width = 1280;
    cfg.stats_cfg.act_win.height = 720;
    tm_config(&cfg, 0);

    tm_algo_set_reg_default(1280, 720);
    tm_algo_fw_param_init();

    return 0;
}

int tm_control(const tm_param_t *config)
{
    int ret = 0;

    tm_fw.param.curve_weight = config->curve_weight;
    tm_fw.param.dark_max_ratio = config->dark_max_ratio;
    tm_fw.param.bright_max_ratio = config->bright_max_ratio;

    for (int i = 0; i < MAX_ENTRY_NUM_TM_IDX_AUTO; i++) {
        tm_fw.param.dark_weight[i] = config->dark_weight[i];
        tm_fw.param.bright_weight[i] = config->bright_weight[i];
    }
    ret = tm_algo_control(&tm_fw);
    tm_hw_update();

    return ret;
}

int tm_img_update(GAIN_DB gain)
{
    tm_param_t sw_config = {0};

    if (calc_tm_sw_config_by_sys_gain(gain, &sw_config) == -1) return 0;
    tm_control(&sw_config);

    return 0;
}


int tm_img_update_curve(GAIN_DB gain)
{
    tm_curve_t config = {0};
    isp_tm_curve_t data;

    if (calc_tm_curve_config_by_sys_gain(gain, &config) == -1) return 0;

    for(int i = 0; i < ISP_WDR_CURVE_COEF_NUM; i++) {
        data.coef[i] = config.coef[i];
    }

    isp_set_TM_curve(&data);
    isp_set_TM_curve_weight(16);

    return 0;
}


int tm_img_manual(void)
{
    tm_param_t *config = 0;

    img_param_sw_get_tm_manual(&config);
    tm_control(config);

    return 0;
}

int tm_img_manual_curve(void)
{
    tm_curve_t *config;
    isp_tm_curve_t data;

    img_param_sw_get_tm_manual_curve(&config);

    for(int i = 0; i < ISP_WDR_CURVE_COEF_NUM; i++) {
        data.coef[i] = config->coef[i];
    }

    isp_set_TM_curve(&data);
    isp_set_TM_curve_weight(16);

    return 0;
}

int imgproc_tonemapping(uint8_t flag_sys_gain, GAIN_DB sys_gain, bool is_update_force)
{

    TM_TYPE_E tm_type = 0;

    if (flag_sys_gain){
        tm_need_update = TM_CONVERGE_FRAME_CNT;
    } else{
        if (tm_need_update > 0)
            tm_need_update--;
    }

    if (!img_param_sw_get_states(ISP_ADJ_TM)) {
        isp_set_TM_state(DISABLE);
        return -1;
    }

    isp_set_TM_state(ENABLE);
    img_param_sw_get_tm_type(&tm_type);
    if (tm_type == ISP_TM_TYPE_HIST_DEFINE) {
        if (img_param_sw_get_tm_manual_mode()) {
            tm_img_manual();
        } else if (tm_need_update || is_update_force) {
            tm_img_update(sys_gain);
        }
    } else if (tm_type == ISP_TM_TYPE_USER_DEFINE) {
        if (img_param_sw_get_tm_manual_mode()) {
            tm_img_manual_curve();
        } else if (tm_need_update || is_update_force) {
            tm_img_update_curve(sys_gain);
        }
    } else {
        return -1;
    }

    return 0;
}

