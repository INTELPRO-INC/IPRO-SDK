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

#include "drv_ir.h"
#include "drv_glb.h"

#define NEC_HEAD_H_MIN          17000
#define NEC_HEAD_H_MAX          19000
#define NEC_HEAD_L_MIN          8400
#define NEC_HEAD_L_MAX          9600
#define NEC_BIT0_H_MIN          525
#define NEC_BIT0_H_MAX          1725
#define RC5_ONE_PLUSE_MIN       1175
#define RC5_ONE_PLUSE_MAX       2375
#define RC5_TWO_PLUSE_MIN       2955
#define RC5_TWO_PLUSE_MAX       4155
#define IR_TX_INT_TIMEOUT_COUNT (100 * 160 * 1000)
#define IR_RX_INT_TIMEOUT_COUNT (100 * 160 * 1000)

int IR_TxInit(IR_TxCfg_Type *irTxCfg)
{
    uint32_t tmpVal;

    /* Disable clock gate */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IRR);

    tmpVal = readl(IR_BASE + IRTX_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_DATA_NUM, irTxCfg->dataBits - 1);
    
    /* Set tail pulse */
    ENABLE == irTxCfg->tailPulseInverse ? (tmpVal = reg_set_bit(tmpVal, IR_CR_IRTX_TAIL_HL_INV)) : (tmpVal = reg_clr_bit(tmpVal, IR_CR_IRTX_TAIL_HL_INV));
    ENABLE == irTxCfg->tailPulse ? (tmpVal = reg_set_bit(tmpVal, IR_CR_IRTX_TAIL_EN)) : (tmpVal = reg_clr_bit(tmpVal, IR_CR_IRTX_TAIL_EN));
    /* Set head pulse */
    ENABLE == irTxCfg->headPulseInverse ? (tmpVal = reg_set_bit(tmpVal, IR_CR_IRTX_HEAD_HL_INV)) : (tmpVal = reg_clr_bit(tmpVal, IR_CR_IRTX_HEAD_HL_INV));
    ENABLE == irTxCfg->headPulse ? (tmpVal = reg_set_bit(tmpVal, IR_CR_IRTX_HEAD_EN)) : (tmpVal = reg_clr_bit(tmpVal, IR_CR_IRTX_HEAD_EN));
    /* Enable or disable logic 1 and 0 pulse inverse */
    ENABLE == irTxCfg->logic1PulseInverse ? (tmpVal = reg_set_bit(tmpVal, IR_CR_IRTX_LOGIC1_HL_INV)) : (tmpVal = reg_clr_bit(tmpVal, IR_CR_IRTX_LOGIC1_HL_INV));
    ENABLE == irTxCfg->logic0PulseInverse ? (tmpVal = reg_set_bit(tmpVal, IR_CR_IRTX_LOGIC0_HL_INV)) : (tmpVal = reg_clr_bit(tmpVal, IR_CR_IRTX_LOGIC0_HL_INV));
    /* Enable or disable data pulse */
    ENABLE == irTxCfg->dataPulse ? (tmpVal = reg_set_bit(tmpVal, IR_CR_IRTX_DATA_EN)) : (tmpVal = reg_clr_bit(tmpVal, IR_CR_IRTX_DATA_EN));
    /* Enable or disable output modulation */
    ENABLE == irTxCfg->outputModulation ? (tmpVal = reg_set_bit(tmpVal, IR_CR_IRTX_MOD_EN)) : (tmpVal = reg_clr_bit(tmpVal, IR_CR_IRTX_MOD_EN));
    /* Enable or disable output inverse */
    ENABLE == irTxCfg->outputInverse ? (tmpVal = reg_set_bit(tmpVal, IR_CR_IRTX_OUT_INV)) : (tmpVal = reg_clr_bit(tmpVal, IR_CR_IRTX_OUT_INV));

    /* Write back */
    writel(tmpVal, IR_BASE + IRTX_CONFIG_OFFSET);

    return SUCCESS;
}

int IR_TxPulseWidthConfig(IR_TxPulseWidthCfg_Type *irTxPulseWidthCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(IR_BASE + IRTX_PW_OFFSET);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_LOGIC0_PH0_W, irTxPulseWidthCfg->logic0PulseWidth_0 - 1);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_LOGIC0_PH1_W, irTxPulseWidthCfg->logic0PulseWidth_1 - 1);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_LOGIC1_PH0_W, irTxPulseWidthCfg->logic1PulseWidth_0 - 1);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_LOGIC1_PH1_W, irTxPulseWidthCfg->logic1PulseWidth_1 - 1);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_HEAD_PH0_W, irTxPulseWidthCfg->headPulseWidth_0 - 1);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_HEAD_PH1_W, irTxPulseWidthCfg->headPulseWidth_1 - 1);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_TAIL_PH0_W, irTxPulseWidthCfg->tailPulseWidth_0 - 1);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_TAIL_PH1_W, irTxPulseWidthCfg->tailPulseWidth_1 - 1);
    writel(tmpVal, IR_BASE + IRTX_PW_OFFSET);

    tmpVal = readl(IR_BASE + IRTX_PULSE_WIDTH_OFFSET);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_MOD_PH0_W, irTxPulseWidthCfg->moduWidth_0 - 1);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_MOD_PH1_W, irTxPulseWidthCfg->moduWidth_1 - 1);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_PW_UNIT, irTxPulseWidthCfg->pulseWidthUnit - 1);
    writel(tmpVal, IR_BASE + IRTX_PULSE_WIDTH_OFFSET);

    return SUCCESS;
}

int IR_TxSWMPulseWidthConfig(IR_TxSWMPulseWidthCfg_Type *irTxSWMPulseWidthCfg)
{
    writel(irTxSWMPulseWidthCfg->swmData0, IR_BASE + IRTX_SWM_PW_0_OFFSET);
    writel(irTxSWMPulseWidthCfg->swmData1, IR_BASE + IRTX_SWM_PW_1_OFFSET);
    writel(irTxSWMPulseWidthCfg->swmData2, IR_BASE + IRTX_SWM_PW_2_OFFSET);
    writel(irTxSWMPulseWidthCfg->swmData3, IR_BASE + IRTX_SWM_PW_3_OFFSET);
    writel(irTxSWMPulseWidthCfg->swmData4, IR_BASE + IRTX_SWM_PW_4_OFFSET);
    writel(irTxSWMPulseWidthCfg->swmData5, IR_BASE + IRTX_SWM_PW_5_OFFSET);
    writel(irTxSWMPulseWidthCfg->swmData6, IR_BASE + IRTX_SWM_PW_6_OFFSET);
    writel(irTxSWMPulseWidthCfg->swmData7, IR_BASE + IRTX_SWM_PW_7_OFFSET);

    return SUCCESS;
}

int IR_RxInit(IR_RxCfg_Type *irRxCfg)
{
    uint32_t tmpVal;

    /* Disable clock gate */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IRR);

    tmpVal = readl(IR_BASE + IRRX_CONFIG_OFFSET);

    /* Set rx mode */
    switch (irRxCfg->rxMode) {
        case IR_RX_NEC:
            tmpVal = reg_set_bits(tmpVal, IR_CR_IRRX_MODE, 0x0);
            break;

        case IR_RX_RC5:
            tmpVal = reg_set_bits(tmpVal, IR_CR_IRRX_MODE, 0x1);
            break;

        case IR_RX_SWM:
            tmpVal = reg_set_bits(tmpVal, IR_CR_IRRX_MODE, 0x2);
            break;

        default:
            break;
    }

    /* Enable or disable input inverse */
    ENABLE == irRxCfg->inputInverse ? (tmpVal = reg_set_bit(tmpVal, IR_CR_IRRX_IN_INV)) : (tmpVal = reg_clr_bit(tmpVal, IR_CR_IRRX_IN_INV));
    /* Enable or disable rx input de-glitch function */
    ENABLE == irRxCfg->rxDeglitch ? (tmpVal = reg_set_bit(tmpVal, IR_CR_IRRX_DEG_EN)) : (tmpVal = reg_clr_bit(tmpVal, IR_CR_IRRX_DEG_EN));
    /* Set de-glitch function cycle count */
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRRX_DEG_CNT, irRxCfg->DeglitchCnt);
    /* Write back */
    writel(tmpVal, IR_BASE + IRRX_CONFIG_OFFSET);

    tmpVal = readl(IR_BASE + IRRX_PW_CONFIG_OFFSET);
    /* Set pulse width threshold to trigger end condition */
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRRX_END_TH, irRxCfg->endThreshold - 1);
    /* Set pulse width threshold for logic0/1 detection */
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRRX_DATA_TH, irRxCfg->dataThreshold - 1);
    /* Write back */
    writel(tmpVal, IR_BASE + IRRX_PW_CONFIG_OFFSET);

    return SUCCESS;
}

int IR_DeInit(void)
{
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_IR_REMOTE);

    return SUCCESS;
}

int IR_Enable(IR_Direction_Type direct)
{
    uint32_t tmpVal;

    if (direct == IR_TX || direct == IR_TXRX) {
        /* Enable ir tx unit */
        tmpVal = readl(IR_BASE + IRTX_CONFIG_OFFSET);
        writel(reg_set_bit(tmpVal, IR_CR_IRTX_EN), IR_BASE + IRTX_CONFIG_OFFSET);
    }

    if (direct == IR_RX || direct == IR_TXRX) {
        /* Enable ir rx unit */
        tmpVal = readl(IR_BASE + IRRX_CONFIG_OFFSET);
        writel(reg_set_bit(tmpVal, IR_CR_IRRX_EN), IR_BASE + IRRX_CONFIG_OFFSET);
    }

    return SUCCESS;
}

int IR_Disable(IR_Direction_Type direct)
{
    uint32_t tmpVal;

    if (direct == IR_TX || direct == IR_TXRX) {
        /* Disable ir tx unit */
        tmpVal = readl(IR_BASE + IRTX_CONFIG_OFFSET);
        writel(reg_clr_bit(tmpVal, IR_CR_IRTX_EN), IR_BASE + IRTX_CONFIG_OFFSET);
    }

    if (direct == IR_RX || direct == IR_TXRX) {
        /* Disable ir rx unit */
        tmpVal = readl(IR_BASE + IRRX_CONFIG_OFFSET);
        writel(reg_clr_bit(tmpVal, IR_CR_IRRX_EN), IR_BASE + IRRX_CONFIG_OFFSET);
    }

    return SUCCESS;
}

int IR_TxSWM(int txSWM)
{
    uint32_t tmpVal;

    /* Enable or disable tx swm */
    tmpVal = readl(IR_BASE + IRTX_CONFIG_OFFSET);

    if (ENABLE == txSWM) {
        writel(reg_set_bit(tmpVal, IR_CR_IRTX_SWM_EN), IR_BASE + IRTX_CONFIG_OFFSET);
    } else {
        writel(reg_clr_bit(tmpVal, IR_CR_IRTX_SWM_EN), IR_BASE + IRTX_CONFIG_OFFSET);
    }

    return SUCCESS;
}

int IR_RxFIFOClear(void)
{
    uint32_t tmpVal;

    tmpVal = readl(IR_BASE + IRRX_SWM_FIFO_CONFIG_0_OFFSET);
    writel(reg_set_bit(tmpVal, IR_RX_FIFO_CLR), IR_BASE + IRRX_SWM_FIFO_CONFIG_0_OFFSET);

    return SUCCESS;
}

int IR_SendData(IR_Word_Type irWord, uint32_t data)
{
    if (IR_WORD_0 == irWord) {
        writel(data, IR_BASE + IRTX_DATA_WORD0_OFFSET);
    } else {
        writel(data, IR_BASE + IRTX_DATA_WORD1_OFFSET);
    }

    return SUCCESS;
}

int IR_SWMSendData(uint16_t *data, uint8_t length)
{
    uint8_t i = 0, j = 0;
    uint16_t minData = data[0];
    uint32_t tmpVal;
    uint32_t pwVal = 0;
    uint32_t count = (length + 7) / 8;

    /* Search for min value */
    for (i = 1; i < length; i++) {
        if (minData > data[i] && data[i] != 0) {
            minData = data[i];
        }
    }

    /* Set pulse width unit */
    tmpVal = readl(IR_BASE + IRTX_PULSE_WIDTH_OFFSET);
    writel(reg_set_bits(tmpVal, IR_CR_IRTX_PW_UNIT, minData), IR_BASE + IRTX_PULSE_WIDTH_OFFSET);

    /* Set tx SWM pulse width data as multiples of pulse width unit */
    for (i = 0; i < count; i++) {
        pwVal = 0;

        if (i < count - 1) {
            for (j = 0; j < 8; j++) {
                tmpVal = ((2 * data[j + i * 8] + minData) / (2 * minData) - 1) & 0xf;
                pwVal |= tmpVal << (4 * j);
            }
            writel(pwVal, IR_BASE + IRTX_SWM_PW_0_OFFSET + i * 4);
        } else {
            for (j = 0; j < length % 8; j++) {
                tmpVal = ((2 * data[j + i * 8] + minData) / (2 * minData) - 1) & 0xf;
                pwVal |= tmpVal << (4 * j);
            }
            writel(pwVal, IR_BASE + IRTX_SWM_PW_0_OFFSET + i * 4);
        }
    }

    return SUCCESS;
}

int IR_SendCommand(uint32_t word1, uint32_t word0)
{
    uint32_t timeoutCnt = IR_TX_INT_TIMEOUT_COUNT;

    /* Write data */
    IR_SendData(IR_WORD_1, word1);
    IR_SendData(IR_WORD_0, word0);

    /* Mask tx interrupt */
    IR_IntMask(IR_INT_TX, MASK);

    /* Clear tx interrupt */
    IR_ClrIntStatus(IR_INT_TX);

    /* Enable ir tx */
    IR_Enable(IR_TX);

    /* Wait for tx interrupt */
    while (SET != IR_GetIntStatus(IR_INT_TX)) {
        timeoutCnt--;

        if (timeoutCnt == 0) {
            IR_Disable(IR_TX);

            return TIMEOUT;
        }
    }

    /* Disable ir tx */
    IR_Disable(IR_TX);

    /* Clear tx interrupt */
    IR_ClrIntStatus(IR_INT_TX);

    return SUCCESS;
}

int IR_SWMSendCommand(uint16_t *data, uint8_t length)
{
    uint32_t timeoutCnt = IR_TX_INT_TIMEOUT_COUNT;

    /* Write fifo */
    IR_SWMSendData(data, length);

    /* Mask tx interrupt */
    IR_IntMask(IR_INT_TX, MASK);

    /* Clear tx interrupt */
    IR_ClrIntStatus(IR_INT_TX);

    /* Enable ir tx */
    IR_Enable(IR_TX);

    /* Wait for tx interrupt */
    while (SET != IR_GetIntStatus(IR_INT_TX)) {
        timeoutCnt--;

        if (timeoutCnt == 0) {
            IR_Disable(IR_TX);

            return TIMEOUT;
        }
    }

    /* Disable ir tx */
    IR_Disable(IR_TX);

    /* Clear tx interrupt */
    IR_ClrIntStatus(IR_INT_TX);

    return SUCCESS;
}

int IR_SendNEC(uint8_t address, uint8_t command)
{
    uint32_t tmpVal = ((~command & 0xff) << 24) + (command << 16) + ((~address & 0xff) << 8) + address;

    IR_SendCommand(0, tmpVal);

    return SUCCESS;
}

int IR_IntMask(IR_INT_Type intType, int intMask)
{
    uint32_t tmpVal;

    if (intType == IR_INT_TX || intType == IR_INT_ALL) {
        /* Mask or unmask tx interrupt */
        tmpVal = readl(IR_BASE + IRTX_INT_STS_OFFSET);
        writel(reg_set_bits(tmpVal, IR_CR_IRTX_END_MASK, intMask), IR_BASE + IRTX_INT_STS_OFFSET);
    }

    if (intType == IR_INT_RX || intType == IR_INT_ALL) {
        /* Mask or unmask rx interrupt */
        tmpVal = readl(IR_BASE + IRRX_INT_STS_OFFSET);
        writel(reg_set_bits(tmpVal, IR_CR_IRRX_END_MASK, intMask), IR_BASE + IRRX_INT_STS_OFFSET);
    }

    return SUCCESS;
}

int IR_ClrIntStatus(IR_INT_Type intType)
{
    uint32_t tmpVal;

    if (intType == IR_INT_TX || intType == IR_INT_ALL) {
        /* Clear tx interrupt */
        tmpVal = readl(IR_BASE + IRTX_INT_STS_OFFSET);
        writel(reg_set_bit(tmpVal, IR_CR_IRTX_END_CLR), IR_BASE + IRTX_INT_STS_OFFSET);
    }

    if (intType == IR_INT_RX || intType == IR_INT_ALL) {
        /* Clear rx interrupt */
        tmpVal = readl(IR_BASE + IRRX_INT_STS_OFFSET);
        writel(reg_set_bit(tmpVal, IR_CR_IRRX_END_CLR), IR_BASE + IRRX_INT_STS_OFFSET);
    }

    return SUCCESS;
}

int IR_GetIntStatus(IR_INT_Type intType)
{
    uint32_t tmpVal = 0;

    /* Read tx or rx interrupt status */
    if (IR_INT_TX == intType) {
        tmpVal = readl(IR_BASE + IRTX_INT_STS_OFFSET);
        tmpVal = reg_get_bits(tmpVal, IRTX_END_INT);
    } else if (IR_INT_RX == intType) {
        tmpVal = readl(IR_BASE + IRRX_INT_STS_OFFSET);
        tmpVal = reg_get_bits(tmpVal, IRRX_END_INT);
    }

    if (tmpVal) {
        return SET;
    } else {
        return RESET;
    }
}

int IR_GetRxFIFOStatus(IR_FifoStatus_Type fifoSts)
{
    uint32_t tmpVal;

    /* Read rx fifo status */
    tmpVal = readl(IR_BASE + IRRX_SWM_FIFO_CONFIG_0_OFFSET);

    if (fifoSts == IR_RX_FIFO_UNDERFLOW) {
        tmpVal = reg_get_bits(tmpVal, IR_RX_FIFO_UNDERFLOW);
    } else {
        tmpVal = reg_get_bits(tmpVal, IR_RX_FIFO_OVERFLOW);
    }

    if (tmpVal) {
        return SET;
    } else {
        return RESET;
    }
}

uint32_t IR_ReceiveData(IR_Word_Type irWord)
{
    uint32_t tmpVal;

    if (IR_WORD_0 == irWord) {
        tmpVal = readl(IR_BASE + IRRX_DATA_WORD0_OFFSET);
    } else {
        tmpVal = readl(IR_BASE + IRRX_DATA_WORD1_OFFSET);
    }

    return tmpVal;
}

uint8_t IR_SWMReceiveData(uint16_t *data, uint8_t length)
{
    uint8_t rxLen = 0;

    while (rxLen < length && IR_GetRxFIFOCount() > 0) {
        data[rxLen++] = readl(IR_BASE + IRRX_SWM_FIFO_RDATA_OFFSET) & 0xffff;
    }

    return rxLen;
}

int IR_ReceiveNEC(uint8_t *address, uint8_t *command)
{
    uint32_t tmpVal = IR_ReceiveData(IR_WORD_0);

    *address = tmpVal & 0xff;
    *command = (tmpVal >> 16) & 0xff;

    if ((~(*address) & 0xff) != ((tmpVal >> 8) & 0xff) || (~(*command) & 0xff) != ((tmpVal >> 24) & 0xff)) {
        return ERROR;
    }

    return SUCCESS;
}

uint8_t IR_GetRxDataBitCount(void)
{
    uint32_t tmpVal;

    tmpVal = readl(IR_BASE + IRRX_DATA_COUNT_OFFSET);
    tmpVal = reg_get_bits(tmpVal, IR_STS_IRRX_DATA_CNT);

    return tmpVal;
}

uint8_t IR_GetRxFIFOCount(void)
{
    uint32_t tmpVal;

    tmpVal = readl(IR_BASE + IRRX_SWM_FIFO_CONFIG_0_OFFSET);
    tmpVal = reg_get_bits(tmpVal, IR_RX_FIFO_CNT);

    return tmpVal;
}

IR_RxMode_Type IR_LearnToInit(uint32_t *data, uint8_t *length)
{
    uint32_t tmpVal;
    uint32_t timeoutCnt = IR_RX_INT_TIMEOUT_COUNT;

    /* Disable clock gate */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IRR);

    /* Disable rx,set rx in software mode and enable rx input inverse */
    tmpVal = readl(IR_BASE + IRRX_CONFIG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, IR_CR_IRRX_EN);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRRX_MODE, 0x2);
    tmpVal = reg_set_bit(tmpVal, IR_CR_IRRX_IN_INV);
    writel(tmpVal, IR_BASE + IRRX_CONFIG_OFFSET);
    /* Set pulse width threshold to trigger end condition */
    tmpVal = readl(IR_BASE + IRRX_PW_CONFIG_OFFSET);
    writel(reg_set_bits(tmpVal, IR_CR_IRRX_END_TH, 19999), IR_BASE + IRRX_PW_CONFIG_OFFSET);

    /* Clear and mask rx interrupt */
    tmpVal = readl(IR_BASE + IRRX_INT_STS_OFFSET);
    tmpVal = reg_set_bit(tmpVal, IR_CR_IRRX_END_MASK);
    writel(reg_set_bit(tmpVal, IR_CR_IRRX_END_CLR), IR_BASE + IRRX_INT_STS_OFFSET);

    /* Enable rx */
    tmpVal = readl(IR_BASE + IRRX_CONFIG_OFFSET);
    writel(reg_set_bit(tmpVal, IR_CR_IRRX_EN), IR_BASE + IRRX_CONFIG_OFFSET);

    /* Wait for rx interrupt */
    while (SET != IR_GetIntStatus(IR_INT_RX)) {
        timeoutCnt--;

        if (timeoutCnt == 0) {
            IR_Disable(IR_RX);

            return IR_RX_SWM;
        }
    }

    /* Disable rx */
    tmpVal = readl(IR_BASE + IRRX_CONFIG_OFFSET);
    writel(reg_clr_bit(tmpVal, IR_CR_IRRX_EN), IR_BASE + IRRX_CONFIG_OFFSET);

    /* Clear rx interrupt */
    tmpVal = readl(IR_BASE + IRRX_INT_STS_OFFSET);
    writel(reg_set_bit(tmpVal, IR_CR_IRRX_END_CLR), IR_BASE + IRRX_INT_STS_OFFSET);

    /*Receive data */
    *length = IR_GetRxFIFOCount();
    *length = IR_SWMReceiveData((uint16_t *)data, *length);

    /* Judge protocol type */
    if (NEC_HEAD_H_MIN < (data[0] & 0xffff) && (data[0] & 0xffff) < NEC_HEAD_H_MAX && NEC_HEAD_L_MIN < (data[0] >> 16) && (data[0] >> 16) < NEC_HEAD_L_MAX && NEC_BIT0_H_MIN < (data[1] & 0xffff) && (data[1] & 0xffff) < NEC_BIT0_H_MAX) {
        /* Set rx in NEC mode */
        tmpVal = readl(IR_BASE + IRRX_CONFIG_OFFSET);
        writel(reg_set_bits(tmpVal, IR_CR_IRRX_MODE, 0x0), IR_BASE + IRRX_CONFIG_OFFSET);
        /* Set pulse width threshold to trigger end condition and pulse width threshold for logic0/1 detection */
        writel(0x23270d47, IR_BASE + IRRX_PW_CONFIG_OFFSET);
        /* Set tx in NEC mode */
        /* Tx configure */
        writel(0x1f514, IR_BASE + IRTX_CONFIG_OFFSET);
        /* Set logic 0,logic 1,head and tail pulse width */
        writel(0x7f2000, IR_BASE + IRTX_PW_OFFSET);
        /* Set modulation phase width and pulse width unit */
        writel(0x22110464, IR_BASE + IRTX_PULSE_WIDTH_OFFSET);

        return IR_RX_NEC;
    } else if (RC5_ONE_PLUSE_MIN < (data[0] & 0xffff) && (data[0] & 0xffff) < RC5_ONE_PLUSE_MAX && ((RC5_ONE_PLUSE_MIN < (data[0] >> 16) && (data[0] >> 16) < RC5_ONE_PLUSE_MAX) || (RC5_TWO_PLUSE_MIN < (data[0] >> 16) && (data[0] >> 16) < RC5_TWO_PLUSE_MAX)) &&
               ((RC5_ONE_PLUSE_MIN < (data[1] & 0xffff) && (data[1] & 0xffff) < RC5_ONE_PLUSE_MAX) || (RC5_TWO_PLUSE_MIN < (data[1] & 0xffff) && (data[1] & 0xffff) < RC5_TWO_PLUSE_MAX))) {
        /* Set rx in RC-5 mode */
        tmpVal = readl(IR_BASE + IRRX_CONFIG_OFFSET);
        writel(reg_set_bits(tmpVal, IR_CR_IRRX_MODE, 0x1), IR_BASE + IRRX_CONFIG_OFFSET);
        /* Set pulse width threshold to trigger end condition and pulse width threshold for logic0/1 detection */
        writel(0x13870a6a, IR_BASE + IRRX_PW_CONFIG_OFFSET);
        /* Set tx in RC-5 mode */
        /* Tx configure */
        writel(0xc134, IR_BASE + IRTX_CONFIG_OFFSET);
        /* Set logic 0,logic 1,head and tail pulse width */
        writel(0, IR_BASE + IRTX_PW_OFFSET);
        /* Set modulation phase width and pulse width unit */
        writel(0x221106f1, IR_BASE + IRTX_PULSE_WIDTH_OFFSET);

        return IR_RX_RC5;
    } else if ((data[0] >> 16) != 0) {
        /* Set tx in software mode */
        /* Tx configure */
        writel(*length << 12 | 0xc, IR_BASE + IRTX_CONFIG_OFFSET);
        /* Set modulation phase width */
        writel(0x22110000, IR_BASE + IRTX_PULSE_WIDTH_OFFSET);

        return IR_RX_SWM;
    } else {
        tmpVal = readl(IR_BASE + IRRX_CONFIG_OFFSET);
        tmpVal = reg_get_bits(tmpVal, IR_CR_IRRX_MODE);

        if (tmpVal == 0) {
            return IR_RX_NEC;
        } else if (tmpVal == 1) {
            return IR_RX_RC5;
        } else {
            return IR_RX_SWM;
        }
    }
}

uint8_t IR_LearnToReceive(IR_RxMode_Type mode, uint32_t *data)
{
    uint8_t length = 0;
    uint32_t timeoutCnt = IR_RX_INT_TIMEOUT_COUNT;

    /* Disable ir rx */
    IR_Disable(IR_RX);

    /* Clear and mask rx interrupt */
    IR_ClrIntStatus(IR_INT_RX);
    IR_IntMask(IR_INT_RX, MASK);

    /* Enable ir rx */
    IR_Enable(IR_RX);

    /* Wait for rx interrupt */
    while (SET != IR_GetIntStatus(IR_INT_RX)) {
        timeoutCnt--;

        if (timeoutCnt == 0) {
            IR_Disable(IR_RX);

            return TIMEOUT;
        }
    }

    /* Disable ir rx */
    IR_Disable(IR_RX);

    /* Clear rx interrupt */
    IR_ClrIntStatus(IR_INT_RX);

    /* Receive data according to mode */
    if (mode == IR_RX_NEC || mode == IR_RX_RC5) {
        /* Get data bit count */
        length = IR_GetRxDataBitCount();
        data[0] = IR_ReceiveData(IR_WORD_0);
    } else {
        /* Get fifo count */
        length = IR_GetRxFIFOCount();
        length = IR_SWMReceiveData((uint16_t *)data, length);
    }

    return length;
}

int IR_LearnToSend(IR_RxMode_Type mode, uint32_t *data, uint8_t length)
{
    uint32_t tmpVal;

    /* Set send length */
    tmpVal = readl(IR_BASE + IRTX_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, IR_CR_IRTX_DATA_NUM, length - 1);
    writel(tmpVal, IR_BASE + IRTX_CONFIG_OFFSET);

    if (mode == IR_RX_NEC || mode == IR_RX_RC5) {
        IR_SendCommand(0, data[0]);
    } else {
        IR_SWMSendCommand((uint16_t *)data, length);
    }

    return SUCCESS;
}

int IR_LEDInit(HBN_XCLK_CLK_Type clk, uint8_t div, uint8_t unit, uint8_t code0H, uint8_t code0L, uint8_t code1H, uint8_t code1L)
{
    IR_TxCfg_Type txCfg = {
        24,      /* 24-bit data */
        DISABLE, /* Disable signal of tail pulse inverse */
        DISABLE, /* Disable signal of tail pulse */
        DISABLE, /* Disable signal of head pulse inverse */
        DISABLE, /* Disable signal of head pulse */
        DISABLE, /* Disable signal of logic 1 pulse inverse */
        DISABLE, /* Disable signal of logic 0 pulse inverse */
        ENABLE,  /* Enable signal of data pulse */
        DISABLE, /* Disable signal of output modulation */
        ENABLE   /* Enable signal of output inverse */
    };

    IR_TxPulseWidthCfg_Type txPWCfg = {
        code0L, /* Pulse width of logic 0 pulse phase 1 */
        code0H, /* Pulse width of logic 0 pulse phase 0 */
        code1L, /* Pulse width of logic 1 pulse phase 1 */
        code1H, /* Pulse width of logic 1 pulse phase 0 */
        1,      /* Pulse width of head pulse phase 1 */
        1,      /* Pulse width of head pulse phase 0 */
        1,      /* Pulse width of tail pulse phase 1 */
        1,      /* Pulse width of tail pulse phase 0 */
        1,      /* Modulation phase 1 width */
        1,      /* Modulation phase 0 width */
        unit    /* Pulse width unit */
    };

    HBN_Set_XCLK_CLK_Sel(clk);
    GLB_Set_IR_CLK(ENABLE, GLB_IR_CLK_SRC_XCLK, div - 1);

    /* Disable ir before config */
    IR_Disable(IR_TXRX);

    /* IR tx init */
    IR_TxInit(&txCfg);
    IR_TxPulseWidthConfig(&txPWCfg);

    return SUCCESS;
}

int IR_LEDSend(uint32_t data)
{
    /* Change MSB_first to LSB_first */
    data = ((data >> 1) & 0x55555555) | ((data << 1) & 0xaaaaaaaa);
    data = ((data >> 2) & 0x33333333) | ((data << 2) & 0xcccccccc);
    data = ((data >> 4) & 0x0f0f0f0f) | ((data << 4) & 0xf0f0f0f0);
    data = ((data >> 16) & 0xff) | (data & 0xff00) | ((data << 16) & 0xff0000);
    IR_SendCommand(0, data);

    return SUCCESS;
}

