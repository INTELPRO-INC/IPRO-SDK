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

#ifndef __DRV_SCALER_HYDRE_H__
#define __DRV_SCALER_HYDRE_H__

#include "scaler_hydre_reg.h"
#include "drv_common.h"

typedef enum {
    SCALER_HYDRE_TYPE_A, // ScalerHydre out port A
    SCALER_HYDRE_TYPE_B, // ScalerHydre out port B
} SCALER_HYDRE_TYPE_T;

typedef struct {
    uint16_t in_width;
    uint16_t in_height;
    uint16_t start_x;
    uint16_t start_y;
    uint16_t width;
    uint16_t height;
} SCALER_INPUT_SIZE_S;

typedef struct {
    uint16_t width;
    uint16_t height;
} SCALER_OUTPUT_SIZE_S;

typedef struct {
    SCALER_INPUT_SIZE_S  input;
    SCALER_OUTPUT_SIZE_S output;
} SCALER_CONFIG_S;

typedef struct {
    int32_t x_coeff_set;
    int32_t y_coeff_set;
    int32_t x_coeff_ratio;
    int32_t y_coeff_ratio;
} SCALER_COEF_S;

void SCALER_HYDRE_SetSizeCfg(SCALER_HYDRE_TYPE_T scaler, SCALER_CONFIG_S size_cfg);

void SCALER_HYDRE_SetXYCoef(SCALER_HYDRE_TYPE_T scaler, SCALER_COEF_S coef_cfg);

void SCALER_HYDRE_SetShdEn(void);

void SCALER_HYDRE_CheckStatus(void);

void SCALER_HYDRE_ErrIntEn(uint8_t enable);

#endif  /* __DRV_SCALER_HYDRE_H__ */
