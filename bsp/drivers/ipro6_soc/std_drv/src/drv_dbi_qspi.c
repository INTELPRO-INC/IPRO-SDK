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



#include "drv_dbi_qspi.h"

int DBI_QSPI_Init(DBI_QSPI_CFG_Type *dbiCfg)
{
    uint32_t tmpVal;

    /* Check the parameters */

    /* Set dbi config */
    tmpVal = readl(DBI_BASE + DBI_CONFIG_OFFSET);
    /* Select DBI type QSPI */
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_SEL, DBI_TYPE_QSPI);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_SCL_PH, dbiCfg->clkPhase);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_SCL_POL, dbiCfg->clkPolarity);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_CS_STRETCH, dbiCfg->CSStretchMode);
    tmpVal = reg_clr_bit(tmpVal, DBI_CR_DBI_EN);
    writel(tmpVal, DBI_BASE + DBI_CONFIG_OFFSET);

    /* Set qspi config */
    tmpVal = readl(DBI_BASE + DBI_QSPI_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_QSPI_ADR_BC, dbiCfg->qspiAddressSize-1);
    writel(tmpVal, DBI_BASE + DBI_QSPI_CONFIG_OFFSET);

    /* Set qspi start Command */
    writel(dbiCfg->startCommand, DBI_BASE + DBI_CMD_OFFSET);

    /* Set data period */
    DBI_SetPeriod(&(dbiCfg->period));

    /* Set pixel format */
    tmpVal = readl(DBI_BASE + DBI_PIX_CNT_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_PIX_FORMAT, dbiCfg->pixelFormat);
    writel(tmpVal, DBI_BASE + DBI_PIX_CNT_OFFSET);

    /* Set fifo format */
    tmpVal = readl(DBI_BASE + DBI_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_FIFO_FORMAT, dbiCfg->fifoFormat);
    writel(tmpVal, DBI_BASE + DBI_FIFO_CONFIG_0_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(DBI_IRQn, DBI_IRQHandler);
#endif

    return SUCCESS;
}

void DBI_QSPI_SetAddress(uint32_t address)
{
    /* Set dbi qspi Address */
    writel(address, DBI_BASE + DBI_QSPI_ADR_OFFSET);
}

void DBI_QSPI_SetAddressSize(uint8_t addressSize )
{
    uint32_t tmpVal;

    /* Set qspi address size */
    tmpVal = readl(DBI_BASE + DBI_QSPI_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_QSPI_ADR_BC, addressSize-1);
    writel(tmpVal, DBI_BASE + DBI_QSPI_CONFIG_OFFSET);
}

void DBI_QSPI_SetWire(DBI_QSPI_WIRE_CFG_Type *qspiWire )
{
    uint32_t tmpVal;

    /* Set qspi config */
    tmpVal = readl(DBI_BASE + DBI_QSPI_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_QSPI_DAT_4B, qspiWire->dataWire);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_QSPI_ADR_4B, qspiWire->addressWire);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_QSPI_CMD_4B, qspiWire->commandWire);
    writel(tmpVal, DBI_BASE + DBI_QSPI_CONFIG_OFFSET);
}

void DBI_QSPI_SendCAWithNormalData(uint8_t command, uint32_t address, uint8_t dataSize, uint8_t *dataBuff)
{
    uint8_t wordsCount,n;

    /* disable transfer */
    DBI_Disable();

    /* clear fifo and interrupt */
    DBI_TxFifoClear();
    DBI_IntClear();

    /* set command */
    DBI_SetCommand(command);

    /* set address */
    DBI_QSPI_SetAddress(address);

    /* set data direction */
    DBI_SetNormalDataDir(DBI_DATA_WRITE);

    /* Data type select, and set data count */
    DBI_SetDataTypeAndCount(DBI_DATA_NORMAL, dataSize);

    wordsCount = (dataSize + 3) >> 2;
    /* Pre-populate data */
    n = (wordsCount < DBI_TX_FIFO_SIZE) ? wordsCount : DBI_TX_FIFO_SIZE;
    DBI_WirteData2FIFO(n, (uint32_t *)dataBuff);

    /* enable transfer */
    DBI_Enable();

    /* Populate remaining data */
    DBI_WirteData2FIFO(wordsCount-n, ((uint32_t *)dataBuff)+n);

    /* Wait transfer end */
    while (DBI_GetIntStatus(DBI_INT_END) != SET) {
    }

    /* clear interrupt */
    DBI_IntClear();
}

void DBI_QSPI_SendCAAndReadNormalData(uint8_t command, uint32_t address, uint8_t dataSize, uint8_t *dataBuff)
{
    /* disable transfer */
    DBI_Disable();

    /* clear interrupt */
    DBI_IntClear();

    /* set command */
    DBI_SetCommand(command);

    /* set address */
    DBI_QSPI_SetAddress(address);

    /* set data direction */
    DBI_SetNormalDataDir(DBI_DATA_READ);

    /* Data type select, and set data count */
    DBI_SetDataTypeAndCount(DBI_DATA_NORMAL, dataSize);

    /* enable transfer */
    DBI_Enable();

    /* Wait transfer end */
    while (DBI_GetIntStatus(DBI_INT_END) != SET) {
    }

    /* get data */
    DBI_ReadNormlData(dataSize, dataBuff);

    /* clear interrupt */
    DBI_IntClear();
}

void DBI_QSPI_SendCAWithPixelData(uint8_t command, uint32_t address, int32_t PixelCount, uint32_t *pixelBuff)
{
    /* disable transfer */
    DBI_Disable();

    /* clear fifo and interrupt */
    DBI_TxFifoClear();
    DBI_IntClear();

    /* set command */
    DBI_SetCommand(command);

    /* set address */
    DBI_QSPI_SetAddress(address);

    /* set data direction */
    DBI_SetNormalDataDir(DBI_DATA_WRITE);

    /* Data type select, and set data count */
    DBI_SetDataTypeAndCount(DBI_DATA_PIXEL, PixelCount);

    /* enable transfer */
    DBI_Enable();

    /* get words count */
    uint32_t wordsCount = DBI_GetWordCountOfPixelData(PixelCount);
    /* send data */
    DBI_WirteData2FIFO(wordsCount, pixelBuff);

    /* Wait transfer end */
    while (DBI_GetIntStatus(DBI_INT_END) != SET) {
    }

    /* clear interrupt */
    DBI_IntClear();

}

