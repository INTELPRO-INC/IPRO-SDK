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

#ifndef __SENSOR_H__
#define __SENSOR_H__


#include "hal_common.h"
#include "board_config.h"


typedef int (*_camera_probe)(void);
typedef int (*_camera_reset)(void);
typedef int (*_camera_setFMT)(void);
typedef int (*_camera_set_case)(uint8_t case_id);
typedef int (*_camera_set_awb_gain)(uint8_t enable, int r_gain, int b_gain);
typedef int (*_camera_get_awb_register)(uint8_t case_id, int *value);
typedef int (*_camera_set_ae_expo_gain)(uint8_t enable, int gain_db);
typedef int (*_camera_get_ae_register)(uint8_t case_id, int *value);
typedef int (*_camera_set_register)(int addr, int value);
typedef int (*_camera_get_register)(int addr, int *value);

typedef struct _rt_camera_desc
{
    char *name;
    uint8_t addr;
    uint8_t frm_vld_high;
    _camera_probe probe;
    _camera_reset reset;
    _camera_setFMT setFMT;
    _camera_set_case set_case;
    _camera_set_awb_gain set_awb_gain;
    _camera_get_awb_register get_awb_register;
    _camera_set_ae_expo_gain set_ae_expo_gain;
    _camera_get_ae_register get_ae_register;
    _camera_set_register set_register;
    _camera_get_register get_register;
    uint16_t width;
    uint16_t height;

    // Fifo threshold for each DVP line to start to output,
    // ((Width-Fifo_th)*T_in*2)<(Width*T_out)<((Width-Fifo_th)*T_in*2+Blank*T_in)
    uint16_t fifo_threshold;
    uint32_t pclk;
    uint16_t total_width;
    uint16_t total_height;
} rt_camera_desc;

const rt_camera_desc* camera_module_init(void);

#endif
