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


#ifndef __WB03_ACOMP_H__
#define __WB03_ACOMP_H__

#include "aon_reg.h"
#include "drv_common.h"

typedef enum {
    AON_ACOMP0_ID, /*!< Analog compare 0 */
    AON_ACOMP1_ID, /*!< Analog compare 1 */
} AON_ACOMP_ID_Type;

typedef enum {
    AON_ACOMP_SCALING_FACTOR_0P25 = 0x00, /*!< Analog compare level scaling factor 0.25 */
    AON_ACOMP_SCALING_FACTOR_0P5 = 0x10,  /*!< Analog compare level scaling factor 0.5 */
    AON_ACOMP_SCALING_FACTOR_0P75 = 0x20, /*!< Analog compare level scaling factor 0.75 */
    AON_ACOMP_SCALING_FACTOR_1 = 0x30,    /*!< Analog compare level scaling factor 1 */
} AON_ACOMP_Scaling_Factor_Type;

typedef enum {
    AON_ACOMP_CHAN_ADC0,                   /*!< Analog compare channel,ADC input channel 0 */
    AON_ACOMP_CHAN_ADC1,                   /*!< Analog compare channel,ADC input channel 1 */
    AON_ACOMP_CHAN_ADC2,                   /*!< Analog compare channel,ADC input channel 2 */
    AON_ACOMP_CHAN_ADC3,                   /*!< Analog compare channel,ADC input channel 3 */
    AON_ACOMP_CHAN_ADC4,                   /*!< Analog compare channel,ADC input channel 4 */
    AON_ACOMP_CHAN_ADC5,                   /*!< Analog compare channel,ADC input channel 5 */
    AON_ACOMP_CHAN_ADC6,                   /*!< Analog compare channel,ADC input channel 6 */
    AON_ACOMP_CHAN_ADC7,                   /*!< Analog compare channel,ADC input channel 7 */
    AON_ACOMP_CHAN_DACA,                   /*!< Analog compare channel,DAC output channel A */
    AON_ACOMP_CHAN_DACB,                   /*!< Analog compare channel,DAC output channel B */
    AON_ACOMP_CHAN_VREF_1P25V,             /*!< Analog compare channel,1.25V ref voltage */
    AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_1, /*!< Analog compare channel,first,VIO * scaling factor */
    AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_2, /*!< Analog compare channel,second,VIO * scaling factor */
    AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_3, /*!< Analog compare channel,third,VIO * scaling factor */
    AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_4, /*!< Analog compare channel,forth,VIO * scaling factor */
    AON_ACOMP_CHAN_VSS,                    /*!< Analog compare channel,vss */
} AON_ACOMP_Chan_Type;

typedef enum {
    AON_ACOMP_BIAS_POWER_MODE1, /*!< Analog compare power mode 1,slow response mode */
    AON_ACOMP_BIAS_POWER_MODE2, /*!< Analog compare power mode 2,medium response mode */
    AON_ACOMP_BIAS_POWER_MODE3, /*!< Analog compare power mode 3,fast response mode */
    AON_ACOMP_BIAS_POWER_NONE,  /*!< Analog compare power mode none */
} AON_ACOMP_Bias_Prog_Type;

typedef enum {
    AON_ACOMP_HYSTERESIS_VOLT_NONE, /*!< Analog compare hysteresis voltage none */
    AON_ACOMP_HYSTERESIS_VOLT_10MV, /*!< Analog compare hysteresis voltage 10mv */
    AON_ACOMP_HYSTERESIS_VOLT_20MV, /*!< Analog compare hysteresis voltage 20mv */
    AON_ACOMP_HYSTERESIS_VOLT_30MV, /*!< Analog compare hysteresis voltage 30mv */
    AON_ACOMP_HYSTERESIS_VOLT_40MV, /*!< Analog compare hysteresis voltage 40mv */
    AON_ACOMP_HYSTERESIS_VOLT_50MV, /*!< Analog compare hysteresis voltage 50mv */
    AON_ACOMP_HYSTERESIS_VOLT_60MV, /*!< Analog compare hysteresis voltage 60mv */
    AON_ACOMP_HYSTERESIS_VOLT_70MV, /*!< Analog compare hysteresis voltage 70mv */
} AON_ACOMP_Hysteresis_Volt_Type;

typedef struct
{
    int muxEn;                                /*!< ACOMP mux enable */
    uint8_t posChanSel;                               /*!< ACOMP positive channel select */
    uint8_t negChanSel;                               /*!< ACOMP negtive channel select */
    uint8_t vioSel;                                   /*!< ACOMP VIO voltage select, (vioSel/66)*avdd33 */
    AON_ACOMP_Scaling_Factor_Type scalingFactor;      /*!< ACOMP scaling factor select factor */
    AON_ACOMP_Bias_Prog_Type biasProg;                /*!< ACOMP bias current control */
    AON_ACOMP_Hysteresis_Volt_Type hysteresisPosVolt; /*!< ACOMP hysteresis voltage for positive */
    AON_ACOMP_Hysteresis_Volt_Type hysteresisNegVolt; /*!< ACOMP hysteresis voltage for negtive */
} AON_ACOMP_CFG_Type;

#define IS_AON_ACOMP_ID_TYPE(type) (((type) == AON_ACOMP0_ID) || \
                                    ((type) == AON_ACOMP1_ID))

#define IS_AON_ACOMP_SCALING_FACTOR_TYPE(type) (((type) == AON_ACOMP_SCALING_FACTOR_0P25) || \
                                                ((type) == AON_ACOMP_SCALING_FACTOR_0P5) ||  \
                                                ((type) == AON_ACOMP_SCALING_FACTOR_0P75) || \
                                                ((type) == AON_ACOMP_SCALING_FACTOR_1))

#define IS_AON_ACOMP_CHAN_TYPE(type) (((type) == AON_ACOMP_CHAN_ADC0) ||                   \
                                      ((type) == AON_ACOMP_CHAN_ADC1) ||                   \
                                      ((type) == AON_ACOMP_CHAN_ADC2) ||                   \
                                      ((type) == AON_ACOMP_CHAN_ADC3) ||                   \
                                      ((type) == AON_ACOMP_CHAN_ADC4) ||                   \
                                      ((type) == AON_ACOMP_CHAN_ADC5) ||                   \
                                      ((type) == AON_ACOMP_CHAN_ADC6) ||                   \
                                      ((type) == AON_ACOMP_CHAN_ADC7) ||                   \
                                      ((type) == AON_ACOMP_CHAN_DACA) ||                   \
                                      ((type) == AON_ACOMP_CHAN_DACB) ||                   \
                                      ((type) == AON_ACOMP_CHAN_VREF_1P25V) ||             \
                                      ((type) == AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_1) || \
                                      ((type) == AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_2) || \
                                      ((type) == AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_3) || \
                                      ((type) == AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_4) || \
                                      ((type) == AON_ACOMP_CHAN_VSS))

#define IS_AON_ACOMP_BIAS_PROG_TYPE(type) (((type) == AON_ACOMP_BIAS_POWER_MODE1) || \
                                           ((type) == AON_ACOMP_BIAS_POWER_MODE2) || \
                                           ((type) == AON_ACOMP_BIAS_POWER_MODE3) || \
                                           ((type) == AON_ACOMP_BIAS_POWER_NONE))

#define IS_AON_ACOMP_HYSTERESIS_VOLT_TYPE(type) (((type) == AON_ACOMP_HYSTERESIS_VOLT_NONE) || \
                                                 ((type) == AON_ACOMP_HYSTERESIS_VOLT_10MV) || \
                                                 ((type) == AON_ACOMP_HYSTERESIS_VOLT_20MV) || \
                                                 ((type) == AON_ACOMP_HYSTERESIS_VOLT_30MV) || \
                                                 ((type) == AON_ACOMP_HYSTERESIS_VOLT_40MV) || \
                                                 ((type) == AON_ACOMP_HYSTERESIS_VOLT_50MV) || \
                                                 ((type) == AON_ACOMP_HYSTERESIS_VOLT_60MV) || \
                                                 ((type) == AON_ACOMP_HYSTERESIS_VOLT_70MV))

int AON_ACOMP_Init(AON_ACOMP_ID_Type acompNo, AON_ACOMP_CFG_Type *cfg);
int AON_ACOMP_Enable(AON_ACOMP_ID_Type acompNo);
int AON_ACOMP_Get_Result(AON_ACOMP_ID_Type acompNo);
uint32_t AON_ACOMP_Get_Postive_Input(AON_ACOMP_ID_Type acompNo);
int AON_ACOMP_Disable(AON_ACOMP_ID_Type acompNo);

#endif /* __BL606P_ACOMP_H__ */
