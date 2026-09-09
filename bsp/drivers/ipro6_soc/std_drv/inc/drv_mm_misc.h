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


#ifndef __DRV_ISP_MISC_H__
#define __DRV_ISP_MISC_H__

#include "mm_misc_reg.h"
#include "drv_common.h"

/**
 *  @brief SPI2DVP data order
 */
typedef enum {
    MM_MISC_SPI2DVP_ORDER_UV_FIRST,
    MM_MISC_SPI2DVP_ORDER_Y_FIRST,
} MM_MISC_SPI2DVP_DATA_ORDER;

/**
 *  @brief SPI2DVP format
 */
typedef enum {
    MM_MISC_SPI2DVP_FORMAT_YUV422,
    MM_MISC_SPI2DVP_FORMAT_Y_ONLY,
} MM_MISC_SPI2DVP_DATA_FORMAT;

/**
 *  @brief SPI2DVP mode (aka SPI mode)
 */
typedef enum {
    MM_MISC_SPI2DVP_MODE_0,     /* SPI mode 0, phase = 0, polarity = 0  */
    MM_MISC_SPI2DVP_MODE_1,     /* SPI mode 1, phase = 1, polarity = 0  */
    MM_MISC_SPI2DVP_MODE_2,     /* SPI mode 2, phase = 0, polarity = 1  */
    MM_MISC_SPI2DVP_MODE_3,     /* SPI mode 3, phase = 1, polarity = 1  */
} MM_MISC_SPI2DVP_MODE;

/**
 *  @brief SPI2DVP endian
 */
typedef enum {
    MM_MISC_SPI2DVP_ENDIAN_LSB,
    MM_MISC_SPI2DVP_ENDIAN_MSB,
} MM_MISC_SPI2DVP_DATA_ENDIAN;

/**
 *  @brief SPI2DVP configuration struct definition
 */
typedef struct {
    MM_MISC_SPI2DVP_DATA_ORDER  dataOrder;
    MM_MISC_SPI2DVP_DATA_FORMAT dataFormat;
    MM_MISC_SPI2DVP_MODE        spiMode;
    MM_MISC_SPI2DVP_DATA_ENDIAN dataEndian;
}MM_MISC_SPI2DVP_Cfg;

/**
 *  @brief SPI2DVP interrupt status
 */
typedef struct {
    uint8_t spi2dvp_fifo_int;
    uint8_t spi2dvp_ls_int;
    uint8_t spi2dvp_fe_int;
    uint8_t spi2dvp_fs_int;
    uint8_t spi2dvp_sync_int;
}MM_MISC_SPI2DVP_INT_Status;

/**
 *  @brief SPI2DVP status
 */
typedef struct {
    uint16_t spi2dvp_data_size;
    uint8_t  spi2dvp_data_id;
    uint8_t  spi2dvp_pkt_id;
    uint16_t spi2dvp_img_h;
    uint16_t spi2dvp_img_w;
}MM_MISC_SPI2DVP_Status;

/**
 *  @brief Input V/H sync active level type definition
 */
typedef enum {
    MM_MISC_ACTIVE_HIGH, /*!< Input is active-high */
    MM_MISC_ACTIVE_LOW,  /*!< Input is active-low */
}MM_MISC_Active_Level_Type;

/**
 *  @brief Input data order type definition
 */
typedef enum {
    MM_MISC_DATA_BYTE_LOWER,  /*!< The first data is lower-byte */
    MM_MISC_DATA_BYTE_HIGHER, /*!< The first data is higher-byte */
}MM_MISC_Data_Order_Type;

/**
 *  @brief S2P configuration struct definition
 */
typedef struct {
    MM_MISC_Active_Level_Type hSyncLevel;        /*!< Input H-Sync active level */
    MM_MISC_Active_Level_Type vSyncLevel;        /*!< Input V-Sync active level */
    MM_MISC_Data_Order_Type dataOrder;           /*!< Input data order */
}MM_MISC_S2P_Cfg_Type;

/**
 *  @brief DVP AS configuration struct definition
 */
typedef struct {
    uint16_t fifoThreshold;
}MM_MISC_DVP_AS_Cfg;

/**
 *  @brief DVP AS TG configuration struct definition
 */
typedef struct {
    uint16_t totalHPixels;
    uint16_t actStartPixel;
    uint16_t actPixelWidth;
    uint16_t actStartLine;
    uint16_t actLineHeight;
}MM_MISC_DVP_AS_TG_Cfg;

/**
 *  @brief soft reset ID (can OR to set in API MM_MISC_Set_SW_Reset)
 */
typedef enum {
    MM_MISC_SW_RESET_ID_MM_MISC                = (1 << 0),
    MM_MISC_SW_RESET_ID_DVP2AXI_A              = (1 << 1),
    MM_MISC_SW_RESET_ID_DVP2AXI_B              = (1 << 2),
    MM_MISC_SW_RESET_ID_JPG                    = (1 << 8),
    MM_MISC_SW_RESET_ID_DVP_AS                 = (1 << 13),
}MM_MISC_SW_RESET_ID;


void MM_MISC_Set_SNSR_Reset(uint8_t value);
void MM_MISC_Set_SNSR_PowerDown(uint8_t value);
void MM_MISC_SPI2DVP_Init(MM_MISC_SPI2DVP_Cfg cfg);
void MM_MISC_Set_SPI2DVP_State(uint8_t state);
void MM_MISC_Get_SPI2DVP_State(uint8_t *state);
void MM_MISC_SPI2DVP_INT_Enable(void);
void MM_MISC_SPI2DVP_INT_Disable(void);
void MM_MISC_Get_SPI2DVP_INT_Status(MM_MISC_SPI2DVP_INT_Status *status);
void MM_MISC_Get_SPI2DVP_Status(MM_MISC_SPI2DVP_Status *status);
void MM_MISC_SPI2DVP_WaitReady(void);
void MM_MISC_S2P_Init(MM_MISC_S2P_Cfg_Type* s2pCfg);
void MM_MISC_Get_S2P_Cfg(MM_MISC_S2P_Cfg_Type* s2pCfg);
void MM_MISC_Set_S2P_State(uint8_t state);
void MM_MISC_Get_S2P_State(uint8_t *state);
void MM_MISC_Set_DVP_AS_TG_State(uint8_t state);
void MM_MISC_Set_DVP_AS_TG_Cfg(MM_MISC_DVP_AS_TG_Cfg* dvpAsTgCfg);
void MM_MISC_Set_DVP_AS_State(uint8_t state);
void MM_MISC_Get_DVP_AS_State(uint8_t *state);
void MM_MISC_DVP_AS_Set_Cfg(MM_MISC_DVP_AS_Cfg* dvpAsCfg);
void MM_MISC_Set_SW_Reset(uint32_t id, uint8_t aRst);

#endif //__DRV_ISP_MISC_H__
