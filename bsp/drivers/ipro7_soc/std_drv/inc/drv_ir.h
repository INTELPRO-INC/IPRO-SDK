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

#ifndef __DRV_IR_H__
#define __DRV_IR_H__

#include "ir_reg.h"
#include "drv_common.h"
#include "drv_hbn.h"

typedef enum {
    IR_TX,   /*!< IR TX direction */
    IR_RX,   /*!< IR RX direction */
    IR_TXRX, /*!< IR TX and RX direction */
} IR_Direction_Type;

typedef enum {
    IR_WORD_0, /*!< IR data word 0 */
    IR_WORD_1, /*!< IR data word 1 */
} IR_Word_Type;

typedef enum {
    IR_RX_NEC, /*!< IR RX NEC mode */
    IR_RX_RC5, /*!< IR RX RC5 mode */
    IR_RX_SWM, /*!< IR RX software pulse-width detection mode */
} IR_RxMode_Type;

typedef enum {
    IR_INT_TX,  /*!< IR TX transfer end interrupt */
    IR_INT_RX,  /*!< IR RX transfer end interrupt */
    IR_INT_ALL, /*!< IR all interrupt */
} IR_INT_Type;

typedef enum {
    IR_RX_FIFO_UNDERFLOW, /*!< Underflow flag of rx fifo */
    IR_RX_FIFO_OVERFLOW,  /*!< Overflow flag of rx fifo */
} IR_FifoStatus_Type;

typedef struct
{
    uint8_t dataBits;               /*!< Bit count of data phase */
    int tailPulseInverse;   /*!< Enable or disable signal of tail pulse inverse (don't care if SWM is enabled) */
    int tailPulse;          /*!< Enable or disable signal of tail pulse (don't care if SWM is enabled) */
    int headPulseInverse;   /*!< Enable or disable signal of head pulse inverse (don't care if SWM is enabled) */
    int headPulse;          /*!< Enable or disable signal of head pulse (don't care if SWM is enabled) */
    int logic1PulseInverse; /*!< Enable or disable signal of logic 1 pulse inverse (don't care if SWM is enabled) */
    int logic0PulseInverse; /*!< Enable or disable signal of logic 0 pulse inverse (don't care if SWM is enabled) */
    int dataPulse;          /*!< Enable or disable signal of data pulse (don't care if SWM is enabled) */
    int outputModulation;   /*!< Enable or disable signal of output modulation */
    int outputInverse;      /*!< Enable or disable signal of output inverse,0:output stays at low during idle state,1:stay at high */
} IR_TxCfg_Type;

typedef struct
{
    uint8_t logic0PulseWidth_1; /*!< Pulse width of logic 0 pulse phase 1 (don't care if SWM is enabled) */
    uint8_t logic0PulseWidth_0; /*!< Pulse width of logic 0 pulse phase 0 (don't care if SWM is enabled) */
    uint8_t logic1PulseWidth_1; /*!< Pulse width of logic 1 pulse phase 1 (don't care if SWM is enabled) */
    uint8_t logic1PulseWidth_0; /*!< Pulse width of logic 1 pulse phase 0 (don't care if SWM is enabled) */
    uint8_t headPulseWidth_1;   /*!< Pulse width of head pulse phase 1 (don't care if SWM is enabled) */
    uint8_t headPulseWidth_0;   /*!< Pulse width of head pulse phase 0 (don't care if SWM is enabled) */
    uint8_t tailPulseWidth_1;   /*!< Pulse width of tail pulse phase 1 (don't care if SWM is enabled) */
    uint8_t tailPulseWidth_0;   /*!< Pulse width of tail pulse phase 0 (don't care if SWM is enabled) */
    uint8_t moduWidth_1;        /*!< Modulation phase 1 width */
    uint8_t moduWidth_0;        /*!< Modulation phase 0 width */
    uint16_t pulseWidthUnit;    /*!< Pulse width unit */
} IR_TxPulseWidthCfg_Type;

typedef struct
{
    uint32_t swmData0; /*!< SWM pulse width data   0~7,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData1; /*!< SWM pulse width data  8~15,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData2; /*!< SWM pulse width data 16~23,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData3; /*!< SWM pulse width data 24~31,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData4; /*!< SWM pulse width data 32~39,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData5; /*!< SWM pulse width data 40~47,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData6; /*!< SWM pulse width data 48~55,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData7; /*!< SWM pulse width data 56~63,multiples of pulse width unit,each pulse is represented by 4-bit */
} IR_TxSWMPulseWidthCfg_Type;

typedef struct
{
    IR_RxMode_Type rxMode;    /*!< Set ir rx mode */
    int inputInverse; /*!< Enable or disable signal of input inverse */
    uint16_t endThreshold;    /*!< Pulse width threshold to trigger end condition */
    uint16_t dataThreshold;   /*!< Pulse width threshold for logic 0/1 detection (don't care if SWM is enabled) */
    int rxDeglitch;   /*!< Enable or disable signal of rx input de-glitch function */
    uint8_t DeglitchCnt;      /*!< De-glitch function cycle count */
} IR_RxCfg_Type;

#define IR_RX_FIFO_SIZE 64

int IR_TxInit(IR_TxCfg_Type *irTxCfg);
int IR_TxPulseWidthConfig(IR_TxPulseWidthCfg_Type *irTxPulseWidthCfg);
int IR_TxSWMPulseWidthConfig(IR_TxSWMPulseWidthCfg_Type *irTxSWMPulseWidthCfg);
int IR_RxInit(IR_RxCfg_Type *irRxCfg);
int IR_DeInit(void);
int IR_Enable(IR_Direction_Type direct);
int IR_Disable(IR_Direction_Type direct);
int IR_TxSWM(int txSWM);
int IR_RxFIFOClear(void);
int IR_SendData(IR_Word_Type irWord, uint32_t data);
int IR_SWMSendData(uint16_t *data, uint8_t length);
int IR_SendCommand(uint32_t word1, uint32_t word0);
int IR_SWMSendCommand(uint16_t *data, uint8_t length);
int IR_SendNEC(uint8_t address, uint8_t command);
int IR_IntMask(IR_INT_Type intType, int intMask);
int IR_ClrIntStatus(IR_INT_Type intType);
int IR_GetIntStatus(IR_INT_Type intType);
int IR_GetRxFIFOStatus(IR_FifoStatus_Type fifoSts);
uint32_t IR_ReceiveData(IR_Word_Type irWord);
uint8_t IR_SWMReceiveData(uint16_t *data, uint8_t length);
int IR_ReceiveNEC(uint8_t *address, uint8_t *command);
uint8_t IR_GetRxDataBitCount(void);
uint8_t IR_GetRxFIFOCount(void);
IR_RxMode_Type IR_LearnToInit(uint32_t *data, uint8_t *length);
int IR_LearnToSend(IR_RxMode_Type mode, uint32_t *data, uint8_t length);
uint8_t IR_LearnToReceive(IR_RxMode_Type mode, uint32_t *data);
int IR_LEDInit(HBN_XCLK_CLK_Type clk, uint8_t div, uint8_t unit, uint8_t code0H, uint8_t code0L, uint8_t code1H,
                       uint8_t code1L);
int IR_LEDSend(uint32_t data);

#endif /* __DRV_IR_H__ */
