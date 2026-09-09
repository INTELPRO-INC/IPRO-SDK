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

#include "ipro6_sco_i2s_tx.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include <compiler/common.h>
#include <hal_dma.h>
#include <hal_i2s.h>
#include <system_soc.h>
#include <drv_gpio.h>
#include <drv_glb.h>
#include <drv_clock.h>
#include <drv_dma.h>
#include <i2s_reg.h>
#include <ipro_log.h>

#include <audio_pcm_ring.h>

#define TAG "SCO_I2S"

#define SCO_I2S_SAMPLE_RATE       8000u
#define SCO_I2S_DMA_SAMPLES       64u
#define A2DP_I2S_DMA_SAMPLES      256u
#define I2S_DMA_MAX_SAMPLES       A2DP_I2S_DMA_SAMPLES
#define SCO_I2S_DMA_BYTES         (SCO_I2S_DMA_SAMPLES * sizeof(int16_t))
#define A2DP_I2S_DMA_BYTES        (A2DP_I2S_DMA_SAMPLES * sizeof(int16_t))
#define SCO_I2S_RING_BYTES        32768u
#define A2DP_I2S_PRIME_BYTES      4096u
#define SCO_I2S_TASK_STACK_WORDS  768u

static uint8_t s_ring_storage[SCO_I2S_RING_BYTES];
static audio_pcm_ring_t s_ring;
static int16_t s_dma_pcm[2][I2S_DMA_MAX_SAMPLES]
    __attribute__((aligned(32)));
static TaskHandle_t s_refill_task;
static dma_ch_handle_t s_dma_handle;
static uint8_t s_dma_ch;
static volatile uint8_t s_completed_half;
static volatile uint8_t s_a2dp_active;
static volatile uint8_t s_sco_active;
static volatile uint8_t s_tone_active;
static volatile uint8_t s_primed;
static volatile uint8_t s_channels = 1u;
static volatile uint16_t s_dma_bytes = SCO_I2S_DMA_BYTES;
static volatile uint32_t s_sample_rate_hz = SCO_I2S_SAMPLE_RATE;
static volatile uint32_t s_input_packets;
static volatile uint32_t s_input_bytes;
static volatile uint32_t s_input_drops;
static volatile uint32_t s_dma_frames;
static volatile uint32_t s_dma_underruns;
static volatile uint32_t s_dma_errors;
static volatile uint32_t s_dma_callback_ch[DMA_MAX_INDEX];
static volatile uint32_t s_dma_lli_changes;
static volatile uint32_t s_dma_lli_repeats;
static volatile uint32_t s_dma_last_lli;
static volatile uint32_t s_tx_hash = 2166136261u;
static uint32_t s_tone_phase;
static uint32_t s_bclk_div_floor;
static uint32_t s_bclk_remainder;
static uint32_t s_bclk_denominator;
static uint32_t s_bclk_fraction_phase;

/* IPRO6S feeds I2S from the 960 MHz Wi-Fi PLL VCO divided by postdiv=20,
 * or 48 MHz.  PCM16 mono still occupies two 16-bit I2S slots, so an exact
 * 8 kHz sample clock needs a fractional BCLK divisor:
 *
 *     48 MHz / (8 kHz * 32) = 187.5
 *
 * The BCLK register has integer high/low periods only.  The accumulator below
 * alternates adjacent integer divisors so the long-term sample rate is exact.
 * For 44.1 kHz stereo the corresponding divisor is about 34.014.
 */
static uint32_t sco_i2s_bclk_register(uint32_t total_clocks)
{
    uint32_t high_clocks = total_clocks / 2u;
    uint32_t low_clocks = total_clocks - high_clocks;

    return ((high_clocks - 1u) << 16) | (low_clocks - 1u);
}

static inline void sco_i2s_advance_fractional_bclk(void)
{
    uint32_t divisor = s_bclk_div_floor;

    s_bclk_fraction_phase += s_bclk_remainder;
    if (s_bclk_fraction_phase >= s_bclk_denominator) {
        s_bclk_fraction_phase -= s_bclk_denominator;
        divisor++;
    }
    writel(sco_i2s_bclk_register(divisor),
           I2S_BASE + I2S_BCLK_CONFIG_OFFSET);
}

static void sco_i2s_dma_callback(void *dev, uint32_t event)
{
    BaseType_t woken = pdFALSE;
    const dma_device_t *dma = (const dma_device_t *)dev;
    uint32_t bit;
    uint32_t lli;

    if (dma != NULL && dma->ch < DMA_MAX_INDEX) {
        s_dma_callback_ch[dma->ch]++;
    }
    if (event != HAL_DMA_INT_COMPLETE) {
        s_dma_errors++;
        return;
    }

    lli = readl(DMA_BASE + 0x100u + ((uint32_t)s_dma_ch * 0x100u) +
                DMA_LLI_OFFSET);
    if (lli == s_dma_last_lli) {
        s_dma_lli_repeats++;
    } else {
        s_dma_last_lli = lli;
        s_dma_lli_changes++;
    }

    bit = 1u << s_completed_half;
    s_completed_half ^= 1u;
    s_dma_frames++;
    sco_i2s_advance_fractional_bclk();
    if (s_refill_task != NULL) {
        xTaskNotifyFromISR(s_refill_task, bit, eSetBits, &woken);
        portYIELD_FROM_ISR(woken);
    }
}

static dma_device_t s_i2s_tx_dma = {
    .id = DMA0_ID,
    .ch = DMA0_CH0_INDEX,
    .transfer_mode = DMA_LLI_PINGPONG_MODE,
    .direction = DMA_MEMORY_TO_PERIPH,
    .src_req = DMA_REQUEST_NONE,
    .dst_req = DMA_REQUEST_I2S_TX,
    .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_size = DMA_BURST_INCR4,
    .dst_burst_size = DMA_BURST_INCR4,
    .src_width = DMA_TRANSFER_WIDTH_16BIT,
    .dst_width = DMA_TRANSFER_WIDTH_16BIT,
    .lli_cfg = NULL,
    .callback = sco_i2s_dma_callback,
};

static void fill_tone(int16_t *dst, uint32_t samples)
{
    static const int16_t tone_1khz[8] = {
        0, 11585, 16384, 11585, 0, -11585, -16384, -11585,
    };

    if (s_channels == 2u) {
        for (uint32_t i = 0; i + 1u < samples; i += 2u) {
            int16_t value = tone_1khz[s_tone_phase++ & 7u];

            dst[i] = value;
            dst[i + 1u] = value;
        }
    } else {
        for (uint32_t i = 0; i < samples; i++) {
            dst[i] = tone_1khz[s_tone_phase++ & 7u];
        }
    }
}

static void refill_half(uint8_t half)
{
    uint8_t *dst = (uint8_t *)s_dma_pcm[half];
    uint32_t hash = s_tx_hash;
    uint32_t dma_bytes = s_dma_bytes;
    bool stream_active = s_sco_active != 0u || s_a2dp_active != 0u;
    bool read_ok = false;

    if (s_tone_active != 0u) {
        fill_tone(s_dma_pcm[half], dma_bytes / sizeof(int16_t));
    } else {
        if (s_a2dp_active != 0u && s_primed == 0u &&
            audio_pcm_ring_available(&s_ring) >= A2DP_I2S_PRIME_BYTES) {
            s_primed = 1u;
        }
        if (stream_active && (s_a2dp_active == 0u || s_primed != 0u)) {
            read_ok = audio_pcm_ring_read(&s_ring, dst, dma_bytes);
        }
        if (!read_ok) {
            memset(dst, 0, dma_bytes);
            if (stream_active &&
                (s_a2dp_active == 0u || s_primed != 0u)) {
                s_dma_underruns++;
                if (s_a2dp_active != 0u) {
                    s_primed = 0u;
                }
            }
        }
    }

    for (uint32_t i = 0; i < dma_bytes; i++) {
        hash = (hash ^ dst[i]) * 16777619u;
    }
    s_tx_hash = hash;
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)dst, dma_bytes);
}

static void sco_i2s_refill_task(void *arg)
{
    uint32_t bits;

    (void)arg;
    for (;;) {
        if (xTaskNotifyWait(0u, UINT32_MAX, &bits, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if ((bits & 1u) != 0u) {
            refill_half(0u);
        }
        if ((bits & 2u) != 0u) {
            refill_half(1u);
        }
    }
}

static void configure_i2s_pins(void)
{
    /* IPRO6S I2S signal selection follows the BL616-class GPIO route:
     *   n+0=BCLK, n+1=FS, n+2=DI, n+3=DO.
     * GPIO25 is unavailable and GPIO17 is tied to XTAL on the target board.
     * Use GPIO4/5/7 for the three master outputs. */
    GLB_GPIO_Cfg_Type pins[] = {
        { GLB_GPIO_PIN_4, GPIO_FUN_I2S, GPIO_MODE_AF, GPIO_PULL_NONE, 1, 1 },
        { GLB_GPIO_PIN_5, GPIO_FUN_I2S, GPIO_MODE_AF, GPIO_PULL_NONE, 1, 1 },
        { GLB_GPIO_PIN_7, GPIO_FUN_I2S, GPIO_MODE_AF, GPIO_PULL_NONE, 1, 1 },
    };

    for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        GLB_GPIO_Init(&pins[i]);
    }
}

int ipro6_sco_i2s_init(void)
{
    if (audio_pcm_ring_init(&s_ring, s_ring_storage,
                            sizeof(s_ring_storage)) != 0) {
        return -1;
    }
    memset(s_dma_pcm, 0, sizeof(s_dma_pcm));
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)s_dma_pcm, sizeof(s_dma_pcm));

    if (xTaskCreate(sco_i2s_refill_task, "sco_i2s", SCO_I2S_TASK_STACK_WORDS,
                    NULL, configMAX_PRIORITIES - 3, &s_refill_task) != pdPASS) {
        return -2;
    }

    configure_i2s_pins();
    GLB_Set_I2S_CLK(ENABLE, 0u, GLB_I2S_DI_SEL_I2S_DI_INPUT,
                    GLB_I2S_DO_SEL_I2S_DO_OUTPT);
    s_dma_handle = dma_request_channel(DMA_REQUEST_I2S_TX,
                                       DMA_MEMORY_TO_PERIPH, "sco_i2s_tx");
    if (s_dma_handle == NULL) {
        return -3;
    }
    s_dma_ch = (uint8_t)dma_handle_to_ch(s_dma_handle);
    s_i2s_tx_dma.ch = s_dma_ch;
    dma_setConfig(&s_i2s_tx_dma);
    if (dma_init(DMA0_ID, s_dma_ch) != 0) {
        return -4;
    }
    dma_ioctl(DMA0_ID, s_dma_ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, s_dma_ch, DMA_CTRL_SET_CALLBACK,
              sco_i2s_dma_callback);

    if (ipro6_sco_i2s_configure(SCO_I2S_SAMPLE_RATE, 1u) != 0) {
        return -5;
    }

    IPRO_LOGI(TAG, "ready: 8kHz PCM16 mono, GPIO4=BCLK GPIO5=FS GPIO7=DO dma=%u",
              s_dma_ch);
    return 0;
}

int ipro6_sco_i2s_configure(uint32_t sample_rate_hz, uint8_t channels)
{
    i2s_device_t i2s = {
        .id = 0,
        .iis_mode = I2S_MODE_MASTER,
        .interface_mode = I2S_MODE_STD,
        .sampl_freq_hz = sample_rate_hz,
        .channel_num = channels == 2u ? I2S_FS_CHANNELS_NUM_2
                                      : I2S_FS_CHANNELS_NUM_MONO,
        .frame_size = I2S_FRAME_LEN_16,
        .data_size = I2S_DATA_LEN_16,
        .fifo_threshold = 4,
    };
    uint32_t source_clock_hz;
    uint32_t postdiv;
    uint32_t refdiv;
    uint32_t bit_clock_hz;
    dma_trans_param_t dma_width = {
        .map = DMA_PARAM_UPDATE_WIDTH,
        .src_width = channels == 2u ? DMA_TRANSFER_WIDTH_32BIT
                                    : DMA_TRANSFER_WIDTH_16BIT,
        .dst_width = channels == 2u ? DMA_TRANSFER_WIDTH_32BIT
                                    : DMA_TRANSFER_WIDTH_16BIT,
    };
    int ret;

    if ((sample_rate_hz != 8000u && sample_rate_hz != 16000u &&
         sample_rate_hz != 32000u && sample_rate_hz != 44100u &&
         sample_rate_hz != 48000u) ||
        (channels != 1u && channels != 2u)) {
        return -1;
    }

    dma_channel_stop(DMA0_ID, s_dma_ch);
    taskENTER_CRITICAL();
    s_sample_rate_hz = sample_rate_hz;
    s_channels = channels;
    s_dma_bytes = sample_rate_hz <= 16000u ? SCO_I2S_DMA_BYTES
                                           : A2DP_I2S_DMA_BYTES;
    s_completed_half = 0u;
    s_primed = 0u;
    audio_pcm_ring_reset(&s_ring);
    memset(s_dma_pcm, 0, sizeof(s_dma_pcm));
    taskEXIT_CRITICAL();
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)s_dma_pcm, sizeof(s_dma_pcm));

    hal_i2s_setConfig(&i2s);
    ret = hal_i2s_init(0, COMMON_CAPS_DMA_TX);
    if (ret != 0) {
        return -2;
    }
    hal_i2s_control(0, I2S_CTRL_ATTACH_TX_DMA,
                    (void *)(uintptr_t)s_dma_ch);

    /* In stereo/16-bit mode the HAL enables the I2S L/R merge path.  Its
     * FIFO request is one 32-bit word containing an interleaved L/R pair.
     * Keeping the SCO 16-bit DMA width here halves the PCM consumption rate
     * and splits channel pairs across requests, which fills the ring and is
     * audible as noise. */
    dma_ioctl(DMA0_ID, s_dma_ch, DMA_CTRL_CHANNEL_TRANS_PARAM_UPDATE,
              &dma_width);
    if (dma_channel_attach(DMA0_ID, s_dma_ch) != 0) {
        return -3;
    }
    dma_ioctl(DMA0_ID, s_dma_ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, s_dma_ch, DMA_CTRL_SET_CALLBACK,
              sco_i2s_dma_callback);

    postdiv = reg_get_bits(readl(CCI_BASE + CCI_WIFI_PLL_CFG1_OFFSET),
                           CCI_WIFIPLL_POSTDIV);
    refdiv = reg_get_bits(readl(GLB_BASE + GLB_I2S_CFG0_OFFSET),
                          GLB_REG_I2S_REF_CLK_DIV);
    source_clock_hz = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_I2S);
    if (source_clock_hz == 0u) {
        return -5;
    }
    IPRO_LOGI(TAG,
              "i2s clk: postdiv=%lu refdiv=%lu source=%lu Hz",
              (unsigned long)postdiv, (unsigned long)refdiv,
              (unsigned long)source_clock_hz);
    bit_clock_hz = sample_rate_hz * 32u;
    s_bclk_div_floor = source_clock_hz / bit_clock_hz;
    s_bclk_remainder = source_clock_hz % bit_clock_hz;
    s_bclk_denominator = bit_clock_hz;
    s_bclk_fraction_phase = 0u;
    writel(sco_i2s_bclk_register(s_bclk_div_floor),
           I2S_BASE + I2S_BCLK_CONFIG_OFFSET);

    ret = hal_i2s_write(0, s_dma_pcm[0], s_dma_bytes);
    if (ret != 0) {
        return -4;
    }
    IPRO_LOGI(TAG, "configured: %luHz PCM16 channels=%u dma_bytes=%u bclk=%lu+%lu/%lu",
              (unsigned long)sample_rate_hz, channels, s_dma_bytes,
              (unsigned long)s_bclk_div_floor,
              (unsigned long)s_bclk_remainder,
              (unsigned long)s_bclk_denominator);
    return 0;
}

void ipro6_sco_i2s_set_a2dp_active(bool active)
{
    taskENTER_CRITICAL();
    s_a2dp_active = active ? 1u : 0u;
    s_primed = 0u;
    audio_pcm_ring_reset(&s_ring);
    taskEXIT_CRITICAL();
    if (active) {
        s_dma_underruns = 0u;
    }
}

void ipro6_sco_i2s_set_sco_active(bool active)
{
    taskENTER_CRITICAL();
    s_sco_active = active ? 1u : 0u;
    s_primed = active ? 1u : 0u;
    audio_pcm_ring_reset(&s_ring);
    taskEXIT_CRITICAL();
    if (active) {
        s_dma_underruns = 0u;
    }
}

bool ipro6_sco_i2s_write(const uint8_t *pcm, size_t len)
{
    s_input_packets++;
    s_input_bytes += (uint32_t)len;
    if (pcm == NULL || len == 0u || (len & 1u) != 0u ||
        !audio_pcm_ring_write(&s_ring, pcm, len)) {
        s_input_drops++;
        return false;
    }
    return true;
}

void ipro6_sco_i2s_set_tone(bool enable)
{
    taskENTER_CRITICAL();
    s_tone_active = enable ? 1u : 0u;
    audio_pcm_ring_reset(&s_ring);
    taskEXIT_CRITICAL();
}

void ipro6_sco_i2s_get_stats(ipro6_sco_i2s_stats_t *stats)
{
    if (stats == NULL) {
        return;
    }
    stats->input_packets = s_input_packets;
    stats->input_bytes = s_input_bytes;
    stats->input_drops = s_input_drops;
    stats->dma_frames = s_dma_frames;
    stats->dma_underruns = s_dma_underruns;
    stats->dma_errors = s_dma_errors;
    for (uint32_t i = 0u; i < DMA_MAX_INDEX; i++) {
        stats->dma_callback_ch[i] = s_dma_callback_ch[i];
    }
    stats->dma_lli_changes = s_dma_lli_changes;
    stats->dma_lli_repeats = s_dma_lli_repeats;
    stats->dma_last_lli = s_dma_last_lli;
    stats->tx_hash = s_tx_hash;
    stats->ring_bytes = (uint32_t)audio_pcm_ring_available(&s_ring);
    stats->sample_rate_hz = s_sample_rate_hz;
    stats->dma_bytes = s_dma_bytes;
    stats->channels = s_channels;
    stats->a2dp_active = s_a2dp_active;
    stats->sco_active = s_sco_active;
    stats->tone_active = s_tone_active;
    stats->primed = s_primed;
    stats->dma_channel = s_dma_ch;
}
