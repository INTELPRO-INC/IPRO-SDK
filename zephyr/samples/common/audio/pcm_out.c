/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * See pcm_out.h. DAC channel A (GPIO23) through ipro_dac_stream (hardware
 * DMA ping-pong), class-D enable on GPIO22, which is also DAC channel B's
 * pad: it is re-muxed as a GPIO output after every GPDAC (re)initialisation.
 *
 * Measured on the EVK: the GPDAC's 16/32 kHz are within 0.1 % of the LC3
 * source's clock (not the 15873 Hz the AUADC runs at); the servo is there
 * for the residual drift, +-1 % of trim around in_rate/out_rate.
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/dac.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <string.h>
#include <ipro_zephyr/ipro_dac_stream.h>
#include "pcm_out.h"

LOG_MODULE_REGISTER(pcm_out, CONFIG_LOG_DEFAULT_LEVEL);

#define SEG_SAMPLES 160
#define RB_BYTES 16384
/* Ring level the servo aims at: 100 ms at the input rate, set at start. */
#define STEP_Q16_TRIM_MAX(step) ((step) / 100)

static const struct device *const dma = DEVICE_DT_GET(DT_NODELABEL(dma0));
static const struct device *const dac = DEVICE_DT_GET(DT_NODELABEL(dac0));
static const struct gpio_dt_spec spk_en = GPIO_DT_SPEC_GET(DT_NODELABEL(spk_enable), gpios);
RING_BUF_DECLARE(pcm_rb, RB_BYTES);

static struct {
	bool dac_ready, running;
	uint32_t in_rate, out_rate;
	int32_t target_samples;
	int32_t step_nominal, step_q16;
	uint32_t phase_q16;
	int16_t rs_prev, rs_cur;
	uint16_t gain_q8; /* 256 = unity */
	uint32_t frames, underruns, overruns;
	int32_t in_peak, out_peak;
} s = {.gain_q8 = 256};

static bool rs_next(void)
{
	int16_t v;

	if (ring_buf_get(&pcm_rb, (uint8_t *)&v, 2) != 2) {
		return false;
	}
	s.rs_prev = s.rs_cur;
	s.rs_cur = v;
	return true;
}

static void fill(int16_t *pcm, size_t n, void *user)
{
	ARG_UNUSED(user);
	int32_t level = (int32_t)ring_buf_size_get(&pcm_rb) / 2 - s.target_samples;
	int32_t trim = STEP_Q16_TRIM_MAX(s.step_nominal);

	s.step_q16 = s.step_nominal + CLAMP(level / 2, -trim, trim);
	for (size_t i = 0; i < n; i++) {
		while (s.phase_q16 >= 0x10000U) {
			if (!rs_next()) {
				s.underruns++;
				memset(&pcm[i], 0, (n - i) * 2);
				return;
			}
			s.phase_q16 -= 0x10000U;
		}
		int32_t v = s.rs_prev +
			    (((int32_t)(s.rs_cur - s.rs_prev) * (int32_t)s.phase_q16) >> 16);

		s.out_peak = MAX(s.out_peak, abs((int)v));
		pcm[i] = (int16_t)v;
		s.phase_q16 += (uint32_t)s.step_q16;
	}
}

int pcm_out_init(void)
{
	struct dac_channel_cfg ch = {.channel_id = 0, .resolution = 10};

	if (!device_is_ready(dma) || !device_is_ready(dac) || !gpio_is_ready_dt(&spk_en)) {
		return -ENODEV;
	}
	if (!s.dac_ready) {
		int r = dac_channel_setup(dac, &ch);

		if (r != 0) {
			return r;
		}
		(void)dac_write_value(dac, 0, 512);
		(void)gpio_pin_configure_dt(&spk_en, GPIO_OUTPUT_INACTIVE);
		s.dac_ready = true;
	}
	return 0;
}

int pcm_out_start(uint32_t in_rate)
{
	struct ipro_dac_stream_cfg cfg = {
		.dma = dma, .dma_channel = 1, .seg_samples = SEG_SAMPLES, .fill = fill,
	};
	int r = pcm_out_init();

	if (r != 0) {
		return r;
	}
	if (s.running) {
		return s.in_rate == in_rate ? 0 : -EBUSY;
	}
	if (in_rate < 8000 || in_rate > 48000) {
		return -ENOTSUP;
	}
	s.in_rate = in_rate;
	s.out_rate = in_rate <= 16000 ? 16000 : 32000;
	s.step_nominal = (int32_t)(((uint64_t)in_rate << 16) / s.out_rate);
	s.step_q16 = s.step_nominal;
	s.target_samples = in_rate / 10; /* 100 ms */
	s.phase_q16 = 0x10000U;
	s.rs_prev = 0; s.rs_cur = 0;
	s.frames = 0; s.underruns = 0; s.overruns = 0;
	ring_buf_reset(&pcm_rb);
	{
		/* prefill: silence up to the target so the ring never runs dry
		 * while the decoder's first frames arrive */
		static const uint8_t z[512];
		size_t left = s.target_samples * 2;

		while (left > 0) {
			size_t chunk = MIN(left, sizeof(z));

			ring_buf_put(&pcm_rb, z, chunk);
			left -= chunk;
		}
	}
	cfg.sample_rate = s.out_rate;
	r = ipro_dac_stream_start(&cfg);
	if (r != 0) {
		LOG_ERR("dac stream: %d", r);
		return r;
	}
	(void)gpio_pin_configure_dt(&spk_en, GPIO_OUTPUT_ACTIVE);
	s.running = true;
	LOG_INF("speaker on: %u Hz in, GPDAC %u Hz, step %d/65536", s.in_rate, s.out_rate,
		s.step_nominal);
	return 0;
}

int pcm_out_write(const int16_t *pcm, size_t n)
{
	if (!s.running) {
		return -EAGAIN;
	}
	for (size_t i = 0; i < n; i++) {
		int32_t v = ((int32_t)pcm[i] * s.gain_q8) >> 8;

		s.in_peak = MAX(s.in_peak, abs((int)pcm[i]));
		int16_t o = (int16_t)CLAMP(v, INT16_MIN, INT16_MAX);

		if (ring_buf_put(&pcm_rb, (const uint8_t *)&o, 2) != 2) {
			s.overruns += n - i;
			break;
		}
	}
	s.frames++;
	return 0;
}

void pcm_out_set_volume(uint8_t volume, bool mute)
{
	/* VCP 0..255 -> linear amplitude, 255 = unity, 0 = off */
	uint32_t g = mute ? 0 : ((uint32_t)volume * 256U) / 255U;

	s.gain_q8 = (uint16_t)g;
}

void pcm_out_stop(void)
{
	if (s.running) {
		(void)gpio_pin_configure_dt(&spk_en, GPIO_OUTPUT_INACTIVE);
		ipro_dac_stream_stop();
		s.running = false;
	}
}

bool pcm_out_running(void)
{
	return s.running;
}

uint32_t pcm_out_rate(void)
{
	return s.in_rate;
}

void pcm_out_stats(struct pcm_out_stats *st)
{
	*st = (struct pcm_out_stats){
		.frames = s.frames, .underruns = s.underruns, .overruns = s.overruns,
		.ring_bytes = ring_buf_size_get(&pcm_rb),
		.dma_segments = ipro_dac_stream_segments(),
		.refills = ipro_dac_stream_refills(),
		.step_q16 = s.step_q16, .in_peak = s.in_peak, .out_peak = s.out_peak,
	};
	s.in_peak = 0; s.out_peak = 0;
}

#ifdef CONFIG_SHELL
#include <zephyr/shell/shell.h>

static int cmd_pcm_status(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc); ARG_UNUSED(argv);
	struct pcm_out_stats st;

	pcm_out_stats(&st);
	shell_print(sh, "running %d, %u Hz in -> %u Hz DAC, frames %u, ring %u/%u B", s.running,
		    s.in_rate, s.out_rate, st.frames, st.ring_bytes, RB_BYTES);
	shell_print(sh, "underruns %u, overruns %u, segments %u, refills %u, step %d, peak in %d out %d, gain %u/256, spk_en %d",
		    st.underruns, st.overruns, st.dma_segments, st.refills, st.step_q16,
		    st.in_peak, st.out_peak, s.gain_q8, gpio_pin_get_dt(&spk_en));
	return 0;
}

static uint32_t tone_phase;
static void tone_fill(int16_t *pcm, size_t n, void *user)
{
	ARG_UNUSED(user);
	static const int16_t tbl[16] = {0, 4600, 8500, 11100, 12000, 11100, 8500, 4600,
					0, -4600, -8500, -11100, -12000, -11100, -8500, -4600};

	for (size_t i = 0; i < n; i++) {
		pcm[i] = tbl[tone_phase++ & 15];
	}
}

static int cmd_pcm_tone(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t ms = argc > 1 ? strtoul(argv[1], NULL, 0) : 1000;
	struct ipro_dac_stream_cfg cfg = {
		.dma = dma, .dma_channel = 1, .sample_rate = 16000, .seg_samples = SEG_SAMPLES,
		.fill = tone_fill,
	};

	if (s.running) {
		shell_print(sh, "stream running; stop it first");
		return -EBUSY;
	}
	(void)pcm_out_init();
	int r = ipro_dac_stream_start(&cfg);

	(void)gpio_pin_configure_dt(&spk_en, GPIO_OUTPUT_ACTIVE);
	shell_print(sh, "1 kHz tone for %u ms: start %d", ms, r);
	k_msleep(ms);
	ipro_dac_stream_stop();
	(void)gpio_pin_configure_dt(&spk_en, GPIO_OUTPUT_INACTIVE);
	return 0;
}

static int cmd_pcm_vol(const struct shell *sh, size_t argc, char **argv)
{
	uint32_t v = strtoul(argv[1], NULL, 0);

	pcm_out_set_volume(MIN(v, 255), false);
	shell_print(sh, "volume %u -> gain %u/256", v, s.gain_q8);
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_pcm,
	SHELL_CMD(status, NULL, "stream statistics", cmd_pcm_status),
	SHELL_CMD_ARG(tone, NULL, "tone [ms] - local 1 kHz on the speaker", cmd_pcm_tone, 1, 1),
	SHELL_CMD_ARG(vol, NULL, "vol <0-255>", cmd_pcm_vol, 2, 0),
	SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(pcm, &sub_pcm, "DAC speaker output", NULL);
#endif
