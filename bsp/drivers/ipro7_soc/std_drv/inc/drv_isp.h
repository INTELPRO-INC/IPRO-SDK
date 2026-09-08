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

#ifndef __DRV_ISP_H__
#define __DRV_ISP_H__

#include "isp_reg.h"
#include "drv_common.h"
#include <stdint.h>


#define CCM_COEF_NUM        (9)
#define CCM_OFFSET_NUM      (3)

#define CE_COEF_NUM         (9)
#define CE_OFFSET_NUM       (3)

#define GAMMA_COEF_NUM      (257)

#define ISP_AE_HIST_Y_BIN_NUM     (257)
#define ISP_AE_HIST_BAYER_BIN_NUM (65)
#define ISP_AWB3_WEIGHT_NUM       (9)
#define ISP_AWB3_HIST_BIN_NUM_MAX (512)

#define AE_STATS_GRID_NUM_X   (12)
#define AE_STATS_GRID_NUM_Y   (8)
#define AE_STATS_GRID_NUM     ((AE_STATS_GRID_NUM_X)*(AE_STATS_GRID_NUM_Y))
#define AE_HIST_BIN_NUM       (256)

typedef enum {
    BAYER_PATTERN_RG = 0,
    BAYER_PATTERN_GR,
    BAYER_PATTERN_GB,
    BAYER_PATTERN_BG,
} BAYER_PATTERN_E;

typedef enum {
    ISP_GAMMA_CHANNEL_R,
    ISP_GAMMA_CHANNEL_G,
    ISP_GAMMA_CHANNEL_B,
}ISP_Gamma_Channel;

typedef struct {
    uint16_t curve_coef[GAMMA_COEF_NUM];
} ISP_Gamma_Curve_Type;

/**
 *  @brief Crop configuration strcut definition
 */
typedef struct {
    uint16_t hsyncStart;                     /*!< Hsync valid start */
    uint16_t hsyncEnd;                       /*!< Hsync valid end */
    uint16_t vsyncStart;                     /*!< Vsync valid start */
    uint16_t vsyncEnd;                       /*!< Vsync valid end */
}ISP_Crop_Cfg_Type;

typedef struct {
    uint16_t y_luma;
    uint16_t y_mul0;
    uint16_t y_mul1;
    uint8_t y_min;
    uint8_t y_max;
    uint16_t uv_mul0;
    uint16_t uv_mul1;
    uint8_t uv_min;
    uint8_t uv_max;
}ISP_YUV_Adjust_Cfg_Type;

/*
    Color enhance Coefficient
*/
typedef enum {
    ISP_CE_11 = 0,
    ISP_CE_12 = 1,
    ISP_CE_13 = 2,
    ISP_CE_21 = 3,
    ISP_CE_22 = 4,
    ISP_CE_23 = 5,
    ISP_CE_31 = 6,
    ISP_CE_32 = 7,
    ISP_CE_33 = 8,
} ISP_CE_COEF_TYPE_E;

typedef enum {
    ISP_CE_OFFSET_0 = 0,
    ISP_CE_OFFSET_1 = 1,
    ISP_CE_OFFSET_2 = 2,
} ISP_CE_CFFSET_TYPE_E;

typedef struct {
    int offset[CE_OFFSET_NUM];
} ISP_CE_OFFSET_S;

typedef struct {
    int coef[CE_COEF_NUM];
} ISP_CE_COEF_S;

typedef struct {
    uint32_t saturation_cb;
    uint32_t saturation_cr;
} ISP_SATURATION_S;

typedef enum {
    AE_GRID_DATA_RGB = 0,
    AE_GRID_DATA_Y = 1,
} AE_GRID_DATA_TYPE_E;

typedef enum {
    AWB_GRID_DATA_RG = 0,
    AWB_GRID_DATA_B = 1,
} AWB_GRID_DATA_TYPE_E;

typedef struct {
    uint16_t            roi_start_x;
    uint16_t            roi_start_y;
    uint16_t            roi_width;
    uint16_t            roi_height;
    uint16_t            img_width;
    uint16_t            img_height;
    uint8_t             grid_width;
    uint8_t             grid_height;
    uint8_t             grid_num_x;
    uint8_t             grid_num_y;
} isp_ae_stats_conf_t;

typedef struct {
    uint32_t            r_sum           :   8;
    uint32_t            g_sum           :   8;
    uint32_t            b_sum           :   8;
    uint32_t            reserved_24_31  :   8;
    uint32_t            y_sum           :   8;
    uint32_t            reserved_8_31   :   24;
} isp_ae_sum_t;

typedef enum {
    AE_HIST_MODE_BAYER = 0,
    AE_HIST_MODE_Y
} AE_HIST_MODE_TYPE_E;

typedef struct {
    uint16_t    ae_hist_x_min;
    uint16_t    ae_hist_x_max;
    uint16_t    ae_hist_y_min;
    uint16_t    ae_hist_y_max;
} isp_ae_hist_roi_t;

typedef struct {
    // control register
    // int8_t stat_en; // r/w, shadow, 1'b
    uint16_t stat_x_min; // r/w, shadow, 11'b, range: 0 ~ (width - 1), must be even
    uint16_t stat_x_max; // r/w, shadow, 11'b, range: 0 ~ (width - 1), must be odd
    uint16_t stat_y_min; // r/w, shadow, 11'b, range: 0 ~ (height - 1)
    uint16_t stat_y_max; // r/w, shadow, 11'b, range: 0 ~ (height - 1)
    uint16_t r_ratio_ofst; // r/w, shadow, 12'b, = RATIO_BW, 4.8 fixed-point, range: 0 ~ (2 ^ 12 - 1)
    uint16_t b_ratio_ofst; // r/w, shadow, 12'b, = RATIO_BW, 4.8 fixed-point, range: 0 ~ (2 ^ 12 - 1)
    uint16_t r_ratio_mult; // r/w, shadow, 12'b, integer, range: 0 ~ (2 ^ 12 - 1)
    uint16_t b_ratio_mult; // r/w, shadow, 12'b, integer, range: 0 ~ (2 ^ 12 - 1)
    uint16_t ratio_sum_min;  // r/w, shadow, 9'b, = NORM_BW + 1, range: 0 ~ (2 ^ 9 - 1)
    uint16_t ratio_sum_max; // r/w, shadow, 9'b, = NORM_BW + 1, range: 0 ~ (2 ^ 9 - 1)
    uint8_t g_divisor; // r/w, shadow, 1'b
    uint8_t weight_ratio_en; // r/w, shadow, 1'b
    uint8_t weight_ratio_gain; // r/w, shadow, 8'b, = WEIGHT_GAIN_BW, 4.4 fixed-point, range: 0 ~ (2 ^ 8 - 1)
    uint8_t weight_ratio[ISP_AWB3_WEIGHT_NUM][ISP_AWB3_WEIGHT_NUM]; // r/w, shadow, 8'b, = WEIGHT_BW, range: 0 ~ (2 ^ 8 - 1)
    uint16_t y_r_coeff; // r/w, shadow, 12'b, = COEFF_BW, 4.8 fixed-point, range: 0 ~ (2 ^ 12 - 1)
    uint16_t y_g_coeff; // r/w, shadow, 12'b, = COEFF_BW, 4.8 fixed-point, range: 0 ~ (2 ^ 12 - 1)
    uint16_t y_b_coeff; // r/w, shadow, 12'b, = COEFF_BW, 4.8 fixed-point, range: 0 ~ (2 ^ 12 - 1)
    uint8_t weight_y_use_g; // r/w, shadow, 1'b
    uint8_t weight_y_en; // r/w, shadow, 1'b
    uint8_t weight_y_gain; // r/w, shadow, 8'b, = WEIGHT_GAIN_BW, 4.4 fixed-point, range: 0 ~ (2 ^ 8 - 1)
    uint8_t weight_y[ISP_AWB3_WEIGHT_NUM][ISP_AWB3_WEIGHT_NUM]; // r/w, shadow, 8'b, = WEIGHT_BW, range: 0 ~ (2 ^ 8 - 1)
    uint8_t count_zero_weight; // r/w, shadow, 1'b
    uint8_t ink_type; // r/w, shadow, 2'b, range: 0 ~ 2
    uint8_t ink_gain; // r/w, shadow, 4'b, integer, range: 0 ~ (2 ^ 4 - 1)
} isp_awb3_stats_conf_t;

typedef struct {
    // status register
    uint32_t r_sum; // r, ping-pong, 32'b, = PIXEL_NUM_BW + PIXEL_BW
    uint32_t g_sum; // r, ping-pong, 32'b, = PIXEL_NUM_BW + PIXEL_BW
    uint32_t b_sum; // r, ping-pong, 32'b, = PIXEL_NUM_BW + PIXEL_BW
    uint32_t r_sum_hex; // r, ping-pong, 32'b, = PIXEL_NUM_BW + PIXEL_BW
    uint32_t g_sum_hex; // r, ping-pong, 32'b, = PIXEL_NUM_BW + PIXEL_BW
    uint32_t b_sum_hex; // r, ping-pong, 32'b, = PIXEL_NUM_BW + PIXEL_BW
    uint32_t pixel_hex; // r, ping-pong, 20'b, = PIXEL_NUM_BW
    uint64_t r_ratio_hex_y; // r, ping-pong, 40'b, = PIXEL_NUM_BW + WEIGHT_BW + RATIO_BW
    uint64_t b_ratio_hex_y; // r, ping-pong, 40'b, = PIXEL_NUM_BW + WEIGHT_BW + RATIO_BW
    uint32_t weight_hex_y; // r, ping-pong, 28'b, = PIXEL_NUM_BW + WEIGHT_BW
    uint32_t pixel_hex_y; // r, ping-pong, 20'b, = PIXEL_NUM_BW
    uint64_t r_ratio_grid; // r, ping-pong, 40'b, = PIXEL_NUM_BW + WEIGHT_BW + RATIO_BW
    uint64_t b_ratio_grid; // r, ping-pong, 40'b, = PIXEL_NUM_BW + WEIGHT_BW + RATIO_BW
    uint32_t weight_grid; // r, ping-pong, 28'b, = PIXEL_NUM_BW + WEIGHT_BW
    uint32_t pixel_grid; // r, ping-pong, 20'b, = PIXEL_NUM_BW
} isp_awb3_sum_t;

typedef enum {
    ISP_2A_INT_ID_AE_STAT,
    ISP_2A_INT_ID_AE_HIST,
    ISP_2A_INT_ID_AWB_STAT,
}ISP_2A_INT_ID;

typedef enum {
    ISP_DBG_LINK_SEL_YUV422TO444        = 0,   // YUV444(8),  {v, u, y}
    ISP_DBG_LINK_SEL_WDR_YU             = 1,   // YUV444(12), {u, y}
    ISP_DBG_LINK_SEL_WDR_VY             = 2,   // YUV444(12), {y, v}
    ISP_DBG_LINK_SEL_WDR_UV             = 3,   // YUV444(12), {v, u}
    ISP_DBG_LINK_SEL_YUVCTRL            = 4,   // YUV444(8),  {v, u, y}
    ISP_DBG_LINK_SEL_CROP               = 5,   // YUV422(8),  {8'd0, c, y}
    ISP_DBG_LINK_SEL_CE_RG              = 6,   // RGB(12),    {b1, b0}
    ISP_DBG_LINK_SEL_CE_BR              = 7,   // RGB(12),    {b0, b2}
    ISP_DBG_LINK_SEL_CE_GB              = 8,   // RGB(12),    {b2, b1}
    ISP_DBG_LINK_SEL_GAMMA_RG           = 9,   // RGB(12),    {b1, b0}
    ISP_DBG_LINK_SEL_GAMMA_BR           = 10,  // RGB(12),    {b0, b2}
    ISP_DBG_LINK_SEL_GAMMA_GB           = 11,  // RGB(12),    {b2, b1}
    ISP_DBG_LINK_SEL_RGBTOBAYER         = 12,  // Bayer(12),  {12'd0, bayer}
    ISP_DBG_LINK_SEL_AWB_INK            = 13,  // INK(8),     {16'd0, awb3_ink}
}ISP_DBG_LINK_SEL;

/*
    Set/Get Brightness
    value range: TODO  -128-127
 */
void ISP_Set_Brightness(int value);
void ISP_Get_Brightness(int *value);
/*
    Set/Get Contrast
    value range: TODO 0-255
*/
void ISP_Set_Contrast(int value);
void ISP_Get_Contrast(int *value);
/*
    Set/Get Saturation
    value range:TODO  0-255
*/
void ISP_Set_Saturation(const ISP_SATURATION_S *saturation);
void ISP_Get_Saturation(ISP_SATURATION_S *saturation);
void ISP_Set_Special_Mode(int value);
void ISP_Get_Special_Mode(int *value);
void ISP_Set_LPF_Filting_Mode(int value);
void ISP_Get_LPF_Filting_Mode(int *value);
void ISP_Set_Sepia(int SepiaCr, int SepiaCb);
void ISP_Get_Sepia(int *SepiaCr, int *SepiaCb);
void ISP_Set_Solar_Thred(int value);
void ISP_Get_Solar_Thred(int *value);

void ISP_Set_Crop_Cfg(ISP_Crop_Cfg_Type* cropCfg);
void ISP_Get_Crop_Cfg(ISP_Crop_Cfg_Type* cropCfg);
void ISP_Set_Crop_State(uint8_t state);
void ISP_Get_Crop_State(uint8_t *state);

void ISP_YUV_Adjust_Init(void);
void ISP_Set_YUV_Adjust_Cfg(ISP_YUV_Adjust_Cfg_Type *cfg);
void ISP_Get_YUV_Adjust_Cfg(ISP_YUV_Adjust_Cfg_Type *cfg);
void ISP_Set_YUV_Adjust_State(uint8_t state);
void ISP_Get_YUV_Adjust_State(uint8_t *state);

void ISP_Init_Color_Enhancement();
void ISP_Set_CE_State(uint8_t state);
void ISP_Get_CE_State(uint8_t *state);
void ISP_Set_CE_Coef(const ISP_CE_COEF_S *ce);
void ISP_Get_CE_Coef(ISP_CE_COEF_S *ce);
void ISP_Set_CE_Pre_Offset(const ISP_CE_OFFSET_S *ce);
void ISP_Get_CE_Pre_Offset(ISP_CE_OFFSET_S *ce);
void ISP_Set_CE_Post_Offset(const ISP_CE_OFFSET_S *ce);
void ISP_Get_CE_Post_Offset(ISP_CE_OFFSET_S *ce);

void ISP_Set_Gamma_State(uint8_t state);
void ISP_Get_Gamma_State(uint8_t *state);
void ISP_Set_Gamma_Update(void);
/*
    Set/Get Gamma
    value range: 0-4095
*/
void ISP_Set_Gamma_Coef(const ISP_Gamma_Curve_Type *gamma_curve, ISP_Gamma_Channel chn);
void ISP_Get_Gamma_Coef(ISP_Gamma_Curve_Type *gamma_curve, ISP_Gamma_Channel chn);

void ISP_Set_AWB3_Stats_State(uint8_t state);
void ISP_Get_AWB3_Stats_State(uint8_t *state);
void ISP_Set_AWB3_Stats_Conf(const isp_awb3_stats_conf_t *awb_stats_conf);
void ISP_Get_AWB3_Stats_Conf(isp_awb3_stats_conf_t *awb_stats_conf);
int  ISP_Get_AWB3_Sum(isp_awb3_sum_t *sum);
void ISP_Set_AWB3_G_Divisor(int value);
int ISP_Get_AWB3_G_Divisor(int *value);
void ISP_Set_RGBtoBayer(const BAYER_PATTERN_E pattern);
void ISP_Get_RGBtoBayer(BAYER_PATTERN_E *pattern);

void ISP_Set_AE_State(uint8_t state);
void ISP_Set_AE_Stats_Conf(const isp_ae_stats_conf_t *ae_stats_conf);
void ISP_Get_AE_Stats_Conf(isp_ae_stats_conf_t *ae_stats_conf);
void ISP_Set_AE_Ysum_Coeff(uint8_t coeff_r, uint8_t coeff_g, uint8_t coeff_b);
void ISP_Get_AE_Ysum_Coeff(uint8_t *coeff_r, uint8_t *coeff_g, uint8_t *coeff_b);
int ISP_Get_AE_Stats(int grid_num, uint32_t *data);
void ISP_Get_AE_Sum(int grid_idx, AE_GRID_DATA_TYPE_E data_type, uint32_t *data);
void ISP_Set_AE_Hist_State(uint8_t state);
int  ISP_Get_AE_Hist_State(uint8_t *state);
void ISP_Set_AE_Hist_Mode(AE_HIST_MODE_TYPE_E mode);
int  ISP_Get_AE_Hist_Mode(AE_HIST_MODE_TYPE_E *mode);
void ISP_Set_AE_Hist_ROI_State(uint8_t state);
int  ISP_Get_AE_Hist_ROI_State(uint8_t *state);
int  ISP_Set_AE_Hist_ROI(uint8_t state, const isp_ae_hist_roi_t *roi);
int  ISP_Get_AE_Hist_ROI(uint8_t state, isp_ae_hist_roi_t *roi);
int  ISP_Get_AE_Hist_SUM_Y(uint32_t *y_sum);
int  ISP_Get_AE_Hist_SUM_Bayer(uint32_t *r_sum, uint32_t *gr_sum, uint32_t *gb_sum, uint32_t *b_sum);

void ISP_Set_2A_Interrupt(ISP_2A_INT_ID id, uint8_t enable);
void ISP_Clr_2A_Interrupt_Status(ISP_2A_INT_ID id);

void ISP_Set_Debug_Link_Sel(uint8_t id, ISP_DBG_LINK_SEL sel);

int ISP_Get_AE_W_Cnt_R(void);

#endif  /* __DRV_ISP_H__ */
