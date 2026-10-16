/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Microphone (PDM IM69D129 on GPIO18/19 through the DMIC driver) and
 * speaker (GPDAC channel A -> TPA2037D1 class-D, enable on GPIO22) for the
 * EVK demo: level meter, record to PSRAM, playback, tone, live loopback.
 */
#include "demo.h"
#include <zephyr/device.h>
#include <zephyr/audio/dmic.h>
#include <zephyr/drivers/dac.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/ring_buffer.h>
#include <ipro_zephyr/ipro_dac_stream.h>
#include <string.h>
#include <math.h>

#define RATE            16000          /* nominal; the AUADC really runs 15875 Hz */
#define BLOCK_SAMPLES   320            /* 20 ms */
#define BLOCK_BYTES     (BLOCK_SAMPLES * 2)
#define DAC_SEG         320
#define REC_SECONDS_MAX 20
#define REC_SAMPLES_MAX (RATE * REC_SECONDS_MAX)

static const struct device *const mic = DEVICE_DT_GET(DT_NODELABEL(pdm0));
static const struct device *const dac = DEVICE_DT_GET(DT_NODELABEL(dac0));
static const struct device *const dma = DEVICE_DT_GET(DT_NODELABEL(dma0));
static const struct gpio_dt_spec spk_en = GPIO_DT_SPEC_GET(DT_NODELABEL(spk_enable), gpios);

K_MEM_SLAB_DEFINE_STATIC(mic_slab, BLOCK_BYTES, 8, 4);

/* Recording lives in PSRAM (.psram_bss): 20 s at 16 kHz = 640 KB. */
__attribute__((section(".psram_bss"))) static int16_t rec_buf[REC_SAMPLES_MAX];
static uint32_t rec_len;
static volatile uint32_t play_pos;

/* Loopback ring between the mic thread (producer) and the DAC fill (consumer). */
RING_BUF_DECLARE(loop_ring, 8192);
static volatile bool loop_active;

static bool mic_configured;
static int makeup_shift = 2; /* playback gain 2^n (4 = +12 dB); the SDK uses +30 dB with a limiter */

static int32_t hp_prev_in, hp_prev_out;

/* Remove the AUADC's DC offset (one-pole high-pass, ~20 Hz) then apply the make-up gain. */
static inline int16_t makeup(int16_t v)
{
	int32_t y = (int32_t)v - hp_prev_in + (hp_prev_out * 255) / 256;

	hp_prev_in = v;
	hp_prev_out = y;
	int32_t x = y << makeup_shift;

	return (int16_t)CLAMP(x, INT16_MIN, INT16_MAX);
}

uint32_t audio_recorded_samples(void) { return rec_len; }
uint32_t audio_rate(void) { return RATE; }

static int mic_setup(void)
{
	static struct pcm_stream_cfg stream = {
		.pcm_rate = RATE, .pcm_width = 16, .block_size = BLOCK_BYTES, .mem_slab = &mic_slab,
	};
	static struct dmic_cfg cfg = {
		.io = {.min_pdm_clk_freq = 1000000, .max_pdm_clk_freq = 3500000},
		.streams = &stream,
		.channel = {.req_num_streams = 1, .req_num_chan = 1},
	};

	if (!device_is_ready(mic)) {
		return -ENODEV;
	}
	if (!mic_configured) {
		int r = dmic_configure(mic, &cfg);

		if (r != 0) {
			return r;
		}
		mic_configured = true;
	}
	return 0;
}

static int spk_enable(bool on)
{
	if (!gpio_is_ready_dt(&spk_en)) {
		return -ENODEV;
	}
	return gpio_pin_configure_dt(&spk_en, on ? GPIO_OUTPUT_ACTIVE : GPIO_OUTPUT_INACTIVE);
}

int audio_init(void)
{
	struct dac_channel_cfg ch = {.channel_id = 0, .resolution = 10};

	if (!device_is_ready(dac) || !device_is_ready(dma)) {
		return -ENODEV;
	}
	(void)dac_channel_setup(dac, &ch);
	(void)dac_write_value(dac, 0, 512);
	(void)spk_enable(false);
	return 0;
}

/* ---- capture helpers ---------------------------------------------------- */

static int mic_start(void)
{
	int r = mic_setup();

	if (r == 0) {
		r = dmic_trigger(mic, DMIC_TRIGGER_START);
	}
	return r;
}

static void mic_stop(void)
{
	(void)dmic_trigger(mic, DMIC_TRIGGER_STOP);
}

static uint32_t block_rms(const int16_t *pcm, size_t n)
{
	uint64_t acc = 0;

	for (size_t i = 0; i < n; i++) {
		acc += (int64_t)pcm[i] * pcm[i];
	}
	return (uint32_t)sqrt((double)(acc / n));
}

int audio_mic_level(const struct shell *sh, uint32_t seconds, bool on_lcd)
{
	int r = mic_start();
	int64_t end = k_uptime_get() + (int64_t)seconds * 1000;
	uint32_t peak = 0, blocks = 0;
	int64_t next_print = 0;
	uint32_t rms_acc = 0, rms_n = 0;
	int16_t bmin = INT16_MAX, bmax = INT16_MIN;
	int64_t mean_acc = 0;

	if (r != 0) {
		shell_error(sh, "mic start: %d", r);
		return r;
	}
	shell_print(sh, "mic level for %u s: 20 ms blocks, 16-bit PCM (full scale 32767)", seconds);
	while (k_uptime_get() < end) {
		void *blk;
		size_t sz;

		r = dmic_read(mic, 0, &blk, &sz, 500);
		if (r != 0) {
			shell_error(sh, "dmic_read: %d (no frames from the AUADC)", r);
			break;
		}
		const int16_t *pcm = blk;
		size_t n = sz / 2;

		if (blocks == 0) {
			shell_print(sh, "first block, samples 0..15:");
			for (int i = 0; i < 16; i++) {
				shell_fprintf(sh, SHELL_NORMAL, " %d", pcm[i]);
			}
			shell_print(sh, "");
		}
		uint32_t rms = block_rms(pcm, n);

		for (size_t i = 0; i < n; i++) {
			bmin = MIN(bmin, pcm[i]);
			bmax = MAX(bmax, pcm[i]);
			mean_acc += pcm[i];
		}
		rms_acc += rms;
		rms_n++;
		blocks++;
		peak = MAX(peak, rms);
		k_mem_slab_free(&mic_slab, blk);
		if (on_lcd) {
			(void)lcd_level_bar(MIN(rms * 1000 / 4000, 1000), rms > 3200 ? C_RED : C_GREEN);
		}
		if (k_uptime_get() >= next_print) {
			uint32_t avg = rms_n ? rms_acc / rms_n : 0;

			shell_print(sh, "t=%5lld ms  rms %5u  min %6d  max %6d  mean %6lld  |%-32.*s|",
				    k_uptime_get(), avg, bmin, bmax, mean_acc / (int64_t)(rms_n * n),
				    (int)MIN(avg * 32 / 4000, 32), "################################");
			next_print = k_uptime_get() + 250;
			rms_acc = 0; rms_n = 0; bmin = INT16_MAX; bmax = INT16_MIN; mean_acc = 0;
		}
	}
	mic_stop();
	shell_print(sh, "%u blocks, peak rms %u", blocks, peak);
	return r;
}

int audio_mic_dump(const struct shell *sh)
{
	int r = mic_start();

	if (r != 0) {
		return r;
	}
	shell_print(sh, "4 consecutive blocks: first 6 and last 4 samples (block boundary check)");
	for (int b = 0; b < 4; b++) {
		void *blk;
		size_t sz;

		if (dmic_read(mic, 0, &blk, &sz, 500) != 0) {
			break;
		}
		const int16_t *p = blk;
		size_t n = sz / 2;

		shell_print(sh, "blk %d: %6d %6d %6d %6d %6d %6d ... %6d %6d %6d %6d", b, p[0], p[1],
			    p[2], p[3], p[4], p[5], p[n - 4], p[n - 3], p[n - 2], p[n - 1]);
		k_mem_slab_free(&mic_slab, blk);
	}
	mic_stop();
	return 0;
}

int audio_mic_record(const struct shell *sh, uint32_t seconds)
{
	int r;

	seconds = MIN(seconds, REC_SECONDS_MAX);
	r = mic_start();
	if (r != 0) {
		shell_error(sh, "mic start: %d", r);
		return r;
	}
	rec_len = 0;
	shell_print(sh, "recording %u s ...", seconds);
	while (rec_len + BLOCK_SAMPLES <= seconds * RATE) {
		void *blk;
		size_t sz;

		r = dmic_read(mic, 0, &blk, &sz, 200);
		if (r != 0) {
			shell_error(sh, "dmic_read: %d", r);
			break;
		}
		memcpy(&rec_buf[rec_len], blk, sz);
		rec_len += sz / 2;
		k_mem_slab_free(&mic_slab, blk);
	}
	mic_stop();
	shell_print(sh, "recorded %u samples (%u.%02u s), peak %d", rec_len, rec_len / RATE,
		    (rec_len % RATE) * 100 / RATE, 0);
	return r;
}

/* ---- playback ----------------------------------------------------------- */

static void fill_recording(int16_t *pcm, size_t n, void *user)
{
	ARG_UNUSED(user);
	for (size_t i = 0; i < n; i++) {
		pcm[i] = (play_pos < rec_len) ? makeup(rec_buf[play_pos++]) : 0;
	}
}

static int play_run(const struct shell *sh, ipro_dac_stream_fill_t fill, void *user, uint32_t ms)
{
	struct ipro_dac_stream_cfg cfg = {
		.dma = dma, .dma_channel = 1, .sample_rate = RATE, .seg_samples = DAC_SEG,
		.fill = fill, .user = user,
	};
	int r = ipro_dac_stream_start(&cfg);

	if (r != 0) {
		shell_error(sh, "dac stream start: %d", r);
		return r;
	}
	(void)spk_enable(true);
	k_msleep(ms);
	(void)spk_enable(false);
	ipro_dac_stream_stop();
	shell_print(sh, "played %u DMA segments (%u ms)", ipro_dac_stream_segments(),
		    ipro_dac_stream_segments() * DAC_SEG / (RATE / 1000));
	return 0;
}

int audio_play_recording(const struct shell *sh)
{
	if (rec_len == 0) {
		shell_error(sh, "nothing recorded");
		return -ENODATA;
	}
	play_pos = 0;
	hp_prev_in = 0; hp_prev_out = 0;
	shell_print(sh, "playing %u samples (x%d make-up gain, DC removed)", rec_len, 1 << makeup_shift);
	return play_run(sh, fill_recording, NULL, rec_len * 1000 / RATE + 60);
}

struct tone {
	uint32_t hz;
	uint32_t phase; /* Q16 turns */
};

static void fill_tone(int16_t *pcm, size_t n, void *user)
{
	struct tone *t = user;
	uint32_t step = (uint32_t)(((uint64_t)t->hz << 16) / RATE);

	for (size_t i = 0; i < n; i++) {
		double a = (double)(t->phase & 0xFFFF) / 65536.0 * 6.283185307;

		pcm[i] = (int16_t)(sin(a) * 12000.0);
		t->phase += step;
	}
}

int audio_tone(const struct shell *sh, uint32_t hz, uint32_t ms)
{
	struct tone t = {.hz = hz, .phase = 0};

	shell_print(sh, "tone %u Hz for %u ms", hz, ms);
	return play_run(sh, fill_tone, &t, ms);
}

/* ---- live loopback ------------------------------------------------------ */

static void fill_loop(int16_t *pcm, size_t n, void *user)
{
	ARG_UNUSED(user);
	uint32_t got = ring_buf_get(&loop_ring, (uint8_t *)pcm, n * 2);

	if (got < n * 2) {
		memset((uint8_t *)pcm + got, 0, n * 2 - got);
	}
	for (size_t i = 0; i < n; i++) {
		pcm[i] = makeup(pcm[i]);
	}
}

int audio_loopback(const struct shell *sh, uint32_t seconds)
{
	struct ipro_dac_stream_cfg cfg = {
		.dma = dma, .dma_channel = 1, .sample_rate = RATE, .seg_samples = DAC_SEG,
		.fill = fill_loop,
	};
	int64_t end = k_uptime_get() + (int64_t)seconds * 1000;
	int r = mic_start();
	uint32_t drops = 0;

	if (r != 0) {
		shell_error(sh, "mic start: %d", r);
		return r;
	}
	ring_buf_reset(&loop_ring);
	/* Prime ~40 ms so the DAC never runs dry on the mic's slower real rate. */
	{
		uint8_t z[BLOCK_BYTES] = {0};
		ring_buf_put(&loop_ring, z, sizeof(z));
		ring_buf_put(&loop_ring, z, sizeof(z));
	}
	r = ipro_dac_stream_start(&cfg);
	if (r != 0) {
		shell_error(sh, "dac stream start: %d", r);
		mic_stop();
		return r;
	}
	(void)spk_enable(true);
	loop_active = true;
	shell_print(sh, "mic -> speaker for %u s (speak into the mic)", seconds);
	while (k_uptime_get() < end) {
		void *blk;
		size_t sz;

		if (dmic_read(mic, 0, &blk, &sz, 200) != 0) {
			continue;
		}
		if (ring_buf_put(&loop_ring, blk, sz) < sz) {
			drops++;
		}
		k_mem_slab_free(&mic_slab, blk);
	}
	loop_active = false;
	(void)spk_enable(false);
	ipro_dac_stream_stop();
	mic_stop();
	shell_print(sh, "loopback done, %u DMA segments, %u dropped mic blocks",
		    ipro_dac_stream_segments(), drops);
	return 0;
}

int audio_set_gain_shift(int shift)
{
	if (shift < 0 || shift > 5) {
		return -EINVAL;
	}
	makeup_shift = shift;
	return 0;
}
