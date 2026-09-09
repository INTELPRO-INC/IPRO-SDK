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



#include "drv_spi.h"
#include "drv_glb.h"

#define SPI_TX_TIMEOUT_COUNT (160 * 1000)
#define SPI_RX_TIMEOUT_COUNT (160 * 1000)

static const uint32_t spiAddr[SPI_ID_MAX] = { SPI_BASE };

int SPI_Init(SPI_ID_Type spiNo, SPI_CFG_Type *spiCfg)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Disable clock gate when use SPI0 */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SPI);

    /* spi config */
    tmpVal = readl(SPIx + SPI_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_DEG_EN, spiCfg->deglitchEnable);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_S_3PIN_MODE, spiCfg->ignore_cs);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_M_CONT_EN, spiCfg->continuousEnable);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_BYTE_INV, spiCfg->byteSequence);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_BIT_INV, spiCfg->bitSequence);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_SCLK_PH, (spiCfg->clkPhaseInv + 1) & 1);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_SCLK_POL, spiCfg->clkPolarity);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_FRAME_SIZE, spiCfg->frameSize);
    writel(tmpVal, SPIx + SPI_CONFIG_OFFSET);

    return SUCCESS;
}

int SPI_DeInit(SPI_ID_Type spiNo)
{
    /* Check the parameters */

    if (spiNo == SPI_ID_0)
        GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SPI);
    return SUCCESS;
}

int SPI_ClockConfig(SPI_ID_Type spiNo, SPI_ClockCfg_Type *clockCfg)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Configure length of data phase1/0 and start/stop condition */
    tmpVal = readl(SPIx + SPI_PRD_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_PRD_S, clockCfg->startLen ? clockCfg->startLen - 1 : 0);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_PRD_P, clockCfg->stopLen ? clockCfg->stopLen - 1 : 0);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_PRD_D_PH_0, clockCfg->dataPhase0Len ? clockCfg->dataPhase0Len - 1 : 0);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_PRD_D_PH_1, clockCfg->dataPhase1Len ? clockCfg->dataPhase1Len - 1 : 0);
    writel(tmpVal, SPIx + SPI_PRD_0_OFFSET);

    /* Configure length of interval between frame */
    tmpVal = readl(SPIx + SPI_PRD_1_OFFSET);
    writel(reg_set_bits(tmpVal, SPI_CR_SPI_PRD_I, clockCfg->intervalLen ? clockCfg->intervalLen - 1 : 0), SPIx + SPI_PRD_1_OFFSET);

    return SUCCESS;
}

int SPI_FifoConfig(SPI_ID_Type spiNo, SPI_FifoCfg_Type *fifoCfg)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Set fifo threshold value */
    tmpVal = readl(SPIx + SPI_FIFO_CONFIG_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SPI_TX_FIFO_TH, fifoCfg->txFifoThreshold);
    tmpVal = reg_set_bits(tmpVal, SPI_RX_FIFO_TH, fifoCfg->rxFifoThreshold);
    writel(tmpVal, SPIx + SPI_FIFO_CONFIG_1_OFFSET);

    /* Enable or disable dma function */
    tmpVal = readl(SPIx + SPI_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SPI_DMA_TX_EN, fifoCfg->txFifoDmaEnable);
    tmpVal = reg_set_bits(tmpVal, SPI_DMA_RX_EN, fifoCfg->rxFifoDmaEnable);
    writel(tmpVal, SPIx + SPI_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int SPI_SetFrameSize(SPI_ID_Type spiNo, uint8_t frame_size)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* The SPI must be clear fifo first */
    SPI_ClrRxFifo(spiNo);
    SPI_ClrTxFifo(spiNo);

    /* set framsize*/
    tmpVal = readl(SPIx + SPI_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_FRAME_SIZE, frame_size);
    writel(tmpVal, SPIx + SPI_CONFIG_OFFSET);

    return SUCCESS;
}

int SPI_DmaTxEnable(SPI_ID_Type spiNo)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    tmpVal = readl(SPIx + SPI_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SPI_DMA_TX_EN);
    writel(tmpVal, SPIx +  SPI_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int SPI_DmaTxDisable(SPI_ID_Type spiNo)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    tmpVal = readl(SPIx + SPI_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SPI_DMA_TX_EN);
    writel(tmpVal, SPIx +  SPI_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int SPI_DmaRxEnable(SPI_ID_Type spiNo)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    tmpVal = readl(SPIx + SPI_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SPI_DMA_RX_EN);
    writel(tmpVal, SPIx +  SPI_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int SPI_DmaRxDisable(SPI_ID_Type spiNo)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    tmpVal = readl(SPIx + SPI_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SPI_DMA_RX_EN);
    writel(tmpVal, SPIx +  SPI_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int SPI_SetClock(SPI_ID_Type spiNo, uint32_t clk)
{
    uint32_t glb_div = 0, spi_div = 0;
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    if (clk > 80000000) {
        clk = 80000000;
    } else if (clk < 9766) {
        clk = 9766;
    }

    if (clk >= 312500) {
        glb_div = 0;
        spi_div = 80000000 / clk - 1;
    } else {
        glb_div = 31;
        spi_div = 2500000 / clk - 1;
    }

    if (spiNo == SPI_ID_0) {
        GLB_Set_SPI_CLK(ENABLE, GLB_SPI_CLK_MCU_MUXPLL_160M, glb_div);
    }

    /* Configure length of data phase1/0 and start/stop condition */
    tmpVal = readl(SPIx + SPI_PRD_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_PRD_S, spi_div);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_PRD_P, spi_div);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_PRD_D_PH_0, spi_div);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_PRD_D_PH_1, spi_div);
    writel(tmpVal, SPIx + SPI_PRD_0_OFFSET);

    tmpVal = readl(SPIx + SPI_PRD_1_OFFSET);
    writel(reg_set_bits(tmpVal, SPI_CR_SPI_PRD_I, spi_div), SPIx + SPI_PRD_1_OFFSET);

    return SUCCESS;
}

int SPI_Enable(SPI_ID_Type spiNo, SPI_WORK_MODE_Type modeType)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    tmpVal = readl(SPIx + SPI_CONFIG_OFFSET);

    if (modeType != SPI_WORK_MODE_SLAVE) {
        /* master mode */
        tmpVal = reg_clr_bit(tmpVal, SPI_CR_SPI_S_EN);
        tmpVal = reg_set_bit(tmpVal, SPI_CR_SPI_M_EN);
    } else {
        /* slave mode */
        tmpVal = reg_clr_bit(tmpVal, SPI_CR_SPI_M_EN);
        tmpVal = reg_set_bit(tmpVal, SPI_CR_SPI_S_EN);
    }

    writel(tmpVal, SPIx + SPI_CONFIG_OFFSET);

    return SUCCESS;
}

int SPI_Disable(SPI_ID_Type spiNo, SPI_WORK_MODE_Type modeType)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* close master and slave */
    tmpVal = readl(SPIx + SPI_CONFIG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, SPI_CR_SPI_M_EN);
    tmpVal = reg_clr_bit(tmpVal, SPI_CR_SPI_S_EN);
    writel(tmpVal, SPIx + SPI_CONFIG_OFFSET);

    return SUCCESS;
}

int SPI_SetTimeOutValue(SPI_ID_Type spiNo, uint16_t value)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Set time-out value */
    tmpVal = readl(SPIx + SPI_STO_VALUE_OFFSET);
    writel(reg_set_bits(tmpVal, SPI_CR_SPI_STO_VALUE, value - 1), SPIx + SPI_STO_VALUE_OFFSET);

    return SUCCESS;
}

int SPI_SetDeglitchCount(SPI_ID_Type spiNo, uint8_t cnt)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Set count value */
    tmpVal = readl(SPIx + SPI_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, SPI_CR_SPI_DEG_CNT, cnt);
    writel(tmpVal, SPIx + SPI_CONFIG_OFFSET);

    return SUCCESS;
}

int SPI_RxIgnoreEnable(SPI_ID_Type spiNo, uint8_t startPoint, uint8_t stopPoint)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Enable rx ignore function */
    tmpVal = readl(SPIx + SPI_CONFIG_OFFSET);
    writel(reg_set_bit(tmpVal, SPI_CR_SPI_RXD_IGNR_EN), SPIx + SPI_CONFIG_OFFSET);

    /* Set start and stop point */
    tmpVal = startPoint << SPI_CR_SPI_RXD_IGNR_S_POS | stopPoint;
    writel(tmpVal, SPIx + SPI_RXD_IGNR_OFFSET);

    return SUCCESS;
}

int SPI_RxIgnoreDisable(SPI_ID_Type spiNo)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Disable rx ignore function */
    tmpVal = readl(SPIx + SPI_CONFIG_OFFSET);
    writel(reg_clr_bit(tmpVal, SPI_CR_SPI_RXD_IGNR_EN), SPIx + SPI_CONFIG_OFFSET);

    return SUCCESS;
}

int SPI_ClrTxFifo(SPI_ID_Type spiNo)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Clear tx fifo */
    tmpVal = readl(SPIx + SPI_FIFO_CONFIG_0_OFFSET);
    writel(reg_set_bit(tmpVal, SPI_TX_FIFO_CLR), SPIx + SPI_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int SPI_ClrRxFifo(SPI_ID_Type spiNo)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Clear rx fifo */
    tmpVal = readl(SPIx + SPI_FIFO_CONFIG_0_OFFSET);
    writel(reg_set_bit(tmpVal, SPI_RX_FIFO_CLR), SPIx + SPI_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int SPI_ClrIntStatus(SPI_ID_Type spiNo, SPI_INT_Type intType)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Clear certain or all interrupt */
    tmpVal = readl(SPIx + SPI_INT_STS_OFFSET);

    if (SPI_INT_ALL == intType) {
        tmpVal |= 0x1f << SPI_CR_SPI_END_CLR_POS;
    } else {
        tmpVal |= 1 << (intType + SPI_CR_SPI_END_CLR_POS);
    }

    writel(tmpVal, SPIx + SPI_INT_STS_OFFSET);

    return SUCCESS;
}

int SPI_IntMask(SPI_ID_Type spiNo, SPI_INT_Type intType, int intMask)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    tmpVal = readl(SPIx + SPI_INT_STS_OFFSET);

    /* Mask or unmask certain or all interrupt */
    if (SPI_INT_ALL == intType) {
        if (MASK == intMask) {
            tmpVal |= 0x3f << SPI_CR_SPI_END_MASK_POS;
        } else {
            tmpVal &= ~(0x3f << SPI_CR_SPI_END_MASK_POS);
        }
    } else {
        if (MASK == intMask) {
            tmpVal |= 1 << (intType + SPI_CR_SPI_END_MASK_POS);
        } else {
            tmpVal &= ~(1 << (intType + SPI_CR_SPI_END_MASK_POS));
        }
    }

    /* Write back */
    writel(tmpVal, SPIx + SPI_INT_STS_OFFSET);

    return SUCCESS;
}

int SPI_SendData(SPI_ID_Type spiNo, void *buff, uint32_t length, SPI_Timeout_Type timeoutType)
{
    uint32_t tmpVal, tx_data;
    uint32_t spixBase = spiAddr[spiNo];
    uint32_t timeoutCnt = SPI_RX_TIMEOUT_COUNT;
    uint8_t fifo_cnt, frameSize;

    /* Check the parameters */

    /* Get fifo valid width */
    tmpVal = readl(spixBase + SPI_CONFIG_OFFSET);
    frameSize = reg_get_bits(tmpVal, SPI_CR_SPI_FRAME_SIZE) + 1;
    if (frameSize == 3) {
        frameSize = 4;
    }

    /* ready to clear rx fifo */
    tmpVal = readl(spixBase + SPI_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SPI_RX_FIFO_CLR);

    for (; length > 0;) {
        /* get tx fifo cnt */
        fifo_cnt = SPI_GetTxFifoCount(spiNo) / frameSize;

        if (fifo_cnt) {
            fifo_cnt = fifo_cnt > length ? length : fifo_cnt;
            length -= fifo_cnt;
            timeoutCnt = SPI_TX_TIMEOUT_COUNT;
        } else {
            if (timeoutType) {
                timeoutCnt--;
                if (timeoutCnt == 0) {
                    return TIMEOUT;
                }
            }
            continue;
        }
        /* clear rx fifo */
        writel(tmpVal, spixBase + SPI_FIFO_CONFIG_0_OFFSET);
        /* write to tx fifo */
        for (; fifo_cnt > 0; fifo_cnt--) {
            switch (frameSize) {
                case 1:
                    tx_data = *(uint8_t *)buff;
                    writel(tx_data, spixBase + SPI_FIFO_WDATA_OFFSET);
                    buff++;
                    break;
                case 2:
                    tx_data = *(uint16_t *)buff;
                    writel(tx_data, spixBase + SPI_FIFO_WDATA_OFFSET);
                    buff += 2;
                    break;
                case 3:
                case 4:
                    tx_data = *(uint32_t *)buff;
                    writel(tx_data, spixBase + SPI_FIFO_WDATA_OFFSET);
                    buff += 4;
                    break;
                default:
                    return INVALID;
                    break;
            }
        }
    }
    /* Waiting for sending to end, supported slave mode */
    while (SPI_GetBusyStatus(spiNo) == SET || SPI_GetTxFifoCount(spiNo) < SPI_FIFO_LEN) {
        if (timeoutType) {
            timeoutCnt--;
            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        }
    }
    /* clear rx fifo */
    writel(tmpVal, spixBase + SPI_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int SPI_ReceiveData(SPI_ID_Type spiNo, void *recvBuff, uint32_t length, SPI_Timeout_Type timeoutType)
{
    uint32_t tmpVal;
    uint32_t spixBase = spiAddr[spiNo];
    uint32_t timeoutCnt = SPI_RX_TIMEOUT_COUNT;
    uint32_t tx_cnt = length;
    uint8_t fifo_cnt, frameSize;
    SPI_WORK_MODE_Type spi_mode;

    /* Check the parameters */

    /* Get fifo valid width and spi mode */
    tmpVal = readl(spixBase + SPI_CONFIG_OFFSET);
    frameSize = reg_get_bits(tmpVal, SPI_CR_SPI_FRAME_SIZE) + 1;
    if (frameSize == 3) {
        frameSize = 4;
    }
    spi_mode = reg_is_bit_set(tmpVal, SPI_CR_SPI_S_EN) ? SPI_WORK_MODE_SLAVE : SPI_WORK_MODE_MASTER;

    if (spi_mode == SPI_WORK_MODE_MASTER) {
        /* clear tx/rx fifo when master mode */
        tmpVal = readl(spixBase + SPI_FIFO_CONFIG_0_OFFSET);
        tmpVal = reg_set_bit(tmpVal, SPI_TX_FIFO_CLR);
        tmpVal = reg_set_bit(tmpVal, SPI_RX_FIFO_CLR);
        writel(tmpVal, spixBase + SPI_FIFO_CONFIG_0_OFFSET);

        /* first fill tx fifo when master mode */
        fifo_cnt = SPI_GetTxFifoCount(spiNo) / frameSize;
        fifo_cnt = fifo_cnt > length ? length : fifo_cnt;
        tx_cnt -= fifo_cnt;
        for (; fifo_cnt > 0; fifo_cnt--) {
            writel(0xFFFFFFFF, spixBase + SPI_FIFO_WDATA_OFFSET);
        }
    }

    /* read and write data */
    for (; length > 0;) {
        /* get rx fifo cnt */
        fifo_cnt = SPI_GetRxFifoCount(spiNo) / frameSize;

        if (fifo_cnt) {
            fifo_cnt = fifo_cnt > length ? length : fifo_cnt;
            length -= fifo_cnt;
            timeoutCnt = SPI_TX_TIMEOUT_COUNT;
        } else {
            if (timeoutType) {
                timeoutCnt--;
                if (timeoutCnt == 0) {
                    return TIMEOUT;
                }
            }
            continue;
        }

        /* read and write data */
        for (; fifo_cnt > 0; fifo_cnt--) {
            switch (frameSize) {
                case 1:
                    tmpVal = readl(spixBase + SPI_FIFO_RDATA_OFFSET);
                    *(uint8_t *)recvBuff = (uint8_t)tmpVal;
                    recvBuff++;
                    break;
                case 2:
                    tmpVal = readl(spixBase + SPI_FIFO_RDATA_OFFSET);
                    *(uint16_t *)recvBuff = (uint16_t)tmpVal;
                    recvBuff += 2;
                    break;
                case 3:
                case 4:
                    tmpVal = readl(spixBase + SPI_FIFO_RDATA_OFFSET);
                    *(uint32_t *)recvBuff = (uint32_t)tmpVal;
                    recvBuff += 4;
                    break;
                default:
                    return INVALID;
                    break;
            }
            if (spi_mode == SPI_WORK_MODE_MASTER && tx_cnt) {
                writel(0xFFFFFFFF, spixBase + SPI_FIFO_WDATA_OFFSET);
                tx_cnt--;
            }
        }
    }

    return SUCCESS;
}

int SPI_SendRecvData(SPI_ID_Type spiNo, void *sendBuff, void *recvBuff, uint32_t length, SPI_Timeout_Type timeoutType)
{
    uint32_t tmpVal;
    uint32_t spixBase = spiAddr[spiNo];
    uint32_t timeoutCnt = SPI_RX_TIMEOUT_COUNT;
    uint32_t tx_cnt = length;
    uint8_t fifo_cnt, frameSize;

    /* Check the parameters */

    /* Get fifo valid width */
    tmpVal = readl(spixBase + SPI_CONFIG_OFFSET);
    frameSize = reg_get_bits(tmpVal, SPI_CR_SPI_FRAME_SIZE) + 1;
    if (frameSize == 3) {
        frameSize = 4;
    }

    /* clear tx/rx fifo */
    tmpVal = readl(spixBase + SPI_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SPI_RX_FIFO_CLR);
    tmpVal = reg_set_bit(tmpVal, SPI_TX_FIFO_CLR);
    writel(tmpVal, spixBase + SPI_FIFO_CONFIG_0_OFFSET);

    /* First fill tx fifo */
    fifo_cnt = SPI_GetTxFifoCount(spiNo) / frameSize;
    fifo_cnt = fifo_cnt > length ? length : fifo_cnt;
    tx_cnt -= fifo_cnt;
    for (; fifo_cnt > 0; fifo_cnt--) {
        switch (frameSize) {
            case 1:
                tmpVal = *(uint8_t *)sendBuff;
                writel(tmpVal, spixBase + SPI_FIFO_WDATA_OFFSET);
                sendBuff++;
                break;
            case 2:
                tmpVal = *(uint16_t *)sendBuff;
                writel(tmpVal, spixBase + SPI_FIFO_WDATA_OFFSET);
                sendBuff += 2;
                break;
            case 3:
            case 4:
                tmpVal = *(uint32_t *)sendBuff;
                writel(tmpVal, spixBase + SPI_FIFO_WDATA_OFFSET);
                sendBuff += 4;
                break;
            default:
                return INVALID;
                break;
        }
    }

    /* read and write rest of the data */
    for (; length > 0;) {
        /* get rx fifo cnt */
        fifo_cnt = SPI_GetRxFifoCount(spiNo) / frameSize;

        if (fifo_cnt) {
            fifo_cnt = fifo_cnt > length ? length : fifo_cnt;
            length -= fifo_cnt;
            timeoutCnt = SPI_TX_TIMEOUT_COUNT;
        } else {
            if (timeoutType) {
                timeoutCnt--;
                if (timeoutCnt == 0) {
                    return TIMEOUT;
                }
            }
            continue;
        }

        /* read and write data */
        for (; fifo_cnt > 0; fifo_cnt--) {
            switch (frameSize) {
                case 1:
                    tmpVal = readl(spixBase + SPI_FIFO_RDATA_OFFSET);
                    *((uint8_t *)recvBuff) = (uint8_t)tmpVal;
                    recvBuff++;
                    if (tx_cnt) {
                        tmpVal = *(uint8_t *)sendBuff;
                        writel(tmpVal, spixBase + SPI_FIFO_WDATA_OFFSET);
                        sendBuff++;
                        tx_cnt--;
                    }
                    break;
                case 2:
                    tmpVal = readl(spixBase + SPI_FIFO_RDATA_OFFSET);
                    *((uint16_t *)recvBuff) = (uint16_t)tmpVal;
                    recvBuff += 2;
                    if (tx_cnt) {
                        tmpVal = *(uint16_t *)sendBuff;
                        writel(tmpVal, spixBase + SPI_FIFO_WDATA_OFFSET);
                        sendBuff += 2;
                        tx_cnt--;
                    }
                    break;
                case 3:
                case 4:
                    tmpVal = readl(spixBase + SPI_FIFO_RDATA_OFFSET);
                    *((uint32_t *)recvBuff) = (uint32_t)tmpVal;
                    recvBuff += 4;
                    if (tx_cnt) {
                        tmpVal = *(uint32_t *)sendBuff;
                        writel(tmpVal, spixBase + SPI_FIFO_WDATA_OFFSET);
                        sendBuff += 4;
                        tx_cnt--;
                    }
                    break;
                default:
                    return INVALID;
                    break;
            }
        }
    }

    return SUCCESS;
}

uint8_t SPI_GetTxFifoCount(SPI_ID_Type spiNo)
{
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Get count value */
    return reg_get_bits(readl(SPIx + SPI_FIFO_CONFIG_1_OFFSET), SPI_TX_FIFO_CNT);
}

uint8_t SPI_GetRxFifoCount(SPI_ID_Type spiNo)
{
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Get count value */
    return reg_get_bits(readl(SPIx + SPI_FIFO_CONFIG_1_OFFSET), SPI_RX_FIFO_CNT);
}

int SPI_GetIntStatus(SPI_ID_Type spiNo, SPI_INT_Type intType)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Get certain or all interrupt status */
    tmpVal = readl(SPIx + SPI_INT_STS_OFFSET);

    if (SPI_INT_ALL == intType) {
        if ((tmpVal & 0x3f) != 0) {
            return SET;
        } else {
            return RESET;
        }
    } else {
        if ((tmpVal & (1U << intType)) != 0) {
            return SET;
        } else {
            return RESET;
        }
    }
}

int SPI_ClearInt(SPI_ID_Type spiNo, SPI_INT_Type intType)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    tmpVal = readl(SPIx + SPI_INT_STS_OFFSET);

    switch (intType) {
    case SPI_INT_END:
        tmpVal = reg_clr_bit(tmpVal, SPI_CR_SPI_END_CLR);
        break;
    case SPI_INT_SLAVE_TIMEOUT:
        tmpVal = reg_clr_bit(tmpVal, SPI_CR_SPI_STO_CLR);
        break;
    case SPI_INT_SLAVE_UNDERRUN:
        tmpVal = reg_clr_bit(tmpVal, SPI_CR_SPI_TXU_CLR);
        break;
    case SPI_INT_ALL:
        tmpVal = reg_clr_bit(tmpVal, SPI_CR_SPI_END_CLR);
        tmpVal = reg_clr_bit(tmpVal, SPI_CR_SPI_STO_CLR);
        tmpVal = reg_clr_bit(tmpVal, SPI_CR_SPI_TXU_CLR);
        break;
    default:
        return -1;
    };
    writel(tmpVal, SPIx + SPI_INT_STS_OFFSET);
    return 0;
}

int SPI_GetBusyStatus(SPI_ID_Type spiNo)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Get bus busy status */
    tmpVal = readl(SPIx + SPI_BUS_BUSY_OFFSET);

    if (reg_is_bit_set(tmpVal, SPI_STS_SPI_BUS_BUSY)) {
        return SET;
    } else {
        return RESET;
    }
}

int SPI_GetFifoStatus(SPI_ID_Type spiNo, SPI_FifoStatus_Type fifoSts)
{
    uint32_t tmpVal;
    uint32_t SPIx = spiAddr[spiNo];

    /* Check the parameters */

    /* Get tx/rx fifo overflow or underflow status */
    tmpVal = readl(SPIx + SPI_FIFO_CONFIG_0_OFFSET);

    if ((tmpVal & (1U << (fifoSts + SPI_TX_FIFO_OVERFLOW_POS))) != 0) {
        return SET;
    } else {
        return RESET;
    }
}


