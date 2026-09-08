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

#ifndef __FIRMWARE_CS_H__
#define __FIRMWARE_CS_H__

#include "img_struct.h"

void cs_init(void);

void cs_control(const cs_param_t *config);
int cs_img_update(GAIN_DB gain);
int cs_img_manual(void);
int imgproc_cs(uint8_t flag_iso, GAIN_DB gain, bool is_update_force);

#endif
