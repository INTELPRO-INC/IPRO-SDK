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


#ifndef __DEMO_MJDEC_CFG_H_
#define __DEMO_MJDEC_CFG_H_

#if !defined(CONFIG_FPGA)   /*for real chip verification.*/
#define REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR     (0x11000000 + (320-79)*1024)   /*reserve 79K for jpeg bit stream source*/
#endif

#define _DECODE_TEST_SAMPLE_VERIFICATION_
#if defined(_DECODE_TEST_SAMPLE_VERIFICATION_) /*Decode Test sample*/

    //#define DECODE_JPEG_SAMPE_320x240_NON_STD_HUFF          /*Case  1*/  /*ipro6 OK*/
    //#define DECODE_JPEG_SAMPE_1920x1080_STD_HUFF            /*Case  2*/
    
    //#define DECODE_JPEG_SAMPE_Sep_1280x720_YUV400_Q50       /*Case  3*/  /*ipro6 OK*/
    //#define DECODE_JPEG_SAMPE_Sep_1280x720_YUV400_QLL       /*Case  4*/  /*ipro6 OK*/
    
    //#define DECODE_JPEG_SAMPE_FHD_3_YUV400_Q50              /*Case  5*/
    //#define DECODE_JPEG_SAMPE_FHD_3_YUV400_QLL              /*Case  6*/
    
    //#define DECODE_JPEG_SAMPE_FHD_3_YUV420_Q50              /*Case  7*/
    //#define DECODE_JPEG_SAMPE_FHD_3_YUV420_QLL              /*Case  8*/
    
    //#define DECODE_JPEG_SAMPE_FHD_3_YUV422_Q50              /*Case  9*/
    //#define DECODE_JPEG_SAMPE_FHD_3_YUV422_QLL              /*Case 10*/
    
    //#define DECODE_JPEG_SAMPE_HD_1_YUV422_Q50               /*Case 11*/
    //#define DECODE_JPEG_SAMPE_HD_1_YUV422_QLL               /*Case 12*/  /*ipro7 20250202 bit file hangup, 20250322 bit file pass*/
    
    //#define DECODE_JPEG_SAMPE_CAFE_YUV422_Q50               /*Case 13*/  /*ipro6 OK*/
    //#define DECODE_JPEG_SAMPE_CAFE_YUV422_QLL               /*Case 14*/  /*ipro6 OK*/
    
    #define DECODE_JPEG_SAMPE_320X240_YUV420_QTABLE_Q100    /*Case 15*/  /*ipro6 OK*/
    //#define DECODE_JPEG_SAMPE_320X240_YUV420_QTABLE_Q200    /*Case 16*/  /*ipro6 OK*/
    //#define DECODE_JPEG_SAMPE_320X240_YUV420_QTABLE_Q255    /*Case 17*/  /*ipro6 OK*/
    //#define DECODE_JPEG_SAMPE_320X240_YUV422_QTABLE_Q100    /*Case 18*/  /*ipro6 OK*/
    //#define DECODE_JPEG_SAMPE_320X240_YUV422_QTABLE_Q200    /*Case 19*/  /*ipro6 OK*/
    //#define DECODE_JPEG_SAMPE_320X240_YUV422_QTABLE_Q255    /*Case 20*/  /*ipro6 OK*/
    
    //#define DECODE_JPEG_SAMPE_320X240_YUV420_QTABLE_Q100_VU /*Case 21*/  /*ipro6 OK*/
    //#define DECODE_JPEG_SAMPE_320X240_YUV422_QTABLE_Q100_VU /*Case 22*/  /*ipro6 OK*/
    
    //#define DECODE_JPEG_SAMPE_640x480_DEBUGTOOL_420_TEST    /*Case 23*/  /*ipro6 OK*/
    //#define DECODE_JPEG_SAMPE_96x64_DEBUGTOOL_422_TEST        /*Case 24*/  /*ipro6 OK*/

    #if defined(DECODE_JPEG_SAMPE_320x240_NON_STD_HUFF)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "pic_data.h"               /*sample JPEG stream, user Huffman*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (mjdec_jpeg_data)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "mjdec_pic_data"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV420_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (320)
        #define MJDEC_TEST_IMG_H           (240)
        #define MJDEC_TEST_CASE_ID         (1)
        #define MJDEC_TEST_SUM_TARGET      (0xad398f5f)
        #define MJDEC_TEST_XOR_TARGET      (0x8c2e7811)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_1920x1080_STD_HUFF)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "pic_data_default_huff.h"  /*sample JPEG stream, default Huffman*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (jpeg_data_default_huff)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "pic_data_default_huff"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV420_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (1920)
        #define MJDEC_TEST_IMG_H           (1080)
        #define MJDEC_TEST_CASE_ID         (2)
        #define MJDEC_TEST_SUM_TARGET      (0x251dff70)
        #define MJDEC_TEST_XOR_TARGET      (0x97f585d4)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif

    #if defined(DECODE_JPEG_SAMPE_Sep_1280x720_YUV400_Q50)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "Sep_1280x720_400_q50.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (Sep_1280x720_400_q50)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "Sep_1280x720_400_q50"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV400 //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (1280)
        #define MJDEC_TEST_IMG_H           (720)
        #define MJDEC_TEST_CASE_ID         (3)
        #define MJDEC_TEST_SUM_TARGET      (0xa31beabc)
        #define MJDEC_TEST_XOR_TARGET      (0x45bac4f8)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_Sep_1280x720_YUV400_QLL)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "Sep_1280x720_400_qll.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (Sep_1280x720_400_qll)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "Sep_1280x720_400_qll"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV400 //0:420, 1:400, 2:422
 
        #define MJDEC_TEST_IMG_W           (1280)
        #define MJDEC_TEST_IMG_H           (720)
        #define MJDEC_TEST_CASE_ID         (4)
        #define MJDEC_TEST_SUM_TARGET      (0xefd12a96)
        #define MJDEC_TEST_XOR_TARGET      (0x7ee040a8)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif




    #if defined(DECODE_JPEG_SAMPE_FHD_3_YUV400_Q50)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "FHD_3_yuv400_q50.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (FHD_3_yuv400_q50)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "FHD_3_yuv400_q50"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV400 //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (1920)
        #define MJDEC_TEST_IMG_H           (1080)
        #define MJDEC_TEST_CASE_ID         (5)
        #define MJDEC_TEST_SUM_TARGET      (0x7f2e8bb9)
        #define MJDEC_TEST_XOR_TARGET      (0x10668575)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_FHD_3_YUV400_QLL)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "FHD_3_yuv400_qll.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (FHD_3_yuv400_qll)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "FHD_3_yuv400_qll"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV400 //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (1920)
        #define MJDEC_TEST_IMG_H           (1080)
        #define MJDEC_TEST_CASE_ID         (6)
        #define MJDEC_TEST_SUM_TARGET      (0x4670100b)
        #define MJDEC_TEST_XOR_TARGET      (0x323642b3)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif

    #if defined(DECODE_JPEG_SAMPE_FHD_3_YUV420_Q50)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "FHD_3_yuv420_q50.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (FHD_3_yuv420_q50)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "FHD_3_yuv420_q50"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV420_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (1920)
        #define MJDEC_TEST_IMG_H           (1080)
        #define MJDEC_TEST_CASE_ID         (7)
        #define MJDEC_TEST_SUM_TARGET      (0xc2b75f58)
        #define MJDEC_TEST_XOR_TARGET      (0x7d735200)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_FHD_3_YUV420_QLL)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "FHD_3_yuv420_qll.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (FHD_3_yuv420_qll)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "FHD_3_yuv420_qll"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV420_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (1920)
        #define MJDEC_TEST_IMG_H           (1080)
        #define MJDEC_TEST_CASE_ID         (8)
        #define MJDEC_TEST_SUM_TARGET      (0xccc4851b)
        #define MJDEC_TEST_XOR_TARGET      (0x5666fbab)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif

    #if defined(DECODE_JPEG_SAMPE_FHD_3_YUV422_Q50)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "FHD_3_yuv422_q50.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (FHD_3_yuv422_q50)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "FHD_3_yuv422_q50"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV422_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (1920)
        #define MJDEC_TEST_IMG_H           (1080)
        #define MJDEC_TEST_CASE_ID         (9)
        #define MJDEC_TEST_SUM_TARGET      (0x4e03f43)
        #define MJDEC_TEST_XOR_TARGET      (0xae19ad71)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_FHD_3_YUV422_QLL)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "FHD_3_yuv422_qll.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (FHD_3_yuv422_qll)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "FHD_3_yuv422_qll"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV422_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (1920)
        #define MJDEC_TEST_IMG_H           (1080)
        #define MJDEC_TEST_CASE_ID         (10)
        #define MJDEC_TEST_SUM_TARGET      (0xbe346bf0)
        #define MJDEC_TEST_XOR_TARGET      (0x2e63e502)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif

    #if defined(DECODE_JPEG_SAMPE_HD_1_YUV422_Q50)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "HD_1_yuv422_q50.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (HD_1_yuv422_q50)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "HD_1_yuv422_q50"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV422_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (1280)
        #define MJDEC_TEST_IMG_H           (720)
        #define MJDEC_TEST_CASE_ID         (11)
        #define MJDEC_TEST_SUM_TARGET      (0x76efa84d)
        #define MJDEC_TEST_XOR_TARGET      (0x5443187d)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_HD_1_YUV422_QLL)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "HD_1_yuv422_qll.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (HD_1_yuv422_qll)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "HD_1_yuv422_qll"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV422_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (1280)
        #define MJDEC_TEST_IMG_H           (720)
        #define MJDEC_TEST_CASE_ID         (12)
        #define MJDEC_TEST_SUM_TARGET      (0x47190a56)
        #define MJDEC_TEST_XOR_TARGET      (0xeef54e56)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif

    #if defined(DECODE_JPEG_SAMPE_CAFE_YUV422_Q50)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "cafe_yuv422_q50.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (cafe_yuv422_q50)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "cafe_yuv422_q50"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV422_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (240)
        #define MJDEC_TEST_IMG_H           (320)
        #define MJDEC_TEST_CASE_ID         (13)
        #define MJDEC_TEST_SUM_TARGET      (0x637ae2db)
        #define MJDEC_TEST_XOR_TARGET      (0x52b4f1f)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_CAFE_YUV422_QLL)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "cafe_yuv422_qll.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (cafe_yuv422_qll)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "cafe_yuv422_qll"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV422_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (240)
        #define MJDEC_TEST_IMG_H           (320)
        #define MJDEC_TEST_CASE_ID         (14)
        #define MJDEC_TEST_SUM_TARGET      (0x67d9ae0a)
        #define MJDEC_TEST_XOR_TARGET      (0xf2b567a2)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif




    /*Test Q Values*/
    #if defined(DECODE_JPEG_SAMPE_320X240_YUV420_QTABLE_Q100)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "320x240_yuv420_QTABLE_q100.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (g_auc320x240_yuv420_QTABLE_q100)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "320x240_yuv420_QTABLE_q100"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV420_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (320)
        #define MJDEC_TEST_IMG_H           (240)
        #define MJDEC_TEST_CASE_ID         (15)
        #define MJDEC_TEST_SUM_TARGET      (0x0a5d9117)
        #define MJDEC_TEST_XOR_TARGET      (0xfa0e89ed)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_320X240_YUV420_QTABLE_Q200)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "320x240_yuv420_QTABLE_q200.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (g_auc320x240_yuv420_QTABLE_q200)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "320x240_yuv420_QTABLE_q200"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV420_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (320)
        #define MJDEC_TEST_IMG_H           (240)
        #define MJDEC_TEST_CASE_ID         (16)
        #define MJDEC_TEST_SUM_TARGET      (0x9cbef5ba)
        #define MJDEC_TEST_XOR_TARGET      (0x6eab684e)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_320X240_YUV420_QTABLE_Q255)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "320x240_yuv420_QTABLE_q255.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (g_auc320x240_yuv420_QTABLE_q255)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "320x240_yuv420_QTABLE_q255"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV420_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (320)
        #define MJDEC_TEST_IMG_H           (240)
        #define MJDEC_TEST_CASE_ID         (17)
        #define MJDEC_TEST_SUM_TARGET      (0x8af02ce3)
        #define MJDEC_TEST_XOR_TARGET      (0x094f1de1)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_320X240_YUV422_QTABLE_Q100)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "320x240_yuv422_QTABLE_q100.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (g_auc320x240_yuv422_QTABLE_q100)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "320x240_yuv422_QTABLE_q100"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV422_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (320)
        #define MJDEC_TEST_IMG_H           (240)
        #define MJDEC_TEST_CASE_ID         (18)
        #define MJDEC_TEST_SUM_TARGET      (0x1685c21d)
        #define MJDEC_TEST_XOR_TARGET      (0x05f16215)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_320X240_YUV422_QTABLE_Q200)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "320x240_yuv422_QTABLE_q200.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (g_auc320x240_yuv422_QTABLE_q200)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "320x240_yuv422_QTABLE_q200"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV422_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (320)
        #define MJDEC_TEST_IMG_H           (240)
        #define MJDEC_TEST_CASE_ID         (19)
        #define MJDEC_TEST_SUM_TARGET      (0x4c6c0b42)
        #define MJDEC_TEST_XOR_TARGET      (0x6ea76842)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif
    #if defined(DECODE_JPEG_SAMPE_320X240_YUV422_QTABLE_Q255)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "320x240_yuv422_QTABLE_q255.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (g_auc320x240_yuv422_QTABLE_q255)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "320x240_yuv422_QTABLE_q255"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV422_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (320)
        #define MJDEC_TEST_IMG_H           (240)
        #define MJDEC_TEST_CASE_ID         (20)
        #define MJDEC_TEST_SUM_TARGET      (0x1b4e7303)
        #define MJDEC_TEST_XOR_TARGET      (0x094f1de1)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif

    /*Test MJDEC_TEST_U_ORDER_EVEN*/
    #if defined(DECODE_JPEG_SAMPE_320X240_YUV420_QTABLE_Q100_VU)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "320x240_yuv420_QTABLE_q100.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (g_auc320x240_yuv420_QTABLE_q100)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "320x240_yuv420_QTABLE_q100_vu"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV420_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (320)
        #define MJDEC_TEST_IMG_H           (240)
        #define MJDEC_TEST_CASE_ID         (21)
        #define MJDEC_TEST_SUM_TARGET      (0xaebbe1dc)
        #define MJDEC_TEST_XOR_TARGET      (0x01f586e2)
        #define MJDEC_TEST_U_ORDER_EVEN    (0)
    #endif
    #if defined(DECODE_JPEG_SAMPE_320X240_YUV422_QTABLE_Q100_VU)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "320x240_yuv422_QTABLE_q100.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (g_auc320x240_yuv422_QTABLE_q100)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "320x240_yuv422_QTABLE_q100_vu"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV422_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (320)
        #define MJDEC_TEST_IMG_H           (240)
        #define MJDEC_TEST_CASE_ID         (22)
        #define MJDEC_TEST_SUM_TARGET      (0xdac6f80d)
        #define MJDEC_TEST_XOR_TARGET      (0xfe0a7e09)
        #define MJDEC_TEST_U_ORDER_EVEN    (0)
    #endif

    #if defined(DECODE_JPEG_SAMPE_640x480_DEBUGTOOL_420_TEST)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "640x480_DebugToolTest_420.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (g_auc640x480_DebugToolTest_420)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "640x480_DebugToolTest_420"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV420_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (640)
        #define MJDEC_TEST_IMG_H           (480)
        #define MJDEC_TEST_CASE_ID         (23)
        #define MJDEC_TEST_SUM_TARGET      (0xffd7e48b)
        #define MJDEC_TEST_XOR_TARGET      (0xfd39b7d3)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif

    #if defined(DECODE_JPEG_SAMPE_96x64_DEBUGTOOL_422_TEST)
        #if defined(CONFIG_FPGA)   /*for real chip use reserved mempry area.*/
            #include "96x64_DebugToolTest_422.h"               /*sample JPEG stream*/
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         (g_auc96x64_DebugToolTest_422)
        #else
            #define MJDEC_TEST_JPG_BIT_STREAM_BUF_ADDR         REALCHIP_MJDEC_TEST_RSVD_MEM_START_ADDR
        #endif
        #define MJDEC_TEST_CASE_INFO                       "96x64_DebugToolTest_422"
        #define MJDEC_TEST_YUV_MODE                        MJDEC_HAL_FORMAT_YUV422_SP //0:420, 1:400, 2:422

        #define MJDEC_TEST_IMG_W           (96)
        #define MJDEC_TEST_IMG_H           (64)
        #define MJDEC_TEST_CASE_ID         (24)
        #define MJDEC_TEST_SUM_TARGET      (0x88eac8df)
        #define MJDEC_TEST_XOR_TARGET      (0x11f783af)
        #define MJDEC_TEST_U_ORDER_EVEN    (1)
    #endif

#if !defined(MJDEC_TEST_KICK_MODE_EN)
    #define MJDEC_TEST_KICK_MODE_EN  (1)
    #define MJDEC_TEST_KICK_MODE_SEL (1)                    /*0: burst block mode. 1: burst line mode*/
    #if (MJDEC_TEST_KICK_MODE_SEL==1)
        #define MJDEC_TEST_BLOCK_LINE_MODE_OUT_DIRECT (1)       /*0: use memcpy to dst buffer. 1: HW direct output to dst buffer*/
    #endif
#endif
#endif /*#if defined(_DECODE_TEST_SAMPLE_VERIFICATION_)*/

#endif /*#ifndef __DEMO_MJDEC_CFG_H_*/ 
