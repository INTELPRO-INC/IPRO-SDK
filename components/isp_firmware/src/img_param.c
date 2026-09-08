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

#include <string.h>
#include <stdio.h>
#include "img_param.h"

#define TO_LOWER(c) (((c) >= 'A' && (c) <= 'Z') ? ((c) + 32) : (c))

extern IPRO_IMG_PARAM_DESC_T _ld_img_param_desc_start;
extern IPRO_IMG_PARAM_DESC_T _ld_img_param_desc_end;
//extern IPRO_IMG_PARAM_DESC_T sc101iot_img_param;

#define PARAM_IDX_MIN  0
#define PARAM_MODULE_IDX_MAX        (ISP_ADJ_PARAM_NUM - 1)
#define PARAM_AE_TARGET_IDX_MAX     (MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO - 1)
#define PARAM_AE_METERING_IDX_MAX   (AE_METERING_NUM - 1)
#define PARAM_AE_SPEED_LEVEL_MAX    (5)
#define PARAM_AWB_SPEED_LEVEL_MAX   (5)
#define PARAM_AWB_PREF_IDX_MAX      (MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO - 1)
#define PARAM_TM_IDX_MAX            (MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO - 1)
#define PARAM_TM_MODE_NUM           (3)
#define PARAM_DEFOG_IDX_MAX         (MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO - 1)
#define PARAM_CS_IDX_MAX            (MAX_ENTRY_NUM_AE_ISO_AUTO - 1)
#define PARAM_CS_TRANS_IDX_MAX      (MAX_ENTRY_CS_TRANS_TBL_NUM - 1)
#define PARAM_YUVCTRL_IDX_MAX       (MAX_ENTRY_NUM_AE_ISO_AUTO - 1)

#define PRINT_REG 0

//static int load_inited = 0;
static IPRO_IMG_PARAM_DESC_T *img_param = NULL;
static IPRO_IMG_PARAM_SW_DESC_T *img_param_sw = NULL;
/*static IPRO_IMG_PARAM_SW_BIN_DESC_T *img_param_sw_init = NULL;
static IPRO_IMG_PARAM_CALIB_BIN_DESC_T *img_param_calib = NULL;*/
ATTR_PSRAM_SECTION  ae_metering_tbl_t g_ae_metering_tbl[AE_METERING_NUM -1] = {
    { // AE_METERING_AVERAGE
        .coef = {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        }
    },
    { // AE_METERING_CENTER_WEIGHTED
        .coef = {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 2, 3, 3, 2, 1, 1, 1, 1,
            1, 1, 1, 2, 3, 4, 4, 3, 2, 1, 1, 1,
            1, 1, 1, 2, 3, 4, 4, 3, 2, 1, 1, 1,
            1, 1, 1, 1, 2, 3, 3, 2, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        }
    },
    { // AE_METERING_SPOT
        .coef = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 3, 3, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 3, 3, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    }
};

void print_tbl(void);

int img_param_init(IPRO_IMG_PARAM_DESC_T *imgParam)
{
    img_param = imgParam;
    return 0;
}

static int img_param_get_index(char *name, int value, int min, int max)
{
    int index = value;
    if (!is_param_valid(index, min, max)) {
        clip_param_range(&index, min, max);
        LOGE("[ERROR] [%s] img_param_get_index : invalid param idx %d, clip to %d\r\n", name, value, index);
    }

    return index;
}

static int find_name(const char* sensor_name, char* iq_name)
{
    if (!sensor_name || !iq_name || !*iq_name)
        return 0;
    int i, j;
    int la = strlen(iq_name), ln = strlen(sensor_name);
    for (i = 0; i <= ln; i++) {
        for (j = 0; j < la; j++) {
            if (TO_LOWER(sensor_name[i + j]) != TO_LOWER(iq_name[j]))
                break;
        }
        if (j == la) return 1;
    }
    return 0;
}

#if 0
/*********** calib tbl ************************************************************************************************/
int img_param_free_calib_buffer(void)
{
    //if (NULL != img_param_calib) free(img_param_calib);
    return 0;
}

IPRO_IMG_PARAM_CALIB_BIN_DESC_T img_param_calib_buff;
wb_info_t wb_info_buff[MAX_ENTRY_NUM_AWB_CT_AUTO] = {0};

int img_param_calib_load(const char* sensor_name)
{
    img_param_free_calib_buffer();

    // alloc memory for pointer
    img_param_calib = &img_param_calib_buff;//malloc(sizeof(IPRO_IMG_PARAM_CALIB_BIN_DESC_T));
    memset(img_param_calib, 0, sizeof(IPRO_IMG_PARAM_CALIB_BIN_DESC_T));

    img_param_calib->wb_info_tbl.wb_info = &wb_info_buff[0];//malloc(sizeof(wb_info_t) * MAX_ENTRY_NUM_AWB_CT_AUTO);
    memset(img_param_calib->wb_info_tbl.wb_info, 0, sizeof(wb_info_t) * MAX_ENTRY_NUM_AWB_CT_AUTO);

    memcpy(&img_param_calib->name, img_param->name, sizeof(char) * 16);
    memcpy(&img_param_calib->wb_info_tbl, img_param->wb_info_tbl, sizeof(wb_info_container_t));
    memcpy(img_param_calib->wb_info_tbl.wb_info, img_param->wb_info_tbl->wb_info, sizeof(wb_info_t) * img_param->wb_info_tbl->num_wb_info);

    memcpy(&img_param_calib->wb3_cal_tbl, img_param->wb3_cal_tbl, sizeof(wb3_container_cal_t));

   // memcpy(&img_param_calib->gamma_cal_tbl, img_param_sw->gamma_cal_tbl, sizeof(gamma_container_cal_t));

    return 0;

}
/*
int img_param_calib_get_gamma(int param_idx, gamma_cal_t **gamma_cal)
{
    if (param_idx < 0) {
        *gamma_cal = &img_param_calib->gamma_cal_tbl.container;
    } else if (param_idx < MAX_ENTRY_NUM_GAMMA_IDX_AUTO) {
        *gamma_cal = &img_param_calib->gamma_cal_tbl.container[param_idx];
    } else {
        return -1;
    }
    return 0;
}

int img_param_calib_set_gamma(int param_idx, const gamma_cal_t *gamma_cal)
{
    if (param_idx < 0) {
        memcpy(&img_param_calib->gamma_cal_tbl.container, gamma_cal, sizeof(gamma_cal_t) * MAX_ENTRY_NUM_GAMMA_IDX_AUTO);
    } else if (param_idx < MAX_ENTRY_NUM_GAMMA_IDX_AUTO) {
        memcpy(&img_param_calib->gamma_cal_tbl.container[param_idx], gamma_cal, sizeof(gamma_cal_t));
    } else {
        return -1;
    }
    return 0;
}*/


void print_cal_tbl(void)
{

    int i;//,j;

    LOGD("[OK] img_param_cal_load name %s\r\n", img_param_calib->name);

    LOGD("[OK] img_param_get_wb_info_cal num: %u,\r\n", img_param_calib->wb_info_tbl.num_wb_info);
    for (i = 0; i < img_param_calib->wb_info_tbl.num_wb_info; i++) {
        LOGD("[OK] img_param_get_wb_info_cal cfg[%d]: %d, %d %d\r\n", i,
            (int)img_param_calib->wb_info_tbl.wb_info[i].rb_gain.r_gain,
            (int)img_param_calib->wb_info_tbl.wb_info[i].rb_gain.b_gain,
            (int)img_param_calib->wb_info_tbl.wb_info[i].color_temp
            );
    }


    for (i = 0; i < MAX_ENTRY_NUM_AWB3_CAL_BIN_NUM; i++) {
            LOGD("[OK] img_param_get_awb_cal cfg[%d]: %u,\r\n", i, (unsigned int)img_param_calib->wb3_cal_tbl.wb3_cal.bin[i]);
    }

   /* for (i = 0; i < MAX_ENTRY_NUM_GAMMA_IDX_AUTO; i++) {
        LOGD("[OK] img_param_get_gamma_cal cfg[%d]:", i);
        for (j = 0; j < GAMMA_COEF_NUM; j++) {
            LOGD(" %d,", img_param_calib->gamma_cal_tbl.container[i].gamma_curve.coef[j]);
        }
        LOGD("\r\n");
        LOGD("[OK] img_param_get_gamma_cal idx[%d]: %d,\r\n", i, img_param_calib->gamma_cal_tbl.container[i].idx);
    }*/

}

#endif
/*********** fw tbl ************************************************************************************************/
int img_param_load(const char* sensor_name, int iq_param_type)
{

#if 0
    img_param = &_ld_img_param_desc_start;//&sc101iot_img_param;//
    return 0;

#else
    int i = 0;
    IPRO_IMG_PARAM_DESC_T *start, *end;

    start = &_ld_img_param_desc_start;//&sc101iot_img_param;//
    end = &_ld_img_param_desc_end;

    while (start < end) {
        LOGI("try matching the img param desc [%s] [%s] iq_param_type %d\r\n", sensor_name, start->name, iq_param_type);
        if ((iq_param_type == ISP_IQ_PARAM_EVB) && (find_name(sensor_name, start->name) == 1)){
            img_param = start;
            //LOGI("img_param_load |%s|  |%s|\r\n", img_param->name, sensor_name);
            return 0;
        }

        for (i = 1; i < ISP_IQ_PARAM_NUM; i++) {
              if ((find_name(sensor_name, start->name) == 1) && (iq_param_type == i) && (strstr(start->name, img_param_type_list[i]) != NULL)) {
                img_param = start;
                return 0;
            }
        }
        start++;
    }

    return -1;
#endif
}
#if 0
int img_param_get_module(const IPRO_IMG_PARAM_DESC_T **module)
{
    if (NULL == img_param) {
        LOGE("img_param not init!");
        return -1;
    }

    *module = img_param;
    return 0;
}

int img_param_get_ae_agc_db_tbl(const ae_agc_db_tbl **cfg)
{
    *cfg = &img_param->ae_init_tbl->max_agc_db_tbl;

    return 0;
}

int img_param_get_ae_slow_shutter_conf(const ae_slow_shutter_conf_t **cfg)
{
    *cfg = &img_param->ae_init_tbl->slow_shutter_conf;

    return 0;
}
#endif
int img_param_get_wb_info(int param_idx, const wb_info_t **wb_info)
{
    int index = img_param_get_index("get_wb_info", param_idx, PARAM_IDX_MIN, img_param->wb_info_tbl->num_wb_info-1);
    *wb_info = &img_param->wb_info_tbl->wb_info[index];

    return 0;
}

int img_param_set_wb_info(int param_idx, wb_info_t *wb_info)
{
    int index = img_param_get_index("set_wb_info", param_idx, PARAM_IDX_MIN, img_param->wb_info_tbl->num_wb_info-1);

    img_param->wb_info_tbl->wb_info[index].color_temp = wb_info->color_temp;
    img_param->wb_info_tbl->wb_info[index].rb_gain.r_gain = wb_info->rb_gain.r_gain;
    img_param->wb_info_tbl->wb_info[index].rb_gain.b_gain = wb_info->rb_gain.b_gain;

    return 0;
}

int img_param_get_num_wb_info(void)
{
    return img_param->wb_info_tbl->num_wb_info;
}

int img_param_set_num_wb_info(int num)
{
    img_param->wb_info_tbl->num_wb_info = num;
    return 0;
}

int img_param_get_wb3_cal(const wb3_cal_bin_t **wb3_cal)
{
    *wb3_cal = &img_param->wb3_cal_tbl->wb3_cal;
    return 0;
}

int img_param_set_wb3_cal(wb3_cal_bin_t *wb3_cal)
{
    memcpy(&img_param->wb3_cal_tbl->wb3_cal, wb3_cal, sizeof(wb3_cal_bin_t));
    return 0;
}

//IPRO_IMG_PARAM_SW_BIN_DESC_T img_param_sw_init_buff;//

/*********** sw tbl ************************************************************************************************/
/*
int img_param_malloc_sw_param_init_buffer(void)
{
    if (load_inited == 1) {
        return 0;
    }

    img_param_free_sw_param_init_buffer();
    img_param_sw_init = &img_param_sw_init_buff;//malloc(sizeof(IPRO_IMG_PARAM_SW_BIN_DESC_T));
    memset(img_param_sw_init, 0, sizeof(IPRO_IMG_PARAM_SW_BIN_DESC_T));

    load_inited = 1;
    return load_inited;
}

int img_param_free_sw_param_init_buffer(void)
{
    //if (NULL != img_param_sw_init) free(img_param_sw_init);
    return 0;
}
*/
int img_param_free_sw_param_buffer(void)
{
    if (NULL != img_param_sw) free(img_param_sw);
    return 0;
}
/*

int img_param_get_sw_bin_Addr(uint32_t *addr)
{
    *addr = (uint32_t)img_param_sw_init;
    return 0;
}

int img_param_sw_tbl_init(void)
{
    img_param_malloc_sw_param_init_buffer();

    memcpy(img_param_sw_init->name, img_param->name, sizeof(char) * SENSOR_NAME_STRING_LENGTH);
    memcpy(&img_param_sw_init->state_tbl, img_param->state_tbl, sizeof(img_param_state_container_t));
    memcpy(&img_param_sw_init->ae_tbl, &img_param->ae_tbl->ae_sw, sizeof(ae_container_sw_t));
    memcpy(&img_param_sw_init->awb_tbl, &img_param->awb_tbl->awb_sw, sizeof(awb_container_sw_t));
    memcpy(&img_param_sw_init->awb_tbl.preference_enable , &img_param->awb_tbl->awb_sw.preference_enable, sizeof(uint8_t));
    memcpy(&img_param_sw_init->awb_tbl.special_color_enable , &img_param->awb_tbl->awb_sw.special_color_enable, sizeof(uint8_t));
    memcpy(img_param_sw_init->awb_tbl.preference_gain, img_param->awb_tbl->awb_sw.preference_gain, sizeof(awb_bais_t) * MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO);
    memcpy(img_param_sw_init->awb_tbl.special_color_info, img_param->awb_tbl->awb_sw.special_color_info, sizeof(awb_special_color_t) * MAX_ENTRY_NUM_AWB3_SPECIAL_COLOR);
    memcpy(&img_param_sw_init->awb_tbl.speed_level , &img_param->awb_tbl->awb_sw.speed_level, sizeof(uint8_t));
    memcpy(&img_param_sw_init->awb_tbl.ct , &img_param->awb_tbl->awb_sw.ct, sizeof(awb_ct_param_t));
    memcpy(&img_param_sw_init->gamma_tbl, &img_param->gamma_corr_tbl->gamma, sizeof(gamma_container_sw_t));
    memcpy(&img_param_sw_init->ct_tbl, &img_param->color_trans_tbl->Color_Trans, sizeof(ct_container_sw_t));
    memcpy(&img_param_sw_init->tm_tbl, &img_param->tm_config_tbl->tm_config, sizeof(tm_container_sw_t));
    memcpy(&img_param_sw_init->tm_tbl.manual_curve, &img_param->tm_config_tbl->tm_config.manual_curve, sizeof(uint16_t) * MAX_ENTRY_NUM_TM_CURVE_COEF_NUM);
    //memcpy(&img_param_sw_init->tm_tbl.auto_curve, &img_param->tm_config_tbl->tm_config.auto_curve, sizeof(uint16_t) * MAX_ENTRY_NUM_TM_CURVE_COEF_NUM * MAX_ENTRY_NUM_AE_ISO_AUTO);
    memcpy(&img_param_sw_init->defog_tbl, &img_param->defog_config_tbl->defog_config, sizeof(defog_container_sw_t));
    memcpy(&img_param_sw_init->cs_tbl, &img_param->cs_tbl->csCfg, sizeof(cs_container_sw_t));
    memcpy(&img_param_sw_init->yuvctrl_tbl, &img_param->yuvctrl_tbl->yuvctrlCfg, sizeof(yuvctrl_container_sw_t));
    memcpy(&img_param_sw_init->yuvadj_tbl, &img_param->yuvadj_tbl->yuvadjCfg, sizeof(yuvadj_container_sw_t));

    //LOGI("try matching the img param sw img_param_sw_init 0x%x\r\n", (unsigned int)img_param_sw_init);
    //LOGI("try matching the img param sw desc [%s]\r\n", img_param_sw_init->name);

    return 0;

}
*/

//IPRO_IMG_PARAM_SW_DESC_T img_param_sw_buff = {0};

int img_param_sw_load(const char* sensor_name)
{

#if 1
#if 1
#if PRINT_REG
    print_tbl();
#endif
    img_param_free_sw_param_buffer();
    img_param_sw = malloc(sizeof(IPRO_IMG_PARAM_SW_DESC_T));//&img_param_sw_buff;//
    memset(img_param_sw, 0, sizeof(IPRO_IMG_PARAM_SW_DESC_T));

    img_param_sw->name      = img_param->name;
    img_param_sw->state_tbl = img_param->state_tbl;
    img_param_sw->ae_tbl    = &img_param->ae_tbl->ae_sw;
    img_param_sw->awb_tbl   = &img_param->awb_tbl->awb_sw;
    img_param_sw->gamma_tbl = &img_param->gamma_corr_tbl->gamma;
    img_param_sw->ct_tbl    = &img_param->color_trans_tbl->Color_Trans;
    img_param_sw->tm_tbl    = &img_param->tm_config_tbl->tm_config;
    img_param_sw->defog_tbl = &img_param->defog_config_tbl->defog_config;
    img_param_sw->cs_tbl    = &img_param->cs_tbl->csCfg;
    img_param_sw->yuvctrl_tbl   = &img_param->yuvctrl_tbl->yuvctrlCfg;
    img_param_sw->yuvadj_tbl    = &img_param->yuvadj_tbl->yuvadjCfg;

#if PRINT_REG
    print_sw_tbl();
#endif
    return 0;

#else
    print_tbl();

    img_param_free_sw_param_buffer();
    img_param_sw = malloc(sizeof(IPRO_IMG_PARAM_SW_DESC_T));//&img_param_sw_buff;//
    memset(img_param_sw, 0, sizeof(IPRO_IMG_PARAM_SW_DESC_T));

    img_param_sw_tbl_init();
    img_param_sw->name = img_param_sw_init->name;
    img_param_sw->state_tbl = &img_param_sw_init->state_tbl;
    img_param_sw->ae_tbl = &img_param_sw_init->ae_tbl;
    img_param_sw->awb_tbl = &img_param_sw_init->awb_tbl;
    img_param_sw->gamma_tbl = &img_param_sw_init->gamma_tbl;
    img_param_sw->ct_tbl = &img_param_sw_init->ct_tbl;
    img_param_sw->tm_tbl = &img_param_sw_init->tm_tbl;
    img_param_sw->defog_tbl = &img_param_sw_init->defog_tbl;
    img_param_sw->cs_tbl = &img_param_sw_init->cs_tbl;
    img_param_sw->yuvctrl_tbl = &img_param_sw_init->yuvctrl_tbl;
    img_param_sw->yuvadj_tbl = &img_param_sw_init->yuvadj_tbl;

    print_sw_tbl();
    return 0;
#endif
#else
    img_param_free_sw_param_buffer();
    img_param_sw = &img_param_sw_buff;//malloc(sizeof(IPRO_IMG_PARAM_SW_DESC_T));//
    memset(img_param_sw, 0, sizeof(IPRO_IMG_PARAM_SW_DESC_T));

    if ((load_inited == 0) || (strcmp(img_param_sw_init->name, "") == 0)) img_param_sw_tbl_init();
    if(strstr(img_param_sw_init->name, sensor_name) != 0) {
        img_param_sw->name = img_param_sw_init->name;
        img_param_sw->state_tbl = &img_param_sw_init->state_tbl;
        img_param_sw->ae_tbl = &img_param_sw_init->ae_tbl;
        img_param_sw->awb_tbl = &img_param_sw_init->awb_tbl;
        img_param_sw->gamma_tbl = &img_param_sw_init->gamma_tbl;
        img_param_sw->ct_tbl = &img_param_sw_init->ct_tbl;
        img_param_sw->tm_tbl = &img_param_sw_init->tm_tbl;
        img_param_sw->defog_tbl = &img_param_sw_init->defog_tbl;
        img_param_sw->cs_tbl = &img_param_sw_init->cs_tbl;
        img_param_sw->yuvctrl_tbl = &img_param_sw_init->yuvctrl_tbl;
        img_param_sw->yuvadj_tbl = &img_param_sw_init->yuvadj_tbl;

        //print_sw_tbl();
        return 0;
    }

    //print_sw_tbl();
    return -1;
#endif
}

int img_param_sw_get_module(const IPRO_IMG_PARAM_SW_DESC_T **module)
{
    if (NULL == img_param_sw) {
        LOGE("img_param sw not init!");
        return -1;
    }

    *module = img_param_sw;
    return 0;
}

bool img_param_sw_get_states(int param_idx)
{
    int index = img_param_get_index("get_states", param_idx, PARAM_IDX_MIN, PARAM_MODULE_IDX_MAX);
    return img_param_sw->state_tbl->state[index];
}

int img_param_sw_set_states(int param_idx, const bool state)
{
    int index = img_param_get_index("set_states", param_idx, PARAM_IDX_MIN, PARAM_MODULE_IDX_MAX);
    img_param_sw->state_tbl->state[index] = state;

    return 0;
}

bool img_param_sw_get_ae_manual_mode(void)
{
    bool isMnaual;
    isMnaual = img_param_sw->ae_tbl->manual_mode;

    return isMnaual;
}

int img_param_sw_set_ae_manual_mode(const bool isMnaual)
{
    img_param_sw->ae_tbl->manual_mode = isMnaual;

    return 0;
}

int img_param_sw_get_ae_manual(ae_param_t **cfg)
{
    *cfg = &img_param_sw->ae_tbl->manual;

    return 0;
}

int img_param_sw_set_ae_manual(const ae_param_t *cfg)
{
    memcpy(&img_param_sw->ae_tbl->manual, cfg, sizeof(ae_param_t));

    return 0;
}

int img_param_sw_get_ae(int param_idx, ae_sub_attr **cfg)
{
    int index = img_param_get_index("get_ae", param_idx, PARAM_IDX_MIN, PARAM_AE_TARGET_IDX_MAX);
    *cfg = &img_param_sw->ae_tbl->container[index];

    return 0;
}

int img_param_sw_set_ae(int param_idx, const ae_sub_attr *cfg)
{
    int index = img_param_get_index("set_ae", param_idx, PARAM_IDX_MIN, PARAM_AE_TARGET_IDX_MAX);
    memcpy(&img_param_sw->ae_tbl->container[index], cfg, sizeof(ae_sub_attr));

    return 0;
}

/*int img_param_sw_get_ae_highlight_weight_en(int *en)
{
    *en = img_param_sw->ae_tbl->highlight_weight_conf.enable;

    return 0;
}

int img_param_sw_set_ae_highlight_weight_en(const int en)
{
    img_param_sw->ae_tbl->highlight_weight_conf.enable = en;
    return 0;
}

int img_param_sw_get_ae_highlight_weight(int param_idx, ae_hightlight_weight **cfg)
{
    int index = img_param_get_index(param_idx, PARAM_IDX_MIN, PARAM_AE_TARGET_IDX_MAX);
    *cfg = &img_param_sw->ae_tbl->highlight_weight_conf.highlight_weight[index];

    return 0;
}

int img_param_sw_set_ae_highlight_weight(int param_idx, const ae_hightlight_weight *cfg)
{
    int index = img_param_get_index(param_idx, PARAM_IDX_MIN, PARAM_AE_TARGET_IDX_MAX);
    memcpy(&img_param_sw->ae_tbl->highlight_weight_conf.highlight_weight[index], cfg, sizeof(ae_hightlight_weight));

    return 0;
}*/

int img_param_sw_get_ae_metering_tbl(int param_idx, ae_metering_tbl_t **cfg)
{
    int index = img_param_get_index("get_ae_metering_tbl", param_idx, PARAM_IDX_MIN, PARAM_AE_METERING_IDX_MAX);

    if (index == AE_METERING_CUSTOM) {
        *cfg = &img_param_sw->ae_tbl->metering_tbl;
    } else {
        *cfg = &g_ae_metering_tbl[index];
    }
    return 0;
}

int img_param_sw_set_ae_metering_tbl(const ae_metering_tbl_t *cfg)
{
    memcpy(&img_param_sw->ae_tbl->metering_tbl, cfg, sizeof(ae_metering_tbl_t));

    return 0;
}

/*int img_param_sw_get_ae_highlight_weight_conf(ae_hightlight_weight_conf **cfg)
{
    *cfg = &img_param_sw->ae_tbl->highlight_weight_conf;

    return 0;
}

int img_param_sw_set_ae_highlight_weight_conf(const ae_hightlight_weight_conf *cfg)
{
    memcpy(&img_param_sw->ae_tbl->highlight_weight_conf, cfg, sizeof(ae_hightlight_weight_conf));

    return 0;
}
*/
int img_param_sw_get_ae_metering_mode(AE_METERING *cfg)
{
    *cfg = img_param_sw->ae_tbl->metering_mode;

    return 0;
}

int img_param_sw_set_ae_metering_mode(const AE_METERING cfg)
{
    int index = img_param_get_index("set_ae_metering_model", cfg, PARAM_IDX_MIN, PARAM_AE_METERING_IDX_MAX);
    memcpy(&img_param_sw->ae_tbl->metering_mode, &index, sizeof(uint8_t));

    return 0;
}

/*int img_param_sw_get_ae_speed_level(int *cfg)
{
    *cfg = img_param_sw->ae_tbl->speed_level;

    return 0;
}

int img_param_sw_set_ae_speed_level(const int cfg)
{
    int index = img_param_get_index(cfg, PARAM_IDX_MIN, PARAM_AE_SPEED_LEVEL_MAX);
    memcpy(&img_param_sw->ae_tbl->speed_level, &index, sizeof(uint8_t));

    return 0;
}
*/
bool img_param_sw_get_awb_manual_mode(void)
{
    bool isMnaual;
    isMnaual = img_param_sw->awb_tbl->manual_mode;

    return isMnaual;
}

int img_param_sw_set_awb_manual_mode(const bool isMnaual)
{
    img_param_sw->awb_tbl->manual_mode = isMnaual;

    return 0;
}

int img_param_sw_get_awb_manual(awb_param_t **cfg)
{
    *cfg = &img_param_sw->awb_tbl->manual;

    return 0;
}

int img_param_sw_set_awb_manual(const awb_param_t *cfg)
{
    memcpy(&img_param_sw->awb_tbl->manual, cfg, sizeof(awb_param_t));

    return 0;
}

/*int img_param_sw_get_awb_ct(awb_ct_param_t **cfg)
{
    *cfg = &img_param_sw->awb_tbl->ct;

    return 0;
}

int img_param_sw_set_awb_ct(const awb_ct_param_t *cfg)
{
    memcpy(&img_param_sw->awb_tbl->ct, cfg, sizeof(awb_ct_param_t));

    return 0;
}

int img_param_sw_get_awb_preference_en(int *en)
{
    *en = img_param_sw->awb_tbl->preference_enable;

    return 0;
}

int img_param_sw_set_awb_preference_en(const int en)
{
    img_param_sw->awb_tbl->preference_enable = en;

    return 0;
}*/

int img_param_sw_get_awb_special_color_en(int *en)
{
    *en = img_param_sw->awb_tbl->special_color_enable;

    return 0;
}

int img_param_sw_set_awb_special_color_en(const int en)
{
    img_param_sw->awb_tbl->special_color_enable = en;

    return 0;
}

/*int img_param_sw_get_awb_preference_gain(int param_idx, awb_bais_t **cfg)
{
    int index = img_param_get_index(param_idx, PARAM_IDX_MIN, PARAM_AWB_PREF_IDX_MAX);
    *cfg = &img_param_sw->awb_tbl->preference_gain[index];

    return 0;
}

int img_param_sw_set_awb_preference_gain(int param_idx, const awb_bais_t *cfg)
{
    int index = img_param_get_index(param_idx, PARAM_IDX_MIN, PARAM_AWB_PREF_IDX_MAX);
    memcpy(&img_param_sw->awb_tbl->preference_gain[index], cfg, sizeof(awb_bais_t));

    return 0;
}*/

int img_param_sw_get_awb_special_color(int param_idx, awb_special_color_t **cfg)
{
    int index = img_param_get_index("get_awb_special_color", param_idx, PARAM_IDX_MIN, MAX_ENTRY_NUM_AWB3_SPECIAL_COLOR);
    *cfg = &img_param_sw->awb_tbl->special_color_info[index];

    return 0;
}

int img_param_sw_set_awb_special_color(int param_idx, const awb_special_color_t *cfg)
{
    int index = img_param_get_index("set_awb_special_color", param_idx, PARAM_IDX_MIN, MAX_ENTRY_NUM_AWB3_SPECIAL_COLOR);
    memcpy(&img_param_sw->awb_tbl->special_color_info[index], cfg, sizeof(awb_special_color_t));

    return 0;
}

/*int img_param_sw_get_awb_speed_level(int *cfg)
{
    *cfg = img_param_sw->awb_tbl->speed_level;

    return 0;
}

int img_param_sw_set_awb_speed_level(const int cfg)
{
    int index = img_param_get_index(cfg, PARAM_IDX_MIN, PARAM_AWB_SPEED_LEVEL_MAX);
    memcpy(&img_param_sw->awb_tbl->speed_level, &index, sizeof(uint8_t));

    return 0;
}*/

int img_param_sw_get_gamma_curve_mode(GAMMA_CURVE_MODE_E *mode)
{
    *mode = img_param_sw->gamma_tbl->mode;

    return 0;
}

int img_param_sw_set_gamma_curve_mode(const GAMMA_CURVE_MODE_E mode)
{
    img_param_sw->gamma_tbl->mode = mode;

    return 0;
}


int img_param_sw_get_gamma(gamma_curve_t **cfg)
{
    *cfg = &img_param_sw->gamma_tbl->curve;

    return 0;
}

int img_param_sw_set_gamma(const gamma_curve_t *cfg)
{
    memcpy(&img_param_sw->gamma_tbl->curve, cfg, sizeof(gamma_curve_t));

    return 0;
}


int img_param_sw_get_ct_color_gamut(CT_GAMUT_E *color_gamut)
{
    *color_gamut = img_param_sw->ct_tbl->color_gamut;

    return 0;
}

int img_param_sw_set_ct_color_gamut(const CT_GAMUT_E color_gamut)
{
    img_param_sw->ct_tbl->color_gamut = color_gamut;

    return 0;
}

int img_param_sw_get_ct_manual(ct_param_t **cfg)
{
    *cfg = &img_param_sw->ct_tbl->ct;

    return 0;
}

int img_param_sw_set_ct_manual(const ct_param_t *cfg)
{
    memcpy(&img_param_sw->ct_tbl->ct, cfg, sizeof(ct_param_t));

    return 0;
}

bool img_param_sw_get_tm_manual_mode(void)
{
    bool isMnaual;
    isMnaual = img_param_sw->tm_tbl->manual_mode;

    return isMnaual;
}

int img_param_sw_set_tm_manual_mode(const bool isMnaual)
{
    img_param_sw->tm_tbl->manual_mode = isMnaual;

    return 0;
}

int img_param_sw_get_tm_type(TM_TYPE_E *type)
{
    *type = img_param_sw->tm_tbl->type;

    return 0;
}

int img_param_sw_set_tm_type(const TM_TYPE_E type)
{
    img_param_sw->tm_tbl->type = type;

    return 0;
}

int img_param_sw_get_tm_manual(tm_param_t **cfg)
{
    *cfg = &img_param_sw->tm_tbl->manual;

    return 0;
}

int img_param_sw_set_tm_manual(const tm_param_t *cfg)
{
    memcpy(&img_param_sw->tm_tbl->manual, cfg, sizeof(tm_param_t));

    return 0;
}

int img_param_sw_get_tm(int param_idx, tm_param_t **cfg)
{
    int index = img_param_get_index("get_tm", param_idx, PARAM_IDX_MIN, PARAM_TM_IDX_MAX);
    *cfg = &img_param_sw->tm_tbl->container[index];

    return 0;
}

int img_param_sw_set_tm(int param_idx, const tm_param_t *cfg)
{
    int index = img_param_get_index("set_tm", param_idx, PARAM_IDX_MIN, PARAM_TM_IDX_MAX);
    memcpy(&img_param_sw->tm_tbl->container[index], cfg, sizeof(tm_param_t));

    return 0;
}

int img_param_sw_get_tm_manual_curve(tm_curve_t **cfg)
{
    *cfg = &img_param_sw->tm_tbl->manual_curve;

    return 0;
}

int img_param_sw_set_tm_manual_curve(const tm_curve_t *cfg)
{
    memcpy(&img_param_sw->tm_tbl->manual_curve, cfg, sizeof(tm_curve_t));

    return 0;
}

int img_param_sw_get_tm_auto_curve(int param_idx, tm_curve_t **cfg)
{
    int index = img_param_get_index("get_tm_auto_curve",param_idx, PARAM_IDX_MIN, PARAM_TM_IDX_MAX);
    *cfg = &img_param_sw->tm_tbl->auto_curve[index];

    return 0;
}

int img_param_sw_set_tm_auto_curve(int param_idx, const tm_curve_t *cfg)
{
    int index = img_param_get_index("set_tm_auto_curve",param_idx, PARAM_IDX_MIN, PARAM_TM_IDX_MAX);
    memcpy(&img_param_sw->tm_tbl->auto_curve[index], cfg, sizeof(tm_curve_t));

    return 0;
}

bool img_param_sw_get_defog_manual_mode(void)
{
    bool isMnaual;
    isMnaual = img_param_sw->defog_tbl->manual_mode;

    return isMnaual;
}

int img_param_sw_set_defog_manual_mode(const bool isMnaual)
{
    img_param_sw->defog_tbl->manual_mode = isMnaual;

    return 0;
}

int img_param_sw_get_defog_manual(defog_param_t **cfg)
{
    *cfg = &img_param_sw->defog_tbl->manual;

    return 0;
}

int img_param_sw_set_defog_manual(const defog_param_t *cfg)
{
    memcpy(&img_param_sw->defog_tbl->manual, cfg, sizeof(defog_param_t));

    return 0;
}

int img_param_sw_get_defog(int param_idx, defog_param_t **cfg)
{
    int index = img_param_get_index("get_defog", param_idx, PARAM_IDX_MIN, PARAM_DEFOG_IDX_MAX);
    *cfg = &img_param_sw->defog_tbl->container[index];

    return 0;
}

int img_param_sw_set_defog(int param_idx, const defog_param_t *cfg)
{
    int index = img_param_get_index("set_defog", param_idx, PARAM_IDX_MIN, PARAM_DEFOG_IDX_MAX);
    memcpy(&img_param_sw->defog_tbl->container[index], cfg, sizeof(defog_param_t));

    return 0;
}


bool img_param_sw_get_cs_manual_mode(void)
{
    bool isMnaual;
    isMnaual = img_param_sw->cs_tbl->manual_mode;

    return isMnaual;
}

int img_param_sw_set_cs_manual_mode(const bool isMnaual)
{
    img_param_sw->cs_tbl->manual_mode = isMnaual;

    return 0;
}

int img_param_sw_get_cs_manual(cs_param_t **cfg)
{
    *cfg = &img_param_sw->cs_tbl->manual;

    return 0;
}

int img_param_sw_set_cs_manual(const cs_param_t *cfg)
{
    memcpy(&img_param_sw->cs_tbl->manual, cfg, sizeof(cs_param_t));

    return 0;
}

int img_param_sw_get_cs(int param_idx, cs_param_t **cfg)
{
    int index = img_param_get_index("get_cs", param_idx, PARAM_IDX_MIN, PARAM_CS_IDX_MAX);
    *cfg = &img_param_sw->cs_tbl->container[index];

    return 0;
}

int img_param_sw_set_cs(int param_idx, const cs_param_t *cfg)
{
    int index = img_param_get_index("set_cs", param_idx, PARAM_IDX_MIN, PARAM_CS_IDX_MAX);
    memcpy(&img_param_sw->cs_tbl->container[index], cfg, sizeof(cs_param_t));

    return 0;
}

int img_param_sw_get_cs_trnas_tbl(int param_idx, cs_trans_tbl_t **cfg)
{
    int index = img_param_get_index("get_cs_trnas_tbl", param_idx, PARAM_IDX_MIN, PARAM_CS_TRANS_IDX_MAX);
    *cfg = &img_param_sw->cs_tbl->trans_tbl[index];

    return 0;
}

int img_param_sw_set_cs_trnas_tbl(int param_idx, const cs_trans_tbl_t *cfg)
{
    int index = img_param_get_index("set_cs_trnas_tbl", param_idx, PARAM_IDX_MIN, PARAM_CS_TRANS_IDX_MAX);
    memcpy(&img_param_sw->cs_tbl->trans_tbl[index], cfg, sizeof(cs_trans_tbl_t));

    return 0;
}


bool img_param_sw_get_yuvctrl_manual_mode(void)
{
    bool isMnaual;
    isMnaual = img_param_sw->yuvctrl_tbl->manual_mode;

    return isMnaual;
}

int img_param_sw_set_yuvctrl_manual_mode(const bool isMnaual)
{
    img_param_sw->yuvctrl_tbl->manual_mode = isMnaual;

    return 0;
}

int img_param_sw_get_yuvctrl_manual(yuvctrl_param_t **cfg)
{
    *cfg = &img_param_sw->yuvctrl_tbl->manual;

    return 0;
}

int img_param_sw_set_yuvctrl_manual(const yuvctrl_param_t *cfg)
{
    memcpy(&img_param_sw->yuvctrl_tbl->manual, cfg, sizeof(yuvctrl_param_t));

    return 0;
}

int img_param_sw_get_yuvctrl(int param_idx, yuvctrl_param_t **cfg)
{
    int index = img_param_get_index("get_yuvctrl", param_idx, PARAM_IDX_MIN, PARAM_YUVCTRL_IDX_MAX);
    *cfg = &img_param_sw->yuvctrl_tbl->container[index];
    return 0;
}

int img_param_sw_set_yuvctrl(int param_idx, const yuvctrl_param_t *cfg)
{
    int index = img_param_get_index("set_yuvctrl", param_idx, PARAM_IDX_MIN, PARAM_YUVCTRL_IDX_MAX);
    memcpy(&img_param_sw->yuvctrl_tbl->container[index], cfg, sizeof(yuvctrl_param_t));
    return 0;
}

int img_param_sw_get_yuvadj_mode(YUVADJ_RANGE_MODE_E *mode)
{
    *mode = img_param_sw->yuvadj_tbl->mode;

    return 0;
}

int img_param_sw_set_yuvadj_mode(const YUVADJ_RANGE_MODE_E mode)
{
    img_param_sw->yuvadj_tbl->mode = mode;

    return 0;
}

int img_param_sw_get_yuvadj_manual(yuvadj_param_t **cfg)
{
    *cfg = &img_param_sw->yuvadj_tbl->container;

    return 0;
}

int img_param_sw_set_yuvadj_manual(const yuvadj_param_t *cfg)
{
    memcpy(&img_param_sw->yuvadj_tbl->container, cfg, sizeof(yuvadj_param_t));

    return 0;
}


void print_sw_tbl(void)
{
    int i,j;

    printf("[OK] img_param_sw_load name %s\r\n", img_param_sw->name);

    for (i = 0; i < ISP_ADJ_PARAM_NUM; i++) {
        int val = img_param_sw->state_tbl->state[i];
        printf("[OK] img_param_sw_get_states cfg[%d]: %d,\r\n", i, val);
    }

    for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
        printf("[OK] img_param_get_ae_sw cfg[%d]: %lu\r\n", i, img_param_sw->ae_tbl->container[i].ae_target);
    }

    printf("[OK] img_param_get_ae_sw metering_tbl:\r\n");
    for (j = 0; j < AE_STATS_GRID_NUM_Y; j++) {
        for (i = 0; i < AE_STATS_GRID_NUM_X; i++) {
            printf("%u, ", img_param_sw->ae_tbl->metering_tbl.coef[j * AE_STATS_GRID_NUM_X + i]);
        }
        printf("\r\n");
    }

    /*printf("[OK] img_param_get_ae_sw highlight weight en: %d,\r\n", img_param_sw->ae_tbl->highlight_weight_conf.enable);
    for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
        printf("[OK] img_param_get_ae_sw highlight weight[%d]: %u, %u, %u,\r\n", i,
            img_param_sw->ae_tbl->highlight_weight_conf.highlight_weight[i].idx[0],
            img_param_sw->ae_tbl->highlight_weight_conf.highlight_weight[i].idx[1],
            img_param_sw->ae_tbl->highlight_weight_conf.highlight_weight[i].idx[2]);
    }
    printf("[OK] img_param_get_ae_sw highlight_weight conf: %u, %u, {%u, %u, %u, %u,}\r\n", 
        img_param_sw->ae_tbl->highlight_weight_conf.roi_start,img_param_sw->ae_tbl->highlight_weight_conf.roi_end,
        img_param_sw->ae_tbl->highlight_weight_conf.thr[0],img_param_sw->ae_tbl->highlight_weight_conf.thr[1],
        img_param_sw->ae_tbl->highlight_weight_conf.thr[2],img_param_sw->ae_tbl->highlight_weight_conf.thr[3]);*/
    printf("[OK] img_param_get_ae_sw metering mode: %u,\r\n", img_param_sw->ae_tbl->metering_mode);
    //printf("[OK] img_param_get_ae_sw speed_levl: %u,\r\n", img_param_sw->ae_tbl->speed_level);

    //printf("[OK] img_param_get_awb_sw preference en: %u,\r\n", img_param_sw->awb_tbl->preference_enable);
    printf("[OK] img_param_get_awb_sw special_color en: %u,\r\n", img_param_sw->awb_tbl->special_color_enable);
    /*for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
        printf("[OK] img_param_get_awb_sw preference_gain[%d]: (%d, %d), (%d, %d), (%d, %d)\r\n", i,
            img_param_sw->awb_tbl->preference_gain[i].bais[0].r_gain,img_param_sw->awb_tbl->preference_gain[i].bais[0].b_gain,
            img_param_sw->awb_tbl->preference_gain[i].bais[1].r_gain,img_param_sw->awb_tbl->preference_gain[i].bais[1].b_gain,
            img_param_sw->awb_tbl->preference_gain[i].bais[2].r_gain,img_param_sw->awb_tbl->preference_gain[i].bais[2].b_gain
            );
    }*/
    for (i = 0; i < MAX_ENTRY_NUM_AWB3_SPECIAL_COLOR; i++) {
        printf("[OK] img_param_get_awb_sw special color conf [%d]: %d, %d %d\r\n", i,
            img_param_sw->awb_tbl->special_color_info[i].r_gain,img_param_sw->awb_tbl->special_color_info[i].b_gain,
            img_param_sw->awb_tbl->special_color_info[i].weight);
    }
    //printf("[OK] img_param_get_awb_sw speed_levl: %u,\r\n", img_param_sw->awb_tbl->speed_level);
    

    printf("[OK] img_param_get_awb_sw manual_mode: %d,\r\n", img_param_sw->awb_tbl->manual_mode);
    printf("[OK] img_param_get_awb_sw manual: %u,%u,%u,%u,\r\n", 
        img_param_sw->awb_tbl->manual.r_gain, img_param_sw->awb_tbl->manual.gr_gain, img_param_sw->awb_tbl->manual.gb_gain, img_param_sw->awb_tbl->manual.b_gain);
    printf("\r\n");

    printf("[OK] img_param_get_gamma_sw gamma_curve_mode: %d,\r\n", img_param_sw->gamma_tbl->mode);
    printf("[OK] img_param_get_gamma curve \r\n");
    for (i = 0; i < MAX_ENTRY_NUM_GAMMA_COEF_NUM; i++) {
        printf(" %d,", img_param_sw->gamma_tbl->curve.coef[i]);
    }
    printf("\r\n");

    printf("[OK] img_param_get_ct_sw color_gamut: %d,\r\n", img_param_sw->ct_tbl->color_gamut);
    printf("[OK] img_param_get_ct_sw coef :");
    for (j = 0; j < MAX_ENTRY_CT_COEF_NUM; j++) {
        printf(" %d,", img_param_sw->ct_tbl->ct.coef[j]);
    }
    printf("\r\n");
    printf("[OK] img_param_get_ct_sw pre_offset :");
    for (j = 0; j < MAX_ENTRY_CT_OFFSET_NUM; j++) {
        printf(" %d,", img_param_sw->ct_tbl->ct.pre_offset[j]);
    }
    printf("\r\n");
    printf("[OK] img_param_get_ct_sw post_offset :");
    for (j = 0; j < MAX_ENTRY_CT_OFFSET_NUM; j++) {
        printf(" %d,",  img_param_sw->ct_tbl->ct.post_offset[j]);
    }
    printf("\r\n");

    printf("[OK] img_param_get_tone type: %d  \r\n", img_param_sw->tm_tbl->type);
    printf("[OK] img_param_get_tone manual mode: %d  \r\n", img_param_sw->tm_tbl->manual_mode);
    printf("[OK] img_param_get_tm_sw manual:  %ld, %ld, {%ld, %ld, %ld,}, %ld, {%ld, %ld, %ld,} \r\n", 
        img_param_sw->tm_tbl->manual.curve_weight,
        img_param_sw->tm_tbl->manual.dark_max_ratio,
        img_param_sw->tm_tbl->manual.dark_weight[0],
        img_param_sw->tm_tbl->manual.dark_weight[1],
        img_param_sw->tm_tbl->manual.dark_weight[2],
        img_param_sw->tm_tbl->manual.bright_max_ratio,
        img_param_sw->tm_tbl->manual.bright_weight[0],
        img_param_sw->tm_tbl->manual.bright_weight[1],
        img_param_sw->tm_tbl->manual.bright_weight[2]);

    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_tm_sw cfg[%d]:  %ld, %ld, {%ld, %ld, %ld,}, %ld, {%ld, %ld, %ld,} \r\n", i,
            img_param_sw->tm_tbl->container[i].curve_weight,
            img_param_sw->tm_tbl->container[i].dark_max_ratio,
            img_param_sw->tm_tbl->container[i].dark_weight[0],
            img_param_sw->tm_tbl->container[i].dark_weight[1],
            img_param_sw->tm_tbl->container[i].dark_weight[2],
            img_param_sw->tm_tbl->container[i].bright_max_ratio,
            img_param_sw->tm_tbl->container[i].bright_weight[0],
            img_param_sw->tm_tbl->container[i].bright_weight[1],
            img_param_sw->tm_tbl->container[i].bright_weight[2]);
    }

    printf("[OK] img_param_get_tone manual curve \r\n");
    for (j = 0; j < MAX_ENTRY_NUM_TM_CURVE_COEF_NUM; j++) {
        printf(" %d,", img_param_sw->tm_tbl->manual_curve.coef[j]);
    }
    printf("\r\n");
    

    /*for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_tone auto curve[%d]: \r\n", i);
        for (j = 0; j < MAX_ENTRY_NUM_TM_CURVE_COEF_NUM; j++) {
            printf(" %d,", img_param_sw->tm_tbl->auto_curve[i].coef[j]);
        }
        printf("\r\n");
    }**/
    printf("\r\n");

    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_defog_sw cfg[%d]: %ld, %ld, %ld, \r\n", i,
            img_param_sw->defog_tbl->container[i].enh_weight,
            img_param_sw->defog_tbl->container[i].bright_enh_level,
            img_param_sw->defog_tbl->container[i].dark_enh_level
            );
    }
    printf("\r\n");

    printf("[OK] img_param_get_cs_sw manual mode %d,\r\n", img_param_sw->cs_tbl->manual_mode);
    printf("[OK] img_param_get_cs_sw manual cfg: %d,\r\n", img_param_sw->cs_tbl->manual.strength);
    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_cs_sw cfg[%d]: %d\r\n", i, img_param_sw->cs_tbl->container[i].strength);
    }
    for (i = 0; i < MAX_ENTRY_CS_TRANS_TBL_NUM; i++) {
        printf("[OK] img_param_get_cs_trans cfg[%d]: %d, %d, %d\r\n", i, 
            img_param_sw->cs_tbl->trans_tbl[i].strength,
            img_param_sw->cs_tbl->trans_tbl[i].weight,
            img_param_sw->cs_tbl->trans_tbl[i].grey_thr);
    }

    #if 1
    printf("[OK] img_param_get_yuvctrl_sw manual mode %d,\r\n", img_param_sw->yuvctrl_tbl->manual_mode);
    printf("[OK] img_param_get_yuvctrl_sw manual cfg: %d,%d,%d\r\n", 
        img_param_sw->yuvctrl_tbl->manual.brightness,
        img_param_sw->yuvctrl_tbl->manual.contrast,
        img_param_sw->yuvctrl_tbl->manual.saturation
    );
    
    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_yuvctrl_sw cfg[%d]: %d,%d,%d\r\n", i, 
            img_param_sw->yuvctrl_tbl->container[i].brightness,
            img_param_sw->yuvctrl_tbl->container[i].contrast,
            img_param_sw->yuvctrl_tbl->container[i].saturation
            );
    }
    printf("\r\n");
    #else
    printf("[OK] img_param_get_yuvctrl_sw manual mode %d,\r\n", img_param_sw->yuvctrl_tbl->manual_mode);
    printf("[OK] img_param_get_yuvctrl_sw manual cfg: %d,%d,%d,%d,%d,%d,%d,\r\n", 
        img_param_sw->yuvctrl_tbl->manual.brightness,
        img_param_sw->yuvctrl_tbl->manual.contrast,
        img_param_sw->yuvctrl_tbl->manual.saturation,
        img_param_sw->yuvctrl_tbl->manual.specialMode,
        img_param_sw->yuvctrl_tbl->manual.sepiaCb,
        img_param_sw->yuvctrl_tbl->manual.sepiaCr,
        img_param_sw->yuvctrl_tbl->manual.solarThred
    );
    
    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_yuvctrl_sw cfg[%d]: %d,%d,%d,%d,%d,%d,%d,\r\n", i, 
            img_param_sw->yuvctrl_tbl->container[i].brightness,
            img_param_sw->yuvctrl_tbl->container[i].contrast,
            img_param_sw->yuvctrl_tbl->container[i].saturation,
            img_param_sw->yuvctrl_tbl->container[i].specialMode,
            img_param_sw->yuvctrl_tbl->container[i].sepiaCb,
            img_param_sw->yuvctrl_tbl->container[i].sepiaCr,
            img_param_sw->yuvctrl_tbl->container[i].solarThred
            );
    }
    printf("\r\n");
    #endif
    
    printf("[OK] img_param_get_yuvadj_sw mode %d,\r\n", img_param_sw->yuvadj_tbl->mode);
    printf("[OK] img_param_get_yuvadj_sw cfg: %d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n", 
        img_param_sw->yuvadj_tbl->container.y_luma,
        img_param_sw->yuvadj_tbl->container.y_mul0,
        img_param_sw->yuvadj_tbl->container.y_mul1,
        img_param_sw->yuvadj_tbl->container.y_min,
        img_param_sw->yuvadj_tbl->container.y_max,
        img_param_sw->yuvadj_tbl->container.uv_mul0,
        img_param_sw->yuvadj_tbl->container.uv_mul1,
        img_param_sw->yuvadj_tbl->container.uv_min,
        img_param_sw->yuvadj_tbl->container.uv_max
        );
    printf("\r\n");

}

void print_tbl(void)
{
    int i;//,j,x;

    printf("[OK] img_param_sw_load name %s\r\n", img_param->name);

    for (i = 0; i < ISP_ADJ_PARAM_NUM; i++) {
        int val = img_param->state_tbl->state[i];
        printf("[OK] img_param_sw_get_states cfg[%d]: %d,\r\n", i, val);
    }
#if 0
    for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
        printf("[OK] img_param_get_ae_sw cfg[%d]: %lu\r\n", i, img_param_sw->ae_tbl->container[i].ae_target);
    }

    for (x = 0; x < AE_METERING_TBL_NUM; x++) {
        printf("[OK] img_param_get_ae_sw metering_tbl[%d]:\r\n", x);
        for (j = 0; j < AE_STATS_GRID_NUM_Y; j++) {
            for (i = 0; i < AE_STATS_GRID_NUM_X; i++) {
                printf("%u, ", img_param_sw->ae_tbl->metering_tbl[x].coef[j * AE_STATS_GRID_NUM_X + i]);
            }
            printf("\r\n");
        }
    }
    printf("[OK] img_param_get_ae_sw highlight weight en: %d,\r\n", img_param_sw->ae_tbl->highlight_weight_conf.enable);
    for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
        printf("[OK] img_param_get_ae_sw highlight weight[%d]: %u, %u, %u,\r\n", i,
            img_param_sw->ae_tbl->highlight_weight_conf.highlight_weight[i].idx[0],
            img_param_sw->ae_tbl->highlight_weight_conf.highlight_weight[i].idx[1],
            img_param_sw->ae_tbl->highlight_weight_conf.highlight_weight[i].idx[2]);
    }
    printf("[OK] img_param_get_ae_sw highlight_weight conf: %u, %u, {%u, %u, %u, %u,}\r\n", 
        img_param_sw->ae_tbl->highlight_weight_conf.roi_start,img_param_sw->ae_tbl->highlight_weight_conf.roi_end,
        img_param_sw->ae_tbl->highlight_weight_conf.thr[0],img_param_sw->ae_tbl->highlight_weight_conf.thr[1],
        img_param_sw->ae_tbl->highlight_weight_conf.thr[2],img_param_sw->ae_tbl->highlight_weight_conf.thr[3]);
    printf("[OK] img_param_get_ae_sw metering mode: %u,\r\n", img_param_sw->ae_tbl->metering_mode);
    printf("[OK] img_param_get_ae_sw speed_levl: %u,\r\n", img_param_sw->ae_tbl->speed_level);

    printf("[OK] img_param_get_awb_sw preference en: %u,\r\n", img_param_sw->awb_tbl->preference_enable);
    printf("[OK] img_param_get_awb_sw special_color en: %u,\r\n", img_param_sw->awb_tbl->special_color_enable);
    for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
        printf("[OK] img_param_get_awb_sw preference_gain[%d]: (%d, %d), (%d, %d), (%d, %d)\r\n", i,
            img_param_sw->awb_tbl->preference_gain[i].bais[0].r_gain,img_param_sw->awb_tbl->preference_gain[i].bais[0].b_gain,
            img_param_sw->awb_tbl->preference_gain[i].bais[1].r_gain,img_param_sw->awb_tbl->preference_gain[i].bais[1].b_gain,
            img_param_sw->awb_tbl->preference_gain[i].bais[2].r_gain,img_param_sw->awb_tbl->preference_gain[i].bais[2].b_gain
            );
    }
    for (i = 0; i < MAX_ENTRY_NUM_AWB3_SPECIAL_COLOR; i++) {
        printf("[OK] img_param_get_awb_sw special color conf [%d]: %d, %d %d\r\n", i,
            img_param_sw->awb_tbl->special_color_info[i].r_gain,img_param_sw->awb_tbl->special_color_info[i].b_gain,
            img_param_sw->awb_tbl->special_color_info[i].weight);
    }
    printf("[OK] img_param_get_awb_sw speed_levl: %u,\r\n", img_param_sw->awb_tbl->speed_level);
    

    printf("[OK] img_param_get_awb_sw manual_mode: %d,\r\n", img_param_sw->awb_tbl->manual_mode);
    printf("[OK] img_param_get_awb_sw manual: %u,%u,%u,%u,\r\n", 
        img_param_sw->awb_tbl->manual.r_gain, img_param_sw->awb_tbl->manual.gr_gain, img_param_sw->awb_tbl->manual.gb_gain, img_param_sw->awb_tbl->manual.b_gain);
    printf("\r\n");

    printf("[OK] img_param_get_gamma manual: \r\n");
    for (i = 0; i < MAX_ENTRY_NUM_GAMMA_COEF_NUM; i++) {
        printf(" %d,", img_param_sw->gamma_tbl->curve.coef[i]);
    }
    printf("\r\n");

    printf("[OK] img_param_get_ct_sw color_gamut: %d,\r\n", img_param_sw->ct_tbl->color_gamut);
    for (j = 0; j < MAX_ENTRY_CT_COEF_NUM; j++) {
        printf(" %d,", img_param_sw->ct_tbl->ct.coef[j]);
    }
    for (j = 0; j < MAX_ENTRY_CT_OFFSET_NUM; j++) {
        printf(" %d,    %d", img_param_sw->ct_tbl->ct.pre_offset[j], img_param_sw->ct_tbl->ct.post_offset[j]);
    }
    printf("\r\n");

    printf("[OK] img_param_get_tone manual mode: %d  \r\n", img_param_sw->tm_tbl->manual_mode);
    printf("[OK] img_param_get_tm_sw manual:  %ld, %ld, {%ld, %ld, %ld,}, %ld, {%ld, %ld, %ld,} \r\n", 
        img_param_sw->tm_tbl->manual.curve_weight,
        img_param_sw->tm_tbl->manual.dark_max_ratio,
        img_param_sw->tm_tbl->manual.dark_weight[0],
        img_param_sw->tm_tbl->manual.dark_weight[1],
        img_param_sw->tm_tbl->manual.dark_weight[2],
        img_param_sw->tm_tbl->manual.bright_max_ratio,
        img_param_sw->tm_tbl->manual.bright_weight[0],
        img_param_sw->tm_tbl->manual.bright_weight[1],
        img_param_sw->tm_tbl->manual.bright_weight[2]);

    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_tm_sw cfg[%d]:  %ld, %ld, {%ld, %ld, %ld,}, %ld, {%ld, %ld, %ld,} \r\n", i,
            img_param_sw->tm_tbl->container[i].curve_weight,
            img_param_sw->tm_tbl->container[i].dark_max_ratio,
            img_param_sw->tm_tbl->container[i].dark_weight[0],
            img_param_sw->tm_tbl->container[i].dark_weight[1],
            img_param_sw->tm_tbl->container[i].dark_weight[2],
            img_param_sw->tm_tbl->container[i].bright_max_ratio,
            img_param_sw->tm_tbl->container[i].bright_weight[0],
            img_param_sw->tm_tbl->container[i].bright_weight[1],
            img_param_sw->tm_tbl->container[i].bright_weight[2]);
    }

    printf("[OK] img_param_get_tone tm_type: %d  \r\n", img_param_sw->tm_tbl->tm_type);
    printf("[OK] img_param_get_tone manual curve \r\n");
    for (j = 0; j < MAX_ENTRY_NUM_TM_CURVE_COEF_NUM; j++) {
        printf(" %d,", img_param_sw->tm_tbl->manual_curve.coef[j]);
    }
    printf("\r\n");
    

    /*for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_tone auto curve[%d]: \r\n", i);
        for (j = 0; j < MAX_ENTRY_NUM_TM_CURVE_COEF_NUM; j++) {
            printf(" %d,", img_param_sw->tm_tbl->auto_curve[i].coef[j]);
        }
        printf("\r\n");
    }**/
    printf("\r\n");

    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_defog_sw cfg[%d]: %ld, %ld, %ld, \r\n", i,
            img_param_sw->defog_tbl->container[i].enh_weight,
            img_param_sw->defog_tbl->container[i].bright_enh_level,
            img_param_sw->defog_tbl->container[i].dark_enh_level
            );
    }
    printf("\r\n");

    printf("[OK] img_param_get_cs_sw manual mode %d,\r\n", img_param_sw->cs_tbl->manual_mode);
    printf("[OK] img_param_get_cs_sw manual cfg: %d,\r\n", img_param_sw->cs_tbl->manual.strength);
    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_cs_sw cfg[%d]: %d\r\n", i, img_param_sw->cs_tbl->container[i].strength);
    }
    for (i = 0; i < MAX_ENTRY_CS_TRANS_TBL_NUM; i++) {
        printf("[OK] img_param_get_cs_trans cfg[%d]: %d, %d, %d\r\n", i, 
            img_param_sw->cs_tbl->trans_tbl[i].strength,
            img_param_sw->cs_tbl->trans_tbl[i].weight,
            img_param_sw->cs_tbl->trans_tbl[i].grey_thr);
    }

    #if 1
    printf("[OK] img_param_get_yuvctrl_sw manual mode %d,\r\n", img_param_sw->yuvctrl_tbl->manual_mode);
    printf("[OK] img_param_get_yuvctrl_sw manual cfg: %d,%d,%d\r\n", 
        img_param_sw->yuvctrl_tbl->manual.brightness,
        img_param_sw->yuvctrl_tbl->manual.contrast,
        img_param_sw->yuvctrl_tbl->manual.saturation
    );
    
    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_yuvctrl_sw cfg[%d]: %d,%d,%d\r\n", i, 
            img_param_sw->yuvctrl_tbl->container[i].brightness,
            img_param_sw->yuvctrl_tbl->container[i].contrast,
            img_param_sw->yuvctrl_tbl->container[i].saturation
            );
    }
    printf("\r\n");
    #else
    printf("[OK] img_param_get_yuvctrl_sw manual mode %d,\r\n", img_param_sw->yuvctrl_tbl->manual_mode);
    printf("[OK] img_param_get_yuvctrl_sw manual cfg: %d,%d,%d,%d,%d,%d,%d,\r\n", 
        img_param_sw->yuvctrl_tbl->manual.brightness,
        img_param_sw->yuvctrl_tbl->manual.contrast,
        img_param_sw->yuvctrl_tbl->manual.saturation,
        img_param_sw->yuvctrl_tbl->manual.specialMode,
        img_param_sw->yuvctrl_tbl->manual.sepiaCb,
        img_param_sw->yuvctrl_tbl->manual.sepiaCr,
        img_param_sw->yuvctrl_tbl->manual.solarThred
    );
    
    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        printf("[OK] img_param_get_yuvctrl_sw cfg[%d]: %d,%d,%d,%d,%d,%d,%d,\r\n", i, 
            img_param_sw->yuvctrl_tbl->container[i].brightness,
            img_param_sw->yuvctrl_tbl->container[i].contrast,
            img_param_sw->yuvctrl_tbl->container[i].saturation,
            img_param_sw->yuvctrl_tbl->container[i].specialMode,
            img_param_sw->yuvctrl_tbl->container[i].sepiaCb,
            img_param_sw->yuvctrl_tbl->container[i].sepiaCr,
            img_param_sw->yuvctrl_tbl->container[i].solarThred
            );
    }
    printf("\r\n");
    #endif
    #endif
    printf("[OK] img_param_get_yuvadj_sw mode %d,\r\n", img_param->yuvadj_tbl->yuvadjCfg.mode);
    printf("[OK] img_param_get_yuvadj_sw cfg: %d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n", 
        img_param->yuvadj_tbl->yuvadjCfg.container.y_luma,
        img_param->yuvadj_tbl->yuvadjCfg.container.y_mul0,
        img_param->yuvadj_tbl->yuvadjCfg.container.y_mul1,
        img_param->yuvadj_tbl->yuvadjCfg.container.y_min,
        img_param->yuvadj_tbl->yuvadjCfg.container.y_max,
        img_param->yuvadj_tbl->yuvadjCfg.container.uv_mul0,
        img_param->yuvadj_tbl->yuvadjCfg.container.uv_mul1,
        img_param->yuvadj_tbl->yuvadjCfg.container.uv_min,
        img_param->yuvadj_tbl->yuvadjCfg.container.uv_max
        );
    printf("\r\n");

}


/*********** sw tbl end ************************************************************************************************/

