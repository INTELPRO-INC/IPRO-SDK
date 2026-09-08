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
 * @file bt_audio_out.c
 * @brief LC3 decode + rate-matched DAC feed. See bt_audio_out.h for the why.
 */
#include "bt_audio_out.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "lc3.h"

#include "audio.h"
#include <string.h>
#include "audio_output.h"
#include "hal_mtimer.h"
#include "riscv_math.h"
#include "ipro_log.h"

#define LOG_TAG "BTAUD"

/*
 * DAC ring: deeper than audio.c's 4x20 ms, which was sized for locally
 * generated frames with no network jitter in front of them. An ISO stream
 * arrives in bursts, so the ring has to absorb bunching; the reference headset
 * app uses 4x128 ms for the same reason. 4x64 ms is the compromise - 256 ms of
 * slack, and 256 ms of added latency.
 */
#define AO_DAC_RING_COUNT  4
#define AO_DAC_RING_MS     64

/*
 * True DAC rate for hal_dac.c's 32 kHz setting.
 *
 * Do NOT compute this from Clock_Peripheral_Clock_Get(): that applies
 * clock/(div+1), but bench measurement of this DAC says the hardware divides
 * by the written value. Two cross-checked points (div 124 -> 16070 Hz,
 * div 125 -> 15938 Hz, consistent to 0.02%) give
 *
 *     rate = 1992465 / div_written
 *
 * and the 1992465 is itself 0.38% below 32e6/16, so the "32M" XCLK really runs
 * near 31.88 MHz. hal_dac.c writes div 62 for the 32 kHz setting, so:
 *
 *     1992465 / 62 = 32136 Hz
 *
 * That was measured on a DIFFERENT board, and this one disagrees: the DMA
 * segment counter says it drains at 31985 Hz, i.e. -468 ppm from nominal and
 * within crystal tolerance of a plain 32000. Starting from 32136 therefore
 * introduced a +4700 ppm error of its own for the first few seconds of every
 * stream, until the measurement kicked in and corrected it.
 *
 * So the default is nominal now. It is only ever a starting point - the DMA
 * segment count measures the truth a few seconds in and every stream after
 * that uses the measured value. Do not put a "clever" constant here again:
 * whatever sets XCLK differs between boards, and the measurement settles it.
 */
#define AO_DAC_DEFAULT_HZ  BT_AO_DAC_RATE

/*
 * Shortest window whose segment quantisation is smaller than the drift worth
 * correcting. 20 s puts it near 3200 ppm and it keeps improving from there.
 */
#define AO_MEAS_MIN_US     20000000u

/*
 * Below this the rate matcher is bypassed and PCM passes through untouched.
 *
 * Linear interpolation at a near-unity ratio is not harmless. Its response
 * depends on the fractional phase: at phase 0 it is flat, at phase 0.5 it is
 * the average of two neighbours, which is -3 dB at fs/2. The phase walks
 * slowly with the drift, so the top octave breathes in and out at the rate the
 * phase wraps - at 125 ppm that is a wrap every 0.25 s, i.e. an audible ~4 Hz
 * warble. Continuous warble is far worse than the drift it corrects: at
 * 1000 ppm the DAC ring takes over four minutes to walk its own depth.
 *
 * So do nothing until the error is big enough to be worth an artefact.
 */
#define AO_RATE_BYPASS_PPM 1000

#define AO_TASK_STACK      2048   /* words, from the PSRAM heap */

/*
 * Same priority as audio.c's worker, which this task replaces for the duration
 * of a stream - they are mutually exclusive anyway. An absolute number here
 * would sit below the shell and starve.
 */
#define AO_TASK_PRIO       (configMAX_PRIORITIES - 4)

/*
 * Anti-alias low-pass, run before the rate matcher whenever the stream rate is
 * above the DAC's. Only 48 kHz reaches that: the DAC drains near 32136 Hz, so
 * its Nyquist is ~16068 Hz and everything between there and 24 kHz would fold
 * straight back into the audible band. Linear interpolation is not a filter -
 * it barely attenuates the fold-back at all.
 *
 * 104-tap Hamming windowed sinc, fc 14.5 kHz at 48 kHz. Measured response:
 * flat to 13 kHz, -6 dB at 14.5 kHz, -71 dB at 16068 Hz, >= -60 dB beyond.
 * Coefficients are symmetric, so the time-reversed order CMSIS wants is the
 * same array. Sum is exactly 32768 for unity DC gain after q15 rounding.
 *
 * q15 rather than f32 deliberately: the q15 path uses the DSP extension's
 * integer SIMD, so it never touches f0-f31 and stays clear of the missing FPU
 * context save described in the header.
 */
#define AO_FIR_TAPS   104u
#define AO_FIR_FS     48000u   /* the rate the taps were designed for */
#define AO_DAC_FIR_FS_OK AO_FIR_FS

static const q15_t ao_fir_coeff[AO_FIR_TAPS] = {
        -6,     17,     -5,    -15,     17,      7,    -25,     10,
        25,    -31,     -9,     46,    -22,    -43,     58,     12,
       -82,     44,     70,   -104,    -12,    136,    -81,   -108,
       174,      7,   -214,    143,    156,   -280,     10,    326,
      -241,   -219,    442,    -49,   -496,    409,    310,   -718,
       132,    796,   -741,   -476,   1318,   -351,  -1569,   1761,
      1039,  -4147,   2014,  16950,  16948,   2014,  -4147,   1039,
      1761,  -1569,   -351,   1318,   -476,   -741,    796,    132,
      -718,    310,    409,   -496,    -49,    442,   -219,   -241,
       326,     10,   -280,    156,    143,   -214,      7,    174,
      -108,    -81,    136,    -12,   -104,     70,     44,    -82,
        12,     58,    -43,    -22,     46,     -9,    -31,     25,
        10,    -25,      7,     17,    -15,     -5,     17,     -6
};

typedef struct {
    uint16_t len;                      /* 0 = lost frame, decode as PLC */
    uint8_t  data[BT_AO_MAX_OCTETS];
} ao_frame_t;

/*
 * The ring lives on the PSRAM heap, not in .bss: OCRAM is at 93% and a ring
 * deep enough to cover one DAC segment would not fit.
 */
static ao_frame_t        *s_ring;
static volatile uint32_t  s_head;      /* producer: BT RX thread   */
static volatile uint32_t  s_tail;      /* consumer: decode task    */

static SemaphoreHandle_t     s_sem;
static volatile TaskHandle_t s_task;
static volatile bool         s_run;

static lc3_decoder_t      s_dec;
static void              *s_dec_mem;
static int16_t           *s_pcm;        /* decoder output, s_pcm_samples  */
static uint32_t           s_pcm_samples;
static int16_t           *s_filt;       /* anti-aliased, s_pcm_samples    */
static riscv_fir_instance_q15 s_fir;
static q15_t             *s_fir_state;
static bool               s_fir_on;
static int16_t           *s_rs;         /* rate-matched output            */
static uint32_t           s_rs_cap;
static audio_output_t    *s_out;

/* Rate matcher: Q16 phase, one sample of history across frame boundaries. */
static uint32_t  s_rs_step;
static uint32_t  s_rs_phase;
static int16_t   s_rs_prev;
static bool      s_rs_bypass;

static uint32_t  s_dac_rate_override;
static uint32_t  s_dac_rate_learned;   /* measured by a previous stream */

/*
 * Sink selection.  The DAC path is unchanged; UAC swaps in a different nominal
 * rate and a different learned value, because "what the sink really drains at"
 * is a property of that sink's clock - the DAC's divider chain in one case,
 * the host's SOF in the other - and they must not contaminate each other.
 */
static bt_ao_sink_t s_sink = BT_AO_SINK_DAC;
static uint32_t  s_uac_rate_learned;
static uint32_t  s_sink_nominal = BT_AO_DAC_RATE;  /* resolved at start() */

/* Drain-rate measurement, from DMA segment completions. */
static uint64_t  s_t0_us;
static uint32_t  s_consumed0;
static uint64_t  s_produced;

/* Optional capture of decoded PCM, for the replay test - see audio.c. */
static int16_t  *s_cap;
static uint32_t  s_cap_size;
static uint32_t  s_cap_len;

static bt_audio_out_status_t s_st;

/* ------------------------------------------------------------------------- */

/**
 * The rate to match to: an explicit override first, then whatever a previous
 * stream measured on this board, then the bench constant above.
 *
 * Preferring the measured value makes this self-correcting - if the constant
 * is wrong for a particular part, the first stream is enough to find out and
 * every stream after it is right.
 */
static uint32_t ao_true_dac_rate(void)
{
    if (s_sink == BT_AO_SINK_UAC) {
        /* No hand override for USB: there is no divider to second-guess, only
         * the host's SOF, and the drain measurement settles that on its own. */
        return s_uac_rate_learned ? s_uac_rate_learned : BT_AO_UAC_RATE;
    }
    if (s_dac_rate_override) {
        return s_dac_rate_override;
    }
    if (s_dac_rate_learned) {
        return s_dac_rate_learned;
    }
    return AO_DAC_DEFAULT_HZ;
}

int bt_audio_out_set_sink(bt_ao_sink_t sink)
{
    if (sink != BT_AO_SINK_DAC && sink != BT_AO_SINK_UAC) {
        return -1;
    }
    if (s_st.running) {
        IPRO_LOGE(LOG_TAG, "sink is fixed while a stream runs - stop it first");
        return -1;
    }
    s_sink = sink;
    return 0;
}

bt_ao_sink_t bt_audio_out_get_sink(void)
{
    return s_sink;
}

/**
 * Linear resample from the decoder rate to the DAC's true rate.
 *
 * The phase accumulator and one sample of history are static, so a frame
 * continues exactly where the previous one stopped. Resampling each frame from
 * phase zero would quantise the output count to a whole sample every 10 ms -
 * both a residual rate error and an audible discontinuity at every boundary.
 *
 * @return number of samples written to @p out
 */
static uint32_t ao_resample(const int16_t *in, uint32_t n, int16_t *out)
{
    uint32_t produced = 0;
    uint32_t p        = s_rs_phase;

    /* Virtual input is [prev, in[0] .. in[n-1]]; p indexes it in Q16. */
    while ((p >> 16) < n) {
        uint32_t i = p >> 16;
        uint32_t f = p & 0xFFFFu;
        int32_t  a = (i == 0u) ? s_rs_prev : in[i - 1u];
        int32_t  b = in[i];

        out[produced++] = (int16_t)(a + (((b - a) * (int32_t)f) >> 16));
        p += s_rs_step;
    }

    s_rs_phase = p - (n << 16);
    s_rs_prev  = in[n - 1u];
    return produced;
}

static void ao_wait_idle(void);

/* Set the matcher ratio, and decide whether it is worth running at all. */
static void ao_set_rate(uint32_t out_rate, uint32_t dac_rate)
{
    int64_t ppm = ((int64_t)dac_rate - out_rate) * 1000000 / (int64_t)out_rate;

    s_rs_step   = (uint32_t)(((uint64_t)out_rate << 16) / dac_rate);
    s_rs_bypass = (ppm > -AO_RATE_BYPASS_PPM && ppm < AO_RATE_BYPASS_PPM);
}

static void ao_free(void)
{
    if (s_out) {
        if (s_st.sink == BT_AO_SINK_UAC) {
            audio_output_stop(s_out);
            audio_output_destroy(s_out);
        } else {
            audio_dac_close(s_out);
        }
        s_out = NULL;
    }
    if (s_rs) {
        vPortFree(s_rs);
        s_rs = NULL;
    }
    if (s_fir_state) {
        vPortFree(s_fir_state);
        s_fir_state = NULL;
    }
    if (s_filt) {
        vPortFree(s_filt);
        s_filt = NULL;
    }
    s_fir_on = false;
    if (s_pcm) {
        vPortFree(s_pcm);
        s_pcm = NULL;
    }
    if (s_dec_mem) {
        vPortFree(s_dec_mem);
        s_dec_mem = NULL;
    }
    if (s_ring) {
        vPortFree(s_ring);
        s_ring = NULL;
    }
    s_dec = NULL;
    s_pcm_samples = 0;
    s_rs_cap = 0;
}

static int ao_peak(const int16_t *pcm, uint32_t n)
{
    int peak = 0;

    for (uint32_t i = 0; i < n; i++) {
        int v = pcm[i] < 0 ? -pcm[i] : pcm[i];
        if (v > peak) {
            peak = v;
        }
    }
    return peak;
}

static void ao_task(void *arg)
{
    (void)arg;

    while (s_run) {
        if (xSemaphoreTake(s_sem, pdMS_TO_TICKS(200)) != pdTRUE) {
            continue;
        }

        while (s_tail != s_head && s_run) {
            ao_frame_t *f = &s_ring[s_tail % BT_AO_RING_FRAMES];
            bool lost = (f->len == 0u);
            const int16_t *out;
            uint32_t n;
            int ret;

            ret = lc3_decode(s_dec,
                             lost ? NULL : f->data,
                             lost ? 0    : (int)f->len,
                             LC3_PCM_FORMAT_S16, s_pcm, 1);
            s_tail++;

            if (ret < 0) {
                s_st.errors++;
                continue;
            }
            /*
             * Two different failures that used to be counted as one. "lost"
             * means the SDU never arrived intact - a link problem. ret > 0
             * means the SDU arrived but liblc3 rejected its contents and
             * concealed it itself - a payload problem, e.g. a length that does
             * not match the negotiated octets_per_frame. They point at
             * completely different places, so keep them apart.
             */
            if (lost) {
                s_st.plc++;
            } else if (ret > 0) {
                s_st.dec_plc++;
            }
            s_st.decoded++;
            {
                /*
                 * Running max, not the last frame. The last frame is usually
                 * the tail of a fade-out, so reporting it says nothing about
                 * whether the stream carried any signal at all.
                 */
                int pk = ao_peak(s_pcm, s_pcm_samples);

                if (pk > s_st.peak) {
                    s_st.peak = pk;
                }
            }

            {
                const int16_t *src = s_pcm;

                if (s_fir_on) {
                    riscv_fir_q15(&s_fir, s_pcm, s_filt, s_pcm_samples);
                    src = s_filt;
                }
                if (s_rs_bypass) {
                    out = src;                 /* untouched - see the note */
                    n   = s_pcm_samples;
                } else {
                    n   = ao_resample(src, s_pcm_samples, s_rs);
                    out = s_rs;
                }
            }

            /*
             * Blocks until the DAC frees a ring slot. That block is what paces
             * this task, so it is also the measurement: samples handed over,
             * divided by elapsed time, is the rate the hardware really drains
             * at. The first write is excluded - the ring starts empty and the
             * first few writes do not block.
             */
            if (s_cap && s_cap_len < s_cap_size) {
                /* Pre-gain, so the capture is the decoder's own output. */
                uint32_t room = s_cap_size - s_cap_len;
                uint32_t take = (n < room) ? n : room;

                memcpy(&s_cap[s_cap_len], out, take * sizeof(int16_t));
                s_cap_len += take;
                if (s_cap_len >= s_cap_size) {
                    audio_capture_commit(s_cap_len);
                    s_cap = NULL;
                    IPRO_LOGI(LOG_TAG, "capture full (%u samples) - stop the "
                                       "stream, then `mic play` to hear the "
                                       "decoder output through the DAC path a "
                                       "tone already proves clean",
                              (unsigned)s_cap_len);
                }
            }

            /* Gain into s_rs either way, so the bypass never writes to s_pcm. */
            audio_gain_apply(out, s_rs, n);    /* `spk gain` applies here too */
            audio_output_write(s_out, s_rs, n);

            /*
             * Measure what the DAC really drains at, and correct to it.
             *
             * The count comes from DMA segment completions, not from how fast
             * this task can push: audio_output_write() blocks only when the
             * ring is full, so a producer running slow never blocks and would
             * just measure itself. Segment completions are independent of us,
             * which is the whole point - it is the one number that says
             * whether the assumed rate is right, and in which direction.
             *
             * Applied straight away rather than through a control loop. This
             * is a direct measurement, so there is nothing to converge and no
             * loop to go unstable; the phase accumulator carries on smoothly
             * across the change.
             */
            {
                uint64_t now      = mtimer_get_time_us();
                uint32_t consumed = audio_output_consumed(s_out);

                s_produced += n;

                if (s_t0_us == 0u) {
                    /*
                     * Open the window only once the sink is actually draining.
                     *
                     * The DAC starts on audio_output_start(), so its first
                     * write and its first drain coincide.  UAC does not: the
                     * DMA waits for the host to open the streaming interface,
                     * which can be hundreds of ms after PCM starts arriving -
                     * or never.  Latching t0 on the first write therefore put
                     * a stretch of zero drain inside the window and reported
                     * the sink as running slow by exactly that fraction
                     * (-13854 ppm for a ~530 ms late start over 38 s).  That
                     * is inside the +-3.125% accept band, so it was believed,
                     * and the matcher left bypass to "correct" a sink that was
                     * in fact perfectly on rate.
                     */
                    if (consumed != 0u) {
                        s_t0_us     = now;
                        s_consumed0 = consumed;
                    }
                } else {
                    uint64_t dt = now - s_t0_us;

                    /*
                     * Cumulative window, never restarted.
                     *
                     * consumed only advances a whole DAC segment at a time, so
                     * a short window is dominated by that quantisation: over
                     * 3 s a single 2048-sample segment is 683 samples/s, which
                     * is 21000 ppm - an order of magnitude larger than the
                     * drift being corrected. Re-deriving the rate from a rolling
                     * 3 s window and writing it straight back into s_rs_step,
                     * as this did at first, just jittered the resampling ratio
                     * by +-2% every three seconds.
                     *
                     * Letting the window grow makes the error decay as 1/T:
                     * ~6400 ppm at 10 s, ~2000 ppm at 30 s, ~1000 ppm at 60 s.
                     * The correction is only applied once the window is long
                     * enough to beat the drift it is meant to remove, and it is
                     * clamped, because a wrong correction is worse than none.
                     */
                    if (dt > AO_MEAS_MIN_US) {
                        uint32_t d    = consumed - s_consumed0;
                        uint32_t rate = (uint32_t)(((uint64_t)d * 1000000u) / dt);
                        /*
                         * How far the sink is allowed to be off nominal before
                         * the reading is treated as a measurement fault.
                         *
                         * The DAC really is ~0.8% off - its divider chain is
                         * the whole reason this loop exists - so it needs the
                         * wide band.  USB SOF is specified to +-500 ppm, so
                         * anything past ~0.2% on the UAC sink is a bug in the
                         * measurement, not a clock.  Believing one cost a
                         * starved ring and a stream that went silent 20 s in.
                         */
                        uint32_t band = (s_sink == BT_AO_SINK_UAC)
                                      ? s_sink_nominal / 512u    /* ~1950 ppm */
                                      : s_sink_nominal / 32u;    /* ~3.1%    */
                        uint32_t lo   = s_sink_nominal - band;
                        uint32_t hi   = s_sink_nominal + band;

                        /*
                         * consumed only advances a segment at a time, so this
                         * window can only resolve one segment/elapsed
                         * Hz. Acting on a difference smaller than that is
                         * acting on quantisation noise - and the correction
                         * costs an audible artefact, so it has to be earned.
                         */
                        uint32_t res_hz = (uint32_t)
                            (((uint64_t)audio_output_segment_samples(s_out)
                              * 1000000u) / dt);
                        /*
                         * Against the nominal DAC rate, not out_rate. out_rate
                         * is the decoder's PCM rate - 48000 for a 48 kHz
                         * stream - so comparing against it made the difference
                         * ~16000 Hz and the guard passed unconditionally. It
                         * only ever worked at 16 kHz, where out_rate happens to
                         * equal the DAC rate.
                         */
                        uint32_t diff = (rate > s_sink_nominal)
                                      ? rate - s_sink_nominal
                                      : s_sink_nominal - rate;

                        s_st.measured_rate = (rate > lo && rate < hi) ? rate : 0;

                        if (s_st.measured_rate && diff > res_hz) {
                            if (s_sink == BT_AO_SINK_UAC) {
                                s_uac_rate_learned = rate;
                            } else {
                                s_dac_rate_learned = rate;
                            }
                            s_st.dac_rate      = rate;
                            ao_set_rate(s_st.out_rate, rate);
                        }
                        /* window keeps growing - s_t0_us/s_consumed0 stay put */
                    }
                }
            }
        }
    }

    /*
     * The task owns teardown. bt_audio_out_stop() is called from BT callbacks
     * that must not block - blocking the host's RX thread stalls HCI event
     * processing, which surfaces as "Unable to look up conn" and friends - so
     * it only signals, and the freeing happens here.
     */
    IPRO_LOGI(LOG_TAG, "playback stopped: %u decoded, %u lost SDUs, %u decoder "
                       "PLC, %u dropped, %u errors, peak %d/32767",
              (unsigned)s_st.decoded, (unsigned)s_st.plc,
              (unsigned)s_st.dec_plc, (unsigned)s_st.dropped,
              (unsigned)s_st.errors, s_st.peak);
    if (s_st.measured_rate) {
        IPRO_LOGI(LOG_TAG, "%s drained at %u Hz (nominal %u, %+d ppm) "
                           "- this is the measured truth",
                  (s_st.sink == BT_AO_SINK_UAC) ? "UAC" : "DAC",
                  (unsigned)s_st.measured_rate, (unsigned)s_sink_nominal,
                  (int)(((int64_t)s_st.measured_rate - s_sink_nominal)
                        * 1000000 / s_sink_nominal));
    } else {
        IPRO_LOGW(LOG_TAG, "stream too short to measure the sink rate");
    }

    ao_free();
    s_task = NULL;
    vTaskDelete(NULL);
}

/* ------------------------------------------------------------------------- */

int bt_audio_out_start(uint32_t sample_rate, uint32_t frame_us, uint32_t octets)
{
    uint32_t out_rate;
    uint32_t dac_rate;
    int      sr_pcm;
    unsigned mem_size;
    int      samples;

    if (frame_us != 7500u && frame_us != 10000u) {
        IPRO_LOGE(LOG_TAG, "unsupported frame duration %u us", (unsigned)frame_us);
        return -1;
    }
    if (sample_rate < 8000u || sample_rate > 48000u) {
        IPRO_LOGE(LOG_TAG, "unsupported sample rate %u Hz", (unsigned)sample_rate);
        return -1;
    }
    if (octets == 0u || octets > BT_AO_MAX_OCTETS) {
        IPRO_LOGE(LOG_TAG, "unsupported frame size %u octets", (unsigned)octets);
        return -1;
    }

    bt_audio_out_stop();
    ao_wait_idle();

    /*
     * Only one task may do float work at a time on this port - see the FPU
     * note in the header. audio.c's worker is the other candidate, so idle it.
     */
    audio_stop();

    /*
     * Start at unity. The default make-up gain is +30 dB, which exists because
     * the PDM microphone path is very quiet; LE Audio arrives near full scale,
     * so the same setting would clip it into a wall. `spk gain <db>` adjusts
     * from here and now reaches this path - it used to be reported by
     * `bt audio` while having no effect on it at all.
     */
    audio_set_gain(0);

    /*
     * Everything below derives from the sink, so pin it first.  At 48 kHz on
     * UAC out_rate lands exactly on the nominal rate, which is what keeps the
     * FIR and the matcher out of the chain for the common phone configuration.
     */
    s_sink_nominal = (s_sink == BT_AO_SINK_UAC) ? BT_AO_UAC_RATE : BT_AO_DAC_RATE;

    out_rate = (sample_rate < s_sink_nominal) ? s_sink_nominal : sample_rate;
    sr_pcm   = (out_rate == sample_rate) ? 0 : (int)out_rate;
    dac_rate = ao_true_dac_rate();

    samples = lc3_frame_samples((int)frame_us, (int)out_rate);
    if (samples <= 0) {
        IPRO_LOGE(LOG_TAG, "lc3_frame_samples(%u, %u) rejected",
                  (unsigned)frame_us, (unsigned)out_rate);
        return -1;
    }
    s_pcm_samples = (uint32_t)samples;

    /* Worst case the matcher upsamples; twice the frame covers any sane ratio. */
    s_rs_cap  = s_pcm_samples * 2u + 4u;

    mem_size  = lc3_decoder_size((int)frame_us, (int)out_rate);
    s_dec_mem = pvPortMalloc(mem_size);
    s_pcm     = pvPortMalloc(s_pcm_samples * sizeof(int16_t));
    s_rs      = pvPortMalloc(s_rs_cap * sizeof(int16_t));
    s_ring    = pvPortMalloc(BT_AO_RING_FRAMES * sizeof(ao_frame_t));
    if (!s_dec_mem || !s_pcm || !s_rs || !s_ring) {
        IPRO_LOGE(LOG_TAG, "out of memory (decoder %u B)", mem_size);
        ao_free();
        return -2;
    }

    /*
     * Decimating (a 48 kHz stream on a 32 kHz DAC) needs the anti-alias filter
     * in front. Upsampling or near-1:1 does not - liblc3 already band-limits
     * its own output, and there is nothing above the output Nyquist to fold.
     */
    if (out_rate > dac_rate) {
        uint32_t state_len = AO_FIR_TAPS + s_pcm_samples - 1u;

        if (out_rate != AO_DAC_FIR_FS_OK) {
            IPRO_LOGW(LOG_TAG, "no anti-alias taps for %u Hz - designed for %u",
                      (unsigned)out_rate, AO_FIR_FS);
        }
        s_filt      = pvPortMalloc(s_pcm_samples * sizeof(int16_t));
        s_fir_state = pvPortMalloc(state_len * sizeof(q15_t));
        if (!s_filt || !s_fir_state) {
            IPRO_LOGE(LOG_TAG, "anti-alias filter out of memory");
            ao_free();
            return -2;
        }
        memset(s_fir_state, 0, state_len * sizeof(q15_t));
        riscv_fir_init_q15(&s_fir, AO_FIR_TAPS, (q15_t *)ao_fir_coeff,
                           s_fir_state, s_pcm_samples);
        s_fir_on = true;
    }

    s_dec = lc3_setup_decoder((int)frame_us, (int)sample_rate, sr_pcm, s_dec_mem);
    if (!s_dec) {
        IPRO_LOGE(LOG_TAG, "lc3_setup_decoder(%u us, %u Hz, out %u Hz) failed",
                  (unsigned)frame_us, (unsigned)sample_rate, (unsigned)out_rate);
        ao_free();
        return -3;
    }

    if (!s_sem) {
        s_sem = xSemaphoreCreateBinary();
        if (!s_sem) {
            ao_free();
            return -2;
        }
    }

    /*
     * Opened at the nominal rate so the component's own resampler stays out of
     * the way - it would restart its phase on every call. The rate match is
     * done above, in ao_resample().
     */
    if (s_sink == BT_AO_SINK_UAC) {
        audio_output_cfg_t ucfg = {
            .type        = AUDIO_OUTPUT_UAC,
            .mode        = AUDIO_OUTPUT_MODE_STREAMING,
            .sample_rate = BT_AO_UAC_RATE,
            .channels    = 1,
        };

        s_out = audio_output_create(&ucfg);
        if (s_out && audio_output_start(s_out) != 0) {
            audio_output_destroy(s_out);
            s_out = NULL;
        }
        if (!s_out) {
            IPRO_LOGE(LOG_TAG, "UAC open failed");
            ao_free();
            return -4;
        }
    } else {
        s_out = audio_dac_open_ex(BT_AO_DAC_RATE, AO_DAC_RING_COUNT, AO_DAC_RING_MS);
        if (!s_out) {
            IPRO_LOGE(LOG_TAG, "DAC open failed");
            ao_free();
            return -4;
        }
    }

    s_head = 0;
    s_tail = 0;
    ao_set_rate(out_rate, dac_rate);
    s_rs_phase = 0;
    s_rs_prev  = 0;
    s_t0_us     = 0;
    s_consumed0 = 0;
    s_produced  = 0;

    memset(&s_st, 0, sizeof(s_st));
    s_st.sample_rate = sample_rate;
    s_st.frame_us    = frame_us;
    s_st.octets      = octets;
    s_st.out_rate    = out_rate;
    s_st.dac_rate    = dac_rate;
    s_st.sink        = s_sink;

    s_run = true;
    if (xTaskCreate(ao_task, "bt_audio", AO_TASK_STACK, NULL,
                    AO_TASK_PRIO, (TaskHandle_t *)&s_task) != pdPASS) {
        IPRO_LOGE(LOG_TAG, "decode task create failed");
        s_run = false;
        ao_free();
        return -2;
    }

    s_st.running = true;
    IPRO_LOGI(LOG_TAG, "LC3 %u Hz / %u us / %u octets -> %u Hz PCM%s",
              (unsigned)sample_rate, (unsigned)frame_us, (unsigned)octets,
              (unsigned)out_rate, sr_pcm ? " (upsampled in the decoder)" : "");
    if (s_fir_on) {
        IPRO_LOGI(LOG_TAG, "anti-alias FIR on (%u taps, -71 dB at %u Hz)",
                  (unsigned)AO_FIR_TAPS, (unsigned)(dac_rate / 2u));
    }
    IPRO_LOGI(LOG_TAG, "sink %s, rate match %u -> %u Hz %s(%s, %+d ppm off nominal)",
              (s_sink == BT_AO_SINK_UAC) ? "UAC" : "DAC",
              (unsigned)out_rate, (unsigned)dac_rate,
              s_rs_bypass ? "- BYPASSED, passthrough " : "",
              (s_sink == BT_AO_SINK_UAC)
                  ? (s_uac_rate_learned ? "measured on this host" : "SOF nominal")
                  : s_dac_rate_override ? "set by hand" :
                    s_dac_rate_learned  ? "measured on this board" : "bench default",
              (int)(((int64_t)dac_rate - s_sink_nominal) * 1000000 / s_sink_nominal));
    return 0;
}

/**
 * Signal the decode task to stop. Returns immediately - see the note in
 * ao_task() about why this must never block.
 */
void bt_audio_out_stop(void)
{
    s_run = false;
    if (s_sem) {
        xSemaphoreGive(s_sem);          /* break the 200 ms wait early */
    }
    s_st.running = false;
}

/** Wait for a previous instance to be gone, so start() can reuse the statics. */
static void ao_wait_idle(void)
{
    int wait_ms = 0;

    /*
     * The task exits within one DAC segment - that is the longest it can be
     * blocked inside audio_output_write() - so this is tens of milliseconds in
     * practice. It runs on a BT callback, so the bound stays short.
     */
    while (s_task && wait_ms < 400) {
        vTaskDelay(pdMS_TO_TICKS(10));
        wait_ms += 10;
    }
    if (s_task) {
        IPRO_LOGE(LOG_TAG, "previous decode task still alive after %d ms",
                  wait_ms);
    }
}

void bt_audio_out_push(const uint8_t *data, uint16_t len)
{
    ao_frame_t *f;

    if (!s_run || !s_ring) {
        return;
    }
    if ((s_head - s_tail) >= BT_AO_RING_FRAMES) {
        s_st.dropped++;                 /* decoder is behind; keep the oldest */
        return;
    }

    f = &s_ring[s_head % BT_AO_RING_FRAMES];
    if (data != NULL && len > 0u) {
        if (len > BT_AO_MAX_OCTETS) {
            len = BT_AO_MAX_OCTETS;
        }
        memcpy(f->data, data, len);
        f->len = len;
    } else {
        f->len = 0u;                    /* conceal this one */
    }

    __asm volatile ("" ::: "memory");   /* publish the payload before the index */
    s_head++;

    xSemaphoreGive(s_sem);
}

int bt_audio_out_capture(uint32_t seconds)
{
    uint32_t cap = 0;
    int16_t *buf;

    if (!s_run) {
        return -1;
    }
    buf = audio_capture_buffer(&cap);
    if (!buf) {
        return -2;
    }

    s_cap_len  = 0;
    s_cap_size = seconds * BT_AO_DAC_RATE;
    if (s_cap_size > cap) {
        s_cap_size = cap;
    }
    s_cap = buf;                      /* published last - the task reads it */
    return (int)(s_cap_size / BT_AO_DAC_RATE);
}

void bt_audio_out_capture_stop(void)
{
    if (s_cap) {
        s_cap = NULL;
        audio_capture_commit(s_cap_len);
    }
}

void bt_audio_out_set_dac_rate(uint32_t hz)
{
    s_dac_rate_override = hz;
}

bool bt_audio_out_running(void)
{
    return s_run;
}

void bt_audio_out_get_status(bt_audio_out_status_t *out)
{
    if (out) {
        *out = s_st;
        out->running = s_run;
        if (!out->dac_rate) {
            out->dac_rate = ao_true_dac_rate();
        }
    }
}
