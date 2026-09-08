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

#ifndef __DRV_L1C_H__
#define __DRV_L1C_H__

#include "l1c_reg.h"
#include "drv_common.h"

typedef struct
{
    int wrapDis;  /*!< wrap disable */
    int bypassEn; /*!< bypass cache enable */
    uint8_t wayDis;       /*!< Disable part of cache ways & used as ITCM */
    int cntEn;    /*!< l1c count enable */
} L1C_CACHE_Cfg_Type;

typedef enum {
    L1C_BMX_ARB_FIX,         /*!< 0->fix */
    L1C_BMX_ARB_ROUND_ROBIN, /*!< 2->round-robin */
    L1C_BMX_ARB_RANDOM,      /*!< 3->random */
} L1C_BMX_ARB_Type;

typedef struct
{
    uint8_t timeoutEn;       /*!< Bus timeout enable: detect slave no reaponse in 1024 cycles */
    int errEn;       /*!< Bus error response enable */
    L1C_BMX_ARB_Type arbMod; /*!< 0->fix, 2->round-robin, 3->random */
} L1C_BMX_Cfg_Type;

typedef enum {
    L1C_BMX_BUS_ERR_TRUSTZONE_DECODE, /*!< Bus trustzone decode error */
    L1C_BMX_BUS_ERR_ADDR_DECODE,      /*!< Bus addr decode error */
} L1C_BMX_BUS_ERR_Type;

typedef enum {
    L1C_BMX_ERR_INT_ERR, /*!< L1C BMX bus err interrupt */
    L1C_BMX_ERR_INT_ALL, /*!< L1C BMX bus err interrupt max num */
} L1C_BMX_ERR_INT_Type;

typedef enum {
    L1C_BMX_TO_INT_TIMEOUT, /*!< L1C_BMX timeout interrupt */
    L1C_BMX_TO_INT_ALL,     /*!< L1C_BMX timeout interrupt max num */
} L1C_BMX_TO_INT_Type;

#define L1C_WAY_DISABLE_NONE  0x00
#define L1C_WAY_DISABLE_ONE   0x01
#define L1C_WAY_DISABLE_TWO   0x03
#define L1C_WAY_DISABLE_THREE 0x07
#define L1C_WAY_DISABLE_ALL   0x0F
#define L1C_WAY_DISABLE_USER  0xFF

/*NP config address */
#define L1C_CONF_REG        (L1C_BASE + 0x00)
#define L1C_HIT_CNT_LSB_REG (L1C_BASE + 0x04)
#define L1C_HIT_CNT_MSB_REG (L1C_BASE + 0x08)
#define L1C_MISS_CNT_REG    (L1C_BASE + 0x0C)
/* Get miss and hit count */
#define L1C_Get_Miss_Cnt()    readl(L1C_MISS_CNT_REG)
#define L1C_Get_Hit_Cnt_LSB() readl(L1C_HIT_CNT_LSB_REG)
#define L1C_Get_Hit_Cnt_MSB() readl(L1C_HIT_CNT_MSB_REG)

int L1C_Cache_Enable_Set(uint8_t wayDisable);
void L1C_Cache_Write_Set(int wtEn, int wbEn, int waEn);
int L1C_Cache_Flush(uint8_t wayDisable);
int L1C_Cache_Flush_Ext(void);
void L1C_Cache_Hit_Count_Get(uint32_t *hitCountLow, uint32_t *hitCountHigh);
uint32_t L1C_Cache_Miss_Count_Get(void);
void L1C_Cache_Read_Disable(void);
/*----------*/
int L1C_Set_Wrap(int wrap);
int L1C_Set_Way_Disable(uint8_t disableVal);
uint8_t L1C_Get_Way_Disable(void);
int L1C_IROM_2T_Access_Set(uint8_t enable);
/*----------*/
int L1C_BMX_Init(L1C_BMX_Cfg_Type *l1cBmxCfg);
int L1C_BMX_Addr_Monitor_Enable(void);
int L1C_BMX_Addr_Monitor_Disable(void);
int L1C_BMX_BusErrResponse_Enable(void);
int L1C_BMX_BusErrResponse_Disable(void);
int L1C_BMX_Get_Status(L1C_BMX_BUS_ERR_Type errType);
uint32_t L1C_BMX_Get_Err_Addr(void);
#endif /* __DRV_L1C_H__ */
