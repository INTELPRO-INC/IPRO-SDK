/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Packet-loss concealment in front of liblc3 for the upstream sink's
 * lc3.c (compiled with -Dlc3_decode=ipro_lc3_decode_plc and
 * -Dlc3_setup_decoder=ipro_lc3_setup_decoder_plc). liblc3's own
 * PLC repeats the last spectrum with random sign flips, which turns a lost
 * 10 ms of tonal audio into a burst of noise - a clear hiccup on the EVK
 * speaker at ~1 % SDU loss. Here a lost frame replays the last good PCM
 * frame instead, each further consecutive loss at half the level, and the
 * decoder is left untouched so the next good frame decodes as usual.
 * S16 output only (what the sample uses); other formats fall through.
 */
#include <zephyr/kernel.h>
#include <string.h>
#include <lc3.h>

#define MAX_FRAME_SAMPLES 480 /* 48 kHz x 10 ms */

static int16_t last_good[CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT][MAX_FRAME_SAMPLES];
static uint8_t lost_run[CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT];
static lc3_decoder_t known[CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT];
static int frame_samples[CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT];

static int slot_of(lc3_decoder_t decoder)
{
	for (int i = 0; i < ARRAY_SIZE(known); i++) {
		if (known[i] == decoder) {
			return i;
		}
	}
	return -1;
}

lc3_decoder_t ipro_lc3_setup_decoder_plc(int dt_us, int sr_hz, int sr_pcm_hz, void *mem)
{
	lc3_decoder_t d = lc3_setup_decoder(dt_us, sr_hz, sr_pcm_hz, mem);

	if (d != NULL) {
		int slot = slot_of(d);

		if (slot < 0) {
			slot = slot_of(NULL);
		}
		if (slot >= 0) {
			known[slot] = d;
			frame_samples[slot] = lc3_frame_samples(dt_us, sr_pcm_hz ? sr_pcm_hz : sr_hz);
			lost_run[slot] = 0;
			memset(last_good[slot], 0, sizeof(last_good[slot]));
		}
	}
	return d;
}

int ipro_lc3_decode_plc(lc3_decoder_t decoder, const void *in, int nbytes,
			enum lc3_pcm_format fmt, void *pcm, int stride)
{
	const int slot = slot_of(decoder);
	const int n = slot < 0 ? 0 : frame_samples[slot];

	if (slot < 0 || fmt != LC3_PCM_FORMAT_S16 || stride != 1 || n <= 0 ||
	    n > MAX_FRAME_SAMPLES) {
		return lc3_decode(decoder, in, nbytes, fmt, pcm, stride);
	}
	if (in == NULL) {
		int16_t *out = pcm;
		const int shift = MIN(lost_run[slot], 8);

		for (int i = 0; i < n; i++) {
			out[i] = (int16_t)(last_good[slot][i] >> shift);
		}
		if (lost_run[slot] < UINT8_MAX) {
			lost_run[slot]++;
		}
		return 0;
	}
	int r = lc3_decode(decoder, in, nbytes, fmt, pcm, stride);

	if (r == 0) {
		memcpy(last_good[slot], pcm, n * sizeof(int16_t));
		lost_run[slot] = 0;
	}
	return r;
}
