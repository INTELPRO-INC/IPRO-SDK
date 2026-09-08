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

#ifndef __DRV_AON_H__
#define __DRV_AON_H__

#include "aon_reg.h"
#include "glb_reg.h"
#include "hbn_reg.h"
#include "pds_reg.h"
#include "drv_ef_ctrl.h"
#include "drv_common.h"

int AON_Power_On_MBG(void);
int AON_Power_Off_MBG(void);
int AON_Power_On_XTAL(void);
int AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut);
uint8_t AON_Get_Xtal_CapCode(void);
int AON_Set_Xtal_CapCode_Extra(uint8_t extra);
int AON_Power_Off_XTAL(void);
int AON_Power_On_BG(void);
int AON_Power_Off_BG(void);
int AON_Power_On_LDO11_SOC(void);
int AON_Power_Off_LDO11_SOC(void);
int AON_Power_On_SFReg(void);
int AON_Power_Off_SFReg(void);
int AON_LowPower_Enter_PDS0(void);
int AON_LowPower_Exit_PDS0(void);
int AON_Set_LDO11_SOC_Sstart_Delay(uint8_t delay);
int AON_Set_DCDC14_Top_0(uint8_t voutSel, uint8_t vpfm);
int AON_Set_LDO14_Vout_Sel(uint8_t sel);
int AON_Wait_DCDC_Ready(uint32_t max_polls);
int AON_Trim_Ldo11socVoutTrim(void);
int AON_Trim_Ldo14VoutTrim(void);
int AON_Trim_Dcdc14VoutTrim(void);

#endif /* __DRV_AON_H__ */
