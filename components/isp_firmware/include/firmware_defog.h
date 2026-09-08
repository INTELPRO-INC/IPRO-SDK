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

#ifndef __FIRMWARE_DEFOG_H__
#define __FIRMWARE_DEFOG_H__

#include "img_struct.h"

int  defog_init(void);

int  defog_control(const defog_param_t *config);
int defog_img_update(GAIN_DB sys_gain);
int defog_img_manual(void);
int imgproc_defog(uint8_t flag_sys_gain, GAIN_DB sys_gain, bool is_update_force);


#endif
