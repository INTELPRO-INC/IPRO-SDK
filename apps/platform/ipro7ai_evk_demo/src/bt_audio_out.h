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
 * @file bt_audio_out.h
 * @brief LE Audio sink playback: LC3 frames from the ISO channel -> DAC.
 *
 * The BAP sink delivers one LC3 frame per SDU on the BT RX thread. That thread
 * must not block, so recv() only copies the frame into a small ring and wakes a
 * decode task; the task does the LC3 work and hands PCM to the audio_output
 * component, which owns the DAC.
 *
 * liblc3 upsamples during decode (the sr_pcm_hz argument to lc3_setup_decoder),
 * so a 16 or 24 kHz stream becomes BT_AO_DAC_RATE PCM inside the decoder.
 *
 * RATE MATCHING - why there is a resampler here at all
 * ----------------------------------------------------
 * The DAC does not run at 32 kHz. hal_dac.c programs the GPDAC clock as
 * XCLK/(62+1) and then divides by 16, so asking for 32000 actually yields
 * about 31746 Hz - roughly 8000 ppm slow. Every layer above it, including
 * audio_output_dac.c's DAC_RATE constant, assumes the 32000 it asked for.
 *
 * Feeding 32000 samples per second into a sink that consumes 31746 leaves a
 * surplus of ~254 samples every second, which is a whole 10 ms frame every
 * 1.3 s. The frame has to go somewhere, and what comes out is a periodic
 * glitch. Deeper buffers do not fix this - they only change how often it
 * happens, because the error accumulates without bound.
 *
 * So the true rate is read back from the clock tree at start-up and the
 * decoded PCM is resampled 32000 -> true rate before it reaches the DAC. The
 * phase accumulator persists across frames; resampling each frame
 * independently would restart the phase every 10 ms and click.
 *
 * FPU HAZARD - read before adding a second user
 * ---------------------------------------------
 * The IPRO7 FreeRTOS port does not save the float registers f0-f31 across a
 * context switch, yet everything is built hard-float (ilp32f). liblc3 is
 * float-heavy, so this decode task must be the only task doing float work
 * while it runs. bt_audio_out_start() therefore stops the audio.c worker
 * first, and the camera (whose 3A is also float) must not be streaming.
 * Two float users running concurrently corrupt each other silently - there is
 * no fault, just wrong numbers.
 */
#ifndef BT_AUDIO_OUT_H
#define BT_AUDIO_OUT_H

#include <stdint.h>
#include <stdbool.h>

/** DAC native rate - matches AUDIO_TONE_RATE in audio_port.h. */
#define BT_AO_DAC_RATE     32000u

/*
 * UAC endpoint rate.
 *
 * 48 kHz is what phones negotiate for LE Audio media, so at this rate the
 * decoder's PCM reaches USB with no rate conversion at all - out_rate equals
 * the sink's nominal rate, which puts the matcher in bypass and leaves the
 * anti-alias FIR out of the chain.  The residual BLE-vs-SOF drift is still
 * measured and corrected the same way the DAC path does it.
 */
#define BT_AO_UAC_RATE     48000u

/** Where decoded PCM goes. Select before bt_audio_out_start(). */
typedef enum {
    BT_AO_SINK_DAC = 0,   /**< on-chip DAC -> speaker (default) */
    BT_AO_SINK_UAC,       /**< USB Audio Class -> host records/plays it */
} bt_ao_sink_t;

/**
 * @brief Choose the sink for the next stream.
 *
 * Rejected while a stream is running - the sink is opened in
 * bt_audio_out_start() and the rate matcher is configured against it.
 *
 * @return 0 on success, -1 if a stream is running or the sink is unknown
 */
int bt_audio_out_set_sink(bt_ao_sink_t sink);

/** @brief Sink that the next (or current) stream uses. */
bt_ao_sink_t bt_audio_out_get_sink(void);

/** Largest LC3 frame LE Audio defines (48_6 = 155 octets). */
#define BT_AO_MAX_OCTETS   160u

/*
 * Frames held between the ISO thread and the decoder. Has to cover one DAC
 * segment, because that is how long the decode task can sit blocked in
 * audio_output_write() while frames keep arriving. At 64 ms segments and
 * 7.5 ms frames that is ~9; 32 leaves room and costs 5 KB of PSRAM heap.
 */
#define BT_AO_RING_FRAMES  32u

/** Runtime counters, all monotonic while a stream is up. */
typedef struct {
    bool     running;        /* decode task alive and DAC open      */
    uint32_t sample_rate;    /* negotiated LC3 rate, Hz             */
    uint32_t frame_us;       /* negotiated frame duration           */
    uint32_t octets;         /* negotiated octets per frame         */
    uint32_t out_rate;       /* decoder PCM rate, before rate match  */
    uint32_t dac_rate;       /* true sink rate being matched to      */
    bt_ao_sink_t sink;       /* where the PCM is going               */
    uint32_t measured_rate;  /* rate the DAC actually drained at     */
    uint32_t decoded;        /* frames decoded to PCM               */
    uint32_t plc;            /* concealed because the SDU was lost  */
    uint32_t dec_plc;        /* concealed by the decoder's own call  */
    uint32_t errors;         /* lc3_decode() rejections             */
    uint32_t dropped;        /* frames the ring could not hold      */
    int      peak;           /* |sample| peak of the last frame     */
} bt_audio_out_status_t;

/**
 * @brief Bring up the decoder and the DAC for a negotiated configuration.
 *
 * Safe to call again with different parameters; the previous instance is torn
 * down first. Stops audio.c's worker - see the FPU note above.
 *
 * @param sample_rate  LC3 sample rate in Hz (8000..48000)
 * @param frame_us     frame duration in us (7500 or 10000)
 * @param octets       octets per LC3 frame
 * @return 0 on success, negative on error
 */
int bt_audio_out_start(uint32_t sample_rate, uint32_t frame_us, uint32_t octets);

/** @brief Tear down the decoder and release the DAC. Idempotent. */
void bt_audio_out_stop(void);

/**
 * @brief Hand one received SDU to the decoder. Never blocks.
 *
 * @param data  LC3 payload, or NULL for a lost frame (concealed)
 * @param len   payload length, 0 for a lost frame
 */
void bt_audio_out_push(const uint8_t *data, uint16_t len);

/**
 * @brief Override the assumed hardware DAC rate, in Hz. 0 restores the value
 *        read from the clock tree. Takes effect on the next stream.
 *
 * The computed value should be right, but it depends on XCLK being exactly
 * what the clock tree reports. `bt audio` prints the rate the DAC actually
 * drained at once a stream has been running; if that disagrees, set it here.
 */
void bt_audio_out_set_dac_rate(uint32_t hz);

/**
 * @brief Capture decoded PCM into the PSRAM record buffer for @p seconds.
 *
 * Replay it afterwards with `mic play`, which uses the same DAC path a
 * synthesised tone does. A tone comes out clean, so a clean replay says the
 * decoder output was always fine and the fault is in real-time delivery; a
 * rough replay says the fault is upstream of the DAC. Nothing else separates
 * those two by ear.
 *
 * @return seconds actually reserved, or negative on error
 */
int  bt_audio_out_capture(uint32_t seconds);
void bt_audio_out_capture_stop(void);

/** @brief True once the decode path is up. */
bool bt_audio_out_running(void);

void bt_audio_out_get_status(bt_audio_out_status_t *out);

#endif /* BT_AUDIO_OUT_H */
