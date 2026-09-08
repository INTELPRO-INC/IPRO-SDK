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

#ifndef __HAL_ISP_TSRC_SAMPLE_H__
#define __HAL_ISP_TSRC_SAMPLE_H__

#include "hal_common.h"
#include "board_config.h"


#define TSRC_WIDTH        (1280)
#define TSRC_HEIGHT       (720)
#if 1
#define TSRC_IN_ADDR      (0x1E900000)/*for output ring*/
#else
#define TSRC_IN_ADDR      (0x1E649800)/*for output full image*/
#endif

/*TSRC must use YUYV format.*/
#define TSRC_H_BLANK      (65535 - TSRC_WIDTH) /*(65535 - TSRC_WIDTH) case42 ok but case43 NG with psram@192MHz*/ /*(4000*10) case41 OK*/ /*(4000)*/ // magic number for scaler up usage
#define TSRC_V_BLANK      (50)


int tsrc_init(void);
int tsrc_trigger(void);

#endif
