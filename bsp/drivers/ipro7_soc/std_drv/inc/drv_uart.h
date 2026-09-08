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

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include "uart_reg.h"
#include "drv_common.h"

typedef enum {
    UART0_ID,    /*!< UART0 port define */
    UART1_ID,    /*!< UART1 port define */
    UART_ID_MAX, /*!< UART MAX ID define */
} UART_ID_Type;

typedef enum {
    UART_TX,   /*!< UART TX Direction */
    UART_RX,   /*!< UART RX Direction */
    UART_TXRX, /*!< UART TX and RX Direction */
} UART_Direction_Type;

typedef enum {
    UART_PARITY_NONE, /*!< UART parity none define */
    UART_PARITY_ODD,  /*!< UART parity odd define */
    UART_PARITY_EVEN, /*!< UART parity even define */
} UART_Parity_Type;

typedef enum {
    UART_DATABITS_5, /*!< UART data bits length:5 bits */
    UART_DATABITS_6, /*!< UART data bits length:6 bits */
    UART_DATABITS_7, /*!< UART data bits length:7 bits */
    UART_DATABITS_8, /*!< UART data bits length:8 bits */
} UART_DataBits_Type;

typedef enum {
    UART_STOPBITS_0_5, /*!< UART data stop bits length:0.5 bits */
    UART_STOPBITS_1,   /*!< UART data stop bits length:1 bits */
    UART_STOPBITS_1_5, /*!< UART data stop bits length:1.5 bits */
    UART_STOPBITS_2,   /*!< UART data stop bits length:2 bits */
} UART_StopBits_Type;

typedef enum {
    UART_LSB_FIRST, /*!< UART each byte is send out LSB-first */
    UART_MSB_FIRST, /*!< UART each byte is send out MSB-first */
} UART_ByteBitInverse_Type;

typedef enum {
    UART_AUTOBAUD_0X55,     /*!< UART auto baudrate detection using codeword 0x55 */
    UART_AUTOBAUD_STARTBIT, /*!< UART auto baudrate detection using start bit */
} UART_AutoBaudDetection_Type;

typedef enum {
    UART_RS485_DE_ACTIVE_LOW,  /*!< UART DE is active-low */
    UART_RS485_DE_ACTIVE_HIGH, /*!< UART DE is active-high */
} UART_RS485Polarity_Type;

typedef enum {
    UART_INT_TX_END,      /*!< UART tx transfer end interrupt */
    UART_INT_RX_END,      /*!< UART rx transfer end interrupt */
    UART_INT_TX_FIFO_REQ, /*!< UART tx fifo interrupt when tx fifo count reaches,auto clear */
    UART_INT_RX_FIFO_REQ, /*!< UART rx fifo interrupt when rx fifo count reaches,auto clear */
    UART_INT_RTO,         /*!< UART rx time-out interrupt */
    UART_INT_PCE,         /*!< UART rx parity check error interrupt */
    UART_INT_TX_FER,      /*!< UART tx fifo overflow/underflow error interrupt */
    UART_INT_RX_FER,      /*!< UART rx fifo overflow/underflow error interrupt */
    UART_INT_LSE,         /*!< UART rx lin mode sync field error interrupt */
    UART_INT_BCR,         /*!< UART rx byte count reached interrupt */
    UART_INT_STARTBIT,    /*!< UART rx auto baudrate detection finish interrupt using start bit */
    UART_INT_0X55,        /*!< UART rx auto baudrate detection finish interrupt using codeword 0x55 */
    UART_INT_ALL,         /*!< All the interrupt */
} UART_INT_Type;

typedef enum {
    UART_TX_OVERFLOW,  /*!< UART tx fifo overflow */
    UART_TX_UNDERFLOW, /*!< UART tx fifo underflow */
    UART_RX_OVERFLOW,  /*!< UART rx fifo overflow */
    UART_RX_UNDERFLOW, /*!< UART rx fifo underflow */
} UART_Overflow_Type;

typedef struct
{
    uint32_t uartClk;                        /*!< Uart module clock */
    uint32_t baudRate;                       /*!< Uart baudrate */
    UART_DataBits_Type dataBits;             /*!< Uart frame length of data bit */
    UART_StopBits_Type stopBits;             /*!< Uart frame length of stop bit */
    UART_Parity_Type parity;                 /*!< Uart parity check type */
    int ctsFlowControl;              /*!< Enable or disable tx CTS flow control */
    int rxDeglitch;                  /*!< Enable or disable rx input de-glitch function */
    int rtsSoftwareControl;          /*!< Enable or disable rx RTS output SW control mode */
    int txSoftwareControl;           /*!< Enable or disable tx output SW control mode */
    int txLinMode;                   /*!< Enable or disable tx LIN mode,LIN header will be sent before sending data */
    int rxLinMode;                   /*!< Enable or disable rx LIN mode,LIN header will be required and checked before receiving data */
    uint8_t txBreakBitCnt;                   /*!< Uart tx break bit count,additional 8 bit times will be added since LIN break field requires at
                                                 least 13 bit times */
    UART_ByteBitInverse_Type byteBitInverse; /*!< Uart each data byte is send out LSB-first or MSB-first */
} UART_CFG_Type;

typedef struct
{
    uint8_t txFifoDmaThreshold;  /*!< TX FIFO threshold, dma tx request will not be asserted if tx fifo count is less than this value */
    uint8_t rxFifoDmaThreshold;  /*!< RX FIFO threshold, dma rx request will not be asserted if rx fifo count is less than this value */
    int txFifoDmaEnable; /*!< Enable or disable tx dma req/ack interface */
    int rxFifoDmaEnable; /*!< Enable or disable rx dma req/ack interface */
} UART_FifoCfg_Type;

typedef struct
{
    int txIrEnable;  /*!< Enable or disable uart tx ir mode */
    int rxIrEnable;  /*!< Enable or disable uart rx ir mode */
    int txIrInverse; /*!< Enable or disable inverse signal of uart tx output in ir mode */
    int rxIrInverse; /*!< Enable or disable inverse signal of uart rx input in ir mode */
    uint16_t txIrPulseStart; /*!< Set start position of uart tx ir pulse */
    uint16_t txIrPulseStop;  /*!< Set stop position of uart tx ir pulse */
    uint16_t rxIrPulseStart; /*!< Set start position of uart rx pulse recovered from ir signal */
} UART_IrCfg_Type;

#define UART_RX_FIFO_SIZE         16
#define UART_TX_FIFO_SIZE         16
#define UART_DEFAULT_RECV_TIMEOUT 80

int UART_SetBaudRate(UART_ID_Type uartId, uint32_t baudRate);
int UART_Init(UART_ID_Type uartId, UART_CFG_Type *uartCfg);
int UART_DeInit(UART_ID_Type uartId);
int UART_FifoConfig(UART_ID_Type uartId, UART_FifoCfg_Type *fifoCfg);
int UART_IrConfig(UART_ID_Type uartId, UART_IrCfg_Type *irCfg);
int UART_Enable(UART_ID_Type uartId, UART_Direction_Type direct);
int UART_Disable(UART_ID_Type uartId, UART_Direction_Type direct);
int UART_DmaEnable(UART_ID_Type uartId, UART_Direction_Type direct);
int UART_DmaDisable(UART_ID_Type uartId, UART_Direction_Type direct);
int UART_SetTxDataLength(UART_ID_Type uartId, uint16_t length);
int UART_SetRxDataLength(UART_ID_Type uartId, uint16_t length);
int UART_SetRxTimeoutValue(UART_ID_Type uartId, uint8_t time);
int UART_SetDeglitchCount(UART_ID_Type uartId, uint8_t deglitchCnt);
int UART_ApplyAbrResult(UART_ID_Type uartId, UART_AutoBaudDetection_Type autoBaudDet);
int UART_SetRtsValue(UART_ID_Type uartId);
int UART_ClrRtsValue(UART_ID_Type uartId);
int UART_SetTxValue(UART_ID_Type uartId);
int UART_ClrTxValue(UART_ID_Type uartId);
int UART_TxFreeRun(UART_ID_Type uartId, int txFreeRun);
int UART_AutoBaudDetection(UART_ID_Type uartId, int autoBaud);
int UART_SetAllowableError0X55(UART_ID_Type uartId, uint8_t allowableError);
int UART_GetBitWidth0X55(UART_ID_Type uartId, uint16_t *width);
int UART_SetRS485(UART_ID_Type uartId, int enable, UART_RS485Polarity_Type polarity);
int UART_TxFifoClear(UART_ID_Type uartId);
int UART_RxFifoClear(UART_ID_Type uartId);
int UART_IntMask(UART_ID_Type uartId, UART_INT_Type intType, int intMask);
int UART_IntClear(UART_ID_Type uartId, UART_INT_Type intType);
int UART_SendData(UART_ID_Type uartId, uint8_t *data, uint32_t len);
int UART_SendDataBlock(UART_ID_Type uartId, uint8_t *data, uint32_t len);
uint32_t UART_ReceiveData(UART_ID_Type uartId, uint8_t *data, uint32_t maxLen);
uint16_t UART_GetAutoBaudCount(UART_ID_Type uartId, UART_AutoBaudDetection_Type autoBaudDet);
uint8_t UART_GetTxFifoCount(UART_ID_Type uartId);
uint8_t UART_GetRxFifoCount(UART_ID_Type uartId);
int UART_GetIntStatus(UART_ID_Type uartId, UART_INT_Type intType);
int UART_GetTxBusBusyStatus(UART_ID_Type uartId);
int UART_GetRxBusBusyStatus(UART_ID_Type uartId);
int UART_GetOverflowStatus(UART_ID_Type uartId, UART_Overflow_Type overflow);
uint16_t UART_GetRxByteCount(UART_ID_Type uartId);

#endif /* __DRV_UART_H__ */
