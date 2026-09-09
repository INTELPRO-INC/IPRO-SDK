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



#include "drv_i2s.h"
#include "drv_glb.h"

static const uint32_t i2sAddr[I2S_ID_MAX] = { I2S_BASE };
#define I2S_USE_ID I2S0_ID

void I2S_SetBclkPeriod(I2S_ID_Type i2sId, I2S_CFG_Type *i2sCfg)
{
    uint32_t overSampleRate;
    uint32_t bclkDivCnt;
    uint32_t tmpVal;
    uint8_t enumOffset = 2;
    uint32_t I2Sx = i2sAddr[i2sId];

    if (i2sCfg->fsChannel == I2S_FS_CHANNELS_6) {
        enumOffset = 3;
    }
    overSampleRate = i2sCfg->audioFreqHz / i2sCfg->sampleFreqHz;

    switch (i2sCfg->frameSize) {
        case I2S_SIZE_FRAME_8:
            bclkDivCnt = overSampleRate / (8 * (i2sCfg->fsChannel + enumOffset));
            break;

        case I2S_SIZE_FRAME_16:
            bclkDivCnt = overSampleRate / (16 * (i2sCfg->fsChannel + enumOffset));
            break;

        case I2S_SIZE_FRAME_24:
            bclkDivCnt = overSampleRate / (24 * (i2sCfg->fsChannel + enumOffset));
            break;

        case I2S_SIZE_FRAME_32:
            bclkDivCnt = overSampleRate / (32 * (i2sCfg->fsChannel + enumOffset));
            break;

        default:
            bclkDivCnt = overSampleRate / (16 * (i2sCfg->fsChannel + enumOffset));
            break;
    }

    bclkDivCnt = (bclkDivCnt / 2) - 1;

    tmpVal = (bclkDivCnt << 16) | bclkDivCnt;
    writel(tmpVal, I2Sx + I2S_BCLK_CONFIG_OFFSET);
}

void I2S_Init(I2S_ID_Type i2sId, I2S_CFG_Type *i2sCfg)
{
    uint32_t tmpVal;
    uint32_t I2Sx = i2sAddr[i2sId];

    /* Check the parameters */

    /* Enable the I2S peripheral bus clock before touching its registers. */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_I2S);

    tmpVal = readl(I2Sx + I2S_CONFIG_OFFSET);

    /* Set data endian*/
    tmpVal = reg_set_bits(tmpVal, I2S_CR_ENDIAN, i2sCfg->endianType);

    /* Set I2S mode */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_I2S_MODE, i2sCfg->modeType);

    /* Set BCLK invert */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_I2S_BCLK_INV, i2sCfg->bclkInvert);

    /* Set FS size */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_FRAME_SIZE, i2sCfg->frameSize);

    /* Set FS invert */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_I2S_FS_INV, i2sCfg->fsInvert);

    /* Set FS mode */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_FS_1T_MODE, i2sCfg->fsMode);

    /* Set FS channel mode */

    tmpVal = reg_set_bits(tmpVal, I2S_CR_FS_CH_CNT, i2sCfg->fsChannel);

    /* Set Data size */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_DATA_SIZE, i2sCfg->dataSize);

    /* Set Data offset */
    if (i2sCfg->dataOffset != 0) {
        tmpVal = reg_set_bit(tmpVal, I2S_CR_OFS_EN);
        tmpVal = reg_set_bits(tmpVal, I2S_CR_OFS_CNT, i2sCfg->dataOffset - 1);
    } else {
        tmpVal = reg_clr_bit(tmpVal, I2S_CR_OFS_EN);
    }

    /* Set mono mode */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_MONO_MODE, i2sCfg->monoMode);

    /* Set rx mono mode channel left or right */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_MONO_RX_CH, i2sCfg->monoModeChannel);

    /* Clear mute mode */
    tmpVal = reg_clr_bit(tmpVal, I2S_CR_MUTE_MODE);

    writel(tmpVal, I2Sx + I2S_CONFIG_OFFSET);

    I2S_SetBclkPeriod(I2S_USE_ID, i2sCfg);
}

void I2S_FifoConfig(I2S_ID_Type i2sId, I2S_FifoCfg_Type *fifoCfg)
{
    uint32_t tmpVal;
    uint32_t I2Sx = i2sAddr[i2sId];

    tmpVal = readl(I2Sx + I2S_FIFO_CONFIG_0_OFFSET);
    /* Set packed mode */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_FIFO_LR_MERGE, fifoCfg->lRMerge);
    tmpVal = reg_set_bits(tmpVal, I2S_CR_FIFO_LR_EXCHG, fifoCfg->frameDataExchange);
    /* Clear tx and rx FIFO signal */
    tmpVal = reg_set_bit(tmpVal, I2S_TX_FIFO_CLR);
    tmpVal = reg_set_bit(tmpVal, I2S_RX_FIFO_CLR);

    /* Set DMA config */
    tmpVal = reg_set_bits(tmpVal, I2S_DMA_TX_EN, fifoCfg->txfifoDmaEnable);
    tmpVal = reg_set_bits(tmpVal, I2S_DMA_RX_EN, fifoCfg->rxfifoDmaEnable);

    writel(tmpVal, I2Sx + I2S_FIFO_CONFIG_0_OFFSET);

    /* Set CLR signal to 0*/
    tmpVal = reg_clr_bit(tmpVal, I2S_TX_FIFO_CLR);
    tmpVal = reg_clr_bit(tmpVal, I2S_RX_FIFO_CLR);
    writel(tmpVal, I2Sx + I2S_FIFO_CONFIG_0_OFFSET);

    tmpVal = readl(I2Sx + I2S_FIFO_CONFIG_1_OFFSET);
    /* Set TX and RX FIFO threshold */
    tmpVal = reg_set_bits(tmpVal, I2S_TX_FIFO_TH, fifoCfg->txFifoLevel);
    tmpVal = reg_set_bits(tmpVal, I2S_RX_FIFO_TH, fifoCfg->rxFifoLevel);

    writel(tmpVal, I2Sx + I2S_FIFO_CONFIG_1_OFFSET);
}

void I2S_IOConfig(I2S_ID_Type i2sId, I2S_IOCfg_Type *ioCfg)
{
    uint32_t tmpVal;
    uint32_t I2Sx = i2sAddr[i2sId];

    tmpVal = readl(I2Sx + I2S_IO_CONFIG_OFFSET);
    /* Enable or disable deglitch */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_DEG_EN, ioCfg->deglitchEn);

    /* Set deglitch cycle count */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_DEG_CNT, ioCfg->deglitchCnt);

    /* Enable or disable inverse BCLK signal */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_I2S_BCLK_INV, ioCfg->inverseBCLK);

    /* Enable or disable inverse FS signal */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_I2S_FS_INV, ioCfg->inverseFS);

    /* Enable or disable inverse RX signal */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_I2S_RXD_INV, ioCfg->inverseRX);

    /* Enable or disable inverse TX signal */
    tmpVal = reg_set_bits(tmpVal, I2S_CR_I2S_TXD_INV, ioCfg->inverseTX);

    writel(tmpVal, I2Sx + I2S_IO_CONFIG_OFFSET);
}

void I2S_Enable(I2S_ID_Type i2sId, I2S_Role_Type roleType)
{
    uint32_t tmpVal;
    uint32_t I2Sx = i2sAddr[i2sId];

    /* Check the parameters */

    tmpVal = readl(I2Sx + I2S_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, I2S_CR_I2S_TXD_EN);
    tmpVal = reg_set_bit(tmpVal, I2S_CR_I2S_RXD_EN);

    /* Set role type */
    if (I2S_ROLE_MASTER == roleType) {
        tmpVal = reg_set_bit(tmpVal, I2S_CR_I2S_M_EN);
        tmpVal = reg_clr_bit(tmpVal, I2S_CR_I2S_S_EN);
    } else if (I2S_ROLE_SLAVE == roleType) {
        tmpVal = reg_clr_bit(tmpVal, I2S_CR_I2S_M_EN);
        tmpVal = reg_set_bit(tmpVal, I2S_CR_I2S_S_EN);
    }

    writel(tmpVal, I2Sx + I2S_CONFIG_OFFSET);
}

void I2S_Disable(I2S_ID_Type i2sId)
{
    uint32_t tmpVal;
    uint32_t I2Sx = i2sAddr[i2sId];

    tmpVal = readl(I2Sx + I2S_CONFIG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, I2S_CR_I2S_TXD_EN);
    tmpVal = reg_clr_bit(tmpVal, I2S_CR_I2S_RXD_EN);

    tmpVal = reg_clr_bit(tmpVal, I2S_CR_I2S_M_EN);
    tmpVal = reg_clr_bit(tmpVal, I2S_CR_I2S_S_EN);
    writel(tmpVal, I2Sx + I2S_CONFIG_OFFSET);
}

uint32_t I2S_Read(I2S_ID_Type i2sId)
{
    uint32_t I2Sx = i2sAddr[i2sId];

    while (0 == reg_get_bits(readl(I2Sx + I2S_FIFO_CONFIG_1_OFFSET), I2S_RX_FIFO_CNT)) {
    };

    return readl(I2Sx + I2S_FIFO_RDATA_OFFSET);
}

void I2S_Write(I2S_ID_Type i2sId, uint32_t data)
{
    uint32_t I2Sx = i2sAddr[i2sId];

    while (0 == reg_get_bits(readl(I2Sx + I2S_FIFO_CONFIG_1_OFFSET), I2S_TX_FIFO_CNT)) {
    };

    writel(data, I2Sx + I2S_FIFO_WDATA_OFFSET);
}

void I2S_Mute(I2S_ID_Type i2sId, int enabled)
{
    uint32_t tmpVal;
    uint32_t I2Sx = i2sAddr[i2sId];

    tmpVal = readl(I2Sx + I2S_CONFIG_OFFSET);

    if (enabled ? 1 : 0) {
        tmpVal = reg_set_bit(tmpVal, I2S_CR_MUTE_MODE);
    } else {
        tmpVal = reg_clr_bit(tmpVal, I2S_CR_MUTE_MODE);
    }

    writel(tmpVal, I2Sx + I2S_CONFIG_OFFSET);
}

void I2S_SetFifoJustified(I2S_ID_Type i2sId, I2S_FIFO_24_Justified_Type justType)
{
    uint32_t tmpVal;
    uint32_t I2Sx = i2sAddr[i2sId];

    /* Check the parameters */

    tmpVal = readl(I2Sx + I2S_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, I2S_CR_FIFO_24B_LJ, justType);
    writel(tmpVal, I2Sx + I2S_FIFO_CONFIG_0_OFFSET);
}

uint32_t I2S_GetTxFIFO_AvlCnt(I2S_ID_Type i2sId)
{
    uint32_t I2Sx = i2sAddr[i2sId];

    return reg_get_bits(readl(I2Sx + I2S_FIFO_CONFIG_1_OFFSET), I2S_TX_FIFO_CNT);
}

uint32_t I2S_GetRxFIFO_AvlCnt(I2S_ID_Type i2sId)
{
    uint32_t I2Sx = i2sAddr[i2sId];

    return reg_get_bits(readl(I2Sx + I2S_FIFO_CONFIG_1_OFFSET), I2S_RX_FIFO_CNT);
}

void I2S_Tx_FIFO_Flush(I2S_ID_Type i2sId)
{
    uint32_t tmpVal;
    uint32_t I2Sx = i2sAddr[i2sId];

    tmpVal = readl(I2Sx + I2S_FIFO_CONFIG_0_OFFSET);

    tmpVal = reg_set_bit(tmpVal, I2S_TX_FIFO_CLR);

    writel(tmpVal, I2Sx + I2S_CONFIG_OFFSET);
}

void I2S_Rx_FIFO_Flush(I2S_ID_Type i2sId)
{
    uint32_t tmpVal;
    uint32_t I2Sx = i2sAddr[i2sId];

    tmpVal = readl(I2Sx + I2S_FIFO_CONFIG_0_OFFSET);

    tmpVal = reg_set_bit(tmpVal, I2S_RX_FIFO_CLR);

    writel(tmpVal, I2Sx + I2S_FIFO_CONFIG_0_OFFSET);
}
