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
#include <hal_isp.h>
#include <drv_mm_misc.h>
#include "compiler/common.h"

#include "img_param.h"
#include "img_proc.h"

#include "firmware_ct.h"
#include "firmware_gamma.h"
#include "firmware_tonemapping.h"
#include "firmware_defog.h"
#include "firmware_cs.h"
#include "firmware_yuvctrl.h"
#include "firmware_yuvadj.h"
#include "img_utils.h"
#include "hal_isp_module.h"
#include "drv_isp.h"
#include "sensor.h"
#include "sensor_utils.h"
#include "algo.h"
#include "hal_mtimer.h"

#define FAST_AE_ALLWAYS_WAKE (1)

#define GAIN_INFO_NUM           (5)
#define VIRTUAL_LOOP            0
#define PERFORMANCE_MONITOR     0
#define IMG_PROC_SKIP_NUM       0

#define GPIO_IR_BOARD_IRC       (0xFF)
#define GPIO_SENSOR_BOARD_IRC   (0xFF)
#define UNUPDATE_VALUE          (0xFFFFFFFF)

#define FAST_AE_SETTING (2)

#if (FAST_AE_SETTING == 2)//init, first
#define FAST_START_CNT  (1)
#define FAST_CALC_IDX   (2)
#define AE_SETTING_IDX  (3)
#define AWB_SETTING_IDX (4)
#define GET_FLAG_IDX    (2)
#elif (FAST_AE_SETTING == 0)
#define FAST_START_CNT  (0)
#define FAST_CALC_IDX   (2)
#define AE_SETTING_IDX  (4)
#define AWB_SETTING_IDX (4)
#define GET_FLAG_IDX    (3)
#endif

#define FAST_END_CNT    (10)

static int fast_ae_total_gain = 0;
static int fast_rgain = 0;
static int fast_bgain = 0;
static uint8_t fast_flag = 0;
static uint8_t awb_flag = 0;
static int low_flag = 0;
static int calib_update_flag[IMGPROC_CALIB_NUM] = {0};

static imgproc_calib_t g_calib = {
    .sys_gain   = 6144,
    .r_gain     = 16,
    .b_gain     = 16,
    .ae_target  = 45,
};

ae_fast_param_t g_ae_param = {
    .start_cnt   = FAST_START_CNT,
    .calc_idx    = FAST_CALC_IDX,
    .luma_target = 45,
    .base_ev     = 3,
};

typedef struct {
    uint8_t status;
    int stop_imgproc_flag;
    uint16_t skip_num;
    uint16_t start_skip_num;
    uint16_t update_force;
    bool is_virtual_sensor;
    const IPRO_IMG_PARAM_DESC_T *img_param;
    const IPRO_IMG_PARAM_SW_DESC_T *img_param_sw;
} imgproc_obj_t;

/*typedef struct
{
    uint8_t fw_module_en[ISP_ADJ_PARAM_NUM];
    awb_rb_gain_t rb_gain;
} img_porc_isp_cfg_t;

typedef struct
{
    int iso_flag;
    int ct_flag;
    GAIN_DB ae_gain;
    EXPO_TIME ae_expo_time;
    GAIN_DB ae_sys_gain;
    IPRO_COLOR_TEMP awb_ct;
} img_porc_isp_keep_cfg_t;
*/

typedef struct {
    uint8_t force;
    uint8_t iso;
    uint8_t expo_time;
    uint8_t sys_gain;
    uint8_t ct;
    uint8_t ae;
    uint8_t awb;
 } imgproc_update_flag_t;

typedef struct {
    uint16_t force;
    GAIN_DB iso;
    EXPO_TIME expo_time;
    GAIN_DB sys_gain;
    IPRO_COLOR_TEMP ct;
} imgproc_update_val_t;

typedef struct {
    imgproc_update_flag_t flag;
    imgproc_update_val_t value;
} imgproc_update_t;

/*typedef struct {
    bool fps_flag;
    uint32_t fps;
    uint32_t cur_frame_num;
} imgproc_fps_t;*/

static imgproc_obj_t img_proc = {0};
static char g_sensor_name[32] = {0};

static imgproc_update_t isp_update;
static TaskHandle_t gImgProcHdl = NULL;               /* task handle */
static const rt_camera_desc *g_cam_desc;

#if PERFORMANCE_MONITOR
#define FRM_CNT_FOR_ONE_STATS     100       /* 8 sec */
static int ae_handl_cnt = 0;
#endif


static void isp_stats_ready_cb(void *usrdata)
{
    BaseType_t xHigherPriorityTaskWoken;

    xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(gImgProcHdl, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void imgproc_set_update_state(const imgproc_update_flag_type_t flag_type, bool flag, int value)
{
    switch (flag_type)
        {
        case FORCE_FLAG:
            isp_update.flag.force = flag;
            if (value != UNUPDATE_VALUE) isp_update.value.force = value;
            break;
        case ISO_FLAG:
            isp_update.flag.iso = flag;
            if (value != UNUPDATE_VALUE) isp_update.value.iso = value;
            break;
        case EXPO_TIME_FLAG:
            isp_update.flag.expo_time = flag;
            if (value != UNUPDATE_VALUE) isp_update.value.expo_time = value;
            break;
        case SYS_GAIN_FLAG:
            isp_update.flag.sys_gain = flag;
            if (value != UNUPDATE_VALUE) isp_update.value.sys_gain = value;
            break;
        case CT_FLAG:
            isp_update.flag.ct = flag;
            if (value != UNUPDATE_VALUE) isp_update.value.ct = value;
            break;
        default:
            break;
        }
}



static bool imgproc_is_update_force()
{
    return ((0 != img_proc.update_force)? true: false);
}

int imgproc_update_force()
{
    img_proc.update_force = 10;
    return 0;
}

void imgproc_sw_reg_init(void)
{
    //int i = 0;

    //ae_sw_init();
    //awb_sw_init();
    ct_init();
    gamma_init();
    tm_init();
    defog_init();
    cs_init();
    yuvctrl_init();
    yuvadj_init();
    
    //memset(&img_proc.img_param_sw->state_tbl->state, ENABLE, sizeof(Fun_Type) * ISP_ADJ_PARAM_NUM);
    //for(i = 0; i < ISP_ADJ_PARAM_NUM; i++) {
    //    imgproc_set_fw_module_en(i, img_proc.img_param_sw->state_tbl->state[i]);
    ///}
   

    /* Set default value of IQ */
    GAIN_DB gain = 1536;
    GAIN_DB sys_gain = 1536;
    //IPRO_COLOR_TEMP color_temp = COLOR_TEMP_D65;

    ct_img_update();
    gamma_img_update();
    tm_img_update(sys_gain);
    defog_img_update(sys_gain);
    cs_img_update(gain);
    yuvctrl_img_update(gain);
    yuvadj_img_update();

}


/*static int imgproc_win_init(isp_hal_vin_cfg_t *vin_cfg)
{
    int ret;
    img_proc_cfg_t cfg;
    bool is_virtual_sensor = 0;

    cfg.stats_cfg.act_win.width =  vin_cfg->in_size.width;
    cfg.stats_cfg.act_win.height = vin_cfg->in_size.height;
    cfg.stats_cfg.out_win.width =  vin_cfg->glb_crop.width;
    cfg.stats_cfg.out_win.height = vin_cfg->glb_crop.height;
    ret = imgproc_set_config(&cfg, is_virtual_sensor);

    return ret;
}*/

int imgproc_init(isp_hal_vin_cfg_t *vin_cfg)
{
     isp_set_default_iq_param(vin_cfg->in_size.width, vin_cfg->in_size.height);

     if (imgproc_bind_sensor_model(vin_cfg->sensor_name) < 0) {
        printf("[error] cannot match any IQ param to sensor model %s\r\n", vin_cfg->sensor_name);
        return -1;
    }

    fast_awb_init(vin_cfg->in_size.width, vin_cfg->in_size.height);
    imgproc_sw_reg_init();

    return 0;
}

int imgproc_calib_init(isp_hal_vin_cfg_t *vin_cfg)
{

     if (imgproc_bind_sensor_model(vin_cfg->sensor_name) < 0) {
        printf("[error] cannot match any IQ param to sensor model %s\r\n", vin_cfg->sensor_name);
        return -1;
    }

    fast_awb_init(vin_cfg->in_size.width, vin_cfg->in_size.height);
    isp_set_cal_iq_param(vin_cfg->in_size.width, vin_cfg->in_size.height);
    return 0;
}

int imgproc_bind_sensor_model(const char* sensor_name)
{
    int ret = -1;
    int g_iq_param_typ = 0;
    strncpy(g_sensor_name, sensor_name, sizeof(g_sensor_name) - 1);
    ret = img_param_load(sensor_name, g_iq_param_typ);

    if (ret == 0) {
        img_param_sw_load(sensor_name);
      //  img_param_calib_load(sensor_name);
    }
    return ret;
}

int imgproc_set_config(img_proc_cfg_t *cfg, bool is_virtual_sensor)
{
  /*  ae_config_t  ae_cfg;
    awb_config_t awb_cfg;

    memcpy(&(ae_cfg.stats_cfg),  &(cfg->stats_cfg), sizeof(aaa_stats_cfg_t));
    memcpy(&(awb_cfg.stats_cfg), &(cfg->stats_cfg), sizeof(aaa_stats_cfg_t));
    
    printf("is_virtual_sensor %d\r\n", is_virtual_sensor);
    
    if (ae_config(&ae_cfg, is_virtual_sensor) < 0)
        return -1;

    if (awb_config(&awb_cfg, is_virtual_sensor) < 0)
        return -1;
    */
    return 0;
}


#define SAVE_CNT (AWB_SETTING_IDX + 2)
static ATTR_PSRAM_BSS_SECTION int g_frame_cnt = 1;
static ATTR_PSRAM_BSS_SECTION int g_luma_avg_list[SAVE_CNT] = {0};
static ATTR_PSRAM_BSS_SECTION int g_gain_db_list[SAVE_CNT] = {0};
static ATTR_PSRAM_BSS_SECTION int g_time_list[SAVE_CNT] = {0};
static ATTR_PSRAM_BSS_SECTION int g_w_cnt_list[SAVE_CNT] = {0};
static ATTR_PSRAM_BSS_SECTION int g_rgain_list[SAVE_CNT] = {0};
static ATTR_PSRAM_BSS_SECTION int g_bgain_list[SAVE_CNT] = {0};

#if !FAST_AE_ALLWAYS_WAKE
static void imgproc_fast_set(void)
{
    g_cam_desc->set_case(SENSOR_CASE_AE_MANUAL);
    g_cam_desc->set_case(SENSOR_CASE_AWB_MANUAL);
    g_cam_desc->set_ae_expo_gain(SENSOR_CASE_SET_AE_SYS_GAIN, fast_ae_total_gain);
    g_cam_desc->set_awb_gain(SENSOR_CASE_SET_AWB_RGAIN_BGAIN, fast_rgain, fast_bgain);
    g_cam_desc->set_case(SENSOR_CASE_OTHER_MODULE_ON);//enable all module, ae/awb manual
    //printf("aa g_frame_cnt %d, img_proc.status %d,  fast_flag %d, fast_ae_total_gain %d, fast_rgain %d, %d\r\n", g_frame_cnt, img_proc.status , fast_flag, fast_ae_total_gain,fast_rgain,fast_bgain);
}
#endif

static void imgproc_wake_up_init(void)
{
    awb_flag = 0;
    low_flag = 0;

    /* init settting */
    memset(&g_luma_avg_list, 0, sizeof(int) * SAVE_CNT);
    memset(&g_luma_avg_list, 0, sizeof(int) * SAVE_CNT);
    memset(&g_gain_db_list,  0, sizeof(int) * SAVE_CNT);
    memset(&g_time_list,     0, sizeof(int) * SAVE_CNT);
    memset(&g_w_cnt_list,    0, sizeof(int) * SAVE_CNT);
    memset(&g_rgain_list,    0, sizeof(int) * SAVE_CNT);
    memset(&g_bgain_list,    0, sizeof(int) * SAVE_CNT);
    g_frame_cnt = 1;
    g_time_list[0] = xTaskGetTickCountFromISR();

    fast_ae_init(&g_ae_param);

    g_cam_desc->set_case(SENSOR_CASE_FAST_INIT);//init fast
    g_cam_desc->set_ae_expo_gain(SENSOR_CASE_SET_AE_SYS_GAIN, fast_ae_get_low_base());
}

/*
isp_ae_sum_t global_sum[AE_STATS_GRID_NUM];
static int _isp_ae_stat_luma_avg()
{
     int i;
     uint32_t luma;
     uint32_t luma_sum = 0;
     int res_avg = 0;
    memset(&global_sum, 0, sizeof(isp_ae_sum_t)*AE_STATS_GRID_NUM);
    ISP_Get_AE_Stats(AE_STATS_GRID_NUM, (uint32_t *)&global_sum);

     for (i = 0; i < AE_STATS_GRID_NUM; i++) {
         luma = global_sum[i].y_sum;
         luma_sum += luma;
         //printf("\t\t YSUM[%d]: value %ld, total %ld,     %d,  %d,  %d\r\n", i, luma, luma_sum, global_sum[i].r_sum, global_sum[i].g_sum, global_sum[i].b_sum);
    }
     res_avg = luma_sum / AE_STATS_GRID_NUM;

    return res_avg;
}
*/

void vImageControlTask_Calib(void *pvParameters)
{
    printf("vImageControlTask_Calib\r\n");
    gImgProcHdl = xTaskGetCurrentTaskHandle();
    ipro_isp_stats_ready_config(isp_stats_ready_cb, NULL);

    g_cam_desc->set_case(SENSOR_CASE_CALIBRATION);//calib mode

    while (!img_proc.stop_imgproc_flag) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        if (g_calib.sys_gain == 1) {
            g_cam_desc->set_case(SENSOR_CASE_AE_AUTO);
        } else {
            g_cam_desc->set_case(SENSOR_CASE_AE_MANUAL);
            g_cam_desc->set_ae_expo_gain(SENSOR_CASE_SET_AE_SYS_GAIN, g_calib.sys_gain);
        }
       if ((calib_update_flag[IMGPROC_CALIB_R_GAIN] == 1) || (calib_update_flag[IMGPROC_CALIB_B_GAIN] == 1)) {
            g_cam_desc->set_awb_gain(SENSOR_CASE_SET_AWB_RGAIN_BGAIN, g_calib.r_gain, g_calib.b_gain);
            memset(calib_update_flag, 0, sizeof(calib_update_flag));
        }

    }
}

void vImageControlTask_isp(void *pvParameters)
{
    bool is_update_force = 0;
    GAIN_DB sys_gain;
    g_cam_desc->set_case(SENSOR_CASE_NORMAL);//enable all

    gImgProcHdl = xTaskGetCurrentTaskHandle();
    ipro_isp_stats_ready_config(isp_stats_ready_cb, NULL);

    while (!img_proc.stop_imgproc_flag) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        g_cam_desc->get_ae_register(SENSOR_CASE_GET_AE_SYS_GAIN, &sys_gain);
        imgproc_set_update_state(ISO_FLAG, 1, sys_gain);
        imgproc_set_update_state(SYS_GAIN_FLAG, 1, sys_gain);

        is_update_force = imgproc_is_update_force();
        imgproc_tonemapping(isp_update.flag.sys_gain, isp_update.value.sys_gain, is_update_force);
        imgproc_defog(      isp_update.flag.sys_gain, isp_update.value.sys_gain, is_update_force);
        imgproc_cs(         isp_update.flag.iso,      isp_update.value.iso,      is_update_force);
        imgproc_yuvctrl(    isp_update.flag.iso,      isp_update.value.iso,      is_update_force);
        imgproc_yuvadj(is_update_force);
    }
}

void vImageControlTask_Fast_Isp(void *pvParameters)
{
    bool is_update_force = 1;
    GAIN_DB sys_gain;

    int ae_total_gain = 0;
    int isp_luma_avg = 0;
    int ae_expo_level = 0;
    int rgain, bgain;

    int level_offset = FAST_CALC_IDX-1;

    gImgProcHdl = xTaskGetCurrentTaskHandle();
    ipro_isp_stats_ready_config(isp_stats_ready_cb, NULL);

    while (!img_proc.stop_imgproc_flag) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        if (fast_flag == 0) {
            isp_luma_avg = fast_ae_stat_luma_avg();
            if ((g_frame_cnt <= AE_SETTING_IDX) && (awb_flag != 1)) {
                fast_calc_ae_expo_gain(isp_luma_avg, &ae_total_gain, &ae_expo_level);
                g_cam_desc->set_ae_expo_gain(SENSOR_CASE_SET_AE_SYS_GAIN, ae_total_gain);

                if(g_frame_cnt == GET_FLAG_IDX) {
                    fast_ae_get_internal_cfg(&low_flag);
                }
            }

            calc_wb_gain(&rgain, &bgain);
            if (((g_frame_cnt == AWB_SETTING_IDX) || (low_flag == 1)) && ((awb_flag == 0))){
                get_awb_level((ae_expo_level+level_offset), &rgain, &bgain);
                //printf("!!!!!!!! frame_cnt %d, ae_expo_level %d,offset %d, rgain %d, bgain %d\r\n", g_frame_cnt, ae_expo_level, level_offset,   rgain, bgain);
                g_cam_desc->set_awb_gain(SENSOR_CASE_SET_AWB_RGAIN_BGAIN, rgain, bgain);
                g_cam_desc->set_case(SENSOR_CASE_NORMAL);//enable all

                awb_flag = 1;
            }
            if (g_frame_cnt == AWB_SETTING_IDX) {
                fast_flag = 1;
                awb_flag = 0;
            }
        }
////////////////////////////////////////////////////////////////////////////////////////////////
        g_cam_desc->get_ae_register(SENSOR_CASE_GET_AE_SYS_GAIN, &sys_gain);
        imgproc_set_update_state(ISO_FLAG, 1, sys_gain);
        imgproc_set_update_state(SYS_GAIN_FLAG, 1, sys_gain);

        is_update_force = imgproc_is_update_force();
        if (g_frame_cnt > AE_SETTING_IDX + 1) {
            imgproc_tonemapping(isp_update.flag.sys_gain, isp_update.value.sys_gain, is_update_force);
            imgproc_defog(      isp_update.flag.sys_gain, isp_update.value.sys_gain, is_update_force);
            imgproc_cs(         isp_update.flag.iso,      isp_update.value.iso,      is_update_force);
            imgproc_yuvctrl(    isp_update.flag.iso,      isp_update.value.iso,      is_update_force);
            imgproc_yuvadj(is_update_force);
        }
        if (g_frame_cnt < FAST_END_CNT) g_frame_cnt++;
    }

    ipro_isp_stats_ready_config(NULL, NULL);
    imgproc_stop();
    vTaskDelete(NULL);
}


void vImageControlTask_PIR(void *pvParameters)
{
    bool is_update_force = 1;
    GAIN_DB sys_gain;

    int ae_total_gain = 0;
    int isp_luma_avg = 0;
    int ae_expo_level = 0;
    int rgain, bgain;
    
    int time_diff = 0;

    int level_offset = FAST_CALC_IDX-1;
    g_time_list[0] = xTaskGetTickCountFromISR();

    gImgProcHdl = xTaskGetCurrentTaskHandle();
    ipro_isp_stats_ready_config(isp_stats_ready_cb, NULL);

    while (!img_proc.stop_imgproc_flag) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        if (fast_flag == 0) {

            if (g_frame_cnt < SAVE_CNT) g_time_list[g_frame_cnt] = xTaskGetTickCountFromISR();
            if (g_frame_cnt < SAVE_CNT) g_w_cnt_list[g_frame_cnt] = ISP_Get_AE_W_Cnt_R();

            isp_luma_avg = fast_ae_stat_luma_avg();
            if (g_frame_cnt < SAVE_CNT) g_luma_avg_list[g_frame_cnt] = isp_luma_avg;

            if ((g_frame_cnt <= AE_SETTING_IDX) && (awb_flag != 1)) {
                fast_calc_ae_expo_gain(isp_luma_avg, &ae_total_gain, &ae_expo_level);
                g_cam_desc->set_ae_expo_gain(SENSOR_CASE_SET_AE_SYS_GAIN, ae_total_gain);
                if(g_frame_cnt == GET_FLAG_IDX) {
                    fast_ae_get_internal_cfg(&low_flag);
                    //printf("low_flag %d \r\n", low_flag);
                }
            }
            if (g_frame_cnt < SAVE_CNT) g_gain_db_list[g_frame_cnt] = ae_total_gain;

            calc_wb_gain(&rgain, &bgain);
            if (g_frame_cnt < SAVE_CNT) g_rgain_list[g_frame_cnt] = rgain;
            if (g_frame_cnt < SAVE_CNT) g_bgain_list[g_frame_cnt] = bgain;

            if (((g_frame_cnt == AWB_SETTING_IDX) || (low_flag == 1)) && ((awb_flag == 0))){
                get_awb_level((ae_expo_level+level_offset), &rgain, &bgain);
                //printf("!!!!!!!! frame_cnt %d, ae_expo_level %d,offset %d, rgain %d, bgain %d\r\n", g_frame_cnt, ae_expo_level, level_offset,   rgain, bgain);
                g_cam_desc->set_awb_gain(SENSOR_CASE_SET_AWB_RGAIN_BGAIN, rgain, bgain);
                g_cam_desc->set_case(SENSOR_CASE_OTHER_MODULE_ON);//enable all module, ae/awb manual
                awb_flag = 1;

                //save result
                fast_ae_total_gain = ae_total_gain;
                fast_rgain = rgain;
                fast_bgain = bgain;
            }
            if (g_frame_cnt == AWB_SETTING_IDX) {
                fast_flag = 1;
                awb_flag = 0;

                if (0) {
                    for (int idx = 0; idx < SAVE_CNT; idx++) {
                        if (idx > 0) time_diff  = g_time_list[idx]-g_time_list[idx-1];
                        printf("frame_cnt %d, luma_avg %d, gain_db %d, time %d, %d, gain %d, %d, w_cnt %d\r\n",idx,
                        g_luma_avg_list[idx],g_gain_db_list[idx],g_time_list[idx],time_diff,g_rgain_list[idx],g_bgain_list[idx],g_w_cnt_list[idx]);
                    }
                }
            }
        }
////////////////////////////////////////////////////////////////////////////////////////////////
        g_cam_desc->get_ae_register(SENSOR_CASE_GET_AE_SYS_GAIN, &sys_gain);
        imgproc_set_update_state(ISO_FLAG, 1, sys_gain);
        imgproc_set_update_state(SYS_GAIN_FLAG, 1, sys_gain);

        is_update_force = imgproc_is_update_force();
        if (g_frame_cnt > AE_SETTING_IDX + 1) {
            imgproc_tonemapping(isp_update.flag.sys_gain, isp_update.value.sys_gain, is_update_force);
            imgproc_defog(      isp_update.flag.sys_gain, isp_update.value.sys_gain, is_update_force);
            imgproc_cs(         isp_update.flag.iso,      isp_update.value.iso,      is_update_force);
            imgproc_yuvctrl(    isp_update.flag.iso,      isp_update.value.iso,      is_update_force);
            imgproc_yuvadj(is_update_force);
        }
        if (g_frame_cnt < FAST_END_CNT) g_frame_cnt++;
    }

    ipro_isp_stats_ready_config(NULL, NULL);
    imgproc_stop();
    vTaskDelete(NULL);
}

int imgproc_start_calib(const      rt_camera_desc *cam_desc)
{
    printf("imgproc_start_calib 1\r\n");

    g_cam_desc = cam_desc;
    if ((img_proc.status == IMGPROC_INIT)) {
        img_proc.status = IMGPROC_RUN;
        img_proc.stop_imgproc_flag = 0;

        if (gImgProcHdl == NULL) {
            xTaskCreate(vImageControlTask_Calib, "Image_control_calib", 4096, NULL, 10, NULL);
        }
    }

    return 0;
}
#if 0
int imgproc_start_fast(const      rt_camera_desc *cam_desc)
{
    g_cam_desc = cam_desc;
    if ((img_proc.status == IMGPROC_INIT)) {
        img_proc.status = IMGPROC_RUN;
        img_proc.stop_imgproc_flag = 0;

        if (gImgProcHdl == NULL) {
            xTaskCreate(vImageControlTask_Fast, "Image_control_fast", 4096, NULL, 10, NULL);
        }
    }

    return 0;
}
#endif

int imgproc_start(const      rt_camera_desc *cam_desc)
{
    g_cam_desc = cam_desc;
    if ((img_proc.status == IMGPROC_INIT)) {
        img_proc.status = IMGPROC_RUN;
        img_proc.stop_imgproc_flag = 0;
        if (gImgProcHdl == NULL) {
            //xTaskCreate(vImageControlTask, "Image_control", 1024, NULL, 10, NULL);
            BaseType_t rc = xTaskCreate(vImageControlTask_isp, "Image_control_isp", 4096, NULL, 10, NULL);
            if (rc != pdPASS) {
                printf("xTaskCreate failed: rc=%ld\n", (long)rc);
                printf("FreeHeap=%lu, MinEverFreeHeap=%lu\n",
                       (unsigned long)xPortGetFreeHeapSize(),
                       (unsigned long)xPortGetMinimumEverFreeHeapSize());
            }
        }
    }

    if (img_proc.is_virtual_sensor) {
        img_proc.status = IMGPROC_RUN;
        img_proc.stop_imgproc_flag = 0;
    }

    return 0;
}

int imgproc_start_fast_isp(const      rt_camera_desc *cam_desc)
{
    g_cam_desc = cam_desc;
    if ((img_proc.status == IMGPROC_INIT)) {
        img_proc.status = IMGPROC_RUN;
        img_proc.stop_imgproc_flag = 0;

        if (gImgProcHdl == NULL) {
            BaseType_t rc = xTaskCreate(vImageControlTask_Fast_Isp, "Image_control_fast_isp", 4096, NULL, 10, NULL);
            if (rc != pdPASS) {
            printf("xTaskCreate failed: rc=%ld\n", (long)rc);
            printf("FreeHeap=%lu, MinEverFreeHeap=%lu\n",
                   (unsigned long)xPortGetFreeHeapSize(),
                   (unsigned long)xPortGetMinimumEverFreeHeapSize());
            }
        } else {
            eTaskState state = eTaskGetState(gImgProcHdl);
            /*const char *stateStr[] = {
                "Running", "Ready", "Blocked", "Suspended", "Deleted", "Invalid"
            };
            printf("Task State: %s\r\n", stateStr[state]);*/
            if (state == eDeleted) {
                /* init settting */
                imgproc_wake_up_init();
                xTaskCreate(vImageControlTask_Fast_Isp, "Image_control_fast_isp", 4096, NULL, 10, NULL);
            }
            if (state == eSuspended) {
                g_frame_cnt = 1;
                //printf("imgproc_start fast sensor isp 4\r\n");
                vTaskResume(gImgProcHdl);
            }
        }
    }

    return 0;
}

int imgproc_start_pir(void)
{
    if ((img_proc.status != IMGPROC_RUN)) {//might be init or pause
        img_proc.status = IMGPROC_RUN;
        img_proc.stop_imgproc_flag = 0;

        if (gImgProcHdl == NULL) {
            BaseType_t rc = xTaskCreate(vImageControlTask_PIR, "Image_control_pir", 4096, NULL, 10, NULL);
            if (rc != pdPASS) {
            printf("xTaskCreate failed: rc=%ld\n", (long)rc);
            printf("FreeHeap=%lu, MinEverFreeHeap=%lu\n",
                   (unsigned long)xPortGetFreeHeapSize(),
                   (unsigned long)xPortGetMinimumEverFreeHeapSize());
            }
        } else {
            eTaskState state = eTaskGetState(gImgProcHdl);
            /*const char *stateStr[] = {
                "Running", "Ready", "Blocked", "Suspended", "Deleted", "Invalid"
            };
            printf("Task State: %s\r\n", stateStr[state]);*/
            if (state == eDeleted) {
                /* init settting */
                imgproc_wake_up_init();
                xTaskCreate(vImageControlTask_PIR, "Image_control_pir", 4096, NULL, 10, NULL);
            }
            /*if (state == eSuspended) {
                g_frame_cnt = 1;
                //printf("imgproc_start fast sensor isp 4\r\n");
                vTaskResume(gImgProcHdl);
            }*/
        }
    }

    return 0;
}

int imgproc_stop(void)
{
    if (gImgProcHdl == NULL)
        img_proc.status = IMGPROC_STOP;
    else
        img_proc.status = IMGPROC_PAUSE;
    img_proc.stop_imgproc_flag = 1;

    return 0;
}

int imgproc_status(int     *status)
{
    if (img_proc.status >= IMGPROC_STATUS_NUM) {
        //LOGE("imgproc_status is over status numer %d\r\n", img_proc.status);
        return -1;
    }
    *status = img_proc.status;
    return 0;

}

const char* imgproc_get_sensor_name(void)
{
    return g_sensor_name;
}

// no off power setting
int imgproc_reset_fast_flag(void)
{
    fast_flag = 0;
    fast_awb_reset();

    return 0;
}

//only for calib
int imgproc_set_calib_reg(imgproc_calib_t *reg)
{
    printf("%d, %d, %d, %d \r\n", reg->sys_gain, reg->r_gain, reg->b_gain, reg->ae_target);
    memset(calib_update_flag, 0, sizeof(calib_update_flag));

    if (reg->sys_gain != -1) {
        g_calib.sys_gain = reg->sys_gain;
        calib_update_flag[IMGPROC_CALIB_SYS_GAIN] = 1;
    }
    if (reg->r_gain != -1) {
        g_calib.r_gain = reg->r_gain;
        calib_update_flag[IMGPROC_CALIB_R_GAIN] = 1;
    }
    if (reg->b_gain != -1) {
        g_calib.b_gain = reg->b_gain;
        calib_update_flag[IMGPROC_CALIB_B_GAIN] = 1;
    }
    if (reg->ae_target != -1) {
        g_calib.ae_target = reg->ae_target;
        g_ae_param.luma_target = reg->ae_target;
        calib_update_flag[IMGPROC_CALIB_BLC] = 1;
    }
    printf("g %d, %d, %d, %d \r\n", g_calib.sys_gain, g_calib.r_gain, g_calib.b_gain, g_calib.ae_target);

    return 0;
}

int imgproc_pir_init(const      rt_camera_desc *cam_desc, bool is_led_on)
{
    g_cam_desc = cam_desc;

    if (is_led_on) g_ae_param.base_ev = 4;

#if FAST_AE_ALLWAYS_WAKE
    imgproc_wake_up_init();
#else
    if (fast_flag == 0) {
        imgproc_wake_up_init();
    } else {
        imgproc_fast_set();
    }
#endif
    return 0;
}
