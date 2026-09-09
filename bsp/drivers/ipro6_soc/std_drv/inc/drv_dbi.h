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


#ifndef __DRV_DBI_H__
#define __DRV_DBI_H__

#include "dbi_reg.h"
#include "drv_common.h"

typedef enum {
    DBI_TYPE_B = 0,    /*!< DBI type B */
    DBI_TYPE_C_4_WIRE, /*!< DBI type C 3 wire mode */
    DBI_TYPE_C_3_WIRE, /*!< DBI type C 4 wire mode */
    DBI_TYPE_QSPI,     /*!< DBI type QSPI mode */
} DBI_Mode_Type;

typedef enum {
    DBI_PIXEL_RGB565 = 0, /*!< DBI pixel format is RGB565 */
    DBI_PIXEL_RGB666 = 1, /*!< DBI pixel format is RGB666 */
    DBI_PIXEL_RGB888 = 1, /*!< DBI pixel format is RGB888 */
} DBI_Pixel_Format_Type;

typedef enum {
    DBI_CS_STRETCHMODE_DISABLE = 0, /*!< CS will de-assert before FIFO is filled again and new transfer starts */
    DBI_CS_STRETCHMODE_ENABLE,      /*!< CS will stay asserted while waiting for FIFO to be filled again */
} DBI_CS_StretchMode_Type;

typedef enum {
    DBI_SCL_CLK_PHASE_0 = 0, /*!< DBI SCL clock phase 0 */
    DBI_SCL_CLK_PHASE_1,     /*!< DBI SCL clock phase 1 */
} DBI_SCL_CLK_Phase_Type;

typedef enum {
    DBI_SCL_CLK_POLARITY_LOW = 0, /*!< DBI SCL clock output low at idle state */
    DBI_SCL_CLK_POLARITY_HIGH,    /*!< DBI SCL clock output high at idle state */
} DBI_SCL_CLK_Polarity_Type;

typedef enum {
    DBI_DATA_READ = 0, /*!< DBI read data */
    DBI_DATA_WRITE,    /*!< DBI write data */
} DBI_Data_Direction_Type;

typedef enum {
    DBI_DATA_NORMAL = 0, /*!< Normal data */
    DBI_DATA_PIXEL,      /*!< Pixel data */
} DBI_Data_Type;

typedef enum {
    DBI_INT_END = 0,     /*!< DBI transfer end interrupt,shared by both type B and C mode */
    DBI_INT_TX_FIFO_REQ, /*!< DBI tx fifo ready interrupt(tx fifo count > tx fifo threshold) */
    DBI_INT_FIFO_ERR,    /*!< DBI tx/rx fifo overflow/underflow error interrupt */
    DBI_INT_ALL,         /*!< DBI all interrupt */
} DBI_INT_Type;

typedef enum {
    DBI_TX_OVERFLOW,  /*!< DBI tx fifo overflow */
    DBI_TX_UNDERFLOW, /*!< DBI tx fifo underflow */
} DBI_Overflow_Type;

typedef enum {
    DBI_FIFO_MODE_RGB = 0,  /*!< RBG mode, The FIFO format is set for DBI_FIFO_Format_Type */
    DBI_FIFO_MODE_YUV,      /*!< YUV mode, Change the Settings of DBI_FIFO_Format_Type, FIFO format: R->Y,  G->U/Cb,  B->V/Cr */
} DBI_FIFO_Mode_Type;

typedef enum {
    DBI_FIFO_888_NBGR = 0, /*!< DBI fifo format is None[31:24] B[23:16] G[15:8] R[7:0] */
    DBI_FIFO_888_NRGB,     /*!< DBI fifo format is None[31:24] R[23:16] G[15:8] B[7:0] */
    DBI_FIFO_888_BGRN,     /*!< DBI fifo format is B[31:24] G[23:16] R[15:8] None[7:0] */
    DBI_FIFO_888_RGBN,     /*!< DBI fifo format is R[31:24] G[23:16] B[15:8] None[7:0] */
    DBI_FIFO_888_RGBR,     /*!< DBI fifo format is R[31:24] G[23:16] B[15:8] R[7:0] */
    DBI_FIFO_888_BGRB,     /*!< DBI fifo format is B[31:24] G[23:16] R[15:8] B[7:0] */
    DBI_FIFO_565_BGRBGR,   /*!< DBI fifo format is B[31:27] G[26:21] R[20:16] B[15:11] G[10:5] R[4:0] */
    DBI_FIFO_565_RGBRGB,   /*!< DBI fifo format is R[31:27] G[26:21] B[20:16] R[15:11] G[10:5] B[4:0] */
} DBI_FIFO_Format_Type;

typedef struct
{
    uint8_t startLen;                      /*!< Length of start/stop condition */
    uint8_t dataPhase0Len;                 /*!< Length of data phase 0 */
    uint8_t dataPhase1Len;                 /*!< Length of data phase 1 */
    uint8_t intervalLen;                   /*!< Length of interval between pixel data */
}DBI_Period_CFG_Type;

typedef struct
{
    DBI_Mode_Type mode;                    /*!< DBI type B or C select */
    DBI_Pixel_Format_Type pixelFormat;     /*!< DBI pixel format */
    DBI_FIFO_Format_Type fifoFormat;       /*!< DBI fifo format */
    DBI_CS_StretchMode_Type CSStretchMode; /*!< DBI signal of CS-low stretch mode */
    int continueEn;                /*!< Enable:CS will stay asserted between each consecutive pixel, disable:CS will de-assert between each pixel */
    int dummyEn;                   /*!< Enable:dummy cycle will be inserted between command phase adn data phase, disable:no dummy cycle */
    uint8_t dummyCnt;                      /*!< Dummy cycle count,effective only in type C(fixed to 1 in type B) */
    DBI_SCL_CLK_Phase_Type clkPhase;       /*!< DBI clock phase */
    DBI_SCL_CLK_Polarity_Type clkPolarity; /*!< DBI clock polarity */
    DBI_Period_CFG_Type period;            /*!< Period configuration */
} DBI_CFG_Type;

typedef struct
{
    int enable;   /*!< Enable or disable yuv2rgb function */
    uint16_t preOffset0;  /*!< YUV2RGB pre offset 0 */
    uint16_t preOffset1;  /*!< YUV2RGB pre offset 1 */
    uint16_t preOffset2;  /*!< YUV2RGB pre offset 2 */
    uint16_t postOffset0; /*!< YUV2RGB post offset 0 */
    uint16_t postOffset1; /*!< YUV2RGB post offset 1 */
    uint16_t postOffset2; /*!< YUV2RGB post offset 2 */
    uint16_t matrix00;    /*!< YUV2RGB matrix 00 */
    uint16_t matrix01;    /*!< YUV2RGB matrix 01 */
    uint16_t matrix02;    /*!< YUV2RGB matrix 02 */
    uint16_t matrix10;    /*!< YUV2RGB matrix 10 */
    uint16_t matrix11;    /*!< YUV2RGB matrix 11 */
    uint16_t matrix12;    /*!< YUV2RGB matrix 12 */
    uint16_t matrix20;    /*!< YUV2RGB matrix 20 */
    uint16_t matrix21;    /*!< YUV2RGB matrix 21 */
    uint16_t matrix22;    /*!< YUV2RGB matrix 22 */
} DBI_YUV2RGB_Cfg_Type;

#define IS_DBI_MODE_TYPE(type) (((type) == DBI_TYPE_B) ||        \
                                ((type) == DBI_TYPE_C_4_WIRE) || \
                                ((type) == DBI_TYPE_C_3_WIRE) || \
                                ((type) == DBI_TYPE_QSPI))

#define IS_DBI_PIXEL_FORMAT_TYPE(type) (((type) == DBI_PIXEL_RGB565) || \
                                        ((type) == DBI_PIXEL_RGB666) || \
                                        ((type) == DBI_PIXEL_RGB888))

#define DBI_CS_STRETCHMODE_TYPE(type) (((type) == DBI_CS_STRETCHMODE_DISABLE) || \
                                       ((type) == DBI_CS_STRETCHMODE_ENABLE))

#define IS_DBI_SCL_CLK_PHASE_TYPE(type) (((type) == DBI_SCL_CLK_PHASE_0) || \
                                         ((type) == DBI_SCL_CLK_PHASE_1))

#define IS_DBI_SCL_CLK_POLARITY_TYPE(type) (((type) == DBI_SCL_CLK_POLARITY_LOW) || \
                                            ((type) == DBI_SCL_CLK_POLARITY_HIGH))

#define IS_DBI_DATA_DIRECTION_TYPE(type) (((type) == DBI_DATA_READ) || \
                                          ((type) == DBI_DATA_WRITE))

#define IS_DBI_DATA_TYPE(type) (((type) == DBI_DATA_NORMAL) || \
                                ((type) == DBI_DATA_PIXEL))

#define IS_DBI_INT_TYPE(type) (((type) == DBI_INT_END) ||         \
                               ((type) == DBI_INT_TX_FIFO_REQ) || \
                               ((type) == DBI_INT_FIFO_ERR) ||    \
                               ((type) == DBI_INT_ALL))

#define IS_DBI_OVERFLOW_TYPE(type) (((type) == DBI_TX_OVERFLOW) || \
                                    ((type) == DBI_TX_UNDERFLOW))

#define IS_DBI_FIFO_FORMAT_TYPE(type) (((type) == DBI_FIFO_888_NBGR) ||   \
                                       ((type) == DBI_FIFO_888_NRGB) ||   \
                                       ((type) == DBI_FIFO_888_BGRN) ||   \
                                       ((type) == DBI_FIFO_888_RGBN) ||   \
                                       ((type) == DBI_FIFO_888_RGBR) ||   \
                                       ((type) == DBI_FIFO_888_BGRB) ||   \
                                       ((type) == DBI_FIFO_565_BGRBGR) || \
                                       ((type) == DBI_FIFO_565_RGBRGB))

#define DBI_TX_FIFO_SIZE 8

#ifndef IPRO_USE_HAL_DRIVER
void DBI_IRQHandler(void);
#endif
int DBI_Init(DBI_CFG_Type *dbiCfg);
void DBI_SetFIFOMode(DBI_FIFO_Mode_Type fifo_mode);
void DBI_YUV2RGB_Init(DBI_YUV2RGB_Cfg_Type *yuv2rgbCfg);
void DBI_Enable(void);
void DBI_Disable(void);
void DBI_SetPeriod(DBI_Period_CFG_Type *period);
void DBI_SetCommand(uint8_t Command);
void DBI_SetPhaseState(int commandEn, int dataEn);
void DBI_SetDataTypeAndCount(DBI_Data_Type dataType, uint32_t dataCount);
void DBI_SetNormalDataDir(DBI_Data_Direction_Type dataDir);
void DBI_ReadNormlData(uint8_t dataSize, uint8_t *dataBuff);
void DBI_WirteData2FIFO(uint32_t wordCount, uint32_t *dataBuff);
uint32_t DBI_GetWordCountOfPixelData(int32_t PixelCount);
void DBI_SendCmdWithNormalData(uint8_t command, uint8_t dataSize, uint8_t *dataBuff);
void DBI_SendCmdAndReadNormalData(uint8_t command, uint8_t dataSize, uint8_t *dataBuff);
void DBI_SendCmdWithPixelData(uint8_t command, int32_t PixelCount, uint32_t *pixelBuff);
void DBI_TxFifoClear(void);
void DBI_SetDMA(int dmaEn);
void DBI_SetTxFifoThreshold(uint8_t threshold);
uint8_t DBI_GetTxFifoCount(void);
int DBI_GetIntStatus(DBI_INT_Type intType);
int DBI_GetOverflowStatus(DBI_Overflow_Type overflow);
int DBI_GetBusBusyStatus(void);
void DBI_IntMask(DBI_INT_Type intType, int intMask);
void DBI_IntClear(void);
void DBI_Int_Callback_Install(DBI_INT_Type intType, intCallback_Type *cbFun);

#endif /* __DRV_DBI_H__ */
