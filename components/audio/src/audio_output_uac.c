/**
 * @file audio_output_uac.c
 * @brief UAC output sink — generic ring buffer + cyclic DMA
 *
 * Component responsibilities:
 *   - USB descriptor generation from config (rate, channels)
 *   - DMA ring buffer management (cyclic N-mode)
 *   - Staging buffer for atomic segment commits
 *   - USB stack init/deinit and audio callbacks
 *
 * Caller responsibilities:
 *   - Sample rate conversion (resample before write)
 *   - Channel format conversion (mono/stereo before write)
 *   - PCM data must match cfg.sample_rate and cfg.channels
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#include "audio_output.h"
#include "ipro_vendor_ids.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_dma.h>
#include "usbd_core.h"
#include "usbd_audio.h"
#include "system_soc.h"

typedef void (*usbd_event_callback_t)(uint8_t event);
extern void usbd_event_register_callback(usbd_event_callback_t callback);

/* ---------------------------------------------------------------------------
 * Constants
 * ------------------------------------------------------------------------- */

#define USBD_VID            IPRO_USB_VID
#define USBD_PID            0x0003
#define AUDIO_IN_EP         0x81
#define AUDIO_IN_FU_ID      0x02

/* Ring buffer: 12 segments, sized per config rate */
#define UAC_BUF_COUNT       12
#define UAC_BUF_MS          16   /* ms per segment */

/* Max samples per write call */
#define UAC_MAX_WRITE_SAMPLES  2048

/* ---------------------------------------------------------------------------
 * USB Descriptor (built at create time based on config)
 * ------------------------------------------------------------------------- */

#define UAC_AC_SIZ (AUDIO_SIZEOF_AC_HEADER_DESC(1) +          \
                    AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +      \
                    AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(1, 1) +  \
                    AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

#define UAC_CONFIG_SIZ (9 +                                    \
                        AUDIO_AC_DESCRIPTOR_INIT_LEN(1) +      \
                        AUDIO_SIZEOF_AC_INPUT_TERMINAL_DESC +  \
                        AUDIO_SIZEOF_AC_FEATURE_UNIT_DESC(1, 1) + \
                        AUDIO_SIZEOF_AC_OUTPUT_TERMINAL_DESC + \
                        AUDIO_AS_DESCRIPTOR_INIT_LEN(1))

/* Legacy default when CONFIG_DMA_CHANNEL_ALLOCATOR is off. With the
 * allocator enabled, the channel is obtained at start-time via
 * dma_request_channel(DMA_REQUEST_USB_EP1, ...). */
#define UAC_DMA_CH_LEGACY   DMA0_CH1_INDEX

/* ---------------------------------------------------------------------------
 * State
 * ------------------------------------------------------------------------- */

struct audio_output {
    audio_output_cfg_t cfg;
    int                started;

    /* Derived from config */
    uint32_t           buf_samples;   /* samples per segment */
    uint32_t           buf_bytes;     /* bytes per segment */
    uint32_t           total_samples; /* total ring samples */
    uint32_t           packet_size;   /* USB packet size (bytes per ms) */

    /* Resolved at start(). Legacy build uses UAC_DMA_CH_LEGACY; allocator
     * build gets the channel from dma_request_channel(). The usbd_audio_open
     * callback and dma callback both read this field rather than the
     * compile-time DMA0_CH1_INDEX. */
    uint8_t            dma_ch;
#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    dma_ch_handle_t    dma_handle;
#endif

    /* DMA ring buffer (PSRAM, cacheable) */
    int16_t           *dma_buf;
    int16_t           *dma_buf_raw;

    /* Staging buffer — only copy to ring when full */
    int16_t           *staging;
    uint32_t           staging_count;

    /* Ring state */
    /* usbd_audio_open() is a class callback and runs in USB interrupt context.
     * printf there costs ~1.5 KB of whatever task stack was interrupted - the
     * IDLE task's 4 KB one, which sits directly above g_timer_tcb in
     * .ocram_bss.  Latch the event and let write() report it. */
    volatile uint8_t   report_started;
    volatile uint32_t  segments_done; /* monotonic DMA segment completions */
    volatile uint32_t  read_idx;
    volatile uint32_t  write_idx;
    volatile int       running;
    SemaphoreHandle_t  write_sem;

    /* USB descriptor (dynamically built) */
    uint8_t            usb_desc[256];
    uint32_t           usb_desc_len;
};

static audio_output_t *s_uac_out;

/* ---------------------------------------------------------------------------
 * Ring buffer helpers
 * ------------------------------------------------------------------------- */

static inline int16_t *ring_seg(audio_output_t *out, uint32_t idx)
{
    return &out->dma_buf[idx * out->buf_samples * out->cfg.channels];
}

static uint32_t ring_free(audio_output_t *out)
{
    uint32_t r = out->read_idx, w = out->write_idx;
    return (w >= r) ? (UAC_BUF_COUNT - (w - r) - 1) : (r - w - 1);
}

/* ---------------------------------------------------------------------------
 * DMA + USB callbacks
 * ------------------------------------------------------------------------- */

static void uac_dma_cb(void *dev, uint32_t event)
{
    (void)dev;
    BaseType_t woken = pdFALSE;
    if (!s_uac_out || event != HAL_DMA_INT_COMPLETE) return;

    audio_output_t *out = s_uac_out;
    usbd_ep_toggle_dma_tx(AUDIO_IN_EP);

    out->read_idx = (out->read_idx + 1) % UAC_BUF_COUNT;
    out->segments_done++;

    /* Underrun: silence ahead */
    if (out->read_idx == out->write_idx) {
        for (int i = 0; i < 2 && i < UAC_BUF_COUNT; i++) {
            uint32_t idx = (out->read_idx + i) % UAC_BUF_COUNT;
            memset(ring_seg(out, idx), 0, out->buf_bytes);
            L1C_DCACHE_CLEAN_RANGE(ring_seg(out, idx), out->buf_bytes);
        }
        out->write_idx = (out->read_idx + 2) % UAC_BUF_COUNT;
        out->staging_count = 0;
    }

    if (out->write_sem) {
        xSemaphoreGiveFromISR(out->write_sem, &woken);
        portYIELD_FROM_ISR(woken);
    }
}

/* .ch is patched at start() from out->dma_ch (allocator or legacy). */
static dma_device_t s_usb_dma = {
    .id = DMA0_ID,
    .ch = UAC_DMA_CH_LEGACY,
    .transfer_mode = DMA_LLI_CYCLE_N_MODE,
    .direction = DMA_MEMORY_TO_PERIPH,
    .src_req = DMA_REQUEST_NONE,
    .dst_req = DMA_REQUEST_USB_EP1,
    .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_size = DMA_BURST_INCR16,
    .dst_burst_size = DMA_BURST_INCR1,
    .src_width = DMA_TRANSFER_WIDTH_8BIT,
    .dst_width = DMA_TRANSFER_WIDTH_8BIT,
    .lli_count = UAC_BUF_COUNT,
    .lli_cfg = NULL,
    .callback = uac_dma_cb,
};

static void uac_event_handler(uint8_t event)
{
    if (event == USBD_EVENT_DISCONNECTED && s_uac_out) {
        s_uac_out->running = 0;
        dma_channel_stop(DMA0_ID, s_uac_out->dma_ch);
    }
}

void usbd_audio_open(uint8_t intf)
{
    if (intf != 1 || !s_uac_out || s_uac_out->running) return;
    audio_output_t *out = s_uac_out;

    s_usb_dma.ch = out->dma_ch;
    dma_setConfig(&s_usb_dma);
    dma_init(DMA0_ID, out->dma_ch);
    dma_ioctl(DMA0_ID, out->dma_ch, DMA_CTRL_SET_INT, NULL);
    dma_ioctl(DMA0_ID, out->dma_ch, DMA_CTRL_SET_CALLBACK, uac_dma_cb);
    usbd_ep_enable_dma_tx(AUDIO_IN_EP);

    out->read_idx = 0;
    out->segments_done = 0;
    out->write_idx = UAC_BUF_COUNT / 2;
    out->staging_count = 0;
    memset(out->dma_buf, 0, out->total_samples * out->cfg.channels * sizeof(int16_t));
    L1C_DCACHE_CLEAN_RANGE(out->dma_buf,
                           out->total_samples * out->cfg.channels * sizeof(int16_t));

    int ret = dma_reload(DMA0_ID, out->dma_ch,
                         (uint32_t)(uintptr_t)out->dma_buf,
                         usbd_ep_get_tx_fifo_addr(AUDIO_IN_EP),
                         out->buf_bytes, NULL);
    if (ret == 0) {
        dma_channel_start(DMA0_ID, out->dma_ch);
        usbd_ep_trigger_ready(AUDIO_IN_EP);
        out->running = 1;
        out->report_started = 1;   /* printed from write(), in task context */
    }
}

void usbd_audio_close(uint8_t intf) { (void)intf; }
void usbd_audio_in_callback(uint8_t ep, uint32_t nbytes) { (void)ep; (void)nbytes; }

/* ---------------------------------------------------------------------------
 * USB descriptor builder
 * ------------------------------------------------------------------------- */

static void build_usb_descriptor(audio_output_t *out)
{
    uint32_t rate = out->cfg.sample_rate;
    uint8_t  nch = out->cfg.channels;
    uint32_t pkt = out->packet_size;

    /* Template descriptor */
    const uint8_t desc[] = {
        USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01,
                                   USBD_VID, USBD_PID, 0x0001, 0x01),
        USB_CONFIG_DESCRIPTOR_INIT(UAC_CONFIG_SIZ, 0x02, 0x01,
                                   USB_CONFIG_BUS_POWERED, 100),
        AUDIO_AC_DESCRIPTOR_INIT(0x00, 0x02, UAC_AC_SIZ, 0x00, 0x01),
        AUDIO_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x01, AUDIO_INTERM_MIC,
                                                nch, (nch == 1) ? 0x0004 : 0x0003),
        AUDIO_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_IN_FU_ID, 0x01, 0x01,
                                              0x03, 0x00),
        AUDIO_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x03, AUDIO_TERMINAL_STREAMING,
                                                 AUDIO_IN_FU_ID),
        AUDIO_AS_DESCRIPTOR_INIT(0x01, 0x03, nch, 2, 16,
                                 AUDIO_IN_EP, 0x05, pkt,
                                 0x01, AUDIO_SAMPLE_FREQ_3B(rate)),
        USB_LANGID_INIT(1033),
        /* String 1: Manufacturer */
        0x14, USB_DESCRIPTOR_TYPE_STRING,
        'I', 0, 'N', 0, 'T', 0, 'E', 0, 'L', 0, 'P', 0, 'R', 0, 'O', 0, '7', 0,
        /* String 2: Product */
        0x1C, USB_DESCRIPTOR_TYPE_STRING,
        'I', 0, 'P', 0, 'R', 0, 'O', 0, '7', 0, ' ', 0,
        'U', 0, 'A', 0, 'C', 0, ' ', 0, 'O', 0, 'u', 0, 't', 0,
        /* String 3: Serial */
        0x16, USB_DESCRIPTOR_TYPE_STRING,
        '2', 0, '0', 0, '2', 0, '6', 0, '0', 0, '3', 0,
        '2', 0, '6', 0, '0', 0, '1', 0,
        0x00
    };

    out->usb_desc_len = sizeof(desc);
    if (out->usb_desc_len > sizeof(out->usb_desc)) {
        out->usb_desc_len = sizeof(out->usb_desc);
    }
    memcpy(out->usb_desc, desc, out->usb_desc_len);
}

/* ---------------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------------- */

static struct usbd_endpoint s_ep = {
    .ep_cb = usbd_audio_in_callback,
    .ep_addr = AUDIO_IN_EP,
};
static struct usbd_interface s_intf0, s_intf1;
static struct audio_entity_info s_entity[] = {
    { .bEntityId = AUDIO_IN_FU_ID,
      .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT,
      .ep = AUDIO_IN_EP },
};

audio_output_t *audio_output_uac_create(const audio_output_cfg_t *cfg)
{
    audio_output_t *out = pvPortMalloc(sizeof(audio_output_t));
    if (!out) return NULL;
    memset(out, 0, sizeof(*out));
    out->cfg = *cfg;

    /* Compute buffer sizes from config */
    uint32_t samples_per_seg = cfg->sample_rate * UAC_BUF_MS / 1000;
    out->buf_samples = samples_per_seg;
    out->buf_bytes = samples_per_seg * cfg->channels * sizeof(int16_t);
    out->total_samples = samples_per_seg * UAC_BUF_COUNT;
    out->packet_size = ((cfg->sample_rate + 999) / 1000) * sizeof(int16_t) * cfg->channels;

    /* Allocate DMA ring buffer (PSRAM, cache-aligned) */
    uint32_t ring_bytes = out->total_samples * cfg->channels * sizeof(int16_t);
    out->dma_buf_raw = pvPortMalloc(ring_bytes + 32);
    if (!out->dma_buf_raw) {
        vPortFree(out);
        return NULL;
    }
    out->dma_buf = (int16_t *)(((uintptr_t)out->dma_buf_raw + 31) & ~(uintptr_t)31);

    /* Staging buffer */
    out->staging = pvPortMalloc(out->buf_bytes);
    if (!out->staging) {
        vPortFree(out->dma_buf_raw);
        vPortFree(out);
        return NULL;
    }

    out->write_sem = xSemaphoreCreateBinary();
    if (!out->write_sem) {
        vPortFree(out->staging);
        vPortFree(out->dma_buf_raw);
        vPortFree(out);
        return NULL;
    }

    /* Build USB descriptor */
    build_usb_descriptor(out);

    printf("uac_out: %lu Hz, %d ch, %lu samples/seg, %lu bytes/seg\n",
           (unsigned long)cfg->sample_rate, cfg->channels,
           (unsigned long)out->buf_samples, (unsigned long)out->buf_bytes);

    return out;
}

int audio_output_uac_stop(audio_output_t *out);

void audio_output_uac_destroy(audio_output_t *out)
{
    if (!out) return;
    audio_output_uac_stop(out);
    if (out->write_sem) vSemaphoreDelete(out->write_sem);
    vPortFree(out->staging);
    vPortFree(out->dma_buf_raw);
    vPortFree(out);
    if (s_uac_out == out) s_uac_out = NULL;
}

int audio_output_uac_start(audio_output_t *out)
{
    if (!out || out->started) return -1;

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    /* Reserve a DMA channel for the USB IN endpoint before the USB stack
     * enumerates. The usbd_audio_open() callback (fired on SET_INTERFACE
     * from the host) needs a valid channel immediately. */
    out->dma_handle = dma_request_channel(DMA_REQUEST_USB_EP1,
                                          DMA_MEMORY_TO_PERIPH,
                                          "audio_uac");
    if (!out->dma_handle) {
        printf("uac_out: DMA channel request failed\n");
        return -1;
    }
    out->dma_ch = (uint8_t)dma_handle_to_ch(out->dma_handle);
#else
    out->dma_ch = UAC_DMA_CH_LEGACY;
#endif

    s_uac_out = out;
    out->running = 0;

    usbd_deinitialize();
    usbd_desc_register(out->usb_desc);
    usbd_add_interface(usbd_audio_init_intf(&s_intf0, 0x0100, s_entity, 1));
    usbd_add_interface(usbd_audio_init_intf(&s_intf1, 0x0100, s_entity, 1));
    usbd_add_endpoint(&s_ep);
    usbd_event_register_callback(uac_event_handler);
    usbd_initialize();

    out->started = 1;
    printf("uac_out: USB init (%lu Hz %dch, ch=%d), waiting for host\n",
           (unsigned long)out->cfg.sample_rate, out->cfg.channels,
           out->dma_ch);
    return 0;
}

int audio_output_uac_stop(audio_output_t *out)
{
    if (!out || !out->started) return 0;
    if (out->running) {
        dma_channel_stop(DMA0_ID, out->dma_ch);
        out->running = 0;
    }
    usbd_deinitialize();
    out->started = 0;

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    if (out->dma_handle) {
        dma_release_channel(out->dma_handle);
        out->dma_handle = NULL;
    }
#endif
    return 0;
}

int audio_output_uac_write(audio_output_t *out,
                           const int16_t *pcm, uint32_t samples)
{
    if (out && out->report_started) {
        out->report_started = 0;
        printf("uac_out: DMA started (%d x %lu samples, %dms/seg, ch=%d)\n",
               UAC_BUF_COUNT, (unsigned long)out->buf_samples, UAC_BUF_MS,
               out->dma_ch);
    }

    if (!out || !out->started || !out->running || samples == 0) return 0;

    /* PCM must already be at cfg.sample_rate and cfg.channels.
     * samples = number of frames (samples per channel). */
    uint32_t frame_size = out->cfg.channels;  /* int16_t per frame */
    uint32_t seg_frames = out->buf_samples;   /* frames per segment */

    uint32_t pos = 0;
    while (pos < samples) {
        uint32_t space = seg_frames - out->staging_count;
        uint32_t avail = samples - pos;
        uint32_t n = (avail < space) ? avail : space;

        memcpy(&out->staging[out->staging_count * frame_size],
               &pcm[pos * frame_size],
               n * frame_size * sizeof(int16_t));
        out->staging_count += n;
        pos += n;

        /* Full segment — commit to ring */
        if (out->staging_count == seg_frames) {
            while (ring_free(out) == 0) {
                xSemaphoreTake(out->write_sem, pdMS_TO_TICKS(100));
            }

            int16_t *dst = ring_seg(out, out->write_idx);
            memcpy(dst, out->staging, out->buf_bytes);
            L1C_DCACHE_CLEAN_RANGE(dst, out->buf_bytes);

            out->write_idx = (out->write_idx + 1) % UAC_BUF_COUNT;
            out->staging_count = 0;
        }
    }

    return 0;
}

/*
 * Samples the USB host has actually drained, since start().
 *
 * Counted from DMA segment completions, which are driven by the host's SOF
 * clock - not by how fast the producer pushes.  That independence is what lets
 * a caller measure the drift between the USB clock and its source clock.
 */
uint32_t audio_output_uac_consumed(audio_output_t *out)
{
    if (!out || out->cfg.mode == AUDIO_OUTPUT_MODE_ONESHOT) {
        return 0;
    }
    return out->segments_done * out->buf_samples;
}

uint32_t audio_output_uac_segment_samples(audio_output_t *out)
{
    return out ? out->buf_samples : 0;
}
