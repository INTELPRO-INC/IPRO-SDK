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
 * @brief `spk` and `mic` shell commands for the DAC and PDM microphone.
 */

#include "audio.h"
#include "audio_speaker_eq.h"
#include "audio_port.h"

#ifdef CONFIG_SHELL

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *mode_name(audio_mode_t m)
{
    switch (m) {
        case AUDIO_MODE_TONE:  return "tone";
        case AUDIO_MODE_REC:   return "record";
        case AUDIO_MODE_PLAY:  return "play";
        case AUDIO_MODE_LOOP:  return "loopback";
        case AUDIO_MODE_SCOPE: return "scope";
        case AUDIO_MODE_MIC:   return "capture";
        default:               return "idle";
    }
}

static void print_status(void)
{
    audio_level_t l;

    audio_get_level(&l);
    printf("state   : %s\n", mode_name(audio_get_mode()));
    printf("speaker : GPIO%d %s (TPA2037D1 EN, ACTIVE HIGH), DAC on GPIO%d\n",
           SPK_PIN_EN, audio_spk_is_enabled() ? "ON" : "off", SPK_PIN_DAC);
    printf("mic     : PDM CLK=GPIO%d DAT=GPIO%d, %s slot, 1.8 V rail\n",
           MIC_PIN_CLK, MIC_PIN_DAT, audio_get_channel() ? "right" : "left");
    printf("gain    : mic %+d dB requested, %+.1f dB read back from the "
           "AUADC\n", audio_get_mic_gain(),
           (double)audio_get_mic_gain_actual());
    printf("          make-up %+d dB (software, applied on playback)\n",
           audio_get_gain());
    printf("level   : peak %.1f dBFS, rms %.1f dBFS over %u frames\n",
           (double)l.peak_dbfs, (double)l.rms_dbfs, (unsigned)l.frames);
    printf("faults  : %u clipped samples, %u capture timeouts\n",
           (unsigned)l.clipped, (unsigned)l.overruns);
    printf("auadc   : %u frames from the DMA ISR, %u dropped\n",
           (unsigned)audio_isr_frames(), (unsigned)audio_isr_dropped());
    if (l.frames) {
        printf("headroom: %+d dB of make-up before the recorded peak clips\n",
               audio_headroom_db());
    }
    printf("recorded: %u samples (%u ms at a nominal %u Hz)\n",
           (unsigned)audio_recorded_samples(),
           (unsigned)(audio_recorded_samples() * 1000u / audio_get_rate()),
           (unsigned)audio_get_rate());

    if (audio_isr_frames() == 0u) {
        printf("hint    : the AUADC completion ISR has never fired, so nothing\n"
               "          is clocking the microphone. That is upstream of any\n"
               "          gain setting - check the 1.8 V mic rail first.\n");
    }
    if (l.peak_dbfs > -0.5f) {
        printf("hint    : input pinned to full scale. If this is `mic loop`,\n"
               "          it is acoustic feedback, not too much gain - try\n"
               "          `mic rec` + `mic stats` with the speaker silent.\n");
    }
}

static void spk_usage(void)
{
    printf(
      "spk - on-chip DAC into the TPA2037D1 class-D amplifier\n"
      "  spk status                 state, gains, level meter\n"
      "  spk on | spk off           amplifier enable (GPIO%d, active HIGH)\n"
      "  spk tone [Hz] [sec] [amp%%] synthesised sine (default 1000 2 30)\n"
      "  spk gain <-20..48>         software make-up gain before the DAC\n"
      "  spk dc <0..1023>           hold a DC code for multimeter checks\n"
      "                             (leaves the amplifier OFF on purpose)\n"
      "  spk eq [on|off]            speaker correction: two peaking cuts,\n"
      "                             -13 dB @ 2850 Hz Q2.65 and -9 dB @ 7225 Hz\n"
      "                             Q4.9, fitted to the measured response.\n"
      "                             Attenuates only - costs ~4 dB of level.\n"
      "  spk stop                   stop whatever is running\n",
      SPK_PIN_EN);
}

static void mic_usage(void)
{
    printf(
      "mic - IM69D129 PDM microphone through the AUADC\n"
      "  mic status                 same view as `spk status`\n"
      "  mic start                  capture only, no output (no feedback)\n"
      "  mic rec [sec]              record into PSRAM (default 3, max 10)\n"
      "  mic play [gain_dB|auto]    play the recording through the DAC;\n"
      "                             `auto` picks the gain from the take\n"
      "  mic loop [sec]             live PDM -> DAC (will howl if it can\n"
      "                             hear the speaker)\n"
      "  mic scope [sec]            scrolling waveform on the LCD\n"
      "  mic gain <0..18>           AUADC digital gain, applied at next start\n"
      "  mic stats                  level meter only\n"
      "  mic dump [off] [n]         print recorded samples\n"
      "  mic measure [sec] [f1] [f2]  duplex acoustic sweep: play pilot+sweep\n"
      "                             on the DAC while recording with the mic\n"
      "                             (default 8 s, 50 -> 15000 Hz)\n"
      "  mic b64                    base64 the recording for host analysis\n"
      "  mic ch <left|right>        PDM slot (before the first capture only)\n"
      "  mic probe                  clock the mic and watch DATA\n"
      "  mic pads [hex]             dump GLB + HBN pad registers for 18/19;\n"
      "                             with an argument, write HBN_PAD_CTRL_2\n"
      "                             first (reset default 0, boots as 0xC00)\n"
      "  mic stop                   stop\n");
}

static void report(int rc, const char *what)
{
    switch (rc) {
        case AUDIO_OK:        printf("%s started\n", what); break;
        case AUDIO_ERR_PARAM: printf("%s: bad argument\n", what); break;
        case AUDIO_ERR_NOMEM: printf("%s: out of memory\n", what); break;
        case AUDIO_ERR_STATE: printf("%s: nothing recorded yet\n", what); break;
        case AUDIO_ERR_HW:    printf("%s: hardware init failed\n", what); break;
        default:              printf("%s: error %d\n", what, rc); break;
    }
}

int cmd_spk(int argc, char **argv)
{
    const char *sub;

    if (argc < 2) {
        spk_usage();
        return 0;
    }
    sub = argv[1];

    if (strcmp(sub, "status") == 0) {
        print_status();
    } else if (strcmp(sub, "on") == 0) {
        audio_spk_enable(1);
        printf("amplifier ON (GPIO%d high)\n", SPK_PIN_EN);
    } else if (strcmp(sub, "off") == 0) {
        audio_spk_enable(0);
        printf("amplifier off (GPIO%d low)\n", SPK_PIN_EN);
    } else if (strcmp(sub, "tone") == 0) {
        uint32_t f   = (argc > 2) ? (uint32_t)atoi(argv[2]) : 1000u;
        uint32_t sec = (argc > 3) ? (uint32_t)atoi(argv[3]) : 2u;
        uint32_t amp = (argc > 4) ? (uint32_t)atoi(argv[4]) : 30u;
        report(audio_tone(f, sec, amp), "tone");
    } else if (strcmp(sub, "gain") == 0) {
        if (argc < 3) {
            printf("make-up gain = %+d dB\n", audio_get_gain());
        } else if (audio_set_gain(atoi(argv[2])) != AUDIO_OK) {
            printf("gain must be -20..48 dB\n");
        } else {
            printf("make-up gain = %+d dB\n", audio_get_gain());
        }
    } else if (strcmp(sub, "dc") == 0) {
        if (argc < 3) {
            printf("usage: spk dc <0..1023>\n");
        } else {
            report(audio_dac_dc(atoi(argv[2])), "dc");
        }
    } else if (strcmp(sub, "eq") == 0) {
        if (argc > 2) {
            audio_speaker_eq_enable(strcmp(argv[2], "off") != 0);
        }
        printf("speaker EQ: %s  (-13 dB @ 2850 Hz Q2.65, -9 dB @ 7225 Hz Q4.9)\n",
               audio_speaker_eq_enabled() ? "ON" : "OFF");
    } else if (strcmp(sub, "stop") == 0) {
        audio_stop();
        printf("stopped\n");
    } else {
        spk_usage();
    }
    return 0;
}

int cmd_mic(int argc, char **argv)
{
    const char *sub;

    if (argc < 2) {
        mic_usage();
        return 0;
    }
    sub = argv[1];

    if (strcmp(sub, "status") == 0 || strcmp(sub, "stats") == 0) {
        print_status();
    } else if (strcmp(sub, "start") == 0) {
        report(audio_mic_start(), "capture");
    } else if (strcmp(sub, "rec") == 0) {
        uint32_t sec = (argc > 2) ? (uint32_t)atoi(argv[2]) : 3u;
        report(audio_record(sec), "record");
    } else if (strcmp(sub, "play") == 0) {
        if (argc > 2 && strcmp(argv[2], "auto") == 0) {
            report(audio_play_auto(), "playback");
        } else if (argc > 2) {
            report(audio_play_gain(atoi(argv[2])), "playback");
        } else {
            report(audio_play(), "playback");
        }
    } else if (strcmp(sub, "loop") == 0) {
        uint32_t sec = (argc > 2) ? (uint32_t)atoi(argv[2]) : 0u;
        report(audio_loop(sec), "loopback");
    } else if (strcmp(sub, "scope") == 0) {
        uint32_t sec = (argc > 2) ? (uint32_t)atoi(argv[2]) : 0u;
        report(audio_scope(sec), "scope");
    } else if (strcmp(sub, "gain") == 0) {
        if (argc < 3) {
            printf("mic digital gain = %+d dB\n", audio_get_mic_gain());
        } else if (audio_set_mic_gain(atoi(argv[2])) != AUDIO_OK) {
            printf("mic gain must be 0..18 dB (18 is the hardware maximum)\n");
        } else {
            printf("mic digital gain = %+d dB (applied at next start)\n",
                   audio_get_mic_gain());
        }
    } else if (strcmp(sub, "measure") == 0) {
        uint32_t sec = (argc > 2) ? (uint32_t)atoi(argv[2]) : 8u;
        uint32_t f1  = (argc > 3) ? (uint32_t)atoi(argv[3]) : 50u;
        uint32_t f2  = (argc > 4) ? (uint32_t)atoi(argv[4]) : 15000u;
        printf("measure: 1 s pilot @ 1000 Hz, then %lu s sweep %lu -> %lu Hz\n",
               (unsigned long)sec, (unsigned long)f1, (unsigned long)f2);
        printf("keep the room quiet; `mic b64` when it finishes\n");
        report(audio_measure(sec, f1, f2, 60u), "measure");
    } else if (strcmp(sub, "b64") == 0) {
        report(audio_dump_b64(), "b64");
    } else if (strcmp(sub, "dump") == 0) {
        uint32_t off = (argc > 2) ? (uint32_t)atoi(argv[2]) : 0u;
        uint32_t n   = (argc > 3) ? (uint32_t)atoi(argv[3]) : 16u;
        if (audio_dump_samples(off, n) != AUDIO_OK) {
            printf("nothing recorded - run `mic rec` first\n");
        }
    } else if (strcmp(sub, "ch") == 0) {
        if (argc < 3) {
            printf("channel = %s\n", audio_get_channel() ? "right" : "left");
        } else if (audio_set_channel(strcmp(argv[2], "right") == 0)
                   != AUDIO_OK) {
            printf("PDM is already initialised; the slot can only be chosen\n"
                   "before the first capture, so reboot to change it.\n");
        } else {
            printf("channel = %s\n", audio_get_channel() ? "right" : "left");
        }
    } else if (strcmp(sub, "pads") == 0) {
        if (argc > 2) {
            audio_write_pad_ctrl2((uint32_t)strtoul(argv[2], NULL, 16));
        }
        audio_dump_pads();
    } else if (strcmp(sub, "probe") == 0) {
        uint32_t hz = 0, ones = 0, edges = 0, total = 0;
        if (audio_mic_probe(&hz, &ones, &edges, &total) != AUDIO_OK) {
            printf("probe failed to start the AUADC\n");
            return 0;
        }
        printf("sampled DATA %u times at ~%u Hz while the AUADC clocked\n"
               "PDM_CLK at 2.032 MHz (in spec for an IM69D129)\n",
               (unsigned)total, (unsigned)hz);
        printf("DATA: %u ones / %u samples (%u%%), %u transitions\n",
               (unsigned)ones, (unsigned)total,
               (unsigned)(total ? ones * 100u / total : 0u), (unsigned)edges);
        if (edges == 0u) {
            printf("verdict: DATA never moved with a correct 2.032 MHz clock\n"
                   "         on GPIO%d. Combined with the pull-up/pull-down\n"
                   "         result logged above this is a hardware answer,\n"
                   "         not a firmware one.\n", MIC_PIN_CLK);
            printf("         Measure, in this order:\n"
                   "           1. DMIC_1V8 at the mic's VDD (pin 5) - a PDM\n"
                   "              mic with no supply looks exactly like this\n"
                   "           2. GPIO%d for a 2 MHz square wave while this\n"
                   "              command runs - proves CLK leaves the SoC\n"
                   "           3. continuity of R29/R31 (both 0R)\n",
                   MIC_PIN_CLK);
        } else {
            printf("verdict: the mic IS driving PDM - %u%% ones density and\n"
                   "         %u transitions is a healthy idle sigma-delta\n"
                   "         stream. Pads and hardware are both good.\n"
                   "         Now run `mic rec 3` then `mic stats`.\n",
                   (unsigned)(total ? ones * 100u / total : 0u),
                   (unsigned)edges);
        }
        printf("note: DATA is handed back to the AUADC, so `mic rec` still\n"
               "      works without a reboot.\n");
    } else if (strcmp(sub, "stop") == 0) {
        audio_stop();
        printf("stopped\n");
    } else {
        mic_usage();
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_spk, spk, DAC speaker output test);
SHELL_CMD_EXPORT_ALIAS(cmd_mic, mic, PDM microphone test);

#endif /* CONFIG_SHELL */
