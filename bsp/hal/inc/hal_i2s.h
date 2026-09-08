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

#ifndef __HAL_I2S__H__
#define __HAL_I2S__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "board_config.h"

enum i2s_index_type {
#ifdef BSP_USING_I2S0
    I2S0_INDEX,
#endif
    I2S_MAX_INDEX
};

#define I2S_DEFAULT_RTO_TIMEOUT 15

enum {
    I2S_CTRL_ATTACH_TX_DMA,
    I2S_CTRL_ATTACH_RX_DMA,
    I2S_CTRL_GET_TX_FIFO,
    I2S_CTRL_GET_RX_FIFO,
    I2S_CTRL_SET_SAMPL_FREQ,
    I2S_CTRL_SET_FIFO_24B_LJ,  /* args: 0=right-justified {8'h0,data[23:0]}, 1=left-justified {data[23:0],8'h0} */
};

typedef enum {
    I2S_MODE_STD,   /*!< I2S STD Mode */
    I2S_MODE_LEFT,  /*!< Left-Justified Mode */
    I2S_MODE_RIGHT, /*!< Right-Justified Mode */
    I2S_MODE_DSP_A, /*!< DSP/PCM Mode A*/
    I2S_MODE_DSP_B, /*!< DSP/PCM Mode B*/
} interface_mode_t;

typedef enum {
    I2S_FRAME_LEN_8 = 1,  /*!< I2S frame size 8 bits */
    I2S_FRAME_LEN_16 = 2, /*!< I2S frame size 16 bits */
    I2S_FRAME_LEN_24 = 3, /*!< I2S frame size 24 bits */
    I2S_FRAME_LEN_32 = 4, /*!< I2S frame size 32 bits */
} i2s_frame_size_t;

typedef enum {
    I2S_DATA_LEN_8 = 1,  /*!< I2S data size 8 bits */
    I2S_DATA_LEN_16 = 2, /*!< I2S data size 16 bits */
    I2S_DATA_LEN_24 = 3, /*!< I2S data size 24 bits */
    I2S_DATA_LEN_32 = 4, /*!< I2S data size 32 bits */
} i2s_data_size_t;

typedef enum {
    I2S_FS_CHANNELS_NUM_MONO = 1, /*!< I2S frame is for 1 channels */
    I2S_FS_CHANNELS_NUM_2 = 2,    /*!< I2S frame is for 2 channels */
    I2S_FS_CHANNELS_NUM_3 = 3,    /*!< I2S frame is for 3 channels, DSP mode only, frame_size must equal data_size*/
    I2S_FS_CHANNELS_NUM_4 = 4,    /*!< I2S frame is for 4 channels, DSP mode only, frame_size must equal data_size*/
} i2s_channel_num_t;

typedef enum {
    I2S_MODE_MASTER = 0, /*!< I2S as master */
    I2S_MODE_SLAVE,      /*!< I2S as slave */
} i2s_mode_t;

typedef struct i2s_device {
    uint16_t caps;
    uint8_t id;
    i2s_mode_t iis_mode;
    interface_mode_t interface_mode;
    uint32_t sampl_freq_hz; /*!< I2S sample data frequency in Hz */
    i2s_channel_num_t channel_num;
    i2s_frame_size_t frame_size;
    i2s_data_size_t data_size;
    uint8_t fifo_threshold; /*!< I2S receive and transmit threshold*/
    int tx_dma;
    int rx_dma;
} i2s_device_t;

void hal_i2s_setConfig(i2s_device_t *dev);
int hal_i2s_write(int i2s_id, const void *buffer, uint32_t size);
int hal_i2s_read(int i2s_id, void *buffer, uint32_t size);
int hal_i2s_control(int i2s_id, int cmd, void *args);
int hal_i2s_init(int i2s_id, uint16_t caps);

#ifdef __cplusplus
}
#endif

#endif