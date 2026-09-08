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
 * @file app_shell_commands.c
 * @brief Shell Commands for LE Audio Headset
 * 
 * Provides interactive debugging commands.
 */

#include "vcp_server.h"
#include "app_audio.h"

#ifdef CONFIG_AUDIO_OUTPUT_UAC
#include "app_uac.h"
#elif defined(CONFIG_BSP_USING_DAC)
#include "app_pcm_monitor.h"
#include "app_dac.h"
#include "audio_speaker_eq.h"
#include "audio_bass_boost.h"
#endif
#include "shell.h"
#include "ipro_log.h"

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAG "SHELL"

/*******************************************************************************
 * Media Info Structure (for MCP - Media Control Profile, Phase 3)
 ******************************************************************************/
typedef struct {
    char player_name[32];
    char track_title[64];
    uint32_t duration_ms;
    uint32_t position_ms;
    uint8_t state;  // 0=Inactive, 1=Playing, 2=Paused, 3=Seeking
} app_media_info_t;

/*******************************************************************************
 * BLE API Wrappers for Shell Commands (Phase 2)
 ******************************************************************************/

/**
 * @brief Get current volume from VCP server
 */
uint8_t app_ble_get_volume(void)
{
	return vcp_server_get_volume();
}

/**
 * @brief Get current mute state from VCP server
 */
bool app_ble_is_muted(void)
{
	return vcp_server_is_muted();
}

/**
 * @brief Get media information (stub for Phase 2)
 */
void app_ble_get_media_info(app_media_info_t *info)
{
	if (info == NULL) {
		return;
	}
	
	/* TODO Phase 3: Implement MCP (Media Control Profile) support */
	memset(info, 0, sizeof(app_media_info_t));
	strncpy(info->player_name, "LC3 Audio", sizeof(info->player_name) - 1);
	strncpy(info->track_title, "LE Audio Stream", sizeof(info->track_title) - 1);
	info->state = 0; /* Inactive */
}

/*******************************************************************************
 * Shell Commands
 ******************************************************************************/

/**
 * @brief pcm command - Show audio statistics
 *
 * Usage:
 *   pcm stats      - Show audio statistics
 */
static void cmd_pcm(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: pcm <stats>\n");
        return;
    }

    if (strcmp(argv[1], "stats") == 0) {
        app_audio_stats_t stats;
        app_audio_get_stats(&stats);

        printf("\n=== Audio Statistics ===\n");
        printf("Frames Received:  %lu\n", (unsigned long)stats.frames_received);
        printf("Frames Decoded:   %lu\n", (unsigned long)stats.frames_decoded);
        printf("Frames Lost:      %lu\n", (unsigned long)stats.frames_lost);
        printf("Decode Errors:    %lu\n", (unsigned long)stats.decode_errors);

        if (stats.frames_received > 0) {
            float loss_rate = (float)stats.frames_lost * 100.0f / stats.frames_received;
            printf("Loss Rate:        %.2f%%\n", loss_rate);
        }

        // Frame timing statistics
        printf("\n=== Frame Timing ===\n");
        printf("Interval Min:     %lu us\n", (unsigned long)stats.frame_interval_min_us);
        printf("Interval Max:     %lu us\n", (unsigned long)stats.frame_interval_max_us);
        printf("Interval Avg:     %lu us\n", (unsigned long)stats.frame_interval_avg_us);
        printf("Late Frames:      %lu (>1.5x expected)\n", (unsigned long)stats.late_frames);
        printf("Burst Frames:     %lu (<0.5x expected)\n", (unsigned long)stats.burst_frames);

        // Accumulated delay statistics
        printf("\n=== Accumulated Delay ===\n");
        printf("Current Delay:    %ld us (%ld ms)\n",
               (long)stats.current_delay_us, (long)(stats.current_delay_us / 1000));
        printf("Max Delay:        %ld us (%ld ms)\n",
               (long)stats.max_delay_us, (long)(stats.max_delay_us / 1000));
        printf("Min Delay:        %ld us (%ld ms)\n",
               (long)stats.min_delay_us, (long)(stats.min_delay_us / 1000));

        // Output statistics
#ifdef CONFIG_AUDIO_OUTPUT_UAC
        app_uac_stats_t uac_stats;
        app_uac_get_stats(&uac_stats);

        printf("\n=== UAC Statistics ===\n");
        printf("Frames Sent:      %lu\n", (unsigned long)uac_stats.frames_sent);
        printf("Underruns:        %lu (max consec: %lu)\n",
               (unsigned long)uac_stats.underruns,
               (unsigned long)uac_stats.max_consecutive_underruns);
        printf("Overruns:         %lu\n", (unsigned long)uac_stats.overruns);
        printf("Written:          %lu samples\n", (unsigned long)uac_stats.samples_written);
#elif defined(CONFIG_BSP_USING_DAC)
        app_dac_stats_t dac_stats;
        app_dac_get_stats(&dac_stats);

        printf("\n=== DAC Statistics ===\n");
        printf("Buffer Config:    %d x %dms = %dms total\n",
               DAC_BUFFER_COUNT, DAC_BUFFER_DURATION_MS,
               DAC_BUFFER_COUNT * DAC_BUFFER_DURATION_MS);
        printf("Buffers Sent:     %lu\n", (unsigned long)dac_stats.buffers_sent);
        printf("Underruns:        %lu\n", (unsigned long)dac_stats.underruns);
        printf("Consec Underruns: %lu (max=%lu)\n",
               (unsigned long)dac_stats.consecutive_underruns,
               (unsigned long)dac_stats.max_consecutive_underruns);
        printf("Last Underrun:    %lu ms\n", (unsigned long)dac_stats.last_underrun_time_ms);
        printf("Min Buf Ready:    %u/%d (lowest water mark)\n",
               dac_stats.min_buffers_ready, DAC_BUFFER_COUNT);
        printf("Buf@Underrun:     %u/%d\n",
               dac_stats.buffers_ready_at_underrun, DAC_BUFFER_COUNT);
        printf("Errors:           %lu\n", (unsigned long)dac_stats.errors);
#endif
    }
    else {
        printf("Unknown option: %s\n", argv[1]);
    }
}

/**
 * @brief dac command - Control DAC output (DAC mode only)
 *
 * Usage:
 *   dac vol <0-100>      - Set volume (0-100 -> gain 0.0-1.0)
 *   dac gain <0.0-2.0>   - Set linear gain directly
 *   dac status           - Show DAC status
 */
#if defined(CONFIG_BSP_USING_DAC) && !defined(CONFIG_AUDIO_OUTPUT_UAC)
static void cmd_dac(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: dac <vol|gain|status>\n");
        printf("  dac vol <0-100>     - Set volume level (gain 0.0-1.0)\n");
        printf("  dac gain <0.0-2.0>  - Set linear gain directly\n");
        printf("  dac status          - Show DAC status\n");
        printf("\nCurrent gain: %.2f\n", app_dac_get_gain());
        return;
    }

    if (strcmp(argv[1], "vol") == 0 && argc >= 3) {
        int volume = atoi(argv[2]);
        if (volume < 0) volume = 0;
        if (volume > 100) volume = 100;

        app_dac_set_volume(volume);
        printf("Volume set to %d%% (gain=%.2f)\n", volume, app_dac_get_gain());
    }
    else if (strcmp(argv[1], "gain") == 0 && argc >= 3) {
        float gain = strtof(argv[2], NULL);
        if (gain < 0.0f) gain = 0.0f;
        if (gain > 2.0f) gain = 2.0f;

        app_dac_set_gain(gain);
        printf("Gain set to %.2f", gain);
        if (gain > 1.43f) {
            printf(" (warning: may clip on loud signals)");
        }
        printf("\n");
    }
    else if (strcmp(argv[1], "status") == 0) {
        app_dac_stats_t stats;
        app_dac_get_stats(&stats);

        printf("\n=== DAC Status ===\n");
        printf("Ready:            %s\n", app_dac_is_ready() ? "Yes" : "No");
        printf("Current Gain:     %.2f\n", app_dac_get_gain());
        printf("Buffers Sent:     %lu\n", (unsigned long)stats.buffers_sent);
        printf("Underruns:        %lu\n", (unsigned long)stats.underruns);
        printf("Errors:           %lu\n", (unsigned long)stats.errors);

        const app_audio_config_t* cfg = app_audio_get_config();
        printf("\n=== Audio Config ===\n");
        printf("Sample Rate:      %lu Hz\n", (unsigned long)cfg->sample_rate);
        printf("Frame Duration:   %lu us\n", (unsigned long)cfg->frame_duration_us);
        printf("Octets/Frame:     %u bytes\n", cfg->octets_per_frame);
        printf("Channels:         %u\n", cfg->num_channels);

        // Calculate bitrate
        uint32_t bitrate = cfg->octets_per_frame * 8 * 1000000 / cfg->frame_duration_us / 1000;
        printf("Bitrate:          %lu kbps\n", (unsigned long)bitrate);
    }
    else {
        printf("Unknown option: %s\n", argv[1]);
    }
}
#endif /* CONFIG_BSP_USING_DAC && !CONFIG_AUDIO_OUTPUT_UAC */

/**
 * @brief audio command - Audio information
 * 
 * Usage:
 *   audio info         - Show audio configuration
 */
static void cmd_audio(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: audio <info>\n");
        printf("  audio info  - Display audio configuration\n");
        return;
    }
    
    if (strcmp(argv[1], "info") == 0) {
        const app_audio_config_t* cfg = app_audio_get_config();
        
        printf("\n╔════════════════════════════════════════╗\n");
        printf("║       LE Audio Configuration          ║\n");
        printf("╠════════════════════════════════════════╣\n");
        printf("║ Sample Rate:    %6lu Hz           ║\n", (unsigned long)cfg->sample_rate);
        printf("║ Frame Duration: %6lu us           ║\n", (unsigned long)cfg->frame_duration_us);
        printf("║ Octets/Frame:   %6u bytes        ║\n", cfg->octets_per_frame);
        printf("║ Channels:       %6u              ║\n", cfg->num_channels);
        
        uint32_t bitrate = cfg->octets_per_frame * 8 * 1000000 / cfg->frame_duration_us / 1000;
        printf("║ Bitrate:        %6lu kbps         ║\n", (unsigned long)bitrate);
        
        int samples_per_frame = cfg->sample_rate * cfg->frame_duration_us / 1000000;
        printf("║ Samples/Frame:  %6d              ║\n", samples_per_frame);
        
        printf("╚════════════════════════════════════════╝\n");
        
        // Show statistics
        app_audio_stats_t stats;
        app_audio_get_stats(&stats);
        
        if (stats.frames_received > 0) {
            printf("\n╔════════════════════════════════════════╗\n");
            printf("║       Audio Statistics                 ║\n");
            printf("╠════════════════════════════════════════╣\n");
            printf("║ Received:       %6lu frames        ║\n", (unsigned long)stats.frames_received);
            printf("║ Decoded:        %6lu frames        ║\n", (unsigned long)stats.frames_decoded);
            printf("║ Lost:           %6lu frames        ║\n", (unsigned long)stats.frames_lost);
            printf("║ Errors:         %6lu              ║\n", (unsigned long)stats.decode_errors);
            
            float loss_rate = (float)stats.frames_lost * 100.0f / stats.frames_received;
            printf("║ Loss Rate:      %6.2f %%           ║\n", loss_rate);
            
            printf("╚════════════════════════════════════════╝\n");
        }
    }
}

/**
 * @brief test_tone command - Play test tone for hardware verification (DAC mode only)
 *
 * Usage:
 *   test_tone                 - Play 1kHz tone for 3 seconds (amplitude 10000)
 *   test_tone <duration>      - Play for specified duration in seconds
 *   test_tone <dur> <amp>     - Play with specific duration and amplitude
 *   test_tone stop            - Stop current test tone
 *   test_tone max             - Play with maximum amplitude (20000) for 5 seconds
 *
 * NOTE: Only available in DAC mode (not UAC mode)
 * 
 * Examples:
 *   test_tone          - Default: 3 sec, amplitude 10000
 *   test_tone 5        - 5 seconds, amplitude 10000
 *   test_tone 10 8000  - 10 seconds, amplitude 8000
 *   test_tone 0 3000   - Continuous, amplitude 3000 (stop with: test_tone stop)
 *   test_tone max      - Maximum volume test
 */
#if defined(CONFIG_BSP_USING_DAC) && !defined(CONFIG_AUDIO_OUTPUT_UAC)
static void cmd_test_tone(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "stop") == 0) {
        app_audio_stop_test_tone();
        printf("Test tone stopped\n");
        return;
    }
    
    if (argc >= 2 && strcmp(argv[1], "max") == 0) {
        printf("\n⚠️  WARNING: MAXIMUM AMPLITUDE TEST\n");
        printf("   Remove headphones if connected!\n");
        printf("   Playing 20000 amplitude for 5 seconds...\n\n");
        int ret = app_audio_play_test_tone(5000, 20000);
        if (ret != 0) {
            printf("❌ Failed to start test tone\n");
        } else {
            printf("✅ MAX amplitude test tone started\n");
        }
        return;
    }
    
    uint32_t duration_sec = 3;    // Default: 3 seconds
    int16_t amplitude = 10000;    // Default: 10000 (increased from 5000)
    
    if (argc >= 2) {
        duration_sec = atoi(argv[1]);
    }
    
    if (argc >= 3) {
        amplitude = atoi(argv[2]);
        if (amplitude < 100 || amplitude > 20000) {
            printf("Error: Amplitude must be 100-20000 (recommend 5000-10000)\n");
            return;
        }
    }
    
    uint32_t duration_ms = duration_sec * 1000;
    
    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║         Playing Test Tone (1kHz)           ║\n");
    printf("╠══════════════════════════════════════════════╣\n");
    printf("║ Duration:   %5lu %s                    ║\n", 
           duration_sec > 0 ? (unsigned long)duration_sec : 0UL,
           duration_sec > 0 ? "seconds" : "(continuous)");
    printf("║ Amplitude:  %5d (peak)                   ║\n", amplitude);
    printf("║ Frequency:  1000 Hz                       ║\n");
    printf("╚══════════════════════════════════════════════╝\n");
    
    if (duration_sec == 0) {
        printf("⚠️  Continuous mode - use 'test_tone stop' to stop\n");
    }
    
    int ret = app_audio_play_test_tone(duration_ms, amplitude);
    if (ret != 0) {
        printf("❌ Failed to start test tone\n");
    } else {
        printf("✅ Test tone started\n");
        printf("\n💡 What to check:\n");
        printf("   1. Do you hear a 1kHz beep from speaker?\n");
        printf("   2. Check GPIO 22 (speaker enable) is HIGH\n");
        printf("   3. Check GPIO 23 (DAC output) with oscilloscope\n");
    }
}

/**
 * @brief dac_raw command - Test DAC with raw reference-like data
 * 
 * Sends raw uint16_t values directly to DAC, mimicking reference implementation
 * to verify DAC hardware is working correctly.
 * 
 * Usage:
 *   dac_raw           - Default amplitude (10000)
 *   dac_raw <amp>     - Custom amplitude (100-30000)
 */
static void cmd_dac_raw(int argc, char **argv)
{
    int16_t amplitude = 10000;  // Default
    
    if (argc >= 2) {
        amplitude = atoi(argv[1]);
        if (amplitude < 100) amplitude = 100;
        if (amplitude > 30000) amplitude = 30000;
    }
    
    printf("\n╔══════════════════════════════════════════════╗\n");
    printf("║       DAC Raw Test (Square Wave)           ║\n");
    printf("╚══════════════════════════════════════════════╝\n\n");
    
    // Generate large continuous buffer - 9600 samples (300ms at 32kHz)
    // Like reference code (demo_dac_dma) playing complete audio file
    const int total_samples = 9600;
    int16_t *test_samples = (int16_t *)malloc(total_samples * sizeof(int16_t));
    if (!test_samples) {
        printf("❌ Failed to allocate %d samples\n", total_samples);
        return;
    }
    
    // Generate square wave in single large buffer
    for (int i = 0; i < total_samples; i++) {
        // Square wave: alternate between +amplitude and -amplitude every 32 samples
        test_samples[i] = (i % 64 < 32) ? amplitude : -amplitude;
    }
    
    printf("Amplitude: %d (PCM)\n", amplitude);
    printf("Playing %d samples (300ms) in SINGLE DMA transfer...\n", total_samples);
    printf("You should hear a clear tone\n\n");
    
    // Single large DMA transfer - exactly like reference code (demo_dac_dma)
    int ret = app_dac_write(test_samples, total_samples, true);
    
    free(test_samples);
    
    if (ret == 0) {
        printf("\n✅ Test complete: Single continuous transfer successful\n");
        printf("💡 If you heard a tone, DAC is working!\n");
        printf("   Try: 'dac_raw 20000' for louder test\n");
        printf("   Try: 'test_tone' for continuous sine wave\n");
    } else {
        printf("\n❌ DAC write failed\n");
    }
}
#endif /* CONFIG_BSP_USING_DAC && !CONFIG_AUDIO_OUTPUT_UAC */

/**
 * @brief vol command - Display volume control status (Phase 2: Re-enabled)
 */
static void cmd_dacprof(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "reset") == 0) {
        app_dac_profile_reset();
        app_audio_profile_reset();
        printf("profile reset - play for a few seconds then run `dacprof`\n");
        return;
    }
    app_dac_profile_report();
    app_audio_profile_report();
}
SHELL_CMD_EXPORT_ALIAS(cmd_dacprof, dacprof, DAC chain cycles per sample);

static void cmd_bass(int argc, char **argv)
{
    if (argc >= 2) {
        if (strcmp(argv[1], "off") == 0) {
            audio_bass_boost_enable(false);
        } else if (strcmp(argv[1], "on") == 0) {
            audio_bass_boost_enable(true);
        } else {
            /* a bare number sets the mix and turns it on */
            audio_bass_boost_set_gain_q8((uint32_t)atoi(argv[1]));
            audio_bass_boost_enable(true);
        }
    }
    uint32_t g = audio_bass_boost_get_gain_q8();
    printf("\nbass enhance: %s, mix %lu/256 (%lu%%)\n",
           audio_bass_boost_enabled() ? "ON" : "OFF",
           (unsigned long)g, (unsigned long)(g * 100u / 256u));
    printf("  removes 40-380 Hz (unreproducible, -6..-7.5 dB RMS of amp power)\n");
    printf("  synthesises its harmonic series into 550-1600 Hz instead\n");
    printf("  usage: bass on | off | <0-1024>   (256 = full envelope match)\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_bass, bass, Psychoacoustic bass enhancement);

static void cmd_eq(int argc, char **argv)
{
    if (argc >= 2) {
        audio_speaker_eq_enable(strcmp(argv[1], "off") != 0);
    }
    printf("\nspeaker EQ: %s\n", audio_speaker_eq_enabled() ? "ON" : "OFF");
    printf("  -13.0 dB @ 2850 Hz Q2.65   (main cone resonance, +12.3 dB raw)\n");
    printf("  -9.0 dB @ 7225 Hz Q4.90    (second resonance, +5.9 dB raw)\n");
    printf("  fitted to the IPRO7AI EVK measured response; attenuates only,\n");
    printf("  so it costs about 4 dB of level - make it up with `dac gain`.\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_eq, eq, Speaker correction EQ on/off);

static void cmd_vol(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    printf("\n=== Volume Control (VCP) ===\n");
    printf("Volume:  %u/255 (%u%%)\n", 
           app_ble_get_volume(), 
           (app_ble_get_volume() * 100) / 255);
    printf("Muted:   %s\n", app_ble_is_muted() ? "YES" : "NO");
}

/**
 * @brief media command - Display media player status (Phase 2: Simplified)
 */
static void cmd_media(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    app_media_info_t info;
    app_ble_get_media_info(&info);
    
    const char* state_names[] = {
        "Inactive", "Playing", "Paused", "Seeking"
    };
    
    printf("\n=== Media Control (MCP) ===\n");
    printf("Player:   %s\n", info.player_name[0] ? info.player_name : "(none)");
    printf("Track:    %s\n", info.track_title[0] ? info.track_title : "(none)");
    printf("State:    %s\n", state_names[info.state < 4 ? info.state : 0]);
    
    if (info.duration_ms > 0) {
        uint32_t pos_sec = info.position_ms / 1000;
        uint32_t dur_sec = info.duration_ms / 1000;
        printf("Position: %lu:%02lu / %lu:%02lu\n",
               (unsigned long)(pos_sec / 60), (unsigned long)(pos_sec % 60),
               (unsigned long)(dur_sec / 60), (unsigned long)(dur_sec % 60));
    }
}

// Register shell commands - pcm/audio always available

/*
 * Capture the post-resampler PCM and dump it for offline training.
 *
 * Two steps on purpose.  At 32 kHz the stream is 64 KB/s and the console
 * carries about 11.5 KB/s, so dumping while capturing would stall the decode
 * task and distort the very signal being measured.
 */
static void cmd_pcmcap(int argc, char **argv)
{
    uint32_t len, cap, rate;

    if (argc < 2) {
        printf("Usage: pcmcap <start [sec] | dump | status | free>\n");
        printf("  start 10   arm a 10 s capture, then play audio\n");
        printf("  dump       base64 it to the console (~74 s for 10 s @115200)\n");
        printf("  free       release the buffer\n");
        return;
    }

    if (strcmp(argv[1], "start") == 0) {
        uint32_t sec = (argc >= 3) ? (uint32_t)atoi(argv[2]) : 10u;
        const app_audio_config_t *cfg = app_audio_get_config();
        /* Capture at the sink rate, which is what the model will see. */
        uint32_t r = (cfg && cfg->sample_rate) ? cfg->sample_rate : 32000u;
#ifdef CONFIG_BSP_USING_DAC
        r = 32000u;                    /* DAC path resamples down to 32 kHz */
#endif
        app_pcm_capture_start(sec, r);
        return;
    }
    if (strcmp(argv[1], "dump") == 0)   { app_pcm_capture_dump();  return; }
    if (strcmp(argv[1], "free") == 0)   { app_pcm_capture_start(0, 0); return; }
    if (strcmp(argv[1], "status") == 0) {
        app_pcm_capture_status(&len, &cap, &rate);
        printf("captured %lu / %lu samples (%lu%%) @ %lu Hz\n",
               (unsigned long)len, (unsigned long)cap,
               (unsigned long)(cap ? len * 100u / cap : 0u), (unsigned long)rate);
        return;
    }
    printf("unknown: %s\n", argv[1]);
}


/*
 * Play a log sweep so the acoustic chain can be measured with a microphone.
 *
 * Stop any LE Audio stream first: the IPRO7 FreeRTOS port does not save the
 * float registers across a context switch, so the decoder and this generator
 * must not run at once.
 */
static void cmd_sweep(int argc, char **argv)
{
    uint32_t sec = (argc >= 2) ? (uint32_t)atoi(argv[1]) : 30u;
    uint32_t f1  = (argc >= 3) ? (uint32_t)atoi(argv[2]) : 20u;
    uint32_t f2  = (argc >= 4) ? (uint32_t)atoi(argv[3]) : 15500u;
    /* Default sits just under DAC_KNEE so the path stays linear. */
    int      amp = (argc >= 5) ? atoi(argv[4]) : 5000;

    printf("sweep %lu s, %lu -> %lu Hz, amp %d\n",
           (unsigned long)sec, (unsigned long)f1, (unsigned long)f2, amp);
    printf("gain is forced to 1.0 for the run and restored after.\n");
    printf("stop any stream first so only the sweep is in the air.\n");

    int rc = app_audio_play_sweep(sec, f1, f2, (int16_t)amp);
    if (rc != 0) printf("sweep failed (%d)\n", rc);
}

SHELL_CMD_EXPORT_ALIAS(cmd_pcm, pcm, Audio statistics);
SHELL_CMD_EXPORT_ALIAS(cmd_sweep, sweep, Play a log sweep for acoustic measurement);
SHELL_CMD_EXPORT_ALIAS(cmd_pcmcap, pcmcap, Capture PCM for offline training);
SHELL_CMD_EXPORT_ALIAS(cmd_audio, audio, Audio information);

#if defined(CONFIG_BSP_USING_DAC) && !defined(CONFIG_AUDIO_OUTPUT_UAC)
SHELL_CMD_EXPORT_ALIAS(cmd_dac, dac, DAC control);
SHELL_CMD_EXPORT_ALIAS(cmd_test_tone, test_tone, Play test tone for hardware verification);
SHELL_CMD_EXPORT_ALIAS(cmd_dac_raw, dac_raw, Test DAC with raw reference pattern);
#endif

SHELL_CMD_EXPORT_ALIAS(cmd_vol, vol, Volume control status);
SHELL_CMD_EXPORT_ALIAS(cmd_media, media, Media player status);
