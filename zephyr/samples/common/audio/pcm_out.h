/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Mono 16-bit PCM to the IPRO7 GPDAC speaker path, shared by the LE Audio
 * samples (broadcast sink, unicast headset). Any input rate: the DAC runs
 * at 16 kHz for inputs up to 16 kHz and at 32 kHz above, and the DMA
 * refill resamples (linear interpolation) with a step servoed on the ring
 * level so the source's and the DAC's clocks never run apart.
 */
#ifndef IPRO_SAMPLES_PCM_OUT_H
#define IPRO_SAMPLES_PCM_OUT_H

#include <stdint.h>
#include <stddef.h>

struct pcm_out_stats {
	uint32_t frames;     /* pcm_out_write() calls accepted */
	uint32_t underruns;  /* refills that found the ring empty */
	uint32_t overruns;   /* samples dropped because the ring was full */
	uint32_t ring_bytes; /* current ring level */
	uint32_t dma_segments;
	uint32_t refills;
	int32_t step_q16;    /* resampler step, in_rate/out_rate in Q16 */
	int32_t in_peak;     /* since the last pcm_out_stats() call */
	int32_t out_peak;
};

/* One-time DAC/pad setup; safe to call again. */
int pcm_out_init(void);
/* Start playing input at in_rate Hz; no-op when already running. */
int pcm_out_start(uint32_t in_rate);
/* Queue n mono samples (blocking never; drops when the ring is full). */
int pcm_out_write(const int16_t *pcm, size_t n);
/* Volume 0..255 (VCP scale), 255 = unity; applied on write. */
void pcm_out_set_volume(uint8_t volume, bool mute);
void pcm_out_stop(void);
bool pcm_out_running(void);
uint32_t pcm_out_rate(void);
void pcm_out_stats(struct pcm_out_stats *st);

#endif
