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

/*----------*/
int AON_Power_On_MBG(void);
int AON_Power_Off_MBG(void);
/*----------*/
int AON_Power_On_XTAL(void);
int AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut);
uint8_t AON_Get_Xtal_CapCode(void);
int AON_Power_Off_XTAL(void);
/*----------*/
int AON_Power_On_BG(void);
int AON_Power_Off_BG(void);
/*----------*/
int AON_Power_On_LDO15_RF(void);
int AON_Power_Off_LDO15_RF(void);
/*----------*/
int AON_Power_On_SFReg(void);
int AON_Power_Off_SFReg(void);
/*----------*/
int AON_Power_On_MicBias(void);
int AON_Power_Off_MicBias(void);
/*----------*/
int AON_LowPower_Enter_PDS0(void);
int AON_LowPower_Exit_PDS0(void);
/*----------*/
int AON_Trim_DcdcDis(void);
int AON_Trim_DcdcVoutSel(void);
int AON_Trim_DcdcVoutTrim(void);
int AON_Trim_Ldo11socVoutTrim(void);
int AON_Trim_Usb20RcalCode(void);
/*----------*/
int AON_Output_Float_LDO15_RF(void);
int AON_Output_Pulldown_LDO15_RF(void);
int AON_Output_LDO15_RF_Ctrl_by_Pd(void);
int AON_Output_Pulldown_DCDC18(void);
int AON_Output_Float_DCDC18(void);
/*----------*/

#endif /* __DRV_AON_H__ */
