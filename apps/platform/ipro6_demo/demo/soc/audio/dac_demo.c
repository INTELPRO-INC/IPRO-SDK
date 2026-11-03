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

#include <generated/autoconf.h>
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
#include <hal_dac.h>
#include <hal_gpio.h>
#include <hal_mtimer.h>
#include <drv_glb.h>
#ifdef CONFIG_DEMO_DAC_AUDIO_DATA
#include "dac_demo_audio.h"
#endif
#include "system_soc.h"

#define DAC_GPIO_SPK    (16)

static SemaphoreHandle_t dma_complete_sem = NULL;

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

void demo_dac_dma(uint32_t *p_u32addr, uint32_t bufsize, uint32_t sample_freq)
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

static int cmd_dac_test(int argc, char **argv)
{
    struct stat st;
    const char *path;
    int fd;
    uint32_t buf_len, sample_freq, file_size;
    uint32_t *buff;

    if (argc != 3) {
        printf("Usage: dac_demo <file path> <sameple rate>\r\n");
        return 0;
    }
    path = argv[1];
    sample_freq = atoi(argv[2]);

    if (sample_freq != 8000 && sample_freq != 16000 && sample_freq != 32000 && sample_freq != 512000) {
        printf("Supported sample rate: 8000, 16000, 32000, 512000\r\n");
        return 0;
    }

#ifdef CONFIG_DEMO_DAC_AUDIO_DATA
    if (!strcmp(path, "mem")) {
        demo_dac_dma((uint32_t *)audio_32k, audio_32k_len, sample_freq);
    } else
#endif
    {
        if (stat(path, &st) != 0) {
            printf("stat %s fail\r\n", path);
            return 0;
        }
        file_size = st.st_size;
        buf_len = ((file_size + 511) >> 9) << 9;
        buff = malloc(buf_len);

        printf("Opening file %s\r\n", path);
        fd = open(path, O_RDONLY);
        if (read(fd, buff, file_size) != file_size) {
            printf("read fail\r\n");
        } else {
            demo_dac_dma(buff, file_size, sample_freq);
        }
        free(buff);
        close(fd);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dac_test, dac_demo, DAC Demo);


void demo_gpdac_init(void)
{
    /* Create semaphore for DMA completion */
    if (dma_complete_sem == NULL) {
        dma_complete_sem = xSemaphoreCreateBinary();
    }

}
