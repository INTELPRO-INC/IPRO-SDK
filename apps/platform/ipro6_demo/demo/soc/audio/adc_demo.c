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

#include <stdio.h>

/* Filesystem - EVFS POSIX API */
#include "evfs.h"
#include "evfs_posix.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "shell.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal_dma.h>
#include <hal_adc.h>
#include <hal_dac.h>
#include <hal_gpio.h>
#include <hal_mtimer.h>
#include <hal_auadc.h>


#define FRAME_SIZE (320)
#define FRAME_NUM  (3*10*10)
static uint16_t *audio_buf = NULL;
static uint16_t *audio_buf_processed = NULL;
//static int16_t pcm_buf[2][FRAME_SIZE];
static int16_t *pcm_buf[2] = {NULL, NULL};
static int gAudioBufferPos = 0;
static SemaphoreHandle_t dma_complete_sem = NULL;

void audio_callback(int buf_idx)
{
    L1C_DCACHE_INVALID_RANGE(pcm_buf[buf_idx], FRAME_SIZE* sizeof(int16_t));
    memcpy((void*)audio_buf + gAudioBufferPos, pcm_buf[buf_idx], FRAME_SIZE* sizeof(int16_t));
    gAudioBufferPos += FRAME_SIZE* sizeof(int16_t);
    if (gAudioBufferPos == FRAME_SIZE * sizeof(uint16_t) * FRAME_NUM) {
        hal_auadc_stop();
        L1C_DCACHE_CLEAN_RANGE(audio_buf, gAudioBufferPos);
        printf("record done, %p %p\r\n", audio_buf, (void*)audio_buf + gAudioBufferPos);
#if 1
        for (int i = 0; i < gAudioBufferPos/2; i++) {
            //audio_buf_processed[i] = (audio_buf[i] + 32768) * 0.0156099794003204;
            audio_buf_processed[i] = (audio_buf[i] + 32768) >> 6;
            //       audio_buf_processed[i + 1] = val;
        }
#endif
    }
}

static void _auadc_pdm_init(int gain)
{
    hal_auadc_pdm_cfg_t cfg;

    if (pcm_buf[0] == NULL) {
        pcm_buf[0] = (int16_t *)malloc(FRAME_SIZE * sizeof(int16_t) * 2);
        pcm_buf[1] = pcm_buf[0] + FRAME_SIZE;
    }

    cfg.pdm_clk_pin = 34;
    cfg.pdm_in_pin = 35;
    cfg.pdm_ch = HAL_AUADC_PDM_CH_RIGHT;
    cfg.pcm_frame_size = FRAME_SIZE * sizeof(uint16_t);
    cfg.pcm_frame_buf[0] = pcm_buf[0];
    cfg.pcm_frame_buf[1] = pcm_buf[1];
    cfg.pcm_frame_event = audio_callback;

    if (audio_buf == NULL) {
        audio_buf = (uint16_t *)malloc(FRAME_SIZE * FRAME_NUM * sizeof(uint16_t));
        audio_buf_processed = (void *)malloc(FRAME_SIZE * FRAME_NUM * sizeof(uint16_t));
        printf("audio_buf = %p\r\n", audio_buf);
    }
    gAudioBufferPos = 0;

    hal_auadc_pdm_init(&cfg);
    hal_auadc_set_digital_gain(gain);
    printf("gain: %u\r\n", gain);
}

static void _auadc_amic_init(int gain)
{
    hal_auadc_amic_cfg_t cfg;

    if (pcm_buf[0] == NULL) {
        pcm_buf[0] = (int16_t *)malloc(FRAME_SIZE * sizeof(int16_t) * 2);
        pcm_buf[1] = pcm_buf[0] + FRAME_SIZE;
    }

    cfg.amic_single_ended = 0;
    cfg.amic_pos_ch = 4;
    cfg.amic_neg_ch = 7;
    cfg.pcm_frame_size = FRAME_SIZE* sizeof(int16_t);
    cfg.pcm_frame_buf[0] = pcm_buf[0];
    cfg.pcm_frame_buf[1] = pcm_buf[1];
    cfg.pcm_frame_event = audio_callback;

    if (audio_buf == NULL) {
        audio_buf = (void *)malloc(FRAME_SIZE * FRAME_NUM * sizeof(uint16_t));
        audio_buf_processed = (void *)malloc(FRAME_SIZE * FRAME_NUM * sizeof(uint16_t));
        printf("audio_buf = %p\r\n", audio_buf);
    }
    hal_auadc_set_analog_gain(gain);
    printf("gain: %u\r\n", gain);
    gAudioBufferPos = 0;

    hal_auadc_amic_init(&cfg);
}

static int cmd_auadc_test(int argc, char **argv)
{
    int gain = 18;
    if (argc > 1) {
        gain = atoi(argv[1]);
    }
    gAudioBufferPos = 0;
    _auadc_amic_init(gain);
    hal_auadc_start();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_auadc_test, auadc_demo, AUADC Demo);

static int cmd_aupdm_test(int argc, char **argv)
{
    int gain = 0x30;
    if (argc > 1) {
        gain = atoi(argv[1]);
    }
    gAudioBufferPos = 0;
    _auadc_pdm_init(gain);
    hal_auadc_start();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_aupdm_test, aupdm_demo, AUPDM Demo);

#define DAC_GPIO_SPK    (16)

static void dma_callback(void *dev, uint32_t event)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (event == HAL_DMA_INT_COMPLETE) {
        /* Give semaphore from ISR */
        xSemaphoreGiveFromISR(dma_complete_sem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static dma_device_t dac_dma_dev = {
    .id = DMA0_ID,
    .ch = DMA0_CH0_INDEX,
    .transfer_mode = DMA_LLI_ONCE_MODE,
    .direction = DMA_MEMORY_TO_PERIPH,
    .src_req = DMA_REQUEST_NONE,
    .dst_req = DMA_REQUEST_DAC0,
    .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_size = DMA_BURST_INCR8,
    .dst_burst_size = DMA_BURST_INCR1,
    .src_width = DMA_TRANSFER_WIDTH_16BIT,
    .dst_width = DMA_TRANSFER_WIDTH_16BIT,
    .lli_cfg = NULL,
    .callback = dma_callback,
};

void replay_record(uint32_t *p_u32addr, uint32_t bufsize, uint32_t sample_freq)
{
    int ret = 0;
    GLB_GPIO_Cfg_Type cfg;

    dac_device_t dac0 = {
        .caps = COMMON_CAPS_DMA_TX,
        .channels = DAC_CHANNEL_0,
        .sample_freq = DAC_SAMPLE_FREQ_16KHZ,
        .vref = DAC_VREF_INTERNAL
    };

    dac0.sample_freq = sample_freq;

    // DAC GPIO Init
    cfg.gpioFun = GPIO_FUN_ANALOG;
    cfg.pullType = GPIO_PULL_NONE;
    cfg.drive = 1;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_23;
    cfg.gpioMode = GPIO_MODE_AF;
    GLB_GPIO_Init(&cfg);

    /* Configure DMA0_CH1 for USB EP1 TX using dma_setConfig */
    dma_setConfig(&dac_dma_dev);
    dma_init(dac_dma_dev.id, dac_dma_dev.ch);

    /* Enable DMA interrupt */
    dma_ioctl(dac_dma_dev.id, dac_dma_dev.ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(dac_dma_dev.id, dac_dma_dev.ch, DMA_CTRL_SET_CALLBACK, dma_callback);

    hal_dac_setConfig(&dac0);
    hal_dac_init(dac_dma_dev.id);

    hal_dac_control(dac_dma_dev.id, DAC_CTRL_TX_DMA, (void *)(uintptr_t)dac_dma_dev.ch);
    gpio_set_mode(DAC_GPIO_SPK, GPIO_OUTPUT_PP_MODE);
    gpio_write(DAC_GPIO_SPK, 1);

    L1C_DCACHE_CLEAN_RANGE(p_u32addr, bufsize);
    ret = hal_dac_write(dac_dma_dev.id, DAC_CHANNEL_0, p_u32addr, bufsize);

    if (ret != 0) {
        printf("dac start failed \r\n");
        return;
    }
    printf("Wait for DMA\n");
    if (xSemaphoreTake(dma_complete_sem, portMAX_DELAY) == pdTRUE) {
        printf("dac dma complete\r\n");
        gpio_write(DAC_GPIO_SPK, 0);
    }
}

static int cmd_replay_record(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: dac_replay_record <sameple rate>\r\n");
        return 0;
    }
    int sample_freq = atoi(argv[1]);
    replay_record((uint32_t *)audio_buf_processed, gAudioBufferPos, sample_freq);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_replay_record, replay_record, Replay Record Demo);

#define AUADC_SAVE_FILE_NAME    "/sdcard/audio/rec.pcm"
static int cmd_auadc_save_test(int argc, char **argv)
{
    int fd = open(AUADC_SAVE_FILE_NAME, O_RDWR | O_CREAT | O_TRUNC);
    if (fd < 0) {
        printf("open file failed\r\n");
        return 0;
    }
    if (write(fd, audio_buf, gAudioBufferPos) != gAudioBufferPos) {
        printf("write file failed\r\n");
    }
    close(fd);
    printf("Audio record save to %s, size %d\r\n", AUADC_SAVE_FILE_NAME, gAudioBufferPos);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_auadc_save_test, aud_save, AUADC save file demo);

#define ADC_SAMPLE_CHANNEL  (ADC_CHANNEL4)
static adc_data_parse_t data_parse;
adc_channel_val_t result_val[16];
uint32_t adc_buffer[16];
void adc_dma_callback(void *dev, uint32_t event)
{
    hal_adc_ioctl(0, ADC_CTRL_DATA_PARSE, (void*)&data_parse);
    for (uint8_t i = 0; i < 16; i++) {
        printf("PosId = %d NegId = %d V= %ld mV \r\n", result_val[i].posChan, result_val[i].negChan, (uint32_t)(result_val[i].volt * 1000));
    }
}
#if 0
void adc_irq_callback(void *dev, void *args, uint32_t size, uint32_t state)
{
    if (state == ADC_EVENT_FIFO) {
        MSG("FIFO Ready Interrupt Happend \r\n");
        adc_channel_val_t *adc_parse_val = (adc_channel_val_t *)args;
        adc_device_t *adc_dev = (adc_device_t *)dev;
        for (uint32_t i = 0; i < size; i++) {
            MSG("PosId = %d NegId = %d V= %lu mV\r\n", adc_parse_val[i].posChan, adc_parse_val[i].negChan, (unsigned long)(adc_parse_val[i].volt * 1000));
        }

        hal_adc_ioctl(adc_dev->id, ADC_CTRL_CLEAR_INT, (void *)(ADC_FIFO_IT));
    } else {
    }
}
#endif
static void adc_get_val_proc(void *arg)
{
    while (1) {
        hal_adc_read(0, adc_buffer, 16);
        for (uint8_t i = 0; i < 16; i++) {
            if (result_val[i].posChan == ADC_SAMPLE_CHANNEL) {
                printf("PosId = %d NegId = %d V= %ld mV \r\n", result_val[i].posChan, result_val[i].negChan, (uint32_t)(result_val[i].volt * 1000));
            }
        }
        vTaskDelay(1000);
    }
    vTaskDelete(NULL);
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

    if (argc == 2)
        dma_en = atoi(argv[1]);

    printf("ADC DMA: %d\r\n", dma_en);

    adc0.id = 0;
    if (dma_en) {
        adc0.caps = COMMON_CAPS_DMA_RX;
        adc0.continuous_conv_mode = 1;
        adc0.callback = NULL;
    } else {
        adc0.caps = COMMON_CAPS_INT_RX;
        adc0.continuous_conv_mode = 1;
        adc0.callback = NULL;
    }
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

    // ADC GPIO Init
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
        dma_ioctl(adc_dma_dev.id, adc_dma_dev.ch, DMA_CTRL_SET_CALLBACK, adc_dma_callback);
        dma_reload(adc_dma_dev.id, adc_dma_dev.ch, DMA_ADDR_ADC_RDR, (uint32_t)adc_buffer, sizeof(adc_buffer), NULL);
    }

    adc_channel_cfg.pos_channel = posChList;
    adc_channel_cfg.neg_channel = negChList;
    adc_channel_cfg.num = 1;

    hal_adc_setConfig(&adc0);
    hal_adc_init(adc0.id);

    hal_adc_ioctl(adc0.id, ADC_CTRL_CHANNEL_CONFIG, (void *)(uintptr_t)&adc_channel_cfg);
    data_parse.input = adc_buffer;
    data_parse.output = result_val;
    data_parse.num = 16;
    if (dma_en) {
        hal_adc_ioctl(adc0.id, ADC_CTRL_ATTACH_RX_DMA, (void *)(uintptr_t)adc_dma_dev.ch);
        dma_channel_start(adc_dma_dev.id, adc_dma_dev.ch);
    } else {
        //hal_adc_ioctl(adc0.id, ADC_CTRL_SET_INT, (void *)ADC_FIFO_IT);
        //hal_adc_ioctl(adc0.id, ADC_CTRL_ADD_INT_CB, adc_irq_callback);
    }
    hal_adc_ioctl(adc0.id, ADC_CTRL_CHANNEL_START, NULL);

    if (!dma_en) {
        xTaskCreate(adc_get_val_proc, "adc_get_val", 1024, NULL, 10, NULL);
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_adc_test, adc_demo, ADC Demo);


void demo_auadc_init(void)
{
    if (dma_complete_sem == NULL) {
        dma_complete_sem = xSemaphoreCreateBinary();
    }
}
