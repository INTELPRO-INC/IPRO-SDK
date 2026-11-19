/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 *
 * PCM16 output on the IPRO6S I2S port, fed from the A2DP decoder.
 */
#ifndef A2DP_I2S_OUT_H
#define A2DP_I2S_OUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t sample_rate_hz;
    uint8_t channels;
    bool playing;
    uint32_t written_bytes;
    uint32_t dropped_writes;
    uint32_t underruns;
    uint32_t buffered_bytes;
} a2dp_i2s_stats_t;

/* Pins, DMA channel and the refill task. Call once before any other call. */
int a2dp_i2s_init(void);

/* Set the stream format (8/16/32/44.1/48 kHz, 1 or 2 channels). */
int a2dp_i2s_configure(uint32_t sample_rate_hz, uint8_t channels);

/* Start or stop playing; stopping outputs silence and drops buffered PCM. */
void a2dp_i2s_set_playing(bool playing);

/* Queue interleaved PCM16. Returns false when the buffer is full. */
bool a2dp_i2s_write(const uint8_t *pcm, size_t len);

void a2dp_i2s_get_stats(a2dp_i2s_stats_t *stats);

#endif /* A2DP_I2S_OUT_H */
