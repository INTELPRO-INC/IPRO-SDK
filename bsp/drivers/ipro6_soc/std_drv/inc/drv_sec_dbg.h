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


#ifndef __DRV_SEC_DBG_H__
#define __DRV_SEC_DBG_H__

#include "sec_dbg_reg.h"
#include "drv_common.h"

typedef enum {
    SEC_DBG_DEBUG_OPEN_MODE,       /*!< debug open mode */
    SEC_DBG_DEBUG_PASSWORD_MODE,   /*!< debug password mode */
    SEC_DBG_DEBUG_CLOSE_MODE,      /*!< debug close mode */
} SEC_DBG_STATE_Type;

#define IS_SEC_DBG_STATE_TYPE(type) (((type) == SEC_DBG_DEBUG_OPEN_MODE) || \
                                     ((type) == SEC_DBG_DEBUG_PASSWORD_MODE) || \
                                     ((type) == SEC_DBG_DEBUG_CLOSE_MODE))

void Sec_Dbg_Read_Chip_ID(uint8_t id[8]);
void Sec_Dbg_Set_Dbg_Pwd(const uint32_t pwd[4]);
SEC_DBG_STATE_Type Sec_Dbg_Read_Dbg_State(void);
uint32_t Sec_Dbg_Read_Dbg_Mode(void);
uint32_t Sec_Dbg_Read_Dbg_Enable(void);
void Sec_Dbg_Set_Dbg_Trigger(void);
uint32_t Sec_Dbg_Wait_Ready(void);

#endif /* __DRV_SEC_DBG_H__ */
