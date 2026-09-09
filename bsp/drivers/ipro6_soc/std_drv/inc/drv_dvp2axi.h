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

#ifndef __DRV_DVP2AXI_H__
#define __DRV_DVP2AXI_H__

#include "dvp2axi_reg.h"
#include "drv_common.h"

typedef enum {
    DVP2AXI_PORT_A,
    DVP2AXI_PORT_B,
    DVP2AXI_PORT_MAX, /*!< max ID define */
} DVP2AXI_PORT_E;

typedef enum {
    DVP2AXI_DATA_ACTIVE_LOW = 0,
    DVP2AXI_DATA_ACTIVE_HIGH = 1,
} DVP2AXI_DATA_POLARITY_E;

typedef enum {
    DVP2AXI_DATA_VALID_V_AND_H = 0,
    DVP2AXI_DATA_VALID_V_OR_H = 1,
    DVP2AXI_DATA_VALID_V = 2,
    DVP2AXI_DATA_VALID_H = 3,
} DVP2AXI_DATA_VALID_E;

typedef enum {
    DVP2AXI_DATA_16_BIT = 0,
    DVP2AXI_DATA_24_BIT = 1,
    DVP2AXI_DATA_24_COMP_16BIT = 2,
    DVP2AXI_DATA_24_EXP_32BIT = 3,
    DVP2AXI_DATA_8_BIT = 4,
    DVP2AXI_DATA_12_BIT = 5,
} DVP2AXI_DATA_MODE_E;

typedef enum {
    DVP2AXI_SEL_LOW_BYTE = 0,
    DVP2AXI_SEL_HIGH_BYTE = 1,
} DVP2AXI_BYTE_SEL_E;

typedef enum {
    DVP2AXI_ODD_LINE_MASKED = 0,
    DVP2AXI_EVEN_LINE_MASKED = 1,
} DVP2AXI_LINE_MASK_E;

typedef struct {
	DVP2AXI_DATA_MODE_E bit_width;
    DVP2AXI_BYTE_SEL_E byte_sel;
    uint8_t v_subsampl_enable;
} DVP2AXI_CFG_S;

typedef struct {
    DVP2AXI_DATA_POLARITY_E framePol;   /*!< Frame polarity */
    DVP2AXI_DATA_POLARITY_E linePol;    /*!< Line polarity */
    uint8_t swIntCnt;                   /*!< Set frame count to issue interrupt at software mode */
    DVP2AXI_DATA_VALID_E sensorMode;    /*!< Sensor mode */
    DVP2AXI_DATA_MODE_E bitWidth;       /*!< Pix data width */
    uint8_t subSampleEn;                /*!< Subsample enable for UV at 420 */
    uint8_t wrapEn;                     /*!< wrap (ring) mode enable */
    uint16_t wrapLine;                  /*!< wrap (ring) line count */
    DVP2AXI_BYTE_SEL_E byte_sel;        /*!< avaliable when 8-bit mode */
    uint32_t memStart;                  /*!< output start address */
    uint16_t imgWidth;                  /*!< image width (pixel) */
    uint16_t imgHeight;                 /*!< image height (line) */
} DVP2AXI_INIT_CFG_S;

typedef enum {
    DVP2AXI_16_BIT_RGB, /*!< DVP2AXI 16-bit pix data is RGB */
    DVP2AXI_16_BIT_GRB, /*!< DVP2AXI 16-bit pix data is GRB */
    DVP2AXI_16_BIT_RBG, /*!< DVP2AXI 16-bit pix data is RBG */
    DVP2AXI_16_BIT_GBR, /*!< DVP2AXI 16-bit pix data is GBR */
    DVP2AXI_16_BIT_BRG, /*!< DVP2AXI 16-bit pix data is BRG */
    DVP2AXI_16_BIT_BGR, /*!< DVP2AXI 16-bit pix data is BGR */
} DVP2AXI_16_BIT_RGB_E;

/**
 *  @brief DVP2AXI interrupt type definition
 */
typedef enum {
    DVP2AXI_INT_NORMAL,           /*!< Normal write interrupt */
    DVP2AXI_INT_FIFO_OVERWRITE,   /*!< Fifo overwrite interrupt */
    DVP2AXI_INT_VSYNC_CNT_ERROR,  /*!< Vsync valid line count non-match interrupt */
    DVP2AXI_INT_HSYNC_CNT_ERROR,  /*!< Hsync valid pixel count non-match interrupt */
    DVP2AXI_INT_ALL,              /*!< All of interrupt */
} DVP2AXI_INT_Type;

#define IS_DVP2AXI_ID_TYPE(type)                          (((type) == DVP2AXI_A) || \
                                                          ((type)  == DVP2AXI_B) || \
                                                          ((type)  == DVP2AXI_C))

#define IS_DVP2AXI_16_BIT_RGB_TYPE(type) (((type) == DVP2AXI_16_BIT_RGB) || \
                                      ((type) == DVP2AXI_16_BIT_GRB) || \
                                      ((type) == DVP2AXI_16_BIT_RBG) || \
                                      ((type) == DVP2AXI_16_BIT_GBR) || \
                                      ((type) == DVP2AXI_16_BIT_BRG) || \
                                      ((type) == DVP2AXI_16_BIT_BGR))

/** @defgroup  DVP2AXI_INT_Type
 *  @{
 */
#define IS_DVP2AXI_INT_TYPE(type) (((type) == DVP2AXI_INT_NORMAL) ||           \
                                   ((type) == DVP2AXI_INT_FIFO_OVERWRITE) ||   \
                                   ((type) == DVP2AXI_INT_VSYNC_CNT_ERROR) ||  \
                                   ((type) == DVP2AXI_INT_HSYNC_CNT_ERROR) ||  \
                                   ((type) == DVP2AXI_INT_ALL))

void DVP2AXI_Init(DVP2AXI_PORT_E port, DVP2AXI_INIT_CFG_S* init);
void DVP2AXI_Config(DVP2AXI_PORT_E port, DVP2AXI_CFG_S* cfg);

void DVP2AXI_SetFrameValid(DVP2AXI_PORT_E port, DVP2AXI_DATA_POLARITY_E frm_vld);
void DVP2AXI_SetFrameSize(DVP2AXI_PORT_E port, uint32_t bytesPerLine, uint32_t height);
void DVP2AXI_MemConfig(DVP2AXI_PORT_E port, uint32_t bufAddr, uint32_t linesForRing);
void DVP2AXI_Enable(DVP2AXI_PORT_E port);
void DVP2AXI_Disable(DVP2AXI_PORT_E port);
uint32_t DVP2AXI_GetBusyStatus(DVP2AXI_PORT_E port);
void DVP2AXI_SetFrameActive(DVP2AXI_PORT_E port, DVP2AXI_DATA_POLARITY_E frame_active);
void DVP2AXI_SetLineActive(DVP2AXI_PORT_E port, DVP2AXI_DATA_POLARITY_E line_active);
void DVP2AXI_SetBitWidth(DVP2AXI_PORT_E port, DVP2AXI_DATA_MODE_E bitwidth);
void DVP2AXI_GetFrameBuffer(DVP2AXI_PORT_E port, uint32_t *frameAddr, uint32_t *byteCnt);
void DVP2AXI_SetAddress(DVP2AXI_PORT_E port, uint32_t bufAddr);


void DVP2AXI_Crop(DVP2AXI_PORT_E port, uint32_t h_start, int32_t h_end, uint32_t v_start, int32_t v_end);
void DVP2AXI_Set_Frame_Skip(DVP2AXI_PORT_E port, uint32_t bitwiseFrmVld, uint32_t frmPeriodCnt);
void DVP2AXI_Valid_Cnt_Exm(DVP2AXI_PORT_E port, uint32_t hcnt, uint32_t vcnt);

void DVP2AXI_16_Bit_RGB_order(DVP2AXI_PORT_E port, DVP2AXI_16_BIT_RGB_E rgbType);

void DVP2AXI_WrapModeEnable(DVP2AXI_PORT_E port);
void DVP2AXI_WrapModeDisable(DVP2AXI_PORT_E port);
void DVP2AXI_SetStrideSize(DVP2AXI_PORT_E port, uint32_t bytesPerLine);

void DVP2AXI_FlipModeEnable(DVP2AXI_PORT_E port);
void DVP2AXI_FlipModeDisable(DVP2AXI_PORT_E port);

void DVP2AXI_IntMask(DVP2AXI_PORT_E port, DVP2AXI_INT_Type intType, int intMask);
void DVP2AXI_IntClr(DVP2AXI_PORT_E port, DVP2AXI_INT_Type intType);
uint32_t DVP2AXI_IntStatus(DVP2AXI_PORT_E port, DVP2AXI_INT_Type intType);

uint16_t DVP2AXI_GetLineOffset(uint32_t bytesPerLine);

#endif /* __DRV_DVP2AXI_H__ */
