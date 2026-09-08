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

#ifndef __DRV_KYS_H__
#define __DRV_KYS_H__

#include "kys_reg.h"
#include "drv_common.h"

typedef enum {
    KYS_KEYCODE_0, /*!< KYS keycode 0 */
    KYS_KEYCODE_1, /*!< KYS keycode 1 */
    KYS_KEYCODE_2, /*!< KYS keycode 2 */
    KYS_KEYCODE_3, /*!< KYS keycode 3 */
    KYS_KEYCODE_ALL,
} KYS_Keycode_Type;

typedef struct
{
    uint8_t col;            /*!< Col of keyboard,max:20 */
    uint8_t row;            /*!< Row of keyboard,max:8 */
    uint8_t idleDuration;   /*!< Idle duration between column scans */
    int ghostEn;    /*!< Enable or disable ghost key event detection */
    int deglitchEn; /*!< Enable or disable deglitch function */
    uint8_t deglitchCnt;    /*!< Deglitch count */
} KYS_CFG_Type;

int KYS_Init(KYS_CFG_Type *kysCfg);
int KYS_Enable(void);
int KYS_Disable(void);
int KYS_IntMask(int intMask);
int KYS_IntClear(void);
uint8_t KYS_GetIntStatus(void);
uint8_t KYS_GetKeycode(KYS_Keycode_Type keycode, uint8_t *col, uint8_t *row);
#endif /* __DRV_KYS_H__ */
