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

#ifndef __CALIB_DATA_H__
#define __CALIB_DATA_H__

#include "../../isp_firmware/include/img_struct.h"

#define D65_IDX                 (3)
#define NORM_NUM                (9)
#define ISP_AWB3_CAL_BIN_NUM    (88)

typedef int COLOR_TEMP;                   /* ct value in K */

typedef struct {
    uint16_t r_gain;
    uint16_t b_gain;
} RB_Gain_Type;

typedef struct {
    int bin[ISP_AWB3_CAL_BIN_NUM];
} wb_container_cal_t;

typedef struct {
    uint8_t container[NORM_NUM];
} wb_y_weight_container_t;


/*uint32_t wb_gain_map_input[5][2] = {
   {3908, 7889},//"A"
   {5973, 9102},//"TL84"
   {6122, 8896},//"CWF"
   {6736, 8192}//"D50"
   {8734, 6527}//"D65"
};
**/
/* //light_box_100_100
static const wb3_cal_bin_t wb_cal = {
    .bin = {
        0, 130, 379, 108, 415, 200, 384, 0, 0, 0, 0, 0, 3, 
            23, 42, 26, 0, 0, 0, 0, 1, 44, 107, 50, 8, 0, 
            0, 0, 0, 9, 119, 89, 6, 0, 0, 0, 0, 0, 35, 172,
        47, 0, 0, 0, 0, 0, 3, 95, 177, 19, 0, 0, 0, 6, 3, 
        15, 185, 130, 5, 0, 0, 8, 48, 36, 56, 255, 70, 1, 
        0, 0, 21, 43, 82, 160, 237, 26, 0, 0, 0, 16, 8, 52, 
        238, 130, 5, 0, 0, 0
    }
};*/

static const wb3_cal_bin_t wb_cal = {
    .bin = {
        0, 242, 286, 140, 128, 184, 312, 0, 0, 0, 0, 0, 3,
            19, 36, 22, 0, 0, 0, 0, 4, 37, 90, 42, 7, 0, 0,
        0, 19, 102, 113, 72, 5, 0, 0, 0, 0, 56, 254, 179,
        40, 0, 0, 0, 0, 8, 81, 202, 153, 18, 0, 0, 0, 13,
        54, 77, 74, 59, 4, 0, 0, 7, 43, 40, 13, 4, 3, 0,
        0, 0, 18, 32, 5, 0, 0, 0, 0, 0, 0, 17, 9, 0, 0, 0,
        0, 0, 0, 0
    }
};

const wb3_cal_bin_t* cal_bin = &wb_cal;


int weight_y[NORM_NUM] = {        0,    //0~28
                                 32,    //29~57
                                128,    //56~85
                                192,    //84~114
                                192,    //115~142
                                192,    //143~170
                                203,    //171~198
                                224,    //199~227
                                256};   //238~255


#endif //__CALIB_DATA_H__

