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

/*
 * I2S DMIC Test - Record audio from digital microphone via I2S interface
 *
 * GPIO Pin Mapping (function 3 = I2S):
 *   GPIO11 = I2S0_DI   (data in from DMIC)
 *   GPIO12 = I2S0_BCLK (bit clock, master output)
 *   GPIO13 = I2S0_FS   (frame sync / LRCK, master output)
 *   GPIO14 = I2S0_DO   (data out, unused but configured to avoid floating)
 *
 * Audio format: stereo 24-bit 16kHz (I2S standard mode, master RX)
 * SPH0645LM4H-1: OSR=64, BCLK=WS*64, 24-bit data in 32-bit frame
 *
 * Usage: i2s_dmic [seconds]   (default 5, range 1~20)
 */

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "shell.h"
#include <hal_dma.h>
#include <hal_i2s.h>
#include <drv_gpio.h>
#include <drv_glb.h>

/* Filesystem - EVFS POSIX API */
#include "evfs.h"
#include "evfs_posix.h"
#include <unistd.h>
#include <fcntl.h>

#define DMIC_DMA_BUF_SIZE   4096
#define DMIC_BYTES_PER_SEC  (16000 * 2 * sizeof(uint32_t))  /* stereo 32-bit frame 16kHz = 128KB/s */
#define DMIC_MAX_SECONDS    20
#define DMIC_HEX_DUMP_SIZE  128

static uint8_t __attribute__((aligned(4))) dmic_dma_buf[2][DMIC_DMA_BUF_SIZE];
static uint8_t *dmic_rec_buf;
static uint32_t dmic_rec_size;
static volatile uint32_t dmic_rec_offset;
static volatile int dmic_rec_done;

static void dmic_rx_callback(void *dev, uint32_t event)
{
    int idx = (event == 0) ? 0 : 1;
    uint8_t *src = dmic_dma_buf[idx];
    uint32_t remaining = dmic_rec_size - dmic_rec_offset;
    uint32_t len = (remaining < DMIC_DMA_BUF_SIZE) ? remaining : DMIC_DMA_BUF_SIZE;

    if (len > 0) {
        L1C_DCACHE_INVALID_RANGE((uintptr_t)src, DMIC_DMA_BUF_SIZE);
        memcpy(dmic_rec_buf + dmic_rec_offset, src, len);
        dmic_rec_offset += len;
    }

    if (dmic_rec_offset >= dmic_rec_size)
        dmic_rec_done = 1;
}

static void dmic_pinmux_init(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioFun  = GPIO_FUN_I2S,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive    = 1,
        .smtCtrl  = 1,
    };
    uint8_t pins[] = {
        GLB_GPIO_PIN_11,    /* I2S0_DI   - DMIC data in */
        GLB_GPIO_PIN_12,    /* I2S0_BCLK - master clock out */
        GLB_GPIO_PIN_13,    /* I2S0_FS   - frame sync out */
        GLB_GPIO_PIN_14,    /* I2S0_DO   - unused, avoid floating */
    };

    for (int i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        cfg.gpioPin = pins[i];
        GLB_GPIO_Init(&cfg);
    }
}

static void dmic_hex_dump(const uint8_t *buf, int len)
{
    printf("\n--- First %d bytes ---\n", len);
    for (int i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
        if ((i & 0xf) == 0xf)
            printf("\n");
    }
    if (len & 0xf)
        printf("\n");
}

static int dmic_save_file(const char *path, const uint8_t *buf, uint32_t size)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0) {
        printf("Error: cannot open %s\n", path);
        return -1;
    }
    int written = write(fd, buf, size);
    close(fd);
    if (written != (int)size) {
        printf("Error: write %d/%lu bytes\n", written, (unsigned long)size);
        return -1;
    }
    printf("Saved %lu bytes to %s\n", (unsigned long)size, path);
    return 0;
}

static int cmd_i2s_dmic(int argc, char **argv)
{
    int seconds = 5;

    if (argc > 1)
        seconds = atoi(argv[1]);

    if (seconds < 1 || seconds > DMIC_MAX_SECONDS) {
        printf("Usage: i2s_dmic [1~%d], default 5\n", DMIC_MAX_SECONDS);
        return -1;
    }

    dmic_rec_size = seconds * DMIC_BYTES_PER_SEC;
    dmic_rec_buf = pvPortMalloc(dmic_rec_size);
    if (!dmic_rec_buf) {
        printf("Error: malloc %lu bytes failed\n", (unsigned long)dmic_rec_size);
        return -1;
    }
    dmic_rec_offset = 0;
    dmic_rec_done = 0;

    printf("I2S DMIC: stereo 24-bit/32-frame 16kHz, %d sec (%lu bytes)\n",
           seconds, (unsigned long)dmic_rec_size);

    /* Configure GPIO11-14 as I2S function */
    dmic_pinmux_init();

    /* Set I2S ref clock source to 48MHz (default 2.032MHz is too low for 32-bit frame)
     * With 2.032MHz: overSampleRate/64 = 1, causes bclk divider underflow
     * With 48MHz: BCLK = 48M / 46 = 1.043MHz, fs ≈ 16.3kHz */
    GLB_Set_I2S_CLK_Sel(GLB_I2S_REF_CLK_SRC_48M, 0);

    /* I2S config: master RX, stereo 24-bit 16kHz
     * SPH0645LM4H-1 requires: OSR=64, BCLK >= 1.024MHz
     * frame_size=32 -> BCLK = 16kHz * 2ch * 32bit = 1.024MHz */
    i2s_device_t i2s_dev = {
        .id             = 0,
        .iis_mode       = I2S_MODE_MASTER,
        .interface_mode = I2S_MODE_STD,
        .sampl_freq_hz  = 16000,
        .channel_num    = I2S_FS_CHANNELS_NUM_2,
        .frame_size     = I2S_FRAME_LEN_32,
        .data_size      = I2S_DATA_LEN_24,
        .fifo_threshold = 3,
    };
    hal_i2s_setConfig(&i2s_dev);
    hal_i2s_init(0, COMMON_CAPS_DMA_RX);

    /* Left-justify 24-bit data in 32-bit FIFO: {data[23:0], 8'h0}
     * Gives proper sign extension when played as s32le */
    hal_i2s_control(0, I2S_CTRL_SET_FIFO_24B_LJ, (void *)1);

    /* DMA config: pingpong RX */
    dma_device_t rx_dma = {
        .id             = DMA0_ID,
        .ch             = DMA0_CH0_INDEX,
        .transfer_mode  = DMA_LLI_PINGPONG_MODE,
        .direction      = DMA_PERIPH_TO_MEMORY,
        .src_req        = DMA_REQUEST_I2S_RX,
        .dst_req        = DMA_REQUEST_NONE,
        .src_addr_inc   = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc   = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR1,
        .dst_burst_size = DMA_BURST_INCR1,
        .src_width      = DMA_TRANSFER_WIDTH_32BIT,
        .dst_width      = DMA_TRANSFER_WIDTH_32BIT,
        .lli_cfg        = NULL,
        .callback       = dmic_rx_callback,
    };
    dma_setConfig(&rx_dma);
    dma_init(DMA0_ID, rx_dma.ch);
    dma_ioctl(DMA0_ID, rx_dma.ch, DMA_CTRL_SET_INT, NULL);
    hal_i2s_control(0, I2S_CTRL_ATTACH_RX_DMA, (void *)(uintptr_t)rx_dma.ch);

    /* Start DMA capture */
    hal_i2s_read(0, dmic_dma_buf[0], DMIC_DMA_BUF_SIZE);

    printf("Recording...\n");
    while (!dmic_rec_done)
        vTaskDelay(pdMS_TO_TICKS(100));

    /* Stop DMA and I2S */
    dma_channel_stop(DMA0_ID, rx_dma.ch);

    printf("Recording done.\n");

    /* Hex dump first 128 bytes */
    int dump_len = (dmic_rec_size < DMIC_HEX_DUMP_SIZE)
                 ? dmic_rec_size : DMIC_HEX_DUMP_SIZE;
    dmic_hex_dump(dmic_rec_buf, dump_len);

    /* Save to SD card */
    dmic_save_file("/sdcard/audio/dmic_rec.pcm", dmic_rec_buf, dmic_rec_size);

    vPortFree(dmic_rec_buf);
    dmic_rec_buf = NULL;
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_i2s_dmic, i2s_dmic, I2S DMIC record test);
