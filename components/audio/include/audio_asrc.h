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
 * @file audio_asrc.h
 * @brief Integer streaming resampler with a trimmable ratio, and the FIFO
 *        level controller that trims it.
 *
 * Audio clocks are rarely the rates their names say (on the IPRO7AI EVK the
 * PDM mic runs at 15873 Hz for "16 kHz"), and two boards' crystals differ by
 * a few ppm. audio_asrc converts between any two
 * rates within 1:4..4:1 (16-tap polyphase windowed sinc, 128 phases
 * interpolated, Q32.32 position) and keeps its state across calls, so the
 * output does not depend on how the input is cut into blocks. A controller
 * (audio_asrc_ctl) watches the level of the FIFO on the far side of the
 * resampler and trims its ratio by up to a few hundred ppm to hold it.
 *
 * Integer only (the FreeRTOS port saves no FPU state). ~1 % of the CPU at a
 * 32 kHz output rate.
 */
#ifndef AUDIO_ASRC_H
#define AUDIO_ASRC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_ASRC_TAPS    16
#define AUDIO_ASRC_PHASES  128
#define AUDIO_ASRC_MAX_IN  640        /* input samples one audio_asrc_process() call accepts */

extern const int16_t g_audio_asrc_coef[AUDIO_ASRC_PHASES + 1][AUDIO_ASRC_TAPS];

typedef struct {
    uint64_t step_nom;              /* Q32.32 input samples per output sample */
    uint64_t step;                  /* step_nom after the ppm trim */
    uint64_t pos;                   /* Q32.32 position of the next output in buf[] */
    uint32_t count;                 /* valid samples in buf[] */
    int16_t  buf[AUDIO_ASRC_TAPS + AUDIO_ASRC_MAX_IN];
} audio_asrc_t;

/**
 * @brief Set up a converter from @p in_mhz to @p out_mhz (sample rates in
 *        milli-hertz, e.g. 15875000 for 15875 Hz).
 * @return 0, or -1 for a zero rate or a ratio outside 1:4..4:1.
 */
int audio_asrc_init(audio_asrc_t *s, uint32_t in_mhz, uint32_t out_mhz);

/** @brief Clear the history (the ratio stays). */
void audio_asrc_reset(audio_asrc_t *s);

/**
 * @brief Trim the ratio: consume @p ppm_x100 / 100 ppm more input per output
 *        than nominal (negative = less). Replaces any earlier trim.
 */
void audio_asrc_trim_ppm_x100(audio_asrc_t *s, int32_t ppm_x100);

/**
 * @brief Convert. Takes at most AUDIO_ASRC_MAX_IN samples and as many as fit
 *        in the history; produces as many outputs as the input allows, up to
 *        @p out_cap.
 * @param in_used samples of @p in taken (the caller keeps the rest)
 * @return samples written to @p out
 */
uint32_t audio_asrc_process(audio_asrc_t *s, const int16_t *in, uint32_t n_in,
                          int16_t *out, uint32_t out_cap, uint32_t *in_used);

/**
 * @brief Input samples held inside the converter that no output has passed
 *        yet. Add it to the FIFO level fed to audio_asrc_ctl_update(): the
 *        converter takes input in blocks, so the FIFO level alone jumps.
 */
uint32_t audio_asrc_pending(const audio_asrc_t *s);

/* ---------------------------------------------------------- level control */

typedef struct {
    int32_t target;                 /* FIFO level to hold, samples */
    int32_t max_x100;               /* trim limit, ppm x 100 */
    int32_t level_q8;               /* smoothed level, Q8 */
    bool    primed;
} audio_asrc_ctl_t;

/**
 * @brief Controller for a FIFO on either side of a resampler: a FIFO that
 *        runs fuller asks the resampler for a larger step (it reads the
 *        FIFO faster, or feeds it slower). Proportional, 0.5 ppm per sample
 *        of error (settles in ~2 min at 16 kHz), level smoothed over ~128
 *        calls. Slow on purpose: it absorbs crystal offsets (tens of ppm),
 *        so the nominal rates given to audio_asrc_init() must be right to
 *        ~20 ppm, and the FIFO target needs ~100 samples of room per
 *        50 ppm of offset.
 * @param max_ppm trim limit in ppm
 */
void audio_asrc_ctl_init(audio_asrc_ctl_t *c, int32_t target, int32_t max_ppm);

/**
 * @brief Feed one level reading and get the trim for audio_asrc_trim_ppm_x100().
 *        Call at a fixed rate and a fixed point of the fill / drain cycle,
 *        with everything queued between producer and consumer (FIFO +
 *        audio_asrc_pending() + what the DAC ring still holds, in FIFO samples).
 */
int32_t audio_asrc_ctl_update(audio_asrc_ctl_t *c, int32_t level);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_ASRC_H */
