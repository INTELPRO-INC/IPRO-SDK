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
    I2C1_ID = 0, /*!< I2C1 define */
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
    uint8_t len0; /*!< Length of START condition phase 0 */
    uint8_t len1; /*!< Length of START condition phase 1 */
    uint8_t len2; /*!< Length of START condition phase 2 */
    uint8_t len3; /*!< Length of START condition phase 3 */
} I2C_Start_Condition_Phase_Type;

typedef struct
{
    uint8_t len0; /*!< Length of STOP condition phase 0 */
    uint8_t len1; /*!< Length of STOP condition phase 1 */
    uint8_t len2; /*!< Length of STOP condition phase 2 */
    uint8_t len3; /*!< Length of STOP condition phase 3 */
} I2C_Stop_Condition_Phase_Type;

typedef struct
{
    uint8_t len0; /*!< Length of DATA phase 0 */
    uint8_t len1; /*!< Length of DATA phase 1 */
    uint8_t len2; /*!< Length of DATA phase 2 */
    uint8_t len3; /*!< Length of DATA phase 3 */
} I2C_Data_Phase_Type;

typedef struct
{
    uint8_t slaveAddr;         /*!< I2C slave address */
    uint8_t slaveAddr10Bit;         /*!< I2C slave address */
    uint8_t stopEveryByte; /*!< I2C all data byte with stop bit */
    uint8_t subAddrSize;       /*!< Specifies the size of I2C sub address section */
    uint32_t subAddr;          /*!< I2C sub address */
    uint16_t dataSize;         /*!< Specifies the size of I2C data section */
    uint8_t *data;             /*!< Specifies the pointer of I2C R/W data */
    uint32_t clk;              /*!< I2C clk:default is 400k */
} I2C_Transfer_Cfg;

void I2C_SendWord(I2C_ID_Type i2cNo, uint32_t data);
int I2C_RecieveWord(I2C_ID_Type i2cNo, uint32_t *recvBuff);
void I2C_Enable(I2C_ID_Type i2cNo);
void I2C_Disable(I2C_ID_Type i2cNo);
int I2C_SetDeglitchCount(I2C_ID_Type i2cNo, uint8_t cnt);
int I2C_Reset(I2C_ID_Type i2cNo);
void I2C_SetPrd(I2C_ID_Type i2cNo, uint8_t phase);
void I2C_ClockSet(I2C_ID_Type i2cNo, uint32_t clk);
void I2C_SetSclSync(I2C_ID_Type i2cNo, uint8_t enable);
void I2C_Init(I2C_ID_Type i2cNo, I2C_Direction_Type direct, I2C_Transfer_Cfg *cfg);
int I2C_IsBusy(I2C_ID_Type i2cNo);
int I2C_TransferEndStatus(I2C_ID_Type i2cNo);
int I2C_TransferNackStatus(I2C_ID_Type i2cNo);
int I2C_MasterSendBlocking(I2C_ID_Type i2cNo, I2C_Transfer_Cfg *cfg);
int I2C_MasterReceiveBlocking(I2C_ID_Type i2cNo, I2C_Transfer_Cfg *cfg);
void I2C_IntMask(I2C_ID_Type i2cNo, I2C_INT_Type intType, int intMask);

#endif /* __DRV_I2C_H__ */
