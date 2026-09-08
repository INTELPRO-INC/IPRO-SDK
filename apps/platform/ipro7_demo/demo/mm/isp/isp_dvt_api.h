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

#ifndef __ISP_DVT_API_H__
#define __ISP_DVT_API_H__

#include "drv_common.h"
#include <stdint.h>
#include "hal_isp.h"

void isp_set_default_param(uint32_t bld_img_addr, uint16_t in_width, uint16_t in_height);
void isp_module_interrupt(uint8_t from_sensor);
void sof_update_param(uint8_t from_sensor);

#endif  /* __ISP_DVT_API_H__ */
