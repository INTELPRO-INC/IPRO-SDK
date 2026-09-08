/**
 * @file mp3_decoder.h
 * @brief MP3 decoder with optional DSP acceleration
 *
 * Frame-by-frame MP3 decoding backed by minimp3. When CONFIG_MP3_DSP_ACCEL
 * is enabled, inner loops use NMSIS-DSP functions for faster throughput.
 *
 * Usage:
 *   mp3_decoder_t *dec = mp3_decoder_create();
 *   mp3_decoder_open(dec, "/sd/song.mp3");
 *   mp3_info_t info;
 *   mp3_decoder_get_info(dec, &info);
 *   int16_t pcm[MP3_MAX_SAMPLES_PER_FRAME * 2];
 *   int n;
 *   while (mp3_decoder_decode_frame(dec, pcm, &n) == 0) { ... }
 *   mp3_decoder_close(dec);
 *   mp3_decoder_destroy(dec);
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum samples per channel per MP3 frame (MPEG-1 Layer III) */
#define MP3_MAX_SAMPLES_PER_FRAME  1152

/** File read chunk size (bytes). Tune for SD card read performance. */
#define MP3_READ_BUF_SIZE          (8 * 1024)

/** MP3 stream information (populated after open) */
typedef struct {
    uint32_t sample_rate;   /**< Sample rate in Hz (e.g. 44100) */
    uint8_t  channels;      /**< 1 = mono, 2 = stereo */
    uint32_t bitrate;       /**< Average bitrate in kbps */
    uint32_t data_bytes;    /**< Total file size in bytes */
} mp3_info_t;

/** Per-frame decode statistics for profiling */
typedef struct {
    uint32_t frame_count;
    uint32_t decode_us_min;
    uint32_t decode_us_max;
    uint64_t decode_us_total;
    uint64_t decode_cycles_total;
    uint32_t peak_heap_bytes;
} mp3_profile_stats_t;

/** Opaque decoder handle */
typedef struct mp3_decoder mp3_decoder_t;

/**
 * Allocate and initialize a decoder instance.
 * @return Decoder handle, or NULL on allocation failure.
 */
mp3_decoder_t *mp3_decoder_create(void);

/**
 * Free decoder instance and all associated resources.
 */
void mp3_decoder_destroy(mp3_decoder_t *dec);

/**
 * Open an MP3 file for decoding.
 * Reads enough data to parse the first frame header.
 * @param dec   Decoder handle
 * @param path  Filesystem path (EVFS/POSIX)
 * @return 0 on success, -1 on file error, -2 on invalid MP3
 */
int mp3_decoder_open(mp3_decoder_t *dec, const char *path);

/**
 * Get stream info (valid after open).
 */
int mp3_decoder_get_info(mp3_decoder_t *dec, mp3_info_t *info);

/**
 * Decode the next MP3 frame.
 * @param dec          Decoder handle
 * @param pcm_out      Output buffer (at least MP3_MAX_SAMPLES_PER_FRAME * channels * sizeof(int16_t))
 * @param samples_out  Number of samples decoded per channel
 * @return 0 on success, 1 on end-of-file, -1 on decode error
 */
int mp3_decoder_decode_frame(mp3_decoder_t *dec,
                             int16_t *pcm_out,
                             int *samples_out);

/**
 * Close the current file. Decoder can be reused with another open() call.
 */
void mp3_decoder_close(mp3_decoder_t *dec);

/**
 * Get accumulated profile statistics (valid during/after decoding).
 */
int mp3_decoder_get_stats(mp3_decoder_t *dec, mp3_profile_stats_t *stats);

/**
 * Reset profile statistics counters.
 */
void mp3_decoder_reset_stats(mp3_decoder_t *dec);

#ifdef __cplusplus
}
#endif

#endif /* MP3_DECODER_H */
