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
 * @file audio_resampler.c
 * @brief Audio sample rate converter implementation
 *
 * FIR polyphase resampling (CONFIG_NMSIS_DSP) with linear interpolation fallback.
 */

#include "audio_resampler.h"
#include "ipro_log.h"

#include <string.h>

#ifdef CONFIG_NMSIS_DSP
#include "riscv_math.h"
#include "dsp/filtering_functions.h"

/* Compile-time size checks for opaque DSP instance storage */
_Static_assert(sizeof(riscv_fir_interpolate_instance_f32) <= AUDIO_RESAMPLER_DSP_INST_SIZE,
               "AUDIO_RESAMPLER_DSP_INST_SIZE too small for interpolate instance");
_Static_assert(sizeof(riscv_fir_decimate_instance_f32) <= AUDIO_RESAMPLER_DSP_INST_SIZE,
               "AUDIO_RESAMPLER_DSP_INST_SIZE too small for decimate instance");

/* Accessor macros for opaque DSP instances */
#define INTERP_INST(ctx) ((riscv_fir_interpolate_instance_f32 *)(ctx)->interp_inst)
#define DECIMATE_INST(ctx) ((riscv_fir_decimate_instance_f32 *)(ctx)->decimate_inst)
#endif

#define TAG "RESAMP"

#define DEFAULT_HEADROOM 0.9f

/* Resampling mode enumeration */
enum {
    RESAMPLE_MODE_NONE = 0,
    RESAMPLE_MODE_16_TO_32,
    RESAMPLE_MODE_24_TO_32,
    RESAMPLE_MODE_48_TO_32,
    RESAMPLE_MODE_16_TO_48,
    RESAMPLE_MODE_24_TO_48,
    RESAMPLE_MODE_32_TO_48,
    RESAMPLE_MODE_48_TO_16,
    RESAMPLE_MODE_32_TO_16,
    RESAMPLE_MODE_24_TO_16,
};

/*
 * Pre-computed FIR lowpass, 128-tap Kaiser (beta=5.653), designed at the
 * interpolated rate.
 *
 * The cutoff is normalised to the INTERMEDIATE Nyquist, and it has to hold for
 * every rate pair this resampler serves.  The binding case is
 * output_nyquist/intermediate_nyquist = 1/3 (48->32, 48->16, 24->32, 24->16),
 * so the stopband must begin at 0.3333: -6 dB at 0.3050, passband flat to
 * 0.2767 (13284 Hz when the intermediate rate is 96 kHz),
 * stopband from 0.3333 at better than -57 dB.
 *
 * The previous 48-tap set was documented as "fc=0.32 (15.4kHz @ 96kHz)" but
 * measured flat to 0.5 and -36 dB only at 0.7 - a cutoff near 0.6, i.e. the
 * scipy firwin normalisation was taken as fraction-of-fs where the docstring
 * means fraction-of-Nyquist.  At 48->32 that let everything from 16 kHz to
 * 32 kHz fold back into the audible band at roughly unit gain: the anti-alias
 * filter was not attenuating anything it needed to.
 */
#ifdef CONFIG_NMSIS_DSP
static const float32_t fir_coeffs_base[AUDIO_RESAMPLER_NUM_TAPS] = {
    -0.000093773f,  -0.000026579f,   0.000112778f,   0.000200110f,
     0.000110710f,  -0.000136619f,  -0.000341165f,  -0.000270131f,
     0.000102298f,   0.000496149f,   0.000518025f,   0.000031722f,
    -0.000626190f,  -0.000852050f,  -0.000307440f,   0.000674256f,
     0.001247100f,   0.000758200f,  -0.000568763f,  -0.001649839f,
    -0.001397854f,   0.000231205f,   0.001976470f,   0.002209696f,
     0.000412504f,  -0.002114824f,  -0.003136844f,  -0.001418008f,
     0.001931238f,   0.004075757f,   0.002807450f,  -0.001281807f,
    -0.004873988f,  -0.004555316f,   0.000026558f,   0.005332343f,
     0.006577024f,   0.001955952f,  -0.005209944f,  -0.008721318f,
    -0.004757294f,   0.004228183f,   0.010765844f,   0.008436291f,
    -0.002064885f,  -0.012411742f,  -0.013033061f,  -0.001680400f,
     0.013265129f,   0.018617641f,   0.007598462f,  -0.012770448f,
    -0.025423048f,  -0.016812075f,   0.009976284f,   0.034250070f,
     0.032176758f,  -0.002583245f,  -0.048149291f,  -0.064314420f,
    -0.019028272f,   0.086054259f,   0.210013981f,   0.293440190f,
     0.293440190f,   0.210013981f,   0.086054259f,  -0.019028272f,
    -0.064314420f,  -0.048149291f,  -0.002583245f,   0.032176758f,
     0.034250070f,   0.009976284f,  -0.016812075f,  -0.025423048f,
    -0.012770448f,   0.007598462f,   0.018617641f,   0.013265129f,
    -0.001680400f,  -0.013033061f,  -0.012411742f,  -0.002064885f,
     0.008436291f,   0.010765844f,   0.004228183f,  -0.004757294f,
    -0.008721318f,  -0.005209944f,   0.001955952f,   0.006577024f,
     0.005332343f,   0.000026558f,  -0.004555316f,  -0.004873988f,
    -0.001281807f,   0.002807450f,   0.004075757f,   0.001931238f,
    -0.001418008f,  -0.003136844f,  -0.002114824f,   0.000412504f,
     0.002209696f,   0.001976470f,   0.000231205f,  -0.001397854f,
    -0.001649839f,  -0.000568763f,   0.000758200f,   0.001247100f,
     0.000674256f,  -0.000307440f,  -0.000852050f,  -0.000626190f,
     0.000031722f,   0.000518025f,   0.000496149f,   0.000102298f,
    -0.000270131f,  -0.000341165f,  -0.000136619f,   0.000110710f,
     0.000200110f,   0.000112778f,  -0.000026579f,  -0.000093773f
};
#endif

#ifndef CONFIG_NMSIS_DSP
/*
 * Linear interpolation resampler (fallback when NMSIS-DSP unavailable)
 */
static uint32_t resample_linear(const int16_t *in, uint32_t in_count,
                                 int16_t *out, uint32_t out_rate, uint32_t in_rate)
{
    if (in_rate == out_rate) {
        memcpy(out, in, in_count * sizeof(int16_t));
        return in_count;
    }

    uint32_t out_count = (in_count * out_rate + in_rate - 1) / in_rate;

    for (uint32_t i = 0; i < out_count; i++) {
        uint32_t pos = (i * in_rate) / out_rate;
        uint32_t frac = ((i * in_rate) % out_rate) * 65536 / out_rate;

        if (pos + 1 < in_count) {
            int32_t s0 = in[pos];
            int32_t s1 = in[pos + 1];
            out[i] = (int16_t)((s0 * (65536 - frac) + s1 * frac) >> 16);
        } else {
            out[i] = in[pos];
        }
    }

    return out_count;
}
#endif /* !CONFIG_NMSIS_DSP */

#ifdef CONFIG_NMSIS_DSP
/*
 * FIR polyphase resampling: interpolate by L, then decimate by M
 */
static uint32_t resample_fir(audio_resampler_t *ctx,
                              const int16_t *in, uint32_t in_count, int16_t *out)
{
    if (in_count > AUDIO_RESAMPLER_MAX_INPUT) {
        IPRO_LOGW(TAG, "Input too large: %lu > %d",
                  (unsigned long)in_count, AUDIO_RESAMPLER_MAX_INPUT);
        in_count = AUDIO_RESAMPLER_MAX_INPUT;
    }

    /* Convert int16 to float32 with headroom */
    const float32_t scale_in = ctx->headroom / 32768.0f;
    for (uint32_t i = 0; i < in_count; i++) {
        ctx->input_f32[i] = (float32_t)in[i] * scale_in;
    }

    uint32_t out_count;

    if (ctx->interp_factor == 1) {
        /* Decimate only (no interpolation) — used for 48k→16k, 32k→16k */
        out_count = in_count / ctx->decimate_factor;
        riscv_fir_decimate_f32(DECIMATE_INST(ctx),
                                ctx->input_f32, ctx->output_f32, in_count);

        float32_t scale_out = ctx->output_gain * 32768.0f;
        for (uint32_t i = 0; i < out_count; i++) {
            float32_t sample = ctx->output_f32[i] * scale_out;
            if (sample > 32767.0f) sample = 32767.0f;
            else if (sample < -32768.0f) sample = -32768.0f;
            out[i] = (int16_t)sample;
        }
    } else if (ctx->decimate_factor == 1) {
        /* Interpolate only (no decimation) */
        uint32_t interp_count = in_count * ctx->interp_factor;
        riscv_fir_interpolate_f32(INTERP_INST(ctx),
                                   ctx->input_f32, ctx->interp_f32, in_count);

        out_count = interp_count;
        float32_t scale_out = ctx->output_gain * 32768.0f;
        for (uint32_t i = 0; i < out_count; i++) {
            float32_t sample = ctx->interp_f32[i] * scale_out;
            if (sample > 32767.0f) sample = 32767.0f;
            else if (sample < -32768.0f) sample = -32768.0f;
            out[i] = (int16_t)sample;
        }
    } else {
        /* Interpolate then decimate */
        uint32_t interp_count = in_count * ctx->interp_factor;
        riscv_fir_interpolate_f32(INTERP_INST(ctx),
                                   ctx->input_f32, ctx->interp_f32, in_count);

        out_count = interp_count / ctx->decimate_factor;
        riscv_fir_decimate_f32(DECIMATE_INST(ctx),
                                ctx->interp_f32, ctx->output_f32, interp_count);

        float32_t scale_out = ctx->output_gain * 32768.0f;
        for (uint32_t i = 0; i < out_count; i++) {
            float32_t sample = ctx->output_f32[i] * scale_out;
            if (sample > 32767.0f) sample = 32767.0f;
            else if (sample < -32768.0f) sample = -32768.0f;
            out[i] = (int16_t)sample;
        }
    }

    return out_count;
}
#endif /* CONFIG_NMSIS_DSP */

int audio_resampler_init(audio_resampler_t *ctx, const audio_resampler_config_t *cfg)
{
    if (!ctx || !cfg) return -1;

    memset(ctx, 0, sizeof(audio_resampler_t));

    ctx->input_rate = cfg->input_rate;
    ctx->output_rate = cfg->output_rate;
    ctx->headroom = (cfg->headroom > 0.0f && cfg->headroom <= 1.0f)
                    ? cfg->headroom : DEFAULT_HEADROOM;

    /* Passthrough */
    if (cfg->input_rate == cfg->output_rate) {
        ctx->mode = RESAMPLE_MODE_NONE;
        ctx->initialized = true;
        IPRO_LOGI(TAG, "Resampler: passthrough (%lu Hz)", (unsigned long)cfg->input_rate);
        return 0;
    }

#ifdef CONFIG_NMSIS_DSP
    /* Determine rational resampling factors */
    if (cfg->input_rate == 16000 && cfg->output_rate == 32000) {
        ctx->mode = RESAMPLE_MODE_16_TO_32;
        ctx->interp_factor = 2;  ctx->decimate_factor = 1;
    } else if (cfg->input_rate == 24000 && cfg->output_rate == 32000) {
        ctx->mode = RESAMPLE_MODE_24_TO_32;
        ctx->interp_factor = 4;  ctx->decimate_factor = 3;
    } else if (cfg->input_rate == 48000 && cfg->output_rate == 32000) {
        ctx->mode = RESAMPLE_MODE_48_TO_32;
        ctx->interp_factor = 2;  ctx->decimate_factor = 3;
    } else if (cfg->input_rate == 16000 && cfg->output_rate == 48000) {
        ctx->mode = RESAMPLE_MODE_16_TO_48;
        ctx->interp_factor = 3;  ctx->decimate_factor = 1;
    } else if (cfg->input_rate == 24000 && cfg->output_rate == 48000) {
        ctx->mode = RESAMPLE_MODE_24_TO_48;
        ctx->interp_factor = 2;  ctx->decimate_factor = 1;
    } else if (cfg->input_rate == 32000 && cfg->output_rate == 48000) {
        ctx->mode = RESAMPLE_MODE_32_TO_48;
        ctx->interp_factor = 3;  ctx->decimate_factor = 2;
    } else if (cfg->input_rate == 48000 && cfg->output_rate == 16000) {
        ctx->mode = RESAMPLE_MODE_48_TO_16;
        ctx->interp_factor = 1;  ctx->decimate_factor = 3;
    } else if (cfg->input_rate == 32000 && cfg->output_rate == 16000) {
        ctx->mode = RESAMPLE_MODE_32_TO_16;
        ctx->interp_factor = 1;  ctx->decimate_factor = 2;
    } else if (cfg->input_rate == 24000 && cfg->output_rate == 16000) {
        ctx->mode = RESAMPLE_MODE_24_TO_16;
        ctx->interp_factor = 2;  ctx->decimate_factor = 3;
    } else {
        IPRO_LOGE(TAG, "Unsupported rate %lu -> %lu",
                  (unsigned long)cfg->input_rate, (unsigned long)cfg->output_rate);
        return -1;
    }

    /* Scale filter coefficients by interpolation factor (energy preservation) */
    for (int i = 0; i < AUDIO_RESAMPLER_NUM_TAPS; i++) {
        ctx->interp_coeffs[i] = fir_coeffs_base[i] * ctx->interp_factor;
    }
    ctx->output_gain = (ctx->interp_factor > 1) ? (1.0f / ctx->interp_factor) : 1.0f;

    riscv_status status;

    /* Initialize interpolator (skip when interp_factor=1, decimate-only mode) */
    if (ctx->interp_factor > 1) {
        status = riscv_fir_interpolate_init_f32(
            INTERP_INST(ctx),
            ctx->interp_factor,
            AUDIO_RESAMPLER_NUM_TAPS,
            ctx->interp_coeffs,
            ctx->interp_state,
            AUDIO_RESAMPLER_MAX_INPUT
        );
        if (status != RISCV_MATH_SUCCESS) {
            IPRO_LOGE(TAG, "FIR interpolator init failed: %d", status);
            return -1;
        }
    }

    /* Initialize decimator (skip when decimate_factor=1, interpolate-only mode) */
    if (ctx->decimate_factor > 1) {
        uint32_t decimate_block_len = (ctx->interp_factor > 1)
            ? AUDIO_RESAMPLER_MAX_INTERP : AUDIO_RESAMPLER_MAX_INPUT;
        status = riscv_fir_decimate_init_f32(
            DECIMATE_INST(ctx),
            AUDIO_RESAMPLER_NUM_TAPS,
            ctx->decimate_factor,
            fir_coeffs_base,
            ctx->decimate_state,
            decimate_block_len
        );
        if (status != RISCV_MATH_SUCCESS) {
            IPRO_LOGE(TAG, "FIR decimator init failed: %d", status);
            return -1;
        }
    }

    ctx->initialized = true;
    IPRO_LOGI(TAG, "FIR resampler: %lu -> %lu Hz (L=%d, M=%d, taps=%d, headroom=%.2f)",
              (unsigned long)cfg->input_rate, (unsigned long)cfg->output_rate,
              ctx->interp_factor, ctx->decimate_factor, AUDIO_RESAMPLER_NUM_TAPS,
              (double)ctx->headroom);
    return 0;

#else
    /* Linear fallback — always succeeds */
    ctx->mode = RESAMPLE_MODE_16_TO_32; /* Non-zero = needs resampling */
    ctx->initialized = true;
    IPRO_LOGI(TAG, "Linear resampler: %lu -> %lu Hz",
              (unsigned long)cfg->input_rate, (unsigned long)cfg->output_rate);
    return 0;
#endif
}

void audio_resampler_reset(audio_resampler_t *ctx)
{
    if (!ctx || !ctx->initialized) return;

#ifdef CONFIG_NMSIS_DSP
    memset(ctx->interp_state, 0, sizeof(ctx->interp_state));
    memset(ctx->decimate_state, 0, sizeof(ctx->decimate_state));
#endif
}

uint32_t audio_resampler_process(audio_resampler_t *ctx,
                                  const int16_t *in, uint32_t in_count,
                                  int16_t *out, uint32_t out_capacity)
{
    if (!ctx || !ctx->initialized || !in || !out || in_count == 0) return 0;

    /* Passthrough */
    if (ctx->input_rate == ctx->output_rate) {
        uint32_t n = (in_count < out_capacity) ? in_count : out_capacity;
        memcpy(out, in, n * sizeof(int16_t));
        return n;
    }

#ifdef CONFIG_NMSIS_DSP
    return resample_fir(ctx, in, in_count, out);
#else
    return resample_linear(in, in_count, out, ctx->output_rate, ctx->input_rate);
#endif
}

uint32_t audio_resampler_get_output_count(const audio_resampler_t *ctx, uint32_t in_count)
{
    if (!ctx || ctx->input_rate == 0) return 0;

    if (ctx->input_rate == ctx->output_rate) return in_count;

    return (in_count * ctx->output_rate + ctx->input_rate - 1) / ctx->input_rate;
}
