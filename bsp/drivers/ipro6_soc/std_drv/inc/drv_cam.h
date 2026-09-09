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


#ifndef __DRV_CAM_H__
#define __DRV_CAM_H__

#include "cam_reg.h"
#include "dvp_misc_reg.h"
#include "drv_common.h"

typedef enum {
    CAM_DFE_BYTE_LOWER,  /*!< The first data is lower-byte */
    CAM_DFE_BYTE_HIGHER, /*!< The first data is higher-byte */
}CAM_DFE_Byte_Order_Type;

typedef enum {
    CAM_DFE_SYNC_ACTIVE_HIGH, /*!< Input is active-high */
    CAM_DFE_SYNC_ACTIVE_LOW,  /*!< Input is active-low */
}CAM_DFE_Sync_Level_Type;

typedef enum {
    CAM_DFE_PWDN_RESET_LEVEL_LOW,  /*!< Low level */
    CAM_DFE_PWDN_RESET_LEVEL_HIGH, /*!< High level */
}CAM_DFE_PWDN_Reset_Level_Type;

typedef enum {
    CAM_DFE_YUV422,        /*!< Low level */
    CAM_DFE_YUV422_TO_420, /*!< High level */
}CAM_DFE_Function_Type;

typedef enum {
    CAM_DFE_FRAME_ID_VSYNC_NEGEDGE, /*!< Frame ID base on vsync negedge */
    CAM_DFE_FRAME_ID_VSYNC_POSEDGE, /*!< Frame ID base on vsync posedge */
} CAM_DFE_Frame_ID_Edge_Type;

typedef enum {
    CAM0_ID,    /*!< Camera0 port define */
    CAM1_ID,    /*!< Camera1 port define */
    CAM_ID_MAX, /*!< Camera max ID define */
} CAM_ID_Type;

typedef enum {
    CAM_BURST_TYPE_SINGLE,     /*!< Camera AHB burst type:single */
    CAM_BURST_TYPE_INCR4,      /*!< Camera AHB burst type:incrementing 4 */
    CAM_BURST_TYPE_INCR8,      /*!< Camera AHB burst type:incrementing 8 */
    CAM_BURST_TYPE_INCR16,     /*!< Camera AHB burst type:incrementing 16 */
    CAM_BURST_TYPE_INCR32 = 5, /*!< Camera AHB burst type:incrementing 32 */
    CAM_BURST_TYPE_INCR64,     /*!< Camera AHB burst type:incrementing 64 */
} CAM_Burst_Type;

typedef enum {
    CAM_SW_MODE_AUTO,   /*!< CAM auto mode with mjpeg */
    CAM_SW_MODE_MANUAL, /*!< CAM manual mode(software mode) */
} CAM_SW_Mode_Type;

typedef enum {
    CAM_DROP_NONE,               /*!< Not drop */
    CAM_DROP_ODD_ROW_ODD_PIXEL,  /*!< Dropp odd pix data in odd rows, for example:(raw data:YUYV YUYV..., processed data:YUYV YY...) */
    CAM_DROP_ODD_ROW_EVEN_PIXEL, /*!< Dropp even pix data in odd rows, for example:(raw data:UYVY UYVY..., processed data:UYVY YY...) */
    CAM_DROP_ALL_ODD_PIXEL,      /*!< Dropp odd pix data, for example:(raw data:YUYV YUYV..., processed data:YY YY...) */
    CAM_DROP_ALL_EVEN_PIXEL,     /*!< Dropp even pix data, for example:(raw data:UYVY UYVY..., processed data:YY YY...) */
} CAM_Output_Drop_Mode_Type;

typedef enum {
    CAM_PIX_DATA_BIT_16,     /*!< CAM pix data is 16-bit */
    CAM_PIX_DATA_BIT_24,     /*!< CAM pix data is 24-bit */
    CAM_PIX_DATA_BIT_24TO16, /*!< CAM pix data is 24-comp-16-bit */
    CAM_PIX_DATA_BIT_24TO32, /*!< CAM pix data is 24-exp-32-bit */
    CAM_PIX_DATA_BIT_8,      /*!< CAM pix data is 8-bit */
} CAM_Input_Pix_Width_Type;

typedef enum {
    CAM_8_BIT_SELECT_LOWER, /*!< CAM select the lower byte of pix data */
    CAM_8_BIT_SELECT_UPPER, /*!< CAM select the upper byte of pix data */
} CAM_8_Bit_Byte_Type;

typedef enum {
    CAM_16_BIT_RGB, /*!< CAM 16-bit pix data is RGB */
    CAM_16_BIT_GRB, /*!< CAM 16-bit pix data is GRB */
    CAM_16_BIT_RBG, /*!< CAM 16-bit pix data is RBG */
    CAM_16_BIT_GBR, /*!< CAM 16-bit pix data is GBR */
    CAM_16_BIT_BRG, /*!< CAM 16-bit pix data is BRG */
    CAM_16_BIT_BGR, /*!< CAM 16-bit pix data is BGR */
} CAM_16_Bit_RGB_Type;

typedef enum {
    CAM_LINE_ACTIVE_POLARITY_LOW,  /*!< CAM line active polarity low */
    CAM_LINE_ACTIVE_POLARITY_HIGH, /*!< CAM line active polarity high */
} CAM_Line_Active_Pol;

typedef enum {
    CAM_FRAME_ACTIVE_POLARITY_LOW,  /*!< CAM frame active polarity low */
    CAM_FRAME_ACTIVE_POLARITY_HIGH, /*!< CAM frame active polarity high */
} CAM_Frame_Active_Pol;

typedef enum {
    CAM_SENSOR_MODE_V_AND_H, /*!< CAM sensor type v and h */
    CAM_SENSOR_MODE_V_OR_H,  /*!< CAM sensor type v  or h */
    CAM_SENSOR_MODE_V,       /*!< CAM sensor type v */
    CAM_SENSOR_MODE_H,       /*!< CAM sensor type h */
} CAM_Sensor_Mode_Type;

typedef enum {
    CAM_INT_NORMAL,           /*!< Normal write interrupt */
    CAM_INT_MEMORY_OVERWRITE, /*!< Memory overwrite interrupt */
    CAM_INT_FRAME_OVERWRITE,  /*!< Frame overwrite interrupt */
    CAM_INT_FIFO_OVERWRITE,   /*!< Fifo overwrite interrupt */
    CAM_INT_VSYNC_CNT_ERROR,  /*!< Vsync valid line count non-match interrupt */
    CAM_INT_HSYNC_CNT_ERROR,  /*!< Hsync valid pixel count non-match interrupt */
    CAM_INT_ALL,              /*!< All of interrupt */
} CAM_INT_Type;

typedef struct {
    CAM_DFE_Sync_Level_Type hSyncLevel; /*!< Input H-Sync active level */
    CAM_DFE_Sync_Level_Type vSyncLevel; /*!< Input V-Sync active level */
    CAM_DFE_Byte_Order_Type dataOrder;  /*!< Input data order */
    uint16_t fifoThreshold;             /*!< Fifo threshold for each DVP line to start to output,
                                             ((Width-Fifo_th)*T_in*2)<(Width*T_out)<((Width-Fifo_th)*T_in*2+Blank*T_in) */
}CAM_DFE_Cfg_Type;

typedef struct
{
    CAM_SW_Mode_Type swMode;            /*!< Software mode */
    uint8_t swIntCnt;                   /*!< Set frame count to issue interrupt at software mode */
    CAM_Input_Pix_Width_Type pixWidth;  /*!< Input pix data width */
    CAM_Output_Drop_Mode_Type dropMode; /*!< Drop mode */
    CAM_Frame_Active_Pol framePol;      /*!< Frame polarity */
    CAM_Line_Active_Pol linePol;        /*!< Line polarity */
    CAM_Burst_Type burstType;           /*!< AHB burst type */
    CAM_Sensor_Mode_Type camSensorMode; /*!< CAM sensor mode */
    uint8_t waitCount;                  /*!< Cycles in FSM wait mode, default value:0x40 */
    uint32_t memStart;                  /*!< Data start address */
    uint32_t memSize;                   /*!< Memory size in burst */
    uint32_t frameSize;                 /*!< Frame byte size */
} CAM_CFG_Type;

typedef struct
{
    uint8_t validFrames;    /*!< Valid frames */
    uint16_t curFrameId;    /*!< Current frame id */
    uint32_t curFrameAddr;  /*!< Current frame address */
    uint32_t curFrameBytes; /*!< Current frame bytes */
    uint32_t status;        /*!< CAM module status */
} CAM_Frame_Info;

#define IS_CAM_DFE_BYTE_ORDER_TYPE(type) (((type) == CAM_DFE_BYTE_LOWER) || \
                                          ((type) == CAM_DFE_BYTE_HIGHER))

#define IS_CAM_DFE_SYNC_LEVEL_TYPE(type) (((type) == CAM_DFE_SYNC_ACTIVE_HIGH) || \
                                          ((type) == CAM_DFE_SYNC_ACTIVE_LOW))

#define IS_CAM_DFE_PWDN_RESET_LEVEL_TYPE(type) (((type) == CAM_DFE_PWDN_RESET_LEVEL_LOW) || \
                                                ((type) == CAM_DFE_PWDN_RESET_LEVEL_HIGH))

#define IS_CAM_DFE_FUNCTION_TYPE(type) (((type) == CAM_DFE_YUV422) || \
                                        ((type) == CAM_DFE_YUV422_TO_420))

#define IS_CAM_DFE_FRAME_ID_EDGE_TYPE(type) (((type) == CAM_DFE_FRAME_ID_VSYNC_NEGEDGE) || \
                                             ((type) == CAM_DFE_FRAME_ID_VSYNC_POSEDGE))

#define IS_CAM_ID_TYPE(type) (((type) == CAM0_ID) || \
                              ((type) == CAM1_ID) || \
                              ((type) == CAM_ID_MAX))

#define IS_CAM_BURST_TYPE(type) (((type) == CAM_BURST_TYPE_SINGLE) || \
                                 ((type) == CAM_BURST_TYPE_INCR4) ||  \
                                 ((type) == CAM_BURST_TYPE_INCR8) ||  \
                                 ((type) == CAM_BURST_TYPE_INCR16) || \
                                 ((type) == CAM_BURST_TYPE_INCR32) || \
                                 ((type) == CAM_BURST_TYPE_INCR64))

#define IS_CAM_SW_MODE_TYPE(type) (((type) == CAM_SW_MODE_AUTO) || \
                                   ((type) == CAM_SW_MODE_MANUAL))

#define IS_CAM_OUTPUT_DROP_MODE_TYPE(type) (((type) == CAM_DROP_NONE) ||               \
                                            ((type) == CAM_DROP_ODD_ROW_ODD_PIXEL) ||  \
                                            ((type) == CAM_DROP_ODD_ROW_EVEN_PIXEL) || \
                                            ((type) == CAM_DROP_ALL_ODD_PIXEL) ||      \
                                            ((type) == CAM_DROP_ALL_EVEN_PIXEL))

#define IS_CAM_INPUT_PIX_WIDTH_TYPE(type) (((type) == CAM_PIX_DATA_BIT_16) ||     \
                                           ((type) == CAM_PIX_DATA_BIT_24) ||     \
                                           ((type) == CAM_PIX_DATA_BIT_24TO16) || \
                                           ((type) == CAM_PIX_DATA_BIT_24TO32) || \
                                           ((type) == CAM_PIX_DATA_BIT_8))

#define IS_CAM_8_BIT_BYTE_TYPE(type) (((type) == CAM_8_BIT_SELECT_LOWER) || \
                                      ((type) == CAM_8_BIT_SELECT_UPPER))

#define IS_CAM_16_BIT_RGB_TYPE(type) (((type) == CAM_16_BIT_RGB) || \
                                      ((type) == CAM_16_BIT_GRB) || \
                                      ((type) == CAM_16_BIT_RBG) || \
                                      ((type) == CAM_16_BIT_GBR) || \
                                      ((type) == CAM_16_BIT_BRG) || \
                                      ((type) == CAM_16_BIT_BGR))

#define IS_CAM_LINE_ACTIVE_POL(type) (((type) == CAM_LINE_ACTIVE_POLARITY_LOW) || \
                                      ((type) == CAM_LINE_ACTIVE_POLARITY_HIGH))

#define IS_CAM_FRAME_ACTIVE_POL(type) (((type) == CAM_FRAME_ACTIVE_POLARITY_LOW) || \
                                       ((type) == CAM_FRAME_ACTIVE_POLARITY_HIGH))

#define IS_CAM_SENSOR_MODE_TYPE(type) (((type) == CAM_SENSOR_MODE_V_AND_H) || \
                                       ((type) == CAM_SENSOR_MODE_V_OR_H) ||  \
                                       ((type) == CAM_SENSOR_MODE_V) ||       \
                                       ((type) == CAM_SENSOR_MODE_H))

#define IS_CAM_INT_TYPE(type) (((type) == CAM_INT_NORMAL) ||           \
                               ((type) == CAM_INT_MEMORY_OVERWRITE) || \
                               ((type) == CAM_INT_FRAME_OVERWRITE) ||  \
                               ((type) == CAM_INT_FIFO_OVERWRITE) ||   \
                               ((type) == CAM_INT_VSYNC_CNT_ERROR) ||  \
                               ((type) == CAM_INT_HSYNC_CNT_ERROR) ||  \
                               ((type) == CAM_INT_ALL))

#ifndef IPRO_USE_HAL_DRIVER
void CAM0_IRQHandler(void);
void CAM1_IRQHandler(void);
#endif
void CAM_DFE_Init(CAM_DFE_Cfg_Type* dfeCfg);
void CAM_DFE_Enable(void);
void CAM_DFE_Disable(void);
void CAM_DFE_Set_Power_Down(CAM_DFE_PWDN_Reset_Level_Type level);
void CAM_DFE_Set_Reset(CAM_DFE_PWDN_Reset_Level_Type level);
void CAM_DFE_Function_Select(CAM_DFE_Function_Type type);
void CAM_DFE_Frame_ID_Init(CAM_DFE_Frame_ID_Edge_Type edgeType, uint16_t count);
void CAM_DFE_Frame_ID_Reset(void);
void CAM_Init(CAM_ID_Type camId, CAM_CFG_Type *cfg);
void CAM_Deinit(CAM_ID_Type camId);
void CAM_Enable(CAM_ID_Type camId);
void CAM_Disable(CAM_ID_Type camId);
void CAM_Clock_Gate(CAM_ID_Type camId, int enable);
void CAM_Hsync_Crop(CAM_ID_Type camId, uint16_t start, uint16_t end);
void CAM_Vsync_Crop(CAM_ID_Type camId, uint16_t start, uint16_t end);
void CAM_Set_Hsync_Total_Count(CAM_ID_Type camId, uint16_t count);
void CAM_Set_Vsync_Total_Count(CAM_ID_Type camId, uint16_t count);
void CAM_Get_Frame_Info(CAM_ID_Type camId, CAM_Frame_Info *info);
uint8_t CAM_Get_Frame_Count(CAM_ID_Type camId);
void CAM_Pop_Frame(CAM_ID_Type camId);
void CAM_IntMask(CAM_ID_Type camId, CAM_INT_Type intType, int intMask);
void CAM_IntClr(CAM_ID_Type camId, CAM_INT_Type intType);
void CAM_Int_Callback_Install(CAM_ID_Type camId, CAM_INT_Type intType, intCallback_Type *cbFun);
void CAM_8_Bit_Byte_Select(CAM_ID_Type camId, CAM_8_Bit_Byte_Type byteType);
void CAM_16_Bit_RGB_order(CAM_ID_Type camId, CAM_16_Bit_RGB_Type rgbType);
void CAM_32_Bit_Alpha_Set(CAM_ID_Type camId, uint8_t alpha);
void CAM_Set_Frame_Valid_Period(CAM_ID_Type camId, uint8_t period, uint32_t validBit);
void CAM_HW_Cover_First_Frame_Everytime(CAM_ID_Type camId, int enable);

#endif /* __DRV_CAM_H__ */
