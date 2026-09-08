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
 *  @brief DVP mux shadow update ID (can OR to set in API MM_MISC_Set_Mux_Shadow_Update)
 */
typedef enum {
    MM_MISC_DVP_MUX_SH_ISP                     = (1 << 0),
    MM_MISC_DVP_MUX_SH_SCALER                  = (1 << 1),
    MM_MISC_DVP_MUX_SH_OSD_BLEND               = (1 << 2),
    MM_MISC_DVP_MUX_SH_OSD_DRAW                = (1 << 3),
    MM_MISC_DVP_MUX_SH_422TO444                = (1 << 4),
    MM_MISC_DVP_MUX_SH_422TO420                = (1 << 5),
    MM_MISC_DVP_MUX_SH_TRA                     = (1 << 6),
    MM_MISC_DVP_MUX_SH_444TO422                = (1 << 7),
    MM_MISC_DVP_MUX_SH_D2XA                    = (1 << 8),
    MM_MISC_DVP_MUX_SH_D2XB                    = (1 << 9),
    MM_MISC_DVP_MUX_SH_D2XC                    = (1 << 10),
}MM_MISC_DVP_MUX_SH_ID;

/**
 *  @brief SPI2DVP select
 */
typedef enum {
    MM_MISC_SPI2DVP_SEL_SPI,
    MM_MISC_SPI2DVP_SEL_SPI_2,
}MM_MISC_SPI2DVP_SEL;

/**
 *  @brief DVP AS Shared SRAM mode (assume only single input)
 */
typedef enum {
    MM_MISC_SHARED_SRAM_MODE_DVP_AS,
    MM_MISC_SHARED_SRAM_MODE_SPI2DVP,
    MM_MISC_SHARED_SRAM_MODE_DVP_TSRC,
}MM_MISC_SHARED_SRAM_MODE;

/**
 *  @brief DVP2AXI port. Support three dvp2axi at most
 */
typedef enum {
    MM_MISC_DVP2AXI_PORT_A,
    MM_MISC_DVP2AXI_PORT_B,
    MM_MISC_DVP2AXI_PORT_C,
}MM_MISC_DVP2AXI_PORT;

/**
 *  @brief DVP2AXI Source
 */
typedef enum {
    MM_MISC_DVP2AXI_SRC_SPI2DVP                = 0,
    MM_MISC_DVP2AXI_SRC_DVP_AS                 = 1,
    MM_MISC_DVP2AXI_SRC_ISP_OUTPUT             = 2,
    MM_MISC_DVP2AXI_SRC_SCALER_F0              = 3,
    MM_MISC_DVP2AXI_SRC_SCALER_F1              = 4,
    MM_MISC_DVP2AXI_SRC_OSD_BLEND              = 5,
    MM_MISC_DVP2AXI_SRC_OSD_DRAW               = 6,
    MM_MISC_DVP2AXI_SRC_YUV444TO422            = 7,
    MM_MISC_DVP2AXI_SRC_IPROAI                   = 8,
    MM_MISC_DVP2AXI_SRC_DVP_TSRC               = 9,
    MM_MISC_DVP2AXI_SRC_YUV422TO444            = 10,
    MM_MISC_DVP2AXI_SRC_YUV2RGB                = 11,
    MM_MISC_DVP2AXI_SRC_YUV422TO420            = 12,
    MM_MISC_DVP2AXI_SRC_ISP_DEBUG_LINK_0       = 13,
    MM_MISC_DVP2AXI_SRC_ISP_DEBUG_LINK_1       = 14,
}MM_MISC_DVP2AXI_SRC;

/**
 *  @brief YUV422 input format mux ID
 */
typedef enum {
    MM_MISC_YUV422_IN_MUX_ID_ISP               = 0,
    MM_MISC_YUV422_IN_MUX_ID_SCALER            = 1,
    MM_MISC_YUV422_IN_MUX_ID_OSD_BLEND         = 2,
    MM_MISC_YUV422_IN_MUX_ID_OSD_DRAW          = 3,
    MM_MISC_YUV422_IN_MUX_ID_YUV422TO444       = 4,
    MM_MISC_YUV422_IN_MUX_ID_YUV422TO420       = 5,
}MM_MISC_YUV422_IN_MUX_ID;

/**
 *  @brief YUV422 input format mux select (select which to be YUV422 MUX ID input)
 */
typedef enum {
    MM_MISC_YUV422_IN_MUX_SEL_SPI2DVP          = 0,
    MM_MISC_YUV422_IN_MUX_SEL_DVP_AS           = 1,
    MM_MISC_YUV422_IN_MUX_SEL_ISP_OUTPUT       = 2,
    MM_MISC_YUV422_IN_MUX_SEL_SCALER_F0        = 3,
    MM_MISC_YUV422_IN_MUX_SEL_SCALER_F1        = 4,
    MM_MISC_YUV422_IN_MUX_SEL_OSD_BLEND        = 5,
    MM_MISC_YUV422_IN_MUX_SEL_OSD_DRAW         = 6,
    MM_MISC_YUV422_IN_MUX_SEL_YUV444TO422      = 7,
    MM_MISC_YUV422_IN_MUX_SEL_IPROAI             = 8,
    MM_MISC_YUV422_IN_MUX_SEL_DVP_TSRC         = 9,
}MM_MISC_YUV422_IN_MUX_SEL;

/**
 *  @brief YUV444 input format mux ID
 */
typedef enum {
    MM_MISC_YUV444_IN_MUX_ID_YUVTORGB          = 0,
    MM_MISC_YUV444_IN_MUX_ID_YUV444TO422       = 1,
}MM_MISC_YUV444_IN_MUX_ID;

/**
 *  @brief YUV444 input format mux select (select which to be YUV444 MUX ID input)
 */
typedef enum {
    MM_MISC_YUV444_IN_MUX_SEL_YUV422TO444      = 0,
    MM_MISC_YUV444_IN_MUX_SEL_YUVTORGB         = 1,
    MM_MISC_YUV444_IN_MUX_SEL_IPROAI             = 2,
    MM_MISC_YUV444_IN_MUX_SEL_DVP_TSRC         = 3,
}MM_MISC_YUV444_IN_MUX_SEL;

/**
 *  @brief Crop configuration struct definition
 */
typedef struct {
    uint16_t hsyncStart;                     /*!< Hsync valid start */
    uint16_t hsyncEnd;                       /*!< Hsync valid end */
    uint16_t vsyncStart;                     /*!< Vsync valid start */
    uint16_t vsyncEnd;                       /*!< Vsync valid end */
}MM_MISC_Crop_Cfg_Type;

/**
 *  @brief YUV2RGB configuration struct definition
 */
typedef struct {
    uint8_t  enable;                         /*!< Enable or disable yuv2rgb function */
    uint16_t preOffset0;                     /*!< YUV2RGB pre offset 0 */
    uint16_t preOffset1;                     /*!< YUV2RGB pre offset 1 */
    uint16_t preOffset2;                     /*!< YUV2RGB pre offset 2 */
    uint16_t postOffset0;                    /*!< YUV2RGB post offset 0 */
    uint16_t postOffset1;                    /*!< YUV2RGB post offset 1 */
    uint16_t postOffset2;                    /*!< YUV2RGB post offset 2 */
    uint16_t matrix00;                       /*!< YUV2RGB matrix 00 */
    uint16_t matrix01;                       /*!< YUV2RGB matrix 01 */
    uint16_t matrix02;                       /*!< YUV2RGB matrix 02 */
    uint16_t matrix10;                       /*!< YUV2RGB matrix 10 */
    uint16_t matrix11;                       /*!< YUV2RGB matrix 11 */
    uint16_t matrix12;                       /*!< YUV2RGB matrix 12 */
    uint16_t matrix20;                       /*!< YUV2RGB matrix 20 */
    uint16_t matrix21;                       /*!< YUV2RGB matrix 21 */
    uint16_t matrix22;                       /*!< YUV2RGB matrix 22 */
}MM_MISC_YUV2RGB_Cfg_Type;

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
    MM_MISC_Active_Level_Type hSyncLevel;        /*!< Input H-Sync active level */
    MM_MISC_Active_Level_Type vSyncLevel;        /*!< Input V-Sync active level */
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
 *  @brief MM MISC interrupt type definition
 */
typedef enum {
    MM_MISC_INT_SEOF0,      /*!< S/E-oF 0 interrupt */
    MM_MISC_INT_SEOF1,      /*!< S/E-oF 1 interrupt */
} MM_MISC_INT_Type;

/*
    SEOF number
 */
typedef enum {
    MM_MISC_SEOF0,
    MM_MISC_SEOF1,
    MM_MISC_SEOF_NUM,
}MM_MISC_SEOF_E;

typedef union {
    struct {
        uint8_t SoF:1;           /*!< SEOF type definition for vsync rising (SoF) */
        uint8_t EoF:1;           /*!< SEOF type definition for vsync falling (EoF)*/
        uint8_t lineCnt:1;       /*!< SEOF type definition for frame line count */
    }BF;
    uint8_t byte;
} MM_MISC_SEOF_TYPE_E;

typedef enum {
    MM_MISC_SEOF_INTR_SRC_SPI2DVP                = 0,
    MM_MISC_SEOF_INTR_SRC_DVP_AS                 = 0x1,
    MM_MISC_SEOF_INTR_SRC_ISP_OUTPUT             = 0x2,
    MM_MISC_SEOF_INTR_SRC_SCALER_F0              = 0x3,
    MM_MISC_SEOF_INTR_SRC_SCALER_F1              = 0x4,
    MM_MISC_SEOF_INTR_SRC_OSD_BLEND              = 0x5,
    MM_MISC_SEOF_INTR_SRC_OSD_DRAW               = 0x6,
    MM_MISC_SEOF_INTR_SRC_YUV444TO422            = 0x7,
    MM_MISC_SEOF_INTR_SRC_IPROAI                   = 0x8,
    MM_MISC_SEOF_INTR_SRC_DVP_TSRC               = 0x9,
    MM_MISC_SEOF_INTR_SRC_YUV422TO444            = 0xa,
    MM_MISC_SEOF_INTR_SRC_YUV2RGB                = 0xb,
    MM_MISC_SEOF_INTR_SRC_YUV422TO420            = 0xc,
    MM_MISC_SEOF_INTR_SRC_ISP_DEBUG_LINK_0       = 0xd,
    MM_MISC_SEOF_INTR_SRC_ISP_DEBUG_LINK_1       = 0xe,
    MM_MISC_SEOF_INTR_SRC_INVALID,
} MM_MISC_SEOF_INTR_SRC;

/**
 *  @brief soft reset ID (can OR to set in API MM_MISC_Set_SW_Reset)
 */
typedef enum {
    MM_MISC_SW_RESET_ID_MM_MISC                = (1 << 0),
    MM_MISC_SW_RESET_ID_DVP2AXI_A              = (1 << 1),
    MM_MISC_SW_RESET_ID_DVP2AXI_B              = (1 << 2),
    MM_MISC_SW_RESET_ID_DVP2AXI_C              = (1 << 3),
    MM_MISC_SW_RESET_ID_SCALER                 = (1 << 5),
    MM_MISC_SW_RESET_ID_OSD_DRAW               = (1 << 6),
    MM_MISC_SW_RESET_ID_OSD_BLEND              = (1 << 7),
    MM_MISC_SW_RESET_ID_JPG                    = (1 << 8),
    MM_MISC_SW_RESET_ID_IPROAI                 = (1 << 9),
    MM_MISC_SW_RESET_ID_ISP                    = (1 << 10),
    MM_MISC_SW_RESET_ID_DVP_TSRC               = (1 << 11),
    MM_MISC_SW_RESET_ID_SPI2DVP                = (1 << 12),
    MM_MISC_SW_RESET_ID_DVP_AS                 = (1 << 13),
}MM_MISC_SW_RESET_ID;

/**
 *  @brief clock generate ID (can OR to set in API MM_MISC_Set_Clock_Gen)
 */
typedef enum {
    MM_MISC_CLK_GEN_ID_SCALER                  = (1 << 0),
    MM_MISC_CLK_GEN_ID_OSD_DRAW                = (1 << 1),
    MM_MISC_CLK_GEN_ID_OSD_BLEND               = (1 << 2),
    MM_MISC_CLK_GEN_ID_ISP                     = (1 << 3),
    MM_MISC_CLK_GEN_ID_JPG                     = (1 << 4),
    MM_MISC_CLK_GEN_ID_DVP2AXI_A               = (1 << 5),
    MM_MISC_CLK_GEN_ID_DVP2AXI_B               = (1 << 6),
    MM_MISC_CLK_GEN_ID_DVP2AXI_C               = (1 << 7),
    MM_MISC_CLK_GEN_ID_DVP_TSRC                = (1 << 8),
}MM_MISC_CLK_GEN_ID;

/**
 *  @brief JPG Src select.
 */
typedef enum {
    MM_MISC_JPG_SRC_SEL_Y,
    MM_MISC_JPG_SRC_SEL_C,
}MM_MISC_JPG_SRC_SEL;

/**
 *  @brief IPROAIPRO cmd mode.
 */
typedef enum {
    MM_MISC_IPROAIPRO_MODE_BYPASS,
    MM_MISC_IPROAIPRO_MODE_FIX,
    MM_MISC_IPROAIPRO_MODE_PROACTIVE,
}MM_MISC_IPROAIPRO_MODE;

void MM_MISC_Set_SNSR_Reset(uint8_t value);
void MM_MISC_Set_SNSR_PowerDown(uint8_t value);
void MM_MISC_Set_PCLK_Force_On(uint16_t value);
void MM_MISC_Set_Mux_Shadow_Update(uint32_t id);
void MM_MISC_Set_SPI2DVP_Select(MM_MISC_SPI2DVP_SEL sel);
void MM_MISC_Set_DVP_AS_SRAM_Mode(MM_MISC_SHARED_SRAM_MODE mode);
void MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT port, MM_MISC_DVP2AXI_SRC src);
void MM_MISC_Get_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT port, MM_MISC_DVP2AXI_SRC *src);
void MM_MISC_Set_YUV422_In_Mux(MM_MISC_YUV422_IN_MUX_ID id, MM_MISC_YUV422_IN_MUX_SEL sel);
void MM_MISC_Set_YUV444_In_Mux(MM_MISC_YUV444_IN_MUX_ID id, MM_MISC_YUV444_IN_MUX_SEL sel);
void MM_MISC_Set_Crop_Cfg(MM_MISC_Crop_Cfg_Type* cropCfg);
void MM_MISC_Get_Crop_Cfg(MM_MISC_Crop_Cfg_Type* cropCfg);
void MM_MISC_Set_Crop_State(uint8_t state);
void MM_MISC_Get_Crop_State(uint8_t *state);
void MM_MISC_YUV2RGB_Init(MM_MISC_YUV2RGB_Cfg_Type* yuv2rgbCfg);
void MM_MISC_Get_YUV2RGB_Cfg(MM_MISC_YUV2RGB_Cfg_Type* yuv2rgbCfg);
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
void MM_MISC_Set_SEOF_Src(MM_MISC_SEOF_E seof, MM_MISC_SEOF_INTR_SRC selection);
void MM_MISC_Get_SEOF_Src(MM_MISC_SEOF_E seof, MM_MISC_SEOF_INTR_SRC *selection);
void MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF_E seof, MM_MISC_SEOF_TYPE_E type);
void MM_MISC_Get_SEOF_TypeEn(MM_MISC_SEOF_E seof, MM_MISC_SEOF_TYPE_E *type);
void MM_MISC_Set_SEOF_LineCnt(MM_MISC_SEOF_E seof, uint16_t lineCnt);
void MM_MISC_Get_SEOF_TypeStatus(MM_MISC_SEOF_E seof, MM_MISC_SEOF_TYPE_E *type);
void MM_MISC_Clr_SEOF_TypeStatus(MM_MISC_SEOF_E seof, MM_MISC_SEOF_TYPE_E type);
void MM_MISC_Set_SW_Reset(uint32_t id, uint8_t aRst);
void MM_MISC_Set_Clock_Gen(uint32_t id, uint8_t enable);

void MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL src, MM_MISC_DVP2AXI_PORT port);
void MM_MISC_Get_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL src, MM_MISC_DVP2AXI_PORT *port);

void MM_MISC_Set_IPROAIPRO_Rcmd_Mode(MM_MISC_IPROAIPRO_MODE mode);
void MM_MISC_Set_IPROAIPRO_Rcmd_MasterEn(uint8_t dvp2axi_a, uint8_t dvp2axi_b, uint8_t dvp2axi_c);
void MM_MISC_Set_IPROAIPRO_Wcmd_Mode(MM_MISC_IPROAIPRO_MODE mode);
void MM_MISC_Set_IPROAIPRO_Wcmd_MasterEn(uint8_t dvp2axi_a, uint8_t dvp2axi_b, uint8_t dvp2axi_c);

#endif //__DRV_ISP_MISC_H__
