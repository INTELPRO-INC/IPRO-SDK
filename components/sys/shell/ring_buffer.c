/**
 * @file ring_buffer.c
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
#include "ring_buffer.h"

void Ring_Buffer_Init(Ring_Buffer_Type *rbType, uint8_t *buffer, uint32_t size,
                      ringBuffer_Lock_Callback *lockCb, ringBuffer_Lock_Callback *unlockCb)
{
    rbType->pointer = buffer;
    rbType->readMirror = 0;
    rbType->readIndex = 0;
    rbType->writeMirror = 0;
    rbType->writeIndex = 0;
    rbType->size = size;
    rbType->lock = lockCb;
    rbType->unlock = unlockCb;
}

void Ring_Buffer_Reset(Ring_Buffer_Type *rbType)
{
    if (rbType->lock != NULL) {
        rbType->lock();
    }

    rbType->readMirror = 0;
    rbType->readIndex = 0;
    rbType->writeMirror = 0;
    rbType->writeIndex = 0;

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }
}

uint32_t Ring_Buffer_Write(Ring_Buffer_Type *rbType, const uint8_t *data, uint32_t length)
{
    uint32_t sizeRemained = Ring_Buffer_Get_Empty_Length(rbType);
    uint32_t indexRemained;

    if (rbType->lock != NULL) {
        rbType->lock();
    }

    if (sizeRemained == 0) {
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }
        return 0;
    }

    if (length > sizeRemained) {
        length = sizeRemained;
    }

    indexRemained = rbType->size - rbType->writeIndex;

    if (indexRemained > length) {
        memcpy(&rbType->pointer[rbType->writeIndex], data, length);
        rbType->writeIndex += length;
    } else {
        memcpy(&rbType->pointer[rbType->writeIndex], data, indexRemained);
        memcpy(&rbType->pointer[0], &data[indexRemained], length - indexRemained);
        rbType->writeIndex = length - indexRemained;
        rbType->writeMirror = ~rbType->writeMirror;
    }

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return length;
}

uint32_t Ring_Buffer_Write_Byte(Ring_Buffer_Type *rbType, const uint8_t data)
{
    if (rbType->lock != NULL) {
        rbType->lock();
    }

    if (!Ring_Buffer_Get_Empty_Length(rbType)) {
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }
        return 0;
    }

    rbType->pointer[rbType->writeIndex] = data;

    if (rbType->writeIndex != (rbType->size - 1)) {
        rbType->writeIndex++;
    } else {
        rbType->writeIndex = 0;
        rbType->writeMirror = ~rbType->writeMirror;
    }

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return 1;
}

uint32_t Ring_Buffer_Read(Ring_Buffer_Type *rbType, uint8_t *data, uint32_t length)
{
    uint32_t size = Ring_Buffer_Get_Length(rbType);
    uint32_t indexRemained;

    if (rbType->lock != NULL) {
        rbType->lock();
    }

    if (!size) {
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }
        return 0;
    }

    if (size < length) {
        length = size;
    }

    indexRemained = rbType->size - rbType->readIndex;

    if (indexRemained > length) {
        memcpy(data, &rbType->pointer[rbType->readIndex], length);
        rbType->readIndex += length;
    } else {
        memcpy(data, &rbType->pointer[rbType->readIndex], indexRemained);
        memcpy(&data[indexRemained], &rbType->pointer[0], length - indexRemained);
        rbType->readIndex = length - indexRemained;
        rbType->readMirror = ~rbType->readMirror;
    }

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return length;
}

uint32_t Ring_Buffer_Read_Byte(Ring_Buffer_Type *rbType, uint8_t *data)
{
    if (rbType->lock != NULL) {
        rbType->lock();
    }

    if (!Ring_Buffer_Get_Length(rbType)) {
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }
        return 0;
    }

    *data = rbType->pointer[rbType->readIndex];

    if (rbType->readIndex == rbType->size - 1) {
        rbType->readIndex = 0;
        rbType->readMirror = ~rbType->readMirror;
    } else {
        rbType->readIndex++;
    }

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return 1;
}

uint32_t Ring_Buffer_Get_Length(Ring_Buffer_Type *rbType)
{
    uint32_t readMirror = rbType->readMirror;
    uint32_t writeMirror = rbType->writeMirror;
    uint32_t readIndex = rbType->readIndex;
    uint32_t writeIndex = rbType->writeIndex;
    uint32_t size = rbType->size;

    if (readMirror == writeMirror) {
        return writeIndex - readIndex;
    } else {
        return size - (readIndex - writeIndex);
    }
}

uint32_t Ring_Buffer_Get_Empty_Length(Ring_Buffer_Type *rbType)
{
    return (rbType->size - Ring_Buffer_Get_Length(rbType));
}

Ring_Buffer_Status_Type Ring_Buffer_Get_Status(Ring_Buffer_Type *rbType)
{
    if (rbType->readIndex == rbType->writeIndex) {
        if (rbType->readMirror == rbType->writeMirror) {
            return RING_BUFFER_EMPTY;
        } else {
            return RING_BUFFER_FULL;
        }
    }
    return RING_BUFFER_PARTIAL;
}
