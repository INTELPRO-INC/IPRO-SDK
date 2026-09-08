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

#ifndef __IMG_STRUCT_H__
#define __IMG_STRUCT_H__

#include <stdbool.h>
#include "isp_comm_sns.h"

typedef int IPRO_COLOR_TEMP;                   /* ct value in K */

#define ISP_AE_HIST_Y_BIN_NUM     (257)
#define ISP_AE_HIST_BAYER_BIN_NUM (65)
//#define ISP_AWB3_WEIGHT_NUM       (9)
#define ISP_AWB3_HIST_BIN_NUM_MAX (512)

#define AE_STATS_GRID_NUM_X   (12)
#define AE_STATS_GRID_NUM_Y   (8)
#define AE_STATS_GRID_NUM     ((AE_STATS_GRID_NUM_X)*(AE_STATS_GRID_NUM_Y))
#define AE_HIST_BIN_NUM       (256)

#define SENSOR_NAME_STRING_LENGTH           (32)
#define MAX_ENTRY_NUM_BAYER_PATTERN_NUM     (4)
#define MAX_ENTRY_NUM_AE_ISO_AUTO           (10)
#define MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO      (19)
#define MAX_ENTRY_NUM_AE_AGC_DB_TBL         (16)
#define MAX_ENTRY_NUM_AWB3_CAL_BIN_NUM      (88)
#define MAX_ENTRY_NUM_AWB3_SPECIAL_COLOR    (4)
#define MAX_ENTRY_NUM_AWB_INFO_TBL          (7)
#define MAX_ENTRY_NUM_GAMMA_IDX_AUTO        (3)
#define MAX_ENTRY_NUM_TM_IDX_AUTO           (3)
#define MAX_ENTRY_NUM_AWB_WHITE_REGION      (6)
#define VERT_NUM                            (6)
#define MAX_SYS_GAIN_TBL                 (82944) // 54 *1536
#define MIN_SYS_GAIN_TBL                 (-82944) // 54 *1536
#define MAX_ENTRY_NUM_NR_SAD_CURVE          (10)
#define MAX_ENTRY_NUM_NR_LUMA_CURVE         (9)

#define MAX_ENTRY_CT_COEF_NUM        (9)
#define MAX_ENTRY_CT_OFFSET_NUM      (3)
#define MAX_ENTRY_NUM_GAMMA_COEF_NUM      (257)
#define MAX_ENTRY_NUM_TM_CURVE_COEF_NUM      (64)
#define MAX_ENTRY_CS_TRANS_TBL_NUM      (2)


/****************** MODULE STATE TYPE ***********************/

typedef enum {
    ISP_HW_MODULE_AE = 0,
    ISP_HW_MODULE_AWB,
    ISP_HW_MODULE_GAMMA,
    ISP_HW_MODULE_CE,
    ISP_HW_MODULE_WDR,
    ISP_HW_MODULE_YUVCTL,
    ISP_HW_MODULE_YUVADJ,
    ISP_HW_MODULE_PARAM_NUM
} ISP_HW_MODULE_STATE_ATTR;

typedef struct  {
    Fun_Type state[ISP_HW_MODULE_PARAM_NUM];
} ISP_HW_MODULE_STATE;

typedef enum {
    FLICKER_MODE_50 = 0,
    FLICKER_MODE_60,
    FLICKER_MODE_NUM,
} flicker_mode;

typedef enum {
    ISO_AUTO          = 0,
    ISO_100_0DB       = 0,  // 0 DB
    ISO_200_6DB       = 6,  // 6 DB
    ISO_400_12DB      = 12, // 12DB
    ISO_800_18DB      = 18, // 18DB
    ISO_1600_24DB     = 24, // 24DB
    ISO_3200_30DB     = 30, // 30DB
    ISO_6400_36DB     = 36, // 36DB
    ISO_12800_42DB    = 42, // 42DB
    ISO_25600_48DB    = 48, // 48DB
    ISO_51200_54DB    = 54, // 54DB
    ISO_102400_60DB   = 60, // 60DB
    ISO_204800_66DB   = 66, // 66DB
    ISO_409600_72DB   = 72, // 72DB
    ISO_CUSTOM
}ae_iso_mode_t;

typedef enum {
    WB_AUTO,
    WB_H_2300K     = 2300,
    WB_A_2800K     = 2800,
    WB_TL84_4000K  = 4000,
    WB_CWF_4150K   = 4150,
    WB_D50_5000K   = 5000,
    WB_D65_6500K   = 6500,
    WB_D75_7500K   = 7500,
    WB_CUSTOM
}awb_color_temper_mode_t;

typedef enum {
    ISP_ADJ_AE = 0,
    ISP_ADJ_AE_TARGET,
    ISP_ADJ_AWB,
    ISP_ADJ_AWB_GAIN,
    ISP_ADJ_AWB3_CAL,
    ISP_ADJ_AWB_BIAS,
    ISP_ADJ_AWB_REGION,

    ISP_ADJ_GAMMA,
    ISP_ADJ_CT,
    ISP_ADJ_TM,
    ISP_ADJ_CS,
    ISP_ADJ_DEFOG,
    ISP_ADJ_YUVCTRL,
    ISP_ADJ_YUVADJ,
    ISP_ADJ_PARAM_NUM
}img_param_module_t;

typedef enum {
    ISP_TBL_AUTO_ISO = 0,
    ISP_TBL_AUTO_COLOR_TMPER,
    ISP_TBL_CUSTOM_ISO,
    ISP_TBL_CUSTOM_COLOR_TMPER,
    ISP_TBL_CUSTOM_EV,
    ISP_TBL_INIT,
}img_param_table_t;

typedef enum {
    ISP_IQ_PARAM_EVB = 0,
    ISP_IQ_PARAM_TURKEY_DOORBELL,
    ISP_IQ_PARAM_NUM,
}img_param_type_e;

static const char img_param_type_list[ISP_IQ_PARAM_NUM][32] = {"", "turnkey_doorbell"};

// tips for IQ tunner
typedef struct  {
    img_param_module_t module;
    img_param_table_t  table;
} img_param_header_t;

typedef struct  {
    Fun_Type state[ISP_ADJ_PARAM_NUM];
} img_param_state_container_t;

/*********************************************************/
/****************** AE PARAM TYPE ***********************/

/*typedef struct {
    uint8_t agc_db[MAX_ENTRY_NUM_AE_AGC_DB_TBL];
} ae_agc_db_tbl;

typedef struct {
    uint8_t  enable;
    uint16_t fps;
} ae_slow_shutter_conf_t;

typedef struct {
    ae_agc_db_tbl max_agc_db_tbl;
    ae_slow_shutter_conf_t slow_shutter_conf;
} ae_init_container_t;*/

typedef enum {
    AE_METERING_AVERAGE = 0,
    AE_METERING_CENTER_WEIGHTED,
    AE_METERING_SPOT,
    AE_METERING_CUSTOM,
    AE_METERING_NUM,
} AE_METERING;

typedef struct {
    uint32_t ae_target;
} ae_sub_attr;

typedef struct {
    uint16_t coef[AE_STATS_GRID_NUM];
} ae_metering_tbl_t;

/*typedef struct {
    uint8_t idx[3];
} ae_hightlight_weight;

typedef struct  {
    uint8_t enable;
    uint16_t roi_start;
    uint16_t roi_end;
    uint16_t thr[4];
    ae_hightlight_weight highlight_weight[MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO];
} ae_hightlight_weight_conf;*/

typedef struct {
    uint32_t expo_time;
    uint32_t gain;/*range 0~72*/
} ae_param_t;

typedef struct  {
    bool manual_mode;
    ae_param_t manual;
    AE_METERING metering_mode;
    ae_sub_attr container[MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO];
    ae_metering_tbl_t metering_tbl;
    //ae_hightlight_weight_conf highlight_weight_conf;
    //uint8_t speed_level;
} ae_container_sw_t;

typedef struct {
    img_param_header_t header;
    ae_container_sw_t ae_sw;
} ae_fw_container_t;

/*********************************************************/
/****************** AWB3 PARAM TYPE ***********************/

typedef struct {
    uint16_t r_gain;
    uint16_t g_gain;
    uint16_t b_gain;
} awb_rgb_gain_t;

typedef struct {
    uint16_t r_gain;
    uint16_t b_gain;
} awb_rb_gain_t;

/*typedef struct {
    uint16_t r_gain;
    uint16_t g_gain;
    uint16_t b_gain;
} awb_ct_param_t;
*/
typedef struct {
    awb_rb_gain_t bais[3];
} awb_bais_t;

typedef struct {
    uint16_t r_gain;
    uint16_t b_gain;
    uint8_t  weight;
} awb_special_color_t;

typedef struct {
    uint16_t r_gain;
    uint16_t gr_gain;
    uint16_t gb_gain;
    uint16_t b_gain;
} awb_param_t;

typedef struct {
    bool manual_mode;
    awb_param_t manual;
    //uint8_t preference_enable;
    uint8_t special_color_enable;
    //awb_ct_param_t ct;
    //awb_bais_t preference_gain[MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO];
    awb_special_color_t special_color_info[MAX_ENTRY_NUM_AWB3_SPECIAL_COLOR];
   // uint8_t speed_level;
} awb_container_sw_t;

typedef struct {
    img_param_header_t header;
    awb_container_sw_t awb_sw;
} awb_fw_container_t;

// AWB Gain
typedef struct blWB_INFO_S {
    awb_rb_gain_t rb_gain;
    IPRO_COLOR_TEMP color_temp;
} wb_info_t;

typedef struct {
    img_param_header_t header;
    int num_wb_info;
    wb_info_t wb_info[MAX_ENTRY_NUM_AWB_INFO_TBL];
} wb_info_container_t;

typedef struct {
    int32_t bin[MAX_ENTRY_NUM_AWB3_CAL_BIN_NUM];
} wb3_cal_bin_t;

typedef struct {
    img_param_header_t header;
    wb3_cal_bin_t wb3_cal;
} wb3_container_cal_t;

/*********************************************************/
/****************** GAMMA PARAM TYPE *********************/

typedef enum {
    GAMMA_CURVE_MODE_INVERSE_BT601 = 0,
    GAMMA_CURVE_MODE_INVERSE_BT709,
    GAMMA_CURVE_MODE_USER_DEFINE,
    GAMMA_CURVE_MODE_MAX_NUM
} GAMMA_CURVE_MODE_E;


typedef struct {
    int16_t coef[MAX_ENTRY_NUM_GAMMA_COEF_NUM];
} gamma_curve_t;

typedef struct {
    GAMMA_CURVE_MODE_E mode;
    gamma_curve_t curve;
} gamma_container_sw_t;

typedef struct {
    img_param_header_t header;
    gamma_container_sw_t gamma;
} gamma_corr_container_t;

/*********************************************************/
/******************* Color Enhance PARAM TYPE **********************/

typedef enum {
    ISP_CT_COLOR_GAMUT_BT601 = 0,
    ISP_CT_COLOR_GAMUT_BT709,
    ISP_CT_COLOR_GAMUT_USER,
    ISP_CT_COLOR_GAMUT_MAX
} CT_GAMUT_E;

typedef struct {
    int16_t coef[MAX_ENTRY_CT_COEF_NUM];
    int16_t pre_offset[MAX_ENTRY_CT_OFFSET_NUM];
    int16_t post_offset[MAX_ENTRY_CT_OFFSET_NUM];
} ct_param_t;

typedef struct {
    CT_GAMUT_E color_gamut;
    ct_param_t ct;
} ct_container_sw_t;

typedef struct {
    img_param_header_t header;
    ct_container_sw_t Color_Trans;
} Color_Trans_container_t;

/*********************************************************/
/******************* TM PARAM TYPE **********************/

typedef enum {
    ISP_TM_TYPE_HIST_DEFINE = 0,
    ISP_TM_TYPE_USER_DEFINE,
    ISP_TM_TYPE_MAX
} TM_TYPE_E;

typedef struct {
    int32_t curve_weight;
    int32_t dark_max_ratio;
    int32_t dark_weight[MAX_ENTRY_NUM_TM_IDX_AUTO];
    int32_t bright_max_ratio;
    int32_t bright_weight[MAX_ENTRY_NUM_TM_IDX_AUTO];
} tm_param_t;

typedef struct {
    uint16_t coef[MAX_ENTRY_NUM_TM_CURVE_COEF_NUM];
} tm_curve_t;

typedef struct {
    TM_TYPE_E type;
    bool manual_mode;
    tm_param_t manual;
    tm_param_t container[MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO];
    tm_curve_t manual_curve;
    tm_curve_t auto_curve[MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO];
} tm_container_sw_t;

typedef struct {
    img_param_header_t header;
    tm_container_sw_t tm_config;
} tm_config_container_t;


/******************* DEFOG PARAM TYPE **********************/

typedef struct {
    int32_t enh_weight;//0 ~ 16
    int32_t bright_enh_level;//0~31, 1x:32
    int32_t dark_enh_level;//0~31, 1x:32
} defog_param_t;

typedef struct {
    bool manual_mode;
    defog_param_t manual;
    defog_param_t container[MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO];
} defog_container_sw_t;

typedef struct {
    img_param_header_t header;
    defog_container_sw_t defog_config;;
} defog_config_container_t;

/******************** CS PARAM TYPE **********************/

typedef struct {
    int strength;
    int weight;//[0~16]
    int grey_thr;//[0~15]
} cs_trans_tbl_t;

typedef struct {
    uint8_t strength;
} cs_param_t;

typedef struct {
    bool manual_mode;
    cs_param_t manual;
    cs_param_t container[MAX_ENTRY_NUM_AE_ISO_AUTO];
    cs_trans_tbl_t trans_tbl[MAX_ENTRY_CS_TRANS_TBL_NUM];
} cs_container_sw_t;

typedef struct {
    img_param_header_t header;
    cs_container_sw_t csCfg;
} chroma_suppress_container_t;

/******************** YUV CTL PARAM TYPE **********************/
typedef struct {
    uint8_t brightness;
    uint8_t contrast;
    uint8_t saturation;
    /*uint8_t specialMode;
    uint8_t sepiaCb;
    uint8_t sepiaCr;
    uint8_t solarThred;*/
} yuvctrl_param_t;


typedef struct {
    bool manual_mode;
    yuvctrl_param_t manual;
    yuvctrl_param_t container[MAX_ENTRY_NUM_AE_ISO_AUTO];
} yuvctrl_container_sw_t;

typedef struct {
    img_param_header_t header;
    yuvctrl_container_sw_t yuvctrlCfg;
} yuvctrl_container_t;
/******************** YUV ADJ PARAM TYPE **********************/


typedef enum {
    YUVADJ_RANGE_LIMIT = 0,
    YUVADJ_RANGE_FULL,
    YUVADJ_RANGE_USER_DEFINE,
    YUVADJ_RANGE_MODE,
} YUVADJ_RANGE_MODE_E;

typedef struct {
    uint8_t   y_luma;
    uint16_t   y_mul0;
    uint16_t   y_mul1;
    uint8_t   y_min;
    uint8_t   y_max;
    uint16_t   uv_mul0;
    uint16_t   uv_mul1;
    uint8_t   uv_min;
    uint8_t   uv_max;
} yuvadj_param_t;

typedef struct {
    YUVADJ_RANGE_MODE_E mode;
    yuvadj_param_t container;
} yuvadj_container_sw_t;

typedef struct {
    img_param_header_t header;
    yuvadj_container_sw_t yuvadjCfg;
} yuvadj_container_t;


/************ IMG PARAM DESCRIPTION **********************/
typedef struct {
    char name[SENSOR_NAME_STRING_LENGTH];
    img_param_state_container_t   state_tbl;
    ae_container_sw_t             ae_tbl;
    awb_container_sw_t            awb_tbl;
    gamma_container_sw_t          gamma_tbl;
    ct_container_sw_t             ct_tbl;
    tm_container_sw_t             tm_tbl;
    defog_container_sw_t          defog_tbl;
    cs_container_sw_t             cs_tbl;
    yuvctrl_container_sw_t        yuvctrl_tbl;
    yuvadj_container_sw_t         yuvadj_tbl;
} IPRO_IMG_PARAM_SW_BIN_DESC_T;

typedef struct {
    char *name;
    img_param_state_container_t   *state_tbl;
    ae_container_sw_t             *ae_tbl;
    awb_container_sw_t            *awb_tbl;
    gamma_container_sw_t          *gamma_tbl;
    ct_container_sw_t             *ct_tbl;
    tm_container_sw_t             *tm_tbl;
    defog_container_sw_t          *defog_tbl;
    cs_container_sw_t             *cs_tbl;
    yuvctrl_container_sw_t        *yuvctrl_tbl;
    yuvadj_container_sw_t         *yuvadj_tbl;
} IPRO_IMG_PARAM_SW_DESC_T;

typedef struct {
    char name[SENSOR_NAME_STRING_LENGTH];
    wb_info_container_t         wb_info_tbl;
    wb3_container_cal_t         wb3_cal_tbl;
} IPRO_IMG_PARAM_CALIB_BIN_DESC_T;

typedef struct {
    char* name;
    img_param_state_container_t *state_tbl;               //sw    fw
    //ae_init_container_t         *ae_init_tbl;             //         init
    ae_fw_container_t           *ae_tbl;                  //sw
    awb_fw_container_t          *awb_tbl;                 //sw
    wb_info_container_t         *wb_info_tbl;             //      cal
    wb3_container_cal_t         *wb3_cal_tbl;             //      cal
    gamma_corr_container_t      *gamma_corr_tbl;          //sw
    Color_Trans_container_t     *color_trans_tbl;         //sw
    tm_config_container_t       *tm_config_tbl;           //sw
    defog_config_container_t    *defog_config_tbl;        //sw
    chroma_suppress_container_t *cs_tbl;                  //sw
    yuvctrl_container_t         *yuvctrl_tbl;             //sw
    yuvadj_container_t          *yuvadj_tbl;              //sw
} IPRO_IMG_PARAM_DESC_T;


#define ATTR_IMG_PARAM_DESC_TABLE __attribute__((section(".img_param_desc")))  __attribute__((used))


#endif /* __IMG_STRUCT_H__ */

