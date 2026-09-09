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


#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

#include "spi_reg.h"
#include "drv_common.h"

#define SPI_FIFO_LEN 32

typedef enum {
    SPI_ID_0,   /*!< SPI0 port define */
    SPI_ID_MAX, /*!< SPI MAX ID define */
} SPI_ID_Type;

typedef enum {
    SPI_BYTE_INVERSE_BYTE0_FIRST, /*!< SPI byte 0 is sent out first */
    SPI_BYTE_INVERSE_BYTE3_FIRST, /*!< SPI byte 3 is sent out first */
} SPI_BYTE_INVERSE_Type;

typedef enum {
    SPI_BIT_INVERSE_MSB_FIRST, /*!< SPI each byte is sent out MSB first */
    SPI_BIT_INVERSE_LSB_FIRST, /*!< SPI each byte is sent out LSB first */
} SPI_BIT_INVERSE_Type;

typedef enum {
    SPI_CLK_PHASE_INVERSE_0, /*!< SPI clock phase inverse 0 */
    SPI_CLK_PHASE_INVERSE_1, /*!< SPI clock phase inverse 1 */
} SPI_CLK_PHASE_INVERSE_Type;

typedef enum {
    SPI_CLK_POLARITY_LOW,  /*!< SPI clock output low at IDLE state */
    SPI_CLK_POLARITY_HIGH, /*!< SPI clock output high at IDLE state */
} SPI_CLK_POLARITY_Type;

typedef enum {
    SPI_FRAME_SIZE_8,  /*!< SPI frame size 8 bit */
    SPI_FRAME_SIZE_16, /*!< SPI frame size 16 bit */
    SPI_FRAME_SIZE_24, /*!< SPI frame size 24 bit */
    SPI_FRAME_SIZE_32, /*!< SPI frame size 32 bit */
} SPI_FrameSize_Type;

typedef enum {
    SPI_WORK_MODE_SLAVE,  /*!< SPI work at slave mode */
    SPI_WORK_MODE_MASTER, /*!< SPI work at master mode */
} SPI_WORK_MODE_Type;

typedef enum {
    SPI_TIMEOUT_DISABLE, /*!< SPI disable timeout judgment */
    SPI_TIMEOUT_ENABLE,  /*!< SPI enable timeout judgment */
} SPI_Timeout_Type;

typedef enum {
    SPI_FIFO_TX_OVERFLOW,  /*!< SPI tx fifo overflow flag */
    SPI_FIFO_TX_UNDERFLOW, /*!< SPI tx fifo underflow flag */
    SPI_FIFO_RX_OVERFLOW,  /*!< SPI rx fifo overflow flag */
    SPI_FIFO_RX_UNDERFLOW, /*!< SPI rx fifo underflow flag */
} SPI_FifoStatus_Type;

typedef enum {
    SPI_INT_END,            /*!< SPI transfer end interrupt,shared by both master and slave mode */
    SPI_INT_TX_FIFO_REQ,    /*!< SPI tx fifo ready interrupt(tx fifo count > tx fifo threshold) */
    SPI_INT_RX_FIFO_REQ,    /*!< SPI rx fifo ready interrupt(rx fifo count > rx fifo threshold) */
    SPI_INT_SLAVE_TIMEOUT,  /*!< SPI slave mode transfer time-out interrupt,triggered when spi bus is idle for the given value */
    SPI_INT_SLAVE_UNDERRUN, /*!< SPI slave mode tx underrun error interrupt,triggered when tx is not ready during transfer */
    SPI_INT_FIFO_ERROR,     /*!< SPI tx/rx fifo error interrupt(overflow/underflow) */
    SPI_INT_ALL,            /*!< All the interrupt */
} SPI_INT_Type;

typedef struct
{
    int deglitchEnable;             /*!< Enable or disable de-glitch function */
    int continuousEnable;           /*!< Enable or disable master continuous transfer mode,enable:SS will stay asserted if next data is valid */
    int ignore_cs;
    SPI_BYTE_INVERSE_Type byteSequence;     /*!< The byte is sent first in SPI transfer */
    SPI_BIT_INVERSE_Type bitSequence;       /*!< The bit is sent first in SPI transfer */
    SPI_CLK_PHASE_INVERSE_Type clkPhaseInv; /*!< Inverse SPI clock phase */
    SPI_CLK_POLARITY_Type clkPolarity;      /*!< SPI clock plarity */
    SPI_FrameSize_Type frameSize;           /*!< SPI frame size(also the valid width for each fifo entry) */
} SPI_CFG_Type;

typedef struct
{
    uint8_t startLen;      /*!< Length of start condition */
    uint8_t stopLen;       /*!< Length of stop condition */
    uint8_t dataPhase0Len; /*!< Length of data phase 0,affecting clock */
    uint8_t dataPhase1Len; /*!< Length of data phase 1,affecting clock */
    uint8_t intervalLen;   /*!< Length of interval between frame */
} SPI_ClockCfg_Type;

typedef struct
{
    uint8_t txFifoThreshold;     /*!< SPI tx FIFO threshold */
    uint8_t rxFifoThreshold;     /*!< SPI rx FIFO threshold */
    int txFifoDmaEnable; /*!< Enable or disable tx dma req/ack interface */
    int rxFifoDmaEnable; /*!< Enable or disable rx dma req/ack interface */
} SPI_FifoCfg_Type;

#define SPI_RX_FIFO_SIZE 32
#define SPI_TX_FIFO_SIZE 32

int SPI_Init(SPI_ID_Type spiNo, SPI_CFG_Type *spiCfg);
int SPI_DeInit(SPI_ID_Type spiNo);
int SPI_SetClock(SPI_ID_Type spiNo, uint32_t clk);
int SPI_ClockConfig(SPI_ID_Type spiNo, SPI_ClockCfg_Type *clockCfg);
int SPI_FifoConfig(SPI_ID_Type spiNo, SPI_FifoCfg_Type *fifoCfg);
int SPI_Enable(SPI_ID_Type spiNo, SPI_WORK_MODE_Type modeType);
int SPI_Disable(SPI_ID_Type spiNo, SPI_WORK_MODE_Type modeType);
int SPI_SetFrameSize(SPI_ID_Type spiNo, uint8_t frame_size);
int SPI_DmaTxEnable(SPI_ID_Type spiNo);
int SPI_DmaTxDisable(SPI_ID_Type spiNo);
int SPI_DmaRxEnable(SPI_ID_Type spiNo);
int SPI_DmaRxDisable(SPI_ID_Type spiNo);
int SPI_SetTimeOutValue(SPI_ID_Type spiNo, uint16_t value);
int SPI_SetDeglitchCount(SPI_ID_Type spiNo, uint8_t cnt);
int SPI_RxIgnoreEnable(SPI_ID_Type spiNo, uint8_t startPoint, uint8_t stopPoint);
int SPI_RxIgnoreDisable(SPI_ID_Type spiNo);
int SPI_ClrTxFifo(SPI_ID_Type spiNo);
int SPI_ClrRxFifo(SPI_ID_Type spiNo);
int SPI_ClrIntStatus(SPI_ID_Type spiNo, SPI_INT_Type intType);
int SPI_IntMask(SPI_ID_Type spiNo, SPI_INT_Type intType, int intMask);
int SPI_SendData(SPI_ID_Type spiNo, void *buff, uint32_t length, SPI_Timeout_Type timeoutType);
int SPI_ReceiveData(SPI_ID_Type spiNo, void *recvBuff, uint32_t length, SPI_Timeout_Type timeoutType);
int SPI_SendRecvData(SPI_ID_Type spiNo, void *sendBuff, void *recvBuff, uint32_t length, SPI_Timeout_Type timeoutType);
uint8_t SPI_GetTxFifoCount(SPI_ID_Type spiNo);
uint8_t SPI_GetRxFifoCount(SPI_ID_Type spiNo);
int SPI_GetIntStatus(SPI_ID_Type spiNo, SPI_INT_Type intType);
int SPI_GetFifoStatus(SPI_ID_Type spiNo, SPI_FifoStatus_Type fifoSts);
int SPI_GetBusyStatus(SPI_ID_Type spiNo);
int SPI_ClearInt(SPI_ID_Type spiNo, SPI_INT_Type intType);

#endif /* __DRV_SPI_H__ */
