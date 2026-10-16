/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Continuous PCM playback through the IPRO7 GPDAC (channel A, GPIO23) fed
 * by DMA - the streaming path the SDK's components/audio uses, exposed as a
 * small vendor API since Zephyr's DAC API has no streaming. Mono signed
 * 16-bit PCM; the GPDAC clock offers 8, 16 and 32 kHz.
 *
 * Starting the stream (re)initialises the GPDAC, which puts BOTH DAC pads
 * (GPIO23 = A, GPIO22 = B) on the analogue function: a board that uses
 * GPIO22 as a GPIO (the EVK's speaker enable) must gpio_pin_configure() it
 * again after ipro_dac_stream_start().
 */
#ifndef IPRO_ZEPHYR_IPRO_DAC_STREAM_H_
#define IPRO_ZEPHYR_IPRO_DAC_STREAM_H_

#include <zephyr/device.h>
#include <stdint.h>
#include <stddef.h>

/** Called from a work queue to produce the next @p samples PCM samples. */
typedef void (*ipro_dac_stream_fill_t)(int16_t *pcm, size_t samples, void *user);

struct ipro_dac_stream_cfg {
	const struct device *dma;     /* dma0 */
	uint32_t dma_channel;         /* channel to use (avoid 0: AUADC HAL owns it) */
	uint32_t sample_rate;         /* 8000, 16000 or 32000 */
	size_t seg_samples;           /* samples per DMA segment (two segments ping-pong) */
	ipro_dac_stream_fill_t fill;  /* producer callback */
	void *user;
};

int ipro_dac_stream_start(const struct ipro_dac_stream_cfg *cfg);
void ipro_dac_stream_stop(void);
bool ipro_dac_stream_running(void);
/** Number of DMA segments consumed since start (underrun diagnostics). */
uint32_t ipro_dac_stream_segments(void);
/** Segments refilled by the producer since start; lagging behind segments = replayed audio. */
uint32_t ipro_dac_stream_refills(void);

#endif /* IPRO_ZEPHYR_IPRO_DAC_STREAM_H_ */
