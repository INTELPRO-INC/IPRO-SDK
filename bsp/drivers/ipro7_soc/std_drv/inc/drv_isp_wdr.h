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

#ifndef __DRV_ISP_WDR_H__
#define __DRV_ISP_WDR_H__

#include "wdr_csr_reg.h"
#include "wdr_hist_sts_reg.h"
#include <drv_isp.h>

#define ISP_WDR_HIST_ROI_NUM                (8)
#define ISP_WDR_HIST_BIN_NUM                (65)
#define ISP_WDR_CURVE_COEF_NUM              (64)
#define ISP_WDR_CURVE_HIST_BIN_NUM          (65)

typedef struct {
    uint16_t gain;
    uint8_t  weight;
    uint8_t  grey_thr;
} ISP_Chroma_Suppress_Type;

typedef struct {
    uint32_t hist_bin[ISP_WDR_HIST_BIN_NUM];
} ISP_WDR_Hist_Data_Type;

typedef struct {
    uint16_t curve_coef[ISP_WDR_CURVE_COEF_NUM];
} ISP_WDR_Curve_Type;

void ISP_WDR_Set_WDR_State(uint8_t state);
void ISP_WDR_Get_WDR_State(uint8_t *state);
void ISP_WDR_Set_CS_State(uint8_t state);
void ISP_WDR_Get_CS_State(uint8_t *state);
void ISP_WDR_Set_CS_Config(const ISP_Chroma_Suppress_Type *chromaSupprCfg);
void ISP_WDR_Get_CS_Config(ISP_Chroma_Suppress_Type *chromaSupprCfg);
void ISP_WDR_Set_ROI_State(int roi_id, uint8_t state);
void ISP_WDR_Get_ROI_State(int roi_id, uint8_t *state);
void ISP_WDR_Set_ROI_Position(int roi_id, int left_x, int right_x,
                                int top_y, int bottom_y);
void ISP_WDR_Get_ROI_Position(int roi_id, int *left_x, int *right_x,
                                int *top_y, int *bottom_y);
void ISP_WDR_Set_ROI_Weight(int roi_id, int weight);
void ISP_WDR_Get_ROI_Weight(int roi_id, int *weight);
int  ISP_WDR_Get_Hist_Data(ISP_WDR_Hist_Data_Type *hist);
void ISP_WDR_Set_WDR_Curve_State(uint8_t state);
void ISP_WDR_Get_WDR_Curve_State(uint8_t *state);
void ISP_WDR_Set_WDR_Curve(const ISP_WDR_Curve_Type *wdr_curve);
void ISP_WDR_Get_WDR_Curve(ISP_WDR_Curve_Type *wdr_curve);
void ISP_WDR_Set_WDR_Curve_Weight(int weight);
void ISP_WDR_Get_WDR_Curve_Weight(int *weight);
void ISP_WDR_Set_Post_Enhance_State(uint8_t state);
void ISP_WDR_Get_Post_Enhance_State(uint8_t *state);
void ISP_WDR_Set_Post_Enhance_Weight(int weight);
void ISP_WDR_Get_Post_Enhance_Weight(int *weight);
void ISP_WDR_Set_Post_Enhance_Y_Offset(int y_offset);
void ISP_WDR_Get_Post_Enhance_Y_Offset(int *y_offset);
void ISP_WDR_Set_Post_Enhance_Luma_Gain(int gain);
void ISP_WDR_Get_Post_Enhance_Luma_Gain(int *gain);
void ISP_WDR_Set_Post_Enhance_Chroma_Gain(int gain);
void ISP_WDR_Get_Post_Enhance_Chroma_Gain(int *gain);

void ISP_WDR_Set_Interrupt(uint8_t enable);
void ISP_WDR_Clr_Interrupt_Status(void);

#endif /* __DRV_ISP_WDR_H__ */
