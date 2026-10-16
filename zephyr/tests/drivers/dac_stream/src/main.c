/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * GPDAC DMA stream (ipro_dac_stream.h): a 1 kHz sine for 500 ms at 16 kHz.
 * The DMA segment counter must advance at the sample rate and the ADC's
 * internal tap of DAC channel A (channel 12) must see the swing.
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/dac.h>
#include <ipro_zephyr/ipro_dac_stream.h>
#include <math.h>

static const struct device *const adc = DEVICE_DT_GET(DT_NODELABEL(adc0));
static const struct device *const dac = DEVICE_DT_GET(DT_NODELABEL(dac0));
static const struct device *const dma = DEVICE_DT_GET(DT_NODELABEL(dma0));

#define RATE 16000
#define SEG  320
#define CH_DACA 12

static uint32_t phase;
static uint32_t fills;

static void fill(int16_t *pcm, size_t n, void *user)
{
	ARG_UNUSED(user);
	fills++;
	for (size_t i = 0; i < n; i++) {
		pcm[i] = (int16_t)(sin((double)phase / RATE * 2.0 * 3.14159265 * 1000.0) * 14000.0);
		phase++;
	}
}

static uint16_t adc_daca(void)
{
	uint16_t v = 0;
	struct adc_sequence seq = {
		.channels = BIT(CH_DACA), .buffer = &v, .buffer_size = sizeof(v), .resolution = 12,
	};
	zassert_ok(adc_read(adc, &seq));
	return v;
}

ZTEST(ipro_dac_stream, test_tone_runs)
{
	struct adc_channel_cfg ach = {
		.gain = ADC_GAIN_1, .reference = ADC_REF_INTERNAL,
		.acquisition_time = ADC_ACQ_TIME_DEFAULT, .channel_id = CH_DACA,
	};
	struct dac_channel_cfg dch = {.channel_id = 0, .resolution = 10, .internal = true};
	struct ipro_dac_stream_cfg cfg = {
		.dma = dma, .dma_channel = 1, .sample_rate = RATE, .seg_samples = SEG, .fill = fill,
	};
	uint16_t lo = 0xFFFF, hi = 0;

	zassert_true(device_is_ready(adc) && device_is_ready(dac) && device_is_ready(dma));
	zassert_ok(adc_channel_setup(adc, &ach));
	zassert_ok(dac_channel_setup(dac, &dch));
	zassert_equal(ipro_dac_stream_start(&(struct ipro_dac_stream_cfg){.dma = dma, .fill = fill,
		      .seg_samples = SEG, .sample_rate = 44100}), -ENOTSUP);

	zassert_ok(ipro_dac_stream_start(&cfg));
	for (int i = 0; i < 200; i++) {
		uint16_t v = adc_daca();

		lo = MIN(lo, v);
		hi = MAX(hi, v);
		k_busy_wait(137);
	}
	k_msleep(500);
	uint32_t segs = ipro_dac_stream_segments();

	ipro_dac_stream_stop();
	printk("segments %u (expect ~%u), fills %u, ADC ch12 swing %u..%u\n", segs,
	       (uint32_t)(530 * RATE / 1000 / SEG), fills, lo, hi);
	zassert_true(segs >= 20 && segs <= 32, "DMA did not run at the sample rate: %u", segs);
	zassert_true(fills >= segs, "refills lagged: %u fills for %u segments", fills, segs);
	zassert_true(hi - lo > 300, "no swing on DAC A: %u..%u", lo, hi);
	zassert_equal(ipro_dac_stream_segments(), segs, "still running after stop");
}

ZTEST_SUITE(ipro_dac_stream, NULL, NULL, NULL, NULL, NULL);
