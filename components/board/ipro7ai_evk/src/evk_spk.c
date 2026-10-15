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
 * @file evk_spk.c
 * @brief DAC + TPA2037D1 speaker. See evk_spk.h.
 */

#include "evk_spk.h"
#include "evk_pcm.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "audio_output.h"
#include "hal_gpio.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "ipro_log.h"

#define LOG_TAG "EVK_SPK"

#define DEFAULT_RING_COUNT  4
#define DEFAULT_RING_MS     20
#define CHUNK_SAMPLES       320       /* volume scratch: 20 ms at 16 kHz, 10 ms at 32 kHz */

static audio_output_t *s_out;
static uint32_t        s_rate;
static int             s_volume_pct = 100;
static int32_t         s_volume_q12 = 4096;
static int16_t         s_scratch[CHUNK_SAMPLES];
static uint32_t        s_written;     /* samples handed to the sink since open */

/* ---------------------------------------------------------------------------
 * Amplifier enable
 * -------------------------------------------------------------------------*/

void evk_spk_enable(int on)
{
    uint8_t pin[1] = { EVK_SPK_PIN_EN };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(EVK_SPK_PIN_EN, GPIO_OUTPUT_PP_MODE);
    gpio_write(EVK_SPK_PIN_EN, (uint32_t)(on ? EVK_SPK_EN_ACTIVE : !EVK_SPK_EN_ACTIVE));
}

/* ---------------------------------------------------------------------------
 * Sink
 * -------------------------------------------------------------------------*/

int evk_spk_open(uint32_t rate_hz, uint8_t ring_count, uint16_t ring_ms)
{
    if (s_out) {
        return EVK_SPK_ERR_STATE;
    }
    if (rate_hz < 8000 || rate_hz > 48000) {
        return EVK_SPK_ERR_PARAM;
    }
    audio_output_cfg_t cfg = {
        .type              = AUDIO_OUTPUT_DAC,
        .mode              = AUDIO_OUTPUT_MODE_STREAMING,
        .sample_rate       = rate_hz,
        .channels          = 1,
        .ring_buf_count    = ring_count ? ring_count : DEFAULT_RING_COUNT,
        .ring_buf_ms       = ring_ms ? ring_ms : DEFAULT_RING_MS,
        .dac_spk_gpio      = EVK_SPK_PIN_EN,
        /* +1 = force active high. 0 would fall back to the board header, which
         * is right for CONFIG_IPRO7AI_EVK but wrong the day this library is
         * built against another board's header by mistake. */
        .dac_spk_en_active = EVK_SPK_EN_ACTIVE ? 1 : -1,
    };
    audio_output_t *out = audio_output_create(&cfg);

    if (!out) {
        IPRO_LOGE(LOG_TAG, "audio_output_create failed");
        return EVK_SPK_ERR_HW;
    }
    if (audio_output_start(out) != 0) {
        IPRO_LOGE(LOG_TAG, "audio_output_start failed");
        audio_output_destroy(out);
        return EVK_SPK_ERR_HW;
    }
    s_out     = out;
    s_rate    = rate_hz;
    s_written = 0;
    return EVK_SPK_OK;
}

void evk_spk_close(void)
{
    audio_output_t *out = s_out;

    if (!out) {
        return;
    }
    s_out = NULL;
    audio_output_stop(out);
    audio_output_destroy(out);
}

bool evk_spk_is_open(void)
{
    return s_out != NULL;
}

int evk_spk_write(const int16_t *pcm, uint32_t n)
{
    if (!s_out) {
        return EVK_SPK_ERR_STATE;
    }
    if (!pcm) {
        return EVK_SPK_ERR_PARAM;
    }
    if (s_volume_q12 == 4096) {
        if (audio_output_write(s_out, pcm, n) != 0) {
            return EVK_SPK_ERR_HW;
        }
        s_written += n;
        return EVK_SPK_OK;
    }
    while (n) {
        uint32_t k = n > CHUNK_SAMPLES ? CHUNK_SAMPLES : n;
        evk_pcm_apply_gain(pcm, s_scratch, k, s_volume_q12);
        if (audio_output_write(s_out, s_scratch, k) != 0) {
            return EVK_SPK_ERR_HW;
        }
        s_written += k;
        pcm += k;
        n   -= k;
    }
    return EVK_SPK_OK;
}

int evk_spk_write_silence(uint32_t n)
{
    if (!s_out) {
        return EVK_SPK_ERR_STATE;
    }
    memset(s_scratch, 0, sizeof(s_scratch));
    while (n) {
        uint32_t k = n > CHUNK_SAMPLES ? CHUNK_SAMPLES : n;
        if (audio_output_write(s_out, s_scratch, k) != 0) {
            return EVK_SPK_ERR_HW;
        }
        s_written += k;
        n -= k;
    }
    return EVK_SPK_OK;
}

uint32_t evk_spk_written(void)
{
    return s_written;
}

uint32_t evk_spk_played(void)
{
    return s_out ? audio_output_dac_consumed(s_out) : 0;
}

int evk_spk_set_volume(int pct)
{
    if (pct < 0 || pct > EVK_SPK_VOLUME_MAX) {
        return EVK_SPK_ERR_PARAM;
    }
    s_volume_pct = pct;
    s_volume_q12 = (int32_t)(4096 * pct / 100);
    return EVK_SPK_OK;
}

int evk_spk_get_volume(void)
{
    return s_volume_pct;
}

/* ---------------------------------------------------------------------------
 * Tone
 * -------------------------------------------------------------------------*/

int evk_spk_tone(uint32_t freq_hz, uint32_t ms, uint32_t amp_pct)
{
    static int16_t chunk[CHUNK_SAMPLES];
    const uint32_t rate  = EVK_SPK_NATIVE_RATE_HZ;
    uint32_t total = ms * rate / 1000u, done = 0, phase = 0;
    uint32_t step  = evk_pcm_sine_step(freq_hz, rate);
    int32_t  amp   = (int32_t)((amp_pct > 100u ? 100u : amp_pct) * 32767u / 100u);
    int rc;

    if (freq_hz == 0 || freq_hz > rate / 2u) {
        return EVK_SPK_ERR_PARAM;
    }
    rc = evk_spk_open(rate, 0, 0);
    if (rc != EVK_SPK_OK) {
        return rc;
    }
    while (done < total) {
        uint32_t n = total - done < CHUNK_SAMPLES ? total - done : CHUNK_SAMPLES;
        evk_pcm_sine_fill(chunk, n, &phase, step, amp);
        rc = evk_spk_write(chunk, n);
        if (rc != EVK_SPK_OK) {
            break;
        }
        done += n;
    }
    /* Let the ring drain before the amplifier is switched off, or the tail
     * is cut and the stop lands on a non-zero sample (pop). */
    evk_spk_write_silence(rate * (DEFAULT_RING_COUNT * DEFAULT_RING_MS) / 1000u);
    evk_spk_close();
    return rc;
}
