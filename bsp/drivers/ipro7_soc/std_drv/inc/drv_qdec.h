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

#ifndef __DRV_QDEC_H__
#define __DRV_QDEC_H__

#include "qdec_reg.h"
#include "drv_common.h"

typedef enum {
    QDEC0_ID,    /*!< QDEC0 port define */
    QDEC1_ID,    /*!< QDEC1 port define */
    QDEC2_ID,    /*!< QDEC2 port define */
    QDEC_ID_MAX, /*!< QDEC MAX ID define */
} QDEC_ID_Type;

typedef enum {
    QDEC_SAMPLE_SINGLE_MOD,   /*!< Stop sample if rpt_rdy */
    QDEC_SAMPLE_CONTINUE_MOD, /*!< Continue sample */
} QDEC_SAMPLE_MODE_Type;

typedef enum {
    QDEC_SAMPLE_PERIOD_32US,  /*!< 32  us at clock 1MHz */
    QDEC_SAMPLE_PERIOD_64US,  /*!< 64  us at clock 1MHz */
    QDEC_SAMPLE_PERIOD_128US, /*!< 128 us at clock 1MHz */
    QDEC_SAMPLE_PERIOD_256US, /*!< 256 us at clock 1MHz */
    QDEC_SAMPLE_PERIOD_512US, /*!< 512 us at clock 1MHz */
    QDEC_SAMPLE_PERIOD_1MS,   /*!< 1   ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_2MS,   /*!< 2   ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_4MS,   /*!< 4   ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_8MS,   /*!< 8   ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_16MS,  /*!< 16  ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_32MS,  /*!< 32  ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_65MS,  /*!< 65  ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_131MS, /*!< 131 ms at clock 1MHz */
} QDEC_SAMPLE_PERIOD_Type;

typedef enum {
    QDEC_REPORT_SAMPLE_CHANGE_MOD, /*!< Count time only if sample change */
    QDEC_REPORT_TIME_MOD,          /*!< Continue time */
} QDEC_REPORT_MODE_Type;

typedef struct
{
    QDEC_SAMPLE_MODE_Type sampleMod;      /*!< Sample work mode */
    QDEC_SAMPLE_PERIOD_Type samplePeriod; /*!< Sample period time */
} QDEC_SAMPLE_Type;

typedef struct
{
    QDEC_REPORT_MODE_Type reportMod; /*!< Report work mode */
    uint16_t reportPeriod;           /*!< RPT_US report period in [us/report] = SP * RP */
} QDEC_REPORT_Type;

typedef enum {
    QDEC_ACC_STOP_SAMPLE_IF_OVERFLOW, /*!< Stop accumulate if overflow */
    QDEC_ACC_CONTINUE_ACCUMULATE,     /*!< Continue accumulate */
} QDEC_ACC_MODE_Type;

typedef struct
{
    int ledEn;   /*!< LED enable */
    int ledSwap; /*!< LED on/off polarity swap */
    uint16_t ledPeriod;  /*!< Period in us the LED is switched on prior to sampling */
} QDEC_LED_Type;

typedef struct
{
    int deglitchEn;   /*!< deglitch enable */
    uint8_t deglitchStrength; /*!< deglitch strength */
} QDEC_DEGLITCH_Type;

typedef struct
{
    QDEC_SAMPLE_Type sampleCfg;     /*!< QDEC sample config structure */
    QDEC_REPORT_Type reportCfg;     /*!< QDEC report config structure */
    QDEC_ACC_MODE_Type accMod;      /*!< QDEC acc mode config */
    QDEC_LED_Type ledCfg;           /*!< QDEC led config structure */
    QDEC_DEGLITCH_Type deglitchCfg; /*!< QDEC deglitch config structure */
} QDEC_CFG_Type;

typedef enum {
    QDEC_DIRECTION_NO_CHANGE,         /*!< Direction of last change no change */
    QDEC_DIRECTION_CLOCKWISE,         /*!< Direction of last change clockwise */
    QDEC_DIRECTION_COUNTER_CLOCKWISE, /*!< Direction of last change counter-clockwise */
    QDEC_DIRECTION_ERROR,             /*!< Direction of last change error */
} QDEC_DIRECTION_Type;

typedef enum {
    QDEC_INT_REPORT,   /*!< report interrupt */
    QDEC_INT_SAMPLE,   /*!< sample interrupt */
    QDEC_INT_ERROR,    /*!< error interrupt */
    QDEC_INT_OVERFLOW, /*!< ACC1 and ACC2 overflow interrupt */
    QDEC_INT_ALL,      /*!< interrupt max num */
} QDEC_INT_Type;

void QDEC_Init(QDEC_ID_Type qdecId, QDEC_CFG_Type *qdecCfg);
void QDEC_DeInit(QDEC_ID_Type qdecId);
void QDEC_Enable(QDEC_ID_Type qdecId);
void QDEC_Disable(QDEC_ID_Type qdecId);
void QDEC_SetIntMask(QDEC_ID_Type qdecId, QDEC_INT_Type intType, int intMask);
int QDEC_GetIntMask(QDEC_ID_Type qdecId, QDEC_INT_Type intType);
int QDEC_Get_Int_Status(QDEC_ID_Type qdecId, QDEC_INT_Type intType);
void QDEC_Clr_Int_Status(QDEC_ID_Type qdecId, QDEC_INT_Type intType);
QDEC_DIRECTION_Type QDEC_Get_Sample_Direction(QDEC_ID_Type qdecId);
uint8_t QDEC_Get_Err_Cnt(QDEC_ID_Type qdecId);
uint16_t QDEC_Get_Sample_Val(QDEC_ID_Type qdecId);
void QDEC_IntHandler(QDEC_ID_Type qdecId, QDEC_INT_Type intType);

#endif /* __DRV_QDEC_H__ */
