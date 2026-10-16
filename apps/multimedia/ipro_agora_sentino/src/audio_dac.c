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
 * @file audio_dac.c
 * @brief DAC playback — audio_output_ops_t implementation
 *
 * Triple-buffer circular DMA DAC output at 32kHz.
 * Extracted from app_dac_simple.c.
 *
 * Architecture: Triple-buffer circular DMA
 * - Uses DMA_LLI_CYCLE_N_MODE for continuous playback
 * - 3 buffers cycling (0->1->2->0...)
 * - Speaker enable GPIO22 is ACTIVE LOW (0=unmute, 1=mute)
 */

#include "audio_dac.h"

#include <FreeRTOS.h>
#include <hal_dac.h>
#include <hal_dma.h>
#include <hal_gpio.h>
#include "board_config.h"
#include <semphr.h>
#include <stdio.h>
#include <string.h>
#include <system_soc.h>
#include <task.h>

#include "app_log.h"

#define LOG_TAG "DAC"

/* DAC Hardware Configuration.
 * The DAC is GPIO23 everywhere; the speaker enable differs per board:
 * EVB GPIO22 active LOW, IPRO7AI EVK GPIO22 active HIGH (TPA2037D1),
 * IPRO7AI Reef GPIO25 active HIGH (PAM8302A SD#). The board configs carry
 * the pin and level as BOARD_SPK_EN_PIN / BOARD_SPK_EN_ACTIVE. */
#define DAC_GPIO     23
#if defined(CONFIG_IPRO7AI_EVK) || defined(CONFIG_IPRO7AI_REEF)
#define DAC_SPK_GPIO      BOARD_SPK_EN_PIN
#define DAC_SPK_ON_LEVEL  BOARD_SPK_EN_ACTIVE
#else
#define DAC_SPK_GPIO      22
#define DAC_SPK_ON_LEVEL  0   /* Active LOW: 0=unmute, 1=mute */
#endif
#define DAC_SPK_UNMUTE()  gpio_write(DAC_SPK_GPIO, DAC_SPK_ON_LEVEL)
#define DAC_SPK_MUTE()    gpio_write(DAC_SPK_GPIO, !DAC_SPK_ON_LEVEL)
#define DAC_DMA_CH   DMA0_CH1_INDEX

/* Buffer Configuration */
#define DAC_BUFFER_COUNT   8
#define DAC_SAMPLES_PER_BUF 640  /* 20ms at 32kHz */
#define DAC_TOTAL_SAMPLES  (DAC_SAMPLES_PER_BUF * DAC_BUFFER_COUNT)

/* Volume control: Q15 fixed-point gain */
#define DAC_KNEE_DEFAULT       16384
#define DAC_GAIN_Q15_DEFAULT   13845  /* ~42% = vol 65 */

static int32_t s_dac_gain_q15 = DAC_GAIN_Q15_DEFAULT;
static int s_dac_knee = DAC_KNEE_DEFAULT;

/* Auto-mute */
#define DAC_AUTOMUTE_BUFFERS  100
#define DAC_QUIET_THRESHOLD   200

/* DAC State */
static struct __attribute__((aligned(32))) {
    uint16_t buffer[DAC_TOTAL_SAMPLES];
    volatile uint8_t active_idx;
    volatile bool buffer_ready[DAC_BUFFER_COUNT];
    volatile bool running;
    volatile bool spk_muted;
    volatile uint8_t silence_count;
    SemaphoreHandle_t feed_sem;
} s_dac = {0};

/* DMA Device Configuration */
static dma_device_t dac_dma_dev = {
    .id             = DMA0_ID,
    .ch             = DAC_DMA_CH,
    .transfer_mode  = DMA_LLI_CYCLE_N_MODE,
    .direction      = DMA_MEMORY_TO_PERIPH,
    .src_req        = DMA_REQUEST_NONE,
    .dst_req        = DMA_REQUEST_DAC0,
    .src_addr_inc   = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc   = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_size = DMA_BURST_INCR8,
    .dst_burst_size = DMA_BURST_INCR1,
    .src_width      = DMA_TRANSFER_WIDTH_16BIT,
    .dst_width      = DMA_TRANSFER_WIDTH_16BIT,
    .lli_count      = DAC_BUFFER_COUNT,
    .lli_cfg        = NULL,
    .callback       = NULL,
};

static inline uint16_t *get_buffer_segment(int idx) {
    return &s_dac.buffer[idx * DAC_SAMPLES_PER_BUF];
}

static uint16_t s_last_sample = 512;

static void fill_silence(uint16_t *buf, uint32_t count) {
    int32_t val = s_last_sample;
    for (uint32_t i = 0; i < count; i++) {
        int32_t diff = val - 512;
        if (diff > 0)
            val -= (diff >> 4) + 1;
        else if (diff < 0)
            val -= (diff >> 4) - 1;
        buf[i] = (uint16_t)val;
    }
    s_last_sample = (uint16_t)val;
}

static void dac_dma_callback(void *dev, uint32_t event) {
    if (event != HAL_DMA_INT_COMPLETE)
        return;

    uint8_t completed = s_dac.active_idx;
    s_dac.active_idx = (s_dac.active_idx + 1) % DAC_BUFFER_COUNT;
    s_dac.buffer_ready[completed] = false;

    uint16_t *buf = get_buffer_segment(completed);
    fill_silence(buf, DAC_SAMPLES_PER_BUF);
    L1C_DCACHE_CLEAN_RANGE(buf, DAC_SAMPLES_PER_BUF * sizeof(uint16_t));

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(s_dac.feed_sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* Tilt EQ */
#define EQ_LPF_ALPHA_Q15    6434
#define EQ_BASS_GAIN_Q15    32768
#define EQ_TREBLE_GAIN_Q15  36768
#define EQ_DIFF_GAIN_Q15    (EQ_BASS_GAIN_Q15 - EQ_TREBLE_GAIN_Q15)

static int32_t s_eq_lpf;

static inline int16_t eq_process(int16_t input) {
    int32_t x = input;
    s_eq_lpf += ((int32_t)EQ_LPF_ALPHA_Q15 * (x - s_eq_lpf)) >> 15;
    int32_t out = ((int32_t)EQ_TREBLE_GAIN_Q15 * x +
                   (int32_t)EQ_DIFF_GAIN_Q15 * s_eq_lpf) >> 15;
    if (out > 32767)  out = 32767;
    else if (out < -32768) out = -32768;
    return (int16_t)out;
}

/* Accumulation buffer */
static uint16_t s_accum_buf[DAC_SAMPLES_PER_BUF];
static uint32_t s_accum_pos;

/* LFSR dither */
static uint32_t s_lfsr = 0xACE1u;

static inline uint32_t lfsr_next(void) {
    s_lfsr = (s_lfsr >> 1) ^ (-(s_lfsr & 1u) & 0xD0000001u);
    return s_lfsr;
}

static inline int32_t dither_tpdf(void) {
    int32_t d1 = (int32_t)(lfsr_next() & 1u);
    int32_t d2 = (int32_t)(lfsr_next() & 1u);
    return d1 - d2;
}

/* ------------------------------------------------------------------ */
/* audio_output_ops_t implementation                                  */
/* ------------------------------------------------------------------ */

static app_err_t dac_init(void)
{
    IPRO_LOGI(LOG_TAG, "Initializing DAC (Triple Buffer, 32kHz)");

    if (s_dac.running) {
        IPRO_LOGW(LOG_TAG, "Already initialized");
        return APP_OK;
    }

    if (s_dac.feed_sem == NULL) {
        s_dac.feed_sem = xSemaphoreCreateBinary();
        if (!s_dac.feed_sem) {
            IPRO_LOGE(LOG_TAG, "Failed to create semaphore");
            return APP_ERR_NOMEM;
        }
    }

    s_dac.active_idx = 0;
    for (int i = 0; i < DAC_BUFFER_COUNT; i++)
        s_dac.buffer_ready[i] = false;

    for (int i = 0; i < DAC_BUFFER_COUNT; i++)
        fill_silence(get_buffer_segment(i), DAC_SAMPLES_PER_BUF);
    L1C_DCACHE_CLEAN_RANGE(s_dac.buffer, sizeof(s_dac.buffer));

    GLB_GPIO_Cfg_Type gpio_cfg;
    gpio_cfg.gpioFun  = GPIO_FUN_ANALOG;
    gpio_cfg.pullType = GPIO_PULL_NONE;
    gpio_cfg.drive    = 1;
    gpio_cfg.smtCtrl  = 1;
    gpio_cfg.gpioPin  = GLB_GPIO_PIN_23;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    GLB_GPIO_Init(&gpio_cfg);

    dac_dma_dev.callback = dac_dma_callback;
    dma_setConfig(&dac_dma_dev);
    dma_init(dac_dma_dev.id, dac_dma_dev.ch);
    dma_ioctl(dac_dma_dev.id, dac_dma_dev.ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(dac_dma_dev.id, dac_dma_dev.ch, DMA_CTRL_SET_CALLBACK,
              dac_dma_callback);

    dac_device_t dac_cfg = {
        .caps        = COMMON_CAPS_DMA_TX,
        .channels    = DAC_CHANNEL_0,
        .sample_freq = DAC_SAMPLE_FREQ_32KHZ,
        .vref        = DAC_VREF_INTERNAL,
    };
    hal_dac_setConfig(&dac_cfg);
    hal_dac_init(dac_dma_dev.id);
    hal_dac_control(dac_dma_dev.id, DAC_CTRL_TX_DMA,
                    (void *)(uintptr_t)dac_dma_dev.ch);

    gpio_set_mode(DAC_SPK_GPIO, GPIO_OUTPUT_PP_MODE);
    DAC_SPK_MUTE();

    IPRO_LOGI(LOG_TAG,
              "DAC initialized (GPIO%d, SPK_EN=GPIO%d active-%s, gain_q15=%ld)",
              DAC_GPIO, DAC_SPK_GPIO, DAC_SPK_ON_LEVEL ? "high" : "low",
              (long)s_dac_gain_q15);

    return APP_OK;
}

static app_err_t dac_start(void)
{
    if (s_dac.running)
        return APP_OK;

    IPRO_LOGI(LOG_TAG, "Starting DAC playback...");
    s_dac.active_idx = 0;

    uint32_t segment_bytes = DAC_SAMPLES_PER_BUF * sizeof(uint16_t);
    int ret = dma_reload(dac_dma_dev.id, dac_dma_dev.ch, (uint32_t)s_dac.buffer,
                         DMA_ADDR_DAC_TDR, segment_bytes, NULL);
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "dma_reload failed: %d", ret);
        return APP_ERR_INIT;
    }

    dma_channel_start(dac_dma_dev.id, dac_dma_dev.ch);
    s_dac.running = true;
    DAC_SPK_UNMUTE();

    IPRO_LOGI(LOG_TAG, "DAC started, speaker unmuted");
    return APP_OK;
}

static app_err_t dac_stop(void)
{
    if (!s_dac.running)
        return APP_OK;

    DAC_SPK_MUTE();
    dma_channel_stop(dac_dma_dev.id, dac_dma_dev.ch);
    s_dac.running = false;

    IPRO_LOGI(LOG_TAG, "DAC stopped");
    return APP_OK;
}

static app_err_t dac_feed(const int16_t *pcm_data, uint32_t num_samples)
{
    if (!s_dac.running)
        return APP_ERR_INIT;

    /* Measure peak for auto-mute */
    int16_t peak = 0;
    for (uint32_t i = 0; i < num_samples; i++) {
        int16_t abs_val = (pcm_data[i] >= 0) ? pcm_data[i] : -pcm_data[i];
        if (abs_val > peak)
            peak = abs_val;
    }

    if (peak >= DAC_QUIET_THRESHOLD) {
        if (s_dac.spk_muted) {
            DAC_SPK_UNMUTE();
            s_dac.spk_muted = false;
        }
        s_dac.silence_count = 0;
    } else {
        if (s_dac.silence_count < 255)
            s_dac.silence_count++;
        if (s_dac.silence_count >= DAC_AUTOMUTE_BUFFERS && !s_dac.spk_muted) {
            DAC_SPK_MUTE();
            s_dac.spk_muted = true;
        }
    }

    for (uint32_t i = 0; i < num_samples; i++) {
        int32_t s = eq_process(pcm_data[i]);
        if (s > s_dac_knee)
            s = s_dac_knee + ((s - s_dac_knee) >> 2);
        else if (s < -s_dac_knee)
            s = -s_dac_knee + ((s + s_dac_knee) >> 2);

        int32_t scaled = (s * s_dac_gain_q15) >> 15;
        int32_t dac_raw = (scaled >> 6) + 512;
        int32_t sample = dac_raw;
        if (scaled > 4 || scaled < -4)
            sample += dither_tpdf();
        if (sample < 0)
            sample = 0;
        else if (sample > 1023)
            sample = 1023;

        s_accum_buf[s_accum_pos++] = (uint16_t)sample;
        s_last_sample = (uint16_t)sample;

        if (s_accum_pos >= DAC_SAMPLES_PER_BUF) {
            int fill_idx = -1;
            for (int k = 0; k < DAC_BUFFER_COUNT; k++) {
                int candidate = (s_dac.active_idx + 1 + k) % DAC_BUFFER_COUNT;
                if (!s_dac.buffer_ready[candidate]) {
                    fill_idx = candidate;
                    break;
                }
            }

            if (fill_idx >= 0) {
                uint16_t *dst = get_buffer_segment(fill_idx);
                memcpy(dst, s_accum_buf, DAC_SAMPLES_PER_BUF * sizeof(uint16_t));
                L1C_DCACHE_CLEAN_RANGE(dst, DAC_SAMPLES_PER_BUF * sizeof(uint16_t));
                s_dac.buffer_ready[fill_idx] = true;
            } else {
                static uint32_t drop_count = 0;
                drop_count++;
                if (drop_count > 1 && (drop_count % 100) == 0) {
                    IPRO_LOGW(LOG_TAG, "DAC buffer full, dropped %lu frames",
                              (unsigned long)drop_count);
                }
            }

            s_accum_pos = 0;
        }
    }

    return APP_OK;
}

static void dac_set_volume(int vol)
{
    if (vol < 0) vol = 0;
    if (vol > 100) vol = 100;
    s_dac_gain_q15 = ((int32_t)vol * vol * 32768L) / 10000;
}

static int dac_get_volume(void)
{
    int32_t target = (s_dac_gain_q15 * 10000L) / 32768;
    if (target <= 0) return 0;
    int32_t x = 50;
    for (int i = 0; i < 5; i++) {
        x = (x + target / x) / 2;
    }
    if (x > 100) x = 100;
    return (int)x;
}

const audio_output_ops_t audio_dac_ops = {
    .name       = "dac",
    .init       = dac_init,
    .start      = dac_start,
    .stop       = dac_stop,
    .feed       = dac_feed,
    .set_volume = dac_set_volume,
    .get_volume = dac_get_volume,
};

/* ------------------------------------------------------------------ */
/* Shell commands                                                     */
/* ------------------------------------------------------------------ */
#ifdef CONFIG_SHELL
#include "shell.h"

static void cmd_dac_vol(int argc, char **argv) {
    if (argc < 2) {
        printf("DAC volume: %d/100 (gain_q15=%ld, knee=%d)\r\n",
               dac_get_volume(), (long)s_dac_gain_q15, s_dac_knee);
        return;
    }
    int vol = atoi(argv[1]);
    dac_set_volume(vol);
    printf("DAC volume: %d/100 (gain_q15=%ld, knee=%d)\r\n",
           dac_get_volume(), (long)s_dac_gain_q15, s_dac_knee);
}
SHELL_CMD_EXPORT_ALIAS(cmd_dac_vol, dac_vol, Set DAC volume 0 - 100);
#endif /* CONFIG_SHELL */
