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

#ifndef __HAL_SPI__H__
#define __HAL_SPI__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common.h"
#include "board_config.h"

enum {
    SPI_CTRL_ATTACH_TX_DMA = 0,
    SPI_CTRL_ATTACH_RX_DMA,
    SPI_CTRL_CONFIG_CLOCK,
    SPI_CTRL_TX_DMA_SUSPEND,
    SPI_CTRL_RX_DMA_SUSPEND,
    SPI_CTRL_TX_DMA_RESUME,
    SPI_CTRL_RX_DMA_RESUME,
    SPI_CTRL_GET_TX_FIFO,
    SPI_CTRL_GET_RX_FIFO,
    SPI_CTRL_CLEAR_TX_FIFO,
    SPI_CTRL_CLEAR_RX_FIFO,
    SPI_CTRL_GET_BUS_BUSY_STATUS,
    SPI_CTRL_SET_FRAME_SIZE,
    SPI_CTRL_SET_CALLBACK,
    SPI_CTRL_SET_INT_MASK,
    SPI_CTRL_SET_INT_UNMASK,
};

enum spi_index_type {
    SPI0_INDEX,
    SPI1_INDEX,
    SPI_MAX_INDEX
};

typedef enum {
    HAL_SPI_INT_END,            /*!< SPI transfer end interrupt,shared by both master and slave mode */
    HAL_SPI_INT_TX_FIFO_REQ,    /*!< SPI tx fifo ready interrupt(tx fifo count > tx fifo threshold) */
    HAL_SPI_INT_RX_FIFO_REQ,    /*!< SPI rx fifo ready interrupt(rx fifo count > rx fifo threshold) */
    HAL_SPI_INT_SLAVE_TIMEOUT,  /*!< SPI slave mode transfer time-out interrupt,triggered when spi bus is idle for the given value */
    HAL_SPI_INT_SLAVE_UNDERRUN, /*!< SPI slave mode tx underrun error interrupt,triggered when tx is not ready during transfer */
    HAL_SPI_INT_FIFO_ERROR,     /*!< SPI tx/rx fifo error interrupt(overflow/underflow) */
    HAL_SPI_INT_ALL,            /*!< All the interrupt */
} HAL_SPI_INT_Type;

#define SPI_LSB_BYTE0_DIRECTION_FIRST 0
#define SPI_LSB_BYTE3_DIRECTION_FIRST 1
#define SPI_MSB_BYTE0_DIRECTION_FIRST 2
#define SPI_MSB_BYTE3_DIRECTION_FIRST 3

#define SPI_DATASIZE_8BIT  0
#define SPI_DATASIZE_16BIT 1
#define SPI_DATASIZE_24BIT 2
#define SPI_DATASIZE_32BIT 3

#define SPI_POLARITY_LOW  0
#define SPI_POLARITY_HIGH 1

#define SPI_PHASE_1EDGE 0
#define SPI_PHASE_2EDGE 1

#define SPI_SLAVE_MODE  0
#define SPI_MASTER_MODE 1

#define SPI_TRANSFER_TYPE_8BIT  0
#define SPI_TRANSFER_TYPE_16BIT 1
#define SPI_TRANSFER_TPYE_24BIT 2
#define SPI_TRANSFER_TYPE_32BIT 3

enum spi_event_type {
    SPI_EVENT_TX_FIFO,
    SPI_EVENT_RX_FIFO,
    SPI_EVENT_UNKNOWN
};

typedef struct spi_device {
    uint16_t caps;
    uint8_t id;
    uint32_t clk;
    uint8_t mode;
    uint8_t direction;
    uint8_t ignore_cs;
    uint8_t clk_polaraity;
    uint8_t clk_phase;
    uint8_t datasize;
    uint8_t fifo_threshold;
    uint8_t pin_swap_enable; /*swap mosi and miso*/
    uint8_t delitch_cnt;
    void (*callback)(void *dev, uint32_t event);
} spi_device_t;

int spi_init(int spi_id);
int spi_deinit(int spi_id);
void spi_setConfig(spi_device_t *dev);
int spi_enable(int spi_id, int en);
int spi_ioctl(int spi_id, int cmd, void *args);
int spi_transmit(int spi_id, void *buffer, uint32_t size);
int spi_receive(int spi_id, void *buffer, uint32_t size);
int spi_transmit_receive(int spi_id, const void *send_buf, void *recv_buf, uint32_t length);
#ifdef __cplusplus
}
#endif
#endif
