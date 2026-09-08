/**
 * @file duplex_demo.c
 * @brief Simultaneous DAC playback + AMIC capture (loopback self-test)
 *
 * Shell command:
 *   audio_duplex <seconds>             - Play 1 kHz sine + record AMIC
 *   audio_duplex <file> <seconds>      - Play file + record AMIC
 *
 * Outputs to SD card:
 *   /sdcard/audio/loopback_src.pcm     - Source PCM (what we asked DAC to play)
 *   /sdcard/audio/loopback_rec.pcm     - AMIC capture (what the mic heard)
 *
 * Runs only when DAC is on DMA CH1 (see audio_output_dac.c DAC_DMA_CH)
 * and AUADC is on DMA CH0 (hal_auadc.c default). Depends on duplex-safe
 * channel allocation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
#include <task.h>

#include "audio_capture.h"
#include "audio_output.h"

#define DUPLEX_RATE         16000
#define DUPLEX_FRAME_SAMPLES 320
#define CHUNK_SAMPLES       512          /* DAC streaming write size */

#define SRC_PATH            "/sdcard/audio/loopback_src.pcm"
#define REC_PATH            "/sdcard/audio/loopback_rec.pcm"

/* 16-sample LUT of 1 kHz sine at 16 kHz, 16-bit signed, ~60% amplitude */
static const int16_t s_sine_lut[16] = {
        0,   7523,  13900,  18179,  19661,  18179,  13900,   7523,
        0,  -7523, -13900, -18179, -19661, -18179, -13900,  -7523,
};

/* Context shared with playback task */
typedef struct {
    audio_output_t *out;
    const int16_t  *src;
    uint32_t        total_samples;
    volatile int    done;
    volatile int    err;
} play_ctx_t;

static void playback_task(void *arg)
{
    play_ctx_t *pc = (play_ctx_t *)arg;
    uint32_t pos = 0;

    while (pos < pc->total_samples) {
        uint32_t remain = pc->total_samples - pos;
        uint32_t n = (remain < CHUNK_SAMPLES) ? remain : CHUNK_SAMPLES;
        if (audio_output_write(pc->out, pc->src + pos, n) != 0) {
            pc->err = 1;
            break;
        }
        pos += n;
    }
    pc->done = 1;
    vTaskDelete(NULL);
}

static int write_pcm_file(const char *path, const void *buf, uint32_t bytes)
{
    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC);
    if (fd < 0) {
        printf("duplex: open %s failed\r\n", path);
        return -1;
    }
    int n = write(fd, buf, bytes);
    close(fd);
    if (n != (int)bytes) {
        printf("duplex: short write %d/%lu bytes to %s\r\n",
               n, (unsigned long)bytes, path);
        return -1;
    }
    return 0;
}

/* Generate N seconds of 1 kHz sine @ 16 kHz via LUT */
static void fill_sine(int16_t *buf, uint32_t samples)
{
    for (uint32_t i = 0; i < samples; i++) {
        buf[i] = s_sine_lut[i & 15];
    }
}

static int cmd_audio_duplex(int argc, char **argv)
{
    /* Enabled 2026-04-20 on top of the DMA channel allocator.
     * DAC and AUADC each claim their own channel via
     * dma_request_channel(); channel bring-up no longer does a
     * controller-wide DMA_Disable/Enable so either side can start
     * while the other is live. If CONFIG_DMA_CHANNEL_ALLOCATOR is
     * off, this hangs the same way it used to — build ipro7_demo
     * with CONFIG_DMA_CHANNEL_ALLOCATOR=y for duplex. */
    const char *src_path = NULL;   /* NULL = use built-in sine */
    int seconds = 3;

    if (argc < 2) {
        printf("Usage: audio_duplex <seconds>\r\n"
               "       audio_duplex <file> <seconds>\r\n");
        return 0;
    }
    if (argc == 2) {
        seconds = atoi(argv[1]);
    } else {
        src_path = argv[1];
        seconds = atoi(argv[2]);
    }
    if (seconds <= 0 || seconds > 30) {
        printf("duplex: seconds must be 1..30\r\n");
        return 0;
    }

    uint32_t samples = (uint32_t)DUPLEX_RATE * seconds;
    uint32_t bytes   = samples * sizeof(int16_t);

    /* Allocate source (playback) and record (capture) buffers */
    int16_t *src = pvPortMalloc(bytes);
    int16_t *rec = pvPortMalloc(bytes);
    if (!src || !rec) {
        printf("duplex: buffer alloc failed (need %lu bytes x2)\r\n",
               (unsigned long)bytes);
        if (src) vPortFree(src);
        if (rec) vPortFree(rec);
        return 0;
    }

    /* Fill source from file or built-in sine */
    if (src_path) {
        int fd = open(src_path, O_RDONLY);
        if (fd < 0) {
            printf("duplex: open %s failed\r\n", src_path);
            goto fail_bufs;
        }
        int n = read(fd, src, bytes);
        close(fd);
        if (n < (int)bytes) {
            /* Short file: pad tail with silence */
            memset((uint8_t *)src + n, 0, bytes - n);
            printf("duplex: %s is %d bytes, padded to %lu\r\n",
                   src_path, n, (unsigned long)bytes);
        }
    } else {
        fill_sine(src, samples);
        printf("duplex: playing built-in 1 kHz sine (%d sec)\r\n", seconds);
    }

    /* Init capture (AMIC differential on GPIO10/13, CH0) */
    audio_capture_cfg_t ccfg = {
        .type          = AUDIO_SRC_AMIC,
        .sample_rate   = DUPLEX_RATE,
        .frame_samples = DUPLEX_FRAME_SAMPLES,
        .amic = {
            .single_ended = 0,
            .pos_ch       = 4,   /* GPIO10 */
            .neg_ch       = 7,   /* GPIO13 */
            .gain_db      = 24,
        },
    };
    if (audio_capture_init(&ccfg) != 0) {
        printf("duplex: capture init failed\r\n");
        goto fail_bufs;
    }
    int cid = audio_capture_add_consumer(DUPLEX_FRAME_SAMPLES *
                                         sizeof(int16_t) * 4);
    if (cid < 0) {
        printf("duplex: consumer add failed\r\n");
        goto fail_cap_init;
    }

    /* Init DAC streaming output (on CH1) */
    audio_output_cfg_t ocfg = {
        .type        = AUDIO_OUTPUT_DAC,
        .mode        = AUDIO_OUTPUT_MODE_STREAMING,
        .sample_rate = DUPLEX_RATE,
        .channels    = 1,
        .dac_spk_gpio = -1,
    };
    audio_output_t *out = audio_output_create(&ocfg);
    if (!out) {
        printf("duplex: output create failed\r\n");
        goto fail_cap_consumer;
    }

    /* DAC start first: hal_dma's dma_init() internally calls DMA_Disable()
     * on the controller (global, not per-channel). If we start AUADC first,
     * that DMA_Disable would abort the in-flight capture transfer. Since
     * the DAC is warming up its N-ring with silence initially, the audio
     * lead-in we lose on the record side is negligible (<64 ms). */
    if (audio_output_start(out) != 0) {
        printf("duplex: output start failed\r\n");
        goto fail_out;
    }

    if (audio_capture_start() != 0) {
        printf("duplex: capture start failed\r\n");
        audio_output_stop(out);
        goto fail_out;
    }

    /* Kick off playback task */
    play_ctx_t pc = {
        .out = out, .src = src, .total_samples = samples,
        .done = 0, .err = 0,
    };
    TaskHandle_t play_task = NULL;
    if (xTaskCreate(playback_task, "duplex_play", 1024, &pc, 5,
                    &play_task) != pdPASS) {
        printf("duplex: play task create failed\r\n");
        audio_output_stop(out);
        audio_capture_stop();
        goto fail_out;
    }

    /* Drain capture on this task until we have enough bytes */
    uint32_t off = 0;
    uint32_t timeout_ms = (uint32_t)seconds * 1000 + 2000;
    TickType_t start = xTaskGetTickCount();
    while (off < bytes) {
        size_t got = audio_capture_read(cid,
                                        (uint8_t *)rec + off,
                                        bytes - off,
                                        500);
        if (got) {
            off += got;
        }
        if ((xTaskGetTickCount() - start) > pdMS_TO_TICKS(timeout_ms)) {
            printf("duplex: capture timeout at %lu/%lu bytes\r\n",
                   (unsigned long)off, (unsigned long)bytes);
            break;
        }
    }

    /* Wait for playback to drain */
    while (!pc.done) vTaskDelay(pdMS_TO_TICKS(50));

    /* Tear down */
    audio_output_stop(out);
    audio_capture_stop();
    audio_output_destroy(out);
    audio_capture_remove_consumer(cid);
    audio_capture_deinit();

    printf("duplex: play %s, captured %lu/%lu bytes\r\n",
           pc.err ? "FAILED" : "OK",
           (unsigned long)off, (unsigned long)bytes);

    /* Persist both buffers */
    if (write_pcm_file(SRC_PATH, src, bytes) == 0) {
        printf("duplex: wrote %s (%lu bytes)\r\n",
               SRC_PATH, (unsigned long)bytes);
    }
    if (write_pcm_file(REC_PATH, rec, off) == 0) {
        printf("duplex: wrote %s (%lu bytes)\r\n",
               REC_PATH, (unsigned long)off);
    }

    vPortFree(src);
    vPortFree(rec);
    return 0;

fail_out:
    audio_output_destroy(out);
fail_cap_consumer:
    audio_capture_remove_consumer(cid);
fail_cap_init:
    audio_capture_deinit();
fail_bufs:
    if (src) vPortFree(src);
    if (rec) vPortFree(rec);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_audio_duplex, audio_duplex,
                       Simultaneous DAC playback and AMIC capture);
