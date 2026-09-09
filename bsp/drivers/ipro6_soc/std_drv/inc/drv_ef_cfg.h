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


#ifndef __DRV_EF_CFG_H__
#define __DRV_EF_CFG_H__

#include "ef_ctrl_reg.h"
#include "drv_common.h"

uint8_t EF_Ctrl_Is_MAC_Address_Slot_Empty(uint8_t slot, uint8_t reload);
int EF_Ctrl_Write_MAC_Address_Opt(uint8_t slot, uint8_t mac[6], uint8_t program);
int EF_Ctrl_Read_MAC_Address_Opt(uint8_t slot, uint8_t mac[6], uint8_t reload);

#endif /* __DRV_EF_CFG_H__ */
