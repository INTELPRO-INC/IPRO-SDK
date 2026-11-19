/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 *
 * PCM16 output on the IPRO6S I2S port (master, standard I2S, 16-bit slots).
 *
 * The decoder writes PCM into a ring buffer; DMA plays two halves of a
 * ping-pong buffer and a refill task copies the next half from the ring each
 * time one finishes. Playback starts only once the ring holds PRIME_BYTES, so
 * the radio's packet jitter does not reach the DAC as gaps; an empty ring
 * outputs silence and waits to be primed again.
 *
 * Clock: the I2S block runs from the Wi-Fi PLL divider (48 MHz on IPRO6S).
 * 48 MHz is not a whole multiple of 44.1 kHz x 32, so BCLK alternates
 * between two adjacent integer dividers after every DMA half to hold the
 * long-term sample rate exact.
 */
#include "a2dp_i2s_out.h"

#include <string.h>

#include <generated/autoconf.h>
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

#define TAG "I2S"

#ifndef CONFIG_APP_I2S_PIN_BASE
#define CONFIG_APP_I2S_PIN_BASE 0
#endif
#if (CONFIG_APP_I2S_PIN_BASE % 4) != 0
#error "CONFIG_APP_I2S_PIN_BASE must be a multiple of 4 (BCLK, FS, DI, DO follow GPIO % 4)"
#endif

#define I2S_DMA_SAMPLES     256u  /* PCM16 samples per DMA half */
#define I2S_DMA_BYTES       (I2S_DMA_SAMPLES * sizeof(int16_t))
#define I2S_RING_BYTES      32768u
#define I2S_PRIME_BYTES     4096u
#define I2S_TASK_STACK      768u

static uint8_t s_ring_storage[I2S_RING_BYTES];
static audio_pcm_ring_t s_ring;
static int16_t s_dma_pcm[2][I2S_DMA_SAMPLES] __attribute__((aligned(32)));
static TaskHandle_t s_refill_task;
static dma_ch_handle_t s_dma_handle;
static uint8_t s_dma_ch;
static volatile uint8_t s_completed_half;
static volatile bool s_playing;
static volatile bool s_primed;
static volatile uint8_t s_channels = 2u;
static volatile uint32_t s_sample_rate_hz = 44100u;
static volatile uint32_t s_written_bytes;
static volatile uint32_t s_dropped_writes;
static volatile uint32_t s_underruns;
static uint32_t s_bclk_div_floor;
static uint32_t s_bclk_remainder;
static uint32_t s_bclk_denominator;
static uint32_t s_bclk_fraction_phase;

static uint32_t i2s_bclk_register(uint32_t total_clocks)
{
    uint32_t high_clocks = total_clocks / 2u;
    uint32_t low_clocks = total_clocks - high_clocks;

    return ((high_clocks - 1u) << 16) | (low_clocks - 1u);
}

static inline void i2s_advance_fractional_bclk(void)
{
    uint32_t divisor = s_bclk_div_floor;

    s_bclk_fraction_phase += s_bclk_remainder;
    if (s_bclk_fraction_phase >= s_bclk_denominator) {
        s_bclk_fraction_phase -= s_bclk_denominator;
        divisor++;
    }
    writel(i2s_bclk_register(divisor), I2S_BASE + I2S_BCLK_CONFIG_OFFSET);
}

static void i2s_dma_callback(void *dev, uint32_t event)
{
    BaseType_t woken = pdFALSE;
    uint32_t bit;

    (void)dev;
    if (event != HAL_DMA_INT_COMPLETE) {
        return;
    }
    bit = 1u << s_completed_half;
    s_completed_half ^= 1u;
    i2s_advance_fractional_bclk();
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
    .src_width = DMA_TRANSFER_WIDTH_32BIT,
    .dst_width = DMA_TRANSFER_WIDTH_32BIT,
    .lli_cfg = NULL,
    .callback = i2s_dma_callback,
};

static void refill_half(uint8_t half)
{
    uint8_t *dst = (uint8_t *)s_dma_pcm[half];
    bool read_ok = false;

    if (s_playing && !s_primed &&
        audio_pcm_ring_available(&s_ring) >= I2S_PRIME_BYTES) {
        s_primed = true;
    }
    if (s_playing && s_primed) {
        read_ok = audio_pcm_ring_read(&s_ring, dst, I2S_DMA_BYTES);
        if (!read_ok) {
            s_underruns++;
            s_primed = false;
        }
    }
    if (!read_ok) {
        memset(dst, 0, I2S_DMA_BYTES);
    }
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)dst, I2S_DMA_BYTES);
}

static void i2s_refill_task(void *arg)
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
    /* GPIO % 4 selects the I2S signal: 0 = BCLK, 1 = FS, 2 = DI, 3 = DO.
     * Only the three master outputs are routed; DI stays a plain GPIO. */
    GLB_GPIO_Cfg_Type pins[] = {
        { (GLB_GPIO_Type)(CONFIG_APP_I2S_PIN_BASE + 0), GPIO_FUN_I2S, GPIO_MODE_AF,
          GPIO_PULL_NONE, 1, 1 },
        { (GLB_GPIO_Type)(CONFIG_APP_I2S_PIN_BASE + 1), GPIO_FUN_I2S, GPIO_MODE_AF,
          GPIO_PULL_NONE, 1, 1 },
        { (GLB_GPIO_Type)(CONFIG_APP_I2S_PIN_BASE + 3), GPIO_FUN_I2S, GPIO_MODE_AF,
          GPIO_PULL_NONE, 1, 1 },
    };

    for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        GLB_GPIO_Init(&pins[i]);
    }
}

int a2dp_i2s_init(void)
{
    if (audio_pcm_ring_init(&s_ring, s_ring_storage, sizeof(s_ring_storage)) != 0) {
        return -1;
    }
    memset(s_dma_pcm, 0, sizeof(s_dma_pcm));
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)s_dma_pcm, sizeof(s_dma_pcm));

    if (xTaskCreate(i2s_refill_task, "i2s_out", I2S_TASK_STACK, NULL,
                    configMAX_PRIORITIES - 3, &s_refill_task) != pdPASS) {
        return -2;
    }

    configure_i2s_pins();
    GLB_Set_I2S_CLK(ENABLE, 0u, GLB_I2S_DI_SEL_I2S_DI_INPUT, GLB_I2S_DO_SEL_I2S_DO_OUTPT);
    s_dma_handle = dma_request_channel(DMA_REQUEST_I2S_TX, DMA_MEMORY_TO_PERIPH, "i2s_tx");
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
    dma_ioctl(DMA0_ID, s_dma_ch, DMA_CTRL_SET_CALLBACK, i2s_dma_callback);

    if (a2dp_i2s_configure(44100u, 2u) != 0) {
        return -5;
    }
    IPRO_LOGI(TAG, "ready: BCLK GPIO%d, FS GPIO%d, DO GPIO%d",
              CONFIG_APP_I2S_PIN_BASE, CONFIG_APP_I2S_PIN_BASE + 1,
              CONFIG_APP_I2S_PIN_BASE + 3);
    return 0;
}

int a2dp_i2s_configure(uint32_t sample_rate_hz, uint8_t channels)
{
    i2s_device_t i2s = {
        .id = 0,
        .iis_mode = I2S_MODE_MASTER,
        .interface_mode = I2S_MODE_STD,
        .sampl_freq_hz = sample_rate_hz,
        .channel_num = channels == 2u ? I2S_FS_CHANNELS_NUM_2 : I2S_FS_CHANNELS_NUM_MONO,
        .frame_size = I2S_FRAME_LEN_16,
        .data_size = I2S_DATA_LEN_16,
        .fifo_threshold = 4,
    };
    /* Stereo 16-bit: one 32-bit FIFO word carries an L/R pair. */
    dma_trans_param_t dma_width = {
        .map = DMA_PARAM_UPDATE_WIDTH,
        .src_width = channels == 2u ? DMA_TRANSFER_WIDTH_32BIT : DMA_TRANSFER_WIDTH_16BIT,
        .dst_width = channels == 2u ? DMA_TRANSFER_WIDTH_32BIT : DMA_TRANSFER_WIDTH_16BIT,
    };
    uint32_t source_clock_hz;
    uint32_t bit_clock_hz;

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
    s_completed_half = 0u;
    s_primed = false;
    audio_pcm_ring_reset(&s_ring);
    memset(s_dma_pcm, 0, sizeof(s_dma_pcm));
    taskEXIT_CRITICAL();
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)s_dma_pcm, sizeof(s_dma_pcm));

    hal_i2s_setConfig(&i2s);
    if (hal_i2s_init(0, COMMON_CAPS_DMA_TX) != 0) {
        return -2;
    }
    hal_i2s_control(0, I2S_CTRL_ATTACH_TX_DMA, (void *)(uintptr_t)s_dma_ch);
    dma_ioctl(DMA0_ID, s_dma_ch, DMA_CTRL_CHANNEL_TRANS_PARAM_UPDATE, &dma_width);
    if (dma_channel_attach(DMA0_ID, s_dma_ch) != 0) {
        return -3;
    }
    dma_ioctl(DMA0_ID, s_dma_ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, s_dma_ch, DMA_CTRL_SET_CALLBACK, i2s_dma_callback);

    source_clock_hz = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_I2S);
    if (source_clock_hz == 0u) {
        return -4;
    }
    bit_clock_hz = sample_rate_hz * 32u;   /* two 16-bit slots per frame */
    s_bclk_div_floor = source_clock_hz / bit_clock_hz;
    s_bclk_remainder = source_clock_hz % bit_clock_hz;
    s_bclk_denominator = bit_clock_hz;
    s_bclk_fraction_phase = 0u;
    writel(i2s_bclk_register(s_bclk_div_floor), I2S_BASE + I2S_BCLK_CONFIG_OFFSET);

    if (hal_i2s_write(0, s_dma_pcm[0], I2S_DMA_BYTES) != 0) {
        return -5;
    }
    IPRO_LOGI(TAG, "format %lu Hz, %u channel(s)", (unsigned long)sample_rate_hz, channels);
    return 0;
}

void a2dp_i2s_set_playing(bool playing)
{
    taskENTER_CRITICAL();
    s_playing = playing;
    s_primed = false;
    audio_pcm_ring_reset(&s_ring);
    taskEXIT_CRITICAL();
}

bool a2dp_i2s_write(const uint8_t *pcm, size_t len)
{
    if (pcm == NULL || len == 0u || (len & 1u) != 0u ||
        !audio_pcm_ring_write(&s_ring, pcm, len)) {
        s_dropped_writes++;
        return false;
    }
    s_written_bytes += (uint32_t)len;
    return true;
}

void a2dp_i2s_get_stats(a2dp_i2s_stats_t *stats)
{
    if (stats == NULL) {
        return;
    }
    stats->sample_rate_hz = s_sample_rate_hz;
    stats->channels = s_channels;
    stats->playing = s_playing;
    stats->written_bytes = s_written_bytes;
    stats->dropped_writes = s_dropped_writes;
    stats->underruns = s_underruns;
    stats->buffered_bytes = (uint32_t)audio_pcm_ring_available(&s_ring);
}
