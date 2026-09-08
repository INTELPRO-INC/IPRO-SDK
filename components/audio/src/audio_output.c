/**
 * @file audio_output.c
 * @brief Audio output sink dispatcher
 *
 * Routes create/start/stop/write/destroy to the appropriate backend
 * based on audio_output_type_t. Each backend is conditionally compiled
 * based on its hardware/stack dependencies (UAC needs CherryUSB, I2S
 * needs hal_i2s, etc). The DAC and DUMP backends are always available.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#include <generated/autoconf.h>

#include "audio_output.h"
#include <stdio.h>

/* DAC and DUMP backends are always built */
extern audio_output_t *audio_output_dac_create(const audio_output_cfg_t *cfg);
extern void audio_output_dac_destroy(audio_output_t *out);
extern int  audio_output_dac_start(audio_output_t *out);
extern int  audio_output_dac_stop(audio_output_t *out);
extern int  audio_output_dac_write(audio_output_t *out, const int16_t *pcm, uint32_t samples);
extern uint32_t audio_output_dac_segment_samples(audio_output_t *out);

extern audio_output_t *audio_output_dump_create(const audio_output_cfg_t *cfg);
extern void audio_output_dump_destroy(audio_output_t *out);
extern int  audio_output_dump_start(audio_output_t *out);
extern int  audio_output_dump_stop(audio_output_t *out);
extern int  audio_output_dump_write(audio_output_t *out, const int16_t *pcm, uint32_t samples);

/* Must track the CMake gate in components/audio/CMakeLists.txt: an app that
 * implements its own UAC device sets AUDIO_OUTPUT_UAC_EXCLUDE, which drops
 * audio_output_uac.c from the build, so the dispatcher must fall back to the
 * "not compiled in" cases below rather than reference missing symbols. */
#if defined(CONFIG_CHERRYUSB_DEVICE_AUDIO) && !defined(CONFIG_AUDIO_OUTPUT_UAC_EXCLUDE)
extern audio_output_t *audio_output_uac_create(const audio_output_cfg_t *cfg);
extern void audio_output_uac_destroy(audio_output_t *out);
extern int  audio_output_uac_start(audio_output_t *out);
extern int  audio_output_uac_stop(audio_output_t *out);
extern int  audio_output_uac_write(audio_output_t *out, const int16_t *pcm, uint32_t samples);
extern uint32_t audio_output_uac_consumed(audio_output_t *out);
extern uint32_t audio_output_uac_segment_samples(audio_output_t *out);
#endif

#ifdef CONFIG_BSP_USING_I2S0
extern audio_output_t *audio_output_i2s_create(const audio_output_cfg_t *cfg);
extern void audio_output_i2s_destroy(audio_output_t *out);
extern int  audio_output_i2s_start(audio_output_t *out);
extern int  audio_output_i2s_stop(audio_output_t *out);
extern int  audio_output_i2s_write(audio_output_t *out, const int16_t *pcm, uint32_t samples);
#endif

#if defined(CONFIG_CHERRYUSB_DEVICE_AUDIO) && !defined(CONFIG_AUDIO_OUTPUT_UAC_EXCLUDE)
#  define _UAC_CASE(method, ...) case AUDIO_OUTPUT_UAC: return audio_output_uac_##method(__VA_ARGS__);
#  define _UAC_CASE_VOID(method) case AUDIO_OUTPUT_UAC: audio_output_uac_##method(out); return;
#else
#  define _UAC_CASE(method, ...) case AUDIO_OUTPUT_UAC: \
        printf("audio_output: UAC backend not compiled in (need CONFIG_CHERRYUSB_DEVICE_AUDIO " \
               "and AUDIO_OUTPUT_UAC_EXCLUDE off)\n"); \
        return method ## _NULL;
#  define _UAC_CASE_VOID(method) case AUDIO_OUTPUT_UAC: return;
#endif

#ifdef CONFIG_BSP_USING_I2S0
#  define _I2S_CASE(method, ...) case AUDIO_OUTPUT_I2S: return audio_output_i2s_##method(__VA_ARGS__);
#  define _I2S_CASE_VOID(method) case AUDIO_OUTPUT_I2S: audio_output_i2s_##method(out); return;
#else
#  define _I2S_CASE(method, ...) case AUDIO_OUTPUT_I2S: \
        printf("audio_output: I2S backend not compiled in (need CONFIG_BSP_USING_I2S0)\n"); \
        return method ## _NULL;
#  define _I2S_CASE_VOID(method) case AUDIO_OUTPUT_I2S: return;
#endif

/* Sentinel return values for unavailable backends */
#define create_NULL  NULL
#define start_NULL   (-1)
#define stop_NULL    (-1)
#define write_NULL   (-1)

#define DISPATCH(method, ...) \
    switch (cfg->type) { \
    case AUDIO_OUTPUT_DAC:  return audio_output_dac_##method(__VA_ARGS__); \
    case AUDIO_OUTPUT_DUMP: return audio_output_dump_##method(__VA_ARGS__); \
    _UAC_CASE(method, __VA_ARGS__) \
    _I2S_CASE(method, __VA_ARGS__) \
    default: break; \
    }

audio_output_t *audio_output_create(const audio_output_cfg_t *cfg)
{
    if (!cfg) return NULL;
    DISPATCH(create, cfg);
    printf("audio_output: unknown type %d\n", cfg->type);
    return NULL;
}

void audio_output_destroy(audio_output_t *out)
{
    if (!out) return;
    const audio_output_cfg_t *cfg = (const audio_output_cfg_t *)out;
    switch (cfg->type) {
    case AUDIO_OUTPUT_DAC:  audio_output_dac_destroy(out);  return;
    case AUDIO_OUTPUT_DUMP: audio_output_dump_destroy(out); return;
    _UAC_CASE_VOID(destroy)
    _I2S_CASE_VOID(destroy)
    default: break;
    }
}

int audio_output_start(audio_output_t *out)
{
    if (!out) return -1;
    const audio_output_cfg_t *cfg = (const audio_output_cfg_t *)out;
    DISPATCH(start, out);
    return -1;
}

int audio_output_stop(audio_output_t *out)
{
    if (!out) return -1;
    const audio_output_cfg_t *cfg = (const audio_output_cfg_t *)out;
    DISPATCH(stop, out);
    return -1;
}

int audio_output_write(audio_output_t *out, const int16_t *pcm, uint32_t samples)
{
    if (!out) return -1;
    const audio_output_cfg_t *cfg = (const audio_output_cfg_t *)out;
    DISPATCH(write, out, pcm, samples);
    return -1;
}

/*
 * Backend-agnostic drain counter.
 *
 * audio_output_dac_consumed() dereferences the DAC backend's own
 * struct audio_output; every backend declares a different one, so calling it
 * on a handle from another backend silently reads the wrong layout.  Route
 * through the type tag instead, exactly like every other entry point here.
 */
uint32_t audio_output_consumed(audio_output_t *out)
{
    if (!out) return 0;

    const audio_output_cfg_t *cfg = (const audio_output_cfg_t *)out;

    switch (cfg->type) {
    case AUDIO_OUTPUT_DAC:
        return audio_output_dac_consumed(out);
#if defined(CONFIG_CHERRYUSB_DEVICE_AUDIO) && !defined(CONFIG_AUDIO_OUTPUT_UAC_EXCLUDE)
    case AUDIO_OUTPUT_UAC:
        return audio_output_uac_consumed(out);
#endif
    default:
        return 0;
    }
}

/*
 * The quantum of audio_output_consumed(): that counter only advances one DMA
 * segment at a time, so a drift measurement can resolve no better than this
 * many samples.  Callers use it to tell a real difference from quantisation.
 */
uint32_t audio_output_segment_samples(audio_output_t *out)
{
    if (!out) return 0;

    const audio_output_cfg_t *cfg = (const audio_output_cfg_t *)out;

    switch (cfg->type) {
    case AUDIO_OUTPUT_DAC:
        return audio_output_dac_segment_samples(out);
#if defined(CONFIG_CHERRYUSB_DEVICE_AUDIO) && !defined(CONFIG_AUDIO_OUTPUT_UAC_EXCLUDE)
    case AUDIO_OUTPUT_UAC:
        return audio_output_uac_segment_samples(out);
#endif
    default:
        return 0;
    }
}
