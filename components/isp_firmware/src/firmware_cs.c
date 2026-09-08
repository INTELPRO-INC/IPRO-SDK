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
#include "firmware_cs.h"
#include "hal_isp_module.h"
#include "img_param.h"
#include "img_param_parse.h"


typedef struct {
    int weight;
    int grey_thr;
} chroma_suppress_t;

static isp_cs_conf_t         cs_reg;

void cs_hw_update(void)
{
    isp_set_CS_conf(&cs_reg);
}

void cs_init(void)
{

}


void cs_control(const cs_param_t *config)
{
    int divisor;
    cs_trans_tbl_t res = {0};

    calc_cs_trans_tbl(config, &res);

    cs_reg.weight = res.weight;
    cs_reg.grey_thr = res.grey_thr;

    // ref to wdr
    divisor = (1 << (8 - 1)) - cs_reg.grey_thr;
    cs_reg.gain = ((1 << (8 - 1)) * (1 << 8) + (divisor - 1)) / divisor;
    //printf("weight %d, grey_thr %d, gtin %d \r\n",  cs_reg.weight, cs_reg.grey_thr, cs_reg.gain);

    cs_hw_update();
}

int cs_img_update(GAIN_DB gain)
{
    cs_param_t config;

    if (calc_cs_sw_config_by_iso(gain, &config) == -1) return 0;
    cs_control(&config);

    return 0;
}

int cs_img_manual(void)
{
    cs_param_t *config;

    img_param_sw_get_cs_manual(&config);
    cs_control(config);

    return 0;
}

int imgproc_cs(uint8_t flag_iso, GAIN_DB gain, bool is_update_force)
{
    if (!img_param_sw_get_states(ISP_ADJ_CS)) {
        isp_set_CS_state(DISABLE);
        return -1;
    }

    isp_set_CS_state(ENABLE);
    if (img_param_sw_get_cs_manual_mode()) {
        cs_img_manual();
    } else if (flag_iso || is_update_force) {
        cs_img_update(gain);
    }

    return 0;
}

