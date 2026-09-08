/**
 * @file adc_demo.c
 * @brief Generic ADC (non-audio) demo
 *
 * Shell command:
 *   adc_demo [dma_en]
 *       - Read ADC_CHANNEL4 voltage in mV.
 *       - dma_en=0: interrupt-driven, polls periodically
 *       - dma_en=1: DMA-driven, one-shot 16-sample burst
 *
 * This file was split out from demo/soc/audio/adc_demo.c where the
 * generic ADC command was lumped together with audio ADC (AMIC/DMIC)
 * for historical reasons. Audio ADC now lives in the audio_capture
 * framework; this file keeps the non-audio use case.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

#include <FreeRTOS.h>
#include <task.h>

#include <hal_adc.h>
#include <hal_dma.h>
#include <hal_gpio.h>
#include <drv_glb.h>

#define ADC_SAMPLE_CHANNEL  (ADC_CHANNEL4)

static adc_data_parse_t data_parse;
static adc_channel_val_t result_val[16];
static uint32_t adc_buffer[16];

static void adc_dma_callback(void *dev, uint32_t event)
{
    (void)dev; (void)event;
    hal_adc_ioctl(0, ADC_CTRL_DATA_PARSE, (void *)&data_parse);
    for (uint8_t i = 0; i < 16; i++) {
        printf("PosId = %d NegId = %d V= %ld mV \r\n",
               result_val[i].posChan, result_val[i].negChan,
               (uint32_t)(result_val[i].volt * 1000));
    }
}

static void adc_get_val_proc(void *arg)
{
    (void)arg;
    while (1) {
        hal_adc_read(0, adc_buffer, 16);
        for (uint8_t i = 0; i < 16; i++) {
            if (result_val[i].posChan == ADC_SAMPLE_CHANNEL) {
                printf("PosId = %d NegId = %d V= %ld mV \r\n",
                       result_val[i].posChan, result_val[i].negChan,
                       (uint32_t)(result_val[i].volt * 1000));
            }
        }
        vTaskDelay(1000);
    }
}

static int cmd_adc_test(int argc, char **argv)
{
    GLB_GPIO_Cfg_Type cfg;
    adc_channel_cfg_t adc_channel_cfg;
    uint8_t posChList[] = { ADC_SAMPLE_CHANNEL };
    uint8_t negChList[] = { ADC_CHANNEL_GND };
    adc_device_t adc0;
    dma_device_t adc_dma_dev;
    int dma_en = 0;

    if (argc == 2) dma_en = atoi(argv[1]);
    printf("ADC DMA: %d\r\n", dma_en);

    adc0.id = 0;
    if (dma_en) {
        adc0.caps = COMMON_CAPS_DMA_RX;
    } else {
        adc0.caps = COMMON_CAPS_INT_RX;
    }
    adc0.continuous_conv_mode = 1;
    adc0.callback = NULL;
    adc0.clk_div = ADC_CLOCK_DIV_32;
    adc0.vref = ADC_VREF_3V2;
    adc0.differential_mode = false;
    adc0.data_width = ADC_DATA_WIDTH_16B_WITH_256_AVERAGE;
    adc0.fifo_threshold = ADC_FIFO_THRESHOLD_1BYTE;
    adc0.gain = ADC_GAIN_1;
    adc0.rx_dma = 0;

    if (dma_en) {
        adc_dma_dev.id = DMA0_ID;
        adc_dma_dev.ch = DMA0_CH0_INDEX;
        adc_dma_dev.transfer_mode = DMA_LLI_ONCE_MODE;
        adc_dma_dev.direction = DMA_PERIPH_TO_MEMORY;
        adc_dma_dev.src_req = DMA_REQUEST_ADC0;
        adc_dma_dev.dst_req = DMA_REQUEST_NONE;
        adc_dma_dev.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        adc_dma_dev.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        adc_dma_dev.src_burst_size = DMA_BURST_INCR1;
        adc_dma_dev.dst_burst_size = DMA_BURST_INCR1;
        adc_dma_dev.src_width = DMA_TRANSFER_WIDTH_32BIT;
        adc_dma_dev.dst_width = DMA_TRANSFER_WIDTH_32BIT;
        adc_dma_dev.lli_cfg = NULL;
        adc_dma_dev.callback = NULL;
    }

    /* ADC GPIO Init (GPIO24 = ADC analog input) */
    cfg.gpioFun = GPIO_FUN_ANALOG;
    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_24;
    cfg.gpioMode = GPIO_MODE_AF;
    GLB_GPIO_Init(&cfg);

    if (dma_en) {
        dma_setConfig(&adc_dma_dev);
        dma_init(adc_dma_dev.id, adc_dma_dev.ch);
        dma_ioctl(adc_dma_dev.id, adc_dma_dev.ch, DMA_CTRL_SET_INT, NULL);
        dma_ioctl(adc_dma_dev.id, adc_dma_dev.ch, DMA_CTRL_SET_CALLBACK,
                  adc_dma_callback);
        dma_reload(adc_dma_dev.id, adc_dma_dev.ch, DMA_ADDR_ADC_RDR,
                   (uint32_t)adc_buffer, sizeof(adc_buffer), NULL);
    }

    adc_channel_cfg.pos_channel = posChList;
    adc_channel_cfg.neg_channel = negChList;
    adc_channel_cfg.num = 1;

    hal_adc_setConfig(&adc0);
    hal_adc_init(adc0.id);
    hal_adc_ioctl(adc0.id, ADC_CTRL_CHANNEL_CONFIG,
                  (void *)(uintptr_t)&adc_channel_cfg);

    data_parse.input = adc_buffer;
    data_parse.output = result_val;
    data_parse.num = 16;

    if (dma_en) {
        hal_adc_ioctl(adc0.id, ADC_CTRL_ATTACH_RX_DMA,
                      (void *)(uintptr_t)adc_dma_dev.ch);
        dma_channel_start(adc_dma_dev.id, adc_dma_dev.ch);
    }
    hal_adc_ioctl(adc0.id, ADC_CTRL_CHANNEL_START, NULL);

    if (!dma_en) {
        xTaskCreate(adc_get_val_proc, "adc_get_val", 1024, NULL, 10, NULL);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_adc_test, adc_demo, Generic ADC Demo);
