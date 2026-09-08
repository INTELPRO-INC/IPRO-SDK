/**
 * @file audio_output_dump.c
 * @brief Raw PCM dump output sink
 *
 * Writes raw int16 PCM to SD card file or custom hook (CDC-ACM, etc.).
 * No buffering, no DMA — synchronous write on each audio_output_write call.
 *
 * When dump_hook is NULL: writes to dump_path as raw file (s16le interleaved).
 * When dump_hook is set: calls hook for each PCM block (app handles transport).
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#include "audio_output.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <FreeRTOS.h>

struct audio_output {
    audio_output_cfg_t cfg;
    int                started;
    int                fd;          /* file descriptor (file dump mode) */
    uint32_t           bytes_written;
};

audio_output_t *audio_output_dump_create(const audio_output_cfg_t *cfg)
{
    audio_output_t *out = pvPortMalloc(sizeof(audio_output_t));
    if (!out) return NULL;
    memset(out, 0, sizeof(*out));
    out->cfg = *cfg;
    out->fd = -1;
    return out;
}

int audio_output_dump_stop(audio_output_t *out);

void audio_output_dump_destroy(audio_output_t *out)
{
    if (!out) return;
    audio_output_dump_stop(out);
    vPortFree(out);
}

int audio_output_dump_start(audio_output_t *out)
{
    if (!out || out->started) return -1;

    if (!out->cfg.dump_hook) {
        /* File dump mode */
        if (!out->cfg.dump_path) {
            printf("dump_out: no path or hook configured\n");
            return -1;
        }
        out->fd = open(out->cfg.dump_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out->fd < 0) {
            printf("dump_out: cannot create %s\n", out->cfg.dump_path);
            return -1;
        }
        printf("dump_out: writing to %s (%lu Hz, %d ch, s16le)\n",
               out->cfg.dump_path,
               (unsigned long)out->cfg.sample_rate, out->cfg.channels);
    } else {
        printf("dump_out: hook mode (%lu Hz, %d ch)\n",
               (unsigned long)out->cfg.sample_rate, out->cfg.channels);
    }

    out->bytes_written = 0;
    out->started = 1;
    return 0;
}

int audio_output_dump_stop(audio_output_t *out)
{
    if (!out || !out->started) return 0;

    if (out->fd >= 0) {
        close(out->fd);
        printf("dump_out: closed, %lu bytes written\n",
               (unsigned long)out->bytes_written);
        out->fd = -1;
    }

    out->started = 0;
    return 0;
}

int audio_output_dump_write(audio_output_t *out,
                            const int16_t *pcm, uint32_t samples)
{
    if (!out || !out->started || samples == 0) return 0;

    uint32_t bytes = samples * out->cfg.channels * sizeof(int16_t);

    if (out->cfg.dump_hook) {
        /* Hook mode — app handles transport */
        out->cfg.dump_hook(pcm, samples, out->cfg.dump_ctx);
    } else if (out->fd >= 0) {
        /* File dump mode */
        write(out->fd, pcm, bytes);
    }

    out->bytes_written += bytes;
    return 0;
}
