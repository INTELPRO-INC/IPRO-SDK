/**
 * @file mp3_decoder.c
 * @brief MP3 decoder implementation backed by minimp3
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#include "mp3_decoder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <FreeRTOS.h>
#include "system_soc.h"
#include <drv_clock.h>

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_NO_SIMD

/* RISC-V core header defines LOAD as asm mnemonic; minimp3 redefines it
 * as a function-like macro for synth filter. Undef to avoid -Werror clash. */
#ifdef LOAD
#undef LOAD
#endif
#ifdef STORE
#undef STORE
#endif

#include "minimp3.h"

struct mp3_decoder {
    mp3dec_t           mp3d;
    int                fd;
    uint8_t           *file_buf;      /* aligned read buffer */
    uint8_t           *file_buf_raw;  /* raw allocation (for free) */
    uint32_t           buf_size;
    uint32_t           buf_filled;
    uint32_t           buf_pos;
    uint32_t           buf_start;     /* buf_pos right after refill (= sector_skip) */
    uint32_t           file_pos;      /* next unconsumed byte in file */
    uint32_t           audio_offset;  /* file offset where audio data starts (after ID3) */
    mp3_info_t         info;
    int                header_parsed;
    mp3_profile_stats_t stats;
};

static inline uint32_t read_mcycle(void)
{
    uint32_t cycles;
    __asm__ volatile("csrr %0, mcycle" : "=r"(cycles));
    return cycles;
}

static inline uint32_t cycles_to_us(uint32_t cycles)
{
    return (uint32_t)((uint64_t)cycles * 1000000ULL / SystemCoreClockGet());
}

static int refill_buffer(mp3_decoder_t *dec)
{
    /* Advance file_pos by ACTUAL consumed bytes (exclude sector padding) */
    dec->file_pos += dec->buf_pos - dec->buf_start;
    dec->buf_pos = 0;
    dec->buf_start = 0;
    dec->buf_filled = 0;

    /* Align read to 512-byte sector for SDH DMA compatibility */
    uint32_t sector_aligned = dec->file_pos & ~511U;
    uint32_t sector_skip = dec->file_pos - sector_aligned;

    lseek(dec->fd, sector_aligned, SEEK_SET);
    int n = read(dec->fd, dec->file_buf, dec->buf_size);
    if (n > 0) {
        dec->buf_filled = n;
        dec->buf_pos = sector_skip;
        dec->buf_start = sector_skip;
    }

    return dec->buf_filled - dec->buf_pos;
}

mp3_decoder_t *mp3_decoder_create(void)
{
    mp3_decoder_t *dec = pvPortMalloc(sizeof(mp3_decoder_t));
    if (!dec) {
        return NULL;
    }
    memset(dec, 0, sizeof(*dec));
    dec->fd = -1;

    /* Allocate with extra room for 8-byte alignment (SDH DMA requirement) */
    dec->file_buf_raw = pvPortMalloc(MP3_READ_BUF_SIZE + 8);
    if (!dec->file_buf_raw) {
        vPortFree(dec);
        return NULL;
    }
    dec->file_buf = (uint8_t *)(((uintptr_t)dec->file_buf_raw + 7) & ~(uintptr_t)7);
    dec->buf_size = MP3_READ_BUF_SIZE;

    mp3dec_init(&dec->mp3d);
    return dec;
}

void mp3_decoder_destroy(mp3_decoder_t *dec)
{
    if (!dec) {
        return;
    }
    mp3_decoder_close(dec);
    vPortFree(dec->file_buf_raw);
    vPortFree(dec);
}

int mp3_decoder_open(mp3_decoder_t *dec, const char *path)
{
    if (!dec || !path) {
        return -1;
    }

    mp3_decoder_close(dec);
    mp3dec_init(&dec->mp3d);
    memset(&dec->info, 0, sizeof(dec->info));
    memset(&dec->stats, 0, sizeof(dec->stats));
    dec->stats.decode_us_min = UINT32_MAX;
    dec->header_parsed = 0;
    dec->buf_filled = 0;
    dec->buf_pos = 0;
    dec->file_pos = 0;

    struct stat st;
    if (stat(path, &st) != 0) {
        printf("mp3: stat %s failed\n", path);
        return -1;
    }
    dec->info.data_bytes = st.st_size;

    dec->fd = open(path, O_RDONLY);
    if (dec->fd < 0) {
        printf("mp3: open %s failed\n", path);
        return -1;
    }

    /* Initial fill */
    refill_buffer(dec);
    if (dec->buf_filled < 128) {
        printf("mp3: file too small\n");
        mp3_decoder_close(dec);
        return -2;
    }

    /* Quick format check — reject MP4/M4A containers masquerading as .mp3 */
    if (dec->buf_filled >= 8 && memcmp(dec->file_buf + 4, "ftyp", 4) == 0) {
        printf("mp3: file is MP4/M4A container, not MP3\n");
        mp3_decoder_close(dec);
        return -2;
    }

    /* Detect ID3v2 tag size for rewind reference.
     * Don't seek here — let minimp3 skip it naturally to avoid
     * FATFS unaligned sector buffer issues from non-sector-aligned seeks. */
    dec->audio_offset = 0;
    if (dec->buf_filled >= 10 && memcmp(dec->file_buf, "ID3", 3) == 0) {
        uint8_t *h = dec->file_buf;
        size_t id3_size = (((h[6] & 0x7f) << 21) | ((h[7] & 0x7f) << 14) |
                           ((h[8] & 0x7f) << 7) | (h[9] & 0x7f)) + 10;
        if (h[5] & 0x10) {
            id3_size += 10;
        }
        printf("mp3: ID3v2 tag detected (%lu bytes)\n", (unsigned long)id3_size);

        if (id3_size > dec->buf_size) {
            /* Large ID3 tag (cover art) — seek past it, aligned to 512-byte sector */
            dec->audio_offset = (id3_size + 511) & ~511U;
            dec->file_pos = dec->audio_offset;
            dec->buf_filled = 0;
            dec->buf_pos = 0;
            refill_buffer(dec);
        }
        /* Small tags (<8KB) stay in buffer; minimp3 skips them automatically */
    }

    /* Find first valid audio frame for stream info */
    mp3dec_frame_info_t frame_info;
    int16_t *tmp_pcm = pvPortMalloc(MP3_MAX_SAMPLES_PER_FRAME * 2 * sizeof(int16_t));
    if (!tmp_pcm) {
        printf("mp3: tmp_pcm alloc failed\n");
        mp3_decoder_close(dec);
        return -1;
    }

    int found = 0;
    for (int attempt = 0; attempt < 200; attempt++) {
        uint32_t avail = dec->buf_filled - dec->buf_pos;
        if (avail < 128) {
            refill_buffer(dec);
            avail = dec->buf_filled - dec->buf_pos;
            if (avail < 128) {
                break;
            }
        }

        int samples = mp3dec_decode_frame(&dec->mp3d,
                                          dec->file_buf + dec->buf_pos,
                                          avail,
                                          tmp_pcm, &frame_info);
        if (frame_info.frame_bytes == 0) {
            break;
        }
        dec->buf_pos += frame_info.frame_bytes;

        if (samples > 0 && frame_info.hz > 0) {
            found = 1;
            break;
        }
        /* samples==0 means non-audio frame (ID3, Xing header, etc.) — skip */
    }
    vPortFree(tmp_pcm);

    if (!found) {
        printf("mp3: no valid audio frame found\n");
        mp3_decoder_close(dec);
        return -2;
    }

    dec->info.sample_rate = frame_info.hz;
    dec->info.channels = frame_info.channels;
    dec->info.bitrate = frame_info.bitrate_kbps;
    dec->header_parsed = 1;

    /* Rewind to audio start for clean decoding */
    dec->file_pos = dec->audio_offset;
    dec->buf_filled = 0;
    dec->buf_pos = 0;
    mp3dec_init(&dec->mp3d);
    refill_buffer(dec);

    return 0;
}

int mp3_decoder_get_info(mp3_decoder_t *dec, mp3_info_t *info)
{
    if (!dec || !info || !dec->header_parsed) {
        return -1;
    }
    *info = dec->info;
    return 0;
}

int mp3_decoder_decode_frame(mp3_decoder_t *dec,
                             int16_t *pcm_out,
                             int *samples_out)
{
    if (!dec || dec->fd < 0) {
        return -1;
    }

    *samples_out = 0;

    for (;;) {
        /* Ensure we have data to decode */
        uint32_t avail = dec->buf_filled - dec->buf_pos;
        if (avail < 1024) {
            refill_buffer(dec);
            avail = dec->buf_filled - dec->buf_pos;
        }

        if (avail == 0) {
            return 1; /* EOF */
        }

        mp3dec_frame_info_t frame_info;

        uint32_t cyc_start = read_mcycle();
        int samples = mp3dec_decode_frame(&dec->mp3d,
                                          dec->file_buf + dec->buf_pos,
                                          avail,
                                          pcm_out, &frame_info);
        uint32_t cyc_end = read_mcycle();

        if (frame_info.frame_bytes == 0) {
            return 1; /* No more frames */
        }

        dec->buf_pos += frame_info.frame_bytes;

        if (frame_info.bitrate_kbps > 0) {
            dec->info.bitrate = frame_info.bitrate_kbps;
        }

        if (samples > 0) {
            *samples_out = samples;

            uint32_t elapsed_cycles = cyc_end - cyc_start;
            uint32_t elapsed_us = cycles_to_us(elapsed_cycles);

            dec->stats.frame_count++;
            dec->stats.decode_us_total += elapsed_us;
            dec->stats.decode_cycles_total += elapsed_cycles;
            if (elapsed_us < dec->stats.decode_us_min) {
                dec->stats.decode_us_min = elapsed_us;
            }
            if (elapsed_us > dec->stats.decode_us_max) {
                dec->stats.decode_us_max = elapsed_us;
            }
            return 0;
        }
        /* samples==0: non-audio frame (ID3/Xing), loop to next */
    }
}

void mp3_decoder_close(mp3_decoder_t *dec)
{
    if (!dec) {
        return;
    }
    if (dec->fd >= 0) {
        close(dec->fd);
        dec->fd = -1;
    }
    dec->header_parsed = 0;
}

int mp3_decoder_get_stats(mp3_decoder_t *dec, mp3_profile_stats_t *stats)
{
    if (!dec || !stats) {
        return -1;
    }
    *stats = dec->stats;
    /* Fix min if no frames decoded */
    if (stats->frame_count == 0) {
        stats->decode_us_min = 0;
    }
    return 0;
}

void mp3_decoder_reset_stats(mp3_decoder_t *dec)
{
    if (dec) {
        memset(&dec->stats, 0, sizeof(dec->stats));
        dec->stats.decode_us_min = UINT32_MAX;
    }
}
