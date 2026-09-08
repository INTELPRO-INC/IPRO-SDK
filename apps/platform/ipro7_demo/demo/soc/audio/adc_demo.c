/**
 * @file adc_demo.c
 * @brief Audio ADC capture + DAC replay demo via audio framework
 *
 * Shell commands:
 *   auadc_demo [gain_db]      - Record AMIC (GPIO4/7 diff) to RAM
 *   aupdm_demo [gain]         - Record PDM DMIC (GPIO34 clk / GPIO35 data)
 *   replay_record <rate>      - Replay recorded buffer via DAC (one-shot)
 *   aud_save                  - Save recorded buffer to /sdcard/audio/rec.pcm
 *
 * All three use components/audio framework:
 *   - audio_capture (AMIC/DMIC producer with multicast consumer)
 *   - audio_output  (DAC one-shot sink)
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

#define FRAME_SAMPLES       320
#define DEFAULT_SECONDS     6
#define MAX_SECONDS         30
#define CAPTURE_RATE        16000

static int16_t *s_rec_buf;          /* captured PCM, int16 mono */
static uint32_t s_rec_cap_bytes;    /* allocated size of s_rec_buf */
static uint32_t s_rec_bytes;        /* valid bytes after capture */
static TaskHandle_t s_drain_task;

static void capture_drain_task(void *arg)
{
    int consumer_id = (int)(intptr_t)arg;
    uint32_t off = 0;
    uint32_t target = s_rec_cap_bytes;

    while (off < target) {
        size_t got = audio_capture_read(consumer_id,
                                        (uint8_t *)s_rec_buf + off,
                                        target - off,
                                        1000);
        if (got == 0) continue;  /* timeout - keep waiting */
        off += got;
    }
    s_rec_bytes = off;

    audio_capture_stop();
    audio_capture_remove_consumer(consumer_id);
    audio_capture_deinit();

    printf("audio_rec: captured %lu bytes (%lu samples)\r\n",
           (unsigned long)s_rec_bytes,
           (unsigned long)(s_rec_bytes / sizeof(int16_t)));

    s_drain_task = NULL;
    vTaskDelete(NULL);
}

static int start_capture(const audio_capture_cfg_t *cfg, uint32_t seconds)
{
    if (s_drain_task) {
        printf("audio_rec: recording already in progress\r\n");
        return -1;
    }

    uint32_t need_bytes = (uint32_t)CAPTURE_RATE * seconds * sizeof(int16_t);
    if (need_bytes > s_rec_cap_bytes) {
        if (s_rec_buf) vPortFree(s_rec_buf);
        s_rec_buf = pvPortMalloc(need_bytes);
        if (!s_rec_buf) {
            printf("audio_rec: alloc %lu bytes failed\r\n",
                   (unsigned long)need_bytes);
            s_rec_cap_bytes = 0;
            return -1;
        }
        s_rec_cap_bytes = need_bytes;
    }
    s_rec_bytes = 0;

    if (audio_capture_init(cfg) != 0) return -1;

    int cid = audio_capture_add_consumer(FRAME_SAMPLES * sizeof(int16_t) * 4);
    if (cid < 0) {
        audio_capture_deinit();
        return -1;
    }
    if (audio_capture_start() != 0) {
        audio_capture_remove_consumer(cid);
        audio_capture_deinit();
        return -1;
    }

    if (xTaskCreate(capture_drain_task, "aud_drain", 1024,
                    (void *)(intptr_t)cid, 5, &s_drain_task) != pdPASS) {
        audio_capture_stop();
        audio_capture_remove_consumer(cid);
        audio_capture_deinit();
        return -1;
    }
    return 0;
}

static int cmd_auadc_test(int argc, char **argv)
{
    /* Default 36 dB: tested 30 dB sounded faint after the 16->10-bit
     * DAC truncation (-36 dB SNR). 36 dB is mid-high on the 6..42 dB
     * AMIC PGA range, audible without clipping for hand-held distance.
     * Override via the first arg if needed (max 42 dB). */
    int gain = 36;
    int seconds = DEFAULT_SECONDS;
    if (argc > 1) gain = atoi(argv[1]);
    if (argc > 2) seconds = atoi(argv[2]);
    if (seconds <= 0 || seconds > MAX_SECONDS) {
        printf("auadc_demo: seconds must be 1..%d\r\n", MAX_SECONDS);
        return 0;
    }

    /* AMIC differential pair on GPIO10 (+) / GPIO13 (-) per board wiring.
     * audioPinList[] = {4,5,6,7,10,11,12,13} so AIN index 4=GPIO10,
     * 7=GPIO13. Keeping the baseline parameter values (4, 7) which the
     * pre-refactor code used; only flipping single_ended to 0 so both
     * pins are read differentially instead of just GPIO10 to ground. */
    audio_capture_cfg_t cfg = {
        .type          = AUDIO_SRC_AMIC,
        .sample_rate   = CAPTURE_RATE,
        .frame_samples = FRAME_SAMPLES,
        .amic = {
            .single_ended = 0,
            .pos_ch       = 4,   /* GPIO10 */
            .neg_ch       = 7,   /* GPIO13 */
            .gain_db      = gain,
        },
    };
    printf("auadc_demo: gain=%d dB, seconds=%d (diff GPIO10/13)\r\n",
           gain, seconds);
    return start_capture(&cfg, (uint32_t)seconds);
}
SHELL_CMD_EXPORT_ALIAS(cmd_auadc_test, auadc_demo, AMIC Capture Demo [gain] [seconds]);

static int cmd_aupdm_test(int argc, char **argv)
{
    int gain = 0x30;
    int seconds = DEFAULT_SECONDS;
    if (argc > 1) gain = atoi(argv[1]);
    if (argc > 2) seconds = atoi(argv[2]);
    if (seconds <= 0 || seconds > MAX_SECONDS) {
        printf("aupdm_demo: seconds must be 1..%d\r\n", MAX_SECONDS);
        return 0;
    }

    audio_capture_cfg_t cfg = {
        .type          = AUDIO_SRC_DMIC,
        .sample_rate   = CAPTURE_RATE,
        .frame_samples = FRAME_SAMPLES,
        .dmic = {
            .clk_pin        = 34,
            .data_pin       = 35,
            .channel        = 1,   /* HAL_AUADC_PDM_CH_RIGHT */
            .digital_gain_db = (float)gain,
        },
    };
    printf("aupdm_demo: gain=%d, seconds=%d\r\n", gain, seconds);
    return start_capture(&cfg, (uint32_t)seconds);
}
SHELL_CMD_EXPORT_ALIAS(cmd_aupdm_test, aupdm_demo, PDM DMIC Capture Demo [gain] [seconds]);

static int cmd_replay_record(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: replay_record <rate>\r\n");
        return 0;
    }
    if (!s_rec_buf || s_rec_bytes == 0) {
        printf("replay_record: no recording - run auadc_demo/aupdm_demo first\r\n");
        return 0;
    }
    uint32_t rate = atoi(argv[1]);

    audio_output_cfg_t cfg = {
        .type        = AUDIO_OUTPUT_DAC,
        .mode        = AUDIO_OUTPUT_MODE_ONESHOT,
        .sample_rate = rate,
        .channels    = 1,
        .dac_spk_gpio = -1,
    };
    audio_output_t *out = audio_output_create(&cfg);
    if (!out) {
        printf("replay_record: output create failed\r\n");
        return 0;
    }
    if (audio_output_start(out) != 0) {
        printf("replay_record: start failed\r\n");
        audio_output_destroy(out);
        return 0;
    }

    uint32_t samples = s_rec_bytes / sizeof(int16_t);
    printf("replay_record: %lu samples @ %lu Hz\r\n",
           (unsigned long)samples, (unsigned long)rate);
    audio_output_write(out, s_rec_buf, samples);
    audio_output_stop(out);
    audio_output_destroy(out);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_replay_record, replay_record, Replay Recorded Audio);

#define AUADC_SAVE_FILE_NAME    "/sdcard/audio/rec.pcm"
static int cmd_auadc_save_test(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (!s_rec_buf || s_rec_bytes == 0) {
        printf("aud_save: no recording\r\n");
        return 0;
    }
    int fd = open(AUADC_SAVE_FILE_NAME, O_RDWR | O_CREAT | O_TRUNC);
    if (fd < 0) {
        printf("aud_save: open %s failed\r\n", AUADC_SAVE_FILE_NAME);
        return 0;
    }
    if (write(fd, s_rec_buf, s_rec_bytes) != (int)s_rec_bytes) {
        printf("aud_save: write failed\r\n");
    } else {
        printf("aud_save: wrote %lu bytes to %s\r\n",
               (unsigned long)s_rec_bytes, AUADC_SAVE_FILE_NAME);
    }
    close(fd);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_auadc_save_test, aud_save, Save Recording to SD);
