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


#ifndef __DRV_I2S_H__
#define __DRV_I2S_H__

#include "i2s_reg.h"
#include "drv_common.h"

typedef enum {
    I2S_MODE_I2S_LEFT,  /*!< Left-Justified Mode */
    I2S_MODE_I2S_RIGHT, /*!< Right-Justified Mode */
    I2S_MODE_I2S_DSP,   /*!< DSP Mode */
} I2S_Mode_Type;

typedef enum {
    I2S0_ID,    /*!< I2S0 port define */
    I2S_ID_MAX, /*!< I2S MAX ID define */
} I2S_ID_Type;

typedef enum {
    I2S_ROLE_MASTER = 0, /*!< I2S as master */
    I2S_ROLE_SLAVE = 1,  /*!< I2S as slave */
} I2S_Role_Type;

typedef enum {
    I2S_SIZE_DATA_8 = 0, /*!< I2S data size 8 bits */
    I2S_SIZE_DATA_16,    /*!< I2S data size 16 bits */
    I2S_SIZE_DATA_24,    /*!< I2S data size 24 bits */
    I2S_SIZE_DATA_32,    /*!< I2S data size 32 bits */
} I2S_Data_Size_Type;

typedef enum {
    I2S_SIZE_FRAME_8 = 0, /*!< I2S frame size 8 bits */
    I2S_SIZE_FRAME_16,    /*!< I2S frame size 16 bits */
    I2S_SIZE_FRAME_24,    /*!< I2S frame size 24 bits */
    I2S_SIZE_FRAME_32,    /*!< I2S frame size 32 bits */
} I2S_Frame_Size_Type;

typedef enum {
    I2S_DATA_ENDIAN_MSB = 0, /*!< I2S use MSB first */
    I2S_DATA_ENDIAN_LSB,     /*!< I2S use LSB first */
} I2S_Endian_Type;

typedef enum {
    I2S_FS_MODE_EVEN = 0, /*!< I2S frame is even */
    I2S_FS_MODE_1T,       /*!< I2S frame is only 1T high */
} I2S_Fs_Mode_Type;

typedef enum {
    I2S_RX_MONO_MODE_LEFT_CHANNEL,  /*!< I2S rx mono mode left channel */
    I2S_RX_MONO_MODE_RIGHT_CHANNEL, /*!< I2S rx mono mode right channel */
} I2S_Mono_Mode_Channel;

typedef enum {
    I2S_FS_CHANNELS_2 = 0, /*!< I2S frame is for 2 channels */
    I2S_FS_CHANNELS_3,     /*!< I2S frame is for 3 channels */
    I2S_FS_CHANNELS_4,     /*!< I2S frame is for 4 channels */
    I2S_FS_CHANNELS_6,     /*!< I2S frame is for 6 channels */
} I2S_Fs_Channel_Type;

typedef enum {
    I2S_MONO_CONTROL_STEREO = 0, /*!< I2S output stereo */
    I2S_MONO_CONTROL_MONO,       /*!< I2S output mono */
} I2S_MonoControl_Type;

typedef enum {
    I2S_FIFO_24_RIGHT_JUSTIFIED = 0, /*!< I2S fifo:[31:24]0x00,[23:0]24bit_data */
    I2S_FIFO_24_LEFT_JUSTIFIED,      /*!< I2S fifo:[31:8]24bit_data,[7:0]0x00 */
} I2S_FIFO_24_Justified_Type;

typedef struct
{
    I2S_Endian_Type endianType;            /*!< I2S endian configuration */
    I2S_Mode_Type modeType;                /*!< I2S mode configuration */
    uint32_t audioFreqHz;                  /*!< I2S audio pll output frequency in Hz */
    uint32_t sampleFreqHz;                 /*!< I2S sample data frequency in Hz */
    int bclkInvert;                /*!< I2S bclk invert configuration */
    I2S_Frame_Size_Type frameSize;         /*!< I2S frame size configuration */
    int fsInvert;                  /*!< I2S frame select invert configuration */
    I2S_Fs_Mode_Type fsMode;               /*!< I2S frame mode configuration */
    I2S_Fs_Channel_Type fsChannel;         /*!< I2S frame channels configuration */
    uint32_t dataOffset;                   /*!< I2S data output offset configuration */
    I2S_Data_Size_Type dataSize;           /*!< I2S data size configuration */
    int monoMode;                  /*!< I2S enable mono mode,when this mode enable,lRMerge must be disabled */
    I2S_Mono_Mode_Channel monoModeChannel; /*!< I2S mono mode channel configuration */
} I2S_CFG_Type;

typedef struct
{
    int lRMerge;           /*!< I2S FIFO contains both left and right channel data in one entry */
    int frameDataExchange; /*!< I2S exchange left and right channel data in one entry */
    int txfifoDmaEnable;   /*!< Enable or disable I2S tx fifo dma function. */
    int rxfifoDmaEnable;   /*!< Enable or disable I2S rx fifo dma function. */
    uint8_t rxFifoLevel;           /*!< I2S receive fifo interrupt trigger level. only valid when fifo mode enabled. */
    uint8_t txFifoLevel;           /*!< I2S transmit fifo interrupt trigger level. only valid when fifo mode enabled. */
} I2S_FifoCfg_Type;

typedef struct
{
    int deglitchEn;  /*!< Enable or disable deglitch(for all the input pins) */
    uint8_t deglitchCnt;     /*!< Deglitch cycle count(unit:cycle of I2S kernel clock) */
    int inverseBCLK; /*!< Enable or disable inverse BCLK signal */
    int inverseFS;   /*!< Enable or disable inverse FS signal */
    int inverseRX;   /*!< Enable or disable inverse RX signal */
    int inverseTX;   /*!< Enable or disable inverse TX signal */
} I2S_IOCfg_Type;

#define IS_I2S_MODE_TYPE(type) (((type) == I2S_MODE_I2S_LEFT) ||  \
                                ((type) == I2S_MODE_I2S_RIGHT) || \
                                ((type) == I2S_MODE_I2S_DSP))

#define IS_I2S_ID_TYPE(type) (((type) == I2S0_ID) || \
                              ((type) == I2S1_ID) || \
                              ((type) == I2S_ID_MAX))

#define IS_I2S_ROLE_TYPE(type) (((type) == I2S_ROLE_MASTER) || \
                                ((type) == I2S_ROLE_SLAVE))

#define IS_I2S_DATA_SIZE_TYPE(type) (((type) == I2S_SIZE_DATA_8) ||  \
                                     ((type) == I2S_SIZE_DATA_16) || \
                                     ((type) == I2S_SIZE_DATA_24) || \
                                     ((type) == I2S_SIZE_DATA_32))

#define IS_I2S_FRAME_SIZE_TYPE(type) (((type) == I2S_SIZE_FRAME_8) ||  \
                                      ((type) == I2S_SIZE_FRAME_16) || \
                                      ((type) == I2S_SIZE_FRAME_24) || \
                                      ((type) == I2S_SIZE_FRAME_32))

#define IS_I2S_ENDIAN_TYPE(type) (((type) == I2S_DATA_ENDIAN_MSB) || \
                                  ((type) == I2S_DATA_ENDIAN_LSB))

#define IS_I2S_FS_MODE_TYPE(type) (((type) == I2S_FS_MODE_EVEN) || \
                                   ((type) == I2S_FS_MODE_1T))

#define IS_I2S_MONO_MODE_CHANNEL(type) (((type) == I2S_RX_MONO_MODE_LEFT_CHANNEL) || \
                                        ((type) == I2S_RX_MONO_MODE_RIGHT_CHANNEL))

#define IS_I2S_FS_CHANNEL_TYPE(type) (((type) == I2S_FS_CHANNELS_2) || \
                                      ((type) == I2S_FS_CHANNELS_3) || \
                                      ((type) == I2S_FS_CHANNELS_4))

#define IS_I2S_MONOCONTROL_TYPE(type) (((type) == I2S_MONO_CONTROL_STEREO) || \
                                       ((type) == I2S_MONO_CONTROL_MONO))

#define IS_I2S_FIFO_24_JUSTIFIED_TYPE(type) (((type) == I2S_FIFO_24_RIGHT_JUSTIFIED) || \
                                             ((type) == I2S_FIFO_24_LEFT_JUSTIFIED))

#define I2S_TX_FIFO_SIZE 16
#define I2S_RX_FIFO_SIZE 16

void I2S_SetBclkPeriod(I2S_ID_Type i2sId, I2S_CFG_Type *i2sCfg);
void I2S_Init(I2S_ID_Type i2sId, I2S_CFG_Type *i2sCfg);
void I2S_FifoConfig(I2S_ID_Type i2sId, I2S_FifoCfg_Type *fifoCfg);
void I2S_IOConfig(I2S_ID_Type i2sId, I2S_IOCfg_Type *ioCfg);
void I2S_Enable(I2S_ID_Type i2sId, I2S_Role_Type roleType);
void I2S_Disable(I2S_ID_Type i2sId);
uint32_t I2S_Read(I2S_ID_Type i2sId);
void I2S_Write(I2S_ID_Type i2sId, uint32_t data);
void I2S_Mute(I2S_ID_Type i2sId, int enabled);
void I2S_SetFifoJustified(I2S_ID_Type i2sId, I2S_FIFO_24_Justified_Type justType);
uint32_t I2S_GetTxFIFO_AvlCnt(I2S_ID_Type i2sId);
uint32_t I2S_GetRxFIFO_AvlCnt(I2S_ID_Type i2sId);
void I2S_Rx_FIFO_Flush(I2S_ID_Type i2sId);
void I2S_Tx_FIFO_Flush(I2S_ID_Type i2sId);

#endif /* __DRV_I2S_H__ */
