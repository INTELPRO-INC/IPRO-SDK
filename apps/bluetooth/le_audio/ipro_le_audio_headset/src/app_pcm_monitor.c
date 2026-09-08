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
 * @file app_pcm_monitor.c
 * @brief PCM Waveform Monitoring Implementation
 * 
 * Debug tool for visualizing PCM audio data.
 */

#include "app_pcm_monitor.h"
#include "ipro_log.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define TAG "PCM_MON"

// Configuration
#define ASCII_WAVEFORM_WIDTH    80      // Terminal width
#define ASCII_WAVEFORM_HEIGHT   20      // Number of rows
#define SAMPLE_DOWNSAMPLE       8       // Show every Nth sample

static struct {
    bool enabled;
    uint32_t frame_count;
} s_monitor = {
    .enabled = false,
    .frame_count = 0,
};

void app_pcm_monitor_init(void)
{
    s_monitor.enabled = false;
    s_monitor.frame_count = 0;
    IPRO_LOGI(TAG, "PCM Monitor initialized");
}

void app_pcm_monitor_set_enabled(bool enable)
{
    s_monitor.enabled = enable;
    IPRO_LOGI(TAG, "PCM Monitor %s", enable ? "enabled" : "disabled");
}

/**
 * @brief Display ASCII waveform
 */
void app_pcm_monitor_display(const int16_t* samples, uint32_t count)
{
    if (!s_monitor.enabled || !samples || count == 0) return;
    
    // Limit display rate (every 10th frame)
    if (++s_monitor.frame_count % 10 != 0) return;
    
    char waveform[ASCII_WAVEFORM_HEIGHT][ASCII_WAVEFORM_WIDTH + 1];
    memset(waveform, ' ', sizeof(waveform));
    
    // Null terminators
    for (int row = 0; row < ASCII_WAVEFORM_HEIGHT; row++) {
        waveform[row][ASCII_WAVEFORM_WIDTH] = '\0';
    }
    
    // Draw center line
    int center = ASCII_WAVEFORM_HEIGHT / 2;
    for (int col = 0; col < ASCII_WAVEFORM_WIDTH; col++) {
        waveform[center][col] = '-';
    }
    
    // Downsample and plot
    uint32_t step = (count * SAMPLE_DOWNSAMPLE) / ASCII_WAVEFORM_WIDTH;
    if (step == 0) step = 1;
    
    for (int col = 0; col < ASCII_WAVEFORM_WIDTH && col * step < count; col++) {
        int16_t sample = samples[col * step];
        
        // Normalize to waveform height (-32768 to 32767 -> 0 to HEIGHT-1)
        int row = center - (sample * center / 32768);
        
        // Clamp
        if (row < 0) row = 0;
        if (row >= ASCII_WAVEFORM_HEIGHT) row = ASCII_WAVEFORM_HEIGHT - 1;
        
        waveform[row][col] = '*';
    }
    
    // Print waveform
    printf("\n╔════════════════════════════════════════════════════════════════════════════════╗\n");
    for (int row = 0; row < ASCII_WAVEFORM_HEIGHT; row++) {
        printf("║%s║\n", waveform[row]);
    }
    printf("╚════════════════════════════════════════════════════════════════════════════════╝\n");
    printf("  Samples: %lu, Frame: %lu\n", (unsigned long)count, (unsigned long)s_monitor.frame_count);
}

/**
 * @brief Output raw PCM data (can be piped to external tools)
 */
void app_pcm_monitor_output_raw(const int16_t* samples, uint32_t count)
{
    if (!s_monitor.enabled || !samples) return;
    
    printf("PCM_DATA_START,%lu\n", (unsigned long)count);
    
    for (uint32_t i = 0; i < count; i++) {
        printf("%d", samples[i]);
        if (i < count - 1) printf(",");
        if ((i + 1) % 16 == 0) printf("\n");
    }
    
    printf("\nPCM_DATA_END\n");
}

/**
 * @brief Calculate and print PCM statistics
 */
void app_pcm_monitor_print_stats(const int16_t* samples, uint32_t count)
{
    if (!samples || count == 0) return;
    
    int16_t min = 32767;
    int16_t max = -32768;
    int64_t sum = 0;
    uint64_t sum_squares = 0;
    
    for (uint32_t i = 0; i < count; i++) {
        int16_t s = samples[i];
        
        if (s < min) min = s;
        if (s > max) max = s;
        
        sum += s;
        sum_squares += (int64_t)s * s;
    }
    
    float average = (float)sum / count;
    float rms = sqrtf((float)sum_squares / count);
    
    // Peak amplitude as percentage
    int16_t peak = (max > -min) ? max : -min;
    float peak_percent = (float)peak * 100.0f / 32768.0f;
    
    IPRO_LOGI(TAG, "PCM Stats: Min=%d, Max=%d, Avg=%.1f, RMS=%.1f, Peak=%.1f%%",
              min, max, average, rms, peak_percent);
}

/**
 * @brief Simple oscilloscope-style display (single line)
 */
void app_pcm_monitor_oscilloscope(const int16_t* samples, uint32_t count)
{
    if (!s_monitor.enabled || !samples || count == 0) return;
    
    // Show 40 samples across terminal
    char line[41];
    memset(line, ' ', 40);
    line[40] = '\0';
    line[20] = '|';  // Center marker
    
    uint32_t step = count / 40;
    if (step == 0) step = 1;
    
    for (int i = 0; i < 40 && i * step < count; i++) {
        int16_t sample = samples[i * step];
        
        // Map to -20 to +20 positions
        int pos = 20 + (sample * 19 / 32768);
        if (pos < 0) pos = 0;
        if (pos > 39) pos = 39;
        
        line[pos] = (sample >= 0) ? '+' : '-';
    }
    
    printf("[%s]\n", line);
}

/**
 * @brief Hexdump style output
 */
void app_pcm_monitor_hexdump(const int16_t* samples, uint32_t count, uint32_t max_samples)
{
    if (!samples) return;
    
    uint32_t limit = (count < max_samples) ? count : max_samples;
    
    printf("PCM Data (%lu samples, showing first %lu):\n", 
           (unsigned long)count, (unsigned long)limit);
    
    for (uint32_t i = 0; i < limit; i += 8) {
        printf("%04lX: ", (unsigned long)i);
        
        for (uint32_t j = 0; j < 8 && (i + j) < limit; j++) {
            printf("%04X ", (uint16_t)samples[i + j]);
        }
        
        printf("  ");
        
        for (uint32_t j = 0; j < 8 && (i + j) < limit; j++) {
            printf("%6d ", samples[i + j]);
        }
        
        printf("\n");
    }
}

/* ==========================================================================
 * Bulk capture for NN training data
 * ========================================================================== */

#include <FreeRTOS.h>
#include <task.h>

static int16_t  *s_cap;
static uint32_t  s_cap_cap;      /* capacity, samples */
static uint32_t  s_cap_len;      /* filled, samples   */
static uint32_t  s_cap_rate;
static volatile bool s_cap_armed;

int app_pcm_capture_start(uint32_t seconds, uint32_t sample_rate)
{
    s_cap_armed = false;

    if (s_cap) {
        vPortFree(s_cap);
        s_cap = NULL;
    }
    s_cap_cap = s_cap_len = 0;

    if (seconds == 0u) {
        printf("pcm capture: buffer released\n");
        return 0;
    }

    uint32_t n = seconds * sample_rate;
    s_cap = pvPortMalloc(n * sizeof(int16_t));
    if (!s_cap) {
        printf("pcm capture: cannot allocate %lu KB\n",
               (unsigned long)(n * sizeof(int16_t) / 1024u));
        return -1;
    }

    s_cap_cap  = n;
    s_cap_rate = sample_rate;
    s_cap_armed = true;

    printf("pcm capture: armed %lu s @ %lu Hz (%lu KB). Play audio, then `pcmcap dump`.\n",
           (unsigned long)seconds, (unsigned long)sample_rate,
           (unsigned long)(n * sizeof(int16_t) / 1024u));
    return 0;
}

void app_pcm_capture_feed(const int16_t* samples, uint32_t count)
{
    if (!s_cap_armed || !s_cap || !samples) {
        return;
    }

    uint32_t room = s_cap_cap - s_cap_len;
    uint32_t take = (count < room) ? count : room;

    if (take) {
        memcpy(&s_cap[s_cap_len], samples, take * sizeof(int16_t));
        s_cap_len += take;
    }
    if (s_cap_len >= s_cap_cap) {
        s_cap_armed = false;        /* stop at full; the dump is a separate step */
    }
}

void app_pcm_capture_status(uint32_t* len, uint32_t* cap, uint32_t* rate)
{
    if (len)  *len  = s_cap_len;
    if (cap)  *cap  = s_cap_cap;
    if (rate) *rate = s_cap_rate;
}

/*
 * Base64 rather than hex: 4 characters per 3 bytes instead of 2 per byte, so
 * a 10 s capture is 853 KB on the wire instead of 1.25 MB - 74 s versus 109 s
 * at 115200 baud.  The header carries rate and length so the receiver does not
 * have to be told, and END lets it detect a truncated transfer.
 */
/*
 * Milliseconds to allow per 77-byte output line.  Derived from the console
 * baud rate: 77 bytes * 10 bits / baud, rounded up, +1 for margin.
 */
#ifndef CONFIG_PLATFORM_UART_BAUDRATE
#define CONFIG_PLATFORM_UART_BAUDRATE 115200
#endif
#define PCMCAP_LINE_MS  ((77u * 10u * 1000u) / CONFIG_PLATFORM_UART_BAUDRATE + 1u)

static const char B64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void app_pcm_capture_dump(void)
{
    if (!s_cap || s_cap_len == 0u) {
        printf("pcm capture: nothing captured\n");
        return;
    }

    s_cap_armed = false;

    /*
     * Silence everything else for the duration.
     *
     * The dump is raw printf and takes over a minute; any IPRO_LOG from another
     * task lands in the middle of a base64 line and the receiver has to discard
     * that whole line.  The first run lost 8.6% of the capture exactly this way
     * - BAP_SERVER's periodic "ISO RX #N" was visible interleaved in the
     * transfer.  Errors still get through, so a real fault is not hidden.
     */
    ipro_log_level_t saved = ipro_log_get_level();
    ipro_log_set_level(IPRO_LOG_LEVEL_ERROR);

    const uint8_t *p = (const uint8_t *)s_cap;
    uint32_t bytes = s_cap_len * sizeof(int16_t);

    printf("\n---PCMCAP-BEGIN rate=%lu samples=%lu bits=16 ch=1 lines=%lu---\n",
           (unsigned long)s_cap_rate, (unsigned long)s_cap_len,
           (unsigned long)((bytes + 56u) / 57u));   /* 57 bytes -> 76 chars */

    char line[81];
    uint32_t col = 0;
    uint32_t line_idx = 0;
    for (uint32_t i = 0; i < bytes; i += 3u) {
        uint32_t v = (uint32_t)p[i] << 16;
        uint32_t n = bytes - i;
        if (n > 1u) v |= (uint32_t)p[i+1] << 8;
        if (n > 2u) v |= (uint32_t)p[i+2];

        line[col++] = B64[(v >> 18) & 0x3f];
        line[col++] = B64[(v >> 12) & 0x3f];
        line[col++] = (n > 1u) ? B64[(v >> 6) & 0x3f] : '=';
        line[col++] = (n > 2u) ? B64[v & 0x3f]        : '=';

        if (col >= 76u) {
            line[col] = 0;
            /*
             * Every line carries its own index.
             *
             * Other subsystems print straight to the console without going
             * through ipro_log - the controller's CIS diagnostics, the host's
             * ISO/ASCS traces - so raising the log threshold does not silence
             * them, and one landing mid-line costs that line.  Each line is a
             * whole 57 bytes (19 base64 groups), so it decodes independently;
             * with the index the receiver knows exactly which 57-byte block is
             * missing and can place the rest correctly instead of shifting
             * everything after the gap.
             */
            printf("%04lx:%s\n", (unsigned long)line_idx++, line);
            col = 0;
            /*
             * Pace to the wire, not to the tick.
             *
             * 77 bytes at 115200 baud take 6.7 ms.  The first version yielded
             * one tick (1 ms) per line, i.e. pushed 6.7x faster than the line
             * could carry, and the UART simply dropped the overflow - 8.3% of
             * every capture vanished as whole missing lines, which in the PCM
             * shows up as a few hundred discontinuities.  Sleep for the time
             * the line actually needs, plus a margin.
             */
            vTaskDelay(pdMS_TO_TICKS(PCMCAP_LINE_MS));
        }
    }
    if (col) { line[col] = 0; printf("%04lx:%s\n", (unsigned long)line_idx++, line); }

    printf("---PCMCAP-END---\n");
    ipro_log_set_level(saved);
}
