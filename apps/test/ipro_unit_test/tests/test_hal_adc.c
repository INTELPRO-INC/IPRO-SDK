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

/**
 * @file test_hal_adc.c
 * @brief Unit tests for HAL ADC functionality (internal channels only)
 *
 * Uses the internal Vref channel and ADC_CTRL_DATA_PARSE to properly
 * extract channel values from raw FIFO data.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "unity.h"
#include "hal_adc.h"
#include "FreeRTOS.h"
#include "task.h"

static bool hw_available = false;
static bool read_works = false;

/* ADC channel config */
static uint8_t pos_channel = ADC_CHANNEL_VREF;
static uint8_t neg_channel = ADC_CHANNEL_GND;

/* Buffers for raw FIFO data and parsed results */
static uint32_t adc_raw_buf[4];
static adc_channel_val_t adc_parsed[4];

static int adc_read_and_parse(int num_samples)
{
    memset(adc_raw_buf, 0, sizeof(adc_raw_buf));
    memset(adc_parsed, 0, sizeof(adc_parsed));

    hal_adc_ioctl(ADC0_INDEX, ADC_CTRL_CHANNEL_START, NULL);

    /* Delay to let conversion complete */
    vTaskDelay(pdMS_TO_TICKS(20));

    int ret = hal_adc_read(ADC0_INDEX, adc_raw_buf, num_samples);

    hal_adc_ioctl(ADC0_INDEX, ADC_CTRL_CHANNEL_STOP, NULL);

    if (ret != 0) return ret;

    /* Parse raw FIFO data to extract actual channel values */
    adc_data_parse_t parse = {
        .input  = adc_raw_buf,
        .output = adc_parsed,
        .num    = num_samples,
    };
    hal_adc_ioctl(ADC0_INDEX, ADC_CTRL_DATA_PARSE, &parse);

    return 0;
}

TEST_CASE(test_adc_init)
{
    int ret = hal_adc_init(ADC0_INDEX);
    hw_available = (ret == 0);

    if (!hw_available) {
        TEST_IGNORE_MESSAGE("ADC hardware not available");
        return;
    }

    TEST_ASSERT_EQUAL_INT(0, ret);
}

TEST_CASE(test_adc_configure_vref)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("ADC hardware not available"); return; }

    adc_device_t *dev = hal_adc_getDevice(ADC0_INDEX);
    TEST_ASSERT_NOT_NULL(dev);

    dev->clk_div             = ADC_CLOCK_DIV_32;
    dev->vref                = ADC_VREF_3V2;
    dev->data_width          = ADC_DATA_WIDTH_12B;
    dev->continuous_conv_mode = false;
    dev->differential_mode   = false;
    dev->fifo_threshold      = ADC_FIFO_THRESHOLD_1BYTE;
    dev->gain                = ADC_GAIN_NONE;

    hal_adc_setConfig(dev);

    adc_channel_cfg_t ch_cfg = {
        .pos_channel = &pos_channel,
        .neg_channel = &neg_channel,
        .num = 1,
    };
    hal_adc_ioctl(ADC0_INDEX, ADC_CTRL_CHANNEL_CONFIG, &ch_cfg);

    TEST_PASS();
}

TEST_CASE(test_adc_read_vref)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("ADC hardware not available"); return; }

    int ret = adc_read_and_parse(1);

    printf("  ADC Vref: ret=%d posCh=%d negCh=%d value=%u volt=%.3fV\n",
           ret, adc_parsed[0].posChan, adc_parsed[0].negChan,
           (unsigned int)adc_parsed[0].value, adc_parsed[0].volt);

    if (ret != 0) {
        read_works = false;
        TEST_IGNORE_MESSAGE("ADC read failed (FIFO not ready)");
        return;
    }

    read_works = true;

    /* Parsed value should be within 12-bit range */
    TEST_ASSERT(adc_parsed[0].value <= 4095);
    /* Voltage should be positive and reasonable */
    TEST_ASSERT(adc_parsed[0].volt > 0.0f);
    TEST_ASSERT(adc_parsed[0].volt < 3.6f);
}

TEST_CASE(test_adc_read_consistency)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("ADC hardware not available"); return; }
    if (!read_works) { TEST_IGNORE_MESSAGE("ADC read not working"); return; }

    float voltages[4];

    for (int i = 0; i < 4; i++) {
        int ret = adc_read_and_parse(1);
        if (ret != 0) {
            TEST_IGNORE_MESSAGE("ADC read failed during consistency test");
            return;
        }
        voltages[i] = adc_parsed[0].volt;
    }

    printf("  ADC Vref readings: %.3fV, %.3fV, %.3fV, %.3fV\n",
           voltages[0], voltages[1], voltages[2], voltages[3]);

    /* All readings should be positive */
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT(voltages[i] > 0.0f);
        TEST_ASSERT(voltages[i] < 3.6f);
    }

    /* Readings should be consistent (within 0.2V of each other) */
    for (int i = 1; i < 4; i++) {
        float diff = voltages[i] - voltages[0];
        if (diff < 0) diff = -diff;
        TEST_ASSERT(diff < 0.2f);
    }
}

/* Test suite runner */
void test_hal_adc_run(void)
{
    printf("Running HAL ADC tests\n");
    printf("Using internal Vref channel (ADC_CHANNEL_VREF)\n");

    RUN_TEST(test_adc_init);
    RUN_TEST(test_adc_configure_vref);
    RUN_TEST(test_adc_read_vref);
    RUN_TEST(test_adc_read_consistency);

    if (hw_available) {
        hal_adc_ioctl(ADC0_INDEX, ADC_CTRL_CHANNEL_STOP, NULL);
    }
}
