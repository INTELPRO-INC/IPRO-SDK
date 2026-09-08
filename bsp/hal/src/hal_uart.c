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

#include "hal_uart.h"
#include "hal_dma.h"
#include "drv_uart.h"
#include "drv_glb.h"
#include "drv_clock.h"
#include "uart_config.h"
#include "compiler/common.h"

static void UART0_IRQ(void);
static void UART1_IRQ(void);

static ATTR_OCRAM_SECTION uart_device_t uart_devices[UART_MAX_INDEX] = {
    UART0_CONFIG,
    UART1_CONFIG,
};

uart_device_t* uart_getDevice(int uart_id)
{
    return &uart_devices[uart_id];
}

void uart_set_baudrate(int uart_id, int baudrate)
{
    uart_devices[uart_id].baudrate = baudrate;
}

int uart_init(int uart_id, uint16_t caps)
{
    uart_device_t *uart_device = &uart_devices[uart_id];

    UART_FifoCfg_Type fifoCfg = { 0 };
    UART_CFG_Type uart_cfg = { 0 };

    uart_device->caps = caps;
    uart_device->tx_dma = -1;
    uart_device->rx_dma = -1;

    UART_IntMask(uart_device->id, UART_INT_ALL, MASK);
    UART_Disable(uart_device->id, UART_TXRX);

    // UART APB Clock Issue work-around: uart use an APB clock which only work on register read/write
    // IPRO7-ONLY: 0x3000006c is UART0's APB clock control on IPRO7. On IPRO6 the
    // same address is GLB_CORE_CFG23 (reserved); poking bit 11 corrupts the
    // SF_Ctrl/flash path and hangs the next flash access (e.g. boot2 flash_init).
#if defined(CONFIG_IPRO7)
    if (uart_id == UART0_ID)
        writel(readl(0x3000006c) | (0x1 << 11), 0x3000006c);
#endif

    uint32_t uart_clk = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_UART0 + uart_device->id);

    uart_cfg.baudRate = uart_device->baudrate;
    uart_cfg.dataBits = uart_device->databits;
    uart_cfg.stopBits = uart_device->stopbits;
    uart_cfg.parity = uart_device->parity;
    uart_cfg.uartClk = uart_clk;
    uart_cfg.ctsFlowControl = UART_CTS_FLOWCONTROL_ENABLE;
    uart_cfg.rtsSoftwareControl = UART_RTS_FLOWCONTROL_ENABLE;
    uart_cfg.byteBitInverse = UART_MSB_FIRST_ENABLE;
    uart_cfg.txSoftwareControl = UART_TX_SWCONTROL_ENABLE;
    uart_cfg.txLinMode = UART_TX_LINMODE_ENABLE;
    uart_cfg.rxLinMode = UART_RX_LINMODE_ENABLE;
    uart_cfg.txBreakBitCnt = UART_TX_BREAKBIT_CNT;
    uart_cfg.rxDeglitch = ENABLE;

    /* uart init with default configuration */
    UART_Init(uart_device->id, &uart_cfg);

    /* Enable tx free run mode */
    UART_TxFreeRun(uart_device->id, ENABLE);
    /*set de-glitch function cycle count value*/
    UART_SetDeglitchCount(uart_device->id, 2);

    /* Set rx time-out value */
    UART_SetRxTimeoutValue(uart_device->id, UART_DEFAULT_RTO_TIMEOUT);

    fifoCfg.txFifoDmaThreshold = uart_device->fifo_threshold;
    fifoCfg.txFifoDmaEnable = DISABLE;
    fifoCfg.rxFifoDmaThreshold = uart_device->fifo_threshold;
    fifoCfg.rxFifoDmaEnable = DISABLE;

    if (caps & COMMON_CAPS_DMA_TX) {
        fifoCfg.txFifoDmaEnable = ENABLE;
    }
    if (caps & COMMON_CAPS_DMA_RX) {
        fifoCfg.rxFifoDmaEnable = ENABLE;
    }

    UART_FifoConfig(uart_device->id, &fifoCfg);

    UART_TxFifoClear(uart_device->id);
    UART_RxFifoClear(uart_device->id);

    UART_Enable(uart_device->id, UART_TXRX);

    if (uart_device->id == 0)
        Interrupt_Handler_Register(UART0_IRQn, UART0_IRQ);
    if (uart_device->id == 1)
        Interrupt_Handler_Register(UART1_IRQn, UART1_IRQ);

    return 0;
}

int uart_uninit(int uart_id)
{
    if (uart_id == 0) {
        writel(readl(0x3000006c) & ~(0x1 << 11), 0x3000006c);
        GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_UART0);
    } else if (uart_id == 1) {
        GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_UART1);
    }
    return 0;
}

void uart_set_callback(int uart_id, void *args)
{
    uart_device_t *uart_device = &uart_devices[uart_id];

    uart_device->callback = args;
}

int uart_ioctl(int uart_id, int cmd, void *args)
{
    uart_device_t *uart_device = &uart_devices[uart_id];

    switch (cmd) {
        case UART_CTRL_SET_INT:
        {
            uint32_t offset = __builtin_ctz((uint32_t)args);
            while (offset < 9) {
                if ((uint32_t)args & (1 << offset)) {
                    UART_IntMask(uart_device->id, offset, UNMASK);
                }
                offset++;
            }
            if (uart_id == UART0_ID)
                CPU_Interrupt_Enable(UART0_IRQn);
            else if (uart_id == UART1_ID)
                CPU_Interrupt_Enable(UART1_IRQn);

            break;
        }
        case UART_CTRL_CLR_INT:
        {
            uint32_t offset = __builtin_ctz((uint32_t)args);
            while (offset < 9) {
                if ((uint32_t)args & (1 << offset)) {
                    UART_IntMask(uart_device->id, offset, MASK);
                }
                offset++;
            }
            if (uart_device->id == UART0_ID)
                CPU_Interrupt_Disable(UART0_IRQn);
            else if (uart_device->id == UART1_ID)
                CPU_Interrupt_Disable(UART1_IRQn);

            break;
        }
        case UART_CTRL_RESUME:
            UART_Enable(uart_device->id, UART_TXRX);
            break;
        case UART_CTRL_SUSPEND:
            UART_Disable(uart_device->id, UART_TXRX);
            break;
        case UART_CTRL_CONFIG:
        {
            uart_param_cfg_t *cfg = (uart_param_cfg_t *)args;
            UART_CFG_Type uart_cfg = { 0 };

            UART_Disable(uart_device->id, UART_TXRX);

            uint32_t uart_clk = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_UART0 + uart_device->id);

            uart_cfg.uartClk = uart_clk;
            uart_cfg.baudRate = cfg->baudrate;
            uart_cfg.stopBits = cfg->stopbits;
            uart_cfg.parity = cfg->parity;
            uart_cfg.dataBits = cfg->databits;
            uart_cfg.ctsFlowControl = UART_CTS_FLOWCONTROL_ENABLE;
            uart_cfg.rtsSoftwareControl = UART_RTS_FLOWCONTROL_ENABLE;
            uart_cfg.byteBitInverse = UART_MSB_FIRST_ENABLE;
            uart_cfg.txSoftwareControl = UART_TX_SWCONTROL_ENABLE;
            uart_cfg.txLinMode = UART_TX_LINMODE_ENABLE;
            uart_cfg.rxLinMode = UART_RX_LINMODE_ENABLE;
            uart_cfg.txBreakBitCnt = UART_TX_BREAKBIT_CNT;
            uart_cfg.rxDeglitch = ENABLE;
            UART_Init(uart_device->id, &uart_cfg);

            UART_TxFifoClear(uart_device->id);
            UART_RxFifoClear(uart_device->id);

            UART_Enable(uart_device->id, UART_TXRX);

            break;
        }
        case UART_CTRL_ATTACH_TX_DMA:
            uart_device->tx_dma = (int)args;
            break;
        case UART_CTRL_ATTACH_RX_DMA:
            uart_device->rx_dma = (int)args;
            break;
        case UART_CTRL_TX_DMA_SUSPEND:
            UART_DmaDisable(uart_device->id, UART_TX);
            uart_device->caps &= ~COMMON_CAPS_DMA_TX;
            break;
        case UART_CTRL_RX_DMA_SUSPEND:
            UART_DmaDisable(uart_device->id, UART_RX);
            uart_device->caps &= ~COMMON_CAPS_DMA_RX;
            break;
        case UART_CTRL_TX_DMA_RESUME:
            UART_DmaEnable(uart_device->id, UART_TX);
            uart_device->caps |= COMMON_CAPS_DMA_TX;
            break;
        case UART_CTRL_RX_DMA_RESUME:
            UART_DmaEnable(uart_device->id, UART_RX);
            uart_device->caps |= COMMON_CAPS_DMA_RX;
            break;
        case UART_CTRL_TX_TRIGGER_ON:
            uart_int_tx_enable(uart_device->id);
            break;
        case UART_CTRL_TX_TRIGGER_OFF:
            uart_int_tx_disable(uart_device->id);
            break;
        case UART_CTRL_GET_TX_FIFO:
            return UART_GetTxFifoCount(uart_device->id);
        case UART_CTRL_GET_RX_FIFO:
            return UART_GetRxFifoCount(uart_device->id);
        case UART_CTRL_CLEAR_TX_FIFO:
            return UART_TxFifoClear(uart_device->id);
        case UART_CTRL_CLEAR_RX_FIFO:
            return UART_RxFifoClear(uart_device->id);
        case UART_CTRL_SET_CALLBACK:
            uart_device->callback = args;
            break;
        default:
            break;
    }

    return 0;
}

int uart_int_tx_enable(uint8_t id)
{
    UART_IntMask(id, UART_INT_TX_FIFO_REQ, UNMASK);
    return 0;
}

int uart_int_tx_disable(uint8_t id)
{
    UART_IntMask(id, UART_INT_TX_FIFO_REQ, MASK);
    return 0;
}

int uart_write(int uart_id, const void *buffer, uint32_t size)
{
    int ret = 0;
    uart_device_t *uart_device = &uart_devices[uart_id];

    if (uart_device->caps & COMMON_CAPS_DMA_TX) {
        int dma_ch = uart_device->tx_dma;
        if (dma_ch < 0)
            return -1;

        if (uart_device->id == 0) {
            ret = dma_reload(DMA0_ID, dma_ch, (uint32_t)buffer, (uint32_t)DMA_ADDR_UART0_TDR, size, NULL);
            dma_channel_start(DMA0_ID, dma_ch);
        } else if (uart_device->id == 1) {
            ret = dma_reload(DMA0_ID, dma_ch, (uint32_t)buffer, (uint32_t)DMA_ADDR_UART1_TDR, size, NULL);
            dma_channel_start(DMA0_ID, dma_ch);
        }
        return ret;
    } else if (uart_device->caps & COMMON_CAPS_INT_TX) {
        return -2;
    } else
        return UART_SendData(uart_device->id, (uint8_t *)buffer, size);
}

int uart_read(int uart_id, void *buffer, uint32_t size)
{
    int ret = -1;
    uart_device_t *uart_device = &uart_devices[uart_id];

    if (uart_device->caps & COMMON_CAPS_DMA_RX) {
        int dma_ch = uart_device->rx_dma;
        if (dma_ch < 0)
            return -1;

        if (uart_device->id == 0) {
            ret = dma_reload(DMA0_ID, dma_ch, (uint32_t)DMA_ADDR_UART0_RDR, (uint32_t)buffer, size, NULL);
            dma_channel_start(DMA0_ID, dma_ch);
        } else if (uart_device->id == 1) {
            ret = dma_reload(DMA0_ID, dma_ch, (uint32_t)DMA_ADDR_UART1_RDR, (uint32_t)buffer, size, NULL);
            dma_channel_start(DMA0_ID, dma_ch);
        }
        return ret;
    } else if (uart_device->caps & COMMON_CAPS_INT_RX) {
        return -2;
    } else {
        return UART_ReceiveData(uart_device->id, (uint8_t *)buffer, size);
    }
}

void uart_isr(int uart_id)
{
    uart_device_t *uart_device = &uart_devices[uart_id];

    /* Length of uart tx data transfer arrived interrupt */
    if (UART_GetIntStatus(uart_id, UART_INT_TX_END)) {
        UART_IntClear(uart_id, UART_INT_TX_END);
        if (uart_device->callback)
            uart_device->callback(uart_device, NULL, 0, UART_EVENT_TX_END);
    }

    /* Length of uart rx data transfer arrived interrupt */
    if (UART_GetIntStatus(uart_id, UART_INT_RX_END)) {
        UART_IntClear(uart_id, UART_INT_RX_END);
        if (uart_device->callback)
            uart_device->callback(uart_device, NULL, 0, UART_EVENT_RX_END);
    }

    /* Tx fifo ready interrupt,auto-cleared when data is pushed */
    if (UART_GetIntStatus(uart_id, UART_INT_TX_FIFO_REQ)) {
        if (uart_device->callback)
            uart_device->callback(uart_device, NULL, 0, UART_EVENT_TX_FIFO);
    }

    /* Rx fifo ready interrupt,auto-cleared when data is popped */
    if (UART_GetIntStatus(uart_id, UART_INT_RX_FIFO_REQ)) {
        if (uart_device->callback) {
            uart_device->callback(uart_device, NULL, 0, UART_EVENT_RX_FIFO);
        }
    }

    /* Rx time-out interrupt */
    if (UART_GetIntStatus(uart_id, UART_INT_RTO)) {
        UART_IntClear(uart_id, UART_INT_RTO);
        if (uart_device->callback) {
            uart_device->callback(uart_device, NULL, 0, UART_EVENT_RX_FIFO);
        }
    }

    /* Rx parity check error interrupt */
    if (UART_GetIntStatus(uart_id, UART_INT_PCE)) {
        UART_IntClear(uart_id, UART_INT_PCE);
        if (uart_device->callback)
            uart_device->callback(uart_device, NULL, 0, UART_EVENT_PCE);
    }

    /* Tx fifo overflow/underflow error interrupt */
    if (UART_GetIntStatus(uart_id, UART_INT_TX_FER)) {
        UART_IntClear(uart_id, UART_INT_TX_FER);
        if (uart_device->callback)
            uart_device->callback(uart_device, NULL, 0, UART_EVENT_TX_FER);
    }

    /* Rx fifo overflow/underflow error interrupt */
    if (UART_GetIntStatus(uart_id, UART_INT_RX_FER)) {
        UART_IntClear(uart_id, UART_INT_RX_FER);
        if (uart_device->callback)
            uart_device->callback(uart_device, NULL, 0, UART_EVENT_RX_FER);
    }
}

void UART0_IRQ(void)
{
    uart_isr(UART0_INDEX);
}

void UART1_IRQ(void)
{
    uart_isr(UART1_INDEX);
}

int uart_read_available(int uart_id, void *buffer, uint32_t max_size)
{
    return UART_ReceiveData(uart_id, (uint8_t *)buffer, max_size);
}

