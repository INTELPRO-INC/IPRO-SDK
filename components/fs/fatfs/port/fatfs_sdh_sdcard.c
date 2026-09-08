/**
 * @file sdh_sdcard.c
 * @brief
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
 *
 */

#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "hal_sdh.h"
#include "compiler/common.h"

static SD_ID_Type sdId = SDH_ID;

static sd_card_t ATTR_PSRAM_SECTION gSDCardInfo;
static uint32_t sdio_bus_width = SDH_DATA_BUS_WIDTH_1BIT;
#ifdef SDH_USE_SEMPHR
static SemaphoreHandle_t gSDCtrlLock = NULL;
#endif

void hal_sd_bus_8bits_enable(void)
{
    sdio_bus_width = SDH_DATA_BUS_WIDTH_8BITS;
}

void hal_sd_bus_4bits_enable(void)
{
    sdio_bus_width = SDH_DATA_BUS_WIDTH_4BITS;
}

int SD_disk_status(void)
{
    return 0;
}

int SD_disk_initialize(void)
{
    static uint8_t inited = 0;

    if (inited == 0) {
        if (SDH_Init(sdio_bus_width, &gSDCardInfo, sdId) == SD_OK) {
#ifdef SDH_USE_SEMPHR
            gSDCtrlLock = xSemaphoreCreateMutex();
#endif
            inited = 1;
            return 0;
        } else {
            return -1;
        }
    }
    return 0;
}

int SD_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
#ifdef SDH_USE_SEMPHR
    xSemaphoreTake(gSDCtrlLock, 0xffffffff);
#endif
    if (Status_Success == SDH_ReadMultiBlocks(buff, sector, gSDCardInfo.blockSize, count, sdId)) {
#ifdef SDH_USE_SEMPHR
        xSemaphoreGive(gSDCtrlLock);
#endif
        return 0;
    } else {
#ifdef SDH_USE_SEMPHR
        xSemaphoreGive(gSDCtrlLock);
#endif
        return -1;
    }
}

int SD_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    status_t ret = Status_Success;
#ifdef SDH_USE_SEMPHR
    xSemaphoreTake(gSDCtrlLock, 0xffffffff);
#endif
_retry:
#ifndef BOOTROM
    ret = SDH_WriteMultiBlocks(buff, sector, gSDCardInfo.blockSize, count, sdId);
#endif
    if (ret == Status_Success) {
#ifdef SDH_USE_SEMPHR
        xSemaphoreGive(gSDCtrlLock);
#endif
        return 0;
    } else if (ret == Status_Timeout) {
        goto _retry;
    } else {
#ifdef SDH_USE_SEMPHR
        xSemaphoreGive(gSDCtrlLock);
#endif
        return -1;
    }

}

int SD_disk_ioctl(BYTE cmd, void *buff)
{
    switch (cmd) {
        // Get R/W sector size (WORD)
        case GET_SECTOR_SIZE:
            *(WORD *)buff = gSDCardInfo.blockSize;
            break;

        // Get erase block size in unit of sector (DWORD)
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = gSDCardInfo.blockCount;
            break;

        case CTRL_SYNC:
            break;
        default:
            break;
    }

    return 0;
}

DSTATUS Translate_Result_Code(int result)
{
    return result;
}

void fatfs_sdh_driver_register(void)
{
    FATFS_DiskioDriverTypeDef SDH_DiskioDriver = { NULL };

    SDH_DiskioDriver.disk_status = SD_disk_status;
    SDH_DiskioDriver.disk_initialize = SD_disk_initialize;
    SDH_DiskioDriver.disk_write = SD_disk_write;
    SDH_DiskioDriver.disk_read = SD_disk_read;
    SDH_DiskioDriver.disk_ioctl = SD_disk_ioctl;
    SDH_DiskioDriver.error_code_parsing = Translate_Result_Code;

    disk_driver_callback_init(DEV_SD, &SDH_DiskioDriver);
}
