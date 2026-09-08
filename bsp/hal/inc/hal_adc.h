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

#ifndef __HAL_ADC__H__
#define __HAL_ADC__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common.h"
#include "board_config.h"

enum {
    ADC_CTRL_SET_INT = 0,
    ADC_CTRL_CLEAR_INT,
    ADC_CTRL_ADD_INT_CB,
    ADC_CTRL_ATTACH_RX_DMA,
    ADC_CTRL_CHANNEL_START,
    ADC_CTRL_CHANNEL_STOP,
    ADC_CTRL_CHANNEL_CONFIG,
    ADC_CTRL_VBAT_ON,
    ADC_CTRL_VBAT_OFF,
    ADC_CTRL_TSEN_ON,
    ADC_CTRL_TSEN_OFF,
    ADC_CTRL_DATA_PARSE,
};

enum adc_index_type {
    ADC0_INDEX,
    ADC_MAX_INDEX
};

#define adc_channel_start(index)        adc_ioctl(index, ADC_CTRL_CHANNEL_START, NULL)
#define adc_channel_stop(index)         adc_ioctl(index, ADC_CTRL_CHANNEL_STOP, NULL)
#define adc_channel_config(index, list) adc_ioctl(index, ADC_CTRL_CHANNEL_CONFIG, list)

typedef enum {
    ADC_CHANNEL0,          /* ADC channel 0 */
    ADC_CHANNEL1,          /* ADC channel 1 */
    ADC_CHANNEL2,          /* ADC channel 2 */
    ADC_CHANNEL3,          /* ADC channel 3 */
    ADC_CHANNEL4,          /* ADC channel 4 */
    ADC_CHANNEL5,          /* ADC channel 5 */
    ADC_CHANNEL6,          /* ADC channel 6 */
    ADC_CHANNEL7,          /* ADC channel 7 */
    ADC_CHANNEL8,          /* ADC channel 8 */
    ADC_CHANNEL9,          /* ADC channel 9 */
    ADC_CHANNEL10,         /* ADC channel 10 */
    ADC_CHANNEL11,         /* ADC channel 11 */
    ADC_CHANNEL_DAC_OUTA,  /* DACA, ADC channel 12 */
    ADC_CHANNEL_DAC_OUTB,  /* DACB, ADC channel 13 */
    ADC_CHANNEL_TSEN_P,    /* TSenp, ADC channel 14 */
    ADC_CHANNEL_TSEN_N,    /* TSenn, ADC channel 15 */
    ADC_CHANNEL_VREF,      /* Vref, ADC channel 16 */
    ADC_CHANNEL_DCTEST,    /* DCTest, ADC channel 17 */
    ADC_CHANNEL_VABT_HALF, /* VBAT/2, ADC channel 18 */
    ADC_CHANNEL_SENP3,     /* SenVP3, ADC channel 19 */
    ADC_CHANNEL_SENP2,     /* SenVP2, ADC channel 20 */
    ADC_CHANNEL_SENP1,     /* SenVP1, ADC channel 21 */
    ADC_CHANNEL_SENP0,     /* SenVP0, ADC channel 22 */
    ADC_CHANNEL_GND,       /* GND, ADC channel 23 */
} adc_channel_t;

typedef enum {
    ADC_CLOCK_DIV_1,  /*!< ADC clock:on 32M clock is 32M */
    ADC_CLOCK_DIV_4,  /*!< ADC clock:on 32M clock is 8M */
    ADC_CLOCK_DIV_8,  /*!< ADC clock:on 32M clock is 4M */
    ADC_CLOCK_DIV_12, /*!< ADC clock:on 32M clock is 2.666M */
    ADC_CLOCK_DIV_16, /*!< ADC clock:on 32M clock is 2M */
    ADC_CLOCK_DIV_20, /*!< ADC clock:on 32M clock is 1.6M */
    ADC_CLOCK_DIV_24, /*!< ADC clock:on 32M clock is 1.333M */
    ADC_CLOCK_DIV_32, /*!< ADC clock:on 32M clock is 1M */
} adc_clk_div_t;

typedef enum {
    ADC_VREF_3V2 = 0, /* ADC select 3.2V as reference voltage */
    ADC_VREF_2V = 1,  /* ADC select 2V as reference voltage */
} adc_vref_t;

/**
 *  @brief ADC data width type definition
 */
typedef enum {
    ADC_DATA_WIDTH_12B,                  /*!< ADC 12 bits */
    ADC_DATA_WIDTH_14B_WITH_16_AVERAGE,  /*!< ADC 14 bits,and the value is average of 16 converts */
    ADC_DATA_WIDTH_14B_WITH_64_AVERAGE,  /*!< ADC 14 bits,and the value is average of 64 converts */
    ADC_DATA_WIDTH_16B_WITH_128_AVERAGE, /*!< ADC 16 bits,and the value is average of 128 converts */
    ADC_DATA_WIDTH_16B_WITH_256_AVERAGE, /*!< ADC 16 bits,and the value is average of 256 converts */
} adc_data_width_t;

/**
 *  @brief ADC FIFO threshold type definition
 */
typedef enum {
    ADC_FIFO_THRESHOLD_1BYTE,  /*!< ADC FIFO threshold is 1 */
    ADC_FIFO_THRESHOLD_4BYTE,  /*!< ADC FIFO threshold is 4 */
    ADC_FIFO_THRESHOLD_8BYTE,  /*!< ADC FIFO threshold is 8 */
    ADC_FIFO_THRESHOLD_16BYTE, /*!< ADC FIFO threshold is 16 */
} adc_fifo_threshold_t;

/**
 *  @brief ADC PGA gain type definition
 */
typedef enum {
    ADC_GAIN_NONE, /*!< No PGA gain */
    ADC_GAIN_1,    /*!< PGA gain 1 */
    ADC_GAIN_2,    /*!< PGA gain 2 */
    ADC_GAIN_4,    /*!< PGA gain 4 */
    ADC_GAIN_8,    /*!< PGA gain 8 */
    ADC_GAIN_16,   /*!< PGA gain 16 */
    ADC_GAIN_32,   /*!< PGA gain 32 */
} adc_pga_gain_t;

enum adc_event_type {
    ADC_EVENT_UNDERRUN,
    ADC_EVENT_OVERRUN,
    ADC_EVENT_FIFO,
    ADC_EVENT_UNKNOWN
};

enum adc_it_type {
    ADC_UNDERRUN_IT = 1 << 2,
    ADC_OVERRUN_IT = 1 << 3,
    ADC_FIFO_IT = 1 << 5,
};

typedef struct {
    uint8_t *pos_channel;
    uint8_t *neg_channel;
    uint8_t num;
} adc_channel_cfg_t;

typedef struct {
    int8_t posChan; /*!< Positive channel */
    int8_t negChan; /*!< Negative channel */
    uint16_t value; /*!< ADC value */
    float volt;     /*!< ADC voltage result */
} adc_channel_val_t;

typedef struct
{
    uint32_t *input;
    adc_channel_val_t *output;
    uint32_t num;
} adc_data_parse_t;

typedef struct adc_device {
    int id;
    uint16_t caps;
    adc_clk_div_t clk_div;     /* CLK is not more than 2Mhz */
    adc_vref_t vref;           /* ADC voltage reference*/
    bool continuous_conv_mode; /** conversion mode: shot conversion mode or continuous conversion mode. */
    bool differential_mode;    /** Channel type: single-ended or differential. */
    adc_data_width_t data_width;
    adc_fifo_threshold_t fifo_threshold;
    adc_pga_gain_t gain;
    int rx_dma;
    void (*callback)(void *dev, void *args, uint32_t size, uint32_t event);
} adc_device_t;

int hal_adc_init(int adc_id);
int hal_adc_read(int adc_id, void *buffer, uint32_t size);
int hal_adc_trim_tsen(uint16_t *tsen_offset);
float hal_adc_get_tsen(uint16_t tsen_offset);
adc_device_t* hal_adc_getDevice(int index);
void hal_adc_setConfig(adc_device_t *dev);
int hal_adc_ioctl(int adc_id, int cmd, void *args);
#ifdef __cplusplus
}
#endif
#endif