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

#include "drv_uart.h"
#include "drv_glb.h"
#include "drv_clock.h"

#define UART_TX_TIMEOUT_COUNT (160 * 1000)

static const uint32_t uartAddr[2] = { UART0_BASE, UART1_BASE };

int UART_SetBaudRate(UART_ID_Type uartId, uint32_t baudRate)
{
    uint32_t uartClk= 0;
    uint32_t fraction = 0;
    uint32_t baudRateDivisor = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Get uart clk */
    if (uartId == UART0_ID) {
        uartClk = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_UART0);
    } else {
        return ERROR;
    }

    /* Cal the baud rate divisor */
    fraction = uartClk * 10 / baudRate % 10;
    baudRateDivisor = uartClk / baudRate;

    if (fraction >= 5) {
        ++baudRateDivisor;
    }

    /* Set the baud rate register value */
    writel(((baudRateDivisor - 1) << 0x10) | ((baudRateDivisor - 1) & 0xFFFF), UARTx + UART_BIT_PRD_OFFSET);

    return SUCCESS;
}

int UART_Init(UART_ID_Type uartId, UART_CFG_Type *uartCfg)
{
    uint32_t tmpValTxCfg = 0;
    uint32_t tmpValRxCfg = 0;
    uint32_t fraction = 0;
    uint32_t baudRateDivisor = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Cal the baud rate divisor */
    fraction = uartCfg->uartClk * 10 / uartCfg->baudRate % 10;
    baudRateDivisor = uartCfg->uartClk / uartCfg->baudRate;

    if (fraction >= 5) {
        ++baudRateDivisor;
    }

    /* Set the baud rate register value */
    writel(((baudRateDivisor - 1) << 0x10) | ((baudRateDivisor - 1) & 0xFFFF), UARTx + UART_BIT_PRD_OFFSET);

    tmpValTxCfg = readl(UARTx + UART_UTX_CONFIG_OFFSET);
    tmpValRxCfg = readl(UARTx + UART_URX_CONFIG_OFFSET);

    switch (uartCfg->parity) {
        case UART_PARITY_NONE:
            tmpValTxCfg = reg_clr_bit(tmpValTxCfg, UART_CR_UTX_PRT_EN);
            tmpValRxCfg = reg_clr_bit(tmpValRxCfg, UART_CR_URX_PRT_EN);
            break;

        case UART_PARITY_ODD:
            tmpValTxCfg = reg_set_bit(tmpValTxCfg, UART_CR_UTX_PRT_EN);
            tmpValTxCfg = reg_set_bit(tmpValTxCfg, UART_CR_UTX_PRT_SEL);
            tmpValRxCfg = reg_set_bit(tmpValRxCfg, UART_CR_URX_PRT_EN);
            tmpValRxCfg = reg_set_bit(tmpValRxCfg, UART_CR_URX_PRT_SEL);
            break;

        case UART_PARITY_EVEN:
            tmpValTxCfg = reg_set_bit(tmpValTxCfg, UART_CR_UTX_PRT_EN);
            tmpValTxCfg = reg_clr_bit(tmpValTxCfg, UART_CR_UTX_PRT_SEL);
            tmpValRxCfg = reg_set_bit(tmpValRxCfg, UART_CR_URX_PRT_EN);
            tmpValRxCfg = reg_clr_bit(tmpValRxCfg, UART_CR_URX_PRT_SEL);
            break;

        default:
            break;
    }

    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_UTX_BIT_CNT_D, (uartCfg->dataBits + 4));
    tmpValRxCfg = reg_set_bits(tmpValRxCfg, UART_CR_URX_BIT_CNT_D, (uartCfg->dataBits + 4));
    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_UTX_BIT_CNT_P, uartCfg->stopBits);
    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_UTX_CTS_EN, uartCfg->ctsFlowControl);
    tmpValRxCfg = reg_set_bits(tmpValRxCfg, UART_CR_URX_DEG_EN, uartCfg->rxDeglitch);
    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_UTX_LIN_EN, uartCfg->txLinMode);
    tmpValRxCfg = reg_set_bits(tmpValRxCfg, UART_CR_URX_LIN_EN, uartCfg->rxLinMode);
    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_UTX_BIT_CNT_B, uartCfg->txBreakBitCnt);

    writel(tmpValTxCfg, UARTx + UART_UTX_CONFIG_OFFSET);
    writel(tmpValRxCfg, UARTx + UART_URX_CONFIG_OFFSET);

    /* Configure LSB-first or MSB-first */
    tmpValTxCfg = readl(UARTx + UART_DATA_CONFIG_OFFSET);

    if (UART_MSB_FIRST == uartCfg->byteBitInverse) {
        tmpValTxCfg = reg_set_bit(tmpValTxCfg, UART_CR_UART_BIT_INV);
    } else {
        tmpValTxCfg = reg_clr_bit(tmpValTxCfg, UART_CR_UART_BIT_INV);
    }

    writel(tmpValTxCfg, UARTx + UART_DATA_CONFIG_OFFSET);

    tmpValTxCfg = readl(UARTx + UART_SW_MODE_OFFSET);
    /* Configure rx rts output SW control mode */
    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_URX_RTS_SW_MODE, uartCfg->rtsSoftwareControl);
    /* Configure tx output SW control mode */
    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_UTX_TXD_SW_MODE, uartCfg->txSoftwareControl);
    writel(tmpValTxCfg, UARTx + UART_SW_MODE_OFFSET);

    return SUCCESS;
}

int UART_DeInit(UART_ID_Type uartId)
{
    if (UART0_ID == uartId) {
        GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_UART0);
    } else if (UART1_ID == uartId) {
        GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_UART1);
    }

    return SUCCESS;
}

int UART_FifoConfig(UART_ID_Type uartId, UART_FifoCfg_Type *fifoCfg)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Deal with uart fifo configure register */
    tmpVal = readl(UARTx + UART_FIFO_CONFIG_1_OFFSET);
    /* Configure dma tx fifo threshold */
    tmpVal = reg_set_bits(tmpVal, UART_TX_FIFO_TH, fifoCfg->txFifoDmaThreshold);
    /* Configure dma rx fifo threshold */
    tmpVal = reg_set_bits(tmpVal, UART_RX_FIFO_TH, fifoCfg->rxFifoDmaThreshold);
    /* Write back */
    writel(tmpVal, UARTx + UART_FIFO_CONFIG_1_OFFSET);

    /* Enable or disable uart fifo dma function */
    tmpVal = readl(UARTx + UART_FIFO_CONFIG_0_OFFSET);

    if (ENABLE == fifoCfg->txFifoDmaEnable) {
        tmpVal = reg_set_bit(tmpVal, UART_DMA_TX_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, UART_DMA_TX_EN);
    }

    if (ENABLE == fifoCfg->rxFifoDmaEnable) {
        tmpVal = reg_set_bit(tmpVal, UART_DMA_RX_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, UART_DMA_RX_EN);
    }

    writel(tmpVal, UARTx + UART_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int UART_Enable(UART_ID_Type uartId, UART_Direction_Type direct)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    if (direct == UART_TX || direct == UART_TXRX) {
        /* Enable UART tx unit */
        tmpVal = readl(UARTx + UART_UTX_CONFIG_OFFSET);
        writel(reg_set_bit(tmpVal, UART_CR_UTX_EN), UARTx + UART_UTX_CONFIG_OFFSET);
    }

    if (direct == UART_RX || direct == UART_TXRX) {
        /* Enable UART rx unit */
        tmpVal = readl(UARTx + UART_URX_CONFIG_OFFSET);
        writel(reg_set_bit(tmpVal, UART_CR_URX_EN), UARTx + UART_URX_CONFIG_OFFSET);
    }

    return SUCCESS;
}

int UART_Disable(UART_ID_Type uartId, UART_Direction_Type direct)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    if (direct == UART_TX || direct == UART_TXRX) {
        /* Disable UART tx unit */
        tmpVal = readl(UARTx + UART_UTX_CONFIG_OFFSET);
        writel(reg_clr_bit(tmpVal, UART_CR_UTX_EN), UARTx + UART_UTX_CONFIG_OFFSET);
    }

    if (direct == UART_RX || direct == UART_TXRX) {
        /* Disable UART rx unit */
        tmpVal = readl(UARTx + UART_URX_CONFIG_OFFSET);
        writel(reg_clr_bit(tmpVal, UART_CR_URX_EN), UARTx + UART_URX_CONFIG_OFFSET);
    }

    return SUCCESS;
}

int UART_DmaEnable(UART_ID_Type uartId, UART_Direction_Type direct)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    if (direct == UART_TX || direct == UART_TXRX) {
        tmpVal = readl(UARTx + UART_FIFO_CONFIG_0_OFFSET);
        writel(reg_set_bit(tmpVal, UART_DMA_TX_EN), UARTx + UART_FIFO_CONFIG_0_OFFSET);
    }

    if (direct == UART_RX || direct == UART_TXRX) {
        tmpVal = readl(UARTx + UART_FIFO_CONFIG_0_OFFSET);
        writel(reg_set_bit(tmpVal, UART_DMA_RX_EN), UARTx + UART_FIFO_CONFIG_0_OFFSET);
    }

    return SUCCESS;
}

int UART_DmaDisable(UART_ID_Type uartId, UART_Direction_Type direct)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    if (direct == UART_TX || direct == UART_TXRX) {
        tmpVal = readl(UARTx + UART_FIFO_CONFIG_0_OFFSET);
        writel(reg_clr_bit(tmpVal, UART_DMA_TX_EN), UARTx + UART_FIFO_CONFIG_0_OFFSET);
    }

    if (direct == UART_RX || direct == UART_TXRX) {
        tmpVal = readl(UARTx + UART_FIFO_CONFIG_0_OFFSET);
        writel(reg_clr_bit(tmpVal, UART_DMA_RX_EN), UARTx + UART_FIFO_CONFIG_0_OFFSET);
    }

    return SUCCESS;
}


int UART_SetTxDataLength(UART_ID_Type uartId, uint16_t length)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_UTX_CONFIG_OFFSET);
    writel(reg_set_bits(tmpVal, UART_CR_UTX_LEN, length - 1), UARTx + UART_UTX_CONFIG_OFFSET);

    return SUCCESS;
}

int UART_SetRxDataLength(UART_ID_Type uartId, uint16_t length)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_URX_CONFIG_OFFSET);
    writel(reg_set_bits(tmpVal, UART_CR_URX_LEN, length - 1), UARTx + UART_URX_CONFIG_OFFSET);

    return SUCCESS;
}

int UART_SetRxTimeoutValue(UART_ID_Type uartId, uint8_t time)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    tmpVal = readl(UARTx + UART_URX_RTO_TIMER_OFFSET);
    tmpVal = reg_set_bits(tmpVal, UART_CR_URX_RTO_VALUE, time - 1);
    writel(tmpVal, UARTx + UART_URX_RTO_TIMER_OFFSET);

    return SUCCESS;
}

int UART_SetDeglitchCount(UART_ID_Type uartId, uint8_t deglitchCnt)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    tmpVal = readl(UARTx + UART_URX_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, UART_CR_URX_DEG_CNT, deglitchCnt);
    writel(tmpVal, UARTx + UART_URX_CONFIG_OFFSET);

    return SUCCESS;
}

int UART_ApplyAbrResult(UART_ID_Type uartId, UART_AutoBaudDetection_Type autoBaudDet)
{
    uint32_t UARTx = uartAddr[uartId];
    uint16_t tmpVal;

    tmpVal = UART_GetAutoBaudCount(uartId, autoBaudDet);

    writel(tmpVal << 0x10 | tmpVal, UARTx + UART_BIT_PRD_OFFSET);

    return SUCCESS;
}

int UART_SetRtsValue(UART_ID_Type uartId)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    tmpVal = readl(UARTx + UART_SW_MODE_OFFSET);
    writel(reg_set_bit(tmpVal, UART_CR_URX_RTS_SW_VAL), UARTx + UART_SW_MODE_OFFSET);

    return SUCCESS;
}

int UART_ClrRtsValue(UART_ID_Type uartId)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    tmpVal = readl(UARTx + UART_SW_MODE_OFFSET);
    writel(reg_clr_bit(tmpVal, UART_CR_URX_RTS_SW_VAL), UARTx + UART_SW_MODE_OFFSET);

    return SUCCESS;
}

int UART_SetTxValue(UART_ID_Type uartId)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    tmpVal = readl(UARTx + UART_SW_MODE_OFFSET);
    writel(reg_set_bit(tmpVal, UART_CR_UTX_TXD_SW_VAL), UARTx + UART_SW_MODE_OFFSET);

    return SUCCESS;
}

int UART_ClrTxValue(UART_ID_Type uartId)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    tmpVal = readl(UARTx + UART_SW_MODE_OFFSET);
    writel(reg_clr_bit(tmpVal, UART_CR_UTX_TXD_SW_VAL), UARTx + UART_SW_MODE_OFFSET);

    return SUCCESS;
}

int UART_TxFreeRun(UART_ID_Type uartId, int txFreeRun)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_UTX_CONFIG_OFFSET);

    if (ENABLE == txFreeRun) {
        writel(reg_set_bit(tmpVal, UART_CR_UTX_FRM_EN), UARTx + UART_UTX_CONFIG_OFFSET);
    } else {
        writel(reg_clr_bit(tmpVal, UART_CR_UTX_FRM_EN), UARTx + UART_UTX_CONFIG_OFFSET);
    }

    return SUCCESS;
}

int UART_AutoBaudDetection(UART_ID_Type uartId, int autoBaud)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_URX_CONFIG_OFFSET);

    if (ENABLE == autoBaud) {
        writel(reg_set_bit(tmpVal, UART_CR_URX_ABR_EN), UARTx + UART_URX_CONFIG_OFFSET);
    } else {
        writel(reg_clr_bit(tmpVal, UART_CR_URX_ABR_EN), UARTx + UART_URX_CONFIG_OFFSET);
    }

    return SUCCESS;
}

int UART_SetAllowableError0X55(UART_ID_Type uartId, uint8_t allowableError)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_URX_ABR_PW_TOL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, UART_CR_URX_ABR_PW_TOL, allowableError);
    writel(tmpVal, UARTx + UART_URX_ABR_PW_TOL_OFFSET);

    return SUCCESS;
}

int UART_GetBitWidth0X55(UART_ID_Type uartId, uint16_t *width)
{
    uint32_t i;
    uint32_t UARTx = uartAddr[uartId];

    for (i = 0; i < 4; i++) {
        width[2 * i] = (uint16_t)readl(UARTx + UART_URX_ABR_PRD_B01_OFFSET + 4 * i);
        width[2 * i + 1] = (uint16_t)(readl(UARTx + UART_URX_ABR_PRD_B01_OFFSET + 4 * i) >> 16);
    }

    return SUCCESS;
}

int UART_SetRS485(UART_ID_Type uartId, int enable, UART_RS485Polarity_Type polarity)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_UTX_RS485_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, UART_CR_UTX_RS485_EN, enable);
    tmpVal = reg_set_bits(tmpVal, UART_CR_UTX_RS485_POL, polarity);
    writel(tmpVal, UARTx + UART_UTX_RS485_CFG_OFFSET);

    return SUCCESS;
}
int UART_TxFifoClear(UART_ID_Type uartId)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_FIFO_CONFIG_0_OFFSET);
    writel(reg_set_bit(tmpVal, UART_TX_FIFO_CLR), UARTx + UART_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}


int UART_RxFifoClear(UART_ID_Type uartId)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_FIFO_CONFIG_0_OFFSET);
    writel(reg_set_bit(tmpVal, UART_RX_FIFO_CLR), UARTx + UART_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int UART_IntMask(UART_ID_Type uartId, UART_INT_Type intType, int intMask)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_INT_MASK_OFFSET);

    if (UART_INT_ALL == intType) {
        if (MASK == intMask) {
            tmpVal |= 0xfff;
        } else {
            tmpVal &= 0;
        }
    } else {
        if (MASK == intMask) {
            tmpVal |= 1 << intType;
        } else {
            tmpVal &= ~(1 << intType);
        }
    }

    writel(tmpVal, UARTx + UART_INT_MASK_OFFSET);

    return SUCCESS;
}

int UART_IntClear(UART_ID_Type uartId, UART_INT_Type intType)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_INT_CLEAR_OFFSET);

    /* Clear certain or all interrupt */
    if (UART_INT_ALL == intType) {
        tmpVal |= 0xfff;
    } else {
        tmpVal |= 1 << intType;
    }

    writel(tmpVal, UARTx + UART_INT_CLEAR_OFFSET);

    return SUCCESS;
}

int UART_SendData(UART_ID_Type uartId, uint8_t *data, uint32_t len)
{
    uint32_t txLen = 0;
    uint32_t UARTx = uartAddr[uartId];
    uint32_t timeoutCnt = UART_TX_TIMEOUT_COUNT;

    while (txLen < len) {
        if (UART_GetTxFifoCount(uartId) > 0) {
            writeb(data[txLen++], UARTx + UART_FIFO_WDATA_OFFSET);
            timeoutCnt = UART_TX_TIMEOUT_COUNT;
        } else {
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        }
    }

    return SUCCESS;
}

int UART_SendDataBlock(UART_ID_Type uartId, uint8_t *data, uint32_t len)
{
    uint32_t txLen = 0;
    uint32_t UARTx = uartAddr[uartId];
    uint32_t timeoutCnt = UART_TX_TIMEOUT_COUNT;

    while (txLen < len) {
        if (UART_GetTxFifoCount(uartId) > 0) {
            writeb(data[txLen++], UARTx + UART_FIFO_WDATA_OFFSET);
            timeoutCnt = UART_TX_TIMEOUT_COUNT;
        } else {
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        }
    }

    while (UART_GetTxBusBusyStatus(uartId) == SET) {
    }

    return SUCCESS;
}

uint32_t UART_ReceiveData(UART_ID_Type uartId, uint8_t *data, uint32_t maxLen)
{
    uint32_t rxLen = 0;
    uint32_t UARTx = uartAddr[uartId];

    while (rxLen < maxLen && UART_GetRxFifoCount(uartId) > 0) {
        data[rxLen++] = readb(UARTx + UART_FIFO_RDATA_OFFSET);
    }

    return rxLen;
}

uint16_t UART_GetAutoBaudCount(UART_ID_Type uartId, UART_AutoBaudDetection_Type autoBaudDet)
{
    uint32_t UARTx = uartAddr[uartId];

    /* Select 0x55 or start bit detection value */
    if (UART_AUTOBAUD_0X55 == autoBaudDet) {
        return readl(UARTx + UART_STS_URX_ABR_PRD_OFFSET) >> 0x10 & 0xffff;
    } else {
        return readl(UARTx + UART_STS_URX_ABR_PRD_OFFSET) & 0xffff;
    }
}

uint16_t UART_GetRxByteCount(UART_ID_Type uartId)
{
    uint32_t UARTx = uartAddr[uartId];

    return reg_get_bits(readl(UARTx + UART_URX_BCR_INT_CFG_OFFSET), UART_STS_URX_BCR_COUNT);
}
uint8_t UART_GetTxFifoCount(UART_ID_Type uartId)
{
    uint32_t UARTx = uartAddr[uartId];

    return reg_get_bits(readl(UARTx + UART_FIFO_CONFIG_1_OFFSET), UART_TX_FIFO_CNT);
}

uint8_t UART_GetRxFifoCount(UART_ID_Type uartId)
{
    uint32_t UARTx = uartAddr[uartId];

    return reg_get_bits(readl(UARTx + UART_FIFO_CONFIG_1_OFFSET), UART_RX_FIFO_CNT);
}

int UART_GetIntStatus(UART_ID_Type uartId, UART_INT_Type intType)
{
    uint32_t tmpVal, maskSet;
    uint32_t UARTx = uartAddr[uartId];

    maskSet = readl(UARTx + UART_INT_MASK_OFFSET);
    tmpVal = readl(UARTx + UART_INT_STS_OFFSET);

    if (UART_INT_ALL == intType) {
        if ((tmpVal & 0x1ff) != 0) {
            return SET;
        } else {
            return RESET;
        }
    } else {
        if ((tmpVal & (1U << intType)) != 0) {
            if ((maskSet & (1U << intType)) == 0) {
                return SET;
            } else {
                return RESET;
            }
        } else {
            return RESET;
        }
    }
}

int UART_GetTxBusBusyStatus(UART_ID_Type uartId)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_STATUS_OFFSET);

    if (reg_is_bit_set(tmpVal, UART_STS_UTX_BUS_BUSY)) {
        return SET;
    } else {
        return RESET;
    }
}

int UART_GetRxBusBusyStatus(UART_ID_Type uartId)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_STATUS_OFFSET);

    if (reg_is_bit_set(tmpVal, UART_STS_URX_BUS_BUSY)) {
        return SET;
    } else {
        return RESET;
    }
}

int UART_GetOverflowStatus(UART_ID_Type uartId, UART_Overflow_Type overflow)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    /* Get tx/rx fifo overflow or underflow status */
    tmpVal = readl(UARTx + UART_FIFO_CONFIG_0_OFFSET);

    if ((tmpVal & (1U << (overflow + 4))) != 0) {
        return SET;
    } else {
        return RESET;
    }
}

