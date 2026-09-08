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


#ifndef __DEMO_ISP_CFG_H_
#define __DEMO_ISP_CFG_H_

#define ISP_IN_720P_CROP_NONE_OUT0_1080P_NV12_OUT1_360P_RGB
// #define ISP_IN_720P_CROP_360P_OUT0_720P_YUYV_OUT1_360P_ARGB
// #define ISP_IN_720P_CROP_NONE_OUT0_NONE_OUT1_VGA_RGB
// #define ISP_IN_720P_CROP_NONE_OUT0_360x640_Y_OUT1_NONE
// #define ISP_IN_VGA_CROP_NONE_OUT0_1280x960_NV12_OUT1_VGA_RGB
// #define ISP_IN_720P_CROP_NONE_OUT0_720P_YUYV_OUT1_360P_RGB_OSD_DRAW

#if defined(ISP_IN_720P_CROP_NONE_OUT0_1080P_NV12_OUT1_360P_RGB)
    #define IN_WIDTH      (1280)
    #define IN_HEIGHT     (720)

    #define CROP_START_X  (0)
    #define CROP_START_Y  (0)
    #define CROP_WIDTH    (1280)
    #define CROP_HEIGHT   (720)

    #define OUT_WIDTH_0   (1920)
    #define OUT_HEIGHT_0  (1080)
    #define OUT_FORMAT_0  (ISP_HAL_OUT_FMT0_NV12)

    #define OUT_WIDTH_1   (640)
    #define OUT_HEIGHT_1  (360)
    #define OUT_FORMAT_1  (ISP_HAL_OUT_FMT1_RGB)
#endif

#if defined(ISP_IN_720P_CROP_360P_OUT0_720P_YUYV_OUT1_360P_ARGB)
    #define IN_WIDTH      (1280)
    #define IN_HEIGHT     (720)

    #define CROP_START_X  (320)
    #define CROP_START_Y  (180)
    #define CROP_WIDTH    (640)
    #define CROP_HEIGHT   (360)

    #define OUT_WIDTH_0   (1280)
    #define OUT_HEIGHT_0  (720)
    #define OUT_FORMAT_0  (ISP_HAL_OUT_FMT0_YUYV)

    #define OUT_WIDTH_1   (640)
    #define OUT_HEIGHT_1  (360)
    #define OUT_FORMAT_1  (ISP_HAL_OUT_FMT1_ARGB)
#endif
#if defined(ISP_IN_720P_CROP_NONE_OUT0_NONE_OUT1_VGA_RGB)
    #define IN_WIDTH      (1280)
    #define IN_HEIGHT     (720)

    #define CROP_START_X  (0)
    #define CROP_START_Y  (0)
    #define CROP_WIDTH    (1280)
    #define CROP_HEIGHT   (720)

    #define OUT_WIDTH_0   (0)
    #define OUT_HEIGHT_0  (0)
    #define OUT_FORMAT_0  (ISP_HAL_OUT_FMT0_YUYV)

    #define OUT_WIDTH_1   (640)
    #define OUT_HEIGHT_1  (360)
    #define OUT_FORMAT_1  (ISP_HAL_OUT_FMT1_ARGB)
#endif
#if defined(ISP_IN_720P_CROP_NONE_OUT0_360x640_Y_OUT1_NONE)
    #define IN_WIDTH      (1280)
    #define IN_HEIGHT     (720)

    #define CROP_START_X  (0)
    #define CROP_START_Y  (0)
    #define CROP_WIDTH    (1280)
    #define CROP_HEIGHT   (720)

    #define OUT_WIDTH_0   (360)
    #define OUT_HEIGHT_0  (640)
    #define OUT_FORMAT_0  (ISP_HAL_OUT_FMT0_Y_ONLY)

    #define OUT_WIDTH_1   (0)
    #define OUT_HEIGHT_1  (0)
    #define OUT_FORMAT_1  (ISP_HAL_OUT_FMT1_ARGB)
#endif
#if defined(ISP_IN_VGA_CROP_NONE_OUT0_1280x960_NV12_OUT1_VGA_RGB)
    #define IN_WIDTH      (640)
    #define IN_HEIGHT     (480)

    #define CROP_START_X  (0)
    #define CROP_START_Y  (0)
    #define CROP_WIDTH    (640)
    #define CROP_HEIGHT   (480)

    #define OUT_WIDTH_0   (1280)
    #define OUT_HEIGHT_0  (960)
    #define OUT_FORMAT_0  (ISP_HAL_OUT_FMT0_Y_ONLY)

    #define OUT_WIDTH_1   (640)
    #define OUT_HEIGHT_1  (480)
    #define OUT_FORMAT_1  (ISP_HAL_OUT_FMT1_RGB)
#endif
#if defined(ISP_IN_720P_CROP_NONE_OUT0_720P_YUYV_OUT1_360P_RGB_OSD_DRAW)
    #define IN_WIDTH      (1280)
    #define IN_HEIGHT     (720)

    #define CROP_START_X  (0)
    #define CROP_START_Y  (0)
    #define CROP_WIDTH    (1280)
    #define CROP_HEIGHT   (720)

    #define OUT_WIDTH_0   (1280)
    #define OUT_HEIGHT_0  (720)
    #define OUT_FORMAT_0  (ISP_HAL_OUT_FMT0_YUYV)

    #define OUT_WIDTH_1   (640)
    #define OUT_HEIGHT_1  (360)
    #define OUT_FORMAT_1  (ISP_HAL_OUT_FMT1_RGB)

    #define OSD_DRAW_EN
    #define OSD_BLEND_EN
#endif

#endif
