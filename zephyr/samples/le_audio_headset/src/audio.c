/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * LC3 decode for the headset: SDUs from the CIS are queued (net_buf ref) to
 * a decoder thread so the Bluetooth RX thread never waits on the codec;
 * decoded mono PCM goes to pcm_out (GPDAC speaker). A lost or errored SDU
 * replays the last good frame (halving per consecutive loss) rather than
 * liblc3's sign-randomising PLC, which is a burst of noise on music.
 */
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <lc3.h>
#include <string.h>
#include "audio.h"
#include "pcm_out.h"

LOG_MODULE_REGISTER(headset_audio, CONFIG_LOG_DEFAULT_LEVEL);

#define MAX_FRAME_SAMPLES 480 /* 48 kHz x 10 ms */
#define QUEUE_DEPTH 8

struct sdu {
	void *fifo_reserved;
	struct net_buf *buf;
	uint8_t flags;
};

K_MEM_SLAB_DEFINE_STATIC(sdu_slab, sizeof(struct sdu), QUEUE_DEPTH, 4);
static K_FIFO_DEFINE(sdu_fifo);
static K_THREAD_STACK_DEFINE(dec_stack, 8192);
static struct k_thread dec_thread;

static struct {
	uint32_t rate, dur_us;
	uint16_t octets;
	lc3_decoder_t dec;
	lc3_decoder_mem_48k_t mem;
	int16_t pcm[MAX_FRAME_SAMPLES];
	int16_t last_good[MAX_FRAME_SAMPLES];
	int frame_samples;
	uint8_t lost_run;
	bool streaming;
	uint32_t rx, decoded, concealed, dropped, errors;
} a;

static void decode_one(struct sdu *s)
{
	const bool valid = (s->flags & BT_ISO_FLAGS_VALID) != 0 && s->buf != NULL &&
			   s->buf->len == a.octets;

	if (a.dec == NULL) {
		return;
	}
	if (valid) {
		int r = lc3_decode(a.dec, s->buf->data, a.octets, LC3_PCM_FORMAT_S16, a.pcm, 1);

		if (r < 0) {
			a.errors++;
			return;
		}
		memcpy(a.last_good, a.pcm, a.frame_samples * sizeof(int16_t));
		a.lost_run = 0;
		a.decoded++;
	} else {
		const int shift = MIN(a.lost_run, 8);

		for (int i = 0; i < a.frame_samples; i++) {
			a.pcm[i] = (int16_t)(a.last_good[i] >> shift);
		}
		if (a.lost_run < UINT8_MAX) {
			a.lost_run++;
		}
		a.concealed++;
	}
	(void)pcm_out_write(a.pcm, a.frame_samples);
	if ((++a.rx % 1000) == 0) {
		struct pcm_out_stats st;

		pcm_out_stats(&st);
		LOG_INF("%u SDUs: decoded %u, concealed %u, errors %u, dropped %u | ring %u B, underruns %u, overruns %u, step %d, peak in %d out %d",
			a.rx, a.decoded, a.concealed, a.errors, a.dropped, st.ring_bytes,
			st.underruns, st.overruns, st.step_q16, st.in_peak, st.out_peak);
	}
}

static void dec_thread_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
	while (true) {
		struct sdu *s = k_fifo_get(&sdu_fifo, K_FOREVER);

		if (a.streaming) {
			decode_one(s);
		}
		if (s->buf != NULL) {
			net_buf_unref(s->buf);
		}
		k_mem_slab_free(&sdu_slab, (void *)s);
	}
}

int audio_configure(uint32_t sample_rate, uint32_t frame_dur_us, uint16_t octets_per_frame)
{
	static bool thread_started;

	if (!thread_started) {
		k_thread_create(&dec_thread, dec_stack, K_THREAD_STACK_SIZEOF(dec_stack),
				dec_thread_fn, NULL, NULL, NULL, K_PRIO_PREEMPT(5), 0, K_NO_WAIT);
		k_thread_name_set(&dec_thread, "lc3_dec");
		thread_started = true;
	}
	if (a.streaming && (sample_rate != a.rate || frame_dur_us != a.dur_us)) {
		audio_stop();
	}
	a.rate = sample_rate;
	a.dur_us = frame_dur_us;
	a.octets = octets_per_frame;
	a.frame_samples = lc3_frame_samples((int)frame_dur_us, (int)sample_rate);
	if (a.frame_samples <= 0 || a.frame_samples > MAX_FRAME_SAMPLES) {
		LOG_ERR("unsupported LC3 config %u Hz / %u us", sample_rate, frame_dur_us);
		a.dec = NULL;
		return -ENOTSUP;
	}
	a.dec = lc3_setup_decoder((int)frame_dur_us, (int)sample_rate, 0, &a.mem);
	if (a.dec == NULL) {
		LOG_ERR("lc3_setup_decoder failed");
		return -EINVAL;
	}
	memset(a.last_good, 0, sizeof(a.last_good));
	a.lost_run = 0;
	LOG_INF("LC3 %u Hz, %u us, %u octets/frame (%d samples)", sample_rate, frame_dur_us,
		octets_per_frame, a.frame_samples);
	return 0;
}

int audio_start(void)
{
	int r;

	if (a.dec == NULL) {
		return -EINVAL;
	}
	if (a.streaming) {
		return 0;
	}
	r = pcm_out_start(a.rate);
	if (r != 0) {
		LOG_ERR("pcm_out_start: %d", r);
		return r;
	}
	a.rx = 0; a.decoded = 0; a.concealed = 0; a.dropped = 0; a.errors = 0;
	a.streaming = true;
	return 0;
}

void audio_stop(void)
{
	a.streaming = false;
	pcm_out_stop();
}

void audio_push(const struct bt_iso_recv_info *info, struct net_buf *buf)
{
	struct sdu *s;

	if (!a.streaming) {
		return;
	}
	if (k_mem_slab_alloc(&sdu_slab, (void **)&s, K_NO_WAIT) != 0) {
		a.dropped++;
		return;
	}
	s->buf = net_buf_ref(buf);
	s->flags = info != NULL ? info->flags : 0;
	k_fifo_put(&sdu_fifo, s);
}

void audio_set_volume(uint8_t volume, bool mute)
{
	pcm_out_set_volume(volume, mute);
}
