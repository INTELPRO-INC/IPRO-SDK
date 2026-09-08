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
 * @file app_pcm_monitor.h
 * @brief PCM Waveform Monitoring and Visualization
 * 
 * Provides PCM data visualization for debugging audio pipeline.
 */

#ifndef APP_PCM_MONITOR_H
#define APP_PCM_MONITOR_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize PCM monitor
 */
void app_pcm_monitor_init(void);

/**
 * @brief Enable/disable PCM visualization
 * @param enable true to enable, false to disable
 */
void app_pcm_monitor_set_enabled(bool enable);

/**
 * @brief Display PCM samples as ASCII waveform
 * @param samples PCM buffer (int16_t format)
 * @param count Number of samples
 */
void app_pcm_monitor_display(const int16_t* samples, uint32_t count);

/*
 * Bulk PCM capture, for gathering NN training data off the real signal chain.
 *
 * The point of capturing on the device rather than reusing the source files is
 * that this is the signal *after* LC3 decode and resampling - band-limited by
 * the codec and shaped by the anti-alias filter - so a model trained on it sees
 * the distribution it will actually meet, and the int8 input quantisation range
 * is derived from real data rather than from whatever the PC happened to hold.
 *
 * Capture runs into the PSRAM heap and is dumped afterwards, never during: at
 * 32 kHz the stream is 64 KB/s while the console carries about 11.5 KB/s, so
 * dumping inline would stall the decode task and change the very signal being
 * measured.
 */

/** @brief Arm a capture of the post-resampler PCM. 0 = free the buffer. */
int  app_pcm_capture_start(uint32_t seconds, uint32_t sample_rate);

/** @brief Feed samples to an armed capture. No-op when idle or full. */
void app_pcm_capture_feed(const int16_t* samples, uint32_t count);

/** @brief Base64 the captured block to the console. */
void app_pcm_capture_dump(void);

/** @brief Captured samples / capacity / the rate it was taken at. */
void app_pcm_capture_status(uint32_t* len, uint32_t* cap, uint32_t* rate);

/**
 * @brief Output PCM samples as raw data (for external plotting tools)
 * @param samples PCM buffer
 * @param count Number of samples
 */
void app_pcm_monitor_output_raw(const int16_t* samples, uint32_t count);

/**
 * @brief Print PCM statistics (min, max, average, RMS)
 * @param samples PCM buffer
 * @param count Number of samples
 */
void app_pcm_monitor_print_stats(const int16_t* samples, uint32_t count);

#endif // APP_PCM_MONITOR_H
