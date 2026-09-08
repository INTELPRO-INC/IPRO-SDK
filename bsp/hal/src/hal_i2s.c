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

#include "hal_i2s.h"
#include "hal_dma.h"
#include "drv_i2s.h"
#include "drv_glb.h"
#include "drv_clock.h"
#include "i2s_config.h"

static i2s_device_t i2s_devices[I2S_MAX_INDEX] = {
    I2S0_CONFIG,
};

i2s_device_t* hal_i2s_getDevice(int index)
{
    return &i2s_devices[index];
}

void hal_i2s_setConfig(i2s_device_t *dev)
{
    memcpy(&i2s_devices[dev->id], dev, sizeof(i2s_device_t));
}

int hal_i2s_init(int i2s_id, uint16_t caps)
{
    I2S_CFG_Type i2sCfg = { 0 };
    I2S_FifoCfg_Type fifoCfg = { 0 };
    i2s_device_t *i2s_device = &i2s_devices[i2s_id];

    i2s_device->caps = caps;
    i2s_device->tx_dma = -1;
    i2s_device->rx_dma = -1;

    i2sCfg.audioFreqHz = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_I2S);

    i2sCfg.sampleFreqHz = i2s_device->sampl_freq_hz;

    /*!< default I2S msb first */
    i2sCfg.endianType = I2S_DATA_ENDIAN;

    /* Config the I2S type */
    switch (i2s_device->interface_mode) {
        case I2S_MODE_STD:
            i2sCfg.modeType = I2S_MODE_I2S_LEFT;
            i2sCfg.fsMode = I2S_FS_MODE_EVEN;
            i2sCfg.dataOffset = 1;
            break;

        case I2S_MODE_LEFT:
            i2sCfg.modeType = I2S_MODE_I2S_LEFT;
            i2sCfg.fsMode = I2S_FS_MODE_EVEN;
            i2sCfg.dataOffset = 0;
            break;

        case I2S_MODE_RIGHT:
            i2sCfg.modeType = I2S_MODE_I2S_RIGHT;
            i2sCfg.fsMode = I2S_FS_MODE_EVEN;
            i2sCfg.dataOffset = 0;
            break;

        case I2S_MODE_DSP_A:
            i2sCfg.modeType = I2S_MODE_I2S_DSP;
            i2sCfg.fsMode = I2S_FS_MODE_1T;
            i2sCfg.dataOffset = 1;
            break;

        case I2S_MODE_DSP_B:
            i2sCfg.modeType = I2S_MODE_I2S_DSP;
            i2sCfg.fsMode = I2S_FS_MODE_1T;
            i2sCfg.dataOffset = 0;
            break;

        default:
            return -1;
            break;
    }

    /* Config the frame/data Size */
    switch (i2s_device->frame_size) {
        case I2S_FRAME_LEN_8:
            i2sCfg.frameSize = I2S_SIZE_FRAME_8;
            break;

        case I2S_FRAME_LEN_16:
            i2sCfg.frameSize = I2S_SIZE_FRAME_16;
            break;

        case I2S_FRAME_LEN_24:
            i2sCfg.frameSize = I2S_SIZE_FRAME_24;
            break;

        case I2S_FRAME_LEN_32:
            i2sCfg.frameSize = I2S_SIZE_FRAME_32;
            break;

        default:
            return -1;
            break;
    }

    switch (i2s_device->data_size) {
        case I2S_DATA_LEN_8:
            i2sCfg.dataSize = I2S_SIZE_DATA_8;
            break;

        case I2S_DATA_LEN_16:
            i2sCfg.dataSize = I2S_SIZE_DATA_16;
            break;

        case I2S_DATA_LEN_24:
            i2sCfg.dataSize = I2S_SIZE_DATA_24;
            break;

        case I2S_DATA_LEN_32:
            i2sCfg.dataSize = I2S_SIZE_DATA_32;
            break;

        default:
            return -1;
            break;
    }

    fifoCfg.lRMerge = DISABLE;
    fifoCfg.frameDataExchange = DISABLE;

    /* Config the Channel number */
    switch (i2s_device->channel_num) {
        case I2S_FS_CHANNELS_NUM_MONO:
            i2sCfg.monoMode = ENABLE;
            i2sCfg.fsChannel = I2S_FS_CHANNELS_2;
            i2sCfg.monoModeChannel = I2S_MONO_CHANNEL;
            break;

        case I2S_FS_CHANNELS_NUM_2:
            i2sCfg.monoMode = DISABLE;
            i2sCfg.fsChannel = I2S_FS_CHANNELS_2;

            if (i2s_device->data_size == I2S_DATA_LEN_8 || i2s_device->data_size == I2S_DATA_LEN_16) {
                fifoCfg.lRMerge = ENABLE;
                fifoCfg.frameDataExchange = I2S_LR_EXCHANGE;
            }

            break;

        case I2S_FS_CHANNELS_NUM_3:
            if ((i2s_device->interface_mode != I2S_MODE_DSP_A) && (i2s_device->interface_mode != I2S_MODE_DSP_B)) {
                return -1;
            }

            i2sCfg.monoMode = DISABLE;
            i2sCfg.fsChannel = I2S_FS_CHANNELS_3;
            break;

        case I2S_FS_CHANNELS_NUM_4:
            if ((i2s_device->interface_mode != I2S_MODE_DSP_A) && (i2s_device->interface_mode != I2S_MODE_DSP_B)) {
                return -1;
            }

            i2sCfg.monoMode = DISABLE;
            i2sCfg.fsChannel = I2S_FS_CHANNELS_4;

        default:
            return -1;
            break;
    }

    /* Config the bclk/fs invert */
    i2sCfg.bclkInvert = I2S_BCLK_INVERT;
    i2sCfg.fsInvert = I2S_FS_INVERT;

    fifoCfg.txfifoDmaEnable = (caps & COMMON_CAPS_DMA_TX) ? ENABLE : DISABLE;
    fifoCfg.rxfifoDmaEnable = (caps & COMMON_CAPS_DMA_RX) ? ENABLE : DISABLE;
    fifoCfg.txFifoLevel = i2s_device->fifo_threshold;
    fifoCfg.rxFifoLevel = i2s_device->fifo_threshold;

    /* I2S Init */
    I2S_Disable(i2s_id);
    I2S_Init(i2s_id, &i2sCfg);
    I2S_FifoConfig(i2s_id, &fifoCfg);
    I2S_SetBclkPeriod(i2s_id, &i2sCfg);

    if (i2s_device->iis_mode == I2S_MODE_MASTER)
        I2S_Enable(i2s_id, I2S_ROLE_MASTER);
    else if (i2s_device->iis_mode == I2S_MODE_SLAVE)
        I2S_Enable(i2s_id, I2S_ROLE_SLAVE);
    return 0;
}

int hal_i2s_close(int i2s_id)
{
    //i2s_device_t *i2s_device = &i2s_devices[i2s_id];

    I2S_Disable(i2s_id);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_I2S);
    return 0;
}

int hal_i2s_control(int i2s_id, int cmd, void *args)
{
    I2S_CFG_Type i2sCfg;
    i2s_device_t *i2s_device = &i2s_devices[i2s_id];

    switch (cmd) {
        case I2S_CTRL_ATTACH_TX_DMA :
            i2s_device->tx_dma = (int)args;
            break;

        case I2S_CTRL_ATTACH_RX_DMA :
            i2s_device->rx_dma = (int)args;
            break;

        case I2S_CTRL_GET_TX_FIFO:
            return I2S_GetTxFIFO_AvlCnt(i2s_id);

        case I2S_CTRL_GET_RX_FIFO:
            return I2S_GetRxFIFO_AvlCnt(i2s_id);

        case I2S_CTRL_SET_FIFO_24B_LJ:
            I2S_SetFifoJustified(i2s_id, (int)(uintptr_t)args);
            break;

        case I2S_CTRL_SET_SAMPL_FREQ:
            switch (i2s_device->frame_size) {
                case I2S_FRAME_LEN_8:
                    i2sCfg.frameSize = I2S_SIZE_FRAME_8;
                    break;

                case I2S_FRAME_LEN_16:
                    i2sCfg.frameSize = I2S_SIZE_FRAME_16;
                    break;

                case I2S_FRAME_LEN_24:
                    i2sCfg.frameSize = I2S_SIZE_FRAME_24;
                    break;

                case I2S_FRAME_LEN_32:
                    i2sCfg.frameSize = I2S_SIZE_FRAME_32;
                    break;
                default:
                    return -1;
                    break;
            }
            i2sCfg.audioFreqHz = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_I2S);
            i2sCfg.sampleFreqHz = (uint32_t)args;
            I2S_SetBclkPeriod(i2s_id, &i2sCfg);
            break;

        default:
            return -1;
            break;
    }

    return 0;
}

int hal_i2s_write(int i2s_id, const void *buffer, uint32_t size)
{
    i2s_device_t *i2s_device = &i2s_devices[i2s_id];

    if (i2s_device->caps & COMMON_CAPS_DMA_TX) {
        int dma_ch = i2s_device->tx_dma;

        if (dma_ch < 0) {
            return -1;
        }

        dma_reload(DMA0_ID, dma_ch, (uint32_t)buffer, (uint32_t)DMA_ADDR_I2S_TDR, size, NULL);
        dma_channel_start(DMA0_ID, dma_ch);

        return 0;
    } else {
        return -1;
    }
}

int hal_i2s_read(int i2s_id, void *buffer, uint32_t size)
{
    i2s_device_t *i2s_device = &i2s_devices[i2s_id];

    if (i2s_device->caps & COMMON_CAPS_DMA_RX) {
        int dma_ch = i2s_device->rx_dma;

        if (dma_ch < 0) {
            return -1;
        }

        dma_reload(DMA0_ID, dma_ch, (uint32_t)DMA_ADDR_I2S_RDR, (uint32_t)buffer, size, NULL);
        dma_channel_start(DMA0_ID, dma_ch);

        return 0;
    } else {
        return -1;
    }
}
