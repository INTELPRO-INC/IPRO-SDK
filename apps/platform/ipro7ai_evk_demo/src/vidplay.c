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
 * @file vidplay.c
 * @brief SD card -> panel + DAC. See vidplay.h for the format and the limits.
 */
#include "vidplay.h"

#include <string.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "ff.h"
#include "st7789p3.h"
#include "audio.h"
#include "audio_output.h"
#include "sdcard.h"
#include "hal_mtimer.h"
#include "ipro_log.h"

#define LOG_TAG "VID"

#define VID_MAGIC   0x31565049u          /* "IPV1" little-endian */
#define VID_STACK   1536
#define VID_PRIO    (configMAX_PRIORITIES - 4)

/* blit_push takes a uint16_t count, and a frame is more pixels than that. */
#define VID_CHUNK_PIX 32768u

/*
 * Build-time defaults for the two demo switches, both settable at runtime.
 * CMakeLists.txt reads them out of .config - see the note there about why an
 * app-private option cannot come through generated/autoconf.h.
 */
#ifdef CONFIG_EVK_VIDEO_MUTE
#define VID_MUTE_DEFAULT   true
#else
#define VID_MUTE_DEFAULT   false
#endif

#ifdef CONFIG_EVK_VIDEO_CLIP
#define VID_CLIP_DEFAULT   (CONFIG_EVK_VIDEO_CLIP)
#else
#define VID_CLIP_DEFAULT   (-1)          /* -1 = walk the whole list */
#endif

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint16_t width, height, fps, arate;
    uint32_t frames;
} vid_hdr_t;

static volatile TaskHandle_t s_task;
static volatile bool         s_run;
static int                   s_index;
static vidplay_status_t      s_st;
static volatile bool         s_mute = VID_MUTE_DEFAULT;

/*
 * Silence source for muted playback.
 *
 * The DAC still runs while muted, and deliberately so: the ring is what paces
 * the frame loop (see the write in vid_task), so a player that skipped the
 * audio would blit as fast as the SPI bus allows and the clip would run at
 * several times its own frame rate. Muting therefore substitutes the samples
 * rather than removing the write - same sample count, same pacing, no signal.
 * .bss, so it is already zero; 256 samples is 16 ms at 16 kHz, and the loop
 * below covers whatever a frame actually needs.
 */
static const int16_t s_silence[256];

/*
 * The whole clip is loaded into PSRAM before a single frame is shown.
 *
 * Streaming it off the card while driving the panel did not work: the SDH
 * driver polls a status register with no yield, and with the panel push
 * running alongside it, reads eventually blew the driver's 5 s timeout and
 * playback stopped part way. The card itself was never the problem - reading
 * the same 8 MB file straight through with nothing else running sustains
 * 8.7 MB/s with no errors.
 *
 * So the card is used once, up front, with nothing competing. Playback then
 * touches nothing but PSRAM, the panel and the DAC, which also gives the blit
 * the whole core back - streaming cost it about 27 ms a frame in contention.
 *
 * The price is that a clip has to fit in the PSRAM heap.
 */
static uint8_t  *s_clip;        /* the entire payload, frames back to back */
static uint32_t  s_clip_bytes;  /* bytes in use by the current clip        */
static uint32_t  s_clip_cap;    /* bytes actually allocated                */

/*
 * Claim one buffer, as large as the heap will give, and never resize it.
 *
 * Sizing it to each clip was worse than not caching at all: a clip larger than
 * the one before frees the old buffer and asks for a bigger one, and that
 * free-then-allocate is exactly where fragmentation bites. Measured here,
 * 6.16 MB succeeded on a clean heap and 5.39 MB failed later, purely because a
 * 5.03 MB buffer had been held and released in between.
 *
 * So the first play takes the biggest block available - when the heap is at
 * its least fragmented - and every clip after that reuses it untouched. There
 * is never a second large allocation, so there is never a second chance to
 * fail.
 *
 * The ceiling on this board is around 6.3 MB against 8.2 MB of total free
 * heap; the difference is structural and not something playback caused.
 */
static bool vid_reserve(uint32_t bytes)
{
    static const uint32_t try_kb[] = { 6656u, 6144u, 5632u, 5120u, 4608u,
                                       4096u, 3072u, 2048u, 1024u };

    if (s_clip) {
        return bytes <= s_clip_cap;      /* never resize - see above */
    }

    for (unsigned i = 0; i < sizeof(try_kb) / sizeof(try_kb[0]); i++) {
        uint32_t want = try_kb[i] * 1024u;

        if (want < bytes) {
            break;                        /* below this it cannot help */
        }
        s_clip = pvPortMalloc(want);
        if (s_clip) {
            s_clip_cap = want;
            IPRO_LOGI(LOG_TAG, "reserved %u KB for clips - kept for the rest "
                               "of this session so no later play has to "
                               "allocate again",
                      (unsigned)(want / 1024u));
            return true;
        }
    }
    return false;
}

static void vid_free(void)
{
    s_clip_bytes = 0;   /* the buffer itself is kept - see vid_reserve() */
}

/*
 * What the heap will actually hand out in one piece.
 *
 * xPortGetFreeHeapSize() is the total across all free blocks, which is what
 * made a 4.36 MB request failing with 8.2 MB free look impossible. Coarse
 * steps on purpose: every miss trips the malloc-failed hook and prints, so a
 * fine search buries its own answer.
 */
static uint32_t vid_largest_block(void)
{
    uint32_t mb;

    for (mb = 8u; mb >= 1u; mb--) {
        void *p = pvPortMalloc(mb * 1024u * 1024u);

        if (p) {
            vPortFree(p);
            return mb * 1024u * 1024u;
        }
    }
    return 0;
}

static void vid_blit(const uint16_t *px, uint32_t npix)
{
    st7789p3_bus_lock();
    st7789p3_blit_start(0, 0, (uint16_t)(s_st.width - 1u),
                        (uint16_t)(s_st.height - 1u));
    while (npix) {
        uint32_t n = (npix > VID_CHUNK_PIX) ? VID_CHUNK_PIX : npix;

        st7789p3_blit_push(px, (uint16_t)n);
        px   += n;
        npix -= n;
    }
    st7789p3_blit_end();
    st7789p3_bus_unlock();
}

static void vid_write_silence(audio_output_t *out, uint32_t nsamp)
{
    while (nsamp) {
        uint32_t n = (nsamp > (uint32_t)(sizeof(s_silence) / sizeof(s_silence[0])))
                     ? (uint32_t)(sizeof(s_silence) / sizeof(s_silence[0]))
                     : nsamp;

        audio_output_write(out, s_silence, n);
        nsamp -= n;
    }
}

static void vid_task(void *arg)
{
    char path[24];
    FRESULT fr;
    UINT got;
    vid_hdr_t h;
    audio_output_t *out = NULL;
    uint32_t asamp, npix, fbytes, abytes, chunk;
    uint64_t t0, t_start, blit_us = 0;
    bool opened = false;
    FIL fp;

    (void)arg;

    /* The card is registered as volume "sd", so the prefix is not optional:
     * a bare name goes to drive 0, which has no work area and comes back
     * FR_NOT_ENABLED rather than FR_NO_FILE. */
    snprintf(path, sizeof(path), "sd:/%d.ipv", s_index);

    fr = f_open(&fp, path, FA_READ);
    if (fr != FR_OK) {
        IPRO_LOGE(LOG_TAG, "f_open(\"%s\") failed: %s (%d)", path,
                  (fr == FR_NO_FILE)       ? "no such file" :
                  (fr == FR_NO_PATH)       ? "no such path" :
                  (fr == FR_NOT_ENABLED)   ? "that volume is not mounted" :
                  (fr == FR_NO_FILESYSTEM) ? "no filesystem on the card" :
                  (fr == FR_DISK_ERR)      ? "card read error" : "see FRESULT",
                  (int)fr);
        goto done;
    }
    opened = true;

    if (f_read(&fp, &h, sizeof(h), &got) != FR_OK || got != sizeof(h) ||
        h.magic != VID_MAGIC) {
        IPRO_LOGE(LOG_TAG, "\"%s\" is not a packed clip - run tools/mkvid.py",
                  path);
        goto close;
    }
    if (h.width != st7789p3_width() || h.height != st7789p3_height()) {
        IPRO_LOGW(LOG_TAG, "clip is %ux%u but the panel is %ux%u - repack it",
                  h.width, h.height, st7789p3_width(), st7789p3_height());
        goto close;
    }

    s_st.width = h.width; s_st.height = h.height;
    s_st.fps = h.fps;     s_st.arate = h.arate;
    s_st.frames_total = h.frames;

    npix   = (uint32_t)h.width * h.height;
    fbytes = npix * 2u;
    asamp  = (uint32_t)h.arate / h.fps;
    abytes = asamp * 2u;
    chunk  = abytes + fbytes;
    s_clip_bytes = chunk * h.frames;

    if (!vid_reserve(s_clip_bytes)) {
        IPRO_LOGE(LOG_TAG, "%s needs %u KB and the reserved buffer is %u KB. "
                           "Repack it smaller or at fewer fps - the buffer is "
                           "claimed once and not resized, because resizing is "
                           "what fragmentation turns into a hard failure.",
                  path, (unsigned)(s_clip_bytes / 1024u),
                  (unsigned)(s_clip_cap / 1024u));
        goto close;
    }

    /*
     * Load in one pass, with nothing else running. This is the only time the
     * card is touched; at 8.7 MB/s it is well under a second.
     */
    t0 = mtimer_get_time_us();
    if (f_read(&fp, s_clip, s_clip_bytes, &got) != FR_OK || got != s_clip_bytes) {
        IPRO_LOGE(LOG_TAG, "short read: %u of %u bytes", (unsigned)got,
                  (unsigned)s_clip_bytes);
        goto close;
    }
    f_close(&fp);
    opened = false;
    IPRO_LOGI(LOG_TAG, "%s: %ux%u, %u fps, %u frames, audio %u Hz - loaded "
                       "%u KB in %u ms",
              path, h.width, h.height, h.fps, (unsigned)h.frames, h.arate,
              (unsigned)(s_clip_bytes / 1024u),
              (unsigned)((mtimer_get_time_us() - t0) / 1000u));

    /*
     * Only one activity owns the audio path - and the FP note in bt_audio_out.h
     * applies here too, so nothing else should be doing float work meanwhile.
     */
    audio_stop();
    audio_set_gain(0);
    out = audio_dac_open_ex(h.arate, 4, 64);
    if (!out) {
        IPRO_LOGE(LOG_TAG, "DAC open failed at %u Hz", h.arate);
        goto close;
    }
    if (s_mute) {
        /*
         * Belt and braces. Feeding silence already leaves nothing to hear, but
         * the class-D amplifier idles with audible hiss and turns the DAC's
         * open/close step into the pop described in audio.c - neither of which
         * belongs in a muted demo. open_ex() drove EN high, so undo it.
         */
        audio_spk_enable(0);
        IPRO_LOGI(LOG_TAG, "muted - amplifier off, DAC still paces the frames");
    }

    t_start = mtimer_get_time_us();

    while (s_run && s_st.frames_shown < h.frames) {
        const uint8_t *p = s_clip + (uint32_t)s_st.frames_shown * chunk;

        /*
         * Audio first. It goes into the DAC ring, which is what paces this
         * loop - the write blocks once the ring is full, so the loop runs at
         * the rate the DAC drains and the video follows the audio.
         */
        if (s_mute) {
            vid_write_silence(out, asamp);
        } else {
            audio_output_write(out, (const int16_t *)p, asamp);
        }

        t0 = mtimer_get_time_us();
        vid_blit((const uint16_t *)(p + abytes), npix);
        blit_us += mtimer_get_time_us() - t0;

        s_st.frames_shown++;
    }

    {
        uint64_t dt = mtimer_get_time_us() - t_start;

        if (dt && s_st.frames_shown) {
            s_st.achieved_mfps = (uint32_t)((uint64_t)s_st.frames_shown *
                                            1000000000ull / dt);
            s_st.blit_us = (uint32_t)(blit_us / s_st.frames_shown);
        }
        IPRO_LOGI(LOG_TAG, "%u/%u frames in %u ms -> %u.%03u fps "
                           "(blit %u us per frame, playing from PSRAM)",
                  (unsigned)s_st.frames_shown, (unsigned)h.frames,
                  (unsigned)(dt / 1000u),
                  (unsigned)(s_st.achieved_mfps / 1000u),
                  (unsigned)(s_st.achieved_mfps % 1000u),
                  (unsigned)s_st.blit_us);
    }

close:
    if (out) {
        audio_dac_close(out);
    }
    if (opened) {
        f_close(&fp);
    }
done:
    vid_free();
    s_st.playing = false;
    s_run  = false;
    s_task = NULL;
    vTaskDelete(NULL);
}

/* ---------------------------------------------------------------------------
 * Card watcher
 *
 * Polls the detect pin and, when a card appears, mounts it and plays the
 * clips. Insertion is the trigger rather than boot, so a card put in later
 * works the same as one already seated.
 *
 * The order starts with the largest clip on purpose. The payload buffer is
 * claimed once and never resized (see vid_reserve), so whichever clip plays
 * first decides the reservation for the whole session - starting small would
 * leave every larger clip unable to load.
 * -------------------------------------------------------------------------*/
#define VID_AUTO_STACK   768
#define VID_AUTO_PRIO    (configMAX_PRIORITIES - 6)
#define VID_AUTO_POLL_MS 500

static const uint8_t s_auto_order[] = { 3, 0, 1, 2, 4 };   /* largest first */

static volatile TaskHandle_t s_auto_task;
static volatile bool         s_auto_on = true;
static volatile bool         s_auto_loop = true;
static uint32_t              s_auto_rounds;

/*
 * Which clip autoplay shows: an index pins it to that one clip, -1 walks the
 * list above. A pinned clip also retires the largest-first reasoning - only
 * one clip ever loads, so its own size is the reservation and nothing larger
 * can come after it.
 */
static volatile int          s_auto_clip = VID_CLIP_DEFAULT;

/*
 * Walk the list, and keep walking while the card stays in.
 *
 * The card-detect pin is checked between clips as well as between rounds, so
 * pulling the card stops playback within one clip rather than at the end of
 * the round.
 */
static void vid_auto_play_all(void)
{
    do {
        int      pinned = s_auto_clip;
        unsigned count  = (pinned >= 0) ? 1u : (unsigned)sizeof(s_auto_order);

        for (unsigned i = 0; i < count; i++) {
            int idx = (pinned >= 0) ? pinned : (int)s_auto_order[i];

            if (!s_auto_on || sdcard_detect_raw() != 0) {
                return;
            }
            if (vidplay_start(idx) != 0) {
                continue;
            }
            while (vidplay_running() && s_auto_on) {
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            vTaskDelay(pdMS_TO_TICKS(300));    /* a beat between clips */
        }
        s_auto_rounds++;
    } while (s_auto_loop && s_auto_on && sdcard_detect_raw() == 0);
}

static void vid_auto_task(void *arg)
{
    bool was_seated = false;

    (void)arg;

    for (;;) {
        bool seated = (sdcard_detect_raw() == 0);

        if (s_auto_on && seated && !was_seated) {
            IPRO_LOGI(LOG_TAG, "card detected - mounting");
            vTaskDelay(pdMS_TO_TICKS(200));      /* let contacts settle */
            if (sdcard_mount() == 0) {
                vid_auto_play_all();
            } else {
                IPRO_LOGE(LOG_TAG, "auto-mount failed - `sd mount` for why");
            }
        } else if (!seated && was_seated) {
            IPRO_LOGI(LOG_TAG, "card removed");
            vidplay_stop();
            sdcard_unmount();
        }

        was_seated = seated;
        vTaskDelay(pdMS_TO_TICKS(VID_AUTO_POLL_MS));
    }
}

void vidplay_auto_loop(bool on) { s_auto_loop = on; }

void vidplay_auto_enable(bool on)
{
    s_auto_on = on;
    if (on && !s_auto_task) {
        xTaskCreate(vid_auto_task, "vidauto", VID_AUTO_STACK, NULL,
                    VID_AUTO_PRIO, (TaskHandle_t *)&s_auto_task);
    }
}

bool vidplay_auto_enabled(void) { return s_auto_on; }

void vidplay_set_clip(int index) { s_auto_clip = index; }
int  vidplay_get_clip(void)      { return s_auto_clip; }

void vidplay_set_mute(bool on)   { s_mute = on; }
bool vidplay_muted(void)         { return s_mute; }

int vidplay_start(int index)
{
    if (s_task) {
        return -1;
    }
    if (!sdcard_is_mounted() && sdcard_mount() != 0) {
        return -2;
    }

    s_index = index;
    memset(&s_st, 0, sizeof(s_st));
    s_st.playing = true;
    s_run = true;

    if (xTaskCreate(vid_task, "vidplay", VID_STACK, NULL, VID_PRIO,
                    (TaskHandle_t *)&s_task) != pdPASS) {
        s_run = false;
        s_st.playing = false;
        return -3;
    }
    return 0;
}

/* Hand the payload buffer back, for when the PSRAM is wanted elsewhere. */
void vidplay_release(void)
{
    if (!s_task && s_clip) {
        vPortFree(s_clip);
        s_clip = NULL;
        s_clip_cap = 0;
    }
}

void vidplay_stop(void)      { s_run = false; }
bool vidplay_running(void)   { return s_task != NULL; }

void vidplay_get_status(vidplay_status_t *o)
{
    if (o) {
        *o = s_st;
        o->playing = (s_task != NULL);
    }
}

/* ---------------------------------------------------------------------------
 * Shell
 * -------------------------------------------------------------------------*/
#if defined(CONFIG_SHELL)
#include "shell.h"
#include <stdlib.h>

int cmd_vid(int argc, char **argv)
{
    const char *sub = (argc > 1) ? argv[1] : "status";

    if (strcmp(sub, "play") == 0) {
        int n = (argc > 2) ? atoi(argv[2]) : 0;
        int rc = vidplay_start(n);

        if (rc == -1)      printf("already playing - `vid stop` first\n");
        else if (rc == -2) printf("no card, or it would not mount\n");
        else if (rc != 0)  printf("could not start (%d)\n", rc);
        else               printf("playing %d.ipv\n", n);
    } else if (strcmp(sub, "auto") == 0) {
        bool on = (argc > 2) ? (strcmp(argv[2], "off") != 0) : true;

        vidplay_auto_enable(on);
        printf("auto play %s - inserting a card mounts it and plays the\n"
               "clips, largest first so the one-time buffer is claimed at\n"
               "full size.\n", on ? "on" : "off");
    } else if (strcmp(sub, "loop") == 0) {
        bool on = (argc > 2) ? (strcmp(argv[2], "off") != 0) : true;

        vidplay_auto_loop(on);
        printf("loop %s (%u round%s so far). Removing the card stops it\n"
               "within the current clip, not at the end of the round.\n",
               on ? "on" : "off", (unsigned)s_auto_rounds,
               (s_auto_rounds == 1u) ? "" : "s");
    } else if (strcmp(sub, "mem") == 0) {
        uint32_t total = xPortGetFreeHeapSize();
        uint32_t big;

        printf("probing (each miss prints a malloc-failed line - expected)\n");
        big = vid_largest_block();
        printf("\nheap free  : %u KB total\n", (unsigned)(total / 1024u));
        printf("largest    : %u MB in one piece\n",
               (unsigned)(big / (1024u * 1024u)));
        printf("clip limit : about %u s at 320x240 8 fps\n",
               (unsigned)(big / (8u * (320u * 240u * 2u + 8000u))));
        if (big < total / 2u) {
            printf("\nThe gap between those two is fragmentation: the space\n"
                   "exists but not contiguously, so a clip that fits the total\n"
                   "can still fail. Reboot for the cleanest heap, and load the\n"
                   "largest clip first - the buffer is kept and reused after.\n");
        }
    } else if (strcmp(sub, "mute") == 0) {
        bool on = (argc > 2) ? (strcmp(argv[2], "off") != 0) : true;

        vidplay_set_mute(on);
        printf("sound %s%s\n", on ? "muted" : "on",
               vidplay_running() ? " - takes effect on the next clip for the"
                                   " amplifier, immediately for the samples"
                                 : "");
    } else if (strcmp(sub, "clip") == 0) {
        if (argc > 2 && strcmp(argv[2], "all") == 0) {
            vidplay_set_clip(-1);
            printf("autoplay walks all clips, largest first\n");
        } else if (argc > 2) {
            int n = atoi(argv[2]);

            vidplay_set_clip(n);
            printf("autoplay pinned to %d.ipv\n", n);
        } else {
            int n = vidplay_get_clip();

            if (n >= 0) printf("autoplay pinned to %d.ipv\n", n);
            else        printf("autoplay walks all clips, largest first\n");
        }
    } else if (strcmp(sub, "stop") == 0) {
        vidplay_stop();
        printf("stopping\n");
    } else {
        vidplay_status_t s;

        vidplay_get_status(&s);
        if (!s.width) {
            printf("nothing played yet.\n"
                   "  vid play <n>   play <n>.ipv from the card root\n"
                   "  vid stop\n"
                   "  vid auto on|off\n"
                   "  vid loop on|off\n"
                   "  vid clip <n>|all   which clip autoplay shows\n"
                   "  vid mute on|off\n\n"
                   "Pack clips with tools/mkvid.py - the board has no video\n"
                   "decoder, so the scaling, RGB565 conversion and byte order\n"
                   "are all done on the host and the card holds exactly what\n"
                   "gets pushed to the panel.\n");
            return 0;
        }
        printf("clip    : %ux%u, %u fps, %u frames, audio %u Hz\n",
               s.width, s.height, s.fps, (unsigned)s.frames_total, s.arate);
        printf("state   : %s, %u frames shown\n",
               s.playing ? "playing" : "stopped", (unsigned)s.frames_shown);
        if (s.achieved_mfps) {
            uint32_t slot_us = 1000000u / (s.fps ? s.fps : 1u);

            printf("achieved: %u.%03u fps, blit %u us of the %u us slot\n",
                   (unsigned)(s.achieved_mfps / 1000u),
                   (unsigned)(s.achieved_mfps % 1000u),
                   (unsigned)s.blit_us, (unsigned)slot_us);
            if (s.blit_us > slot_us) {
                /*
                 * The card read runs on its own task now, so it no longer adds
                 * to this - but it does compete for the one core, which is why
                 * the blit reads longer here than the bus alone would take.
                 * 320x240x16 at 16 MHz is 77 ms of pure transfer; the excess
                 * over that is CPU lost to the reader.
                 *
                 * SPI DMA would free the CPU for exactly that overlap, but
                 * SPI0's TX DMA handshake is broken above 8 MHz on this
                 * silicon, and halving the clock costs more than the overlap
                 * wins. So the way out is fewer pixels or fewer frames.
                 */
                printf("hint    : over budget by %u us. Repack lower:\n"
                       "          tools/mkvid.py <src> -o <dir> -f %u\n"
                       "          (pure SPI transfer for this frame is 77 ms;\n"
                       "           anything above that is CPU shared with the\n"
                       "           card reader)\n",
                       (unsigned)(s.blit_us - slot_us),
                       (unsigned)(1000000u / s.blit_us));
            }
        }
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_vid, vid, play a packed clip from the SD card);
#endif /* CONFIG_SHELL */
