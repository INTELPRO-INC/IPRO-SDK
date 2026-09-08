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
 * @file main.c
 * @brief Meeting POC — DMIC + NPU Speaker Recognition + BLE streaming
 *
 * Captures audio from PDM DMIC, streams via BLE (LC3), and periodically
 * runs NPU speaker recognition to identify who is speaking.
 *
 * Speaker ID uses the shared speaker-embedding engine (components/ai
 * audio/speaker_embed): fbank -> res2dnet on NPU -> 256-d embedding, with
 * cosine matching against a RAM gallery. The model (mbv2.inm) is loaded
 * at boot from the 'mfg' flash partition (IProCube --ai-model), same as
 * ipro_pdm_mic_demo — it is NOT linked into the firmware.
 *
 * Original shell commands (from pdm_mic_demo):
 *   pdm_rec [seconds]   — record audio
 *   pdm_dump [off] [n]  — hex dump PCM samples
 *   pdm_stats           — show recording statistics
 *   pdm_gain <dB>       — set digital gain
 *   pdm_ch [left|right] — select PDM channel
 *   pdm_stream on|off   — real-time BLE streaming
 *   pdm_lc3 on|off      — toggle LC3 codec
 *
 * Voice ID commands:
 *   vi_start             — start voice ID (during streaming)
 *   vi_stop              — stop voice ID
 *   vi_reg <name>        — record 3s and register speaker
 *   vi_id [seconds]      — record and identify speaker
 *   vi_list              — list registered speakers
 *   vi_clear             — clear all speakers
 */

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <hal_auadc.h>
#include <hal_adc.h>
#include <system_soc.h>
#include <compiler/common.h>
#include <freertos_app_hooks.h>
#include "shell.h"
#include "ipro_log.h"
#include "ble_audio.h"

/* AI / NPU — speaker embedding engine + gallery */
#include "hal_boot2.h"
#include "hal_flash.h"
#include "drv_sf_ctrl.h"
#include "audio/speaker_embed.h"
#include "iproai_cycle_profile.h"

#ifdef CONFIG_MEETING_EMBED_MODEL
/* Provided by src/model_blob.S (.incbin). char[] so the linker symbols carry
 * no alignment promise the section does not actually make. */
extern const char g_embedded_model[];
extern const char g_embedded_model_end[];
#endif
#include "audio/gallery.h"

#define TAG "MEETING"

/* ===================================================================
 * Hardware / Audio configuration (pins + gain match ipro_pdm_mic_demo)
 * =================================================================== */
#define PDM_CLK_PIN         0       /* GPIO0 — PDM clock output */
#define PDM_DATA_PIN        1       /* GPIO1 — PDM data input */
#define SAMPLE_RATE         16000
#define FRAME_SAMPLES       320     /* 20 ms frame */
#define FRAME_BYTES         (FRAME_SAMPLES * sizeof(int16_t))
/* AUADC decimator gain, applied ahead of the 16-bit truncation — the only
 * stage that improves resolution rather than scaling what is already
 * quantised, so it runs at the +18 dB hardware maximum. Measured on the
 * product board: speech then sits at ~440 counts rms (-37.5 dBFS) with the
 * peak around -17 dBFS, i.e. 17 dB of headroom still in hand.
 *
 * This is NOT a free parameter. speaker_embed uses fixed INT8 quantisation
 * scales with no CMVN, so the absolute level shifts every log-mel feature by
 * a constant: enrolment and identification must run at the same gain, and
 * VI_VAD_RMS_THRESHOLD below has to be re-derived if this changes. */
#define PDM_DIGITAL_GAIN    18      /* dB — hardware max */
#define STARTUP_DISCARD     5

/* Recording buffer (PSRAM) */
#define MAX_RECORD_SECONDS  30
#define MAX_RECORD_BYTES    (SAMPLE_RATE * sizeof(int16_t) * MAX_RECORD_SECONDS)

/* Streaming */
#define STREAM_QUEUE_LEN    8

/* Voice ID — NPU accumulator */
#define VI_ACCUMULATE_SEC   3       /* the embedding model needs 3 s */
#define VI_ACCUMULATE_SAMPLES (SAMPLE_RATE * VI_ACCUMULATE_SEC)
#define VI_ACCUMULATE_BYTES (VI_ACCUMULATE_SAMPLES * sizeof(int16_t))
#define VI_TASK_PRIORITY    3       /* low priority — must not starve BLE streaming */
#define VI_TASK_STACK       8192    /* words (32 KB) — fbank + NPU overflow smaller stacks */
/* Must sit BETWEEN the noise floor and speech, and both scale with
 * PDM_DIGITAL_GAIN — so this constant is only meaningful paired with it.
 * Measured on the product board at +18 dB: noise floor ~150 counts rms,
 * speech ~550. 250 separates them with roughly 4 dB either side.
 *
 * The old value of 60 was paired with a 0 dB capture gain and does not work
 * on this hardware at either setting: at 0 dB speech itself only reaches
 * ~44 counts (so every frame reads as silence and inference never runs),
 * and at 18 dB the noise floor alone exceeds it (so it never gates at all).
 * Verify with pdm_stats after any gain change; tune live with vi_vad. */
#define VI_VAD_RMS_THRESHOLD 250    /* RMS below this = silence, skip inference
                                     * (default; tune at runtime with vi_vad) */

/* Phone-PCM registration robustness — write-without-response transfers can
 * lose their tail when the phone app skips flow control, so accept a stalled
 * transfer if enough audio arrived instead of hanging forever. */
#define PCM_REG_STALL_MS    2000    /* no chunk for this long = transfer stalled */
#define PCM_REG_MIN_BYTES   (SAMPLE_RATE * sizeof(int16_t) * 3 / 2)   /* 1.5 s */

/* Model blob location (pdm_mic_demo / pircam XIP pattern) */
#define MODEL_PARTITION     "mfg"
#define PART_HEADER_SIZE    0x1000

/* Persistent speaker gallery — gallery_c.bin blob in the (otherwise unused)
 * 'media' partition. 16 KB fits 15 speakers (16 + n*1088 bytes). Saves are
 * debounced and executed in voice_id_task (flash erase stalls XIP; never
 * run it in the BT RX thread). */
#define GALLERY_PARTITION        "media"
#define GALLERY_SAVE_DEBOUNCE_MS 1500

/* ===================================================================
 * PDM state (unchanged from pdm_mic_demo)
 * =================================================================== */
static int16_t *s_rec_buf;
static volatile uint32_t s_write_pos;
static volatile uint32_t s_target_bytes;
static volatile bool s_recording;
static volatile uint32_t s_discard_count;

static QueueHandle_t s_stream_queue;
static volatile bool s_streaming;

static int16_t *s_stream_ring;
static volatile uint32_t s_stream_widx;

static int16_t ATTR_NOCACHE_NOINIT_RAM_SECTION
    __attribute__((aligned(32))) s_dma_raw[FRAME_SAMPLES * 2];
static int16_t *s_dma_buf[2];

static SemaphoreHandle_t s_rec_done_sem;
static SemaphoreHandle_t s_cleanup_done_sem;

static uint8_t s_pdm_channel = HAL_AUADC_PDM_CH_LEFT;
static int s_pdm_gain = PDM_DIGITAL_GAIN;
static int s_rec_gain = PDM_DIGITAL_GAIN;   /* gain the current buffer was captured at */

static uint32_t s_frames_captured;
static int16_t s_peak_sample;
static int64_t s_energy_acc;

/* ===================================================================
 * Voice ID (NPU) state
 * =================================================================== */
static Gallery s_gallery;
static bool g_ai_initialized;

/* The embedding engine is not reentrant (shared fbank/NPU scratch) —
 * serialize all speaker_embed() callers. */
static SemaphoreHandle_t s_embed_mutex;

/* Guards s_gallery structure ops (identify scan, enroll, clear, list,
 * remove, rename, threshold write). Hold times are µs — safe to take from
 * the BT RX thread. Never held across speaker_embed()/NPU. */
static SemaphoreHandle_t s_gallery_mutex;

/* Gallery flash persistence state */
static uint32_t   s_gal_part_addr;      /* 0 = persistence unavailable */
static uint32_t   s_gal_part_size;
static volatile bool       s_gallery_dirty;
static volatile TickType_t s_gallery_dirty_tick;

/* NPU accumulator — collects PCM during streaming/meeting for periodic inference */
static int16_t *s_vi_accum_buf;          /* PSRAM ring buffer for accumulation */
static volatile uint32_t s_vi_accum_pos; /* write position (bytes) */
static volatile bool s_vi_enabled;       /* voice ID active */
static volatile bool s_vi_reg_pending;   /* request registration from voice_id_task */
static volatile bool s_vi_id_pending;    /* request one-shot identify from voice_id_task */
static volatile bool s_capture_only;     /* meeting mode: AUADC capture + VI, no BLE stream */
static int  s_vi_id_sec = VI_ACCUMULATE_SEC;
static int  s_vi_vad_rms = VI_VAD_RMS_THRESHOLD;
static char s_vi_reg_name[32];           /* name for pending registration */
static SemaphoreHandle_t s_vi_ready_sem; /* signals 3s accumulated OR reg/id request */
static SemaphoreHandle_t s_vi_op_done_sem; /* signals shell that a reg/id op finished */

/* Phone-side PCM registration state */
static volatile uint32_t s_pcm_reg_expected;  /* expected total bytes */
static volatile uint32_t s_pcm_reg_received;  /* bytes received so far */
static volatile bool s_pcm_reg_pending;       /* full PCM received, ready to process */
static char s_pcm_reg_name[32];               /* name for pending phone registration */
static volatile TickType_t s_pcm_reg_last_tick; /* tick of the last received chunk */
static volatile bool s_pcm_reg_resume_stream;   /* restore streaming when done */
static volatile bool s_pcm_reg_resume_vi;       /* restore voice ID when done */

/* Forward declarations */
void pdm_start_recording(int seconds);

/* ===================================================================
 * ISR callback: copy DMA frame to PSRAM + accumulate for voice ID
 * =================================================================== */
static void pdm_frame_callback(int buf_idx)
{
    BaseType_t woken = pdFALSE;

    L1C_DCACHE_INVALID_RANGE(s_dma_buf[buf_idx], FRAME_BYTES);
    int16_t *src = s_dma_buf[buf_idx];

    /* --- PDM -> DAC loopback: hand the frame to the playback path --- */
    extern void dac_test_feed_frame_isr(const int16_t *pcm, uint32_t bytes,
                                        BaseType_t *woken);
    dac_test_feed_frame_isr(src, FRAME_BYTES, &woken);

    /* --- Streaming mode: copy to ring buffer for BLE (skip in capture-only) --- */
    if (s_streaming && !s_capture_only && s_stream_queue && s_stream_ring) {
        uint32_t idx = s_stream_widx % STREAM_QUEUE_LEN;
        int16_t *dst = s_stream_ring + (idx * FRAME_SAMPLES);
        memcpy(dst, src, FRAME_BYTES);
        s_stream_widx++;
        xQueueSendFromISR(s_stream_queue, &dst, &woken);
    }

    /* --- Voice ID: accumulate PCM for periodic NPU inference --- */
    if (s_vi_enabled && s_vi_accum_buf) {
        uint32_t pos = s_vi_accum_pos;
        if (pos + FRAME_BYTES <= VI_ACCUMULATE_BYTES) {
            memcpy((uint8_t *)s_vi_accum_buf + pos, src, FRAME_BYTES);
            s_vi_accum_pos = pos + FRAME_BYTES;

            /* Signal when buffer is full */
            if (s_vi_accum_pos >= VI_ACCUMULATE_BYTES) {
                xSemaphoreGiveFromISR(s_vi_ready_sem, &woken);
            }
        }
    }

    /* --- Recording mode: copy to PSRAM recording buffer --- */
    if (s_recording) {
        if (s_discard_count > 0) {
            s_discard_count--;
            portYIELD_FROM_ISR(woken);
            return;
        }

        uint32_t remaining = s_target_bytes - s_write_pos;
        if (remaining == 0) {
            s_recording = false;
            xSemaphoreGiveFromISR(s_rec_done_sem, &woken);
            portYIELD_FROM_ISR(woken);
            return;
        }

        uint32_t copy_bytes = (remaining < FRAME_BYTES) ? remaining : FRAME_BYTES;
        memcpy((uint8_t *)s_rec_buf + s_write_pos, src, copy_bytes);
        s_write_pos += copy_bytes;
        s_frames_captured++;

        uint16_t n = copy_bytes / sizeof(int16_t);
        for (uint16_t i = 0; i < n; i++) {
            int16_t val = src[i];
            int16_t abs_val = (val >= 0) ? val : (int16_t)(-val);
            if (abs_val > s_peak_sample)
                s_peak_sample = abs_val;
            s_energy_acc += (int32_t)val * val;
        }

        if (s_write_pos >= s_target_bytes) {
            s_recording = false;
            xSemaphoreGiveFromISR(s_rec_done_sem, &woken);
        }
    }

    portYIELD_FROM_ISR(woken);
}

/* ===================================================================
 * PDM hardware init
 * =================================================================== */
static int pdm_hw_init(void)
{
    s_dma_buf[0] = &s_dma_raw[0];
    s_dma_buf[1] = &s_dma_raw[FRAME_SAMPLES];

    if (!s_rec_buf) {
        s_rec_buf = pvPortMalloc(MAX_RECORD_BYTES);
        if (!s_rec_buf) {
            IPRO_LOGE(TAG, "PSRAM rec buf alloc failed");
            return -1;
        }
    }

    hal_auadc_pdm_cfg_t cfg = {
        .pdm_clk_pin    = PDM_CLK_PIN,
        .pdm_in_pin     = PDM_DATA_PIN,
        .pdm_ch         = s_pdm_channel,
        .pcm_frame_size = FRAME_BYTES,
        .pcm_frame_buf  = {s_dma_buf[0], s_dma_buf[1]},
        .pcm_frame_event = pdm_frame_callback,
    };
    hal_auadc_pdm_init(&cfg);
    hal_auadc_set_digital_gain(s_pdm_gain);

    IPRO_LOGI(TAG, "PDM: %d Hz, ch=%s, gain=%d dB",
              SAMPLE_RATE,
              s_pdm_channel == HAL_AUADC_PDM_CH_LEFT ? "LEFT" : "RIGHT",
              s_pdm_gain);
    return 0;
}

/* ===================================================================
 * AI init — load mbv2.inm from the 'mfg' flash partition into the NPU
 * (pdm_mic_demo / pircam XIP pattern) and set up the RAM gallery.
 * =================================================================== */
static int load_model_from_flash(void)
{
    uint32_t part_addr = 0, part_size = 0;
#ifdef CONFIG_MEETING_EMBED_MODEL
    {
        const uint32_t n = (uint32_t)(g_embedded_model_end - g_embedded_model);
        IPRO_LOGI(TAG, "model linked into firmware: %u bytes", (unsigned)n);
        /* speaker_embed_init_buf() only reads during the call, and the blob is
         * already resident, so no copy is needed - unlike the flash path,
         * which has to stage it out of XIP. */
        return speaker_embed_init_buf((const uint8_t *)g_embedded_model);
    }
#endif

    if (hal_boot2_partition_addr_active(MODEL_PARTITION, &part_addr, &part_size) != 0) {
        IPRO_LOGE(TAG, "partition '%s' not found", MODEL_PARTITION);
        return -1;
    }

    /* Model blob starts after the 0x1000 partition header. Translate the flash
     * physical address to its XIP virtual address via the active image offset. */
    uint32_t model_flash = part_addr + PART_HEADER_SIZE;
    uint32_t sf_off = SF_Ctrl_Get_Flash_Image_Offset(0, 0);
    if (model_flash < sf_off) {
        IPRO_LOGE(TAG, "phys 0x%x < sf_off 0x%x", (unsigned)model_flash, (unsigned)sf_off);
        return -1;
    }
    const uint8_t *xip = (const uint8_t *)(uintptr_t)(DRV_FLASH_XIP_BASE + (model_flash - sf_off));

    /* First 32 bytes = 8 x uint32 section lengths; total = sum + 32. */
    uint32_t header[8];
    memcpy(header, xip, sizeof(header));
    uint32_t sum = 0;
    for (int i = 0; i < 8; i++) sum += header[i];
    uint32_t model_size = sum + 32;
    if (model_size == 32 || model_size + PART_HEADER_SIZE > part_size) {
        IPRO_LOGE(TAG, "bad model size %u (part %u) — is mbv2.inm flashed to '%s'?",
                  (unsigned)model_size, (unsigned)part_size, MODEL_PARTITION);
        return -1;
    }
    IPRO_LOGI(TAG, "model in '%s': base=0x%x size=%u", MODEL_PARTITION,
              (unsigned)part_addr, (unsigned)model_size);

    uint8_t *buf = (uint8_t *)pvPortMalloc(model_size);
    if (!buf) { IPRO_LOGE(TAG, "alloc %u failed", (unsigned)model_size); return -1; }
    memcpy(buf, xip, model_size);
    int rc = speaker_embed_init_buf(buf);
    vPortFree(buf);
    return rc;
}

/* ---- gallery flash persistence ('media' partition, gallery_c.bin) ---- */

static void gallery_flash_load(void)
{
    if (hal_boot2_partition_addr_active(GALLERY_PARTITION,
                                        &s_gal_part_addr, &s_gal_part_size) != 0) {
        IPRO_LOGW(TAG, "no '%s' partition — speaker gallery won't persist",
                  GALLERY_PARTITION);
        s_gal_part_addr = 0;
        return;
    }
    uint32_t sf_off = SF_Ctrl_Get_Flash_Image_Offset(0, 0);
    if (s_gal_part_addr < sf_off) {
        s_gal_part_addr = 0;
        return;
    }
    const uint8_t *xip = (const uint8_t *)(uintptr_t)
        (DRV_FLASH_XIP_BASE + (s_gal_part_addr - sf_off));

    uint32_t magic;
    memcpy(&magic, xip, sizeof(magic));
    if (magic != GALLERY_MAGIC) {
        IPRO_LOGI(TAG, "no saved gallery in '%s' — starting empty", GALLERY_PARTITION);
        return;
    }
    if (gallery_load_from_buf(&s_gallery, xip, s_gal_part_size) != 0) {
        IPRO_LOGW(TAG, "saved gallery invalid — starting empty");
        gallery_init(&s_gallery, GALLERY_DEFAULT_THRESHOLD);
    } else {
        IPRO_LOGI(TAG, "Gallery restored from flash: %d speaker(s), threshold %.2f",
                  s_gallery.n, (double)s_gallery.threshold);
    }
}

/* Runs ONLY in voice_id_task. Snapshot under the mutex (fast memcpy), then
 * erase+write outside it — flash ops stall XIP for tens of ms. */
static void gallery_flash_save(void)
{
    if (!s_gal_part_addr)
        return;

    static uint8_t *save_buf;
    if (!save_buf) {
        save_buf = pvPortMalloc(s_gal_part_size);
        if (!save_buf) {
            IPRO_LOGE(TAG, "gallery save buf alloc failed");
            return;
        }
    }

    int max_spk = (int)((s_gal_part_size - 16) /
                        (GALLERY_NAME_LEN + EMBEDDING_DIM * 4));
    if (max_spk > GALLERY_MAX_SPEAKERS)
        max_spk = GALLERY_MAX_SPEAKERS;

    xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
    if (s_gallery.n > max_spk)
        IPRO_LOGW(TAG, "%d speakers but only %d fit in '%s' — extras not persisted",
                  s_gallery.n, max_spk, GALLERY_PARTITION);
    int len = gallery_save_to_buf(&s_gallery, save_buf, s_gal_part_size, max_spk);
    xSemaphoreGive(s_gallery_mutex);
    if (len < 0)
        return;

    TickType_t t0 = xTaskGetTickCount();
    uint32_t erase_len = ((uint32_t)len + 4095u) & ~4095u;
    flash_erase(s_gal_part_addr, erase_len);
    flash_write(s_gal_part_addr, save_buf, (uint32_t)len);
    IPRO_LOGI(TAG, "Gallery saved: %d B (%lu ms)",
              len, (unsigned long)(xTaskGetTickCount() - t0));
}

/* Callable from any task — the actual flash write happens debounced in
 * voice_id_task. */
static void gallery_mark_dirty(void)
{
    s_gallery_dirty_tick = xTaskGetTickCount();
    s_gallery_dirty = true;
    if (s_vi_ready_sem)
        xSemaphoreGive(s_vi_ready_sem);   /* nudge the task awake */
}

static int ai_init(void)
{
    if (g_ai_initialized)
        return 0;

    if (load_model_from_flash() != 0)
        return -1;

    gallery_init(&s_gallery, GALLERY_DEFAULT_THRESHOLD);
    gallery_flash_load();

    s_embed_mutex = xSemaphoreCreateMutex();
    if (!s_embed_mutex) {
        IPRO_LOGE(TAG, "embed mutex alloc failed");
        return -1;
    }

    s_vi_accum_buf = pvPortMalloc(VI_ACCUMULATE_BYTES);
    if (!s_vi_accum_buf) {
        IPRO_LOGE(TAG, "VI accum buf alloc failed");
        return -1;
    }

    g_ai_initialized = true;
    IPRO_LOGI(TAG, "AI ready: speaker-embed engine, threshold %.2f (heap free: %zu)",
              (double)GALLERY_DEFAULT_THRESHOLD, xPortGetFreeHeapSize());
    return 0;
}

/* speaker_embed() is not reentrant — every caller goes through this. */
static int embed_pcm(const int16_t *pcm, int nsamp, float emb[EMBEDDING_DIM])
{
    xSemaphoreTake(s_embed_mutex, portMAX_DELAY);
    int rc = speaker_embed(pcm, nsamp, emb);
    xSemaphoreGive(s_embed_mutex);
    return rc;
}

static int gallery_find(const char *name)
{
    for (int i = 0; i < s_gallery.n; i++)
        if (strcmp(s_gallery.names[i], name) == 0)
            return i;
    return -1;
}

/* Integer RMS over the window (every 8th sample) — energy VAD. */
static uint32_t pcm_rms(const int16_t *pcm, int n)
{
    int64_t energy = 0;
    for (int i = 0; i < n; i += 8)
        energy += (int32_t)pcm[i] * pcm[i];
    uint64_t mean_sq = (uint64_t)(energy / (n / 8));
    uint32_t r = (uint32_t)mean_sq;
    if (!r) return 0;
    for (int j = 0; j < 15; j++)
        r = (r + (uint32_t)(mean_sq / r)) / 2;
    return r;
}

/* Close out a phone-PCM registration (success or failure): clear the
 * transfer state and restore whatever capture ble_reg_pcm_start() stopped. */
static void pcm_reg_finish(void)
{
    s_pcm_reg_expected = 0;
    s_pcm_reg_received = 0;
    if (s_pcm_reg_resume_stream) {
        if (s_stream_queue) xQueueReset(s_stream_queue);
        s_streaming = true;
        hal_auadc_start();
        if (s_pcm_reg_resume_vi) {
            s_vi_accum_pos = 0;
            s_vi_enabled = true;
        }
        IPRO_LOGI(TAG, "[PCM REG] Capture restored (vi=%d)", (int)s_pcm_reg_resume_vi);
    }
    s_pcm_reg_resume_stream = false;
    s_pcm_reg_resume_vi = false;
}

/* ===================================================================
 * Voice ID task — all speaker_embed() calls run here (32 KB stack).
 * Handles: periodic identification during streaming, DMIC registration /
 * one-shot identify (shell + BLE), and phone-PCM registration.
 * =================================================================== */
static void voice_id_task(void *arg)
{
    (void)arg;

    IPRO_LOGI(TAG, "[VI] Voice ID task started");

    for (;;) {
        BaseType_t got = xSemaphoreTake(s_vi_ready_sem, pdMS_TO_TICKS(500));

        /* Debounced gallery persistence — flash erase stalls XIP, so the
         * write runs here, never in the BT RX thread. */
        if (s_gallery_dirty &&
            (xTaskGetTickCount() - s_gallery_dirty_tick) >=
                pdMS_TO_TICKS(GALLERY_SAVE_DEBOUNCE_MS)) {
            s_gallery_dirty = false;
            gallery_flash_save();
        }

        if (got != pdTRUE) {
            /* Timed out — check for a stalled phone-PCM transfer (phone-side
             * write-without-response drops leave the transfer incomplete). */
            if (s_pcm_reg_expected == 0 || s_pcm_reg_pending)
                continue;
            if ((xTaskGetTickCount() - s_pcm_reg_last_tick) < pdMS_TO_TICKS(PCM_REG_STALL_MS))
                continue;
            IPRO_LOGW(TAG, "[PCM REG] Transfer stalled at %lu / %lu bytes",
                      (unsigned long)s_pcm_reg_received,
                      (unsigned long)s_pcm_reg_expected);
            if (s_pcm_reg_received >= PCM_REG_MIN_BYTES) {
                IPRO_LOGW(TAG, "[PCM REG] %.1f s received — registering with partial data",
                          (float)(s_pcm_reg_received / sizeof(int16_t)) / SAMPLE_RATE);
                s_pcm_reg_pending = true;   /* fall through to the block below */
            } else {
                ble_audio_notify_speaker_id(-1, 0.0f, "xfer_failed");
                pcm_reg_finish();
                continue;
            }
        }

        /* Handle phone PCM registration (high-quality audio from phone mic) */
        if (s_pcm_reg_pending) {
            s_pcm_reg_pending = false;

            if (!g_ai_initialized) {
                pcm_reg_finish();
                continue;
            }

            int samples = s_pcm_reg_received / sizeof(int16_t);
            IPRO_LOGI(TAG, "[PCM REG] Processing %d samples for '%s'",
                      samples, s_pcm_reg_name);

            uint32_t rms = pcm_rms(s_rec_buf, samples);
            if (rms < (uint32_t)s_vi_vad_rms) {
                /* Quality gate: a noise-floor clip pollutes the gallery's
                 * running mean — refuse it. */
                IPRO_LOGW(TAG, "[PCM REG] Too quiet (rms=%lu < %d) — not enrolled",
                          (unsigned long)rms, s_vi_vad_rms);
                ble_audio_notify_speaker_id(-1, 0.0f, "too_quiet");
                pcm_reg_finish();
                continue;
            }

            float emb[EMBEDDING_DIM];
            if (embed_pcm(s_rec_buf, samples, emb) == 0) {
                xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
                int clips = gallery_enroll(&s_gallery, s_pcm_reg_name, emb);
                int id = (clips >= 1) ? gallery_find(s_pcm_reg_name) : -1;
                xSemaphoreGive(s_gallery_mutex);
                if (clips >= 1) {
                    IPRO_LOGI(TAG, "[PCM REG] '%s' enrolled (slot %d, %d clip%s)",
                              s_pcm_reg_name, id, clips, clips == 1 ? "" : "s");
                    gallery_mark_dirty();
                    ble_audio_notify_speaker_id(id, 1.0f, s_pcm_reg_name);
                } else {
                    IPRO_LOGW(TAG, "[PCM REG] Gallery full");
                    ble_audio_notify_speaker_id(-1, 0.0f, "reg_failed");
                }
            } else {
                IPRO_LOGW(TAG, "[PCM REG] Embedding failed");
                ble_audio_notify_speaker_id(-1, 0.0f, "inf_failed");
            }

            pcm_reg_finish();
            continue;
        }

        /* Handle DMIC registration / one-shot identify (shell or BLE) */
        if (s_vi_reg_pending || s_vi_id_pending) {
            bool is_reg = s_vi_reg_pending;
            int sec = is_reg ? VI_ACCUMULATE_SEC : s_vi_id_sec;
            s_vi_reg_pending = false;
            s_vi_id_pending = false;

            if (!g_ai_initialized) {
                /* Only reachable via BLE — cmd_vi_reg/vi_id check readiness first. */
                if (is_reg)
                    ble_audio_notify_speaker_id(-1, 0.0f, "not_ready");
                xSemaphoreGive(s_vi_op_done_sem);
                continue;
            }

            /* Stop streaming temporarily for clean recording */
            bool was_streaming = s_streaming;
            bool was_vi = s_vi_enabled;
            if (was_streaming) {
                s_vi_enabled = false;
                s_streaming = false;
                hal_auadc_stop();
                vTaskDelay(pdMS_TO_TICKS(20));
            }

            IPRO_LOGI(TAG, "[VI] Recording %d s for %s...", sec,
                      is_reg ? s_vi_reg_name : "identification");
            xSemaphoreTake(s_cleanup_done_sem, 0);
            pdm_start_recording(sec);
            /* s_recording is now true — the 0x81 RECORDING status is the
             * phone's "speak to the device now" cue. */
            if (is_reg)
                ble_audio_notify_status();
            xSemaphoreTake(s_cleanup_done_sem, pdMS_TO_TICKS(sec * 1000 + 2000));

            int samples = (int)(s_write_pos / sizeof(int16_t));
            uint32_t clip_rms = (samples > 8) ? pcm_rms(s_rec_buf, samples) : 0;
            if (samples >= SAMPLE_RATE / 2 &&
                is_reg && clip_rms < (uint32_t)s_vi_vad_rms) {
                /* Quality gate: refuse to enroll near-silence. */
                printf("Too quiet (rms=%lu < %d) — '%s' NOT enrolled; "
                       "speak louder or raise pdm_gain\r\n",
                       (unsigned long)clip_rms, s_vi_vad_rms, s_vi_reg_name);
                ble_audio_notify_speaker_id(-1, 0.0f, "too_quiet");
            } else if (samples >= SAMPLE_RATE / 2) {
                float emb[EMBEDDING_DIM];
                if (embed_pcm(s_rec_buf, samples, emb) != 0) {
                    printf("[VI] embed failed\r\n");
                    if (is_reg)
                        ble_audio_notify_speaker_id(-1, 0.0f, "inf_failed");
                } else if (is_reg) {
                    xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
                    int clips = gallery_enroll(&s_gallery, s_vi_reg_name, emb);
                    int id = (clips >= 1) ? gallery_find(s_vi_reg_name) : -1;
                    xSemaphoreGive(s_gallery_mutex);
                    if (clips >= 1) {
                        printf("Speaker '%s' registered at slot %d (%d clip%s)\r\n",
                               s_vi_reg_name, id, clips, clips == 1 ? "" : "s");
                        gallery_mark_dirty();
                        ble_audio_notify_speaker_id(id, 1.0f, s_vi_reg_name);
                    } else {
                        printf("Registration failed: gallery full\r\n");
                        ble_audio_notify_speaker_id(-1, 0.0f, "reg_failed");
                    }
                } else {
                    float score = 0.0f;
                    char who[GALLERY_NAME_LEN];
                    /* Copy the name under the lock — a concurrent delete
                     * could shift names[] between identify and notify. */
                    xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
                    int idx = gallery_identify(&s_gallery, emb, &score);
                    strncpy(who, idx >= 0 ? s_gallery.names[idx] : "unknown",
                            sizeof(who) - 1);
                    who[sizeof(who) - 1] = '\0';
                    xSemaphoreGive(s_gallery_mutex);
                    printf("\r\n=== Voice ID Result ===\r\n");
                    if (idx >= 0)
                        printf("ACCEPT speaker=%s score=%.3f\r\n", who, (double)score);
                    else
                        printf("REJECT (best score=%.3f)\r\n", (double)score);
                    ble_audio_notify_speaker_id(idx, score, who);
                }
            } else {
                IPRO_LOGW(TAG, "[VI] Not enough audio (%d samples)", samples);
                if (is_reg)
                    ble_audio_notify_speaker_id(-1, 0.0f, "no_audio");
            }

            /* Restore streaming (and voice ID) if they were active */
            if (was_streaming) {
                if (s_stream_queue) xQueueReset(s_stream_queue);
                s_streaming = true;
                hal_auadc_start();
                if (was_vi) {
                    s_vi_accum_pos = 0;
                    s_vi_enabled = true;
                }
                IPRO_LOGI(TAG, "[VI] Streaming restored (vi=%d)", (int)was_vi);
            }
            xSemaphoreGive(s_vi_op_done_sem);
            continue;
        }

        if (!s_vi_enabled || !g_ai_initialized)
            continue;

        /* --- Simple energy-based VAD: skip inference if silence --- */
        uint32_t rms = pcm_rms(s_vi_accum_buf, VI_ACCUMULATE_SAMPLES);
        if (rms < (uint32_t)s_vi_vad_rms) {
            IPRO_LOGI(TAG, "[VI] Silence (rms=%lu), skip", (unsigned long)rms);
            s_vi_accum_pos = 0;
            continue;
        }
        IPRO_LOGI(TAG, "[VI] Speech detected (rms=%lu), inferring...",
                  (unsigned long)rms);

        TickType_t t0 = xTaskGetTickCount();
        float emb[EMBEDDING_DIM];
        int rc = embed_pcm(s_vi_accum_buf, VI_ACCUMULATE_SAMPLES, emb);
        TickType_t t1 = xTaskGetTickCount();

        /* Buffer no longer needed — start collecting the next 3 s window */
        s_vi_accum_pos = 0;

        if (rc != 0) {
            IPRO_LOGW(TAG, "[VI] Embedding failed");
            continue;
        }

        float score = 0.0f;
        char who[GALLERY_NAME_LEN];
        xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
        int idx = gallery_identify(&s_gallery, emb, &score);
        strncpy(who, idx >= 0 ? s_gallery.names[idx] : "unknown", sizeof(who) - 1);
        who[sizeof(who) - 1] = '\0';
        xSemaphoreGive(s_gallery_mutex);
        IPRO_LOGI(TAG, "[VI] %s score=%.3f rms=%lu (embed %lu ms)",
                  who, (double)score,
                  (unsigned long)rms, (unsigned long)(t1 - t0));
        ble_audio_notify_speaker_id(idx, score, who);
    }
}

/* ===================================================================
 * Recording monitor task
 * =================================================================== */
static void pdm_monitor_task(void *arg)
{
    (void)arg;
    for (;;) {
        xSemaphoreTake(s_rec_done_sem, portMAX_DELAY);
        hal_auadc_stop();
        L1C_DCACHE_CLEAN_RANGE(s_rec_buf, s_write_pos);
        IPRO_LOGI(TAG, "Recording complete: %lu bytes",
                  (unsigned long)s_write_pos);
        ble_audio_notify_recording_done();
        xSemaphoreGive(s_cleanup_done_sem);
    }
}

/* ===================================================================
 * Public PDM APIs (unchanged)
 * =================================================================== */
void pdm_start_recording(int seconds)
{
    if (s_recording) return;
    if (seconds < 1) seconds = 1;
    if (seconds > MAX_RECORD_SECONDS) seconds = MAX_RECORD_SECONDS;

    s_write_pos = 0;
    s_target_bytes = SAMPLE_RATE * sizeof(int16_t) * seconds;
    s_frames_captured = 0;
    s_peak_sample = 0;
    s_energy_acc = 0;
    s_discard_count = STARTUP_DISCARD;
    /* Latch the gain this take was captured at: pdm_play must report the
     * gain the samples actually carry, not whatever pdm_gain was changed to
     * afterwards. It also matters for speaker ID — enrolment and
     * identification have to be at the same gain (see PDM_DIGITAL_GAIN). */
    s_rec_gain = s_pdm_gain;
    memset(s_rec_buf, 0, s_target_bytes);
    hal_auadc_set_digital_gain(s_pdm_gain);

    s_recording = true;
    hal_auadc_start();
}

void pdm_stop_recording(void)
{
    if (!s_recording) return;
    hal_auadc_stop();
    s_recording = false;
    L1C_DCACHE_CLEAN_RANGE(s_rec_buf, s_write_pos);
}

const int16_t *pdm_get_buffer(void)       { return s_rec_buf; }
uint32_t pdm_get_recorded_bytes(void)     { return s_write_pos; }
bool pdm_is_recording(void)               { return s_recording; }
void pdm_set_gain(int gain_db)            { s_pdm_gain = gain_db; }
int  pdm_get_gain(void)                   { return s_pdm_gain; }
int  pdm_get_rec_gain(void)               { return s_rec_gain; }

void pdm_start_streaming(void)
{
    if (s_recording) return;

    /* If already streaming, restart (handles phone state desync) */
    if (s_streaming) {
        IPRO_LOGI(TAG, "Streaming restart (was already active)");
        s_vi_enabled = false;
        hal_auadc_stop();
        s_streaming = false;
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    if (s_stream_queue) xQueueReset(s_stream_queue);
    hal_auadc_set_digital_gain(s_pdm_gain);
    s_streaming = true;
    hal_auadc_start();
    IPRO_LOGI(TAG, "Streaming started");
}

void pdm_stop_streaming(void)
{
    if (!s_streaming) return;
    s_streaming = false;
    s_vi_enabled = false;
    hal_auadc_stop();
    IPRO_LOGI(TAG, "Streaming stopped");
}

bool pdm_is_streaming(void)               { return s_streaming; }

/* ===================================================================
 * Shell commands — original PDM commands
 * =================================================================== */
static int cmd_pdm_rec(int argc, char **argv)
{
    int seconds = 5;
    if (argc > 1) seconds = atoi(argv[1]);
    if (seconds < 1) seconds = 1;
    if (seconds > MAX_RECORD_SECONDS) seconds = MAX_RECORD_SECONDS;
    if (s_recording) { printf("Already recording!\r\n"); return 0; }

    xSemaphoreTake(s_cleanup_done_sem, 0);
    pdm_start_recording(seconds);
    printf("Recording %d sec...\r\n", seconds);

    if (xSemaphoreTake(s_cleanup_done_sem, pdMS_TO_TICKS(seconds * 1000 + 2000)) == pdTRUE) {
        printf("Done: %lu bytes, %lu frames\r\n",
               (unsigned long)s_write_pos, (unsigned long)s_frames_captured);
    } else {
        pdm_stop_recording();
        printf("Timeout!\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_rec, pdm_rec, Record PDM audio [seconds]);

static int cmd_pdm_dump(int argc, char **argv)
{
    uint32_t offset = 0, count = 64;
    if (argc > 1) offset = (uint32_t)strtoul(argv[1], NULL, 0);
    if (argc > 2) count  = (uint32_t)strtoul(argv[2], NULL, 0);
    uint32_t total = s_write_pos / sizeof(int16_t);
    if (offset >= total) { printf("No data\r\n"); return 0; }
    if (offset + count > total) count = total - offset;
    for (uint32_t i = 0; i < count; i++) {
        printf("%6d ", s_rec_buf[offset + i]);
        if ((i + 1) % 10 == 0) printf("\r\n");
    }
    if (count % 10 != 0) printf("\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_dump, pdm_dump, Dump PCM samples [offset] [count]);

static int cmd_pdm_stats(int argc, char **argv)
{
    (void)argc; (void)argv;
    uint32_t total_samples = s_write_pos / sizeof(int16_t);
    printf("=== PDM Stats ===\r\n");
    printf("Recorded: %lu bytes (%.2f sec)\r\n",
           (unsigned long)s_write_pos, (float)total_samples / SAMPLE_RATE);
    printf("Peak: %d, Frames: %lu\r\n", s_peak_sample, (unsigned long)s_frames_captured);
    if (total_samples > 0 && s_peak_sample > 0)
        printf("Peak dBFS: %.1f\r\n", 20.0f * log10f((float)s_peak_sample / 32767.0f));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_stats, pdm_stats, Show recording statistics);

static int cmd_pdm_gain(int argc, char **argv)
{
    if (argc < 2) { printf("Gain: %d dB\r\n", s_pdm_gain); return 0; }
    s_pdm_gain = atoi(argv[1]);
    printf("Gain set to %d dB\r\n", s_pdm_gain);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_gain, pdm_gain, Set PDM digital gain <dB>);

static int cmd_pdm_ch(int argc, char **argv)
{
    if (argc < 2) {
        printf("Channel: %s\r\n", s_pdm_channel == HAL_AUADC_PDM_CH_LEFT ? "LEFT" : "RIGHT");
        return 0;
    }
    s_pdm_channel = (strcmp(argv[1], "right") == 0 || strcmp(argv[1], "r") == 0)
                    ? HAL_AUADC_PDM_CH_RIGHT : HAL_AUADC_PDM_CH_LEFT;
    printf("Channel: %s (next rec/stream)\r\n",
           s_pdm_channel == HAL_AUADC_PDM_CH_LEFT ? "LEFT" : "RIGHT");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_ch, pdm_ch, Select PDM channel left|right);

static int cmd_pdm_stream(int argc, char **argv)
{
    if (argc < 2) { printf("Streaming: %s\r\n", s_streaming ? "ON" : "OFF"); return 0; }
    if (strcmp(argv[1], "on") == 0) pdm_start_streaming();
    else pdm_stop_streaming();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_stream, pdm_stream, Start/stop streaming on|off);

static int cmd_pdm_lc3(int argc, char **argv)
{
    if (argc < 2) { printf("LC3: %s\r\n", ble_audio_get_lc3() ? "ON" : "OFF"); return 0; }
    ble_audio_set_lc3(strcmp(argv[1], "on") == 0);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_lc3, pdm_lc3, Toggle LC3 codec on|off);

/* ===================================================================
 * Shell commands — Voice ID
 * All embedding work runs in voice_id_task (32 KB stack) — fbank + NPU
 * would overflow the shell exec thread's stack, so the commands below
 * only set request flags and block on the op-done semaphore.
 * =================================================================== */

/* vi_start — enable voice ID during streaming */
static int cmd_vi_start(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (!g_ai_initialized) { printf("AI not ready\r\n"); return -1; }
    if (!s_streaming) { printf("Start streaming first (pdm_stream on)\r\n"); return -1; }
    s_vi_accum_pos = 0;
    s_vi_enabled = true;
    printf("Voice ID enabled (every %d sec)\r\n", VI_ACCUMULATE_SEC);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_vi_start, vi_start, Enable voice ID during streaming);

/* ---------------------------------------------------------------------------
 * ai_cycles — how much of a .inm inference is the CPU parked on the NPU?
 *
 * That share is the concurrency budget for anything co-running on the core:
 * iproai_npu_inference() blocks on a FreeRTOS semaphore rather than spinning,
 * so every cycle it reports as NPU-busy is a cycle another task can use. This
 * is the number that decides whether running a TFLM model alongside the .inm
 * one buys anything, or whether the two would simply contend.
 *
 * Needs CONFIG_AI_CYCLE_PROFILE=y; prints a reminder otherwise.
 * ---------------------------------------------------------------------------*/
#ifdef CONFIG_AI_CYCLE_PROFILE
/* Runs on its own task, not on the shell exec thread. speaker_embed() does
 * fbank + NPU dispatch and needs the same ~32 KB the voice_id_task gets;
 * SHELL_EXEC_THREAD_STACK_SIZE is 1024 words, so calling it inline overflows
 * before the first inference completes. The banner at the top of the Voice ID
 * command block says exactly this - those commands set a request flag and let
 * voice_id_task do the work. This one is a bench tool that should not perturb
 * that state machine, so it spawns a throwaway worker instead. */
static int      s_aic_runs;
static SemaphoreHandle_t s_aic_done;

static void ai_cycles_task(void *arg)
{
    (void)arg;

    const int nsamp = SAMPLE_RATE * 3;   /* speaker_embed caps at 3 s */
    int16_t *pcm = pvPortMalloc((size_t)nsamp * sizeof(int16_t));
    float   *emb = pvPortMalloc(EMBEDDING_DIM * sizeof(float));

    if (pcm == NULL || emb == NULL) {
        printf("ai_cycles: OOM (%d samples)\r\n", nsamp);
    } else {
        /* Deterministic non-silent input; the graph is data-independent, so
         * content does not affect the cycle split. */
        int32_t seed = 12345;
        for (int i = 0; i < nsamp; i++) {
            seed = seed * 1103515245 + 12345;
            pcm[i] = (int16_t)(seed >> 18);
        }

        iproai_cycle_reset();
        for (int i = 0; i < s_aic_runs; i++) {
            if (embed_pcm(pcm, nsamp, emb) != 0) {
                printf("ai_cycles: embed failed at run %d\r\n", i);
                break;
            }
        }
        printf("\r\n");
        iproai_cycle_report();
        printf("  worker stack free: %u words\r\n",
               (unsigned)uxTaskGetStackHighWaterMark(NULL));
    }

    vPortFree(pcm);
    vPortFree(emb);
    xSemaphoreGive(s_aic_done);
    vTaskDelete(NULL);
}
#endif

static int cmd_ai_cycles(int argc, char **argv)
{
#ifndef CONFIG_AI_CYCLE_PROFILE
    (void)argc; (void)argv;
    printf("ai_cycles: rebuild with CONFIG_AI_CYCLE_PROFILE=y\r\n");
    return -1;
#else
    if (!g_ai_initialized) {
        printf("ai_cycles: AI not ready - flash mbv2.inm to the 'mfg' partition\r\n");
        return -1;
    }

    s_aic_runs = (argc > 1) ? atoi(argv[1]) : 5;
    if (s_aic_runs < 1)  s_aic_runs = 1;
    if (s_aic_runs > 50) s_aic_runs = 50;

    if (s_aic_done == NULL) {
        s_aic_done = xSemaphoreCreateBinary();
        if (s_aic_done == NULL) { printf("ai_cycles: sem alloc failed\r\n"); return -1; }
    }

    printf("ai_cycles: %d inferences of %d samples (%d s)...\r\n",
           s_aic_runs, SAMPLE_RATE * 3, 3);

    /* VI_TASK_STACK words, same as voice_id_task. Priority below it so a live
     * streaming session still wins. */
    if (xTaskCreate(ai_cycles_task, "ai_cycles", VI_TASK_STACK, NULL,
                    VI_TASK_PRIORITY, NULL) != pdPASS) {
        printf("ai_cycles: task create failed (heap free: %zu)\r\n",
               xPortGetFreeHeapSize());
        return -1;
    }

    xSemaphoreTake(s_aic_done, portMAX_DELAY);
    return 0;
#endif
}
SHELL_CMD_EXPORT_ALIAS(cmd_ai_cycles, ai_cycles, NPU/CPU cycle split [runs]);

/* vi_stop — disable voice ID */
static int cmd_vi_stop(int argc, char **argv)
{
    (void)argc; (void)argv;
    s_vi_enabled = false;
    printf("Voice ID disabled\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_vi_stop, vi_stop, Disable voice ID);

/* vi_id [seconds] — record and identify (runs in voice_id_task) */
static int cmd_vi_id(int argc, char **argv)
{
    if (!g_ai_initialized) { printf("AI not ready\r\n"); return -1; }
    int seconds = VI_ACCUMULATE_SEC;
    if (argc > 1) seconds = atoi(argv[1]);
    if (seconds < 1) seconds = 1;
    if (seconds > 5) seconds = 5;

    s_vi_id_sec = seconds;
    xSemaphoreTake(s_vi_op_done_sem, 0);
    s_vi_id_pending = true;
    xSemaphoreGive(s_vi_ready_sem);
    printf("Recording %d sec for identification...\r\n", seconds);

    if (xSemaphoreTake(s_vi_op_done_sem, pdMS_TO_TICKS(seconds * 1000 + 10000)) != pdTRUE) {
        printf("Timeout\r\n");
        return -1;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_vi_id, vi_id, Record and identify speaker [seconds]);

/* vi_reg <name> — record and register (runs in voice_id_task) */
static int cmd_vi_reg(int argc, char **argv)
{
    if (!g_ai_initialized) { printf("AI not ready\r\n"); return -1; }
    if (argc < 2) { printf("Usage: vi_reg <name>\r\n"); return -1; }

    strncpy(s_vi_reg_name, argv[1], sizeof(s_vi_reg_name) - 1);
    s_vi_reg_name[sizeof(s_vi_reg_name) - 1] = '\0';
    xSemaphoreTake(s_vi_op_done_sem, 0);
    s_vi_reg_pending = true;
    xSemaphoreGive(s_vi_ready_sem);
    printf("Recording %d sec for registration...\r\n", VI_ACCUMULATE_SEC);

    if (xSemaphoreTake(s_vi_op_done_sem,
                       pdMS_TO_TICKS(VI_ACCUMULATE_SEC * 1000 + 10000)) != pdTRUE) {
        printf("Timeout\r\n");
        return -1;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_vi_reg, vi_reg, Record and register speaker <name>);

/* vi_list — list speakers */
static void vi_print_speakers(void)
{
    if (!g_ai_initialized || s_gallery.n == 0) {
        printf("[VI] gallery empty\r\n");
        return;
    }
    xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
    for (int i = 0; i < s_gallery.n; i++)
        printf("  %d: %s (%d clip%s)\r\n", i, s_gallery.names[i],
               s_gallery.count[i], s_gallery.count[i] == 1 ? "" : "s");
    xSemaphoreGive(s_gallery_mutex);
}

static int cmd_vi_list(int argc, char **argv)
{
    (void)argc; (void)argv;
    vi_print_speakers();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_vi_list, vi_list, List registered speakers);

/* vi_vad [rms] — show/set the energy-VAD threshold */
static int cmd_vi_vad(int argc, char **argv)
{
    if (argc < 2) {
        printf("VAD rms threshold: %d (skip inference below this)\r\n", s_vi_vad_rms);
        return 0;
    }
    s_vi_vad_rms = atoi(argv[1]);
    printf("VAD rms threshold set to %d\r\n", s_vi_vad_rms);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_vi_vad, vi_vad, Show/set voice ID VAD rms threshold);

/* vi_clear — clear all speakers */
static int cmd_vi_clear(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (g_ai_initialized) {
        xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
        gallery_clear(&s_gallery);
        xSemaphoreGive(s_gallery_mutex);
        gallery_mark_dirty();
    }
    printf("All speakers cleared\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_vi_clear, vi_clear, Clear all speakers);

/* ===================================================================
 * BLE Voice ID callback wrappers
 * =================================================================== */
static void ble_vi_start(void)
{
    if (!g_ai_initialized || !s_streaming) return;
    s_vi_accum_pos = 0;
    s_vi_enabled = true;
    IPRO_LOGI(TAG, "Voice ID enabled via BLE");
}

static void ble_vi_stop(void)
{
    s_vi_enabled = false;
    IPRO_LOGI(TAG, "Voice ID disabled via BLE");
}

static void ble_vi_reg(const char *name)
{
    /* Non-blocking: just set flag, voice_id_task handles the rest.
     * Ignore re-entry — a BLE retry mid-registration must not clobber
     * s_vi_reg_name while the task is using it. */
    if (s_vi_reg_pending)
        return;
    strncpy(s_vi_reg_name, name, sizeof(s_vi_reg_name) - 1);
    s_vi_reg_name[sizeof(s_vi_reg_name) - 1] = '\0';
    s_vi_reg_pending = true;
    xSemaphoreGive(s_vi_ready_sem);
    IPRO_LOGI(TAG, "Speaker registration '%s' requested (background)", name);
}

static void ble_vi_list(void)
{
    const char *names[GALLERY_MAX_SPEAKERS];
    if (g_ai_initialized) {
        /* Hold the lock across the notify — the pointers reference
         * s_gallery.names[] and a concurrent delete would shift them.
         * bt_gatt_notify copies into a net_buf and returns (fast). */
        xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
        int n = s_gallery.n;
        for (int i = 0; i < n; i++)
            names[i] = s_gallery.names[i];
        ble_audio_notify_speaker_list(n, names);
        xSemaphoreGive(s_gallery_mutex);
    } else {
        ble_audio_notify_speaker_list(0, names);
    }
    vi_print_speakers();
}

static void ble_vi_clear(void)
{
    if (g_ai_initialized) {
        xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
        gallery_clear(&s_gallery);
        xSemaphoreGive(s_gallery_mutex);
        gallery_mark_dirty();
    }
    IPRO_LOGI(TAG, "All speakers cleared via BLE");
}

static int ble_vi_delete(uint8_t slot)
{
    if (!g_ai_initialized) return -1;
    xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
    int rc = gallery_remove(&s_gallery, (int)slot);
    xSemaphoreGive(s_gallery_mutex);
    if (rc == 0)
        gallery_mark_dirty();
    IPRO_LOGI(TAG, "Speaker slot %u delete via BLE: %s (n=%d)",
              slot, rc == 0 ? "OK" : "bad slot", s_gallery.n);
    return rc;
}

static int ble_vi_rename(uint8_t slot, const char *new_name)
{
    if (!g_ai_initialized) return -1;
    xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
    int rc = gallery_rename(&s_gallery, (int)slot, new_name);
    xSemaphoreGive(s_gallery_mutex);
    if (rc == 0)
        gallery_mark_dirty();
    IPRO_LOGI(TAG, "Speaker slot %u rename -> '%s': %d", slot, new_name, rc);
    return rc;
}

/* ===================================================================
 * BLE runtime config (gain / VAD / match threshold)
 * =================================================================== */
static void ble_cfg_get(int8_t *gain_db, uint16_t *vad_rms, uint8_t *threshold_x100)
{
    int g = s_pdm_gain;
    if (g > 127) g = 127; else if (g < -128) g = -128;
    *gain_db = (int8_t)g;

    int v = s_vi_vad_rms;
    if (v < 0) v = 0; else if (v > 65535) v = 65535;
    *vad_rms = (uint16_t)v;

    float t = g_ai_initialized ? s_gallery.threshold : GALLERY_DEFAULT_THRESHOLD;
    int t100 = (int)(t * 100.0f + 0.5f);
    if (t100 < 0) t100 = 0; else if (t100 > 255) t100 = 255;
    *threshold_x100 = (uint8_t)t100;    /* u8 x100 caps at 2.55 — cosine <= 1 */
}

static void ble_cfg_set(uint8_t mask, int8_t gain_db, uint16_t vad_rms,
                        uint8_t threshold_x100)
{
    if (mask & 0x01) {                  /* bit0 = gain */
        s_pdm_gain = gain_db;
        if (s_streaming || s_recording) /* live-apply; else next capture start */
            hal_auadc_set_digital_gain(s_pdm_gain);
    }
    if (mask & 0x02)                    /* bit1 = VAD rms */
        s_vi_vad_rms = vad_rms;
    if ((mask & 0x04) && g_ai_initialized) {  /* bit2 = match threshold */
        xSemaphoreTake(s_gallery_mutex, portMAX_DELAY);
        s_gallery.threshold = (float)threshold_x100 / 100.0f;
        xSemaphoreGive(s_gallery_mutex);
        gallery_mark_dirty();   /* threshold lives in the persisted blob */
    }
    IPRO_LOGI(TAG, "SET_CONFIG mask=0x%02x -> gain=%d vad=%d thr=%.2f",
              mask, s_pdm_gain, s_vi_vad_rms,
              g_ai_initialized ? (double)s_gallery.threshold : -1.0);
}

/* ===================================================================
 * Device health (VBAT / temperature via GPADC — independent of AUADC,
 * safe during audio capture). One-shot sessions, 1 s cache.
 * =================================================================== */
#define HEALTH_CACHE_MS      1000
#define HEALTH_VBAT_SAMPLES  4
#define HEALTH_TSEN_SAMPLES  2
#define TSEN_DEFAULT_OFFSET  1310   /* empirical fallback for unprogrammed
                                     * eFuse TSEN trim (see tsen_demo.c) */

static int16_t    s_health_temp_x10;
static uint16_t   s_health_vbat_mv;
static TickType_t s_health_stamp;
static bool       s_health_valid;

/* One-shot averaged read of the internal VBAT/2 channel (pattern from
 * ipro7_fall_neck/battery_monitor.c). Returns millivolts, 0 on failure. */
static uint16_t health_read_vbat_mv(void)
{
    adc_device_t adc0;
    memset(&adc0, 0, sizeof(adc0));
    adc0.id                   = 0;
    adc0.caps                 = COMMON_CAPS_INT_RX;
    adc0.continuous_conv_mode = 1;
    adc0.clk_div              = ADC_CLOCK_DIV_32;
    adc0.vref                 = ADC_VREF_3V2;
    adc0.differential_mode    = false;
    adc0.data_width           = ADC_DATA_WIDTH_16B_WITH_256_AVERAGE;
    adc0.fifo_threshold       = ADC_FIFO_THRESHOLD_1BYTE;
    adc0.gain                 = ADC_GAIN_1;
    adc0.rx_dma               = -1;

    hal_adc_setConfig(&adc0);
    hal_adc_init(0);
    hal_adc_ioctl(0, ADC_CTRL_VBAT_ON, NULL);

    uint8_t pos_list[1] = { ADC_CHANNEL_VABT_HALF };
    uint8_t neg_list[1] = { ADC_CHANNEL_GND };
    adc_channel_cfg_t ch_cfg = { .pos_channel = pos_list,
                                 .neg_channel = neg_list, .num = 1 };
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_CONFIG, &ch_cfg);
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_START, NULL);

    uint32_t sum_mv = 0, n = 0;
    for (int i = 0; i < HEALTH_VBAT_SAMPLES; i++) {
        adc_channel_val_t v[1];
        if (hal_adc_read(0, v, 1) > 0) {
            sum_mv += (uint32_t)(v[0].volt * 2000.0f);  /* VBAT = (VBAT/2)*2 */
            n++;
        }
    }

    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_STOP, NULL);
    hal_adc_ioctl(0, ADC_CTRL_VBAT_OFF, NULL);

    return n ? (uint16_t)(sum_mv / n) : 0;
}

/* One-shot TSEN read (tsen_demo.c single-shot pattern — no CHANNEL_START,
 * no continuous mode: TSEN_Get_Temp drives the ADC itself). */
static int16_t health_read_temp_x10(void)
{
    static uint16_t s_tsen_offset;

    adc_device_t adc0;
    memset(&adc0, 0, sizeof(adc0));
    adc0.id                   = 0;
    adc0.caps                 = COMMON_CAPS_INT_RX;
    adc0.continuous_conv_mode = 0;
    adc0.clk_div              = ADC_CLOCK_DIV_32;
    adc0.vref                 = ADC_VREF_3V2;
    adc0.differential_mode    = false;
    adc0.data_width           = ADC_DATA_WIDTH_16B_WITH_256_AVERAGE;
    adc0.fifo_threshold       = ADC_FIFO_THRESHOLD_1BYTE;
    adc0.gain                 = ADC_GAIN_1;
    adc0.rx_dma               = -1;

    uint8_t pos_list[1] = { ADC_CHANNEL_TSEN_P };
    uint8_t neg_list[1] = { ADC_CHANNEL_GND };
    adc_channel_cfg_t ch_cfg = { .pos_channel = pos_list,
                                 .neg_channel = neg_list, .num = 1 };

    hal_adc_setConfig(&adc0);
    hal_adc_init(0);
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_CONFIG, &ch_cfg);
    hal_adc_ioctl(0, ADC_CTRL_TSEN_ON, NULL);

    if (s_tsen_offset == 0) {
        hal_adc_trim_tsen(&s_tsen_offset);
        if (s_tsen_offset == 0)
            s_tsen_offset = TSEN_DEFAULT_OFFSET;
    }

    float sum = 0.0f;
    for (int i = 0; i < HEALTH_TSEN_SAMPLES; i++)
        sum += hal_adc_get_tsen(s_tsen_offset);
    float t = sum / HEALTH_TSEN_SAMPLES;

    hal_adc_ioctl(0, ADC_CTRL_TSEN_OFF, NULL);
    hal_adc_ioctl(0, ADC_CTRL_CHANNEL_STOP, NULL);

    return (int16_t)(t * 10.0f + (t >= 0 ? 0.5f : -0.5f));
}

static void ble_health_get(int16_t *temp_c_x10, uint16_t *vbat_mv,
                           uint8_t *model_ready, uint8_t *speaker_count)
{
    TickType_t now = xTaskGetTickCount();
    if (!s_health_valid ||
        (now - s_health_stamp) >= pdMS_TO_TICKS(HEALTH_CACHE_MS)) {
        s_health_vbat_mv  = health_read_vbat_mv();
        s_health_temp_x10 = health_read_temp_x10();
        s_health_stamp = now;
        s_health_valid = true;
    }
    *temp_c_x10  = s_health_temp_x10;
    *vbat_mv     = s_health_vbat_mv;
    *model_ready = g_ai_initialized ? 1 : 0;
    int n = g_ai_initialized ? s_gallery.n : 0;
    *speaker_count = (uint8_t)(n > 255 ? 255 : n);
}

/* Phone-side PCM registration: receive PCM from phone, embed, enroll.
 * Uses s_rec_buf as the receive buffer (PSRAM, 960 KB, plenty for 3 sec @ 16kHz). */
static void ble_reg_pcm_start(const char *name, uint32_t total_bytes)
{
    if (!g_ai_initialized) {
        IPRO_LOGW(TAG, "AI not ready");
        return;
    }
    if (total_bytes == 0 || total_bytes > MAX_RECORD_BYTES) {
        IPRO_LOGW(TAG, "Invalid size %lu", (unsigned long)total_bytes);
        return;
    }
    /* Stop any active capture to avoid interference — remember what was
     * running so pcm_reg_finish() can restore it (success OR failure). */
    if (s_streaming) {
        s_pcm_reg_resume_stream = true;
        s_pcm_reg_resume_vi = s_vi_enabled;
        s_vi_enabled = false;
        s_streaming = false;
        hal_auadc_stop();
    } else {
        s_pcm_reg_resume_stream = false;
        s_pcm_reg_resume_vi = false;
    }
    strncpy(s_pcm_reg_name, name, sizeof(s_pcm_reg_name) - 1);
    s_pcm_reg_name[sizeof(s_pcm_reg_name) - 1] = '\0';
    s_pcm_reg_expected = total_bytes;
    s_pcm_reg_received = 0;
    s_pcm_reg_last_tick = xTaskGetTickCount();
    IPRO_LOGI(TAG, "[PCM REG] Start '%s', expecting %lu bytes",
              s_pcm_reg_name, (unsigned long)total_bytes);
}

static void ble_reg_pcm_data(const uint8_t *data, uint16_t len)
{
    if (s_pcm_reg_expected == 0 || s_pcm_reg_received >= s_pcm_reg_expected)
        return;

    uint32_t remaining = s_pcm_reg_expected - s_pcm_reg_received;
    uint32_t copy_len = (len < remaining) ? len : remaining;

    memcpy((uint8_t *)s_rec_buf + s_pcm_reg_received, data, copy_len);
    s_pcm_reg_received += copy_len;
    s_pcm_reg_last_tick = xTaskGetTickCount();

    /* Log progress every ~16KB */
    if ((s_pcm_reg_received % 16384) < len) {
        IPRO_LOGI(TAG, "[PCM REG] %lu / %lu bytes",
                  (unsigned long)s_pcm_reg_received,
                  (unsigned long)s_pcm_reg_expected);
    }

    if (s_pcm_reg_received >= s_pcm_reg_expected) {
        IPRO_LOGI(TAG, "[PCM REG] All bytes received, triggering inference");
        s_pcm_reg_pending = true;
        xSemaphoreGive(s_vi_ready_sem);
    }
}

/* Meeting mode: AUADC capture + voice ID, no BLE LC3 streaming */
static void ble_meeting_start(bool with_stream)
{
    if (s_streaming) {
        IPRO_LOGW(TAG, "Already running");
        return;
    }
    if (!g_ai_initialized) {
        IPRO_LOGW(TAG, "AI not ready");
        return;
    }

    /* Capture always runs; the BLE stream is optional. Leaving it off is
     * cheaper (no LC3 encode, no radio time) and is the right default for a
     * device doing speaker ID on its own. Turning it on lets the phone keep
     * the audio for transcription while the device still does the speaker
     * work — the two paths are independent in pdm_frame_callback. */
    s_capture_only = !with_stream;
    s_vi_accum_pos = 0;
    s_vi_enabled = true;
    hal_auadc_set_digital_gain(s_pdm_gain);
    s_streaming = true;  /* AUADC active flag */
    hal_auadc_start();
    IPRO_LOGI(TAG, "Meeting started (capture + VI%s)",
              with_stream ? " + BLE stream" : ", no stream");
}

static void ble_meeting_stop(void)
{
    s_vi_enabled = false;
    s_capture_only = false;
    if (s_streaming) {
        s_streaming = false;
        hal_auadc_stop();
    }
    IPRO_LOGI(TAG, "Meeting stopped");
}

/* ===================================================================
 * BLE recorder callbacks
 * =================================================================== */
static ble_audio_recorder_t s_recorder = {
    .start_rec    = pdm_start_recording,
    .stop_rec     = pdm_stop_recording,
    .get_buf      = pdm_get_buffer,
    .get_bytes    = pdm_get_recorded_bytes,
    .is_recording = pdm_is_recording,
    .set_gain     = pdm_set_gain,
    .start_stream = pdm_start_streaming,
    .stop_stream  = pdm_stop_streaming,
    .is_streaming = pdm_is_streaming,
    .stream_queue = NULL,
    .vi_start     = ble_vi_start,
    .vi_stop      = ble_vi_stop,
    .vi_reg       = ble_vi_reg,
    .vi_list      = ble_vi_list,
    .vi_clear     = ble_vi_clear,
    .meeting_start = ble_meeting_start,
    .meeting_stop  = ble_meeting_stop,
    .reg_pcm_start = ble_reg_pcm_start,
    .reg_pcm_data  = ble_reg_pcm_data,
    .vi_delete     = ble_vi_delete,
    .vi_rename     = ble_vi_rename,
    .cfg_get       = ble_cfg_get,
    .cfg_set       = ble_cfg_set,
    .health_get    = ble_health_get,
};

/* ===================================================================
 * Main task
 * =================================================================== */
static void main_task(void *arg)
{
    (void)arg;

    IPRO_LOGI(TAG, "=== Meeting POC ===");
    IPRO_LOGI(TAG, "DMIC + NPU Speaker Recognition + BLE");

    if (pdm_hw_init() != 0) {
        IPRO_LOGE(TAG, "PDM init failed");
        vTaskDelete(NULL);
        return;
    }

    /* DAC playback path (pdm_play / pdm_loop / dac_tone). Also parks the
     * amplifier enable pin in its muted state so it is never left floating. */
    extern void dac_test_init(void);
    dac_test_init();

    /* Load the speaker-embed model from the 'mfg' flash partition.
     * Non-fatal: if the model is missing, vi_* commands report not-ready. */
    if (ai_init() != 0) {
        IPRO_LOGW(TAG, "AI init failed — flash mbv2.inm to '%s' partition; voice ID disabled",
                  MODEL_PARTITION);
    }

    /* Init BLE */
    int ret = ble_audio_init(&s_recorder);
    if (ret != 0) {
        IPRO_LOGW(TAG, "BLE init failed (%d) — shell only", ret);
    }

    IPRO_LOGI(TAG, "Ready. Commands:");
    IPRO_LOGI(TAG, "  pdm_stream on/off  — BLE audio streaming");
    IPRO_LOGI(TAG, "  vi_start/vi_stop   — voice ID on/off (during stream)");
    IPRO_LOGI(TAG, "  vi_reg <name>      — register speaker");
    IPRO_LOGI(TAG, "  vi_id [sec]        — identify speaker");
    IPRO_LOGI(TAG, "  vi_list / vi_clear — manage speakers");

    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

/* ===================================================================
 * Entry point
 * =================================================================== */
int main(void)
{
    ipro_platform_init();

    s_rec_done_sem = xSemaphoreCreateBinary();
    s_cleanup_done_sem = xSemaphoreCreateBinary();
    s_vi_ready_sem = xSemaphoreCreateBinary();
    s_vi_op_done_sem = xSemaphoreCreateBinary();
    s_gallery_mutex = xSemaphoreCreateMutex();
    s_stream_queue = xQueueCreate(STREAM_QUEUE_LEN, sizeof(int16_t *));
    s_stream_ring = pvPortMalloc(STREAM_QUEUE_LEN * FRAME_BYTES);
    s_recorder.stream_queue = s_stream_queue;

    xTaskCreate(pdm_monitor_task, "pdm_mon", 512, NULL, 12, NULL);
    xTaskCreate(voice_id_task, "voice_id", VI_TASK_STACK, NULL, VI_TASK_PRIORITY, NULL);
    xTaskCreate(main_task, "main", 2048, NULL, 10, NULL);

    vTaskStartScheduler();
    while (1) {}
}
