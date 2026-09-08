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
 * @file app_audio.c
 * @brief Audio Processing Module Implementation
 * 
 * LC3 decoding and PCM output handling.
 */

#include "app_audio.h"
#ifdef CONFIG_AUDIO_OUTPUT_UAC
#include "app_uac.h"
#else
#include "app_dac.h"
#endif
#include "app_pcm_monitor.h"
#include "lc3.h"
#include "ipro_log.h"

#include "audio_resampler.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define TAG "AUDIO"

// Math constants for sine wave generation
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
 * LC3 frame ring between the BT RX thread and the decode task.
 *
 * 32 frames is 320 ms at 10 ms - enough to ride out the decode task being
 * descheduled or blocked on a full UAC ring without pushing back on the
 * producer.  Lives on the heap (PSRAM here), not in .bss.
 */
#define AUDIO_RING_FRAMES   32u
#define AUDIO_MAX_OCTETS    160u   /* LC3 48_6 is 155, the PAC's cap */

typedef struct {
    uint8_t  data[AUDIO_MAX_OCTETS];
    uint16_t len;      /* 0 = lost SDU, decode with PLC */
    uint8_t  status;
} audio_frame_t;

static audio_frame_t    *s_ring;
static volatile uint32_t s_head;
static volatile uint32_t s_tail;
static SemaphoreHandle_t s_frame_sem;
static SemaphoreHandle_t s_cfg_lock;
static volatile bool     s_dec_run;
static TaskHandle_t      s_dec_task;

#define AUDIO_DEC_TASK_STACK  2048  /* words - lc3_mdct_inverse needs 2 KB alone */
#define AUDIO_DEC_TASK_PRIO   (configMAX_PRIORITIES - 4)

static void audio_decode_task(void *arg);

/*
 * Per-stage cost of the decode path.
 *
 * Average CPU load did not explain the failure: enabling the bass enhancement
 * put the DAC conversion chain at 13% of a 192 MHz core, which cannot starve
 * anything, yet ISO packets were being dropped at the HCI layer. An average
 * hides an occasional long frame, and one frame overrunning its 10 ms slot is
 * enough to make the host run out of ISO receive buffers. So this records the
 * maximum as well as the mean, per stage.
 */
typedef struct {
    uint64_t dec, res, out;      /* accumulated cycles per stage   */
    uint32_t dec_max, res_max, out_max, all_max;
    uint32_t frames;
    uint32_t ring_max;           /* deepest the input queue ever got */
} audio_prof_t;
static audio_prof_t s_prof;

static inline uint32_t aud_mcycle(void)
{
    uint32_t v;
    __asm__ volatile("csrr %0, mcycle" : "=r"(v));
    return v;
}

// Resampler instance (statically allocated)
static audio_resampler_t s_resampler __attribute__((aligned(4)));

/*
 * Private Data
 */
typedef struct {
    int16_t pcm_buffer[480];      // Decoded PCM (mono, max 48kHz * 10ms = 480)
    bool frame_ready;              // Frame decoded and ready
    uint32_t frames_decoded;       // Statistics
} audio_channel_t;

static struct {
    // Configuration
    app_audio_config_t config;
    
    // Single LC3 decoder (mono)
    lc3_decoder_t decoder;
    lc3_decoder_mem_48k_t decoder_mem;
    
    // PCM buffer (mono)
    int16_t pcm_buffer[480];      // Decoded PCM (mono, max 48kHz * 10ms = 480)
    
    // Resampled buffer (after rate conversion)
    int16_t resampled[640];   // Max: 480 * (32/24) = 640
    
    // Accumulation buffer for output (cache-aligned)
#ifdef CONFIG_AUDIO_OUTPUT_UAC
    #define ACCUM_MAX_FRAME_SAMPLES     640   // Mono: 640 samples max
    #define ACCUM_BUFFER_SIZE           (ACCUM_MAX_FRAME_SAMPLES * 4)
#else
    #define ACCUM_MAX_FRAME_SAMPLES     640   // Mono: 640 samples max
    #define ACCUM_BUFFER_SIZE           (DAC_SAMPLES_PER_SEGMENT * DAC_BUFFER_COUNT + ACCUM_MAX_FRAME_SAMPLES)
#endif
    int16_t accumulation_buffer[ACCUM_BUFFER_SIZE] __attribute__((aligned(32)));
    uint32_t accumulated_samples;
    uint32_t accumulation_threshold;
    bool dac_started;

    // Statistics
    app_audio_stats_t stats;

    // State
    bool initialized;
    uint32_t dac_sample_rate;

    // Rate tracking
    uint32_t samples_produced;
    uint32_t dac_start_time_ms;
    uint32_t last_rate_log_time_ms;
    uint32_t samples_at_last_log;
    uint32_t frames_at_last_log;

    // Test tone state
    bool test_tone_active;
    uint32_t test_tone_phase;
    int16_t test_tone_amplitude;
    uint32_t test_tone_duration;
    uint32_t test_tone_elapsed;  // Elapsed time in ms

    // Frame arrival timing tracking
    uint32_t last_frame_time_us;      // Timestamp of last frame arrival
    uint32_t frame_interval_sum_us;   // Sum for rolling average
    uint32_t frame_interval_count;    // Count for rolling average
} s_audio = {0};

/*
 * PCM Output - Accumulate and feed to DAC triple buffer
 *
 * Strategy: Accumulate LC3 frames until we have enough for one buffer segment,
 * then feed to DAC. DAC handles the triple-buffering internally.
 */
static void output_pcm(const int16_t* samples, uint32_t count)
{
    // Note: This function now receives STEREO interleaved samples [L,R,L,R,...]
    // The 'count' parameter is the TOTAL number of samples (both channels)
    
    const int16_t* output_samples = samples;
    uint32_t output_count = count;

    // No resampling here - already done in app_audio_process_frame()
    
    // Display PCM waveform (if enabled)
    #ifdef CONFIG_PCM_MONITOR_ENABLE
    app_pcm_monitor_display(output_samples, output_count);
    app_pcm_monitor_print_stats(output_samples, output_count);
    #endif

    /* Tap for NN training data: this is the signal that actually reaches the
     * sink - post LC3 decode, post resample - which is the distribution the
     * model has to work on.  No-op unless a capture is armed. */
    app_pcm_capture_feed(output_samples, output_count);

    // Accumulate samples in buffer with bounds checking
    // Buffer overflow protection: truncate if exceeding buffer size
    if (s_audio.accumulated_samples + output_count > ACCUM_BUFFER_SIZE) {
        uint32_t available = ACCUM_BUFFER_SIZE - s_audio.accumulated_samples;
        IPRO_LOGE(TAG, "Buffer overflow! acc=%lu + out=%lu > %d, truncating to %lu",
                  (unsigned long)s_audio.accumulated_samples,
                  (unsigned long)output_count,
                  ACCUM_BUFFER_SIZE,
                  (unsigned long)available);
        output_count = available;
        s_audio.stats.decode_errors++;  // Track as error
    }

    if (output_count > 0) {
        memcpy(&s_audio.accumulation_buffer[s_audio.accumulated_samples],
               output_samples, output_count * sizeof(int16_t));
        s_audio.accumulated_samples += output_count;
    }

    // Determine threshold based on output mode and state
#ifdef CONFIG_AUDIO_OUTPUT_UAC
    // UAC mode: feed every frame directly, ring buffer handles prebuffering
    uint32_t threshold = s_audio.accumulation_threshold;
#else
    // DAC mode: prebuffer multiple segments before starting
    uint32_t threshold;
    if (!s_audio.dac_started) {
        threshold = s_audio.accumulation_threshold * DAC_PREBUFFER_COUNT;
    } else {
        threshold = s_audio.accumulation_threshold;
    }
#endif

    // Feed to output when threshold reached
    while (s_audio.accumulated_samples >= threshold) {
#ifdef CONFIG_AUDIO_OUTPUT_UAC
        // UAC mode: feed one frame at a time
        uint32_t feed_count = s_audio.accumulation_threshold;

        int ret = app_uac_write(s_audio.accumulation_buffer, feed_count, true);
        if (ret != 0) {
            IPRO_LOGW(TAG, "UAC feed failed: %d", ret);
        }

        if (!s_audio.dac_started) {
            s_audio.dac_started = true;
            s_audio.dac_start_time_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
            s_audio.samples_produced = 0;
            s_audio.last_rate_log_time_ms = s_audio.dac_start_time_ms;
            s_audio.samples_at_last_log = 0;
            s_audio.frames_at_last_log = s_audio.stats.frames_decoded;
            IPRO_LOGI(TAG, "UAC started, feeding %lu samples per frame",
                      (unsigned long)feed_count);
        }
#else
        // DAC mode: existing prebuffer + segment logic
        uint32_t feed_count;
        if (!s_audio.dac_started) {
            feed_count = s_audio.accumulation_threshold * DAC_PREBUFFER_COUNT;
        } else {
            feed_count = s_audio.accumulation_threshold;
        }

        int ret = app_dac_write(s_audio.accumulation_buffer, feed_count, true);
        if (ret != 0) {
            IPRO_LOGW(TAG, "DAC feed failed: %d", ret);
        }

        if (!s_audio.dac_started) {
            s_audio.dac_started = true;
            s_audio.dac_start_time_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
            s_audio.samples_produced = 0;
            s_audio.last_rate_log_time_ms = s_audio.dac_start_time_ms;
            s_audio.samples_at_last_log = 0;
            s_audio.frames_at_last_log = s_audio.stats.frames_decoded;
            IPRO_LOGI(TAG, "DAC started with %lu samples prebuffer (%d/%d buffers)",
                      (unsigned long)feed_count, DAC_PREBUFFER_COUNT, DAC_BUFFER_COUNT);
        }
#endif

        // Move remaining samples to beginning of buffer
        uint32_t remaining = s_audio.accumulated_samples - feed_count;
        if (remaining > 0) {
            memmove(s_audio.accumulation_buffer,
                    &s_audio.accumulation_buffer[feed_count],
                    remaining * sizeof(int16_t));
        }
        s_audio.accumulated_samples = remaining;
    }

    // Track samples produced for rate analysis
    s_audio.samples_produced += output_count;
}

/*
 * LC3 Decode Worker Task
 * 
 * Offloads LC3 decoding from BLE ISR to FreeRTOS task context.
 * This prevents BLE ISR from blocking on lengthy decode operations.
 */
void app_audio_init(void)
{
    IPRO_LOGI(TAG, "Initializing LC3 decoder (default: %d Hz)",
              APP_AUDIO_DEFAULT_SAMPLE_RATE);

    // Set default configuration
    s_audio.config.sample_rate = APP_AUDIO_DEFAULT_SAMPLE_RATE;
    s_audio.config.frame_duration_us = APP_AUDIO_DEFAULT_FRAME_DURATION;
    s_audio.config.octets_per_frame = APP_AUDIO_DEFAULT_OCTETS_PER_FRAME;
    s_audio.config.num_channels = 1;  // Mono output
    
    // Initialize accumulation buffer
    s_audio.accumulated_samples = 0;

    // Initialize LC3 decoder (mono)
    s_audio.decoder = lc3_setup_decoder(
        s_audio.config.frame_duration_us,
        s_audio.config.sample_rate,
        0,  // No resampling
        &s_audio.decoder_mem
    );

    if (!s_audio.decoder) {
        IPRO_LOGE(TAG, "Failed to initialize LC3 decoder");
        return;
    }

    IPRO_LOGI(TAG, "LC3 decoder initialized successfully (%lu Hz)",
              (unsigned long)s_audio.config.sample_rate);

    // Initialize PCM monitor (for debugging)
    app_pcm_monitor_init();
    #ifdef CONFIG_PCM_MONITOR_ENABLE
    app_pcm_monitor_set_enabled(true);
    #endif

#ifdef CONFIG_AUDIO_OUTPUT_UAC
    // UAC mode: output rate is fixed by the endpoint (see UAC_OUTPUT_RATE_HZ)
    s_audio.dac_sample_rate = UAC_OUTPUT_RATE_HZ;

    // Threshold = samples per LC3 frame at OUTPUT rate (after resampling)
    uint32_t lc3_samples_per_frame = s_audio.config.sample_rate
        * s_audio.config.frame_duration_us / 1000000;
    s_audio.accumulation_threshold = (lc3_samples_per_frame * s_audio.dac_sample_rate)
        / s_audio.config.sample_rate;

    // Initialize resampler
    {
        audio_resampler_config_t rcfg = {
            .input_rate = s_audio.config.sample_rate,
            .output_rate = s_audio.dac_sample_rate,
        };
        if (audio_resampler_init(&s_resampler, &rcfg) != 0) {
            IPRO_LOGW(TAG, "Resampler init failed");
        }
    }

    app_uac_config_t uac_cfg = {
        .sample_rate = UAC_OUTPUT_RATE_HZ,
        .channels = s_audio.config.num_channels,
    };

    IPRO_LOGI(TAG, "UAC mode: %lu Hz output (LC3 %lu Hz)",
              (unsigned long)s_audio.dac_sample_rate,
              (unsigned long)s_audio.config.sample_rate);

    int ret = app_uac_init(&uac_cfg);
    if (ret != 0) {
        IPRO_LOGE(TAG, "Failed to initialize UAC: %d", ret);
        s_audio.initialized = false;
        return;
    }
#else
    // DAC mode: map to supported hardware rates
    // DAC supports: 8/16/32/512 kHz only
    s_audio.accumulation_threshold = DAC_SAMPLES_PER_SEGMENT;

    uint32_t dac_rate = s_audio.config.sample_rate;
    if (s_audio.config.sample_rate == 24000) {
        dac_rate = 32000;  // Upsample 24kHz to 32kHz
        IPRO_LOGI(TAG, "LC3 rate 24kHz will be upsampled to DAC rate 32kHz");
    } else if (s_audio.config.sample_rate == 48000) {
        dac_rate = 32000;  // Downsample 48kHz to 32kHz (closest)
        IPRO_LOGI(TAG, "LC3 rate 48kHz will be downsampled to DAC rate 32kHz");
    }

    s_audio.dac_sample_rate = dac_rate;

    // Initialize resampler
    {
        audio_resampler_config_t rcfg = {
            .input_rate = s_audio.config.sample_rate,
            .output_rate = dac_rate,
        };
        if (audio_resampler_init(&s_resampler, &rcfg) != 0) {
            IPRO_LOGW(TAG, "Resampler init failed");
        }
    }

    app_dac_config_t dac_cfg = {
        .sample_rate = dac_rate,
        .channels = s_audio.config.num_channels,
        .volume = 100,   // [NOT IMPLEMENTED]
        .gain_2x = false // No 2x gain (prevents clipping)
    };

    int ret = app_dac_init(&dac_cfg);
    if (ret != 0) {
        IPRO_LOGE(TAG, "Failed to initialize DAC: %d", ret);
        s_audio.initialized = false;
        return;
    }
#endif /* CONFIG_AUDIO_OUTPUT_UAC */

    // Reset statistics
    memset(&s_audio.stats, 0, sizeof(s_audio.stats));

    /*
     * The frame ring and its consumer.  Created last so a failure here leaves
     * initialized = false and the producer drops frames instead of touching a
     * half-built ring.
     */
    s_ring      = pvPortMalloc(AUDIO_RING_FRAMES * sizeof(audio_frame_t));
    s_frame_sem = xSemaphoreCreateCounting(AUDIO_RING_FRAMES, 0);
    s_cfg_lock  = xSemaphoreCreateMutex();
    if (!s_ring || !s_frame_sem || !s_cfg_lock) {
        IPRO_LOGE(TAG, "frame ring allocation failed");
        return;
    }
    s_head = 0;
    s_tail = 0;

    s_dec_run = true;
    if (xTaskCreate(audio_decode_task, "lc3_dec", AUDIO_DEC_TASK_STACK, NULL,
                    AUDIO_DEC_TASK_PRIO, &s_dec_task) != pdPASS) {
        IPRO_LOGE(TAG, "decode task create failed");
        s_dec_run = false;
        return;
    }
    IPRO_LOGI(TAG, "decode task up: %u frame ring, %u word stack",
              (unsigned)AUDIO_RING_FRAMES, (unsigned)AUDIO_DEC_TASK_STACK);

    s_audio.initialized = true;
    
    IPRO_LOGI(TAG, "LC3: %lu Hz, %lu us, %u bytes, %lu kbps (DAC: %lu Hz)",
              (unsigned long)s_audio.config.sample_rate,
              (unsigned long)s_audio.config.frame_duration_us,
              s_audio.config.octets_per_frame,
              (unsigned long)(s_audio.config.octets_per_frame * 8 * 1000000 / s_audio.config.frame_duration_us / 1000),
              (unsigned long)s_audio.dac_sample_rate);
}

int app_audio_reconfigure(const app_audio_config_t* config)
{
    if (!config) return -1;

    IPRO_LOGI(TAG, "Reconfiguring: LC3 rate=%lu Hz, dur=%lu us, octets=%u",
              (unsigned long)config->sample_rate,
              (unsigned long)config->frame_duration_us,
              config->octets_per_frame);
    
    // Validate parameters
    if (!LC3_CHECK_SR_HZ(config->sample_rate)) {
        IPRO_LOGE(TAG, "Invalid sample rate %lu", (unsigned long)config->sample_rate);
        return -1;
    }
    
    if (!LC3_CHECK_DT_US(config->frame_duration_us)) {
        IPRO_LOGE(TAG, "Invalid frame duration %lu", (unsigned long)config->frame_duration_us);
        return -1;
    }
    
    /*
     * BAP calls this from the BT RX thread while the decode task may be inside
     * lc3_decode() on the old decoder.  Take the same lock the task holds, and
     * drop whatever is still queued for the previous configuration - those
     * frames are the wrong size for the decoder about to replace it.
     */
    if (s_cfg_lock) {
        xSemaphoreTake(s_cfg_lock, portMAX_DELAY);
    }
    s_tail = s_head;

    // Store new configuration
    memcpy(&s_audio.config, config, sizeof(app_audio_config_t));
    
    // Reinitialize single decoder (shared for L+R)
    s_audio.decoder = lc3_setup_decoder(
        s_audio.config.frame_duration_us,
        s_audio.config.sample_rate,
        0,
        &s_audio.decoder_mem
    );
    
    if (!s_audio.decoder) {
        IPRO_LOGE(TAG, "Failed to reinitialize decoder");
        s_audio.initialized = false;
        if (s_cfg_lock) {
            xSemaphoreGive(s_cfg_lock);
        }
        return -1;
    }
    
#ifdef CONFIG_AUDIO_OUTPUT_UAC
    // UAC fixed at 32kHz (EP1 TX FIFO = 64 bytes, max 32kHz mono 16-bit)
    s_audio.dac_sample_rate = UAC_OUTPUT_RATE_HZ;

    // Threshold = samples per LC3 frame at OUTPUT rate (after resampling)
    uint32_t lc3_samples_per_frame = s_audio.config.sample_rate
        * s_audio.config.frame_duration_us / 1000000;
    s_audio.accumulation_threshold = (lc3_samples_per_frame * s_audio.dac_sample_rate)
        / s_audio.config.sample_rate;
    s_audio.accumulated_samples = 0;

    // Reinitialize resampler
    {
        audio_resampler_config_t rcfg = {
            .input_rate = s_audio.config.sample_rate,
            .output_rate = s_audio.dac_sample_rate,
        };
        if (audio_resampler_init(&s_resampler, &rcfg) != 0) {
            IPRO_LOGW(TAG, "Resampler reinit failed");
        }
    }

    IPRO_LOGI(TAG, "Reconfigured: UAC=%lu Hz (LC3 %lu Hz), dur=%lu us, accum=%lu samples",
              (unsigned long)s_audio.dac_sample_rate,
              (unsigned long)config->sample_rate,
              (unsigned long)config->frame_duration_us,
              (unsigned long)s_audio.accumulation_threshold);
#else
    // Map LC3 rate to DAC rate (DAC supports 8/16/32/512 kHz only)
    uint32_t dac_rate = s_audio.config.sample_rate;
    if (s_audio.config.sample_rate == 24000) {
        dac_rate = 32000;  // Upsample 24kHz to 32kHz
        IPRO_LOGI(TAG, "Resampling: 24kHz LC3 → 32kHz DAC");
    } else if (s_audio.config.sample_rate == 48000) {
        dac_rate = 32000;  // Downsample 48kHz to 32kHz
        IPRO_LOGI(TAG, "Resampling: 48kHz LC3 → 32kHz DAC");
    }

    s_audio.dac_sample_rate = dac_rate;

    // Reinitialize resampler
    {
        audio_resampler_config_t rcfg = {
            .input_rate = s_audio.config.sample_rate,
            .output_rate = dac_rate,
        };
        if (audio_resampler_init(&s_resampler, &rcfg) != 0) {
            IPRO_LOGW(TAG, "Resampler reinit failed");
        }
    }

    // Accumulation threshold matches DAC buffer segment size
    s_audio.accumulation_threshold = DAC_SAMPLES_PER_SEGMENT;
    s_audio.accumulated_samples = 0;

    uint32_t duration_ms = (s_audio.accumulation_threshold * 1000) / dac_rate;

    // Reconfigure DAC
    app_dac_config_t dac_cfg = {
        .sample_rate = dac_rate,
        .channels = config->num_channels,
        .volume = 100,   // [NOT IMPLEMENTED]
        .gain_2x = false // No 2x gain (prevents clipping)
    };
    app_dac_reconfigure(&dac_cfg);

    IPRO_LOGI(TAG, "Reconfigured: LC3=%lu Hz DAC=%lu Hz dur=%lu us octets=%u accum=%lu samples (%lums)",
              (unsigned long)config->sample_rate,
              (unsigned long)dac_rate,
              (unsigned long)config->frame_duration_us,
              config->octets_per_frame,
              (unsigned long)s_audio.accumulation_threshold,
              (unsigned long)duration_ms);
#endif

    if (s_cfg_lock) {
        xSemaphoreGive(s_cfg_lock);
    }
    return 0;
}

/*
 * Decode one LC3 frame to PCM and push it at the sink.
 *
 * Runs on the decode task, never on the BT RX thread: lc3_mdct_inverse alone
 * takes 2 KB of stack, and app_uac_write() blocks when the UAC ring is full.
 * Doing either inline in the recv callback stalls HCI processing for the
 * whole host.
 */
static void audio_decode_one(const uint8_t* p_data, uint16_t length, uint8_t status)
{
    uint32_t t_start = aud_mcycle();
    // Decode LC3 frame (mono)
    int num_samples;
    if (status != 0 || p_data == NULL || length == 0) {
        s_audio.stats.frames_lost++;
        
        // PLC (Packet Loss Concealment)
        int ret = lc3_decode(s_audio.decoder, NULL, 0, 
                             LC3_PCM_FORMAT_S16, s_audio.pcm_buffer, 1);
        if (ret < 0) {
            s_audio.stats.decode_errors++;
            return;
        }
        num_samples = lc3_frame_samples(s_audio.config.frame_duration_us,
                                         s_audio.config.sample_rate);
    } else {
        // Normal decode
        int ret = lc3_decode(s_audio.decoder, p_data, length,
                             LC3_PCM_FORMAT_S16, s_audio.pcm_buffer, 1);
        
        if (ret < 0) {
            IPRO_LOGW(TAG, "Decode error: %d", ret);
            s_audio.stats.decode_errors++;
            return;
        }
        
        if (ret > 0) {  // PLC was used
            s_audio.stats.frames_lost++;
        }
        
        num_samples = lc3_frame_samples(s_audio.config.frame_duration_us,
                                         s_audio.config.sample_rate);
    }
    
    s_audio.stats.frames_decoded++;
    
    // Log first few frames
    static uint32_t debug_frame_count = 0;
    debug_frame_count++;
    if (debug_frame_count <= 5) {
        IPRO_LOGI(TAG, "FRAME#%lu: LC3 rate=%lu dur=%luus -> %d samples (DAC rate=%lu)",
                  (unsigned long)debug_frame_count,
                  (unsigned long)s_audio.config.sample_rate,
                  (unsigned long)s_audio.config.frame_duration_us,
                  num_samples,
                  (unsigned long)s_audio.dac_sample_rate);
    }

    if (debug_frame_count % 100 == 0) {
        IPRO_LOGD(TAG, "LC3 decoded frame #%lu",
                  (unsigned long)debug_frame_count);
    }
    
    uint32_t t_dec = aud_mcycle();

    // Resample if needed
    int16_t *samples = s_audio.pcm_buffer;
    uint32_t sample_count = num_samples;
    
    if (s_audio.dac_sample_rate != s_audio.config.sample_rate) {
        sample_count = audio_resampler_process(&s_resampler,
                                                s_audio.pcm_buffer, num_samples,
                                                s_audio.resampled, 640);
        samples = s_audio.resampled;
    }

    uint32_t t_res = aud_mcycle();

    // Output PCM
    output_pcm(samples, sample_count);

    uint32_t t_end = aud_mcycle();
    uint32_t d = t_dec - t_start, r = t_res - t_dec, o = t_end - t_res;
    s_prof.dec += d; s_prof.res += r; s_prof.out += o; s_prof.frames++;
    if (d > s_prof.dec_max) s_prof.dec_max = d;
    if (r > s_prof.res_max) s_prof.res_max = r;
    if (o > s_prof.out_max) s_prof.out_max = o;
    if (t_end - t_start > s_prof.all_max) s_prof.all_max = t_end - t_start;
}

void app_audio_profile_reset(void)
{
    memset(&s_prof, 0, sizeof(s_prof));
}

void app_audio_profile_report(void)
{
    extern uint32_t SystemCoreClockGet(void);
    uint32_t clk = SystemCoreClockGet();
    if (s_prof.frames == 0u || clk == 0u) {
        printf("\naudio profile: no frames yet\n");
        return;
    }
    uint32_t us_per_frame = clk / 100000u;      /* cycles per 10 us */
    printf("\n=== decode path, %lu frames ===\n", (unsigned long)s_prof.frames);
    printf("             mean cyc   max cyc   max us\n");
    printf("  lc3 decode %9lu %9lu %8lu\n",
           (unsigned long)(s_prof.dec / s_prof.frames),
           (unsigned long)s_prof.dec_max, (unsigned long)(s_prof.dec_max * 10u / us_per_frame));
    printf("  resample   %9lu %9lu %8lu\n",
           (unsigned long)(s_prof.res / s_prof.frames),
           (unsigned long)s_prof.res_max, (unsigned long)(s_prof.res_max * 10u / us_per_frame));
    printf("  dac chain  %9lu %9lu %8lu\n",
           (unsigned long)(s_prof.out / s_prof.frames),
           (unsigned long)s_prof.out_max, (unsigned long)(s_prof.out_max * 10u / us_per_frame));
    uint32_t mean = (uint32_t)((s_prof.dec + s_prof.res + s_prof.out) / s_prof.frames);
    printf("  TOTAL      %9lu %9lu %8lu   (budget 10000 us)\n",
           (unsigned long)mean, (unsigned long)s_prof.all_max,
           (unsigned long)(s_prof.all_max * 10u / us_per_frame));
    printf("  mean load %lu.%02lu%%   queue high-water %lu/%d frames\n",
           (unsigned long)((uint64_t)mean * 100u * 100u / (clk / 100u)),
           (unsigned long)(((uint64_t)mean * 10000u * 100u / (clk / 100u)) % 100u),
           (unsigned long)s_prof.ring_max, AUDIO_RING_FRAMES);
}

/* Decode task - the only consumer of the frame ring. */
static void audio_decode_task(void *arg)
{
    (void)arg;

    while (s_dec_run) {
        if (xSemaphoreTake(s_frame_sem, pdMS_TO_TICKS(200)) != pdTRUE) {
            continue;
        }

        while (s_dec_run && s_tail != s_head) {
            uint32_t depth = s_head - s_tail;
            if (depth > s_prof.ring_max) s_prof.ring_max = depth;
            audio_frame_t *f = &s_ring[s_tail % AUDIO_RING_FRAMES];

            /* Held across the decode so a BAP reconfigure cannot swap the
             * decoder out from under lc3_decode(). */
            xSemaphoreTake(s_cfg_lock, portMAX_DELAY);
            audio_decode_one(f->len ? f->data : NULL, f->len, f->status);
            xSemaphoreGive(s_cfg_lock);

            s_tail++;
        }
    }

    s_dec_task = NULL;
    vTaskDelete(NULL);
}

/*
 * Producer - runs on the BT RX thread.  Copies the encoded frame and returns;
 * everything expensive happens on the decode task.
 */
void app_audio_process_frame(const uint8_t* p_data, uint16_t length,
                              uint32_t timestamp, uint8_t status, uint8_t stream_id)
{
    if (!s_audio.initialized) return;

    s_audio.stats.frames_received++;
    
    // Frame arrival timing tracking
    uint32_t now_us = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS * 1000);
    uint32_t expected_interval_us = s_audio.config.frame_duration_us;

    if (s_audio.last_frame_time_us != 0) {
        uint32_t interval_us = now_us - s_audio.last_frame_time_us;

        // Update min/max
        if (s_audio.stats.frame_interval_min_us == 0 ||
            interval_us < s_audio.stats.frame_interval_min_us) {
            s_audio.stats.frame_interval_min_us = interval_us;
        }
        if (interval_us > s_audio.stats.frame_interval_max_us) {
            s_audio.stats.frame_interval_max_us = interval_us;
        }

        // Rolling average (simple exponential moving average)
        if (s_audio.stats.frame_interval_avg_us == 0) {
            s_audio.stats.frame_interval_avg_us = interval_us;
        } else {
            // EMA: avg = avg * 0.9 + new * 0.1
            s_audio.stats.frame_interval_avg_us =
                (s_audio.stats.frame_interval_avg_us * 9 + interval_us) / 10;
        }

        // Track timing drift
        int32_t delay_delta = (int32_t)interval_us - (int32_t)expected_interval_us;
        s_audio.stats.current_delay_us += delay_delta;
        if (s_audio.stats.current_delay_us > s_audio.stats.max_delay_us) {
            s_audio.stats.max_delay_us = s_audio.stats.current_delay_us;
        }
        if (s_audio.stats.current_delay_us < s_audio.stats.min_delay_us) {
            s_audio.stats.min_delay_us = s_audio.stats.current_delay_us;
        }

        // Detect late frames (> 1.5x expected)
        if (interval_us > expected_interval_us * 3 / 2) {
            s_audio.stats.late_frames++;
            // Only warn on severe delays (>100ms), not expected 64ms bursts
            if (interval_us > 100000) {
                IPRO_LOGW(TAG, "SEVERE LATE frame: gap=%lu ms",
                          (unsigned long)(interval_us / 1000));
            }
        }

        // Detect burst frames
        if (interval_us < expected_interval_us / 2) {
            s_audio.stats.burst_frames++;
        }
    }
    s_audio.last_frame_time_us = now_us;
    
    /* Enqueue for the decode task.  Dropping the newest frame is the right
     * failure here: the alternative is blocking the host's RX thread, and a
     * dropped frame costs one 10 ms concealment while a stalled RX thread
     * costs the whole link. */
    if (!s_ring || (uint32_t)(s_head - s_tail) >= AUDIO_RING_FRAMES) {
        s_audio.stats.frames_lost++;
        return;
    }

    {
        audio_frame_t *f = &s_ring[s_head % AUDIO_RING_FRAMES];
        uint16_t n = (length > AUDIO_MAX_OCTETS) ? AUDIO_MAX_OCTETS : length;

        if (status != 0 || p_data == NULL || n == 0) {
            f->len = 0;                 /* decode_one() runs PLC for this */
        } else {
            memcpy(f->data, p_data, n);
            f->len = n;
        }
        f->status = status;
    }

    s_head++;
    xSemaphoreGive(s_frame_sem);

    (void)timestamp;
    (void)stream_id;
}

void app_audio_deinit(void)
{
    if (!s_audio.initialized) return;
    
    IPRO_LOGI(TAG, "Deinitializing audio");
    
    // Deinitialize audio output
#ifdef CONFIG_AUDIO_OUTPUT_UAC
    app_uac_deinit();
#else
    app_dac_deinit();
#endif
    
    s_audio.decoder = NULL;
    s_audio.initialized = false;

    /* Stop the consumer before releasing what it reads.  It waits at most
     * 200 ms on s_frame_sem, and the give below cuts that short. */
    s_dec_run = false;
    if (s_frame_sem) {
        xSemaphoreGive(s_frame_sem);
    }
    while (s_dec_task) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    if (s_ring) {
        vPortFree(s_ring);
        s_ring = NULL;
    }
    if (s_frame_sem) {
        vSemaphoreDelete(s_frame_sem);
        s_frame_sem = NULL;
    }
    if (s_cfg_lock) {
        vSemaphoreDelete(s_cfg_lock);
        s_cfg_lock = NULL;
    }
}

void app_audio_stop(void)
{
    IPRO_LOGI(TAG, "Stopped. Total: rx=%lu, dec=%lu, lost=%lu",
              (unsigned long)s_audio.stats.frames_received,
              (unsigned long)s_audio.stats.frames_decoded,
              (unsigned long)s_audio.stats.frames_lost);

    // Log frame timing statistics
    if (s_audio.stats.frames_received > 1) {
        IPRO_LOGI(TAG, "Frame timing: min=%lu us, max=%lu us, avg=%lu us",
                  (unsigned long)s_audio.stats.frame_interval_min_us,
                  (unsigned long)s_audio.stats.frame_interval_max_us,
                  (unsigned long)s_audio.stats.frame_interval_avg_us);
        IPRO_LOGI(TAG, "Frame issues: late=%lu, burst=%lu",
                  (unsigned long)s_audio.stats.late_frames,
                  (unsigned long)s_audio.stats.burst_frames);
    }

    // Stop audio output
#ifdef CONFIG_AUDIO_OUTPUT_UAC
    app_uac_stop();
#else
    app_dac_stop();
#endif

    // Reset accumulation state
    s_audio.accumulated_samples = 0;
    s_audio.dac_started = false;

    // Reset frame timing tracking
    s_audio.last_frame_time_us = 0;

    // Reset rate tracking
    s_audio.samples_produced = 0;
    s_audio.dac_start_time_ms = 0;
    s_audio.last_rate_log_time_ms = 0;
    s_audio.samples_at_last_log = 0;
    s_audio.frames_at_last_log = 0;

    // Reset statistics for next session
    memset(&s_audio.stats, 0, sizeof(s_audio.stats));
}

void app_audio_get_stats(app_audio_stats_t* stats)
{
    if (stats) {
        memcpy(stats, &s_audio.stats, sizeof(app_audio_stats_t));
    }
}

const app_audio_config_t* app_audio_get_config(void)
{
    return &s_audio.config;
}

/*
 * Test Tone Generation (1kHz Sine Wave)
 * NOTE: Test tone is only available in DAC mode
 */
#ifndef CONFIG_AUDIO_OUTPUT_UAC
static void generate_sine_wave(int16_t* buffer, uint32_t num_samples, 
                               uint32_t sample_rate, uint32_t frequency, 
                               int16_t amplitude, uint32_t* phase)
{
    // Phase increment per sample (fixed-point: 32-bit for full circle)
    uint32_t phase_increment = (uint32_t)((double)frequency * 4294967296.0 / (double)sample_rate);
    
    for (uint32_t i = 0; i < num_samples; i++) {
        // Generate sine wave using phase accumulator
        double angle = ((double)*phase / 4294967296.0) * 2.0 * M_PI;
        buffer[i] = (int16_t)(sin(angle) * amplitude);
        *phase += phase_increment;
    }
}

static void test_tone_task(void* arg)
{
    const uint32_t test_frequency = TEST_TONE_FREQUENCY_HZ;  // From app_audio.h

    IPRO_LOGI(TAG, "Test tone started: %d Hz, amplitude=%d, duration=%lu ms",
              test_frequency, s_audio.test_tone_amplitude,
              (unsigned long)s_audio.test_tone_duration);

    // Get ping-pong buffer size from DAC
    uint32_t buffer_size = app_dac_get_buffer_size();
    uint32_t duration_per_buffer_ms = (buffer_size * 1000) / s_audio.dac_sample_rate;

    // Allocate buffer for one ping-pong half
    int16_t *tone_buffer = (int16_t *)malloc(buffer_size * sizeof(int16_t));
    if (!tone_buffer) {
        IPRO_LOGE(TAG, "Failed to allocate %lu samples for test tone", (unsigned long)buffer_size);
        s_audio.test_tone_active = false;
        vTaskDelete(NULL);
        return;
    }

    IPRO_LOGI(TAG, "Triple buffer mode: %lu samples/segment (%.1f ms), %d buffers",
              (unsigned long)buffer_size, (float)duration_per_buffer_ms, DAC_BUFFER_COUNT);

    // Pre-fill all triple buffers before starting
    for (int i = 0; i < DAC_BUFFER_COUNT; i++) {
        generate_sine_wave(tone_buffer, buffer_size,
                          s_audio.dac_sample_rate, test_frequency,
                          s_audio.test_tone_amplitude, &s_audio.test_tone_phase);
        app_dac_feed(tone_buffer, buffer_size);
    }

    // Start DMA triple buffer playback
    int ret = app_dac_start();
    if (ret != 0) {
        IPRO_LOGE(TAG, "Failed to start DAC: %d", ret);
        free(tone_buffer);
        s_audio.test_tone_active = false;
        vTaskDelete(NULL);
        return;
    }

    uint32_t loop_count = 0;
    while (s_audio.test_tone_active) {
        // Wait for DMA callback (buffer consumed)
        if (!app_dac_wait_buffer_needed(1000)) {
            // Timeout - check if still active
            continue;
        }

        loop_count++;

        // Generate next buffer of samples (phase continues from previous)
        generate_sine_wave(tone_buffer, buffer_size,
                          s_audio.dac_sample_rate, test_frequency,
                          s_audio.test_tone_amplitude, &s_audio.test_tone_phase);

        // Feed to inactive buffer
        ret = app_dac_feed(tone_buffer, buffer_size);
        if (ret != 0) {
            IPRO_LOGW(TAG, "Test tone: DAC feed failed at loop %lu", (unsigned long)loop_count);
            break;
        }

        // Update elapsed time
        s_audio.test_tone_elapsed += duration_per_buffer_ms;

        if (loop_count % 10 == 0) {  // Log every 10 buffers
            IPRO_LOGI(TAG, "Test tone: loop=%lu, elapsed=%lu ms",
                      (unsigned long)loop_count, (unsigned long)s_audio.test_tone_elapsed);
        }

        // Check if duration reached (for non-continuous mode)
        if (s_audio.test_tone_duration > 0 &&
            s_audio.test_tone_elapsed >= s_audio.test_tone_duration) {
            break;
        }
    }

    // Stop DMA playback
    app_dac_stop();

    free(tone_buffer);
    s_audio.test_tone_active = false;
    IPRO_LOGI(TAG, "Test tone stopped (played %lu ms)",
              (unsigned long)s_audio.test_tone_elapsed);

    vTaskDelete(NULL);
}

int app_audio_play_test_tone(uint32_t duration_ms, int16_t amplitude)
{
    if (!s_audio.initialized) {
        IPRO_LOGE(TAG, "Audio not initialized");
        return -1;
    }
    
    if (s_audio.test_tone_active) {
        IPRO_LOGW(TAG, "Test tone already playing, stopping first");
        app_audio_stop_test_tone();
        vTaskDelay(pdMS_TO_TICKS(100)); // Wait for previous task to finish
    }
    
    // Validate amplitude
    if (amplitude <= 0 || amplitude > 32767) {
        amplitude = TEST_TONE_DEFAULT_AMPLITUDE;  // From app_audio.h
    }
    
    // Initialize test tone state
    s_audio.test_tone_active = true;
    s_audio.test_tone_phase = 0;
    s_audio.test_tone_amplitude = amplitude;
    s_audio.test_tone_duration = duration_ms;
    s_audio.test_tone_elapsed = 0;
    
    // Create test tone task
    BaseType_t ret = xTaskCreate(test_tone_task, "test_tone", 2048, NULL, 10, NULL);
    if (ret != pdPASS) {
        IPRO_LOGE(TAG, "Failed to create test tone task");
        s_audio.test_tone_active = false;
        return -1;
    }
    
    return 0;
}

void app_audio_stop_test_tone(void)
{
    if (s_audio.test_tone_active) {
        IPRO_LOGI(TAG, "Stopping test tone...");
        s_audio.test_tone_active = false;
    }
}
#endif /* !CONFIG_AUDIO_OUTPUT_UAC - test tone is DAC-only */

/* ==========================================================================
 * Exponential sine sweep, for measuring the acoustic chain
 * ========================================================================== */

static volatile bool s_sweep_active;
static uint32_t s_sweep_sec, s_sweep_f1, s_sweep_f2;
static int16_t  s_sweep_amp;

/*
 * Farina exponential sweep, generated by accumulating instantaneous phase:
 *
 *     f(n)   = f1 * (f2/f1)^(n/(N-1))
 *     phase += 2*pi*f(n)/fs,  wrapped to [0, 2*pi)
 *     x(n)   = A * sin(phase)
 *
 * Accumulating rather than evaluating the closed-form phase keeps the
 * arithmetic small: the closed form reaches ~450000 radians for a 20 Hz ->
 * 16 kHz sweep over 30 s, which float32 resolves to only about 0.03 rad.
 * make_sweep_ref.py uses this same recurrence.
 */
#include "sweep_sine_table.h"

/* Gain in force before the sweep took over, restored when it ends. */
static float s_sweep_saved_gain = 1.0f;

/*
 * Interpolated table lookup. phase is a full-scale uint32 accumulator where
 * 2^32 == 2*pi, so it wraps for free and never drifts.
 */
static inline int32_t sweep_sin_q15(uint32_t phase)
{
    uint32_t idx  = phase >> (32u - SWEEP_TAB_BITS);
    int32_t  frac = (int32_t)((phase >> (32u - SWEEP_TAB_BITS - 8u)) & 0xFFu);
    int32_t  a    = s_sweep_sin[idx];
    int32_t  b    = s_sweep_sin[(idx + 1u) & (SWEEP_TAB_SIZE - 1u)];
    return a + (((b - a) * frac) >> 8);
}

static void sweep_task(void *arg)
{
    (void)arg;

    const uint32_t fs = s_audio.dac_sample_rate;
    const uint32_t total = s_sweep_sec * fs;

#ifdef CONFIG_AUDIO_OUTPUT_UAC
    const uint32_t chunk = 480;
#else
    const uint32_t chunk = app_dac_get_buffer_size();
#endif

    int16_t *buf = malloc(chunk * sizeof(int16_t));
    if (!buf) {
        IPRO_LOGE(TAG, "sweep: cannot allocate %lu samples", (unsigned long)chunk);
        s_sweep_active = false;
        vTaskDelete(NULL);
        return;
    }

    /*
     * Frequency schedule.
     *
     * f(n) = f1 * (f2/f1)^(n/(N-1)) is evaluated once per chunk only; within a
     * chunk the phase increment is interpolated linearly. Over one chunk the
     * frequency moves <1%, so the error against the true exponential is ~1e-5.
     *
     * This is deliberately NOT per-sample pow(): double maths is soft-float on
     * rv32imafc, and 32000 pow() calls per second could not keep the DAC ring
     * fed - it underran 2.4 s into a 5 s sweep. Soft-float uses integer
     * registers only, so calling it 31 times a second is both cheap and safe.
     */
    const double ratio = (double)s_sweep_f2 / (double)s_sweep_f1;
    const double inv_n = 1.0 / (double)(total > 1u ? total - 1u : 1u);
    const double scale = 4294967296.0 / (double)fs;   /* Hz -> phase step */
    uint32_t phase = 0u;

    IPRO_LOGI(TAG, "sweep: %lu Hz -> %lu Hz over %lu s @ %lu Hz, amp %d",
              (unsigned long)s_sweep_f1, (unsigned long)s_sweep_f2,
              (unsigned long)s_sweep_sec, (unsigned long)fs, s_sweep_amp);
    IPRO_LOGI(TAG, "sweep: recording now -> measured.wav; reference from "
                   "make_sweep_ref.py %lu %lu %lu %lu %d",
              (unsigned long)fs, (unsigned long)s_sweep_sec,
              (unsigned long)s_sweep_f1, (unsigned long)s_sweep_f2, s_sweep_amp);

#ifndef CONFIG_AUDIO_OUTPUT_UAC
    uint32_t prefill = 0;
    bool started = false;
#endif
    uint32_t n = 0;
    uint32_t dropped = 0;
    while (s_sweep_active && n < total) {
        uint32_t take = (total - n < chunk) ? (total - n) : chunk;

        uint32_t d0 = (uint32_t)((double)s_sweep_f1 *
                                 pow(ratio, (double)n * inv_n) * scale);
        uint32_t d1 = (uint32_t)((double)s_sweep_f1 *
                                 pow(ratio, (double)(n + take) * inv_n) * scale);

        for (uint32_t i = 0; i < take; i++) {
            uint32_t d = d0 + (uint32_t)(((int64_t)(d1 - d0) * (int64_t)i) /
                                         (int64_t)take);
            phase += d;
            buf[i] = (int16_t)((sweep_sin_q15(phase) * (int32_t)s_sweep_amp) >> 15);
        }
        for (uint32_t i = take; i < chunk; i++) buf[i] = 0;

#ifdef CONFIG_AUDIO_OUTPUT_UAC
        while (s_sweep_active && app_uac_write(buf, chunk, true) != 0) {
            vTaskDelay(1);
        }
#else
        /*
         * app_dac_feed() is non-blocking and returns -1 when every DMA buffer
         * is still busy. Waiting here is what keeps the sweep continuous; the
         * first version advanced n regardless and silently dropped the chunk.
         */
        uint32_t waits = 0;
        while (s_sweep_active && app_dac_feed(buf, chunk) != 0) {
            if (++waits > 200u) { dropped++; break; }
            vTaskDelay(1);
        }
        if (!started && ++prefill >= DAC_BUFFER_COUNT) {
            app_dac_start();
            started = true;
        }
#endif
        n += take;
    }

    vTaskDelay(pdMS_TO_TICKS(300));      /* let the tail drain */
#ifndef CONFIG_AUDIO_OUTPUT_UAC
    app_dac_stop();
#endif
    free(buf);
    s_sweep_active = false;
#ifndef CONFIG_AUDIO_OUTPUT_UAC
    app_dac_set_gain(s_sweep_saved_gain);   /* undo the forced unity gain */
#endif
    if (dropped) {
        IPRO_LOGE(TAG, "sweep: %lu chunks dropped - measurement is NOT valid",
                  (unsigned long)dropped);
    }
    IPRO_LOGI(TAG, "sweep: done (%lu samples, %lu dropped)",
              (unsigned long)n, (unsigned long)dropped);
    vTaskDelete(NULL);
}

int app_audio_play_sweep(uint32_t seconds, uint32_t f1, uint32_t f2, int16_t amplitude)
{
    if (s_sweep_active)                    return -1;
    if (seconds == 0u || seconds > 120u)   return -1;
    if (f1 == 0u || f2 <= f1)              return -1;
    if (f2 > s_audio.dac_sample_rate / 2u) {
        IPRO_LOGE(TAG, "sweep: %lu Hz is above Nyquist (%lu)",
                  (unsigned long)f2, (unsigned long)(s_audio.dac_sample_rate / 2u));
        return -1;
    }

    s_sweep_sec = seconds; s_sweep_f1 = f1; s_sweep_f2 = f2;
    s_sweep_amp = amplitude; s_sweep_active = true;

#ifndef CONFIG_AUDIO_OUTPUT_UAC
    /*
     * Pin the gain to unity for the duration. A connected phone drives this
     * through VCP and typically parks it around 0.07, which puts the sweep
     * ~23 dB down into the noise floor and makes the capture unusable.
     */
    s_sweep_saved_gain = app_dac_get_gain();
    app_dac_set_gain(1.0f);

    /*
     * The DAC path soft-limits 4:1 above DAC_KNEE. Anything above it is
     * compressed, and a compressor in the path measures itself rather than
     * the speaker, so refuse to produce an invalid measurement silently.
     */
    if (amplitude > DAC_KNEE) {
        IPRO_LOGW(TAG, "sweep: amp %d exceeds the %d soft knee - the response "
                       "will be compressed, not linear", amplitude, DAC_KNEE);
    }
#endif

    /* Own task. The generator itself is integer-only (table lookup), so it is
     * safe alongside a decoding stream; the per-chunk schedule uses double,
     * which is soft-float on rv32imafc and touches no f-registers. */
    if (xTaskCreate(sweep_task, "sweep", 2048, NULL, 6, NULL) != pdPASS) {
        s_sweep_active = false;
        return -2;
    }
    return 0;
}
