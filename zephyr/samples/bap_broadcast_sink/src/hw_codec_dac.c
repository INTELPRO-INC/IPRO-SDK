/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * hw_codec.h (samples/bluetooth/bap_broadcast_sink) over the IPRO7 GPDAC
 * stream: decoded LC3 frames (mono, 16-bit) go through a ring buffer into
 * the DMA-fed DAC on channel A (GPIO23), the class-D enable on GPIO22.
 *
 * The sample hands over sizeof(lc3_rx_buf) bytes per frame whatever the
 * sample rate; only the first samplerate x 10 ms samples are the frame.
 * 48 kHz streams are decimated by 3 (the GPDAC offers 8/16/32 kHz). The
 * source's and the GPDAC's clocks are not the same clock (measured on the
 * EVK: the DAC runs within 0.1 % of the LC3 rate, NOT the 15873 Hz the
 * AUADC shows): the DMA refill resamples (linear interpolation) with a
 * step servoed on the ring-buffer level, so the two never run apart and
 * no sample is ever dropped or repeated (a dropped sample every 8 ms was
 * an audible 127 Hz buzz).
 *
 * One BIS only: the sample forwards channel 0 of every synced stream
 * through one buffer, so prj.conf syncs to the FRONT_LEFT BIS alone
 * (CONFIG_TARGET_BROADCAST_CHANNEL=1).
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/dac.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/ring_buffer.h>
#include <ipro_zephyr/ipro_dac_stream.h>
#include UPSTREAM_HW_CODEC_H

LOG_MODULE_REGISTER(hw_codec_dac, CONFIG_LOG_DEFAULT_LEVEL);

#define FRAME_US 10000U
#define SEG_SAMPLES 160
#define RB_BYTES 8192
/* Ring-buffer level the resampler servo aims at: 100 ms of audio. */
#define PREFILL_SAMPLES 1600
/* Q16 resampler step: 1:1 nominal, the servo trims it by up to +-1 %. */
#define STEP_Q16_NOMINAL 65536
#define STEP_Q16_TRIM_MAX 655

static const struct device *const dma = DEVICE_DT_GET(DT_NODELABEL(dma0));
static const struct device *const dac = DEVICE_DT_GET(DT_NODELABEL(dac0));
static const struct gpio_dt_spec spk_en = GPIO_DT_SPEC_GET(DT_NODELABEL(spk_enable), gpios);
RING_BUF_DECLARE(pcm_rb, RB_BYTES);
static uint32_t in_rate, out_rate, decim = 1;
static uint32_t underruns, frames;
static bool running;
static int32_t in_peak, out_peak;
static int16_t in_snap[8], out_snap[8];
static uint32_t dropped_bytes;
static int32_t step_q16 = STEP_Q16_NOMINAL;
static uint32_t phase_q16; /* position between rs_prev and rs_cur */
static int16_t rs_prev, rs_cur;

/* One more source sample; false on an empty ring (output holds the last). */
static bool rs_next(void)
{
	int16_t s;

	if (ring_buf_get(&pcm_rb, (uint8_t *)&s, 2) != 2) {
		return false;
	}
	rs_prev = rs_cur;
	rs_cur = s;
	return true;
}

static void fill(int16_t *pcm, size_t n, void *user)
{
	ARG_UNUSED(user);
	int32_t level = (int32_t)ring_buf_size_get(&pcm_rb) / 2 - PREFILL_SAMPLES;

	/* Consume a little faster when the ring is above target, slower below. */
	step_q16 = STEP_Q16_NOMINAL + CLAMP(level / 2, -STEP_Q16_TRIM_MAX, STEP_Q16_TRIM_MAX);

	for (size_t i = 0; i < n; i++) {
		while (phase_q16 >= 0x10000U) {
			if (!rs_next()) {
				underruns++;
				memset(&pcm[i], 0, (n - i) * 2);
				memcpy(out_snap, pcm, sizeof(out_snap));
				return;
			}
			phase_q16 -= 0x10000U;
		}
		int32_t s = rs_prev + (((int32_t)(rs_cur - rs_prev) * (int32_t)phase_q16) >> 16);

		out_peak = MAX(out_peak, abs((int)s));
		pcm[i] = (int16_t)s;
		phase_q16 += (uint32_t)step_q16;
	}
	memcpy(out_snap, pcm, sizeof(out_snap));
}

int hw_codec_open(void)
{
	struct dac_channel_cfg ch = {.channel_id = 0, .resolution = 10};

	if (!device_is_ready(dma) || !device_is_ready(dac) || !gpio_is_ready_dt(&spk_en)) {
		return -ENODEV;
	}
	/*
	 * The sample opens the codec once per stream (twice for a 2-BIS
	 * broadcast), the second time while the first stream already plays.
	 * dac_channel_setup() re-initialises the GPDAC onto the register
	 * source, which silences a running DMA stream: do the one-time pad /
	 * channel setup only once and never while running.
	 */
	static bool dac_ready;

	if (!dac_ready) {
		(void)dac_channel_setup(dac, &ch);
		(void)dac_write_value(dac, 0, 512);
		(void)gpio_pin_configure_dt(&spk_en, GPIO_OUTPUT_INACTIVE);
		dac_ready = true;
	}
	return 0;
}

int hw_codec_cfg(uint32_t samplerate)
{
	struct ipro_dac_stream_cfg cfg = {
		.dma = dma, .dma_channel = 1, .seg_samples = SEG_SAMPLES, .fill = fill,
	};

	if (running) {
		return 0;
	}
	switch (samplerate) {
	case 8000: case 16000: case 32000:
		out_rate = samplerate; decim = 1; break;
	case 48000:
		out_rate = 16000; decim = 3; break;
	case 24000:
		out_rate = 8000; decim = 3; break;
	default:
		LOG_ERR("sample rate %u not supported", samplerate);
		return -ENOTSUP;
	}
	in_rate = samplerate;
	cfg.sample_rate = out_rate;
	ring_buf_reset(&pcm_rb);
	underruns = 0; frames = 0; dropped_bytes = 0;
	phase_q16 = 0x10000U; rs_prev = 0; rs_cur = 0;
	{
		/* 100 ms of silence: latency the decoder thread may be late by
		 * before the DMA runs dry (its scheduling is bursty under load). */
		static const uint8_t z[PREFILL_SAMPLES * 2];

		ring_buf_put(&pcm_rb, z, sizeof(z));
	}
	int r = ipro_dac_stream_start(&cfg);

	if (r != 0) {
		LOG_ERR("dac stream: %d", r);
		return r;
	}
	/* GPIO22 doubles as DAC channel B: the GPDAC (re)init above puts the
	 * pad back on the analogue function, so re-mux it as a GPIO output. */
	(void)gpio_pin_configure_dt(&spk_en, GPIO_OUTPUT_ACTIVE);
	running = true;
	LOG_INF("speaker on: %u Hz in, GPDAC %u Hz (decimate %u)", in_rate, out_rate, decim);
	return 0;
}

uint32_t hw_codec_write_data(const uint8_t *data, uint32_t len)
{
	const int16_t *pcm = (const int16_t *)data;
	uint32_t n = MIN(len / 2, in_rate * FRAME_US / USEC_PER_SEC);

	if (!running) {
		return 0;
	}
	for (uint32_t i = 0; i < n; i++) {
		in_peak = MAX(in_peak, abs((int)pcm[i]));
	}
	memcpy(in_snap, pcm, sizeof(in_snap));
	for (uint32_t i = 0; i + decim <= n; i += decim) {
		int32_t acc = 0;

		for (uint32_t k = 0; k < decim; k++) {
			acc += pcm[i + k];
		}
		int16_t s = (int16_t)(acc / (int32_t)decim);

		if (ring_buf_put(&pcm_rb, (const uint8_t *)&s, 2) != 2) {
			dropped_bytes += (n - i) * 2;
			break; /* full: the DAC is behind, drop the rest of this frame */
		}
	}
	if ((++frames % 500) == 0) {
		LOG_INF("%u frames, ring %u/%u B, underruns %u, dropped %u B, DMA segments %u, peak in %d out %d, spk_en %d",
			frames, ring_buf_size_get(&pcm_rb), RB_BYTES, underruns, dropped_bytes,
			ipro_dac_stream_segments(), in_peak, out_peak, gpio_pin_get_dt(&spk_en));
		LOG_INF("refills %u, resampler step %d/65536", ipro_dac_stream_refills(), step_q16);
		LOG_INF(" in: %d %d %d %d %d %d %d %d", in_snap[0], in_snap[1], in_snap[2], in_snap[3],
			in_snap[4], in_snap[5], in_snap[6], in_snap[7]);
		LOG_INF("out: %d %d %d %d %d %d %d %d", out_snap[0], out_snap[1], out_snap[2],
			out_snap[3], out_snap[4], out_snap[5], out_snap[6], out_snap[7]);
		in_peak = 0; out_peak = 0;
	}
	return 0; /* the sample treats non-zero as an error */
}

int hw_codec_close(void)
{
	if (running) {
		(void)gpio_pin_configure_dt(&spk_en, GPIO_OUTPUT_INACTIVE);
		ipro_dac_stream_stop();
		running = false;
	}
	return 0;
}

/* ---- bring-up shell: "codec status", "codec spk on|off" ---------------- */
#ifdef CONFIG_SHELL
#include <zephyr/shell/shell.h>

static int cmd_codec_status(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	/* GLB_GPIO_CFGCTL11 holds pads 22 (low half) and 23 (high half). */
	uint32_t cfg = sys_read32(0x30000000 + 0x100 + (22 / 2) * 4);
	uint32_t out = sys_read32(0x30000000 + 0x188); /* CFGCTL33: output values 0-31 */
	uint32_t oe = sys_read32(0x30000000 + 0x190);  /* CFGCTL35: output enables 0-31 */
	uint32_t in = sys_read32(0x30000000 + 0x180);  /* CFGCTL32: input values 0-31 */

	shell_print(sh, "GPIO22 cfg 0x%04x (ie %u pu %u func %u) | GPIO23 cfg 0x%04x (func %u)",
		    cfg & 0xFFFF, cfg & 1, (cfg >> 4) & 1, (cfg >> 8) & 0x1F, cfg >> 16,
		    (cfg >> 24) & 0x1F);
	shell_print(sh, "GPIO22 oe %u out %u in %u | gpio_pin_get %d", (oe >> 22) & 1, (out >> 22) & 1,
		    (in >> 22) & 1, gpio_pin_get_dt(&spk_en));
	shell_print(sh, "running %d, frames %u, ring %u B, underruns %u, segments %u", running, frames,
		    ring_buf_size_get(&pcm_rb), underruns, ipro_dac_stream_segments());
	return 0;
}

static int cmd_codec_spk(const struct shell *sh, size_t argc, char **argv)
{
	bool on = argc < 2 || strcmp(argv[1], "off") != 0;
	int r = gpio_pin_configure_dt(&spk_en, on ? GPIO_OUTPUT_ACTIVE : GPIO_OUTPUT_INACTIVE);

	shell_print(sh, "spk %s: %d", on ? "on" : "off", r);
	return cmd_codec_status(sh, 0, NULL);
}

static uint32_t tone_phase;
static void tone_fill(int16_t *pcm, size_t n, void *user)
{
	ARG_UNUSED(user);
	for (size_t i = 0; i < n; i++) {
		/* 1 kHz at 16 kHz: 16 samples per period, triangle-ish via sine table of 16 */
		static const int16_t tbl[16] = {0, 4600, 8500, 11100, 12000, 11100, 8500, 4600,
						0, -4600, -8500, -11100, -12000, -11100, -8500, -4600};
		pcm[i] = tbl[tone_phase++ & 15];
	}
}

static int cmd_codec_tone(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t ms = argc > 1 ? strtoul(argv[1], NULL, 0) : 1000;
	struct ipro_dac_stream_cfg cfg = {
		.dma = dma, .dma_channel = 1, .sample_rate = 16000, .seg_samples = SEG_SAMPLES,
		.fill = tone_fill,
	};
	bool was_running = running;

	if (was_running) {
		ipro_dac_stream_stop();
		running = false;
	}
	int r = ipro_dac_stream_start(&cfg);

	(void)gpio_pin_configure_dt(&spk_en, GPIO_OUTPUT_ACTIVE);
	shell_print(sh, "local 1 kHz tone for %u ms: start %d", ms, r);
	k_msleep(ms);
	ipro_dac_stream_stop();
	shell_print(sh, "tone done, %u segments; BT stream %s", ipro_dac_stream_segments(),
		    was_running ? "was running - resync needed (reset the board)" : "idle");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_codec,
	SHELL_CMD_ARG(tone, NULL, "tone [ms] - local 1 kHz on the speaker (bypasses BT)", cmd_codec_tone, 1, 1),
	SHELL_CMD(status, NULL, "GPIO22/23 pad state, stream stats", cmd_codec_status),
	SHELL_CMD_ARG(spk, NULL, "spk on|off", cmd_codec_spk, 1, 1),
	SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(codec, &sub_codec, "DAC speaker output", NULL);
#endif
