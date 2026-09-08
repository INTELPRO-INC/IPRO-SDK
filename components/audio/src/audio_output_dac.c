/**
 * @file audio_output_dac.c
 * @brief DAC audio output sink - one-shot + N-ring streaming
 *
 * STREAMING mode: DMA_LLI_CYCLE_N_MODE with N cache-line aligned ring
 * segments (N and segment-ms are runtime-configurable). ISR-driven flow
 * control, glitch-free continuous playback. Based on LE Audio app_dac.c.
 *
 * ONE-SHOT mode: DMA_LLI_ONCE_MODE with a single buffer; write() blocks
 * on a binary semaphore until DMA INT_COMPLETE fires.
 *
 * Both modes share GPIO/DAC/resample/pcm_to_dac plumbing. Because the SoC
 * has exactly one DAC, this backend enforces a single live instance.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#include "audio_output.h"
#include "audio_speaker_eq.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_cache_alloc.h>
#include <hal_dma.h>
#include <hal_dac.h>
#include <hal_gpio.h>
#include <drv_glb.h>
#include "system_soc.h"

/* Hardware config */
#define DAC_GPIO_SPK_DEFAULT    22

/* Legacy default when CONFIG_DMA_CHANNEL_ALLOCATOR is off. With the
 * allocator enabled, the channel is obtained at start-time via
 * dma_request_channel(DMA_REQUEST_DAC0, ...) and may land on any of
 * CH0..CH7 — the peripheral request mux routes DMA_REQUEST_DAC0
 * freely (verified 2026-04-20 via apps/test/dma_mux_probe). */
#define DAC_DMA_CH              DMA0_CH1_INDEX

/* DAC is 10-bit: output range 0-1023, center = 512 */
#define DAC_SILENCE             512

/* On-chip DAC is fixed-rate; framework resamples to this rate */
#define DAC_RATE                32000

/* Streaming-mode defaults (overridable via cfg) */
#define DAC_RING_COUNT_DEFAULT  3
#define DAC_RING_MS_DEFAULT     64

/* Max mono samples per write batch (for channel mix + resample scratch).
 * 1152 = MP3 frame size; fine for all practical PCM feeders. */
#define MAX_MONO_SAMPLES        1152
#define MAX_RESAMPLE_SAMPLES    ((uint32_t)((uint64_t)MAX_MONO_SAMPLES * DAC_RATE / 8000) + 2)

/* --------------------------------------------------------------------- */
/* Resample + PCM conversion helpers                                      */
/* --------------------------------------------------------------------- */

/* Linear-interpolation resampler for arbitrary rate conversion */
static uint32_t resample_linear(const int16_t *in, uint32_t in_count,
                                int16_t *out, uint32_t out_cap,
                                uint32_t in_rate, uint32_t out_rate)
{
    if (in_rate == out_rate) {
        uint32_t n = (in_count < out_cap) ? in_count : out_cap;
        memcpy(out, in, n * sizeof(int16_t));
        return n;
    }

    uint32_t out_count = (uint32_t)((uint64_t)in_count * out_rate / in_rate);
    if (out_count > out_cap) {
        out_count = out_cap;
    }

    for (uint32_t i = 0; i < out_count; i++) {
        uint64_t pos64 = (uint64_t)i * in_rate;
        uint32_t pos = (uint32_t)(pos64 / out_rate);
        uint32_t frac = (uint32_t)((pos64 % out_rate) * 65536 / out_rate);

        if (pos + 1 < in_count) {
            int32_t s0 = in[pos];
            int32_t s1 = in[pos + 1];
            out[i] = (int16_t)((s0 * (65536 - frac) + s1 * frac) >> 16);
        } else {
            out[i] = in[pos < in_count ? pos : in_count - 1];
        }
    }
    return out_count;
}

/* Convert int16_t PCM -> 10-bit unsigned DAC (0-1023) */
/*
 * Speaker correction runs here rather than in the write path: this is the one
 * point both the streaming and one-shot paths pass through, it is always at
 * DAC_RATE (so the coefficients are valid), and src stays read-only, so the
 * caller's buffer is never touched.
 */
#if DAC_RATE != AUDIO_SPEAKER_EQ_RATE
#error "speaker EQ coefficients are designed for DAC_RATE"
#endif

static void pcm_to_dac(const int16_t *src, uint16_t *dst, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        int32_t s = (audio_speaker_eq_sample((int32_t)src[i]) >> 6) + DAC_SILENCE;
        if (s < 0) s = 0;
        if (s > 1023) s = 1023;
        dst[i] = (uint16_t)s;
    }
}

static void fill_silence(uint16_t *buf, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        buf[i] = DAC_SILENCE;
    }
}

/* --------------------------------------------------------------------- */
/* Instance state                                                         */
/* --------------------------------------------------------------------- */

struct audio_output {
    audio_output_cfg_t  cfg;              /* MUST be first field (audio_output.c
                                             casts the handle back to cfg*) */
    int                 need_resample;
    int                 spk_gpio;
    int                 started;
#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    dma_ch_handle_t     dma_handle;       /* allocator-owned channel handle */
#endif

    /* Streaming-mode state */
    uint8_t             ring_count;       /* N buffers */
    uint32_t            ring_samples;     /* samples per segment */
    uint16_t           *dma_buf;          /* contiguous N*ring_samples u16 */
    volatile uint8_t    active_idx;
    volatile uint8_t    fill_idx;
    volatile int        buf_ready[8];     /* max 8 buffers, plenty */
    volatile uint32_t   segments_done;    /* DMA segments played since start */
    volatile int        running;

    /* Leftover accumulator for partial segments (streaming) */
    int16_t            *leftover;         /* ring_samples int16 */
    uint32_t            leftover_count;

    /* One-shot mode state */
    uint16_t           *oneshot_buf;      /* heap, sized per-write */
    uint32_t            oneshot_cap;

    /* Pre-allocated work buffers (avoid malloc per frame) */
    int16_t             mono_buf[MAX_MONO_SAMPLES];
    int16_t             resample_buf[MAX_RESAMPLE_SAMPLES];

    SemaphoreHandle_t   feed_sem;         /* streaming: DMA gives */
    SemaphoreHandle_t   done_sem;         /* one-shot: DMA signals completion */
};

/* The SoC has one DAC; enforce a single live instance */
static audio_output_t *s_dac_out;

static inline uint16_t *get_segment(audio_output_t *out, uint8_t idx)
{
    return &out->dma_buf[idx * out->ring_samples];
}

/* --------------------------------------------------------------------- */
/* DMA callback (shared by both modes)                                    */
/* --------------------------------------------------------------------- */

static void dac_dma_callback(void *dev, uint32_t event)
{
    BaseType_t woken = pdFALSE;
    (void)dev;

    audio_output_t *out = s_dac_out;
    if (!out || event != HAL_DMA_INT_COMPLETE) {
        return;
    }

    if (out->cfg.mode == AUDIO_OUTPUT_MODE_ONESHOT) {
        xSemaphoreGiveFromISR(out->done_sem, &woken);
    } else {
        uint8_t done = out->active_idx;
        out->active_idx = (done + 1) % out->ring_count;
        out->buf_ready[done] = 0;
        out->segments_done++;
        xSemaphoreGiveFromISR(out->feed_sem, &woken);
    }
    portYIELD_FROM_ISR(woken);
}

/* DMA template - transfer_mode/lli_count are patched in start() */
static dma_device_t dac_dma_dev = {
    .id = DMA0_ID,
    .ch = DAC_DMA_CH,
    .direction = DMA_MEMORY_TO_PERIPH,
    .src_req = DMA_REQUEST_NONE,
    .dst_req = DMA_REQUEST_DAC0,
    .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_size = DMA_BURST_INCR8,
    .dst_burst_size = DMA_BURST_INCR1,
    .src_width = DMA_TRANSFER_WIDTH_16BIT,
    .dst_width = DMA_TRANSFER_WIDTH_16BIT,
    .lli_cfg = NULL,
    .callback = dac_dma_callback,
};

/* --------------------------------------------------------------------- */
/* Create / destroy                                                       */
/* --------------------------------------------------------------------- */

audio_output_t *audio_output_dac_create(const audio_output_cfg_t *cfg)
{
    if (s_dac_out) {
        printf("dac_out: another DAC instance already live\n");
        return NULL;
    }

    audio_output_t *out = pvPortMalloc(sizeof(audio_output_t));
    if (!out) {
        return NULL;
    }
    memset(out, 0, sizeof(*out));
    out->cfg = *cfg;
    out->need_resample = (cfg->sample_rate != DAC_RATE);
    /* Treat 0 (left-default by C designated initializers — common in
     * existing call sites like mp3_demo.c) AND negative values as
     * "use the board default GPIO". Only a positive pin number
     * overrides. The previous `>= 0` check silently routed mp3_play
     * to GPIO0 instead of GPIO22, leaving the LM4871 SHUTDOWN pin
     * unwritten and the speaker permanently muted. */
    out->spk_gpio = (cfg->dac_spk_gpio > 0) ? cfg->dac_spk_gpio
                                             : DAC_GPIO_SPK_DEFAULT;

    /* Resolve ring-buffer parameters */
    uint8_t  N  = cfg->ring_buf_count ? cfg->ring_buf_count : DAC_RING_COUNT_DEFAULT;
    uint16_t ms = cfg->ring_buf_ms    ? cfg->ring_buf_ms    : DAC_RING_MS_DEFAULT;
    if (N > 8) N = 8;  /* buf_ready[] capacity */
    out->ring_count   = N;
    out->ring_samples = (uint32_t)DAC_RATE * ms / 1000;

    printf("dac_out: src %lu Hz%s, mode=%s",
           (unsigned long)cfg->sample_rate,
           out->need_resample ? " (resample)" : "",
           cfg->mode == AUDIO_OUTPUT_MODE_ONESHOT ? "oneshot" : "streaming");
    if (cfg->mode != AUDIO_OUTPUT_MODE_ONESHOT) {
        printf(", ring=%ux%lu samples (%ums)",
               N, (unsigned long)out->ring_samples, ms);
    }
    printf("\n");

    if (cfg->mode == AUDIO_OUTPUT_MODE_ONESHOT) {
        out->done_sem = xSemaphoreCreateBinary();
        if (!out->done_sem) goto fail;
    } else {
        out->feed_sem = xSemaphoreCreateBinary();
        if (!out->feed_sem) goto fail;

        /* DMA ring buffer (cache-line aligned; vPortFreeCacheAligned
         * stores the original raw pointer at [-1] so destroy is clean) */
        uint32_t total = out->ring_count * out->ring_samples;
        out->dma_buf = pvPortMallocCacheAligned(total * sizeof(uint16_t));
        if (!out->dma_buf) goto fail;

        out->leftover = pvPortMalloc(out->ring_samples * sizeof(int16_t));
        if (!out->leftover) goto fail;

        for (int i = 0; i < out->ring_count; i++) {
            fill_silence(get_segment(out, i), out->ring_samples);
        }
    }

    s_dac_out = out;
    return out;

fail:
    if (out->feed_sem) vSemaphoreDelete(out->feed_sem);
    if (out->done_sem) vSemaphoreDelete(out->done_sem);
    vPortFree(out);
    return NULL;
}

int audio_output_dac_stop(audio_output_t *out);  /* fwd */

void audio_output_dac_destroy(audio_output_t *out)
{
    if (!out) return;
    audio_output_dac_stop(out);

    if (out->feed_sem)  vSemaphoreDelete(out->feed_sem);
    if (out->done_sem)  vSemaphoreDelete(out->done_sem);
    if (out->leftover)  vPortFree(out->leftover);
    if (out->dma_buf)   vPortFreeCacheAligned(out->dma_buf);
    if (out->oneshot_buf) vPortFreeCacheAligned(out->oneshot_buf);

    if (s_dac_out == out) s_dac_out = NULL;
    vPortFree(out);
}

/* --------------------------------------------------------------------- */
/* Start / stop                                                           */
/* --------------------------------------------------------------------- */

static void dac_hw_init(audio_output_t *out)
{
    /* DAC output pin (GPIO23, analog fn) */
    GLB_GPIO_Cfg_Type gpio_cfg = {
        .gpioFun = GPIO_FUN_ANALOG,
        .pullType = GPIO_PULL_NONE,
        .drive = 1,
        .smtCtrl = 1,
        .gpioPin = GLB_GPIO_PIN_23,
        .gpioMode = GPIO_MODE_AF,
    };
    GLB_GPIO_Init(&gpio_cfg);

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    /* Request a DMA channel for DAC0 output. The allocator picks any
     * free CH0..CH7 and handles clock-ungating; no hardcoded channel
     * number, no shared-state bug with AUADC. */
    out->dma_handle = dma_request_channel(DMA_REQUEST_DAC0,
                                          DMA_MEMORY_TO_PERIPH,
                                          "audio_dac");
    if (!out->dma_handle) {
        printf("dac_out: DMA channel request failed\n");
        return;
    }
    dac_dma_dev.ch = (uint8_t)dma_handle_to_ch(out->dma_handle);
#else
    /* Legacy path: channel is the compile-time DAC_DMA_CH. Both
     * AUADC (AMIC/DMIC capture) and on-chip DAC historically shared
     * DMA0_CH0; hal_auadc_stop only calls dma_channel_stop and
     * leaves stale LLI/callback state that corrupts our first IRQ.
     * Close the slot cleanly before reconfiguring. */
    dma_close(dac_dma_dev.id, dac_dma_dev.ch);
#endif

    /* DMA - patched for mode */
    dac_dma_dev.transfer_mode =
        (out->cfg.mode == AUDIO_OUTPUT_MODE_ONESHOT) ? DMA_LLI_ONCE_MODE
                                                     : DMA_LLI_CYCLE_N_MODE;
    dac_dma_dev.lli_count =
        (out->cfg.mode == AUDIO_OUTPUT_MODE_ONESHOT) ? 0 : out->ring_count;

    dma_setConfig(&dac_dma_dev);
    dma_init(dac_dma_dev.id, dac_dma_dev.ch);
    dma_ioctl(dac_dma_dev.id, dac_dma_dev.ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(dac_dma_dev.id, dac_dma_dev.ch, DMA_CTRL_SET_CALLBACK,
              dac_dma_callback);

    /* DAC */
    dac_device_t dac0 = {
        .caps = COMMON_CAPS_DMA_TX,
        .channels = DAC_CHANNEL_0,
        .sample_freq = DAC_RATE,
        .vref = DAC_VREF_INTERNAL,
    };
    hal_dac_setConfig(&dac0);
    hal_dac_init(dac_dma_dev.id);
    hal_dac_control(dac_dma_dev.id, DAC_CTRL_TX_DMA,
                    (void *)(uintptr_t)dac_dma_dev.ch);

    /* Unmute speaker. The IPRO7 EVB drives the LM4871 audio amplifier
     * SHUTDOWN pin from this GPIO. Per LM4871 datasheet (TI SNAS002F),
     * SHUTDOWN is active HIGH:
     *   gpio_write(SPK, 0) = LOW  -> SHUTDOWN deasserted -> amp enabled
     *   gpio_write(SPK, 1) = HIGH -> SHUTDOWN asserted   -> amp muted
     * Note: LE Audio app_dac.c uses opposite polarity because that
     * board has a different amplifier (or external inverter) wired
     * here — do NOT cargo-cult the polarity from there. */
    gpio_set_mode(out->spk_gpio, GPIO_OUTPUT_PP_MODE);
    gpio_write(out->spk_gpio, 0);
}

int audio_output_dac_start(audio_output_t *out)
{
    if (!out || out->started) return -1;

    /* Stale filter state from a previous stream would click on the first
     * samples of this one. */
    audio_speaker_eq_reset();

    dac_hw_init(out);

    if (out->cfg.mode == AUDIO_OUTPUT_MODE_STREAMING) {
        /* Prime ring with silence and kick off cyclic DMA */
        uint32_t total_bytes = out->ring_count * out->ring_samples * sizeof(uint16_t);
        L1C_DCACHE_CLEAN_RANGE(out->dma_buf, total_bytes);

        uint32_t seg_bytes = out->ring_samples * sizeof(uint16_t);
        int ret = dma_reload(dac_dma_dev.id, dac_dma_dev.ch,
                             (uint32_t)(uintptr_t)out->dma_buf,
                             DMA_ADDR_DAC_TDR, seg_bytes, NULL);
        if (ret != 0) {
            printf("dac_out: dma_reload failed: %d\n", ret);
            return -1;
        }
        dma_channel_start(dac_dma_dev.id, dac_dma_dev.ch);
        out->running = 1;
        out->active_idx = 0;
        out->fill_idx = 0;
    }
    /* One-shot mode: DMA is set up in write(), not here */

    out->started = 1;
    return 0;
}

int audio_output_dac_stop(audio_output_t *out)
{
    if (!out || !out->started) return 0;

    /* Mute speaker (LM4871 SHUTDOWN active HIGH) */
    gpio_write(out->spk_gpio, 1);

    dma_channel_stop(dac_dma_dev.id, dac_dma_dev.ch);
    out->running = 0;

    hal_dac_deinit(dac_dma_dev.id);
    out->started = 0;

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    if (out->dma_handle) {
        dma_release_channel(out->dma_handle);
        out->dma_handle = NULL;
    }
#endif
    return 0;
}

/*
 * How many samples the DAC has actually consumed since the stream started.
 *
 * This is the only honest measure of the hardware's real sample rate. Timing
 * how fast a producer can push into audio_output_write() does not work: that
 * call blocks only when the ring is full, so a producer that is running slow
 * simply never blocks and ends up measuring itself. The DMA completion count
 * is independent of the producer entirely.
 */
uint32_t audio_output_dac_consumed(audio_output_t *out)
{
    if (!out || out->cfg.mode == AUDIO_OUTPUT_MODE_ONESHOT) {
        return 0;
    }
    return out->segments_done * out->ring_samples;
}

uint32_t audio_output_dac_segment_samples(audio_output_t *out)
{
    return out ? out->ring_samples : 0;
}

/* --------------------------------------------------------------------- */
/* Streaming-mode write                                                   */
/* --------------------------------------------------------------------- */

static void flush_segment(audio_output_t *out)
{
    /* Wait for DMA to release this ring slot */
    while (out->buf_ready[out->fill_idx]) {
        xSemaphoreTake(out->feed_sem, pdMS_TO_TICKS(100));
    }

    uint16_t *dst = get_segment(out, out->fill_idx);
    pcm_to_dac(out->leftover, dst, out->ring_samples);
    L1C_DCACHE_CLEAN_RANGE(dst, out->ring_samples * sizeof(uint16_t));
    out->buf_ready[out->fill_idx] = 1;
    out->fill_idx = (out->fill_idx + 1) % out->ring_count;
    out->leftover_count = 0;
}

static int write_streaming(audio_output_t *out,
                           const int16_t *src, uint32_t src_samples)
{
    uint32_t pos = 0;
    while (pos < src_samples) {
        uint32_t space = out->ring_samples - out->leftover_count;
        uint32_t avail = src_samples - pos;
        uint32_t n = (avail < space) ? avail : space;

        memcpy(&out->leftover[out->leftover_count], &src[pos],
               n * sizeof(int16_t));
        out->leftover_count += n;
        pos += n;

        if (out->leftover_count == out->ring_samples) {
            flush_segment(out);
        }
    }
    return 0;
}

/* --------------------------------------------------------------------- */
/* One-shot-mode write (blocking)                                         */
/* --------------------------------------------------------------------- */

static int write_oneshot(audio_output_t *out,
                         const int16_t *src, uint32_t src_samples)
{
    /* Re-allocate DAC buffer if too small. Cache-line aligned so the
     * cache clean before hal_dac_write() doesn't touch neighbours. */
    if (src_samples > out->oneshot_cap) {
        if (out->oneshot_buf) vPortFreeCacheAligned(out->oneshot_buf);
        out->oneshot_buf = pvPortMallocCacheAligned(src_samples * sizeof(uint16_t));
        if (!out->oneshot_buf) {
            out->oneshot_cap = 0;
            return -1;
        }
        out->oneshot_cap = src_samples;
    }

    pcm_to_dac(src, out->oneshot_buf, src_samples);
    L1C_DCACHE_CLEAN_RANGE(out->oneshot_buf, src_samples * sizeof(uint16_t));

    /* Drain any stale semaphore from previous write */
    xSemaphoreTake(out->done_sem, 0);

    int ret = hal_dac_write(dac_dma_dev.id, DAC_CHANNEL_0,
                            out->oneshot_buf,
                            src_samples * sizeof(uint16_t));
    if (ret != 0) {
        printf("dac_out: hal_dac_write failed: %d\n", ret);
        return -1;
    }

    /* Block with a generous timeout (2x playback + 2s) rather than
     * portMAX_DELAY so a stuck DMA surfaces as an error instead of
     * hanging the caller forever. */
    uint32_t play_ms = (uint32_t)((uint64_t)src_samples * 1000 / DAC_RATE);
    TickType_t to = pdMS_TO_TICKS(play_ms * 2 + 2000);
    if (xSemaphoreTake(out->done_sem, to) != pdTRUE) {
        printf("dac_out: oneshot TIMEOUT after %lu ms (expected %lu ms)\n",
               (unsigned long)(play_ms * 2 + 2000), (unsigned long)play_ms);
        return -1;
    }
    return 0;
}

/* --------------------------------------------------------------------- */
/* Public write (dispatches by mode)                                      */
/* --------------------------------------------------------------------- */

int audio_output_dac_write(audio_output_t *out,
                           const int16_t *pcm, uint32_t samples)
{
    if (!out || !out->started || samples == 0) return -1;

    const int16_t *src = pcm;
    uint32_t src_samples = samples;

    /* Stereo -> mono mixdown (single-channel DAC) */
    if (out->cfg.channels == 2) {
        uint32_t n = (samples > MAX_MONO_SAMPLES) ? MAX_MONO_SAMPLES : samples;
        for (uint32_t i = 0; i < n; i++) {
            out->mono_buf[i] = (int16_t)(((int32_t)pcm[i * 2] + pcm[i * 2 + 1]) >> 1);
        }
        src = out->mono_buf;
        src_samples = n;
    }

    /* Resample to DAC rate (scratch buffer is bounded;
     * one-shot long writes are resampled in-place below instead) */
    int16_t *rs_scratch = NULL;
    uint32_t rs_scratch_cap = 0;

    if (out->need_resample) {
        if (out->cfg.mode == AUDIO_OUTPUT_MODE_ONESHOT) {
            /* Allocate large scratch for the whole one-shot buffer */
            rs_scratch_cap =
                (uint32_t)((uint64_t)src_samples * DAC_RATE / out->cfg.sample_rate) + 2;
            rs_scratch = pvPortMalloc(rs_scratch_cap * sizeof(int16_t));
            if (!rs_scratch) return -1;
            src_samples = resample_linear(src, src_samples,
                                          rs_scratch, rs_scratch_cap,
                                          out->cfg.sample_rate, DAC_RATE);
            src = rs_scratch;
        } else {
            src_samples = resample_linear(src, src_samples,
                                          out->resample_buf,
                                          MAX_RESAMPLE_SAMPLES,
                                          out->cfg.sample_rate, DAC_RATE);
            src = out->resample_buf;
        }
    }

    int ret;
    if (out->cfg.mode == AUDIO_OUTPUT_MODE_ONESHOT) {
        ret = write_oneshot(out, src, src_samples);
    } else {
        ret = write_streaming(out, src, src_samples);
    }

    if (rs_scratch) vPortFree(rs_scratch);
    return ret;
}
