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

#include "drv_ef_ctrl.h"
#include "ef_ctrl_reg.h"
#include "drv_common.h"

uint8_t EF_Ctrl_Get_Trim_Parity(uint32_t val, uint8_t len);
uint32_t EF_Ctrl_Get_Common_Trim_List(const Efuse_Common_Trim_Cfg **trim_list);
void EF_Ctrl_Read_Common_Trim(char *name, Efuse_Common_Trim_Type *trim, uint8_t reload);
void EF_Ctrl_Write_Common_Trim(char *name, uint8_t trim_en, uint32_t trim_value);
/* Shadow-only write — sets en/parity/value bits in shadow regs without
 * triggering eFuse program. Use for split-storage trims (en/parity shared,
 * value across multiple cfg entries) where parity must be over the full
 * composed value, not each half. Caller handles parity computation and
 * calls EF_Ctrl_Program_Efuse_0() once after all halves are written.
 *
 *   trim_en=0/parity=0 skips that bit (already-set or shared-with-other-half).
 *   trim_value bits beyond value_len are ignored (masked).
 */
void EF_Ctrl_Write_Common_Trim_Shadow(char *name, uint8_t trim_en,
                                       uint8_t parity, uint32_t trim_value);
uint8_t EF_Ctrl_Is_MAC_Address_Slot_Empty(uint8_t slot, uint8_t reload);
int EF_Ctrl_Write_MAC_Address_Opt(uint8_t slot, uint8_t mac[8], uint8_t program);
int EF_Ctrl_Read_MAC_Address_Opt(uint8_t slot, uint8_t mac[8], uint8_t reload);

#endif /* __DRV_EF_CFG_H__ */
