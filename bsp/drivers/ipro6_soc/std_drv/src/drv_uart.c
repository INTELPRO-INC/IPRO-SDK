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

#define UART_TX_TIMEOUT_COUNT (160 * 1000)

static const uint32_t uartAddr[UART_ID_MAX] = { UART0_BASE, UART1_BASE };
#if (defined BOOTROM) || (!defined IPRO_USE_HAL_DRIVER)
static intCallback_Type *uartIntCbfArra[UART_ID_MAX][UART_INT_ALL] = {
    { NULL }
};
#endif

#if (defined BOOTROM) || (!defined IPRO_USE_HAL_DRIVER)
static void UART_IntHandler(uint8_t uartId);
#endif

#if (defined BOOTROM) || (!defined IPRO_USE_HAL_DRIVER)
static void UART_IntHandler(uint8_t uartId)
{
    uint32_t tmpVal = 0;
    uint32_t maskVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    tmpVal = readl(UARTx + UART_INT_STS_OFFSET);
    maskVal = readl(UARTx + UART_INT_MASK_OFFSET);

    /* Length of uart tx data transfer arrived interrupt */
    if (reg_is_bit_set(tmpVal, UART_UTX_END_INT) && !reg_is_bit_set(maskVal, UART_CR_UTX_END_MASK)) {
        writel(1 << UART_CR_UTX_END_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_TX_END] != NULL) {
            uartIntCbfArra[uartId][UART_INT_TX_END]();
        }
    }

    /* Length of uart rx data transfer arrived interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_END_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_END_MASK)) {
        writel(1 << UART_CR_URX_END_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_RX_END] != NULL) {
            uartIntCbfArra[uartId][UART_INT_RX_END]();
        }
    }

    /* Tx fifo ready interrupt,auto-cleared when data is pushed */
    if (reg_is_bit_set(tmpVal, UART_UTX_FRDY_INT) && !reg_is_bit_set(maskVal, UART_CR_UTX_FRDY_MASK)) {
        if (uartIntCbfArra[uartId][UART_INT_TX_FIFO_REQ] != NULL) {
            uartIntCbfArra[uartId][UART_INT_TX_FIFO_REQ]();
        }
    }

    /* Rx fifo ready interrupt,auto-cleared when data is popped */
    if (reg_is_bit_set(tmpVal, UART_URX_FRDY_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_FRDY_MASK)) {
        if (uartIntCbfArra[uartId][UART_INT_RX_FIFO_REQ] != NULL) {
            uartIntCbfArra[uartId][UART_INT_RX_FIFO_REQ]();
        }
    }

    /* Rx time-out interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_RTO_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_RTO_MASK)) {
        writel(1 << UART_CR_URX_RTO_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_RTO] != NULL) {
            uartIntCbfArra[uartId][UART_INT_RTO]();
        }
    }

    /* Rx parity check error interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_PCE_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_PCE_MASK)) {
        writel(1 << UART_CR_URX_PCE_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_PCE] != NULL) {
            uartIntCbfArra[uartId][UART_INT_PCE]();
        }
    }

    /* Tx fifo overflow/underflow error interrupt */
    if (reg_is_bit_set(tmpVal, UART_UTX_FER_INT) && !reg_is_bit_set(maskVal, UART_CR_UTX_FER_MASK)) {
        if (uartIntCbfArra[uartId][UART_INT_TX_FER] != NULL) {
            uartIntCbfArra[uartId][UART_INT_TX_FER]();
        }
    }

    /* Rx fifo overflow/underflow error interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_FER_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_FER_MASK)) {
        if (uartIntCbfArra[uartId][UART_INT_RX_FER] != NULL) {
            uartIntCbfArra[uartId][UART_INT_RX_FER]();
        }
    }

    /* Rx lin mode sync field error interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_LSE_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_LSE_MASK)) {
        writel(1 << UART_CR_URX_LSE_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_LSE] != NULL) {
            uartIntCbfArra[uartId][UART_INT_LSE]();
        }
    }

    /* Rx byte count reached interrupt */
    if (reg_is_bit_set(tmpVal, UART_URX_BCR_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_BCR_MASK)) {
        writel(1 << UART_CR_URX_BCR_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_BCR] != NULL) {
            uartIntCbfArra[uartId][UART_INT_BCR]();
        }
    }

    /* Rx auto baud rate detection finish interrupt using start bit */
    if (reg_is_bit_set(tmpVal, UART_URX_ADS_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_ADS_MASK)) {
        writel(1 << UART_CR_URX_ADS_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_STARTBIT] != NULL) {
            uartIntCbfArra[uartId][UART_INT_STARTBIT]();
        }
    }

    /* Rx auto baud rate detection finish interrupt using codeword 0x55 */
    if (reg_is_bit_set(tmpVal, UART_URX_AD5_INT) && !reg_is_bit_set(maskVal, UART_CR_URX_AD5_MASK)) {
        writel(1 << UART_CR_URX_AD5_CLR_POS, UARTx + UART_INT_CLEAR_OFFSET);

        if (uartIntCbfArra[uartId][UART_INT_0X55] != NULL) {
            uartIntCbfArra[uartId][UART_INT_0X55]();
        }
    }
}
#endif

int UART_SetBaudRate(uint8_t uartId, uint32_t baudRate)
{
    uint32_t uartClk= 0;
    uint32_t fraction = 0;
    uint32_t baudRateDivisor = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Get uart clk */
    if (uartId == UART0_ID)
        uartClk = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_UART0);
    else if (uartId == UART1_ID)
        uartClk = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_UART1);
    else
        return ERROR;

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

int UART_Init(uint8_t uartId, UART_CFG_Type *uartCfg)
{
    uint32_t tmpValTxCfg = 0;
    uint32_t tmpValRxCfg = 0;
    uint32_t fraction = 0;
    uint32_t baudRateDivisor = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Cal the baud rate divisor */
    fraction = uartCfg->uartClk * 10 / uartCfg->baudRate % 10;
    baudRateDivisor = uartCfg->uartClk / uartCfg->baudRate;

    if (fraction >= 5) {
        ++baudRateDivisor;
    }

    /* Set the baud rate register value */
    writel(((baudRateDivisor - 1) << 0x10) | ((baudRateDivisor - 1) & 0xFFFF), UARTx + UART_BIT_PRD_OFFSET);

    /* Configure parity type */
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

    /* Configure data bits */
    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_UTX_BIT_CNT_D, (uartCfg->dataBits + 4));
    tmpValRxCfg = reg_set_bits(tmpValRxCfg, UART_CR_URX_BIT_CNT_D, (uartCfg->dataBits + 4));

    /* Configure tx stop bits */
    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_UTX_BIT_CNT_P, uartCfg->stopBits);

    /* Configure tx cts flow control function */
    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_UTX_CTS_EN, uartCfg->ctsFlowControl);

    /* Configure rx input de-glitch function */
    tmpValRxCfg = reg_set_bits(tmpValRxCfg, UART_CR_URX_DEG_EN, uartCfg->rxDeglitch);

    /* Configure tx lin mode function */
    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_UTX_LIN_EN, uartCfg->txLinMode);

    /* Configure rx lin mode function */
    tmpValRxCfg = reg_set_bits(tmpValRxCfg, UART_CR_URX_LIN_EN, uartCfg->rxLinMode);

    /* Set tx break bit count for lin protocol */
    tmpValTxCfg = reg_set_bits(tmpValTxCfg, UART_CR_UTX_BIT_CNT_B, uartCfg->txBreakBitCnt);

    /* Write back */
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

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(UART0_IRQn, UART0_IRQHandler);
    Interrupt_Handler_Register(UART1_IRQn, UART1_IRQHandler);
#endif
    return SUCCESS;
}

int UART_DeInit(uint8_t uartId)
{

    switch(uartId){
        case UART0_ID:
            GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_UART0);
            break;
        case UART1_ID:
            GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_UART1);
            break;
        default:
            break;
    }

    return SUCCESS;
}

int UART_FifoConfig(uint8_t uartId, UART_FifoCfg_Type *fifoCfg)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

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

int UART_IrConfig(uint8_t uartId, UART_IrCfg_Type *irCfg)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Configure tx ir mode */
    tmpVal = readl(UARTx + UART_UTX_CONFIG_OFFSET);

    if (ENABLE == irCfg->txIrEnable) {
        tmpVal = reg_set_bit(tmpVal, UART_CR_UTX_IR_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, UART_CR_UTX_IR_EN);
    }

    if (ENABLE == irCfg->txIrInverse) {
        tmpVal = reg_set_bit(tmpVal, UART_CR_UTX_IR_INV);
    } else {
        tmpVal = reg_clr_bit(tmpVal, UART_CR_UTX_IR_INV);
    }

    writel(tmpVal, UARTx + UART_UTX_CONFIG_OFFSET);

    /* Configure rx ir mode */
    tmpVal = readl(UARTx + UART_URX_CONFIG_OFFSET);

    if (ENABLE == irCfg->rxIrEnable) {
        tmpVal = reg_set_bit(tmpVal, UART_CR_URX_IR_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, UART_CR_URX_IR_EN);
    }

    if (ENABLE == irCfg->rxIrInverse) {
        tmpVal = reg_set_bit(tmpVal, UART_CR_URX_IR_INV);
    } else {
        tmpVal = reg_clr_bit(tmpVal, UART_CR_URX_IR_INV);
    }

    writel(tmpVal, UARTx + UART_URX_CONFIG_OFFSET);

    /* Configure tx ir pulse start and stop position */
    writel(irCfg->txIrPulseStop << 0x10 | irCfg->txIrPulseStart, UARTx + UART_UTX_IR_POSITION_OFFSET);

    /* Configure rx ir pulse start position */
    writel(irCfg->rxIrPulseStart, UARTx + UART_URX_IR_POSITION_OFFSET);

    return SUCCESS;
}

int UART_Enable(uint8_t uartId, uint8_t direct)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

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

int UART_Disable(uint8_t uartId, uint8_t direct)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

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

int UART_SetTxDataLength(uint8_t uartId, uint16_t length)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Set length */
    tmpVal = readl(UARTx + UART_UTX_CONFIG_OFFSET);
    writel(reg_set_bits(tmpVal, UART_CR_UTX_LEN, length - 1), UARTx + UART_UTX_CONFIG_OFFSET);

    return SUCCESS;
}

int UART_SetRxDataLength(uint8_t uartId, uint16_t length)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Set length */
    tmpVal = readl(UARTx + UART_URX_CONFIG_OFFSET);
    writel(reg_set_bits(tmpVal, UART_CR_URX_LEN, length - 1), UARTx + UART_URX_CONFIG_OFFSET);

    return SUCCESS;
}

int UART_SetRxTimeoutValue(uint8_t uartId, uint8_t time)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    /* Check the parameters */

    /* Set time-out value */
    tmpVal = readl(UARTx + UART_URX_RTO_TIMER_OFFSET);
    tmpVal = reg_set_bits(tmpVal, UART_CR_URX_RTO_VALUE, time - 1);
    writel(tmpVal, UARTx + UART_URX_RTO_TIMER_OFFSET);

    return SUCCESS;
}

int UART_SetRxByteCount(uint8_t uartId, uint16_t count)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    /* Check the parameters */

    /* Set count value */
    tmpVal = readl(UARTx + UART_URX_BCR_INT_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, UART_CR_URX_BCR_VALUE, count);
    writel(tmpVal, UARTx + UART_URX_BCR_INT_CFG_OFFSET);

    return SUCCESS;
}

int UART_SetDeglitchCount(uint8_t uartId, uint8_t deglitchCnt)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    /* Check the parameters */

    /* Set count value */
    tmpVal = readl(UARTx + UART_URX_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, UART_CR_URX_DEG_CNT, deglitchCnt);
    writel(tmpVal, UARTx + UART_URX_CONFIG_OFFSET);

    return SUCCESS;
}

int UART_ApplyAbrResult(uint8_t uartId, uint8_t autoBaudDet)
{
    uint32_t UARTx = uartAddr[uartId];
    uint16_t tmpVal;

    /* Check the parameters */

    /* Get detection value */
    tmpVal = UART_GetAutoBaudCount(uartId, autoBaudDet);

    /* Set tx baudrate */
    writel(tmpVal << 0x10 | tmpVal, UARTx + UART_BIT_PRD_OFFSET);

    return SUCCESS;
}

int UART_SetRtsValue(uint8_t uartId)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    /* Check the parameters */

    /* Rts set 1*/
    tmpVal = readl(UARTx + UART_SW_MODE_OFFSET);
    writel(reg_set_bit(tmpVal, UART_CR_URX_RTS_SW_VAL), UARTx + UART_SW_MODE_OFFSET);

    return SUCCESS;
}

int UART_ClrRtsValue(uint8_t uartId)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    /* Check the parameters */

    /* Rts clear 0 */
    tmpVal = readl(UARTx + UART_SW_MODE_OFFSET);
    writel(reg_clr_bit(tmpVal, UART_CR_URX_RTS_SW_VAL), UARTx + UART_SW_MODE_OFFSET);

    return SUCCESS;
}

int UART_SetTxValue(uint8_t uartId)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    /* Check the parameters */

    /* Tx set 1*/
    tmpVal = readl(UARTx + UART_SW_MODE_OFFSET);
    writel(reg_set_bit(tmpVal, UART_CR_UTX_TXD_SW_VAL), UARTx + UART_SW_MODE_OFFSET);

    return SUCCESS;
}

int UART_ClrTxValue(uint8_t uartId)
{
    uint32_t UARTx = uartAddr[uartId];
    uint32_t tmpVal;

    /* Check the parameters */

    /* Rts clear 0 */
    tmpVal = readl(UARTx + UART_SW_MODE_OFFSET);
    writel(reg_clr_bit(tmpVal, UART_CR_UTX_TXD_SW_VAL), UARTx + UART_SW_MODE_OFFSET);

    return SUCCESS;
}

int UART_TxFreeRun(uint8_t uartId, int txFreeRun)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Enable or disable tx free run mode */
    tmpVal = readl(UARTx + UART_UTX_CONFIG_OFFSET);

    if (ENABLE == txFreeRun) {
        writel(reg_set_bit(tmpVal, UART_CR_UTX_FRM_EN), UARTx + UART_UTX_CONFIG_OFFSET);
    } else {
        writel(reg_clr_bit(tmpVal, UART_CR_UTX_FRM_EN), UARTx + UART_UTX_CONFIG_OFFSET);
    }

    return SUCCESS;
}

int UART_AutoBaudDetection(uint8_t uartId, int autoBaud)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Enable or disable auto baud rate detection function */
    tmpVal = readl(UARTx + UART_URX_CONFIG_OFFSET);

    if (ENABLE == autoBaud) {
        writel(reg_set_bit(tmpVal, UART_CR_URX_ABR_EN), UARTx + UART_URX_CONFIG_OFFSET);
    } else {
        writel(reg_clr_bit(tmpVal, UART_CR_URX_ABR_EN), UARTx + UART_URX_CONFIG_OFFSET);
    }

    return SUCCESS;
}

int UART_SetAllowableError0X55(uint8_t uartId, uint8_t allowableError)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameter */

    tmpVal = readl(UARTx + UART_URX_ABR_PW_TOL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, UART_CR_URX_ABR_PW_TOL, allowableError);
    writel(tmpVal, UARTx + UART_URX_ABR_PW_TOL_OFFSET);

    return SUCCESS;
}

int UART_GetBitWidth0X55(uint8_t uartId, uint16_t *width)
{
    uint32_t i;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameter */

    for (i = 0; i < 4; i++) {
        width[2 * i] = (uint16_t)readl(UARTx + UART_URX_ABR_PRD_B01_OFFSET + 4 * i);
        width[2 * i + 1] = (uint16_t)(readl(UARTx + UART_URX_ABR_PRD_B01_OFFSET + 4 * i) >> 16);
    }

    return SUCCESS;
}

int UART_SetRS485(uint8_t uartId, int enable, uint8_t polarity)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameter */

    tmpVal = readl(UARTx + UART_UTX_RS485_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, UART_CR_UTX_RS485_EN, enable);
    tmpVal = reg_set_bits(tmpVal, UART_CR_UTX_RS485_POL, polarity);
    writel(tmpVal, UARTx + UART_UTX_RS485_CFG_OFFSET);

    return SUCCESS;
}

int UART_TxFifoClear(uint8_t uartId)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameter */

    /* Clear tx fifo */
    tmpVal = readl(UARTx + UART_FIFO_CONFIG_0_OFFSET);
    writel(reg_set_bit(tmpVal, UART_TX_FIFO_CLR), UARTx + UART_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int UART_RxFifoClear(uint8_t uartId)
{
    uint32_t tmpVal = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameter */

    /* Clear rx fifo */
    tmpVal = readl(UARTx + UART_FIFO_CONFIG_0_OFFSET);
    writel(reg_set_bit(tmpVal, UART_RX_FIFO_CLR), UARTx + UART_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int UART_IntMask(uint8_t uartId, uint8_t intType, int intMask)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    tmpVal = readl(UARTx + UART_INT_MASK_OFFSET);

    /* Mask or unmask certain or all interrupt */
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

    /* Write back */
    writel(tmpVal, UARTx + UART_INT_MASK_OFFSET);

    return SUCCESS;
}

int UART_IntClear(uint8_t uartId, uint8_t intType)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    tmpVal = readl(UARTx + UART_INT_CLEAR_OFFSET);

    /* Clear certain or all interrupt */
    if (UART_INT_ALL == intType) {
        tmpVal |= 0xfff;
    } else {
        tmpVal |= 1 << intType;
    }

    /* Write back */
    writel(tmpVal, UARTx + UART_INT_CLEAR_OFFSET);

    return SUCCESS;
}

#if (defined BOOTROM) || (!defined IPRO_USE_HAL_DRIVER)
int UART_Int_Callback_Install(uint8_t uartId, uint8_t intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

    uartIntCbfArra[uartId][intType] = cbFun;

    return SUCCESS;
}
#endif

int UART_SendData(uint8_t uartId, uint8_t *data, uint32_t len)
{
    uint32_t txLen = 0;
    uint32_t UARTx = uartAddr[uartId];
    uint32_t timeoutCnt = UART_TX_TIMEOUT_COUNT;

    /* Check the parameter */

    /* Send data */
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

int UART_SendDataBlock(uint8_t uartId, uint8_t *data, uint32_t len)
{
    uint32_t txLen = 0;
    uint32_t UARTx = uartAddr[uartId];
    uint32_t timeoutCnt = UART_TX_TIMEOUT_COUNT;

    /* Check the parameter */

    /* Send data */
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

    timeoutCnt = UART_TX_TIMEOUT_COUNT;

    while (UART_GetTxBusBusyStatus(uartId) == SET) {
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    }

    return SUCCESS;
}

uint32_t UART_ReceiveData(uint8_t uartId, uint8_t *data, uint32_t maxLen)
{
    uint32_t rxLen = 0;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameter */

    /* Receive data */
    while (rxLen < maxLen && UART_GetRxFifoCount(uartId) > 0) {
        data[rxLen++] = readb(UARTx + UART_FIFO_RDATA_OFFSET);
    }

    return rxLen;
}

uint16_t UART_GetAutoBaudCount(uint8_t uartId, uint8_t autoBaudDet)
{
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameter */

    /* Select 0x55 or start bit detection value */
    if (UART_AUTOBAUD_0X55 == autoBaudDet) {
        return readl(UARTx + UART_STS_URX_ABR_PRD_OFFSET) >> 0x10 & 0xffff;
    } else {
        return readl(UARTx + UART_STS_URX_ABR_PRD_OFFSET) & 0xffff;
    }
}

uint16_t UART_GetRxByteCount(uint8_t uartId)
{
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameter */

    return reg_get_bits(readl(UARTx + UART_URX_BCR_INT_CFG_OFFSET), UART_STS_URX_BCR_COUNT);
}

uint8_t UART_GetTxFifoCount(uint8_t uartId)
{
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameter */

    return reg_get_bits(readl(UARTx + UART_FIFO_CONFIG_1_OFFSET), UART_TX_FIFO_CNT);
}

uint8_t UART_GetRxFifoCount(uint8_t uartId)
{
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameter */

    return reg_get_bits(readl(UARTx + UART_FIFO_CONFIG_1_OFFSET), UART_RX_FIFO_CNT);
}

int UART_GetIntStatus(uint8_t uartId, uint8_t intType)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Get certain or all interrupt status */
    tmpVal = readl(UARTx + UART_INT_STS_OFFSET);

    if (UART_INT_ALL == intType) {
        if ((tmpVal & 0xfff) != 0) {
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

int UART_GetTxBusBusyStatus(uint8_t uartId)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Get tx bus busy status */
    tmpVal = readl(UARTx + UART_STATUS_OFFSET);

    if (reg_is_bit_set(tmpVal, UART_STS_UTX_BUS_BUSY)) {
        return SET;
    } else {
        return RESET;
    }
}

int UART_GetRxBusBusyStatus(uint8_t uartId)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Get rx bus busy status */
    tmpVal = readl(UARTx + UART_STATUS_OFFSET);

    if (reg_is_bit_set(tmpVal, UART_STS_URX_BUS_BUSY)) {
        return SET;
    } else {
        return RESET;
    }
}

int UART_GetOverflowStatus(uint8_t uartId, uint8_t overflow)
{
    uint32_t tmpVal;
    uint32_t UARTx = uartAddr[uartId];

    /* Check the parameters */

    /* Get tx/rx fifo overflow or underflow status */
    tmpVal = readl(UARTx + UART_FIFO_CONFIG_0_OFFSET);

    if ((tmpVal & (1U << (overflow + 4))) != 0) {
        return SET;
    } else {
        return RESET;
    }
}

#if (defined BOOTROM) || (!defined IPRO_USE_HAL_DRIVER)
void UART0_IRQHandler(void)
{
    UART_IntHandler(UART0_ID);
}
#endif

#if (defined BOOTROM) || (!defined IPRO_USE_HAL_DRIVER)
void UART1_IRQHandler(void)
{
    UART_IntHandler(UART1_ID);
}
#endif

