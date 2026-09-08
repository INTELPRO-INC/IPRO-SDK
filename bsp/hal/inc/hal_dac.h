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

#ifndef __HAL_DAC__H__
#define __HAL_DAC__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common.h"
#include "board_config.h"

enum dac_index_type {
    DAC0_INDEX,
    DAC_MAX_INDEX
};

enum {
    DAC_CTRL_TX_DMA = 0,
};

#define DAC_CHANNEL_0   (1 << 0)
#define DAC_CHANNEL_1   (1 << 1)
#define DAC_CHANNEL_ALL (DAC_CHANNEL_0 | DAC_CHANNEL_1)

/*output Voltage = (1.8V-0.2V) * digital_val/1024 + 0.2V */
#define DAC_VREF_INTERNAL 0 /*0.2V~1.8V*/
/*output Voltage = (0.9vref-0.1vref) * digital_val/1024 + 0.1vref */
#define DAC_VREF_EXTERNAL 1 /*0.1vref~0.9vref,using gpio7 for GPIO_FUN_ADC*/

enum dac_sample_frequence {
    DAC_SAMPLE_FREQ_8KHZ = 8000,
    DAC_SAMPLE_FREQ_16KHZ = 16000,
    DAC_SAMPLE_FREQ_32KHZ = 32000,
    DAC_SAMPLE_FREQ_512KHZ = 512000
};

typedef struct dac_device {
    int id;
    uint16_t caps;
    enum dac_sample_frequence sample_freq;
    uint8_t channels;
    uint8_t vref;
    int tx_dma;
} dac_device_t;

void hal_dac_setConfig(dac_device_t *dev);
int hal_dac_init(int dac_id);
int hal_dac_deinit(int dac_id);
int hal_dac_control(int dac_id, int cmd, void *args);
int hal_dac_write(int dac_id, int channel, const void *buffer, uint32_t size);

#ifdef __cplusplus
}
#endif
#endif
