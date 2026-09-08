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

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include "misc.h"

typedef enum {
    RING_BUFFER_EMPTY,   /*!< Ring buffer is empty */
    RING_BUFFER_PARTIAL, /*!< Ring buffer has partial data */
    RING_BUFFER_FULL,    /*!< Ring buffer is full */
} Ring_Buffer_Status_Type;

typedef struct
{
    uint8_t *pointer;     /*!< Pointer of ring buffer */
    uint8_t readMirror;   /*!< Read mirror,used to judge empty or full */
    uint32_t readIndex;   /*!< Index of read address */
    uint8_t writeMirror;  /*!< Write mirror,used to judge empty or full */
    uint32_t writeIndex;  /*!< Index of write address */
    uint32_t size;        /*!< Size of ring buffer */
    void (*lock)(void);   /*!< Lock ring buffer */
    void (*unlock)(void); /*!< Unlock ring buffer */
} Ring_Buffer_Type;

#define IS_RING_BUFFER_STATUS_TYPE(type) (((type) == RING_BUFFER_EMPTY) ||   \
                                          ((type) == RING_BUFFER_PARTIAL) || \
                                          ((type) == RING_BUFFER_FULL))

typedef void(ringBuffer_Lock_Callback)(void);
typedef void(ringBuffer_Read_Callback)(void *, uint8_t *, uint32_t);
typedef void(ringBuffer_Write_Callback)(void *, uint8_t *, uint32_t);

int Ring_Buffer_Init(Ring_Buffer_Type *rbType, uint8_t *buffer, uint32_t size, ringBuffer_Lock_Callback *lockCb,
                             ringBuffer_Lock_Callback *unlockCb);
int Ring_Buffer_Reset(Ring_Buffer_Type *rbType);
uint32_t Ring_Buffer_Write_Callback(Ring_Buffer_Type *rbType, uint32_t length, ringBuffer_Write_Callback *writeCb,
                                    void *parameter);
uint32_t Ring_Buffer_Write(Ring_Buffer_Type *rbType, const uint8_t *data, uint32_t length);
uint32_t Ring_Buffer_Write_Byte(Ring_Buffer_Type *rbType, const uint8_t data);
uint32_t Ring_Buffer_Write_Force(Ring_Buffer_Type *rbType, const uint8_t *data, uint32_t length);
uint32_t Ring_Buffer_Write_Byte_Force(Ring_Buffer_Type *rbType, const uint8_t data);
uint32_t Ring_Buffer_Read_Callback(Ring_Buffer_Type *rbType, uint32_t length, ringBuffer_Read_Callback *readCb,
                                   void *parameter);
uint32_t Ring_Buffer_Read(Ring_Buffer_Type *rbType, uint8_t *data, uint32_t length);
uint32_t Ring_Buffer_Read_Byte(Ring_Buffer_Type *rbType, uint8_t *data);
uint32_t Ring_Buffer_Peek(Ring_Buffer_Type *rbType, uint8_t *data, uint32_t length);
uint32_t Ring_Buffer_Peek_Byte(Ring_Buffer_Type *rbType, uint8_t *data);
uint32_t Ring_Buffer_Get_Length(Ring_Buffer_Type *rbType);
uint32_t Ring_Buffer_Get_Empty_Length(Ring_Buffer_Type *rbType);
Ring_Buffer_Status_Type Ring_Buffer_Get_Status(Ring_Buffer_Type *rbType);

#endif /* __RING_BUFFER_H__ */
