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
 * @file evk_mic.h
 * @brief PDM microphone (IM69D129 on GPIO18/19 through the AUADC).
 *
 * Frames of EVK_MIC_FRAME_SAMPLES (20 ms at the nominal 16 kHz) arrive by
 * DMA into a non-cached OCRAM ping-pong; the completion ISR copies each one
 * into a FreeRTOS stream buffer and evk_mic_read() hands them to the caller.
 * The reader is expected to keep up: a full stream buffer drops the newest
 * frame and counts it.
 *
 * hal_auadc_pdm_init() may only run once per boot (the SDK's PDM demo:
 * "reinit breaks DMA on subsequent calls"), so the hardware is configured on
 * the first evk_mic_start() and later starts/stops only gate the AUADC.
 * Everything here is integer arithmetic, including the digital-gain register
 * write (the HAL's float entry point is not used) - the FreeRTOS port does
 * not save the FPU across tasks.
 */
#ifndef EVK_MIC_H
#define EVK_MIC_H

#include <stdint.h>
#include <stdbool.h>

#include "evk_audio_port.h"

#define EVK_MIC_OK          0
#define EVK_MIC_ERR_PARAM  -1
#define EVK_MIC_ERR_NOMEM  -2
#define EVK_MIC_ERR_HW     -3

typedef struct {
    uint32_t isr_frames;     /* completions since boot, counted even when idle */
    uint32_t delivered;      /* frames queued for the reader                   */
    uint32_t dropped;        /* frames lost because the reader was late        */
    uint32_t discarded;      /* start-up frames thrown away (mic settling)     */
} evk_mic_stats_t;

/** @brief Start capture (configures the AUADC on first use). */
int  evk_mic_start(void);
void evk_mic_stop(void);
bool evk_mic_running(void);

/**
 * @brief Pull one frame of EVK_MIC_FRAME_SAMPLES int16 samples.
 * @return Samples copied (EVK_MIC_FRAME_SAMPLES), or 0 on timeout.
 */
uint32_t evk_mic_read(int16_t *dst, uint32_t timeout_ms);

/** @brief Drop whatever is queued (call after a pause before reading again). */
void evk_mic_flush(void);

/** @brief AUADC digital gain, 0..18 dB. Applied immediately when running. */
int  evk_mic_set_gain_db(int db);
int  evk_mic_get_gain_db(void);

/** @brief What the AUADC really produces for the nominal rate (~15875 Hz). */
uint32_t evk_mic_real_rate(void);

void evk_mic_get_stats(evk_mic_stats_t *out);

/**
 * @brief Peak and RMS of @p n samples, integer only.
 * @param peak  max |sample|, 0..32768
 * @param rms   sqrt(mean(sample^2)), 0..32767
 */
void evk_mic_level(const int16_t *pcm, uint32_t n, uint16_t *peak, uint16_t *rms);

/** @brief 20*log10(v/32768) in tenths of a dB, integer only; -1200 for 0. */
int  evk_mic_dbfs_x10(uint32_t v);

#endif /* EVK_MIC_H */
