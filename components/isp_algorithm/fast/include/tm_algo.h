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

#ifndef __TM_ALGO_H__
#define __TM_ALGO_H__

#include "../../isp_firmware/include/hal_isp_module.h"
#include "../../isp_firmware/include/img_struct.h"

typedef struct {
    aaa_stats_cfg_t stats_cfg;
} tm_config_t;

typedef struct {
    tm_param_t param;
} tm_fw_config_t;

typedef struct {
    isp_tm_curve_t curve;
    int curve_weight;
} tm_algo_conf_t;

int tm_get_algo_version(int *version);
int tm_get_algo_state(Fun_Type *state);
int tm_set_algo_state(const Fun_Type state);
int tm_config(tm_config_t *config, bool is_virtual_sensor);
void tm_algo_set_reg_default(int width, int height);
void tm_algo_fw_param_init(void);
int tm_algo_control(const tm_fw_config_t *tm_fw);
int tm_algo_get_conf(tm_algo_conf_t *conf);


#endif
