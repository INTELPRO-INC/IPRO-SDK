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

#ifndef __DEFOG_ALGO_H__
#define __DEFOG_ALGO_H__

#include "../../isp_firmware/include/hal_isp_module.h"
#include "../../isp_firmware/include/img_struct.h"

typedef struct {
    defog_param_t param;
    int sys_gain_db;
} defog_fw_config_t;

int defog_get_algo_version(int *version);
void defog_algo_set_reg_default(void);
void defog_algo_fw_param_init(void);
int defog_algo_control(const defog_fw_config_t *defog_fw);
int defog_algo_get_conf(isp_defog_t *conf);

#endif
