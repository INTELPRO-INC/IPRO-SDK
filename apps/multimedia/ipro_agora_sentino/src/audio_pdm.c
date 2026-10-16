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
 * @file audio_pdm.c
 * @brief PDM DMIC (3SM222) audio capture — audio_input_ops_t implementation
 *
 * DMA double-buffered PDM DMIC capture at 16kHz mono.
 * Captured PCM frames are queued to a TX task which calls a registered
 * callback.  Extracted from app_audio.c.
 *
 * 3SM222 PDM DMIC wiring: GPIO0 = CLK, GPIO1 = DATA
 */

#include "audio_pdm.h"
#include "board_config.h"
#if defined(CONFIG_IPRO7AI_EVK)
#include <drv_hbn.h>
#include <hbn_reg.h>
#endif

#include <FreeRTOS.h>
#include <hal_cache_alloc.h>
#include <hal_auadc.h>
#include <math.h>
#include <queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <system_soc.h>
#include <task.h>

#include "app_log.h"

#define LOG_TAG "AUDIO_PDM"

/* Audio configuration */
#define AUDIO_SAMPLE_RATE   16000
#define AUDIO_CHANNELS      1
#define AUDIO_FRAME_MS      20
#define AUDIO_FRAME_SAMPLES (AUDIO_SAMPLE_RATE * AUDIO_FRAME_MS / 1000) /* 320 */
#define AUDIO_FRAME_BYTES   (AUDIO_FRAME_SAMPLES * 2) /* 640 bytes */

/* Audio queue for sending frames from ISR to task */
#define AUDIO_QUEUE_LENGTH 4
static QueueHandle_t g_audio_tx_queue;

/* TX frame callback (matches audio_frame_cb_t signature) */
static audio_frame_cb_t g_tx_cb;

static TaskHandle_t g_audio_tx_task;
static volatile bool g_pdm_initialized;

/* PDM DMIC pin configuration.
 * EVB: external 3SM222 on GPIO0 (CLK) / GPIO1 (DATA).
 * IPRO7AI EVK: on-board IM69D129 on GPIO18/19. Those are AON pads that the
 * always-on block owns at reset with their input receivers off; they must be
 * handed back to the GLB mux before AUADC PDM can see the data line
 * (components/board/ipro7ai_evk/src/evk_mic.c found this).
 * IPRO7AI Reef: on-board MSM261 on GPIO16 (CLK) / GPIO17 (DATA), L/R tied to
 * GND so it talks in the left slot; ordinary GLB pads, nothing to claim. */
#if defined(CONFIG_IPRO7AI_EVK) || defined(CONFIG_IPRO7AI_REEF)
#define PDM_CLK_PIN  BOARD_MIC_CLK_PIN
#define PDM_DATA_PIN BOARD_MIC_DAT_PIN
#else
#define PDM_CLK_PIN  0
#define PDM_DATA_PIN 1
#endif
#define PDM_DEFAULT_DIGITAL_GAIN 18 /* dB */

/* Double buffering for PDM capture */
static int16_t *g_capture_buf[2];
static volatile int g_capture_enabled;

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

static void audio_tx_task(void *param)
{
    int16_t *frame_ptr;
    (void)param;

    IPRO_LOGI(LOG_TAG, "Audio TX task started");

    while (1) {
        if (xQueueReceive(g_audio_tx_queue, &frame_ptr, pdMS_TO_TICKS(1000)) == pdTRUE) {
            if (g_tx_cb) {
                g_tx_cb(frame_ptr, AUDIO_FRAME_SAMPLES);
            }
        }
    }
}

static void audio_capture_callback(int buf_idx)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (!g_capture_enabled)
        return;

    L1C_DCACHE_INVALID_RANGE(g_capture_buf[buf_idx], AUDIO_FRAME_BYTES);

    xQueueSendFromISR(g_audio_tx_queue, &g_capture_buf[buf_idx],
                      &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* ------------------------------------------------------------------ */
/* audio_input_ops_t implementation                                   */
/* ------------------------------------------------------------------ */

#if defined(CONFIG_IPRO7AI_EVK)
/* Same sequence as components/board/ipro7ai_evk/src/evk_mic.c claim_pads():
 * HBN_PAD_CTRL_0[27:20] / HBN_PAD_CTRL_2[11:10] hold the AON "I own this
 * pad" bits, and the pads double as the 32.768 kHz crystal pins. */
static void pdm_claim_aon_pads(void)
{
    uint32_t p2 = readl(HBN_BASE + HBN_PAD_CTRL_2_OFFSET);
    p2 &= HBN_REG_EN_AON_CTRL_GPIO_MSB_UMSK;   /* top two AON pads -> GLB */
    p2 |= HBN_REG_AON_GPIO_IE_SMT_MSB_MSK;     /* input receivers on */
    writel(p2, HBN_BASE + HBN_PAD_CTRL_2_OFFSET);

    uint32_t p0 = readl(HBN_BASE + HBN_PAD_CTRL_0_OFFSET);
    p0 &= HBN_REG_EN_AON_CTRL_GPIO_7_0_UMSK;
    p0 |= HBN_REG_AON_GPIO_IE_SMT_MSK;
    writel(p0, HBN_BASE + HBN_PAD_CTRL_0_OFFSET);

    uint32_t x = readl(HBN_BASE + HBN_XTAL32K_OFFSET);
    if (!(x & HBN_XTAL32K_HIZ_EN_MSK)) {
        HBN_32K_Sel(HBN_32K_RC);
        x &= HBN_PU_XTAL32K_UMSK;
        x &= HBN_PU_XTAL32K_BUF_UMSK;
        x |= HBN_XTAL32K_HIZ_EN_MSK;
        writel(x, HBN_BASE + HBN_XTAL32K_OFFSET);
    }
    IPRO_LOGI(LOG_TAG, "EVK AON pads GPIO%d/%d released to GLB", PDM_CLK_PIN, PDM_DATA_PIN);
}
#endif

static app_err_t pdm_init(void)
{
    hal_auadc_pdm_cfg_t cfg;

    if (g_audio_tx_queue == NULL) {
        g_audio_tx_queue = xQueueCreate(AUDIO_QUEUE_LENGTH, sizeof(int16_t *));
        if (g_audio_tx_queue == NULL) {
            IPRO_LOGE(LOG_TAG, "Failed to create audio TX queue");
            return APP_ERR_NOMEM;
        }
    }

    if (g_audio_tx_task == NULL) {
        BaseType_t ret = xTaskCreate(audio_tx_task, "audio_tx", 4096, NULL,
                                     tskIDLE_PRIORITY + 3, &g_audio_tx_task);
        if (ret != pdPASS) {
            IPRO_LOGE(LOG_TAG, "Failed to create audio TX task");
            return APP_ERR_NOMEM;
        }
    }

    if (g_capture_buf[0] == NULL) {
        g_capture_buf[0] = (int16_t *)pvPortMallocCacheAligned(AUDIO_FRAME_BYTES * 2);
        if (g_capture_buf[0] == NULL) {
            IPRO_LOGE(LOG_TAG, "Failed to allocate capture buffers");
            return APP_ERR_NOMEM;
        }
        g_capture_buf[1] = g_capture_buf[0] + AUDIO_FRAME_SAMPLES;
    }

#if defined(CONFIG_IPRO7AI_EVK)
    pdm_claim_aon_pads();
#endif
    cfg.pdm_clk_pin       = PDM_CLK_PIN;
    cfg.pdm_in_pin        = PDM_DATA_PIN;
    cfg.pdm_ch            = HAL_AUADC_PDM_CH_LEFT;
    cfg.pcm_frame_size    = AUDIO_FRAME_BYTES;
    cfg.pcm_frame_buf[0]  = g_capture_buf[0];
    cfg.pcm_frame_buf[1]  = g_capture_buf[1];
    cfg.pcm_frame_event   = audio_capture_callback;

    hal_auadc_pdm_init(&cfg);
    hal_auadc_set_digital_gain(PDM_DEFAULT_DIGITAL_GAIN);

    IPRO_LOGI(LOG_TAG,
              "PDM DMIC initialized: %d Hz, %d ch, %d samples/frame, "
              "CLK=GPIO%d, DATA=GPIO%d, digital_gain=%d dB",
              AUDIO_SAMPLE_RATE, AUDIO_CHANNELS, AUDIO_FRAME_SAMPLES,
              PDM_CLK_PIN, PDM_DATA_PIN, PDM_DEFAULT_DIGITAL_GAIN);

    g_pdm_initialized = true;
    return APP_OK;
}

static app_err_t pdm_start(void)
{
    if (g_capture_buf[0] == NULL) {
        IPRO_LOGE(LOG_TAG, "Capture not initialized");
        return APP_ERR_INIT;
    }
    g_capture_enabled = 1;
    hal_auadc_start();
    return APP_OK;
}

static app_err_t pdm_stop(void)
{
    g_capture_enabled = 0;
    hal_auadc_stop();
    return APP_OK;
}

static void pdm_set_callback(audio_frame_cb_t cb)
{
    g_tx_cb = cb;
}

static app_err_t pdm_set_gain(int gain)
{
    hal_auadc_set_digital_gain(gain);
    IPRO_LOGI(LOG_TAG, "PDM digital gain set to %d dB", gain);
    return APP_OK;
}

const audio_input_ops_t audio_pdm_ops = {
    .name         = "pdm",
    .init         = pdm_init,
    .start        = pdm_start,
    .stop         = pdm_stop,
    .set_callback = pdm_set_callback,
    .set_gain     = pdm_set_gain,
};

/* ------------------------------------------------------------------ */
/* Shell debug commands                                               */
/* ------------------------------------------------------------------ */
#ifdef CONFIG_SHELL
#include "shell.h"

static volatile int g_mic_dbg_active;
static volatile int32_t g_dbg_peak;
static volatile int64_t g_dbg_sum_sq;
static volatile uint32_t g_dbg_frame_count;

static void pdm_dbg_update(const int16_t *pcm, uint32_t num_samples)
{
    if (!g_mic_dbg_active)
        return;

    int32_t peak = 0;
    int64_t sum_sq = 0;

    for (uint32_t i = 0; i < num_samples; i++) {
        int32_t s = pcm[i];
        int32_t abs_s = s < 0 ? -s : s;
        if (abs_s > peak)
            peak = abs_s;
        sum_sq += s * s;
    }

    g_dbg_peak = peak;
    g_dbg_sum_sq = sum_sq;
    g_dbg_frame_count++;
}

static int cmd_mic_dbg(int argc, char **argv)
{
    int duration = 5;
    if (argc > 1) {
        duration = atoi(argv[1]);
        if (duration <= 0 || duration > 60)
            duration = 5;
    }

    if (!g_pdm_initialized) {
        printf("PDM DMIC not initialized\r\n");
        return -1;
    }

    printf("\r\nPDM DMIC signal monitor (%d seconds)\r\n", duration);
    printf("CLK=GPIO%d DATA=GPIO%d  %dHz mono\r\n\r\n",
           PDM_CLK_PIN, PDM_DATA_PIN, AUDIO_SAMPLE_RATE);

    audio_frame_cb_t prev_cb = g_tx_cb;
    g_tx_cb = pdm_dbg_update;
    g_mic_dbg_active = 1;

    int was_enabled = g_capture_enabled;
    if (!was_enabled) {
        pdm_start();
    }

    int total_frames = 0;
    for (int t = 0; t < duration * 5; t++) {
        vTaskDelay(pdMS_TO_TICKS(200));

        uint32_t fc = g_dbg_frame_count;
        int32_t peak = g_dbg_peak;
        int64_t sum_sq = g_dbg_sum_sq;

        float rms = 0;
        if (fc > 0 && AUDIO_FRAME_SAMPLES > 0) {
            rms = sqrtf((float)sum_sq / AUDIO_FRAME_SAMPLES);
        }

        float peak_dbfs = peak > 0 ? 20.0f * log10f((float)peak / 32768.0f) : -96.0f;
        float rms_dbfs = rms > 1.0f ? 20.0f * log10f(rms / 32768.0f) : -96.0f;

        int bar_len = (int)((peak_dbfs + 96.0f) / 3.0f);
        if (bar_len < 0) bar_len = 0;
        if (bar_len > 32) bar_len = 32;
        char bar[33];
        memset(bar, '#', bar_len);
        memset(bar + bar_len, ' ', 32 - bar_len);
        bar[32] = '\0';

        printf("Peak:%6ld (%5.1f dBFS)  RMS:%5.0f (%5.1f dBFS)  [%s] frames:%lu\r\n",
               (long)peak, (double)peak_dbfs,
               (double)rms, (double)rms_dbfs,
               bar, (unsigned long)fc);

        total_frames += fc;
        g_dbg_frame_count = 0;
    }

    g_mic_dbg_active = 0;
    g_tx_cb = prev_cb;

    if (!was_enabled) {
        pdm_stop();
    }

    printf("\r\nTotal frames: %d (expected ~%d)\r\n",
           total_frames, duration * 1000 / AUDIO_FRAME_MS);

    if (total_frames == 0) {
        printf("WARNING: No frames received! Check:\r\n");
        printf("  - 3SM222 wiring: CLK=GPIO%d, DATA=GPIO%d\r\n", PDM_CLK_PIN, PDM_DATA_PIN);
        printf("  - PDM DMIC power supply\r\n");
        printf("  - DMA channel availability\r\n");
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mic_dbg, mic_dbg, Monitor PDM DMIC signal level [seconds]);

static int cmd_mic_gain(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: mic_gain <dB>  (e.g. mic_gain 18)\r\n");
        return 0;
    }
    int gain = atoi(argv[1]);
    pdm_set_gain(gain);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mic_gain, mic_gain, Set PDM DMIC digital gain in dB);
#endif /* CONFIG_SHELL */
