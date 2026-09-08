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

#ifndef __HAL_AUADC_H__
#define __HAL_AUADC_H__

#include "drv_auadc_v1.h"
#include "drv_glb.h"
#include "drv_dma.h"

typedef void (*hal_auadc_callback_t)(int buf_idx);

// amic_single_ended: 0 - differential mode (pos + neg), 1 - single ended mode (pos only)
// amic_pos_ch/amic_neg_ch: 0 - 7, corresponding to gpio {2, 3, 7, 8, 10, 11, 14, 15}
// pcm_frame_size: 1 - 4095
typedef struct {
    uint8_t amic_single_ended;
    uint8_t amic_pos_ch;
    uint8_t amic_neg_ch;
    uint16_t pcm_frame_size;
    int16_t *pcm_frame_buf[2];
    hal_auadc_callback_t pcm_frame_event;
} hal_auadc_amic_cfg_t;

// pdm_clk_pin: 0, 2, 8
// pdm_in_pin: 1, 3, 7
// pcm_frame_size: 1 - 4095
typedef struct {
    uint8_t pdm_clk_pin;
    uint8_t pdm_in_pin;
    uint8_t pdm_ch;
    uint16_t pcm_frame_size;
    int16_t *pcm_frame_buf[2];
    hal_auadc_callback_t pcm_frame_event;
    /*
     * Capture rate in Hz: 8000, 16000, 32000 or 48000. Zero keeps the
     * historical default of 16000 so existing callers are unaffected.
     *
     * The real rate is the audio clock divided down, not the nominal figure:
     * with GLB_AUDIO_CLK_SRC_2P032M the "16 kHz" setting yields 2032000/128 =
     * 15875 Hz and "32 kHz" yields 2032000/64 = 31750 Hz. Anything that needs
     * an absolute time base must use the real rate.
     */
    uint32_t sample_rate;
} hal_auadc_pdm_cfg_t;

/** @brief Real capture rate for a nominal setting, in Hz. */
uint32_t hal_auadc_real_rate(uint32_t nominal_rate);

enum {
    HAL_AUADC_PDM_CH_LEFT,                   /*!< AUADC Souce select PDM left */
    HAL_AUADC_PDM_CH_RIGHT,                  /*!< AUADC Souce select PDM right */
};

int hal_auadc_amic_init(hal_auadc_amic_cfg_t *cfg);
int hal_auadc_pdm_init(hal_auadc_pdm_cfg_t *cfg);

int hal_auadc_start(void);
int hal_auadc_stop(void);

/*
 * Release DMA/AUADC hardware resources. Pairs with
 * hal_auadc_amic_init / hal_auadc_pdm_init. Must be called after
 * hal_auadc_stop when the DMA channel is no longer needed — without
 * this, the allocator leaks the channel for the lifetime of the
 * process. A no-op if no init has happened.
 */
void hal_auadc_deinit(void);

// 1-order high pass filter
//                  1 - z^-1
// H(z) = -----------------------------
//         1 - (1 - 2^-k1 - 2^-k2)z^-1
// k1: available range 3 - 13; set 0 to disable it; default 8
// k2: available range 3 - 13; set 0 to disable it; default disabled
void hal_auadc_set_high_pass_filter(int k1, int k2);

void hal_auadc_set_analog_gain(int gain_db);  // gain_db: 6dB to 42dB in 3dB step
void hal_auadc_set_digital_gain(float gain_db);  // gain_db: -95.5dB to +18dB in 0.5dB step

int hal_auadc_get_analog_gain(void);
float hal_auadc_get_digital_gain(void);

#endif
