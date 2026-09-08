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

#ifndef __IMG_PORC_H__
#define __IMG_PORC_H__


#include "isp_comm_sns.h"
/*#include "ae_algo.h"
#include "awb_algo.h"
#include "tm_algo.h"
#include "defog_algo.h"
*/
#include "hal_isp_module.h"
#include "img_struct.h"
#include "sensor.h"

#define PRINTF (0)

#define AE_ENABLE_BIT          (1 << 0)
#define AWB_ENABLE_BIT         (1 << 1)

typedef void (*ipro_isp_stats_ready_cb)(void *usrdata);

typedef enum {
    FORCE_FLAG = 0,
    ISO_FLAG,
    EXPO_TIME_FLAG,
    SYS_GAIN_FLAG,
    CT_FLAG
} imgproc_update_flag_type_t;

typedef struct {
    aaa_stats_cfg_t stats_cfg;
} img_proc_cfg_t;

typedef enum {
    IMGPROC_INIT = 0,
    IMGPROC_RUN,
    IMGPROC_PAUSE,
    IMGPROC_STOP,
    IMGPROC_STATUS_NUM,
} imgproc_status_type_t;

typedef enum {
    IMGPROC_CALIB_SYS_GAIN = 0,
    IMGPROC_CALIB_R_GAIN,
    IMGPROC_CALIB_B_GAIN,
    IMGPROC_CALIB_BLC,
    IMGPROC_CALIB_NUM,
} imgproc_calib_e;

typedef struct {
    int sys_gain;
    int iso;
    int expo;
    int r_gain;
    int b_gain;
} set_info_t;

typedef struct {
    int sys_gain;
    int r_gain;
    int b_gain;
    int ae_target;
} imgproc_calib_t;


int imgproc_init(isp_hal_vin_cfg_t *cfg);
int imgproc_calib_init(isp_hal_vin_cfg_t *vin_cfg);

int  imgproc_bind_sensor_model(const char* sensor_name);
int  imgproc_set_config(img_proc_cfg_t *config, bool is_virtual_sensor);
int imgproc_start_calib(const rt_camera_desc *cam_desc);
int  imgproc_start(const      rt_camera_desc *cam_desc);
//int imgproc_start_fast(const rt_camera_desc *cam_desc);
int imgproc_start_fast_isp(const rt_camera_desc *cam_desc);
int imgproc_start_pir(void);
int  imgproc_stop(void);
int imgproc_status(int     *status);
const char* imgproc_get_sensor_name(void);
int imgproc_reset_fast_flag(void);
int imgproc_pir_init(const rt_camera_desc *cam_desc, bool is_led_on);

int  imgproc_update_force(void);
void get_info(set_info_t *info);
void imgproc_sw_reg_init(void);

int imgproc_set_flicker_mode(flicker_mode mode);
int imgproc_get_flicker_mode(void);

int ipro_isp_stats_ready_config(ipro_isp_stats_ready_cb cb, void *usrdata);

int imgproc_set_calib_reg(imgproc_calib_t *reg);


#endif  /* __IMG_PROC_H__ */

