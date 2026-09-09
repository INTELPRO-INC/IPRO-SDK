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


#ifndef __DRV_DBI_QSPI_H__
#define __DRV_DBI_QSPI_H__

#include "drv_dbi.h"

typedef enum
{
    DBI_QSPI_WIRE_1 = 0,
    DBI_QSPI_WIRE_4 = 1,
} DBI_QSPI_WIRE_Type;

typedef struct
{
    DBI_QSPI_WIRE_Type dataWire;
    DBI_QSPI_WIRE_Type addressWire;
    DBI_QSPI_WIRE_Type commandWire;
} DBI_QSPI_WIRE_CFG_Type;

typedef struct
{
    DBI_Pixel_Format_Type pixelFormat;     /*!< DBI pixel format */
    DBI_FIFO_Format_Type fifoFormat;       /*!< DBI fifo format */
    DBI_SCL_CLK_Phase_Type clkPhase;       /*!< DBI clock phase */
    DBI_SCL_CLK_Polarity_Type clkPolarity; /*!< DBI clock polarity */
    DBI_Period_CFG_Type period;            /*!< Period configuration */
    uint8_t startCommand;                  /*!< start command, first byte */
    uint8_t qspiAddressSize;               /*!< address size, 1~4 */
    DBI_CS_StretchMode_Type CSStretchMode; /*!< Set signal of CS-low stretch mode */
} DBI_QSPI_CFG_Type;

int DBI_QSPI_Init(DBI_QSPI_CFG_Type *dbiCfg);
void DBI_QSPI_SetAddress(uint32_t address);
void DBI_QSPI_SetAddressSize(uint8_t addressSize);
void DBI_QSPI_SetWire(DBI_QSPI_WIRE_CFG_Type *qspiWire);

void DBI_QSPI_SendCAWithNormalData(uint8_t command, uint32_t address, uint8_t dataSize, uint8_t *dataBuff);
void DBI_QSPI_SendCAAndReadNormalData(uint8_t command, uint32_t address, uint8_t dataSize, uint8_t *dataBuff);
void DBI_QSPI_SendCAWithPixelData(uint8_t command, uint32_t address, int32_t PixelCount, uint32_t *pixelBuff);

#endif /* __DRV_DBI_H__ */
