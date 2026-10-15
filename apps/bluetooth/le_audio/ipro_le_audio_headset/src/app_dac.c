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
 * @file app_dac.c
 * @brief DAC Output Module Implementation
 *
 * Hardware DAC output for LE Audio headset using DMA Triple Buffer mode.
 * Provides seamless triple-buffering for glitch-free audio playback with
 * improved tolerance for timing jitter.
 *
 * Architecture:
 *   Audio Source -> app_dac_feed() -> [Buffer 0/1/2] -> DMA -> DAC -> Speaker
 *
 * Triple Buffer Mode:
 *   - Three contiguous buffers (0, 1, 2)
 *   - DMA automatically cycles: 0 -> 1 -> 2 -> 0 -> ...
 *   - Callback fires when each segment completes
 *   - Application has 2 buffer periods to fill the next buffer
 *   - Significantly reduces underrun risk compared to ping-pong
 */

/*
 * Board wiring. Where the board header names the amplifier enable, it wins
 * over the EVB/EVK pin in app_dac.h; that has to happen before app_dac.h is
 * read.
 */
#include "board_config.h"
#ifdef BOARD_SPK_EN_PIN
#define DAC_GPIO_SPEAKER_EN         BOARD_SPK_EN_PIN
#define DAC_SPK_EN_ACTIVE           BOARD_SPK_EN_ACTIVE
#endif

#include "app_dac.h"
#include "dac_ns.h"
#include "audio_speaker_eq.h"
#include "audio_bass_boost.h"
#include "drv_clock.h"
#include "ipro_log.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <string.h>
#include <stdio.h>

#include <hal_dma.h>
#include <hal_dac.h>
#include <hal_gpio.h>
#include <drv_glb.h>
#include <drv_dac.h>
#include "system_soc.h"

#define TAG "DAC"

/*
 * Hardware Configuration
 */
#define DAC_GPIO_PIN        DAC_GPIO_OUTPUT       // From app_dac.h
#define DAC_GPIO_SPK        DAC_GPIO_SPEAKER_EN   // From app_dac.h
#define DAC_DMA_CH          DMA0_CH0_INDEX
#define DAC_CHANNEL         DAC_CHANNEL_0

/*
 * Triple Buffer Configuration
 * DAC_BUFFER_DURATION_MS, DAC_BUFFER_COUNT, DAC_SAMPLES_PER_SEGMENT defined in app_dac.h
 */
#define DAC_TOTAL_SAMPLES           (DAC_SAMPLES_PER_SEGMENT * DAC_BUFFER_COUNT)
/* DAC_SILENCE_VALUE defined in app_dac.h */

/* Gain: shift=4 (÷16) with soft limiter to avoid hard clipping */
#define DAC_GAIN_SHIFT       4

/*
 * Default speaker trim.
 *
 * The IPRO7AI Reef's PAM8302A takes DAC A through R52 10k for a gain of
 * 15-30 V/V, against 2 V/V for the EVK's TPA2037D1 that the volume curve was
 * set on, and it runs straight off USB VBUS - full scale into it clips and
 * can brown the board out. -12 dB is a starting point, not a measurement;
 * `dac spk <dB>` changes it at run time.
 */
#if defined(CONFIG_IPRO7AI_REEF)
#define DAC_SPK_TRIM_DEFAULT_DB     (-12)
#else
#define DAC_SPK_TRIM_DEFAULT_DB     0
#endif

/* round(65536 * 10^(-n/20)) for n = 0 .. -DAC_SPK_TRIM_MIN_DB */
static const uint32_t s_trim_q16[] = {
    65536, 58409, 52057, 46396, 41350, 36854, 32846, 29274,
    26090, 23253, 20724, 18471, 16462, 14672, 13076, 11654,
    10387, 9257, 8250, 7353, 6554, 5841, 5206, 4640,
    4135, 3685, 3285, 2927, 2609, 2325, 2072,
};
_Static_assert(sizeof(s_trim_q16) / sizeof(s_trim_q16[0]) == 1 - DAC_SPK_TRIM_MIN_DB,
               "one entry per dB from 0 to DAC_SPK_TRIM_MIN_DB");

#ifdef BOARD_HP_DET_PIN
/* Two equal reads, one per fed segment, before the jack state is believed. */
#define DAC_JACK_DEBOUNCE           2
#endif

/*
 * Output path.
 *
 * Order 0 is the original path: the DMA plays the segment buffers as 32 kHz
 * DAC codes. Orders 1 and 2 run the DAC at 512 kHz instead: the segment
 * buffers hold 32 kHz samples, and dac_os_task() expands them through
 * dac_ns (interpolation + noise shaping of that order) into a small DMA ring.
 * That moves the 10-bit quantisation hiss out of the audio band. The Reef
 * starts in order 1; `dac os <0|1|2>` switches at run time.
 */
#if defined(CONFIG_IPRO7AI_REEF)
#define DAC_OS_DEFAULT_ORDER        1
#else
#define DAC_OS_DEFAULT_ORDER        0
#endif

#define OS_IN_PER_SEG               512                          /* 32 kHz samples per ring segment */
#define OS_SEG_SAMPLES              (OS_IN_PER_SEG * DAC_NS_OSR) /* 8192 codes = 16 ms at 512 kHz */
#define OS_TASK_STACK_WORDS         512

/*
 * Below the BLE host threads (ipro_hci_rx, bt_work: priority 6). At
 * configMAX_PRIORITIES - 2 the expander's bursts made the stream lose 4-13%
 * of its ISO packets on the bench, and none at 5: the output can ride out a
 * late refill, the HCI receive path cannot. The ring gives it 48 ms of slack
 * (three 16 ms segments) against everything above it - including console
 * output, which holds the CPU for about 10 ms a line at 115200 baud.
 */
#define OS_TASK_PRIORITY            5

_Static_assert(DAC_GAIN_SHIFT == DAC_NS_IN_SHIFT, "dac_ns and the 32 kHz path must share a scale");
_Static_assert(DAC_SAMPLE_RATE_HZ * DAC_NS_OSR == 512000, "the oversampled DAC rate is 512 kHz");
_Static_assert(DAC_SAMPLES_PER_SEGMENT % OS_IN_PER_SEG == 0, "segment boundaries must fall on ring boundaries");

/*
 * The 512 kHz DMA ring. It reuses the segment count of the 32 kHz path, so
 * the DMA channel's LLI layout (lli_count, fixed at dma_setConfig) is the
 * same in both modes and only the source address and size change.
 *
 * In OCRAM, not with the rest of .bss in PSRAM: the DMA reads it 512 000
 * times a second, and PSRAM is shared with code and data the BLE controller
 * fetches through the cache. 4 x 16 KB.
 */
static uint16_t s_os_ring[DAC_BUFFER_COUNT * OS_SEG_SAMPLES]
    __attribute__((section(".ocram_bss"), aligned(32)));

static void dac_os_task(void *arg);

/*
 * Private Data
 */
static struct {
    // Configuration
    app_dac_config_t config;

    // Software volume control (linear gain)
    // Range: 0.0 (mute) to 2.0 (6dB boost)
    // Note: With FIR input headroom of 0.7, gain up to ~1.43 won't clip
    /*
     * Volume as Q16 (65536 == unity).
     *
     * Was a float, multiplied per sample in convert_samples(). The IPRO7
     * FreeRTOS port does not save f0-f31 across a context switch, so 32000
     * float multiplies a second in the DAC path raced the LC3 decoder's float
     * work in another task - the kind of fault that shows up as occasional
     * wrong samples, i.e. sporadic clicks, and is very hard to chase.
     *
     * The public API still takes and returns float; the conversion happens in
     * the setters, which run on volume changes rather than per sample.
     */
    int32_t volume_gain_q16;

    // Speaker routing: amplifier state, trim, and the debounced jack state
    volatile bool amp_on;
    int8_t  spk_trim_db;
    int32_t spk_trim_q16;
#ifdef BOARD_HP_DET_PIN
    bool    hp_in;
    uint8_t hp_disagree;
#endif

    // Triple DMA buffer (contiguous, cache-aligned)
    // [0 .. DAC_SAMPLES_PER_SEGMENT-1] = Buffer 0
    // [DAC_SAMPLES_PER_SEGMENT .. 2*DAC_SAMPLES_PER_SEGMENT-1] = Buffer 1
    // [2*DAC_SAMPLES_PER_SEGMENT .. 3*DAC_SAMPLES_PER_SEGMENT-1] = Buffer 2
    uint16_t buffer[DAC_TOTAL_SAMPLES] __attribute__((aligned(32)));

    // Triple buffer state
    volatile uint8_t active_idx;        // Currently playing: 0, 1, or 2
    volatile uint8_t next_fill_idx;     // Next buffer to fill (for pre-fill before start)
    volatile bool buffer_ready[DAC_BUFFER_COUNT]; // True if buffer has valid data
    volatile bool running;              // DMA is running

    // Synchronization
    SemaphoreHandle_t feed_sem;         // Signaled when buffer needs filling

    // 512 kHz output path
    uint8_t  os_req;                    // order asked for (`dac os`), applied while stopped
    uint8_t  os_order;                  // order in effect
    bool     os;                        // buffers hold 32 kHz samples for dac_ns, DAC at 512 kHz
    uint32_t rd_pos;                    // expander's read position in buffer[active_idx]
    volatile uint32_t os_played;        // ring segments the DMA has finished (ISR)
    uint32_t os_filled;                 // ring segments refilled since start; the next is
                                        // os_filled % DAC_BUFFER_COUNT
    dac_ns_t ns;
    SemaphoreHandle_t os_sem;           // wakes the expander when a ring segment finishes
    SemaphoreHandle_t os_lock;          // expander vs. start/stop

    // Statistics
    app_dac_stats_t stats;

    // State
    bool initialized;
} s_dac = {0};

/*
 * LFSR state for TPDF dithering
 * Dithering converts quantization distortion into benign white noise,
 * significantly improving perceived audio quality when reducing bit depth.
 */
static uint32_t s_dither_lfsr = 0xACE1u;

/*
 * Generate TPDF (Triangular Probability Density Function) dither value
 *
 * TPDF dithering adds noise with triangular distribution, which:
 * - Completely eliminates quantization distortion (not just masks it)
 * - Produces uncorrelated noise floor (no harmonic artifacts)
 * - Is the standard for professional audio bit-depth reduction
 *
 * Returns: dither in int16 units, +-1 DAC LSB peak (each uniform spans one
 * LSB = 1 << DAC_GAIN_SHIFT). It used to be +-32, which was +-1 LSB when the
 * shift was 5; after the shift went to 4 it was twice the TPDF width and
 * added 4.8 dB of hiss.
 */
#define DITHER_SPAN                 (1 << DAC_GAIN_SHIFT)

static inline int32_t get_tpdf_dither(void)
{
    // LFSR for fast pseudo-random generation (Galois LFSR)
    s_dither_lfsr ^= s_dither_lfsr >> 7;
    s_dither_lfsr ^= s_dither_lfsr << 9;
    s_dither_lfsr ^= s_dither_lfsr >> 13;
    int32_t r1 = (int32_t)(s_dither_lfsr & (DITHER_SPAN - 1)) - DITHER_SPAN / 2;

    s_dither_lfsr ^= s_dither_lfsr >> 7;
    s_dither_lfsr ^= s_dither_lfsr << 9;
    s_dither_lfsr ^= s_dither_lfsr >> 13;
    int32_t r2 = (int32_t)(s_dither_lfsr & (DITHER_SPAN - 1)) - DITHER_SPAN / 2;

    // Sum of two uniform distributions = triangular distribution
    return r1 + r2;
}

/*
 * Fill buffer with silence: the DAC centre code on the 32 kHz path, sample
 * value 0 when the buffers feed dac_ns.
 */
static void fill_silence(uint16_t *buf, uint32_t count)
{
    const uint16_t v = s_dac.os ? 0 : DAC_SILENCE_VALUE;

    for (uint32_t i = 0; i < count; i++) {
        buf[i] = v;
    }
}

/*
 * Get pointer to specified buffer segment
 */
static inline uint16_t* get_buffer_segment(uint8_t idx)
{
    return &s_dac.buffer[idx * DAC_SAMPLES_PER_SEGMENT];
}

/*
 * Speaker amplifier: on while DMA runs and, on a board with a jack detect,
 * no plug is in. Start, stop and the jack poll can run in different tasks,
 * so the decision and the pin write happen together.
 */
static void amp_apply(void)
{
    taskENTER_CRITICAL();
    bool on = s_dac.running;
#ifdef BOARD_HP_DET_PIN
    on = on && !s_dac.hp_in;
#endif
    gpio_write(DAC_GPIO_SPK, on ? DAC_SPK_EN_ACTIVE : !DAC_SPK_EN_ACTIVE);
    s_dac.amp_on = on;
    taskEXIT_CRITICAL();
}

#ifdef BOARD_HP_DET_PIN
static bool jack_plugged_now(void)
{
    return gpio_read(BOARD_HP_DET_PIN) == BOARD_HP_DET_ACTIVE;
}

/* Once per fed segment, i.e. every DAC_BUFFER_DURATION_MS while playing. */
static void jack_poll(void)
{
    bool in = jack_plugged_now();

    if (in == s_dac.hp_in) {
        s_dac.hp_disagree = 0;
        return;
    }
    if (++s_dac.hp_disagree < DAC_JACK_DEBOUNCE) {
        return;
    }
    s_dac.hp_disagree = 0;
    s_dac.hp_in = in;
    amp_apply();
    IPRO_LOGI(TAG, "Headphone %s, speaker %s", in ? "plugged in" : "removed",
              s_dac.amp_on ? "on" : "off");
}
#endif

/*
 * One segment has been played out: advance active_idx, release the segment to
 * the feeder and keep the underrun statistics. Runs in the DMA ISR on the
 * 32 kHz path and in dac_os_task() (interrupts masked) on the 512 kHz path.
 * Returns the consecutive-underrun count, 0 when the next segment was ready.
 */
static uint32_t segment_complete(uint32_t now_ms, uint8_t *ready_out)
{
    // Previous segment completed, DMA now playing the next segment
    uint8_t completed_idx = s_dac.active_idx;
    s_dac.active_idx = (s_dac.active_idx + 1) % DAC_BUFFER_COUNT;

    // Mark completed buffer as consumed (needs refill)
    s_dac.buffer_ready[completed_idx] = false;

    // Count how many buffers are ready (excluding the one being played)
    uint8_t ready_count = 0;
    for (int i = 0; i < DAC_BUFFER_COUNT; i++) {
        if (s_dac.buffer_ready[i]) ready_count++;
    }
    *ready_out = ready_count;

    s_dac.stats.buffers_sent++;

    // Check if next buffer has valid data
    // NOTE: Do NOT fill silence in ISR - buffer should already be pre-filled
    // with silence during init. Just track the underrun for statistics.
    if (!s_dac.buffer_ready[s_dac.active_idx]) {
        // Buffer underrun - the 32 kHz DMA plays whatever is in the buffer;
        // the 512 kHz expander plays silence
        s_dac.stats.underruns++;
        s_dac.stats.consecutive_underruns++;
        s_dac.stats.buffers_ready_at_underrun = ready_count;

        // Track max consecutive underruns
        if (s_dac.stats.consecutive_underruns > s_dac.stats.max_consecutive_underruns) {
            s_dac.stats.max_consecutive_underruns = s_dac.stats.consecutive_underruns;
        }
        s_dac.stats.last_underrun_time_ms = now_ms;
        return s_dac.stats.consecutive_underruns;
    }

    // Buffer was ready - reset consecutive counter
    s_dac.stats.consecutive_underruns = 0;

    // Track minimum buffers ready (when not underrunning)
    if (s_dac.stats.min_buffers_ready == 0 || ready_count < s_dac.stats.min_buffers_ready) {
        s_dac.stats.min_buffers_ready = ready_count;
    }
    return 0;
}

/* printf, not IPRO_LOG*: this can run in the ISR, where the log mutex would hang. */
static void log_underrun(uint32_t consecutive, uint8_t ready_count, uint32_t now_ms)
{
    // Log when underrun starts (first in sequence)
    if (consecutive == 1) {
        printf("[E][DAC] UNDERRUN START @%lums: ready=%d/%d, total=%lu\n",
               (unsigned long)now_ms, ready_count, DAC_BUFFER_COUNT,
               (unsigned long)s_dac.stats.underruns);
    }

    // Log every 10 consecutive underruns to track ongoing issues
    if (consecutive != 0 && consecutive % 10 == 0) {
        printf("[E][DAC] UNDERRUN x%lu @%lums\n",
               (unsigned long)consecutive, (unsigned long)now_ms);
    }
}

/*
 * DMA Callback - Triple Buffer Mode (ISR Context)
 *
 * Called three times per cycle:
 *   1. Buffer 0 complete -> active_idx switches to 1, signal to fill 0
 *   2. Buffer 1 complete -> active_idx switches to 2, signal to fill 1
 *   3. Buffer 2 complete -> active_idx switches to 0, signal to fill 2
 *
 * WARNING: This runs in ISR context. DO NOT use IPRO_LOG* here as it uses
 * mutex (xSemaphoreTake) which causes deadlock/hang in ISR context.
 * Use printf directly for minimal ISR-safe logging (if UART is polled).
 */
static void dac_dma_callback(void *dev, uint32_t event)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    (void)dev;  // Unused parameter

    if (event == HAL_DMA_INT_COMPLETE) {
        if (s_dac.os) {
            // 512 kHz ring: count the finished segment and wake the expander.
            // The count, not the semaphore, says what to refill, so a
            // saturated semaphore loses nothing.
            s_dac.os_played++;
            (void)xSemaphoreGiveFromISR(s_dac.os_sem, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            return;
        }

        uint32_t now_ms = xTaskGetTickCountFromISR() * portTICK_PERIOD_MS;
        uint8_t ready_count;
        uint32_t consecutive = segment_complete(now_ms, &ready_count);
        log_underrun(consecutive, ready_count, now_ms);

        // Signal application to fill the completed buffer
        xSemaphoreGiveFromISR(s_dac.feed_sem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else if (event == HAL_DMA_INT_ERR) {
        s_dac.stats.errors++;
        // printf("[E][DAC] DMA error event=0x%lx\n", (unsigned long)event);
    } else {
        // printf("[W][DAC] DMA unknown event=0x%lx\n", (unsigned long)event);
    }
}

/*
 * DMA Device Configuration - N-Buffer Circular Mode
 *
 * Key features:
 *   - transfer_mode = DMA_LLI_CYCLE_N_MODE with lli_count = DAC_BUFFER_COUNT
 *   - DMA creates N LLI chains (configurable via DAC_BUFFER_COUNT)
 *   - Interrupt fires at each 1/N boundary
 *   - Last LLI links back to first (circular)
 *   - N=2: equivalent to pingpong, N=3: triple buffer, etc.
 */
static dma_device_t dac_dma_dev = {
    .id = DMA0_ID,
    .ch = DAC_DMA_CH,
    .transfer_mode = DMA_LLI_CYCLE_N_MODE,      // N-buffer circular mode
    .direction = DMA_MEMORY_TO_PERIPH,
    .src_req = DMA_REQUEST_NONE,
    .dst_req = DMA_REQUEST_DAC0,
    .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,  // Memory address increments
    .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE, // DAC FIFO fixed address
    .src_burst_size = DMA_BURST_INCR8,
    .dst_burst_size = DMA_BURST_INCR1,
    .src_width = DMA_TRANSFER_WIDTH_16BIT,
    .dst_width = DMA_TRANSFER_WIDTH_16BIT,
    .lli_count = DAC_BUFFER_COUNT,              // Number of buffers (from app_dac.h)
    .lli_cfg = NULL,
    .callback = dac_dma_callback,
};

/*
 * Convert sample rate to DAC enum
 */
static uint32_t get_dac_sample_rate(uint32_t sample_rate)
{
    // DAC hardware only supports: 8kHz, 16kHz, 32kHz, 512kHz
    switch (sample_rate) {
        case 8000:  return DAC_SAMPLE_FREQ_8KHZ;
        case 16000: return DAC_SAMPLE_FREQ_16KHZ;
        case 32000: return DAC_SAMPLE_FREQ_32KHZ;
        case 512000: return DAC_SAMPLE_FREQ_512KHZ;
        default:
            // Default to 32kHz for unsupported rates
            return DAC_SAMPLE_FREQ_32KHZ;
    }
}

/* (Re)initialise the GPDAC for the current path: 512 kHz when dac_ns feeds it. */
static void dac_hw_init(void)
{
    dac_device_t dac_cfg = {
        .caps = COMMON_CAPS_DMA_TX,
        .channels = DAC_CHANNEL,
        .sample_freq = s_dac.os ? DAC_SAMPLE_FREQ_512KHZ
                                : get_dac_sample_rate(s_dac.config.sample_rate),
        .vref = DAC_VREF_INTERNAL
    };

    hal_dac_setConfig(&dac_cfg);
    hal_dac_init(dac_dma_dev.id);
    hal_dac_control(dac_dma_dev.id, DAC_CTRL_TX_DMA, (void *)(uintptr_t)dac_dma_dev.ch);
}

/*
 * Apply a pending `dac os` request. Only while stopped: the segment buffers
 * change format with the path, so they are flushed, and the GPDAC is
 * re-initialised when its rate changes. dac_ns needs exactly 32 kHz input;
 * any other stream rate stays on the direct path.
 */
static void dac_mode_apply(void)
{
    s_dac.os_order = s_dac.os_req;

    const bool os = s_dac.os_order != 0 && s_dac.config.sample_rate == DAC_SAMPLE_RATE_HZ;
    if (os == s_dac.os) {
        return;
    }
    s_dac.os = os;

    for (int i = 0; i < DAC_BUFFER_COUNT; i++) {
        fill_silence(get_buffer_segment(i), DAC_SAMPLES_PER_SEGMENT);
        s_dac.buffer_ready[i] = false;
    }
    L1C_DCACHE_CLEAN_RANGE(s_dac.buffer, DAC_TOTAL_SAMPLES * sizeof(uint16_t));
    s_dac.next_fill_idx = 0;

    if (s_dac.initialized) {
        dac_hw_init();
    }
    IPRO_LOGI(TAG, "Output path: %s", os ? "512 kHz, noise shaped" : "32 kHz direct");
}

/*
 * Public API Implementation - Ping-Pong Mode
 */
int app_dac_init(const app_dac_config_t* config)
{
    IPRO_LOGI(TAG, "Initializing DAC output (Triple Buffer mode, %d buffers)", DAC_BUFFER_COUNT);

    if (s_dac.initialized) {
        IPRO_LOGW(TAG, "Already initialized");
        return 0;
    }

    // Set configuration
    if (config) {
        memcpy(&s_dac.config, config, sizeof(app_dac_config_t));
    } else {
        s_dac.config.sample_rate = 32000;
        s_dac.config.channels = 1;
        s_dac.config.volume = 100;
        s_dac.config.gain_2x = false;  // Default: no 2x gain (prevents clipping)
    }

    // Initialize software volume gain (1.0 = unity gain, no change)
    s_dac.volume_gain_q16 = 65536;
    app_dac_set_spk_trim_db(DAC_SPK_TRIM_DEFAULT_DB);

#if defined(CONFIG_IPRO7AI_REEF)
    /* The speaker EQ was fitted to the EVK's speaker. On the Reef the DAC
     * drives a flat headphone buffer, and J_SPK takes whatever speaker is
     * plugged in, so the correction would only colour the sound. `eq on`
     * still turns it back on. */
    audio_speaker_eq_enable(false);
#endif

    // Create semaphore for buffer-ready signaling
    if (s_dac.feed_sem == NULL) {
        s_dac.feed_sem = xSemaphoreCreateBinary();
        if (!s_dac.feed_sem) {
            IPRO_LOGE(TAG, "Failed to create feed semaphore");
            return -1;
        }
    }

    // 512 kHz path: ring-refill semaphore, lock and the expander task
    if (s_dac.os_sem == NULL) {
        s_dac.os_sem = xSemaphoreCreateCounting(DAC_BUFFER_COUNT, 0);
        s_dac.os_lock = xSemaphoreCreateMutex();
        if (!s_dac.os_sem || !s_dac.os_lock ||
            xTaskCreate(dac_os_task, "dac_os", OS_TASK_STACK_WORDS, NULL,
                        OS_TASK_PRIORITY, NULL) != pdPASS) {
            IPRO_LOGE(TAG, "Failed to create the 512 kHz expander");
            return -1;
        }
    }
    s_dac.os_req = s_dac.os_order = DAC_OS_DEFAULT_ORDER;
    s_dac.os = s_dac.os_order != 0 && s_dac.config.sample_rate == DAC_SAMPLE_RATE_HZ;

    // Initialize triple buffer state
    s_dac.active_idx = 0;
    s_dac.next_fill_idx = 0;    // Start filling from buffer 0
    for (int i = 0; i < DAC_BUFFER_COUNT; i++) {
        s_dac.buffer_ready[i] = false;
    }
    s_dac.running = false;

    // Pre-fill all buffers with silence and clean cache
    // This ensures DMA will play silence on underrun (not garbage)
    for (int i = 0; i < DAC_BUFFER_COUNT; i++) {
        fill_silence(get_buffer_segment(i), DAC_SAMPLES_PER_SEGMENT);
    }
    L1C_DCACHE_CLEAN_RANGE(s_dac.buffer, DAC_TOTAL_SAMPLES * sizeof(uint16_t));

    // DAC GPIO Init
    GLB_GPIO_Cfg_Type gpio_cfg;
    gpio_cfg.gpioFun = GPIO_FUN_ANALOG;
    gpio_cfg.pullType = GPIO_PULL_NONE;
    gpio_cfg.drive = 1;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioPin = GLB_GPIO_PIN_23;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    GLB_GPIO_Init(&gpio_cfg);

    // Configure DMA for Ping-Pong mode
    dma_setConfig(&dac_dma_dev);
    dma_init(dac_dma_dev.id, dac_dma_dev.ch);

    // Enable DMA interrupt
    dma_ioctl(dac_dma_dev.id, dac_dma_dev.ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(dac_dma_dev.id, dac_dma_dev.ch, DMA_CTRL_SET_CALLBACK, dac_dma_callback);

    // Initialize DAC
    dac_hw_init();

    // Start with the amplifier off until playback begins
    gpio_set_mode(DAC_GPIO_SPK, GPIO_OUTPUT_PP_MODE);
    amp_apply();

#ifdef BOARD_HP_DET_PIN
    /* Same set-up as the Reef demo's `hp`: input with the pull-up, which
     * reads HIGH with no plug (against R51 1M) and LOW with an earpiece on
     * the line. */
    gpio_set_mode(BOARD_HP_DET_PIN, GPIO_INPUT_PP_MODE);
    s_dac.hp_in = jack_plugged_now();
    s_dac.hp_disagree = 0;
    IPRO_LOGI(TAG, "Headphone jack (GPIO%d): %s", BOARD_HP_DET_PIN,
              s_dac.hp_in ? "plug in" : "empty");
#endif

    // Reset statistics
    memset(&s_dac.stats, 0, sizeof(s_dac.stats));

    s_dac.initialized = true;

    IPRO_LOGI(TAG, "DAC initialized: %lu Hz, %d buffers x %u samples, gain_shift=%d, knee=%d",
              (unsigned long)s_dac.config.sample_rate, DAC_BUFFER_COUNT, DAC_SAMPLES_PER_SEGMENT,
              DAC_GAIN_SHIFT, DAC_KNEE);
    IPRO_LOGI(TAG, "Output path: %s (order %u)",
              s_dac.os ? "512 kHz, noise shaped" : "32 kHz direct", s_dac.os_order);

    return 0;
}

void app_dac_deinit(void)
{
    if (!s_dac.initialized) return;

    IPRO_LOGI(TAG, "Deinitializing DAC");

    // Stop DMA if running
    if (s_dac.running) {
        dma_channel_stop(dac_dma_dev.id, dac_dma_dev.ch);
        s_dac.running = false;
    }

    // Amplifier off
    amp_apply();

    // Delete semaphore
    if (s_dac.feed_sem) {
        vSemaphoreDelete(s_dac.feed_sem);
        s_dac.feed_sem = NULL;
    }

    s_dac.initialized = false;
}

int app_dac_reconfigure(const app_dac_config_t* config)
{
    if (!s_dac.initialized || !config) return -1;
    
    IPRO_LOGI(TAG, "Reconfiguring DAC: %lu Hz", (unsigned long)config->sample_rate);
    
    // Update configuration
    memcpy(&s_dac.config, config, sizeof(app_dac_config_t));

    // Reconfigure DAC. A new stream rate can take the stream on or off the
    // 512 kHz path; while playing that waits for the next stop.
    if (!s_dac.running && s_dac.os != (s_dac.os_req != 0 &&
                                       config->sample_rate == DAC_SAMPLE_RATE_HZ)) {
        dac_mode_apply();
        return 0;
    }

    dac_device_t dac_cfg = {
        .caps = COMMON_CAPS_DMA_TX,
        .channels = DAC_CHANNEL,
        .sample_freq = s_dac.os ? DAC_SAMPLE_FREQ_512KHZ : get_dac_sample_rate(config->sample_rate),
        .vref = DAC_VREF_INTERNAL
    };

    hal_dac_setConfig(&dac_cfg);

    return 0;
}

/*
 * Convert int16_t PCM to DAC format (10-bit, 0-1023) with TPDF dithering
 *
 * Dithering is applied before quantization to convert harsh quantization
 * distortion into benign white noise. This is critical when reducing from
 * 16-bit to 10-bit (losing 6 bits = 64 quantization levels).
 *
 * Gain: shift=4 (÷16) with soft limiter to avoid hard clipping.
 * Soft knee at 5120: linear below, 4:1 compress above.
 */
/*
 * Cost accounting for the per-sample chain.
 *
 * Enabling the bass enhancement starved the DAC ring and dropped ISO packets,
 * and guessing at instruction counts from a disassembly is not evidence about
 * a real system with a BLE controller taking interrupts. Two CSR reads per
 * buffer of 320 samples is free; `dacprof` reports what it actually costs.
 */
static uint64_t s_prof_cycles;
static uint32_t s_prof_samples;

static inline uint32_t dac_mcycle(void)
{
    uint32_t v;
    __asm__ volatile("csrr %0, mcycle" : "=r"(v));
    return v;
}

void app_dac_profile_report(void);
void app_dac_profile_reset(void);

static void convert_samples(const int16_t* src, uint16_t* dst, uint32_t count)
{
    uint32_t t0 = dac_mcycle();
    const int shift = DAC_GAIN_SHIFT;
    const bool os = s_dac.os;
    /* The trim follows the amplifier, so a headphone keeps the full level. */
    const int32_t gain_q16 = s_dac.amp_on
        ? (int32_t)(((int64_t)s_dac.volume_gain_q16 * s_dac.spk_trim_q16) >> 16)
        : s_dac.volume_gain_q16;

    // Debug: log once per session
    static bool logged = false;
    if (!logged && count > 0) {
        logged = true;
        int16_t min_in = src[0], max_in = src[0];
        for (uint32_t i = 1; i < count && i < 100; i++) {
            if (src[i] < min_in) min_in = src[i];
            if (src[i] > max_in) max_in = src[i];
        }
        IPRO_LOGI(TAG, "convert_samples: gain=%d.%02d, shift=%d, knee=%d, "
                       "input[0..99] range=%d~%d",
                  (int)(gain_q16 >> 16), (int)(((gain_q16 & 0xFFFF) * 100) >> 16),
                  shift, DAC_KNEE, min_in, max_in);
    }

    static uint32_t dac_clip_count = 0;
    uint32_t frame_clips = 0;

#if DAC_SAMPLE_RATE_HZ != AUDIO_SPEAKER_EQ_RATE
#error "speaker EQ coefficients are designed for DAC_SAMPLE_RATE_HZ"
#endif

    for (uint32_t i = 0; i < count; i++) {
        /*
         * Speaker correction runs before the volume gain and before the soft
         * knee, so the filter always sees the same undistorted signal
         * regardless of where the phone has set the volume. Putting it after
         * the knee would make the correction depend on level.
         */
        /*
         * Order matters. Bass enhancement is a content transform and runs
         * first; the speaker EQ is a correction for the transducer and must
         * be the last thing that shapes the signal before it is scaled.
         */
        int32_t eq = audio_speaker_eq_sample(
                         audio_bass_boost_sample((int32_t)src[i]));

        // Apply software volume gain first (before dither)
        int32_t sample = (int32_t)(((int64_t)eq * gain_q16) >> 16);

        // Clamp to int16 range after gain (track pre-DAC clipping)
        if (sample > 32767) {
            sample = 32767;
            frame_clips++;
        } else if (sample < -32768) {
            sample = -32768;
            frame_clips++;
        }

        // Soft limiter: linear below knee, 4:1 compress above
        if (sample > DAC_KNEE)
            sample = DAC_KNEE + ((sample - DAC_KNEE) >> 2);
        else if (sample < -DAC_KNEE)
            sample = -DAC_KNEE + ((sample + DAC_KNEE) >> 2);

        // 512 kHz path: dac_ns quantises later; keep the full-resolution
        // sample, clipped to the shaper's headroom
        if (os) {
            if (sample > DAC_NS_TARGET_MAX) {
                sample = DAC_NS_TARGET_MAX;
            } else if (sample < -DAC_NS_TARGET_MAX) {
                sample = -DAC_NS_TARGET_MAX;
            }
            dst[i] = (uint16_t)(int16_t)sample;
            continue;
        }

        // Apply TPDF dither only to non-silent samples
        // Noise gate: skip dither when signal is near silence
        if (sample > 64 || sample < -64) {
            sample += get_tpdf_dither();
        }

        // Scale to DAC range (0~1023): dither + round + shift
        sample = ((sample + (1 << (shift - 1))) >> shift) + 512;

        // Clamp to DAC range (should rarely happen after int16 clamp)
        if (sample < 0) {
            sample = 0;
        } else if (sample > 1023) {
            sample = 1023;
        }

        dst[i] = (uint16_t)sample;
    }

    /* 32-bit mcycle wraps about every 22 s at this clock; the subtraction is
     * still correct across one wrap, which is all a buffer ever spans. */
    s_prof_cycles  += (uint32_t)(dac_mcycle() - t0);
    s_prof_samples += count;

    // Log clipping (every 100 clips to avoid log spam)
    if (frame_clips > 0) {
        dac_clip_count += frame_clips;
        if (dac_clip_count <= 10 || dac_clip_count % 100 == 0) {
            printf("[W][DAC] CLIP: %lu samples (gain=%d.%02d), total=%lu\n",
                   (unsigned long)frame_clips,
                   (int)(gain_q16 >> 16), (int)(((gain_q16 & 0xFFFF) * 100) >> 16),
                   (unsigned long)dac_clip_count);
        }
    }
}

/*
 * 512 kHz path. A segment buffer holds DAC_SAMPLES_PER_SEGMENT samples, a
 * whole number of OS_IN_PER_SEG blocks, so a block never straddles two.
 * Crossing a segment boundary does what the DMA ISR does on the 32 kHz path;
 * a segment that is not ready plays as silence.
 */
static uint64_t s_os_cycles;
static uint32_t s_os_samples;
static const int16_t s_os_silence[OS_IN_PER_SEG];

static void os_consume_block(void)
{
    s_dac.rd_pos += OS_IN_PER_SEG;
    if (s_dac.rd_pos == DAC_SAMPLES_PER_SEGMENT) {
        uint32_t now_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
        uint8_t ready_count;
        uint32_t consecutive;

        s_dac.rd_pos = 0;
        taskENTER_CRITICAL();
        consecutive = segment_complete(now_ms, &ready_count);
        taskEXIT_CRITICAL();
        log_underrun(consecutive, ready_count, now_ms);
        xSemaphoreGive(s_dac.feed_sem);
    }
}

static void os_fill_segment(uint16_t *dst)
{
    const uint32_t t0 = dac_mcycle();
    const uint8_t idx = s_dac.active_idx;
    const int16_t *src = s_dac.buffer_ready[idx]
        ? (const int16_t *)&s_dac.buffer[idx * DAC_SAMPLES_PER_SEGMENT + s_dac.rd_pos]
        : s_os_silence;

    dac_ns_run(&s_dac.ns, src, OS_IN_PER_SEG, dst);
    L1C_DCACHE_CLEAN_RANGE(dst, OS_SEG_SAMPLES * sizeof(uint16_t));
    os_consume_block();

    s_os_cycles  += (uint32_t)(dac_mcycle() - t0);
    s_os_samples += OS_SEG_SAMPLES;
}

/*
 * Refill j (ring segment j % N) is due once the DMA has finished it
 * (os_played > j) and must land before the DMA comes back to it
 * (os_played < j + N). Falling further behind than that means the DMA is
 * already replaying stale codes: skip forward to the segment after the one
 * playing, and drop the input that would have filled the skipped ones so
 * the stream stays in time.
 */
static void os_service_ring(void)
{
    for (;;) {
        const uint32_t behind = s_dac.os_played - s_dac.os_filled;

        if (behind == 0u) {
            return;
        }
        if (behind >= DAC_BUFFER_COUNT) {
            const uint32_t skip = behind - (DAC_BUFFER_COUNT - 1u);

            for (uint32_t k = 0; k < skip; k++) {
                os_consume_block();
            }
            s_dac.os_filled += skip;
            s_dac.stats.os_late += skip;
            continue;
        }
        os_fill_segment(&s_os_ring[(s_dac.os_filled % DAC_BUFFER_COUNT) * OS_SEG_SAMPLES]);
        s_dac.os_filled++;
    }
}

static void dac_os_task(void *arg)
{
    (void)arg;

    for (;;) {
        xSemaphoreTake(s_dac.os_sem, portMAX_DELAY);
        xSemaphoreTake(s_dac.os_lock, portMAX_DELAY);
        if (s_dac.running && s_dac.os) {
            os_service_ring();
        }
        xSemaphoreGive(s_dac.os_lock);
    }
}

/* 512 kHz path: prime the whole ring from the segment buffers and start the DMA. */
static int os_start(void)
{
    int ret;

    xSemaphoreTake(s_dac.os_lock, portMAX_DELAY);
    while (xSemaphoreTake(s_dac.os_sem, 0) == pdTRUE) {
    }
    s_dac.active_idx = 0;
    s_dac.rd_pos = 0;
    s_dac.os_played = 0;
    s_dac.os_filled = 0;
    dac_ns_reset(&s_dac.ns, s_dac.os_order);
    for (int i = 0; i < DAC_BUFFER_COUNT; i++) {
        os_fill_segment(&s_os_ring[i * OS_SEG_SAMPLES]);
    }

    ret = dma_reload(dac_dma_dev.id, dac_dma_dev.ch, (uint32_t)s_os_ring, DMA_ADDR_DAC_TDR,
                     OS_SEG_SAMPLES * sizeof(uint16_t), NULL);
    if (ret == 0) {
        dma_channel_start(dac_dma_dev.id, dac_dma_dev.ch);
        s_dac.running = true;
    }
    xSemaphoreGive(s_dac.os_lock);
    return ret;
}

/**
 * @brief Start DMA playback (call after first feed)
 *
 * Starts the DMA ping-pong cycle. Must call app_dac_feed() first
 * to fill at least one buffer.
 */
int app_dac_start(void)
{
    /* Stale filter state from the previous stream would click. */
    audio_speaker_eq_reset();
    audio_bass_boost_reset();

    if (!s_dac.initialized) {
        IPRO_LOGE(TAG, "Not initialized");
        return -1;
    }

    if (s_dac.running) {
        IPRO_LOGW(TAG, "Already running");
        return 0;
    }

    if (s_dac.os) {
        if (os_start() != 0) {
            IPRO_LOGE(TAG, "512 kHz start failed");
            return -1;
        }
#ifdef BOARD_HP_DET_PIN
        s_dac.hp_in = jack_plugged_now();
        s_dac.hp_disagree = 0;
#endif
        amp_apply();
        IPRO_LOGI(TAG, "DMA started at 512 kHz (noise shaping order %u), speaker %s",
                  s_dac.os_order, s_dac.amp_on ? "unmuted" : "muted (headphone)");
        return 0;
    }

    IPRO_LOGI(TAG, "Starting DMA triple buffer playback");
    IPRO_LOGI(TAG, "  buffer=%p, ready[0]=%d, ready[1]=%d, ready[2]=%d",
              s_dac.buffer, s_dac.buffer_ready[0], s_dac.buffer_ready[1], s_dac.buffer_ready[2]);

    // Reset state
    s_dac.active_idx = 0;

    // Clean cache for entire triple buffer
    uint32_t total_bytes = DAC_TOTAL_SAMPLES * sizeof(uint16_t);
    L1C_DCACHE_CLEAN_RANGE(s_dac.buffer, total_bytes);

    // Setup DMA LLI chain for triple buffer (pass segment size)
    // dma_reload creates 3x LLI in TRIPLE mode, covering all three buffers
    uint32_t segment_bytes = DAC_SAMPLES_PER_SEGMENT * sizeof(uint16_t);
    IPRO_LOGI(TAG, "  dma_reload: src=0x%08lx, dst=0x%08lx, segment_size=%lu",
              (unsigned long)(uint32_t)s_dac.buffer,
              (unsigned long)DMA_ADDR_DAC_TDR,
              (unsigned long)segment_bytes);

    int ret = dma_reload(dac_dma_dev.id, dac_dma_dev.ch,
                         (uint32_t)s_dac.buffer,
                         DMA_ADDR_DAC_TDR,
                         segment_bytes, NULL);
    if (ret != 0) {
        IPRO_LOGE(TAG, "dma_reload failed: %d", ret);
        return -1;
    }
    IPRO_LOGI(TAG, "  dma_reload OK");

    // Start DMA
    dma_channel_start(dac_dma_dev.id, dac_dma_dev.ch);
    s_dac.running = true;

    // Amplifier on, unless a headphone is in
#ifdef BOARD_HP_DET_PIN
    s_dac.hp_in = jack_plugged_now();
    s_dac.hp_disagree = 0;
#endif
    amp_apply();

    IPRO_LOGI(TAG, "DMA started, speaker %s", s_dac.amp_on ? "unmuted" : "muted (headphone)");

    return 0;
}

/**
 * @brief Stop DMA playback
 */
int app_dac_stop(void)
{
    if (!s_dac.running) {
        return 0;
    }

    IPRO_LOGI(TAG, "Stopping DMA playback");

    // Amplifier off first, to avoid a pop
    gpio_write(DAC_GPIO_SPK, !DAC_SPK_EN_ACTIVE);

    // Stop DMA. The lock keeps the 512 kHz expander out while state resets.
    xSemaphoreTake(s_dac.os_lock, portMAX_DELAY);
    dma_channel_stop(dac_dma_dev.id, dac_dma_dev.ch);
    s_dac.running = false;

    // A jack poll that raced the write above may have switched the amplifier
    // back on; with running false this settles it off.
    amp_apply();

    // Reset buffer state
    for (int i = 0; i < DAC_BUFFER_COUNT; i++) {
        s_dac.buffer_ready[i] = false;
    }
    s_dac.next_fill_idx = 0;    // Reset for next start
    xSemaphoreGive(s_dac.os_lock);

    if (s_dac.os_req != s_dac.os_order) {
        dac_mode_apply();
    }

    return 0;
}

/**
 * @brief Feed audio data to next buffer
 *
 * In triple buffer mode, this fills the next buffer that needs data.
 * Call this when app_dac_wait_buffer_needed() returns, or proactively.
 *
 * @param samples PCM samples (int16_t format)
 * @param count Number of samples (should be DAC_SAMPLES_PER_SEGMENT)
 * @return 0 on success, -1 on error
 */
int app_dac_feed(const int16_t* samples, uint32_t count)
{
    if (!s_dac.initialized) {
        IPRO_LOGE(TAG, "Not initialized");
        return -1;
    }

    if (!samples || count == 0) {
        return -1;
    }

#ifdef BOARD_HP_DET_PIN
    jack_poll();
#endif

    // Determine which buffer to fill
    // Use critical section to prevent race with ISR modifying active_idx
    uint8_t fill_idx;
    taskENTER_CRITICAL();
    if (s_dac.running) {
        // DMA running: find the first non-ready buffer starting from active_idx+1
        // This ensures we fill buffers in the correct order even if feed is delayed
        fill_idx = (s_dac.active_idx + 1) % DAC_BUFFER_COUNT;
        for (int i = 0; i < DAC_BUFFER_COUNT; i++) {
            uint8_t check_idx = (s_dac.active_idx + 1 + i) % DAC_BUFFER_COUNT;
            if (!s_dac.buffer_ready[check_idx]) {
                fill_idx = check_idx;
                break;
            }
        }
    } else {
        // DMA not running (pre-fill mode): use next_fill_idx and advance it
        fill_idx = s_dac.next_fill_idx;
        s_dac.next_fill_idx = (s_dac.next_fill_idx + 1) % DAC_BUFFER_COUNT;
    }
    taskEXIT_CRITICAL();

    uint16_t* dst = get_buffer_segment(fill_idx);

    // Limit to buffer size
    if (count > DAC_SAMPLES_PER_SEGMENT) {
        count = DAC_SAMPLES_PER_SEGMENT;
    }

    // Convert and copy samples
    convert_samples(samples, dst, count);

    // Pad with silence if partial buffer
    if (count < DAC_SAMPLES_PER_SEGMENT) {
        fill_silence(&dst[count], DAC_SAMPLES_PER_SEGMENT - count);
    }

    // Clean cache for this segment
    uint32_t segment_bytes = DAC_SAMPLES_PER_SEGMENT * sizeof(uint16_t);
    L1C_DCACHE_CLEAN_RANGE(dst, segment_bytes);

    // Mark buffer ready
    s_dac.buffer_ready[fill_idx] = true;

    return 0;
}

/**
 * @brief Wait for buffer to need filling
 *
 * Blocks until DMA callback signals that a buffer has been consumed.
 * Use this in your audio processing loop.
 *
 * @param timeout_ms Timeout in milliseconds (0 = no wait, portMAX_DELAY = forever)
 * @return true if buffer needs filling, false on timeout
 */
bool app_dac_wait_buffer_needed(uint32_t timeout_ms)
{
    if (!s_dac.feed_sem) {
        return false;
    }

    TickType_t ticks = (timeout_ms == 0) ? 0 :
                       (timeout_ms == UINT32_MAX) ? portMAX_DELAY :
                       pdMS_TO_TICKS(timeout_ms);

    return xSemaphoreTake(s_dac.feed_sem, ticks) == pdTRUE;
}

/**
 * @brief Get index of buffer that needs filling
 * @return buffer index to fill next
 */
uint8_t app_dac_get_fill_index(void)
{
    // Use critical section to prevent race with ISR modifying active_idx
    uint8_t idx;
    taskENTER_CRITICAL();
    if (s_dac.running) {
        // Find first non-ready buffer starting from active_idx+1
        idx = (s_dac.active_idx + 1) % DAC_BUFFER_COUNT;
        for (int i = 0; i < DAC_BUFFER_COUNT; i++) {
            uint8_t check_idx = (s_dac.active_idx + 1 + i) % DAC_BUFFER_COUNT;
            if (!s_dac.buffer_ready[check_idx]) {
                idx = check_idx;
                break;
            }
        }
    } else {
        idx = s_dac.next_fill_idx;
    }
    taskEXIT_CRITICAL();
    return idx;
}

/**
 * @brief Get samples per buffer segment
 */
uint32_t app_dac_get_buffer_size(void)
{
    return DAC_SAMPLES_PER_SEGMENT;
}

/**
 * @brief Get count of ready (filled) buffers
 */
uint8_t app_dac_get_ready_count(void)
{
    uint8_t count = 0;
    taskENTER_CRITICAL();
    for (int i = 0; i < DAC_BUFFER_COUNT; i++) {
        if (s_dac.buffer_ready[i]) count++;
    }
    taskEXIT_CRITICAL();
    return count;
}

/**
 * @brief Legacy write interface (for backward compatibility)
 *
 * Handles large buffers by splitting across multiple buffer fills.
 * If not running, pre-fills all buffers then starts playback.
 */
int app_dac_write(const int16_t* samples, uint32_t count, bool auto_start)
{
    if (!s_dac.initialized || !samples || count == 0) {
        return -1;
    }

    const int16_t* src = samples;
    uint32_t remaining = count;

    // `dac os` while playing: stop here, in the feeding task, so the switch
    // never races a feed; the pre-fill below restarts on the new path.
    if (s_dac.running && s_dac.os_req != s_dac.os_order) {
        app_dac_stop();
    }

    // If not running, pre-fill all buffers for seamless start
    if (!s_dac.running && auto_start) {
        uint32_t segment_bytes = DAC_SAMPLES_PER_SEGMENT * sizeof(uint16_t);

        // Fill all buffers
        for (int i = 0; i < DAC_BUFFER_COUNT; i++) {
            uint32_t chunk = (remaining > DAC_SAMPLES_PER_SEGMENT) ? DAC_SAMPLES_PER_SEGMENT : remaining;
            if (chunk > 0) {
                convert_samples(src, get_buffer_segment(i), chunk);
                src += chunk;
                remaining -= chunk;
            } else {
                chunk = 0;
            }
            // Fill remainder with silence
            if (chunk < DAC_SAMPLES_PER_SEGMENT) {
                fill_silence(&get_buffer_segment(i)[chunk], DAC_SAMPLES_PER_SEGMENT - chunk);
            }
            L1C_DCACHE_CLEAN_RANGE(get_buffer_segment(i), segment_bytes);
            s_dac.buffer_ready[i] = true;
        }

        // Reset next_fill_idx since we manually filled all buffers
        s_dac.next_fill_idx = 0;

        // Start playback
        return app_dac_start();
    }

    // Already running: feed to next buffer
    // Only fill one buffer per call to avoid blocking
    return app_dac_feed(src, remaining > DAC_SAMPLES_PER_SEGMENT ? DAC_SAMPLES_PER_SEGMENT : remaining);
}

void app_dac_set_volume(uint8_t volume)
{
    if (volume > 100) volume = 100;
    s_dac.config.volume = volume;

    // Convert 0-100 to linear gain 0.0-1.0
    /* volume is 0..100, so this is exact integer arithmetic - no float. */
    s_dac.volume_gain_q16 = ((int32_t)volume * 65536) / 100;

    IPRO_LOGI(TAG, "Volume set to %d%% (gain=%d.%02d)", volume,
              (int)(s_dac.volume_gain_q16 >> 16),
              (int)(((s_dac.volume_gain_q16 & 0xFFFF) * 100) >> 16));
}

void app_dac_profile_reset(void)
{
    s_prof_cycles = 0;
    s_prof_samples = 0;
    s_os_cycles = 0;
    s_os_samples = 0;
}

void app_dac_profile_report(void)
{
    uint32_t clk = SystemCoreClockGet();
    if (s_prof_samples == 0u || clk == 0u) {
        printf("\nDAC profile: nothing measured yet - play something first\n");
        return;
    }
    uint32_t cps = (uint32_t)(s_prof_cycles / s_prof_samples);
    /* The chain runs once per output sample, so the load it imposes is just
     * cycles-per-sample times the output rate. */
    uint32_t load_ppm = (uint32_t)(((uint64_t)cps * DAC_SAMPLE_RATE_HZ * 1000000ull) / clk);

    printf("\n=== DAC chain cost ===\n");
    printf("Samples:      %lu\n", (unsigned long)s_prof_samples);
    printf("Cycles/sample:%lu\n", (unsigned long)cps);
    printf("CPU clock:    %lu Hz\n", (unsigned long)clk);
    printf("Load @%d Hz:  %lu.%02lu%%\n", DAC_SAMPLE_RATE_HZ,
           (unsigned long)(load_ppm / 10000u), (unsigned long)((load_ppm / 100u) % 100u));
    printf("bass=%s  eq=%s\n",
           audio_bass_boost_enabled() ? "on" : "off",
           audio_speaker_eq_enabled() ? "on" : "off");

    if (s_os_samples != 0u) {
        /* Per 512 kHz output sample. This is wall time: the expander runs
         * below nearly everything, so it includes time spent preempted and
         * overstates the cost; `ps` gives its real CPU share (about 8%). */
        uint32_t ocps = (uint32_t)(s_os_cycles / s_os_samples);
        printf("512 kHz expander: %lu cycles/sample wall time incl. preemption "
               "(late ring segments %lu)\n",
               (unsigned long)ocps, (unsigned long)s_dac.stats.os_late);
    }
}

void app_dac_set_gain(float gain)
{
    // Clamp gain to valid range
    if (gain < 0.0f) gain = 0.0f;
    if (gain > 2.0f) gain = 2.0f;

    /* One conversion per volume change, not one per sample. */
    s_dac.volume_gain_q16 = (int32_t)(gain * 65536.0f + 0.5f);

    // Update volume percentage (capped at 100 for gains <= 1.0)
    if (gain <= 1.0f) {
        s_dac.config.volume = (uint8_t)(gain * 100.0f);
    } else {
        s_dac.config.volume = 100;
    }

    IPRO_LOGI(TAG, "Gain set to %.2f (volume=%d%%)", gain, s_dac.config.volume);
}

void app_dac_set_spk_trim_db(int db)
{
    if (db > 0) db = 0;
    if (db < DAC_SPK_TRIM_MIN_DB) db = DAC_SPK_TRIM_MIN_DB;

    s_dac.spk_trim_db = (int8_t)db;
    s_dac.spk_trim_q16 = (int32_t)s_trim_q16[-db];
}

int app_dac_set_os(uint8_t order)
{
    if (order > 2) {
        return -1;
    }
    s_dac.os_req = order;
    if (!s_dac.running) {
        dac_mode_apply();
    }
    return 0;
}

uint8_t app_dac_get_os(void)
{
    return s_dac.os ? s_dac.os_order : 0;
}

void app_dac_set_range(uint8_t range)
{
    uint32_t v = readl(GLB_BASE + GLB_GPDAC_ACTRL_OFFSET);

    v = reg_set_bits(v, GLB_GPDAC_A_RNG, range & 3u);
    writel(v, GLB_BASE + GLB_GPDAC_ACTRL_OFFSET);
}

void app_dac_set_pad(bool on)
{
    uint32_t v = readl(GLB_BASE + GLB_GPDAC_ACTRL_OFFSET);

    v = reg_set_bits(v, GLB_GPDAC_IOA_EN, on ? 1u : 0u);
    writel(v, GLB_BASE + GLB_GPDAC_ACTRL_OFFSET);
}

bool app_dac_get_pad(void)
{
    return reg_get_bits(readl(GLB_BASE + GLB_GPDAC_ACTRL_OFFSET), GLB_GPDAC_IOA_EN) != 0u;
}

uint8_t app_dac_get_range(void)
{
    return (uint8_t)reg_get_bits(readl(GLB_BASE + GLB_GPDAC_ACTRL_OFFSET), GLB_GPDAC_A_RNG);
}

void app_dac_get_route(app_dac_route_t *route)
{
    if (!route) return;

    route->amp_on = s_dac.amp_on;
#ifdef BOARD_HP_DET_PIN
    route->jack = s_dac.hp_in ? 1 : 0;
#else
    route->jack = -1;
#endif
    route->spk_trim_db = s_dac.spk_trim_db;
}

float app_dac_get_gain(void)
{
    return (float)s_dac.volume_gain_q16 / 65536.0f;
}

void app_dac_get_stats(app_dac_stats_t* stats)
{
    if (stats) {
        memcpy(stats, &s_dac.stats, sizeof(app_dac_stats_t));
    }
}

bool app_dac_is_ready(void)
{
    return s_dac.initialized;
}
