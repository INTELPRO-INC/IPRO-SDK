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

#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#include "glb_reg.h"
#include "gpip_reg.h"
#include "drv_common.h"

typedef enum {
    ADC_CHAN0,          /*!< ADC channel 0 */
    ADC_CHAN1,          /*!< ADC channel 1 */
    ADC_CHAN2,          /*!< ADC channel 2 */
    ADC_CHAN3,          /*!< ADC channel 3 */
    ADC_CHAN4,          /*!< ADC channel 4 */
    ADC_CHAN5,          /*!< ADC channel 5 */
    ADC_CHAN6,          /*!< ADC channel 6 */
    ADC_CHAN7,          /*!< ADC channel 7 */
    ADC_CHAN8,          /*!< ADC channel 8 */
    ADC_CHAN9,          /*!< ADC channel 9 */
    ADC_CHAN10,         /*!< ADC channel 10 */
    ADC_CHAN11,         /*!< ADC channel 11 */
    ADC_CHAN_DAC_OUTA,  /*!< DACA, ADC channel 12 */
    ADC_CHAN_DAC_OUTB,  /*!< DACB, ADC channel 13 */
    ADC_CHAN_TSEN_P,    /*!< TSenp, ADC channel 14 */
    ADC_CHAN_TSEN_N,    /*!< TSenn, ADC channel 15 */
    ADC_CHAN_VREF,      /*!< Vref, ADC channel 16 */
    ADC_CHAN_DCTEST,    /*!< DCTest, ADC channel 17 */
    ADC_CHAN_VABT_HALF, /*!< VBAT/2, ADC channel 18 */
    ADC_CHAN_SENP3,     /*!< SenVP3, ADC channel 19 */
    ADC_CHAN_SENP2,     /*!< SenVP2, ADC channel 20 */
    ADC_CHAN_SENP1,     /*!< SenVP1, ADC channel 21 */
    ADC_CHAN_SENP0,     /*!< SenVP0, ADC channel 22 */
    ADC_CHAN_GND,       /*!< GND, ADC channel 23 */
} ADC_Chan_Type;

typedef enum {
    ADC_V18_SEL_1P62V, /*!< V18 select 1.62V */
    ADC_V18_SEL_1P72V, /*!< V18 select 1.72V */
    ADC_V18_SEL_1P82V, /*!< V18 select 1.82V */
    ADC_V18_SEL_1P92V, /*!< V18 select 1.92V */
} ADC_V18_SEL_Type;

typedef enum {
    ADC_V11_SEL_1P0V,  /*!< V11 select 1.0V */
    ADC_V11_SEL_1P1V,  /*!< V11 select 1.1V */
    ADC_V11_SEL_1P18V, /*!< V11 select 1.18V */
    ADC_V11_SEL_1P26V, /*!< V11 select 1.26V */
} ADC_V11_SEL_Type;

typedef enum {
    ADC_CLK_DIV_1,  /*!< ADC clock:on 32M clock is 32M */
    ADC_CLK_DIV_4,  /*!< ADC clock:on 32M clock is 8M */
    ADC_CLK_DIV_8,  /*!< ADC clock:on 32M clock is 4M */
    ADC_CLK_DIV_12, /*!< ADC clock:on 32M clock is 2.666M */
    ADC_CLK_DIV_16, /*!< ADC clock:on 32M clock is 2M */
    ADC_CLK_DIV_20, /*!< ADC clock:on 32M clock is 1.6M */
    ADC_CLK_DIV_24, /*!< ADC clock:on 32M clock is 1.333M */
    ADC_CLK_DIV_32, /*!< ADC clock:on 32M clock is 1M */
} ADC_CLK_Type;

typedef enum {
    ADC_DELAY_SEL_0, /*!< Select delay 0 */
    ADC_DELAY_SEL_1, /*!< Select delay 1 */
    ADC_DELAY_SEL_2, /*!< Select delay 2 */
    ADC_DELAY_SEL_3, /*!< Select delay 3 */
    ADC_DELAY_SEL_4, /*!< Select delay 4, not recommend */
    ADC_DELAY_SEL_5, /*!< Select delay 5, not recommend */
    ADC_DELAY_SEL_6, /*!< Select delay 6, not recommend */
    ADC_DELAY_SEL_7, /*!< Select delay 7, not recommend */
} ADC_DELAY_SEL_Type;

typedef enum {
    ADC_PGA_GAIN_NONE, /*!< No PGA gain */
    ADC_PGA_GAIN_1,    /*!< PGA gain 1 */
    ADC_PGA_GAIN_2,    /*!< PGA gain 2 */
    ADC_PGA_GAIN_4,    /*!< PGA gain 4 */
    ADC_PGA_GAIN_8,    /*!< PGA gain 8 */
    ADC_PGA_GAIN_16,   /*!< PGA gain 16 */
    ADC_PGA_GAIN_32,   /*!< PGA gain 32 */
} ADC_PGA_GAIN_Type;

typedef enum {
    ADC_BIAS_SEL_MAIN_BANDGAP, /*!< ADC current from main bandgap */
    ADC_BIAS_SEL_AON_BANDGAP,  /*!< ADC current from aon bandgap for HBN mode */
} ADC_BIAS_SEL_Type;

typedef enum {
    ADC_CHOP_MOD_ALL_OFF,       /*!< all off */
    ADC_CHOP_MOD_AZ_ON,         /*!< Vref AZ on */
    ADC_CHOP_MOD_AZ_PGA_ON,     /*!< Vref AZ and PGA chop on */
    ADC_CHOP_MOD_AZ_PGA_RPC_ON, /*!< Vref AZ and PGA chop+RPC on */
} ADC_CHOP_MOD_Type;

typedef enum {
    ADC_PGA_VCM_1V,   /*!< ADC VCM=1V */
    ADC_PGA_VCM_1P2V, /*!< ADC VCM=1.2V */
    ADC_PGA_VCM_1P4V, /*!< ADC VCM=1.4V */
    ADC_PGA_VCM_1P6V, /*!< ADC VCM=1.6V */
} ADC_PGA_VCM_Type;

typedef enum {
    ADC_TSEN_MOD_INTERNAL_DIODE, /*!< Internal diode mode */
    ADC_TSEN_MOD_EXTERNAL_DIODE, /*!< External diode mode */
} ADC_TSEN_MOD_Type;

typedef enum {
    ADC_VREF_3P2V, /*!< ADC select 3.2V as reference voltage */
    ADC_VREF_2P0V, /*!< ADC select 2V as reference voltage */
} ADC_VREF_Type;

typedef enum {
    ADC_INPUT_SINGLE_END, /*!< ADC signal is single end */
    ADC_INPUT_DIFF,       /*!< ADC signal is differential */
} ADC_SIG_INPUT_Type;

typedef enum {
    ADC_DATA_WIDTH_12,                  /*!< ADC 12 bits */
    ADC_DATA_WIDTH_14_WITH_16_AVERAGE,  /*!< ADC 14 bits,and the value is average of 16 converts */
    ADC_DATA_WIDTH_14_WITH_64_AVERAGE,  /*!< ADC 14 bits,and the value is average of 64 converts */
    ADC_DATA_WIDTH_16_WITH_128_AVERAGE, /*!< ADC 16 bits,and the value is average of 128 converts */
    ADC_DATA_WIDTH_16_WITH_256_AVERAGE, /*!< ADC 16 bits,and the value is average of 256 converts */
} ADC_Data_Width_Type;

typedef enum {
    ADC_MICBOOST_DB_16DB, /*!< MIC boost 16db */
    ADC_MICBOOST_DB_32DB, /*!< MIC boost 32db */
} ADC_MICBOOST_DB_Type;

typedef enum {
    ADC_PGA2_GAIN_0DB,  /*!< MIC pga2 gain 0db */
    ADC_PGA2_GAIN_6DB,  /*!< MIC pga2 gain 6db */
    ADC_PGA2_GAIN_N6DB, /*!< MIC pga2 gain -6db */
    ADC_PGA2_GAIN_12DB, /*!< MIC pga2 gain 12db */
} ADC_PGA2_GAIN_Type;

typedef enum {
    ADC_MIC_MODE_SINGLE, /*!< MIC single mode */
    ADC_MIC_MODE_DIFF,   /*!< MIC diff mode */
} ADC_MIC_MODE_Type;

typedef struct
{
    ADC_MICBOOST_DB_Type micboostDb; /*!< MIC boost db */
    ADC_PGA2_GAIN_Type micPga2Gain;  /*!< MIC pga2 gain */
    ADC_MIC_MODE_Type mic1Mode;      /*!< MIC1 single or diff */
    ADC_MIC_MODE_Type mic2Mode;      /*!< MIC2 single or diff */
    int dwaEn;               /*!< Improve dynamic performance */
    int micboostBypassEn;    /*!< MIC boost amp bypass enable or disable */
    int micPgaEn;            /*!< MIC pga enable or disable */
    int micBiasEn;           /*!< MIC bias enable or disable */
} ADC_MIC_Type;

typedef struct
{
    ADC_V18_SEL_Type v18Sel;      /*!< ADC 1.8V select */
    ADC_V11_SEL_Type v11Sel;      /*!< ADC 1.1V select */
    ADC_CLK_Type clkDiv;          /*!< Clock divider */
    ADC_PGA_GAIN_Type gain1;      /*!< PGA gain 1 */
    ADC_PGA_GAIN_Type gain2;      /*!< PGA gain 2 */
    ADC_CHOP_MOD_Type chopMode;   /*!< ADC chop mode select */
    ADC_BIAS_SEL_Type biasSel;    /*!< ADC current form main bandgap or aon bandgap */
    ADC_PGA_VCM_Type vcm;         /*!< ADC VCM value */
    ADC_VREF_Type vref;           /*!< ADC voltage reference */
    ADC_SIG_INPUT_Type inputMode; /*!< ADC input signal type */
    ADC_Data_Width_Type resWidth; /*!< ADC resolution and oversample rate */
    int offsetCalibEn;    /*!< Offset calibration enable */
    int16_t offsetCalibVal;       /*!< Offset calibration value */
} ADC_CFG_Type;

typedef struct
{
    int8_t posChan; /*!< Positive channel */
    int8_t negChan; /*!< Negative channel */
    uint16_t value; /*!< ADC value */
    float volt;     /*!< ADC voltage result */
} ADC_Result_Type;

typedef enum {
    ADC_FIFO_THRESHOLD_1,  /*!< ADC FIFO threshold is 1 */
    ADC_FIFO_THRESHOLD_4,  /*!< ADC FIFO threshold is 4 */
    ADC_FIFO_THRESHOLD_8,  /*!< ADC FIFO threshold is 8 */
    ADC_FIFO_THRESHOLD_16, /*!< ADC FIFO threshold is 16 */
} ADC_FIFO_Threshold_Type;

typedef enum {
    ADC_INT_POS_SATURATION, /*!< ADC positive channel saturation */
    ADC_INT_NEG_SATURATION, /*!< ADC negative channel saturation */
    ADC_INT_FIFO_UNDERRUN,  /*!< ADC FIFO underrun interrupt */
    ADC_INT_FIFO_OVERRUN,   /*!< ADC FIFO overrun interrupt */
    ADC_INT_ADC_READY,      /*!< ADC data ready interrupt */
    ADC_INT_FIFO_READY,     /*!< ADC FIFO count upper to threhold interrupt */
    ADC_INT_ALL,            /*!< ADC all the interrupt */
} ADC_INT_Type;

typedef struct
{
    ADC_FIFO_Threshold_Type fifoThreshold; /*!< ADC FIFO threshold */
    int dmaEn;                     /*!< ADC DMA enable */
} ADC_FIFO_Cfg_Type;

typedef struct
{
    int adcGainCoeffEnable; /*!< ADC_Gain_Coeff enable */
    uint16_t adcgainCoeffVal;       /*!< ADC_Gain_Coeff value */
    float coe;                      /*!< ADC_Gain_Coeff result */
} ADC_Gain_Coeff_Type;

void ADC_Vbat_Enable(void);
void ADC_Vbat_Disable(void);
void ADC_Reset(void);
void ADC_Enable(void);
void ADC_Disable(void);
void ADC_Init(ADC_CFG_Type *cfg);
void ADC_Channel_Config(uint8_t posCh, uint8_t negCh, int contEn);
void ADC_Scan_Channel_Config(uint8_t posChList[], uint8_t negChList[], uint8_t scanLength, int contEn);
void ADC_Start(void);
void ADC_Stop(void);
void ADC_FIFO_Cfg(ADC_FIFO_Cfg_Type *fifoCfg);
uint8_t ADC_Get_FIFO_Count(void);
int ADC_FIFO_Is_Empty(void);
int ADC_FIFO_Is_Full(void);
void ADC_FIFO_Clear(void);
uint32_t ADC_Read_FIFO(void);
void ADC_Parse_Result(uint32_t *orgVal, uint32_t len, ADC_Result_Type *result);
void ADC_IntClr(ADC_INT_Type intType);
int ADC_IntGetMask(ADC_INT_Type intType);
int ADC_GetIntStatus(ADC_INT_Type intType);
void ADC_IntMask(ADC_INT_Type intType, int intMask);
void ADC_SET_TSVBE_LOW(void);
void ADC_SET_TSVBE_HIGH(void);
void ADC_Tsen_Init(ADC_TSEN_MOD_Type tsenMod);
void ADC_Tsen_Enable(void);
void ADC_Tsen_Disable(void);
void ADC_PGA_Config(uint8_t pga_vcmi_enable, uint8_t pga_os_cal);
int ADC_Mic_Init(ADC_MIC_Type *adc_mic_config);
void ADC_MIC_Bias_Disable(void);
void ADC_MIC_Bias_Enable(void);
int ADC_Trim_TSEN(uint16_t *tsen_offset);
int ADC_Gain_Trim(void);
uint32_t ADC_Cal_Reg_Coeff_Value(uint32_t raw_reg);
float TSEN_Get_Temp(uint32_t tsen_offset);

#endif /* __DRV_ADC_H__ */
