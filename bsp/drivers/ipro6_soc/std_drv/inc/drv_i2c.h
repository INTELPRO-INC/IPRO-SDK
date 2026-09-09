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


#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#include "i2c_reg.h"
#include "drv_common.h"

typedef enum {
    I2C0_ID = 0, /*!< I2C0 define */
    I2C1_ID = 1, /*!< I2C1 define */
    I2C_ID_MAX,  /*!< I2C max define */
} I2C_ID_Type;

typedef enum {
    I2C_WRITE = 0, /*!< I2C write direction */
    I2C_READ,      /*!< I2C read direction */
} I2C_Direction_Type;

typedef enum {
    I2C_TRANS_END_INT,     /*!< I2C transfer end interrupt */
    I2C_TX_FIFO_READY_INT, /*!< I2C TX fifo ready interrupt */
    I2C_RX_FIFO_READY_INT, /*!< I2C RX fifo ready interrupt */
    I2C_NACK_RECV_INT,     /*!< I2C nack received interrupt */
    I2C_ARB_LOST_INT,      /*!< I2C arbitration lost interrupt */
    I2C_FIFO_ERR_INT,      /*!< I2C TX/RX FIFO error interrupt */
    I2C_INT_ALL,           /*!< I2C interrupt all type */
} I2C_INT_Type;

typedef struct
{
    uint8_t start_phase0; /*!< Length of start signal phase 0 */
    uint8_t start_phase1; /*!< Length of start signal phase 1 */
    uint8_t start_phase2; /*!< Length of start signal phase 2 */
    uint8_t start_phase3; /*!< Length of start signal phase 3 */
    uint8_t stop_phase0;  /*!< Length of stop signal phase 0 */
    uint8_t stop_phase1;  /*!< Length of stop signal phase 1 */
    uint8_t stop_phase2;  /*!< Length of stop signal phase 2 */
    uint8_t stop_phase3;  /*!< Length of stop signal phase 3 */
    uint8_t data_phase0;  /*!< Length of data signal phase 0 */
    uint8_t data_phase1;  /*!< Length of data signal phase 1，should not be 0 */
    uint8_t data_phase2;  /*!< Length of data signal phase 2 */
    uint8_t data_phase3;  /*!< Length of data signal phase 3 */
} I2C_Timing_Phase_Type;

typedef struct
{
    uint16_t slaveAddr;         /*!< I2C slave address */
    int slaveAddr10Bit; /*!< Enable or disable slave address 10-bit mode */
    int stopEveryByte;  /*!< I2C all data byte with stop bit */
    uint8_t subAddrSize;        /*!< Specifies the size of I2C sub address section */
    uint32_t subAddr;           /*!< I2C sub address */
    uint16_t dataSize;          /*!< Specifies the size of I2C data section */
    uint8_t *data;              /*!< Specifies the pointer of I2C R/W data */
    uint32_t clk;               /*!< I2C clk:default is 400k */
} I2C_Transfer_Cfg;

#define IS_I2C_ID_TYPE(type) (((type) == I2C0_ID) || \
                              ((type) == I2C1_ID) || \
                              ((type) == I2C_ID_MAX))

#define IS_I2C_DIRECTION_TYPE(type) (((type) == I2C_WRITE) || \
                                     ((type) == I2C_READ))

#define IS_I2C_INT_TYPE(type) (((type) == I2C_TRANS_END_INT) ||     \
                               ((type) == I2C_TX_FIFO_READY_INT) || \
                               ((type) == I2C_RX_FIFO_READY_INT) || \
                               ((type) == I2C_NACK_RECV_INT) ||     \
                               ((type) == I2C_ARB_LOST_INT) ||      \
                               ((type) == I2C_FIFO_ERR_INT) ||      \
                               ((type) == I2C_INT_ALL))

#ifndef IPRO_USE_HAL_DRIVER
void I2C0_IRQHandler(void);
void I2C1_IRQHandler(void);
#endif
int I2C_SendWord(I2C_ID_Type i2cNo, uint32_t data);
int I2C_RecieveWord(I2C_ID_Type i2cNo, uint32_t *recvBuff);
void I2C_Enable(I2C_ID_Type i2cNo);
void I2C_Disable(I2C_ID_Type i2cNo);
int I2C_SetDeglitchCount(I2C_ID_Type i2cNo, uint8_t cnt);
int I2C_DeInit(I2C_ID_Type i2cNo);
void I2C_SetPrd(I2C_ID_Type i2cNo, uint8_t phase);
void I2C_SetTimingPhase(I2C_ID_Type i2cNo, I2C_Timing_Phase_Type *timing);
void I2C_GetTimingPhase(I2C_ID_Type i2cNo, I2C_Timing_Phase_Type *timing);
void I2C_ClockSet(I2C_ID_Type i2cNo, uint32_t clk);
void I2C_SetSclSync(I2C_ID_Type i2cNo, uint8_t enable);
void I2C_Init(I2C_ID_Type i2cNo, I2C_Direction_Type direct, I2C_Transfer_Cfg *cfg);
int I2C_IsBusy(I2C_ID_Type i2cNo);
int I2C_TransferEndStatus(I2C_ID_Type i2cNo);
int I2C_TransferNackStatus(I2C_ID_Type i2cNo);
int I2C_MasterSendBlocking(I2C_ID_Type i2cNo, I2C_Transfer_Cfg *cfg);
int I2C_MasterReceiveBlocking(I2C_ID_Type i2cNo, I2C_Transfer_Cfg *cfg);
void I2C_IntMask(I2C_ID_Type i2cNo, I2C_INT_Type intType, int intMask);
void I2C_Int_Callback_Install(I2C_ID_Type i2cNo, I2C_INT_Type intType, intCallback_Type *cbFun);

#endif /* __DRV_I2C_H__ */
