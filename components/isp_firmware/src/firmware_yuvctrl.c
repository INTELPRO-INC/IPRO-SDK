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
#include "firmware_yuvctrl.h"
#include "hal_isp_module.h"
#include "img_param.h"
#include "img_param_parse.h"

static isp_yuvctrl_conf_t yuvctrl_reg;

void yuvctrl_hw_update(void)
{
    isp_set_YUVCTRL_conf(&yuvctrl_reg);
}

void yuvctrl_init(void)
{

}

void yuvctrl_control(const yuvctrl_param_t *config)
{
    yuvctrl_reg.brightness = config->brightness;
    yuvctrl_reg.contrast   = config->contrast;
    yuvctrl_reg.saturation = config->saturation;
    yuvctrl_hw_update();
}

int yuvctrl_img_update(GAIN_DB gain)
{
    yuvctrl_param_t config = {0};

    if (calc_yuvctrl_sw_config_by_iso(gain, &config) == -1) return 0;
   // printf("brightness %d \r\n",config.brightness);
    //printf("contrast  %d \r\n",config.contrast);
    //printf("saturation %d\r\n",config.saturation);
    yuvctrl_control(&config);

    return 0;
}

int yuvctrl_img_manual(void)
{

    yuvctrl_param_t *config = 0;

    img_param_sw_get_yuvctrl_manual(&config);
    yuvctrl_control(config);

    return 0;
}

int imgproc_yuvctrl(uint8_t flag_iso, GAIN_DB gain, bool is_update_force)
{
    if (!img_param_sw_get_states(ISP_ADJ_YUVCTRL)) {
        isp_disable_yuvctrl_soft();
        return -1;
    }

    if (img_param_sw_get_yuvctrl_manual_mode()) {
        yuvctrl_img_manual();
    } else if (flag_iso || is_update_force) {
        yuvctrl_img_update(gain);
    }

    return 0;
}

