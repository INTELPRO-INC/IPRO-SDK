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
#include "firmware_ct.h"
#include "hal_isp_module.h"
#include "img_param.h"
#include "img_param_parse.h"

static isp_ct_coef_s coef_reg;
static isp_ct_offset_s pre_reg;
static isp_ct_offset_s post_reg;

static ct_param_t bt601_yuv2rgb = {
    .coef = { 512,    0,    718,
              512, -176,   -366,
              512,  907,      0,},//1x:512
    .pre_offset = {0, -2048, -2048},//1x 16
    .post_offset = {0, 0, 0},
};
static ct_param_t bt709_yuv2rgb = {
    .coef = { 512,    0,    806,
              512,  -95,   -240,
              512,  950,      0,},//1x:512
    .pre_offset = {0, -2048, -2048},//1x 16
    .post_offset = {0, 0, 0},

};

void ct_hw_update(void)
{
    isp_set_CT_conf(&coef_reg);
    isp_set_CT_pre_offset(&pre_reg);
    isp_set_CT_post_offset(&post_reg);
}

void ct_init(void)
{

}

void ct_control(const ct_param_t *config)
{
    int i;

    for(i = 0; i < CE_COEF_NUM; i++) coef_reg.coef[i] = config->coef[i];
    for(i = 0; i < CE_OFFSET_NUM; i++) pre_reg.offset[i] = config->pre_offset[i];
    for(i = 0; i < CE_OFFSET_NUM; i++) post_reg.offset[i] = config->post_offset[i];
    ct_hw_update();

}
int ct_img_update(void)
{
    ct_param_t *config = 0;
    CT_GAMUT_E mode = 0;

    img_param_sw_get_ct_color_gamut(&mode);
    if (mode == ISP_CT_COLOR_GAMUT_BT709) {
       ct_control(&bt709_yuv2rgb);
    } else if (mode == ISP_CT_COLOR_GAMUT_BT601) {
       ct_control(&bt601_yuv2rgb);
    } else if (mode == ISP_CT_COLOR_GAMUT_USER) {
        img_param_sw_get_ct_manual(&config);
        ct_control(config);
    } else {
        return -1;
    }

    return 0;
}

int imgproc_ct(void)
{
    if (!img_param_sw_get_states(ISP_ADJ_CT)) {
        isp_disable_ct_soft();
        return -1;
    }

    ct_img_update();

    return 0;
}

