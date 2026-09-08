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

#ifndef __DRV_TSRC_H__
#define __DRV_TSRC_H__

#include "dvp_tsrc_lite_reg.h"
#include "drv_common.h"

/** @addtogroup  Peripheral_Driver
 *  @{
 */

/** @addtogroup  TSRC
 *  @{
 */

/** @defgroup  TSRC_Public_Types
 *  @{
 */

/**
 *  @brief TSRC pixel format type definition
 */
typedef enum {
    TSRC_PIXEL_YUV422_16BIT,             /*!< YUV422 16-bit mode */
    TSRC_PIXEL_RGB888_24BIT,             /*!< RGB888 24-bit mode */
    TSRC_PIXEL_RGB565_16TO24BIT,         /*!< RGB565 16-bit extend 24-bit mode */
    TSRC_PIXEL_RGBA8888_32TO24BIT,       /*!< RGBA8888 32-bit compress 24-bit mode */
    TSRC_PIXEL_YUV400_8BIT,              /*!< YUV400 8-bit mode */
    TSRC_PIXEL_12BIT,                    /*!< 12-bit mode */
}TSRC_Pixel_Format_Type;

/**
 *  @brief TSRC AXI burst type definition
 */
typedef enum {
    TSRC_BURST_TYPE_SINGLE,              /*!< TSRC AXI burst type:single */
    TSRC_BURST_TYPE_INCR4,               /*!< TSRC AXI burst type:incrementing 4 */
    TSRC_BURST_TYPE_INCR8,               /*!< TSRC AXI burst type:incrementing 8 */
    TSRC_BURST_TYPE_INCR16,              /*!< TSRC AXI burst type:incrementing 16 */
    TSRC_BURST_TYPE_INCR32 = 5,          /*!< TSRC AXI burst type:incrementing 32 */
    TSRC_BURST_TYPE_INCR64,              /*!< TSRC AXI burst type:incrementing 64 */
}TSRC_Burst_Type;

typedef struct {
    uint16_t width;                      /*!< image width, unit: pix */
    uint16_t height;                     /*!< image height, unit: line */
    uint16_t hBlank;                     /*!< horizontal blanking */
    uint16_t vBlank;                     /*!< vertical blanking */
} TSRC_Frame_Size_Cfg_Type;

/**
 *  @brief TSRC configuration structure type definition
 */
typedef struct {
    TSRC_Pixel_Format_Type format;           /*!< Set pixel data format */
    TSRC_Burst_Type burst;                   /*!< AXI burst length */
    uint8_t pushMode;                        /*!< Enable or disable push mode */
    uint8_t byte0;                           /*!< Byte 0 selection */
    uint8_t byte1;                           /*!< Byte 1 Selection */
    uint8_t byte2;                           /*!< Byte 2 Selection */
    TSRC_Frame_Size_Cfg_Type imgSize;        /*!< Image width, height, and blanking */
    uint32_t memStart;                       /*!< Start address */
    uint16_t prefetch;                       /*!< Vertical prefetch start position, relative to blank start position. Should be less than vBlank */
}TSRC_Cfg_Type;

/**
 *  @brief TSRC fake gradient data configuration structure type definition
 */
typedef struct {
    uint16_t minData;                        /*!< Minimum data value of fake gradient data */
    uint16_t maxData;                        /*!< Maximum data value of fake gradient data */
    uint8_t step;                            /*!< Data step value for eack line of fake gradient data */
}TSRC_Fake_Data_Cfg_Type;

/**
 *  @brief TSRC CEA-861 configuration structure type definition
 */
typedef struct {
    uint8_t hsyncStart;                      /*!< Hsync start position, relative to blank start position */
    uint8_t hsyncEnd;                        /*!< Hsync end position, relative to blank start position */
    uint8_t vsyncStart;                      /*!< Vsync start position, relative to blank start position */
    uint8_t vsyncEnd;                        /*!< Vsync end position, relative to blank start position */
}TSRC_CEA861_Cfg_Type;

typedef struct {
    uint16_t totalHPixels;
    uint16_t actStartPixel;
    uint16_t actPixelWidth;
    uint16_t actStartLine;
    uint16_t actLineHeight;
}TSRC_TG_Cfg;

/*@} end of group TSRC_Public_Types */

/** @defgroup  TSRC_Public_Constants
 *  @{
 */

/** @defgroup  TSRC_PIXEL_FORMAT_TYPE
 *  @{
 */
#define IS_TSRC_PIXEL_FORMAT_TYPE(type)                  (((type) == TSRC_PIXEL_YUV422_16BIT) || \
                                                          ((type) == TSRC_PIXEL_RGB888_24BIT) || \
                                                          ((type) == TSRC_PIXEL_RGB565_16TO24BIT) || \
                                                          ((type) == TSRC_PIXEL_RGBA8888_32TO24BIT) || \
                                                          ((type) == TSRC_PIXEL_YUV400_8BIT) || \
                                                          ((type) == TSRC_PIXEL_12BIT))

/** @defgroup  TSRC_BURST_TYPE
 *  @{
 */
#define IS_TSRC_BURST_TYPE(type)                         (((type) == TSRC_BURST_TYPE_SINGLE) || \
                                                          ((type) == TSRC_BURST_TYPE_INCR4) || \
                                                          ((type) == TSRC_BURST_TYPE_INCR8) || \
                                                          ((type) == TSRC_BURST_TYPE_INCR16) || \
                                                          ((type) == TSRC_BURST_TYPE_INCR32) || \
                                                          ((type) == TSRC_BURST_TYPE_INCR64))

/*@} end of group TSRC_Public_Constants */

/** @defgroup  TSRC_Public_Macros
 *  @{
 */

/*@} end of group TSRC_Public_Macros */

/** @defgroup  TSRC_Public_Functions
 *  @{
 */
uint32_t TSRC_Init(TSRC_Cfg_Type *cfg);
void TSRC_Fake_Data_Init(TSRC_Fake_Data_Cfg_Type *cfg);
// void TSRC_CEA861_Init(TSRC_CEA861_Cfg_Type *cfg);
void TSRC_Enable(void);
void TSRC_Disable(void);
void TSRC_Clear_Drain_Error(void);

// void TSRC_Set_Frame_Size(TSRC_Frame_Size_Cfg_Type *cfg);
// void TSRC_Mem_Cfg(uint32_t memAddr);
void TSRC_Push_Mode_Trigger(void);
void TSRC_Set_Stride_Width(uint32_t bytesPerLine);
void TSRC_Set_TG_State(uint8_t state);
void TSRC_Set_TG_Cfg(TSRC_TG_Cfg *cfg);
// void TSRC_Set_Pix_Format(TSRC_Pixel_Format_Type type);

/*@} end of group TSRC_Public_Functions */

/*@} end of group TSRC */

/*@} end of group Peripheral_Driver */

#endif /* __DRV_TSRC_H__ */
