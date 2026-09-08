/**
 * @file audio_output_i2s.c
 * @brief I2S audio output sink
 *
 * Outputs decoded PCM to an external DAC via I2S interface.
 * Supports native sample rates (44.1kHz, 48kHz, etc.) without resampling.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#include "audio_output.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <hal_dma.h>
#include <hal_i2s.h>
#include "system_soc.h"

/* Legacy default when CONFIG_DMA_CHANNEL_ALLOCATOR is off. With the
 * allocator enabled, the channel is obtained at start-time via
 * dma_request_channel(DMA_REQUEST_I2S_TX, ...). */
#define I2S_DMA_CH_LEGACY   DMA0_CH2_INDEX
#define I2S_DMA_BUF_SAMPLES 1152
#define I2S_DMA_BUF_SIZE    (I2S_DMA_BUF_SAMPLES * 2 * sizeof(int16_t))  /* stereo */

struct audio_output {
    audio_output_cfg_t  cfg;
    int16_t            *dma_buf;
    SemaphoreHandle_t   dma_sem;
    int                 started;
    uint8_t             dma_ch;       /* resolved in start() */
#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    dma_ch_handle_t     dma_handle;
#endif
};

static audio_output_t *s_i2s_out;

static void i2s_dma_callback(void *dev, uint32_t event)
{
    BaseType_t woken = pdFALSE;
    if (event == HAL_DMA_INT_COMPLETE && s_i2s_out) {
        xSemaphoreGiveFromISR(s_i2s_out->dma_sem, &woken);
        portYIELD_FROM_ISR(woken);
    }
}

/* .ch is patched at start() from out->dma_ch (allocator or legacy). */
static dma_device_t i2s_dma_dev = {
    .id = DMA0_ID,
    .ch = I2S_DMA_CH_LEGACY,
    .transfer_mode = DMA_LLI_ONCE_MODE,
    .direction = DMA_MEMORY_TO_PERIPH,
    .src_req = DMA_REQUEST_NONE,
    .dst_req = DMA_REQUEST_I2S_TX,
    .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_size = DMA_BURST_INCR4,
    .dst_burst_size = DMA_BURST_INCR4,
    .src_width = DMA_TRANSFER_WIDTH_16BIT,
    .dst_width = DMA_TRANSFER_WIDTH_16BIT,
    .lli_cfg = NULL,
    .callback = i2s_dma_callback,
};

/* Forward declaration */
int audio_output_i2s_stop(audio_output_t *out);

audio_output_t *audio_output_i2s_create(const audio_output_cfg_t *cfg)
{
    audio_output_t *out = pvPortMalloc(sizeof(audio_output_t));
    if (!out) {
        return NULL;
    }
    memset(out, 0, sizeof(*out));
    out->cfg = *cfg;

    out->dma_buf = pvPortMalloc(I2S_DMA_BUF_SIZE);
    if (!out->dma_buf) {
        vPortFree(out);
        return NULL;
    }

    out->dma_sem = xSemaphoreCreateBinary();
    if (!out->dma_sem) {
        vPortFree(out->dma_buf);
        vPortFree(out);
        return NULL;
    }

    return out;
}

void audio_output_i2s_destroy(audio_output_t *out)
{
    if (!out) {
        return;
    }
    audio_output_i2s_stop(out);
    if (out->dma_sem) {
        vSemaphoreDelete(out->dma_sem);
    }
    vPortFree(out->dma_buf);
    vPortFree(out);
    if (s_i2s_out == out) {
        s_i2s_out = NULL;
    }
}

int audio_output_i2s_start(audio_output_t *out)
{
    if (!out || out->started) {
        return -1;
    }

    s_i2s_out = out;

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    out->dma_handle = dma_request_channel(DMA_REQUEST_I2S_TX,
                                          DMA_MEMORY_TO_PERIPH,
                                          "audio_i2s");
    if (!out->dma_handle) {
        printf("i2s_out: DMA channel request failed\n");
        return -1;
    }
    out->dma_ch = (uint8_t)dma_handle_to_ch(out->dma_handle);
#else
    out->dma_ch = I2S_DMA_CH_LEGACY;
#endif
    i2s_dma_dev.ch = out->dma_ch;

    /* I2S device config */
    i2s_device_t i2s_dev = {
        .id = 0,
        .caps = COMMON_CAPS_DMA_TX,
        .iis_mode = I2S_MODE_MASTER,
        .interface_mode = I2S_MODE_STD,
        .sampl_freq_hz = out->cfg.sample_rate,
        .channel_num = I2S_FS_CHANNELS_NUM_2,
        .frame_size = I2S_FRAME_LEN_16,
        .data_size = I2S_DATA_LEN_16,
        .fifo_threshold = 4,
    };

    hal_i2s_setConfig(&i2s_dev);
    hal_i2s_init(0, COMMON_CAPS_DMA_TX);

    /* DMA init */
    dma_setConfig(&i2s_dma_dev);
    dma_init(i2s_dma_dev.id, i2s_dma_dev.ch);
    dma_ioctl(i2s_dma_dev.id, i2s_dma_dev.ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(i2s_dma_dev.id, i2s_dma_dev.ch, DMA_CTRL_SET_CALLBACK,
              i2s_dma_callback);

    hal_i2s_control(0, I2S_CTRL_ATTACH_TX_DMA,
                    (void *)(uintptr_t)i2s_dma_dev.ch);

    out->started = 1;
    printf("i2s_out: started (%lu Hz, %d ch, dma_ch=%d)\n",
           (unsigned long)out->cfg.sample_rate, out->cfg.channels,
           out->dma_ch);
    return 0;
}

int audio_output_i2s_stop(audio_output_t *out)
{
    if (!out || !out->started) {
        return 0;
    }
    out->started = 0;

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    if (out->dma_handle) {
        dma_release_channel(out->dma_handle);
        out->dma_handle = NULL;
    }
#endif
    return 0;
}

int audio_output_i2s_write(audio_output_t *out,
                           const int16_t *pcm, uint32_t samples)
{
    if (!out || !out->started || samples == 0) {
        return -1;
    }

    /* Prepare stereo interleaved data in DMA buffer */
    uint32_t stereo_samples = samples;
    uint32_t byte_len;

    if (out->cfg.channels == 2) {
        byte_len = samples * 2 * sizeof(int16_t);
        memcpy(out->dma_buf, pcm, byte_len);
    } else {
        /* Mono to stereo duplication */
        byte_len = samples * 2 * sizeof(int16_t);
        for (uint32_t i = 0; i < samples; i++) {
            out->dma_buf[i * 2] = pcm[i];
            out->dma_buf[i * 2 + 1] = pcm[i];
        }
    }
    (void)stereo_samples;

    L1C_DCACHE_CLEAN_RANGE(out->dma_buf, byte_len);

    int ret = hal_i2s_write(0, out->dma_buf, byte_len);
    if (ret == 0) {
        xSemaphoreTake(out->dma_sem, pdMS_TO_TICKS(500));
    }

    return ret;
}
