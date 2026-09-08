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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sensor.h"
#include "sccb.h"
#include "sc101iot.h"
#include "sensor_utils.h"

//#include <math.h>
//#include <stdio.h>

#define resolution_720P 1
// #define PCLK_36M
// #define TEST_MODE

#define FAST_INIT_SETTING 0
#define CLIP(min, max, val)  ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

#if 1 // 36M
static const uint8_t sc101iot_reglist[][2] =
{
    {0xf0,0x30},
    {0x01,0xff},
    {0x02,0xe0},
    {0x30,0x10},
    {0x3f,0x81},
    {0xf0,0x00},
#if FAST_INIT_SETTING //joie
    {0x70,0x68},//AEC[0], AGC[1]: 1~auto; 0~mannu
    {0x72,0x30},//Global gain and digital gain con[7]:1~independent; 0~non independent
    {0x84,0xb4},
    {0x8b,0x00},//Global gain[3:0], Hight bit //1x:0x20
    {0x8c,0x20},//Global gain, Low bit
    // exp, 23, gain_db -4608
    //{0x8d,0x00},//integration time, Hight bit //half line
    //{0x8e,0x17}, //integration time, Low bit
     // exp, 373, gain_db 1536
    {0x8d,0x01},//integration time, Hight bit //half line
    {0x8e,0x75}, //integration time, Low bit
    {0x9e,0x10},//digital gain  //1x:0x10
    {0xb0,0xc0},//awb[0], joie
    {0xc8,0x10},//blue gain, 1x
    {0xc9,0x10},//red gain, 1x
#else//ori
    {0x70,0x6b},//AEC[0], AGC[1]
    {0x72,0x30},//Global gain and digital gain con[7]:1~independent; 0~non independent
    {0x84,0xb4},
    {0x8b,0x00},//Global gain[3:0], Hight bit //1x:0x20
    {0x8c,0x20},//Global gain, Low bit
    {0x8d,0x02},//integration time, Hight bit //half line
    {0x8e,0xec},//integration time, Low bit
    {0x9e,0x10},//digital gain  //1x:0x10
    {0xb0,0xc1},
    {0xc8,0x10},//blue gain, 1x
    {0xc9,0x10},//red gain, 1x
#endif
    {0xc6,0x00},
    {0xe0,0x0f},//EE[1], NC[0], dpc[2][3][5]
    {0xb5,0xf0},
    {0xde,0x80},
    {0xb5,0xf0},
    {0xde,0x80},
    {0xb2,0x50},
    {0xb3,0xfc},
    {0xb4,0x40},
    {0xb5,0xc0},
    {0xb6,0x50},
    {0xb7,0xfc},
    {0xb8,0x40},
    {0xb9,0xc0},
    {0xba,0xff},
    {0xbb,0xcc},
    {0xbc,0xa9},
    {0xbd,0x7d},
    {0xc1,0x77},
    {0xf0,0x01},
    {0x70,0x02},
    {0x71,0x02},
    {0x72,0x50},
    {0x73,0x02},
    {0x74,0xd2},
    {0x75,0x20},
    {0x76,0x81},
    {0x77,0x8c},
    {0x78,0x81},
#ifdef PCLK_36M
    {0xf4,0x01},
#endif
#if FAST_INIT_SETTING //joie
    //{0xf5,0x26},//saturation[5] 0ff, cc[2] off, gamma[1] off
    {0xf5,0x66},//contrast[6],saturation[5] 0ff, cc[2] off, gamma[1] off
#else
    {0xf5,0x00},
#endif
    {0xf6,0x00},
#ifdef PCLK_36M
    {0xf4,0x08},
#endif
    {0xf0,0x36},
    {0x40,0x03},
    {0x41,0x01},
    {0xf0,0x39},
    {0x02,0x70},
    {0xf0,0x32},
    {0x41,0x00},
    {0x43,0x01},
    {0x48,0x02},
    {0xf0,0x45},
    {0x09,0x20},
    {0xf0,0x33},
    {0x33,0x10},
    {0xf0,0x30},
    {0x38,0x44},
    {0xf0,0x39},
#if FAST_INIT_SETTING //joie
    {0x07,0x00},//0x3907, 0x3908, blc target offset
    //{0x08,0x19},// ori, ob will be 6
    {0x08,0x01},//set 1 then OB will close to 0
#else
    {0x07,0x00},
    {0x08,0x19},
#endif
    {0x47,0x00},
    {0x48,0x00},
    {0xf4,0x08},
    {0xf0,0x37},
    {0x24,0x31},
    {0xf0,0x34},
    {0x9f,0x02},
    {0xa6,0x51},
    {0xa7,0x57},
    {0xe8,0x5f},
    {0xa8,0x50},
    {0xa9,0x50},
    {0xe9,0x50},
    {0xf0,0x33},
    {0xb3,0x58},
    {0xb2,0x78},
    {0xf0,0x34},
    {0x9f,0x03},
    {0xa6,0x51},
    {0xa7,0x57},
    {0xaa,0x01},
    {0xab,0x28},
    {0xac,0x01},
    {0xad,0x38},
    {0xf0,0x33},
    {0x0a,0x01},
    {0x0b,0x28},
    {0xf0,0x33},
    {0x64,0x0f},
    {0xec,0x51},
    {0xed,0x57},
    {0x06,0x58},
    {0xe9,0x58},
    {0xeb,0x68},
    {0xf0,0x33},
    {0x64,0x0f},
    {0xf0,0x36},
    {0x70,0xdf},
    {0xb6,0x40},
    {0xb7,0x51},
    {0xb8,0x53},
    {0xb9,0x57},
    {0xba,0x5f},
    {0xb0,0x84},
    {0xb1,0x82},
    {0xb2,0x84},
    {0xb3,0x88},
    {0xb4,0x90},
    {0xb5,0x90},
    {0xf0,0x36},
    {0x7e,0x50},
    {0x7f,0x51},
    {0x77,0x81},
    {0x78,0x86},
    {0x79,0x89},
    {0xf0,0x36},
    {0x70,0xdf},
    {0x9c,0x51},
    {0x9d,0x57},
    {0x90,0x54},
    {0x91,0x54},
    {0x92,0x56},
    {0xf0,0x36},
    {0xa0,0x51},
    {0xa1,0x57},
    {0x96,0x33},
    {0x97,0x43},
    {0x98,0x43},
    {0xf0,0x36},
    {0x70,0xdf},
    {0x7c,0x40},
    {0x7d,0x53},
    {0x74,0xd0},
    {0x75,0xf0},
    {0x76,0xf0},
    {0xf0,0x37},
    {0x0f,0xd5},
    {0x7a,0x40},
    {0x7b,0x57},
    {0x71,0x09},
    {0x72,0x09},
    {0x73,0x05},
    {0xf0,0x33},
    {0x01,0x44},
    {0xf0,0x36},
    {0x37,0xfb},
    {0xf0,0x36},
    {0x3c,0x0d},
    {0xf0,0x33},
    {0x14,0x95},
    {0xf0,0x33},
    {0x8f,0x80},
    {0xf0,0x37},
    {0x27,0x14},
    {0x28,0x03},
    {0xf0,0x36},
    {0x37,0xf4},
    {0xf0,0x33},
    {0x01,0x44},
    {0xf0,0x36},
    {0x79,0x89},
    {0xf0,0x34},
    {0xac,0x01},
    {0xad,0x40},
    {0xf0,0x33},
    {0xeb,0x70},
    {0xf0,0x34},
    {0xa8,0x50},
    {0xa9,0x50},
    {0xf0,0x33},
    {0xb3,0x58},
    {0xf0,0x36},
    {0x11,0x80},
    {0xf0,0x36},
    {0x41,0x51},
    {0xf0,0x3f},
#ifdef PCLK_36M
    {0x03,0x0b},
    {0xf0,0x36},
    {0xea,0x09},
    {0xeb,0xf5},
    {0xec,0x11},
    {0xed,0x17},
    {0xe9,0x20},
    {0xf4,0x00},
    {0xf0,0x32},
    {0x0c,0x09},
    {0x0d,0x60},
#else
    {0x03,0x03},
    {0xf0,0x36},
    {0xea,0x09},
#endif
#ifdef TEST_MODE
    {0xf0,0x01},
    {0x00,0x80},
#endif
};
#else
static const uint8_t sc101iot_reglist[][2] =
{
    {0xf0,0x30},
    {0x01,0xff},
    {0x02,0xe0},
    {0x30,0x10},
    {0x3f,0x81},
    {0xf0,0x00},
#if 1
    {0x70,0x68},//AEC[0], AGC[1]: 1~auto; 0~mannu
    //{0x72,0xB0},//independetn ,Global gain and digital gain con[7]:1~independent; 0~non independent
    {0x72,0x30},//Global gain and digital gain con[7]:1~independent; 0~non independent
    {0x84,0xb4},
    {0x8b,0x00},//Global gain[3:0], Hight bit //1x:0x20
    {0x8c,0x20},//Global gain, Low bit
    {0x8d,0x00},//integration time, Hight bit //half line
    {0x8e,0x2e},//integration time, Low bit
    {0x9e,0x10},//digital gain  //1x:0x10
#else//ori
    {0x70,0x6b},//AEC[0], AGC[1]
    {0x72,0x30},
    {0x84,0xb4},
    {0x8b,0x00},//Global gain[3:0], Hight bit
    {0x8c,0x20},//Global gain, Low bit
    {0x8d,0x02},//integration time, Hight bit
    {0x8e,0xec},//integration time, Low bit
    {0x9e,0x10},//digital gain
#endif
    {0xb0,0xc0},//awb[0]
    {0xc8,0x10},//blue gain
    {0xc9,0x10},//red gain
    {0xc6,0x00},
    {0xe0,0x0f},//EE[1], NC[0], dpc[2][3][5]
    {0xb5,0xf0},
    {0xde,0x80},
    {0xb5,0xf0},
    {0xde,0x80},
    {0xb2,0x50},
    {0xb3,0xfc},
    {0xb4,0x40},
    {0xb5,0xc0},
    {0xb6,0x50},
    {0xb7,0xfc},
    {0xb8,0x40},
    {0xb9,0xc0},
    {0xba,0xff},
    {0xbb,0xcc},
    {0xbc,0xa9},
    {0xbd,0x7d},
    {0xc1,0x77},
    {0xf0,0x01},
    #if resolution_720P//720p
    {0x70,0x02},
    {0x71,0x02},
    {0x72,0x50},
    {0x73,0x02},
    {0x74,0xd2},
    {0x75,0x20},
    #else // 320x240
    {0x70,0x02},
    {0x71,0x42},
    {0x72,0x10},
    {0x73,0x02},
    {0x74,0xf2},
    {0x75,0x00},
    #endif
    {0x76,0x81},
    {0x77,0x8c},
    {0x78,0x81},
    {0xf5,0x26},//saturation[5] 0ff, cc[2] off, gamma[1] off
    {0xf6,0x00},
    {0xf0,0x36},
    {0x40,0x03},
    {0x41,0x01},
    {0xf0,0x39},
    {0x02,0x70},
    {0xf0,0x32},
    {0x41,0x00},
    {0x43,0x01},
    {0x48,0x02},
    {0xf0,0x45},
    {0x09,0x20},
    {0xf0,0x33},
    {0x33,0x10},
    {0xf0,0x30},
    {0x38,0x44},
    {0xf0,0x39},
    {0x07,0x00},//0x3907, 0x3908, blc target offset,
    //{0x08,0x19},// ori, ob will be 6
    {0x08,0x01},//set 1 then OB will close to 0
    {0x47,0x00},
    {0x48,0x00},
    {0xf4,0x08},
    {0xf0,0x37},
    {0x24,0x31},
    {0xf0,0x34},
    {0x9f,0x02},
    {0xa6,0x51},
    {0xa7,0x57},
    {0xe8,0x5f},
    {0xa8,0x50},
    {0xa9,0x50},
    {0xe9,0x50},
    {0xf0,0x33},
    {0xb3,0x58},
    {0xb2,0x78},
    {0xf0,0x34},
    {0x9f,0x03},
    {0xa6,0x51},
    {0xa7,0x57},
    {0xaa,0x01},
    {0xab,0x28},
    {0xac,0x01},
    {0xad,0x38},
    {0xf0,0x33},
    {0x0a,0x01},
    {0x0b,0x28},
    {0xf0,0x33},
    {0x64,0x0f},
    {0xec,0x51},
    {0xed,0x57},
    {0x06,0x58},
    {0xe9,0x58},
    {0xeb,0x68},
    {0xf0,0x33},
    {0x64,0x0f},
    {0xf0,0x36},
    {0x70,0xdf},
    {0xb6,0x40},
    {0xb7,0x51},
    {0xb8,0x53},
    {0xb9,0x57},
    {0xba,0x5f},
    {0xb0,0x84},
    {0xb1,0x82},
    {0xb2,0x84},
    {0xb3,0x88},
    {0xb4,0x90},
    {0xb5,0x90},
    {0xf0,0x36},
    {0x7e,0x50},
    {0x7f,0x51},
    {0x77,0x81},
    {0x78,0x86},
    {0x79,0x89},
    {0xf0,0x36},
    {0x70,0xdf},
    {0x9c,0x51},
    {0x9d,0x57},
    {0x90,0x54},
    {0x91,0x54},
    {0x92,0x56},
    {0xf0,0x36},
    {0xa0,0x51},
    {0xa1,0x57},
    {0x96,0x33},
    {0x97,0x43},
    {0x98,0x43},
    {0xf0,0x36},
    {0x70,0xdf},
    {0x7c,0x40},
    {0x7d,0x53},
    {0x74,0xd0},
    {0x75,0xf0},
    {0x76,0xf0},
    {0xf0,0x37},
    {0x0f,0xd5},
    {0x7a,0x40},
    {0x7b,0x57},
    {0x71,0x09},
    {0x72,0x09},
    {0x73,0x05},
    {0xf0,0x33},
    {0x01,0x44},
    {0xf0,0x36},
    {0x37,0xfb},
    {0xf0,0x36},
    {0x3c,0x0d},
    {0xf0,0x33},
    {0x14,0x95},
    {0xf0,0x33},
    {0x8f,0x80},
    {0xf0,0x37},
    {0x27,0x14},
    {0x28,0x03},
    {0xf0,0x36},
    {0x37,0xf4},
    {0xf0,0x33},
    {0x01,0x44},
    {0xf0,0x36},
    {0x79,0x89},
    {0xf0,0x34},
    {0xac,0x01},
    {0xad,0x40},
    {0xf0,0x33},
    {0xeb,0x70},
    {0xf0,0x34},
    {0xa8,0x50},
    {0xa9,0x50},
    {0xf0,0x33},
    {0xb3,0x58},
    {0xf0,0x36},
    {0x11,0x80},
    {0xf0,0x36},
    {0x41,0x51},
    {0xf0,0x3f},
    {0x03,0x03},
    {0xf0,0x36},
    {0xea,0x09},
    {0xf0,0x32},
    {0x0c,0x07}, // HTS
    {0x0d,0x80},
    {0x0e,0x02}, // VTS
    {0x0f,0xee},

    // 15fps
    {0x0e,0x05}, // VTS
    {0x0f,0xdc},

#if 1//TEST_MODE
    {0xf0,0x01},
    {0x00,0x80},
#endif
};
#endif
//TODO use data section
//static const uint8_t addr_list[] =
//{
//    0x6e
//};

#define SC101_ADDR          0x68
#define SC101_PAGE_ADDR     0xf0
#define BF_ID_MSB           0xda
#define BF_ID_LSB           0x4a
#define AWB_ENABLE          0xB0//[0]
#define AWB_RED_GAIN        0xc9
#define AWB_BLUE_GAIN       0xc8
#define AE_ENABLE           0x70//[1:0]
#define AE_TARGET           0x72//[6:0]
#define AE_EXPO_TIME_H      0x8D
#define AE_EXPO_TIME_L      0x8E
#define AE_GLOBAL_GAIN_H    0x8B //[3:0]
#define AE_GLOBAL_GAIN_L    0x8C
#define AE_DIGITAL_GAIN     0x9E
#define AE_LUMA_VAL         0x60 //0x0060 8bit
#define OTHER_MODULE        0xf5 //{0xf5,0x66},//contrast[6] 0ff,saturation[5] 0ff, cc[2] off, gamma[1] off

/**********************************************************************/
#if 1
//idx                                 0                                       9    10    11
//exp                                                                           (1/120),(1/60),(1/30),
//db                                                            ,-4608,-3072 -1536, 0,  1536, 3072, 4608, 6144, 7680, 9216, 10752, 12288, 13824, 15360,
int expo_map[EXPO_GAIN_TBL_NUM]    = {1,   1,   1,    1,  3 , 6 , 11, 23, 46,  93, 186,  373, 373,   373,  373,  373,   373,   373,  373,  746,  746};
int global_map[EXPO_GAIN_TBL_NUM]  = {32,  32,  32,  32,  32, 32, 32, 32, 32,  32,  32,   32,  64,   128,  256,   512, 1024,  2048, 4095, 4095, 4095};
int digital_map[EXPO_GAIN_TBL_NUM] = {16,  16,  16,  16,  16, 16, 16, 16, 16,  16,  16,   16,  16,    16,   16,    16,   16,    16,   16,   16,   16};

#else
//idx                                 0                                       9    10    11
//exp                                                                           (1/120),(1/60),(1/30),
//db                                                            ,-4608,-3072 -1536, 0,  1536, 3072, 4608,
int expo_map[EXPO_GAIN_TBL_NUM]    = {1,   1,   1,    1,  3 , 6 , 11, 23, 46,  93, 186,  373,  746,  746,   746,  746,  746,   746,   746,  746,  746};
int global_map[EXPO_GAIN_TBL_NUM]  = {32,  32,  32,  32,  32, 32, 32, 32, 32,  32,  32,   32,   32,   64,   128,  256,   512, 1024,  2048, 4095, 4095};
int digital_map[EXPO_GAIN_TBL_NUM] = {16,  16,  16,  16,  16, 16, 16, 16, 16,  16,  16,   16,   16,   16,    16,   16,    16,   16,    16,   16,   16};
#endif

static int __set_ae_auto(uint8_t enable)
{
    uint8_t enable_temp = 0;
    uint8_t page_num = 0x00;

    //AE ctrl, {0x70,0x68},//AEC[0], AGC[1]: 1~auto; 0~mannu
    SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);
    SCCB_Read(SC101_ADDR, AE_ENABLE, &enable_temp, 1);
    if (enable == 1) {
        enable_temp |= 0x3;
    } else if (enable == 0) {
        enable_temp &= ~0x3;
    }
    SCCB_Write(SC101_ADDR, AE_ENABLE, &enable_temp);

    return 0;
}

static int __set_awb_auto(uint8_t enable)
{
    uint8_t enable_temp = 0;
    uint8_t page_num = 0x00;

    //{0xb0,0xc0},//awb[0], joie
    SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);
    SCCB_Read(SC101_ADDR, AWB_ENABLE, &enable_temp, 1);
    if (enable == 1) {
        enable_temp |= 0x1;
    } else if (enable == 0) {
        enable_temp &= ~0x1;
    }
    SCCB_Write(SC101_ADDR, AWB_ENABLE, &enable_temp);

    return 0;
}

static int __set_blc_auto(uint8_t enable)
{
    uint8_t enable_temp = 0;
    uint8_t page_num = 0x39;

    SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);
    if (enable == SENSOR_CASE_BLC_ON) {
        enable_temp = 0x01;
        SCCB_Write(SC101_ADDR, 0x08, &enable_temp);//set 1 then OB will close to 0
    }
    
#if 0
    if (enable == 1) {//0x3907, 0x3908, blc target offset,
        page_num = 0x39;
        SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);// ori, ob will be 6

        enable_temp = 0x19;
        SCCB_Write(SC101_ADDR, 0x08, &enable_temp);
        /*{0x07,0x00},//0x3907, 0x3908, blc target offset
        {0x08,0x19},// ori, ob will be 6 */
    } else if (enable == 0) {
        page_num = 0x39;
        SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);

        enable_temp = 0x01;
        SCCB_Write(SC101_ADDR, 0x08, &enable_temp);//set 1 then OB will close to 0
        /*{0x07,0x00},//0x3907, 0x3908, blc target offset
       {0x08,0x01},//set 1 then OB will close to 0*/
    }
#endif

    return 0;
}

//Modules related to brightness and color
static int __set_other_module_auto(uint8_t enable)
{
    uint8_t enable_temp = 0;
    uint8_t page_num = 0x00;

    //{0xf5,0x66},//contrast[6] 0ff,saturation[5] 0ff, cc[2] off, gamma[1] off
    SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);
    if (enable == 1) {//data_enable = 0x0;//enable sat, cc, gamma
        //enable_temp  = 0x0;
        enable_temp  = 0x40;// cont, off
    } else if (enable == 0) {
        enable_temp = 0x66;// cont, sat, cc, gamma off
        //enable_temp = 0x26;// sat, cc, gamma off
        //enable_temp = 0x6;// cc, gamma off
        //enable_temp = 0x2;// gamma off
    } else if (enable == SENSOR_CASE_GAMMA_ON) {
        enable_temp = 0x64;// cont, sat, cc,  off, gamma on
   }
    SCCB_Write(SC101_ADDR, OTHER_MODULE, &enable_temp);

    return 0;
}

/******************************************************/
static int sc101iot_get_register(int addr, int *value)
{
    uint8_t val = 0;

    SCCB_Read(SC101_ADDR, addr, &val, 1);
    *value = val;

    return 0;
}

static int sc101iot_set_register(int addr, int value)
{
    SCCB_Write(SC101_ADDR, addr, (uint8_t *)&value);

    return 0;
}

static int sc101iot_get_ae_register(uint8_t case_id, int *value)
{
    uint8_t page_num = 0x00;
    uint8_t val = 0;
    uint16_t expo = 0, global_gain = 0;
    uint8_t expo_H = 0, global_gain_H = 0, expo_L = 0, global_gain_L = 0;

    if (case_id == SENSOR_CASE_GET_AE_LUMA_AVG) {//read sensor luma avg
        page_num = 0x00;
        SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);

        SCCB_Read(SC101_ADDR, AE_LUMA_VAL, &val, 1);
        //printf("luma_avg %d\r\n", val);
        *value = val;
     } else if (case_id == SENSOR_CASE_GET_AE_SYS_GAIN) {//expo+gain
        page_num = 0x00;
        SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);

        SCCB_Read(SC101_ADDR, AE_EXPO_TIME_H, &expo_H, 1);
        SCCB_Read(SC101_ADDR, AE_EXPO_TIME_L, &expo_L, 1);
        expo = (expo_H << 8) + expo_L;

        SCCB_Read(SC101_ADDR, AE_GLOBAL_GAIN_H, &global_gain_H, 1);
        SCCB_Read(SC101_ADDR, AE_GLOBAL_GAIN_L, &global_gain_L, 1);
        global_gain = (global_gain_H << 8) + global_gain_L;

        ana_ae_gain_from_expo(expo, global_gain, 16, expo_map, global_map, digital_map, value);
        //printf("===== value %d, expo %d, expo_ 0x%x 0x%x, global_gain %d, global_gain_ 0x%x 0x%x\r\n", 
        //    *value, expo,  expo_H, expo_L, global_gain, global_gain_H,  global_gain_L);
    }

    return val;
}

static int sc101iot_set_ae_expo_gain(uint8_t enable, int gain_db)
{
    uint8_t page_num = 0x00;
    uint16_t expo = 0, global_gain = 0;
    uint8_t expo_H = 0, global_gain_H = 0, expo_L = 0, global_gain_L = 0;
    uint8_t global_gain_temp = 0, digital_gain = 0;

    ana_ae_gain(gain_db, expo_map, global_map, digital_map, &expo, &global_gain, &digital_gain);
    //printf("===== gain_db %d,  expo %d , global_gain %d , digital_gain %d\r\n", gain_db, expo, global_gain, digital_gain);

    if (enable <= 1) {
        if (enable) __set_ae_auto(1);
        else __set_ae_auto(0);
    } else if (enable == SENSOR_CASE_SET_AE_EXPO) {//expo
        page_num = 0x00;
        SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);

        expo_L = expo & 0xff;
        expo_H = (expo >> 8) & 0xff;
        SCCB_Write(SC101_ADDR, AE_EXPO_TIME_H, (uint8_t*)&expo_H);
        SCCB_Write(SC101_ADDR, AE_EXPO_TIME_L, (uint8_t*)&expo_L);

        printf("===== gain_db %d,  expo_ 0x%x 0x%x,\r\n", gain_db, expo_H, expo_L);
    } else if (enable == SENSOR_CASE_SET_AE_AGAIN) {//gain
        page_num = 0x00;
        SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);

        SCCB_Read(SC101_ADDR, AE_GLOBAL_GAIN_H, &global_gain_temp, 1);
        global_gain_H = (global_gain_temp & 0xf0) + (global_gain >> 8);
        global_gain_L = global_gain & 0xff;
        SCCB_Write(SC101_ADDR, AE_GLOBAL_GAIN_H, (uint8_t*)&global_gain_H);
        SCCB_Write(SC101_ADDR, AE_GLOBAL_GAIN_L, (uint8_t*)&global_gain_L);

        printf("===== gain_db %d,  global_gain_ 0x%x 0x%x\r\n", gain_db, global_gain_H,  global_gain_L);
     } else if (enable == SENSOR_CASE_SET_AE_SYS_GAIN) {//expo+gain
        page_num = 0x00;
        SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);

        expo_L = expo & 0xff;
        expo_H = (expo >> 8) & 0xff;
        SCCB_Write(SC101_ADDR, AE_EXPO_TIME_H, (uint8_t*)&expo_H);
        SCCB_Write(SC101_ADDR, AE_EXPO_TIME_L, (uint8_t*)&expo_L);

        SCCB_Read(SC101_ADDR, AE_GLOBAL_GAIN_H, &global_gain_temp, 1);
        global_gain_H = (global_gain_temp & 0xf0) + (global_gain >> 8);
        global_gain_L = global_gain & 0xff;
        SCCB_Write(SC101_ADDR, AE_GLOBAL_GAIN_H, (uint8_t*)&global_gain_H);
        SCCB_Write(SC101_ADDR, AE_GLOBAL_GAIN_L, (uint8_t*)&global_gain_L);

        // printf("===== gain_db %d, expo_ 0x%x 0x%x, global_gain_ 0x%x 0x%x\r\n", gain_db,  expo_H, expo_L, global_gain_H,  global_gain_L);
    }

    return 0;
}

#define SENSOR_1X   16
#define SENSOR_STEP 128
#define ALGO_1X     4096
static uint8_t sc101iot_ana_awb_gain(int gain)
{
    uint8_t ret_gain = SENSOR_1X;

    /*sensor_val = real_val - 0x70, real_value = (g_val/r_b_val) * 128*/
    ret_gain = CLIP(SENSOR_1X, 255, (gain * SENSOR_STEP / ALGO_1X) - 112);

    return ret_gain;
}

static int sc101iot_get_awb_register(uint8_t case_id, int *value)
{
    uint8_t page_num = 0x00;
    uint8_t val = 0;

    if (case_id == SENSOR_CASE_GET_AWB_RGAIN) {
        page_num = 0x00;
        SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);

        SCCB_Read(SC101_ADDR, AWB_RED_GAIN, &val, 1);
        *value = val;
    } else if (case_id == SENSOR_CASE_GET_AWB_BGAIN) {
        page_num = 0x00;
        SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);

        SCCB_Read(SC101_ADDR, AWB_BLUE_GAIN, &val, 1);
        *value = val;
    }
    return val;
}

static int sc101iot_set_awb_gain(uint8_t enable, int r_gain, int b_gain)
{
    uint8_t page_num = 0x00;
    uint8_t rgain, bgain;

    rgain = sc101iot_ana_awb_gain(r_gain);
    bgain = sc101iot_ana_awb_gain(b_gain);
    //printf("sc101iot_set_awb_gain %d  %d %d %d\r\n", r_gain, b_gain, rgain, bgain);

    if (enable <= 1) {
        __set_awb_auto(enable);

        page_num = 0x00;
        SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);

        SCCB_Write(SC101_ADDR, AWB_RED_GAIN, &rgain);
        SCCB_Write(SC101_ADDR, AWB_BLUE_GAIN, &bgain);
    } else if (enable == SENSOR_CASE_SET_AWB_RGAIN_BGAIN) {
        page_num = 0x00;
        SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);

        SCCB_Write(SC101_ADDR, AWB_RED_GAIN, &rgain);
        SCCB_Write(SC101_ADDR, AWB_BLUE_GAIN, &bgain);
    }

    return 0;
}

static int sc101iot_set_case(uint8_t case_id)
{
    /* auto blc */
    __set_blc_auto(SENSOR_CASE_BLC_ON);

    if (case_id == SENSOR_CASE_FAST_INIT) {
        /* ae */
        __set_ae_auto(0);

        /* awb */
        __set_awb_auto(0);
        sc101iot_set_awb_gain(SENSOR_CASE_SET_AWB_RGAIN_BGAIN, ALGO_1X, ALGO_1X);//rgain, bgain 1x

        /* other */
        __set_other_module_auto(0); 
    } else if (case_id == SENSOR_CASE_NORMAL) {
        /* ae */
        __set_ae_auto(1);

        /* awb */
        __set_awb_auto(1);

        /* other */
        __set_other_module_auto(1); 
    } else if (case_id == SENSOR_CASE_CALIBRATION) {//calib mode
        /* ae */
        __set_ae_auto(1);

        /* awb */
        __set_awb_auto(0);
        sc101iot_set_awb_gain(SENSOR_CASE_SET_AWB_RGAIN_BGAIN, ALGO_1X, ALGO_1X);//rgain, bgain 1x

        /* other */
        __set_other_module_auto(0);
    } else if (case_id == SENSOR_CASE_GAMMA_ON) {//apply other mode
        /* other */
        __set_other_module_auto(SENSOR_CASE_GAMMA_ON);
    } else if (case_id == SENSOR_CASE_OTHER_MODULE_ON) {//apply other mode
        /* other */
        __set_other_module_auto(1);
    } else if (case_id == SENSOR_CASE_AE_AUTO) {
        /* ae */
        __set_ae_auto(1);
    } else if (case_id == SENSOR_CASE_AWB_AUTO) {
        /* awb */
        __set_awb_auto(1);
    } else if (case_id == SENSOR_CASE_AE_MANUAL) {
        /* ae */
        __set_ae_auto(0);
    } else if (case_id == SENSOR_CASE_AWB_MANUAL) {
        /* awb */
        __set_awb_auto(0);
    }

    return 0;
}

int sc101iot_probe(void)
{
    //printf("------------------sc101iot_probe---------------\r\n");
    uint8_t id_msb = 0, id_lsb = 0;
    SCCB_Read(SC101_ADDR, 0xf7, &id_msb, 1);
    //printf("------------------%02x \r\n", id_msb);
    SCCB_Read(SC101_ADDR, 0xf8, &id_lsb, 1);
    //printf("------------------%02x \r\n", id_lsb);

    if(BF_ID_MSB == id_msb && BF_ID_LSB == id_lsb)
        return 0;
    else
        return -1;
}

int sc101iot_reset(void)
{
    uint8_t page_num = 0x31;
    uint8_t reset = 0x1;

    page_num = 0x01;
    SCCB_Write(SC101_ADDR, SC101_PAGE_ADDR, &page_num);
    SCCB_Write(SC101_ADDR, 0x03, &reset);

    return 0;
}

int sc101iot_setFMT(void)
{
    int i;

    for (i = 0; i < sizeof(sc101iot_reglist)/sizeof(sc101iot_reglist[0]); i++) {
        SCCB_Write(SC101_ADDR, sc101iot_reglist[i][0], (uint8_t*)&sc101iot_reglist[i][1]);
    }

    return 0;
}

const rt_camera_desc __rt_camera_sc101iot_desc __attribute__((section(".camera_desc"))) =
{
    .name = "__rti_name_sc101iot",
    .addr = SC101_ADDR,
    .frm_vld_high = 1,
    .probe = sc101iot_probe,
    .reset = sc101iot_reset,
    .setFMT = sc101iot_setFMT,
    .set_case = sc101iot_set_case,
    .set_awb_gain = sc101iot_set_awb_gain,
    .get_awb_register = sc101iot_get_awb_register,
    .set_ae_expo_gain = sc101iot_set_ae_expo_gain,
    .get_ae_register = sc101iot_get_ae_register,
    .set_register = sc101iot_set_register,
    .get_register = sc101iot_get_register,
    #if resolution_720P
    .width = 1280,
    .height = 720,
    #else
    .width = 320,
    .height = 240,
    #endif
    #ifdef PCLK_36M
    .pclk = 36000000,
    #else
    .pclk = 72000000,
    #endif
    .total_width = 1600,
    .total_height = 750,
};

