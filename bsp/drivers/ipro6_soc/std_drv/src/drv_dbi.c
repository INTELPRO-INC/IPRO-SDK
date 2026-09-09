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



#include "drv_dbi.h"

#ifndef IPRO_USE_HAL_DRIVER
static intCallback_Type *DBIIntCbfArra[DBI_INT_ALL] = {
    NULL
};
#endif

#ifndef IPRO_USE_HAL_DRIVER
void DBI_IRQHandler(void)
{
    uint32_t tmpVal;

    tmpVal = readl(DBI_BASE + DBI_INT_STS_OFFSET);

    /* DBI transfer end interrupt */
    if (reg_is_bit_set(tmpVal, DBI_END_INT) && !reg_is_bit_set(tmpVal, DBI_CR_DBI_END_MASK)) {
        writel(reg_set_bit(tmpVal, DBI_CR_DBI_END_CLR), DBI_BASE + DBI_INT_STS_OFFSET);

        if (DBIIntCbfArra[DBI_INT_END] != NULL) {
            DBIIntCbfArra[DBI_INT_END]();
        }
    }

    /* DBI tx fifo ready interrupt */
    if (reg_is_bit_set(tmpVal, DBI_TXF_INT) && !reg_is_bit_set(tmpVal, DBI_CR_DBI_TXF_MASK)) {
        if (DBIIntCbfArra[DBI_INT_TX_FIFO_REQ] != NULL) {
            DBIIntCbfArra[DBI_INT_TX_FIFO_REQ]();
        }
    }

    /* DBI tx fifo ready interrupt */
    if (reg_is_bit_set(tmpVal, DBI_FER_INT) && !reg_is_bit_set(tmpVal, DBI_CR_DBI_FER_MASK)) {
        if (DBIIntCbfArra[DBI_INT_FIFO_ERR] != NULL) {
            DBIIntCbfArra[DBI_INT_FIFO_ERR]();
        }
    }
}
#endif

int DBI_Init(DBI_CFG_Type *dbiCfg)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(DBI_BASE + DBI_CONFIG_OFFSET);

    /* Select DBI type  */
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_SEL, dbiCfg->mode);

    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_CS_STRETCH, dbiCfg->CSStretchMode);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_CONT_EN, dbiCfg->continueEn);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_DMY_EN, dbiCfg->dummyEn);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_DMY_CNT, dbiCfg->dummyCnt);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_SCL_PH, dbiCfg->clkPhase);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_SCL_POL, dbiCfg->clkPolarity);

    writel(tmpVal, DBI_BASE + DBI_CONFIG_OFFSET);

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

void DBI_SetFIFOMode(DBI_FIFO_Mode_Type fifo_mode)
{
    uint32_t tmpVal;

    /* Set FIFO format */
    tmpVal = readl(DBI_BASE + DBI_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_FIFO_YUV_MODE, fifo_mode);
    writel(tmpVal, DBI_BASE + DBI_FIFO_CONFIG_0_OFFSET);
}

void DBI_YUV2RGB_Init(DBI_YUV2RGB_Cfg_Type *yuv2rgbCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(DBI_BASE + DBI_YUV_RGB_CONFIG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_PRE_0, yuv2rgbCfg->preOffset0);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_PRE_1, yuv2rgbCfg->preOffset1);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_PRE_2, yuv2rgbCfg->preOffset2);
    writel(tmpVal, DBI_BASE + DBI_YUV_RGB_CONFIG_0_OFFSET);

    tmpVal = readl(DBI_BASE + DBI_YUV_RGB_CONFIG_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_POS_0, yuv2rgbCfg->postOffset0);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_POS_1, yuv2rgbCfg->postOffset1);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_POS_2, yuv2rgbCfg->postOffset2);
    writel(tmpVal, DBI_BASE + DBI_YUV_RGB_CONFIG_1_OFFSET);

    tmpVal = readl(DBI_BASE + DBI_YUV_RGB_CONFIG_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_MTX_00, yuv2rgbCfg->matrix00);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_MTX_01, yuv2rgbCfg->matrix01);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_MTX_02_L, yuv2rgbCfg->matrix02);
    writel(tmpVal, DBI_BASE + DBI_YUV_RGB_CONFIG_2_OFFSET);

    tmpVal = readl(DBI_BASE + DBI_YUV_RGB_CONFIG_3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_MTX_02_U, yuv2rgbCfg->matrix02 >> 8);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_MTX_10, yuv2rgbCfg->matrix10);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_MTX_11, yuv2rgbCfg->matrix11);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_MTX_12_L, yuv2rgbCfg->matrix12);
    writel(tmpVal, DBI_BASE + DBI_YUV_RGB_CONFIG_3_OFFSET);

    tmpVal = readl(DBI_BASE + DBI_YUV_RGB_CONFIG_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_MTX_12_U, yuv2rgbCfg->matrix12 >> 4);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_MTX_20, yuv2rgbCfg->matrix20);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_MTX_21, yuv2rgbCfg->matrix21);
    writel(tmpVal, DBI_BASE + DBI_YUV_RGB_CONFIG_4_OFFSET);

    tmpVal = readl(DBI_BASE + DBI_YUV_RGB_CONFIG_5_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_Y2R_MTX_22, yuv2rgbCfg->matrix22);
    writel(tmpVal, DBI_BASE + DBI_YUV_RGB_CONFIG_5_OFFSET);

    /* Enable or disable function */
    tmpVal = readl(DBI_BASE + DBI_YUV_RGB_CONFIG_0_OFFSET);
    writel(reg_set_bits(tmpVal, DBI_CR_Y2R_EN, yuv2rgbCfg->enable), DBI_BASE + DBI_YUV_RGB_CONFIG_0_OFFSET);
}

void DBI_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(DBI_BASE + DBI_CONFIG_OFFSET);
    writel(reg_set_bit(tmpVal, DBI_CR_DBI_EN), DBI_BASE + DBI_CONFIG_OFFSET);
}

void DBI_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(DBI_BASE + DBI_CONFIG_OFFSET);
    writel(reg_clr_bit(tmpVal, DBI_CR_DBI_EN), DBI_BASE + DBI_CONFIG_OFFSET);
}

void DBI_SetPeriod(DBI_Period_CFG_Type *period)
{
    uint32_t tmpVal;

    /* Set data period */
    tmpVal = readl(DBI_BASE + DBI_PRD_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_PRD_S, period->startLen);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_PRD_D_PH_0, period->dataPhase0Len);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_PRD_D_PH_1, period->dataPhase1Len);
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_PRD_I, period->intervalLen);
    writel(tmpVal, DBI_BASE + DBI_PRD_OFFSET);
}

void DBI_SetCommand(uint8_t Command )
{
    /* Set dbi qspi start Command */
    writel(Command, DBI_BASE + DBI_CMD_OFFSET);
}

void DBI_SetPhaseState(int commandEn, int dataEn)
{
    uint32_t tmpVal;

    /* Set dbi config */
    tmpVal = readl(DBI_BASE + DBI_CONFIG_OFFSET);
    /* set cmd phase */
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_CMD_EN, commandEn);
    /* set data phase */
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_DAT_EN, dataEn);
    writel(tmpVal, DBI_BASE + DBI_CONFIG_OFFSET);
}

void DBI_SetDataTypeAndCount(DBI_Data_Type dataType, uint32_t dataCount)
{
    uint32_t tmpVal, tmpVal2;

    /* Set dbi config */
    tmpVal = readl(DBI_BASE + DBI_CONFIG_OFFSET);
    /* Set data type */
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_DAT_TP, dataType);

    if(!dataCount){
        /* No data phase */
        tmpVal = reg_clr_bit(tmpVal, DBI_CR_DBI_DAT_EN);
    }else{
        /* data will be sent */
        tmpVal = reg_set_bit(tmpVal, DBI_CR_DBI_DAT_EN);

        if(dataType == DBI_DATA_NORMAL){
            /* data byte count of normal-data */
            tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_DAT_BC, dataCount-1);
        }else{
            /* pixel count of pixel-data */
            tmpVal2 = readl(DBI_BASE + DBI_PIX_CNT_OFFSET);
            tmpVal2 = reg_set_bits(tmpVal2, DBI_CR_DBI_PIX_CNT, dataCount-1);
            writel(tmpVal2, DBI_BASE + DBI_PIX_CNT_OFFSET);
        }
    }
    writel(tmpVal, DBI_BASE + DBI_CONFIG_OFFSET);
}

void DBI_SetNormalDataDir(DBI_Data_Direction_Type dataDir)
{
    uint32_t tmpVal;
    /* Set dbi config */
    tmpVal = readl(DBI_BASE + DBI_CONFIG_OFFSET);
    /* set normal data direction */
    tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_DAT_WR, dataDir);
    writel(tmpVal, DBI_BASE + DBI_CONFIG_OFFSET);
}

void DBI_ReadNormlData(uint8_t dataSize, uint8_t *dataBuff)
{
    if(dataSize>8){
        dataSize = 8;
    }

    /* get receive data from registers*/
    uint32_t para_data[2];
    para_data[0] = readl(DBI_BASE + DBI_RDATA_0_OFFSET);
    para_data[1] = readl(DBI_BASE + DBI_RDATA_1_OFFSET);

    /* copy to buff */
    memcpy(dataBuff, para_data, dataSize);
}

void DBI_WirteData2FIFO(uint32_t wordCount, uint32_t *dataBuff)
{
    for(uint32_t i = 0; i < wordCount; ){
        if (DBI_GetTxFifoCount() > 0) {
            writel(dataBuff[i++], DBI_BASE + DBI_FIFO_WDATA_OFFSET);
        }
    }
}

uint32_t DBI_GetWordCountOfPixelData(int32_t PixelCount)
{
    uint32_t wordCount;
    /* get pixel-formats of fifo*/
    DBI_FIFO_Format_Type fifoFormat = reg_get_bits(readl(DBI_BASE + DBI_FIFO_CONFIG_0_OFFSET), DBI_FIFO_FORMAT);

    if(fifoFormat == DBI_FIFO_888_NBGR || fifoFormat == DBI_FIFO_888_NRGB){
        wordCount = PixelCount;
    }else if(fifoFormat == DBI_FIFO_565_BGRBGR || fifoFormat == DBI_FIFO_565_RGBRGB){
        wordCount = (PixelCount + 1) >> 1;
    }else{
        wordCount = (PixelCount * 3 + 3) >> 2;
    }
    return wordCount;
}

void DBI_SendCmdWithNormalData(uint8_t command, uint8_t dataSize, uint8_t *dataBuff)
{
    uint8_t wordsCount,n;

    /* disable transfer */
    DBI_Disable();

    /* clear fifo and interrupt */
    DBI_TxFifoClear();
    DBI_IntClear();

    /* set command */
    DBI_SetCommand(command);

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

void DBI_SendCmdAndReadNormalData(uint8_t command, uint8_t dataSize, uint8_t *dataBuff)
{
    /* disable transfer */
    DBI_Disable();

    /* clear interrupt */
    DBI_IntClear();

    /* set command */
    DBI_SetCommand(command);

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

void DBI_SendCmdWithPixelData(uint8_t command, int32_t PixelCount, uint32_t *pixelBuff)
{
    /* disable transfer */
    DBI_Disable();

    /* clear fifo and interrupt */
    DBI_TxFifoClear();
    DBI_IntClear();

    /* set command */
    DBI_SetCommand(command);

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

void DBI_TxFifoClear(void)
{
    uint32_t tmpVal;

    tmpVal = readl(DBI_BASE + DBI_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, DBI_TX_FIFO_CLR);
    writel(tmpVal, DBI_BASE + DBI_FIFO_CONFIG_0_OFFSET);
}

void DBI_SetDMA(int dmaEn)
{
    uint32_t tmpVal;

    tmpVal = readl(DBI_BASE + DBI_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_DMA_TX_EN, dmaEn);
    writel(tmpVal, DBI_BASE + DBI_FIFO_CONFIG_0_OFFSET);
}

void DBI_SetTxFifoThreshold(uint8_t threshold)
{
    uint32_t tmpVal;

    tmpVal = readl(DBI_BASE + DBI_FIFO_CONFIG_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DBI_TX_FIFO_TH, threshold);
    writel(tmpVal, DBI_BASE + DBI_FIFO_CONFIG_1_OFFSET);
}

uint8_t DBI_GetTxFifoCount(void)
{
    return reg_get_bits(readl(DBI_BASE + DBI_FIFO_CONFIG_1_OFFSET), DBI_TX_FIFO_CNT);
}

int DBI_GetIntStatus(DBI_INT_Type intType)
{
    uint32_t tmpVal;

    tmpVal = readl(DBI_BASE + DBI_INT_STS_OFFSET);

    if ((tmpVal >> intType & 1) == 1) {
        return SET;
    } else {
        return RESET;
    }
}

int DBI_GetOverflowStatus(DBI_Overflow_Type overflow)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(DBI_BASE + DBI_FIFO_CONFIG_0_OFFSET);

    if (overflow == DBI_TX_OVERFLOW) {
        if (reg_is_bit_set(tmpVal, DBI_TX_FIFO_OVERFLOW)) {
            return SET;
        } else {
            return RESET;
        }
    } else {
        if (reg_is_bit_set(tmpVal, DBI_TX_FIFO_UNDERFLOW)) {
            return SET;
        } else {
            return RESET;
        }
    }
}

int DBI_GetBusBusyStatus(void)
{
    if (reg_is_bit_set(readl(DBI_BASE + DBI_CONFIG_OFFSET), DBI_STS_DBI_BUS_BUSY)) {
        return SET;
    } else {
        return RESET;
    }
}

void DBI_IntMask(DBI_INT_Type intType, int intMask)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = readl(DBI_BASE + DBI_INT_STS_OFFSET);

    switch (intType) {
        case DBI_INT_END:
            tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_END_MASK, intMask);
            break;

        case DBI_INT_TX_FIFO_REQ:
            tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_TXF_MASK, intMask);
            break;

        case DBI_INT_FIFO_ERR:
            tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_FER_MASK, intMask);
            break;

        case DBI_INT_ALL:
            tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_END_MASK, intMask);
            tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_TXF_MASK, intMask);
            tmpVal = reg_set_bits(tmpVal, DBI_CR_DBI_FER_MASK, intMask);
            break;

        default:
            break;
    }

    writel(tmpVal, DBI_BASE + DBI_INT_STS_OFFSET);
}

void DBI_IntClear(void)
{
    uint32_t tmpVal;

    tmpVal = readl(DBI_BASE + DBI_INT_STS_OFFSET);
    writel(reg_set_bit(tmpVal, DBI_CR_DBI_END_CLR), DBI_BASE + DBI_INT_STS_OFFSET);
}

#ifndef IPRO_USE_HAL_DRIVER
void DBI_Int_Callback_Install(DBI_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

    DBIIntCbfArra[intType] = cbFun;
}
#endif

