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

#ifndef __HAL_UART__H__
#define __HAL_UART__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "board_config.h"

#define UART_FIFO_LEN 128

enum {
    UART_CTRL_SET_INT = 0,
    UART_CTRL_CLR_INT,
    UART_CTRL_RESUME,
    UART_CTRL_SUSPEND,
    UART_CTRL_CONFIG,
    UART_CTRL_ATTACH_TX_DMA,
    UART_CTRL_ATTACH_RX_DMA,
    UART_CTRL_TX_DMA_SUSPEND,
    UART_CTRL_RX_DMA_SUSPEND,
    UART_CTRL_TX_DMA_RESUME,
    UART_CTRL_RX_DMA_RESUME,
    UART_CTRL_GET_TX_FIFO,
    UART_CTRL_GET_RX_FIFO,
    UART_CTRL_CLEAR_TX_FIFO,
    UART_CTRL_CLEAR_RX_FIFO,
    UART_CTRL_SET_CALLBACK,
    UART_CTRL_TX_FLUSH,
    UART_CTRL_BAUDRATE_SET,
    UART_CTRL_TX_TRIGGER_OFF,
    UART_CTRL_TX_TRIGGER_ON,
};

enum uart_index_type {
    UART0_INDEX = 0,
    UART1_INDEX,
    UART_MAX_INDEX
};

typedef enum {
    UART_DATA_LEN_5 = 0, /*!< Data length is 5 bits */
    UART_DATA_LEN_6 = 1, /*!< Data length is 6 bits */
    UART_DATA_LEN_7 = 2, /*!< Data length is 7 bits */
    UART_DATA_LEN_8 = 3  /*!< Data length is 8 bits */
} uart_databits_t;

typedef enum {
    UART_STOP_ZERO_D_FIVE = 0, /*!< 0.5 stop bit */
    UART_STOP_ONE = 1,         /*!< 1 stop bit */
    UART_STOP_ONE_D_FIVE = 2,  /*!< 1.5 stop bit */
    UART_STOP_TWO = 3          /*!< 2 stop bits */
} uart_stopbits_t;

typedef enum {
    UART_PAR_NONE = 0, /*!< No parity */
    UART_PAR_ODD = 1,  /*!< Parity bit is odd */
    UART_PAR_EVEN = 2, /*!< Parity bit is even */
} uart_parity_t;

enum uart_event_type {
    UART_EVENT_TX_END,
    UART_EVENT_TX_FIFO,
    UART_EVENT_RX_END,
    UART_EVENT_RX_FIFO,
    UART_EVENT_RTO,
    UART_EVENT_PCE,
    UART_EVENT_TX_FER,
    UART_EVENT_RX_FER,
    UART_EVENT_UNKNOWN
};

enum uart_it_type {
    UART_TX_END_IT = 1 << 0,
    UART_RX_END_IT = 1 << 1,
    UART_TX_FIFO_IT = 1 << 2,
    UART_RX_FIFO_IT = 1 << 3,
    UART_RTO_IT = 1 << 4,
    UART_PCE_IT = 1 << 5,
    UART_TX_FER_IT = 1 << 6,
    UART_RX_FER_IT = 1 << 7,
    UART_ALL_IT = 1 << 8
};

typedef struct
{
    uint32_t baudrate;
    uart_databits_t databits;
    uart_stopbits_t stopbits;
    uart_parity_t parity;
} uart_param_cfg_t;

typedef struct uart_device {
    uint8_t id;
    uint16_t caps;
    uint32_t baudrate;
    uart_databits_t databits;
    uart_stopbits_t stopbits;
    uart_parity_t parity;
    uint8_t fifo_threshold;
    int tx_dma;
    int rx_dma;
    void (*callback)(void *dev, void *args, uint32_t size, uint32_t event);
} uart_device_t;

int uart_init(int uart_id, uint16_t oflag);
int uart_uninit(int uart_id);
int uart_ioctl(int uart_id, int cmd, void *args);
int uart_write(int uart_id, const void *buffer, uint32_t size);
int uart_read(int uart_id, void *buffer, uint32_t size);
int uart_int_tx_enable(uint8_t id);
int uart_int_tx_disable(uint8_t id);
void uart_set_baudrate(int uart_id, int baudrate);
void uart_set_callback(int uart_id, void *args);
uart_device_t* uart_getDevice(int uart_id);

/**
 * @brief Read available data from UART RX FIFO (non-blocking)
 * @param uart_id UART index (UART0_INDEX or UART1_INDEX)
 * @param buffer Buffer to store received data
 * @param max_size Maximum bytes to read
 * @return Number of bytes actually read
 */
int uart_read_available(int uart_id, void *buffer, uint32_t max_size);

#ifdef __cplusplus
}
#endif
#endif
