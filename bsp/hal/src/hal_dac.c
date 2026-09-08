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

#include "hal_dac.h"
#include "hal_dma.h"
#include "drv_dac.h"
#include "drv_clock.h"
#include "drv_glb.h"

static dac_device_t dac_devices[] = {
    DAC_CONFIG,
};

static uint8_t dac_channel_enable_check = 0;

dac_device_t* hal_dac_getDevice(int index)
{
    return &dac_devices[index];
}

void hal_dac_setConfig(dac_device_t *dev)
{
    memcpy(&dac_devices[dev->id], dev, sizeof(dac_device_t));
}

int hal_dac_init(int dac_id)
{

    GLB_GPIP_DAC_Cfg_Type gpdac_cfg;
    GLB_GPIP_DAC_ChanA_Cfg_Type gpdac_chA_cfg;
    GLB_GPIP_DAC_ChanB_Cfg_Type gpdac_chB_cfg;
    uint8_t dac_div = 0;
    dac_device_t *dac_device = &dac_devices[dac_id];

    dac_device->tx_dma = -1;

    GLB_Set_GPDAC_CLK_Sel(GLB_DAC_CLK_XCLK_32M, 62, 1);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_GPIP);

    if ((GLB_GPIO_Get_Fun(GLB_GPIO_PIN_23) == GPIO_FUN_ANALOG) && (dac_device->channels & DAC_CHANNEL_0)) {
        dac_channel_enable_check |= DAC_CHANNEL_0;
    }
    if ((GLB_GPIO_Get_Fun(GLB_GPIO_PIN_22) == GPIO_FUN_ANALOG) && (dac_device->channels & DAC_CHANNEL_1)) {
        dac_channel_enable_check |= DAC_CHANNEL_1;
    }

    if (dac_device->sample_freq == 8000) {
        dac_div = DAC_CLK_DIV_64;
    } else if (dac_device->sample_freq == 16000) {
        dac_div = DAC_CLK_DIV_32;
    } else if (dac_device->sample_freq == 32000) {
        dac_div = DAC_CLK_DIV_16;
    } else if (dac_device->sample_freq == 512000) {
        dac_div = DAC_CLK_DIV_1;
    } else {
        printf("not support this freq!\r\n");
        return -1;
    }

    memset(&gpdac_cfg, 0, sizeof(gpdac_cfg));
    memset(&gpdac_chA_cfg, 0, sizeof(gpdac_chA_cfg));
    memset(&gpdac_chB_cfg, 0, sizeof(gpdac_chB_cfg));

    gpdac_cfg.refSel = dac_device->vref;
    gpdac_cfg.resetChanA = 1;
    gpdac_cfg.resetChanB = 1;
    gpdac_cfg.div = dac_div;
    if (dac_device->caps & COMMON_CAPS_DMA_TX) {
        gpdac_cfg.dmaEn = 1;
        if (dac_channel_enable_check == 2) {
            gpdac_cfg.dmaFmt = GPIP_DAC_DMA_FORMAT_1;
        } else {
            gpdac_cfg.dmaFmt = GPIP_DAC_DMA_FORMAT_0;
        }
        gpdac_chA_cfg.src = GPIP_DAC_ChanA_SRC_DMA;
        gpdac_chB_cfg.src = GPIP_DAC_ChanB_SRC_DMA;
    } else {
        gpdac_chA_cfg.src = GPIP_DAC_ChanA_SRC_REG;
        gpdac_chB_cfg.src = GPIP_DAC_ChanB_SRC_REG;
    }

    /*dac vref select*/
    if (dac_device->vref == DAC_VREF_EXTERNAL) {
        /*
        if (GLB_GPIO_Get_Fun(GLB_GPIO_PIN_7) != GPIO_FUN_ANALOG)
            return -1;
        */
    }

    if (dac_channel_enable_check & DAC_CHANNEL_0) {
        gpdac_chA_cfg.chanEn = 1;
        gpdac_chA_cfg.outputEn = 1;
        gpdac_chA_cfg.chanCovtEn = 1;
    }

    if (dac_channel_enable_check & DAC_CHANNEL_1) {
        gpdac_chA_cfg.chanEn = 1;
        gpdac_chA_cfg.outputEn = 1;
        gpdac_chA_cfg.chanCovtEn = 1;
        gpdac_chB_cfg.chanEn = 1;
        gpdac_chB_cfg.outputEn = 1;
        gpdac_chB_cfg.chanCovtEn = 1;
    }

    GLB_GPIP_DAC_Init(&gpdac_cfg);
    GLB_GPIP_DAC_Set_ChanA_Config(&gpdac_chA_cfg);
    GLB_GPIP_DAC_Set_ChanB_Config(&gpdac_chB_cfg);

    return 0;
}

int hal_dac_deinit(int dac_id)
{
//    dac_device_t *dac_device = &dac_devices[dac_id];

    GLB_GPIP_DAC_ChanA_Cfg_Type chCfg = { 0 };
    GLB_GPIP_DAC_Cfg_Type dacCfg = { 0 };
    GLB_GPIP_DAC_Init(&dacCfg);
    GLB_GPIP_DAC_Set_ChanA_Config(&chCfg);
    GLB_GPIP_DAC_Set_ChanB_Config((GLB_GPIP_DAC_ChanB_Cfg_Type *)&chCfg);
    GPIP_Set_DAC_DMA_TX_Disable();
    return 0;
}

int hal_dac_control(int dac_id, int cmd, void *args)
{
    dac_device_t *dac_device = &dac_devices[dac_id];

    switch (cmd) {
        case DAC_CTRL_TX_DMA:
            dac_device->tx_dma = (int)args;
            break;

        default:
            break;
    }

    return 0;
}

int hal_dac_write(int dac_id, int channel, const void *buffer, uint32_t size)
{
    int ret = 0;
    uint32_t i = 0;
    dac_device_t *dac_device = &dac_devices[dac_id];

    if (dac_device->caps & COMMON_CAPS_DMA_TX) {
        int dma_ch = dac_device->tx_dma;

        if (dma_ch < 0) {
            return -1;
        }

        ret = dma_reload(DMA0_ID, dma_ch, (uint32_t)buffer, (uint32_t)DMA_ADDR_DAC_TDR, size, NULL);
        dma_channel_start(DMA0_ID, dma_ch);
        return ret;
    } else {
        if ((channel & DAC_CHANNEL_ALL) == DAC_CHANNEL_ALL) {
            for (i = 0; i < size; i++) {
                GLB_DAC_Set_ChanA_Value(*((uint16_t *)buffer + i));
                GLB_DAC_Set_ChanB_Value(*((uint16_t *)buffer + i));
            }
        } else if (channel & DAC_CHANNEL_0) {
            for (i = 0; i < size; i++) {
                GLB_DAC_Set_ChanA_Value(*((uint16_t *)buffer + i));
            }
        } else if (channel & DAC_CHANNEL_1) {
            for (i = 0; i < size; i++) {
                GLB_DAC_Set_ChanB_Value(*((uint16_t *)buffer + i));
            }
        } else {
            return -1;
        }
        return 0;
    }

    return -1;
}

