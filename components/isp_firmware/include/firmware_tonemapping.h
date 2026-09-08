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

#ifndef __FIRMWARE_TONEMAPPING_H__
#define __FIRMWARE_TONEMAPPING_H__


#include "img_struct.h"

int  tm_init(void);

int tm_control(const tm_param_t *config);
int tm_img_update(GAIN_DB gain);
int tm_img_manual(void);
int tm_img_manual_curve(void);
int tm_img_auto_curve(GAIN_DB gain);
int imgproc_tonemapping(uint8_t flag_sys_gain, GAIN_DB sys_gain, bool is_update_force);

#endif
