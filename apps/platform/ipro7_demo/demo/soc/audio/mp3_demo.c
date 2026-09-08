/**
 * @file mp3_demo.c
 * @brief MP3 playback and decode profiling demo
 *
 * Shell commands:
 *   mp3_play <path> <dac|uac>  - Play MP3 file to output sink
 *   mp3_stop                   - Stop current playback
 *   mp3_profile <path>         - Decode-only performance benchmark
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#include <generated/autoconf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "shell.h"
#include "mp3_decoder.h"
#include "audio_output.h"
#include "system_soc.h"
#include <drv_clock.h>

/* ---------------------------------------------------------------------------
 * Playback state
 * ------------------------------------------------------------------------- */

typedef struct {
    mp3_decoder_t      *decoder;
    audio_output_t     *output;
    TaskHandle_t        task;
    volatile int        stop_req;
    char               *path;
    audio_output_type_t out_type;
    char               *dump_path;  /* for AUDIO_OUTPUT_DUMP */
} mp3_play_ctx_t;

static mp3_play_ctx_t s_play_ctx;

/* ---------------------------------------------------------------------------
 * Playback task — all heavy work (open/decode/output) runs here, not in shell
 * ------------------------------------------------------------------------- */

static void mp3_play_task(void *arg)
{
    mp3_play_ctx_t *ctx = (mp3_play_ctx_t *)arg;

    /* Open decoder (uses minimp3 which needs large stack) */
    mp3_decoder_t *dec = mp3_decoder_create();
    if (!dec) {
        printf("mp3_play: decoder alloc failed\n");
        goto done;
    }

    if (mp3_decoder_open(dec, ctx->path) != 0) {
        mp3_decoder_destroy(dec);
        goto done;
    }
    ctx->decoder = dec;

    mp3_info_t info;
    mp3_decoder_get_info(dec, &info);

    printf("mp3_play: %lu Hz, %d ch, %lu kbps\n",
           (unsigned long)info.sample_rate, info.channels,
           (unsigned long)info.bitrate);

    /* Create output sink */
    audio_output_cfg_t ocfg = {
        .type = ctx->out_type,
        .sample_rate = info.sample_rate,
        .channels = info.channels,
        .dump_path = ctx->dump_path,
    };
    audio_output_t *out = audio_output_create(&ocfg);
    if (!out) {
        printf("mp3_play: output create failed\n");
        goto done;
    }
    ctx->output = out;
    audio_output_start(out);

    int16_t *pcm = pvPortMalloc(MP3_MAX_SAMPLES_PER_FRAME * 2 * sizeof(int16_t));
    if (!pcm) {
        printf("mp3_play: PCM buffer alloc failed\n");
        goto done;
    }

    int samples;
    uint32_t total_frames = 0;
    uint32_t loop_count = 0;

    for (;;) {
        while (!ctx->stop_req) {
            int ret = mp3_decoder_decode_frame(dec, pcm, &samples);
            if (ret == 1) {
                loop_count++;
                printf("mp3_play: loop %lu done (%lu frames)\n",
                       (unsigned long)loop_count, (unsigned long)total_frames);
                break;
            }
            if (ret < 0) {
                printf("mp3_play: decode error at frame %lu\n",
                       (unsigned long)total_frames);
                ctx->stop_req = 1;
                break;
            }

            if (samples > 0) {
                audio_output_write(out, pcm, samples);
                total_frames++;
            }
        }

        if (ctx->stop_req) break;

        /* Reopen file for loop playback */
        mp3_decoder_close(dec);
        if (mp3_decoder_open(dec, ctx->path) != 0) {
            printf("mp3_play: reopen failed\n");
            break;
        }
    }

    if (out) {
        audio_output_stop(out);
    }
    vPortFree(pcm);

done:
    if (ctx->output) {
        audio_output_destroy(ctx->output);
        ctx->output = NULL;
    }
    if (ctx->decoder) {
        mp3_decoder_close(ctx->decoder);
        mp3_decoder_destroy(ctx->decoder);
        ctx->decoder = NULL;
    }
    vPortFree(ctx->path);
    vPortFree(ctx->dump_path);
    ctx->path = NULL;
    ctx->dump_path = NULL;
    ctx->task = NULL;
    ctx->stop_req = 0;

    vTaskDelete(NULL);
}

/* ---------------------------------------------------------------------------
 * mp3_play command — lightweight shell handler, just launches task
 * ------------------------------------------------------------------------- */

static int cmd_mp3_play(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: mp3_play <file> <dac|uac|i2s|dump [path]>\n");
        return 0;
    }

    if (s_play_ctx.task) {
        printf("mp3_play: already playing, use mp3_stop first\n");
        return 0;
    }

    const char *sink = argv[2];

    audio_output_type_t out_type;
    if (strcmp(sink, "dac") == 0) {
        out_type = AUDIO_OUTPUT_DAC;
    } else if (strcmp(sink, "uac") == 0) {
        out_type = AUDIO_OUTPUT_UAC;
    } else if (strcmp(sink, "i2s") == 0) {
        out_type = AUDIO_OUTPUT_I2S;
    } else if (strcmp(sink, "dump") == 0) {
        out_type = AUDIO_OUTPUT_DUMP;
    } else {
        printf("mp3_play: unknown output '%s' (use dac, uac, i2s, or dump)\n", sink);
        return 0;
    }

    /* Copy path — shell argv is transient */
    size_t len = strlen(argv[1]) + 1;
    char *path = pvPortMalloc(len);
    if (!path) {
        printf("mp3_play: alloc failed\n");
        return 0;
    }
    memcpy(path, argv[1], len);

    memset(&s_play_ctx, 0, sizeof(s_play_ctx));
    s_play_ctx.path = path;
    s_play_ctx.out_type = out_type;

    /* Dump output path: optional 4th arg, default /sdcard/dump.pcm */
    if (out_type == AUDIO_OUTPUT_DUMP) {
        const char *dp = (argc >= 4) ? argv[3] : "/sdcard/dump.pcm";
        size_t dplen = strlen(dp) + 1;
        s_play_ctx.dump_path = pvPortMalloc(dplen);
        if (s_play_ctx.dump_path) {
            memcpy(s_play_ctx.dump_path, dp, dplen);
        }
    }

    BaseType_t ret = xTaskCreate(mp3_play_task, "mp3play", 8192, &s_play_ctx,
                                 configMAX_PRIORITIES - 3, &s_play_ctx.task);
    if (ret != pdPASS) {
        printf("mp3_play: task create failed\n");
        vPortFree(path);
        memset(&s_play_ctx, 0, sizeof(s_play_ctx));
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mp3_play, mp3_play, Play MP3: mp3_play <file> <dac|uac|i2s|dump> [dump_path]);

/* ---------------------------------------------------------------------------
 * mp3_stop command
 * ------------------------------------------------------------------------- */

static int cmd_mp3_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!s_play_ctx.task) {
        printf("mp3_stop: not playing\n");
        return 0;
    }

    printf("mp3_stop: stopping...\n");
    s_play_ctx.stop_req = 1;

    /* Wait for task to finish (up to 2s) */
    for (int i = 0; i < 200 && s_play_ctx.task; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    if (s_play_ctx.task) {
        printf("mp3_stop: force delete\n");
        vTaskDelete(s_play_ctx.task);
        if (s_play_ctx.output) {
            audio_output_stop(s_play_ctx.output);
            audio_output_destroy(s_play_ctx.output);
        }
        if (s_play_ctx.decoder) {
            mp3_decoder_close(s_play_ctx.decoder);
            mp3_decoder_destroy(s_play_ctx.decoder);
        }
        vPortFree(s_play_ctx.path);
        vPortFree(s_play_ctx.dump_path);
        memset(&s_play_ctx, 0, sizeof(s_play_ctx));
    }

    printf("mp3_stop: done\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mp3_stop, mp3_stop, Stop MP3 playback);

/* ---------------------------------------------------------------------------
 * mp3_profile command — runs in dedicated task (minimp3 needs ~8KB stack)
 * ------------------------------------------------------------------------- */

static void mp3_profile_task(void *arg)
{
    const char *path = (const char *)arg;

    mp3_decoder_t *dec = mp3_decoder_create();
    if (!dec) {
        printf("mp3_profile: decoder alloc failed\n");
        goto done;
    }

    if (mp3_decoder_open(dec, path) != 0) {
        mp3_decoder_destroy(dec);
        goto done;
    }

    mp3_info_t info;
    mp3_decoder_get_info(dec, &info);

    printf("\n=== MP3 Decode Profile ===\n");
    printf("File: %s\n", path);
    printf("Info: %lu Hz, %s, %lu kbps\n",
           (unsigned long)info.sample_rate,
           info.channels == 2 ? "stereo" : "mono",
           (unsigned long)info.bitrate);
    printf("Size: %lu bytes\n", (unsigned long)info.data_bytes);
    printf("\nDecoding (no output)...\n");

    int16_t *pcm = pvPortMalloc(MP3_MAX_SAMPLES_PER_FRAME * 2 * sizeof(int16_t));
    if (!pcm) {
        printf("mp3_profile: PCM buffer alloc failed\n");
        mp3_decoder_close(dec);
        mp3_decoder_destroy(dec);
        goto done;
    }

    uint32_t wall_start = xTaskGetTickCount();
    int samples;
    uint32_t total_samples = 0;

    while (1) {
        int ret = mp3_decoder_decode_frame(dec, pcm, &samples);
        if (ret != 0) {
            break;
        }
        total_samples += samples;
    }

    uint32_t wall_end = xTaskGetTickCount();
    uint32_t wall_ms = (wall_end - wall_start) * portTICK_PERIOD_MS;

    mp3_profile_stats_t stats;
    mp3_decoder_get_stats(dec, &stats);

    float duration_sec = 0.0f;
    if (info.sample_rate > 0) {
        duration_sec = (float)total_samples / info.sample_rate;
    }
    float wall_sec = (float)wall_ms / 1000.0f;
    float rt_ratio = (wall_sec > 0.001f) ? (duration_sec / wall_sec) : 0.0f;

    float throughput_kbs = 0.0f;
    if (wall_sec > 0.001f) {
        throughput_kbs = (float)info.data_bytes / 1024.0f / wall_sec;
    }

    float avg_cycles = 0.0f;
    float mips = 0.0f;
    if (stats.frame_count > 0) {
        avg_cycles = (float)stats.decode_cycles_total / stats.frame_count;
        float fps = (info.sample_rate > 0 && total_samples > 0)
            ? ((float)info.sample_rate * stats.frame_count / total_samples)
            : 0.0f;
        mips = avg_cycles * fps / 1000000.0f;
    }

    printf("\nFrames decoded: %lu\n", (unsigned long)stats.frame_count);
    printf("Total samples:  %lu (%.2f sec audio)\n",
           (unsigned long)total_samples, duration_sec);

    printf("\nPer-frame decode time:\n");
    printf("  Min: %lu us, Max: %lu us, Avg: %lu us\n",
           (unsigned long)stats.decode_us_min,
           (unsigned long)stats.decode_us_max,
           stats.frame_count > 0
               ? (unsigned long)(stats.decode_us_total / stats.frame_count)
               : 0UL);

    printf("\nThroughput:     %.1f KB/s\n", throughput_kbs);
    printf("Wall time:      %lu ms\n", (unsigned long)wall_ms);
    printf("Realtime ratio: %.1fx\n", rt_ratio);

    printf("\nCycle counter (mcycle):\n");
    printf("  Avg cycles/frame: %.0f\n", avg_cycles);
    printf("  Estimated MIPS:   %.1f\n", mips);
    printf("  CPU clock:        %lu MHz\n",
           (unsigned long)(SystemCoreClockGet() / 1000000));

    printf("\n=== Profile Complete ===\n");

    vPortFree(pcm);
    mp3_decoder_close(dec);
    mp3_decoder_destroy(dec);

done:
    vPortFree((void *)path);
    vTaskDelete(NULL);
}

static int cmd_mp3_profile(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: mp3_profile <file>\n");
        return 0;
    }

    /* Copy path string — shell argv is transient */
    size_t len = strlen(argv[1]) + 1;
    char *path = pvPortMalloc(len);
    if (!path) {
        printf("mp3_profile: alloc failed\n");
        return 0;
    }
    memcpy(path, argv[1], len);

    BaseType_t ret = xTaskCreate(mp3_profile_task, "mp3prof", 8192, path,
                                 configMAX_PRIORITIES - 4, NULL);
    if (ret != pdPASS) {
        printf("mp3_profile: task create failed\n");
        vPortFree(path);
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mp3_profile, mp3_profile, MP3 decode benchmark: mp3_profile <file>);

/* ---------------------------------------------------------------------------
 * mp3_dump command — decode to raw PCM and save to SD card
 * ------------------------------------------------------------------------- */

static void mp3_dump_task(void *arg)
{
    const char *src_path = (const char *)arg;
    const char *dst_path = "/sdcard/dump.pcm";

    mp3_decoder_t *dec = mp3_decoder_create();
    if (!dec) {
        printf("mp3_dump: decoder alloc failed\n");
        goto done;
    }

    if (mp3_decoder_open(dec, src_path) != 0) {
        mp3_decoder_destroy(dec);
        goto done;
    }

    mp3_info_t info;
    mp3_decoder_get_info(dec, &info);
    printf("mp3_dump: %lu Hz, %d ch, %lu kbps\n",
           (unsigned long)info.sample_rate, info.channels,
           (unsigned long)info.bitrate);
    printf("mp3_dump: output -> %s (raw int16 PCM, %d ch)\n",
           dst_path, info.channels);

    int fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        printf("mp3_dump: cannot create %s\n", dst_path);
        mp3_decoder_close(dec);
        mp3_decoder_destroy(dec);
        goto done;
    }

    int16_t *pcm = pvPortMalloc(MP3_MAX_SAMPLES_PER_FRAME * 2 * sizeof(int16_t));
    if (!pcm) {
        printf("mp3_dump: PCM alloc failed\n");
        close(fd);
        mp3_decoder_close(dec);
        mp3_decoder_destroy(dec);
        goto done;
    }

    int samples;
    uint32_t total_frames = 0;
    uint32_t total_bytes = 0;

    while (1) {
        int ret = mp3_decoder_decode_frame(dec, pcm, &samples);
        if (ret != 0) break;

        if (samples > 0) {
            uint32_t bytes = samples * info.channels * sizeof(int16_t);
            write(fd, pcm, bytes);
            total_bytes += bytes;
            total_frames++;
            if (total_frames % 1000 == 0) {
                printf("mp3_dump: %lu frames, %lu KB\n",
                       (unsigned long)total_frames,
                       (unsigned long)(total_bytes / 1024));
            }
        }
    }

    close(fd);
    vPortFree(pcm);
    mp3_decoder_close(dec);
    mp3_decoder_destroy(dec);

    printf("mp3_dump: done — %lu frames, %lu bytes -> %s\n",
           (unsigned long)total_frames, (unsigned long)total_bytes, dst_path);
    printf("mp3_dump: play with: ffplay -f s16le -ar %lu -ac %d %s\n",
           (unsigned long)info.sample_rate, info.channels, dst_path);

done:
    vPortFree((void *)src_path);
    vTaskDelete(NULL);
}

static int cmd_mp3_dump(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: mp3_dump <file>\n");
        return 0;
    }

    size_t len = strlen(argv[1]) + 1;
    char *path = pvPortMalloc(len);
    if (!path) return 0;
    memcpy(path, argv[1], len);

    BaseType_t ret = xTaskCreate(mp3_dump_task, "mp3dump", 8192, path,
                                 configMAX_PRIORITIES - 4, NULL);
    if (ret != pdPASS) {
        printf("mp3_dump: task create failed\n");
        vPortFree(path);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mp3_dump, mp3_dump, Decode MP3 to raw PCM on SD: mp3_dump <file>);
