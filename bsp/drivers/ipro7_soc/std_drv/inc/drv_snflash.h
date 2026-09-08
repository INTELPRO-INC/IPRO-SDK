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

#ifndef __DRV_SNFLASH_H__
#define __DRV_SNFLASH_H__

#include "drv_aon.h"
#include "drv_common.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_spi.h"
#include "drv_gpio.h"

#include "ffconf.h"
#include "ff.h"
#include "diskio.h"

#define FLASH_WRITE_READ_SIZE (2560)
#define READ_PRINT_OPEN       (0)

#define SPI_PIN_CLK  GLB_GPIO_PIN_11
#define SPI_PIN_MOSI GLB_GPIO_PIN_12
#define SPI_PIN_MISO GLB_GPIO_PIN_21
#define SPI_PIN_CS   GLB_GPIO_PIN_10

#define NANDFLASH_WRITE_ENABLE             (0x06)
#define NANDFLASH_WRITE_DISABLE            (0x04)
#define NANDFLASH_GET_FEATURES             (0x0F)
#define NANDFLASH_SET_FEATURES             (0x1F)
#define NANDFLASH_PAGE_READ_TO_CACHE       (0x13)
#define NANDFLASH_READ_FROM_CACHE          (0x03)
#define NANDFLASH_READ_ID                  (0x9F)
#define NANDFLASH_PROGRAM_LOAD             (0x02)
#define NANDFLASH_PROGRAM_LOAD_RANDOM_DATA (0x84)
#define NANDFLASH_PROGRAM_EXECUTE          (0x10)
#define NANDFLASH_BLOCK_ERASE              (0xD8)
#define NANDFLASH_RESET                    (0xFF)

#define NANDFLASH_BLOCK_LOCK_ADDR (0xA0)
#define NANDFLASH_FEATURE_ADDR    (0xB0)
#define NANDFLASH_STATUS_ADDR     (0xC0)
#define NANDFLASH_DUMMY_BYTE      (0x00)
#define NANDFLASH_PAGE_SIZE       (2048)
#define NANDFLASH_PAGE_TOTAL_SIZE (2048 + 64)
#define NANDFLASH_BLOCK_SIZE      (128 * 1024)
#define NANDFLASH_PAGES_PER_BLOCK (64)

typedef struct
{
    SPI_ID_Type spiNo;
    SPI_CFG_Type spiCfg;
    SPI_ClockCfg_Type clockCfg;
} NANDFLASH_CFG_Type;

extern NANDFLASH_CFG_Type nfCfg;

typedef struct
{
    uint8_t Reserved0 : 1;
    uint8_t CMP       : 1;
    uint8_t INV       : 1;
    uint8_t BP0       : 1;
    uint8_t BP1       : 1;
    uint8_t BP2       : 1;
    uint8_t Reserved6 : 1;
    uint8_t BRWD      : 1;
} NANDFLASH_BLOCK_LOCK_Type;

typedef struct
{
    uint8_t QE        : 1;
    uint8_t Reserved1 : 1;
    uint8_t Reserved2 : 1;
    uint8_t Reserved3 : 1;
    uint8_t ECC_EN    : 1;
    uint8_t Reserved5 : 1;
    uint8_t OTP_EN    : 1;
    uint8_t OTP_PRT   : 1;
} NANDFLASH_FEATURE_Type;

typedef struct
{
    uint8_t OIP         : 1;
    uint8_t WEL         : 1;
    uint8_t EFAIL_ECCS0 : 1;
    uint8_t PFAIL_ECCS1 : 1;
    uint8_t ECCS2       : 1;
    uint8_t ECCS3       : 1;
    uint8_t Reserved6   : 1;
    uint8_t Reserved7   : 1;
} NANDFLASH_STATUS_Type;

typedef enum {
    NO_BIT_ERROR = 0,
    ONE_BIT_ERROR_DETECTED_AND_CORRECTED = 1,
    TWO_BIT_ERROR_DETECTED_AND_CORRECTED = 2,
    THREE_BIT_ERROR_DETECTED_AND_CORRECTED = 3,
    FOUR_BIT_ERROR_DETECTED_AND_CORRECTED = 4,
    FIVE_BIT_ERROR_DETECTED_AND_CORRECTED = 5,
    SIX_BIT_ERROR_DETECTED_AND_CORRECTED = 6,
    SEVEN_BIT_ERROR_DETECTED_AND_CORRECTED = 7,
    BIT_ERROR_DETECTED_AND_NOT_CORRECTED = 8,
    EIGHT_BIT_ERROR_DETECTED_AND_CORRECTED_MAX = 9
} NANDFLASH_ECC_Type;

void ATTR_TCM_SECTION Nandflash_Psram_Cfg(void);
void Nandflash_Init(void);

void Nandflash_Gpio_Init(int clk, int mosi, int miso);
void Nandflash_CS_High(void);
void Nandflash_CS_Low(void);
void Nandflash_Clock_Init(uint8_t enable, uint8_t div);
void Nandflash_SPI_Init(NANDFLASH_CFG_Type *pNandflashCfg);

void Nandflash_WriteEnable(NANDFLASH_CFG_Type *pNandflashCfg);
void Nandflash_WriteDisable(NANDFLASH_CFG_Type *pNandflashCfg);
void Nandflash_Get_BlockLock(NANDFLASH_CFG_Type *pNandflashCfg, uint8_t *pBlockLock);
void Nandflash_Set_BlockLock(NANDFLASH_CFG_Type *pNandflashCfg, uint8_t *pBlockLock);
void Nandflash_Get_Feature(NANDFLASH_CFG_Type *pNandflashCfg, uint8_t *pFeature);
void Nandflash_Set_Feature(NANDFLASH_CFG_Type *pNandflashCfg, uint8_t *pFeature);
void Nandflash_Get_Status(NANDFLASH_CFG_Type *pNandflashCfg, uint8_t *pStatus);
void Nandflash_Set_Status(NANDFLASH_CFG_Type *pNandflashCfg, uint8_t *pStatus);

NANDFLASH_ECC_Type Nandflash_CheckBadBlock(NANDFLASH_CFG_Type *pNandflashCfg, uint32_t blockNum, uint8_t *buff, uint32_t len);

NANDFLASH_ECC_Type Nandflash_PagesRead(NANDFLASH_CFG_Type *pNandflashCfg, uint32_t addr, uint8_t *buff, uint32_t len);
NANDFLASH_ECC_Type Nandflash_PageRead(NANDFLASH_CFG_Type *pNandflashCfg, uint32_t addr, uint8_t *buff, uint32_t len);
NANDFLASH_ECC_Type Nandflash_PageReadToCache(NANDFLASH_CFG_Type *pNandflashCfg, uint32_t addr);
void Nandflash_ReadFromCache(NANDFLASH_CFG_Type *pNandflashCfg, uint16_t offset, uint8_t *buff, uint32_t len);

void Nandflash_ReadID(NANDFLASH_CFG_Type *pNandflashCfg, uint8_t *pManufactureID, uint8_t *pDeviceID);
void Nandflash_PagesProgram(NANDFLASH_CFG_Type *pNandflashCfg, uint32_t addr, const uint8_t *buff, uint32_t len);
void Nandflash_PageProgram(NANDFLASH_CFG_Type *pNandflashCfg, uint32_t addr, const uint8_t *buff, uint32_t len);
void Nandflash_ProgramLoad(NANDFLASH_CFG_Type *pNandflashCfg, uint32_t pageOffset, const uint8_t *buff, uint32_t len);
void Nandflash_ProgramExecute(NANDFLASH_CFG_Type *pNandflashCfg, uint32_t addr);
void Nandflash_InternalDataMove(NANDFLASH_CFG_Type *pNandflashCfg);
void Nandflash_ProgramLoadRandomData(NANDFLASH_CFG_Type *pNandflashCfg, uint32_t addr, uint8_t *buff, uint32_t len);
void Nandflash_BlockErase(NANDFLASH_CFG_Type *pNandflashCfg, uint32_t addr);
void Nandflash_Reset(NANDFLASH_CFG_Type *pNandflashCfg);

void Nandflash_WriteUnprotect(NANDFLASH_CFG_Type *pNandflashCfg);
NANDFLASH_ECC_Type Nandflash_Check_ECC_Status(NANDFLASH_CFG_Type *pNandflashCfg);

int Nandflash_Disk_Status(void);
int Nandflash_Disk_Initialize(void);
int Nandflash_Disk_Read(BYTE *buff, LBA_t sector, UINT count);
int Nandflash_Disk_Write(const BYTE *buff, LBA_t sector, UINT count);
int Nandflash_Disk_Ioctl(BYTE cmd, void *buff);
DSTATUS Nandflash_Translate_Result_Code(int result);

#endif //__DRV_SNFLASH_H__
