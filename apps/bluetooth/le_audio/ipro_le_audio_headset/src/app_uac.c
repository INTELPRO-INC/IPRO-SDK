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
 * @file app_uac.c
 * @brief UAC Output Module Implementation - N-way Cyclic DMA
 *
 * Uses DMA_LLI_CYCLE_N_MODE similar to DAC for continuous USB audio streaming.
 * DMA automatically cycles through N buffers at USB SOF rate (1ms).
 */

#include "app_uac.h"
#include "ipro_vendor_ids.h"
#include "ipro_log.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <string.h>
#include <stdlib.h>

#include "usbd_core.h"
#include "usbd_audio.h"
#include "usb_audio.h"
#include "usbd_event_common.h"
#include <hal_dma.h>

#define TAG "UAC"

/* ============================================================================
 * N-way Cyclic Buffer Configuration
 * ============================================================================ */

#define UAC_BUFFER_COUNT        24      /* Number of DMA LLI buffers */
/* 480 samples = 960 bytes, which divides evenly by both packet sizes we use:
 * 10 packets at 48 kHz (96 B/ms) and 15 packets at 32 kHz (64 B/ms).  Keeping
 * the LLI boundary on a packet boundary is what keeps read_idx accounting
 * aligned with what the USB engine has actually drained. */
#define UAC_SAMPLES_PER_BUFFER  480
#define UAC_BYTES_PER_BUFFER    (UAC_SAMPLES_PER_BUFFER * sizeof(int16_t))
#define UAC_TOTAL_SAMPLES       (UAC_BUFFER_COUNT * UAC_SAMPLES_PER_BUFFER)
#define UAC_UNDERRUN_THRESHOLD  100     /* High threshold - prefer silence over restart */

/* ============================================================================
 * USB Descriptor Configuration
 * ============================================================================ */

#define USBD_VID            IPRO_USB_VID  /* Test VID (not for production!) */
#define USBD_PID            0x0001  /* Test PID for UAC microphone */
#define USBD_MAX_POWER      100
#define USBD_LANGID_STRING  1033

#define EP_INTERVAL         0x01    /* Full Speed: 1ms */

#define AUDIO_IN_EP         UAC_AUDIO_IN_EP
#define AUDIO_IN_FU_ID      0x02

/* Audio params */
static uint32_t s_sample_rate = UAC_OUTPUT_RATE_HZ;
static uint8_t  s_channels = 1;

#define UAC_FREQ_DEFAULT      UAC_OUTPUT_RATE_HZ
#define UAC_FRAME_SIZE_BYTE   2u
#define UAC_RESOLUTION_BIT    16u
#define UAC_CHANNELS          1u  // Mono output

#define UAC_PACKET_SIZE(freq) ((uint32_t)(((freq) * UAC_FRAME_SIZE_BYTE * UAC_CHANNELS) / 1000))

/* 32 kHz stays listed so `uac_test init 32000` can still exercise the
 * resampler path for an A/B against 48 kHz passthrough. */
static const uint32_t s_supported_rates[] = { 48000, 32000 };
#define UAC_NUM_SUPPORTED_RATES (sizeof(s_supported_rates) / sizeof(s_supported_rates[0]))

#define UAC_AC_SIZ (AUDIO_SIZEOF_AC_HEADER_DESC(1) +          \
                    AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +      \
                    AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(1, 1) +  \
                    AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

#define UAC_CONFIG_DESC_SIZ (9 +                                   \
                             AUDIO_AC_DESCRIPTOR_INIT_LEN(1) +     \
                             AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC + \
                             AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(1, 1) + \
                             AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC + \
                             AUDIO_AS_DESCRIPTOR_INIT_LEN(1))

static uint8_t uac_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(UAC_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_AC_DESCRIPTOR_INIT(0x00, 0x02, UAC_AC_SIZ, 0x00, 0x01),
    AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x01, AUDIO_INTERM_MIC, UAC_CHANNELS, 0x0004),
    AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x01, 0x01, 0x03, 0x00),
    AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x03, AUDIO_TERMINAL_STREAMING, AUDIO_IN_FU_ID),
    AUDIO_AS_DESCRIPTOR_INIT(0x01, 0x03, UAC_CHANNELS, UAC_FRAME_SIZE_BYTE, UAC_RESOLUTION_BIT,
                             AUDIO_IN_EP, 0x05, UAC_PACKET_SIZE(UAC_FREQ_DEFAULT),
                             EP_INTERVAL, AUDIO_SAMPLE_FREQ_3B(UAC_FREQ_DEFAULT)),
    USB_LANGID_INIT(USBD_LANGID_STRING),
    0x12, USB_DESCRIPTOR_TYPE_STRING,
    'I', 0x00, 'N', 0x00, 'T', 0x00, 'E', 0x00,
    'L', 0x00, 'P', 0x00, 'R', 0x00, 'O', 0x00,
    0x22, USB_DESCRIPTOR_TYPE_STRING,
    'L', 0x00, 'E', 0x00, ' ', 0x00, 'A', 0x00,
    'u', 0x00, 'd', 0x00, 'i', 0x00, 'o', 0x00,
    ' ', 0x00, 'U', 0x00, 'A', 0x00, 'C', 0x00,
    ' ', 0x00, 'O', 0x00, 'u', 0x00, 't', 0x00,
    0x16, USB_DESCRIPTOR_TYPE_STRING,
    '2', 0x00, '0', 0x00, '2', 0x00, '6', 0x00,
    '0', 0x00, '1', 0x00, '2', 0x00, '8', 0x00,
    '0', 0x00, '1', 0x00,
    0x00
};

static int patch_descriptor_sample_rate(uint32_t sample_rate)
{
    bool valid = false;
    for (uint32_t i = 0; i < UAC_NUM_SUPPORTED_RATES; i++) {
        if (sample_rate == s_supported_rates[i]) {
            valid = true;
            break;
        }
    }
    if (!valid) {
        IPRO_LOGE(TAG, "Unsupported sample rate: %lu Hz", (unsigned long)sample_rate);
        return -1;
    }

    uint32_t packet_size = UAC_PACKET_SIZE(sample_rate);
    size_t desc_len = sizeof(uac_descriptor);
    size_t offset = 0;
    bool patched_ep = false;
    bool patched_rate = false;

    IPRO_LOGI(TAG, "Patching descriptor for %lu Hz, desc_len=%d",
              (unsigned long)sample_rate, (int)desc_len);

    while (offset < desc_len - 1) {
        uint8_t len = uac_descriptor[offset];
        uint8_t type = uac_descriptor[offset + 1];

        if (len == 0 || offset + len > desc_len) break;

        if (type == USB_DESCRIPTOR_TYPE_ENDPOINT && len >= 7) {
            IPRO_LOGI(TAG, "  EP desc @%d: pkt=%d->%lu", (int)offset,
                      uac_descriptor[offset + 4] | (uac_descriptor[offset + 5] << 8),
                      (unsigned long)packet_size);
            uac_descriptor[offset + 4] = (uint8_t)(packet_size & 0xFF);
            uac_descriptor[offset + 5] = (uint8_t)((packet_size >> 8) & 0xFF);
            patched_ep = true;
        }

        /* Format Type descriptor: type=0x24, subtype=0x02, len=11 for 1 sample rate
         * Check bFormatType (offset+3) == 0x01 (AUDIO_FORMAT_TYPE_I) AND
         * bSamFreqType (offset+7) == 0x01 (1 sample rate) to distinguish
         * from Input Terminal descriptor which has subtype=0x02 but different structure */
        if (type == 0x24 && len == 11 && uac_descriptor[offset + 2] == 0x02 &&
            uac_descriptor[offset + 3] == 0x01 && uac_descriptor[offset + 7] == 0x01) {
            size_t rate_offset = offset + len - 3;
            uint32_t old_rate = uac_descriptor[rate_offset] |
                               (uac_descriptor[rate_offset + 1] << 8) |
                               (uac_descriptor[rate_offset + 2] << 16);
            IPRO_LOGI(TAG, "  Format Type desc @%d (len=%d): %lu->%lu Hz",
                      (int)offset, len, (unsigned long)old_rate, (unsigned long)sample_rate);
            uac_descriptor[rate_offset + 0] = (uint8_t)(sample_rate & 0xFF);
            uac_descriptor[rate_offset + 1] = (uint8_t)((sample_rate >> 8) & 0xFF);
            uac_descriptor[rate_offset + 2] = (uint8_t)((sample_rate >> 16) & 0xFF);
            patched_rate = true;
        }

        offset += len;
    }

    if (!patched_ep || !patched_rate) {
        IPRO_LOGE(TAG, "Failed to patch: ep=%d rate=%d", patched_ep, patched_rate);
        return -1;
    }

    IPRO_LOGI(TAG, "Patched USB descriptor: %lu Hz, packet=%lu bytes",
              (unsigned long)sample_rate, (unsigned long)packet_size);
    return 0;
}

/* ============================================================================
 * N-way Cyclic Buffer State
 * ============================================================================ */

typedef struct {
    /* N contiguous buffers for DMA LLI chain */
    int16_t buffer[UAC_TOTAL_SAMPLES] __attribute__((aligned(32)));

    /* Buffer indices */
    volatile uint32_t read_idx;     /* DMA consumer index (updated by ISR) */
    volatile uint32_t write_idx;    /* Producer index (updated by task) */
    uint32_t write_offset;          /* Offset within current write buffer */

    /* State */
    volatile bool running;
    volatile bool tx_flag;          /* USB host opened stream */

    /* Statistics */
    app_uac_stats_t stats;

    /* Synchronization */
    SemaphoreHandle_t write_sem;    /* Signaled when buffer consumed */
} uac_state_t;

/* Must be in non-cacheable memory for DMA */
USB_NOCACHE_RAM_SECTION static uac_state_t s_uac __attribute__((aligned(32)));

/* Forward declarations */
static int dma_usb_start_cyclic(void);

/* ============================================================================
 * USB Event Handling
 * ============================================================================ */

static void uac_event_handler(uint8_t event)
{
    if (event == USBD_EVENT_DISCONNECTED) {
        IPRO_LOGI(TAG, "USB disconnected");
        s_uac.tx_flag = false;
        if (s_uac.running) {
            s_uac.running = false;
            dma_channel_stop(DMA0_ID, DMA0_CH1_INDEX);
        }
    }
}

void usbd_audio_open(uint8_t intf)
{
    if (intf == 1) {
        s_uac.tx_flag = true;
        IPRO_LOGI(TAG, "USB host opened audio stream");

        /* Start DMA immediately if not already running.
         * USB host expects isochronous data packets right away.
         * If no BLE audio yet, we'll send silence (buffers already zeroed). */
        if (!s_uac.running) {
            /* Reset buffer indices to ensure clean state */
            s_uac.read_idx = 0;
            s_uac.write_idx = UAC_BUFFER_COUNT / 2;  /* Pre-advance write pointer */
            s_uac.write_offset = 0;

            /* Clear buffers to silence */
            memset(s_uac.buffer, 0, sizeof(s_uac.buffer));
            L1C_DCACHE_CLEAN_RANGE(s_uac.buffer, sizeof(s_uac.buffer));

            /* Start DMA cyclic transfer */
            if (dma_usb_start_cyclic() == 0) {
                s_uac.running = true;
                IPRO_LOGI(TAG, "DMA started with silence (no BLE audio yet)");
            } else {
                IPRO_LOGE(TAG, "Failed to start DMA");
            }
        }
    }
}

void usbd_audio_close(uint8_t intf)
{
    if (intf == 1) {
        s_uac.tx_flag = false;
        IPRO_LOGI(TAG, "USB host closed audio stream");
    }
}

void usbd_audio_in_callback(uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    (void)nbytes;
}

static struct usbd_endpoint s_audio_in_ep = {
    .ep_cb = usbd_audio_in_callback,
    .ep_addr = AUDIO_IN_EP
};

static struct usbd_interface s_intf0;
static struct usbd_interface s_intf1;

static struct audio_entity_info s_entity_table[] = {
    {
        .bEntityId = AUDIO_IN_FU_ID,
        .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT,
        .ep = AUDIO_IN_EP
    },
};

/* ============================================================================
 * DMA N-way Cyclic Mode for USB
 * ============================================================================ */

/* Forward declarations */
static void uac_dma_callback(void *dev, uint32_t event);
static int16_t* get_buffer_ptr(uint32_t idx);

static dma_device_t s_usb_dma = {
    .id = DMA0_ID,
    .ch = DMA0_CH1_INDEX,
    .transfer_mode = DMA_LLI_CYCLE_N_MODE,      /* N-buffer circular mode */
    .direction = DMA_MEMORY_TO_PERIPH,
    .src_req = DMA_REQUEST_NONE,
    .dst_req = DMA_REQUEST_USB_EP1,             /* USB EP1 requests data at SOF */
    .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_size = DMA_BURST_INCR16,         /* Original UAC setting */
    .dst_burst_size = DMA_BURST_INCR1,
    .src_width = DMA_TRANSFER_WIDTH_8BIT,       /* USB FIFO is 8-bit */
    .dst_width = DMA_TRANSFER_WIDTH_8BIT,
    .lli_count = UAC_BUFFER_COUNT,              /* Number of buffers */
    .lli_cfg = NULL,
    .callback = uac_dma_callback,               /* Set callback directly! */
};

/*
 * DMA callback - fires when each buffer is consumed by USB
 * Only updates read index, no waiting or blocking
 */
static void uac_dma_callback(void *dev, uint32_t event)
{
    (void)dev;
    BaseType_t woken = pdFALSE;

    if (event == HAL_DMA_INT_COMPLETE) {
        /* HW workaround: toggle DMA TX enable to recover from stall */
        usbd_ep_toggle_dma_tx(AUDIO_IN_EP);

        /* Advance read index (DMA just consumed this buffer) */
        s_uac.read_idx = (s_uac.read_idx + 1) % UAC_BUFFER_COUNT;
        s_uac.stats.frames_sent++;

        /* Check for underrun: if read catches up to write */
        uint32_t write_idx = s_uac.write_idx;
        if (s_uac.read_idx == write_idx) {
            s_uac.stats.underruns++;
            s_uac.stats.consecutive_underruns++;
            if (s_uac.stats.consecutive_underruns > s_uac.stats.max_consecutive_underruns) {
                s_uac.stats.max_consecutive_underruns = s_uac.stats.consecutive_underruns;
            }

            /*
             * Instead of restarting DMA (which causes audio gaps), just
             * zero-fill buffers ahead of the read pointer. This produces
             * brief silence instead of a full audio dropout.
             *
             * We advance write_idx by 2 buffers to give producer time to
             * catch up, then fill those buffers with silence.
             */
            uint32_t fill_count = 2;
            for (uint32_t i = 0; i < fill_count; i++) {
                uint32_t idx = (s_uac.read_idx + i) % UAC_BUFFER_COUNT;
                memset(get_buffer_ptr(idx), 0, UAC_BYTES_PER_BUFFER);
                L1C_DCACHE_CLEAN_RANGE(get_buffer_ptr(idx), UAC_BYTES_PER_BUFFER);
            }
            /* Advance write index past the silence we just inserted */
            s_uac.write_idx = (s_uac.read_idx + fill_count) % UAC_BUFFER_COUNT;
            s_uac.write_offset = 0;
        } else {
            s_uac.stats.consecutive_underruns = 0;
        }

        /* Signal producer that a buffer is now free */
        xSemaphoreGiveFromISR(s_uac.write_sem, &woken);
        portYIELD_FROM_ISR(woken);
    }
}

static int16_t* get_buffer_ptr(uint32_t idx)
{
    return &s_uac.buffer[idx * UAC_SAMPLES_PER_BUFFER];
}

static void dma_usb_init(void)
{
    s_usb_dma.callback = uac_dma_callback;
    dma_setConfig(&s_usb_dma);
    dma_init(DMA0_ID, DMA0_CH1_INDEX);
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, DMA0_CH1_INDEX, DMA_CTRL_SET_CALLBACK, uac_dma_callback);
    usbd_ep_enable_dma_tx(AUDIO_IN_EP);
}

static int dma_usb_start_cyclic(void)
{
    /* Clean cache for entire buffer */
    L1C_DCACHE_CLEAN_RANGE(s_uac.buffer, sizeof(s_uac.buffer));

    uint32_t src_addr = (uint32_t)(uintptr_t)s_uac.buffer;
    uint32_t dst_addr = usbd_ep_get_tx_fifo_addr(AUDIO_IN_EP);

    /* Setup DMA LLI chain - pass ONE segment size, DMA creates N LLIs */
    int ret = dma_reload(DMA0_ID, DMA0_CH1_INDEX,
                         src_addr, dst_addr,
                         UAC_BYTES_PER_BUFFER, NULL);
    if (ret != 0) {
        IPRO_LOGE(TAG, "DMA reload failed: %d", ret);
        return ret;
    }

    /* Start DMA - runs continuously */
    dma_channel_start(DMA0_ID, DMA0_CH1_INDEX);

    /* Trigger EP ready once to start the first transfer */
    usbd_ep_trigger_ready(AUDIO_IN_EP);

    IPRO_LOGI(TAG, "DMA started: %d buffers x %d samples",
              UAC_BUFFER_COUNT, UAC_SAMPLES_PER_BUFFER);
    return 0;
}

/* ============================================================================
 * Ring Buffer for Producer (LC3 decode) -> Consumer (DMA)
 * ============================================================================ */

/*
 * Get number of buffers available for writing (free buffers)
 * Producer can write to buffers from write_idx up to (read_idx - 1)
 */
static uint32_t buffers_free(void)
{
    uint32_t read = s_uac.read_idx;
    uint32_t write = s_uac.write_idx;

    if (write >= read) {
        /* Free = total - (write - read) - 1 (keep one buffer margin) */
        return UAC_BUFFER_COUNT - (write - read) - 1;
    } else {
        /* Wrapped: free = read - write - 1 */
        return read - write - 1;
    }
}

/*
 * Get number of buffers filled and ready for DMA
 */
static uint32_t buffers_filled(void)
{
    uint32_t read = s_uac.read_idx;
    uint32_t write = s_uac.write_idx;

    if (write >= read) {
        return write - read;
    } else {
        return UAC_BUFFER_COUNT - read + write;
    }
}

/* ============================================================================
 * Public API
 * ============================================================================ */

static bool s_uac_initialized = false;

int app_uac_init(const app_uac_config_t *config)
{
    if (!config) return -1;

    if (config->channels != UAC_CHANNELS) {
        IPRO_LOGE(TAG, "UAC only supports %u channel(s)", (unsigned)UAC_CHANNELS);
        return -1;
    }

    /* Skip full reinit if already initialized with same sample rate */
    if (s_uac_initialized && config->sample_rate == s_sample_rate) {
        IPRO_LOGI(TAG, "UAC already at %lu Hz, skipping reinit",
                  (unsigned long)config->sample_rate);
        return 0;
    }

    /* Stop any running DMA and USB first (for re-init case) */
    s_uac.running = false;
    dma_channel_stop(DMA0_ID, DMA0_CH1_INDEX);
    usbd_deinitialize();

    if (patch_descriptor_sample_rate(config->sample_rate) != 0) {
        return -1;
    }

    IPRO_LOGI(TAG, "Initializing UAC: %lu Hz, %d buffers x %d samples",
              (unsigned long)config->sample_rate, UAC_BUFFER_COUNT, UAC_SAMPLES_PER_BUFFER);

    s_sample_rate = config->sample_rate;
    s_channels = config->channels;

    /* Initialize state */
    memset(&s_uac, 0, sizeof(s_uac));
    s_uac.read_idx = 0;
    s_uac.write_idx = 0;
    s_uac.write_offset = 0;

    /* Pre-fill all buffers with silence */
    memset(s_uac.buffer, 0, sizeof(s_uac.buffer));

    /* Create semaphore */
    if (s_uac.write_sem == NULL) {
        s_uac.write_sem = xSemaphoreCreateBinary();
        if (!s_uac.write_sem) {
            IPRO_LOGE(TAG, "Failed to create semaphore");
            return -1;
        }
    }

    /* Initialize USB stack */
    usbd_desc_register(uac_descriptor);
    usbd_add_interface(usbd_audio_init_intf(&s_intf0, 0x0100, s_entity_table, 1));
    usbd_add_interface(usbd_audio_init_intf(&s_intf1, 0x0100, s_entity_table, 1));
    usbd_add_endpoint(&s_audio_in_ep);
    usbd_event_register_callback(uac_event_handler);
    usbd_initialize();

    /* Initialize DMA */
    dma_usb_init();

    s_uac_initialized = true;
    IPRO_LOGI(TAG, "UAC initialized, waiting for USB host");
    return 0;
}

void app_uac_deinit(void)
{
    s_uac.running = false;
    s_uac_initialized = false;

    dma_channel_stop(DMA0_ID, DMA0_CH1_INDEX);
    usbd_deinitialize();

    if (s_uac.write_sem) {
        vSemaphoreDelete(s_uac.write_sem);
        s_uac.write_sem = NULL;
    }

    IPRO_LOGI(TAG, "UAC deinitialized");
}

int app_uac_write(const int16_t *samples, uint32_t count, bool auto_start)
{
    if (!samples || count == 0) return -1;

    // UAC is mono, write samples directly
    s_uac.stats.samples_written += count;

    /* Write samples to buffers, accumulating until buffer is full */
    uint32_t written = 0;
    while (written < count) {
        /* Check if current buffer is full and we need a new one */
        if (s_uac.write_offset >= UAC_SAMPLES_PER_BUFFER) {
            /* Current buffer is full, try to advance to next */
            if (buffers_free() == 0) {
                /* Wait for DMA to free a buffer instead of dropping.
                 * DMA fires every ~16ms; a short wait avoids cascading
                 * drops of consecutive frames per overrun. */
                xSemaphoreTake(s_uac.write_sem, pdMS_TO_TICKS(20));
                if (buffers_free() == 0) {
                    /* Still full after timeout - drop remaining */
                    s_uac.stats.overruns++;
                    if (s_uac.stats.overruns <= 10 || s_uac.stats.overruns % 100 == 0) {
                        IPRO_LOGW(TAG, "OVERRUN #%lu: drop %lu samples after wait",
                                  (unsigned long)s_uac.stats.overruns,
                                  (unsigned long)(count - written));
                    }
                    break;
                }
            }
            /* Advance to next buffer */
            s_uac.write_idx = (s_uac.write_idx + 1) % UAC_BUFFER_COUNT;
            s_uac.write_offset = 0;
        }

        /* Get current write buffer and position */
        int16_t *buf = get_buffer_ptr(s_uac.write_idx);

        /* Calculate how many samples we can write to this buffer */
        uint32_t space_in_buffer = UAC_SAMPLES_PER_BUFFER - s_uac.write_offset;
        uint32_t to_copy = count - written;
        
        if (to_copy > space_in_buffer) {
            to_copy = space_in_buffer;
        }

        /* Copy mono samples directly */
        memcpy(&buf[s_uac.write_offset], &samples[written], to_copy * sizeof(int16_t));
        s_uac.write_offset += to_copy;
        
        written += to_copy;
        s_uac.stats.samples_read += to_copy;

        /* If buffer is now full, clean cache for DMA */
        if (s_uac.write_offset >= UAC_SAMPLES_PER_BUFFER) {
            L1C_DCACHE_CLEAN_RANGE(buf, UAC_BYTES_PER_BUFFER);
        }
    }

    /* Start (or restart) DMA when prebuffer is ready */
    if (!s_uac.running && auto_start && s_uac.tx_flag) {
        // Wait for 75% full to have better burst absorption
        if (buffers_filled() >= (UAC_BUFFER_COUNT * 3) / 4) {
            if (dma_usb_start_cyclic() == 0) {
                s_uac.running = true;
                IPRO_LOGI(TAG, "DMA %s with %lu buffers pre-filled",
                          s_uac.stats.restarts ? "restarted" : "started",
                          (unsigned long)buffers_filled());
            }
        }
    }

    return 0;
}

void app_uac_stop(void)
{
    s_uac.running = false;
    dma_channel_stop(DMA0_ID, DMA0_CH1_INDEX);
    IPRO_LOGI(TAG, "UAC stopped");
}

void app_uac_get_stats(app_uac_stats_t *stats)
{
    if (stats) *stats = s_uac.stats;
}

bool app_uac_is_ready(void)
{
    return s_uac.running && s_uac.tx_flag;
}

/* ============================================================================
 * Shell Commands
 * ============================================================================ */

#include "shell.h"

static int cmd_uac_test(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: uac_test <init|stats|stop|tone|fifo>\n");
        return 0;
    }

    if (strcmp(argv[1], "init") == 0) {
        uint32_t rate = 32000;
        if (argc >= 3) rate = (uint32_t)atoi(argv[2]);
        app_uac_config_t cfg = { .sample_rate = rate, .channels = 1 };
        int ret = app_uac_init(&cfg);
        printf("UAC init %lu Hz: %s\n", (unsigned long)rate, ret == 0 ? "OK" : "FAILED");
        return ret;
    }

    if (strcmp(argv[1], "stats") == 0) {
        printf("UAC Stats:\n");
        printf("  Sample rate: %lu Hz\n", (unsigned long)s_sample_rate);
        printf("  Buffers: %d x %d samples (%lums each)\n",
               UAC_BUFFER_COUNT, UAC_SAMPLES_PER_BUFFER,
               (unsigned long)(UAC_SAMPLES_PER_BUFFER * 1000 / s_sample_rate));
        printf("  Read idx:    %lu\n", (unsigned long)s_uac.read_idx);
        printf("  Write idx:   %lu (offset: %lu/%d)\n",
               (unsigned long)s_uac.write_idx,
               (unsigned long)s_uac.write_offset,
               UAC_SAMPLES_PER_BUFFER);
        printf("  Filled:      %lu\n", (unsigned long)buffers_filled());
        printf("  Free:        %lu\n", (unsigned long)buffers_free());
        printf("  ---\n");
        printf("  Frames sent: %lu\n", (unsigned long)s_uac.stats.frames_sent);
        printf("  Underruns:   %lu (max consec: %lu)\n",
               (unsigned long)s_uac.stats.underruns,
               (unsigned long)s_uac.stats.max_consecutive_underruns);
        printf("  Overruns:    %lu\n", (unsigned long)s_uac.stats.overruns);
        printf("  Restarts:    %lu\n", (unsigned long)s_uac.stats.restarts);
        printf("  Written:     %lu samples\n", (unsigned long)s_uac.stats.samples_written);
        printf("  ---\n");
        printf("  Running:     %s\n", s_uac.running ? "yes" : "no");
        printf("  USB host:    %s\n", s_uac.tx_flag ? "yes" : "no");
        return 0;
    }

    if (strcmp(argv[1], "stop") == 0) {
        app_uac_stop();
        return 0;
    }

    if (strcmp(argv[1], "fifo") == 0) {
        /* Read USB EP1 FIFO status register to determine FIFO depth */
        volatile uint32_t *fifo_status = (volatile uint32_t *)(0x30084000 + 0x114);
        volatile uint32_t *fifo_config = (volatile uint32_t *)(0x30084000 + 0x110);
        volatile uint32_t *ep1_config  = (volatile uint32_t *)(0x30084000 + 0x10C);
        uint32_t status = *fifo_status;
        uint32_t config = *fifo_config;
        uint32_t epcfg  = *ep1_config;
        uint32_t cnt   = (status >> 0) & 0x7F;
        bool empty     = (status >> 14) & 1;
        bool full      = (status >> 15) & 1;
        bool overflow  = (config >> 4) & 1;
        bool underflow = (config >> 5) & 1;
        uint32_t ep_size = epcfg & 0x7FF;
        printf("USB EP1 FIFO:\n");
        printf("  EP config:    0x%08lx (SIZE=%lu)\n", (unsigned long)epcfg, (unsigned long)ep_size);
        printf("  FIFO config:  0x%08lx (overflow=%d, underflow=%d)\n",
               (unsigned long)config, overflow, underflow);
        printf("  FIFO status:  0x%08lx (cnt=%lu, empty=%d, full=%d)\n",
               (unsigned long)status, (unsigned long)cnt, empty, full);
        printf("  Packet size:  %lu bytes (48kHz=96, 32kHz=64)\n",
               (unsigned long)UAC_PACKET_SIZE(s_sample_rate));

        /* Stress test: fill FIFO to find max depth */
        if (argc >= 3 && strcmp(argv[2], "depth") == 0) {
            /* Stop DMA first to avoid interference */
            dma_channel_stop(DMA0_ID, DMA0_CH1_INDEX);
            /* Clear FIFO */
            *fifo_config |= (1 << 2); /* TX_FIFO_CLR */
            *fifo_config &= ~(1 << 2);
            volatile uint32_t *fifo_wdata = (volatile uint32_t *)(0x30084000 + 0x118);
            uint32_t max_cnt = 0;
            for (int i = 0; i < 512; i++) {
                *fifo_wdata = 0;
                uint32_t c = (*fifo_status >> 0) & 0x7F;
                if (c > max_cnt) max_cnt = c;
                if ((*fifo_status >> 15) & 1) { /* FULL */
                    printf("  FIFO FULL after %d writes, cnt=%lu\n", i + 1, (unsigned long)max_cnt);
                    break;
                }
            }
            printf("  Max FIFO depth: %lu bytes\n", (unsigned long)max_cnt);
            printf("  NOTE: DMA stopped! Run 'uac_test stop' then re-init.\n");
        }
        return 0;
    }

    if (strcmp(argv[1], "tone") == 0) {
        uint32_t freq = 1000;
        uint32_t dur = 5;
        if (argc >= 3) freq = (uint32_t)atoi(argv[2]);
        if (argc >= 4) dur = (uint32_t)atoi(argv[3]);
        printf("Generating %lu Hz tone for %lu sec\n",
               (unsigned long)freq, (unsigned long)dur);
        return app_uac_test_tone(freq, dur);
    }

    printf("Unknown command: %s\n", argv[1]);
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(cmd_uac_test, uac_test, UAC test commands);

/* ============================================================================
 * Test Tone Generator
 * ============================================================================ */

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int app_uac_test_tone(uint32_t freq_hz, uint32_t duration_sec)
{
    if (!s_uac_initialized) {
        IPRO_LOGE(TAG, "UAC not initialized");
        return -1;
    }

    uint32_t total_samples = s_sample_rate * duration_sec;
    uint32_t chunk = s_sample_rate / 100; /* 10ms worth */
    if (chunk > 480) chunk = 480;
    int16_t buf[480];

    IPRO_LOGI(TAG, "Test tone: %lu Hz, %lu sec, %lu samples",
              (unsigned long)freq_hz, (unsigned long)duration_sec,
              (unsigned long)total_samples);

    uint32_t written = 0;
    while (written < total_samples) {
        uint32_t n = total_samples - written;
        if (n > chunk) n = chunk;

        for (uint32_t i = 0; i < n; i++) {
            double t = (double)(written + i) / (double)s_sample_rate;
            buf[i] = (int16_t)(16000.0 * sin(2.0 * M_PI * freq_hz * t));
        }

        app_uac_write(buf, n, true);
        written += n;

        if (buffers_free() < 4) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    IPRO_LOGI(TAG, "Test tone done: %lu samples", (unsigned long)written);
    return 0;
}
