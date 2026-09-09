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
 * @file main.c
 * @brief PDM DMIC Recording Demo with BLE phone control
 *
 * Records audio from a PDM DMIC (GPIO0=CLK, GPIO1=DATA)
 * into a PSRAM buffer using DMA ping-pong. Controllable via shell
 * commands or BLE NUS from a phone app.
 *
 * Shell commands:
 *   pdm_rec [seconds]   — record (default 5s, max 4 hours)
 *   pdm_dump [off] [n]  — hex dump PCM samples
 *   pdm_stats           — show recording statistics
 *   pdm_gain <dB>       — set digital gain at runtime
 *   pdm_ch [left|right] — select PDM channel
 */

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <hal_auadc.h>
#include <system_soc.h>
#include <compiler/common.h>
#include <freertos_app_hooks.h>
#include "shell.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "drv_glb.h"
#include "ipro_log.h"
#include "ble_audio.h"
#include "pdm_app.h"

#define TAG "PDM_DEMO"

/* --- Hardware pin assignments ---
 *
 * Board-dependent, so they come from .config. The defaults are the EVB's
 * GPIO0/1; on the IPRO7AI EVK those two pins are camera DVP D0/D1 and the
 * microphone is on GPIO18/19 instead. The AUADC only accepts clk on 0, 2 or 8
 * and data on 1, 3 or 7 for the EVB-style mux, so a board that differs must
 * set both. */
#ifndef CONFIG_IPRO_PDM_MIC_CLK_PIN
#define CONFIG_IPRO_PDM_MIC_CLK_PIN   0
#endif
#ifndef CONFIG_IPRO_PDM_MIC_DATA_PIN
#define CONFIG_IPRO_PDM_MIC_DATA_PIN  1
#endif
#define PDM_CLK_PIN         CONFIG_IPRO_PDM_MIC_CLK_PIN
#define PDM_DATA_PIN        CONFIG_IPRO_PDM_MIC_DATA_PIN
/* --- Audio parameters --- */
#define SAMPLE_RATE         16000   /* 16 kHz */
#define FRAME_SAMPLES       320     /* 20 ms frame */
#define FRAME_BYTES         (FRAME_SAMPLES * sizeof(int16_t))  /* 640 bytes */
/* AUADC decimator gain, applied before the 16-bit truncation — the only
 * stage that improves resolution rather than scaling up what is already
 * quantised, so it is run at the +18 dB hardware maximum. Measured on the
 * EVB: normal speech then peaks at -19.3 dBFS, i.e. 19 dB of headroom
 * still in hand. See dac_test.c for the rest of the gain staging. */
#define PDM_DIGITAL_GAIN    18      /* dB — hardware max */
#define PDM_GAIN_MIN_DB     -96
#define PDM_GAIN_MAX_DB     18
#define STARTUP_DISCARD     5       /* discard first N frames (DMIC stabilization) */

#ifndef CONFIG_IPRO_PDM_MIC_CCI_ANALYSIS
#define CONFIG_IPRO_PDM_MIC_CCI_ANALYSIS 0
#endif

#define IPRO_GLB_PARM_ADDR              0x30000090UL
#define IPRO_GLB_PARM_CCI_TRANSPORT_MSK ((1UL << 16) | (1UL << 19))

/* --- Recording buffer (PSRAM ring, last 240 s; logical cap 4 hours) --- */
#define MAX_RECORD_SECONDS  BLE_MAX_RECORD_SECONDS
#define REC_BUF_SECONDS     240
#define MAX_RECORD_BYTES    (SAMPLE_RATE * sizeof(int16_t) * REC_BUF_SECONDS)

static int16_t *s_rec_buf = NULL;      /* PSRAM recording ring */
static uint32_t s_rec_cap = MAX_RECORD_BYTES; /* physical ring bytes (may shrink) */
static volatile uint32_t s_write_pos;  /* logical write position in bytes */
static volatile uint32_t s_target_bytes; /* target recording size in bytes */
static volatile bool s_recording;
static volatile uint32_t s_discard_count; /* frames to discard at startup */
static SemaphoreHandle_t s_chunk_ready_sem = NULL;
static uint32_t s_next_chunk_bytes;

/* Streaming mode — sends frames to BLE in near real-time */
static QueueHandle_t s_stream_queue = NULL;
static volatile bool s_streaming;
#define STREAM_QUEUE_LEN    8

/* Stream ring buffer — ISR copies frames here so DMA buffer can be reused */
static int16_t *s_stream_ring = NULL;    /* PSRAM ring buffer of STREAM_QUEUE_LEN frames */
static volatile uint32_t s_stream_widx;  /* write index (0..STREAM_QUEUE_LEN-1) */

/* DMA double buffers — MUST be in OCRAM (not PSRAM) for DMA access */
static int16_t ATTR_NOCACHE_NOINIT_RAM_SECTION
    __attribute__((aligned(32))) s_dma_raw[FRAME_SAMPLES * 2];
static int16_t *s_dma_buf[2];

/* Semaphore: ISR signals recording complete */
static SemaphoreHandle_t s_rec_done_sem = NULL;
/* Semaphore: monitor task signals cleanup done (for shell blocking) */
static SemaphoreHandle_t s_cleanup_done_sem = NULL;

/* Runtime-configurable PDM settings */
static uint8_t s_pdm_channel = HAL_AUADC_PDM_CH_LEFT;
static int s_pdm_gain = PDM_DIGITAL_GAIN;
static int s_rec_gain = PDM_DIGITAL_GAIN;   /* gain the current buffer was captured at */
static volatile bool s_audio_power_on = true;

static void cci_analysis_transport_enable(void)
{
#if CONFIG_IPRO_PDM_MIC_CCI_ANALYSIS
    volatile uint32_t * const glb_parm = (volatile uint32_t *)IPRO_GLB_PARM_ADDR;
    uint32_t v = *glb_parm;

    *glb_parm = v | IPRO_GLB_PARM_CCI_TRANSPORT_MSK;
#endif
}

/* Statistics */
static uint32_t s_frames_captured;
static int16_t s_peak_sample;
static int64_t s_energy_acc;

/* ===================================================================
 * ISR callback: copy DMA frame to PSRAM buffer
 * =================================================================== */
static void pdm_frame_callback(int buf_idx)
{
    BaseType_t woken = pdFALSE;

    /* Invalidate cache for the DMA buffer we're about to read.
     * DMA buffers are in nocache OCRAM so this is a no-op, but
     * kept for safety if buffer placement changes. */
    L1C_DCACHE_INVALID_RANGE(s_dma_buf[buf_idx], FRAME_BYTES);

    int16_t *src = s_dma_buf[buf_idx];

    /* --- Speaker-ID monitor: accumulate PCM for periodic identification --- */
    extern void spk_ai_feed_frame_isr(const int16_t *pcm, uint32_t bytes,
                                      BaseType_t *woken);
    spk_ai_feed_frame_isr(src, FRAME_BYTES, &woken);

    /* --- PDM -> DAC loopback: hand the frame to the DAC test path --- */
    extern void dac_test_feed_frame_isr(const int16_t *pcm, uint32_t bytes,
                                        BaseType_t *woken);
    dac_test_feed_frame_isr(src, FRAME_BYTES, &woken);

    /* --- USB Audio Class mic: hand the frame to the UAC streamer --- */
    extern void uac_mic_feed_frame_isr(const int16_t *pcm, uint32_t bytes,
                                       BaseType_t *woken);
    uac_mic_feed_frame_isr(src, FRAME_BYTES, &woken);

    /* --- Streaming mode: copy frame to ring buffer, queue pointer --- */
    if (s_streaming && s_stream_queue && s_stream_ring) {
        uint32_t idx = s_stream_widx % STREAM_QUEUE_LEN;
        int16_t *dst = s_stream_ring + (idx * FRAME_SAMPLES);
        memcpy(dst, src, FRAME_BYTES);
        s_stream_widx++;
        /* Queue pointer to the copy (drop if full — counted for 破音驗證) */
        if (xQueueSendFromISR(s_stream_queue, &dst, &woken) != pdTRUE) {
            ble_audio_stats_producer_drop();
        }
    }

    /* --- Recording mode: copy frame to PSRAM buffer --- */
    if (s_recording) {
        /* Discard first N frames — DMIC needs startup time */
        if (s_discard_count > 0) {
            s_discard_count--;
            portYIELD_FROM_ISR(woken);
            return;
        }

        uint32_t remaining = s_target_bytes - s_write_pos;

        if (remaining == 0) {
            s_recording = false;
            xSemaphoreGiveFromISR(s_rec_done_sem, &woken);
            portYIELD_FROM_ISR(woken);
            return;
        }

        uint32_t copy_bytes = (remaining < FRAME_BYTES) ? remaining : FRAME_BYTES;
        uint32_t off = s_write_pos % s_rec_cap;
        memcpy((uint8_t *)s_rec_buf + off, src, copy_bytes);
        s_write_pos += copy_bytes;
        s_frames_captured++;

        /* Live STT: signal every 30s of PCM (task sends CHUNK_READY over BLE) */
        while (s_write_pos >= s_next_chunk_bytes) {
            s_next_chunk_bytes += BLE_AUDIO_CHUNK_BYTES;
            if (s_chunk_ready_sem)
                xSemaphoreGiveFromISR(s_chunk_ready_sem, &woken);
        }

        /* Update stats (quick scan in ISR — 320 samples is fast) */
        uint16_t n = copy_bytes / sizeof(int16_t);
        for (uint16_t i = 0; i < n; i++) {
            int16_t val = src[i];
            int16_t abs_val = (val >= 0) ? val : (int16_t)(-val);
            if (abs_val > s_peak_sample)
                s_peak_sample = abs_val;
            s_energy_acc += (int32_t)val * val;
        }

        if (s_write_pos >= s_target_bytes) {
            s_recording = false;
            xSemaphoreGiveFromISR(s_rec_done_sem, &woken);
        }
    }

    portYIELD_FROM_ISR(woken);
}

/* ===================================================================
 * PDM hardware init (called once at startup)
 * =================================================================== */
static int pdm_hw_init(void)
{
    /* DMA double buffers point into static OCRAM array */
    s_dma_buf[0] = &s_dma_raw[0];
    s_dma_buf[1] = &s_dma_raw[FRAME_SAMPLES];

    /* Allocate PSRAM recording ring (240 s). Speaker-ID + model also sit
     * in PSRAM, so fall back to a 30 s buffer if the large ring will not fit. */
    if (!s_rec_buf) {
        s_rec_cap = MAX_RECORD_BYTES;
        s_rec_buf = pvPortMalloc(s_rec_cap);
        if (!s_rec_buf) {
            s_rec_cap = SAMPLE_RATE * sizeof(int16_t) * 30;
            s_rec_buf = pvPortMalloc(s_rec_cap);
        }
        if (!s_rec_buf) {
            IPRO_LOGE(TAG, "PSRAM rec buf alloc failed");
            return -1;
        }
        IPRO_LOGI(TAG, "Recording buffer: %p (%u KB, %lu s ring)",
                  s_rec_buf, (unsigned)(s_rec_cap / 1024),
                  (unsigned long)(s_rec_cap / (SAMPLE_RATE * sizeof(int16_t))));
    }

    IPRO_LOGI(TAG, "DMA buffers in OCRAM: %p, %p", s_dma_buf[0], s_dma_buf[1]);

    /* Configure PDM once — reinit breaks DMA on subsequent calls */
    hal_auadc_pdm_cfg_t cfg = {
        .pdm_clk_pin    = PDM_CLK_PIN,
        .pdm_in_pin     = PDM_DATA_PIN,
        .pdm_ch         = s_pdm_channel,
        .pcm_frame_size = FRAME_BYTES,
        .pcm_frame_buf  = {s_dma_buf[0], s_dma_buf[1]},
        .pcm_frame_event = pdm_frame_callback,
    };
    hal_auadc_pdm_init(&cfg);
    hal_auadc_set_digital_gain(s_pdm_gain);

    IPRO_LOGI(TAG, "PDM configured: %d Hz, ch=%s, gain=%d dB",
              SAMPLE_RATE,
              s_pdm_channel == HAL_AUADC_PDM_CH_LEFT ? "LEFT" : "RIGHT",
              s_pdm_gain);

    return 0;
}

/* ===================================================================
 * Recording monitor task — handles cleanup for both shell and BLE
 * =================================================================== */
static void pdm_chunk_task(void *arg)
{
    (void)arg;
    for (;;) {
        xSemaphoreTake(s_chunk_ready_sem, portMAX_DELAY);
        ble_audio_notify_chunk_ready();
    }
}

static void pdm_cache_clean_rec_buf(void)
{
    uint32_t n;

    if (!s_rec_buf)
        return;
    n = (s_write_pos < s_rec_cap) ? s_write_pos : s_rec_cap;
    if (n)
        L1C_DCACHE_CLEAN_RANGE(s_rec_buf, n);
}

static void pdm_monitor_task(void *arg)
{
    (void)arg;
    for (;;) {
        /* Wait for ISR to signal recording complete */
        xSemaphoreTake(s_rec_done_sem, portMAX_DELAY);
        hal_auadc_stop();
        pdm_cache_clean_rec_buf();
        IPRO_LOGI(TAG, "Recording complete: %lu bytes, %lu frames",
                  (unsigned long)s_write_pos, (unsigned long)s_frames_captured);
        if (s_write_pos > 0)
            ble_audio_notify_recording_done();
        else
            ble_audio_notify_status();
        /* Signal shell command (if waiting) */
        xSemaphoreGive(s_cleanup_done_sem);
    }
}

/* ===================================================================
 * Public PDM recording API (used by both shell and BLE)
 * =================================================================== */
void pdm_start_recording(int seconds)
{
    if (!s_audio_power_on) {
        IPRO_LOGW(TAG, "Recording ignored: audio power off");
        return;
    }
    if (s_recording)
        return;
    if (seconds < 1) seconds = 1;
    if (seconds > MAX_RECORD_SECONDS) seconds = MAX_RECORD_SECONDS;

    if (s_rec_buf == NULL) {
        IPRO_LOGE(TAG, "No recording buffer — pdm_hw_init failed?");
        return;
    }

    s_write_pos = 0;
    s_target_bytes = SAMPLE_RATE * sizeof(int16_t) * seconds;
    s_next_chunk_bytes = BLE_AUDIO_CHUNK_BYTES;
    ble_audio_reset_chunk_cursor();
    if (s_chunk_ready_sem)
        xSemaphoreTake(s_chunk_ready_sem, 0);
    s_frames_captured = 0;
    s_peak_sample = 0;
    s_energy_acc = 0;
    s_discard_count = STARTUP_DISCARD;
    /* Latch the gain this take was captured at. pdm_play/pdm_export must
     * report this, not the live setting — changing pdm_gain and replaying an
     * older buffer otherwise labels it with a gain it was never recorded at. */
    s_rec_gain = s_pdm_gain;
    memset(s_rec_buf, 0, s_rec_cap);

    /* Update gain (no full reinit — reinit breaks DMA) */
    hal_auadc_set_digital_gain(s_pdm_gain);
    hal_auadc_set_high_pass_filter(5, 0);

    IPRO_LOGI(TAG, "Recording %d sec (ch=%s, gain=%d dB)",
              seconds,
              s_pdm_channel == HAL_AUADC_PDM_CH_LEFT ? "LEFT" : "RIGHT",
              s_pdm_gain);

    s_recording = true;
    if (hal_auadc_start() != 0) {
        s_recording = false;
        IPRO_LOGE(TAG, "hal_auadc_start failed");
        return;
    }
    ble_audio_notify_status();
}

void pdm_stop_recording(void)
{
    if (!s_recording)
        return;
    hal_auadc_stop();
    s_recording = false;
    pdm_cache_clean_rec_buf();
    IPRO_LOGI(TAG, "Recording stopped: %lu bytes", (unsigned long)s_write_pos);
    if (s_write_pos > 0)
        ble_audio_notify_recording_done();
    else
        ble_audio_notify_status();
}

const int16_t *pdm_get_buffer(void)
{
    return s_rec_buf;
}

uint32_t pdm_get_recorded_bytes(void)
{
    return s_write_pos;
}

uint32_t pdm_get_buf_capacity(void)
{
    return s_rec_cap;
}

bool pdm_is_recording(void)
{
    return s_recording;
}

void pdm_set_gain(int gain_db)
{
    if (gain_db < PDM_GAIN_MIN_DB) {
        gain_db = PDM_GAIN_MIN_DB;
    }
    if (gain_db > PDM_GAIN_MAX_DB) {
        gain_db = PDM_GAIN_MAX_DB;
    }
    s_pdm_gain = gain_db;
    hal_auadc_set_digital_gain(s_pdm_gain);
    IPRO_LOGI(TAG, "Gain set to %d dB", s_pdm_gain);
}

int pdm_get_gain(void)
{
    return s_pdm_gain;
}

/* Gain in force when the current buffer was captured (see pdm_start_recording) */
int pdm_get_rec_gain(void)
{
    return s_rec_gain;
}

/* ===================================================================
 * Public PDM streaming API
 * =================================================================== */
void pdm_start_streaming(void)
{
    if (!s_audio_power_on) {
        IPRO_LOGW(TAG, "Streaming ignored: audio power off");
        return;
    }
    if (s_streaming || s_recording)
        return;

    /* Empty the queue of any stale pointers */
    if (s_stream_queue) {
        xQueueReset(s_stream_queue);
    }

    hal_auadc_set_digital_gain(s_pdm_gain);

    s_streaming = true;
    hal_auadc_start();
    IPRO_LOGI(TAG, "Streaming started (gain=%d dB)", s_pdm_gain);
}

void pdm_stop_streaming(void)
{
    if (!s_streaming)
        return;
    s_streaming = false;
    hal_auadc_stop();
    IPRO_LOGI(TAG, "Streaming stopped");
}

bool pdm_is_streaming(void)
{
    return s_streaming;
}

void pdm_audio_power_set(bool on)
{
    if (s_audio_power_on == on) {
        return;
    }

    if (!on) {
        pdm_stop_streaming();
        pdm_stop_recording();
    }

    s_audio_power_on = on;
    IPRO_LOGI(TAG, "Audio power %s", on ? "on" : "off");
}

void pdm_audio_power_toggle(void)
{
    pdm_audio_power_set(!s_audio_power_on);
}

bool pdm_audio_power_is_on(void)
{
    return s_audio_power_on;
}

/* ===================================================================
 * Shell commands
 * =================================================================== */
static int cmd_pdm_rec(int argc, char **argv)
{
    int seconds = 5;
    if (argc > 1) {
        seconds = atoi(argv[1]);
        if (seconds < 1) seconds = 1;
        if (seconds > MAX_RECORD_SECONDS) seconds = MAX_RECORD_SECONDS;
    }

    if (s_recording) {
        printf("Already recording!\r\n");
        return 0;
    }

    /* Drain any stale cleanup semaphore */
    xSemaphoreTake(s_cleanup_done_sem, 0);

    pdm_start_recording(seconds);

    printf("Recording %d seconds (ch=%s, gain=%d dB, %lu bytes)...\r\n",
           seconds,
           s_pdm_channel == HAL_AUADC_PDM_CH_LEFT ? "LEFT" : "RIGHT",
           s_pdm_gain,
           (unsigned long)s_target_bytes);

    /* Wait for monitor task to finish cleanup */
    if (xSemaphoreTake(s_cleanup_done_sem, pdMS_TO_TICKS(seconds * 1000 + 2000)) == pdTRUE) {
        printf("Recording done: %lu bytes, %lu frames\r\n",
               (unsigned long)s_write_pos, (unsigned long)s_frames_captured);
    } else {
        pdm_stop_recording();
        printf("Recording timeout! Got %lu of %lu bytes\r\n",
               (unsigned long)s_write_pos, (unsigned long)s_target_bytes);
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_rec, pdm_rec, Record PDM audio [seconds]);

static int cmd_pdm_dump(int argc, char **argv)
{
    uint32_t offset = 0;
    uint32_t count = 64;

    if (argc > 1) offset = (uint32_t)strtoul(argv[1], NULL, 0);
    if (argc > 2) count  = (uint32_t)strtoul(argv[2], NULL, 0);

    uint32_t ring_samples = s_rec_cap / sizeof(int16_t);
    uint32_t playable = (s_write_pos < s_rec_cap)
                        ? (s_write_pos / sizeof(int16_t)) : ring_samples;
    uint32_t origin = (s_write_pos > s_rec_cap)
                      ? ((s_write_pos % s_rec_cap) / sizeof(int16_t)) : 0;
    uint32_t total_samples = playable;

    if (offset >= total_samples) {
        printf("Offset %lu beyond recorded data (%lu samples in ring)\r\n",
               (unsigned long)offset, (unsigned long)total_samples);
        return 0;
    }
    if (offset + count > total_samples)
        count = total_samples - offset;

    printf("Samples [%lu..%lu] of %lu (logical %lu bytes):\r\n",
           (unsigned long)offset, (unsigned long)(offset + count - 1),
           (unsigned long)total_samples, (unsigned long)s_write_pos);

    for (uint32_t i = 0; i < count; i++) {
        printf("%6d ", s_rec_buf[(origin + offset + i) % ring_samples]);
        if ((i + 1) % 10 == 0)
            printf("\r\n");
    }
    if (count % 10 != 0)
        printf("\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_dump, pdm_dump, Dump PCM samples [offset] [count]);

static int cmd_pdm_stats(int argc, char **argv)
{
    (void)argc; (void)argv;

    uint32_t total_samples = s_write_pos / sizeof(int16_t);
    printf("=== PDM Recording Stats ===\r\n");
    printf("Recorded:     %lu bytes (%lu samples, %.2f sec logical)\r\n",
           (unsigned long)s_write_pos,
           (unsigned long)total_samples,
           (float)total_samples / SAMPLE_RATE);
    printf("Ring:         %lu s (%u KB)%s\r\n",
           (unsigned long)(s_rec_cap / (SAMPLE_RATE * sizeof(int16_t))),
           (unsigned)(s_rec_cap / 1024),
           s_write_pos > s_rec_cap ? " WRAP" : "");
    printf("Frames:       %lu\r\n", (unsigned long)s_frames_captured);
    printf("Peak sample:  %d (of 32767)\r\n", s_peak_sample);

    if (total_samples > 0) {
        double rms = (double)s_energy_acc / total_samples;
        float frms = 0.0f;
        if (rms > 0) {
            float x = (float)rms;
            for (int i = 0; i < 10; i++)
                x = (x + (float)rms / x) * 0.5f;
            frms = x;
        }
        printf("RMS level:    %.1f (of 32767)\r\n", frms);
        printf("Peak dBFS:    %.1f dB\r\n",
               s_peak_sample > 0 ? 20.0f * log10f((float)s_peak_sample / 32767.0f) : -96.0f);
    }
    printf("Buffer:       %p, max %u KB\r\n",
           s_rec_buf, (unsigned)(s_rec_cap / 1024));

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_stats, pdm_stats, Show recording statistics);

static int cmd_pds(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "probe") == 0) {
        extern int pdm_sleep_probe(void);
        return pdm_sleep_probe();
    }
    uint32_t sec = (argc > 1) ? (uint32_t)atoi(argv[1]) : 0u;

    printf("PDS31: OCRAM is retained, so the recording buffer and the BLE\r\n");
    printf("       state survive; the console is reset and restored on wake.\r\n");
    if (sec) {
        printf("       timed wake in %lu s, or press GPIO7.\r\n",
               (unsigned long)sec);
    } else {
        printf("       press GPIO7 to wake (vol keys do not wake).\r\n");
    }

    extern int pdm_sleep_enter(uint32_t seconds);
    return pdm_sleep_enter(sec);
}
SHELL_CMD_EXPORT_ALIAS(cmd_pds, pds, Sleep in PDS31 wake on GPIO7 [sec]);

/*
 * Button diagnostics.
 *
 * "the buttons do not trigger" has three quite different causes and the log
 * alone cannot tell them apart: the pad may not be sensing the level at all,
 * the level may be seen but no interrupt raised, or the interrupt may fire and
 * the handler reject it (there is a 120 ms debounce and an index lookup that
 * both return silently). This prints the raw level and the latched interrupt
 * status so the three can be separated.
 */
static int cmd_btn(int argc, char **argv)
{
    extern int GLB_Get_GPIO_IntStatus(int gpioPin);
    extern GLB_GPIO_INT_CONTROL_Type GLB_Get_GPIO_IntCtlMod(int gpioPin);
    const uint32_t pins[3] = { 7U, 5U, 6U };
    const char *names[3] = { "power", "vol_up", "vol_down" };

    if (argc > 1 && strcmp(argv[1], "watch") == 0) {
        printf("watching for 5 s - press the buttons now\r\n");
        for (int i = 0; i < 50; i++) {
            for (int b = 0; b < 3; b++) {
                if (gpio_read(pins[b]) == 0) {
                    printf("  GPIO%lu (%s) reads LOW\r\n",
                           (unsigned long)pins[b], names[b]);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        printf("done\r\n");
        return 0;
    }

    printf("\r\npin  name      level  int_status  int_ctrl\r\n");
    for (int b = 0; b < 3; b++) {
        printf("  %lu  %-9s %5d  %10d  %8d\r\n",
               (unsigned long)pins[b], names[b],
               gpio_read(pins[b]),
               GLB_Get_GPIO_IntStatus((int)pins[b]),
               (int)GLB_Get_GPIO_IntCtlMod((int)pins[b]));
    }
    printf("\r\nlevel 1 = released (20k pull-up), 0 = pressed to ground.\r\n");
    printf("`btn watch` polls for 5 s without using interrupts at all.\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_btn, btn, Button pin level and interrupt status [watch]);

static int cmd_pdm_gain(int argc, char **argv)
{
    if (argc < 2) {
        printf("Current gain: %d dB\r\n", s_pdm_gain);
        printf("Usage: pdm_gain <dB>  (range: -96 to 18)\r\n");
        return 0;
    }
    s_pdm_gain = atoi(argv[1]);
    printf("Gain set to %d dB (applied on next pdm_rec)\r\n", s_pdm_gain);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_gain, pdm_gain, Set PDM digital gain in dB);

static int cmd_pdm_ch(int argc, char **argv)
{
    if (argc < 2) {
        printf("Current channel: %s\r\n",
               s_pdm_channel == HAL_AUADC_PDM_CH_LEFT ? "LEFT" : "RIGHT");
        printf("Usage: pdm_ch left|right\r\n");
        return 0;
    }
    if (strcmp(argv[1], "right") == 0 || strcmp(argv[1], "r") == 0) {
        s_pdm_channel = HAL_AUADC_PDM_CH_RIGHT;
        printf("Channel set to RIGHT (applied on next pdm_rec)\r\n");
    } else {
        s_pdm_channel = HAL_AUADC_PDM_CH_LEFT;
        printf("Channel set to LEFT (applied on next pdm_rec)\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_ch, pdm_ch, Select PDM channel left|right);

static int cmd_pdm_stream(int argc, char **argv)
{
    if (argc < 2) {
        printf("Streaming: %s\r\n", s_streaming ? "ON" : "OFF");
        printf("Usage: pdm_stream on|off\r\n");
        return 0;
    }
    if (strcmp(argv[1], "on") == 0) {
        pdm_start_streaming();
        printf("Streaming started\r\n");
    } else {
        pdm_stop_streaming();
        printf("Streaming stopped\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_stream, pdm_stream, Start/stop audio streaming on|off);

static int cmd_pdm_lc3(int argc, char **argv)
{
    if (argc < 2) {
        printf("LC3 codec: %s\r\n", ble_audio_get_lc3() ? "ON" : "OFF");
        printf("Usage: pdm_lc3 on|off\r\n");
        return 0;
    }
    ble_audio_set_lc3(strcmp(argv[1], "on") == 0);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_lc3, pdm_lc3, Toggle LC3 codec for streaming on|off);

/* ===================================================================
 * BLE recorder callbacks
 * =================================================================== */
static ble_audio_recorder_t s_recorder = {
    .start_rec    = pdm_start_recording,
    .stop_rec     = pdm_stop_recording,
    .get_buf      = pdm_get_buffer,
    .get_bytes    = pdm_get_recorded_bytes,
    .get_capacity = pdm_get_buf_capacity,
    .is_recording = pdm_is_recording,
    .set_gain     = pdm_set_gain,
    .start_stream = pdm_start_streaming,
    .stop_stream  = pdm_stop_streaming,
    .is_streaming = pdm_is_streaming,
    .stream_queue = NULL, /* set after queue creation */
};

/* ===================================================================
 * Main task
 * =================================================================== */
static void main_task(void *arg)
{
    (void)arg;

    IPRO_LOGI(TAG, "PDM DMIC Recording Demo + BLE (4h / %ds PSRAM ring)",
              REC_BUF_SECONDS);
    /* Which image this is, in the first line that matters. The EVB and EVK
     * builds differ only in pins and a polarity, are the same size, and both
     * say "PDM configured" - an EVB image on an EVK records silence and mutes
     * the amplifier while playing, and the log gave no way to tell. */
    IPRO_LOGI(TAG, "board %s: PDM clk G%d / data G%d, speaker enable G%d active-%s, buttons %s",
              CONFIG_BOARD, PDM_CLK_PIN, PDM_DATA_PIN,
              CONFIG_IPRO_PDM_MIC_SPK_GPIO,
              CONFIG_IPRO_PDM_MIC_SPK_UNMUTE ? "HIGH" : "LOW",
              CONFIG_IPRO_PDM_MIC_BUTTONS ? "on (G7 power, G5/G6 vol)" : "off");

    if (pdm_hw_init() != 0) {
        IPRO_LOGE(TAG, "PDM init failed, halting");
        vTaskDelete(NULL);
        return;
    }

    /* Buttons are optional: the EVK has no switches on this bank, and on that
     * board GPIO5-7 are camera DVP lines. */
#if !defined(CONFIG_IPRO_PDM_MIC_BUTTONS) || CONFIG_IPRO_PDM_MIC_BUTTONS != 0
    if (pdm_buttons_init() != 0) {
        IPRO_LOGW(TAG, "button init failed");
    }
#endif

    /* Initialize BLE with recorder callbacks */
    int ret = ble_audio_init(&s_recorder);
    if (ret != 0) {
        IPRO_LOGW(TAG, "BLE init failed (%d) — shell-only mode", ret);
    }

    /* On-device speaker ID — loads mbv2.inm from the 'mfg' flash partition.
     * Non-fatal: if the model is missing, spk_* commands report not-ready. */
    extern int spk_ai_init(void);
    if (spk_ai_init() != 0) {
        IPRO_LOGW(TAG, "speaker-ID engine not ready (flash mbv2.inm to 'mfg' partition)");
    }

    /* PDM -> DAC test path (dac_tone / pdm_play / pdm_loop) */
    extern void dac_test_init(void);
    dac_test_init();

    /* Start the shell. The app previously only had the CONFIG_SHELL help text
     * and never called this, so the commands were compiled in but nothing
     * ever read the console. */
#if defined(CONFIG_SHELL)
    shell_init_with_task(UART0_INDEX);
#endif
#if defined(CONFIG_SHELL)
    IPRO_LOGI(TAG, "  pds [sec]            - sleep in PDS31, wake on GPIO7");
#endif
    IPRO_LOGI(TAG, "Ready. Shell %s",
#if defined(CONFIG_SHELL)
              "commands:"
#else
              "disabled; UART log only"
#endif
    );
#if defined(CONFIG_SHELL)
    IPRO_LOGI(TAG, "  pdm_rec [seconds]    - record (1-%d sec, ring %d s)",
              MAX_RECORD_SECONDS, REC_BUF_SECONDS);
    IPRO_LOGI(TAG, "  pdm_dump [off] [n]   - dump samples");
    IPRO_LOGI(TAG, "  pdm_stats            - show stats");
    IPRO_LOGI(TAG, "  pdm_gain <dB>        - set digital gain");
    IPRO_LOGI(TAG, "  pdm_ch left|right    - select PDM channel");
    IPRO_LOGI(TAG, "  pdm_stream on|off    - real-time BLE streaming");
    IPRO_LOGI(TAG, "  pdm_play [gain dB]   - replay recording via DAC");
    IPRO_LOGI(TAG, "  pdm_loop [sec] [dB]  - live PDM -> DAC loopback");
    IPRO_LOGI(TAG, "  dac_tone [f] [s] [%%] - reference tone via DAC");
    IPRO_LOGI(TAG, "  dac_gain [dB]        - software makeup gain");
    IPRO_LOGI(TAG, "  bat                  - GPIO22 battery ADC");
    IPRO_LOGI(TAG, "  epd_demo [clear|colors|text] - e-Paper");
#endif
#if !defined(CONFIG_IPRO_PDM_MIC_BUTTONS) || CONFIG_IPRO_PDM_MIC_BUTTONS != 0
    IPRO_LOGI(TAG, "Buttons: GPIO7 short=record 4h, long=sleep/wake; GPIO5/6 vol (no wake)");
#endif

    /* Keep task alive (BLE stack needs it) */
    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

int main(void)
{
    ipro_platform_init();
    cci_analysis_transport_enable();

    s_rec_done_sem = xSemaphoreCreateBinary();
    s_cleanup_done_sem = xSemaphoreCreateBinary();
    s_chunk_ready_sem = xSemaphoreCreateBinary();
    s_stream_queue = xQueueCreate(STREAM_QUEUE_LEN, sizeof(int16_t *));
    s_stream_ring = pvPortMalloc(STREAM_QUEUE_LEN * FRAME_BYTES);
    s_recorder.stream_queue = s_stream_queue;

    xTaskCreate(pdm_monitor_task, "pdm_mon", 512, NULL, 12, NULL);
    xTaskCreate(pdm_chunk_task, "pdm_chk", 512, NULL, 11, NULL);
    xTaskCreate(main_task, "pdm_main", 2048, NULL, 10, NULL);

    vTaskStartScheduler();

    /* Should never reach here */
    while (1) {}
}
