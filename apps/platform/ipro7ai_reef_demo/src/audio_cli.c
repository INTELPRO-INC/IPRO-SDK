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
 * @file audio_cli.c
 * @brief `spk` and `mic` shell commands.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_SHELL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "board_config.h"
#include "audio.h"

static audio_route_t route_arg(int argc, char **argv, int idx)
{
    if (argc > idx && strcmp(argv[idx], "hp") == 0) {
        return AUDIO_ROUTE_HP;
    }
    return AUDIO_ROUTE_SPK;
}

static int src_arg(const char *s, audio_in_t *out)
{
    if (strcmp(s, "pdm") == 0)          *out = AUDIO_IN_PDM;
    else if (strcmp(s, "headset") == 0) *out = AUDIO_IN_HSMIC;
    else if (strcmp(s, "hs") == 0)      *out = AUDIO_IN_HSMIC;
    else if (strcmp(s, "aux") == 0)     *out = AUDIO_IN_AUX;
    else return -1;
    return 0;
}

/* ---------------------------------------------------------------------------
 * spk
 * -------------------------------------------------------------------------*/

static void spk_usage(void)
{
    printf("spk tone <hz> <ms> [amp%%] [hp]   sine on the DAC (default 20%%; "
           "'hp' keeps the amp muted)\n"
           "spk on | spk off                 PAM8302A SD# (GPIO%d), active high\n"
           "spk stop                         end the running job\n"
           "spk status\n"
           "Headphone and speaker share DAC A (GPIO%d); the amp gain is 30 V/V,\n"
           "so keep amp%% modest on the speaker.\n",
           BOARD_SPK_EN_PIN, BOARD_SPK_DAC_PIN);
}

static int cmd_spk(int argc, char **argv)
{
    if (argc < 2) {
        spk_usage();
        return 0;
    }
    if (strcmp(argv[1], "tone") == 0) {
        uint32_t hz  = (argc > 2) ? (uint32_t)atoi(argv[2]) : 1000u;
        uint32_t ms  = (argc > 3) ? (uint32_t)atoi(argv[3]) : 1000u;
        uint32_t amp = (argc > 4 && strcmp(argv[4], "hp") != 0) ? (uint32_t)atoi(argv[4]) : 20u;
        audio_route_t route = route_arg(argc, argv, 4);
        if (route == AUDIO_ROUTE_SPK) {
            route = route_arg(argc, argv, 5);
        }
        if (audio_tone(hz, ms, amp, route) == 0) {
            printf("%u Hz for %u ms at %u%% -> %s\n", (unsigned)hz, (unsigned)ms,
                   (unsigned)amp, route == AUDIO_ROUTE_HP ? "headphone" : "speaker+headphone");
        } else {
            printf("rejected (20..12000 Hz, ms > 0, not while another job runs)\n");
        }
    } else if (strcmp(argv[1], "on") == 0) {
        audio_spk_enable(true);
        printf("amp on (GPIO%d=%d)\n", BOARD_SPK_EN_PIN, BOARD_SPK_EN_ACTIVE);
    } else if (strcmp(argv[1], "off") == 0) {
        audio_spk_enable(false);
        printf("amp muted (GPIO%d=%d)\n", BOARD_SPK_EN_PIN, !BOARD_SPK_EN_ACTIVE);
    } else if (strcmp(argv[1], "stop") == 0) {
        audio_stop();
        printf("stopped\n");
    } else if (strcmp(argv[1], "status") == 0) {
        printf("amp %s, job %s\n", audio_spk_enabled() ? "ON" : "muted", audio_job_name());
    } else {
        spk_usage();
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk, spk, speaker / headphone DAC output);

/* ---------------------------------------------------------------------------
 * mic
 * -------------------------------------------------------------------------*/

static void mic_usage(void)
{
    printf("mic rec <pdm|headset|aux> <sec>       record up to %u s at 16 kHz\n"
           "mic play [hp]                         play the recording\n"
           "mic loop <pdm|headset|aux> [sec] [hp] live monitor (0/omit = until stop)\n"
           "mic gain <db>                         playback make-up gain 0..40 (now %d)\n"
           "mic ch <0|1>                          PDM slot, 0 = left (board default)\n"
           "mic stop | mic status\n"
           "pdm: GPIO%d/%d  headset: AIN%d/AIN%d (GPIO%d/%d)  aux: AIN%d (GPIO%d)\n",
           AUDIO_REC_MAX_SEC, audio_get_gain_db(),
           BOARD_MIC_CLK_PIN, BOARD_MIC_DAT_PIN,
           BOARD_HSMIC_POS_CH, BOARD_HSMIC_NEG_CH, BOARD_HSMIC_POS_PIN, BOARD_HSMIC_NEG_PIN,
           BOARD_AUX_L_CH, BOARD_AUX_L_PIN);
}

static int cmd_mic(int argc, char **argv)
{
    audio_in_t src;

    if (argc < 2) {
        mic_usage();
        return 0;
    }
    if (strcmp(argv[1], "rec") == 0 && argc >= 4 && src_arg(argv[2], &src) == 0) {
        uint32_t sec = (uint32_t)atoi(argv[3]);
        if (audio_record(src, sec) == 0) {
            printf("recording %u s from %s...\n", (unsigned)sec, audio_src_name(src));
        } else {
            printf("rejected (1..%u s, not while another job runs)\n", AUDIO_REC_MAX_SEC);
        }
    } else if (strcmp(argv[1], "play") == 0) {
        if (audio_play(route_arg(argc, argv, 2)) == 0) {
            printf("playing %u samples, +%d dB\n", (unsigned)audio_rec_samples(),
                   audio_get_gain_db());
        } else {
            printf("rejected\n");
        }
    } else if (strcmp(argv[1], "loop") == 0 && argc >= 3 && src_arg(argv[2], &src) == 0) {
        uint32_t sec = (argc > 3 && strcmp(argv[3], "hp") != 0) ? (uint32_t)atoi(argv[3]) : 0u;
        audio_route_t route = route_arg(argc, argv, 3);
        if (route == AUDIO_ROUTE_SPK) {
            route = route_arg(argc, argv, 4);
        }
        if (audio_loop(src, sec, route) == 0) {
            printf("monitoring %s -> %s (%s)\n", audio_src_name(src),
                   route == AUDIO_ROUTE_HP ? "headphone" : "speaker",
                   sec ? "timed" : "until `mic stop`");
            if (route == AUDIO_ROUTE_SPK && src != AUDIO_IN_AUX) {
                printf("note: a microphone into the speaker will howl - use headphones or 'hp'\n");
            }
        } else {
            printf("rejected\n");
        }
    } else if (strcmp(argv[1], "gain") == 0 && argc >= 3) {
        if (audio_set_gain_db(atoi(argv[2])) == 0) {
            printf("make-up gain %d dB\n", audio_get_gain_db());
        } else {
            printf("0..40 dB\n");
        }
    } else if (strcmp(argv[1], "ch") == 0 && argc >= 3) {
        if (audio_set_pdm_channel(atoi(argv[2])) == 0) {
            printf("PDM slot %d\n", audio_get_pdm_channel());
        } else {
            printf("0 or 1\n");
        }
    } else if (strcmp(argv[1], "stop") == 0) {
        audio_stop();
        printf("stopped\n");
    } else if (strcmp(argv[1], "status") == 0) {
        printf("job %s, recording %u samples, last peak %d dBFS, rms %d dBFS, voice band %d dBFS, gain +%d dB, pdm slot %d\n",
               audio_job_name(), (unsigned)audio_rec_samples(), audio_last_peak_dbfs(), audio_last_rms_dbfs(),
               audio_last_band_rms_dbfs(),
               audio_get_gain_db(), audio_get_pdm_channel());
    } else {
        mic_usage();
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mic, mic, PDM / headset / AUX-IN capture);

#endif /* CONFIG_SHELL */
