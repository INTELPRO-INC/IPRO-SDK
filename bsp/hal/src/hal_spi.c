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

#include "hal_spi.h"
#include "hal_dma.h"
#include "drv_glb.h"
#include "drv_spi.h"
#include "compiler/common.h"

#define SPI_MODULE_FREQ (96*1000*1000) /*IPRO7 default freq.*/
static void SPI0_IRQ(void);

static ATTR_OCRAM_SECTION spi_device_t spi_devices[SPI_MAX_INDEX] = {
    SPI0_CONFIG, SPI1_CONFIG
};

void spi_setConfig(spi_device_t *dev)
{
    memcpy(&spi_devices[dev->id], dev, sizeof(spi_device_t));
}

spi_device_t* spi_getDevice(int index)
{
    return &spi_devices[index];
}


int spi_init(int spi_id)
{
    spi_device_t *spi_device = &spi_devices[spi_id];
    SPI_CFG_Type spiCfg = { 0 };
    SPI_FifoCfg_Type fifoCfg = { 0 };

    if (spi_device->pin_swap_enable) {
        GLB_Swap_SPI_0_MOSI_With_MISO(ENABLE);
    }

    CPU_Interrupt_Disable(SPI0_IRQn);
    SPI_IntMask(spi_device->id, SPI_INT_ALL, UNMASK);

    SPI_Disable(spi_device->id, spi_device->mode);

    GLB_Set_SPI_0_ACT_MOD_Sel(spi_device->mode);

    if (spi_device->clk == 0 || spi_device->clk > 48000000) {
        SPI_SetClock(spi_device->id, 48000000);
    } else {
        SPI_SetClock(spi_device->id, spi_device->clk);
    }
    #if 0
    /* Set SPI clock */
    SPI_ClockCfg_Type clockCfg = {
        2, /* Length of start condition */
        2, /* Length of stop condition */
        2, /* Length of data phase 0,affecting clock */
        2, /* Length of data phase 1,affecting clock */
        2  /* Length of interval between frame */
    };

    if (spi_device->clk > SPI_MODULE_FREQ) {
        return -1;
    }

    uint8_t length = SPI_MODULE_FREQ / spi_device->clk;

    if (!(length % 2)) {
        clockCfg.dataPhase0Len = length / 2;
        clockCfg.dataPhase1Len = length / 2;
    } else {
        clockCfg.dataPhase0Len = length / 2;
        clockCfg.dataPhase1Len = length / 2 + 1;
    }

    SPI_ClockConfig(spi_device->id, &clockCfg);
    #endif

    spiCfg.continuousEnable = 1;
    spiCfg.ignore_cs = spi_device->ignore_cs;
    if (spi_device->direction == SPI_LSB_BYTE0_DIRECTION_FIRST) {
        spiCfg.bitSequence = SPI_BIT_INVERSE_LSB_FIRST;
        spiCfg.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST;
    } else if (spi_device->direction == SPI_LSB_BYTE3_DIRECTION_FIRST) {
        spiCfg.bitSequence = SPI_BIT_INVERSE_LSB_FIRST;
        spiCfg.byteSequence = SPI_BYTE_INVERSE_BYTE3_FIRST;
    } else if (spi_device->direction == SPI_MSB_BYTE0_DIRECTION_FIRST) {
        spiCfg.bitSequence = SPI_BIT_INVERSE_MSB_FIRST;
        spiCfg.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST;
    } else if (spi_device->direction == SPI_MSB_BYTE3_DIRECTION_FIRST) {
        spiCfg.bitSequence = SPI_BIT_INVERSE_MSB_FIRST;
        spiCfg.byteSequence = SPI_BYTE_INVERSE_BYTE3_FIRST;
    }

    spiCfg.clkPolarity = spi_device->clk_polaraity;
    spiCfg.clkPhaseInv = spi_device->clk_phase;
    spiCfg.frameSize = spi_device->datasize;

    if (spi_device->delitch_cnt) {
        spiCfg.deglitchEnable = 1;
    }

    /* SPI config */
    SPI_Init(spi_device->id, &spiCfg);
    SPI_ClrTxFifo(spi_device->id);
    SPI_ClrRxFifo(spi_device->id);

    SPI_SetDeglitchCount(spi_device->id, spi_device->delitch_cnt);

    fifoCfg.txFifoThreshold = spi_device->fifo_threshold;
    fifoCfg.txFifoDmaEnable = DISABLE;
    fifoCfg.rxFifoThreshold = spi_device->fifo_threshold;
    fifoCfg.rxFifoDmaEnable = DISABLE;

    if (spi_device->caps & COMMON_CAPS_INT_TX || spi_device->caps & COMMON_CAPS_INT_RX) {
        Interrupt_Handler_Register(SPI0_IRQn, SPI0_IRQ);
    }

    if (spi_device->caps & COMMON_CAPS_DMA_TX) {
        fifoCfg.txFifoDmaEnable = ENABLE;
    }

    if (spi_device->caps & COMMON_CAPS_DMA_RX) {
        fifoCfg.rxFifoDmaEnable = ENABLE;
    }

    SPI_FifoConfig(spi_device->id, &fifoCfg);

    return 0;
}

int spi_deinit(int spi_id)
{
    spi_device_t *spi_device = &spi_devices[spi_id];

    SPI_Disable(spi_device->id, spi_device->mode);
    if (spi_id == 0)
        GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SPI);
#ifdef IPRO7
    else if (spi_id == 1)
        GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SPI2);
#endif
    return 0;
}

int spi_enable(int spi_id, int en)
{
    spi_device_t *spi_device = &spi_devices[spi_id];

    if (en)
        SPI_Enable(spi_device->id, spi_device->mode);
    else
        SPI_Disable(spi_device->id, spi_device->mode);
    return 0;
}

int spi_ioctl(int spi_id, int cmd, void *args)
{
    spi_device_t *spi_device = &spi_devices[spi_id];

    switch (cmd) {
        case SPI_CTRL_CONFIG_CLOCK:
            SPI_SetClock(spi_device->id, (uint32_t)args);
            break;

        case SPI_CTRL_TX_DMA_SUSPEND: {
            SPI_DmaTxDisable(spi_device->id);
            spi_device->caps &= ~COMMON_CAPS_DMA_TX;
            break;
        }

        case SPI_CTRL_RX_DMA_SUSPEND: {
            SPI_DmaRxDisable(spi_device->id);
            spi_device->caps &= ~COMMON_CAPS_DMA_RX;
            break;
        }

        case SPI_CTRL_ATTACH_TX_DMA:
        case SPI_CTRL_TX_DMA_RESUME: {
            SPI_DmaTxEnable(spi_device->id);
            spi_device->caps |= COMMON_CAPS_DMA_TX;
            break;
        }
        case SPI_CTRL_ATTACH_RX_DMA:
        case SPI_CTRL_RX_DMA_RESUME: {
            SPI_DmaRxEnable(spi_device->id);
            spi_device->caps |= COMMON_CAPS_DMA_RX;
            break;
        }

        case SPI_CTRL_GET_TX_FIFO :
            return SPI_GetTxFifoCount(spi_device->id);

        case SPI_CTRL_GET_RX_FIFO :
            return SPI_GetRxFifoCount(spi_device->id);

        case SPI_CTRL_CLEAR_TX_FIFO :
            return SPI_ClrTxFifo(spi_device->id);

        case SPI_CTRL_CLEAR_RX_FIFO :
            return SPI_ClrRxFifo(spi_device->id);

        case SPI_CTRL_GET_BUS_BUSY_STATUS :
            return SPI_GetBusyStatus(spi_device->id);

        case SPI_CTRL_SET_FRAME_SIZE:
            SPI_ClrTxFifo(spi_device->id);
            SPI_ClrRxFifo(spi_device->id);
            SPI_SetFrameSize(spi_device->id, (uint8_t)(uintptr_t)args);
            spi_device->datasize = (uint32_t)args;
            break;

        case SPI_CTRL_SET_CALLBACK:
            spi_device->callback = args;
            break;

        case SPI_CTRL_SET_INT_MASK:
            SPI_IntMask(spi_device->id, (SPI_INT_Type)args, MASK);
            break;
        case SPI_CTRL_SET_INT_UNMASK:
            SPI_IntMask(spi_device->id, (SPI_INT_Type)args, UNMASK);
            break;

        default:
            break;
    }

    return 0;
}

int spi_transmit(int spi_id, void *buffer, uint32_t size)
{
    return SPI_SendData(spi_id, (void *)buffer, size, SPI_TIMEOUT_DISABLE);
}

int spi_receive(int spi_id, void *buffer, uint32_t size)
{
    return SPI_ReceiveData(spi_id, (void *)buffer, size, SPI_TIMEOUT_DISABLE);
}

int spi_transmit_receive(int spi_id, const void *send_buf, void *recv_buf, uint32_t length)
{
    return SPI_SendRecvData(spi_id, (void *)send_buf, (void *)recv_buf, length, SPI_TIMEOUT_DISABLE);
}

void spi_isr(int spi_id)
{
    spi_device_t *spi_device = &spi_devices[spi_id];

    /* Transfer end interrupt,shared by both master and slave mode */
    if (SPI_GetIntStatus(spi_id, SPI_INT_END)) {
        SPI_ClearInt(spi_id, SPI_INT_END);
        if (spi_device->callback)
            spi_device->callback(spi_device, HAL_SPI_INT_END);
    }

    /* TX fifo ready interrupt(fifo count > fifo threshold) */
    if (SPI_GetIntStatus(spi_id, SPI_INT_TX_FIFO_REQ)) {
        if (spi_device->callback)
            spi_device->callback(spi_device, HAL_SPI_INT_TX_FIFO_REQ);
    }

    /*  RX fifo ready interrupt(fifo count > fifo threshold) */
    if (SPI_GetIntStatus(spi_id, SPI_INT_RX_FIFO_REQ)) {
        if (spi_device->callback)
            spi_device->callback(spi_device, HAL_SPI_INT_RX_FIFO_REQ);
    }

    /* Slave mode transfer time-out interrupt,triggered when bus is idle for the given value */
    if (SPI_GetIntStatus(spi_id, SPI_INT_SLAVE_TIMEOUT)) {
        SPI_ClearInt(spi_id, SPI_INT_SLAVE_TIMEOUT);
        if (spi_device->callback)
            spi_device->callback(spi_device, HAL_SPI_INT_SLAVE_TIMEOUT);
    }

    /* Slave mode tx underrun error interrupt,trigged when tx is not ready during transfer */
    if (SPI_GetIntStatus(spi_id, SPI_INT_SLAVE_UNDERRUN)) {
        SPI_ClearInt(spi_id, SPI_INT_SLAVE_UNDERRUN);
        if (spi_device->callback)
            spi_device->callback(spi_device, HAL_SPI_INT_SLAVE_UNDERRUN);
    }
    

    /* TX/RX fifo overflow/underflow interrupt */
    if (SPI_GetIntStatus(spi_id, SPI_INT_FIFO_ERROR)) {
        if (spi_device->callback)
            spi_device->callback(spi_device, HAL_SPI_INT_FIFO_ERROR);
    }
}

void SPI0_IRQ()
{
    spi_isr(SPI0_INDEX);
}

