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


#ifndef __DEMO_MJPEG_CFG_H_
#define __DEMO_MJPEG_CFG_H_

void dummyprintf(const char * format, ... );

#if 1
#define DEMO_MJPEG_PRINTF   printf
#else
#define DEMO_MJPEG_PRINTF   dummyprintf
#endif

//#define USE_OCRAM_FOR_INPUT_RING                /*define to run input ring cases using OCRAM*/
//#define MJENC_TEST_HW_AUTO_MODE_FOREVERLOOP_EN  /*define to run input ring cases in forever loop*/
//#define MJENC_TEST_MENUAL_POP_FRAME_EN          /*define to test menual pop frame to simulate long pop frame period*/


#define _ENCODE_TEST_SAMPLE_VERIFICATION_
#if defined(_ENCODE_TEST_SAMPLE_VERIFICATION_) /*Encode Test sample*/
    /******************************************************/
    /*Define to select 1 test case for running encode test*/
    /******************************************************/
    //#define MJENC_TEST_CASE_000_1920x1080_YUV420SP_UV

    //#define MJENC_TEST_CASE_ID_1  /*checksum error, 1 byte diff*/     /*ipro6 ok*//*ocram only ok*/
    //#define MJENC_TEST_CASE_ID_2  /*checksum error, 1 byte diff*/     /*ipro6 ok*//*ocram only ok*/
    //#define MJENC_TEST_CASE_ID_3                                      /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_4  /*checksum error, 1 byte diff*/
    //#define MJENC_TEST_CASE_ID_5                                      /*ipro6 ok*//*ocram only ok*/
    //#define MJENC_TEST_CASE_ID_6  /*checksum error, 1 byte diff*/     /*ipro6 ok*//*ocram only ok*/
    //#define MJENC_TEST_CASE_ID_7                                      /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_8  /*checksum error, 1 byte diff*/
    //#define MJENC_TEST_CASE_ID_9  /*checksum error, 1 byte diff*/     /*ipro6 ok*//*ocram only ok*/
    //#define MJENC_TEST_CASE_ID_10 /*checksum error, 1 byte diff*/     /*ipro6 ok*//*ocram only ok*/
    //#define MJENC_TEST_CASE_ID_11
    //#define MJENC_TEST_CASE_ID_12
    //#define MJENC_TEST_CASE_ID_13                                     /*ipro6 ok*//*ocram only ok*/
    //#define MJENC_TEST_CASE_ID_14                                     /*ipro6 ok*//*ocram only ok*/
    //#define MJENC_TEST_CASE_ID_15                                     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_16
    //#define MJENC_TEST_CASE_ID_17 /*checksum error, 1 byte diff*/     /*ipro6 ok*//*ocram only ok*/
    //#define MJENC_TEST_CASE_ID_18 /*checksum error, 1 byte diff*/     /*ipro6 ok*//*ocram only ok*/
    //#define MJENC_TEST_CASE_ID_19 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_20 /*checksum error, 1 byte diff*/
    //#define MJENC_TEST_CASE_ID_21                                     /*ipro6 ok*//*ocram only ok*/
    #define MJENC_TEST_CASE_ID_22                                     /*ipro6 ok*//*ocram only ok*/
    //#define MJENC_TEST_CASE_ID_23 /*checksum error, 1 byte diff*/
    //#define MJENC_TEST_CASE_ID_24 /*checksum error, 1 byte diff*/
    //#define MJENC_TEST_CASE_ID_25 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_26 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_27 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_28 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_29 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_30                                     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_31 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_32                                     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_33 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_34 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_35 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_36 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_37 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_38 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_39 /*checksum error, 1 byte diff*/     /*ipro6 ok*/
    //#define MJENC_TEST_CASE_ID_40 /*checksum error, 1 byte diff*/     /*ipro6 ok*/

    /*case41-44 for tsrc2mjpeg tsrc read 1280x720 and flyby to mjpeg test*//*IPRO6 has no tsrc and need intput from sensor for testing input ring*/
    //#define MJENC_TEST_CASE_ID_41     /*YUV400*/
    //#define MJENC_TEST_CASE_ID_42     /*YUV420sp*/
    //#define MJENC_TEST_CASE_ID_43     /*YUV422sp*/
    //#define MJENC_TEST_CASE_ID_44     /*YUV422 YUYV*/


    #if defined(MJENC_TEST_CASE_000_1920x1080_YUV420SP_UV)
        #define MJENC_TEST_CASE_ID                          (0)
        #define MJENC_TEST_CASE_INFO                        "1920x1080_YUV420SP_UV"
        #define MJENC_TEST_JPG_ENC_YUV_MODE                 MJPEG_HAL_FORMAT_YUV420SP_NV12
        #define MJENC_TEST_JPG_ENC_Q_VALUIE                 (50)
        #define MJENC_TEST_IMG_W                            (1920)
        #define MJENC_TEST_IMG_H                            (1080)
        #define MJENC_TEST_SUM_TARGET                       /*(0x70bb0b0a)*/(0xd87b772e)
        #define MJENC_TEST_XOR_TARGET                       /*(0x3b75c446)*/(0x58741668)
        #define MJENC_TEST_OUT_SWAP_MODE_EN                 (0)
        #define MJENC_TEST_HW_AUTO_MODE_EN                  (0) /*define to run HW auto mode, use input ring, undefine to run SW mode.*/

        #define MJENC_TEST_RUN_SW_MODE_SEL                  (1) /*0: run SW frame mode, 1:run SW kick mode.*/
    #endif

    #if defined( MJENC_TEST_CASE_ID_1)
        #define MJENC_TEST_CASE_ID (1)
        #define MJENC_TEST_CASE_INFO ("cafe_422_to_400.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_GRAY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0x9b3eb4f7)
        #define MJENC_TEST_XOR_TARGET (0x2ac4f8b1)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (0)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_2)
        #define MJENC_TEST_CASE_ID (2)
        #define MJENC_TEST_CASE_INFO ("Cb_19_422_to_400.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_GRAY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0x3340e9c6)
        #define MJENC_TEST_XOR_TARGET (0x8da9852a)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_3)
        #define MJENC_TEST_CASE_ID (3)
        #define MJENC_TEST_CASE_INFO ("HD_1_422_to_400.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_GRAY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (1280)
        #define MJENC_TEST_IMG_H (720)
        #define MJENC_TEST_SUM_TARGET (0xfe169a54)
        #define MJENC_TEST_XOR_TARGET (0x2d0d3182)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_4)
        #define MJENC_TEST_CASE_ID (4)
        #define MJENC_TEST_CASE_INFO ("FHD_3_422_to_400.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_GRAY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (1920)
        #define MJENC_TEST_IMG_H (1080)
        #define MJENC_TEST_SUM_TARGET (0xdc3391ca)
        #define MJENC_TEST_XOR_TARGET (0x1fdb88d4)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_5)
        #define MJENC_TEST_CASE_ID (5)
        #define MJENC_TEST_CASE_INFO ("cafe_420_to_420sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV12)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0x27615bd6)
        #define MJENC_TEST_XOR_TARGET (0x73c114e6)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_6)
        #define MJENC_TEST_CASE_ID (6)
        #define MJENC_TEST_CASE_INFO ("Cb_19_420_to_420sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV12)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0x7f91d1c1)
        #define MJENC_TEST_XOR_TARGET (0xbf3c9101)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_7)
        #define MJENC_TEST_CASE_ID (7)
        #define MJENC_TEST_CASE_INFO ("HD_1_420_to_420sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV12)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (1280)
        #define MJENC_TEST_IMG_H (720)
        #define MJENC_TEST_SUM_TARGET (0x792adb32)
        #define MJENC_TEST_XOR_TARGET (0x705ae314)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_8)
        #define MJENC_TEST_CASE_ID (8)
        #define MJENC_TEST_CASE_INFO ("FHD_3_420_to_420sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV12)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (1920)
        #define MJENC_TEST_IMG_H (1080)
        #define MJENC_TEST_SUM_TARGET (0x65cd0b73)
        #define MJENC_TEST_XOR_TARGET (0xfe12a9a3)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_9)
        #define MJENC_TEST_CASE_ID (9)
        #define MJENC_TEST_CASE_INFO ("cafe_422_to_422sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV16)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0xb8c59f59)
        #define MJENC_TEST_XOR_TARGET (0x43a58a35)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_10)
        #define MJENC_TEST_CASE_ID (10)
        #define MJENC_TEST_CASE_INFO ("Cb_19_422_to_422sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV16)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0xcf70d113)
        #define MJENC_TEST_XOR_TARGET (0xbea78e45)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_11)
        #define MJENC_TEST_CASE_ID (11)
        #define MJENC_TEST_CASE_INFO ("HD_1_422_to_422sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV16)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (1280)
        #define MJENC_TEST_IMG_H (720)
        #define MJENC_TEST_SUM_TARGET (0xdbe772a9)
        #define MJENC_TEST_XOR_TARGET (0x58577b33)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_12)
        #define MJENC_TEST_CASE_ID (12)
        #define MJENC_TEST_CASE_INFO ("FHD_3_422_to_422sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV16)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (1920)
        #define MJENC_TEST_IMG_H (1080)
        #define MJENC_TEST_SUM_TARGET (0x36df5beb)
        #define MJENC_TEST_XOR_TARGET (0x3b3f7165)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_13)
        #define MJENC_TEST_CASE_ID (13)
        #define MJENC_TEST_CASE_INFO ("cafe_422_to_400.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_GRAY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0xc535bffb)
        #define MJENC_TEST_XOR_TARGET (0xdcd33115)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_14)
        #define MJENC_TEST_CASE_ID (14)
        #define MJENC_TEST_CASE_INFO ("Cb_19_422_to_400.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_GRAY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0x71549a86)
        #define MJENC_TEST_XOR_TARGET (0xe3bf8e38)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_15)
        #define MJENC_TEST_CASE_ID (15)
        #define MJENC_TEST_CASE_INFO ("HD_1_422_to_400.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_GRAY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (1280)
        #define MJENC_TEST_IMG_H (720)
        #define MJENC_TEST_SUM_TARGET (0xc2511752)
        #define MJENC_TEST_XOR_TARGET (0x882f5ff6)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_16)
        #define MJENC_TEST_CASE_ID (16)
        #define MJENC_TEST_CASE_INFO ("FHD_3_422_to_400.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_GRAY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (1920)
        #define MJENC_TEST_IMG_H (1080)
        #define MJENC_TEST_SUM_TARGET (0x87c41172)
        #define MJENC_TEST_XOR_TARGET (0x1e156554)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_17)
        #define MJENC_TEST_CASE_ID (17)
        #define MJENC_TEST_CASE_INFO ("cafe_420_to_420sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV12)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0xdb637f2c)
        #define MJENC_TEST_XOR_TARGET (0xf5a7fc10)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_18)
        #define MJENC_TEST_CASE_ID (18)
        #define MJENC_TEST_CASE_INFO ("Cb_19_420_to_420sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV12)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0x94200c7f)
        #define MJENC_TEST_XOR_TARGET (0xa8b59b25)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_19)
        #define MJENC_TEST_CASE_ID (19)
        #define MJENC_TEST_CASE_INFO ("HD_1_420_to_420sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV12)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (1280)
        #define MJENC_TEST_IMG_H (720)
        #define MJENC_TEST_SUM_TARGET (0x23eb4dbf)
        #define MJENC_TEST_XOR_TARGET (0x128ed659)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_20)
        #define MJENC_TEST_CASE_ID (20)
        #define MJENC_TEST_CASE_INFO ("FHD_3_420_to_420sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV12)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (1920)
        #define MJENC_TEST_IMG_H (1080)
        #define MJENC_TEST_SUM_TARGET (0x1a07982b)
        #define MJENC_TEST_XOR_TARGET (0x5710b387)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_21)
        #define MJENC_TEST_CASE_ID (21)
        #define MJENC_TEST_CASE_INFO ("cafe_422_to_422sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV16)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0x86334463)
        #define MJENC_TEST_XOR_TARGET (0x461046f1)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_22)
        #define MJENC_TEST_CASE_ID (22)
        #define MJENC_TEST_CASE_INFO ("Cb_19_422_to_422sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV16)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (240)
        #define MJENC_TEST_IMG_H (320)
        #define MJENC_TEST_SUM_TARGET (0x53d1fc98)
        #define MJENC_TEST_XOR_TARGET (0x116f32c4)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (0)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)

        #define MJENC_TEST_RUN_SW_MODE_SEL                  (1) /*0: run SW frame mode, 1:run SW kick mode.*/
    #endif
    #if defined( MJENC_TEST_CASE_ID_23)
        #define MJENC_TEST_CASE_ID (23)
        #define MJENC_TEST_CASE_INFO ("HD_1_422_to_422sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV16)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (1280)
        #define MJENC_TEST_IMG_H (720)
        #define MJENC_TEST_SUM_TARGET (0x5b293866)
        #define MJENC_TEST_XOR_TARGET (0xdf8d1dee)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_24)
        #define MJENC_TEST_CASE_ID (24)
        #define MJENC_TEST_CASE_INFO ("FHD_3_422_to_422sp_uv.yuv")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV16)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (1920)
        #define MJENC_TEST_IMG_H (1080)
        #define MJENC_TEST_SUM_TARGET (0xdce2aea)
        #define MJENC_TEST_XOR_TARGET (0x8bc41b6c)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_25)
        #define MJENC_TEST_CASE_ID (25)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_420SP_Y_CbCr.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV12)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0xf8c70b58)
        #define MJENC_TEST_XOR_TARGET (0xd2ef261e)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_26)
        #define MJENC_TEST_CASE_ID (26)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_420SP_Y_CrCb.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV21)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0xb21f1873)
        #define MJENC_TEST_XOR_TARGET (0xc8095b6f)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_27)
        #define MJENC_TEST_CASE_ID (27)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422SP_Y_CbCr.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV16)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0xba03db85)
        #define MJENC_TEST_XOR_TARGET (0x8d5d5ed5)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_28)
        #define MJENC_TEST_CASE_ID (28)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422SP_Y_CrCb.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV61)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0xe239a3e5)
        #define MJENC_TEST_XOR_TARGET (0xb6643525)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_29)
        #define MJENC_TEST_CASE_ID (29)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422_UYVY.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422_UYVY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0xba03db85)
        #define MJENC_TEST_XOR_TARGET (0x8d5d5ed5)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_30)
        #define MJENC_TEST_CASE_ID (30)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422_VYUY.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422_VYUY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0xddf4e063)
        #define MJENC_TEST_XOR_TARGET (0x8af1547)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_31)
        #define MJENC_TEST_CASE_ID (31)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422_YUYV.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422_YUYV)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0x1a011fec)
        #define MJENC_TEST_XOR_TARGET (0x41a946b6)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_32)
        #define MJENC_TEST_CASE_ID (32)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422_YVYU.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422_YVYU)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0x35c5f44d)
        #define MJENC_TEST_XOR_TARGET (0xc3f0eebd)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_33)
        #define MJENC_TEST_CASE_ID (33)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_420SP_Y_CbCr.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV12)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0x28811764)
        #define MJENC_TEST_XOR_TARGET (0x6ed7ab2e)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_34)
        #define MJENC_TEST_CASE_ID (34)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_420SP_Y_CrCb.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV21)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0xb3408494)
        #define MJENC_TEST_XOR_TARGET (0x94ddb0b6)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_35)
        #define MJENC_TEST_CASE_ID (35)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422SP_Y_CbCr.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV16)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0x671af70d)
        #define MJENC_TEST_XOR_TARGET (0x20ff53e7)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_36)
        #define MJENC_TEST_CASE_ID (36)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422SP_Y_CrCb.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV61)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0x26c6afb6)
        #define MJENC_TEST_XOR_TARGET (0x93343a2)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_37)
        #define MJENC_TEST_CASE_ID (37)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422_UYVY.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422_UYVY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0x671af70d)
        #define MJENC_TEST_XOR_TARGET (0x20ff53e7)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_38)
        #define MJENC_TEST_CASE_ID (38)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422_VYUY.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422_VYUY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0x95fb437a)
        #define MJENC_TEST_XOR_TARGET (0x25451b22)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_39)
        #define MJENC_TEST_CASE_ID (39)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422_YUYV.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422_YUYV)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0xbff2856b)
        #define MJENC_TEST_XOR_TARGET (0x7f27cac9)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif
    #if defined( MJENC_TEST_CASE_ID_40)
        #define MJENC_TEST_CASE_ID (40)
        #define MJENC_TEST_CASE_INFO ("ForJPEG_640x480_ycc444_422_YVYU.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422_YVYU)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (100)
        #define MJENC_TEST_IMG_W (640)
        #define MJENC_TEST_IMG_H (480)
        #define MJENC_TEST_SUM_TARGET (0x1a08b3a9)
        #define MJENC_TEST_XOR_TARGET (0xc626d43b)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (0)
        #define MJENC_TEST_HW_AUTO_MODE_EN (0)
    #endif

    #if defined( MJENC_TEST_CASE_ID_41)
        #define MJENC_TEST_CASE_ID (41)
        #define MJENC_TEST_CASE_INFO ("Tsrc2Mjpeg_1920x1080_400.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_GRAY)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (1920)
        #define MJENC_TEST_IMG_H (1080)
        #define MJENC_TEST_SUM_TARGET (0x80d8b569)
        #define MJENC_TEST_XOR_TARGET (0xf9a91f51)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (0)
        #define MJENC_TEST_HW_AUTO_MODE_EN (1)
        #define TSRC_OUT_RING_LINE     (8*2)                        /*min 8 line x2 */
        #define TSRC_OUT_FORMAT        (ISP_TSRC_OUT_FMT0_Y_ONLY)   /*ISP_TSRC_OUT_FMT0_YUYV, ISP_TSRC_OUT_FMT0_YUV422SP, ISP_TSRC_OUT_FMT0_NV12, ISP_TSRC_OUT_FMT0_Y_ONLY*/
    #endif
    #if defined( MJENC_TEST_CASE_ID_42)
        #define MJENC_TEST_CASE_ID (42)
        #define MJENC_TEST_CASE_INFO ("Tsrc2Mjpeg_1920x1080_420sp_Y_CbCr.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV420SP_NV12)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (1920)
        #define MJENC_TEST_IMG_H (1080)
        #define MJENC_TEST_SUM_TARGET (0x9a55a95c)
        #define MJENC_TEST_XOR_TARGET (0x822c5580)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (0)
        #define MJENC_TEST_HW_AUTO_MODE_EN (1)
        #define TSRC_OUT_RING_LINE     (16*2)       /*min 16 line x2 */
        #define TSRC_OUT_FORMAT        (ISP_TSRC_OUT_FMT0_NV12)  /*ISP_TSRC_OUT_FMT0_YUYV, ISP_TSRC_OUT_FMT0_YUV422SP, ISP_TSRC_OUT_FMT0_NV12, ISP_TSRC_OUT_FMT0_Y_ONLY*/
    #endif
    #if defined( MJENC_TEST_CASE_ID_43)
        #define MJENC_TEST_CASE_ID (43)
        #define MJENC_TEST_CASE_INFO ("Tsrc2Mjpeg_1920x1080_422sp_Y_CbCr.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422SP_NV16)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (1920)
        #define MJENC_TEST_IMG_H (1080)
        #define MJENC_TEST_SUM_TARGET (0x9654f44d)
        #define MJENC_TEST_XOR_TARGET (0xa2072e49)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (0)
        #define MJENC_TEST_HW_AUTO_MODE_EN (1)
        #define TSRC_OUT_RING_LINE     (8*2)                        /*min 8 line x2 */
        #define TSRC_OUT_FORMAT        (ISP_TSRC_OUT_FMT0_YUV422SP)    /*ISP_TSRC_OUT_FMT0_YUYV, ISP_TSRC_OUT_FMT0_YUV422SP, ISP_TSRC_OUT_FMT0_NV12, ISP_TSRC_OUT_FMT0_Y_ONLY*/
    #endif
    #if defined( MJENC_TEST_CASE_ID_44)
        #define MJENC_TEST_CASE_ID (44)
        #define MJENC_TEST_CASE_INFO ("Tsrc2Mjpeg_1920x1080_422_yuyv.raw")
        #define MJENC_TEST_JPG_ENC_YUV_MODE (MJPEG_HAL_FORMAT_YUV422_YUYV)
        #define MJENC_TEST_JPG_ENC_Q_VALUIE (50)
        #define MJENC_TEST_IMG_W (1920)
        #define MJENC_TEST_IMG_H (1080)
        #define MJENC_TEST_SUM_TARGET (0x9654f44d)
        #define MJENC_TEST_XOR_TARGET (0xa2072e49)
        #define MJENC_TEST_OUT_SWAP_MODE_EN (1)
        #define MJENC_TEST_HW_AUTO_MODE_EN (1)
        #define TSRC_OUT_RING_LINE     (8*2)                        /*min 8 line x2 */
        #define TSRC_OUT_FORMAT        (ISP_TSRC_OUT_FMT0_YUYV)
    #endif

#endif  /*#if defined(_ENCODE_TEST_SAMPLE_VERIFICATION_)*/

#if (MJENC_TEST_HW_AUTO_MODE_EN == 0)
    #if !defined(MJENC_TEST_RUN_SW_MODE_SEL)
        #define MJENC_TEST_RUN_SW_MODE_SEL  (0) /*0: run SW frame mode, 1:run SW kick mode.*/
    #endif
#endif


#if defined(CONFIG_FPGA)
    /*for most test cases, image load from FLASH range*/
    #define FLASH_END_ADDR                              (0x12000000+0x00200000)
    #define FLASH_RSVD_FOR_JENC_1350K_START_ADDR        (FLASH_END_ADDR-1350*1024)

    #define MJENC_TEST_YUV_BUF_SIZE                     ( 1350*1024 )
    #define MJENC_TEST_YUV_BUF_ADDR                     ( FLASH_RSVD_FOR_JENC_1350K_START_ADDR)

    #define MJENC_TEST_SWAP_MODE_JPG_OUT_BUF_SIZE       ( 8*1024 )      /*swap output buffer*/
    #define MJENC_TEST_SWAP_MODE_STREAM_OUT_BUF_SIZE    ( 320*1024 )    /*max for case23 output bst size*/
    #define MJENC_TEST_NORM_MODE_JPG_OUT_BUF_SIZE       ( 641*1024 )
#else
    /*for test cases input and output using ocram*/
    #define REALCHIP_MJPEG_TEST_RSVD_MEM_START_ADDR     (0x11050000 - 320*240*2)
    #define FLASH_END_ADDR                              (REALCHIP_MJPEG_TEST_RSVD_MEM_START_ADDR + 320*240*2)

    #define MJENC_TEST_YUV_BUF_SIZE                     ( 320*240*2 )
    #define MJENC_TEST_YUV_BUF_ADDR                     ( FLASH_END_ADDR - MJENC_TEST_YUV_BUF_SIZE)

    #define MJENC_TEST_SWAP_MODE_JPG_OUT_BUF_SIZE       ( 2*1024 )      /*swap output buffer*/
    #define MJENC_TEST_SWAP_MODE_STREAM_OUT_BUF_SIZE    ( 79*1024 )    /*max for case13 output bst size */
    #define MJENC_TEST_NORM_MODE_JPG_OUT_BUF_SIZE       ( 79*1024 )
#endif



#endif  /*#ifndef __DEMO_MJPEG_CFG_H_*/
