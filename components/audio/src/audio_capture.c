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
 * @file audio_capture.c
 * @brief Audio capture producer with multi-consumer multicast
 *
 * Supports AMIC, DMIC (both via AUADC HAL), and I2S input sources.
 * DMA ping-pong buffers → ISR callback → multicast to StreamBuffers.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <stream_buffer.h>
#include <task.h>
#include <hal_cache_alloc.h>

#include <hal_auadc.h>
#include <hal_dma.h>
#include <system_soc.h>

#include "audio_capture.h"

#define TAG "AUDIO_CAP"

/* ------------------------------------------------------------------ */
/* Internal state                                                      */
/* ------------------------------------------------------------------ */

typedef struct {
    StreamBufferHandle_t stream;
    volatile uint8_t     active;
} consumer_slot_t;

static struct {
    audio_capture_cfg_t  cfg;
    int16_t             *dma_buf[2];    /* ping-pong pair */
    consumer_slot_t      consumers[AUDIO_CAPTURE_MAX_CONSUMERS];
    volatile uint8_t     running;
    volatile uint8_t     initialized;
    TaskHandle_t         i2s_task;      /* only used for I2S source */
} g_cap;

/* ------------------------------------------------------------------ */
/* Multicast helper — pushes one frame to every active consumer        */
/* ------------------------------------------------------------------ */

static inline void multicast_frame_isr(const void *data, size_t len,
                                       BaseType_t *woken)
{
    for (int i = 0; i < AUDIO_CAPTURE_MAX_CONSUMERS; i++) {
        if (g_cap.consumers[i].active && g_cap.consumers[i].stream)
            xStreamBufferSendFromISR(g_cap.consumers[i].stream,
                                     data, len, woken);
    }
}

static inline void multicast_frame(const void *data, size_t len)
{
    for (int i = 0; i < AUDIO_CAPTURE_MAX_CONSUMERS; i++) {
        if (g_cap.consumers[i].active && g_cap.consumers[i].stream)
            xStreamBufferSend(g_cap.consumers[i].stream,
                              data, len, 0);
    }
}

/* ------------------------------------------------------------------ */
/* AUADC callback (AMIC / DMIC) — ISR context                         */
/* ------------------------------------------------------------------ */

static void auadc_frame_callback(int buf_idx)
{
    if (!g_cap.running)
        return;

    uint32_t frame_bytes = g_cap.cfg.frame_samples * sizeof(int16_t);

    /* Buffer is cache-line aligned, so bare INVALID is safe —
     * no adjacent heap data shares the same cache line. */
    L1C_DCACHE_INVALID_RANGE(g_cap.dma_buf[buf_idx], frame_bytes);

    BaseType_t woken = pdFALSE;
    multicast_frame_isr(g_cap.dma_buf[buf_idx], frame_bytes, &woken);
    portYIELD_FROM_ISR(woken);
}

/* ------------------------------------------------------------------ */
/* I2S capture task — runs in task context (not ISR)                    */
/* ------------------------------------------------------------------ */

#ifdef BSP_USING_I2S0
#include <hal_i2s.h>
#include <hal_dma.h>

static void i2s_capture_task(void *arg)
{
    (void)arg;
    uint32_t frame_bytes = g_cap.cfg.frame_samples * sizeof(int16_t);
    int buf_idx = 0;

    while (g_cap.running) {
        int16_t *buf = g_cap.dma_buf[buf_idx];

        /* hal_i2s_read starts DMA and returns immediately.
         * We poll for DMA completion with a short sleep. */
        if (hal_i2s_read(g_cap.cfg.i2s.i2s_id, buf, frame_bytes) != 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        /* Wait for DMA to finish (frame_samples / sample_rate seconds) */
        uint32_t frame_ms = (g_cap.cfg.frame_samples * 1000) /
                            g_cap.cfg.sample_rate;
        if (frame_ms < 1) frame_ms = 1;
        vTaskDelay(pdMS_TO_TICKS(frame_ms));

        L1C_DCACHE_CLEAN_INVALID_RANGE(buf, frame_bytes);
        multicast_frame(buf, frame_bytes);

        buf_idx ^= 1;
    }

    g_cap.i2s_task = NULL;
    vTaskDelete(NULL);
}
#endif /* BSP_USING_I2S0 */

/* ------------------------------------------------------------------ */
/* Source-specific init                                                 */
/* ------------------------------------------------------------------ */

static int init_amic(void)
{
    hal_auadc_amic_cfg_t cfg = {
        .amic_single_ended = g_cap.cfg.amic.single_ended,
        .amic_pos_ch       = g_cap.cfg.amic.pos_ch,
        .amic_neg_ch       = g_cap.cfg.amic.neg_ch,
        .pcm_frame_size    = g_cap.cfg.frame_samples * sizeof(int16_t),
        .pcm_frame_buf     = { g_cap.dma_buf[0], g_cap.dma_buf[1] },
        .pcm_frame_event   = auadc_frame_callback,
    };

    hal_auadc_set_analog_gain(g_cap.cfg.amic.gain_db);
    return hal_auadc_amic_init(&cfg);
}

static int init_dmic(void)
{
    hal_auadc_pdm_cfg_t cfg = {
        .pdm_clk_pin     = g_cap.cfg.dmic.clk_pin,
        .pdm_in_pin      = g_cap.cfg.dmic.data_pin,
        .pdm_ch          = g_cap.cfg.dmic.channel,
        .pcm_frame_size  = g_cap.cfg.frame_samples * sizeof(int16_t),
        .pcm_frame_buf   = { g_cap.dma_buf[0], g_cap.dma_buf[1] },
        .pcm_frame_event = auadc_frame_callback,
    };

    hal_auadc_set_digital_gain(g_cap.cfg.dmic.digital_gain_db);
    return hal_auadc_pdm_init(&cfg);
}

static int init_i2s(void)
{
#ifdef BSP_USING_I2S0
    i2s_device_t dev = {
        .id             = g_cap.cfg.i2s.i2s_id,
        .iis_mode       = g_cap.cfg.i2s.master ? I2S_MODE_MASTER : I2S_MODE_SLAVE,
        .interface_mode = I2S_MODE_STD,
        .sampl_freq_hz  = g_cap.cfg.sample_rate,
        .channel_num    = (g_cap.cfg.i2s.channels == 1) ?
                          I2S_FS_CHANNELS_NUM_MONO : I2S_FS_CHANNELS_NUM_2,
        .frame_size     = I2S_FRAME_LEN_16,
        .data_size      = I2S_DATA_LEN_16,
        .fifo_threshold = 4,
        .tx_dma         = -1,
        .rx_dma         = -1,
    };

    hal_i2s_setConfig(&dev);
    int ret = hal_i2s_init(dev.id, COMMON_CAPS_DMA_RX);
    if (ret != 0)
        return ret;

    /* Attach RX DMA channel — use CH3 (CH0=AUADC, CH1=UAC, CH2=NCM) */
    int dma_ch = DMA0_CH3_INDEX;
    hal_i2s_control(dev.id, I2S_CTRL_ATTACH_RX_DMA, &dma_ch);
    hal_i2s_control(dev.id, I2S_CTRL_SET_SAMPL_FREQ, &g_cap.cfg.sample_rate);

    return 0;
#else
    printf("[%s] I2S not available (BSP_USING_I2S0 not defined)\r\n", TAG);
    return -1;
#endif
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

int audio_capture_init(const audio_capture_cfg_t *cfg)
{
    if (g_cap.initialized) {
        printf("[%s] Already initialized\r\n", TAG);
        return -1;
    }

    memset(&g_cap, 0, sizeof(g_cap));
    g_cap.cfg = *cfg;

    /* Allocate DMA ping-pong buffers (contiguous, cache-line aligned) */
    uint32_t frame_bytes = cfg->frame_samples * sizeof(int16_t);
    g_cap.dma_buf[0] = (int16_t *)pvPortMallocCacheAligned(frame_bytes * 2);
    if (!g_cap.dma_buf[0]) {
        printf("[%s] Failed to allocate DMA buffers (%lu bytes)\r\n",
               TAG, (unsigned long)(frame_bytes * 2));
        return -1;
    }
    g_cap.dma_buf[1] = g_cap.dma_buf[0] + cfg->frame_samples;

    /* Source-specific hardware init */
    int ret;
    switch (cfg->type) {
    case AUDIO_SRC_AMIC: ret = init_amic(); break;
    case AUDIO_SRC_DMIC: ret = init_dmic(); break;
    case AUDIO_SRC_I2S:  ret = init_i2s();  break;
    default:
        printf("[%s] Unknown source type %d\r\n", TAG, cfg->type);
        ret = -1;
    }

    if (ret != 0) {
        vPortFreeCacheAligned(g_cap.dma_buf[0]);
        g_cap.dma_buf[0] = NULL;
        return ret;
    }

    g_cap.initialized = 1;
    printf("[%s] Init: type=%d, rate=%lu, frame=%u samples\r\n",
           TAG, cfg->type, (unsigned long)cfg->sample_rate,
           cfg->frame_samples);
    return 0;
}

void audio_capture_deinit(void)
{
    if (!g_cap.initialized)
        return;

    audio_capture_stop();

    /* Remove all consumers */
    for (int i = 0; i < AUDIO_CAPTURE_MAX_CONSUMERS; i++)
        audio_capture_remove_consumer(i);

    if (g_cap.dma_buf[0]) {
        vPortFreeCacheAligned(g_cap.dma_buf[0]);
        g_cap.dma_buf[0] = NULL;
        g_cap.dma_buf[1] = NULL;
    }

    /* Release the DMA channel back to the allocator (no-op when
     * allocator is disabled). Without this, every capture session
     * leaks its channel for the lifetime of the process. */
    if (g_cap.cfg.type == AUDIO_SRC_AMIC || g_cap.cfg.type == AUDIO_SRC_DMIC) {
        hal_auadc_deinit();
    }

    g_cap.initialized = 0;
    printf("[%s] Deinitialized\r\n", TAG);
}

int audio_capture_start(void)
{
    if (!g_cap.initialized)
        return -1;
    if (g_cap.running)
        return 0;

    g_cap.running = 1;

    switch (g_cap.cfg.type) {
    case AUDIO_SRC_AMIC:
    case AUDIO_SRC_DMIC:
        hal_auadc_start();
        break;
    case AUDIO_SRC_I2S:
#ifdef BSP_USING_I2S0
        xTaskCreate(i2s_capture_task, "i2s_cap", 512, NULL, 6, &g_cap.i2s_task);
#endif
        break;
    }

    printf("[%s] Capture started\r\n", TAG);
    return 0;
}

int audio_capture_stop(void)
{
    if (!g_cap.running)
        return 0;

    g_cap.running = 0;

    switch (g_cap.cfg.type) {
    case AUDIO_SRC_AMIC:
    case AUDIO_SRC_DMIC:
        hal_auadc_stop();
        /* Don't dma_close(CH0) here: DAC may have set up CH1 in
         * parallel for duplex (audio_duplex). dma_close issues a
         * DMA_Channel_Init with a zeroed config which is per-channel
         * but the surrounding framework re-init can perturb adjacent
         * channels via the global DMA_Disable/Enable pair in dma_init.
         * Cleanup of CH0 is left to the next user that needs it (the
         * DAC framework handles its own channel reset). */
        break;
    case AUDIO_SRC_I2S:
        /* i2s_capture_task checks g_cap.running and exits */
        break;
    }

    printf("[%s] Capture stopped\r\n", TAG);
    return 0;
}

int audio_capture_add_consumer(uint32_t buf_size)
{
    if (!g_cap.initialized)
        return -1;

    uint32_t trigger = g_cap.cfg.frame_samples * sizeof(int16_t);

    for (int i = 0; i < AUDIO_CAPTURE_MAX_CONSUMERS; i++) {
        if (!g_cap.consumers[i].active) {
            g_cap.consumers[i].stream =
                xStreamBufferCreate(buf_size, trigger);
            if (!g_cap.consumers[i].stream) {
                printf("[%s] Failed to create consumer %d StreamBuffer\r\n",
                       TAG, i);
                return -1;
            }
            g_cap.consumers[i].active = 1;
            printf("[%s] Consumer %d registered (buf=%lu)\r\n",
                   TAG, i, (unsigned long)buf_size);
            return i;
        }
    }

    printf("[%s] No consumer slots available\r\n", TAG);
    return -1;
}

void audio_capture_remove_consumer(int consumer_id)
{
    if (consumer_id < 0 || consumer_id >= AUDIO_CAPTURE_MAX_CONSUMERS)
        return;

    consumer_slot_t *slot = &g_cap.consumers[consumer_id];
    if (slot->active) {
        slot->active = 0;
        if (slot->stream) {
            vStreamBufferDelete(slot->stream);
            slot->stream = NULL;
        }
        printf("[%s] Consumer %d removed\r\n", TAG, consumer_id);
    }
}

size_t audio_capture_read(int consumer_id, void *buf, size_t size,
                          uint32_t timeout_ms)
{
    if (consumer_id < 0 || consumer_id >= AUDIO_CAPTURE_MAX_CONSUMERS)
        return 0;

    consumer_slot_t *slot = &g_cap.consumers[consumer_id];
    if (!slot->active || !slot->stream)
        return 0;

    TickType_t ticks;
    if (timeout_ms == 0)
        ticks = 0;
    else if (timeout_ms >= portMAX_DELAY)
        ticks = portMAX_DELAY;
    else
        ticks = pdMS_TO_TICKS(timeout_ms);
    return xStreamBufferReceive(slot->stream, buf, size, ticks);
}

uint32_t audio_capture_get_sample_rate(void)
{
    return g_cap.cfg.sample_rate;
}

uint16_t audio_capture_get_frame_samples(void)
{
    return g_cap.cfg.frame_samples;
}

int audio_capture_is_running(void)
{
    return g_cap.running;
}
