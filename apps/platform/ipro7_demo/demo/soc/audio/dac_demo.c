/**
 * @file dac_demo.c
 * @brief DAC playback demo via audio_output framework
 *
 * Shell commands:
 *   dac_demo <file|mem> <rate> [oneshot|stream]
 *       - Play raw 16-bit mono PCM @ <rate> Hz to on-chip DAC.
 *       - Source: file path, or "mem" (compiled-in sample, needs
 *         CONFIG_DEMO_DAC_AUDIO_DATA).
 *       - Mode: "oneshot" (default) for small buffers; "stream" for
 *         large files (uses N-ring DMA, low RAM).
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <generated/autoconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "evfs.h"
#include "evfs_posix.h"
#include "shell.h"

#include <FreeRTOS.h>

#include "audio_output.h"

#ifdef CONFIG_DEMO_DAC_AUDIO_DATA
#include "dac_demo_audio.h"
#endif

/* Supported DAC sample rates (from hal_dac.h) */
static int is_supported_rate(uint32_t hz)
{
    return hz == 8000 || hz == 16000 || hz == 32000 || hz == 512000;
}

/* Play a whole buffer in one-shot mode. src is int16_t mono PCM. */
static void play_oneshot(const int16_t *pcm, uint32_t samples, uint32_t rate)
{
    audio_output_cfg_t cfg = {
        .type        = AUDIO_OUTPUT_DAC,
        .mode        = AUDIO_OUTPUT_MODE_ONESHOT,
        .sample_rate = rate,
        .channels    = 1,
        .dac_spk_gpio = -1,
    };
    audio_output_t *out = audio_output_create(&cfg);
    if (!out) {
        printf("dac_demo: create failed\r\n");
        return;
    }
    if (audio_output_start(out) != 0) {
        printf("dac_demo: start failed\r\n");
        audio_output_destroy(out);
        return;
    }

    printf("dac_demo: oneshot play %lu samples @ %lu Hz\r\n",
           (unsigned long)samples, (unsigned long)rate);
    if (audio_output_write(out, pcm, samples) != 0) {
        printf("dac_demo: write failed\r\n");
    } else {
        printf("dac_demo: oneshot done\r\n");
    }

    audio_output_stop(out);
    audio_output_destroy(out);
}

/* Play in streaming mode by chunking through a ring. For large files. */
#define STREAM_CHUNK_SAMPLES  512
static void play_stream_file(const char *path, uint32_t rate)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("dac_demo: open %s failed\r\n", path);
        return;
    }

    audio_output_cfg_t cfg = {
        .type        = AUDIO_OUTPUT_DAC,
        .mode        = AUDIO_OUTPUT_MODE_STREAMING,
        .sample_rate = rate,
        .channels    = 1,
        .dac_spk_gpio = -1,
        /* .ring_buf_count/ms = 0 -> backend defaults (3 x 64ms) */
    };
    audio_output_t *out = audio_output_create(&cfg);
    if (!out) {
        printf("dac_demo: create failed\r\n");
        close(fd);
        return;
    }
    if (audio_output_start(out) != 0) {
        printf("dac_demo: start failed\r\n");
        audio_output_destroy(out);
        close(fd);
        return;
    }

    int16_t *chunk = pvPortMalloc(STREAM_CHUNK_SAMPLES * sizeof(int16_t));
    if (!chunk) {
        printf("dac_demo: chunk alloc failed\r\n");
        audio_output_stop(out);
        audio_output_destroy(out);
        close(fd);
        return;
    }

    uint32_t total = 0;
    for (;;) {
        int n = read(fd, chunk, STREAM_CHUNK_SAMPLES * sizeof(int16_t));
        if (n <= 0) break;
        uint32_t samples = (uint32_t)n / sizeof(int16_t);
        if (audio_output_write(out, chunk, samples) != 0) {
            printf("dac_demo: write failed at %lu samples\r\n",
                   (unsigned long)total);
            break;
        }
        total += samples;
    }
    printf("dac_demo: stream done (%lu samples)\r\n", (unsigned long)total);

    vPortFree(chunk);
    audio_output_stop(out);
    audio_output_destroy(out);
    close(fd);
}

static int cmd_dac_test(int argc, char **argv)
{
    const char *path;
    uint32_t rate;
    int streaming = 0;

    if (argc < 3) {
        printf("Usage: dac_demo <file|mem> <rate> [oneshot|stream]\r\n");
        printf("  rate: 8000 | 16000 | 32000 | 512000\r\n");
        return 0;
    }
    path = argv[1];
    rate = atoi(argv[2]);
    if (argc >= 4) {
        if (strcmp(argv[3], "stream") == 0) streaming = 1;
        else if (strcmp(argv[3], "oneshot") == 0) streaming = 0;
    }
    if (!is_supported_rate(rate)) {
        printf("Unsupported rate %lu (use 8000/16000/32000/512000)\r\n",
               (unsigned long)rate);
        return 0;
    }

#ifdef CONFIG_DEMO_DAC_AUDIO_DATA
    if (strcmp(path, "mem") == 0) {
        /* Built-in sample is int16_t mono PCM */
        uint32_t samples = audio_32k_len / sizeof(int16_t);
        play_oneshot((const int16_t *)audio_32k, samples, rate);
        return 0;
    }
#endif

    if (streaming) {
        play_stream_file(path, rate);
    } else {
        struct stat st;
        if (stat(path, &st) != 0) {
            printf("dac_demo: stat %s failed\r\n", path);
            return 0;
        }
        uint32_t samples = (uint32_t)st.st_size / sizeof(int16_t);
        int16_t *buf = pvPortMalloc(samples * sizeof(int16_t));
        if (!buf) {
            printf("dac_demo: alloc %lu samples failed "
                   "(try 'stream' mode)\r\n", (unsigned long)samples);
            return 0;
        }
        int fd = open(path, O_RDONLY);
        if (fd < 0 || read(fd, buf, samples * sizeof(int16_t))
                      != (int)(samples * sizeof(int16_t))) {
            printf("dac_demo: read %s failed\r\n", path);
            if (fd >= 0) close(fd);
            vPortFree(buf);
            return 0;
        }
        close(fd);
        play_oneshot(buf, samples, rate);
        vPortFree(buf);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dac_test, dac_demo, DAC Demo);
