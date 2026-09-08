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
#include "defog_algo.h"
#include "firmware_defog.h"
#include "hal_isp_module.h"
#include "img_param.h"
#include "img_param_parse.h"

static defog_fw_config_t fw_conf;

void defog_hw_update(void)
{
    isp_defog_t algo;

    defog_algo_get_conf(&algo);
    isp_set_DEFOG_enh(&algo);
}

int defog_init()
{
    defog_algo_set_reg_default();
    defog_algo_fw_param_init();

    return 0;
}

int defog_control(const defog_param_t *config)
{
    int ret = 0;
    
    fw_conf.param.enh_weight        = config->enh_weight;
    fw_conf.param.bright_enh_level  = config->bright_enh_level;
    fw_conf.param.dark_enh_level    = config->dark_enh_level;

    ret = defog_algo_control(&fw_conf);
    defog_hw_update();

    return ret;
}

int defog_img_update(GAIN_DB gain)
{
    defog_param_t config = {0};
    
    if (calc_defog_sw_config_by_sys_gain(gain, &config) == -1) return 0;
    fw_conf.sys_gain_db = gain;
    defog_control(&config);

    return 0;
}

int defog_img_manual(void)
{
    defog_param_t *config = 0;

    img_param_sw_get_defog_manual(&config);
    defog_control(config);

    return 0;
}

int imgproc_defog(uint8_t flag_sys_gain, GAIN_DB sys_gain, bool is_update_force)
{
    if (!img_param_sw_get_states(ISP_ADJ_DEFOG)) {
        isp_set_DEFOG_state(DISABLE);
        return -1;
    }

    isp_set_DEFOG_state(ENABLE);
    if (img_param_sw_get_defog_manual_mode()) {
        defog_img_manual();
    } else if (flag_sys_gain || is_update_force) {
        defog_img_update(sys_gain);
    }

    return 0;
}

