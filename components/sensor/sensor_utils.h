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

#ifndef __SENSOR_UTILS_H__
#define __SENSOR_UTILS_H__

typedef enum {
    SENSOR_CASE_GET_AE_EXPO = 10,
    SENSOR_CASE_GET_AE_AGAIN,
    SENSOR_CASE_GET_AE_DGAIN,
    SENSOR_CASE_GET_AE_SYS_GAIN,
    SENSOR_CASE_GET_AE_LUMA_AVG,
    SENSOR_CASE_GET_AWB_RGAIN,
    SENSOR_CASE_GET_AWB_BGAIN,
    SENSOR_CASE_GET_MAX_NUM,
} SENSOR_CASE_GET_E;

typedef enum {
    SENSOR_CASE_SET_AE_EXPO = 10,
    SENSOR_CASE_SET_AE_AGAIN,
    SENSOR_CASE_SET_AE_DGAIN,
    SENSOR_CASE_SET_AE_SYS_GAIN,
    SENSOR_CASE_SET_AWB_RGAIN_BGAIN,
    SENSOR_CASE_SET_MAX_NUM,
} SENSOR_CASE_SET_E;

typedef enum {
    SENSOR_CASE_NORMAL = 10,
    SENSOR_CASE_FAST_INIT,
    SENSOR_CASE_CALIBRATION,
    SENSOR_CASE_AE_AUTO,
    SENSOR_CASE_AE_MANUAL,
    SENSOR_CASE_AWB_AUTO,
    SENSOR_CASE_AWB_MANUAL,
    SENSOR_CASE_OTHER_MODULE_ON,
    SENSOR_CASE_GAMMA_ON,
    SENSOR_CASE_BLC_ON,
    SENSOR_CASE_MAX_NUM,
} SENSOR_CASE_E;

#define EXPO_GAIN_TBL_NUM (21)
#define SYS_GAIN_DB_MIN (-(EXPO_GAIN_TBL_NUM / 2 * 6))
#define SYS_GAIN_DB_MAX (EXPO_GAIN_TBL_NUM / 2 * 6)

int ana_ae_gain(int gain_db,
                const int *expo_map,
                const int *global_map,
                const int *digital_map,
                uint16_t *expo,
                uint16_t *global_gain,
                uint8_t  *digital_gain);
int ana_ae_gain_from_expo(uint16_t expo, uint16_t global_gain, uint8_t digital_gain,
                          const int *expo_map, const int *global_map, const int *digital_map,
                          int *value);


#endif

