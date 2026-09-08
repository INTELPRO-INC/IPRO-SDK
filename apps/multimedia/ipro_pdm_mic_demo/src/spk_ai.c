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

/*
 * spk_ai.c — on-device speaker ID for ipro_pdm_mic_demo.
 *
 * Reuses the demo's EXISTING PDM capture (pdm_start_recording / pdm_get_buffer)
 * so it never re-inits hal_auadc — the demo inits it once at boot and a second
 * init would break the shared DMA. The model (mbv2.inm) is read from the "mfg"
 * flash partition into RAM and loaded into the NPU via speaker_embed_init_buf().
 *
 * Live shell commands (share the one mic with BLE streaming — one at a time):
 *   spk_live_enroll <name> [sec]   spk_live_id [sec]   spk_live_verify <name> [sec]
 *   spk_live_diarize <segs> [sec]  spk_list   spk_clear   spk_gain <dB>
 *
 * Gallery is RAM-only (cleared on reboot). id/verify threshold 0.42, diarize 0.55.
 */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "shell.h"
#include "ipro_log.h"
#include "hal_boot2.h"
#include "hal_auadc.h"
#include "drv_sf_ctrl.h"
#include "system_soc.h"          /* ipro7.h -> DRV_FLASH_XIP_BASE */

#include <iproai_core.h>
#include <iproai_def.h>
#include "audio/speaker_embed.h"
#include "audio/gallery.h"
#include "audio/diarize.h"

#define TAG "SPK_AI"

#define MODEL_PARTITION   "mfg"
#define PART_HEADER_SIZE  0x1000

/* From main.c — the demo's shared PDM capture (single hal_auadc instance). */
extern void           pdm_start_recording(int seconds);
extern const int16_t *pdm_get_buffer(void);
extern uint32_t       pdm_get_recorded_bytes(void);
extern bool           pdm_is_recording(void);
extern bool           pdm_is_streaming(void);
extern void           pdm_set_gain(int gain_db);

static Gallery s_gallery;
static int     s_ready;

/* The embedding engine is not reentrant (shared fbank/NPU scratch) — serialize
 * the live commands and the streaming monitor. Created in spk_ai_init(). */
static SemaphoreHandle_t s_embed_mutex;

/* ---- streaming monitor (meeting_poc pattern: ISR accumulate -> task infer) ---- */
#define MON_SEC        3                                   /* model needs 3 s */
#define MON_SAMPLES    (16000 * MON_SEC)
#define MON_BYTES      (MON_SAMPLES * (int)sizeof(int16_t))
#define MON_TASK_PRIO  3                                   /* below BLE tasks */

static int16_t          *s_mon_buf;            /* PSRAM, 96 KB */
static volatile uint32_t s_mon_pos;            /* ISR write position (bytes)  */
static volatile bool     s_mon_enabled;
static bool              s_mon_started_capture; /* we started hal_auadc ourselves */
static int               s_mon_vad_rms = 60;   /* RMS below this = silence, skip */
static SemaphoreHandle_t s_mon_ready_sem;
static TaskHandle_t      s_mon_task;

/* -------- load model from the 'mfg' flash partition (pircam XIP pattern) -------- */
static int load_model_from_flash(void)
{
    uint32_t part_addr = 0, part_size = 0;
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

    /* First 32 bytes = 8 x uint32 section lengths; total = sum + 32.
     *
     * Do the size math in 64-bit and reject obviously invalid images before
     * handing the buffer to the NPU loader.  An erased/stale mfg partition can
     * otherwise overflow the 32-bit sum into a tiny model_size (observed: 24),
     * pass the partition-bound check, then make the loader allocate nonsense
     * section sizes and disturb BLE boot/advertising.
     */
    uint32_t header[8];
    memcpy(header, xip, sizeof(header));
    uint64_t sum = 0;
    for (int i = 0; i < 8; i++) {
        sum += header[i];
    }
    uint64_t model_size64 = sum + sizeof(header);
    if ((sum == 0) ||
        (model_size64 <= sizeof(header)) ||
        (model_size64 > UINT32_MAX) ||
        (model_size64 + PART_HEADER_SIZE > part_size)) {
        IPRO_LOGE(TAG, "bad model size %u (part %u) — is mbv2.inm flashed to '%s'?",
                  (unsigned)((model_size64 > UINT32_MAX) ? UINT32_MAX : model_size64),
                  (unsigned)part_size, MODEL_PARTITION);
        return -1;
    }
    uint32_t model_size = (uint32_t)model_size64;
    IPRO_LOGI(TAG, "model in '%s': base=0x%x size=%u", MODEL_PARTITION,
              (unsigned)part_addr, (unsigned)model_size);

    uint8_t *buf = (uint8_t *)pvPortMalloc(model_size);
    if (!buf) { IPRO_LOGE(TAG, "alloc %u failed", (unsigned)model_size); return -1; }
    memcpy(buf, xip, model_size);
    int rc = speaker_embed_init_buf(buf);
    vPortFree(buf);
    return rc;
}

int spk_ai_init(void)
{
    if (s_ready) return 0;
    if (load_model_from_flash() != 0) return -1;
    gallery_init(&s_gallery, GALLERY_DEFAULT_THRESHOLD);
    s_embed_mutex = xSemaphoreCreateMutex();
    if (!s_embed_mutex) { IPRO_LOGE(TAG, "mutex alloc failed"); return -1; }
    s_ready = 1;
    IPRO_LOGI(TAG, "speaker-ID ready (threshold %.2f)", (double)GALLERY_DEFAULT_THRESHOLD);
    return 0;
}

/* ================= streaming monitor ================= */

/* Called from the PDM DMA ISR (main.c pdm_frame_callback) for every frame.
 * Cheap no-op while the monitor is off. */
void spk_ai_feed_frame_isr(const int16_t *pcm, uint32_t bytes, BaseType_t *woken)
{
    if (!s_mon_enabled || !s_mon_buf) return;
    uint32_t pos = s_mon_pos;
    if (pos + bytes <= MON_BYTES) {
        memcpy((uint8_t *)s_mon_buf + pos, pcm, bytes);
        s_mon_pos = pos + bytes;
        if (s_mon_pos >= MON_BYTES && s_mon_ready_sem)
            xSemaphoreGiveFromISR(s_mon_ready_sem, woken);
    }
}

/* Integer RMS over the window (every 8th sample) — energy VAD, meeting_poc style. */
static uint32_t mon_rms(const int16_t *pcm, int n)
{
    int64_t energy = 0;
    for (int i = 0; i < n; i += 8) energy += (int32_t)pcm[i] * pcm[i];
    uint64_t mean_sq = (uint64_t)(energy / (n / 8));
    uint32_t r = (uint32_t)mean_sq;
    if (!r) return 0;
    for (int j = 0; j < 15; j++) r = (r + (uint32_t)(mean_sq / r)) / 2;
    return r;
}

static void spk_monitor_task(void *arg)
{
    (void)arg;
    for (;;) {
        if (xSemaphoreTake(s_mon_ready_sem, portMAX_DELAY) != pdTRUE) continue;
        if (!s_mon_enabled) continue;

        uint32_t rms = mon_rms(s_mon_buf, MON_SAMPLES);
        if (rms < (uint32_t)s_mon_vad_rms) {
            IPRO_LOGD(TAG, "[mon] silence (rms=%lu), skip", (unsigned long)rms);
            s_mon_pos = 0;               /* restart accumulation */
            continue;
        }

        float emb[EMBEDDING_DIM];
        xSemaphoreTake(s_embed_mutex, portMAX_DELAY);
        int rc = speaker_embed(s_mon_buf, MON_SAMPLES, emb);
        xSemaphoreGive(s_embed_mutex);
        if (rc == 0) {
            float score = 0.0f;
            int idx = gallery_identify(&s_gallery, emb, &score);
            if (idx >= 0)
                printf("[mon] speaker=%s score=%.3f (rms=%lu)\r\n",
                       s_gallery.names[idx], (double)score, (unsigned long)rms);
            else
                printf("[mon] unknown (best=%.3f, rms=%lu)\r\n",
                       (double)score, (unsigned long)rms);
        }
        s_mon_pos = 0;                   /* next 3 s window */
    }
}

/* spk_monitor on|off [vad_rms] — continuous identification (works alongside
 * BLE streaming; standalone it starts/stops the capture itself). */
static int cmd_spk_monitor(int argc, char **argv)
{
    if (argc < 2) {
        printf("monitor: %s (vad_rms=%d)\r\nusage: spk_monitor on|off [vad_rms]\r\n",
               s_mon_enabled ? "ON" : "OFF", s_mon_vad_rms);
        return 0;
    }
    if (strcmp(argv[1], "on") == 0) {
        if (!s_ready) { printf("engine not ready\r\n"); return -1; }
        if (s_mon_enabled) { printf("already on\r\n"); return 0; }
        if (argc > 2) s_mon_vad_rms = atoi(argv[2]);
        if (!s_mon_buf) {
            s_mon_buf = (int16_t *)pvPortMalloc(MON_BYTES);
            if (!s_mon_buf) { printf("buf alloc failed\r\n"); return -1; }
        }
        if (!s_mon_ready_sem) s_mon_ready_sem = xSemaphoreCreateBinary();
        if (!s_mon_task &&
            xTaskCreate(spk_monitor_task, "spk_mon", 8192, NULL,
                        MON_TASK_PRIO, &s_mon_task) != pdPASS) {
            printf("task create failed\r\n");
            return -1;
        }
        s_mon_pos = 0;
        s_mon_enabled = true;
        /* Standalone (no BLE stream / recording active): start capture ourselves. */
        if (!pdm_is_streaming() && !pdm_is_recording()) {
            hal_auadc_start();
            s_mon_started_capture = true;
        }
        printf("monitor ON — identifying every %d s (vad_rms=%d)\r\n",
               MON_SEC, s_mon_vad_rms);
    } else {
        s_mon_enabled = false;
        if (s_mon_started_capture) {
            if (!pdm_is_streaming() && !pdm_is_recording())
                hal_auadc_stop();
            s_mon_started_capture = false;
        }
        printf("monitor OFF\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk_monitor, spk_monitor, Continuous speaker ID on|off [vad_rms]);

/* -------- capture N seconds through the demo's recorder, return the PCM -------- */
static int capture(int seconds, const int16_t **pcm, int *nsamp)
{
    if (pdm_is_recording()) { printf("busy (recording/streaming in progress)\r\n"); return -1; }
    printf("speak now (%d s)...\r\n", seconds);
    pdm_start_recording(seconds);
    int waited = 0, limit = (seconds + 2) * 1000;
    while (pdm_is_recording() && waited < limit) { vTaskDelay(pdMS_TO_TICKS(20)); waited += 20; }
    vTaskDelay(pdMS_TO_TICKS(60));   /* let the monitor task finish stop + cache clean */
    *pcm = pdm_get_buffer();
    *nsamp = (int)(pdm_get_recorded_bytes() / sizeof(int16_t));
    return (*nsamp > 0) ? 0 : -1;
}

static int embed_live_worker(int seconds, float emb[EMBEDDING_DIM])
{
    const int16_t *pcm = NULL;
    int ns = 0;
    if (capture(seconds, &pcm, &ns) != 0) return -1;
    xSemaphoreTake(s_embed_mutex, portMAX_DELAY);   /* engine not reentrant */
    int rc = speaker_embed(pcm, ns, emb);
    xSemaphoreGive(s_embed_mutex);
    return rc;
}

/* fbank + NPU inference overflows the shell_exec task's 8KB stack, so run the
 * capture+embed in a dedicated 32KB-stack task and block on its result. */
struct embed_job {
    int sec;
    float *emb;
    volatile int rc;
    SemaphoreHandle_t done;
};
static void embed_job_task(void *arg)
{
    struct embed_job *j = (struct embed_job *)arg;
    j->rc = embed_live_worker(j->sec, j->emb);
    xSemaphoreGive(j->done);
    vTaskDelete(NULL);
}
static int embed_live(int seconds, float emb[EMBEDDING_DIM])
{
    struct embed_job j = { seconds, emb, -1, NULL };
    j.done = xSemaphoreCreateBinary();
    if (!j.done) { printf("sem alloc failed\r\n"); return -1; }
    if (xTaskCreate(embed_job_task, "spk_embed", 8192, &j, 10, NULL) != pdPASS) {
        vSemaphoreDelete(j.done);
        printf("embed task create failed\r\n");
        return -1;
    }
    xSemaphoreTake(j.done, portMAX_DELAY);
    vSemaphoreDelete(j.done);
    return j.rc;
}

static int arg_sec(int argc, char **argv, int idx)
{
    int s = (argc > idx) ? atoi(argv[idx]) : 3;
    if (s < 1) s = 1;
    if (s > 5) s = 5;
    return s;
}

/* spk_live_enroll <name> [sec] */
static int cmd_spk_live_enroll(int argc, char **argv)
{
    if (!s_ready) { printf("engine not ready\r\n"); return -1; }
    if (argc < 2) { printf("usage: spk_live_enroll <name> [sec]\r\n"); return -1; }
    float emb[EMBEDDING_DIM];
    if (embed_live(arg_sec(argc, argv, 2), emb) != 0) { printf("capture/embed failed\r\n"); return -1; }
    int c = gallery_enroll(&s_gallery, argv[1], emb);
    if (c < 0) { printf("gallery full\r\n"); return -1; }
    printf("enrolled '%s' (%d clip%s)\r\n", argv[1], c, c == 1 ? "" : "s");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk_live_enroll, spk_live_enroll, Record a clip and enroll a speaker);

/* spk_live_id [sec] */
static int cmd_spk_live_id(int argc, char **argv)
{
    if (!s_ready) { printf("engine not ready\r\n"); return -1; }
    float emb[EMBEDDING_DIM];
    if (embed_live(arg_sec(argc, argv, 1), emb) != 0) { printf("capture/embed failed\r\n"); return -1; }
    float score = 0.0f;
    int idx = gallery_identify(&s_gallery, emb, &score);
    if (idx >= 0) printf("ACCEPT speaker=%s score=%.3f\r\n", s_gallery.names[idx], (double)score);
    else          printf("REJECT (best score=%.3f)\r\n", (double)score);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk_live_id, spk_live_id, Record a clip and identify (1:N));

/* spk_live_verify <name> [sec] */
static int cmd_spk_live_verify(int argc, char **argv)
{
    if (!s_ready) { printf("engine not ready\r\n"); return -1; }
    if (argc < 2) { printf("usage: spk_live_verify <name> [sec]\r\n"); return -1; }
    float emb[EMBEDDING_DIM];
    if (embed_live(arg_sec(argc, argv, 2), emb) != 0) { printf("capture/embed failed\r\n"); return -1; }
    float score = 0.0f;
    int r = gallery_verify(&s_gallery, argv[1], emb, &score);
    if (r == 1)      printf("ACCEPT %s score=%.3f\r\n", argv[1], (double)score);
    else if (r == 0) printf("REJECT %s score=%.3f\r\n", argv[1], (double)score);
    else             printf("no speaker '%s' enrolled\r\n", argv[1]);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk_live_verify, spk_live_verify, Record a clip and verify a claimed name (1:1));

/* spk_live_diarize <segments> [sec] */
static int cmd_spk_live_diarize(int argc, char **argv)
{
    if (!s_ready) { printf("engine not ready\r\n"); return -1; }
    int segs = (argc > 1) ? atoi(argv[1]) : 6;
    if (segs < 2) segs = 2;
    if (segs > 16) segs = 16;
    int sec = arg_sec(argc, argv, 2);
    /* DiarizeState is 32KB (centroids[32][256]) — too big for the shell stack. */
    DiarizeState *st = (DiarizeState *)pvPortMalloc(sizeof(DiarizeState));
    if (!st) { printf("alloc failed\r\n"); return -1; }
    diarize_init(st);
    int nspk = 0;
    for (int k = 0; k < segs; k++) {
        float emb[EMBEDDING_DIM];
        printf("segment %d/%d — ", k + 1, segs);
        if (embed_live(sec, emb) != 0) { printf("capture failed\r\n"); vPortFree(st); return -1; }
        int c = diarize_update(st, emb, 0.55f);
        if (c + 1 > nspk) nspk = c + 1;
        printf("  -> Speaker-%d\r\n", c);
    }
    printf("[live_diar] %d segment(s) -> %d speaker(s)\r\n", segs, nspk);
    vPortFree(st);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk_live_diarize, spk_live_diarize, Record N segments and cluster speakers);

/* spk_list */
static int cmd_spk_list(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (!s_ready || s_gallery.n == 0) { printf("[list] gallery empty\r\n"); return 0; }
    for (int i = 0; i < s_gallery.n; i++)
        printf("  %s (%d clip%s)\r\n", s_gallery.names[i], s_gallery.count[i],
               s_gallery.count[i] == 1 ? "" : "s");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk_list, spk_list, List enrolled speakers);

/* spk_clear */
static int cmd_spk_clear(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (s_ready) gallery_clear(&s_gallery);
    printf("gallery cleared\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk_clear, spk_clear, Clear the enrolled gallery);

/* spk_gain <dB> — reuse the demo's PDM digital gain */
static int cmd_spk_gain(int argc, char **argv)
{
    if (argc < 2) { printf("usage: spk_gain <dB>\r\n"); return -1; }
    pdm_set_gain(atoi(argv[1]));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk_gain, spk_gain, Set PDM mic digital gain in dB);
