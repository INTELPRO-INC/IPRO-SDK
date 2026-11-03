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
 * @file voice.c
 * @brief Microphone, enrollment and live labelling. See voice.h.
 *
 * Everything runs in one task, so the model, the gallery and the capture
 * device never see two callers: it reads 20 ms PDM frames, resamples them
 * from the AUADC's ~15,872.5 Hz to 16 kHz into a 32 s ring, keeps a 100 ms
 * energy VAD beside it, and between frames serves the request queue. An
 * embedding takes the task away for about a tenth of a second; the capture
 * stream buffer (2 s) rides that out.
 */

#include <generated/autoconf.h>

#include "voice.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <stream_buffer.h>
#include <task.h>

#include "board_config.h"
#include "audio_pdm_mic.h"
#include "ipro_ai_event.h"
#include "audio/diarize.h"
#include "audio/gallery.h"
#include "drv_sf_ctrl.h"
#include "hal_boot2.h"
#include "hal_flash.h"
#include "ipro_log.h"
#include "utils_crc.h"
#include <compiler/common.h>     /* ATTR_OCRAM_SECTION */
#include "rgb_led.h"

#define LOG_TAG "VOICE"

#define RATE                16000u
#define FRAME_SAMPLES       AUDIO_PDM_MIC_FRAME
#define CAPTURE_BUF_BYTES   (RATE * 2u * 2u)        /* 2 s */
#define FEED_BUF_BYTES      (RATE * 2u)             /* 1 s of host PCM */
/* The microphone chain is components/audio/audio_pdm_mic.h: the PDM path's
 * real rate (about 15,872 Hz) resampled to 16,000, a 100 Hz high-pass ahead of
 * +20 dB of make-up gain. The PDM path carries DC and mains hum far above a
 * quiet room: full-band RMS sat at -28 dBFS with nobody talking (2026-10-31),
 * so every 100 ms frame cleared the VAD and the hum was labelled as an
 * enrolled speaker; training already high-passed at 80-250 Hz half of the time. */
#define SETTLE_SAMPLES      (RATE / 4u)             /* decimator start-up step */

#define RING_SAMPLES        (RATE * 32u)
#define VAD_FRAME           1600u                   /* 100 ms */
#define VAD_SLOTS           (RING_SAMPLES / VAD_FRAME)
/* VAD: chosen on far-field meeting recordings by replaying this logic on a
 * host (2026-10-31): of margin 3-8 dB, floor rise 0.2-1 dB/s, voiced share
 * 0.3-0.5, 3 dB / 1 dB/s / 0.5 was best (DER 34.8 % at collar 0.25, own name
 * 78 %, against 44.7 % and 63 % for the first guess, 8 dB / 0.2 dB/s), and
 * labelling only the voiced 100 ms frames was worse. */
#define VAD_MARGIN_DB       3.0f                    /* above the noise floor */
#define VAD_ABS_MIN_DB      (-70.0f)
#define VAD_FLOOR_RISE_DB   0.1f                    /* per frame, 1 dB/s */
/* Digital silence (host PCM padding, a muted source) is no noise floor: it
 * would pull the floor to -120 dB, from where it climbs back for minutes and
 * every frame over VAD_ABS_MIN_DB counts as speech meanwhile. */
#define VAD_DIGITAL_SILENCE (-100.0f)
/* Each run starts the floor above everything, so the first frame sets it and
 * the first quiet one pulls it down to the room. A fixed -60 dB start left a
 * -28 dBFS room (an office, 2026-10-31) voiced for the half minute the floor
 * took to climb to it, and that noise got labelled as an enrolled person. */
#define VAD_FLOOR_START     0.0f
#define BANDS_N             8

#define TICK_SAMPLES        4000u                   /* 250 ms: LEVEL lines */
#define HOP_SAMPLES         RATE                    /* live: one label a second */
#define ENROLL_HOP          (RATE / 2u)
#define VOICED_LIVE         0.5f
#define VOICED_ENROLL       0.6f
#define ENROLL_MIN_WINDOWS  4
#define ENROLL_MAX_WINDOWS  64

/* Swept on far-field meeting recordings by replaying this logic on a host
 * (2026-10-31, model b3). The name threshold trades a stranger being given an
 * enrolled name against an enrolled person being left anonymous; with half
 * the speakers enrolled:
 *   0.50: strangers named 26 %, own name 78 %, DER 45.5 % (collar 0.25)
 *   0.55:                 15 %,          71 %,      46.7 %
 *   0.60:                  7 %,          60 %,      49.5 %
 *   0.65:                  2 %,          46 %,      52.8 %
 * A wrong name is the worse mistake to show, so 0.60 (a podcast played to the
 * board named strangers at 0.50-0.65 with 0.50). Averaging a turn's windows,
 * holding the previous second's person and session-mean centring were tried
 * and moved along the same curve, not off it. A new anonymous speaker at 0.50
 * rather than 0.60 splits fewer strangers into several. THRESH changes both
 * at run time; the name match is saved with the voiceprints. */
#define ID_THRESHOLD        0.60f
#define ANON_THRESHOLD      0.50f

/* Stranger mode (MODE STRANGERS, off by default). Everything heard through one
 * microphone and room - and through a loudspeaker even more - shares a
 * component that makes different people score alike: played from a laptop
 * speaker to the Reef, people scored 0.2-0.3 higher against each other than on
 * the recordings, and strangers took the name of the closest enrolled voice.
 * In stranger mode the mean over the session's speakers - each enrolled
 * voiceprint and each anonymous speaker of CENTER_MIN_WINDOWS+ windows, one
 * vote each, so whoever talks most does not become the channel - estimates
 * that component and CENTER_ALPHA of it is removed from every voiceprint
 * before any comparison; names need STRANGER_ID_THRESHOLD on that scale; and
 * an anonymous speaker takes a window from its enrolled match when it scores
 * ANON_WIN_MARGIN higher, since a stranger's own cluster is closer to them than
 * the enrolled voice they resemble. Played through a laptop speaker (Mandarin,
 * 3 enrolled + 2 strangers, 4 groups, model b3): strangers named 35 % -> 0 %,
 * both strangers on their own anonymous number 53 % -> 93 %, own name 95 -> 93 %.
 * On far-field meeting recordings it costs enrolled people their name (60 % ->
 * 30 % of their speech): their own off windows form an anonymous speaker that
 * then wins - hence a mode, not the default. */
#define STRANGER_ID_THRESHOLD 0.50f
#define CENTER_ALPHA        0.4f
#define CENTER_MIN_WINDOWS  3
#define ANON_WIN_MARGIN     0.05f

#define GALLERY_PARTITION   "media"
/* media starts with a tag, the CRC of the model the voiceprints came from, the
 * gallery blob's length and its CRC; the blob follows. A voiceprint is only
 * comparable with others from the same model, so a new model starts with an
 * empty list. The tag also keeps the mode, whose thresholds the saved name
 * match is on. A save cut short fails the blob CRC and is reported, not
 * loaded. V1.1 wrote an 8-byte header (tag, model CRC) and no blob CRC; it is
 * still read. */
#define GALLERY_TAG2        0x324C4756u             /* "VGL2": normal mode */
#define GALLERY_TAG2_STRANGER 0x32534756u           /* "VGS2": stranger mode */
#define GALLERY_HDR2_BYTES  16u
#define GALLERY_TAG         0x4C414756u             /* "VGAL": V1.1, normal mode */
#define GALLERY_TAG_STRANGER 0x53414756u            /* "VGAS": V1.1, stranger mode */
#define GALLERY_TAG_BYTES   8u
#define GALLERY_SAVE_MS     1500u

#define VOICE_STACK_WORDS   8192u                   /* fbank works on the stack */
#define VOICE_PRIO          (configMAX_PRIORITIES - 4)
#define QUEUE_LEN           8

typedef enum { ST_IDLE = 0, ST_ENROLL, ST_LIVE } state_t;

typedef enum {
    REQ_ENROLL, REQ_LIVE_ON, REQ_LIVE_OFF, REQ_STOP, REQ_DELETE, REQ_NAME, REQ_THRESH,
    REQ_EMBED, REQ_HELLO, REQ_LIST, REQ_SRC, REQ_BANDS, REQ_MODE,
} req_type_t;

typedef struct {
    req_type_t type;
    uint32_t   seconds;
    int        k;
    float      a, b;
    char       name[VOICE_NAME_MAX + 1];
} req_t;

static QueueHandle_t   s_q;
static voice_sink_t    s_sink;
static volatile bool   s_echo;
static volatile bool   s_vectors;
static bool            s_stranger;                   /* stranger mode; written by the voice task only */
static volatile state_t s_state;
static int             s_model_ok;

/* capture: the PDM microphone, or PCM the host feeds over USB (SRC HOST) */
static bool            s_src_host;
static StreamBufferHandle_t s_feed;
static volatile uint32_t s_fed;
static bool            s_cap_on;
static void gallery_mark_dirty(void);
static int16_t        *s_ring;
static uint32_t        s_wpos;                       /* samples written, ever */
static uint32_t        s_open_pos;                   /* s_wpos when capture opened */
static float           s_vad_db[VAD_SLOTS];
static uint8_t         s_vad_on[VAD_SLOTS];
static float           s_floor_db = VAD_FLOOR_START;
static uint64_t        s_vad_acc;

/* model work */
static int16_t        *s_win;                        /* SPK_MODEL_SAMPLES, voice task only */
static int16_t        *s_test_pcm;                   /* EMBED from the host */
static float          *s_embs;                       /* [ENROLL_MAX_WINDOWS][dim] */
static float           s_emb[EMBEDDING_DIM];
static volatile bool   s_pcm_busy;

/* gallery + anonymous speakers */
static Gallery         s_gal;
static DiarizeState    s_anon;
static uint32_t        s_anon_named;                 /* bit k: cluster k was NAMEd into the gallery */
static float           s_anon_thr = ANON_THRESHOLD;
static uint32_t        s_gal_addr, s_gal_size;
static bool            s_gal_dirty;
static TickType_t      s_gal_dirty_tick;

/* enroll / live bookkeeping */
static char            s_enroll_name[VOICE_NAME_MAX + 1];
static uint32_t        s_enroll_from, s_enroll_to;
static uint32_t        s_live_from, s_last_label, s_next_tick;
static uint32_t        s_segs, s_embeds;

/* ---------------------------------------------------------------- output */

static char s_line[2304];

static void emit(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    int n = vsnprintf(s_line, sizeof(s_line) - 2, fmt, ap);
    va_end(ap);
    if (n < 0) {
        return;
    }
    if (n > (int)sizeof(s_line) - 3) {
        n = (int)sizeof(s_line) - 3;
    }
    s_line[n] = '\n';
    s_line[n + 1] = 0;
    if (s_sink) {
        s_sink(s_line);
    }
    if (s_echo) {
        printf("[voice] %s", s_line);
    }
}

/* VECS ON: `VEC <e|l> <window | t1 ms> <dim x value*10000>` to the host only
 * (not the console) for every voiceprint computed - numbers, no audio. */
static void emit_vec(char kind, unsigned long at, const float *v)
{
    if (!s_vectors || !s_sink) {
        return;
    }
    int n = snprintf(s_line, sizeof(s_line), "VEC %c %lu", kind, at);
    for (uint32_t d = 0; d < EMBEDDING_DIM && n > 0 && n < (int)sizeof(s_line) - 10; d++) {
        n += snprintf(s_line + n, sizeof(s_line) - (size_t)n, " %ld", lrintf(v[d] * 10000.0f));
    }
    if (n <= 0 || n > (int)sizeof(s_line) - 2) {
        return;
    }
    s_line[n] = '\n';
    s_line[n + 1] = 0;
    s_sink(s_line);
}

/* A labelled segment as an event (ipro_ai_event.h): the gallery index and
 * name, or -1 and "" for a voice not enrolled; t1 in ms of audio. */
static void publish_speaker(unsigned long t1_ms, int index, float score, const char *name)
{
    ipro_ai_event_t e = { .type = IPRO_AI_EVT_SPEAKER, .model_id = 2, .index = index,
                          .score = (int32_t)lrintf(score * 1000.0f), .t_us = (uint64_t)t1_ms * 1000u };
    strcpy(e.model, "voice");
    strncpy(e.label, name, IPRO_AI_EVENT_NAME_MAX);
    ipro_ai_event_publish(&e);
}

static const char *state_name(state_t s)
{
    return s == ST_ENROLL ? "enroll" : s == ST_LIVE ? "live" : "idle";
}

/* ---------------------------------------------------------------- gallery */

static int gallery_find(const char *name)
{
    for (int i = 0; i < s_gal.n; i++) {
        if (strncmp(s_gal.names[i], name, GALLERY_NAME_LEN) == 0) {
            return i;
        }
    }
    return -1;
}

static const uint8_t *gallery_xip(void)
{
    const uint32_t sf_off = SF_Ctrl_Get_Flash_Image_Offset(0, 0);
    return (const uint8_t *)(uintptr_t)(DRV_FLASH_XIP_BASE + (s_gal_addr - sf_off));
}

static void gallery_load(void)
{
    gallery_init(&s_gal, ID_THRESHOLD);
    if (hal_boot2_partition_addr_active(GALLERY_PARTITION, &s_gal_addr, &s_gal_size) != 0 ||
        s_gal_addr < SF_Ctrl_Get_Flash_Image_Offset(0, 0)) {
        IPRO_LOGW(LOG_TAG, "no '%s' partition: enrolled speakers will not survive a reset",
                  GALLERY_PARTITION);
        s_gal_addr = 0;
        return;
    }
    uint32_t hdr[4];
    spk_model_info_t mi;
    memcpy(hdr, gallery_xip(), sizeof(hdr));
    spk_model_get_info(&mi);
    const bool v2 = hdr[0] == GALLERY_TAG2 || hdr[0] == GALLERY_TAG2_STRANGER;
    if (!v2 && hdr[0] != GALLERY_TAG && hdr[0] != GALLERY_TAG_STRANGER) {
        IPRO_LOGI(LOG_TAG, "no saved speakers");
        return;
    }
    if (hdr[1] != mi.crc) {
        IPRO_LOGW(LOG_TAG, "saved speakers came from another model (%08lx, now %08lx): "
                  "starting empty, enroll again", (unsigned long)hdr[1], (unsigned long)mi.crc);
        return;
    }
    const uint32_t off = v2 ? GALLERY_HDR2_BYTES : GALLERY_TAG_BYTES;
    uint32_t len = s_gal_size - off;
    if (v2) {
        if (hdr[2] == 0) {
            IPRO_LOGI(LOG_TAG, "no saved speakers");
            return;
        }
        if (hdr[2] > len || utils_crc32(gallery_xip() + off, hdr[2]) != hdr[3]) {
            IPRO_LOGW(LOG_TAG, "saved speakers are damaged (a save was cut short?) - starting empty");
            return;
        }
        len = hdr[2];
    }
    if (gallery_load_from_buf(&s_gal, gallery_xip() + off, len) != 0) {
        IPRO_LOGW(LOG_TAG, "saved speakers do not fit this model (dim %d) - starting empty",
                  EMBEDDING_DIM);
        gallery_init(&s_gal, ID_THRESHOLD);
        return;
    }
    s_stranger = hdr[0] == GALLERY_TAG_STRANGER || hdr[0] == GALLERY_TAG2_STRANGER;
    IPRO_LOGI(LOG_TAG, "%d speaker(s) restored, id threshold %.2f, %s mode", s_gal.n,
              (double)s_gal.threshold, s_stranger ? "stranger" : "normal");
}

static void gallery_mark_dirty(void)
{
    s_gal_dirty = true;
    s_gal_dirty_tick = xTaskGetTickCount();
}

/* Erase, write, read back; NULL or the step that failed. flash_read() fills
 * its destination while the flash mapping is switched, so it reads into OCRAM. */
static const char *gallery_write(uint8_t *buf, uint32_t total)
{
    static ATTR_OCRAM_SECTION uint8_t check[256];

    if (flash_erase(s_gal_addr, (total + 4095u) & ~4095u) != 0) {
        return "erase";
    }
    if (flash_write(s_gal_addr, buf, total) != 0) {
        return "write";
    }
    for (uint32_t off = 0; off < total; off += sizeof(check)) {
        const uint32_t n = total - off < sizeof(check) ? total - off : sizeof(check);
        if (flash_read(s_gal_addr + off, check, n) != 0 || memcmp(check, buf + off, n) != 0) {
            return "verify";
        }
    }
    return NULL;
}

/* Erase + write stall XIP for tens of ms: only from this task, debounced. */
static void gallery_save_if_due(void)
{
    static uint8_t *buf;

    if (!s_gal_dirty || !s_gal_addr ||
        xTaskGetTickCount() - s_gal_dirty_tick < pdMS_TO_TICKS(GALLERY_SAVE_MS)) {
        return;
    }
    s_gal_dirty = false;
    if (!buf && !(buf = pvPortMalloc(s_gal_size))) {
        IPRO_LOGE(LOG_TAG, "no memory to save the speakers");
        return;
    }
    const uint32_t room = s_gal_size - GALLERY_HDR2_BYTES;
    const int max_spk = (int)((room - 16u) / (GALLERY_NAME_LEN + EMBEDDING_DIM * 4u));
    if (s_gal.n > max_spk) {
        IPRO_LOGW(LOG_TAG, "%d speakers, %d fit in '%s'", s_gal.n, max_spk, GALLERY_PARTITION);
    }
    const int len = gallery_save_to_buf(&s_gal, buf + GALLERY_HDR2_BYTES, room, max_spk);
    if (len < 0) {
        return;
    }
    spk_model_info_t mi;
    spk_model_get_info(&mi);
    const uint32_t hdr[4] = {
        s_stranger ? GALLERY_TAG2_STRANGER : GALLERY_TAG2, mi.crc, (uint32_t)len,
        utils_crc32(buf + GALLERY_HDR2_BYTES, (uint32_t)len),
    };
    memcpy(buf, hdr, sizeof(hdr));
    const char *why = gallery_write(buf, GALLERY_HDR2_BYTES + (uint32_t)len);
    if (why) {
        IPRO_LOGE(LOG_TAG, "speakers NOT saved: flash %s failed", why);
        return;
    }
    IPRO_LOGI(LOG_TAG, "speakers saved (%d, %d B)", s_gal.n, len);
}

/* ---------------------------------------------------------------- capture */

/* FreeRTOS refuses to reset a stream buffer while a sender is blocked on it;
 * voice_feed() waits at most 100 ms at a time and gives up once s_cap_on drops. */
static void feed_reset(void)
{
    for (int i = 0; i < 30 && xStreamBufferReset(s_feed) != pdPASS; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static int capture_open(void)
{
    const audio_pdm_mic_cfg_t mic = {
        .clk_pin   = BOARD_MIC_CLK_PIN,
        .data_pin  = BOARD_MIC_DAT_PIN,
        .channel   = BOARD_MIC_PDM_CHANNEL,
        .buf_bytes = CAPTURE_BUF_BYTES,
    };

    if (s_cap_on) {
        return 0;
    }
    s_floor_db = VAD_FLOOR_START;
    if (s_src_host) {
        feed_reset();
        s_open_pos = s_wpos;
        s_vad_acc = 0;
        s_cap_on = true;
        rgb_led_set(0, 0, 40);                /* blue: listening to the host, not the room */
        return 0;
    }
    if (audio_pdm_mic_open(&mic) != 0) {
        return -1;
    }
    s_open_pos = s_wpos;
    s_vad_acc = 0;
    s_cap_on = true;
    rgb_led_set(40, 0, 0);                    /* red: the microphone is on */
    return 0;
}

static void capture_close(void)
{
    if (!s_cap_on) {
        return;
    }
    s_cap_on = false;
    if (s_src_host) {
        feed_reset();
        rgb_led_off();
        return;
    }
    audio_pdm_mic_close();
    rgb_led_off();
}

/* 100 ms frames sit on absolute sample positions (the frame ending at
 * s_wpos + i + 1), so a window's frames follow from its sample range alone. */
static void vad_push(const int16_t *pcm, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++) {
        s_vad_acc += (uint64_t)((int32_t)pcm[i] * pcm[i]);
        if ((s_wpos + i + 1u) % VAD_FRAME) {
            continue;
        }
        const uint32_t frame = (s_wpos + i + 1u) / VAD_FRAME - 1u;
        const float db = 10.0f * log10f((float)s_vad_acc / VAD_FRAME / (32768.0f * 32768.0f) + 1e-12f);
        if (db > VAD_DIGITAL_SILENCE) {
            if (db < s_floor_db) {
                s_floor_db = db;
            } else {
                s_floor_db += VAD_FLOOR_RISE_DB;
            }
        }
        s_vad_db[frame % VAD_SLOTS] = db;
        s_vad_on[frame % VAD_SLOTS] = db > s_floor_db + VAD_MARGIN_DB && db > VAD_ABS_MIN_DB;
        s_vad_acc = 0;
    }
}

static void ring_store(const int16_t *pcm, uint32_t n)
{
    const uint32_t at = s_wpos % RING_SAMPLES;
    const uint32_t first = (n < RING_SAMPLES - at) ? n : RING_SAMPLES - at;
    memcpy(&s_ring[at], pcm, first * sizeof(int16_t));
    memcpy(s_ring, &pcm[first], (n - first) * sizeof(int16_t));
    s_wpos += n;
}

/* One capture frame into the ring (blocks up to 20 ms). Host PCM is 16 kHz
 * already and goes in as is - no resampling, no extra gain; the microphone's
 * comes out of audio_pdm_mic resampled, high-passed and +20 dB. */
static void capture_pump(void)
{
    static int16_t in[FRAME_SAMPLES];
    static int16_t out[FRAME_SAMPLES + FRAME_SAMPLES / 8u];

    if (s_src_host) {
        const size_t got = xStreamBufferReceive(s_feed, in, sizeof(in), pdMS_TO_TICKS(20));
        const uint32_t n = (uint32_t)(got / sizeof(int16_t));
        if (n) {
            vad_push(in, n);
            ring_store(in, n);
        }
        return;
    }
    uint32_t n;
    do {
        n = audio_pdm_mic_read(out, sizeof(out) / sizeof(out[0]), 20);
        if (n) {
            vad_push(out, n);
            ring_store(out, n);
        }
    } while (n && audio_pdm_mic_pending());
}

static void window_copy(uint32_t end, int16_t *dst)
{
    const uint32_t start = end - SPK_MODEL_SAMPLES;
    const uint32_t at = start % RING_SAMPLES;
    const uint32_t first = (SPK_MODEL_SAMPLES < RING_SAMPLES - at) ? SPK_MODEL_SAMPLES
                                                                    : RING_SAMPLES - at;
    memcpy(dst, &s_ring[at], first * sizeof(int16_t));
    memcpy(&dst[first], s_ring, (SPK_MODEL_SAMPLES - first) * sizeof(int16_t));
}

/* Share of the whole 100 ms frames inside [end - window, end) the VAD calls voiced. */
static float voiced_share(uint32_t end)
{
    const uint32_t f0 = (end - SPK_MODEL_SAMPLES + VAD_FRAME - 1u) / VAD_FRAME;
    const uint32_t f1 = end / VAD_FRAME;
    uint32_t on = 0;

    if (f1 <= f0) {
        return 0.0f;
    }
    for (uint32_t f = f0; f < f1; f++) {
        on += s_vad_on[f % VAD_SLOTS];
    }
    return (float)on / (float)(f1 - f0);
}

static void emit_level(uint32_t from)
{
    const uint32_t last = s_wpos / VAD_FRAME;
    if (last < 2u || s_wpos < from) {
        return;
    }
    const float db = 0.5f * (s_vad_db[(last - 1u) % VAD_SLOTS] + s_vad_db[(last - 2u) % VAD_SLOTS]);
    emit("LEVEL %lu %.1f %d", (unsigned long)((s_wpos - from) / 16u), (double)db,
         s_vad_on[(last - 1u) % VAD_SLOTS]);
}

static int level_db10(double ms)
{
    return (int)lrint(100.0 * log10(ms / (32768.0 * 32768.0) + 1e-12));
}

/* What the last second sounded like, without sending it: the mean, the RMS
 * and octave bands (RBJ band-pass, Q 1.41) of the ring, levels in dBFS x 10
 * as integers (the SDK's printf drops repeated %f). Tells a room's noise from
 * hum or interference coupled into the PDM lines. */
static void emit_bands(void)
{
    static const float centre[BANDS_N] = { 63, 125, 250, 500, 1000, 2000, 4000, 6300 };
    double sum = 0.0, sq = 0.0, band[BANDS_N] = { 0 };
    float b0[BANDS_N], a1[BANDS_N], a2[BANDS_N], x1[BANDS_N] = { 0 }, x2[BANDS_N] = { 0 },
          y1[BANDS_N] = { 0 }, y2[BANDS_N] = { 0 };

    if (!s_cap_on || s_wpos - s_open_pos < SETTLE_SAMPLES + RATE) {
        emit("BANDS ERR %s", s_cap_on ? "short" : "idle");
        return;
    }
    for (int k = 0; k < BANDS_N; k++) {
        const float w0 = 2.0f * 3.14159265f * centre[k] / (float)RATE;
        const float alpha = sinf(w0) / (2.0f * 1.41f);
        b0[k] = alpha / (1.0f + alpha);
        a1[k] = -2.0f * cosf(w0) / (1.0f + alpha);
        a2[k] = (1.0f - alpha) / (1.0f + alpha);
    }
    for (uint32_t i = s_wpos - RATE; i < s_wpos; i++) {
        const float x = (float)s_ring[i % RING_SAMPLES];
        sum += x;
        sq += (double)x * x;
        for (int k = 0; k < BANDS_N; k++) {
            const float y = b0[k] * (x - x2[k]) - a1[k] * y1[k] - a2[k] * y2[k];
            x2[k] = x1[k];
            x1[k] = x;
            y2[k] = y1[k];
            y1[k] = y;
            band[k] += (double)y * y;
        }
    }
    emit("BANDS %ld %d %d %d %d %d %d %d %d %d", lrint(sum / RATE), level_db10(sq / RATE),
         level_db10(band[0] / RATE), level_db10(band[1] / RATE), level_db10(band[2] / RATE),
         level_db10(band[3] / RATE), level_db10(band[4] / RATE), level_db10(band[5] / RATE),
         level_db10(band[6] / RATE), level_db10(band[7] / RATE));
}

static int embed_at(uint32_t end, float *emb)
{
    window_copy(end, s_win);
    if (spk_model_embed(s_win, emb) != 0) {
        return -1;
    }
    s_embeds++;
    return 0;
}

/* ---------------------------------------------------------------- enroll */

static void enroll_begin(const req_t *r)
{
    if (!s_model_ok) {
        emit("ENROLL ERR no_model %s", r->name);
        return;
    }
    if (s_state == ST_LIVE) {
        emit("LIVE OFF");
    } else if (s_state == ST_ENROLL) {
        emit("ENROLL ERR cancelled %s", s_enroll_name);
    }
    capture_close();                          /* a run starts from a fresh position */
    s_state = ST_IDLE;
    if (capture_open() != 0) {
        emit("ENROLL ERR mic %s", r->name);
        s_state = ST_IDLE;
        return;
    }
    memcpy(s_enroll_name, r->name, sizeof(s_enroll_name));
    s_enroll_from = s_open_pos + SETTLE_SAMPLES;
    s_enroll_to = s_enroll_from + r->seconds * RATE;
    s_next_tick = s_wpos + TICK_SAMPLES;
    s_state = ST_ENROLL;
    emit("ENROLL START %lu %s", (unsigned long)r->seconds, s_enroll_name);
}

static void enroll_finish(void)
{
    const uint32_t dim = EMBEDDING_DIM;
    float centroid[EMBEDDING_DIM];
    int n = 0;

    capture_close();
    s_state = ST_IDLE;
    for (uint32_t end = s_enroll_from + SPK_MODEL_SAMPLES;
         end <= s_enroll_to && n < ENROLL_MAX_WINDOWS; end += ENROLL_HOP) {
        if (voiced_share(end) >= VOICED_ENROLL && embed_at(end, &s_embs[n * dim]) == 0) {
            emit_vec('e', (unsigned long)n, &s_embs[n * dim]);
            n++;
        }
    }
    if (n < ENROLL_MIN_WINDOWS) {
        emit("ENROLL ERR too_quiet %s", s_enroll_name);
        return;
    }
    float nrm = 0.0f;
    for (uint32_t d = 0; d < dim; d++) {
        float acc = 0.0f;
        for (int i = 0; i < n; i++) {
            acc += s_embs[i * dim + d];
        }
        centroid[d] = acc;
        nrm += acc * acc;
    }
    nrm = sqrtf(nrm);
    if (!(nrm > 1e-6f)) {
        emit("ENROLL ERR too_quiet %s", s_enroll_name);
        return;
    }
    for (uint32_t d = 0; d < dim; d++) {
        centroid[d] /= nrm;
    }
    float quality = 0.0f;
    for (int i = 0; i < n; i++) {
        for (uint32_t d = 0; d < dim; d++) {
            quality += s_embs[i * dim + d] * centroid[d];
        }
    }
    quality /= (float)n;
    /* A re-enrollment replaces the voiceprint: the page records a person again
     * because the old one was poor, not to average the two. */
    if (gallery_upsert(&s_gal, s_enroll_name, centroid) != 0) {
        emit("ENROLL ERR full %s", s_enroll_name);
        return;
    }
    s_gal.count[gallery_find(s_enroll_name)] = n;
    gallery_mark_dirty();
    emit("ENROLL OK %d %.3f %s", n, (double)quality, s_enroll_name);
}

/* ---------------------------------------------------------------- live */

static void live_begin(void)
{
    if (!s_model_ok) {
        emit("ERR no_model");
        return;
    }
    if (s_state == ST_ENROLL) {
        emit("ENROLL ERR cancelled %s", s_enroll_name);
    }
    capture_close();                          /* a run starts from a fresh position */
    s_state = ST_IDLE;
    if (capture_open() != 0) {
        emit("ERR mic");
        s_state = ST_IDLE;
        return;
    }
    diarize_init(&s_anon);
    s_anon_named = 0;
    s_live_from = s_open_pos + SETTLE_SAMPLES;
    /* labels end on whole seconds after the start; the first whole window ends at 3 s */
    s_last_label = s_live_from + HOP_SAMPLES * ((SPK_MODEL_SAMPLES - 1u) / HOP_SAMPLES);
    s_next_tick = s_wpos + TICK_SAMPLES;
    s_segs = 0;
    s_state = ST_LIVE;
    emit("LIVE ON");
}

static float dot(const float *a, const float *b)
{
    float s = 0.0f;
    for (uint32_t d = 0; d < EMBEDDING_DIM; d++) {
        s += a[d] * b[d];
    }
    return s;
}

/* Console only (spk echo on): every enrolled person's cosine x 100 for the
 * second ending at t1 ms, to see how close the runner-up came. */
static void echo_scores(unsigned long t1, const float *sc)
{
    if (!s_echo || !s_gal.n) {
        return;
    }
    printf("[voice] SCORES %lu", t1);
    for (int i = 0; i < s_gal.n; i++) {
        printf(" %d", (int)lrintf(sc[i] * 100.0f));
    }
    printf("\r\n");
}

/* Stranger mode: CENTER_ALPHA x the mean over the session's speakers (enrolled
 * voiceprints, anonymous speakers of CENTER_MIN_WINDOWS+ windows); zero below
 * two, and always zero in normal mode. */
static void session_mean(float *mu)
{
    int n = 0;

    memset(mu, 0, EMBEDDING_DIM * sizeof(float));
    if (!s_stranger) {
        return;
    }
    for (int i = 0; i < s_gal.n; i++, n++) {
        for (uint32_t d = 0; d < EMBEDDING_DIM; d++) {
            mu[d] += s_gal.embeddings[i][d];
        }
    }
    for (int k = 0; k < s_anon.n_clusters; k++) {
        if (s_anon.counts[k] < CENTER_MIN_WINDOWS || (s_anon_named & (1u << k))) {
            continue;
        }
        for (uint32_t d = 0; d < EMBEDDING_DIM; d++) {
            mu[d] += s_anon.centroids[k][d];
        }
        n++;
    }
    if (n < 2) {
        memset(mu, 0, EMBEDDING_DIM * sizeof(float));
        return;
    }
    for (uint32_t d = 0; d < EMBEDDING_DIM; d++) {
        mu[d] *= CENTER_ALPHA / (float)n;
    }
}

/* out = unit(v - mu) */
static void centre(const float *v, const float *mu, float *out)
{
    float nrm = 0.0f;

    for (uint32_t d = 0; d < EMBEDDING_DIM; d++) {
        out[d] = v[d] - mu[d];
        nrm += out[d] * out[d];
    }
    nrm = sqrtf(nrm) + 1e-8f;
    for (uint32_t d = 0; d < EMBEDDING_DIM; d++) {
        out[d] /= nrm;
    }
}

/* diarize_update with the match already made on compensated scores: the
 * window (uncompensated) joins anonymous speaker ka when it scored the
 * new-speaker threshold there, else starts a new one. -1: table full. */
static int anon_assign(int ka, float best)
{
    if (ka >= 0 && best >= s_anon_thr) {
        float *c = s_anon.centroids[ka];
        const float n = (float)s_anon.counts[ka];
        float nrm = 0.0f;
        for (uint32_t d = 0; d < EMBEDDING_DIM; d++) {
            c[d] = (c[d] * n + s_emb[d]) / (n + 1.0f);
            nrm += c[d] * c[d];
        }
        nrm = sqrtf(nrm) + 1e-8f;
        for (uint32_t d = 0; d < EMBEDDING_DIM; d++) {
            c[d] /= nrm;
        }
        s_anon.counts[ka]++;
        return ka;
    }
    if (s_anon.n_clusters >= DIARIZE_MAX_CLUSTERS) {
        return -1;
    }
    const int k = s_anon.n_clusters++;
    memcpy(s_anon.centroids[k], s_emb, sizeof(s_emb));
    s_anon.counts[k] = 1;
    return k;
}

/* The best enrolled person at or above the name threshold, or -1. */
static int pick_enrolled(const float *sc)
{
    int best = -1;
    for (int i = 0; i < s_gal.n; i++) {
        if (best < 0 || sc[i] > sc[best]) {
            best = i;
        }
    }
    return (best >= 0 && sc[best] >= s_gal.threshold) ? best : -1;
}

static void live_label(uint32_t end)
{
    const unsigned long t0 = (unsigned long)((end - HOP_SAMPLES - s_live_from) / 16u);
    const unsigned long t1 = (unsigned long)((end - s_live_from) / 16u);

    s_segs++;
    if (voiced_share(end) < VOICED_LIVE || embed_at(end, s_emb) != 0) {
        emit("SEG %lu %lu silence -1 0.00 -", t0, t1);
        return;
    }
    emit_vec('l', t1, s_emb);
    float mu[EMBEDDING_DIM], x[EMBEDDING_DIM], v[EMBEDDING_DIM];
    session_mean(mu);
    centre(s_emb, mu, x);
    float sc[GALLERY_MAX_SPEAKERS];
    for (int i = 0; i < s_gal.n; i++) {
        centre(s_gal.embeddings[i], mu, v);
        sc[i] = dot(v, x);
    }
    echo_scores(t1, sc);
    int ka = -1;
    float best_a = -2.0f;
    for (int k = 0; k < s_anon.n_clusters; k++) {
        centre(s_anon.centroids[k], mu, v);
        const float s = dot(v, x);
        if (s > best_a) {
            best_a = s;
            ka = k;
        }
    }
    int id = pick_enrolled(sc);
    if (s_stranger && id >= 0 && ka >= 0 && best_a > sc[id] + ANON_WIN_MARGIN) {
        id = -1;
    }
    if (id >= 0) {
        emit("SEG %lu %lu id %d %.2f %s", t0, t1, id, (double)sc[id], s_gal.names[id]);
        publish_speaker(t1, id, sc[id], s_gal.names[id]);
        return;
    }
    const int k = anon_assign(ka, best_a);
    if (k < 0) {
        emit("SEG %lu %lu silence -1 0.00 -", t0, t1);
        return;
    }
    emit("SEG %lu %lu anon %d %.2f %d", t0, t1, k, (double)(k == ka ? best_a : 1.0f),
         s_anon.counts[k]);
    publish_speaker(t1, -1, k == ka ? best_a : 1.0f, "");
}

static void live_step(void)
{
    if (s_wpos >= s_last_label + HOP_SAMPLES) {
        s_last_label += HOP_SAMPLES * ((s_wpos - s_last_label) / HOP_SAMPLES);
        live_label(s_last_label);
    }
}

/* ---------------------------------------------------------------- requests */

static void do_request(const req_t *r)
{
    switch (r->type) {
    case REQ_ENROLL:
        enroll_begin(r);
        break;
    case REQ_LIVE_ON:
        live_begin();
        break;
    case REQ_LIVE_OFF:
    case REQ_STOP:
        if (s_state == ST_ENROLL) {
            emit("ENROLL ERR cancelled %s", s_enroll_name);
        } else if (s_state == ST_LIVE) {
            emit("LIVE OFF");
        }
        capture_close();
        s_state = ST_IDLE;
        break;
    case REQ_DELETE: {
        const int i = gallery_find(r->name);
        if (i < 0) {
            emit("DELETE ERR not_found %s", r->name);
        } else {
            gallery_remove(&s_gal, i);
            gallery_mark_dirty();
            emit("DELETE OK %s", r->name);
        }
        break;
    }
    case REQ_NAME:
        if (r->k < 0 || r->k >= s_anon.n_clusters) {
            emit("NAME ERR no_speaker %d %s", r->k, r->name);
        } else if (gallery_upsert(&s_gal, r->name, s_anon.centroids[r->k]) != 0) {
            emit("NAME ERR full %d %s", r->k, r->name);
        } else {
            s_gal.count[gallery_find(r->name)] = s_anon.counts[r->k];
            s_anon_named |= 1u << r->k;        /* now in the gallery: one vote, not two */
            gallery_mark_dirty();
            emit("NAME OK %d %s", r->k, r->name);
        }
        break;
    case REQ_THRESH:
        s_gal.threshold = r->a;
        s_anon_thr = r->b;
        gallery_mark_dirty();
        emit("THRESH %.2f %.2f", (double)s_gal.threshold, (double)s_anon_thr);
        break;
    case REQ_MODE:
        /* each mode's thresholds are on its own scale: switching resets them */
        s_stranger = r->k != 0;
        s_gal.threshold = s_stranger ? STRANGER_ID_THRESHOLD : ID_THRESHOLD;
        s_anon_thr = ANON_THRESHOLD;
        gallery_mark_dirty();
        emit("MODE %s %.2f %.2f", s_stranger ? "STRANGER" : "NORMAL", (double)s_gal.threshold,
             (double)s_anon_thr);
        break;
    case REQ_BANDS:
        emit_bands();
        break;
    case REQ_EMBED:
        if (!s_model_ok || spk_model_embed(s_test_pcm, s_emb) != 0) {
            emit("EMB ERR");
        } else {
            /* Integers (value x 32767): compact, and independent of how the
             * SDK's printf formats floats. The count of NaNs comes first. */
            static char vec[sizeof(s_line)];
            spk_model_info_t mi;
            int nan = 0;
            char *p = vec;
            spk_model_get_info(&mi);
            for (uint32_t d = 0; d < EMBEDDING_DIM; d++) {
                const float v = s_emb[d];
                nan += (v != v);
                p += snprintf(p, 12, " %d", (v != v) ? 0 : (int)lrintf(v * 32767.0f));
            }
            emit("EMB %lu %d%s", (unsigned long)mi.last_us, nan, vec);
        }
        s_pcm_busy = false;
        break;
    case REQ_HELLO: {
        spk_model_info_t mi;
        spk_model_get_info(&mi);
        emit("READY %s %d %d %.2f %.2f %s %s %s", s_model_ok ? mi.name : "none", EMBEDDING_DIM,
             s_gal.n, (double)s_gal.threshold, (double)s_anon_thr, state_name(s_state),
             s_src_host ? "host" : "mic", s_stranger ? "stranger" : "normal");
        break;
    }
    case REQ_SRC:
        if (s_state != ST_IDLE) {
            emit("ERR busy SRC");
        } else {
            s_src_host = r->k != 0;
            emit("SRC %s", s_src_host ? "host" : "mic");
        }
        break;
    case REQ_LIST:
        for (int i = 0; i < s_gal.n; i++) {
            emit("SPK %d %d %s", i, s_gal.count[i], s_gal.names[i]);
        }
        emit("SPK END %d", s_gal.n);
        break;
    }
}

static void voice_task(void *arg)
{
    req_t r;

    (void)arg;
    for (;;) {
        while (xQueueReceive(s_q, &r, s_cap_on ? 0 : pdMS_TO_TICKS(200)) == pdTRUE) {
            do_request(&r);
        }
        gallery_save_if_due();
        if (!s_cap_on) {
            continue;
        }
        capture_pump();
        if (s_wpos >= s_next_tick) {
            s_next_tick += TICK_SAMPLES;
            emit_level(s_state == ST_LIVE ? s_live_from : s_enroll_from);
        }
        if (s_state == ST_ENROLL && s_wpos >= s_enroll_to) {
            enroll_finish();
        } else if (s_state == ST_LIVE) {
            live_step();
        }
    }
}

/* ---------------------------------------------------------------- API */

static int post(const req_t *r)
{
    return (s_q && xQueueSend(s_q, r, 0) == pdTRUE) ? 0 : -1;
}

static int post_named(req_type_t type, const char *name, uint32_t seconds, int k)
{
    req_t r = { .type = type, .seconds = seconds, .k = k };

    if (!name || !name[0] || strlen(name) > VOICE_NAME_MAX || strpbrk(name, "\r\n")) {
        return -1;
    }
    strcpy(r.name, name);
    return post(&r);
}

int voice_enroll(const char *name, uint32_t seconds)
{
    if (seconds < VOICE_ENROLL_MIN_S || seconds > VOICE_ENROLL_MAX_S) {
        return -1;
    }
    return post_named(REQ_ENROLL, name, seconds, 0);
}

int voice_live(bool on)
{
    req_t r = { .type = on ? REQ_LIVE_ON : REQ_LIVE_OFF };
    return post(&r);
}

int voice_stop(void)
{
    req_t r = { .type = REQ_STOP };
    return post(&r);
}

int voice_delete(const char *name)
{
    return post_named(REQ_DELETE, name, 0, 0);
}

int voice_name_anon(int k, const char *name)
{
    return post_named(REQ_NAME, name, 0, k);
}

int voice_set_mode(bool stranger)
{
    req_t r = { .type = REQ_MODE, .k = stranger ? 1 : 0 };

    return post(&r);
}

int voice_set_thresholds(float id, float anon)
{
    req_t r = { .type = REQ_THRESH, .a = id, .b = anon };

    if (!(id > 0.0f && id < 1.0f && anon > 0.0f && anon < 1.0f)) {
        return -1;
    }
    return post(&r);
}

int voice_embed_pcm(const int16_t *pcm, uint32_t samples)
{
    req_t r = { .type = REQ_EMBED };

    if (samples != SPK_MODEL_SAMPLES || s_pcm_busy) {
        return -1;
    }
    s_pcm_busy = true;
    memcpy(s_test_pcm, pcm, SPK_MODEL_SAMPLES * sizeof(int16_t));
    if (post(&r) != 0) {
        s_pcm_busy = false;
        return -1;
    }
    return 0;
}

int voice_set_source(bool host)
{
    req_t r = { .type = REQ_SRC, .k = host };
    return post(&r);
}

int voice_feed(const int16_t *pcm, uint32_t samples)
{
    const uint32_t bytes = samples * sizeof(int16_t);
    uint32_t off = 0;

    /* Short waits, not portMAX_DELAY: capture_close() resets the buffer, which
     * FreeRTOS refuses while a sender is blocked on it. */
    while (off < bytes && s_src_host && s_cap_on) {
        off += (uint32_t)xStreamBufferSend(s_feed, (const uint8_t *)pcm + off, bytes - off,
                                           pdMS_TO_TICKS(100));
    }
    s_fed += off / sizeof(int16_t);
    return off == bytes ? 0 : -1;
}

void voice_report_hello(void)
{
    req_t r = { .type = REQ_HELLO };
    post(&r);
}

void voice_report_bands(void)
{
    req_t r = { .type = REQ_BANDS };
    post(&r);
}

void voice_report_list(void)
{
    req_t r = { .type = REQ_LIST };
    post(&r);
}

void voice_set_sink(voice_sink_t sink)
{
    s_sink = sink;
}

void voice_set_echo(bool on)
{
    s_echo = on;
}

void voice_set_vectors(bool on)
{
    s_vectors = on;
}

void voice_print_status(void)
{
    spk_model_info_t mi;

    spk_model_get_info(&mi);
    printf("model   : %s (%lu B, dim %lu, last embed %lu us)\r\n",
           s_model_ok ? mi.name : "none", (unsigned long)mi.blob_bytes, (unsigned long)mi.dim,
           (unsigned long)mi.last_us);
    printf("state   : %s, source %s (%s), %lu labels, %lu embeddings, VAD floor %.1f dBFS, "
           "%lu samples fed\r\n", state_name(s_state), s_src_host ? "host" : "mic",
           s_cap_on ? "on" : "off", (unsigned long)s_segs, (unsigned long)s_embeds,
           (double)s_floor_db, (unsigned long)s_fed);
    printf("gallery : %d speaker(s), id threshold %.2f, anon threshold %.2f, %d anonymous, "
           "%s mode\r\n", s_gal.n, (double)s_gal.threshold, (double)s_anon_thr,
           s_anon.n_clusters, s_stranger ? "stranger" : "normal");
    for (int i = 0; i < s_gal.n; i++) {
        printf("  %2d %-32s %d window(s), cos x100 to", i, s_gal.names[i], s_gal.count[i]);
        for (int j = 0; j < s_gal.n; j++) {
            printf(" %d", (int)lrintf(dot(s_gal.embeddings[i], s_gal.embeddings[j]) * 100.0f));
        }
        printf("\r\n");
    }
}

int voice_init(void)
{
    s_ring = pvPortMalloc(RING_SAMPLES * sizeof(int16_t));
    s_win  = pvPortMalloc(SPK_MODEL_SAMPLES * sizeof(int16_t));
    s_test_pcm = pvPortMalloc(SPK_MODEL_SAMPLES * sizeof(int16_t));
    s_embs = pvPortMalloc(ENROLL_MAX_WINDOWS * EMBEDDING_DIM * sizeof(float));
    s_q    = xQueueCreate(QUEUE_LEN, sizeof(req_t));
    s_feed = xStreamBufferCreate(FEED_BUF_BYTES, sizeof(int16_t));
    if (!s_ring || !s_win || !s_test_pcm || !s_embs || !s_q || !s_feed) {
        IPRO_LOGE(LOG_TAG, "out of memory");
        return -1;
    }
    memset(s_ring, 0, RING_SAMPLES * sizeof(int16_t));
    ipro_ai_event_init(16, true);                 /* named segments as events (ipro_ai_event.h) */
    s_model_ok = (spk_model_init() == 0);
    gallery_load();
    diarize_init(&s_anon);
    s_anon_named = 0;
    if (xTaskCreate(voice_task, "voice", VOICE_STACK_WORDS, NULL, VOICE_PRIO, NULL) != pdPASS) {
        return -1;
    }
    return s_model_ok ? 0 : -1;
}
