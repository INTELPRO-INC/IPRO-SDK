/**
 * @file ring_buffer.h
 * @brief Ring buffer implementation
 *
 * Copyright (c) 2021 IntelPro team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>
#include <string.h>

/**
 *  @brief Ring buffer status type definition
 */
typedef enum {
    RING_BUFFER_EMPTY,   /*!< Ring buffer is empty */
    RING_BUFFER_PARTIAL, /*!< Ring buffer has partial data */
    RING_BUFFER_FULL,    /*!< Ring buffer is full */
} Ring_Buffer_Status_Type;

/**
 *  @brief Ring buffer structure definition
 */
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

typedef void(ringBuffer_Lock_Callback)(void);
typedef void(ringBuffer_Read_Callback)(void *, uint8_t *, uint32_t);
typedef void(ringBuffer_Write_Callback)(void *, uint8_t *, uint32_t);

void Ring_Buffer_Init(Ring_Buffer_Type *rbType, uint8_t *buffer, uint32_t size,
                      ringBuffer_Lock_Callback *lockCb, ringBuffer_Lock_Callback *unlockCb);
void Ring_Buffer_Reset(Ring_Buffer_Type *rbType);
uint32_t Ring_Buffer_Write(Ring_Buffer_Type *rbType, const uint8_t *data, uint32_t length);
uint32_t Ring_Buffer_Write_Byte(Ring_Buffer_Type *rbType, const uint8_t data);
uint32_t Ring_Buffer_Read(Ring_Buffer_Type *rbType, uint8_t *data, uint32_t length);
uint32_t Ring_Buffer_Read_Byte(Ring_Buffer_Type *rbType, uint8_t *data);
uint32_t Ring_Buffer_Get_Length(Ring_Buffer_Type *rbType);
uint32_t Ring_Buffer_Get_Empty_Length(Ring_Buffer_Type *rbType);
Ring_Buffer_Status_Type Ring_Buffer_Get_Status(Ring_Buffer_Type *rbType);

#endif /* __RING_BUFFER_H__ */
