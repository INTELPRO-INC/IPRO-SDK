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

#ifndef __DRV_DAC_H__
#define __DRV_DAC_H__

#include "aon_reg.h"
#include "glb_reg.h"
#include "gpip_reg.h"
#include "drv_common.h"

typedef enum {
    GLB_DAC_REF_SEL_INTERNAL, /*!< DAC reference select internal */
    GLB_DAC_REF_SEL_EXTERNAL, /*!< DAC reference select external */
} GLB_DAC_Ref_Sel_Type;

typedef enum {
    GLB_DAC_Output_Volt_0P2_1,       /*!< DAC output voltage is 0.2-1V */
    GLB_DAC_Output_Volt_0P225_1P425, /*!< DAC output voltage is 0.225-1.425V */
    GLB_DAC_Output_Volt_RESEVED,     /*!< DAC output voltage is 0.225-1.425V */
    GLB_DAC_Output_Volt_0P2_1P8,     /*!< DAC output voltage is 0.2-1.8V */
} GLB_DAC_Output_Volt_Range_Type;

typedef enum {
    GLB_DAC_CHAN0,    /*!< DAC channel 0 */
    GLB_DAC_CHAN1,    /*!< DAC channel 1 */
    GLB_DAC_CHAN2,    /*!< DAC channel 2 */
    GLB_DAC_CHAN3,    /*!< DAC channel 3 */
    GLB_DAC_CHAN4,    /*!< DAC channel 4 */
    GLB_DAC_CHAN5,    /*!< DAC channel 5 */
    GLB_DAC_CHAN6,    /*!< DAC channel 6 */
    GLB_DAC_CHAN7,    /*!< DAC channel 7 */
    GLB_DAC_CHAN_ALL, /*!< DAC channel all */
} GLB_DAC_Chan_Type;

typedef struct
{
    int chanEn;                      /*!< Enable this channel or not */
    int outputEn;                    /*!< Output this channel result to PAD */
    GLB_DAC_Chan_Type outMux;                /*!< DAC output mux,NOT implement yet,DAC use fixed GPIO9 and GPIO10 */
    GLB_DAC_Output_Volt_Range_Type outRange; /*!< DAC output voltage range */
} GLB_DAC_Chan_Cfg_Type;

typedef struct
{
    GLB_DAC_Ref_Sel_Type refSel; /*!< DAC reference voltage select */
    int resetChanA;      /*!< Reset DAC channel A */
    int resetChanB;      /*!< Reset DAC channel B */
} GLB_DAC_Cfg_Type;

typedef enum {
    GPIP_DAC_ChanB_SRC_REG,             /*!< select Reg as source of DAC channel B */
    GPIP_DAC_ChanB_SRC_DMA,             /*!< select DMA as source of DAC channel B */
    GPIP_DAC_ChanB_SRC_DMA_WITH_FILTER, /*!< select DMA with Filter as source of DAC channel B */
    GPIP_DAC_ChanB_SRC_SIN_GEN,         /*!< select Sin Gen as source of DAC channel B */
    GPIP_DAC_ChanB_SRC_A,               /*!< select channel A as source of DAC channel B */
    GPIP_DAC_ChanB_SRC_INVERSE_A,       /*!< select inverse of channel A as source of DAC channel B */
} GPIP_DAC_ChanB_SRC_Type;

typedef enum {
    GPIP_DAC_ChanA_SRC_REG,             /*!< select Reg as source of DAC channel A */
    GPIP_DAC_ChanA_SRC_DMA,             /*!< select DMA as source of DAC channel A */
    GPIP_DAC_ChanA_SRC_DMA_WITH_FILTER, /*!< select DMA with Filter as source of DAC channel A */
    GPIP_DAC_ChanA_SRC_SIN_GEN,         /*!< select Sin Gen as source of DAC channel A */
} GPIP_DAC_ChanA_SRC_Type;

typedef enum {
    DAC_CLK_DIV_16,      /*!< ADC clock:on 32M clock is 2M */
    DAC_CLK_DIV_32,      /*!< ADC clock:on 32M clock is 1M */
    DAC_CLK_DIV_RESERVE, /*!< reserved */
    DAC_CLK_DIV_64,      /*!< ADC clock:on 32M clock is 0.5M */
    DAC_CLK_DIV_1,       /*!< ADC clock:on 32M clock is 32M */
} DAC_CLK_Type;

typedef enum {
    GPIP_DAC_DMA_FORMAT_0, /*!< {A0},{A1},{A2},... */
    GPIP_DAC_DMA_FORMAT_1, /*!< {B0,A0},{B1,A1},{B2,A2},... */
    GPIP_DAC_DMA_FORMAT_2, /*!< {A1,A0},{A3,A2},{A5,A4},... */
} GPIP_DAC_DMA_TX_FORMAT_Type;

typedef struct
{
    GLB_DAC_Ref_Sel_Type refSel;        /*!< DAC reference voltage select */
    int resetChanA;             /*!< Reset DAC channel A */
    int resetChanB;             /*!< Reset DAC channel B */
    DAC_CLK_Type div;                   /*!< DAC clock div */
    int dmaEn;                  /*!< DAC DMA transfer enable */
    GPIP_DAC_DMA_TX_FORMAT_Type dmaFmt; /*!< DAC DMA TX format selection */
} GLB_GPIP_DAC_Cfg_Type;

typedef struct
{
    int chanCovtEn;      /*!< Enable this channel conversion or not */
    int outputEn;        /*!< Output this channel result to PAD */
    int chanEn;          /*!< Enable this channel or not */
    GPIP_DAC_ChanA_SRC_Type src; /*!< DAC channel A source */
} GLB_GPIP_DAC_ChanA_Cfg_Type;

typedef struct
{
    int chanCovtEn;      /*!< Enable this channel conversion or not */
    int outputEn;        /*!< Output this channel result to PAD */
    int chanEn;          /*!< Enable this channel or not */
    GPIP_DAC_ChanB_SRC_Type src; /*!< DAC channel B source */
} GLB_GPIP_DAC_ChanB_Cfg_Type;

void GLB_DAC_Init(GLB_DAC_Cfg_Type *cfg);
void GLB_DAC_Set_ChanA_Config(GLB_DAC_Chan_Cfg_Type *cfg);
void GLB_DAC_Set_ChanB_Config(GLB_DAC_Chan_Cfg_Type *cfg);
void GPIP_Set_DAC_ChanB_SRC_SEL(GPIP_DAC_ChanB_SRC_Type src);
void GPIP_Set_DAC_ChanA_SRC_SEL(GPIP_DAC_ChanA_SRC_Type src);
void GPIP_DAC_ChanB_Enable(void);
void GPIP_DAC_ChanB_Disable(void);
void GPIP_DAC_ChanA_Enable(void);
void GPIP_DAC_ChanA_Disable(void);
void GPIP_Set_DAC_DMA_TX_FORMAT_SEL(GPIP_DAC_DMA_TX_FORMAT_Type fmt);
void GPIP_Set_DAC_DMA_TX_Enable(void);
void GPIP_Set_DAC_DMA_TX_Disable(void);
void GPIP_DAC_DMA_WriteData(uint32_t data);
int GLB_GPIP_DAC_Init(GLB_GPIP_DAC_Cfg_Type *cfg);
void GLB_GPIP_DAC_Set_ChanA_Config(GLB_GPIP_DAC_ChanA_Cfg_Type *cfg);
void GLB_GPIP_DAC_Set_ChanB_Config(GLB_GPIP_DAC_ChanB_Cfg_Type *cfg);
void GLB_DAC_Set_ChanA_Value(uint16_t val);
void GLB_DAC_Set_ChanB_Value(uint16_t val);

#endif /* __DRV_DAC_H__ */
