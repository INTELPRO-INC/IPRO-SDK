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
 * @file app_audio_route.c
 * @brief Audio routing layer — connects input, output, and RTC modules
 *
 * Handles upsample (16kHz->32kHz), local echo suppression, and barge-in
 * detection.  All inter-module audio flow goes through here.
 *
 * Echo suppression strategy (no SDK AEC available):
 *  B) State-based: when AI speaking, raise silence gate threshold
 *  C) Reference-based: track speaker energy, adapt threshold to match
 *     expected echo level from speaker → DMIC acoustic coupling
 *
 * Combined: threshold = max(base, echo_floor, rx_peak × coupling_factor)
 */

#include "app_audio_route.h"
#include "app_lcd.h"

#include <stdlib.h>
#include <string.h>
#include "app_log.h"

#define LOG_TAG "ROUTE"

/* ------------------------------------------------------------------ */
/* Echo suppression parameters (B+C combined)                          */
/* ------------------------------------------------------------------ */

/* Normal silence gate when AI is NOT speaking */
#define SILENCE_THRESHOLD       64

/* [B] Minimum echo gate when AI IS speaking — blocks low-level echo
 * even when speaker is quiet.  Tunable via shell `echo_gate <N>`. */
#define ECHO_GATE_FLOOR_DEFAULT 3000
static int s_echo_gate_floor = ECHO_GATE_FLOOR_DEFAULT;

/* [C] Speaker-to-mic coupling factor (Q8 fixed-point, 0-255).
 * echo_threshold = rx_peak_track × coupling / 256.
 * Physical meaning: if speaker plays peak=10000, and coupling=128 (0.5),
 * we expect DMIC to pick up peak≈5000 → set threshold to 5000.
 * Tune via shell `echo_gate coupling <N>`.  Typical range: 64-192. */
#define ECHO_COUPLING_Q8_DEFAULT 128  /* 0.50 */
static int s_echo_coupling_q8 = ECHO_COUPLING_Q8_DEFAULT;

/* [C] RX peak tracking: exponential decay factor (Q8).
 * Decays tracked peak when no new RX audio.  230/256 ≈ 0.90 per frame. */
#define RX_PEAK_DECAY_Q8 230
static volatile int32_t s_rx_peak_track;

/* Barge-in: consecutive high-energy frames required to confirm interrupt.
 * 15 frames × 20ms = 300ms sustained speech needed — long enough to filter
 * out transient echo spikes but short enough for intentional interruption. */
#define BARGEIN_FRAMES 15
static volatile int s_bargein_count;
static volatile int s_bargein_active;

/* AI state */
static volatile int s_ai_speaking;

/* Stored ops pointers */
static const audio_input_ops_t  *s_input;
static const audio_output_ops_t *s_output;
static const rtc_ops_t          *s_rtc;

/* Upsample buffer: 20ms @ 32kHz = 640 samples */
static int16_t s_up_buf[640];

/* Debug counters */
static volatile uint32_t s_echo_suppress_count;
static volatile uint32_t s_echo_pass_count;
static volatile int32_t  s_last_threshold;
static volatile int16_t  s_last_mic_peak;

/* ------------------------------------------------------------------ */
/* RTC RX -> track speaker energy -> upsample 16kHz->32kHz -> DAC      */
/* ------------------------------------------------------------------ */

static void on_rtc_rx_audio(const int16_t *pcm, uint32_t num_samples)
{
    /* [C] Track speaker peak for echo estimation */
    int16_t rx_peak = 0;
    for (uint32_t i = 0; i < num_samples; i++) {
        int16_t abs_val = (pcm[i] >= 0) ? pcm[i] : -pcm[i];
        if (abs_val > rx_peak)
            rx_peak = abs_val;
    }
    /* Update tracked peak: take max of new peak and decayed old peak */
    int32_t decayed = (s_rx_peak_track * RX_PEAK_DECAY_Q8) >> 8;
    s_rx_peak_track = (rx_peak > decayed) ? rx_peak : decayed;

    /* Upsample 16kHz -> 32kHz (linear interpolation) */
    uint32_t out_count = num_samples * 2;
    if (out_count > 640)
        out_count = 640;

    for (uint32_t i = 0; i < num_samples && i * 2 + 1 < 640; i++) {
        s_up_buf[i * 2] = pcm[i];
        if (i + 1 < num_samples) {
            s_up_buf[i * 2 + 1] =
                (int16_t)(((int32_t)pcm[i] + (int32_t)pcm[i + 1]) / 2);
        } else {
            s_up_buf[i * 2 + 1] = pcm[i];
        }
    }

    s_output->feed(s_up_buf, out_count);
}

/* ------------------------------------------------------------------ */
/* AI state callback                                                   */
/* ------------------------------------------------------------------ */

static void on_ai_state_changed(rtc_ai_state_t state)
{
    if (state == RTC_AI_STATE_SPEAKING) {
        s_ai_speaking = 1;
    } else {
        s_ai_speaking = 0;
        s_bargein_active = 0;
        s_bargein_count = 0;
    }

    /* Update LCD display */
    app_lcd_set_ai_state(state);
}

/* ------------------------------------------------------------------ */
/* MIC capture -> echo suppression (B+C) + barge-in -> RTC send       */
/* ------------------------------------------------------------------ */

static void on_mic_frame(const int16_t *pcm, uint32_t num_samples)
{
    /* Measure mic peak */
    int16_t peak = 0;
    for (uint32_t i = 0; i < num_samples; i++) {
        int16_t abs_val = (pcm[i] >= 0) ? pcm[i] : -pcm[i];
        if (abs_val > peak)
            peak = abs_val;
    }
    s_last_mic_peak = peak;

    /* Compute adaptive threshold */
    int32_t threshold;
    if (s_ai_speaking) {
        /* [B] Start with echo gate floor */
        threshold = s_echo_gate_floor;

        /* [C] Scale up with tracked speaker energy × coupling factor */
        int32_t echo_est = ((int32_t)s_rx_peak_track * s_echo_coupling_q8) >> 8;
        if (echo_est > threshold)
            threshold = echo_est;
    } else {
        /* AI not speaking — normal silence gate */
        threshold = SILENCE_THRESHOLD;
    }
    s_last_threshold = threshold;

    /* Gate */
    if (peak < threshold) {
        s_echo_suppress_count++;
        /* Reset barge-in count — must be CONSECUTIVE frames above threshold */
        if (s_ai_speaking && !s_bargein_active && s_bargein_count > 0) {
            s_bargein_count = 0;
        }
        return;
    }

    /* Barge-in logic when AI is speaking */
    if (s_ai_speaking && !s_bargein_active) {
        s_bargein_count++;
        if (s_bargein_count < BARGEIN_FRAMES) {
            /* Not yet confirmed — suppress to avoid false trigger */
            s_echo_suppress_count++;
            return;
        }
        /* Confirmed barge-in: N consecutive frames above threshold */
        s_bargein_active = 1;
        IPRO_LOGI(LOG_TAG, "Barge-in confirmed (peak=%d threshold=%ld count=%d)",
                  peak, (long)threshold, s_bargein_count);
    }

    s_echo_pass_count++;
    s_rtc->send_audio(pcm, num_samples * sizeof(int16_t));
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */

app_err_t audio_route_init(const audio_route_cfg_t *cfg)
{
    if (!cfg || !cfg->input || !cfg->output || !cfg->rtc)
        return APP_ERR_INVALID;

    s_input  = cfg->input;
    s_output = cfg->output;
    s_rtc    = cfg->rtc;

    s_rtc->set_rx_callback(on_rtc_rx_audio);
    s_rtc->set_ai_state_callback(on_ai_state_changed);
    s_input->set_callback(on_mic_frame);

    IPRO_LOGI(LOG_TAG,
              "Audio route: %s -> %s -> %s | echo_gate: floor=%d coupling=%.2f",
              s_input->name, s_rtc->name, s_output->name,
              s_echo_gate_floor, (float)s_echo_coupling_q8 / 256.0f);

    return APP_OK;
}

/* ------------------------------------------------------------------ */
/* Shell commands                                                     */
/* ------------------------------------------------------------------ */
#ifdef CONFIG_SHELL
#include <stdio.h>
#include "shell.h"

static int cmd_echo_gate(int argc, char **argv)
{
    if (argc >= 3 && strcmp(argv[1], "coupling") == 0) {
        int val = atoi(argv[2]);
        if (val >= 0 && val <= 255)
            s_echo_coupling_q8 = val;
    } else if (argc >= 2) {
        int val = atoi(argv[1]);
        if (val >= 0 && val <= 32767)
            s_echo_gate_floor = val;
    }

    printf("\r\n=== Echo Suppression (B+C) ===\r\n");
    printf("  AI speaking     : %s\r\n", s_ai_speaking ? "YES" : "no");
    printf("  [B] Gate floor  : %d (when AI speaking, min threshold)\r\n",
           s_echo_gate_floor);
    printf("  [C] Coupling    : %d/256 = %.2f (speaker→mic factor)\r\n",
           s_echo_coupling_q8, (float)s_echo_coupling_q8 / 256.0f);
    printf("  [C] RX peak trk : %ld (speaker energy tracker)\r\n",
           (long)s_rx_peak_track);
    printf("  Echo threshold  : %ld (current adaptive value)\r\n",
           (long)s_last_threshold);
    printf("  Last mic peak   : %d\r\n", s_last_mic_peak);
    printf("  Suppressed      : %lu frames\r\n",
           (unsigned long)s_echo_suppress_count);
    printf("  Passed          : %lu frames\r\n",
           (unsigned long)s_echo_pass_count);
    printf("  Barge-in        : active=%d count=%d (need %d consecutive)\r\n",
           s_bargein_active, s_bargein_count, BARGEIN_FRAMES);
    printf("\r\nUsage:\r\n");
    printf("  echo_gate <floor>          — set min threshold when AI speaking\r\n");
    printf("  echo_gate coupling <0-255> — set speaker-to-mic coupling (Q8)\r\n");
    printf("\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_echo_gate, echo_gate, Echo suppression tuning);

#endif /* CONFIG_SHELL */
