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
#include "firmware_yuvadj.h"
#include "hal_isp_module.h"
#include "img_param.h"
#include "img_param_parse.h"

static isp_yuv_adj_cfg_t   yuvadj_reg;


const yuvadj_param_t limitrange_config = {
    .y_luma = 0,
    .y_mul0 = 862,
    .y_mul1 = 896,
    .y_min  = 16,
    .y_max  = 235,
    .uv_mul0= 903,
    .uv_mul1= 896,
    .uv_min = 16,
    .uv_max = 240,
};

const yuvadj_param_t fullrange_config = {
    .y_luma  = 0,
    .y_mul0  = 1024,
    .y_mul1  = 1024,
    .y_min   = 0,
    .y_max   = 255,
    .uv_mul0 = 1024,
    .uv_mul1 = 1024,
    .uv_min  = 0,
    .uv_max  = 255,
};


void yuvadj_init(void)
{
}

void yuvadj_hw_update(void)
{
   isp_set_YUVADJ_cfg(&yuvadj_reg);
}

void yuvadj_control(const yuvadj_param_t *config)
{

   yuvadj_reg.y_luma = config->y_luma;
   yuvadj_reg.y_mul0 = config->y_mul0;
   yuvadj_reg.y_mul1 = config->y_mul1;
   yuvadj_reg.y_min  = config->y_min;
   yuvadj_reg.y_max  = config->y_max;
   yuvadj_reg.uv_mul0= config->uv_mul0;
   yuvadj_reg.uv_mul1= config->uv_mul1;
   yuvadj_reg.uv_min = config->uv_min;
   yuvadj_reg.uv_max = config->uv_max;

    yuvadj_hw_update();
}

int yuvadj_img_update()
{
    yuvadj_param_t *config = 0;
    YUVADJ_RANGE_MODE_E mode = 0;

    img_param_sw_get_yuvadj_mode(&mode);
    if (mode == YUVADJ_RANGE_LIMIT) {
        yuvadj_control(&limitrange_config);
    } else if (mode == YUVADJ_RANGE_FULL) {
        yuvadj_control(&fullrange_config);
    } else if (mode == YUVADJ_RANGE_USER_DEFINE) {
        img_param_sw_get_yuvadj_manual(&config);
        yuvadj_control(config);
    } else {
        return -1;
    }

    return 0;
}


int imgproc_yuvadj(bool is_update_force)
{
    if (!img_param_sw_get_states(ISP_ADJ_YUVADJ)) {
        isp_set_YUVADJ_state(DISABLE);
        return -1;
    }

    isp_set_YUVADJ_state(ENABLE);
    yuvadj_img_update();

    return 0;
}


